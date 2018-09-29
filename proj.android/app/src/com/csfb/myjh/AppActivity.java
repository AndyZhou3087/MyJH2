/****************************************************************************
Copyright (c) 2015-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package com.csfb.myjh;
import android.app.Activity;
import android.app.AlertDialog;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.KeyEvent;

import com.umeng.analytics.UMGameAnalytics;
import com.umeng.analytics.game.UMGameAgent;
import com.umeng.common.UMCocosConfigure;
import com.umeng.commonsdk.UMConfigure;

import org.cocos2dx.lib.Cocos2dxActivity;



public class AppActivity extends Cocos2dxActivity {

    private static Activity m_self = null;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.setEnableVirtualButton(false);
        super.onCreate(savedInstanceState);
        // Workaround in https://stackoverflow.com/questions/16283079/re-launchtrue-of-activity-on-home-button-but-only-the-first-time/16447508
        if (!isTaskRoot()) {
            // Android launched another instance of the root activity into an existing task
            //  so just quietly finish and go away, dropping the user back into the activity
            //  at the top of the stack (ie: the last state of this task)
            // Don't need to finish it again since it's finished in super.onCreate .
            return;
        }

        m_self = this;
        // DO OTHER INITIALIZATION BELOW
        Utils.init(this);
        initUmeng();
    }

    public static void copyToClipboard(final String content)
    {
        m_self.runOnUiThread(new Runnable()
        {
            public void run()
            {
                ClipboardManager cm = (ClipboardManager) m_self.getSystemService(Context.CLIPBOARD_SERVICE);
                // 将文本内容放到系统剪贴板里。
                ClipData myClip = ClipData.newPlainText("qqnum", content);
                cm.setPrimaryClip(myClip);
            }
        });
    }

    public static void initUmeng()
    {
        UMGameAnalytics.init(m_self);
        UMConfigure.setLogEnabled(true);
        UMCocosConfigure.init(m_self, null, Utils.getChannelID(), UMConfigure.DEVICE_TYPE_PHONE,
                null);
    }

    public boolean onKeyDown(int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_BACK) {
            JNI.saveAllData();
            AlertDialog.Builder builder = new AlertDialog.Builder(this);
            builder.setTitle("提示");
            builder.setMessage("确认退出"+ this.getString(R.string.app_name) + "吗？");
            builder.setIcon(R.mipmap.ic_launcher);
            builder.setPositiveButton("确定", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    finish();
                    System.exit(0);
                }
            });
            builder.setNegativeButton("取消", new DialogInterface.OnClickListener() {
                @Override
                public void onClick(DialogInterface dialog, int which) {
                    dialog.dismiss();
                }
            }).show();
            return false;
        }
        return super.onKeyDown(keyCode, event);
    }

    @Override
    public void onResume() {
        super.onResume();
        // 集成游戏统计分析,初始化 Session
        UMGameAgent.onResume(this);
    }

    @Override
    public void onPause() {
        super.onPause();
        // //集成游戏统计分析, 结束 Session
        UMGameAgent.onPause(this);
    }
}
