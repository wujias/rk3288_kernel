cmd_drivers/video/rockchip/hdmi/rockchip-hdmiv2/built-in.o :=  ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/video/rockchip/hdmi/rockchip-hdmiv2/built-in.o drivers/video/rockchip/hdmi/rockchip-hdmiv2/rockchip_hdmiv2.o drivers/video/rockchip/hdmi/rockchip-hdmiv2/rockchip_hdmiv2_hw.o drivers/video/rockchip/hdmi/rockchip-hdmiv2/rockchip_hdmiv2_cec.o drivers/video/rockchip/hdmi/rockchip-hdmiv2/rockchip_hdmiv2_hdcp.o ; scripts/mod/modpost drivers/video/rockchip/hdmi/rockchip-hdmiv2/built-in.o
