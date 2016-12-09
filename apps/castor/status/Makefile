PROGRAM := castor_status

CSCLIENT_DIR ?= ../../apps/csclient/
CSCLIENT_CC := $(CSCLIENT_DIR)csclient.cc
CSCLIENT_HH := $(CSCLIENT_DIR)csclient.hh

$(PROGRAM): $(CSCLIENT_HH) $(CSCLIENT_CC)
	g++ -Os -o $@ $(CSCLIENT_CC) $@.cc

clean:
	rm -f $(PROGRAM)

.PHONY: clean