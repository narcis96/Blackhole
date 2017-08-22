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
#include <cassert>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <functional>
void Battle(const char* server, const char* player1, int player1Index,
       const char* player2, int player2Index,
       std::function<void(int, int, int, int)> callback)
{
    
    std::string cmd = std::string(server) + " \"" + std::string(player1) +
    "\" \"" + std::string(player2) + "\"";
    
    FILE* pipe = popen(cmd.c_str(), "r+");
    if (pipe == NULL) {
        fprintf(stderr, "popen() failed");
        exit(EXIT_FAILURE);
    }
    int player1Score = 0, player2Score = 0;
    char buff[100];
    while (fgets(buff, 100, pipe) != NULL) {
        int score1, score2;
        sscanf(buff, "%d %d", &score1, &score2);
        player1Score += score1;
        player2Score += score2;
    }
    int status = pclose(pipe);
    if (status == -1) {
        fprintf(stderr, "Error reported by pclose()");
        exit(EXIT_FAILURE);
    }
    callback(player1Index, player2Index, player1Score, player2Score);
}

int main(int argc, const char* argv[])
{
#ifdef _WIN32
    assert(0 && "Windows does not support popen()");
    return EXIT_FAILURE;
#endif
    std::vector<const char*> players;
    const char* server = NULL;
    int rounds = 0;
	bool debug = false
    std::atomic<int> availableThreads;
    availableThreads = std::thread::hardware_concurrency() - 1; //- main thread
    for (int i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-player") == 0) // player
        {
            i += 1;
            players.push_back(argv[i]);
        } else {
            if (strcmp(argv[i], "-server") == 0) // server
            {
                i += 1;
                server = argv[i];
            } else {
                if (strcmp(argv[i], "-rounds") == 0) {
                    i += 1;
                    rounds = atoi(argv[i]);
                } if (strcmp(argv[i], "-debug") == 0) {
                    debug = true;
                } 
				else {
                    fprintf(stderr, "Manager: Incorrect parameters\n");
                    fflush(stderr);
                    return EXIT_FAILURE;
                }
            }
        }
    }
    assert(rounds > 0);
    assert(server != NULL);
    assert(players.size() >= 2);
    
    for (const auto& player : players) {
        fprintf(stderr, "%s\n", player);
    }
    fflush(stderr);
    std::vector<int> scores(players.size(), 0);
    std::vector<std::thread> threads;
    int matches = 0;
    std::mutex myMutex;
    for (int round = 1; round <= rounds; round++) {
        for (int i = 0; i < players.size(); i++)
            for (int j = i + 1; j < players.size(); j++) {
                while (availableThreads.load() == 0) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                availableThreads -= 1;
                threads.emplace_back(Battle, server, players[i], i, players[j], j,
                                     [&myMutex, &scores, &matches, players,
                                      &availableThreads, &debug](int player1Index, int player2Index,
                                                         int player1Score, int player2Score) {
                                         
                                         std::lock_guard<std::mutex> myLock(myMutex);
                                         scores[player1Index] += player1Score;
                                         scores[player2Index] += player2Score;
                                         matches += 1;
										 if (debug == true) {
	                                         fprintf(stderr, "After match:%d\n", matches);
	                                         for (int i = 0; i < players.size(); i++) {
	                                             fprintf(stderr, "%d ", scores[i]);
	                                         }
	                                         fprintf(stderr, "\n");
	                                         fflush(stderr);
										 }
                                         availableThreads += 1;
                                     });
            }
    }
    for (auto& thread : threads) {
        thread.join();
    }
    threads.clear();
    for (int i = 0; i < players.size(); i++) {
        fprintf(stdout, "%d\n", scores[i]);
    }
    fflush(stdout);
    return EXIT_SUCCESS;
}
