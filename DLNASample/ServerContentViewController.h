//
//  ServerContentViewController.h
//  DLNASample
//
//  Created by 田中 浩一 on 12/06/27.
//  Copyright (c) 2012 エヌエスプランニング株式会社. All rights reserved.
//

#import <UIKit/UIKit.h>
@class CGUpnpAvServer;

@interface ServerContentViewController : UITableViewController
@property (nonatomic, retain)CGUpnpAvServer *server;
@property (nonatomic, retain)NSArray* dataSource;
/**
 * コンストラクタ
 */
- (id)initWithAvServer:(CGUpnpAvServer*)aServer atIndexPath:(NSIndexPath*)aIndexPath objectId:(NSString*)anObjectId;
@end
