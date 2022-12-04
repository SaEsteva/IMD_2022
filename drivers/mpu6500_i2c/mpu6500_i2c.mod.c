#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x90f75b7e, "module_layout" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xcce0699e, "cdev_del" },
	{ 0x9ab547c5, "class_destroy" },
	{ 0x97877479, "device_destroy" },
	{ 0xf4627308, "i2c_del_driver" },
	{ 0x7ff0b64e, "i2c_unregister_device" },
	{ 0xe363c573, "i2c_put_adapter" },
	{ 0x63413d1d, "i2c_register_driver" },
	{ 0x63995365, "i2c_new_client_device" },
	{ 0xb910c73f, "i2c_get_adapter" },
	{ 0x42d9689d, "device_create" },
	{ 0xcfffe6ec, "__class_create" },
	{ 0x1d01e681, "cdev_add" },
	{ 0xf37beb9b, "cdev_init" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x3c3ff9fd, "sprintf" },
	{ 0x6b61754e, "i2c_smbus_read_i2c_block_data" },
	{ 0x8f678b07, "__stack_chk_guard" },
	{ 0x3ea1b6e4, "__stack_chk_fail" },
	{ 0x65483223, "i2c_transfer_buffer_flags" },
	{ 0x92997ed8, "_printk" },
	{ 0xb1ad28e0, "__gnu_mcount_nc" },
};

MODULE_INFO(depends, "");

MODULE_ALIAS("i2c:mpu6500");

MODULE_INFO(srcversion, "A79E8C5F7374119FE7A1C77");
