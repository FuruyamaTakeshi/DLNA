#include <string.h>

#include <cybergarage/upnp/std/av/cmediarenderer.h>

#include "ClinkAVTestCase.h"

////////////////////////////////////////
// testDevice
////////////////////////////////////////

#define TEST_LASTCHANGE_VALUE \
	"&lt;Event xmlns = &quot;urn:schemas-upnp-org:metadata-1-0/RCS/&quot;&gt;" \
	"&lt;InstanceID val=&quot;0&quot;&gt;" \
	"&lt;Volume val=&quot;100&quot; channel=&quot;RF&quot;/&gt;" \
	"&lt;Volume val=&quot;100&quot; channel=&quot;LF&quot;/&gt;" \
	"&lt;/InstanceID&gt;" \
	"&lt;/Event&gt;" \

void ClinkAVTestCase::testRenderer()
{
	CgUpnpAvRenderer *dmr = cg_upnpav_dmr_new();

	cg_upnpav_dmr_start(dmr);

	cg_upnpav_dmr_setrenderingcontrollastchange(dmr, TEST_LASTCHANGE_VALUE);
	CPPUNIT_ASSERT(cg_streq(TEST_LASTCHANGE_VALUE, cg_upnpav_dmr_getrenderingcontrollastchange(dmr)));

	while	(1) {
		cg_sleep(1000);
	}

	cg_upnpav_dmr_stop(dmr);
}
