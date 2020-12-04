# -*- Makefile -*-
# $Id: Makefile,v 0.1 2015/06/30 00:00:00 nowakm Exp $
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

override kernel_version=4_4
platform=xlr-$(kernel_version)
LINUX_MAKE_USER=1
export LINKER_RELAX = 1


include ${SDK}/make/Make.linux

