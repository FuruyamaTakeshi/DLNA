//
//  CGUpnpAVPositionInfo.m
//  
//
//  Created by Satoshi Konno on 10/12/01.
//  Copyright 2010 Yahoo Japan Corporation. All rights reserved.
//

#import "CGUpnpAVPositionInfo.h"

@implementation CGUpnpAVPositionInfo

@synthesize upnpAction;

-(id)initWithAction:(CGUpnpAction *)aUpnpAction
{
	if ((self = [super init])) {
		[self setUpnpAction:aUpnpAction];
	}
	return self;
}

- (void) dealloc
{
	self.upnpAction = nil;
    
	[super dealloc];
}

-(float)trackDuration
{
	NSString *trackDurationStr = [[self upnpAction] argumentValueForName:@"TrackDuration"];
	return [trackDurationStr durationTime];
}

-(float)absTime
{
	NSString *absTimeStr = [[self upnpAction] argumentValueForName:@"AbsTime"];
	return [absTimeStr durationTime];
}

-(float)relTime
{
	NSString *relTimeStr = [[self upnpAction] argumentValueForName:@"RelTime"];
	return [relTimeStr durationTime];
}

@end

@implementation  NSString(CGUPnPAV)
/*
 H+:MM:SS[.F+] or H+:MM:SS[.F0/F1]
 where :
 •	H+ means one or more digits to indicate elapsed hours
 •	MM means exactly 2 digits to indicate minutes (00 to 59)
 •	SS means exactly 2 digits to indicate seconds (00 to 59)
 •	[.F+] means optionally a dot followed by one or more digits to indicate fractions of seconds
 •	[.F0/F1] means optionally a dot followed by a fraction, with F0 and F1 at least one digit long, and F0 < F1
 */
+(NSString *)stringWithDurationTime:(float)timeValue
{
	return [NSString stringWithFormat:@"%02d:%02d:%02d", 
			(int)(timeValue / 3600.0),
			(int)(fmod(timeValue, 3600.0) / 60.0),
			(int)fmod(timeValue, 60.0)];
}
- (float)durationTime
{
	NSArray *timeStrings = [self componentsSeparatedByString:@":"];
	int timeStringsCount = [timeStrings count];
	if (timeStringsCount < 3)
		return -1.0f;
	float durationTime = 0.0;
	for (int n=0; n<timeStringsCount; n++) {
		NSString *timeString = [timeStrings objectAtIndex:n];
		int timeIntValue = [timeString intValue];
		switch (n) {
			case 0: // HH
				durationTime += timeIntValue * (60 * 60);
				break;
			case 1: // MM
				durationTime += timeIntValue * 60;
				break;
			case 2: // SS
				durationTime += timeIntValue;
				break;
			case 3: // .F?
				durationTime += timeIntValue * 0.1;
				break;
			default:
				break;
		}
	}
	return durationTime;
}
@end
