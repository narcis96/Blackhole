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
#ifdef __APPLE__
#include <sys/syslimits.h>
#endif

#ifdef __linux__
#include <linux/limits.h>
#endif

bool IsInterior(const unsigned long& dim, const int& i, const int& j) {
    return (0 <= i && i < dim && 0 <= j && j <= i);
}

std::pair<int, int> GetPoints(const std::vector<std::vector<int> >& states, const std::vector<std::vector<int> >& values, const int& i, const int& j) {
    static const int dx[6] = { 0, 0, -1, 1, -1, 1 };
    static const int dy[6] = { 1, -1, 0, 0, -1, 1 };
    
    int size = sizeof(dx) / sizeof(*dx);
    std::pair<int, int> points(75, 75);
    for (int pos = 0; pos < size; ++pos) {
        int x = i + dx[pos];
        int y = j + dy[pos];
        if (IsInterior(states.size(), x, y)) {
            if (states[x][y] == 0) {
                points.first += values[x][y];
                points.second -= values[x][y];
            }
            else {
                points.second += values[x][y];
                points.first -= values[x][y];
            }
        }
    }
    return points;
}
std::pair<int, int> GetEmptyCell(const std::vector<std::vector<int> >& states, const std::vector<std::vector<int> >& values) {
    int count = 0;
    std::pair<int, int> emptyCell;
    for (int i = 0; i < states.size(); i++) {
        for (int j = 0; j <= i; j++) {
            if (values[i][j] == 0 && states[i][j] != -1) //the cell is empty
            {
                count += 1;
                assert(count == 1 && "More that one cell empty");
                emptyCell = { i, j };
            }
        }
    }
    return emptyCell;
}

int main(int argc, const char* argv[]) {
    const int blockedCells = 5;
    const unsigned long dimension = 8;
    srand((unsigned int)time(0));
    auto ConvertToIndex = [](const char& line, const char& pos) -> std::pair<int, int> {
        return { line - 'A' + pos - '1', pos - '1' };
    };
    
    auto ConvertToCell = [](const int& i, const int& j) -> std::pair<char, char> {
        return { i + 'A' - j, j + '1' };
    };
    std::pair<int, int> points[2];
    const char* player1 = argv[1];
    const char* player2 = argv[2];
    for (int first = 0; first < 2; first++) {
        clock_t start = clock();
        FILE* clients[2];
        clients[0] = popen(player1, "r+");
        clients[1] = popen(player2, "r+");
        
        if (clients[0] == NULL || clients[1] == NULL) {
            fprintf(stderr, "popen() failed");
            return EXIT_FAILURE;
        }
        std::vector<std::vector<int> > values(dimension, std::vector<int>(dimension, 0));
        std::vector<std::vector<int> > states(dimension, std::vector<int>(dimension, 0));
        
        for (int i = 1; i <= blockedCells; i++) {
            int x, y;
            do {
                x = rand() % dimension;
                y = rand() % (x + 1);
            } while (states[x][y] != 0);
            
            states[x][y] = -1;
            
            std::pair<char, char> pos = ConvertToCell(x, y);
            std::stringstream buff;
            buff << pos.first << pos.second;
            
            //fprintf (stderr, "Server wrote:%s\n",buff.str().c_str());
            //fflush (stderr);
            
            for (int id = 0; id < 2; id++) {
                fprintf(clients[id], "%s\n", buff.str().c_str());
                fflush(clients[id]);
            }
        }
        fprintf(clients[first], "Start\n");
        fflush(clients[first]);
        int turn = first;
        char buff[100];
        for (int i = 1; i <= 30; i++) {
            if (fgets(buff, 100, clients[turn]) == NULL) {
                fprintf(stderr, "fgets() failed at step %d\n", i);
                fflush(stderr);
                return EXIT_FAILURE;
            }
            char line, pos;
            int val, x, y;
            sscanf(buff, "%c%c=%d", &line, &pos, &val);
            std::tie(x, y) = ConvertToIndex(line, pos);
            
            assert(IsInterior(dimension, x, y) && "Server :Index out of boundaries");
            assert(values[x][y] == 0 && states[x][y] != -1 && "Server :Not a valid move, the cell is not empty");
            assert(1 <= val && val <= 15 && "Server: Not a valid move, value is wrong");
            
            //fprintf (stderr, "Server : turn %d\n", i);
            //fflush (stderr);
            
            states[x][y] = turn;
            values[x][y] = val;
            
            turn = turn ^ 1;
            
            fprintf(clients[turn], "%s", buff);
            fflush(clients[turn]);
        }
        
        std::pair<int, int> emptyCell = GetEmptyCell(states, values);
        
        //fprintf (stderr, "Server : empty cell (%d,%d)\n", emptyCell.first, emptyCell.second);
        //fflush (stderr);
        
        points[first] = GetPoints(states, values, emptyCell.first, emptyCell.second);
        
        for (auto& client : clients) {
            int status = pclose(client);
            if (status == -1) {
                fprintf(stderr, "Error reported by pclose()");
                return EXIT_FAILURE;
            }
        }
        fprintf(stderr, "time =  %f\n", (double)(clock()-start)/CLOCKS_PER_SEC);
        fflush(stderr);
    }
    //fprintf (stderr, "Time:\n1:%Lf 2:%Lf\n", timeElapsed[0], timeElapsed[1]);
    //fflush (stderr);
    for (const auto& score : points) {
        fprintf(stdout, "%d %d\n", score.first, score.second);
        fflush(stdout);
    }
    return EXIT_SUCCESS;
}
