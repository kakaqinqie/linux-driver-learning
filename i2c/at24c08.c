#include <linux/init.h>
#include <linux/module.h>
#include <linux/i2c.h>

static const struct i2c_device_id at24_ids[] = {
	{ "at24c08", 0},    //8K bit
    {}   
};

struct bin_attribute *at24_bin;

static int at24_read(struct kobject *kobj, struct bin_attribute *attr, char *buf, loff_t off, size_t count)
{
    int res;
    struct i2c_msg msg[2];
    struct device *dev = container_of(kobj, struct device, kobj);
    struct i2c_client *client = container_of(dev, struct i2c_client, dev);

    printk(KERN_DEBUG"at24_read off: %lld, count: %d\n", off, count);

    memset(msg, 0, sizeof(msg));
    msg[0].addr = client->addr;
    msg[0].buf = (u8 *)(&off);
    msg[0].len = 1;
    msg[1].addr = client->addr;
    msg[1].buf = buf;
    msg[1].len = count;
    msg[1].flags = I2C_M_RD;

    res = i2c_transfer(client->adapter, msg, 2);
    if(res == 2) {
        return count;
    } else {
        printk(KERN_DEBUG"i2c_transfer read failed!\n");
        return -ETIMEDOUT;
    }
}

static int at24_write(struct kobject *kobj, struct bin_attribute *attr, char *buf, loff_t off, size_t count)
{
    int res;
    struct i2c_msg msg;
    struct device *dev = container_of(kobj, struct device, kobj);
    struct i2c_client *client = container_of(dev, struct i2c_client, dev);
    u8 msg_buf[20] = {0};

    printk(KERN_DEBUG"at24_write off: %lld, count: %d\n", off, count);
    msg_buf[0] = off && 0xff;
    memcpy(&msg_buf[1], buf, count);
    msg.addr = client->addr;
    msg.buf = msg_buf;
    msg.len = count + 1;

    res = i2c_transfer(client->adapter, &msg, 1);
    if(res == 1) {
        return count;
    } else {
        printk(KERN_DEBUG"i2c_transfer read failed!\n");
        return -ETIMEDOUT;
    }
}

static int at24_probe(struct i2c_client *client, const struct i2c_device_id * device_id)
{
    int res;
    printk(KERN_DEBUG"at24_probe\n");
    
    at24_bin = kzalloc(sizeof(struct bin_attribute), GFP_KERNEL);
    at24_bin->attr.name = "e2prom";
    at24_bin->attr.mode = S_IRUGO | S_IWUGO;
    at24_bin->read = at24_read;
    at24_bin->write = at24_write;
    res = sysfs_create_bin_file(&client->dev.kobj, at24_bin);
    if(res != 0) {
        printk(KERN_ERR"sysfs_create_bin_file!\n");
        return res;
    }
    return 0;
    //i2c_set_clientdata(client, at24_bin);
}

static int at24_remove(struct i2c_client *client)
{
    sysfs_remove_bin_file(&client->dev.kobj, at24_bin);
    kfree(at24_bin);
    return 0;
}

static struct i2c_driver at24_driver = {
    .probe = at24_probe,
    .remove = at24_remove,
    .driver = {
        .name = "at24",
        .owner = THIS_MODULE,
    },
    .id_table = at24_ids,
};

static int __init at24_init(void)
{
   int res;

   res = i2c_add_driver(&at24_driver);
   if(res != 0) {
        printk(KERN_ERR"i2c_add_driver!\n");
        return res;
   }
   return 0;
}

static void __exit at24_exit(void)
{
    i2c_del_driver(&at24_driver);
}


module_init(at24_init);
module_exit(at24_exit);
MODULE_LICENSE("GPL");

