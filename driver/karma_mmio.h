#ifndef KARMADRV_MMIO_H
#define KARMADRV_MMIO_H

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

#include <linux/io.h>
#include "karma.h"

#ifdef KARMA_REGTRACE
#define WRITETRACE(OFF, VAL) printk(KERN_ERR "IO WRITE offset:0x%lx value:0x%08x\n", (OFF), VAL)
#define READTRACE(OFF) printk(KERN_ERR "IO READ offset:0x%lx\n", (OFF))
#else
#define WRITETRACE(OFF, VAL) /* REGTRACEW() */
#define READTRACE(OFF) /* REGTRACER*/
#endif

static inline u32 karma_reg_read(karma_dev_t *xdev, u32 reg){
	u32 r;
	READTRACE(xdev->mm_tgt_ctrl);
	KARMA_LOCK(xdev);
 	r = ioread32(xdev->mm_tgt_ctrl + (reg*4));
	KARMA_UNLOCK(xdev);

	return r;
}

static inline void karma_reg_write(karma_dev_t *xdev, u32 reg, u32 value){
	WRITETRACE(xdev->mm_tgt_ctrl,value);
	KARMA_LOCK(xdev);
	iowrite32(value, xdev->mm_tgt_ctrl + (reg*4));
	KARMA_UNLOCK(xdev);
}



#endif
