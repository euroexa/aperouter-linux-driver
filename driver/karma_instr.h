#ifndef KARMADRV_INSTR_H
#define KARMADRV_INSTR_H

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

void karmadev_register_proc(struct karma_dev_t* karmadev);
void karmadev_unregister_proc(struct karma_dev_t* karmadev);
#endif
