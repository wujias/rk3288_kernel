cmd_ipc/syscall.o := /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/scripts/gcc-wrapper.py ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-gcc -Wp,-MD,ipc/.syscall.o.d  -nostdinc -isystem /home/wujiasheng/andriodproject/RK3288_AOSP_D/prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/../lib/gcc/arm-eabi/4.6.x-google/include -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include -Iarch/arm/include/generated  -Iinclude -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/uapi -Iarch/arm/include/generated/uapi -I/home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/include/uapi -Iinclude/generated/uapi -include /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/include/linux/kconfig.h -D__KERNEL__ -mlittle-endian -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -Wno-maybe-uninitialized -fno-dwarf2-cfi-asm -mabi=aapcs-linux -mno-thumb-interwork -funwind-tables -marm -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -Wframe-larger-than=1024 -fno-stack-protector -Wno-unused-but-set-variable -fomit-frame-pointer -fno-var-tracking-assignments -fno-inline-functions-called-once -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(syscall)"  -D"KBUILD_MODNAME=KBUILD_STR(syscall)" -c -o ipc/syscall.o ipc/syscall.c

source_ipc/syscall.o := ipc/syscall.c

deps_ipc/syscall.o := \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/include/uapi/linux/unistd.h \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/asm/unistd.h \
    $(wildcard include/config/aeabi.h) \
    $(wildcard include/config/oabi/compat.h) \
  /home/wujiasheng/andriodproject/RK3288_AOSP_D/kernel/arch/arm/include/uapi/asm/unistd.h \

ipc/syscall.o: $(deps_ipc/syscall.o)

$(deps_ipc/syscall.o):
