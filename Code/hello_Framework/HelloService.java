package com.android.server;  
import android.content.Context;  
import android.os.IHelloService;  
import android.util.Slog;  

public class HelloService extends IHelloService.Stub {  
    private static final String TAG = "HelloService";  
    HelloService() {  
        init_native();  
    }  
    public void setVal(int val) {  
        setVal_native(val);  
    }     
    public int getVal() {  
        return getVal_native();  
    }  
      
    private static native boolean init_native();  
        private static native void setVal_native(int val);  
    private static native int getVal_native();  
};  
