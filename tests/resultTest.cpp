#include "../include/helpers/result.hpp"
#include <iostream>

int main() {
    // Create an instance of Error_Event_Info
    std::string m = "";

    std::cout << sizeof(m) << std::endl;
    std::cout << sizeof(Error_Event_Info) << std::endl;
    std::cout << alignof(Error_Event_Info) << std::endl;
    std::cout << sizeof(Result) << std::endl;
    std::cout << alignof(Result) << std::endl;

    return 0;
}