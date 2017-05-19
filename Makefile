bpCount: bpMatrix.h bpCount.h bpCount.cpp bible
	g++-6 -c bpCount.cpp -I . 
bible: bible.h bible.cpp
	g++-6 -c bible.cpp -I .
bible_parse: bible_parse.cpp bpMatrix.h bpCount
	g++-6 bible_parse.cpp -o bible_parse -O3 bpCount.o bible.o -I.
