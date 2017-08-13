//
//  main.cpp
//  Server
//
//  Created by Gemene Narcis on 13/08/2017.
//  Copyright © 2017 Gemene Narcis. All rights reserved.
//
#include <stdio.h>
#include <string>


int main(int argc, const char * argv[])
{
    if(argc < 3)
    {
        fprintf( stderr, "Not enough parameters");
    }
    FILE *clients[2];
    int ind = 0;
    clients[0] = popen(argv[1], "r+");
    clients[1] = popen(argv[2], "r+");
    
    if (clients[0] == NULL || clients[1] == NULL)
    {
        fprintf(stderr, "popen() failed");
        return -1;
    }
    
    while (true)
    {
        printf("Waiting input:\n");
        fflush(stdout);
        
        char buff[100];
        fgets (buff, 100, stdin);
        
        printf("Server send: %s", buff);
        fflush(stdout);
        fprintf(clients[0], "%s", buff);
        fflush(clients[0]);
        if(fgets(buff, 100, clients[0]) == NULL)
        {
            break;
        }
        printf("Server received (0): %s", buff);
        fflush(stdout);
        
        fprintf(clients[ind], "%s", buff);
        fflush(clients[ind]);
        
        if(fgets(buff, 100, clients[ind]) == NULL)
        {
            break;
        }
        
        printf("Server received (1): %s", buff);
        fflush(stdout);
    }
    
    
    return 0;
    
}


