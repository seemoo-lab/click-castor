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
	JitterUnqueue() : simulatorTime(false), p(0), jitter(0), task(this), timer(&task) {};

    const char *class_name() const	{ return "JitterUnqueue"; }
    const char *port_count() const	{ return PORTS_1_1; }
    const char *processing() const	{ return PULL_TO_PUSH; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    int initialize(ErrorHandler *) CLICK_COLD;
    void cleanup(CleanupStage) CLICK_COLD;
    void add_handlers() CLICK_COLD;

    bool run_task(Task *);

  private:
    bool simulatorTime;
    Packet *p;
    Timestamp jitter;
    Task task;
    Timer timer;
    NotifierSignal signal;

};

CLICK_ENDDECLS

#endif
