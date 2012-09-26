//
//  upnpavdump.c
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CyberLink/UPnPAV.h>

void PrintBrowserInfo(CGUpnpAvServer *server, NSString *objectId, int indent) 
{
	NSMutableString *indentStr = [NSMutableString string];
	for (int n=0; n<indent; n++)
		[indentStr appendString:@"  "];
	
	NSArray *avObjects = [server browse:objectId];
	for (CGUpnpAvObject *avObj in avObjects) {
		NSLog(@"%@ [%@] %@", indentStr, [avObj objectId], [avObj title]);
		if ([avObj isContainer]) {
			PrintBrowserInfo(server, [avObj objectId], (indent+1));
		} else if ([avObj isItem]) {
			int resourceNum = 0;
			for (CGUpnpAvResource *avRes in [(CGUpnpAvItem *)avObj resources]) {
				NSLog(@"%@  res[%d] = %@", indentStr, ++resourceNum, [avRes url]);
			}
		}
	}	
}

int main (int argc, const char * argv[])
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	CGUpnpAvController *avCtrl = [[CGUpnpAvController alloc] init];

	[avCtrl search];

	int serverNum = 0;
	NSArray *servers = [avCtrl servers];
	for (CGUpnpAvServer *server in servers) {
		NSLog(@"[%d] %@", ++serverNum, [server friendlyName]);		
		PrintBrowserInfo(server, @"0", 0);
	}

	if (serverNum <= 0)
		NSLog(@"Media Server is not found !!\n");

	[pool drain];
	return 0;
}
