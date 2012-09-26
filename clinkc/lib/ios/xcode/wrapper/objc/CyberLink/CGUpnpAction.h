//
//  CGUpnpAction.h
//  CyberLink for C
//
//  Created by Satoshi Konno on 08/05/12.
//  Copyright 2008 Satoshi Konno. All rights reserved.
//

#import <Foundation/NSString.h>
#import <Foundation/NSDictionary.h>

#if !defined(_CG_UPNP_CACTION_H_)
typedef void CgUpnpAction;
#endif

/**
 * The CGUpnpAction class is a wrapper class for CgUpnpAction of CyberLink for C.
 */
@interface CGUpnpAction : NSObject 
{
	CgUpnpAction *cObject;
}
@property(readonly) CgUpnpAction *cObject;
- (id)initWithCObject:(CgUpnpAction *)cobj;
/**
 * Get the name of the action.
 * 
 * @return The name.
 */
- (NSString *)name;
/**
 * Get all arguments in the action as a NSDictionary object.
 *
 * @return NSDictionary of the arguments.
 */
- (NSDictionary *)arguments;
/**
 * Set a value of the specified argument from the action by the argument's name directly.
 *
 * @param value The value to set
 * @param name The name of the argument to look for
 * @return YES if successfull; otherwise NO
 */
- (BOOL)setArgumentValue:(NSString *)value forName:(NSString *)name;
/**
 * Get a value of the specified argument from the action by the argument's name directly.
 *
 * @param name The name of the argument to look for
 * @return The target value if successfull; otherwise NULL
 */
- (NSString *)argumentValueForName:(NSString *)name;
/**
 * Send action
 *
 * @return YES if successfull; otherwise NO
 */
- (BOOL)post;
/**
 * Send action
 *
 * @param arguments Arguments to set
 *
 * @return YES if successfull; otherwise NO
 */
- (BOOL)postWithArguments:(NSDictionary *)arguments;
/**
 * Get a states code of the last post. 
 *
 * @return The status code
 */
- (NSInteger)statusCode;
@end
