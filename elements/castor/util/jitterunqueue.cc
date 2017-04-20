/*
 * Copyright (c) 2016 Milan Schmittner
 *
 * This file is part of click-castor.
 *
 * click-castor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * click-castor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with click-castor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <click/config.h>
#include <click/error.hh>
#include <click/args.hh>
#include <click/glue.hh>
#include "jitterunqueue.hh"
#include <click/standard/scheduleinfo.hh>
#include <click/timestamp.hh>
#include <click/straccum.hh>

CLICK_DECLS

int JitterUnqueue::configure(Vector<String> &conf, ErrorHandler *errh) {
	uint32_t jitter_usec = 0;
	int result = Args(conf, this, errh)
			.read_mp("JITTER", jitter_usec)
			.read_mp("SIMTIME", simulatorTime)
			.complete();
	jitter = Timestamp::make_usec(jitter_usec);
	return result < 0 ? -1 : 0;
}

int JitterUnqueue::initialize(ErrorHandler *errh) {
	ScheduleInfo::initialize_task(this, &task, errh);
	timer.initialize(this);
	signal = Notifier::upstream_empty_signal(this, 0, &task);
	return 0;
}

void JitterUnqueue::cleanup(CleanupStage) {
	if (p)
		p->kill();
}

bool JitterUnqueue::run_task(Task *) {
	bool worked = false;

	retry:
	// read a packet
	if (!p && (p = input(0).pull())) {
		if (!p->timestamp_anno().sec()) // get timestamp if not set
			p->timestamp_anno().assign_now_steady();

		// generate random jitter
		unsigned int randVal = (double) click_random() / CLICK_RAND_MAX * jitter.usec();
		p->timestamp_anno() += Timestamp::make_usec(randVal);
	}

	if (p) {
		Timestamp now = Timestamp::now_steady();
		if (p->timestamp_anno() <= now) {
			// packet ready for output
			output(0).push(p);
			p = 0;
			worked = true;
			goto retry;
		}

		// If 'simulatorTime' is set, no need to busy wait; just reschedule at that time
		Timestamp expiry;
		if(simulatorTime)
			expiry = p->timestamp_anno();
		else
			expiry = p->timestamp_anno() - Timer::adjustment();
		if (!simulatorTime && expiry <= now) {
			// small delta, reschedule Task
			/* Task rescheduled below */
		} else {
			// large delta, schedule Timer
			timer.schedule_at_steady(expiry);
			return false;		// without rescheduling
		}

	} else {
		// no Packet available
		if (!signal)
			return false;		// without rescheduling
	}

	task.fast_reschedule();
	return worked;
}

void JitterUnqueue::add_handlers() {
	add_data_handlers("jitter", Handler::OP_READ | Handler::OP_WRITE, &jitter, true);
	add_task_handlers(&task);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(JitterUnqueue)
