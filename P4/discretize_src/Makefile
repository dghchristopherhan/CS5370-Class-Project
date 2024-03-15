# Makefile for discretizing data to low=-1, neutral=0, high=1
# Sharlee Climer
# August 2022


CC	= g++
CFLAGS 	= -g
TARGET	= discretize
OBJS	= discretize.o calc.o

$(TARGET):	$(OBJS)
		$(CC) -o $(TARGET) $(OBJS)

discretize.o:	discretize.cpp discretize.h timer.h calc.h
		$(CC) $(CFLAGS) -c discretize.cpp

calc.o:		calc.cpp calc.h
		$(CC) $(CFLAGS) -c calc.cpp

clean:
		/bin/rm -f *.o $(TARGET)
