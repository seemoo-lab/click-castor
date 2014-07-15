#ifndef CLICK_JITTERUNQUEUE_HH
#define CLICK_JITTERUNQUEUE_HH

#include <click/element.hh>
#include <click/task.hh>
#include <click/timer.hh>
#include <click/notifier.hh>

CLICK_DECLS

/**
 * Pulls packets with an added jitter
 */
class JitterUnqueue: public Element {
public:
	JitterUnqueue() CLICK_COLD;
	~JitterUnqueue();

    const char *class_name() const	{ return "JitterUnqueue"; }
    const char *port_count() const	{ return PORTS_1_1; }
    const char *processing() const	{ return PULL_TO_PUSH; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    int initialize(ErrorHandler *) CLICK_COLD;
    void cleanup(CleanupStage) CLICK_COLD;
    void add_handlers() CLICK_COLD;

    bool run_task(Task *);

  private:

    Packet *_p;
    Timestamp _jitter;
    Task _task;
    Timer _timer;
    NotifierSignal _signal;

};

CLICK_ENDDECLS

#endif /* CLICK_JITTERUNQUEUE_HH */
