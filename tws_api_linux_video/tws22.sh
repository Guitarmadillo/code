#!/bin/bash 
#
# This script uses the clang compiler instead which is recommended over gcc by
# some development teams I follow 
# clang seems a lot faster when building with -O0
#
# This script compiles JanBoonen Library and Project source into object code 
# Then links them at compile time to generate the executable
# ---------------------------------------------------------

# Paths to Library source code and project source directories
lib_path="$HOME/dev/twsapi/TwsApiCpp"
src_path="$HOME/dev/twsapi/TwsApiCpp/TwsApiC++/Test/Src"

# Set optimization level (0 for fast compile, 2 or 3 for production)
op_lvl="0"

#Source code filenames we are building 
lib_src="TwsApiL0"
src="Test"

#Target directory
target_path="$HOME/dev/twsapi"

#Output file name
output_file="bash_st_test"

# Go to library source code directory 
cd $lib_path/TwsApiC++/Src

echo "Building $lib_src.cpp into object code"
#Compile the library into object code
clang++ -march=x86-64 -O$op_lvl -std=gnu++17 -fPIC $lib_src.cpp \
-I$lib_path/TwsApiC++/Api -I$lib_path/TwsApiC++/Src \
-I$lib_path/source/PosixClient/Shared -I$lib_path/source/PosixClient/src \
-c -o $lib_src.o -Wno-switch

# Go to executable source code directory 
cd $src_path

echo "Building $src.cpp into object code"
#Compile the project source into object code
clang++ -march=x86-64 -O$op_lvl -std=gnu++17 $src.cpp -I$lib_path/TwsApiC++/Api \
-I$lib_path/TwsApiC++/Src -I$lib_path/source/PosixClient/Shared \
-I$lib_path/source/PosixClient/src -c -o $src.o \

# Go back to working directory  
cd $target_path

# Build the executable using both object files
clang++ -march=x86-64 -O$op_lvl -std=gnu++17 $lib_path/TwsApiC++/Test/Src/$src.o \
$lib_path/TwsApiC++/Src/$lib_src.o -o $output_file \
-lpthread

# Optional Run
echo "Running $output_file"
./$output_file

echo 
