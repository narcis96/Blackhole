#include <cstdio>
#include <string>

#include <iostream>
int main(int argc, const char * argv[])
{
    std::cerr <<"sample started\n";
    const char* player = std::string("./testc++ [1,]").c_str();
    FILE *client = popen(player, "r+");
    char buff[100];
    fgets(buff, 100, client);
    int status = pclose(client);
    if (status == -1) {
        fprintf(stderr, "Error reported by pclose()");
        return EXIT_FAILURE;
    }
    return 0;
    
}


