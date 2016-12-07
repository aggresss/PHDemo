#define LOG_TAG "HelloService"  
#include "jni.h"  
#include "JNIHelp.h"  
#include "android_runtime/AndroidRuntime.h"  
#include <utils/misc.h>  
#include <utils/Log.h>  
#include <hardware/hardware.h>  
#include <hardware/hello.h>  
#include <stdio.h>  

namespace android  
{  
    /*在硬件抽象层中定义的硬件访问结构体，参考<hardware/hello.h>*/  
        struct hello_device_t* hello_device = NULL;  
    /*通过硬件抽象层定义的硬件访问接口设置硬件寄存器val的值*/  
        static void hello_setVal(JNIEnv* env, jobject clazz, jint value) {  
        int val = value;  
        ALOGI("Hello JNI: set value %d to device.", val);  
        if(!hello_device) {  
            ALOGI("Hello JNI: device is not open.");  
            return;  
        }  
          
        hello_device->set_val(hello_device, val);  
    }  
        /*通过硬件抽象层定义的硬件访问接口读取硬件寄存器val的值*/  
    static jint hello_getVal(JNIEnv* env, jobject clazz) {  
        int val = 0;  
        if(!hello_device) {  
            ALOGI("Hello JNI: device is not open.");  
            return val;  
        }  
        hello_device->get_val(hello_device, &val);  
          
        ALOGI("Hello JNI: get value %d from device.", val);  
      
        return val;  
    }  
        /*通过硬件抽象层定义的硬件模块打开接口打开硬件设备*/  
    static inline int hello_device_open(const hw_module_t* module, struct hello_device_t** device) {  
        return module->methods->open(module, HELLO_HARDWARE_MODULE_ID, (struct hw_device_t**)device);  
    }  
        /*通过硬件模块ID来加载指定的硬件抽象层模块并打开硬件*/  
    static jboolean hello_init(JNIEnv* env, jclass clazz) {  
        hello_module_t* module;  
          
        ALOGI("Hello JNI: initializing......");  
        if(hw_get_module(HELLO_HARDWARE_MODULE_ID, (const struct hw_module_t**)&module) == 0) {  
            ALOGI("Hello JNI: hello Stub found.");  
            if(hello_device_open(&(module->common), &hello_device) == 0) {  
                ALOGI("Hello JNI: hello device is open.");  
                return 0;  
            }  
            ALOGE("Hello JNI: failed to open hello device.");  
            return -1;  
        }  
        ALOGE("Hello JNI: failed to get hello stub module.");  
        return -1;        
    }  
        /*JNI方法表*/  
    static const JNINativeMethod method_table[] = {  
        {"init_native", "()Z", (void*)hello_init},  
        {"setVal_native", "(I)V", (void*)hello_setVal},  
        {"getVal_native", "()I", (void*)hello_getVal},  
    };  
        /*注册JNI方法*/  
    int register_android_server_HelloService(JNIEnv *env) {  
            return jniRegisterNativeMethods(env, "com/android/server/HelloService", method_table, NELEM(method_table));  
    }  
};  
