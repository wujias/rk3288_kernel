cmd_drivers/net/ethernet/rockchip/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/net/ethernet/rockchip/built-in.o drivers/net/ethernet/rockchip/eth_mac.o drivers/net/ethernet/rockchip/vmac/built-in.o drivers/net/ethernet/rockchip/gmac/built-in.o ; scripts/mod/modpost drivers/net/ethernet/rockchip/built-in.o