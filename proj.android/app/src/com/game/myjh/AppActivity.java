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
package com.game.myjh;
import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.ProgressDialog;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.support.annotation.RequiresApi;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.FileProvider;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.Toast;

import com.game.myjh.R;
import com.umeng.analytics.UMGameAnalytics;
import com.umeng.analytics.game.UMGameAgent;
import com.umeng.common.UMCocosConfigure;
import com.umeng.commonsdk.UMConfigure;

import org.cocos2dx.lib.Cocos2dxActivity;

import java.io.File;
import java.text.DecimalFormat;


public class AppActivity extends Cocos2dxActivity implements DownloadApkTask.onDownloadApkListener {

    private static AppActivity m_self = null;
    private ProgressDialog updateProgressDialog = null;

    private static String upgardeUrl = null;

    private static String[] PERMISSIONS_STORAGE = {
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE
    };

    private static int REQUEST_PERMISSION_CODE = 1;//请求状态码

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
        //UMConfigure.setLogEnabled(true);
        UMCocosConfigure.init(m_self, null, Utils.getChannelID(), UMConfigure.DEVICE_TYPE_PHONE,
                null);
    }

    public static void upgradeApk(final String url) {

        upgardeUrl = url;
        Runnable updateApkThread = new Runnable() {
            public void run() {
                downloadApk(url);
            }
        };
        m_self.runOnUiThread(updateApkThread);
    }

    private static void downloadApk(final String url)
    {
        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.LOLLIPOP)
        {
            if (ActivityCompat.checkSelfPermission(m_self, Manifest.permission.WRITE_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {

                if (ActivityCompat.shouldShowRequestPermissionRationale(m_self, Manifest.permission
                        .WRITE_EXTERNAL_STORAGE)) {
                    Toast.makeText(m_self, "请开通相关权限，否则无法使用更新功能！", Toast.LENGTH_SHORT).show();
                }

                ActivityCompat.requestPermissions(m_self, PERMISSIONS_STORAGE, REQUEST_PERMISSION_CODE);
                return;
            }
        }

        DownloadApkTask tsk = new DownloadApkTask(m_self);
        String downloadUrl = url;
        String destFilePath = Utils.getExternalStorage();
        if (destFilePath != null)
        {
            Toast.makeText(m_self, "开始下载...", Toast.LENGTH_LONG).show();
            destFilePath = destFilePath + "/myjh.apk";
            tsk.execute(downloadUrl, destFilePath);
        }
        else
        {
            Toast.makeText(m_self, "无法下载，未检查到内存卡。", Toast.LENGTH_LONG).show();
        }

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
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == REQUEST_PERMISSION_CODE) {
            boolean isok = true;
            for (int i = 0; i < permissions.length; i++) {
                Log.i("MainActivity", "申请的权限为：" + permissions[i] + ",申请结果：" + grantResults[i]);
                if (grantResults[i] != 0) {
                    isok = false;
                    break;
                }
            }
            if (isok)
            {
                downloadApk(upgardeUrl);
            }
        }
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

    @Override
    public void onUpdateProgress(Long... values) {
        if (updateProgressDialog == null)
        {
            double s = ((double) values[0] / 1024/1024);
            DecimalFormat df = new DecimalFormat("#.00");
            updateProgressDialog = new ProgressDialog(m_self);
            updateProgressDialog.setMessage("正在下载	" + df.format(s) +"M");
            updateProgressDialog.setIndeterminate(false);
            updateProgressDialog.setProgressStyle(ProgressDialog.STYLE_HORIZONTAL);
            updateProgressDialog.setMax(100);
            updateProgressDialog.setProgress(0);
            updateProgressDialog.setCanceledOnTouchOutside(false);
            updateProgressDialog.show();
        }

        if (updateProgressDialog != null && updateProgressDialog.isShowing())
        {
            updateProgressDialog.setProgress((int) (values[1] * 100.0 / values[0]));
        }
    }

    @Override
    public void onDownloadFinished(String destFilePath) {
        // TODO Auto-generated method stub
        if (updateProgressDialog != null && updateProgressDialog.isShowing())
        {
            updateProgressDialog.dismiss();
            updateProgressDialog = null;
        }
        if (!Utils.IsExistFile(destFilePath))
        {
            return;
        }

        Intent intent = new Intent(Intent.ACTION_VIEW);
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            intent.setFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            Uri contentUri = FileProvider.getUriForFile(m_self, Utils.getPkgName() + ".provider", new File(destFilePath));
            intent.setDataAndType(contentUri, "application/vnd.android.package-archive");
        } else {
            intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            intent.setDataAndType(Uri.parse("file://" + destFilePath), "application/vnd.android.package-archive");
        }
        m_self.startActivity(intent);
    }
}
