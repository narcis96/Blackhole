//
//  main.cpp
//  Manager
//
//  Created by Gemene Narcis on 15/08/2017.
//  Copyright © 2017 Gemene Narcis. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <atomic>
#include <mutex>
#include <algorithm>
#include <thread>
#include <chrono>
#include <functional>
//#include "ParamParser.h"
#include "../../ParamParser/ParamParser.h"
void Battle(FILE* pipe, int player1Index, int player2Index,
       std::function<void(int, int, int, int)> callback)
{
    fflush(stderr);
    if (pipe == NULL) {
        fprintf(stderr, "popen() failed");
        exit(EXIT_FAILURE);
    }
    int player1Score = 0, player2Score = 0, score1, score2;
    
    std::stringstream ss;
    ss << std::this_thread::get_id();
    std::string threadId = ss.str();
    
    char buff[100];
    for (int match = 1; match <= 2; match++) {
        if (fgets(buff, 100, pipe) == NULL) {
            fprintf(stderr, "Manager (%s): fgets() failed at step %d\n", threadId.c_str(), match);
            fflush(stderr);
            exit(EXIT_FAILURE);
        }
        sscanf(buff, "%d %d", &score1, &score2);
        fprintf(stderr, "Manager(%s) received : %d %d at match %d\n", threadId.c_str(), score1, score2, match);
        fflush(stderr);
        player1Score += score1;
        player2Score += score2;
    }
    int status = pclose(pipe);
    if (status == -1) {
        fprintf(stderr, "Error reported by pclose()");
        exit(EXIT_FAILURE);
    }
    fflush(stderr);
    callback(player1Index, player2Index, player1Score, player2Score);
}


int main(int argc, const char* argv[])
{
#ifdef _WIN32
    assert(0 && "Windows does not support popen()");
    return EXIT_FAILURE;
#endif
    auto Add = [](std::string &cmd, const std::string& option, const std::string &value)
    {
        cmd += " " + option +  " \"" + value + "\"";
    };
    
    std::atomic<int> availableThreads;
    availableThreads = std::thread::hardware_concurrency() - 1; //- main thread
    ParamParser parser(argc, argv);
    std::vector<std::string> players = parser.GetParam("-player", true);
    std::string server = parser.GetParam("-server");
    const bool debug = std::stoi(parser.GetParam("-debug"));
    const int rounds = std::stoi(parser.GetParam( "-rounds"));
    assert(rounds > 0);
    assert(players.size() >= 2);
    
    if (debug == true) {
        for (const auto& player : players) {
            fprintf(stderr, "%s\n", player.c_str());
        }
        fflush(stderr);
    }
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
                
                std::string cmd = server;
                Add(cmd,"-player1", players[i]);
                Add(cmd,"-player2", players[j]);
//                fprintf(stderr, "cmd = %s\n", cmd.c_str());
//                fflush(stderr);
                FILE* pipe = popen(cmd.c_str(), "r+");
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                threads.emplace_back(Battle, pipe, i, j,
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
        fprintf(stdout, "%d\n", std::max(scores[i],0));
    }
    fflush(stdout);
    return EXIT_SUCCESS;
}
