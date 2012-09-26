//
//  CGUpnpIcon.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSString.h>
#import <Foundation/NSDictionary.h>

#if !defined(_CG_UPNP_CICON_H_)
typedef void CgUpnpIcon;
#endif

/**
 * The CGUpnpIcon class is a wrapper class for CgUpnpIcon of CyberLink for C.
 */
@interface CGUpnpIcon : NSObject
{
	CgUpnpIcon *cObject;
#if defined(TARGET_OS_IPHONE)
	NSString *resourceName;
#endif
}
@property(readonly) CgUpnpIcon *cObject;
#if defined(TARGET_OS_IPHONE)
@property(readonly) NSString *resourceName;
#endif
- (id)initWithCObject:(CgUpnpIcon *)cobj;
/**
 * Get the url of the icon.
 * 
 * @return The url.
 */
- (NSString *)url;
/**
 * Get the url of the width.
 * 
 * @return The width.
 */
- (NSInteger)width;
/**
 * Get the height of the icon.
 * 
 * @return The height.
 */
- (NSInteger)height;
/**
 * Get the depth of the icon.
 * 
 * @return The depth.
 */
- (NSInteger)depth;
#if defined(TARGET_OS_IPHONE)
- (void)setResourceName:(NSString *) aResourceName;
#endif
@end

