cmd_drivers/net/wireless/rockchip_wlan/rtl8188eu/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    --strip-debug -r -o drivers/net/wireless/rockchip_wlan/rtl8188eu/built-in.o drivers/net/wireless/rockchip_wlan/rtl8188eu/8188eu.o ; scripts/mod/modpost drivers/net/wireless/rockchip_wlan/rtl8188eu/built-in.o