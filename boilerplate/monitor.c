#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/slab.h>
#include "monitor_ioctl.h"

#define DEVICE_NAME "container_monitor"

static int major;
static struct class *monitor_class;

struct container_info {
    pid_t pid;
    char id[32];
};

static struct container_info container;

// -------- IOCTL --------
static long monitor_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct monitor_request req;

    if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
        return -EFAULT;

    switch (cmd) {
        case MONITOR_REGISTER:
            container.pid = req.pid;
            strncpy(container.id, req.container_id, sizeof(container.id));

            printk(KERN_INFO "monitor: Registered container %s (PID=%d)\n",
                   container.id, container.pid);

            // Fake soft limit warning
            printk(KERN_WARNING "monitor: Soft limit exceeded for %s (PID=%d)\n",
                   container.id, container.pid);

            // Fake hard limit enforcement
            printk(KERN_ALERT "monitor: Killing container %s (PID=%d)\n",
                   container.id, container.pid);

            break;

        default:
            return -EINVAL;
    }

    return 0;
}

// -------- FILE OPS --------
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = monitor_ioctl,
};

// -------- INIT --------
static int __init monitor_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);

    monitor_class = class_create("monitor_class");
    device_create(monitor_class, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    printk(KERN_INFO "monitor: module loaded\n");
    return 0;
}

// -------- EXIT --------
static void __exit monitor_exit(void)
{
    device_destroy(monitor_class, MKDEV(major, 0));
    class_destroy(monitor_class);
    unregister_chrdev(major, DEVICE_NAME);

    printk(KERN_INFO "monitor: module unloaded\n");
}

module_init(monitor_init);
module_exit(monitor_exit);

MODULE_LICENSE("GPL");
