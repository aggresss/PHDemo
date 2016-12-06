#define LOG_TAG "HelloStub"  
  
#include <hardware/hardware.h>  
#include <hardware/hello.h>  
#include <fcntl.h>  
#include <errno.h>  
#include <cutils/log.h>  
#include <cutils/atomic.h>  
  
#define DEVICE_NAME "/dev/hello"  
#define MODULE_NAME "Hello"  
#define MODULE_AUTHOR "shyluo@gmail.com"  
  
/*设备打开和关闭接口*/  
static int hello_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device);  
static int hello_device_close(struct hw_device_t* device);  
  
/*设备访问接口*/  
static int hello_set_val(struct hello_device_t* dev, int val);  
static int hello_get_val(struct hello_device_t* dev, int* val);  
  
/*模块方法表*/  
static struct hw_module_methods_t hello_module_methods = {  
    open: hello_device_open  
};  
  
/*模块实例变量*/  
struct hello_module_t HAL_MODULE_INFO_SYM = {  
    common: {  
        tag: HARDWARE_MODULE_TAG,  
        version_major: 1,  
        version_minor: 0,  
        id: HELLO_HARDWARE_MODULE_ID,  
        name: MODULE_NAME,  
        author: MODULE_AUTHOR,  
        methods: &hello_module_methods,  
    }  
};  


static int hello_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device) {  
    struct hello_device_t* dev;dev = (struct hello_device_t*)malloc(sizeof(struct hello_device_t));  
      
    if(!dev) {  
        ALOGE("Hello Stub: failed to alloc space");  
        return -EFAULT;  
    }  
  
    memset(dev, 0, sizeof(struct hello_device_t));  
    dev->common.tag = HARDWARE_DEVICE_TAG;  
    dev->common.version = 0;  
    dev->common.module = (hw_module_t*)module;  
    dev->common.close = hello_device_close;  
    dev->set_val = hello_set_val;dev->get_val = hello_get_val;  
  
    if((dev->fd = open(DEVICE_NAME, O_RDWR)) == -1) {  
        ALOGE("Hello Stub: failed to open /dev/hello -- %s.", strerror(errno));free(dev);  
        return -EFAULT;  
    }  
  
    *device = &(dev->common);  
    ALOGI("Hello Stub: open /dev/hello successfully.");  
  
    return 0;  
}  

static int hello_device_close(struct hw_device_t* device) {  
    struct hello_device_t* hello_device = (struct hello_device_t*)device;  
  
    if(hello_device) {  
        close(hello_device->fd);  
        free(hello_device);  
    }  
      
    return 0;  
}  
  
static int hello_set_val(struct hello_device_t* dev, int val) {  
    ALOGI("Hello Stub: set value %d to device.", val);  
  
    write(dev->fd, &val, sizeof(val));  
  
    return 0;  
}  
  
static int hello_get_val(struct hello_device_t* dev, int* val) {  
    if(!val) {  
        ALOGE("Hello Stub: error val pointer");  
        return -EFAULT;  
    }  
  
    read(dev->fd, val, sizeof(*val));  
  
    ALOGI("Hello Stub: get value %d from device", *val);  
  
    return 0;  
}  
