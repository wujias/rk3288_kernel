cmd_drivers/rtc/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/rtc/built-in.o drivers/rtc/rtc-lib.o drivers/rtc/hctosys.o drivers/rtc/systohc.o drivers/rtc/rtc-core.o drivers/rtc/rtc-pcf8563.o drivers/rtc/rtc-HYM8563.o drivers/rtc/rtc-ricoh619.o ; scripts/mod/modpost drivers/rtc/built-in.o