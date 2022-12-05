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
    int data,ret,cont;
    float value;
    cont = 0;
    while(1){
        if (my_dev < 0) {
            perror("Fail to open device file: /dev/mpu_device.");
            return -1;
        } else {
            ret= ioctl(my_dev, 0, &data);
            value = ((float) data)/ACELLSCALE;
            printf("Se extrae la medición en x %.4f\n",value);
            usleep(STOP);
            ret= ioctl(my_dev, 1, &data);
            value = ((float) data)/ACELLSCALE;
            printf("Se extrae la medición en y %.4f\n",value);
            usleep(STOP);
            ret= ioctl(my_dev, 3, &data);
            value = ((float) data)/ACELLSCALE;
            printf("Se extrae la medición en z %.4f\n",value);
            usleep(STOP);
            ret= ioctl(my_dev, 4, &data);
            value = (float)data/TEMPSCALE + TEMPOFFSET;
            printf("Se extrae la medición de temperatura %.3f\n",value);
            usleep(STOP);
        }
        if(cont++>NUM_CYCLE) break;
    }
    close(my_dev);
return 0;
}


