#include <string.h>

#include "ClinkTestCase.h"
#include "TestDevice.h"

////////////////////////////////////////
// testDevice`
////////////////////////////////////////

void ClinkTestCase::testDevice()
{
	CgUpnpDevice *testDev;

	testDev = upnp_test_device_new();
	CPPUNIT_ASSERT(testDev);
	CPPUNIT_ASSERT(strcmp(cg_upnp_device_getfriendlyname(testDev), TEST_DEVICE_FREINDLYNAME) == 0);
	CPPUNIT_ASSERT(strcmp(cg_upnp_device_getdevicetype(testDev), TEST_DEVICE_DEVICE_TYPE) == 0);

	// Embedded Devices
	int ebdDeviceCnt = 0;
	for (CgUpnpDevice *ebdDev = cg_upnp_device_getdevices(testDev); ebdDev; ebdDev = cg_upnp_device_next(ebdDev)) {
		ebdDeviceCnt++;
		if (ebdDeviceCnt == 1)
			CPPUNIT_ASSERT(strcmp(cg_upnp_device_getdevicetype(ebdDev), TEST_DEVICE_EMBEDDEDDEVICE_TYPE1) == 0);
		if (ebdDeviceCnt == 2)
			CPPUNIT_ASSERT(strcmp(cg_upnp_device_getdevicetype(ebdDev), TEST_DEVICE_EMBEDDEDDEVICE_TYPE2) == 0);
	}
	CPPUNIT_ASSERT(ebdDeviceCnt == 2);

	CPPUNIT_ASSERT(cg_upnp_device_start(testDev));

	CPPUNIT_ASSERT(cg_upnp_device_stop(testDev));
	cg_upnp_device_delete(testDev);
}
