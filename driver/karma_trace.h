#ifndef KARMA_TRACE_H
#define KARMA_TRACE_H

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

#define PERROR(fmt, args...) printk(KERN_ERR "E [%s:%d]:" fmt, __FUNCTION__, __LINE__, ## args)

#define PERRORC(C, FMT, ARGS...)					      \
do {									      \
	static int __cnt = C;						      \
	if(__cnt) {							      \
		--__cnt;						      \
		PERROR(FMT, ## ARGS);					      \
	}								      \
} while(0)

#define PWARN(fmt, args...) printk(KERN_WARNING "W [%s:%d]:" fmt, __FUNCTION__, __LINE__, ## args)

#define PWARNC(C, FMT, ARGS...)						      \
do {									      \
	static int __cnt = C;						      \
	if(__cnt) {							      \
		--__cnt;						      \
		PWARN(FMT, ## ARGS);					      \
	}								      \
} while(0)

/* we use PDEBUG() for debug */
#ifdef KARMA_DEBUG
#warning DEBUG is ON !!!
#define PDEBUG(fmt, args...) printk(KERN_DEBUG "D [%s:%d]:" fmt, __FUNCTION__, __LINE__, ## args)
#else
#define PDEBUG(...) do { } while(0)
#endif

#define PDEBUGC(C, FMT, ARGS...)					      \
do {									      \
	static int __cnt = C;						      \
	if(__cnt) {							      \
		--__cnt;						      \
		PDEBUG(FMT, ## ARGS);					      \
	}								      \
} while(0)


#ifdef KARMA_PTRACE
#warning "PTRACE enabled, expected slow-down"
#define PTRACE(fmt, args...) printk(KERN_ERR "T [%s:%d]:" fmt, __FUNCTION__, __LINE__, ## args)
#else
#define PTRACE(fmt, args...) do { } while(0)
#endif

#define PTRACEC(C, FMT, ARGS...)					      \
do {									      \
	static int __cnt = C;						      \
	if(__cnt) {							      \
		--__cnt;						      \
		PTRACE(FMT, ## ARGS);					      \
	}								      \
} while(0)

#endif
