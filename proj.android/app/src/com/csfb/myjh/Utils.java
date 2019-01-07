package com.csfb.myjh;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileReader;
import java.io.InputStreamReader;
import java.io.LineNumberReader;
import java.io.Reader;
import java.io.StringWriter;
import java.io.UnsupportedEncodingException;
import java.net.Inet4Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.util.Enumeration;
import java.util.Map;
import java.util.UUID;
import android.content.Context;
import android.content.SharedPreferences;
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
import android.util.Log;
import android.util.Xml;

import org.xmlpull.v1.XmlSerializer;

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
				Build.HARDWARE.length()%10 + Build.DEVICE.length()%10 +
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
        }
		return "";
    }
    
    public static String getPkgName()
    {
        return sContext.getPackageName();
    }
    
	public static String getChannelID() {
		String channel = null;
		try {
			channel = sContext.getPackageManager().getApplicationInfo(getPkgName(), PackageManager.GET_META_DATA).metaData.getString("TH_PAYCHANNEL");
		} catch (Exception e) {
			e.printStackTrace();
		}
		if (channel == null)
			channel = "NONE";
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
        try {
            ConnectivityManager connectivity = (ConnectivityManager) sContext.getSystemService(Context.CONNECTIVITY_SERVICE);
            if (connectivity != null) {
                NetworkInfo info = connectivity.getActiveNetworkInfo();
                    if (info != null && info.isConnected()) {
                        if (info.getDetailedState() == NetworkInfo.DetailedState.CONNECTED) {
                            return true;
                        }
                    }
            }
        } catch (Exception e) {
            return false;
        }
        return false;
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

    public static String readAssetStringByLine(String filename, int line) {
        String result = "";
        try {
            //获取assets资源管理器
            AssetManager assetManager = sContext.getAssets();
            //通过管理器打开文件并读取
            //BufferedReader bf = new BufferedReader(new  InputStreamReader(assetManager.open(filename)));
            DataInputStream dataInputStream = new DataInputStream(assetManager.open(filename));
			dataInputStream.skip(line*14);
			byte namebype[] = new byte[12];
			if (dataInputStream.read(namebype, 0, 12) > 0)
			{
				String ss = new String(namebype);
				result = ss.trim();
			}
        } catch (Exception e) {
        	e.printStackTrace();
        }
        return result;
    }

    public static String getUserDefaultXmlString(int type)
	{
        final  String xmlRoot = "userDefaultRoot";
		XmlSerializer serializer = Xml.newSerializer();
        StringWriter sw = new StringWriter();
		try {
            serializer.setOutput(sw);
			serializer.startDocument("utf-8", true);
			serializer.startTag(null, xmlRoot);

			//Cocos2dxPrefsFile 是cocos自定义的userDefultXml 文件名，在org.cocos2dx.lib.Cocos2dxHelper中定义，如有修改请随改之
			SharedPreferences userDefultXml = sContext.getSharedPreferences("Cocos2dxPrefsFile", 0);
			Map<String, ?> allContent = userDefultXml.getAll();
			//遍历map的方法
			for(Map.Entry<String, ?>  entry : allContent.entrySet()){
				String key = entry.getKey();
				String vaulestr = entry.getValue().toString();
				if (vaulestr != null && vaulestr.trim().length() > 0 && key.startsWith("jh"))
				{
					boolean isok = false;
					if (type == 1&& !(key.startsWith("jhm") && getSubCount(key,"-") == 2))
						isok = true;
					else if (type == 0 && !(key.contains("guide") || key.contains("UserProtocal") || key.contains("firstenter")))
						isok = true;
					if (isok)
					{
						serializer.startTag(null, key);
						serializer.text(entry.getValue().toString());
						serializer.endTag(null, key);
					}
                }
			}
			serializer.endTag(null, xmlRoot);
			serializer.endDocument();
		} catch (Exception e) {
			e.printStackTrace();
			return "";
		}
		return sw.toString();
	}

	public static int getSubCount(String str, String key) {
		int count = 0;
		int index = 0;
		while ((index = str.indexOf(key, index)) != -1) {
			index = index + key.length();

			count++;
		}
		return count;
	}
}
