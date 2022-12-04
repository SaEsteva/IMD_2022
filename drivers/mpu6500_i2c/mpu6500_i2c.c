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

#define I2C_BUS					1 /*I2C used to coenctc mpu sensor*/
#define DRV_NAME "mpu6500"



struct mpu6500_data {
	struct i2c_adapter *i2c_adapter;
	struct i2c_client *client;
	struct class *dev_class;
	struct cdev client_cdev;
	dev_t dev;
	uint16_t gyro_x;
	uint16_t gyro_y;
	uint16_t gyro_z;
	uint16_t accel_x;
	uint16_t accel_y;
	uint16_t accel_z;
};

static struct mpu6500_data mpuData;
static struct mpu6500_data *mpu_data = &mpuData;


/*************** Driver functions **********************/
static int clt_open(struct inode *inode, struct file *file);
static int clt_release(struct inode *inode, struct file *file);
static ssize_t clt_read(struct file *filp, 
                char __user *buf, size_t len,loff_t * off);
static ssize_t clt_write(struct file *filp, 
                const char *buf, size_t len, loff_t * off);
static int mcp_i2c_write(const char *data, const char ndata);
static int mcp_i2c_read(unsigned char *data_read, unsigned int len);
static int mcp_i2c_read_block(const char reg, char *buff);
static int mpu6500_write(const char reg, const char data);
static int mpu6500_read(const char reg);
static int mpu6500_read_sensor_data(const char reg, char *buff);
static int mpu6500_chip_init(void);
static ssize_t mpu6500_gyroXYZ_show(void);
static ssize_t mpu6500_accelXYZ_show(void);
static int mpu6500_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int mpu6500_remove(struct i2c_client *client);
static int __init mpu6500_init(void);
static void __exit mpu6500_exit(void);
/******************************************************/

//File operation structure 
static struct file_operations fops =
{
  .owner          = THIS_MODULE,
  .read           = clt_read,
  .write          = clt_write,
  .open           = clt_open,
  .release        = clt_release,
};

static const struct i2c_device_id mpu6500_id[] = {
	{DRV_NAME, 0}, 
	{}
};

MODULE_DEVICE_TABLE(i2c, mpu6500_id);
/******************** Funcitons ***********************/
static int clt_open(struct inode *inode, struct file *file)
{
    /*
    ** This function will be called when we open the Device file
    */ 
    pr_info("[%s]:Device File Opened...!!!\n", DRV_NAME);
    return 0;
}


static int clt_release(struct inode *inode, struct file *file)
{
    /*
    ** This function will be called when we close the Device file
    */
    pr_info("[%s]:Device File Closed...!!!\n", DRV_NAME);
    return 0;
}

static ssize_t clt_read(struct file * pf, char __user * buff, size_t len, loff_t *loff)
{
    /*
    ** This function will be called when we read the Device file
    */ 
   	ssize_t buffer;

    buffer = mpu6500_accelXYZ_show();
    pr_info("[%s]:Aceleración %d\n", DRV_NAME,buffer);
    pr_info("[%s]:Driver Read Data ...!!!\n", DRV_NAME);
    return 0;
}

static ssize_t clt_write(struct file *filp, const char *buf, size_t len, loff_t * off)
{    /*
    ** This function will be called when we write the Device file
    */
   	size_t datalen = 4;
   	size_t datanull = 0;
   	if(len>datanull){
		if(len<datalen){
			mpu6500_write(MPU6500_ACCEL_CONFIG1,len);
    		pr_info("[%s]:[%d] gan write \n", DRV_NAME,len);
		}
	}
	else{
		pr_info("[%s]:No gain write\n", DRV_NAME);
	}
	
   	pr_info("[%s]:Finish  writing ..\n", DRV_NAME);
    return 0;
}
/********************************************************/

static int mcp_i2c_write(const char *data, const char ndata)
{
    int ret = i2c_master_send(mpu_data->client, data, ndata);
    
	return ret;
}

static int mcp_i2c_read(unsigned char *data_read, unsigned int len)
{
    int ret = i2c_master_recv(mpu_data->client, data_read,len);
    
    return ret;
}

static int mcp_i2c_read_block(const char reg, char *buff)
{
    int ret = i2c_smbus_read_i2c_block_data(mpu_data->client, reg, 2, buff);
    
    return ret;
}


static int mpu6500_write(const char reg, const char data) {
	int ret = 0;

	unsigned char data_write[2] = {0};
    data_write[0] = reg;
    data_write[1] = data;

	ret = mcp_i2c_write(data_write,2);

	if (ret < 0)
		pr_info("[%s]: Failed to write data to the device! (reg = 0x%2x, data = 0x%2x)\n", DRV_NAME, reg, data);

	return ret;
}

static int mpu6500_read(const char reg)
{
	int ret = 0;

	unsigned char data_read[2] = {0};

	ret = mcp_i2c_read(data_read,2);
	if (ret < 0)
		pr_info("[%s]: Failed to read from device (reg = 0x%2x)!\n", DRV_NAME, reg);

	return ret;
}

static int mpu6500_read_sensor_data(const char reg, char *buff)
{
	int ret = 0;

	ret = mcp_i2c_read_block(reg,buff);
	if(ret < 0)
		pr_info("[%s]: Failed to read from device (reg = 0x%2x)!\n", DRV_NAME, reg);

	return ret;
}

static int mpu6500_chip_init(void)
{
	int ret = 0;

	ret = mpu6500_write(MPU6500_PWR_MGMT_1, 0x01);
	if (ret < 0)
		return -EINVAL;
	ret = mpu6500_write(MPU6500_SAMPLE_DIV, 0x07);
	if (ret < 0)
		return -EINVAL;
	ret = mpu6500_write(MPU6500_CONFIG, 0x06);
	if (ret < 0)
		return -EINVAL;
	ret = mpu6500_write(MPU6500_GYRO_CONFIG, 0x10);
	if (ret < 0)
		return -EINVAL;
	ret = mpu6500_write(MPU6500_ACCEL_CONFIG1, 0x01);
	if (ret < 0)
		return -EINVAL;

	pr_info("[%s]: correct init...\n", DRV_NAME);
	return 0;
}

static ssize_t mpu6500_gyroXYZ_show(void)
{
	char buff[2] = {0x00};
	char *message;

	mpu_data->gyro_x = 0;
	mpu_data->gyro_y = 0;
	mpu_data->gyro_z = 0;
	
	mpu6500_read_sensor_data(MPU6500_GYRO_XOUT, buff);
	mpu_data->gyro_x = (mpu_data->gyro_x | buff[0]) << 16;
	mpu_data->gyro_x = (mpu_data->gyro_x | buff[1]);
	mpu6500_read_sensor_data(MPU6500_GYRO_YOUT, buff);
	mpu_data->gyro_y = (mpu_data->gyro_y | buff[0]) << 16;
	mpu_data->gyro_y = (mpu_data->gyro_y | buff[1]);
	mpu6500_read_sensor_data(MPU6500_GYRO_ZOUT, buff);
	mpu_data->gyro_z = (mpu_data->gyro_z | buff[0]) << 16;
	mpu_data->gyro_z = (mpu_data->gyro_z | buff[1]);

	return sprintf(message, "%d (0x%x) %d (0x%x) %d(0x%x)", mpu_data->gyro_x, mpu_data->gyro_x, mpu_data->gyro_y, mpu_data->gyro_y, mpu_data->gyro_z, mpu_data->gyro_z);
}

static ssize_t mpu6500_accelXYZ_show(void)
{
	char buff[2] = {0x00};
	char *message;
	
	mpu_data->accel_x = 0;
	mpu_data->accel_y = 0;
	mpu_data->accel_z = 0;

	mpu6500_read_sensor_data(MPU6500_ACCEL_XOUT, buff);
	mpu_data->accel_x = (mpu_data->accel_x | buff[0]) << 16;
	mpu_data->accel_x = (mpu_data->accel_x | buff[1]);
	mpu6500_read_sensor_data(MPU6500_ACCEL_YOUT, buff);
	mpu_data->accel_y = (mpu_data->accel_y | buff[0]) << 16;
	mpu_data->accel_y = (mpu_data->accel_y | buff[1]);
	mpu6500_read_sensor_data(MPU6500_ACCEL_ZOUT, buff);
	mpu_data->accel_z = (mpu_data->accel_z | buff[0]) << 16;
	mpu_data->accel_z = (mpu_data->accel_z | buff[1]);
	return sprintf(message, "%d (0x%x) %d (0x%x) %d (0x%x)", mpu_data->accel_x, mpu_data->accel_x, mpu_data->accel_y, mpu_data->accel_y, mpu_data->accel_z, mpu_data->accel_z);
}



static int mpu6500_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int ret = 0;

	mpu6500_chip_init();

	pr_info("[%s]: probe successful!\n", DRV_NAME);

	return 0;
}

static int mpu6500_remove(struct i2c_client *client)
{
	pr_info("[%s]: removed\n", DRV_NAME);

	return 0;
}

static struct i2c_driver mpu6500_drv = {
	.probe = mpu6500_probe,
	.remove = mpu6500_remove,
	.driver = {
		.name = DRV_NAME,
		.owner = THIS_MODULE,
	},
	.id_table = mpu6500_id,
};
/*
** I2C Board Info strucutre
*/
static struct i2c_board_info i2c_mpu6500_board_info = {
        I2C_BOARD_INFO(DRV_NAME, MPU6050_I2C_ADDR)
};

static int __init mpu6500_init(void)
{
	int ret = 0;

	/*Allocating Major number*/
    if((alloc_chrdev_region(&(mpu_data->dev), 0, 1, "mpu6500_dev")) <0){
        pr_err("[%s]:Cannot allocate major number\n", DRV_NAME);
        return -1;
    }
    pr_info("[%s]:Major = %d Minor = %d \n",MAJOR(mpu_data->dev), MINOR(mpu_data->dev), DRV_NAME);

    /*Creating cdev structure*/
    cdev_init(&(mpu_data->client_cdev),&fops);

    /*Adding character device to the system*/
    if((cdev_add(&(mpu_data->client_cdev),mpu_data->dev,0)) < 0){
        pr_err("[%s]:Cannot add the device to the system\n", DRV_NAME);
        return -1;
    }

    /*Creating struct class*/
    if((mpu_data->dev_class = class_create(THIS_MODULE,"mpu6500_class")) == NULL){
        pr_err("[%s]:Cannot create the struct class\n", DRV_NAME);
        return -1;
    }

    /*Creating device*/
    if((device_create(mpu_data->dev_class,NULL,mpu_data->dev,NULL,"mpu6500")) == NULL){
        pr_err( "[%s]:Cannot create the Device \n", DRV_NAME);
        return -1;
    }
    mpu_data->i2c_adapter     = i2c_get_adapter(I2C_BUS);
    
    if( mpu_data->i2c_adapter  != NULL )
    {
        mpu_data->client = i2c_new_client_device(mpu_data->i2c_adapter, &i2c_mpu6500_board_info);
        
        if( mpu_data->client != NULL )
        {
            ret = i2c_add_driver(&mpu6500_drv);

			if (ret < 0) {
				pr_info("[%s]: Failed to register driver!\n", DRV_NAME);
				return -1;
			}
        }
        
        i2c_put_adapter(mpu_data->i2c_adapter);
    }
	
	pr_info("[%s] driver initialized !\n",DRV_NAME);    
	return ret;
}

static void __exit mpu6500_exit(void)
{
	i2c_unregister_device(mpu_data->client);
    i2c_del_driver(&mpu6500_drv);
    device_destroy(mpu_data->dev_class,mpu_data->dev);
    class_destroy(mpu_data->dev_class);
    cdev_del(&(mpu_data->client_cdev));
    unregister_chrdev_region(mpu_data->dev, 1);
	pr_info("[%s]: exit\n", DRV_NAME);
}

module_init(mpu6500_init);
module_exit(mpu6500_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santiago Esteva <sesteva@fi.uba.ar>");
MODULE_DESCRIPTION("Mpu6500 i2c driver");
MODULE_VERSION("1:1.0");
