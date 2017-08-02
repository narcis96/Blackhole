/**
 * This file acts as a wrapper program that executes Java programs.
 * It executes a Java jar file in the current directory. The name of this
 * jar file is derived from the executable name of this program.
 * The jar file should contain a manifest which points to the class with
 * the main method.
 * 
 * For example:
 * If this program is named Player.exe it executes:
 * java -jar Player
 *
 * (C) 2007, CodeCup.nl, Jaap Taal, <jtaal@codecup.nl>
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define CMD_MAX 1024

int main(int argc, char *argv[]) {
	char cmd[CMD_MAX];
	int i, n, spos, epos;
	strncpy(cmd, argv[0], CMD_MAX - 4); // leave space for extention: .jar
	n = strlen(cmd);

	// SEARCH FOR THE LAST \ OR /
	spos = 0;
	for (i = 0; i < n; i++) {
		if (cmd[i] == '/' || cmd[i] == '\\') {
			spos = i + 1;
		}
	}

	epos = spos;
	// SEARCH FOR FIRST NON [a-zA-Z0-9_] CHARACTER
	for (i = spos; i < n; i++) {
		if ((cmd[i] >= 'a' && cmd[i] <= 'z') ||
				(cmd[i] >= 'A' && cmd[i] <= 'Z') ||
				(cmd[i] >= '0' && cmd[i] <= '9') ||
				cmd[i] == '_') {
			epos = i;
		} else {
			break;
		}
	}

	//REPLACE THE CMD
	for (i = spos, n = 0; i <= epos; i++, n++) {
		cmd[n] = cmd[i];
	}
	cmd[n] = '\0';
	strcat(cmd, ".jar");

	execlp("java", "java", "-jar", cmd, NULL);
	return 0;
}
