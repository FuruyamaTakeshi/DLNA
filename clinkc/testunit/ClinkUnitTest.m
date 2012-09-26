//
//  ClinkUnitTest.m
//  ClinkUnitTest
//
//  Created by Satoshi Konno on 08/07/15.
//  Copyright 2008 CyberGarage. All rights reserved.
//

#import "ClinkUnitTest.h"

@implementation ClinkUnitTest

- (void) setUp
{
	cp = [[CGUpnpControlPoint alloc] init];
}
 
- (void) tearDown
{
	[cp release];
}

- (void) testSearch
{
	[cp search];
	NSArray *devices = [cp devices];
	STAssertTrue(0 < [devices count], @"0 < [devices count]");
}

- (void) testControlPoint
{
	NSArray *devices = [cp devices];
	for (CGUpnpDevice *dev in devices) {
		STAssertNotNil(dev, nil);
		CGUpnpDevice *foundDev; 
		foundDev = [cp deviceForUDN:[dev udn]]; 
		STAssertNotNil(foundDev, nil);
		STAssertTrue(dev == foundDev, @"dev == foundDev");
	}
}

@end
