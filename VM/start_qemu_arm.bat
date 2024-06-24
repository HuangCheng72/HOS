qemu-system-arm -M virt -m 128M -monitor stdio -kernel u-boot_qemu_arm.elf -drive if=none,file=HOS.vhd,format=raw,id=hd -device virtio-blk-device,drive=hd
