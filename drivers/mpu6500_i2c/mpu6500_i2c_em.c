#include <linux/kernel.h>
#include <linux/of.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/err.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define MPU6050_I2C_ADDR 		0x68

#define MPU6500_SAMPLE_DIV		0x19
#define MPU6500_CONFIG			0x1A
#define MPU6500_GYRO_CONFIG		0x1B
#define MPU6500_ACCEL_CONFIG1		0x1C
#define MPU6500_ACCEL_CONFIG2		0x1D
#define	MPU6500_FIFO_EN			0x23
#define MPU6500_INT_PIN_CFG		0x37
#define MPU6500_INT_ENABLE		0x38
#define MPU6500_INT_STATUS		0x3A
#define MPU6500_ACCEL_XOUT		0x3B
	#define MPU6500_ACCEL_XOUT_H	0x3B
	#define MPU6500_ACCEL_XOUT_L	0x3C
#define MPU6500_ACCEL_YOUT		0x3D
	#define MPU6500_ACCEL_YOUT_H	0x3D
	#define MPU6500_ACCEL_YOUT_L	0x3E
#define MPU6500_ACCEL_ZOUT		0x3F
	#define MPU6500_ACCEL_ZOUT_H	0x3F
	#define MPU6500_ACCEL_ZOUT_L	0x40
#define MPU6500_TEMP_OUT		0x41
	#define MPU6500_TEMP_OUT_H	0x41
	#define MPU6500_TEMP_OUT_L	0x42
#define MPU6500_GYRO_XOUT		0x43
	#define MPU6500_GYRO_XOUT_H	0x43
	#define MPU6500_GYRO_XOUT_L	0x44
#define MPU6500_GYRO_YOUT		0x45
	#define MPU6500_GYRO_YOUT_H	0x45
	#define MPU6500_GYRO_YOUT_L	0x46
#define MPU6500_GYRO_ZOUT		0x47
	#define MPU6500_GYRO_ZOUT_H	0x47
	#define MPU6500_GYRO_ZOUT_L	0x48
#define MPU6500_USER_CTRL		0x6A
#define MPU6500_PWR_MGMT_1		0x6B
#define MPU6500_PWR_MGMT_2		0x6C
#define MPU6500_REG_FIFO		0x74

#define I2C_BUS					(1) /*I2C used to coenctc mpu sensor*/
#define DRV_NAME 				"mpu6500"
#define CLASS_NAME 				"mpu6500_class"
#define CLIENT_NAME 				"mpu6500_dev"
#define CLIENT_DEVICE 				"mpu6500_device"

 
#define I2C_BUS_AVAILABLE   (          1 )              // I2C Bus available in our Raspberry Pi
#define SLAVE_DEVICE_NAME   ( "ETX_SENS" )              // Device and Driver Name
 
static struct i2c_adapter *etx_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *etx_i2c_client_cap_sense = NULL;  // I2C Cient Structure (In our case it is CAP1188)
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev etx_cdev;

/*************** Driver functions **********************/
static int etx_open(struct inode *inode, struct file *file);
static int etx_release(struct inode *inode, struct file *file);
static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t etx_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
/******************************************************/
static void CAP1188_write(unsigned char reg, unsigned char data);
static int I2C_read(unsigned char *out_buf, unsigned int len);
static int I2C_write(unsigned char *buf, unsigned int len);
//File operation structure 
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .read           = etx_read,
  .write          = etx_write,
  .open           = etx_open,
  .release        = etx_release,
};
/********************************************************/

static int etx_open(struct inode *inode, struct file *file)
{
    /*
    ** This function will be called when we open the Device file
    */ 
    pr_info("Device File Opened...!!!\n");
    return 0;
}


static int etx_release(struct inode *inode, struct file *file)
{
    /*
    ** This function will be called when we close the Device file
    */
    pr_info("Device File Closed...!!!\n");
    return 0;
}

static ssize_t etx_read(struct file *filp, 
                char __user *buf, size_t count, loff_t *off)
{
    /*
    ** This function will be called when we read the Device file
    */ 
    char str[5];
    uint8_t sensor_datah = 0x00;
    uint8_t sensor_datal = 0x00;
    uint16_t sensor_data = 0;
    uint8_t lectura_reg= MPU6500_REG_FIFO;
    size_t datalen = 0;

    I2C_write(&lectura_reg, 1);
    I2C_read(&sensor_datah, 1);
    I2C_read(&sensor_datal, 1);

    sensor_data =(uint16_t) ((( sensor_datah & 0xFF) << 8) | sensor_datal);

    sprintf(str, "%d\n", sensor_data);
    datalen = strlen(str);
    if (count > datalen) {
        count = datalen;
    }
    if (copy_to_user(buf, str, count)) {
        return -EFAULT;
    }
    pr_info("Driver Read Function Called...!!!\n");
    pr_info("Driver Read Data %d ...!!!\n",sensor_data);
    return count;
}

static ssize_t etx_write(struct file *filp, 
                const char __user *buf, size_t len, loff_t *off)
{
    /*
    ** This function will be called when we write the Device file
    */ 
    pr_info("Writting device file...!!!\n");
    return 0;
}

/*******************************************************/
static int I2C_write(unsigned char *buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_send(etx_i2c_client_cap_sense, buf, len);
    
    return ret;
}

static int I2C_read(unsigned char *out_buf, unsigned int len)
{
    /*
    ** Sending Start condition, Slave address with R/W bit, 
    ** ACK/NACK and Stop condtions will be handled internally.
    */ 
    int ret = i2c_master_recv(etx_i2c_client_cap_sense, out_buf, len);
    
    return ret;
}

static void CAP1188_write(unsigned char reg, unsigned char data)
{
    unsigned char buf[2] = {0};
    int ret;
    buf[0] = reg;
    buf[1] = data;
    
    ret = I2C_write(buf, 2);
}

static int CAP1188_display_init(void)
{
    msleep(100);/*delay*/
 
    /*
    ** Commands to initialize the CAP1188
    */
    CAP1188_write(MPU6500_PWR_MGMT_1, 0x01);
	CAP1188_write(MPU6500_ACCEL_CONFIG1, 0x00);
	CAP1188_write(MPU6500_CONFIG, 0x00);
	CAP1188_write(MPU6500_SAMPLE_DIV, 0x07);
	CAP1188_write(MPU6500_USER_CTRL, 0x44);
	CAP1188_write(MPU6500_FIFO_EN, 0x08);
    
    return 0;
}

/*
** This function getting called when the slave has been found
** Note : This will be called only once when we load the driver.
*/
static int etx_cap_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    CAP1188_display_init();
 
    pr_info("cap1188 Probed!!!\n");
    
    return 0;
}

/*
** This function getting called when the slave has been removed
** Note : This will be called only once when we unload the driver.
*/
static int etx_cap_remove(struct i2c_client *client)
{   
    pr_info("cap1188 Removed!!!\n");
    return 0;
}

/*
** Structure that has slave device id
*/
static const struct i2c_device_id etx_cap_id[] = {
        { SLAVE_DEVICE_NAME, 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, etx_cap_id);

static struct i2c_driver etx_cap_driver = {
        .driver = {
            .name   = SLAVE_DEVICE_NAME,
            .owner  = THIS_MODULE,
        },
        .probe          = etx_cap_probe,
        .remove         = etx_cap_remove,
        .id_table       = etx_cap_id,
};
/*
** I2C Board Info strucutre
*/
static struct i2c_board_info cap_i2c_board_info = {
        I2C_BOARD_INFO(SLAVE_DEVICE_NAME, MPU6050_I2C_ADDR)
};

/*
** Module Exit function
*/
static void __exit etx_driver_exit(void)
{
    i2c_unregister_device(etx_i2c_client_cap_sense);
    i2c_del_driver(&etx_cap_driver);
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&etx_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("Driver Removed!!!\n");
}

static int __init etx_driver_init(void)
{
    int ret = -1;
    /*Allocating Major number*/
    if((alloc_chrdev_region(&dev, 0, 1, "etx_Dev")) <0){
        pr_err("Cannot allocate major number\n");
        return -1;
    }
    pr_info("Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&etx_cdev,&fops);

    /*Adding character device to the system*/
    if((cdev_add(&etx_cdev,dev,1)) < 0){
        pr_err("Cannot add the device to the system\n");
        return -1;
    }

    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"etx_class")) == NULL){
        pr_err("Cannot create the struct class\n");
        return -1;
    }

    /*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"etx_device")) == NULL){
        pr_err( "Cannot create the Device \n");
        return -1;
    }
    etx_i2c_adapter     = i2c_get_adapter(I2C_BUS_AVAILABLE);
    
    if( etx_i2c_adapter != NULL )
    {
        etx_i2c_client_cap_sense = i2c_new_client_device(etx_i2c_adapter, &cap_i2c_board_info);
        
        if( etx_i2c_client_cap_sense != NULL )
        {
            i2c_add_driver(&etx_cap_driver);
            ret = 0;
        }
        
        i2c_put_adapter(etx_i2c_adapter);
    }
    
    pr_info("Driver Added!!!\n");
    return ret;
}

module_init(etx_driver_init);
module_exit(etx_driver_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santiago Esteva <sesteva@fi.uba.ar>");
MODULE_DESCRIPTION("Mpu6500 i2c driver");
MODULE_VERSION("1:1.0");
