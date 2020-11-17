#Compiler to use
CC=gcc -std=c89 -Wall

#Include Directory(External Libraries, EX: OpenGL, GLEW)
SOURCEDIR = ./
OBJDIR = build
INCLUDE = ~/Files/projects/justcodealready/libraries
MAIN_INCLUDE = ../
LIBDIR = ~/Files/projects/justcodealready/libraries
LIBRARIES =
STATICLIBS = $(addprefix $(LIBDIR)/, $(LIBRARIES))

#Grab sources & headers
SOURCES = $(wildcard $(SOURCEDIR)/*.c)
HEADERS = $(wildcard $(SOURCEDIR)/*.h)

#Use addprefix function to add ./obj/ to each individual obj file name (EX: ./obj/ + Camera.o = ./obj/Camera.o)
OBJECTS = $(addprefix $(OBJDIR)/, $(notdir $(SOURCES:.c=.o)))
MAIN_OBJECTS = $(filter-out ../build/main.o, $(wildcard ../build/*.o))

#Compiler flags
CFLAGS= -Wall -g -I $(INCLUDE) -I $(MAIN_INCLUDE)
LDFLAGS= -L $(LIBDIR) $(STATICLIBS)

#Set executable name
EXECUTABLE = main

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) $(MAIN_OBJECTS) $(LDFLAGS) -o $(EXECUTABLE)

$(OBJDIR)/%.o: $(SOURCEDIR)/%.c 
	$(CC) -c $< $(CFLAGS) -o $@

#Objects depend on object directory
$(OBJECTS): | $(OBJDIR)

#Create new object directory if one doesnt exist already
$(OBJDIR):
	mkdir $(OBJDIR)

.PHONY : clean
clean:
	rm $(OBJECTS)
	rm -rf $(OBJDIR)

.PHONY : test
test: all
	./$(EXECUTABLE); test $$? -eq 0
