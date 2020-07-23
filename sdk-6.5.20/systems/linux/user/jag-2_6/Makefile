# -*- Makefile -*-
# $Id: Makefile,v 1.4 2008/08/18 20:25:16 majotk Exp $
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
# MIPS_TOOLS_DIR     - path to build tools (if not in PATH already)
# MIPS_CROSS_COMPILE - cross compile tools prefix
# LINUX_INCLUDE      - path to Linux kernel include directory
#

SDK :=$(shell if [ -n "$$SDK" ] ; then\
	echo $$SDK;\
	else\
	cd $(dir $(lastword $(MAKEFILE_LIST))); while /usr/bin/test ! -e RELEASE ; do \
	dir=`cd ../;pwd`;			\
	if [ "$$dir" = "/" ] ; then		\
	   echo Cannot find SDK in $(lastword $(MAKEFILE_LIST)) 1>&2;		\
	   exit 1;				\
	fi ;					\
	cd $$dir;				\
	done ;					\
	pwd;					\
	fi)

ifeq ($(SDK),)					
$(error Please run this in a tree)		
endif 				

export SDK

override kernel_version=2_6
platform=jag-$(kernel_version)

# Select the build environment
#BUILD_PLATFORM=ELDK
#BUILD_PLATFORM=WR_LINUX

#Select the LINUX KERNEL VERSION
#KERN_VER = 2.6.21.7


# Select the ELDK version
#ELDK_VERSION=4.1

# Windriver linux version
#WRS_LINUX_VERSION=1.4
#WRS_LINUX_VERSION=2.0

export KERN_VER
export ELDK_VERSION
export BUILD_PLATFORM
export WRS_LINUX_VERSION

LINUX_MAKE_USER=1
include ${SDK}/make/Make.linux

