//
//  DDReceiveMessageACKAPI.m
//  IOSDuoduo
//
//  Created by Michael Scofield on 2014-08-09.
//  Copyright (c) 2014 dujia. All rights reserved.
//

#import "DDReceiveMessageACKAPI.h"
#import "IMMessage.pbobjc.h"
@implementation DDReceiveMessageACKAPI

- (int)requestTimeOutTimeInterval
{
    return 0;
}

- (int)requestServiceID
{
    return DDSERVICE_MESSAGE;
}

- (int)responseServiceID
{
    return DDSERVICE_MESSAGE;
}

- (int)requestCommendID
{
    return DDCMD_MSG_RECEIVE_DATA_ACK;
}

- (int)responseCommendID
{
    return DDCMD_MSG_RECEIVE_DATA_ACK;
}

- (Analysis)analysisReturnData
{
    Analysis analysis = (id)^(NSData* data)
    {
      
    };
    return analysis;
}

/**
 *  打包数据的block
 *
 *  @return 打包数据的block
 */
- (Package)packageRequestObject
{
    Package package = (id)^(id object,uint32_t seqNo)
    {
        
        DDDataOutputStream *dataout = [[DDDataOutputStream alloc] init];
        IMMsgDataAck *dataAck = [[IMMsgDataAck alloc] init];
        [dataAck setUserId:0];
        [dataAck setMsgId:[object[1] intValue]];
        [dataAck setSessionId:[TheRuntime changeIDToOriginal:object[2]]];
        [dataAck setSessionType:[object[3] integerValue]];

        [dataout writeInt:0];
        [dataout writeTcpProtocolHeader:DDSERVICE_MESSAGE cId:DDCMD_MSG_RECEIVE_DATA_ACK seqNo:seqNo];
        [dataout directWriteBytes:[dataAck data]];
        [dataout writeDataCount];
        return [dataout toByteArray];
    };
    return package;
}
@end
