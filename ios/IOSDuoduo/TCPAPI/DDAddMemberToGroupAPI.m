//
//  DDAddMemberToGroupAPI.m
//  Duoduo
//
//  Created by 独嘉 on 14-5-8.
//  Copyright (c) 2014年 zuoye. All rights reserved.
//

#import "DDAddMemberToGroupAPI.h"
#import "DDGroupModule.h"
#import "GroupEntity.h"
#import "RuntimeStatus.h"
#import "IMGroup.pbobjc.h"
#import "GPBProtocolBuffers.h"


@implementation DDAddMemberToGroupAPI

/**
 *  请求超时时间
 *
 *  @return 超时时间
 */
-(int)requestTimeOutTimeInterval
{
    return TimeOutTimeInterval;
}

/**
 *  请求的serviceID
 *
 *  @return 对应的serviceID
 */
-(int)requestServiceID
{
    return SERVICE_GROUP;
}

/**
 *  请求返回的serviceID
 *
 *  @return 对应的serviceID
 */
-(int)responseServiceID
{
    return SERVICE_GROUP;
}

/**
 *  请求的commendID
 *
 *  @return 对应的commendID
 */
-(int)requestCommendID
{
    return CMD_ID_GROUP_CHANGE_GROUP_REQ;
}

/**
 *  请求返回的commendID
 *
 *  @return 对应的commendID
 */
-(int)responseCommendID
{
    return CMD_ID_GROUP_CHANGE_GROUP_RES;
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
        
        IMGroupChangeMemberRsp* rsp = [IMGroupChangeMemberRsp parseFromData:data error:nil];

        uint32_t result = rsp.resultCode;
        NSMutableArray* array = [NSMutableArray new];
        if (result != 0)
        {
            return array;
        }
    
        NSUInteger userCnt = [rsp.curUserIdListArray count];

        for (NSUInteger i = 0; i < userCnt; i++)
        {
            NSString* userId = [TheRuntime changeOriginalToLocalID:[[rsp curUserIdListArray] valueAtIndex:i]
                                                       sessionType:SessionType_SessionTypeSingle];
            [array addObject:userId];
        }
        return array;
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
    Package package = (id)^(id object, uint16_t seqNo)
    {
        NSArray* array = (NSArray*)object;
        NSString* groupId = array[0];
        NSArray* userList = array[1];
        NSMutableArray *users = [NSMutableArray new];
        for (NSString* user in userList)
        {
            [users addObject:@([TheRuntime changeIDToOriginal:user])];
        }
        IMGroupChangeMemberReq* memberChange = [[IMGroupChangeMemberReq alloc] init];
        [memberChange setUserId:0];
        [memberChange setChangeType:GroupModifyType_GroupModifyTypeAdd];
        [memberChange setGroupId:[TheRuntime changeIDToOriginal:groupId]];
        
        
        GPBUInt32Array *tempUsers = [[GPBUInt32Array alloc] init];
        for (NSNumber *number in users) {
            [tempUsers addValue:number.unsignedIntValue];
        }
        [memberChange setMemberIdListArray:tempUsers];
        DDDataOutputStream* dataout = [[DDDataOutputStream alloc] init];
        [dataout writeInt:0];
        [dataout writeTcpProtocolHeader:SERVICE_GROUP
                                    cId:CMD_ID_GROUP_CHANGE_GROUP_REQ
                                  seqNo:seqNo];
        
        [dataout directWriteBytes:[memberChange data]];
        [dataout writeDataCount];
        return [dataout toByteArray];
    };
    return package;
}
@end
