//
//  LogoutAPI.m
//  TeamTalk
//
//  Created by Michael Scofield on 2014-10-20.
//  Copyright (c) 2014 dujia. All rights reserved.
//

#import "LogoutAPI.h"
#import "IMLogin.pbobjc.h"


@implementation LogoutAPI

-(int)requestTimeOutTimeInterval
{
    return 5;
}

-(int)requestServiceID
{
    return DDSERVICE_LOGIN;
}

-(int)responseServiceID
{
    return DDSERVICE_LOGIN;
}

-(int)requestCommendID
{
    return CID_LOGIN_REQ_LOGINOUT;
}

-(int)responseCommendID
{
    return CID_LOGIN_RES_LOGINOUT;
}

-(Analysis)analysisReturnData
{
    Analysis analysis = (id)^(NSData* data)
    {
        IMLogoutRsp* rsp = [IMLogoutRsp parseFromData:data error:nil];
        int isok = rsp.resultCode;
        return isok;
    };
    return analysis;
}

-(Package)packageRequestObject
{
    Package package = (id)^(id object, uint32_t seqNo)
    {
        IMLogoutReq* logoutbuilder = [[IMLogoutReq alloc] init];
        DDDataOutputStream* dataout = [[DDDataOutputStream alloc] init];
        [dataout writeInt:0];
        [dataout writeTcpProtocolHeader:DDSERVICE_LOGIN
                                    cId:CID_LOGIN_REQ_LOGINOUT
                                  seqNo:seqNo];
        [dataout directWriteBytes:[logoutbuilder data]];
        [dataout writeDataCount];
        return [dataout toByteArray];
    };
    return package;
}
@end
