
CXX = clang++
CXXFLAGS = -std=c++0x -c -Wall
CMDLINE =
LDFLAGS =
HEADERS = lexical.hpp syntax.hpp semantic_ir.hpp node.hpp \
		  symbol_table.hpp stack_vm.hpp  vm_symbol.hpp repl.hpp debug.hpp
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
	$(CXX) $(LDFLAGS) $(OBJECTS) -o $(EXECUTABLE)

run_without_gosh:  $(EXECUTABLE)
	./$(EXECUTABLE)

run: $(EXECUTABLE)
	gosh $(INPUT_FILE)
	./$(EXECUTABLE)

gdb_run:
	$(CXX) -std=c++11 -ggdb3 -o $(DEBUG_EXECUTABL) main.cpp
	gdb -x cmd.gdb

clean:
	rm -f *.o *.gch $(EXECUTABLE)
