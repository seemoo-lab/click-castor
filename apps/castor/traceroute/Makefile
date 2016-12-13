PROGRAM := traceroute
TEST := $(PROGRAM)_test
DEPS := $(PROGRAM).cc cli.cc route.cc route_entry.cc

CXX := g++
CXXFLAGS := -std=c++11

all: $(PROGRAM) $(PROGRAM)_test

$(PROGRAM): $(DEPS) ../castor_socket.hh
	$(CXX) $(CXXFLAGS) $(DEPS) -o $@

$(TEST): $(TEST).cc ../castor_socket.hh
	$(CXX) $(CXXFLAGS) $@.cc -o $@

clean:
	rm -rf $(PROGRAM)
	rm -rf $(TEST)

.PHONY: clean