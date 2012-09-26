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
*	File: caction_ctrl.c
*
*	Revision:
*
*	05/17/05
*		- first revision
*
*
*	10/31/05
*		- cg_upnp_action_post: If action post was unsuccessful, put the 
*		  error message to the action response
*
******************************************************************/

#include <cybergarage/upnp/control/ccontrol.h>
#include <cybergarage/util/clog.h>

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (Begin)
****************************************/

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

/****************************************
* cg_upnp_action_clearoutputargumentvalues
****************************************/

void cg_upnp_action_clearoutputargumentvalues(CgUpnpAction *action)
{
	CgUpnpArgumentList *argList;
	CgUpnpArgument *arg;
	
	cg_log_debug_l4("Entering...\n");

	argList = cg_upnp_action_getargumentlist(action);
	for (arg=cg_upnp_argumentlist_gets(argList); arg != NULL; arg = cg_upnp_argument_next(arg)) {
		if (cg_upnp_argument_isoutdirection(arg) == TRUE)
			cg_upnp_argument_setvalue(arg, "");
	}

	cg_log_debug_l4("Leaving...\n");
}

/****************************************
* cg_upnp_action_performlistener
****************************************/

BOOL cg_upnp_action_performlistner(CgUpnpAction *action, CgUpnpActionRequest *actionReq)
{
	CG_UPNP_ACTION_LISTNER listener;
	CgUpnpActionResponse *actionRes;
	CgHttpRequest *actionReqHttpReq;
	CgHttpResponse *actionResHttpRes;
	
	cg_log_debug_l4("Entering...\n");

	listener = cg_upnp_action_getlistner(action);
	if (listener == NULL)
		return FALSE;

	actionRes = cg_upnp_control_action_response_new();
	
	cg_upnp_action_setstatuscode(action, CG_UPNP_STATUS_INVALID_ACTION);
	cg_upnp_action_setstatusdescription(action, cg_upnp_status_code2string(CG_UPNP_STATUS_INVALID_ACTION));

	cg_upnp_action_clearoutputargumentvalues(action);
	
	if (listener(action) == TRUE)
		cg_upnp_control_action_response_setresponse(actionRes, action);
	else
		cg_upnp_control_soap_response_setfaultresponse(cg_upnp_control_action_response_getsoapresponse(actionRes), cg_upnp_action_getstatuscode(action), cg_upnp_action_getstatusdescription(action));
	
	actionReqHttpReq = cg_soap_request_gethttprequest(cg_upnp_control_action_request_getsoaprequest(actionReq));
	actionResHttpRes = cg_soap_response_gethttpresponse(cg_upnp_control_action_response_getsoapresponse(actionRes));
	cg_http_request_postresponse(actionReqHttpReq, actionResHttpRes);	

	cg_upnp_control_action_response_delete(actionRes);
	
	cg_log_debug_l4("Leaving...\n");

	return TRUE;
}

/****************************************
* cg_upnp_action_post
****************************************/

BOOL cg_upnp_action_post(CgUpnpAction *action)
{
	CgUpnpActionRequest *actionReq;
	CgUpnpActionResponse *actionRes;
	BOOL actionSuccess;

	cg_log_debug_l4("Entering...\n");
	
	actionReq = cg_upnp_control_action_request_new();
	
	cg_upnp_control_action_request_setaction(actionReq, action);
	actionRes = cg_upnp_control_action_request_post(actionReq);
	actionSuccess = cg_upnp_control_action_response_issuccessful(actionRes);
	if (actionSuccess == TRUE) {
                /* Reset status code to 0 (otherwise latest error stays in action) */
                cg_upnp_action_setstatuscode(action, 0);
		if (cg_upnp_control_action_response_getresult(actionRes, action) == FALSE) {
			actionSuccess = FALSE;
		}
	} else {
		/* Action was unsuccesful, but put the error to the action */
		cg_upnp_control_action_response_geterror(actionRes, action);
	}

	cg_upnp_control_action_request_delete(actionReq);
	
	cg_log_debug_l4("Leaving...\n");

	return actionSuccess;
}

/****************************************
* CG_UPNP_NOUSE_ACTIONCTRL (End)
****************************************/

#endif

