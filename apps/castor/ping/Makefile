PROGRAM := ping
DEPS := $(PROGRAM).cc cli.cc
TEST := $(PROGRAM)_test

CXX := g++
CXXFLAGS := -std=c++11

all: $(PROGRAM) $(PROGRAM)_test

$(PROGRAM): $(DEPS)
	$(CXX) $(CXXFLAGS) $(DEPS) -o $@

$(TEST): $(TEST).cc
	$(CXX) $(CXXFLAGS) $@.cc -o $@

clean:
	rm -rf $(PROGRAM)
	rm -rf $(TEST)

.PHONY: clean