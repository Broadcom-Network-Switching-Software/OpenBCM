# -*- Makefile -*-
# $Id: Makefile,v 1.7 2013/01/03 23:10:19 bpeela Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.

#
# This make job requires the following environment variables to be set:
#
# SDK                - path to StrataXGS SDK root directory
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

override kernel_version=5_4
platform=iproc_64-5_4

IPROC_BUILD=1
export IPROC_BUILD
export BUILD_PLATFORM
export ARM_LINUX_VERSION

LINUX_MAKE_USER=1
export ADD_TO_CFLAGS
export BR_NO_CCACHE

include ${SDK}/make/Make.linux
