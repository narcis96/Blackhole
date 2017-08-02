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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <assert.h>
#include "program.h"
#include "prflush.h"
#include "debug.h"

// program constructor
program::program(const char * const c)
{
  cmd = new char[strlen(c) + 1];
  argv = new char *[2];
  strcpy(cmd, c);
  logfile = NULL;
  logfptr = NULL;
  argc = 1;
  argv[0] = cmd;
  argv[1] = NULL;
  // make sure internal buffer is set righteously
  resetbuffer();
  resetbuffererr();
  fd_in = -1;
  fd_out = -1;
  fd_err = -1;
  dontpipeerr = false;
  crash = false;
  terminated = true;
}

void program::set_logfile(const char * const filename)
{
  logfile = new char[strlen(filename)+1];
  strcpy(logfile, filename);
}

void program::set_logfptr(FILE * const fptr) {
  logfptr = fptr;
}

void program::open_logfile() {
  if (logfile == NULL) return;
  logfptr = fopen(logfile, "w");
  if (!logfptr)
  {
    fprflush(stderr, DEBUG_ERROR_LOGFILE, logfile, strerror(errno));
  }
}

void program::open_logfile(const char * const filename) {
  set_logfile(filename);
  open_logfile();
}

void program::write_logfile(const char * const line) 
{
  if (logfptr == NULL) return;
  fprflush(logfptr, "%s\n", line);
}

void program::close_logfile()
{
  if (logfptr == NULL) return;
  fclose(logfptr);
}

void program::add_arg(char *arg)
{
  int i;
  char **tmpargv = new char *[argc + 2];
  assert(argv != NULL);
  for (i = 0; i < argc; i++)
  {
    tmpargv[i] = argv[i]; 
    argv[i] = NULL;
  }
  tmpargv[argc] = new char[strlen(arg) + 1];
  tmpargv[argc + 1] = NULL;
  strcpy(tmpargv[argc++], arg);
  delete[] argv;
  argv = tmpargv;
}

void program::set_dontpipeerr(bool dpe)
{
  dontpipeerr = dpe;
}

// program destructor
program::~program(void)
{

  int i;
  delete[] cmd;
  for (i = 1; i < argc; i++)
  {
    delete[] argv[i];
    argv[i] = NULL;
  }
  delete[] argv;
  if (logfile != NULL) {
    delete[] logfile;
  }
}

// program start, creates pipes, forks and executes
void program::start(void)
{
  int r;
  // create pipe for stdin
  create_pipe(indes);
  // create pipe for stdout
  create_pipe(outdes);
  if (!dontpipeerr) 
  {
    // create pipe for stderr
    create_pipe(errdes);
  }

  // fork this process, parent receives pid != 0, child receives pid == 0
  pid = fork();
  if (!pid)
  {
    // reroute std...
    superdup(outdes, STDOUT_FILENO);
    if (!dontpipeerr) 
    {
      superdup(errdes, STDERR_FILENO);
    }
    superdup(indes, STDIN_FILENO);

    // execute command
    r = execv(cmd, argv);
    // shouldn't come here...
    fprflush(stderr, "I: terminated starting a program: execv(%s, argv) failed\n", cmd);
    exit(1);
  }
  // close unused ends of pipes 
  fd_in = indes[PIPEWRITE];
  close(indes[PIPEREAD]);
  fd_out = outdes[PIPEREAD];
  close(outdes[PIPEWRITE]);
  if (!dontpipeerr) 
  {
    fd_err = errdes[PIPEREAD];
    close(errdes[PIPEWRITE]);
  }
  terminated = false;
}

void program::stop() 
{
  if (pid)
  {
    if (fd_out > 0) close(fd_out);
    if (fd_in > 0) close(fd_in);
    if (fd_err > 0) close(fd_err);
    kill(pid, SIGTERM);
//    waitpid(pid, NULL, WNOHANG);
    if (!terminated)
      wait(NULL);
    pid = 0;
  }
}

// write a line to the program's stdin
void program::writeln(const char * const buf)
{
  int i=0, n;
  int cur;
  char *buf2;
  // we don't want to alter the original buffer
  n = strlen(buf);
  buf2 = new char[n+1];
  strcpy(buf2, buf);
  // don't send a '\0' sends a '\n' instead
  buf2[n++] = '\n';
  // write doesn't always write the whole buffer, using while solves this
  
  if (!checkalive()) 
  {
    delete[] buf2;
    return;
  }
  while (i < n)
  {
    cur = handle_write(fd_in, buf2 + i, n - i);
    if (cur < 0)
    {
      // Necesary to handle the case where the program stops between checkalive and write
      delete[] buf2;
      fprflush(stderr, "Could not write to stdin of program %s\n", cmd);
      terminated = true;
      return;
    }
    i += cur;
  }
  delete[] buf2;
}

// resets the buffer
void program::resetbuffer(void)
{
  bufferpoint = 0;
  bufferend = -1;
}

// resets the buffererr
void program::resetbuffererr(void)
{
  buffererrpoint = 0;
  buffererrend = -1;
}

// reads from the buffer
int program::readbuffererr(void)
{
  buffererrpoint = 0;
  buffererrend = handle_read(fd_err, buffererr, MAXBUF);
  if (buffererrend == 0) 
  {
    close(fd_err);
    fd_err = -1;
  }
  return buffererrend;
}

// reads from the buffer
int program::readbuffer(void)
{
  bufferpoint = 0;
  bufferend = handle_read(fd_out, buffer, MAXBUF);
  if (bufferend < 0) {
    terminated = true;
  } else if (bufferend == 0) 
  {
//    debug("readbuffer: no bytes to read\n"); 
    close(fd_out);
    fd_out = -1;
  }
  return bufferend;
}

// read a line from the program's stderr
int program::readlnerr(char **buf, int * const max)
{
  char *tmpbuf;
  int n;

  n = 0;
  // copy byte per byte to the presented buffer
  do 
  {
    // if presented buffer wasn't initilized or too small create a new or larger buffer
    if (n >= *max)
    {
      // create a new buffer
      if (*max < 0)
      {
        *buf = new char[MAXBUF];
        *max = MAXBUF;
      }
      // make buffer larger
      else
      {
        tmpbuf = new char[*max + MAXBUF];
        memcpy(tmpbuf, *buf, *max);
        delete *buf;
        *buf = tmpbuf;
        *max += MAXBUF;
      }
    }
    // if bufferpoint >= bufferend do a readbuffer, if readbuffer `fails' close buffer and return
    if (buffererrpoint >= buffererrend && !readbuffererr()) 
    {
      // if readbuffer returns 0 on the first byte that is read 
      // readln returns -1 which indicates the program has terminated
      // pipe is closed on other side?
      (*buf)[n] = '\0';
      return -1;
    }
    (*buf)[n] = buffererr[buffererrpoint++];
  } while ((*buf)[n++] != '\n');
  // close buffer, replace '\n' with '\0'
  (*buf)[n-1] = '\0';
  return n-1;
}

// read a line from the program's stdout
int program::readln(char **buf, int * const max)
{
  char *tmpbuf;
  int n;

  n = 0;
  // copy byte per byte to the presented buffer
  do 
  {
    // if presented buffer wasn't initilized or too small create a new or larger buffer
    if (n >= *max)
    {
      // create a new buffer
      if (*max < 0)
      {
        *buf = new char[MAXBUF];
        *max = MAXBUF;
      }
      // make buffer larger
      else
      {
        tmpbuf = new char[*max + MAXBUF];
        memcpy(tmpbuf, *buf, *max);
        delete[] *buf;
        *buf = tmpbuf;
        *max += MAXBUF;
      }
    }
    // if bufferpoint >= bufferend do a readbuffer, if readbuffer `fails' close buffer and return
    if (bufferpoint >= bufferend && readbuffer() <= 0) 
    {
      // if readbuffer returns 0 on the first byte that is read 
      // readln returns -1 which indicates the program has terminated
      (*buf)[n] = '\0';
      return -1;
    }
    (*buf)[n] = buffer[bufferpoint++];
  } while ((*buf)[n++] != '\n');
  // close buffer, replace '\n' with '\0'
  (*buf)[n-1] = '\0';
  return n-1;
}

int program::checkalive(void)
{
  int status = 0;
  int r = 1;

  if (terminated) return false;

  r = waitpid(pid, &status, WNOHANG);
  if (r < 0)
  {
    // remark by Jaap:
    // does this occur under Windows
    // remark by Marcel:
    // this error can occur if the manager is nog programmed right
    debug("I: waitpid failed: %s\n", strerror(errno));
    terminated = true;
    return false;
  }
  else if (r) 
  {
    if (WIFSIGNALED(status))
    {
      fprflush(stderr, "program %s crashed (%s)\n", cmd, strsignal(WTERMSIG(status)));
      terminated = true;
      termstatus = 1;
      crash = true;
      crashsig = WTERMSIG(status);
      r = 0;
    }
    else if (WIFEXITED(status))
    {
      debug(DEBUG_PROGRAM_STOPPED, cmd);
      terminated = true;
      termstatus = WEXITSTATUS(status);
      r = 0;
    }
    return false;
  }
  return true;
}

int program::check_out(void)
{
  struct timeval nowait = { tv_sec:0, tv_usec:0 };
  if (bufferpoint < bufferend) return 1;
  if (fd_out < 0) return 0;
  return handle_select(fd_out, nowait);
}

int program::check_err(void)
{
  struct timeval nowait = { tv_sec:0, tv_usec:0 };
  if (buffererrpoint < buffererrend) return 1;
  if (fd_err < 0) return 0;
  return handle_select(fd_err, nowait);
}

bool program::running(void)
{
  return !terminated;
}

bool program::bufferempty(void)
{
  struct timeval nowait = { tv_sec:0, tv_usec:0 };
//debug("bufferempty?: ");
  if (bufferpoint < bufferend) 
  {
    debug(DEBUG_INFO_IN_BUFFER); 
    return false;
  }
  if (fd_out < 0) 
  {
//  debug("yes, stream closed\n"); 
    return true;
  }
  if (handle_select(fd_out, nowait))
  {
    debug(DEBUG_INFO_ON_STREAM); 
    return false;
  }
//debug("yes\n"); 
  return true;
}

bool program::out_closed(void)
{
  return fd_out < 0;
}

void program::suspend(void)
{
  kill(pid, SIGSTOP);
}

void program::quitsuspend(void)
{
  kill(pid, SIGCONT);
}

bool program::crashed(void)
{
  return crash;
}

int program::crashedsig(void)
{
  return crashsig;
}

