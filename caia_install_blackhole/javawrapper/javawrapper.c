/**
 * This file acts as a wrapper program that executes Java programs.
 * It executes a Java class in the default package. The name for this
 * class is derived from the executable name of this program. For example:
 * If this program is named PlayerName.exe it executes:
 * java PlayerName
 *
 * (C) 2007, CodeCup.nl, Jaap Taal, <jtaal@codecup.nl>
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
	char cmd[1000];
	int i, n, spos, epos;
	strcpy(cmd, argv[0]);
	n = strlen(cmd);

	// SEARCH FOR THE LAST \ OR /
	spos = 0;
	for (i = 0; i < n; i++) {
		if (cmd[i] == '/' || cmd[i] == '\\') {
			spos = i + 1;
		}
	}

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

	execlp("java", "java", cmd, NULL);
	return 0;
}
