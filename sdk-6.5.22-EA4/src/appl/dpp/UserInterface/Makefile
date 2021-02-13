# $Id: Makefile,v 1.2 Broadcom SDK $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2021 Broadcom Inc. All rights reserved.
# 

export TARGET
export BLDDIR
export LIBDIR

LOCALDIR = src/appl/dpp/UserInterface

include ${SDK}/make/Make.config

subdirs += ppd

include ${SDK}/make/Make.subdirs

lib = libappl_dpp_ui

OBJS  = \
      ui_general_hrp.o \
      ui_history.o    \
      ui_mem_init.o   \
      ui_memory_hrp.o \
      ui_module.o     \
      ui_nvram_hrp.o  \
      ui_ppd_api.o \
      ui_process.o \
      ui_sand.o    \
      ui_sweep_app_b.o  \
      ui_swp_p_pp_api.o \
      ui_utils.o 

BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
