//
//  main.cpp
//  Client
//
//  Created by Gemene Narcis on 13/08/2017.
//  Copyright © 2017 Gemene Narcis. All rights reserved.
//

#include <iostream>
#include <cstring>
int main(int argc, const char * argv[]) {
    char buff[100];
    while(true)
    {
        std::cin >> buff;
        if(strcmp(buff,"stop") == 0)
            return  0;
        std::cout << "Client_Received:" << buff << std::endl << std::flush;
    }
    return 0;
}
