
CXX = clang++
CXXFLAGS = -std=c++11 -c -Wall
CMDLINE =
LDFLAGS =
HEADERS = lexical.hpp syntax.hpp semantic.hpp debug.hpp
SOURCES = main.cpp
OBJECTS = main.o
INPUT_FILE = test.scm
EXECUTABLE = popo
DEBUG_EXECUTABL = popo_debug

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CMDLINE) $< -o $@

all: $(EXECUTABLE) $(OBJECTS)

main.o: $(HEADERS)

$(EXECUTABLE): $(OBJECTS)
	ctags -R .
	gosh $(INPUT_FILE)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

#TODO: make not override $(DEBUG_EXECUTABL)
debug: $(SOURCES) $(HEADERS)
	$(CXX) -std=c++11 -ggdb3 -o $(DEBUG_EXECUTABL) main.cpp

gdb_run: $(DEBUG_EXECUTABL)
	gdb -x cmd.gdb

clean:
	rm -f *.o *.gch $(EXECUTABLE)

