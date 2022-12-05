# Driver MPU6500 

Primero se debe compilar el driver ejecutando en la consula de las raspberry:
```
rpisesteva@raspberrypi:~/Documents/IMD/mpu6500_i2c $ make
```
En caso de compilarse sin errores de debe insertar el módulo creado ejecutando:
```
rpisesteva@raspberrypi:~/Documents/IMD/mpu6500_i2c $ sudo insmod mpu6500_i2c.ko
```
Esto inicializa el módulo, para verificar que no hubo errores de pueden ver los mensajes ejecutando 
```
rpisesteva@raspberrypi:~/Documents/IMD/mpu6500_i2c $ dmesg
[17704.765523] [mpu6500]: Major = 238 Minor = 0
[17704.884742] [mpu6500]: Probed!!!
[17704.884993] [mpu6500]: Driver Added!!!
```
En este punto ya se pueden probar los programas de escritura y lectura. En particular se tiene:
```
rpisesteva@raspberrypi:~/Documents/IMD/mpu6500_i2c $ sudo ./ioctl_test.out 
Se extrae la medición en x 0.0023
Se extrae la medición en y 0.0021
Se extrae la medición en z 0.0098
Se extrae la medición de temperatura 36.636
Se extrae la medición en x 0.0023
Se extrae la medición en y 0.0022
Se extrae la medición en z 0.0021
Se extrae la medición de temperatura 36.636
```
La cual lee e imprime 20 mensajes, en caso de queres analziar los mesajes del kernel se puede ejecutar nuevamente:
```
rpisesteva@raspberrypi:~/Documents/IMD/mpu6500_i2c $ dmesg 
[17778.778687] Device File Opened...!!!
[17778.778724] [mpu6500]: Axis 0
[17778.782131] [mpu6500]: Aceleración 37
[17778.982953] [mpu6500]: Axis 1
[17778.986311] [mpu6500]: Aceleración 35
[17779.186534] [mpu6500]: Axis 3
[17779.189887] [mpu6500]: Aceleración 160
[17779.390070] [mpu6500]: Axis 4
[17779.393438] [mpu6500]: Aceleración 36
```

Por ultimo se debe remover el módulo creado:
```
rpisesteva@raspberrypi:~/Documents/IMD/mpu6500_i2c $ sudo rmmod mpu6500_i2c 
```