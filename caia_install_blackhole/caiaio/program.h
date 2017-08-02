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

#ifndef PROGRAM_H
#define PROGRAM_H

#include <sys/types.h>
#include <stdio.h>
#include "caiaio.h"

class program
{
  private:
    char *cmd;
    char *logfile;
    FILE *logfptr;
    char **argv;
    int argc;
    pid_t pid;

    int termstatus;
    int crashsig;

  protected:
    char buffer[MAXBUF];
    int bufferpoint;
    int bufferend;
    char buffererr[MAXBUF];
    int buffererrpoint;
    int buffererrend;

    bool terminated;
    bool crash;

  private:
    int indes[2];
    int outdes[2];
    int errdes[2];
  public:
    int fd_in;
    int fd_out;
    int fd_err;
  private:
    bool dontpipeerr;

  protected:
    void resetbuffer(void);
    void resetbuffererr(void);
    virtual int readbuffer(void);
    virtual int readbuffererr(void);
  public:
    program(const char * const c);
    void set_logfile(const char * const filename);
    void set_logfptr(FILE * const fptr);
    void open_logfile();
    void close_logfile();
    void open_logfile(const char * const filename);
    void write_logfile(const char * const line);
    void add_arg(char *arg);
    virtual ~program(void);
    void start(void);
    void stop(void);
    int readln(char **buf, int * const max);
    int readlnerr(char **buf, int * const max);
    void writeln(const char * const buf);
    int checkalive(void);
    int check_out(void);
    int check_err(void);
    void set_dontpipeerr(bool dpe);
    bool running(void);
    bool bufferempty(void);
    bool out_closed(void);
    void suspend(void);
    void quitsuspend(void);
    bool crashed(void);
    int crashedsig(void);

};

#endif

