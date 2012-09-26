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

#ifndef _CG_TEST_DEVICE_H_
#define _CG_TEST_DEVICE_H_

#include <cybergarage/upnp/cupnp.h>

#ifdef  __cplusplus
extern "C" {
#endif

#define TEST_DEVICE_DEVICE_TYPE "urn:schemas-upnp-org:device:BinaryLight:1"

#define TEST_DEVICE_EMBEDDEDDEVICE_TYPE1 "urn:schemas-upnp-org:device:EmbeddedsBinaryLight:1"
#define TEST_DEVICE_EMBEDDEDDEVICE_TYPE2 "urn:schemas-upnp-org:device:EmbeddedsBinaryLight:2"

#define TEST_DEVICE_SERVICE_TYPE "urn:schemas-upnp-org:service:SwitchPower:1"

#define TEST_DEVICE_FREINDLYNAME "CyberGarage Power Device"
#define TEST_DEVICE_STATEVARIABLE_STATUS "Status"

/****************************************
* Function
****************************************/

CgUpnpDevice *upnp_test_device_new();

#ifdef  __cplusplus
}
#endif

#endif
