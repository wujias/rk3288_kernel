cmd_fs/fat/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o fs/fat/built-in.o fs/fat/fat.o fs/fat/vfat.o fs/fat/msdos.o ; scripts/mod/modpost fs/fat/built-in.o
