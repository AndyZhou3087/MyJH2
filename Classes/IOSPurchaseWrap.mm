#import "IOSPurchaseWrap.h"
#import "IOSPurchase.h"

IOSPurchase * iosPurchase;
void buy(const char * productId) {
    [iosPurchase buy:[NSString stringWithUTF8String:productId]];
}

void initBuy() {
    iosPurchase = [[IOSPurchase alloc] init];
    [iosPurchase initMyBuy];
}

void restoreBuy() {
    [iosPurchase restore];
}
