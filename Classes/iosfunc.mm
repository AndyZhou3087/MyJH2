//
//  iosfunc.m
//
//  Created by  admin on 16-11-25.
//
//

#import <Foundation/Foundation.h>
#import "iosfunc.h"
//#import <UShareUI/UShareUI.h>
#import "JHKeychain.h"
#import <sys/utsname.h>

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
}

void alterView() {

//    UIViewController * s = [[[UIApplication sharedApplication] keyWindow] rootViewController];
//    UIAlertController * alert = [UIAlertController alertControllerWithTitle:@"大侠！帮个忙吧！" message:@"大侠！您一生行侠仗义，给个好评吧！" preferredStyle:UIAlertControllerStyleAlert];
//
//    alert.popoverPresentationController.barButtonItem = s.navigationItem.leftBarButtonItem;
//    UIAlertAction * ac = [UIAlertAction actionWithTitle:@"好说好说" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
//        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:@"itms-apps://itunes.apple.com/cn/app/id1215576714?mt=8&action=write-review"]];
//
//        //GlobalData::setNoComments(true);
//    }];
//
//    [alert addAction:ac];
//    [alert addAction:[UIAlertAction actionWithTitle:@"下次再说" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
//
//    }]];
//    UIAlertAction * ac3 = [UIAlertAction actionWithTitle:@"懒得理你" style:UIAlertActionStyleDefault handler:^(UIAlertAction * _Nonnull action) {
//        //GlobalData::setNoComments(true);
//    }];
//    [alert addAction:ac3];
//    if ( [[UIDevice currentDevice].systemVersion floatValue] >= 8.3)
//        [ac3 setValue:[UIColor grayColor] forKey:@"_titleTextColor"];
//    [s presentViewController:alert animated:YES completion:nil];
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

const char* getUserDefaultXml()
{
    NSString *content = @"<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    content = [content stringByAppendingFormat:@"%s", "<userDefaultRoot>"];
    
    NSDictionary * dict = [[NSUserDefaults standardUserDefaults] dictionaryRepresentation];
    for (NSString *key in dict) {
        //[defs removeObjectForKey:key];
        NSString *kvalue = [[NSUserDefaults standardUserDefaults] stringForKey:key];
        
        if (kvalue && kvalue.length > 0 && [key hasPrefix:@"jh"] && !([key hasPrefix:@"jhm"] && [key rangeOfString:@"-"].location != NSNotFound))
        {
            NSString * xmlele = [NSString stringWithFormat:@"<%@>%@</%@>",key, kvalue,key];
            content = [content stringByAppendingFormat:@"%@", xmlele];
        }
    }
    content = [content stringByAppendingFormat:@"%s", "</userDefaultRoot>"];
    return [content UTF8String];
}

bool isIphoneX()
{
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString *deviceString = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    //iPhone
    //    if ([deviceString isEqualToString:@"iPhone1,1"])    return @"iPhone 1G";
    //    if ([deviceString isEqualToString:@"iPhone1,2"])    return @"iPhone 3G";
    //    if ([deviceString isEqualToString:@"iPhone2,1"])    return @"iPhone 3GS";
    //    if ([deviceString isEqualToString:@"iPhone3,1"])    return @"iPhone 4";
    //    if ([deviceString isEqualToString:@"iPhone3,2"])    return @"Verizon iPhone 4";
    //    if ([deviceString isEqualToString:@"iPhone4,1"])    return @"iPhone 4S";
    //    if ([deviceString isEqualToString:@"iPhone5,1"])    return @"iPhone 5";
    //    if ([deviceString isEqualToString:@"iPhone5,2"])    return @"iPhone 5";
    //    if ([deviceString isEqualToString:@"iPhone5,3"])    return @"iPhone 5C";
    //    if ([deviceString isEqualToString:@"iPhone5,4"])    return @"iPhone 5C";
    //    if ([deviceString isEqualToString:@"iPhone6,1"])    return @"iPhone 5S";
    //    if ([deviceString isEqualToString:@"iPhone6,2"])    return @"iPhone 5S";
    //    if ([deviceString isEqualToString:@"iPhone7,1"])    return @"iPhone 6 Plus";
    //    if ([deviceString isEqualToString:@"iPhone7,2"])    return @"iPhone 6";
    //    if ([deviceString isEqualToString:@"iPhone8,1"])    return @"iPhone 6s";
    //    if ([deviceString isEqualToString:@"iPhone8,2"])    return @"iPhone 6s Plus";
    //    if ([deviceString isEqualToString:@"iPhone9,1"] || [deviceString isEqualToString:@"iPhone9,3"])    return @"iPhone 7";
    //    if ([deviceString isEqualToString:@"iPhone9,2"] || [deviceString isEqualToString:@"iPhone9,4"])    return @"iPhone 7 Plus";
    //    if ([deviceString isEqualToString:@"iPhone10,1"] || [deviceString isEqualToString:@"iPhone10,4"])    return @"iPhone 8";
    //    if ([deviceString isEqualToString:@"iPhone10,2"] || [deviceString isEqualToString:@"iPhone10,5"])    return @"iPhone 8 Plus";
    //    if ([deviceString isEqualToString:@"iPhone10,3"] || [deviceString isEqualToString:@"iPhone10,6"])    return @"iPhone X";
    if ([deviceString isEqualToString:@"iPhone10,3"] || [deviceString isEqualToString:@"iPhone10,6"])    return TRUE;
    return FALSE;
}

