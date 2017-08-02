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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h> // vanwege de exit()
#include "timeprogram.h"
#include "prflush.h"

timeprogram::timeprogram(const char * const c, int maxmilliseconds): program(c)
{
  outoftime = false;
  timelimit = maxmilliseconds;
  timeleft.tv_sec = maxmilliseconds / 1000;
  timeleft.tv_usec = (maxmilliseconds % 1000) * 1000;
}

int timeprogram::msecused(void) 
{
  return timelimit - (timeleft.tv_sec * 1000 + timeleft.tv_usec / 1000);
}

int timeprogram::msecleft(void)
{
  return timeleft.tv_sec * 1000 + timeleft.tv_usec / 1000;
}

int timeprogram::readbuffer(void)
{
  int r;
  int sec, usec;
  bufferpoint = 0;

  r = handle_select_measure(fd_out, timeleft, &sec, &usec);
  if (r < 0) {
    terminated = true;
    return r;
  }
  timeleft.tv_sec -= sec;
  timeleft.tv_usec -= usec;
  if (timeleft.tv_usec < 0) 
  {
    timeleft.tv_usec += 1000000;
    timeleft.tv_sec--;
  }
  if (r)
  {
    bufferend = handle_read(fd_out, buffer, MAXBUF);
  }
  else
  {
    outoftime = true;
  }
  return bufferend;
}

int timeprogram::timedout(void)
{
  return outoftime;
}

int timeprogram::poll_error(int millisecs)
{
  int r;
  struct timeval timeout = { tv_sec: millisecs / 1000, tv_usec: (millisecs % 1000) * 1000 };
  fd_set rfds;
  if (fd_err < 0) return 0;
  FD_ZERO(&rfds);
  FD_SET(fd_err, &rfds);
  r = select(fd_err+1, &rfds, NULL, NULL, &timeout);
  if (r < 0)
  {
    fprflush(stderr, "(Position timeprogram.cc timeprogram::poll_error) Error: select: %s\n", strerror(errno));
    exit(1);
  }
  return r;
}

