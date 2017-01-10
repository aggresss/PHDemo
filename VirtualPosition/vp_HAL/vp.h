 #ifndef ANDROID_VP_INTERFACE_H  
#define ANDROID_VP_INTERFACE_H  
#include <hardware/hardware.h>  
__BEGIN_DECLS  
  
/*定义模块ID*/  
#define VP_HARDWARE_MODULE_ID "vp"  

//typedef enum{false, true} bool;

/*define virtual position structrue*/
typedef struct {
    int toggle;
    double virtual_latitude;
    double virtual_longitude;
} VirtualPosition;



/*硬件模块结构体*/  
struct vp_module_t {  
    struct hw_module_t common;  
};  
  
/*硬件接口结构体*/  
struct vp_device_t {  
    struct hw_device_t common;  
    int fd;  
    int (*set_val)(struct vp_device_t* dev, VirtualPosition val);  
    int (*get_val)(struct vp_device_t* dev, VirtualPosition* val);  
};  
  
__END_DECLS  
  
#endif  
