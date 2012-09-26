//
//  DLNAImageView.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Cocoa/Cocoa.h>

@class DLNAController;

@interface DLNAImageView : NSImageView
{
	DLNAController *controller;
}
@property(readwrite,retain) DLNAController *controller;
- (id)initWithFrame:(NSRect)frameRect;
- (void)mouseDown:(NSEvent *)theEvent;
@end
