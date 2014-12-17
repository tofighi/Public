package com.simaafzar.TextGPSLocation;

import android.app.Activity;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.telephony.SmsManager;
import android.telephony.SmsMessage;
import android.util.Log;
import android.widget.EditText;
import android.widget.Toast;

public class MainActivity extends Activity {
	private static final String TAG = "MainActivity";
	private EditText mUrl;
	private String senderPhoneNumber = null;
	GPSLocation myGPSLocation = null;
	

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		mUrl = (EditText) findViewById(R.id.name);
		Intent serviceIntent = new Intent(this, FindMyPhone.class);       
	    startService(serviceIntent);   

	}
	

}
