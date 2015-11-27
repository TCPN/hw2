#!/bin/bash

all:
	g++ MCS_go99.cpp -o MCS_go99.exe -O3
	
random:
	g++ random_go99.cpp -o random_go99.exe -O3