.PHONY: all clean install uninstall

TARGET = librbt-tm.a
PREFIX = .
NAMEDIR = librbt-tm

all: rbt-tm.cpp
	g++-5 -std=c++14 -Wall -c rbt-tm.cpp -fgnu-tm
	ar rc $(TARGET) rbt-tm.o
	rm -rf *.o

clean:
	rm -rf $(TARGET)

install:
	mkdir $(PREFIX)/$(NAMEDIR)
	install rbt-tm.h $(PREFIX)/$(NAMEDIR)
	install $(TARGET) $(PREFIX)/$(NAMEDIR)

uninstall:
	rm -rf $(PREFIX)/$(NAMEDIR)
