cmd_drivers/input/sensors/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/input/sensors/built-in.o drivers/input/sensors/accel/built-in.o drivers/input/sensors/lsensor/built-in.o drivers/input/sensors/sensor-i2c.o drivers/input/sensors/sensor-dev.o ; scripts/mod/modpost drivers/input/sensors/built-in.o