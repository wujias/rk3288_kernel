cmd_arch/arm/boot/dts/rk3288-tb_8846.dtb := /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/scripts/gcc-wrapper.py ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-gcc -E -Wp,-MD,arch/arm/boot/dts/.rk3288-tb_8846.dtb.d.pre.tmp -nostdinc -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/include -undef -D__DTS__ -x assembler-with-cpp -o arch/arm/boot/dts/.rk3288-tb_8846.dtb.dts arch/arm/boot/dts/rk3288-tb_8846.dts ; /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/scripts/dtc/dtc -O dtb -o arch/arm/boot/dts/rk3288-tb_8846.dtb -b 0 -i arch/arm/boot/dts/  -d arch/arm/boot/dts/.rk3288-tb_8846.dtb.d.dtc.tmp arch/arm/boot/dts/.rk3288-tb_8846.dtb.dts ; cat arch/arm/boot/dts/.rk3288-tb_8846.dtb.d.pre.tmp arch/arm/boot/dts/.rk3288-tb_8846.dtb.d.dtc.tmp > arch/arm/boot/dts/.rk3288-tb_8846.dtb.d

source_arch/arm/boot/dts/rk3288-tb_8846.dtb := arch/arm/boot/dts/rk3288-tb_8846.dts

deps_arch/arm/boot/dts/rk3288-tb_8846.dtb := \
  arch/arm/boot/dts/rk3288.dtsi \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/clock/rk_system_status.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/interrupt-controller/arm-gic.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/interrupt-controller/irq.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/rkfb/rk_fb.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/rkmipi/mipi_dsi.h \
    $(wildcard include/config/mipi/dsi/ft.h) \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/suspend/rockchip-pm.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/sensor-dev.h \
  arch/arm/boot/dts/skeleton.dtsi \
  arch/arm/boot/dts/rk3288-pinctrl.dtsi \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/gpio/gpio.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/pinctrl/rockchip.h \
    $(wildcard include/config/to/pull.h) \
    $(wildcard include/config/to/vol.h) \
    $(wildcard include/config/to/drv.h) \
    $(wildcard include/config/to/tri.h) \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/pinctrl/rockchip-rk3288.h \
  arch/arm/boot/dts/rk3288-clocks.dtsi \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/clock/rockchip,rk3288.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/boot/dts/include/dt-bindings/clock/rockchip.h \
  arch/arm/boot/dts/lcd-box.dtsi \
  arch/arm/boot/dts/vtl_ts_sdk8846.dtsi \
  arch/arm/boot/dts/rk3288-cif-sensor.dtsi \
  arch/arm/boot/dts/../../mach-rockchip/rk_camera_sensor_info.h \
    $(wildcard include/config/soc/camera/ov5642/interpolation/8m.h) \
    $(wildcard include/config/soc/camera/gc0308/interpolation/5m.h) \
    $(wildcard include/config/soc/camera/gc0308/interpolation/3m.h) \
    $(wildcard include/config/soc/camera/gc0308/interpolation/2m.h) \
    $(wildcard include/config/soc/camera/hi253/interpolation/5m.h) \
    $(wildcard include/config/soc/camera/hi253/interpolation/3m.h) \
  arch/arm/boot/dts/act8846.dtsi \

arch/arm/boot/dts/rk3288-tb_8846.dtb: $(deps_arch/arm/boot/dts/rk3288-tb_8846.dtb)

$(deps_arch/arm/boot/dts/rk3288-tb_8846.dtb):
