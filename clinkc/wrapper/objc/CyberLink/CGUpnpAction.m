//
//  CGUpnpAction.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/caction.h>
#include <cybergarage/upnp/control/ccontrol.h>
#import "CGUpnpAction.h"

static BOOL CGUpnpActionListener(CgUpnpAction *cAction)
{
	if (!cAction)
		return FALSE;
	CGUpnpAction *objcAction = (CGUpnpAction *)cg_upnp_action_getuserdata(cAction);
	if (!objcAction)
		return FALSE;
	SEL actionReceived = @selector(actionReceived);		
	if (!actionReceived)
		return FALSE;
	if (![objcAction respondsToSelector: actionReceived])
		return FALSE;
        [objcAction performSelector:actionReceived];
	return TRUE;
}

@implementation CGUpnpAction

@synthesize cObject;

- (id) initWithCObject:(CgUpnpAction *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	cg_upnp_action_setuserdata(cObject, self);
	cg_upnp_action_setlistener(cObject, CGUpnpActionListener);
	return self;
}

- (id) init
{
	[self initWithCObject:NULL];
	return self;
}

- (void) dealloc
{
	[super dealloc];
}

- (void) finalize
{
	[super finalize];
}

- (NSString *)name
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_action_getname(cObject)] autorelease];
}

- (NSDictionary *)arguments
{
	if (!cObject)
		return [NSDictionary dictionary];
	NSMutableDictionary *argDir = [NSMutableDictionary dictionary];
	CgUpnpArgument *carg;
	for (carg = cg_upnp_action_getarguments(cObject); carg; carg = cg_upnp_argument_next(carg)) {
		char *name = cg_upnp_argument_getname(carg);
		char *value = cg_upnp_argument_getvalue(carg);
		NSString *obj = [[NSString alloc] initWithUTF8String:(value ? value : "")];
		NSString *key = [[NSString alloc] initWithUTF8String:name];
		[argDir setObject:obj forKey:key];
		[obj release];
		[key release];
	}
	return argDir;
}

- (BOOL)setArgumentValue:(NSString *)value forName:(NSString *)name
{
	CgUpnpArgument *cArg;

	if (!cObject)
		return NO;
	cArg = cg_upnp_action_getargumentbyname(cObject, (char *)[name UTF8String]);
	if (!cArg)
		return NO;
	cg_upnp_argument_setvalue(cArg, (char *)[value UTF8String]);
	return YES;
}

- (NSString *)argumentValueForName:(NSString *)name
{
	char *cValue;
	CgUpnpArgument *cArg;

	if (!cObject)
		return nil;
	cArg = cg_upnp_action_getargumentbyname(cObject, (char *)[name UTF8String]);
	if (!cArg)
		return nil;
	cValue = cg_upnp_argument_getvalue(cArg);
	if (cg_strlen(cValue) <= 0)
		return nil;
	return [NSString stringWithUTF8String:cValue];
}

- (BOOL)post
{
	if (!cObject)
		return NO;
	BOOL ret = cg_upnp_action_post(cObject);
	return ret;
}

- (BOOL)postWithArguments:(NSDictionary *)arguments
{
	NSString *name;

	for (name in arguments) {
		NSString *value = [arguments valueForKey:name];
		[self setArgumentValue:value forName:name];
	}
	return [self post];
}

- (NSInteger)statusCode
{
	if (!cObject)
		return 0;
	return cg_upnp_action_getstatuscode(cObject);
}

@end
