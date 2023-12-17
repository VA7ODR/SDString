# SDString
Secure Delete String. A CPP string class interchangable with std::string and std::wstring which zeros it's allocated memory before freeing it. It is intended to thwart memory sniffer malware from reading secret data from freed memory, as per PCI recommendataions among others.

## Use
Import into your project using:

    git submodule add https://github.com/VA7ODR/SDString.git

Then write your application like this:

    #include <iostream>
    #include "SDString/sdstring.hpp"

    int main()
    {
        sdstring hi("Hello, World!");
      
        std::court << hi << std::endl;
      
        return 0;
    }
