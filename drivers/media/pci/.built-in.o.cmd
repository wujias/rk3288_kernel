cmd_drivers/media/pci/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/media/pci/built-in.o drivers/media/pci/ttpci/built-in.o drivers/media/pci/b2c2/built-in.o drivers/media/pci/pluto2/built-in.o drivers/media/pci/dm1105/built-in.o drivers/media/pci/pt1/built-in.o drivers/media/pci/mantis/built-in.o drivers/media/pci/ngene/built-in.o drivers/media/pci/ddbridge/built-in.o drivers/media/pci/saa7146/built-in.o ; scripts/mod/modpost drivers/media/pci/built-in.o
