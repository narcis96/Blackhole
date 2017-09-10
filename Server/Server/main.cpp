//
//  main.cpp
//  Server
//
//  Created by Gemene Narcis on 13/08/2017.
//  Copyright © 2017 Gemene Narcis. All rights reserved.
//
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <sstream>
#include <tuple>
#include <fstream>
#include <sys/types.h>
#include <unistd.h>
#include <thread>
#include <utility>
#include <chrono>
#include <random>
#include "../../ParamParser/ParamParser.h"
class RandomGenerator
{
public:
    static int GetNumber(const int x) {
        static auto d = std::chrono::system_clock::now().time_since_epoch();
        static std::mt19937 gen(std::chrono::duration_cast<std::chrono::milliseconds>(d).count());
        static std::uniform_int_distribution <int> dist(1, 1<<14);
        return dist(gen) % x;
    }
};
std::pair<int, int> GetPoints(const std::vector<int>& states,
                              const std::vector<int>& values, const std::vector<int>&neighbors)
{
    std::pair< int, int> points;
    for (const auto &x: neighbors)
        if (states[x] == 0) {
            points.first += values[x];
            points.second -= values[x];
        } else if(states[x] == 1){
            points.second += values[x];
            points.first -= values[x];
        }
    return points;
}
int GetEmptyCell(const std::vector<int>& states)
{
    int count = 0;
    int emptyCell = 0;
    for (int i = 0; i < states.size(); i++) {
        if (states[i] == -2) // the cell is empty
        {
            count += 1;
            assert(count == 1 && "More that one cell empty");
            emptyCell = i;
        }
    }
    return emptyCell;
}

std::vector<std::vector<int>>ReadGraph(const char* graphPath) {
    std::ifstream f(graphPath);
    int n, x, y;
    f >> n;
    std::vector<std::vector<int>>graph(n, std::vector<int>());
    while(f >> x >> y) {
        graph[x].push_back(y);
        graph[y].push_back(x);
    }
    f.close();
    return graph;
}

int main(int argc, const char* argv[])
{
#ifdef _WIN32
    assert(0 && "Windows does not support popen()");
    return EXIT_FAILURE;
#endif
    if (argc < 1 + 2*8) {
        fprintf(stderr, "Server(%d):Not enough or too much parameters\n", getpid());
        fflush(stderr);
        return EXIT_FAILURE;
    }
    auto Add = [](std::string &cmd, const std::string& option, const std::string &value)
    {
        cmd += " " + option +  " " + value + "";
    };
    std::vector<const char* > PlayerParams{
        "-graphPath",
        "-blockedCells",
        "-moves"
    };
    ParamParser parser(argc,argv);
    std::string player1 = parser.GetParam("-player1");
    std::string player2 = parser.GetParam("-player2");
    for (const auto& param :PlayerParams) {
        Add(player1, param, parser.GetParam(param));
        Add(player2, param, parser.GetParam(param));        
    }
    const int blockedCells = std::stoi(parser.GetParam("-blockedCells"));
    const int moves = std::stoi(parser.GetParam("-moves"));
    const bool debug = std::stoi(parser.GetParam("-debug"));
    const bool printMoves = std::stoi(parser.GetParam("-printMoves"));
    const bool generatedCells = std::stoi(parser.GetParam("-generatedCells"));
    std::vector < std::vector <int > > graph = ReadGraph(parser.GetParam("-graphPath").c_str());
    
    std::vector<int>cells(blockedCells, -1);
    if(generatedCells == true) {
        for(const auto& cell: parser.GetParam("-cell", true)) {
            cells.push_back(std::stoi(cell));
        }
    }
    assert(cells.size() == blockedCells && "Server: Not enough cells setted");
    assert(graph.size() > 0 && "Server: Graph is empty");

    if (debug == true) {
        fprintf(stderr, "Server(%d): Started \nplayer1 = %s\nplayer2 = %s\n", getpid(), player1.c_str(), player2.c_str());
        fflush(stderr);
    }
    std::pair<int, int> points[2];
    std::vector<int> playedValues[2][2];
    
    std::vector<int> values(graph.size(), -2);
    std::vector<int> states(graph.size(), -2);

    std::vector<int>games(1,0);
    if(printMoves == false) {
        games.push_back(1);
    }
    for(const auto& first:games) {
//        clock_t start = clock();
        FILE* clients[2];
        clients[0] = popen(player1.c_str(), "r+");
        clients[1] = popen(player2.c_str(), "r+");
        if (clients[0] == NULL || clients[1] == NULL) {
            fprintf(stderr, "popen() failed");
            fflush(stderr);
            return EXIT_FAILURE;
        }
        std::fill(values.begin(), values.end(), -2);
        std::fill(states.begin(), states.end(), -2);

        if(generatedCells == false) {
            for (int i = 0; i < blockedCells; i++) {
                int x;
                do {
                    x = RandomGenerator::GetNumber(graph.size());    
                } while (states[x] == -1);
        
                states[x] = -1;
                values[x] = -1;                
                cells[i] = x;
            }
        }
        for (const auto& x: cells) {
            std::stringstream buff;
            buff << x;
            if (debug == true) {
                fprintf(stderr, "Server(%d) : wrote %d\n", getpid(), x);
                fflush(stderr);
            }
            if (printMoves == true) {
                fprintf(stdout, "%d\n",x);
                fflush(stdout);        
            }
            for (int id = 0; id < 2; id++) {
                fprintf(clients[id], "%s\n", buff.str().c_str());
                fflush(clients[id]);
            }
        }
        fprintf(clients[first], "Start\n");
        fflush(clients[first]);
        if (debug == true) {
            fprintf(stderr, "Server(%d) : wrote Start\n", getpid());
            fflush(stderr);
        }
        int turn = first;
        char buff[100];
        for (int i = 1; i <= 2 * moves; i++) {
            if (fgets(buff, 100, clients[turn]) == NULL) {
                fprintf(stderr, "fgets() failed at step %d\n", i);
                fflush(stderr);
                return EXIT_FAILURE;
            }
            int indx, val;
            sscanf(buff, "%d=%d", &indx, &val);
            
            if (printMoves == true) {
                fprintf(stdout, "%d %d\n", indx, val);
                fflush(stdout); 
            }
            if (debug == true) {
                fprintf(stderr, "Server(%d) : received %s", getpid(), buff);
                fflush(stderr);
            }

            assert(0 <= indx  && indx < graph.size() && "Server :Index out of boundaries");
            assert(1 <= val && val <= moves &&
                   "Server: Not a valid move, value is wrong");
            assert(values[indx] == -2 && states[indx] == -2 &&
                   "Server :Not a valid move, the cell is not empty");
            playedValues[first][turn].push_back(val);
            states[indx] = turn;
            values[indx] = val;
            
            turn = turn ^ 1;
            
            fprintf(clients[turn], "%s", buff);
            fflush(clients[turn]);
        }
        
        int emptyCell = GetEmptyCell(states);
        points[first] = GetPoints(states, values, graph[emptyCell]);
        for (auto& client : clients) {
            int status = pclose(client);
            if (status == -1) {
                fprintf(stderr, "Error reported by pclose()");
                return EXIT_FAILURE;
            }
        }
        //    fprintf(stderr, "time =  %f\n", (double)(clock() - start) /
        //    CLOCKS_PER_SEC);
        //    fflush(stderr);
    }
    
    //	fprintf (stderr, "Time:\n1:%Lf 2:%Lf\n", timeElapsed[0],
    // timeElapsed[1]);
    //    fflush (stderr);
    
    if (debug == true) {
        for (int game = 0; game < games.size(); game++) {
            for (int i = 0; i < 2; i++) {
                fprintf(stderr, "Server(%d) : player %d values:", getpid(), i);
                for (auto x : playedValues[game][i]) {
                    fprintf(stderr, "%d ", x);
                
                }
                fprintf(stderr, "\n");
                fflush(stderr);
            }
        }
    }
    
    for (const auto& score : points) {
        if (debug == true) {
            fprintf(stderr, "Server(%d) : %d %d\n", getpid(), score.first, score.second);
            fflush(stderr);
        }
        
        fprintf(stdout, "%d %d\n", score.first, score.second);
        fflush(stdout);
    }
    return EXIT_SUCCESS;
}
