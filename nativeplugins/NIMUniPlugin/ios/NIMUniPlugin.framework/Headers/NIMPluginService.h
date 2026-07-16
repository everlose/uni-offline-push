//
//  NIMPluginService.h
//  NIMUniPlugin
//

#import <Foundation/Foundation.h>
#import <UserNotifications/UserNotifications.h>
#import <PushKit/PushKit.h>
#import "DCUniDefine.h"

NS_ASSUME_NONNULL_BEGIN

@interface NIMPluginService : NSObject <UNUserNotificationCenterDelegate, PKPushRegistryDelegate>

@property (nonatomic, copy, nullable) NSString *deviceToken;
@property (nonatomic, copy, nullable) NSString *pushKitToken;
@property (nonatomic, copy, nullable) UniModuleKeepAliveCallback deviceTokenCallback;
@property (nonatomic, copy, nullable) UniModuleKeepAliveCallback pushNotificationCallback;
@property (nonatomic, strong, nullable) NSDictionary *launchOptions;
@property (nonatomic, strong, nullable) NSDictionary *pendingNotificationUserInfo;
@property (nonatomic, strong, nullable) NSDictionary *deviceTokenOptions;
@property (nonatomic, strong, nullable) PKPushRegistry *pushRegistry;
@property (nonatomic, assign) BOOL onceLaunchPushBack;
@property (nonatomic, assign) BOOL waitingForPushKitToken;

+ (instancetype)shared;

- (void)requestDeviceTokenWithOptions:(NSDictionary * _Nullable)options callback:(UniModuleKeepAliveCallback _Nullable)callback;
- (void)registerDeviceToken:(NSString *)token;
- (void)registerPushKitToken:(NSString *)token;
- (void)callbackCurrentDeviceToken;
- (void)handleApnsCallback:(NSDictionary *)userInfo;
- (void)flushPendingNotification;
- (void)requestNotificationAuthorization;

@end

NS_ASSUME_NONNULL_END
