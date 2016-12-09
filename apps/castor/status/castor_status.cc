#include <string.h>
#include <iostream>
#include <cstdlib>
#include <arpa/inet.h>

#include "../../csclient/csclient.hh"

using namespace std;

int main(int argc, char **argv)
{
  unsigned short port = 7777;
  unsigned long ip;
  if (argc > 1)
    ip = inet_addr(argv[1]);
  else
    ip = inet_addr("127.0.0.1");

  if (argc > 2)
    port = (unsigned short) atoi(argv[2]);

  typedef ControlSocketClient csc_t;
  csc_t cs;

  typedef csc_t::err_t err_t;
  err_t err = cs.configure(ip, port);

  cout << "# Castor Neighbors" << endl;
  string data = "";
  err = cs.read("neighbors", "print", data);
  cout << data << endl;

  cout << "# Castor Routing Table" << endl;
  err = cs.read("routingtable", "print", data);
  cout << data << endl;

  return 0;
}
