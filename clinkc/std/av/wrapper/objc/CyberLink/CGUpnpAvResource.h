//
//  CGUpnpAvResource.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CyberLink/CGXmlNode.h>
#import <QuartzCore/QuartzCore.h>

/**
 * The CGUpnpAvResource class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAvResource : CGXmlNode 
{
}
- (id)init;
#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
#else
- (id)initWithXMLNode:(CgXmlNode *)aXmlNode;
#endif
- (NSString *)url;
- (long long)size;
- (NSString *)protocolInfo;
- (NSString *)protocolInfoAtIndex:(NSUInteger)anIndex;
- (NSString *)protocol;
- (NSString *)network;
- (NSString *)contentFormat;
- (NSString *)mimeType;
- (NSString *)extention;
- (NSString *)additionalInfo;
- (NSString *)additionalInfoForKey:(NSString*)aKey;
- (NSString *)dlnaOrgPn;
- (NSString *)dlnaOrgOp;
- (NSString *)dlnaOrgFlags;
- (CGSize)resolution;
- (BOOL)isThumbnail;
- (BOOL)isSmallImage;
- (BOOL)isMediumImage;
- (BOOL)isLargeImage;
- (BOOL)isImage;
- (BOOL)isMovie;
- (BOOL)isVideo;
- (BOOL)isAudio;
@end

