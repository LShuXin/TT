//
//  DDChattingMainViewController.h
//  IOSDuoduo
//
//  Created by 东邪 on 14-5-26.
//  Copyright (c) 2014年 dujia. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "JSMessageInputView.h"
#import "RecorderManager.h"
#import "PlayerManager.h"
#import "Photo.h"
#import "JSMessageTextView.h"
#import "DDMessageEntity.h"
#import "ChattingModule.h"
#import "EmotionsViewController.h"
typedef void(^TimeCellAddBlock)(bool isok);
@class ChatUtilityViewController;
@class EmotionsViewController;
@class SessionEntity;
@class RecordingView;
@interface ChattingMainViewController : UIViewController<UITextViewDelegate, JSMessageInputViewDelegate,UITableViewDataSource,UITableViewDelegate,RecordingDelegate,PlayingDelegate,UIScrollViewDelegate,UIGestureRecognizerDelegate,UIAlertViewDelegate,DDEmotionsViewControllerDelegate,UINavigationControllerDelegate>
{
    RecordingView* _recordingView;
}
@property(nonatomic,strong)ChattingModule* module;
@property(nonatomic,strong)ChatUtilityViewController *ddUtility;
@property(nonatomic,strong)JSMessageInputView *chatInputView;
@property (assign, nonatomic) CGFloat previousTextViewContentHeight;
@property(nonatomic,weak)IBOutlet UITableView *tableView;
@property(nonatomic,strong)EmotionsViewController *emotions;
@property (assign, nonatomic, readonly) UIEdgeInsets originalTableViewContentInset;
@property (nonatomic, strong) UIRefreshControl* refreshControl;
@property (assign)BOOL hadLoadHistory;
+ (instancetype )shareInstance;

-(void)sendImageMessage:(Photo *)photo Image:(UIImage *)image;
/**
 *  任意页面跳转到聊天界面并开始一个会话
 *
 *  @param session 传入一个会话实体
 */
- (void)showChattingContentForSession:(SessionEntity*)session;
-(void)insertEmojiFace:(NSString *)string;
- (void)checkSessionLastMsgIDThenUpdate;
-(void)deleteEmojiFace;
- (void)p_popViewController;
@end


@interface ChattingMainViewController(ChattingInput)
- (void)initialInput;
@end
