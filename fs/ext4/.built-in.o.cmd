cmd_fs/ext4/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o fs/ext4/built-in.o fs/ext4/ext4.o ; scripts/mod/modpost fs/ext4/built-in.o
