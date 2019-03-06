package com.game.myjh;


import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import org.json.JSONObject;

import com.tencent.mm.opensdk.modelpay.PayReq;
import com.tencent.mm.opensdk.openapi.IWXAPI;
import com.tencent.mm.opensdk.openapi.WXAPIFactory;

import android.app.Activity;
import android.app.ProgressDialog;
import android.os.Message;


public class WXPay {
	static Activity context;
	static String HTTPURL = "https://www.stormnet.cn/api/"; 
	private static ProgressDialog mProgressDialog = null;
	private static Runnable progressThread;
	static public IWXAPI wxapi;
	
	public static void pay(String payName, int price)
	{ 
		 context.runOnUiThread(progressThread);
		 int ret = submitOrder(payName, price);
		 if (ret != 0)
		 {
	        Message msg = AppActivity.handler.obtainMessage();
	        msg.what = -2;
	        msg.sendToTarget();
	        dismissProgressDialog();
		 }
	}

	public static void init(Activity act)
	{
		context = act;
		progressThread = new Runnable()      
		{     
		 public void run()
		 {
			 showProgressDialog();
		 }                
		};
		
	}
	
	private static int submitOrder(String payName, int price)
	{
		
		StringBuffer strb = new StringBuffer();
		strb.append(HTTPURL);
		strb.append("wx_wxprepay?");
		strb.append("playerid=");
		strb.append(Utils.UUID());
		strb.append("&vercode=");
		strb.append(Utils.getVersion());
		strb.append("&pkg=");
		strb.append(Utils.getPkgName());
		strb.append("&pay_amount=");
		String amountstr = price + "";
		strb.append(amountstr);
		
		strb.append("&pay_type=wx");
		
		strb.append("&body=");
		strb.append(payName);
		
		strb.append("&attach=");
		strb.append(Utils.UUID() + ":" + payName);
		
		HttpUtil hp = new HttpUtil(strb.toString());
		String ret = hp.getSimpleString();
		if (ret == null)
		{
			return -2;
		}
		
        JSONObject json;
		try 
		{
			json = new JSONObject(ret);
			String appid = json.getString("appid");
			wxapi = WXAPIFactory.createWXAPI(context, appid, false);
			wxapi.registerApp(appid);
			String partnerid = json.getString("partnerid");
			String prepay_id = json.getString("prepay_id");
			String packagevalue = json.getString("package");
			String noncestr = json.getString("noncestr");
			String timestamp = json.getInt("timestamp") + "";
			String sign = json.getString("sign");
			wxpay(appid, partnerid, prepay_id, packagevalue, noncestr, timestamp, sign);
			
			final Timer timer = new Timer();
			TimerTask timerTask = new TimerTask() {
			 @Override
			 public void run() {
				 timer.cancel();
					dismissProgressDialog();
					JNI.sendMessage(-1);//微信支付时，HOME键支付界面会关闭
			 }
			};
			timer.schedule(timerTask, 600, 600);
		}
		catch(Exception e)
		{
			e.printStackTrace();
			return -1;
		}
		return 0;
	}
	
	private static String getRandomString(int length) {
		String base = "abcdefghijklmnopqrstuvwxyz0123456789";
		Random random = new Random();
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < length; i++) {
			int number = random.nextInt(base.length());
			sb.append(base.charAt(number));
		}
		return sb.toString();
	}
	
	private static void wxpay(String appid, String merchantid, String prepayid, String packagevalue, String nonceStr, String timeStamp, String sign)
	{
		PayReq request = new PayReq();

		request.appId = appid;

		request.partnerId = merchantid;

		request.prepayId = prepayid;

		request.packageValue = packagevalue;

		request.nonceStr = nonceStr;

		request.timeStamp = timeStamp;

		request.sign = sign;

		wxapi.sendReq(request);
	}
	
    public static void dismissProgressDialog() {
		if (mProgressDialog != null && mProgressDialog.isShowing()) {
			mProgressDialog.dismiss();
			
		}
	}
    public static void showProgressDialog()
    {
    	mProgressDialog = ProgressDialog.show(context, "", "处理中，请稍等...", true, false); 
    }
}

















