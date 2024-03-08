mandelbrodt: mandel.o _Display.o _Userinterface.o _Apple.o _Colorinterface.o
	g++ -o mandelbrodt mandel.o _Display.o _Userinterface.o _Apple.o _Colorinterface.o
	make clean
#-fsanitize=leak -fsanitize=address -fsanitize=undefined
mandel.o: mandel.cpp
	g++ -c mandel.cpp

#
_Display.o: include/_Display.cpp
	g++ -lpthread -c include/_Display.cpp
	
_Userinterface.o: include/_Userinterface.cpp
	g++ -c include/_Userinterface.cpp

_Apple.o: include/_Apple.cpp
	g++ -c include/_Apple.cpp
	
_Colorinterface.o: include/_Colorinterface.cpp
	g++ -c include/_Colorinterface.cpp
clean:
	rm -f mandel.o
	rm -f _Display.o
	rm -f _Userinterface.o
	rm -f _Apple.o
	rm -f _Colorinterface.o
