/*
  This file is part of the APErouter tools stack.
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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "karma_interface.h"

int main(int argc, char *argv[])
{
	int fd;
	uint32_t offset, value;
	char *endptr;
	const char karmadev[] = "/dev/aperouter";

	if (argc != 3) {
		fprintf(stderr, "Usage: %s regnum value\n", argv[0]);
		return -1;
	}

	offset = strtoul(argv[1], &endptr, 0);
	if (errno == ERANGE || *endptr != '\0') {
		fprintf(stderr, "ERROR: cannot parse offset '%s'\n", argv[1]);
		return -1;
	}

	value = strtoul(argv[2], &endptr, 0);
	if (errno == ERANGE || *endptr != '\0') {
		fprintf(stderr, "ERROR: cannot parse value '%s'\n", argv[2]);
		return -1;
	}

	fd = open(karmadev, O_RDWR | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "ERROR: cannot open %s, %s\n", karmadev, strerror(errno));
		return -1;
	}

	uint32_t tmp[2] = {offset, value};

	printf("setting reg[%u]=0x%08x\n", tmp[0], tmp[1]);
	if (ioctl(fd, KARMA_IOSETREG, tmp) != 0)
		fprintf(stderr, "ERROR: ioctl failed, %s\n", strerror(errno));

	close(fd);
	return 0;
}
