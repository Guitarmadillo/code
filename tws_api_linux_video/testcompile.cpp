#include <cstdlib>
#include <iostream>
#include <thread>
#include <unistd.h>			// usleep(), in microseconds
	#define Sleep( m ) usleep( m*1000 )

// Hello this is going to be a test program
int main()
{
	std::cout << "This is a test program" << std::endl;
	for(int i = 0; i < 10; i++)
	{
		std::cout << "How many employees do we need? The answer is: "
		<< i << std::endl;	
		Sleep(777);
	}
	return 0;
}
