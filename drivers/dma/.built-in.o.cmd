cmd_drivers/dma/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/dma/built-in.o drivers/dma/dmaengine.o drivers/dma/of-dma.o drivers/dma/pl330.o ; scripts/mod/modpost drivers/dma/built-in.o
