//
//  DDMessageEntity.m
//  IOSDuoduo
//
//  Created by 独嘉 on 14-5-26.
//  Copyright (c) 2014年 dujia. All rights reserved.
//

#import "DDMessageEntity.h"
#import "DDUserModule.h"
#import "EmotionsModule.h"
#import "ChattingModule.h"
#import "Encapsulator.h"
#import "DDMessageModule.h"
#import "DDDataInputStream.h"
#import "RuntimeStatus.h"
#import "IMMessage.pbobjc.h"
#import "EmotionsModule.h"
#import "security.h"


@implementation DDMessageEntity

-(DDMessageEntity*)initWithMsgID:(NSUInteger)ID
                         msgType:(MsgType)msgType
                         msgTime:(double)msgTime
                       sessionID:(NSString*)sessionID
                        senderID:(NSString*)senderID
                      msgContent:(NSString*)msgContent
                        toUserID:(NSString*)toUserID
{
    self = [super init];
    if (self)
    {
        _msgID = ID;
        _msgType = msgType;
        _msgTime = msgTime;
        _sessionId = [sessionID copy];
        _senderId = [senderID copy];
        _msgContent = msgContent;
        _toUserID = [toUserID copy];
        _info = [[NSMutableDictionary alloc] init];
    }
    return self;
}

-(id)copyWithZone:(NSZone*)zone
{
    DDMessageEntity* ddmentity = [[[self class] allocWithZone:zone] initWithMsgID:_msgID
                                                                          msgType:_msgType
                                                                          msgTime:_msgTime
                                                                        sessionID:_sessionId
                                                                         senderID:_senderId
                                                                       msgContent:_msgContent
                                                                         toUserID:_toUserID];
    return ddmentity;
}

#pragma mark - 
#pragma mark - privateAPI
-(NSString*)getNewMessageContentFromContent:(NSString*)content
{
    NSMutableString* msgContent = [NSMutableString stringWithString: content ? content : @""];
    NSMutableString* resultContent = [NSMutableString string];
    NSRange startRange;
    // NSDictionary* emotionDic = [EmotionsModule shareInstance].emotionUnicodeDic;
    while ((startRange = [msgContent rangeOfString:@"["]).location != NSNotFound)
    {
        if (startRange.location > 0)
        {
            NSString* str = [msgContent substringWithRange:NSMakeRange(0, startRange.location)];
            DDLog(@"[前文本内容:%@", str);
            [msgContent deleteCharactersInRange:NSMakeRange(0, startRange.location)];
            startRange.location = 0;
            [resultContent appendString: str];
        }
        
        NSRange endRange = [msgContent rangeOfString:@"]"];
        if (endRange.location != NSNotFound)
        {
            NSRange range;
            range.location = 0;
            range.length = endRange.location + endRange.length;
            NSString* emotionText = [msgContent substringWithRange:range];
            [msgContent deleteCharactersInRange: NSMakeRange(0, endRange.location + endRange.length)];
            
            DDLog(@"类似表情字串:%@", emotionText);
//            NSString *emotion = emotionDic[emotionText];
//            if (emotion) {
//                // 表情
//                [resultContent appendString:emotion];
//            } else
//            {
//                [resultContent appendString:emotionText];
//            }
        }
        else
        {
            DDLog(@"没有[匹配的后缀");
            break;
        }
    }
    
    if ([msgContent length] > 0)
    {
        [resultContent appendString:msgContent];
    }
    return resultContent;
}

+(DDMessageEntity*)makeMessage:(NSString*)content
                        Module:(ChattingModule*)module
                       MsgType:(DDMessageContentType)type
{
    double msgTime = [[NSDate date] timeIntervalSince1970];
    NSString* senderID = [RuntimeStatus instance].user.objID;
    
    MsgType msgType;
    if (module.sessionEntity.sessionType == SessionType_SessionTypeSingle)
    {
        msgType = MsgType_MsgTypeSingleText;
    }
    else
    {
        msgType = MsgType_MsgTypeGroupText;
    }
    
    DDMessageEntity* message = [[DDMessageEntity alloc] initWithMsgID:[DDMessageModule getMessageID]
                                                              msgType:msgType
                                                              msgTime:msgTime
                                                            sessionID:module.sessionEntity.sessionID
                                                             senderID:senderID
                                                           msgContent:content
                                                             toUserID:module.sessionEntity.sessionID];
    message.state = DDMessageSending;
    message.msgContentType = type;
    
    [module addShowMessage:message];
    [module updateSessionUpdateTime:message.msgTime];
    return message;
}

-(BOOL)isGroupMessage
{
    if (self.msgType == MsgType_MsgTypeGroupAudio || self.msgType == MsgType_MsgTypeGroupText)
    {
        return YES;
    }
    return NO;
}

-(SessionType)getMessageSessionType
{
    return ( ![self isGroupMessage] ? SessionType_SessionTypeSingle : SessionType_SessionTypeGroup );
}

-(BOOL)isGroupVoiceMessage
{
    if (self.msgType == MsgType_MsgTypeGroupAudio) {
        return YES;
    }
    return NO;
}

-(BOOL)isVoiceMessage
{
    if (self.msgType == MsgType_MsgTypeGroupAudio || self.msgType == MsgType_MsgTypeSingleAudio)
    {
        return YES;
    }
    return NO;
}

-(BOOL)isImageMessage
{
    if (self.msgContentType == DDMessageTypeImage)
    {
        return YES;
    }
    return NO;
}

-(BOOL)isSendBySelf
{
    if ([self.senderId isEqualToString:TheRuntime.user.objID])
    {
        return YES;
    }
    return NO;
}

+(DDMessageEntity*)makeMessageFromPB:(MsgInfo*)info SessionType:(SessionType)sessionType
{
    DDMessageEntity* msg = [DDMessageEntity new];
   
    msg.msgType = info.msgType;
    
    NSMutableDictionary* msgInfo = [[NSMutableDictionary alloc] init];
    
    if ([msg isVoiceMessage])
    {
        msg.msgContentType = DDMessageTypeVoice;
        
        NSData* data = info.msgData;
        NSData* voiceData;
        
        if (data.length > 4)
        {
            voiceData = [data subdataWithRange:NSMakeRange(4, [data length] - 4)];
            NSString* filename = [NSString stringWithString:[Encapsulator defaultFileName]];
            if ([voiceData writeToFile:filename atomically:YES])
            {
                msg.msgContent = filename;
            }
            else
            {
                msg.msgContent = @"语音存储出错";
            }
            
            NSData* voiceLengthData = [data subdataWithRange:NSMakeRange(0, 4)];
            
            int8_t ch1;
            [voiceLengthData getBytes:&ch1 range:NSMakeRange(0, 1)];
            ch1 = ch1 & 0x0ff;
            
            int8_t ch2;
            [voiceLengthData getBytes:&ch2 range:NSMakeRange(1, 1)];
            ch2 = ch2 & 0x0ff;
            
            int32_t ch3;
            [voiceLengthData getBytes:&ch3 range:NSMakeRange(2, 1)];
            ch3 = ch3 & 0x0ff;
            
            int32_t ch4;
            [voiceLengthData getBytes:&ch4 range:NSMakeRange(3, 1)];
            ch4 = ch4 & 0x0ff;
            
            if ((ch1 | ch2 | ch3 | ch4) < 0)
            {
                @throw [NSException exceptionWithName:@"Exception" reason:@"EOFException" userInfo:nil];
            }
            
            int voiceLength = ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));
            [msgInfo setObject:@(voiceLength) forKey:VOICE_LENGTH];
            [msgInfo setObject:@(1) forKey:DDVOICE_PLAYED];
        }
      
      
    }
    else
    {
        NSString* tmpStr = [[NSString alloc] initWithData:info.msgData encoding:NSUTF8StringEncoding];
        
        char* pOut;
        uint32_t nOutLen;
        uint32_t nInLen = strlen([tmpStr cStringUsingEncoding:NSUTF8StringEncoding]);
        int nRet = DecryptMsg([tmpStr cStringUsingEncoding:NSUTF8StringEncoding], nInLen, &pOut, nOutLen);
        if (nRet == 0)
        {
            msg.msgContent = [NSString stringWithCString:pOut encoding:NSUTF8StringEncoding];
//            if ([msg.msgContent hasPrefix:DD_MESSAGE_IMAGE_PREFIX])
//            {
//                msg.msgContentType = DDMessageTypeImage;
//            }
            Free(pOut);
        }
        else
        {
            msg.msgContent = @"";
        }
        
    }
    
    if ([[self class] isEmotionMsg:msg.msgContent])
    {
        msg.msgContentType = DDMEssageEmotion;
    }
    msg.sessionId = [TheRuntime changeOriginalToLocalID:info.fromSessionId sessionType:sessionType];
    msg.msgID = info.msgId;
    msg.toUserID = TheRuntime.user.objID;
    msg.senderId = [TheRuntime changeOriginalToLocalID:info.fromSessionId sessionType:SessionType_SessionTypeSingle];
    msg.msgTime = info.createTime;
    msg.info = msgInfo;
    return msg;
}

+(NSData*)hexStringToData:(NSString*)string
{
    NSString *command = string;
    command = [command stringByReplacingOccurrencesOfString:@" " withString:@""];
    NSMutableData *commandToSend = [[NSMutableData alloc] init];
    unsigned char whole_byte;
    char byte_chars[3] = {'\0', '\0', '\0'};
    int i;
    for (i = 0; i < [command length] / 2; i++)
    {
        byte_chars[0] = [command characterAtIndex:i*2];
        byte_chars[1] = [command characterAtIndex:i*2+1];
        whole_byte = strtol(byte_chars, NULL, 16);
        [commandToSend appendBytes:&whole_byte length:1];
    }
    return commandToSend;
}

+(DDMessageEntity*)makeMessageFromPBData:(IMMsgData*)data
{
    DDMessageEntity* msg = [DDMessageEntity new];
    msg.msgType = data.msgType;
    SessionType type = [msg isGroupMessage] ? SessionType_SessionTypeGroup : SessionType_SessionTypeSingle;
    msg.sessionType = type;
    NSMutableDictionary* msgInfo = [[NSMutableDictionary alloc] init];
    if ([msg isVoiceMessage])
    {
        msg.msgContentType = DDMessageTypeVoice;
        NSData* tempdata = data.msgData;
        NSData* voiceData = [tempdata subdataWithRange:NSMakeRange(4, [tempdata length] - 4)];
        NSString* filename = [NSString stringWithString:[Encapsulator defaultFileName]];
        if ([voiceData writeToFile:filename atomically:YES])
        {
            msg.msgContent = filename;
        }
        else
        {
            msg.msgContent = @"语音存储出错";
        }
        NSData* voiceLengthData = [tempdata subdataWithRange:NSMakeRange(0, 4)];
        
        int8_t ch1;
        [voiceLengthData getBytes:&ch1 range:NSMakeRange(0, 1)];
        ch1 = ch1 & 0x0ff;
        
        int8_t ch2;
        [voiceLengthData getBytes:&ch2 range:NSMakeRange(1, 1)];
        ch2 = ch2 & 0x0ff;
        
        int32_t ch3;
        [voiceLengthData getBytes:&ch3 range:NSMakeRange(2, 1)];
        ch3 = ch3 & 0x0ff;
        
        int32_t ch4;
        [voiceLengthData getBytes:&ch4 range:NSMakeRange(3, 1)];
        ch4 = ch4 & 0x0ff;
        
        if ((ch1 | ch2 | ch3 | ch4) < 0)
        {
            @throw [NSException exceptionWithName:@"Exception" reason:@"EOFException" userInfo:nil];
        }
        int voiceLength = ((ch1 << 24) + (ch2 << 16) + (ch3 << 8) + (ch4 << 0));
        [msgInfo setObject:@(voiceLength) forKey:VOICE_LENGTH];
        [msgInfo setObject:@(0) forKey:DDVOICE_PLAYED];
    }
    else
    {
        
        NSString *tmpStr = [[NSString alloc] initWithData:data.msgData encoding:NSUTF8StringEncoding];

        char* pOut;
        uint32_t nOutLen;
        uint32_t nInLen = strlen([tmpStr cStringUsingEncoding:NSUTF8StringEncoding]);
        DecryptMsg([tmpStr cStringUsingEncoding:NSUTF8StringEncoding], nInLen, &pOut, nOutLen);
        msg.msgContent = [NSString stringWithCString:pOut encoding:NSUTF8StringEncoding];
//        if ([msg.msgContent hasPrefix:DD_MESSAGE_IMAGE_PREFIX])
//        {
//            msg.msgContentType = DDMessageTypeImage;
//        }
        Free(pOut);
    }
    
    if (msg.sessionType == SessionType_SessionTypeSingle)
    {
          msg.sessionId = [TheRuntime changeOriginalToLocalID:data.fromUserId sessionType:type];
    }
    else
    {
        msg.sessionId = [TheRuntime changeOriginalToLocalID:data.toSessionId sessionType:type];
    }
    
    if ([[self class] isEmotionMsg:msg.msgContent])
    {
        msg.msgContentType = DDMEssageEmotion;
    }
    
    msg.msgID = data.msgId;
    msg.toUserID = msg.sessionId;
    msg.senderId = [TheRuntime changeOriginalToLocalID:data.fromUserId sessionType:SessionType_SessionTypeSingle];
    if ([msg.senderId isEqual:TheRuntime.user.objID])
    {
        msg.sessionId = [TheRuntime changeOriginalToLocalID:data.toSessionId sessionType:type];
    }
    msg.msgTime = data.createTime;
    msg.info = msgInfo;
    return msg;
}

+(BOOL)isEmotionMsg:(NSString*)content
{
    return [[[EmotionsModule shareInstance].emotionUnicodeDic allKeys] containsObject:content];
}
@end
