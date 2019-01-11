package com.game.myjh;

import android.app.Activity;


public class PayAction {
	static Activity context;
	public static void pay(String payCode, String goodsDesc, int price/*分为单位*/)
	{
	    JNI.sendMessage(0);
	}

	public static void init(Activity act)
	{
		context = act;
	}
}

















