/******************************************************************
*
* CyberLink for Objective-C
*
* UPnPDeviceTableViewCell.m
*
* Copyright (C) Satoshi Konno 2011
*
* This is licensed under BSD-style license, see file COPYING.
*
******************************************************************/

#import "UPnPDeviceTableViewCell.h"
#import <CyberLink/UPnP.h>

@implementation UPnPDeviceTableViewCell

- (UIImage *)getIconImage:(NSString *)iconUrl
{
	//NSLog(@"getIconImage = %@" , iconUrl);
	NSURL *url = [NSURL URLWithString:iconUrl];
	if (url == nil)
		return nil;
	NSData *data = [NSData dataWithContentsOfURL:url];
	if (url == nil)
		return nil;
	UIImage *iconImage = [[UIImage alloc] initWithData:data];
	if (iconImage != nil)
		[iconImage autorelease];
	return iconImage;
}

- (void)setDevice:(CGUpnpDevice *)upnpDevice
{	
	[self setName:[upnpDevice friendlyName]];		
	[self setMime:[upnpDevice manufacturer]];		
	[self setDate:[upnpDevice ipaddress]];		

	UIImage *iconImage = nil;
	CGUpnpIcon *smallestIcon = [upnpDevice smallestIconWithMimeType:@"image/png"];
	if (smallestIcon == nil)
		smallestIcon = [upnpDevice smallestIconWithMimeType:@"image/jpeg"];
	if (smallestIcon != nil) {
		NSString *iconUrl = [upnpDevice absoluteIconUrl:smallestIcon];
		//NSLog(@"iconUrl = %@", iconUrl);
		if (0 < [iconUrl length])
			iconImage = [self getIconImage:iconUrl];
	}
	
	if (iconImage == nil)
		iconImage = [UIImage imageNamed:@"icon_device.png"];

	UIImageView *imageView = (UIImageView *)[self.contentView viewWithTag:UPNPAV_CELL_IMAGE];
	[imageView setImage:iconImage];

	[self setAccessoryType:UITableViewCellAccessoryDisclosureIndicator];
}


- (void)dealloc 
{
	[super dealloc];
}


@end
