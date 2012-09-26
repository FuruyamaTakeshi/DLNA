//
//  CGUpnpControlPoint.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/ccontrolpoint.h>
#include <cybergarage/upnp/control/ccontrol.h>
#import "CGUpnpControlPoint.h"
#import "CGUpnpDevice.h"

static void CGUpnpControlPointDeviceListener(CgUpnpControlPoint *ctrlPoint, char* udn, CgUpnpDeviceStatus status);

@implementation CGUpnpControlPoint

@synthesize cObject;
@synthesize delegate;

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cg_upnp_controlpoint_new();
	if (cObject) {
		cg_upnp_controlpoint_setdevicelistener(cObject, CGUpnpControlPointDeviceListener);
		cg_upnp_controlpoint_setuserdata(cObject, self);
		if (![self start])
			self = nil;
	}
	else
		self = nil;
	return self;
}

- (void)dealloc
{
	if (cObject)
		cg_upnp_controlpoint_delete(cObject);
	[super dealloc];
}

- (void)finalize
{
	if (cObject)
		cg_upnp_controlpoint_delete(cObject);
	[super finalize];
}

- (BOOL)start
{
	if (!cObject)
		return NO;
	return cg_upnp_controlpoint_start(cObject);
}

- (BOOL)stop
{
	if (!cObject)
		return NO;
	return cg_upnp_controlpoint_stop(cObject);
}

-(BOOL)isRunning
{
	if (!cObject)
		return NO;
	return cg_upnp_controlpoint_isrunning(cObject);
}

- (void)search
{
	[self searchWithST:[NSString stringWithUTF8String:CG_UPNP_NT_ROOTDEVICE]];
}

- (void)searchWithST:(NSString *)aST
{
	if (!cObject)
		return;
	cg_upnp_controlpoint_search(cObject, (char *)[aST UTF8String]);
	
#if defined(CG_UPNPCONTROLPOINT_ENABLE_SEARCH_SLEEP)
	int mx = cg_upnp_controlpoint_getssdpsearchmx(cObject);
	if (0 < mx)
		cg_sleep((mx * 1000));
#endif
}

- (NSInteger)ssdpSearchMX
{
	if (!cObject)
		return 0;
	return cg_upnp_controlpoint_getssdpsearchmx(cObject);
}

- (void)setSsdpSearchMX:(NSInteger)mx;
{
	if (!cObject)
		return;
	cg_upnp_controlpoint_setssdpsearchmx(cObject, (int)mx);
}

- (NSArray *)devices
{
	if (!cObject)
		return [[[NSArray alloc] init] autorelease];
	NSMutableArray *devArray = [[[NSMutableArray alloc] init] autorelease];
	CgUpnpDevice *cDevice;
	for (cDevice = cg_upnp_controlpoint_getdevices(cObject); cDevice; cDevice = cg_upnp_device_next(cDevice)) {
		CGUpnpDevice *device = [[[CGUpnpDevice alloc] initWithCObject:cDevice] autorelease];
		[devArray addObject:device];
	}
	return devArray;
}

- (CGUpnpDevice *)deviceForUDN:(NSString *)udn
{
	if (!cObject)
		return nil;
	CgUpnpDevice *cDevice;
	for (cDevice = cg_upnp_controlpoint_getdevices(cObject); cDevice; cDevice = cg_upnp_device_next(cDevice)) {
		if (cg_strcmp(cg_upnp_device_getudn(cDevice), (char *)[udn UTF8String]) == 0) 
			return [[[CGUpnpDevice alloc] initWithCObject:cDevice] autorelease];
	}
	return nil;
}

@end

static void CGUpnpControlPointDeviceListener(CgUpnpControlPoint *cCtrlPoint, char* udn, CgUpnpDeviceStatus status)
{
	CGUpnpControlPoint *ctrlPoint = cg_upnp_controlpoint_getuserdata(cCtrlPoint);
	if (ctrlPoint == nil)
		return;
	
	if ([ctrlPoint delegate] == nil)
		return;

	NSString *deviceUdn = [[NSString alloc ] initWithUTF8String:udn];
	
	switch (status) {
		case CgUpnpDeviceStatusAdded:
			{
				if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceAdded:)])
					[[ctrlPoint delegate] controlPoint:ctrlPoint deviceAdded:deviceUdn];
			}
			break;
		case CgUpnpDeviceStatusUpdated:
			{
				if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceUpdated:)])
					[[ctrlPoint delegate] controlPoint:ctrlPoint deviceUpdated:deviceUdn];
			}
			break;
		case CgUpnpDeviceStatusRemoved:
			{
				if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceRemoved:)])
					[[ctrlPoint delegate] controlPoint:ctrlPoint deviceRemoved:deviceUdn];
			}
			break;
		case CgUpnpDeviceStatusInvalid:
			{
				if ([[ctrlPoint delegate] respondsToSelector:@selector(controlPoint:deviceInvalid:)])
					[[ctrlPoint delegate] controlPoint:ctrlPoint deviceInvalid:deviceUdn];
			}
			break;
		default:
			break;
	}
	
	[deviceUdn release];
}

