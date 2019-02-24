//
//  IOSPurchase.m
//  kuxx-hllm_v1.1.1
//
//  Created by Hailin Rao on 2016/12/16.
//
//

#import <Foundation/Foundation.h>
#import "IOSPurchase.h"
#import "ShopLayer.h"
#import "GlobalInstance.h"

@interface IOSPurchase ()

@end

@implementation IOSPurchase

- (void) initMyBuy {
    _productId = @"";
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
}

- (void) dealloc {
    // 移除监听
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    [super dealloc];
}

- (void) restore {
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

- (void) buy:(NSString *) productId {
    _productId = productId;

    if([SKPaymentQueue canMakePayments]){
        
        NSSet *nsset = [NSSet setWithArray:@[productId]];
        // 请求商品
        SKProductsRequest *request = [[SKProductsRequest alloc] initWithProductIdentifiers:nsset];
        request.delegate = self;
        [request start];
        
    }else{
        [[[UIAlertView alloc]initWithTitle:nil message:NSLocalizedString(@"用户禁止应用内付费购买", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"确定", nil) otherButtonTitles:nil, nil] show];
        ShopLayer::setMessage(PAY_FAIL);
    }
}

//收到产品返回信息
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response{
    if(response.products.count == 0){ // 无法获取产品信息，购买失败
        [[[UIAlertView alloc]initWithTitle:nil message:NSLocalizedString(@"无法获取产品信息，请重试", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"确定", nil) otherButtonTitles:nil, nil] show];
        ShopLayer::setMessage(PAY_FAIL);
        return;
    }
    //    NSArray * myProduct = response.products;
    //    for (SKProduct * product in myProduct) {
    //        //NSLog(@"product info");
    //        //NSLog(@"des=%@", [product description]);
    //        //NSLog(@"title=%@", product.localizedTitle);
    //        //NSLog(@"ldes=%@", product.localizedDescription);
    //        //NSLog(@"price=%@", product.price);
    //        //NSLog(@"id=%@", product.productIdentifier);
    //        if ([product.productIdentifier isEqualToString:_productId]) {
    //            SKPayment * payment = [SKPayment paymentWithProduct:product];
    //            [[SKPaymentQueue defaultQueue] addPayment:payment];
    //            break;
    //        }
    //    }
    SKMutablePayment *payment = [SKMutablePayment paymentWithProduct:response.products[0]];
    
    [[SKPaymentQueue defaultQueue] addPayment:payment];
}

//请求失败
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error {
    
    //[self.navigationController.view hideToastActivity];
    //self.navigationController.view.userInteractionEnabled = YES;
    
    [[[UIAlertView alloc]initWithTitle:nil message:NSLocalizedString(@"无法获取产品信息，请重试", nil) delegate:nil cancelButtonTitle:NSLocalizedString(@"确定", nil) otherButtonTitles:nil, nil] show];
    ShopLayer::setMessage(PAY_FAIL);
    
}

- (void)requestDidFinish:(SKRequest *)request {
    //    [self.navigationController.view hideToastActivity];
    //    self.navigationController.view.userInteractionEnabled = YES;
}


- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray<SKPaymentTransaction *> *)transactions NS_AVAILABLE_IOS(3_0) {
    UIAlertView * alertView;
    for (SKPaymentTransaction * transaction in transactions) {
        switch (transaction.transactionState) {
            case SKPaymentTransactionStatePurchasing:
                break;
            case SKPaymentTransactionStatePurchased:
            {
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                //NSString * pid = transaction.payment.productIdentifier;
                if (GlobalInstance::isServerReceipt)
                {
                    [self postServerReceipt:0];
                }
                else{
                    alertView = [[UIAlertView alloc] initWithTitle:@"提示" message:@"购买成功" delegate:nil cancelButtonTitle:NSLocalizedString(@"close", nil) otherButtonTitles:nil, nil];
                    [alertView show];
                    
                    ShopLayer::setMessage(PAY_SUCC);
                }
            }
                break;
            case SKPaymentTransactionStateFailed:
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                alertView = [[UIAlertView alloc] initWithTitle:@"提示" message:@"购买失败" delegate:nil cancelButtonTitle:NSLocalizedString(@"close", nil) otherButtonTitles:nil, nil];
                [alertView show];
                /*
                 GlobalData::setHasBuy(false);
                 }*/
                ShopLayer::setMessage(PAY_FAIL);
                break;
            case SKPaymentTransactionStateRestored:
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                alertView = [[UIAlertView alloc] initWithTitle:@"提示" message:@"恢复成功" delegate:nil cancelButtonTitle:NSLocalizedString(@"close", nil) otherButtonTitles:nil, nil];
                [alertView show];
                /*
                 GlobalData::setHasBuy(false);
                 }*/
                break;
            case SKPaymentTransactionStateDeferred:
                break;
        }
        
    }
}

- (void) postServerReceipt: (int) issand{
    
    NSURL *receiptURL = [[NSBundle mainBundle] appStoreReceiptURL];
    NSData *receipt = [NSData dataWithContentsOfURL:receiptURL];
    if (receipt) {
        /**
         服务器要做的事情:
         接收ios端发过来的购买凭证。
         判断凭证是否已经存在或验证过，然后存储该凭证。
         将该凭证发送到苹果的服务器验证，并将验证结果返回给客户端。
         如果需要，修改用户相应的会员权限
         */
        NSString* receiptstr = [receipt base64EncodedStringWithOptions:0];
        //1.创建url
        NSString *urlStr = [NSString stringWithFormat:@"https://www.stormnet.cn/jhapi/jh_iospaycheck?is_sandbox=%d",issand];
        
        //注意:如果网址中含中文是无法解析的,需要stringByAddingPercentEncodingWithAllowedCharacters:进行编码,另外,解码用stringByRemovingPercentEncoding
        urlStr =[urlStr stringByAddingPercentEncodingWithAllowedCharacters:[NSCharacterSet URLQueryAllowedCharacterSet]];
        
        //2.创建请求
        NSMutableURLRequest *request = [NSMutableURLRequest requestWithURL:[NSURL URLWithString:urlStr]];
        //设置请求方式
        [request setHTTPMethod:@"POST"];
        
        NSString *args = [NSString stringWithFormat:@"{\"receipt-data\":\"%@\"}",receiptstr];

        request.HTTPBody = [args dataUsingEncoding:NSUTF8StringEncoding];
        //3.创建会话（这里使用了一个全局会话）并且启动任务
        NSURLSession *session=[NSURLSession sharedSession];
        
        //从会话创建任务
        NSURLSessionDataTask *dataTask=[session dataTaskWithRequest:request completionHandler:^(NSData *data, NSURLResponse *response, NSError *error) {
            bool isOK = false;
            if (data && (error == nil)) {
                //NSString *dataStr=[[NSString alloc]initWithData:data encoding:NSUTF8StringEncoding];
                NSDictionary *dic = [NSJSONSerialization JSONObjectWithData:data options:0 error:nil];
                
                if (dic != nil)
                {
                    int code = [[dic objectForKey:@"status"] intValue];
                    if(code == 0)
                    {
                        isOK = true;
                        ShopLayer::setMessage(PAY_SUCC);
                        dispatch_sync(dispatch_get_main_queue(), ^{
                            //Update UI in UI thread here
                            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"提示" message:@"购买成功" delegate:nil cancelButtonTitle:NSLocalizedString(@"close", nil) otherButtonTitles:nil, nil];
                            [alertView show];
                            
                        });
                    }
                    else if (code == 21007){
                        isOK = true;
                        [self postServerReceipt:1];
                    }
                }
                
            }
            if (!isOK)
            {
                ShopLayer::setMessage(PAY_FAIL);
                dispatch_sync(dispatch_get_main_queue(), ^{
                    //Update UI in UI thread here
                    UIAlertView * alertView = [[UIAlertView alloc] initWithTitle:@"提示" message:@"购买异常，如有问题请联系客服" delegate:nil cancelButtonTitle:NSLocalizedString(@"close", nil) otherButtonTitles:nil, nil];
                    [alertView show];
                });

            }
        }];
        
        [dataTask resume];//恢复线程，启动任务,开始请求
    }
}

@end
    
    
