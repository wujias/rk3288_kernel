cmd_drivers/gpu/arm/mali400/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/gpu/arm/mali400/built-in.o drivers/gpu/arm/mali400/mali/built-in.o drivers/gpu/arm/mali400/ump/built-in.o ; scripts/mod/modpost drivers/gpu/arm/mali400/built-in.o
