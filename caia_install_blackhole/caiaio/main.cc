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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include "caiaio.h"
#include "manager.h"
#include "referee.h"
#include "player.h"
#include "mainloop.h"
#include "commandline.h"
#include "prflush.h"
#include "debug.h"

int main (int argc, char *argv[]) 
{
  struct timeval start, stop;
//  struct timeval timeout = { tv_sec: 1, tv_usec: 0 };
  int sec, usec;
  commandline(argc, argv);

  init_caiaio();
  init_manager();
  init_referee();

  start_manager();

  gettimeofday(&start, NULL);
  mainloop();
  gettimeofday(&stop, NULL);

  stop_players();
  stop_referee();
  stop_manager();

  sec = (stop.tv_sec - start.tv_sec);
  usec = (stop.tv_usec - start.tv_usec);
  if (usec < 0)
  {
    sec--;
    usec += 1000000;
  }
  debug(DEBUG_GOODBYE_STRING, sec + (usec + 500000) / 1000000);
  return 0;
}

