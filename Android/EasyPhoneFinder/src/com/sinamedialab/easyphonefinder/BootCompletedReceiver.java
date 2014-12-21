package com.sinamedialab.easyphonefinder;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

public class BootCompletedReceiver extends BroadcastReceiver {

    final static String TAG = "BootCompletedReceiver";
    static final String ACTION = "android.intent.action.BOOT_COMPLETED";   

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.w(TAG, "starting service...");
        if (intent.getAction().equals(ACTION)) { 
        //Service    
        Intent serviceIntent = new Intent(context, FindMyPhone.class);       
        context.startService(serviceIntent);   
        }
        
    }

}