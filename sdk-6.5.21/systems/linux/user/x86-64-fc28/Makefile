# -*- Makefile -*-
# $Id: Makefile,v 0.1 2017/06/01 00:00:00 spaulraj Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.

#
# This make job requires the following environment variables to be set:
#
# SDK                - path to StrataXGS SDK root directory
#
# Optionally the following environment variables can be set to
# override the default build server configuration:
#
# TOOLS_DIR      - path to build tools (if not in PATH already)
# CROSS_COMPILE  - cross compile tools prefix
# LINUX_INCLUDE  - path to Linux kernel include directory
#

# SHARED_LIBRARY_SUPPORT will enable the shared library creation - update the BINUTILSVER version.
ifneq ($(filter $(SHARED_LIBRARY_SUPPORT) $(COMPILE_OPENNSA_SUPPORT),1),)
# Newer GNU BINUTILS are neeed in order to avoid a known internal bug
# in GCC that results in a linker error when creating a .so library
export BINUTILSVER = 2.27
endif

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

override kernel_version=4_18
platform=x86-64-fc28
LINUX_MAKE_USER=1
export LINKER_RELAX = 1

# SHARED_LIBRARY_SUPPORT will enable the shared library creation - add a PIC to the compilation flags.
ifneq ($(filter $(SHARED_LIBRARY_SUPPORT) $(COMPILE_OPENNSA_SUPPORT),1),)
ADD_TO_CFLAGS = -frecord-gcc-switches -fPIC -DSAL_THREAD_STACK_MIN=8388608
export ADD_TO_CFLAGS
endif

include ${SDK}/make/Make.linux
