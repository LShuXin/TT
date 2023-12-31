//
//  DDMsgServer.h
//  Duoduo
//
//  Created by 独嘉 on 14-4-5.
//  Copyright (c) 2014年 zuoye. All rights reserved.
//

#import <Foundation/Foundation.h>


@interface DDMsgServer : NSObject

/**
 *  连接消息服务器
 *
 *  @param userID  用户ID
 *  @param token   token
 *  @param success 连接成功执行的 block
 *  @param failure 连接失败执行的 block
 */
-(void)checkUserID:(NSString*)userID
               Pwd:(NSString*)password
             token:(NSString*)token
           success:(void(^)(id object))success
           failure:(void(^)(id object))failure;

@end
