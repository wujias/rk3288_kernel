cmd_fs/jbd2/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o fs/jbd2/built-in.o fs/jbd2/jbd2.o ; scripts/mod/modpost fs/jbd2/built-in.o
