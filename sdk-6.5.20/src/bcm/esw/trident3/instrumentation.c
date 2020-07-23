/*
 * $Id: instrumentaion.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        instrumentaion.c
 * Purpose:     Visibility Packet trace core implementaion for
 *              Trident3.
 */

#include <soc/drv.h>
#if defined(BCM_INSTRUMENTATION_SUPPORT)
#include <soc/format.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/loopback.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/switch.h>
#include <bcm_int/esw/instrumentation.h>
#include <shared/bsl.h>
#include <shared/types.h>
#include <shared/bitop.h>
#include <bcm/l2.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw/trident3.h>

#define _BCM_PKT_TRACE_FLOW_TYPE_FIXED(_type_) \
       (((_type_) == bcmPktFlowIdFixedNoop) || \
        ((_type_) == bcmPktFlowIdFixedTunnelMim) || \
        ((_type_) == bcmPktFlowIdFixedTunnelNvgre) || \
        ((_type_) == bcmPktFlowIdFixedTunnelVxlan) || \
        ((_type_) == bcmPktFlowIdFixedTunnelIpinIp) || \
        ((_type_) == bcmPktFlowIdFixedTunnelMpls))

#define bcmPktFlowIdFixedNoop 1
#define bcmPktFlowIdFixedTunnelMim 2
#define bcmPktFlowIdFixedTunnelNvgre 3
#define bcmPktFlowIdFixedTunnelVxlan 4
#define bcmPktFlowIdFixedTunnelIpinIp 5
#define bcmPktFlowIdFixedTunnelMpls 6

#define MAX_DOP_FORMATS(_u_)         ((SOC_IS_TRIDENT3(_u_)) ? 120 : ((SOC_IS_HELIX5X(_u_)) ? 133 : 123))
#define MAX_ING_DOP_FORMATS(_u_)     ((SOC_IS_TRIDENT3(_u_)) ? 105 : ((SOC_IS_HELIX5X(_u_)) ? 114 : 108))
#define MAX_EGR_DOP_FORMATS(_u_)     ((SOC_IS_HELIX5X(_u_)) ? 19 : 15)
#define DOP_INGRESS 0x2
#define DOP_EGRESS 0x4
#define DOP_PIPE_0 0
#define DOP_PIPE_1 1

#define TD3_FORWARDING_TYPE_L2_FORWARD 0
#define TD3_FORWARDING_TYPE_L3UC 1

#define TD3_MAX_PKT_TRACE_CPU_PROFILE_INDEX 0xF
#define BCM_PKT_TRC_DOP_DATA_WORD_LEN 4

#define BYTE_MASK 0xFF
#define BYTE_SHIFT 8

/* when visibility packet is sent
 *    following datastructure will be set */
STATIC _bcm_switch_pkt_trace_port_info_t td3_pkt_trace_port_info[BCM_MAX_NUM_UNITS];
STATIC uint8 td3_cpu_pkt_profile_id[BCM_MAX_NUM_UNITS];

typedef struct pkt_trace_dop_format_s {
    soc_format_t format;
    uint16 dop_id;
    uint16 data_phases;
    uint32 *entry;
    uint32 *next;
} pkt_trace_dop_format_t;

typedef struct pkt_trace_hash_node_s {
    pkt_trace_dop_format_t *ft_entry;
    uint32                 entry_num;
} pkt_trace_hash_node_t;

typedef struct pkt_trace_hash_info_s{
     int unit;
     pkt_trace_hash_node_t *pkt_trace_hash_list;
} pkt_trace_hash_info_t;

STATIC pkt_trace_hash_info_t *pkt_trace_hash_info[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_esw_pkt_trace_dop_data_swap
 * Purpose:
 *      dop data swap
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_esw_pkt_trace_dop_data_swap(int unit,
                                 uint32 src_word_len,
                                 uint32 *src,
                                 uint32 *dst)
{
    int i = 0;

    if ((src == NULL)
        || (dst == NULL)){
        return BCM_E_INTERNAL;
    }

    for (i = 0; i < src_word_len; i++) {
        *(dst + (src_word_len - 1) - i) = *src++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_info_dop_init
 * Purpose:
 *      Initialize the td3 packet trace.
 *      DOP's
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_dop_init(int unit,
                            uint32 block,
                            uint32 dop_id_start)
{
    /* initialize the DOP */
    uint32 i = 0;
    soc_reg_t ing_dop_ctrl_0[3] = {ING_DOP_CTRL_0_64r,
                                   ING_DOP_CTRL_0_64_PIPE0r,
                                   ING_DOP_CTRL_0_64_PIPE1r};
    soc_reg_t egr_dop_ctrl_0[3] = {EGR_DOP_CTRL_0_64r,
                                   EGR_DOP_CTRL_0_64_PIPE0r,
                                   EGR_DOP_CTRL_0_64_PIPE1r};
    soc_reg_t ing_dop_ctrl_2[3] = {ING_DOP_CTRL_2_64r,
                                   ING_DOP_CTRL_2_64_PIPE0r,
                                   ING_DOP_CTRL_2_64_PIPE1r};
    soc_reg_t egr_dop_ctrl_2[3] = {EGR_DOP_CTRL_2_64r,
                                   EGR_DOP_CTRL_2_64_PIPE0r,
                                   EGR_DOP_CTRL_2_64_PIPE1r};

    uint64 irval;
    uint64 erval;
    switch (block) {
        case DOP_INGRESS:
            for (i = 0; i < (NUM_PIPE(unit) + 1); i++) {
                /* DOP BASE set  for Ingress DOPs*/
                sal_memset(&irval, 0, sizeof(irval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, ing_dop_ctrl_2[i],
                             REG_PORT_ANY, 0, &irval));
                soc_reg64_field32_set(unit, ing_dop_ctrl_2[i],
                        &irval, DOP_IDf, dop_id_start);
                soc_reg64_field32_set(unit, ing_dop_ctrl_2[i],
                        &irval, NUM_DOPSf, MAX_ING_DOP_FORMATS(unit));
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, ing_dop_ctrl_2[i],
                            REG_PORT_ANY, 0, irval));
                sal_memset(&irval, 0, sizeof(irval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, ing_dop_ctrl_0[i],
                             REG_PORT_ANY, 0, &irval));
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, CAPTURE_MODEf, 0 /* capture_first*/);
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, COMMANDf,  1/* init*/);
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, STARTf, 1 /* start */);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, ing_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, irval));
            }
            break;
        case DOP_EGRESS:
            for (i = 0; i < (NUM_PIPE(unit) + 1); i++) {
                /* DOP BASE set for Egress DOP's */
                sal_memset(&erval, 0, sizeof(erval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, egr_dop_ctrl_2[i],
                             REG_PORT_ANY, 0, &erval));
                soc_reg64_field32_set(unit, egr_dop_ctrl_2[i],
                        &erval, DOP_IDf, dop_id_start);
                soc_reg64_field32_set(unit, egr_dop_ctrl_2[i],
                        &erval, NUM_DOPSf, MAX_EGR_DOP_FORMATS(unit));
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, egr_dop_ctrl_2[i],
                            REG_PORT_ANY, 0, erval));
                sal_memset(&erval, 0, sizeof(erval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, egr_dop_ctrl_0[i],
                             REG_PORT_ANY, 0, &erval));
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, COMMANDf,  1/* init*/);
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, STARTf, 1 /* start */);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, egr_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, erval));
            }
            break;
        default:
            break;
    }
    return BCM_E_NONE;
}

#if 0
/*
 * Function:
 *      _bcm_td3_pkt_trace_info_dop_capture
 * Purpose:
 *      Capture on the td3 packet trace
 *      DOP's
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_dop_capture(int unit, uint32 block)
{
    /* initialize the DOP */
    uint32 i = 0;
    soc_reg_t ing_dop_ctrl_0[3] = {ING_DOP_CTRL_0_64r,
                                   ING_DOP_CTRL_0_64_PIPE0r,
                                   ING_DOP_CTRL_0_64_PIPE1r};

    soc_reg_t egr_dop_ctrl_0[3] = {EGR_DOP_CTRL_0_64r,
                                   EGR_DOP_CTRL_0_64_PIPE0r,
                                   EGR_DOP_CTRL_0_64_PIPE1r};
    uint64 irval;
    uint64 erval;

    switch (block) {
        case DOP_INGRESS:
            for (i = 0; i < 3 ; i++) {
                sal_memset(&irval, 0, sizeof(irval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, ing_dop_ctrl_0[i],
                             REG_PORT_ANY, 0, &irval));
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, CAPTURE_MODEf, 1 /* capture_first*/);
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, STARTf, 1 /* start */);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, ing_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, irval));
            }
            break;
        case DOP_EGRESS:
            for (i = 0; i < 3 ; i++) {
                sal_memset(&erval, 0, sizeof(erval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, egr_dop_ctrl_0[0],
                             REG_PORT_ANY, 0, &erval));
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[1],
                        &erval, CAPTURE_MODEf, 1 /* capture_first*/);
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &irval, STARTf, 1 /* start */);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, egr_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, erval));
            }
            break;
        default:
            break;
    }
    return BCM_E_NONE;

}
#endif

/*
 * Function:
 *      _bcm_td3_pkt_trace_info_dop_reset
 * Purpose:
 *      Reset the td3 packet trace
 *      DOP's
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_dop_reset(int unit, uint32 block)
{
    /* initialize the DOP */
    soc_reg_t ing_dop_ctrl_0[3] = {ING_DOP_CTRL_0_64r,
                                   ING_DOP_CTRL_0_64_PIPE0r,
                                   ING_DOP_CTRL_0_64_PIPE1r};
    soc_reg_t egr_dop_ctrl_0[3] = {EGR_DOP_CTRL_0_64r,
                                   EGR_DOP_CTRL_0_64_PIPE0r,
                                   EGR_DOP_CTRL_0_64_PIPE1r};

    uint32 i = 0;
    uint64 irval;
    uint64 erval;
    switch (block) {
        case DOP_INGRESS:
            for (i = 0; i < (NUM_PIPE(unit) + 1); i++) {
                sal_memset(&irval, 0, sizeof(irval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, ing_dop_ctrl_0[i],
                             REG_PORT_ANY, 0, &irval));
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, CAPTURE_MODEf, 0 /* capture_first*/);
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, COMMANDf, 0 /* reset*/);
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, STARTf, 0 /* start */);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, ing_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, irval));
            }
            break;
        case DOP_EGRESS:
            for (i = 0; i < (NUM_PIPE(unit) + 1); i++) {
                sal_memset(&erval, 0, sizeof(erval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, egr_dop_ctrl_0[i],
                             REG_PORT_ANY, 0, &erval));
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, COMMANDf, 0 /*reset*/);
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, STARTf, 0 /* start */);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, egr_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, erval));
            }
            break;
        default:
            break;
    }
    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_td3_pkt_trace_info_dop_collect
 * Purpose:
 *      Collect the td3 packet trace
 *      DOP's
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_dop_collect(int unit,
                                uint32 block)
{
    /* initialize the DOP */
    soc_reg_t ing_dop_ctrl_0[3] = {ING_DOP_CTRL_0_64r,
                                   ING_DOP_CTRL_0_64_PIPE0r,
                                   ING_DOP_CTRL_0_64_PIPE1r};
    soc_reg_t egr_dop_ctrl_0[3] = {EGR_DOP_CTRL_0_64r,
                                   EGR_DOP_CTRL_0_64_PIPE0r,
                                   EGR_DOP_CTRL_0_64_PIPE1r};
    uint32 i = 0;
    uint64 irval;
    uint64 erval;

    switch (block) {
        case DOP_INGRESS:
            for (i = 0; i < (NUM_PIPE(unit) + 1); i++) {
                sal_memset(&irval, 0, sizeof(irval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, ing_dop_ctrl_0[i],
                             REG_PORT_ANY, 0, &irval));
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, CAPTURE_MODEf, 0 /* capture_first*/);
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, COMMANDf,  3/* collect*/);
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, STARTf, 1 /* start */);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, ing_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, irval));
            }
            break;
        case DOP_EGRESS:
            for (i = 0; i < (NUM_PIPE(unit) + 1); i++) {
                sal_memset(&erval, 0, sizeof(erval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, egr_dop_ctrl_0[i],
                             REG_PORT_ANY, 0, &erval));
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, CAPTURE_MODEf, 0 /* capture_first*/);
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, COMMANDf,  3/* collect*/);
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, STARTf, 1 /* start */);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, egr_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, erval));
            }
            break;
        default:
            break;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_info_dop_
 * Purpose:
 *      Clear the td3 packet trace
 *      DOP's
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_dop_clear(int unit, uint32 block)
{
    /* initialize the DOP */
    soc_reg_t ing_dop_ctrl_0[3] = {ING_DOP_CTRL_0_64r,
                                   ING_DOP_CTRL_0_64_PIPE0r,
                                   ING_DOP_CTRL_0_64_PIPE1r};
    soc_reg_t egr_dop_ctrl_0[3] = {EGR_DOP_CTRL_0_64r,
                                   EGR_DOP_CTRL_0_64_PIPE0r,
                                   EGR_DOP_CTRL_0_64_PIPE1r};
    uint32 i = 0;
    uint64 irval;
    uint64 erval;

    switch (block) {
        case DOP_INGRESS:
            for (i = 0; i < (NUM_PIPE(unit) + 1); i++) {
                sal_memset(&irval, 0, sizeof(irval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, ing_dop_ctrl_0[i],
                             REG_PORT_ANY, 0, &irval));
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, COMMANDf,  2/* clear*/);
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, CAPTURE_MODEf, 0 /* capture_first*/);
                soc_reg64_field32_set(unit, ing_dop_ctrl_0[i],
                        &irval, STARTf, 1 /* start */);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, ing_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, irval));
            }
            break;
        case DOP_EGRESS:
            for (i = 0; i < (NUM_PIPE(unit) + 1); i++) {
                sal_memset(&erval, 0, sizeof(erval));
                BCM_IF_ERROR_RETURN(
                    soc_reg64_get(unit, egr_dop_ctrl_0[i],
                             REG_PORT_ANY, 0, &erval));
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, COMMANDf,  2/* clear*/);
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, STARTf, 1 /* start */);
                soc_reg64_field32_set(unit, egr_dop_ctrl_0[i],
                        &erval, CAPTURE_MODEf,
                        0 /* capture_start*/);
                BCM_IF_ERROR_RETURN(
                  soc_reg64_set(unit, egr_dop_ctrl_0[i],
                            REG_PORT_ANY, 0, erval));
            }
            break;
        default:
            break;
    }
    return BCM_E_NONE;
}



/*
 * Function:
 *      _bcm_td3_pkt_trace_dop_buffer_collecct
 * Purpose:
 *      read results ptr buffer for
 *      visibility packet process and
 *      store into pkt_trace_info
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_td3_pkt_trace_dop_buffer_collect(int unit,
                                               uint32 block,
                                               uint32 buf_size,
                                               uint32 *buf,
                                               uint32 *data_size)
{
    soc_mem_t mem = INVALIDm;
    uint32 mem_idx_min = 0;
    uint32 mem_idx_max = 0;
    uint32 dop_index = 0;
    uint32 *tmp_buf = NULL;
    int rv = BCM_E_NONE;
    uint32 dop_data[BCM_PKT_TRC_DOP_DATA_WORD_LEN];
    uint32 data[BCM_PKT_TRC_DOP_DATA_WORD_LEN];
    uint32 *dop_entry_ptr = NULL;
    int pipe = 0;

    LOG_VERBOSE(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
           "Packet trace buffer collect")));

    pipe = td3_pkt_trace_port_info[unit].pkt_trace_src_pipe;

    if ((buf == NULL) ||
        (buf_size == 0) ||
        (data_size == NULL)) {
        return BCM_E_INTERNAL;
    }
    if ((pipe != DOP_PIPE_0) &&
       (pipe != DOP_PIPE_1)) {
        return BCM_E_PARAM;
    }
    *data_size = 0;
    if (block == DOP_INGRESS) {
        if (pipe == DOP_PIPE_0) {
            mem = ING_DOP_STORAGE_MEM_PIPE0m;
        } else if (pipe == DOP_PIPE_1) {
            mem = ING_DOP_STORAGE_MEM_PIPE1m;
        }
    } else if (block == DOP_EGRESS) {
        if (pipe == DOP_PIPE_0) {
            mem = EGR_DOP_STORAGE_MEM_PIPE0m;
        } else if (pipe == DOP_PIPE_1) {
            mem = EGR_DOP_STORAGE_MEM_PIPE1m;
        }
    } else {
        return BCM_E_PARAM;
    }
    mem_idx_min = soc_mem_index_min(unit, mem);
    mem_idx_max = soc_mem_index_max(unit, mem);

    tmp_buf = (uint32 *) soc_cm_salloc(unit, buf_size,
                              "dop buffer collect");
    if (tmp_buf == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(tmp_buf, 0, buf_size);

    /*Reading the whole chunk */
    rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ANY,
                                mem_idx_min, mem_idx_max, buf);

    if (BCM_FAILURE(rv)) {
        goto clean_up;
    }
    for (dop_index = 0; dop_index <= mem_idx_max; dop_index++) {
        dop_entry_ptr = soc_mem_table_idx_to_pointer(unit, mem, uint32 *,
                                             buf, dop_index);
        soc_mem_field_get(unit, mem, dop_entry_ptr, DATAf, data);
         _bcm_esw_pkt_trace_dop_data_swap(unit,
                                          BCM_PKT_TRC_DOP_DATA_WORD_LEN,
                                          data,
                                          dop_data);
        sal_memcpy((tmp_buf + (dop_index * BCM_PKT_TRC_DOP_DATA_WORD_LEN)),
                dop_data, sizeof(dop_data));
        *data_size += (BCM_PKT_TRC_DOP_DATA_WORD_LEN * (sizeof(uint32)));
    }
    sal_memset(buf, 0, buf_size);
    sal_memcpy(buf, tmp_buf, *data_size);
clean_up:
    if (tmp_buf != NULL) {
        soc_cm_sfree(unit, tmp_buf);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_pkt_trace_format_hash_key_get
 * Purpose:
 *      DOP parsing.
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int  _bcm_pkt_trace_format_hash_key_get(int unit,
                                      soc_format_t format,
                                      uint32 *key)
{
    uint32 num_dops = MAX_DOP_FORMATS(unit);
    if (key == NULL) {
        return BCM_E_INTERNAL;
    }
    *key = (1 * format) % num_dops;
    return BCM_E_NONE;
}

STATIC
uint32 _bcm_td3_dop_format_field_get(int unit,
                                    soc_format_t format,
                                    soc_field_t field,
                                    uint32 *fldbuf)
{
    pkt_trace_hash_info_t  *ph = NULL;
    pkt_trace_hash_node_t  *hash_node = NULL;
    pkt_trace_hash_node_t  *hash_list = NULL;
    pkt_trace_dop_format_t *fptr = NULL;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 key;

    ph = pkt_trace_hash_info[unit];
    if (ph->pkt_trace_hash_list == NULL) {
        return BCM_E_INTERNAL;
    }
    hash_list = ph->pkt_trace_hash_list;

    BCM_IF_ERROR_RETURN(
             _bcm_pkt_trace_format_hash_key_get(unit,
                       format, &key));

    if (key >= MAX_DOP_FORMATS(unit)) {
        return BCM_E_INTERNAL;
    }

    hash_node = &hash_list[key];

    if ((!hash_node->entry_num) ||
        (hash_node->ft_entry == NULL)) {
        return BCM_E_PARAM;
    }
    sal_memset(entry, 0, sizeof(entry));
    for(fptr = hash_node->ft_entry;
        fptr != NULL;
        fptr = (pkt_trace_dop_format_t *) fptr->next) {
        if (fptr->format == format) {
            sal_memcpy(entry, fptr->entry,
                     WORDS2BYTES(fptr->data_phases));
            soc_format_field_get(unit, format,
                        entry,
                        field,
                        fldbuf);
            return BCM_E_NONE;
        }
    }
    return BCM_E_NOT_FOUND;
}

STATIC
uint32  _bcm_td3_dop_format_field32_get(int unit,
                                    soc_format_t format,
                                    soc_field_t field)
{
    pkt_trace_hash_info_t  *ph = NULL;
    pkt_trace_hash_node_t  *hash_node = NULL;
    pkt_trace_hash_node_t  *hash_list = NULL;
    pkt_trace_dop_format_t *fptr = NULL;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 key;
    uint32 value = 0;

    ph = pkt_trace_hash_info[unit];
    if (ph->pkt_trace_hash_list == NULL) {
        return BCM_E_INTERNAL;
    }
    hash_list = ph->pkt_trace_hash_list;


    BCM_IF_ERROR_RETURN(
             _bcm_pkt_trace_format_hash_key_get(unit,
                       format, &key));

    if (key >= MAX_DOP_FORMATS(unit)) {
        return 0;
    }
    hash_node = &hash_list[key];

    if ((!hash_node->entry_num) ||
        (hash_node->ft_entry == NULL)) {
        return 0;
    }
    sal_memset(entry, 0, sizeof(entry));
    for(fptr = hash_node->ft_entry;
        fptr != NULL;
        fptr = (pkt_trace_dop_format_t *) fptr->next) {
        if (fptr->format == format) {
            sal_memcpy(entry, fptr->entry,
                     WORDS2BYTES(fptr->data_phases));
            value = soc_format_field32_get(unit,
                    format, entry, field);
            break;
        }
    }

    return value;
}

STATIC
int _bcm_td3_pkt_trace_flow_type_get(int unit)

{
    uint32 pkt_flow_id = 0;
    uint32 flow_type_fixed = 0;

    pkt_flow_id =
         _bcm_td3_dop_format_field32_get(unit,
                 PKT_FLOW_SELECT_TCAM_1_DOP_DOPfmt,
                 PKT_FLOW_ID_1f);
    flow_type_fixed = soc_format_field32_get(unit, PKT_FLOW_ID_1_FORMATfmt,
                      &pkt_flow_id,
                      MACRO_FLOW_ID_FIXEDf);
    if (_BCM_PKT_TRACE_FLOW_TYPE_FIXED(flow_type_fixed)) {
        return flow_type_fixed;
    }
    return FALSE;
}

#if defined(INCLUDE_L3)
STATIC
int _bcm_td3_pkt_trace_nh_dvp_get(int unit,
                                  uint32 nh,
                                  uint32 *dvp)

{
    uint32 vp = 0;
    uint32 nh_index = 0;
    ing_dvp_table_entry_t dvp_entry;

    sal_memset(&dvp_entry, 0, sizeof(ing_dvp_table_entry_t));

    if (dvp == NULL) {
       return BCM_E_PARAM;
    }
    *dvp = 0;
    for (vp = 0;
         vp < soc_mem_index_count(unit, SOURCE_VPm);
         vp++) {
         if (_bcm_vp_used_get(unit, vp, _bcmVpTypeAny)) {
             BCM_IF_ERROR_RETURN(
               READ_ING_DVP_TABLEm(unit, MEM_BLOCK_ANY,
                          vp, &dvp_entry));
               nh_index = soc_ING_DVP_TABLEm_field32_get(
                           unit, &dvp_entry,
                           NEXT_HOP_INDEXf);
             if (nh_index == nh) {
                 *dvp = vp;
                 return BCM_E_NONE;
             }
         }
    }
    return BCM_E_NOT_FOUND;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_td3_pkt_trace_lookup_status_get
 * Purpose:
 *      Lookup status processing
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_lookup_status_get(int unit,
                bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    uint32 pkt_flow_type_fixed = FALSE;
    uint32 ifp_lookup_vec = 0;

    pkt_flow_type_fixed = _bcm_td3_pkt_trace_flow_type_get(unit);

    if (pkt_flow_type_fixed) {
        if (pkt_flow_type_fixed == bcmPktFlowIdFixedTunnelMpls) {
             if (_bcm_td3_dop_format_field32_get(unit,
                    ING_TUNNEL_ADAPT_2_DOPfmt,
                      HIT_0f))  {
                SHR_BITSET(
                 (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
                  bcmSwitchPktTraceLookupMplsLabel1Hit);
            }
            if (_bcm_td3_dop_format_field32_get(unit,
                    ING_TUNNEL_ADAPT_2_DOPfmt,
                      HIT_1f))  {
                SHR_BITSET(
                 (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
                  bcmSwitchPktTraceLookupMplsLabel2Hit);
            }
        } else {
            if (_bcm_td3_dop_format_field32_get(unit,
                    ING_TUNNEL_ADAPT_1_DOPfmt,
                    HIT_0f))  {
                SHR_BITSET(
                 (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
                  bcmSwitchPktTraceLookupFirstVlanTranslationHit);
            }
            if (_bcm_td3_dop_format_field32_get(unit,
                    ING_TUNNEL_ADAPT_1_DOPfmt,
                    HIT_1f))  {
                SHR_BITSET(
                 (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
                  bcmSwitchPktTraceLookupSecondVlanTranslationHit);
            }
        }
        if (_bcm_td3_dop_format_field32_get(unit,
                ING_TUNNEL_ADAPT_2_DOPfmt,
                  HIT_0f))  {
            SHR_BITSET(
             (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
              bcmSwitchPktTraceLookupMplsLabel1Hit);
        }
        if (_bcm_td3_dop_format_field32_get(unit,
                ING_TUNNEL_ADAPT_2_DOPfmt,
                  HIT_1f))  {
            SHR_BITSET(
             (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
              bcmSwitchPktTraceLookupMplsLabel2Hit);
        }
        if (_bcm_td3_dop_format_field32_get(unit,
                ING_TUNNEL_ADAPT_3_LKUP_DOPfmt,
                  HITf))  {
            SHR_BITSET(
             (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
              bcmSwitchPktTraceLookupIpTunnelHit);
        }
    }
    /********************************************************************
      NOTE : FORWARDING_1_LKUP_DOP.HIT0/1
             FORWARDING_2_LKUP_DOP.HIT0/1
             FORWARDING_3_LKUP_DOP.HIT0/1
             TD3-3732 look for INDEX_0/1 ! = 0 for HIT.
 */
    if (_bcm_td3_dop_format_field32_get(unit,
            RSEL1_3_DOPfmt,
            FORWARD_1_LKUP_1_VALIDf)){
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
          bcmSwitchPktTraceLookupL2SrcHit);
    }
    if (_bcm_td3_dop_format_field32_get(unit,
            RSEL1_3_DOPfmt,
            FORWARD_1_LKUP_0_VALIDf)){
        SHR_BITSET(
        (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
        bcmSwitchPktTraceLookupL2DstHit);
    }
    if (_bcm_td3_dop_format_field32_get(unit,
             L2_USER_ENTRY_DOPfmt,
             HITf) != 0) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
          bcmSwitchPktTraceLookupL2CacheHit);
    }
    if (_bcm_td3_dop_format_field32_get(unit,
            RSEL1_3_DOPfmt,
            FORWARD_2_LKUP_1_VALIDf)){
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
          bcmSwitchPktTraceLookupL3SrcHostHit);
    }
    if (_bcm_td3_dop_format_field32_get(unit,
            RSEL1_3_DOPfmt,
            FORWARD_2_LKUP_0_VALIDf)){
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
          bcmSwitchPktTraceLookupL3DestHostHit);
    }
    if (_bcm_td3_dop_format_field32_get(unit,
            RSEL1_3_DOPfmt,
            FORWARD_3_LKUP_0_VALIDf)){
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
          bcmSwitchPktTraceLookupL3DestRouteHit);
    }
    if (_bcm_td3_dop_format_field32_get(unit,
         MY_STATION_TCAM_VAL_DOPfmt,
         HITf)) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupMystationHit);
    }
    ifp_lookup_vec = _bcm_td3_dop_format_field32_get(unit,
         IFP_LOOKUP_STATUS_VECTOR_DOPfmt,
           IFP_LOOKUP_STATUS_VECTORf);

    if (ifp_lookup_vec &
        (1U << bcmSwitchPktTraceLookupL2DstHit)) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupL2DstHit);
    }

    if (ifp_lookup_vec &
        (1U << bcmSwitchPktTraceLookupForwardingVlanValid)) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupForwardingVlanValid);
    }
    if (ifp_lookup_vec &
        (1U << bcmSwitchPktTraceLookupL2SrcStatic)) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupL2SrcStatic);
    }
    if (ifp_lookup_vec &
        (1U << bcmSwitchPktTraceLookupL2CacheHit)) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupL2CacheHit);
    }
    if (ifp_lookup_vec &
         (1U << bcmSwitchPktTraceLookupL2SrcMiss)) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupL2SrcMiss);
    }
    if (ifp_lookup_vec &
        (1U << bcmSwitchPktTraceLookupDosAttack)) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupDosAttack);
    }
    if (ifp_lookup_vec &
        (1U << bcmSwitchPktTraceLookupMplsLabel1Hit)) {
       SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupMplsLabel1Hit);
    }
    if (ifp_lookup_vec &
        (1U << bcmSwitchPktTraceLookupMplsLabel2Hit)) {
       SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupMplsLabel2Hit);
    }
    if (ifp_lookup_vec &
        (1U << bcmSwitchPktTraceLookupMplsTerminated)) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupMplsTerminated);
    }
    if (ifp_lookup_vec &
        (1U <<  bcmSwitchPktTraceLookupSecondVlanTranslationHit)) {
        SHR_BITSET(
         (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap),
         bcmSwitchPktTraceLookupSecondVlanTranslationHit);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_pkt_resolution_get
 * Purpose:
 *      Packet Resolution  processing
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_pkt_resolution_get(int unit,
                bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    uint32 pkt_rsl = 0;

    if (pkt_trace_info == NULL) {
        return BCM_E_INTERNAL;
    }
    pkt_rsl = _bcm_td3_dop_format_field32_get(unit,
                     RSEL1_2_DOPfmt,
                     PKT_RESOLUTION_VECTORf);

    pkt_trace_info->pkt_trace_resolution = pkt_rsl;
    return BCM_E_NONE;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_td3_pkt_trace_ecmp_resolution_get
 * Purpose:
 *      ECMP Resolution  processing
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_ecmp_resolution_get(int unit,
                bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    uint16 ecmp_id_1 = 0;
    uint16 ecmp_id_2 = 0;
    uint16 ecmp_offset_1 = 0;
    uint16 ecmp_offset_2 = 0;
    uint16 ecmp_hash_val_1 = 0;
    uint16 ecmp_hash_val_2 = 0;
    uint32 ecmp_ptr = 0;
    uint32 ecmp_mem_index = 0;
    uint32 ecmp_flag = 0;
    uint32 nhop_ecmp_group = 0;
    uint32 destination = 0;
    uint8  count1 = 0;
    uint8  count2 = 0;
    uint8  nh_flag = 0;
    ecmp_count_entry_t      ecmpc_entry;
    ecmp_entry_t            ecmp_entry;
    uint32 first_ecmp_rsl = 0;
    uint32 second_ecmp_rsl = 0;

    if (pkt_trace_info == NULL) {
        return BCM_E_INTERNAL;
    }
    /* ECMP GROUP 1 */
    ecmp_id_1 = _bcm_td3_dop_format_field32_get(unit,
                     RSEL2_FIRST_ECMP_DOPfmt,
                     ECMP1_GROUPf);

    ecmp_hash_val_1 = _bcm_td3_dop_format_field32_get(unit,
                       RSEL2_FIRST_ECMP_DOPfmt,
                       ECMP1_HASH_VALUEf);

    first_ecmp_rsl = _bcm_td3_dop_format_field32_get(unit,
                     RSEL2_FIRST_ECMP_DOPfmt,
                     FIRST_ECMP_RESOLUTION_DONEf);
    /* ECMP GROUP 2 */
    ecmp_id_2 = _bcm_td3_dop_format_field32_get(unit,
                     RSEL2_SECOND_ECMP_DOPfmt,
                     ECMP2_GROUPf);

    ecmp_hash_val_2 = _bcm_td3_dop_format_field32_get(unit,
                      RSEL2_SECOND_ECMP_DOPfmt,
                      ECMP2_HASH_VALUEf);

    second_ecmp_rsl = _bcm_td3_dop_format_field32_get(unit,
                     RSEL2_SECOND_ECMP_DOPfmt,
                     SECOND_ECMP_RESOLUTION_DONEf);
    /* ECMP ID 1 Resolution */
    if (first_ecmp_rsl) {
        sal_memset(&ecmpc_entry, 0, sizeof(ecmpc_entry));
        sal_memset(&ecmp_entry, 0, sizeof(ecmp_entry));
        BCM_IF_ERROR_RETURN
                (READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                        ecmp_id_1, &ecmpc_entry));

        ecmp_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry,
                                                      BASE_PTRf);
        count1 = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry,
                                                      COUNTf);
        ecmp_offset_1 = ((ecmp_hash_val_1) % (count1 + 1)) &
                         ((1 << (soc_format_field_length(
                                 unit,
                                 RSEL2_FIRST_ECMP_DOPfmt,
                                 ECMP1_OFFSETf))) -1) ;

        ecmp_mem_index = (ecmp_ptr + ecmp_offset_1) &
                 (soc_mem_index_count(unit, L3_ECMPm) - 1);

        BCM_IF_ERROR_RETURN
                (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, ecmp_mem_index, &ecmp_entry));
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_group   = ecmp_id_1;
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_group
                              += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

        /* ecmp_flag is set only when hierarchial ECMP */
        destination = soc_L3_ECMPm_field32_get(unit,
                                &ecmp_entry, DESTINATIONf);
        ecmp_flag = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                                &destination, DEST_TYPE1f);
        nh_flag   = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                                &destination, DEST_TYPE0f);
        if (ecmp_flag == BCMI_TD3_DEST_TYPE1_ECMP) {
            nhop_ecmp_group =  soc_format_field32_get(unit,
                  DESTINATION_FORMATfmt,
                  &destination,
                  ECMP_GROUPf);
            pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress  = nhop_ecmp_group;
        } else {
            if (nh_flag  == BCMI_TD3_DEST_TYPE0_NH) {
                pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress  =
                    soc_format_field32_get(
                            unit,
                            DESTINATION_FORMATfmt,
                            &destination,
                            NEXT_HOP_INDEXf);
            } else {
                pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress  =
                     soc_L3_ECMPm_field32_get(
                            unit,
                            &ecmp_entry,
                            NEXT_HOP_INDEXf);
            }
        }

        if (second_ecmp_rsl) {
            pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress
                             += BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);
        } else {
            pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress
                             += BCM_XGS3_EGRESS_IDX_MIN(unit);
        }
        pkt_trace_info->pkt_trace_hash_info.flags
                             += BCM_SWITCH_PKT_TRACE_ECMP_1;
    }

    if (second_ecmp_rsl) {
        /* ECMP ID 2 Resolution */
        sal_memset(&ecmpc_entry, 0, sizeof(ecmpc_entry));
        sal_memset(&ecmp_entry, 0, sizeof(ecmp_entry));
        ecmp_flag = 0;
        nh_flag = 0;
        destination = 0;
        BCM_IF_ERROR_RETURN
            (READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY,
                                 ecmp_id_2, &ecmpc_entry));

        ecmp_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry,
                                                  BASE_PTRf);
        count2 = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry,
                                                      COUNTf);
        ecmp_offset_2 = ((ecmp_hash_val_2) % (count2 + 1)) &
                         ((1 << (soc_format_field_length(
                                 unit,
                                 RSEL2_SECOND_ECMP_DOPfmt,
                                 ECMP2_OFFSETf))) -1) ;

        ecmp_mem_index = (ecmp_ptr + ecmp_offset_2) &
                  (soc_mem_index_count(unit, L3_ECMPm) - 1);

        BCM_IF_ERROR_RETURN
            (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, ecmp_mem_index, &ecmp_entry));

        pkt_trace_info->pkt_trace_hash_info.ecmp_2_group   = ecmp_id_2;
        pkt_trace_info->pkt_trace_hash_info.ecmp_2_group  +=
                       BCM_XGS3_MPATH_EGRESS_IDX_MIN(unit);

        destination = soc_L3_ECMPm_field32_get(unit,
                            &ecmp_entry, DESTINATIONf);
        nh_flag   = soc_format_field32_get(unit, DESTINATION_FORMATfmt,
                                &destination, DEST_TYPE0f);
        if (nh_flag  == BCMI_TD3_DEST_TYPE0_NH) {
            pkt_trace_info->pkt_trace_hash_info.ecmp_2_egress  =
                    soc_format_field32_get(
                            unit,
                            DESTINATION_FORMATfmt,
                            &destination,
                            NEXT_HOP_INDEXf);
        } else {
            pkt_trace_info->pkt_trace_hash_info.ecmp_2_egress  =
                     soc_L3_ECMPm_field32_get(
                            unit,
                            &ecmp_entry,
                            NEXT_HOP_INDEXf);
        }

        pkt_trace_info->pkt_trace_hash_info.ecmp_2_egress  +=
                               BCM_XGS3_EGRESS_IDX_MIN(unit);
        pkt_trace_info->pkt_trace_hash_info.flags
                               += BCM_SWITCH_PKT_TRACE_ECMP_2;
    }
    return BCM_E_NONE;
}
#endif

/*
 * Function:
 *      _bcm_td3_pkt_trace_hg_trunk_resolution_get
 * Purpose:
 *      HG TRUNK  Resolution  processing
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_hg_trunk_resolution_get(int unit,
                bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    uint16 hg_rsl_done = 0;
    uint32 hg_hash_val = 0;
    uint32 hg_trunk_id = 0;

    int         tgid;
    uint32      trunk_base, trunk_group_size;
    uint32      trunk_index, trunk_member_table_index;
    bcm_module_t        mod_id;
    bcm_port_t          port_id;
    bcm_gport_t         dst_member_port;
    _bcm_gport_dest_t   dest;

    hg_trunk_member_entry_t hg_trunk_member_entry;
    hg_trunk_group_entry_t  hg_tg_entry;

    bcm_trunk_chip_info_t chip_info;


    if (pkt_trace_info == NULL) {
        return BCM_E_INTERNAL;
    }

    /* HIGIG TRUNK RESOLVE*/
    hg_rsl_done =  _bcm_td3_dop_format_field32_get(unit,
                         HG_TRUNK_DOPfmt,
                         INSTR_HG_TRUNK_RESOLUTION_DONEf);
    hg_hash_val = _bcm_td3_dop_format_field32_get(unit,
                         HG_TRUNK_DOPfmt,
                         INSTR_HG_TRUNK_HASHf);
    hg_trunk_id = _bcm_td3_dop_format_field32_get(unit,
                         HG_TRUNK_DOPfmt,
                         INSTR_HG_TRUNK_IDf);

    if (hg_rsl_done) {
        tgid = hg_trunk_id;

        BCM_IF_ERROR_RETURN(READ_HG_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tgid, &hg_tg_entry));

        trunk_group_size    = soc_HG_TRUNK_GROUPm_field32_get(unit, &hg_tg_entry, TG_SIZEf);
        trunk_base          = soc_HG_TRUNK_GROUPm_field32_get(unit, &hg_tg_entry, BASE_PTRf);
        trunk_index         = hg_hash_val % (trunk_group_size + 1);
        trunk_member_table_index = (trunk_base + trunk_index) & 0xff;

        BCM_IF_ERROR_RETURN(READ_HG_TRUNK_MEMBERm(unit, MEM_BLOCK_ANY,
                            trunk_member_table_index, &hg_trunk_member_entry));
        port_id = soc_HG_TRUNK_MEMBERm_field32_get(unit,
                            &hg_trunk_member_entry, PORT_NUMf);

        if (BCM_FAILURE(bcm_esw_stk_my_modid_get(unit, &mod_id))) {
            mod_id = 0;
        }

        BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mod_id, port_id,
                                             &(dest.modid), &(dest.port)));
        dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, &dst_member_port));

        BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
        pkt_trace_info->pkt_trace_hash_info.fabric_trunk =
                            tgid + chip_info.trunk_fabric_id_min;
        pkt_trace_info->pkt_trace_hash_info.fabric_trunk_member =
                            dst_member_port;
        pkt_trace_info->pkt_trace_hash_info.flags +=
                            BCM_SWITCH_PKT_TRACE_FABRIC_TRUNK;

    } /* end hg trunk resolution */

    return BCM_E_NONE;

}

/*
 * Function:
 *      _bcm_td3_pkt_trace_ucast_lag_resolution_get
 * Purpose:
 *      Unicast LAG Resolution processing
 i* Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_ucast_lag_resolution_get(int unit,
                bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    uint32 sw_dst;
    bcm_gport_t dgpp_gport;
    _bcm_gport_dest_t   dest;
    uint16 pkt_opcode = 0;
    egr_gpp_attributes_modbase_entry_t modbase_entry;
    egr_gpp_attributes_entry_t egr_gpp_attributes;
    int base = -1;
    int egr_attr_index = -1;
    int is_trunk = -1;
    int tgid = -1;
    int mod_is_local = -1;

    pkt_opcode = _bcm_td3_dop_format_field32_get(unit,
                         RSEL2_3_DOPfmt,
                         MH_OPCODEf);

    sw_dst =  _bcm_td3_dop_format_field32_get(unit,
                         SW3_0_DOPfmt,
                         SW3_DESTINATIONf);

    if (pkt_opcode == BCM_PKT_OPCODE_UC) {
        dest.port   = (sw_dst & BYTE_MASK);
        dest.modid  =  ((sw_dst >> BYTE_SHIFT) & BYTE_MASK);

        BCM_IF_ERROR_RETURN(
            READ_EGR_GPP_ATTRIBUTES_MODBASEm(unit,
                                             MEM_BLOCK_ANY,
                                             dest.modid,
                                             &modbase_entry));


        base = soc_EGR_GPP_ATTRIBUTES_MODBASEm_field32_get(
                                             unit,
                                             &modbase_entry,
                                             BASEf);

        egr_attr_index = base + dest.port;

        BCM_IF_ERROR_RETURN(
            READ_EGR_GPP_ATTRIBUTESm(unit, MEM_BLOCK_ANY,
                                     egr_attr_index,
                                     &egr_gpp_attributes));

        is_trunk = soc_EGR_GPP_ATTRIBUTESm_field32_get(
                                            unit,
                                            &egr_gpp_attributes,
                                            ISTRUNKf);

        if (is_trunk) {
            tgid = soc_EGR_GPP_ATTRIBUTESm_field32_get(
                                            unit,
                                            &egr_gpp_attributes,
                                            TGIDf);
            dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
            BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, dest.modid,
                                     &mod_is_local));
            if (mod_is_local) {
                if (IS_ST_PORT(unit, dest.port)) {
                    dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
                }
            }
            BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, &dgpp_gport));
            pkt_trace_info->pkt_trace_hash_info.trunk        = tgid;
            pkt_trace_info->pkt_trace_hash_info.trunk_member = dgpp_gport;
            pkt_trace_info->pkt_trace_hash_info.flags += BCM_SWITCH_PKT_TRACE_TRUNK;
        }
    }
     return BCM_E_NONE;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_td3_pkt_trace_fwd_dst_resolution_get
 * Purpose:
 *      Forward Destination Resolution  processing
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_fwd_dst_resolution_get(int unit,
                bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    uint16 pkt_opcode = 0;
    uint32 sw_dst;
    uint32 l2mc_index, l3mc_index;
    uint32 fwd_dst_type;
    uint32 l3_routed;
    bcm_multicast_t mc_group;
    uint32 dvp_nh_1;
    uint32 dvp_nh_sel_1;
    uint32 dvp_nh_2;
    uint32 dvp_nh_sel_2;
    bcm_gport_t dgpp_gport;
    bcm_module_t modid;
    bcm_port_t port;
#if defined(INCLUDE_L3)
    uint32 dvp = 0;
    bcm_gport_t dvp_gport;
    int rv = BCM_E_NONE;
#endif /* INCLUDE_L3 */
    pkt_opcode = _bcm_td3_dop_format_field32_get(unit,
                         RSEL2_3_DOPfmt,
                         MH_OPCODEf);
    sw_dst =  _bcm_td3_dop_format_field32_get(unit,
                         SW3_0_DOPfmt,
                         SW3_DESTINATIONf);
    fwd_dst_type = _bcm_td3_dop_format_field32_get(unit,
                         SW3_0_DOPfmt,
                         FORWARDING_TYPEf);
    if (fwd_dst_type >= TD3_FORWARDING_TYPE_L3UC) {
        l3_routed = 1;
    } else {
        l3_routed = 0;
    }

    if (pkt_opcode == BCM_PKT_OPCODE_UC) {
        dvp_nh_1 = _bcm_td3_dop_format_field32_get(unit,
                  RSEL2_FIRST_DVP_NHI_DOPfmt,
                  DVP_NHIf);
        dvp_nh_sel_1 = _bcm_td3_dop_format_field32_get(unit,
                  RSEL2_FIRST_DVP_NHI_DOPfmt,
                  SELf);
        dvp_nh_2 = _bcm_td3_dop_format_field32_get(unit,
                  RSEL2_SECOND_DVP_NHI_DOPfmt,
                  DVP_NHIf);
        dvp_nh_sel_2 = _bcm_td3_dop_format_field32_get(unit,
                  RSEL2_SECOND_DVP_NHI_DOPfmt,
                  SELf);

        port   = (sw_dst & BYTE_MASK);
        modid  =  ((sw_dst >> BYTE_SHIFT) & BYTE_MASK);
        BCM_GPORT_MODPORT_SET(dgpp_gport, modid, port);
        pkt_trace_info->pkt_trace_fwd_dst_info.flags |=
                 BCM_SWITCH_PKT_TRACE_FWD_DST_DGPP;
        pkt_trace_info->pkt_trace_fwd_dst_info.port = dgpp_gport;

        if (l3_routed) {
            /****************
             Present
             1. DGPP which is sw_dst
             3. NH1/NH2 which ever is valid.
 */
            if ((dvp_nh_1 != 0)
                && (dvp_nh_sel_1)) {
                pkt_trace_info->pkt_trace_fwd_dst_info.egress_intf =
                        dvp_nh_1 + BCM_XGS3_EGRESS_IDX_MIN(unit);
                pkt_trace_info->pkt_trace_fwd_dst_info.flags |=
                     BCM_SWITCH_PKT_TRACE_FWD_DST_NH;
            } else if ((dvp_nh_2 != 0)
               && (dvp_nh_sel_2)) {
                pkt_trace_info->pkt_trace_fwd_dst_info.egress_intf =
                        dvp_nh_2 + BCM_XGS3_EGRESS_IDX_MIN(unit);
                pkt_trace_info->pkt_trace_fwd_dst_info.flags |=
                     BCM_SWITCH_PKT_TRACE_FWD_DST_NH;
            }

        } else {
#if defined(INCLUDE_L3)
            if ((dvp_nh_1 != 0) &&
                 (dvp_nh_sel_1)) {
                rv = _bcm_td3_pkt_trace_nh_dvp_get(unit,
                                dvp_nh_1, &dvp);
            } else if ((dvp_nh_2 != 0)
               && (dvp_nh_sel_2)) {
                rv = _bcm_td3_pkt_trace_nh_dvp_get(unit,
                                dvp_nh_2, &dvp);
            }
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            rv = _bcm_vp_encode_gport(unit, dvp, &dvp_gport);
            if (BCM_FAILURE(rv)) {
                return rv;
            }
            pkt_trace_info->pkt_trace_fwd_dst_info.flags |=
                         BCM_SWITCH_PKT_TRACE_FWD_DST_DVP;
            pkt_trace_info->pkt_trace_fwd_dst_info.dvp = dvp_gport;
#endif /* INCLUDE_L3 */
        }
    } else if (pkt_opcode == BCM_PKT_OPCODE_BC) {
        if (l3_routed) {
            pkt_trace_info->pkt_trace_fwd_dst_info.flags |=
                     BCM_SWITCH_PKT_TRACE_FWD_DST_IPMC;
            _BCM_MULTICAST_GROUP_SET(mc_group,
                      _BCM_MULTICAST_TYPE_L3,sw_dst);
            pkt_trace_info->pkt_trace_fwd_dst_info.mc_group =
                     mc_group;
        } else {
            pkt_trace_info->pkt_trace_fwd_dst_info.flags |=
                     BCM_SWITCH_PKT_TRACE_FWD_DST_VLAN;
            pkt_trace_info->pkt_trace_fwd_dst_info.fwd_vlan =
                    sw_dst;
        }
    } else if (pkt_opcode == BCM_PKT_OPCODE_MC) {
        l2mc_index =  _bcm_td3_dop_format_field32_get(unit,
                         MC_INDICES_DOPfmt,
                         L2MC_INDEXf);
        if (l2mc_index != 0) {
             /* valid l2mc index */
           if (l2mc_index != sw_dst) {
               return BCM_E_INTERNAL;
           }
           pkt_trace_info->pkt_trace_fwd_dst_info.flags |=
                     BCM_SWITCH_PKT_TRACE_FWD_DST_L2MC;
          _BCM_MULTICAST_GROUP_SET(mc_group,
                      _BCM_MULTICAST_TYPE_L2, l2mc_index);
           pkt_trace_info->pkt_trace_fwd_dst_info.mc_group =
                     mc_group;
        }
     } else if (pkt_opcode == BCM_PKT_OPCODE_IPMC) {
        l3mc_index =  _bcm_td3_dop_format_field32_get(unit,
                         MC_INDICES_DOPfmt,
                         L3_IPMC_INDEXf);
        if (l3mc_index != 0) {
            /* valida l3mc index */
           if (l3mc_index != sw_dst) {
               return BCM_E_INTERNAL;
           }
           pkt_trace_info->pkt_trace_fwd_dst_info.flags |=
                     BCM_SWITCH_PKT_TRACE_FWD_DST_IPMC;
          _BCM_MULTICAST_GROUP_SET(mc_group,
                      _BCM_MULTICAST_TYPE_L3, l3mc_index);
           pkt_trace_info->pkt_trace_fwd_dst_info.mc_group =
                     mc_group;
        }
    }
    return BCM_E_NONE;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_td3_pkt_trace_drop_reason_get
 * Purpose:
 *       Drop Reason  processing
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_drop_reason_get(int unit,
                bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    uint32  tl_dp_v = 0;
    uint32  disc_dp_v = 0;
    uint32  vlan_dp_v = 0;
    uint32 c_v[4];
    uint32 pkt_dp_v[4];
    uint32 pre_coun_v = 0;
    uint32 pre_dp_v = 0;
    uint32 vx_dp_v = 0;
    uint32 rsel2_dp_v =0;

    sal_memset(c_v, 0, sizeof(c_v));
    sal_memset(pkt_dp_v, 0, sizeof(pkt_dp_v));

    _bcm_td3_dop_format_field_get(unit,
                         COUNTER_UPDATE_VECTOR_DOPfmt,
                         COUNTER_UPDATE_VECTORf,
                         c_v);

    soc_format_field_get(unit,
             INGRESS_COUNTER_UPDATE_VECTORfmt,
             c_v,
             PKT_PROC_DROP_VECTORf,
             pkt_dp_v);

    pre_coun_v = soc_format_field32_get(unit,
             INGRESS_COUNTER_UPDATE_VECTORfmt,
             c_v,
             PRE_COUNTER_UPDATE_VECTORf);

    /* only pre drop vector */
    SHR_BITCOPY_RANGE(&pre_dp_v, 12, &pre_coun_v, 12, 20);

    vx_dp_v = soc_format_field32_get(unit,
                  PKT_PROC_DROP_VECTORfmt,
                  &pkt_dp_v,
                  VXLT_DROP_VECTORf);

    disc_dp_v  = soc_format_field32_get(unit,
                  PKT_PROC_DROP_VECTORfmt,
                  &pkt_dp_v,
                  DISCARD_PROC_DROP_VECTORf);

    vlan_dp_v = soc_format_field32_get(unit,
                  PKT_PROC_DROP_VECTORfmt,
                  &pkt_dp_v,
                  VLAN_DROP_VECTORf);

    tl_dp_v = soc_format_field32_get(unit,
                  PKT_PROC_DROP_VECTORfmt,
                  &pkt_dp_v,
                  TUNNEL_PROC_DROP_VECTORf);

    rsel2_dp_v = soc_format_field32_get(unit,
                  PKT_PROC_DROP_VECTORfmt,
                  &pkt_dp_v,
                  RSEL2_DROP_VECTORf);

    SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
              bcmSwitchPktTraceNoDrop);

    if (soc_format_field32_get(unit, TUNNEL_PROC_DROP_VECTORfmt,
             &tl_dp_v, MPLS_LABEL_MISSf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonMplsLabelLookupMiss);
    }

    if (soc_format_field32_get(unit, TUNNEL_PROC_DROP_VECTORfmt,
             &tl_dp_v, MPLS_INVALID_ACTIONf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonMplsInvalidAction);
    }

    if (soc_format_field32_get(unit, TUNNEL_PROC_DROP_VECTORfmt,
             &tl_dp_v, MPLS_INVALID_PAYLOADf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonMplsInvalidPayload);
    }

    if (soc_format_field32_get(unit, TUNNEL_PROC_DROP_VECTORfmt,
             &tl_dp_v, MPLS_CONTROL_PKTf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonMplsInvalidControlWord);
    }

#if 0
    if (soc_format_field32_get(unit, TUNNEL_PROC_DROP_VECTORfmt,
             &tl_dp_v, TUNNEL_TTL_CHECK_FAILf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonMplsTtlCheckFail);
    }
#endif

    if (soc_format_field32_get(unit, TUNNEL_PROC_DROP_VECTORfmt,
             &tl_dp_v, TUNNEL_OBJECT_VALIDATION_FAILf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnelObjectValidationFail);
    }

    if (soc_format_field32_get(unit, TUNNEL_PROC_DROP_VECTORfmt,
             &tl_dp_v, TUNNEL_TTL_CHECK_FAILf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnelTTLError);
    }

    if (soc_format_field32_get(unit, TUNNEL_PROC_DROP_VECTORfmt,
             &tl_dp_v, TUNNEL_INTERFACE_CHECK_FAILf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnelInterfaceCheckFail);
    }

    if (soc_format_field32_get(unit, TUNNEL_PROC_DROP_VECTORfmt,
             &tl_dp_v, TUNNEL_SHIM_HDR_ERRORf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnelShimHeaderError);
    }

    if (soc_format_field32_get(unit, VXLT_DROP_VECTORfmt,
                &vx_dp_v, TUNNEL_ADAPT_1_LOOKUP_MISSf))  {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnelAdapt1LookupMiss);
    }

    if (soc_format_field32_get(unit, VXLT_DROP_VECTORfmt,
                &vx_dp_v, TUNNEL_ADAPT_2_LOOKUP_MISSf))  {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnelAdapt2LookupMiss);
    }

    if (soc_format_field32_get(unit, VXLT_DROP_VECTORfmt,
                &vx_dp_v, TUNNEL_ADAPT_3_LOOKUP_MISSf))  {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnelAdapt3LookupMiss);
    }

    if (soc_format_field32_get(unit, VXLT_DROP_VECTORfmt,
                &vx_dp_v, TUNNEL_ADAPT_4_LOOKUP_MISSf))  {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnelAdapt4LookupMiss);
    }

    if (soc_format_field32_get(unit, RSEL2_DROP_VECTORfmt,
                &rsel2_dp_v, TUNNEL_ERRORf))  {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnelError);
    }

    if (soc_format_field32_get(unit, PKT_PROC_DROP_VECTORfmt,
             &pkt_dp_v, TUNNEL_DECAP_ECNf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnlDecapEcnError);
    }

    if (soc_format_field32_get(unit, DISCARD_PROC_DROP_VECTORfmt,
             &disc_dp_v, HIGIG_LOOPBACKf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonHigigLoopback);
    }

    if (soc_format_field32_get(unit, DISCARD_PROC_DROP_VECTORfmt,
             &disc_dp_v, HIGIG_HDR_ERRORf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonHigigUnknownHeader);
    }

    if (soc_format_field32_get(unit, DISCARD_PROC_DROP_VECTORfmt,
             &disc_dp_v, HIGIG_UNKNOWN_OPCODEf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonHigigUnknownOpcode);
    }

    if (soc_format_field32_get(unit, DISCARD_PROC_DROP_VECTORfmt,
             &disc_dp_v, LAG_FAIL_LOOPBACKf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonLagFailLoopback);
    }

    if (soc_format_field32_get(unit, DISCARD_PROC_DROP_VECTORfmt,
             &disc_dp_v, MACSA_EQUALS_DAf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonL2SrcEqualL2Dst);
    }

    if (soc_format_field32_get(unit, DISCARD_PROC_DROP_VECTORfmt,
             &disc_dp_v, NIV_VNTAG_PRESENTf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonNivVntagPresent);
    }

    if (soc_format_field32_get(unit, DISCARD_PROC_DROP_VECTORfmt,
             &disc_dp_v, NIV_VNTAG_NOT_PRESENTf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonNivVntagNotPresent);
    }

    if (soc_format_field32_get(unit, DISCARD_PROC_DROP_VECTORfmt,
             &disc_dp_v, NIV_VNTAG_FORMATf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonNivVntagFormat);
    }

    if (soc_format_field32_get(unit, PRE_COUNTER_UPDATE_VECTORfmt,
             &pre_coun_v, RIPHE4f)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonIpv4HeaderError);
    }

    if (soc_format_field32_get(unit, PRE_COUNTER_UPDATE_VECTORfmt,
             &pre_coun_v, RIPHE6f)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonIpv6HeaderError);
    }

    if (soc_format_field32_get(unit, PRE_COUNTER_UPDATE_VECTORfmt,
             &pre_coun_v, PARITYDf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonParityError);
    }

    if (soc_format_field32_get(unit, PKT_PROC_DROP_VECTORfmt,
             &pkt_dp_v, BPDUf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonBpdu);
    }

    if (soc_format_field32_get(unit, PKT_PROC_DROP_VECTORfmt,
             &pkt_dp_v, TUNNEL_DECAP_ECNf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTunnlDecapEcnError);
    }

    if (soc_format_field32_get(unit, PKT_PROC_DROP_VECTORfmt,
             &pkt_dp_v, L2_RPF_CHECK_FAILf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonRpfCheckFail);
    }

    if (soc_format_field32_get(unit, VLAN_DROP_VECTORfmt,
             &vlan_dp_v, INVALID_VLANf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonVlanNotValid);
    }

    if (soc_format_field32_get(unit, VLAN_DROP_VECTORfmt,
             &vlan_dp_v, ENIFILTERf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonIngressPortNotInVlanMember);
    }

    if (soc_format_field32_get(unit, VLAN_DROP_VECTORfmt,
             &vlan_dp_v, INVALID_TPIDf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonTpidMismatch);
    }

    if (soc_format_field32_get(unit, VXLT_DROP_VECTORfmt,
                &vx_dp_v, VFPf))  {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonStageLookup);
    }

    if (soc_format_field32_get(unit, RSEL2_DROP_VECTORfmt,
                &rsel2_dp_v, IFP_DROPf))  {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonStageIngress);
    }

    if (soc_format_field32_get(unit, RSEL2_DROP_VECTORfmt,
                &rsel2_dp_v, URPF_CHECK_FAILf)) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                   bcmSwitchPktTraceDropReasonRpfCheckFail);
    }
    if (tl_dp_v || disc_dp_v || vlan_dp_v ||
        vx_dp_v || pre_dp_v || pkt_dp_v[0] ||
        pkt_dp_v[1]||  pkt_dp_v[2] ||  pkt_dp_v[3] ||
        rsel2_dp_v) {
        SHR_BITSET(pkt_trace_info->pkt_trace_drop_reason,
                    bcmSwitchPktTraceDropReasonInternal);
        SHR_BITCLR(pkt_trace_info->pkt_trace_drop_reason,
                    bcmSwitchPktTraceNoDrop);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_stp_state_get
 * Purpose:
 *       STP state processing
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_stp_state_get(int unit,
                bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
     uint32 hw_stp_st = 0;
     uint32 stp_state = BCM_STG_STP_DISABLE;

     hw_stp_st = _bcm_td3_dop_format_field32_get(unit,
                         FWD1_1_DOPfmt,
                         SPST_INGRESSf);

     switch (hw_stp_st) {
         case 0:
              stp_state = BCM_STG_STP_DISABLE;
              break;
         case 1:
              stp_state = BCM_STG_STP_BLOCK;
              break;
         case 2:
              stp_state = BCM_STG_STP_LEARN;
              break;
         case 3:
              stp_state = BCM_STG_STP_FORWARD;
              break;
         default:
              return BCM_E_INTERNAL;
    }

    pkt_trace_info->pkt_trace_stp_state = stp_state;
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_dop_parse_hash_init
 * Purpose:
 *      DOP parsing.
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_dop_parse_hash_init(int unit,
                                 uint32 block,
                                 uint32 *buf)
{
    uint32 resp_word;
    uint16 dop_id = 0;
    uint16 data_valid = 0;
    uint16 data_phases = 0;
    soc_format_t format_id = 0;
    uint16 format_len= 0;
    uint32 key = 0;
    pkt_trace_hash_info_t  *ph = NULL;
    pkt_trace_hash_node_t  *hash_list = NULL;
    pkt_trace_hash_node_t  *hash_node = NULL;
    pkt_trace_dop_format_t *dop_format = NULL;
    pkt_trace_dop_format_t *fptr = NULL;
    uint32 *entry;
    uint32 *dop_ptr = NULL;
    uint32 temp_entry[SOC_MAX_MEM_WORDS * 2];

    ph = pkt_trace_hash_info[unit];

    if (ph->pkt_trace_hash_list == NULL) {
        return BCM_E_INTERNAL;
    }
    hash_list = ph->pkt_trace_hash_list;

    if ((block != DOP_INGRESS) &&
        (block != DOP_EGRESS)) {
        return BCM_E_INTERNAL;
    }

    dop_ptr = buf;
    do {
        /* Read the first word of the dop_data
         * response word */
        sal_memcpy(&resp_word, dop_ptr, sizeof(resp_word));
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
            (BSL_META_U(unit, "\n resp_word 0x%x\n"),resp_word));
        dop_ptr++;
        dop_id = soc_format_field32_get(unit,
                    DOP_COLLECTION_RESP_WORDfmt,
                    &resp_word, DOP_IDf);
        data_phases = soc_format_field32_get(unit,
                      DOP_COLLECTION_RESP_WORDfmt,
                      &resp_word, DATA_PHASESf);
        data_valid = soc_format_field32_get(unit,
                      DOP_COLLECTION_RESP_WORDfmt,
                      &resp_word, DATA_VALIDf);

        LOG_VERBOSE(BSL_LS_BCM_COMMON,
          (BSL_META_U(unit,
          "\n dop_id %d data_phases %d data_valid %d\n"),
          dop_id, data_phases, data_valid));
        if (dop_id == 0) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
            (BSL_META_U(unit, "\n INVALID DOP_ID: %d\n"), dop_id));
            
            return BCM_E_NONE;
        }
        BCM_IF_ERROR_RETURN(
            soc_dop_format_get(unit, dop_id, 0,/*dop_ext for ingress */
                    &format_len, &format_id));
        if (format_len != data_phases) {
            LOG_ERROR(BSL_LS_BCM_COMMON,
            (BSL_META_U(unit, "\n DOP_ID %d - Mismatch in DOP Length. "
              "\n Data_phases %d format_len %d\n"),
             dop_id, data_phases, format_len));
            
        }

        sal_memset(temp_entry, 0, sizeof(temp_entry));
        entry = soc_cm_salloc(unit, WORDS2BYTES(data_phases),
                             "dop format entry");
        if (entry == NULL) {
            return BCM_E_MEMORY;
        }

        sal_memcpy(temp_entry, dop_ptr, WORDS2BYTES(data_phases));
        _bcm_esw_pkt_trace_dop_data_swap(unit, data_phases, temp_entry, entry);
        dop_ptr += data_phases;

        BCM_IF_ERROR_RETURN(
             _bcm_pkt_trace_format_hash_key_get(unit, format_id, &key));

        LOG_VERBOSE(BSL_LS_BCM_COMMON,
            (BSL_META_U(unit,
            "\n HASH  format ID %d Key  %d\n"),format_id, key));
        hash_node = &hash_list[key];

        dop_format = (pkt_trace_dop_format_t *) soc_cm_salloc(unit,
                             sizeof(pkt_trace_dop_format_t),
                             "dop format ");
        if (dop_format == NULL){
            return BCM_E_MEMORY;
        }

        sal_memset(dop_format, 0, sizeof(pkt_trace_dop_format_t));
        dop_format->next = NULL;

        dop_format->entry = entry;
        dop_format->format = format_id;
        dop_format->data_phases = format_len;
        dop_format->dop_id = dop_id;

        if (hash_node->entry_num == 0) {
            hash_node->ft_entry = dop_format;
        } else {
             fptr = hash_node->ft_entry;
             if (fptr == NULL) {
                 return BCM_E_INTERNAL;
             }
             dop_format->next = (uint32 *)fptr;
             hash_node->ft_entry = dop_format;
        }

        hash_node->entry_num++;
    } while ((dop_ptr != NULL) &&
            (dop_id < MAX_ING_DOP_FORMATS(unit)));
    return BCM_E_NONE;
}

STATIC
int _bcm_td3_pkt_trace_hash_info_alloc(int unit, pkt_trace_hash_info_t **ph)
{
    pkt_trace_hash_info_t *hash_info;

    hash_info = sal_alloc(sizeof(pkt_trace_hash_info_t),
              "pkt_trace_hash_info_t");
    if (hash_info == NULL) {
       goto alloc_clean_up;
    }
    hash_info->pkt_trace_hash_list = sal_alloc(
        (sizeof(pkt_trace_hash_node_t) *MAX_DOP_FORMATS(unit)),
        "pkt_trace_hash_list");
    if (hash_info->pkt_trace_hash_list == NULL) {
       goto alloc_clean_up;
    }

    sal_memset(hash_info->pkt_trace_hash_list, 0,
            (sizeof(pkt_trace_hash_node_t) *MAX_DOP_FORMATS(unit)));
    *ph = hash_info;
    return 0;
alloc_clean_up:
    if (hash_info) {
        if (hash_info->pkt_trace_hash_list) {
            sal_free(hash_info->pkt_trace_hash_list);
            hash_info->pkt_trace_hash_list= NULL;
        }
        sal_free(hash_info);
        hash_info = NULL;
    }
    return BCM_E_MEMORY;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_dop_parse_init
 * Purpose:
 *      DOP parsing init.
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_dop_parse_init(int unit)
{
    pkt_trace_hash_info_t *ph = NULL;

    ph = pkt_trace_hash_info[unit];
    if (ph == NULL) {
        BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_hash_info_alloc(unit, &ph));
        ph->unit = unit;
        pkt_trace_hash_info[unit] = ph;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_dop_parse_deinit
 * Purpose:
 *      DOP parsing deinit
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
void _bcm_td3_pkt_trace_dop_parse_deinit(int unit)
{
    pkt_trace_hash_info_t *ph = pkt_trace_hash_info[unit];

    if (ph != NULL) {
        if (ph->pkt_trace_hash_list != NULL) {
            sal_free(ph->pkt_trace_hash_list);
            ph->pkt_trace_hash_list = NULL;
        }
        sal_free(ph);
        pkt_trace_hash_info[unit] = NULL;
    }
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_dop_parse_deinit
 * Purpose:
 *      DOP parsing deinit.
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
void _bcm_td3_pkt_trace_dop_parse_hash_deinit(int unit)
{
    pkt_trace_hash_info_t *ph = NULL;
    pkt_trace_hash_node_t  *hash_node = NULL;
    pkt_trace_dop_format_t *fptr = NULL;
    pkt_trace_dop_format_t *tmp = NULL;
    pkt_trace_hash_node_t *hash_list = NULL;
    int i = 0;

    ph = pkt_trace_hash_info[unit];
    if (ph->pkt_trace_hash_list != NULL) {
        hash_list = ph->pkt_trace_hash_list;
        for (i = 0; i < MAX_DOP_FORMATS(unit); i++) {
            hash_node = &hash_list[i];
            if ((hash_node->ft_entry == NULL) ||
                (!hash_node->entry_num)){
                continue;
            }
            for (fptr = hash_node->ft_entry;
                 fptr != NULL;) {
                 tmp = fptr;
                 fptr = (pkt_trace_dop_format_t *) fptr->next;
                 soc_cm_sfree(unit, tmp->entry);
                 tmp->entry = NULL;
                 soc_cm_sfree(unit, tmp);
                 tmp = NULL;
            }
        }
    }
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_info_processing
 * Purpose:
 *      Packet trace processing for the
 *      processed data.
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
STATIC
int _bcm_td3_pkt_trace_info_processing(int unit,
                              bcm_switch_pkt_trace_info_t *pkt_trace_info)
{

    /*****************************************************
     1. Read from the DOP MEM entrie chunk per ingress
          egress pipeline
     2. Process each dop _id.
     3. Get the format per block
     4. Process the data.
 */
    uint32 *ing_dop_tbl_chunk  = NULL;
    soc_mem_t mem = ING_DOP_STORAGE_MEMm;
    int rv = BCM_E_NONE;
    int entry_dw, entry_sz, indexes;
    uint32 buf_size = 0;
    uint32 data_size = 0;
    int max_idx, min_idx;

    entry_dw = soc_mem_entry_words(unit, mem);
    entry_sz = entry_dw * sizeof(uint32);

    min_idx = soc_mem_index_min(unit, mem);
    max_idx = soc_mem_index_max(unit, mem);
    indexes = (max_idx - min_idx) + 1;

    buf_size = entry_sz * indexes;

    /* initalize the EGRESS DOPs */
    BCM_IF_ERROR_RETURN(
        _bcm_td3_pkt_trace_dop_parse_init(unit));

    LOG_VERBOSE(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
           "Packet trace information processing - START")));

    ing_dop_tbl_chunk =  (uint32 *) soc_cm_salloc(unit, buf_size,
                               "flow match traverse");
    if (NULL == ing_dop_tbl_chunk) {
        return BCM_E_MEMORY;
    }

    /* Collect the ingress dop_storage memory and process */
    rv = _bcm_td3_pkt_trace_dop_buffer_collect(unit,
                                 DOP_INGRESS,
                                 buf_size,
                                 ing_dop_tbl_chunk,
                                 &data_size);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }


    rv =  _bcm_td3_pkt_trace_dop_parse_hash_init(unit,
                                     DOP_INGRESS,
                                     ing_dop_tbl_chunk);

    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

    rv =  _bcm_td3_pkt_trace_lookup_status_get(unit,
                                 pkt_trace_info);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

    rv =  _bcm_td3_pkt_trace_pkt_resolution_get(unit,
                                  pkt_trace_info);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }


    /*HASH Resolution */
#ifdef INCLUDE_L3
    rv =   _bcm_td3_pkt_trace_ecmp_resolution_get(unit,
                                 pkt_trace_info);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

#endif /* INCLUDE_L3 */
    rv = _bcm_td3_pkt_trace_hg_trunk_resolution_get(unit,
                                 pkt_trace_info);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

    rv  = _bcm_td3_pkt_trace_ucast_lag_resolution_get(unit,
                                 pkt_trace_info);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

    /* END - HASH Resolution */
#ifdef INCLUDE_L3
    rv =  _bcm_td3_pkt_trace_fwd_dst_resolution_get(unit,
                                 pkt_trace_info);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

#endif /* INCLUDE_L3 */
    rv = _bcm_td3_pkt_trace_drop_reason_get(unit,
                                 pkt_trace_info);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

    rv = _bcm_td3_pkt_trace_stp_state_get(unit,
                                 pkt_trace_info);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

clean_up:
    _bcm_td3_pkt_trace_dop_parse_hash_deinit(unit);
    _bcm_td3_pkt_trace_dop_parse_deinit(unit);

    if (ing_dop_tbl_chunk != NULL) {
        soc_cm_sfree(unit, ing_dop_tbl_chunk);
    }
    return rv;

}

/*
 * Function:
 *      _bcm_td3_pkt_trace_raw_data_get
 * Purpose:
 *      read results ptr buffer for
 *      visibility packet and store into into
 *      raw data.
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_td3_pkt_trace_raw_data_get(int unit,
                      uint32 *raw_data_size,
                      uint8 *raw_data)

{
    uint32 *ing_dop_tbl_chunk  = NULL;
    uint32 *egr_dop_tbl_chunk  = NULL;
    soc_mem_t mem;
    int rv = BCM_E_NONE;
    int entry_sz, indexes;
    uint32 ing_buf_size = 0;
    uint32 egr_buf_size = 0;
    int max_idx, min_idx;
    uint32 ing_data_size = 0;
    uint32 egr_data_size = 0;

    if ((raw_data == NULL) ||
        (raw_data_size == NULL)) {
        return BCM_E_INTERNAL;
    }

    /* INGRESS DOP STORAGE MEM */
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_reset(unit, DOP_INGRESS));
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_collect(unit, DOP_INGRESS));

    mem = ING_DOP_STORAGE_MEMm;
    entry_sz = 0;
    min_idx = max_idx = indexes = 0;
    entry_sz = (BCM_PKT_TRC_DOP_DATA_WORD_LEN * sizeof(uint32));
    min_idx = soc_mem_index_min(unit, mem);
    max_idx = soc_mem_index_max(unit, mem);
    indexes = (max_idx - min_idx) + 1;
    ing_buf_size = entry_sz * indexes;

    ing_dop_tbl_chunk =  soc_cm_salloc(unit, ing_buf_size,
                               "flow match traverse");
    if (NULL == ing_dop_tbl_chunk) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }
    /* Collect the ingress dop_storage memory and process */
    rv = _bcm_td3_pkt_trace_dop_buffer_collect(unit,
                                 DOP_INGRESS,
                                 ing_buf_size,
                                 ing_dop_tbl_chunk,
                                 &ing_data_size);

    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

    mem = EGR_DOP_STORAGE_MEMm;
    entry_sz = 0;
    min_idx = max_idx = indexes = 0;
    entry_sz = (BCM_PKT_TRC_DOP_DATA_WORD_LEN * sizeof(uint32));
    min_idx = soc_mem_index_min(unit, mem);
    max_idx = soc_mem_index_max(unit, mem);
    indexes = (max_idx - min_idx) + 1;
    egr_buf_size = entry_sz * indexes;
    egr_dop_tbl_chunk =  soc_cm_salloc(unit, egr_buf_size,
                               "flow match traverse");
    if (NULL == egr_dop_tbl_chunk) {
        rv = BCM_E_MEMORY;
        goto clean_up;
    }

    /* INGRESS DOP STORAGE MEM */
    rv = _bcm_td3_pkt_trace_dop_reset(unit, DOP_EGRESS);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }
    rv = _bcm_td3_pkt_trace_dop_collect(unit, DOP_EGRESS);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

    /* Collect the ingress dop_storage memory and process */
    rv = _bcm_td3_pkt_trace_dop_buffer_collect(unit,
                                  DOP_EGRESS,
                                  egr_buf_size,
                                  egr_dop_tbl_chunk,
                                  &egr_data_size);
    if (BCM_FAILURE(rv)) {
       goto clean_up;
    }

    if (*raw_data_size < (ing_buf_size + egr_buf_size)) {
        LOG_ERROR(BSL_LS_BCM_COMMON, (BSL_META_U(unit,
           "\n Insufficient raw data buffer size %d \
           Expected raw data buffer size %d"), *raw_data_size,
           (ing_buf_size + egr_buf_size)));
        rv = BCM_E_PARAM;
        goto clean_up;
    }

    sal_memcpy(raw_data, ing_dop_tbl_chunk, ing_data_size);
    *raw_data_size = ing_data_size;

    sal_memcpy((raw_data + ing_data_size),
                 egr_dop_tbl_chunk, egr_data_size);
    *raw_data_size += egr_data_size;

clean_up:
    if (ing_dop_tbl_chunk != NULL) {
        soc_cm_sfree(unit, ing_dop_tbl_chunk);
    }
    if (egr_dop_tbl_chunk != NULL) {
        soc_cm_sfree(unit, egr_dop_tbl_chunk);
    }

    return rv;
}

int _bcm_td3_pkt_trace_dop_config_set(int unit)
{

    /* INGRESS DOP STORAGE MEM */
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_reset(unit, DOP_INGRESS));
    BCM_IF_ERROR_RETURN(
        _bcm_td3_pkt_trace_dop_init(unit, DOP_INGRESS, 1));
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_reset(unit, DOP_INGRESS));
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_clear(unit, DOP_INGRESS));
    /* EGRESS DOP STORAGE MEM */
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_reset(unit, DOP_EGRESS));
    BCM_IF_ERROR_RETURN(
        _bcm_td3_pkt_trace_dop_init(unit, DOP_EGRESS, 1));
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_reset(unit, DOP_EGRESS));
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_clear(unit, DOP_EGRESS));

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td3_pkt_trace_info_get
 * Purpose:
 *      read results ptr buffer for
 *      visibility packet process and
 *      store into pkt_trace_info
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_td3_pkt_trace_info_get(int unit,
                      bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    /* INGRESS DOP STORAGE MEM */
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_reset(unit, DOP_INGRESS));
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_collect(unit, DOP_INGRESS));
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_info_processing(unit, pkt_trace_info));

    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcm_td3_pkt_trace_src_port_set
 * Purpose:
 *      set the source port of
 *      visiblity packet
 *      this must be called before visibilty packet
 *      is sent to set the pipe to read the resuls
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_td3_pkt_trace_src_port_set(int unit, uint32 logical_src_port)
{
    soc_info_t  *si;
    si = &SOC_INFO(unit);

    td3_pkt_trace_port_info[unit].pkt_trace_src_logical_port = logical_src_port;
    td3_pkt_trace_port_info[unit].pkt_trace_src_pipe =
                                      si->port_pipe[logical_src_port];

    return 0;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_src_port_get
 * Purpose:
 *      get the source port of
 *      visiblity packet
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_td3_pkt_trace_src_port_get(int unit)
{
    return td3_pkt_trace_port_info[unit].pkt_trace_src_logical_port;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_src_pipe_get
 * Purpose:
 *      get the ingress pipe of
 *      visiblity packet
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_td3_pkt_trace_src_pipe_get(int unit)
{
    return td3_pkt_trace_port_info[unit].pkt_trace_src_pipe;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_cpu_profile_init
 * Purpose:
 *      pre-configure CPU_PKT_PROFILE_1,2 Register according to
 *      index : drop_egr | dropmmu | ifp  | no learn
 *       0       0           0        0      0
 *       1       0           0        0      1
 *       2       0           0        1      0
 *       3       0           0        1      1
 *       4       0           1        0      0
 *       5       0           1        1      1
 *       6       1           0        0      0
 *       7       1           0        1      1
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_td3_pkt_trace_cpu_profile_init(int unit) {
    /* pkt_profile_1 contains control of IFP and Learning */
    uint32  pkt_profile_1_w = 0xACAA;
    /* pkt_profile_2 contains control of forwarding */
    uint32  pkt_profile_2_w = 0xC030;

    if (SOC_REG_IS_VALID(unit, CPU_PKT_PROFILE_1r)) {
        WRITE_CPU_PKT_PROFILE_1r(unit, pkt_profile_1_w);
    }

    if (SOC_REG_IS_VALID(unit, CPU_PKT_PROFILE_2r)) {
        WRITE_CPU_PKT_PROFILE_2r(unit, pkt_profile_2_w);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_cpu_profile_set
 * Purpose:
 *      given visibility packet behavior flags
 *      setting the profile id for the next visiblity packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_td3_pkt_trace_cpu_profile_set(int unit,
                                     uint32 flags)
{
    uint32  param_flags[1] = {0};
    uint32 profile_id = 0;

    param_flags[0] = flags;
    if (((flags & BCM_PKT_TRACE_FORWARD) &&
         ((flags & BCM_PKT_TRACE_DROP_TO_MMU) ||
         (flags & BCM_PKT_TRACE_DROP_TO_EGR))) ||
         ((flags & BCM_PKT_TRACE_DROP_TO_MMU)&&
         (flags & BCM_PKT_TRACE_DROP_TO_EGR)) ) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                   (BSL_META_U(unit,
                    "\n Invalid visibility option flag's\n")));

        return BCM_E_PARAM;
    }

    if (flags & BCM_PKT_TRACE_FORWARD) {
        /*  This flag is equivalent of
             NO_MMU_DROP & NO_EGR_DROP
         */
         SHR_BITCLR_RANGE(param_flags, 2,  1);
    }

    switch (param_flags[0]) {
        /* special case only BCM_PKT_TRACE_FORWARD */
         case 0:
               profile_id = 3;
             break;
         case (BCM_PKT_TRACE_NO_IFP):
               profile_id = 1;
             break;
         case (BCM_PKT_TRACE_LEARN):
               profile_id = 2;
             break;
         case (BCM_PKT_TRACE_LEARN |
               BCM_PKT_TRACE_NO_IFP):
               profile_id = 0;
             break;
         case (BCM_PKT_TRACE_DROP_TO_MMU):
               profile_id = 5;
             break;
         case (BCM_PKT_TRACE_LEARN  |
               BCM_PKT_TRACE_NO_IFP |
               BCM_PKT_TRACE_DROP_TO_MMU):
               profile_id = 4;
             break;
         case (BCM_PKT_TRACE_DROP_TO_EGR):
               profile_id = 7;
             break;
         case (BCM_PKT_TRACE_LEARN  |
               BCM_PKT_TRACE_NO_IFP |
               BCM_PKT_TRACE_DROP_TO_EGR):
               profile_id = 6;
             break;
         default:
               LOG_ERROR(BSL_LS_BCM_COMMON,
                   (BSL_META_U(unit,
                    "\n Invalid FLAGs\n")));
             return BCM_E_PARAM;

    }
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                   (BSL_META_U(unit,
                    "\n Visibility option FLAG's %d profile Id %d \n"),
                 flags, profile_id));
    td3_cpu_pkt_profile_id[unit] = profile_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_cpu_profile_get
 * Purpose:
 *      retrieve cpu profile id to be used for
 *      the next visibilty packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_td3_pkt_trace_cpu_profile_get(int unit, uint32* profile_id)
{
    if (profile_id == NULL) {
        return BCM_E_INTERNAL;
    }
    *profile_id = td3_cpu_pkt_profile_id[unit];
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_td3_pkt_trace_info_init
 * Purpose:
 *      Initialize the td3 packet trace.
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_td3_pkt_trace_init(int unit)
{
    uint32 block = 0;

    block = DOP_INGRESS;
    /* initalize the INGRESS DOPs */
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_reset(unit, block));
    BCM_IF_ERROR_RETURN(
        _bcm_td3_pkt_trace_dop_init(unit, block, 1));


    block = DOP_EGRESS;
    BCM_IF_ERROR_RETURN(
         _bcm_td3_pkt_trace_dop_reset(unit, block));
    BCM_IF_ERROR_RETURN(
        _bcm_td3_pkt_trace_dop_init(unit, block, 1));
    return BCM_E_NONE;
}

int _bcm_td3_pkt_trace_source_trunk_map_pp_set(int unit,
                                              bcm_module_t mod_id,
                                              bcm_port_t pp_port,
                                              uint16 reset)
{
    int pipe_num;
    int loopback_port;
    source_trunk_map_modbase_entry_t modbase_entry;
    int i, lb_idx;
    uint32 src_ent[SOC_MAX_MEM_FIELD_WORDS];
    uint32 lb_src_ent[SOC_MAX_MEM_FIELD_WORDS];
    int lport_index = 0;
    int lb_lport_index = 0;
    /* Program the SOURCE_TRUNK_MAP with packet
     * processing port
     */
    SOC_IF_ERROR_RETURN(
        soc_port_pipe_get(unit, pp_port, &pipe_num));
    loopback_port = soc_loopback_lbport_num_get(unit, pipe_num);
    if (loopback_port == -1) {
        return BCM_E_PARAM;
    }
    sal_memset(src_ent, 0, sizeof(src_ent));
    sal_memset(lb_src_ent, 0, sizeof(lb_src_ent));
    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, ING_DEVICE_PORTm, MEM_BLOCK_ANY, pp_port,
         src_ent));
    lport_index =
        soc_mem_field32_get(unit, ING_DEVICE_PORTm, src_ent, LPORT_PROFILE_IDXf);

    BCM_IF_ERROR_RETURN
        (soc_mem_read(unit, ING_DEVICE_PORTm, MEM_BLOCK_ANY, loopback_port,
         lb_src_ent));
    lb_lport_index=
        soc_mem_field32_get(unit, ING_DEVICE_PORTm, lb_src_ent, LPORT_PROFILE_IDXf);

    BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, SOURCE_TRUNK_MAP_MODBASEm, MEM_BLOCK_ANY,
                mod_id, &modbase_entry));

    i = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_MODBASEm,
                    &modbase_entry, BASEf) + pp_port;
    lb_idx = soc_mem_field32_get(unit, SOURCE_TRUNK_MAP_MODBASEm,
                    &modbase_entry, BASEf) + loopback_port;
    if (!reset) {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    i, PP_PORT_NUMf, pp_port));
         BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    i, LPORT_PROFILE_IDXf, lport_index));
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    lb_idx, PP_PORT_NUMf, loopback_port));
         BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    lb_idx, LPORT_PROFILE_IDXf, lb_lport_index));
    } else {
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    i, PP_PORT_NUMf, 0));
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    i, LPORT_PROFILE_IDXf, 0));
        BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    lb_idx, PP_PORT_NUMf, 0));
         BCM_IF_ERROR_RETURN
            (soc_mem_field32_modify(unit, SOURCE_TRUNK_MAP_TABLEm,
                    lb_idx, LPORT_PROFILE_IDXf, 0));
    }
    return BCM_E_NONE;
}
#endif /* BCM_INSTRUMENTATION_SUPPORT*/
