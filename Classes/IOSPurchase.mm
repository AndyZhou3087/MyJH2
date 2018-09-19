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
                [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
                //NSString * pid = transaction.payment.productIdentifier;
                alertView = [[UIAlertView alloc] initWithTitle:@"提示" message:@"购买成功" delegate:nil cancelButtonTitle:NSLocalizedString(@"close", nil) otherButtonTitles:nil, nil];
                [alertView show];
                
                ShopLayer::setMessage(PAY_SUCC);
                /*
                 GlobalData::setHasBuy(false);
                 }*/
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

@end
    
    
