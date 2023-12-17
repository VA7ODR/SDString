# SDString
Copyright (c) 2023 James Baker

SDString or (S)ecure (D)elete String, is a CPP (C++17 +) string class interchangeable with std::string and std::wstring which zeros its allocated memory before freeing it. It is intended to thwart memory sniffer malware from reading secret data from freed memory, as per PCI and other security recommendations.

## Use
Import into your project using:

    git submodule add https://github.com/VA7ODR/SDString.git

Then write your application like this:

    #include <iostream>
    #include "SDString/sdstring.hpp"

    int main()
    {
        sdstring hi("Hello, World!");
      
        std::cout << hi << std::endl;
      
        return 0;
    }
