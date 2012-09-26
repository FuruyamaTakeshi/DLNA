//
//  CGUpnpAvItem.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CyberLink/CGUpnpAvObject.h>
#import <CyberLink/CGUpnpAvResource.h>

/**
 * The CGUpnpAction class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvItem : CGUpnpAvObject 
{
	NSMutableArray *resourceArray;
}
- (id)init;
#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
#else
- (id)initWithXMLNode:(CgXmlNode *)aXmlNode;
#endif
- (void)addResource:(CGUpnpAvResource *)res;
- (void)removeResource:(CGUpnpAvResource *)res;
- (NSArray *)resources;
- (CGUpnpAvResource *)smallImageResource;
- (CGUpnpAvResource *)mediumImageResource;
- (CGUpnpAvResource *)largeImageResource;
- (CGUpnpAvResource *)lowestImageResource;
- (CGUpnpAvResource *)highestImageResource;
- (CGUpnpAvResource *)applicableImageResourceBySize:(CGSize)size;
- (NSString *)thumbnailUrl;
- (NSString *)smallImageUrl;
- (NSString *)mediumImageUrl;
- (NSString *)largeImageUrl;
- (NSString *)lowestImageUrl;
- (NSString *)highestImageUrl;
- (NSString *)applicableImageUrlBySize:(CGSize)size;
- (CGUpnpAvResource *)applicableImageResourceBySize:(CGSize)wantedSize mimeTypes:(NSArray *)mimeTypes;
- (CGUpnpAvResource *)resource;
- (BOOL)hasRendererResource;
- (CGUpnpAvResource *)rendererResource;
- (NSURL *)resourceUrl;
- (CGUpnpAvResource *)movieResource;
- (CGUpnpAvResource *)videoResource;
- (CGUpnpAvResource *)audioResource;
- (CGUpnpAvResource *)imageResource;
- (BOOL)hasMovieResource;
- (BOOL)hasVideoResource;
- (BOOL)hasAudioResource;
- (BOOL)hasImageResource;
- (BOOL)isMovieClass;
- (BOOL)isVideoClass;
- (BOOL)isAudioClass;
- (BOOL)isImageClass;
- (BOOL)writeToFile:(NSString *)path;
@end
