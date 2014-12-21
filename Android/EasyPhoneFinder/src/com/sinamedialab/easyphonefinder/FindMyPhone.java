package com.sinamedialab.easyphonefinder;


import android.annotation.SuppressLint;
import android.app.Service;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.IBinder;
import android.telephony.SmsManager;
import android.telephony.SmsMessage;
import android.util.Log;
import android.widget.Toast;

@SuppressLint("DefaultLocale") 
public class FindMyPhone extends Service{
	 
	private static String TAG = "FindMyPhone Service";
	private String senderPhoneNumber = null;
	GPSLocation myGPSLocation = null;
	
	public FindMyPhone() {
	}
	@Override
	public IBinder onBind(Intent intent) {
		// TODO Auto-generated method stub
		Log.d(TAG, "FindMyPhone Started");
		return null;
	}
	
	@Override
	public int onStartCommand(Intent intent, int flags, int startId) {   
	   // Toast.makeText(this, "The new Service was Created", Toast.LENGTH_LONG).show();
			registerReceivers();
		Log.d(TAG, "FindMyPhone Service Started");
		return Service.START_STICKY;
	    
	}
	
	private void registerReceivers() {
		registerReceiver(LocReceiver,
		        new IntentFilter("com.sinamedialab.easyphonefinder.ACTION_LOCATION"));
		registerReceiver(SMSReceiver,
		        new IntentFilter("android.provider.Telephony.SMS_RECEIVED"));
		Log.e(TAG, "SMS and Location receivers are registered");
	}
	


	@Override
    public void onCreate() {

    }

    @Override
    public void onStart(Intent intent, int startId) {
    	// For time consuming an long tasks you can launch a new thread here...
        //Toast.makeText(this, " Service Started", Toast.LENGTH_LONG).show();

    }

    @Override
    public void onDestroy() {
    	super.onDestroy();
		//Log.d(TAG, "FindMyPhone destroyed");
        //Toast.makeText(this, "Service Destroyed", Toast.LENGTH_LONG).show();

    }
 
	private BroadcastReceiver LocReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			// Stop GPS
			
			String Message = "Location:\n";
			double latitude = myGPSLocation.getLatitude();
			double longitude = myGPSLocation.getLongitude();
			Message += "Latitude: " + latitude + "\n";
			Message += "Longtitude: " + longitude + "\n";
			Message += "https://maps.google.com/maps?q=" + latitude + ","
					+ longitude;
			myGPSLocation.stopGPS();
			myGPSLocation = null;
			registerReceiver(SMSReceiver,
			        new IntentFilter("android.provider.Telephony.SMS_RECEIVED"));
			
			
			try {
				// Sending the message
				SmsManager.getDefault().sendTextMessage(senderPhoneNumber, null, Message,
						null, null);
				
				//Toast.makeText(getApplicationContext(), R.string.message_sent,Toast.LENGTH_SHORT).show();
			}
			 catch (  Exception e) {
				    Toast.makeText(getApplicationContext(),"SMS faild, please try again later!",Toast.LENGTH_LONG).show();
				    e.printStackTrace();
			}
			
		}
	};
	
	private BroadcastReceiver SMSReceiver = new BroadcastReceiver() {

		@Override
		public void onReceive(Context context, Intent intent) {
			// Receive SMS
			Log.i(TAG, "SMS received.");
			Bundle bundle = intent.getExtras();
	        Object[] messages = (Object[]) bundle.get("pdus");
	        SmsMessage[] sms = new SmsMessage[messages.length];
	        // Create messages for each incoming PDU
	        for (int n = 0; n < messages.length; n++) {
	            sms[n] = SmsMessage.createFromPdu((byte[]) messages[n]);
	        }
	        for (SmsMessage msg : sms) {
	        	
	        	if (msg.getMessageBody().toString().toLowerCase().equals("findme")) {
	        		unregisterReceiver(SMSReceiver);
	        		myGPSLocation = new GPSLocation(getApplicationContext());	
	        		senderPhoneNumber = msg.getOriginatingAddress();
	        		Log.i(TAG, msg.getMessageBody());
	        		Log.i("senderPhoneNumber", senderPhoneNumber);
	        		
	        	}
	        }
		}
	};

}
