#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>

static int my_pdrv_probe (struct platform_device *pdev){
	pr_info("Hello! device probed!\n");
	return 0;
}

static int my_pdrv_remove(struct platform_device *pdev){
	pr_info("good bye reader! Removing me\n");
	return 0;
}

static struct platform_driver mypdrv = {
	.probe = my_pdrv_probe,
	.remove = my_pdrv_remove,
	.driver = {
		.name = "sesteva_driver",
		.owner = THIS_MODULE,
	},
};

static int __init my_pdrv_init(void) {
	pr_info("Howdy!\n");

	/* Registering with Kernel */
	platform_driver_register(&mypdrv);
	return 0;
}

static void __exit my_pdrv_exit (void) {
	pr_info("C'ya Pal. Exit and unregister\n");

	/* Unregistering from Kernel */
	platform_driver_unregister(&mypdrv);
}

module_init(my_pdrv_init);
module_exit(my_pdrv_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Santiago Esteva sesteva@fi.uba.ar");
MODULE_DESCRIPTION("My platform Hello World module");
