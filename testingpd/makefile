all:	cpp2pd

cpp2pd:	cpp2pd.o pdBase.o pdObject.o
	g++ main.o factorial.o hello.o -o hello

cpp2pd.o:	cpp2pd.cpp
	g++ -c cpp2pd.cpp

pdBase.o:	pdBase.cpp
	g++ -c pdBase.cpp

pdObject.o:	pdObject.cpp
	g++ -c pdObject.cpp

clean:
	rm *o cpp2pd
