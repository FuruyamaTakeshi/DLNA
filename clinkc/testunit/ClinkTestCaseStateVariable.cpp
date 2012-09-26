#include <string.h>

#include "ClinkTestCase.h"
#include "TestDevice.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

#define TEST_UPDATE_STATEVARIABLE_UPDATEVALUE "4649"

void ClinkTestCase::testStateVariable()
{
	CgUpnpDevice *testDev = upnp_test_device_new();
	CPPUNIT_ASSERT(testDev);
	CPPUNIT_ASSERT(cg_upnp_device_start(testDev));

	// Update State Variable
	CgUpnpService *testDevService = cg_upnp_device_getservicebyexacttype(testDev, TEST_DEVICE_SERVICE_TYPE);
	CPPUNIT_ASSERT(testDevService != NULL);
	CgUpnpStateVariable *testDevState = cg_upnp_service_getstatevariablebyname(testDevService, TEST_DEVICE_STATEVARIABLE_STATUS);
	CPPUNIT_ASSERT(testDevState != NULL);
	cg_upnp_statevariable_setvalue(testDevState, TEST_UPDATE_STATEVARIABLE_UPDATEVALUE);
	CPPUNIT_ASSERT(cg_streq(cg_upnp_statevariable_getvalue(testDevState), TEST_UPDATE_STATEVARIABLE_UPDATEVALUE));

	CPPUNIT_ASSERT(cg_upnp_device_stop(testDev));
	cg_upnp_device_delete(testDev);
}
