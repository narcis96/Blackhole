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
void Battle(const char* cmd, int player1Index, int player2Index,
       std::function<void(int, int, int, int)> callback)
{
    FILE* pipe = popen(cmd, "r+");
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
std::vector<const char*> GetParam(int argc,const char* argv[], const char* option, bool more = false) {
    std::vector<const char*>values;
    for (int i = 1; i < argc - 1; i += 2)
        if (strcmp(argv[i], option) == 0)
        {
            values.push_back(argv[i+1]);
        }
    if (values.empty() == true || (more == false && values.size() > 1)) {
        fprintf(stdout,"Parameter %s not found ", option);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }
    return values;
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

    std::vector<const char*> players = GetParam(argc,argv,"-player", true);
    const char* server = GetParam(argc,argv,"-server").back();
    const int rounds = atoi(GetParam(argc,argv, "-rounds").back());
    const char* graphPath = GetParam(argc,argv,"-graphPath").back();
    const int blockedCells = atoi(GetParam(argc,argv,"-blockedCells").back());
    const int moves = atoi(GetParam(argc,argv, "-moves").back());
    const bool debug = atoi(GetParam(argc,argv, "-debug").back());
    
    assert(rounds > 0);
    assert(server != NULL);
    assert(players.size() >= 2);
    
    if (debug == true) {
        for (const auto& player : players) {
            fprintf(stderr, "%s\n", player);
        }
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
                
                std::string cmd = server;
                Add(cmd,"-player1", players[i]);
                Add(cmd,"-player2", players[j]);
                Add(cmd,"-graphPath", graphPath);
                Add(cmd,"-blockedCells", std::to_string(blockedCells));
                Add(cmd,"-moves", std::to_string(moves));
                Add(cmd,"-debug", std::to_string(debug));
                threads.emplace_back(Battle, cmd.c_str(), i, j,
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
