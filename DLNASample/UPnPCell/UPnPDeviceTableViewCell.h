/******************************************************************
*
* CyberLink for Objective-C
*
* UPnPDeviceTableViewCell.h
*
* Copyright (C) Satoshi Konno 2011
*
* This is licensed under BSD-style license, see file COPYING.
*
******************************************************************/

#import "UPnPTableViewCell.h"

@class CGUpnpDevice;

@interface UPnPDeviceTableViewCell : UPnPTableViewCell 
{
}

- (void)setDevice:(CGUpnpDevice *)upnpDevice;

@end
