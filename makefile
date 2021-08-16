all: sim_mem.hh sim_mem.cpp main.cpp
	g++ sim_mem.cpp main.cpp -o main
all-GDB: sim_mem.hh sim_mem.cpp main.cpp
	g++ -g sim_mem.cpp main.cpp -o main