/*******************************************
*include file and define functions
*******************************************/
#include <linux/init.h>  
#include <linux/module.h>  
#include <linux/types.h>  
#include <linux/fs.h>  
#include <linux/proc_fs.h>  
#include <linux/device.h>  
#include <asm/uaccess.h>  
  
#include "vp.h"  
  
/*主设备和从设备号变量*/  
static int vp_major = 0;  
static int vp_minor = 0;  
  
/*设备类别和设备变量*/  
static struct class* vp_class = NULL;  
static struct vp_android_dev* vp_dev = NULL;  
  
/*传统的设备文件操作方法*/  
static int vp_open(struct inode* inode, struct file* filp);  
static int vp_release(struct inode* inode, struct file* filp);  
static ssize_t vp_read(struct file* filp, char __user *buf, size_t count, loff_t* f_pos);  
static ssize_t vp_write(struct file* filp, const char __user *buf, size_t count, loff_t* f_pos);  
  
/*设备文件操作方法表*/  
static struct file_operations vp_fops = {  
    .owner = THIS_MODULE,  
    .open = vp_open,  
    .release = vp_release,  
    .read = vp_read,  
    .write = vp_write,   
};  
  
/*访问设置属性方法*/  
static ssize_t vp_lamp_show(struct device* dev, struct device_attribute* attr,  char* buf);  
static ssize_t vp_lamp_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count);  
  
/*定义设备属性*/  
static DEVICE_ATTR(val, S_IRUGO | S_IWUSR, vp_lamp_show, vp_lamp_store);

/*******************************************
*define traditional file access 
*******************************************/
/*打开设备方法*/  
static int vp_open(struct inode* inode, struct file* filp) {  
    struct vp_android_dev* dev;          
      
    /*将自定义设备结构体保存在文件指针的私有数据域中，以便访问设备时拿来用*/  
    dev = container_of(inode->i_cdev, struct vp_android_dev, dev);  
    filp->private_data = dev;  
      
    return 0;  
}  
  
/*设备文件释放时调用，空实现*/  
static int vp_release(struct inode* inode, struct file* filp) {  
    return 0;  
}  
  
/*读取设备的寄存器val的值*/  
static ssize_t vp_read(struct file* filp, char __user *buf, size_t count, loff_t* f_pos) {  
    ssize_t err = 0;  
    struct vp_android_dev* dev = filp->private_data;          
  
    /*同步访问*/  
    if(down_interruptible(&(dev->sem))) {  
        return -ERESTARTSYS;  
    }  
  
    if(count < sizeof(dev->val)) {  
        goto out;  
    }          
  
    /*将寄存器val的值拷贝到用户提供的缓冲区*/  
    if(copy_to_user(buf, &(dev->val), sizeof(dev->val))) {  
        err = -EFAULT;  
        goto out;  
    }  
  
    err = sizeof(dev->val);  
  
out:  
    up(&(dev->sem));  
    return err;  
}  
  
/*写设备的寄存器值val*/  
static ssize_t vp_write(struct file* filp, const char __user *buf, size_t count, loff_t* f_pos) {  
    struct vp_android_dev* dev = filp->private_data;  
    ssize_t err = 0;          
  
    /*同步访问*/  
    if(down_interruptible(&(dev->sem))) {  
        return -ERESTARTSYS;          
    }          
  
    if(count != sizeof(dev->val)) {  
        goto out;          
    }          
  
    /*将用户提供的缓冲区的值写到设备寄存器去*/  
    if(copy_from_user(&(dev->val), buf, count)) {  
        err = -EFAULT;  
        goto out;  
    }  
  
    err = sizeof(dev->val);  
  
out:  
    up(&(dev->sem));  
    return err;  
}  

/*******************************************
*define devfs access
*******************************************/
/*读取寄存器lamp的值到缓冲区buf中，内部使用*/  
static ssize_t __vp_get_lamp(struct vp_android_dev* dev, char* buf) {  
    int lamp = 0;          
  
    /*同步访问*/  
    if(down_interruptible(&(dev->sem))) {                  
        return -ERESTARTSYS;          
    }          
  
    lamp = dev->lamp;          
    up(&(dev->sem));          
  
    return snprintf(buf, PAGE_SIZE, "%d\n", lamp);  
}  
  
/*把缓冲区buf的值写到设备寄存器lamp中去，内部使用*/  
static ssize_t __vp_set_lamp(struct vp_android_dev* dev, const char* buf, size_t count) {  
    int lamp = 0;          
  
    /*将字符串转换成数字*/          
    lamp = simple_strtol(buf, NULL, 10);          
  
    /*同步访问*/          
    if(down_interruptible(&(dev->sem))) {                  
        return -ERESTARTSYS;          
    }          
  
    dev->lamp = lamp;          
    up(&(dev->sem));  
  
    return count;  
}  
  
/*读取设备属性lamp*/  
static ssize_t vp_lamp_show(struct device* dev, struct device_attribute* attr, char* buf) {  
    struct vp_android_dev* hdev = (struct vp_android_dev*)dev_get_drvdata(dev);          
  
    return __vp_get_lamp(hdev, buf);  
}  
  
/*写设备属性lamp*/  
static ssize_t vp_lamp_store(struct device* dev, struct device_attribute* attr, const char* buf, size_t count) {   
    struct vp_android_dev* hdev = (struct vp_android_dev*)dev_get_drvdata(dev);    
      
    return __vp_set_lamp(hdev, buf, count);  
}  

/*******************************************
*define proc access
*******************************************/
/*读取设备寄存器lamp的值，保存在page缓冲区中*/  
static ssize_t vp_proc_read(char* page, char** start, off_t off, int count, int* eof, void* data) {  
    if(off > 0) {  
        *eof = 1;  
        return 0;  
    }  
  
    return __vp_get_lamp(vp_dev, page);  
}  
  
/*把缓冲区的值buff保存到设备寄存器lamp中去*/  
static ssize_t vp_proc_write(struct file* filp, const char __user *buff, unsigned long len, void* data) {  
    int err = 0;  
    char* page = NULL;  
  
    if(len > PAGE_SIZE) {  
        printk(KERN_ALERT"The buff is too large: %lu.\n", len);  
        return -EFAULT;  
    }  
  
    page = (char*)__get_free_page(GFP_KERNEL);  
    if(!page) {                  
        printk(KERN_ALERT"Failed to alloc page.\n");  
        return -ENOMEM;  
    }          
  
    /*先把用户提供的缓冲区值拷贝到内核缓冲区中去*/  
    if(copy_from_user(page, buff, len)) {  
        printk(KERN_ALERT"Failed to copy buff from user.\n");                  
        err = -EFAULT;  
        goto out;  
    }  
  
    err = __vp_set_lamp(vp_dev, page, len);  
  
out:  
    free_page((unsigned long)page);  
    return err;  
}  
  
/*创建/proc/vp文件*/  
static void vp_create_proc(void) {
	struct proc_dir_entry *entry;
	entry = create_proc_entry(VP_DEVICE_PROC_NAME, 0, NULL);
	if(entry)
	{
		entry->read_proc = vp_proc_read;
		entry->write_proc = vp_proc_write;
	}
}  
  
/*删除/proc/vp文件*/  
static void vp_remove_proc(void) {  
    remove_proc_entry(VP_DEVICE_PROC_NAME, NULL);  
}  

/*******************************************
*define load and remove function
*******************************************/
/*初始化设备*/  
static int  __vp_setup_dev(struct vp_android_dev* dev) {  
    int err;  
    dev_t devno = MKDEV(vp_major, vp_minor);  
  
    memset(dev, 0, sizeof(struct vp_android_dev));  
  
    cdev_init(&(dev->dev), &vp_fops);  
    dev->dev.owner = THIS_MODULE;  
    dev->dev.ops = &vp_fops;          
  
    /*注册字符设备*/  
    err = cdev_add(&(dev->dev),devno, 1);  
    if(err) {  
        return err;  
    }          
  
    /*初始化信号量和寄存器lamp, val的值*/  
    sema_init(&(dev->sem), 1);  
    dev->lamp = 7777;  
    dev->val.toggle = 1;  
    dev->val.virtual_latitude = 45.104108; 
    dev->val.virtual_longitude = 130.816878; 
 
    return 0;  
}  
  
/*模块加载方法*/  
static int __init vp_init(void){   
    int err = -1;  
    dev_t dev = 0;  
    struct device* temp = NULL;  
  
    printk(KERN_ALERT"Initializing vp device.\n");          
  
    /*动态分配主设备和从设备号*/  
    err = alloc_chrdev_region(&dev, 0, 1, VP_DEVICE_NODE_NAME);  
    if(err < 0) {  
        printk(KERN_ALERT"Failed to alloc char dev region.\n");  
        goto fail;  
    }  
  
    vp_major = MAJOR(dev);  
    vp_minor = MINOR(dev);          
  
    /*分配helo设备结构体变量*/  
    vp_dev = kmalloc(sizeof(struct vp_android_dev), GFP_KERNEL);  
    if(!vp_dev) {  
        err = -ENOMEM;  
        printk(KERN_ALERT"Failed to alloc vp_dev.\n");  
        goto unregister;  
    }          
  
    /*初始化设备*/  
    err = __vp_setup_dev(vp_dev);  
    if(err) {  
        printk(KERN_ALERT"Failed to setup dev: %d.\n", err);  
        goto cleanup;  
    }          
  
    /*在/sys/class/目录下创建设备类别目录vp*/  
    vp_class = class_create(THIS_MODULE, VP_DEVICE_CLASS_NAME);  
    if(IS_ERR(vp_class)) {  
        err = PTR_ERR(vp_class);  
        printk(KERN_ALERT"Failed to create vp class.\n");  
        goto destroy_cdev;  
    }          
  
    /*在/dev/目录和/sys/class/vp目录下分别创建设备文件vp*/  
    temp = device_create(vp_class, NULL, dev, "%s", VP_DEVICE_FILE_NAME);  
    if(IS_ERR(temp)) {  
        err = PTR_ERR(temp);  
        printk(KERN_ALERT"Failed to create vp device.");  
        goto destroy_class;  
    }          
  
    /*在/sys/class/vp/vp目录下创建属性文件val*/  
    err = device_create_file(temp, &dev_attr_val);  
    if(err < 0) {  
        printk(KERN_ALERT"Failed to create attribute val.");                  
        goto destroy_device;  
    }  
  
    dev_set_drvdata(temp, vp_dev);          
  
    /*创建/proc/vp文件*/  
    vp_create_proc();  
  
    printk(KERN_ALERT"Succedded to initialize vp device.\n");  
    return 0;  
  
destroy_device:  
    device_destroy(vp_class, dev);  
  
destroy_class:  
    class_destroy(vp_class);  
  
destroy_cdev:  
    cdev_del(&(vp_dev->dev));  
  
cleanup:  
    kfree(vp_dev);  
  
unregister:  
    unregister_chrdev_region(MKDEV(vp_major, vp_minor), 1);  
  
fail:  
    return err;  
}  
  
/*模块卸载方法*/  
static void __exit vp_exit(void) {  
    dev_t devno = MKDEV(vp_major, vp_minor);  
  
    printk(KERN_ALERT"Destroy vp device.\n");          
  
    /*删除/proc/vp文件*/  
    vp_remove_proc();          
  
    /*销毁设备类别和设备*/  
    if(vp_class) {  
        device_destroy(vp_class, MKDEV(vp_major, vp_minor));  
        class_destroy(vp_class);  
    }          
  
    /*删除字符设备和释放设备内存*/  
    if(vp_dev) {  
        cdev_del(&(vp_dev->dev));  
        kfree(vp_dev);  
    }          
  
    /*释放设备号*/  
    unregister_chrdev_region(devno, 1);  
}  
  
MODULE_LICENSE("GPL");  
MODULE_DESCRIPTION("Virtualposition Driver");  
  
module_init(vp_init);  
module_exit(vp_exit);  
