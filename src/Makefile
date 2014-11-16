
CXX = clang++
CXXFLAGS = -std=c++11 -c -Wall
CMDLINE =
LDFLAGS =
SOURCES = main.cpp lexer.hpp
OBJECTS = main.o
EXECUTABLE = popo


all: $(EXECUTABLE) $(OBJECTS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(CMDLINE) $< -o $@

$(EXECUTABLE): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)

clean:
	rm -f *.o *.gch $(EXECUTABLE)

