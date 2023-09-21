#!/bin/bash 
#
#This script builds JanBoonen TWS API Library into a .dll
#Then compiles the target source file
#The target output file will be dynamically linked to the library

# Paths to Library and Project source code 
lib_path="$HOME/dev/twsapi/TwsApiCpp"
src_path="$HOME/dev/twsapi/TwsApiCpp/TwsApiC++/Test/Src"

# Source code file we are building 
lib_src="TwsApiL0"
src="Test"

# Set optimization level (0 for fast compile, 2 or 3 for production)
op_lvl="0"

#Target directory
target_path="$HOME/dev/twsapi"

#Output library and file names
output_lib="tws_cpp_linux_api"
output_file="Test"

# Go to library source code directory 
cd $lib_path/TwsApiC++/Src

echo "Building TwsApiL0.cpp into .dll"
#Compile the library
g++ -march=x86-64 -shared -O$op_lvl -s -std=gnu++17 -fPIC $lib_src.cpp \
-I$lib_path/TwsApiC++/Api -I$lib_path/TwsApiC++/Src \
-I$lib_path/source/PosixClient/Shared -I$lib_path/source/PosixClient/src \
-o $target_path/$output_lib.dll -lpthread

# Go to executable source code directory 
cd $src_path

echo "Building $output_file"
#Compile the executable
g++ -march=x86-64 -O$op_lvl -s -std=gnu++17 $src.cpp -I$lib_path/TwsApiC++/Api \
-I$lib_path/TwsApiC++/Src -I$lib_path/source/PosixClient/Shared \
-I$lib_path/source/PosixClient/src -o $target_path/$output_file \
$target_path/$output_lib.dll 

# Go back to working directory  
cd $target_path

echo "Running $output_file"
#Optional Run
./$output_file

