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
    int my_dev = open("/dev/etx_device",0);
    int data,ret,cont;
    float value;
    char *m;
    cont = 0;
    while(1){
        if (my_dev < 0) {
            perror("Fail to open device file: /dev/etx_device.");
        } else {
            ret= ioctl(my_dev, 0, &data);
            printf("Se extrae la medición de temperatura %s\n",data);
            usleep(STOP);
        }
        if(cont++>NUM_CYCLE) break;
    }
    close(my_dev);
return 0;
}


