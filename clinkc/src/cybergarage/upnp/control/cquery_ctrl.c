/******************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*       Copyright (C) 2006 Nokia Corporation. All rights reserved.
*
*       This is licensed under BSD-style license,
*       see file COPYING.
*
*	File: cquery_ctrl.c
*
*	Revision:
*
*	06/09/05
*		- first revision
*
******************************************************************/

#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_QUERY (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_QUERYCTRL)

/****************************************
* cg_upnp_action_performlistener
****************************************/

BOOL cg_upnp_statevariable_performlistner(CgUpnpStateVariable *statVar, CgUpnpQueryRequest *queryReq)
{
	CG_UPNP_STATEVARIABLE_LISTNER	 listener;
	CgUpnpQueryResponse *queryRes;
	CgHttpRequest *queryReqHttpReq;
	CgHttpResponse *queryResHttpRes;
	
	cg_log_debug_l4("Entering...\n");

	listener = cg_upnp_statevariable_getlistener(statVar);
	if (listener == NULL)
		return FALSE;
	
	queryRes = cg_upnp_control_query_response_new();

	cg_upnp_statevariable_setstatuscode(statVar, CG_UPNP_STATUS_INVALID_ACTION);
	cg_upnp_statevariable_setstatusdescription(statVar, cg_upnp_status_code2string(CG_UPNP_STATUS_INVALID_ACTION));
	cg_upnp_statevariable_setvalue(statVar, "");
	
	if (listener(statVar) == TRUE)
		cg_upnp_control_query_response_setresponse(queryRes, statVar);
	else
		cg_upnp_control_soap_response_setfaultresponse(cg_upnp_control_query_response_getsoapresponse(queryRes), cg_upnp_statevariable_getstatuscode(statVar), cg_upnp_statevariable_getstatusdescription(statVar));
	
	queryReqHttpReq = cg_soap_request_gethttprequest(cg_upnp_control_query_request_getsoaprequest(queryReq));
	queryResHttpRes = cg_soap_response_gethttpresponse(cg_upnp_control_query_response_getsoapresponse(queryRes));
	cg_http_request_postresponse(queryReqHttpReq, queryResHttpRes);	

	cg_upnp_control_query_response_delete(queryRes);
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_statevariable_post
****************************************/

BOOL cg_upnp_statevariable_post(CgUpnpStateVariable *statVar)
{
	CgUpnpQueryRequest *queryReq;
	CgUpnpQueryResponse *queryRes;
	BOOL querySuccess;
	
	cg_log_debug_l4("Entering...\n");

	queryReq = cg_upnp_control_query_request_new();
	
	cg_upnp_control_query_request_setstatevariable(queryReq, statVar);
	queryRes = cg_upnp_control_query_request_post(queryReq);
	querySuccess = cg_upnp_control_query_response_issuccessful(queryRes);
	cg_upnp_statevariable_setvalue(statVar, (querySuccess == TRUE) ? cg_upnp_control_query_response_getreturnvalue(queryRes) : "");
	
	cg_upnp_control_query_request_delete(queryReq);
	
	cg_log_debug_l4("Leaving...\n");

	return querySuccess;
}

/****************************************
* CG_UPNP_NOUSE_QUERY (End)
****************************************/

#endif
