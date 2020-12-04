# $Id: Makefile,v 1.2.10.1 2011/10/26 17:03:16 assafz Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
export TARGET
export BLDDIR
export LIBDIR

LOCALDIR = src/soc/dpp/SAND/Utils

include ${SDK}/make/Make.config

lib = libbcm_dpp_sand_utils

OBJS = \
  sand_delta_list.o \
  sand_conv.o \
  sand_64cnt.o \
  sand_workload_status.o \
  sand_u64.o \
  sand_trace.o \
  sand_rand.o \
  sand_pp_mac.o \
  sand_bitstream.o \
  sand_sorted_list.o \
  sand_exact_match_hash.o \
  sand_tcm.o \
  sand_pat_tree.o \
  sand_os_bcm_interface.o \
  sand_integer_arithmetic.o \
  sand_group_member_list.o \
  sand_exact_match.o \
  sand_array_memory_allocator.o \
  sand_framework.o \
  sand_multi_set.o \
  sand_hashtable.o \
  sand_occupation_bitmap.o


BOBJS = $(addprefix ${BLDDIR}/, ${OBJS})

include ${SDK}/make/Make.lib
include ${SDK}/make/Make.depend
