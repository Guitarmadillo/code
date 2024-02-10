:: This script will ssh from a vm back to parent (non windows) machine and grab
:: a source file. 

:: Then it calls a second script which compiles the source and sends the built
:: binary back to the parent linux machine. The purpose of this is to avoid cross
:: compiling, and instead build the windows binaries on windows. 
:: Since SC is still a Windows only software, this is a workflow that is necessary
:: for developers working on a linux machine but compiling apps for windows that
:: can also run on linux through wine like Sierra Chart.

:: When calling this script It is necessary to input the name of the source file 
:: WITHOUT the file extension
:: the script will take care of the .cpp file extension
:: example command: get VT_SCTelegramDrawingAlert
set source=%1
:: change directory to sc working source directory
cd C:\SierraChart\ACS_Source
:: get the source file from host linux coding machine (requires pubkey authentication)
scp -F C:\Users\chris\.ssh\config christian@home:~/dev/sc_studies/ACS_Source/%source%.cpp %source%.cpp
:: compile the code (assumes the proper file name is set from the start)
scstudy_clang_passed.bat %source%
:: this second script will also send the new binary back over to home machine
