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

#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "caiaio.h"
#include "prflush.h"

pid_t caiaio_pid;
int child_term;

void init_caiaio(void)
{
  caiaio_pid = getpid();
  child_term = 0;

  // Make sure we get EPIPE and not get aborted by a SIGPIPE
  if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
  {
    fprintf(stderr, "%s\n", strerror(errno));
    exit(1);
  }
}

// reroute input output
void superdup(int filedes[2], int s) 
{
  switch (s) 
  {
    case STDOUT_FILENO:
    case STDERR_FILENO:
      close(filedes[PIPEREAD]);
      close(s);
      dup2(filedes[PIPEWRITE], s);
      close(filedes[PIPEWRITE]);
      break;
    case STDIN_FILENO:
      close(filedes[PIPEWRITE]);
      close(s);
      dup2(filedes[PIPEREAD], s);
      close(filedes[PIPEREAD]);
      break;
  }
}

void create_pipe(int filedes[2]) {
  if (pipe(filedes)) 
  {
    fprflush(stderr, "Error: create pipe: %s\n", strerror(errno));
    exit(1);
  }
}

int handle_select_measure(int fd, struct timeval timeout, int *sec, int *usec)
{
  int r;
  fd_set rfds;
  struct timeval start, stop;
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
//  fprflush(stderr, "timeout => %d\n", timeout.tv_sec * 1000 + timeout.tv_usec / 1000);
  gettimeofday(&start, NULL);
  r = select(fd+1, &rfds, NULL, NULL, &timeout);
  if (r < 0)
  {
    fprflush(stderr, "(Position caiaio.cc handle_select_measure) Error: select: %s (%d)\n", strerror(errno), errno);
    return r;
  }
  gettimeofday(&stop, NULL);
  *sec = stop.tv_sec - start.tv_sec;
  *usec = stop.tv_usec - start.tv_usec;
  if (*usec < 0)
  {
    (*sec)--;
    *usec += 1000000;
  }
  return r;
}

int handle_read(int fd, char *buf, size_t count)
{
  int r;
  r = read(fd, buf, count);
  if (r < 0)
  {
    fprflush(stderr, "Error: read: %s\n", strerror(errno));
  }
  return r;
}

int handle_write(int fd, const void * const buf, size_t count)
{
  int r;
  r = write(fd, buf, count);
  if (r < 0)
  {
    if (errno == EPIPE)
      return r;		// Handle this in Program::writeln
    fprflush(stderr, "Error: write: %s\n", strerror(errno)); 
    exit(1);
  }
  return r;
}

int handle_select(int fd, struct timeval timeout)
{
  int r;
  fd_set rfds;
  FD_ZERO(&rfds);
  FD_SET(fd, &rfds);
  r = select(fd+1, &rfds, NULL, NULL, &timeout);
  if (r < 0)
  {
    fprflush(stderr, "(Position caiaio.cc int handle_select) Error: select: %s\n", strerror(errno)); 
    exit(1);
  }
  return r;
}

void rewritespace(char * const str)
{
  int i, n = strlen(str);
  for (i = 0; i < n; i++)
  {
    if (str[i] == ' ') str[i] = '_';
  }
}


