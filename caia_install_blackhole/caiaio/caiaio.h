/* Copyright (C) 2005 Jaap Taal and Marcel Vlastuin.

This file is part of the Caia project. This project can be
downloaded from the website www.codecup.nl. You can email to
marcel@vlastuin.net or write to Marcel Vlastuin, Perenstraat 40,
2564 SE Den Haag, The Netherlands.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA. */

#ifndef CACAIO_H
#define CACAIO_H

#include <sys/time.h>
#define PIPEREAD 0
#define PIPEWRITE 1
#define IO_NAME_TAG "I"

#define MAXBUF 256

void init_caiaio(void);

void superdup(int filedes[2], int s);
void create_pipe(int filedes[2]);

int handle_select_measure (int fd, struct timeval timeout, int *sec, int *usec);
int handle_read(int fd, char *buf, size_t count);
int handle_write(int fd, const void * const buf, size_t count);
int handle_select (int fd, struct timeval timeout);
void rewritespace(char * const str);

#endif

