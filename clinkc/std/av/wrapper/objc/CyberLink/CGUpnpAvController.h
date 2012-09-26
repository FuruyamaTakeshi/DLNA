//
//  CGUpnpAvController.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/06/25.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>

#import <CyberLink/UPnP.h>

@class CGUpnpAvServer;
@class CGUpnpAvObject;
@class CGUpnpAvRenderer;

/**
 * The CGUpnpControlPoint class is a wrapper class for CgUpnpControlPoint of CyberLink for C to 
 * program using only Objective-C directly on MacOSX.
 * Currently, the only basic methods are wrapped to control UPnP devices.
 */
@interface CGUpnpAvController : CGUpnpControlPoint 
{
}
/**
 * Activate some background threads of the control point such as SSDP and 
 * HTTP servers to listen messages and events of UPnP. You must call this 
 * method before you can actually use a control point.
 *
 * @return TRUE if successful; otherwise FALSE
 *
 */
- (NSArray *)servers;
- (CGUpnpAvServer *)serverForUDN:(NSString *)aUdn;
- (CGUpnpAvServer *)serverForFriendlyName:(NSString *)aFriendlyName;
- (CGUpnpAvServer *)serverForPath:(NSString *)aPath;
- (CGUpnpAvServer *)serverForIndexPath:(NSIndexPath *)aIndexPath;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aServerAndTitlePath;
- (CGUpnpAvObject *)objectForIndexPath:(NSIndexPath *)aServerAndTitleIndexPath;
- (NSArray *)browseDirectChildrenWithTitlePath:(NSString *)aServerAndTitlePath;
- (NSArray *)browseDirectChildrenWithIndexPath:(NSIndexPath *)aServerAndTitleIndexPath;

- (NSArray *)renderers;
- (CGUpnpAvRenderer *)rendererForUDN:(NSString *)aUdn;

- (void)search;

@end
