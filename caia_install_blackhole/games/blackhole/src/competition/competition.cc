/*

  Made by Marcel Vlastuin, marcel@codecup.nl.
  Competition manager for the game Blackhole (CodeCup 2018).
  For more information: www.codecup.nl.
  This program is part of the Caia project for the CodeCup 2018.
  The code is "as it is".
 
  A small patch is applied on 31 October 2009 by Matthijs Tijink.

*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>

#ifdef __linux__
#include <unistd.h>
#endif

#ifdef __CYGWIN__
#include <unistd.h>
#endif

#ifdef __APPLE__
#include <unistd.h>
#endif

#ifndef _MAX_PATH
#define _MAX_PATH 512
#endif

#define LOCK(x) printf("I lock\n"); fflush(stdout); do scanf("%s", (x)); while (strcmp((x), "lock_ok"))
#define UNLOCK printf("I unlock\n"); fflush(stdout)
#define MAXPLAYERS 100
#define MAXGAMELOGS 1000
#define AANTAL 10
#define AANTALEEN 1 // normally this value would be: 1

// prflush() is a combination of a printf() followed by a fflush(stdout).
// The flushing is necessary to put the cacaio immediately on a job.

int prflush(const char *fmt, ...)
{
  int r;
  va_list ap;
  va_start(ap, fmt);
  r = vprintf(fmt, ap);
  va_end(ap);
  fflush(stdout);
  return r;
}

// fprflush() is a combination of a fprintf(stream, ...) and a fflush(stream).
// In this program only fprintf(stderr, ...) and fflush(stderr) is used.

int fprflush(FILE *stream, const char *fmt, ...)
{
  int r;
  va_list ap;
  va_start(ap, fmt);
  r = vfprintf(stream, fmt, ap);
  va_end(ap);
  fflush(stream);
  return r;
}

void GetCurrentPath(char *buffer)
{
  getcwd(buffer, _MAX_PATH);
}

typedef struct {char player[200]; char typeplayer; long modified; int score;} complist;


void cleanup(int *number, complist *list)
{
  int i, j;
  char dummy[200];
  //.
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    dummy[1]='\0';
    if (!strcmp(dummy, "."))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number;
      break;
    }
  }
  //..
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    dummy[2]='\0';
    if (!strcmp(dummy, ".."))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number;
      break;
    }
  }
  //javascript
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "javascript"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number;
      break;
    }
  }
  //jarwrapper(.exe)
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "jarwrapper") || !strcmp(dummy, "jarwrapper.exe"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //javawrapper(.exe)
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "javawrapper") || !strcmp(dummy, "javawrapper.exe"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //manager(.exe)
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "manager") || !strcmp(dummy, "manager.exe"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //referee(.exe)
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "referee") || !strcmp(dummy, "referee.exe"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //generator(.exe)
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "generator") || !strcmp(dummy, "generator.exe"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //competition(.exe)
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "competition") || !strcmp(dummy, "competition.exe"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //*.jar
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    j=strlen(dummy);
    if (!strcmp(dummy+j-4, ".jar"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //*.py
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    j=strlen(dummy);
    if (!strcmp(dummy+j-3, ".py"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //*.class
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    j=strlen(dummy);
    if (!strcmp(dummy+j-6, ".class"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //*.txt
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    j=strlen(dummy);
    if (!strcmp(dummy+j-4, ".txt"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //caiaio(.exe)
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "caiaio") || !strcmp(dummy, "caiaio.exe"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number; --i;
    }
  }
  //cygwin1.dll
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "cygwin1.dll"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number;
      break;
    }
  }  
  //competition.sh
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    if (!strcmp(dummy, "competition.sh"))
    {
      strcpy(list[i].player, list[*number-1].player);
      list[i].modified=list[*number-1].modified;
      list[i].typeplayer=list[*number-1].typeplayer;
      list[i].score=list[*number-1].score;
      --*number;
      break;
    }
  }  
  //*.exe
  for (i=0; i<*number; ++i)
  {
    strcpy(dummy, list[i].player);
    j=strlen(dummy);
    if (!strcmp(dummy+j-4, ".exe")) list[i].player[j-4]='\0';
  }
  return;
}

int vergelijk(const void *a, const void *b)
{
  return ((complist *)b)->score-((complist *)a)->score;
}

int main (int argc, char *argv[])
{
  FILE *instr = NULL, *complog = NULL, *comptxt = NULL;
  DIR *dirp;
  int ii, i, j, k, l, number, numberhis, found, aantal, txtmax;
  int answer1, answer2, bufanswer1, bufanswer2;
  char buf3[200], buf4[200], buf5[200], buf6[200], bufbuf3[200*AANTAL], dummy[200];
  char logname[200], history[MAXGAMELOGS][200];
  struct
  {
    char player1[200], dummy[200];
    char typeplayer1;
    long modified1;
    char player2[200];
    char typeplayer2;
    long modified2;
    int score1;
    int score2;
  } hislist[MAXGAMELOGS];
  complist list[MAXPLAYERS];
  int matrix[MAXPLAYERS][MAXPLAYERS][2];
  char CurrentPath[_MAX_PATH], OtherPath[_MAX_PATH];
  struct dirent *dptr;
  struct stat buf;
  struct stat *statptr=&buf;
  prflush("I cpu_speed 2800\n");
  if (argc >= 2) instr = fopen(argv[1], "wt");
  comptxt=fopen("competition.txt", "rt");
  GetCurrentPath(CurrentPath);
  if ((dirp=opendir(CurrentPath))==NULL)
  {
    fprflush(stderr, "Error opening %s", CurrentPath);
    perror("dirlist");
    exit(1);
  }
  if (comptxt)
  {
	  fscanf(comptxt, "%d", &txtmax);
	  for (j=0; j<txtmax; ++j) fscanf(comptxt, "%s", history[j]);
    i=0;
    while ((dptr=readdir(dirp)))
    {
      if (stat(dptr->d_name, statptr))
      {
        fprflush(stderr, "Error in stat call\n");
        exit(1);
      }
	    for (j=0; j<txtmax; ++j)
	    {
        strcpy(buf3, history[j]);
        if (!strcmp(buf3, dptr->d_name))
		    {
		      strcpy(list[i].player, dptr->d_name); list[i].modified=statptr->st_mtime;
          list[i].typeplayer='D'; list[i].score=0;
          ++i;
         break;
	      }
        strcat(buf3, ".exe");
        if (!strcmp(buf3, dptr->d_name))
        {
          strcpy(list[i].player, dptr->d_name); list[i].modified=statptr->st_mtime;
          list[i].typeplayer='D'; list[i].score=0;
          ++i;
          break;
        }
      }
    }
    number=i;
    closedir(dirp);
    cleanup(&number, list);
  }
  else
  {
    i=0;
    while ((dptr=readdir(dirp)))
    {
      if (stat(dptr->d_name, statptr))
      {
        fprflush(stderr, "Error in stat call\n");
        exit(1);
      }
      strcpy(list[i].player, dptr->d_name); list[i].modified=statptr->st_mtime;
      list[i].typeplayer='D'; list[i].score=0;
      ++i;
    }
    number=i;
    closedir(dirp);
    cleanup(&number, list);
  }
  fprflush(stderr, "\nParticipants          Modified    Type\n");
  fprflush(stderr, "--------------------------------------\n");
  if (instr)
  {
    fprflush(instr, "Nr  Participants          Modified    Type\n");
    fprflush(instr, "------------------------------------------\n");
  }
  for (i=0; i<number; ++i)
  {
    LOCK(dummy);
    prflush("I number_players 2\n");
    prflush("I player 1 %s 5000\n", list[i].player);
    prflush("I start 1 1000\n");
    scanf("%s", buf3);
    if (!strcmp(buf3, "firsterror"))
    { 
      scanf("%s", buf4);
      switch(buf4[0])
      {
        case 'S': list[i].typeplayer='S'; break;
        case 'T': list[i].typeplayer='T'; break;
        case 'R': list[i].typeplayer='R'; break;
        default:  list[i].typeplayer='D';
      }
      scanf("%s", buf5);
    }
    UNLOCK;
    prflush("I kill 1\n");
    fprflush(stderr, "%-20s (%ld)   %c\n", list[i].player, list[i].modified, list[i].typeplayer);
    if (instr) fprflush(instr, "%2d  %-20s (%ld)   %c\n", i+1, list[i].player, list[i].modified, list[i].typeplayer);
  }
  fprflush(stderr, "\n");
  if (instr) fprflush(instr, "\n");
  strcpy(OtherPath, CurrentPath);
  l=strlen(OtherPath);
  OtherPath[l-3]='\0';
  strcat(OtherPath, "competitionlogs");
  if ((dirp=opendir(OtherPath))==NULL)
  {
    fprflush(stderr, "Error opening %s\n", OtherPath);
    perror("dirlist");
    exit(1);
  }
  i=0;
  while ((dptr=readdir(dirp)))
  {  
    strcpy(history[i], dptr->d_name);
    ++i;
  }
  numberhis=i;
  closedir(dirp);
  for (i=2; i<numberhis; ++i) 
  {
    sscanf(history[i], "%s (%c %ld) %s (%c %ld) %d - %d", hislist[i-2].player1, &hislist[i-2].typeplayer1, &hislist[i-2].modified1, hislist[i-2].player2, &hislist[i-2].typeplayer2, &hislist[i-2].modified2, &hislist[i-2].score1, &hislist[i-2].score2);
  }
  numberhis-=2;
  for (i=0; i<number; ++i) for (j=0; j<number; ++j) if (i!=j)  
  { 
    found=0;
    for (k=0; k<numberhis; ++k)
    if (!strcmp(list[i].player, hislist[k].player1) &&
        list[i].typeplayer==hislist[k].typeplayer1 &&
        list[i].modified==hislist[k].modified1 &&
        !strcmp(list[j].player, hislist[k].player2) &&
        list[j].typeplayer==hislist[k].typeplayer2 &&
        list[j].modified==hislist[k].modified2)
    {
      list[i].score+=hislist[k].score1;
      list[j].score+=hislist[k].score2;
      matrix[i][j][0]=hislist[k].score1;
      matrix[i][j][1]=hislist[k].score2;
      found=1;
      break;
    }
    if (found) continue;
    strcpy(bufbuf3, "");
    bufanswer1=0; bufanswer2=0;
    if (list[i].typeplayer=='R' || list[j].typeplayer=='R') aantal=AANTAL; else aantal=AANTALEEN;
    for (ii=0; ii<aantal; ++ii)
    {
      fprflush(stderr, "M: GAME %s <> %s\n", list[i].player, list[j].player);
      LOCK(dummy);
      prflush("I number_players 2\n");
      prflush("I player 1 %s 5000 ../playerlogs/%s.white.%s.%d.log\n", list[i].player, list[i].player, list[j].player,ii+1);
      prflush("I start 1\n");
      prflush("I player 2 %s 5000 ../playerlogs/%s.black.%s.%d.log\n", list[j].player, list[j].player, list[i].player, ii+1);
      prflush("I start 2\n");
      UNLOCK;
      prflush("I referee referee ../refereelogs/%s-%s.%d.log\n", list[i].player, list[j].player, ii+1);
      scanf("%s", dummy);    // string report
      scanf("%d", &answer1); // int gamepoints1
      scanf("%d", &answer2); // int gamepoints2
      scanf("%s", buf3);     // string message_about_what_happened
      answer1*=AANTAL; answer2*=AANTAL;
      fprflush(stderr, "M: RESULT = %d - %d (game marks) %s\n", answer1/AANTAL, answer2/AANTAL, buf3);
      bufanswer1+=answer1; bufanswer2+=answer2;
      sprintf(buf6, "Result: %3d - %3d  Description: %s\n", answer1/AANTAL, answer2/AANTAL, buf3); strcat(bufbuf3, buf6); 
      prflush("I kill_referee\n");
      prflush("I kill 1\n");
      prflush("I kill 2\n");
    }
    sprintf(logname, "../competitionlogs/%s (%c %ld) %s (%c %ld) %d - %d", list[i].player, list[i].typeplayer, list[i].modified, list[j].player, list[j].typeplayer, list[j].modified, bufanswer1/aantal, bufanswer2/aantal);
    complog=fopen(logname, "wt");
    fprflush(complog, "%s", bufbuf3);
    fclose(complog);
    matrix[i][j][0]=bufanswer1/aantal;
    matrix[i][j][1]=bufanswer2/aantal;
    list[i].score+=bufanswer1/aantal; list[j].score+=bufanswer2/aantal;
  }  
  if (instr)
  {
    fprflush(instr, "Competition Results      |");
    for (i=0; i<number; ++i)
    {
      strcpy(dummy, list[i].player);
      dummy[9]='\0';
      fprflush(instr, " %2d:%-10s", i+1, dummy);
    }
    fprflush(instr, "\n-------------------------+");
    for (i=0; i<number; ++i) fprflush(instr, "--------------");
    fprflush(instr, "\n");
    for (i=0; i<number; ++i)
    {
      fprflush(instr, "%2d  %-20s |", i+1, list[i].player);
      for (j=0; j<number; ++j)
      {
        if (i!=j) fprflush(instr, "  %5d-%5d ", matrix[i][j][0], matrix[i][j][1]);
        else fprflush(instr, "       *      ");
      }
      fprflush(instr, "\n");
    }
  }
  qsort(list, number, sizeof(complist), vergelijk);
  fprflush(stderr, "\nRanking order             Modified    Type  Score\n");
  fprflush(stderr, "-------------------------------------------------\n");
  if (instr)
  {
    fprflush(instr, "\nRanking order             Modified    Type   Score\n");
    fprflush(instr, "--------------------------------------------------\n");
  }
  for (i=0; i<number; ++i)
  {
    fprflush(stderr, "%2d  %-20s (%ld)   %c   %4d\n", i+1, list[i].player, list[i].modified, list[i].typeplayer, list[i].score);
    if (instr) fprflush(instr, "%2d  %-20s (%ld)   %c   %4d\n", i+1, list[i].player, list[i].modified, list[i].typeplayer, list[i].score);
  }
  fprflush(stderr, "\n");
  prflush("I stop_caiaio\n");
  return 0;
}
