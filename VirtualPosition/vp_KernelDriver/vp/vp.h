#ifndef _VP_ANDROID_H_  
#define _VP_ANDROID_H_  
  
#include <linux/cdev.h>  
#include <linux/semaphore.h>  
  
#define VP_DEVICE_NODE_NAME  "vp"  
#define VP_DEVICE_FILE_NAME  "vp"  
#define VP_DEVICE_PROC_NAME  "vp"  
#define VP_DEVICE_CLASS_NAME "vp"  
  
typedef struct {
    int toggle;
    double virtual_latitude;
    double virtual_longitude;
} VirtualPosition;


struct vp_android_dev {  
    int lamp;
    VirtualPosition val;
    struct semaphore sem;  
    struct cdev dev;  
};  
  
#endif  
