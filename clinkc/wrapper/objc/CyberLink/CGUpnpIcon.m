//
//  CGUpnpIcon.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cicon.h>
#import "CGUpnpIcon.h"

@implementation CGUpnpIcon

@synthesize cObject;
#if defined(TARGET_OS_IPHONE)
@synthesize resourceName;
#endif

- (id) initWithCObject:(CgUpnpIcon *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	return self;
}

- (id) init
{
	if ((self = [self initWithCObject:NULL]) == nil)
    	return nil;
	return self;
}

- (void) dealloc
{
	self.resourceName = nil;
    
	[super dealloc];
}

- (void) finalize
{
	[super finalize];
}

- (NSString *)url
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_icon_geturl(cObject)] autorelease];
}

- (NSString *)mimeType
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_icon_getmimetype(cObject)] autorelease];
}

- (NSInteger)width
{
	if (!cObject)
		return 0;
	return cg_upnp_icon_getwidth(cObject);
}

- (NSInteger)height
{
	if (!cObject)
		return 0;
	return cg_upnp_icon_getheight(cObject);
}

- (NSInteger)depth
{
	if (!cObject)
		return 0;
	return cg_upnp_icon_getdepth(cObject);
}

#if defined(TARGET_OS_IPHONE)
- (void)setResourceName:(NSString *) aResourceName
{
	//UIImage *image = [UIImage imageNamed:aResourceName];
	//CGSize imageSize = CGSizeMake(CGImageGetWidth([image CGImage]), CGImageGetHeight([image CGImage]));
}
#endif

@end
