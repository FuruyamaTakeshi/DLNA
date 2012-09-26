//
//  CGUpnpAvServer.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/07/02.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#if defined(TARGET_OS_IPHONE)
#import <UIKit/UIKit.h>
#else
#import <Foundation/NSArray.h>
#import <Foundation/NSString.h>
#endif

#import <CyberLink/UPnP.h>

@class CGUpnpAvContentDirectory;
@class CGUpnpAvObject;
@class CGUpnpAvContainer;
@class CGUpnpAvServer;

#if !defined(_CG_CLINKCAV_SERVER_H_)
typedef void CgUpnpAvServer;
#endif

@protocol CGUpnpAvServerDelegate <NSObject>
@optional
- (void)upnpAvServer:(CGUpnpAvServer *)upnpAvServer browse:(CGUpnpAction *)browseAction avObject:(CGUpnpAvObject *)avObject;
- (void)upnpAvServer:(CGUpnpAvServer *)upnpAvServer search:(CGUpnpAction *)searchAction avObject:(CGUpnpAvObject *)avObject;
@end

/**
 * The CGUpnpAvServer class is a UPnP/AV media server class.
 */
@interface CGUpnpAvServer : CGUpnpDevice
{
}
@property(readonly) CGUpnpAvContentDirectory *contentDirectory;
@property(readonly) CgUpnpAvServer *cAvObject;
@property(assign) id<CGUpnpAvServerDelegate> delegate;

#if defined(TARGET_OS_IPHONE)
@property(retain) UIImage *thumbnailImage;
#endif

/*
 * Create a new UPnP/AV server.
 * 
 * @return New instance if successfull; otherwise nil.
 */
- (id)init;
/**
 * Create a new UPnP/AV server with the C instance.
 * 
 * @return New instance if successfull; otherwise nil.
 */
- (id)initWithCObject:(CgUpnpDevice *)cobj;
- (void)setUserObject:(id)aUserObj;
- (id)userObject;
- (BOOL)browse:(NSString *)aObjectId browseFlag:(NSString *)aBrowseFlag options:(NSDictionary *)options;
- (CGUpnpAvObject *)browseMetadata:(NSString *)aObjectId;
- (NSArray *)browseDirectChildren:(NSString *)aObjectId;
- (NSArray *)browseDirectChildren:(NSString *)aObjectId requestedCount:(NSUInteger) aRequestedCount;
- (NSArray *)browseDirectChildren:(NSString *)aObjectId requestedCount:(NSUInteger)aRequestedCount startingIndex:(NSUInteger)aStartingIndex;
- (CGUpnpAction *)browseAction;
- (NSArray *)search:(NSString *)aSearchCriteria;
- (NSArray *)searchByBrowse:(NSString *)aSearchCriteria;
- (NSArray *)searchByBrowse:(NSString *)aSearchCriteria objectId:(NSString *)objectId;
- (CGUpnpAction *)searchAction;
- (CGUpnpAvContainer *)rootObject;
- (CGUpnpAvObject *)objectForId:(NSString *)aObjectId;
- (CGUpnpAvObject *)objectForTitlePath:(NSString *)aTitlePath;
- (BOOL)start;
- (BOOL)stop;
- (void)lock;
- (void)unlock;
@end
