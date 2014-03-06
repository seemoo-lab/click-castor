/*
 * castor_routingtable.hh
 *
 *  Created on: 05.12.2013
 *      Author: dast
 */

#ifndef CASTOR_ROUTINGTABLE_HH_
#define CASTOR_ROUTINGTABLE_HH_
#include <click/element.hh>
#include <click/vector.hh>
#include "castor.hh"

CLICK_DECLS

#define BROADCAST_ADJUST 1
#define UPDATE_DELTA 0.5

typedef uint8_t FlowId[CASTOR_HASHLENGTH];
typedef uint32_t Id;

enum Operation {increase, decrease};
enum Estimate {first, all};

typedef struct{
	IPAddress 	nextHop;
	double 		alpha_all;
	double 		beta_all;
	double 		alpha_first;
	double 		beta_first;
}RoutingEntry;

typedef struct{
	FlowId 					flow;
	Vector<RoutingEntry> 	routes;
}FlowEntry;

class CastorRoutingTable : public Element {
	public:
		CastorRoutingTable();
		~CastorRoutingTable();

		const char *class_name() const	{ return "CastorRoutingTable"; }
		const char *port_count() const	{ return PORTS_0_0; }
		const char *processing() const	{ return AGNOSTIC; }

		/**
		 * updateNeighbour: Updates the Host in Database, if Host is not present create it
		 * Method is used to keep a view of neighbors
		 */
		void updateNeighbor(IPAddress);

		/**
		 * Perform the Lookup operation, determines the best route for Packet with given FlowID
		 */
		IPAddress lookup(FlowId);

		/**
		 * Updates the Estimation values in the Database for Flow Host
		 * Third parameter is true for successfull updading, else
		 */
		void updateEstimates(FlowId, IPAddress, Operation, Estimate);


		//int configure(Vector<String>&, ErrorHandler*);

	private:
		Vector<FlowEntry> _flows;

		Vector<RoutingEntry>* getRoutingTable(FlowId flow);

		RoutingEntry*	getRoutingEntry(Vector<RoutingEntry>*, IPAddress);

		void 			printRoutingTable(FlowId);

		double			getEstimate(RoutingEntry*);



};

CLICK_ENDDECLS
#endif /* CASTOR_ROUTINGTABLE_HH_ */
