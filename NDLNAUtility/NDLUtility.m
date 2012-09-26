//
//  NDLUtility.m
//  DLNASample
//
//  Created by 田中 浩一 on 12/06/28.
//  Copyright (c) 2012 エヌエスプランニング株式会社. All rights reserved.
//

#import "NDLUtility.h"
#import <CyberLink/UPnPAV.h>

@interface NDLUtility ()

@end

@implementation NDLUtility

+ (NSArray*)getDmsInfo:(CGUpnpDevice *)dev dmsNum:(int) dmsNum
{
	NSLog(@"[%d] %@", dmsNum, [dev friendlyName]);
    
	CGUpnpService *conDirService = [dev getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return nil;
    
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
		return nil;
    
	[browseAction setArgumentValue:@"0" forName:@"ObjectID"];
	[browseAction setArgumentValue:@"BrowseDirectChildren" forName:@"BrowseFlag"];
	[browseAction setArgumentValue:@"*" forName:@"Filter"];
	[browseAction setArgumentValue:@"0" forName:@"StartingIndex"];
	[browseAction setArgumentValue:@"0" forName:@"RequestedCount"];
	[browseAction setArgumentValue:@"" forName:@"SortCriteria"];
	
	if (![browseAction post])
		return nil;
	
	NSMutableString *indentStr = [NSMutableString string];
	for (int n=0; n<0; n++)
		[indentStr appendString:@"  "];
    
	NSString *resultStr = [browseAction argumentValueForName:@"Result"];
	NSLog(@"%@", resultStr);
    NSArray* array = [CGUpnpAvObject arrayWithXMLString:resultStr];
    
    for (CGUpnpAvObject* avObj in array) {
        NSLog(@"####%@", avObj.title);
    }
    return array;
}

+ (NSArray*)getContainer:(CGUpnpAvObject*)aAvObject objectId:(NSString*)aObjectId
{
    CGUpnpService *conDirService = [aAvObject getServiceForType:@"urn:schemas-upnp-org:service:ContentDirectory:1"];
	if (!conDirService)
		return nil;

    CGUpnpAction *browseAction = [conDirService getActionForName:@"Browse"];
	if (!browseAction)
		return nil;
    
	[browseAction setArgumentValue:aObjectId forName:@"ObjectID"];
	[browseAction setArgumentValue:@"BrowseDirectChildren" forName:@"BrowseFlag"];
	[browseAction setArgumentValue:@"*" forName:@"Filter"];
	[browseAction setArgumentValue:@"0" forName:@"StartingIndex"];
	[browseAction setArgumentValue:@"0" forName:@"RequestedCount"];
	[browseAction setArgumentValue:@"" forName:@"SortCriteria"];
	
	if (![browseAction post])
		return nil;
	
	NSMutableString *indentStr = [NSMutableString string];
	for (int n=0; n<0; n++)
		[indentStr appendString:@"  "];
    
	NSString *resultStr = [browseAction argumentValueForName:@"Result"];
	NSLog(@"%@", resultStr);
    NSArray* array = [CGUpnpAvObject arrayWithXMLString:resultStr];
    
    for (CGUpnpAvObject* avObj in array) {
        NSLog(@"####%@", avObj.title);
    }
    return array;

    
}

@end
