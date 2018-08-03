#ifndef APENEST_TYPES_H
#define APENEST_TYPES_H

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

// this file is used also in user-space.
// typedef uint8_t  u8;
// typedef uint16_t u16;
// typedef uint32_t u32;
// typedef uint64_t u64;


typedef union apenest_address {
  struct{
    int x:7;
    int y:7;
    int z:7;
  } s;
  struct {
    unsigned xyz:21;
  } all;
  uint32_t u;
} apenest_address_t;

typedef apenest_address_t an_net_addr_t;

typedef uint64_t an_vaddr_t;
typedef uint16_t an_pid_t;
#define AN_PORTID_BITS 16
#define AN_NPORTS      (1<<AN_PORTID_BITS)

#endif
