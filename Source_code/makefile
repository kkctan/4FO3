CC = pgCC
CXX = pgCC
PROGRAM_NAME=mandelbulb
CXXFLAGS= -fast -acc -ta=tesla,cc30 -Minfo=all -O2 
#LDFLAGS= -acc

$(PROGRAM_NAME): main.o 
	$(CXX) $(CXXFLAGS) -o $@ $^


clean:
	rm -f *.o $(PROGRAM_NAME) $(EXEEXT) *~ imageBulb.bmp
