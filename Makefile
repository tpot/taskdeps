LIBS = -lgecodeflatzinc -lgecodedriver -lgecodesearch -lgecodeminimodel \
       -lgecodeset -lgecodefloat -lgecodeint -lgecodekernel -lgecodesupport

CFLAGS = -std=c++11

taskdeps: taskdeps.cpp
	g++ $(CFLAGS) -o taskdeps taskdeps.cpp $(LIBS)
