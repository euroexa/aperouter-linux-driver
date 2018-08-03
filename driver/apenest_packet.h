#ifndef __apenest_packet__
#define __apenest_packet__

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

  $Id: apenest_packet.h,v 1.5 2018/08/03 13:34:38 martinelli Exp $
*/


#define AN_PKT_HEADER_SIZE_BYTES  16
#define AN_PKT_WORD_SIZE_BYTES    16
#define AN_PKT_FOOTER_SIZE_BYTES  16


typedef union apenest_header {
	struct __attribute__((packed)) {
		unsigned long virt_chan   :  5;
		unsigned long proc_id     : 16;
		unsigned long dest_x      :  4;
		unsigned long dest_y      :  4;
		unsigned long dest_z      :  4;
		unsigned long dest_t      :  2;
		unsigned long dest_tor    :  8;
		unsigned long flags       :  5;
		unsigned long packet_size : 14;
		unsigned long dest_vaddr  : 42;
		unsigned long intra_dest  :  3;
		unsigned long frst_valid  :  2;
	        unsigned long last_valid  :  3;
		unsigned long edac        : 16;
	} s;
	uint32_t l[4];
	uint64_t u[2];
} apenest_header_t;

// COMPILE_TIME_ASSERT(sizeof(apenest_header_t) == AN_PKT_HEADER_SIZE_BYTES);

typedef union __attribute__((packed)) apenest_footer {
	struct {
		unsigned int word1;
		unsigned int word2;
		unsigned int word3;
		unsigned int word4;
	} s;
	uint32_t l[4];
	uint64_t u[2];
} apenest_footer_t;

// COMPILE_TIME_ASSERT(sizeof(apenest_footer_t) == AN_PKT_FOOTER_SIZE_BYTES);

//AL keep it for the moment...
typedef union __attribute__((packed)) apenest_word {
	struct {
		unsigned long low;
		unsigned long high;
	} s;
	uint32_t l[4];
	char c[16];
} apenest_word_t;

// COMPILE_TIME_ASSERT(sizeof(apenest_word_t) == AN_PKT_WORD_SIZE_BYTES);

#endif
