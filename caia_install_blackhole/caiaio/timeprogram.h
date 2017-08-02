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

#ifndef TIMEPROGRAM_H
#define TIMEPROGRAM_H

#include <sys/time.h>
#include "program.h"

class timeprogram: public program
{
  private:
    bool outoftime;
    int timelimit;
    struct timeval timeleft;
  public:
    timeprogram(const char * const c, int maxmilliseconds);
    int readbuffer(void);
    int msecused(void);
    int msecleft(void);
    int timedout(void);
    int poll_error(int millisecs);
};

#endif


