cmd_fs/notify/dnotify/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o fs/notify/dnotify/built-in.o fs/notify/dnotify/dnotify.o ; scripts/mod/modpost fs/notify/dnotify/built-in.o