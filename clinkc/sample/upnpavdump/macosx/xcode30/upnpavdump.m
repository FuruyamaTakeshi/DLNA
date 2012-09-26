//
//  upnpavdump.c
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CyberLink/UPnP.h>

void PrintContentDirectory(CGUpnpAction *browseAction, int indent, NSString *objectId)
{
	int n;
	
	[browseAction setArgumentValue:objectId forName:@"ObjectID"];
	[browseAction setArgumentValue:@"BrowseDirectChildren" forName:@"BrowseFlag"];
	[browseAction setArgumentValue:@"*" forName:@"Filter"];
	[browseAction setArgumentValue:@"0" forName:@"StartingIndex"];
	[browseAction setArgumentValue:@"0" forName:@"RequestedCount"];
	[browseAction setArgumentValue:@"" forName:@"SortCriteria"];
	
	if (![browseAction post])
		return;
	
	NSMutableString *indentStr = [NSMutableString string];
	for (n=0; n<indent; n++)
		[indentStr appendString:@"  "];
		
	NSString *resultStr = [browseAction argumentValueForName:@"Result"];
	//NSLog(@"%@", resultStr);
	
	NSError *xmlErr;
	NSXMLDocument *xmlDoc = [[NSXMLDocument alloc] initWithXMLString:resultStr options:0 error:&xmlErr];
	if (!xmlDoc)
		return;
	NSArray *contentArray = [xmlDoc nodesForXPath:@"/DIDL-Lite/*" error:&xmlErr];
	for (NSXMLElement *contentNode in contentArray) {
		NSString *objId = [[contentNode attributeForName:@"id"] stringValue];
		NSArray *titleArray = [contentNode elementsForName:@"dc:title"];
		NSString *title = @"";
		for (NSXMLNode *titleNode in titleArray) {
			title = [titleNode stringValue];
			break;
		}
		if ([[contentNode name] isEqualToString:@"container"]) {
			NSLog(@"%@  [%@] %@", indentStr, objId, title);
			PrintContentDirectory(browseAction, (indent+1), objId);
		}
		else {
			NSArray *resArray = [contentNode elementsForName:@"res"];
			NSString *url = @"";
			for (NSXMLNode *resNode in resArray) {
				url = [resNode stringValue];
				break;
			}
			NSLog(@"%@  [%@] %@ (%@)", indentStr, objId, title, url);
		}
	}
}

void PrintDmsInfo(CGUpnpDevice *dev, int dmsNum) 
{
	NSLog(@"[%d] %@", dmsNum, [dev friendlyName]);

	CGUpnpService *conDirService = [dev getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return;

	CGUpnpStateVariable *searchCap = [conDirService getStateVariableForName:@"SearchCapabilities"];
	if (searchCap) {
		if ([searchCap query])
			NSLog(@"  SearchCapabilities = %@\n", [searchCap value]);
	}

	CGUpnpStateVariable *sorpCap = [conDirService getStateVariableForName:@"SortCapabilities"];
	if (sorpCap) {
		if ([sorpCap query])
			NSLog(@"  SortCapabilities = %@\n", [sorpCap value]);
	}

	CGUpnpAction *browseAction = [conDirService getActionForName:@"Browse"];
	if (!browseAction)
		return;

	PrintContentDirectory(browseAction, 0, @"0");
}

int main (int argc, const char * argv[])
{
	NSAutoreleasePool * pool = [[NSAutoreleasePool alloc] init];

	CGUpnpControlPoint *ctrlPoint = [[CGUpnpControlPoint alloc] init];

	[ctrlPoint search];

	int dmsNum = 0;
	NSArray *devArray = [ctrlPoint devices];
	for (CGUpnpDevice *dev in devArray) {
		if ([dev isDeviceType:@"urn:schemas-upnp-org:device:MediaServer:1"])
			PrintDmsInfo(dev, ++dmsNum);
	}

	if (dmsNum <= 0)
		NSLog(@"Media Server is not found !!\n");

	[pool drain];
	return 0;
}
