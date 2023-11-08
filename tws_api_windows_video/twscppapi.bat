:: This script is for building the C++ TWS API Library as a DLL to be
:: dynamically linked when compiling your project source code. There is nothing
:: technically wrong with this method, but since making that video, for this
:: specific library I would prefer to use the static linking method.

:: Both static and dynamic linking build methods are shown in this updated
:: video where I built the TWS C++ API under a Linux system.

:: Video by VerrilloTrading building TWS C++ API on Linux: https://youtu.be/iIGA0x8mImY


:: Video by VerrilloTrading where this script was used on Microsoft Windows: https://youtu.be/ZxwdTgMY44g

:: Set the name of the source file and output executable 
set source= TwsApiL0
set exe=%source%_64

:: This is used to kill the executable if it is running while you run this script.
:: useful for quick debugging.
TASKLIST | FINDSTR /I "%exe%"
IF ERRORLEVEL 1 (echo "%exe%" is not running) ELSE (taskkill /F /IM %exe%.exe)

:: go to the library source directory
cd C:\TwsApiCpp-master\TwsApiC++\Src

:: g++ compiler line to build library source code into a dynamic link library.
:: This line is specific to the g++ or MinGW compiler and will need to be slightly
:: adjusted to use with other toolchains like clang.

:: You may get a warning about certain macros in one of the source code files being redefined.
:: You can go in and remove the redefinitions or leave them as is. That is source code from the 
:: JanBoonen repository.
g++ -D _WIN64 -D CorrectAfxWinIncluded -shared -static -static-libgcc -static-libstdc++ -s -std=gnu++20 %source%.cpp -IC:\TwsApiCpp-master\TwsApiC++\Api -IC:\TwsApiCpp-master\TwsApiC++\Src -IC:\TwsApiCpp-master\source\PosixClient\Shared -IC:\TwsApiCpp-master\source\PosixClient\src -LC:\TwsApiCpp-master\TwsApiC++\Api -Wl,--no-as-needed -o C:\TwsApiCpp-master\TwsApiC++\Api\TwsCppApi_64.dll -Wno-deprecated -lwsock32 -lws2_32 -lpthread
