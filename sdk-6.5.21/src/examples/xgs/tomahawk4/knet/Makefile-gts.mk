ifeq (,$(BUILD_PLATFORM))
BUILD_PLATFORM=FEDORA_LINUX
endif

# Define some basic path variables for tools and kernel source, etc.
export GTS_TOOLS_BASE = /projects/ntsw-tools
TOOLCHAIN_DIR = $(GTS_TOOLS_BASE)/toolchains/x86_64/xlr

# Set up tools dir variables. #
export TOOLS_DIR := $(TOOLCHAIN_DIR)/bin

# GTS toolchain
override PATH := $(TOOLSDIR):$(PATH)

CC=gcc
CFLAGS=-Wall -O2
LDFLAGS=

all: th4_hsdk_knet_rx-gts th4_hsdk_knet_tx-gts
	touch all

th4_hsdk_knet_rx-gts: th4_hsdk_knet_rx.c
	$(CC) $(CFLAGS) -o th4_hsdk_knet_rx-gts th4_hsdk_knet_rx.c $(LDFLAGS)

th4_hsdk_knet_tx-gts: th4_hsdk_knet_tx.c
	$(CC) $(CFLAGS) -o th4_hsdk_knet_tx-gts th4_hsdk_knet_tx.c $(LDFLAGS)
