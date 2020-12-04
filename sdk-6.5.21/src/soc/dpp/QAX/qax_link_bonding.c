/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */ 

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LB

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <shared/swstate/access/sw_state_access.h>
#include <soc/types.h>
#include <soc/mem.h>
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/register.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/arad_chip_defines.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#ifdef BCM_LB_SUPPORT
#include <soc/dpp/QAX/qax_link_bonding.h>

#define QAX_LB_TC_DP_MAP_ENTRY_INDX(_tc, _dp) \
    ((_tc << 2) + _dp)
#define QAX_LB_TC_DP_MAP_ENTRY_SIZE           5
#define QAX_LB_TC_DP_MAP_ENTRY_DATA_SET(_entry_data, _pri, _cfi, _dp) \
    (_entry_data = (_pri << 2) + (_cfi << 1) + _dp)
#define QAX_LB_TC_DP_MAP_ENTRY_DATA_GET(_entry_data, _pri, _cfi, _dp) { \
    _pri = (_entry_data >> 2);                                          \
    _cfi = (_entry_data >> 1) & 0x1;                                    \
    _dp  = (_entry_data & 0x1);                                         \
}
#define QAX_LB_EGR_LBG_CREDIT_ENTRY_SIZE      8

#define QAX_LB_PORT_VLAN_ENTRY_MODEM_SIZE     4
#define QAX_LB_PORT_VLAN_ENTRY_INDX(_port, _vlan) \
    ((_port << 1) + ((_vlan >> 3) & 0x1))
#define QAX_LB_PORT_VLAN_ENTRY_MODEM_OFFSET(_vlan) \
    ((QAX_LB_PORT_VLAN_ENTRY_MODEM_SIZE) * (_vlan & 0x7))

#define QAX_LB_EGQ_LBG_STAT_OFFSET(_lbg) \
    ((_lbg<<1) + 0)

#define QAX_ILB_LBRG_CONFIGURATION_INDEX(_lbg)    (6+_lbg)
#define QAX_ILB_FIFO_CNT_INDEX(_modem)            (16+_modem)
#define QAX_EPNI_LBG_FIFOS_EMPTY_INDEX(_lbg)      (_lbg<<1)
#define QAX_EPNI_LBG_CONFIGURATION_INDEX(_lbg)    (_lbg<<1)

#define QAX_LB_RX_LBG_TIMEOUT_MAX           ((1 << soc_reg_field_length(unit, ILB_LBRG_CONFIGURATIONr, LBRG_N_TIMEOUT_THf)) - 1)
#define QAX_LB_RX_LBG_SEQUENCE_MAX          ((1 << soc_reg_field_length(unit, ILB_LBRG_CONFIGURATIONr, LBRG_N_SEQUENCE_NUMf)) - 1)
#define QAX_LB_RX_LBG_OUT_OF_ORDER_MAX      ((1 << soc_reg_field_length(unit, ILB_LBRG_CONFIGURATIONr, LBRG_N_MAX_OUT_OF_ORDERf)) - 1)
#define QAX_LB_RX_LBG_BUFFER_MAX            ((1 << soc_reg_field_length(unit, ILB_LBRG_CONFIGURATIONr, LBRG_N_MAX_BUFFERSf)) - 1)

#define QAX_LB_TX_LBG_SEQUENCE_MAX          ((1 << soc_reg_field_length(unit, EPNI_SET_SEQUENCE_NUMBERr, SET_SEQ_NUM_VALUEf)) - 1)

typedef struct soc_qax_lb_stats_info_s {
    soc_reg_t         reg;
    soc_field_t       fld;
} soc_qax_lb_stats_info_t;

#define LB_EGR_LBG_FIFO_SIZE                   384
#define LB_EGR_FIFO_RANGE_PER_LBG              2
#define LB_EGR_FIFO_RANGE_IDX_BASE(__lbg_id)   (__lbg_id*2)
#define LB_EGQ_NOF_FIFO_ENTRY_MIN              10

#define LB_EGR_CREDIT_ENTRY_PER_LBG            2

typedef struct _soc_qax_lb_egr_lbg_fifo_range_s {
    int               fifo_start;
    int               nof_entries;
} _soc_qax_lb_egr_lbg_fifo_range_t;

soc_error_t
  _qax_lb_port_get_by_lbg_id(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT soc_port_t                  *port
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_port_t
        port_i = SOC_PORT_INVALID;
    soc_lbg_t
        lbg_id_i = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOCDNX_NULL_CHECK(port);

    BCM_PBMP_ITER(PBMP_LBG_ALL(unit), port_i) {
        rv = sw_state_access[unit].dpp.bcm.linkbonding.port_to_lbg.get(unit, port_i, &lbg_id_i);
        SOCDNX_IF_ERR_EXIT(rv);
        if (lbg_id_i == lbg_id) {
            *port = port_i;
            break;
        }
    }

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  _qax_lb_egr_lbg_credit_and_fifo_optimial_value_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          speed,
    SOC_SAND_OUT uint32                      *credit_size,
    SOC_SAND_OUT uint32                      *fifo_size
  )
{
    soc_error_t
        rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_CHECK(speed, 0, SOC_TMC_LB_LBG_SPEED_MAX, "expected_sequence_number");
    SOCDNX_NULL_CHECK(credit_size);
    SOCDNX_NULL_CHECK(fifo_size);

    
    if (speed <= 1000) {
        *fifo_size = 10;
        *credit_size = 20;
    } else if (speed <= 10000) {
        *fifo_size = 29;
        *credit_size = 58;
    } else if (speed <= 20000) {
        *fifo_size = 58;
        *credit_size = 116;
    } else if (speed <= 30000) {
        *fifo_size = 88;
        *credit_size = 174;
    }

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  _qax_lb_egr_lbg_enable_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  uint32                       is_enable
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0,
        fld_val32[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, (SOC_TMC_LB_LBG_MAX+1), "lbg_id");

    reg = EGQ_IFC_IS_LBGr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, IFC_IS_LBGf);
    if (is_enable) {
        SHR_BITSET(fld_val32, lbg_id);
    }
    else {
        SHR_BITCLR(fld_val32, lbg_id);
    }
    soc_reg_field_set(unit, reg, &reg_val32, IFC_IS_LBGf, *fld_val32);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    reg = EPNI_IFC_IS_LBGr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32));
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, IFC_IS_LBGf);
    if (is_enable) {
        SHR_BITSET(fld_val32, lbg_id);
    }
    else {
        SHR_BITCLR(fld_val32, lbg_id);
    }
    soc_reg_field_set(unit, reg, &reg_val32, IFC_IS_LBGf, *fld_val32);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  _qax_lb_egr_lbg_credit_size_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lbg_t                    credit_entry_cnt,
    SOC_SAND_IN  uint32                       credit_size
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    soc_reg_above_64_val_t
        reg_abv64,
        fld_abv64;
    uint32
        reg_val32 = 0,
        fld_val32[1] = {0};
    int
        j = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, (SOC_TMC_LB_LBG_MAX+1), "lbg_id");

    SOC_REG_ABOVE_64_CLEAR(reg_abv64);
    SOC_REG_ABOVE_64_CLEAR(fld_abv64);

    reg = EGQ_LBG_CREDIT_SIZEr;
    rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_abv64);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_above_64_field_get(unit, reg, reg_abv64, LBG_CREDIT_SIZEf, fld_abv64);
    fld_val32[0] = credit_size;
    for (j = 0; j < credit_entry_cnt; j++) {
        SHR_BITCOPY_RANGE(fld_abv64,
            QAX_LB_EGR_LBG_CREDIT_ENTRY_SIZE*LB_EGR_CREDIT_ENTRY_PER_LBG*lbg_id+QAX_LB_EGR_LBG_CREDIT_ENTRY_SIZE*j,
            fld_val32, 0, QAX_LB_EGR_LBG_CREDIT_ENTRY_SIZE);
    }
    soc_reg_above_64_field_set(unit, reg, reg_abv64, LBG_CREDIT_SIZEf, fld_abv64);
    rv = soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg_abv64);

    reg = EGQ_LBG_CREDIT_INITr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    *fld_val32 = 0;
    for (j = 0; j < credit_entry_cnt; j++) {
        SHR_BITSET(fld_val32, lbg_id*LB_EGR_CREDIT_ENTRY_PER_LBG + j);
    }

    soc_reg_field_set(unit, reg, &reg_val32, LBG_CREDIT_INITf, *fld_val32);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    reg = EGQ_LBG_CREDIT_INITr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    soc_reg_field_set(unit, reg, &reg_val32, LBG_CREDIT_INITf, 0);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

int _lb_egr_lbg_fifo_range_cmpfunc (void * a, void * b)
{
    return (((_soc_qax_lb_egr_lbg_fifo_range_t*)(a))->nof_entries - ((_soc_qax_lb_egr_lbg_fifo_range_t*)(b))->nof_entries);
}

soc_error_t
  _qax_lb_egr_lbg_fifo_free_range_get (
    SOC_SAND_IN     int                                  unit,
    SOC_SAND_IN     soc_lbg_t                            lbg_id,
    SOC_SAND_INOUT  _soc_qax_lb_egr_lbg_fifo_range_t    *fifo_ranges,
    SOC_SAND_IN     int                                  max_nof_ranges,
    SOC_SAND_OUT    int                                 *nof_ranges
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    int 
        fifo_idx = 0,
        fifo_range_i = 0,
        free_entry_cnt = 0, 
        first_free_index = -1, 
        nof_ranges_l = 0,
        lb_egr_fifo_start_tmp = 0, 
        lb_egr_fifo_nof_entries_tmp = 0;
    SHR_BITDCL
        lb_egr_fifo_entry_bmp_used[ARAD_BIT_TO_U32(LB_EGR_LBG_FIFO_SIZE+1)] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, (SOC_TMC_LB_LBG_MAX+1), "lbg_id");
    SOCDNX_NULL_CHECK(fifo_ranges);
    SOCDNX_NULL_CHECK(nof_ranges);

    for (fifo_range_i = 0; fifo_range_i < LB_EGR_NOF_FIFO_RANGE_MAX; fifo_range_i++) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_start.get(unit, fifo_range_i, &lb_egr_fifo_start_tmp));
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_nof_entries.get(unit, fifo_range_i, &lb_egr_fifo_nof_entries_tmp));
        LOG_VERBOSE(BSL_LS_SOC_LB, (BSL_META_U(unit, "Get sw db: fifo_range_i(%d) lb_egr_lbg_fifo_start(%d, %x) lb_egr_lbg_fifo_nof_entries(%d, %x)\n"), 
            fifo_range_i, lb_egr_fifo_start_tmp, lb_egr_fifo_start_tmp, lb_egr_fifo_nof_entries_tmp, lb_egr_fifo_nof_entries_tmp));
        if ((0 == lb_egr_fifo_nof_entries_tmp) || ((fifo_range_i/LB_EGR_FIFO_RANGE_PER_LBG) == lbg_id)) {
            continue;
        }

        SHR_BITSET_RANGE(lb_egr_fifo_entry_bmp_used, lb_egr_fifo_start_tmp, lb_egr_fifo_nof_entries_tmp);
    }
    SHR_BITSET(lb_egr_fifo_entry_bmp_used, LB_EGR_LBG_FIFO_SIZE);

    for (fifo_idx = 0; fifo_idx < (LB_EGR_LBG_FIFO_SIZE+1); fifo_idx++) {
        if (lb_egr_fifo_entry_bmp_used[fifo_idx/SHR_BITWID] == 0) {
            if (0 == free_entry_cnt) {
                first_free_index = fifo_idx;
            }
            fifo_idx += (SHR_BITWID - 1); 
            free_entry_cnt += SHR_BITWID;
        } else if (!SHR_BITGET(lb_egr_fifo_entry_bmp_used, fifo_idx)) {
            if (0 == free_entry_cnt) {
                first_free_index = fifo_idx;
            }
            free_entry_cnt++;
        }
        else {
            if (free_entry_cnt >= LB_EGQ_NOF_FIFO_ENTRY_MIN) {
                if (nof_ranges_l < max_nof_ranges) {
                    fifo_ranges[nof_ranges_l].fifo_start  = first_free_index;
                    fifo_ranges[nof_ranges_l].nof_entries = free_entry_cnt;
                    nof_ranges_l++;
                }
                else {
                    break;
                }
            }
            first_free_index = fifo_idx;
            free_entry_cnt = 0;
        }
    }

    
    soc_sand_os_qsort(fifo_ranges, nof_ranges_l, sizeof(_soc_qax_lb_egr_lbg_fifo_range_t), &_lb_egr_lbg_fifo_range_cmpfunc);

    *nof_ranges = nof_ranges_l;

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  _qax_lb_egr_lbg_fifo_alloc (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          nof_fifo_range_req,
    SOC_SAND_IN  uint32                       nof_fifo_entries
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    _soc_qax_lb_egr_lbg_fifo_range_t
        fifo_ranges_avail[LB_EGR_NOF_FIFO_RANGE_MAX+1];
    int 
        nof_range_avail = 0,
        fifo_range_i = 0,
        range_i = 0,
        nof_matched = 0,
        first_entry_index[2] = {0}, 
        j = 0,
        is_succ = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, (SOC_TMC_LB_LBG_MAX+1), "lbg_id");

    SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_fifo_free_range_get(unit, lbg_id, fifo_ranges_avail, 14, &nof_range_avail));

    for (range_i = 0; range_i < nof_range_avail; range_i++) {
        if (fifo_ranges_avail[range_i].nof_entries >= nof_fifo_entries) {
            first_entry_index[nof_matched] = fifo_ranges_avail[range_i].fifo_start;
            nof_matched++;
            if (nof_matched >= nof_fifo_range_req) {
                is_succ = 1;
                break;
            }
        }
    }

    if (nof_matched < nof_fifo_range_req) {
        for (range_i = 0; range_i < nof_range_avail; range_i++) {
            if (fifo_ranges_avail[range_i].nof_entries >= (nof_fifo_entries*nof_fifo_range_req)) {
                is_succ = 1;
                for (j = 0; j < nof_fifo_range_req; j++) {
                    if (j == 0) {
                        first_entry_index[j] = fifo_ranges_avail[range_i].fifo_start;
                    }
                    else {
                        first_entry_index[j] = first_entry_index[j-1] + nof_fifo_entries;
                    }
                }
                break;
            }
        }
    }

    if (is_succ == 1) {
        
        for (j = 0; j < nof_fifo_range_req; j++) {
            fifo_range_i = LB_EGR_FIFO_RANGE_IDX_BASE(lbg_id) + j;
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_start.set(unit, fifo_range_i, first_entry_index[j]));
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_nof_entries.set(unit, fifo_range_i, nof_fifo_entries));

            LOG_VERBOSE(BSL_LS_SOC_LB, (BSL_META_U(unit, "Set sw db: fifo_range_i(%d) lb_egr_lbg_fifo_start(%d, %x) lb_egr_lbg_fifo_nof_entries(%d, %x)\n"), 
                fifo_range_i, first_entry_index[j], first_entry_index[j], nof_fifo_entries, nof_fifo_entries));
        }
    } else {
        rv = SOC_E_RESOURCE;
        SOCDNX_EXIT_WITH_ERR(rv, 
            (_BSL_SOCDNX_MSG("Fail(%s) Not enough free entries in lbg fifo.\n"), soc_errmsg(rv)));
    }

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  _qax_lb_egr_lbg_fifo_free (
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          nof_fifo_range_req
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    int 
        fifo_range_i = 0,
        range_i = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, (SOC_TMC_LB_LBG_MAX+1), "lbg_id");

    for (range_i = 0; range_i < nof_fifo_range_req; range_i++) {
        fifo_range_i = LB_EGR_FIFO_RANGE_IDX_BASE(lbg_id) + range_i;
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_start.set(unit, fifo_range_i, 0));
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_nof_entries.set(unit, fifo_range_i, 0));
        LOG_VERBOSE(BSL_LS_SOC_LB, (BSL_META_U(unit, "Set sw db: fifo_range_i(%d) lb_egr_lbg_fifo_start(%d, %x) lb_egr_lbg_fifo_nof_entries(%d, %x)\n"), 
            fifo_range_i, 0, 0, 0, 0)); 
    }

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  _qax_lb_egr_lbg_fifo_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          fifo_range_cnt
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    soc_reg_above_64_val_t
        entry_data,
        reg_abv64, 
        fld_abv64;
    uint32
        reg_val32 = 0, 
        first_fifo = 0,
        last_fifo = 0;
    int
        fifo_range_i = 0,
        j = 0, 
        lb_egr_fifo_start_tmp = 0, 
        lb_egr_fifo_nof_entries_tmp = 0;
    soc_field_t
        fld = INVALIDf,
        lbg_fifo_wr_flds[] = {
            LBG_FIFO_CONFIG_WR_0f, LBG_FIFO_CONFIG_WR_1f, LBG_FIFO_CONFIG_WR_2f, LBG_FIFO_CONFIG_WR_3f, 
            LBG_FIFO_CONFIG_WR_4f, LBG_FIFO_CONFIG_WR_5f, LBG_FIFO_CONFIG_WR_6f, LBG_FIFO_CONFIG_WR_7f,
            LBG_FIFO_CONFIG_WR_8f, LBG_FIFO_CONFIG_WR_9f, LBG_FIFO_CONFIG_WR_10f, LBG_FIFO_CONFIG_WR_11f,
            LBG_FIFO_CONFIG_WR_12f};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, (SOC_TMC_LB_LBG_MAX+1), "lbg_id");

    SOC_REG_ABOVE_64_CLEAR(reg_abv64);
    SOC_REG_ABOVE_64_CLEAR(fld_abv64);

    for (j = 0; j < fifo_range_cnt; j++) {
        fifo_range_i = LB_EGR_FIFO_RANGE_IDX_BASE(lbg_id) + j;

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_start.get(unit, fifo_range_i, &lb_egr_fifo_start_tmp));
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_nof_entries.get(unit, fifo_range_i, &lb_egr_fifo_nof_entries_tmp));
        LOG_VERBOSE(BSL_LS_SOC_LB, (BSL_META_U(unit, "Get sw db: fifo_range_i(%d) lb_egr_lbg_fifo_start(%d, %x) lb_egr_lbg_fifo_nof_entries(%d, %x)\n"), 
            fifo_range_i, lb_egr_fifo_start_tmp, lb_egr_fifo_start_tmp, lb_egr_fifo_nof_entries_tmp, lb_egr_fifo_nof_entries_tmp));

        first_fifo = lb_egr_fifo_start_tmp;
        last_fifo = first_fifo + lb_egr_fifo_nof_entries_tmp - 1;

        SOC_REG_ABOVE_64_CLEAR(entry_data);
        SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, EPNI_LBG_FIFO_CONFIGm, MEM_BLOCK_ANY, fifo_range_i, entry_data));
        soc_mem_field32_set(unit, EPNI_LBG_FIFO_CONFIGm, entry_data, LBG_FIFO_FIRST_POINTERf, first_fifo);
        soc_mem_field32_set(unit, EPNI_LBG_FIFO_CONFIGm, entry_data, LBG_FIFO_LAST_POINTERf, last_fifo);
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, EPNI_LBG_FIFO_CONFIGm, MEM_BLOCK_ANY, fifo_range_i, entry_data));
    }

    reg = EPNI_LBG_FIFO_CONFIGURATION_WRITEr;
    SOCDNX_IF_ERR_EXIT(READ_EPNI_LBG_FIFO_CONFIGURATION_WRITEr(unit, &reg_val32));
    for (j = 0; j < fifo_range_cnt; j++) {
        fld = lbg_fifo_wr_flds[lbg_id*LB_EGR_FIFO_RANGE_PER_LBG + j];
        soc_reg_field_set(unit, reg, &reg_val32, fld, 1);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_LBG_FIFO_CONFIGURATION_WRITEr(unit, reg_val32));

    reg_val32 = 0;
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_LBG_FIFO_CONFIGURATION_WRITEr(unit, reg_val32));

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  qax_lb_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    soc_field_t
        fld = INVALIDf;
    soc_reg_above_64_val_t
        reg_abv64,
        fld_abv64,
        mem_data;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    int 
        reg_idx = 0;
    uint32
        reg_val32 = 0, 
        freq_mhz = 0,
        fld_len = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    reg = SOC_IS_QUX(unit) ? ILB_REG_0192r : ILB_REG_0148r;
    fld = FIELD_0_7f;
    for (reg_idx = 0; reg_idx < SOC_REG_NUMELS(unit, reg) ; reg_idx++) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, reg_idx, 0xff));
    }

    reg = SOC_IS_QUX(unit) ? EPNI_REG_012Dr : EPNI_REG_0129r;
    fld = FIELD_16_31f;
    soc_reg_field_set(unit, reg, &reg_val32, fld, 0x0);
    fld = FIELD_0_15f;
    soc_reg_field_set(unit, reg, &reg_val32, fld, 0xffff);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    
    reg = ILB_LBRG_CONFIGURATIONr;
    SOC_REG_ABOVE_64_CLEAR(reg_abv64);
    for (reg_idx = 0; reg_idx < SOC_REG_NUMELS(unit, reg) ; reg_idx++) {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, REG_PORT_ANY, reg_idx, reg_abv64));
        soc_reg_above_64_field32_set(unit, reg, reg_abv64, LBRG_N_SEQUENCE_NUMf, 0);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, REG_PORT_ANY, reg_idx, reg_abv64));
    }

    
    freq_mhz = arad_chip_kilo_ticks_per_sec_get(unit)/1000;

    reg = ILB_MISC_CONFIG_REGr;
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        soc_reg64_field32_set(unit, reg, &reg_val64, CLOCK_FREQf, freq_mhz);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val64));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        soc_reg_field_set(unit, reg, &reg_val32, CLOCK_FREQf, freq_mhz);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));
    }

    if (SOC_IS_QUX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,0), SOC_QAX_EGR_IF_LBG_RESERVE2, mem_data));
        soc_mem_field32_set(unit, EGQ_PER_IFC_CFGm, mem_data, NIF_CANCEL_EN_1f, 0);
        soc_mem_field32_set(unit, EGQ_PER_IFC_CFGm, mem_data, NIF_CANCEL_EN_2f, 0);
        soc_mem_field32_set(unit, EGQ_PER_IFC_CFGm, mem_data, NIF_CANCEL_EN_3f, 0);
        soc_mem_field32_set(unit, EGQ_PER_IFC_CFGm, mem_data, NIF_CANCEL_EN_4f, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,0), SOC_QAX_EGR_IF_LBG_RESERVE2, mem_data));

        reg = EGQ_IFC_IS_WORK_CONSERVINGr;
        SOC_REG_ABOVE_64_CLEAR(fld_abv64);
        fld_len = soc_reg_field_length(unit, reg, IFC_IS_WORK_CONSERVINGf)-1;
        SHR_BITSET_RANGE(fld_abv64, 0, fld_len);

        soc_reg_above_64_field_set(unit, reg, reg_abv64, IFC_IS_WORK_CONSERVINGf, fld_abv64);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, REG_PORT_ANY, reg_idx, reg_abv64));

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, 0, 0, WORK_CONSERVING_MODE_CANCEL_ENf, 0x0));

        reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        soc_reg64_field32_set(unit, reg, &reg_val64, FIELD_35_35f, 0);
        soc_reg64_field32_set(unit, reg, &reg_val64, FIELD_36_36f, 0);
        soc_reg64_field32_set(unit, reg, &reg_val64, FIELD_37_37f, 0);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val64));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_init(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        flags = 0,
        tm_port = 0,
        egress_offset = 0;
    soc_pbmp_t
        pbmp;
    soc_port_t
        port_i;
    int
        core = 0,
        speed = 0,
        is_alloc_fifo = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_PCID_LITE_SKIP(unit);

    if (!SOC_WARM_BOOT(unit)) {
        rv = qax_lb_regs_init(unit);
        SOCDNX_IF_ERR_EXIT(rv);

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));

        SOC_PBMP_ITER(pbmp, port_i) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));

            if (SOC_PORT_IS_LB_MODEM(flags)) {
                
                SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_map_set(unit, port_i, FALSE, FALSE));
            }
            else if (SOC_DPP_CONFIG(unit)->qax->lbi_en && (is_alloc_fifo == 0)) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

                if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port_i, &speed));
                    SOCDNX_IF_ERR_EXIT(qax_lb_port_speed_set(unit, port_i, speed));
                    is_alloc_fifo = 1;
                }
            }
            else {
                continue;
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_init(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0;
    int 
        ilb_buffer_size = 0, 
        ilb_buffer_size_hw = 0,
        ilb_buffer_size_single = 0, 
        ilb_buffer_size_single_hw = 0;
    uint32
        fifo_size = 0, 
        total_fifo_size = 0,
        start_fifo = 0;
    int
        fifo_i = 0, 
        j = 0, 
        fifo_cnt_per_modem = 2;
    soc_modem_t
        modem_id = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    ilb_buffer_size = soc_property_get(unit, spn_LB_BUFFER_SIZE, 8);
    switch (ilb_buffer_size) {
    case 0:   
    case 2:   
    case 4:   
    case 6:   
    case 8:   
    case 10:  
    case 12:  
    case 14:  
    case 16:  
        if ((!SOC_IS_QUX(unit)) && (ilb_buffer_size > 8)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG(
                "Unexpected property value (\"%d\") for %s"), ilb_buffer_size, "lb_buffer_size"));     
        }
        ilb_buffer_size_hw = ilb_buffer_size/2;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG(
            "Unexpected property value (\"%d\") for %s"), ilb_buffer_size, "lb_buffer_size"));
    }

    reg = ECI_GLOBAL_GENERAL_CFG_3r;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    soc_reg_field_set(unit, reg, &reg_val32, ILB_BUFFER_SIZEf, ilb_buffer_size_hw);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));


    if (ilb_buffer_size > 0) {
        if (SOC_IS_QUX(unit)) {
            
            ilb_buffer_size_single = soc_property_get(unit, spn_LB_INGRESS_BUFFER_SIZE_SINGLE, 256);
            switch (ilb_buffer_size_single) {
            case 256:
                ilb_buffer_size_single_hw = 0;
                break;
            case 128:
                ilb_buffer_size_single_hw = 1;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG(
                    "Unexpected property value (\"%d\") for %s"), ilb_buffer_size_single, "lb_ingress_buffer_size_single"));
            }
		
            reg = ECI_GLOBAL_GENERAL_CFG_3r;
            SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
            soc_reg_field_set(unit, reg, &reg_val32, ILB_BUFFER_MODEf, ilb_buffer_size_single_hw);
            SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));
        }

        
        for (modem_id = 0; modem_id < SOC_TMC_LB_NOF_MODEM; modem_id++) {
            fifo_size = soc_property_suffix_num_get(unit, modem_id, spn_LB_MODEM_FIFO_SIZE, "fifo", 512);
            total_fifo_size += fifo_size;
            if (total_fifo_size > 16384) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Total fifo size should be in range of 0 to 16384")));
            }
        }

        reg = ILB_MODEM_FIFO_CONFIGr;
        start_fifo = 0;
        for (modem_id = 0; modem_id < SOC_TMC_LB_NOF_MODEM; modem_id++) {
            fifo_size = soc_property_suffix_num_get(unit, modem_id, spn_LB_MODEM_FIFO_SIZE, "fifo", 512);

            for (j = 0; j < fifo_cnt_per_modem; j++) {
                fifo_i = modem_id + j*SOC_TMC_LB_NOF_MODEM;
                SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, fifo_i, &reg_val32));
                if (!SOC_IS_QUX(unit)) {
                    soc_reg_field_set(unit, reg, &reg_val32, MODEM_N_START_PTRf, start_fifo);
                }
                soc_reg_field_set(unit, reg, &reg_val32, MODEM_N_FIFO_SIZEf, (fifo_size/2 - 1));
                SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, fifo_i, reg_val32));
                start_fifo += (fifo_size/fifo_cnt_per_modem);

            }
        }

        reg = ILB_ILB_INIT_REGr;
        SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, reg, REG_PORT_ANY, FPC_INITf, 0x0));
        SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, reg, REG_PORT_ANY, FPC_INITf, 0x1));
        SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, reg, REG_PORT_ANY, FPC_INITf, 0x0));
        SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, reg, REG_PORT_ANY, ILB_INITf, 0x0));
        SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, reg, REG_PORT_ANY, ILB_INITf, 0x1));
        SOCDNX_IF_ERR_EXIT( soc_reg_field32_modify(unit, reg, REG_PORT_ANY, ILB_INITf, 0x0));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_init(
    SOC_SAND_IN  int                 unit
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        reg_val32 = 0;
    soc_reg_above_64_val_t
        entry_data;
    uint32
        fifo_size = 0, 
        total_fifo_size = 0,
        first_fifo = 0, 
        last_fifo = 0;
    int
        fifo_i = 0, 
        j = 0, 
        fifo_cnt_per_lbg = 2;
    soc_lbg_t
        lbg_id = 0;

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QUX(unit)) {
        SOC_EXIT;
    }

    for (lbg_id = 0; lbg_id < SOC_TMC_LB_NOF_LBG; lbg_id++) {
        fifo_size = soc_property_suffix_num_get(unit, lbg_id, spn_LB_LBG_FIFO_SIZE, "fifo", 30);
        total_fifo_size += fifo_size;
        if (total_fifo_size > 384) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("Total fifo size should be in range of 0 to 384")));
        }
    }

    for (lbg_id = 0; lbg_id < SOC_TMC_LB_NOF_LBG; lbg_id++) {
        fifo_size = soc_property_suffix_num_get(unit, lbg_id, spn_LB_LBG_FIFO_SIZE, "fifo", 30);

        for (j = 0; j < fifo_cnt_per_lbg; j++) {
            fifo_i = lbg_id*fifo_cnt_per_lbg + j;
            last_fifo = (first_fifo + fifo_size/fifo_cnt_per_lbg - 1);

            SOC_REG_ABOVE_64_CLEAR(entry_data);
            SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, EPNI_LBG_FIFO_CONFIGm, MEM_BLOCK_ANY, fifo_i, entry_data));
            soc_mem_field32_set(unit, EPNI_LBG_FIFO_CONFIGm, entry_data, LBG_FIFO_FIRST_POINTERf, first_fifo);
            soc_mem_field32_set(unit, EPNI_LBG_FIFO_CONFIGm, entry_data, LBG_FIFO_LAST_POINTERf, last_fifo);
            SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, EPNI_LBG_FIFO_CONFIGm, MEM_BLOCK_ANY, fifo_i, entry_data));

            first_fifo = (last_fifo + 1);
        }
    }

    first_fifo = total_fifo_size;
    last_fifo = 383;
    fifo_i = 12;
    SOC_REG_ABOVE_64_CLEAR(entry_data);
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, EPNI_LBG_FIFO_CONFIGm, MEM_BLOCK_ANY, fifo_i, entry_data));
    soc_mem_field32_set(unit, EPNI_LBG_FIFO_CONFIGm, entry_data, LBG_FIFO_FIRST_POINTERf, first_fifo);
    soc_mem_field32_set(unit, EPNI_LBG_FIFO_CONFIGm, entry_data, LBG_FIFO_LAST_POINTERf, last_fifo);
    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, EPNI_LBG_FIFO_CONFIGm, MEM_BLOCK_ANY, fifo_i, entry_data));

    SOCDNX_IF_ERR_EXIT(READ_EPNI_LBG_FIFO_CONFIGURATION_WRITEr(unit, &reg_val32));
    soc_reg_field_set(unit, EPNI_LBG_FIFO_CONFIGURATION_WRITEr, &reg_val32, LBG_FIFO_CONFIG_WRf, 0x1fff);
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_LBG_FIFO_CONFIGURATION_WRITEr(unit, reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_EPNI_LBG_FIFO_CONFIGURATION_WRITEr(unit, &reg_val32));
    soc_reg_field_set(unit, EPNI_LBG_FIFO_CONFIGURATION_WRITEr, &reg_val32, LBG_FIFO_CONFIG_WRf, 0x0);
    SOCDNX_IF_ERR_EXIT(WRITE_EPNI_LBG_FIFO_CONFIGURATION_WRITEr(unit, reg_val32));

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_total_shared_buffer_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 nof_buffers
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(nof_buffers, SOC_TMC_LB_ING_NOF_TOTAL_SHARED_BUFFER_MAX, "nof_buffers");

    reg = ILB_MISC_CONFIG_REGr;
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
    soc_reg64_field32_set(unit, reg, &reg_val64, TOTAL_SHAREDf, nof_buffers); 
    SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val64));

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_total_shared_buffer_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT int                                *nof_buffers
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOCDNX_NULL_CHECK(nof_buffers);

    reg = ILB_MISC_CONFIG_REGr;
    SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
    *nof_buffers = soc_reg64_field32_get(unit, reg, reg_val64, TOTAL_SHAREDf); 

    SOCDNX_IF_ERR_EXIT(rv);
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_guaranteed_buffer_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_IN  int                                 nof_buffers
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        lbg_data;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOC_TMC_LB_VALUE_MAX_CHECK(nof_buffers, SOC_TMC_LB_ING_NOF_GUARANTEED_BUFFER_MAX, "nof_buffers");

    SOC_REG_ABOVE_64_CLEAR(lbg_data);
    rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_above_64_field32_set(unit, lbg_reg, lbg_data, LBRG_N_GUARANTEED_BUFFERSf, nof_buffers); 
    rv = soc_reg_above_64_set(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_guaranteed_buffer_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_OUT int                                *nof_buffers
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        lbg_data;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOCDNX_NULL_CHECK(nof_buffers);

    SOC_REG_ABOVE_64_CLEAR(lbg_data);
    rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);
    *nof_buffers = soc_reg_above_64_field32_get(unit, lbg_reg, lbg_data, LBRG_N_GUARANTEED_BUFFERSf); 

 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_modem_to_lbg_map_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        lbg_data;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    uint32
        active_modems[1] = {0}; 

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    SOC_REG_ABOVE_64_CLEAR(lbg_data);

    rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);

    *active_modems = soc_reg_above_64_field32_get(unit, lbg_reg, lbg_data, LBRG_N_ACTIVE_MODEMSf);
    SHR_BITSET(active_modems, modem_id);
    soc_reg_above_64_field32_set(unit, lbg_reg, lbg_data, LBRG_N_ACTIVE_MODEMSf, *active_modems); 

    rv = soc_reg_above_64_set(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_modem_to_lbg_unmap_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        lbg_data;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    uint32
        active_modems[1] = {0}; 

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    SOC_REG_ABOVE_64_CLEAR(lbg_data);

    rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);

    *active_modems = soc_reg_above_64_field32_get(unit, lbg_reg, lbg_data, LBRG_N_ACTIVE_MODEMSf);
    SHR_BITCLR(active_modems, modem_id);
    soc_reg_above_64_field32_set(unit, lbg_reg, lbg_data, LBRG_N_ACTIVE_MODEMSf, *active_modems); 

    rv = soc_reg_above_64_set(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_modem_to_lbg_map_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT soc_lbg_t                   *lbg_id
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        lbg_data;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    uint32
        active_modems[1] = {0};
    soc_lbg_t
        lbg_id_tmp = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOCDNX_NULL_CHECK(lbg_id);

    *lbg_id = SOC_TMC_LB_LBG_INVALID;

    SOC_REG_ABOVE_64_CLEAR(lbg_data);

    for (lbg_id_tmp = 0; lbg_id_tmp < (SOC_TMC_LB_LBG_MAX+1); lbg_id_tmp++) {
        rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id_tmp), lbg_data);
        SOCDNX_IF_ERR_EXIT(rv);

        *active_modems = soc_reg_above_64_field32_get(unit, lbg_reg, lbg_data, LBRG_N_ACTIVE_MODEMSf);
        if (SHR_BITGET(active_modems, modem_id)) {
            *lbg_id = lbg_id_tmp;
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_expected_seqeunce_num_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          expected_seq_num
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0,
        fld_val32[1] = {0};
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    soc_reg_above_64_val_t
        lbg_data;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOC_TMC_LB_VALUE_CHECK(expected_seq_num, 0, QAX_LB_RX_LBG_SEQUENCE_MAX, "expected_sequence_number");

    reg = ILB_RESERVED_SPARE_0r;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, RESERVED_SPARE_0f);
    SHR_BITSET(fld_val32, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id));
    soc_reg_field_set(unit, reg, &reg_val32, RESERVED_SPARE_0f, *fld_val32);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    SOC_REG_ABOVE_64_CLEAR(lbg_data);
    rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_above_64_field32_set(unit, lbg_reg, lbg_data, LBRG_N_SEQUENCE_NUMf, expected_seq_num);
    rv = soc_reg_above_64_set(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);

    reg = ILB_RESERVED_SPARE_0r;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, RESERVED_SPARE_0f);
    SHR_BITCLR(fld_val32, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id));
    soc_reg_field_set(unit, reg, &reg_val32, RESERVED_SPARE_0f, *fld_val32);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_expected_seqeunce_num_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT int                         *expected_seq_num
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    soc_reg_above_64_val_t
        lbg_data;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOCDNX_NULL_CHECK(expected_seq_num);

    SOC_REG_ABOVE_64_CLEAR(lbg_data);

    rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);
    *expected_seq_num = soc_reg_above_64_field32_get(unit, lbg_reg, lbg_data, LBRG_N_SEQUENCE_NUMf);

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_glb_seqeunce_num_width_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          num_width
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0, 
        hw_val = 0;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_CHECK(num_width, SOC_TMC_LB_ING_SEQUENCE_NUM_WIDTH_MIN, SOC_TMC_LB_ING_SEQUENCE_NUM_WIDTH_MAX, "num_width");

    hw_val = SOC_TMC_LB_ING_SEQUENCE_NUM_WIDTH_MAX - num_width;

    reg = ILB_MISC_CONFIG_REGr;
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        soc_reg64_field32_set(unit, reg, &reg_val64, SEQ_NUM_WRAP_WIDTHf, hw_val); 
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val64));
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        soc_reg_field_set(unit, reg, &reg_val32, SEQ_NUM_WRAP_WIDTHf, hw_val);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));
    }

    SOCDNX_IF_ERR_EXIT(rv);
	
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_glb_seqeunce_num_width_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT int                         *num_width
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0, 
        hw_val = 0;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(num_width);

    reg = ILB_MISC_CONFIG_REGr;
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        hw_val = soc_reg64_field32_get(unit, reg, reg_val64, SEQ_NUM_WRAP_WIDTHf); 
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        hw_val = soc_reg_field_get(unit, reg, reg_val32, SEQ_NUM_WRAP_WIDTHf);
    }

    *num_width = SOC_TMC_LB_ING_SEQUENCE_NUM_WIDTH_MAX - hw_val;

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_packet_to_modem_map_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lb_rx_modem_map_index_t      *map_index,
    SOC_SAND_IN  soc_lb_rx_modem_map_config_t     *map_config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    mem = INVALIDm;
    uint32
        entry_data = 0, 
        fld_data = 0;
    uint32
        modem = 0, 
        lbg_type = 0;
    soc_reg_above_64_val_t
        entry_data_abv64;
    uint32
        phy_port;
    int
        entry_index = 0,
        data_offset = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(map_index);
    SOCDNX_NULL_CHECK(map_config);
    SOC_TMC_LB_VALUE_CHECK(map_index->vlan, BCM_VLAN_DEFAULT, (BCM_VLAN_INVALID-1), "map_index->vlan");
    SOC_TMC_LB_VALUE_MAX_CHECK(map_config->modem_id, SOC_TMC_LB_MODEM_MAX, "map_config->modem_id");

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, map_index->port, &phy_port ));
    phy_port--; 

    mem = ILB_PORT_CONFIG_TABLEm;
    entry_index = phy_port;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_index, &entry_data);
    SOCDNX_IF_ERR_EXIT(rv);

    if (socLbgFormatTypeBypass == map_config->lbg_type) {
        modem = 0;
        lbg_type = 0;
    }
    else if (socLbgFormatTypeNonChannelize == map_config->lbg_type) {
        modem = map_config->modem_id;
        lbg_type = 1;
    }
    else if (socLbgFormatTypeChannelize == map_config->lbg_type) {
        modem = 0;
        lbg_type = 2;
    }
    soc_mem_field32_set(unit, mem, &entry_data, MODEMf, modem);
    soc_mem_field32_set(unit, mem, &entry_data, LB_TYPEf, lbg_type);
 
    rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_index, &entry_data);
    SOCDNX_IF_ERR_EXIT(rv);

    if (socLbgFormatTypeChannelize == map_config->lbg_type) {
        mem = ILB_VLAN_CONFIG_TABLEm;
        modem = map_config->modem_id;
        SOC_REG_ABOVE_64_CLEAR(entry_data_abv64);
        entry_index = QAX_LB_PORT_VLAN_ENTRY_INDX(phy_port, map_index->vlan);
        data_offset = QAX_LB_PORT_VLAN_ENTRY_MODEM_OFFSET(map_index->vlan);

        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_index, &entry_data);
        SOCDNX_IF_ERR_EXIT(rv);

        fld_data = soc_mem_field32_get(unit, mem, &entry_data, MODEMf);
        SHR_BITCOPY_RANGE(&fld_data, data_offset, &modem, 0, QAX_LB_PORT_VLAN_ENTRY_MODEM_SIZE);
        soc_mem_field32_set(unit, mem, &entry_data, MODEMf, fld_data);

        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_index, &entry_data);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_packet_to_modem_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lb_rx_modem_map_index_t         *map_index,
    SOC_SAND_INOUT  soc_lb_rx_modem_map_config_t     *map_config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    mem = INVALIDm;
    uint32
        entry_data = 0, 
        fld_data = 0;
    uint32
        modem = 0, 
        lbg_type = 0;
    soc_reg_above_64_val_t
        entry_data_abv64;
    uint32
        phy_port;
    int
        entry_index = 0,
        data_offset = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(map_index);
    SOCDNX_NULL_CHECK(map_config);
    SOC_TMC_LB_VALUE_CHECK(map_index->vlan, BCM_VLAN_DEFAULT, (BCM_VLAN_INVALID-1), "map_index->vlan");

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, map_index->port, &phy_port ));
    phy_port--; 

    mem = ILB_PORT_CONFIG_TABLEm;
    entry_index = phy_port;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_index, &entry_data);
    SOCDNX_IF_ERR_EXIT(rv);

    lbg_type = soc_mem_field32_get(unit, mem, &entry_data, LB_TYPEf);
    modem = soc_mem_field32_get(unit, mem, &entry_data, MODEMf);

    if (socLbgFormatTypeBypass == lbg_type) {
        map_config->lbg_type = socLbgFormatTypeBypass;
        map_config->modem_id = SOC_TMC_LB_MODEM_INVALID;
    }
    else if (socLbgFormatTypeNonChannelize == lbg_type) {
        map_config->lbg_type = socLbgFormatTypeNonChannelize;
        map_config->modem_id = modem;
    }
    else if (socLbgFormatTypeChannelize == lbg_type) {
        mem = ILB_VLAN_CONFIG_TABLEm;
        SOC_REG_ABOVE_64_CLEAR(entry_data_abv64);
        entry_index = QAX_LB_PORT_VLAN_ENTRY_INDX(phy_port, map_index->vlan);
        data_offset = QAX_LB_PORT_VLAN_ENTRY_MODEM_OFFSET(map_index->vlan);

        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_index, &entry_data);
        SOCDNX_IF_ERR_EXIT(rv);

        fld_data = soc_mem_field32_get(unit, mem, &entry_data, MODEMf);
        SHR_BITCOPY_RANGE(&modem, 0, &fld_data, data_offset, QAX_LB_PORT_VLAN_ENTRY_MODEM_SIZE);

        map_config->lbg_type = socLbgFormatTypeChannelize;
        map_config->modem_id = modem;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_reorder_config_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_OUT soc_lb_ing_reorder_config_t     *reorder_config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    soc_reg_above_64_val_t
        lbg_data;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(reorder_config);
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    SOC_REG_ABOVE_64_CLEAR(lbg_data);

    rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);
    reorder_config->timeout_thresh = soc_reg_above_64_field32_get(unit, lbg_reg, lbg_data, LBRG_N_TIMEOUT_THf);
    reorder_config->max_out_of_order = soc_reg_above_64_field32_get(unit, lbg_reg, lbg_data, LBRG_N_MAX_OUT_OF_ORDERf);
    reorder_config->max_buffer = soc_reg_above_64_field32_get(unit, lbg_reg, lbg_data, LBRG_N_MAX_BUFFERSf);

    SOCDNX_IF_ERR_EXIT(rv);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_reorder_config_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_IN  soc_lb_ing_reorder_config_t     *reorder_config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    soc_reg_above_64_val_t
        lbg_data;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(reorder_config);
    SOC_TMC_LB_VALUE_MAX_CHECK(reorder_config->timeout_thresh, QAX_LB_RX_LBG_TIMEOUT_MAX, "timeout_thresh");
    SOC_TMC_LB_VALUE_MAX_CHECK(reorder_config->max_out_of_order, QAX_LB_RX_LBG_OUT_OF_ORDER_MAX, "max_out_of_order");
    SOC_TMC_LB_VALUE_MAX_CHECK(reorder_config->max_buffer, QAX_LB_RX_LBG_BUFFER_MAX, "max_buffer");

    SOC_REG_ABOVE_64_CLEAR(lbg_data);

    rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_above_64_field32_set(unit, lbg_reg, lbg_data, LBRG_N_TIMEOUT_THf, reorder_config->timeout_thresh);
    soc_reg_above_64_field32_set(unit, lbg_reg, lbg_data, LBRG_N_MAX_OUT_OF_ORDERf, reorder_config->max_out_of_order);
    soc_reg_above_64_field32_set(unit, lbg_reg, lbg_data, LBRG_N_MAX_BUFFERSf, reorder_config->max_buffer);
    rv = soc_reg_above_64_set(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_flush_config_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_flush_type_t          type,
    SOC_SAND_IN  int                          enable
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    soc_reg_above_64_val_t
        lbg_data;
    soc_field_t
        flush_flds[] = {
            LBRG_N_FORCE_FLUSHf, 
            LBRG_N_FLUSH_ON_GROUP_OVERFLOWf,
            LBRG_N_FLUSH_ON_FIFO_OVERFLOWf,
            LBRG_N_FLUSH_ON_SHARED_OVERFLOWf};

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    SOC_REG_ABOVE_64_CLEAR(lbg_data);

    switch (type) {
    case socLbFlushTypeForceFlush:
    case socLbFlushTypeFlushOnGroupOverflow:
    case socLbFlushTypeFlushOnFifoOverflow:
    case socLbFlushTypeFlushOnSharedOverflow:
        rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
        SOCDNX_IF_ERR_EXIT(rv);
        soc_reg_above_64_field32_set(unit, lbg_reg, lbg_data, flush_flds[type], (enable?1:0));
        rv = soc_reg_above_64_set(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
        SOCDNX_IF_ERR_EXIT(rv);
        break;
    default:
        if (type < socLbFlushTypeCount) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL); 
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("%s: flush type not supported: %d\n"), FUNCTION_NAME(), type));
    }

    SOCDNX_IF_ERR_EXIT(rv);


exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_flush_config_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_flush_type_t          type,
    SOC_SAND_OUT int                         *enable
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    soc_reg_above_64_val_t
        lbg_data;
    soc_field_t
        flush_flds[] = {
            LBRG_N_FORCE_FLUSHf, 
            LBRG_N_FLUSH_ON_GROUP_OVERFLOWf,
            LBRG_N_FLUSH_ON_FIFO_OVERFLOWf,
            LBRG_N_FLUSH_ON_SHARED_OVERFLOWf};

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOCDNX_NULL_CHECK(enable);

    SOC_REG_ABOVE_64_CLEAR(lbg_data);

    switch (type) {
    case socLbFlushTypeFlushOnGroupOverflow:
    case socLbFlushTypeFlushOnFifoOverflow:
    case socLbFlushTypeFlushOnSharedOverflow:
        rv = soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data);
        SOCDNX_IF_ERR_EXIT(rv);

        *enable = soc_reg_above_64_field32_get(unit, lbg_reg, lbg_data, flush_flds[type]);
        break;
    default:
        if (type < socLbFlushTypeCount) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL); 
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("%s: flush type not supported: %d\n"), FUNCTION_NAME(), type));
    }

    SOCDNX_IF_ERR_EXIT(rv);
  
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  qax_lb_ing_status_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_status_type_t         status_type,
    SOC_SAND_OUT int                         *status
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        reg_abv64;
    soc_reg_t 
        reg = INVALIDr;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOCDNX_NULL_CHECK(status);

    SOC_REG_ABOVE_64_CLEAR(reg_abv64);

    switch (status_type) {
    case socLbStatusIsInSync:
        reg = ILB_LBRG_CONFIGURATIONr;
        rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), reg_abv64);
        SOCDNX_IF_ERR_EXIT(rv);

        *status = soc_reg_above_64_field32_get(unit, reg, reg_abv64, LBRG_N_IN_SYNCf);
        break;
    default:
        if (status_type < socLbStatusCount) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL); 
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("%s: Status type not supported: %d\n"), FUNCTION_NAME(), status_type));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_modem_status_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lb_modem_status_type_t   status_type,
    SOC_SAND_OUT int                         *status
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        reg = INVALIDr;
    uint32
        reg_val32 = 0, 
        fld_val32[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOCDNX_NULL_CHECK(status);

    switch (status_type) {
    case socLbModemStatusIsEmptyFifo:
        reg = ILB_FIFO_EMPTY_REGr;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, FIFO_EMPTYf);
        
        *status = (SHR_BITGET(fld_val32, modem_id) != 0);
        break;
    default:
        if (status_type < socLbModemStatusCount) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("%s: Status type not supported: %d\n"), FUNCTION_NAME(), status_type));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_glb_stat_get(
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_lb_stat_val_t            type, 
    SOC_SAND_OUT uint64                      *value
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        reg = ILB_FIFO_EMPTY_REGr;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0); 

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(value);

    switch (type) {
    case socLbStatsDiscardFragments:
        reg = ILB_FRAGMENT_DROP_CNT_REGr;
        rv = soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64);
        SOCDNX_IF_ERR_EXIT(rv);

        *value = soc_reg64_field_get(unit, reg, reg_val64, FRAGMENT_DROP_CNTf);            
        break;
    default:
        if (type < socLbStatsCount) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("%s: Statistic not supported: %d\n"), FUNCTION_NAME(), type));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_lbg_stat_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_stat_val_t            type, 
    SOC_SAND_OUT uint64                      *value
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        reg = INVALIDr;
    soc_reg_above_64_val_t
        reg_abv64;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOCDNX_NULL_CHECK(value);

    SOC_REG_ABOVE_64_CLEAR(reg_abv64);

    switch (type) {
    case socLbStatsDiscardFragments:
        reg = ILB_DEBUG_LBRG_STATUS_REGr;
        rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), reg_abv64);
        SOCDNX_IF_ERR_EXIT(rv);
        *value = soc_reg_above_64_field64_get(unit, reg, reg_abv64, DEBUG_27f);          
        break;
    default:
        if (type < socLbStatsCount) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL); 
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("%s: Statistic not supported: %d\n"), FUNCTION_NAME(), type));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ing_modem_stat_get(
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lb_modem_stat_val_t      type, 
    SOC_SAND_OUT uint64                      *value
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        reg = ILB_FIFO_EMPTY_REGr;
    soc_reg_above_64_val_t
        reg_abv64;
    uint64 
        value_hw, 
        value_sw;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(value);
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");

    SOC_REG_ABOVE_64_CLEAR(reg_abv64);

    switch (type) {
    case socLbModemStatsPkts:
        reg = ILB_FIFO_CNTr;

        rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, QAX_ILB_FIFO_CNT_INDEX(modem_id), reg_abv64);
        SOCDNX_IF_ERR_EXIT(rv);
        value_hw = soc_reg_above_64_field64_get(unit, reg, reg_abv64, FRAGMENT_CNT_Nf);
        
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_ing_modem_fragment_cnt.get(unit, modem_id, &value_sw));
        COMPILER_64_ADD_64(value_sw, value_hw);
        
        COMPILER_64_COPY(*value, value_sw);
        
        COMPILER_64_ZERO(value_sw);
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_ing_modem_fragment_cnt.set(unit, modem_id, value_sw));

        
        value_hw = soc_reg_above_64_field64_get(unit, reg, reg_abv64, BYTE_CNT_Nf);
        
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_ing_modem_byte_cnt.get(unit, modem_id, &value_sw));
        COMPILER_64_ADD_64(value_sw, value_hw);        
        
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_ing_modem_byte_cnt.set(unit, modem_id, value_sw));

        break;
    case socLbModemStatsOctets:
        reg = ILB_FIFO_CNTr;

        rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, QAX_ILB_FIFO_CNT_INDEX(modem_id), reg_abv64);
        SOCDNX_IF_ERR_EXIT(rv);
        value_hw = soc_reg_above_64_field64_get(unit, reg, reg_abv64, BYTE_CNT_Nf);
        
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_ing_modem_byte_cnt.get(unit, modem_id, &value_sw));
        COMPILER_64_ADD_64(value_sw, value_hw);
        
        COMPILER_64_COPY(*value, value_sw);
        
        COMPILER_64_ZERO(value_sw);
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_ing_modem_byte_cnt.set(unit, modem_id, value_sw));

        
        value_hw = soc_reg_above_64_field64_get(unit, reg, reg_abv64, FRAGMENT_CNT_Nf);
        
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_ing_modem_fragment_cnt.get(unit, modem_id, &value_sw));
        COMPILER_64_ADD_64(value_sw, value_hw);        
        
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_ing_modem_fragment_cnt.set(unit, modem_id, value_sw));
        break;
    default:
        if (type < socLbModemStatsCount) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL); 
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("%s: Statistic not supported: %d\n"), FUNCTION_NAME(), type));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_to_port_map_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_port_t                   port
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        egr_if;
    soc_mem_t 
	    modem_table = EPNI_LBG_MODEM_CONFIGm;
    soc_reg_above_64_val_t 
        modem_data;
    uint32
        is_valid = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,
            (_BSL_SOCDNX_MSG("%s: port(%d) is invalid\n"), FUNCTION_NAME(), port));
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(modem_data);

    rv = soc_mem_read(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_mem_field32_set(unit, modem_table, modem_data, EGQ_IFCf, egr_if);
    rv = soc_mem_write(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_to_port_unmap_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_port_t                   port
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        egr_if;
    soc_mem_t 
	    modem_table = EPNI_LBG_MODEM_CONFIGm;
    soc_reg_above_64_val_t 
        modem_data;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_REFERENCE(port);

    
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");

    egr_if = 0;

    SOC_REG_ABOVE_64_CLEAR(modem_data);

    rv = soc_mem_read(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_mem_field32_set(unit, modem_table, modem_data, EGQ_IFCf, egr_if);
    rv = soc_mem_write(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_to_port_map_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT  soc_port_t                 *port
  )
{
    soc_error_t
        rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    
    
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_lbg_enable_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  uint32                       is_enable
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        credit_size = 0,
        fifo_size = 0;
    soc_port_t
        port   = SOC_PORT_INVALID;
    int 
        lb_egr_credit_entry = LB_EGR_CREDIT_ENTRY_PER_LBG,
        speed = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    SOCDNX_IF_ERR_EXIT(_qax_lb_port_get_by_lbg_id(unit, lbg_id, &port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, &speed));

    
    SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_credit_and_fifo_optimial_value_get(unit, speed, &credit_size, &fifo_size));

    
    SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_enable_set(unit, lbg_id, is_enable));

    
    SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_credit_size_set(unit, lbg_id, lb_egr_credit_entry, credit_size));

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_lbg_enable_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT uint32                       *is_enable
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0, 
        fld_val32[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOCDNX_NULL_CHECK(is_enable);

    reg = EGQ_IFC_IS_LBGr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, IFC_IS_LBGf);
    *is_enable = (SHR_BITGET(fld_val32, lbg_id) != 0);

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_to_lbg_map_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    map_table = EPNI_LBG_MAPPINGm;
    uint32
        entry_data = 0, 
        fld_data[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    rv = soc_mem_read(unit, map_table, MEM_BLOCK_ANY, lbg_id, &entry_data);
    SOCDNX_IF_ERR_EXIT(rv);

    *fld_data = soc_mem_field32_get(unit, map_table, &entry_data, LBG_MAPPINGf);
    SHR_BITSET(fld_data, modem_id);
    soc_mem_field32_set(unit, map_table, &entry_data, LBG_MAPPINGf, *fld_data);

    rv = soc_mem_write(unit, map_table, MEM_BLOCK_ANY, lbg_id, &entry_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_to_lbg_unmap_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lbg_t                    lbg_id
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    map_table = EPNI_LBG_MAPPINGm;
    uint32
        entry_data = 0, 
        fld_data[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    rv = soc_mem_read(unit, map_table, MEM_BLOCK_ANY, lbg_id, &entry_data);
    SOCDNX_IF_ERR_EXIT(rv);

    *fld_data = soc_mem_field32_get(unit, map_table, &entry_data, LBG_MAPPINGf);
    SHR_BITCLR(fld_data, modem_id);
    soc_mem_field32_set(unit, map_table, &entry_data, LBG_MAPPINGf, *fld_data);

    rv = soc_mem_write(unit, map_table, MEM_BLOCK_ANY, lbg_id, &entry_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_to_lbg_map_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT soc_lbg_t                   *lbg_id
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    map_table = EPNI_LBG_MAPPINGm;
    uint32
        entry_data = 0, 
        fld_data[1] = {0};
    soc_lbg_t
        lbg_id_tmp = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOCDNX_NULL_CHECK(lbg_id);

    *lbg_id = SOC_TMC_LB_LBG_INVALID;

    for (lbg_id_tmp = 0; lbg_id_tmp < (SOC_TMC_LB_LBG_MAX+1); lbg_id_tmp++) {
        rv = soc_mem_read(unit, map_table, MEM_BLOCK_ANY, lbg_id_tmp, &entry_data);
        SOCDNX_IF_ERR_EXIT(rv);

        *fld_data = soc_mem_field32_get(unit, map_table, &entry_data, LBG_MAPPINGf);
        if (SHR_BITGET(fld_data, modem_id)) {
            *lbg_id = lbg_id_tmp;
            break;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_expected_seqeunce_num_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          expected_seq_num
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0,
        fld_val32 = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOC_TMC_LB_VALUE_CHECK(expected_seq_num, 0, QAX_LB_TX_LBG_SEQUENCE_MAX, "expected_sequence_number");

    reg = EPNI_SET_SEQUENCE_NUMBERr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    soc_reg_field_set(unit, reg, &reg_val32, SET_SEQ_NUM_LBGf, QAX_EPNI_LBG_CONFIGURATION_INDEX(lbg_id));
    soc_reg_field_set(unit, reg, &reg_val32, SET_SEQ_NUM_VALUEf, expected_seq_num);
    fld_val32 = 1;
    soc_reg_field_set(unit, reg, &reg_val32, SET_SEQ_NUM_TRIGGERf, fld_val32);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    fld_val32 = 0;
    soc_reg_field_set(unit, reg, &reg_val32, SET_SEQ_NUM_TRIGGERf, fld_val32);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_expected_seqeunce_num_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT int                         *expected_seq_num
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOCDNX_NULL_CHECK(expected_seq_num);

    reg = EPNI_SET_SEQUENCE_NUMBERr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    *expected_seq_num = soc_reg_field_get(unit, reg, reg_val32, SET_SEQ_NUM_VALUEf);

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_seqeunce_num_width_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  int                          num_width
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0, 
        fld_val32[1] = {0}, 
        fld_val32_tmp[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOC_TMC_LB_VALUE_CHECK(num_width, SOC_TMC_LB_EGR_SEQUENCE_NUM_WIDTH_MIN, SOC_TMC_LB_EGR_SEQUENCE_NUM_WIDTH_MAX, "num_width");

    *fld_val32_tmp = num_width;

    reg = EPNI_PER_LBG_SEQ_MAX_VALUEr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, LBG_SEQ_MAX_VALUEf);
    SHR_BITCOPY_RANGE(fld_val32, lbg_id*5, fld_val32_tmp, 0, 5);
    soc_reg_field_set(unit, reg, &reg_val32, LBG_SEQ_MAX_VALUEf, *fld_val32);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_seqeunce_num_width_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_OUT int                         *num_width
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0,
        fld_val32[1] = {0}, 
        fld_val32_tmp[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOCDNX_NULL_CHECK(num_width);
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    reg = EPNI_PER_LBG_SEQ_MAX_VALUEr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, LBG_SEQ_MAX_VALUEf);
    SHR_BITCOPY_RANGE(fld_val32_tmp, 0, fld_val32, lbg_id*5, 5);

    *num_width = *fld_val32_tmp;

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_seqeunce_num_width_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  int                          num_width
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    soc_reg_above_64_val_t
        reg_abv64, 
        fld_abv64;
    uint32
        fld_val32_tmp[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOC_TMC_LB_VALUE_CHECK(num_width, SOC_TMC_LB_EGR_SEQUENCE_NUM_WIDTH_MIN, SOC_TMC_LB_EGR_SEQUENCE_NUM_WIDTH_MAX, "num_width");

    *fld_val32_tmp = num_width;
    SOC_REG_ABOVE_64_CLEAR(reg_abv64);

    reg = EPNI_PER_LBG_MODEM_SEQ_MAX_VALUEr;
    rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_abv64);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_above_64_field_get(unit, reg, reg_abv64, LBG_SEQ_MAX_VALUEf, fld_abv64);
    SHR_BITCOPY_RANGE(fld_abv64, modem_id*5, fld_val32_tmp, 0, 5);
    soc_reg_above_64_field_set(unit, reg, reg_abv64, LBG_SEQ_MAX_VALUEf, fld_abv64);
    rv = soc_reg_above_64_set(unit, reg, REG_PORT_ANY, 0, reg_abv64);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_seqeunce_num_width_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT int                         *num_width
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    soc_reg_above_64_val_t
        reg_abv64, 
        fld_abv64;
    uint32
        fld_val32_tmp[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOCDNX_NULL_CHECK(num_width);

    SOC_REG_ABOVE_64_CLEAR(reg_abv64);

    reg = EPNI_PER_LBG_MODEM_SEQ_MAX_VALUEr;
    rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, 0, reg_abv64);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_above_64_field_get(unit, reg, reg_abv64, LBG_SEQ_MAX_VALUEf, fld_abv64);
    SHR_BITCOPY_RANGE(fld_val32_tmp, 0, fld_abv64, modem_id*5, 5);

    *num_width = *fld_val32_tmp;

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
  qax_lb_global_packet_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lb_global_packet_config_t      *config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0;
    soc_lb_segment_mode_t
        segment_mode;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(config);

    if ((!SOC_IS_QUX(unit)) && config->packet_crc_enable) {
        rv = qax_lb_egr_segment_config_get(unit, SOC_TMC_LB_GLOBAL, &segment_mode);
        SOCDNX_IF_ERR_EXIT(rv);

        if ((segment_mode == socLbSegmentMode192) || (segment_mode == socLbSegmentMode192Enhanced)) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG,
                (_BSL_SOCDNX_MSG("Fail(%s) 192B segmentations can not support packet CRC addition\n"),
                                 soc_errmsg(SOC_E_CONFIG)));
        }
    }

    reg = EPNI_LBG_HEADER_CONFIGURATIONr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    soc_reg_field_set(unit, reg, &reg_val32, OUTER_VLAN_TPIDf, config->outer_vlan_tpid);
    soc_reg_field_set(unit, reg, &reg_val32, LBG_TPIDf, config->lb_tpid);
    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));

    reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        soc_reg_field_set(unit, reg, &reg_val32,  PACKET_CRC_ENABLEf, ((config->packet_crc_enable)?1:0));  
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_global_packet_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT soc_lb_global_packet_config_t      *config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(config);

    reg = EPNI_LBG_HEADER_CONFIGURATIONr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    config->outer_vlan_tpid = soc_reg_field_get(unit, reg, reg_val32, OUTER_VLAN_TPIDf);
    config->lb_tpid = soc_reg_field_get(unit, reg, reg_val32, LBG_TPIDf);

    reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        config->packet_crc_enable = soc_reg_field_get(unit, reg, reg_val32,  PACKET_CRC_ENABLEf);
    }

    SOCDNX_IF_ERR_EXIT(rv);
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_lbg_packet_config_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_IN  soc_lb_lbg_packet_config_t         *config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        fld_val32[1] = {0};
    soc_lb_segment_mode_t
        segment_mode;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(config);

    reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    if (SOC_IS_QUX(unit)) {
        if (config->packet_crc_enable) {
            rv = qax_lb_egr_segment_config_get(unit, lbg_id, &segment_mode);
            SOCDNX_IF_ERR_EXIT(rv);

            if ((segment_mode == socLbSegmentMode192) || (segment_mode == socLbSegmentMode192Enhanced)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG,
                    (_BSL_SOCDNX_MSG("Fail(%s) 192B segmentations can not support packet CRC addition\n"),
                                    soc_errmsg(SOC_E_CONFIG)));
            }
        }

        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *fld_val32 = soc_reg64_field32_get(unit, reg, reg_val64, PACKET_CRC_ENABLEf);
        if (config->packet_crc_enable) {
            SHR_BITSET(fld_val32, lbg_id);
        }
        else {
            SHR_BITCLR(fld_val32, lbg_id);
        }
        soc_reg64_field32_set(unit, reg, &reg_val64, PACKET_CRC_ENABLEf, *fld_val32);
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val64));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_lbg_packet_config_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  soc_lbg_t                           lbg_id,
    SOC_SAND_OUT soc_lb_lbg_packet_config_t         *config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        fld_val32[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(config);

    reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *fld_val32 = soc_reg64_field32_get(unit, reg, reg_val64, PACKET_CRC_ENABLEf);
        config->packet_crc_enable = SHR_BITGET(fld_val32, lbg_id) ? 1 : 0;
    }

    SOCDNX_IF_ERR_EXIT(rv);
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_modem_packet_config_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  soc_modem_t                        modem_id,
    SOC_SAND_IN  soc_lb_modem_packet_config_t      *config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    modem_table = EPNI_LBG_MODEM_CONFIGm;
    soc_reg_above_64_val_t 
        modem_data;
    int
        hw_hdr_type = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(config);
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOC_TMC_LB_VALUE_CHECK(config->pkt_format, socLbgFormatTypeNonChannelize, socLbgFormatTypeChannelize, "pkt_format");
    if (config->pkt_format == socLbgFormatTypeChannelize && config->vlan == BCM_VLAN_NONE) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("Fail(%s) parameter(%s=%d) can't be equal to %d\n"),
                             soc_errmsg(BCM_E_PARAM), "vlan", config->vlan, BCM_VLAN_NONE));
    }

    if (config->pkt_format == socLbgFormatTypeNonChannelize) {
        hw_hdr_type = 0;
    }
    else if (config->pkt_format == socLbgFormatTypeChannelize) {
        hw_hdr_type = 1;
    }

    SOC_REG_ABOVE_64_CLEAR(modem_data);

    rv = soc_mem_read(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_mem_field32_set(unit, modem_table, modem_data, HDR_TYPEf, hw_hdr_type);
    soc_mem_field32_set(unit, modem_table, modem_data, VLAN_IDf, config->vlan);
    soc_mem_field32_set(unit, modem_table, modem_data, VLAN_PCP_STAMPINGf, config->use_global_priority_map);
    soc_mem_mac_addr_set(unit, modem_table, modem_data, MAC_SAf, config->src_mac);
    soc_mem_mac_addr_set(unit, modem_table, modem_data, MAC_DAf, config->dst_mac);

    rv = soc_mem_write(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_modem_packet_config_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  soc_modem_t                        modem_id,
    SOC_SAND_OUT soc_lb_modem_packet_config_t      *config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    modem_table = EPNI_LBG_MODEM_CONFIGm;
    soc_reg_above_64_val_t 
        modem_data;
    int
        hw_hdr_type = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(config);
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOC_REG_ABOVE_64_CLEAR(modem_data);

    rv = soc_mem_read(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);

    hw_hdr_type = soc_mem_field32_get(unit, modem_table, modem_data, HDR_TYPEf);
    if (hw_hdr_type == 0 || hw_hdr_type == 2) {
        config->pkt_format = socLbgFormatTypeNonChannelize;
    }
    else if (hw_hdr_type == 1 || hw_hdr_type == 3) {
        config->pkt_format = socLbgFormatTypeChannelize;
    }
    else {
        SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
    }
    config->vlan = soc_mem_field32_get(unit, modem_table, modem_data, VLAN_IDf);
    config->use_global_priority_map = soc_mem_field32_get(unit, modem_table, modem_data, VLAN_PCP_STAMPINGf);
    soc_mem_mac_addr_get(unit, modem_table, modem_data, MAC_SAf, config->src_mac);
    soc_mem_mac_addr_get(unit, modem_table, modem_data, MAC_DAf, config->dst_mac);

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_tc_dp_to_packet_priority_config_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lb_tc_dp_t              *tc_dp,
    SOC_SAND_IN  soc_lb_pkt_pri_t            *pkt_pri
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        map_data;
    soc_reg_t 
        map_reg = EPNI_LBG_TC_DP_MAPPINGr;
    int
        dst_offset = 0;
    uint32
        entry_data = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(tc_dp);
    SOCDNX_NULL_CHECK(pkt_pri);    
    SOC_TMC_LB_VALUE_MAX_CHECK(tc_dp->tc, SOC_SAND_PP_TC_MAX, "tc_dp->tc");
    SOC_TMC_LB_VALUE_MAX_CHECK(tc_dp->dp, SOC_SAND_PP_DP_MAX, "tc_dp->dp");
    SOC_TMC_LB_VALUE_MAX_CHECK(pkt_pri->pkt_pri, SOC_SAND_PP_PCP_UP_MAX, "pkt_pri->pkt_pri");
    SOC_TMC_LB_VALUE_MAX_CHECK(pkt_pri->pkt_cfi, SOC_SAND_PP_DEI_CFI_MAX, "pkt_pri->pkt_cfi");
    SOC_TMC_LB_VALUE_MAX_CHECK(pkt_pri->pkt_dp, SOC_TMC_LB_EGR_DP_MAX, "pkt_pri->pkt_dp");

    SOC_REG_ABOVE_64_CLEAR(map_data);
    
    dst_offset = QAX_LB_TC_DP_MAP_ENTRY_INDX(tc_dp->tc, tc_dp->dp)*QAX_LB_TC_DP_MAP_ENTRY_SIZE;
    QAX_LB_TC_DP_MAP_ENTRY_DATA_SET(entry_data, pkt_pri->pkt_pri, pkt_pri->pkt_cfi, pkt_pri->pkt_dp); 

    rv = soc_reg_above_64_get(unit, map_reg, REG_PORT_ANY, 0, map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    SHR_BITCOPY_RANGE(map_data, dst_offset, &entry_data, 0, QAX_LB_TC_DP_MAP_ENTRY_SIZE);

    rv = soc_reg_above_64_set(unit, map_reg, REG_PORT_ANY, 0, map_data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_tc_dp_to_packet_priority_config_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lb_tc_dp_t              *tc_dp,
    SOC_SAND_OUT soc_lb_pkt_pri_t            *pkt_pri
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t
        map_data;
    soc_reg_t 
        map_reg = EPNI_LBG_TC_DP_MAPPINGr;
    int
        dst_offset = 0;
    uint32
        entry_data = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(tc_dp);
    SOCDNX_NULL_CHECK(pkt_pri);
    SOC_TMC_LB_VALUE_MAX_CHECK(tc_dp->tc, SOC_SAND_PP_TC_MAX, "tc_dp->tc");
    SOC_TMC_LB_VALUE_MAX_CHECK(tc_dp->dp, SOC_SAND_PP_DP_MAX, "tc_dp->dp");

    SOC_REG_ABOVE_64_CLEAR(map_data);
    
    dst_offset = QAX_LB_TC_DP_MAP_ENTRY_INDX(tc_dp->tc, tc_dp->dp)*QAX_LB_TC_DP_MAP_ENTRY_SIZE;

    rv = soc_reg_above_64_get(unit, map_reg, REG_PORT_ANY, 0, map_data);
    SOCDNX_IF_ERR_EXIT(rv);

    SHR_BITCOPY_RANGE(&entry_data, 0, map_data, dst_offset, QAX_LB_TC_DP_MAP_ENTRY_SIZE);

    QAX_LB_TC_DP_MAP_ENTRY_DATA_GET(entry_data, pkt_pri->pkt_pri, pkt_pri->pkt_cfi, pkt_pri->pkt_dp); 

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_sched_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       lbg_count,
    SOC_SAND_IN  soc_lb_lbg_weight_t         *lbg_weights
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    mem = INVALIDm;
    uint32
        entry_data = 0;
    int
        sum_of_lbg_weight = 0,
        lbg_temp = 0, 
        entry_offset = 0,
        nof_entries = 0,
        entry_start = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_count, (SOC_TMC_LB_LBG_MAX+1), "lbg_count");
    SOCDNX_NULL_CHECK(lbg_weights);

    for (lbg_temp = 0; lbg_temp < lbg_count; lbg_temp++) {
        SOC_TMC_LB_VALUE_MAX_CHECK(lbg_weights[lbg_temp].lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
        sum_of_lbg_weight += lbg_weights[lbg_temp].lbg_weight;
    }
    SOC_TMC_LB_VALUE_MAX_CHECK(sum_of_lbg_weight, SOC_TMC_LB_EGR_NOF_SCHEDULER_ENTRY, "lbg_weight");

    mem = EPNI_LBG_CALENDARm;
    entry_start = 0;
    for (lbg_temp = 0; lbg_temp < lbg_count; lbg_temp++) {
        nof_entries = lbg_weights[lbg_temp].lbg_weight;
        for (entry_offset = 0; entry_offset < nof_entries; entry_offset++) {
            soc_mem_field32_set(unit, mem, &entry_data, LBG_CALENDARf, (lbg_weights[lbg_temp].lbg_id));   
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, (entry_start + entry_offset), &entry_data);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        entry_start += nof_entries;
    }

    for (; entry_start < SOC_TMC_LB_EGR_NOF_SCHEDULER_ENTRY; entry_start++) {
        soc_mem_field32_set(unit, mem, &entry_data, LBG_CALENDARf, 7);   
        rv = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_start, &entry_data);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    SOCDNX_IF_ERR_EXIT(rv);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_sched_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                       max_lbg_count,
    SOC_SAND_OUT soc_lb_lbg_weight_t         *lbg_weights,
    SOC_SAND_OUT uint32                      *lbg_count    
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    mem = INVALIDm;
    uint32
        entry_data = 0;
    int
        lbg_id = 0, 
        lbg_count_tmp = 0,
        weights[SOC_TMC_LB_NOF_LBG+1] = {0},
        entry_index = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(lbg_weights);

    mem = EPNI_LBG_CALENDARm;
    for (entry_index = 0; entry_index < SOC_TMC_LB_EGR_NOF_SCHEDULER_ENTRY; entry_index++) {
        rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_index, &entry_data);
        SOCDNX_IF_ERR_EXIT(rv);

        lbg_id = soc_mem_field32_get(unit, mem, &entry_data, LBG_CALENDARf);   
        if (lbg_id <= SOC_TMC_LB_LBG_MAX) {
            weights[lbg_id] += 1;
        }
    }

    lbg_count_tmp = 0;
    for (lbg_id = 0; lbg_id < SOC_TMC_LB_NOF_LBG; lbg_id++) {
        if ((lbg_count_tmp < max_lbg_count) && (weights[lbg_id] != 0)) {
            lbg_weights[lbg_count_tmp].lbg_id = lbg_id;
            lbg_weights[lbg_count_tmp].lbg_weight = weights[lbg_id];
            lbg_count_tmp++;
        }
    }
    *lbg_count = lbg_count_tmp;

    SOCDNX_IF_ERR_EXIT(rv);
  
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_segment_config_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_IN  soc_lb_segment_mode_t            segment_mode
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    uint32
        reg_val32 = 0;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        segment_mode_set = 0,
        segment_mode_val[1] = {0},
        is_enhanced_set = 0, 
        is_enhanced_val[1] = {0};
    soc_lb_lbg_packet_config_t
        lbg_pkt;
    soc_lb_global_packet_config_t
        glb_pkt;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(segment_mode, (socLbSegmentModeCount-1), "segment_mode");
    
    if (SOC_IS_QUX(unit)) {
        SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    }
    else {
        if (SOC_TMC_LB_GLOBAL != lbg_id) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                (_BSL_SOCDNX_MSG("Fail(%s) parameter(%s=%d) must be equal to %d\n"),
                                 soc_errmsg(BCM_E_PARAM), "lbg_id", SOC_TMC_LB_GLOBAL ));
        }
    }

    segment_mode_set = ((segment_mode == socLbSegmentMode128) || (segment_mode == socLbSegmentMode128Enhanced)) ? 1 : 0;
    is_enhanced_set = ((segment_mode == socLbSegmentMode128Enhanced) || (segment_mode == socLbSegmentMode192Enhanced)) ? 1 : 0;

    if (SOC_IS_QUX(unit)) {
        if ((segment_mode == socLbSegmentMode192) || (segment_mode == socLbSegmentMode192Enhanced)) {

            rv = qax_lb_lbg_packet_config_get(unit, lbg_id, &lbg_pkt);
            SOCDNX_IF_ERR_EXIT(rv);

            if (lbg_pkt.packet_crc_enable) {
                lbg_pkt.packet_crc_enable = 0;
                rv = qax_lb_lbg_packet_config_set(unit, lbg_id, &lbg_pkt);
                SOCDNX_IF_ERR_EXIT(rv);

                LOG_INFO(BSL_LS_SOC_LB,
                         (BSL_META_U(unit,
                                     "unit %d, disable packet CRC addition for lbg(%d), since 192B segmentations can not support \\n"), 
                                     unit, lbg_id));  
            }
        }

        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *segment_mode_val = soc_reg64_field32_get(unit, reg, reg_val64, SEGMENTATION_MODEf);
        if (segment_mode_set) {
            SHR_BITSET(segment_mode_val, lbg_id);
        }
        else {
            SHR_BITCLR(segment_mode_val, lbg_id);
        }
        soc_reg64_field32_set(unit, reg, &reg_val64, SEGMENTATION_MODEf, *segment_mode_val);

        *is_enhanced_val = soc_reg64_field32_get(unit, reg, reg_val64, ENHANCED_SEG_ENABLEf);
        if (is_enhanced_set) {
            SHR_BITSET(is_enhanced_val, lbg_id);
        }
        else {
            SHR_BITCLR(is_enhanced_val, lbg_id);
        }
        soc_reg64_field32_set(unit, reg, &reg_val64, ENHANCED_SEG_ENABLEf, *is_enhanced_val); 
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val64));
    }
    else {
        if ((segment_mode == socLbSegmentMode192) || (segment_mode == socLbSegmentMode192Enhanced)) {
            
            rv = qax_lb_global_packet_config_get(unit, &glb_pkt);
            SOCDNX_IF_ERR_EXIT(rv);

            if (glb_pkt.packet_crc_enable) {
                glb_pkt.packet_crc_enable = 0;
                rv = qax_lb_global_packet_config_set(unit, &glb_pkt);
                SOCDNX_IF_ERR_EXIT(rv);

                LOG_INFO(BSL_LS_SOC_LB,
                         (BSL_META_U(unit,
                                     "unit %d, disable packet CRC addition, since 192B segmentations can not support\\n"), 
                                     unit));
            }
        }

        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        soc_reg_field_set(unit, reg, &reg_val32, SEGMENTATION_MODEf, segment_mode_set);
        soc_reg_field_set(unit, reg, &reg_val32, ENHANCED_SEG_ENABLEf, is_enhanced_set);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_segment_config_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_lbg_t                        lbg_id,
    SOC_SAND_OUT soc_lb_segment_mode_t           *segment_mode
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t
        reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    uint32
        reg_val32 = 0;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        segment_mode_get = 0,
        segment_mode_val[1] = {0},
        is_enhanced_get = 0, 
        is_enhanced_val[1] = {0};

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(segment_mode);
    
    if (SOC_IS_QUX(unit)) {
        SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    }
    else {
        if (SOC_TMC_LB_GLOBAL != lbg_id) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                (_BSL_SOCDNX_MSG("Fail(%s) parameter(%s=%d) must be equal to %d\n"),
                                 soc_errmsg(BCM_E_PARAM), "lbg_id", SOC_TMC_LB_GLOBAL ));
        }
    }

    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *segment_mode_val = soc_reg64_field32_get(unit, reg, reg_val64, SEGMENTATION_MODEf); 
        segment_mode_get = SHR_BITGET(segment_mode_val, lbg_id) ? 1 : 0;
        *is_enhanced_val = soc_reg64_field32_get(unit, reg, reg_val64, ENHANCED_SEG_ENABLEf); 
        is_enhanced_get = SHR_BITGET(is_enhanced_val, lbg_id) ? 1 : 0;
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        segment_mode_get = soc_reg_field_get(unit, reg, reg_val32, SEGMENTATION_MODEf);
        is_enhanced_get = soc_reg_field_get(unit, reg, reg_val32, ENHANCED_SEG_ENABLEf);
    }

    if (segment_mode_get == 1) {
        if (is_enhanced_get == 0) {
            *segment_mode = socLbSegmentMode128;
        }
        else {
            *segment_mode = socLbSegmentMode128Enhanced;
        }
    }
    else {
        if (is_enhanced_get == 0) {
            *segment_mode = socLbSegmentMode192;
        }
        else {
            *segment_mode = socLbSegmentMode192Enhanced;
        }
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_shaper_set(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_modem_t                      modem_id,
    SOC_SAND_IN  soc_lb_modem_shaper_config_t     *shaper_config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
	    modem_table = EPNI_LBG_MODEM_CONFIGm;
    soc_reg_above_64_val_t 
        modem_data;
    soc_reg_t
        reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    uint32
        reg_val32 = 0, 
        fld_val32[1] = {0};
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        hw_hdr_compensation[1] = {0};
    uint32
        hw_rate = 0;
    SOC_SAND_U64 
        u64_1, 
        u64_2;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(shaper_config);
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOC_TMC_LB_VALUE_CHECK(shaper_config->hdr_compensation, SOC_TMC_LB_EGR_MODEM_SHAPER_HDR_COMPENSATION_MIN, SOC_TMC_LB_EGR_MODEM_SHAPER_HDR_COMPENSATION_MAX, "hdr_compensation");
    SOC_TMC_LB_VALUE_MAX_CHECK(shaper_config->rate, SOC_TMC_LB_EGR_MODEM_SHAPER_RATE_MAX, "rate");
    SOC_TMC_LB_VALUE_MAX_CHECK(shaper_config->max_burst, SOC_TMC_LB_EGR_MODEM_SHAPER_BURST_MAX, "max_burst");

    if (shaper_config->hdr_compensation < 0) {
        *hw_hdr_compensation = -shaper_config->hdr_compensation;
        SHR_BITSET(hw_hdr_compensation, 6);
    }
    else {
        *hw_hdr_compensation = shaper_config->hdr_compensation;
    }

    
    soc_sand_u64_multiply_longs(shaper_config->rate, 1000, &u64_1);
    soc_sand_u64_devide_u64_long(&u64_1, SOC_TMC_LB_EGR_MODEM_SHAPER_RATE_GRANULARITY(unit), &u64_2);
    hw_rate = u64_2.arr[0];

    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *fld_val32 = soc_reg64_field32_get(unit, reg, reg_val64, MODEM_SHAPER_DISf); 
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, MODEM_SHAPER_DISf);
    }
    if (shaper_config->enable) {
        SHR_BITCLR(fld_val32, modem_id);
    }
    else {
        SHR_BITSET(fld_val32, modem_id);
    }
    if (SOC_IS_QUX(unit)) {
        soc_reg64_field32_set(unit, reg, &reg_val64, MODEM_SHAPER_DISf, *fld_val32); 
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val64));
    }
    else {
        soc_reg_field_set(unit, reg, &reg_val32, MODEM_SHAPER_DISf, *fld_val32);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));
    }

    SOC_REG_ABOVE_64_CLEAR(modem_data);

    rv = soc_mem_read(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_mem_field32_set(unit, modem_table, modem_data, HDR_COMPENSATIONf, *hw_hdr_compensation);
    soc_mem_field32_set(unit, modem_table, modem_data, MODEM_SHAPER_RATEf, hw_rate);
    soc_mem_field32_set(unit, modem_table, modem_data, MODEM_SHAPER_MAX_BURSTf, shaper_config->max_burst);

    rv = soc_mem_write(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_shaper_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  soc_modem_t                      modem_id,
    SOC_SAND_OUT soc_lb_modem_shaper_config_t     *shaper_config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
        modem_table = EPNI_LBG_MODEM_CONFIGm;
    soc_reg_above_64_val_t 
        modem_data;
    soc_reg_t
        reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    uint32
        reg_val32 = 0, 
        fld_val32[1] = {0};
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        hw_hdr_compensation[1] = {0}, 
        hdr_compensation_abs[1] = {0};
    uint32
        hw_rate = 0;
    SOC_SAND_U64 
        u64_1, 
        u64_2;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(shaper_config);
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");

    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *fld_val32 = soc_reg64_field32_get(unit, reg, reg_val64, MODEM_SHAPER_DISf); 
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, MODEM_SHAPER_DISf);
    }
    shaper_config->enable = (SHR_BITGET(fld_val32, modem_id) == 0);

    SOC_REG_ABOVE_64_CLEAR(modem_data);
    rv = soc_mem_read(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);
    *hw_hdr_compensation = soc_mem_field32_get(unit, modem_table, modem_data, HDR_COMPENSATIONf);
    hw_rate = soc_mem_field32_get(unit, modem_table, modem_data, MODEM_SHAPER_RATEf);
    shaper_config->max_burst = soc_mem_field32_get(unit, modem_table, modem_data, MODEM_SHAPER_MAX_BURSTf);

    
    soc_sand_u64_multiply_longs(hw_rate, SOC_TMC_LB_EGR_MODEM_SHAPER_RATE_GRANULARITY(unit), &u64_1);
    soc_sand_u64_devide_u64_long(&u64_1, 1000, &u64_2);
    shaper_config->rate = u64_2.arr[0];  

    SHR_BITCOPY_RANGE(hdr_compensation_abs, 0, hw_hdr_compensation, 0, 6);
    shaper_config->hdr_compensation = *hdr_compensation_abs;
    if (SHR_BITGET(hw_hdr_compensation, 6)) {
        shaper_config->hdr_compensation = -shaper_config->hdr_compensation;
    }

    SOCDNX_IF_ERR_EXIT(rv);
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_segment_shaper_set(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  soc_modem_t                               modem_id,
    SOC_SAND_IN  soc_lb_modem_segment_shaper_config_t     *shaper_config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
        modem_table = EPNI_LBG_MODEM_CONFIGm;
    soc_reg_above_64_val_t 
        modem_data;
    soc_reg_t
        reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    uint32
        reg_val32 = 0, 
        fld_val32[1] = {0};
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        hw_rate = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOCDNX_NULL_CHECK(shaper_config);
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");
    SOC_TMC_LB_VALUE_MAX_CHECK(shaper_config->rate, SOC_TMC_LB_EGR_MODEM_SEGMENT_SHAPER_RATE_MAX, "rate");
    SOC_TMC_LB_VALUE_MAX_CHECK(shaper_config->max_burst, SOC_TMC_LB_EGR_MODEM_SEGMENT_SHAPER_BURST_MAX, "max_burst");

    
    hw_rate = shaper_config->rate/SOC_TMC_LB_EGR_MODEM_SEGMENT_SHAPER_RATE_GRANULARITY;

    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *fld_val32 = soc_reg64_field32_get(unit, reg, reg_val64, MODEM_SEGMENT_SHAPER_DISf); 
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, MODEM_SEGMENT_SHAPER_DISf);
    }
    if (shaper_config->enable) {
        SHR_BITCLR(fld_val32, modem_id);
    }
    else {
        SHR_BITSET(fld_val32, modem_id);
    }
    if (SOC_IS_QUX(unit)) {
        soc_reg64_field32_set(unit, reg, &reg_val64, MODEM_SEGMENT_SHAPER_DISf, *fld_val32); 
        SOCDNX_IF_ERR_EXIT(soc_reg64_set(unit, reg, REG_PORT_ANY, 0, reg_val64));
    }
    else {
        soc_reg_field_set(unit, reg, &reg_val32, MODEM_SEGMENT_SHAPER_DISf, *fld_val32);
        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val32));
    }

    SOC_REG_ABOVE_64_CLEAR(modem_data);

    rv = soc_mem_read(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);

    soc_mem_field32_set(unit, modem_table, modem_data, SHAPER_SEGMENT_RATEf, hw_rate);
    soc_mem_field32_set(unit, modem_table, modem_data, SHAPER_SEGMENT_MAX_BURSTf, shaper_config->max_burst);

    rv = soc_mem_write(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_segment_shaper_get(
    SOC_SAND_IN  int                                       unit,
    SOC_SAND_IN  soc_modem_t                               modem_id,
    SOC_SAND_OUT soc_lb_modem_segment_shaper_config_t     *shaper_config
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
        modem_table = EPNI_LBG_MODEM_CONFIGm;
    soc_reg_above_64_val_t 
        modem_data;
    soc_reg_t
        reg = EPNI_LBG_GENERAL_CONFIGURATIONr;
    uint32
        reg_val32 = 0, 
        fld_val32[1] = {0};
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);
    uint32
        hw_rate = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    if (!SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL);
    }
    SOCDNX_NULL_CHECK(shaper_config);
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");

    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64));
        *fld_val32 = soc_reg64_field32_get(unit, reg, reg_val64, MODEM_SEGMENT_SHAPER_DISf); 
    }
    else {
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        *fld_val32 = soc_reg_field_get(unit, reg, reg_val32, MODEM_SEGMENT_SHAPER_DISf);
    }
    shaper_config->enable = (SHR_BITGET(fld_val32, modem_id) == 0);

    SOC_REG_ABOVE_64_CLEAR(modem_data);
    rv = soc_mem_read(unit, modem_table, MEM_BLOCK_ANY, modem_id, modem_data);
    SOCDNX_IF_ERR_EXIT(rv);
    hw_rate = soc_mem_field32_get(unit, modem_table, modem_data, SHAPER_SEGMENT_RATEf);
    shaper_config->max_burst = soc_mem_field32_get(unit, modem_table, modem_data, SHAPER_SEGMENT_MAX_BURSTf);

    
    shaper_config->rate = hw_rate*SOC_TMC_LB_EGR_MODEM_SEGMENT_SHAPER_RATE_GRANULARITY;

    SOCDNX_IF_ERR_EXIT(rv);
 
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_status_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_status_type_t         status_type,
    SOC_SAND_OUT int                         *status
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        reg = EPNI_LBG_FIFOS_EMPTYr;
    soc_field_t
        fld = INVALIDf,
        lbg_fifo_empty_flds[] = {
            LBG_FIFO_EMPTY_INDICATION_0f, LBG_FIFO_EMPTY_INDICATION_1f, LBG_FIFO_EMPTY_INDICATION_2f, LBG_FIFO_EMPTY_INDICATION_3f,
            LBG_FIFO_EMPTY_INDICATION_4f, LBG_FIFO_EMPTY_INDICATION_5f, LBG_FIFO_EMPTY_INDICATION_6f, LBG_FIFO_EMPTY_INDICATION_7f,
            LBG_FIFO_EMPTY_INDICATION_8f, LBG_FIFO_EMPTY_INDICATION_9f, LBG_FIFO_EMPTY_INDICATION_10f, LBG_FIFO_EMPTY_INDICATION_11f, 
            LBG_FIFO_EMPTY_INDICATION_12f};
    uint32
        reg_val32 = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    SOCDNX_NULL_CHECK(status);

    switch (status_type) {
    case socLbStatusIsEmptyFifo:
        reg = EPNI_LBG_FIFOS_EMPTYr;
        fld = lbg_fifo_empty_flds[QAX_EPNI_LBG_FIFOS_EMPTY_INDEX(lbg_id)];
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        
        *status = soc_reg_field_get(unit, reg, reg_val32, fld);
        break;
    default:
        if (status_type < socLbStatusCount) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL); 
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("%s: Status type not supported: %d\n"), FUNCTION_NAME(), status_type));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_stat_get(
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_lbg_t                    lbg_id,
    SOC_SAND_IN  soc_lb_stat_val_t            type, 
    SOC_SAND_OUT uint64                      *value
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        reg = ILB_FIFO_EMPTY_REGr;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0); 
    int 
        is_global = 0;
    soc_qax_lb_stats_info_t lbg_egr_stats_reg_info[] = {
        {EPNI_LBG_TX_UC_PKT_COUNTER_0r,         LBG_TX_UC_PKT_COUNTER_0f},
        {EPNI_LBG_TX_UC_PKT_COUNTER_2r,         LBG_TX_UC_PKT_COUNTER_2f},
        {EPNI_LBG_TX_UC_PKT_COUNTER_4r,         LBG_TX_UC_PKT_COUNTER_4f},
        {EPNI_LBG_TX_UC_PKT_COUNTER_6r,         LBG_TX_UC_PKT_COUNTER_6f},
        {EPNI_LBG_TX_UC_PKT_COUNTER_8r,         LBG_TX_UC_PKT_COUNTER_8f},
        {EPNI_LBG_TX_UC_PKT_COUNTER_10r,        LBG_TX_UC_PKT_COUNTER_10f},
        {EPNI_LBG_TX_MC_PKT_COUNTER_0r,         LBG_TX_MC_PKT_COUNTER_0f},
        {EPNI_LBG_TX_MC_PKT_COUNTER_2r,         LBG_TX_MC_PKT_COUNTER_2f},
        {EPNI_LBG_TX_MC_PKT_COUNTER_4r,         LBG_TX_MC_PKT_COUNTER_4f},
        {EPNI_LBG_TX_MC_PKT_COUNTER_6r,         LBG_TX_MC_PKT_COUNTER_6f},
        {EPNI_LBG_TX_MC_PKT_COUNTER_8r,         LBG_TX_MC_PKT_COUNTER_8f},
        {EPNI_LBG_TX_MC_PKT_COUNTER_10r,        LBG_TX_MC_PKT_COUNTER_10f},
        {EPNI_LBG_TX_BC_PKT_COUNTER_0r,         LBG_TX_BC_PKT_COUNTER_0f},
        {EPNI_LBG_TX_BC_PKT_COUNTER_2r,         LBG_TX_BC_PKT_COUNTER_2f},
        {EPNI_LBG_TX_BC_PKT_COUNTER_4r,         LBG_TX_BC_PKT_COUNTER_4f},
        {EPNI_LBG_TX_BC_PKT_COUNTER_6r,         LBG_TX_BC_PKT_COUNTER_6f},
        {EPNI_LBG_TX_BC_PKT_COUNTER_8r,         LBG_TX_BC_PKT_COUNTER_8f},
        {EPNI_LBG_TX_BC_PKT_COUNTER_10r,        LBG_TX_BC_PKT_COUNTER_10f},
        {EPNI_LBG_TX_PKT_64_COUNTER_0r,         LBG_TX_PKT_64_COUNTER_0f},
        {EPNI_LBG_TX_PKT_64_COUNTER_2r,         LBG_TX_PKT_64_COUNTER_2f},
        {EPNI_LBG_TX_PKT_64_COUNTER_4r,         LBG_TX_PKT_64_COUNTER_4f},
        {EPNI_LBG_TX_PKT_64_COUNTER_6r,         LBG_TX_PKT_64_COUNTER_6f},
        {EPNI_LBG_TX_PKT_64_COUNTER_8r,         LBG_TX_PKT_64_COUNTER_8f},
        {EPNI_LBG_TX_PKT_64_COUNTER_10r,        LBG_TX_PKT_64_COUNTER_10f},
        {EPNI_LBG_TX_PKT_65_COUNTER_0r,         LBG_TX_PKT_65_COUNTER_0f},
        {EPNI_LBG_TX_PKT_65_COUNTER_2r,         LBG_TX_PKT_65_COUNTER_2f},
        {EPNI_LBG_TX_PKT_65_COUNTER_4r,         LBG_TX_PKT_65_COUNTER_4f},
        {EPNI_LBG_TX_PKT_65_COUNTER_6r,         LBG_TX_PKT_65_COUNTER_6f},
        {EPNI_LBG_TX_PKT_65_COUNTER_8r,         LBG_TX_PKT_65_COUNTER_8f},
        {EPNI_LBG_TX_PKT_65_COUNTER_10r,        LBG_TX_PKT_65_COUNTER_10f},
        {EPNI_LBG_TX_PKT_128_COUNTER_0r,        LBG_TX_PKT_128_COUNTER_0f},
        {EPNI_LBG_TX_PKT_128_COUNTER_2r,        LBG_TX_PKT_128_COUNTER_2f},
        {EPNI_LBG_TX_PKT_128_COUNTER_4r,        LBG_TX_PKT_128_COUNTER_4f},
        {EPNI_LBG_TX_PKT_128_COUNTER_6r,        LBG_TX_PKT_128_COUNTER_6f},
        {EPNI_LBG_TX_PKT_128_COUNTER_8r,        LBG_TX_PKT_128_COUNTER_8f},
        {EPNI_LBG_TX_PKT_128_COUNTER_10r,       LBG_TX_PKT_128_COUNTER_10f},
        {EPNI_LBG_TX_PKT_256_COUNTER_0r,        LBG_TX_PKT_256_COUNTER_0f},
        {EPNI_LBG_TX_PKT_256_COUNTER_2r,        LBG_TX_PKT_256_COUNTER_2f},
        {EPNI_LBG_TX_PKT_256_COUNTER_4r,        LBG_TX_PKT_256_COUNTER_4f},
        {EPNI_LBG_TX_PKT_256_COUNTER_6r,        LBG_TX_PKT_256_COUNTER_6f},
        {EPNI_LBG_TX_PKT_256_COUNTER_8r,        LBG_TX_PKT_256_COUNTER_8f},
        {EPNI_LBG_TX_PKT_256_COUNTER_10r,       LBG_TX_PKT_256_COUNTER_10f},
        {EPNI_LBG_TX_PKT_512_COUNTER_0r,        LBG_TX_PKT_512_COUNTER_0f},
        {EPNI_LBG_TX_PKT_512_COUNTER_2r,        LBG_TX_PKT_512_COUNTER_2f},
        {EPNI_LBG_TX_PKT_512_COUNTER_4r,        LBG_TX_PKT_512_COUNTER_4f},
        {EPNI_LBG_TX_PKT_512_COUNTER_6r,        LBG_TX_PKT_512_COUNTER_6f},
        {EPNI_LBG_TX_PKT_512_COUNTER_8r,        LBG_TX_PKT_512_COUNTER_8f},
        {EPNI_LBG_TX_PKT_512_COUNTER_10r,       LBG_TX_PKT_512_COUNTER_10f},
        {EPNI_LBG_TX_PKT_1024_COUNTER_0r,       LBG_TX_PKT_1024_COUNTER_0f},
        {EPNI_LBG_TX_PKT_1024_COUNTER_2r,       LBG_TX_PKT_1024_COUNTER_2f},
        {EPNI_LBG_TX_PKT_1024_COUNTER_4r,       LBG_TX_PKT_1024_COUNTER_4f},
        {EPNI_LBG_TX_PKT_1024_COUNTER_6r,       LBG_TX_PKT_1024_COUNTER_6f},
        {EPNI_LBG_TX_PKT_1024_COUNTER_8r,       LBG_TX_PKT_1024_COUNTER_8f},
        {EPNI_LBG_TX_PKT_1024_COUNTER_10r,      LBG_TX_PKT_1024_COUNTER_10f},
        {EPNI_LBG_TX_PKT_1519_COUNTER_0r,       LBG_TX_PKT_1519_COUNTER_0f},
        {EPNI_LBG_TX_PKT_1519_COUNTER_2r,       LBG_TX_PKT_1519_COUNTER_2f},
        {EPNI_LBG_TX_PKT_1519_COUNTER_4r,       LBG_TX_PKT_1519_COUNTER_4f},
        {EPNI_LBG_TX_PKT_1519_COUNTER_6r,       LBG_TX_PKT_1519_COUNTER_6f},
        {EPNI_LBG_TX_PKT_1519_COUNTER_8r,       LBG_TX_PKT_1519_COUNTER_8f},
        {EPNI_LBG_TX_PKT_1519_COUNTER_10r,      LBG_TX_PKT_1519_COUNTER_10f},
        {EPNI_LBG_TX_PKT_COUNTER_0r,            LBG_TX_PKT_COUNTER_0f},
        {EPNI_LBG_TX_PKT_COUNTER_2r,            LBG_TX_PKT_COUNTER_2f},
        {EPNI_LBG_TX_PKT_COUNTER_4r,            LBG_TX_PKT_COUNTER_4f},
        {EPNI_LBG_TX_PKT_COUNTER_6r,            LBG_TX_PKT_COUNTER_6f},
        {EPNI_LBG_TX_PKT_COUNTER_8r,            LBG_TX_PKT_COUNTER_8f},
        {EPNI_LBG_TX_PKT_COUNTER_10r,           LBG_TX_PKT_COUNTER_10f},
        {EPNI_LBG_TX_BYTES_COUNTER_0r,          LBG_TX_BYTES_COUNTER_0f},
        {EPNI_LBG_TX_BYTES_COUNTER_2r,          LBG_TX_BYTES_COUNTER_2f},
        {EPNI_LBG_TX_BYTES_COUNTER_4r,          LBG_TX_BYTES_COUNTER_4f},
        {EPNI_LBG_TX_BYTES_COUNTER_6r,          LBG_TX_BYTES_COUNTER_6f},
        {EPNI_LBG_TX_BYTES_COUNTER_8r,          LBG_TX_BYTES_COUNTER_8f},
        {EPNI_LBG_TX_BYTES_COUNTER_10r,         LBG_TX_BYTES_COUNTER_10f},
    };

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(value);
    if (lbg_id == SOC_TMC_LB_GLOBAL) {
        is_global = 1;
    }
    else {
        is_global = 0;
        SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    }

    if (is_global) {
        if (type == socLbStatsErrorPkts) {
            reg =EPNI_LBG_TX_ERROR_PKTS_COUNTERr;
            rv = soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64);
            SOCDNX_IF_ERR_EXIT(rv);

            *value = soc_reg64_field_get(unit, reg, reg_val64, LBG_TX_ERROR_PKTS_COUNTERf); 
        }
        else {
            if (type < socLbStatsCount) {
                SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL); 
            }

            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                (_BSL_SOCDNX_MSG("%s: Statistic not supported: %d\n"), FUNCTION_NAME(), type));
        }
    }
    else {
        switch (type) {
        case socLbStatsUnicastPkts:
        case socLbStatsMulticastPkts:
        case socLbStatsBroadcastPkts:
        case socLbStatsPkts64Octets:
        case socLbStatsPkts65to127Octets:
        case socLbStatsPkts128to255Octets:
        case socLbStatsPkts256to511Octets:
        case socLbStatsPkts512to1023Octets:
        case socLbStatsPkts1024to1518Octets:
        case socLbStatsPkts1519Octets:
        case socLbStatsPkts:
        case socLbStatsOctets:
            reg = lbg_egr_stats_reg_info[type*SOC_TMC_LB_NOF_LBG+lbg_id].reg;
            rv = soc_reg64_get(unit, reg, REG_PORT_ANY, 0, &reg_val64);
            SOCDNX_IF_ERR_EXIT(rv);

            *value = soc_reg64_field_get(unit, reg, reg_val64, lbg_egr_stats_reg_info[type*SOC_TMC_LB_NOF_LBG+lbg_id].fld);
            break;
        default:
            if (type < socLbStatsCount) {
                SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL); 
            }

            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
                 (_BSL_SOCDNX_MSG("%s: Statistic not supported: %d\n"), FUNCTION_NAME(), type));
        }
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_egr_modem_stat_get(
    SOC_SAND_IN  int                          unit, 
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_IN  soc_lb_modem_stat_val_t      type, 
    SOC_SAND_OUT uint64                      *value
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_t 
        reg = ILB_FIFO_EMPTY_REGr;
    uint64
        reg_val64 = COMPILER_64_INIT(0, 0);

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(value);
    SOC_TMC_LB_VALUE_MAX_CHECK(modem_id, SOC_TMC_LB_MODEM_MAX, "modem_id");

    switch (type) {
    case socLbModemStatsPkts:
        reg = EPNI_LBG_PACKET_COUNTER_MODEMr;

        rv = soc_reg64_get(unit, reg, REG_PORT_ANY, modem_id, &reg_val64);
        SOCDNX_IF_ERR_EXIT(rv);
        *value = soc_reg64_field_get(unit, reg, reg_val64, LBG_PACKET_COUNTER_MODEMf);
        break;
    case socLbModemStatsOctets:
        reg = EPNI_LBG_BYTES_COUNTER_MODEMr;

        rv = soc_reg64_get(unit, reg, REG_PORT_ANY, modem_id, &reg_val64);
        SOCDNX_IF_ERR_EXIT(rv);
        *value = soc_reg64_field_get(unit, reg, reg_val64, LBG_BYTES_COUNTER_MODEMf);            
        break;
    default:
        if (type < socLbModemStatsCount) {
            SOCDNX_IF_ERR_EXIT(SOC_E_UNAVAIL); 
        }
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM,
            (_BSL_SOCDNX_MSG("%s: Statistic not supported: %d\n"), FUNCTION_NAME(), type));
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_lb_lbg_to_ingress_receive_editor(
    SOC_SAND_IN  int              unit, 
    SOC_SAND_IN  soc_port_t       port
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_reg_above_64_val_t 
        lbg_data;
    soc_reg_t 
        lbg_reg = ILB_LBRG_CONFIGURATIONr;
    soc_lbg_t
        lbg_id;
    uint32
        phy_port;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
    phy_port--; 

    rv = sw_state_access[unit].dpp.bcm.linkbonding.port_to_lbg.get(unit, port, &lbg_id);
    SOCDNX_IF_ERR_EXIT(rv);
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    SOC_REG_ABOVE_64_CLEAR(lbg_data);
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data));
    soc_reg_above_64_field32_set(unit, lbg_reg, lbg_data, LBRG_N_OUTPUT_PORTf, phy_port); 
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, lbg_reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), lbg_data));

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_lb_lbg_egr_interface_get_by_port(
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT uint32     *egr_if
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_lbg_t
        lbg_id = SOC_TMC_LB_LBG_INVALID;
    uint32
        is_valid = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(egr_if);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if(!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,
            (_BSL_SOCDNX_MSG("%s: port(%d) is invalid\n"), FUNCTION_NAME(), port));
    }

    rv = sw_state_access[unit].dpp.bcm.linkbonding.port_to_lbg.get(unit, port, &lbg_id);
    SOCDNX_IF_ERR_EXIT(rv);
    SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");

    *egr_if = SOC_QAX_EGR_IF_GET_BY_LBG(lbg_id);

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_lb_lbg_egr_interface_alloc(
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT uint32     *egr_if
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        flags = 0, 
        egr_if_tmp = 0;
    uint32
        lb_egr_if_bmp_used[1] = {0};
    uint32
        is_found = 0;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_NULL_CHECK(egr_if);
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    if (!SOC_PORT_IS_LB_MODEM(flags)) {
        *egr_if = SOC_QAX_EGR_IF_LBG_MIN;
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,
            (_BSL_SOCDNX_MSG("%s: port(%d) should be LB Modem\n"), FUNCTION_NAME(), port));
    }

    *egr_if = INVALID_EGR_INTERFACE;
    if (SOC_IS_QUX(unit)) {
        SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_if_bmp_used.bit_range_read(
        unit, 0, 0, SOC_QAX_NOF_EGR_IF_LBG, lb_egr_if_bmp_used));
    LOG_VERBOSE(BSL_LS_SOC_LB, (BSL_META_U(unit, "Get sw db: lb_egr_if_bmp_used(%x)\n"), lb_egr_if_bmp_used[0])); 

    *egr_if = INVALID_EGR_INTERFACE;
    for (egr_if_tmp = 0; egr_if_tmp < SOC_QAX_NOF_EGR_IF_LBG; egr_if_tmp++) {
        if (!SHR_BITGET(lb_egr_if_bmp_used, egr_if_tmp)) {
            *egr_if = SOC_QAX_EGR_IF_LBG_MIN + egr_if_tmp;
            is_found = 1;
            break;
        }
    }

    if (is_found) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_if_bmp_used.bit_set(
            unit, egr_if_tmp));
        LOG_VERBOSE(BSL_LS_SOC_LB, (BSL_META_U(unit, "Set sw db: egr_if(%d) in lb_egr_if_bmp_used\n"), egr_if_tmp)); 
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  qax_lb_lbg_egr_interface_free(
    SOC_SAND_IN  int        unit, 
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  uint32     egr_if
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    uint32
        flags = 0; 

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    if (!SOC_PORT_IS_LB_MODEM(flags)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT,
            (_BSL_SOCDNX_MSG("%s: port(%d) should be LB Modem\n"), FUNCTION_NAME(), port));
    }
    SOC_TMC_LB_VALUE_CHECK(egr_if, SOC_QAX_EGR_IF_LBG_MIN, SOC_QAX_EGR_IF_LBG_MAX, "egr_if");

    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_if_bmp_used.bit_clear(
        unit, egr_if));
    LOG_VERBOSE(BSL_LS_SOC_LB, (BSL_META_U(unit, "Clear sw db: egr_if(%d) in lb_egr_if_bmp_used\n"), egr_if));

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;

}

soc_error_t
  qax_lb_modem_ports_on_same_interface_get(
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_OUT soc_pbmp_t *ports
  )
{
    soc_port_t port_i;
    uint32 is_valid;
    int first_phy_port;
    int cur_first_phy_port;
    uint32     flags;
    soc_pbmp_t ports_bm;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.first_phy_port.get(unit, port, &first_phy_port);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_PBMP_CLEAR(*ports);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));

    SOC_PBMP_ITER(ports_bm, port_i) {
        if (port == port_i) continue;

        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.first_phy_port.get(unit, port_i, &cur_first_phy_port);
        SOCDNX_IF_ERR_EXIT(rv);
        if (first_phy_port == cur_first_phy_port) {
            rv = soc_port_sw_db_flags_get(unit, port_i, &flags);
            SOCDNX_IF_ERR_EXIT(rv);
            if (SOC_PORT_IS_LB_MODEM(flags)) {
                SOC_PBMP_PORT_ADD(*ports, port_i);
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_ports_on_reserve_intf_get(
    SOC_SAND_IN  int         unit, 
    SOC_SAND_OUT soc_pbmp_t *ports,
    SOC_SAND_OUT uint32     *port_num
  )
{
    soc_port_t port_i;
    uint32 egr_if;
    uint32 port_num_tmp = 0;
    soc_pbmp_t ports_bm;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_DPP_CONFIG(unit)->qax->lbi_en) {
        rv = SOC_E_UNAVAIL;
        SOC_EXIT;
    }

    SOC_PBMP_CLEAR(*ports);

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));

    SOC_PBMP_ITER(ports_bm, port_i) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port_i, &egr_if);
        SOCDNX_IF_ERR_EXIT(rv);

        if (egr_if == SOC_QAX_EGR_IF_LBG_RESERVE) {
            SOC_PBMP_PORT_ADD(*ports, port_i);
            port_num_tmp++;
        }
    }

    *port_num = port_num_tmp;

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_port_speed_validate (
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  int         is_check_by_port, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_IN  soc_lbg_t   lbg_id,
    SOC_SAND_IN  int         speed
  )
{
    soc_lbg_t
        lbg_id_tmp = 0;
    uint32
        is_valid = 0,
        fifo_size = 0,
        credit_size = 0;
    int
        lb_egr_fifo_range = 0,
        pcnt = 0;
    soc_pbmp_t 
        modem_pbmp;
    soc_error_t 
        rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_LB, (BSL_META_U(unit, "%s: is_check_by_port(%d) port(%d) lbg(%d) speed(%d)\n"), 
        __func__, is_check_by_port, port, lbg_id, speed)); 

    
    if (is_check_by_port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
        if (!is_valid) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
        }

        SOCDNX_IF_ERR_EXIT(qax_lb_modem_ports_on_same_interface_get(unit, port, &modem_pbmp)); 
        SOC_PBMP_COUNT(modem_pbmp, pcnt);
        if (IS_LBG_PORT(unit,port)) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.linkbonding.port_to_lbg.get(unit, port, &lbg_id_tmp));
            SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id_tmp, SOC_TMC_LB_LBG_MAX, "lbg_id");
            lb_egr_fifo_range = LB_EGR_FIFO_RANGE_PER_LBG;
        }
        else if (SOC_DPP_CONFIG(unit)->qax->lbi_en && (pcnt != 0)) {
            lbg_id_tmp = SOC_TMC_LB_LBG_MAX+1;
            lb_egr_fifo_range = 1;
        }
        else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is not lb port"),port));
        }
    }
    else {
        lbg_id_tmp = lbg_id;
    }
    SOC_TMC_LB_VALUE_CHECK(speed, 0, SOC_TMC_LB_LBG_SPEED_MAX, "lbg speed");

    
    SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_credit_and_fifo_optimial_value_get(unit, speed, &credit_size, &fifo_size));

    
    SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_fifo_alloc(unit, lbg_id_tmp, lb_egr_fifo_range, fifo_size));

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
  qax_lb_port_speed_set(
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_IN  int         speed
  )
{
    soc_lbg_t
        lbg_id = 0;
    uint32
        is_valid;
    uint32
        fifo_size = 0,
        credit_size = 0;
    int
        lb_egr_fifo_nof_entries_tmp1 = 0,
        lb_egr_fifo_nof_entries_tmp2 = 0,
	    lb_egr_fifo_range = 0,
	    lb_egr_credit_entry = 0,
        pcnt = 0;
    soc_pbmp_t 
        modem_pbmp, lb_ports;
    soc_port_t
        lb_port;
    uint32
        lb_port_num = 0;
    soc_error_t 
        rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

    LOG_VERBOSE(BSL_LS_SOC_LB, (BSL_META_U(unit, "%s: port(%d) speed(%d)\n"), 
        __func__, port, speed));

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(qax_lb_modem_ports_on_same_interface_get(unit, port, &modem_pbmp)); 
    SOC_PBMP_COUNT(modem_pbmp, pcnt);
    if (IS_LBG_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.linkbonding.port_to_lbg.get(unit, port, &lbg_id));
        SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
        lb_egr_fifo_range = LB_EGR_FIFO_RANGE_PER_LBG; 
        lb_egr_credit_entry = LB_EGR_CREDIT_ENTRY_PER_LBG;
    }
    else if (SOC_DPP_CONFIG(unit)->qax->lbi_en && (pcnt != 0)) {
        lbg_id = SOC_TMC_LB_LBG_MAX+1;
        lb_egr_fifo_range = 1;
        lb_egr_credit_entry = 1;
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is not lb port"),port));
    }
    SOC_TMC_LB_VALUE_CHECK(speed, 0, SOC_TMC_LB_LBG_SPEED_MAX, "lbg speed");

    if (0 == speed) {
       SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_fifo_free(unit, lbg_id, lb_egr_fifo_range)); 
    } else {
        
        SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_credit_and_fifo_optimial_value_get(unit, speed, &credit_size, &fifo_size));

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_nof_entries.get(unit, LB_EGR_FIFO_RANGE_IDX_BASE(lbg_id), &lb_egr_fifo_nof_entries_tmp1));
        if (lb_egr_credit_entry > 1) {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.qax.tm.lb_info.lb_egr_lbg_fifo_nof_entries.get(unit, LB_EGR_FIFO_RANGE_IDX_BASE(lbg_id) +1, &lb_egr_fifo_nof_entries_tmp2));
        }
        if ((lb_egr_fifo_nof_entries_tmp1 != fifo_size) || 
            ((lb_egr_credit_entry > 1) && (lb_egr_fifo_nof_entries_tmp2 != fifo_size))) {
            SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_fifo_alloc(unit, lbg_id, lb_egr_fifo_range, fifo_size));
        }

        
        SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_enable_set(unit, lbg_id, 0));

        
        SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_fifo_set(unit, lbg_id, lb_egr_fifo_range));

        
        SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_enable_set(unit, lbg_id, 1));

        
        SOCDNX_IF_ERR_EXIT(_qax_lb_egr_lbg_credit_size_set(unit, lbg_id, lb_egr_credit_entry, credit_size));
    }

    if (SOC_DPP_CONFIG(unit)->qax->lbi_en && 
        ((SOC_TMC_LB_LBG_MAX+1) == lbg_id)) {
        SOCDNX_IF_ERR_EXIT(qax_lb_ports_on_reserve_intf_get(unit, &lb_ports, &lb_port_num));
        SOC_PBMP_ITER(lb_ports, lb_port) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_set(unit, port, speed));
        }
    }

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_port_speed_get(
    SOC_SAND_IN  int         unit, 
    SOC_SAND_IN  soc_port_t  port,
    SOC_SAND_OUT int         *speed
  )
{
    soc_lbg_t
        lbg_id = 0;
    uint32
        is_valid;
    int
        pcnt = 0;
    soc_pbmp_t 
        modem_pbmp;
    soc_error_t 
        rv = SOC_E_NONE;

    SOCDNX_INIT_FUNC_DEFS;

     
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }
    SOCDNX_IF_ERR_EXIT(qax_lb_modem_ports_on_same_interface_get(unit, port, &modem_pbmp)); 
    SOC_PBMP_COUNT(modem_pbmp, pcnt);
    if (IS_LBG_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.bcm.linkbonding.port_to_lbg.get(unit, port, &lbg_id));
        SOC_TMC_LB_VALUE_MAX_CHECK(lbg_id, SOC_TMC_LB_LBG_MAX, "lbg_id");
    }
    else if (SOC_DPP_CONFIG(unit)->qax->lbi_en && (pcnt != 0)) {
        lbg_id = SOC_TMC_LB_LBG_MAX+1;
    }
    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is not lb port"),port));
    }

    SOCDNX_NULL_CHECK(speed);

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, speed));

    SOCDNX_IF_ERR_EXIT(rv);
exit:
    SOCDNX_FUNC_RETURN;
}


STATIC soc_error_t
  _qax_lb_ing_glb_config_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT soc_lb_cfg_rx_glb_info_t *rx_glb_info 
  )
{
    soc_error_t rv = SOC_E_NONE;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0;
    int 
        ilb_buffer_size = 0, 
        ilb_buffer_size_hw = 0, 
        ilb_buffer_mode = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(rx_glb_info);

    
    reg = ECI_GLOBAL_GENERAL_CFG_3r;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
    ilb_buffer_size_hw = soc_reg_field_get(unit, reg, reg_val32, ILB_BUFFER_SIZEf);
    switch (ilb_buffer_size_hw) {
    case 0:  
    case 1:  
    case 2:  
    case 3:  
    case 4:  
    case 5:  
    case 6:  
    case 7:  
    case 8:  
        ilb_buffer_size = ilb_buffer_size_hw*2;
        break;
    default:
        SOCDNX_IF_ERR_EXIT(SOC_E_INTERNAL);
    }
    rx_glb_info->buffer_size = ilb_buffer_size;

    
    rv = qax_lb_ing_glb_seqeunce_num_width_get(unit, &(rx_glb_info->seq_num_width));
    SOCDNX_IF_ERR_EXIT(rv);

    if (SOC_IS_QUX(unit)) {
        
        rv = qax_lb_ing_total_shared_buffer_config_get(unit, &(rx_glb_info->nof_total_shared_buffer));
        SOCDNX_IF_ERR_EXIT(rv);

        
        reg = ECI_GLOBAL_GENERAL_CFG_3r;
        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val32)); 
        ilb_buffer_mode = soc_reg_field_get(unit, reg, reg_val32, ILB_BUFFER_MODEf);
        rx_glb_info->single_buffer_size = (ilb_buffer_mode == 0) ? 256 : 128;
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  _qax_lb_ing_lbg_config_info_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_lbg_t                  lbg_id,
    SOC_SAND_OUT soc_lb_cfg_rx_lbg_info_t  *rx_lbg_info 
  )
{
    soc_error_t rv = SOC_E_NONE;
    soc_reg_above_64_val_t 
        reg_abv64;
    soc_reg_t 
        reg = INVALIDr;
    soc_port_t 
        port = 0;
    soc_lbg_t
        lbg_id_tmp = 0;
    soc_lb_ing_reorder_config_t
        reorder_config;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(rx_lbg_info);

    
    SOC_REG_ABOVE_64_CLEAR(reg_abv64);
    reg = ILB_LBRG_CONFIGURATIONr;
    rv = soc_reg_above_64_get(unit, reg, REG_PORT_ANY, QAX_ILB_LBRG_CONFIGURATION_INDEX(lbg_id), reg_abv64);
    SOCDNX_IF_ERR_EXIT(rv);

    rx_lbg_info->modem_bmp = soc_reg_above_64_field32_get(unit, reg, reg_abv64, LBRG_N_ACTIVE_MODEMSf);
    rx_lbg_info->ptc = soc_reg_above_64_field32_get(unit, reg, reg_abv64, LBRG_N_OUTPUT_PORTf);

    
    rx_lbg_info->logic_port = BCM_LB_PORT_INVALID;
    BCM_PBMP_ITER(PBMP_LBG_ALL(unit), port) {
        rv = sw_state_access[unit].dpp.bcm.linkbonding.port_to_lbg.get(unit, port, &lbg_id_tmp);
        SOCDNX_IF_ERR_EXIT(rv);
        if (lbg_id_tmp == lbg_id) {
            rx_lbg_info->logic_port = port;
        }
    }

    
    rv = qax_lb_ing_reorder_config_get(unit, lbg_id, &reorder_config);
    SOCDNX_IF_ERR_EXIT(rv);
    rx_lbg_info->timeout = reorder_config.timeout_thresh;
    rx_lbg_info->max_out_of_order = reorder_config.max_out_of_order;
    rx_lbg_info->max_buffer = reorder_config.max_buffer;

    if (SOC_IS_QUX(unit)) {
        
        rv = qax_lb_ing_guaranteed_buffer_config_get(unit, lbg_id, &(rx_lbg_info->nof_guaranteed_buffer));
        SOCDNX_IF_ERR_EXIT(rv);
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  _qax_lb_ing_modem_config_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT soc_lb_cfg_rx_modem_info_t  *rx_modem_info 
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_lbg_t
        lbg_id_tmp = 0;
    soc_port_t 
        port = 0;
    int
        vlan = 0;
    soc_lb_rx_modem_map_index_t
        map_index;
    soc_lb_rx_modem_map_config_t
        map_config;
    soc_reg_t
        reg = INVALIDr;
    uint32
        reg_val32 = 0;
    uint32
        fifo_size = 0, 
        start_fifo = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(rx_modem_info);

    
    rx_modem_info->vlan_bmp = 0;
    rv = sw_state_access[unit].dpp.bcm.linkbonding.modem_to_ports.get(unit, modem_id, &port);
    SOCDNX_IF_ERR_EXIT(rv);
    rx_modem_info->logic_port = port;
    if (BCM_LB_PORT_INVALID != port) {
        for (vlan = BCM_VLAN_DEFAULT; vlan <= SOC_TMC_LB_ING_NOF_VLAN_IN_PKT_TO_MODEM; vlan++) {
            map_index.port = port;
            map_index.vlan = vlan;
            rv = qax_lb_ing_packet_to_modem_map_get(unit, &map_index, &map_config);
            SOCDNX_IF_ERR_EXIT(rv);
            if (map_config.modem_id == modem_id) {
                if (SOC_TMC_LB_ING_NOF_VLAN_IN_PKT_TO_MODEM == vlan) {
                    rx_modem_info->vlan_bmp |= 1 << 0;
                }
                else {
                    rx_modem_info->vlan_bmp |= 1 << vlan;
                }
            }
        }
    }

    
    rv = qax_lb_ing_modem_to_lbg_map_get(unit, modem_id, &lbg_id_tmp);
    SOCDNX_IF_ERR_EXIT(rv);
    rx_modem_info->lbg_id = lbg_id_tmp;

    
    reg = ILB_MODEM_FIFO_CONFIGr;
    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, reg, REG_PORT_ANY, modem_id + SOC_TMC_LB_NOF_MODEM, &reg_val32));
    if (!SOC_IS_QUX(unit)) {
        start_fifo = soc_reg_field_get(unit, reg, reg_val32, MODEM_N_START_PTRf);
    }
    fifo_size = soc_reg_field_get(unit, reg, reg_val32, MODEM_N_FIFO_SIZEf);

    rx_modem_info->fifo_start = start_fifo;
    rx_modem_info->fifo_size = fifo_size;

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  _qax_lb_egr_glb_config_info_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_OUT soc_lb_cfg_tx_glb_info_t  *tx_glb_info 
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_lb_global_packet_config_t
        pkt_cfg;
    uint32
        tc;
    uint32
        dp;
    soc_lb_tc_dp_t
        tc_dp;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(tx_glb_info);

    if (!SOC_IS_QUX(unit)) {
        
        rv = qax_lb_egr_segment_config_get(unit, SOC_TMC_LB_GLOBAL, &(tx_glb_info->segment_mode));
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    rv = qax_lb_global_packet_config_get(unit, &pkt_cfg);
    SOCDNX_IF_ERR_EXIT(rv);
    tx_glb_info->vlan_tpid = pkt_cfg.outer_vlan_tpid;
    tx_glb_info->lbg_tpid = pkt_cfg.lb_tpid;
    if (!SOC_IS_QUX(unit)) {
        tx_glb_info->is_pkt_crc_ena = pkt_cfg.packet_crc_enable;
    }

    
    for (tc = 0; tc < SOC_TMC_NOF_TRAFFIC_CLASSES; tc++) {
        for (dp = 0; dp < SOC_TMC_NOF_DROP_PRECEDENCE; dp++) {
            tc_dp.tc = tc;
            tc_dp.dp = dp;
            rv = qax_lb_tc_dp_to_packet_priority_config_get(unit, &tc_dp, &(tx_glb_info->pkt_pri[tc][dp]));
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  _qax_lb_egr_lbg_config_info_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  soc_lbg_t                  lbg_id,
    SOC_SAND_OUT soc_lb_cfg_tx_lbg_info_t  *tx_lbg_info 
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_mem_t 
        mem = INVALIDm;
    uint32
        entry_data32 = 0;
    soc_port_t 
        port = 0;
    soc_lbg_t
        lbg_id_tmp = 0;
    soc_modem_t
        modem_id_tmp = 0;
    soc_lb_lbg_weight_t
        lbg_weights[SOC_TMC_LB_NOF_LBG];
    uint32
        lbg_count = 0;
    soc_reg_above_64_val_t
        entry_data;
    uint32
        first_fifo = 0, 
        last_fifo = 0;
    soc_lb_lbg_packet_config_t
        lbg_pkt;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(tx_lbg_info);

    
    rv = qax_lb_egr_lbg_enable_get(unit, lbg_id, &(tx_lbg_info->is_enable));
    SOCDNX_IF_ERR_EXIT(rv);

    
    mem = EPNI_LBG_MAPPINGm;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, lbg_id, &entry_data32);
    SOCDNX_IF_ERR_EXIT(rv);
    tx_lbg_info->modem_bmp = soc_mem_field32_get(unit, mem, &entry_data32, LBG_MAPPINGf);

    
    tx_lbg_info->logic_port = BCM_LB_PORT_INVALID;
    BCM_PBMP_ITER(PBMP_LBG_ALL(unit), port) {
        rv = sw_state_access[unit].dpp.bcm.linkbonding.port_to_lbg.get(unit, port, &lbg_id_tmp);
        SOCDNX_IF_ERR_EXIT(rv);
        if (lbg_id_tmp == lbg_id) {
            tx_lbg_info->logic_port = port;
        }
    }

    
    mem = EPNI_LBG_FIFO_CONFIGm;
    SOC_REG_ABOVE_64_CLEAR(entry_data);
    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, mem, MEM_BLOCK_ANY, lbg_id*2, entry_data));
    first_fifo = soc_mem_field32_get(unit, mem, entry_data, LBG_FIFO_FIRST_POINTERf);
    last_fifo = soc_mem_field32_get(unit, mem, entry_data, LBG_FIFO_LAST_POINTERf);
    tx_lbg_info->fifo_start = first_fifo;
    tx_lbg_info->fifo_end = last_fifo;

    
    if (SOC_IS_QUX(unit)) {
        
        for (modem_id_tmp = 0; modem_id_tmp < (SOC_TMC_LB_MODEM_MAX+1); modem_id_tmp++) {
            rv = LB_INFO_ACCESS.modem_to_egr_lbg.get(unit, modem_id_tmp, &lbg_id_tmp);
            SOCDNX_IF_ERR_EXIT(rv);

            if (lbg_id_tmp == lbg_id) {
                rv = qax_lb_egr_modem_seqeunce_num_width_get(unit, modem_id_tmp, &(tx_lbg_info->seq_num_width));
                SOCDNX_IF_ERR_EXIT(rv);
                break;
            }
        }
    }
    else {
        rv = qax_lb_egr_seqeunce_num_width_get(unit, lbg_id, &(tx_lbg_info->seq_num_width));
        SOCDNX_IF_ERR_EXIT(rv);
    }

    
    tx_lbg_info->lbg_weight = 0;
    rv = qax_lb_egr_sched_get(unit, SOC_TMC_LB_NOF_LBG, lbg_weights, &lbg_count);
    SOCDNX_IF_ERR_EXIT(rv);
    for (lbg_id_tmp = 0; lbg_id_tmp < lbg_count; lbg_id_tmp++) {
        if (lbg_weights[lbg_id_tmp].lbg_id == lbg_id) {
            tx_lbg_info->lbg_weight  = lbg_weights[lbg_id_tmp].lbg_weight;
            break;
        }
    }

    
    if (SOC_IS_QUX(unit)) {
        
        rv = qax_lb_egr_segment_config_get(unit, lbg_id, &(tx_lbg_info->segment_mode));
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_IS_QUX(unit)) {
        
        rv = qax_lb_lbg_packet_config_get(unit, lbg_id, &lbg_pkt);
        SOCDNX_IF_ERR_EXIT(rv);
        tx_lbg_info->is_pkt_crc_ena = lbg_pkt.packet_crc_enable;
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t
  _qax_lb_egr_modem_config_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  soc_modem_t                  modem_id,
    SOC_SAND_OUT soc_lb_cfg_tx_modem_info_t  *tx_modem_info 
  )
{
    soc_error_t
        rv = SOC_E_NONE;
    soc_lbg_t
        lbg_id_tmp = 0;
    soc_port_t
        port = 0;
    soc_mem_t 
        mem = INVALIDm;
    soc_reg_above_64_val_t 
        reg_abv64;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(tx_modem_info);

    
    rv = qax_lb_egr_modem_to_lbg_map_get(unit, modem_id, &lbg_id_tmp);
    SOCDNX_IF_ERR_EXIT(rv);
    tx_modem_info->lbg_id = lbg_id_tmp;

    
    rv = qax_lb_modem_packet_config_get(unit, modem_id, &(tx_modem_info->modem_packet));
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = qax_lb_egr_modem_shaper_get(unit, modem_id, &(tx_modem_info->modem_shaper));
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOC_REG_ABOVE_64_CLEAR(reg_abv64);
    mem = EPNI_LBG_MODEM_CONFIGm;
    rv = soc_mem_read(unit, mem, MEM_BLOCK_ANY, modem_id, reg_abv64);
    SOCDNX_IF_ERR_EXIT(rv);
    tx_modem_info->egr_intf = soc_mem_field32_get(unit, mem, reg_abv64, EGQ_IFCf);

       
    rv = sw_state_access[unit].dpp.bcm.linkbonding.modem_to_ports.get(unit, modem_id, &port);
    SOCDNX_IF_ERR_EXIT(rv);
    tx_modem_info->logic_port = port;

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
  qax_lb_config_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  soc_lb_info_key_t      *lb_key,
    SOC_SAND_OUT soc_lb_cfg_info_t      *lb_cfg_info 
  )
{
    soc_error_t rv = SOC_E_NONE;
    soc_lbg_t lbg_tmp = 0, lbg_start = 0, lbg_end = 0;
    soc_modem_t modem_tmp = 0, modem_start = 0, modem_end = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(lb_key);
    SOCDNX_NULL_CHECK(lb_cfg_info);
    if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
        if (lb_key->lbg_id != -1) {
            SOC_TMC_LB_VALUE_MAX_CHECK(lb_key->lbg_id, SOC_TMC_LB_LBG_MAX, "lb_key->lbg_id");
        }
    }
    if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
        if (lb_key->modem_id != -1) {
            SOC_TMC_LB_VALUE_MAX_CHECK(lb_key->modem_id, SOC_TMC_LB_LBG_MAX, "lb_key->modem_id");
        }
    }

    if ((socLbDirectionRx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) {
        if ((socLbObjGlobal == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            
            rv = _qax_lb_ing_glb_config_info_get(unit, &(lb_cfg_info->rx_cfg_info.rx_glb_info));
            SOCDNX_IF_ERR_EXIT(rv);
        }

        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            
            if (lb_key->lbg_id == -1) {
                lbg_start = 0;
                lbg_end = SOC_TMC_LB_LBG_MAX;
            }
            else {
                lbg_start = lbg_end = lb_key->lbg_id;
            }

            for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
                rv = _qax_lb_ing_lbg_config_info_get(unit, lbg_tmp, &(lb_cfg_info->rx_cfg_info.rx_lbg_info[lbg_tmp]));
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            
            if (lb_key->modem_id == -1) {
                modem_start = 0;
                modem_end = SOC_TMC_LB_MODEM_MAX;
            }
            else {
                modem_start = modem_end = lb_key->modem_id;
            }

            for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
                rv = _qax_lb_ing_modem_config_info_get(unit, modem_tmp, &(lb_cfg_info->rx_cfg_info.rx_modem_info[modem_tmp]));
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    if ((socLbDirectionTx == lb_key->direction) || (socLbDirectionBoth == lb_key->direction)) {
        if ((socLbObjGlobal == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            
            rv = _qax_lb_egr_glb_config_info_get(unit, &(lb_cfg_info->tx_cfg_info.tx_glb_info));
            SOCDNX_IF_ERR_EXIT(rv);
        }

        if ((socLbObjLbg == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            
            if (lb_key->lbg_id == -1) {
                lbg_start = 0;
                lbg_end = SOC_TMC_LB_LBG_MAX;
            }
            else {
                lbg_start = lbg_end = lb_key->lbg_id;
            }

            for (lbg_tmp = lbg_start; lbg_tmp <= lbg_end; lbg_tmp++) {
                rv = _qax_lb_egr_lbg_config_info_get(unit, lbg_tmp, &(lb_cfg_info->tx_cfg_info.tx_lbg_info[lbg_tmp]));
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }

        if ((socLbObjModem == lb_key->obj_type) || (socLbObjAll == lb_key->obj_type)) {
            
            if (lb_key->modem_id == -1) {
                modem_start = 0;
                modem_end = SOC_TMC_LB_MODEM_MAX;
            }
            else {
                modem_start = modem_end = lb_key->modem_id;
            }

            for (modem_tmp = modem_start; modem_tmp <= modem_end; modem_tmp++) {
                rv = _qax_lb_egr_modem_config_info_get(unit, modem_tmp, &(lb_cfg_info->tx_cfg_info.tx_modem_info[modem_tmp]));
                SOCDNX_IF_ERR_EXIT(rv);
            }
        }
    }

    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

#endif 
#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>


