//
//  iosfunc.h
//
//  Created by  admin on 16-11-25.
//
//


const char * getuuid();
const char * getbundleid();
const char * getvercode();
void copytoclipboard(char * p);
void alterView();
const char * getDeviceIDInKeychain();
const char* gbkToUTF8(const char * p);
bool isIphoneX();
const char* getUserDefaultXml(int type);
void openAppUri(const char * url);
void UMengInit();
