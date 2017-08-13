//
//  main.cpp
//  Server
//
//  Created by Gemene Narcis on 13/08/2017.
//  Copyright © 2017 Gemene Narcis. All rights reserved.
//
#include <stdio.h>
#include <string>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <cassert>
#ifdef __APPLE__
#include <sys/syslimits.h>
#endif

#ifdef __linux__
#include <linux/limits.h>
#endif


int main(int argc, const char * argv[]) {
    const int blockedCells = 5;
    srand((unsigned int)time(0));
    auto ConvertToIndex = [](const char &line, const char &pos) -> std::pair < int, int >
    {
        return{ line - 'A' + pos - '1',pos - '1' };
    };
    
    auto ConvertToCell = [](const int &i, const int &j) -> std::pair < char, char >
    {
        return{ i + 'A' - j, j + '1' };
    };
    auto IsInterior = [](const int &i, const int &j) ->bool
    {
        return (0 <= i && i < 8 && 0 <= j && j <= i);
    };
    int first = 0;
    //for(int first = 0; first < 2; first++)
    {
        std::set<std::pair<int,int>>m;
        FILE *clients[2];
        clients[0] = popen("./mainPlayer", "r+");
        clients[1] = popen("./mainPlayer", "r+");

        std::vector< std::vector<int> > values(8, std::vector<int>(8,0));
        std::vector< std::vector<int> > states(8, std::vector<int>(8,0));

        
        if (clients[0] == NULL || clients[1] == NULL)
        {
            fprintf (stderr, "popen() failed");
            return -1;
        }
        
        for (int i = 1; i <= blockedCells; i++)
        {
            int x, y;
            do
            {
                x = rand() % 8;
                y = rand() % (x+1);
            }
            while(states[x][y] != 0);
            
            states[x][y] = -1;
            
            std::pair < char , char > pos = ConvertToCell(x, y);
            std::stringstream buff;
            buff << pos.first << pos.second;

            fprintf (stderr, "Server wrote:%s\n",buff.str().c_str());
            fflush (stderr);

            for(int id = 0; id < 2; id++)
            {
                fprintf(clients[id], "%s\n", buff.str().c_str());
                fflush(clients[id]);
            }
        }
        fprintf(clients[first], "Start\n");
        fflush(clients[first]);
        int turn = first;
        char buff[100];
        for(int i = 1; i <= 31; i++)
        {
            if(fgets(buff, 100, clients[turn]) == NULL)
            {
                fprintf (stderr, "fgets() failed at step %d\n", i);
                fflush (stderr);
                return -2;
            }
            
            char line, pos;
            int val;
            sscanf(buff, "%c%c=%d", &line, &pos, &val);
            int x, y;
            std::tie(x, y) = ConvertToIndex(line, pos);
            
            assert(IsInterior(x,y) && "Server :index out of boundaries");
            assert(states[x][y] == 0 && "Server :Not a valid move, the cell is not empty");
            
            fprintf (stderr, "Server : turn %d\n", i);
            fflush (stderr);
            states[x][y] = turn;
            values[x][y] = val;
            if (i <= 30)
            {
                turn = turn ^ 1;

                fprintf(clients[turn], "%s", buff);
                fflush(clients[turn]);
            }
        }
        for(int id = 0; id < 2; id++)
        {
            int status = pclose(clients[id]);
            if (status == -1)
            {
                fprintf( stderr, "Error reported by pclose()");
                
            }
            else
            {
                /* Use macros described under wait() to inspect `status' in order
                 to determine success/failure of command executed by popen() */
            }
        }
        
    }
    

    return 0;
}
