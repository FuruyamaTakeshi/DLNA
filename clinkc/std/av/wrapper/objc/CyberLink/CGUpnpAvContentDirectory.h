//
//  CGUpnpAvContentDirectory.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSString.h>

@class CGUpnpAvContentManager;

/**
 * The CGUpnpAvContentDirectory class has UPnP/AV objects of a media servers.
 *
 */
@interface CGUpnpAvContentDirectory : NSObject
{
	CGUpnpAvContentManager *contentMgr;
}
- (CGUpnpAvContainer *)rootObject;
- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath;
@end

