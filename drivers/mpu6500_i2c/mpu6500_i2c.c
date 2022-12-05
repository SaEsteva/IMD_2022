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

#define I2C_BUS				(1) /*I2C used to coenctc mpu sensor*/
#define DRV_NAME 			"mpu6500"
#define CLASS_NAME 			"mpu6500_class"
#define CLIENT_NAME 		"mpu6500_dev"
#define CLIENT_DEVICE 		"mpu6500_device"
#define CLIENT_DEVICE_NAME   "mpu_SENS"
 
static struct i2c_adapter *mpu_i2c_adapter     = NULL;  // I2C Adapter Structure
static struct i2c_client  *mpu_i2c_client_cap_sense = NULL;  // I2C Cient Structure (In our case it is CAP1188)
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev mpu_cdev;

struct mpu6500_data {
	uint16_t gyro_x;
	uint16_t gyro_y;
	uint16_t gyro_z;
	uint16_t accel_x;
	uint16_t accel_y;
	uint16_t accel_z;
	uint16_t temp;
};

static struct mpu6500_data mpuData;
static struct mpu6500_data *mpu_data = &mpuData;

/*************** Driver functions **********************/
static int mpu_open(struct inode *inode, struct file *file);
static int mpu_release(struct inode *inode, struct file *file);
static ssize_t mpu_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t mpu_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
static long mpu_ioctl(struct file *, unsigned int, unsigned long);
/******************************************************/
static void mpu6500_write(unsigned char reg, unsigned char data);
static int mpu_i2c_read(unsigned char *out_buf, unsigned int len);
static int mpu_i2c_write(unsigned char *buf, unsigned int len);
static int mpu_i2c_read_block(const char reg, char *buff);
static int mpu6500_accelXYZ_show(int axis);
//File operation structure 
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .unlocked_ioctl	= mpu_ioctl,
  .read           = mpu_read,
  .write          = mpu_write,
  .open           = mpu_open,
  .release        = mpu_release,
};
/********************************************************/

static int mpu_open(struct inode *inode, struct file *file)
{
    pr_info("Device File Opened...!!!\n");
    return 0;
}


static int mpu_release(struct inode *inode, struct file *file)
{
    pr_info("Device File Closed...!!!\n");
    return 0;
}

static ssize_t mpu_read(struct file *filp, 
                char __user *buf, size_t count, loff_t *off)
{
    char str[5];
    uint8_t sensor_datah = 0x00;
    uint8_t sensor_datal = 0x00;
    uint16_t sensor_data = 0;
    uint8_t lectura_reg= MPU6500_REG_FIFO;
    size_t datalen = 0;

    mpu_i2c_write(&lectura_reg, 1);
    mpu_i2c_read(&sensor_datah, 1);
    mpu_i2c_read(&sensor_datal, 1);

    sensor_data =(uint16_t) ((( sensor_datah & 0xFF) << 8) | sensor_datal);

    sprintf(str, "%d\n", sensor_data);
    datalen = strlen(str);
    if (count > datalen) {
        count = datalen;
    }
    if (copy_to_user(buf, str, count)) {
        return -EFAULT;
    }
    pr_info("[%s]: Driver Read Function Called...!!!\n",DRV_NAME);
    pr_info("[%s]: Driver Read Data %d ...!!!\n",DRV_NAME,sensor_data);
    return count;
}

static ssize_t mpu_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{
   	size_t datalen = 4;
   	size_t datanull = 0;
   	if(len>datanull){
		if(len<datalen){
			mpu6500_write(MPU6500_ACCEL_CONFIG1,len);
    		pr_info("[%s]:gain %d write \n", DRV_NAME,len);
		}
	}
	else{
		pr_info("[%s]:No gain write\n", DRV_NAME);
	}
	
   	pr_info("[%s]:Finish  writing ..\n", DRV_NAME);
    return 0;
}

static long mpu_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
   	void __user *argp = (void __user *)arg;
    int16_t data;

	pr_info("[%s]: Axis %d\t", DRV_NAME,cmd);
	data = mpu6500_accelXYZ_show(cmd);
    if (copy_to_user(argp, &data, sizeof(data))){
		pr_info("[%s]: Problem copy to user\n", DRV_NAME);
		return -EFAULT;
	}
    pr_info("[%s]: Aceleración %d\n", DRV_NAME,data);
	
	return 0;
}

/*******************************************************/
static int mpu_i2c_write(unsigned char *buf, unsigned int len)
{
    int ret = i2c_master_send(mpu_i2c_client_cap_sense, buf, len);
    
    return ret;
}

static int mpu_i2c_read(unsigned char *out_buf, unsigned int len)
{
    int ret = i2c_master_recv(mpu_i2c_client_cap_sense, out_buf, len);
    
    return ret;
}

static int mpu_i2c_read_block(const char reg, char *buff)
{
    int ret = i2c_smbus_read_i2c_block_data(mpu_i2c_client_cap_sense, reg, 2, buff);
    
    return ret;
}

static void mpu6500_write(unsigned char reg, unsigned char data)
{
    unsigned char buf[2] = {0};
    int ret;
    buf[0] = reg;
    buf[1] = data;
    
    ret = mpu_i2c_write(buf, 2);
	if (ret < 0)
		pr_info("[%s]: Failed to write data to the device! (reg = 0x%2x, data = 0x%2x)\n", DRV_NAME, reg, data);

}

static int mpu6500_read_sensor_data(const char reg, char *buff)
{
	int ret = 0;

	ret = mpu_i2c_read_block(reg,buff);
	if(ret < 0)
		pr_info("[%s]: Failed to read from device (reg = 0x%2x)!\n", DRV_NAME, reg);
	return ret;
}

static int mpu6500_sensor_init(void)
{
    msleep(100);

    mpu6500_write(MPU6500_PWR_MGMT_1, 0x01);
	mpu6500_write(MPU6500_ACCEL_CONFIG1, 0x00);
	mpu6500_write(MPU6500_CONFIG, 0x00);
	mpu6500_write(MPU6500_SAMPLE_DIV, 0x07);
	mpu6500_write(MPU6500_USER_CTRL, 0x44);
	mpu6500_write(MPU6500_FIFO_EN, 0x08);
    
    return 0;
}

static int mpu6500_accelXYZ_show(int axis)
{
	char buff[2] = {0x00};
	// char *message;
	int data;
	
	mpu_data->accel_x = 0;
	mpu_data->accel_y = 0;
	mpu_data->accel_z = 0;
	mpu_data->temp = 0;

	mpu6500_read_sensor_data(MPU6500_REG_FIFO, buff);
	mpu_data->accel_x = (mpu_data->accel_x | buff[0]) << 16;
	mpu_data->accel_x = (mpu_data->accel_x | buff[1]);
	mpu6500_read_sensor_data(MPU6500_REG_FIFO, buff);
	mpu_data->accel_y = (mpu_data->accel_y | buff[0]) << 16;
	mpu_data->accel_y = (mpu_data->accel_y | buff[1]);
	mpu6500_read_sensor_data(MPU6500_REG_FIFO, buff);
	mpu_data->accel_z = (mpu_data->accel_z | buff[0]) << 16;
	mpu_data->accel_z = (mpu_data->accel_z | buff[1]);
	mpu6500_read_sensor_data(MPU6500_REG_FIFO, buff);
	mpu_data->temp = (mpu_data->temp | buff[0]) << 16;
	mpu_data->temp = (mpu_data->temp | buff[1]);
	switch(axis){
		case 0:
			data = mpu_data->accel_x;
			break;
		case 1:
			data = mpu_data->accel_y;
			break;
		case 3:
			data = mpu_data->accel_z;
			break;
		case 4:
			data = mpu_data->temp;
			break;
		default:
			data = 0;
			break;
	}

    if((data & 0x8000) == 0x8000) { // negative number
		data = (int16_t) -(~data);
	} else {
		data = (int16_t) (data & 0x7fff);
	}

	return data;
}

static int mpu_cap_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    mpu6500_sensor_init();
 
    pr_info("[%s]: Probed!!!\n", DRV_NAME);
    
    return 0;
}

static int mpu_cap_remove(struct i2c_client *client)
{   
    pr_info("[%s]: Removed!!!\n", DRV_NAME);
    return 0;
}

static const struct i2c_device_id mpu_cap_id[] = {
        { CLIENT_DEVICE_NAME, 0 },
        { }
};
MODULE_DEVICE_TABLE(i2c, mpu_cap_id);

static struct i2c_driver mpu_cap_driver = {
        .driver = {
            .name   = CLIENT_DEVICE_NAME,
            .owner  = THIS_MODULE,
        },
        .probe          = mpu_cap_probe,
        .remove         = mpu_cap_remove,
        .id_table       = mpu_cap_id,
};

static struct i2c_board_info cap_i2c_board_info = {
        I2C_BOARD_INFO(CLIENT_DEVICE_NAME, MPU6050_I2C_ADDR)
};

static void __exit mpu_driver_exit(void)
{
    i2c_unregister_device(mpu_i2c_client_cap_sense);
    i2c_del_driver(&mpu_cap_driver);
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    cdev_del(&mpu_cdev);
    unregister_chrdev_region(dev, 1);
    pr_info("[%s]: Driver Removed!!!\n",DRV_NAME);
}

static int __init mpu_driver_init(void)
{
    int ret = -1;
    /*Allocating Major number*/
    if((alloc_chrdev_region(&dev, 0, 1, "mpu_Dev")) <0){
        pr_err("Cannot allocate major number\n");
        return -1;
    }
    pr_info("[%s]: Major = %d Minor = %d \n",DRV_NAME,MAJOR(dev), MINOR(dev));

    /*Creating cdev structure*/
    cdev_init(&mpu_cdev,&fops);

    /*Adding character device to the system*/
    if((cdev_add(&mpu_cdev,dev,1)) < 0){
        pr_err("[%s]: Cannot add the device to the system\n", DRV_NAME);
        return -1;
    }

    /*Creating struct class*/
    if((dev_class = class_create(THIS_MODULE,"mpu_class")) == NULL){
        pr_err("[%s]:Cannot create the struct class\n", DRV_NAME);
        return -1;
    }

    /*Creating device*/
    if((device_create(dev_class,NULL,dev,NULL,"mpu_device")) == NULL){
        pr_err( "[%s]: Cannot create the Device \n", DRV_NAME);
        return -1;
    }
    mpu_i2c_adapter     = i2c_get_adapter(I2C_BUS);
    
    if( mpu_i2c_adapter != NULL )
    {
        mpu_i2c_client_cap_sense = i2c_new_client_device(mpu_i2c_adapter, &cap_i2c_board_info);
        
        if( mpu_i2c_client_cap_sense != NULL )
        {
            i2c_add_driver(&mpu_cap_driver);
            ret = 0;
        }
        
        i2c_put_adapter(mpu_i2c_adapter);
    }
    
    pr_info("[%s]: Driver Added!!!\n", DRV_NAME);
    return ret;
}

module_init(mpu_driver_init);
module_exit(mpu_driver_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santiago Esteva <sesteva@fi.uba.ar>");
MODULE_DESCRIPTION("Mpu6500 i2c driver");
MODULE_VERSION("1:1.0");