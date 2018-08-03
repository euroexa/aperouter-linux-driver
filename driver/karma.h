#ifndef KARMADRV_H
#define KARMADRV_H

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

typedef struct karma_dev_t {
	void __iomem *mm_tgt_ctrl;
 	struct platform_device *pdev;
  	struct device *dev;
 	dev_t devt;
	struct class *cl;
	struct cdev cdev;

	struct proc_dir_entry *proc_entry;

	u32               addr;
	u32               topo;
	// Absolute topology
	u32               abs_n_proc_x;
	u32               abs_n_proc_y;
	u32               abs_n_proc_z;
	// Absolute coordinate
	u32               abs_tor;
	u32               abs_t;
	u32               abs_x;
	u32               abs_y;
	u32               abs_z;

	spinlock_t glock;

} karma_dev_t;

#define KARMA_DEVICES 1


#define KARMA_LOCK(xdev) spin_lock((spinlock_t *)&xdev->glock);
#define KARMA_UNLOCK(xdev) spin_unlock((spinlock_t *)&xdev->glock);

#define KARMA_BUF_LOCK(xdev) spin_lock((spinlock_t *)&xdev->buf_lock);
#define KARMA_BUF_UNLOCK(xdev) spin_unlock((spinlock_t *)&xdev->buf_lock);
#endif
