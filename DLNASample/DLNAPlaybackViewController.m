//
//  DLNAPlaybackViewController.m
//  DLNASample
//
//  Created by 健司 古山 on 12/07/03.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "DLNAPlaybackViewController.h"
#import <CyberLink/UPnPAV.h>
#import "AppDelegate.h"
@interface DLNAPlaybackViewController ()

@end

@implementation DLNAPlaybackViewController
@synthesize avItem = _avItem;
@synthesize renderer = _renderer;
@synthesize isPlay = _isPlay;

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (id)initWithAvItem:(CGUpnpAvItem*)anAvItem
{
    self = [super init];
    if (self) {
        self.avItem = anAvItem;
        AppDelegate* app = [[UIApplication sharedApplication] delegate];
        self.renderer = app.avRenderer;
        self.renderer.delegate = self;
        self.isPlay = NO;
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    UILabel* titleLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 20, 200, 20)];
    titleLabel.text = (self.avItem).title;
    [self.view addSubview:titleLabel];
    [titleLabel release];
    UILabel* artistLabel = [[UILabel alloc] initWithFrame:CGRectMake(10, 40, 200, 20)];
    artistLabel.text = [(self.avItem) artist];
    [self.view addSubview:artistLabel];
    [artistLabel release];
    
    
    NSURL* imageUrl = [NSURL URLWithString:[self.avItem albumArtURI]];
    NSData* artData = [NSData dataWithContentsOfURL:imageUrl];
    UIImage* coverArt = [UIImage imageWithData:artData];
    UIImageView* coverArtView = [[UIImageView alloc] initWithImage:coverArt];
    coverArtView.frame = CGRectMake(10, 60, 300, 300);
    [self.view addSubview:coverArtView];
    [coverArtView release];
    UIButton *prevButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [prevButton addTarget:self action:@selector(prevButtonDidPush) forControlEvents:UIControlEventTouchUpInside];
    prevButton.frame = CGRectMake(self.view.center.x-85, 380, 50, 20);
    [self.view addSubview:prevButton];
    
    UIButton *playPauseButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [playPauseButton addTarget:self action:@selector(playPauseButtonDidPush) forControlEvents:UIControlEventTouchUpInside];
    playPauseButton.frame = CGRectMake(self.view.center.x-25, 380, 50, 20);
    [self.view addSubview:playPauseButton];
    
    UIButton *nextButton = [UIButton buttonWithType:UIButtonTypeRoundedRect];
    [nextButton addTarget:self action:@selector(nextButtondidPush) forControlEvents:UIControlEventTouchUpInside];
    nextButton.frame = CGRectMake(self.view.center.x+45, 380, 50, 20);
    [self.view addSubview:nextButton];
    
    if ([self.renderer setAVTransportURI:[self.avItem.resourceUrl description]]) {
        [self.renderer play];
        self.isPlay = [self.renderer isPlaying];
    } 
    
    NSLog(@"Renderer:%@", [self.renderer friendlyName]);
    
    NSLog(@"title:%@", [self.avItem title]);
    NSLog(@"upnpClass:%@", [self.avItem upnpClass]);
    NSLog(@"date:%@", [self.avItem date]);
    NSLog(@"albumArtURI:%@", [self.avItem albumArtURI]);
    NSLog(@"objectID   :%@", [self.avItem objectId]);

    NSLog(@"ancestorTitle   :%@", [self.avItem ancestor].title);
    NSLog(@"parentTitle     :%@", [self.avItem parent].title);

    NSLog(@"thumnailUri     :%@", [self.avItem thumbnailUrl]);
    NSLog(@"albumartUir     :%@", [self.avItem albumArtURI]);
    NSLog(@"smallImageUrl   :%@", [self.avItem smallImageUrl]);
    NSLog(@"mediumImageUrl  :%@", [self.avItem mediumImageUrl]);
    NSLog(@"largeImageUrl   :%@", [self.avItem largeImageUrl]);
    NSLog(@"lowestImageUrl  :%@", [self.avItem lowestImageUrl]);
    NSLog(@"highestImageUr  :%@", [self.avItem highestImageUrl]);

    NSLog(@"artist          :%@", [self.avItem artist]);

    NSLog(@"protcolInfo%@", [self.avItem resource].protocolInfo);
    
    NSLog(@"protocol:%@", [self.avItem resource].protocol);
    NSLog(@"netowrk:%@", [self.avItem resource].network);
    NSLog(@"contentFormat:%@", [self.avItem resource].contentFormat);
    NSLog(@"mimeType:%@", [self.avItem resource].mimeType);
    NSLog(@"extention:%@", [self.avItem resource].extention);
    NSLog(@"addtionalInfo:%@", [self.avItem resource].additionalInfo);
    
    NSLog(@"lowestImageResource:%@", [self.avItem lowestImageResource].url);

    NSLog(@"dlnaPn:%@", [self.avItem resource].dlnaOrgPn);
    NSLog(@"dlnaOp:%@", [self.avItem resource].dlnaOrgOp);
    NSLog(@"dlnaFlags:%@", [self.avItem resource].dlnaOrgFlags);
    
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (void)playPauseButtonDidPush 
{
    if (self.isPlay) {
        [self.renderer stop];
        
    }else {
        [self.renderer play];
    }
    self.isPlay = [self.renderer isPlaying];
}
- (void)prevButtonDidPush
{
    [self.renderer previous];
}

- (void)nextButtondidPush
{
    [self.renderer next];
}

- (BOOL)device:(CGUpnpDevice *)device service:(CGUpnpService *)service actionReceived:(CGUpnpAction *)action
{
    NSLog(@"action %@", [action description]);
    return YES;
}
@end
