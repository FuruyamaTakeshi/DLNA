#include <string.h>

#include <cybergarage/http/chttp.h>

#include "ClinkTestCase.h"

////////////////////////////////////////
// testHttpServer
////////////////////////////////////////

#define CLINK_TESTCASE_HTTP_PORT 38192
#define CLINK_TESTCASE_HTTP_PAGE "<HTML><BODY>Hello World</BODY></HTML>"
#define CLINK_TESTCASE_HTTP_URL "/index.html"
#define CLINK_TESTCASE_HTTP_LOOP 100

void ClinkTestcaseHttpRequestRecieved(CgHttpRequest *httpReq)
{
	CgHttpResponse *httpRes;

	httpRes = cg_http_response_new();
	cg_http_response_setstatuscode(httpRes, CG_HTTP_STATUS_OK);
	cg_http_response_setcontent(httpRes, CLINK_TESTCASE_HTTP_PAGE);
	cg_http_response_setcontenttype(httpRes, "text/html");
	cg_http_response_setcontentlength(httpRes, strlen(CLINK_TESTCASE_HTTP_PAGE));
	cg_http_request_postresponse(httpReq, httpRes);
	cg_http_response_delete(httpRes);
}

void ClinkTestCase::testHttpServer()
{
	/**** HTTP Server ****/
	CgHttpServer *httpServer = cg_http_server_new();
	CPPUNIT_ASSERT(httpServer);
	CPPUNIT_ASSERT(cg_http_server_open(httpServer, CLINK_TESTCASE_HTTP_PORT, NULL));
	cg_http_server_setlistener(httpServer, ClinkTestcaseHttpRequestRecieved);
	cg_http_server_start(httpServer);

	/**** HTTP Client ****/
	for (int n=0; n<CLINK_TESTCASE_HTTP_LOOP; n++) {
		CgHttpRequest *httpReq = cg_http_request_new();
		CPPUNIT_ASSERT(httpReq);
		cg_http_request_setmethod(httpReq, CG_HTTP_GET);
		cg_http_request_seturi(httpReq, CLINK_TESTCASE_HTTP_URL);
		cg_http_request_setcontentlength(httpReq, 0);
		CgHttpResponse *httpRes = cg_http_request_post(httpReq, "localhost", CLINK_TESTCASE_HTTP_PORT);
		CPPUNIT_ASSERT(httpRes);
		CPPUNIT_ASSERT(cg_http_response_issuccessful(httpRes));
		CPPUNIT_ASSERT(cg_streq(cg_http_response_getcontent(httpRes), CLINK_TESTCASE_HTTP_PAGE));
		CPPUNIT_ASSERT(cg_http_response_getcontentlength(httpRes) == cg_strlen(CLINK_TESTCASE_HTTP_PAGE));
		cg_http_request_delete(httpReq);
	}

	/**** HTTP Server ****/
	cg_http_server_stop(httpServer);
}
