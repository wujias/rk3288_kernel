cmd_fs/ramfs/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o fs/ramfs/built-in.o fs/ramfs/ramfs.o ; scripts/mod/modpost fs/ramfs/built-in.o
