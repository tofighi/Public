package com.sinamedialab.easyphonefinder;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.telephony.TelephonyManager;
import android.util.Log;

public class MainActivity extends Activity {
	private static final String TAG = "MainActivity:";
	private static final String PHONE_NUMBER = "Phone Number";
	private static final String SERVICE_PROVIDER = "Service Provider Name (SPN)";

	
	GPSLocation myGPSLocation = null;
	private TelephonyManager mTelephonyMgr;
	

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		Intent serviceIntent = new Intent(this, FindMyPhone.class);       
	    startService(serviceIntent); 
	    mTelephonyMgr = (TelephonyManager)getSystemService(Context.TELEPHONY_SERVICE);
	    String phoneNumber = mTelephonyMgr.getLine1Number();
	    
	    Log.i(TAG+PHONE_NUMBER, phoneNumber);
	    
	    Log.i(TAG+SERVICE_PROVIDER,mTelephonyMgr.getSimOperatorName());
	    Log.i(TAG+"Serial Number",mTelephonyMgr.getSimSerialNumber());
	    Log.i(TAG+"SIM state",mTelephonyMgr.getSimState()+"");

	}

}
