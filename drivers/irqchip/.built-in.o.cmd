cmd_drivers/irqchip/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/irqchip/built-in.o drivers/irqchip/irqchip.o drivers/irqchip/irq-gic.o drivers/irqchip/irq-gic-common.o ; scripts/mod/modpost drivers/irqchip/built-in.o
