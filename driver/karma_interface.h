#ifndef KARMA_INTERFACE_H
#define KARMA_INTERFACE_H

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

#include "apenest_types.h"

#define KARMA_IOC_MAGIC  'k' 

typedef an_net_addr_t karma_net_address_t;
typedef an_vaddr_t karma_vaddr_t;

typedef struct{
  karma_vaddr_t       src_vaddr;
  karma_vaddr_t       dst_vaddr;
  karma_net_address_t dst_addr;
  size_t size;
} karma_msg_desc_t;

#define KARMA_IOGETSELFADDR      _IOR(KARMA_IOC_MAGIC,  10, u32 * )
#define KARMA_IOSETREG           _IOR(KARMA_IOC_MAGIC,  13, karma_msg_desc_t )
#define KARMA_IOGETREG           _IOR(KARMA_IOC_MAGIC,  14, karma_msg_desc_t )

typedef struct karma_io_op {
#define KARMA_IO_OP_MAGIC 0x20455041
  uint32_t    magic;

  uint32_t      type;
  
  karma_vaddr_t vaddr;
  uint64_t      rkey;
  size_t        len;
  int           flags;
  int           err_code; 
} karma_io_op_t; 
#endif
