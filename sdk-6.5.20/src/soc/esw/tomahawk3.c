/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        tomahawk3.c
 * Purpose:
 * Requires:
 */
#include <bcm_int/esw_dispatch.h>

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/core/dpc.h>
#include <shared/bsl.h>
#include <soc/tdm/core/tdm_top.h>
#ifdef BCM_CMICX_SUPPORT
#include <soc/cmicx.h>
#include <soc/cmicx_miim.h>
#endif
#include <soc/bradley.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/i2c.h>
#include <soc/l2x.h>
#include <soc/soc_ser_log.h>
#include <soc/iproc.h>
#include <soc/mcm/intr_iproc.h>

#ifdef BCM_TOMAHAWK3_SUPPORT

#ifdef ALPM_ENABLE
#include <soc/lpm.h>
#include <soc/alpm.h>
#include <soc/esw/alpm_int.h>
#endif /* ALPM_ENABLE */

#include <soc/esw/portctrl.h>

#include <soc/mmu_config.h>
#include <soc/tomahawk.h>
#include <soc/tomahawk3.h>
#include <soc/init/tomahawk3_tdm.h>
#include <soc/init/tomahawk3_idb_init.h>
#include <soc/trident2.h>
#include <soc/drv.h>
#include <soc/pfc.h>
#include <soc/pstats.h>

#define NUM_READS 200

#define _SOC_TH3_HW_TEMP_MAX   (125)
#define _SOC_TH3_SW_TEMP_MAX   (110)

#define UPDATE_ALL_PIPES -1

#define _SOC_TH_RECONFIGURE_PORT_TYPE_INFO(ptype) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, it_port) { \
        si->ptype.port[si->ptype.num++] = it_port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = it_port; \
        } \
        if (it_port > si->ptype.max) { \
            si->ptype.max = it_port; \
        } \
    }

static int num_shared_alpm_banks[SOC_MAX_NUM_DEVICES] = {0};
static soc_mem_t *_soc_th3_lpm_view_map[SOC_MAX_NUM_DEVICES];
uint32 _soc_mem_entry_reset_value[1] = {1};

soc_th_latency_table_cfg_t
soc_th3_tbl_cfg[SOC_MAX_NUM_DEVICES] = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

static const soc_reg_t temp_thr_reg[] = {
    TOP_PVTMON_0_INTR_THRESHOLDr, TOP_PVTMON_1_INTR_THRESHOLDr,
    TOP_PVTMON_2_INTR_THRESHOLDr, TOP_PVTMON_3_INTR_THRESHOLDr,
    TOP_PVTMON_4_INTR_THRESHOLDr, TOP_PVTMON_5_INTR_THRESHOLDr,
    TOP_PVTMON_6_INTR_THRESHOLDr, TOP_PVTMON_7_INTR_THRESHOLDr,
    TOP_PVTMON_8_INTR_THRESHOLDr, TOP_PVTMON_9_INTR_THRESHOLDr,
    TOP_PVTMON_10_INTR_THRESHOLDr, TOP_PVTMON_11_INTR_THRESHOLDr,
    TOP_PVTMON_12_INTR_THRESHOLDr, TOP_PVTMON_13_INTR_THRESHOLDr,
    TOP_PVTMON_14_INTR_THRESHOLDr, TOP_PVTMON_15_INTR_THRESHOLDr
};
static const soc_reg_t pvtmon_ctrl_1_reg_th3[] = {
    TOP_PVTMON_0_CTRL_1r, TOP_PVTMON_1_CTRL_1r,
    TOP_PVTMON_2_CTRL_1r, TOP_PVTMON_3_CTRL_1r,
    TOP_PVTMON_4_CTRL_1r
};

static const struct {
    soc_reg_t pvtmon_reg;
    soc_field_t pvtmon_field;
} temp_pvtmon_ctrl_reg[] = {
        {TOP_PVTMON_0_CTRL_1r, TOP_PVT_MON_MIN_RST_Lf},
        {TOP_PVTMON_1_CTRL_1r, TOP_PVT_MON_MIN_RST_Lf},
        {TOP_PVTMON_2_CTRL_1r, TOP_PVT_MON_MIN_RST_Lf},
        {TOP_PVTMON_3_CTRL_1r, TOP_PVT_MON_MIN_RST_Lf},
        {TOP_PVTMON_4_CTRL_1r, TOP_PVT_MON_MIN_RST_Lf},
        {TOP_PVTMON_13_17_CTRL_1r, PVTMON13_TOP_PVT_MON_MIN_RST_Lf},
        {TOP_PVTMON_13_17_CTRL_1r, PVTMON14_TOP_PVT_MON_MIN_RST_Lf},
        {TOP_PVTMON_13_17_CTRL_1r, PVTMON15_TOP_PVT_MON_MIN_RST_Lf},
    };

static const char *temp_thr_prop[] = {
    "temp0_threshold", "temp1_threshold", "temp2_threshold",
    "temp3_threshold", "temp4_threshold", "temp5_threshold",
    "temp6_threshold", "temp7_threshold", "temp8_threshold",
    "temp9_threshold", "temp10_threshold", "temp11_threshold",
    "temp12_threshold", "temp13_threshold", "temp14_threshold",
    "temp15_threshold"
};

static const soc_reg_t pvtmon_result_reg_th3[] = {
    TOP_PVTMON_RESULT_0r, TOP_PVTMON_RESULT_1r,
    TOP_PVTMON_RESULT_2r, TOP_PVTMON_RESULT_3r,
    TOP_PVTMON_RESULT_4r, TOP_PVTMON_RESULT_5r,
    TOP_PVTMON_RESULT_6r, TOP_PVTMON_RESULT_7r,
    TOP_PVTMON_RESULT_8r, TOP_PVTMON_RESULT_9r,
    TOP_PVTMON_RESULT_10r, TOP_PVTMON_RESULT_11r,
    TOP_PVTMON_RESULT_12r, TOP_PVTMON_RESULT_13r,
    TOP_PVTMON_RESULT_14r, AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr
};

static int from_soc_attach = 0;

int
soc_tomahawk3_mem_config(int unit)
{
#define _SOC_TH3_DEFIP_MAX_TCAMS    8
#define _SOC_TH3_DEFIP_TCAM_DEPTH   256
#define _SOC_TH3_FIXED_L2_TBL_SZ    (8 * 1024)
#define _SOC_TH3_FIXED_L3_TBL_SZ    (16 * 1024)
    soc_persist_t *sop;
    int l2_entries, l3_entries;
    int num_ipv6_128b_entries = 0;
    int config_v6_entries = 0;
    int defip_config = 0;
    int fpem_entries = 0, shared_fpem_banks = 0;
    int alpm_enable;
    int max_tcams  = _SOC_TH3_DEFIP_MAX_TCAMS;
    int tcam_depth = _SOC_TH3_DEFIP_TCAM_DEPTH;
    int def_ipv6_128b_entries = 0;

    sop = SOC_PERSIST(unit);

    /* TH3 doesn't need TCAM atomicity WAR */
    SOC_CONTROL(unit)->tcam_protect_write = 0;
    SOC_CONTROL(unit)->tcam_protect_write_init = 0;

    /* TH3 Emulation has only 16 entries per TCAM slice */
    tcam_depth =
       soc_property_get(unit, "l3_defip_tcam_depth", _SOC_TH3_DEFIP_TCAM_DEPTH);
    def_ipv6_128b_entries = tcam_depth * max_tcams / 4;

    /* Fetch L2 entries */
    l2_entries =
        soc_property_get(unit, spn_L2_MEM_ENTRIES, _SOC_TH3_FIXED_L2_TBL_SZ);
    if (l2_entries > _SOC_TH3_FIXED_L2_TBL_SZ) {
        LOG_CLI(("ERROR: Unsupported L2 table size specified in config.bcm\n"));
        LOG_CLI(("INFO : L2 table size reduced to 8K \n"));
    }
    l2_entries  = _SOC_TH3_FIXED_L2_TBL_SZ;  /* 8k dedicated L2 entries */

    /* Fetch L3 entries */
    l3_entries =
        soc_property_get(unit, spn_L3_MEM_ENTRIES, _SOC_TH3_FIXED_L3_TBL_SZ);
    if (l3_entries > _SOC_TH3_FIXED_L3_TBL_SZ) {
        LOG_CLI(("ERROR: Unsupported L3 table size specified in config.bcm\n"));
        LOG_CLI(("INFO : L3 table size reduced to 16K \n"));
    }
    l3_entries = _SOC_TH3_FIXED_L3_TBL_SZ;  /* 16k dedicated L3 entries */

    /* Fetch ALPM enable mode */
    alpm_enable = soc_property_get(unit, spn_L3_ALPM_ENABLE, 0);

    /* Fetch FPEM entries
     * Check if config parameter fpem_mem_entries is defined in the
     * file. If not defined then choose default based on ALPM mode
     * requested. If ALPM is requested, then no FPEM as user wants to
     * use 8 bank (full) mode ALPM. If ALPM is not requested then assign
     * max(64K) for FPEM. If fpem_mem_entries is defined then use that
     */
    if (NULL == soc_property_get_str(unit, spn_FPEM_MEM_ENTRIES)) {
        fpem_entries = (alpm_enable) ? 0 : (64 * 1024);
    } else {
        fpem_entries = soc_property_get(unit, spn_FPEM_MEM_ENTRIES, 0);
    }
    /* Find out how many UFT banks are taken up by FPEM */
    shared_fpem_banks = (fpem_entries + (16 * 1024 - 1)) / (16 * 1024);
    fpem_entries = shared_fpem_banks * 16 * 1024;

    /* Max 4 banks from UFT Shared table can be used by FPEM */
    if (shared_fpem_banks > 4) {
        LOG_CLI(("ERROR: Unsupported FPEM_MEM_ENTRIES in config.bcm\n"));
        return SOC_E_PARAM;
    }

    /* Adjust L2 table size */
    sop->memState[L2Xm].index_max = l2_entries - 1;
    sop->memState[L2_ENTRY_ECCm].index_max = l2_entries - 1;
    sop->memState[L2_HITDA_ONLYm].index_max = l2_entries / 4 - 1;
    sop->memState[L2_HITSA_ONLYm].index_max = l2_entries / 4 - 1;

    /* Adjust L3 table size */
    sop->memState[L3_ENTRY_SINGLEm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_ECCm].index_max = l3_entries - 1;
    sop->memState[L3_ENTRY_HIT_ONLYm].index_max = l3_entries / 4 - 1;

    /* Adjust FP exact match table size */
    sop->memState[EXACT_MATCH_2m].index_max = fpem_entries - 1;
    sop->memState[EXACT_MATCH_4m].index_max = fpem_entries / 2 - 1;

    SOC_CONTROL(unit)->l3_defip_max_tcams = max_tcams;
    SOC_CONTROL(unit)->l3_defip_tcam_size = tcam_depth;

    if (soc_feature(unit, soc_feature_alpm)) {
        _soc_alpm_mode[unit] = alpm_enable;
    }
    if (soc_property_get(unit, "l3_defip_sizing", TRUE)) {
        if (!(soc_feature(unit, soc_feature_alpm) && alpm_enable)) {
            defip_config = soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1);

            num_ipv6_128b_entries =
                soc_property_get(unit, spn_NUM_IPV6_LPM_128B_ENTRIES,
                                    (defip_config ? def_ipv6_128b_entries : 0));
            num_ipv6_128b_entries = num_ipv6_128b_entries +
                                    (num_ipv6_128b_entries % 2);

            config_v6_entries = num_ipv6_128b_entries;
            if (soc_property_get(unit, spn_LPM_SCALING_ENABLE, 0)) {
                num_ipv6_128b_entries = 0;
                if (!soc_property_get(unit, spn_LPM_IPV6_128B_RESERVED, 1)) {
                    config_v6_entries =
                        ((config_v6_entries /
                          SOC_CONTROL(unit)->l3_defip_tcam_size) +
                         ((config_v6_entries %
                           SOC_CONTROL(unit)->l3_defip_tcam_size
                          ) ? 1 : 0)
                        ) * SOC_CONTROL(unit)->l3_defip_tcam_size;
                }
            }
            sop->memState[L3_DEFIP_PAIR_LEVEL1m].index_max =
                                              num_ipv6_128b_entries - 1;
            sop->memState[L3_DEFIP_LEVEL1m].index_max =
                                     (SOC_CONTROL(unit)->l3_defip_max_tcams *
                                     SOC_CONTROL(unit)->l3_defip_tcam_size) -
                                     (num_ipv6_128b_entries * 2) - 1;

            SOC_CONTROL(unit)->l3_defip_index_remap = num_ipv6_128b_entries;
        } else {
            if (soc_property_get(unit, spn_IPV6_LPM_128B_ENABLE, 1)) {
                /* slightly different processing for v6-128 */
                num_ipv6_128b_entries = soc_property_get(unit,
                                            spn_NUM_IPV6_LPM_128B_ENTRIES,
                                            def_ipv6_128b_entries);
                num_ipv6_128b_entries = num_ipv6_128b_entries +
                                        (num_ipv6_128b_entries % 2);
                if (soc_tomahawk_alpm_mode_get(unit) == 1 ||
                    soc_tomahawk_alpm_mode_get(unit) == 3) {
                    num_ipv6_128b_entries = (num_ipv6_128b_entries + 3) / 4 * 4;
                }
                config_v6_entries = num_ipv6_128b_entries;
                sop->memState[L3_DEFIP_PAIR_LEVEL1m].index_max =
                                                num_ipv6_128b_entries - 1;
                sop->memState[L3_DEFIP_LEVEL1m].index_max =
                                  (SOC_CONTROL(unit)->l3_defip_max_tcams *
                                  SOC_CONTROL(unit)->l3_defip_tcam_size) -
                                  (num_ipv6_128b_entries * 2) - 1;

                SOC_CONTROL(unit)->l3_defip_index_remap =
                                       (num_ipv6_128b_entries / 2) * 2;
            } else {
                sop->memState[L3_DEFIP_PAIR_LEVEL1m].index_max = -1;
            }
        }
        soc_l3_defip_indexes_init(unit, config_v6_entries);
    }

    if (fpem_entries > 0) {
        num_shared_alpm_banks[unit] = 4;
    } else {
        num_shared_alpm_banks[unit] = 8;
    }

    /* update tbl cfg to latency subsystem for diag purposes */
    soc_th3_tbl_cfg[unit].l2_entries = l2_entries;
    soc_th3_tbl_cfg[unit].fixed_l2_entries = l2_entries;
    soc_th3_tbl_cfg[unit].shared_l2_banks = 0;
    soc_th3_tbl_cfg[unit].l3_entries = l3_entries;
    soc_th3_tbl_cfg[unit].fixed_l3_entries = l3_entries;
    soc_th3_tbl_cfg[unit].shared_l3_banks = 0;
    soc_th3_tbl_cfg[unit].fpem_entries = fpem_entries;
    soc_th3_tbl_cfg[unit].shared_fpem_banks = shared_fpem_banks;
    soc_th3_tbl_cfg[unit].alpm_enable = alpm_enable;
    soc_th3_tbl_cfg[unit].max_tcams = max_tcams;
    soc_th3_tbl_cfg[unit].tcam_depth = tcam_depth;

    if (_soc_th3_lpm_view_map[unit] != NULL) {
        sal_free(_soc_th3_lpm_view_map[unit]);
        _soc_th3_lpm_view_map[unit] = NULL;
    }

    _soc_th3_lpm_view_map[unit] = sal_alloc((sizeof(soc_mem_t) *
                                             SOC_L3_DEFIP_MAX_TCAMS_GET(unit) *
                                             SOC_L3_DEFIP_TCAM_DEPTH_GET(unit)),
                                            "lpm_view_map");
    if (NULL == _soc_th3_lpm_view_map[unit]) {
        return SOC_E_MEMORY;
    }

    sal_memset(_soc_th3_lpm_view_map[unit], INVALIDm,
               sizeof(soc_mem_t) *
               SOC_L3_DEFIP_MAX_TCAMS_GET(unit) *
               SOC_L3_DEFIP_TCAM_DEPTH_GET(unit));

    return SOC_E_NONE;
}

int
soc_tomahawk3_mem_cpu_write_control(int unit, soc_mem_t mem, int copyno,
                                   int enable, int *orig_enable)
{
    soc_reg_t reg;
    soc_field_t field;
    uint32 rval = 0, fval;
    int blk, port, block_info_index;
    uint32 orig_fval, enable_fval, disable_fval;

    enable_fval = 1;
    disable_fval = 0;

    switch (mem) {
    case CLPORT_WC_UCMEM_DATAm:
        reg = CLPORT_WC_UCMEM_CTRLr;
        field = ACCESS_MODEf;
        SOC_BLOCK_ITER(unit, blk, SOC_BLK_CLPORT) {
            port = SOC_BLOCK_PORT(unit, blk);
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
            /* It will use the setting from the last block */
            *orig_enable = soc_reg_field_get(unit, reg, rval, field);
            soc_reg_field_set(unit, reg, &rval, field, enable ? 1 : 0);
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
        }
        return SOC_E_NONE;

    case MMU_MTRO_EGRMETERINGBUCKET_PIPE0m:
    case MMU_MTRO_EGRMETERINGBUCKET_PIPE1m:
    case MMU_MTRO_EGRMETERINGBUCKET_PIPE2m:
    case MMU_MTRO_EGRMETERINGBUCKET_PIPE3m:
    case MMU_MTRO_EGRMETERINGBUCKET_PIPE4m:
    case MMU_MTRO_EGRMETERINGBUCKET_PIPE5m:
    case MMU_MTRO_EGRMETERINGBUCKET_PIPE6m:
    case MMU_MTRO_EGRMETERINGBUCKET_PIPE7m:
    case MMU_MTRO_BUCKET_CPU_L1_PIPE0m:
    case MMU_MTRO_BUCKET_CPU_L1_PIPE1m:
    case MMU_MTRO_BUCKET_CPU_L1_PIPE2m:
    case MMU_MTRO_BUCKET_CPU_L1_PIPE3m:
    case MMU_MTRO_BUCKET_CPU_L1_PIPE4m:
    case MMU_MTRO_BUCKET_CPU_L1_PIPE5m:
    case MMU_MTRO_BUCKET_CPU_L1_PIPE6m:
    case MMU_MTRO_BUCKET_CPU_L1_PIPE7m:
    case MMU_MTRO_BUCKET_L0_PIPE0m:
    case MMU_MTRO_BUCKET_L0_PIPE1m:
    case MMU_MTRO_BUCKET_L0_PIPE2m:
    case MMU_MTRO_BUCKET_L0_PIPE3m:
    case MMU_MTRO_BUCKET_L0_PIPE4m:
    case MMU_MTRO_BUCKET_L0_PIPE5m:
    case MMU_MTRO_BUCKET_L0_PIPE6m:
    case MMU_MTRO_BUCKET_L0_PIPE7m:
        reg = MMU_MTRO_CONFIGr;
        field = REFRESH_DISABLEf;
        enable_fval = 1;
        disable_fval = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        orig_fval = soc_reg_field_get(unit, reg, rval, field);
        fval = enable ? enable_fval : disable_fval;
        *orig_enable = orig_fval == enable_fval;
        if (fval != orig_fval) {
            soc_reg_field_set(unit, reg, &rval, field, fval);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
        return SOC_E_NONE;

    case MMU_WRED_AVG_QSIZE_ITM0m:
    case MMU_WRED_AVG_QSIZE_ITM1m:
    case MMU_WRED_AVG_PORTSP_SIZE_ITM0m:
    case MMU_WRED_AVG_PORTSP_SIZE_ITM1m:
    case MMU_WRED_PORT_SP_DROP_THD_0_ITM0m:
    case MMU_WRED_PORT_SP_DROP_THD_0_ITM1m:
    case MMU_WRED_PORT_SP_DROP_THD_1_ITM0m:
    case MMU_WRED_PORT_SP_DROP_THD_1_ITM1m:
    case MMU_WRED_PORT_SP_DROP_THD_2_ITM0m:
    case MMU_WRED_PORT_SP_DROP_THD_2_ITM1m:
    case MMU_WRED_PORT_SP_DROP_THD_3_ITM0m:
    case MMU_WRED_PORT_SP_DROP_THD_3_ITM1m:
    case MMU_WRED_UC_QUEUE_DROP_THD_ITM0m:
    case MMU_WRED_UC_QUEUE_DROP_THD_ITM1m:
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_0_ITM0m:
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_0_ITM1m:
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_1_ITM0m:
    case MMU_WRED_UC_QUEUE_DROP_THD_MARK_1_ITM1m:
        reg = MMU_WRED_REFRESH_CONTROLr;
        field = REFRESH_DISABLEf;
        enable_fval = 1;
        disable_fval = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        orig_fval = soc_reg_field_get(unit, reg, rval, field);
        fval = enable ? enable_fval : disable_fval;
        *orig_enable = orig_fval == enable_fval;
        if (fval != orig_fval) {
            soc_reg_field_set(unit, reg, &rval, field, fval);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
        return SOC_E_NONE;

    case MMU_QSCH_L2_WEIGHT_MEM_PIPE0m:
    case MMU_QSCH_L2_WEIGHT_MEM_PIPE1m:
    case MMU_QSCH_L2_WEIGHT_MEM_PIPE2m:
    case MMU_QSCH_L2_WEIGHT_MEM_PIPE3m:
    case MMU_QSCH_L2_WEIGHT_MEM_PIPE4m:
    case MMU_QSCH_L2_WEIGHT_MEM_PIPE5m:
    case MMU_QSCH_L2_WEIGHT_MEM_PIPE6m:
    case MMU_QSCH_L2_WEIGHT_MEM_PIPE7m:
        reg = MMU_QSCH_VOQ_FAIRNESS_CONFIGr;
        field = DYNAMIC_WEIGHT_ENABLEf;
        enable_fval = 0;
        disable_fval = 1;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        orig_fval = soc_reg_field_get(unit, reg, rval, field);
        fval = enable ? enable_fval : disable_fval;
        *orig_enable = orig_fval == enable_fval;
        if (fval != orig_fval) {
            soc_reg_field_set(unit, reg, &rval, field, fval);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
        return SOC_E_NONE;

    case MMU_EBMB_CCBE_SLICE_CPU_PIPE0m:
    case MMU_EBMB_CCBE_SLICE_CPU_PIPE1m:
    case MMU_EBMB_CCBE_SLICE_CPU_PIPE2m:
    case MMU_EBMB_CCBE_SLICE_CPU_PIPE3m:
    case MMU_EBMB_CCBE_SLICE_CPU_PIPE4m:
    case MMU_EBMB_CCBE_SLICE_CPU_PIPE5m:
    case MMU_EBMB_CCBE_SLICE_CPU_PIPE6m:
    case MMU_EBMB_CCBE_SLICE_CPU_PIPE7m:
    case MMU_EBMB_PAYLOAD_SLICE0_CPU_PIPE0m:
    case MMU_EBMB_PAYLOAD_SLICE0_CPU_PIPE1m:
    case MMU_EBMB_PAYLOAD_SLICE0_CPU_PIPE2m:
    case MMU_EBMB_PAYLOAD_SLICE0_CPU_PIPE3m:
    case MMU_EBMB_PAYLOAD_SLICE0_CPU_PIPE4m:
    case MMU_EBMB_PAYLOAD_SLICE0_CPU_PIPE5m:
    case MMU_EBMB_PAYLOAD_SLICE0_CPU_PIPE6m:
    case MMU_EBMB_PAYLOAD_SLICE0_CPU_PIPE7m:
    case MMU_EBMB_PAYLOAD_SLICE1_CPU_PIPE0m:
    case MMU_EBMB_PAYLOAD_SLICE1_CPU_PIPE1m:
    case MMU_EBMB_PAYLOAD_SLICE1_CPU_PIPE2m:
    case MMU_EBMB_PAYLOAD_SLICE1_CPU_PIPE3m:
    case MMU_EBMB_PAYLOAD_SLICE1_CPU_PIPE4m:
    case MMU_EBMB_PAYLOAD_SLICE1_CPU_PIPE5m:
    case MMU_EBMB_PAYLOAD_SLICE1_CPU_PIPE6m:
    case MMU_EBMB_PAYLOAD_SLICE1_CPU_PIPE7m:
    case MMU_EBMB_PAYLOAD_SLICE2_CPU_PIPE0m:
    case MMU_EBMB_PAYLOAD_SLICE2_CPU_PIPE1m:
    case MMU_EBMB_PAYLOAD_SLICE2_CPU_PIPE2m:
    case MMU_EBMB_PAYLOAD_SLICE2_CPU_PIPE3m:
    case MMU_EBMB_PAYLOAD_SLICE2_CPU_PIPE4m:
    case MMU_EBMB_PAYLOAD_SLICE2_CPU_PIPE5m:
    case MMU_EBMB_PAYLOAD_SLICE2_CPU_PIPE6m:
    case MMU_EBMB_PAYLOAD_SLICE2_CPU_PIPE7m:
    case MMU_EBMB_PAYLOAD_SLICE3_CPU_PIPE0m:
    case MMU_EBMB_PAYLOAD_SLICE3_CPU_PIPE1m:
    case MMU_EBMB_PAYLOAD_SLICE3_CPU_PIPE2m:
    case MMU_EBMB_PAYLOAD_SLICE3_CPU_PIPE3m:
    case MMU_EBMB_PAYLOAD_SLICE3_CPU_PIPE4m:
    case MMU_EBMB_PAYLOAD_SLICE3_CPU_PIPE5m:
    case MMU_EBMB_PAYLOAD_SLICE3_CPU_PIPE6m:
    case MMU_EBMB_PAYLOAD_SLICE3_CPU_PIPE7m:
    case MMU_EBMB_PAYLOAD_SLICE4_CPU_PIPE0m:
    case MMU_EBMB_PAYLOAD_SLICE4_CPU_PIPE1m:
    case MMU_EBMB_PAYLOAD_SLICE4_CPU_PIPE2m:
    case MMU_EBMB_PAYLOAD_SLICE4_CPU_PIPE3m:
    case MMU_EBMB_PAYLOAD_SLICE4_CPU_PIPE4m:
    case MMU_EBMB_PAYLOAD_SLICE4_CPU_PIPE5m:
    case MMU_EBMB_PAYLOAD_SLICE4_CPU_PIPE6m:
    case MMU_EBMB_PAYLOAD_SLICE4_CPU_PIPE7m:
    case MMU_EBMB_PAYLOAD_SLICE5_CPU_PIPE0m:
    case MMU_EBMB_PAYLOAD_SLICE5_CPU_PIPE1m:
    case MMU_EBMB_PAYLOAD_SLICE5_CPU_PIPE2m:
    case MMU_EBMB_PAYLOAD_SLICE5_CPU_PIPE3m:
    case MMU_EBMB_PAYLOAD_SLICE5_CPU_PIPE4m:
    case MMU_EBMB_PAYLOAD_SLICE5_CPU_PIPE5m:
    case MMU_EBMB_PAYLOAD_SLICE5_CPU_PIPE6m:
    case MMU_EBMB_PAYLOAD_SLICE5_CPU_PIPE7m:
    case MMU_EBMB_PAYLOAD_SLICE6_CPU_PIPE0m:
    case MMU_EBMB_PAYLOAD_SLICE6_CPU_PIPE1m:
    case MMU_EBMB_PAYLOAD_SLICE6_CPU_PIPE2m:
    case MMU_EBMB_PAYLOAD_SLICE6_CPU_PIPE3m:
    case MMU_EBMB_PAYLOAD_SLICE6_CPU_PIPE4m:
    case MMU_EBMB_PAYLOAD_SLICE6_CPU_PIPE5m:
    case MMU_EBMB_PAYLOAD_SLICE6_CPU_PIPE6m:
    case MMU_EBMB_PAYLOAD_SLICE6_CPU_PIPE7m:
    case MMU_EBMB_PAYLOAD_SLICE7_CPU_PIPE0m:
    case MMU_EBMB_PAYLOAD_SLICE7_CPU_PIPE1m:
    case MMU_EBMB_PAYLOAD_SLICE7_CPU_PIPE2m:
    case MMU_EBMB_PAYLOAD_SLICE7_CPU_PIPE3m:
    case MMU_EBMB_PAYLOAD_SLICE7_CPU_PIPE4m:
    case MMU_EBMB_PAYLOAD_SLICE7_CPU_PIPE5m:
    case MMU_EBMB_PAYLOAD_SLICE7_CPU_PIPE6m:
    case MMU_EBMB_PAYLOAD_SLICE7_CPU_PIPE7m:
        reg = MMU_EBPTS_CBMG_VALUEr;
        field = MMU_NULL_SLOT_COUNTER_VALUEf;
        enable_fval = 0;
        disable_fval = 1;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        orig_fval = soc_reg_field_get(unit, reg, rval, field);
        fval = enable ? enable_fval : disable_fval;
        *orig_enable = orig_fval == enable_fval;
        if (fval != orig_fval) {
            soc_reg_field_set(unit, reg, &rval, field, fval);
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
        return SOC_E_NONE;

    default:
        block_info_index = SOC_MEM_BLOCK_ANY(unit, mem);
        if ((SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_XPE) ||
            (SOC_BLOCK_TYPE(unit, block_info_index) == SOC_BLK_MMU_SC)){
            reg = MMU_GCFG_MISCCONFIGr;
            field = REFRESH_ENf;
            enable_fval = 0;
            disable_fval = 1;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            orig_fval = soc_reg_field_get(unit, reg, rval, field);
            fval = enable ? enable_fval : disable_fval;
            *orig_enable = orig_fval == enable_fval;
            if (fval != orig_fval) {
                soc_reg_field_set(unit, reg, &rval, field, fval);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            }
        }
        return _soc_trident2_mem_cpu_write_control(unit, mem, copyno,
                                                   enable, orig_enable);
    }
    return SOC_E_NONE;
}

int
soc_tomahawk3_reg_cpu_write_control(int unit, int enable)
{
    uint32 intfo_dis = 0;

    if (enable) {
        /* Disable HW updates */
        soc_reg_field_set(unit, MMU_INTFO_HW_UPDATE_DISr, &intfo_dis,
                          RQE_SPf, 1);
        soc_reg_field_set(unit, MMU_INTFO_HW_UPDATE_DISr, &intfo_dis,
                          EG_SPf, 1);
        soc_reg_field_set(unit, MMU_INTFO_HW_UPDATE_DISr, &intfo_dis,
                          ING_SPf, 1);
        soc_reg_field_set(unit, MMU_INTFO_HW_UPDATE_DISr, &intfo_dis,
                          CONGST_STf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_MMU_INTFO_HW_UPDATE_DISr(unit, intfo_dis));

    return SOC_E_NONE;
}

uint32 _soc_th3_mmu_port(int unit, int port)
{
    soc_info_t *si = &SOC_INFO(unit);
    return si->port_p2m_mapping[si->port_l2p_mapping[port]];
}

STATIC int
soc_tomahawk3_max_frequency_get(int unit,  uint16 dev_id, uint8 rev_id,
                               int skew_id, int *frequency)
{
    switch (dev_id) {
    case BCM56984_DEVICE_ID:
        *frequency = 925;
        break;
    default:
        *frequency = 1325;
        break;
    }

    return SOC_E_NONE;
}

void
soc_tomahawk3_tsc_map_get(int unit, uint32 *tsc_map)
{
    *tsc_map = 0xffffffff;
}

void
soc_tomahawk3_pipe_map_get(int unit, uint32 *pipe_map)
{
    soc_info_t *si;
    int port;

    si = &SOC_INFO(unit);

    *pipe_map = 0;
    PBMP_LB_ITER(unit, port) {
        *pipe_map |= 1 << si->port_pipe[port];
    }
}

int soc_tomahawk3_itm_valid(int unit, int itm) {
    soc_info_t *si;
    uint32 itm_map;

    si = &SOC_INFO(unit);
    itm_map = si->itm_map;

    if (itm < 0 || itm >= NUM_ITM(unit)) {
        return 0;
    }
    if (itm_map & (1U << itm)) {
        return 1;
    }
    return 0;
}

/* Function:
 *      _soc_tomahawk3_port_flex_init
 * Purpose:
 *      Set flexport configuration parameters from the config file.
 * Parameters:
 *      unit            - (IN) Unit number.
 *      is_any_cap      - (IN) All PM can flex
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_tomahawk3_port_flex_init(int unit, int is_any_cap)
{
    soc_info_t *si;
    int pm, blk_idx, blk_type = 0;
    int flex_en, static_ports, max_ports;

    si = &SOC_INFO(unit);
    SHR_BITCLR_RANGE(si->flexible_pm_bitmap, 0, SOC_MAX_NUM_BLKS);

    /* portmap_x=y:speed:i */
    /* portmap_x=y:speed:cap */
    if (SOC_PBMP_NOT_NULL(SOC_PORT_DISABLED_BITMAP(unit, all)) || is_any_cap) {
        SHR_BITSET_RANGE(si->flexible_pm_bitmap, 1, _TH3_PBLKS_PER_DEV);
    } else {
        /* port_flex_enable */
        for (blk_idx = 0; blk_idx < SOC_MAX_NUM_BLKS; blk_idx++) {
            blk_type = SOC_BLOCK_TYPE(unit, blk_idx);
            if (blk_type == SOC_BLOCK_TYPE_INVALID) {
                break;
            }
            pm = SOC_BLOCK_NUMBER(unit, blk_idx);
            if (blk_type == SOC_BLK_CDPORT) {
                /* port_flex_enable_corex=<0...1> */
                flex_en = soc_property_suffix_num_get_only_suffix(unit,
                    pm, spn_PORT_FLEX_ENABLE, "core", -1);
                /* port_flex_enable{x}=<0...1> */
                /* port_flex_enable=<0...1> */
                if (flex_en == -1) {
                    flex_en = soc_property_phys_port_get(unit,
                        (pm * _TH3_PORTS_PER_PBLK + 1),
                        spn_PORT_FLEX_ENABLE, 0);
                }
                if (flex_en) {
                    SHR_BITSET(si->flexible_pm_bitmap, blk_idx);
                }
            }
        }
    }

    /* TH3's CDPORT blocks start at 65 */
    si->flex_eligible =
        (!SHR_BITNULL_RANGE(si->flexible_pm_bitmap, 65, _TH3_PBLKS_PER_DEV));

    /* port_flex_max_ports */
    memset(&(si->pm_max_ports), 0xff, sizeof(si->pm_max_ports));
    for (blk_idx = 0; blk_idx < SOC_MAX_NUM_BLKS; blk_idx++) {
        blk_type = SOC_BLOCK_TYPE(unit, blk_idx);
        if (blk_type == SOC_BLOCK_TYPE_INVALID) {
            break;
        }
        pm = SOC_BLOCK_NUMBER(unit, blk_idx);
        if (blk_type == SOC_BLK_CDPORT) {
            static_ports = _soc_th3_ports_per_pm_get(unit, pm);
            if (SHR_BITGET(si->flexible_pm_bitmap, blk_idx)) {
                /* port_flex_max_ports_corex=y */
                max_ports = soc_property_suffix_num_get_only_suffix(unit,
                    pm, spn_PORT_FLEX_MAX_PORTS, "core", -1);
                /* port_flex_max_ports{x}=y */
                /* port_flex_max_ports=y */
                if (max_ports == -1) {
                    max_ports = soc_property_phys_port_get(unit,
                        (pm * _TH3_PORTS_PER_PBLK + 1),
                        spn_PORT_FLEX_MAX_PORTS, 8);
                }
                /* validation check */
                if ((max_ports < 0) || (max_ports > 8) || (max_ports < static_ports)) {
                    LOG_CLI((BSL_META_U(unit,
                        "Core %d: Bad port_flex_max_ports %d; static ports %d"),
                        pm, max_ports, static_ports));
                    return SOC_E_CONFIG;
                }
                si->pm_max_ports[blk_idx] = max_ports;
            } else {
                si->pm_max_ports[blk_idx] = static_ports;
            }

        }
    }

    return SOC_E_NONE;
}
/*
 * Function:
 *      _soc_tomahwk3_port_ratio_resolve
 * Purpose:
 *      Derive the modes each of the 4-lane macs (2 macs in TH3) is
 *      operating in.  In previous devices, there was only 1 4-lane
 *      mac, so *mode only carried that information.  However, there
 *      are 2 4-lane macs in each TH3 PM, so the mode needs to be
 *      derived for each of them.  There's an additional bit that
 *      gets set when the PM is in 400G mode so the port will combine
 *      the two macs into one port.
 *
 *      In single port mode, each mac is only supporting 1 port:
 *                                              200G (4 x 50G in PAM-4)
 *                                              100G (4 x 25G in 25G-NRZ)
 *                                              40G  (4 x 10G in 10G-NRZ)
 *
 *      In Dual port mode, each mac supports 2 ports
 *
 *      In Tri port mode, each mac supports 3 ports, but the mac needs
 *      to know which port is taking up 2 lanes, hence
 *      SOC_TH3_PORT_RATIO_TRI_023_2_1_1 and SOC_TH3_PORT_RATIO_TRI_012_1_1_2
 *      where 023 indicates lane 0 is used for one port and lanes 2 and 3 are
 *      used for the others.
 *
 *      In Quad port mode, each mac supports 4 ports.
 *
 *      Examples are below.
 *
 * Parameters:
 *      unit - (IN) Unit number
 *      num_lanes - (IN) an array containing the number of lanes occupied
 *      speed_max - (IN) an array containing the speeds of the ports
 *      mode - (OUT) an encoding of the mac modes where the 4 least
 *                  significant bits are mac0, the next 4 bits are mac1
 *                  and the most significant bit is whether to combine
 *                  both macs into a single 400G port
 *
 * Returns:
 *      Nothing
 *
 * #1   single: 400  -   x  -   x  -   x  SOC_TH3_PORT_RATIO_SINGLE
 *                x  -   x  -   x  -   x
 *
 * #2   single: 200  -   x  -   x  -   x  SOC_TH3_PORT_RATIO_SINGLE
 *              200  -   x  -   x  -   x
 * #3   single: 100  -   x  -   x  -   x  SOC_TH3_PORT_RATIO_SINGLE
 * #4   single:  40  -   x  -   x  -   x  SOC_TH3_PORT_RATIO_SINGLE
 * #5   dual:   100  -   x  -  100 -   x  SOC_TH3_PORT_RATIO_DUAL_1_1
 * #6   dual:    50  -   x  -  50  -   x  SOC_TH3_PORT_RATIO_DUAL_1_1
 * #7   dual:   100  -   x  -  50  -   x  SOC_TH3_PORT_RATIO_DUAL_2_1
 * #8   dual:    50  -   x  -  100 -   x  SOC_TH3_PORT_RATIO_DUAL_1_2
 * #9   tri:    100  -   x  - 50/x - 50/x SOC_TH3_PORT_RATIO_TRI_023_2_1_1
 * #10  tri:     50  -   x  - 25/x - 25/x SOC_TH3_PORT_RATIO_TRI_023_2_1_1
 * #11  tri:    50/x - 50/x -  100 -   x  SOC_TH3_PORT_RATIO_TRI_012_1_1_2
 * #12  tri:    25/x - 25/x -  50  -   x  SOC_TH3_PORT_RATIO_TRI_012_1_1_2
 * #13  quad:   25/x - 25/x - 25/x - 25/x SOC_TH3_PORT_RATIO_QUAD
 * #14  quad:   50/x - 50/x - 50/x - 50/x SOC_TH3_PORT_RATIO_QUAD
 * #15  quad:   10/x - 10/x - 10/x - 10/x SOC_TH3_PORT_RATIO_QUAD
 */
#define _TH3_MACS_PER_PM 2
#define _TH3_LANES_PER_MAC 4

STATIC void
_soc_tomahawk3_port_ratio_resolve(int unit, 
                                  const int * num_lanes, 
                                  const int * speed_max,
                                  int * mode)
{
    int i;

    *mode = 0;
    /* Encode Mac0 in the 4 least significant bits, 0-3, and encode Mac1 in bits 4-7 */
    if (num_lanes[0] == 8) {
        /* 8 lanes means 400G port, so encode as such */
        *mode |= (SOC_TH3_PORT_RATIO_SINGLE | 
                 (SOC_TH3_PORT_RATIO_SINGLE << 4));
        /* also encode that the 400G field should be set */
        *mode |= 1 << 8;
    } else {
        /* Iterate through both macs in the pm (2) */
        for (i = 0; i < _TH3_MACS_PER_PM; i++) {
            /* If the port in the mac uses 4 lanes, it's a single port */
            if (num_lanes[i * _TH3_LANES_PER_MAC] == 4) {
                *mode |= SOC_TH3_PORT_RATIO_SINGLE << (i * 4);
            } else if (num_lanes[i * _TH3_LANES_PER_MAC] == 2 && 
                       num_lanes[(i * _TH3_LANES_PER_MAC) + 2] == 2) {
                /* First Port uses 2 lanes and Second Port uses 2 lanes,
                 * so Dual mode */
                if (speed_max[i * _TH3_LANES_PER_MAC] == 
                        speed_max[(i * _TH3_LANES_PER_MAC) + 2]) {
                    *mode |= SOC_TH3_PORT_RATIO_DUAL_1_1 << (i * 4);
                } else if (speed_max[i * _TH3_LANES_PER_MAC] > 
                        speed_max[(i * _TH3_LANES_PER_MAC) + 2]) {
                    *mode |= SOC_TH3_PORT_RATIO_DUAL_2_1 << (i * 4);
                } else {
                    *mode |= SOC_TH3_PORT_RATIO_DUAL_1_2 << (i * 4);
                }
            } else if (num_lanes[i * _TH3_LANES_PER_MAC] == 2) {
                /* First Port uses 2 lanes */
                if (num_lanes[(i * _TH3_LANES_PER_MAC) + 2] == -1) {
                    /* Second port is not specified, so leave in Dual mode */
                    *mode |= SOC_TH3_PORT_RATIO_DUAL_1_1 << (i * 4);
                } else {
                    /* Second port is 1 lane (2 lanes would have been the 
                     * Dual mode case in the first else if), so tri mode
                     * with the first port occupying 2 lanes */
                    *mode |= SOC_TH3_PORT_RATIO_TRI_023_2_1_1 << (i * 4); 
                }
            } else if (num_lanes[(i * _TH3_LANES_PER_MAC) + 2] == 2) {
                /* Second Port uses 2 lanes */
                if (num_lanes[i * _TH3_LANES_PER_MAC] == -1) {
                    /* If the first port is undefined, leave in dual mode */
                    *mode |= SOC_TH3_PORT_RATIO_DUAL_1_1 << (i * 4);
                } else {
                    /* First port is 1 lane, indicating Tri mode */
                    *mode |= SOC_TH3_PORT_RATIO_TRI_012_1_1_2 << (i * 4);
                }
            } else {
                /* 4 ports in this mac */
                *mode |= SOC_TH3_PORT_RATIO_QUAD << (i * 4);
            }
        }
    }
}

/*
 * Function:
 *      soc_tomahwk3_port_ratio_get
 * Purpose:
 *      Derive the modes of the cdport port macro passed in.  This function
 *      calls _soc_tomahawk3_port_ratio_get after fetching the designated
 *      port macro's lane assignment and port speeds.
 * Parameters:
 *      unit     - (IN) Unit number
 *      initTime - (IN) Whether the device is being initialized
 *      cdport   - (IN) Which port macro we need the mode of (0 - 31)
 *      mode     - (OUT) an encoding of the mac modes where the 4 least
 *                  significant bits are mac0, the next 4 bits are mac1
 *                  and the most significant bit is whether to combine
 *                  both macs into a single 400G port
 *
 * Returns:
 *      Nothing
 */
void
soc_tomahawk3_port_ratio_get(int unit, int initTime, int cdport, int *mode)
{
    soc_info_t *si;
    int port, phy_port_base, lane;
    int num_lanes[_TH3_PORTS_PER_PBLK];
    int speed_max[_TH3_PORTS_PER_PBLK];

    si = &SOC_INFO(unit);
    phy_port_base = 1 + cdport * _TH3_PORTS_PER_PBLK;
    for (lane = 0; lane < _TH3_PORTS_PER_PBLK; lane += 2) {
        port = si->port_p2l_mapping[phy_port_base + lane];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
            speed_max[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
            /* At init time port_speed_max_max contains port init speed_max and
             * at run time during flex port opeartion, init_speed_max
             * contains current port speed_max
             */
            speed_max[lane] = initTime ? si->port_speed_max[port] :
                              si->port_init_speed[port];
        }
    }
    _soc_tomahawk3_port_ratio_resolve(unit, num_lanes, speed_max, mode);
}

/*
 * Function:
 *      soc_tomahwk3_port_ratio_get_schedule_state
 * Purpose:
 *      Derive the modes of the cdport port macro passed in, but use
 *      the num_lanes and speed_max from the passed port_map. This function
 *      calls _soc_tomahawk3_port_ratio_get after fetching the designated
 *      port macro's lane assignment and port speeds.
 * Parameters:
 *      unit     - (IN) Unit number
 *      port_schedule_state - (IN) Pointer containing the num_lanes
 *                                 and speed_max we're to use for the PM,
 *                                 rather than using SOC_INFO
 *      cdport   - (IN) Which port macro we need the mode of (0 - 31)
 *      mode     - (OUT) an encoding of the mac modes where the 4 least
 *                  significant bits are mac0, the next 4 bits are mac1
 *                  and the most significant bit is whether to combine
 *                  both macs into a single 400G port
 *      prev_or_new - (IN) Function looks in in_port_map or out_port_map
 *
 * Returns:
 *      Nothing
 */
void
soc_tomahawk3_port_ratio_get_schedule_state(int unit,
                             soc_port_schedule_state_t *port_schedule_state,
                             int cdport, int *mode,
                             int prev_or_new)
{
    int port, phy_port_base, lane;
    int num_lanes[_TH3_PORTS_PER_PBLK];
    int speed_max[_TH3_PORTS_PER_PBLK];
    soc_port_map_type_t *port_map;

    if (prev_or_new == 1) {
        port_map = &port_schedule_state->in_port_map;
    } else {
        port_map = &port_schedule_state->out_port_map;
    }

    phy_port_base = 1 + cdport * _TH3_PORTS_PER_PBLK;
    for (lane = 0; lane < _TH3_PORTS_PER_PBLK; lane += 2) {
        port = port_map->port_p2l_mapping[phy_port_base + lane];
        if (port == -1 || (port_map->log_port_speed[port]==0) ){
            num_lanes[lane] = -1;
            speed_max[lane] = -1;
        } else {
            num_lanes[lane] = port_map->port_num_lanes[port];
            speed_max[lane] = port_map->log_port_speed[port];
        }
    }
    _soc_tomahawk3_port_ratio_resolve(unit, num_lanes, speed_max, mode);
}

#if 0
STATIC void
_soc_tomahawk3_pipe_bandwidth_get(int unit, int pipe,
                                 int *max_pipe_core_bandwidth,
                                 int *pipe_linerate_bandwidth,
                                 int *pipe_oversub_bandwidth)
{
    soc_info_t *si;
    int port;
    int bandwidth;

    si = &SOC_INFO(unit);

    *max_pipe_core_bandwidth = si->bandwidth / 4;

    *pipe_linerate_bandwidth = 0;
    *pipe_oversub_bandwidth = 0;
    PBMP_PORT_ITER(unit, port) {
        if (si->port_pipe[port] != pipe) {
            continue;
        }
        bandwidth = si->port_speed_max[port] >= 2500 ?
            si->port_speed_max[port] : 2500;
        if (SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
            *pipe_oversub_bandwidth += bandwidth;
        } else {
            *pipe_linerate_bandwidth += bandwidth;
        }
    }
    /* 10G cpu/management, 5G loopback, 10G purge/refresh */
    *pipe_linerate_bandwidth += 25;
}
#endif

int
_soc_th3_port_speed_cap[SOC_MAX_NUM_DEVICES][_TH3_DEV_PORTS_PER_DEV];

STATIC int
_soc_th3_post_mmu_init_update(int unit)
{
    int port;
    soc_info_t *si = &SOC_INFO(unit);

    PBMP_ALL_ITER(unit, port) {
        /* Set init speed to max speed by default */
        si->port_init_speed[port] = si->port_speed_max[port];
        if (_soc_th3_port_speed_cap[unit][port]) {
            /* If cap speed is available then adjust max speed for further use */
            si->port_speed_max[port] = _soc_th3_port_speed_cap[unit][port];
        }
    }
    return SOC_E_NONE;
}

/*
 * FUNCTION : soc_th3_bypass_unused_pm
 * ARGS:
 *         unit    -    IN, unit number
 * Description :
 * Bypass unused port blocks, by programming TOP_TSC_ENABLEr
 * appropriately.
 */
soc_error_t
soc_th3_bypass_unused_pm(int unit)
{
    int port, phy_port, pm, block, bn;
    uint32 rval = 0, orval, xrval, pipe_map;
    uint16 dev_id;
    uint8 rev_id;
    int is_master_pm;
    soc_info_t *si = &SOC_INFO(unit);
    uint8 used_pm_map[_TH3_PBLKS_PER_DEV + 1]; /* #CDPORT + #XLPORT */
    soc_reg_t reg = TOP_TSC_ENABLEr;
    soc_field_t reg_field[_TH3_PBLKS_PER_DEV] = {
        TSC_0_ENABLEf, TSC_1_ENABLEf, TSC_2_ENABLEf, TSC_3_ENABLEf,
        TSC_4_ENABLEf, TSC_5_ENABLEf, TSC_6_ENABLEf, TSC_7_ENABLEf,
        TSC_8_ENABLEf, TSC_9_ENABLEf, TSC_10_ENABLEf, TSC_11_ENABLEf,
        TSC_12_ENABLEf, TSC_13_ENABLEf, TSC_14_ENABLEf, TSC_15_ENABLEf,
        TSC_16_ENABLEf, TSC_17_ENABLEf, TSC_18_ENABLEf, TSC_19_ENABLEf,
        TSC_20_ENABLEf, TSC_21_ENABLEf, TSC_22_ENABLEf, TSC_23_ENABLEf,
        TSC_24_ENABLEf, TSC_25_ENABLEf, TSC_26_ENABLEf, TSC_27_ENABLEf,
        TSC_28_ENABLEf, TSC_29_ENABLEf, TSC_30_ENABLEf, TSC_31_ENABLEf
    };

    sal_memset(used_pm_map, 0, sizeof(used_pm_map));
    for (phy_port = 1; phy_port <= _TH3_PORTS_PER_DEV; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port > 0) {
            pm = si->port_serdes[port];
            if ((-1 != pm) && (0 == used_pm_map[pm])) {
                used_pm_map[pm] = 1;
                si->active_pm_map[pm] = 1;
            }
        }
    }

    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
    orval = rval;

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);

    soc_cm_get_id(unit, &dev_id, &rev_id);

    /* Upper loop bound omits XLPORT block intentionally */
    for (pm = 0; pm < _TH3_PBLKS_PER_DEV; pm++) {

        is_master_pm = 0;

        /* In TH3 A0, the "inner" port macros in a pipe are the master port
         * macros -- they drive the "outer" port macros and act as
         * the refclk master. Do not shut them off
         */
        if (rev_id == BCM56980_A0_REV_ID) {
            if ((pm == 1 || pm == 2 || pm == 5 || pm == 6 ||
              pm == 9 || pm == 10 || pm == 13 || pm == 14 ||
              pm == 17 || pm == 18 || pm == 21 || pm == 22 ||
              pm == 25 || pm == 26 || pm == 29 || pm == 30)) {
                is_master_pm = 1;
            }
        } else {
            if ((pm == 3 || pm == 4 || pm == 11 || pm == 12 ||
                 pm == 19 || pm == 20 || pm == 27 || pm == 28)) {
                is_master_pm = 1;
            }
        }

        if (is_master_pm) {
            /* If the pipe is active, skip turning off the master port macros */
            if (pipe_map & (1 << (pm / _TH3_PBLKS_PER_PIPE))) {
                continue;
            }
        }

        /* If PM is not used, bypass it */
        if (0 == used_pm_map[pm]) {
            soc_reg_field_set(unit, reg, &rval, reg_field[pm], 0);
            phy_port = 1 + (pm * _TH3_PORTS_PER_PBLK);
            block = SOC_PORT_BLOCK(unit, phy_port);
            /* Invalidate port block in soc info */
            if (SOC_BLK_CDPORT == SOC_BLOCK_INFO(unit, block).type) {
                si->block_valid[block] = 0;
            }
        }
    }
    if (rval != orval) {
        /*Calculate different bits number between old and new*/
        xrval = rval ^ orval;
        for (bn = 0; xrval != 0; bn++) {
            xrval &= (xrval - 1);
        }

        /*Do two steps configuration change for the case of different bits number beyond 10*/
        if (bn > 10) {
            /*Change half of those different bits to new at first*/
            bn /= 2;
            xrval = rval ^ orval;
            do {
                xrval &= (xrval-1);
            } while (bn-- > 0);
            xrval ^= rval;
            SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            sal_usleep(100000);
        }
        SOC_IF_ERROR_RETURN(
                soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        sal_usleep(100000);
    }

    return SOC_E_NONE;
}

#if 0
static int
_soc_tomahawk3_phy_dev_port_check(int unit, int phy_port, int dev_port)
{
    int num_phy_port = 136;

    if( phy_port < 0 || phy_port >= num_phy_port) {
        return FALSE;
    } else if ( phy_port >= 1 && phy_port <= 32) {
        return ( dev_port >= 1 && dev_port <=32 );
    } else if ( phy_port >= 33 && phy_port <= 64) {
        return ( dev_port >= 34 && dev_port <= 66 );
    } else if ( phy_port >= 65 && phy_port <= 96) {
        return ( dev_port >= 68 && dev_port <= 100 );
    } else if ( phy_port >= 97 && phy_port <= 128) {
        return ( dev_port >= 102 && dev_port <= 133 );
    } else if ( phy_port == 129) {
        return ( dev_port >= 34 && dev_port <= 66 );
    } else if ( phy_port == 131) {
        return ( dev_port >= 68 && dev_port <= 100 );
    } else {
        /*skip LBPORT(132,133,134,135) , CPU port(0) and hole (130)*/
        return TRUE;
    }

}
#endif

/*
 *Function: soc_tomahawk3_dpr_frequency_range_check
 *Purpose: Check if dpr frequency is in valid range based on the core clock
 *         frequency. Packet Processor can have a maximum of 1000 (1G) or
 *         the core clock frequency, whichever is less
 *Params:
 *      unit: Device unit number
 *      dpr_freq: dpr frequency value
 */
int
soc_tomahawk3_dpr_frequency_range_check(int unit, int dpr_freq)
{
    int dpr_freq_max, dpr_freq_min;
    soc_info_t *si = &SOC_INFO(unit);

    dpr_freq_max = si->frequency;

    /* set maximum to 1000 if si->frequency is greater than 1000 */
    if (dpr_freq_max > 1000) {
        dpr_freq_max = 1000;
    }

    dpr_freq_min = si->frequency / 2;
    if ((dpr_freq < dpr_freq_min) || (dpr_freq > dpr_freq_max) ||
            dpr_freq == -1) {
        LOG_CLI((BSL_META_U(unit,
            "*** Input DPR clock frequency %dMHz is not supported!\n"
            "Valid range is in between %dMHz and %dMHz\n"), dpr_freq,
            dpr_freq_max, dpr_freq_min));
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk3_port_mapping_check(int unit)
{
    soc_info_t *si;
    int port, phy_port, lanes, i;

    si = &SOC_INFO(unit);

    for (phy_port = 1; phy_port < _TH3_PORTS_PER_DEV;) {
        port = si->port_p2l_mapping[phy_port];
        if (port < 0) {
            phy_port ++;
            continue;
        }
        if (port >= _TH3_DEV_PORTS_PER_DEV) {
            LOG_CLI((BSL_META_U(unit, "wrong logical port %d, phy %d\n"),
                     port, phy_port));
            return SOC_E_CONFIG;
        }
        lanes = si->port_num_lanes[port];
        if ((lanes < 0) || (lanes > 8)) {
            LOG_CLI((BSL_META_U(unit, "wrong lane number, port %d, lanes %d\n"),
                     port, lanes));
            return SOC_E_CONFIG;
        }
        for (i = 1; i < lanes; i ++) {
            if (si->port_p2l_mapping[phy_port + i] != -1) {
                LOG_CLI((BSL_META_U(unit, "port overlay, port %d(%d-%d) and "
                                          "port %d(%d..)\n"),
                         port, phy_port, phy_port + lanes - 1,
                         si->port_p2l_mapping[phy_port + i], phy_port + i));
                return SOC_E_CONFIG;
            }
        }
        phy_port += lanes;
    }

    return SOC_E_NONE;
}
/*
 * Function: soc_tomahawk3_find_disabled_pipes
 *
 * Purpose: In the event of half chip or emulation, sometimes TH3 doesn't
 * have all 8 pipes.  This function is meant to mark the ones that do not
 * exist so that calls to soc_tomahawk3_pipe_map_get will be accurate
 * 
 * Parameters:
 *      unit            - Switch unit
 *      disabled_pipes  - Array of pipes indicating enabled or disabled
 *
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
soc_tomahawk3_find_disabled_pipes(int unit, int *disabled_pipes)
{
    uint32 reg_val;
    uint16 dev_id;
    uint8  rev_id;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    /* DMU_PCU_OTP_CONFIG_4.otp_config[7] tells us whether only
     * pipes 0, 1, 4, 5 are active */
    SOC_IF_ERROR_RETURN(READ_DMU_PCU_OTP_CONFIG_4r(unit, &reg_val));
    if (reg_val & 0x80) {
        disabled_pipes[2] = 1;
        disabled_pipes[3] = 1;
        disabled_pipes[6] = 1;
        disabled_pipes[7] = 1;
    } else if (SAL_BOOT_QUICKTURN && SOC_INFO(unit).num_pipe == 4) {
        disabled_pipes[2] = 1;
        disabled_pipes[3] = 1;
        disabled_pipes[4] = 1;
        disabled_pipes[5] = 1;
    } else if (dev_id == BCM56983_DEVICE_ID) {
        disabled_pipes[2] = 1;
        disabled_pipes[3] = 1;
        disabled_pipes[4] = 1;
        disabled_pipes[5] = 1;
    }

    return SOC_E_NONE;
}
/*
 * Tomahawk3 port mapping
 *
 *                   physical   idb/       device   mmu
 *                   port       idb port   port     port
 *     CMIC          0          0/19       0        19
 *     CDPORT0-3     1-32       0/0-17     1-18     0-17
 *     CDPORT4-7     33-64      1/0-17     20-37    39-49
 *     CDPORT8-11    65-96      2/0-17     40-57    64-81
 *     CDPORT12-15   97-128     3/0-17     60-77    96-113
 *     CDPORT16-19   129-160    4/0-17     80-97    128-145
 *     CDPORT20-23   161-192    5/0-17     100-117  160-177
 *     CDPORT24-27   193-224    6/0-17     120-137  192-209
 *     CDPORT28-31   225-256    7/0-17     140-157  224-241
 *     XLPORT0/1     257        1/19       38       51
 *     XLPORT0/5     258        5/19       118      179
 *     LBPORT0       259        0/18       19       18
 *     LBPORT1       260        1/18       39       50
 *     LBPORT2       261        2/18       59       82
 *     LBPORT3       262        3/18       79       114
 *     LBPORT4       263        4/18       99       146
 *     LBPORT5       264        5/18       119      178
 *     LBPORT6       265        6/18       139      210
 *     LBPORT7       266        7/18       159      242
 * Device port number is flexible within the above range
 * Although MMU port number is flexible within the above range, it is
 *     configured as a fixed mapped to physical port number
 */
int
soc_tomahawk3_port_config_init(int unit, uint16 dev_id)
{
#define BCM_TH3_MAX_LOGICAL_PORT_NUM    (160)
    soc_info_t *si;
    char *config_str, *sub_str, *sub_str_end;
    static const char str_2p5[] = "2.5";
    char str_buf[8];
    int rv, oversub_mode, frequency;
    int port, phy_port, mmu_port, idb_port;
    int pipe, itm, index, bandwidth_cap, is_any_cap = FALSE;
    int port_bandwidth, freq_list_len, blk_idx;
    int disabled_pipe[_TH3_PIPES_PER_DEV];
    int dpr_freq;
    int speed_cd, speed_ce, speed_xe;
    uint16 dev_id_temp;
    uint8 rev_id;
    char option1, option2;
    uint32 pipe_map, itm_map, ipipe_map = 0;
    static const int freq_list[] = { 1325, 1250, 1175, 1100, 1000, 925,
                                     850, 775, 675 };
    static const struct {
        int port;
        int phy_port;
        int pipe;
        int idb_port;
        int mmu_port;
    } fixed_ports[] = {
        { 0,   0,   0, 19, 19 },    /* cpu port */
        { 19,  259, 0, 18, 18 },    /* loopback port 0 */
        { 39,  260, 1, 18, 50 },    /* loopback port 1 */
        { 59,  261, 2, 18, 82 },   /* loopback port 2 */
        { 79,  262, 3, 18, 114 },    /* loopback port 3 */
        { 99,  263, 4, 18, 146 },    /* loopback port 4 */
        { 119, 264, 5, 18, 178 },    /* loopback port 5 */
        { 139, 265, 6, 18, 210 },    /* loopback port 6 */
        { 159, 266, 7, 18, 242 },    /* loopback port 7 */
    };
    portmod_speed_config_t speed_config_temp;

    si = &SOC_INFO(unit);


    /* Keep track of the pipes that are disabled */
    sal_memset(disabled_pipe, 0, _TH3_PIPES_PER_DEV * sizeof(int));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_find_disabled_pipes(unit, disabled_pipe));

    /* Can't read TOP register at this point */
    soc_tomahawk3_max_frequency_get(unit, dev_id, -1, -1, &si->frequency);
    frequency = soc_property_get(unit, spn_CORE_CLOCK_FREQUENCY, -1);


    /* Core CLK frequency */
    if (frequency != -1 && frequency != si->frequency) {
        freq_list_len = COUNTOF(freq_list);
        for (index = 0; index < freq_list_len; index++) {
            if (freq_list[index] <= si->frequency &&
                frequency == freq_list[index]) {
                break;
            }
        }

        if (index < freq_list_len) {
            si->frequency = frequency;
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "*** Input core clock frequency %dMHz is not "
                                "supported!\n"), frequency));
        }
    }


    /*DPR frequency check*/
    /*TH3 supports the DPR:clock frequency ratio between 1:2 and 1:1*/
    dpr_freq = soc_property_get(unit, spn_DPR_CLOCK_FREQUENCY, -1);
#if defined(PLISIM)
    if (SAL_BOOT_PLISIM) {
        dpr_freq = 0;
    }
#endif
    if (dpr_freq) {
        /* PCID server is property agnostic  */
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_dpr_frequency_range_check(unit, dpr_freq));
    }

    /* TH3 is only oversubscribed mode */
    oversub_mode = 1;
    si->oversub_mode = oversub_mode;

    for (phy_port = 0; phy_port < 267; phy_port++) {
        si->port_p2l_mapping[phy_port] = -1;
        si->port_p2m_mapping[phy_port] = -1;
    }
    for (port = 0; port < BCM_TH3_MAX_LOGICAL_PORT_NUM; port++) {
        si->port_l2p_mapping[port] = -1;
        si->port_l2i_mapping[port] = -1;
        si->port_speed_max[port] = -1;
        si->port_group[port] = -1;
        si->port_serdes[port] = -1;
        si->port_pipe[port] = -1;
        si->port_num_lanes[port] = -1;
        _soc_th3_port_speed_cap[unit][port] = 0;
    }
    for (mmu_port = 0; mmu_port < _TH3_MMU_PORTS_PER_DEV; mmu_port++) {
        si->port_m2p_mapping[mmu_port] = -1;
    }
    SOC_PBMP_CLEAR(si->eq_pbm);
    SOC_PBMP_CLEAR(si->management_pbm);
    for (pipe = 0; pipe < si->num_pipe; pipe++) {
        SOC_PBMP_CLEAR(si->pipe_pbm[pipe]);
    }
    SOC_PBMP_CLEAR(si->oversub_pbm);
    SOC_PBMP_CLEAR(si->all.disabled_bitmap);

    /* Populate the fixed mapped ports */
    for (index = 0; index < COUNTOF(fixed_ports); index++) {
        port = fixed_ports[index].port;
        phy_port = fixed_ports[index].phy_port;
        pipe = fixed_ports[index].pipe;;
        si->port_l2p_mapping[port] = phy_port;
        si->port_l2i_mapping[port] = fixed_ports[index].idb_port;
        si->port_p2l_mapping[phy_port] = port;
        si->port_p2m_mapping[phy_port] = fixed_ports[index].mmu_port;
        si->port_pipe[port] = pipe;
    }


    rv = SOC_E_NONE;
    for (port = 1; port < BCM_TH3_MAX_LOGICAL_PORT_NUM; port++) {
        if (si->port_l2p_mapping[port] != -1) { /* fixed mapped port */
            continue;
        }
#if 0 
        if (port == 134) { /* Reserved port */
            continue;
        }
#endif
        config_str = soc_property_port_get_str(unit, port, spn_PORTMAP);
        if (config_str == NULL) {
            continue;
        }

        /*
         * portmap_<port>=<physical port number>:<bandwidth in Gb>[:<bandwidth cap in Gb/i(inactive)/1/2/4(num lanes)>][:<i>]
         * eg:    portmap_1=1:100
         *     or portmap_1=1:40
         *     or portmap_1=1:40:i
         *     or portmap_1=1:40:2
         *     or portmap_1=1:40:2:i
         *     or portmap_1=1:10:100
         */
        sub_str = config_str;

        /* Parse physical port number */
        phy_port = sal_ctoi(sub_str, &sub_str_end);
        if (sub_str == sub_str_end) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Missing physical port information "
                                "\"%s\"\n"),
                     port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }
        if (phy_port < 0 || phy_port >= 267) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Invalid physical port number %d\n"),
                     port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }
        /* coverity[check_after_sink : FALSE] */
        if (si->port_p2l_mapping[phy_port] != -1) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Physical port %d is used by port "
                                "%d\n"),
                     port, phy_port, si->port_p2l_mapping[phy_port]));
            rv = SOC_E_FAIL;
            continue;
        }
        /* In order to reuse the mapping between logical and physical port
         * number for the mapping relationship between physical and IDB port
         * number, we have to make maping between IDB and logical port as fixed.
         * For the management ports:
         *  phy_port    IDB port  => Logical port
         *  257         32              66
         *  259         32              100
         */
        if ((phy_port == 257 && port != 38) ||
            (phy_port == 258 && port != 118)) {
            LOG_CLI((BSL_META_U(unit,
                                "Management Port %d: Invalid port number %d\n"),
                     phy_port, port));
            rv = SOC_E_FAIL;
            continue;
        }

        pipe = port/_TH3_DEV_PORTS_PER_PIPE;
        if ((port != 38 && port != 118) &&
            ((phy_port < (pipe * 32 + 1)) ||
            (phy_port > (pipe + 1) * 32))) {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d, Physical port %d: "
                                "Not in same pipeline range\n"),
                     port, phy_port));
            rv = SOC_E_FAIL;
            continue;
        }


        /* Skip ':' between physical port number and bandwidth */
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;
        }

        /* Parse bandwidth */
        for (index = 0; index < sizeof(str_2p5) - 1; index++) {
            if (sub_str[index] == '\0') {
                break;
            }
            str_buf[index] = sub_str[index];
        }
        str_buf[index] = '\0';
        if (!sal_strcmp(str_buf, str_2p5)) {
            port_bandwidth = 2500;
            sub_str_end = &sub_str[sizeof(str_2p5) - 1];
        } else {
            port_bandwidth = sal_ctoi(sub_str, &sub_str_end) * 1000;
            if (sub_str == sub_str_end) {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Missing bandwidth information "
                                    "\"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            switch (port_bandwidth) {
            case 1000:
            case 10000:
            case 25000:
            case 40000:
            case 50000:
            case 100000:
            case 200000:
            case 400000:
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Invalid bandwidth %d Gb\n"),
                         port, port_bandwidth / 1000));
                rv = SOC_E_FAIL;
                continue;
            }
        }

        /* Check if option presents */
        option1 = 0; option2 = 0;
        sub_str = sub_str_end;
        if (*sub_str != '\0') {
            /* Skip ':' between bandwidth and cap or options */
            if (*sub_str != ':') {
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d: Bad config string \"%s\"\n"),
                         port, config_str));
                rv = SOC_E_FAIL;
                continue;
            }
            sub_str++;

            if (*sub_str != '\0') {
                /* Parse bandwidth cap or options */
                bandwidth_cap = sal_ctoi(sub_str, &sub_str_end) * 1000;
                switch (bandwidth_cap) {
                case 10000:
                case 25000:
                case 40000:
                case 50000:
                case 100000:
                case 200000:
                case 400000:
                    sub_str = sub_str_end;
                    _soc_th3_port_speed_cap[unit][port] = bandwidth_cap;
                    /* Flex config detected, port speed cap specified */
                    is_any_cap = TRUE;
                    break;
                default:
                    if (!(*sub_str == 'i' || *sub_str == '1' ||
                          *sub_str == '2' || *sub_str == '4' ||
                          *sub_str == '8')) {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string "
                                            "\"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    } else {
                        option1 = *sub_str;
                        sub_str++;
                    }
                }
                /* Check if more options present */
                if (*sub_str != '\0') {
                    /* Skip ':' between options */
                    if (*sub_str != ':') {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string "
                                            "\"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    }
                    sub_str++;

                    if (*sub_str != 'i') {
                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Bad config string "
                                            "\"%s\"\n"),
                                 port, config_str));
                        rv = SOC_E_FAIL;
                        continue;
                    }
                    option2 = *sub_str;
                    sub_str++;
                }
            }
        }

        /* Check trailing string */
        if (*sub_str != '\0') {
            LOG_CLI((BSL_META_U(unit,
                                "Port %d: Bad config string \"%s\"\n"),
                     port, config_str));
            rv = SOC_E_FAIL;
            continue;
        }

        /* Update soc_info */
        si->port_l2p_mapping[port] = phy_port;
        si->port_p2l_mapping[phy_port] = port;
        si->port_speed_max[port] = port_bandwidth;
        if (option1 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        } else {
            switch (option1) {
            case '1': si->port_num_lanes[port] = 1; break;
            case '2': si->port_num_lanes[port] = 2; break;
            case '4': si->port_num_lanes[port] = 4; break;
            case '8': si->port_num_lanes[port] = 8; break;
            default: break;
            }
        }
        if (option2 == 'i') {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
        }
    }

    speed_cd = 0;
    speed_ce = 0;
    speed_xe = 0;

    /* Setup port_num_lanes and initial port names based on speed.
     * port names is needed because the next section fetches port_fec,
     * and if the user specifies */
    for (port = 0; port < BCM_TH3_MAX_LOGICAL_PORT_NUM; port++) {
        if (si->port_speed_max[port] == 400000) {
            si->port_num_lanes[port] = 8;
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                     "cd%d", speed_cd++);
        } else if (si->port_speed_max[port] == 200000) {
            si->port_num_lanes[port] = 4;
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                     "cd%d", speed_cd++);
        } else if (si->port_speed_max[port] == 100000) {
            if (si->port_num_lanes[port] == -1) {
                /* if it's 100G and the user didn't specify the number
                 * of lanes, choose 2 lanes (2 x 50G) */
                si->port_num_lanes[port] = 2;
            }
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                     "ce%d", speed_ce++);
        } else if (si->port_speed_max[port] == 50000) {
            if (si->port_num_lanes[port] == -1) {
                /* if it's 50G and the user didn't specify the number
                 * of lanes, choose 1 lane (1 x 50G) */
                si->port_num_lanes[port] = 1;
            }
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                     "xe%d", speed_xe++);
        } else if (si->port_speed_max[port] == 40000) {
            if (si->port_num_lanes[port] == -1) {
                si->port_num_lanes[port] = 2;
            }
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                    "xe%d", speed_xe++);
        } else if (si->port_speed_max[port] > 0) {
            if (si->port_num_lanes[port] == -1) {
                /* But RXAUI and XAUI on mgmt ports use 2 and 4 lanes, which
                 * can be provided via the portmap interface.
                 */
                si->port_num_lanes[port] = 1;
            }
            sal_snprintf(si->port_name[port], sizeof(si->port_name[port]),
                "xe%d", speed_xe++);
        }

        /* If the port is mapped find out its fec, link training,
         * lane config from config file */
        if (si->port_speed_max[port] > 0) {
            /* Fetch FEC, Link Training and Phy lane Config from Config File */
            SOC_IF_ERROR_RETURN(soc_esw_portctrl_speed_config_fill(unit, port,
                            si->port_speed_max[port],
                            si->port_num_lanes[port],
                            -1,
                            -1,
                            -1,
                            &speed_config_temp));
            si->port_fec_type[port] = speed_config_temp.fec;
            si->port_link_training[port] = speed_config_temp.link_training;
            si->port_phy_lane_config[port] = speed_config_temp.lane_config;
        }
    }


    /* check portmap */
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_port_mapping_check(unit));

    /* get flex port properties */
    if (SOC_FAILURE(_soc_tomahawk3_port_flex_init(unit, is_any_cap))) {
        rv = SOC_E_FAIL;
    }

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (si->flex_eligible) {
        if (SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(
                soc_th3_flexport_scache_recovery(unit));
        } else {
            SOC_IF_ERROR_RETURN(
                soc_th3_flexport_scache_allocate(unit));
        }
    }
#endif


    /* Setup high speed port (HSP) pbm */
    for (port = 0; port < BCM_TH3_MAX_LOGICAL_PORT_NUM; port++) {
        phy_port = si->port_l2p_mapping[port];
        if (phy_port <= 0 || phy_port > 258) {
            continue;
        }

        pipe = port/_TH3_DEV_PORTS_PER_PIPE;
        if (phy_port == 257) {
            si->port_l2i_mapping[port] = 19;
            si->port_p2m_mapping[phy_port] = 51;
            si->port_serdes[port] = 32;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else if (phy_port == 258) {
            si->port_l2i_mapping[port] = 19;
            si->port_p2m_mapping[phy_port] = 179;
            si->port_serdes[port] = 32;
            SOC_PORT_BLOCK(unit, phy_port) = SOC_PORT_BLOCK(unit, 257);
            SOC_PORT_BINDEX(unit, phy_port) = 2;
            SOC_PBMP_PORT_ADD(si->management_pbm, port);
        } else {
            idb_port = (port % 20) - (pipe == 0 ? 1 : 0);
            mmu_port = pipe * 32 + idb_port;
            si->port_l2i_mapping[port] = idb_port;
            si->port_p2m_mapping[phy_port] = mmu_port;
            si->port_serdes[port] = (phy_port - 1) / 8;
        }

        si->port_pipe[port] = pipe;
        SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);

        if (si->port_speed_max[port] >= 40000) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, port);
        }

        /* Verify speed for front panel port is valid. Skip if warmboot
         * since it's reading from scache. Assume previous coldboot
         * already error checked */
        if (!IS_MANAGEMENT_PORT(unit, port) && !SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(soc_th3_device_speed_check(unit,
                    si->port_speed_max[port],
                    si->port_num_lanes[port]));
        }
    }

    for (index = 1; index < COUNTOF(fixed_ports); index++) {
        pipe = fixed_ports[index].pipe;
        port = fixed_ports[index].port;
        phy_port = fixed_ports[index].phy_port;
        /* If this is a half chip or a SKU with disabled pipes,
         * remove the mappings */
        if (disabled_pipe[pipe] == 1) {
            /* Remove loopback port if the entire pipe does not exist */
            si->port_l2p_mapping[port] = -1;
            si->port_l2i_mapping[port] = -1;
            si->port_p2l_mapping[phy_port] = -1;
            si->port_p2m_mapping[phy_port] = -1;
            si->port_pipe[port] = -1;
        } else {
            SOC_PBMP_PORT_ADD(si->pipe_pbm[pipe], port);
        }
    }

    /* Add CPU Port to PIP_PBM of PIPE 0 */
    SOC_PBMP_PORT_ADD(si->pipe_pbm[0], CMIC_PORT(unit));

    /* Since we have the port speeds and lanes from the config file,
     * calculate the port macro VCOs for later PM init. Skip for WB/Fast reboot
     * We check from_soc_attach because port_config_init is also called during
     * soc_attach, which is PRIOR to when init soc gets the boot flags. Calls
     * down to portmod to fetch the VCO for forced speeds while the port is
     * actually in autoneg will cause VCO fetch errors from portmod, so we
     * skip fetching and updating the local VCOs if we're in warmboot/fast
     * reboot. soc_counter_control is set after the first call, so it's NULL
     * when soc_attach calls this function, but set when do_init calls this
     * function
     */
    if (from_soc_attach == 0) {
        if (!(SOC_WARM_BOOT(unit) || SOC_FAST_REBOOT(unit))){
            SOC_IF_ERROR_RETURN(soc_th3_portctrl_vco_soc_info_update(unit));
        }
    }

    /* Setup pipe/xpe mapping
     * XPE0 (SC_R/XPE_A): IP 0/3 EP 0/1
     * XPE1 (SC_S/XPE_A): IP 0/3 EP 2/3
     * XPE2 (SC_R/XPE_B): IP 1/2 EP 0/1
     * XPE3 (SC_S/XPE_B): IP 1/2 EP 2/3
     */
    pipe_map = 0;
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            continue;
        }
        pipe_map |= 1 << pipe;
    }
    itm_map = 0;
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        ipipe_map = (itm == 0 ? 0xc3 : 0x3c) & pipe_map;
        if (ipipe_map == 0) {
            continue;
        }
        itm_map |= 1 << itm;
        /* Number of ITMs is 2 in Tomahawk3 */
        /* coverity[overrun-local : FALSE] */
        si->itm_ipipe_map[itm] = ipipe_map;
    }
    for (pipe = 0; pipe < _TH3_PIPES_PER_DEV; pipe++) {
        /* Number of pipes is 8 in Tomahawk3 */
        /* coverity[overrun-local : FALSE] */
        if (SOC_PBMP_IS_NULL(si->pipe_pbm[pipe])) {
            continue;
        }
        /* coverity[overrun-local : FALSE] */
        si->ipipe_itm_map[pipe] = ((pipe < 2 || pipe > 5 )? 0x1 : 0x2) ;
    }
    si->itm_map = itm_map;

    /* Setup cdport refclk master block bitmap.
     * In Tomahawk3, the "inner" port macros in a pipe are the master
     * port macros and will drive the "outer" port macros in a pipe from the
     * system board and they will act as the refclk master to drive the other
     * instances.
     */
    SHR_BITCLR_RANGE(si->pm_ref_master_bitmap, 0, SOC_MAX_NUM_BLKS);
    for (blk_idx = 0; blk_idx < SOC_MAX_NUM_BLKS; blk_idx++) {
        int blknum = SOC_BLOCK_NUMBER(unit, blk_idx);
        int blktype = SOC_BLOCK_TYPE(unit, blk_idx);
        if (blktype == SOC_BLOCK_TYPE_INVALID) {
            break;
        }
        if (blktype == SOC_BLK_CDPORT) {
            soc_cm_get_id(unit, &dev_id_temp, &rev_id);

            /* Set Master PM bitmap for flexport purposes depending on whether
             * it's A0 or B0 chip */
            if (rev_id == BCM56980_A0_REV_ID) {
                if ((blknum == 1 || blknum == 2 || blknum == 5 || blknum == 6 ||
                  blknum == 9 ||blknum == 10 || blknum == 13 || blknum == 14 ||
                  blknum == 17 || blknum == 18 || blknum == 21 || blknum == 22 ||
                  blknum == 25 || blknum == 26 || blknum == 29 || blknum == 30)) {
                    SHR_BITSET(si->pm_ref_master_bitmap, blk_idx);
                }
            } else {
                if ((blknum == 3 || blknum == 4 || blknum == 11 || blknum == 12 ||
                  blknum == 19 || blknum == 20 || blknum == 27 || blknum == 28)) {
                    SHR_BITSET(si->pm_ref_master_bitmap, blk_idx);
                }
            }
        }
    }

#undef BCM_TH3_MAX_LOGICAL_PORT_NUM
    return rv;
}

STATIC int _soc_tomahawk3_init_ipep_memory(int unit);

static int
_soc_th3_pp_pll_config(int unit, int freq)
{
    uint32 rval;
    unsigned pp_idx;
    static const soc_pp_pll_param_t pp_pll[] = {
    /* Fref,       Ndiv,   Pdiv, Mdiv, pp_clk,  VCO */
      {110010,      80,     1,    4,    1000,    4},
      {110010,      70,     1,    4,     875,  3.5},
      {110010,      60,     1,    4,     750,    3},
      {110010,      53,     1,    4,   662.5, 2.65},
      {110010,      50,     1,    4,     625,  2.5},
      {110010,      40,     1,    4,     500,  2.0}
    };
    int pp_pll_len;

    pp_pll_len = sizeof(pp_pll) / sizeof(pp_pll[0]);
    for (pp_idx = 0; pp_idx < pp_pll_len; ++pp_idx) {
        if (pp_pll[pp_idx].pp_clk == freq) {
            break;
        }
    }

    if (pp_idx == pp_pll_len) {
        LOG_CLI(("ERROR: Unsupported freq specified in config.bcm\n"));
        return SOC_E_CONFIG;
    }

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_PP_PLL_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_PP_PLL_POST_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* hold all post div channels */
    SOC_IF_ERROR_RETURN(READ_TOP_PP_PLL_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PP_PLL_CTRL_1r, &rval, POST_HOLD_ALLf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PP_PLL_CTRL_1r(unit, rval));

    /* Enable glitchless bypass enable */
    SOC_IF_ERROR_RETURN(READ_TOP_PP_PLL_CTRL_4r(unit, &rval));
    soc_reg_field_set(unit, TOP_PP_PLL_CTRL_4r, &rval, PLL_RESERVEDf, 2);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PP_PLL_CTRL_4r(unit, rval));

    /* Set to normal operation instead of bypassing all post-div channels*/
    SOC_IF_ERROR_RETURN(READ_TOP_PP_PLL_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PP_PLL_CTRL_1r, &rval, CH_BYP_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PP_PLL_CTRL_1r(unit, rval));

    /* Set ndiv_int and mdiv for target freq */
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PP_PLL_CTRL_0r,
                                REG_PORT_ANY, NDIV_INTf,
                                pp_pll[pp_idx].ndiv_int));
   SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PP_PLL_CTRL_3r,
                                REG_PORT_ANY, CH_MDIVf,
                                pp_pll[pp_idx].mdiv));

    /* Deassert PP_PLL_RST_L */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_PP_PLL_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* Wait for 10us */
    sal_usleep(10);

    /* Deassert PP_PLL_POST_RST_L */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_PP_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PP_PLL_CTRL_4r(unit, &rval));
    soc_reg_field_set(unit, TOP_PP_PLL_CTRL_4r, &rval, PLL_RESERVEDf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PP_PLL_CTRL_4r(unit, rval));

    /* Adjusting the clock phases on 2 post div channels. We set the
       channel#1  twice so it is shifted by 2 VCO cycles which is
       180 degrees */

    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PP_PLL_CTRL_3r,
                                REG_PORT_ANY, CH_MDELf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PP_PLL_CTRL_3r,
                                REG_PORT_ANY, CH_MDELf, 2));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PP_PLL_CTRL_3r,
                                REG_PORT_ANY, CH_MDELf, 0));
    SOC_IF_ERROR_RETURN
        (soc_reg_field32_modify(unit, TOP_PP_PLL_CTRL_3r,
                                REG_PORT_ANY, CH_MDELf, 2));

    /* Release all post div channels to normal operation */
    SOC_IF_ERROR_RETURN(READ_TOP_PP_PLL_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PP_PLL_CTRL_1r, &rval, POST_HOLD_ALLf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PP_PLL_CTRL_1r(unit, rval));

    return 0;
}

/*
 * Function:
 *      soc_tomahawk3_core_pll_bypass_prog_pre
 * Purpose:
 *      Performs steps 1-3 of the CORE_PLL Programming sequence, and
 *      is done when TH3 is operating at a different core clock frequency
 * Parameters:
 *      unit     - (IN) Unit number
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_tomahawk3_core_pll_bypass_prog_pre(int unit)
{
    uint32 rval;

    /* 1 */
    SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_CORE_PLL_CTRL_1r, &rval, AUX_POST_BYP_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL_CTRL_1r(unit, rval));

    /* 2: wait 1 us */
    sal_usleep(1);

    /* 3 */
    soc_reg_field_set(unit, TOP_CORE_PLL_CTRL_1r, &rval, SW_RST_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_CORE_PLL_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_CORE_PLL_POST_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));


    /* To 4: Programming of PLL config parameters follows this in calling
     * function */

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_tomahawk3_core_pll_bypass_prog_post
 * Purpose:
 *      Performs steps 5-9 of the CORE_PLL Programming sequence, and
 *      is done when TH3 is operating at a different core clock frequency
 * Parameters:
 *      unit     - (IN) Unit number
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_tomahawk3_core_pll_bypass_prog_post(int unit)
{
    uint32 rval;

    /* From 4: This follows the programming of the PLL config parameters from
     * the calling function */

    /* 5 */
    SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_CORE_PLL_CTRL_1r, &rval, SW_CFG_SELf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL_CTRL_1r(unit, rval));

    /* 6 */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_CORE_PLL_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* 7: wait 10 us */
    sal_usleep(10);

    /* 8 */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_CORE_PLL_POST_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

    /* 9 */
    SOC_IF_ERROR_RETURN(READ_TOP_CORE_PLL_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_CORE_PLL_CTRL_1r, &rval, AUX_POST_BYP_ENf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_CORE_PLL_CTRL_1r(unit, rval));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_tomahawk3_core_pll_config
 * Purpose:
 *      Performs necessary steps to configure the chip for different
 *      core frequencies.  For TH3, only TOP_CORE_PLL_CTRL_0.NDIV_INT
 *      is changed
 * Parameters:
 *      unit     - (IN) Unit number
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_tomahawk3_core_pll_config(int unit, int frequency)
{
#define _SOC_TH3_PLL_CFG_FIELDS  (1)

    soc_field_t fields[_SOC_TH3_PLL_CFG_FIELDS];
    uint32 values[_SOC_TH3_PLL_CFG_FIELDS];

    int ndiv_int = -1;
    int count;

    if (frequency > 0) {
        switch (frequency) {
        case 1325:
            /* Don't do anything if core clock is default */
            break;
        case 1250:
            ndiv_int = 100;
            break;
        case 1175:
            ndiv_int = 94;
            break;
        case 1100:
            ndiv_int = 88;
            break;
        case 1000:
            ndiv_int = 80;
            break;
        case 925:
            ndiv_int = 74;
            break;
        default:
            break;
        }
    } else {
        ndiv_int = soc_property_get(unit, "force_core_pll_ndiv_int", -1);
    }

    count = 0;
    if (ndiv_int != -1) {
        LOG_INFO(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "*** change NDIV_INT to %d\n"), ndiv_int));
        fields[count] = NDIV_INTf;
        values[count] = ndiv_int;
        count++;
    }
    if (count > 0) {
        /* Call this to prepare TH3 for changing the core pll frequency */
        SOC_IF_ERROR_RETURN(_soc_tomahawk3_core_pll_bypass_prog_pre(unit));

        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit, TOP_CORE_PLL_CTRL_0r,
                                     REG_PORT_ANY, count, fields, values));
        /* Call this after changing the core pll frequency to bring the
         * core pll bypass out of reset */
        SOC_IF_ERROR_RETURN(_soc_tomahawk3_core_pll_bypass_prog_post(unit));
    }

    return SOC_E_NONE;
}

int
soc_tomahawk3_chip_reset_fast_reboot(int unit)
{
    uint32 to_usec, rval;
    int parity_enable;

    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /* Toggle IP, EP, and MMU blocks */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* Need to enable SLOT_PIPELINE_ECC_EN before configuring slot pipeline */
    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IPOST_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   SLOT_PIPELINE_ECC_ENf,1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, EPOST_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   SLOT_PIPELINE_ECC_ENf,1));
    }
    /* IP/EP Slot pipeline configuration */
    /* Needs to be configured before accessing registers in IP/EP
       and also before ip/ep memories are initialized
    */

    SOC_IF_ERROR_RETURN(soc_tomahawk3_slot_pipeline_config(unit));

    SOC_IF_ERROR_RETURN(_soc_tomahawk3_init_ipep_memory(unit));

    return SOC_E_NONE;
}

int
soc_tomahawk3_chip_reset(int unit)
{
    uint16 dev_id;
    uint8 rev_id;
    uint32 to_usec, temp_mask;
    uint32 rval, temp_thr;
    uint32 trval;
    uint32 val;
    soc_reg_t reg;
    int index = 0, freq = 0;
    int parity_enable;
    int frequency;

    soc_cm_get_id(unit, &dev_id, &rev_id);
    to_usec = SAL_BOOT_QUICKTURN ? (250 * MILLISECOND_USEC) :
                                   (10 * MILLISECOND_USEC);

    /*
     * SBUS ring and block number:
     * ring 0: OTPC(6,7), AVS(87), TOP(8)
     * ring 1: IP(1)
     * ring 2: EP(2)
     * ring 3: MMU_ITM(3), MMU_EB(4), MMU_GLB(5)
     * ring 4: PM0(16), PM1(17), ... , PM14(30), PM15(31)
     * ring 5: PM16(32), PM17(33), ... , PM30(46), PM31(47)
     * ring 6: CEV(88)
     * ring 7: PM_MGMT(12)
     */
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_0_7_OFFSET,0x00333210);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_8_15_OFFSET,0x00070000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_16_23_OFFSET,0x44444444);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_24_31_OFFSET,0x44444444);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_32_39_OFFSET,0x55555555);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_40_47_OFFSET,0x55555555);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_48_55_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_56_63_OFFSET,0x00005004);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_64_71_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_72_79_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_80_87_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_88_95_OFFSET,0x00000006);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_96_103_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_104_111_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_112_119_OFFSET,0x00000000);
        soc_pci_write(unit, CMIC_TOP_SBUS_RING_MAP_120_127_OFFSET,0x00000000);
        /*if (!SAL_BOOT_QUICKTURN) {*/
        soc_pci_write(unit, CMIC_TOP_SBUS_TIMEOUT_OFFSET,0x2700);
        /*}*/
    }
#endif
    sal_usleep(to_usec);


    if (!SAL_BOOT_SIMULATION) {
        if (soc_property_get(unit, "force_core_pll", 0)) {
            frequency = soc_property_get(unit, "force_core_pll_freq", -1);
        } else {
            frequency = SOC_INFO(unit).frequency;
        }
        SOC_IF_ERROR_RETURN(_soc_tomahawk3_core_pll_config(unit, frequency));
        sal_usleep(to_usec);
    }

    /* Set the PP_PLL */
    if (!SAL_BOOT_SIMULATION) {
        freq = soc_property_get(unit, spn_DPR_CLOCK_FREQUENCY, 1000);
        SOC_IF_ERROR_RETURN(_soc_th3_pp_pll_config(unit, freq));
    }


    /* PLLs are not required in simulation environments */
    if (!SAL_BOOT_SIMULATION) {
        /* Configure TS PLL */
        
#if 0
        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_1r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_PDIV, 2));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_2r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_2r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_TS_PLL_MNDIV, 5));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_0r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_0r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_TS_PLL_N, 100));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_TS_PLL_CTRL_1r(unit,&rval));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_1r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_TS_KA, 10));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_1r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_TS_KI, 3));
        soc_reg_field_set(unit, TOP_TS_PLL_CTRL_1r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_TS_KP, 10));
        SOC_IF_ERROR_RETURN(WRITE_TOP_TS_PLL_CTRL_1r(unit, rval));
#endif
        /* Set 250Mhz (implies 4ns resolution) default timesync clock to
           calculate assymentric delays */
        SOC_TIMESYNC_PLL_CLOCK_NS(unit) = (1/250 * 1000); /* clock period in nsec */
#if 0
        /* Configure BS PLL0 */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, PDIVf,
                      soc_property_get(unit, spn_PTP_BS_PDIV, 2));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_2r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_BS_MNDIV, 125));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_0r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_0r, &rval, NDIV_INTf,
                      soc_property_get(unit, spn_PTP_BS_NDIV_INT, 100));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL0_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_BS_KA, 10));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_BS_KI, 3));
        soc_reg_field_set(unit, TOP_BS_PLL0_CTRL_1r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_BS_KP, 10));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL0_CTRL_1r(unit, rval));

        /* Configure BS PLL1 */
        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, PDIVf,
                          soc_property_get(unit, spn_PTP_BS_PDIV, 2));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_2r, &rval, CH0_MDIVf,
                          soc_property_get(unit, spn_PTP_BS_MNDIV, 125));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_0r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_0r, &rval, NDIV_INTf,
                          soc_property_get(unit, spn_PTP_BS_NDIV_INT, 100));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_0r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_TOP_BS_PLL1_CTRL_1r(unit, &rval));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, KAf,
                          soc_property_get(unit, spn_PTP_BS_KA, 10));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, KIf,
                          soc_property_get(unit, spn_PTP_BS_KI, 3));
        soc_reg_field_set(unit, TOP_BS_PLL1_CTRL_1r, &rval, KPf,
                          soc_property_get(unit, spn_PTP_BS_KP, 10));
        SOC_IF_ERROR_RETURN(WRITE_TOP_BS_PLL1_CTRL_1r(unit, rval));
#endif

        /* Bring LCPLL, time sync, BroadSync, IPROC, PP, CORE PLLs
           and AVS out of reset */
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_TS_PLL_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL0_RST_Lf,
                          1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_BS_PLL1_RST_Lf,
                          1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval, TOP_AVS_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_IPROC_PLL_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_PP_PLL_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_CORE_PLL_RST_Lf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));

        SOC_IF_ERROR_RETURN(READ_IDM_GENRES_S0_IDM_RESET_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, IDM_GENRES_S0_IDM_RESET_CONTROLr, &rval, RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_IDM_GENRES_S0_IDM_RESET_CONTROLr(unit, rval));
        
        SOC_IF_ERROR_RETURN(READ_IDM_GENRES_S0_IDM_RESET_CONTROLr(unit, &rval));
        soc_reg_field_set(unit, IDM_GENRES_S0_IDM_RESET_CONTROLr, &rval, RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_IDM_GENRES_S0_IDM_RESET_CONTROLr(unit, rval));




        sal_usleep(to_usec);

        /* Check LCPLL lock status */
        
#if 0
        reg = TOP_XG_PLL0_STATUSr;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, TOP_XGPLL_LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                 "LCPLL %d not locked on unit %d "
                                 "status = 0x%08x\n"), index, unit, rval));
        }
#endif

        /* Check time sync lock status */
        reg = TOP_TS_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TS_PLL not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }
        /* Check BroadSync lock status */
        reg = TOP_BS_PLL0_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BS_PLL0 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }

        reg = TOP_BS_PLL1_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BS_PLL0 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }

        /* Check PP lock status */
        reg = TOP_PP_PLL_STATUSr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (!soc_reg_field_get(unit, reg, rval, LOCKf)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "BS_PLL0 not locked on unit %d "
                                  "status = 0x%08x\n"), unit, rval));
        }

        /* De-assert LCPLL's post reset */
        SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REG_2r(unit, &rval));
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_TS_PLL_POST_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_BS_PLL0_POST_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_BS_PLL1_POST_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_IPROC_PLL_POST_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_PP_PLL_POST_RST_Lf, 1);
        soc_reg_field_set(unit, TOP_SOFT_RESET_REG_2r, &rval,
                          TOP_CORE_PLL_POST_RST_Lf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_2r(unit, rval));
    }

    sal_usleep(to_usec);

    /* Check the the sticky bit whether HW PVTMON High Temperature Protection
    * happened in system's previous running or not. Halt the current bootup, print error
    * message, and ask for a system reboot if the sticky bit was set. */

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_RESULT_0r(unit, &rval));
    if (soc_reg_field_get(unit, TOP_PVTMON_RESULT_0r,
                          rval, PVTMON_HIGHTEMP_STATUSf)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "HW PVTMON High Temperature Protection "
                              "was invoked during last system run. "
                              "Please correct the temperature problem "
                              "and cycle power to continue.\n")));

            /* Clear HIGHTEMP sticky bit on all PVTMONs */
            SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r(unit, &rval));
            soc_reg_field_set(unit, TOP_MISC_CONTROL_1r, &rval, PVTMON_HIGHTEMP_STAT_CLEARf, 1);
            SOC_IF_ERROR_RETURN(WRITE_TOP_MISC_CONTROL_1r(unit, rval));

        return SOC_E_FAIL;
    }


    /* Set correct value for BG_ADJ */
    /* TH2 leaves it to be default
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_CTRL_0r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_CTRL_0r, &rval, BG_ADJf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_CTRL_0r(unit, rval));
    */

    /* Enable high temperature interrupt monitoring.
     */
    temp_mask = soc_property_get(unit, "temp_monitor_select", ((1 << 16)-1));
    /* The above is a 9 bit mask to indicate which temp sensor to hook up to
     * the interrupt.
     */
    val = 0;
    for (index = 0; index <COUNTOF(temp_thr_reg)-1; index++) {
        /* AVS excluded */

        trval = 0;
        /* Temp = 434.10 - o_ADC_data * 0.53504
         * data = (434.10 - temp)/0.53504 = (434100-(temp*1000))/535
         * Note: Since this is a high temp value we can safely assume it to
         * always be a +ive number. This is in degrees celcius.
         */
        temp_thr =
            soc_property_get(unit, temp_thr_prop[index], 
                ((index < 5) || (index == 6) || (index == 10)) ? \
                _SOC_TH3_HW_TEMP_MAX : _SOC_TH3_SW_TEMP_MAX);
        if (temp_thr > _SOC_TH3_HW_TEMP_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                    "Unsupported temp value %d !!. "
                                    "Max %d. Using %d.\n"),
                        temp_thr, _SOC_TH3_HW_TEMP_MAX,  _SOC_TH3_HW_TEMP_MAX));
            temp_thr = _SOC_TH3_HW_TEMP_MAX;
        }

        /* Convert temperature to config data */
        temp_thr = (434100-(temp_thr*1000))/535;

        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_thr_reg[index], REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_thr_reg[index], &trval, MIN_THRESHOLDf,
                          temp_thr);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_thr_reg[index], REG_PORT_ANY, 0, trval));
        if (temp_mask & (1 << index)) {
            val |= (1 << ((index * 2) + 1)); /* 2 bits per pvtmon, using min */
        }
    }
    _soc_th_temp_mon_mask[unit] = temp_mask;
    /* SDK-215193 SW WAR for abnormal small pulses from max_temp_clk output pin */
    rval = 0;
    soc_reg_field_set(unit, AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr, &rval,
                      SEQ_MASK_PVT_MNTRf, 0x7e);
    SOC_IF_ERROR_RETURN(WRITE_AVS_REG_HW_MNTR_SEQUENCER_MASK_PVT_MNTRr(unit, rval));

    /* Enable high temperature interrupt monitoring excluding 15 (AVS), 10, 6, 4, 3, 2, 1, 0 . */
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_INTR_MASK_0r(unit, val & 0x3fcfcc00));

    /* Enable PVTMON interrupt in CMIC */
    SOC_IF_ERROR_RETURN(soc_iproc_getreg(unit,
                        soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                        REG_PORT_ANY, 0), &rval));
    rval |= 0x4;
    SOC_IF_ERROR_RETURN(soc_iproc_setreg(unit,
                        soc_reg_addr(unit, ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                        REG_PORT_ANY, 0), rval));


    /* Bring the Temp monitor minimum value out of reset */
    for (index = 0; index <COUNTOF(temp_pvtmon_ctrl_reg); index++) {

        trval = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, temp_pvtmon_ctrl_reg[index].pvtmon_reg,
                           REG_PORT_ANY, 0, &trval));
        soc_reg_field_set(unit, temp_pvtmon_ctrl_reg[index].pvtmon_reg, &trval,
                          temp_pvtmon_ctrl_reg[index].pvtmon_field, 1);
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, temp_pvtmon_ctrl_reg[index].pvtmon_reg,
                           REG_PORT_ANY, 0, trval));
    }

    /* Enable HW PVTMON high temperature protection by setting
       hightemp_ctrl_en field for pvtmon 0-4, 6 and 10 */
    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_0_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_0_CTRL_1r, &rval, PVTMON_HIGHTEMP_CTRL_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_0_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_1_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_1_CTRL_1r, &rval, PVTMON_HIGHTEMP_CTRL_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_1_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_2_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_2_CTRL_1r, &rval, PVTMON_HIGHTEMP_CTRL_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_2_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_3_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_3_CTRL_1r, &rval, PVTMON_HIGHTEMP_CTRL_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_3_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_4_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_4_CTRL_1r, &rval, PVTMON_HIGHTEMP_CTRL_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_4_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_TMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_6_PVTMON_HIGHTEMP_CTRL_ENf, 1);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_10_PVTMON_HIGHTEMP_CTRL_ENf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TMON_CTRL_1r(unit, rval));

    /* Bring port blocks out of reset */
    rval = 0;
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_PM_MGMT_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_TS_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REG_3r(unit, 0xffffffff));
    sal_usleep(to_usec);

    /* Need set PSG/PCG to 0 before de-assert IP/EP/MMU SOFT reset  */
    SOC_IF_ERROR_RETURN(WRITE_TOP_CLOCKING_ENFORCE_PSGr(unit, 0));
    SOC_IF_ERROR_RETURN(WRITE_TOP_CLOCKING_ENFORCE_PCGr(unit, 0));

    /* Bring IP, EP, and MMU blocks out of reset */
    SOC_IF_ERROR_RETURN(READ_TOP_SOFT_RESET_REGr(unit, &rval));
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_IP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_EP_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_SOFT_RESET_REGr, &rval, TOP_MMU_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_SOFT_RESET_REGr(unit, rval));
    sal_usleep(to_usec);

    /* Need to enable SLOT_PIPELINE_ECC_EN before configuring slot pipeline */
    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    if (parity_enable) {
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, IPOST_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   SLOT_PIPELINE_ECC_ENf,1));
        SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit, EPOST_SER_CONTROLr,
                                                   REG_PORT_ANY,
                                                   SLOT_PIPELINE_ECC_ENf,1));
    }
    /* IP/EP Slot pipeline configuration */
    /* Needs to be configured before accessing registers in IP/EP
       and also before ip/ep memories are initialized
    */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_slot_pipeline_config(unit));

    SOC_IF_ERROR_RETURN(_soc_tomahawk3_init_ipep_memory(unit));

    /* Bypass unused Port Blocks */
    if (soc_property_get(unit, "disable_unused_port_blocks", TRUE)) {
        SOC_IF_ERROR_RETURN(soc_th3_bypass_unused_pm(unit));
    }

    return SOC_E_NONE;
}

STATIC
int soc_tomahawk3_cdport_reset(int unit)
{
    int phy_ports[2];
    int sbus_bcast_blocks[2];

    SOC_IF_ERROR_RETURN(soc_th3_portctrl_sbus_broadcast_setup(unit));

    phy_ports[0] = 1;
    phy_ports[1] = 129;

    sbus_bcast_blocks[0] = _TH3_SBUS_BCAST_BLOCK_SBUS4;
    sbus_bcast_blocks[1] = _TH3_SBUS_BCAST_BLOCK_SBUS5;

    SOC_IF_ERROR_RETURN(soc_tsc_xgxs_sbus_broadcast_reset(unit, CDPORT_XGXS0_CTRL_REGr,
                    NULL, phy_ports, sbus_bcast_blocks, 2));

    return SOC_E_NONE;
}

int
soc_tomahawk3_port_reset(int unit)
{
    uint32 rval;
    int blk, port;

    /* Instead of using the generic TSC init, TH3 needs to be aware
     * of master port macros that need to be initialized so that the clocks
     * of the slaves will come up. This is why this function is needed
     * rather than calling soc_tsc_xgxs_reset */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_cdport_reset(unit));

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);
        SOC_IF_ERROR_RETURN(soc_tsc_xgxs_reset(unit, port, 0));
    }


    SOC_BLOCK_ITER(unit, blk, SOC_BLK_CDPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        SOC_IF_ERROR_RETURN(READ_CDPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &rval, CDMAC0_RESETf, 1);
        soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &rval, CDMAC1_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_CDPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &rval, CDMAC0_RESETf, 0);
        soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &rval, CDMAC1_RESETf, 0);
#if 0
        /* removed from latest regsfile */
        soc_reg_field_set(unit, CDPORT_MAC_CONTROLr, &rval, SYS_64B_INTF_MODEf, 1);
#endif

        SOC_IF_ERROR_RETURN(WRITE_CDPORT_MAC_CONTROLr(unit, port, rval));
    }

    SOC_BLOCK_ITER(unit, blk, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, blk);

        SOC_IF_ERROR_RETURN(READ_XLPORT_MAC_CONTROLr(unit, port, &rval));
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 1);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
        sal_udelay(10);
        soc_reg_field_set(unit, XLPORT_MAC_CONTROLr, &rval, XMAC0_RESETf, 0);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MAC_CONTROLr(unit, port, rval));
    }

    return SOC_E_NONE;
}


int
soc_tomahawk3_port_speed_update(int unit, soc_port_t port, int speed)
{
    uint32 rval, fval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_info_t *si = &SOC_INFO(unit);
    int rv;

    /*
     * No reconfiguration is needed if new speed
     * matches current speed
     */
    if (speed == si->port_init_speed[port]) {
        return SOC_E_NONE;
    }

    rv = soc_th_port_asf_mode_set(unit, port, speed,
                                  _SOC_TH_ASF_MODE_CFG_UPDATE);
    if ((SOC_E_NONE != rv) && (SOC_E_UNAVAIL != rv) && (SOC_E_PARAM != rv)) {
        return rv;
    }

    /* OS related updates */
    if (SOC_PBMP_NOT_NULL(SOC_INFO(unit).oversub_pbm)) {
        sal_memset(entry, 0, sizeof(egr_xmit_start_count_entry_t));
        fval = (speed * 11875)/100000;
        soc_mem_field32_set(unit, EDB_1DBG_Bm, &entry, FIELD_Bf, fval);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EDB_1DBG_Bm, MEM_BLOCK_ALL,
                           SOC_INFO(unit).port_l2p_mapping[port], entry));

        rval = 0;
        if (speed >= 100000) {
            fval = 140;
        } else if (speed >= 40000) {
            fval = 60;
        } else if (speed >= 25000) {
            fval = 40;
        } else if (speed >= 20000) {
            fval = 30;
        } else {
            fval = 15;
        }
        soc_reg_field_set(unit, MMU_3DBG_Cr, &rval, FIELD_Af,
                          fval + sal_rand() % 20);
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_sc_reg32_set(unit, MMU_3DBG_Cr, -1, port, 0, rval));
    }
    if (!SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
        soc_th_port_lanes_t lanes_ctrl;

        sal_memset(&lanes_ctrl, 0, sizeof(lanes_ctrl));
        lanes_ctrl.port_base = port;
        lanes_ctrl.lanes = TH_FLEXPORT_SPEED_SET_NUM_LANES;
        lanes_ctrl.speed = speed;
        SOC_IF_ERROR_RETURN(soc_th3_speed_set_init_ctrl(unit, &lanes_ctrl));
        SOC_IF_ERROR_RETURN
            (soc_tomahawk_port_lanes_set(unit, &lanes_ctrl));
    }

    return SOC_E_NONE;
}

int soc_th3_get_alpm_banks(int unit)
{
    int rv = SOC_E_NONE;
    uint32 bank_config;
    uint32 hash_control_entry[SOC_MAX_MEM_WORDS];

    if (num_shared_alpm_banks[unit] == 0) {
        rv = READ_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ANY, 0, hash_control_entry);
        if (SOC_FAILURE(rv)) {
            num_shared_alpm_banks[unit] = 8;
        } else {
            bank_config = soc_mem_field32_get(unit, EXACT_MATCH_HASH_CONTROLm,
                                  hash_control_entry, HASH_TABLE_BANK_CONFIGf);
            if (bank_config & 0xF) { /* EXACT MATCH used UFT banks */
                num_shared_alpm_banks[unit] = 4; /* TH3 ALPM uses only 4 or 8 UFT banks */
            } else {
                num_shared_alpm_banks[unit] = 8;
            }
        }
    }

    return num_shared_alpm_banks[unit];
}

int
soc_tomahawk3_alpm_mode_get(int unit)
{
    return _soc_alpm_mode[unit];
}

void
_soc_tomahawk3_alpm_bkt_view_set(int unit, int index, soc_mem_t view)
{
    int bkt = (index >> (soc_th3_get_alpm_banks(unit) + 1) / 2) & SOC_TH_ALPM_BKT_MASK(unit);

    if (view != INVALIDm) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Unit:%d ALPM bkt set index:%d bkt:%d view:%s\n"),
                                unit, index, bkt, SOC_MEM_NAME(unit, view)));
    }
}

soc_mem_t
_soc_tomahawk3_alpm_bkt_view_get(int unit, int index)
{
    soc_mem_t view = L3_DEFIP_ALPM_LEVEL3m;
    int bkt = (index >> (soc_th3_get_alpm_banks(unit) + 1) / 2) & SOC_TH_ALPM_BKT_MASK(unit);

    if (view != INVALIDm) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Unit:%d ALPM bkt get index:%d bkt:%d view:%s\n"),
                                unit, index, bkt, SOC_MEM_NAME(unit, view)));
    }
    return view;
}

int soc_tomahawk3_mem_basetype_get(int unit, soc_mem_t mem)
{
    int base_type;

    base_type = ((SOC_MEM_INFO(unit, mem).base) >> 23) & 0x7;
    return base_type;
}

/* Function to Clear all Memories.
 * Param
 *   mmu_init - bool val to indicate whether to initialize mmu or not.
 */
STATIC int
_soc_tomahawk3_init_ipep_memory(int unit)
{
    uint32 pipe_map;
    soc_reg_t reg;
    int  pipe;
    uint32 rval, in_progress;
    int pipe_init_usec;
    soc_timeout_t to;
    l2_entry_hash_control_entry_t l2_entry_hash_control;
    l3_entry_hash_control_entry_t l3_entry_hash_control;
    tunnel_hash_control_entry_t tunnel_hash_control;
    mpls_entry_hash_control_entry_t mpls_entry_hash_control;
    exact_match_hash_control_entry_t exact_match_hash_control;

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);

    sal_memset(&l2_entry_hash_control, 0, sizeof(l2_entry_hash_control_entry_t));
    sal_memset(&l3_entry_hash_control, 0, sizeof(l3_entry_hash_control_entry_t));
    sal_memset(&tunnel_hash_control, 0, sizeof(tunnel_hash_control_entry_t));
    sal_memset(&mpls_entry_hash_control, 0, sizeof(mpls_entry_hash_control_entry_t));
    sal_memset(&exact_match_hash_control, 0, sizeof(exact_match_hash_control_entry_t));

    /* In TH3, some of the IPEP memories are hash memories and we need special settings to clear
       the hash memories. The settings include the configuring the reset_val of the fields in the
       respective hash_control memories of the hash tables. Refer TH3-3821
    */
    soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm, &l2_entry_hash_control, ROBUST_HASH_ENf, 0x1);
    soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm, &l2_entry_hash_control, HASH_OFFSET_DEDICATED_BANK_1f, 0x20);
    soc_mem_field32_set(unit, L2_ENTRY_HASH_CONTROLm, &l2_entry_hash_control, HASH_TABLE_BANK_CONFIGf, 0x3);
    SOC_IF_ERROR_RETURN
        (WRITE_L2_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &l2_entry_hash_control));

    soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, &l3_entry_hash_control, ROBUST_HASH_ENf, 0x1);
    soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, &l3_entry_hash_control, HASH_OFFSET_DEDICATED_BANK_1f, 0x20);
    soc_mem_field32_set(unit, L3_ENTRY_HASH_CONTROLm, &l3_entry_hash_control, HASH_TABLE_BANK_CONFIGf, 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_L3_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &l3_entry_hash_control));

    soc_mem_field32_set(unit, L3_TUNNEL_HASH_CONTROLm, &tunnel_hash_control, ROBUST_HASH_ENf, 0x1);
    soc_mem_field32_set(unit, L3_TUNNEL_HASH_CONTROLm, &tunnel_hash_control, HASH_OFFSET_DEDICATED_BANK_1f, 0x20);
    soc_mem_field32_set(unit, L3_TUNNEL_HASH_CONTROLm, &tunnel_hash_control, HASH_TABLE_BANK_CONFIGf, 0xf);
    SOC_IF_ERROR_RETURN
        (WRITE_L3_TUNNEL_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &tunnel_hash_control));

    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, &mpls_entry_hash_control, ROBUST_HASH_ENf, 0x1);
    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, &mpls_entry_hash_control, HASH_OFFSET_DEDICATED_BANK_1f, 0x20);
    soc_mem_field32_set(unit, MPLS_ENTRY_HASH_CONTROLm, &mpls_entry_hash_control, HASH_TABLE_BANK_CONFIGf, 0x3);
    SOC_IF_ERROR_RETURN
        (WRITE_MPLS_ENTRY_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &mpls_entry_hash_control));

    soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm, &exact_match_hash_control, ROBUST_HASH_ENf, 0x1);
    soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm, &exact_match_hash_control, LOGICAL_BANK_1_PHYSICAL_BANK_LOCATIONf, 0x1);
    soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm, &exact_match_hash_control, LOGICAL_BANK_2_PHYSICAL_BANK_LOCATIONf, 0x2);
    soc_mem_field32_set(unit, EXACT_MATCH_HASH_CONTROLm, &exact_match_hash_control, LOGICAL_BANK_3_PHYSICAL_BANK_LOCATIONf, 0x3);
    SOC_IF_ERROR_RETURN
        (WRITE_EXACT_MATCH_HASH_CONTROLm(unit, MEM_BLOCK_ALL, 0,
                                             &exact_match_hash_control));

	/* Initial IPIPE memory */
    rval = 0;
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, ING_HW_RESET_CONTROL_2r, &rval, COUNTf, 0x20000);
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));

    /* Initial DLB memory */
    rval = 0;
    soc_reg_field_set(unit, DLB_ECMP_HW_RESET_CONTROLr, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, DLB_ECMP_HW_RESET_CONTROLr, &rval, VALIDf, 1);
    soc_reg_field_set(unit, DLB_ECMP_HW_RESET_CONTROLr, &rval, COUNTf, 0x1000);
    SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_HW_RESET_CONTROLr(unit, rval));

    /* Initial EPIPE memory */
    rval = 0;
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, VALIDf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1r, &rval, COUNTf, 0xFFFF);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));

    /* Initial EPIPE memory */
    rval = 0;
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1_COREr, &rval, RESET_ALLf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1_COREr, &rval, VALIDf, 1);
    soc_reg_field_set(unit, EGR_HW_RESET_CONTROL_1_COREr, &rval, COUNTf, 0xFFFF);
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1_COREr(unit, rval));

    /* For simulation, set timeout to 10 sec.  Otherwise, timeout = 50 ms */
    if (SAL_BOOT_SIMULATION) {
        pipe_init_usec = 10000000;
    } else {
        pipe_init_usec = 50000;
    }
    soc_timeout_init(&to, pipe_init_usec, 0);

    if (!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN)
    {
        /* Wait for IPIPE memory initialization done */
        in_progress = pipe_map;
        do {
            for (pipe = 0; pipe < _TH3_PIPES_PER_DEV && in_progress; pipe++) {
                if (!(pipe_map & (1 << pipe))) {
                    continue;
                }
                reg = SOC_REG_UNIQUE_ACC(unit, ING_HW_RESET_CONTROL_2r)[pipe];
                if (in_progress & (1 << pipe)) { /* not done yet */
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                        if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                            in_progress ^= 1 << pipe;
                        }
                }
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "unit %d : ING_HW_RESET timeout\n"), unit));
                break;
            }
        } while (in_progress != 0);

        /* Wait for EPIPE memory initialization done */
        in_progress = pipe_map;
        do {
            for (pipe = 0; pipe < _TH3_PIPES_PER_DEV && in_progress; pipe++) {
                if (!(pipe_map & (1 << pipe))) {
                    continue;
                }
                reg = SOC_REG_UNIQUE_ACC(unit, EGR_HW_RESET_CONTROL_1r)[pipe];
                if (in_progress & (1 << pipe)) { /* not done yet */
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                        if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                            in_progress ^= 1 << pipe;
                        }
                }
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "unit %d : ING_HW_RESET timeout\n"), unit));
                break;
            }
        } while (in_progress != 0);

        /* Wait for DLB memory initialization done */
        in_progress = 1;
        do {
            reg = DLB_ECMP_HW_RESET_CONTROLr;
            SOC_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
            if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                in_progress = 0;
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "unit %d : EGR_HW_RESET timeout\n"), unit));
                break;
            }
        } while (in_progress != 0);

        /* Wait for EPIPE memory initialization done */
        /* This is a new register EGR_HW_RESET_CONTROL_1_CORE added for
           TH3 which would help in initializing the EPIPE memories */
        in_progress = pipe_map;
        do {
            for (pipe = 0; pipe < _TH3_PIPES_PER_DEV && in_progress; pipe++) {
                if (!(pipe_map & (1 << pipe))) {
                    continue;
                }
                reg = SOC_REG_UNIQUE_ACC(unit, EGR_HW_RESET_CONTROL_1_COREr)[pipe];
                if (in_progress & (1 << pipe)) { /* not done yet */
                    SOC_IF_ERROR_RETURN
                        (soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
                        if (soc_reg_field_get(unit, reg, rval, DONEf)) {
                            in_progress ^= 1 << pipe;
                        }
                }
            }
            if (soc_timeout_check(&to)) {
                LOG_WARN(BSL_LS_SOC_COMMON,
                         (BSL_META_U(unit,
                                     "unit %d : EGR_HW_RESET timeout\n"), unit));
                break;
            }
        } while (in_progress != 0);
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(WRITE_ING_HW_RESET_CONTROL_2r(unit, rval));
    SOC_IF_ERROR_RETURN(WRITE_DLB_ECMP_HW_RESET_CONTROLr(unit, rval));
    rval = SOC_REG_INFO(unit, EGR_HW_RESET_CONTROL_1r).rst_val_lo;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1r(unit, rval));
    rval = SOC_REG_INFO(unit, EGR_HW_RESET_CONTROL_1_COREr).rst_val_lo;
    SOC_IF_ERROR_RETURN(WRITE_EGR_HW_RESET_CONTROL_1_COREr(unit, rval));

    return SOC_E_NONE;
}


STATIC int
_soc_tomahawk3_port_mapping_init(int unit)
{
    soc_info_t *si;
    int port, phy_port;
    /* int num_port, num_phy_port; */
    /* uint32 entry[SOC_MAX_MEM_WORDS];*/
    int i;
    uint16 dev_id = 0;
    uint8 rev_id = 0;

    /*
     * 96 entries MMU_CHFC_SYSPORT_MAPPINGm.SYS_PORT
     * 256 entries SYS_PORTMAPm.DEVICE_PORT_NUMBER
     */

    si = &SOC_INFO(unit);
    soc_cm_get_id(unit, &dev_id, &rev_id);

    /*sal_memset(&entry, 0, sizeof(entry));*/



#if 0
    num_port = soc_mem_index_count(unit, PORT_TABm) - 1;
    num_phy_port = 136;

    /* Ingress physical to device port mapping */
    /* Ingress GPP port to device port mapping */
    mem = SYS_PORTMAPm;
    sal_memset(&entry, 0, sizeof(sys_portmap_entry_t));
    for (port = 0; port < num_port; port++) {
        soc_mem_field32_set(unit, mem, &entry, DEVICE_PORT_NUMBERf, port);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, port, &entry));
    }

    /* Ingress device port to GPP port mapping
     * PORT_TAB.SRC_SYS_PORT_ID is programmed in the general port config
     * init routine _bcm_fb_port_cfg_init()
     */

#endif

    if (BCM56963_DEVICE_ID == dev_id) {
        const int phy_pbmp_lb_disable[] = {
            13,14,15,16,17,18,19,20,25,26,27,28,29,30,31,32,45,46,47,48,
            57,58,59,60,61,62,63,64,73,74,75,76,77,78,79,80,89,90,91,92,
            93,94,95,96,113,114,115,116,121,122,123,124,125,126,127,128};
        for (i = 0; i < COUNTOF(phy_pbmp_lb_disable); i++) {
            phy_port = phy_pbmp_lb_disable[i];
            port = si->port_p2l_mapping[phy_port];
            if (port != -1) {
                SOC_PBMP_PORT_ADD(loopback_disable[unit],port);
            }
        }
    }

    /* MMU portmapping from TM */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_port_mapping_init(unit));

    return SOC_E_NONE;
}

extern int
(*_phy_tsce_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);
extern int
(*_phy_tscf_firmware_set_helper[SOC_MAX_NUM_DEVICES])(int, int, uint8 *, int);

STATIC int
_soc_tomahawk3_tscx_firmware_set(int unit, int port, uint8 *array, int datalen)
{
    soc_mem_t mem = IS_CL_PORT(unit, port) ?
                    CLPORT_WC_UCMEM_DATAm : XLPORT_WC_UCMEM_DATAm;
    soc_reg_t reg = IS_CL_PORT(unit, port) ?
                    CLPORT_WC_UCMEM_CTRLr : XLPORT_WC_UCMEM_CTRLr;

    return soc_warpcore_firmware_set(unit, port, array, datalen, 0, mem, reg);
}

#define TSC_REG_ADDR_TSCID_SET(_phy_reg, _phyad)    \
                            ((_phy_reg) |= ((_phyad) & 0x1f) << 19)

STATIC int
_soc_tomahawk3_mdio_addr_to_port(uint32 phy_addr)
{
    int bus, offset;

    /* Must be internal MDIO address */
    if ((phy_addr & 0x80) == 0) {
        return 0;
    }

    /*
     * Internal phy address:
     * bus 0 phy 1 to 24 are mapped to Physical port 1 to 24
     * bus 1 phy 1 to 24 are mapped to Physical port 25 to 48
     * bus 2 phy 1 to 24 are mapped to Physical port 49 to 72
     * bus 3 phy 1 to 24 are mapped to Physical port 73 to 96
     * bus 4 phy 1 to 24 are mapped to Physical port 97 to 120
     * bus 5 phy 1 to 24 are mapped to Physical port 121 to 144
     * bus 6 phy 1 to 24 are mapped to Physical port 145 to 168
     * bus 7 phy 1 to 24 are mapped to Physical port 169 to 192
     * bus 8 phy 1 to 24 are mapped to Physical port 193 to 216
     * bus 9 phy 1 to 24 are mapped to Physical port 217 to 240
     * bus 10 phy 1 to 16 are mapped to Physical port 241 to 256
     * bus 11 phy 1 is mapped to Physical port 257 and
     *        phy 2 is mapped to Physical port 258
     */
    bus = PHY_ID_BUS_NUM(phy_addr);
    if (bus > 11) {
        return 0;
    }
    switch (bus) {
    case 0: offset = 0;
        break;
    case 1: offset = 24;
        break;
    case 2: offset = 48;
        break;
    case 3: offset = 72;
        break;
    case 4: offset = 96;
        break;
    case 5: offset = 120;
        break;
    case 6: offset = 144;
        break;
    case 7: offset = 168;
        break;
    case 8: offset = 192;
        break;
    case 9: offset = 216;
        break;
    case 10: offset = 240;
        break;
    case 11: offset = 256;
        /* Management ports take up the first 2 ports on the 4x10G in TH3 */
        if ((phy_addr & 0x1f) > 2) {
            return 0;
        }
        break;
    default:
        return 0;
    }

    return (phy_addr & 0x1f) + offset;
}

STATIC int
_soc_tomahawk3_tscx_reg_read(int unit, uint32 phy_addr,
                            uint32 phy_reg, uint32 *phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_tomahawk3_mdio_addr_to_port(phy_addr);

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_tomahawk3_tscx_reg_read[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    rv = soc_sbus_tsc_reg_read(unit, port, blk, phy_addr,
                               phy_reg, phy_data);
    return rv;
}

STATIC int
_soc_tomahawk3_tscx_reg_write(int unit, uint32 phy_addr,
                             uint32 phy_reg, uint32 phy_data)
{
    int rv, blk, port;
    int phy_port = _soc_tomahawk3_mdio_addr_to_port(phy_addr);

    port = SOC_INFO(unit).port_p2l_mapping[phy_port];
    blk = SOC_PORT_BLOCK(unit, phy_port);
    LOG_INFO(BSL_LS_SOC_MII,
             (BSL_META_U(unit,
                         "soc_tomahawk3_tscx_reg_write[%d]: %d/%d/%d/%d\n"),
              unit, phy_addr, phy_port, port, blk));
    rv = soc_sbus_tsc_reg_write(unit, port, blk, phy_addr,
                                phy_reg, phy_data);
    return rv;
}

#ifdef BCM_CMICX_SUPPORT 
STATIC int
_soc_th3_ledup_init(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int freq;
    uint32 rval, clk_half_period, refresh_period;

    /* freq in KHz */
    freq = (si->frequency) * 1000;

    /* For LED refresh period = 33 ms (about 30Hz)  */

    /* refresh period
       = (required refresh period in sec)*(switch clock frequency in Hz)
     */
    refresh_period = (freq * 33);

    rval = 0;
    soc_reg_field_set(unit, U0_LED_REFRESH_CTRLr, &rval,
                      REFRESH_CYCLE_PERIODf, refresh_period);

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_REFRESH_CTRLr(unit, rval));

    /* For LED clock period */
    /* LEDCLK_HALF_PERIOD
       = [(required LED clock period in sec)/2]*(M0SS clock frequency in Hz)

       Where M0SS freqency is 858MHz and
       Typical LED clock period is 200ns(5MHz) = 2*10^-7
    */
    freq = 858 * 1000000;
    clk_half_period = (freq + 2500000) / 5000000;
    clk_half_period = clk_half_period / 2;
    
    rval = 0;
    soc_reg_field_set(unit, U0_LED_CLK_DIV_CTRLr, &rval, 
                      LEDCLK_HALF_PERIODf, clk_half_period);      

    SOC_IF_ERROR_RETURN(WRITE_U0_LED_CLK_DIV_CTRLr(unit, rval)); 


    SOC_IF_ERROR_RETURN(READ_U0_LED_ACCU_CTRLr(unit, &rval)); 
    soc_reg_field_set(unit, U0_LED_ACCU_CTRLr, &rval, 
                      LAST_PORTf, 0);
    SOC_IF_ERROR_RETURN(WRITE_U0_LED_ACCU_CTRLr(unit, rval));
 
    /* To initialize M0s for LED or Firmware Link Scan*/ 
    if (soc_feature(unit, soc_feature_cmicx) && !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM)) {
        SOC_IF_ERROR_RETURN(soc_iproc_m0_init(unit));
    }

    return SOC_E_NONE;
}
#endif /* BCM_CMICX_SUPPORT */    

STATIC int
_soc_tomahawk3_management_port_init (int unit) {

    int port, fval;
    uint32 rval;
    soc_info_t *si=&SOC_INFO(unit);
    
    SOC_PBMP_ITER(si->management_pbm, port) {
        /* Assert XLPORT soft reset */
        rval = 0;
        if (si->port_p2l_mapping[257] != -1) { /* management port 0 */
            soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval, PORT0f, 1);
        }
        if (si->port_p2l_mapping[258] != -1) { /* management port 1 */
            soc_reg_field_set(unit, XLPORT_SOFT_RESETr, &rval, PORT2f, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, rval));

        /* Set port mode based upon lanes config */
        rval = 0;
        /* Note - this is a special h/w requirement to ONLY use dual lane mode */
        fval = 3;
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_CORE_PORT_MODEf,
                          fval);
        soc_reg_field_set(unit, XLPORT_MODE_REGr, &rval, XPORT0_PHY_PORT_MODEf,
                          fval);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MODE_REGr(unit, port, rval));

        /* De-assert XLPORT soft reset */
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_SOFT_RESETr(unit, port, 0));

        /* Enable XLPORT */
        rval = 0;
        if (si->port_p2l_mapping[257] != -1) { /* management port 0 */
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval, PORT0f, 1);
        }
        if (si->port_p2l_mapping[258] != -1) { /* management port 1 */
            soc_reg_field_set(unit, XLPORT_ENABLE_REGr, &rval, PORT2f, 1);
        }
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_ENABLE_REGr(unit, port, rval));

        rval = 0;
        soc_reg_field_set(unit, XLPORT_FLOW_CONTROL_CONFIGr, &rval,
                          MERGE_MODE_ENf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_XLPORT_FLOW_CONTROL_CONFIGr(unit, port, rval));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_tomahawk3_l2mcast_size_set
 * Purpose:
 *      This function programs L2 multicast table size in hardware and sets
 *      up soc_control structure's mc_size field value
 * Parameters:
 *      unit    - (IN) Unit number
 *      mc_size - (IN) Size of L2 multicast table. Cannot exceed hardware
 *                L2 multicast table size
 * Returns:
 *      Nothing
 * Note:
 *      TH3 has separate L2 and L3 multicast tables (as in TD3). Hardware
 *      may not use value programmed in MC_CONTROL_5r's field.
 */
STATIC int
_soc_tomahawk3_l2mcast_size_set(int unit, int mc_size)
{
    uint32	mc_ctrl;
    soc_control_t       *soc = SOC_CONTROL(unit);
    int l2mc_table_size = soc_mem_index_count(unit, L2MCm);

    if (mc_size > l2mc_table_size) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_MC_CONTROL_5r(unit, &mc_ctrl));
    soc_reg_field_set(unit, MC_CONTROL_5r, &mc_ctrl,
                      SHARED_TABLE_L2MC_SIZEf, mc_size);
    SOC_IF_ERROR_RETURN(WRITE_MC_CONTROL_5r(unit, mc_ctrl));

    soc->mcast_size = mc_size;

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
STATIC int
_soc_th3_l2_mem_entry_count_update(int unit)
{
    uint32 index_min, index_max, count;
    int i;
    int rv;
    l2x_entry_t *l2x_entries;
    int l2copyno;

    index_min = soc_mem_index_min(unit, L2Xm);
    index_max = soc_mem_index_max(unit, L2Xm);
    count = soc_mem_index_count(unit, L2Xm);

    /* Allocate memory to accomodate L2X table */
    l2x_entries = soc_cm_salloc(unit,
                           sizeof(l2x_entry_t) *
                           soc_mem_index_count(unit, L2Xm),
                           "L2Xm_entries");

    if (l2x_entries == NULL) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                                      "_soc_th3_l2_mem_entry_count_update:"
                                      " Memory alloc failed, size %d\n"),
                                      (int)(sizeof(l2x_entry_t) *
                                      soc_mem_index_count(unit, L2Xm))));

        return SOC_E_MEMORY;
    }

    sal_memset((void *)l2x_entries, 0, sizeof(l2x_entry_t) * count);

    /* Read L2 table */
    soc_mem_lock(unit, L2Xm);
    rv = soc_mem_read_range(unit, L2Xm, MEM_BLOCK_ANY,
                            index_min, index_max, l2x_entries);

    if (SOC_FAILURE(rv)) {
        soc_mem_unlock(unit, L2Xm);

        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                   "%s:DMA read failed: %s\n"), __FUNCTION__, soc_errmsg(rv)));
        return rv;
    }

    l2copyno = SOC_MEM_BLOCK_ANY(unit, L2Xm);

    /* Update count for each valid entry */
    SOP_MEM_STATE(unit, L2Xm).count[l2copyno] = 0;
    for (i = index_min; i <= index_max; i++) {
        l2x_entry_t *l2x_entry;

        l2x_entry = soc_mem_table_idx_to_pointer(unit, L2Xm, l2x_entry_t*,
                                                 l2x_entries, i);

        /* Skip invalid entry */
        if (!soc_L2Xm_field32_get(unit, l2x_entry, BASE_VALIDf)) {
            continue;
        }

        SOP_MEM_STATE(unit, L2Xm).count[l2copyno]++;
    }

    soc_mem_unlock(unit, L2Xm);

    if (l2x_entries != NULL) {
        soc_cm_sfree(unit, l2x_entries);
    }

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      _soc_tomahawk3_ipmc_size_set
 * Purpose:
 *      This function programs IPMC table size in hardware and sets
 *      up soc_control structure's mc_size field value
 * Parameters:
 *      unit    - (IN) Unit number
 *      mc_size - (IN) Size of IPMC table. Cannot exceed hardware
 *                IPMC size
 * Returns:
 *      Nothing
 * Note:
 *      TH3 has separate L2 and L3 multicast tables (as in TD3). Hardware
 *      may not use value programmed in MC_CONTROL_5r's field.
 */
STATIC int
_soc_tomahawk3_ipmc_size_set(int unit, int mc_size)
{
    uint32	mc_ctrl;
    soc_control_t       *soc = SOC_CONTROL(unit);
    int ipmc_table_size = soc_mem_index_count(unit, L3_IPMCm);

    if (mc_size > ipmc_table_size) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(READ_MC_CONTROL_5r(unit, &mc_ctrl));
    soc_reg_field_set(unit, MC_CONTROL_5r, &mc_ctrl,
                      SHARED_TABLE_IPMC_SIZEf, mc_size);
    SOC_IF_ERROR_RETURN(WRITE_MC_CONTROL_5r(unit, mc_ctrl));

    soc->ipmc_size = mc_size;

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk3_ipep_default_init (int unit)
{
    uint32 rval;
    int index, port;
    int count, sub_sel, offset;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int ecmp_mode = 0;
    soc_pbmp_t pbmp;
    uint64 rval64;
    static int rtag7_field_width[] = { 16, 16, 4, 16, 8, 8, 16, 16 };

    
    SOC_IF_ERROR_RETURN(READ_ING_CONFIG_64r(unit, &rval64));
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L3SRC_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          L2DST_HIT_ENABLEf, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L2f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          APPLY_EGR_MASK_ON_L3f, 1);
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_RARP_TO_FPf, 0x3); /* enable both ARP & RARP */
    soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                          ARP_VALIDATION_ENf, 1);
    if (soc_feature(unit, soc_feature_port_lag_failover)) {
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                              IGNORE_HG_HDR_LAG_FAILOVERf, 0);
    } else {
        soc_reg64_field32_set(unit, ING_CONFIG_64r, &rval64,
                              IGNORE_HG_HDR_LAG_FAILOVERf, 1);
    }
    SOC_IF_ERROR_RETURN(WRITE_ING_CONFIG_64r(unit, rval64));

    rval = 0;
    /* Enable pri/cfi remarking on egress ports. */
    soc_reg_field_set(unit, EGR_VLAN_CONTROL_1r, &rval, REMARK_OUTER_DOT1Pf,
                      1);
    PBMP_ALL_ITER(unit, port) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, port, rval));
    }

    SOC_PBMP_ASSIGN(pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(pbmp, PBMP_LB(unit));
    SOC_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm, &entry, BITMAPf,
                           &pbmp);
    SOC_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm,
                                      MEM_BLOCK_ANY, 0, &entry));

    
    /* Multicast range initialization */
    SOC_IF_ERROR_RETURN
        (_soc_tomahawk3_l2mcast_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L2_RANGE, soc_mem_index_count(unit, L2MCm))));
    SOC_IF_ERROR_RETURN
        (_soc_tomahawk3_ipmc_size_set(unit, soc_property_get(unit,
             spn_MULTICAST_L3_RANGE, soc_mem_index_count(unit, L3_IPMCm))));

    /* Check if Hierarchical ECMP mode is set */
    rval = 0;
    SOC_IF_ERROR_RETURN(READ_ECMP_CONFIGr(unit, &rval));

    if (soc_property_get(unit, spn_L3_ECMP_LEVELS, 1) == 2) {
        /* HW encoding for Hierarchical ECMP mode is 1.*/
        ecmp_mode = 1;
        soc_reg_field_set(unit, ECMP_CONFIGr, &rval, ECMP_MODEf, ecmp_mode);
        SOC_IF_ERROR_RETURN(WRITE_ECMP_CONFIGr(unit, rval));
    }
    /* Populate and enable RTAG7 Macro flow offset table */
    if (soc_mem_is_valid(unit, RTAG7_FLOW_BASED_HASHm)) {
        count = soc_mem_index_max(unit, RTAG7_FLOW_BASED_HASHm);
        sal_memset(entry, 0, sizeof(rtag7_flow_based_hash_entry_t));
        for (index = 0; index < count; ) {
            for (sub_sel = 0; sub_sel < 8 && index < count; sub_sel++) {
                for (offset = 0;
                     offset < rtag7_field_width[sub_sel] && index < count;
                     offset++) {
                    if (ecmp_mode == 1) {
                        soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                            SUB_SEL_ECMP_LEVEL2f, sub_sel);
                        soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                            OFFSET_ECMP_LEVEL2f, offset);
                    }
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        SUB_SEL_ECMPf, sub_sel);
                    soc_mem_field32_set(unit, RTAG7_FLOW_BASED_HASHm, &entry,
                                        OFFSET_ECMPf, offset);
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit, RTAG7_FLOW_BASED_HASHm,
                                       MEM_BLOCK_ANY, index, &entry));
                    index++;
                }
            }
        }
        rval = 0;
        soc_reg_field_set(unit, RTAG7_HASH_SELr, &rval, USE_FLOW_SEL_ECMPf, 1);

        SOC_IF_ERROR_RETURN(WRITE_RTAG7_HASH_SELr(unit, rval));
    }
    return SOC_E_NONE;
}

STATIC int 
_soc_tomahawk3_cpu_lb_portbitmap_set (int unit)
{
    uint32 entry[SOC_MAX_MEM_WORDS];

    sal_memset(entry, 0, sizeof(cpu_pbm_entry_t));
    soc_mem_pbmp_field_set(unit, CPU_PBMm, entry, BITMAPf, &PBMP_CMIC(unit));
    SOC_IF_ERROR_RETURN
        (soc_mem_write(unit, CPU_PBMm, MEM_BLOCK_ALL, 0, entry));

    return SOC_E_NONE;
}

int
soc_tomahawk3_clear_mmu_counters(int unit) {
/* Refer to SBus BASE TYPE info structure in REGSFILE */
#define SOC_MMU_BASE_TYPE_IPORT 0
#define SOC_MMU_BASE_TYPE_EPORT 1
#define SOC_MMU_BASE_TYPE_IPIPE 2
#define SOC_MMU_BASE_TYPE_EPIPE 3
#define SOC_MMU_BASE_TYPE_CHIP  4
#define SOC_MMU_BASE_TYPE_ITM   5
    soc_info_t *si;
    int i, j, use_base_type_views, base_type, index, num_views;
    uint32 entry[SOC_MAX_MEM_WORDS], valid_views_bmp = 0;
    soc_mem_t mem;
    static const struct {
        soc_mem_t mem; /* base view */
        int use_base_type_views; /* 1 means use itm/sc/etc views */
        int index; /* -1 means clear entire mem */
        int val; /* clear with assigned value */
    } mem_list[] = {
        { MMU_THDO_QUEUE_DROP_COUNTm, 1, -1, 0 },
        { MMU_THDO_PORT_DROP_COUNT_MCm, 1, -1, 0 },
        { MMU_THDO_PORT_DROP_COUNT_UCm, 1, -1, 0 },
        { MMU_THDO_COUNTER_QUEUEm, 1, -1, 0 },
        { MMU_THDI_PORT_PG_MIN_COUNTERm, 1, -1, 0 },
        { MMU_THDI_PORT_PG_SHARED_COUNTERm, 1, -1, 0 },
        { MMU_THDI_PORT_PG_HDRM_COUNTERm, 1, -1, 0 },
        { MMU_THDI_PORTSP_COUNTERm, 1, -1, 0 },
        { MMU_THDO_SRC_PORT_DROP_COUNTm, 1, -1, 0 },
        { INVALIDm, 0, -1, 0}
    };

    si = &SOC_INFO(unit);

    /* clear memories */
    for (i = 0; mem_list[i].mem != INVALIDm; i++) {
        mem = mem_list[i].mem;
        index = mem_list[i].index;
        use_base_type_views = mem_list[i].use_base_type_views;
        if (use_base_type_views) {
            base_type = soc_tomahawk3_mem_basetype_get(unit, mem);
            switch (base_type) {
            case SOC_MMU_BASE_TYPE_ITM:
                num_views = NUM_ITM(unit);
                valid_views_bmp = si->itm_map;
                break;
            case SOC_MMU_BASE_TYPE_IPIPE:
            case SOC_MMU_BASE_TYPE_EPIPE:
                num_views = NUM_PIPE(unit);
                soc_tomahawk3_pipe_map_get(unit, &valid_views_bmp);
                break;
            default:
                num_views = -1; /* not supported */
                break;
            }
            if (num_views < 0) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "mmu_mem %s, base_type %d will not be"
                                      "cleared \n"),
                           SOC_MEM_NAME(unit,mem), base_type));
                continue;
            }
        } else {
            num_views = 0; /* dont_care */
        }

        sal_memset(entry, mem_list[i].val,
                   soc_mem_entry_words(unit, mem) * sizeof(uint32));
        if (use_base_type_views) {
            for (j = 0; j < num_views; j++) {
                if((valid_views_bmp & (1 << j)) == 0) {
                    continue;
                }
                LOG_VERBOSE(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "mmu_mem %s, index %d will be "
                                        "cleared \n"),
                            SOC_MEM_NAME(unit, SOC_MEM_UNIQUE_ACC(unit, mem)[j]),
                            index));
                if (index >= 0) {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_write(unit,
                                       SOC_MEM_UNIQUE_ACC(unit, mem)[j],
                                       MEM_BLOCK_ALL, index, entry));
                } else {
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit,
                                       SOC_MEM_UNIQUE_ACC(unit, mem)[j],
                                       COPYNO_ALL, TRUE));
                }
            }
        } else {
            LOG_VERBOSE(BSL_LS_SOC_COMMON,
                        (BSL_META_U(unit,
                                    "mmu_mem %s, index %d will be "
                                    "cleared \n"),
                        SOC_MEM_NAME(unit, mem), index));
            if (index >= 0) {
                SOC_IF_ERROR_RETURN
                    (soc_mem_write(unit, mem, MEM_BLOCK_ALL, index,
                                   entry));
            } else {
                SOC_IF_ERROR_RETURN
                    (soc_mem_clear(unit, mem, COPYNO_ALL, TRUE));
            }
        }
    }
    return SOC_E_NONE;
}

STATIC int 
_soc_tomahawk3_port_mib_reset (int unit) 
{
    uint32 rval=0;
    int port, phy_port_base, block_info_idx;

    /* Reset CDMIB counters */
    rval = 0;
    soc_reg_field_set(unit, CDMAC_MIB_COUNTER_CTRLr, &rval, CNT_CLEARf, 1);
    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CDPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = SOC_TH3_PORT_BLOCK_BASE_PORT(port);
        for (port = phy_port_base; port < phy_port_base + _TH3_PORTS_PER_PBLK; port++) {
            soc_reg32_rawport_set(unit, CDMAC_MIB_COUNTER_CTRLr, port, 0, rval);
        }
    }
    sal_usleep(1);
    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_CDPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        phy_port_base = SOC_TH3_PORT_BLOCK_BASE_PORT(port);
        for (port = phy_port_base; port < phy_port_base + _TH3_PORTS_PER_PBLK; port++) {
            soc_reg32_rawport_set(unit, CDMAC_MIB_COUNTER_CTRLr, port, 0, 0);
        }
    }

    /* Reset XLMIB counters */
    rval = 0;
    soc_reg_field_set(unit, XLPORT_MIB_RESETr, &rval, CLR_CNTf, 0xf);
    SOC_BLOCK_ITER(unit, block_info_idx, SOC_BLK_XLPORT) {
        port = SOC_BLOCK_PORT(unit, block_info_idx);
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, rval));
        SOC_IF_ERROR_RETURN(WRITE_XLPORT_MIB_RESETr(unit, port, 0));
    }
    return SOC_E_NONE;
}

/* All ports are either Ethernet (0) or Loopback (2).  No HiGig in TH3 */
STATIC int
_soc_tomahawk3_hg_lb_port_type_set (int unit)
{
    uint32 entry[SOC_MAX_MEM_WORDS];
    int port;

    sal_memset(entry, 0, sizeof(egr_ing_port_entry_t));
    soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 2);
    PBMP_LB_ITER(unit, port) {
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, port, entry));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk3_cmic_mdio_freq_config (int unit)
{
    int int_divisor, ext_divisor, delay;
    /* spn_RATE_EXT_MDIO_DIVIDEND and spn_RATE_INT_MDIO_DIVIDEND are
     * NOT applicable to TH3.
     */

    /* MDIO Clock Frquency for TH3 is 250 MHz
     * Set external MDIO freq to around 5 MHz
     * Hence divisor is set to 50
     */

    ext_divisor = soc_property_get(unit, spn_RATE_EXT_MDIO_DIVISOR, 50);

    /*
     * Set internal MDIO freq to around 10 MHz
     * Valid range is from 2.5MHz to 20MHz
     * Hence divisor is set to 25
     */
    int_divisor = soc_property_get(unit, spn_RATE_INT_MDIO_DIVISOR, 25);

    delay = soc_property_get(unit, spn_MDIO_OUTPUT_DELAY, -1);
    if (delay < 0  || delay > 255) {
        /* valid range for delay is 0-255
         * if outside this range, skip configuring delay
         */
        delay = -1;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
              "Config MIIM rings with int divisor %d, ext divisor %d, delay %d\n"),
              int_divisor, ext_divisor, delay));

    soc_cmicx_miim_divider_set_all(unit, int_divisor, ext_divisor, delay);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_th3_speed_class_validate
 * Purpose:
 *      Check speed classes and ensure 4 speed max for device
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      This function is called at early stage of initialization for
 *      port configuration check.
 */
int
soc_th3_speed_class_validate(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port, speed;
    uint32 speed_mask, count;
    int speed_400 = 0, speed_200 = 0;

    speed_mask = 0;
    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port) ||
            IS_MANAGEMENT_PORT(unit, port) || IS_CPU_PORT(unit, port) ||
            IS_LB_PORT(unit, port)) {
            continue;
        }

        /* No HG, so just take speed rather than converting from HG to eth */
        speed = si->port_speed_max[port];

        /* 400G and 200G aren't part of SOC_PA_SPEED, so need to be counted
         * differently */
        if (speed == 400000) {
            speed_400 = 1;
        } else if (speed == 200000) {
            speed_200 = 1;
        } else {
            speed_mask |= SOC_PA_SPEED(speed);
        }
    }

    count = _shr_popcount(speed_mask);

    /* Add 400G and 200G if they exist */
    count += speed_400;
    count += speed_200;

    if (count > 7) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                  (BSL_META_U(unit,
                              "No port configurations with more than 7 port "
                              "speed classes are supported.\n")));
        return SOC_E_CONFIG;
    }

    return SOC_E_NONE;
}

STATIC void
_soc_tomahawk3_ser_hw_clear(int unit)
{
    int i;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 data = 0;
    soc_reg_t ep_ser_fifo[] = {
        EGR_SER_FIFO_CTRL_PIPE0r,
        EGR_SER_FIFO_CTRL_PIPE1r,
        EGR_SER_FIFO_CTRL_PIPE2r,
        EGR_SER_FIFO_CTRL_PIPE3r,
        EGR_SER_FIFO_CTRL_PIPE4r,
        EGR_SER_FIFO_CTRL_PIPE5r,
        EGR_SER_FIFO_CTRL_PIPE6r,
        EGR_SER_FIFO_CTRL_PIPE7r
    };
    soc_reg_t ep_ser_fifo_2[] = {
        EGR_SER_FIFO_CTRL_2_PIPE0r,
        EGR_SER_FIFO_CTRL_2_PIPE1r,
        EGR_SER_FIFO_CTRL_2_PIPE2r,
        EGR_SER_FIFO_CTRL_2_PIPE3r,
        EGR_SER_FIFO_CTRL_2_PIPE4r,
        EGR_SER_FIFO_CTRL_2_PIPE5r,
        EGR_SER_FIFO_CTRL_2_PIPE6r,
        EGR_SER_FIFO_CTRL_2_PIPE7r
    };
    soc_mem_t mmu_eb_fifo[] = {
        MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE0m,
        MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE1m,
        MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE2m,
        MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE3m,
        MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE4m,
        MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE5m,
        MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE6m,
        MMU_EBCFG_MEM_FAIL_ADDR_64_PIPE7m
    };
    uint32 pipe_map;

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    /* clear mmu eb ser fifo */
    for (i = 0; i < NUM_EB(unit); i++) {
        (void)soc_mem_pop(unit, mmu_eb_fifo[i], MEM_BLOCK_ANY, entry);
    }

    /* clear EP ser fifo */
    for (i = 0; i < NUM_PIPE(unit); i++) {
        if (!(pipe_map & (1 << i))) {
            continue;
        }
        (void)soc_reg_field32_modify(unit, ep_ser_fifo[i], REG_PORT_ANY,
                               FIFO_RESETf, 1);
        (void)soc_reg_field32_modify(unit, ep_ser_fifo_2[i], REG_PORT_ANY,
                               FIFO_RESETf, 1);
        (void)soc_reg_field32_modify(unit, ep_ser_fifo[i], REG_PORT_ANY,
                               FIFO_RESETf, 0);
        (void)soc_reg_field32_modify(unit, ep_ser_fifo_2[i], REG_PORT_ANY,
                               FIFO_RESETf, 0);
    }

    /* clear MEM_PAR_ERR_INT_STAT bit in mmu EB interrupt status register,
     * write 1 to clear
     */
    for (i = 0; i < NUM_PIPE(unit); i ++) {
        if (!(pipe_map & (1 << i))) {
            continue;
        }
        (void)soc_tomahawk3_eb_reg32_get(unit, MMU_EBCFG_CPU_INT_STATr, i, i, 0, &data);
        if(soc_reg_field_get(unit, MMU_EBCFG_CPU_INT_STATr, data, MEM_PAR_ERR_INT_STATf)) {
            soc_tomahawk3_eb_reg32_set(unit, MMU_EBCFG_CPU_INT_STATr, i, i, 0, 0x1);
        }
        soc_tomahawk3_eb_reg32_set(unit, MMU_EBCFG_MEM_FAIL_INT_CTRr, i, i, 0, 0x0);
    }
    /* clear EBCFGx_INT_STAT bit in GLB interrupt status register, write 1 to clear */
    (void)soc_reg32_set(unit, MMU_GLBCFG_CPU_INT_STATr, REG_PORT_ANY, 0, 0x1fe);

}

/*
 * Function:
 *      soc_th3_process_pm_intr
 * Description:
 *      Tomahawk3 specific DPC function to service PM interrupts
 * Parameters:
 *      unit_vp       - Device number
 *      d1            - IRQ4 mask
 *      d2            - IRQ4 number (to distinguish between
 *                      different types of interrupts).
 */
static void
soc_th3_process_pm_intr(void *unit_vp, void *d1,
                             void *d2, void *d3, void *d4)
{

    int unit = PTR_TO_INT(unit_vp);
    int cmic_bit_num = PTR_TO_INT(d1);
    int cmic_reg_type = PTR_TO_INT(d2);
    uint32 intr_en = PTR_TO_INT(d3);
    int pm_id = -1; /* invalid id */
    int rv = SOC_E_NONE;
    portmod_ecc_intr_info_t ecc_intr_info;
    soc_reg_t ser_intr_reg[4] = {ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                                 ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                                 ICFG_CHIP_LP_INTR_ENABLE_REG2r,
                                 ICFG_CHIP_LP_INTR_ENABLE_REG3r};

    /*
     * Get the SW Port Macro Id from the hw
     * Port Macro Id(mapped to core num)
     */
    rv = portmod_core_pm_id_get(unit, cmic_bit_num - 1, &pm_id);

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "soc_th3_process_pm_intr sw PM id for "
                                    "hw PM Id - %d not found!!\n"
                                    "Disabling unhandled interrupt(s)"),
                                    (cmic_bit_num - 1)));
        /* Disable the specific PM interrupt and recover control bit for the other PM interrupts */
       (void) soc_iproc_setreg(unit, soc_reg_addr(unit, ser_intr_reg[cmic_reg_type],
                               REG_PORT_ANY, 0), intr_en & ~(1 << (cmic_bit_num - 1)));
        return;
    }

    sal_memset(&ecc_intr_info, 0, sizeof(portmod_ecc_intr_info_t));

    /* Process interrupts for the specific port Macro */
    rv = portmod_pm_interrupt_process(unit, pm_id, &ecc_intr_info);

    if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "soc_th3_process_pm_intr - pm_id(%d)"
                                    "Disabling unhandled interrupt(s)"), pm_id));
         /* Disable the specific PM interrupt and recover control bit for the other PM interrupts */
        (void) soc_iproc_setreg(unit, soc_reg_addr(unit, ser_intr_reg[cmic_reg_type],
                                REG_PORT_ANY, 0), intr_en & ~(1 << (cmic_bit_num - 1)));
        return;
    }

    /* Report ECC errors */
    if (ecc_intr_info.ecc_num_bits_err != PORTMOD_PM_ECC_NO_ERR) {

        soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                           SOC_SWITCH_EVENT_DATA_ERROR_ECC, 0, 0);

        if (ecc_intr_info.ecc_num_bits_err & PORTMOD_PM_ECC_1B_ERR) {

            soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                           SOC_SWITCH_EVENT_DATA_ERROR_AUTO_CORRECTED, 0, 0);

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                 "\n%s - \n\r"
                                 "single-bit ECC error on port %s\n"),
                                 ecc_intr_info.mem_str,
                                 SOC_PORT_NAME(unit, ecc_intr_info.port)));

        } else if (ecc_intr_info.ecc_num_bits_err & PORTMOD_PM_ECC_2B_ERR) {

            soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                           SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                 "\n%s - \n\r"
                                 "double-bit ECC error on port %s\n"),
                                 ecc_intr_info.mem_str,
                                 SOC_PORT_NAME(unit, ecc_intr_info.port)));

        } else if (ecc_intr_info.ecc_num_bits_err & PORTMOD_PM_ECC_MULTIB_ERR) {

            soc_event_generate(unit, SOC_SWITCH_EVENT_DATA_ERROR_PARITY,
                           SOC_SWITCH_EVENT_DATA_ERROR_UNCORRECTABLE, 0, 0);

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                 "\n%s - \n\r"
                                 "multi-bit ECC error on port %s\n"),
                                 ecc_intr_info.mem_str,
                                 SOC_PORT_NAME(unit, ecc_intr_info.port)));

        } else {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                       (BSL_META_U(unit, "ECC interrupt info\n"
                     "m - %s, ecc_num_bits - %d, addr %x - error - %s\n"),
                     SOC_MEM_NAME(unit, ecc_intr_info.err_mem_info),
                     ecc_intr_info.ecc_num_bits_err,
                     ecc_intr_info.err_addr, ecc_intr_info.mem_str));
       }
    }

     /* Recover control bit for the PM interrupts */
    (void) soc_iproc_setreg(unit, soc_reg_addr(unit, ser_intr_reg[cmic_reg_type],
                            REG_PORT_ANY, 0), intr_en);

    /*soc_th3_ser_intr_handler(unit, PTR_TO_INT(d1), cmic_reg_type);*/
    return;
}

/*
 * Function:
 *      soc_th3_process_pm_intr
 * Description:
 *      Tomahawk3 specific DPC function to service PM interrupts
 * Parameters:
 *      unit_vp       - Device number
 *      d1            - IRQ4 mask
 *      d2            - IRQ4 number (to distinguish between
 *                      different types of interrupts).
 */
static void
soc_th3_pm_intr_handler(int unit, uint32 cmic_val, int reg_num)
{
    uint32 cmic_bit = cmic_val;
    uint32 cmic_bit_num = 0;
    uint32 reg_val = 0;
    soc_reg_t ser_intr_reg[4] = {ICFG_CHIP_LP_INTR_ENABLE_REG0r,
                                 ICFG_CHIP_LP_INTR_ENABLE_REG1r,
                                 ICFG_CHIP_LP_INTR_ENABLE_REG2r,
                                 ICFG_CHIP_LP_INTR_ENABLE_REG3r};

    /* Get the bit num or the hw port macro id */
    while(cmic_bit != 0) {
        cmic_bit >>= 1;
        cmic_bit_num++;
    }

    (void) soc_iproc_getreg(unit, soc_reg_addr(unit, ser_intr_reg[reg_num],
                            REG_PORT_ANY, 0), &reg_val);
    if (reg_val != 0) {

        /* Disable the interrupt to avoid repetitive interrupts*/
        (void) soc_iproc_setreg(unit, soc_reg_addr(unit, ser_intr_reg[reg_num],
                                REG_PORT_ANY, 0), 0);

        /* Register dpc thread to read and clear intterupts */
        sal_dpc(soc_th3_process_pm_intr, INT_TO_PTR(unit),
                INT_TO_PTR(cmic_bit_num), INT_TO_PTR(reg_num), INT_TO_PTR(reg_val), 0);
    }

    return;
}

/*
 * Function:
 *      soc_th3_lp_intr_handler
 * Purpose:
 *      This function is the main low priority interrupt handler. L2 learn cache
 *      SER and other low priority interrupts should be handled by this
 *      function.
 * Parameters:
 *      unit  - SOC unit #
 * Returns:
 *     Nothing
 */
STATIC
void soc_th3_lp_intr_handler(int unit, void *data)
{
    int parity_enable = 0;
    uint32 regval = 0;
    soc_reg_t reg[] = { ICFG_CHIP_LP_INTR_RAW_STATUS_REG0r,
                        ICFG_CHIP_LP_INTR_RAW_STATUS_REG1r,
                        ICFG_CHIP_LP_INTR_RAW_STATUS_REG2r,
                        ICFG_CHIP_LP_INTR_RAW_STATUS_REG3r,
                        ICFG_CHIP_LP_INTR_STATUS_REG0r,
                        ICFG_CHIP_LP_INTR_STATUS_REG1r,
                        ICFG_CHIP_LP_INTR_STATUS_REG2r,
                        ICFG_CHIP_LP_INTR_STATUS_REG3r
                      };

    /* Note CHIP_INTR_LOW_PRIORITY is disabled in the calling handler function
     * (from CMICx layer)
     */
    /* SER and any other modules should check the interrupt bits they will
     * handle, add their handlers here
     */
    /* Check interrupts in reg[4] */
    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, reg[4], REG_PORT_ANY, 0),
                           &regval);

    /* Check for the temperature interrupt and call handler */
    if (regval & 0x4) { /* PVTMON_INTR , bit 2 */
        /* soc_tomahawk3_temperature_intr(INT_TO_PTR(unit), NULL, NULL, NULL, NULL); */
        sal_dpc(soc_tomahawk3_temperature_intr, INT_TO_PTR(unit), 0, 0, 0, 0);
    }

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
    /* Check interrupts in reg[0] */
    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, reg[0], REG_PORT_ANY, 0),
                           &regval);
    if ((regval & SOC_TH3_SER_REG0_INTR_MASK) && parity_enable) {
        soc_th3_ser_intr_handler(unit, regval, 0);
    }

    regval = 0;
    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, reg[2], REG_PORT_ANY, 0),
                           &regval);
    if ((regval & SOC_TH3_SER_REG2_INTR_MASK) && parity_enable) {
        soc_th3_pm_intr_handler(unit, regval, 2);
        /*sal_dpc(soc_th3_process_pm_intr, INT_TO_PTR(unit),
                INT_TO_PTR(regval), INT_TO_PTR(2), 0, 0);*/
    }

    regval = 0;
    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, reg[3], REG_PORT_ANY, 0),
                           &regval);
    if ((regval & SOC_TH3_SER_REG3_INTR_MASK) && parity_enable) {
        soc_th3_ser_intr_handler(unit, regval, 3);
    }

    regval = 0;
    (void)soc_iproc_getreg(unit, soc_reg_addr(unit, reg[1], REG_PORT_ANY, 0),
                           &regval);
    if ((regval & SOC_TH3_SER_REG1_INTR_MASK) && parity_enable) {
        soc_th3_ser_intr_handler(unit, regval, 1);
    }
    /* Atleast one L2 learn cache interrupt set, call learn handler */
    if (regval & SOC_TH3_L2_LEARN_CACHE_INTR_MASK) {
        soc_th3_lrn_cache_intr_handler(unit, data);
    }

   /* Re-enable main CMICx interrupt */
   soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
}


/*
 * Function:
 *      soc_th3_lp_intr_register
 * Purpose:
 *      This function registers a low priority interrupt handler for all low
 *      priority interrupts. L2 learn cache, SER and other low priority
 *      interrupts are handled by the handler function registered in this
 *      function
 * Parameters:
 *      unit  - SOC unit #
 * Returns:
 *     Nothing
 */
STATIC void
soc_th3_lp_intr_register(int unit)
{
    soc_cmic_intr_handler_t handle;

    handle.num = CHIP_INTR_LOW_PRIORITY;
    handle.intr_fn = soc_th3_lp_intr_handler;
    handle.intr_data = NULL;
    soc_cmic_intr_register(unit, &handle, 1);

   /* Enable main CMICx interrupt */
   soc_cmic_intr_enable(unit, CHIP_INTR_LOW_PRIORITY);
}

int
soc_tomahawk3_misc_init_fast_reboot(int unit)
{
    /*soc_mem_t mem;*/
    int rv, i;
    uint8 bypass_valid = 0;
    uint8 rev_id;
    uint16 dev_id;
    /* _soc_tomahawk_tdm_t *tdm; */
    int  parity_enable;
    /*int clport, lane, mode, class, index;*/
    soc_info_t *si = &SOC_INFO(unit);
    /*static soc_field_t port_fields[_TH_PORTS_PER_PBLK] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    */
    uint64 fval_64;
    uint64 rval_64;
    int port, cnt;
    const static soc_reg_t cnt_reg_list[] =
    {
        RUC_64r,    RPORTD_64r, RDBGC0_64r,  RDBGC1_64r,
        RDBGC2_64r, RDBGC3_64r, RDBGC4_64r,  RDBGC5_64r,
        RDBGC6_64r, RDBGC7_64r, RDBGC8_64r,  ING_ECN_COUNTER_64r,
        TDBGC0_64r, TDBGC1_64r, TDBGC2_64r,  TDBGC3_64r,
        TDBGC4_64r, TDBGC5_64r, TDBGC6_64r,  TDBGC7_64r,
        TDBGC8_64r, TDBGC9_64r, TDBGC10_64r, TDBGC11_64r
    };
    void *null_entry = NULL;
    int copyno;
    uint32 index = 0;
    soc_mem_t mem = INVALIDm;

    uint32 pipe_map;
    uint8 used_pm_map[_TH3_PBLKS_PER_DEV + 1];
    uint8 *buf;
    int phy_port, pm_num, pipe_num, size, pm;
    const soc_mem_t idb_obm_list[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE] = {
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE0m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE0m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE0m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE0m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE1m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE1m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE1m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE1m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE2m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE2m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE2m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE2m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE3m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE3m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE3m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE3m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE4m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE4m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE4m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE4m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE5m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE5m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE5m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE5m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE6m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE6m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE6m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE6m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE7m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE7m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE7m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE7m},
    };

    sal_memset(used_pm_map, 0, sizeof(used_pm_map));
    for (phy_port = 1; phy_port <= _TH3_PORTS_PER_DEV; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port > 0) {
            pm = si->port_serdes[port];
            if ((-1 != pm) && (0 == used_pm_map[pm])) {
                used_pm_map[pm] = 1;
            }
        }
    }


    soc_cm_get_id(unit, &dev_id, &rev_id);

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_stop(unit));

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify
            (unit, AUX_ARB_CONTROLr, REG_PORT_ANY, FP_REFRESH_ENABLEf, 1));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);

    /* Not used in cold boot, so not used in fast reboot either */
#if 0
     /*Slot pipeline config*/
     SOC_IF_ERROR_RETURN(soc_tomahawk3_slot_pipeline_config(unit));
#endif

    /* Reset counts on debug tx/rx debug counters */
    if (SAL_BOOT_SIMULATION && !SAL_BOOT_QUICKTURN) {
        COMPILER_64_ZERO(fval_64);
        PBMP_ALL_ITER(unit, port) {
            for (cnt=0; cnt<COUNTOF(cnt_reg_list) ; cnt++) {
                COMPILER_64_ZERO(rval_64);
                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, cnt_reg_list[cnt], port, 0, &rval_64));
                soc_reg64_field_set(
                    unit, cnt_reg_list[cnt], &rval_64, COUNTf, fval_64);
                SOC_IF_ERROR_RETURN(
                    soc_reg_set(unit, cnt_reg_list[cnt], port, 0, rval_64));
           }
        }
    }
    COMPILER_64_ZERO(rval_64);
    /* For TH3-5414, we need to make sure the below register
       is cleared even for invalid ports 
     */
    for (i = 0; i < 160 ; i++) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_1588_LINK_DELAY_64r(unit, i, rval_64));
    }

    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {

        /* Skip this for fast reboot or else counters will be set to off 
        SOC_IF_ERROR_RETURN(_soc_tomahawk3_port_mib_reset(unit));
        */

        /* Dummy call to clear garbage h/w values */

        /* Commented out for Fast Reboot 
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_temperature_monitor_get(unit, COUNTOF(pvtmon_result_reg_th3),
                                                  NULL, NULL));

        SOC_IF_ERROR_RETURN(soc_tomahawk3_clear_mmu_counters(unit));
        */

        SOC_IF_ERROR_RETURN(soc_tomahawk3_init_tm_memory(unit));
        mem = EDB_XMIT_START_COUNTm;
        null_entry = soc_mem_entry_null(unit, mem);
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        for (index = soc_mem_index_min(unit, mem);
             index <= soc_mem_index_max(unit, mem); index++) {
            rv = soc_mem_write(unit, mem, copyno, index, null_entry);
        }

        size = SOC_MEM_TABLE_BYTES(unit, idb_obm_list[0][0]);
        buf = soc_cm_salloc(unit, size, "IDB OBM MEMORY");
        sal_memset(buf, 0, size);
        PBMP_PORT_ITER(unit, port) {
            if (SOC_PBMP_MEMBER(si->management_pbm, port)) {
                continue;
            }
            phy_port = si->port_l2p_mapping[port];
            pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
            pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
            if (!(pipe_map & (1 << pipe_num))) {
                continue;
            }
            if (used_pm_map[pm_num] == 0) {
                continue;
            }
            mem = idb_obm_list[pipe_num][pm_num];
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
            rv = soc_mem_write_range(unit, mem, copyno, soc_mem_index_min(unit, mem),
                                     soc_mem_index_max(unit, mem), buf);
            if (rv != SOC_E_NONE) {
                soc_cm_sfree(unit, buf);
                return rv;
            }
        }
        soc_cm_sfree(unit, buf);
    }

    if (parity_enable) {
        if ((!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN) &&
            !SOC_WARM_BOOT(unit)) {

            /* Certain mems/regs need to be cleared before enabling SER */
            
        }

        /* Enable logging of SER events */
        soc_ser_log_init(unit, NULL, 0);

        
        /* Enable l2_mgmt interrupt */
        /* SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, L2_MGMT_INTR_MASKr, REG_PORT_ANY,
                                    SER_FIFO_NOT_EMPTYf, 1));*/
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            
            /*soc_cmicm_intr3_enable(unit, 0x2); L2_MGMT_TO_CMIC_INTR bit 1 */
        }
#endif
        
        /* SOC_IF_ERROR_RETURN(soc_tomahawk_tcam_ser_init(unit)); */
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_enable_all(unit, TRUE));
        SOC_IF_ERROR_RETURN(soc_tomahawk3_ser_enable_parity_table_all(unit, TRUE));
        soc_tomahawk_ser_register(unit);
#if defined(SER_TR_TEST_SUPPORT)
        /*Initialize chip-specific functions for SER testing*/
        soc_th_ser_test_register(unit);
#endif /*defined(SER_TR_TEST_SUPPORT*/
    }

        /* Clear SER related fifo before enabling low priority interrupt */
        if (!SAL_BOOT_SIMULATION) {
        _soc_tomahawk3_ser_hw_clear(unit);
    }

    /* Turn on low priority interrupts for actual h/w and emulation */
    /* Note the individual modules should check if they will be working in
     * interrupt mode, and enable/disable module level interrupts accordingly
     */
    if ((!SAL_BOOT_SIMULATION) || (SAL_BOOT_QUICKTURN)) {
        if (soc_feature(unit, soc_feature_cmicx)) {
            soc_th3_lp_intr_register(unit);
        }
    }

    /* Configure Switch Latency Bypass Mode */
    bypass_valid = soc_property_get(unit, spn_SWITCH_BYPASS_MODE,
                                        SOC_SWITCH_BYPASS_MODE_NONE);
    if ((SOC_SWITCH_BYPASS_MODE_NONE != bypass_valid) &&
        (SOC_SWITCH_BYPASS_MODE_EFP != bypass_valid)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Low latency and Balanced latency modes not"
                                "supported on BCM5698x \n")));
        return SOC_E_CONFIG;
    }
    SOC_IF_ERROR_RETURN(soc_th_latency_init(unit));

    /*Dev_port, Phyport, MMU port, IDB port mappings*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_port_mapping_init(unit));

    /* Commented out for Fast Reboot */
#if 0
    /* No HG port support */
    SOC_CONTROL_LOCK(unit);
    si->fabric_port_enable = 0;
    SOC_CONTROL_UNLOCK(unit);

    for(port_chk = 0; port_chk < num_port; port_chk++) {
        if (si->port_speed_max[port_chk] > 0) {
            rv = soc_th3_port_speed_supported(unit, port_chk,
                                             si->port_speed_max[port_chk]);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                            (BSL_META_U(unit,
                                        "Unsupported port speed : port - %d, "
                                        "speed - %d\n"), port_chk,
                                        si->port_speed_max[port_chk]));
                return rv;
            }
        }
    }

    /* Check the speed classes, make sure only 4 speeds in the device */
    SOC_IF_ERROR_RETURN(soc_th3_speed_class_validate(unit));
#endif 

    /*DV: TDM Code*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_update(unit));

        /* Initialize IDB */
        /*DV: Integrate IDB init from DV*/
        rv = soc_tomahawk3_idb_init_update(unit);
        if (SOC_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "FAILED in"
                                  "soc_tomahawk3_idb_init_update\n")));
            return rv;
        }

    /*Setup CPU_PBM and Loopback port bitmap*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_cpu_lb_portbitmap_set(unit));

    /*Setup port_type for HG/LB ports*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_hg_lb_port_type_set(unit));

    /* Commented out for Fast Reboot */
#if 0
#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
        /* done later, after port mapping is configured by ep_top. */
    } else
#endif
    {
        /*XLPORT init*/
        SOC_IF_ERROR_RETURN(_soc_tomahawk3_management_port_init(unit));
    }
#endif
    /*Enable refresh in MMU*/
     SOC_IF_ERROR_RETURN(soc_tomahawk3_en_mmu_refresh(unit));

    /* Enable dual hash tables */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_hash_init(unit));

    /*DV: integrate EP credit int and port enable from DV*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_top_init(unit));

    /* Commented out for Fast Reboot */
#if 0
#if defined(PLISIM)
    if (SAL_BOOT_PLISIM) {
        /*XLPORT init*/
        SOC_IF_ERROR_RETURN(_soc_tomahawk3_management_port_init(unit));
    }
#endif /* PLISIM */
    /*Enable MAC after EP enable*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_cdport_init(unit));
#endif

    /* Enable ingress forwarding is handled when the user has finished reconfig
     * via the api bcm_port_fast_reboot_traffic_enable
     */

    /*Default programming required in IPEP*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_ipep_default_init(unit));



#if 0
    
    if (SAL_BOOT_QUICKTURN) {
        PBMP_PORT_ITER(unit, port) {
            if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_port_speed_update(unit, port,
                                                si->port_speed_max[port]));
        }
    }
#endif
#if 0
    /*external,internal MDIO freq config*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_cmic_mdio_freq_config(unit));
#endif

    /*FIXME:
    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "unit %d : MSPI Init Failed\n"), unit));
    }*/
    _phy_tsce_firmware_set_helper[unit] = _soc_tomahawk3_tscx_firmware_set;
    _phy_tscf_firmware_set_helper[unit] = _soc_tomahawk3_tscx_firmware_set;


    SOC_IF_ERROR_RETURN
        (soc_counter_tomahawk_status_enable(unit, TRUE));
#if 0
    /* Mem Scan thread start should be the last step in Misc init */
    if (parity_enable) {
        if ((!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN) &&
            !SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_start(unit));
        }
    }
#endif
    /* Create a mutex for s/w re-ordering */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_hash_mutex_init(unit));

    /* Commented out for Fast Reboot */
#if 0
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) &&
        !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM))
    {
        SOC_IF_ERROR_RETURN(_soc_th3_ledup_init(unit));
    }
#endif /* BCM_CMICX_SUPPORT */

#ifdef INCLUDE_AVS
    SOC_IF_ERROR_RETURN(soc_tomahawk3_avs_init(unit));
#endif /* INCLUDE_AVS  */
#endif

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MISC Init fast reboot completed (u=%d)\n"), unit));
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk3_misc_init(int unit)
{
    /*soc_mem_t mem;*/
    int rv, i;
    uint8 bypass_valid = 0;
    uint8 rev_id;
    uint16 dev_id;
    /* _soc_tomahawk_tdm_t *tdm; */
    int  parity_enable;
    /*int clport, lane, mode, class, index;*/
    soc_info_t *si = &SOC_INFO(unit);
    /*static soc_field_t port_fields[_TH_PORTS_PER_PBLK] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };
    */
    uint64 fval_64;
    uint64 rval_64;
    int port, cnt;
    const static soc_reg_t cnt_reg_list[] =
    {
        RUC_64r,    RPORTD_64r, RDBGC0_64r,  RDBGC1_64r,
        RDBGC2_64r, RDBGC3_64r, RDBGC4_64r,  RDBGC5_64r,
        RDBGC6_64r, RDBGC7_64r, RDBGC8_64r,  ING_ECN_COUNTER_64r,
        TDBGC0_64r, TDBGC1_64r, TDBGC2_64r,  TDBGC3_64r,
        TDBGC4_64r, TDBGC5_64r, TDBGC6_64r,  TDBGC7_64r,
        TDBGC8_64r, TDBGC9_64r, TDBGC10_64r, TDBGC11_64r
    };
    void *null_entry = NULL;
    int copyno;
    uint32 index = 0;
    soc_mem_t mem = INVALIDm;

    uint32 pipe_map;
    uint8 used_pm_map[_TH3_PBLKS_PER_DEV + 1];
    uint8 *buf;
    int phy_port, pm_num, pipe_num, size, pm;
    const soc_mem_t idb_obm_list[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE] = {
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE0m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE0m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE0m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE0m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE1m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE1m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE1m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE1m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE2m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE2m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE2m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE2m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE3m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE3m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE3m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE3m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE4m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE4m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE4m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE4m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE5m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE5m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE5m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE5m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE6m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE6m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE6m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE6m},
        {IDB_OBM0_IOM_STATS_WINDOW_RESULTS_PIPE7m,
         IDB_OBM1_IOM_STATS_WINDOW_RESULTS_PIPE7m,
         IDB_OBM2_IOM_STATS_WINDOW_RESULTS_PIPE7m,
         IDB_OBM3_IOM_STATS_WINDOW_RESULTS_PIPE7m},
    };

    sal_memset(used_pm_map, 0, sizeof(used_pm_map));
    for (phy_port = 1; phy_port <= _TH3_PORTS_PER_DEV; phy_port++) {
        port = si->port_p2l_mapping[phy_port];
        if (port > 0) {
            pm = si->port_serdes[port];
            if ((-1 != pm) && (0 == used_pm_map[pm])) {
                used_pm_map[pm] = 1;
            }
        }
    }


    soc_cm_get_id(unit, &dev_id, &rev_id);

    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    /* Stop the mem scan task before all of the parity init takes place */
    SOC_IF_ERROR_RETURN(soc_generic_ser_mem_scan_stop(unit));
    SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_stop(unit));

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify
            (unit, AUX_ARB_CONTROLr, REG_PORT_ANY, FP_REFRESH_ENABLEf, 1));

    parity_enable = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);
#if 0
     /*Slot pipeline config*/
     SOC_IF_ERROR_RETURN(soc_tomahawk3_slot_pipeline_config(unit));
#endif

    /* Reset counts on debug tx/rx debug counters */
    if (SAL_BOOT_SIMULATION && !SAL_BOOT_QUICKTURN) {
        COMPILER_64_ZERO(fval_64);
        PBMP_ALL_ITER(unit, port) {
            for (cnt=0; cnt<COUNTOF(cnt_reg_list) ; cnt++) {
                COMPILER_64_ZERO(rval_64);
                SOC_IF_ERROR_RETURN(
                    soc_reg_get(unit, cnt_reg_list[cnt], port, 0, &rval_64));
                soc_reg64_field_set(
                    unit, cnt_reg_list[cnt], &rval_64, COUNTf, fval_64);
                SOC_IF_ERROR_RETURN(
                    soc_reg_set(unit, cnt_reg_list[cnt], port, 0, rval_64));
           }
        }
    }
    COMPILER_64_ZERO(rval_64);
    /* For TH3-5414, we need to make sure the below register
       is cleared even for invalid ports 
     */
    for (i = 0; i < 160 ; i++) {
        SOC_IF_ERROR_RETURN(WRITE_EGR_1588_LINK_DELAY_64r(unit, i, rval_64));
    }
  
    if (!SOC_IS_RELOADING(unit) && !SOC_WARM_BOOT(unit)) {

        SOC_IF_ERROR_RETURN(_soc_tomahawk3_port_mib_reset(unit));

        /* Dummy call to clear garbage h/w values */
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_temperature_monitor_get(unit, COUNTOF(pvtmon_result_reg_th3),
                                                  NULL, NULL));

        SOC_IF_ERROR_RETURN(soc_tomahawk3_clear_mmu_counters(unit));
        SOC_IF_ERROR_RETURN(soc_tomahawk3_init_tm_memory(unit));
        mem = EDB_XMIT_START_COUNTm;
        null_entry = soc_mem_entry_null(unit, mem);
        copyno = SOC_MEM_BLOCK_ANY(unit, mem);
        for (index = soc_mem_index_min(unit, mem);
             index <= soc_mem_index_max(unit, mem); index++) {
            rv = soc_mem_write(unit, mem, copyno, index, null_entry);
        }

        size = SOC_MEM_TABLE_BYTES(unit, idb_obm_list[0][0]);
        buf = soc_cm_salloc(unit, size, "IDB OBM MEMORY");
        sal_memset(buf, 0, size);
        PBMP_PORT_ITER(unit, port) {
            if (SOC_PBMP_MEMBER(si->management_pbm, port)) {
                continue;
            }
            phy_port = si->port_l2p_mapping[port];
            pipe_num = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
            pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
            if (!(pipe_map & (1 << pipe_num))) {
                continue;
            }
            if (used_pm_map[pm_num] == 0) {
                continue;
            }
            mem = idb_obm_list[pipe_num][pm_num];
            copyno = SOC_MEM_BLOCK_ANY(unit, mem);
            rv = soc_mem_write_range(unit, mem, copyno, soc_mem_index_min(unit, mem),
                                     soc_mem_index_max(unit, mem), buf);
            if (rv != SOC_E_NONE) {
                soc_cm_sfree(unit, buf);
                return rv;
            }
        }
        soc_cm_sfree(unit, buf);
    }

    if (parity_enable) {
        if ((!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN) &&
            !SOC_WARM_BOOT(unit)) {

            /* Certain mems/regs need to be cleared before enabling SER */
            
        }

        /* Enable logging of SER events */
        soc_ser_log_init(unit, NULL, 0);

        
        /* Enable l2_mgmt interrupt */
        /* SOC_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, L2_MGMT_INTR_MASKr, REG_PORT_ANY,
                                    SER_FIFO_NOT_EMPTYf, 1));*/
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            
            /*soc_cmicm_intr3_enable(unit, 0x2); L2_MGMT_TO_CMIC_INTR bit 1 */
        }
#endif
        
        /* SOC_IF_ERROR_RETURN(soc_tomahawk_tcam_ser_init(unit)); */
        SOC_IF_ERROR_RETURN(soc_tomahawk_ser_enable_all(unit, TRUE));
        SOC_IF_ERROR_RETURN(soc_tomahawk3_ser_enable_parity_table_all(unit, TRUE));
        soc_tomahawk_ser_register(unit);
#if defined(SER_TR_TEST_SUPPORT)
        /*Initialize chip-specific functions for SER testing*/
        soc_th_ser_test_register(unit);
#endif /*defined(SER_TR_TEST_SUPPORT*/
    }

        /* Clear SER related fifo before enabling low priority interrupt */
        if (!SAL_BOOT_SIMULATION) {
        _soc_tomahawk3_ser_hw_clear(unit);
    }

    /* Turn on low priority interrupts for actual h/w and emulation */
    /* Note the individual modules should check if they will be working in
     * interrupt mode, and enable/disable module level interrupts accordingly
     */
    if ((!SAL_BOOT_SIMULATION) || (SAL_BOOT_QUICKTURN)) {
        if (soc_feature(unit, soc_feature_cmicx)) {
            soc_th3_lp_intr_register(unit);
        }
    }

    /* Configure Switch Latency Bypass Mode */
    bypass_valid = soc_property_get(unit, spn_SWITCH_BYPASS_MODE,
                                        SOC_SWITCH_BYPASS_MODE_NONE);
    if ((SOC_SWITCH_BYPASS_MODE_NONE != bypass_valid) &&
        (SOC_SWITCH_BYPASS_MODE_EFP != bypass_valid)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "Low latency and Balanced latency modes not"
                                "supported on BCM5698x \n")));
        return SOC_E_CONFIG;
    }
    SOC_IF_ERROR_RETURN(soc_th_latency_init(unit));

    /*Dev_port, Phyport, MMU port, IDB port mappings*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_port_mapping_init(unit));

    /* No HG port support */
    SOC_CONTROL_LOCK(unit);
    si->fabric_port_enable = 0;
    SOC_CONTROL_UNLOCK(unit);

    /* Check the speed classes, make sure only 4 speeds in the device */
    SOC_IF_ERROR_RETURN(soc_th3_speed_class_validate(unit));

    /*DV: TDM Code*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tdm_update(unit));

        /* Initialize IDB */
        /*DV: Integrate IDB init from DV*/
        rv = soc_tomahawk3_idb_init_update(unit);
        if (SOC_FAILURE(rv)){
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "FAILED in"
                                  "soc_tomahawk3_idb_init_update\n")));
            return rv;
        }

    /*Setup CPU_PBM and Loopback port bitmap*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_cpu_lb_portbitmap_set(unit));

    /*Setup port_type for HG/LB ports*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_hg_lb_port_type_set(unit));

#ifdef PLISIM
    if (SAL_BOOT_PLISIM) {
        /* done later, after port mapping is configured by ep_top. */
    } else
#endif
    {
        /*XLPORT init*/
        SOC_IF_ERROR_RETURN(_soc_tomahawk3_management_port_init(unit));
    }
    
    /*Enable refresh in MMU*/
     SOC_IF_ERROR_RETURN(soc_tomahawk3_en_mmu_refresh(unit));

    /* Enable dual hash tables */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_hash_init(unit));

    /*DV: integrate EP credit int and port enable from DV*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_ep_top_init(unit));

#if defined(PLISIM)
    if (SAL_BOOT_PLISIM) {
        /*XLPORT init*/
        SOC_IF_ERROR_RETURN(_soc_tomahawk3_management_port_init(unit));
    }
#endif /* PLISIM */

    /*Enable MAC after EP enable*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_cdport_init(unit));

    /*DV: Enable ingress forwarding */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_enable_ingress_forwarding(unit));


    /*Default programming required in IPEP*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_ipep_default_init(unit));



#if 0
    
    if (SAL_BOOT_QUICKTURN) {
        PBMP_PORT_ITER(unit, port) {
            if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
                continue;
            }
            SOC_IF_ERROR_RETURN
                (soc_tomahawk3_port_speed_update(unit, port,
                                                si->port_speed_max[port]));
        }
    }
#endif

    /*external,internal MDIO freq config*/
    SOC_IF_ERROR_RETURN(_soc_tomahawk3_cmic_mdio_freq_config(unit));

    /*FIXME:
    if (soc_mspi_init(unit) != SOC_E_NONE) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META_U(unit,
                             "unit %d : MSPI Init Failed\n"), unit));
    }*/
    _phy_tsce_firmware_set_helper[unit] = _soc_tomahawk3_tscx_firmware_set;
    _phy_tscf_firmware_set_helper[unit] = _soc_tomahawk3_tscx_firmware_set;


    SOC_IF_ERROR_RETURN
        (soc_counter_tomahawk_status_enable(unit, TRUE));

    /* Mem Scan thread start should be the last step in Misc init */
    if (parity_enable) {
        if ((!SAL_BOOT_SIMULATION || SAL_BOOT_QUICKTURN) &&
            !SOC_WARM_BOOT(unit)) {
            SOC_IF_ERROR_RETURN(soc_generic_sram_mem_scan_start(unit));
        }
    }

    /* Create a mutex for s/w re-ordering */
    SOC_IF_ERROR_RETURN(soc_tomahawk3_hash_mutex_init(unit));

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx) &&
        !(SAL_BOOT_PLISIM || SAL_BOOT_BCMSIM))
    {
        SOC_IF_ERROR_RETURN(_soc_th3_ledup_init(unit));
    }
#endif /* BCM_CMICX_SUPPORT */

#ifdef INCLUDE_AVS
    SOC_IF_ERROR_RETURN(soc_tomahawk3_avs_init(unit));
#endif /* INCLUDE_AVS  */

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        /* Update count of L2 entries */
        SOC_IF_ERROR_RETURN(_soc_th3_l2_mem_entry_count_update(unit));
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "MISC Init completed (u=%d)\n"), unit));
    return SOC_E_NONE;
}



/* Function to get the number of ports present in a given Port Macro */
int
soc_th3_ports_per_pm_get(int unit, int pm_id)
{
    soc_info_t *si;
    int phy_port_base, num_ports = 0;

    si = &SOC_INFO(unit);
    if (pm_id >= _TH3_PBLKS_PER_DEV) {
       return SOC_E_PARAM;
    }
    phy_port_base = 1 + (pm_id * _TH3_PORTS_PER_PBLK);

    if (si->port_p2l_mapping[phy_port_base] != -1) {
        num_ports = 1;
        if ((si->port_num_lanes[phy_port_base] == 2) &&
            (si->port_p2l_mapping[phy_port_base + 2] != -1)) {
            /* for cases when num_lanes is given in the config (40g).
             */
            num_ports = 2;
        }

        /* When phy_port_base + 1 is valid,
         *      All 4 ports of the PM(port macro) are valid.
         * When phy_port_base + 2 is valid,
         *      The PM is operating in Dual lane mode
         */
        if (si->port_p2l_mapping[phy_port_base + 1] != -1) {
            num_ports = 4;
            if (si->port_p2l_mapping[phy_port_base + 2] == -1) {
                num_ports = 2;
            }
        } else if (si->port_p2l_mapping[phy_port_base + 2] != -1) {
            num_ports = 2;
        }
    }
    return num_ports;
}

void
_soc_th3_lpm_view_set(int unit, int index, soc_mem_t view, int pair_lpm)
{
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    if (!_soc_th3_lpm_view_map[unit]) {
        return;
    }

    if (pair_lpm) {
        index = (index % tcam_depth) +
                (index / tcam_depth) * 2 * tcam_depth;
        _soc_th3_lpm_view_map[unit][index] = view;
        _soc_th3_lpm_view_map[unit][index + tcam_depth] = view;
    } else {
        _soc_th3_lpm_view_map[unit][index] = view;
    }
}

soc_mem_t
_soc_th3_lpm_view_get(int unit, int index, int pair_lpm)
{
    soc_mem_t view;
    int tcam_depth = SOC_L3_DEFIP_TCAM_DEPTH_GET(unit);

    if (!_soc_th3_lpm_view_map[unit]) {
        return INVALIDm;
    }

    if (pair_lpm) {
        index = (index % tcam_depth) +
                (index / tcam_depth) * 2 * tcam_depth;
        view = _soc_th3_lpm_view_map[unit][index];
        if (view == INVALIDm) {
            view = _soc_th3_lpm_view_map[unit][index + tcam_depth];
        }
    } else {
        view = _soc_th3_lpm_view_map[unit][index];
    }
    return view;
}

STATIC int
soc_tomahawk3_rqe_threshold_init(int unit) {

#define TH3_MMU_RQE_QUEUE_NUM 9
#define TH3_THDR_QE_CONFIG1_VALUE 0x7
#define TH3_THDR_QE_CONIFG_PRI_LIMIT 8
#define TH3_THDR_QE_LIMIT_MIN 7
#define TH3_MMU_THDR_QE_RESET_OFFSET 0xe
#define TH3_THDR_QE_SP_SHARED_LIMIT 0xaa9
#define TH3_THDR_QE_SP_RESUME_LIMIT 0xa9b

    soc_reg_t reg = INVALIDr;
    uint32 idx, rval;
    for(idx = 0; idx < TH3_MMU_RQE_QUEUE_NUM; idx++) {
        reg = MMU_THDR_QE_CONFIG1_PRIQr;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, TH3_THDR_QE_CONFIG1_VALUE));

        reg = MMU_THDR_QE_CONFIG_PRIQr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
        soc_reg_field_set(unit, reg, &rval, RESET_OFFSETf,
                                        TH3_MMU_THDR_QE_RESET_OFFSET);
        soc_reg_field_set(unit, reg, &rval, SHARED_LIMITf,
                                        TH3_THDR_QE_CONIFG_PRI_LIMIT);
        soc_reg_field_set(unit, reg, &rval, SHARED_ALPHAf,
                                        TH3_THDR_QE_CONIFG_PRI_LIMIT);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, rval));

        reg = MMU_THDR_QE_LIMIT_MIN_PRIQr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
        soc_reg_field_set(unit, reg, &rval, MIN_LIMITf, TH3_THDR_QE_LIMIT_MIN);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, rval));

        reg = MMU_THDR_QE_RESET_OFFSET_COLOR_PRIQr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
        soc_reg_field_set(unit, reg, &rval, RESET_OFFSET_YELLOWf,
                                           TH3_MMU_THDR_QE_RESET_OFFSET);
        soc_reg_field_set(unit, reg, &rval, RESET_OFFSET_REDf,
                                           TH3_MMU_THDR_QE_RESET_OFFSET);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, rval));
    }

    reg = MMU_THDR_QE_CONFIGr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
    soc_reg_field_set(unit, reg, &rval, LAST_PKT_ACCEPT_MODEf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, rval));

    reg = MMU_THDR_QE_CONFIG_SPr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
    soc_reg_field_set(unit, reg, &rval, SHARED_LIMITf,
                                    TH3_THDR_QE_SP_SHARED_LIMIT);
    soc_reg_field_set(unit, reg, &rval, RESUME_LIMITf,
                                    TH3_THDR_QE_SP_RESUME_LIMIT);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, rval));

    reg = MMU_THDR_QE_SHARED_COLOR_LIMIT_SPr;
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
    soc_reg_field_set(unit, reg, &rval, SHARED_LIMIT_YELLOWf,
                                    TH3_THDR_QE_SP_SHARED_LIMIT);
    soc_reg_field_set(unit, reg, &rval, SHARED_LIMIT_REDf,
                                    TH3_THDR_QE_SP_SHARED_LIMIT);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, rval));

    reg = MMU_THDR_QE_RESUME_COLOR_LIMIT_SPr;
    soc_reg_field_set(unit, reg, &rval, RESUME_LIMIT_YELLOWf,
                                          TH3_THDR_QE_SP_RESUME_LIMIT);
    soc_reg_field_set(unit, reg, &rval, RESUME_LIMIT_REDf,
                                          TH3_THDR_QE_SP_RESUME_LIMIT);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, rval));


    return SOC_E_NONE;
}

STATIC int
soc_tomahawk3_rqe_sched_shaper_init(int unit) {
    soc_reg_t reg = INVALIDr;
    uint32 idx, rval;
    uint64 rval_64;

    for(idx = 0; idx < TH3_MMU_RQE_QUEUE_NUM; idx++) {
        reg = MMU_RQE_MAX_SHAPER_ENr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
        soc_reg_field_set(unit, reg, &rval, VALUEf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, rval));

        reg = MMU_RQE_PRIORITY_WERR_WEIGHTr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, idx, &rval));
        soc_reg_field_set(unit, reg, &rval, COUNTf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY,
                                   idx, rval));
    }
    COMPILER_64_ZERO(rval_64);
    reg = MMU_RQE_SCH_INACTIVE_CONTROLr;
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, REG_PORT_ANY, 0, &rval_64));
    soc_reg64_field32_set(unit, reg, &rval_64, INTERVALf, 20);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, REG_PORT_ANY, 0, rval_64));


    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk3_mmu_init(int unit)
{
    int test_only, parity_en;
    SOC_INFO(unit).mmu_lossless = soc_property_get(unit, spn_MMU_LOSSLESS,
                                        SOC_TH3_MMU_LOSSLESS_DEFAULT_DISABLE);

    test_only = (SAL_BOOT_BCMSIM || SAL_BOOT_XGSSIM) ? TRUE : FALSE;
    parity_en = soc_property_get(unit, spn_PARITY_ENABLE, TRUE);

    SOC_IF_ERROR_RETURN(_soc_th3_post_mmu_init_update(unit));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_rqe_threshold_init(unit));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_rqe_sched_shaper_init(unit));
    /*Port speed encodings to be setup in MMU diuring init*/
    /*Can be re-used for port speed update as well*/
    SOC_IF_ERROR_RETURN(soc_tomahawk3_tm_port_speed_encoding(unit));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_cell_pointer_init(unit));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_mmu_port_rx_enables(unit));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_scheduler_init(unit));
    SOC_IF_ERROR_RETURN(soc_th3_mmu_config_init_thresholds(unit, test_only));
    SOC_IF_ERROR_RETURN(soc_th3_mmu_rqe_queue_number_init(unit));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_wred_init(unit));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_scb_init(unit));
    SOC_IF_ERROR_RETURN(soc_tomahawk3_rl_init(unit));
#ifdef INCLUDE_PSTATS
    soc_tomahawk3_mmu_pstats_tbl_config(unit);
#endif

    SOC_IF_ERROR_RETURN(soc_th3_pfc_rx_init_from_cfg(unit));
    SOC_IF_ERROR_RETURN(soc_th3_pfc_priority_to_pg_map_init_from_cfg(unit));
    if (parity_en) {
        SOC_IF_ERROR_RETURN(soc_th3_mmu_enable_non_func_intr(unit));
    }
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk3_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    soc_control_t *soc = SOC_CONTROL(unit);

    *enabled = soc->l2x_age_pid != SAL_THREAD_ERROR &&
               soc->l2x_age_enable ?
               1 : 0;
    *age_seconds = soc->l2x_age_pid != SAL_THREAD_ERROR &&
                   soc->l2x_age_enable ?
                   soc->l2x_age_interval : 0;

    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk3_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds = 0x7fffffff;
    return SOC_E_NONE;
}

STATIC int
_soc_tomahawk3_age_timer_set(int unit, int age_seconds, int enable)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    sal_usecs_t interval = soc->l2x_age_interval;

    if (soc->l2x_age_interval) {
        if (!enable) {
            soc->l2x_age_enable = 0;
        } else {
            if (age_seconds) {
                SOC_CONTROL_LOCK(unit);
                soc->l2x_age_interval = age_seconds;
                SOC_CONTROL_UNLOCK(unit);
            }
            soc->l2x_age_enable = 1;
            if (interval != age_seconds) {
                sal_sem_give(soc->l2x_age_notify);
            }
        }
    } else {
        if (enable) {
            soc->l2x_age_enable = 1;
            SOC_IF_ERROR_RETURN
                (soc_th3_l2_age_start(unit, age_seconds));
        }
    }

    return SOC_E_NONE;
}

soc_error_t
soc_th3_port_speed_supported(int unit, soc_port_t port, int speed)
{
    uint8 rev_id;
    uint16 dev_id;
    int num_lanes;
    soc_info_t *si = &SOC_INFO(unit);

    /*
     * If Universal calendar was not selected at init time then
     * disallow speed change to HG2 speeds.
     */
    if (FALSE == si->fabric_port_enable) {
        if (IS_HG2_SPEED(speed)) {
            /*
             * Intentionally returning SOC_E_CONFIG as this is a case
             * of incorrect configuration
             */
            return SOC_E_CONFIG;
        }
    }

    /*
     * Management ports only support 1G, 2.5G and 10G. Flexport
     * on management ports is not supported. Management ports
     * are guaranteed 10G bandwidth.
     */
    if (SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
        if ((1000 == speed) || (2500 == speed) || (10000 == speed)) {
            return SOC_E_NONE;
        } else {
            return SOC_E_PARAM;
        }
    }

    soc_cm_get_id(unit, &dev_id, &rev_id);
    num_lanes = si->port_num_lanes[port];
    
    /*
     * Check if Core Clock Frequency supports Port Speed
     */

    switch (speed) {
    /* 10G, 40G */
    case 10000:
    case 40000:
        break;

    /* 25G, 50G, 100G, 200G, 400G */
    case 25000:
    case 50000:
        if (si->frequency < 300) {
            return SOC_E_PARAM;
        }
        break;
    case 100000:
        if (si->frequency < 600) {
            return SOC_E_PARAM;
        }
        break;
    case 200000:
        if (si->frequency < 600) {
            return SOC_E_PARAM;
        }
        break;
    case 400000:
        if (si->frequency < 1200) {
            return SOC_E_PARAM;
        }
        break;

    default:
       return SOC_E_PARAM; 
    }

    /*
     * Laying down the framework to handle SKU differences. This is
     * needed because some SKUs have speed caps on certain PMs.
     */
    switch (dev_id) {
    case BCM56961_DEVICE_ID:
        /* SERDES VCO capped at 21.875Ghz */
        if ((25000 == speed) || (27000 == speed) || (speed >= 50000)) {
            return SOC_E_PARAM;
        }
        /* Fall through to default case */

    default:
        if (speed >= 400000) { /* 400G */
            if (8 != num_lanes) {
                return SOC_E_PARAM;
            }
        } else if (speed >= 200000) { /* 200G */
            if (4 != num_lanes) {
                return SOC_E_PARAM;
            }
        }else if (speed >= 100000) { /* 100G.  Account for 2x50G or 4x25G*/
            if ((4 != num_lanes) && (2 != num_lanes)) {
                return SOC_E_PARAM;
            }
        } else if (speed >= 50000) { /* 50G.  Account for 1 x 50G or 2 x 25G */
            if ((1 != num_lanes) && (2 != num_lanes)) {
                return SOC_E_PARAM;
            }
        } else if (speed >= 40000) { /* 40G 4 x 10G */
            if (4 != num_lanes && (2 != num_lanes)) {
                return SOC_E_PARAM;
            }
        } else if (speed >= 25000) { /* 25G */
            if (1 != num_lanes) {
                return SOC_E_PARAM;
            }
        } else { /* 10G */
            if (1 != num_lanes) {
                return SOC_E_PARAM;
            }
        }
    }
    return SOC_E_NONE;
}


soc_error_t
soc_th3_speed_set_init_ctrl(int unit, soc_th_port_lanes_t *lanes_ctrl)
{
#if 0
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t port_base = lanes_ctrl->port_base;

    lanes_ctrl->oversub = SOC_PBMP_MEMBER(si->oversub_pbm, port_base);

    if (lanes_ctrl->oversub) {
        SOC_IF_ERROR_RETURN(soc_tomahawk3_oversub_group_find(unit, lanes_ctrl));
    }
#endif

    return SOC_E_NONE;
}

void
soc_tomahawk3_temperature_intr(void *unit_vp, void *d1, void *d2,
                              void *d3, void *d4)
{
    uint32 rval, trval;
    int i, rv, unit = PTR_TO_INT(unit_vp);
    
    if ((rv = READ_TOP_PVTMON_INTR_STATUS_0r(unit, &rval)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "TH3 Temp Intr, Reg access error.\n")));
    }
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "High temp interrupt: 0x%08x\n"), rval));
    /* Raise event to app for it to take further graceful actions, ignore AVS */
    for (i = 0; i < COUNTOF(pvtmon_result_reg_th3)-1; i++) {
        if ((0x1 << i)  & _soc_th_temp_mon_mask[unit]) {
            if ((rv = soc_reg32_get(unit, pvtmon_result_reg_th3[i], REG_PORT_ANY, 0,
                                    &trval)) != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                                      "TH3 Temp Intr, Reg access error.\n")));
            }
            trval = soc_reg_field_get(unit, pvtmon_result_reg_th3[i], trval,
                                      PVT_DATAf);
            /* Convert data to temperature.
             * temp = 434.1-(data*0.5350) = (4341000-(trval*5350))/1000;
             * Note: Since this is a high temp interrupt we can safely assume
             * that this will end up being a +ive value.
             */            
            trval = (4341000-(trval*5350))/10000;

            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "TempMon %d: %d deg C.\n"), i, trval));
            (void)soc_event_generate(unit, SOC_SWITCH_EVENT_ALARM,
                                     SOC_SWITCH_EVENT_ALARM_HIGH_TEMP, i, trval);
        }
    }

    /* Acknowledge the interrupt */
    if ((rv = WRITE_TOP_PVTMON_INTR_STATUS_0r(unit, 0xFFFFFFFF)) != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "TH3 Temp Intr, Reg access error.\n")));
    }

    /* TH3 has HW overtemp protection. So no need to shutdown here. */

}

int
soc_tomahawk3_temperature_monitor_get(int unit, int temperature_max,
    soc_switch_temperature_monitor_t *temperature_array, int *temperature_count)
{
    soc_reg_t reg;
    int index, i;
    uint32 rval;
    int cur, peak, fval, cur_fval, peak_fval;
    int num_entries_out;

    if (temperature_count) {
        *temperature_count = 0;
    }
    if (COUNTOF(pvtmon_result_reg_th3) > temperature_max) {
        num_entries_out = temperature_max;
    } else {
        num_entries_out = COUNTOF(pvtmon_result_reg_th3) - 1; 
    }

    for (index = 0; index < num_entries_out; index++) {
        reg = pvtmon_result_reg_th3[index];

        cur_fval = 0;
        peak_fval = 0;

        for(i = 0; i < NUM_READS; i++) {
            SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

            cur_fval += soc_reg_field_get(unit, reg, rval, PVT_DATAf);
            peak_fval += soc_reg_field_get(unit, reg, rval, MIN_PVT_DATAf);
        }
        cur_fval /= NUM_READS;
        peak_fval /= NUM_READS;

        cur = (4341000-(cur_fval*5350))/1000;
        peak = (4341000-(peak_fval*5350))/1000;

        if (temperature_array) {
            (temperature_array + index)->curr = cur;
            (temperature_array + index)->peak = peak;
        }
    }
    SOC_IF_ERROR_RETURN(READ_AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr(unit, &rval));
    if (soc_reg_field_get(unit, AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr, rval, VALID_DATAf)) {
        fval = soc_reg_field_get(unit, AVS_REG_RO_REGISTERS_0_PVT_TEMPERATURE_MNTR_STATUSr, rval, DATAf);
        cur = peak = (4341000-(fval*5350))/1000;
        if (temperature_array) {
            (temperature_array + num_entries_out)->curr = cur;
            (temperature_array + num_entries_out)->peak = cur;
        }
    }

    /* TOP_PVT_MON_MIN corresponds to MAX temp. */ 

    for (index = 0; index < COUNTOF(pvtmon_ctrl_1_reg_th3); index++) {
        reg =  pvtmon_ctrl_1_reg_th3[index];
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, TOP_PVT_MON_MIN_RST_Lf, 0);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        soc_reg_field_set(unit, reg, &rval, TOP_PVT_MON_MIN_RST_Lf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    } 

    SOC_IF_ERROR_RETURN(READ_TOP_TMON_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_5_TOP_PVT_MON_MIN_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_6_TOP_PVT_MON_MIN_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_7_TOP_PVT_MON_MIN_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_8_TOP_PVT_MON_MIN_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_9_TOP_PVT_MON_MIN_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_10_TOP_PVT_MON_MIN_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_11_TOP_PVT_MON_MIN_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_12_TOP_PVT_MON_MIN_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TMON_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_5_TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_6_TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_7_TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_8_TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_9_TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_10_TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_11_TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_TMON_CTRL_1r, &rval, PVTMON_12_TOP_PVT_MON_MIN_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_TMON_CTRL_1r(unit, rval));

    SOC_IF_ERROR_RETURN(READ_TOP_PVTMON_13_17_CTRL_1r(unit, &rval));
    soc_reg_field_set(unit, TOP_PVTMON_13_17_CTRL_1r, &rval, PVTMON13_TOP_PVT_MON_MIN_RST_Lf, 0);
    soc_reg_field_set(unit, TOP_PVTMON_13_17_CTRL_1r, &rval, PVTMON14_TOP_PVT_MON_MIN_RST_Lf, 0);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_13_17_CTRL_1r(unit, rval));
    soc_reg_field_set(unit, TOP_PVTMON_13_17_CTRL_1r, &rval, PVTMON13_TOP_PVT_MON_MIN_RST_Lf, 1);
    soc_reg_field_set(unit, TOP_PVTMON_13_17_CTRL_1r, &rval, PVTMON14_TOP_PVT_MON_MIN_RST_Lf, 1);
    SOC_IF_ERROR_RETURN(WRITE_TOP_PVTMON_13_17_CTRL_1r(unit, rval));

    if (temperature_count) {
        /* *temperature_count = num_entries_out + 1; */
        *temperature_count = num_entries_out; /* Avoid displaying AVS for now */
    }
    return SOC_E_NONE;
}

int
soc_tomahawk3_show_ring_osc(int unit)
{
    static const struct {
        int osc_sel;
        soc_field_t field0;
        int value0;
        soc_field_t field1;
        int value1;
        char *name;
    } osc_tbl[] = {
        { 2, IROSC_SELf, 0, INVALIDf, -1, "Core ring 0 five stages" },
        { 2, IROSC_SELf, 1, INVALIDf, -1, "Core ring 0 nine stages" },
        { 3, IROSC_SELf, 0, INVALIDf, -1, "Core ring 1 five stages" },
        { 3, IROSC_SELf, 1, INVALIDf, -1, "Core ring 1 nine stages" },
        { 4, SRAM_OSC_0_PENf, 0, SRAM_OSC_0_NENf, 1, "SRAM ring 0 NMOS" },
        { 5, SRAM_OSC_0_PENf, 1, SRAM_OSC_0_NENf, 0, "SRAM ring 0 PMOS" },
        { 6, SRAM_OSC_1_PENf, 0, SRAM_OSC_1_NENf, 1, "SRAM ring 1 NMOS" },
        { 7, SRAM_OSC_1_PENf, 1, SRAM_OSC_1_NENf, 0, "SRAM ring 1 PMOS" },
        { 8, SRAM_OSC_2_PENf, 0, SRAM_OSC_2_NENf, 1, "SRAM ring 2 NMOS" },
        { 9, SRAM_OSC_2_PENf, 1, SRAM_OSC_2_NENf, 0, "SRAM ring 2 PMOS" },
        { 10, SRAM_OSC_3_PENf, 0, SRAM_OSC_3_NENf, 1, "SRAM ring 3 NMOS" },
        { 11, SRAM_OSC_3_PENf, 1, SRAM_OSC_3_NENf, 0, "SRAM ring 3 PMOS" }
    };
    soc_reg_t ctrl_reg, stat_reg;
    uint32 rval, fval;
    int index, core_clk, quo, rem, frac, retry;

    core_clk = SOC_INFO(unit).frequency * 1024;
    ctrl_reg = TOP_RING_OSC_CTRLr;
    stat_reg = TOP_OSC_COUNT_STATr;

    for (index = 0; index < COUNTOF(osc_tbl); index++) {
        rval = 0;
        /*
         * set OSC_CNT_RSTBf to 0 to do softreset
         * set OSC_CNT_START to 0 to hold the counter until it selects
         * the input signal
         */
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_ENABLEf, 1);
        soc_reg_field_set(unit, ctrl_reg, &rval, IROSC_ENf, 1);
        soc_reg_field_set(unit, ctrl_reg, &rval, osc_tbl[index].field0,
                          osc_tbl[index].value0);
        if (osc_tbl[index].field1 != INVALIDf) {
            soc_reg_field_set(unit, ctrl_reg, &rval, osc_tbl[index].field1,
                              osc_tbl[index].value1);
        }
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_SELf,
                          osc_tbl[index].osc_sel);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_CNT_RSTBf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));
        soc_reg_field_set(unit, ctrl_reg, &rval, OSC_CNT_STARTf, 1);
        SOC_IF_ERROR_RETURN(WRITE_TOP_RING_OSC_CTRLr(unit, rval));

        for (retry = 0; retry < 10; retry++) {
            sal_usleep(1000);
            SOC_IF_ERROR_RETURN(READ_TOP_OSC_COUNT_STATr(unit, &rval));
            if (!soc_reg_field_get(unit, stat_reg, rval, OSC_CNT_DONEf)) {
                continue;
            }

            fval = soc_reg_field_get(unit, stat_reg, rval, OSC_CNT_VALUEf);
            quo = core_clk / fval;
            rem = core_clk - quo * fval;
            frac = (rem * 10000) / fval;
            LOG_CLI((BSL_META_U(unit,
                                "%s: %d.%04d Mhz\n"),
                     osc_tbl[index].name, quo, frac));
            break;
        }
    }

    return SOC_E_NONE;
}

STATIC _soc_th3_bst_hw_cb th3_bst_cb;

int
soc_th3_process_mmu_bst(int unit, int itm, soc_field_t field)
{
    int rv = SOC_E_NONE;
    if (th3_bst_cb != NULL) {
        rv = th3_bst_cb(unit);
    }
    return rv;
}

int soc_th3_set_bst_callback(int unit, _soc_th_bst_hw_cb cb)
{
    th3_bst_cb = cb;
    return SOC_E_NONE;
}
void
soc_tomahawk3_process_func_intr(void *unit_vp, void *d1, void *d2, void *d3, void *d4)
{
    int rv = SOC_E_NONE;
    int unit = PTR_TO_INT(unit_vp);
    uint32 rval;
    char       prefix_str[10];
    sal_sprintf(prefix_str, "Unit: %d \n", unit);

    rv = READ_L2_MGMT_INTRr(unit, &rval);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit, "Error reading %s reg !!\n"),
                              SOC_REG_NAME(unit, L2_MGMT_INTRr)));
        return;
    }

    if (soc_reg_field_get(unit, L2_MGMT_INTRr, rval, SER_FIFO_NOT_EMPTYf)) {
        (void) soc_tomahawk_process_ser_fifo(unit, SOC_BLK_IPIPE,
                                             0, /* pipe */
                                             prefix_str,
                                             1, 0, 0); /* l2_mgmt_ser_fifo */
    }

    if (soc_feature(unit, soc_feature_l2_overflow)) {
        if (soc_reg_field_get(unit, L2_MGMT_INTRr, rval, L2_LEARN_INSERT_FAILUREf)) {
            soc_td2_l2_overflow_interrupt_handler(unit);
        }
    }

    sal_usleep(SAL_BOOT_QUICKTURN ? 100000 : 1000); /* Don't reenable too soon */
    if (d1 != NULL) {
#ifdef BCM_CMICM_SUPPORT
        if (soc_feature(unit, soc_feature_cmicm)) {
            (void) soc_cmicm_intr3_enable(unit, PTR_TO_INT(d1));
        }
#endif
#ifdef BCM_CMICX_SUPPORT
        if (soc_feature(unit, soc_feature_cmicx)) {
            
        }
#endif
    }
}

soc_pstats_tbl_desc_t
_soc_th3_pstats_tbl_desc_all = {SOC_BLK_NONE, INVALIDm, FALSE, FALSE};
soc_pstats_tbl_desc_t _soc_th3_pstats_tbl_desc[] = {
    /* List all the potential memories */
    {
        SOC_BLK_MMU_ITM,
        MMU_THDO_BST_TOTAL_QUEUE_PKTSTATm, /* MMU_THDO_BST_TOTAL_QUEUEm,*/
        FALSE, TRUE
    }
};

void
soc_tomahawk3_mmu_pstats_tbl_config_all(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_pstats_mem_desc_t *desc, *desc_all;
    int ti, mi, di = 0;

    desc_all = _soc_th3_pstats_tbl_desc_all.desc;
    /* Check and strip invalid memories */
    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        mi = 0;
        desc = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].desc;
        while (desc[mi].mem != INVALIDm) {
            desc_all[di] = desc[mi];
            di++;
            mi++;
        }
    }
    desc_all[di].mem = INVALIDm;

    soc->pstats_tbl_desc = &_soc_th3_pstats_tbl_desc_all;
    soc->pstats_tbl_desc_count = 1;
}

void
soc_tomahawk3_mmu_pstats_tbl_config(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_info_t *si = &SOC_INFO(unit);
    soc_pstats_mem_desc_t *desc;
    soc_mem_t bmem;
    int pe, mor_dma;
    int ti, mi, itm, pipe;
    uint32 itm_map;
    uint32 pipe_map;

    soc->pstats_tbl_desc = _soc_th3_pstats_tbl_desc;
    soc->pstats_tbl_desc_count = COUNTOF(_soc_th3_pstats_tbl_desc);
    soc->pstats_mode = PSTATS_MODE_NULL;

    itm_map = si->itm_map;
    soc_tomahawk3_pipe_map_get(unit, &pipe_map);
    /* Check and strip invalid memories */
    for (ti = 0; ti < soc->pstats_tbl_desc_count; ti++) {
        mi = 0;
        desc = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].desc;
        bmem = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].bmem;
        if (bmem == INVALIDm) {
            while (desc[mi].mem != INVALIDm) {
                desc[mi].width = soc_mem_entry_words(unit, desc[mi].mem);
                desc[mi].entries = soc_mem_index_count(unit, desc[mi].mem);
                mi++;
            }
            continue;
        }
        pe = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].pipe_enum;
        mor_dma = ((soc_pstats_tbl_desc_t *)soc->pstats_tbl_desc)[ti].mor_dma;
        for (itm = 0; itm < NUM_ITM(unit); itm++) {
            if (!(itm_map & (1U << itm))) {
                continue;
            }
            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                if (!(pipe_map & (1 << pipe))) {
                    continue;
                }
                desc[mi].mem = pe ? SOC_MEM_UNIQUE_ACC_ITM_PIPE(unit, bmem, itm, pipe) :
                                    SOC_MEM_UNIQUE_ACC(unit, bmem)[itm];
                if (desc[mi].mem == INVALIDm) {
                    continue;
                }
                desc[mi].width = soc_mem_entry_words(unit, desc[mi].mem);
                desc[mi].entries = soc_mem_index_count(unit, desc[mi].mem);
                if (mor_dma) {
                    desc[mi].mor_dma = TRUE;
                }
                mi++;
                if (!pe) {
                    break;
                }
            }
        }
        desc[mi].mem = INVALIDm;
    }

    if (soc_property_get(unit, "pstats_desc_all", 1)) {
        /* Use single desc chain */
        soc_tomahawk3_mmu_pstats_tbl_config_all(unit);
    }
}

int
soc_tomahawk3_mmu_pstats_mode_set(int unit, uint32 flags)
{
#define _SOC_MMU_PSTATS_BLK 2
    soc_control_t *soc = SOC_CONTROL(unit);
    int i, hwm_mode, cor_en;
    int pf = 0;
    uint32 rval;
    soc_reg_t reg = INVALIDr;
    soc_reg_t blk_regs[_SOC_MMU_PSTATS_BLK] =
                {MMU_THDO_BST_CONFIGr, MMU_THDI_BSTCONFIGr};
    soc_field_t cor_fields[] = {HWM_CORf, HW_CORf};
    soc_field_t mode_fields[] = {TRACKING_MODEf, TRACKING_MODEf};
    soc_field_t track_fields[] = {BST_TRACK_EN_THDO0f, BST_TRACK_EN_THDO1f,
                                BST_TRACK_EN_THDI0f, BST_TRACK_EN_THDI1f};
    SOC_PSTATS_LOCK(unit);

    hwm_mode = cor_en = 0;
    if (flags & _TH3_MMU_PSTATS_HWM_MOD) {
        hwm_mode = 1;
        pf |= _TH3_MMU_PSTATS_HWM_MOD;
        if (flags & _TH3_MMU_PSTATS_RESET_ON_READ) {
            cor_en = 1;
            pf |= _TH3_MMU_PSTATS_RESET_ON_READ;
        }
    }
    for (i = 0; i < _SOC_MMU_PSTATS_BLK; i++) {
        reg = blk_regs[i];
        rval = 0;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        soc_reg_field_set(unit, reg, &rval, mode_fields[i], hwm_mode);
        soc_reg_field_set(unit, reg, &rval, cor_fields[i], cor_en);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }
    if (flags & _TH3_MMU_PSTATS_ENABLE) {
        pf |= _TH3_MMU_PSTATS_ENABLE;
        if ((flags & _TH3_MMU_PSTATS_SYNC) ||
                (flags & _TH3_MMU_PSTATS_HWM_MOD &&
                 flags & _TH3_MMU_PSTATS_RESET_ON_READ)) {
            soc->pstats_mode = PSTATS_MODE_PKT_SYNC;
            pf |= _TH3_MMU_PSTATS_SYNC;
        }
        /* enable BST tracking */
        rval = 0;
        reg = MMU_GLBCFG_BST_TRACKING_ENABLEr;
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        for (i = 0; i < _SOC_MMU_PSTATS_BLK; i++) {
            soc_reg_field_set(unit, reg, &rval, track_fields[2 * i], 1);
            soc_reg_field_set(unit, reg, &rval, track_fields[2 * i + 1], 1);
        }
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        /* disable snapshot */
        rval = 0;
        reg = MMU_GLBCFG_BST_SNAPSHOT_ACTION_ENr;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

    } else {
       soc->pstats_mode = PSTATS_MODE_NULL;
    }
    soc->pstats_flags = pf;
    SOC_PSTATS_UNLOCK(unit);
#undef _SOC_MMU_PSTATS_BLK
    return SOC_E_NONE;
}

int
soc_tomahawk3_mmu_pstats_mode_get(int unit, uint32 *flags)
{
    int pf = 0;
    uint32 rval = 0;
    int rv;
    soc_reg_t reg = INVALIDr;
    soc_field_t track_fields[] = {BST_TRACK_EN_THDO0f, BST_TRACK_EN_THDO1f,
                                BST_TRACK_EN_THDI0f, BST_TRACK_EN_THDI1f};

    SOC_PSTATS_LOCK(unit);

    rv = READ_MMU_GLBCFG_BST_TRACKING_ENABLEr(unit, &rval);

    if (SOC_FAILURE(rv)) {
        SOC_PSTATS_UNLOCK(unit);
        return rv;
    }

    /* If one of the BST thresholds is enabled then PSTATS
       is enabled */
    reg = MMU_GLBCFG_BST_TRACKING_ENABLEr;
    if (soc_reg_field_get(unit, reg, rval, track_fields[0]) ||
        soc_reg_field_get(unit, reg, rval, track_fields[1]) ||
        soc_reg_field_get(unit, reg, rval, track_fields[2]) ||
        soc_reg_field_get(unit, reg, rval, track_fields[3])) {
        pf |= _TH3_MMU_PSTATS_ENABLE;
    }

    reg = MMU_THDI_BSTCONFIGr;
    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    if (soc_reg_field_get(unit, reg, rval, TRACKING_MODEf)) {
        pf |= _TH3_MMU_PSTATS_HWM_MOD;
        if (soc_reg_field_get(unit, reg, rval, HW_CORf)) {
            pf |= _TH3_MMU_PSTATS_RESET_ON_READ;
        }
    }

    reg = MMU_THDO_BST_CONFIGr;
    rval = 0;
    SOC_IF_ERROR_RETURN(
        soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

    if (soc_reg_field_get(unit, reg, rval, TRACKING_MODEf)) {
        pf |= _TH3_MMU_PSTATS_HWM_MOD_EGR;
        if (soc_reg_field_get(unit, reg, rval, HWM_CORf)) {
            pf |= _TH3_MMU_PSTATS_RESET_ON_READ_EGR;
        }
    }

    SOC_PSTATS_UNLOCK(unit);
    *flags = pf;

    return SOC_E_NONE;
}

int
soc_tomahawk3_mmu_splitter_reset(int unit)
{
    uint32 rval;

    SOC_IF_ERROR_RETURN(READ_MMU_GLBCFG_SPLITTERr(unit, &rval));
    soc_reg_field_set(unit, MMU_GLBCFG_SPLITTERr, &rval, RESET_SBUSf, 1);
    SOC_IF_ERROR_RETURN(WRITE_MMU_GLBCFG_SPLITTERr(unit, rval));

    return SOC_E_NONE;
}

int
soc_th3_mmu_non_ser_intr_handler(int unit, soc_block_t blocktype,
                                 int mmu_base_index,
                                 uint32 rval_intr_status_reg)
{
    int i;
    soc_reg_t intr_stat_reg/*, intr_clr_reg*/;
    static soc_field_t itm_intr_field_list[] = {
        CRB_INT_STATf,
        THDI_INT_STATf,
        THDR_INT_STATf,
        CFAP_INT_STATf,
        RQE_INT_STATf,
        THDO_INT_STATf,
        INVALIDf
    };
    static soc_field_t eb_intr_field_list[] = {
        EBCTM_INT_STATf,
        EBCFP_INT_STATf,
        MTRO_INT_STATf,
        INVALIDf
    };

    if (blocktype == SOC_BLK_MMU_ITM) {
        intr_stat_reg = MMU_ITMCFG_CPU_INT_STATr;
        /* SW is allowed to clear an interrupt by writing 1'b1 to that status bit. */
        /*intr_clr_reg = MMU_ITMCFG_CPU_INT_STATr;*/
        for (i = 0; itm_intr_field_list[i] != INVALIDf; i++) {
            if (!soc_reg_field_get(unit, intr_stat_reg,
                                   rval_intr_status_reg,
                                   itm_intr_field_list[i])) {
                continue;
            }
            switch (itm_intr_field_list[i]) {
            case CFAP_INT_STATf:
                SOC_IF_ERROR_RETURN(soc_th3_process_itm_cfap_int(unit,
                            mmu_base_index, itm_intr_field_list[i]));
                break;
            case THDI_INT_STATf:
                SOC_IF_ERROR_RETURN(soc_th3_process_itm_thdi_int(unit,
                            mmu_base_index, itm_intr_field_list[i]));
                break;
            case THDO_INT_STATf:
                SOC_IF_ERROR_RETURN(soc_th3_process_itm_thdo_int(unit,
                            mmu_base_index, itm_intr_field_list[i]));
                break;
            case THDR_INT_STATf:
                SOC_IF_ERROR_RETURN(soc_th3_process_itm_thdr_int(unit,
                            mmu_base_index, itm_intr_field_list[i]));
                break;
            case RQE_INT_STATf:
                SOC_IF_ERROR_RETURN(soc_th3_process_itm_rqe_int(unit,
                            mmu_base_index, itm_intr_field_list[i]));
                break;
            case CRB_INT_STATf:
                SOC_IF_ERROR_RETURN(soc_th3_process_itm_crb_int(unit,
                            mmu_base_index, itm_intr_field_list[i]));
                break;
            default:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit, "Unit: %0d -- Could not service %s "
                                            "intr from itm = %0d \n"),
                           unit, SOC_FIELD_NAME(unit, itm_intr_field_list[i]),
                           mmu_base_index));
                break;
            }
        }
        return SOC_E_NONE;
    }

    if (blocktype == SOC_BLK_MMU_EB) {
        intr_stat_reg = MMU_EBCFG_CPU_INT_STATr;
        /* SW is allowed to clear an interrupt by writing 1'b1 to that status bit. */
        /*intr_clr_reg = MMU_EBCFG_CPU_INT_STATr;*/
        for (i = 0; eb_intr_field_list[i] != INVALIDf; i++) {
            if (!soc_reg_field_get(unit, intr_stat_reg,
                                   rval_intr_status_reg,
                                   eb_intr_field_list[i])) {
                continue;
            }
            switch (eb_intr_field_list[i]) {
                case EBCTM_INT_STATf:
                    SOC_IF_ERROR_RETURN(
                        soc_th3_process_eb_ebctm_int(unit, mmu_base_index,
                            eb_intr_field_list[i]));
                    break;
                case EBCFP_INT_STATf:
                    SOC_IF_ERROR_RETURN(
                        soc_th3_process_eb_ebcfp_int(unit, mmu_base_index,
                            eb_intr_field_list[i]));
                    break;
                case MTRO_INT_STATf:
                    SOC_IF_ERROR_RETURN(
                        soc_th3_process_eb_mtro_int(unit, mmu_base_index,
                            eb_intr_field_list[i]));
                    break;
                default:
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META_U(unit, "Unit: %0d -- Could not service %s "
                                                "intr from sc = %0d \n"),
                               unit, SOC_FIELD_NAME(unit, eb_intr_field_list[i]),
                               mmu_base_index));
                    break;
            }
        }
        return SOC_E_NONE;
    }

    /* cannot be here for any other blocktype */
    return SOC_E_FAIL;
}

/*
 * Function:
 *      soc_th3_skip_obm_mem_if_pm_down
 * Purpose:
 *      Checks the passed memory. If it's of type
 *      IDB_OBM*_IOM_STATS_WINDOW_RESULTS*, check to see if the PM it is
 *      associated with is disabled.  If the PM is disabled, tell the calling
 *      function to skip reading the memory
 * Parameters:
 *      unit        - (IN) Unit number.
 *      mem         - (IN) The mem to check
 *      skip        - (OUT) Pointer to tell whether the memory should be
 *                          skipped
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_th3_skip_obm_mem_if_pm_down(int unit, soc_mem_t mem, int *skip)
{
    int i, j, pm_num;
    uint32 rval;
    soc_reg_t reg = TOP_TSC_ENABLEr;
    soc_field_t reg_field[_TH3_PBLKS_PER_DEV] = {
        TSC_0_ENABLEf, TSC_1_ENABLEf, TSC_2_ENABLEf, TSC_3_ENABLEf,
        TSC_4_ENABLEf, TSC_5_ENABLEf, TSC_6_ENABLEf, TSC_7_ENABLEf,
        TSC_8_ENABLEf, TSC_9_ENABLEf, TSC_10_ENABLEf, TSC_11_ENABLEf,
        TSC_12_ENABLEf, TSC_13_ENABLEf, TSC_14_ENABLEf, TSC_15_ENABLEf,
        TSC_16_ENABLEf, TSC_17_ENABLEf, TSC_18_ENABLEf, TSC_19_ENABLEf,
        TSC_20_ENABLEf, TSC_21_ENABLEf, TSC_22_ENABLEf, TSC_23_ENABLEf,
        TSC_24_ENABLEf, TSC_25_ENABLEf, TSC_26_ENABLEf, TSC_27_ENABLEf,
        TSC_28_ENABLEf, TSC_29_ENABLEf, TSC_30_ENABLEf, TSC_31_ENABLEf
    };

    soc_mem_t mem_it, mem_it_unique;
    soc_mem_t iom_stats_window_results[] =
                    { IDB_OBM0_IOM_STATS_WINDOW_RESULTSm,
                      IDB_OBM1_IOM_STATS_WINDOW_RESULTSm,
                      IDB_OBM2_IOM_STATS_WINDOW_RESULTSm,
                      IDB_OBM3_IOM_STATS_WINDOW_RESULTSm };


    if (NULL == skip) {
        return SOC_E_PARAM;
    }
    /* start with skip == 0 */
    *skip = 0;

    /* Check if the memory passed is one that we want to check */
    for (i = 0; i < COUNTOF(iom_stats_window_results); i++) {
        /* Must be in the range of the starting memory, plus all the
         * individual ones for each pipe */
        if (mem >= iom_stats_window_results[i] &&
            mem <= iom_stats_window_results[i] + _TH3_PIPES_PER_DEV) {
            break;
        }
    }

    /* return if not in range */
    if (i == COUNTOF(iom_stats_window_results)) {
        return SOC_E_NONE;
    }

    for (i = 0; i < _TH3_PIPES_PER_DEV; i++) {
        for (j = 0; j < _TH3_PBLKS_PER_PIPE; j++) {
            /* Get the current port block index */
            mem_it = iom_stats_window_results[j];

            /* Get the port block index for the pipe */
            mem_it_unique = SOC_MEM_UNIQUE_ACC(unit, mem_it)[i];


            /* if the memory is the passed one, check if the port
             * macro is disabled */
            if (mem_it == mem || mem_it_unique == mem) {
                /* get the pm number associated with this mem */
                pm_num = (i * _TH3_PBLKS_PER_PIPE) + j;

                SOC_IF_ERROR_RETURN(
                        soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));

                /* check if the PM is disabled */
                if (!soc_reg_field_get(unit, reg, rval, reg_field[pm_num])) {
                    /* if disabled, skip this memory */
                    *skip = 1;
                    return SOC_E_NONE;
                }
            }
        }
    }
    return SOC_E_NONE;
}

int
soc_th3_from_soc_attach(int unit, int val)
{
    from_soc_attach = val;

    return SOC_E_NONE;
}

/*
 * Tomahawk chip driver functions.
 * Pls keep at the end of this file for easy access.
 */
soc_functions_t soc_tomahawk3_drv_funs = {
    _soc_tomahawk3_misc_init,
    _soc_tomahawk3_mmu_init,
    _soc_tomahawk3_age_timer_get,
    _soc_tomahawk3_age_timer_max_get,
    _soc_tomahawk3_age_timer_set,
    _soc_tomahawk3_tscx_firmware_set,
    _soc_tomahawk3_tscx_reg_read,
    _soc_tomahawk3_tscx_reg_write,
    soc_tomahawk3_bond_info_init
};
#endif /* BCM_TOMAHAWK3_SUPPORT */

