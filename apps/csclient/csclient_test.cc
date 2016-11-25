#include <string.h>
#include <iostream>
#include <assert.h>
#include <arpa/inet.h>

#include "csclient.hh"

using namespace std;

#define assert_eq(e, v) do { if ((e) != (v)) { cerr << "got " << (e) << endl; } assert((e) == (v)); } while (false)
#define ok(err) assert_eq(err, ControlSocketClient::no_err)
#define test(x, e) do { err_t err = (x); if (err != (e)) { cerr << "wanted " << assert(0) } while (false)

int
main(int argc, char **argv)
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
  ok(err);

  cout << "Router name: ``" << cs.name() << "''" << endl;

  string s;
  err = cs.get_router_version(s);
  ok(err);
  cout << "Router version: ``" << s << "''" << endl;

  vector<string> vs;
  err = cs.get_config_el_names(vs);
  ok(err);
  cout << "Elements:" << endl;
  for (size_t i = 0; i < vs.size(); i++)
    cout << i << " ``" << vs[i] << "''" << endl;

  err = cs.get_router_classes(vs);
  ok(err);
  cout << "Classes:" << endl;
  for (size_t i = 0; i < vs.size(); i++)
    cout << i << " ``" << vs[i] << "''" << endl;

  err = cs.get_router_packages(vs);
  ok(err);
  cout << "Packages:" << endl;
  for (size_t i = 0; i < vs.size(); i++)
    cout << i << " ``" << vs[i] << "''" << endl;

  err = cs.get_config_reqs(vs);
  ok(err);
  cout << "Configuration requirements:" << endl;
  for (size_t i = 0; i < vs.size(); i++)
    cout << i << " ``" << vs[i] << "''" << endl;

  err = cs.get_router_config(s);
  ok(err);
  cout << endl;
  cout << "*************** Begin Router Configuration *******************" << endl;
  cout << s;
  cout << "*************** End Router Configuration *******************" << endl;

  err = cs.get_router_flat_config(s);
  ok(err);
  cout << endl;
  cout << "*************** Begin Flattened Router Configuration *******************" << endl;
  cout << s;
  cout << "*************** End Flattened Router Configuration *******************" << endl;

  cout << endl;
  cout << "Handler info test: " << endl;
  vector<csc_t::handler_info_t> vhi;
  /* NB: this test assumes that there are some interesting elements in the configuration */
  err = cs.get_config_el_names(vs);
  ok(err);

  if (vs.size() == 0) {
    cout << "No elements to test handler info on, exiting tests early" << endl;
    return 0;
  }
  string el = vs[0];
  err = cs.get_el_handlers(el, vhi);

  for (size_t i = 0; i < vhi.size(); i++)
    cout << vhi[i].element_name << "." << vhi[i].handler_name << "\t"
	 << (vhi[i].can_read ? "r" : "")
	 << (vhi[i].can_write ? "w" : "")
	 << endl;

  cout << endl;

  cout << "Check Handler test: " << endl;
  for (size_t i = 0; i < vhi.size(); i++) {
    cout << vhi[i].element_name << "." << vhi[i].handler_name << '\t';

    bool res;
    /* is writeable? */
    err = cs.check_handler(vhi[i].element_name, vhi[i].handler_name,
			   true, res);
    ok(err);
    cout << "is_write:" << ((res == vhi[i].can_write) ? "pass" : "FAIL") << '\t';

    /* is readable? */
    err = cs.check_handler(vhi[i].element_name, vhi[i].handler_name,
			   false, res);
    ok(err);
    cout << "is_read:" << ((res == vhi[i].can_read) ? "pass" : "FAIL") << endl;
  }

  cout << endl;
  cout << "Read/Write handler test: ";
  string data = "1234567891abcdefghij";
  /*
   * NB: to place spaces in this handler's data requires that the
   * string be quoted; however, the handler's read function won't
   * return the quotes around the string so the read value doesn't
   * exactly match the write value.  to avoid, we don't use spaces....
   */
  err = cs.write("InfiniteSource@1", "data", data);
  ok(err);
  string data2;
  err = cs.read("InfiniteSource@1", "data", data2);
  ok(err);

  if (data2 != data)
    cout << "FAIL (wanted ``" << data << "'', but got ``" << data2 << "'')";
  else
    cout << "pass";
  cout << endl;

  cout << endl
       << endl
       << "********** Tests complete **********" << endl;

  return 0;
}
