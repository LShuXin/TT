//
//  FinderViewController.h
//  TeamTalk
//
//  Created by 独嘉 on 14-10-22.
//  Copyright (c) 2014年 dujia. All rights reserved.
//

#import <UIKit/UIKit.h>


@interface FinderViewController : UIViewController<UITableViewDataSource, UITableViewDelegate, NSURLConnectionDelegate>

@property(nonatomic, weak) IBOutlet UITableView* tableView;

@end
