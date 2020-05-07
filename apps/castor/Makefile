PROGRAMS := status

all: $(PROGRAMS)

$(PROGRAMS):
	make -C $@

clean:
	for program in $(PROGRAMS); do \
		$(MAKE) -C $$program $@; \
	done

.PHONY: all $(PROGRAMS) clean