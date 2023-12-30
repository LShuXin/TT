//
//  DDGetUserUnreadMessagesAPI.m
//  IOSDuoduo
//
//  Created by 独嘉 on 14-6-12.
//  Copyright (c) 2014年 dujia. All rights reserved.
//

#import "GetUnreadMessagesAPI.h"
#import "DDMessageEntity.h"
#import "Encapsulator.h"
#import "DDUserModule.h"
#import "DDMessageModule.h"
#import "RuntimeStatus.h"
#import "SessionEntity.h"
#import "IMMessage.pbobjc.h"
#import "IMBaseDefine.pbobjc.h"
#import "GroupEntity.h"


@implementation GetUnreadMessagesAPI

/**
 *  请求超时时间
 *
 *  @return 超时时间
 */
-(int)requestTimeOutTimeInterval
{
    return 20;
}

/**
 *  请求的serviceID
 *
 *  @return 对应的serviceID
 */
-(int)requestServiceID
{
    return DDSERVICE_MESSAGE;
}

/**
 *  请求返回的serviceID
 *
 *  @return 对应的serviceID
 */
-(int)responseServiceID
{
    return DDSERVICE_MESSAGE;
}

/**
 *  请求的commendID
 *
 *  @return 对应的commendID
 */
-(int)requestCommendID
{
    return CID_MSG_UNREAD_CNT_REQUEST;
}

/**
 *  请求返回的commendID
 *
 *  @return 对应的commendID
 */
-(int)responseCommendID
{
    return CID_MSG_UNREAD_CNT_RESPONSE;
}

/**
 *  解析数据的block
 *
 *  @return 解析数据的block
 */
-(Analysis)analysisReturnData
{
    Analysis analysis = (id)^(NSData* data)
    {
        IMUnreadMsgCntRsp* unreadrsp = [IMUnreadMsgCntRsp parseFromData:data error:nil];
       
        NSMutableDictionary* dic = [NSMutableDictionary new];
        NSInteger m_total_cnt = unreadrsp.totalCnt;
        [dic setObject:@(m_total_cnt) forKey:@"m_total_cnt"];
        NSMutableArray* array = [NSMutableArray new];
        
        for (UnreadInfo* unreadInfo in unreadrsp.unreadinfoListArray)
        {
            NSString* userID = @"";
            NSInteger sessionType = unreadInfo.sessionType;
            if (sessionType == SessionType_SessionTypeSingle)
            {
                userID = [DDUserEntity pbUserIdToLocalID:unreadInfo.sessionId];
            }
            else
            {
                userID = [GroupEntity pbGroupIdToLocalID:unreadInfo.sessionId];
            }
            NSInteger unread_cnt = unreadInfo.unreadCnt;
            NSInteger latest_msg_id = unreadInfo.latestMsgId;
            NSString* latest_msg_content = [[NSString alloc] initWithData:unreadInfo.latestMsgData encoding:NSUTF8StringEncoding];
            SessionEntity* session = [[SessionEntity alloc] initWithSessionID:userID type:sessionType];
            session.unReadMsgCount = unread_cnt;
            session.lastMsg = latest_msg_content;
            session.lastMsgID = latest_msg_id;
            [array addObject:session];
        }
       
        [dic setObject:array forKey:@"sessions"];
        return dic;
    };
    return analysis;
}

/**
 *  打包数据的block
 *
 *  @return 打包数据的block
 */
-(Package)packageRequestObject
{
    Package package = (id)^(id object, uint32_t seqNo)
    {
        IMUnreadMsgCntReq* unreadreq = [[IMUnreadMsgCntReq alloc] init];
        [unreadreq setUserId:0];
        DDDataOutputStream* dataout = [[DDDataOutputStream alloc] init];
        [dataout writeInt:0];
        [dataout writeTcpProtocolHeader:DDSERVICE_MESSAGE
                                    cId:CID_MSG_UNREAD_CNT_REQUEST
                                  seqNo:seqNo];
        [dataout directWriteBytes:[unreadreq data]];
        [dataout writeDataCount];
        return [dataout toByteArray];
    };
    return package;
}
@end
