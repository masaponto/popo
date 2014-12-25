
CXX = clang++
CXXFLAGS = -std=c++11 -c -Wall
CMDLINE =
LDFLAGS =
SOURCES = main.cpp lexer.hpp parser.hpp
OBJECTS = main.o
EXECUTABLE = popo


.cpp.o:
	$(CXX) $(CXXFLAGS) $(CMDLINE) $< -o $@


all: $(EXECUTABLE) $(OBJECTS)

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

test: $(SOURCES)
	$(CXX) -std=c++11 -ggdb3 -o $(EXECUTABLE) main.cpp


gdb_run: test
	gdb -x cmd.gdb

clean:
	rm -f *.o *.gch $(EXECUTABLE)

