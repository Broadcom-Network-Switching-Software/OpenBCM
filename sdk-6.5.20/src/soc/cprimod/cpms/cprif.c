/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <phymod/phymod.h>
#include <shared/bsl.h>
#include <soc/error.h>
#include <soc/esw/port.h>
#include <soc/cprimod/cprimod.h>
#include <soc/cprimod/cprimod_internal.h>
#include <soc/cprimod/cprimod_dispatch.h>
#include <soc/cprimod/cprif_drv.h>
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon2_monterey_interface.h"
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon_api_uc_common.h"
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon_dpll_cfg_seq.h"
#include "../../../../libs/phymod/chip/falcon_dpll/tier1/falcon2_monterey_enum.h"
#include "../../../../libs/phymod/chip/tscf_gen3/tier1/tefmod_gen3.h"


#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_SOC_PORT

#ifdef CPRIMOD_CPRI_FALCON_SUPPORT
#define CPRI_NOF_LANES_IN_CORE     4

#define CPRI_PMD_CRC_UCODE_VERIFY 1

extern unsigned char  falcon_dpll_ucode[];
extern unsigned short falcon_dpll_ucode_ver;
extern unsigned short falcon_dpll_ucode_crc;
extern unsigned short falcon_dpll_ucode_len;


typedef struct _cpri_debug_info_s {
    soc_reg_t       reg;   /* register info */
    soc_field_t     clear_field; /* clear field, it invalid, clear on read. */
    int             clear_supported;
    int             write_supported;
    soc_mem_t       mem; /* mem info */
}_cpri_debug_info_t;


STATIC _cpri_debug_info_t _cpri_port_debug_info[] = {
    {
        INVALIDr,
        INVALIDf,
        0,
        1,
        CPRI_DECAP_QUEUE_STS_0m
    },
    {
        INVALIDr,
        INVALIDf,
        0,
        1,
        CPRI_ENCAP_QUEUE_STS_0m
    },
    {
        INVALIDr,
        INVALIDf,
        1,
        0,
        CPRI_DECAP_QUEUE_STATS_0m
    },
    {
        INVALIDr,
        INVALIDf,
        1,
        0,
        CPRI_ENCAP_QUEUE_STATS_0m
    },
    {
        CPRI_DECAP_DROP_CNTr,
        DECAP_DROP_CNT_CLRf,
        1,
        1,
        INVALIDm
    }
};

STATIC
int _cprif_cpri_port_intr_hierarchy_enable(int unit, int port, int enable);

extern cprimod_interrupt_callback_entry_t cpri_intr_cbs[SOC_MAX_NUM_DEVICES][_shrCpriIntrCount];

int cprif_test_api_dispatch(int unit, int port, int* value)
{
    SOC_INIT_FUNC_DEFS;
    LOG_CLI((" cprif_test_api_dispatch call u=%d port=%d .\n",unit,port));
    SOC_FUNC_RETURN;

}

static
int _cprif_from_speed_toframe_len (cprimod_port_speed_t speed)
{
    int basic_frm_len = 0;

    switch (speed) {
    case cprimodSpd614p4:
        basic_frm_len = 128;        /* 16*8   */
        break;
    case cprimodSpd1228p8:
        basic_frm_len = 256;        /* 16*16  */
        break;
    case cprimodSpd2457p6:
        basic_frm_len = 512;        /* 16*32  */
        break;
    case cprimodSpd3072p0:
        basic_frm_len = 640;        /* 16*40  */
        break;
    case cprimodSpd4915p2:
        basic_frm_len = 1024;       /* 16*64  */
        break;
    case cprimodSpd6144p0:
        basic_frm_len = 1280;       /* 16*80  */
        break;
    case cprimodSpd8110p08:
        basic_frm_len = 2048;       /* 16*128 */
        break;
    case cprimodSpd9830p4:
        basic_frm_len = 2048;       /* 16*128 */
        break;
    case cprimodSpd10137p6:
        basic_frm_len = 2560;       /* 16*160 */
        break;
    case cprimodSpd12165p12:
        basic_frm_len = 3072;       /* 16*192 */
        break;
    case cprimodSpd24330p24:
        basic_frm_len = 6144;       /* 16*384 */
        break;
    default:
        basic_frm_len = 256;        /* 16*16  */
        break;
    }

    return(basic_frm_len);
}

static
int _cprif_from_speed_towd_len (cprimod_port_speed_t speed)
{

    int wd_len = 0;

    switch (speed) {
    case cprimodSpd614p4:
        wd_len = 8;
        break;
    case cprimodSpd1228p8:
        wd_len =  16;
        break;
    case cprimodSpd2457p6:
        wd_len =  32;
        break;
    case cprimodSpd3072p0:
        wd_len =  40;
        break;
    case cprimodSpd4915p2:
        wd_len =  64;
        break;
    case cprimodSpd6144p0:
        wd_len =  80;
        break;
    case cprimodSpd8110p08:
        wd_len =  80;
        break;
    case cprimodSpd9830p4:
        wd_len = 128;
        break;
    case cprimodSpd10137p6:
        wd_len = 128;
        break;
    case cprimodSpd12165p12:
        wd_len = 128;
        break;
    case cprimodSpd24330p24:
        wd_len = 128;
        break;
    default:
        wd_len =  16;
        break;
    }
    return(wd_len);
}

static
int _cprif_get_basic_frm_len(int unit, int port, int *basic_frm_len, cprimod_direction_t dir)
{
    cprimod_port_speed_t speed;
    cprimod_port_interface_type_t interface;

    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_txcpri_port_type_get(unit, port,
                                                       &interface));
    if (interface == cprimodRsvd4){
        /*
         * For RSVD4, there is a fixed table need to be programmed.
         * The size of the table is same as message group size.
         */
        *basic_frm_len = CPRIF_RSVD4_MSG_GRP_SIZE;
    } else {
        if (dir == CPRIMOD_DIR_TX) {
          _SOC_IF_ERR_EXIT(cprif_drv_cpri_txpcs_speed_get(unit, port, &speed));
        } else {
          _SOC_IF_ERR_EXIT(cprif_drv_cpri_rxpcs_speed_get(unit, port, &speed));
        }
        *basic_frm_len = _cprif_from_speed_toframe_len(speed);
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_basic_frame_parser_active_table_set(int unit, int port, cprimod_basic_frame_table_id_t table,  int sync_enable)
{
    cprimod_basic_frame_usage_entry_t* usage_table;
    uint8 new_active_table;
    uint8 current_active_table;
    int num_entries = 0;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    usage_table    = (cprimod_basic_frame_usage_entry_t*)sal_alloc(sizeof(cprimod_basic_frame_usage_entry_t) *
                                                                      CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY, "cprif_drv");
    SOC_NULL_CHECK(usage_table);
    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len, CPRIMOD_DIR_RX));

    if (table == cprimod_basic_frame_table_0) {
        new_active_table = 0;
        current_active_table = 1;
    } else {
        new_active_table = 1;
        current_active_table = 0;
    }
    /*
     * Get Entries from the new_active_table/current standby table to be reprogram to
     * current_active_table/new standby table.
     */
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 new_active_table,
                                                 CPRIMOD_DIR_RX,
                                                 CPRIMOD_AXC_ID_ALL,
                                                 usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,  /* table size */
                                                 &num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_active_table_set(unit, port, new_active_table));

    /*
     * Only when sync_enable, active and standby table will get sync up.
     * This sync_enable is only needed for CPRI structure aware mode.
     */

    if (sync_enable) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                               current_active_table,
                                                               CPRIMOD_DIR_RX,
                                                               basic_frm_len,
                                                               usage_table,
                                                               &num_entries));
        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port, current_active_table, num_entries));
    }
exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_parser_active_table_get(int unit, int port, cprimod_basic_frame_table_id_t* table)
{
    uint8 tmp_table;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_active_table_get(unit, port, &tmp_table));

    if (tmp_table == CPRIF_BASIC_FRAME_TABLE_0) {
        *table = cprimod_basic_frame_table_0;
    } else {
        *table = cprimod_basic_frame_table_1;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_basic_frame_parser_table_num_entries_set(int unit, int port, cprimod_basic_frame_table_id_t table, uint32 num_entries)
{
    uint8 tmp_table;

    SOC_INIT_FUNC_DEFS;

    if (table == cprimod_basic_frame_table_0) {
        tmp_table = 0;
    } else {
        tmp_table = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port, tmp_table, num_entries));

exit:
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_parser_table_num_entries_get(int unit, int port, cprimod_basic_frame_table_id_t table, uint32* num_entries)
{
    uint8 tmp_table;

    SOC_INIT_FUNC_DEFS;

    if (num_entries == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("num_entries NULL parameter"));
    }

    if (table == cprimod_basic_frame_table_0) {
        tmp_table = 0;
    } else {
        tmp_table = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_table_num_entries_get(unit, port, tmp_table, num_entries));

exit:
    SOC_FUNC_RETURN;

}


int cprif_basic_frame_parser_switch_active_table(int unit, int port)
{
    cprimod_basic_frame_table_id_t table;
    SOC_INIT_FUNC_DEFS;

   _SOC_IF_ERR_EXIT
        (cprif_basic_frame_parser_active_table_get(unit, port,&table));
    if (table == cprimod_basic_frame_table_0) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_parser_active_table_set(unit, port, cprimod_basic_frame_table_1));
    } else {
        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_parser_active_table_set(unit, port, cprimod_basic_frame_table_0));
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_basic_frame_assembly_active_table_set(int unit, int port, cprimod_basic_frame_table_id_t table,  int sync_enable)
{
    cprimod_basic_frame_usage_entry_t* usage_table;
    uint8 new_active_table;
    uint8 current_active_table;
    int num_entries = 0;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    usage_table    = (cprimod_basic_frame_usage_entry_t*)sal_alloc(sizeof(cprimod_basic_frame_usage_entry_t) *
                                                                      CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY, "cprif_drv");
    SOC_NULL_CHECK(usage_table);
    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len, CPRIMOD_DIR_TX));

    if (table == cprimod_basic_frame_table_0) {
        new_active_table = 0;
        current_active_table = 1;
    } else {
        new_active_table = 1;
        current_active_table = 0;
    }
    /*
     * Get Entries from the new_active_table/current standby table before activation to be reprogram to
     * current_active_table/new standby table.
     */

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 new_active_table,
                                                 CPRIMOD_DIR_TX,
                                                 CPRIMOD_AXC_ID_ALL,
                                                 usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,  /* table size */
                                                 &num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_active_table_set(unit, port, new_active_table));

    /*
     * Only when sync_enable, active and standby table will get sync up.
     * This sync_enable is only needed for CPRI structure aware mode.
     */
    if (sync_enable) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                               current_active_table,
                                                               CPRIMOD_DIR_TX,
                                                               basic_frm_len,
                                                               usage_table,
                                                               &num_entries));
        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port, current_active_table, num_entries));
    }
exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}


int cprif_basic_frame_assembly_active_table_get(int unit, int port, cprimod_basic_frame_table_id_t* table)
{
    uint8 tmp_table;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_active_table_get(unit, port, &tmp_table));

    if (tmp_table == CPRIF_BASIC_FRAME_TABLE_0) {
        *table = cprimod_basic_frame_table_0;
    } else {
        *table = cprimod_basic_frame_table_1;
    }

exit:
    SOC_FUNC_RETURN;
}
int cprif_basic_frame_assembly_table_num_entries_set(int unit, int port, cprimod_basic_frame_table_id_t table, uint32 num_entries)
{
    uint8 table_num;
    SOC_INIT_FUNC_DEFS;

    if (table == cprimod_basic_frame_table_0) {
        table_num = 0;
    } else {
        table_num = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port, table_num, num_entries));

exit:
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_assembly_table_num_entries_get(int unit, int port, cprimod_basic_frame_table_id_t table, uint32* num_entries)
{
    uint8 table_num;
    SOC_INIT_FUNC_DEFS;

    if (table == cprimod_basic_frame_table_0) {
        table_num = 0;
    } else {
        table_num = 1;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_table_num_entries_get(unit, port, table_num, num_entries));
exit:
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_assembly_switch_active_table(int unit, int port)
{
    cprimod_basic_frame_table_id_t table;
    SOC_INIT_FUNC_DEFS;

   _SOC_IF_ERR_EXIT
        (cprif_basic_frame_assembly_active_table_get(unit, port,&table));
    if (table == cprimod_basic_frame_table_0) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_assembly_active_table_set(unit, port, cprimod_basic_frame_table_1));
    } else {
        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_assembly_active_table_set(unit, port, cprimod_basic_frame_table_0));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_rx_axc_basic_frame_add(int unit, int port,
                                  uint32 axc_id,
                                  uint32 start_bit,
                                  uint32 num_bits)
{
    cprimod_basic_frame_usage_entry_t new_entry;
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    uint8 active_table;
    uint8 standby_table;
    int num_entries = 0 ;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len, CPRIMOD_DIR_RX));
    if ((axc_id >= CPRIF_MAX_NUM_OF_AXC)&&(axc_id != 0xff)) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_parser_active_table_get(unit, port,
                                                                   &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    usage_table = (cprimod_basic_frame_usage_entry_t*)sal_alloc
                        (sizeof(cprimod_basic_frame_usage_entry_t) * CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY , "cprimod");
    CPRIMOD_NULL_CHECK(usage_table);

    new_entry.axc_id = axc_id;
    new_entry.start_bit = start_bit;
    new_entry.num_bits = num_bits;

    /*
     * Transfer HW table to usage table and also add the new
     * entry to the appropriate location.
     */

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entry_add(unit, port,
                                              standby_table,
                                              CPRIMOD_DIR_RX,
                                              new_entry,
                                              usage_table,
                                              CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,
                                              &num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                          standby_table,
                                                          CPRIMOD_DIR_RX,
                                                          basic_frm_len,
                                                          usage_table,
                                                          &num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port, standby_table, num_entries));

exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}

int cprif_rx_axc_basic_frame_delete(int unit, int port,
                                     uint32 axc_id)
{
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    uint8 active_table;
    uint8 standby_table;
    int num_entries = 0;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len, CPRIMOD_DIR_RX));
    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_parser_active_table_get(unit, port,
                                                   &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    usage_table = (cprimod_basic_frame_usage_entry_t*)sal_alloc
                        (sizeof(cprimod_basic_frame_usage_entry_t) * CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY , "cprimod");
    CPRIMOD_NULL_CHECK(usage_table);
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entry_delete(unit, port,
                                                 standby_table,
                                                 CPRIMOD_DIR_RX,
                                                 axc_id,
                                                 usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,
                                                 &num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                          standby_table,
                                                          CPRIMOD_DIR_RX,
                                                          basic_frm_len,
                                                          usage_table,
                                                          &num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port, standby_table, num_entries));


exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}

int cprif_rx_axc_basic_frame_clear(int unit, int port)
{
    cprif_bfa_bfp_table_entry_t table_entry;
    uint8 active_table;
    uint8 standby_table;
    int table_index;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_parser_active_table_get(unit, port,
                                                                    &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    /* clear the entry. */
    cprif_bfa_bfp_table_entry_t_init( &table_entry);

    for (table_index=0; table_index < CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY ; table_index++) {

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_entry_set(unit, port,
                                           standby_table,
                                           CPRIMOD_DIR_RX,
                                           table_index,
                                           &table_entry));
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_axc_basic_frame_get(int unit, int port,
                                  uint32 axc_id,
                                  cprimod_basic_frame_table_id_t table,
                                  cprimod_basic_frame_usage_entry_t* usage_table,
                                  int* num_entries)
{
    uint8 tmp_table;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    if (table == cprimod_basic_frame_table_0) {
        tmp_table = CPRIF_BASIC_FRAME_TABLE_0;
    } else {
        tmp_table = CPRIF_BASIC_FRAME_TABLE_1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                 tmp_table,
                                                 CPRIMOD_DIR_RX,
                                                 axc_id,
                                                 usage_table,
                                                 *num_entries,  /* table size */
                                                 num_entries));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_axc_basic_frame_add(int unit, int port, uint32 axc_id, uint32 start_bit, uint32 num_bits)
{
    cprimod_basic_frame_usage_entry_t new_entry;
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    uint8 active_table;
    uint8 standby_table;
    int num_entries = 0 ;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len, CPRIMOD_DIR_TX));
    if ((axc_id >= CPRIF_MAX_NUM_OF_AXC)&&(axc_id != 0xff)) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_assembly_active_table_get(unit, port,
                                                   &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    usage_table = (cprimod_basic_frame_usage_entry_t*)sal_alloc
                        (sizeof(cprimod_basic_frame_usage_entry_t) * CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY , "cprimod");
    CPRIMOD_NULL_CHECK(usage_table);

    new_entry.axc_id = axc_id;
    new_entry.start_bit = start_bit;
    new_entry.num_bits = num_bits;

    /*
     * Transfer HW table to usage table and also add the new
     * entry to the appropriate location.
     */

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entry_add(unit, port,
                                              standby_table,
                                              CPRIMOD_DIR_TX,
                                              new_entry,
                                              usage_table,
                                              CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,
                                              &num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                          standby_table,
                                                          CPRIMOD_DIR_TX,
                                                          basic_frm_len,
                                                          usage_table,
                                                          &num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port, standby_table, num_entries));

exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}


int cprif_tx_axc_basic_frame_delete(int unit, int port, uint32 axc_id)
{
    cprimod_basic_frame_usage_entry_t* usage_table=NULL;
    uint8 active_table;
    uint8 standby_table;
    int num_entries = 0;
    int basic_frm_len =0;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(_cprif_get_basic_frm_len(unit, port, &basic_frm_len, CPRIMOD_DIR_TX));
    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_assembly_active_table_get(unit, port,
                                                   &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    usage_table = (cprimod_basic_frame_usage_entry_t*)sal_alloc
                        (sizeof(cprimod_basic_frame_usage_entry_t) * CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY , "cprimod");
    CPRIMOD_NULL_CHECK(usage_table);
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entry_delete(unit, port,
                                                 standby_table,
                                                 CPRIMOD_DIR_TX,
                                                 axc_id,
                                                 usage_table,
                                                 CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY,
                                                &num_entries));

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_table_to_hw_table_set(unit, port,
                                                          standby_table,
                                                          CPRIMOD_DIR_TX,
                                                          basic_frm_len,
                                                          usage_table,
                                                          &num_entries));
    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port, standby_table, num_entries));

exit:
    sal_free(usage_table);
    SOC_FUNC_RETURN;

}

int cprif_tx_axc_basic_frame_clear(int unit, int port)
{
    uint16 table_index = 0;
    cprif_bfa_bfp_table_entry_t table_entry;
    uint8 active_table;
    uint8 standby_table;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_basic_frame_assembly_active_table_get(unit, port,
                                                                    &active_table));

    if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
        standby_table = CPRIF_BASIC_FRAME_TABLE_1;
    } else {
        standby_table = CPRIF_BASIC_FRAME_TABLE_0;
    }

    /* clear the entry. */
    cprif_bfa_bfp_table_entry_t_init( &table_entry);

    for (table_index=0; table_index < CPRIMOD_BASIC_FRAME_MAX_NUM_ENTRY ; table_index++) {

        _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_entry_set( unit, port, standby_table,
                                       CPRIMOD_DIR_TX, table_index, &table_entry));
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_axc_basic_frame_get(int unit, int port,
                                  uint32 axc_id,
                                  cprimod_basic_frame_table_id_t table,
                                  cprimod_basic_frame_usage_entry_t* usage_table,
                                  int* num_entries)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_basic_frame_usage_entries_get(unit, port,
                                                table,
                                                CPRIMOD_DIR_TX,
                                                axc_id,
                                                usage_table,
                                                *num_entries,  /* table size */
                                                num_entries));

exit:
    SOC_FUNC_RETURN;

}

int cprif_basic_frame_debug(int unit, int port, uint32 axc_id,
                              cprimod_basic_frame_table_id_t rx_table,
                              cprimod_basic_frame_table_id_t tx_table,
                              uint32 flags)
{

    SOC_INIT_FUNC_DEFS;

    /*
     * If axc_id is not between 0-63, and not control AxC, it want to list all AxCs.
     */
    /* coverity[unsigned_compare:FALSE] */
    if (!((axc_id >= 0) && (axc_id <=63)) &&
       (axc_id != CPRIMOD_AXC_ID_CONTROL)) {
        axc_id = CPRIMOD_AXC_ID_ALL ;
    }


    switch (flags) {

        case CPRIMOD_DEBUG_BASIC_FRAME_RAW :
            cprif_drv_basic_frame_debug_raw (unit, port, axc_id, rx_table, tx_table);
            break;
        case CPRIMOD_DEBUG_BASIC_FRAME_USAGE :
        default:
            cprif_drv_basic_frame_debug_usage (unit, port, axc_id, rx_table, tx_table);
            break;
    }

    SOC_FUNC_RETURN;

}


/*
 *
 *    CONTAINER Mapping
 */
static
int _cprif_cpri_container_map_config_set(int unit, int port,
                                             cprimod_direction_t dir,
                                             uint8 axc_id,
                                             cprimod_cpri_container_config_t* config_info)
{
    cprif_cpri_container_map_entry_t entry;
    uint32 quotient;
    uint32 remainder;
    uint32 Na,Nc,Naxc,K,Nst,Nv; /* CPRI spec. */
    SOC_INIT_FUNC_DEFS;

    cprif_cpri_container_map_entry_t_init( &entry);

    if (config_info->map_method == cprimod_cpri_frame_map_method_1) {
        entry.map_method = CPRIF_CONTAINER_MAP_CPRI_METHOD_1;
    } else if (config_info->map_method == cprimod_cpri_frame_map_method_3) {
        entry.map_method = CPRIF_CONTAINER_MAP_CPRI_METHOD_3;
    } else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Unsupported Map Method Only Method 1 and 3 are supported"));
    }

    Naxc    = config_info->Naxc;
    Nst     = config_info->Nst;
    K       = config_info->K;

    Na      = config_info->Na;
    Nc      = config_info->Nc;
    Nv      = config_info->Nv;

    entry.axc_id = axc_id;

    /* For CPRI, stuffing is always at the beginning. */
    entry.stuffing_at_end = CPRIF_CONTAINER_MAP_STUFFING_AT_BEGINNING;


    if (entry.map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_1) {
        /* MAP METHOD #1 */
        entry.cblk_cnt  = K*Naxc;
        entry.naxc_cnt  = 2;
        entry.stuff_cnt = Nst;
        entry.Na        = 0;
        entry.Nv        = 0;
    } else if (entry.map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_3) {
        /* MAP METHOD #3 */
        entry.naxc_cnt  = Naxc; /* has to be 2M */
        entry.cblk_cnt  = K*Nc*Naxc;

        if (Nv) {
            quotient  = ((K * Nc) / Nv);
            remainder = ((K * Nc) % Nv);
            entry.stuff_cnt = ((quotient & 0x00ff) << 13) | (remainder & 0x1fff);
        } else {
            entry.stuff_cnt = 0;
        }
        entry.Na        = Na;
        entry.Nv        = Nv;
    }

    entry.bfrm_offset = config_info->basic_frame_offset;
    entry.hfn_offset = config_info->hyper_frame_offset;
    entry.bfn_offset = config_info->radio_frame_offset;

    if (config_info->frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_HYPER) {
        entry.hfrm_sync = 1;
        entry.rfrm_sync = 0; /* don't care */

    } else if (config_info->frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_RADIO) {
        entry.hfrm_sync = 0;
        entry.rfrm_sync = 1;

    } else if (config_info->frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_BASIC) {
        entry.hfrm_sync = 0;
        entry.rfrm_sync = 0;

    } else if (config_info->frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_TUNNEL) {
        entry.hfrm_sync = 1;
        entry.rfrm_sync = 1;
    } else {
        /* ERROR Condition */
    }

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_container_entry_set(unit, port,
                                      dir,
                                      axc_id,
                                      &entry));
exit:
    SOC_FUNC_RETURN;

}

static
int _cprif_cpri_container_map_config_get(int unit, int port,
                                             cprimod_direction_t dir,
                                             uint8 axc_id,
                                             cprimod_cpri_container_config_t* config_info)
{
    cprif_cpri_container_map_entry_t entry;

    SOC_INIT_FUNC_DEFS;

    if (config_info == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info NULL parameter"));
    }
    if (SOC_E_NONE != cprimod_cpri_container_config_t_init( config_info)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info initialization failed"));
    }

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_container_entry_get(unit, port, dir, axc_id, &entry));

    if (entry.map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_1) {
        config_info->map_method = cprimod_cpri_frame_map_method_1;
    } else if (entry.map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_3){
        config_info->map_method = cprimod_cpri_frame_map_method_3;
    }else {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Unsupported Map Method Only Method 1 and 3 are supported"));
    }
    if (entry.map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_1) {
        config_info->cblk_cnt   = entry.cblk_cnt;
        config_info->Nst    = entry.stuff_cnt;
        config_info->Na     =  entry.Na;
        config_info->Nv     =  entry.Nv;
    } else if (entry.map_method == CPRIF_CONTAINER_MAP_CPRI_METHOD_3) {
        config_info->Naxc   = entry.naxc_cnt;
        config_info->Na     =  entry.Na;
        config_info->Nv     = entry.Nv;
        config_info->cblk_cnt      = entry.cblk_cnt;
        config_info->Nst           = 0;
    }

    config_info->basic_frame_offset = entry.bfrm_offset;
    config_info->hyper_frame_offset = entry.hfn_offset;
    config_info->radio_frame_offset = entry.bfn_offset;

    if (entry.hfrm_sync == 1) {
        config_info->frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_HYPER;
    } else {
        if(entry.rfrm_sync == 1){
            config_info->frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_RADIO;
        } else {
            config_info->frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_BASIC;
        }
    }

exit:
    SOC_FUNC_RETURN;

}

static
int _cprif_rsvd4_container_map_config_set(int unit, int port,
                                             cprimod_direction_t dir,
                                             uint8 flow_id,
                                             cprimod_rsvd4_container_config_t* config_info)
{
    cprif_rsvd4_container_map_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    cprif_rsvd4_container_map_entry_t_init( &entry);

    /* always this. */
    entry.map_method = CPRIF_CONTAINER_MAP_RSVD4;

    entry.axc_id = config_info->axc_id;

    /* For RSVD4, stuffing is always at the end. */
    entry.stuffing_at_end = CPRIF_CONTAINER_MAP_STUFFING_AT_END;
    entry.stuff_cnt = config_info->stuffing_cnt;

    entry.cblk_cnt = config_info->container_block_cnt;

    entry.rfrm_offset = config_info->message_number_offset;
    entry.bfn_offset = config_info->master_frame_offset;

    if ((config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_NO_SYNC)||
        (config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_NO_SYNC )) {
        entry.hfrm_sync = 1;
        entry.rfrm_sync = 0; /* don't care */
    } else if ((config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_MSG_OFFSET)||
               (config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_MSG_OFFSET)) {
        entry.hfrm_sync = 0;
        entry.rfrm_sync = 1;
    } else if ((config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_MSG_AND_MASTER_OFFSET)||
               (config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_MSG_AND_MASTER_OFFSET)) {
        entry.hfrm_sync = 0;
        entry.rfrm_sync = 0;
    }

    /*
     * if GSM mode and rx dirction and any one of msg_ts mode.
     */

    if (
        ((config_info->msg_ts_mode==CPRIMOD_RSVD4_MSG_TS_MODE_GSM_DL)||
         (config_info->msg_ts_mode==CPRIMOD_RSVD4_MSG_TS_MODE_GSM_UL)) &&
        (dir == CPRIMOD_DIR_RX) &&
        ((config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_NO_SYNC)||
         (config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_MSG_OFFSET)||
         (config_info->frame_sync_mode == CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_MSG_AND_MASTER_OFFSET))
      ) {
        entry.msg_ts_sync = 1;
    }

    /* Set the Prev TS Valid Bit.  */
    if ((dir == CPRIMOD_DIR_RX) &&
        ((config_info->msg_ts_mode==CPRIMOD_RSVD4_MSG_TS_MODE_GSM_DL)||
         (config_info->msg_ts_mode==CPRIMOD_RSVD4_MSG_TS_MODE_GSM_UL))){
        _SOC_IF_ERR_EXIT
            (cprif_drv_container_parser_map_ts_prev_valid_set(unit, port, flow_id, 1));
    }

    if (config_info->msg_ts_mode == CPRIMOD_RSVD4_MSG_TS_MODE_WCDMA) {
        entry.msg_ts_mode = CPRIF_CONTAINER_MAP_MSG_TS_MODE_WCDMA;
    } else if (config_info->msg_ts_mode == CPRIMOD_RSVD4_MSG_TS_MODE_LTE) {
        entry.msg_ts_mode = CPRIF_CONTAINER_MAP_MSG_TS_MODE_LTE;
    } else if (config_info->msg_ts_mode == CPRIMOD_RSVD4_MSG_TS_MODE_GSM_DL) {
        entry.msg_ts_mode = CPRIF_CONTAINER_MAP_MSG_TS_MODE_GSM_DL;
    }else if (config_info->msg_ts_mode == CPRIMOD_RSVD4_MSG_TS_MODE_GSM_UL) {
        entry.msg_ts_mode = CPRIF_CONTAINER_MAP_MSG_TS_MODE_GSM_UL;
    }

    entry.msg_ts_cnt = config_info->msg_ts_cnt;

    if (dir == CPRIMOD_DIR_TX) {
        entry.use_ts_dbm = config_info->use_ts_dbm;

        if (entry.use_ts_dbm) {
            entry.ts_dbm_prof_num = config_info->ts_dbm_prof_num;
            entry.num_active_slots = config_info->num_active_slots;
        }
        entry.msg_addr = config_info->msg_addr;
        entry.msg_type = config_info->msg_type;
        entry.msg_ts_offset = config_info->msg_ts_offset;
    }
    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_container_entry_set(unit, port,
                                                       dir,
                                                       flow_id,
                                                       &entry));
exit:
    SOC_FUNC_RETURN;

}

static
int _cprif_rsvd4_container_map_config_get(int unit, int port,
                                             cprimod_direction_t dir,
                                             uint8 flow_id,
                                             cprimod_rsvd4_container_config_t* config_info)
{
    cprif_rsvd4_container_map_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (config_info == NULL) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info NULL parameter"));
    }
    if (SOC_E_NONE != cprimod_rsvd4_container_config_t_init( config_info)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("config_info initialization failed"));
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_container_entry_get(unit, port,
                                             dir,
                                             flow_id,
                                             &entry));

    config_info->axc_id                 = entry.axc_id;
    config_info->stuffing_cnt           = entry.stuff_cnt;
    config_info->container_block_cnt    = entry.cblk_cnt;
    config_info->message_number_offset  = entry.rfrm_offset;
    config_info->master_frame_offset    = entry.bfn_offset;

    if (entry.hfrm_sync == 1) {
        if (entry.msg_ts_sync == 1){
            config_info->frame_sync_mode = CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_NO_SYNC;
        } else {
            config_info->frame_sync_mode = CPRIMOD_RSVD4_FRAME_SYNC_MODE_NO_SYNC;
        }
    } else {
        if (entry.rfrm_sync == 1) {
            if (entry.msg_ts_sync == 1){
                config_info->frame_sync_mode = CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_MSG_OFFSET;
            } else {
                config_info->frame_sync_mode = CPRIMOD_RSVD4_FRAME_SYNC_MODE_MSG_OFFSET;
            }
        } else {
            if (entry.msg_ts_sync == 1){
                config_info->frame_sync_mode = CPRIMOD_RSVD4_FRAME_SYNC_MODE_TS_MSG_AND_MASTER_OFFSET;
            } else {
                config_info->frame_sync_mode = CPRIMOD_RSVD4_FRAME_SYNC_MODE_MSG_AND_MASTER_OFFSET;
            }
        }
    }

    switch (entry.msg_ts_mode) {
        case CPRIF_CONTAINER_MAP_MSG_TS_MODE_WCDMA:
            config_info->msg_ts_mode = CPRIMOD_RSVD4_MSG_TS_MODE_WCDMA;
            break;

        case CPRIF_CONTAINER_MAP_MSG_TS_MODE_LTE:
            config_info->msg_ts_mode = CPRIMOD_RSVD4_MSG_TS_MODE_LTE;
            break;
        case CPRIF_CONTAINER_MAP_MSG_TS_MODE_GSM_DL:
            config_info->msg_ts_mode = CPRIMOD_RSVD4_MSG_TS_MODE_GSM_DL;
            break;

        case CPRIF_CONTAINER_MAP_MSG_TS_MODE_GSM_UL:
            config_info->msg_ts_mode = CPRIMOD_RSVD4_MSG_TS_MODE_GSM_UL;
            break;
        default:
            break;
    }

    config_info->msg_ts_cnt = entry.msg_ts_cnt;

    if (dir == CPRIMOD_DIR_TX) {
        config_info->use_ts_dbm         = entry.use_ts_dbm;
        config_info->ts_dbm_prof_num    = entry.ts_dbm_prof_num ;
        config_info->num_active_slots   = entry.num_active_slots;
        config_info->msg_addr           = entry.msg_addr;
        config_info->msg_type           = entry.msg_type;
        config_info->msg_ts_offset      = entry.msg_ts_offset;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_axc_container_config_set(int unit, int port, uint32 axc_id, cprimod_cpri_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_cpri_container_map_config_set(unit, port,
                                               CPRIMOD_DIR_RX,
                                               axc_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_rx_axc_container_config_get(int unit, int port, uint32 axc_id, cprimod_cpri_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    /* NOT IMPLEMENTED */
    _SOC_IF_ERR_EXIT
        (_cprif_cpri_container_map_config_get(unit, port,
                                               CPRIMOD_DIR_RX,
                                               axc_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_axc_container_config_clear(int unit, int port, uint32 axc_id)
{
    cprif_cpri_container_map_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }
    cprif_cpri_container_map_entry_t_init( &entry);
    entry.hfn_offset = 0xFF;
    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_container_entry_set(unit, port,
                                             CPRIMOD_DIR_RX,
                                             axc_id,
                                             &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_axc_container_config_set(int unit, int port, uint32 axc_id, cprimod_cpri_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_cpri_container_map_config_set(unit, port,
                                               CPRIMOD_DIR_TX,
                                               axc_id,
                                               config_info));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_axc_container_config_get(int unit, int port, uint32 axc_id, cprimod_cpri_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

   /* NOT IMPLEMENTED */
    _SOC_IF_ERR_EXIT
        (_cprif_cpri_container_map_config_get(unit, port,
                                               CPRIMOD_DIR_TX,
                                               axc_id,
                                               config_info));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_axc_container_config_clear(int unit, int port, uint32 axc_id)
{
    cprif_cpri_container_map_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }
    cprif_cpri_container_map_entry_t_init( &entry);
    entry.hfn_offset = 0xFF;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_container_entry_set(unit, port,
                                             CPRIMOD_DIR_TX,
                                             axc_id,
                                             &entry));
exit:
    SOC_FUNC_RETURN;

}


int cprif_rsvd4_rx_axc_container_config_set(int unit, int port, uint32 flow_id, cprimod_rsvd4_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_rsvd4_container_map_config_set(unit, port,
                                               CPRIMOD_DIR_RX,
                                               flow_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_rx_axc_container_config_get(int unit, int port, uint32 flow_id, cprimod_rsvd4_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_rsvd4_container_map_config_get(unit, port,
                                               CPRIMOD_DIR_RX,
                                               flow_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_rx_axc_container_config_clear(int unit, int port, uint32 flow_id)
{
    cprif_rsvd4_container_map_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }
    cprif_rsvd4_container_map_entry_t_init( &entry);
    entry.rfrm_offset = 0xFFFFFFFF;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_container_entry_set(unit, port,
                                             CPRIMOD_DIR_RX,
                                             flow_id,
                                             &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_axc_container_config_set(int unit, int port, uint32 flow_id, cprimod_rsvd4_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_rsvd4_container_map_config_set(unit, port,
                                               CPRIMOD_DIR_TX,
                                               flow_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_axc_container_config_get(int unit, int port, uint32 flow_id, cprimod_rsvd4_container_config_t* config_info)
{
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_rsvd4_container_map_config_get(unit, port,
                                               CPRIMOD_DIR_TX,
                                               flow_id,
                                               config_info));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_axc_container_config_clear(int unit, int port, uint32 flow_id)
{
    cprif_rsvd4_container_map_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          flow_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }
    cprif_rsvd4_container_map_entry_t_init( &entry);
    entry.rfrm_offset = 0xFFFFFFFF;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_container_entry_set(unit, port,
                                             CPRIMOD_DIR_TX,
                                             flow_id,
                                             &entry));
exit:
    SOC_FUNC_RETURN;

}

/*
 *
 *    IQ Packing/Unpacking
 */

int cprif_rx_roe_payload_size_set(int unit, int port, uint32 axc_id, cprimod_payload_size_info_t* config_info)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    cprif_iq_buffer_config_t_init( &entry);

    entry.payload_size      = config_info->packet_size;
    entry.last_packet_num   = config_info->last_packet_num;
    entry.last_payload_size = config_info->last_packet_size;

    _SOC_IF_ERR_EXIT (cprif_drv_iq_buffer_config_set(unit, port,
                                                 CPRIMOD_DIR_RX,
                                                 axc_id,
                                                 &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_roe_payload_size_get(int unit, int port, uint32 axc_id, cprimod_payload_size_info_t* config_info)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    cprif_iq_buffer_config_t_init( &entry);

    _SOC_IF_ERR_EXIT(cprif_drv_iq_buffer_config_get(unit, port,
                                                CPRIMOD_DIR_RX,
                                                axc_id,
                                                &entry));

    config_info->packet_size        = entry.payload_size;
    config_info->last_packet_num    = entry.last_packet_num;
    config_info->last_packet_size   = entry.last_payload_size;

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_roe_payload_size_set(int unit, int port, uint32 axc_id, cprimod_payload_size_info_t* config_info)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    cprif_iq_buffer_config_t_init( &entry);

    entry.payload_size      = config_info->packet_size;
    entry.last_packet_num   = config_info->last_packet_num;
    entry.last_payload_size = config_info->last_packet_size;

    _SOC_IF_ERR_EXIT (cprif_drv_iq_buffer_config_set(unit, port,
                                                 CPRIMOD_DIR_TX,
                                                 axc_id,
                                                 &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_roe_payload_size_get(int unit, int port, uint32 axc_id, cprimod_payload_size_info_t* config_info)
{
    cprif_iq_buffer_config_t entry;
    SOC_INIT_FUNC_DEFS;

    if (axc_id >= CPRIF_MAX_NUM_OF_AXC) {
        CPRIMOD_DEBUG_ERROR(("AxC ID  %d is out of range > than %d",
                          axc_id,
                          CPRIF_MAX_NUM_OF_AXC)) ;
        return SOC_E_PARAM;
    }

    cprif_iq_buffer_config_t_init( &entry);

    _SOC_IF_ERR_EXIT(cprif_drv_iq_buffer_config_get(unit, port,
                                                CPRIMOD_DIR_TX,
                                                axc_id,
                                                &entry));

    config_info->packet_size        = entry.payload_size;
    config_info->last_packet_num    = entry.last_packet_num;
    config_info->last_packet_size   = entry.last_payload_size;

exit:
    SOC_FUNC_RETURN;

}

/*
 *
 * ENCAP/DECAP  DATA Configuration.
 */
int cprif_cpri_encap_data_config_set(int unit, int port, uint32 queue_num, cprimod_encap_decap_data_config_t* config_info)
{
    cprif_encap_decap_data_entry_t entry;
    uint16 queue_offset=0;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_decap_data_entry_t_init( &entry);

    entry.valid = 1;
    entry.sample_size = config_info->sample_size;
    entry.out_sample_size = entry.sample_size;

    /* for 16 bits  to 15 bits, process before compression. */

    if (config_info->truncation_enable) {
        if ((config_info->truncation_type == cprimod_truncation_type_16_to_15)||
            (config_info->truncation_type == cprimod_truncation_type_add_1)) {
            if (entry.out_sample_size == 16) {
                entry.out_sample_size = 15;
                entry.sign_ext_enable = 1;
                if (config_info->truncation_type == cprimod_truncation_type_16_to_15) {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_16_TO_15;
                } else {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_ADD_1;
                }

            } else {
                LOG_CLI(("truncation can only apply to 16 bit sample.\n"));
            }
        }
    }

    if (config_info->compression_type == cprimod_decompress) {
        if (entry.out_sample_size == 9) {
            entry.out_sample_size = 15;
        } else {
            LOG_CLI(("decompression can only apply to 9 bits sample.\n"));
        }
    } else if (config_info->compression_type == cprimod_compress) {

        if (entry.out_sample_size == 15) {
                entry.out_sample_size = 9;
        } else {
                LOG_CLI(("Compression can only be done on 15 bit samples."));
        }
    }

    /* for 15 bits  to 16 bits, process after compression. */
    if (config_info->truncation_enable) {
        if ((config_info->truncation_type == cprimod_truncation_type_15_to_16)||
            (config_info->truncation_type == cprimod_truncation_type_add_0)) {
            if (entry.out_sample_size == 15) {
                entry.out_sample_size = 16;
                entry.sign_ext_enable = 1;
                if (config_info->truncation_type == cprimod_truncation_type_15_to_16) {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_15_TO_16;
                } else {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_ADD_0;
                }
            } else {
                LOG_CLI(("15 to 16 or add 0  truncation can only apply to 15 bit sample.\n"));
            }
        }
    }

    /*
     * Agnostic Mode do not need to allocate as there will only be one queue.
     * It will always start at 0.
     */

    if (config_info->is_agnostic) {
        queue_offset = 0;
    } else {
        _SOC_IF_ERR_EXIT
            (cprif_drv_encap_data_allocate_buffer(unit, port,
                                                  CPRIMOD_DIR_RX,
                                                  config_info->buffer_size,
                                                  &queue_offset));
    }

    entry.mux_enable        = config_info->mux_enable;
    entry.queue_offset      = queue_offset;
    /*
     * Convert the number of bytes into number of 16 bytes entries.
     */
    entry.queue_size        = CPRIF_NUM_OF_BUFFER_ENTRIES(config_info->buffer_size);
    entry.work_queue_select = config_info->priority;
    entry.bit_reversal      = config_info->bit_reversal;

    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_entry_set(unit, port,
                                                 CPRIMOD_DIR_RX,
                                                 queue_num,
                                                 &entry));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_encap_data_config_get(int unit, int port, uint32 queue_num, cprimod_encap_decap_data_config_t* config_info)
{
    cprif_encap_decap_data_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_entry_get(unit, port,
                                                 CPRIMOD_DIR_RX,
                                                 queue_num,
                                                 &entry));
    config_info->sample_size = entry.sample_size;
    config_info->truncation_enable = entry.sign_ext_enable;
    if (config_info->truncation_enable) {
        if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_15_TO_16) {
            config_info->truncation_type = cprimod_truncation_type_15_to_16;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_ADD_0) {
            config_info->truncation_type = cprimod_truncation_type_add_0;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_16_TO_15) {
            config_info->truncation_type = cprimod_truncation_type_16_to_15;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_ADD_1) {
            config_info->truncation_type = cprimod_truncation_type_add_1;
        }
    }

    if ((entry.out_sample_size == 9) && (entry.sample_size > 9)) {
        config_info->compression_type = cprimod_compress;
    } else if ((entry.out_sample_size >= 15) &&
       (entry.sample_size < 15)) {
        config_info->compression_type = cprimod_decompress;
    }

    config_info->mux_enable = entry.mux_enable;
    config_info->bit_reversal = entry.bit_reversal;
    /*
     * Convert number of 16 bytes entries into bytes.
     */
    config_info->buffer_size  = entry.queue_size * CPRIF_DATA_BUFFER_BLOCK_SIZE;
    config_info->priority     = entry.work_queue_select;

exit:
    SOC_FUNC_RETURN;

}


int cprif_cpri_decap_data_config_set(int unit, int port, uint32 queue_num, cprimod_encap_decap_data_config_t* config_info)
{
    cprif_encap_decap_data_entry_t entry;
    uint16 queue_offset =0;
    uint32 buffer_size_in_bytes;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_decap_data_entry_t_init( &entry);

    entry.valid             = 1;
    entry.sample_size       = config_info->sample_size;
    entry.out_sample_size   = entry.sample_size;

    /* for 16 bits  to 15 bits, process before compression. */

    if (config_info->truncation_enable) {
        if ((config_info->truncation_type == cprimod_truncation_type_16_to_15)||
            (config_info->truncation_type == cprimod_truncation_type_add_1)) {
            if (entry.out_sample_size == 16) {
                entry.out_sample_size = 15;
                entry.sign_ext_enable = 1;
                if (config_info->truncation_type == cprimod_truncation_type_16_to_15) {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_16_TO_15;
                } else {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_ADD_1;
                }

            } else {
                LOG_CLI(("truncation can only apply to 16 bit sample.\n"));
            }
        }
    }

    if (config_info->compression_type == cprimod_decompress) {
        if (entry.out_sample_size == 9) {
            entry.out_sample_size = 15;
        } else {
            LOG_CLI(("decompression can only apply to 9 bits sample.\n"));
        }
    } else if (config_info->compression_type == cprimod_compress) {

        if (entry.out_sample_size == 15) {
                entry.out_sample_size = 9;
        } else {
                LOG_CLI(("Compression can only be done on 15 bit samples."));
        }
    }

    /* for 15 bits  to 16 bits, process after compression. */
    if (config_info->truncation_enable) {
        if ((config_info->truncation_type == cprimod_truncation_type_15_to_16)||
            (config_info->truncation_type == cprimod_truncation_type_add_0)) {
            if (entry.out_sample_size == 15) {
                entry.out_sample_size = 16;
                entry.sign_ext_enable = 1;
                if (config_info->truncation_type == cprimod_truncation_type_15_to_16) {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_15_TO_16;
                } else {
                    entry.sign_ext_type = CPRIF_DATA_SIGN_EXT_ADD_0;
                }
            } else {
                LOG_CLI(("15 to 16 or add 0  truncation can only apply to 15 bit sample.\n"));
            }
        }
    }

    /*
     * Calculate space for the buffer.
     */

    buffer_size_in_bytes = CPRIF_NUM_OF_BUFFER_ENTRIES(config_info->buffer_size) * CPRIF_DATA_BUFFER_BLOCK_SIZE * config_info->cycle_size;

    /*
     * Agnostic Mode do not need to allocate as there will only be one queue.
     * It will always start at 0.
     */
    if (config_info->is_agnostic) {
        queue_offset = 0;
    } else {
        _SOC_IF_ERR_EXIT
            (cprif_drv_encap_data_allocate_buffer(unit, port,
                                                  CPRIMOD_DIR_TX,
                                                  buffer_size_in_bytes,
                                                  &queue_offset));
    }
    entry.mux_enable    = config_info->mux_enable;
    entry.queue_offset  = queue_offset; /* Memory management Here. */
    /*
     * Conver Buffer Size into number of 16 bits entries.
     */
    entry.queue_size    = CPRIF_NUM_OF_BUFFER_ENTRIES(config_info->buffer_size);
    entry.tx_cycle_size = config_info->cycle_size;
    entry.bit_reversal  = config_info->bit_reversal;

    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_entry_set(unit, port,
                                                 CPRIMOD_DIR_TX,
                                                 queue_num,
                                                 &entry));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_decap_data_config_get(int unit, int port, uint32 queue_num, cprimod_encap_decap_data_config_t* config_info)
{
    cprif_encap_decap_data_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_entry_get(unit, port,
                                                 CPRIMOD_DIR_TX,
                                                 queue_num,
                                                 &entry));
    config_info->sample_size = entry.sample_size;
    config_info->truncation_enable = entry.sign_ext_enable;
    if (config_info->truncation_enable) {
        if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_15_TO_16) {
            config_info->truncation_type = cprimod_truncation_type_15_to_16;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_ADD_0) {
            config_info->truncation_type = cprimod_truncation_type_add_0;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_16_TO_15) {
            config_info->truncation_type = cprimod_truncation_type_16_to_15;
        } else if (entry.sign_ext_type == CPRIF_DATA_SIGN_EXT_ADD_1) {
            config_info->truncation_type = cprimod_truncation_type_add_1;
        }
    }

    if ((entry.out_sample_size == 9) && (entry.sample_size > 9)) {
        config_info->compression_type = cprimod_compress;
    } else if ((entry.out_sample_size >= 15) &&
       (entry.sample_size < 15)) {
        config_info->compression_type = cprimod_decompress;
    }

    config_info->mux_enable     = entry.mux_enable;
    config_info->bit_reversal   = entry.bit_reversal;
    /*
     * Convert number of 16 bytes entries into bytes.
     */
    config_info->buffer_size    = entry.queue_size * CPRIF_DATA_BUFFER_BLOCK_SIZE ;
    config_info->cycle_size     = entry.tx_cycle_size;

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_encap_data_config_set(int unit, int port, uint32 queue_num, cprimod_rsvd4_encap_data_config_t* config_info)
{
    uint16 queue_offset=0;
    cprif_rsvd4_encap_decap_data_entry_t entry;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_rsvd4_encap_decap_data_entry_t_init( &entry);

    entry.valid                 = 1;
    entry.gsm_pad_size          = config_info->gsm_pad_size;
    entry.gsm_extra_pad_size    = config_info->gsm_extra_pad_size;
    entry.gsm_pad_enable        = config_info->gsm_pad_enable;
    entry.gsm_control_location  = config_info->gsm_control_location;
    entry.queue_size            = config_info->buffer_size;
    entry.work_queue_select     = config_info->priority;
    entry.bit_reversal          = config_info->bit_reversal;

    _SOC_IF_ERR_EXIT (cprif_drv_encap_data_allocate_buffer(unit, port,
                                                           CPRIMOD_DIR_RX,
                                                           config_info->buffer_size,
                                                           &queue_offset));
    entry.queue_offset          = queue_offset;

    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_encap_data_entry_set(unit, port,
                                                       CPRIMOD_DIR_RX,
                                                       queue_num,
                                                       &entry));

exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_encap_data_config_get(int unit, int port, uint32 queue_num, cprimod_rsvd4_encap_data_config_t* config_info)
{

    cprif_rsvd4_encap_decap_data_entry_t entry;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_rsvd4_encap_decap_data_entry_t_init( &entry);

    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_encap_data_entry_get(unit, port,
                                                       CPRIMOD_DIR_RX,
                                                       queue_num,
                                                       &entry));

    config_info->gsm_pad_size          = entry.gsm_pad_size;
    config_info->gsm_extra_pad_size    = entry.gsm_extra_pad_size;
    config_info->gsm_pad_enable        = entry.gsm_pad_enable;
    config_info->gsm_control_location  = entry.gsm_control_location;
    config_info->buffer_size           = entry.queue_size;
    config_info->priority              = entry.work_queue_select;
    config_info->bit_reversal          = entry.bit_reversal;


exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_decap_data_config_set(int unit, int port, uint32 queue_num, cprimod_rsvd4_decap_data_config_t* config_info)
{
    uint16 queue_offset =0;
    uint32 buffer_size_in_bytes;
    cprif_rsvd4_encap_decap_data_entry_t entry;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_rsvd4_encap_decap_data_entry_t_init( &entry);

    entry.valid                 = 1;
    entry.gsm_pad_size          = config_info->gsm_pad_size;
    entry.gsm_extra_pad_size    = config_info->gsm_extra_pad_size;
    entry.gsm_pad_enable        = config_info->gsm_pad_enable;
    entry.gsm_control_location  = config_info->gsm_control_location;
    entry.queue_size            = config_info->buffer_size;
    entry.tx_cycle_size         = config_info->cycle_size;
    entry.bit_reversal          = config_info->bit_reversal;

    buffer_size_in_bytes = CPRIF_NUM_OF_BUFFER_ENTRIES(config_info->buffer_size)
                         * CPRIF_DATA_BUFFER_BLOCK_SIZE
                         * config_info->cycle_size;
    _SOC_IF_ERR_EXIT(cprif_drv_encap_data_allocate_buffer(unit, port,
                                                          CPRIMOD_DIR_TX,
                                                          buffer_size_in_bytes,
                                                          &queue_offset));
    entry.queue_offset          = queue_offset;

    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_encap_data_entry_set(unit, port,
                                                       CPRIMOD_DIR_TX,
                                                       queue_num,
                                                       &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_decap_data_config_get(int unit, int port, uint32 queue_num, cprimod_rsvd4_decap_data_config_t* config_info)
{
    cprif_rsvd4_encap_decap_data_entry_t entry;

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_rsvd4_encap_decap_data_entry_t_init( &entry);

    _SOC_IF_ERR_EXIT(cprif_drv_rsvd4_encap_data_entry_get(unit, port,
                                                       CPRIMOD_DIR_TX,
                                                       queue_num,
                                                       &entry));

    config_info->gsm_pad_size          = entry.gsm_pad_size;
    config_info->gsm_extra_pad_size    = entry.gsm_extra_pad_size;
    config_info->gsm_pad_enable        = entry.gsm_pad_enable;
    config_info->gsm_control_location  = entry.gsm_control_location;
    config_info->buffer_size           = entry.queue_size;
    config_info->cycle_size            = entry.tx_cycle_size;
    config_info->bit_reversal          = entry.bit_reversal;


exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_header_config_set(int unit, int port, uint32 queue_num, cprimod_encap_header_config_t* config_info)
{
    cprif_encap_header_entry_t entry;
    uint32 pc_id;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    if (config_info->header_type == cprimod_hdr_encap_type_roe) {
        entry.header_type = CPRIF_ENCAP_HEADER_HDR_TYPE_ROE;
    } else if (config_info->header_type == cprimod_hdr_encap_type_encap_none) {
        entry.header_type = CPRIF_ENCAP_HEADER_HDR_TYPE_ENCAP_NONE;
    } else {
        /* not suppose to be here. */
    }

    if (config_info->roe_frame_format == CPRIMOD_CPRI_ROE_FRAME_FORMAT_IEEE1914) {
        entry.ver       = (config_info->roe_subtype & 0xc0) >> 6;
        entry.pkttype   = (config_info->roe_subtype & 0x3f);
        entry.flow_id   = config_info->flow_id;

    } else if (config_info->roe_frame_format == CPRIMOD_CPRI_ROE_FRAME_FORMAT_RSVD3) {
        entry.ver       = (config_info->version & 0x0c)>>2;
        entry.pkttype   = ((config_info->version & 0x3) << 4 ) | ((config_info->flow_id & 0xf00) >> 8);
        entry.flow_id   = config_info->flow_id & 0xFF;
    } else if (config_info->roe_frame_format == CPRIMOD_CPRI_ROE_FRAME_FORMAT_ECPRI8) {

        entry.roe_opcode = config_info->ecpri_msg_type;
        /* msb of pc_id */
        pc_id = ((config_info->ecpri_pc_id & 0xFF00) >> 8);
        entry.ver       = (pc_id & 0xc0) >> 6;
        entry.pkttype   = (pc_id & 0x3f);
        /* lsb of pc_id */
        entry.flow_id = config_info->ecpri_pc_id & 0x00FF;

    } else if (config_info->roe_frame_format == CPRIMOD_CPRI_ROE_FRAME_FORMAT_ECPRI12) {

        entry.roe_opcode = config_info->ecpri_msg_type;
        entry.ver       = (config_info->ecpri12_header_byte0 & 0xc0) >> 6;
        entry.pkttype   = (config_info->ecpri12_header_byte0 & 0x3f);
        /* only 8 bits of PCID is available in ECPRI12. */
        entry.flow_id = config_info->ecpri_pc_id & 0x00FF;

    }else {
        CPRIMOD_DEBUG_ERROR(("Unsupported RoE Frame Format %d",
                              config_info->roe_frame_format));
        return SOC_E_PARAM;
    }

    switch (config_info->vlan_type) {
        case cprimodHdrVlanTypeUntagged :
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_UNTAGGED;
            break;

        case cprimodHdrVlanTypeTaggedVlan0 :
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_0;
            break;

        case cprimodHdrVlanTypeQinQ :
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_Q_IN_Q;
            break;

        case cprimodHdrVlanTypeTaggedVlan1 :
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_1;
            break;
        default:
            entry.vlan_type = CPRIF_ENCAP_HEADER_VLAN_TYPE_UNTAGGED;
            break;
    }


    entry.ordering_info_index   = config_info->ordering_info_index;
    entry.mac_da_index          = config_info->mac_da_index;
    entry.mac_sa_index          = config_info->mac_sa_index;
    entry.vlan_id_0_index       = config_info->vlan_id_0_index;
    entry.vlan_id_1_index       = config_info->vlan_id_1_index;
    entry.vlan_0_priority       = config_info->vlan_0_priority;
    entry.vlan_1_priority       = config_info->vlan_1_priority;
    entry.ether_type_index      = config_info->vlan_eth_type_index;
    entry.use_tagid_for_flowid  = config_info->use_tagid_for_flowid;
    entry.use_tagid_for_vlan    = config_info->use_tagid_for_vlan;
    entry.use_opcode            = config_info->use_opcode;
    if ((config_info->roe_frame_format != CPRIMOD_CPRI_ROE_FRAME_FORMAT_ECPRI8)&&
        (config_info->roe_frame_format != CPRIMOD_CPRI_ROE_FRAME_FORMAT_ECPRI12)) {
        entry.roe_opcode            = config_info->roe_opcode;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_entry_set(unit, port,
                                          queue_num,
                                          CPRIF_ENCAP_HEADER_FLAGS_HDR_CONFIG_SET,
                                          &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_header_config_get(int unit, int port, uint32 queue_num, cprimod_encap_header_config_t* config_info)
{
    cprif_encap_header_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_header_entry_t_init(&entry);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_entry_get(unit, port,
                                          queue_num,
                                          &entry));

    if (entry.header_type == CPRIF_ENCAP_HEADER_HDR_TYPE_ROE) {
        config_info->header_type = cprimod_hdr_encap_type_roe;
    } else if (entry.header_type == CPRIF_ENCAP_HEADER_HDR_TYPE_ENCAP_NONE) {
        config_info->header_type = cprimod_hdr_encap_type_encap_none;
    }
    switch (entry.vlan_type) {
        case CPRIF_ENCAP_HEADER_VLAN_TYPE_UNTAGGED:
            config_info->vlan_type = cprimodHdrVlanTypeUntagged;
            break;

        case CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_0:
            config_info->vlan_type = cprimodHdrVlanTypeTaggedVlan0;
            break;

        case CPRIF_ENCAP_HEADER_VLAN_TYPE_Q_IN_Q:
            config_info->vlan_type = cprimodHdrVlanTypeQinQ;
            break;

        case CPRIF_ENCAP_HEADER_VLAN_TYPE_TAGGED_VLAN_1:
            config_info->vlan_type = cprimodHdrVlanTypeTaggedVlan1;
            break;
        default:
            config_info->vlan_type = cprimodHdrVlanTypeUntagged; /* ???? */
            break;
    }

    config_info->flow_id                = entry.flow_id;
    config_info->roe_subtype            = (entry.pkttype & 0x3f)| ((entry.ver & 0x3)<<6);
    config_info->ordering_info_index    = entry.ordering_info_index;
    config_info->mac_da_index           = entry.mac_da_index;
    config_info->mac_sa_index           = entry.mac_sa_index;
    config_info->vlan_id_0_index        = entry.vlan_id_0_index;
    config_info->vlan_id_1_index        = entry.vlan_id_1_index;
    config_info->vlan_0_priority        = entry.vlan_0_priority;
    config_info->vlan_1_priority        = entry.vlan_1_priority;
    config_info->vlan_eth_type_index    = entry.ether_type_index;
    config_info->use_tagid_for_flowid   = entry.use_tagid_for_flowid;
    config_info->use_tagid_for_vlan     = entry.use_tagid_for_vlan;
    config_info->use_opcode             = entry.use_opcode;
    config_info->roe_opcode             = entry.roe_opcode;
    config_info->ecpri12_header_byte0   = (entry.pkttype & 0x3f)| ((entry.ver & 0x3)<<6);

exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_rsvd4_gsm_tsn_bitmap_set(int unit, int port, uint32 queue_num, uint32 tsn_bitmap)
{
    cprif_encap_header_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_header_entry_t_init( &entry);

    entry.tsn_bitmap = tsn_bitmap;

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_entry_set(unit, port,
                                          queue_num,
                                          CPRIF_ENCAP_HEADER_FLAGS_GSM_CONFIG_SET,
                                          &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_rsvd4_gsm_tsn_bitmap_get(int unit, int port, uint8 queue_num, uint8* tsn_bitmap)
{
    cprif_encap_header_entry_t entry;
    SOC_INIT_FUNC_DEFS;


    cprif_encap_header_entry_t_init(&entry);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_entry_get(unit, port,
                                          queue_num,
                                          &entry));
    *tsn_bitmap = entry.tsn_bitmap;
exit:
    SOC_FUNC_RETURN;

}

static
int _cprif_sequence_number_size_validate(uint8 p_size, uint8 q_size)
{

    if ((p_size < 1) || (p_size > 32)) {
        LOG_CLI(("P size need to be between 1 and 32.\n"));
        return SOC_E_PARAM;
    }

    if ((q_size+p_size)>32) {
        LOG_CLI(("P and Q cannot exceed 32.\n"));
        return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}


int cprif_encap_ordering_info_entry_set(int unit, int port,
                                        uint32 index,
                                        const cprimod_encap_ordering_info_entry_t* user_entry)
{
    cprif_encap_ordering_info_entry_t entry;
    uint32  p_mask;
    uint32  q_mask;
    uint32  r_mask;
    uint8     r_bits;
    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          index,
                          CPRIF_ORDERING_INFO_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_ordering_info_entry_t_init( &entry);

    cprif_drv_cprimod_to_cprif_ordering_info_type(user_entry->type, &entry.type);
    cprif_drv_cprimod_to_cprif_ordering_info_inc_prop(user_entry->pcnt_prop, &entry.pcnt_prop);
    cprif_drv_cprimod_to_cprif_ordering_info_inc_prop(user_entry->qcnt_prop, &entry.qcnt_prop);

    entry.p_size = user_entry->pcnt_size;
    entry.q_size = user_entry->qcnt_size;

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(entry.p_size, entry.q_size));

    p_mask = ((0x00000001<<entry.p_size)-1);
    q_mask = ((0x00000001 << entry.q_size)-1);
    r_bits =  32 - entry.p_size - entry.q_size;
    r_mask = ((0x00000001 << r_bits)-1);

    entry.max = 0x00;
    entry.max |= (user_entry->pcnt_max & p_mask);
    entry.max |= ((user_entry->qcnt_max & q_mask) << entry.p_size);

    entry.increment = 0x00;
    entry.increment |= (user_entry->pcnt_increment & p_mask);
    entry.increment |= ((user_entry->qcnt_increment & q_mask) << entry.p_size);
    entry.increment |= ((user_entry->seq_reserve_value & r_mask) << (entry.p_size+entry.q_size));

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_ordering_info_entry_set(unit, port,
                                                 index,
                                                 &entry));


exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_ordering_info_entry_get(int unit, int port, uint32 index, cprimod_encap_ordering_info_entry_t* user_entry)
{
    cprif_encap_ordering_info_entry_t entry;
    uint32 p_mask;
    uint32 q_mask;
    uint32 r_mask;
    uint8  r_bits;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          index,
                          CPRIF_ORDERING_INFO_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    cprif_encap_ordering_info_entry_t_init( &entry);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_ordering_info_entry_get(unit, port,
                                                 index,
                                                 &entry));

    cprif_drv_cprif_to_cprimod_ordering_info_type(entry.type, &user_entry->type);
    cprif_drv_cprif_to_cprimod_ordering_info_inc_prop(entry.pcnt_prop,&user_entry->pcnt_prop);
    cprif_drv_cprif_to_cprimod_ordering_info_inc_prop(entry.qcnt_prop,&user_entry->qcnt_prop);

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(entry.p_size, entry.q_size));

    user_entry->pcnt_size = entry.p_size;
    user_entry->qcnt_size = entry.q_size;

    p_mask = (0x00000001 << entry.p_size)-1;
    q_mask = (0x00000001 << entry.q_size)-1;
    r_bits =  32 - entry.p_size - entry.q_size;
    r_mask = ((0x00000001 << r_bits)-1);


    user_entry->pcnt_max = entry.max & p_mask;
    user_entry->qcnt_max = ((entry.max >> entry.p_size) & q_mask);

    user_entry->pcnt_increment      = entry.increment & p_mask;
    user_entry->qcnt_increment      = ((entry.increment >> entry.p_size) & q_mask);
    user_entry->seq_reserve_value   = ((entry.increment >> (entry.p_size+entry.q_size)) & r_mask);

exit:
    SOC_FUNC_RETURN;

}



int cprif_decap_ordering_info_entry_set(int unit, int port, uint32 index,
                                        const cprimod_decap_ordering_info_entry_t* user_entry)
{
    cprif_decap_ordering_info_entry_t entry;
    uint32  p_mask;
    uint32  q_mask;

    SOC_INIT_FUNC_DEFS;
    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          index,
                          CPRIF_ORDERING_INFO_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    cprif_decap_ordering_info_entry_t_init( &entry);

    cprif_drv_cprimod_to_cprif_ordering_info_type(user_entry->type, &entry.type);

    entry.p_size = user_entry->pcnt_size;
    entry.q_size = user_entry->qcnt_size;

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(entry.p_size, entry.q_size));

        p_mask = ((0x00000001 << entry.p_size)-1);
    q_mask = ((0x00000001 << entry.q_size)-1);

    entry.max           = 0x00;
    entry.max          |= (user_entry->pcnt_max & p_mask);
    entry.max          |= ((user_entry->qcnt_max & q_mask) << entry.p_size);

    entry.bias          = 0x00;
    entry.bias         |= (user_entry->pcnt_bias & p_mask);
    entry.bias         |= ((user_entry->qcnt_bias & q_mask) << entry.p_size);

    entry.increment     = 0x00;
    entry.increment    |= (user_entry->pcnt_increment & p_mask);
    entry.increment    |= ((user_entry->qcnt_increment & q_mask) << entry.p_size);

    entry.pcnt_inc_p2       = user_entry->pcnt_increment_p2;
    entry.qcnt_inc_p2       = user_entry->qcnt_increment_p2;
    entry.pcnt_extended     = user_entry->pcnt_extended;
    entry.pcnt_pkt_count    = user_entry->pcnt_pkt_count;
    entry.modulo_2          = user_entry->modulo_2;

    entry.gsm_tsn_bitmap    = user_entry->gsm_tsn_bitmap;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_ordering_info_entry_set(unit, port,
                                                 index,
                                                 &entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_ordering_info_entry_get(int unit, int port, uint32 index,
                                        cprimod_decap_ordering_info_entry_t* user_entry)
{
    cprif_decap_ordering_info_entry_t entry;
    uint32  p_mask;
    uint32  q_mask;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          index,
                          CPRIF_ORDERING_INFO_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    cprif_decap_ordering_info_entry_t_init( &entry);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_ordering_info_entry_get(unit, port,
                                                 index,
                                                 &entry));
    cprif_drv_cprif_to_cprimod_ordering_info_type(entry.type, &user_entry->type);

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(entry.p_size, entry.q_size));

    user_entry->pcnt_increment_p2   = entry.pcnt_inc_p2;
    user_entry->qcnt_increment_p2   = entry.qcnt_inc_p2;
    user_entry->pcnt_extended       = entry.pcnt_extended;
    user_entry->pcnt_pkt_count      = entry.pcnt_pkt_count;
    user_entry->modulo_2            = entry.modulo_2;
    user_entry->gsm_tsn_bitmap       = entry.gsm_tsn_bitmap;

    user_entry->pcnt_size           = entry.p_size;
    user_entry->qcnt_size           = entry.q_size;

    p_mask = ((0x00000001 << entry.p_size)-1);
    q_mask = ((0x00000001 << entry.q_size)-1);


    user_entry->pcnt_max = (entry.max & p_mask);
    user_entry->qcnt_max = ((entry.max >> entry.p_size) & q_mask);

    user_entry->pcnt_bias = (entry.bias & p_mask);
    user_entry->qcnt_bias = ((entry.bias >> entry.p_size) & q_mask);

    user_entry->pcnt_increment = (entry.increment & p_mask);
    user_entry->qcnt_increment = ((entry.increment>>entry.p_size) & q_mask);

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_ordering_info_sequence_offset_set(int unit, int port, uint32 queue,
                                                  uint32 control,
                                                  cprimod_cpri_ordering_info_offset_t* offset_entry)
{
    uint32 offset;
    uint32 p_mask;
    uint32 q_mask;

    SOC_INIT_FUNC_DEFS;

    if (queue >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }


    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(offset_entry->p_size, offset_entry->q_size));

    /* p_cnt only valid from 1-32 */

    p_mask = 0xFFFFFFFF;
    /* coverity[large_shift:FALSE] */
    if (offset_entry->p_size < 32) {
        p_mask = ((0x00000001 << offset_entry->p_size)-1);
    }

    q_mask = ((0x00000001 << offset_entry->q_size)-1);

    offset  = 0;
    offset |= (offset_entry->p_offset & p_mask);
    /* coverity[large_shift:FALSE] */
    offset |= ((offset_entry->q_offset & q_mask) << offset_entry->p_size);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_queue_ordering_info_sequence_offset_set (unit, port,
                                                                  queue, 
                                                                  offset));
exit:
    SOC_FUNC_RETURN;

}

/* caller need to provide the p_size and q_size. */
int cprif_encap_ordering_info_sequence_offset_get(int unit, int port, uint32 queue,
                                                  uint8 control,
                                                  uint32 pcnt_size, uint32 qcnt_size,
                                                  cprimod_cpri_ordering_info_offset_t* offset_entry)
{
    uint32 offset;
    uint32 p_mask;
    uint32 q_mask;

    SOC_INIT_FUNC_DEFS;

    if (queue >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    offset_entry->p_size = pcnt_size;
    offset_entry->q_size = qcnt_size;

    _SOC_IF_ERR_EXIT(_cprif_sequence_number_size_validate(offset_entry->p_size,
                                                     offset_entry->q_size));

    p_mask = 0xFFFFFFFF;
    /* coverity[large_shift:FALSE] */
    if (offset_entry->p_size < 32) {
        p_mask = ((0x00000001 << offset_entry->p_size)-1);
    }
    q_mask = ((0x00000001 << offset_entry->q_size)-1);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_queue_ordering_info_sequence_offset_get (unit, port,
                                                                  queue, control,
                                                                  &offset));
    offset_entry->p_offset = offset & p_mask;
    /* coverity[large_shift:FALSE] */
    offset_entry->q_offset = ((offset >> offset_entry->p_size) & q_mask);
exit:
    SOC_FUNC_RETURN;

}


int cprif_decap_ordering_info_sequence_offset_set(int unit, int port, uint32 queue,
                                                  cprimod_cpri_ordering_info_offset_t* offset_entry)
{
    uint32 offset;
    uint32 p_mask;
    uint32 q_mask;
    SOC_INIT_FUNC_DEFS;

    if (queue >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (_cprif_sequence_number_size_validate(offset_entry->p_size, offset_entry->q_size));

    p_mask = 0xFFFFFFFF;
    /* coverity [large_shift:FALSE] */
    if (offset_entry->p_size < 32) {
        p_mask = ((0x00000001 << offset_entry->p_size)-1);
    }

    q_mask = ((0x00000001 << offset_entry->q_size)-1);

    offset  = 0;
    offset |= (offset_entry->p_offset & p_mask);
    /* coverity[large_shift:FALSE] */
    offset |= ((offset_entry->q_offset & q_mask) << offset_entry->p_size);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_ordering_info_sequence_offset_set (unit, port,
                                                                  queue,
                                                                  offset));


exit:
    SOC_FUNC_RETURN;

}

/* caller need to provide the p_size and q_size. */
int cprif_decap_ordering_info_sequence_offset_get(int unit, int port, uint32 queue,
                                                    uint32 pcnt_size, uint32 qcnt_size,
                                                    cprimod_cpri_ordering_info_offset_t* offset_entry)
{
    uint32 offset;
    uint32 p_mask;
    uint32 q_mask;

    SOC_INIT_FUNC_DEFS;

    if (queue >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    offset_entry->p_offset = 0;
    offset_entry->q_offset = 0;
    offset_entry->p_size = pcnt_size;
    offset_entry->q_size = qcnt_size;

    _SOC_IF_ERR_EXIT(_cprif_sequence_number_size_validate(
                      offset_entry->p_size, offset_entry->q_size));

    p_mask = 0xFFFFFFFF;
    if (offset_entry->p_size < 32) {
        p_mask = ((0x00000001 << offset_entry->p_size)-1);
    }
    q_mask = ((0x00000001 << offset_entry->q_size)-1);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_ordering_info_sequence_offset_get (unit, port,
                                                                  queue,
                                                                  &offset));
    offset_entry->p_offset = offset & p_mask;
    if (offset_entry->p_size < 32) {
        offset_entry->q_offset = ((offset >> offset_entry->p_size) & q_mask);
    } 

exit:
    SOC_FUNC_RETURN;

}


static
int _cprif_pack_mac_addr(cprimod_mac_addr_t addr, uint64 *mac_addr)
{
    int loop;
    uint64 tmp_addr;
    uint8 shift_bits;

    /* packing addr bytes */
    COMPILER_64_ZERO(*mac_addr);
    shift_bits = 0;
    for(loop = 0; loop < CPRIF_NUM_BYTES_IN_MAC_ADDR; loop++) {

        /*
         * Change shift bit formula when the packing byte order
         * need to be reverse. (loop * 8). Change both pack and unpack.
         */
        shift_bits = (CPRIF_NUM_BYTES_IN_MAC_ADDR - 1 - loop)*8;
        COMPILER_64_SET(tmp_addr, 0, (uint32) addr[loop]);
        COMPILER_64_SHL(tmp_addr, shift_bits);
        COMPILER_64_OR(*mac_addr, tmp_addr);
    }

    return SOC_E_NONE;
}

int _cprif_unpack_mac_addr(uint64 mac_addr, cprimod_mac_addr_t addr)
{
    int loop;
    uint64 tmp_addr;
    uint8 shift_bits;

    /* unpacking addr bytes */
    COMPILER_64_ZERO(tmp_addr);
    for(loop = 0; loop < CPRIF_NUM_BYTES_IN_MAC_ADDR; loop++) {

        shift_bits = (CPRIF_NUM_BYTES_IN_MAC_ADDR - 1 - loop)*8;
        COMPILER_64_COPY(tmp_addr, mac_addr);
        COMPILER_64_SHR(tmp_addr, shift_bits);
        addr[loop] = COMPILER_64_LO(tmp_addr) & 0xff;
    }
    return SOC_E_NONE;
}

int cprif_encap_mac_da_entry_set(int unit, int port, uint32 index, cprimod_mac_addr_t addr)
{
    uint64 mac_addr;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_MAC_ADDR_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                              index,
                              CPRIF_MAC_ADDR_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    COMPILER_64_ZERO(mac_addr);

    /* packing addr bytes */
    _cprif_pack_mac_addr(addr,&mac_addr);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_mac_da_set(unit, port,
                                    index,
                                    mac_addr));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_mac_da_entry_get(int unit, int port, uint32 index, cprimod_mac_addr_t addr)
{
    uint64 mac_addr;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_MAC_ADDR_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                              index,
                              CPRIF_MAC_ADDR_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    COMPILER_64_ZERO(mac_addr);
    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_mac_da_get(unit, port,
                                    index,
                                    &mac_addr));

    _cprif_unpack_mac_addr(mac_addr, addr);

exit:
    SOC_FUNC_RETURN;

}


int cprif_encap_mac_sa_entry_set(int unit, int port, uint32 index, cprimod_mac_addr_t addr)
{
    uint64 mac_addr;
    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_MAC_ADDR_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                              index,
                              CPRIF_MAC_ADDR_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    COMPILER_64_ZERO(mac_addr);

    /* packing addr bytes */
    _cprif_pack_mac_addr(addr,&mac_addr);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_mac_sa_set(unit, port,
                                    index,
                                    mac_addr));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_mac_sa_entry_get(int unit, int port, uint32 index, cprimod_mac_addr_t addr)
{
    uint64 mac_addr;

    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_MAC_ADDR_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                              index,
                              CPRIF_MAC_ADDR_TABLE_SIZE)) ;
        return SOC_E_PARAM;
    }

    COMPILER_64_ZERO(mac_addr);
    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_mac_sa_get(unit, port,
                                    index,
                                    &mac_addr));

    _cprif_unpack_mac_addr(mac_addr, addr);

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_vlan_id_entry_set(int unit, int port, cprimod_vlan_table_id_t table_id, uint32 index, uint32 vlan_id)
{
    uint8   table_num;
    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_VLAN_ID_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                          index,
                          CPRIF_VLAN_ID_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    if (table_id == cprimodVlanTable0) {
        table_num = 0;
    } else {
        table_num = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_vlan_entry_set(unit, port,
                                        table_num,
                                        index,
                                        vlan_id));



exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_vlan_id_entry_get(int unit, int port, cprimod_vlan_table_id_t table_id, uint32 index, uint32* vlan_id)
{
    uint8   table_num;
    SOC_INIT_FUNC_DEFS;

    if (index >=  CPRIF_VLAN_ID_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Index %d is out of range > than %d",
                          index,
                          CPRIF_VLAN_ID_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    if (table_id == cprimodVlanTable0) {
        table_num = 0;
    } else {
        table_num = 1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_vlan_entry_get(unit, port,
                                        table_num,
                                        index,
                                        vlan_id));
exit:
    SOC_FUNC_RETURN;

}


static
void _cprif_cprimod_to_cprif_decap_ethtype_id(cprimod_ethertype_t type_id, uint8 *int_type)
{
    switch (type_id) {
        case cprimodEthertypeRoe:
            *int_type = CPRIF_DECAP_ROE_ETHTYPE;
            break;

        case cprimodEthertypeFast:
            *int_type = CPRIF_DECAP_FAST_ETH_ETHTYPE;
            break;

        case cprimodEthertypeVlan:
            *int_type = CPRIF_DECAP_VLAN_TAGGED_ETHTYPE;
            break;

        case cprimodEthertypeQinQ:
            *int_type = CPRIF_DECAP_VLAN_QINQ_ETHTYPE;
            break;

        default:
            *int_type = CPRIF_DECAP_ROE_ETHTYPE;
            break;

    }
}

int cprif_decap_ethertype_config_set(int unit, int port, cprimod_ethertype_t type_id, uint16 ethertype)
{
    uint8 int_type;
    SOC_INIT_FUNC_DEFS;

    _cprif_cprimod_to_cprif_decap_ethtype_id(type_id, &int_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_vlan_ethtype_set(unit, port,
                                          int_type,
                                          ethertype));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_ethertype_config_get(int unit, int port, cprimod_ethertype_t type_id, uint16* ethertype)
{
    uint8 int_type;
    SOC_INIT_FUNC_DEFS;

    _cprif_cprimod_to_cprif_decap_ethtype_id(type_id, &int_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_vlan_ethtype_get(unit, port,
                                          int_type,
                                          ethertype));

exit:
    SOC_FUNC_RETURN;

}

static
void _cprif_cprimod_to_cprif_encap_ethtype_id(cprimod_ethertype_t type_id, uint8 *int_type)
{
    switch (type_id) {
        case cprimodEthertypeRoe:
            *int_type = CPRIF_ETHTYPE_ROE_ETHTYPE_0;
            break;

        case cprimodEthertypeRoe1:
            *int_type = CPRIF_ETHTYPE_ROE_ETHTYPE_1;
            break;

        case cprimodEthertypeVlan:
            *int_type = CPRIF_ETHTYPE_VLAN_TAGGED;
            break;

        case cprimodEthertypeQinQ:
            *int_type = CPRIF_ETHTYPE_VLAN_QINQ;
            break;

        default:
            *int_type = CPRIF_ETHTYPE_ROE_ETHTYPE_0;
            break;

    }
}


int cprif_encap_ethertype_config_set(int unit, int port, cprimod_ethertype_t type_id, uint16 ethertype)
{
    uint8 int_type;
    SOC_INIT_FUNC_DEFS;

    _cprif_cprimod_to_cprif_encap_ethtype_id(type_id, &int_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_vlan_ethtype_set(unit, port,
                                          int_type,
                                          ethertype));

exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_ethertype_config_get(int unit, int port, cprimod_ethertype_t type_id, uint16* ethertype)
{
    uint8 int_type;
    SOC_INIT_FUNC_DEFS;

    _cprif_cprimod_to_cprif_encap_ethtype_id(type_id, &int_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_vlan_ethtype_get(unit, port,
                                          int_type,
                                          ethertype));
exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_memory_clear(int unit, int port, int queue_num)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprif_drv_encap_queue_sts_clear(unit, port,
                     queue_num));
    _SOC_IF_ERR_EXIT(cprif_drv_encap_ordering_info_sts_clear(unit, port,
                     queue_num));
exit:
    SOC_FUNC_RETURN;
}


int cprif_cpri_rx_framer_state_clear(int unit, int port, int queue_num)
{
    SOC_INIT_FUNC_DEFS;
    /* only for data framer state queue_num <64 for data axc*/
    if (queue_num < 64) {
        _SOC_IF_ERR_EXIT(cprif_drv_container_parser_mapp_state_tab_clear(unit,
                                                               port, queue_num));
        _SOC_IF_ERR_EXIT(cprif_drv_iq_pak_buff_state_tab_clear(unit, port,
                                                               queue_num));
        _SOC_IF_ERR_EXIT(cprif_drv_iq_pak_buff_payld_tab_clear(unit, port,
                                                               queue_num));
    }
exit:
    SOC_FUNC_RETURN;

}

/*
 * Before clearing queue from decap the flows should be cleared. This will
 * be done at higher level. Keeping the flow and removing the queue may result
 * in unpredictable behavior
 */

int cprif_decap_memory_clear(int unit, int port, int queue_num)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_decap_queue_state_tbl_clear(unit, port,
                     queue_num));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_framer_state_clear(int unit, int port, int queue_num)
{
    SOC_INIT_FUNC_DEFS;
    /* only for data framer state queue_num <64 for data axc*/
    if (queue_num < 64) {
        _SOC_IF_ERR_EXIT(cprif_drv_iq_unpsk_buff_state_tbl_clear(unit, port,
                         queue_num));
        _SOC_IF_ERR_EXIT(cprif_drv_cont_assembly_map_state_tbl_clear(unit, port,
                         queue_num));
    }
exit:
    SOC_FUNC_RETURN;

}

#define CPRIF_DECAP_CLS_OPTION_USE_QUEUE_NUM            0
#define CPRIF_DECAP_CLS_OPTION_USE_FLOW_ID_TO_QUEUE     1
#define CPRIF_DECAP_CLS_OPTION_USE_USE_OPCODE_QUEUE     2

int cprif_decap_flow_classification_config_set(int unit, int port,
                                               uint8 subtype,
                                               uint32 queue_num,
                                               cprimod_cls_option_t cls_option,
                                               cprimod_cls_flow_type_t flow_type)
{
    uint32 option=0;
    uint32 type=0;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    switch (cls_option) {
        case cprimodClsOptionUseQueueNum:
            option = CPRIF_DECAP_CLS_OPTION_USE_QUEUE_NUM;
            break;

        case cprimodClsOptionUseFlowIdToQueue:
            option = CPRIF_DECAP_CLS_OPTION_USE_FLOW_ID_TO_QUEUE;
            break;

        case cprimodClsOptionUseOpcodeToQueue:
            option = CPRIF_DECAP_CLS_OPTION_USE_USE_OPCODE_QUEUE;
            break;

        default:
            break;
    }

    switch (flow_type) {
        case cprimodClsFlowTypeData:
            type = CPRIF_DECAP_CLS_FLOW_TYPE_DATA;
            break;

        case cprimodClsFlowTypeDataWithExt: /* Not Suppoprted */
            type = CPRIF_DECAP_CLS_FLOW_TYPE_DATA_WITH_EXT;
            break;

        case cprimodClsFlowTypeCtrlWithOpcode:
            type = CPRIF_DECAP_CLS_FLOW_TYPE_CTRL_WITH_OPCODE;
            break;

        case cprimodClsFlowTypeCtrl:
            type = CPRIF_DECAP_CLS_FLOW_TYPE_CTRL;
            break;
        default:
            break;
    }


    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_classification_entry_set (unit, port,
                                                        subtype,
                                                        queue_num,
                                                        option,
                                                        type));


exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_flow_classification_config_get(int unit, int port,
                                               uint8 subtype,
                                               uint32* queue_num,
                                               cprimod_cls_option_t* cls_option,
                                               cprimod_cls_flow_type_t* flow_type)
{
    uint32 option;
    uint32 type;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_classification_entry_get(unit, port,
                                                       subtype,
                                                       queue_num,
                                                       &option,
                                                       &type));
    switch (option) {
        case CPRIF_DECAP_CLS_OPTION_USE_QUEUE_NUM:
            *cls_option = cprimodClsOptionUseQueueNum;
            break;

        case CPRIF_DECAP_CLS_OPTION_USE_FLOW_ID_TO_QUEUE:
            *cls_option = cprimodClsOptionUseFlowIdToQueue;
            break;

        case CPRIF_DECAP_CLS_OPTION_USE_USE_OPCODE_QUEUE:
            *cls_option = cprimodClsOptionUseOpcodeToQueue;
            break;

        default:
            break;
    }

    switch (type) {
        case CPRIF_DECAP_CLS_FLOW_TYPE_DATA:
            *flow_type = cprimodClsFlowTypeData;
            break;

        case CPRIF_DECAP_CLS_FLOW_TYPE_DATA_WITH_EXT: /* Not Suppoprted */
            *flow_type = cprimodClsFlowTypeDataWithExt;
            break;

        case CPRIF_DECAP_CLS_FLOW_TYPE_CTRL_WITH_OPCODE:
            *flow_type = cprimodClsFlowTypeCtrlWithOpcode;
            break;

        case CPRIF_DECAP_CLS_FLOW_TYPE_CTRL:
            *flow_type = cprimodClsFlowTypeCtrl;
            break;
        default:
            break;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_flow_classification_config_clear(int unit, int port,
                                                 uint8 subtype)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_classification_entry_set (unit, port,
                                                        subtype,
                                                        CPRIF_FLOW_CLS_INVALID_QUEUE,
                                                        CPRIF_DECAP_CLS_OPTION_USE_QUEUE_NUM,
                                                        CPRIF_DECAP_CLS_FLOW_TYPE_DATA));
exit:
    SOC_FUNC_RETURN;

}
int cprif_decap_flow_to_queue_mapping_set(int unit, int port, uint32 flow_id, uint32 queue_num)
{
    uint8 queue;
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    queue = queue_num;
    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_to_queue_map_set(unit, port,
                                               flow_id,
                                               queue));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_flow_to_queue_mapping_get(int unit, int port, uint32 flow_id, uint32* queue_num)
{

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_to_queue_map_get(unit, port,
                                               flow_id,
                                               queue_num));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_flow_to_queue_mapping_clear(int unit, int port, uint32 flow_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_flow_to_queue_map_set(unit, port,
                                               flow_id,
                                               CPRIF_INVALID_QUEUE));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_queue_to_ordering_info_index_mapping_set(int unit, int port, uint32 queue_num, uint32 ordering_info_index)
{
    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }

    if (ordering_info_index >=  CPRIF_ORDERING_INFO_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Decap Ordering info only have %d entries, %d is out of range",
                           CPRIF_ORDERING_INFO_TABLE_SIZE,
                           ordering_info_index));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_to_ordering_info_index_set(unit, port,
                                                          queue_num,
                                                          ordering_info_index));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_queue_to_ordering_info_index_mapping_get(int unit, int port, uint32 queue_num, uint32* ordering_info_index)
{

    SOC_INIT_FUNC_DEFS;

    if (queue_num >=  CPRIF_MAX_NUM_OF_QUEUES) {
        CPRIMOD_DEBUG_ERROR(("Queue %d is out of range > than %d",
                          queue_num,
                          CPRIF_MAX_NUM_OF_QUEUES)) ;
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_to_ordering_info_index_get(unit, port,
                                                          queue_num,
                                                          ordering_info_index));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_tick_set(int unit, int port, uint32 divider, uint32 basic_frame_tick, uint32 tick_bitmap, uint32 tick_bitmap_size)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_tick_set(unit, port,
                                           divider,
                                           basic_frame_tick,
                                           tick_bitmap,
                                           tick_bitmap_size));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_tick_get(int unit, int port, uint32* divider, uint32* basic_frame_tick, uint32* tick_bitmap, uint32* tick_bitmap_size)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_tick_get(unit, port,
                                           divider,
                                           basic_frame_tick,
                                           tick_bitmap,
                                           tick_bitmap_size));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_framer_tgen_tick_config_set(int unit, int port, cprimod_port_speed_t speed)
{
    uint32 divider = 1;
    uint32 ticks_bitmap = 0;
    uint32 ticks_bitmap_size = 0;
    uint32 basic_frame_ticks = 0;
    SOC_INIT_FUNC_DEFS;

    divider = 1;

    switch (speed) {
        case cprimodSpd614p4:
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 64;
            break;
        case cprimodSpd1228p8:
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 64;
            break;
        case cprimodSpd2457p6:
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 64;
            break;
        case cprimodSpd3072p0:
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 80;
            break;
        case cprimodSpd4915p2:
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 32;
            break;
        case cprimodSpd6144p0:
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 40;
            break;
        case cprimodSpd9830p4:
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 64;
            break;
        case cprimodSpd10137p6:
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 66;
            break;
        case cprimodSpd12165p12:
            ticks_bitmap        = 0xfff0;
            ticks_bitmap_size   = 4;
            basic_frame_ticks   = 79;
            break;
        case cprimodSpd24330p24:
            ticks_bitmap        = 0xfff8;
            ticks_bitmap_size   = 4;
            basic_frame_ticks   = 158;
            break;
        default:
            _SOC_EXIT_WITH_ERR
                (SOC_E_PARAM, ("Speed is not supported for cpri"));
            break;
    }
    _SOC_IF_ERR_EXIT
        (cprif_tx_framer_tgen_tick_set(unit, port,
                                       divider,
                                       basic_frame_ticks,
                                       ticks_bitmap,
                                       ticks_bitmap_size));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_tx_framer_tgen_tick_config_set(int unit, int port, cprimod_port_rsvd4_speed_mult_t speed)
{
    uint32 divider = 0;
    uint32 ticks_bitmap = 0;
    uint32 ticks_bitmap_size = 0;
    uint32 basic_frame_ticks = 0;
    SOC_INIT_FUNC_DEFS;

    switch (speed) {
        case cprimodRsvd4SpdMult4X:
            divider             = 2;
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 200;
            break;
        case cprimodRsvd4SpdMult8X:
            divider             = 1;
            ticks_bitmap        = 0;
            ticks_bitmap_size   = 0;
            basic_frame_ticks   = 100;
            break;
        default:
            _SOC_EXIT_WITH_ERR
                (SOC_E_PARAM, ("This RSVD4 Speed is not supported.\n"));
            break;
    }
    _SOC_IF_ERR_EXIT
        (cprif_tx_framer_tgen_tick_set(unit, port,
                                       divider,
                                       basic_frame_ticks,
                                       ticks_bitmap,
                                       ticks_bitmap_size));

exit:
    SOC_FUNC_RETURN;
}

int cprif_tx_framer_tgen_enable(int unit, int port, uint8 enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_enable(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}


int cprif_tx_framer_tgen_offset_set(int unit, int port, uint64 timestamp_offset)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_offset_set(unit, port,timestamp_offset));



exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_offset_get(int unit, int port, uint64* timestamp_offset)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_offset_get(unit, port,timestamp_offset));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_next_bfn_config_set(int unit, int port, uint32 tgen_bfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_next_bfn_config_set(unit, port, tgen_bfn));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_next_bfn_config_get(int unit, int port, uint32* tgen_bfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_next_bfn_config_get(unit, port, tgen_bfn));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_next_hfn_config_set(int unit, int port, uint32 tgen_hfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_next_hfn_config_set(unit, port, tgen_hfn));

exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_next_hfn_config_get(int unit, int port, uint32* tgen_hfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_next_hfn_config_get(unit, port, tgen_hfn));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_next_bfn_get(int unit, int port, uint32* tgen_bfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_next_bfn_get(unit, port, tgen_bfn));

exit:
    SOC_FUNC_RETURN;

}   


int cprif_tx_framer_next_hfn_get(int unit, int port, uint32* tgen_hfn)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_next_hfn_get(unit, port, tgen_hfn));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_select_counter_set(int unit, int port, cprimod_tgen_counter_select_t tgen_counter_select)
{
    uint8 bfn_hfn;
    SOC_INIT_FUNC_DEFS;

    if (tgen_counter_select == cprimodTgenCounterSelectBfn) {
        bfn_hfn = CPRIF_TX_TGEN_USE_BFN;
    } else {
        bfn_hfn = CPRIF_TX_TGEN_USE_HFN;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_select_counter_set(unit,port,bfn_hfn));
exit:
    SOC_FUNC_RETURN;

}

int cprif_tx_framer_tgen_select_counter_get(int unit, int port, cprimod_tgen_counter_select_t* tgen_counter_select)
{
    uint8 bfn_hfn;
    SOC_INIT_FUNC_DEFS;

    bfn_hfn = 0;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_framer_tgen_select_counter_get(unit,port,&bfn_hfn));

    if (bfn_hfn == CPRIF_TX_TGEN_USE_BFN) {
        *tgen_counter_select = cprimodTgenCounterSelectBfn;
    } else {
        *tgen_counter_select = cprimodTgenCounterSelectHfn;
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_header_compare_entry_add(int unit, int port,
                                    cprimod_header_compare_entry_t* entry)
{
    cprimod_header_compare_entry_t local_entry;
    int index;

    cprimod_header_compare_entry_t_init( &local_entry);
    for(index=0; index < CPRIF_HEADER_COMPARE_TABLE_SIZE;index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_rsvd4_header_compare_entry_get(unit, port,
                                                      index,
                                                      &local_entry));
        /*
         * if empty spot is found, add the entry.
         */
        if (local_entry.valid == 0) {
            return(cprif_drv_rsvd4_header_compare_entry_set(unit, port,
                                                            index,
                                                            entry));
        }
    }
    LOG_CLI(("Header Compare Table No Empty Space Found.\n"));
    return SOC_E_PARAM;
}


int cprif_header_compare_entry_delete(int unit, int port,
                                      cprimod_header_compare_entry_t* entry)
{
    cprimod_header_compare_entry_t local_entry;
    int index;

    cprimod_header_compare_entry_t_init( &local_entry);

    for(index=0; index < CPRIF_HEADER_COMPARE_TABLE_SIZE;index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_rsvd4_header_compare_entry_get(unit, port,
                                                      index,
                                                      &local_entry));
        /*
         * if valid entry, compare to see if match.
         */
        if (local_entry.valid == 1) {

            /*
             * If matched entry found, invalidate and get out.
             */

            if ((local_entry.match_data == entry->match_data) &&
               (local_entry.mask == entry->mask) &&
               (local_entry.flow_id == entry->flow_id) &&
               (local_entry.flow_type == entry->flow_type)) {

                    cprimod_header_compare_entry_t_init( &local_entry);
                    local_entry.valid = 0;
                    return(cprif_drv_rsvd4_header_compare_entry_set(unit, port,
                                                            index,
                                                            &local_entry));
            }
        }
    }
    LOG_CLI(("No Matching Header Compare Entry Found to Delete .\n"));
    return SOC_E_PARAM;
}

int cprif_header_compare_entry_for_flow_delete(int unit, int port, uint8 flow_id, cprimod_flow_type_t flow_type)
{
    cprimod_header_compare_entry_t local_entry;
    int index;

    cprimod_header_compare_entry_t_init( &local_entry);

    for(index=0; index < CPRIF_HEADER_COMPARE_TABLE_SIZE;index++) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_rsvd4_header_compare_entry_get(unit, port,
                                                      index,
                                                      &local_entry));
        /*
         * if valid entry, compare to see if match.
         */
        if (local_entry.valid == 1) {

            /*
             * If matched entry found, invalidate and get out.
             */

            if ((local_entry.flow_id   == flow_id) &&
                (local_entry.flow_type == flow_type)) {

                    cprimod_header_compare_entry_t_init( &local_entry);
                    local_entry.valid = 0;
                    return(cprif_drv_rsvd4_header_compare_entry_set(unit, port,
                                                            index,
                                                            &local_entry));
            }
        }
    }
    LOG_CLI(("No Matching Header Compare Entry Found to Delete .\n"));
    return SOC_E_PARAM;
}

int cprif_modulo_rule_entry_set(int unit, int port,
                                uint8 modulo_rule_num,
                                cprimod_transmission_rule_type_t modulo_rule_type,
                                cprimod_modulo_rule_entry_t* modulo_rule)
{
    uint8 ctrl_rule;
    SOC_INIT_FUNC_DEFS;

    if (modulo_rule_type == cprimodTxRuleTypeCtrl) {
        ctrl_rule = CPRIF_MODULO_RULE_USE_CTRL_RULE;
        if (modulo_rule_num >= CPRIF_MODULO_RULE_CTRL_RULE_SIZE) {
            CPRIMOD_DEBUG_ERROR(("Rule Num  %d is out of range > than %d",
                    modulo_rule_num,
                    CPRIF_MODULO_RULE_CTRL_RULE_SIZE));
            return SOC_E_PARAM;
        }

    } else {
        ctrl_rule = CPRIF_MODULO_RULE_USE_DATA_RULE;
        if (modulo_rule_num >= CPRIF_MODULO_RULE_DATA_RULE_SIZE) {
            CPRIMOD_DEBUG_ERROR(("Rule Num  %d is out of range > than %d",
                    modulo_rule_num,
                    CPRIF_MODULO_RULE_DATA_RULE_SIZE));
            return SOC_E_PARAM;
        }
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_modulo_rule_entry_set(unit, port,
                                               modulo_rule_num,
                                               ctrl_rule,
                                               modulo_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_modulo_rule_entry_get(int unit, int port,
                                uint8 modulo_rule_num,
                                cprimod_transmission_rule_type_t modulo_rule_type,
                                cprimod_modulo_rule_entry_t* modulo_rule)
{
    uint8 ctrl_rule;
    SOC_INIT_FUNC_DEFS;

    if (modulo_rule_type == cprimodTxRuleTypeCtrl) {
        ctrl_rule = CPRIF_MODULO_RULE_USE_CTRL_RULE;
        if (modulo_rule_num >= CPRIF_MODULO_RULE_CTRL_RULE_SIZE) {
            CPRIMOD_DEBUG_ERROR(("Rule Num  %d is out of range > than %d",
                    modulo_rule_num,
                    CPRIF_MODULO_RULE_CTRL_RULE_SIZE));
            return SOC_E_PARAM;
        }

    } else {
        ctrl_rule = CPRIF_MODULO_RULE_USE_DATA_RULE;
        if (modulo_rule_num >= CPRIF_MODULO_RULE_DATA_RULE_SIZE) {
            CPRIMOD_DEBUG_ERROR(("Rule Num  %d is out of range > than %d",
                    modulo_rule_num,
                    CPRIF_MODULO_RULE_DATA_RULE_SIZE));
            return SOC_E_PARAM;
        }
    }


    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_modulo_rule_entry_get(unit, port,
                                               modulo_rule_num,
                                               ctrl_rule,
                                               modulo_rule));
exit:
    SOC_FUNC_RETURN;
}

int cprif_modulo_rule_flow_enable_set(int unit, int port, int enable, cprimod_flow_type_t flow_type, uint32 flow_id)
{
    uint8 ctrl_rule = 0;
    int rule_num;
    cprimod_modulo_rule_entry_t modulo_rule;
    SOC_INIT_FUNC_DEFS;

    /*
     * Enable/Disable all the DATA DBM rule that are associated with this particular 
     * flow.
     */

    ctrl_rule = 0;
    for (rule_num = 0; rule_num < CPRIF_MODULO_RULE_DATA_RULE_SIZE; rule_num++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_modulo_rule_entry_get(unit, port,
                                                   rule_num,
                                                   ctrl_rule,
                                                   &modulo_rule));
        if ((modulo_rule.active != enable) &&
            (modulo_rule.dbm_enable == 0) &&
            (modulo_rule.flow_type == flow_type) &&
            (modulo_rule.flow_dbm_id == flow_id)) {
                modulo_rule.active = enable;
                _SOC_IF_ERR_EXIT
                    (cprif_drv_rsvd4_modulo_rule_entry_set(unit, port,
                                                           rule_num,
                                                           ctrl_rule,
                                                           &modulo_rule));
        } else {
            /* 
             * if the mod rule is already disabled  and try to disable it,
             * mark that with combination that didn't exist.
             */
            if ((enable == 0) && (modulo_rule.active == 0)) {
                modulo_rule.flow_type       = cprimodFlowTypeCtrl;
                modulo_rule.flow_dbm_id     = 0x3F;  
               _SOC_IF_ERR_EXIT
                    (cprif_drv_rsvd4_modulo_rule_entry_set(unit, port,
                                                           rule_num,
                                                           ctrl_rule,
                                                           &modulo_rule));
            }
        }
    }
    /*
     * Enable/Disable all the CTRL DBM rule that are associated with this particular 
     * flow.
     */
    ctrl_rule = 1;
    for (rule_num = 0; rule_num < CPRIF_MODULO_RULE_CTRL_RULE_SIZE; rule_num++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_modulo_rule_entry_get(unit, port,
                                                   rule_num,
                                                   ctrl_rule,
                                                   &modulo_rule));
        if ((modulo_rule.active != enable) &&
            (modulo_rule.dbm_enable == 0) &&
            (modulo_rule.flow_type == flow_type) &&
            (modulo_rule.flow_dbm_id == flow_id)) {
                modulo_rule.active = enable;
                _SOC_IF_ERR_EXIT
                    (cprif_drv_rsvd4_modulo_rule_entry_set(unit, port,
                                                           rule_num,
                                                           ctrl_rule,
                                                           &modulo_rule));
        } else {
            /* 
             * if the mod rule is already disabled  and try to disable it,
             * mark that with combination that didn't exist.
             */
            if ((enable == 0) && (modulo_rule.active == 0)) {
                modulo_rule.flow_type       = cprimodFlowTypeCtrl;
                modulo_rule.flow_dbm_id     = 0x3F; 
               _SOC_IF_ERR_EXIT
                    (cprif_drv_rsvd4_modulo_rule_entry_set(unit, port,
                                                           rule_num,
                                                           ctrl_rule,
                                                           &modulo_rule));
            }
        }
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_modulo_rule_flow_enable_get(int unit, int port, int* enable, cprimod_flow_type_t flow_type, uint32 flow_id)
{
    uint8 ctrl_rule = 0;
    int rule_num;
    cprimod_modulo_rule_entry_t modulo_rule;
    SOC_INIT_FUNC_DEFS;

    *enable = 0;
    /*
     * Get enable status of  all the DATA DBM rule that are associated with this particular 
     * flow.
     */

    ctrl_rule = 0;
    for (rule_num = 0; rule_num < CPRIF_MODULO_RULE_DATA_RULE_SIZE; rule_num++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_modulo_rule_entry_get(unit, port,
                                                   rule_num,
                                                   ctrl_rule,
                                                   &modulo_rule));
        if ((modulo_rule.active == 1) &&
            (modulo_rule.dbm_enable == 0) &&
            (modulo_rule.flow_type == flow_type) &&
            (modulo_rule.flow_dbm_id == flow_id)) {
                *enable |= TRUE;
        } 
    }
    /*
     * Get enable status of  all the CTRL DBM rule that are associated with this particular 
     * flow.
     */
    ctrl_rule = 1;
    for (rule_num = 0; rule_num < CPRIF_MODULO_RULE_CTRL_RULE_SIZE; rule_num++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_modulo_rule_entry_get(unit, port,
                                                   rule_num,
                                                   ctrl_rule,
                                                   &modulo_rule));
        if ((modulo_rule.active == 1) &&
            (modulo_rule.dbm_enable == 0) &&
            (modulo_rule.flow_type == flow_type) &&
            (modulo_rule.flow_dbm_id == flow_id)) {
                *enable |= TRUE;
        } 
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_dual_bitmap_rule_entry_set(int unit, int port,
                                     uint8 dbm_rule_num,
                                     cprimod_dbm_rule_entry_t* dbm_rule)
{
    SOC_INIT_FUNC_DEFS;

    if (dbm_rule_num >= CPRIF_DBM_RULE_PROFILE_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("DBM Rule Number  %d is out of range > than %d",
                              dbm_rule_num,
                              CPRIF_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }


    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_dual_bitmap_rule_entry_set(unit, port,
                                                    dbm_rule_num,
                                                    dbm_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_dual_bitmap_rule_entry_get(int unit, int port,
                                     uint8 dbm_rule_num,
                                     cprimod_dbm_rule_entry_t* dbm_rule)
{

    SOC_INIT_FUNC_DEFS;

    if (dbm_rule_num >= CPRIF_DBM_RULE_PROFILE_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("DBM Rule Number  %d is out of range > than %d",
                              dbm_rule_num,
                              CPRIF_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_dual_bitmap_rule_entry_get(unit, port,
                                                    dbm_rule_num,
                                                    dbm_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_secondary_dual_bitmap_rule_entry_set(int unit, int port,
                                     uint8 dbm_rule_num,
                                     cprimod_secondary_dbm_rule_entry_t* dbm_rule)
{
    SOC_INIT_FUNC_DEFS;

    if (dbm_rule_num >= CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Secondary DBM Rule Number  %d is out of range > than %d",
                              dbm_rule_num,
                              CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }


    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_set(unit, port,
                                                    dbm_rule_num,
                                                    dbm_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_secondary_dual_bitmap_rule_entry_get(int unit, int port,
                                     uint8 dbm_rule_num,
                                     cprimod_secondary_dbm_rule_entry_t* dbm_rule)
{

    SOC_INIT_FUNC_DEFS;

    if (dbm_rule_num >= CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("Secondary DBM Rule Number  %d is out of range > than %d",
                              dbm_rule_num,
                              CPRIF_SECONDARY_DBM_RULE_PROFILE_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_secondary_dual_bitmap_rule_entry_get(unit, port,
                                                    dbm_rule_num,
                                                    dbm_rule));
exit:
    SOC_FUNC_RETURN;

}

int cprif_dbm_position_entry_set(int unit, int port, uint8 index,
                                 cprimod_dbm_pos_table_entry_t* pos_entry)
{
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_DBM_POSITION_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("DBM Position Table Index  %d is out of range > than %d",
                              index,
                              CPRIF_DBM_POSITION_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_dbm_position_entry_set(unit, port, index, pos_entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_dbm_position_entry_get(int unit, int port, uint8 index,
                                 cprimod_dbm_pos_table_entry_t* pos_entry)
{
    SOC_INIT_FUNC_DEFS;
    if (index >= CPRIF_DBM_POSITION_TABLE_SIZE) {
        CPRIMOD_DEBUG_ERROR(("DBM Position Table Index  %d is out of range > than %d",
                              index,
                              CPRIF_DBM_POSITION_TABLE_SIZE));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_dbm_position_entry_get(unit, port, index, pos_entry));
exit:
    SOC_FUNC_RETURN;

}

int cprif_dbm_position_flow_enable_set(int unit, int port, int enable, cprimod_flow_type_t flow_type, uint32 flow_id)
{
    cprimod_dbm_pos_table_entry_t pos_entry;
    int index;
    SOC_INIT_FUNC_DEFS;

    /*
     * Enable/Disable all the position entries that are associated with this particular
     * flow.
     */

    for (index = 0; index < CPRIF_DBM_POSITION_TABLE_SIZE; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_dbm_position_entry_get(unit, port, index, &pos_entry));
        if((pos_entry.valid != enable) &&
           (pos_entry.flow_id == flow_id) &&
           (pos_entry.flow_type == flow_type)) {
            pos_entry.valid = enable;
            _SOC_IF_ERR_EXIT
                (cprif_drv_rsvd4_dbm_position_entry_set(unit, port, index, &pos_entry));
        } else {
            if ((enable == 0) && (pos_entry.valid == 0)){
                pos_entry.flow_id = 0x3F;
                pos_entry.flow_type = cprimodFlowTypeCtrl;
                _SOC_IF_ERR_EXIT
                    (cprif_drv_rsvd4_dbm_position_entry_set(unit, port, index, &pos_entry));
            }
        }
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_dbm_position_flow_enable_get(int unit, int port, int* enable, cprimod_flow_type_t flow_type, uint32 flow_id)
{   
    cprimod_dbm_pos_table_entry_t pos_entry;
    int index;
    SOC_INIT_FUNC_DEFS;

    *enable = 0;
    /*
     *  Get enable status of  all the position entries that are associated with 
     *  this particular flow.
 */
    for (index = 0; index < CPRIF_DBM_POSITION_TABLE_SIZE; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rsvd4_dbm_position_entry_get(unit, port, index, &pos_entry));
        if((pos_entry.valid == 1) &&
           (pos_entry.flow_id == flow_id) &&
           (pos_entry.flow_type == flow_type)) {
            *enable |= TRUE;
        }
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_fast_clk_bit_time_period_set(int unit, int port, uint32 bit_time_period)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_fast_clk_bit_time_period_set(unit, port, bit_time_period));

exit:
    SOC_FUNC_RETURN;

}

int cprif_fast_clk_bit_time_period_get(int unit, int port, uint32* bit_time_period)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_fast_clk_bit_time_period_get(unit, port, bit_time_period));

exit:
    SOC_FUNC_RETURN;

}



static
int _cprif_port_rx_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                               cprimod_port_speed_t speed)
{
    uint32 wd_len;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_cpri_rxpcs_speed_set(unit, port, phy,
                                                       speed));

    if (speed <=cprimodSpd9830p4) {
        SOC_IF_ERROR_RETURN
            (cprif_drv_ingress_traffic_rate_set(unit, port, CPRIF_INGRESS_RATE_ONE_THIRD));

       /* Enable the decompression traffic increase to compensate for lower clock. */
        SOC_IF_ERROR_RETURN
            (cprif_drv_decompression_traffic_rate_increase_set(unit, port, 1));

    } else {
        SOC_IF_ERROR_RETURN
            (cprif_drv_ingress_traffic_rate_set(unit, port, CPRIF_INGRESS_RATE_TWO_THIRD));

       /* Disable the decompression traffic increase to compensate for lower clock. */
        SOC_IF_ERROR_RETURN
            (cprif_drv_decompression_traffic_rate_increase_set(unit, port, 0));

    }

    _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));
    wd_len = _cprif_from_speed_towd_len(speed);
    /* Adding AXC for control word */
    _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, 255,
                                                  0, wd_len));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_rx_interface_config_set(int unit, int port, const phymod_phy_access_t* phy,
                               const cprimod_port_interface_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    if (config->speed >= cprimodSpdCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed not supported for CPRI"));
    }
    _SOC_IF_ERR_EXIT(_cprif_port_rx_speed_set(unit, port, phy, config->speed));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_rx_interface_config_get(int unit, int port,
                                       cprimod_port_interface_config_t* config)
{
    cprimod_port_rsvd4_speed_mult_t rsvd4_speed;

    SOC_INIT_FUNC_DEFS;


    SOC_IF_ERROR_RETURN(cprif_drv_rxcpri_port_type_get(unit, port,
                                                   &(config->interface)));

    if (config->interface == cprimodTunnel) {
        /*
         *  In Tunnel Mode, only CPRI or RSVD4 speed will be valid.
         *  Whichever is valid, that is the port encap type.
         */
        SOC_IF_ERROR_RETURN(cprif_drv_cpri_rxpcs_speed_get(unit, port,
                                                    &(config->speed)));

        if (config->speed == cprimodSpdCount) {
            SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rxpcs_speed_get(unit, port,
                                                       &rsvd4_speed));
            switch(rsvd4_speed) {
                case cprimodRsvd4SpdMult4X:
                    config->speed = cprimodSpd3072p0;
                    break;
                case cprimodRsvd4SpdMult8X:
                    config->speed = cprimodSpd6144p0;
                    break;
                default:
                    config->speed = cprimodSpdCount;
                    break;
            }
            config->port_encap_type =  cprimodPortEncapRsvd4;
        } else {
            config->port_encap_type =  cprimodPortEncapCpri;
        }
    } else if (config->interface == cprimodRsvd4) {
        SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rxpcs_speed_get(unit, port,
                                                       &rsvd4_speed));
        switch(rsvd4_speed) {
        case cprimodRsvd4SpdMult4X:
            config->speed = cprimodSpd3072p0;
            break;
        case cprimodRsvd4SpdMult8X:
            config->speed = cprimodSpd6144p0;
            break;
        default:
            config->speed = cprimodSpdCount;
            break;
        }
        config->port_encap_type =  cprimodPortEncapRsvd4;
    }else {
        SOC_IF_ERROR_RETURN(cprif_drv_cpri_rxpcs_speed_get(unit, port,
                                                    &(config->speed)));
        config->port_encap_type =  cprimodPortEncapCpri;
    }

    SOC_FUNC_RETURN;
}

static
int _cprif_port_tx_speed_set(int unit, int port, const phymod_phy_access_t* phy,
                               cprimod_port_speed_t speed)
{
    uint32 wd_len;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_txpcs_speed_set(unit, port, phy, speed));

   _SOC_IF_ERR_EXIT 
        (cprif_cpri_tx_framer_tgen_tick_config_set(unit, port, speed));

    _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));
    wd_len = _cprif_from_speed_towd_len(speed);
    /* Adding AXC for control word */
    _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 255,
                                                  0, wd_len));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_tx_interface_config_set(int unit, int port, const phymod_phy_access_t* phy,
                                       const cprimod_port_interface_config_t* config)
{
    SOC_INIT_FUNC_DEFS;

    if (config->speed >= cprimodSpdCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed not supported for CPRI"));
    }
    SOC_IF_ERROR_RETURN(_cprif_port_tx_speed_set(unit, port, phy,
                                                        config->speed));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_tx_interface_config_get(int unit, int port,
                                       cprimod_port_interface_config_t* config)
{
    cprimod_port_rsvd4_speed_mult_t rsvd4_speed;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_txcpri_port_type_get(unit, port,
                                                   &(config->interface)));

    if (config->interface == cprimodTunnel) {
        /*
         *  In Tunnel Mode, only CPRI or RSVD4 speed will be valid.
         *  Whichever is valid, that is the port encap type.
         */
        SOC_IF_ERROR_RETURN(cprif_drv_cpri_txpcs_speed_get(unit, port,
                                                    &(config->speed)));

        if (config->speed == cprimodSpdCount) {
            SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_txpcs_speed_get(unit, port,
                                                       &rsvd4_speed));
            switch(rsvd4_speed) {
                case cprimodRsvd4SpdMult4X:
                    config->speed = cprimodSpd3072p0;
                    break;
                case cprimodRsvd4SpdMult8X:
                    config->speed = cprimodSpd6144p0;
                    break;
                default:
                    config->speed = cprimodSpdCount;
                    break;
            }
            config->port_encap_type =  cprimodPortEncapRsvd4;
        } else {
            config->port_encap_type =  cprimodPortEncapCpri;
        }
    } else if (config->interface == cprimodRsvd4) {
        SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_txpcs_speed_get(unit, port,
                                                       &rsvd4_speed));
        switch(rsvd4_speed) {
        case cprimodRsvd4SpdMult4X:
            config->speed = cprimodSpd3072p0;
            break;
        case cprimodRsvd4SpdMult8X:
            config->speed = cprimodSpd6144p0;
            break;
        default:
            config->speed = cprimodSpdCount;
            break;
        }
        config->port_encap_type =  cprimodPortEncapRsvd4;
    } else {
        SOC_IF_ERROR_RETURN(cprif_drv_cpri_txpcs_speed_get(unit, port,
                                                       &(config->speed)));
        config->port_encap_type =  cprimodPortEncapCpri;
    }

    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_rx_interface_config_set(int unit, int port, const phymod_phy_access_t* phy,
                          const cprimod_port_rsvd4_config_t* config) {
    SOC_INIT_FUNC_DEFS;

    if (config->speed >= cprimodRsvd4SpdMultCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed not supported for CPRI"));
    }
    /* coverity[mixed_enums:FALSE] */
    SOC_IF_ERROR_RETURN(cprif_drv_cpri_rxpcs_speed_set(unit, port, phy,
                                                        config->speed));
exit:
    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_interface_config_set(int unit, int port, const phymod_phy_access_t* phy,
                          const cprimod_port_rsvd4_config_t* config) {
    SOC_INIT_FUNC_DEFS;

    if (config->speed >= cprimodRsvd4SpdMultCount) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Speed not supported for CPRI"));
    }

    SOC_IF_ERROR_RETURN
        (cprif_drv_rsvd4_txpcs_speed_set(unit, port, phy, config->speed));

exit:
    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_tx_interface_config_get(int unit, int port,
                          cprimod_port_rsvd4_config_t* config) {
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_txpcs_speed_get(unit, port,
                                                   &(config->speed)));

    SOC_FUNC_RETURN;
}

int cprif_port_rx_frame_optional_config_set(int unit, int port,
                     cprimod_rx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rx_frame_optional_config_set(
                        unit, port, field, value));
    SOC_FUNC_RETURN;
}

int cprif_port_rx_frame_optional_config_get(int unit, int port,
                     cprimod_rx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rx_frame_optional_config_get(
                        unit, port, field, value));

    SOC_FUNC_RETURN;
}


int cprif_port_tx_frame_optional_config_set(int unit, int port,
                     cprimod_tx_config_field_t field, uint32 value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_tx_frame_optional_config_set(
                        unit, port, field, value));
    SOC_FUNC_RETURN;
}

int cprif_port_tx_frame_optional_config_get(int unit, int port,
                     cprimod_tx_config_field_t field, uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_tx_frame_optional_config_get(
                        unit, port, field, value));

    SOC_FUNC_RETURN;
}


int cprif_port_rsvd4_rx_frame_optional_config_set(int unit, int port,
                     cprimod_rsvd4_rx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_frame_optional_config_set(
                        unit, port, field, value));

    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_rx_frame_optional_config_get(int unit, int port,
                     cprimod_rsvd4_rx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_frame_optional_config_get(
                        unit, port, field, value));

    SOC_FUNC_RETURN;
}


int _cprif_port_rsvd4_basic_frame_table_set (int unit, int port, cprimod_direction_t dir)
{
    cprif_bfa_bfp_table_entry_t table_entry;
    uint8 active_table;
    uint8 standby_table;
    int index = 0;
    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {
        /* add following
         *entry num   axcc_id     rsrv_cnt    data_cnt    Description
         *0            0           0           24         extract 24 bits of hdr
         *1            1           0           128        extract 128 bits of pld
         */

        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_parser_active_table_get(unit, port, &active_table));

        if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
            standby_table = CPRIF_BASIC_FRAME_TABLE_1;
        } else {
            standby_table = CPRIF_BASIC_FRAME_TABLE_0;
        }

        table_entry.axc_id = 0;
        table_entry.rsrv_bit_cnt = 0;
        table_entry.data_bit_cnt = CPRIMOD_RSVD4_HEADER_SIZE;

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_entry_set(unit, port,
                                             standby_table,
                                             CPRIMOD_DIR_RX,
                                             0,
                                             &table_entry));
        table_entry.axc_id = 1;
        table_entry.rsrv_bit_cnt = 0;
        table_entry.data_bit_cnt = CPRIMOD_RSVD4_PAYLOAD_SIZE;

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_entry_set(unit, port,
                                             standby_table,
                                             CPRIMOD_DIR_RX,
                                             1,
                                             &table_entry));

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port,
                                                                standby_table,
                                                                2));


    } else if (dir == CPRIMOD_DIR_TX) {
        /*entry num   axcc_id     rsrv_cnt    data_cnt    Description
         *0           0           0           24          extract 24 bits of hdr
         *1           1           0           128         extract 128 bits of pld
         *2           0           0           24          extract 24 bits of hdr
         *3           1           0           128         extract 128 bits of pld
         *  ...............................
         *40          0           0           24          extract 24 bits of hdr
         *41          1           0           128         extract 128 bits of pld
         *
         * i  axcc_id   start bit             datasize
         *
         * 0  0         0*(24+128)    = 0      24
         * 0  1         0*(24+128)+24 = 24     128
         * 1  0         1*(24+128)    = 152    24
         * 1  1         1*(24+128)+24 = 176    128
         * 2  0         2*(24+128)    = 304    24
         * 2  1         2*(24+128)+24 = 328    128
         */

        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_assembly_active_table_get(unit, port, &active_table));

        if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
            standby_table = CPRIF_BASIC_FRAME_TABLE_1;
        } else {
            standby_table = CPRIF_BASIC_FRAME_TABLE_0;
        }

        for(index = 0; index < 21; index++) {
            table_entry.axc_id = 0;
            table_entry.rsrv_bit_cnt = 0;
            table_entry.data_bit_cnt = CPRIMOD_RSVD4_HEADER_SIZE;

            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_entry_set(unit, port,
                                                 standby_table,
                                                 CPRIMOD_DIR_TX,
                                                 index*2,
                                                 &table_entry));
            table_entry.axc_id = 1;
            table_entry.rsrv_bit_cnt = 0;
            table_entry.data_bit_cnt = CPRIMOD_RSVD4_PAYLOAD_SIZE;

            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_entry_set(unit, port,
                                                 standby_table,
                                                 CPRIMOD_DIR_TX,
                                                 index*2+1,
                                                 &table_entry));

        }

        table_entry.axc_id = 1;
        table_entry.rsrv_bit_cnt = 8;
        table_entry.data_bit_cnt = 0;

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_entry_set(unit, port,
                                             standby_table,
                                             CPRIMOD_DIR_TX,
                                             42,
                                             &table_entry));

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port,
                                                               standby_table,
                                                               43));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_rx_speed_set(int unit, int port, const phymod_phy_access_t* phy, cprimod_port_rsvd4_speed_mult_t speed)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rxpcs_speed_set(unit, port, phy, speed));
    SOC_IF_ERROR_RETURN
        (cprif_drv_ingress_traffic_rate_set(unit, port, CPRIF_INGRESS_RATE_ONE_THIRD));

    SOC_IF_ERROR_RETURN
        (_cprif_port_rsvd4_basic_frame_table_set (unit, port, CPRIMOD_DIR_RX));

    SOC_FUNC_RETURN;
}

int cprif_port_rsvd4_rx_speed_get(int unit, int port, cprimod_port_rsvd4_speed_mult_t* speed)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rxpcs_speed_get(unit, port, speed));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_speed_set(int unit, int port, const phymod_phy_access_t* phy, cprimod_port_rsvd4_speed_mult_t speed)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_txpcs_speed_set(unit, port, phy, speed));

    SOC_IF_ERROR_RETURN
        (_cprif_port_rsvd4_basic_frame_table_set (unit, port, CPRIMOD_DIR_TX));

   _SOC_IF_ERR_EXIT
        (cprif_rsvd4_tx_framer_tgen_tick_config_set(unit, port, speed));

exit:
    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_speed_get(int unit, int port, cprimod_port_rsvd4_speed_mult_t* speed)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_txpcs_speed_get(unit, port, speed));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_rx_fsm_state_set(int unit, int port,
                     cprimod_rsvd4_rx_fsm_state_t state)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_fsm_state_set(
                        unit, port, state));

    SOC_FUNC_RETURN;
}


int cprif_port_rsvd4_rx_fsm_state_get(int unit, int port,
                     cprimod_rsvd4_rx_fsm_state_t* state)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_fsm_state_get(
                        unit, port, state));

    SOC_FUNC_RETURN;
}


int cprif_port_rsvd4_rx_overide_set(int unit, int port,
          cprimod_rsvd4_rx_overide_t parameter, int enable, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_overide_set(
                        unit, port, parameter, enable, value));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_rx_overide_get(int unit, int port,
          cprimod_rsvd4_rx_overide_t parameter, int* enable, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_overide_get(
                        unit, port, parameter, enable, value));

    SOC_FUNC_RETURN;

}


int cprif_port_rsvd4_tx_frame_optional_config_set(int unit, int port,
                     cprimod_rsvd4_tx_config_field_t field, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_frame_optional_config_set(
                        unit, port, field, value));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_frame_optional_config_get(int unit, int port,
                     cprimod_rsvd4_tx_config_field_t field, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_frame_optional_config_get(
                        unit, port, field, value));

    SOC_FUNC_RETURN;

}


int cprif_port_rsvd4_tx_fsm_state_set(int unit, int port,
                     cprimod_rsvd4_tx_fsm_state_t state)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_fsm_state_set(
                        unit, port, state));
    SOC_FUNC_RETURN;

}


int cprif_port_rsvd4_tx_fsm_state_get(int unit, int port,
                     cprimod_rsvd4_tx_fsm_state_t *state)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_fsm_state_get(
                        unit, port, state));
    SOC_FUNC_RETURN;

}


int cprif_port_rsvd4_tx_overide_set(int unit, int port,
          cprimod_rsvd4_tx_overide_t parameter, int enable, int value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_overide_set(
                        unit, port, parameter, enable, value));

    SOC_FUNC_RETURN;

}

int cprif_port_rsvd4_tx_overide_get(int unit, int port,
          cprimod_rsvd4_tx_overide_t parameter, int* enable, int* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_overide_get(
                        unit, port, parameter, enable, value));

    SOC_FUNC_RETURN;

}

int cprif_port_rx_pcs_status_get(int unit, int port,
                     cprimod_rx_pcs_status_t status, uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_rx_pcs_status_get(
                        unit, port, status, value));
    SOC_FUNC_RETURN;

}

int cprif_port_pmd_status_get(int unit, int port, cprimod_pmd_port_status_t status, uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_pmd_port_status_get( unit, port, status, value));

    SOC_FUNC_RETURN;
}

STATIC
int _cpri_core_firmware_load(const phymod_core_access_t* core, cprimod_firmware_load_method_t load_method, cprimod_firmware_loader_f fw_loader)
{
    SOC_INIT_FUNC_DEFS;
    /*
     * Temporarily supress the DEADCODE until this forced phymodFirmwareLoadMethodInternal
     * is removed.
     */

    /* coverity[dead_error_condition:FALSE] */
    load_method = phymodFirmwareLoadMethodInternal;
    /* coverity[dead_error_condition:FALSE] */
    switch(load_method){
    case phymodFirmwareLoadMethodInternal:
        _SOC_IF_ERR_EXIT(falcon2_monterey_ucode_mdio_load(&core->access, falcon_dpll_ucode, falcon_dpll_ucode_len));
        break;
    /* coverity[dead_error_condition:FALSE] */
    case phymodFirmwareLoadMethodExternal:
        PHYMOD_NULL_CHECK(fw_loader);
        _SOC_IF_ERR_EXIT(falcon2_monterey_ucode_init(&core->access));
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_pram_firmware_enable(&core->access, 1, 0));
        _SOC_IF_ERR_EXIT(fw_loader(core, falcon_dpll_ucode_len, falcon_dpll_ucode));
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_pram_firmware_enable(&core->access, 0, 0));
        break;
    /* coverity[dead_error_condition:FALSE] */
    case phymodFirmwareLoadMethodNone:
        break;
    default:
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, (_PHYMOD_MSG("illegal fw load method %u"), load_method));
    }

exit:
    SOC_FUNC_RETURN;
}

/* cpri_or_pmd == 1 => cpri lane swap
   cpri_or_pmd == 0 => PMD lane swap */
STATIC
int _cpri_core_lane_map_set(int unit, int port,
                            const phymod_core_access_t* core,
                            const phymod_lane_map_t* lane_map, int cpri_or_pmd)
{
    uint32_t lane;
    uint32_t map_lane, tsc_tx_swap = 0, tsc_rx_swap = 0;
    uint8_t pmd_tx_addr[4], pmd_rx_addr[4];
    SOC_INIT_FUNC_DEFS;

    if (lane_map->num_of_lanes != CPRI_NOF_LANES_IN_CORE){
        return PHYMOD_E_CONFIG;
    }
    for (lane = 0; lane < CPRI_NOF_LANES_IN_CORE; lane++){
        if ((lane_map->lane_map_tx[lane] >= CPRI_NOF_LANES_IN_CORE)||
             (lane_map->lane_map_rx[lane] >= CPRI_NOF_LANES_IN_CORE)){
            return SOC_E_PARAM;
        }
    }
    /* PMD lane addr is based on PCS logical to physical mapping */
    for (lane = 0; lane < CPRI_NOF_LANES_IN_CORE; lane++){
        pmd_tx_addr[(lane_map->lane_map_tx[lane] & 0xf)] = lane;
        pmd_rx_addr[(lane_map->lane_map_rx[lane] & 0xf)] = lane;
    }

    if(cpri_or_pmd) {
        for (lane = 0; lane < CPRI_NOF_LANES_IN_CORE; lane++){
            map_lane = lane_map->lane_map_tx[lane];
            SOC_IF_ERROR_RETURN
            (cprif_drv_lane_swap_set(unit, port, lane, CPRIMOD_DIR_TX, map_lane));
            map_lane = lane_map->lane_map_rx[lane];
            SOC_IF_ERROR_RETURN
            (cprif_drv_lane_swap_set(unit, port, lane, CPRIMOD_DIR_RX, map_lane));
            tsc_tx_swap += lane_map->lane_map_tx[lane] << (lane * 4);
            tsc_rx_swap += lane_map->lane_map_rx[lane] << (lane * 4);
        }
        /*
         * Program TSC lane map so that TSC driver can use this information
         * to get logical to physical lane mapping for SyncE logic programming
         * on Cpri port.
         */
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pcs_tx_lane_swap(&core->access, tsc_tx_swap));
        PHYMOD_IF_ERR_RETURN
            (tefmod_gen3_pcs_rx_lane_swap(&core->access, tsc_rx_swap));
    } else {
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_map_lanes(&core->access, CPRI_NOF_LANES_IN_CORE, pmd_tx_addr, pmd_rx_addr));
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _cpri_firmware_core_config_set(const phymod_phy_access_t* phy, phymod_firmware_core_config_t fw_core_config)
{
    struct falcon2_monterey_uc_core_config_st serdes_firmware_core_config;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&serdes_firmware_core_config, 0, sizeof(serdes_firmware_core_config));
    serdes_firmware_core_config.field.core_cfg_from_pcs = fw_core_config.CoreConfigFromPCS;
    serdes_firmware_core_config.field.vco_rate = fw_core_config.VcoRate;
    serdes_firmware_core_config.field.disable_write_pll_iqp = fw_core_config.disable_write_pll_iqp;
    _SOC_IF_ERR_EXIT(falcon2_monterey_set_uc_core_config(&phy->access, serdes_firmware_core_config));

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _cpri_pmd_pll_configure (phymod_access_t* pc, int pll_div,  phymod_ref_clk_t ref_clock)
{
    enum falcon2_monterey_pll_enum pll_cfg;
    SOC_INIT_FUNC_DEFS;

    switch (pll_div) {
        case phymod_TSCF_PLL_DIV160:
            pll_cfg = FALCON2_MONTEREY_pll_div_160x_vco2;
            break;
        case phymod_TSCF_PLL_DIV165:
            pll_cfg = FALCON2_MONTEREY_pll_div_165x_refc122;
            break;
        case phymod_TSCF_PLL_DIV198:
            pll_cfg = FALCON2_MONTEREY_pll_div_198x;
            break;
        case phymod_TSCF_PLL_DIV200:
            pll_cfg = FALCON2_MONTEREY_pll_div_200x;
            break;
        default:
            pll_cfg = FALCON2_MONTEREY_pll_div_160x_vco2;
            break;
    }

    _SOC_IF_ERR_EXIT
        (falcon2_monterey_configure_pll(pc, pll_cfg));

exit:
    SOC_FUNC_RETURN;
}

/*
 * Caculate vco rate in MHz.
 */
STATIC
int _cpri_pmd_pll_to_vco_rate(phymod_access_t* pc, int pll_div, phymod_ref_clk_t ref_clk, uint32_t* vco_rate)
{
    uint32_t pll_multiplier = 0;

    switch (pll_div){
        case phymod_TSCF_PLL_DIV160:
            pll_multiplier = 160;
            break;
        case phymod_TSCF_PLL_DIV165:
            pll_multiplier = 165;
            break;
        case phymod_TSCF_PLL_DIV198:
            pll_multiplier = 198;
            break;
        case phymod_TSCF_PLL_DIV200:
            pll_multiplier = 200;
            break;
        default:
            pll_multiplier = 160;
            break;
    }
   *vco_rate = pll_multiplier * 122 + pll_multiplier *88/100;

    return SOC_E_NONE;
}

/*
 * get fast clock bit time period based on VCO.
 */
STATIC
int _cpri_fast_clock_bit_time_period_get (int pll_div, uint32_t* bit_time_period)
{

    switch (pll_div){
        case phymod_TSCF_PLL_DIV160:
            *bit_time_period = 0x683;
            break;
        case phymod_TSCF_PLL_DIV165:
            *bit_time_period = 0x650;
            break;
        case phymod_TSCF_PLL_DIV198:
            *bit_time_period = 0x543;
            break;
        case phymod_TSCF_PLL_DIV200:
            *bit_time_period = 0x535;
            break;
        default:
            *bit_time_period = 0x683;
            break;
    }
    return SOC_E_NONE;
}

STATIC
int _cpri_core_init_pass2(int unit, int port, const phymod_core_access_t* core,
                          const phymod_core_init_config_t* init_config,
                          const phymod_core_status_t* core_status)
{
    phymod_phy_access_t  phy_access;
    phymod_phy_access_t  phy_access_copy;
    phymod_core_access_t  core_copy;
    phymod_firmware_core_config_t  firmware_core_config_tmp;
    uint32_t vco_rate;
    uint32_t fast_clock_bit_time_period = 0;

    SOC_INIT_FUNC_DEFS;
    sal_memcpy(&phy_access.access, &core->access, sizeof(phy_access.access));
    sal_memcpy(&core_copy, core, sizeof(core_copy));

    phy_access.type           = core->type; \
    phy_access.port_loc       = core->port_loc; \
    phy_access.device_op_mode = core->device_op_mode; \
    phy_access.access.lane_mask = 0x1;

    core_copy.access.lane_mask = 0x1;
    phy_access_copy = phy_access;
    phy_access_copy.access = core->access;
    phy_access_copy.access.lane_mask = 0x1;
    phy_access_copy.type = core->type;

#ifndef CPRI_PMD_CRC_UCODE_VERIFY
    if (init_config->firmware_load_method != phymodFirmwareLoadMethodNone) {
        int rv;
        rv = falcon2_monterey_ucode_load_verify(&core_copy.access, (uint8_t *) &falcon_dpll_ucode, falcon_dpll_ucode_len);
        if (rv != SOC_E_NONE) {
            /*_SOC_EXIT_WITH_ERR(SOC_E_FAIL, ("devad 0x%x lane 0x%x: UC load-verify failed\n", core->access.addr, core->access.lane_mask));*/
            SOC_IF_ERROR_RETURN(rv);
        }
    }
#endif

    /* cpri lane swap */
    SOC_IF_ERROR_RETURN
        (_cpri_core_lane_map_set(unit, port, &core_copy, &init_config->lane_map, 1));

    if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
#ifndef CPRI_PMD_CRC_UCODE_VERIFY
        /* poll the ready bit in 10 ms */
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_poll_uc_dsc_ready_for_cmd_equals_1(&phy_access_copy.access, 100));

#else
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_ucode_crc_verify(&core_copy.access, falcon_dpll_ucode_len, falcon_dpll_ucode_crc));
#endif
    }

        _SOC_IF_ERR_EXIT
            (falcon2_monterey_pmd_ln_h_rstb_pkill_override(&phy_access_copy.access, 0x0));

    /* set charge pump current */
    if (init_config->afe_pll.afe_pll_change_default){
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_afe_pll_reg_set(&core_copy.access, &init_config->afe_pll));
        firmware_core_config_tmp.disable_write_pll_iqp = 1;
    } else {
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_afe_pll_reg_set(&core_copy.access, &init_config->afe_pll));
        firmware_core_config_tmp.disable_write_pll_iqp = 0;
    }

    /* PLL configuration */
    phy_access_copy.access.lane_mask = 0x1;
    firmware_core_config_tmp = init_config->firmware_core_config;
    firmware_core_config_tmp.CoreConfigFromPCS = 0;


    /* reset core DP */
    core_copy.access.pll_idx = 0;
    _SOC_IF_ERR_EXIT
        (falcon2_monterey_core_soft_reset_release(&core_copy.access, 0));

    falcon2_monterey_write_cip_clk_div(&core_copy.access, 2);

    if (init_config->pll1_div_init_value) {
        core_copy.access.pll_idx = 1;
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_core_soft_reset_release(&core_copy.access, 0));
        falcon2_monterey_write_cip_clk_div(&core_copy.access, 2);
    }
    /* PLL0 config */
    phy_access_copy.access.pll_idx = 0;

    SOC_IF_ERROR_RETURN
        (_cpri_pmd_pll_configure(&phy_access_copy.access, init_config->pll0_div_init_value, init_config->interface.ref_clock));
    SOC_IF_ERROR_RETURN
        (_cpri_pmd_pll_to_vco_rate(&phy_access_copy.access, init_config->pll0_div_init_value, init_config->interface.ref_clock, &vco_rate));
    firmware_core_config_tmp.VcoRate = MHZ_TO_VCO_RATE(vco_rate);
    SOC_IF_ERROR_RETURN
        (_cpri_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));

    /* 
     * match the PLL0 setting with fclk_bit_timer_period.    
     */
    SOC_IF_ERROR_RETURN
        (_cpri_fast_clock_bit_time_period_get(init_config->pll0_div_init_value, &fast_clock_bit_time_period));

    SOC_IF_ERROR_RETURN
        (cprif_drv_fast_clk_bit_time_period_set(unit, port, fast_clock_bit_time_period)); 

    /* PLL1 config */
    if (init_config->pll1_div_init_value) {
        phy_access_copy.access.pll_idx = 1;
        SOC_IF_ERROR_RETURN
            (_cpri_pmd_pll_configure(&phy_access_copy.access, init_config->pll1_div_init_value, init_config->interface.ref_clock));
        SOC_IF_ERROR_RETURN
            (_cpri_pmd_pll_to_vco_rate(&phy_access_copy.access, init_config->pll1_div_init_value, init_config->interface.ref_clock, &vco_rate));

        SOC_IF_ERROR_RETURN
            (_cpri_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));
    }
    /* Release core DP soft reset */
    core_copy.access.pll_idx = 0;
    _SOC_IF_ERR_EXIT
        (falcon2_monterey_core_soft_reset_release(&core_copy.access, 1));

    if (init_config->pll1_div_init_value) {
        core_copy.access.pll_idx = 1;
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_core_soft_reset_release(&core_copy.access, 1));
    }

exit:
    SOC_FUNC_RETURN;

}

STATIC
int _cpri_core_init_pass1(int unit, int port, const phymod_core_access_t* core,
                          const phymod_core_init_config_t* init_config,
                          const phymod_core_status_t* core_status)
{
    uint32_t uc_enable = 0;
    phymod_phy_access_t phy_access;
    phymod_core_access_t  core_copy;
    SOC_INIT_FUNC_DEFS;

    sal_memcpy(&phy_access.access, &core->access, sizeof(phy_access.access));
    sal_memcpy(&core_copy, core, sizeof(core_copy));

    phy_access.type           = core->type; \
    phy_access.port_loc       = core->port_loc; \
    phy_access.device_op_mode = core->device_op_mode; \
    phy_access.access.lane_mask = 0xf;

    core_copy.access.lane_mask = 0x1;

    /* De-assert PMD core power and core data path reset */
    SOC_IF_ERROR_RETURN(cprif_drv_pmd_reset_seq(unit, port, core_status->pmd_active));

    SOC_IF_ERROR_RETURN(tefmod_gen3_pmd_reset_seq(&core_copy.access, core_status->pmd_active));

    _SOC_IF_ERR_EXIT
        (falcon2_monterey_uc_active_get(&phy_access.access, &uc_enable));
    if (uc_enable) return SOC_E_NONE;

    /* PMD lane swap */
    SOC_IF_ERROR_RETURN
        (_cpri_core_lane_map_set(unit, port, &core_copy, &init_config->lane_map, 0));

    /*  Micro code load */
    _rv = _cpri_core_firmware_load(&core_copy, init_config->firmware_load_method, init_config->firmware_loader);
    if (_rv != SOC_E_NONE) {
        _SOC_EXIT_WITH_ERR(SOC_E_CONFIG, ("devad 0x%"PRIx32" lane 0x%"PRIx32": UC firmware-load failed\n", core->access.addr, core->access.lane_mask));
    }
    /* pmd lane hard reset */
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_pmd_ln_h_rstb_pkill_override(&core_copy.access, 0x1));

    /* Set uc_active = 1 */
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_uc_active_set(&core_copy.access, 1));

    /* De-assert micro reset */
    PHYMOD_IF_ERR_RETURN
        (falcon2_monterey_uc_reset(&core_copy.access, 0));

    if (PHYMOD_CORE_INIT_F_FIRMWARE_LOAD_VERIFY_GET(init_config)) {
#ifdef TSCF_GEN3_PMD_CRC_UCODE_VERIFY
        PHYMOD_IF_ERR_RETURN(
            falcon2_monterey_start_ucode_crc_calc(&core_copy.access, falcon_dpll_ucode_len));
#endif
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_core_init(int unit, int port, const phymod_core_access_t* core,
                    const phymod_core_init_config_t* init_config,
                    phymod_core_status_t* core_status)
{
    phymod_phy_access_t phy_access;
    SOC_INIT_FUNC_DEFS;

    /* remove POR H reset */

    SOC_IF_ERROR_RETURN(cprif_drv_cip_top_ctrl_set(unit, port, 0));
    SOC_IF_ERROR_RETURN(cprif_drv_pmd_ctrl_por_h_rstb_set(unit, port, 0));

    if ((!CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
         !CPRIMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
          CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
        SOC_IF_ERROR_RETURN
            (_cpri_core_init_pass1(unit, port, core, init_config, core_status));

        if (CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config)) {
            return SOC_E_NONE;
        }
    }

    if ( (!CPRIMOD_CORE_INIT_F_EXECUTE_PASS1_GET(init_config) &&
          !CPRIMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) ||
          CPRIMOD_CORE_INIT_F_EXECUTE_PASS2_GET(init_config)) {
        SOC_IF_ERROR_RETURN
            (_cpri_core_init_pass2(unit, port, core, init_config, core_status));
    }

    /* PLL programming */
    if (init_config->pll0_div_init_value) {
        /* PLL is CPRI */
        SOC_IF_ERROR_RETURN(cprif_drv_top_ctrl_config_pll_ctrl_set(unit, port, 0, 1));
        /* enable reg clk in and ref clk out */
        SOC_IF_ERROR_RETURN(cprif_drv_top_ctrl_config_ref_clk_ctrl_set(unit, port, 0, 1, 1));
        /* Set lcref_sel */
        SOC_IF_ERROR_RETURN(cprif_drv_cpmport_pll_ctrl_config_set(unit, port, 0, 0));

        sal_memcpy(&phy_access.access, &core->access, sizeof(phy_access.access));
        phy_access.access.lane_mask = 0x1;
        /* need to set the heart beat  */
        _SOC_IF_ERR_EXIT(falcon2_monterey_pll_heart_beat_set(&phy_access.access, 0));

        /*
         * init pll0
         */
        SOC_IF_ERROR_RETURN(cprif_drv_pmd_core_datapath_hard_reset_set(unit, port, 0, 1));
        /* wait for pll to lock */
    }
    if (init_config->pll1_div_init_value) {
        /* PLL is CPRI */
        SOC_IF_ERROR_RETURN(cprif_drv_top_ctrl_config_pll_ctrl_set(unit, port, 1, 1));
        /* enable reg clk in and ref clk out */
        SOC_IF_ERROR_RETURN(cprif_drv_top_ctrl_config_ref_clk_ctrl_set(unit, port, 1, 1, 1));
        /* Set lcref_sel */
        SOC_IF_ERROR_RETURN(cprif_drv_cpmport_pll_ctrl_config_set(unit, port, 1, 1));
        /*
         * init pll1
         */
        SOC_IF_ERROR_RETURN(cprif_drv_pmd_core_datapath_hard_reset_set(unit, port, 1, 1));
        /* wait for pll to lock */
    }
    /*
     * tsc_clk is always source from external. even if all cpri port,
     * selecting external will not cause any issue.
     */
    SOC_IF_ERROR_RETURN(cprif_drv_tsc_clk_ext_select(unit, port, 1));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_pll_status_get(int unit, int port,
                              const phymod_phy_access_t* phy,
                              int* pll_sel, int* pll_lock)
{
    uint8 pll_sel8, pll_lock8;
    phymod_phy_access_t  phy_access_copy;
    SOC_INIT_FUNC_DEFS;

    sal_memcpy(&phy_access_copy, phy, sizeof(phy_access_copy));
    falcon2_monterey_pll_select_get(&phy->access, &pll_sel8);
    *pll_sel = pll_sel8;
    if(*pll_sel == 1) {
        phy_access_copy.access.pll_idx = 1;
    } else {
        phy_access_copy.access.pll_idx = 0;
    }
    falcon2_monterey_pll_lock_get(&phy_access_copy.access, &pll_lock8);
    *pll_lock = pll_lock8;

    SOC_FUNC_RETURN;

}

static int _cprif_container_config_clear (int unit, int port, cprimod_port_interface_type_t interface)
{
    int axc_id=0;
    SOC_INIT_FUNC_DEFS;

    if (interface == cprimodCpri) { 
        for ( axc_id = 0; axc_id < CPRIF_MAX_NUM_OF_AXC; axc_id++) {
            _SOC_IF_ERR_EXIT
                (cprif_cpri_rx_axc_container_config_clear(unit, port, axc_id));
            _SOC_IF_ERR_EXIT
                (cprif_cpri_tx_axc_container_config_clear(unit, port, axc_id));
        }
    } else if (interface == cprimodRsvd4) {
        for ( axc_id = 0; axc_id < CPRIF_MAX_NUM_OF_AXC; axc_id++) {
            _SOC_IF_ERR_EXIT
                (cprif_rsvd4_rx_axc_container_config_clear(unit, port, axc_id));
            _SOC_IF_ERR_EXIT
                (cprif_rsvd4_tx_axc_container_config_clear(unit, port, axc_id));
        }
    }
exit:
    SOC_FUNC_RETURN;
}


int cprif_port_init(int unit, int port, const phymod_phy_access_t* phy,
                    const cprimod_port_init_config_t* port_config)
{
    uint32 wd_len;
    uint32 pll_mode0;
    uint32 pll_mode1;
    uint32_t vco_rate;
    phymod_firmware_core_config_t  firmware_core_config_tmp;
    phymod_phy_access_t  phy_access_copy;
    struct falcon2_monterey_uc_lane_config_st serdes_firmware_config;
    phymod_tx_t tx_afe;
    cprimod_basic_frame_table_id_t table;
    uint32 fast_clock_bit_time_period = 0;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&tx_afe, 0, sizeof(phymod_tx_t));
    sal_memcpy(&phy_access_copy, phy, sizeof(phy_access_copy));
    phy_access_copy.access.pll_idx = 0;
    falcon2_monterey_pll_mode_get(&phy_access_copy.access, &pll_mode0);
    phy_access_copy.access.pll_idx = 1;
    falcon2_monterey_pll_mode_get(&phy_access_copy.access, &pll_mode1);
    SOC_IF_ERROR_RETURN(cprif_drv_pmd_ctrl_ln_por_h_rstb_set(unit, port));
    /* PLL0 is selected for cpri for now Need to change when new speeds are supported */
    falcon2_monterey_lane_soft_reset_release(&phy->access, 0);
    /* pll_in_use tells which plls are in use, check if PLL0 is used (enabled) And
       if vco required is vco of pll0 then use pll0 for the lane */
    if (((port_config->pll_in_use & cprimodInUsePll0) != 0) &&
       (port_config->vco_for_lane == pll_mode0)) {
        falcon2_monterey_pll_select_set(&phy->access, 0);
        SOC_IF_ERROR_RETURN(cprif_drv_cip_clk_pll_select_set(unit, port, 0));
    /* pll_in_use tells which plls are in use, check if PLL1 is used (enabled) And
       if vco required is vco of pll1 then use pll1 for the lane */
    } else if (((port_config->pll_in_use & cprimodInUsePll1) != 0) &&
              (port_config->vco_for_lane == pll_mode1)) {
        falcon2_monterey_pll_select_set(&phy->access, 1);
        SOC_IF_ERROR_RETURN(cprif_drv_cip_clk_pll_select_set(unit, port, 1));
    } else {
        /* When required vco does not match with existing pll vco try to use pll that is
          not used by other lanes */
        if ((port_config->pll_in_use & cprimodInUsePll0) == 0) {
            /* if pll0 is not used by other lanes bringup pll0 for new vco */
            falcon2_monterey_pll_select_set(&phy->access, 0);
            phy_access_copy.access.pll_idx = 0;
            SOC_IF_ERROR_RETURN(cprif_drv_cip_clk_pll_select_set(unit, port, 0));
        } else {
            falcon2_monterey_pll_select_set(&phy->access, 1);
            phy_access_copy.access.pll_idx = 1;
            SOC_IF_ERROR_RETURN(cprif_drv_cip_clk_pll_select_set(unit, port, 1));
        }
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_core_soft_reset_release(&phy_access_copy.access, 0));
        SOC_IF_ERROR_RETURN
            (_cpri_pmd_pll_configure(&phy_access_copy.access, port_config->vco_for_lane, 0));
        SOC_IF_ERROR_RETURN
            (_cpri_pmd_pll_to_vco_rate(&phy_access_copy.access,
                                       port_config->vco_for_lane, 0, &vco_rate));

        /*
         * match the PLL0 setting with fclk_bit_timer_period.
         */
        if (phy_access_copy.access.pll_idx == 0) {
            _SOC_IF_ERR_EXIT 
                (_cpri_fast_clock_bit_time_period_get(port_config->vco_for_lane, &fast_clock_bit_time_period));

            _SOC_IF_ERR_EXIT
                (cprif_drv_fast_clk_bit_time_period_set(unit, port, fast_clock_bit_time_period));
        }

        firmware_core_config_tmp.disable_write_pll_iqp = 0;
        firmware_core_config_tmp.CoreConfigFromPCS = 0;
        firmware_core_config_tmp.VcoRate = MHZ_TO_VCO_RATE(vco_rate);

        SOC_IF_ERROR_RETURN
            (_cpri_firmware_core_config_set(&phy_access_copy, firmware_core_config_tmp));
        _SOC_IF_ERR_EXIT
            (falcon2_monterey_core_soft_reset_release(&phy_access_copy.access, 1));

    }


    serdes_firmware_config.field.lane_cfg_from_pcs = 0;
    serdes_firmware_config.field.an_enabled        = 0;
    serdes_firmware_config.field.dfe_on            = 1;
    serdes_firmware_config.field.dfe_lp_mode       = 0;
    serdes_firmware_config.field.force_brdfe_on    = 0;
    serdes_firmware_config.field.media_type        = phymodFirmwareMediaTypeOptics;
    serdes_firmware_config.field.unreliable_los    = 1;
    if((port_config->rx_cpri_speed == cprimodSpd10137p6)||
      (port_config->rx_cpri_speed == cprimodSpd12165p12)||
      (port_config->rx_cpri_speed == cprimodSpd24330p24)) {
        serdes_firmware_config.field.scrambling_dis    = 0;
    } else {
        serdes_firmware_config.field.scrambling_dis    = 1;
    }
    serdes_firmware_config.field.cl72_auto_polarity_en   = 0;
    serdes_firmware_config.field.cl72_restart_timeout_en = 0;
    serdes_firmware_config.word = 0;
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_set_uc_lane_cfg(&phy->access, serdes_firmware_config));
    falcon2_monterey_lane_soft_reset_release(&phy->access, 1);

    SOC_IF_ERROR_RETURN(cprif_drv_tx_h_reset_set(unit, port, cpriResetEnAssert));
    SOC_IF_ERROR_RETURN(cprif_drv_rx_h_reset_set(unit, port, cpriResetEnAssert));

    SOC_IF_ERROR_RETURN(cprif_drv_cip_tx_h_reset_set(unit, port,
                                                     cpriResetEnAssert));
    SOC_IF_ERROR_RETURN(cprif_drv_cip_rx_h_reset_set(unit, port,
                                                     cpriResetEnAssert));

    SOC_IF_ERROR_RETURN(cprif_drv_datapath_tx_h_reset_set(unit, port,
                                                          cpriResetEnAssert));
    SOC_IF_ERROR_RETURN(cprif_drv_datapath_rx_h_reset_set(unit, port,
                                                          cpriResetEnAssert));

    SOC_IF_ERROR_RETURN(cprif_drv_tx_h_reset_set(unit, port,
                                                 cpriResetEnDeassert));
    SOC_IF_ERROR_RETURN(cprif_drv_rx_h_reset_set(unit, port,
                                                 cpriResetEnDeassert));

    SOC_IF_ERROR_RETURN(cprif_drv_cip_tx_h_reset_set(unit, port,
                                                     cpriResetEnDeassert));
    SOC_IF_ERROR_RETURN(cprif_drv_cip_rx_h_reset_set(unit, port,
                                                     cpriResetEnDeassert));

    SOC_IF_ERROR_RETURN(cprif_drv_datapath_tx_h_reset_set(unit, port,
                                                          cpriResetEnDeassert));
    SOC_IF_ERROR_RETURN(cprif_drv_datapath_rx_h_reset_set(unit, port,
                                                          cpriResetEnDeassert));
    /* remove POR H reset */
    /* SOC_IF_ERROR_RETURN(cprif_drv_pmd_ctrl_por_h_rstb_set(unit, port, 0)); */

    /*
     * set polarity port_config->polarity
     * set other per lane PMD setting
     */
    falcon2_monterey_tx_rx_polarity_set(&phy->access, port_config->polarity_tx, port_config->polarity_rx);
    /*
     * Check speed if current VCO will support it or through error
     */
    falcon2_monterey_lane_soft_reset_release(&phy->access, 0);
    if (port_config->interface == cprimodCpri) {
        SOC_IF_ERROR_RETURN(cprif_drv_tx_frame_optional_config_set(unit,
                                                                   port,
                                                                   cprimodTxConfigseed8B10B,
                                                                   port_config->serdes_scrambler_seed));
        SOC_IF_ERROR_RETURN(cprif_drv_tx_frame_optional_config_set(unit,
                                                                   port,
                                                                   cprimodTxConfigcwaScrEn8b10b,
                                                                   port_config->serdes_scrambler_enable));
        SOC_IF_ERROR_RETURN(cprif_drv_rx_frame_optional_config_set(unit,
                                                                   port,
                                                                   cprimodRxConfig8b10bDescrEn,
                                                                   port_config->serdes_scrambler_enable));

        SOC_IF_ERROR_RETURN(_cprif_port_tx_speed_set(unit, port, phy,
                                                           port_config->tx_cpri_speed));
        SOC_IF_ERROR_RETURN(_cprif_port_rx_speed_set(unit, port, phy,
                                                           port_config->rx_cpri_speed));
    } else {
        SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_frame_optional_config_set(unit,
                                                                         port,
                                                                         CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_SEED,
                                                                         port_config->serdes_scrambler_seed));
        SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_tx_frame_optional_config_set(unit,
                                                                         port,
                                                                         CPRIMOD_RSVD4_TX_CONFIG_SCRAMBLE_ENABLE,
                                                                         port_config->serdes_scrambler_enable));
        SOC_IF_ERROR_RETURN(cprif_drv_rsvd4_rx_frame_optional_config_set(unit,
                                                                         port,
                                                                         CPRIMOD_RSVD4_RX_CONFIG_DESCRAMBLE_ENABLE,
                                                                         port_config->serdes_scrambler_enable));

        SOC_IF_ERROR_RETURN(cprif_port_rsvd4_tx_speed_set(unit, port, phy,
                                                            port_config->tx_rsvd4_speed));
        SOC_IF_ERROR_RETURN(cprif_port_rsvd4_rx_speed_set(unit, port, phy,
                                                            port_config->rx_rsvd4_speed));
    }
    falcon2_monterey_lane_soft_reset_release(&phy->access, 1);

    if (port_config->interface == cprimodCpri) {
        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_parser_active_table_get(unit, port,&table));
        if (table == cprimod_basic_frame_table_0) {
            _SOC_IF_ERR_EXIT
                (cprif_basic_frame_parser_active_table_set(unit, port,cprimod_basic_frame_table_1, TRUE));
        } else {
            _SOC_IF_ERR_EXIT
                (cprif_basic_frame_parser_active_table_set(unit, port,cprimod_basic_frame_table_0, TRUE));
        }
        _SOC_IF_ERR_EXIT
            (cprif_basic_frame_assembly_active_table_get(unit, port,&table));
        if (table == cprimod_basic_frame_table_0) {
            _SOC_IF_ERR_EXIT
                (cprif_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_1, TRUE));
        } else {
            _SOC_IF_ERR_EXIT
                (cprif_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_0, TRUE));
        }
    } else {
        _SOC_IF_ERR_EXIT
            (cprif_basic_frame_parser_switch_active_table(unit, port));

        _SOC_IF_ERR_EXIT
            (cprif_basic_frame_assembly_switch_active_table(unit, port));
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_stuff_rsrv_bits_set(unit, port, port_config->roe_stuffing_bit, port_config->roe_reserved_bit));
    _SOC_IF_ERR_EXIT(cprif_drv_glas_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_cip_rx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_rx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_rx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_credit_reset_set(unit, port, 1));
    /* remove soft resets Rx path*/
    _SOC_IF_ERR_EXIT(cprif_drv_glas_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_cip_rx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_rx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_rx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_credit_reset_set(unit, port, 0));
    /* remove soft resets Tx path*/
    _SOC_IF_ERR_EXIT(cprif_drv_cip_tx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_tx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_tx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_cip_tx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_tx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_tx_dp_reset_set(unit, port, 0));
    if (port_config->interface == cprimodCpri) {
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));
        wd_len = _cprif_from_speed_towd_len(port_config->rx_cpri_speed);
        /* Adding AXC for control word */
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, 255,
                                                      0, wd_len));
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));
        wd_len = _cprif_from_speed_towd_len(port_config->tx_cpri_speed);
        /* Adding AXC for control word */
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 255,
                                                      0, wd_len));
    } else {
        SOC_IF_ERROR_RETURN
            (_cprif_port_rsvd4_basic_frame_table_set(unit, port, CPRIMOD_DIR_RX));
        SOC_IF_ERROR_RETURN
            (_cprif_port_rsvd4_basic_frame_table_set(unit, port, CPRIMOD_DIR_TX));
    }
    /* 
     * clear the container parser and assembler table, in such a way that it 
     * will never sync.
     */
    _SOC_IF_ERR_EXIT
        (_cprif_container_config_clear(unit, port, port_config->interface)); 

    /* Enable CPRI Port Link Status Interrupts for HW Linkscan handling */

    /*
     * This will not be enable for HW linkscan as there is already another
     * Link status interrupt going to the CMIC.  CMIC_MIIM_LINK_STATUS_0
     * and CMIC_MIIM_LINK_STATUS_1.
     */
#if 0
    if (port_config->interface == cprimodCpri) {
        _SOC_IF_ERR_EXIT(_cprif_cpri_port_link_event_intr_enable_set(unit,
                                                                     port, 1));
    }
#endif

    /* Enable ECC interrupts if parity is enabled */
    if (port_config->parity_enable) {
        cprimod_cpri_port_ecc_interrupt_enable_set(unit, port, 1);
    }

    /*
     * Enable all the interrupts in the interrupts hierarchy during
     * the init except the actual interrupts, which need
     * to enabled using the interrupt enable API
     */
    _SOC_IF_ERR_EXIT(_cprif_cpri_port_intr_hierarchy_enable(unit, port, 1));

    /* Program TX AFE setting */
    SOC_IF_ERROR_RETURN
        (phymod_phy_media_type_tx_get(phy, phymodMediaTypeChipToChip, &tx_afe));
    if (port_config->tx_params_user_config) {
         tx_afe.pre = port_config->tx_params[0].pre;
         tx_afe.main = port_config->tx_params[0].main;
         tx_afe.post = port_config->tx_params[0].post;
    }
    SOC_IF_ERROR_RETURN
        (phymod_phy_tx_set(phy, &tx_afe));
    /* Set time period for Tx and Rx timestamping */
    SOC_IF_ERROR_RETURN(cprimod_1588_bit_time_period_set(unit, port, CPRIMOD_DIR_TX, port_config->tx_cpri_speed));
    SOC_IF_ERROR_RETURN(cprimod_1588_bit_time_period_set(unit, port, CPRIMOD_DIR_RX, port_config->rx_cpri_speed));

    /* 
     * Enable FEC error bypass indication. This apply only to CPRI 24.33G. 
     * No affect on other speeds. 
     */
    SOC_IF_ERROR_RETURN
        (cprif_drv_cpri_port_fec_aux_config_set(unit, port, 
                                                CPRIMOD_CPRI_FEC_AUX_CONFIG_BYPASS_INDICATION_ENABLE,
                                                1));
exit:
    SOC_FUNC_RETURN;
}


int cprif_rsvd4_rx_master_frame_sync_config_set(int unit, int port, uint32 master_frame_number, uint64 master_frame_start_time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_master_frame_sync_config_set(unit, port,
                                                    master_frame_number,
                                                    master_frame_start_time));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_rx_master_frame_sync_config_get(int unit, int port, uint32* master_frame_number, uint64* master_frame_start_time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_master_frame_sync_config_get(unit, port,
                                                    master_frame_number,
                                                    master_frame_start_time));


exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_control_word_sync_info_get(int unit, int port, uint32* hyper_frame_num, uint32* radio_frame_num)
{
    cprif_drv_rx_control_word_status_t cw_info;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&cw_info, 0, sizeof(cprif_drv_rx_control_word_status_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_control_word_status_get(unit, port, &cw_info));

    *hyper_frame_num    =  cw_info.hyper_frame;
    *radio_frame_num    =  cw_info.radio_frame;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_control_word_l1_inband_info_get(int unit, int port, cprimod_cpri_cw_l1_inband_info_t* l1_inband_info)
{
    cprif_drv_rx_control_word_status_t cw_info;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&cw_info, 0, sizeof(cprif_drv_rx_control_word_status_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_control_word_status_get(unit, port, &cw_info));

    l1_inband_info->hdlc_rate      = cw_info.hdlc_rate;
    l1_inband_info->protocol_ver   = cw_info.protocol_ver;
    l1_inband_info->eth_ptr        = cw_info.eth_pointer;
    if (cw_info.reset) {
        CPRIMOD_L1_FUNCTION_RESET_SET(l1_inband_info->layer1_function);
    } else {
        CPRIMOD_L1_FUNCTION_RESET_CLR(l1_inband_info->layer1_function);
    }
    if (cw_info.l1_rai) {
        CPRIMOD_L1_FUNCTION_RAI_SET(l1_inband_info->layer1_function);
    } else {
        CPRIMOD_L1_FUNCTION_RAI_CLR(l1_inband_info->layer1_function);
    }   
    if (cw_info.l1_sdi) {
        CPRIMOD_L1_FUNCTION_SDI_SET(l1_inband_info->layer1_function);
    } else {
        CPRIMOD_L1_FUNCTION_SDI_CLR(l1_inband_info->layer1_function);
    }
    if (cw_info.l1_los) {
        CPRIMOD_L1_FUNCTION_LOS_SET(l1_inband_info->layer1_function);
    } else {
        CPRIMOD_L1_FUNCTION_LOS_CLR(l1_inband_info->layer1_function);
    }
    if (cw_info.l1_lof) {
        CPRIMOD_L1_FUNCTION_LOF_SET(l1_inband_info->layer1_function);
    } else {
        CPRIMOD_L1_FUNCTION_LOF_CLR(l1_inband_info->layer1_function);
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_control_word_l1_signal_signal_protection_set(int unit, int port, uint32 signal_map, uint32 enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_rx_l1_signal_signal_protection_set(unit, port, signal_map, enable));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_rx_control_word_l1_signal_signal_protection_get(int unit, int port, uint32 signal_map, uint32* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_rx_l1_signal_signal_protection_get(unit, port, signal_map, enable));

exit:
    SOC_FUNC_RETURN;

}

static
void _cprimod_to_cprif_fcs_size ( cprimod_cpri_hdlc_fcs_size_t cprimod_fcs_size, uint32* cprif_fcs_size)
{
    switch (cprimod_fcs_size) {
        case CPRIMOD_CPRI_HDLC_FCS_SIZE_8_BITS:
            *cprif_fcs_size = CPRIF_HDLC_FCS_SIZE_8_BITS;
            break;
        case CPRIMOD_CPRI_HDLC_FCS_SIZE_16_BITS:
            *cprif_fcs_size = CPRIF_HDLC_FCS_SIZE_16_BITS;
            break;
        case CPRIMOD_CPRI_HDLC_FCS_SIZE_32_BITS:
            *cprif_fcs_size = CPRIF_HDLC_FCS_SIZE_32_BITS;
            break;
        default:
            *cprif_fcs_size = CPRIF_HDLC_FCS_SIZE_8_BITS;
            break;
    }
}

static
void _cprif_to_cprimod_fcs_size ( uint32 cprif_fcs_size, cprimod_cpri_hdlc_fcs_size_t *cprimod_fcs_size)
{
    switch (cprif_fcs_size) {
        case CPRIF_HDLC_FCS_SIZE_8_BITS:
            *cprimod_fcs_size = CPRIMOD_CPRI_HDLC_FCS_SIZE_8_BITS;
            break;
        case CPRIF_HDLC_FCS_SIZE_16_BITS:
            *cprimod_fcs_size = CPRIMOD_CPRI_HDLC_FCS_SIZE_16_BITS;
            break;
        case CPRIF_HDLC_FCS_SIZE_32_BITS:
            *cprimod_fcs_size = CPRIMOD_CPRI_HDLC_FCS_SIZE_32_BITS;
            break;
        default:
            *cprimod_fcs_size = CPRIMOD_CPRI_HDLC_FCS_SIZE_8_BITS;
            break;
    }
}

static
void _cprimod_to_cprif_filling_flag_pattern(cprimod_cpri_hdlc_filling_flag_pattern_t cprimod_pattern, uint32* cprif_pattern)
{
    switch (cprimod_pattern) {

        case CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_7E:
            *cprif_pattern = CPRIF_HDLC_FILLING_FLAG_PATTERN_7E;
            break;
        case CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_7F:
            *cprif_pattern = CPRIF_HDLC_FILLING_FLAG_PATTERN_7F;
            break;
        case CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_FF:
            *cprif_pattern = CPRIF_HDLC_FILLING_FLAG_PATTERN_FF;
            break;
        default:
            *cprif_pattern = CPRIF_HDLC_FILLING_FLAG_PATTERN_7E;
            break;
    }
}

static
void _cprif_to_cprimod_filling_flag_pattern( uint32 cprif_pattern, cprimod_cpri_hdlc_filling_flag_pattern_t *cprimod_pattern)
{
    switch (cprif_pattern) {

        case CPRIF_HDLC_FILLING_FLAG_PATTERN_7E:
            *cprimod_pattern = CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_7E;
            break;
        case CPRIF_HDLC_FILLING_FLAG_PATTERN_7F:
            *cprimod_pattern = CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_7F;
            break;
        case CPRIF_HDLC_FILLING_FLAG_PATTERN_FF:
            *cprimod_pattern = CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_FF;
            break;
        default:
            *cprimod_pattern = CPRIMOD_CPRI_HDLC_FILLING_FLAG_PATTERN_7E;
            break;
    }
}

static
void _cprimod_to_cprif_hdlc_crc_mode( cprimod_cpri_crc_mode_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_CRC_MODE_APPEND:
            *cprif_mode = CPRIF_HDLC_CRC_MODE_APPEND;
            break;
        case CPRIMOD_CPRI_CRC_MODE_REPLACE:
            *cprif_mode = CPRIF_HDLC_CRC_MODE_REPLACE;
            break;
        case CPRIMOD_CPRI_CRC_MODE_NO_UPDATE:
            *cprif_mode = CPRIF_HDLC_CRC_MODE_NO_UPDATE;
            break;
        default:
            *cprif_mode = CPRIF_HDLC_CRC_MODE_APPEND;
    }
}

static
void _cprif_to_cprimod_hdlc_crc_mode( uint32 cprif_mode , cprimod_cpri_crc_mode_t* cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_HDLC_CRC_MODE_APPEND:
            *cprimod_mode = CPRIMOD_CPRI_CRC_MODE_APPEND ;
            break;
        case CPRIF_HDLC_CRC_MODE_REPLACE:
            *cprimod_mode = CPRIMOD_CPRI_CRC_MODE_REPLACE;
            break;
        case CPRIF_HDLC_CRC_MODE_NO_UPDATE:
            *cprimod_mode = CPRIMOD_CPRI_CRC_MODE_NO_UPDATE ;
            break;
        default:
            *cprimod_mode = CPRIMOD_CPRI_CRC_MODE_APPEND;
    }
}

int cprif_cpri_port_cw_slow_hdlc_config_set(int unit, int port, const cprimod_slow_hdlc_config_info_t* config_info)
{
    cprif_cpri_rx_hdlc_config_t rx_config;
    cprif_cpri_tx_hdlc_config_t tx_config;
    SOC_INIT_FUNC_DEFS;


    /****  RX Setting   ****/
    sal_memset(&rx_config, 0, sizeof(cprif_cpri_rx_hdlc_config_t));

    rx_config.cw_sel = config_info->cw_sel;
    rx_config.cw_size= config_info->cw_size;

    if (config_info->crc_byte_swap) {
        rx_config.crc_byte_swap= CPRIF_HDLC_CRC_BYTE_SWAP;
    } else {
        rx_config.crc_byte_swap= CPRIF_HDLC_CRC_BYTE_NO_SWAP;
    }

    if (config_info->crc_init_val == CPRIMOD_CPRI_HDLC_CRC_INIT_VAL_ALL_1) {
        rx_config.crc_init_val = CPRIF_HDLC_CRC_INIT_VAL_ALL_1S;
    } else {
        rx_config.crc_init_val = CPRIF_HDLC_CRC_INIT_VAL_ALL_0S;
    }

    rx_config.queue_num= config_info->queue_num;

    if (config_info->use_fe_mac) {
        rx_config.use_fe_mac= CPRIF_HDLC_USE_FE_MAC;

    } else {
        rx_config.use_fe_mac= CPRIF_HDLC_USE_HDLC;
    }

    if (config_info->no_fcs_err_check) {
        rx_config.ignore_fcs_err= CPRIF_HDLC_FCS_NO_CHECK;
    } else {
        rx_config.ignore_fcs_err= CPRIF_HDLC_FCS_CHECK;
    }
    _cprimod_to_cprif_fcs_size(config_info->fcs_size, &rx_config.fcs_size);

    rx_config.runt_frame_drop= config_info->runt_frame_drop ? 1 : 0;
    rx_config.long_frame_drop= config_info->long_frame_drop ? 1 : 0;
    rx_config.min_frame_size = config_info->min_frame_size;
    rx_config.max_frame_size = config_info->max_frame_size;;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_slow_hdlc_config_set(unit, port, &rx_config));

    /****  TX Setting   ****/

    sal_memset(&tx_config, 0, sizeof(cprif_cpri_tx_hdlc_config_t));

    tx_config.cw_sel = config_info->cw_sel;
    tx_config.cw_size= config_info->cw_size;
    _cprimod_to_cprif_filling_flag_pattern(config_info->tx_filling_flag_pattern, &tx_config.filling_flag_pattern);

    if (config_info->tx_flag_size == CPRIMOD_HDLC_FLAG_SIZE_1_BYTE) {
        tx_config.flag_size = CPRIF_HDLC_FLAG_SIZE_1_BYTE;
    } else if (config_info->tx_flag_size == CPRIMOD_HDLC_FLAG_SIZE_2_BYTE) {
        tx_config.flag_size = CPRIF_HDLC_FLAG_SIZE_2_BYTE;
    } else {

    }
    if (config_info->crc_byte_swap) {
        tx_config.crc_byte_swap= CPRIF_HDLC_CRC_BYTE_SWAP;
    } else {
        tx_config.crc_byte_swap= CPRIF_HDLC_CRC_BYTE_NO_SWAP;
    }

    if (config_info->crc_init_val == CPRIMOD_CPRI_HDLC_CRC_INIT_VAL_ALL_1) {
        tx_config.crc_init_val = CPRIF_HDLC_CRC_INIT_VAL_ALL_1S;
    } else {
        tx_config.crc_init_val = CPRIF_HDLC_CRC_INIT_VAL_ALL_0S;
    }

    tx_config.queue_num = config_info->queue_num;

    if (config_info->use_fe_mac) {
        tx_config.use_fe_mac= CPRIF_HDLC_USE_FE_MAC;

    } else {
        tx_config.use_fe_mac= CPRIF_HDLC_USE_HDLC;
    }

    _cprimod_to_cprif_fcs_size(config_info->fcs_size, &tx_config.fcs_size);
    _cprimod_to_cprif_hdlc_crc_mode(config_info->tx_crc_mode, &tx_config.crc_mode);

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_tx_cw_slow_hdlc_config_set(unit, port, &tx_config));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_cw_slow_hdlc_config_get(int unit, int port, cprimod_slow_hdlc_config_info_t* config_info)
{
    cprif_cpri_rx_hdlc_config_t rx_config;
    cprif_cpri_tx_hdlc_config_t tx_config;
    SOC_INIT_FUNC_DEFS;


    /****  RX Setting   ****/
    sal_memset(&rx_config, 0, sizeof(cprif_cpri_rx_hdlc_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_slow_hdlc_config_get(unit, port, &rx_config));

    config_info->cw_sel         = rx_config.cw_sel;
    config_info->cw_size        = rx_config.cw_size;
    config_info->crc_byte_swap  = (rx_config.crc_byte_swap == CPRIF_HDLC_CRC_BYTE_SWAP)?1:0;

    if (rx_config.crc_init_val == CPRIF_HDLC_CRC_INIT_VAL_ALL_1S) {
        config_info->crc_init_val = CPRIMOD_CPRI_HDLC_CRC_INIT_VAL_ALL_1;
    } else {
        config_info->crc_init_val = CPRIMOD_CPRI_HDLC_CRC_INIT_VAL_ALL_0;
    }

    config_info->queue_num = rx_config.queue_num ;

    config_info->use_fe_mac = (rx_config.use_fe_mac == CPRIF_HDLC_USE_FE_MAC )?1:0;

    config_info->no_fcs_err_check = (rx_config.ignore_fcs_err == CPRIF_HDLC_FCS_NO_CHECK)? 1:0;

    _cprif_to_cprimod_fcs_size (rx_config.fcs_size, &config_info->fcs_size);

    config_info->runt_frame_drop = rx_config.runt_frame_drop ? 1:0;
    config_info->long_frame_drop = rx_config.long_frame_drop ? 1:0;
    config_info->min_frame_size  = rx_config.min_frame_size;
    config_info->max_frame_size  = rx_config.max_frame_size;


    /****  TX Setting   ****/

    sal_memset(&tx_config, 0, sizeof(cprif_cpri_tx_hdlc_config_t));
    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_tx_cw_slow_hdlc_config_get(unit, port, &tx_config));

    config_info->cw_sel  = tx_config.cw_sel;
    config_info->cw_size = tx_config.cw_size;
    _cprif_to_cprimod_filling_flag_pattern(tx_config.filling_flag_pattern, &config_info->tx_filling_flag_pattern);

    if (tx_config.flag_size == CPRIF_HDLC_FLAG_SIZE_1_BYTE) {
        config_info->tx_flag_size = CPRIMOD_HDLC_FLAG_SIZE_1_BYTE;
    } else if (tx_config.flag_size == CPRIF_HDLC_FLAG_SIZE_2_BYTE) {
        config_info->tx_flag_size = CPRIMOD_HDLC_FLAG_SIZE_2_BYTE;
    }

    _cprif_to_cprimod_hdlc_crc_mode(tx_config.crc_mode,&config_info->tx_crc_mode);

exit:
    SOC_FUNC_RETURN;
}

static
void _cprimod_to_cprif_fast_eth_crc_mode( cprimod_cpri_crc_mode_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_CRC_MODE_APPEND:
            *cprif_mode = CPRIF_FAST_ETH_CRC_MODE_APPEND;
            break;
        case CPRIMOD_CPRI_CRC_MODE_REPLACE:
            *cprif_mode = CPRIF_FAST_ETH_CRC_MODE_REPLACE;
            break;
        case CPRIMOD_CPRI_CRC_MODE_NO_UPDATE:
            *cprif_mode = CPRIF_FAST_ETH_CRC_MODE_NO_UPDATE;
            break;
        default:
            *cprif_mode = CPRIF_FAST_ETH_CRC_MODE_APPEND;
            break;
    }
}

static
void _cprif_to_cprimod_fast_eth_crc_mode( uint32 cprif_mode, cprimod_cpri_crc_mode_t* cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_FAST_ETH_CRC_MODE_APPEND:
            *cprimod_mode = CPRIMOD_CPRI_CRC_MODE_APPEND;
            break;
        case CPRIF_FAST_ETH_CRC_MODE_REPLACE:
            *cprimod_mode = CPRIMOD_CPRI_CRC_MODE_REPLACE;
            break;
        case CPRIF_FAST_ETH_CRC_MODE_NO_UPDATE:
            *cprimod_mode = CPRIMOD_CPRI_CRC_MODE_NO_UPDATE;
            break;
        default:
            *cprimod_mode = CPRIMOD_CPRI_CRC_MODE_APPEND;
            break;
    }
}

int cprif_cpri_port_cw_fast_eth_config_set(int unit, int port, const cprimod_fast_eth_config_info_t* config_info)
{
    cprif_cpri_rx_fast_eth_config_t rx_config;
    cprif_cpri_rx_fast_eth_word_config_t rx_word_config;
    cprif_cpri_tx_fast_eth_config_t tx_config;
    SOC_INIT_FUNC_DEFS;


    sal_memset(&rx_config, 0, sizeof(cprif_cpri_rx_fast_eth_config_t));
    sal_memset(&rx_word_config, 0, sizeof(cprif_cpri_rx_fast_eth_word_config_t));

    rx_word_config.sub_channel_start     = config_info->schan_start ;
    rx_word_config.sub_channel_size      = config_info->schan_size ;
    rx_word_config.cw_sel                = config_info->cw_sel ;
    rx_word_config.cw_size               = config_info->cw_size ;
    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_fast_eth_word_config_set(unit, port, &rx_word_config));

    rx_config.queue_num             = config_info->queue_num ;
    rx_config.ignore_fcs_err        = config_info->no_fcs_err_check ;
    rx_config.min_packet_drop       = config_info->min_packet_drop ;
    rx_config.max_packet_drop       = config_info->max_packet_drop ;
    rx_config.min_packet_size       = config_info->min_packet_size ;
    rx_config.max_packet_size       = config_info->max_packet_size ;
    rx_config.strip_crc             = config_info->strip_crc;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_fast_eth_config_set(unit, port, &rx_config));

    sal_memset(&tx_config, 0, sizeof(cprif_cpri_tx_fast_eth_config_t));
    tx_config.sub_channel_start     = config_info->schan_start ;
    tx_config.sub_channel_size      = config_info->schan_size ;
    tx_config.cw_sel                = config_info->cw_sel ;
    tx_config.cw_size               = config_info->cw_size ;
    tx_config.queue_num             = config_info->queue_num ;
    tx_config.min_ipg               = config_info->min_ipg;
    _cprimod_to_cprif_fast_eth_crc_mode(config_info->tx_crc_mode, &tx_config.crc_mode);
    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_tx_cw_fast_eth_config_set(unit, port, &tx_config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_cw_fast_eth_config_get(int unit, int port, cprimod_fast_eth_config_info_t* config_info)
{
    cprif_cpri_rx_fast_eth_config_t rx_config;
    cprif_cpri_rx_fast_eth_word_config_t rx_word_config;
    cprif_cpri_tx_fast_eth_config_t tx_config;
    SOC_INIT_FUNC_DEFS;


    sal_memset(&rx_config, 0, sizeof(cprif_cpri_rx_fast_eth_config_t));
    sal_memset(&rx_word_config, 0, sizeof(cprif_cpri_rx_fast_eth_word_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_fast_eth_word_config_get(unit, port, &rx_word_config));

    config_info->schan_start    = rx_word_config.sub_channel_start;
    config_info->schan_size     = rx_word_config.sub_channel_size;
    config_info->cw_sel         = rx_word_config.cw_sel;
    config_info->cw_size        = rx_word_config.cw_size;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_fast_eth_config_get(unit, port, &rx_config));

    config_info->queue_num          = rx_config.queue_num;
    config_info->no_fcs_err_check   = rx_config.ignore_fcs_err;
    config_info->min_packet_drop    = rx_config.min_packet_drop;
    config_info->max_packet_drop    = rx_config.max_packet_drop;
    config_info->min_packet_size    = rx_config.min_packet_size;
    config_info->max_packet_size    = rx_config.max_packet_size;
    config_info->strip_crc          = rx_config.strip_crc;


    sal_memset(&tx_config, 0, sizeof(cprif_cpri_tx_fast_eth_config_t));
    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_tx_cw_fast_eth_config_get(unit, port, &tx_config));

    config_info->schan_start    = tx_config.sub_channel_start;
    config_info->schan_size     = tx_config.sub_channel_size;
    config_info->cw_sel         = tx_config.cw_sel;
    config_info->cw_size        = tx_config.cw_size;
    config_info->queue_num      = tx_config.queue_num;
    config_info->min_ipg        = tx_config.min_ipg;
    _cprif_to_cprimod_fast_eth_crc_mode(tx_config.crc_mode,&config_info->tx_crc_mode);
exit:
    SOC_FUNC_RETURN;
}


static
void _cprimod_to_cprif_vsd_control_schan_num_bytes( cprimod_cpri_vsd_control_subchan_num_bytes_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_1:
            *cprif_mode = CPRIF_VSD_SCHAN_NUM_OF_BYTES_1;
            break;
        case CPRIMOD_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_2:
            *cprif_mode = CPRIF_VSD_SCHAN_NUM_OF_BYTES_2;
            break;
        case CPRIMOD_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_4:
            *cprif_mode = CPRIF_VSD_SCHAN_NUM_OF_BYTES_4;
            break;
        default:
            *cprif_mode = CPRIF_VSD_SCHAN_NUM_OF_BYTES_1;
            break;
    }
}

static
void _cprif_to_cprimod_vsd_control_schan_num_bytes( uint32 cprif_mode, cprimod_cpri_vsd_control_subchan_num_bytes_t *cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_VSD_SCHAN_NUM_OF_BYTES_1:
            *cprimod_mode = CPRIMOD_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_1;
            break;
        case CPRIF_VSD_SCHAN_NUM_OF_BYTES_2:
            *cprimod_mode = CPRIMOD_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_2;
            break;
        case CPRIF_VSD_SCHAN_NUM_OF_BYTES_4:
            *cprimod_mode = CPRIMOD_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_4;
            break;
        default:
            *cprimod_mode = CPRIMOD_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_1;
            break;
    }
}

static
void _cprimod_to_cprif_vsd_control_flow_num_bytes( cprimod_cpri_vsd_control_flow_num_bytes_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_VSD_CONTROL_FLOW_NUM_BYTES_2:
            *cprif_mode = CPRIF_VSD_FLOW_NUM_OF_BYTES_2;
            break;
        case CPRIMOD_CPRI_VSD_CONTROL_FLOW_NUM_BYTES_4:
            *cprif_mode = CPRIF_VSD_FLOW_NUM_OF_BYTES_4;
            break;
        default:
            *cprif_mode = CPRIF_VSD_FLOW_NUM_OF_BYTES_2;
            break;
    }
}

static
void _cprif_to_cprimod_vsd_control_flow_num_bytes( uint32 cprif_mode , cprimod_cpri_vsd_control_flow_num_bytes_t *cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_VSD_FLOW_NUM_OF_BYTES_2:
            *cprimod_mode = CPRIMOD_CPRI_VSD_CONTROL_FLOW_NUM_BYTES_2;
            break;
        case CPRIF_VSD_FLOW_NUM_OF_BYTES_4:
            *cprimod_mode = CPRIMOD_CPRI_VSD_CONTROL_FLOW_NUM_BYTES_4;
            break;
        default:
            *cprimod_mode = CPRIMOD_CPRI_VSD_CONTROL_FLOW_NUM_BYTES_2;
            break;
    }
}

int cprif_cpri_port_rx_vsd_cw_config_set(int unit, int port, const cprimod_cpri_rx_vsd_config_info_t* config_info)
{
    cprif_cpri_vsd_config_t config;
    SOC_INIT_FUNC_DEFS;


    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_config_t));

    config.sub_channel_start        = config_info->subchan_start ;
    config.sub_channel_size         = config_info->subchan_size ;
    config.sub_channel_steps        = config_info->subchan_steps ;
    config.queue_num                = config_info->queue_num ;

    if (config_info->subchan_bytes == CPRIMOD_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_4) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("SubChannel Number of Bytes 4 is NOT supported for this Device.\n"));
    }

    _cprimod_to_cprif_vsd_control_schan_num_bytes(config_info->subchan_bytes,
                                                  &config.sub_channel_num_bytes);

    _cprimod_to_cprif_vsd_control_flow_num_bytes(config_info->flow_bytes,
                                                 &config.flow_bytes);
    config.byte_order = CPRIF_VSD_CTRL_BYTE_NO_SWAP ;

    COMPILER_64_SET(config.rsvd_mask[0],config_info->rsvd_sector_mask[1],config_info->rsvd_sector_mask[0]);
    COMPILER_64_SET(config.rsvd_mask[1],config_info->rsvd_sector_mask[3],config_info->rsvd_sector_mask[2]);

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_config_set(unit, port, &config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_vsd_cw_config_get(int unit, int port, cprimod_cpri_rx_vsd_config_info_t* config_info)
{
   cprif_cpri_vsd_config_t config;
    SOC_INIT_FUNC_DEFS;


    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_config_get(unit, port, &config));

    config_info->subchan_start  = config.sub_channel_start;
    config_info->subchan_size   = config.sub_channel_size;
    config_info->subchan_steps  = config.sub_channel_steps;
    config_info->queue_num      = config.queue_num;

    _cprif_to_cprimod_vsd_control_schan_num_bytes(config.sub_channel_num_bytes,
                                                  &config_info->subchan_bytes);

    _cprif_to_cprimod_vsd_control_flow_num_bytes(config.flow_bytes,
                                                 &config_info->flow_bytes);

    config_info->rsvd_sector_mask[0] = COMPILER_64_LO(config.rsvd_mask[0]);
    config_info->rsvd_sector_mask[1] = COMPILER_64_HI(config.rsvd_mask[0]);
    config_info->rsvd_sector_mask[2] = COMPILER_64_LO(config.rsvd_mask[1]);
    config_info->rsvd_sector_mask[3] = COMPILER_64_HI(config.rsvd_mask[1]);

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_rx_vsd_ctrl_flow_add(int unit, int port, uint32 group_id, const cprimod_cpri_rx_vsd_flow_info_t* config_info)
{
    uint32 rsvd_mask[4]={0};
    uint32 ctrl_mask[4]={0};
    uint32 grp_ptr_mask[2]={0};
    uint32 valid_group_mask[2]={0};
    uint32 grp_ptr_index=0;
    uint32 tag_id;
    int index=0;
    uint32 sector=0;
    uint32 section=0;
    uint32 sector_num[CPRIF_VSD_CTRL_MAX_GROUP_SIZE];
    uint32 copy_id;
    cprif_cpri_rx_vsd_ctrl_flow_config_t flow_config;
    uint32 group=0;
    uint32 num_sector=0;

    SOC_INIT_FUNC_DEFS;

    if (group_id > CPRIF_VSD_CTRL_NUM_GROUPS) {
        LOG_CLI(("VSD CTRL group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_rsvd_mask_get(unit, port, rsvd_mask));

    if (config_info->num_sector > CPRIF_VSD_CTRL_MAX_GROUP_SIZE) {
        return SOC_E_PARAM;
    }

    for ( index=0 ; index < config_info->num_sector ; index++) {

        if (config_info->section_num[index] >= CPRIF_VSD_CTRL_MAX_NUM_SECTION){
            /* if sector has illegal number, error and get out */
            return SOC_E_PARAM;
        }
        if (!CPRIF_PBMP_IS_SET(rsvd_mask, config_info->section_num[index])) {
            /* if sector is not reserved, error and get out */
            return SOC_E_PARAM;
        }

        /* find sector number */
        sector = 0;
        for (section = 0; section <= config_info->section_num[index]; section++)
        {
            if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
                sector++;
            }
        }
        sector_num[index] = sector -1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_ctrl_mask_get(unit, port, copy_id, ctrl_mask));

    /* find valid  group list. */
    sector = 0;
    for ( section = 0 ; (section < CPRIF_VSD_CTRL_MAX_NUM_SECTION) && (sector < CPRIF_VSD_CTRL_MAX_NUM_SECTOR); section++) {
        if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
            if (CPRIF_PBMP_IS_SET(ctrl_mask, section)) {
                /* This is valid sector. */
                _SOC_IF_ERR_EXIT
                    (cprif_drv_rx_vsd_ctrl_flow_config_get(unit, port, sector, &flow_config));

                if (flow_config.group_id == group_id) {
                    _SOC_EXIT_WITH_ERR
                        (SOC_E_PARAM, ("The requested group id %d is already configured. Delete first.\n",group_id));
                }

                CPRIF_PBMP_IDX_MARK(valid_group_mask, flow_config.group_id);
            }
            sector++;
        }
    }

    /* marked all used grp ptr. */
    for ( group = 0 ; group < CPRIF_VSD_CTRL_NUM_GROUPS; group++) {
        if (CPRIF_PBMP_IS_SET(valid_group_mask, group)){
            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_group_config_get(unit, port, group,  &tag_id, &grp_ptr_index));

            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_group_num_sector_get(unit, port, group,  &num_sector));
            for(index = 0; index < (num_sector + 1); index++){
                CPRIF_PBMP_IDX_MARK(grp_ptr_mask, grp_ptr_index + index);
            }
        }
    }

    /* get the grp ptr space. */
    _SOC_IF_ERR_EXIT
        (cprif_drv_allocate_grp_ptr(grp_ptr_mask, config_info->num_sector, &grp_ptr_index));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_group_num_sector_set(unit, port, group_id, config_info->num_sector-1));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_group_config_set(unit, port, group_id, config_info->tag_id, grp_ptr_index));
    sal_memset(&flow_config, 0, sizeof(cprif_cpri_rx_vsd_ctrl_flow_config_t));

    flow_config.group_id            = group_id;
    flow_config.hyper_frame_number  = config_info->hyper_frame_number;
    flow_config.hyper_frame_modulo  = config_info->hyper_frame_modulo;
    flow_config.filter_zero_data    = config_info->filter_zero_data? 1:0;


    for(index = 0; index < config_info->num_sector; index++){

        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_vsd_ctrl_flow_config_set(unit,port,
                                                   sector_num[index],
                                                   &flow_config));

        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_vsd_ctrl_group_assign_ptr_set(unit, port,
                                                        grp_ptr_index + index,
                                                        sector_num[index]));
    }

    /* add ctrl_mask with new bits. */
    for ( index=0 ; index < config_info->num_sector ; index++) {
        CPRIF_PBMP_IDX_MARK(ctrl_mask, config_info->section_num[index]);
    }

    /* update the ctrl mask to alternate location and set the active copy. */

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_ctrl_mask_set(unit, port, (copy_id == 0) ? 1:0, ctrl_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_copy_id_set(unit, port, (copy_id == 0) ? 1:0));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_vsd_ctrl_flow_delete (int unit, int port, uint32 group_id)
{
    uint32 rsvd_mask[4]={0};
    uint32 ctrl_mask[4]={0};
    uint32 sector=0;
    uint32 section=0;
    uint32 copy_id;
    cprif_cpri_rx_vsd_ctrl_flow_config_t flow_config;

    SOC_INIT_FUNC_DEFS;

    if (group_id > CPRIF_VSD_CTRL_NUM_GROUPS) {
        LOG_CLI(("VSD CTRL group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_rsvd_mask_get(unit, port, rsvd_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_ctrl_mask_get(unit, port, copy_id, ctrl_mask));

    /* unmark ctrl_mask with section belong to the group.. */
    sector = 0;
    for ( section = 0 ; (section < CPRIF_VSD_CTRL_MAX_NUM_SECTION) && (sector < CPRIF_VSD_CTRL_MAX_NUM_SECTOR); section++) {
        if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
            if (CPRIF_PBMP_IS_SET(ctrl_mask, section)) {
                /* This is valid sector. */
                _SOC_IF_ERR_EXIT
                    (cprif_drv_rx_vsd_ctrl_flow_config_get(unit, port, sector, &flow_config));

                if (flow_config.group_id == group_id) {
                    CPRIF_PBMP_IDX_UNMARK(ctrl_mask, section);
                }
            }
            sector++;
        }
    }

    /* update the ctrl mask to alternate location and set the active copy. */
    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_ctrl_mask_set(unit, port, (copy_id == 0) ? 1:0, ctrl_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_copy_id_set(unit, port, (copy_id == 0) ? 1:0));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_vsd_ctrl_flow_get(int unit, int port, uint32 group_id, cprimod_cpri_rx_vsd_flow_info_t* config_info)
{
    int sector=-1;
    int section=0;
    int index=0;
    uint32 num_sector=0;
    uint32 grp_ptr_index=0;
    uint32 sector_num;
    uint32 sector_mask[2]={0};
    uint32 valid_sector_mask[2]={0};
    uint32 valid_group_mask[4]={0};
    uint32 rsvd_mask[4]={0};
    uint32 ctrl_mask[4]={0};
    cprif_cpri_rx_vsd_ctrl_flow_config_t flow_config;
    uint32 copy_id=0;

    SOC_INIT_FUNC_DEFS;

    if (group_id > CPRIF_VSD_CTRL_NUM_GROUPS) {
        LOG_CLI(("VSD CTRL group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_rsvd_mask_get(unit, port, rsvd_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_ctrl_ctrl_mask_get(unit, port, copy_id, ctrl_mask));

    /* find valid sector list and group list.*/
    sector = 0;
    for ( section = 0 ; (section < CPRIF_VSD_CTRL_MAX_NUM_SECTION) && (sector < CPRIF_VSD_CTRL_MAX_NUM_SECTOR); section++) {
        if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
            if (CPRIF_PBMP_IS_SET(ctrl_mask, section)) {
                /* This is valid sector. Mark it. */
                CPRIF_PBMP_IDX_MARK(valid_sector_mask, sector);
               _SOC_IF_ERR_EXIT
                    (cprif_drv_rx_vsd_ctrl_flow_config_get(unit, port, sector, &flow_config));
                CPRIF_PBMP_IDX_MARK(valid_group_mask, flow_config.group_id);

            }
            sector++;
        }
    }

    if (CPRIF_PBMP_IS_SET(valid_group_mask, group_id)) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_vsd_ctrl_group_config_get(unit, port,
                                                    group_id,
                                                    &(config_info->tag_id),
                                                    &grp_ptr_index));

        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_vsd_ctrl_group_num_sector_get(unit, port,
                                                        group_id,
                                                        &num_sector));

        for (index = 0; index < num_sector+1; index++){

            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_group_assign_ptr_get(unit, port,
                                                            grp_ptr_index + index,
                                                            &sector_num));
            CPRIF_PBMP_IDX_MARK(sector_mask, sector_num);
            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_flow_config_get(unit,port,
                                                       sector_num,
                                                       &flow_config));
        }

        config_info->hyper_frame_number = flow_config.hyper_frame_number;
        config_info->hyper_frame_modulo = flow_config.hyper_frame_modulo;
        config_info->filter_zero_data   = flow_config.filter_zero_data;

        /*
         * convert sector number to section number.
         */
        sector = 0;
        index  = 0;
        for ( section = 0 ; (section < CPRIF_VSD_CTRL_MAX_NUM_SECTION) && (sector < CPRIF_VSD_CTRL_MAX_NUM_SECTOR); section++) {
            if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
                if (CPRIF_PBMP_IS_SET(sector_mask, sector) &&
                    (index < CPRIF_VSD_CTRL_MAX_GROUP_SIZE)) {
                    config_info->section_num[index] = section;
                     index++;
                }
                sector++;
            }
        }
        config_info->num_sector = index;
    } else {
        LOG_CLI(("The requested group id %d is not configured.\n",group_id));
        return SOC_E_NOT_FOUND;
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_control_queue_tag_to_flow_id_map_set(int unit, int port, uint32 tag_id, uint32 flow_id)
{
    SOC_INIT_FUNC_DEFS;


    if (tag_id >= CPRIF_TAG_FLOW_MAP_MAX) {
        LOG_CLI(("Tag to Flow MaP TAG_ID  = %d is out of range.\n",tag_id));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_tag_to_flow_id_set(unit, port, tag_id, flow_id));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_control_queue_tag_to_flow_id_map_get(int unit, int port, uint32 tag_id, uint32* flow_id)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_tag_to_flow_id_get(unit, port, tag_id, flow_id));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_cw_config_set(int unit, int port, const cprimod_cpri_tx_vsd_config_info_t* config_info)
{
    cprif_cpri_vsd_config_t config;
    SOC_INIT_FUNC_DEFS;


    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_config_t));

    config.sub_channel_start        = config_info->subchan_start ;
    config.sub_channel_size         = config_info->subchan_size ;
    config.sub_channel_steps        = config_info->subchan_steps ;
    config.queue_num                = config_info->queue_num ;

    if (config_info->subchan_bytes == CPRIMOD_CPRI_VSD_CONTROL_SUBCHAN_NUM_BYTES_4) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("SubChannel Number of Bytes 4 is NOT supported for this Device.\n"));
    }

    _cprimod_to_cprif_vsd_control_schan_num_bytes(config_info->subchan_bytes,
                                                  &config.sub_channel_num_bytes);

    _cprimod_to_cprif_vsd_control_flow_num_bytes(config_info->flow_bytes,
                                                 &config.flow_bytes);
    config.byte_order = config_info->byte_order_swap? CPRIF_VSD_CTRL_BYTE_SWAP : CPRIF_VSD_CTRL_BYTE_NO_SWAP ;

    COMPILER_64_SET(config.rsvd_mask[0],config_info->rsvd_sector_mask[1],config_info->rsvd_sector_mask[0]);
    COMPILER_64_SET(config.rsvd_mask[1],config_info->rsvd_sector_mask[3],config_info->rsvd_sector_mask[2]);

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_config_set(unit, port, &config));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_tx_vsd_cw_config_get(int unit, int port, cprimod_cpri_tx_vsd_config_info_t* config_info)
{
    cprif_cpri_vsd_config_t config;
    SOC_INIT_FUNC_DEFS;


    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_config_get(unit, port, &config));

    config_info->subchan_start  = config.sub_channel_start;
    config_info->subchan_size   = config.sub_channel_size;
    config_info->subchan_steps  = config.sub_channel_steps;
    config_info->queue_num      = config.queue_num;
    config_info->byte_order_swap= (config.byte_order == CPRIF_VSD_CTRL_BYTE_SWAP)? 1 : 0;;

    _cprif_to_cprimod_vsd_control_schan_num_bytes(config.sub_channel_num_bytes,
                                                  &config_info->subchan_bytes);

    _cprif_to_cprimod_vsd_control_flow_num_bytes(config.flow_bytes,&config_info->flow_bytes);

    config_info->rsvd_sector_mask[0] = COMPILER_64_LO(config.rsvd_mask[0]);
    config_info->rsvd_sector_mask[1] = COMPILER_64_HI(config.rsvd_mask[0]);
    config_info->rsvd_sector_mask[2] = COMPILER_64_LO(config.rsvd_mask[1]);
    config_info->rsvd_sector_mask[3] = COMPILER_64_HI(config.rsvd_mask[1]);

exit:

    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_ctrl_flow_add(int unit, int port, uint32 group_id, uint32* group_list, const cprimod_cpri_tx_vsd_flow_info_t* config_info)
{

    uint32 rsvd_mask[4]={0};
    uint32 ctrl_mask[4]={0};
    uint32 grp_ptr_mask[2]={0};
    uint32 grp_ptr_index=0;
    int index=0;
    uint32 sector=0;
    uint32 section=0;
    uint32 sector_num[CPRIF_VSD_CTRL_MAX_GROUP_SIZE];
    uint32 copy_id;
    cprif_cpri_tx_vsd_ctrl_flow_config_t flow_config;
    uint32 group=0;
    uint32 num_sector=0;
    SOC_INIT_FUNC_DEFS;

    if (group_id > CPRIF_VSD_CTRL_NUM_GROUPS) {
        LOG_CLI(("VSD CTRL group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    if (CPRIF_PBMP_IS_SET(group_list, group_id)){
        LOG_CLI(("VSD CTRL group_id  =%d is already configured.\n",group_id));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_rsvd_mask_get(unit, port, rsvd_mask));

    if (config_info->num_sector > CPRIF_VSD_CTRL_MAX_GROUP_SIZE) {
        LOG_CLI(("Num Of Sector  =%d is out of range.\n",config_info->num_sector));
        return SOC_E_PARAM;
    }

    for ( index=0 ; index < config_info->num_sector ; index++) {

        if (config_info->section_num[index] >= CPRIF_VSD_CTRL_MAX_NUM_SECTION){
            /* if sector has illegal number, error and get out */
            return SOC_E_PARAM;
        }
        if (!CPRIF_PBMP_IS_SET(rsvd_mask, config_info->section_num[index])) {
            /* if sector is not reserved, error and get out */
            return SOC_E_PARAM;
        }

        /* find sector number */
        sector = 0;
        for (section = 0; section <= config_info->section_num[index]; section++)
        {
            if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
                sector++;
            }
        }
        sector_num[index] = sector -1;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_ctrl_mask_get(unit, port, copy_id, ctrl_mask));

    /* marked all used grp ptr. */
    for (group = 0 ; group < CPRIF_VSD_CTRL_NUM_GROUPS; group++) {
        if (CPRIF_PBMP_IS_SET(group_list, group)){
            _SOC_IF_ERR_EXIT
                (cprif_drv_tx_vsd_ctrl_group_config_get(unit, port,
                                                        group,
                                                        &num_sector,
                                                        &grp_ptr_index));
            for(index = 0; index < (num_sector + 1); index++){
                CPRIF_PBMP_IDX_MARK(grp_ptr_mask, grp_ptr_index + index);
            }
        }
    }


    /* get the grp ptr space. */
    _SOC_IF_ERR_EXIT
        (cprif_drv_allocate_grp_ptr(grp_ptr_mask, config_info->num_sector, &grp_ptr_index));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_group_config_set(unit, port, group_id, config_info->num_sector-1, grp_ptr_index));

    sal_memset(&flow_config, 0, sizeof(cprif_cpri_tx_vsd_ctrl_flow_config_t));

    flow_config.hyper_frame_number  = config_info->hyper_frame_number;
    flow_config.hyper_frame_modulo  = config_info->hyper_frame_modulo;
    flow_config.repeat_mode         = config_info->repeat_mode ? 1:0;


    for(index = 0; index < config_info->num_sector; index++){

        _SOC_IF_ERR_EXIT
            (cprif_drv_tx_vsd_ctrl_flow_config_set(unit,port,
                                                   sector_num[index],
                                                   &flow_config));

        _SOC_IF_ERR_EXIT
            (cprif_drv_tx_vsd_ctrl_group_assign_ptr_set(unit, port,
                                                        grp_ptr_index + index,
                                                        sector_num[index]));
    }

    /*
     * Program flow id to vsd control group id mapping.
     */
    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_flow_id_to_group_mapping_set(unit, port,
                                                            config_info->roe_flow_id,
                                                            group_id));
    /* add ctrl_mask with new bits. */
    for ( index=0 ; index < config_info->num_sector ; index++) {
        CPRIF_PBMP_IDX_MARK(ctrl_mask, config_info->section_num[index]);
    }

    /* update the ctrl mask to alternate location and set the active copy. */

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_ctrl_mask_set(unit, port, (copy_id == 0) ? 1:0, ctrl_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_copy_id_set(unit, port, (copy_id == 0) ? 1:0));

    /* update the group list */
    CPRIF_PBMP_IDX_MARK(group_list, group_id);
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_ctrl_flow_delete(int unit, int port, uint32 group_id, uint32* group_list)
{
    uint32 num_sector=0;
    uint32 grp_ptr_index;
    int index;
    int sector;
    int section;
    uint32 sector_num[CPRIF_VSD_CTRL_MAX_GROUP_SIZE];
    uint32 rsvd_mask[4]={0};
    uint32 ctrl_mask[4]={0};
    uint32 sector_mask[2]={0};
    uint32 copy_id;
    SOC_INIT_FUNC_DEFS;

   if (!CPRIF_PBMP_IS_SET(group_list, group_id)){
        LOG_CLI(("VSD CTRL group_id  =%d is not  configured.\n",group_id));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_rsvd_mask_get(unit, port, rsvd_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_ctrl_mask_get(unit, port, copy_id, ctrl_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_group_config_get(unit, port, group_id, &num_sector, &grp_ptr_index));

    num_sector += 1;

    for (index = 0; index < num_sector; index++){
        _SOC_IF_ERR_EXIT
            (cprif_drv_tx_vsd_ctrl_group_assign_ptr_get(unit, port,
                                                        grp_ptr_index + index,
                                                        &sector_num[index]));

        CPRIF_PBMP_IDX_MARK(sector_mask, sector_num[index]);

    }

    sector = 0;
    index  = 0;
    for ( section = 0 ; (section < CPRIF_VSD_CTRL_MAX_NUM_SECTION) && (sector < CPRIF_VSD_CTRL_MAX_NUM_SECTOR); section++) {
        if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
            if (CPRIF_PBMP_IS_SET(sector_mask, sector)) { 
                if (CPRIF_PBMP_IS_SET(ctrl_mask, section)){
                    CPRIF_PBMP_IDX_UNMARK(ctrl_mask,section);
                } 
            }
            sector++;
        }
    }

    /* update the ctrl mask to alternate location and set the active copy. */

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_ctrl_mask_set(unit, port, (copy_id == 0) ? 1:0, ctrl_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_copy_id_set(unit, port, (copy_id == 0) ? 1:0));

    /* update the group list */
    CPRIF_PBMP_IDX_UNMARK(group_list, group_id);
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_ctrl_flow_get(int unit, int port, uint32 group_id, uint32* group_list, cprimod_cpri_tx_vsd_flow_info_t* config_info)
{
    uint32 num_sector=0;
    uint32 grp_ptr_index;
    uint32 group_num;
    int index;
    int sector;
    int section;
    uint32 copy_id;
    uint32 sector_num[CPRIF_VSD_CTRL_MAX_GROUP_SIZE];
    cprif_cpri_tx_vsd_ctrl_flow_config_t flow_config;
    uint32 rsvd_mask[4]={0};
    uint32 ctrl_mask[4]={0};
    uint32 sector_mask[2]={0};
    SOC_INIT_FUNC_DEFS;

   if (!CPRIF_PBMP_IS_SET(group_list, group_id)){
        LOG_CLI(("VSD CTRL group_id  =%d is not  configured.\n",group_id));
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_rsvd_mask_get(unit, port, rsvd_mask));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_copy_id_get(unit, port, &copy_id));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_ctrl_mask_get(unit, port, copy_id, ctrl_mask));


    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_ctrl_group_config_get(unit, port, group_id, &num_sector, &grp_ptr_index));

    num_sector += 1;

    for (index = 0; index < num_sector; index++){
        _SOC_IF_ERR_EXIT
            (cprif_drv_tx_vsd_ctrl_group_assign_ptr_get(unit, port,
                                                        grp_ptr_index + index,
                                                        &sector_num[index]));

        CPRIF_PBMP_IDX_MARK(sector_mask, sector_num[index]);

        _SOC_IF_ERR_EXIT
            (cprif_drv_tx_vsd_ctrl_flow_config_get(unit,port,
                                                   sector_num[index],
                                                   &flow_config));

        config_info->hyper_frame_number  =   flow_config.hyper_frame_number;
        config_info->hyper_frame_modulo  =   flow_config.hyper_frame_modulo;
        config_info->repeat_mode         =   flow_config.repeat_mode;
    }

    group_num = 0xff;
    for (index = 0; index < 128 ; index++){
        _SOC_IF_ERR_EXIT
            (cprif_drv_tx_vsd_ctrl_flow_id_to_group_mapping_get(unit, port,
                                                                index,
                                                                &group_num));
        if (group_num == group_id) {
            config_info->roe_flow_id = index;
            continue;
        }
    }

    sector = 0;
    index  = 0;
    for ( section = 0 ; (section < CPRIF_VSD_CTRL_MAX_NUM_SECTION) && (sector < CPRIF_VSD_CTRL_MAX_NUM_SECTOR); section++) {
        if (CPRIF_PBMP_IS_SET(rsvd_mask, section)) {
            if (CPRIF_PBMP_IS_SET(sector_mask, sector) &&
                (index < num_sector)) {
                if (CPRIF_PBMP_IS_SET(ctrl_mask, section)) {
                    config_info->section_num[index] = section;
                    index++;
                } else {
                    LOG_CLI(("The requested group id %d is not configured.\n",group_id));
                    return SOC_E_NOT_FOUND;
                }
            }
            sector++;
        }
    }
    config_info->num_sector = index;
exit:
    SOC_FUNC_RETURN;

}
void _cprimod_to_cprif_cw_filter_mode( cprimod_cpri_cw_filter_mode_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_CW_FILTER_DISABLE:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_DISABLE;
            break;
        case CPRIMOD_CPRI_CW_FILTER_NON_ZERO:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_NON_ZERO;
            break;
        case CPRIMOD_CPRI_CW_FILTER_PERIODIC:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_PERIODIC;
            break;
        case CPRIMOD_CPRI_CW_FILTER_CHANGE:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_CHANGE;
            break;
        case CPRIMOD_CPRI_CW_FILTER_PATTERN_MATCH:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_PATTERN_MATCH;
            break;
        default:
            *cprif_mode = CPRIF_VSD_RAW_FILTER_MODE_DISABLE;
            break;
    }
}

void _cprif_to_cprimod_cw_filter_mode( uint32 cprif_mode , cprimod_cpri_cw_filter_mode_t *cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_VSD_RAW_FILTER_MODE_DISABLE:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_DISABLE;
            break;
        case CPRIF_VSD_RAW_FILTER_MODE_NON_ZERO:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_NON_ZERO;
            break;
        case CPRIF_VSD_RAW_FILTER_MODE_PERIODIC:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_PERIODIC;
            break;
        case CPRIF_VSD_RAW_FILTER_MODE_CHANGE:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_CHANGE;
            break;
        case CPRIF_VSD_RAW_FILTER_MODE_PATTERN_MATCH:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_PATTERN_MATCH;
            break;
        default:
            *cprimod_mode = CPRIMOD_CPRI_CW_FILTER_DISABLE;
            break;
    }
}

int cprif_cpri_port_rx_vsd_raw_config_set(int unit, int port, uint32 index, const cprimod_cpri_rx_vsd_raw_config_t* config_info)
{
    cprif_cpri_vsd_raw_config_t config;
    cprif_cpri_rx_vsd_raw_filter_config_t filter_config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_VSD_RAW_MAX_GROUP) {
        LOG_CLI(("VSD RAW  index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    /*
     *  - Stop VSD raw flow by CW_SEL to 0. To prevent erroneous packet forming.
     *  - Update filtering option.
     *  - Configure new flow.
     */

    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_raw_config_t));
    config.schan_start  = 0;
    config.schan_size   = 0;
    config.cw_select    = 0;
    config.cw_size      = 0;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_raw_config_set(unit, port, index, &config));

    /*
     *  Set new filter configuration.
     */
    sal_memset(&filter_config, 0, sizeof(cprif_cpri_rx_vsd_raw_filter_config_t));

    _cprimod_to_cprif_cw_filter_mode(config_info->filter_mode, &filter_config.filter_mode);

    filter_config.hfn_index     = config_info->hyper_frame_index;
    filter_config.hfn_modulo    = config_info->hyper_frame_modulo;
    filter_config.match_value   = config_info->match_value;
    filter_config.match_offset  = config_info->match_offset;
    filter_config.match_mask    = config_info->match_mask;
    filter_config.queue_num     = config_info->queue_num;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_raw_filter_config_set(unit, port, index, &filter_config));

    /*
     * Lastly, configure the VSD raw flow.
     */
    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_raw_config_t));
    config.schan_start  = config_info->schan_start;
    config.schan_size   = config_info->schan_size;
    config.cw_select    = config_info->cw_sel;
    config.cw_size      = config_info->cw_size;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_raw_config_set(unit, port, index, &config));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_vsd_raw_config_get(int unit, int port, uint32 index, cprimod_cpri_rx_vsd_raw_config_t* config_info)
{
    cprif_cpri_vsd_raw_config_t config;
    cprif_cpri_rx_vsd_raw_filter_config_t filter_config;
    SOC_INIT_FUNC_DEFS;

   if (index >= CPRIF_VSD_RAW_MAX_GROUP) {
        LOG_CLI(("VSD RAW  index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }
 sal_memset(&config, 0, sizeof(cprif_cpri_vsd_raw_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_raw_config_get(unit, port, index, &config));
    config_info->schan_start    = config.schan_start;
    config_info->schan_size     = config.schan_size;
    config_info->cw_sel         = config.cw_select;
    config_info->cw_size        = config.cw_size;

    sal_memset(&filter_config, 0, sizeof(cprif_cpri_rx_vsd_raw_filter_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_vsd_raw_filter_config_get(unit, port, index, &filter_config));
    config_info->filter_mode = filter_config.filter_mode;

    _cprif_to_cprimod_cw_filter_mode(filter_config.filter_mode, &config_info->filter_mode);
    config_info->hyper_frame_index  = filter_config.hfn_index;
    config_info->hyper_frame_modulo = filter_config.hfn_modulo;
    config_info->match_value        = filter_config.match_value;
    config_info->match_offset       = filter_config.match_offset;
    config_info->match_mask         = filter_config.match_mask;
    config_info->queue_num          = filter_config.queue_num;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_raw_config_set(int unit, int port, uint32 index, const cprimod_cpri_tx_vsd_raw_config_t* config_info)
{
    cprif_cpri_vsd_raw_config_t config;
    cprif_cpri_tx_vsd_raw_filter_config_t filter_config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_VSD_RAW_MAX_GROUP) {
        LOG_CLI(("VSD RAW  index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_raw_config_t));
    config.schan_start  = config_info->schan_start;
    config.schan_size   = config_info->schan_size;
    config.cw_select    = config_info->cw_sel;
    config.cw_size      = config_info->cw_size;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_config_set(unit, port, index, &config));

    sal_memset(&filter_config, 0, sizeof(cprif_cpri_tx_vsd_raw_filter_config_t));

    filter_config.queue_num     = config_info->queue_num;

    if (config_info->map_mode == CPRIMOD_CPRI_CW_MAP_MODE_ROE_FRAME){
        filter_config.map_mode = CPRIF_VSD_RAW_MAP_MODE_USE_ROE_HDR;
    } else {
        filter_config.map_mode = CPRIF_VSD_RAW_MAP_MODE_USE_MODULO;
    }

    if (config_info->repeat_enable) {
        filter_config.repeat_mode = CPRIF_CW_REPEAT_MODE_CONTINUOUS;
    } else {
        filter_config.repeat_mode = CPRIF_CW_REPEAT_MODE_ONCE;
    }

    if (config_info->bfn0_filter_enable) {
        filter_config.bfn0_filter_enable = 1;
    } else {
        filter_config.bfn0_filter_enable = 0;
    }

    if (config_info->bfn1_filter_enable) {
        filter_config.bfn1_filter_enable = 1;
    } else {
        filter_config.bfn1_filter_enable = 0;
    }

    filter_config.hfn_index     = config_info->hyper_frame_index;
    filter_config.hfn_modulo    = config_info->hyper_frame_modulo;
    filter_config.idle_value    = config_info->idle_value;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_filter_config_set(unit, port, index, &filter_config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_raw_config_get(int unit, int port, uint32 index, cprimod_cpri_tx_vsd_raw_config_t* config_info)
{
    cprif_cpri_vsd_raw_config_t config;
    cprif_cpri_tx_vsd_raw_filter_config_t filter_config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_VSD_RAW_MAX_GROUP) {
        LOG_CLI(("VSD RAW  index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_vsd_raw_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_config_get(unit, port, index, &config));
    config_info->schan_start    = config.schan_start;
    config_info->schan_size     = config.schan_size;
    config_info->cw_sel         = config.cw_select;
    config_info->cw_size        = config.cw_size;

    sal_memset(&filter_config, 0, sizeof(cprif_cpri_tx_vsd_raw_filter_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_filter_config_get(unit, port, index, &filter_config));

    config_info->hyper_frame_index  = filter_config.hfn_index;
    config_info->hyper_frame_modulo = filter_config.hfn_modulo;
    config_info->queue_num          = filter_config.queue_num;
    config_info->idle_value         = filter_config.idle_value;

    if (filter_config.bfn0_filter_enable) {
        config_info->bfn0_filter_enable = 1;
    } else {
        config_info->bfn0_filter_enable = 0;
    }

    if (filter_config.bfn1_filter_enable) {
        config_info->bfn1_filter_enable = 1;
    } else {
        config_info->bfn1_filter_enable = 0;
    }
    if (filter_config.repeat_mode == CPRIF_CW_REPEAT_MODE_CONTINUOUS){
        config_info->repeat_enable = 1;
    } else {
        config_info->repeat_enable = 0;
    }
    if (filter_config.map_mode == CPRIF_VSD_RAW_MAP_MODE_USE_ROE_HDR) {
        config_info->map_mode = CPRIMOD_CPRI_CW_MAP_MODE_ROE_FRAME;
    } else {
        config_info->map_mode = CPRIMOD_CPRI_CW_MAP_MODE_PERIODIC;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_raw_filter_set(int unit, int port, uint32 bfn0_value, uint32 bfn0_mask, uint32 bfn1_value, uint32 bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_radio_frame_filter_set(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_vsd_raw_filter_get(int unit, int port, uint32* bfn0_value, uint32* bfn0_mask, uint32* bfn1_value, uint32* bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_vsd_raw_radio_frame_filter_get(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_brcm_rsvd5_control_config_set(int unit, int port, uint32 schan_start, uint32 schan_size, uint32 queue_num, int parity_disable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_brcm_rsvd5_config_set(unit, port,schan_start, schan_size, queue_num, parity_disable));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_brcm_rsvd5_control_config_get(int unit, int port, uint32* schan_start, uint32* schan_size, uint32* queue_num, int* parity_disable)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_brcm_rsvd5_config_get(unit, port,schan_start, schan_size, queue_num, parity_disable));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_brcm_rsvd5_control_config_set(int unit, int port, uint32 schan_start, uint32 schan_size, uint32 queue_num, int crc_enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_brcm_rsvd5_config_set(unit, port,schan_start, schan_size, queue_num, crc_enable));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_brcm_rsvd5_control_config_get(int unit, int port, uint32* schan_start, uint32* schan_size, uint32* queue_num, int* crc_enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_brcm_rsvd5_config_get(unit, port,schan_start, schan_size, queue_num, crc_enable));
exit:
    SOC_FUNC_RETURN;

}

void _cprimod_to_cprif_gcw_mask( cprimod_cpri_gcw_mask_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CPRI_GCW_MASK_NONE:
            *cprif_mode = CPRIF_GCW_MASK_NONE;
            break;
        case CPRIMOD_CPRI_GCW_MASK_LSB:
            *cprif_mode = CPRIF_GCW_MASK_LSB;
            break;
        case CPRIMOD_CPRI_GCW_MASK_MSB:
            *cprif_mode = CPRIF_GCW_MASK_MSB;
            break;
        case CPRIMOD_CPRI_GCW_MASK_BOTH:
            *cprif_mode = CPRIF_GCW_MASK_BOTH;
            break;
        default:
            *cprif_mode = CPRIF_GCW_MASK_NONE;
            break;
    }
}

void _cprif_to_cprimod_gcw_mask( uint32 cprif_mode, cprimod_cpri_gcw_mask_t* cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_GCW_MASK_NONE:
            *cprimod_mode = CPRIMOD_CPRI_GCW_MASK_NONE;
            break;
        case CPRIF_GCW_MASK_LSB:
            *cprimod_mode = CPRIMOD_CPRI_GCW_MASK_LSB;
            break;
        case CPRIF_GCW_MASK_MSB:
            *cprimod_mode = CPRIMOD_CPRI_GCW_MASK_MSB;
            break;
        case CPRIF_GCW_MASK_BOTH:
            *cprimod_mode = CPRIMOD_CPRI_GCW_MASK_BOTH;
            break;
        default:
            *cprimod_mode = cprimodGcwMaskNone;
            break;
    }
}


int cprif_cpri_port_rx_gcw_config_set(int unit, int port, uint32 index, const cprimod_cpri_rx_gcw_config_t* config_info)
{
    cprif_cpri_rx_gcw_config_t config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_GCW_MAX_GROUP) {
        LOG_CLI(("GCW index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_rx_gcw_config_t));

    config.Ns           = config_info->Ns;
    config.Xs           = config_info->Xs;
    config.Y            = config_info->Y;
    config.hfn_index    = config_info->hyper_frame_index;
    config.hfn_modulo   = config_info->hyper_frame_modulo;
    config.match_mask   = config_info->match_mask;
    config.match_value  = config_info->match_value;

    _cprimod_to_cprif_gcw_mask(config_info->mask, &config.mask);
    _cprimod_to_cprif_cw_filter_mode(config_info->filter_mode, &config.filter_mode);

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_gcw_config_set(unit, port,
                                     index, &config));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_rx_gcw_config_get(int unit, int port, uint32 index, cprimod_cpri_rx_gcw_config_t* config_info)
{
    cprif_cpri_rx_gcw_config_t config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_GCW_MAX_GROUP) {
        LOG_CLI(("GCW index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_rx_gcw_config_t));
    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_gcw_config_get(unit, port,
                                     index, &config));

    config_info->Ns                 = config.Ns;
    config_info->Xs                 = config.Xs;
    config_info->Y                  = config.Y;
    config_info->hyper_frame_index  = config.hfn_index;
    config_info->hyper_frame_modulo = config.hfn_modulo;
    config_info->match_mask         = config.match_mask;
    config_info->match_value        = config.match_value;

    _cprif_to_cprimod_gcw_mask(config.mask, &config_info->mask);
    _cprif_to_cprimod_cw_filter_mode(config.filter_mode, &config_info->filter_mode);

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_gcw_config_set(int unit, int port, uint32 index, const cprimod_cpri_tx_gcw_config_t* config_info)
{
    cprif_cpri_tx_gcw_config_t config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_GCW_MAX_GROUP) {
        LOG_CLI(("GCW index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_tx_gcw_config_t));
    config.Ns = config_info->Ns;
    config.Xs = config_info->Xs;
    config.Y = config_info->Y;
    _cprimod_to_cprif_gcw_mask(config_info->mask, &config.mask);

    if (config_info->bfn0_filter_enable) {
        config.bfn0_filter_enable = 1;
    } else {
        config.bfn0_filter_enable = 0;
    }

    if (config_info->bfn1_filter_enable) {
        config.bfn1_filter_enable = 1;
    } else {
        config.bfn1_filter_enable = 0;
    }

    if (config_info->repeat_enable){
        config.repeat_mode = CPRIF_CW_REPEAT_MODE_CONTINUOUS;
    } else {
        config.repeat_mode = CPRIF_CW_REPEAT_MODE_ONCE;
    }
    config.hfn_index = config_info->hyper_frame_index;
    config.hfn_modulo = config_info->hyper_frame_modulo;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_gcw_config_set(unit, port,
                                     index, &config));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_tx_gcw_config_get(int unit, int port, uint32 index, cprimod_cpri_tx_gcw_config_t* config_info)
{
    cprif_cpri_tx_gcw_config_t config;
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_GCW_MAX_GROUP) {
        LOG_CLI(("GCW index  =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    sal_memset(&config, 0, sizeof(cprif_cpri_tx_gcw_config_t));
    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_gcw_config_get(unit, port,
                                     index, &config));
    config_info->Ns = config.Ns;
    config_info->Xs = config.Xs;
    config_info->Y = config.Y;
    _cprif_to_cprimod_gcw_mask(config.mask, &config_info->mask);

    if (config.repeat_mode == CPRIF_CW_REPEAT_MODE_CONTINUOUS){
        config_info->repeat_enable = 1;
    } else {
        config_info->repeat_enable = 0;
    }

    if (config.bfn0_filter_enable) {
        config_info->bfn0_filter_enable = 1;
    } else {
        config_info->bfn0_filter_enable = 0;
    }

    if (config.bfn1_filter_enable) {
        config_info->bfn1_filter_enable = 1;
    } else {
        config_info->bfn1_filter_enable = 0;
    }

    config_info->hyper_frame_index = config.hfn_index;
    config_info->hyper_frame_modulo = config.hfn_modulo;

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_tx_gcw_filter_set(int unit, int port, uint32 bfn0_value, uint32 bfn0_mask, uint32 bfn1_value, uint32 bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_gcw_filter_set(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_gcw_filter_get(int unit, int port, uint32* bfn0_value, uint32* bfn0_mask, uint32* bfn1_value, uint32* bfn1_mask)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_gcw_filter_get(unit, port, bfn0_value, bfn0_mask, bfn1_value, bfn1_mask));


exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_control_word_l1_inband_info_set(int unit, int port, const cprimod_cpri_cw_l1_inband_info_t* inband_info)
{
    cprif_drv_tx_control_word_t control;
    SOC_INIT_FUNC_DEFS;

    sal_memset(&control, 0, sizeof(cprif_drv_tx_control_word_t));
    control.enet_ptr        = inband_info->eth_ptr;
    control.l1_fun          = inband_info->layer1_function;
    control.hdlc_rate       = inband_info->hdlc_rate;
    control.protocol_ver    = inband_info->protocol_ver;
    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_control_word_set(unit, port, &control));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_tx_control_word_l1_inband_info_get(int unit, int port, cprimod_cpri_cw_l1_inband_info_t* inband_info)
{
    cprif_drv_tx_control_word_t control;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_control_word_get(unit, port, &control));
    inband_info->eth_ptr            = control.enet_ptr;
    inband_info->layer1_function    = control.l1_fun;
    inband_info->hdlc_rate          = control.hdlc_rate;
    inband_info->protocol_ver       = control.protocol_ver;
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_rx_control_message_config_set(int unit, int port, uint32 queue_num, uint32 default_tag, uint32 no_match_tag)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_control_message_config_set( unit, port,
                                                        queue_num,
                                                        default_tag,
                                                        no_match_tag));
exit:
    SOC_FUNC_RETURN;
}

void _cprimod_to_cprif_proc_type( cprimod_control_msg_proc_type_t cprimod_mode, uint32* cprif_mode)
{
    switch (cprimod_mode) {
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_FE:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_ETH;
            break;
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_FCB:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_FCB;
            break;
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_PAYLOAD:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_PAYLOAD;
            break;
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG;
            break;
        case CPRIMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE_W_TAG:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG_WITH_TAG;
            break;
        default:
            *cprif_mode = CPRIF_CTRL_FLOW_PROC_TYPE_ETH;
            break;
    }
}

void _cprif_to_cprimod_proc_type( uint32 cprif_mode, cprimod_control_msg_proc_type_t* cprimod_mode)
{
    switch (cprif_mode) {
        case CPRIF_CTRL_FLOW_PROC_TYPE_ETH:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FE;
            break;
        case CPRIF_CTRL_FLOW_PROC_TYPE_FCB:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FCB;
            break;
        case CPRIF_CTRL_FLOW_PROC_TYPE_PAYLOAD:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_PAYLOAD;
            break;
        case CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE;
            break;
        case CPRIF_CTRL_FLOW_PROC_TYPE_FULL_MSG_WITH_TAG:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FULL_MESSAGE_W_TAG;
            break;
        default:
            *cprimod_mode = CPRIMOD_CONTROL_MSG_PROC_TYPE_FE;
            break;
    }
}

int cprif_rsvd4_rx_control_flow_config_set(int unit, int port, uint32 flow_id, cprimod_control_flow_config_t* config)
{
    cprif_rsvd4_rx_ctrl_flow_config_t config_info;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_CTRL_FLOW_MAX) {
        LOG_CLI(("control flow flow_id  =%d is out of range.\n",flow_id));
        return SOC_E_PARAM;
    }

    sal_memset(&config_info, 0, sizeof(cprif_rsvd4_rx_ctrl_flow_config_t));

    _cprimod_to_cprif_proc_type(config->proc_type, &config_info.proc_type);
    config_info.queue_num       = config->queue_num;
    config_info.sync_profile    = config->sync_profile;
    config_info.sync_enable     = config->sync_enable;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_control_flow_config_set ( unit, port,
                                                     flow_id,
                                                     &config_info));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_rx_control_flow_config_get(int unit, int port, uint32 flow_id, cprimod_control_flow_config_t* config)
{
    cprif_rsvd4_rx_ctrl_flow_config_t config_info;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_CTRL_FLOW_MAX) {
        LOG_CLI(("control flow flow_id  =%d is out of range.\n",flow_id));
        return SOC_E_PARAM;
    }

    sal_memset(&config_info, 0, sizeof(cprif_rsvd4_rx_ctrl_flow_config_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_rx_control_flow_config_get(unit, port,
                                                    flow_id,
                                                    &config_info));
    _cprif_to_cprimod_proc_type(config_info.proc_type, &config->proc_type);
    config->queue_num       = config_info.queue_num;
    config->sync_profile    = config_info.sync_profile;
    config->sync_enable     = config_info.sync_enable;
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_rx_sync_profile_entry_set(int unit, int port, uint32 profile_id, const cprimod_sync_profile_entry_t* entry)
{
    cprif_rsvd4_rx_sync_profile_entry_t int_entry;
    SOC_INIT_FUNC_DEFS;

    if (profile_id >= CPRIF_SYNC_PROFILE_MAX) {
        LOG_CLI(("sync profile_id  =%d is out of range.\n",profile_id));
        return SOC_E_PARAM;
    }
    sal_memset(&int_entry, 0, sizeof(cprif_rsvd4_rx_sync_profile_entry_t));

    int_entry.bfn_offset  = entry->master_frame_offset;
    int_entry.rfrm_offset = entry->message_offset;

    if (entry->count_cycle == CPRIMOD_SYNC_COUNT_CYCLE_UP_TO_6MF){
        int_entry.count_cycle = CPRIF_SYNC_PROFILE_UP_TO_6MF;
    } else {
        int_entry.count_cycle = CPRIF_SYNC_PROFILE_EVERY_MF;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_sync_profile_entry_set(unit, port, profile_id, &int_entry));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_rx_sync_profile_entry_get(int unit, int port, uint32 profile_id, cprimod_sync_profile_entry_t* entry)
{
    cprif_rsvd4_rx_sync_profile_entry_t int_entry;
    SOC_INIT_FUNC_DEFS;

    if (profile_id >= CPRIF_SYNC_PROFILE_MAX) {
        LOG_CLI(("sync profile_id  =%d is out of range.\n",profile_id));
        return SOC_E_PARAM;
    }

    sal_memset(&int_entry, 0, sizeof(cprif_rsvd4_rx_sync_profile_entry_t));

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_sync_profile_entry_get(unit, port, profile_id, &int_entry));
    entry->master_frame_offset  = int_entry.bfn_offset;
    entry->message_offset       = int_entry.rfrm_offset;

    if ( int_entry.count_cycle == CPRIF_SYNC_PROFILE_UP_TO_6MF){
        entry->count_cycle = CPRIMOD_SYNC_COUNT_CYCLE_UP_TO_6MF;
    } else {
        entry->count_cycle = CPRIMOD_SYNC_COUNT_CYCLE_EVERY_MF;
    }

exit:
    SOC_FUNC_RETURN;

}
int cprif_rx_tag_config_set(int unit, int port, uint32 default_tag, uint32 no_match_tag)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_tag_config_set( unit, port, default_tag, no_match_tag));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_tag_gen_entry_add(int unit, int port, cprimod_tag_gen_entry_t* entry)
{
    cprif_rx_tag_gen_entry_t table_entry;
    int index;

    SOC_INIT_FUNC_DEFS;

    for(index=0; index < CPRIF_TAG_GEN_MAX_ENTRY; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_tag_gen_entry_get(unit, port, index, &table_entry));

        if (table_entry.valid == 0) {
            /* found an avaialble location */
            table_entry.valid           = 1;
            table_entry.mask            = entry->mask;
            table_entry.header          = entry->header;
            table_entry.tag_id          = entry->tag_id;
            table_entry.rtwp_word_count = entry->word_count;

            return (cprif_drv_rx_tag_gen_entry_set(unit, port, index, &table_entry));
        }
    }
    LOG_CLI(("No Space to add Tag Gen Entry  .\n"));
    return SOC_E_PARAM;
exit:
    SOC_FUNC_RETURN;
}

int cprif_rx_tag_gen_entry_delete(int unit, int port, cprimod_tag_gen_entry_t* entry)
{
    cprif_rx_tag_gen_entry_t table_entry;
    int index;

    SOC_INIT_FUNC_DEFS;

    for(index=0; index < CPRIF_TAG_GEN_MAX_ENTRY; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_tag_gen_entry_get(unit, port, index, &table_entry));

        if (table_entry.valid == 1) {
            /* found a valid entry */
            if ((table_entry.mask   == entry->mask)  &&
                (table_entry.header == entry->header) &&
                (table_entry.tag_id == entry->tag_id)){
                    /* Matching valid entry found, delete the entry by invalidating the entry.*/
                    table_entry.valid = 0;
                    return (cprif_drv_rx_tag_gen_entry_set(unit, port, index, &table_entry));
                }
        }
    }
    LOG_CLI(("No Matching Entry Found  .\n"));
    return SOC_E_PARAM;
exit:
    SOC_FUNC_RETURN;

}

int cprif_rx_control_flow_tag_option_set(int unit, int port, uint32 flow_id, cprimod_cpri_tag_option_t tag_option)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_control_flow_tag_option_set(unit, port,
                                                  flow_id,
                                                  (tag_option == CPRIMOD_CPRI_TAG_LOOKUP)?1:0));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_control_flow_group_member_add(int unit, int port, uint32 group_id,
                                                uint32 priority, uint32 queue_num,
                                                cprimod_control_msg_proc_type_t proc_type)
{
    cprif_control_group_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (group_id >= CPRIF_TX_CONTROL_MAX_GROUP) {
        LOG_CLI(("RSVD4 Tx Control Group group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    if (priority >= CPRIF_TX_CONTROL_MAX_MEMBER) {
        LOG_CLI(("RSVD4 Tx Control Priority   =%d is out of range.\n",priority));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_group_entry_get(unit, port,group_id, &entry));

    entry.queue_num[priority] = queue_num;
    _cprimod_to_cprif_proc_type(proc_type, &entry.proc_type[priority]);
    entry.valid_mask |=  (0x1 << priority);

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_group_entry_set(unit, port,group_id, &entry));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_control_flow_group_member_delete(int unit, int port, uint32 group_id, uint32 priority)
{
    cprif_control_group_entry_t entry;
    SOC_INIT_FUNC_DEFS;

    if (group_id >= CPRIF_TX_CONTROL_MAX_GROUP) {
        LOG_CLI(("RSVD4 Tx Control Group group_id  =%d is out of range.\n",group_id));
        return SOC_E_PARAM;
    }

    if (priority >= CPRIF_TX_CONTROL_MAX_MEMBER) {
        LOG_CLI(("RSVD4 Tx Control Priority   =%d is out of range.\n",priority));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_group_entry_get(unit, port,group_id, &entry));

    /* masked off the valid bit. */
    entry.valid_mask &=  ~(0x1 << priority);

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_group_entry_set(unit, port,group_id, &entry));

exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_eth_message_config_set(int unit, int port, uint32 msg_node, uint32 msg_subnode, uint32 msg_type, uint32 msg_padding)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_control_eth_msg_config_set( unit, port,
                                                  msg_node, msg_subnode,
                                                  msg_type, msg_padding));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_single_raw_message_config_set(int unit, int port, uint32 msg_id, uint32 msg_type)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_control_single_msg_config_set(unit, port, msg_id, msg_type));

exit:
    SOC_FUNC_RETURN;

}


int cprif_rsvd4_tx_single_tunnel_message_config_set(int unit, int port, cprimod_cpri_crc_option_t crc_option)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_control_single_tunnel_msg_config_set(unit, port, (crc_option == CPRIMOD_CPRI_CRC_REGENERATE)?1:0));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_control_flow_config_set(int unit, int port, uint32 flow_id, const cprimod_rsvd4_tx_config_info_t* config_info)
{
    uint32 int_proc_type=0;
    cprif_cpri_tx_fast_eth_config_t tx_config;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_CTRL_FLOW_MAX) {
        LOG_CLI(("control flow flow_id  =%d is out of range.\n",flow_id));
        return SOC_E_PARAM;
    }

    if(config_info->proc_type == CPRIMOD_CONTROL_MSG_PROC_TYPE_FE){
      sal_memset(&tx_config, 0, sizeof(cprif_cpri_tx_fast_eth_config_t));
      tx_config.crc_mode  = config_info->crc_mode;
      tx_config.min_ipg  = 4;
      tx_config.queue_num  = 0x7f;/*defult value of reg*/
      tx_config.cw_sel  = 0;
      tx_config.cw_size  = 0;
      tx_config.sub_channel_size  = 0;
      tx_config.sub_channel_start  = 0x10;/*defult value of reg*/

      _SOC_IF_ERR_EXIT
          (cprif_drv_cpri_port_tx_cw_fast_eth_config_set(unit, port, &tx_config));
    }

    _cprimod_to_cprif_proc_type(config_info->proc_type, &int_proc_type);

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_flow_config_set( unit, port, flow_id, config_info->queue_num, int_proc_type));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_control_flow_config_get(int unit, int port, uint32 flow_id, cprimod_rsvd4_tx_config_info_t config_info)
{
    uint32 int_proc_type;
    uint32 queue_num;
    SOC_INIT_FUNC_DEFS;

    if (flow_id >= CPRIF_CTRL_FLOW_MAX) {
        LOG_CLI(("control flow flow_id  =%d is out of range.\n",flow_id));
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_tx_control_flow_config_get( unit, port, flow_id, &queue_num, &int_proc_type));

    config_info.queue_num = queue_num;

    _cprif_to_cprimod_proc_type(int_proc_type, &config_info.proc_type);

exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_tx_control_flow_header_index_set(int unit, int port, uint32 roe_flow_id, uint32 index)
{
    SOC_INIT_FUNC_DEFS;

    if (roe_flow_id >= CPRIF_CTRL_MAX_ROE_FLOW_ID) {
        LOG_CLI(("control flow roe_flow_id  =%d is out of range.\n",roe_flow_id));
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_control_flow_header_index_set( unit, port,
                                                             roe_flow_id,
                                                             index));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_tx_control_flow_header_index_get(int unit, int port, uint32 roe_flow_id, uint32* index)
{
    SOC_INIT_FUNC_DEFS;

    if (roe_flow_id >= CPRIF_CTRL_MAX_ROE_FLOW_ID) {
        LOG_CLI(("control flow roe_flow_id  =%d is out of range.\n",roe_flow_id));
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_control_flow_header_index_get( unit, port,
                                                             roe_flow_id,
                                                             index));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_tx_control_header_entry_set(int unit, int port, uint32 index, uint32 header_node, uint32 header_subnode, uint32 payload_node)
{
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_TX_HEADER_CONFIG_TABLE_SIZE) {
        LOG_CLI(("header look up index =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_control_header_entry_set( unit, port,
                                                        index,
                                                        header_node,
                                                        header_subnode,
                                                        payload_node));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_tx_control_header_entry_get(int unit, int port, uint32 index, uint32* header_node, uint32* header_subnode, uint32* payload_node)
{
    SOC_INIT_FUNC_DEFS;

    if (index >= CPRIF_TX_HEADER_CONFIG_TABLE_SIZE) {
        LOG_CLI(("header look up index =%d is out of range.\n",index));
        return SOC_E_PARAM;
    }
    _SOC_IF_ERR_EXIT
        (cprif_drv_brcm_rsvd4_control_header_entry_get( unit, port,
                                                        index,
                                                        header_node,
                                                        header_subnode,
                                                        payload_node));
exit:
    SOC_FUNC_RETURN;
}

int cprif_rsvd4_fast_eth_config_set(int unit, int port, const cprimod_cpri_fast_eth_config_t* config_info)
{
    cprif_cpri_rx_fast_eth_config_t rx_config;
    SOC_INIT_FUNC_DEFS;

    rx_config.queue_num             = config_info->queue_num ; /* Not used for for RSVD4 */
    rx_config.ignore_fcs_err        = config_info->no_fcs_err_check ;
    rx_config.min_packet_drop       = config_info->min_packet_drop ;
    rx_config.max_packet_drop       = config_info->max_packet_drop ;
    rx_config.min_packet_size       = config_info->min_packet_size ;
    rx_config.max_packet_size       = config_info->max_packet_size ;
    rx_config.strip_crc             = config_info->strip_crc;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_rx_cw_fast_eth_config_set(unit, port, &rx_config));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_or_enet_port_set(int unit, int port, int val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT (cprif_drv_cpri_or_enet_port_set(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_or_enet_port_get(int unit, int port, int* val)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT (cprif_drv_cpri_or_enet_port_get(unit, port, val));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_rx_enable_set(int unit, int port, int val)
{
    int axc_id;
    int queue;
    int vsd_raw_index;
    cprif_container_parser_sync_info_t sync_info[CPRIF_MAX_NUM_OF_AXC];
    uint32 copy_id;
    uint32 table_index;
    cprimod_port_interface_config_t interface_config;
    uint32 hdlc_size=0;
    uint32 fast_eth_size=0;
    uint32 ctrl_mask[4]={0};
    uint32 rsvd5_schan_size =0;
    uint32 vsd_cw_sel[CPRIF_VSD_RAW_MAX_GROUP]={0};
    uint32 vsd_cw_size[CPRIF_VSD_RAW_MAX_GROUP]={0};
    uint32 header_compare_valid_bit[CPRIF_HEADER_COMPARE_TABLE_SIZE]={0};
    SOC_INIT_FUNC_DEFS;

    if (val) {
        /*
         * Rx Enable, WAR for pointer corruption.
         * save settings and disable all axc and control flows.
         * clear the pipeline.
         */
        _SOC_IF_ERR_EXIT
            (cprif_port_rx_interface_config_get(unit, port, &interface_config));

        if (interface_config.interface == cprimodRsvd4) {
            /*
             * Invalidate all the header compare entries.
             */
            for (table_index = 0; table_index < CPRIF_MAX_NUM_OF_AXC ; table_index++) {
                header_compare_valid_bit[table_index] = 0;
                _SOC_IF_ERR_EXIT
                    (cprif_drv_rsvd4_header_compare_entry_valid_bit_swap(unit, port,
                                                                        table_index,
                                                                        &header_compare_valid_bit[table_index]));
            }
        } else if (interface_config.interface == cprimodCpri) {
            /*
             *  program all AxC with sync condition which cannot be true.
             */
            for (axc_id = 0; axc_id < CPRIF_MAX_NUM_OF_AXC ; axc_id++) {
                sync_info[axc_id].hfn_offset    = 0xFF;
                sync_info[axc_id].rfrm_sync     = 0;
                sync_info[axc_id].hfrm_sync     = 0;
                _SOC_IF_ERR_EXIT
                    (cprif_drv_cpri_container_parser_sync_info_swap(unit, port,
                                                                    axc_id,
                                                                    &sync_info[axc_id]));
            }
            /*
             *  Set all control word sizes to 0.
             *  HDLC, Fast ETh, VSD, VSD raw, RSVD5.
             */
            hdlc_size       = 0;
            fast_eth_size   = 0;
            _SOC_IF_ERR_EXIT
                (cprif_drv_cpri_port_rx_cw_hdlc_fast_eth_sizes_swap(unit, port, &hdlc_size, &fast_eth_size));

            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_copy_id_get(unit, port, &copy_id));
            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_ctrl_mask_set(unit, port, (copy_id == 0) ? 1:0, ctrl_mask));
            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_copy_id_set(unit, port, (copy_id == 0) ? 1:0));

            for (vsd_raw_index = 0; vsd_raw_index < CPRIF_VSD_RAW_MAX_GROUP; vsd_raw_index++) {
                vsd_cw_sel[vsd_raw_index]   = 0;
                vsd_cw_size[vsd_raw_index]  = 0;
                _SOC_IF_ERR_EXIT
                    (cprif_drv_rx_vsd_raw_config_cw_sel_size_swap(unit, port, vsd_raw_index, 
                                                                  &vsd_cw_sel[vsd_raw_index],
                                                                  &vsd_cw_size[vsd_raw_index]));
            }

            rsvd5_schan_size = 0;
            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_brcm_rsvd5_config_size_swap (unit, port, &rsvd5_schan_size));
        }

        /* clear the rx framer internal state. */
        for (axc_id = 0; axc_id < CPRIF_MAX_NUM_OF_AXC; axc_id++) {
            (cprif_cpri_rx_framer_state_clear(unit, port, axc_id));
        }
        /* Clear the encap pipeline. */
        for (queue=0; queue < CPRIF_MAX_NUM_OF_QUEUES; queue++) {
            _SOC_IF_ERR_EXIT
                (cprif_encap_memory_clear(unit, port, queue));
        }
    }
    /* make val ^1 to convert enable to disable */
    _SOC_IF_ERR_EXIT (cprif_drv_rx_disable_set(unit, port, (val&1)^1));

    if (val) {
        /*
         * Part of WAR, restored the configuration.
         */
         if (interface_config.interface == cprimodCpri) {
            /*
             * restore the AxC sync mode.
             */
            for (axc_id = 0; axc_id < CPRIF_MAX_NUM_OF_AXC ; axc_id++) {
                _SOC_IF_ERR_EXIT
                    (cprif_drv_cpri_container_parser_sync_info_swap(unit, port,
                                                                    axc_id,
                                                                    &sync_info[axc_id]));
            }
            /*
             * Restore control word sizes.
             */

            _SOC_IF_ERR_EXIT
                (cprif_drv_cpri_port_rx_cw_hdlc_fast_eth_sizes_swap(unit, port, &hdlc_size, &fast_eth_size));

            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_vsd_ctrl_copy_id_set(unit, port, copy_id));

            for (vsd_raw_index = 0; vsd_raw_index < CPRIF_VSD_RAW_MAX_GROUP; vsd_raw_index++) {
                _SOC_IF_ERR_EXIT
                    (cprif_drv_rx_vsd_raw_config_cw_sel_size_swap(unit, port, vsd_raw_index, 
                                                                  &vsd_cw_sel[vsd_raw_index],
                                                                  &vsd_cw_size[vsd_raw_index]));
            }

            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_brcm_rsvd5_config_size_swap (unit, port, &rsvd5_schan_size));

        } else if (interface_config.interface == cprimodRsvd4) {
            for (table_index = 0; table_index < CPRIF_MAX_NUM_OF_AXC ; table_index++) {
                if (header_compare_valid_bit[table_index] != 0 ){
                    _SOC_IF_ERR_EXIT
                        (cprif_drv_rsvd4_header_compare_entry_valid_bit_swap(unit, port,
                                                                        table_index,
                                                                        &header_compare_valid_bit[table_index]));
                }
            }
        }
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_enable_get(int unit, int port, int* val)
{
    int tmpval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT (cprif_drv_rx_disable_get(unit, port, &tmpval));
    /* make val ^1 to convert disable to enable */
    *val = (tmpval&1)^1;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_enable_set(int unit, int port, int val)
{
    SOC_INIT_FUNC_DEFS;

    /* make val ^1 to convert enable to disable */
    _SOC_IF_ERR_EXIT(cprif_drv_tx_frame_optional_config_set(unit, port,
                     cprimodTxConfigtxpmdDisableOverrideVal, (val&1)^1));
    _SOC_IF_ERR_EXIT(cprif_drv_tx_frame_optional_config_set(unit, port,
                     cprimodTxConfigtxpmdDisableOverrideEn, (val&1)^1));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_enable_get(int unit, int port, int* val)
{
    uint32 tmpval0, tmpval1;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_tx_frame_optional_config_get(unit, port,
                     cprimodTxConfigtxpmdDisableOverrideVal, &tmpval0));
    _SOC_IF_ERR_EXIT(cprif_drv_tx_frame_optional_config_get(unit, port,
                     cprimodTxConfigtxpmdDisableOverrideEn, &tmpval1));
    /* make val ^1 to convert disable to enable */
    *val = (tmpval0 & tmpval1 & 1) ^ 1;

exit:
    SOC_FUNC_RETURN;

}

STATIC
int _cprif_cpri_port_intr_hierarchy_enable(int unit, int port, int enable)
{
    SOC_INIT_FUNC_DEFS;

    /*
     * Enable/Disable the RX/TX interrupts in the
     * interrupts hierarchy for the CPM.
     */

    /* Enable Level 1 Port RX/TX interrupts */
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_interrupt_enable_set(unit, port,
                                              (CPRIF_RX_INTR | CPRIF_TX_INTR),
                                              enable));

    /*
     * Enable All Level 2 Port RX interrupts.
     * Rx operational error interrupts
     * Rx functional error interrupts
     * Not enabling the Rx ecc error interrupts.
     */
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_rx_interrupts_enable_set(unit, port,
                                                     (CPRIF_RX_OP_ERR_INTR |
                                                      CPRIF_RX_FUNC_ERR_INTR),
                                                      enable));

    /*
     * Enable All Level 3 Port RX interrupts.
     * All Rx operational error interrupts
     * All Rx functional error interrupts
     * Not enabling the Rx ecc error interrupts.
     */
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_rx_op_err_interrupts_enable_set(unit,
                                  port, CPRI_INTR_RX_OP_ERR_INTR_ALL, enable));


    _SOC_IF_ERR_EXIT(
            cprif_drv_cpri_port_rx_func_interrupts_enable_set(unit, port,
                                                      CPRI_INTR_RX_FUNC_ERR_INTR_ALL,
                                                      enable));

    /*
     * Enable All Level 2 Port TX interrupts.
     * Tx operational error interrupts
     * Tx functional error interrupts
     * Not enabling the Rx ecc error interrupts.
     */
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_tx_interrupts_enable_set(unit, port,
                                                     (CPRIF_TX_OP_ERR_INTR |
                                                      CPRIF_TX_FUNC_ERR_INTR),
                                                      enable));

    /*
     * Enable All Level 3 Port TX interrupts.
     * All Tx operational error interrupts
     * All Tx functional error interrupts
     * Not enabling the Rx ecc error interrupts.
     */
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_tx_op_err_interrupts_enable_set(unit,
                                  port, CPRI_INTR_TX_OP_ERR_INTR_ALL, enable));

    _SOC_IF_ERR_EXIT(
            cprif_drv_cpri_port_tx_func_interrupts_enable_set(unit, port,
                                                      CPRI_INTR_TX_FUNC_ERR_INTR_ALL,
                                                      enable));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_interrupt_link_status_get(int unit, int port,
                                              int *is_link_intr_status)
{
    int link_up = 0, link_down = 0;
    SOC_INIT_FUNC_DEFS;

    /* Check if link is up */
    _SOC_IF_ERR_EXIT(
        cprif_drv_rx_pcs_link_up_intr_status_clr_get(unit, port, &link_up));

    /* Check if link is down */
    _SOC_IF_ERR_EXIT(
        cprif_drv_rx_pcs_link_down_intr_status_clr_get(unit, port, &link_down));

    /* Capture if there is any change in the link status */
    *is_link_intr_status = link_up | link_down;

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_rx_1588_fifo_intr_enable_get(int unit, int port,
                                                 int *status)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_rx_1588_fifo_intr_enable_get(unit, port,
                                                            status));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_rx_1588_sw_intr_enable_get(int unit, int port,
                                                 int *status)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_rx_1588_sw_intr_enable_get(unit, port,
                                                          status));

exit:
    SOC_FUNC_RETURN;
}


int cprif_cpri_port_interrupt_enable_set(int unit, int port,
                                   cprimod_cpri_port_intr_type_t intr_type,
                                   int data, int enable)
{
    SOC_INIT_FUNC_DEFS;

    switch(intr_type) {
        case _shrCpriIntrRx1588TsFifo:
        case _shrCpriIntrRx1588CapturedTs:
        case _shrCpriIntrRxPcs64B66BBlkLckLl:
        case _shrCpriIntrRxPcs64B66BBlkLckLh:
        case _shrCpriIntrRxPcs64B66BHiBerLl:
        case _shrCpriIntrRxPcs64B66BHiBerLh:
        case _shrCpriIntrRxPcsLosLl:
        case _shrCpriIntrRxPcsLosLh:
        case _shrCpriIntrRxPcsLinkStatusLl:
        case _shrCpriIntrRxPcsLinkStatusLh:
        case _shrCpriIntrRxPcsExtractFifoOvrFlow:
        case _shrCpriIntrRxGcw:
        case _shrCpriIntrRxEncapDataQOverflow:
        case _shrCpriIntrRxEncapCtrlQOverflow:
        case _shrCpriIntrRxEncapGsmTsQErr:
        case _shrCpriIntrRxEncapWqFiFoOverflow:
        case _shrCpriIntrRxEncapRxFrmInFifoOverflow:
        case _shrCpriIntrRxEncapIpsmCdcFifoErr:
        case _shrCpriIntrRxFrmRsvd4Ts:
        case _shrCpriIntrRxFrmRsvd4SingleTsErr:
        case _shrCpriIntrRxFrmL1SigChange:
        case _shrCpriIntrRxPcsFecCsCwBad:
        case _shrCpriIntrTxDecapDataQOvflErr:
        case _shrCpriIntrTxDecapDataQUdflErr:
        case _shrCpriIntrTxDecapDataQBufSizeErr:
        case _shrCpriIntrTxDecapAgModePktMiss:
        case _shrCpriIntrTx1588CapturedTs:
        case _shrCpriIntrTx1588TsFifo:
        case _shrCpriIntrTxFrmCwaCtrlPktFlowIdErr:
        case _shrCpriIntrTxFrmBfaHfStartMisalign:
        case _shrCpriIntrTxFrmDrMultiEthPsizeUndersize:
        case _shrCpriIntrTxFrmDrAckMisalign:
        case _shrCpriIntrTxFrmCwaMultipleValids:
        case _shrCpriIntrTxFrmCwaSdvmProcOverflow:
        case _shrCpriIntrTxFrmArbPsizeZero:
        case _shrCpriIntrTxFrmArbCpriAckMismatch:
        case _shrCpriIntrTxFrmArbRsvd4ReqConflict:
        case _shrCpriIntrTxFrmArbRsvd4AckMisalign:
        case _shrCpriIntrTxFrmCwaHdlcBufferOverflow:
        case _shrCpriIntrTxFrmCwaFastEthBufferOverflow:
        case _shrCpriIntrTxFrmCwaVsdRawBufferOverflow:
        case _shrCpriIntrTxFrmCwaSdvmBufferOverflow:
        case _shrCpriIntrTxFrmMultiEthBufferOverflow:
        case _shrCpriIntrTxPcsGboxOverflow:
        case _shrCpriIntrTxPcsGboxUnderflow:
        case _shrCpriIntrTxPcsFifoOverflow:
        case _shrCpriIntrTxPcsFifoUnderflow:
            _SOC_IF_ERR_EXIT(
                        cprif_drv_cpri_port_interrupt_type_enable_set(unit,
                                            port, intr_type, data, enable));
            break;
        case _shrCpriIntrCount:
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Unknow interrupt type."));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_interrupt_enable_get(int unit, int port,
                                   cprimod_cpri_port_intr_type_t intr_type,
                                   int data, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    switch(intr_type) {
        case _shrCpriIntrRx1588TsFifo:
        case _shrCpriIntrRx1588CapturedTs:
        case _shrCpriIntrRxPcs64B66BBlkLckLl:
        case _shrCpriIntrRxPcs64B66BBlkLckLh:
        case _shrCpriIntrRxPcs64B66BHiBerLl:
        case _shrCpriIntrRxPcs64B66BHiBerLh:
        case _shrCpriIntrRxPcsLosLl:
        case _shrCpriIntrRxPcsLosLh:
        case _shrCpriIntrRxPcsLinkStatusLl:
        case _shrCpriIntrRxPcsLinkStatusLh:
        case _shrCpriIntrRxPcsExtractFifoOvrFlow:
        case _shrCpriIntrRxGcw:
        case _shrCpriIntrRxEncapDataQOverflow:
        case _shrCpriIntrRxEncapCtrlQOverflow:
        case _shrCpriIntrRxEncapGsmTsQErr:
        case _shrCpriIntrRxEncapWqFiFoOverflow:
        case _shrCpriIntrRxEncapRxFrmInFifoOverflow:
        case _shrCpriIntrRxEncapIpsmCdcFifoErr:
        case _shrCpriIntrRxFrmRsvd4Ts:
        case _shrCpriIntrRxFrmRsvd4SingleTsErr:
        case _shrCpriIntrRxFrmL1SigChange:
        case _shrCpriIntrRxPcsFecCsCwBad:
        case _shrCpriIntrTxDecapDataQOvflErr:
        case _shrCpriIntrTxDecapDataQUdflErr:
        case _shrCpriIntrTxDecapDataQBufSizeErr:
        case _shrCpriIntrTxDecapAgModePktMiss:
        case _shrCpriIntrTx1588CapturedTs:
        case _shrCpriIntrTx1588TsFifo:
        case _shrCpriIntrTxFrmCwaCtrlPktFlowIdErr:
        case _shrCpriIntrTxFrmBfaHfStartMisalign:
        case _shrCpriIntrTxFrmDrMultiEthPsizeUndersize:
        case _shrCpriIntrTxFrmDrAckMisalign:
        case _shrCpriIntrTxFrmCwaMultipleValids:
        case _shrCpriIntrTxFrmCwaSdvmProcOverflow:
        case _shrCpriIntrTxFrmArbPsizeZero:
        case _shrCpriIntrTxFrmArbCpriAckMismatch:
        case _shrCpriIntrTxFrmArbRsvd4ReqConflict:
        case _shrCpriIntrTxFrmArbRsvd4AckMisalign:
        case _shrCpriIntrTxFrmCwaHdlcBufferOverflow:
        case _shrCpriIntrTxFrmCwaFastEthBufferOverflow:
        case _shrCpriIntrTxFrmCwaVsdRawBufferOverflow:
        case _shrCpriIntrTxFrmCwaSdvmBufferOverflow:
        case _shrCpriIntrTxFrmMultiEthBufferOverflow:
        case _shrCpriIntrTxPcsGboxOverflow:
        case _shrCpriIntrTxPcsGboxUnderflow:
        case _shrCpriIntrTxPcsFifoOverflow:
        case _shrCpriIntrTxPcsFifoUnderflow:
            _SOC_IF_ERR_EXIT(
                        cprif_drv_cpri_port_interrupt_type_enable_get(unit,
                                            port, intr_type, data, enable));
            break;
        case _shrCpriIntrCount:
        default:
            _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("Unknow interrupt type."));
            break;
    }

exit:
    SOC_FUNC_RETURN;
}
STATIC
int _cprif_cpri_1588_timestamp_interrupt_process ( int unit, int port,
                                                   cprimod_direction_t dir,
                                                   cprimod_1588_ts_type_t ts_type )
{

    int enable = 0;
    int max_buf = 0;
    cprimod_cpri_intr_info_t data;
    uint64 timestamps[CPRIMOD_INTR_DATA_NUM_OF_DATA64];
    uint8 num_timestamps=0;
    _shr_cpri_interrupt_type_t intr_type = _shrCpriIntrCount;
    int index;
    SOC_INIT_FUNC_DEFS;

    if (ts_type == CPRIMOD_1588_TS_TYPE_SW_CAP) {
        if (dir == CPRIMOD_DIR_RX) {
            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_1588_sw_intr_enable_get(unit, port, &enable));
            intr_type = _shrCpriIntrRx1588CapturedTs;
        } else {
            _SOC_IF_ERR_EXIT
                (cprif_drv_tx_1588_sw_intr_enable_get(unit, port, &enable));
            intr_type = _shrCpriIntrTx1588CapturedTs;
        }
        max_buf = 1;
    } else if (ts_type == CPRIMOD_1588_TS_TYPE_FIFO) {
        if (dir == CPRIMOD_DIR_RX) {
            _SOC_IF_ERR_EXIT
                (cprif_drv_rx_1588_fifo_intr_enable_get(unit, port, &enable));
            intr_type = _shrCpriIntrRx1588TsFifo;
        } else {
            _SOC_IF_ERR_EXIT
                (cprif_drv_tx_1588_fifo_intr_enable_get(unit, port, &enable));
            intr_type = _shrCpriIntrTx1588TsFifo;
        }
        max_buf = CPRIMOD_INTR_DATA_NUM_OF_DATA64;
    }

    if (enable) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_1588_captured_timestamp_get(unit, port,
                                                   dir,
                                                   ts_type,
                                                   max_buf,
                                                   &num_timestamps,
                                                   timestamps));
        if (num_timestamps > 0) {
            data.cpri_intr_type = intr_type;
            data.data           = num_timestamps;
            for (index=0; index < num_timestamps ; index++) {
                COMPILER_64_COPY(data.data64[index], timestamps[index]);
            }
            if (cpri_intr_cbs[unit][intr_type].cb_func != NULL) {
                cpri_intr_cbs[unit][intr_type].cb_func(unit, port, &data, cpri_intr_cbs[unit][intr_type].cb_data);
            }
        }
    }

exit:
    SOC_FUNC_RETURN;
}

STATIC
int _cprif_cpri_port_interrupt_type_process(int unit, int port,
                                            cprimod_cpri_port_intr_type_t intr_type)
{
    int index;
    int intr_enable = 0;
    int intr_status = 0;
    int num_subid = 0;
    cprimod_cpri_intr_info_t data;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_port_interrupt_type_num_subid_get(unit, port, intr_type, &num_subid));

    for (index=0; index <= num_subid; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_cpri_port_interrupt_type_enable_get(unit, port,
                                                           intr_type,
                                                           index,
                                                           &intr_enable));
        if (intr_enable) {
            _SOC_IF_ERR_EXIT
                (cprif_drv_cpri_port_interrupt_type_status_get(unit, port,
                                                               intr_type,
                                                               index,
                                                               &intr_status));
            if (intr_status) {
                data.cpri_intr_type = intr_type;
                data.axc_id         = index;
                data.queue_num      = index;
                if (cpri_intr_cbs[unit][intr_type].cb_func != NULL) {
                    cpri_intr_cbs[unit][intr_type].cb_func(unit, port, &data, cpri_intr_cbs[unit][intr_type].cb_data);
                }
            }
        }
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_interrupt_process(int unit, int port)
{
    cprimod_cpri_port_intr_type_t intr_type;
    SOC_INIT_FUNC_DEFS;

    /*
     *  1588 Rx FIFO Timestamp Processing.
     */
    _SOC_IF_ERR_EXIT
        (_cprif_cpri_1588_timestamp_interrupt_process(unit, port,
                                                     CPRIMOD_DIR_RX,
                                                     CPRIMOD_1588_TS_TYPE_FIFO));
    /*
     *  1588 Tx FIFO Timestamp Processing.
     */
    _SOC_IF_ERR_EXIT
        (_cprif_cpri_1588_timestamp_interrupt_process(unit, port,
                                                     CPRIMOD_DIR_TX,
                                                     CPRIMOD_1588_TS_TYPE_FIFO));
    /*
     *  1588 Rx Capture Timestamp Processing.
     */
    _SOC_IF_ERR_EXIT
        (_cprif_cpri_1588_timestamp_interrupt_process(unit, port,
                                                     CPRIMOD_DIR_RX,
                                                     CPRIMOD_1588_TS_TYPE_SW_CAP));
    /*
     *  1588 Tx Capture Timestamp Processing.
     */
    _SOC_IF_ERR_EXIT
        (_cprif_cpri_1588_timestamp_interrupt_process(unit, port,
                                                     CPRIMOD_DIR_TX,
                                                     CPRIMOD_1588_TS_TYPE_SW_CAP));

    /*
     * Process all other interrupts except above 4 types.
     */

    for (intr_type = 0; intr_type < _shrCpriIntrCount; intr_type++) {

        if ( (intr_type != _shrCpriIntrRx1588CapturedTs) &&
             (intr_type != _shrCpriIntrTx1588CapturedTs) &&
             (intr_type != _shrCpriIntrRx1588TsFifo) &&
             (intr_type != _shrCpriIntrTx1588TsFifo)) {

            _SOC_IF_ERR_EXIT
                (_cprif_cpri_port_interrupt_type_process(unit, port, intr_type));
        }
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_1588_bit_time_period_set(int unit, int port, cprimod_direction_t direction, cprimod_port_speed_t speed)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_bit_time_period_set(unit, port, direction, speed));

exit:
    SOC_FUNC_RETURN;

}

int cprif_1588_bit_time_period_get(int unit, int port, cprimod_direction_t direction, uint32* bit_time_period)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_bit_time_period_get(unit, port, direction, bit_time_period));

exit:
    SOC_FUNC_RETURN;

}

int cprif_1588_timestamp_capture_config_set(int unit, int port, cprimod_direction_t direction, cprimod_1588_capture_config_t *config)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_timestamp_capture_config_set(unit, port, direction, config));
exit:
    SOC_FUNC_RETURN;

}
int cprif_1588_timestamp_capture_config_get(int unit, int port, cprimod_direction_t direction, cprimod_1588_capture_config_t* config)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_timestamp_capture_config_get(unit, port, direction, config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_1588_captured_timestamp_get(int unit, int port, cprimod_direction_t direction, cprimod_1588_ts_type_t ts_type, uint8 mx_cnt, uint8* count, uint64* captured_time)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_captured_timestamp_get(unit, port, direction, ts_type, mx_cnt, count, captured_time));
exit:
    SOC_FUNC_RETURN;

}

int cprif_1588_timestamp_adjust_set(int unit, int port, cprimod_direction_t direction, cprimod_1588_time_t *adjust_time)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_timestamp_adjust_set(unit, port, direction, adjust_time));
exit:
    SOC_FUNC_RETURN;

}
int cprif_1588_timestamp_adjust_get(int unit, int port, cprimod_direction_t direction, cprimod_1588_time_t* adjust_time)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_timestamp_adjust_get(unit, port, direction, adjust_time));
exit:
    SOC_FUNC_RETURN;

}

int cprif_1588_timestamp_fifo_config_set(int unit, int port, cprimod_direction_t direction, uint16 modulo)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_timestamp_fifo_config_set(unit, port, direction, modulo));
exit:
    SOC_FUNC_RETURN;

}

int cprif_1588_timestamp_fifo_config_get(int unit, int port, cprimod_direction_t direction, uint16* modulo)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_timestamp_fifo_config_get(unit, port, direction, modulo));
exit:
    SOC_FUNC_RETURN;

}

int cprif_1588_timestamp_cmic_config_set(int unit, int port, cprimod_direction_t direction, uint16 modulo)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_timestamp_cmic_config_set(unit, port, direction, modulo));

exit:
    SOC_FUNC_RETURN;

}

int cprif_1588_timestamp_cmic_config_get(int unit, int port, cprimod_direction_t direction, uint16* modulo)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(
        cprif_drv_1588_timestamp_cmic_config_get(unit, port, direction, modulo));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_basic_frame_info_get (int unit, int port, cprimod_direction_t dir,
                                uint32* frame_len_in_bits,
                                uint32* control_word_len_in_bits)
{
    cprimod_port_speed_t speed;
    cprimod_port_interface_type_t interface;

    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN(cprif_drv_txcpri_port_type_get(unit, port,
                                                       &interface));
    if (interface == cprimodCpri) {
        if (dir == CPRIMOD_DIR_TX) {
          _SOC_IF_ERR_EXIT(cprif_drv_cpri_txpcs_speed_get(unit, port, &speed));
        } else {
          _SOC_IF_ERR_EXIT(cprif_drv_cpri_rxpcs_speed_get(unit, port, &speed));
        }
        *frame_len_in_bits          = _cprif_from_speed_toframe_len(speed);
        *control_word_len_in_bits   = _cprif_from_speed_towd_len(speed);
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_agnostic_mode_basic_frame_config_set(int unit, int port, cprimod_direction_t dir, int enable)
{
    cprimod_port_speed_t speed;
    uint32 cw_len;
    uint32 frame_len;
    int bits_cnt;
    int current_index=1;
    uint8 active_table;
    uint8 standby_table;
    cprif_bfa_bfp_table_entry_t table_entry;

    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {

        /*
         * Setup Rx Basic Frame Parser Table.
         */
        _SOC_IF_ERR_EXIT(cprif_drv_cpri_rxpcs_speed_get(unit, port, &speed));
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));

        /*
         * Adding control word.
         */
        cw_len = _cprif_from_speed_towd_len(speed);
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, CPRIMOD_AXC_ID_CONTROL,
                                                  0, cw_len));

        if (enable) {
            /*
             * Adding data AxCs for the rest of the basic frame.
             */
            frame_len = _cprif_from_speed_toframe_len(speed);
            _SOC_IF_ERR_EXIT
                (cprif_rx_axc_basic_frame_add(unit, port, CPRIMOD_AXC_ID_AGNOSTIC,
                                                          cw_len, frame_len-cw_len));
        }
        /*
         * Set the   control word to be forwarded to encap with
         * AxC ID is 0.
         */

        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_framer_agnostic_mode_set(unit, port, 0 /* AxC_ID */, enable, cprimod_agnostic_mode_cpri, FALSE));

    } else { /* CPRIMOD_DIR_TX case */

        /*
         * Setup Tx Basic Frame Assebler Table.
         */

        _SOC_IF_ERR_EXIT(cprif_drv_cpri_txpcs_speed_get(unit, port, &speed));
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));

        if (enable) {
            /*
             * Adding AxC for the whole frame. Not require to add control frame as it
             * will come from RoE frame. Need to set the control word size to 0.
             */
            frame_len = _cprif_from_speed_toframe_len(speed);

            /*
             * Special Table handling for BFA table.
             */
            cprif_bfa_bfp_table_entry_t_init(&table_entry);
            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_assembly_active_table_get(unit, port, &active_table));

            if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
                standby_table = CPRIF_BASIC_FRAME_TABLE_1;
            } else {
                standby_table = CPRIF_BASIC_FRAME_TABLE_0;
            }

            bits_cnt = frame_len;
            table_entry.axc_id = CPRIMOD_AXC_ID_AGNOSTIC;
            table_entry.rsrv_bit_cnt = 0;
            while (bits_cnt) {
                if (bits_cnt > CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY) {
                    table_entry.data_bit_cnt = CPRIMOD_BASIC_FRAME_MAX_BITS_PER_ENTRY ;
                } else {
                    table_entry.data_bit_cnt =  bits_cnt;
                }

                bits_cnt -= table_entry.data_bit_cnt;
                _SOC_IF_ERR_EXIT
                    (cprif_drv_basic_frame_entry_set(unit, port,
                                                     standby_table,
                                                     CPRIMOD_DIR_TX,
                                                     current_index,
                                                     &table_entry));
                current_index++;
            }
            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port, standby_table, current_index));

            _SOC_IF_ERR_EXIT
                (cprif_drv_tx_control_word_size_set(unit, port, 0));
        } else { /* disable case */
            /*
             * Adding control word back, and set the control word size.
             */
            cw_len = _cprif_from_speed_towd_len(speed);
            _SOC_IF_ERR_EXIT
                (cprif_tx_axc_basic_frame_add(unit, port, CPRIMOD_AXC_ID_CONTROL,
                                              0, cw_len));
            _SOC_IF_ERR_EXIT
                (cprif_drv_tx_control_word_size_set(unit, port, cw_len/8));
        }
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_interface_bus_width_get(int unit, int port, int* is_10bit_mode)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT (cprif_drv_rxpcs_interface_width_get(unit, port, is_10bit_mode));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tunnel_mode_basic_frame_config_set(int unit, int port, cprimod_direction_t dir, int enable, int restore_rsvd4)
{
    uint8 active_table;
    uint8 standby_table;
    cprif_bfa_bfp_table_entry_t table_entry;
    int index=0;
    int is_10bit = -1;
    cprimod_port_interface_config_t interface_config;
    cprimod_port_speed_t speed=0;
    uint32 cw_len;

    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {

        /*
         * Setup Rx Basic Frame Parser Table.
         */

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_parser_active_table_get(unit, port, &active_table));

        if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
            standby_table = CPRIF_BASIC_FRAME_TABLE_1;
        } else {
            standby_table = CPRIF_BASIC_FRAME_TABLE_0;
        }

        _SOC_IF_ERR_EXIT (cprif_drv_rxpcs_interface_width_get(unit, port, &is_10bit));
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));

        if (enable) {

            cprif_bfa_bfp_table_entry_t_init(&table_entry);
            table_entry.axc_id = CPRIMOD_AXC_ID_AGNOSTIC;

            if (is_10bit) {
                table_entry.rsrv_bit_cnt = 54;
                table_entry.data_bit_cnt = 10;
            } else {
                table_entry.rsrv_bit_cnt = 24;
                table_entry.data_bit_cnt = 40;
            }

            for (index = 0; index < CPRIF_TUNNEL_MODE_NUM_OF_BFP_ENTRIES ; index++) {
                _SOC_IF_ERR_EXIT
                    (cprif_drv_basic_frame_entry_set(unit, port,
                                                     standby_table,
                                                     CPRIMOD_DIR_RX,
                                                     index,
                                                     &table_entry));
            }
            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port,
                                                                    standby_table,
                                                                    CPRIF_TUNNEL_MODE_NUM_OF_BFP_ENTRIES));
        } 

        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_framer_agnostic_mode_set(unit, port,
                                                   CPRIMOD_AXC_ID_AGNOSTIC,
                                                   enable,
                                                   cprimod_agnostic_mode_tunnel,
                                                   restore_rsvd4));

    } else { /* CPRIMOD_DIR_TX case */

        /*
         * Setup Tx Basic Frame Assebler Table.
         */

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_assembly_active_table_get(unit, port, &active_table));

        if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
            standby_table = CPRIF_BASIC_FRAME_TABLE_1;
        } else {
            standby_table = CPRIF_BASIC_FRAME_TABLE_0;
        }

        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));

        if (enable) {

            /* TX side is always 40 bits interface.  */
            cprif_bfa_bfp_table_entry_t_init(&table_entry);
            table_entry.axc_id = CPRIMOD_AXC_ID_AGNOSTIC;
            table_entry.rsrv_bit_cnt = 24;
            table_entry.data_bit_cnt = 40;

            for (index = 0; index < CPRIF_TUNNEL_MODE_NUM_OF_BFA_ENTRIES ; index++) {
                _SOC_IF_ERR_EXIT
                    (cprif_drv_basic_frame_entry_set(unit, port,
                                                     standby_table,
                                                     CPRIMOD_DIR_TX,
                                                     index,
                                                     &table_entry));
            }
            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port,
                                                                       standby_table,
                                                                       CPRIF_TUNNEL_MODE_NUM_OF_BFA_ENTRIES));
            _SOC_IF_ERR_EXIT
                (cprif_drv_tx_control_word_size_set(unit, port, 0));

            /*
             *  Enable the RSVD4 agnostic mode, which disable the RSVD4 mode
             *  in  tx framer. It  need to be in non RSVD4 mode for RSVD4 agnostic
             *  mode and tunnel mode.
             */
            _SOC_IF_ERR_EXIT
                (cprif_drv_tx_framer_rsvd4_agnostic_mode_set(unit, port, enable));


        } else { /* Disable */

            _SOC_IF_ERR_EXIT
                (cprif_port_tx_interface_config_get(unit, port, &interface_config));
            if  (interface_config.port_encap_type == cprimodPortEncapRsvd4) {
                _SOC_IF_ERR_EXIT
                    (_cprif_port_rsvd4_basic_frame_table_set (unit, port, CPRIMOD_DIR_TX));
                /*
                 *  Disable the RSVD4 agnostic mode, which re-enable the RSVD4 mode
                 *  in  tx framer.
                 */
                _SOC_IF_ERR_EXIT
                    (cprif_drv_tx_framer_rsvd4_agnostic_mode_set(unit, port, enable));

            } else if  (interface_config.port_encap_type == cprimodPortEncapCpri){ /* CPRI port */
                _SOC_IF_ERR_EXIT
                    (cprif_drv_cpri_txpcs_speed_get(unit, port, &speed));
                cw_len = _cprif_from_speed_towd_len(speed);
                _SOC_IF_ERR_EXIT
                    (cprif_tx_axc_basic_frame_add(unit, port, CPRIMOD_AXC_ID_CONTROL, 0, cw_len));
                _SOC_IF_ERR_EXIT
                    (cprif_drv_tx_control_word_size_set(unit, port, cw_len/8));
            }
        }
    }
exit:
    SOC_FUNC_RETURN;

}


int cprif_rsvd4_agnostic_mode_basic_frame_config_set(int unit, int port, cprimod_direction_t dir, int enable)
{
    uint8 active_table;
    uint8 standby_table;
    cprif_bfa_bfp_table_entry_t table_entry;
    int index=0;

    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {

        _SOC_IF_ERR_EXIT
            (cprif_drv_rx_framer_agnostic_mode_set(unit, port,
                                                     CPRIMOD_AXC_ID_AGNOSTIC,
                                                     enable,
                                                     cprimod_agnostic_mode_rsvd4,
                                                     FALSE));
        /*
         * Setup Rx Basic Frame Parser Table.
         */

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_parser_active_table_get(unit, port, &active_table));

        if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
            standby_table = CPRIF_BASIC_FRAME_TABLE_1;
        } else {
            standby_table = CPRIF_BASIC_FRAME_TABLE_0;
        }

        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));

        if (enable) {

            cprif_bfa_bfp_table_entry_t_init(&table_entry);
            table_entry.axc_id = CPRIMOD_AXC_ID_AGNOSTIC;

            table_entry.rsrv_bit_cnt = 0;
            table_entry.data_bit_cnt = 128;

            for (index = 0; index < CPRIF_RSVD4_AGNOSTIC_MODE_NUM_OF_BFP_ENTRIES; index++) {
                _SOC_IF_ERR_EXIT
                    (cprif_drv_basic_frame_entry_set(unit, port,
                                                     standby_table,
                                                     CPRIMOD_DIR_RX,
                                                     index,
                                                     &table_entry));
            }
            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_parser_table_num_entries_set(unit, port,
                                                                    standby_table,
                                                                    CPRIF_RSVD4_AGNOSTIC_MODE_NUM_OF_BFP_ENTRIES));
        } 
    } else { /* CPRIMOD_DIR_TX case */

        /*
         * Setup Tx Basic Frame Assebler Table.
         */

        _SOC_IF_ERR_EXIT
            (cprif_drv_basic_frame_assembly_active_table_get(unit, port, &active_table));

        if (active_table == CPRIF_BASIC_FRAME_TABLE_0) {
            standby_table = CPRIF_BASIC_FRAME_TABLE_1;
        } else {
            standby_table = CPRIF_BASIC_FRAME_TABLE_0;
        }

        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));

        if (enable) {

            cprif_bfa_bfp_table_entry_t_init(&table_entry);
            table_entry.axc_id = CPRIMOD_AXC_ID_AGNOSTIC;
            table_entry.rsrv_bit_cnt = 0;
            table_entry.data_bit_cnt = 128;

            for (index = 0; index < CPRIF_RSVD4_AGNOSTIC_MODE_NUM_OF_BFA_ENTRIES; index++) {
                _SOC_IF_ERR_EXIT
                    (cprif_drv_basic_frame_entry_set(unit, port,
                                                     standby_table,
                                                     CPRIMOD_DIR_TX,
                                                     index,
                                                     &table_entry));
            }
            _SOC_IF_ERR_EXIT
                (cprif_drv_basic_frame_assembly_table_num_entries_set(unit, port,
                                                                      standby_table,
                                                                      CPRIF_RSVD4_AGNOSTIC_MODE_NUM_OF_BFA_ENTRIES));

        } else { /* disable case */
        }

        _SOC_IF_ERR_EXIT
            (cprif_drv_tx_framer_rsvd4_agnostic_mode_set(unit, port, enable));
    }

exit:
    SOC_FUNC_RETURN;
}

int cprif_encap_presentation_time_enable_set(int unit, int port, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_ingress_presentation_time_enable_set(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_presentation_time_enable_get(int unit, int port, int* enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_ingress_presentation_time_enable_get(unit, port, enable));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_presentation_time_config_set(int unit, int port, const cprimod_encap_presentation_time_config_t* config)
{
    cprif_cpri_ingress_pres_time_ctrl_t pres_time_config;
    SOC_INIT_FUNC_DEFS;

    pres_time_config.pres_mod_cnt   = config->mod_count;
    pres_time_config.bfn_offset     = config->radio_frame_offset;
    pres_time_config.hfn_offset     = config->hyper_frame_offset;


    pres_time_config.hfrm_sync = 0;
    pres_time_config.rfrm_sync = 0;

    if (config->frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_HYPER) {
        pres_time_config.hfrm_sync = 1;
        pres_time_config.rfrm_sync = 0; /* don't care */

    } else if (config->frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_RADIO) {
        pres_time_config.hfrm_sync = 0;
        pres_time_config.rfrm_sync = 1;
    } else if (config->frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_BASIC) {
        pres_time_config.hfrm_sync = 0;
        pres_time_config.rfrm_sync = 0;
    } else if (config->frame_sync_mode == CPRIMOD_CPRI_FRAME_SYNC_MODE_TUNNEL) {
        pres_time_config.hfrm_sync = 1;
        pres_time_config.rfrm_sync = 1;
    }

    pres_time_config.approx_inc_disable = config->approx_inc_disable;

    _SOC_IF_ERR_EXIT
        (cprif_drv_ingress_presentation_time_config_set(unit, port, &pres_time_config));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_presentation_time_config_get(int unit, int port, cprimod_encap_presentation_time_config_t* config)
{
    cprif_cpri_ingress_pres_time_ctrl_t pres_time_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_ingress_presentation_time_config_get(unit, port, &pres_time_config));

    config->mod_count            = pres_time_config.pres_mod_cnt;
    config->radio_frame_offset   = pres_time_config.bfn_offset;
    config->hyper_frame_offset   = pres_time_config.hfn_offset;

    if (pres_time_config.hfrm_sync == 1) {
        config->frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_HYPER;
    } else {
        if(pres_time_config.rfrm_sync == 1) {
            config->frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_RADIO;
        } else {
            config->frame_sync_mode = CPRIMOD_CPRI_FRAME_SYNC_MODE_BASIC;
        }
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_presentation_time_adjust_set(int unit, int port, uint32 time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_ingress_presentation_time_adjust_set(unit, port, time));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_presentation_time_adjust_get(int unit, int port, uint32* time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_ingress_presentation_time_adjust_get(unit, port, time));


exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_presentation_time_approximate_increment_set(int unit, int port, uint32 time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_ingress_presentation_time_approximate_increment_set(unit, port, time));

exit:
    SOC_FUNC_RETURN;

}

int cprif_encap_presentation_time_approximate_increment_get(int unit, int port, uint32* time)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_ingress_presentation_time_approximate_increment_get(unit, port, time));

exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_agnostic_mode_enable_set(int unit, int port, int enable, cprimod_cpri_roe_ordering_info_option_t mode)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_egress_agnostic_mode_enable_set(unit, port, enable, mode));

exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_agnostic_mode_enable_get(int unit, int port, int* enable, cprimod_cpri_roe_ordering_info_option_t* mode)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_egress_agnostic_mode_enable_get(unit, port, enable, mode));

exit:
    SOC_FUNC_RETURN;
}

int cprif_decap_agnostic_mode_config_set(int unit, int port, const cprimod_decap_agnostic_mode_config_t* config)
{
    cprif_cpri_egress_agnostic_config_t cprif_config;
    SOC_INIT_FUNC_DEFS;

    /*
     * calculate cycle size x buffer size to  # of 16 bytes buffer
     */

    cprif_config.rd_ptr_max = (CPRIF_NUM_OF_BUFFER_ENTRIES(config->packet_size) * config->mod_count);

    if ( cprif_config.rd_ptr_max >= CPRIF_DECAP_BUFFER_NUM_ENTRIES) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("\n Requested Buffer %d Max Buffer Entries %d, Packet Size %d bytes Cycle Size %d.\n",
                                          cprif_config.rd_ptr_max,
                                          CPRIF_DECAP_BUFFER_NUM_ENTRIES,
                                          config->packet_size,
                                          config->mod_count));
    }

    cprif_config.queue_num  = config->queue_num;
    cprif_config.mod_cnt    = config->mod_count;

    _SOC_IF_ERR_EXIT
        (cprif_drv_egress_agnostic_mode_config_set(unit, port, &cprif_config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_agnostic_mode_config_get(int unit, int port, cprimod_decap_agnostic_mode_config_t* config)
{
    cprif_cpri_egress_agnostic_config_t cprif_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_egress_agnostic_mode_config_get(unit, port, &cprif_config));
    config->packet_size     = ((cprif_config.rd_ptr_max)/cprif_config.mod_cnt)*CPRIF_DATA_BUFFER_BLOCK_SIZE;
    config->queue_num       = cprif_config.queue_num;
    config->mod_count       = cprif_config.mod_cnt;
exit:
    SOC_FUNC_RETURN;
}

int cprif_decap_presentation_time_config_set(int unit, int port, uint32 mod_offset, uint32 mod_count)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_egress_presentation_time_config_set(unit, port, mod_offset, mod_count));
exit:
    SOC_FUNC_RETURN;

}

int cprif_decap_presentation_time_config_get(int unit, int port, uint32* mod_offset, uint32* mod_count)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_egress_presentation_time_config_get(unit, port, mod_offset, mod_count));
exit:
    SOC_FUNC_RETURN;
}

int cprif_compression_lookup_table_set(int unit, int port, int table_depth, const uint32* table)
{
    int index =0;
    SOC_INIT_FUNC_DEFS;

    if (table_depth < CPRIF_COMPRESSION_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("\n Table Need 1024 entries."));
    }

    for (index = 0; index < CPRIF_COMPRESSION_TABLE_SIZE; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_compression_entry_set(unit, port, index, table[index], table[index]));
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_compression_lookup_table_get(int unit, int port, int max_depth, int* table_depth, uint32* table)
{
    int index =0;
    SOC_INIT_FUNC_DEFS;

    if (max_depth < CPRIF_COMPRESSION_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("\n Table Buffer Need 1024 entries."));
    }

    for (index = 0; index < CPRIF_COMPRESSION_TABLE_SIZE; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_compression_entry_get(unit, port, index, &table[index], &table[index]));
    }
    *table_depth = CPRIF_COMPRESSION_TABLE_SIZE;
exit:
    SOC_FUNC_RETURN;

}

int cprif_decompression_lookup_table_set(int unit, int port, int table_depth, const uint32* table)
{
   int index =0;
    SOC_INIT_FUNC_DEFS;

    if (table_depth < CPRIF_DECOMPRESSION_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("\n Table Need 256 entries."));
    }

    for (index = 0; index < CPRIF_DECOMPRESSION_TABLE_SIZE; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_decompression_entry_set(unit, port, index, table[index]));
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_decompression_lookup_table_get(int unit, int port, int max_depth, int* table_depth, uint32* table)
{
   int index =0;
    SOC_INIT_FUNC_DEFS;

    if (max_depth < CPRIF_DECOMPRESSION_TABLE_SIZE) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("\n Buffer Need 256 entries."));
    }

    for (index = 0; index < CPRIF_DECOMPRESSION_TABLE_SIZE; index++) {
        _SOC_IF_ERR_EXIT
            (cprif_drv_decompression_entry_get(unit, port, index, &table[index]));
    }
    *table_depth = CPRIF_DECOMPRESSION_TABLE_SIZE;
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_decap_queue_flow_control_set(int unit, int port, uint32 queue_num, int enable, uint32_t xon_threshold, uint32_t xoff_threshold)
{
    int index = 0;
    uint32 xon_num_buff;
    uint32 xoff_num_buff;
    SOC_INIT_FUNC_DEFS;

    if ((queue_num < CPRIF_CONTROL_FLOW_MIN_QUEUE_NUM) ||
        (queue_num >= CPRIF_MAX_NUM_OF_QUEUES)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("\n Cannot Support Flow Control on this Queue."));
    }
    index           = queue_num - CPRIF_CONTROL_FLOW_MIN_QUEUE_NUM;
    xon_num_buff    = CPRIF_NUM_OF_BUFFER_ENTRIES(xon_threshold);
    xoff_num_buff   = CPRIF_NUM_OF_BUFFER_ENTRIES(xoff_threshold);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_flow_control_entry_set(unit, port,
                                                      index,
                                                      enable,
                                                      queue_num,
                                                      xon_num_buff,
                                                      xoff_num_buff));



exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_decap_queue_flow_control_get(int unit, int port, uint32 queue_num, int* enable, uint32_t* xon_threshold, uint32_t* xoff_threshold)
{
    int index=0;
    uint32 hw_queue_num=0;
    uint32 xon_num_buff=0;
    uint32 xoff_num_buff=0;
    SOC_INIT_FUNC_DEFS;

    if ((queue_num < CPRIF_CONTROL_FLOW_MIN_QUEUE_NUM) ||
        (queue_num >= CPRIF_MAX_NUM_OF_QUEUES)) {
        _SOC_EXIT_WITH_ERR(SOC_E_PARAM, ("\n Cannot Support Flow Control on this Queue."));
    }
    index = queue_num - CPRIF_CONTROL_FLOW_MIN_QUEUE_NUM;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_queue_flow_control_entry_get(unit, port,
                                                      index,
                                                      enable,
                                                      &hw_queue_num,
                                                      &xon_num_buff,
                                                      &xoff_num_buff));
    *xon_threshold  = xon_num_buff  * CPRIF_DATA_BUFFER_BLOCK_SIZE;
    *xoff_threshold = xoff_num_buff * CPRIF_DATA_BUFFER_BLOCK_SIZE;

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_fec_enable_set(int unit, int port, cprimod_direction_t dir, int enable)
{
    cprimod_port_interface_config_t interface_config;
    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {
        _SOC_IF_ERR_EXIT
            (cprif_port_rx_interface_config_get(unit, port, &interface_config));
    } else { /* CPRIMOD_DIR_TX */
        _SOC_IF_ERR_EXIT
            (cprif_port_tx_interface_config_get(unit, port, &interface_config));
    }

    if (interface_config.speed != cprimodSpd24330p24) {
        _SOC_EXIT_WITH_ERR
            (SOC_E_UNAVAIL, ("\n FEC is only supported in 24330M speed on this device.."));
    }

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_fec_enable_set(unit, port, dir, enable));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_fec_enable_get(int unit, int port, cprimod_direction_t dir, int* enable)
{
    cprimod_port_interface_config_t interface_config;
    SOC_INIT_FUNC_DEFS;

    if (dir == CPRIMOD_DIR_RX) {
        _SOC_IF_ERR_EXIT
            (cprif_port_rx_interface_config_get(unit, port, &interface_config));
    } else { /* CPRIMOD_DIR_TX */
        _SOC_IF_ERR_EXIT
            (cprif_port_tx_interface_config_get(unit, port, &interface_config));
    }

    *enable = FALSE;
    if (interface_config.speed == cprimodSpd24330p24) {
        _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_fec_enable_get(unit, port, dir, enable));
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_fec_timestamp_config_set(int unit, int port, cprimod_direction_t dir, const cprimod_fec_timestamp_config_t* config)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_fec_timestamp_config_set(unit, port, dir, config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_fec_timestamp_config_get(int unit, int port, cprimod_direction_t dir, cprimod_fec_timestamp_config_t* config)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_fec_timestamp_config_get(unit, port, dir, config));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_fec_aux_config_set(int unit, int port, cprimod_fec_aux_config_para_t parameter_id, uint32 value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_fec_aux_config_set(unit, port, parameter_id, value));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_fec_aux_config_get(int unit, int port, cprimod_fec_aux_config_para_t parameter_id, uint32* value)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_fec_aux_config_get(unit, port, parameter_id, value));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_fec_stat_get(int unit, int port, cprimod_cpri_fec_stat_type_t stat_type, uint32* value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_port_fec_stat_get(unit, port, stat_type, value));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_ecc_interrupt_enable_set(int unit, int port, int enable)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_ecc_interrupt_enable_set(unit,
                                                                port, enable));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_ecc_interrupt_status_get(int unit, int port,
                                    cprimod_cpri_ecc_intr_info_t *ecc_err_info)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_ecc_interrupt_status_get(unit, port,
                                                                ecc_err_info));

exit:
    SOC_FUNC_RETURN;
}

#ifdef CPRIMOD_SUPPORT_ECC_INJECT
int cprif_cpri_port_ecc_interrupt_test(int unit, int port)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT(cprif_drv_cpri_port_ecc_interrupt_test(unit, port));

exit:
    SOC_FUNC_RETURN;
}
#endif

int cprif_cpri_port_reset_set(int unit, int port, int rst)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_tx_h_reset_set(unit, port, rst));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_rx_h_reset_set(unit, port, rst));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_reset_get(int unit, int port, int* rst)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_rx_h_reset_get(unit, port, rst));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_init(int unit, int port)
{
    SOC_INIT_FUNC_DEFS;
    _SOC_IF_ERR_EXIT(cprif_drv_init(unit, port));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_rx_gcw_word_get(int unit, int port, int group_index, uint16* gcw_word)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_gcw_word_get(unit, port, group_index, gcw_word));

exit:
    SOC_FUNC_RETURN;

}


int cprif_cpri_port_tx_gcw_word_set(int unit, int port, int group_index, uint16 gcw_word)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_gcw_word_set(unit, port, group_index, gcw_word));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_mac_drain_start(int unit, int port)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_flush_set(unit, port, 1));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_mac_drain_stop(int unit, int port)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_tx_flush_set(unit, port, 0));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_mac_credit_reset_set(int unit, int port, int rst)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_datapath_credit_reset_set(unit, port, rst));


exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_mac_credit_reset_get(int unit, int port, int* rst)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_datapath_credit_reset_get(unit, port, rst));


exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_link_up_event(int unit, int port)
{
    SOC_INIT_FUNC_DEFS;

#if 0
    LOG_CLI(("Port(%s)-%d Link Up  Event \n",
        SOC_PORT_NAME(unit, port), port));
#endif
    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_byte_error_count_clear(unit, port));
    _SOC_IF_ERR_EXIT
        (cprif_drv_cpri_work_queue_interrupt_status_clear(unit, port));
exit:
    SOC_FUNC_RETURN;

}
 
int cprif_cpri_rx_datapath_reset(int unit, int port)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_port_rx_datapath_reset(unit, port, 1));
    _SOC_IF_ERR_EXIT
        (cprif_drv_port_rx_datapath_reset(unit, port, 0));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_tx_datapath_reset(int unit, int port)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_port_tx_datapath_reset(unit, port, 1));
    _SOC_IF_ERR_EXIT
        (cprif_drv_port_tx_datapath_reset(unit, port, 0));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_link_down_event(int unit, int port)
{
    uint32 queue;
    int axc_id;
    SOC_INIT_FUNC_DEFS;

#if 0
    LOG_CLI(("Port(%s)-%d Link Down  Resetting Port \n",
        SOC_PORT_NAME(unit, port), port));
#endif
    _SOC_IF_ERR_EXIT
        (cprif_cpri_rx_datapath_reset(unit, port));

    /* Clear the queue ordering info offset. */
    for (queue=0; queue < CPRIF_MAX_NUM_OF_QUEUES ; queue++) {
        _SOC_IF_ERR_EXIT 
            (cprif_encap_memory_clear(unit, port, queue));
    }
    for (axc_id = 0; axc_id < CPRIF_MAX_NUM_OF_AXC; axc_id++) {
        (cprif_cpri_rx_framer_state_clear(unit, port, axc_id));
    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_tunnel_mode_set(int unit, int port, int enable,
                                       uint32 payload_size,
                                       int restore_rsvd4,
                                       cprimod_port_speed_t restore_speed)
{
    uint32 timestamp_interval;
    SOC_INIT_FUNC_DEFS;

    timestamp_interval = payload_size;
    _SOC_IF_ERR_EXIT
        (cprif_drv_rxpcs_tunnel_mode_set(unit, port, enable, timestamp_interval, restore_rsvd4, restore_speed));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_tunnel_mode_get(int unit, int port, int* enable, uint32* payload_size)
{
    uint32 timestamp_interval;
    SOC_INIT_FUNC_DEFS;


    _SOC_IF_ERR_EXIT
        (cprif_drv_rxpcs_tunnel_mode_get(unit, port, enable, &timestamp_interval));

    if (*enable) {
        *payload_size = timestamp_interval;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_agnostic_mode_set(int unit, int port,
                                         int enable,
                                         cprimod_agnostic_mode_type_t mode,
                                         uint32 payload_size,
                                         int restore_rsvd4,
                                         cprimod_port_speed_t restore_speed)
{
    uint32 timestamp_interval;
    SOC_INIT_FUNC_DEFS;

    timestamp_interval = payload_size;
    _SOC_IF_ERR_EXIT
        (cprif_drv_txpcs_agnostic_mode_set(unit, port,
                                           enable,
                                           mode,
                                           timestamp_interval,
                                           restore_rsvd4,
                                           restore_speed));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_tx_agnostic_mode_get(int unit, int port, int* enable, cprimod_agnostic_mode_type_t* mode, uint32* payload_size)
{
    uint32 timestamp_interval;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_txpcs_agnostic_mode_get(unit, port, enable, mode, &timestamp_interval));

    if (*enable) {
        *payload_size = timestamp_interval;
    }

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_agnostic_mode_get(int unit, int port, int* enable, cprimod_agnostic_mode_type_t* mode)
{
    uint32 axc_id=0xfe;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rx_framer_agnostic_mode_get(unit, port, &axc_id, enable, mode));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_link_get(int unit, int port, int* link)
{
    int tunnel_mode=FALSE;
    uint32 payload;
    uint32 value=0;
    SOC_INIT_FUNC_DEFS;

    SOC_IF_ERROR_RETURN
        (cprif_cpri_port_rx_tunnel_mode_get(unit, port, &tunnel_mode, &payload));

    /*
     * If tunnel mode, get status from PMD since PCS is bypassed.
     */
    if (tunnel_mode) {
        _SOC_IF_ERR_EXIT
            (cprif_port_pmd_status_get(unit, port,cprimodPmdPortStatusRxLock, &value));
    } else {
        _SOC_IF_ERR_EXIT
            (cprif_port_rx_pcs_status_get(unit, port, cprimodRxPcsStatusLinkStatusLive, &value));
    }

    *link = (value != 0)? 1:0;
exit:
    SOC_FUNC_RETURN;

}

uint32 _cprimod_to_cprif_drv_roe_header_field(cprimod_cpri_roe_header_field_t cprimod_field)
{
    uint32 cprif_drv_field;
 
    switch (cprimod_field) {
        case CPRIMOD_CPRI_ROE_HEADER_FIELD_SUBTYPE:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_SUBTYPE;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_FLOW_ID:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_FLOW_ID;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_15_8:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_LENGTH_15_8;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_7_0:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_LENGTH_7_0;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_31_24:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_31_24;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_23_16:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_23_16;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_15_8:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_15_8;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_7_0:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_7_0;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_OPCODE:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_OPCODE;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_UNMAPPED:
        default:
            cprif_drv_field = CPRIF_ENCAP_HEADER_FIELD_ID_UNMAPPED;
            break;
    }
    return cprif_drv_field;
}

cprimod_cpri_roe_header_field_t _cprif_drv_to_cprimod_roe_header_field(uint32 cprif_drv_field)
{
    cprimod_cpri_roe_header_field_t cprimod_field;

    switch (cprif_drv_field) {
        case CPRIF_ENCAP_HEADER_FIELD_ID_SUBTYPE:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_SUBTYPE;
            break;

        case CPRIF_ENCAP_HEADER_FIELD_ID_FLOW_ID:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_FLOW_ID;
            break;

        case CPRIF_ENCAP_HEADER_FIELD_ID_LENGTH_15_8:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_15_8;
            break;

        case CPRIF_ENCAP_HEADER_FIELD_ID_LENGTH_7_0:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_7_0;
            break;

        case CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_31_24:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_31_24;
            break;

        case CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_23_16:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_23_16;
            break;

        case CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_15_8:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_15_8;
            break;

        case CPRIF_ENCAP_HEADER_FIELD_ID_ORDER_INFO_7_0:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_7_0;
            break;

        case CPRIF_ENCAP_HEADER_FIELD_ID_OPCODE:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_OPCODE;
            break;

        case CPRIF_ENCAP_HEADER_FIELD_ID_UNMAPPED:
        default:
            cprimod_field = CPRIMOD_CPRI_ROE_HEADER_FIELD_UNMAPPED;
            break;
    }
    return cprimod_field;

}

uint32 _cprimod_to_cprif_drv_ecpri_header_field(cprimod_cpri_ecpri_header_field_t cprimod_field)
{
    uint32 cprif_drv_field;

    switch (cprimod_field) {
        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_0:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_0;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_1;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_2:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_2;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_3:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_3;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_4:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_4;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_5:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_5;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_6:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_6;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_7:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_7;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_8:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_8;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_9:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_9;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_10:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_10;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_11:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_11;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_VLAN_0_BYTE_0:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_VLAN_0_BYTE_0;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_VLAN_0_BYTE_1:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_VLAN_0_BYTE_1;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_VLAN_1_BYTE_0:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_VLAN_1_BYTE_0;
            break;

        case CPRIMOD_CPRI_ECPRI_HEADER_FIELD_VLAN_1_BYTE_1:
            cprif_drv_field = CPRIF_DECAP_HEADER_FIELD_ID_VLAN_1_BYTE_1;
            break;
        default:
            cprif_drv_field = 0x9;
    }

    return cprif_drv_field;

}

cprimod_cpri_ecpri_header_field_t _cprif_drv_to_cprimod_ecpri_header_field(uint32 cprif_drv_field)
{
    cprimod_cpri_ecpri_header_field_t cprimod_field;
    switch (cprif_drv_field) {
        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_0:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_0;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_1:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_2:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_3:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_4:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_5:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_6:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_7:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_8:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_9:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_10:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_HDR_BYTE_11:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_VLAN_0_BYTE_0:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_VLAN_0_BYTE_0;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_VLAN_0_BYTE_1:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_VLAN_0_BYTE_1;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_VLAN_1_BYTE_0:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_VLAN_1_BYTE_0;
            break;

        case CPRIF_DECAP_HEADER_FIELD_ID_VLAN_1_BYTE_1:
            cprimod_field = CPRIMOD_CPRI_ECPRI_HEADER_FIELD_VLAN_1_BYTE_1;
            break;
        default:
            cprimod_field = 0x9;
    }

    return cprimod_field;

}

int cprif_cpri_encap_header_field_mapping_set(int unit, int port, const cprimod_cpri_encap_header_field_mapping_t* config_mapping)
{
    cprif_drv_encap_hdr_field_mapping_t drv_mapping;
    SOC_INIT_FUNC_DEFS;

    drv_mapping.hdr_byte0_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte0_sel);
    drv_mapping.hdr_byte1_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte1_sel);
    drv_mapping.hdr_byte2_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte2_sel);
    drv_mapping.hdr_byte3_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte3_sel);
    drv_mapping.hdr_byte4_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte4_sel);
    drv_mapping.hdr_byte5_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte5_sel);
    drv_mapping.hdr_byte6_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte6_sel);
    drv_mapping.hdr_byte7_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte7_sel);
    drv_mapping.hdr_byte8_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte8_sel);
    drv_mapping.hdr_byte9_sel  = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte9_sel);
    drv_mapping.hdr_byte10_sel = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte10_sel);
    drv_mapping.hdr_byte11_sel = _cprimod_to_cprif_drv_roe_header_field(config_mapping->hdr_byte11_sel);

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_field_mapping_set(unit, port, &drv_mapping));


exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_encap_header_field_mapping_get(int unit, int port, cprimod_cpri_encap_header_field_mapping_t* config_mapping)
{
    cprif_drv_encap_hdr_field_mapping_t drv_mapping;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_encap_header_field_mapping_get(unit, port, &drv_mapping));

    config_mapping->hdr_byte0_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte0_sel);
    config_mapping->hdr_byte1_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte1_sel);
    config_mapping->hdr_byte2_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte2_sel);
    config_mapping->hdr_byte3_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte3_sel);
    config_mapping->hdr_byte4_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte4_sel);
    config_mapping->hdr_byte5_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte5_sel);
    config_mapping->hdr_byte6_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte6_sel);
    config_mapping->hdr_byte7_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte7_sel);
    config_mapping->hdr_byte8_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte8_sel);
    config_mapping->hdr_byte9_sel  = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte9_sel);
    config_mapping->hdr_byte10_sel = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte10_sel);
    config_mapping->hdr_byte11_sel = _cprif_drv_to_cprimod_roe_header_field(drv_mapping.hdr_byte11_sel);

exit:
    SOC_FUNC_RETURN;

}


int cprif_cpri_decap_header_field_mapping_set(int unit, int port, const cprimod_cpri_decap_header_field_mapping_t* config_mapping)
{
    cprif_drv_decap_hdr_field_mapping_t drv_mapping;
    SOC_INIT_FUNC_DEFS;

    drv_mapping.subtype             = _cprimod_to_cprif_drv_ecpri_header_field(config_mapping->subtype_sel);
    drv_mapping.flowid              = _cprimod_to_cprif_drv_ecpri_header_field(config_mapping->flowid_sel);
    drv_mapping.length_15_8         = _cprimod_to_cprif_drv_ecpri_header_field(config_mapping->length_15_8_sel );
    drv_mapping.length_7_0          = _cprimod_to_cprif_drv_ecpri_header_field(config_mapping->length_7_0_sel);
    drv_mapping.orderinginfo_31_24  = _cprimod_to_cprif_drv_ecpri_header_field(config_mapping->orderinginfo_31_24_sel);
    drv_mapping.orderinginfo_23_16  = _cprimod_to_cprif_drv_ecpri_header_field(config_mapping->orderinginfo_23_16_sel );
    drv_mapping.orderinginfo_15_8   = _cprimod_to_cprif_drv_ecpri_header_field(config_mapping->orderinginfo_15_8_sel);
    drv_mapping.orderinginfo_7_0    = _cprimod_to_cprif_drv_ecpri_header_field(config_mapping->orderinginfo_7_0_sel);

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_header_field_mapping_set(unit, port, &drv_mapping));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_decap_header_field_mapping_get(int unit, int port, cprimod_cpri_decap_header_field_mapping_t* config_mapping)
{
    cprif_drv_decap_hdr_field_mapping_t drv_mapping;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_header_field_mapping_get(unit, port, &drv_mapping));

    config_mapping->subtype_sel             = _cprif_drv_to_cprimod_ecpri_header_field(drv_mapping.subtype) ;
    config_mapping->flowid_sel              = _cprif_drv_to_cprimod_ecpri_header_field(drv_mapping.flowid) ;
    config_mapping->length_15_8_sel         = _cprif_drv_to_cprimod_ecpri_header_field(drv_mapping.length_15_8);
    config_mapping->length_7_0_sel          = _cprif_drv_to_cprimod_ecpri_header_field(drv_mapping.length_7_0) ;
    config_mapping->orderinginfo_31_24_sel  = _cprif_drv_to_cprimod_ecpri_header_field(drv_mapping.orderinginfo_31_24) ;
    config_mapping->orderinginfo_23_16_sel  = _cprif_drv_to_cprimod_ecpri_header_field(drv_mapping.orderinginfo_23_16);
    config_mapping->orderinginfo_15_8_sel   = _cprif_drv_to_cprimod_ecpri_header_field(drv_mapping.orderinginfo_15_8) ;
    config_mapping->orderinginfo_7_0_sel    = _cprif_drv_to_cprimod_ecpri_header_field(drv_mapping.orderinginfo_7_0) ;

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_decap_header_field_mask_set(int unit, int port, cprimod_cpri_roe_header_field_t field_id, uint32 mask )
{
    cprif_drv_decap_hdr_field_mapping_t mask_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_header_field_mask_get(unit, port, &mask_config));

    switch (field_id) {

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_SUBTYPE:
            mask_config.subtype  = mask;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_FLOW_ID:
            mask_config.flowid  = mask;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_15_8:
            mask_config.length_15_8  = mask;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_7_0:
            mask_config.length_7_0  = mask;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_31_24:
            mask_config.orderinginfo_31_24  = mask;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_23_16:
            mask_config.orderinginfo_23_16  = mask;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_15_8:
            mask_config.orderinginfo_15_8  = mask;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_7_0:
            mask_config.orderinginfo_7_0  = mask;
            break;
        default:
            break;
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_header_field_mask_set(unit, port, &mask_config));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_decap_header_field_mask_get(int unit, int port, cprimod_cpri_roe_header_field_t field_id, uint32* mask )
{
    cprif_drv_decap_hdr_field_mapping_t mask_config;
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decap_header_field_mask_get(unit, port, &mask_config));

    switch (field_id) {

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_SUBTYPE:
            *mask = mask_config.subtype;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_FLOW_ID:
            *mask = mask_config.flowid;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_15_8:
            *mask = mask_config.length_15_8;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_LENGTH_7_0:
            *mask = mask_config.length_7_0;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_31_24:
            *mask = mask_config.orderinginfo_31_24;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_23_16:
            *mask = mask_config.orderinginfo_23_16;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_15_8:
            *mask = mask_config.orderinginfo_15_8;
            break;

        case CPRIMOD_CPRI_ROE_HEADER_FIELD_ORDER_INFO_7_0:
            *mask = mask_config.orderinginfo_7_0;
            break;
        default:
            break;
    }
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_header_field_adjustment_config_set (int unit, int port,
                                                   cprimod_direction_t dir,
                                                   int ext_hdr_enable,
                                                   int len_field_adj)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_length_field_adjustment_set(unit, port, dir, len_field_adj));

    _SOC_IF_ERR_EXIT
        (cprif_drv_extended_header_enable_set(unit, port, dir, ext_hdr_enable));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_header_field_adjustment_config_get( int unit, int port,
                                                   cprimod_direction_t dir,
                                                   int* ext_hdr_enable,
                                                   int* len_field_adj)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_length_field_adjustment_get(unit, port, dir, len_field_adj));

    _SOC_IF_ERR_EXIT
        (cprif_drv_extended_header_enable_get(unit, port, dir, ext_hdr_enable));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_compression_saturation_config_set(int unit, int port, uint32 threshold_value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_compression_negative_saturation_threshold_set(unit, port, threshold_value));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_compression_saturation_config_get(int unit, int port, uint32* threshold_value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_compression_negative_saturation_threshold_get(unit, port, threshold_value));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_decompression_saturation_config_set(int unit, int port, uint32 sat_value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decompression_negative_saturation_value_set(unit, port, sat_value));
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_decompression_saturation_config_get(int unit, int port, uint32* sat_value)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_decompression_negative_saturation_value_get(unit, port, sat_value));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_padding_size_set(int unit, int port, uint32 padding_size)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_tx_padding_size_set(unit, port, padding_size));
exit:
    SOC_FUNC_RETURN;

}

int cprif_rsvd4_tx_padding_size_get(int unit, int port, uint32* padding_size)
{
    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_rsvd4_tx_padding_size_get(unit, port, padding_size));

exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_rx_pipeline_clear(int unit, int port)
{
    int queue;
    int axc_id;

    SOC_INIT_FUNC_DEFS;

    _SOC_IF_ERR_EXIT
        (cprif_drv_port_rx_pmd_datapath_reset(unit, port, 1));

    _SOC_IF_ERR_EXIT
        (cprif_drv_port_rx_datapath_reset(unit, port, 1));

    /* Clear the encap pipeline. */
    for (queue=0; queue < CPRIF_MAX_NUM_OF_QUEUES; queue++) {
        _SOC_IF_ERR_EXIT
            (cprif_encap_memory_clear(unit, port, queue));
    }
    /* clear the rx framer internal state. */
    for (axc_id = 0; axc_id < CPRIF_MAX_NUM_OF_AXC; axc_id++) {
        (cprif_cpri_rx_framer_state_clear(unit, port, axc_id));
    }

    _SOC_IF_ERR_EXIT
        (cprif_drv_port_rx_datapath_reset(unit, port, 0));
    _SOC_IF_ERR_EXIT
        (cprif_drv_port_rx_pmd_datapath_reset(unit, port, 0));

exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_tx_pipeline_clear(int unit, int port)
{
    int queue;
    int axc_id;
    SOC_INIT_FUNC_DEFS;

    /* Reset Data Path. */
    _SOC_IF_ERR_EXIT
        (cprif_drv_port_tx_datapath_reset(unit, port, 1));
    /* Disable TGEN */
    _SOC_IF_ERR_EXIT
        (cprif_tx_framer_tgen_enable(unit, port, FALSE));

    /* Clear the encap pipeline. */
    for (queue=0; queue < CPRIF_MAX_NUM_OF_QUEUES ; queue++) {
        _SOC_IF_ERR_EXIT
            (cprif_decap_memory_clear(unit, port, queue));
    }

    /* clear the rx framer internal state. */
    for (axc_id = 0; axc_id < CPRIF_MAX_NUM_OF_AXC; axc_id++) {
        (cprif_cpri_tx_framer_state_clear(unit, port, axc_id));
    }
    /* Reset Data Path. */
    _SOC_IF_ERR_EXIT
        (cprif_drv_port_tx_datapath_reset(unit, port, 0));
exit:
    SOC_FUNC_RETURN;
}

int cprif_cpri_port_speed_set(int unit, int port, 
                              const phymod_phy_access_t* phy, 
                              const cprimod_port_init_config_t* port_config)

{
    uint32 wd_len = 0;
    uint32 pll_mode0 = 0;
    uint32 pll_mode1= 0;
    uint8  new_pll = 0;
    uint8  pll_lock_status=0;
    phymod_phy_access_t  phy_access_copy;
    struct falcon2_monterey_uc_lane_config_st serdes_firmware_config;
    cprimod_basic_frame_table_id_t table;
    SOC_INIT_FUNC_DEFS;

    sal_memcpy(&phy_access_copy, phy, sizeof(phy_access_copy));

    phy_access_copy.access.pll_idx = 0;
    falcon2_monterey_pll_mode_get(&phy_access_copy.access, &pll_mode0);

    phy_access_copy.access.pll_idx = 1;
    falcon2_monterey_pll_mode_get(&phy_access_copy.access, &pll_mode1);

    /* 
     * PLL_in_use tells which PLLs are in use, check if ether PLL is used 
     * And vco required is vco of one of the PLL then use matching PLL for 
     * the lane 
     */
    if (((port_config->pll_in_use & cprimodInUsePll0) != 0) &&
       (port_config->vco_for_lane == pll_mode0)) {
        new_pll = 0;
    } else if (((port_config->pll_in_use & cprimodInUsePll1) != 0) &&
              (port_config->vco_for_lane == pll_mode1)) {
        new_pll = 1;
    } else {

        /* 
         * If the PLL is not in use, check to see of the VCO is configure for
         * the new speed and is locked.
         */
        if (pll_mode0 == port_config->vco_for_lane) {
            phy_access_copy.access.pll_idx = 0;
            falcon2_monterey_pll_lock_get(&phy_access_copy.access, &pll_lock_status);
            if (pll_lock_status) {
                new_pll = 0;
            }
        } else if (pll_mode1 == port_config->vco_for_lane) {
            phy_access_copy.access.pll_idx = 1;
            falcon2_monterey_pll_lock_get(&phy_access_copy.access, &pll_lock_status);
            if (pll_lock_status) {
                new_pll = 1;
            }
        } else {
            LOG_DEBUG(BSL_LS_BCM_CPRI,
                (BSL_META_U(unit,
                    "There is no PLL to support this speed u=%d port=%d .\n"), unit, port));
            /*
             * This is not an error, just that there is no resource to support
             * the speed.
             */
            return SOC_E_NONE;
        }
    }

    SOC_IF_ERROR_RETURN(cprif_drv_pmd_ctrl_ln_por_h_rstb_set(unit, port));
    falcon2_monterey_lane_soft_reset_release(&phy->access, 0);
    falcon2_monterey_pll_select_set(&phy->access, new_pll);
    SOC_IF_ERROR_RETURN(cprif_drv_cip_clk_pll_select_set(unit, port,new_pll));

    serdes_firmware_config.field.lane_cfg_from_pcs = 0;
    serdes_firmware_config.field.an_enabled        = 0;
    serdes_firmware_config.field.dfe_on            = 1;
    serdes_firmware_config.field.dfe_lp_mode       = 0;
    serdes_firmware_config.field.force_brdfe_on    = 0;
    serdes_firmware_config.field.media_type        = phymodFirmwareMediaTypeOptics;
    serdes_firmware_config.field.unreliable_los    = 1;

    if((port_config->rx_cpri_speed == cprimodSpd10137p6)||
      (port_config->rx_cpri_speed == cprimodSpd12165p12)||
      (port_config->rx_cpri_speed == cprimodSpd24330p24)) {
        serdes_firmware_config.field.scrambling_dis    = 0;
    } else {
        serdes_firmware_config.field.scrambling_dis    = 1;
    }
    serdes_firmware_config.field.cl72_auto_polarity_en   = 0;
    serdes_firmware_config.field.cl72_restart_timeout_en = 0;
    serdes_firmware_config.word = 0;
    PHYMOD_IF_ERR_RETURN(falcon2_monterey_set_uc_lane_cfg(&phy->access, serdes_firmware_config));

    falcon2_monterey_lane_soft_reset_release(&phy->access, 1);

    SOC_IF_ERROR_RETURN(cprif_drv_tx_h_reset_set( unit, port, cpriResetEnAssert));
    SOC_IF_ERROR_RETURN(cprif_drv_rx_h_reset_set( unit, port, cpriResetEnAssert));

    SOC_IF_ERROR_RETURN(cprif_drv_cip_tx_h_reset_set(unit, port, cpriResetEnAssert));
    SOC_IF_ERROR_RETURN(cprif_drv_cip_rx_h_reset_set(unit, port, cpriResetEnAssert));

    SOC_IF_ERROR_RETURN(cprif_drv_datapath_tx_h_reset_set(unit, port, cpriResetEnAssert));
    SOC_IF_ERROR_RETURN(cprif_drv_datapath_rx_h_reset_set(unit, port, cpriResetEnAssert));

    SOC_IF_ERROR_RETURN(cprif_drv_tx_h_reset_set(unit, port, cpriResetEnDeassert));
    SOC_IF_ERROR_RETURN(cprif_drv_rx_h_reset_set(unit, port, cpriResetEnDeassert));

    SOC_IF_ERROR_RETURN(cprif_drv_cip_tx_h_reset_set(unit, port, cpriResetEnDeassert));
    SOC_IF_ERROR_RETURN(cprif_drv_cip_rx_h_reset_set(unit, port, cpriResetEnDeassert));

    SOC_IF_ERROR_RETURN(cprif_drv_datapath_tx_h_reset_set(unit, port, cpriResetEnDeassert));
    SOC_IF_ERROR_RETURN(cprif_drv_datapath_rx_h_reset_set(unit, port, cpriResetEnDeassert));


    falcon2_monterey_lane_soft_reset_release(&phy->access, 0);

    SOC_IF_ERROR_RETURN(_cprif_port_tx_speed_set(unit, port, phy,
                                                 port_config->tx_cpri_speed));
    SOC_IF_ERROR_RETURN(_cprif_port_rx_speed_set(unit, port, phy,
                                                 port_config->rx_cpri_speed));
    falcon2_monterey_lane_soft_reset_release(&phy->access, 1);

    if (port_config->interface == cprimodCpri) {
        _SOC_IF_ERR_EXIT
            (cprimod_basic_frame_parser_active_table_get(unit, port, &table));
        if (table == cprimod_basic_frame_table_0) {
            _SOC_IF_ERR_EXIT
                (cprif_basic_frame_parser_active_table_set(unit, port, cprimod_basic_frame_table_1, TRUE));
        } else {
            _SOC_IF_ERR_EXIT
                (cprif_basic_frame_parser_active_table_set(unit, port, cprimod_basic_frame_table_0, TRUE));
        }
        _SOC_IF_ERR_EXIT
            (cprif_basic_frame_assembly_active_table_get(unit, port, &table));
        if (table == cprimod_basic_frame_table_0) {
            _SOC_IF_ERR_EXIT
                (cprif_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_1, TRUE));
        } else {
            _SOC_IF_ERR_EXIT
                (cprif_basic_frame_assembly_active_table_set(unit, port,cprimod_basic_frame_table_0, TRUE));
        }
    } else {
        _SOC_IF_ERR_EXIT
            (cprif_basic_frame_parser_switch_active_table(unit, port));

        _SOC_IF_ERR_EXIT
            (cprif_basic_frame_assembly_switch_active_table(unit, port));
    }


    /* disable TGEN. */
    cprif_drv_tx_framer_tgen_enable(unit, port, 0);

    _SOC_IF_ERR_EXIT(cprif_drv_glas_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_cip_rx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_rx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_rx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_credit_reset_set(unit, port, 1));
    /* remove soft resets Rx path*/
    _SOC_IF_ERR_EXIT(cprif_drv_glas_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_cip_rx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_rx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_rx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_credit_reset_set(unit, port, 0));
    /* remove soft resets Tx path*/
    _SOC_IF_ERR_EXIT(cprif_drv_cip_tx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_tx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_tx_dp_reset_set(unit, port, 1));
    _SOC_IF_ERR_EXIT(cprif_drv_cip_tx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_datapath_tx_dp_reset_set(unit, port, 0));
    _SOC_IF_ERR_EXIT(cprif_drv_tx_dp_reset_set(unit, port, 0));


    if (port_config->interface == cprimodCpri) {
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_clear(unit, port));
        wd_len = _cprif_from_speed_towd_len(port_config->rx_cpri_speed);
        /* Adding AXC for control word */
        _SOC_IF_ERR_EXIT(cprif_rx_axc_basic_frame_add(unit, port, 255,
                                                      0, wd_len));
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_clear(unit, port));
        wd_len = _cprif_from_speed_towd_len(port_config->tx_cpri_speed);
        /* Adding AXC for control word */
        _SOC_IF_ERR_EXIT(cprif_tx_axc_basic_frame_add(unit, port, 255,
                                                      0, wd_len));
    } else {
        SOC_IF_ERROR_RETURN
            (_cprif_port_rsvd4_basic_frame_table_set(unit, port, CPRIMOD_DIR_RX));
        SOC_IF_ERROR_RETURN
            (_cprif_port_rsvd4_basic_frame_table_set(unit, port, CPRIMOD_DIR_TX));
    }
    /* 
     * clear the container parser and assembler table, in such a way that it 
     * will never sync.
     */
    _SOC_IF_ERR_EXIT
        (_cprif_container_config_clear(unit, port, port_config->interface)); 

    /* Enable CPRI Port Link Status Interrupts for HW Linkscan handling */


exit:
    SOC_FUNC_RETURN;
}



int _cprif_debug_memory_read (int  unit, int port, soc_mem_t mem , int index, uint32 *data)
{
    int phy_port = 0;
    int port_core_index = 0;
    soc_mem_t local_mem;
    uint32 blk_num = 0;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);
    local_mem = mem + port_core_index;
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    return (soc_mem_read(unit, local_mem, blk_num , index, data));
}

int _cprif_debug_memory_write (int  unit, int port, soc_mem_t mem , int index, const uint32 *data)
{
    int phy_port = 0;
    int port_core_index = 0;
    soc_mem_t local_mem;
    uint32 blk_num = 0;

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    port_core_index = ((phy_port-1)%4);
    local_mem = mem + port_core_index;
    blk_num = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);

    return (soc_mem_write(unit, local_mem, blk_num , index, (void *)data));
}

int cprif_cpri_port_debug_set(int unit, int port, cprimod_cpri_debug_attr_t type, int index, int buffer_size, const uint32* data)
{
    _cpri_debug_info_t *info;
    int entry_words = 0;
    uint64 reg_data;
    SOC_INIT_FUNC_DEFS;

    info = &_cpri_port_debug_info[type];

    if (info->write_supported) {
        if (info->reg != INVALIDr) {
            if (SOC_REG_IS_64(unit, info->reg)){
                entry_words = 2;
                COMPILER_64_SET(reg_data, data[1], data[0]);
            } else {
                entry_words = 1;
                COMPILER_64_SET(reg_data, 0, data[0]);
            }
            if (entry_words >  buffer_size) {
                _SOC_EXIT_WITH_ERR
                    (SOC_E_PARAM, (("\n Not Enoguh Data, need %d words."),entry_words));
            }

            _SOC_IF_ERR_EXIT
                (soc_reg_set(unit, info->reg, port, 0, reg_data));

        } else if (info->mem != INVALIDm) {
            entry_words = soc_mem_entry_words(unit, info->mem);

            if (entry_words >  buffer_size) {
                _SOC_EXIT_WITH_ERR
                    (SOC_E_PARAM, (("\n Not Enoguh data, need %d words."),entry_words));
            }
            _SOC_IF_ERR_EXIT
                (_cprif_debug_memory_write( unit, port, info->mem , index, data));
        }
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_UNAVAIL, ("\n Set is not Supported for this Debug."));

    }
exit:
    SOC_FUNC_RETURN;

}

int cprif_cpri_port_debug_get(int unit, int port, cprimod_cpri_debug_attr_t type, int index, int buffer_size, uint32* data)
{
    _cpri_debug_info_t *info;
    int entry_words = 0;
    uint64 reg_data;
    SOC_INIT_FUNC_DEFS;

    info = &_cpri_port_debug_info[type];

    if (info->reg != INVALIDr) {
        if (SOC_REG_IS_64(unit, info->reg)){
            entry_words = 2;
        } else {
            entry_words = 1;
        }
        if (entry_words >  buffer_size) {
            _SOC_EXIT_WITH_ERR
                (SOC_E_PARAM, (("\n Not Enoguh Space for data, need %d words."),entry_words));
        }

        _SOC_IF_ERR_EXIT
            (soc_reg_get(unit, info->reg, port, 0, &reg_data));
        sal_memcpy(data, &reg_data, entry_words *sizeof(uint32));

    } else if (info->mem != INVALIDm) {
        entry_words = soc_mem_entry_words(unit, info->mem);

        if (entry_words >  buffer_size) {
            _SOC_EXIT_WITH_ERR
                (SOC_E_PARAM, (("\n Not Enoguh Space for data, need %d words."),entry_words));
        }
        _SOC_IF_ERR_EXIT
            (_cprif_debug_memory_read( unit, port, info->mem , index, data));
    }

exit:
    SOC_FUNC_RETURN;

}


int cprif_cpri_port_debug_clear(int unit, int port, cprimod_cpri_debug_attr_t type, int index)
{
    _cpri_debug_info_t *info;
    uint64 reg_data;
    uint32 mem_data[5];
    SOC_INIT_FUNC_DEFS;

    info = &_cpri_port_debug_info[type];

    if (info->clear_supported) {
        if (info->reg != INVALIDr) {
            _SOC_IF_ERR_EXIT
                (soc_reg_get(unit, info->reg, port, 0, &reg_data));
            if (info->clear_field != INVALIDf) {
                soc_reg64_field_set(unit, info->reg, &reg_data, info->clear_field, 1);
                _SOC_IF_ERR_EXIT
                    (soc_reg_set(unit, info->reg, port, 0, reg_data));
            }
        } else if (info->mem != INVALIDm) {
            _SOC_IF_ERR_EXIT
                (_cprif_debug_memory_read( unit, port, info->mem , index, mem_data));
        }
    } else {
        _SOC_EXIT_WITH_ERR
            (SOC_E_UNAVAIL, ("\n Clear is not Supported for this Debug."));
    }
exit:
    SOC_FUNC_RETURN;
}

#endif /* CPRIMOD_CPRI_FALCON_SUPPORT */

#undef _ERR_MSG_MODULE_NAME
