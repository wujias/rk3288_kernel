cmd_drivers/power/reset/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/power/reset/built-in.o drivers/power/reset/gpio-poweroff.o ; scripts/mod/modpost drivers/power/reset/built-in.o
