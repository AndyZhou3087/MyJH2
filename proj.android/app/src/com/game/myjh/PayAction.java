package com.game.myjh;

import android.app.Activity;


public class PayAction {
	static Activity context;
	public static void pay(String payName, int price/*分为单位*/)
	{
		WXPay.pay(payName, price);
	    //JNI.sendMessage(-1);
	}

	public static void init(Activity act)
	{
		context = act;
		WXPay.init(act);
	}
}