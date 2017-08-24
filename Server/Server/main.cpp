//
//  main.cpp
//  Server
//
//  Created by Gemene Narcis on 13/08/2017.
//  Copyright © 2017 Gemene Narcis. All rights reserved.
//
#include <cstdio>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <set>
#include <vector>
#include <cassert>
#include <tuple>
#include <fstream>

bool IsInterior(const unsigned long& dim, const int& i, const int& j)
{
    return (0 <= i && i < dim && 0 <= j && j <= i);
}

std::pair<int, int> GetPoints(const std::vector<int>& states,
                              const std::vector<int>& values, const std::vector<int>&neighbors)
{
    std::pair< int, int> points;
    for (const auto &x: neighbors)
        if (states[x] == 0) {
            points.first += values[x];
            points.second -= values[x];
        } else {
            points.second += values[x];
            points.first -= values[x];
        }
    return points;
}
int GetEmptyCell(const std::vector<int>& states, const std::vector<int>& values)
{
    int count = 0;
    int emptyCell = 0;
    for (int i = 0; i < states.size(); i++) {
        if (values[i] == 0 && states[i] != -1) // the cell is empty
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
    int n, m;
    f >> n >> m;
    std::vector<std::vector<int>>graph(n, std::vector<int>());
    while(m-- > 0) {
        int x, y;
        f >> x >> y;
        graph[x].push_back(y);
        graph[y].push_back(x);
    }
    f.close();
    return graph;
}

const char* GetParam(int argc,const char* argv[], const char* option) {
    for (int i = 1; i < argc - 1; i += 2)
        if (strcmp(argv[i], option) == 0)
        {
            i += 1;
            return argv[i];
        }
    fprintf(stdout,"Parameter %s not found ", option);
    fflush(stdout);
    exit(EXIT_FAILURE);
}

int main(int argc, const char* argv[])
{
#ifdef _WIN32
    assert(0 && "Windows does not support popen()");
    return EXIT_FAILURE;
#endif
    if (argc < 4) {
        fprintf(stdout, "Not enough parameters");
        fflush(stdout);
        return EXIT_FAILURE;
    }
    const char* player1 = GetParam(argc, argv, "-player1");
    const char* player2 = GetParam(argc, argv, "-player2");
    const char* graphPath = GetParam(argc,argv,"-graphPath");
    const int blockedCells = atoi(GetParam(argc,argv,"-blockedCells"));
    const int moves = atoi(GetParam(argc,argv, "-moves"));
    const bool debug = atoi(GetParam(argc,argv, "-debug"));
    
    assert(player1 != NULL);
    assert(player2 != NULL);
    assert(blockedCells != -1);
    assert(moves != -1);
    
    std::vector < std::vector <int > > graph = ReadGraph(graphPath);
    int total = 2 * moves + blockedCells + 1;
    
    srand((unsigned int)time(0));
    
    std::pair<int, int> points[2];
    if (debug == true) {
        fprintf(stderr, "Server Started:\n");
        fprintf(stderr, "player1 = %s\nplayer2 = %s\n", player1, player2);
        fflush(stderr);
    }
    for (int first = 0; first < 2; first++) {
        clock_t start = clock();
        FILE* clients[2];
        fflush(stderr);
        clients[0] = popen(player1, "r+");
        clients[1] = popen(player2, "r+");
        fflush(stderr);
        if (clients[0] == NULL || clients[1] == NULL) {
            fprintf(stderr, "popen() failed");
            return EXIT_FAILURE;
        }
        std::vector<int> values(graph.size(),0);
        std::vector<int> states(graph.size(),0);
        
        for (int i = 1; i <= blockedCells; i++) {
            int x;
            do {
                x = rand() % graph.size();
            } while (states[x] != 0);
            
            states[x] = -1;
            
            std::stringstream buff;
            buff << x;
            if (debug == true) {
                fprintf(stderr, "Server wrote:%s\n", buff.str().c_str());
                fflush(stderr);
            }
            for (int id = 0; id < 2; id++) {
                fprintf(clients[id], "%s\n", buff.str().c_str());
                fflush(clients[id]);
            }
        }
        fprintf(clients[first], "Start\n");
        fflush(clients[first]);
        if (debug == true) {
            fprintf(stderr, "Server wrote: Start\n");
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
            assert(0 <= indx  && indx < graph.size() && "Server :Index out of boundaries");
            assert(values[indx] == 0 && states[indx] != -1 &&
                   "Server :Not a valid move, the cell is not empty");
            assert(1 <= val && val <= moves &&
                   "Server: Not a valid move, value is wrong");
            if (debug == true) {
                fprintf(stderr, "Server : turn %d\n", i);
                fflush(stderr);
            }
            states[indx] = turn;
            values[indx] = val;
            
            turn = turn ^ 1;
            
            fprintf(clients[turn], "%s", buff);
            fflush(clients[turn]);
        }
        
        int emptyCell = GetEmptyCell(states, values);
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
    for (const auto& score : points) {
        fprintf(stdout, "%d %d\n", score.first, score.second);
        fflush(stdout);
    }
    return EXIT_SUCCESS;
}
