/************************************************************
*
*	CyberLink for C
*
*	Copyright (C) Satoshi Konno 2005
*
*	File: clock_device.h
*
*	Revision:
*       05/11/05
*               - first release.
*
************************************************************/

#ifndef _CG_CLOCK_DEVICE_H_
#define _CG_CLOCK_DEVICE_H_

#include <cybergarage/upnp/cupnp.h>

#ifdef  __cplusplus
extern "C" {
#endif

/****************************************
* Function
****************************************/

CgUpnpDevice *upnp_clock_device_new();
void upnp_clock_device_update(CgUpnpDevice *dev);

#ifdef  __cplusplus
}
#endif

#endif
