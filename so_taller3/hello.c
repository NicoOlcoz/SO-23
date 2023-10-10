#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/device.h>

static ssize_t char_device_read(struct file *filp, char __user *data, size_t s, loff_t *off) {
  return 0;
}


static struct class *mi_class;

static struct cdev char_dev;

static struct file_operations char_operaciones = {
    .owner = THIS_MODULE,
    .read = char_device_read,
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