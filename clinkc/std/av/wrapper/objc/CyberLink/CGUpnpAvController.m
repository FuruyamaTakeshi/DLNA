//
//  CGUpnpAvController.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/06/25.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/ssdp/cssdp.h>
#include <cybergarage/upnp/std/av/cmediaserver.h>

#import "CGUpnpAvController.h"
#import "CGUpnpAvServer.h"
#import "CGUpnpAvConstants.h"
#import "CGXml.h"
#import "CGXmlNode.h"
#import "CGUpnpAvObject.h"
#import "CGUpnpAvContainer.h"
#import "CGUpnpAvRenderer.h"

@implementation CGUpnpAvController

- (id)init
{
	if ((self = [super init]) == nil)
		return nil;
	
	[self setSsdpSearchMX:1];
	
	return self;
}

- (void)dealloc
{
	[super dealloc];
}

- (void)finalize
{
	[super finalize];
}

////////////////////////////////////////////////////////////
// Media Server
////////////////////////////////////////////////////////////

- (NSArray *)servers;
{
	NSArray *devices = [self devices];
	NSMutableArray *serverArray = [[[NSMutableArray alloc] init] autorelease];

	for (CGUpnpDevice *dev in devices) {
		if (![dev isDeviceType:@CG_UPNPAV_DMS_DEVICE_TYPE])
			continue;
		CGUpnpAvServer *server = nil;		
		void *devData = [dev userData];
		if (!devData) {
			CgUpnpDevice *cDevice = [dev cObject];
			if (!cDevice)
				continue;
			server = [[[CGUpnpAvServer alloc] initWithCObject:cDevice] autorelease];
			[server setUserObject:server];
		}
		else 
			server = (CGUpnpAvServer *)((id)devData);
		if (server == nil)
			continue;
		[serverArray addObject:server];
	}
	return serverArray;
}

- (CGUpnpAvServer *)serverForUDN:(NSString *)aUdn
{
	if (aUdn == nil)
		return nil;
	NSArray *servers = [self servers];
	for (CGUpnpAvServer *server in servers) {
		if ([server isUDN:aUdn])
			return [[server retain] autorelease];
	}
	return nil;
}

- (CGUpnpAvServer *)serverForFriendlyName:(NSString *)aFriendlyName
{
	if (aFriendlyName == nil)
		return nil;
	NSArray *servers = [self servers];
	for (CGUpnpAvServer *server in servers) {
		if ([server isFriendlyName:aFriendlyName])
			return [[server retain] autorelease];
	}
	return nil;
}

- (CGUpnpAvServer *)serverForPath:(NSString *)aPath;
{
	NSArray *srvAndObjPathArray = [aPath pathComponents];
	if ([srvAndObjPathArray count] <= 0)
		return nil;
	if ([aPath isAbsolutePath] && ([srvAndObjPathArray count] <= 1))
		return nil;
	NSString *avSrvName = [CGXml unescapestring:[aPath isAbsolutePath] ? [srvAndObjPathArray objectAtIndex:1] : [srvAndObjPathArray objectAtIndex:0]];
	return [self serverForFriendlyName:avSrvName];
}

- (CGUpnpAvServer *)serverForIndexPath:(NSIndexPath *)aIndexPath
{
	int idxCnt = [aIndexPath length];
	if (idxCnt < 1)
		return nil;
	
	NSArray *servers = [self servers];
	int serverNum = [aIndexPath indexAtPosition:0];
	if ([servers count] <= serverNum)
		return nil;
	
	return [servers objectAtIndex:serverNum];
}

- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aServerAndTitlePath
{
	CGUpnpAvServer *avSrv = [self serverForPath:aServerAndTitlePath];
	if (avSrv == nil)
		return nil;

	NSMutableArray *titlePathArray = [NSMutableArray arrayWithArray:[aServerAndTitlePath pathComponents]];
	[titlePathArray removeObjectAtIndex:0];
	if (aServerAndTitlePath.isAbsolutePath)
		[titlePathArray removeObjectAtIndex:0];

	NSString *titlePath = [NSString pathWithComponents:titlePathArray];
	CGUpnpAvObject *avObj = [avSrv objectForTitlePath:titlePath];
	
	return [[avObj retain] autorelease];
}

- (CGUpnpAvObject *)objectForIndexPath:(NSIndexPath *)aServerAndTitleIndexPath
{
	CGUpnpAvServer *avSrv = [self serverForIndexPath:aServerAndTitleIndexPath];
	if (!avSrv)
		return nil;
	
	CGUpnpAvContainer*rootObj = [avSrv rootObject];
	if (!rootObj)
		return nil;

	CGUpnpAvObject *avObj = rootObj;
	int idxPathLength = [aServerAndTitleIndexPath length];
	int n;
	for (n=1; n<idxPathLength; n++) {
		if ([avObj isItem])
			return nil;
		CGUpnpAvContainer *avCon = (CGUpnpAvContainer *)avObj;
		int avObjIdx = [aServerAndTitleIndexPath indexAtPosition:n];
		avObj = [avCon childAtIndex:avObjIdx];
		NSLog(@"objectForIndexPath = [%d/%d][%d/%d] %@ (%@)", n, idxPathLength, avObjIdx,  [avCon childCount], [avObj title], [avObj objectId]);
		if (!avObj)
			return nil;
	}
	
	
	return [[avObj retain] autorelease];
}

- (NSArray *)browseDirectChildrenWithTitlePath:(NSString *)aServerAndTitlePath
{
	CGUpnpAvServer *avSrv = [self serverForPath:aServerAndTitlePath];
	if (avSrv == nil)
		return nil;
	
	CGUpnpAvObject *avObj = [self objectForTitlePath:aServerAndTitlePath];
	if (avObj == nil)
		return nil;
	
	return [avSrv browseDirectChildren:[avObj objectId]];
}

- (NSArray *)browseDirectChildrenWithIndexPath:(NSIndexPath *)aServerAndTitleIndexPath
{
	CGUpnpAvServer *avSrv = [self serverForIndexPath:aServerAndTitleIndexPath];
	if (avSrv == nil)
		return nil;
	
	CGUpnpAvObject *avObj = [self objectForIndexPath:aServerAndTitleIndexPath];
	if (avObj == nil)
		return nil;

	return [avSrv browseDirectChildren:[avObj objectId]];
}

////////////////////////////////////////////////////////////
// Media Renderer
////////////////////////////////////////////////////////////

- (NSArray *)renderers;
{
	NSArray *devices = [self devices];
	NSMutableArray *rendererrArray = [[[NSMutableArray alloc] init] autorelease];
	
	for (CGUpnpDevice *dev in devices) {
		if (![dev isDeviceType:@CG_UPNPAV_DMR_DEVICE_TYPE])
			continue;
		CgUpnpDevice *cDevice = [dev cObject];
		if (!cDevice)
			continue;
		CGUpnpAvRenderer *renderer = [[[CGUpnpAvRenderer alloc] initWithCObject:cDevice] autorelease];
		if (renderer == nil)
			continue;
		[rendererrArray addObject:renderer];
	}
	return rendererrArray;
}

- (CGUpnpAvRenderer *)rendererForUDN:(NSString *)aUdn
{
	if (aUdn == nil)
		return nil;
	NSArray *renderers = [self renderers];
	for (CGUpnpAvRenderer *renderer in renderers) {
		if ([renderer isUDN:aUdn])
			return [[renderer retain] autorelease];
	}
	return nil;
}

////////////////////////////////////////////////////////////
// Search
////////////////////////////////////////////////////////////

- (void)search
{
	[super search];
#if defined(CG_UPNPAVCONTROLLER_SERCH_DEVICETYPES)
	[super searchWithST:[NSString stringWithUTF8String:CG_UPNPAV_DMS_DEVICE_TYPE]];
	[super searchWithST:[NSString stringWithUTF8String:CG_UPNPAV_DMR_DEVICE_TYPE]];
#endif
}

@end

