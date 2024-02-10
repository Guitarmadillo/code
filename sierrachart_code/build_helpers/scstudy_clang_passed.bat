:: This script is called from the get.bat script where the user
:: will input the name of the source file they want to retrieve from the coding
:: machine to be built on this windows 11 virtual machine.

:: This script builds the source into a DLL that can be read by Sierra Chart.
:: Once complete, it provides the necessary behaviour for copying the binary
:: back to the coding machine and ensures the binary can be copied even when SC
:: is running by using the UDP interface to release and allow DLLs.
set source=%1
set dll=%source%_64.dll
cd C:\SierraChart\ACS_Source\ 
:: release DLLs so we can build the binary (in case SC is also running on this VM)
sc-release
:: Build REGULAR SC STUDIES
REM clang++ -D _WIN64 -std=c++17 -mtune=x86-64 -O2 -shared -static -IC:\boost_1_84_0 %source%.cpp -o C:\SierraChart\Data\%dll% -lGdi32  
:: Build TELEGRAM STUDY INCLUDING CURL static linking
clang++ -D _WIN64 -DCURL_STATICLIB -std=c++17 -mtune=x86-64 -O2 -shared -IC:\boost_1_84_0 -IC:\vcpkg-master\installed\x64-windows-static\include -LC:\OpenSSL\curl-7.86.0_2-win64-mingw\lib -LC:\vcpkg-master\installed\x64-windows-static\lib  %source%.cpp -o C:\SierraChart\Data\%dll% -luser32 -lAdvapi32 -lwsock32 -lWs2_32 -lGdi32 -llibcurl -llibssl -llibcrypto -lCrypt32 -lbcrypt -lzlib   
:: Allow load the new DLL (in case SC is also running on this VM)
sc-allow		
:: call release dll through ssh on home machine
ssh home "wine /mnt/SierraChart/ACS_Source/sc-release.exe"
:: go to where the new binary is on this virtual machine
cd C:\SierraChart\Data
:: copy the new binary back over to home machine to run on wine/linux
scp -F C:\Users\chris\.ssh\config %source%_64.dll christian@home:/mnt/SierraChart/Data/%source%_64.dll
:: call allow dll function through ssh on home machine
ssh home "wine /mnt/SierraChart/ACS_Source/sc-allow.exe"
