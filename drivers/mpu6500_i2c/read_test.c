#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
int my_dev = open("/dev/mpu6500", 0);
if (my_dev < 0) {
		
 perror("Fail to open device file: /dev/mpu6500.");
} else {
 
    printf(read(my_dev, 0, 0));
		
    close(my_dev);
}
return 0;
}


