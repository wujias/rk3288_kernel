cmd_drivers/scsi/scsi_tgt.o := ../prebuilts/gcc/linux-x86/arm/arm-eabi-4.6/bin/arm-eabi-ld -EL    -r -o drivers/scsi/scsi_tgt.o drivers/scsi/scsi_tgt_lib.o drivers/scsi/scsi_tgt_if.o ; scripts/mod/modpost drivers/scsi/scsi_tgt.o