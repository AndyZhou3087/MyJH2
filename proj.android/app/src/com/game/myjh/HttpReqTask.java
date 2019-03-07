package com.game.myjh;
import java.io.OutputStream;
import java.net.HttpURLConnection;

import java.net.URL;
import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.X509Certificate;



import android.os.AsyncTask;
import android.util.Log;

import javax.net.ssl.HostnameVerifier;
import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSession;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

public class HttpReqTask extends AsyncTask<Object, Object, HttpURLConnection>
{
    private static final int HTTP_TIMEOUT_MS = 30000;
    private static final String HTTP_ORIGIN = "";
    private static final String TAG = "HttpReqTask";
    //重要：关于静态代码块的执行顺序问题，请关注本博文开头的相关链接。
    static {
        //忽略安全证书的问题，即https
        disableSslVerification();
    }

    public HttpURLConnection create(String url0, String type0, String session0, String pairs0) {

        HttpURLConnection connection = null;
        try {
            String url = url0;
            String method = type0;
            if (method == null)
                method = "GET";

            String session = session0;

            URL Url = new URL(url);

            connection = (HttpURLConnection) Url.openConnection();

            byte[] postData = new byte[0];
            if (method == "POST" && pairs0 != null && pairs0.length() > 0) {
                postData = pairs0.getBytes("UTF-8");
            }
            connection.setRequestMethod(method);
            connection.setUseCaches(false);
            connection.setDoInput(true);
            connection.setConnectTimeout(HTTP_TIMEOUT_MS);
            connection.setReadTimeout(HTTP_TIMEOUT_MS);
            // TODO(glaznev) - query request origin from pref_room_server_url_key preferences.
            connection.addRequestProperty("origin", HTTP_ORIGIN);
            /**
             * 为了防止CSRF的攻击，我们建议修改浏览器在发送POST请求的时候加上一个Origin字段.
             * 这个Origin字段主要是用来标识出最初请求是从哪里发起的。
             * 如果浏览器不能确定源在哪里，那么在发送的请求里面Origin字段的值就为空。
             */

            boolean doOutput = false;
            if (method.equals("POST")) {
                doOutput = true;
                connection.setDoOutput(true);
                connection.setFixedLengthStreamingMode(postData.length);
            }

            connection.setRequestProperty("Content-Type", "text/plain; charset=utf-8");


            // Send POST request.
            if (doOutput && postData.length > 0) {
                OutputStream outStream = connection.getOutputStream();
                outStream.write(postData);
                outStream.close();
            }

            // Get response.
            int responseCode = connection.getResponseCode();
            //Log.e(TAG, "responseCode:" + responseCode);

            if (responseCode != 200) {
                Log.d(TAG, "Non-200 response to " + method + " to URL: " + url + " : "
                        + connection.getHeaderField(null));
                connection.disconnect();
                return null;
            }

        } catch (Exception e) {

            e.printStackTrace();
            if (connection != null) {
                connection.disconnect();
                connection = null;
            }
        }
        return connection;
    }

    protected HttpURLConnection doInBackground(Object... params) {
        String url = (String) params[0];
        String method = "GET";
        if (params.length > 1)
            method = (String) params[1];
        String session = null;
        if (params.length > 2)
            session = (String) params[2];

        String postdata = null;
        if (params.length > 3)
            postdata = (String) params[3];

        HttpURLConnection connection = create(url, method, session, postdata);

        return connection;
    }

    private static void disableSslVerification() {
        try {
            // Create a trust manager that does not validate certificate chains
            TrustManager[] trustAllCerts = new TrustManager[]{new X509TrustManager() {
                public X509Certificate[] getAcceptedIssuers() {
                    return null;
                }

                public void checkClientTrusted(X509Certificate[] certs, String authType) {
                }

                public void checkServerTrusted(X509Certificate[] certs, String authType) {
                }
            }
            };

            // Install the all-trusting trust manager
            SSLContext sc = SSLContext.getInstance("SSL");
            sc.init(null, trustAllCerts, new java.security.SecureRandom());
            HttpsURLConnection.setDefaultSSLSocketFactory(sc.getSocketFactory());

            // Create all-trusting host name verifier
            HostnameVerifier allHostsValid = new HostnameVerifier() {
                public boolean verify(String hostname, SSLSession session) {
                    return true;
                }
            };

            // Install the all-trusting host verifier
            HttpsURLConnection.setDefaultHostnameVerifier(allHostsValid);
        } catch (NoSuchAlgorithmException e) {
            e.printStackTrace();
        } catch (KeyManagementException e) {
            e.printStackTrace();
        }
    }
    
}