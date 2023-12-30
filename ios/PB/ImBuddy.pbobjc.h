// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: IM.Buddy.proto

// This CPP symbol can be defined to use imports that match up to the framework
// imports needed when using CocoaPods.
#if !defined(GPB_USE_PROTOBUF_FRAMEWORK_IMPORTS)
 #define GPB_USE_PROTOBUF_FRAMEWORK_IMPORTS 0
#endif

#if GPB_USE_PROTOBUF_FRAMEWORK_IMPORTS
 #import <Protobuf/GPBProtocolBuffers.h>
#else
 #import "GPBProtocolBuffers.h"
#endif

#if GOOGLE_PROTOBUF_OBJC_VERSION < 30004
#error This file was generated by a newer version of protoc which is incompatible with your Protocol Buffer library sources.
#endif
#if 30004 < GOOGLE_PROTOBUF_OBJC_MIN_SUPPORTED_VERSION
#error This file was generated by an older version of protoc which is incompatible with your Protocol Buffer library sources.
#endif

// @@protoc_insertion_point(imports)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"

CF_EXTERN_C_BEGIN

@class ContactSessionInfo;
@class DepartInfo;
@class UserInfo;
@class UserStat;
GPB_ENUM_FWD_DECLARE(SessionType);
GPB_ENUM_FWD_DECLARE(UserStatType);

NS_ASSUME_NONNULL_BEGIN

#pragma mark - ImBuddyRoot

/**
 * Exposes the extension registry for this file.
 *
 * The base class provides:
 * @code
 *   + (GPBExtensionRegistry *)extensionRegistry;
 * @endcode
 * which is a @c GPBExtensionRegistry that includes all the extensions defined by
 * this file and all files that it depends on.
 **/
GPB_FINAL @interface ImBuddyRoot : GPBRootObject
@end

#pragma mark - IMRecentContactSessionReq

typedef GPB_ENUM(IMRecentContactSessionReq_FieldNumber) {
  IMRecentContactSessionReq_FieldNumber_UserId = 1,
  IMRecentContactSessionReq_FieldNumber_LatestUpdateTime = 2,
  IMRecentContactSessionReq_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMRecentContactSessionReq : GPBMessage

/** cmd id:        0x0201 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) uint32_t latestUpdateTime;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMRecentContactSessionRsp

typedef GPB_ENUM(IMRecentContactSessionRsp_FieldNumber) {
  IMRecentContactSessionRsp_FieldNumber_UserId = 1,
  IMRecentContactSessionRsp_FieldNumber_ContactSessionListArray = 2,
  IMRecentContactSessionRsp_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMRecentContactSessionRsp : GPBMessage

/** cmd id:        0x0202 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite, strong, null_resettable) NSMutableArray<ContactSessionInfo*> *contactSessionListArray;
/** The number of items in @c contactSessionListArray without causing the array to be created. */
@property(nonatomic, readonly) NSUInteger contactSessionListArray_Count;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMUserStatNotify

typedef GPB_ENUM(IMUserStatNotify_FieldNumber) {
  IMUserStatNotify_FieldNumber_UserStat = 1,
};

GPB_FINAL @interface IMUserStatNotify : GPBMessage

/** cmd id:        0x0203 */
@property(nonatomic, readwrite, strong, null_resettable) UserStat *userStat;
/** Test to see if @c userStat has been set. */
@property(nonatomic, readwrite) BOOL hasUserStat;

@end

#pragma mark - IMUsersInfoReq

typedef GPB_ENUM(IMUsersInfoReq_FieldNumber) {
  IMUsersInfoReq_FieldNumber_UserId = 1,
  IMUsersInfoReq_FieldNumber_UserIdListArray = 2,
  IMUsersInfoReq_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMUsersInfoReq : GPBMessage

/** cmd id:        0x0204 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite, strong, null_resettable) GPBUInt32Array *userIdListArray;
/** The number of items in @c userIdListArray without causing the array to be created. */
@property(nonatomic, readonly) NSUInteger userIdListArray_Count;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMUsersInfoRsp

typedef GPB_ENUM(IMUsersInfoRsp_FieldNumber) {
  IMUsersInfoRsp_FieldNumber_UserId = 1,
  IMUsersInfoRsp_FieldNumber_UserInfoListArray = 2,
  IMUsersInfoRsp_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMUsersInfoRsp : GPBMessage

/** cmd id:        0x0205 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite, strong, null_resettable) NSMutableArray<UserInfo*> *userInfoListArray;
/** The number of items in @c userInfoListArray without causing the array to be created. */
@property(nonatomic, readonly) NSUInteger userInfoListArray_Count;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMRemoveSessionReq

typedef GPB_ENUM(IMRemoveSessionReq_FieldNumber) {
  IMRemoveSessionReq_FieldNumber_UserId = 1,
  IMRemoveSessionReq_FieldNumber_SessionType = 2,
  IMRemoveSessionReq_FieldNumber_SessionId = 3,
  IMRemoveSessionReq_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMRemoveSessionReq : GPBMessage

/** cmd id:        0x0206 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) enum SessionType sessionType;

@property(nonatomic, readwrite) uint32_t sessionId;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

/**
 * Fetches the raw value of a @c IMRemoveSessionReq's @c sessionType property, even
 * if the value was not defined by the enum at the time the code was generated.
 **/
int32_t IMRemoveSessionReq_SessionType_RawValue(IMRemoveSessionReq *message);
/**
 * Sets the raw value of an @c IMRemoveSessionReq's @c sessionType property, allowing
 * it to be set to a value that was not defined by the enum at the time the code
 * was generated.
 **/
void SetIMRemoveSessionReq_SessionType_RawValue(IMRemoveSessionReq *message, int32_t value);

#pragma mark - IMRemoveSessionRsp

typedef GPB_ENUM(IMRemoveSessionRsp_FieldNumber) {
  IMRemoveSessionRsp_FieldNumber_UserId = 1,
  IMRemoveSessionRsp_FieldNumber_ResultCode = 2,
  IMRemoveSessionRsp_FieldNumber_SessionType = 3,
  IMRemoveSessionRsp_FieldNumber_SessionId = 4,
  IMRemoveSessionRsp_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMRemoveSessionRsp : GPBMessage

/** cmd id:        0x0207 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) uint32_t resultCode;

@property(nonatomic, readwrite) enum SessionType sessionType;

@property(nonatomic, readwrite) uint32_t sessionId;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

/**
 * Fetches the raw value of a @c IMRemoveSessionRsp's @c sessionType property, even
 * if the value was not defined by the enum at the time the code was generated.
 **/
int32_t IMRemoveSessionRsp_SessionType_RawValue(IMRemoveSessionRsp *message);
/**
 * Sets the raw value of an @c IMRemoveSessionRsp's @c sessionType property, allowing
 * it to be set to a value that was not defined by the enum at the time the code
 * was generated.
 **/
void SetIMRemoveSessionRsp_SessionType_RawValue(IMRemoveSessionRsp *message, int32_t value);

#pragma mark - IMAllUserReq

typedef GPB_ENUM(IMAllUserReq_FieldNumber) {
  IMAllUserReq_FieldNumber_UserId = 1,
  IMAllUserReq_FieldNumber_LatestUpdateTime = 2,
  IMAllUserReq_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMAllUserReq : GPBMessage

/** cmd id:        0x0208 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) uint32_t latestUpdateTime;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMAllUserRsp

typedef GPB_ENUM(IMAllUserRsp_FieldNumber) {
  IMAllUserRsp_FieldNumber_UserId = 1,
  IMAllUserRsp_FieldNumber_LatestUpdateTime = 2,
  IMAllUserRsp_FieldNumber_UserListArray = 3,
  IMAllUserRsp_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMAllUserRsp : GPBMessage

/** cmd id:        0x0209 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) uint32_t latestUpdateTime;

@property(nonatomic, readwrite, strong, null_resettable) NSMutableArray<UserInfo*> *userListArray;
/** The number of items in @c userListArray without causing the array to be created. */
@property(nonatomic, readonly) NSUInteger userListArray_Count;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMUsersStatReq

typedef GPB_ENUM(IMUsersStatReq_FieldNumber) {
  IMUsersStatReq_FieldNumber_UserId = 1,
  IMUsersStatReq_FieldNumber_UserIdListArray = 2,
  IMUsersStatReq_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMUsersStatReq : GPBMessage

/** cmd id:        0x020a */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite, strong, null_resettable) GPBUInt32Array *userIdListArray;
/** The number of items in @c userIdListArray without causing the array to be created. */
@property(nonatomic, readonly) NSUInteger userIdListArray_Count;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMUsersStatRsp

typedef GPB_ENUM(IMUsersStatRsp_FieldNumber) {
  IMUsersStatRsp_FieldNumber_UserId = 1,
  IMUsersStatRsp_FieldNumber_UserStatListArray = 2,
  IMUsersStatRsp_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMUsersStatRsp : GPBMessage

/** cmd id:        0x020b */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite, strong, null_resettable) NSMutableArray<UserStat*> *userStatListArray;
/** The number of items in @c userStatListArray without causing the array to be created. */
@property(nonatomic, readonly) NSUInteger userStatListArray_Count;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMChangeAvatarReq

typedef GPB_ENUM(IMChangeAvatarReq_FieldNumber) {
  IMChangeAvatarReq_FieldNumber_UserId = 1,
  IMChangeAvatarReq_FieldNumber_AvatarURL = 2,
  IMChangeAvatarReq_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMChangeAvatarReq : GPBMessage

/** cmd id:        0x020c */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite, copy, null_resettable) NSString *avatarURL;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMChangeAvatarRsp

typedef GPB_ENUM(IMChangeAvatarRsp_FieldNumber) {
  IMChangeAvatarRsp_FieldNumber_UserId = 1,
  IMChangeAvatarRsp_FieldNumber_ResultCode = 2,
  IMChangeAvatarRsp_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMChangeAvatarRsp : GPBMessage

/** cmd id:        0x020d */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) uint32_t resultCode;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMPCLoginStatusNotify

typedef GPB_ENUM(IMPCLoginStatusNotify_FieldNumber) {
  IMPCLoginStatusNotify_FieldNumber_UserId = 1,
  IMPCLoginStatusNotify_FieldNumber_LoginStat = 2,
};

/**
 * 只给移动端通知
 **/
GPB_FINAL @interface IMPCLoginStatusNotify : GPBMessage

/** cmd id:        0x020e */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) enum UserStatType loginStat;

@end

/**
 * Fetches the raw value of a @c IMPCLoginStatusNotify's @c loginStat property, even
 * if the value was not defined by the enum at the time the code was generated.
 **/
int32_t IMPCLoginStatusNotify_LoginStat_RawValue(IMPCLoginStatusNotify *message);
/**
 * Sets the raw value of an @c IMPCLoginStatusNotify's @c loginStat property, allowing
 * it to be set to a value that was not defined by the enum at the time the code
 * was generated.
 **/
void SetIMPCLoginStatusNotify_LoginStat_RawValue(IMPCLoginStatusNotify *message, int32_t value);

#pragma mark - IMRemoveSessionNotify

typedef GPB_ENUM(IMRemoveSessionNotify_FieldNumber) {
  IMRemoveSessionNotify_FieldNumber_UserId = 1,
  IMRemoveSessionNotify_FieldNumber_SessionType = 2,
  IMRemoveSessionNotify_FieldNumber_SessionId = 3,
};

GPB_FINAL @interface IMRemoveSessionNotify : GPBMessage

/** cmd id:        0x020f */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) enum SessionType sessionType;

@property(nonatomic, readwrite) uint32_t sessionId;

@end

/**
 * Fetches the raw value of a @c IMRemoveSessionNotify's @c sessionType property, even
 * if the value was not defined by the enum at the time the code was generated.
 **/
int32_t IMRemoveSessionNotify_SessionType_RawValue(IMRemoveSessionNotify *message);
/**
 * Sets the raw value of an @c IMRemoveSessionNotify's @c sessionType property, allowing
 * it to be set to a value that was not defined by the enum at the time the code
 * was generated.
 **/
void SetIMRemoveSessionNotify_SessionType_RawValue(IMRemoveSessionNotify *message, int32_t value);

#pragma mark - IMDepartmentReq

typedef GPB_ENUM(IMDepartmentReq_FieldNumber) {
  IMDepartmentReq_FieldNumber_UserId = 1,
  IMDepartmentReq_FieldNumber_LatestUpdateTime = 2,
  IMDepartmentReq_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMDepartmentReq : GPBMessage

/** cmd id:        0x0210 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) uint32_t latestUpdateTime;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

#pragma mark - IMDepartmentRsp

typedef GPB_ENUM(IMDepartmentRsp_FieldNumber) {
  IMDepartmentRsp_FieldNumber_UserId = 1,
  IMDepartmentRsp_FieldNumber_LatestUpdateTime = 2,
  IMDepartmentRsp_FieldNumber_DeptListArray = 3,
  IMDepartmentRsp_FieldNumber_AttachData = 20,
};

GPB_FINAL @interface IMDepartmentRsp : GPBMessage

/** cmd id:        0x0211 */
@property(nonatomic, readwrite) uint32_t userId;

@property(nonatomic, readwrite) uint32_t latestUpdateTime;

@property(nonatomic, readwrite, strong, null_resettable) NSMutableArray<DepartInfo*> *deptListArray;
/** The number of items in @c deptListArray without causing the array to be created. */
@property(nonatomic, readonly) NSUInteger deptListArray_Count;

@property(nonatomic, readwrite, copy, null_resettable) NSData *attachData;
/** Test to see if @c attachData has been set. */
@property(nonatomic, readwrite) BOOL hasAttachData;

@end

NS_ASSUME_NONNULL_END

CF_EXTERN_C_END

#pragma clang diagnostic pop

// @@protoc_insertion_point(global_scope)
