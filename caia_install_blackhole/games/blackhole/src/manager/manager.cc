/*

  Made by Marcel Vlastuin, marcel@codecup.nl.
  Manager example for the game Blackhole (CodeCup 2018).
  For more information: www.codecup.nl.
  This program is part of the Caia project for the CodeCup 2018.
  The code is "as it is".

*/

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#define LOCK(x) printf("I lock\n"); fflush(stdout); do scanf("%s", (x)); while (strcmp((x), "lock_ok"))
#define UNLOCK printf("I unlock\n"); fflush(stdout)

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

int main (int argc, char *argv[])
{
  FILE *instr = NULL;
  int i, ngames;
  int answer1, answer2;
  char log1[80], log2[80], buf3[80], dummy[80];
  char player1[80], player2[80];
  
// Use the next line if your computer CPU speed is 2200 MHz:
  
//prflush("I cpu_speed 2800 2200\n");

// Use the next line if you let the cacaio make a guess of the computer CPU speed:  

  prflush("I cpu_speed 2800\n");

// It is possible to leave both lines out, in that case the cacaio asumes that
// your computer CPU speed is the same as that of the CodeCup computer.  
  
  if (argc >= 2) instr = fopen(argv[1], "r");
  else instr = fopen("manager.txt", "r");
  
// The file mamanger.txt, or another one,  must exist! Read the documentation about this.
  
  if (instr == NULL)
  {
    fprflush(stderr, "Non-existing filename\n");
    prflush("I stop_caiaio\n");
    return 1;
  }

// Read the number of games to be played:
  
  fscanf(instr, "%d", &ngames);
  
  for (i=0; i<ngames; i++)
  { 

// Read the names of both players and the log files:

    fscanf(instr, "%s", player1);
    fscanf(instr, "%s", log1);
    fscanf(instr, "%s", player2);
    fscanf(instr, "%s", log2);

    fprflush(stderr, "M: GAME %d = %s <> %s\n", i+1, player1, player2);

// Inform the cacaio that there are 2 players:
    
    prflush("I number_players 2\n");

// Inform the caiaio who player 1 is and where its stderr can be printed to
// (if the player does so)
// The playing time for player 1 is 30000 milliseconds:

    LOCK(dummy);
    prflush("I player 1 %s 5000 ../playerlogs/%s\n", player1, log1);
    UNLOCK;
    
// It is under Linux possible to identify the player using the so called firsterror.
// In that case:
//
// prflush("I start 1 20\n");
//
// schould be used.
//
// There are two possibilities:
// 1. "no_firsterror" is returned by the cacaio if the player did not printed anything
//    to its stderr since 20 milliseconds after being started.
// 2. "firsterror string1 string2" is returned by the caiaio. In this case the player
//    must return two strings in order to identify himself.
//    
// The next 6 lines should be included as well:
//    
//  scanf("%s", buf3);
//  if (!strcmp(buf3, "firsterror"))
//  { 
//    scanf("%s", buf4);
//    scanf("%s", buf5);
//  }
//
// For more details about the so called firsterror: see the Caia documentation.    

// Repeat for player 2:
    
    LOCK(dummy);
    prflush("I player 2 %s 5000 ../playerlogs/%s\n", player2, log2);
    UNLOCK;
    
// After that, start the players:
    
    prflush("I start 1\n");
    prflush("I start 2\n");

// Put the referee on a job and provide him a unique log file name:
// You must be unlocked before giving this command to the caiaio!    

    prflush("I referee referee ../refereelogs/%s-%s.%d.log\n", player1, player2, i+1);

// The referee sends back: "report gamepoints1 gamepoints2 message\n":
// The referee must be written this way to do this!     
    
    scanf("%s", dummy);    // string report
    scanf("%d", &answer1); // int gamepoints1
    scanf("%d", &answer2); // int gamepoints2
    scanf("%s", buf3);     // string message_about_what_happened
    
    fprflush(stderr, "M: RESULT = %d - %d (game marks) %s\n", answer1, answer2, buf3);
    
// Make sure all 3 processes are killed:   
    
    prflush("I kill_referee\n");
    prflush("I kill 1\n");
    prflush("I kill 2\n");
  }

// Tell the cacaio he stops himself:
  
  prflush("I stop_caiaio\n");
  return 0;
}
