all: compile 

compile: ./src/index.cpp
			g++ ./src/index.cpp -O3 -o program.exe