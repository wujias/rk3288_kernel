cmd_drivers/media/media.o := ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/media/media.o drivers/media/media-device.o drivers/media/media-devnode.o drivers/media/media-entity.o ; scripts/mod/modpost drivers/media/media.o