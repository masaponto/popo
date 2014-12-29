
CXX = clang++
CXXFLAGS = -std=c++11 -c -Wall
CMDLINE =
LDFLAGS =
HEADERS = lexical.hpp syntax.hpp semantic.hpp debug.hpp
SOURCES = main.cpp
OBJECTS = main.o
EXECUTABLE = popo

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CMDLINE) $< -o $@

all: $(EXECUTABLE) $(OBJECTS)

main.o: $(HEADERS)

$(EXECUTABLE): $(OBJECTS)
	ctags -R .
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

test: $(SOURCES) $(EXECUTABLE)
	$(CXX) -std=c++11 -ggdb3 -o $(EXECUTABLE) main.cpp


gdb_run: test
	gdb -x cmd.gdb

clean:
	rm -f *.o *.gch $(EXECUTABLE)

