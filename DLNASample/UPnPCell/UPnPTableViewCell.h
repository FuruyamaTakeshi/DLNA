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

#import <UIKit/UIKit.h>

enum {
	UPNPAV_CELL_IMAGE = 1,
	UPNPAV_CELL_NAME,
	UPNPAV_CELL_MIME,
	UPNPAV_CELL_DATE,
};

@interface UPnPTableViewCell : UITableViewCell 
{
}
- (UILabel *)nameLabel;
- (UILabel *)mimeLabel;
- (UILabel *)dateLabel;
- (UIImageView *)imageView;
- (NSString *)name;
- (NSString *)mime;
- (NSString *)date;
- (void)setName:(NSString *)aName;
- (void)setMime:(NSString *)aMime;
- (void)setDate:(NSString *)aDate;
- (void)setImage:(UIImage *)anImage;
-(void)disable;
@end
