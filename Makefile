CC=gcc # define the compiler to use
TARGET=mandelbg # define the name of the executable
SOURCES=main.c fractals/mandelbrot.c render.c util.c
CFLAGS=-O3
LFLAGS=-lm
LIBS = -lX11 -lXrandr

# define list of objects
COBJECTS=$(SOURCES:.c=.o)

# the target is obtained linking all .o files
all: $(SOURCES) $(TARGET)

$(TARGET): $(COBJECTS)
	$(CC) $(LFLAGS) $(COBJECTS) -o $(TARGET) $(LIBS)

purge: clean
	rm -f $(TARGET)

clean:
	rm -f *.o
