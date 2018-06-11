//
//  iosfunc.m
//
//  Created by  admin on 16-11-25.
//
//

#import <Foundation/Foundation.h>
#import "iosfunc.h"
#include "MBProgressHUD.h"
#import <UShareUI/UShareUI.h>
#import "JHKeychain.h"

const char * getuuid()
{
    NSString * uuid = [[NSUUID UUID] UUIDString];
    return [uuid UTF8String];
}

const char * getbundleid()
{
    return [[[NSBundle mainBundle]bundleIdentifier] UTF8String];
}

const char * getvercode()
{
    return [[[NSBundle mainBundle]objectForInfoDictionaryKey:@"CFBundleShortVersionString"] UTF8String];
}

void copytoclipboard(char * p) {
    //把char*转换成OC的NSString
    NSString *nsMessage= [[NSString alloc] initWithCString:p encoding:NSUTF8StringEncoding];
    
    //获得iOS的剪切板
    UIPasteboard *pasteboard = [UIPasteboard generalPasteboard];
    
    //改变剪切板的内容
    pasteboard.string = nsMessage;
    toast("QQ号码已经复制到剪贴板！");
}

const void toast(char * info)
{
    UIViewController * s = [[[UIApplication sharedApplication] keyWindow] rootViewController];
    MBProgressHUD * HUD = [[MBProgressHUD alloc] initWithView:s.view];
    [s.view addSubview:HUD];
    HUD.label.text = [NSString stringWithUTF8String:info];
    HUD.mode = MBProgressHUDModeText;
    
    //指定距离中心点的X轴和Y轴的偏移量，如果不指定则在屏幕中间显示
    //    HUD.yOffset = 150.0f;
    //    HUD.xOffset = 100.0f;
    
    [HUD showAnimated:YES whileExecutingBlock:^{
        sleep(1.5);
    } completionBlock:^{
        [HUD removeFromSuperview];
        [HUD release];
        //HUD = NULL;
    }];
}

void alterView() {

    UIViewController * s = [[[UIApplication sharedApplication] keyWindow] rootViewController];
    UIAlertController * alert = [UIAlertController alertControllerWithTitle:@"大侠！帮个忙吧！" message:@"大侠！您一生行侠仗义，给个好评吧！" preferredStyle:UIAlertControllerStyleAlert];

    alert.popoverPresentationController.barButtonItem = s.navigationItem.leftBarButtonItem;
    UIAlertAction * ac = [UIAlertAction actionWithTitle:@"好说好说" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"itms-apps://itunes.apple.com/cn/app/id1215576714?mt=8&action=write-review"]];

        //GlobalData::setNoComments(true);
    }];

    [alert addAction:ac];
    [alert addAction:[UIAlertAction actionWithTitle:@"下次再说" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        
    }]];
    UIAlertAction * ac3 = [UIAlertAction actionWithTitle:@"懒得理你" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
        //GlobalData::setNoComments(true);
    }];
    [alert addAction:ac3];
    if ( [[UIDevice currentDevice].systemVersion floatValue] >= 8.3)
        [ac3 setValue:[UIColor grayColor] forKey:@"_titleTextColor"];
    [s presentViewController:alert animated:YES completion:nil];
}

const char * getDeviceIDInKeychain()
{
    NSString * uuid = [JHKeychain getDeviceIDInKeychain];
    return [uuid UTF8String];
}

const char* gbkToUTF8(const char * p) {
    //把char*转换成OC的NSString
    NSString *nsstr = [[NSString alloc] initWithCString:p encoding:NSUTF8StringEncoding];
    NSString *utf8str = [nsstr stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding];
    return [utf8str UTF8String];
}

