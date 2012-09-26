//
//  CGUpnpAVPositionInfo.h
//  
//
//  Created by Satoshi Konno on 10/12/01.
//  Copyright 2010 Satoshi Konno. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <CyberLink/UPnP.h>

@class CGUpnpAction;

@interface CGUpnpAVPositionInfo : NSObject {

}
@property(retain) CGUpnpAction *upnpAction;
-(id)initWithAction:(CGUpnpAction *)aUpnpAction;
-(float)trackDuration;
-(float)absTime;
-(float)relTime;
@end

@interface NSString(CGUPnPAV)
+(NSString *)stringWithDurationTime:(float)timeValue;
- (float)durationTime;
@end
