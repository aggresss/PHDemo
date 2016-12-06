 #ifndef ANDROID_HELLO_INTERFACE_H  
#define ANDROID_HELLO_INTERFACE_H  
#include <hardware/hardware.h>  
  
__BEGIN_DECLS  
  
/*定义模块ID*/  
#define HELLO_HARDWARE_MODULE_ID "hello"  
  
/*硬件模块结构体*/  
struct hello_module_t {  
    struct hw_module_t common;  
};  
  
/*硬件接口结构体*/  
struct hello_device_t {  
    struct hw_device_t common;  
    int fd;  
    int (*set_val)(struct hello_device_t* dev, int val);  
    int (*get_val)(struct hello_device_t* dev, int* val);  
};  
  
__END_DECLS  
  
#endif  
