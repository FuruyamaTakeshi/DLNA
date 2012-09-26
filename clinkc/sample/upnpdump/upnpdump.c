/************************************************************
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
*	File: upnpdump.c
*
*	Revision:
*       05/11/05
*               - first release.
*
*	10/31/05
*		- Added some M-SEARCH sending actions
*
************************************************************/

#include <cybergarage/upnp/cupnp.h>

#include <ctype.h>

#if defined(WIN32) && !defined(__CYGWIN__)
#include <conio.h>
#endif

#if defined(TENGINE)
#if defined(PROCESS_BASE) /* Process based */
#include <basic.h>
#define MBEG	int main( W ac, TC *av[] )
#define MEND	0
#define DIR	"/SYS/bin/"
#else /* T-Kernel based */
#include <basic.h>
#include <tk/tkernel.h>
#include <sys/pinfo.h>
#define MBEG	ER main( INT ac, UB *av[] )
#define MEND	E_SYS
#define DIR	"/SYS/bin/"
#endif
#endif

#include "kb_util.h"

#if defined(WIN32) && !defined(__CYGWIN__)
#define kbgetkey getch
#else
#define kbgetkey getchar
#endif

/* Additional control commands */

#define CMD_NO_ALTERATIONS 1
#define CMD_LOOP_ACTION_CALLS 2

void ControlDeviceAlter(CgUpnpControlPoint *ctrlPoint, int alteration_mask);

/////////////////////////////////////////////////////////////////////////////////
// PrintKeyMessage
/////////////////////////////////////////////////////////////////////////////////

void PrintKeyMessage()
{
	printf("'p' : Print\n");
#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)
	printf("'c' : Control\n");
	printf("'o' : Control (multiple actions)\n");
#endif
#if !defined(CG_UPNP_NOUSE_QUERYCTRL)
	printf("'q' : Query\n");
#endif
#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
	printf("'s' : Subscribe\n");
	printf("'u' : Unsubscribe\n");
#endif
	printf("'m' : Change MX value for M-Search\n");
	printf("'r' : M-Search (rootdevice)\n");
	printf("'h' : M-Search (ssdp:all)\n");
	printf("'v' : M-Search (urn:schemas-upnp-org:device:clock:1)\n");
	printf("'t' : M-Search (urn:schemas-upnp-org:service:timer:1)\n");
	printf("'a' : Do all searches four times in a loop.\n");
	printf("'x' : eXit\n");
}

/////////////////////////////////////////////////////////////////////////////////
// Notify Listener
/////////////////////////////////////////////////////////////////////////////////

void SSDPNotifyListner(CgUpnpSSDPPacket *ssdpPkt)
{
	if (cg_upnp_ssdp_packet_isdiscover(ssdpPkt) == TRUE) {
		printf("ssdp:discover : ST = %s\n",
			cg_upnp_ssdp_packet_getst(ssdpPkt)); 
	}
	else if (cg_upnp_ssdp_packet_isalive(ssdpPkt) == TRUE) {
		printf("ssdp:alive : uuid = %s, NT = %s, location = %s\n",
			cg_upnp_ssdp_packet_getusn(ssdpPkt), 
			cg_upnp_ssdp_packet_getnt(ssdpPkt), 
			cg_upnp_ssdp_packet_getlocation(ssdpPkt)); 
	}
	else if (cg_upnp_ssdp_packet_isbyebye(ssdpPkt) == TRUE) {
		printf("ssdp:byebye : uuid = %s, NT = %s\n",
			cg_upnp_ssdp_packet_getusn(ssdpPkt), 
			cg_upnp_ssdp_packet_getnt(ssdpPkt));
	}
	cg_upnp_ssdp_packet_print(ssdpPkt); 
}

/////////////////////////////////////////////////////////////////////////////////
// Print Device
/////////////////////////////////////////////////////////////////////////////////

void PrintDeviceInfo(CgUpnpDevice *dev, int indent)
{
	char indentStr[128];
	int n;
	CgUpnpService *service;
	int serviceCnt;
	CgUpnpAction *action;
	int actionCnt;
	CgUpnpArgumentList *arg;
	int argCnt;
	CgUpnpStateVariable *stateVar;
	int stateVarCnt;
	
	for (n=0; n<indent && n<(sizeof(indentStr)-1); n++)
		indentStr[n] = ' ';
	indentStr[n] = '\0';
	
	serviceCnt = 0;
	for (service = cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		printf("%s service[%d] = %s\n", indentStr, ++serviceCnt, cg_upnp_service_getservicetype(service));
		actionCnt = 0;
		for (action = cg_upnp_service_getactions(service); action != NULL; action = cg_upnp_action_next(action)) {
			printf("%s  action[%d] = %s\n", indentStr, ++actionCnt, cg_upnp_action_getname(action));
			argCnt = 0;
			for (arg = cg_upnp_action_getarguments(action); arg != NULL; arg = cg_upnp_argument_next(arg)) {
				printf("%s   arg[%d] = %s\n", indentStr, ++argCnt, cg_upnp_argument_getname(arg));
			}
		}
		stateVarCnt = 0;
		for (stateVar = cg_upnp_service_getstatevariables(service); stateVar != NULL; stateVar = cg_upnp_statevariable_next(stateVar)) {
			printf("%s  stateVar[%d] = %s = %s\n", 
			       indentStr, 
			       ++stateVarCnt, 
			       cg_upnp_statevariable_getname(stateVar),
			       cg_upnp_statevariable_getvalue(stateVar));
		}
	}
}

void PrintDevice(CgUpnpDevice *dev, int indent)
{
	CgUpnpDevice *childDev;
	
	PrintDeviceInfo(dev, indent);

	for (childDev = cg_upnp_device_getdevices(dev); childDev != NULL; childDev = cg_upnp_device_next(childDev))
		PrintDevice(childDev, indent+1);
}


void PrintControlPointDevice(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpDevice *dev;
	int devCnt;
		
	printf("Device Num = %d\n", cg_upnp_controlpoint_getndevices(ctrlPoint));
	
	devCnt = 0;
	for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = cg_upnp_device_next(dev)) {
		printf("[%d] = %s\n", ++devCnt, cg_upnp_device_getfriendlyname(dev));
		PrintDevice(dev, 1);
	}
}

/////////////////////////////////////////////////////////////////////////////////
// Select*
/////////////////////////////////////////////////////////////////////////////////

CgUpnpDevice *SelectDevice(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpDevice *dev;
	int n;
	char key;
	int devNum;

	n = 0;
	for (dev = cg_upnp_controlpoint_getdevices(ctrlPoint); dev != NULL; dev = cg_upnp_device_next(dev)) {
		key = 'a' + n;
		if ('z' < key)
			break;
		printf(" [%c] = %s\n", key, cg_upnp_device_getfriendlyname(dev));
		n++;
	}
	if (n == 0)
		return NULL;
	printf("Select Device : ");
	key = kbgetkey();
	key = tolower(key);
	printf("%c\n", key);
	
	if (!isalpha(key))
		return NULL;
	
	devNum = key - 'a';
	dev = cg_upnp_controlpoint_getdevices(ctrlPoint);
	for (n=0; n<devNum; n++)
		dev = cg_upnp_device_next(dev);

	return dev;
}

CgUpnpService *SelectService(CgUpnpDevice *dev)
{
	CgUpnpService *service;
	int n;
	char key;
	int serviceNum;

	n = 0;
	for (service = cg_upnp_device_getservices(dev); service != NULL; service = cg_upnp_service_next(service)) {
		key = 'a' + n;
		if ('z' < key)
			break;
		printf(" [%c] = %s\n", key, cg_upnp_service_getservicetype(service));
		n++;
	}
	printf("Select Service : ");
	key = kbgetkey();
	key = tolower(key);
	printf("%c\n", key);
	
	if (!isalpha(key))
		return NULL;
	
	serviceNum = key - 'a';
	service = cg_upnp_device_getservices(dev);
	for (n=0; n<serviceNum; n++)
		service = cg_upnp_service_next(service);

	return service;
}

CgUpnpAction *SelectAction(CgUpnpService *service)
{
	CgUpnpAction *action;
	int n;
	char key;
	int actionNum;

	n = 0;
	for (action = cg_upnp_service_getactions(service); action != NULL; action = cg_upnp_action_next(action)) {
		key = 'a' + n;
		if ('z' < key)
			break;
		printf(" [%c] = %s\n", key, cg_upnp_action_getname(action));
		n++;
	}
	printf("Select Action : ");
	key = kbgetkey();
	key = tolower(key);
	printf("%c\n", key);
	
	if (!isalpha(key))
		return NULL;
	
	actionNum = key - 'a';
	action = cg_upnp_service_getactions(service);
	for (n=0; n<actionNum; n++)
		action = cg_upnp_action_next(action);

	return action;
}

CgUpnpStateVariable *SelectStateVariable(CgUpnpService *service)
{
	CgUpnpStateVariable *stateVar;
	int n;
	char key;
	int serviceNum;

	n = 0;
	for (stateVar = cg_upnp_service_getstatevariables(service); stateVar != NULL; stateVar = cg_upnp_statevariable_next(stateVar)) {
		key = 'a' + n;
		if ('z' < key)
			break;
		printf(" [%c] = %s\n", key, cg_upnp_statevariable_getname(stateVar));
		n++;
	}
	printf("Select StateVariable : ");
	key = kbgetkey();
	key = tolower(key);
	printf("%c\n", key);
	
	if (!isalpha(key))
		return NULL;
	
	serviceNum = key - 'a';
	stateVar = cg_upnp_service_getstatevariables(service);
	for (n=1; n<serviceNum; n++)
		stateVar = cg_upnp_statevariable_next(stateVar);

	return stateVar;
}

/////////////////////////////////////////////////////////////////////////////////
// Action
/////////////////////////////////////////////////////////////////////////////////

#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)

void ControlDevice(CgUpnpControlPoint *ctrlPoint)
{
	ControlDeviceAlter(ctrlPoint, CMD_NO_ALTERATIONS);
}

void ControlDeviceAlter(CgUpnpControlPoint *ctrlPoint, int alteration_mask)
{
	CgUpnpDevice *selDev;
	CgUpnpService *selService;
	CgUpnpAction *selAction;
	BOOL actionSuccess;
	CgUpnpArgument *arg;
	char argValue[2048];
	
	printf("Control Device\n");
	
	selDev = SelectDevice(ctrlPoint);
	if (selDev == NULL)
		return;
	selService = SelectService(selDev);
	if (selService == NULL)
		return;
	selAction = SelectAction(selService);
	if (selAction == NULL)
		return;
	
	for (arg = cg_upnp_action_getarguments(selAction); arg; arg = cg_upnp_argument_next(arg)) {
		if (cg_upnp_argument_isindirection(arg) == TRUE) {
			printf("%s : ", cg_upnp_argument_getname(arg));
			if (scanf("%s", argValue) == 1)
				cg_upnp_argument_setvalue(arg, argValue);
		}
	}

	/* NOTE: Go through selAction memory management... */
	if (alteration_mask & CMD_LOOP_ACTION_CALLS)
	{
		int loop_count, i;

		printf("\nHow many times action should be sent?");
		
		if ( 1 == scanf("%d", &loop_count) )
		{
			printf("\n");
			for (i=0; i<loop_count; i++)
			{
				actionSuccess = cg_upnp_action_post(selAction);
				printf("Control Result(%d)\n", (int)actionSuccess);
			}

			cg_sleep(3000);

			for (	arg = cg_upnp_action_getarguments(selAction); 
				arg; 
				arg = cg_upnp_argument_next(arg)) 
			{
				if (cg_upnp_argument_isoutdirection(arg) == TRUE)
					printf(" %s = %s\n", 
							cg_upnp_argument_getname(arg), 
							cg_upnp_argument_getvalue(arg));
			}

			cg_sleep(2000);
		}
	}

	if ((alteration_mask & CMD_NO_ALTERATIONS) == CMD_NO_ALTERATIONS)
	{	
		actionSuccess = cg_upnp_action_post(selAction);
		
		printf("Control Result(%d)\n", (int)actionSuccess);
		for (arg = cg_upnp_action_getarguments(selAction); arg; arg = cg_upnp_argument_next(arg)) {
			if (cg_upnp_argument_isoutdirection(arg) == TRUE)
				printf(" %s = %s\n", cg_upnp_argument_getname(arg), cg_upnp_argument_getvalue(arg));
		}
	}
}

#endif

////////////////////////////////////////////////////////////////////////////////
// Query
/////////////////////////////////////////////////////////////////////////////////

#if !defined(CG_UPNP_NOUSE_QUERYCTRL)

void QueryDevice(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpDevice *selDev;
	CgUpnpService *selService;
	CgUpnpStateVariable *selStateVar;
	BOOL querySuccess;
	char *stateValue;
	
	printf("Query Device\n");
	
	selDev = SelectDevice(ctrlPoint);
	if (selDev == NULL)
		return;
	selService = SelectService(selDev);
	if (selService == NULL)
		return;
	selStateVar = SelectStateVariable(selService);
	if (selStateVar == NULL)
		return;
	
	querySuccess = cg_upnp_statevariable_post(selStateVar);
	
	stateValue = cg_upnp_statevariable_getvalue(selStateVar);

	printf("Query Result(%d) = %s\n",
		(int)querySuccess, 
		(stateValue != NULL) ? stateValue : "");
}

#endif

////////////////////////////////////////////////////////////////////////////////
// Subscribe
/////////////////////////////////////////////////////////////////////////////////

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

void SubscribeService(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpDevice *selDev;
	CgUpnpService *selService;
	BOOL subSuccess;
	
	printf("Subscribe Device\n");
	
	cg_upnp_controlpoint_lock(ctrlPoint);
	selDev = SelectDevice(ctrlPoint);
	if (selDev == NULL)
	{
		cg_upnp_controlpoint_unlock(ctrlPoint);
		return;
	}
	selService = SelectService(selDev);
	if (selService == NULL)
	{
		cg_upnp_controlpoint_unlock(ctrlPoint);
		return;
	}
	subSuccess = cg_upnp_controlpoint_subscribe(ctrlPoint, selService, 300);
	
	printf("Subscribe Result(%d) = %s\n",
		(int)subSuccess, 
		(subSuccess == TRUE) ? cg_upnp_service_getsubscriptionsid(selService) : "");
	cg_upnp_controlpoint_unlock(ctrlPoint);
	
}

#endif

////////////////////////////////////////////////////////////////////////////////
// Unsubscribe
/////////////////////////////////////////////////////////////////////////////////

#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)

void UnsubscribeService(CgUpnpControlPoint *ctrlPoint)
{
	CgUpnpDevice *selDev;
	CgUpnpService *selService;
	BOOL subSuccess;
	
	printf("Query Device\n");
	
	selDev = SelectDevice(ctrlPoint);
	if (selDev == NULL)
		return;
	selService = SelectService(selDev);
	if (selService == NULL)
		return;

	subSuccess = cg_upnp_controlpoint_unsubscribe(ctrlPoint, selService);

	printf("Unsubscribe Result(%d)\n",
		(int)subSuccess);
}

#endif

/////////////////////////////////////////////////////////////////////////////////
// Set MX value
/////////////////////////////////////////////////////////////////////////////////

void SetMXValue(CgUpnpControlPoint *ctrlPoint)
{
        unsigned int mxValue;

        printf("Give new MX value: ");
        if (scanf("%u", &mxValue) == 1) {
                printf( "%d", mxValue );
                cg_upnp_controlpoint_setssdpsearchmx(ctrlPoint, mxValue);
        }
        printf( "\n" );
}

/////////////////////////////////////////////////////////////////////////////////
// Event
/////////////////////////////////////////////////////////////////////////////////

void EventListener(CgUpnpProperty *prop)
{
	printf("Property Changed (%s) = %s\n",
		cg_upnp_property_getname(prop),
		cg_upnp_property_getvalue(prop));
}

/////////////////////////////////////////////////////////////////////////////////
// main
/////////////////////////////////////////////////////////////////////////////////

#if defined(TENGINE)
MBEG
#else
int main( int argc, char* argv[] )
#endif
{
	CgUpnpControlPoint *ctrlPoint;
	int key;
	
	ctrlPoint = cg_upnp_controlpoint_new();
	cg_upnp_controlpoint_setssdplistener(ctrlPoint, SSDPNotifyListner);
	cg_upnp_controlpoint_seteventlistener(ctrlPoint, EventListener);
	if (cg_upnp_controlpoint_start(ctrlPoint) == FALSE) {
		printf("Couldn't start this control point !!");
		exit(1);
	}
	
	PrintKeyMessage();

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(TENGINE)
	kbinit();
#endif
	
	key = 0;
	do
	{
#if !defined(TENGINE)
		key = kbgetkey();
		key = toupper(key);
		switch (key) {
		case 'P':
			PrintControlPointDevice(ctrlPoint);
			break;
#if !defined(CG_UPNP_NOUSE_ACTIONCTRL)
		case 'C':
			ControlDeviceAlter(ctrlPoint, CMD_NO_ALTERATIONS);
			break;

		case 'O':
			ControlDeviceAlter(ctrlPoint, CMD_LOOP_ACTION_CALLS);
			break;
#endif
#if !defined(CG_UPNP_NOUSE_QUERYCTRL)
		case 'Q':
			QueryDevice(ctrlPoint);
			break;
#endif
#if !defined(CG_UPNP_NOUSE_SUBSCRIPTION)
		case 'S':
			SubscribeService(ctrlPoint);
			break;
		case 'U':
			UnsubscribeService(ctrlPoint);
			break;
#endif
                case 'M':
                        SetMXValue(ctrlPoint);
                        break;
		case 'R':
	          printf("M-Search upnp::rootdevice\n");
		  cg_upnp_controlpoint_search(ctrlPoint, CG_UPNP_ST_ROOT_DEVICE);
		  break;
		case 'H':
	          printf("M-Search ssdp:all\n");
		  cg_upnp_controlpoint_search(ctrlPoint, "ssdp:all");
		  break;
		case 'V':
	          printf("M-Search device type\n");
		  cg_upnp_controlpoint_search(ctrlPoint, "urn:schemas-upnp-org:device:clock:1");
		  break;
		case 'T':
	          printf("M-Search service type\n");
		  cg_upnp_controlpoint_search(ctrlPoint, "urn:schemas-upnp-org:service:timer:1");
		  break;
		case 'A':
		  {
			  int i;

			  printf("Doing all searches four times...\n");
			  
			  for (i=0; i<4; i++)
			  {
				  cg_upnp_controlpoint_search(ctrlPoint, CG_UPNP_ST_ROOT_DEVICE);
				  cg_upnp_controlpoint_search(ctrlPoint, "ssdp:all");
				  cg_upnp_controlpoint_search(ctrlPoint, "urn:schemas-upnp-org:device:clock:1");
				  cg_upnp_controlpoint_search(ctrlPoint, "urn:schemas-upnp-org:service:timer:1");
			  }
			  
			  printf("Done (doing all searches four times).\n");
		  }
		  break;
		case 'X':
			break;
		default:
			PrintKeyMessage();
		}
#else
		cg_wait(1000);
#endif
	} while( key != 'X');

#if (!defined(WIN32) || defined(__CYGWIN__)) && !defined(TENGINE)
	kbexit();
#endif
	
	cg_upnp_controlpoint_stop(ctrlPoint);
	cg_upnp_controlpoint_delete(ctrlPoint);
	
	return(0);
}

