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

#import "UPnPTableViewCell.h"

@implementation UPnPTableViewCell

enum {
UPNPAVOBJECT_CELL_IMAGE = 1,
UPNPAVOBJECT_CELL_NAME,
UPNPAVOBJECT_CELL_MIME,
UPNPAVOBJECT_CELL_DATE,
};

#define UPNPAVOBJECT_CELL_DI_WIDTH 32
#define UPNPAVOBJECT_CELL_HEIGHT_FONT_SIZE 18
#define UPNPAVOBJECT_CELL_LOW_FONT_SIZE 12
#define UPNPAVOBJECT_CELL_LOW_HEIGHT (UPNPAVOBJECT_CELL_LOW_FONT_SIZE + 4)

- (id)initWithFrame:(CGRect)frame reuseIdentifier:(NSString *)reuseIdentifier 
{
	if (self = [super initWithFrame:frame reuseIdentifier:reuseIdentifier]) {
		CGRect cellFrameRect = [self frame];
		int cellWidth =  cellFrameRect.size.width - UPNPAVOBJECT_CELL_DI_WIDTH;
		int cellHeight =  cellFrameRect.size.height;
		int imgWidth =  32;
		int imgHeight =  imgWidth;
		int imgMargin = (cellHeight - imgHeight) / 2;
		int imgCellWidth =  cellHeight;
		//int imgCellHeight =  cellHeight;
		
		UIImageView *imageView = [[[UIImageView alloc] initWithFrame:CGRectMake(imgMargin, imgMargin, imgWidth, imgHeight)] autorelease];
		imageView.tag = UPNPAVOBJECT_CELL_IMAGE;
		[self.contentView addSubview:imageView];
		
		UILabel *nameLabel = [[[UILabel alloc] initWithFrame:
							   CGRectMake(
										  imgCellWidth, 
										  0, 
										  cellWidth - imgCellWidth, 
										  (cellHeight - UPNPAVOBJECT_CELL_LOW_HEIGHT))] autorelease];
		nameLabel.tag = UPNPAVOBJECT_CELL_NAME;
		nameLabel.font = [UIFont systemFontOfSize:UPNPAVOBJECT_CELL_HEIGHT_FONT_SIZE];
		nameLabel.textAlignment = UITextAlignmentLeft;
		nameLabel.textColor = [UIColor blackColor];
		nameLabel.autoresizingMask = UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleHeight;
		[self.contentView addSubview:nameLabel];

		UILabel *mimeLabel = [[[UILabel alloc] initWithFrame:
							   CGRectMake(
										  imgCellWidth, 
										  (cellHeight - UPNPAVOBJECT_CELL_LOW_HEIGHT), 
										  ((cellWidth - imgCellWidth)/2), 
										  UPNPAVOBJECT_CELL_LOW_HEIGHT)] autorelease];
		mimeLabel.tag = UPNPAVOBJECT_CELL_MIME;
		mimeLabel.font = [UIFont systemFontOfSize:UPNPAVOBJECT_CELL_LOW_FONT_SIZE];
		mimeLabel.textAlignment = UITextAlignmentLeft;
		mimeLabel.textColor = [UIColor darkGrayColor];
		mimeLabel.autoresizingMask = UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleHeight;
		[self.contentView addSubview:mimeLabel];

		UILabel *dateLabel = [[[UILabel alloc] initWithFrame:
							   CGRectMake(
										  (imgCellWidth + ((cellWidth - imgCellWidth)/2)), 
										  (cellHeight - UPNPAVOBJECT_CELL_LOW_HEIGHT), 
										  ((cellWidth - imgCellWidth)/2), 
										  UPNPAVOBJECT_CELL_LOW_HEIGHT)] autorelease];
		dateLabel.tag = UPNPAVOBJECT_CELL_DATE;
		dateLabel.font = [UIFont systemFontOfSize:UPNPAVOBJECT_CELL_LOW_FONT_SIZE];
		dateLabel.textAlignment = UITextAlignmentRight;
		dateLabel.textColor = [UIColor darkGrayColor];
		dateLabel.autoresizingMask = UIViewAutoresizingFlexibleRightMargin | UIViewAutoresizingFlexibleHeight;
		[self.contentView addSubview:dateLabel];
	}
	return self;
}

- (void)dealloc 
{
	[super dealloc];
}

- (UILabel *)nameLabel
{
	return (UILabel *)[self.contentView viewWithTag:UPNPAV_CELL_NAME];
}

- (UILabel *)mimeLabel
{
	return (UILabel *)[self.contentView viewWithTag:UPNPAV_CELL_MIME];
}

- (UILabel *)dateLabel
{
	return (UILabel *)[self.contentView viewWithTag:UPNPAV_CELL_DATE];
}

- (UIImageView *)imageView
{
	return (UIImageView *)[self.contentView viewWithTag:UPNPAV_CELL_IMAGE];
}

- (NSString *)name
{
	return [[[[self nameLabel] text] retain] autorelease];
}

- (NSString *)mime
{
	return [[[[self mimeLabel] text] retain] autorelease];
}

- (NSString *)date
{
	return [[[[self dateLabel] text] retain] autorelease];
}

- (void)setName:(NSString *)aName
{
	[[self nameLabel] setText:aName];
}

- (void)setMime:(NSString *)aMime
{
	[[self mimeLabel] setText:aMime];
}

- (void)setDate:(NSString *)aDate
{
	[[self dateLabel] setText:aDate];
}

- (void)setImage:(UIImage *)anImage
{
	if (!anImage)
		return;
	[[self imageView] setImage:anImage];
}

- (void)setTextColor:(UIColor *) aTextColor
{
	[[self nameLabel] setTextColor:aTextColor];
	[[self mimeLabel] setTextColor:aTextColor];
	[[self dateLabel] setTextColor:aTextColor];
}

- (void)setAlpha:(CGFloat) anAlphaValue
{
	[[self nameLabel] setAlpha:anAlphaValue];
	[[self mimeLabel] setAlpha:anAlphaValue];
	[[self dateLabel] setAlpha:anAlphaValue];
	[[self imageView] setAlpha:anAlphaValue];
}

-(void)disable
{
	//[cell setTextColor:[UIColor grayColor]];
	[self setAlpha:0.5f];
}

@end
