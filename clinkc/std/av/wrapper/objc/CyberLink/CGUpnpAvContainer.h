//
//  CGUpnpAvContainer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CyberLink/CGUpnpAvObject.h>

/**
 * The CGUpnpAvContainer class is a container of UPnP/AV container.
 */
@interface CGUpnpAvContainer : CGUpnpAvObject
{
	NSMutableArray *childArray;
}
- (id)init;
#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
#else
- (id)initWithXMLNode:(CgXmlNode *)aXmlNode;
#endif
- (void)addChild:(CGUpnpAvObject *)obj;
- (void)addChildren:(NSArray *)objArray;
- (void)removeChild:(CGUpnpAvObject *)obj;
- (void)removeAllChildren;
- (NSArray *)children;
- (CGUpnpAvObject *)childAtIndex:(NSUInteger)anIndex;
- (CGUpnpAvObject *)childforId:(NSString *)anObjectId;
- (CGUpnpAvObject *)childforTitle:(NSString *)aTitle;
- (CGUpnpAvObject *)objectForId:(NSString *)anObjectId;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath;
@end

