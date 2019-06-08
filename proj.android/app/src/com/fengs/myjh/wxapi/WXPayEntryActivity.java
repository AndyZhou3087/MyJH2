package com.fengs.myjh.wxapi;

import java.util.Timer;
import java.util.TimerTask;

import com.game.myjh.AppActivity;
import com.game.myjh.R;
import com.game.myjh.WXPay;
import com.tencent.mm.opensdk.modelbase.BaseReq;
import com.tencent.mm.opensdk.modelbase.BaseResp;
import com.tencent.mm.opensdk.openapi.IWXAPI;
import com.tencent.mm.opensdk.openapi.IWXAPIEventHandler;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.os.Message;
import android.view.Window;
import android.view.WindowManager;

public class WXPayEntryActivity extends Activity implements IWXAPIEventHandler{
	
    private IWXAPI api;
	
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        requestWindowFeature(Window.FEATURE_NO_TITLE);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.pay_result);
        api = WXPay.wxapi;
        api.handleIntent(getIntent(), this);
    }

	@Override
	protected void onNewIntent(Intent intent) {
		super.onNewIntent(intent);
		setIntent(intent);
        api.handleIntent(intent, this);
	}

	@Override
	public void onReq(BaseReq arg0) {
		// TODO Auto-generated method stub
	}

	@Override
	public void onResp(BaseResp resp) {
		// TODO Auto-generated method stub
		if (resp.errCode == BaseResp.ErrCode.ERR_OK) 
		{
	        Message msg = AppActivity.handler.obtainMessage();
	        msg.what = 0;
	        msg.obj = WXPay.orderid;
	        msg.sendToTarget();
		}
		else if (resp.errCode == BaseResp.ErrCode.ERR_USER_CANCEL)
		{
	        Message msg = AppActivity.handler.obtainMessage();
	        msg.what = 2;
	        msg.sendToTarget();	
		}
		else
		{
	        Message msg = AppActivity.handler.obtainMessage();
	        msg.what = 1;
	        msg.sendToTarget();	
		}
		api.unregisterApp();
		
		final Timer timer = new Timer();
		TimerTask timerTask = new TimerTask() {
		 @Override
		 public void run() {
			 timer.cancel();
			 WXPayEntryActivity.this.finish();
		 }
		};
		timer.schedule(timerTask, 1000, 1000);	
	}
}