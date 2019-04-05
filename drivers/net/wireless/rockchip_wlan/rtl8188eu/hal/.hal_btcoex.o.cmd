cmd_drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/hal_btcoex.o := /home/mylinux/rk3288/myrk3128/kernel/scripts/gcc-wrapper.py ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-gcc -Wp,-MD,drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/.hal_btcoex.o.d  -nostdinc -isystem /home/mylinux/rk3288/myrk3128/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/../lib/gcc/arm-eabi/4.6.x-google/include -I/home/mylinux/rk3288/myrk3128/kernel/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/mylinux/rk3288/myrk3128/kernel/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/mylinux/rk3288/myrk3128/kernel/include/uapi -Iinclude/generated/uapi -include /home/mylinux/rk3288/myrk3128/kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -Wno-maybe-uninitialized -fno-dwarf2-cfi-asm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -fno-var-tracking-assignments -fno-inline-functions-called-once -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -O1 -Wno-unused-variable -Wno-unused-value -Wno-unused-label -Wno-unused-parameter -Wno-unused-function -Wno-unused -Idrivers/net/wireless/rockchip_wlan/rtl8188eu/include -Idrivers/net/wireless/rockchip_wlan/rtl8188eu/platform -DCONFIG_RTL8188E -DCONFIG_MP_INCLUDED -DCONFIG_POWER_SAVING -DCONFIG_TRAFFIC_PROTECT -DCONFIG_LOAD_PHY_PARA_FROM_FILE -DCONFIG_RTW_ADAPTIVITY_EN=0 -DCONFIG_RTW_ADAPTIVITY_MODE=0 -DCONFIG_LITTLE_ENDIAN -DCONFIG_PLATFORM_ANDROID -DCONFIG_MINIMAL_MEMORY_USAGE -DCONFIG_CONCURRENT_MODE -DCONFIG_IOCTL_CFG80211 -DRTW_USE_CFG80211_STA_EVENT -DCONFIG_P2P_IPS -DCONFIG_RADIO_WORK    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(hal_btcoex)"  -D"KBUILD_MODNAME=KBUILD_STR(8188eu)" -c -o drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/hal_btcoex.o drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/hal_btcoex.c

source_drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/hal_btcoex.o := drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/hal_btcoex.c

deps_drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/hal_btcoex.o := \
    $(wildcard include/config/bt/coexist.h) \
    $(wildcard include/config/lps/lclk.h) \
    $(wildcard include/config/concurrent/mode.h) \
    $(wildcard include/config/dualmac/concurrent.h) \
    $(wildcard include/config/p2p.h) \
    $(wildcard include/config/pci/hci.h) \
    $(wildcard include/config/usb/hci.h) \
    $(wildcard include/config/sdio/hci.h) \
    $(wildcard include/config/gspi/hci.h) \

drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/hal_btcoex.o: $(deps_drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/hal_btcoex.o)

$(deps_drivers/net/wireless/rockchip_wlan/rtl8188eu/hal/hal_btcoex.o):
