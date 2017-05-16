bpCount: bpMatrix.h bpCount.h bpCount.cpp
	g++-6 -c bpCount.cpp -I . 
bible_parse: bible_parse.cpp bpMatrix.h bpCount
	g++-6 bible_parse.cpp -o bible_parse -O3 bpCount.o -I.
