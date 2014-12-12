//
//  DetailViewController.h
//  DLNASample
//
//  Created by 健司 古山 on 12/05/06.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CyberLink/CGUpnpAvServer.h>

/**
 *  @brief 詳細画面
 */
@interface DetailViewController : UIViewController <CGUpnpAvServerDelegate>

@property (strong, nonatomic) id detailItem;

@property (strong, nonatomic) IBOutlet UILabel *detailDescriptionLabel;
@property (nonatomic, retain) UILabel *ipAddressLable;
@end
