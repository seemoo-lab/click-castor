#include <click/config.h>
#include <click/error.hh>
#include <click/args.hh>
#include <click/glue.hh>
#include "jitterunqueue.hh"
#include <click/standard/scheduleinfo.hh>
#include <click/timestamp.hh>
#include <click/straccum.hh>

CLICK_DECLS

JitterUnqueue::JitterUnqueue() :
		_p(0),
		_task(this),
		_timer(&_task) {
}

JitterUnqueue::~JitterUnqueue() {
}

int JitterUnqueue::configure(Vector<String> &conf, ErrorHandler *errh) {
	uint32_t jitter = 0;
	int result = Args(conf, this, errh).read_mp("JITTER", jitter).complete();
	_jitter = Timestamp::make_usec(jitter);
	return result;
}

int JitterUnqueue::initialize(ErrorHandler *errh) {
	ScheduleInfo::initialize_task(this, &_task, errh);
	_timer.initialize(this);
	_signal = Notifier::upstream_empty_signal(this, 0, &_task);
	return 0;
}

void JitterUnqueue::cleanup(CleanupStage) {
	if (_p)
		_p->kill();
}

bool JitterUnqueue::run_task(Task *) {
	bool worked = false;

	retry:
	// read a packet
	if (!_p && (_p = input(0).pull())) {
		if (!_p->timestamp_anno().sec()) // get timestamp if not set
			_p->timestamp_anno().assign_now();

		// generate random jitter
		unsigned int randVal = (double) click_random() / CLICK_RAND_MAX * _jitter.usec();
		_p->timestamp_anno() += Timestamp::make_usec(randVal);
	}

	if (_p) {
		Timestamp now = Timestamp::now();
		if (_p->timestamp_anno() <= now) {
			// packet ready for output
			output(0).push(_p);
			_p = 0;
			worked = true;
			goto retry;
		}

		Timestamp expiry = _p->timestamp_anno() - Timer::adjustment();
		if (expiry <= now)
			// small delta, reschedule Task
			/* Task rescheduled below */;
		else {
			// large delta, schedule Timer
			_timer.schedule_at(expiry);
			return false;		// without rescheduling
		}
	} else {
		// no Packet available
		if (!_signal)
			return false;		// without rescheduling
	}

	_task.fast_reschedule();
	return worked;
}

void JitterUnqueue::add_handlers() {
	add_data_handlers("jitter", Handler::OP_READ | Handler::OP_WRITE, &_jitter,	true);
	add_task_handlers(&_task);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(JitterUnqueue)
