#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>

#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/random.h>

int res = 0;

static ssize_t char_device_read(struct file *filp, char __user *data, size_t s, loff_t *off) {

    if (res == 0) return -EPERM;
    
    int number;
    get_random_bytes(&number, sizeof(number));
    number &= 0x7FFFFFFF;
    number %= res;

    char* random_string;
    int size_rs = snprintf(random_string, sizeof(number), "%d/n", number);

    int min_size = (size_rs < s) ? size_rs : s;
    copy_to_user(data, random_string, min_size);

    return 0;
}

static ssize_t char_device_write(struct file *filp, const char __user *data, size_t s, loff_t *off) {

    char* string_user = (char*)kmalloc(s + 1, GFP_KERNEL);
    copy_from_user(string_user, data, s);
    string_user[s] = 0;

    
    if (kstrtoint(string_user, 10, &res) != 0 ) {
        return -EPERM;
    }


  return s;
}


static struct class *mi_class;

static struct cdev char_dev;

static struct file_operations char_operaciones = {
    .owner = THIS_MODULE,
    .read = char_device_read,
    .write = char_device_write,
};


static dev_t major;

static int __init char_init(void) {
    cdev_init(&char_dev, &char_operaciones);
    alloc_chrdev_region(&major, 0, 1, "char_device");
    cdev_add(&char_dev, major, 1);

    mi_class = class_create(THIS_MODULE, "char_device");
    device_create(mi_class, NULL, major, NULL, "char_device");

    printk(KERN_ALERT "Cargando modulo 'char_device'\n");
	return 0;

}

static void __exit char_exit(void) {
    unregister_chrdev_region(major, 1);
    cdev_del(&char_dev);

    device_destroy(mi_class, major);
    class_destroy(mi_class);

    printk(KERN_ALERT "Descargando modulo 'char_device'.\n");
}

module_init(char_init);
module_exit(char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("La banda de SO");
MODULE_DESCRIPTION("Una suerte de '/dev/null'");