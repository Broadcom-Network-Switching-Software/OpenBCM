/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pfc.h
 * Purpose:     Declarations for Tomahawk3 PFC SOC functions.
 */

#ifndef _SOC_PFC_H
#define _SOC_PFC_H
#include <soc/defs.h>
#include <soc/types.h>
#include <soc/tomahawk3.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)
#define TH3_PFC_COS_NUM 12
#define TH3_PFC_PRIORITY_NUM 8
#define TH3_PFC_MMU_QUEUE_NUM 12
#define TH3_PFC_RX_PROFILE_NUM_ENTRIES 8

#define TH3_PFC_PROPERTIES_PROFILE_ID_MIN 0
#define TH3_PFC_PROPERTIES_PROFILE_ID_MAX 7
#define TH3_PFC_CLASS_PROFILE_ID_MIN 0
#define TH3_PFC_CLASS_PROFILE_ID_MAX 7
#define SOC_TH3_PFC_BMP_MEMBER(bmp, mber) ((bmp) & (1U << (mber)))
#define SOC_TH3_PFC_BMP_ITER(bmp, max_n, mber) \
        for((mber) = 0; (mber) < (max_n); (mber)++) \
            if (SOC_TH3_PFC_BMP_MEMBER((bmp), (mber)))


typedef struct soc_cosq_pfc_class_map_config_s {
    uint8 pfc_enable;
    uint8 pfc_optimized;
    uint32 cos_list_bmp;
} soc_cosq_pfc_class_map_config_t;

typedef enum soc_th3_pfc_profile_op_e {
    SOC_TH3_PFC_PROFILE_OP_GET = 0,
    SOC_TH3_PFC_PROFILE_OP_SET = 1
} soc_th3_pfc_pfc_profile_op_t;

typedef struct soc_cosq_priority_group_properties_s {
    int priority_group_id;
    int  service_pool_id;
    int  headroom_pool_id;

} soc_cosq_priority_group_properties_t;

typedef enum soc_th3_pfc_profile_e {
    socTH3PFCPGProperties = 0, /* PG properties profile */
    socTH3PFCReceiveClass = 1, /* PFC-rx Priority to CoS mapping */
    socTH3PFCProfileMax = 2
} soc_th3_pfc_profile_t;

int
soc_th3_pfc_pg_profile_op(
    int unit,
    int profile_id,
    int max_count,
    soc_th3_pfc_pfc_profile_op_t op,
    int* array_count,
    soc_cosq_priority_group_properties_t *properties);

int
soc_th3_pfc_tx_priority_mapping_profile_op(
    int unit,
    int profile_id,
    int max_count,
    soc_th3_pfc_pfc_profile_op_t op,
    int *array_count,
    int *pg_array);

int
soc_th3_pfc_num_optimized_group_get(
    int unit,
    soc_port_t port,
    int *num);


int
soc_th3_pfc_rx_priority_mapping_profile_get(
    int unit,
    int profile_id,
    int max_count,
    soc_cosq_pfc_class_map_config_t *config_array,
    int *count);
int
soc_th3_pfc_rx_priority_mapping_profile_set(
    int unit,
    int profile_id,
    soc_cosq_pfc_class_map_config_t  *config_array,
    int count);

int
soc_th3_pfc_config_profile_id_set(
    int unit,
    soc_port_t port,
    soc_th3_pfc_profile_t type,
    uint32 profile_id);

int
soc_th3_pfc_config_profile_id_get(
    int unit,
    soc_port_t port,
    soc_th3_pfc_profile_t type,
    uint32 *profile_id);

int
soc_th3_pfc_rx_init_from_cfg(int unit);

int
soc_th3_pfc_priority_to_pg_map_init_from_cfg(int unit);
#endif
#endif

