cmd_drivers/input/keyboard/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/input/keyboard/built-in.o drivers/input/keyboard/rk_keys.o ; scripts/mod/modpost drivers/input/keyboard/built-in.o
