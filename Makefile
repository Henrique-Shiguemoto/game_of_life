# you can change to gcc, clang, cl, etc
COMPILER=g++
COMPILER_FLAGS=-g -Wall -Wextra -pedantic -std=c++11 -Wno-unused-parameter
LINKER_FLAGS=-lmingw32 -lSDL2main -lSDL2
INCLUDE_PATH=-Isrc/include
LIBRARY_PATH=-Lsrc/lib

# put all your source files here (put a space after each one to add another one)
SOURCE_FILES=src/main.cpp
SOURCE_PATH=src
BINARY_FILE=$(SOURCE_PATH)/game.exe
OBJECTS_FILES=main.o

# I'm using gdb for simple stuff but certainly a front-end would be nice
DEBUGGER=gdb

# build everything that needs to be built
all: $(BINARY_FILE)
	@echo "Compiling and Linking done..."

$(BINARY_FILE): $(OBJECTS_FILES)
	$(COMPILER) $(OBJECTS_FILES) -o $(BINARY_FILE) $(LIBRARY_PATH) $(LINKER_FLAGS)

%.o:$(SOURCE_PATH)/%.cpp $(SOURCE_PATH)/%.h
	$(COMPILER) $(COMPILER_FLAGS) -o $@ -c $< $(INCLUDE_PATH)

# Just running the program
.PHONY: run
run: $(BINARY_FILE)
	$(BINARY_FILE)

# build from scratch (cleans first)
.PHONY: everything
everything: clean all
	@echo "Full Build Successful..."

# debugging BINARY_FILE
.PHONY: debug
debug: everything
	$(DEBUGGER) $(BINARY_FILE)

# clear unnecessary files
.PHONY: clean
clean:
	del *.o *.exe *.s src\*.o src\*.exe src\*.s