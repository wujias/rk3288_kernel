cmd_kernel/cpu/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o kernel/cpu/built-in.o kernel/cpu/idle.o ; scripts/mod/modpost kernel/cpu/built-in.o
