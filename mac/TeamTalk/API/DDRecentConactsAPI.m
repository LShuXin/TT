//
//  DDRecentConactsAPI.m
//  Duoduo
//
//  Created by 独嘉 on 14-4-24.
//  Copyright (c) 2014年 zuoye. All rights reserved.
//

#import "DDRecentConactsAPI.h"
#import "MTUserEntity.h"
#import "IMBuddy.pbobjc.h"
#import "MTGroupEntity.h"
#import "MTSessionEntity.h"
@implementation DDRecentConactsAPI

#pragma mark - DDAPIScheduleProtocol

- (int)requestTimeOutTimeInterval
{
    return 20;
}

- (int)requestServiceID
{
    return MODULE_ID_SESSION;
}

- (int)responseServiceID
{
    return MODULE_ID_SESSION;
}

- (int)requestCommendID
{
    return RECENT_SESSION_REQ;
}

- (int)responseCommendID
{
    return RECENT_SESSION_RES;
}

- (Analysis)analysisReturnData
{
    Analysis analysis = (id)^(NSData* data)
    {
        IMRecentContactSessionRsp *rsp =[IMRecentContactSessionRsp parseFromData:data error:nil];
        NSMutableArray *array = [NSMutableArray new];
        
        for (ContactSessionInfo *sessionInfo in rsp.contactSessionListArray) {
            NSString *originID =[NSString stringWithFormat:@"%d",sessionInfo.sessionId];
            SessionType sessionType =sessionInfo.sessionType;
            NSInteger updated_time = sessionInfo.updatedTime;
            MTSessionEntity *session =[[MTSessionEntity alloc] initWithOriginID:originID type:sessionType];
            session.lastMsg=[[NSString alloc] initWithData:sessionInfo.latestMsgData encoding:NSUTF8StringEncoding];
            [session updateUpdateTime:updated_time];
            session.lastMsgID=sessionInfo.latestMsgId;
            [array addObject:session];
        }
        return array;
    };
    return analysis;
}

- (Package)packageRequestObject
{
    Package package = (id)^(id object,uint16_t seqNo)
    {
        IMRecentContactSessionReq *req = [[IMRecentContactSessionReq alloc] init];
        [req setUserId:0];
        [req setLatestUpdateTime:(UInt32)[object[0] integerValue]];
        DataOutputStream *dataout = [[DataOutputStream alloc] init];
        [dataout writeInt:0];
        [dataout writeTcpProtocolHeader:MODULE_ID_SESSION
                                    cId:RECENT_SESSION_REQ
                                  seqNo:seqNo];
        [dataout directWriteBytes:[req data]];
        [dataout writeDataCount];
        return [dataout toByteArray];
    };
    return package;
}
@end
