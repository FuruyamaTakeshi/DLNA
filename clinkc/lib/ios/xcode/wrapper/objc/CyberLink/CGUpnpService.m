//
//  CGUpnpService.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#include <cybergarage/upnp/cservice.h>
#include <cybergarage/upnp/caction.h>
#include <cybergarage/upnp/cstatevariable.h>
#import "CGUpnpService.h"
#import "CGUpnpAction.h"
#import "CGUpnpStateVariable.h"

@implementation CGUpnpService

@synthesize cObject;

- (id) initWithCObject:(CgUpnpService *)cobj
{
	if ((self = [super init]) == nil)
		return nil;
	cObject = cobj;
	cg_upnp_service_setuserdata(cObject, self);
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

- (BOOL) parseXMLDescription:(NSString *)xmlDesc;
{
	if (!cObject)
		return NO;
	return cg_upnp_service_parsedescription(cObject, (char *)[xmlDesc UTF8String], [xmlDesc length]);
}

- (NSString *)serviceId
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_service_getserviceid(cObject)] autorelease];
}

- (NSString *)serviceType
{
	if (!cObject)
		return nil;
	return [[[NSString alloc] initWithUTF8String:cg_upnp_service_getservicetype(cObject)] autorelease];
}

- (NSArray *)actions
{
	if (!cObject)
		return [[[NSArray alloc] init] autorelease];
	NSMutableArray *actionArray = [[[NSMutableArray alloc] init] autorelease];
	CgUpnpAction *cAction;
	for (cAction = cg_upnp_service_getactions(cObject); cAction; cAction = cg_upnp_action_next(cAction)) {
		CGUpnpAction *action = [[[CGUpnpAction alloc] initWithCObject:(void *)cAction] autorelease];
		[actionArray addObject:action];
	}
	return actionArray;
}

- (NSArray *)stateVariables
{
	if (!cObject)
		return [[[NSArray alloc] init] autorelease];
	NSMutableArray *statVarArray = [[[NSMutableArray alloc] init] autorelease];
	CgUpnpStateVariable *cStatVar;
	for (cStatVar = cg_upnp_service_getstatevariables(cObject); cStatVar; cStatVar = cg_upnp_statevariable_next(cStatVar)) {
		CGUpnpStateVariable *statVar = [[[CGUpnpStateVariable alloc] initWithCObject:(void *)cStatVar] autorelease];
		[statVarArray addObject:statVar];
	}
	return statVarArray;
}

- (CGUpnpAction *)getActionForName:(NSString *)name
{
	if (!cObject)
		return nil;
	CgUpnpAction *cAction = cg_upnp_service_getactionbyname(cObject, (char *)[name UTF8String]);
	if (!cAction)
		return nil;
	return [[[CGUpnpAction alloc] initWithCObject:(void *)cAction] autorelease];
}

- (CGUpnpStateVariable *)getStateVariableForName:(NSString *)name
{
	if (!cObject)
		return nil;
	CgUpnpStateVariable *cStatVar = cg_upnp_service_getstatevariablebyname(cObject, (char *)[name UTF8String]);
	if (!cStatVar)
		return nil;
	return [[[CGUpnpStateVariable alloc] initWithCObject:(void *)cStatVar] autorelease];
}

- (BOOL)isStateVariableImpemented:(NSString*)name;
{
	CGUpnpStateVariable* stateVariable = [self getStateVariableForName:name];
	
	if (stateVariable)
	{
		return (![stateVariable isAllowedValue:@"NOT_IMPLEMENTED"]);
	}
	
	return NO;
}

@end
