cmd_block/partitions/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o block/partitions/built-in.o block/partitions/check.o block/partitions/msdos.o block/partitions/efi.o block/partitions/rk.o ; scripts/mod/modpost block/partitions/built-in.o