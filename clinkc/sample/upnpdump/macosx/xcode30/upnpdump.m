//
//  upnpdump.c
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CyberLink/UPnP.h>

void PrintDevice(CGUpnpDevice *dev) 
{
	static int devNum = 0;
	int serviceNum = 0;
	int actionNum = 0;
	int argNum = 0;
	int statVarNum = 0;
	
	NSLog(@"[%d] %@", ++devNum, [dev friendlyName]);
	NSLog(@"  deviceType = %@", [dev deviceType]);

	NSArray *serviceArray = [dev services];
	for (CGUpnpService *service in serviceArray) {
		NSLog(@"    [%d] service = %@", ++serviceNum, [service serviceType]);

		NSArray *actionArray = [service actions];
		actionNum = 0;
		for (CGUpnpAction *action in actionArray) {
			//NSLog(@"      [%d] action = %@", ++actionNum, [action name]);
			NSDictionary *argDir = [action arguments];
			argNum = 0;
			for (NSString *name in argDir) {
				//NSLog(@"        [%d] argument = %@", ++argNum, name);
			}
		}

		NSArray *statVarArray = [service stateVariables];
		statVarNum = 0;
		for (CGUpnpStateVariable *statVar in statVarArray) {
			//NSLog(@"      [%d] stateVariable = %@", ++statVarNum, [statVar name]);
		}
	}
}

int main (int argc, const char * argv[])
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];
	[ctrlPoint search];
	NSArray *devArray = [ctrlPoint devices];
	for (CGUpnpDevice *dev in devArray)
		PrintDevice(dev);

	[pool drain];
	return 0;
}
