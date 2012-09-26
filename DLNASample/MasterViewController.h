//
//  MasterViewController.h
//  DLNASample
//
//  Created by 健司 古山 on 12/05/06.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CyberLink/UPnPAV.h>
@class ServerContentViewController;
@class CGUpnpAvController;

@interface MasterViewController : UITableViewController <CGUpnpControlPointDelegate>

@property (strong, nonatomic) ServerContentViewController *detailViewController;
@property (nonatomic, retain) NSArray* dataSource;
@property (nonatomic, retain) NSArray* renderers;

@property (nonatomic, retain) CGUpnpAvController* avController;
@end
