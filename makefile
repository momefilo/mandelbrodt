mandelbrodt: mandel.o _Display.o _Userinterface.o _Apple.o
	g++ -o mandelbrodt mandel.o _Display.o _Userinterface.o _Apple.o
	make clean

mandel.o: mandel.cpp
	g++ -lpthread -c mandel.cpp

_Display.o: include/_Display.cpp
	g++ -c include/_Display.cpp
	
_Userinterface.o: include/_Userinterface.cpp
	g++ -c include/_Userinterface.cpp

_Apple.o: include/_Apple.cpp
	g++ -c include/_Apple.cpp
	
clean:
	rm -f mandel.o
	rm -f _Display.o
	rm -f _Userinterface.o
	rm -f _Apple.o
