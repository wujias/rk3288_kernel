cmd_drivers/input/remotectl/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/input/remotectl/built-in.o drivers/input/remotectl/rockchip_pwm_remotectl.o ; scripts/mod/modpost drivers/input/remotectl/built-in.o