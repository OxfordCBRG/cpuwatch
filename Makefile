all : cpuwatch
cpuwatch : cpuwatch.cpp 
	g++ -O3 -Wall -Wextra -Wpedantic -Werror -std=c++20 -o cpuwatch cpuwatch.cpp 
clean :
	rm cpuwatch
