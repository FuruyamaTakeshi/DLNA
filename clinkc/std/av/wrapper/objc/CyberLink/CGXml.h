//
//  CGXml.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/29.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSString.h>

@interface CGXml : NSObject 
{
}
+ (NSString *)escapestring:(NSString *)aString;
+ (NSString *)escapestringFromIndex:(NSString *)aString index:(NSUInteger)anIndex;
+ (NSString *)unescapestring:(NSString *)aString;
@end

