#include <iostream>
int main(int argc, const char* argv[])
{
    
    std::cerr <<"test started\n";
    for (int i = 0; i < argc ; i++) {
        std::cerr << argv[i]<< "\n";
    }
    return 0;
}
