cmd_net/bluetooth/bnep/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o net/bluetooth/bnep/built-in.o net/bluetooth/bnep/bnep.o ; scripts/mod/modpost net/bluetooth/bnep/built-in.o
