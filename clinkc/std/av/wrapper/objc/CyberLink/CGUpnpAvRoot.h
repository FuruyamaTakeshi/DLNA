//
//  CGUpnpAvRoot.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/01.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <CyberLink/CGUpnpAvContainer.h>

/**
 * The CGUpnpAvRoot class is a root container of UPnP/AV.
 */
@interface CGUpnpAvRoot : CGUpnpAvContainer 
{
}
- (id)init;
#if  !defined(TARGET_OS_IPHONE)
- (id)initWithXMLNode:(NSXMLElement *)aXmlNode;
#else
- (id)initWithXMLNode:(CgXmlNode *)aXmlNode;
#endif
@end

