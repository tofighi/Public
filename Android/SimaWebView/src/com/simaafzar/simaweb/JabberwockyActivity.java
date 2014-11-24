package com.simaafzar.simaweb;

import android.app.Activity;
import android.content.Intent;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;

public class JabberwockyActivity extends Activity {
	 
	private WebView myWebView;
	private MediaPlayer mediaPlayer;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_jabberwocky);
		myWebView = (WebView) findViewById(R.id.webView1);
        //Webview Settings
        WebSettings settings = myWebView.getSettings();
        //settings.setBuiltInZoomControls(true);
        //If javascript is necessary
        //settings.setJavaScriptEnabled(true); 
        

        
        //Set myWebView as the web client
        myWebView.setWebViewClient(new WebViewClient(){
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                if (url != null && (url.startsWith("http://")||url.startsWith("https://"))) {
                    view.getContext().startActivity(
                        new Intent(Intent.ACTION_VIEW, Uri.parse(url)));
                    return true;
                } else {
                    return false;
                }
            }
        });
		
        //Show contents of Jabberwocky by Lewis Carroll     
        myWebView.loadUrl("file:///android_asset/webcontent/jabberwocky.html");
        
        
	}
	
	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		Log.e("SimaWeb", "onPause");
		mediaPlayer.stop();
		mediaPlayer.release(); 
		super.onPause();
	}
	
	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		Log.e("SimaWeb", "onResume");
		mediaPlayer =  MediaPlayer.create(this.getApplicationContext(), R.raw.jabberwocky);
		mediaPlayer.setLooping(true);
        mediaPlayer.start();
		super.onResume();
	}
	
	
	public void openWP(View v){
		//Set myWebView as the web client
        myWebView.setWebViewClient(new WebViewClient(){
            public boolean shouldOverrideUrlLoading(WebView view, String url) {
                if (url != null && (url.startsWith("http://")||url.startsWith("https://"))) {
                    view.getContext().startActivity(
                        new Intent(Intent.ACTION_VIEW, Uri.parse(url)));
                    return true;
                } else {
                    return false;
                }
            }
        });
	    String url = "http://en.wikipedia.org/wiki/Jabberwocky";
	    Intent i = new Intent(Intent.ACTION_VIEW);
	    i.setData(Uri.parse(url));
	    startActivity(i); 
		
	}
	
	public void openPic(View v){
		//Set myWebView as the web client
        myWebView.loadUrl("file:///android_asset/webcontent/jabberwocky_pic.html");
       
	}


}
