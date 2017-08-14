//
//  main.cpp
//  Manager
//
//  Created by Gemene Narcis on 15/08/2017.
//  Copyright © 2017 Gemene Narcis. All rights reserved.
//

#include <iostream>
#include <vector>
#include <string>

std::pair < int, int > Battle(const char * server,
                              const char * player1,
                              const char * player2) {
    std::pair < int, int > scores(0, 0);
    std::string cmd = std::string(server) + std::string(" ") + std::string(player1) + std::string(" ") + std::string(player2);
    FILE * pipe = popen(cmd.c_str(), "r+");
    
    if (pipe == NULL) {
        fprintf(stderr, "popen() failed");
        exit(EXIT_FAILURE);
    }
    
    char buff[100];
    while (fgets(buff, 100, pipe) != NULL) {
        int score1, score2;
        sscanf(buff, "%d %d", & score1, & score2);
        scores.first += score1;
        scores.second += score2;
    }
    if (pclose(pipe) == -1) {
        fprintf(stderr, "Error reported by pclose()");
        exit(EXIT_FAILURE);
    }
    return scores;
}

int main(int argc,const char * argv[]) {
    std::vector < const char * > players;
    const char * server;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-p") == 0) //player
        {
            i += 1;
            if (i == argc) {
                fprintf(stderr, "Incorrect parameters\n");
                fflush(stderr);
                return EXIT_FAILURE;
            }
            players.push_back(argv[i]);
        } else {
            if (strcmp(argv[i], "-s") == 0) //server
            {
                i += 1;
                if (i == argc) {
                    fprintf(stderr, "Incorrect parameters\n");
                    fflush(stderr);
                    return EXIT_FAILURE;
                }
                server = argv[i];
            } else {
                fprintf(stderr, "Incorrect parameters\n");
                fflush(stderr);
                return EXIT_FAILURE;
            }
        }
    }
    std::vector < int > scores(players.size(), 0);
    for (int rounds = 1; rounds > 0; rounds--) {
        for (int i = 0; i < players.size(); i++)
            for (int j = i + 1; j < players.size(); j++) {
                auto score = Battle(server, players[i], players[j]);
                scores[i] += score.first;
                scores[j] += score.second;
            }
    }
    return EXIT_SUCCESS;
}
