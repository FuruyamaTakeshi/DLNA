//
//  DLNAImageView.m
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import "DLNAController.h"
#import "DLNAImageView.h"

@implementation DLNAImageView

@synthesize controller;

- (id)initWithFrame:(NSRect)frameRect
{
	if ((self = [super initWithFrame:frameRect]) == nil)
		return nil;
	return self;
}

- (void)mouseDown:(NSEvent *)theEvent
{
	DLNAController *dmc = [self controller]; 
	NSView *browserView = [dmc browserView];
	NSWindow *mainWin = [NSApp mainWindow];
	NSView *currView = [mainWin contentView];
	[mainWin setContentView:browserView];
	[currView release];
}

@end
