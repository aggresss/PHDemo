package com.example.phdemo.myapplication;

import android.os.RemoteException;
import android.app.Activity;
import android.os.ServiceManager;
import android.os.Bundle;
import android.widget.CompoundButton;
import android.os.IVirtualPositionService;
import android.os.RemoteException;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ToggleButton;
import android.widget.CompoundButton.OnCheckedChangeListener;

public class MainActivity extends Activity implements View.OnClickListener {

    private final static String LOG_TAG = "com.example.phdemo.virtualposition";

    private IVirtualPositionService virtualpositionService = null;
	private ToggleButton toggleButton = null;
	private EditText altitudeValueText = null;
    private EditText longitudeValueText = null;
    private Button getButton = null;
    private Button setButton = null;
    private Button clearButton = null;

    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        virtualpositionService = IVirtualPositionService.Stub.asInterface(
                ServiceManager.getService("virtualposition"));

        toggleButton=(ToggleButton)findViewById(R.id.toggleButton);
		altitudeValueText = (EditText)findViewById(R.id.altitude_value);
		longitudeValueText = (EditText)findViewById(R.id.longitude_value);
        getButton = (Button)findViewById(R.id.button_get);
        setButton = (Button)findViewById(R.id.button_set);
        clearButton = (Button)findViewById(R.id.button_clear);
        
        getButton.setOnClickListener(this);
        setButton.setOnClickListener(this);
        clearButton.setOnClickListener(this);
	    
        try{	
        int val_tog = virtualpositionService.getVirtualToggle();
            if(val_tog == 1){
                toggleButton.setChecked(true);
            }else{
                toggleButton.setChecked(false);
            }
        } catch (Exception e) {}

        toggleButton.setOnCheckedChangeListener(new OnCheckedChangeListener(){
			public void onCheckedChanged(CompoundButton buttonView,boolean isChecked) {
				toggleButton.setChecked(isChecked);
				try{
                    virtualpositionService.setVirtualToggle(isChecked?1:0);
                }catch(Exception e){}
			}
		});
        Log.i(LOG_TAG, "VirtualPosition Activity Created");
    }

    @Override
    public void onClick(View v) {
        if(v.equals(getButton)) {
            try {
                double val_altitude = virtualpositionService.getVirtualLatitude();
                String text_altitude = String.valueOf(val_altitude);
                altitudeValueText.setText(text_altitude);
				double val_longitude = virtualpositionService.getVirtualLongitude();
                String text_longitude = String.valueOf(val_longitude);
                longitudeValueText.setText(text_longitude);
				int val_tog = virtualpositionService.getVirtualToggle();
                if(val_tog == 1){
                    toggleButton.setChecked(true);
                }else{
                    toggleButton.setChecked(false);
                }
            } catch (Exception e) {
                Log.e(LOG_TAG, "Remote Exception while reading value from GpsLocationProvider.");
            }
        }
        else if(v.equals(setButton)) {
            try {
                String text_altitude = altitudeValueText.getText().toString();
				String text_longitude = longitudeValueText.getText().toString();
                double val_altitude = Double.parseDouble(text_altitude);
				double val_longitude = Double.parseDouble(text_longitude);
                virtualpositionService.setVirtualLatitude(val_altitude);
				virtualpositionService.setVirtualLongitude(val_longitude);
            } catch (Exception e) {
                Log.e(LOG_TAG, "Remote Exception while writing value to GpsLocationProvider.");
            }
        }
        else if(v.equals(clearButton)) {
            String text = "";
            altitudeValueText.setText(text);
			longitudeValueText.setText(text);
        }
    }
}
