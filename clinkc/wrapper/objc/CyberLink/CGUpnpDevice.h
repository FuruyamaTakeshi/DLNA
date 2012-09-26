//
//  CGUpnpDevice.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/03/14.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSString.h>
#import <Foundation/NSDictionary.h>

#if !defined(_CG_UPNP_CDEVICE_H_)
typedef void CgUpnpDevice;
#endif

@class CGUpnpDevice;
@class CGUpnpService;
@class CGUpnpAction;
@class CGUpnpIcon;

/**
 * The CGUpnpDevice class is a wrapper class for CgUpnpDevice of CyberLink for C.
 */
@protocol CGUpnpDeviceDelegate <NSObject>
@optional
- (BOOL)device:(CGUpnpDevice *)device service:(CGUpnpService *)service actionReceived:(CGUpnpAction *)action;
@end

/**
 * The CGUpnpDevice class is a wrapper class for CgUpnpDevice of CyberLink for C.
 */
@interface CGUpnpDevice : NSObject 
{
	BOOL isCObjectCreated;
}
@property(assign) CgUpnpDevice *cObject;
@property(assign) id<CGUpnpDeviceDelegate> delegate;
/**
 * Create a new UPnP device.
 * 
 * @return New instance if successfull; otherwise nil.
 */
- (id) init;
/**
 * Create a new UPnP device with the C instance.
 * 
 * @return New instance if successfull; otherwise nil.
 */
- (id) initWithCObject:(CgUpnpDevice *)cobj;
/**
 * Create a new UPnP device with the XML descriptioin.
 * 
 * @return New instance if successfull; otherwise nil.
 */
- (id) initWithXMLDescription:(NSString *)xmlDesc;
/**
 * Parse the specified XML description.
 * 
 * @return YES if successfull; otherwise nui.
 */
- (BOOL) parseXMLDescription:(NSString *)xmlDesc;
/**
 * Get the friendly name of the device.
 * 
 * @return Friendly name.
 */
- (NSString *)friendlyName;
/**
 * Get the device type of the device.
 * 
 * @return Device Type.
 */
- (NSString *)deviceType;
/**
 * Get the UDN of the device.
 * 
 * @return UDN of the specified device.
 */
- (NSString *)udn;
/**
 * Return a manufacture.
 *
 * @return Manufacture of the device.
 */
- (NSString *)manufacturer;

/**
 * Return a model number.
 *
 * @return Model number of the device.
 */
- (NSString *)modelNumber;

/**
 * Return a model name.
 *
 * @return Model name of the device.
 */
- (NSString *)modelName;

/**
 * Return a urlBase
 *
 * @return URLBase of the device.
 */
- (NSString *)urlBase;
/**
 * Return a descrition URL
 *
 * @return Descripton URL of the device.
 */
- (NSString *)descriptionURL;
/**
 * Return a location URL
 *
 * @return Location URL of the device.
 */
- (NSString *)locationURL;
/**
 * Set a device type of the device.
 * 
 * @param aType Device type to set.
 */
-(void)setDeviceType:(NSString *)aType;
/**
 * Set a friendly name of the device.
 * 
 * @param aName Friendly name to set.
 */
-(void)setFriendlyName:(NSString *)aName;
/**
 * Set a udn of the device.
 * 
 * @param aUdn UDN to set.
 */
-(void)setUdn:(NSString *)aUdn;
/**
 * Check whether the device type is the specified type.
 *
 * @param aType A type string of the device.
 * 
 * @return YES if the specified type is same as the device type; otherwise NO.
 */
- (BOOL)isDeviceType:(NSString *)aType;
/**
 * Check whether the device's UDN is the specified UDN.
 *
 * @param aUDN A UDN string of the device.
 * 
 * @return YES if the specified UDN is same as the device' UDN; otherwise NO.
 */
- (BOOL)isUDN:(NSString *)aUDN;
/**
 * Check whether the device's friendly name is the specified name.
 *
 * @param aFriendlyName A friendly name string of the device.
 * 
 * @return YES if the specified friendly name is same as the device' friendly name; otherwise NO.
 */
- (BOOL)isFriendlyName:(NSString *)aFriendlyName;
/**
 * Return a presentationURL
 *
 * @return presentationURL of the device.
 */
- (NSString *)presentationURL;
/**
 * Get all services in the device as a NSArray object. The array has the services as instances of CGUpnpService.
 *
 * @return NSArray of CGUpnpService.
 */
- (NSArray *)services;
/**
 * Get a service in the device by the specified service ID.
 *
 * @param serviceId A service ID string of the service.
 *
 * @return The CGUpnpService if the specified service is found; otherwise nil.
 */
- (CGUpnpService *)getServiceForID:(NSString *)serviceId;
/**
 * Get a service in the device by the specified service type.
 *
 * @param serviceType A service type string of the service.
 *
 * @return The CGUpnpService if the specified service is found; otherwise nil.
 */
- (CGUpnpService *)getServiceForType:(NSString *)serviceType;
/**
 * Get all icons in the device as a NSArray object. The array has the services as instances of CGUpnpIconIcon.
 *
 * @return NSArray of CGUpnpIcon.
 */
- (NSArray *)icons;
/**
 * Start the device.
 *
 * @return YES if the device is started normally, otherwise NO.
 */
- (BOOL)start;
/**
 * Stop the device.
 *
 * @return YES if the device is started normally, otherwise NO.
 */
- (BOOL)stop;
/**
 * Check if the controll point is running.
 *
 * @return YES if the device is started normally, otherwise NO.
 */
- (BOOL)isRunning;
/**
 * Announce the device.
 *
 * @return YES if the device is started normally, otherwise NO.
 */
- (BOOL)announce;
/**
 * Set a user data.
 *
 * @param aUserData A user data to set.
 *
 * @return The CGUpnpService if the specified service is found; otherwise nil.
 */
- (void)setUserData:(void *)aUserData;
/**
 * Get a stored user data.
 *
 * @return A stored user data.
 */
- (void *)userData;
/**
 * Return a IP address.
 *
 * @return IP address of the device.
 */
- (NSString *)ipaddress;
/**
 * Return a most smallest icon.
 *
 * @return Smallest Icon.
 */
- (CGUpnpIcon *)smallestIcon;
/**
 * Return a most smallest icon with a specified mime type.
 *
 * @return Smallest Icon.
 */
- (CGUpnpIcon *)smallestIconWithMimeType:(NSString *)mimeType;
/**
 * Return a absolute url of the specified icon.
 *
 * @param anIcon Icon.
 *
 * @return Absolute url of the specified icon.
 */
- (NSString *)absoluteIconUrl:(CGUpnpIcon *)anIcon;
@end
