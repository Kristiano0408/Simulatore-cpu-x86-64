#include "../include/helpers/result.hpp"
#include <iostream>

int main() {
    // Create an instance of Error_Event_Info
    

    std::cout << sizeof(std::string) << '\n';
    std::cout << sizeof(Error_Event_Info) << '\n';
    std::cout << alignof(Error_Event_Info) << '\n';
    std::cout << sizeof(Result) << '\n';
    std::cout << alignof(Result) << '\n';

    return 0;
}