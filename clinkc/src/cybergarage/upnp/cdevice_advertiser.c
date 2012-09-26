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
*	File: cdevice_advertiser.c
*
*	Revision:
*
*	06/14/05
*		- first revision
*
*	10/31/05
*		- Added comments
*
*	03/20/06 Theo Beisch
*		- added NULL check on advertiser_stop
*
******************************************************************/

#include <cybergarage/upnp/cdevice.h>
#include <cybergarage/util/ctime.h>
#include <cybergarage/util/clog.h>

/**
 * cg_upnp_device_advertiser_action
 * 
 * Function run as a UPnP device advertisement
 * thread.
 *
 * \param thread The thread context that this function is running in
 */
static void cg_upnp_device_advertiser_action(CgThread *thread) 
{
	CgUpnpDevice *dev;
	CgSysTime leaseTime;
	CgSysTime notifyInterval;
  
	cg_log_debug_l4("Entering...\n");

  /**
   * Get the device struct that this function is advertising.
   */
  dev = (CgUpnpDevice *)cg_thread_getuserdata(thread);

  /**
   * Lease time
   */
  leaseTime = cg_upnp_device_getleasetime(dev);
  
  /* Run this thread until it is stopped from outside */
    while ( cg_thread_isrunnable(thread) == TRUE )
    {
      notifyInterval = (leaseTime/4) + (long)((float)leaseTime * (cg_random() * 0.25f));
      notifyInterval *= 1000;

      /* Sleep *notifyInterval* msecs */
      cg_wait(notifyInterval); 

      /* Check if we must exit before sending new announce */
      if ( cg_thread_isrunnable(thread) == FALSE ) break;

      /* Initiate advertise routine after sleeping */
      cg_upnp_device_announce(dev);
    }

	cg_log_debug_l4("Leaving...\n");
}

/**
 * cg_upnp_device_advertiser_start
 *
 * Start the advertiser thread for the given device
 *
 * \param dev The device that is to be advertised
 */
BOOL cg_upnp_device_advertiser_start(CgUpnpDevice *dev)
{
	CgThread *advertiser;
  
	cg_log_debug_l4("Entering...\n");

  /* Get the already existing (created in device_new) advertiser thread */
  advertiser = cg_upnp_device_getadvertiserthead(dev);

  /* Store the device into the advertiser thread struct's user data */
  cg_thread_setuserdata(advertiser, dev);

  /* Set the function that will run the thread */
  cg_thread_setaction(advertiser, cg_upnp_device_advertiser_action);

  /* Start the thread */
  return cg_thread_start(advertiser);

	cg_log_debug_l4("Leaving...\n");
}

/**
 * cg_upnp_device_advertiser_stop
 *
 * Stop the advertiser thread for the given device
 *
 * \param dev Device that is being advertised
 */
BOOL cg_upnp_device_advertiser_stop(CgUpnpDevice *dev)
{
	CgThread *advertiser;
  
	cg_log_debug_l4("Entering...\n");

	/* Get the advertiser thread from the device struct */
	advertiser = cg_upnp_device_getadvertiserthead(dev);
  
	//Theo Beisch 
	if (advertiser != NULL)
 		return cg_thread_stop(advertiser);
	return FALSE;
}

/**
 * cg_upnp_device_advertiser_isrunning
 *
 * Check if the advertiser has been started
 *
 * \param dev Device that is being advertised
 */
BOOL cg_upnp_device_advertiser_isrunning(CgUpnpDevice *dev)
{
	CgThread *advertiser;
	
	cg_log_debug_l4("Entering...\n");
	
	advertiser = cg_upnp_device_getadvertiserthead(dev);
	if (advertiser != NULL)
 		return cg_thread_isrunnable(advertiser);

	return FALSE;
}
