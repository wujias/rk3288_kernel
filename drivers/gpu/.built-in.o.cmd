cmd_drivers/gpu/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/gpu/built-in.o drivers/gpu/drm/built-in.o drivers/gpu/vga/built-in.o drivers/gpu/arm/built-in.o ; scripts/mod/modpost drivers/gpu/built-in.o