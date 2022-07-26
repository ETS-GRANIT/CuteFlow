SHELL:=/bin/bash
cute_to_cgns : cute_to_cgns.cpp makefile
	module load gcc cgns/4.1.2;\
	g++ -std=c++11 -O3 -o cute_to_cgns cute_to_cgns.cpp -lcgns
