#define LOG_TAG "VpStub"  
  
#include <hardware/hardware.h>  
#include <hardware/vp.h>  
#include <fcntl.h>  
#include <errno.h>  
#include <cutils/log.h>  
#include <cutils/atomic.h>  
  
#define DEVICE_NAME "/dev/vp"  
#define MODULE_NAME "Vp"  
#define MODULE_AUTHOR "aggresss@163.com"  
  
/*设备打开和关闭接口*/  
static int vp_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device);  
static int vp_device_close(struct hw_device_t* device);  
  
/*设备访问接口*/  
static int vp_set_val(struct vp_device_t* dev, VirtualPosition val);  
static int vp_get_val(struct vp_device_t* dev, VirtualPosition* val);  
  
/*模块方法表*/  
static struct hw_module_methods_t vp_module_methods = {  
    open: vp_device_open  
};  
  
/*模块实例变量*/  
struct vp_module_t HAL_MODULE_INFO_SYM = {  
    common: {  
        tag: HARDWARE_MODULE_TAG,  
        version_major: 1,  
        version_minor: 0,  
        id: VP_HARDWARE_MODULE_ID,  
        name: MODULE_NAME,  
        author: MODULE_AUTHOR,  
        methods: &vp_module_methods,  
    }  
};  


static int vp_device_open(const struct hw_module_t* module, const char* name, struct hw_device_t** device) {  
    struct vp_device_t* dev;dev = (struct vp_device_t*)malloc(sizeof(struct vp_device_t));  
      
    if(!dev) {  
        ALOGE("Vp Stub: failed to alloc space");  
        return -EFAULT;  
    }  
  
    memset(dev, 0, sizeof(struct vp_device_t));  
    dev->common.tag = HARDWARE_DEVICE_TAG;  
    dev->common.version = 0;  
    dev->common.module = (hw_module_t*)module;  
    dev->common.close = vp_device_close;  
    dev->set_val = vp_set_val;dev->get_val = vp_get_val;  
  
    if((dev->fd = open(DEVICE_NAME, O_RDWR)) == -1) {  
        ALOGE("Vp Stub: failed to open /dev/vp -- %s.", strerror(errno));free(dev);  
        return -EFAULT;  
    }  
  
    *device = &(dev->common);  
    ALOGI("Vp Stub: open /dev/vp successfully.");  
  
    return 0;  
}  

static int vp_device_close(struct hw_device_t* device) {  
    struct vp_device_t* vp_device = (struct vp_device_t*)device;  
  
    if(vp_device) {  
        close(vp_device->fd);  
        free(vp_device);  
    }  
      
    return 0;  
}  
  
static int vp_set_val(struct vp_device_t* dev, VirtualPosition val) {  
    ALOGI("Vp Stub: set value %d to device.", val);  
  
    write(dev->fd, &val, sizeof(val));  
  
    return 0;  
}  
  
static int vp_get_val(struct vp_device_t* dev, VirtualPosition* val) {  
    if(!val) {  
        ALOGE("Vp Stub: error val pointer");  
        return -EFAULT;  
    }  
  
    read(dev->fd, val, sizeof(*val));  
  
    ALOGI("Vp Stub: get value %d from device", *val);  
  
    return 0;  
}  
