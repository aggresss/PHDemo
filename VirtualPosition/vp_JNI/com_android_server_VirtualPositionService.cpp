#define LOG_TAG "VirtualPositionService"  
#include "jni.h"  
#include "JNIHelp.h"  
#include "android_runtime/AndroidRuntime.h"  
#include <utils/misc.h>  
#include <utils/Log.h>  
#include <hardware/hardware.h>  
#include <hardware/vp.h>  
#include <stdio.h>  

namespace android  
{  
    
        VirtualPosition virtual_position = {1, 0.0, 0.0};
    /*在硬件抽象层中定义的硬件访问结构体，参考<hardware/vp.h>*/  
        struct vp_device_t* vp_device = NULL;  
    /*通过硬件抽象层定义的硬件访问接口设置硬件寄存器val的值*/  
        static void vp_setVal() {  
        ALOGI("VirtualPosition JNI: set value to device.");  
        if(!vp_device) {  
            ALOGI("VirtualPosition JNI: device is not open.");  
            return;  
        }  
          
        vp_device->set_val(vp_device, virtual_position);  
    }  
        /*通过硬件抽象层定义的硬件访问接口读取硬件寄存器val的值*/  
    static void vp_getVal() {  
        if(!vp_device) {  
            ALOGI("VirtualPosition JNI: device is not open.");  
        }  
        vp_device->get_val(vp_device, &virtual_position);  
          
        ALOGI("VirtualPosition JNI: get value from device.");  
    }  
        /*通过硬件抽象层定义的硬件模块打开接口打开硬件设备*/  
    static inline int vp_device_open(const hw_module_t* module, struct vp_device_t** device) {  
        return module->methods->open(module, VP_HARDWARE_MODULE_ID, (struct hw_device_t**)device);  
    }  
        /*通过硬件模块ID来加载指定的硬件抽象层模块并打开硬件*/  
    static jboolean vp_init(JNIEnv* env, jclass clazz) {  
        vp_module_t* module;  
          
        ALOGI("VirtualPosition JNI: initializing......");  
        if(hw_get_module(VP_HARDWARE_MODULE_ID, (const struct hw_module_t**)&module) == 0) {  
            ALOGI("VirtualPosition JNI: vp Stub found.");  
            if(vp_device_open(&(module->common), &vp_device) == 0) {  
                ALOGI("VirtualPosition JNI: vp device is open.");  
                return 0;  
            }  
            ALOGE("VirtualPosition JNI: failed to open vp device.");  
            return -1;  
        }  
        ALOGE("VirtualPosition JNI: failed to get vp stub module.");  
        return -1;        
    }  
    
    static void android_server_VirtualPositionService_set_virtual_toggle(JNIEnv* env, jclass clazz, jint tog)
    {
        virtual_position.toggle = tog;
        vp_setVal();
    }

    static jint android_server_VirtualPositionService_get_virtual_toggle(JNIEnv* env, jclass clazz)
    {
        vp_getVal();
        return virtual_position.toggle;
    }

    static void android_server_VirtualPositionService_set_virtual_latitude(JNIEnv* env, jclass clazz, jdouble vlat)
    {
        virtual_position.virtual_latitude = vlat;
        vp_setVal();
    }

    static jdouble android_server_VirtualPositionService_get_virtual_latitude(JNIEnv* env, jclass clazz)
    {
        vp_getVal();
        return virtual_position.virtual_latitude;
    }

    static void android_server_VirtualPositionService_set_virtual_longitude(JNIEnv* env, jclass clazz, jdouble vlon)
    {
        virtual_position.virtual_longitude = vlon;
        vp_setVal();
    }

    static jdouble android_server_VirtualPositionService_get_virtual_longitude(JNIEnv* env, jclass clazz)
    {
        vp_getVal();
        return virtual_position.virtual_longitude;
    }



    /*JNI方法表*/  
    static const JNINativeMethod method_table[] = {  
        {"init_native",
            "()Z", 
            (void*)vp_init},  
        {"native_set_virtual_toggle",
            "(I)V",
            (void*)android_server_VirtualPositionService_set_virtual_toggle},
        {"native_get_virtual_toggle",
            "()I",
            (void*)android_server_VirtualPositionService_get_virtual_toggle},
        {"native_set_virtual_latitude",
            "(D)V",
            (void*)android_server_VirtualPositionService_set_virtual_latitude},
        {"native_get_virtual_latitude",
            "()D",
            (void*)android_server_VirtualPositionService_get_virtual_latitude},
        {"native_set_virtual_longitude",
            "(D)V",
            (void*)android_server_VirtualPositionService_set_virtual_longitude},
        {"native_get_virtual_longitude",
            "()D",
            (void*)android_server_VirtualPositionService_get_virtual_longitude},

    };  
        /*注册JNI方法*/  
    int register_android_server_VirtualPositionService(JNIEnv *env) {  
            return jniRegisterNativeMethods(env, "com/android/server/VirtualPositionService", method_table, NELEM(method_table));  
    }  
};  
