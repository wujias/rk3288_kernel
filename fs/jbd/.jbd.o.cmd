cmd_fs/jbd/jbd.o := ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o fs/jbd/jbd.o fs/jbd/transaction.o fs/jbd/commit.o fs/jbd/recovery.o fs/jbd/checkpoint.o fs/jbd/revoke.o fs/jbd/journal.o ; scripts/mod/modpost fs/jbd/jbd.o
