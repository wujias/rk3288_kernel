cmd_drivers/usb/dwc_otg_310/usbdev_rk3368.o := /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/scripts/gcc-wrapper.py ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-gcc -Wp,-MD,drivers/usb/dwc_otg_310/.usbdev_rk3368.o.d  -nostdinc -isystem /home/wujiasheng/andriodproject/RK3288_AOSP_D/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/../lib/gcc/arm-eabi/4.6.x-google/include -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/include/uapi -Iinclude/generated/uapi -include /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -Wno-maybe-uninitialized -fno-dwarf2-cfi-asm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -fno-var-tracking-assignments -fno-inline-functions-called-once -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Dlinux -DDWC_HS_ELECT_TST -DDWC_LINUX -DLM_INTERFACE    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(usbdev_rk3368)"  -D"KBUILD_MODNAME=KBUILD_STR(dwc_otg)" -c -o drivers/usb/dwc_otg_310/usbdev_rk3368.o drivers/usb/dwc_otg_310/usbdev_rk3368.c

source_drivers/usb/dwc_otg_310/usbdev_rk3368.o := drivers/usb/dwc_otg_310/usbdev_rk3368.c

deps_drivers/usb/dwc_otg_310/usbdev_rk3368.o := \
    $(wildcard include/config/arm64.h) \
    $(wildcard include/config/usb20/otg.h) \
    $(wildcard include/config/rk/usb/uart.h) \
    $(wildcard include/config/usb/ehci/rk.h) \
    $(wildcard include/config/of.h) \

drivers/usb/dwc_otg_310/usbdev_rk3368.o: $(deps_drivers/usb/dwc_otg_310/usbdev_rk3368.o)

$(deps_drivers/usb/dwc_otg_310/usbdev_rk3368.o):
