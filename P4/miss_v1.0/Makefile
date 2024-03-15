# Makefile for missingData
# Sharlee Climer
# December, 2011


CC	= g++
CFLAGS 	= -g
TARGET	= miss
OBJS	= miss.o

$(TARGET):	$(OBJS)
		$(CC) -o $(TARGET) $(OBJS)

miss.o:		miss.cpp miss.h timer.h
		$(CC) $(CFLAGS) -c miss.cpp

clean:
		/bin/rm -f *.o $(TARGET)
