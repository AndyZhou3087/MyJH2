package com.csfb.myjh;

import android.app.Activity;
import android.os.Message;


public class PayAction {
	static Activity context;
	public static void pay(String payCode, String goodsDesc, int price/*分为单位*/)
	{
		JNI.sendMessage(-1);
	}

	public static void init(Activity act)
	{
		context = act;
	}
}

















