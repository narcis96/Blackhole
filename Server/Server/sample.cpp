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
    const char* player = std::string("./testc++ -weights [1,4]").c_str();
    FILE *client = popen(player, "r+");
    char buff[100];
    fgets(buff, 100, client);
    return 0;
    
}


