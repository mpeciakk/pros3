# Configuration
BUILD_DIR = build
SRC_DIR = kernel/src

DISK_IMG = pros.img

MOUNT_POINT = /mnt/pros

QEMU = qemu-system-i386
QEMU_IMG = qemu-img

QEMU_FLAGS = -serial stdio -m 32 -no-reboot -no-shutdown

BOCHS = "/mnt/d/Program Files/Bochs-2.6.11/bochsdbg.exe"
BOCHS_FLAGS =

# Script related
LINKER_SCRIPT = kernel/linker.ld

SOURCES_CPP = $(shell find $(SRC_DIR)/ -name "*.cpp")
OBJECTS_CPP = $(SOURCES_CPP:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

SOURCES_S = $(shell find $(SRC_DIR)/ -name "*.S")
OBJECTS_S = $(SOURCES_S:$(SRC_DIR)/%.S=$(BUILD_DIR)/%.o)

INCLUDES = kernel/include
INCLUDE_PARAMS = $(INCLUDES:%=-I%)

GCC_OPTIONS = -m32 -c -no-pie $(INCLUDE_PARAMS) -Wno-write-strings -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -Wall -Wextra -fno-use-cxa-atexit -nostdlib -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore
AS_OPTIONS = --32
LD_OPTIONS = -m elf_i386 -Map=kmap.map

all: kernel

.SECONDEXPANSION:

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(@D)
	$(CXX) $(GCC_OPTIONS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.S
	mkdir -p $(@D)
	as $(AS_OPTIONS) -o $@ $^

kernel: $(OBJECTS_CPP) $(OBJECTS_S)
	ld $(LD_OPTIONS) -r $(OBJECTS_CPP) -o $(BUILD_DIR)/kernelc.o
	ld $(LD_OPTIONS) -r $(OBJECTS_S) -o $(BUILD_DIR)/kernels.o
	ld $(LD_OPTIONS) -T $(LINKER_SCRIPT) -o $(BUILD_DIR)/pros.bin $(BUILD_DIR)/kernelc.o $(BUILD_DIR)/kernels.o

iso: kernel
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $(BUILD_DIR)/pros.bin iso/boot/pros.bin
	echo 'set timeout=0'                      > iso/boot/grub/grub.cfg
	echo 'set default=0'                     >> iso/boot/grub/grub.cfg
	echo ''                                  >> iso/boot/grub/grub.cfg
	echo 'menuentry "pros" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/pros.bin'    	 >> iso/boot/grub/grub.cfg
	echo '  boot'                            >> iso/boot/grub/grub.cfg
	echo '}'                                 >> iso/boot/grub/grub.cfg
	grub-mkrescue -v --output=$(BUILD_DIR)/pros.iso iso
	rm -rf iso

mount:
	#sudo losetup -o 1048576 /dev/loop0 $(DISK_IMG)
	sudo mount -t vfat $(DISK_IMG) $(MOUNT_POINT) -o loop,rw,uid=1000,gid=1000

umount:
	@if mountpoint -q $(MOUNT_POINT); then\
		sync $(MOUNT_POINT);\
        sudo umount $(MOUNT_POINT);\
		#sudo losetup -d /dev/loop0;\
	fi

disk:
	$(QEMU_IMG) create $(DISK_IMG) 32M
	mkfs.vfat -F32 $(DISK_IMG)

run: clean kernel umount
	$(QEMU) $(QEMU_FLAGS) -kernel $(BUILD_DIR)/pros.bin -drive file=$(DISK_IMG)

runiso: clean umount iso
	$(QEMU) $(QEMU_FLAGS) $(BUILD_DIR)/pros.iso -drive file=$(DISK_IMG)

bochs: clean $(TARGET) iso umount
	$(BOCHS) -q -f bochsrc.txt

clean:
	rm -rf $(BUILD_DIR)

.PHONY: clean
