:: This script is used on windows for building sierra chart udp commands into executables
:: These udp executables are used when building SC studies while SC is running.
:: DLL's must be released from SC before building or it will not allow the build
:: These are only necessary when locally building DLL's while SC is running and
:: very helpful when doing quick compiles for debug workflow.

:: go to directory of source
cd C:\SierraChart\ACS_Source\ 
:: build release dll exe 
clang++ -D _WIN64 -std=c++17 -mtune=x86-64 -O2 ReleaseDLL_udp.cpp -o sc-release.exe -lwsock32 -lws2_32

:: build allow dll exe
clang++ -D _WIN64 -std=c++17 -mtune=x86-64 -O2 AllowDLL_udp.cpp -o sc-allow.exe -lwsock32 -lws2_32
