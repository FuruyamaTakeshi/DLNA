#include <string.h>

#include <cybergarage/net/cinterface.h>

#include "ClinkTestCase.h"

////////////////////////////////////////
// testNetworkInterface
////////////////////////////////////////

void ClinkTestCase::testNetworkInterface()
{
	CgByte nullMacAddr[CG_NET_MACADDR_SIZE];
	CgByte macAddr[CG_NET_MACADDR_SIZE];
	memset(nullMacAddr, 0, CG_NET_MACADDR_SIZE);

	CgNetworkInterfaceList *netIfList = cg_net_interfacelist_new();
	CPPUNIT_ASSERT(netIfList);
	CPPUNIT_ASSERT(0 < cg_net_gethostinterfaces(netIfList));
	for (CgNetworkInterface *netIf=cg_net_interfacelist_gets(netIfList); netIf; netIf=cg_net_interface_next(netIf)) {
		char *ipaddr = cg_net_interface_getaddress(netIf);
		CPPUNIT_ASSERT(0 < cg_strlen(ipaddr));
		CPPUNIT_ASSERT(cg_streq(ipaddr, "0.0.0.0") == FALSE);
		cg_net_interface_getmacaddress(netIf, macAddr);
		CPPUNIT_ASSERT(memcmp(macAddr, nullMacAddr, CG_NET_MACADDR_SIZE) != 0);
		//CPPUNIT_ASSERT(0 < cg_strlen(cg_net_interface_getname(netIf)));
		//CPPUNIT_ASSERT(0 < cg_strlen(cg_net_interface_getnetmask(netIf)));
	}
	cg_net_interfacelist_delete(netIfList);
}
