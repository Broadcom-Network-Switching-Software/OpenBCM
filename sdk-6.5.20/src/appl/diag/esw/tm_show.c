/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diag CLI TMCheck command
 */

#include <soc/defs.h>
#if defined(BCM_TOMAHAWK3_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <shared/bsl.h>
#include <bcm/error.h>

#include <soc/tomahawk3.h>
#include <soc/init/tomahawk3_idb_init.h>

#define _TH3_EGR_Q_RESUME_LMT_GRAN 8
#define _TH3_EGR_Q_DROP_STATE_OFFSET_GREEN 2
#define UINT64_HEXA_STR_MAX_SIZE 20
#define _TH3_IDB_THD_GRAN 64
#define _TH3_NUM_MTR_GRAN 8

#ifdef __KERNEL__
#define atoi _shr_ctoi
#endif


char cmd_tm_show_usage[] =
    "  tmshow ing_buf_debug <port> <pri_grp> \n"
    "  tmshow ing_buf_debug <port> <queue> \n";


typedef enum _soc_th3_ing_config_type_e {
    _SOC_TH3_ING_INPPRI_PROFILE = 0,
    _SOC_TH3_ING_PG_PROFILE,
    _SOC_TH3_ING_ERROR
} _soc_th3_ing_config_type_t;

STATIC int
_soc_th3_ing_port_cfg_profile_get (int unit, int port,
        _soc_th3_ing_config_type_t th3_ing_cfg, int *profile) {
    soc_reg_t port_prof_reg = MMU_THDI_ING_PORT_CONFIGr;
    soc_field_t port_prof_fld;
    uint32 rval = 0;

    switch(th3_ing_cfg) {
        case _SOC_TH3_ING_INPPRI_PROFILE:
            port_prof_fld = INPPRI_PROFILE_SELf;
            break;
        case _SOC_TH3_ING_PG_PROFILE:
            port_prof_fld = PG_PROFILE_SELf;
            break;
        default:
            port_prof_fld = INVALIDf;
            break;
    }
    if (port_prof_fld != INVALIDf) {
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, port_prof_reg, port, 0, &rval));
        *profile = soc_reg_field_get(unit, port_prof_reg, rval, port_prof_fld);
        return SOC_E_NONE;
    }
    return SOC_E_INTERNAL;
}

static const  soc_field_t pg_sp_flds[_TH3_MMU_NUM_PG] = {PG0_SPIDf, PG1_SPIDf,
    PG2_SPIDf, PG3_SPIDf, PG4_SPIDf, PG5_SPIDf, PG6_SPIDf, PG7_SPIDf};

static const  soc_field_t pg_hp_flds[_TH3_MMU_NUM_PG] = {PG0_HPIDf, PG1_HPIDf,
    PG2_HPIDf, PG3_HPIDf, PG4_HPIDf, PG5_HPIDf, PG6_HPIDf, PG7_HPIDf};

STATIC int
_soc_th3_ing_pg_pools_get(int unit, int port, int pg, int *spid, int *hpid) {
    int port_profile;
    uint64 rval64;
    soc_reg_t pg_pool_reg = MMU_THDI_PG_PROFILEr;

    SOC_IF_ERROR_RETURN
        (_soc_th3_ing_port_cfg_profile_get(unit, port,
                                           _SOC_TH3_ING_PG_PROFILE,
                                           &port_profile));
    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, pg_pool_reg, port_profile, 0, &rval64));
    *spid = soc_reg64_field32_get(unit, pg_pool_reg, rval64, pg_sp_flds[pg]);
    *hpid = soc_reg64_field32_get(unit, pg_pool_reg, rval64, pg_hp_flds[pg]);
    return SOC_E_NONE;
}

static const soc_field_t pg_min_limit_flds[_TH3_MMU_NUM_PG] = {PG0_MIN_LIMITf,
    PG1_MIN_LIMITf, PG2_MIN_LIMITf, PG3_MIN_LIMITf, PG4_MIN_LIMITf,
    PG5_MIN_LIMITf, PG6_MIN_LIMITf, PG7_MIN_LIMITf};

static const soc_field_t pg_min_en_flds[_TH3_MMU_NUM_PG] = {PG0_USE_PORTSP_MINf,
    PG1_USE_PORTSP_MINf, PG2_USE_PORTSP_MINf, PG3_USE_PORTSP_MINf,
    PG4_USE_PORTSP_MINf, PG5_USE_PORTSP_MINf, PG6_USE_PORTSP_MINf,
    PG7_USE_PORTSP_MINf};

static const soc_field_t pg_min_count_flds[_TH3_MMU_NUM_PG] = {PG0_MIN_COUNTf,
    PG1_MIN_COUNTf, PG2_MIN_COUNTf, PG3_MIN_COUNTf, PG4_MIN_COUNTf,
    PG5_MIN_COUNTf, PG6_MIN_COUNTf, PG7_MIN_COUNTf};

static const soc_field_t ing_portsp_min_limit_flds[_TH3_MMU_NUM_POOL] =
    {PORTSP0_MIN_LIMITf, PORTSP1_MIN_LIMITf, PORTSP2_MIN_LIMITf, PORTSP3_MIN_LIMITf};

static const soc_field_t ing_portsp_min_count_flds[_TH3_MMU_NUM_POOL] =
    {PORTSP0_MIN_COUNTf, PORTSP1_MIN_COUNTf, PORTSP2_MIN_COUNTf,
        PORTSP3_MIN_COUNTf};

STATIC int
_soc_th3_port_pg_min_debug_print(int unit, int port, int pg) {
    int pipe, mmu_local_port, enable;
    int limit_cells, usage_cells;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t usage_mem = MMU_THDI_PORT_PG_MIN_COUNTERm;
    soc_mem_t config_mem = MMU_THDI_PORT_PG_MIN_CONFIGm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t read_mem = INVALIDm;

    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];

    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, config_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    /*PG min vs portsp Min*/
    enable = !(soc_mem_field32_get(unit, read_mem, &entry, pg_min_en_flds[pg]));
    /*PG min limit*/
    limit_cells = soc_mem_field32_get(unit, read_mem, &entry,
            pg_min_limit_flds[pg]);

    /*PG min counter*/
    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, usage_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    usage_cells = soc_mem_field32_get(unit, read_mem, &entry,
            pg_min_count_flds[pg]);

    cli_out("PG Min:       Enable: %1d, Limit: %7d, Usage: %13d \n",
            enable, limit_cells, usage_cells);

    return SOC_E_NONE;
}

static const soc_field_t pg_hdrm_limit_flds[_TH3_MMU_NUM_PG] = {PG0_HDRM_LIMITf,
    PG1_HDRM_LIMITf, PG2_HDRM_LIMITf, PG3_HDRM_LIMITf, PG4_HDRM_LIMITf,
    PG5_HDRM_LIMITf, PG6_HDRM_LIMITf, PG7_HDRM_LIMITf};

static const soc_field_t pg_hdrm_count_flds[_TH3_MMU_NUM_PG] = {PG0_HDRM_COUNTf,
    PG1_HDRM_COUNTf, PG2_HDRM_COUNTf, PG3_HDRM_COUNTf, PG4_HDRM_COUNTf,
    PG5_HDRM_COUNTf, PG6_HDRM_COUNTf, PG7_HDRM_COUNTf};


STATIC int
_soc_th3_port_pg_hdrm_debug_print(int unit, int port, int pg) {
    int pipe, mmu_local_port;
    int limit_cells, usage_cells;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t usage_mem = MMU_THDI_PORT_PG_HDRM_COUNTERm;
    soc_mem_t config_mem = MMU_THDI_PORT_PG_HDRM_CONFIGm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t read_mem = INVALIDm;

    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];

    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, config_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    /* PG hdrm limit */
    limit_cells = soc_mem_field32_get(unit, read_mem, &entry,
            pg_hdrm_limit_flds[pg]);

    /* PG hdrm counter */
    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, usage_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    usage_cells = soc_mem_field32_get(unit, read_mem, &entry,
            pg_hdrm_count_flds[pg]);

    cli_out("PG Headroom:             Limit: %7d, Usage: %13d \n",
             limit_cells, usage_cells);

    return SOC_E_NONE;
}

STATIC int
_soc_th3_port_ing_portsp_min_debug_print(int unit, int port, int pg) {
    int pipe, mmu_local_port, enable, spid, hpid;
    int limit_cells, usage_cells;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t usage_mem = MMU_THDI_PORTSP_COUNTERm;
    soc_mem_t pg_config_mem = MMU_THDI_PORT_PG_MIN_CONFIGm;
    soc_mem_t config_mem = MMU_THDI_PORTSP_CONFIGm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t read_mem = INVALIDm;

    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];
    SOC_IF_ERROR_RETURN(
    _soc_th3_ing_pg_pools_get(unit, port, pg, &spid, &hpid));

    /*Port SP min enable*/
    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, pg_config_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    enable = (soc_mem_field32_get(unit, read_mem, &entry, pg_min_en_flds[pg]));


    /*Port SP min limit*/
    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, config_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    limit_cells = soc_mem_field32_get(unit, read_mem, &entry,
            ing_portsp_min_limit_flds[spid]);

    /*Port SP Min Count*/
    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, usage_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    usage_cells = soc_mem_field32_get(unit, read_mem, &entry,
            ing_portsp_min_count_flds[spid]);

    cli_out("PortSP Min:   Enable: %1d, Limit: %7d, Usage: %13d \n",
            enable, limit_cells, usage_cells);

    return SOC_E_NONE;
}

static const soc_field_t pg_shr_limit_flds[_TH3_MMU_NUM_PG] = {PG0_SHARED_LIMITf,
    PG1_SHARED_LIMITf, PG2_SHARED_LIMITf, PG3_SHARED_LIMITf, PG4_SHARED_LIMITf,
    PG5_SHARED_LIMITf, PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};

static const soc_field_t pg_shr_count_flds[_TH3_MMU_NUM_PG] = {PG0_SHARED_COUNTf,
    PG1_SHARED_COUNTf, PG2_SHARED_COUNTf, PG3_SHARED_COUNTf, PG4_SHARED_COUNTf,
    PG5_SHARED_COUNTf, PG6_SHARED_COUNTf, PG7_SHARED_COUNTf};

static const soc_field_t pg_shr_dyn_flds[_TH3_MMU_NUM_PG] = {PG0_SHARED_DYNAMICf,
    PG1_SHARED_DYNAMICf, PG2_SHARED_DYNAMICf, PG3_SHARED_DYNAMICf, PG4_SHARED_DYNAMICf,
    PG5_SHARED_DYNAMICf, PG6_SHARED_DYNAMICf, PG7_SHARED_DYNAMICf};

static const soc_field_t pg_reset_offset_flds[_TH3_MMU_NUM_PG] =
{PG0_RESET_OFFSETf, PG1_RESET_OFFSETf, PG2_RESET_OFFSETf, PG3_RESET_OFFSETf,
    PG4_RESET_OFFSETf, PG5_RESET_OFFSETf, PG6_RESET_OFFSETf, PG7_RESET_OFFSETf};

static const soc_field_t pg_reset_floor_flds[_TH3_MMU_NUM_PG] =
{PG0_RESET_FLOORf, PG1_RESET_FLOORf, PG2_RESET_FLOORf, PG3_RESET_FLOORf,
    PG4_RESET_FLOORf, PG5_RESET_FLOORf, PG6_RESET_FLOORf, PG7_RESET_FLOORf};

STATIC int
_soc_th3_port_pg_shr_debug_print(int unit, int port, int pg) {
    int pipe, mmu_local_port, dynamic;
    int limit_cells, usage_cells, reset_offset, reset_floor;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t usage_mem = MMU_THDI_PORT_PG_SHARED_COUNTERm;
    soc_mem_t config_mem = MMU_THDI_PORT_PG_SHARED_CONFIGm;
    soc_mem_t resume_mem = MMU_THDI_PORT_PG_RESUME_CONFIGm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t read_mem = INVALIDm;
    soc_reg_t xoff_state_reg = MMU_THDI_FLOW_CONTROL_XOFF_STATEr;
    soc_field_t xoff_state_field = PG_XOFF_FCf;
    uint32 rval = 0;
    int port_xoff = 0, pg_xoff = 0;

    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];

    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, config_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    /* PG share dynamic enable */
    dynamic = (soc_mem_field32_get(unit, read_mem, &entry, pg_shr_dyn_flds[pg]));
    /* PG shared limit */
    limit_cells = soc_mem_field32_get(unit, read_mem, &entry,
            pg_shr_limit_flds[pg]);

    /* PG shared counter */
    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, usage_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    usage_cells = soc_mem_field32_get(unit, read_mem, &entry,
            pg_shr_count_flds[pg]);

    /* PG XOFF state */
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, xoff_state_reg, port, 0, &rval));
    port_xoff = soc_reg_field_get(unit, xoff_state_reg, rval, xoff_state_field);
    if (port_xoff & (1 << pg)) {
        pg_xoff = 1;
    }

    /* PG Reset Offset */
    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, resume_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    reset_offset = soc_mem_field32_get(unit, read_mem, &entry,
            pg_reset_offset_flds[pg]);
    /* PG Reset Floor */
    reset_floor = soc_mem_field32_get(unit, read_mem, &entry,
            pg_reset_floor_flds[pg]);

    if (dynamic == 1) {
        cli_out("PG Shared:   Dynamic: %1d, Alpha: %7d, Usage: %13d Xoff State:"
                " %1d\n", dynamic, limit_cells, usage_cells, pg_xoff);
    } else {
        cli_out("PG Shared:   Dynamic: %1d, Limit: %7d, Usage: %13d Xoff State:"
                " %1d\n", dynamic, limit_cells, usage_cells, pg_xoff);
    }
    cli_out("                  ResumeOffset: %7d, Floor: %13d\n", reset_offset,
            reset_floor);
    return SOC_E_NONE;
}


static const soc_field_t ing_portsp_shr_limit_flds[_TH3_MMU_NUM_POOL] = {
    PORTSP0_SHARED_LIMITf, PORTSP1_SHARED_LIMITf, PORTSP2_SHARED_LIMITf,
    PORTSP3_SHARED_LIMITf};


static const soc_field_t ing_portsp_resume_limit_flds[_TH3_MMU_NUM_POOL] = {
    PORTSP0_RESUME_LIMITf, PORTSP1_RESUME_LIMITf, PORTSP2_RESUME_LIMITf,
    PORTSP3_RESUME_LIMITf};

static const soc_field_t ing_portsp_shr_count_flds[_TH3_MMU_NUM_POOL] = {
    PORTSP0_SHARED_COUNTf, PORTSP1_SHARED_COUNTf, PORTSP2_SHARED_COUNTf,
    PORTSP3_SHARED_COUNTf};


STATIC int
_soc_th3_ing_portsp_shr_debug_print(int unit, int port, int pg) {
    int pipe, mmu_local_port;
    int limit_cells, usage_cells, resume_lmt;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t usage_mem = MMU_THDI_PORTSP_COUNTERm;
    soc_mem_t config_mem = MMU_THDI_PORTSP_CONFIGm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t read_mem = INVALIDm;
    soc_reg_t limit_state_reg = MMU_THDI_PORT_LIMIT_STATESr;
    soc_field_t portsp_lmt = PORTSP_LIMIT_STATEf;
    uint32 rval;
    int spid, hpid, limit_state, sp_limit_state = 0;

    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];
    SOC_IF_ERROR_RETURN(
        _soc_th3_ing_pg_pools_get(unit, port, pg, &spid, &hpid));

    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, config_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    /* PortSP shared limit */
    limit_cells = soc_mem_field32_get(unit, read_mem, &entry,
            ing_portsp_shr_limit_flds[spid]);
    /* PortSP Reusme Limit */
    resume_lmt = soc_mem_field32_get(unit, read_mem, &entry,
            ing_portsp_resume_limit_flds[spid]);

    /* PortSP shared counter */
    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, usage_mem, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    usage_cells = soc_mem_field32_get(unit, read_mem, &entry,
            ing_portsp_shr_count_flds[spid]);

    /* PortSP Limit State */
    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit,  limit_state_reg, port, 0, &rval));
    limit_state = soc_reg_field_get(unit, limit_state_reg, rval, portsp_lmt);
    if(limit_state & (1 << spid)) {
        sp_limit_state = 1;
    }

    cli_out("PortSP Shared:           Limit: %7d, Usage: %13d, Limit Hit: %1d \n",
            limit_cells, usage_cells, sp_limit_state);
    cli_out("                   Reset Limit: %7d\n", resume_lmt);
    return SOC_E_NONE;
}

STATIC int
_soc_th3_ing_service_pool_print(int unit, int pool) {
    soc_reg_t sp_limit_reg = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
    soc_field_t sp_limit_fld = LIMITf;
    soc_reg_t sp_count_reg = MMU_THDI_POOL_SHARED_COUNT_SPr;
    soc_field_t sp_count_fld = TOTAL_BUFFER_COUNTf;
    soc_reg_t sp_reset_reg = MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr;
    soc_field_t sp_reset_offset_fld = OFFSETf;
    soc_reg_t sp_drop_state_reg = MMU_THDI_POOL_DROP_STATEr;
    soc_field_t sp_drop_state_flds[_TH3_MMU_NUM_POOL] = {POOL_0f, POOL_1f,
        POOL_2f, POOL_3f};
    uint32 rval = 0;
    int sp_limit, sp_count, sp_offset, drop_state, green_drop_state;
    if (pool >= _TH3_MMU_NUM_POOL) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sp_limit_reg, pool, 0, &rval));
    sp_limit = soc_reg_field_get(unit, sp_limit_reg, rval, sp_limit_fld);

    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sp_count_reg, pool, 0, &rval));
    sp_count = soc_reg_field_get(unit, sp_count_reg, rval, sp_count_fld);

    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sp_reset_reg , pool, 0, &rval));
    sp_offset = soc_reg_field_get(unit, sp_reset_reg, rval, sp_reset_offset_fld);

    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sp_drop_state_reg, 0, 0, &rval));
    drop_state = soc_reg_field_get(unit, sp_drop_state_reg, rval,
            sp_drop_state_flds[pool]);
    /* The 3 bits [2:0] are organized as {green, yellow, red} drop state */
    green_drop_state = (drop_state >> 2);

    cli_out("Ing Service Pool: %1d      Limit: %7d, Usage: %13d"
            " Drop State: %1d\n",
            pool, sp_limit, sp_count, green_drop_state);
    cli_out("Ing Service Pool   Reset Limit: %7d \n", (sp_limit - sp_offset));

    return SOC_E_NONE;
}

STATIC int
_soc_th3_ing_headroom_pool_print(int unit, int hdrm_pool) {

    soc_reg_t hp_lmt_reg = MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
    soc_field_t hp_lmt_fld = LIMITf;
    soc_reg_t hp_count_reg = MMU_THDI_HDRM_POOL_COUNT_HPr;
    soc_field_t hp_count_fld = TOTAL_BUFFER_COUNTf;
    soc_reg_t hp_state_reg = MMU_THDI_HDRM_POOL_STATUSr;
    soc_field_t hp_state_fld = HDRM_POOL_LIMIT_STATE_HPf;
    soc_reg_t hp_drop_reg = MMU_THDI_POOL_DROP_COUNT_HPr;
    soc_field_t hp_drop_fld = PACKET_DROP_COUNTf;
    uint32 rval = 0;
    int hp_limit, hp_count, hp_state;
    uint64 rval64, hp_drop;
    char tmp[UINT64_HEXA_STR_MAX_SIZE];

    if (hdrm_pool >= _TH3_MMU_NUM_POOL) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, hp_lmt_reg, hdrm_pool, 0, &rval));
    hp_limit = soc_reg_field_get(unit, hp_lmt_reg, rval, hp_lmt_fld);

    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, hp_count_reg, hdrm_pool, 0, &rval));
    hp_count = soc_reg_field_get(unit, hp_count_reg, rval, hp_count_fld);

    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, hp_state_reg, 0, 0, &rval));
    hp_state = soc_reg_field_get(unit, hp_state_reg, rval, hp_state_fld);

    COMPILER_64_ZERO(rval64);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, hp_drop_reg, hdrm_pool, 0, &rval64));
    hp_drop = soc_reg64_field_get(unit, hp_drop_reg, rval64, hp_drop_fld);
    _shr_format_uint64_hexa_string (hp_drop, tmp);

    cli_out("Headroom Pool: %1d         Limit: %7d, Usage: %13d Limit Hit:  %1d \n",
            hdrm_pool, hp_limit, hp_count,
            ((hp_state >> hdrm_pool) & 1));


    return SOC_E_NONE;
}

STATIC int
_soc_th3_pg_debug_print(int unit, int port, int pg) {
    int spid, hpid;

    if (pg >= _TH3_MMU_NUM_PG) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN
        (_soc_th3_ing_pg_pools_get(unit, port, pg, &spid, &hpid));
    cli_out("### Ingress Buffer Usage Debug Info for Port: %d, PriGrp: %d "
        "###\n", port, pg);
    SOC_IF_ERROR_RETURN
        (_soc_th3_port_pg_min_debug_print(unit, port, pg));
    SOC_IF_ERROR_RETURN
        (_soc_th3_port_pg_shr_debug_print(unit, port, pg));
    SOC_IF_ERROR_RETURN
        (_soc_th3_port_pg_hdrm_debug_print(unit, port, pg));
    SOC_IF_ERROR_RETURN
        (_soc_th3_port_ing_portsp_min_debug_print(unit, port, pg));
    SOC_IF_ERROR_RETURN
        (_soc_th3_ing_portsp_shr_debug_print(unit, port, pg));
    SOC_IF_ERROR_RETURN
        (_soc_th3_ing_service_pool_print(unit, spid));
    SOC_IF_ERROR_RETURN
        (_soc_th3_ing_headroom_pool_print(unit, hpid));

    return SOC_E_NONE;
}

static int
_soc_th3_cpu_queue_debug_print(int unit, int port, int queue) {
    int min_en, shr_en, shr_dyn;
    int min_limit, shr_limit, resume_offset;
    int min_usage, shr_usage;
    soc_mem_t q_limit_mem = MMU_THDO_QUEUE_CONFIGm;
    soc_mem_t q_resume_mem = MMU_THDO_QUEUE_RESUME_OFFSETm;
    soc_mem_t q_usage_mem = MMU_THDO_COUNTER_QUEUEm;
    soc_reg_t cpu_q_drop_regs[4] = {MMU_THDO_CPU_QUEUE_DROP_STATES_11_00r,
        MMU_THDO_CPU_QUEUE_DROP_STATES_23_12r,
        MMU_THDO_CPU_QUEUE_DROP_STATES_35_24r,
        MMU_THDO_CPU_QUEUE_DROP_STATES_47_36r};

    soc_field_t cpu_q_drop_flds[SOC_TH3_NUM_CPU_QUEUES] = {DS_QUEUE0f,
    DS_QUEUE1f, DS_QUEUE2f, DS_QUEUE3f, DS_QUEUE4f, DS_QUEUE5f, DS_QUEUE6f,
    DS_QUEUE7f, DS_QUEUE8f, DS_QUEUE9f, DS_QUEUE10f, DS_QUEUE11f,  DS_QUEUE12f,
    DS_QUEUE13f, DS_QUEUE14f, DS_QUEUE15f, DS_QUEUE16f, DS_QUEUE17f,
    DS_QUEUE18f, DS_QUEUE19f, DS_QUEUE20f, DS_QUEUE21f, DS_QUEUE22f,
    DS_QUEUE23f, DS_QUEUE24f, DS_QUEUE25f, DS_QUEUE26f, DS_QUEUE27f,
    DS_QUEUE28f, DS_QUEUE29f, DS_QUEUE30f, DS_QUEUE31f, DS_QUEUE32f,
    DS_QUEUE33f, DS_QUEUE34f, DS_QUEUE35f, DS_QUEUE36f, DS_QUEUE37f,
    DS_QUEUE38f, DS_QUEUE39f, DS_QUEUE40f, DS_QUEUE41f, DS_QUEUE42f,
    DS_QUEUE43f, DS_QUEUE44f, DS_QUEUE45f, DS_QUEUE46f, DS_QUEUE47f};
    uint32 entry[SOC_MAX_MEM_WORDS];
    int itm;
    soc_mem_t read_mem;
    int voq, drop_state_fld, drop_state;
    int q_drop_mem_idx;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 rval = 0;

    voq = si->port_cosq_base[port] + queue;
    q_drop_mem_idx = queue / SOC_TH3_NUM_GP_QUEUES;
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, q_limit_mem, MEM_BLOCK_ALL, voq, &entry));
    min_en = !(soc_mem_field32_get(unit, q_limit_mem, &entry, USE_QGROUP_MINf));
    shr_en = soc_mem_field32_get(unit, q_limit_mem, &entry, LIMIT_ENABLEf);
    shr_dyn = soc_mem_field32_get(unit, q_limit_mem, &entry, LIMIT_DYNAMICf);
    min_limit = soc_mem_field32_get(unit, q_limit_mem, &entry, MIN_LIMITf);
    shr_limit = soc_mem_field32_get(unit, q_limit_mem, &entry, SHARED_LIMITf);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, q_resume_mem, MEM_BLOCK_ALL, voq, &entry));
    resume_offset = _TH3_EGR_Q_RESUME_LMT_GRAN *
        soc_mem_field32_get(unit, q_resume_mem, &entry, RESUME_OFFSETf);

    cli_out("Queue Min:     Enable:%1d, Limit: %7d \n",
            min_en, min_limit);
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, q_usage_mem, itm);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, voq, &entry));
        min_usage = soc_mem_field32_get(unit, q_usage_mem, &entry, MIN_COUNTf);

        cli_out("                  ITM:%1d, Usage: %7d"
                "\n", itm, min_usage);
    }

    if (shr_dyn) {
        cli_out("Queue Shared:  Enable:%1d, Alpha: %7d, ResumeOffset: %7d \n",
            shr_en, shr_limit, resume_offset);
    } else {
        cli_out("Queue Shared:  Enable:%1d, Limit: %7d, Reset Limit:  %7d\n",
            shr_en, shr_limit, (shr_limit - resume_offset));
    }
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, q_usage_mem, itm);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, voq, &entry));
        shr_usage = soc_mem_field32_get(unit, q_usage_mem, &entry, SHARED_COUNTf);

        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_itm_reg32_get(unit, cpu_q_drop_regs[q_drop_mem_idx],
                                         itm, itm, 0, &rval));
        drop_state_fld = soc_reg_field_get(unit,
                cpu_q_drop_regs[q_drop_mem_idx], rval, cpu_q_drop_flds[queue]);
        drop_state = (drop_state_fld >> _TH3_EGR_Q_DROP_STATE_OFFSET_GREEN) & 1;
        cli_out("                  ITM:%1d, Usage: %7d, DropState:    %7d"
                "\n", itm, shr_usage, drop_state);
    }

    return SOC_E_NONE;

}
static const soc_field_t q_drop_flds[SOC_TH3_NUM_GP_QUEUES] = {DS_QUEUE0f,
    DS_QUEUE1f, DS_QUEUE2f, DS_QUEUE3f, DS_QUEUE4f, DS_QUEUE5f, DS_QUEUE6f,
    DS_QUEUE7f, DS_QUEUE8f, DS_QUEUE9f, DS_QUEUE10f, DS_QUEUE11f};
static int
_soc_th3_port_queue_limit_debug_print(int unit, int port, int queue)
{
    int min_en, shr_en, shr_dyn;
    int min_limit, shr_limit, resume_offset;
    int min_usage, shr_usage;
    soc_mem_t q_limit_mem = MMU_THDO_QUEUE_CONFIGm;
    soc_mem_t q_resume_mem = MMU_THDO_QUEUE_RESUME_OFFSETm;
    soc_mem_t q_usage_mem = MMU_THDO_COUNTER_QUEUEm;
    soc_mem_t q_drop_mem = MMU_THDO_PORT_Q_DROP_STATEm;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int itm;
    soc_mem_t read_mem;
    int voq, drop_state_fld, drop_state;
    soc_info_t *si = &SOC_INFO(unit);


    voq = si->port_uc_cosq_base[port] + queue;
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, q_limit_mem, MEM_BLOCK_ALL, voq, &entry));
    min_en = !(soc_mem_field32_get(unit, q_limit_mem, &entry, USE_QGROUP_MINf));
    shr_en = soc_mem_field32_get(unit, q_limit_mem, &entry, LIMIT_ENABLEf);
    shr_dyn = soc_mem_field32_get(unit, q_limit_mem, &entry, LIMIT_DYNAMICf);
    min_limit = soc_mem_field32_get(unit, q_limit_mem, &entry, MIN_LIMITf);
    shr_limit = soc_mem_field32_get(unit, q_limit_mem, &entry, SHARED_LIMITf);

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, q_resume_mem, MEM_BLOCK_ALL, voq, &entry));
    resume_offset = _TH3_EGR_Q_RESUME_LMT_GRAN *
        soc_mem_field32_get(unit, q_resume_mem, &entry, RESUME_OFFSETf);

    cli_out("Queue Min:     Enable:%1d, Limit: %7d \n",
            min_en, min_limit);
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, q_usage_mem, itm);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, voq, &entry));
        min_usage = soc_mem_field32_get(unit, q_usage_mem, &entry, MIN_COUNTf);
        cli_out("                  ITM:%1d, Usage: %7d"
                "\n", itm, min_usage);
    }

    if (shr_dyn) {
        cli_out("Queue Shared:  Enable:%1d, Alpha: %7d, ResumeOffset: %7d \n",
            shr_en, shr_limit, resume_offset);
    } else {
        cli_out("Queue Shared:  Enable:%1d, Limit: %7d, Reset Limit:  %7d\n",
            shr_en, shr_limit, (shr_limit - resume_offset));
    }
    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, q_usage_mem, itm);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, voq, &entry));
        shr_usage = soc_mem_field32_get(unit, q_usage_mem, &entry, SHARED_COUNTf);

        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, q_drop_mem, itm);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, voq, &entry));
        drop_state_fld = soc_mem_field32_get(unit, q_drop_mem, &entry,
                q_drop_flds[queue]);
        drop_state = (drop_state_fld >> _TH3_EGR_Q_DROP_STATE_OFFSET_GREEN) & 1;
        cli_out("                  ITM:%1d, Usage: %7d, DropState:    %7d"
                "\n", itm, shr_usage, drop_state);
    }

    return SOC_E_NONE;
}

static const soc_mem_t qgrp_mems[2][2] = {
    {MMU_THDO_CONFIG_UC_QGROUP0m, MMU_THDO_COUNTER_UC_QGROUPm},
    {MMU_THDO_CONFIG_MC_QGROUPm, MMU_THDO_COUNTER_MC_QGROUPm}};

static int
_soc_th3_port_qgroup_limit_debug_print(int unit, int port, int uc_mc, int
        qgrp_min_en) {
    soc_mem_t qgrp_cfg_mem = qgrp_mems[uc_mc][0];
    soc_mem_t qgrp_cntr_mem = qgrp_mems[uc_mc][1];
    int itm, min_lmt, min_usage;
    soc_mem_t read_mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int mmu_chip_port;

    mmu_chip_port = soc_th3_mmu_chip_port_num(unit, port);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, qgrp_cfg_mem, MEM_BLOCK_ALL, mmu_chip_port, &entry));
    min_lmt =
        soc_mem_field32_get(unit, qgrp_cfg_mem, &entry, MIN_LIMITf);
    cli_out("QGroup Min:    Enable:%1d, Limit: %7d \n",
            qgrp_min_en, min_lmt);

    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, qgrp_cntr_mem, itm);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_chip_port, &entry));
        min_usage = soc_mem_field32_get(unit, qgrp_cntr_mem, &entry, MIN_COUNTf);
        cli_out("                  ITM:%1d, Usage: %7d\n",
                itm, min_usage);
    }
    return SOC_E_NONE;
}

static const soc_field_t egr_portsp_lmt_flds[_TH3_MMU_NUM_POOL] =
{SP0_SHARED_LIMITf, SP1_SHARED_LIMITf, SP2_SHARED_LIMITf, SP3_SHARED_LIMITf};

static const soc_field_t egr_portsp_usage_flds[_TH3_MMU_NUM_POOL] =
{SP0_SHARED_COUNTf, SP1_SHARED_COUNTf, SP2_SHARED_COUNTf, SP3_SHARED_COUNTf};

static const soc_field_t egr_uc_portsp_drop_flds[_TH3_MMU_NUM_POOL] = {DS_SP0f,
    DS_SP1f, DS_SP2f, DS_SP3f};

static int
_soc_th3_egr_portsp_limit_debug_print(int unit, int port, int spid) {
    soc_mem_t portsp_cfg_mem = MMU_THDO_CONFIG_PORT_UC0m;
    soc_mem_t resume_mem = MMU_THDO_RESUME_PORT_UC0m;
    soc_mem_t portsp_cntr_mem = MMU_THDO_COUNTER_PORT_UCm;
    soc_mem_t drop_mem = MMU_THDO_PORT_SP_DROP_STATE_UCm;
    soc_mem_t read_mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int shr_lmt, itm, shr_usage;
    uint32 mmu_chip_port;
    int resume_index, resume_lmt, drop_state_fld, drop_state;

    mmu_chip_port = soc_th3_mmu_chip_port_num(unit, port);
    resume_index = (mmu_chip_port * _TH3_MMU_NUM_POOL) + spid;

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, portsp_cfg_mem, MEM_BLOCK_ALL, mmu_chip_port,
                      &entry));
    shr_lmt =
        soc_mem_field32_get(unit, portsp_cfg_mem, &entry,
                egr_portsp_lmt_flds[spid]);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, resume_mem, MEM_BLOCK_ALL, resume_index, &entry));
    resume_lmt =
        soc_mem_field32_get(unit, resume_mem, &entry, SHARED_RESUMEf);
    cli_out("PortSP Shared:           Limit: %7d, ResetLimit:   %7d \n",
            shr_lmt, (shr_lmt - resume_lmt));

    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, portsp_cntr_mem, itm);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_chip_port, &entry));
        shr_usage = soc_mem_field32_get(unit, portsp_cntr_mem, &entry,
                egr_portsp_usage_flds[spid]);

        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, drop_mem, itm);
                SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_chip_port, &entry));
        drop_state_fld = soc_mem_field32_get(unit, drop_mem, &entry,
                egr_uc_portsp_drop_flds[spid]);
        drop_state = (drop_state_fld >> _TH3_EGR_Q_DROP_STATE_OFFSET_GREEN) & 1;
        cli_out("                  ITM:%1d, Usage: %7d, DropState:    %7d\n",
                itm, shr_usage, drop_state);
    }

    return SOC_E_NONE;
}

static const soc_field_t egr_sp_drop_flds[_TH3_MMU_NUM_POOL] = {GREEN_SP0f,
    GREEN_SP1f, GREEN_SP2f, GREEN_SP3f};

static int
_soc_th3_egr_sp_limit_debug_print(int unit, int spid) {
    soc_reg_t sp_limit_reg = MMU_THDO_SHARED_DB_POOL_SHARED_LIMITr;
    soc_reg_t sp_usage_reg = MMU_THDO_SHARED_DB_POOL_SHARED_COUNTr;
    soc_reg_t sp_resume_limit_reg = MMU_THDO_SHARED_DB_POOL_RESUME_LIMITr;
    soc_reg_t sp_drop_state_reg = MMU_THDO_SHARED_DB_POOL_DROP_STATESr;
    uint32 rval = 0;
    int sp_limit, sp_resume;
    int itm, usage, drop_state;

    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sp_limit_reg, spid, 0, &rval));
    sp_limit = soc_reg_field_get(unit, sp_limit_reg, rval, SHARED_LIMITf);

    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sp_resume_limit_reg, spid, 0, &rval));
    sp_resume =
        soc_reg_field_get(unit, sp_resume_limit_reg, rval, RESUME_LIMITf);

    cli_out("ServicePool: %1d:          Limit: %7d, ResetLimit:   %7d\n",
            spid, sp_limit, (sp_limit - sp_resume));

    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_itm_reg32_get(unit, sp_usage_reg, itm, itm,
                                         spid, &rval));
        usage = soc_reg_field_get(unit, sp_usage_reg, rval, COUNTf);

        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_itm_reg32_get(unit, sp_drop_state_reg, itm, itm, 0,
                                         &rval));
        drop_state = soc_reg_field_get(unit, sp_drop_state_reg, rval,
                egr_sp_drop_flds[spid]);

        cli_out("                  ITM:%1d, Usage: %7d, DropState:    %7d\n",
                itm, usage, drop_state);

    }

    return SOC_E_NONE;
}


static int
_soc_th3_egr_mc_portsp_limit_debug_print(int unit, int port, int spid) {
    soc_mem_t portsp_cfg_mem = MMU_THDO_CONFIG_PORTSP_MCm;
    soc_mem_t resume_mem = MMU_THDO_RESUME_PORT_MC0m;
    soc_mem_t portsp_cntr_mem = MMU_THDO_COUNTER_PORTSP_MCm;
    soc_mem_t drop_mem = MMU_THDO_PORT_SP_DROP_STATE_MCm;
    soc_mem_t read_mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int shr_lmt, itm, shr_usage;
    uint32 mmu_chip_port;
    int mem_index, resume_lmt, drop_state_fld, drop_state;

    mmu_chip_port = soc_th3_mmu_chip_port_num(unit, port);
    mem_index = (mmu_chip_port * _TH3_MMU_NUM_POOL) + spid;

    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, portsp_cfg_mem, MEM_BLOCK_ALL, mem_index, &entry));
    shr_lmt =
        soc_mem_field32_get(unit, portsp_cfg_mem, &entry, SP_SHARED_LIMITf);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, resume_mem, MEM_BLOCK_ALL, mem_index, &entry));
    resume_lmt =
        soc_mem_field32_get(unit, resume_mem, &entry, SHARED_RESUMEf);
    cli_out("MC PortSP Shared:        Limit: %7d, ResetLimit:   %7d \n",
            shr_lmt, (shr_lmt - resume_lmt));

    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, portsp_cntr_mem, itm);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mem_index, &entry));
        shr_usage = soc_mem_field32_get(unit, portsp_cntr_mem, &entry,
                SHARED_COUNTf);

        read_mem = SOC_MEM_UNIQUE_ACC_ITM(unit, drop_mem, itm);
        SOC_IF_ERROR_RETURN
            (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_chip_port, &entry));
        drop_state_fld = soc_mem_field32_get(unit, drop_mem, &entry,
                egr_uc_portsp_drop_flds[spid]);
        drop_state = (drop_state_fld >> _TH3_EGR_Q_DROP_STATE_OFFSET_GREEN) & 1;
        cli_out("                  ITM:%1d, Usage: %7d, DropState:    %7d\n",
                itm, shr_usage, drop_state);
      }

    return SOC_E_NONE;
}

static int
_soc_th3_egr_mc_sp_limit_debug_print(int unit, int spid) {
    soc_reg_t sp_limit_reg = MMU_THDO_MC_SHARED_CQE_POOL_SHARED_LIMITr;
    soc_reg_t sp_usage_reg = MMU_THDO_MC_SHARED_CQE_POOL_SHARED_COUNTr;
    soc_reg_t sp_resume_limit_reg = MMU_THDO_MC_SHARED_CQE_POOL_RESUME_LIMITr;
    soc_reg_t sp_drop_state_reg = MMU_THDO_MC_SHARED_CQE_POOL_DROP_STATESr;
    uint32 rval = 0;
    int sp_limit, sp_resume;
    int itm, usage, drop_state;

    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sp_limit_reg, spid, 0, &rval));
    sp_limit = soc_reg_field_get(unit, sp_limit_reg, rval, SHARED_LIMITf);

    rval = 0;
    SOC_IF_ERROR_RETURN
        (soc_reg32_get(unit, sp_resume_limit_reg, spid, 0, &rval));
    sp_resume =
        soc_reg_field_get(unit, sp_resume_limit_reg, rval, RESUME_LIMITf);

    cli_out("MC ServicePool: %1d:       Limit: %7d, ResetLimit:   %7d\n",
            spid, sp_limit, (sp_limit - sp_resume));

    for (itm = 0; itm < NUM_ITM(unit); itm++) {
        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_itm_reg32_get(unit, sp_usage_reg, itm, itm, spid,
                                         &rval));
        usage = soc_reg_field_get(unit, sp_usage_reg, rval, COUNTf);

        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_tomahawk3_itm_reg32_get(unit, sp_drop_state_reg, itm, itm, 0,
                                         &rval));
        drop_state = soc_reg_field_get(unit, sp_drop_state_reg, rval,
                egr_sp_drop_flds[spid]);

        cli_out("                  ITM:%1d, Usage: %7d, DropState:    %7d\n",
                itm, usage, drop_state);

    }

    return SOC_E_NONE;
}


static int
_soc_th3_egr_queue_debug_print(int unit, int port, int queue) {
    int uc_mc = 0, qgrp_en, spid, num_ucq, voq;
    soc_mem_t q_cfg_mem = MMU_THDO_Q_TO_QGRP_MAPD0m;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int qgrp_vld, use_qgrp_min;
    soc_info_t *si;

    si = &SOC_INFO(unit);
    num_ucq = _soc_th3_get_num_ucq(unit);
    if (IS_CPU_PORT(unit, port)) {
        if (queue >= SOC_TH3_NUM_CPU_QUEUES) {
            return SOC_E_PARAM;
        }
        /* CPU only has MC queues */
        uc_mc = 1;
        voq = si->port_cosq_base[port] + queue;
    } else {
        if (queue >= SOC_TH3_NUM_GP_QUEUES) {
            return SOC_E_PARAM;
        }
        /* Lower queues are UC, upper queues are MC */
        if (queue >= num_ucq) {
            uc_mc = 1;
        }
        voq = si->port_uc_cosq_base[port] + queue;
    }
    if (uc_mc) {
        /* Print for MC queues */
        if (!IS_CPU_PORT(unit, port)) {
            cli_out("### Egress Buffer Usage Debug Info for Port: %d, Queue: %d "
            " MC Cos: %d ###\n", port, queue, (queue - num_ucq));
        } else {
            cli_out("### Egress Buffer Usage Debug Info for Port: %d, Queue: %d "
            " MC Cos: %d ###\n", port, queue, queue);
        }
    } else {
        /* Print for UC queues */
        cli_out("### Egress Buffer Usage Debug Info for Port: %d, Queue: %d "
        " UC Cos: %d ###\n", port, queue, queue);
    }

    /* Queue basic config */
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, q_cfg_mem, MEM_BLOCK_ALL, voq, &entry));
    spid = soc_mem_field32_get(unit, q_cfg_mem, &entry, SPIDf);
    qgrp_vld = soc_mem_field32_get(unit, q_cfg_mem, &entry, QGROUP_VALIDf);
    use_qgrp_min = soc_mem_field32_get(unit, q_cfg_mem, &entry,
            USE_QGROUP_MINf);
    qgrp_en = (qgrp_vld & use_qgrp_min);

    if (!IS_CPU_PORT(unit, port)) {
        SOC_IF_ERROR_RETURN
            (_soc_th3_port_queue_limit_debug_print(unit, port, queue));
    } else {
        SOC_IF_ERROR_RETURN
            (_soc_th3_cpu_queue_debug_print(unit, port, queue));
    }
    SOC_IF_ERROR_RETURN
        (_soc_th3_port_qgroup_limit_debug_print(unit, port, uc_mc, qgrp_en));
    SOC_IF_ERROR_RETURN
        (_soc_th3_egr_portsp_limit_debug_print(unit, port, spid));
    SOC_IF_ERROR_RETURN
        (_soc_th3_egr_sp_limit_debug_print(unit, spid));

    if (uc_mc) {
        SOC_IF_ERROR_RETURN
            (_soc_th3_egr_mc_portsp_limit_debug_print(unit, port, spid));
        SOC_IF_ERROR_RETURN
            (_soc_th3_egr_mc_sp_limit_debug_print(unit, spid));
    }

    return SOC_E_NONE;
}

static const soc_reg_t obm_thresh_pp[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE][2] =
    {
        {{IDB_OBM0_THRESHOLD_PIPE0r, IDB_OBM0_THRESHOLD_1_PIPE0r},
         {IDB_OBM1_THRESHOLD_PIPE0r, IDB_OBM1_THRESHOLD_1_PIPE0r},
         {IDB_OBM2_THRESHOLD_PIPE0r, IDB_OBM2_THRESHOLD_1_PIPE0r},
         {IDB_OBM3_THRESHOLD_PIPE0r, IDB_OBM3_THRESHOLD_1_PIPE0r}},
        {{IDB_OBM0_THRESHOLD_PIPE1r, IDB_OBM0_THRESHOLD_1_PIPE1r},
         {IDB_OBM1_THRESHOLD_PIPE1r, IDB_OBM1_THRESHOLD_1_PIPE1r},
         {IDB_OBM2_THRESHOLD_PIPE1r, IDB_OBM2_THRESHOLD_1_PIPE1r},
         {IDB_OBM3_THRESHOLD_PIPE1r, IDB_OBM3_THRESHOLD_1_PIPE1r}},
        {{IDB_OBM0_THRESHOLD_PIPE2r, IDB_OBM0_THRESHOLD_1_PIPE2r},
         {IDB_OBM1_THRESHOLD_PIPE2r, IDB_OBM1_THRESHOLD_1_PIPE2r},
         {IDB_OBM2_THRESHOLD_PIPE2r, IDB_OBM2_THRESHOLD_1_PIPE2r},
         {IDB_OBM3_THRESHOLD_PIPE2r, IDB_OBM3_THRESHOLD_1_PIPE2r}},
        {{IDB_OBM0_THRESHOLD_PIPE3r, IDB_OBM0_THRESHOLD_1_PIPE3r},
         {IDB_OBM1_THRESHOLD_PIPE3r, IDB_OBM1_THRESHOLD_1_PIPE3r},
         {IDB_OBM2_THRESHOLD_PIPE3r, IDB_OBM2_THRESHOLD_1_PIPE3r},
         {IDB_OBM3_THRESHOLD_PIPE3r, IDB_OBM3_THRESHOLD_1_PIPE3r}},
        {{IDB_OBM0_THRESHOLD_PIPE4r, IDB_OBM0_THRESHOLD_1_PIPE4r},
         {IDB_OBM1_THRESHOLD_PIPE4r, IDB_OBM1_THRESHOLD_1_PIPE4r},
         {IDB_OBM2_THRESHOLD_PIPE4r, IDB_OBM2_THRESHOLD_1_PIPE4r},
         {IDB_OBM3_THRESHOLD_PIPE4r, IDB_OBM3_THRESHOLD_1_PIPE4r}},
        {{IDB_OBM0_THRESHOLD_PIPE5r, IDB_OBM0_THRESHOLD_1_PIPE5r},
         {IDB_OBM1_THRESHOLD_PIPE5r, IDB_OBM1_THRESHOLD_1_PIPE5r},
         {IDB_OBM2_THRESHOLD_PIPE5r, IDB_OBM2_THRESHOLD_1_PIPE5r},
         {IDB_OBM3_THRESHOLD_PIPE5r, IDB_OBM3_THRESHOLD_1_PIPE5r}},
        {{IDB_OBM0_THRESHOLD_PIPE6r, IDB_OBM0_THRESHOLD_1_PIPE6r},
         {IDB_OBM1_THRESHOLD_PIPE6r, IDB_OBM1_THRESHOLD_1_PIPE6r},
         {IDB_OBM2_THRESHOLD_PIPE6r, IDB_OBM2_THRESHOLD_1_PIPE6r},
         {IDB_OBM3_THRESHOLD_PIPE6r, IDB_OBM3_THRESHOLD_1_PIPE6r}},
        {{IDB_OBM0_THRESHOLD_PIPE7r, IDB_OBM0_THRESHOLD_1_PIPE7r},
         {IDB_OBM1_THRESHOLD_PIPE7r, IDB_OBM1_THRESHOLD_1_PIPE7r},
         {IDB_OBM2_THRESHOLD_PIPE7r, IDB_OBM2_THRESHOLD_1_PIPE7r},
         {IDB_OBM3_THRESHOLD_PIPE7r, IDB_OBM3_THRESHOLD_1_PIPE7r}}
    };

static const soc_reg_t obm_usage_pp[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE][2] =
    {
        {{IDB_OBM0_USAGE_PIPE0r, IDB_OBM0_USAGE_1_PIPE0r},
         {IDB_OBM1_USAGE_PIPE0r, IDB_OBM1_USAGE_1_PIPE0r},
         {IDB_OBM2_USAGE_PIPE0r, IDB_OBM2_USAGE_1_PIPE0r},
         {IDB_OBM3_USAGE_PIPE0r, IDB_OBM3_USAGE_1_PIPE0r}},
        {{IDB_OBM0_USAGE_PIPE1r, IDB_OBM0_USAGE_1_PIPE1r},
         {IDB_OBM1_USAGE_PIPE1r, IDB_OBM1_USAGE_1_PIPE1r},
         {IDB_OBM2_USAGE_PIPE1r, IDB_OBM2_USAGE_1_PIPE1r},
         {IDB_OBM3_USAGE_PIPE1r, IDB_OBM3_USAGE_1_PIPE1r}},
        {{IDB_OBM0_USAGE_PIPE2r, IDB_OBM0_USAGE_1_PIPE2r},
         {IDB_OBM1_USAGE_PIPE2r, IDB_OBM1_USAGE_1_PIPE2r},
         {IDB_OBM2_USAGE_PIPE2r, IDB_OBM2_USAGE_1_PIPE2r},
         {IDB_OBM3_USAGE_PIPE2r, IDB_OBM3_USAGE_1_PIPE2r}},
        {{IDB_OBM0_USAGE_PIPE3r, IDB_OBM0_USAGE_1_PIPE3r},
         {IDB_OBM1_USAGE_PIPE3r, IDB_OBM1_USAGE_1_PIPE3r},
         {IDB_OBM2_USAGE_PIPE3r, IDB_OBM2_USAGE_1_PIPE3r},
         {IDB_OBM3_USAGE_PIPE3r, IDB_OBM3_USAGE_1_PIPE3r}},
        {{IDB_OBM0_USAGE_PIPE4r, IDB_OBM0_USAGE_1_PIPE4r},
         {IDB_OBM1_USAGE_PIPE4r, IDB_OBM1_USAGE_1_PIPE4r},
         {IDB_OBM2_USAGE_PIPE4r, IDB_OBM2_USAGE_1_PIPE4r},
         {IDB_OBM3_USAGE_PIPE4r, IDB_OBM3_USAGE_1_PIPE4r}},
        {{IDB_OBM0_USAGE_PIPE5r, IDB_OBM0_USAGE_1_PIPE5r},
         {IDB_OBM1_USAGE_PIPE5r, IDB_OBM1_USAGE_1_PIPE5r},
         {IDB_OBM2_USAGE_PIPE5r, IDB_OBM2_USAGE_1_PIPE5r},
         {IDB_OBM3_USAGE_PIPE5r, IDB_OBM3_USAGE_1_PIPE5r}},
        {{IDB_OBM0_USAGE_PIPE6r, IDB_OBM0_USAGE_1_PIPE6r},
         {IDB_OBM1_USAGE_PIPE6r, IDB_OBM1_USAGE_1_PIPE6r},
         {IDB_OBM2_USAGE_PIPE6r, IDB_OBM2_USAGE_1_PIPE6r},
         {IDB_OBM3_USAGE_PIPE6r, IDB_OBM3_USAGE_1_PIPE6r}},
        {{IDB_OBM0_USAGE_PIPE7r, IDB_OBM0_USAGE_1_PIPE7r},
         {IDB_OBM1_USAGE_PIPE7r, IDB_OBM1_USAGE_1_PIPE7r},
         {IDB_OBM2_USAGE_PIPE7r, IDB_OBM2_USAGE_1_PIPE7r},
         {IDB_OBM3_USAGE_PIPE7r, IDB_OBM3_USAGE_1_PIPE7r}}
    };

static const soc_reg_t
obm_fc_thresh_pp[_TH3_PIPES_PER_DEV][_TH3_PBLKS_PER_PIPE][2] =
    {
        {{IDB_OBM0_FC_THRESHOLD_PIPE0r, IDB_OBM0_FC_THRESHOLD_1_PIPE0r},
         {IDB_OBM1_FC_THRESHOLD_PIPE0r, IDB_OBM1_FC_THRESHOLD_1_PIPE0r},
         {IDB_OBM2_FC_THRESHOLD_PIPE0r, IDB_OBM2_FC_THRESHOLD_1_PIPE0r},
         {IDB_OBM3_FC_THRESHOLD_PIPE0r, IDB_OBM3_FC_THRESHOLD_1_PIPE0r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE1r, IDB_OBM0_FC_THRESHOLD_1_PIPE1r},
         {IDB_OBM1_FC_THRESHOLD_PIPE1r, IDB_OBM1_FC_THRESHOLD_1_PIPE1r},
         {IDB_OBM2_FC_THRESHOLD_PIPE1r, IDB_OBM2_FC_THRESHOLD_1_PIPE1r},
         {IDB_OBM3_FC_THRESHOLD_PIPE1r, IDB_OBM3_FC_THRESHOLD_1_PIPE1r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE2r, IDB_OBM0_FC_THRESHOLD_1_PIPE2r},
         {IDB_OBM1_FC_THRESHOLD_PIPE2r, IDB_OBM1_FC_THRESHOLD_1_PIPE2r},
         {IDB_OBM2_FC_THRESHOLD_PIPE2r, IDB_OBM2_FC_THRESHOLD_1_PIPE2r},
         {IDB_OBM3_FC_THRESHOLD_PIPE2r, IDB_OBM3_FC_THRESHOLD_1_PIPE2r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE3r, IDB_OBM0_FC_THRESHOLD_1_PIPE3r},
         {IDB_OBM1_FC_THRESHOLD_PIPE3r, IDB_OBM1_FC_THRESHOLD_1_PIPE3r},
         {IDB_OBM2_FC_THRESHOLD_PIPE3r, IDB_OBM2_FC_THRESHOLD_1_PIPE3r},
         {IDB_OBM3_FC_THRESHOLD_PIPE3r, IDB_OBM3_FC_THRESHOLD_1_PIPE3r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE4r, IDB_OBM0_FC_THRESHOLD_1_PIPE4r},
         {IDB_OBM1_FC_THRESHOLD_PIPE4r, IDB_OBM1_FC_THRESHOLD_1_PIPE4r},
         {IDB_OBM2_FC_THRESHOLD_PIPE4r, IDB_OBM2_FC_THRESHOLD_1_PIPE4r},
         {IDB_OBM3_FC_THRESHOLD_PIPE4r, IDB_OBM3_FC_THRESHOLD_1_PIPE4r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE5r, IDB_OBM0_FC_THRESHOLD_1_PIPE5r},
         {IDB_OBM1_FC_THRESHOLD_PIPE5r, IDB_OBM1_FC_THRESHOLD_1_PIPE5r},
         {IDB_OBM2_FC_THRESHOLD_PIPE5r, IDB_OBM2_FC_THRESHOLD_1_PIPE5r},
         {IDB_OBM3_FC_THRESHOLD_PIPE5r, IDB_OBM3_FC_THRESHOLD_1_PIPE5r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE6r, IDB_OBM0_FC_THRESHOLD_1_PIPE6r},
         {IDB_OBM1_FC_THRESHOLD_PIPE6r, IDB_OBM1_FC_THRESHOLD_1_PIPE6r},
         {IDB_OBM2_FC_THRESHOLD_PIPE6r, IDB_OBM2_FC_THRESHOLD_1_PIPE6r},
         {IDB_OBM3_FC_THRESHOLD_PIPE6r, IDB_OBM3_FC_THRESHOLD_1_PIPE6r}},
        {{IDB_OBM0_FC_THRESHOLD_PIPE7r, IDB_OBM0_FC_THRESHOLD_1_PIPE7r},
         {IDB_OBM1_FC_THRESHOLD_PIPE7r, IDB_OBM1_FC_THRESHOLD_1_PIPE7r},
         {IDB_OBM2_FC_THRESHOLD_PIPE7r, IDB_OBM2_FC_THRESHOLD_1_PIPE7r},
         {IDB_OBM3_FC_THRESHOLD_PIPE7r, IDB_OBM3_FC_THRESHOLD_1_PIPE7r}}
    };

static const soc_reg_t obm_fc_en_regs[_TH3_PBLKS_PER_PIPE] =
    {IDB_OBM0_FLOW_CONTROL_CONFIGr, IDB_OBM1_FLOW_CONTROL_CONFIGr,
     IDB_OBM2_FLOW_CONTROL_CONFIGr, IDB_OBM3_FLOW_CONTROL_CONFIGr};

static int
_soc_th3_obm_debug_print(int unit, int port) {
    soc_info_t *si;
    int phy_port, pipe, pm, subp;
    uint64 rval64;
    soc_reg_t reg;
    uint32 limit, usage, pri_bmp, xoff, xon, fc_en, pfc;

    si = &SOC_INFO(unit);
    phy_port = si->port_l2p_mapping[port];
    pipe = si->port_pipe[port];
    pm = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
    subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);

     cli_out("######### OBM Limits(Bytes) for Port: %d #########\n", port);

    /* Port FC enable */
    reg =  obm_fc_en_regs[pm];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, pipe, subp, &rval64));
    fc_en = soc_reg64_field32_get(unit, reg, rval64, PORT_FC_ENf);
    pfc = soc_reg64_field32_get(unit, reg, rval64, FC_TYPEf);
    if (pfc) {
        cli_out("  Port          FC Enable:  %7d      Mode: PFC \n", fc_en);
    } else {
        cli_out("  Port          FC Enable:  %7d      Mode: Pause \n", fc_en);
    }

    /* Port Xoff, Xon */
    reg =  obm_fc_thresh_pp[pipe][pm][1];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    xoff = soc_reg64_field32_get(unit, reg, rval64, PORT_XOFFf) *
        _TH3_IDB_THD_GRAN;
    xon = soc_reg64_field32_get(unit, reg, rval64, PORT_XONf) *
        _TH3_IDB_THD_GRAN;
     cli_out("               Xoff Limit:  %7d Xon Limit: %7d \n", xoff, xon);

    /* Port discard limit */
    reg =  obm_thresh_pp[pipe][pm][1];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    limit = soc_reg64_field32_get(unit, reg, rval64, DISCARD_LIMITf) *
        _TH3_IDB_THD_GRAN;
     cli_out("            Discard Limit:  %7d \n", limit);

     /* Port current usage */
    reg =  obm_usage_pp[pipe][pm][1];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    usage = soc_reg64_field32_get(unit, reg, rval64, TOTAL_COUNTf);
     cli_out("              Total Usage:  %7d \n", usage);

    /* Lossless 0 */
    reg =  obm_fc_en_regs[pm];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, pipe, subp, &rval64));
    fc_en = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS0_FC_ENf);
    if (!pfc) {
        cli_out("  Lossless0     FC Enable:  %7d   \n", fc_en);
    } else {
        pri_bmp = soc_reg64_field32_get(unit, reg, rval64,
                LOSSLESS0_PRIORITY_PROFILEf);
        cli_out("  Lossless0     FC Enable:  %7d  Pri_bmp: %7x \n", fc_en,
                pri_bmp);
    }

    reg =  obm_fc_thresh_pp[pipe][pm][0];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    xoff = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS0_XOFFf) *
        _TH3_IDB_THD_GRAN;
    xon = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS0_XONf) *
        _TH3_IDB_THD_GRAN;
     cli_out("               Xoff Limit:  %7d Xon Limit: %7d \n", xoff, xon);

    reg =  obm_thresh_pp[pipe][pm][0];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    limit = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS0_DISCARDf) *
        _TH3_IDB_THD_GRAN;
     cli_out("            Discard Limit:  %7d \n", limit);

    reg =  obm_usage_pp[pipe][pm][0];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    usage = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS0_COUNTf);
     cli_out("                    Usage:  %7d \n", usage);

    /* Lossless 1 */
    reg =  obm_fc_en_regs[pm];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, pipe, subp, &rval64));
    fc_en = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS1_FC_ENf);
    if (!pfc) {
        cli_out("  Lossless1     FC Enable:  %7d   \n", fc_en);
    } else {
        pri_bmp = soc_reg64_field32_get(unit, reg, rval64,
                LOSSLESS1_PRIORITY_PROFILEf);
        cli_out("  Lossless1     FC Enable:  %7d  Pri_bmp: %7x \n", fc_en,
                pri_bmp);
    }
    reg =  obm_fc_thresh_pp[pipe][pm][0];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    xoff = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS1_XOFFf) *
        _TH3_IDB_THD_GRAN;
    xon = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS1_XONf) *
        _TH3_IDB_THD_GRAN;
     cli_out("               Xoff Limit:  %7d Xon Limit: %7d \n", xoff, xon);

    reg =  obm_thresh_pp[pipe][pm][1];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    limit = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS1_DISCARDf) *
        _TH3_IDB_THD_GRAN;
     cli_out("            Discard Limit:  %7d \n", limit);

    reg =  obm_usage_pp[pipe][pm][1];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    usage = soc_reg64_field32_get(unit, reg, rval64, LOSSLESS1_COUNTf);
     cli_out("                    Usage:  %7d \n", usage);


     /* Lossy High */
    reg =  obm_thresh_pp[pipe][pm][0];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    limit = soc_reg64_field32_get(unit, reg, rval64, LOSSY_DISCARDf) *
        _TH3_IDB_THD_GRAN;
     cli_out("  Lossy Hi  Discard Limit:  %7d \n", limit);

    reg =  obm_thresh_pp[pipe][pm][0];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    limit = soc_reg64_field32_get(unit, reg, rval64, LOSSY_LOW_PRIf) *
        _TH3_IDB_THD_GRAN;
     cli_out("  Lossy Lo  Discard Limit:  %7d \n", limit);

     /* Lossy current usage */
    reg =  obm_usage_pp[pipe][pm][0];
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, REG_PORT_ANY, subp, &rval64));
    usage = soc_reg64_field32_get(unit, reg, rval64, LOSSY_COUNTf);
     cli_out("              Lossy Usage:  %7d \n", usage);


    return SOC_E_NONE;
}

static int
_soc_th3_get_bw_from_ref_gran(int unit, uint32 refresh, int gran, int mode,
        uint32 *bw) {

    /* Byte shaping granularity in Kbps*/
    static const uint32 byte_gran[_TH3_NUM_MTR_GRAN] = {32, 64, 128, 256, 512,
        1024, 2048, 4096};
    /* Packet shaping granularity in pps*/
    static const uint32 pkt_gran[_TH3_NUM_MTR_GRAN] = {2, 4, 8, 32, 128, 512,
        2048, 8192};

    if (refresh == 0) {
        *bw = 0;
        return SOC_E_NONE;
    }
    if (mode) {
        /*Packet shaping*/
        *bw = refresh * pkt_gran[gran];
    } else {
        /* Byte shaping */
        *bw = refresh * byte_gran[gran];
    }
    return SOC_E_NONE;
}

static int
_soc_th3_fp_l0_bw_print(int unit, int port, int l0) {
    soc_info_t *si = &SOC_INFO(unit);
    int pipe, mmu_local_port;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    uint32 refresh, bw;
    int gran, mode, oop;
    int l0_index;

    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];
    l0_index = (mmu_local_port * SOC_TH3_NUM_GP_QUEUES) + l0;

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_L0_Am, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l0_index, &entry));
    refresh = soc_mem_field32_get(unit, mem, &entry, MAX_REFRESHf);
    mode = soc_mem_field32_get(unit, mem, &entry, PACKET_SHAPINGf);
    gran = soc_mem_field32_get(unit, mem, &entry, MAX_METER_GRANf);
    SOC_IF_ERROR_RETURN
        (_soc_th3_get_bw_from_ref_gran(unit, refresh, gran, mode, &bw));

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_BUCKET_L0m, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l0_index, &entry));
    oop = soc_mem_field32_get(unit, mem, &entry, MAX_PROFILE_EXCEEDED_FLAGf);

    if (!mode) {
        cli_out("                 Max Bw: %u Kbps Max Exceeded:  %d\n",
                bw, oop);
    } else {
        cli_out("                 Max Bw: %u pps  Max Exceeded:  %d\n",
                bw, oop);
    }

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_L0_Am, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l0_index, &entry));
    refresh = soc_mem_field32_get(unit, mem, &entry, MIN_REFRESHf);
    mode = soc_mem_field32_get(unit, mem, &entry, PACKET_SHAPINGf);
    gran = soc_mem_field32_get(unit, mem, &entry, MIN_METER_GRANf);
    SOC_IF_ERROR_RETURN
        (_soc_th3_get_bw_from_ref_gran(unit, refresh, gran, mode, &bw));

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_BUCKET_L0m, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l0_index, &entry));
    oop = soc_mem_field32_get(unit, mem, &entry, MIN_PROFILE_SATISFIED_FLAGf);

    if (!mode) {
        cli_out("                 Min Bw: %u Kbps Min Satisfied: %d\n",
                bw, oop);
    } else {
        cli_out("                 Min Bw: %u pps  Min Satisfied: %d\n",
                bw, oop);
    }


    return SOC_E_NONE;
}


static const soc_field_t schq_flds[SOC_TH3_NUM_GP_QUEUES] = {SCH_Q_NUM_0f,
    SCH_Q_NUM_1f, SCH_Q_NUM_2f, SCH_Q_NUM_3f, SCH_Q_NUM_4f, SCH_Q_NUM_5f,
    SCH_Q_NUM_6f, SCH_Q_NUM_7f, SCH_Q_NUM_8f, SCH_Q_NUM_9f, SCH_Q_NUM_10f,
    SCH_Q_NUM_11f};

static int
_soc_th3_queue_sched_shap_print(int unit, int port, int queue) {
    soc_info_t *si = &SOC_INFO(unit);
    int num_ucq;
    int l1_node, l1_weight;
    int l0_node, l0_weight;
    int l0_index, l1_index;
    soc_reg_t reg;
    uint64 rval64;
    uint32 l1_l0_map, rval;
    int q_profile, l1_l0_mux;
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int pipe, mmu_local_port;

    if (IS_CPU_PORT(unit, port)) {
        return SOC_E_PARAM;
    }
    num_ucq = _soc_th3_get_num_ucq(unit);
    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];
    if (queue >= num_ucq) {
        /* MC queue */
        cli_out ("Queue: %2d, MC queue \n", queue);
    } else {
        /*UC Queue */
        cli_out ("Queue: %2d, UC queue \n", queue);
    }

    reg = MMU_PORT_MMUQ_SCHQ_CFGr;
    rval = 0;
    SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, reg, port, 0, &rval));
    q_profile = soc_reg_field_get(unit, reg, rval, Q_PROFILE_SELf);

    reg = MMU_MTRO_MMUQ_SCHQ_PROFILEr;
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
            (soc_reg_get(unit, reg, REG_PORT_ANY, q_profile, &rval64));
    l1_node = soc_reg64_field32_get(unit, reg, rval64, schq_flds[queue]);

    l1_index = (mmu_local_port * SOC_TH3_NUM_GP_QUEUES) + l1_node;
    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_QSCH_L1_WEIGHT_MEMm, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l1_index, &entry));
    l1_weight = soc_mem_field32_get(unit, mem, &entry, WEIGHTf);

    cli_out("         L1: %2d  Weight: %d\n", l1_node, l1_weight);


/*
 * bit0  specifies the L0 node that the L1 node 0 is mapped to
                => 0: maps to L0 node 0;  1: maps to L0 node 1.
 * bit1  specifies the L0 node that the L1 node 1 is mapped to
                => 0: maps to L0 node 1;  1: maps to L0 node 2.
 * bit2  specifies the L0 node that the L1 node 2 is mapped to
                => 0: maps to L0 node 2;  1: maps to L0 node 3.
 * bit3  specifies the L0 node that the L1 node 3 is mapped to
                => 0: maps to L0 node 3;  1: maps to L0 node 4.
 * bit4  specifies the L0 node that the L1 node 4 is mapped to
                => 0: maps to L0 node 4;  1: maps to L0 node 5.
 * bit5  specifies the L0 node that the L1 node 5 is mapped to
                => 0: maps to L0 node 5;  1: maps to L0 node 6.
 * bit6  specifies the L0 node that the L1 node 6 is mapped to
                => 0: maps to L0 node 6;  1: maps to L0 node 7.
 * bit7  specifies the L0 node that the L1 node 7 is mapped to
                => 0: maps to L0 node 7;  1: maps to L0 node 8.
 * bit8  specifies the L0 node that the L1 node 8 is mapped to
                => 0: maps to L0 node 8;  1: maps to L0 node 9.
 * bit9  specifies the L0 node that the L1 node 9 is mapped to
                => 0: maps to L0 node 9;  1: maps to L0 node 10.
 * bit10 specifies the L0 node that the L1 node 10 is mapped to
                => 0: maps to L0 node 10; 1: maps to L0 node 11.
 * bit11 specifies the L0 node that the L1 node 11 is mapped to
                => 0: maps to L0 node 11; 1: maps to L0 node 11.
 * Note that L1 node 11 always maps to L0 node 11.
 */

    reg = MMU_QSCH_PORT_CONFIGr;
    COMPILER_64_SET(rval64, 0, 0);
    SOC_IF_ERROR_RETURN
        (soc_reg_get(unit, reg, port, 0, &rval64));
    l1_l0_map = soc_reg64_field32_get(unit, reg, rval64, L1_TO_L0_MAPPINGf);
    l1_l0_mux = (l1_l0_map >> l1_node) & 1;
    if (l1_node == 11) {
        l0_node = 11;
    } else {
        if (l1_l0_mux) {
            l0_node = l1_node + 1;
        } else {
            l0_node = l1_node;
        }
    }

    l0_index = (mmu_local_port * SOC_TH3_NUM_GP_QUEUES) + l0_node;
    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_QSCH_L0_WEIGHT_MEMm, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l0_index, &entry));
    l0_weight = soc_mem_field32_get(unit, mem, &entry, WEIGHTf);
    cli_out("               L0: %d  Weight: %d\n", l0_node, l0_weight);

    SOC_IF_ERROR_RETURN(_soc_th3_fp_l0_bw_print(unit, port, l0_node));

    return SOC_E_NONE;

}

static int
_soc_th3_cpu_l0_bw_wt_print(int unit, int l0) {
    int weight;
    int wt_mem_index, mmu_local_port, pipe;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int mode, gran, oop;
    uint32 refresh, bw;
    int port = 0; /* CPU port */

    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];
    wt_mem_index = (mmu_local_port * SOC_TH3_NUM_GP_QUEUES) + l0;

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_QSCH_L0_WEIGHT_MEMm, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, wt_mem_index, &entry));
    weight = soc_mem_field32_get(unit, mem, &entry, WEIGHTf);

    cli_out("         L0: %2d  Weight: %d\n", l0, weight);

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_L0_Am, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, wt_mem_index, &entry));
    mode = soc_mem_field32_get(unit, mem, &entry, PACKET_SHAPINGf);
    /* Max BW */
    gran = soc_mem_field32_get(unit, mem, &entry, MAX_METER_GRANf);
    refresh = soc_mem_field32_get(unit, mem, &entry, MAX_REFRESHf);
    SOC_IF_ERROR_RETURN
        (_soc_th3_get_bw_from_ref_gran(unit, refresh, gran, mode, &bw));

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_BUCKET_L0m, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, wt_mem_index, &entry));
    oop = soc_mem_field32_get(unit, mem, &entry, MAX_PROFILE_EXCEEDED_FLAGf);

    if (!mode) {
        cli_out("                 Max Bw: %u Kbps Max Exceeded:  %d\n",
                bw, oop);
    } else {
        cli_out("                 Max Bw: %u pps  Max Exceeded:  %d\n",
                bw, oop);
    }

    /*Min BW */
    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_L0_Am, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, wt_mem_index, &entry));
    refresh = soc_mem_field32_get(unit, mem, &entry, MIN_REFRESHf);
    mode = soc_mem_field32_get(unit, mem, &entry, PACKET_SHAPINGf);
    gran = soc_mem_field32_get(unit, mem, &entry, MIN_METER_GRANf);
    SOC_IF_ERROR_RETURN
        (_soc_th3_get_bw_from_ref_gran(unit, refresh, gran, mode, &bw));

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_BUCKET_L0m, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, wt_mem_index, &entry));
    oop = soc_mem_field32_get(unit, mem, &entry, MIN_PROFILE_SATISFIED_FLAGf);

    if (!mode) {
        cli_out("                 Min Bw: %u Kbps Min Satisfied: %d\n",
                bw, oop);
    } else {
        cli_out("                 Min Bw: %u pps  Min Satisfied: %d\n",
                bw, oop);
    }


    return SOC_E_NONE;
}

static int
_soc_th3_cpu_l1_bw_wt_print(int unit, int l1) {
    int weight;
    int wt_mem_index, mmu_local_port, pipe;
    soc_info_t *si = &SOC_INFO(unit);
    soc_mem_t mem;
    uint32 entry[SOC_MAX_MEM_WORDS];
    int mode, gran, oop;
    uint32 refresh, bw;
    int port = 0; /* CPU port */

    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];
    wt_mem_index = (mmu_local_port * SOC_TH3_NUM_GP_QUEUES) + l1;

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_QSCH_L1_WEIGHT_MEMm, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, wt_mem_index, &entry));
    weight = soc_mem_field32_get(unit, mem, &entry, WEIGHTf);

    cli_out("         L1: %2d  Weight: %d\n", l1, weight);

    mem = MMU_MTRO_CPU_L1_Am;
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l1, &entry));
    mode = soc_mem_field32_get(unit, mem, &entry, PACKET_SHAPINGf);
    /* Max BW */
    gran = soc_mem_field32_get(unit, mem, &entry, MAX_METER_GRANf);
    refresh = soc_mem_field32_get(unit, mem, &entry, MAX_REFRESHf);
    SOC_IF_ERROR_RETURN
        (_soc_th3_get_bw_from_ref_gran(unit, refresh, gran, mode, &bw));

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_BUCKET_CPU_L1m, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l1, &entry));
    oop = soc_mem_field32_get(unit, mem, &entry, MAX_PROFILE_EXCEEDED_FLAGf);

    if (!mode) {
        cli_out("                 Max Bw: %u Kbps Max Exceeded:  %d\n",
                bw, oop);
    } else {
        cli_out("                 Max Bw: %u pps  Max Exceeded:  %d\n",
                bw, oop);
    }

    /*Min BW */
    mem = MMU_MTRO_CPU_L1_Am;
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l1, &entry));
    refresh = soc_mem_field32_get(unit, mem, &entry, MIN_REFRESHf);
    mode = soc_mem_field32_get(unit, mem, &entry, PACKET_SHAPINGf);
    gran = soc_mem_field32_get(unit, mem, &entry, MIN_METER_GRANf);
    SOC_IF_ERROR_RETURN
        (_soc_th3_get_bw_from_ref_gran(unit, refresh, gran, mode, &bw));

    mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_BUCKET_CPU_L1m, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, mem, MEM_BLOCK_ALL, l1, &entry));
    oop = soc_mem_field32_get(unit, mem, &entry, MIN_PROFILE_SATISFIED_FLAGf);

    if (!mode) {
        cli_out("                 Min Bw: %u Kbps Min Satisfied: %d\n",
                bw, oop);
    } else {
        cli_out("                 Min Bw: %u pps  Min Satisfied: %d\n",
                bw, oop);
    }


    return SOC_E_NONE;
}


static int
_soc_th3_cpu_queue_sched_shap_print(int unit, int port) {
    soc_reg_t cpu_map_reg = MMU_MTRO_CPU_L1_TO_L0_MAPPINGr;
    int cpu_l1, cpu_l0;
    uint32 rval;

    if (!IS_CPU_PORT(unit, port)) {
        return SOC_E_PARAM;
    }
    for (cpu_l1 = 0; cpu_l1 < SOC_TH3_NUM_CPU_QUEUES; cpu_l1++) {
        cli_out ("Queue: %2d, MC queue \n", cpu_l1);
        SOC_IF_ERROR_RETURN
            (_soc_th3_cpu_l1_bw_wt_print(unit, cpu_l1));

        rval = 0;
        SOC_IF_ERROR_RETURN
            (soc_reg32_get(unit, cpu_map_reg, REG_PORT_ANY, cpu_l1, &rval));
        cpu_l0 = soc_reg_field_get(unit, cpu_map_reg, rval, SELECTf);
        SOC_IF_ERROR_RETURN
            (_soc_th3_cpu_l0_bw_wt_print(unit, cpu_l0));

    }
    return SOC_E_NONE;
}

static int
_soc_th3_mmu_scheduler_print(int unit, int port) {
    uint32 refresh, bw;
    int gran, mode, oop, queue;
    soc_mem_t read_mem;
    int pipe, mmu_local_port;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 entry[SOC_MAX_MEM_WORDS];

    pipe = si->port_pipe[port];
    mmu_local_port = si->port_l2i_mapping[port];
    cli_out("######### Scheduling/Shaping debug for Port: %d #########\n", port);
    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_EGRMETERINGCONFIGm, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    refresh = soc_mem_field32_get(unit, read_mem, &entry, REFRESHf);
    mode = soc_mem_field32_get(unit, read_mem, &entry, PACKET_SHAPINGf);
    gran = soc_mem_field32_get(unit, read_mem, &entry, METER_GRANf);

    SOC_IF_ERROR_RETURN
        (_soc_th3_get_bw_from_ref_gran(unit, refresh, gran, mode, &bw));

    read_mem = SOC_MEM_UNIQUE_ACC_PIPE(unit, MMU_MTRO_EGRMETERINGBUCKETm, pipe);
    SOC_IF_ERROR_RETURN
        (soc_mem_read(unit, read_mem, MEM_BLOCK_ALL, mmu_local_port, &entry));
    oop = soc_mem_field32_get(unit, read_mem, &entry, MAX_PROFILE_EXCEEDED_FLAGf);

    if (!mode) {
        cli_out("Egress Meter Bw: %u Kbps Metering Satisfied: %d\n", bw, oop);
    } else {
        cli_out("Egress Meter Bw: %u pps  Metering Satisfied: %d\n", bw, oop);
    }

    if (!(IS_CPU_PORT(unit, port))) {
        for (queue = 0; queue < SOC_TH3_NUM_GP_QUEUES; queue++) {
            _soc_th3_queue_sched_shap_print(unit, port, queue);
        }
    } else {
        _soc_th3_cpu_queue_sched_shap_print(unit, port);
    }

    return SOC_E_NONE;
}


cmd_result_t
cmd_tm_show_hw_config(int unit, args_t *arg)
{
    char *component, *sub_comp, *buf_ent;
    pbmp_t pbm;
    int port;
    int pg, queue, q_max;
    int rv = 0;

    component = ARG_GET(arg);
    if (component == NULL) {
        return CMD_USAGE;
    }
    if (!sal_strcasecmp(component, "ing_buf_debug")) {
        sub_comp = ARG_GET(arg);
        if (sub_comp == NULL) {
            return CMD_USAGE;
        }
        if (parse_bcm_pbmp(unit, sub_comp, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n",
                    ARG_CMD(arg), sub_comp);
            return CMD_FAIL;
        }
        if (BCM_PBMP_IS_NULL(pbm)) {
            ARG_DISCARD(arg);
            return CMD_OK;
        }
        SOC_PBMP_ITER(pbm, port) {
            buf_ent = ARG_GET(arg);
            if(buf_ent == NULL) {
                for (pg = 0; pg < _TH3_MMU_NUM_PG; pg++) {
                    rv = _soc_th3_pg_debug_print(unit, port, pg);
                    if (rv < 0) {
                        return CMD_FAIL;
                    }
                }
            } else {
                pg = sal_atoi(buf_ent);
                rv = _soc_th3_pg_debug_print(unit, port, pg);
                if (rv < 0) {
                    return CMD_FAIL;
                }
            }
        }
    } else if (!sal_strcasecmp(component, "egr_buf_debug")) {
                sub_comp = ARG_GET(arg);
        if (sub_comp == NULL) {
            return CMD_USAGE;
        }
        if (parse_bcm_pbmp(unit, sub_comp, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n",
                    ARG_CMD(arg), sub_comp);
            return CMD_FAIL;
        }
        if (BCM_PBMP_IS_NULL(pbm)) {
            ARG_DISCARD(arg);
            return CMD_OK;
        }
        SOC_PBMP_ITER(pbm, port) {
            buf_ent = ARG_GET(arg);
            if (IS_CPU_PORT(unit, port)) {
                q_max = SOC_TH3_NUM_CPU_QUEUES;
            } else {
                q_max = SOC_TH3_NUM_GP_QUEUES;
            }
            if(buf_ent == NULL) {
                for (queue = 0; queue < q_max; queue++) {
                    rv = _soc_th3_egr_queue_debug_print(unit, port, queue);
                    if (rv < 0) {
                        return CMD_FAIL;
                    }
                }
            } else {
                queue = sal_atoi(buf_ent);
                rv = _soc_th3_egr_queue_debug_print(unit, port, queue);
                if (rv < 0) {
                    return CMD_FAIL;
                }
            }
        }
    } else if (!sal_strcasecmp(component, "idb_buf_debug")) {
        sub_comp = ARG_GET(arg);
        if (sub_comp == NULL) {
            return CMD_USAGE;
        }
        if (parse_bcm_pbmp(unit, sub_comp, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n",
                    ARG_CMD(arg), sub_comp);
            return CMD_FAIL;
        }
        if (BCM_PBMP_IS_NULL(pbm)) {
            ARG_DISCARD(arg);
            return CMD_OK;
        }
        SOC_PBMP_ITER(pbm, port) {
            if (((IS_MANAGEMENT_PORT(unit, port)) ||
                 (IS_CPU_PORT(unit, port)) ||
                 (IS_LB_PORT(unit, port)))) {
                cli_out("OBM buffers not used for port: %d\n", port);
                continue;
            }
            rv = _soc_th3_obm_debug_print(unit, port);
            if (rv < 0) {
                return CMD_FAIL;
            }
        }
    } else if (!sal_strcasecmp(component, "cosq_debug")) {
        sub_comp = ARG_GET(arg);
        if (sub_comp == NULL) {
            return CMD_USAGE;
        }
        if (parse_bcm_pbmp(unit, sub_comp, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n",
                    ARG_CMD(arg), sub_comp);
            return CMD_FAIL;
        }
        if (BCM_PBMP_IS_NULL(pbm)) {
            ARG_DISCARD(arg);
            return CMD_OK;
        }
        SOC_PBMP_ITER(pbm, port) {
            rv = _soc_th3_mmu_scheduler_print(unit, port);
            if (rv < 0) {
                return CMD_FAIL;
            }
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif /* BCM_TOMAHAWK3_SUPPORT */
