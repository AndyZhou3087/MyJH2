package com.game.myjh;

import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;

import android.os.AsyncTask;

public class HttpUtil
{
	private String url;
	private String type;
	private String session;
	private String pairs;
	
	HttpUtil(String url0, String type0, String session0, String pairs0)
	{
		url = url0;
		type = type0;
		session = session0;
		pairs = pairs0;
	}
	
	public HttpUtil(String url)
	{
		this(url, "GET", null, null);
	}
	
	public String getSimpleString()
	{
		String ret = null;
		
	    Object []param = new Object[4];
	    param[0] = url;
	    param[1] = type;
	    param[2] = session;
	    param[3] = pairs;
	    
	    AsyncTask<Object, Object, HttpURLConnection> task  = new HttpReqTask().execute(param);

		HttpURLConnection connection = null;
	    try 
	    {
			connection = (HttpURLConnection) task.get();
	    } 
	    catch (Exception e) 
	    {
	        return ret;
	    }
	    
	    if (connection == null)
	    	return ret;
	    
	    InputStream inStream;
	    StringBuilder sb = new StringBuilder();
	    try 
	    {
			inStream =  connection.getInputStream();
		    int len;
		    byte[] buf = new byte[4096];
			while ((len = inStream.read(buf)) != -1)
			{
				String s = new String(buf).substring(0, len);
				sb.append(s);
			}
			if (connection != null) {
				connection.disconnect();
				connection = null;
			}
			ret = sb.toString();
		} 
	    catch (IllegalStateException e) 
	    {
		} 
	    catch (IOException e) 
	    {
		}
	    
	    return ret;
	}
}