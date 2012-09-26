#include "ClinkTestCase.h"

#include <cybergarage/net/curi.h>

////////////////////////////////////////
// testNetworkInterface
////////////////////////////////////////

#define CLINK_TESTCASE_NET_URI_PATH "/test.cgi"
#define CLINK_TESTCASE_NET_URI_PARAM1_NAME "abc"
#define CLINK_TESTCASE_NET_URI_PARAM2_NAME "defgh"
#define CLINK_TESTCASE_NET_URI_PARAM1_VALUE "123"
#define CLINK_TESTCASE_NET_URI_PARAM2_VALUE "45678"

#define CLINK_TESTCASE_NET_URI_FRAGMENT "fragment"

void ClinkTestCase::testURI()
{
	CgNetURI *uri = cg_net_uri_new();
	cg_net_uri_setvalue(uri, CLINK_TESTCASE_NET_URI_PATH "?" 
		CLINK_TESTCASE_NET_URI_PARAM1_NAME "=" CLINK_TESTCASE_NET_URI_PARAM1_VALUE "&"
		CLINK_TESTCASE_NET_URI_PARAM2_NAME "=" CLINK_TESTCASE_NET_URI_PARAM2_VALUE
		"#" CLINK_TESTCASE_NET_URI_FRAGMENT);
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getpath(uri), CLINK_TESTCASE_NET_URI_PATH));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getquery(uri), 
		CLINK_TESTCASE_NET_URI_PARAM1_NAME "=" CLINK_TESTCASE_NET_URI_PARAM1_VALUE "&"
		CLINK_TESTCASE_NET_URI_PARAM2_NAME "=" CLINK_TESTCASE_NET_URI_PARAM2_VALUE
		));
	CPPUNIT_ASSERT(cg_streq(cg_net_uri_getfragment(uri), CLINK_TESTCASE_NET_URI_FRAGMENT));
	CgDictionary *paramDir = cg_net_uri_getquerydictionary(uri);
	CgDictionaryElement *paramElem;
	paramElem = cg_dictionary_gets(paramDir);
	CPPUNIT_ASSERT(paramElem != NULL);
	CPPUNIT_ASSERT(cg_streq(cg_dictionary_element_getkey(paramElem), CLINK_TESTCASE_NET_URI_PARAM1_NAME));
	CPPUNIT_ASSERT(cg_streq(cg_dictionary_element_getvalue(paramElem), CLINK_TESTCASE_NET_URI_PARAM1_VALUE));
	paramElem = cg_dictionary_element_next(paramElem);
	CPPUNIT_ASSERT(paramElem != NULL);
	CPPUNIT_ASSERT(cg_streq(cg_dictionary_element_getkey(paramElem), CLINK_TESTCASE_NET_URI_PARAM2_NAME));
	CPPUNIT_ASSERT(cg_streq(cg_dictionary_element_getvalue(paramElem), CLINK_TESTCASE_NET_URI_PARAM2_VALUE));

	cg_net_uri_delete(uri);
}
