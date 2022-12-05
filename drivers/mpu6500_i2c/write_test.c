#include <stdio.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#define NUM_CYCLE 20
#define STOP 200000
#define ACELLSCALE 16384
#define TEMPSCALE 340
#define TEMPOFFSET 36.53

int main(void)
{
    int my_dev = open("/dev/mpu_device",0);
    int gain,ret;
    gain = 0;
    if (my_dev < 0) {
        perror("Fail to open device file: /dev/mpu_device.");
    } else {
        ret= write(my_dev,NULL,gain);
        printf("Se graba la ganancia %d\n",gain);
        usleep(STOP);
    }
    close(my_dev);
    return 0;
}


