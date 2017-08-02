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
#include "player.h"
#include "prflush.h"
#include "error.h"

class timeprogram *player[MAXPLAYERS];
int nplayers;

void init_players(int numplay)
{
  int i;
  nplayers = numplay;
  for (i = 0; i < nplayers; i++)
  {
    player[i] = NULL;
  }
}

void stop_players(void)
{
  int i;
  for (i = 0; i < nplayers; i++)
  {
    if (player[i] != NULL)
    {
      player[i]->stop();
      player[i]->close_logfile();
      delete player[i];
      player[i] = NULL;
    }
  }
}

void init_player(int pnum, char *cmd, int maxmilliseconds, char *logfile)
{
  player[pnum] = new timeprogram(cmd, maxmilliseconds);
  if (logfile != NULL) {
    player[pnum]->set_logfile(logfile);
  }
}

void start_player(int pnum)
{
  player[pnum]->open_logfile();
  player[pnum]->start();
}

void stop_player(int pnum)
{
  player[pnum]->stop();
  player[pnum]->close_logfile();
  delete player[pnum];
  player[pnum] = NULL;
}

int time_player(int pnum)
{
  return player[pnum]->msecused();
}


