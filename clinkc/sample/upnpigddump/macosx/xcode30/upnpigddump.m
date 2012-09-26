//
//  upnpigddump.c
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CyberLink/UPnP.h>

void PrintIGDInfo(CGUpnpDevice *dev, int igdNum) 
{
	NSLog(@"[%d] %@", igdNum, [dev friendlyName]);

	CGUpnpService *ipConService = [dev getServiceForType:@"urn:schemas-upnp-org:service:WANIPConnection:1"];
	if (ipConService) {
		CGUpnpAction *extIpAddrAction = [ipConService getActionForName:@"GetExternalIPAddress"];
		if (extIpAddrAction) {
			if ([extIpAddrAction post])
				NSLog(@"  GetExternalIPAddress = %@", [extIpAddrAction argumentValueForName:@"NewExternalIPAddress"]);
		}
	}

	CGUpnpService *wanComIfCfgService = [dev getServiceForType:@"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1"];
	if (wanComIfCfgService) {
		CGUpnpAction *totalBytesSentAction = [wanComIfCfgService getActionForName:@"GetTotalBytesSent"];
		if (totalBytesSentAction) {
			if ([totalBytesSentAction post])
				NSLog(@"  GetTotalBytesSent = %@", [totalBytesSentAction argumentValueForName:@"NewTotalBytesSent"]);
		}
		CGUpnpAction *totalBytesRecvAction = [wanComIfCfgService getActionForName:@"GetTotalBytesReceived"];
		if (totalBytesRecvAction) {
			if ([totalBytesRecvAction post])
				NSLog(@"  GetTotalBytesReceived = %@", [totalBytesRecvAction argumentValueForName:@"NewTotalBytesReceived"]);
		}
	}
	
}

int main (int argc, const char * argv[])
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];

	[ctrlPoint search];

	int igdNum = 0;
	NSArray *devArray = [ctrlPoint devices];
	for (CGUpnpDevice *dev in devArray) {
		if ([dev isDeviceType:@"urn:schemas-upnp-org:device:InternetGatewayDevice:1"])
			PrintIGDInfo(dev, ++igdNum);
	}

	if (igdNum <= 0)
		NSLog(@"IGD is not found !!\n");

	[pool drain];
	return 0;
}
