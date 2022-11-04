#
# $Id: Makefile,v 1.4 2020/02/14 10:43:54 Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2022 Broadcom Inc. All rights reserved.
#


LOCALDIR = src/bcm/dnx/flow

include ${SDK}/make/Make.config

subdirs += porting
subdirs += app_defs
subdirs += dedicated_logic

include ${SDK}/make/Make.subdirs

lib = libbcm_dnxflow

include ${SDK}/make/Make.lib

include ${SDK}/make/Make.depend
