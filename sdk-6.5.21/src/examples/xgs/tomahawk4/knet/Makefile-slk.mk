# User must select one platform from below.By default ARM_LINUX is selected. .
ifeq (,$(BUILD_PLATFORM))
BUILD_PLATFORM=ARM_LINUX
endif

# Toolchain base directory for NS2 XMC card
ifeq (BE,$(ENDIAN_MODE))
TOOLCHAIN_BASE_DIR ?= /projects/ntsw-tools/toolchains/slk/linaro-be
TARGET_ARCHITECTURE:=aarch64_be-linux-gnu
KERNDIR ?= /projects/ntsw-tools/linux/iproc_ldks/slk-be/poky/brcm-released-source/git
else
TOOLCHAIN_BASE_DIR ?= /projects/ntsw-tools/toolchains/slk/linaro-le
# Compiler for target architecture
TARGET_ARCHITECTURE:= aarch64-linux-gnu
# Kernel directory
KERNDIR ?= /projects/ntsw-tools/linux/iproc_ldks/slk/poky/brcm-released-source/git
endif

ifeq (,$(CROSS_COMPILE))
CROSS_COMPILE:= $(TARGET_ARCHITECTURE)-
endif

# armtools
TOOLCHAIN_BIN_DIR=$(TOOLCHAIN_BASE_DIR)/bin
override PATH:=$(TOOLCHAIN_BIN_DIR):$(PATH)

CC=$(TARGET_ARCHITECTURE)-gcc
CFLAGS=-Wall -O2
LDFLAGS=

all: th4_hsdk_knet_rx-slk th4_hsdk_knet_tx-slk
	touch all_slk

th4_hsdk_knet_rx-slk: th4_hsdk_knet_rx.c
	$(CC) $(CFLAGS) -o th4_hsdk_knet_rx-slk th4_hsdk_knet_rx.c $(LDFLAGS)

th4_hsdk_knet_tx-slk: th4_hsdk_knet_tx.c
	$(CC) $(CFLAGS) -o th4_hsdk_knet_tx-slk th4_hsdk_knet_tx.c $(LDFLAGS)
