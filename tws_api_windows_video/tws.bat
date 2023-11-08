:: This file is used for building a basic C++ TWS API Console application on Microsoft Windows 
:: As shown in this video by VerrilloTrading: https://www.youtube.com/watch?v=ZxwdTgMY44g

:: This method uses dynamic linking as shown in the video, which requires the library to be built
:: separately. This method can result in faster compile times however since the library is lightweight, 
:: this is most likely negligible. This example uses MinGW64 (g++ for Windows) compiler but I prefer clang. 

:: Set the name of the source file and output executable 
set source=Test
set exe=%source%_64

:: This is used to kill the executable if it is running while you run this script.
:: useful for quick debugging.

TASKLIST | FINDSTR /I "%exe%"
IF ERRORLEVEL 1 (echo "%exe%" is not running) ELSE (taskkill /F /IM %exe%.exe)

:: go to the source directory 
cd C:\TwsApiCpp-master\TwsApiC++\Test\Src

:: Compile the source code, keep in mind this method reuqires the library to stay in the specified directory
:: For an example of static linking the library with project source, view the Linux video for TWS C++ API.
:: Link to that here: https://youtu.be/iIGA0x8mImY

:: boost is not necessary and if you do not have boost just remove the include for boost 
g++ -D _WIN64 -march=x86-64 -O0 -static -s -std=gnu++20 %source%.cpp -IC:\boost_1_82_0 -IC:\TwsApiCpp-master\TwsApiC++\Api -IC:\TwsApiCpp-master\TwsApiC++\Src -IC:\TwsApiCpp-master\source\PosixClient\Shared -IC:\TwsApiCpp-master\source\PosixClient\src -o C:\TwsApiCpp-master\TwsApiC++\Api\%exe% C:\TwsApiCpp-master\TwsApiC++\Api\TwsCppApi_64.dll -Wno-deprecated 
cd C:\TwsApiCpp-master\TwsApiC++\Api

:: This is another windows specific command used for killing the command window.
:: It should kill the command window that the previous application was running
:: in. before it was killed using the earlier command.
taskkill /F /IM cmd.exe /FI "WINDOWTITLE eq C:\WINDOWS\system32\cmd.exe*"
start cmd /k %exe%
