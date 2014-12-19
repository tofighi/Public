package com.simaafzar.easyphonefinder;

import android.content.Context;
import android.content.Intent;
import android.location.Location;
import android.location.LocationListener;
import android.location.LocationManager;
import android.os.Bundle;
import android.util.Log;

public class GPSLocation implements LocationListener {
	private static final String TAG = "GPSLocation";
	private static final String LONGITUDE = "Longitude: ";
	private static final String LATITUDE = "Latitude: ";
	private static final String GEO_LOCATION = "Geo Location";

	private LocationManager mlocationManager;
	private double latitude;
	private double longitude;
	private Location location;
	private Context myContext;

	public GPSLocation(Context context) {
		// TODO Auto-generated constructor stub
		myContext = context;
		mlocationManager = (LocationManager) context
				.getSystemService(Context.LOCATION_SERVICE);
		Log.i("GPSLocation", "mlocationManager created!");
		if (mlocationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)) {
			Log.i("GPSLocation", "GPS is enabled!");
			mlocationManager.requestLocationUpdates(
					LocationManager.GPS_PROVIDER, 0, 0, this);
			location = mlocationManager
					.getLastKnownLocation(LocationManager.GPS_PROVIDER);
			latitude = location.getLatitude();
			longitude = location.getLongitude();
		} else {
			Log.i("GPSLocation", "GPS is not enabled!");
		}

	}

	public double getLatitude() {
		return latitude;
	}

	public double getLongitude() {
		return longitude;

	}

	public void stopGPS() {
		// Stop GPS updates
		mlocationManager.removeUpdates(this);
		Log.i(TAG, "GPS Stoped!");

	}

	@Override
	public void onLocationChanged(Location location) {
		if (location != null) {
			
				latitude = location.getLatitude();
				longitude = location.getLongitude();
				Intent intent = new Intent(
						"com.simaafzar.easyphonefinder.ACTION_LOCATION");
				myContext.sendBroadcast(intent);
				Log.i(GEO_LOCATION, LATITUDE + latitude + "," + LONGITUDE + longitude);	
		}
		
	}

	@Override
	public void onProviderDisabled(String provider) {
		// TODO Auto-generated method stub
		Log.i("GPSLocation", "GPS Provider Disabled!");

	}

	@Override
	public void onProviderEnabled(String provider) {
		// TODO Auto-generated method stub

	}

	@Override
	public void onStatusChanged(String provider, int status, Bundle extras) {
		// TODO Auto-generated method stub

	}

}