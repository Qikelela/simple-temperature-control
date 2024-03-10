CXX ?= g++

.PHONY: all clean install uninstall

all: temp

temp:
	$(CXX) -O2 temp.cpp -o temp
clean:
	rm -f temp
uninstall:
	rm -rf /usr/local/bin/temp
install:
	install ./temp /usr/local/bin

