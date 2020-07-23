/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * File:        fec_allocation.h
 * Purpose:     fec allocation managament
 */


#ifndef __USERINTERFACE_FEC_ALLOCATION_H__
#define __USERINTERFACE_FEC_ALLOCATION_H__


#include <soc/dpp/SAND/Utils/sand_sorted_list.h>
#include <bcm/l3.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dpp/l3.h>
#include <shared/bsl.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm/debug.h>
#include <bcm/failover.h>
#include <sal/appl/io.h> 
#include <sal/core/libc.h> 
#include <sal/core/spl.h> 
#include <sal/core/alloc.h> 
#include <sal/core/dpc.h> 


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_L3
#define DOES_NOT_BELONG_TO_ECMP -1
#define DATABASE_FILE_PATH_ALLOC_MANAGER "database_file_alloc_manager.txt"
#define DATABASE_FILE_PATH_FECS_LIST "database_file_fecs_list.txt"

int fec_allocation_bcm_l3_egress_ecmp_destroy(int unit,bcm_l3_egress_ecmp_t *ecmp);
int fec_allocation_bcm_l3_egress_destroy(int unit, bcm_if_t intf);
int fec_allocation_bcm_l3_egress_ecmp_delete(int unit, bcm_l3_egress_ecmp_t *ecmp, bcm_if_t intf);
int fec_allocation_bcm_l3_egress_ecmp_add(int unit, bcm_l3_egress_ecmp_t *ecmp, bcm_if_t intf);
int fec_allocation_bcm_l3_egress_ecmp_create(int unit, bcm_l3_egress_ecmp_t *ecmp, int intf_count, bcm_if_t *intf_array);
int fec_allocation_bcm_l3_egress_create(int unit, uint32 flags, bcm_l3_egress_t *egr, bcm_if_t *intf);
int fec_allocation_create_database_file(int unit);
int fec_allocation_initialize(int unit);
void fec_allocation_deinitialize_last(void);
void fec_allocation_deinitialize_unit(int unit);
int fec_allocation_associate_fec(int unit, bcm_if_t fec_id);
int soc_dpp_fec_allocation_main(int unit);
int fec_allocation_main(int unit, int write_logs);

#endif

