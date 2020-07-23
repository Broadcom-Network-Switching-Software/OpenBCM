/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Flowtracker
 * Purpose: API to configure H/w ETRAP block in TH3
 */
#include <shared/bsl.h>
#include <shared/pbmp.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/format.h>
#include <soc/mcm/formatacc.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/flowtracker.h>

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/tomahawk3_dispatch.h>
#include <bcm_int/esw_dispatch.h>
#include <soc/tomahawk3.h>

/* Max possible value of int_pri */
#define _BCM_TH3_FT_INT_PRI_MAX 15

/* Max value that can be fit in the register field */
#define _BCM_TH3_FT_REG_FIELD_MAX(_unit, _reg, _field)  \
    ((1 << soc_reg_field_length((_unit), (_reg), (_field))) - 1)

/* Number of Bloom filter instances */
#define _BCM_TH3_FT_BLOOM_FILTER_NUM_INSTANCES 4

/* Number of Elephant flow tables banks */
#define _BCM_TH3_FT_FLOW_TABLE_NUM_BANKS 2

/* Number of Elephant flow tables buckets in a bank */
#define _BCM_TH3_FT_FLOW_TABLE_NUM_BANK_BUCKETS 256

/* Number of Elephant flow tables entries in a bucket */
#define _BCM_TH3_FT_FLOW_TABLE_NUM_BUCKET_ENTRIES 5

/* Max number of elephant flow entries possible */
#define _BCM_TH3_FT_FLOW_TABLE_MAX_ENTRIES       \
    (_BCM_TH3_FT_FLOW_TABLE_NUM_BANKS        *   \
     _BCM_TH3_FT_FLOW_TABLE_NUM_BANK_BUCKETS *   \
     _BCM_TH3_FT_FLOW_TABLE_NUM_BUCKET_ENTRIES)

/* Number of flows in a bank */
#define _BCM_TH3_FT_FLOW_TABLE_NUM_BANK_ENTRIES \
    (_BCM_TH3_FT_FLOW_TABLE_NUM_BANK_BUCKETS *  \
     _BCM_TH3_FT_FLOW_TABLE_NUM_BUCKET_ENTRIES)

/* Default queue draint time */
#define _BCM_TH3_FT_DEF_QUEUE_DRAIN_TIME_USECS 100

/* ETRAP statistics */
typedef struct _bcm_th3_ft_stats_s {
    /* Accumulated statistics */
    bcm_flowtracker_elephant_stats_t acc;

    /* Previous per-pipe H/w counter values */
    bcm_flowtracker_elephant_stats_t prev[_TH3_PIPES_PER_DEV];
} _bcm_th3_ft_stats_t;


/* Flowtracker per unit global info */
typedef struct _bcm_th3_ft_info_s {
    /* Mutex */
    sal_mutex_t mutex;

    /* Stats */
    _bcm_th3_ft_stats_t stats;
} _bcm_th3_ft_info_t;

_bcm_th3_ft_info_t *_bcm_th3_ft_info[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      _bcm_th3_ft_lock
 * Purpose:
 *      Take the ETRAP mutex
 * Parameters:
 *      unit             - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_th3_ft_lock(int unit)
{
    _bcm_th3_ft_info_t *ft_info = _bcm_th3_ft_info[unit];

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->mutex == NULL) {
        return BCM_E_INIT;
    }

    sal_mutex_take(ft_info->mutex, sal_mutex_FOREVER);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_unlock
 * Purpose:
 *      Release the ETRAP mutex
 * Parameters:
 *      unit             - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_th3_ft_unlock(int unit)
{
    _bcm_th3_ft_info_t *ft_info = _bcm_th3_ft_info[unit];

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (ft_info->mutex == NULL) {
        return BCM_E_INIT;
    }

    if (sal_mutex_give(ft_info->mutex) != 0) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_counters_collect
 * Purpose:
 *      Get the delta between current and previous ETRAP counter values
 * Parameters:
 *      cur             - (IN) Current 48 bit value
 *      prev            - (IN) Previous 48 bit value
 * Returns:
 *      Delta between current and previous
 */
STATIC uint64 _bcm_th3_ft_counters_delta_get(uint64 cur, uint64 prev)
{
    uint64 delta;
    uint64 wrap_amt;
    const uint8 width = 48;

        delta = cur;
    if (COMPILER_64_GE(cur, prev)) {
        COMPILER_64_SUB_64(delta, prev);
    } else {
        /* Wraparound */
        COMPILER_64_SET(wrap_amt, 1UL << (width - 32), 0);
        COMPILER_64_ADD_64(delta, wrap_amt);
        COMPILER_64_SUB_64(delta, prev);
    }

    return delta;
}

/*
 * Function:
 *      _bcm_th3_ft_counters_collect
 * Purpose:
 *      Read the per pipe counters and convert to accumulated 64 bit value
 *      without taking any locks
 * Parameters:
 *      unit             - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC void _bcm_th3_ft_counters_collect_no_lock(int unit)
{
    _bcm_th3_ft_info_t *ft_info = _bcm_th3_ft_info[unit];
    bcm_flowtracker_elephant_stats_t hw_val[_TH3_PIPES_PER_DEV];
    bcm_flowtracker_elephant_stats_t *cur, *prev, *acc;
    soc_reg_t reg;
    uint64 rval;
    uint64 delta;
    int pipe;
    int rv;

    if (ft_info == NULL) {
        return;
    }

    /* Read the H/w values into memory */
    for (pipe = 0;  pipe < _TH3_PIPES_PER_DEV; pipe ++) {
        if (SOC_PBMP_IS_NULL(PBMP_PIPE(unit, pipe))) {
            continue;
        }
        cur = &(hw_val[pipe]);

        reg = SOC_REG_UNIQUE_ACC(unit, ETRAP_FILT_EXCEED_CTRr)[pipe];
        rv = soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval);
        if (rv != SOC_E_NONE) {
            return;
        }
        cur->num_candidates_detected = rval;

        reg = SOC_REG_UNIQUE_ACC(unit, ETRAP_FLOW_INS_FAIL_CTRr)[pipe];
        rv = soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval);
        if (rv != SOC_E_NONE) {
            return;
        }
        cur->num_flow_table_insert_failures = rval;

        reg = SOC_REG_UNIQUE_ACC(unit, ETRAP_FLOW_INS_SUCCESS_CTRr)[pipe];
        rv = soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval);
        if (rv != SOC_E_NONE) {
            return;
        }
        cur->num_flow_table_insert_success = rval;

        reg = SOC_REG_UNIQUE_ACC(unit, ETRAP_FLOW_DETECT_CTRr)[pipe];
        rv = soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval);
        if (rv != SOC_E_NONE) {
            return;
        }
        cur->num_elephants_detected = rval;
    }

    /* Get the delta and update the accumulated stats */
    acc =  &(ft_info->stats.acc);
    for (pipe = 0;  pipe < _TH3_PIPES_PER_DEV; pipe ++) {
        if (SOC_PBMP_IS_NULL(PBMP_PIPE(unit, pipe))) {
            continue;
        }
        cur = &(hw_val[pipe]);
        prev = &(ft_info->stats.prev[pipe]);

        delta = _bcm_th3_ft_counters_delta_get(cur->num_candidates_detected,
                                               prev->num_candidates_detected);
        COMPILER_64_ADD_64(acc->num_candidates_detected, delta);

        delta = _bcm_th3_ft_counters_delta_get(cur->num_flow_table_insert_failures,
                                               prev->num_flow_table_insert_failures);
        COMPILER_64_ADD_64(acc->num_flow_table_insert_failures, delta);

        delta = _bcm_th3_ft_counters_delta_get(cur->num_flow_table_insert_success,
                                               prev->num_flow_table_insert_success);
        COMPILER_64_ADD_64(acc->num_flow_table_insert_success, delta);

        delta = _bcm_th3_ft_counters_delta_get(cur->num_elephants_detected,
                                               prev->num_elephants_detected);
        COMPILER_64_ADD_64(acc->num_elephants_detected, delta);
    }

    /* Copy current to previous */
    sal_memcpy(ft_info->stats.prev, hw_val,
               _TH3_PIPES_PER_DEV * sizeof(bcm_flowtracker_elephant_stats_t));

}

/*
 * Function:
 *      _bcm_th3_ft_counters_collect
 * Purpose:
 *      Read the per pipe counters and convert to accumulated 64 bit value
 * Parameters:
 *      unit             - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC void _bcm_th3_ft_counters_collect(int unit)
{
    _bcm_th3_ft_info_t *ft_info = _bcm_th3_ft_info[unit];

    if (ft_info == NULL) {
        return;
    }

    if (_bcm_th3_ft_lock(unit) != BCM_E_NONE) {
        return;
    }

    _bcm_th3_ft_counters_collect_no_lock(unit);

    (void)_bcm_th3_ft_unlock(unit);
}

/*
 * Function:
 *      _bcm_th3_ft_pkt_type_match_set
 * Purpose:
 *      Set the pkt type match criteria for elephant actions.
 * Parameters:
 *      unit             - (IN) BCM device number
 *      reg              - (IN) pkt type register
 *      pkt_type_bmp     - (IN) pkt type bitmap
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_pkt_type_match_set(int unit, soc_reg_t reg, uint32 pkt_type_bmp)
{
    uint32 rval;
    int    rv;

    rval = 0;

    if (pkt_type_bmp & BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV4_TCP) {
        soc_reg_field_set(unit, reg, &rval, IPV4_TCP_ENf, 1);
    }

    if (pkt_type_bmp & BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV4_UDP) {
        soc_reg_field_set(unit, reg, &rval, IPV4_UDP_ENf, 1);
    }

    if (pkt_type_bmp & BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV4_OTHER) {
        soc_reg_field_set(unit, reg, &rval, IPV4_OTHER_ENf, 1);
    }

    if (pkt_type_bmp & BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV6_TCP) {
        soc_reg_field_set(unit, reg, &rval, IPV6_TCP_ENf, 1);
    }

    if (pkt_type_bmp & BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV6_UDP) {
        soc_reg_field_set(unit, reg, &rval, IPV6_UDP_ENf, 1);
    }

    if (pkt_type_bmp & BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV6_OTHER) {
        soc_reg_field_set(unit, reg, &rval, IPV6_OTHER_ENf, 1);
    }

    if (pkt_type_bmp & BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_MPLS) {
        soc_reg_field_set(unit, reg, &rval, MPLS_ENf, 1);
    }

    if (pkt_type_bmp & BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_OTHER) {
        soc_reg_field_set(unit, reg, &rval, PKT_TYPE_OTHER_ENf, 1);
    }

    rv = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval);

    return rv;
}

/*
 * Function:
 *      _bcm_th3_ft_pkt_type_match_get
 * Purpose:
 *      Get the pkt type match criteria for elephant actions.
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      reg              - (IN)  pkt type register
 *      pkt_type_bmp     - (OUT) pkt type bitmap
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_pkt_type_match_get(int unit, soc_reg_t reg, uint32 *pkt_type_bmp)
{
    int    rv;
    uint32 rval;

    *pkt_type_bmp = 0;

    rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval);
    SOC_IF_ERROR_RETURN(rv);


    if (soc_reg_field_get(unit, reg, rval, IPV4_TCP_ENf)) {
        *pkt_type_bmp |= BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV4_TCP;
    }

    if (soc_reg_field_get(unit, reg, rval, IPV4_UDP_ENf)) {
        *pkt_type_bmp |= BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV4_UDP;
    }

    if (soc_reg_field_get(unit, reg, rval, IPV4_OTHER_ENf)) {
        *pkt_type_bmp |= BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV4_OTHER;
    }

    if (soc_reg_field_get(unit, reg, rval, IPV6_TCP_ENf)) {
        *pkt_type_bmp |= BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV6_TCP;
    }

    if (soc_reg_field_get(unit, reg, rval, IPV6_UDP_ENf)) {
        *pkt_type_bmp |= BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV6_UDP;
    }

    if (soc_reg_field_get(unit, reg, rval, IPV6_OTHER_ENf)) {
        *pkt_type_bmp |= BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_IPV6_OTHER;
    }

    if (soc_reg_field_get(unit, reg, rval, MPLS_ENf)) {
        *pkt_type_bmp |= BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_MPLS;
    }

    if (soc_reg_field_get(unit, reg, rval, PKT_TYPE_OTHER_ENf)) {
        *pkt_type_bmp |= BCM_FLOWTRACKER_ELEPHANT_PKT_TYPE_OTHER;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_int_pri_match_set
 * Purpose:
 *      Set the int_pri match criteria for elephant actions.
 * Parameters:
 *      unit             - (IN) BCM device number
 *      reg              - (IN) int_pri register
 *      int_pri_bmp      - (IN) int_pri bitmap
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_int_pri_match_set(int unit, soc_reg_t reg, uint32 int_pri_bmp)
{
    int    rv;

    if ((int_pri_bmp >> (_BCM_TH3_FT_INT_PRI_MAX + 1)) != 0) {
        /* Some invalid int_pri is set */
        return BCM_E_PARAM;
    }

    rv = soc_reg_field32_modify(unit, reg, REG_PORT_ANY,
                                INT_PRI_BMP_ENf, int_pri_bmp);
    return rv;
}

/*
 * Function:
 *      _bcm_th3_ft_int_pri_match_get
 * Purpose:
 *      Get the int_pri match criteria for elephant actions.
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      reg              - (IN)  int_pri register
 *      int_pri_bmp      - (OUT) int_pri bitmap
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_int_pri_match_get(int unit, soc_reg_t reg, uint32 *int_pri_bmp)
{
    uint32 rval = 0;
    int    rv;

    rv = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval);
    SOC_IF_ERROR_RETURN(rv);

    *int_pri_bmp = soc_reg_field_get(unit, reg, rval, INT_PRI_BMP_ENf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_port_match_set
 * Purpose:
 *      Set the port match criteria for elephant actions.
 * Parameters:
 *      unit             - (IN) BCM device number
 *      mem              - (IN) pbmp memory
 *      field            - (IN) pbmp field
 *      pbmp             - (IN) pbmp
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_port_match_set(int unit, soc_mem_t mem,
                           soc_field_t field, bcm_pbmp_t pbmp)
{
    uint32     entry_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32     bmp[_SHR_PBMP_WORD_MAX] = {0};
    bcm_port_t port;
    int        rv;

    BCM_PBMP_ITER(pbmp, port) {
        bmp[port / 32] |= (1 << (port % 32));
    }

    sal_memset(entry_buf, 0, sizeof(entry_buf));
    soc_mem_field_set(unit, mem, entry_buf, field, bmp);

    /* coverity[negative_returns] */
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, 0, entry_buf);
    return rv;
}

/*
 * Function:
 *      _bcm_th3_ft_port_match_get
 * Purpose:
 *      Get the port match criteria for elephant actions.
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      mem              - (IN)  pbmp memory
 *      field            - (IN)  pbmp field
 *      pbmp             - (OUT) pbmp
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_port_match_get(int unit, soc_mem_t mem,
                           soc_field_t field, bcm_pbmp_t *pbmp)
{
    uint32     entry_buf[SOC_MAX_MEM_FIELD_WORDS] = {0};
    uint32     bmp[_SHR_PBMP_WORD_MAX] = {0};
    int        i, shift;
    int        rv;

    sal_memset(entry_buf, 0, sizeof(entry_buf));
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, 0, entry_buf);
    SOC_IF_ERROR_RETURN(rv);

    sal_memset(bmp, 0, sizeof(bmp));
    soc_mem_field_get(unit, mem, entry_buf, field, bmp);

    i     = 0;
    shift = 0;
    BCM_PBMP_CLEAR(*pbmp);
    while (i < _SHR_PBMP_WORD_MAX) {
        if ((bmp[i] >> shift) & 0x1) {
            BCM_PBMP_PORT_ADD(*pbmp, i * 32 + shift);
        }
        shift++;
        if (shift == 32) {
            i++;
            shift = 0;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_lookup_match_set
 * Purpose:
 *      Set the match criteria for elephant lookup.
 * Parameters:
 *      unit             - (IN) BCM device number
 *      match_types      - (IN) Match types
 *      match_data       - (IN) Match data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_lookup_match_set(int unit,
                             uint32 match_types,
                             bcm_flowtracker_elephant_match_data_t *match_data)
{
    int rv;
    uint32 valid_match_types = (BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP |
                                BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP  |
                                BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INGRESS_PBMP);

    if ((match_types & ~valid_match_types) != 0) {
        return BCM_E_PARAM;
    }

    if (match_types & BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP) {
        rv = _bcm_th3_ft_pkt_type_match_set(unit, ETRAP_LKUP_EN_PKT_TYPEr,
                                            match_data->pkt_type_bmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (match_types & BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP) {
        rv = _bcm_th3_ft_int_pri_match_set(unit, ETRAP_LKUP_EN_INT_PRIr,
                                           match_data->int_pri_bmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (match_types & BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INGRESS_PBMP) {
        rv = _bcm_th3_ft_port_match_set(unit, ETRAP_LKUP_EN_ING_PORTm,
                                        ING_PORT_BMP_ENf,
                                        match_data->ingress_pbmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_lookup_match_get
 * Purpose:
 *      Get the match criteria for elephant lookup.
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      match_types      - (IN)  Match types
 *      match_data       - (OUT) Match data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_lookup_match_get(int unit,
                             uint32 *match_types,
                             bcm_flowtracker_elephant_match_data_t *match_data)
{
    int rv;
    int port_count;

    *match_types = 0;

    rv = _bcm_th3_ft_pkt_type_match_get(unit, ETRAP_LKUP_EN_PKT_TYPEr,
                                        &(match_data->pkt_type_bmp));
    BCM_IF_ERROR_RETURN(rv);
    if (match_data->pkt_type_bmp != 0) {
        *match_types |= BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP;
    }

    rv = _bcm_th3_ft_int_pri_match_get(unit, ETRAP_LKUP_EN_INT_PRIr,
                                       &(match_data->int_pri_bmp));
    BCM_IF_ERROR_RETURN(rv);
    if (match_data->int_pri_bmp != 0) {
        *match_types |= BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP;
    }

    rv = _bcm_th3_ft_port_match_get(unit, ETRAP_LKUP_EN_ING_PORTm,
                                    ING_PORT_BMP_ENf,
                                    &(match_data->ingress_pbmp));
    BCM_IF_ERROR_RETURN(rv);

    BCM_PBMP_COUNT(match_data->ingress_pbmp, port_count);
    if (port_count != 0) {
        *match_types |= BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INGRESS_PBMP;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_queue_match_set
 * Purpose:
 *      Set the match criteria for elephant queue action.
 * Parameters:
 *      unit             - (IN) BCM device number
 *      match_types      - (IN) Match types
 *      match_data       - (IN) Match data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_queue_match_set(int unit,
                            uint32 match_types,
                            bcm_flowtracker_elephant_match_data_t *match_data)
{
    int rv;
    uint32 valid_match_types = (BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP |
                                BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP  |
                                BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_EGRESS_PBMP);

    if ((match_types & ~valid_match_types) != 0) {
        return BCM_E_PARAM;
    }

    if (match_types & BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP) {
        rv = _bcm_th3_ft_pkt_type_match_set(unit, ETRAP_QUEUE_EN_PKT_TYPEr,
                                            match_data->pkt_type_bmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (match_types & BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP) {
        rv = _bcm_th3_ft_int_pri_match_set(unit, ETRAP_QUEUE_EN_INT_PRIr,
                                           match_data->int_pri_bmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (match_types & BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_EGRESS_PBMP) {
        rv = _bcm_th3_ft_port_match_set(unit, ETRAP_QUEUE_EN_EGR_PORT_BMPm,
                                        EGR_PORT_BMP_ENf,
                                        match_data->egress_pbmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_queue_match_get
 * Purpose:
 *      Get the match criteria for elephant queue action.
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      match_types      - (IN)  Match types
 *      match_data       - (OUT) Match data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_queue_match_get(int unit,
                            uint32 *match_types,
                            bcm_flowtracker_elephant_match_data_t *match_data)
{
    int rv;
    int port_count;

    *match_types = 0;

    rv = _bcm_th3_ft_pkt_type_match_get(unit, ETRAP_QUEUE_EN_PKT_TYPEr,
                                        &(match_data->pkt_type_bmp));
    BCM_IF_ERROR_RETURN(rv);
    if (match_data->pkt_type_bmp != 0) {
        *match_types |= BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP;
    }

    rv = _bcm_th3_ft_int_pri_match_get(unit, ETRAP_QUEUE_EN_INT_PRIr,
                                       &(match_data->int_pri_bmp));
    BCM_IF_ERROR_RETURN(rv);
    if (match_data->int_pri_bmp != 0) {
        *match_types |= BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP;
    }

    rv = _bcm_th3_ft_port_match_get(unit, ETRAP_QUEUE_EN_EGR_PORT_BMPm,
                                    EGR_PORT_BMP_ENf,
                                    &(match_data->egress_pbmp));
    BCM_IF_ERROR_RETURN(rv);

    BCM_PBMP_COUNT(match_data->egress_pbmp, port_count);
    if (port_count != 0) {
        *match_types |= BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_EGRESS_PBMP;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_color_match_set
 * Purpose:
 *      Set the match criteria for elephant color action.
 * Parameters:
 *      unit             - (IN) BCM device number
 *      match_types      - (IN) Match types
 *      match_data       - (IN) Match data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_color_match_set(int unit,
                            uint32 match_types,
                            bcm_flowtracker_elephant_match_data_t *match_data)
{
    int rv;
    uint32 valid_match_types = (BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP |
                                BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP  |
                                BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_EGRESS_PBMP);

    if ((match_types & ~valid_match_types) != 0) {
        return BCM_E_PARAM;
    }

    if (match_types & BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP) {
        rv = _bcm_th3_ft_pkt_type_match_set(unit, ETRAP_COLOR_EN_PKT_TYPEr,
                                            match_data->pkt_type_bmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (match_types & BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP) {
        rv = _bcm_th3_ft_int_pri_match_set(unit, ETRAP_COLOR_EN_INT_PRIr,
                                           match_data->int_pri_bmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    if (match_types & BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_EGRESS_PBMP) {
        rv = _bcm_th3_ft_port_match_set(unit, ETRAP_COLOR_EN_EGR_PORT_BMPm,
                                        EGR_PORT_BMP_ENf,
                                        match_data->egress_pbmp);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_color_match_get
 * Purpose:
 *      Get the match criteria for elephant color action.
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      match_types      - (IN)  Match types
 *      match_data       - (OUT) Match data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_color_match_get(int unit,
                            uint32 *match_types,
                            bcm_flowtracker_elephant_match_data_t *match_data)
{
    int rv;
    int port_count;

    *match_types = 0;

    rv = _bcm_th3_ft_pkt_type_match_get(unit, ETRAP_COLOR_EN_PKT_TYPEr,
                                        &(match_data->pkt_type_bmp));
    BCM_IF_ERROR_RETURN(rv);
    if (match_data->pkt_type_bmp != 0) {
        *match_types |= BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_PKT_TYPE_BMP;
    }

    rv = _bcm_th3_ft_int_pri_match_get(unit, ETRAP_COLOR_EN_INT_PRIr,
                                       &(match_data->int_pri_bmp));
    BCM_IF_ERROR_RETURN(rv);
    if (match_data->int_pri_bmp != 0) {
        *match_types |= BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_INT_PRI_BMP;
    }

    rv = _bcm_th3_ft_port_match_get(unit, ETRAP_COLOR_EN_EGR_PORT_BMPm,
                                    EGR_PORT_BMP_ENf,
                                    &(match_data->egress_pbmp));
    BCM_IF_ERROR_RETURN(rv);

    BCM_PBMP_COUNT(match_data->egress_pbmp, port_count);
    if (port_count != 0) {
        *match_types |= BCM_FLOWTRACKER_ELEPHANT_MATCH_TYPE_EGRESS_PBMP;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_monitor_interval_hw_encode
 * Purpose:
 *      Validate and convert ETRAP monitor interval from usecs to H/w encoding
 * Parameters:
 *      interval     - (IN)  ETRAP monitor intrerval in usecs
 *      hw_value     - (OUT) ETRAP monitor interval H/w encoding
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_monitor_interval_hw_encode(int interval, uint32 *hw_value)
{
    switch (interval) {
        case 1000:
            *hw_value = 0;
            break;

        case 2000:
            *hw_value = 1;
            break;

        case 5000:
            *hw_value = 2;
            break;

        case 10000:
            *hw_value = 3;
            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_monitor_interval_hw_decode
 * Purpose:
 *      Validate and convert ETRAP monitor interval H/w value to usecs
 * Parameters:
 *      hw_value     - (IN)  ETRAP monitor interval H/w encoding
 *      interval     - (OUT) ETRAP monitor intrerval in usecs
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_monitor_interval_hw_decode(uint32 hw_value, int *interval)
{
    switch (hw_value) {
        case 0:
            *interval = 1000;
            break;

        case 1:
            *interval = 2000;
            break;

        case 2:
            *interval = 5000;
            break;

        case 3:
            *interval = 10000;
            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_queue_drain_time_hw_encode
 * Purpose:
 *      Validate and convert ETRAP interval to H/w encoding
 * Parameters:
 *      drain_time   - (IN)  Queue drain time in usecs
 *      hw_value     - (OUT) H/w encoding
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_queue_drain_time_hw_encode(int drain_time, uint32 *hw_value)
{
    switch (drain_time) {
        case 50:
            *hw_value = 0;
            break;

        case 100:
            *hw_value = 1;
            break;

        case 150:
            *hw_value = 2;
            break;

        case 200:
            *hw_value = 3;
            break;

        case 250:
            *hw_value = 4;
            break;

        case 500:
            *hw_value = 5;
            break;

        default:
            return BCM_E_PARAM;

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_hash_type_hw_encode
 * Purpose:
 *      Validate and convert hash type to H/w encoding
 * Parameters:
 *      hash_type    - (IN)  Hash type
 *      hw_value     - (OUT) H/w encoding
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_hash_type_hw_encode(bcm_flowtracker_elephant_hash_type_t hash_type,
                                uint32 *hw_value)
{
    switch (hash_type) {
        case bcmFlowtrackerElephantHashTypeField0Function0:
            *hw_value = 0;
            break;

        case bcmFlowtrackerElephantHashTypeField0Function1:
            *hw_value = 1;
            break;

        case bcmFlowtrackerElephantHashTypeField1Function0:
            *hw_value = 2;
            break;

        case bcmFlowtrackerElephantHashTypeField1Function1:
            *hw_value = 3;
            break;

        default:
            return BCM_E_PARAM;

    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_hash_type_hw_decode
 * Purpose:
 *      Validate and decode the H/w value to hash type
 * Parameters:
 *      hash_type    - (IN)  Hash type
 *      hw_value     - (OUT) H/w encoding
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int
_bcm_th3_ft_hash_type_hw_decode(uint32 hw_value,
                                bcm_flowtracker_elephant_hash_type_t *hash_type)
{
    switch (hw_value) {
        case 0:
            *hash_type = bcmFlowtrackerElephantHashTypeField0Function0;
            break;

        case 1:
            *hash_type = bcmFlowtrackerElephantHashTypeField0Function1;
            break;

        case 2:
            *hash_type = bcmFlowtrackerElephantHashTypeField1Function0;
            break;

        case 3:
            *hash_type = bcmFlowtrackerElephantHashTypeField1Function1;
            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_th3_ft_hw_init
 * Purpose:
 *      Initialize the H/w to default values
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_th3_ft_hw_init(int unit)
{
    int rv;
    uint32 rval, fval;
    uint64 rval64, fval64;
    int queue_drain_time;
    int i;
    etrap_int_pri_remap_table_entry_t int_pri_remap;

    rv = READ_ETRAP_FILT_CFGr(unit, &rval);
    SOC_IF_ERROR_RETURN(rv);

    /* Set default hashing configuration for the bloom filter */
    soc_reg_field_set(unit, ETRAP_FILT_CFGr, &rval, FILTER_0_HASH_SELf, 0);
    soc_reg_field_set(unit, ETRAP_FILT_CFGr, &rval, FILTER_1_HASH_SELf, 1);
    soc_reg_field_set(unit, ETRAP_FILT_CFGr, &rval, FILTER_2_HASH_SELf, 2);
    soc_reg_field_set(unit, ETRAP_FILT_CFGr, &rval, FILTER_3_HASH_SELf, 3);

    rv = WRITE_ETRAP_FILT_CFGr(unit, rval);
    SOC_IF_ERROR_RETURN(rv);

    rv = READ_ETRAP_FLOW_CFGr(unit, &rval);
    SOC_IF_ERROR_RETURN(rv);

    /* Set the default hashing config for the elephant flow table */
    soc_reg_field_set(unit, ETRAP_FLOW_CFGr, &rval, LEFT_BANK_HASH_SELf, 0);
    soc_reg_field_set(unit, ETRAP_FLOW_CFGr, &rval, RIGHT_BANK_HASH_SELf, 2);

    /* Set the default ETRAP interval to 5msec */
    rv = _bcm_th3_ft_monitor_interval_hw_encode(5000, &fval);
    BCM_IF_ERROR_RETURN(rv);
    soc_reg_field_set(unit, ETRAP_FLOW_CFGr, &rval, ETRAP_INTERVALf, fval);

    /* Set the default critical time period to 100usec */
    queue_drain_time =
        soc_property_get(unit,
                         spn_FLOWTRACKER_ELEPHANT_EXPECTED_QUEUE_DRAIN_TIME_USECS,
                         _BCM_TH3_FT_DEF_QUEUE_DRAIN_TIME_USECS);

    rv = _bcm_th3_ft_queue_drain_time_hw_encode(queue_drain_time, &fval);
    BCM_IF_ERROR_RETURN(rv);
    soc_reg_field_set(unit, ETRAP_FLOW_CFGr, &rval, CRITICAL_TIME_PERIODf, fval);

    rv = WRITE_ETRAP_FLOW_CFGr(unit, rval);
    SOC_IF_ERROR_RETURN(rv);


    /* Enable color action from ETRAP block. Match criteria or IFP action will
     * determine if the color is changed for a given packet
     */
    rv = READ_ETRAP_PROC_EN_2r(unit, &rval);
    SOC_IF_ERROR_RETURN(rv);

    soc_reg_field_set(unit, ETRAP_PROC_EN_2r, &rval, COLOR_ACTION_ENf, 1);

    rv = WRITE_ETRAP_PROC_EN_2r(unit, rval);
    SOC_IF_ERROR_RETURN(rv);

    /* Set default int_pri remap to not change the int_pri */
    rv = READ_ETRAP_PROC_ENr(unit, &rval);
    SOC_IF_ERROR_RETURN(rv);

    for (i = 0; i <=  _BCM_TH3_FT_INT_PRI_MAX; i++) {
        sal_memset(&int_pri_remap, 0, sizeof(int_pri_remap));
        soc_ETRAP_INT_PRI_REMAP_TABLEm_field32_set(unit, &int_pri_remap,
                                                   INT_PRIf, i);
        rv = WRITE_ETRAP_INT_PRI_REMAP_TABLEm(unit, MEM_BLOCK_ALL,
                                              i, &int_pri_remap);
        SOC_IF_ERROR_RETURN(rv);
    }


    rv = READ_ETRAP_MONITOR_CONFIGr(unit, &rval64);
    SOC_IF_ERROR_RETURN(rv);

    /* Enable monitoring, there are other controls to decide if the packet
     * needs to be mirrored or copied to CPU
     */
    COMPILER_64_SET(fval64, 0, 1);
    soc_reg64_field_set(unit, ETRAP_MONITOR_CONFIGr, &rval64,
                        ETRAP_MONITOR_ENABLEf, fval64);

    /* Always enable Mirroring, ETRAP_MONITOR_MIRROR_CONFIG has controls to
     * decide whether to Mirror a packet or not.
     */
    COMPILER_64_SET(fval64, 0, 1);
    soc_reg64_field_set(unit, ETRAP_MONITOR_CONFIGr, &rval64, MIRRORf, fval64);

    rv = WRITE_ETRAP_MONITOR_CONFIGr(unit, rval64);
    SOC_IF_ERROR_RETURN(rv);


    /* Enable ETRAP */
    rv = READ_ETRAP_PROC_ENr(unit, &rval);
    SOC_IF_ERROR_RETURN(rv);

    soc_reg_field_set(unit, ETRAP_PROC_ENr, &rval, ETRAP_ENf, 1);
    soc_reg_field_set(unit, ETRAP_PROC_ENr, &rval, BLOOM_FLOW_INS_ENf, 1);

    rv = WRITE_ETRAP_PROC_ENr(unit, rval);
    SOC_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_hw_stats_clear
 * Purpose:
 *      Clear the H/w stats
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_th3_ft_hw_stats_clear(int unit)
{
    int rv;
    uint64 rval64;

    /* Zero out the H/w values */
    COMPILER_64_SET(rval64, 0, 0);

    rv = WRITE_ETRAP_FILT_EXCEED_CTRr(unit, rval64);
    SOC_IF_ERROR_RETURN(rv);

    rv = WRITE_ETRAP_FLOW_INS_FAIL_CTRr(unit, rval64);
    SOC_IF_ERROR_RETURN(rv);

    rv = WRITE_ETRAP_FLOW_INS_SUCCESS_CTRr(unit, rval64);
    SOC_IF_ERROR_RETURN(rv);

    rv = WRITE_ETRAP_FLOW_DETECT_CTRr(unit, rval64);
    SOC_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_hw_detach
 * Purpose:
 *      Initialize the H/w back to original values
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
STATIC int _bcm_th3_ft_hw_detach(int unit)
{
    int rv;
    uint64 rval64;

    rv = WRITE_ETRAP_PROC_ENr(unit, 0);
    SOC_IF_ERROR_RETURN(rv);

    rv = WRITE_ETRAP_PROC_EN_2r(unit, 0);
    SOC_IF_ERROR_RETURN(rv);

    rv = WRITE_ETRAP_FILT_CFGr(unit, 0);
    SOC_IF_ERROR_RETURN(rv);

    rv = WRITE_ETRAP_FLOW_CFGr(unit, 0);
    SOC_IF_ERROR_RETURN(rv);

    COMPILER_64_SET(rval64, 0, 0);
    rv = WRITE_ETRAP_MONITOR_CONFIGr(unit, rval64);
    SOC_IF_ERROR_RETURN(rv);

    rv = soc_mem_clear(unit, ETRAP_INT_PRI_REMAP_TABLEm, COPYNO_ALL, TRUE);
    SOC_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_flowtracker_init
 * Purpose:
 *      Initialize the Flowtracker subsystem.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_init(int unit)
{
    _bcm_th3_ft_info_t *ft_info = _bcm_th3_ft_info[unit];
    int rv;

    if (ft_info != NULL) {
        /* Re-initialization */
        BCM_IF_ERROR_RETURN(bcm_tomahawk3_flowtracker_detach(unit));
    }

    /* Initialize the global memory */
    _bcm_th3_ft_info[unit] = (_bcm_th3_ft_info_t *) sal_alloc(sizeof(_bcm_th3_ft_info_t),
                                                              "TH3 ETRAP global info");
    if (_bcm_th3_ft_info[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    ft_info = _bcm_th3_ft_info[unit];
    sal_memset(ft_info, 0, sizeof(_bcm_th3_ft_info_t));


    /* Create the mutex */
    ft_info->mutex = sal_mutex_create("ft.mutex");
    if (ft_info->mutex == NULL) {
        BCM_IF_ERROR_RETURN(bcm_tomahawk3_flowtracker_detach(unit));
        return BCM_E_MEMORY;
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
#endif /* BCM_WARM_BOOT_SUPPORT */

        /* Set H/w to default value if it is cold boot */
        rv = _bcm_th3_ft_hw_init(unit);
        if (rv != BCM_E_NONE) {
            BCM_IF_ERROR_RETURN(bcm_tomahawk3_flowtracker_detach(unit));
            return rv;
        }

#ifdef BCM_WARM_BOOT_SUPPORT
    }
#endif /* BCM_WARM_BOOT_SUPPORT */


    /* Register counter collection callback. */
    BCM_IF_ERROR_RETURN(
               soc_counter_extra_register(unit,
                                          _bcm_th3_ft_counters_collect));

    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_init(unit));
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_tomahawk3_flowtracker_detach
 * Purpose:
 *      Shutdown the Flowtracker subsystem.
 * Parameters:
 *      unit         - (IN) BCM device number
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_detach(int unit)
{
    _bcm_th3_ft_info_t *ft_info = _bcm_th3_ft_info[unit];

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    /* Unregister counter collection callback. */
    BCM_IF_ERROR_RETURN(
                 soc_counter_extra_unregister(unit,
                                              _bcm_th3_ft_counters_collect));


#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) {
#endif /* BCM_WARM_BOOT_SUPPORT */
        /* Set H/w back to original values */
        BCM_IF_ERROR_RETURN(_bcm_th3_ft_hw_detach(unit));
#ifdef BCM_WARM_BOOT_SUPPORT
    }
#endif /* BCM_WARM_BOOT_SUPPORT */


    if (ft_info->mutex != NULL) {
        sal_mutex_destroy(ft_info->mutex);
        ft_info->mutex = NULL;
    }
    sal_free(ft_info);
    _bcm_th3_ft_info[unit] = NULL;

    BCM_IF_ERROR_RETURN(bcm_esw_flowtracker_detach(unit));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_flowtracker_elephant_action_match_set
 * Purpose:
 *      Set the match criteria for an elephant action.
 * Parameters:
 *      unit             - (IN) BCM device number
 *      action           - (IN) Match action
 *      match_types      - (IN) Match types
 *      match_data       - (IN) Match data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_elephant_action_match_set(
                               int unit,
                               bcm_flowtracker_elephant_match_action_t action,
                               uint32 match_types,
                               bcm_flowtracker_elephant_match_data_t *match_data)
{
    switch(action) {
        case bcmFlowtrackerElephantMatchActionLookup:
            return _bcm_th3_ft_lookup_match_set(unit, match_types, match_data);

        case bcmFlowtrackerElephantMatchActionQueue:
            return _bcm_th3_ft_queue_match_set(unit, match_types, match_data);

        case bcmFlowtrackerElephantMatchActionColor:
            return _bcm_th3_ft_color_match_set(unit, match_types, match_data);

        default:
            return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      bcm_tomahawk3_flowtracker_elephant_action_match_get
 * Purpose:
 *      Get the match criteria for an elephant action.
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      action           - (IN)  Match action
 *      match_types      - (OUT) Match types
 *      match_data       - (OUT) Match data
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_elephant_action_match_get(
                               int unit,
                               bcm_flowtracker_elephant_match_action_t action,
                               uint32 *match_types,
                               bcm_flowtracker_elephant_match_data_t *match_data)
{
    switch(action) {
        case bcmFlowtrackerElephantMatchActionLookup:
            return _bcm_th3_ft_lookup_match_get(unit, match_types, match_data);

        case bcmFlowtrackerElephantMatchActionQueue:
            return _bcm_th3_ft_queue_match_get(unit, match_types, match_data);

        case bcmFlowtrackerElephantMatchActionColor:
            return _bcm_th3_ft_color_match_get(unit, match_types, match_data);

        default:
            return BCM_E_PARAM;
    }
}

/*
 * Function:
 *      bcm_tomahawk3_flowtracker_elephant_int_pri_remap_set
 * Purpose:
 *      Set the internal priority remap for elephant flows.
 * Parameters:
 *      unit             - (IN) BCM device number
 *      int_pri          - (IN) Internal priority
 *      new_int_pri      - (IN) New internal priority
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_elephant_int_pri_remap_set(int unit,
                                                         bcm_cos_t int_pri,
                                                         bcm_cos_t new_int_pri)
{
    int rv;
    etrap_int_pri_remap_table_entry_t int_pri_remap;

    if ((int_pri >= _BCM_TH3_FT_INT_PRI_MAX) ||
        (new_int_pri > _BCM_TH3_FT_INT_PRI_MAX)) {
        return BCM_E_PARAM;
    }


    sal_memset(&int_pri_remap, 0, sizeof(int_pri_remap));
    soc_ETRAP_INT_PRI_REMAP_TABLEm_field32_set(unit, &int_pri_remap,
                                               INT_PRIf, new_int_pri);
    rv = WRITE_ETRAP_INT_PRI_REMAP_TABLEm(unit, MEM_BLOCK_ALL,
                                          int_pri, &int_pri_remap);
    SOC_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_flowtracker_elephant_int_pri_remap_set
 * Purpose:
 *      Set the internal priority remap for elephant flows.
 * Parameters:
 *      unit             - (IN)  BCM device number
 *      int_pri          - (IN)  Internal priority
 *      new_int_pri      - (OUT) New internal priority
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_elephant_int_pri_remap_get(int unit,
                                                         bcm_cos_t int_pri,
                                                         bcm_cos_t *new_int_pri)
{
    int rv;
    etrap_int_pri_remap_table_entry_t int_pri_remap;

    if (int_pri > _BCM_TH3_FT_INT_PRI_MAX) {
        return BCM_E_PARAM;
    }


    sal_memset(&int_pri_remap, 0, sizeof(int_pri_remap));
    rv = READ_ETRAP_INT_PRI_REMAP_TABLEm(unit, MEM_BLOCK_ALL,
                                         int_pri, &int_pri_remap);
    SOC_IF_ERROR_RETURN(rv);

    *new_int_pri = soc_ETRAP_INT_PRI_REMAP_TABLEm_field32_get(unit,
                                                              &int_pri_remap,
                                                              INT_PRIf);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_flowtracker_elephant_control_set
 * Purpose:
 *      Set elephant controls
 * Parameters:
 *      unit        - (IN) BCM device number
 *      type        - (IN) Control type
 *      arg         - (IN) Control arg
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_tomahawk3_flowtracker_elephant_control_set(
                                         int unit,
                                         bcm_flowtracker_elephant_control_t type,
                                         int arg)
{
    int rv;
    uint32 rval, fval;
    uint64 rval64, fval64;
    uint32 limit;

    switch (type) {
        case bcmFlowtrackerElephantControlMonitorIntervalUsecs:
            rv = _bcm_th3_ft_monitor_interval_hw_encode(arg, &fval);
            BCM_IF_ERROR_RETURN(rv);

            return soc_reg_field32_modify(unit, ETRAP_FLOW_CFGr, REG_PORT_ANY,
                                          ETRAP_INTERVALf, fval);

        case bcmFlowtrackerElephantControlBloomFilterByteThreshold:
            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit, ETRAP_FILT_THRESHr,
                                              THRESHOLD_BYTESf);
            if (arg > limit) {
                return BCM_E_PARAM;
            }
            return soc_reg_field32_modify(unit, ETRAP_FILT_THRESHr, REG_PORT_ANY,
                                          THRESHOLD_BYTESf, arg);

        case bcmFlowtrackerElephantControlElephantThresholdBytes:
            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit,
                                              ETRAP_FLOW_ELEPH_THRESHOLDr,
                                              BYTE_THRESHOLDf);
            if (arg > limit) {
                return BCM_E_PARAM;
            }
            return soc_reg_field32_modify(unit, ETRAP_FLOW_ELEPH_THRESHOLDr,
                                          REG_PORT_ANY, BYTE_THRESHOLDf, arg);

        case bcmFlowtrackerElephantControlDemotionThresholdBytes:
            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit,
                                              ETRAP_FLOW_RESET_THRESHOLDr,
                                              BYTE_THRESHOLDf);
            if (arg > limit) {
                return BCM_E_PARAM;
            }
            return soc_reg_field32_modify(unit, ETRAP_FLOW_RESET_THRESHOLDr,
                                          REG_PORT_ANY, BYTE_THRESHOLDf, arg);

        case bcmFlowtrackerElephantControlYellowThreshold:
            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit,
                                              ETRAP_FLOW_ELEPH_THR_YELr,
                                              BYTE_THRESHOLDf);
            if (arg > limit) {
                return BCM_E_PARAM;
            }
            return soc_reg_field32_modify(unit, ETRAP_FLOW_ELEPH_THR_YELr,
                                          REG_PORT_ANY, BYTE_THRESHOLDf, arg);

        case bcmFlowtrackerElephantControlRedThreshold:
            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit,
                                              ETRAP_FLOW_ELEPH_THR_REDr,
                                              BYTE_THRESHOLDf);
            if (arg > limit) {
                return BCM_E_PARAM;
            }
            return soc_reg_field32_modify(unit, ETRAP_FLOW_ELEPH_THR_REDr,
                                          REG_PORT_ANY, BYTE_THRESHOLDf, arg);

        case bcmFlowtrackerElephantControlPacketRemarkEnable:
            rv = READ_ETRAP_COLOR_EN_INT_PRIr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            if (arg == 0) {
                /* Disable packet remarking */
                fval = 0;
            } else {
                /* Enable remarking on all int_pri */
                fval = (1 << (_BCM_TH3_FT_INT_PRI_MAX + 1)) - 1;
            }
            soc_reg_field_set(unit, ETRAP_COLOR_EN_INT_PRIr, &rval,
                              ETRAP_MPB_COLOR_UPDATE_ENf, fval);

            return soc_reg_field32_modify(unit, ETRAP_COLOR_EN_INT_PRIr,
                                          REG_PORT_ANY, ETRAP_MPB_COLOR_UPDATE_ENf,
                                          fval);

        case bcmFlowtrackerElephantControlSampleRate:
            rv = READ_ETRAP_MONITOR_CONFIGr(unit, &rval64);
            SOC_IF_ERROR_RETURN(rv);

            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit, ETRAP_MONITOR_CONFIGr,
                                              SAMPLE_THRESHOLDf);
            if (arg >= limit) {
                /* arg == limit is also invalid, as that implies no sampling
                 * which is set by arg = 0
                 */
                return BCM_E_PARAM;
            }
            /* Set sample rate to 1/arg, packets are sampled if the random
             * number generated is greater than the threshold. If arg == 0
             * disable sampling by writing the max value to sample threshold
             */
            if (arg == 0) {
                fval = limit;
            } else {
                fval = limit - (limit / arg);
            }

            COMPILER_64_SET(fval64, 0, fval);
            soc_reg64_field_set(unit, ETRAP_MONITOR_CONFIGr, &rval64,
                                SAMPLE_THRESHOLDf, fval64);

            rv = WRITE_ETRAP_MONITOR_CONFIGr(unit, rval64);
            SOC_IF_ERROR_RETURN(rv);

            break;

        case bcmFlowtrackerElephantControlSampleSeed:
            rv = READ_ETRAP_MONITOR_CONFIGr(unit, &rval64);
            SOC_IF_ERROR_RETURN(rv);

            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit, ETRAP_MONITOR_CONFIGr,
                                              SEEDf);
            if (arg > limit) {
                return BCM_E_PARAM;
            }
            COMPILER_64_SET(fval64, 0, (uint32) arg);
            soc_reg64_field_set(unit, ETRAP_MONITOR_CONFIGr, &rval64,
                                SEEDf, fval64);

            rv = WRITE_ETRAP_MONITOR_CONFIGr(unit, rval64);
            SOC_IF_ERROR_RETURN(rv);

            break;

        case bcmFlowtrackerElephantControlSampleCopyToCpu:
            rv = READ_ETRAP_MONITOR_CONFIGr(unit, &rval64);
            SOC_IF_ERROR_RETURN(rv);

            if (arg == 0) {
                COMPILER_64_SET(fval64, 0, 0);
            } else {
                COMPILER_64_SET(fval64, 0, 1);
            }
            soc_reg64_field_set(unit, ETRAP_MONITOR_CONFIGr,
                                &rval64, COPY_TO_CPUf, fval64);

            rv = WRITE_ETRAP_MONITOR_CONFIGr(unit, rval64);
            SOC_IF_ERROR_RETURN(rv);
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_flowtracker_elephant_control_get
 * Purpose:
 *      Get elephant controls
 * Parameters:
 *      unit        - (IN)  BCM device number
 *      type        - (IN)  Control type
 *      arg         - (OUT) Control arg
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int
bcm_tomahawk3_flowtracker_elephant_control_get(
                                         int unit,
                                         bcm_flowtracker_elephant_control_t type,
                                         int *arg)
{
    int rv;
    uint32 limit;
    uint32 rval, fval;
    uint64 rval64, fval64;

    switch (type) {
        case bcmFlowtrackerElephantControlMonitorIntervalUsecs:
            rv = READ_ETRAP_FLOW_CFGr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            fval = soc_reg_field_get(unit, ETRAP_FLOW_CFGr, rval,
                                     ETRAP_INTERVALf);

            rv = _bcm_th3_ft_monitor_interval_hw_decode(fval, arg);
            BCM_IF_ERROR_RETURN(rv);
            break;

        case bcmFlowtrackerElephantControlBloomFilterByteThreshold:
            rv = READ_ETRAP_FILT_THRESHr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            *arg = rval;
            break;

        case bcmFlowtrackerElephantControlElephantThresholdBytes:
            rv = READ_ETRAP_FLOW_ELEPH_THRESHOLDr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            *arg = rval;
            break;

        case bcmFlowtrackerElephantControlDemotionThresholdBytes:
            rv = READ_ETRAP_FLOW_RESET_THRESHOLDr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            *arg = rval;
            break;

        case bcmFlowtrackerElephantControlYellowThreshold:
            rv = READ_ETRAP_FLOW_ELEPH_THR_YELr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            *arg = rval;
            break;

        case bcmFlowtrackerElephantControlRedThreshold:
            rv = READ_ETRAP_FLOW_ELEPH_THR_REDr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            *arg = rval;
            break;

        case bcmFlowtrackerElephantControlPacketRemarkEnable:
            rv = READ_ETRAP_COLOR_EN_INT_PRIr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            fval = soc_reg_field_get(unit, ETRAP_COLOR_EN_INT_PRIr, rval,
                                     ETRAP_MPB_COLOR_UPDATE_ENf);
            *arg = (fval == 0) ? 0 : 1;

            break;

        case bcmFlowtrackerElephantControlSampleRate:
            rv = READ_ETRAP_MONITOR_CONFIGr(unit, &rval64);
            SOC_IF_ERROR_RETURN(rv);

            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit, ETRAP_MONITOR_CONFIGr,
                                              SAMPLE_THRESHOLDf);

            fval64 = soc_reg64_field_get(unit, ETRAP_MONITOR_CONFIGr, rval64,
                                         SAMPLE_THRESHOLDf);
            if (COMPILER_64_LO(fval64) == limit) {
                /* 0 implies no sampling */
                *arg = 0;
            } else {
                *arg = limit / (limit - COMPILER_64_LO(fval64));
            }
            break;

        case bcmFlowtrackerElephantControlSampleSeed:
            rv = READ_ETRAP_MONITOR_CONFIGr(unit, &rval64);
            SOC_IF_ERROR_RETURN(rv);

            fval64 = soc_reg64_field_get(unit, ETRAP_MONITOR_CONFIGr, rval64, SEEDf);
            *arg = COMPILER_64_LO(fval64);
            break;

        case bcmFlowtrackerElephantControlSampleCopyToCpu:
            rv = READ_ETRAP_MONITOR_CONFIGr(unit, &rval64);
            SOC_IF_ERROR_RETURN(rv);

            fval64 = soc_reg64_field_get(unit, ETRAP_MONITOR_CONFIGr,
                                         rval64, COPY_TO_CPUf);
            *arg = COMPILER_64_LO(fval64);
            break;

        default:
            return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_flowtracker_elephant_hash_config_set
 * Purpose:
 *      Set the hashing configuration
 * Parameters:
 *      unit              - (IN) BCM device number
 *      hash_table        - (IN) Hash table
 *      instance_num      - (IN) Hash table instance number
 *      bank_num          - (IN) Hash table bank number
 *      hash_type         - (IN) Hash type
 *      right_rotate_bits - (IN) Number of bits the hash result should be
 *                               right rotated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_elephant_hash_config_set(
                                int unit,
                                bcm_flowtracker_elephant_hash_table_t hash_table,
                                int instance_num,
                                int bank_num,
                                bcm_flowtracker_elephant_hash_type_t hash_type,
                                int right_rotate_bits)
{
    int rv;
    uint32 limit;
    uint32 rval, fval;
    soc_field_t hash_fields[4]; /* 4 is the max banks x instances possible */
    soc_field_t rotr_fields[4];


    switch (hash_table) {
        case bcmFlowtrackerElephantHashTableBloomFilter:

            hash_fields[0] = FILTER_0_HASH_SELf;
            hash_fields[1] = FILTER_1_HASH_SELf;
            hash_fields[2] = FILTER_2_HASH_SELf;
            hash_fields[3] = FILTER_3_HASH_SELf;

            rotr_fields[0] = FILTER_0_HASH_ROTRf;
            rotr_fields[1] = FILTER_1_HASH_ROTRf;
            rotr_fields[2] = FILTER_2_HASH_ROTRf;
            rotr_fields[3] = FILTER_3_HASH_ROTRf;

            if (bank_num != 0) {
                /* Bloom filter is single bank memory */
                return BCM_E_PARAM;
            }
            if (instance_num >= _BCM_TH3_FT_BLOOM_FILTER_NUM_INSTANCES) {
                return BCM_E_PARAM;
            }


            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit, ETRAP_FILT_CFGr,
                                              rotr_fields[instance_num]);
            if (right_rotate_bits > limit) {
                return BCM_E_PARAM;
            }

            rv = _bcm_th3_ft_hash_type_hw_encode(hash_type, &fval);
            BCM_IF_ERROR_RETURN(rv);

            /* Write to H/w */
            rv = READ_ETRAP_FILT_CFGr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            soc_reg_field_set(unit, ETRAP_FILT_CFGr, &rval,
                              hash_fields[instance_num], fval);
            soc_reg_field_set(unit, ETRAP_FILT_CFGr, &rval,
                              rotr_fields[instance_num], right_rotate_bits);

            rv = WRITE_ETRAP_FILT_CFGr(unit, rval);
            SOC_IF_ERROR_RETURN(rv);

            break;

        case bcmFlowtrackerElephantHashTableFlowTable:

            hash_fields[0] = LEFT_BANK_HASH_SELf;
            hash_fields[1] = RIGHT_BANK_HASH_SELf;

            rotr_fields[0] = LEFT_BANK_HASH_ROTRf;
            rotr_fields[1] = RIGHT_BANK_HASH_ROTRf;

            if (bank_num >= _BCM_TH3_FT_FLOW_TABLE_NUM_BANKS) {
                return BCM_E_PARAM;
            }
            if (instance_num != 0) {
                /* Eflow table is single instance */
                return BCM_E_PARAM;
            }

            limit = _BCM_TH3_FT_REG_FIELD_MAX(unit, ETRAP_FLOW_CFGr,
                                              rotr_fields[bank_num]);
            if (right_rotate_bits > limit) {
                return BCM_E_PARAM;
            }

            rv = _bcm_th3_ft_hash_type_hw_encode(hash_type, &fval);
            BCM_IF_ERROR_RETURN(rv);

            /* Write to H/w */
            rv = READ_ETRAP_FLOW_CFGr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            soc_reg_field_set(unit, ETRAP_FLOW_CFGr, &rval,
                              hash_fields[bank_num], fval);

            soc_reg_field_set(unit, ETRAP_FLOW_CFGr, &rval,
                              rotr_fields[bank_num], right_rotate_bits);

            rv = WRITE_ETRAP_FLOW_CFGr(unit, rval);
            SOC_IF_ERROR_RETURN(rv);

            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_tomahawk3_flowtracker_elephant_hash_config_get
 * Purpose:
 *      Set the hashing configuration
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      hash_table        - (IN)  Hash table
 *      instance_num      - (IN)  Hash table instance number
 *      bank_num          - (IN)  Hash table bank number
 *      hash_type         - (OUT) Hash type
 *      right_rotate_bits - (OUT) Number of bits the hash result should be
 *                                right rotated
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_elephant_hash_config_get(
                                int unit,
                                bcm_flowtracker_elephant_hash_table_t hash_table,
                                int instance_num,
                                int bank_num,
                                bcm_flowtracker_elephant_hash_type_t *hash_type,
                                int *right_rotate_bits)
{
    int rv;
    uint32 rval, fval;
    soc_field_t hash_fields[4]; /* 4 is the max banks x instances possible */
    soc_field_t rotr_fields[4];

    switch (hash_table) {
        case bcmFlowtrackerElephantHashTableBloomFilter:

            hash_fields[0] = FILTER_0_HASH_SELf;
            hash_fields[1] = FILTER_1_HASH_SELf;
            hash_fields[2] = FILTER_2_HASH_SELf;
            hash_fields[3] = FILTER_3_HASH_SELf;

            rotr_fields[0] = FILTER_0_HASH_ROTRf;
            rotr_fields[1] = FILTER_1_HASH_ROTRf;
            rotr_fields[2] = FILTER_2_HASH_ROTRf;
            rotr_fields[3] = FILTER_3_HASH_ROTRf;

            if (bank_num != 0) {
                /* Bloom filter is single bank memory */
                return BCM_E_PARAM;
            }
            if (instance_num >= _BCM_TH3_FT_BLOOM_FILTER_NUM_INSTANCES) {
                return BCM_E_PARAM;
            }

            rv = READ_ETRAP_FILT_CFGr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            fval = soc_reg_field_get(unit, ETRAP_FILT_CFGr, rval,
                                     hash_fields[instance_num]);

            rv = _bcm_th3_ft_hash_type_hw_decode(fval, hash_type);
            BCM_IF_ERROR_RETURN(rv);

            *right_rotate_bits = soc_reg_field_get(unit, ETRAP_FILT_CFGr, rval,
                                                   rotr_fields[instance_num]);

            break;

        case bcmFlowtrackerElephantHashTableFlowTable:

            hash_fields[0] = LEFT_BANK_HASH_SELf;
            hash_fields[1] = RIGHT_BANK_HASH_SELf;

            rotr_fields[0] = LEFT_BANK_HASH_ROTRf;
            rotr_fields[1] = RIGHT_BANK_HASH_ROTRf;

            if (bank_num > _BCM_TH3_FT_FLOW_TABLE_NUM_BANKS) {
                return BCM_E_PARAM;
            }
            if (instance_num != 0) {
                /* Eflow table is single instance */
                return BCM_E_PARAM;
            }

            rv = READ_ETRAP_FLOW_CFGr(unit, &rval);
            SOC_IF_ERROR_RETURN(rv);

            fval = soc_reg_field_get(unit, ETRAP_FLOW_CFGr, rval,
                                     hash_fields[bank_num]);

            rv = _bcm_th3_ft_hash_type_hw_encode(fval, hash_type);
            BCM_IF_ERROR_RETURN(rv);

            *right_rotate_bits = soc_reg_field_get(unit, ETRAP_FLOW_CFGr, rval,
                                                   rotr_fields[bank_num]);

            break;

        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_flowtracker_elephant_stats_set
 * Purpose:
 *      Set the elephant statistics
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      stats             - (IN)  Elephant statistics
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_elephant_stats_set(
                                         int unit,
                                         bcm_flowtracker_elephant_stats_t *stats)
{
    _bcm_th3_ft_info_t *ft_info = _bcm_th3_ft_info[unit];

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_ft_lock(unit));

    /* Clear the H/w stats */
    BCM_IF_ERROR_RETURN(_bcm_th3_ft_hw_stats_clear(unit));

    /* Clear the cached H/w stats */
    sal_memset(&(ft_info->stats.prev), 0,
               _TH3_PIPES_PER_DEV * sizeof(bcm_flowtracker_elephant_stats_t));

    /* Replace the S/w stats with the user provided ones */
    sal_memcpy(&(ft_info->stats.acc), stats, sizeof(bcm_flowtracker_elephant_stats_t));

    BCM_IF_ERROR_RETURN(_bcm_th3_ft_unlock(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_flowtracker_elephant_stats_get
 * Purpose:
 *      Get the elephant statistics
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      stats             - (OUT) Elephant statistics
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_elephant_stats_get(
                                         int unit,
                                         bcm_flowtracker_elephant_stats_t *stats)
{
    _bcm_th3_ft_info_t *ft_info = _bcm_th3_ft_info[unit];

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (stats == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_ft_lock(unit));

    /* Copy the accumulated values */
    sal_memcpy(stats, &(ft_info->stats.acc), sizeof(bcm_flowtracker_elephant_stats_t));

    BCM_IF_ERROR_RETURN(_bcm_th3_ft_unlock(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_flowtracker_elephant_stats_sync_get
 * Purpose:
 *      Get the elephant statistics synced with H/w
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      stats             - (OUT) Elephant statistics
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int bcm_tomahawk3_flowtracker_elephant_stats_sync_get(
                                         int unit,
                                         bcm_flowtracker_elephant_stats_t *stats)
{
    _bcm_th3_ft_info_t *ft_info = _bcm_th3_ft_info[unit];

    if (ft_info == NULL) {
        return BCM_E_INIT;
    }

    if (stats == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_ft_lock(unit));

    /* Sync with H/w */
    _bcm_th3_ft_counters_collect_no_lock(unit);

    /* Copy the accumulated values */
    sal_memcpy(stats, &(ft_info->stats.acc), sizeof(bcm_flowtracker_elephant_stats_t));

    BCM_IF_ERROR_RETURN(_bcm_th3_ft_unlock(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_dump_stats
 * Purpose:
 *      Dump & Clear ETRAP stats
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      clear             - (IN)  0 - Don't clear the stats
 *                                1 - Clear the stats after reading
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th3_ft_dump_stats(int unit, int clear)
{
    bcm_flowtracker_elephant_stats_t stats;
    int rv;

    rv = bcm_tomahawk3_flowtracker_elephant_stats_sync_get(unit, &stats);
    BCM_IF_ERROR_RETURN(rv);

    LOG_CLI((BSL_META_U(unit, "Candidates     : {0x%x, 0x%x} \r\n"),
                                 COMPILER_64_HI(stats.num_candidates_detected),
                                 COMPILER_64_LO(stats.num_candidates_detected)));

    LOG_CLI((BSL_META_U(unit, "Insert Fail    : {0x%x, 0x%x} \r\n"),
                                 COMPILER_64_HI(stats.num_flow_table_insert_failures),
                                 COMPILER_64_LO(stats.num_flow_table_insert_failures)));

    LOG_CLI((BSL_META_U(unit, "Insert Success : {0x%x, 0x%x} \r\n"),
                                 COMPILER_64_HI(stats.num_flow_table_insert_success),
                                 COMPILER_64_LO(stats.num_flow_table_insert_success)));

    LOG_CLI((BSL_META_U(unit, "Elephants      : {0x%x, 0x%x} \r\n"),
                                 COMPILER_64_HI(stats.num_elephants_detected),
                                 COMPILER_64_LO(stats.num_elephants_detected)));

    if (clear) {
        sal_memset(&stats, 0, sizeof(stats));
        rv = bcm_tomahawk3_flowtracker_elephant_stats_set(unit, &stats);
        BCM_IF_ERROR_RETURN(rv);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th3_ft_dump_flow_table
 * Purpose:
 *      Dump range of entries from Flow table
 * Parameters:
 *      unit              - (IN)  BCM device number
 *      pipe              - (IN)  Pipe Number
 *      start_index       - (IN)  Start index
 *      end_index         - (IN)  End index
 *      valid             - (IN)  TRUE  - Print only valid entries
 *                                FALSE - Print all entries
 * Returns:
 *      BCM_E_XXX   - BCM error code.
 */
int _bcm_th3_ft_dump_flow_table(int unit, int pipe, int start_idx, int end_idx, int valid)
{
    int bank = 0, bucket = 0;
    int idx;
    int sampled_idx, read_idx;
    uint32 rval, fval;
    uint8 reset;
    soc_reg_t reg;
    soc_mem_t mem;
    soc_mem_t flow_hash_mem[2][5];
    soc_field_t field[] = {ENTRY_0f, ENTRY_1f, ENTRY_2f, ENTRY_3f, ENTRY_4f};
    uint32 entry_buf[SOC_MAX_MEM_FIELD_WORDS];
    uint32 field_buf[SOC_MAX_FIELD_WORDS];
    int rv;


    if (pipe >= _TH3_PIPES_PER_DEV) {
        return BCM_E_PARAM;
    }
    if (SOC_PBMP_IS_NULL(PBMP_PIPE(unit, pipe))) {
        return BCM_E_PARAM;
    }

    if ((start_idx < 0) || (end_idx >= _BCM_TH3_FT_FLOW_TABLE_MAX_ENTRIES)) {
        return BCM_E_PARAM;
    }

    if (end_idx == -1) {
        /* Dump the entire table starting from start_idx */
        end_idx = _BCM_TH3_FT_FLOW_TABLE_MAX_ENTRIES - 1;
    }

    if (start_idx > end_idx) {
        return BCM_E_PARAM;
    }

    LOG_CLI((BSL_META_U(unit,
        "+-------+---------------------------------+--------+--------+------+--------------------------------+------------+\r\n")));
    LOG_CLI((BSL_META_U(unit,
        "| Index |             Status              | Notify | Color  | Port |       HASH {A0,A1,B0,B1}       |    Bytes   |\r\n")));
    LOG_CLI((BSL_META_U(unit,
        "+-------+---------------------------------+--------+--------+------+--------------------------------+------------+\r\n")));
    for (sampled_idx = 0, idx = start_idx; idx <= end_idx; idx++) {
        if (idx >= sampled_idx) {
            /* Need to read from H/w */
            bank   = idx / _BCM_TH3_FT_FLOW_TABLE_NUM_BANK_ENTRIES;
            bucket = (idx % _BCM_TH3_FT_FLOW_TABLE_NUM_BANK_ENTRIES) /
                _BCM_TH3_FT_FLOW_TABLE_NUM_BUCKET_ENTRIES;

            reg  = SOC_REG_UNIQUE_ACC(unit, ETRAP_SAMPLE_ADDRr)[pipe];
            rval = 0;
            if (bank == 0) {
                soc_reg_field_set(unit, reg, &rval, BUCKET_ADDR_LEFTf, bucket);
            } else {
                soc_reg_field_set(unit, reg, &rval, BUCKET_ADDR_RIGHTf, bucket);
            }
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));


            /* Find how much we have sampled */
            sampled_idx =
                (_BCM_TH3_FT_FLOW_TABLE_NUM_BUCKET_ENTRIES -
                            (idx % _BCM_TH3_FT_FLOW_TABLE_NUM_BUCKET_ENTRIES)) + idx;
        }
        /* Find which index to read in the bucket */
        read_idx = idx % _BCM_TH3_FT_FLOW_TABLE_NUM_BUCKET_ENTRIES;

        /* Read Flow ctrl */
        if (bank == 0) {
            mem = SOC_MEM_UNIQUE_ACC(unit, ETRAP_SAMPLE_FLOW_CTRL_LEFTm)[pipe];
        } else {
            mem = SOC_MEM_UNIQUE_ACC(unit, ETRAP_SAMPLE_FLOW_CTRL_RIGHTm)[pipe];
        }
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, entry_buf);
        SOC_IF_ERROR_RETURN(rv);
        soc_mem_field_get(unit, mem, entry_buf, field[read_idx], field_buf);

        /* Elephant status */
        fval = soc_ETRAP_FLOW_CTRLfmt_field32_get(unit, field_buf,
                                                   ELEPHANT_STATUSf);
        if (valid && (fval == 0)) {
            /* Unused entry and valid flag is set, don't print */
            continue;
        }

        /* Index */
        LOG_CLI((BSL_META_U(unit,"| %-4d  "), idx));

        if (fval == 0) {
            LOG_CLI((BSL_META_U(unit,"| %-31s "), "Unused"));
        } else if (fval == 1) {
            LOG_CLI((BSL_META_U(unit,"| %-31s "), "Mouse"));
        } else if (fval == 2) {
            LOG_CLI((BSL_META_U(unit,"| %-31s "), "Elephant (Eviction Eligible)"));
        } else {
            LOG_CLI((BSL_META_U(unit,"| %-31s "), "Elephant (No Eviction Eligible)"));
        }

        /* Notify flag */
        fval = soc_ETRAP_FLOW_CTRLfmt_field32_get(unit, field_buf,
                                                   ELEPHANT_NOTIFY_FLAGf);
        if (fval == 1) {
            LOG_CLI((BSL_META_U(unit,"| %-6s "), "Yes"));
        } else {
            LOG_CLI((BSL_META_U(unit,"| %-6s "), "No"));
        }

        /* Color */
        fval = soc_ETRAP_FLOW_CTRLfmt_field32_get(unit, field_buf,
                                                   ELEPHANT_COLORf);
        if (fval == 0) {
            LOG_CLI((BSL_META_U(unit,"| %-7s"), "Green"));
        } else if (fval == 1) {
            LOG_CLI((BSL_META_U(unit,"| %-7s"), "Red"));
        } else if (fval == 2) {
            LOG_CLI((BSL_META_U(unit,"| %-7s"), "None"));
        } else {
            LOG_CLI((BSL_META_U(unit,"| %-7s"), "Yellow"));
        }

        /* Reset flag (Used later) */
        reset = soc_ETRAP_FLOW_CTRLfmt_field32_get(unit, field_buf,
                                                    RESET_COUNTER_FLAGf);


        /* Read flow hash */
        flow_hash_mem[0][0] = ETRAP_SAMPLE_FLOW_HASH_L0m;
        flow_hash_mem[0][1] = ETRAP_SAMPLE_FLOW_HASH_L1m;
        flow_hash_mem[0][2] = ETRAP_SAMPLE_FLOW_HASH_L2m;
        flow_hash_mem[0][3] = ETRAP_SAMPLE_FLOW_HASH_L3m;
        flow_hash_mem[0][4] = ETRAP_SAMPLE_FLOW_HASH_L4m;

        flow_hash_mem[1][0] = ETRAP_SAMPLE_FLOW_HASH_R0m;
        flow_hash_mem[1][1] = ETRAP_SAMPLE_FLOW_HASH_R1m;
        flow_hash_mem[1][2] = ETRAP_SAMPLE_FLOW_HASH_R2m;
        flow_hash_mem[1][3] = ETRAP_SAMPLE_FLOW_HASH_R3m;
        flow_hash_mem[1][4] = ETRAP_SAMPLE_FLOW_HASH_R4m;

        mem = SOC_MEM_UNIQUE_ACC(unit, flow_hash_mem[bank][read_idx])[pipe];
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, entry_buf);
        SOC_IF_ERROR_RETURN(rv);
        soc_mem_field_get(unit, mem, entry_buf, ENTRYf, field_buf);

        /* Port */
        fval = soc_ETRAP_FLOW_HASHfmt_field32_get(unit, field_buf, INGRESS_PORTf);
        LOG_CLI((BSL_META_U(unit,"| %-4d "), fval));

        /* Hash values */
        fval = soc_ETRAP_FLOW_HASHfmt_field32_get(unit, field_buf, HASH_A0f);
        LOG_CLI((BSL_META_U(unit,"| 0x%04x, "), fval));

        fval = soc_ETRAP_FLOW_HASHfmt_field32_get(unit, field_buf, HASH_A1f);
        LOG_CLI((BSL_META_U(unit,"0x%04x, "), fval));

        fval = soc_ETRAP_FLOW_HASHfmt_field32_get(unit, field_buf, HASH_B0f);
        LOG_CLI((BSL_META_U(unit,"0x%04x, "), fval));

        fval = soc_ETRAP_FLOW_HASHfmt_field32_get(unit, field_buf, HASH_B1f);
        LOG_CLI((BSL_META_U(unit,"0x%04x "), fval));

        /* Read Flow count */
        if (bank == 0) {
            mem = SOC_MEM_UNIQUE_ACC(unit, ETRAP_SAMPLE_FLOW_COUNT_LEFTm)[pipe];
        } else {
            mem = SOC_MEM_UNIQUE_ACC(unit, ETRAP_SAMPLE_FLOW_COUNT_RIGHTm)[pipe];
        }
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, 0, entry_buf);
        SOC_IF_ERROR_RETURN(rv);
        soc_mem_field_get(unit, mem, entry_buf, field[read_idx], field_buf);

        /* EF_BYTES */
        fval = soc_ETRAP_FLOW_COUNTfmt_field32_get(unit, field_buf, EF_BYTESf);
        if (reset) {
            /* If reset is set, interpret EF_BYTES as 0 */
            fval = 0;
        }
        LOG_CLI((BSL_META_U(unit,"| %-10u "), fval));

        LOG_CLI((BSL_META_U(unit,"|\r\n")));
        LOG_CLI((BSL_META_U(unit,
        "+-------+---------------------------------+--------+--------+------+--------------------------------+------------+\r\n")));
    }
    return BCM_E_NONE;
}

#else /* BCM_TOMAHAWK3_SUPPORT */
typedef int _th3_ft_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_TOMAHAWK3_SUPPORT */
