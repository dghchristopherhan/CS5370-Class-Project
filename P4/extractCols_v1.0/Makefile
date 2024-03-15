# Makefile for extracting columns of data from a file
# Sharlee Climer
# July, 2010


CC	= g++
CFLAGS 	= -g
TARGET	= extractCols
OBJS	= extractCols.o

$(TARGET):	$(OBJS)
		$(CC) -o $(TARGET) $(OBJS)

extractCols.o:	extractCols.cpp extractCols.h timer.h
		$(CC) $(CFLAGS) -c extractCols.cpp

clean:
		/bin/rm -f *.o $(TARGET)
