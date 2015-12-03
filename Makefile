#!/bin/bash

all:
	g++ MCTS_go99.cpp -o MCTS_go99.exe -O3
	
test:
	./MCTS_go99.exe -display
	
MCTS:
	g++ MCTS_go99.cpp -o MCTS_go99.exe -O3
	
MCS:
	g++ MCS_go99.cpp -o MCS_go99.exe -O3
	
random:
	g++ random_go99.cpp -o random_go99.exe -O3