cmd_drivers/mmc/host/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/mmc/host/built-in.o drivers/mmc/host/rk_sdmmc.o drivers/mmc/host/dw_mmc-pltfm.o drivers/mmc/host/dw_mmc-rockchip.o drivers/mmc/host/rk_sdmmc_dbg.o drivers/mmc/host/rk_sdmmc_ops.o ; scripts/mod/modpost drivers/mmc/host/built-in.o