cmd_arch/arm/lib/strchr.o := /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/scripts/gcc-wrapper.py ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-gcc -Wp,-MD,arch/arm/lib/.strchr.o.d  -nostdinc -isystem /home/wujiasheng/andriodproject/RK3288_AOSP_D/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/../lib/gcc/arm-eabi/4.6.x-google/include -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/include/uapi -Iinclude/generated/uapi -include /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian  -D__ASSEMBLY__ -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a  -include asm/unified.h -msoft-float         -c -o arch/arm/lib/strchr.o arch/arm/lib/strchr.S

source_arch/arm/lib/strchr.o := arch/arm/lib/strchr.S

deps_arch/arm/lib/strchr.o := \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/asm/unified.h \
    $(wildcard include/config/arm/asm/unified.h) \
    $(wildcard include/config/thumb2/kernel.h) \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/asm/linkage.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/asm/assembler.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/cpu/feroceon.h) \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/arm/thumb.h) \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/uapi/asm/ptrace.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/asm/hwcap.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/uapi/asm/hwcap.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/asm/opcodes-virt.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/asm/opcodes.h \
    $(wildcard include/config/cpu/endian/be32.h) \

arch/arm/lib/strchr.o: $(deps_arch/arm/lib/strchr.o)

$(deps_arch/arm/lib/strchr.o):
