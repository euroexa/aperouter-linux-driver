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

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>
#include <linux/seq_file.h>

#include "karma.h"
#include "karma_trace.h"
#include "karma_instr.h"
#include "karma_mmio.h"
#include "karma_regs.h"

/*----------------------------------------------------------------------------*/

// static void karmadev_instr_init(struct karma_dev_t *xdev){
// 	BUG_ON(NULL == xdev);
// 
// }

static int regs_show(struct seq_file *m, void *v){
	struct karma_dev_t *xdev = m->private;
	int reg=0, value;	
	// 	int r;
	// 	unsigned base = 0x100;
	// 	unsigned long flags;

	if(!xdev)
		goto error;

	for(reg=0;reg<KARMA_NUM_REGS;reg++){
		// 		karmadev_lock_irqsave(karmadev, flags);
		//		value=karmadev_readl(reg);
		value = karma_reg_read(xdev,reg);
		// 		karmadev_unlock_irqrestore(karmadev, flags);
		seq_printf(m,"reg[%d]=0x%08x\n", reg, value);
	}	

	return 0;
error:
	return -EINVAL;
}
/*----------------------------------------------------------------------------*/

static struct proc_dir_entry *proc_karmadev_dir = 0;

#define PROC_KARMA_DIR_NAME "karmadev"

static inline struct karma_dev_t *APE_PDE_DATA(struct inode *inode){
	return (struct karma_dev_t *)PDE_DATA(inode);
}

static int regs_proc_open(struct inode *inode, struct file *file)
{
	struct karma_dev_t *karmadev=APE_PDE_DATA(inode);
	return single_open(file, regs_show,karmadev);
}

static struct file_operations regs_fops={
	.owner = THIS_MODULE,
	.open           = regs_proc_open,
	.read           = seq_read,
	.release        = single_release,
};
static struct proc_dir_entry *ape_proc_create_data(const char *name, umode_t mode,struct proc_dir_entry *parent,const struct file_operations *proc_fops,void *data){

	struct proc_dir_entry *new_proc_entry;
	new_proc_entry = proc_create_data (name, mode, parent, proc_fops, data);
	if (!new_proc_entry){
		PERROR("ERROR: can't create /proc/<..>/%s\n",name);
		return NULL;
	}
	else
		return new_proc_entry;
}

void karmadev_register_proc(struct karma_dev_t* karmadev){
	struct proc_dir_entry *regs;
	BUG_ON(0 == karmadev);

	proc_karmadev_dir = proc_mkdir(PROC_KARMA_DIR_NAME, NULL);
	if(0==proc_karmadev_dir) {
		PERROR("can't create /proc/karmadev\n");
		return;
	}


	regs   = ape_proc_create_data ("regs", S_IFREG|S_IRUGO,  proc_karmadev_dir, &regs_fops  ,karmadev  );
	if(!regs)
		PERROR("cannot create /proc/<..>/regs\n");


	karmadev->proc_entry = proc_karmadev_dir;
}

/*----------------------------------------------------------------------------*/

void karmadev_unregister_proc(struct karma_dev_t* karmadev){

	BUG_ON(0 == karmadev);
	if(karmadev->proc_entry) {
		remove_proc_entry("regs", karmadev->proc_entry);
	}  else
		PERROR("no proc_entry\n");

	remove_proc_entry(PROC_KARMA_DIR_NAME, 0);
}
