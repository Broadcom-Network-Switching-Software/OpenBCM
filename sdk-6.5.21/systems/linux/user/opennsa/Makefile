# -*- Makefile -*-
# $Id$
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.

#
# This make job requires the following environment variables to be set:
#
# SDK                - path to the SDK root directory
#
# Optionally the following environment variables can be set to
# override the default build server configuration:
#
# TOOLS_DIR      - path to build tools (if not in PATH already)
# CROSS_COMPILE  - cross compile tools prefix
# LINUX_INCLUDE  - path to Linux kernel include directory
#

# Newer GNU BINUTILS are neeed in order to avoid a known internal bug
# in GCC that results in a linker error when creating a .so library
export BINUTILSVER = 2.27

SDK :=$(shell if [ -n "$$SDK" ] ; then\
    echo $$SDK;\
    else\
    cd $(dir $(lastword $(MAKEFILE_LIST))); while /usr/bin/test ! -e RELEASE ; do \
    dir=`cd ../;pwd`;            \
    if [ "$$dir" = "/" ] ; then        \
       echo Cannot find SDK in $(lastword $(MAKEFILE_LIST)) 1>&2;        \
       exit 1;                \
    fi ;                    \
    cd $$dir;                \
    done ;                    \
    pwd;                    \
    fi)

ifeq ($(SDK),)
$(error Please run this in a tree)
endif

export SDK

ADD_TO_CFLAGS :=$(shell cat $$SDK/RELEASE | awk -F '[^0-9]*' {'print "-DOPENNSA_VERSION_MAJOR=" $$2, "-DOPENNSA_VERSION_MINOR=" $$3, "-DOPENNSA_VERSION_PATCH=" $$4'} )

override kernel_version=4_19
platform=opennsa
LINUX_MAKE_USER=1
LINUX_MAKE_DIR=$(SDK)/systems/linux/user/opennsa_ut
export LINKER_RELAX = 1
ADD_TO_CFLAGS += -frecord-gcc-switches -fPIC
export ADD_TO_CFLAGS
export BR_NO_CCACHE

include ${SDK}/make/Make.linux

#
# OpenNSA release packaging
#
.PHONY: release install
release: libopennsa.a libopennsa.so
	@if [ -z "$(DEST)" ] || [ -z "$(GPL_SRC)" ] ; then \
		echo -e "\n\
Please specify the following parameters:\n\n\
DEST=destination directory\n\
GPL_SRC=location of BDE GPL tar ball\n\n\
Example:\n\
\tmake release DEST=/projects/ntsw-sw8/home/temp/opennsa-6.5.21-EA1 GPL_SRC=/projects/ntsw_sw_releases/sdk/sdk-6.5.20/release/mkgpl/sdk-6.5.20-gpl-modules.tar.gz"; \
		exit 1; \
	fi
	rm -rf $(DEST)
	mkdir -p $(DEST)
	rsync -a $(LINUX_MAKE_DIR)/Legal $(DEST)/Legal
	rsync -a $(LINUX_MAKE_DIR)/README.md $(DEST)
	rsync -a --files-from=$(SDK)/FILES.opennsa $(SDK) $(DEST)
	mv $(DEST)/include/opennsa $(DEST)/include/bcm
	mkdir -p $(DEST)/src/diag
	rsync -a $(LINUX_MAKE_DIR)/demo_opennsa_init.c $(DEST)/src/diag
	rsync -a $(LINUX_MAKE_DIR)/Makefile.external $(DEST)/src/diag/Makefile
	cp $(SDK)/build/unix-user/opennsa/version.c $(DEST)/src/diag
# XGS section
	mkdir -p $(DEST)/lib/x86-64
	cp --dereference libopennsa.{a,so} $(DEST)/lib/x86-64
	perl $(SDK)/tools/bcm2c.pl $(SDK)/rc/config_th3_32x400.bcm > $(DEST)/src/diag/config_init_defaults.c
# end of XGS section
# DNX section
ifneq ("$(wildcard $(SDK)/build/unix-user-opennsa/wrx-opennsa)","")
	rsync -a $(LINUX_MAKE_DIR)/demo_opennsa_init_dpp.c $(DEST)/src/diag
	mkdir -p $(DEST)/lib/mips-BCM9XLP2_XMC_A1
	mkdir -p $(DEST)/lib/x86-64-fc28
	cp --dereference $(SDK)/build/unix-user-opennsa/wrx-opennsa/libopennsa.a $(DEST)/lib/mips-BCM9XLP2_XMC_A1
	cp --dereference $(SDK)/build/unix-user-opennsa/wrx-opennsa/libopennsa.so $(DEST)/lib/mips-BCM9XLP2_XMC_A1
	cp --dereference $(SDK)/build/unix-user/x86-64-fc28/libopennsa.a $(DEST)/lib/x86-64-fc28
	cp --dereference $(SDK)/build/unix-user/x86-64-fc28/libopennsa.so $(DEST)/lib/x86-64-fc28
	perl $(SDK)/tools/bcm2c.pl $(SDK)/rc/config-dcmn.bcm > $(DEST)/src/diag/config_init_dpp_defaults.c
	find $(SDK)/rc/config-jr2.bcm -type f -exec sed -i 's|dram_phy_tune_mode_on_init.BCM8869X=RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES|dram_phy_tune_mode_on_init.BCM8869X=RUN_TUNE|g' {} \;
	find $(SDK)/rc/config-j2c.bcm -type f -exec sed -i 's|dram_phy_tune_mode_on_init.BCM8880X=RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES|dram_phy_tune_mode_on_init.BCM8880X=RUN_TUNE|g' {} \;
	perl $(SDK)/tools/bcm2c.pl $(SDK)/rc/config-sand.bcm > $(DEST)/src/diag/config_init_dnx_defaults.c
	find $(SDK)/rc/config-j2c.bcm -type f -exec sed -i 's|dram_phy_tune_mode_on_init.BCM8880X=RUN_TUNE|dram_phy_tune_mode_on_init.BCM8880X=RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES|g' {} \;
	find $(SDK)/rc/config-jr2.bcm -type f -exec sed -i 's|dram_phy_tune_mode_on_init.BCM8869X=RUN_TUNE|dram_phy_tune_mode_on_init.BCM8869X=RESTORE_TUNE_PARAMETERS_FROM_SOC_PROPERTIES|g' {} \;
else
	@echo WARNING: DNX devices are NOT included in the package!
endif
# end of DNX section
	(cd $(DEST)/src; tar xfz $(GPL_SRC); mv *-gpl-modules gpl-modules)
	cp $(SDK)/RELEASE $(DEST)
	cd $(dir $(DEST)); tar cfz $(notdir $(DEST)).tar.gz $(notdir $(DEST))

#
# install is used for unit testing
#
install: release
	@if [ -z "$(DEST)" ]; then \
		echo -e "\n\
Please specify the following parameters:\n\n\
DEST=destination directory\n\
Example:\n\
\tmake install DEST=/projects/ntsw-sw8/home/iakramov/opennsa-6.5.17-EA10"; \
		exit 1; \
	fi
	cp $(SDK)/rc/bcm88375_board.soc* $(DEST)/src/diag/
	cp $(SDK)/rc/bcm88470_board.soc* $(DEST)/src/diag/
	cp $(SDK)/rc/combo28_dram.soc* $(DEST)/src/diag/
	cp $(SDK)/rc/combo28_dram_multi_88675.soc* $(DEST)/src/diag/
	cp $(SDK)/rc/jer.soc $(DEST)/src/diag/
	cp $(SDK)/rc/rc.soc $(DEST)/src/diag/
	cp $(SDK)/rc/reload-dune.soc $(DEST)/src/diag/
	cp $(SDK)/rc/qax.soc $(DEST)/src/diag/
	cp -arp $(SDK)/rc/dnx_sku $(DEST)/src/diag/
	cp $(SDK)/rc/dnx.soc $(DEST)/src/diag/
	find $(DEST)/src/diag/dnx.soc -type f -exec sed -i 's|INIT_DNX|\#INIT_DNX|g' {} \;
