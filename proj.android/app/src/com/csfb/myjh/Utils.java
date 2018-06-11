package com.csfb.myjh;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.Reader;
import java.io.UnsupportedEncodingException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;
import java.util.UUID;

import android.Manifest;
import android.app.Activity;
import android.content.Context;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.net.NetworkInfo.State;
import android.net.wifi.WifiInfo;
import android.net.wifi.WifiManager;
import android.os.Build;
import android.os.Environment;
import android.telephony.TelephonyManager;
import android.util.Log;


public class Utils {

	static private Context sContext;

	public static void init(Context context) {
		sContext = context;
	}

	public static String getBaseDir() {
		String SDPath = getExternalStorage();
		String destFilePath = "";
		if (SDPath != null) {
			destFilePath = SDPath + "/" + getPkgName();
			File destDir = new File(destFilePath);
			if (!destDir.exists()) {
				destDir.mkdirs();
			}
		} else {
			destFilePath = "";
		}
		return destFilePath;

	}

	public static String getExternalStorage() {
		boolean isExit = Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED);
		if (isExit)
			return Environment.getExternalStorageDirectory().toString();
		else
			return null;

	}

	public static boolean IsExistFile(String path) {
		try {
			File file = new File(path);
			if (file.exists()) {
				return true;
			}
		} catch (Exception e) {
		}
		return false;
	}
    
	public static String GetLocalMacAddress() {
		String str = "";
		String macSerial = "";
		try {
			Process pp = Runtime.getRuntime().exec(
					"cat /sys/class/net/wlan0/address ");
			InputStreamReader ir = new InputStreamReader(pp.getInputStream());
			LineNumberReader input = new LineNumberReader(ir);

			for (; null != str;) {
				str = input.readLine();
				if (str != null) {
					macSerial = str.trim();// 去空格
					break;
				}
			}
		} catch (Exception ex) {
			ex.printStackTrace();
		}
		if (macSerial == null || "".equals(macSerial)) {
			try {
				return loadFileAsString("/sys/class/net/eth0/address").substring(0, 17);
			} catch (Exception e) {
				e.printStackTrace();
			}

		}
		if (macSerial == null)
			macSerial = "";
		
		return macSerial;
	}
	
	public static String loadFileAsString(String fileName) throws Exception {
		FileReader reader = new FileReader(fileName);
		String text = loadReaderAsString(reader);
		reader.close();
		return text;
	}

	public static String loadReaderAsString(Reader reader) throws Exception {
		StringBuilder builder = new StringBuilder();
		char[] buffer = new char[4096];
		int readLength = reader.read(buffer);
		while (readLength >= 0) {
			builder.append(buffer, 0, readLength);
			readLength = reader.read(buffer);
		}
		return builder.toString();
	}
    
    public static String UUID()
    {
        String uuid = null;
		String serial = null;
		String m_szDevIDShort = "35" +
				Build.BOARD.length()%10+ Build.BRAND.length()%10 +
				Build.CPU_ABI.length()%10 + Build.DEVICE.length()%10 +
				Build.DISPLAY.length()%10 + Build.HOST.length()%10 +
				Build.ID.length()%10 + Build.MANUFACTURER.length()%10 +
				Build.MODEL.length()%10 + Build.PRODUCT.length()%10 +
				Build.TAGS.length()%10 + Build.TYPE.length()%10 +
				Build.USER.length()%10 ; //13 位
		try {
			serial = android.os.Build.class.getField("SERIAL").get(null).toString();
			//API>=9 使用serial号
            uuid = new UUID(m_szDevIDShort.hashCode(), serial.hashCode()).toString();
		} catch (Exception exception) {
			exception.printStackTrace();
            uuid = UUID.randomUUID().toString();
		}
		return uuid;
    }
    
    /**
     * 获取版本号
     * @return 当前应用的版本号
     */
    public static String getVersion() {
        try {
            PackageManager manager = sContext.getPackageManager();
            PackageInfo info = manager.getPackageInfo(sContext.getPackageName(), 0);
            String vname = info.versionName;
            return vname;
        } catch (Exception e) {
            e.printStackTrace();
            return "";
        }
    }
    
    public static int getVersionCode() {
        try {
            PackageManager manager = sContext.getPackageManager();
            PackageInfo info = manager.getPackageInfo(sContext.getPackageName(), 0);
            int vcode = info.versionCode;
            return vcode;
        } catch (Exception e) {
            e.printStackTrace();
            return 0;
        }
    }
    
    public static String getPkgName()
    {
        return sContext.getPackageName();
    }
    
	public static String getChannelID() {
		String channel = "";
		try {
			channel = sContext.getPackageManager().getApplicationInfo(getPkgName(), PackageManager.GET_META_DATA).metaData.getString("TH_PAYCHANNEL");
		} catch (Exception e) {
			e.printStackTrace();
		}
		return channel;
	}
	
	public static String getMetaData(Context contex, String meta) {
		String data = "";
		try {
			data = contex.getPackageManager().getApplicationInfo(contex.getPackageName(), PackageManager.GET_META_DATA).metaData.getString(meta);
		} catch (Exception e) {
			e.printStackTrace();
		}
		return data;
	}
	
    public static boolean getNetworkAvailable() 
    { 
    	boolean success = false;
    	//获得网络连接服务   
    	ConnectivityManager connManager = (ConnectivityManager) sContext.getSystemService(Context.CONNECTIVITY_SERVICE);   
    	State state = connManager.getNetworkInfo(   
    	ConnectivityManager.TYPE_WIFI).getState(); // 获取网络连接状态   
    	if (State.CONNECTED == state) 
    	{ // 判断是否正在使用WIFI网络   
    		success = true;
    		return success;
    	}
    	try{
	    	state = connManager.getNetworkInfo(   
	    	ConnectivityManager.TYPE_MOBILE).getState(); // 获取网络连接状态   
	    	if (State.CONNECTED == state) 
	    	{ // 判断是否正在使用GPRS网络   
	    		success = true;   
	    	}
    	}catch (Exception e)
    	{
    		e.printStackTrace();
    	}
    	return success;
    	  
    }
    
    public static String gbkToUTF8(String str)
    {
    	String retstr = "";
		try {
			retstr = new String(str.getBytes("UTF-8"),"ISO-8859-1");
		} catch (UnsupportedEncodingException e) {
			e.printStackTrace();
		}

    	return retstr;
    }
    
    public static String getIPAddress() {
        NetworkInfo info = ((ConnectivityManager) sContext
                .getSystemService(Context.CONNECTIVITY_SERVICE)).getActiveNetworkInfo();
        if (info != null && info.isConnected()) {
            if (info.getType() == ConnectivityManager.TYPE_MOBILE) {//当前使用2G/3G/4G网络
                try {
                    //Enumeration<NetworkInterface> en=NetworkInterface.getNetworkInterfaces();
                    for (Enumeration<NetworkInterface> en = NetworkInterface.getNetworkInterfaces(); en.hasMoreElements(); ) {
                        NetworkInterface intf = en.nextElement();
                        for (Enumeration<InetAddress> enumIpAddr = intf.getInetAddresses(); enumIpAddr.hasMoreElements(); ) {
                            InetAddress inetAddress = enumIpAddr.nextElement();
                            if (!inetAddress.isLoopbackAddress() && inetAddress instanceof Inet4Address) {
                                return inetAddress.getHostAddress();
                            }
                        }
                    }
                } catch (Exception e) {
                    e.printStackTrace();
                }

            } else if (info.getType() == ConnectivityManager.TYPE_WIFI) {//当前使用无线网络
                WifiManager wifiManager = (WifiManager) sContext.getSystemService(Context.WIFI_SERVICE);
                WifiInfo wifiInfo = wifiManager.getConnectionInfo();
                String ipAddress = intIP2StringIP(wifiInfo.getIpAddress());//得到IPV4地址
                return ipAddress;
            }
        } else {
            //当前无网络连接,请在设置中打开网络
        }
        return "";
    }
    
    public static String intIP2StringIP(int ip) {
        return (ip & 0xFF) + "." +
                ((ip >> 8) & 0xFF) + "." +
                ((ip >> 16) & 0xFF) + "." +
                (ip >> 24 & 0xFF);
    }

    public static String readAssetStringByLine(String filename, int line) {
        String result = "";
        try {
            //获取assets资源管理器
            AssetManager assetManager = sContext.getAssets();
            //通过管理器打开文件并读取

            //BufferedReader bf = new BufferedReader(new  InputStreamReader(assetManager.open(filename)));
            DataInputStream dataInputStream = new DataInputStream(assetManager.open(filename));
			dataInputStream.skip(line*14);
			String ss = new String(dataInputStream.readLine().getBytes());
            result = ss;
            Log.d("", "zhou read file  "+filename+":"+result);
        } catch (Exception e) {
        	e.printStackTrace();
        }
        return result;
    }
}
