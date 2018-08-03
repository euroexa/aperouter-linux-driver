/*
  This file is part of the APErouter kernel driver.
  Copyright (C) 2017 INFN APE Lab.


  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <linux/bitops.h>
#include <linux/dmapool.h>
#include <linux/dma/xilinx_dma.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/of_address.h>
#include <linux/of_dma.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/clk.h>
#include <linux/io-64-nonatomic-lo-hi.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/wm97xx.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/ioctl.h>

#include <linux/delay.h>

#include "karma.h"
#include "karma_trace.h"
#include "karma_instr.h"
#include "karma_mmio.h"
#include "karma_regs.h"
#include "karma_interface.h"




static int karma_addr = 0; 
static int karma_topo = 0; 

module_param(karma_addr, int, 0);
module_param(karma_topo, int, 0);

MODULE_PARM_DESC(karma_addr, "APErouter local address X-Y-Z");
MODULE_PARM_DESC(karma_topo, "APErouter topology  X[]Y[]Z[]");


static const struct of_device_id karma_of_ids[] = {
 	{ .compatible = "xlnx,ExaNIP-0.99" },
 	{ .compatible = "xlnx,ExaNIP-0.999" },
	{}
};


static ssize_t karma_read(struct file* f, char *buf, size_t size, loff_t *f_pos){

	PERROR("karma read\n");
	return 0;
}

static ssize_t karma_write(struct file* F, const char *buf, size_t count, loff_t *f_pos){
	PERROR("karma write\n");
	return -EPERM;
}

static int karma_open(struct inode *inode, struct file *file){

	karma_dev_t *xdev = container_of(inode->i_cdev, karma_dev_t, cdev);
	file->private_data = xdev; 
	PTRACE("karma open\n");

	return 0;
}

static int karma_close(struct inode *inode, struct file *file){
	PTRACE("karma close\n");
	return 0;
}

static long karma_ioctl(struct file *f, unsigned int cmd, unsigned long arg){
	karma_dev_t *xdev = f->private_data;
	u32 reg;
	u32 value;

	switch(cmd){
		case KARMA_IOGETSELFADDR:
			PTRACE("%s: IOGETSELFADDR addr=%p\n", __FUNCTION__, xdev);
			if(put_user(xdev->addr, (u32 *)arg))
				goto err_inv_ioarg;

			break;

		case KARMA_IOSETREG:
			PTRACE("\t SET_REG\n");
			if ( get_user(reg, (u32*)arg) )
				goto err_inv_ioarg;
			if ( get_user(value, ((u32*)arg)+1) )
				goto err_inv_ioarg;

  			karma_reg_write(xdev, reg,value);

			break;

		case KARMA_IOGETREG:
			if(get_user(reg, ((u32*)arg)))
				goto err_inv_ioarg;

  			value = karma_reg_read(xdev, reg);

			PTRACE("\tGET_REG%d %x\n",reg,value);
			if(put_user(value, (u32*)arg))
				goto err_inv_ioarg;

			break;
			default:
			PERRORC(10,"Unknow ioctl number!\n");
	}
	return 0;

err_inv_ioarg:
	PERRORC(10,"invalid parameter for IOCTL\n");
	return -EFAULT;
}


static struct file_operations FileOps = {
	.owner                = THIS_MODULE,
	.open                 = karma_open,
	.read                 = karma_read,
	.write                = karma_write,
	.release              = karma_close,
	.unlocked_ioctl        = karma_ioctl,
};



/** devTree2Res - associate the rosources to the fifos
 * @pdev: Pointer to the platform_device structure
 * @np: Pointer to the Device Tree node
 *
 *
 * returns:	NULL if invalid parameter is sent or error on resources
 *
 */

static void __iomem *devTree2Res(struct platform_device *pdev, struct device_node *np, int index){
	int err;
	u32 addr;
	struct resource *io;
	void __iomem *ret;

	if(!pdev || !np){
		PERROR("Invalid parameter\n");
		goto error_out;
	}

	/* Find index in the device tree */
	err =  of_property_read_u32_index(np,"reg",index,&addr);
	if (err < 0) {
		PERROR("No match found for index %d\n", index);
		goto error_out;
	}

	/* Get the resource pointer*/
	io = platform_get_resource(pdev, IORESOURCE_MEM, index);
	if(io == NULL){
		PERROR("Error on getting the resource %d\n", index);
		goto error_out;
	}


	/* Resource remapping*/
	ret = devm_ioremap_resource(&pdev->dev, io); 

	if(ret==NULL){
		PERROR("Error on remapping the resource %d\n", index);
		goto error_out;
	}
	
	PTRACE("index %d regs start address 0x%08llx end address 0x%08llx size= %lld REMAPPED TO: %p\n",index,io->start,io->end,io->end-io->start,ret);


	return ret;

error_out:
	return NULL;
}

/**
 * karma_probe - Driver probe function
 * @pdev: Pointer to the platform_device structure
 *
 * Return: '0' on success and failure value on error
 */
static int karma_probe(struct platform_device *pdev){
	struct karma_dev_t *xdev;
	struct device_node *np = pdev->dev.of_node;
	int err=0;
	dev_t devt;

	/* Allocate and initialize the DMA engine structure */
	xdev = devm_kzalloc(&pdev->dev, sizeof(*xdev), GFP_KERNEL);
	if (!xdev)
		return -ENOMEM;

	xdev->dev = &pdev->dev;
	xdev->pdev = pdev;

	/* Find the memory-mapped fifos */
	if((xdev->mm_tgt_ctrl = devTree2Res(pdev, np, 0)) == NULL)
		goto error_out;

	/* Allocation of the chrdev region*/
	err = alloc_chrdev_region( &devt, 0, KARMA_DEVICES, "karmareg" );
	if( err < 0 ){
		PERROR( "FIFO Device Registration failed\n" );
	}
	PTRACE("Allocated region for %d... \n",devt);
	xdev->devt = devt;


	/* Create the sysfs class */
	if ( (xdev->cl = class_create( THIS_MODULE, "karmaclass") ) == NULL ){
		PERROR( "FIFO class creation failed\n" );
		unregister_chrdev_region( xdev->devt, KARMA_DEVICES );
		goto error_out;
	}
	PTRACE("Class created %p\n",xdev->cl);


	/* Create the /dev device*/
	if( device_create( xdev->cl, NULL, devt,NULL, "aperouter" ) == NULL ){
		PERROR( "FIFO device creation failed\n" );
		class_destroy( xdev->cl );
		unregister_chrdev_region( xdev->devt, KARMA_DEVICES );
		goto error_out;
	}
	PTRACE("Created /dev entry 'APErouter' [%p]...\n",xdev);

	cdev_init( &xdev->cdev, &FileOps );
	if( cdev_add( &xdev->cdev, devt, KARMA_DEVICES ) < 0 ){
		PERROR( "APErouter device addition failed\n" );
		unregister_chrdev_region( xdev->devt, KARMA_DEVICES );
		device_destroy( xdev->cl, xdev->devt );
		class_destroy( xdev->cl );
		goto error_out;
	}
	PTRACE("Associated FOps\n");


	PERROR("APErouter device found! happyness. [%p]\n",xdev);
	xdev->addr = karma_addr;
	xdev->abs_tor = (karma_addr >> 14) & 0xff;
	xdev->abs_t = (karma_addr >> 12 ) & 0x3;
	xdev->abs_x = (karma_addr >> 0 ) & 0xf;
	xdev->abs_y = (karma_addr >> 4 ) & 0xf;
	xdev->abs_z = (karma_addr >> 8) & 0xf;
	PERROR("My network address is 0x%08x (%d|%d,%d,%d)\n", xdev->addr, xdev->abs_t, xdev->abs_z, xdev->abs_y, xdev->abs_x);
	xdev->topo = karma_topo;
	xdev->abs_n_proc_x = (karma_topo >> 0) & 0xf;
	xdev->abs_n_proc_y = (karma_topo >> 4) & 0xf;
	xdev->abs_n_proc_z = (karma_topo >> 8) & 0xf;
	PERROR("My network topology is 0x%08x (%d,%d,%d)\n", xdev->topo, xdev->abs_n_proc_z, xdev->abs_n_proc_y, xdev->abs_n_proc_x);


	spin_lock_init(&xdev->glock);

	//RESET	
	karma_reg_write(xdev, KARMA_REG_RESET, 1);
	karma_reg_write(xdev, KARMA_REG_RESET, 0);


	karma_reg_write(xdev, KARMA_REG_SWITCH_CONFIG_COORD_ME, karma_addr);
	karma_reg_write(xdev, KARMA_REG_SWITCH_CONFIG_LATTICE_SIZE, karma_topo);


	karmadev_register_proc(xdev);


error_out:
	platform_set_drvdata(pdev, xdev);
	return err;
}

/**
 * karma_remove - Driver remove function
 * @pdev: Pointer to the platform_device structure
 *
 * Return: Always '0'
 */
static int karma_remove(struct platform_device *pdev){
	struct karma_dev_t *xdev = platform_get_drvdata(pdev);

 	PTRACE("Deleting cdev\n");
 	cdev_del( &xdev->cdev );

 	PTRACE("Unregister chrdev region %d\n", xdev->devt);
  	unregister_chrdev_region( xdev->devt, KARMA_DEVICES );
 
  	PTRACE("Destroy %p\n",xdev);
 	device_destroy( xdev->cl, xdev->devt );
 
 	PTRACE("Destroy class %p\n",xdev->cl);
  	class_destroy( xdev->cl );

 	karmadev_unregister_proc(xdev);
	return 0;
}


static struct platform_driver karma_driver = {
	.driver = {
		.name = "karma_fifo",
		.of_match_table = karma_of_ids,
	},
	.probe = karma_probe,
	.remove = karma_remove,
};

module_platform_driver(karma_driver);

MODULE_AUTHOR("INFN APE Lab");
MODULE_DESCRIPTION("APErouter driver");
MODULE_LICENSE("GPL v2");
