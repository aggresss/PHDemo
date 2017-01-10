package com.android.server;  
import android.content.Context;  
import android.os.IVirtualPositionService;  
import android.util.Slog;  

public class VirtualPositionService extends IVirtualPositionService.Stub {  
    private static final String TAG = "VirtualPositionService";  
    VirtualPositionService() {  
        init_native();  
    }  
    public void setVirtualToggle(int tog) {
        native_set_virtual_toggle(tog);
    }

    public int getVirtualToggle(){
        return native_get_virtual_toggle();
    }

    public void setVirtualLatitude(double vlat) {
        native_set_virtual_latitude(vlat);
    }

    public double getVirtualLatitude(){
        return native_get_virtual_latitude();
    }

    public void setVirtualLongitude(double vlon) {
        native_set_virtual_longitude(vlon);
    }

    public double getVirtualLongitude() {
        return native_get_virtual_longitude();
    }
    
    private static native boolean init_native();
    private static native void native_set_virtual_toggle(int tog);
    private static native int native_get_virtual_toggle();
    private static native void native_set_virtual_latitude(double vlat);
    private static native double native_get_virtual_latitude();
    private static native void native_set_virtual_longitude(double vlon);
    private static native double native_get_virtual_longitude();

};  
