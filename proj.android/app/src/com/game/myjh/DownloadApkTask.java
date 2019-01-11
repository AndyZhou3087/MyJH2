package com.game.myjh;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;

import android.content.Context;
import android.os.AsyncTask;

public class DownloadApkTask extends AsyncTask<Object, Long, String> {
	//private static final String TAG = "DownloadApkTask";

	public interface onDownloadApkListener {
		public void onUpdateProgress(Long... values);

		public void onDownloadFinished(String destFilePath);
	}

	private static final String FILE_TEMP_SUFFIX = ".temp";

	private Context mCtx;
	private String url;
	private String mDestFilePath;
	private onDownloadApkListener listener;

	// file size
	private long mFileSize;
	private long mDownloadedSize;


	public DownloadApkTask(onDownloadApkListener listener) {
		this.mCtx = (Context) listener;
		this.listener = listener;
		mFileSize = 0;
		mDownloadedSize = 0;
	}

	@Override
	protected String doInBackground(Object... params) {
		this.url = (String)params[0];
		this.mDestFilePath = (String)params[1];
		File destFile = null;
		InputStream is = null;
		HttpURLConnection connection = null;
		try {
			destFile = new File(mDestFilePath + FILE_TEMP_SUFFIX);
			if (destFile.exists()) {
				// if destFile exist ,delete it first
				destFile.delete();
			}
			destFile.createNewFile();

			AsyncTask<Object, Object, HttpURLConnection> task  = new HttpReqTask().execute(params[0]);

			try
			{
				connection = new HttpReqTask().create(url, null, null, null);
			}
			catch (Exception ex)
			{
				ex.printStackTrace();
			}
			if (connection == null)
			{
				return null;
			}

			mFileSize = connection.getContentLength();
			is = connection.getInputStream();

			return saveApkFile(destFile, is);
		} catch (Exception e) {
			if (destFile != null) {
				destFile.delete();
			}
			return null;
		} finally {
			if (is != null) {
				try {
					is.close();
				} catch (IOException e) {
					//
				}
			}
			if (connection != null)
			{
				connection.disconnect();
				connection = null;
			}
		}

	}

	@Override
	protected void onProgressUpdate(Long... values) {
		super.onProgressUpdate(values);
		if (listener != null) {
			listener.onUpdateProgress(values);
		}
	}

	@Override
	protected void onPostExecute(String result) {
		super.onPostExecute(result);
		if (listener != null) {
			listener.onDownloadFinished(result);
		}
	}

	private String saveApkFile(File destFile, InputStream is)
			throws FileNotFoundException, IOException {
		FileOutputStream fos = null;
		byte[] buf = new byte[4096];
		int len = 0;
		fos = new FileOutputStream(destFile);
		long lastReceive = System.currentTimeMillis();
		while ((len = is.read(buf)) != -1) {
			fos.write(buf, 0, len);
			mDownloadedSize += len;
			long now = System.currentTimeMillis();
			if (now - lastReceive > 500 || mDownloadedSize == mFileSize) {
				lastReceive = now;
				publishProgress(mFileSize, mDownloadedSize);
			}
		}
		fos.close();
		// rename
		String destFilePath = destFile.getAbsolutePath();
		int index = destFilePath.indexOf(FILE_TEMP_SUFFIX);
		String newFileName = destFilePath.substring(0, index);
		destFile.renameTo(new File(newFileName));
		return newFileName;
	}
}
