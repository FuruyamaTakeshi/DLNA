//
//  CGUpnpAvServer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>

#import <CyberLink/UPnP.h>
#import <CyberLink/CGUpnpAVPositionInfo.h>

@class CGUpnpAvRenderer;
@class CGUpnpAVPositionInfo;

#if !defined(_CG_CLINKCAV_MEDIARENDERER_H_)
typedef void CgUpnpAvRenderer;
#endif

/**
 * The CGUpnpAvRenderer class is a UPnP/AV media server class.
 */
@interface CGUpnpAvRenderer : CGUpnpDevice
{
	CgUpnpAvRenderer *cAvObject;
}
@property(readonly) CgUpnpAvRenderer *cAvObject;
- (id)init;
- (id)initWithCObject:(CgUpnpDevice *)cobj;
- (BOOL)setAVTransportURI:(NSString *)aURI;
- (BOOL)play;
- (BOOL)stop;
- (BOOL)pause;
- (BOOL)next;
- (BOOL)previous;
- (BOOL)seek:(float)absTime;
- (BOOL)isPlaying;
/*
- (BOOL)start
- (BOOL)stop
*/
- (CGUpnpAVPositionInfo *)positionInfo;
@end
