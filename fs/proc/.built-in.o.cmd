cmd_fs/proc/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o fs/proc/built-in.o fs/proc/proc.o ; scripts/mod/modpost fs/proc/built-in.o
