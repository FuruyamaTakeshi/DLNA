//
//  DLNAPlaybackViewController.h
//  DLNASample
//
//  Created by 健司 古山 on 12/07/03.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CyberLink/UPnP.h>
@class CGUpnpAvItem;
@class CGUpnpAvRenderer;


@interface DLNAPlaybackViewController : UIViewController <CGUpnpDeviceDelegate>
@property(nonatomic, retain)CGUpnpAvItem* avItem;
@property(nonatomic, retain)CGUpnpAvRenderer* renderer;
@property(nonatomic) BOOL isPlay;

- (id)initWithAvItem:(CGUpnpAvItem*)anAvItem;
@end
