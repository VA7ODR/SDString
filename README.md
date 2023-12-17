# SDString
Secure Delete String. A CPP string class interchangable with std::string and std::wstring which zeros it's allocated memory before freeing it. It is intended to thwart memory sniffer malware from reading secret data from freed memory, as per PCI recommendataions among others.
