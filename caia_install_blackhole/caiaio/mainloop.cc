/* Copyright (C) 2007, 2008 Jaap Taal and Marcel Vlastuin.

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
#include <unistd.h>
#include <assert.h>
#include "mainloop.h"
#include "state.h"
#include "manager.h"
#include "referee.h"
#include "player.h"
#include "prflush.h"
#include "debug.h"
#include "error.h"
#include "cpuspeed.h"

void mainloop(void)
{
  class program *lock = NULL;
  char *line = NULL;
  char *linep;
  char word[256];
  char tmp1word[256];
  char tmp2word[256];
  char logline[4096];
  int linesize = -1;
  int tmp1int;
  int tmp2int;
  int i=0;
  int j, k, found;
  float time_factor = 1.0;
  io_state state = STATE_NONE;
  char cmd_sender[256];
  assert(manager != NULL);

  debug(DEBUG_MAINLOOP_STARTED); 
  while (1)
  {
    if (state == CAIAIO_STOP) break;
    if (state == CAIAIO_ERROR)
    {
      fprflush(stderr, "I: Error detected bailing out!\n");
      break;
    }
    if (manager->running()) manager->checkalive();
    if (!manager->running() && manager->bufferempty())
    {
      debug(DEBUG_MANAGER_CLOSED_NO_BUFFER);
      break;
    }
    if ((lock==NULL && manager->check_out()) || lock==manager) // Added by Bauke Conijn
    // if ((lock==NULL || lock==manager) && manager->check_out())
    {
      strcpy(cmd_sender, "manager");
      manager->readln(&line, &linesize);
      if (manager->out_closed())
      {
        // manager closed stdout
        // this means that CAIAIO will exit the main loop
        // it's unwise to make a manager that executes fclose(stdout)!
        // normally the CAIAIO will only end up here when the manager crashed (or exits in rare cases)
        fprflush(stderr, "manager suddenly stopped!\n");
        break;
      }
      debug(DEBUG_MESSAGE_FROM_MANAGER, line);
      linep = line;
      readword(&linep, word);

      if (!strcmp(word, IO_NAME_TAG))
      {
        readword(&linep, word);
        state = state_parse_token(word);
        switch(state)
        {
          case CAIAIO_STOP:
            debug(DEBUG_OK); 
            break;

          case CAIAIO_LOCK:
            lock = manager;
            debug(DEBUG_SEND_OK_BACK_TO_MANAGER); 
            manager->writeln(RESP_LOCK_OK);
            break;

          case CAIAIO_UNLOCK:
            lock = NULL;
            debug(DEBUG_OK); 
            break;

          case REFEREE_START:
            if (readword(&linep, tmp1word))
            {
              fprflush(stderr, ERROR_INC_PROGRAM, STR_REFEREE_START, line);
              break;
            }
            start_referee(tmp1word);
            if (!readword(&linep, tmp2word))
            {
              referee->add_arg(tmp2word);
            }
            referee->start();
            debug(DEBUG_OK); 
            break;

          case REFEREE_KILL:
            stop_referee();
            debug(DEBUG_OK); 
            break;

          case CAIAIO_CPU:
            if (readint(&linep, word, &tmp1int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_CCPU, STR_CAIAIO_CPU, word); }
              else { fprflush(stderr, ERROR_INC_CCPU, STR_CAIAIO_CPU, line); }
              state = CAIAIO_ERROR;
              break;
            }
            if (readint(&linep, word, &tmp2int))
            {
              if (strlen(word)) 
              {
                fprflush(stderr, ERROR_INV_TCPU, STR_CAIAIO_CPU, word);
                state = CAIAIO_ERROR;
                break;
              }
              tmp2int=cpuspeed();
            }
            if (tmp2int <= 0) tmp2int = 1;
            time_factor = float(tmp1int) / float(tmp2int);
            debug(DEBUG_CPU_SPEED, time_factor);
            break;

          case PLAYER_NUMBER:
            if (readint(&linep, word, &tmp1int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_PLAYNUM, STR_PLAYER_NUMBER, word); }
              else { fprflush(stderr, ERROR_INC_PLAYNUM, STR_PLAYER_NUMBER, line); }
              state = CAIAIO_ERROR;
              break;
            }
            if (tmp1int > MAXPLAYERS)
            {
              fprflush(stderr, ERROR_MAX_PLAYNUM, MAXPLAYERS, word); 
              state = CAIAIO_ERROR;
              break;
            }
            if (tmp1int < 1) // 16 july 2008 changed 2 into 1 making one-player games possible! (Marcel Vlastuin)
            {
              fprflush(stderr, ERROR_MIN_PLAYNUM, word); 
              state = CAIAIO_ERROR;
              break;
            }
            init_players(tmp1int);
            debug(DEBUG_OK); 
            break;

          case PLAYER_INIT:
            if (readint(&linep, word, &tmp1int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_PLAY, STR_PLAYER_INIT, word); }
              else { fprflush(stderr, ERROR_INC_PLAY, STR_PLAYER_INIT, word); }
              state = CAIAIO_ERROR;
              break;
            }
            if (tmp1int < 1 || tmp1int > nplayers)
            {
              fprflush(stderr, ERROR_PLAY_DNE, STR_PLAYER_INIT, tmp1int); 
              state = CAIAIO_ERROR;
              break;
            }
            if (player[tmp1int-1] != NULL) 
            {
              fprflush(stderr, ERROR_PLAY_AINI, STR_PLAYER_INIT, tmp1int); 
              state = CAIAIO_ERROR;
              break;
            }
            if (readword(&linep, tmp1word))
            {
              fprflush(stderr, ERROR_INC_PROG, STR_PLAYER_INIT, line);
              state = CAIAIO_ERROR;
              break;
            }
            if (readint(&linep, word, &tmp2int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_TIMEOUT, STR_PLAYER_INIT, word); }
              else { fprflush(stderr, ERROR_INC_TIMEOUT, STR_PLAYER_INIT, line); }
              state = CAIAIO_ERROR;
              break;
            }
            if (readword(&linep, tmp2word))
            {
              init_player(tmp1int-1, tmp1word, multiply(tmp2int, time_factor), NULL);
            }
            else 
            {
              init_player(tmp1int-1, tmp1word, multiply(tmp2int, time_factor), tmp2word);
            }
            debug(DEBUG_OK); 
            break;

          case PLAYER_START:
            if (readint(&linep, word, &tmp1int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_PLAY, STR_PLAYER_START, word);  }
              else { fprflush(stderr, ERROR_INC_PLAY, STR_PLAYER_START, line);  }
              state = CAIAIO_ERROR;
              break;
            }
            if (tmp1int < 1 || tmp1int > nplayers || player[tmp1int-1] == NULL)
            {
              fprflush(stderr, ERROR_PLAY_DNE, STR_PLAYER_START, tmp1int); 
              state = CAIAIO_ERROR;
              break;
            }
            start_player(tmp1int-1);
            if (readint(&linep, word, &tmp2int))
            {
              if (strlen(word)) 
              { 
                fprflush(stderr, ERROR_INV_FIRSTERR, STR_PLAYER_START, word); 
                state = CAIAIO_ERROR; 
                break; 
              }
              debug(DEBUG_OK); 
            }
            else
            {
              if (player[tmp1int-1]->poll_error(tmp2int))
              {
                player[tmp1int-1]->readlnerr(&line, &linesize);
                strcpy(logline, line);
                k=0; found=0;
                for (j=0; j<(int)strlen(line); ++j)
                {
                  if (!found && line[j]!=' ' && line[j]!='\t' && line[j]!='\n') {found=1; ++k;}
                  else if (found && (line[j]==' ' || line[j]=='\t' || line[j]=='\n')) {found=0;}
                }
                if (k==0) sprintf(line, "%s %s", PLAYER_ERROR_ZEROSTRING_TAG, PLAYER_ERROR_ZEROSTRING_TAG);
                else if (k==1) {strcat(line, " "); strcat(line, PLAYER_ERROR_ZEROSTRING_TAG);}
                sprintf(tmp1word, "%s %s", STR_FIRSTERROR, line);
                debug(DEBUG_CAIAIO_SENDS_TO_MANAGER, tmp1word);
                manager->writeln(tmp1word);
                debug("%d: %s\n", tmp1int, logline);
                player[tmp1int-1]->write_logfile(logline);
              }
              else 
              {
                debug(DEBUG_CAIAIO_SENDS_TO_MANAGER, STR_NO_FIRSTERROR);
                manager->writeln(STR_NO_FIRSTERROR);
              }
            }
            break;

          case PLAYER_KILL:
            if (readint(&linep, word, &tmp1int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_PLAY, STR_PLAYER_KILL, word);  }
              else { fprflush(stderr, ERROR_INC_PLAY, STR_PLAYER_KILL, line);  }
              state = CAIAIO_ERROR;
              break;
            }
            if (tmp1int < 1 || tmp1int > nplayers || player[tmp1int-1] == NULL)
            {
              fprflush(stderr, ERROR_PLAY_DNE, STR_PLAYER_KILL, tmp1int); 
              state = CAIAIO_ERROR;
              break;
            }
            while (player[tmp1int-1]->check_err()) 
            {
              player[tmp1int-1]->readlnerr(&line, &linesize);
              debug("%d: %s\n", tmp1int, line);
              player[tmp1int-1]->write_logfile(line);
            }
            stop_player(tmp1int-1);
            debug(DEBUG_OK); 
            break;

          case PLAYER_LISTEN:
            if (readint(&linep, word, &tmp1int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_PLAY, STR_PLAYER_LISTEN, word);  }
              else { fprflush(stderr, ERROR_INC_PLAY, STR_PLAYER_LISTEN, line);  }
              state = CAIAIO_ERROR;
              break;
            }
            if (tmp1int < 1 || tmp1int > nplayers || player[tmp1int-1] == NULL)
            {
              fprflush(stderr, ERROR_PLAY_DNE, STR_PLAYER_LISTEN, tmp1int); 
              state = CAIAIO_ERROR;
              break;
            }
            debug(DEBUG_OK); 

            player[tmp1int-1]->checkalive();
            if (player[tmp1int-1]->crashed()) 
            {
              sprintf(line, PLAYER_ERROR_CRASHED_TAG, strsignal(player[tmp1int-1]->crashedsig())); //see mainloop.h
              rewritespace(line);
            }
            else if (!player[tmp1int-1]->running() && !player[tmp1int-1]->check_out())
            {
              sprintf(line, PLAYER_ERROR_STOPPED_TAG); //see mainloop.h
            }
            else 
            {
              player[tmp1int-1]->readln(&line, &linesize);
            }
            if (player[tmp1int-1]->timedout())
            {
              sprintf(line, PLAYER_ERROR_TIMEOUT_TAG); //see mainloop.h
            }
            debug(DEBUG_MESSAGE_FROM_PLAYER, tmp1int, line);
            // if the players sends a "\n" the manager has a problem reading this with scanf():
            if (strlen(line) == 0)
            {
              sprintf(line, PLAYER_ERROR_ZEROSTRING_TAG); //see mainloop.h
            }
            debug(DEBUG_CAIAIO_SENDS_TO_MANAGER, line);
            manager->writeln(line);
            break;

          case PLAYER_REQUEST_TIME:
            if (readint(&linep, word, &tmp1int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_PLAY, STR_PLAYER_REQUEST_TIME, word);  }
              else { fprflush(stderr, ERROR_INC_PLAY, STR_PLAYER_REQUEST_TIME, line);  }
              state = CAIAIO_ERROR;
              break;
            }
            if (tmp1int < 1 || tmp1int > nplayers || player[tmp1int-1] == NULL)
            {
              fprflush(stderr, ERROR_PLAY_DNE, STR_PLAYER_REQUEST_TIME, tmp1int); 
              state = CAIAIO_ERROR;
              break;
            }
            sprintf(line, "time %d %d", time_player(tmp1int - 1), multiply(time_player(tmp1int - 1), 1 / time_factor));
            debug(DEBUG_CAIAIO_SENDS_TO_MANAGER, line);
            manager->writeln(line);
            break;

          default:
            debug(DEBUG_NEWLINE);
            fprflush(stderr, ERROR_UNKNOWN_CMD, "Manager", line); 
            state = CAIAIO_ERROR;
            break;
        }
      }
      else for (i = 0; i < nplayers; ++i)
      {
        sprintf(tmp1word, "%d", i+1);
        if (!strcmp(word, tmp1word))
        {
          debug(DEBUG_CAIAIO_SENDS_TO_PLAYER, linep, i+1); 
          player[i]->writeln(linep);
          break;
        }
      }  
    }


    if (referee != NULL && referee->running()) referee->checkalive();
    if (referee != NULL && !referee->running() && referee->bufferempty())
    {
      debug("closing referee\n");
      referee->stop();
      delete referee;
      referee = NULL;
      lock = NULL;
      continue;
    }

   if (referee !=NULL && ((lock==NULL && referee->check_out()) || lock==referee)) // Added by Bauke Conijn 
   //if ((lock==NULL || lock==referee) && referee !=NULL && referee->check_out())
    {
      strcpy(cmd_sender, "referee");
      referee->readln(&line, &linesize);
      if (referee->out_closed())
      {
        // referee heeft stdout afgesloten,
        // dit betekent de IO uit de mainloop gaat
        // in de referee NOOIT fclose(stdout) doen dus!!!
        // normaliter komt de IO hier als de referee afgesloten/gecrashed is
        //        debug("referee: out_closed\n");
        //        debug("closing referee\n");
        referee->stop();
        delete referee;
        referee = NULL;
        lock = NULL;
        continue;
      }
      debug(DEBUG_MESSAGE_FROM_REFEREE, line); 
      linep = line;
      readword(&linep, word);

      if (!strcmp(word, IO_NAME_TAG))
      {
        readword(&linep, word);
        state = state_parse_token(word);
        switch(state)
        {  
          case CAIAIO_LOCK:
            lock = referee;
            debug(DEBUG_SEND_OK_BACK_TO_REFEREE); 
            referee->writeln(RESP_LOCK_OK);
            break;

          case CAIAIO_UNLOCK:
            lock = NULL;
            debug(DEBUG_OK); 
            break;

          case PLAYER_LISTEN:
            if (readint(&linep, word, &tmp1int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_PLAY, STR_PLAYER_LISTEN, word);  }
              else { fprflush(stderr, ERROR_INC_PLAY, STR_PLAYER_LISTEN, line);  }
              state = CAIAIO_ERROR;
              break;
            }
            if (tmp1int < 1 || tmp1int > nplayers || player[tmp1int-1] == NULL)
            {
              fprflush(stderr, ERROR_PLAY_DNE, STR_PLAYER_LISTEN, tmp1int); 
              state = CAIAIO_ERROR;
              break;
            }
            debug(DEBUG_OK); 

            player[tmp1int-1]->checkalive();
            if (player[tmp1int-1]->crashed()) 
            {
              sprintf(line, PLAYER_ERROR_CRASHED_TAG, strsignal(player[tmp1int-1]->crashedsig())); //see mainloop.h
              rewritespace(line);
            }
            else if (!player[tmp1int-1]->running() && !player[tmp1int-1]->check_out())
            {
              sprintf(line, PLAYER_ERROR_STOPPED_TAG); //see mainloop.h
            }
            else 
            {
              player[tmp1int-1]->readln(&line, &linesize);
            }
            if (player[tmp1int-1]->timedout())
            {
              sprintf(line, PLAYER_ERROR_TIMEOUT_TAG); //see mainloop.h
            }
            debug(DEBUG_MESSAGE_FROM_PLAYER, tmp1int, line);
            // if the players sends a "\n" the referee has a problem reading this with scanf():
            if (strlen(line) == 0)
            {
              sprintf(line, PLAYER_ERROR_ZEROSTRING_TAG); //see mainloop.h
            }
            debug(DEBUG_CAIAIO_SENDS_TO_REFEREE, line);
            referee->writeln(line);
            break;

          case PLAYER_REQUEST_TIME:
            if (readint(&linep, word, &tmp1int))
            {
              if (strlen(word)) { fprflush(stderr, ERROR_INV_PLAY, STR_PLAYER_REQUEST_TIME, word);  }
              else { fprflush(stderr, ERROR_INC_PLAY, STR_PLAYER_REQUEST_TIME, line);  }
              state = CAIAIO_ERROR;
              break;
            }
            if (tmp1int < 1 || tmp1int > nplayers || player[tmp1int-1] == NULL)
            {
              fprflush(stderr, ERROR_PLAY_DNE, STR_PLAYER_REQUEST_TIME, tmp1int); 
              state = CAIAIO_ERROR;
              break;
            }
            sprintf(line, "time %d %d", time_player(tmp1int - 1), multiply(time_player(tmp1int - 1), 1 / time_factor));            
            debug(DEBUG_CAIAIO_SENDS_TO_REFEREE, line);
            referee->writeln(line);
            break;

          default:
            debug(DEBUG_NEWLINE);
            fprflush(stderr, ERROR_UNKNOWN_CMD, "Referee", line); 
            state = CAIAIO_ERROR;
            break;
        }
      }
      else if (!strcmp(word, MANAGER_NAME_TAG))
      {
        debug(DEBUG_CAIAIO_SENDS_TO_MANAGER, linep); 
        manager->writeln(linep);
      }
      else for (i = 0; i < nplayers; ++i)
      {
        sprintf(tmp1word, "%d", i+1);
        if (!strcmp(word, tmp1word))
        {
          debug(DEBUG_CAIAIO_SENDS_TO_PLAYER, linep, i+1); 
          player[i]->writeln(linep);
          break;
        }
      }  
    }

    for (i = 0; i < nplayers; i++)
    {
      if (player[i] != NULL)
      {
        while (player[i]->check_err()) 
        {
          if (player[i]->readlnerr(&line, &linesize) != -1) {
            debug("%d: %s\n", i+1, line);
            player[i]->write_logfile(line);
          }
        }
      }
    }
  }
  if (line != NULL) {
    delete[] line;
  }
  debug(DEBUG_MAINLOOP_ENDED); 
}


