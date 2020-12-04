/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        helix4.c
 * Purpose:
 * Requires:
 */


#include <sal/core/boot.h>
#include <shared/bsl.h>
#include <soc/triumph3.h>
#include <soc/helix4.h>
#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/mmu_config.h>

#ifdef BCM_HELIX4_SUPPORT

#define SOC_MAX_PHY_PORTS            86

#define CORE_MODE_SINGLE             0
#define CORE_MODE_DUAL               1
#define CORE_MODE_QUAD               2
#define CORE_MODE_NOTDM              3

#define PHY_MODE_SINGLE              0
#define PHY_MODE_DUAL                1
#define PHY_MODE_QUAD                2

#define MAC_MODE_INDEP               0
#define MAC_MODE_AGGR                1

#define _MAX_HX4_PORT_CFG 			 7

#define _MAX_TR3_TDM_SLOTS           380

#define WC40_UCMEM_DATA_ENTRY_LEN    16

#define _TR3_MMU_CELLS_RSVD_IP       100

#define _TR3_MMU_MAX_PACKET_BYTES    10240 /* bytes */
#define _TR3_MMU_PACKET_HEADER_BYTES 64    /* bytes */
#define _TR3_MMU_JUMBO_FRAME_BYTES   9216  /* bytes */
#define _TR3_MMU_DEFAULT_MTU_BYTES   1536  /* bytes */

#define _TR3_MMU_TOTAL_CELLS_24K     24576 /* 24k cells */
#define _TR3_MMU_TOTAL_CELLS_19K     19456 /* 19k cells */
#define _HX4_MMU_TOTAL_CELLS_10K     10240 /* 10k cells */

#define _TR3_MMU_TOTAL_CELLS(unit)  _soc_tr3_mmu_buffer_cells[(unit)]
                
#define _TR3_MMU_BYTES_PER_CELL      208   /* bytes (1664 bits) */
#define _TR3_MMU_NUM_PG              8
#define _TR3_MMU_NUM_POOL            4

#define _MAX_HX4_FLEX_PORT_PROPS    12

/* Default ISM mem config sizes 
 * These are scaled down automatically for various SKU's 
 */
#define _ISM_MEM_VLAN_XLATE_SIZE     (32*1024)/6
#define _ISM_MEM_L2_SIZE             (64*1024)/6
#define _ISM_MEM_L3_SIZE             (32*1024)/6
#define _ISM_MEM_EP_VLAN_XLATE_SIZE  (16*1024)/6
#define _ISM_MEM_MPLS_SIZE           (32*1024)/6

#define _MEM_NOT_APPLICABLE -1


#define _SOC_HX4_DEFIP_MAX_TCAMS   32
#define _SOC_HX4_DEFIP_TCAM_DEPTH  1024

typedef enum { 
    _SOC_PARITY_TYPE_NONE,
    _SOC_PARITY_TYPE_GENERIC,
    _SOC_PARITY_TYPE_PARITY,
    _SOC_PARITY_TYPE_ECC,
    _SOC_PARITY_TYPE_MMU_THDO,
    _SOC_PARITY_TYPE_MMU_THDI,
    _SOC_PARITY_TYPE_MMU_WRED,
    _SOC_PARITY_TYPE_MMU_QCN,
    _SOC_PARITY_TYPE_MMU_LLS,
    _SOC_PARITY_TYPE_AXP,
    _SOC_PARITY_TYPE_BST,
    _SOC_PARITY_TYPE_SER,
    _SOC_PARITY_INFO_TYPE_OAM,
    _SOC_PARITY_INFO_TYPE_NUM
} _soc_hx4_parity_info_type_t;

char *_soc_hx4_ser_hwmem_base_info[] = {
    "CMIC PKT BUFFER - In Iarb",
    "CPU PKT BUFFER - In Iarb",
    "AXP PACKET BUFFER - In Iarb",
    "IFP COUNTER MUX DATA STAGING - In Ifp",
    "INGRESS PACKET BUFFER - In Ipars",
    "ESM AUX1 FIFO - In Iesmif",
    "ESM CTRL BUS FIFO - In Iesmif",
    "ESM ADM CTRL FIFO - In Iesmif",
    "ESM ET RSP FIFO - In Iesmif",
    "ESM CTRL BUS FIFO 1 - In Iesmif",
    "ESM SER STATUS BUS FIFO - In Iesmif",
    "ISW2 EOP BUFFER A - In Iesmif",
    "ISW2 EOP BUFFER B - In Iesmif",
    "ISW2 EOP BUFFER C - In Iesmif",
    "Invalid value",
    "Invalid value",
    "EP MPB DATA - In El3",
    "EP INITBUF - In Ehcpm",
    "CM DATA BUFFER - In Edatabuf",
    "XLP0 DATA BUFFER - In Edatabuf",
    "Invalid value",
    "XTP0 DATA BUFFER - In Edatabuf",
    "XTP1 DATA BUFFER - In Edatabuf",
    "XTP2 DATA BUFFER - In Edatabuf",
    "XTP3 DATA BUFFER - In Edatabuf",
    "XWP0 DATA BUFFER - In Edatabuf",
    "XWP1 DATA BUFFER - In Edatabuf",
    "XWP2 DATA BUFFER - In Edatabuf",
    "XTP RESI DATA BUFFER - In Edatabuf",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "IPARS - PassThrought bus parity check",
    "IVXLT - PassThrought bus parity check",
    "IMPLS - PassThrought bus parity check",
    "IL2L3 - PassThrought bus parity check",
    "Invalid value",
    "IFP - PassThrought bus parity check",
    "IRSEL1 - PassThrought bus parity check",
    "ISW1 - PassThrought bus parity check",
    "IRSEL2 - PassThrought bus parity check",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "Invalid value",
    "EGR VLAN - PassThrought bus parity check",
    "EGR HCPM - PassThrought bus parity check",
    "EGR PMOD - PassThrought bus parity check",
    "EGR FPPARS - PassThrought bus parity check",
    "EFP - PassThrought bus parity check",
};

/* SER processing for TCAMs */
static _soc_generic_ser_info_t _soc_hx4_tcam_ser_info_template[] = {
    { VFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { MY_STATION_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 76}, {1, 76}, {77, 152}, {78, 152} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { L2_USER_ENTRYm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 61}, {1, 61}, {62, 122}, {63, 122} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { L3_TUNNELm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 106}, {1, 106}, {107, 212}, {108, 212} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { FP_GLOBAL_MASK_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 88}, {1, 88}, {89, 176}, {90, 176} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { VLAN_SUBNETm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 64}, {1, 64}, {65, 128}, {66, 128} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { L3_DEFIP_PAIR_128m, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_8BITS, _SOC_SER_INTERLEAVE_NONE,
      { {0, 95}, {96, 189}, {190, 285}, {286, 379} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_NO_DMA |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY},
    { L3_DEFIPm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 95}, {1, 95}, {96, 189}, {97, 189} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_NO_DMA |
      _SOC_SER_FLAG_REMAP_READ | _SOC_SER_FLAG_SIZE_VERIFY},
    { FP_UDF_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 80}, {1, 80}, {81, 160}, {82, 160} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { CPU_COS_MAPm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 112}, {1, 112}, {113, 224}, {114, 224} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { EFP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { FP_TCAMm, INVALIDm, _SOC_SER_TYPE_PARITY,
      _SOC_SER_PARITY_4BITS, _SOC_SER_INTERLEAVE_MOD2,
      { {0, 235}, {1, 235}, {236, 469}, {237, 469} }, 0, 0, 0, 0,
      _SOC_SER_FLAG_XY_READ | _SOC_SER_FLAG_DISCARD_READ},
    { INVALIDm },
};

_soc_generic_ser_info_t *_soc_hx4_tcam_ser_info[SOC_MAX_NUM_DEVICES];

STATIC void
_soc_hx4_ser_info_flag_update(_soc_generic_ser_info_t *ser_info,
                              soc_mem_t mem, uint32 flag, int enable)
{
    int ser_idx = 0;

    while (INVALIDm != ser_info[ser_idx].mem) {
        if (ser_info[ser_idx].mem == mem) {
            if (enable) {
                ser_info[ser_idx].ser_flags |= flag;
            } else {
                ser_info[ser_idx].ser_flags &= ~flag;
            }
        }
        ser_idx++;
    }
}

int
_soc_hx4_tcam_ser_init(int unit)
{
    int rv = 0;
    int alloc_size;
    _soc_generic_ser_info_t *ser_info;

    /* First, make per-unit copy of the master TCAM list */
    alloc_size = sizeof(_soc_hx4_tcam_ser_info_template);
    if (NULL == _soc_hx4_tcam_ser_info[unit]) {
        if ((_soc_hx4_tcam_ser_info[unit] =
             sal_alloc(alloc_size, "hx4 tcam list")) == NULL) {
            return SOC_E_MEMORY;
        }
    }
    /* Make a fresh copy of the TCAM template info */
    sal_memcpy(_soc_hx4_tcam_ser_info[unit],
               &(_soc_hx4_tcam_ser_info_template),
               alloc_size);

    /* Now make per-unit modifications */
    ser_info = _soc_hx4_tcam_ser_info[unit];

    if (soc_feature(unit, soc_feature_field_stage_half_slice) ||
        soc_feature(unit, soc_feature_field_stage_ingress_256_half_slice) ||
        soc_feature(unit, soc_feature_field_slice_size128) ||
        soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
        _soc_hx4_ser_info_flag_update(ser_info, FP_GLOBAL_MASK_TCAMm,
                                      _SOC_SER_FLAG_NO_DMA, TRUE);
        _soc_hx4_ser_info_flag_update(ser_info, FP_TCAMm,
                                      _SOC_SER_FLAG_NO_DMA, TRUE);
    }

    if (soc_feature(unit, soc_feature_field_stage_half_slice) ||
        soc_feature(unit, soc_feature_field_stage_egress_256_half_slice) ||
        soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
        _soc_hx4_ser_info_flag_update(ser_info, EFP_TCAMm,
                                      _SOC_SER_FLAG_NO_DMA, TRUE);
    }

    if (soc_feature(unit, soc_feature_field_stage_lookup_512_half_slice) ||
        soc_feature(unit, soc_feature_field_stage_quarter_slice)) {
        _soc_hx4_ser_info_flag_update(ser_info, VFP_TCAMm,
                                      _SOC_SER_FLAG_NO_DMA, TRUE);
    }

    rv = soc_generic_ser_init(unit, _soc_hx4_tcam_ser_info[unit]);
#ifdef INCLUDE_MEM_SCAN
    soc_mem_scan_ser_list_register(unit, TRUE, _soc_hx4_tcam_ser_info[unit]);
#endif
    return rv;
}

void
soc_hx4_ser_fail(int unit)

{
    soc_generic_ser_process_error(unit, _soc_hx4_tcam_ser_info[unit],
                                  _SOC_PARITY_TYPE_SER);
}

int
soc_hx4_pipe_mem_clear(int unit)
{
    uint32              rval, index;
    int                 pipe_init_usec;
    soc_timeout_t       to;
    int                 tcam_protect_write;

    static const soc_mem_t cam[] = {
        L3_TUNNELm,
        FP_UDF_TCAMm,
        VLAN_SUBNET_ONLYm,
        VFP_TCAMm,
        MY_STATION_TCAM_ENTRY_ONLYm,
        L2_USER_ENTRY_ONLYm,
        L3_DEFIP_ONLYm,
        L3_DEFIP_PAIR_128_ONLYm,
        ESM_PKT_TYPE_IDm,
        ESM_PKT_TYPE_ID_ONLYm,
        FP_TCAMm,
        FP_GLOBAL_MASK_TCAMm,
        CPU_COS_MAP_ONLYm,
        EFP_TCAMm
    };

    static const soc_mem_t axp[] = {
        AXP_WTX_TUNNELm,
        AXP_WRX_WCDm,
        AXP_WRX_SVP_ASSIGNMENTm,
        AXP_WTX_DVP_PROFILEm,
        AXP_WTX_PRI_MAPm,
        AXP_WTX_FRAG_IDm
    };

    /*
     * Reset the IPIPE and EPIPE block
     */
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_1r(unit, rval));
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    /* Set count to # entries in largest IPIPE table */
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 65536);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest EPIPE table */
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 65536);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ISM_HW_RESET_CONTROL_0r(unit, rval));
    soc_reg_field_set(unit, ISM_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ISM_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    /* Set count to # entries in largest ISM table */
    soc_reg_field_set(unit, ISM_HW_RESET_CONTROL_1r, &rval, COUNTf, 131072);
    SOC_IF_ERROR_RETURN(WRITE_ISM_HW_RESET_CONTROL_1r(unit, rval));
    
    /* Clear AXP block mems */
    rval = 0;
    soc_reg_field_set(unit, AXP_WRX_MEMORY_BULK_RESETr, &rval, START_RESETf, 1);
    SOC_IF_ERROR_RETURN(WRITE_AXP_WRX_MEMORY_BULK_RESETr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_AXP_WTX_MEMORY_BULK_RESETr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_MEMORY_BULK_RESETr(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);
    /* Wait for IPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ING_HW_RESET_CONTROL_2r(unit, &rval));
        if (soc_reg_field_get(unit, ING_HW_RESET_CONTROL_2r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ING_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);
    soc_timeout_init(&to, pipe_init_usec, 0);
    /* Wait for EPIPE memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_EGR_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, EGR_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : EGR_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);
    soc_timeout_init(&to, pipe_init_usec, 0);
    /* Wait for ISM memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_ISM_HW_RESET_CONTROL_1r(unit, &rval));
        if (soc_reg_field_get(unit, ISM_HW_RESET_CONTROL_1r, rval, DONEf)) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : ISM_HW_RESET timeout\n"), unit));
            break;
        }
    } while (TRUE);
    soc_timeout_init(&to, pipe_init_usec, 0);
    /* Wait for AXP WLAN RX memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_AXP_WRX_MEMORY_BULK_RESETr(unit, &rval));
        if (soc_reg_field_get(unit, AXP_WRX_MEMORY_BULK_RESETr, rval, RESET_DONEf) ||
            SAL_BOOT_PLISIM) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : AXP_WRX_MEMORY timeout\n"), unit));
            break;
        }
    } while (TRUE);
    soc_timeout_init(&to, pipe_init_usec, 0);
    /* Wait for AXP WLAN TX memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_AXP_WTX_MEMORY_BULK_RESETr(unit, &rval));
        if (soc_reg_field_get(unit, AXP_WTX_MEMORY_BULK_RESETr, rval, RESET_DONEf) ||
            SAL_BOOT_PLISIM) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : AXP_WTX_MEMORY timeout\n"), unit));
            break;
        }
    } while (TRUE);
    soc_timeout_init(&to, pipe_init_usec, 0);
    /* Wait for AXP SM memory initialization done. */
    do {
        SOC_IF_ERROR_RETURN(READ_AXP_SM_MEMORY_BULK_RESETr(unit, &rval));
        if (soc_reg_field_get(unit, AXP_SM_MEMORY_BULK_RESETr, rval, RESET_DONEf) ||
            SAL_BOOT_PLISIM) {
            break;
        }
        if (soc_timeout_check(&to)) {
            LOG_WARN(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                                 "unit %d : AXP_SM_MEMORY timeout\n"), unit));
            break;
        }
    } while (TRUE);
    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_ISM_HW_RESET_CONTROL_1r(unit, rval));
    rval = 0;
    soc_reg_field_set(unit, AXP_WRX_MEMORY_BULK_RESETr, &rval, START_RESETf, 0);
    SOC_IF_ERROR_RETURN(WRITE_AXP_WRX_MEMORY_BULK_RESETr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_AXP_WTX_MEMORY_BULK_RESETr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_AXP_SM_MEMORY_BULK_RESETr(unit, rval));

    if (!SAL_BOOT_SIMULATION || SAL_BOOT_BCMSIM) {
        /* TCAM tables are not handled by hardware reset control */
        tcam_protect_write = SOC_CONTROL(unit)->tcam_protect_write;
        SOC_CONTROL(unit)->tcam_protect_write = FALSE;
        for (index = 0; index < sizeof(cam) / sizeof(soc_mem_t); index++) {
            if (SOC_MEM_IS_VALID(unit, cam[index])) {
                if (SAL_BOOT_BCMSIM &&
                    !((cam[index] == VLAN_SUBNET_ONLYm) ||
                    (cam[index] == VFP_TCAMm) ||
                    (cam[index] == L2_USER_ENTRY_ONLYm))) {
                    continue;
                }
                SOC_IF_ERROR_RETURN(soc_mem_clear(unit, cam[index],
                                                  COPYNO_ALL, TRUE));
            }
        }
        SOC_CONTROL(unit)->tcam_protect_write = tcam_protect_write;
    }
    if (!SAL_BOOT_SIMULATION) {
        for (index = 0; index < sizeof(axp) / sizeof(soc_mem_t); index++) {
            SOC_IF_ERROR_RETURN(soc_mem_clear(unit, axp[index], COPYNO_ALL, TRUE));
        }
    }
    return SOC_E_NONE;
}

int
soc_hx4_init_port_mapping(int unit)
{
    soc_info_t *si;
    soc_mem_t mem;
    uint32 rval;
    ing_physical_to_logical_port_number_mapping_table_entry_t entry;
    int port, phy_port, mmu_port;
    int num_port, num_phy_port, num_mmu_port;

    si = &SOC_INFO(unit);

    /* Ingress physical to logical port mapping */
    mem = ING_PHYSICAL_TO_LOGICAL_PORT_NUMBER_MAPPING_TABLEm;
    num_phy_port = soc_mem_index_count(unit, mem);
    sal_memset(&entry, 0, sizeof(entry));
    for (phy_port = 0; phy_port < num_phy_port; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        soc_mem_field32_set(unit, mem, &entry, LOGICAL_PORT_NUMBERf,
                            port == -1 ? 0x7f : port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, phy_port, &entry));
    }
    num_port = soc_mem_index_count(unit, PORT_TABm);
    /* Egress logical to physical port mapping */
    for (port = 0; port < num_port; port++) {
        phy_port = si->port_l2p_mapping[port];
        rval = 0;
        soc_reg_field_set(unit, EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr,
                          &rval, PHYSICAL_PORT_NUMBERf,
                          phy_port == -1 ? 0x7f : phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_LOGICAL_TO_PHYSICAL_PORT_NUMBER_MAPPINGr(unit, port,
                                                                rval));
    }

    /* MMU to physical port mapping and MMU to logical port mapping */
    num_mmu_port = SOC_REG_NUMELS(unit, MMU_TO_PHY_PORT_MAPPINGr);
    for (mmu_port = 0; mmu_port < num_mmu_port; mmu_port++) {
        phy_port = si->port_m2p_mapping[mmu_port];
        port = phy_port == -1 ? -1 : si->port_p2l_mapping[phy_port];

        rval = 0;
        soc_reg_field_set(unit, MMU_TO_PHY_PORT_MAPPINGr, &rval, PHY_PORTf,
                          phy_port == -1 ? 0x7f : phy_port);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_TO_PHY_PORT_MAPPINGr(unit, mmu_port, rval));

        rval = 0;
        soc_reg_field_set(unit, MMU_TO_LOGIC_PORT_MAPPINGr, &rval, LOGIC_PORTf,
                          port == -1 ? 0x3f : port);
        SOC_IF_ERROR_RETURN
            (WRITE_MMU_TO_LOGIC_PORT_MAPPINGr(unit, mmu_port, rval));
    }

    return SOC_E_NONE;
}

int
soc_hx4_init_num_cosq(int unit)
{
    soc_info_t *si;
    int port;
    int mc_numq, mc_base;
    int uc_numq, uc_base;

    si = &SOC_INFO(unit);

    SOC_PBMP_CLEAR(si->eq_pbm);
    PBMP_ALL_ITER(unit, port) {
        if (IS_HG_PORT(unit, port)) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, port);
        }
    }

    PBMP_ALL_ITER(unit, port) {
        soc_tr3_get_def_qbase(unit, port, _SOC_TR3_INDEX_STYLE_UCAST_QUEUE,
                                &uc_base, &uc_numq);
        soc_tr3_get_def_qbase(unit, port, _SOC_TR3_INDEX_STYLE_MCAST_QUEUE,
                                &mc_base, &mc_numq);
        si->port_num_cosq[port] = mc_numq;
        si->port_cosq_base[port] = mc_base;
        si->port_num_uc_cosq[port] = uc_numq;
        si->port_uc_cosq_base[port] = uc_base;
        si->port_group[port] = -1;
#if 0
        LOG_CLI((BSL_META_U(unit,
                            "Port-%d : UC: %d:%d, MC: %d:%d, EQ:%d:%d\n"),
                 port, uc_base, uc_numq, mc_base, mc_numq,
                 eq_base, eq_numq));
#endif
    }

    return SOC_E_NONE;
}

typedef struct {
    uint16 dev_id;
    uint8 opp_spacing;
    int def_cfg;
    int cfg[_MAX_HX4_PORT_CFG];
} _soc_hx4_port_cfg_t;

typedef struct {
    uint16 dev_id;
    char *props[_MAX_HX4_FLEX_PORT_PROPS];
} _soc_hx4_flex_port_cfg_t;

static _soc_hx4_flex_port_cfg_t hx4_flex_props[] = {
    { BCM56548H_DEVICE_ID, { NULL } },
    { BCM56548_DEVICE_ID, { NULL } },
    { BCM56547_DEVICE_ID, { spn_BCM56547_3X42, spn_BCM56547_2X42, NULL } },
    { BCM56346_DEVICE_ID, { spn_BCM56340_4X10, spn_BCM56346_4X10_2X21, NULL } },
    { BCM56345_DEVICE_ID, { spn_BCM56340_4X10, spn_BCM56345_4X10_2X21, spn_BCM56345_2X21, NULL } },
    { BCM56344_DEVICE_ID, { spn_BCM56344_2X10, NULL } },
    { BCM56342_DEVICE_ID, { spn_BCM56340_4X10, NULL } },
    { BCM56340_DEVICE_ID, { spn_BCM56340_4X10, spn_BCM56340_2X10,
                            "REGRESSION", "EMULATION", NULL } },
    { BCM56049_DEVICE_ID, { NULL } },
    { BCM56048_DEVICE_ID, { NULL } },
    { BCM56047_DEVICE_ID, { NULL } },
    { BCM56042_DEVICE_ID, { NULL } },
    { BCM56041_DEVICE_ID, { NULL } },
    { BCM56040_DEVICE_ID, { NULL } }
};

int16 _hx4_xc_flex_port_speeds[]     = {  3,  1, -1 };
int16 _hx4_xc_flex_port_lanes[]      = {  4,  1, -1 };
int16 _hx4_xc_def_flex_port_speeds[] = {  3,  0,  1 };
int16 _hx4_wc_flex_port_speeds[]     = { 21, 21, 10, 11, 10, 3, 1, -1 }; 
int16 _hx4_wc_flex_port_lanes[]      = {  4,  2,  2,  1,  1, 1, 1, -1 }; 
int16 _hx4_wc_flex_port_speeds2[]     = { 21,  11, 10, -1 };
int16 _hx4_wc_flex_port_lanes2[]      = {2, 1,  1,  -1 };
int16 _hx4_wc_def_flex_port_speeds[] = { 21, 21, 11 };


/* HX4 PORT Configs:

F.HG[42]   = Hot Swap between HG[42]/2xHGd[21]/2xRXAUI/4xHGs[11]/4xXFI/4xKR ?/4x1GE/4x2.5GE
F.HG[21]   = Hot Swap between HG[21]/XAUI/4x1GE/4x2.5G
Flex[4x10] = Hot Swap between HG[21]/2xHGd[21]/2xRXAUI/4xHGs[11]/4xXFI/4xKR ?/4x1GE/4x2.5GE 
F.QSGMII   = NO Hot Swap between 1xQSGMII (4x1G) or 1x2.5GE/1x1GE(SGMII)

-------------------------------------------------------------------------------------------------------------------------------
    dev-id   | cfg-id |           configuration                                                |    property        |  Hot swap
-------------------------------------------------------------------------------------------------------------------------------
 FireScout:  |        |                                                                        |                    |
 =========   |        |                                                                        |                    |
BCM56548H    |  492   | HX4_1G_200_D*: 5xF.QSGMII + 2x(2xHGd[21]/4xHGs[11]) + 1GE              | none               |    yes
BCM56548     |  491   | HX4_1G_200_D*: 7xF.QSGMII + 3xF.HG[42] + 1GE                           | none               |    yes
             |        |                                                                        |                    |
BCM56547     |  490   | HX4_1G_200_D: 10xF.QSGMII + 3xF.HG[42] + 1GE                           | bcm56547_3x42      |    yes
             |  500   | HX4_1G_200_E: 12xF.QSGMII + 2xF.HG[42] + 1GE                           | bcm56547_2x42      |    yes
             |  495   | HX4_1G_200_A, HX4_1G_200_B, HX4_1G_200_C:                              |                    |
             |        |               12xF.QSGMII + F.HG[42] + 2xHG[42] + 1GE                  | none               |    yes
             |        |                                                                        |                    |
 Helix4:     |        |                                                                        |                    |
 ======      |        |                                                                        |                    |
BCM56346     |  462   | HX4_1G_130*: 7xF.QSGMII + Flex[4x10] + 2xHG[21] + 1GE                  | none               |    yes
             |  463   | HX4_1G_130_A:7xF.QSGMII +Flex[4x10]+ 2x(1xHGd[21]/2xHGs[11/10]) + 1GE  | bcm56346_4X10_2x21 |    yes
             |        |                                                                        |                    |
             |        |                                                                        |                    |
BCM56345     |  412   | HX4_1G_150*  : 12xF.QSGMII + Flex[4x10] + 2xHG[21] + 1GE               |                    |    yes
             |  413   | HX4_1G_150_E  : 12xF.QSGMII+Flex[4x10]+2x(1xHGd[21]/2xHGs[11/10])+1GE  | bcm56345_4X10_2x21 |    yes
             |  414   | HX4_1G_150_F  : 12xF.QSGMII + 2x(2xHGd[21]/4xHGs[11/10]) + 1GE         | bcm56345_2x21      |    yes
             |        |                                                                        |                    |
BCM56344     |  497   | HX4_1G_200_D*: 10xF.QSGMII + 3xFlex[4x10] + 1GE                        | none               |    yes
             |  510   | HX4_1G_200_E*: 12xF.QSGMII + 2xFlex[4x10] + 1GE                        | bcm56344_2x10      |    yes
             |        |                                                                        |                    |
BCM56342     |  460   | HX4_1G_130: 7xF.QSGMII + Flex[4x10] + 2xHG[21] + 1GE                   | none               |    yes
             |  464   | HX4_1G_130: 7xF.QSGMII + Flex[4x10] + 2xFlex[21G/1G] + 1GE             | none               |    yes
             |        |                                                                        |                    |
BCM56340     |  410   | HX4_1G_150_B, HX4_1G_150_C:                                            |                    |
             |        |                12xF.QSGMII + Flex[4x10] + 2xHG[21]/1GE + 1GE           | bcm56340_4x10      |    yes
             |  400   | HX4_1G_150_A:  12xF.QSGMII + 4xSGMII + 2xXFI + 2xHGd[21] + 1GE         | none               |    no
             |  610   | HX4_1G_200_E*: 12xF.QSGMII + 2xFlex[4x10] + 1GE                        | bcm56340_2x10      |    yes
             |        |                                                                        |                    |
 Spiral:     |        |                                                                        |                    |
 ======      |        |                                                                        |                    |
BCM56049     |  420   | RGR_XFI_150: 1xF.QSGMII + 3xFlex[4x10] + 1GE                           | none               |    yes
             |        |                                                                        |                    |
BCM56048     |  514   | HX4_1G_200_E*: 7xF.QSGMII + 2xFlex[4x10]] + 1GE                        | none               |    yes
             |        |                                                                        |                    |
BCM56047     |  510   | HX4_1G_200_E*: 12xF.QSGMII + 2xFlex[4x10] + 1GE                        | none               |    yes
             |        |                                                                        |                    |
 Ranger:     |        |                                                                        |                    |
 ======      |        |                                                                        |                    |
BCM56042     |   11   | RGR_2P5G_150: 12x2.5GE/1GE + 12x2.5GE/1GE + 1GE                        | none               |    no
             |        |                                                                        |                    |
BCM56041     |  420   | RGR_XFI_150: 1xF.QSGMII + 3xFlex[4x10] + 1GE                           | none               |    yes
             |        |                                                                        |                    |
BCM56040     |  450   | RGR_XFI_200: 1xF.QSGMII + 3xF.HG[42] + 1GE                             | none               |    yes
             |        |                                                                        |                    |
-----------------------------------------------------------------------------------------------------------------------------

Note: use property portgroup_n=<lanes> for init time flex port selection/config."
Note: use property serdes_asymmetric_speed_mode=1 for GE ports to support 1/2.5G in a group */

void
_soc_hx4_resolve_port_config(int unit, int dev_id) {

    static _soc_hx4_port_cfg_t hx4_port_cfg[] = {

        { BCM56548H_DEVICE_ID, 3, 492,  {  -1 } },
        { BCM56548_DEVICE_ID,  3, 491,  {  -1 } },
        { BCM56547_DEVICE_ID,  3, 495,  {  490, 500, -1 } },
        { BCM56346_DEVICE_ID,  3, 462,  {  6, 463, -1 } },
        { BCM56345_DEVICE_ID,  3, 412,  {  1, 413, 414, -1 } },
        { BCM56344_DEVICE_ID,  3, 497,  {  510, -1 } },
        { BCM56342_DEVICE_ID,  3, 460,  {  464, -1 } },
        { BCM56340_DEVICE_ID,  3, 400,  {  410, 610, 1, 30, -1 } },
        { BCM56049_DEVICE_ID,  3, 420,  {  -1 } },
        { BCM56048_DEVICE_ID,  3, 514,  {  -1 } },
        { BCM56047_DEVICE_ID,  3, 510,  {  -1 } },
        { BCM56042_DEVICE_ID,  3,  11,  {  -1 } },
        { BCM56041_DEVICE_ID,  3, 420,  {  -1 } },
        { BCM56040_DEVICE_ID,  3, 450,  {  -1 } }
    };
    uint8 entries;
    int i, c, config, cfg_id = 0;

    config = soc_property_get(unit, spn_BCM56340_CONFIG, 0);
    if (config) {
        _tr3_port_config_id[unit] = config;
        _tr3_flex_port_data[unit].dev_id = dev_id;
        return;
    }

    entries = COUNTOF(hx4_flex_props);
    for (i = 0; i < entries; i++) {
        if (dev_id == hx4_flex_props[i].dev_id) {
            _tr3_tdm_opp_spacing[unit] = hx4_port_cfg[i].opp_spacing;
            cfg_id = hx4_port_cfg[i].def_cfg;
            for (c = 0; hx4_flex_props[i].props[c] != NULL ;c++) {
                config = soc_property_get(unit, hx4_flex_props[i].props[c], 0);
                if (config) {
                    cfg_id = hx4_port_cfg[i].cfg[c];
                    break;
                }
            }
            break;
        }
    }
    _tr3_port_config_id[unit] = cfg_id;
    _tr3_flex_port_data[unit].dev_id = dev_id;

}

/*
 * HX4 port mapping
 * cpu port number is fixed: physical 0, logical 0, mmu 0
 * loopback port numbers are fixed
 */
int
soc_hx4_get_port_mapping(int unit, uint16 dev_id)
{

    /* 52x1GE + 4xHGd[21] + 1GE - Config for AV */
    static const int p2l_mapping_30[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, -1, -1,
         56, -1, 57, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_30[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, 53, -1, -1,
         54, -1, 55, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_30[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
          1,  1,  1,  1,
         12, 12, -1, -1,
         21, -1, 21, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };


    /* 52x1GE + 2xXFI + 2xHGd[21] + 1GE */
    static const int p2l_mapping_0[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, -1, -1,
         56, -1, 57, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_0[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, 53, -1, -1,
         54, -1, 55, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_0[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
          1,  1,  1,  1,
         10, 10, -1, -1,
         21, -1, 21, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };

    /* 48x1GE + 4xXFI + 2xHG[21] + 1GE */
    static const int p2l_mapping_1[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_1[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, -1, -1, -1,
         53, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_1[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
         10, 10, 10, 10,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };

    /* 4x1GE + 12xXFI + 1GE */
    static const int p2l_mapping_2[] = {
          0,
          1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          5, -1, -1, -1,
          9, 10, 11, 12,
         13, 14, 15, 16,
         17, 18, 19, 20,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_2[] = {
         59,
          1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         44, 45, 46, 47,
         48, 49, 50, 51,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_2[] = {
         -1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         10, 10, 10, 10,
         10, 10, 10, 10,
         10, 10, 10, 10,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };

    /* 48x1GE + 4xXFI + 2xHG[42] + 1GE */
    static const int p2l_mapping_3[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_3[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, -1, -1, -1,
         53, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_3[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
         10, 10, 10, 10,
         42, -1, -1, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };


    /* 48x1GE + 1x40G + 2xHG[42] + 1GE */
    static const int p2l_mapping_4[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_4[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, -1, -1, -1,
         52, -1, -1, -1,
         53, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_4[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
         40, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };

    /* 4x1GE + 3xHG[42] + 1GE */
    static const int p2l_mapping_5[] = {
          0,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1,  1,  2,  3,  4,
          5, -1, -1, -1,
          9, -1, -1, -1,
         13, -1, -1, -1,
         17, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_5[] = {
         59,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, 40, 41, 42, 43,
         60, -1, -1, -1,
         44, -1, -1, -1,
         48, -1, -1, -1,
         52, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_5[] = {
         -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1,  1,  1,  1,  1,
          1, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };

    /* 28x1GE + 4xXFI + 2xHG[21] + 1GE */
    static const int p2l_mapping_6[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_6[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, -1, -1, -1,
         53, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_6[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         10, 10, 10, 10,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };

    /* 12x2.5G (SGMII) + 4xXFI + 2xHG[21] + 1GE */
    static const int p2l_mapping_7[] = {
          0,
          1, -1, -1, -1,  5, -1, -1, -1,  9, -1, -1, -1,
         13, -1, -1, -1, 17, -1, -1, -1, 21, -1, -1, -1,
         25, -1, -1, -1, 29, -1, -1, -1, 33, -1, -1, -1,
         37, -1, -1, -1, 41, -1, -1, -1, 45, -1, -1, -1,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_7[] = {
         59,
          0, -1, -1, -1,  4,  -1, -1, -1, 8, -1, -1, -1,
         12, -1, -1, -1, 16, -1, -1, -1, 20, -1, -1, -1,
         24, -1, -1, -1, 28, -1, -1, -1, 32, -1, -1, -1,
         36, -1, -1, -1, 40, -1, -1, -1, 44, -1, -1, -1,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, -1, -1, -1,
         53, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_7[] = {
         -1,
          1, -1, -1, -1,  1, -1, -1, -1,  1, -1, -1, -1,
          1, -1, -1, -1,  1, -1, -1, -1,  1, -1, -1, -1,
          1, -1, -1, -1,  1, -1, -1, -1,  1, -1, -1, -1,
          1, -1, -1, -1,  1, -1, -1, -1,  1, -1, -1, -1,
          1, -1, -1, -1,
         10, 10, 10, 10,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    /* 12x2.5G (SGMII) + 1x40G + 2xHG[42] + 1GE */
    static const int p2l_mapping_8[] = {
          0,
          1, -1, -1, -1,  5, -1, -1, -1,  9, -1, -1, -1,
         13, -1, -1, -1, 17, -1, -1, -1, 21, -1, -1, -1,
         25, -1, -1, -1, 29, -1, -1, -1, 33, -1, -1, -1,
         37, -1, -1, -1, 41, -1, -1, -1, 45, -1, -1, -1,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_8[] = {
         59,
          0, -1, -1, -1,  4, -1, -1, -1, 8, -1, -1, -1,
         12, -1, -1, -1, 16, -1, -1, -1, 20, -1, -1, -1,
         24, -1, -1, -1, 28, -1, -1, -1, 32, -1, -1, -1,
         36, -1, -1, -1, 40, -1, -1, -1, 44, -1, -1, -1,
         60, -1, -1, -1,
         48, -1, -1, -1,
         52, -1, -1, -1,
         53, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_8[] = {
         -1,
          1, -1, -1, -1,  1, -1, -1, -1,  1, -1, -1, -1,
          1, -1, -1, -1,  1, -1, -1, -1,  1, -1, -1, -1,
          1, -1, -1, -1,  1, -1, -1, -1,  1, -1, -1, -1,
          1, -1, -1, -1,  1, -1, -1, -1,  1, -1, -1, -1,
          1, -1, -1, -1,
         40, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    /* 40x1GE + 12xXFI + 1GE */
    static const int p2l_mapping_9[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, 43, 44, 45,
         46, 47, 48, 49,
         50, 51, 52, 53,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_9[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 36, 37, 38, 39,
         41, 42, 43, 44, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 32, 49, 33,
         50, 34, 51, 35,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_9[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         10, 10, 10, 10,
         10, 10, 10, 10,
         10, 10, 10, 10,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    /* 24x1GE + 6x2.5GE + 2xHGd[21] + 1xHG[42] + 1GE */
    static const int p2l_mapping_10[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, -1, -1, -1, 29, -1, -1, -1, 33, -1, -1, -1,
         37, -1, -1, -1, 41, -1, -1, -1, 45, -1, -1, -1,
         49, -1, -1, -1,
         50, -1, 52, -1,
         54, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_10[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, -1, -1, -1, 28, -1, -1, -1, 32, -1, -1, -1,
         36, -1, -1, -1, 40, -1, -1, -1, 44, -1, -1, -1,
         60, -1, -1, -1,
         48, -1, 50, -1,
         52, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_10[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          3, -1, -1, -1,  3, -1, -1, -1,  3, -1, -1, -1,
          3, -1, -1, -1,  3, -1, -1, -1,  3, -1, -1, -1,
          1, -1, -1, -1,
         21, -1, 21, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    /* 12x2.5G (SGMII) + 12x2.5G + 1GE */
    static const int p2l_mapping_11[] = {
          0,
          1, -1, -1, -1,  2, -1, -1, -1,  3, -1, -1, -1,
          4, -1, -1, -1,  5, -1, -1, -1,  6, -1, -1, -1,
          7, -1, -1, -1,  8, -1, -1, -1,  9, -1, -1, -1,
         10, -1, -1, -1, 11, -1, -1, -1, 12, -1, -1, -1,
         13, -1, -1, -1,
         14, 15, 16, 17,
         18, 19, 20, 21,
         22, 23, 24, 25,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_11[] = {
         59,
          0, -1, -1, -1,  1,  -1, -1, -1, 2, -1, -1, -1,
          3, -1, -1, -1,  4, -1, -1, -1,  5, -1, -1, -1,
          6, -1, -1, -1,  7, -1, -1, -1,  8, -1, -1, -1,
          9, -1, -1, -1, 10, -1, -1, -1, 11, -1, -1, -1,
         60, -1, -1, -1,
         12, 13, 14, 15,
         16, 17, 18, 19,
         20, 21, 22, 23,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_11[] = {
         -1,
          3, -1, -1, -1,  3, -1, -1, -1,  3, -1, -1, -1,
          3, -1, -1, -1,  3, -1, -1, -1,  3, -1, -1, -1,
          3, -1, -1, -1,  3, -1, -1, -1,  3, -1, -1, -1,
          3, -1, -1, -1,  3, -1, -1, -1,  3, -1, -1, -1,
          1, -1, -1, -1,
          3,  3,  3,  3,
          3,  3,  3,  3,
          3,  3,  3,  3,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };

    /*492 : HX4_1G_200_D*: 5xF.QSGMII + 2x(2xHGd[21]/4xHGs[11]) + 1GE  */
    static const int p2l_mapping_492[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, 43, 44, 45,
         46, 47, 48, 49,
         50, 51, 52, 53,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_492[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, -1, -1, -1,
         46, -1, -1, -1,
         50, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_492[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 32, 49, 33,
         50, 34, 51, 35,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_492[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         42, -1, -1, -1,
         21, -1, 21, -1,
         21, -1, 21, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_492 = {
        1,
        { 0, 1, 2, 3, 4, 13, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, _hx4_wc_flex_port_speeds2,
          _hx4_wc_flex_port_speeds2, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, _hx4_wc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, _hx4_wc_flex_port_lanes2,
          _hx4_wc_flex_port_lanes2, NULL }
        };

    /* 7xF.QSGMII + 3xF.HG[42] + 1GE  */
    static const int p2l_mapping_491[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, 43, 44, 45,
         46, 47, 48, 49,
         50, 51, 52, 53,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_491[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, -1, -1, -1,
         46, -1, -1, -1,
         50, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_491[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 32, 49, 33,
         50, 34, 51, 35,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_491[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_491 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 13, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds,
          _tr3_wc_flex_port_speeds_1, _tr3_wc_flex_port_speeds_1,
          _tr3_wc_flex_port_speeds_1, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds,
          _tr3_wc_def_flex_port_speeds_1, _tr3_wc_def_flex_port_speeds_1,
          _tr3_wc_def_flex_port_speeds_1, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes,
          _tr3_wc_flex_port_lanes_1, _tr3_wc_flex_port_lanes_1,
          _tr3_wc_flex_port_lanes_1, NULL }
        };
    /* 10xF.QSGMII + 3xF.HG[42] + 1GE  */
    static const int p2l_mapping_490[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, 43, 44, 45,
         46, 47, 48, 49,
         50, 51, 52, 53,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_490[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, -1, -1, -1,
         46, -1, -1, -1,
         50, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_490[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 36, 37, 38, 39,
         41, 42, 43, 44, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 32, 49, 33,
         50, 34, 51, 35,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_490[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_490 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 13, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _tr3_wc_flex_port_speeds_1, _tr3_wc_flex_port_speeds_1,
          _tr3_wc_flex_port_speeds_1, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _tr3_wc_def_flex_port_speeds_1, _tr3_wc_def_flex_port_speeds_1,
          _tr3_wc_def_flex_port_speeds_1, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _tr3_wc_flex_port_lanes_1, _tr3_wc_flex_port_lanes_1,
          _tr3_wc_flex_port_lanes_1, NULL }
    }; 
    /* 12xF.QSGMII + 2xF.HG[42] + 1GE */
    static const int p2l_mapping_500[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, 56, 57,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_500[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_500[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_500[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         11, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_500 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _tr3_wc_flex_port_speeds_1, _tr3_wc_flex_port_speeds_1,
          NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _tr3_wc_def_flex_port_speeds_1, _tr3_wc_def_flex_port_speeds_1,
          NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _tr3_wc_flex_port_lanes_1, _tr3_wc_flex_port_lanes_1,
          NULL }
    }; 
    /* 12xF.QSGMII + F.HG[42] + 2xHG[42] + 1GE  */
    static const int p2l_mapping_495[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_495[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_495[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, -1, -1, -1,
         53, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_495[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_495 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _tr3_wc_flex_port_speeds_1, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _tr3_wc_def_flex_port_speeds_1, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _tr3_wc_flex_port_lanes_1, NULL }
        };
    /* 10xF.QSGMII + 3xFlex[4x10] + 1GE */
    static const int p2l_mapping_497[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, 43, 44, 45,
         46, 47, 48, 49,
         50, 51, 52, 53,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_497[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, -1, -1, -1,
         46, -1, -1, -1,
         50, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_497[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 36, 37, 38, 39,
         41, 42, 43, 44, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 32, 49, 33,
         50, 34, 51, 35,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_497[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_497 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 13, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, _hx4_wc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, _hx4_wc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, _hx4_wc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, NULL }
    }; 
    /* 12xF.QSGMII + 2xFlex[4x10] + 1GE */
    static const int p2l_mapping_510[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, 56, 57,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_510[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_510[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_510[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_510 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, _hx4_wc_flex_port_speeds,
          NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, _hx4_wc_def_flex_port_speeds,
          NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, _hx4_wc_flex_port_lanes,
          NULL }
    }; 
    /* 7xF.QSGMII + Flex[4x10] + 2xHG[21] + 1GE */
    static const int p2l_mapping_460[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_460[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_460[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, -1, -1, -1,
         53, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_460[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_460 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 13, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_wc_flex_port_speeds,
          NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_wc_def_flex_port_speeds,
          NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_wc_flex_port_lanes,
          NULL }
    };
    /*462 : 7xF.QSGMII + 2xFlex[4x10] + 1GE*/ 
    static const int port_speed_max_462[] = {
         -1,
          3,  1,  1,  1,  3,  1,  1,  1,  3,  1,  1,  1,
          3,  1,  1,  1,  3,  1,  1,  1,  3,  1,  1,  1,
          3,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };

    /*463: HX4_1G_130*:7xF.QSGMII +Flex[4x10]+ 2x(1xHGd[21]/2xHGs[11/10]) + 1GE  */
    static const int p2l_mapping_463[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, -1, -1,
         56, 57, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_463[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         56, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_463[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, 53, -1, -1,
         54, 55, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_463[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_463 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 13, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_wc_flex_port_speeds,
          _hx4_wc_flex_port_speeds2, _hx4_wc_flex_port_speeds2,
          NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_wc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, _hx4_wc_def_flex_port_speeds,
          NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_wc_flex_port_lanes,
          _hx4_wc_flex_port_lanes2, _hx4_wc_flex_port_lanes2,
          NULL }
    };



    static const int p2l_mapping_464[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, 43, 44, 45,
         46, 47, 48, 49,
         50, 51, 52, 53,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_464[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         41, -1, -1, -1,
         42, -1, -1, -1,
         46, -1, -1, -1,
         50, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_464[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, 28, 29, 30,
         53, 31, 32, 33,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_464 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 13, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, _tr3_wc_flex_port_speeds_2,
          _tr3_wc_flex_port_speeds_2, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, _tr3_wc_def_flex_port_speeds_2,
          _tr3_wc_def_flex_port_speeds_2, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, _tr3_wc_flex_port_lanes_2,
          _tr3_wc_flex_port_lanes_2, NULL }
    };

    /* 12xF.QSGMII + Flex[4x10] + 2xHG[21]/1GE + 1GE */
    static const int p2l_mapping_410[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_410[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         55, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_410[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, -1, -1, -1,
         53, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_410[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_410 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, NULL }
    }; 
    /* 12xF.QSGMII + Flex[4x10] + 2xHG[21] + 1GE */
    static const int port_speed_max_412[] = {
        -1,
        3, 1, 1, 1,  3, 1, 1, 1,  3, 1, 1, 1,
        3, 1, 1, 1,  3, 1, 1, 1,  3, 1, 1, 1,
        3, 1, 1, 1,  3, 1, 1, 1,  3, 1, 1, 1,
        3, 1, 1, 1,  3, 1, 1, 1,  3, 1, 1, 1,
        1, -1, -1, -1,
        21, -1, -1, -1,
        21, -1, -1, -1,
        21, -1, -1, -1,
        -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1
    };

   static _soc_tr3_flex_port_inf_t flex_port_inf_412 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, NULL }
    };
/* 413 : HX4_1G_150_B TDM#1 : 12xF.QSGMII+Flex[4x10]+2x(1xHGd[21]/2xHGs[11/10])+1GE */
    static const int p2l_mapping_413[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, -1, -1,
         56, 57, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_413[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         56, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_413[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, 53, -1, -1,
         54, 55, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_413[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_413 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, _hx4_wc_flex_port_speeds2,
          _hx4_wc_flex_port_speeds2, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, _hx4_wc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, _hx4_wc_flex_port_lanes2,
          _hx4_wc_flex_port_lanes2, NULL }
    };

     /* 414: HX4_1G_150_B  TDM#12: 12xF.QSGMII + 2x(2xHGd[21]/4xHGs[11/10]) + 1GE  */
    static const int p2l_mapping_414[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         -1, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, 56, 57,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_414[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         -1, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_414[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         -1, -1, -1, -1,
         48, 49, 50, 51,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_414[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
         -1, -1, -1, -1,
         21, -1, 21, -1,
         21, -1, 21, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_414 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
                                    _hx4_wc_flex_port_speeds2,
          _hx4_wc_flex_port_speeds2, NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
                                        _hx4_wc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
                                   _hx4_wc_flex_port_speeds2,
          _hx4_wc_flex_port_speeds2, NULL }
    };

    /* 12xF.QSGMII + 4xSGMII + 2xXFI + 2xHGd[21] + 1GE */
    static const int p2l_mapping_400[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, -1, -1,
         56, -1, 57, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_400[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36,
         37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, -1, -1,
         56, -1, 57, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_400[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
         36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, 53, -1, -1,
         54, -1, 55, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_400[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1, -1, -1, -1,
          3,  3,  3,  3,
         10, 10, -1, -1,
         21, -1, 21, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_400 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          NULL }
    };
    /* 1xF.QSGMII + 3xFlex[4x10] + 1GE */
    static const int p2l_mapping_420[] = {
          0,
          1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          5, -1, -1, -1,
          9, 10, 11, 12,
         13, 14, 15, 16,
         17, 18, 19, 20,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_420[] = {
          0,
          1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          5, -1, -1, -1,
          9, -1, -1, -1,
         13, -1, -1, -1,
         17, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_420[] = {
         59,
          1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, 
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         44, 45, 46, 47,
         48, 49, 50, 51,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_420[] = {
         -1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_420 = {
        1,
        { 0, 13, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, _hx4_wc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, NULL },
        { _hx4_xc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, _hx4_wc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, NULL },
        { _hx4_xc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, _hx4_wc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, NULL }
    }; 
    /* 7xF.QSGMII + 2xFlex[4x10] + 1GE */
    static const int p2l_mapping_514[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         49, -1, -1, -1,
         50, 51, 52, 53,
         54, 55, 56, 57,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_514[] = {
          0,
          1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12,
         13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
         25, 26, 27, 28, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         49, -1, -1, -1,
         50, -1, -1, -1,
         54, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_514[] = {
         59,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
         12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
         24, 25, 26, 27, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         48, 49, 50, 51,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_514[] = {
         -1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         21, -1, -1, -1,
         21, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_514 = {
        1,
        { 0, 1, 2, 3, 4, 5, 6, 13, 14, -1 },
        { _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds, _hx4_xc_flex_port_speeds,
          _hx4_xc_flex_port_speeds,
          _hx4_wc_flex_port_speeds, _hx4_wc_flex_port_speeds,
          NULL },
        { _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds, _hx4_xc_def_flex_port_speeds,
          _hx4_xc_def_flex_port_speeds,
          _hx4_wc_def_flex_port_speeds, _hx4_wc_def_flex_port_speeds,
          NULL },
        { _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes, _hx4_xc_flex_port_lanes,
          _hx4_xc_flex_port_lanes,
          _hx4_wc_flex_port_lanes, _hx4_wc_flex_port_lanes,
          NULL }
    }; 
    /* 1xF.QSGMII + 3xF.HG[42] + 1GE  */
    static const int p2l_mapping_450[] = {
          0,
          1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, 
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          5, -1, -1, -1,
          9, 10, 11, 12,
         13, 14, 15, 16,
         17, 18, 19, 20,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int flex_p2l_mapping_450[] = {
          0,
          1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1, 
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          5, -1, -1, -1,
          9, -1, -1, -1,
         13, -1, -1, -1,
         17, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         61, 59, 58, 60, 62
    };
    static const int p2m_mapping_450[] = {
         59,
          1,  2,  3,  4, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         60, -1, -1, -1,
         44, 45, 46, 47,
         48, 49, 50, 51,
         52, 53, 54, 55,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         58, 62, 56, 61, 57
    };
    static const int port_speed_max_450[] = {
         -1,
          1,  1,  1,  1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
          1, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         42, -1, -1, -1,
         -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1
    };
    static _soc_tr3_flex_port_inf_t flex_port_inf_450 = {
        1,
        { 0, 13, 14, 15, -1 },
        { _hx4_xc_flex_port_speeds,
          _tr3_wc_flex_port_speeds_1, _tr3_wc_flex_port_speeds_1,
          _tr3_wc_flex_port_speeds_1, NULL },
        { _hx4_xc_def_flex_port_speeds,
          _tr3_wc_def_flex_port_speeds_1, _tr3_wc_def_flex_port_speeds_1,
          _tr3_wc_def_flex_port_speeds_1, NULL },
        { _hx4_xc_flex_port_lanes,
          _tr3_wc_flex_port_lanes_1, _tr3_wc_flex_port_lanes_1,
          _tr3_wc_flex_port_lanes_1, NULL }
        };
    soc_info_t *si;
    int rv = SOC_E_NONE;
    int phy_port;
    soc_pbmp_t pbmp_xport_xe; 
    soc_pbmp_t pbmp_xport_ge;
    const int *p2l_mapping, *p2l_flex_mapping = NULL, *p2m_mapping, *speed_max;
    int flex_p2l_mapping[SOC_MAX_PHY_PORTS] = {-1},
        flex_speed_max[SOC_MAX_PHY_PORTS] = {0};

    sal_memset(&_tr3_flex_port_inf[unit], 0, sizeof(_soc_tr3_flex_port_inf_t));
    _soc_hx4_resolve_port_config(unit, dev_id);
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Port config id: %d\n"), _tr3_port_config_id[unit]));

    switch (_tr3_port_config_id[unit]) {
    case 0:
        p2l_mapping = p2l_mapping_0;
        p2m_mapping = p2m_mapping_0;
        speed_max = port_speed_max_0;
        break;
    case 1:
        p2l_mapping = p2l_mapping_1;
        p2m_mapping = p2m_mapping_1;
        speed_max = port_speed_max_1;
        break;
    case 2:
        p2l_mapping = p2l_mapping_2;
        p2m_mapping = p2m_mapping_2;
        speed_max = port_speed_max_2;
        break;
    case 3:
        p2l_mapping = p2l_mapping_3;
        p2m_mapping = p2m_mapping_3;
        speed_max = port_speed_max_3;
        break;
    case 4:
        p2l_mapping = p2l_mapping_4;
        p2m_mapping = p2m_mapping_4;
        speed_max = port_speed_max_4;
        break;
    case 5:
        p2l_mapping = p2l_mapping_5;
        p2m_mapping = p2m_mapping_5;
        speed_max = port_speed_max_5;
        break;
    case 6:
        p2l_mapping = p2l_mapping_6;
        p2m_mapping = p2m_mapping_6;
        speed_max = port_speed_max_6;
        break;
    case 7:
        p2l_mapping = p2l_mapping_7;
        p2m_mapping = p2m_mapping_7;
        speed_max = port_speed_max_7;
        break;
    case 8:
        p2l_mapping = p2l_mapping_8;
        p2m_mapping = p2m_mapping_8;
        speed_max = port_speed_max_8;
        break;
    case 9:
        p2l_mapping = p2l_mapping_9;
        p2m_mapping = p2m_mapping_9;
        speed_max = port_speed_max_9;
        break;
    case 10:
        p2l_mapping = p2l_mapping_10;
        p2m_mapping = p2m_mapping_10;
        speed_max = port_speed_max_10;
        break;
    case 11:
        p2l_mapping = p2l_mapping_11;
        p2m_mapping = p2m_mapping_11;
        speed_max = port_speed_max_11;
        break;
    case 30:
        p2l_mapping = p2l_mapping_30;
        p2m_mapping = p2m_mapping_30;
        speed_max = port_speed_max_30;
        break;
    case 491:
        p2l_mapping = p2l_mapping_491;
        p2m_mapping = p2m_mapping_491;
        speed_max = port_speed_max_491;
        p2l_flex_mapping = flex_p2l_mapping_491;
        _tr3_flex_port_inf[unit] = flex_port_inf_491;
        break;
    case 492:
        p2l_mapping = p2l_mapping_492;
        p2m_mapping = p2m_mapping_492;
        speed_max = port_speed_max_492;
        p2l_flex_mapping = flex_p2l_mapping_492;
        _tr3_flex_port_inf[unit] = flex_port_inf_492;
        break;
    case 490:
        p2l_mapping = p2l_mapping_490;
        p2m_mapping = p2m_mapping_490;
        speed_max = port_speed_max_490;
        p2l_flex_mapping = flex_p2l_mapping_490;
        _tr3_flex_port_inf[unit] = flex_port_inf_490;
        break;
    case 495:
        p2l_mapping = p2l_mapping_495;
        p2m_mapping = p2m_mapping_495;
        speed_max = port_speed_max_495;
        p2l_flex_mapping = flex_p2l_mapping_495;
        _tr3_flex_port_inf[unit] = flex_port_inf_495;
        break;
    case 500:
        p2l_mapping = p2l_mapping_500;
        p2m_mapping = p2m_mapping_500;
        speed_max = port_speed_max_500;
        p2l_flex_mapping = flex_p2l_mapping_500;
        _tr3_flex_port_inf[unit] = flex_port_inf_500;
        break;
    case 497:
        p2l_mapping = p2l_mapping_497;
        p2m_mapping = p2m_mapping_497;
        speed_max = port_speed_max_497;
        p2l_flex_mapping = flex_p2l_mapping_497;
        _tr3_flex_port_inf[unit] = flex_port_inf_497;
        break;
    case 510:
    case 610:
        p2l_mapping = p2l_mapping_510;
        p2m_mapping = p2m_mapping_510;
        speed_max = port_speed_max_510;
        p2l_flex_mapping = flex_p2l_mapping_510;
        _tr3_flex_port_inf[unit] = flex_port_inf_510;
        break;
    case 464:
        p2l_mapping = p2l_mapping_464;
        p2m_mapping = p2m_mapping_464;
        speed_max = port_speed_max_460;
        p2l_flex_mapping = flex_p2l_mapping_464;
        _tr3_flex_port_inf[unit] = flex_port_inf_464;
        break;
    case 462:
        p2l_mapping = p2l_mapping_460;
        p2m_mapping = p2m_mapping_460;
        speed_max = port_speed_max_462;
        p2l_flex_mapping = flex_p2l_mapping_460;
        _tr3_flex_port_inf[unit] = flex_port_inf_460;
        break;
    case 463:
        p2l_mapping = p2l_mapping_463;
        p2m_mapping = p2m_mapping_463;
        speed_max = port_speed_max_463;
        p2l_flex_mapping = flex_p2l_mapping_463;
        _tr3_flex_port_inf[unit] = flex_port_inf_463;
        break;
    case 460:
        p2l_mapping = p2l_mapping_460;
        p2m_mapping = p2m_mapping_460;
        speed_max = port_speed_max_460;
        p2l_flex_mapping = flex_p2l_mapping_460;
        _tr3_flex_port_inf[unit] = flex_port_inf_460;
        break;
    case 414:
        p2l_mapping = p2l_mapping_414;
        p2m_mapping = p2m_mapping_414;
        speed_max = port_speed_max_414;
        p2l_flex_mapping = flex_p2l_mapping_414;
        _tr3_flex_port_inf[unit] = flex_port_inf_414;
        break;
    case 413:
        p2l_mapping = p2l_mapping_413;
        p2m_mapping = p2m_mapping_413;
        speed_max = port_speed_max_413;
        p2l_flex_mapping = flex_p2l_mapping_413;
        _tr3_flex_port_inf[unit] = flex_port_inf_413;
        break;
    case 412:
        p2l_mapping = p2l_mapping_410;
        p2m_mapping = p2m_mapping_410;
        speed_max = port_speed_max_412;
        p2l_flex_mapping = flex_p2l_mapping_410;
        _tr3_flex_port_inf[unit] = flex_port_inf_412;
        break;
    case 410:
        p2l_mapping = p2l_mapping_410;
        p2m_mapping = p2m_mapping_410;
        speed_max = port_speed_max_410;
        p2l_flex_mapping = flex_p2l_mapping_410;
        _tr3_flex_port_inf[unit] = flex_port_inf_410;
        break;
    case 400:
        p2l_mapping = p2l_mapping_400;
        p2m_mapping = p2m_mapping_400;
        speed_max = port_speed_max_400;
        p2l_flex_mapping = flex_p2l_mapping_400;
        _tr3_flex_port_inf[unit] = flex_port_inf_400;
        break;
    case 420:
        p2l_mapping = p2l_mapping_420;
        p2m_mapping = p2m_mapping_420;
        speed_max = port_speed_max_420;
        p2l_flex_mapping = flex_p2l_mapping_420;
        _tr3_flex_port_inf[unit] = flex_port_inf_420;
        break;
    case 514:
        p2l_mapping = p2l_mapping_514;
        p2m_mapping = p2m_mapping_514;
        speed_max = port_speed_max_514;
        p2l_flex_mapping = flex_p2l_mapping_514;
        _tr3_flex_port_inf[unit] = flex_port_inf_514;
        break;
    case 450:
        p2l_mapping = p2l_mapping_450;
        p2m_mapping = p2m_mapping_450;
        speed_max = port_speed_max_450;
        p2l_flex_mapping = flex_p2l_mapping_450;
        _tr3_flex_port_inf[unit] = flex_port_inf_450;
        break;
    default:
        return SOC_E_PARAM;
    }

    /* Handle any init time flex port configs */
    if (_tr3_flex_port_inf[unit].valid) {
        uint8 idx, i, j, port_groups = 0;
        char *config_str, *sub_str, *sub_str_end;
        int num_lanes, port_bw;

        if (p2l_flex_mapping) {
            sal_memcpy(flex_p2l_mapping, p2l_flex_mapping,
                       sizeof(int) * SOC_MAX_PHY_PORTS);
        }
        sal_memcpy(flex_speed_max, speed_max, sizeof(int) * SOC_MAX_PHY_PORTS);
        for (i = 0; ; i++) {
            if (_tr3_flex_port_inf[unit].flex_port_groups[i] == -1) {
                break;
            }
        }
        port_groups = i;
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Flex port groups: %d\n"), port_groups));
        _tr3_flex_port_data[unit].port_groups = port_groups;
        for (i = 0; i < port_groups; i++) {
            config_str = soc_property_port_get_str(unit, i, spn_PORTGROUP);
            if (config_str == NULL) {
                continue;
            }
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "Flex group cfg: %d=%s\n"), i, config_str));
            /*
             * portgroup_<port group>=<num of lanes>
             */
            sub_str = config_str;
            num_lanes = sal_ctoi(sub_str, &sub_str_end);
            if (sub_str == sub_str_end) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Port group %d: Missing number of lanes information \"%s\"\n"),
                                      i, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            if (num_lanes == 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "Port group %d: Missing number of lanes information \"%s\"\n"),
                                      i, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            SOC_PBMP_CLEAR(pbmp_xport_xe);
            SOC_PBMP_CLEAR(pbmp_xport_ge);
            idx = _tr3_flex_port_inf[unit].flex_port_groups[i]*4 + 1;
            switch (num_lanes) {
            case 1:
                _tr3_flex_port_data[unit].ports[i] = 4;
                /* Determine bandwidth */
                port_bw = _tr3_flex_port_inf[unit].def_flex_port_speeds[i][2];
                /* Check for properties */
                pbmp_xport_ge = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_GE,
                                                              pbmp_xport_ge);
                /* Force 10 -> 1 */
                if (SOC_PBMP_MEMBER(pbmp_xport_ge, p2l_mapping[idx])) {
                    if (port_bw == 10) {
                        for (j = 0; _tr3_flex_port_inf[unit].flex_port_speeds[i][j] != -1;j++) {
                            if (_tr3_flex_port_inf[unit].flex_port_speeds[i][j] == 1 &&
                                _tr3_flex_port_inf[unit].flex_port_lanes[i][j] == num_lanes) {
                                port_bw = 1;
                                break;
                            }
                        }
                    }
                }
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Flex group: %d, lanes: %d, ports: %d, speed: %d gbps\n"), i,
                                        num_lanes, _tr3_flex_port_data[unit].ports[i], port_bw));
                if (SOC_IS_HELIX4(unit) && ((_tr3_port_config_id[unit] == 410) ||
                     (_tr3_port_config_id[unit] == 463) ||(_tr3_port_config_id[unit] == 413))) {
                    flex_p2l_mapping[idx+1] = p2l_mapping[idx+1];
                    flex_p2l_mapping[idx+2] = p2l_mapping[idx+2];
                    flex_p2l_mapping[idx+3] = p2l_mapping[idx+3];
                    flex_speed_max[idx+1] = (flex_p2l_mapping[idx+1]>0) ? port_bw : -1;
                    flex_speed_max[idx+2] = (flex_p2l_mapping[idx+2]>0) ? port_bw : -1;
                    flex_speed_max[idx+3] = (flex_p2l_mapping[idx+3]>0) ? port_bw : -1;
                } else{
                    flex_p2l_mapping[idx+1] = flex_p2l_mapping[idx]+1;
                    flex_p2l_mapping[idx+2] = flex_p2l_mapping[idx]+2;
                    flex_p2l_mapping[idx+3] = flex_p2l_mapping[idx]+3;
                    flex_speed_max[idx+1] = port_bw;
                    flex_speed_max[idx+2] = port_bw;
                    flex_speed_max[idx+3] = port_bw;
                }
                flex_speed_max[idx] = port_bw;
                break;
            case 2:
#ifdef BCM_HELIX4_SUPPORT
                 if (SOC_IS_HELIX4(unit) && (((_tr3_port_config_id[unit] == 464)
                    ||(_tr3_port_config_id[unit] == 410)) && (idx == 57 || idx == 61))) {
                    return SOC_E_PARAM;
                 } 
#endif
                _tr3_flex_port_data[unit].ports[i] = 2;
                /* Determine bandwidth */
                port_bw = _tr3_flex_port_inf[unit].def_flex_port_speeds[i][1];
                /* Check for properties */
                pbmp_xport_xe = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_XE,
                                                              pbmp_xport_xe);
                /* Force 21 -> 10 */
                if (SOC_PBMP_MEMBER(pbmp_xport_xe, flex_p2l_mapping[idx])) {
                    if (port_bw == 21) {
                        for (j = 0; _tr3_flex_port_inf[unit].flex_port_speeds[i][j] != -1;j++) {
                            if (_tr3_flex_port_inf[unit].flex_port_speeds[i][j] == 10 &&
                                _tr3_flex_port_inf[unit].flex_port_lanes[i][j] == num_lanes) {
                                port_bw = 10;
                                break;
                            }
                        }
                    }
                }

                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Flex group: %d, lanes: %d, ports: %d, speed: %d gbps\n"), i,
                                        num_lanes, _tr3_flex_port_data[unit].ports[i], port_bw));
                if (SOC_IS_HELIX4(unit) && ((_tr3_port_config_id[unit]== 463) ||
                    (_tr3_port_config_id[unit]== 413))){
                    flex_p2l_mapping[idx+1] = -1;
                    flex_p2l_mapping[idx+2] = p2l_mapping[idx+2];
                    flex_p2l_mapping[idx+3] = -1;
                    flex_speed_max[idx+1] = (flex_p2l_mapping[idx+1]>0) ? port_bw : -1;
                    flex_speed_max[idx+2] = (flex_p2l_mapping[idx+2]>0) ? port_bw : -1;
                    flex_speed_max[idx+3] = (flex_p2l_mapping[idx+3]>0) ? port_bw : -1;
               } else{
                    flex_p2l_mapping[idx+1] = -1;
                    flex_p2l_mapping[idx+2] = flex_p2l_mapping[idx]+2;
                    flex_p2l_mapping[idx+3] = -1;
                    flex_speed_max[idx] = port_bw;
                    flex_speed_max[idx+1] = -1;
                    flex_speed_max[idx+2] = port_bw;
                    flex_speed_max[idx+3] = -1;
               }
                break;
            case 4:
                if ((BCM56548H_DEVICE_ID == dev_id) &&(idx == 57 || idx == 61)) {
                    return SOC_E_PARAM;
                }
                if ( SOC_IS_HELIX4(unit) && ((_tr3_port_config_id[unit]== 463) ||
                     (_tr3_port_config_id[unit]== 413)) && ((idx == 57) || (idx == 61))) {
                    return SOC_E_PARAM;
                }
                if ( SOC_IS_HELIX4(unit) && (_tr3_port_config_id[unit]== 414)) {
                    return SOC_E_PARAM;
                }
                _tr3_flex_port_data[unit].ports[i] = 1;
                /* Determine bandwidth */
                port_bw = _tr3_flex_port_inf[unit].def_flex_port_speeds[i][0];
                /* Check for properties */
                pbmp_xport_xe = soc_property_get_pbmp_default(unit, spn_PBMP_XPORT_XE,
                                                              pbmp_xport_xe);
                /* Force 21 -> 10 */
                if (SOC_PBMP_MEMBER(pbmp_xport_xe, flex_p2l_mapping[idx])) {
                    if (port_bw == 21) {
                        for (j = 0; _tr3_flex_port_inf[unit].flex_port_speeds[i][j] != -1;j++) {
                            if (_tr3_flex_port_inf[unit].flex_port_speeds[i][j] == 10 &&
                                _tr3_flex_port_inf[unit].flex_port_lanes[i][j] == num_lanes) {
                                port_bw = 10;
                                break;
                            }
                        }
                    }
                }
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Flex group: %d, lanes: %d, ports: %d, speed: %d gbps\n"), i,
                                        num_lanes, _tr3_flex_port_data[unit].ports[i], port_bw));
                flex_p2l_mapping[idx+1] = -1;
                flex_p2l_mapping[idx+2] = -1;
                flex_p2l_mapping[idx+3] = -1;
                flex_speed_max[idx] = port_bw;
                flex_speed_max[idx+1] = -1;
                flex_speed_max[idx+2] = -1;
                flex_speed_max[idx+3] = -1;
                break;
            default: break;
            }
        }
    }

    si = &SOC_INFO(unit);
    si->bandwidth = 260000;
    for (phy_port = 0; phy_port < SOC_MAX_PHY_PORTS; phy_port++) {
        si->port_p2l_mapping[phy_port] = p2l_mapping[phy_port];
        si->flex_port_p2l_mapping[phy_port] = _tr3_flex_port_inf[unit].valid ? flex_p2l_mapping[phy_port] :
                                                  p2l_mapping[phy_port]; /* for disabling unavailable ports */
        si->port_p2m_mapping[phy_port] = p2m_mapping[phy_port];
        _soc_tr3_port_speed_max[phy_port] = 0;
        if (_tr3_flex_port_inf[unit].valid) {
            if (flex_speed_max[phy_port] != -1) {
                si->port_speed_max[si->port_p2l_mapping[phy_port]] =
                    flex_speed_max[phy_port] * 1000;
                _soc_tr3_port_speed_max[phy_port] = flex_speed_max[phy_port] * 1000;
            }
        } else {
            if (speed_max[phy_port] != -1) {
                si->port_speed_max[si->port_p2l_mapping[phy_port]] =
                    speed_max[phy_port] * 1000;
                _soc_tr3_port_speed_max[phy_port] = speed_max[phy_port] * 1000;
            }
        }
    }
    return rv;
}

#ifdef BCM_ISM_SUPPORT
/* Default mem config size */
static soc_ism_mem_size_config_t ism_alloc_size[] = {
    { SOC_ISM_MEM_VLAN_XLATE, _ISM_MEM_VLAN_XLATE_SIZE , 0 , 0},
    { SOC_ISM_MEM_L2_ENTRY, _ISM_MEM_L2_SIZE , 0 , 0},
    { SOC_ISM_MEM_L3_ENTRY, _ISM_MEM_L3_SIZE , 0 , 0},
    { SOC_ISM_MEM_EP_VLAN_XLATE, _ISM_MEM_EP_VLAN_XLATE_SIZE , 0 , 0},
    { SOC_ISM_MEM_MPLS, _ISM_MEM_MPLS_SIZE , 0 , 0},
    { SOC_ISM_MEM_ESM_L2, 0 , 0 , 0},
    { SOC_ISM_MEM_ESM_L3, 0 , 0 , 0},
    { SOC_ISM_MEM_ESM_ACL, 0 , 0 , 0}
};
#endif


int
_soc_hx4_l3_defip_sizing_config(int unit)
{
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;   
    int defip_config = 0;
    int num_defip_entries = 0;
    uint32 lpm_scaling_enable = 0;
    soc_persist_t *sop = SOC_PERSIST(unit);

    if (soc_property_get(unit, "l3_defip_sizing", TRUE) == FALSE) {
       return SOC_E_NONE;
    }

    defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

    /* Pair half the tcams by default when ipv6_128b is enabled */
    num_ipv6_128b_entries =
        ((SOC_CONTROL(unit)->l3_defip_max_tcams/2) * SOC_CONTROL(unit)->l3_defip_tcam_size) / 2;

    num_ipv6_128b_entries = soc_property_get(unit,
                                spn_NUM_IPV6_LPM_128B_ENTRIES,
                                (defip_config ? num_ipv6_128b_entries : 0));
    num_ipv6_128b_entries = num_ipv6_128b_entries + (num_ipv6_128b_entries % 2);

    lpm_scaling_enable = soc_property_get(unit, spn_LPM_SCALING_ENABLE, 0);
    if (SOC_CONTROL(unit)->tcam_protect_write) {
       /* not support in non-scaling LPM mode */
       if (lpm_scaling_enable == 0) {
          LOG_CLI((BSL_META_U(unit,
                            "LPM non-scaling mode does not support "
                            "tcam_protect_write. Please retry with "
                            "lpm_scaling_enable=1.\n")));
          return SOC_E_CONFIG;
       }
       num_ipv6_128b_entries = (num_ipv6_128b_entries + 3) / 4 * 4;
    }

    config_v6_entries = num_ipv6_128b_entries;


    if (lpm_scaling_enable) {
        num_ipv6_128b_entries = 0;
    }
    num_defip_entries = (SOC_CONTROL(unit)->l3_defip_max_tcams *
                         SOC_CONTROL(unit)->l3_defip_tcam_size) -
                         (num_ipv6_128b_entries * 2);

    /* Adjust numbers for tcam_protect_write */
    /* Reserve one entry from each TCAM to maintain duplicate entry during modify */
    if (SOC_CONTROL(unit)->tcam_protect_write) {
       if (num_defip_entries) {
           num_defip_entries -= SOC_CONTROL(unit)->l3_defip_max_tcams;
           /* Don't use up all entries as protect entries */
           if (num_defip_entries <= 0) {
              return SOC_E_CONFIG;
           }
           SOC_CONTROL(unit)->l3_defip_tcam_size --;
       }
       if (num_ipv6_128b_entries) {
          num_ipv6_128b_entries -= SOC_CONTROL(unit)->l3_defip_max_tcams/2;
          /* Don't use up all entries as protect entries */
          if (num_ipv6_128b_entries <= 0) {
             return SOC_E_CONFIG;
          }
          SOC_CONTROL(unit)->l3_defip_tcam_size --;
       }
       if (config_v6_entries) {
          config_v6_entries -= SOC_CONTROL(unit)->l3_defip_max_tcams/2;
          if (config_v6_entries < 0) {
             return SOC_E_CONFIG;
          }
       }
    }

    if (lpm_scaling_enable) {
        if (!soc_property_get(unit, spn_LPM_IPV6_128B_RESERVED, 1)) {
            config_v6_entries = ((config_v6_entries /
                        SOC_CONTROL(unit)->l3_defip_tcam_size) +
                    ((config_v6_entries %
                      SOC_CONTROL(unit)->l3_defip_tcam_size)
                     ? 1 : 0)) * SOC_CONTROL(unit)->l3_defip_tcam_size;
        }
    }

    sop->memState[L3_DEFIP_PAIR_128m].index_max =
                                num_ipv6_128b_entries - 1;
    sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max =
                                num_ipv6_128b_entries - 1;
    sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max =
                                num_ipv6_128b_entries - 1;
    sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max =
                                num_ipv6_128b_entries - 1;
    sop->memState[L3_DEFIPm].index_max = num_defip_entries - 1;
    sop->memState[L3_DEFIP_ONLYm].index_max =
                              sop->memState[L3_DEFIPm].index_max;
    sop->memState[L3_DEFIP_DATA_ONLYm].index_max =
                              sop->memState[L3_DEFIPm].index_max;
    sop->memState[L3_DEFIP_HIT_ONLYm].index_max =
                              sop->memState[L3_DEFIPm].index_max;
    SOC_CONTROL(unit)->l3_defip_index_remap = num_ipv6_128b_entries;
    soc_l3_defip_indexes_init(unit, config_v6_entries);

    return SOC_E_NONE;
}

/* soc_hx4_mem_config:
 * Over-ride the default table sizes (from regsfile) for any SKUs here
 */
int
soc_hx4_mem_config(int unit, int dev_id)
{
    int rv = SOC_E_NONE;
    uint16 scale = 5;
    soc_persist_t *sop = SOC_PERSIST(unit);
    uint8 bcm56049_mode = FALSE;

    sop->memState[ING_UNTAGGED_PHBm].index_max = 62;

    /* MMU buffer size config */
    switch (dev_id) {
        case BCM56040_DEVICE_ID:
        case BCM56041_DEVICE_ID:
        case BCM56042_DEVICE_ID:
            _soc_tr3_mmu_buffer_cells[unit] = _HX4_MMU_TOTAL_CELLS_10K;
            break;
        case BCM56548H_DEVICE_ID:
        case BCM56548_DEVICE_ID:
        case BCM56547_DEVICE_ID:
        case BCM56346_DEVICE_ID:
        case BCM56345_DEVICE_ID:
        case BCM56344_DEVICE_ID:
        case BCM56342_DEVICE_ID:
        case BCM56340_DEVICE_ID:
        case BCM56049_DEVICE_ID:
        case BCM56048_DEVICE_ID:
        case BCM56047_DEVICE_ID:
            _soc_tr3_mmu_buffer_cells[unit] = _TR3_MMU_TOTAL_CELLS_19K;
            break;
        default:
            _soc_tr3_mmu_buffer_cells[unit] = _TR3_MMU_TOTAL_CELLS_19K;
            break;
    }

    switch (dev_id) {
        case BCM56548H_DEVICE_ID:
        case BCM56548_DEVICE_ID:
        case BCM56547_DEVICE_ID:
            /* for Firescont,  only 8 tcams are used */
            SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_HX4_DEFIP_MAX_TCAMS/2; 
            SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_HX4_DEFIP_TCAM_DEPTH;
            /* L3_DEFIP sizing */
            if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
                rv = _soc_hx4_l3_defip_sizing_config(unit);
                if (SOC_FAILURE(rv)) {
                   return rv;
                }
            }
        break;
        case BCM56344_DEVICE_ID:
        case BCM56342_DEVICE_ID:
        case BCM56340_DEVICE_ID:
            /* for HX4,  only 8 tcams are used */
            SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_HX4_DEFIP_MAX_TCAMS/4; 
            SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_HX4_DEFIP_TCAM_DEPTH;
            /* L3_DEFIP sizing */
            if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
                rv = _soc_hx4_l3_defip_sizing_config(unit);
                if (SOC_FAILURE(rv)) {
                   return rv;
                }
            }
        break;
        case BCM56346_DEVICE_ID:
        case BCM56345_DEVICE_ID:
            SOC_CONTROL(unit)->l3_defip_max_tcams = _SOC_HX4_DEFIP_MAX_TCAMS/8; 
            SOC_CONTROL(unit)->l3_defip_tcam_size = _SOC_HX4_DEFIP_TCAM_DEPTH;

            /* L3 Next hop */
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 8191;
            sop->memState[ING_L3_NEXT_HOPm].index_max = 8191;
            sop->memState[EGR_L3_NEXT_HOPm].index_max = 8191;
            sop->memState[INITIAL_PROT_NHI_TABLEm].index_max = 8191;
            /* L3 LPM */
            sop->memState[L3_DEFIPm].index_max = 8191;
            sop->memState[L3_DEFIP_ONLYm].index_max = 8191;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 8191;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 8191;
            /* L3 IPMC */
            sop->memState[L3_IPMCm].index_max = 2047;
            sop->memState[L3_IPMC_1m].index_max = 2047;
            sop->memState[L3_IPMC_REMAPm].index_max = 2047;
            sop->memState[MMU_REPL_GROUPm].index_max = 2047;
            /* sop->memState[ING_UNTAGGED_PHBm].index_max = ?? */
            sop->memState[EGR_IPMCm].index_max = 2047;
            sop->memState[EGR_L3_INTFm].index_max = 8191;
            /* Counters */
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_0m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_1m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_2m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_3m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_4m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_5m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_6m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_7m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_8m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_9m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_10m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_11m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_12m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_13m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_14m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_15m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_8m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_9m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_10m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_11m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_12m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_13m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_14m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_15m].index_max = -1;
            sop->memState[OAM_LM_COUNTERSm].index_max = -1;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_0m].index_max = -1;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_1m].index_max = -1;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_2m].index_max = -1;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_3m].index_max = -1;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_4m].index_max = -1;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_5m].index_max = -1;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_6m].index_max = -1;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_7m].index_max = -1;
            /* LMEP */
            sop->memState[LMEPm].index_max = -1;
            sop->memState[LMEP_DAm].index_max = -1;
            /* RMEP */
            sop->memState[RMEPm].index_max = -1;
            sop->memState[MAID_REDUCTIONm].index_max = -1;
            sop->memState[MA_STATEm].index_max = -1;
      
            /* L3_DEFIP sizing */
            if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
                rv = _soc_hx4_l3_defip_sizing_config(unit);
                if (SOC_FAILURE(rv)) {
                   return rv;
                }
            }
            /* These memories are not present in Twister */
            sop->memState[FT_SESSIONm].index_max = -1;
            sop->memState[FT_EXPORT_FIFOm].index_max = -1; 
            sop->memState[FT_EXPORT_DATA_ONLYm].index_max = -1; 
            sop->memState[FT_EOP_TBLm].index_max = -1; 
            sop->memState[FT_AGE_PROFILEm].index_max = -1; 
            sop->memState[FT_L4PORTm].index_max = -1; 
            sop->memState[FT_DST_LAG_CONFIGm].index_max = -1; 
            sop->memState[FT_DST_PORT_CONFIGm].index_max = -1; 
            sop->memState[FT_POLICYm].index_max = -1; 
            sop->memState[FT_SESSION_IPV6m].index_max = -1; 
            sop->memState[FT_EXPORT_CNTR_ONLYm].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP0m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP1m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP2m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP3m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP4m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP5m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP6m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP7m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP8m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP9m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP10m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP11m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP12m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP13m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP14m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP15m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP16m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP17m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP18m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP19m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP20m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP21m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP22m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP23m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP24m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP25m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP26m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP27m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP28m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP29m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP30m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP31m].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_MATCH_DATAm].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_MATCH_MASKm].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_REPLACE_DATAm].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_REPLACE_MASKm].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_MEMm].index_max = -1;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM0m].index_max = -1;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM1m].index_max = -1;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM2m].index_max = -1;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM3m].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_MATCH_DATAm].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_MATCH_MASKm].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_REPLACE_DATAm].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_REPLACE_MASKm].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_MEM0m].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_MEM1m].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_MEM2m].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_MEM3m].index_max = -1;
            sop->memState[AXP_SM_PACKET_BUFFER_MEMm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_BULK_MATCH_DATAm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_BULK_MATCH_MASKm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_BULK_REPLACE_DATAm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_BULK_REPLACE_MASKm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM0m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM1m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM2m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM3m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM4m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM5m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM6m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM7m].index_max = -1;
            sop->memState[SVM_METER_TABLEm].index_max = -1;
            sop->memState[VRFm].index_max = 127;
        break;
        case BCM56042_DEVICE_ID:
        case BCM56041_DEVICE_ID:
        case BCM56040_DEVICE_ID:
            /* L3 Next hop */
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 1023;
            sop->memState[ING_L3_NEXT_HOPm].index_max = 1023;
            sop->memState[EGR_L3_NEXT_HOPm].index_max = 1023;
            sop->memState[INITIAL_PROT_NHI_TABLEm].index_max = 1023;
            /* L3 LPM */
            sop->memState[L3_DEFIPm].index_max = 127;
            sop->memState[L3_DEFIP_ONLYm].index_max = 127;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 127;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 127;
            /* L3 IPMC */
            sop->memState[L3_IPMCm].index_max = 255;
            sop->memState[L3_IPMC_1m].index_max = 255;
            sop->memState[L3_IPMC_REMAPm].index_max = 255;
            sop->memState[MMU_REPL_GROUPm].index_max = 255;
            /* sop->memState[ING_UNTAGGED_PHBm].index_max = ?? */
            sop->memState[EGR_IPMCm].index_max = 255;
            sop->memState[EGR_L3_INTFm].index_max = 511;
            /* Counters */
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_0m].index_max = 255;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_1m].index_max = 255;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_2m].index_max = 255;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_3m].index_max = 255;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_4m].index_max = 255;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_5m].index_max = 255;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_6m].index_max = 255;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_7m].index_max = 255;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_8m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_9m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_10m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_11m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_12m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_13m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_14m].index_max = -1;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_15m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_8m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_9m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_10m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_11m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_12m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_13m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_14m].index_max = -1;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_15m].index_max = -1;
            sop->memState[VLAN_OR_VFI_MAC_COUNTm].index_max = 8191;
            sop->memState[VLAN_OR_VFI_MAC_LIMITm].index_max = 8191;
            sop->memState[OAM_LM_COUNTERSm].index_max = 8191;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_0m].index_max = 63;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_1m].index_max = 63;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_2m].index_max = 63;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_3m].index_max = 63;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_4m].index_max = 63;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_5m].index_max = 63;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_6m].index_max = 63;
            sop->memState[EGR_FLEX_CTR_COUNTER_TABLE_7m].index_max = 63;
            /* LMEP */
            sop->memState[LMEPm].index_max = -1;
            sop->memState[LMEP_DAm].index_max = -1;
            /* RMEP */
            sop->memState[RMEPm].index_max = -1;
            sop->memState[MAID_REDUCTIONm].index_max = -1;
            sop->memState[MA_STATEm].index_max = -1;
            /* L3_DEFIP sizing */
            if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
                if (soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1)) {
                    sop->memState[L3_DEFIP_PAIR_128m].index_max = 127;
                    sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = 127;
                    sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = 127;
                    sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = 127;
                    sop->memState[L3_DEFIPm].index_max = 127;
                    sop->memState[L3_DEFIP_ONLYm].index_max = 127;
                    sop->memState[L3_DEFIP_DATA_ONLYm].index_max = 127;
                    sop->memState[L3_DEFIP_HIT_ONLYm].index_max = 127;
                    SOC_CONTROL(unit)->l3_defip_index_remap = 127;
                } else {
                    sop->memState[L3_DEFIP_PAIR_128m].index_max = -1;
                    sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = -1;
                    sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = -1;
                    sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = -1;
                }
            }
            /* These memories are not present in Ranger */
            sop->memState[AXP_SM_CHAR_REMAP0m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP1m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP2m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP3m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP4m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP5m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP6m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP7m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP8m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP9m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP10m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP11m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP12m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP13m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP14m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP15m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP16m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP17m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP18m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP19m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP20m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP21m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP22m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP23m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP24m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP25m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP26m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP27m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP28m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP29m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP30m].index_max = -1;
            sop->memState[AXP_SM_CHAR_REMAP31m].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_MATCH_DATAm].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_MATCH_MASKm].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_REPLACE_DATAm].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_REPLACE_MASKm].index_max = -1;
            sop->memState[AXP_SM_FLOW_TABLE_MEMm].index_max = -1;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM0m].index_max = -1;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM1m].index_max = -1;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM2m].index_max = -1;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM3m].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_MATCH_DATAm].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_MATCH_MASKm].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_REPLACE_DATAm].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_REPLACE_MASKm].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_MEM0m].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_MEM1m].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_MEM2m].index_max = -1;
            sop->memState[AXP_SM_MATCH_TABLE_MEM3m].index_max = -1;
            sop->memState[AXP_SM_PACKET_BUFFER_MEMm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_BULK_MATCH_DATAm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_BULK_MATCH_MASKm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_BULK_REPLACE_DATAm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_BULK_REPLACE_MASKm].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM0m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM1m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM2m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM3m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM4m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM5m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM6m].index_max = -1;
            sop->memState[AXP_SM_STATE_TABLE_MEM7m].index_max = -1;
            sop->memState[AXP_WLAN_COS_MAPm].index_max = -1;
            sop->memState[AXP_WRX_SVP_ASSIGNMENTm].index_max = -1;
            sop->memState[AXP_WRX_WCDm].index_max = -1;
            sop->memState[AXP_WRX_SVP_ASSIGNMENTm].index_max = -1;
            sop->memState[AXP_WRX_WCDm].index_max = -1;
            sop->memState[AXP_WTX_DSCP_MAPm].index_max = -1;
            sop->memState[AXP_WTX_DVP_PROFILEm].index_max = -1;
            sop->memState[AXP_WTX_FRAG_IDm].index_max = -1;
            sop->memState[AXP_WTX_PRI_MAPm].index_max = -1;
            sop->memState[AXP_WTX_TRUNK_BLOCK_MASKm].index_max = -1;
            sop->memState[AXP_WTX_TRUNK_GROUP_BITMAPm].index_max = -1;
            sop->memState[AXP_WTX_TUNNELm].index_max = -1;
            sop->memState[SVM_METER_TABLEm].index_max = -1;
            sop->memState[VRFm].index_max = -1;
            break;

        /* Spiral & SKUs */
        case BCM56049_DEVICE_ID:
        case BCM56048_DEVICE_ID:
        case BCM56047_DEVICE_ID:
            bcm56049_mode = TRUE;

            /* VLAN MAC */
            sop->memState[VLAN_MACm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[VLAN_MAC_OVERFLOWm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[VLAN_MAC_SCRATCHm].index_max = _MEM_NOT_APPLICABLE;
            
            sop->memState[VLAN_XLATEm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[EGR_VLAN_XLATEm].index_max = _MEM_NOT_APPLICABLE;

            sop->memState[L3_DEFIP_PAIR_128m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[L3_DEFIP_PAIR_128_ONLYm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[L3_DEFIP_PAIR_128_DATA_ONLYm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[L3_DEFIP_PAIR_128_HIT_ONLYm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[L3_DEFIPm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[L3_DEFIP_ONLYm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[L3_DEFIP_DATA_ONLYm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[L3_DEFIP_HIT_ONLYm].index_max = _MEM_NOT_APPLICABLE;

/*            sop->memState[RPFMEMORYm].index_max = _MEM_NOT_APPLICABLE;*/
        
            /* L3 Next hop */
            sop->memState[INITIAL_ING_L3_NEXT_HOPm].index_max = 1023;
            sop->memState[ING_L3_NEXT_HOPm].index_max = 1023;
            sop->memState[EGR_L3_NEXT_HOPm].index_max = 1023;
            sop->memState[INITIAL_PROT_NHI_TABLEm].index_max = 1023;            

            sop->memState[EGR_L3_INTFm].index_max = _MEM_NOT_APPLICABLE;
    
            /* L3 IPMC */
            sop->memState[L3_IPMCm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[L3_IPMC_1m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[L3_IPMC_REMAPm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[MMU_REPL_GROUPm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[IP_MULTICAST_TCAMm].index_max = _MEM_NOT_APPLICABLE;           

            /* VRF */
            /* sop->memState[VRFm].index_max = _MEM_NOT_APPLICABLE; */

			/* AXP */
            sop->memState[AXP_SM_CHAR_REMAP0m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP1m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP2m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP3m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP4m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP5m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP6m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP7m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP8m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP9m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP10m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP11m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP12m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP13m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP14m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP15m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP16m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP17m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP18m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP19m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP20m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP21m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP22m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP23m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP24m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP25m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP26m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP27m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP28m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP29m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP30m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_CHAR_REMAP31m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_MATCH_DATAm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_MATCH_MASKm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_REPLACE_DATAm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_FLOW_TABLE_BULK_REPLACE_MASKm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_FLOW_TABLE_MEMm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM0m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM1m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM2m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_COUNTERS_MEM3m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_MATCH_DATAm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_MATCH_MASKm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_REPLACE_DATAm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_TABLE_BULK_REPLACE_MASKm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_TABLE_MEM0m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_TABLE_MEM1m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_TABLE_MEM2m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_MATCH_TABLE_MEM3m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_PACKET_BUFFER_MEMm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_BULK_MATCH_DATAm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_BULK_MATCH_MASKm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_BULK_REPLACE_DATAm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_BULK_REPLACE_MASKm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_MEM0m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_MEM1m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_MEM2m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_MEM3m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_MEM4m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_MEM5m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_MEM6m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_SM_STATE_TABLE_MEM7m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WLAN_COS_MAPm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WRX_SVP_ASSIGNMENTm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WRX_WCDm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WRX_SVP_ASSIGNMENTm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WRX_WCDm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WTX_DSCP_MAPm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WTX_DVP_PROFILEm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WTX_FRAG_IDm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WTX_PRI_MAPm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WTX_TRUNK_BLOCK_MASKm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WTX_TRUNK_GROUP_BITMAPm].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[AXP_WTX_TUNNELm].index_max = _MEM_NOT_APPLICABLE;

            /* Counters */
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_0m].index_max = 1023;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_1m].index_max = 1023;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_2m].index_max = 1023;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_3m].index_max = 1023;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_4m].index_max = 1023;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_5m].index_max = 1023;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_6m].index_max = 1023;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_7m].index_max = 1023;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_8m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_9m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_10m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_11m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_12m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_13m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_14m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_COUNTER_TABLE_15m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_8m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_9m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_10m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_11m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_12m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_13m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_14m].index_max = _MEM_NOT_APPLICABLE;
            sop->memState[ING_FLEX_CTR_OFFSET_TABLE_15m].index_max = _MEM_NOT_APPLICABLE;
    
        break;
			
        default:
            break;
    }

#ifdef BCM_ISM_SUPPORT
    switch (dev_id) {
        case BCM56042_DEVICE_ID:
        case BCM56041_DEVICE_ID:
        case BCM56040_DEVICE_ID:
            scale = 6;
            break;
        default:
            scale = 5;
            break;
    }


    if (soc_feature(unit, soc_feature_ism_memory)) {
        soc_ism_mem_size_config_t ism_alloc_size_int[SOC_ISM_MEM_TOTAL];
        sal_memcpy(ism_alloc_size_int, ism_alloc_size, sizeof(ism_alloc_size_int));
        ism_alloc_size_int[0].size = bcm56049_mode ? 0 :
            soc_property_get(unit, spn_VLAN_XLATE_MEM_ENTRIES,
                             (ism_alloc_size_int[0].size*5)/scale);
        ism_alloc_size_int[1].size =
            soc_property_get(unit, spn_L2_MEM_ENTRIES,
                             (ism_alloc_size_int[1].size*5)/scale);
        ism_alloc_size_int[2].size = bcm56049_mode ? 0 :
            soc_property_get(unit, spn_L3_MEM_ENTRIES,
                             (ism_alloc_size_int[2].size*5)/scale);
        ism_alloc_size_int[3].size = bcm56049_mode ? 0 :
            soc_property_get(unit, spn_EGR_VLAN_XLATE_MEM_ENTRIES,
                             (ism_alloc_size_int[3].size*5)/scale);
        ism_alloc_size_int[4].size = bcm56049_mode ? 0 :
            soc_property_get(unit, spn_MPLS_MEM_ENTRIES,
                    (ism_alloc_size_int[4].size*5)/scale);
        if (ism_alloc_size_int[1].size <= 0 && ism_alloc_size_int[5].size <= 0) {
            LOG_CLI((BSL_META_U(unit,
                                "No L2 memory configured !!\n")));
            return SOC_E_RESOURCE;
        }
        ism_alloc_size_int[0].banks =
            soc_property_get(unit, spn_VLAN_XLATE_MEM_BANKS,
                             ism_alloc_size_int[0].banks);
        ism_alloc_size_int[1].banks =
            soc_property_get(unit, spn_L2_MEM_BANKS,
                             ism_alloc_size_int[1].banks);
        ism_alloc_size_int[2].banks =
            soc_property_get(unit, spn_L3_MEM_BANKS,
                             ism_alloc_size_int[2].banks);
        ism_alloc_size_int[3].banks =
            soc_property_get(unit, spn_EGR_VLAN_XLATE_MEM_BANKS,
                             ism_alloc_size_int[3].banks);
        ism_alloc_size_int[4].banks =
            soc_property_get(unit, spn_MPLS_MEM_BANKS,
                             ism_alloc_size_int[4].banks);
        rv = soc_ism_mem_config(unit, ism_alloc_size_int, COUNTOF(ism_alloc_size_int));
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "Invalid or unsupported ISM memory config !!\n")));
            return rv;
        }
    }
#endif
    return rv;
}

/* _soc_hx4_mmu_tdm_array_init
 * This enables reuse of TR3 code for MMU/LLS init through _soc_tr3_mmu_tdm_init
 * _soc_hx4_mmu_tdm_array_init copies requested tdm config into _soc_tr3_tdm_arr
 * and sets the _soc_tr3_tdm_size (both globals shared across TR3/HX4 code).
 */
void
_soc_hx4_mmu_tdm_array_init(int unit)
{
   static uint32 hx4_tdm_0[] = { /* HX4_1G_150_A */
        61, 57,  1, 13, 63, 53, 25, 61, 58, 37,  5, 63, 54, 17, 29, 61, 57, 41, 63, 55,  9, 81, 61, 58, 33, 63, 56, 81,  0,
        61, 57,  2, 21, 63, 53, 26, 61, 58, 45,  6, 63, 54, 14, 30, 61, 57, 38, 63, 55, 18, 81, 61, 58, 42, 63, 56, 81,  0,
        61, 57, 10, 22, 63, 53, 34, 61, 58, 46,  3, 63, 54, 15, 27, 61, 57, 39, 63, 55,  7, 81, 61, 58, 31, 63, 56, 81, -1,
        61, 57, 11, 19, 63, 53, 35, 61, 58, 43,  4, 63, 54, 23, 28, 61, 57, 47, 63, 55, 16, 81, 61, 58, 40, 63, 56, 81,  0,
        61, 57,  8, 20, 63, 53, 32, 61, 58, 44, 12, 63, 54, 24, 36, 61, 57, 48, 63, 55, 49, 81, 61, 58, -1, 63, 56, 81,  0
    };
    static uint32 hx4_tdm_1[] = { /* HX4_1G_150_B */
        53, 57,  1, 13, 54, 61, 25, 55, 57, 37,  5, 56, 61, 17, 29, 53, 57, 41, 54, 61,  9, 81, 55, 57, 33, 56, 61, 81,  0,
        53, 57,  2, 21, 54, 61, 26, 55, 57, 45,  6, 56, 61, 14, 30, 53, 57, 38, 54, 61, 18, 81, 55, 57, 42, 56, 61, 81,  0,
        53, 57, 10, 22, 54, 61, 34, 55, 57, 46,  3, 56, 61, 15, 27, 53, 57, 39, 54, 61,  7, 81, 55, 57, 31, 56, 61, 81, -1,
        53, 57, 11, 19, 54, 61, 35, 55, 57, 43,  4, 56, 61, 23, 28, 53, 57, 47, 54, 61, 16, 81, 55, 57, 40, 56, 61, 81,  0,
        53, 57,  8, 20, 54, 61, 32, 55, 57, 44, 12, 56, 61, 24, 36, 53, 57, 48, 54, 61, 49, 81, 55, 57, -1, 56, 61, 81,  0
    };
    static uint32 hx4_tdm_2[] = { /* RGR_XFI_150 */
        53, 57, 61, 81, 54, 58, 62, 55, 59, 63,  1, 56, 60, 64, 81, 53, 57, 61, 54, 58, 62, 0, 55, 59, 63, 56, 60, 64, -1,
        53, 57, 61, 81, 54, 58, 62, 55, 59, 63,  2, 56, 60, 64, 81, 53, 57, 61, 54, 58, 62, 0, 55, 59, 63, 56, 60, 64, -1,
        53, 57, 61, 81, 54, 58, 62, 55, 59, 63,  3, 56, 60, 64, 81, 53, 57, 61, 54, 58, 62, 0, 55, 59, 63, 56, 60, 64, -1,
        53, 57, 61, 81, 54, 58, 62, 55, 59, 63,  4, 56, 60, 64, 81, 53, 57, 61, 54, 58, 62, 0, 55, 59, 63, 56, 60, 64, -1,
        53, 57, 61, 81, 54, 58, 62, 55, 59, 63, 49, 56, 60, 64, 81, 53, 57, 61, 54, 58, 62, 0, 55, 59, 63, 56, 60, 64, -1
    };
    static uint32 hx4_tdm_3[] = { /* HX4_1G_200_A */
        53, 57, 61,  1, 13, 54, 57, 61, 25, 37, 55, 57, 61,  5, 17, 56, 57, 61, 29, 41, 53, 57, 61, 81, 54, 57, 61,  9, 55, 57, 61, 33,  0, 56, 57, 61, 81,
        53, 57, 61,  2, 21, 54, 57, 61, 26, 45, 55, 57, 61,  6, 14, 56, 57, 61, 30, 38, 53, 57, 61, 81, 54, 57, 61, 18, 55, 57, 61, 42,  0, 56, 57, 61, 81,
        53, 57, 61, 10, 22, 54, 57, 61, 34, 46, 55, 57, 61,  3, 15, 56, 57, 61, 27, 39, 53, 57, 61, 81, 54, 57, 61,  7, 55, 57, 61, 31, -1, 56, 57, 61, 81,
        53, 57, 61, 11, 19, 54, 57, 61, 35, 43, 55, 57, 61,  4, 23, 56, 57, 61, 28, 47, 53, 57, 61, 81, 54, 57, 61, 16, 55, 57, 61, 40,  0, 56, 57, 61, 81,
        53, 57, 61,  8, 20, 54, 57, 61, 32, 44, 55, 57, 61, 12, 24, 56, 57, 61, 36, 48, 53, 57, 61, 81, 54, 57, 61, 49, 55, 57, 61, -1,  0, 56, 57, 61, 81
    };
    static uint32 hx4_tdm_4[] = { /* HX4_1G_200_B */
        53, 57, 61,  1, 13, 53, 57, 61, 25, 37, 53, 57, 61,  5, 17, 53, 57, 61, 29, 41, 53, 57, 61, 81, 53, 57, 61,  9, 53, 57, 61, 33,  0, 53, 57, 61, 81,
        53, 57, 61,  2, 21, 53, 57, 61, 26, 45, 53, 57, 61,  6, 14, 53, 57, 61, 30, 38, 53, 57, 61, 81, 53, 57, 61, 18, 53, 57, 61, 42,  0, 53, 57, 61, 81,
        53, 57, 61, 10, 22, 53, 57, 61, 34, 46, 53, 57, 61,  3, 15, 53, 57, 61, 27, 39, 53, 57, 61, 81, 53, 57, 61,  7, 53, 57, 61, 31, -1, 53, 57, 61, 81,
        53, 57, 61, 11, 19, 53, 57, 61, 35, 43, 53, 57, 61,  4, 23, 53, 57, 61, 28, 47, 53, 57, 61, 81, 53, 57, 61, 16, 53, 57, 61, 40,  0, 53, 57, 61, 81,
        53, 57, 61,  8, 20, 53, 57, 61, 32, 44, 53, 57, 61, 12, 24, 53, 57, 61, 36, 48, 53, 57, 61, 81, 53, 57, 61, 49, 53, 57, 61, -1,  0, 53, 57, 61, 81
    };
    static uint32 hx4_tdm_5[] = { /* RGR_XFI_200 */
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,  1, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,  2, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,  3, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,  4, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1, 49, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1
    };
    static uint32 hx4_tdm_6[] = { /* HX4_1G_130 */
        53, 57,  1, 13, 54, 61, 25, 55, 57,  5, 56, 61, 17, 53, 57, 81, 54, 61,  9, 55, 57, -1, 56, 61,  0,
        53, 57,  2, 21, 54, 61, 26, 55, 57,  6, 56, 61, 14, 53, 57, 81, 54, 61, 18, 55, 57, 81, 56, 61,  0,
        53, 57, 10, 22, 54, 61, 27, 55, 57,  3, 56, 61, 15, 53, 57, 81, 54, 61,  7, 55, 57, 81, 56, 61, -1,
        53, 57, 11, 19, 54, 61, 28, 55, 57,  4, 56, 61, 23, 53, 57, 81, 54, 61, 16, 55, 57, 81, 56, 61,  0,
        53, 57,  8, 20, 54, 61, -1, 55, 57, 12, 56, 61, 24, 53, 57, 81, 54, 61, 49, 55, 57, 81, 56, 61,  0
    };
    static uint32 hx4_tdm_7[] = { /* HX4_1G_150_C */
        53, 57,  1, 13, 54, 61, 25, 55, 57, 37,  5, 56, 61, 17, 29, 53, 57, 41, 54, 61,  9, 81, 55, 57, 33, 56, 61, 81,  0,
        53, 57,  1, 21, 54, 61, 25, 55, 57, 45,  5, 56, 61, 13, 29, 53, 57, 37, 54, 61, 17, 81, 55, 57, 41, 56, 61, 81,  0,
        53, 57,  9, 21, 54, 61, 33, 55, 57, 45,  1, 56, 61, 13, 25, 53, 57, 37, 54, 61,  5, 81, 55, 57, 29, 56, 61, 81, -1,
        53, 57,  9, 17, 54, 61, 33, 55, 57, 41,  1, 56, 61, 21, 25, 53, 57, 45, 54, 61, 13, 81, 55, 57, 37, 56, 61, 81,  0,
        53, 57,  5, 17, 54, 61, 29, 55, 57, 41,  9, 56, 61, 21, 33, 53, 57, 45, 54, 61, 49, 81, 55, 57, -1, 56, 61, 81,  0
    };
    static uint32 hx4_tdm_8[] = { /* HX4_1G_200_C */
        53, 57, 61,  1, 13, 53, 57, 61, 25, 37, 53, 57, 61,  5, 17, 53, 57, 61, 29, 41, 53, 57, 61, 81, 53, 57, 61,  9, 53, 57, 61, 33,  0, 53, 57, 61, 81,
        53, 57, 61,  1, 21, 53, 57, 61, 25, 45, 53, 57, 61,  5, 13, 53, 57, 61, 29, 37, 53, 57, 61, 81, 53, 57, 61, 17, 53, 57, 61, 41,  0, 53, 57, 61, 81,
        53, 57, 61,  9, 21, 53, 57, 61, 33, 45, 53, 57, 61,  1, 13, 53, 57, 61, 25, 37, 53, 57, 61, 81, 53, 57, 61,  5, 53, 57, 61, 29, -1, 53, 57, 61, 81,
        53, 57, 61,  9, 17, 53, 57, 61, 33, 41, 53, 57, 61,  1, 21, 53, 57, 61, 25, 45, 53, 57, 61, 81, 53, 57, 61, 13, 53, 57, 61, 37,  0, 53, 57, 61, 81,
        53, 57, 61,  5, 17, 53, 57, 61, 29, 41, 53, 57, 61,  9, 21, 53, 57, 61, 33, 45, 53, 57, 61, 81, 53, 57, 61, 49, 53, 57, 61, -1,  0, 53, 57, 61, 81
    };
    static uint32 hx4_tdm_9[] = { /* HX4_1G_200_D */
        53, 57, 61,  1, 13, 54, 58, 62, 25, -1, 55, 59, 63,  5, 17, 56, 60, 64, 29, 37, 53, 57, 61, 81, 54, 58, 62,  9, 55, 59, 63, 33,  0, 56, 60, 64, 81,
        53, 57, 61,  2, 21, 54, 58, 62, 26, -1, 55, 59, 63,  6, 14, 56, 60, 64, 30, 38, 53, 57, 61, 81, 54, 58, 62, 18, 55, 59, 63, -1,  0, 56, 60, 64, 81,
        53, 57, 61, 10, 22, 54, 58, 62, 34, -1, 55, 59, 63,  3, 15, 56, 60, 64, 27, 39, 53, 57, 61, 81, 54, 58, 62,  7, 55, 59, 63, 31, -1, 56, 60, 64, 81,
        53, 57, 61, 11, 19, 54, 58, 62, 35, -1, 55, 59, 63,  4, 23, 56, 60, 64, 28, 40, 53, 57, 61, 81, 54, 58, 62, 16, 55, 59, 63, -1,  0, 56, 60, 64, 81,
        53, 57, 61,  8, 20, 54, 58, 62, 32, -1, 55, 59, 63, 12, 24, 56, 60, 64, 36, -1, 53, 57, 61, 81, 54, 58, 62, 49, 55, 59, 63, -1,  0, 56, 60, 64, 81
    };
    static uint32 hx4_tdm_10[] = { /* HX4_1G_200_E */
        53, 57, -1,  1, 13, 55, 57, -1, 25, 37, 53, 57, -1,  5, 17, 55, 57, -1, 29, 41, 53, 57, -1, 81, 55, 57, -1,  9, 53, 57, -1, 33,  0, 55, 57, -1, 81,
        53, 57, -1,  2, 21, 55, 57, -1, 25, 45, 53, 57, -1,  6, 14, 55, 57, -1, 29, 37, 53, 57, -1, 81, 55, 57, -1, 18, 53, 57, -1, 41,  0, 55, 57, -1, 81,
        53, 57, -1, 10, 22, 55, 57, -1, 33, 45, 53, 57, -1,  3, 15, 55, 57, -1, 25, 37, 53, 57, -1, 81, 55, 57, -1,  7, 53, 57, -1, 29, -1, 55, 57, -1, 81,
        53, 57, -1, 11, 19, 55, 57, -1, 33, 41, 53, 57, -1,  4, 23, 55, 57, -1, 25, 45, 53, 57, -1, 81, 55, 57, -1, 16, 53, 57, -1, 37,  0, 55, 57, -1, 81,
        53, 57, -1,  8, 20, 55, 57, -1, 29, 41, 53, 57, -1, 12, 24, 55, 57, -1, 33, 45, 53, 57, -1, 81, 55, 57, -1, 49, 53, 57, -1, -1,  0, 55, 57, -1, 81
    };
    static uint32 hx4_tdm_11[] = { /* RGR_2P5G_150 */
        53, 57,  1, 13, -1, 61, 25, 54, 58, 37,  5, 81, 62, 17, 29, 55, 59, 41, -1, 63, -1, -1, 56, 60, -1, -1, 64, 81,  0,
        53, 57,  9, 21, -1, 61, 33, 54, 58, 45,  1, 81, 62, 13, 25, 55, 59, 37, -1, 63, -1, -1, 56, 60, -1, -1, 64, 81,  0,
        53, 57,  5, 17, -1, 61, 29, 54, 58, 41,  9, 81, 62, 21, 33, 55, 59, 45, -1, 63, -1, -1, 56, 60, -1, -1, 64, 81, -1,
        53, 57,  1, 13, -1, 61, 25, 54, 58, 37,  5, 81, 62, 17, 29, 55, 59, 41, -1, 63, -1, -1, 56, 60, -1, -1, 64, 81,  0,
        53, 57,  9, 21, -1, 61, 33, 54, 58, 45, 49, 81, 62, 17, -1, 55, 59, -1, -1, 63, -1, -1, 56, 60, -1, -1, 64, 81,  0
    };
    static uint32 hx4_tdm_492[] = { /* Subset of HX4_1G_200_D - 56548H FireScout*/
        53, 57, 61,  1, 13, 53, 57, 61, -1, -1, 53, 57, 61,  5, 17, 53, 57, 61, -1, -1, 53, 57, 61, 81, 53, 57, 61,  9, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61,  2, -1, 53, 57, 61, -1, -1, 53, 57, 61,  6, 14, 53, 57, 61, -1, -1, 53, 57, 61, 81, 53, 57, 61, 18, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61, 10, -1, 53, 57, 61, -1, -1, 53, 57, 61,  3, 15, 53, 57, 61, -1, -1, 53, 57, 61, 81, 53, 57, 61,  7, 53, 57, 61, -1, -1, 53, 57, 61, 81,
        53, 57, 61, 11, 19, 53, 57, 61, -1, -1, 53, 57, 61,  4, -1, 53, 57, 61, -1, -1, 53, 57, 61, 81, 53, 57, 61, 16, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61,  8, 20, 53, 57, 61, -1, -1, 53, 57, 61, 12, -1, 53, 57, 61, -1, -1, 53, 57, 61, 81, 53, 57, 61, 49, 53, 57, 61, -1,  0, 53, 57, 61, 81
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_492 = {
        1,
        {  0,  0,  0,  0,  0, 37, 37, 37 },
        {  3, 13, 27,  4, 14,  0,  1,  2 },
        {  0,  0,  0,  0,  0,  5,  5,  5 }
    };
    static uint32 hx4_tdm_491[] = { /* Subset of HX4_1G_200_D - 56548 FireScout*/
        53, 57, 61,  1, 13, 53, 57, 61, 25, -1, 53, 57, 61,  5, 17, 53, 57, 61, -1, -1, 53, 57, 61, 81, 53, 57, 61,  9, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61,  2, 21, 53, 57, 61, 26, -1, 53, 57, 61,  6, 14, 53, 57, 61, -1, -1, 53, 57, 61, 81, 53, 57, 61, 18, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61, 10, 22, 53, 57, 61, -1, -1, 53, 57, 61,  3, 15, 53, 57, 61, 27, -1, 53, 57, 61, 81, 53, 57, 61,  7, 53, 57, 61, -1, -1, 53, 57, 61, 81,
        53, 57, 61, 11, 19, 53, 57, 61, -1, -1, 53, 57, 61,  4, 23, 53, 57, 61, 28, -1, 53, 57, 61, 81, 53, 57, 61, 16, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61,  8, 20, 53, 57, 61, -1, -1, 53, 57, 61, 12, 24, 53, 57, 61, -1, -1, 53, 57, 61, 81, 53, 57, 61, 49, 53, 57, 61, -1,  0, 53, 57, 61, 81
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_491 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0, 37, 37, 37 },
        {  3, 13, 27,  4, 14, 41,  8,  0,  1,  2 },
        {  0,  0,  0,  0,  0,  0,  0,  5,  5,  5 }
    };
    static uint32 hx4_tdm_490[] = { /* HX4_1G_200_D - 56547 FireScout*/
        53, 57, 61,  1, 13, 53, 57, 61, 25, -1, 53, 57, 61,  5, 17, 53, 57, 61, 29, 37, 53, 57, 61, 81, 53, 57, 61,  9, 53, 57, 61, 33,  0, 53, 57, 61, 81,
        53, 57, 61,  2, 21, 53, 57, 61, 26, -1, 53, 57, 61,  6, 14, 53, 57, 61, 30, 38, 53, 57, 61, 81, 53, 57, 61, 18, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61, 10, 22, 53, 57, 61, 34, -1, 53, 57, 61,  3, 15, 53, 57, 61, 27, 39, 53, 57, 61, 81, 53, 57, 61,  7, 53, 57, 61, 31, -1, 53, 57, 61, 81,
        53, 57, 61, 11, 19, 53, 57, 61, 35, -1, 53, 57, 61,  4, 23, 53, 57, 61, 28, 40, 53, 57, 61, 81, 53, 57, 61, 16, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61,  8, 20, 53, 57, 61, 32, -1, 53, 57, 61, 12, 24, 53, 57, 61, 36, -1, 53, 57, 61, 81, 53, 57, 61, 49, 53, 57, 61, -1,  0, 53, 57, 61, 81
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_490 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 37, 37, 37 },
        {  3, 13, 27,  4, 14, 41,  8, 18, 31, 19,  0,  1,  2 },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  5,  5 }
    };
    static uint32 hx4_tdm_495[] = { /* 12xF.QSGMII + F.HG[42] + 2xHG[42] + 1GE */
        53, 57, 61,  1, 13, 53, 57, 61, 25, 37, 53, 57, 61,  5, 17, 53, 57, 61, 29, 41, 53, 57, 61, 81, 53, 57, 61,  9, 53, 57, 61, 33,  0, 53, 57, 61, 81,
        53, 57, 61,  2, 21, 53, 57, 61, 26, 45, 53, 57, 61,  6, 14, 53, 57, 61, 30, 38, 53, 57, 61, 81, 53, 57, 61, 18, 53, 57, 61, 42,  0, 53, 57, 61, 81,
        53, 57, 61, 10, 22, 53, 57, 61, 34, 46, 53, 57, 61,  3, 15, 53, 57, 61, 27, 39, 53, 57, 61, 81, 53, 57, 61,  7, 53, 57, 61, 31, -1, 53, 57, 61, 81,
        53, 57, 61, 11, 19, 53, 57, 61, 35, 43, 53, 57, 61,  4, 23, 53, 57, 61, 28, 47, 53, 57, 61, 81, 53, 57, 61, 16, 53, 57, 61, 40,  0, 53, 57, 61, 81,
        53, 57, 61,  8, 20, 53, 57, 61, 32, 44, 53, 57, 61, 12, 24, 53, 57, 61, 36, 48, 53, 57, 61, 81, 53, 57, 61, 49, 53, 57, 61, -1,  0, 53, 57, 61, 81
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_495 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 37 },
        {  3, 13, 27,  4, 14, 41,  8, 18, 31,  9, 19, 46,  0 },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5 }
    };
    static uint32 hx4_tdm_500[] = { /* HX4_1G_200_E */
        53, 57, -1,  1, 13, 53, 57, -1, 25, 37, 53, 57, -1,  5, 17, 53, 57, -1, 29, 41, 53, 57, -1, 81, 53, 57, -1,  9, 53, 57, -1, 33,  0, 53, 57, -1, 81,
        53, 57, -1,  2, 21, 53, 57, -1, 26, 45, 53, 57, -1,  6, 14, 53, 57, -1, 30, 38, 53, 57, -1, 81, 53, 57, -1, 18, 53, 57, -1, 42,  0, 53, 57, -1, 81,
        53, 57, -1, 10, 22, 53, 57, -1, 34, 46, 53, 57, -1,  3, 15, 53, 57, -1, 27, 39, 53, 57, -1, 81, 53, 57, -1,  7, 53, 57, -1, 31, -1, 53, 57, -1, 81,
        53, 57, -1, 11, 19, 53, 57, -1, 35, 43, 53, 57, -1,  4, 23, 53, 57, -1, 28, 47, 53, 57, -1, 81, 53, 57, -1, 16, 53, 57, -1, 40,  0, 53, 57, -1, 81,
        53, 57, -1,  8, 20, 53, 57, -1, 32, 44, 53, 57, -1, 12, 24, 53, 57, -1, 36, 48, 53, 57, -1, 81, 53, 57, -1, 49, 53, 57, -1, -1,  0, 53, 57, -1, 81
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_500 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 37, 37 },
        {  3, 13, 27,  4, 14, 41,  8, 18, 31,  9, 19, 46,  0,  1 },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  5 }
    };
    static uint32 hx4_tdm_497[] = { /* HX4_1G_200_D*: 10xF.QSGMII + 3xFlex[4x10] + 1GE */
        53, 57, 61,  1, 13, 53, 57, 61, 25, -1, 53, 57, 61,  5, 17, 53, 57, 61, 29, 37, 53, 57, 61, 81, 53, 57, 61,  9, 53, 57, 61, 33,  0, 53, 57, 61, 81,
        53, 57, 61,  2, 21, 53, 57, 61, 26, -1, 53, 57, 61,  6, 14, 53, 57, 61, 30, 38, 53, 57, 61, 81, 53, 57, 61, 18, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61, 10, 22, 53, 57, 61, 34, -1, 53, 57, 61,  3, 15, 53, 57, 61, 27, 39, 53, 57, 61, 81, 53, 57, 61,  7, 53, 57, 61, 31, -1, 53, 57, 61, 81,
        53, 57, 61, 11, 19, 53, 57, 61, 35, -1, 53, 57, 61,  4, 23, 53, 57, 61, 28, 40, 53, 57, 61, 81, 53, 57, 61, 16, 53, 57, 61, -1,  0, 53, 57, 61, 81,
        53, 57, 61,  8, 20, 53, 57, 61, 32, -1, 53, 57, 61, 12, 24, 53, 57, 61, 36, -1, 53, 57, 61, 81, 53, 57, 61, 49, 53, 57, 61, -1,  0, 53, 57, 61, 81
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_497 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 37, 37, 37 },
        {  3, 13, 27,  4, 14, 41,  8, 18, 31, 19,  0,  1,  2 },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  5,  5 }
    };
    static uint32 hx4_tdm_510[] = { /* HX4_1G_200_E*: 12xF.QSGMII + 2xFlex[4x10] + 1GE */
        53, 57, -1,  1, 13, 53, 57, -1, 25, 37, 53, 57, -1,  5, 17, 53, 57, -1, 29, 41, 53, 57, -1, 81, 53, 57, -1,  9, 53, 57, -1, 33,  0, 53, 57, -1, 81,
        53, 57, -1,  2, 21, 53, 57, -1, 26, 45, 53, 57, -1,  6, 14, 53, 57, -1, 30, 38, 53, 57, -1, 81, 53, 57, -1, 18, 53, 57, -1, 42,  0, 53, 57, -1, 81,
        53, 57, -1, 10, 22, 53, 57, -1, 34, 46, 53, 57, -1,  3, 15, 53, 57, -1, 27, 39, 53, 57, -1, 81, 53, 57, -1,  7, 53, 57, -1, 31, -1, 53, 57, -1, 81,
        53, 57, -1, 11, 19, 53, 57, -1, 35, 43, 53, 57, -1,  4, 23, 53, 57, -1, 28, 47, 53, 57, -1, 81, 53, 57, -1, 16, 53, 57, -1, 40,  0, 53, 57, -1, 81,
        53, 57, -1,  8, 20, 53, 57, -1, 32, 44, 53, 57, -1, 12, 24, 53, 57, -1, 36, 48, 53, 57, -1, 81, 53, 57, -1, 49, 53, 57, -1, -1,  0, 53, 57, -1, 81
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_510 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 37, 37 },
        {  3, 13, 27,  4, 14, 41,  8, 18, 31,  9, 19, 46,  0,  1 },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  5 }
    };
    static uint32 hx4_tdm_610[] = { /* HX4_1G_200_E*: 12xF.QSGMII + 2xFlex[4x10] + 1GE */
        53, 57,  1, 13, 53, 57, 25, 53, 57, 37,  5, 53, 57, 17, 29, 53, 57, 41, 53, 57,  9, 81, 53, 57, 33, 53, 57, 81,  0,
        53, 57,  2, 21, 53, 57, 26, 53, 57, 45,  6, 53, 57, 14, 30, 53, 57, 38, 53, 57, 18, 81, 53, 57, 42, 53, 57, 81,  0,
        53, 57, 10, 22, 53, 57, 34, 53, 57, 46,  3, 53, 57, 15, 27, 53, 57, 39, 53, 57,  7, 81, 53, 57, 31, 53, 57, 81, -1,
        53, 57, 11, 19, 53, 57, 35, 53, 57, 43,  4, 53, 57, 23, 28, 53, 57, 47, 53, 57, 16, 81, 53, 57, 40, 53, 57, 81,  0,
        53, 57,  8, 20, 53, 57, 32, 53, 57, 44, 12, 53, 57, 24, 36, 53, 57, 48, 53, 57, 49, 81, 53, 57, -1, 53, 57, 81,  0
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_610 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 29, 29 },
        {  2, 10, 20,  3, 13, 32,  6, 14, 24,  9, 17, 38,  0,  1 },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  4 }
    };
    static uint32 hx4_tdm_460[] = { /* HX4_1G_130: 7xF.QSGMII + Flex[4x10] + 2xHG[21] + 1GE */
        53, 57,  1, 13, 53, 61, 25, 53, 57,  5, 53, 61, 17, 53, 57, 81, 53, 61,  9, 53, 57, -1, 53, 61,  0,
        53, 57,  2, 21, 53, 61, 26, 53, 57,  6, 53, 61, 14, 53, 57, 81, 53, 61, 18, 53, 57, 81, 53, 61,  0,
        53, 57, 10, 22, 53, 61, 27, 53, 57,  3, 53, 61, 15, 53, 57, 81, 53, 61,  7, 53, 57, 81, 53, 61, -1,
        53, 57, 11, 19, 53, 61, 28, 53, 57,  4, 53, 61, 23, 53, 57, 81, 53, 61, 16, 53, 57, 81, 53, 61,  0,
        53, 57,  8, 20, 53, 61, -1, 53, 57, 12, 53, 61, 24, 53, 57, 81, 53, 61, 49, 53, 57, 81, 53, 61,  0
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_460 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0, 25, },
        {  2,  9, 18,  3, 12, 28,  6,  0, },
        {  0,  0,  0,  0,  0,  0,  0,  3, }
    };

    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_464 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0, 25, 25, 25},
        {  2,  9, 18,  3, 12, 28,  6,  0,  1,  5},
        {  0,  0,  0,  0,  0,  0,  0,  3,  6,  5}
    };

    /* HX4_1G_150_F  : 12xF.QSGMII + 2x(2xHGd[21]/4xHGs[11/10]) + 1GE */
    static uint32 hx4_tdm_414[] = {
        61, 57,  1, 13, 61, 57, 25, 61, 57, 37,  5, 61, 57, 17, 29, 61, 57, 41, 61, 57,  9, 81, 61, 57, 33, 61, 57, 81,  0,
        61, 57,  2, 21, 61, 57, 26, 61, 57, 45,  6, 61, 57, 14, 30, 61, 57, 38, 61, 57, 18, 81, 61, 57, 42, 61, 57, 81,  0,
        61, 57, 10, 22, 61, 57, 34, 61, 57, 46,  3, 61, 57, 15, 27, 61, 57, 39, 61, 57,  7, 81, 61, 57, 31, 61, 57, 81, -1,
        61, 57, 11, 19, 61, 57, 35, 61, 57, 43,  4, 61, 57, 23, 28, 61, 57, 47, 61, 57, 16, 81, 61, 57, 40, 61, 57, 81,  0,
        61, 57,  8, 20, 61, 57, 32, 61, 57, 44, 12, 61, 57, 24, 36, 61, 57, 48, 61, 57, 49, 81, 61, 57, -1, 61, 57, 81,  0
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_414 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 29, 29},
        {  2, 10, 20,  3, 13, 32,  6, 14, 24,  9, 17, 38, 1, 0},
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 4, 4}
    };
    static uint32 hx4_tdm_410[] = { /* HX4_1G_150_B, HX4_1G_150_C */
        53, 57,  1, 13, 53, 61, 25, 53, 57, 37,  5, 53, 61, 17, 29, 53, 57, 41, 53, 61,  9, 81, 53, 57, 33, 53, 61, 81,  0,
        53, 57,  2, 21, 53, 61, 26, 53, 57, 45,  6, 53, 61, 14, 30, 53, 57, 38, 53, 61, 18, 81, 53, 57, 42, 53, 61, 81,  0,
        53, 57, 10, 22, 53, 61, 34, 53, 57, 46,  3, 53, 61, 15, 27, 53, 57, 39, 53, 61,  7, 81, 53, 57, 31, 53, 61, 81, -1,
        53, 57, 11, 19, 53, 61, 35, 53, 57, 43,  4, 53, 61, 23, 28, 53, 57, 47, 53, 61, 16, 81, 53, 57, 40, 53, 61, 81,  0,
        53, 57,  8, 20, 53, 61, 32, 53, 57, 44, 12, 53, 61, 24, 36, 53, 57, 48, 53, 61, 49, 81, 53, 57, -1, 53, 61, 81,  0
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_410 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 29, 29, 29},
        {  2, 10, 20,  3, 13, 32,  6, 14, 24,  9, 17, 38,  0, 1,  5},
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, 7,  7}
    };
   static uint32 hx4_tdm_400[] = { /* HX4_1G_150_A */
        61, 57,  1, 13, 63, 53, 25, 61, 58, 37,  5, 63, 54, 17, 29, 61, 57, 41, 63, 55,  9, 81, 61, 58, 33, 63, 56, 81,  0,
        61, 57,  2, 21, 63, 53, 26, 61, 58, 45,  6, 63, 54, 14, 30, 61, 57, 38, 63, 55, 18, 81, 61, 58, 42, 63, 56, 81,  0,
        61, 57, 10, 22, 63, 53, 34, 61, 58, 46,  3, 63, 54, 15, 27, 61, 57, 39, 63, 55,  7, 81, 61, 58, 31, 63, 56, 81, -1,
        61, 57, 11, 19, 63, 53, 35, 61, 58, 43,  4, 63, 54, 23, 28, 61, 57, 47, 63, 55, 16, 81, 61, 58, 40, 63, 56, 81,  0,
        61, 57,  8, 20, 63, 53, 32, 61, 58, 44, 12, 63, 54, 24, 36, 61, 57, 48, 63, 55, 49, 81, 61, 58, -1, 63, 56, 81,  0
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_400 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, },
        {  2, 10, 20,  3, 13, 32,  6, 14, 24,  9, 17, 38, },
        {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, }
    };
    static uint32 hx4_tdm_420[] = { /* RGR_XFI_150 */
        53, 57, 61, 81, 53, 57, 61, 53, 57, 61,  1, 53, 57, 61, 81, 53, 57, 61, 53, 57, 61, 0, 53, 57, 61, 53, 57, 61, -1,
        53, 57, 61, 81, 53, 57, 61, 53, 57, 61,  2, 53, 57, 61, 81, 53, 57, 61, 53, 57, 61, 0, 53, 57, 61, 53, 57, 61, -1,
        53, 57, 61, 81, 53, 57, 61, 53, 57, 61,  3, 53, 57, 61, 81, 53, 57, 61, 53, 57, 61, 0, 53, 57, 61, 53, 57, 61, -1,
        53, 57, 61, 81, 53, 57, 61, 53, 57, 61,  4, 53, 57, 61, 81, 53, 57, 61, 53, 57, 61, 0, 53, 57, 61, 53, 57, 61, -1,
        53, 57, 61, 81, 53, 57, 61, 53, 57, 61, 49, 53, 57, 61, 81, 53, 57, 61, 53, 57, 61, 0, 53, 57, 61, 53, 57, 61, -1
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_420 = {
        1,
        {  0, 29, 29, 29 },
        { 10,  0,  1,  2 },
        {  0,  4,  4,  4 }
    };
    static uint32 hx4_tdm_514[] = { /* HX4_1G_200_E*: 7xF.QSGMII + 2xFlex[4x10] + 1GE */
        53, 57, -1,  1, 13, 53, 57, -1, 25, -1, 53, 57, -1,  5, 17, 53, 57, -1, -1, -1, 53, 57, -1, 81, 53, 57, -1,  9, 53, 57, -1, -1,  0, 53, 57, -1, 81,
        53, 57, -1,  2, 21, 53, 57, -1, 26, -1, 53, 57, -1,  6, 14, 53, 57, -1, -1, -1, 53, 57, -1, 81, 53, 57, -1, 18, 53, 57, -1, -1,  0, 53, 57, -1, 81,
        53, 57, -1, 10, 22, 53, 57, -1, -1, -1, 53, 57, -1,  3, 15, 53, 57, -1, 27, -1, 53, 57, -1, 81, 53, 57, -1,  7, 53, 57, -1, -1, -1, 53, 57, -1, 81,
        53, 57, -1, 11, 19, 53, 57, -1, -1, -1, 53, 57, -1,  4, 23, 53, 57, -1, 28, -1, 53, 57, -1, 81, 53, 57, -1, 16, 53, 57, -1, -1,  0, 53, 57, -1, 81,
        53, 57, -1,  8, 20, 53, 57, -1, -1, -1, 53, 57, -1, 12, 24, 53, 57, -1, -1, -1, 53, 57, -1, 81, 53, 57, -1, 49, 53, 57, -1, -1,  0, 53, 57, -1, 81
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_514 = {
        1,
        {  0,  0,  0,  0,  0,  0,  0, 37, 37 },
        {  3, 13, 27,  4, 14, 41,  8,  0,  1 },
        {  0,  0,  0,  0,  0,  0,  0,  5,  5 }
    };
    static uint32 hx4_tdm_450[] = { /* RGR_XFI_200 */
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,  1, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,  2, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,  3, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,  4, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1,
        53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1, 49, 53, 57, 61, 81,  0, 53, 57, 61, -1, 53, 57, 61, 81, -1, 53, 57, 61, -1
    };
    _soc_tr3_flex_tdm_inf_t flex_tr3_tdm_450 = {
        1,
        {  0, 37, 37, 37 },
        { 18, 0,  1,  2 },
        {  0, 5,  5,  5 }
    };

    switch (_tr3_port_config_id[unit]) {
    case 0:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_0);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_0, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 1:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_1);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_1, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 2:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_2);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_2, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 3:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_3);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_3, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 4:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_4);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_4, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 5:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_5);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_5, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 6:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_6);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_6, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 7:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_7);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_7, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 8:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_8);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_8, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 9:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_9);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_9, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 10:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_10);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_10, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 11:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_11);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_11, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    case 490:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_490);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_490, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_490;
        break;
    case 491:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_491);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_491, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_491;
        break;
    case 492:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_492);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_492, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_492;
        break;
    case 495:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_495);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_495, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_495;
        break;
    case 500:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_500);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_500, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_500;
        break;
    case 497:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_497);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_497, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_497;
        break;
    case 510:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_510);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_510, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_510;
        break;
    case 610:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_610);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_610, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_610;
        break;
    case 462:
    case 460:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_460);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_460, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_460;
        break;
    case 463:
    case 464:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_460);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_460, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_464;
        break;
    case 414:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_414);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_414, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_414;
        break;
    case 413:
    case 412:
    case 410:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_410);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_410, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_410;
        break;
    case 400:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_400);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_400, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_400;
        break;
    case 420:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_420);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_420, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_420;
        break;
    case 514:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_514);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_514, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_514;
        break;
    case 450:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_450);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_450, sizeof(uint32) * _soc_tr3_tdm_size);
        _soc_tr3_flex_tdm = flex_tr3_tdm_450;
        break;
    default:
        _soc_tr3_tdm_size = COUNTOF(hx4_tdm_0);
        sal_memcpy(&_soc_tr3_tdm_arr, &hx4_tdm_0, sizeof(uint32) * _soc_tr3_tdm_size);
        break;
    }
}

extern int (*_phy_wc40_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *,int);

#define _TR3_MMU_BYTES_TO_CELLS(_byte_)  \
    (((_byte_) + _TR3_MMU_BYTES_PER_CELL - 1) / _TR3_MMU_BYTES_PER_CELL)
    
STATIC void
_soc_hx4_mmu_init_dev_config(int unit, _soc_mmu_device_info_t *devcfg)
{
    sal_memset(devcfg, 0, sizeof(_soc_mmu_device_info_t));

    devcfg->max_pkt_byte = _TR3_MMU_MAX_PACKET_BYTES;
    devcfg->mmu_hdr_byte = _TR3_MMU_PACKET_HEADER_BYTES;
    devcfg->jumbo_pkt_size = _TR3_MMU_JUMBO_FRAME_BYTES;
    devcfg->default_mtu_size = _TR3_MMU_DEFAULT_MTU_BYTES;
    devcfg->mmu_cell_size = _TR3_MMU_BYTES_PER_CELL;
    devcfg->mmu_total_cell = _TR3_MMU_TOTAL_CELLS(unit) - _TR3_MMU_CELLS_RSVD_IP; /* cells for IP */
    devcfg->num_pg = _TR3_MMU_NUM_PG;
    devcfg->num_service_pool = _TR3_MMU_NUM_POOL;
}

STATIC int
_soc_hx4_default_pg_headroom(int unit, soc_port_t port, int lossless)
{
    int speed = 1000, hdrm = 0;

    if ((!lossless) || (IS_CPU_PORT(unit, port)) || (IS_AXP_PORT(unit, port))) {
        return 0;
    }

    speed = SOC_INFO(unit).port_speed_max[port];
    
    if ((speed >= 1000) && (speed < 10000)) {
        hdrm = 162;
    } else if ((speed >= 10000) && (speed < 20000)) {
        hdrm = 242;
    } else if ((speed >= 20000) && (speed <= 42000)) {
        hdrm = 344;
    } else if (speed >= 100000) {
        hdrm = 768;
    } else {
        hdrm = 162;
    }
    return hdrm;
}

STATIC void
_soc_hx4_mmu_config_buf_default(int unit, _soc_mmu_cfg_buf_t *buf)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    int max_packet_cells, jumbo_frame_cells, default_mtu_cells;
    int port, idx, per_q_guarentee;
    int total_pool_size = 0, egr_shared_total = 0, lossless = 1;
    int q_reserved = 0, numq, tmp;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;

    si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Initializing default MMU config (u=%d)\n"), unit));
    max_packet_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(_TR3_MMU_MAX_PACKET_BYTES +
                                              _TR3_MMU_PACKET_HEADER_BYTES,
                                              _TR3_MMU_BYTES_PER_CELL);
    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(_TR3_MMU_JUMBO_FRAME_BYTES +
                                               _TR3_MMU_PACKET_HEADER_BYTES,
                                               _TR3_MMU_BYTES_PER_CELL);
    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(_TR3_MMU_DEFAULT_MTU_BYTES +
                                               _TR3_MMU_PACKET_HEADER_BYTES,
                                               _TR3_MMU_BYTES_PER_CELL);

    total_pool_size = _TR3_MMU_TOTAL_CELLS(unit) - _TR3_MMU_CELLS_RSVD_IP;
    
    lossless = soc_property_get(unit, spn_MMU_LOSSLESS, 1);

    buf->headroom = 2 * jumbo_frame_cells;

    per_q_guarentee = (lossless) ? 0 : default_mtu_cells;
    PBMP_ALL_ITER(unit, port) {
        numq = 0;
        soc_tr3_get_def_qbase(unit, port, _SOC_TR3_INDEX_STYLE_UCAST_QUEUE, 
                       NULL, &tmp);
        numq += tmp;
        soc_tr3_get_def_qbase(unit, port, _SOC_TR3_INDEX_STYLE_MCAST_QUEUE, 
                       NULL, &tmp);
        numq += tmp;
        q_reserved += per_q_guarentee * numq;
    }
    
    egr_shared_total = total_pool_size - q_reserved;

    for (idx = 0; idx < _TR3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];

        if (idx == 0) {  /* 100% scale up by 100 */
            buf_pool->size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->yellow_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
            buf_pool->red_size = 10000 | _MMU_CFG_BUF_PERCENT_FLAG;
        } else {
            buf_pool->size = 0;
            buf_pool->yellow_size = 0;
            buf_pool->red_size = 0;
        }
    }

    for (idx = 0; idx < SOC_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
        queue_grp->guarantee = 0;
        queue_grp->pool_limit = total_pool_size;
        queue_grp->discard_enable = (lossless) ? 0 : 1;
        queue_grp->pool_scale = -1;
        queue_grp->pool_resume = default_mtu_cells * 2;
        queue_grp->red_limit = 7 | _MMU_CFG_BUF_DYNAMIC_FLAG;
        queue_grp->yellow_limit = 7 | _MMU_CFG_BUF_DYNAMIC_FLAG;
        queue_grp->yellow_resume = default_mtu_cells * 2;
        queue_grp->red_resume = default_mtu_cells * 2;
    }

    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        /* internal priority to priority group mapping */
        for (idx = 0; idx < 16; idx++) {
            buf_port->pri_to_prigroup[idx] = 7;
        }

        /* priority group to pool mapping */
        for (idx = 0; idx < _TR3_MMU_NUM_PG; idx++) {
            buf_port->prigroups[idx].pool_idx = 0;
        }

        for (idx = 0; idx < _TR3_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            buf_port_pool->guarantee = 0;
            buf_port_pool->pool_limit = 0;
            buf_port_pool->pool_resume = 0;
            if (idx == 0) {
                buf_port_pool->pool_limit = total_pool_size;
                buf_port_pool->pool_resume = 
                            total_pool_size - (default_mtu_cells * 2);
            }
        }

        buf_port->pkt_size = max_packet_cells;
        
        /* priority group */
        for (idx = 0; idx < _TR3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf_port->prigroups[idx];
            buf_prigroup->guarantee = 0;
            buf_prigroup->user_delay = -1;
            buf_prigroup->switch_delay = -1;
            buf_prigroup->pkt_size = max_packet_cells;
            buf_prigroup->device_headroom_enable = 0;
            buf_prigroup->pool_limit = 0;
            buf_prigroup->pool_floor = 0;
            buf_prigroup->pool_scale = -1;
            buf_prigroup->headroom = 0;
            buf_prigroup->pool_resume = 0;
            buf_prigroup->flow_control_enable = 0;
            if (idx == 7) {
                buf_prigroup->device_headroom_enable = 1;
                buf_prigroup->guarantee = jumbo_frame_cells;
                buf_prigroup->headroom = 
                    _soc_hx4_default_pg_headroom(unit, port, lossless);
                buf_prigroup->pool_resume = default_mtu_cells * 2;
                buf_prigroup->flow_control_enable = lossless;
                buf_prigroup->pool_scale = 7;
            }
        }

        /* multicast queue */
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[idx];
            buf_queue->qgroup_id = -1;
            if (lossless) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->color_discard_enable = 0;
                buf_queue->pool_resume = 16;
            } else {
                buf_queue->guarantee = default_mtu_cells;
                buf_queue->discard_enable = 1;
                buf_queue->pool_scale = 7;
                buf_queue->pool_limit = 0;
                buf_queue->red_limit = 7 | _MMU_CFG_BUF_DYNAMIC_FLAG
                                         | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->color_discard_enable = 1;
                buf_queue->pool_resume = 16;
            }
        }

        /* unicast queue */
        for (idx = 0; idx < si->port_num_uc_cosq[port]; idx++) {
            buf_queue = &buf_port->queues[si->port_num_cosq[port] + idx];
            buf_queue->qgroup_id = -1;
            if (lossless) {
                buf_queue->guarantee = 0;
                buf_queue->discard_enable = 0;
                buf_queue->pool_scale = -1;
                buf_queue->pool_limit = egr_shared_total;
                buf_queue->red_limit = egr_shared_total;
                buf_queue->yellow_limit = egr_shared_total;
                buf_queue->color_discard_enable = 0;
                buf_queue->pool_resume = default_mtu_cells * 2;
                buf_queue->yellow_resume = default_mtu_cells * 2;
                buf_queue->red_resume = default_mtu_cells * 2;
            } else {
                buf_queue->guarantee = default_mtu_cells;
                buf_queue->discard_enable = 1;
                buf_queue->pool_scale = 7;
                buf_queue->pool_limit = 0;
                buf_queue->red_limit = 7 | _MMU_CFG_BUF_DYNAMIC_FLAG
                                         | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->yellow_limit = 7 | _MMU_CFG_BUF_DYNAMIC_FLAG
                                            | _MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1;
                buf_queue->color_discard_enable = 1;
                buf_queue->pool_resume = default_mtu_cells * 2;
                buf_queue->yellow_resume = default_mtu_cells * 2;
                buf_queue->red_resume = default_mtu_cells * 2;
            }
        }

        /* queue to pool mapping */
        for (idx = 0; 
             idx < si->port_num_cosq[port] + si->port_num_uc_cosq[port]; idx++) {
            buf_port->queues[idx].pool_idx = 0;
        }
    }

    /* extended queue profiles */
    for (idx = 0; idx < SOC_MMU_CFG_EQUEUE_MAX; idx++) {
        buf_queue = &buf->equeues[idx];
        buf_queue->numq = 0;
        buf_queue->guarantee = 0;
        buf_queue->pool_limit = 0;
        buf_queue->discard_enable = 1;
        buf_queue->pool_scale = -1;
        buf_queue->pool_resume = default_mtu_cells * 2;
        buf_queue->red_limit = 7 | _MMU_CFG_BUF_DYNAMIC_FLAG;
        buf_queue->yellow_limit = 7 | _MMU_CFG_BUF_DYNAMIC_FLAG;
        buf_queue->yellow_resume = default_mtu_cells * 2;
        buf_queue->red_resume = default_mtu_cells * 2;
    }

}

STATIC int
_soc_hx4_pool_scale_to_limit(int size, int scale)
{
    int factor = 1000;

    switch (scale) {
        case 7: factor = 875; break;
        case 6: factor = 750; break;
        case 5: factor = 625; break;
        case 4: factor = 500; break;
        case 3: factor = 375; break;
        case 2: factor = 250; break;
        case 1: factor = 125; break;
        case 0:
        default:
            factor = 1000; break;
    }
    return (size * factor)/1000;
}

STATIC int
_soc_hx4_mmu_config_buf_set_hw(int unit, _soc_mmu_cfg_buf_t *buf)
{
    soc_info_t *si;
    _soc_mmu_cfg_buf_pool_t *buf_pool;
    _soc_mmu_cfg_buf_port_t *buf_port;
    _soc_mmu_cfg_buf_prigroup_t *buf_prigroup;
    _soc_mmu_cfg_buf_queue_t *buf_queue;
    _soc_mmu_cfg_buf_port_pool_t *buf_port_pool;
    soc_reg_t reg = INVALIDr;
    uint32 rval, fval;
    uint32 entry0[SOC_MAX_MEM_WORDS], entry1[SOC_MAX_MEM_WORDS];
    thdi_port_pg_config_entry_t pg_config_mem;
    thdi_port_sp_config_entry_t thdi_sp_config;
    mmu_thdo_config_qgroup_entry_t cfg_qgrp;
    int default_mtu_cells, limit, midx, pri, pool;
    int port, phy_port, mmu_port, base, numq, idx;
    int jumbo_frame_cells, pval;
    _soc_mmu_cfg_buf_qgroup_t *queue_grp;
    static const soc_field_t prigroup_reg[] = {
        PORT_PRI_GRP0r, PORT_PRI_GRP1r
    };
    static const soc_field_t prigroup_field[] = {
        PRI0_GRPf, PRI1_GRPf, PRI2_GRPf, PRI3_GRPf,
        PRI4_GRPf, PRI5_GRPf, PRI6_GRPf, PRI7_GRPf,
        PRI8_GRPf, PRI9_GRPf, PRI10_GRPf, PRI11_GRPf,
        PRI12_GRPf, PRI13_GRPf, PRI14_GRPf, PRI15_GRPf
    };
    static const soc_field_t prigroup_spid_field[] = {
        PG0_SPIDf, PG1_SPIDf, PG2_SPIDf, PG3_SPIDf,
        PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf
    };

    si = &SOC_INFO(unit);

    default_mtu_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(_TR3_MMU_DEFAULT_MTU_BYTES +
                                               _TR3_MMU_PACKET_HEADER_BYTES,
                                               _TR3_MMU_BYTES_PER_CELL);

    jumbo_frame_cells = _MMU_CFG_MMU_BYTES_TO_CELLS(_TR3_MMU_JUMBO_FRAME_BYTES +
                                               _TR3_MMU_PACKET_HEADER_BYTES,
                                               _TR3_MMU_BYTES_PER_CELL);

    rval = 0;
    fval = _TR3_MMU_TOTAL_CELLS(unit) - _TR3_MMU_CELLS_RSVD_IP;
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &rval, CFAPFULLSETPOINTf, fval);
    soc_reg_field_set(unit, CFAPFULLTHRESHOLDr, &rval, CFAPFULLRESETPOINTf,
                        fval - 2*jumbo_frame_cells);
    SOC_IF_ERROR_RETURN(WRITE_CFAPFULLTHRESHOLDr(unit, rval));

    /* internal priority to priority group mapping */
    PBMP_ALL_ITER(unit, port) {
        buf_port = &buf->ports[port];

        for (idx = 0; idx < 16; idx++) {
            if (idx % 8 == 0) { /* 8 fields per register */
                reg = prigroup_reg[idx / 8];
                rval = 0;
            }
            soc_reg_field_set(unit, reg, &rval, prigroup_field[idx],
                              buf_port->pri_to_prigroup[idx]);
            if (idx % 8 == 7) { /* 8 fields per register */
                SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
            }
        }
    }

    /* Input port device settings */
    rval = 0;
    soc_reg_field_set(unit, GLOBAL_HDRM_LIMITr, &rval, GLOBAL_HDRM_LIMITf,
                      buf->headroom);
    SOC_IF_ERROR_RETURN(WRITE_GLOBAL_HDRM_LIMITr(unit, rval));
    
    fval = 0;
    for (idx = 0; idx < _TR3_MMU_NUM_POOL; idx++) {
        if ((buf->pools[idx].size & ~_MMU_CFG_BUF_PERCENT_FLAG) != 0) {
            fval |= 1 << idx;
        }
    }
    rval = 0;
    soc_reg_field_set(unit, USE_SP_SHAREDr, &rval, ENABLEf, fval);
    SOC_IF_ERROR_RETURN(WRITE_USE_SP_SHAREDr(unit, rval));

    fval = 0;
    for (idx = 0; idx < _TR3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        if (buf_pool->yellow_size & _MMU_CFG_BUF_PERCENT_FLAG) {
            if ((buf_pool->yellow_size & ~_MMU_CFG_BUF_PERCENT_FLAG) < 10000) {
                fval |= 1 << idx;
            }
        } else {
            if (buf->pools[idx].size != buf->pools[idx].yellow_size) {
                fval |= 1 << idx;
            }
        }
    }
    rval = 0;
    soc_reg_field_set(unit, COLOR_AWAREr, &rval, ENABLEf, fval);
    SOC_IF_ERROR_RETURN(WRITE_COLOR_AWAREr(unit, rval));

    /* configure Q-groups */
    for (idx = 0; idx < SOC_MMU_CFG_QGROUP_MAX; idx++) {
        queue_grp = &buf->qgroups[idx];
    
        sal_memset(&cfg_qgrp, 0, sizeof(cfg_qgrp));

        soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                Q_MIN_LIMIT_CELLf, queue_grp->guarantee);
        if (queue_grp->pool_scale != -1) {
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                               Q_SHARED_ALPHA_CELLf, queue_grp->pool_scale);
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                                    Q_LIMIT_DYNAMIC_CELLf, 1);
        } else {
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                                Q_SHARED_LIMIT_CELLf, queue_grp->pool_limit);
        }
        
        if ((queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) ||
            (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG)) {
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
        }

        if (queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
            pval = _soc_hx4_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                                    LIMIT_RED_CELLf, pval/8);

        } else {
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                                    LIMIT_RED_CELLf, queue_grp->red_limit);
        }

        if (queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
            pval = _soc_hx4_pool_scale_to_limit(queue_grp->pool_limit,
                          queue_grp->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG);
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                                    LIMIT_YELLOW_CELLf, pval/8);

        } else {
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                                    LIMIT_YELLOW_CELLf, queue_grp->yellow_limit);
        }

        if (queue_grp->discard_enable) {
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QGROUPm, &cfg_qgrp, 
                                    Q_LIMIT_ENABLE_CELLf, 1);
        }

        SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, MMU_THDO_CONFIG_QGROUPm, MEM_BLOCK_ALL,
                               idx, &cfg_qgrp));
    }

    /* per service pool settings */
    for (idx = 0; idx < _TR3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = buf_pool->total - (buf_pool->queue_guarantee + 
                                   buf_pool->prigroup_headroom +
                                   buf_pool->prigroup_guarantee);

        rval = 0;
        soc_reg_field_set(unit, BUFFER_CELL_LIMIT_SPr, &rval, LIMITf, limit);
        SOC_IF_ERROR_RETURN(WRITE_BUFFER_CELL_LIMIT_SPr(unit, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, CELL_RESET_LIMIT_OFFSET_SPr, &rval, OFFSETf,
                          NUM_PORT(unit) * default_mtu_cells / 2);
        SOC_IF_ERROR_RETURN(WRITE_CELL_RESET_LIMIT_OFFSET_SPr(unit, idx, rval));

        if (buf_pool->yellow_size & _MMU_CFG_BUF_PERCENT_FLAG) {
            if ((buf_pool->yellow_size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 10000) {
                continue;
            }
        } else {
            if (buf_pool->size == buf_pool->yellow_size) {
                continue;
            }
        }

        if (buf_pool->yellow_size & _MMU_CFG_BUF_PERCENT_FLAG) {
            fval = (buf_pool->yellow_size & ~_MMU_CFG_BUF_PERCENT_FLAG) * limit /
                10000;
        } else {
            fval = buf_pool->yellow_size;
        }
        rval = 0;
        soc_reg_field_set(unit, CELL_SPAP_YELLOW_OFFSET_SPr, &rval, OFFSETf, 
                          limit - fval);
        SOC_IF_ERROR_RETURN
            (WRITE_CELL_SPAP_YELLOW_OFFSET_SPr(unit, idx, rval));

        if (buf_pool->red_size & _MMU_CFG_BUF_PERCENT_FLAG) {
            fval = (buf_pool->red_size & ~_MMU_CFG_BUF_PERCENT_FLAG) * limit /
                10000;
        } else {
            fval = buf_pool->red_size;
        }
        rval = 0;
        soc_reg_field_set(unit, CELL_SPAP_RED_OFFSET_SPr, &rval, OFFSETf,
                          limit - fval);
        SOC_IF_ERROR_RETURN(WRITE_CELL_SPAP_RED_OFFSET_SPr(unit, idx, rval));
    }

    /* Input port per port settings */
    PBMP_ALL_ITER(unit, port) {
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];
        buf_port = &buf->ports[port];

        rval = 0;
        for (idx = 0; idx < _TR3_MMU_NUM_PG; idx++) {
            soc_reg_field_set(unit, PORT_PG_SPIDr, &rval,
                              prigroup_spid_field[idx],
                              buf_port->prigroups[idx].pool_idx);
        }
        SOC_IF_ERROR_RETURN(WRITE_PORT_PG_SPIDr(unit, port, rval));

        /* Per port per pool settings */
        for (idx = 0; idx < _TR3_MMU_NUM_POOL; idx++) {
            buf_port_pool = &buf_port->pools[idx];
            midx = (mmu_port * 4) + idx;
            sal_memset(&thdi_sp_config, 0, sizeof(thdi_sp_config));
            soc_mem_field32_set(unit, THDI_PORT_SP_CONFIGm, &thdi_sp_config, 
                                PORT_SP_MIN_LIMITf, buf_port_pool->guarantee);
            soc_mem_field32_set(unit, THDI_PORT_SP_CONFIGm, &thdi_sp_config, 
                           PORT_SP_RESUME_LIMITf, buf_port_pool->pool_resume);
            soc_mem_field32_set(unit, THDI_PORT_SP_CONFIGm, &thdi_sp_config, 
                               PORT_SP_MAX_LIMITf, buf_port_pool->pool_limit);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, THDI_PORT_SP_CONFIGm, MEM_BLOCK_ALL,
                               midx, &thdi_sp_config));
        }

        fval = 0;
        for (idx = 0; idx < _TR3_MMU_NUM_PG; idx++) {
            if (buf_port->prigroups[idx].flow_control_enable != 0) {
                for (pri=0; pri < 16; pri++) {
                    if (buf_port->pri_to_prigroup[pri] == idx) {
                        fval |= 1 << pri;
                    }
                }
            }
        }

        rval = 0;
        soc_reg_field_set(unit, PORT_PRI_XON_ENABLEr, &rval,
                          PORT_PRI_XON_ENABLEf, fval);
        SOC_IF_ERROR_RETURN(WRITE_PORT_PRI_XON_ENABLEr(unit, port, rval));

        rval = 0;
        soc_reg_field_set(unit, PORT_MAX_PKT_SIZEr, &rval, PORT_MAX_PKT_SIZEf,
                          buf_port->pkt_size);
        SOC_IF_ERROR_RETURN(WRITE_PORT_MAX_PKT_SIZEr(unit, port, rval));

        /* Input port per port per priority group settings */
        for (idx = 0; idx < _TR3_MMU_NUM_PG; idx++) {
            buf_prigroup = &buf->ports[port].prigroups[idx];

            midx = (mmu_port * 8) + idx;
            sal_memset(&pg_config_mem, 0, sizeof(pg_config_mem));
            soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm, &pg_config_mem, 
                                PG_MIN_LIMITf, buf_prigroup->guarantee);

            if (buf_prigroup->pool_scale != -1) {
                soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm, &pg_config_mem, 
                                           PG_SHARED_DYNAMICf, 1);
                soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm, &pg_config_mem, 
                                    PG_SHARED_LIMITf, buf_prigroup->pool_scale);
            } else {
                soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm, &pg_config_mem, 
                                   PG_SHARED_LIMITf, buf_prigroup->pool_limit);
            }

            soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm, &pg_config_mem, 
                         PG_GBL_HDRM_ENf, buf_prigroup->device_headroom_enable);
            soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm, &pg_config_mem, 
                                       PG_HDRM_LIMITf, buf_prigroup->headroom);

            soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm, &pg_config_mem, 
                                 PG_RESET_OFFSETf, buf_prigroup->pool_resume);

            soc_mem_field32_set(unit, THDI_PORT_PG_CONFIGm, &pg_config_mem, 
                                  PG_RESET_FLOORf, buf_prigroup->pool_floor);
                                        
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, THDI_PORT_PG_CONFIGm, 
                                           MEM_BLOCK_ALL, midx, &pg_config_mem));
        }
    }

    /***********************************
     * THDO 
 */

    /* Output port per port per queue setting for multicast queue */
    PBMP_ALL_ITER(unit, port) {
        for (idx = 0; idx < si->port_num_cosq[port]; idx++) {
            buf_queue = &buf->ports[port].queues[idx];
            pool = buf_queue->pool_idx;
            buf_pool = &buf->pools[pool];
            rval = 0;
            soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval, Q_MIN_CELLf,
                              buf_queue->guarantee);
            if (buf_queue->pool_scale != -1) {
                soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                               Q_SHARED_ALPHA_CELLf, buf_queue->pool_scale);
            } else {
                soc_reg_field_set(unit, OP_QUEUE_CONFIG_CELLr, &rval,
                                Q_SHARED_LIMIT_CELLf, buf_pool->total);
            }
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIG_CELLr(unit, port, idx, rval));

            rval = 0;
            if (buf_queue->discard_enable) {
                soc_reg_field_set(unit, OP_QUEUE_CONFIG1_CELLr, &rval,
                                  Q_LIMIT_ENABLE_CELLf, 1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_reg_field_set(unit, OP_QUEUE_CONFIG1_CELLr, &rval,
                                  Q_LIMIT_DYNAMIC_CELLf, 1);
            }
            if (buf_queue->color_discard_enable) {
                soc_reg_field_set(unit, OP_QUEUE_CONFIG1_CELLr, &rval,
                                  Q_COLOR_ENABLE_CELLf, 1);
            }
            if (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                soc_reg_field_set(unit, OP_QUEUE_CONFIG1_CELLr, &rval,
                                  Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
            }
            soc_reg_field_set(unit, OP_QUEUE_CONFIG1_CELLr, &rval,
                              Q_SPIDf, buf_queue->pool_idx);
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_CONFIG1_CELLr(unit, port, idx, rval));

            rval = 0;
            if (buf_queue->red_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                soc_reg_field_set(unit, OP_QUEUE_LIMIT_COLOR_CELLr, &rval, REDf,
                        buf_queue->red_limit
                                  & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                                  & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                soc_reg_field_set(unit, OP_QUEUE_LIMIT_COLOR_CELLr, &rval, REDf,
                                                        buf_queue->red_limit/8);
            }   
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_LIMIT_COLOR_CELLr(unit, port, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, OP_QUEUE_RESET_OFFSET_CELLr, &rval,
                              Q_RESET_OFFSET_CELLf,
                              buf_queue->pool_resume / 8);
            SOC_IF_ERROR_RETURN
                (WRITE_OP_QUEUE_RESET_OFFSET_CELLr(unit, port, idx, rval));
        }
    }

    /* Output port per port per queue setting for regular unicast queue */
    PBMP_PORT_ITER(unit, port) {
        if (IS_CPU_PORT(unit, port)) {
            continue;
        }

        /* per port regular unicast queue */
        soc_tr3_get_def_qbase(unit, port, _SOC_TR3_INDEX_STYLE_UCAST_QUEUE,
                                &base, &numq);
        for (idx = 0; idx < numq; idx++) {
            buf_queue = &buf->ports[port].queues[si->port_num_cosq[port] + idx];

            sal_memset(entry0, 0, sizeof(mmu_thdo_config_queue_entry_t));
            sal_memset(entry1, 0, sizeof(mmu_thdo_offset_0_entry_t));

            soc_mem_field32_set(unit, MMU_THDO_CONFIG_QUEUEm, entry0, 
                                Q_MIN_LIMIT_CELLf, buf_queue->guarantee);
            if (buf_queue->discard_enable) {
                soc_mem_field32_set(unit, MMU_THDO_CONFIG_QUEUEm, entry0,
                                    Q_LIMIT_ENABLE_CELLf, 1);
            }
            if (buf_queue->pool_scale != -1) {
                soc_mem_field32_set(unit, MMU_THDO_CONFIG_QUEUEm, entry0,
                                    Q_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set(unit, MMU_THDO_CONFIG_QUEUEm, entry0,
                           Q_SHARED_ALPHA_CELLf, buf_queue->pool_scale);
            } else {
                /* Q_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set(unit, MMU_THDO_CONFIG_QUEUEm, entry0,
                               Q_SHARED_LIMIT_CELLf, buf_queue->pool_limit);
            }
            soc_mem_field32_set(unit, MMU_THDO_OFFSET_QUEUEm, 
                entry1, RESET_OFFSET_CELLf, buf_queue->pool_resume / 8);
            if (buf_queue->yellow_limit & _MMU_CFG_BUF_DYNAMIC_FLAG) {
                soc_mem_field32_set(unit, MMU_THDO_CONFIG_QUEUEm, entry0,
                                    Q_COLOR_LIMIT_DYNAMIC_CELLf, 1);
                soc_mem_field32_set
                    (unit, MMU_THDO_CONFIG_QUEUEm, entry0, LIMIT_YELLOW_CELLf,
                     buf_queue->yellow_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
                soc_mem_field32_set
                    (unit, MMU_THDO_CONFIG_QUEUEm, entry0, LIMIT_RED_CELLf,
                     buf_queue->red_limit
                              & ~_MMU_CFG_BUF_DYNAMIC_FLAG
                              & ~_MMU_CFG_BUF_DYNAMIC_MAPPING_TYPE_1);
            } else {
                /* Q_COLOR_LIMIT_DYNAMIC_CELLf is 0 */
                soc_mem_field32_set
                    (unit, MMU_THDO_CONFIG_QUEUEm, entry0, LIMIT_YELLOW_CELLf,
                     buf_queue->yellow_limit / 8);
                soc_mem_field32_set
                    (unit, MMU_THDO_CONFIG_QUEUEm, entry0, LIMIT_RED_CELLf,
                     buf_queue->red_limit / 8);
            }
            soc_mem_field32_set
                (unit, MMU_THDO_OFFSET_QUEUEm, entry1, RESET_OFFSET_YELLOW_CELLf,
                 buf_queue->yellow_resume / 8);
            soc_mem_field32_set
                (unit, MMU_THDO_OFFSET_QUEUEm, entry1, RESET_OFFSET_RED_CELLf,
                 buf_queue->red_resume / 8);
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, MMU_THDO_CONFIG_QUEUEm, MEM_BLOCK_ALL,
                               base + idx, entry0));
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, MMU_THDO_OFFSET_QUEUEm, MEM_BLOCK_ALL,
                               base + idx, entry1));

            sal_memset(entry0, 0, sizeof(mmu_thdo_q_to_qgrp_map_entry_t));
            soc_mem_field32_set(unit, MMU_THDO_Q_TO_QGRP_MAPm, 
                                entry0, Q_SPIDf, buf_queue[idx].pool_idx);
            if (buf_queue->color_discard_enable) {
                soc_mem_field32_set(unit, MMU_THDO_Q_TO_QGRP_MAPm, entry0,
                                    Q_COLOR_ENABLE_CELLf, 1);
            }

            if (buf_queue->qgroup_id != -1) {
                soc_mem_field32_set(unit, MMU_THDO_Q_TO_QGRP_MAPm, entry0,
                                    QGROUP_VALIDf, 1);
                soc_mem_field32_set(unit, MMU_THDO_Q_TO_QGRP_MAPm, entry0,
                                    QGROUPf, buf_queue->qgroup_id);
                if (buf_queue->qgroup_min_enable) {
                    soc_mem_field32_set(unit, MMU_THDO_Q_TO_QGRP_MAPm, entry0,
                                    USE_QGROUP_MINf, 1);
                }
            }
            
            SOC_IF_ERROR_RETURN(soc_mem_write(unit, MMU_THDO_Q_TO_QGRP_MAPm, 
                                              MEM_BLOCK_ALL, base + idx, entry0));
        }
    }


    /* Output port per pool settings */
    for (idx = 0; idx < _TR3_MMU_NUM_POOL; idx++) {
        buf_pool = &buf->pools[idx];
        if ((buf_pool->size & ~_MMU_CFG_BUF_PERCENT_FLAG) == 0) {
            continue;
        }

        limit = buf_pool->total - buf_pool->queue_guarantee;

        rval = 0;
        soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_CELLr, &rval,
                          OP_BUFFER_SHARED_LIMIT_CELLf, limit);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, OP_BUFFER_SHARED_LIMIT_CELLr, REG_PORT_ANY,
                           idx, rval));

        rval = 0;
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLr, &rval,
                          OP_BUFFER_LIMIT_YELLOW_CELLf, limit / 8);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, OP_BUFFER_LIMIT_YELLOW_CELLr, REG_PORT_ANY,
                           idx, rval));

        rval = 0;
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_RED_CELLr, &rval,
                          OP_BUFFER_LIMIT_RED_CELLf, limit / 8);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, OP_BUFFER_LIMIT_RED_CELLr, REG_PORT_ANY,
                           idx, rval));

        rval = 0;
        soc_reg_field_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLr, &rval,
                          OP_BUFFER_SHARED_LIMIT_RESUME_CELLf, limit);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, OP_BUFFER_SHARED_LIMIT_RESUME_CELLr,
                           REG_PORT_ANY, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_CELLr, &rval,
                          OP_BUFFER_LIMIT_RESUME_YELLOW_CELLf, limit / 8);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, OP_BUFFER_LIMIT_RESUME_YELLOW_CELLr,
                           REG_PORT_ANY, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, OP_BUFFER_LIMIT_RESUME_RED_CELLr, &rval,
                          OP_BUFFER_LIMIT_RESUME_RED_CELLf, limit / 8);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, OP_BUFFER_LIMIT_RESUME_RED_CELLr,
                           REG_PORT_ANY, idx, rval));

        /* Output port per port per pool setting */
        PBMP_PORT_ITER(unit, port) {
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];

            sal_memset(entry0, 0, sizeof(mmu_thdo_config_port_entry_t));
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_PORTm, entry0,
                                                      SHARED_LIMITf, limit);
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_PORTm, entry0,
                              SHARED_RESUMEf, limit - (default_mtu_cells * 2));

            soc_mem_field32_set(unit, MMU_THDO_CONFIG_PORTm, entry0,
                                                      YELLOW_LIMITf, limit/8);
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_PORTm, entry0,
                            YELLOW_RESUMEf, (limit - (default_mtu_cells*2))/8);

            soc_mem_field32_set(unit, MMU_THDO_CONFIG_PORTm, entry0,
                                                      RED_LIMITf, limit/8);
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_PORTm, entry0,
                              RED_RESUMEf, (limit - (default_mtu_cells * 2))/8);

            SOC_IF_ERROR_RETURN
                (WRITE_MMU_THDO_CONFIG_PORTm(unit, MEM_BLOCK_ANY, 
                                             (mmu_port*4) + idx, &entry0));
        }

        /* Configure WLAN DECAP port. */
        if (soc_feature(unit, soc_feature_axp)) {
            port = AXP_PORT(unit, SOC_AXP_NLF_WLAN_DECAP);
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];

            sal_memset(entry0, 0, sizeof(mmu_thdo_config_port_entry_t));
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_PORTm, entry0,
                                                    SHARED_LIMITf, limit);
            soc_mem_field32_set(unit, MMU_THDO_CONFIG_PORTm, entry0,
                            SHARED_RESUMEf, limit - (default_mtu_cells * 2));
            SOC_IF_ERROR_RETURN
                (WRITE_MMU_THDO_CONFIG_PORTm(unit, MEM_BLOCK_ANY,
                                            (mmu_port*4) + idx, &entry0));
        }

        /* Output port per port per pool setting for multicast queues */
        PBMP_PORT_ITER(unit, port) {
            rval = 0;
            soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &rval,
                              OP_SHARED_LIMIT_CELLf, limit);
            soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &rval,
                              OP_SHARED_RESET_VALUE_CELLf,
                              limit - (2 * default_mtu_cells));
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, OP_PORT_CONFIG_CELLr, port, idx, rval));

            rval = 0;
            soc_reg_field_set(unit, OP_PORT_LIMIT_COLOR_CELLr, &rval, REDf,
                              limit / 8);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, OP_PORT_LIMIT_COLOR_CELLr, port, idx,
                               rval));

            rval = 0;
            soc_reg_field_set
                (unit, OP_PORT_LIMIT_RESUME_COLOR_CELLr, &rval, REDf,
                              (limit - (2*default_mtu_cells)) / 8);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, OP_PORT_LIMIT_RESUME_COLOR_CELLr, port,
                               idx, rval));
        }

        /* Configure WLAN ENCAP port. */
        if (soc_feature(unit, soc_feature_axp)) {
            port = AXP_PORT(unit, SOC_AXP_NLF_WLAN_ENCAP);
            rval = 0;
            soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &rval,
                            OP_SHARED_LIMIT_CELLf, limit);
            soc_reg_field_set(unit, OP_PORT_CONFIG_CELLr, &rval,
                            OP_SHARED_RESET_VALUE_CELLf,
                            limit - (2 * default_mtu_cells));
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, OP_PORT_CONFIG_CELLr, port, idx, rval));
        }
    }

    /* setup WAMU */
    rval = 0;
    soc_reg_field_set(unit, WAMUTOTALSHRLMTINGr, &rval, SHRPKT_LMTf, 0xfff);
    soc_reg_field_set(unit, WAMUTOTALSHRLMTINGr, &rval, SHRCELL_LMTf, 0x7fff);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, WAMUTOTALSHRLMTINGr, REG_PORT_ANY, 0, rval));
    
    rval = 0;
    soc_reg_field_set(unit, WAMUTOTALSHRLMTPKTEGRr, &rval, SHR_LMTf, 0xfff);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, WAMUTOTALSHRLMTPKTEGRr, REG_PORT_ANY, 0, rval));
    
    rval = 0;
    soc_reg_field_set(unit, CNTX_AGING_LIMITr, &rval, AGING_LIMITf, 0x1fff);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, CNTX_AGING_LIMITr, REG_PORT_ANY, 0, rval));
    
    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, WAMUTHDBYPASSr, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, WAMU_PG_MIN_CELLr, &rval, PG_MIN_ENf, 1);
    soc_reg_field_set(unit, WAMU_PG_MIN_CELLr, &rval, PG_MINf, 
                                                jumbo_frame_cells);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, WAMU_PG_MIN_CELLr, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, WAMU_PG_SHARED_LIMIT_CELLr, &rval, 
                        PG_SHARED_LIMITf, 6);
    soc_reg_field_set(unit, WAMU_PG_SHARED_LIMIT_CELLr, &rval, 
                        PG_SHARED_DYNAMICf, 1);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, WAMU_PG_SHARED_LIMIT_CELLr, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, WAMU_PG_RESET_OFFSET_CELLr, &rval, 
                        WAMU_PG_RESET_OFFSETf, 1);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, WAMU_PG_RESET_OFFSET_CELLr, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, WAMU_PG_RESUME_LIMITr, &rval, 
                            PG_RESUME_LIMITf, 0x2fff);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, WAMU_PG_RESUME_LIMITr, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, WAMUMINLMTPGr, &rval, PKTLMTf, 2);
    soc_reg_field_set(unit, WAMUMINLMTPGr, &rval, CELLLMTf, default_mtu_cells);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, WAMUMINLMTPGr, REG_PORT_ANY, 0, rval));

    rval = 0;
    soc_reg_field_set(unit, WAMUSHRLMTCELLPGr, &rval, SHRLMTf, 6);
    soc_reg_field_set(unit, WAMUSHRLMTCELLPGr, &rval, DYNMf, 1);
    SOC_IF_ERROR_RETURN
        (soc_reg32_set(unit, WAMUSHRLMTCELLPGr, REG_PORT_ANY, 0, rval));

    for (idx = 0; idx < 8; idx++) {
        rval = 0;
        soc_reg_field_set(unit, WAMUSHRLMTPKTQr, &rval, SHRLMTPKTf, 0xfff);
        soc_reg_field_set(unit, WAMUSHRLMTPKTQr, &rval, DYNMf, 1);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, WAMUSHRLMTPKTQr, REG_PORT_ANY, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, WAMUMINLMTPKTQr, &rval, MINLMTPKTf, 2);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, WAMUMINLMTPKTQr, REG_PORT_ANY, idx, rval));

        rval = 0;
        soc_reg_field_set(unit, WAMURSTOFFSETPKTQr, &rval, RSTOFFSETf, 2);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, WAMURSTOFFSETPKTQr, REG_PORT_ANY, idx, rval));
    }

    return SOC_E_NONE;
}

int
soc_hx4_mmu_config_init(int unit, int test_only)
{
    int rv;
    _soc_mmu_cfg_buf_t *buf;
    _soc_mmu_device_info_t devcfg;

    buf = soc_mmu_cfg_alloc(unit);
    if (!buf) {
        return SOC_E_MEMORY;
    }
    
    _soc_hx4_mmu_init_dev_config(unit, &devcfg);
    _soc_hx4_mmu_config_buf_default(unit, buf);
    _soc_mmu_cfg_buf_read(unit, buf, &devcfg);
    rv = _soc_mmu_cfg_buf_check(unit, buf, &devcfg);
    if (!test_only) {
        if (SOC_FAILURE(rv)) {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "MMU config: Use default setting\n")));
            _soc_hx4_mmu_config_buf_default(unit, buf);
            _soc_mmu_cfg_buf_calculate(unit, buf, &devcfg);
        }
        rv = _soc_hx4_mmu_config_buf_set_hw(unit, buf);
    }

    soc_mmu_cfg_free(unit, buf);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MMU THDI/THDO init done\n")));
    return rv;
}

STATIC int
_soc_hx4_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    *age_seconds = soc->l2x_age_interval ? (soc->l2x_age_interval) : 0;
    *enabled = (soc->l2x_age_pid != SAL_THREAD_ERROR) && (soc->l2x_age_interval ? 1 : 0);
    return SOC_E_NONE; 
}

STATIC int
_soc_hx4_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds = 0xffffffff;
    return SOC_E_NONE;
}

STATIC int
_soc_hx4_age_timer_set(int unit, int age_seconds, int enable)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t interval = soc->l2x_age_interval;
    /* 
     * SW aging thread running, 
     * update aging interval and notify the thread 
     */
    if (soc->l2x_age_interval) {
        if (!enable) {
            SOC_IF_ERROR_RETURN(soc_tr3_l2_bulk_age_stop(unit));
        } else {
            if (age_seconds) {
                SOC_CONTROL_LOCK(unit);
                /* Changing age interval, clear previous history */
                if (soc->l2x_prev_age_timeout != age_seconds) {
                    soc->l2x_agetime_adjust_usec = 0;
                }
                soc->l2x_age_interval = age_seconds;
                soc->l2x_prev_age_timeout = age_seconds;
                SOC_CONTROL_UNLOCK(unit);
            }
            if (interval != age_seconds) {
                /* Wake up thread to update the new age timer */
                sal_sem_give(soc->l2x_age_notify);
            }
        }       
    } else {  
        /* SW aging not runninig, start the thread */
        if (enable) {
            if (age_seconds) {
                SOC_CONTROL_LOCK(unit);
                soc->l2x_prev_age_timeout = age_seconds;
                SOC_CONTROL_UNLOCK(unit);
            }
            SOC_IF_ERROR_RETURN
                (soc_tr3_l2_bulk_age_start(unit, age_seconds));
        }
    }
    return SOC_E_NONE;
}

static soc_tr3_oam_handler_t hx4_oam_handler[SOC_MAX_NUM_DEVICES] = {NULL};

void
soc_hx4_oam_handler_register(int unit, soc_tr3_oam_handler_t handler)
{
    hx4_oam_handler[unit] = handler;
    soc_intr_enable(unit, IRQ_MEM_FAIL);
}

static const soc_reg_t thermal_reg[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r
};

int
soc_hx4_show_temperature_monitor(int unit)
{
    uint32 rval;
    soc_reg_t reg;
    int index;
    int fval, cur, peak, avg_cur, max_peak;
    uint16 dev_id, drv_dev_id, drv_rev_id;
    uint8 rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    soc_cm_get_id_driver(dev_id, rev_id, &drv_dev_id, &drv_rev_id);

    reg = TOP_PVTMON_CTRL_0r;
    READ_TOP_PVTMON_CTRL_0r(unit, &rval);
    soc_reg_field_set(unit, reg, &rval, FUNC_MODE_SELf, 0);
    soc_reg_field_set(unit, reg, &rval, MEASUREMENT_CALLIBRATIONf, 5);
    soc_reg_field_set(unit, reg, &rval, BG_ADJf, 3);
    WRITE_TOP_PVTMON_CTRL_0r(unit, rval);
        
    reg = TOP_PVTMON_CTRL_1r;
    READ_TOP_PVTMON_CTRL_1r(unit, &rval);
    soc_reg_field_set(unit, reg, &rval, PVTMON_SELECTf, 0);
    soc_reg_field_set(unit, reg, &rval, PVTMON_RESET_Nf, 0);
    WRITE_TOP_PVTMON_CTRL_1r(unit, rval);
    soc_reg_field_set(unit, reg, &rval, PVTMON_RESET_Nf, 1);
    WRITE_TOP_PVTMON_CTRL_1r(unit, rval);

    sal_usleep(1000);

    avg_cur = 0;
    max_peak = 0x80000000;
    for (index = 0; index < COUNTOF(thermal_reg); index++) {
        reg = thermal_reg[index];
        soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval);
        fval = soc_reg_field_get(unit, reg, rval, TEMP_DATAf);
        cur = (4180000 - (5556 * fval)) / 10000;
        fval = soc_reg_field_get(unit, reg, rval, PEAK_TEMP_DATAf);
        peak = (4180000 - (5556 * fval)) / 10000;
        LOG_CLI((BSL_META_U(unit,
                            "Temperature monitor %d: current=%3d, peak=%3d\n"),
                 index, cur, peak));
        avg_cur += cur;
        if (max_peak < peak) {
            max_peak = peak;
        }
    }
    LOG_CLI((BSL_META_U(unit,
                        "Average current temperature is %d\n"), 
             avg_cur / COUNTOF(thermal_reg)));
    LOG_CLI((BSL_META_U(unit,
                        "Maximum peak temperature is %d\n"), max_peak));

    READ_TOP_SOFT_RESET_REG_2r(unit, &rval);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 0);
    WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                      TOP_TEMP_MON_PEAK_RST_Lf, 1);
    WRITE_TOP_SOFT_RESET_REG_2r(unit, rval);

    return SOC_E_NONE;
}

soc_blk_ctr_reg_desc_t _soc_hx4_blk_ctr_reg_desc[] = {
    {
        SOC_BLK_AXP,
        {
            {AXP_SM_PACKETS_RECEIVED_COUNTERr, 1, 1, 0},
            {AXP_SM_PACKETS_SENT_COUNTERr, 1, 1, 0},
            {AXP_SM_PACKETS_DROPPED_COUNTERr, 1, 1, 0},
            {AXP_SM_BYTES_MATCHED_COUNTERr, 1, 1, 0},
            {AXP_SM_MATCHED_FLOWS_COUNTERr, 1, 1, 0},
            {AXP_SM_UNMATCHED_FLOWS_COUNTERr, 1, 1, 0},
            {AXP_SM_TOTAL_MATCH_COUNTERr, 1, 1, 0},
            {AXP_SM_TOTAL_CROSS_SIG_FLAGS_COUNTERr, 1, 1, 0},
            {AXP_SM_FRAGMENTS_RECEIVED_COUNTERr, 1, 1, 0},
            {AXP_SM_IN_PACKET_ERROR_COUNTERr, 1, 1, 0},
            {AXP_SM_FLOW_TRACKER_ERROR_COUNTERr, 1, 1, 0},
            {AXP_SM_PACKET_LENGTH_ERROR_COUNTERr, 1, 1, 0},
            {AXP_SM_L4_CHECKSUM_ERROR_COUNTERr, 1, 1, 0},
            {AXP_SM_FLOW_DONE_PACKET_DROP_COUNTERr, 1, 1, 0},
            {AXP_SM_FLOW_TIMESTAMP_ERROR_COUNTERr, 1, 1, 0},
            {AXP_SM_FLOW_PACKET_NUM_ERROR_COUNTERr, 1, 1, 0},
            {AXP_SM_ECC_ERROR_COUNTERr, 1, 1, 0},
            {INVALIDr, 0, 0}
        }
    }
};

void
soc_hx4_blk_counter_config(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc->blk_ctr_desc_count = COUNTOF(_soc_hx4_blk_ctr_reg_desc);
    soc->blk_ctr_desc = _soc_hx4_blk_ctr_reg_desc;
}

int
_soc_hx4_ledup_init(int unit)
{

    uint32 rval = 0, ix;

    /* configure the LED scan delay cycles */
    SOC_IF_ERROR_RETURN(READ_CMIC_LEDUP0_CTRLr(unit, &rval));
    soc_reg_field_set(unit, CMIC_LEDUP0_CTRLr, &rval, LEDUP_SCAN_START_DELAYf, 10);
    soc_reg_field_set(unit, CMIC_LEDUP0_CTRLr, &rval, LEDUP_SCAN_INTRA_PORT_DELAYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_CTRLr(unit, rval));
   /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(READ_CMIC_LEDUP1_CTRLr(unit, &rval));
    soc_reg_field_set(unit, CMIC_LEDUP1_CTRLr, &rval, LEDUP_SCAN_START_DELAYf, 0);
    soc_reg_field_set(unit, CMIC_LEDUP1_CTRLr, &rval, LEDUP_SCAN_INTRA_PORT_DELAYf, 1);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_CTRLr(unit, rval));

    rval =0;
    /* Initialize LED Port remap registers */
    
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_60_63r(unit, rval));
    
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r(unit, rval));
    /* coverity[result_independent_of_operands] */
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_8_11r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_12_15r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_16_19r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_20_23r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_24_27r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_28_31r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_32_35r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_36_39r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_40_43r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_44_47r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_48_51r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_52_55r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_56_59r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_60_63r(unit, rval));

    /* Configure LED Port remap registers */
    
    ix = 0;
    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, &rval, REMAP_PORT_59f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, &rval, REMAP_PORT_58f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, &rval, REMAP_PORT_57f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r, &rval, REMAP_PORT_56f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_56_59r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r, &rval, REMAP_PORT_55f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r, &rval, REMAP_PORT_54f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r, &rval, REMAP_PORT_53f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r, &rval, REMAP_PORT_52f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_52_55r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, &rval, REMAP_PORT_51f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, &rval, REMAP_PORT_50f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, &rval, REMAP_PORT_49f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r, &rval, REMAP_PORT_48f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_48_51r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_47f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_46f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_45f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r, &rval, REMAP_PORT_44f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_44_47r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_43f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_42f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_41f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r, &rval, REMAP_PORT_40f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_40_43r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_39f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_38f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_37f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r, &rval, REMAP_PORT_36f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_36_39r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_35f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_34f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_33f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r, &rval, REMAP_PORT_32f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_32_35r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_31f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_30f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_29f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r, &rval, REMAP_PORT_28f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_28_31r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_27f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_26f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_25f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r, &rval, REMAP_PORT_24f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_24_27r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_23f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_22f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_21f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r, &rval, REMAP_PORT_20f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_20_23r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_19f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_18f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_17f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r, &rval, REMAP_PORT_16f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_16_19r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_15f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_14f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_13f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r, &rval, REMAP_PORT_12f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_12_15r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_11f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_10f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_9f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r, &rval, REMAP_PORT_8f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_8_11r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_7f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_6f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_4_7r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_PORT_ORDER_REMAP_0_3r(unit, rval));

    /* Configure LED Port remap registers - LED uP1*/
    ix = 0;
    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_3f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_2f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_1f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r, &rval, REMAP_PORT_0f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_0_3r(unit, rval));

    rval = 0;
    soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_5f, ++ix);
    soc_reg_field_set(unit, CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r, &rval, REMAP_PORT_4f, ++ix);
    SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_PORT_ORDER_REMAP_4_7r(unit, rval));

    /* initialize the UP0, UP1 data ram */
    rval = 0;
    for (ix = 0; ix < 256; ix++) {
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP0_DATA_RAMr(unit,ix, rval));
        SOC_IF_ERROR_RETURN(WRITE_CMIC_LEDUP1_DATA_RAMr(unit,ix, rval));
    }

    return SOC_E_NONE;
}


/*
 * Tr3 chip driver functions.
 */
soc_functions_t soc_helix4_drv_funs = {
    
    _soc_tr3_misc_init,
    _soc_tr3_mmu_init,
    _soc_hx4_age_timer_get,
    _soc_hx4_age_timer_max_get,
    _soc_hx4_age_timer_set,
};

#endif /* BCM_HELIX4_SUPPORT */
