CC = clang

INCLUDES = -I./include
LIBDIR = -L./lib

LIBS = -lraylib -lopengl32 -lgdi32 -lwinmm -Wall -Wextra #-Werror#-mwindows

SRC = main.c
TARGET = main.exe

$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(INCLUDES) $(LIBDIR) $(LIBS)

clean:
	del $(TARGET)
