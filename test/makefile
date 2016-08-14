.PHONY: all clean

all:
	g++-5 -std=c++14 main-tm.cpp -L../library -lrbt-tm -o main-tm -fgnu-tm
	g++-5 -std=c++14 main-me.cpp -o main-me -fgnu-tm
	g++-5 -std=c++14 main-sl.cpp -o main-sl -fgnu-tm

clean:
	rm -rf main-tm main-me main-sl

