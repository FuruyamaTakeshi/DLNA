//
//  RendererTableViewController.h
//  DLNASample
//
//  Created by 健司 古山 on 12/07/18.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

@class CGUpnpAvController;

@interface RendererTableViewController : UITableViewController
@property (nonatomic, retain)NSArray* dataSource;

- (id)initWithAvController:(CGUpnpAvController*)aController;
@end
