#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <asm/uaccess.h>
#include <asm/kvm_para.h>
#include <asm/io.h>
#include <poke.h>

#define DEVICE_NAME "poke"

static int major_number;

static ssize_t poke_write(struct file *f, const char __user *b, size_t s, loff_t *o) {
    kvm_hypercall1(4, 0);
    return s;
}

struct file_operations fops = {
    .write = poke_write,
};

static int __init poke_init(void) {
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    printk(KERN_INFO "%s initialized with major number %d\n", DEVICE_NAME, major_number);
    return 0;
}

static void __exit poke_exit(void) {
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "%s exit\n", DEVICE_NAME);
}

module_init(poke_init);
module_exit(poke_exit);
