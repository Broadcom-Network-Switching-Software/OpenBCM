/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        alloc_mngr.c
 * Purpose:     Resource allocation manager for SOC_SAND chips.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

/*#include <soc/error.h>*/
#include <bcm_int/common/debug.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/module.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/dpp_config_defs.h>
#include <bcm/switch.h>
#include <soc/dpp/PPC/ppc_api_lif.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_trill.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_eg_encap.h>


/* Simple LIF Allocation print */
#define _DPP_AM_LIF_ALLOCATION_DEBUG_PRINT 0

#define _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id) (object_id / _BCM_DPP_AM_EGRESS_ENCAP_NOF_ENTRIES_PER_BANK)

#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id) (object_id / _BCM_DPP_AM_INGRESS_LIF_NOF_ENTRIES_PER_BANK)
#define _BCM_DPP_AM_INGRESS_LIF_NOF_TABLES (4)
#define _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS_IN_TABLE (4)
#define _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(table_id) (table_id * _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS_IN_TABLE)
#define _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(table_id) (_BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(table_id) + _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS_IN_TABLE - 1)
#define _BCM_DPP_AM_INGRESS_LIF_BANK_ID_TO_TABLE_ID(bank_id) (bank_id / _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS_IN_TABLE)

/* Egress encapsulation end definiations */

/* Egress encapsulation functions { */
STATIC uint32
_bcm_dpp_am_egress_encap_internal_alloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int count, int* object);
STATIC uint32
_bcm_dpp_am_egress_encap_internal_alloc_align(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int align, int offset, int count, int* object);
STATIC uint32
_bcm_dpp_am_egress_encap_internal_dealloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, int count, int object);
/* Egress encapsulation functions } */

/* Ingress LIF end definiations */

/* Ingress LIF functions { */
STATIC uint32
_bcm_dpp_am_ingress_lif_internal_alloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int count, int* object);
STATIC uint32
_bcm_dpp_am_ingress_lif_internal_alloc_align(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int align, int offset, int count, int* object);
STATIC uint32
_bcm_dpp_am_ingress_lif_internal_dealloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, int count, int object);
/* Ingress LIF functions } */

/* Sync LIF start definiations { */

#define _BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK (2)
#define _BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MIN (1)
#define _BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MAX (1)
#define _BCM_DPP_AM_SYNC_LIF_TYPE_NOF_BITS (_BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MAX-_BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MIN+1)
#define _BCM_DPP_AM_SYNC_LIF_NOF_BITS (_BCM_DPP_AM_LIF_NOF_BANKS*_BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK) 


/* Sync LIF end definiations } */


/* 
 *  qax_eg_encap_access_type_to_phase
 *  arad_eg_encap_access_type_to_phase
 *  
 *  In QAX, there are additional two access phases to the EEDB. As a result, the mapping between EEDB
 *      application and access phase has changed. These arrays are mapping the application to
 *      the EEDB phase.
 *  
 *  Since this mapping is static, it is kept in a static variable.
 */

/* Mapping for qax. */
SOC_PPC_EG_ENCAP_ACCESS_PHASE qax_eg_encap_access_type_to_phase[SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_COUNT] = {
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_1,      */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_TWO,
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_2,      */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_THREE,
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_LL_ARP,        */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_FOUR,
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_AC,            */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_SIX,
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_NATIVE_LL_ARP, */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE,
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_NATIVE_AC,     */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_FIVE
};

/* Mapping for arad/jer. */
SOC_PPC_EG_ENCAP_ACCESS_PHASE arad_eg_encap_access_type_to_phase[SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_COUNT] = {
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_1,      */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE,      
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_2,      */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_TWO,
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_LL_ARP,        */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_THREE,
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_AC,            */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_FOUR,
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_NATIVE_LL_ARP, */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_COUNT,
    /* SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_NATIVE_AC,     */  SOC_PPC_EG_ENCAP_ACCESS_PHASE_COUNT
};



/* Validate Sync LIF */
#define _BCM_DPP_AM_SYNC_LIF_VALID_CHECK \
    do {                                                                \
        uint8 _init; \
        if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not valid\n" ), (unit))); \
        } \
        if (!SOC_DPP_PP_ENABLE(unit)) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not supported device for sync lif alloc\n" ), (unit))); \
        } \
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.init.get(unit, &_init); \
        BCMDNX_IF_ERR_EXIT(rv); \
        if (_init == FALSE) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_INIT,(_BSL_BCM_MSG("unit %d is not initialize sync lif alloc\n" ), (unit))); \
        } \
    } while (0);

STATIC uint32
_bcm_dpp_am_sync_lif_is_exist_bank(int unit, int bank_id, uint8 *is_exist)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.sync_lif_banks.bit_get(unit,
        bank_id*_BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK, is_exist);
    BCMDNX_IF_ERR_EXIT(rv);
    

exit:
    BCMDNX_FUNC_RETURN;
}

/* Check is sync bank, i.e. valid bank and used of type for SYNC */
STATIC uint32
_bcm_dpp_am_sync_lif_is_sync_bank(int unit, int bank_id, uint8 *is_sync)
{
    uint32 rv;
    uint32 type = 0;
    uint8 is_exist = 0;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_sync);


    *is_sync = FALSE;

    rv = _bcm_dpp_am_sync_lif_is_exist_bank(unit,bank_id,&is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.sync_lif_banks.bit_range_read(unit,
        (bank_id*_BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK+_BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MIN),
        0,
        _BCM_DPP_AM_SYNC_LIF_TYPE_NOF_BITS,
        &type);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_exist && type) {
      *is_sync = TRUE;
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_sync_lif_set_new_bank(int unit, int bank_id, uint8 is_sync_bank)
{
    uint32 type;
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;


    /* Valid */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.sync_lif_banks.bit_set(unit, 
        bank_id * _BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set Type */
    type = (is_sync_bank) ? 1:0;
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.sync_lif_banks.bit_range_write(unit,
        bank_id*_BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK+_BCM_DPP_AM_SYNC_LIF_TYPE_BITS_MIN,
        0,
        _BCM_DPP_AM_SYNC_LIF_TYPE_NOF_BITS,
        &type);    
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


STATIC uint32
_bcm_dpp_am_sync_lif_reset_bank(int unit, int bank_id)
{
    uint32 rv;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;


    /* Clear bits. */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.sync_lif_banks.bit_range_clear(unit,
        bank_id*_BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK,
        _BCM_DPP_AM_SYNC_LIF_NOF_BITS_PER_BANK);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Ingress LIF DB - Start 
 * ARAD only 
 */

/* 
 * Each ingress lif bank (4K entries) consists of 5 bits: first bit indication of existence
 * 4 other bits relate to VTT LIF lookup results:
 * VT-Lookup-Res-0
 * VT-Lookup-Res-1
 * TT-Lookup-Res-0
 * TT-Lookup-Res-1
 * We Define phase i in ingress LIF, in case application can be resulted in VTT-Lookup result i (out of 4)
 * We Define table i in ingress LIF, for [i,i+3] consecutive banks. I.e. LIF-Table 0 Banks 0-3, LIF-Table 1 Banks 4-7...
 * HW Limitation: Two Applications cant be allocated within the same table i in case one application is located
 * in phase 0(2) and the second application is located in phase 1(3).
 */
#define _BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK (5)
#define _BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN (1)
#define _BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MAX (4)
#define _BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS (_BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MAX-_BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN+1)
#define _BCM_DPP_AM_INGRESS_LIF_NOF_BITS (_BCM_DPP_AM_LIF_NOF_BANKS*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK)


/* Validate Ingress LIF */
#define _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK \
    do {                                                                \
        uint8 _init;\
        if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not valid\n" ), (unit))); \
        } \
        if (!SOC_DPP_PP_ENABLE(unit)) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not supported device for ingress lif alloc\n" ), (unit))); \
        } \
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.init.get(unit, &_init); \
        BCMDNX_IF_ERR_EXIT(rv); \
        if (_init == FALSE) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_INIT,(_BSL_BCM_MSG("unit %d is not initialize ingress lif alloc\n" ), (unit))); \
        } \
    } while (0);


/* 
 *  Information on allocation manager for ingress lif:
 *  Ingress LIF includes DB support for following application types:
 *  MPLS tunnel, PWE, IPV4 tunnel, Trill, In-AC, MIM, FCoE
 *  16 Banks where each bank with 4K entries
 *  Allocation manager store for each bank its own management.
 *  Following APIs tries to manage between banks and find corresponding bank for each application.
 *  ID returns from allocation manager is directly the In-LIF ID which access the database.
 */
/* Check is existed bank */
STATIC uint32
_bcm_dpp_am_ingress_lif_is_exist_bank(int unit, int bank_id, uint8 *is_exist)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_exist);

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_banks.bit_get(unit,
        bank_id*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK, is_exist);

    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Check is existed table */
STATIC uint32
_bcm_dpp_am_ingress_lif_is_exist_table(int unit, int table_id, uint8 *is_exist)
{
    int bank_id;
    int rv;
    uint8 is_exist_bank;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_exist);

    *is_exist = FALSE;
    for (bank_id = _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(table_id); 
          bank_id <= _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(table_id) ; ++bank_id) {
        
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist_bank);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if (is_exist_bank) {
            *is_exist = TRUE;
            break;
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/* Check if is valid table according to required phase numbers */
STATIC uint32
_bcm_dpp_am_ingress_lif_is_valid_table(int unit, int table_id, uint32 req_phase_numbers, uint8 *is_valid)
{
    uint32 used_phase_numbers = 0, tmp_phase_numbers = 0;
    int bank_id;
    int rv;
    uint8 is_exist;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_valid);


    /* Gather information regarding phase numbers for this required table */
    for (bank_id = _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(table_id); 
            bank_id <= _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(table_id) ; ++bank_id) {

        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            /* retreive information */
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_banks.bit_range_read(unit,
                bank_id*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK+_BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN,
                0,
                _BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS,
                &tmp_phase_numbers);
            BCMDNX_IF_ERR_EXIT(rv);
            used_phase_numbers |= tmp_phase_numbers;
        }
    }


    /* Decide according to information and given required phase numbers */
    *is_valid = TRUE;

    tmp_phase_numbers = used_phase_numbers;
    tmp_phase_numbers |= req_phase_numbers;
    /* Validate VT-Result-0 & 1 are not on */
    if ((SHR_BITGET(&tmp_phase_numbers,_dpp_am_ingress_lif_phase_vt_lookup_res_0)) && (SHR_BITGET(&tmp_phase_numbers,_dpp_am_ingress_lif_phase_vt_lookup_res_1))) {
        *is_valid = FALSE;
    }

    if ((SHR_BITGET(&tmp_phase_numbers,_dpp_am_ingress_lif_phase_tt_lookup_res_0)) && (SHR_BITGET(&tmp_phase_numbers,_dpp_am_ingress_lif_phase_tt_lookup_res_1))) {
        *is_valid = FALSE;
    }


exit:
  BCMDNX_FUNC_RETURN;
}


/* Retreive phase number. Returns error when bank doesnt exist */
STATIC uint32
_bcm_dpp_am_ingress_lif_bank_bitmap_phases_get(int unit, int bank_id, uint32 *bitmap_access_phase)
{ 
    uint8 is_exist;
    uint32 rv = BCM_E_NONE;
    uint32 bitmap_access_phase_lcl[1];

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(bitmap_access_phase);
    *bitmap_access_phase_lcl = 0;

    rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_exist == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bank id not exist")));
    }

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_banks.bit_range_read(unit,
            bank_id*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK+_BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN,
            0,
            _BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS,
            bitmap_access_phase_lcl);
    BCMDNX_IF_ERR_EXIT(rv);

    *bitmap_access_phase = *bitmap_access_phase_lcl;

exit:
    BCMDNX_FUNC_RETURN;
}


/* Allocate new bank according to requested phase numbers and flags */
STATIC uint32
_bcm_dpp_am_ingress_lif_allocate_new_bank(int unit, uint8 is_sync, uint32 flags, uint32 req_phase_numbers, int *bank_id, uint8 *is_success)
{
    uint32 rv;
    int new_bank = -1, tmp_bank_id, table_id = -1, tmp_table_id;
    uint8 is_valid, is_exist;
    uint8 ingress_lif_count;
    uint8 sync_bank_exists;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(bank_id);
    BCMDNX_NULL_CHECK(is_success);

    if (((req_phase_numbers >> _BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("phases %d are incorrect"), req_phase_numbers));
    }

    *is_success = TRUE;

    /* Find new bank */
    new_bank = -1;
    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        /* Allocate new given bank */
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, *bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bank %d existed even if already suppose to be not existed"), *bank_id));
        } 
        
        /* is valid table */
        table_id = _BCM_DPP_AM_INGRESS_LIF_BANK_ID_TO_TABLE_ID(*bank_id);
        rv = _bcm_dpp_am_ingress_lif_is_valid_table(unit, table_id, req_phase_numbers, &is_valid);
        BCMDNX_IF_ERR_EXIT(rv);
        
        if (!is_valid) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("bank %d not existed but does not match"
                                       " table id %d requirements"),
                              *bank_id,
                              table_id));
        }
        
        new_bank = *bank_id;                             
    } else {
        /* 
         * Find arbitray new bank, according to limitations
         * 1. Find match table i (prefer existed which support phase number and then one that is not allocated)
         * 2. Get arbitray bank from table i
         */ 
        /* Find match table */   
        new_bank = -1;    
        for (tmp_table_id = 0; tmp_table_id < _BCM_DPP_AM_INGRESS_LIF_NOF_TABLES; ++tmp_table_id) {
            rv = _bcm_dpp_am_ingress_lif_is_exist_table(unit, tmp_table_id, &is_exist);
            BCMDNX_IF_ERR_EXIT(rv);
            
            if (is_exist) {
                /* match table */
                rv = _bcm_dpp_am_ingress_lif_is_valid_table(unit, tmp_table_id, req_phase_numbers, &is_valid);
                BCMDNX_IF_ERR_EXIT(rv);
                
                if (is_valid) {
                    for (tmp_bank_id = _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(tmp_table_id); 
                          tmp_bank_id <= _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(tmp_table_id) ; ++tmp_bank_id) {
                        
                        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, tmp_bank_id, &is_exist);
                        BCMDNX_IF_ERR_EXIT(rv);
                        
                        if (!is_exist) {
                            /* In case of sync, have one more constrain bank is not used by any other application at egress */
                            if (is_sync) {
                                rv = _bcm_dpp_am_sync_lif_is_exist_bank(unit, tmp_bank_id, &is_exist);
                                BCMDNX_IF_ERR_EXIT(rv);
                                if (is_exist) {
                                  continue;
                                }
                            }
                            table_id = tmp_table_id;
                            new_bank = tmp_bank_id;
                            break;
                        }
                    }
                    
                    if (new_bank != -1) {
                        /* Found new bank */
                        break;
                    }
                }
            }
        }
                
        if (new_bank == -1) {
            /* In case no valid bank, find from unexist table */
            for (tmp_table_id = 0; tmp_table_id < _BCM_DPP_AM_INGRESS_LIF_NOF_TABLES; ++tmp_table_id) {
                rv = _bcm_dpp_am_ingress_lif_is_exist_table(unit, tmp_table_id, &is_exist);
                BCMDNX_IF_ERR_EXIT(rv);
                
                if (!is_exist) {
                    table_id = tmp_table_id;

                    for (tmp_bank_id = _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MIN_BANK_ID(tmp_table_id);
                         tmp_bank_id <= _BCM_DPP_AM_INGRESS_LIF_TABLE_ID_TO_MAX_BANK_ID(tmp_table_id); ++tmp_bank_id) {

                        /* In case of sync, have one more constrain bank is not used by any other application at egress */
                        if (is_sync) {
                            rv = _bcm_dpp_am_sync_lif_is_exist_bank(unit, tmp_bank_id, &is_exist);
                            BCMDNX_IF_ERR_EXIT(rv);
                            if (is_exist) {
                                continue;
                            }
                        }

                        new_bank = tmp_bank_id;
                        break;
                    }

                    if (new_bank != -1) {
                        /* Found new bank */
                        break;
                    }
                }
            }
        }

        if (table_id == -1) {
            /* Unavaiable table */
            /* No bank is avaiable with the following required phases */
            *is_success = FALSE;
            BCM_EXIT;
        }

        /* Self check */
        if (new_bank != -1) {
            rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, new_bank, &is_exist);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_exist) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("found unexist table id %d, but first bank already allocated"), table_id));
            }
        }


    }


    if (new_bank == -1) { /* No new bank avaiable */
        *is_success = FALSE;
        BCM_EXIT;
    }

#if _DPP_AM_LIF_ALLOCATION_DEBUG_PRINT
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            new_bank, req_phase_numbers)));
#endif

    /* Set SW DB bank information */
    /* Valid */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_banks.bit_set(unit,
                                        new_bank * _BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Required Phase number */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_banks.bit_range_write(unit,
            new_bank * _BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK + _BCM_DPP_AM_INGRESS_LIF_PHASE_BITS_MIN,
            0,
            _BCM_DPP_AM_INGRESS_LIF_PHASE_NOF_BITS,
            &req_phase_numbers);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Sync bank status */
    rv =_bcm_dpp_am_sync_lif_is_exist_bank(unit, new_bank, &sync_bank_exists);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!sync_bank_exists) {
        rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, new_bank, is_sync);
        BCMDNX_IF_ERR_EXIT(rv);
    }


    /* Count */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_count.get(unit, &ingress_lif_count);
    BCMDNX_IF_ERR_EXIT(rv);
    ingress_lif_count++;
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_count.set(unit, ingress_lif_count);
    BCMDNX_IF_ERR_EXIT(rv);

    *bank_id = new_bank;

    /* 
     * Special case: Bank ID is 0, many application do not use InLIF 0. Allocate it so it won't be used 
     * We wont skip the allocation in case global lif index is 0. In this case, we can take advantage of that lif. 
     */
    if (new_bank == 0 && (SOC_DPP_CONFIG(unit)->pp.global_lif_index_simple != 0 && SOC_DPP_CONFIG(unit)->pp.global_lif_index_drop != 0)) {
        int object = 0;
        rv = dpp_am_res_alloc(unit, new_bank, dpp_am_res_obs_inlif, SW_STATE_RES_ALLOC_WITH_ID, 1, &object);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Convert allocation manager pool id to access phase number */
STATIC uint32
_bcm_dpp_am_ingress_lif_alloc_mngr_pool_id_to_bitmap_access_phase(int unit, bcm_dpp_am_ingress_lif_alloc_info_t *allocation_info, uint32 *bitmap_phase_number) {
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(allocation_info);

    *bitmap_phase_number = 0;

    switch (allocation_info->pool_id) {
    case dpp_am_res_lif_pwe: /* PWE termination */
        switch (allocation_info->application_type) {
        case bcm_dpp_am_ingress_lif_app_mpls_term:
            /* In case of Unindexed supported up to two terminations */
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            break;
            /* MPLS Indexed Support */
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1:
            if (SOC_DPP_CONFIG(unit)->pp.tunnel_termination_in_tt_only) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);  /* TT only lookups, tt lookup res 0 is MPLS_1 */
            } else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
            	_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            } else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
            	_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
            } else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_2:
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
        	    _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
        	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            } else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_3:
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
           	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
        	    _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
        	} else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_2:
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
        	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_0);
           	} else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
           	}
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_3:
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d and types %d"), allocation_info->pool_id, allocation_info->application_type));
        }
        break;
    case dpp_am_res_lif_ip_tnl: /* IP tunnel termination */
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
        break;
    case dpp_am_res_lif_dynamic: /* Main applications using Incoming LIF */
        switch (allocation_info->application_type) {
        case bcm_dpp_am_ingress_lif_app_ingress_ac:
        case bcm_dpp_am_ingress_lif_app_vpn_ac:
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21 || SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
   	        } else if (SOC_DPP_CONFIG(unit)->trill.mode) {
   	        	/* In case trill is enabled, make sure the ingress nick lookup does conflict with VD-vlan-vlan lookup */
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            } else {
            	_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_0);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_ingress_isid:
        case bcm_dpp_am_ingress_lif_app_ip_term:
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
            break;
        case bcm_dpp_am_ingress_lif_app_mpls_term:
            /* In case of Unindexed supported up to two terminations */
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            break;
            /* FRR */
        case bcm_dpp_am_ingress_lif_app_mpls_frr_term:
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
        		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
         	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
         		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
         		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
         	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
        		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_0);
         	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
         		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_0);
         		_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
        	} else {
        	    _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
        	}
            break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_explicit_null:
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
            break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_mldp:
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            break;
            /* MPLS Indexed Support */
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1:
        	if (SOC_DPP_CONFIG(unit)->pp.tunnel_termination_in_tt_only) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);  /* TT only lookups, tt lookup res 0 is MPLS_1 */
            } else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
            	_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            } else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
            	_BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
            } else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_2:
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 22) {
        	    _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
        	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 23) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            } else {
               _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_3:
            if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
               _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
           	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
        	   _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
        	} else {
               _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            }
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_2:
        	if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 20) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
        	} else if (SOC_DPP_CONFIG(unit)->pp.mpls_termination_database_mode == 21) {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_0);
           	} else {
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
                _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
           	}
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_3:
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);
            break;
        case bcm_dpp_am_ingress_lif_app_trill_nick:
            _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_1);
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d and types %d"), allocation_info->pool_id, allocation_info->application_type));
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d in the ingress lif database"), allocation_info->pool_id));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Returns all existed banks with the corresponding access phases. Always returns with increasing order */
STATIC uint32
_bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(int unit, uint8 is_sync, uint32 bitmap_phase_access, int *nof_valid_banks, int *banks_ids) {
    uint32 bank_id, bank_bimap_phase_access, rv;
    uint8 is_exist, is_sync_bank;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(nof_valid_banks);
    BCMDNX_NULL_CHECK(banks_ids);

    *nof_valid_banks = 0;
    for (bank_id = 0; bank_id < _BCM_DPP_AM_LIF_NOF_BANKS; ++bank_id) {
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            /* SYNC banks are low priority to handle */
            rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_sync != is_sync_bank) {
                continue;
            }

            /* Validate phase number */
            rv = _bcm_dpp_am_ingress_lif_bank_bitmap_phases_get(unit, bank_id, &bank_bimap_phase_access);
            BCMDNX_IF_ERR_EXIT(rv);

            if ((bitmap_phase_access & bank_bimap_phase_access) == bitmap_phase_access) {
                /* We have a match add it to the valid banks */
                banks_ids[(*nof_valid_banks)] = bank_id;
                (*nof_valid_banks)++;
            }
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Initalize ingress lif allocation */
uint32
_bcm_dpp_am_ingress_lif_init(int unit) {
    bcm_error_t rv;
    uint8       init;

    BCMDNX_INIT_FUNC_DEFS;

    /* 
     * If it is warm boot then the ingress lif info was initialized
     */
    if (SOC_WARM_BOOT(unit)) {
        BCM_EXIT;
    }

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.init.get(unit, &init);
    BCMDNX_IF_ERR_EXIT(rv);
    if (init) {
        /* Nothing to do ingress lif already initialized */
        BCM_EXIT;
    }

    /*
     * Allcoate ingress lif bitmap
     */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_count.set(unit, 0);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_banks.alloc_bitmap(unit, _BCM_DPP_AM_INGRESS_LIF_NOF_BITS);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Done initalize */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.init.set(unit, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* De-Initalize ingress lif allocation */
uint32
_bcm_dpp_am_ingress_lif_deinit(int unit) {
    bcm_error_t rv;
    uint8       init;

    BCMDNX_INIT_FUNC_DEFS;

    /* 
     * If it is warm boot then the ingress lif info was initialized
     */
    if (SOC_WARM_BOOT(unit)) {
        BCM_EXIT;
    }

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.init.get(unit, &init);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!init) {
        /* Nothing to do ingress lif already de-initialized */
        BCM_EXIT;
    }

    /* Done De-initalize */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.init.set(unit, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_ingress_lif_alloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t *allocation_info, uint32 flags, int count, int *object) {
    uint32 rv;
    int bank_id, object_id;
    uint8 is_sync_bank;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use _bcm_dpp_gport_alloc_global_and_local_lif.")));
    }

    /* Input validation, make sure lif is within valid range. */
    if (flags & (SW_STATE_RES_ALLOC_WITH_ID)) {
        if (*object >= SOC_DPP_DEFS_GET(unit, nof_local_lifs)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given outlif value is too high.")));
        }
    }

    /* Allocate object */
    rv = _bcm_dpp_am_ingress_lif_internal_alloc(unit, allocation_info, FALSE, flags, count, object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    object_id = *object;
    bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to allocate also egress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info, 0, sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_EGRESS; /* Sync egress */
        sync_info.ingress_lif.pool_id = allocation_info->pool_id;
        sync_info.ingress_lif.application_type = allocation_info->application_type;

        /* Always WITH_ID since we synchrnoize the other side */
        flags |= SW_STATE_RES_ALLOC_WITH_ID;
        rv = _bcm_dpp_am_sync_lif_alloc(unit, &sync_info, flags, count, object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Allocate entry according to pool id */
STATIC uint32
_bcm_dpp_am_ingress_lif_internal_alloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t *allocation_info, uint8 is_sync, uint32 flags, int count, int *object) {
    uint32 rv;
    uint32 bitmap_access_phase;
    int *banks_ids = NULL;
    int nof_valid_banks, bank_id, new_bank;
    uint32 alloc_size;
    uint8 is_exist, is_success;
    uint8 ingress_lif_count;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    rv = _bcm_dpp_am_ingress_lif_alloc_mngr_pool_id_to_bitmap_access_phase(unit, allocation_info, &bitmap_access_phase);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_count.get(unit, &ingress_lif_count);
    BCMDNX_IF_ERR_EXIT(rv);
    alloc_size = sizeof(int) * ingress_lif_count;
    BCMDNX_ALLOC(banks_ids, alloc_size, "allocate bank ids");
    if (NULL == banks_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation bank ids")));
    }
    sal_memset(banks_ids, 0, alloc_size);

    /* Try to allocate for each bank id unless we are dealing with case of WITH_ID */
    if (flags & (SW_STATE_RES_ALLOC_WITH_ID)) {
        int object_id;
        uint32 bank_bitmap_phase_access;

        object_id = *object;
        /* Find matched bank */
        bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
        /* Validate bank exist and with the correct phase */
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist == 0) {
            uint32 alloc_bank_flags;

            alloc_bank_flags = SW_STATE_RES_ALLOC_WITH_ID;
            /* Allocate new bank since it is not existed */
            rv = _bcm_dpp_am_ingress_lif_allocate_new_bank(unit, is_sync, alloc_bank_flags, bitmap_access_phase, &bank_id, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_success) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("ingress lif resources didnt find new bank avaiable")));
            }
        }

        /* Validate phase number */
        rv = _bcm_dpp_am_ingress_lif_bank_bitmap_phases_get(unit, bank_id, &bank_bitmap_phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((bitmap_access_phase & bank_bitmap_phase_access) != bitmap_access_phase) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                (_BSL_BCM_MSG("given invalid object id %d, bank phase access %d, not matched to phase access %d"), object_id, bank_bitmap_phase_access, bitmap_access_phase));
        }

        rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_obs_inlif, flags, count, object);
        BCMDNX_IF_ERR_EXIT(rv);
    } else {
        uint8 alloc_success;

        alloc_success = FALSE;

        rv = _bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(unit, is_sync, bitmap_access_phase, &nof_valid_banks, banks_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
            rv = dpp_am_res_silent_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_inlif, flags, count, object);
            if (rv == BCM_E_NONE) {
                alloc_success = TRUE;
                /* Allocation succeded no need to find other banks - save to WB and continue */
                break;
            }
        }

        if (!alloc_success) {
            /* Allocate new bank since no success */
            rv = _bcm_dpp_am_ingress_lif_allocate_new_bank(unit, is_sync, 0, bitmap_access_phase, &new_bank, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_success) {
                /* Allocate object */
                rv = dpp_am_res_alloc(unit, new_bank, dpp_am_res_obs_inlif, flags, count, object);
                BCMDNX_IF_ERR_EXIT(rv);
                alloc_success = TRUE;
            } else {
                /* Retreive from sync banks as last option (no more bank to allocate) */
                rv = _bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(unit, TRUE, bitmap_access_phase, &nof_valid_banks, banks_ids);
                BCMDNX_IF_ERR_EXIT(rv);

                for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
                    rv = dpp_am_res_silent_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_inlif, flags, count, object);
                    if (rv == BCM_E_NONE) {
                        alloc_success = TRUE;
                        /* Allocation succeded no need to find other banks */
                        break;
                    }
                }
            }
        }

        if (!alloc_success) {
            /* FAIL all full :( */
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("ingress lif resources didnt find new bank avaiable")));
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(banks_ids);
    BCMDNX_FUNC_RETURN;

}

STATIC uint32
_bcm_dpp_am_ingress_lif_internal_alloc_align(int unit, bcm_dpp_am_ingress_lif_alloc_info_t *allocation_info, uint8 is_sync, uint32 flags, int align, int offset, int count, int *object) {
    uint32 rv;
    uint32 bitmap_phase_access;
    int *banks_ids = NULL;
    int nof_valid_banks, bank_id, new_bank;
    uint32 alloc_size;
    uint8 is_exist, is_success;
    uint8 ingress_lif_count;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    rv = _bcm_dpp_am_ingress_lif_alloc_mngr_pool_id_to_bitmap_access_phase(unit, allocation_info, &bitmap_phase_access);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_count.get(unit, &ingress_lif_count);
    BCMDNX_IF_ERR_EXIT(rv);
    alloc_size = sizeof(int) * ingress_lif_count;
    BCMDNX_ALLOC(banks_ids, alloc_size, "allocate bank ids");
    if (NULL == banks_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation bank ids")));
    }
    sal_memset(banks_ids, 0, alloc_size);

    /* Try to allocate on each bank id unless we are dealing with case of WITH_ID */
    if (flags & (SW_STATE_RES_ALLOC_WITH_ID)) {
        int object_id;
        uint32 bank_bitmap_phase_access;

        object_id = *object;
        /* Find matched bank */
        bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
        /* Validate bank exist and with the correct phase */
        rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist == 0) {
            uint32 alloc_bank_flags;

            alloc_bank_flags = SW_STATE_RES_ALLOC_WITH_ID;
            /* Allocate new bank since it is not existed */
            rv = _bcm_dpp_am_ingress_lif_allocate_new_bank(unit, is_sync, alloc_bank_flags, bitmap_phase_access, &bank_id, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_success) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("ingress lif resources didnt find new bank avaiable")));
            }
        }

        /* Validate phase number */
        rv = _bcm_dpp_am_ingress_lif_bank_bitmap_phases_get(unit, bank_id, &bank_bitmap_phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((bitmap_phase_access & bank_bitmap_phase_access) != bitmap_phase_access) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("given invalid object id %d, bank phase access %d, not matched to phase access %d"), object_id, bank_bitmap_phase_access, bitmap_phase_access));
        }

        /* if replace flag is set and exist then we are done */
        if(flags & BCM_DPP_AM_FLAG_ALLOC_REPLACE) {
            rv = dpp_am_res_check(unit, bank_id, dpp_am_res_obs_inlif, offset, count);
            if(rv == BCM_E_EXISTS) {
                rv = BCM_E_NONE;
                BCM_EXIT;
            }
        }
        rv = dpp_am_res_alloc_align(unit, bank_id, dpp_am_res_obs_inlif, flags, align, offset, count, object);
        BCMDNX_IF_ERR_EXIT(rv);

    } else {
        uint8 alloc_success;

        if (flags & BCM_DPP_AM_FLAG_ALLOC_REPLACE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("cannot set replace when ID is not given")));
        }

        rv = _bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(unit, is_sync, bitmap_phase_access, &nof_valid_banks, banks_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        alloc_success = FALSE;
        for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
            rv = dpp_am_res_silent_alloc_align(unit, banks_ids[bank_id], dpp_am_res_obs_inlif, flags, align, offset, count, object);
            if (rv == BCM_E_NONE) {
                alloc_success = TRUE;

                /* Allocation succeded no need to find other banks */
                break;
            }
        }

        if (!alloc_success) {
            /* Allocate new bank since no success */
            rv = _bcm_dpp_am_ingress_lif_allocate_new_bank(unit, is_sync, 0, bitmap_phase_access, &new_bank, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_success) {
                /* Allocate object */
                rv = dpp_am_res_alloc_align(unit, new_bank, dpp_am_res_obs_inlif, flags, align, offset, count, object);
                BCMDNX_IF_ERR_EXIT(rv);

                alloc_success = TRUE;

            } else {
                /* Retreive from sync banks as last option (no more bank to allocate) */
                rv = _bcm_dpp_am_ingress_lif_banks_by_bitmap_access_phase(unit, TRUE, bitmap_phase_access, &nof_valid_banks, banks_ids);
                BCMDNX_IF_ERR_EXIT(rv);

                for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
                    rv = dpp_am_res_silent_alloc_align(unit, banks_ids[bank_id], dpp_am_res_obs_inlif, flags, align, offset, count, object);
                    if (rv == BCM_E_NONE) {
                        alloc_success = TRUE;
                        /* Allocation succeded no need to find other banks */
                        break;
                    }
                }
            }
        }

        if (!alloc_success) {
            /* FAIL all full :( */
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("ingress lif resources didnt find new bank avaiable")));
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(banks_ids);
    BCMDNX_FUNC_RETURN;
}

/* Deallocate entry according to object id */
uint32
_bcm_dpp_am_ingress_lif_dealloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    int bank_id;
    uint8 is_sync_bank;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use _bcm_dpp_gport_delete_global_and_local_lif.")));
    }

    /* Deallocate object */
    rv = _bcm_dpp_am_ingress_lif_internal_dealloc(unit,allocation_info, FALSE, count, object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit,bank_id,&is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to deallocate also egress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info,0,sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_EGRESS; /* Sync egress */
        sync_info.ingress_lif.pool_id = allocation_info->pool_id;
        sync_info.ingress_lif.application_type = allocation_info->application_type;

        rv = _bcm_dpp_am_sync_lif_dealloc(unit, &sync_info, count,object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_ingress_lif_internal_dealloc(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, uint8 is_sync, int count, int object)
{
    uint32 rv;
    uint8 is_exist;
    int bank_id;
    uint32 flags = 0;
    uint8 ingress_lif_count;
    int used_count;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    

    /* Find matched bank */
    bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object);
    /* Validate bank exist and with the correct phase */
    rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_exist) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("given invalid object id %d"), object));
    }
    if (!SOC_IS_DETACHING(unit))
    {
        rv = dpp_am_res_free_and_status(unit, bank_id, dpp_am_res_obs_inlif, count, object, &flags);
        BCMDNX_IF_ERR_EXIT(rv);
    }
    /* If it's the last element in this bank, clear the "bank is allocated" and bank phase indication. */
    if (flags & (SW_STATE_RES_FREED_TYPE_LAST_ELEM | SW_STATE_RES_FREED_POOL_LAST_ELEM)) {
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_banks.bit_range_clear(unit,
            bank_id*_BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK, _BCM_DPP_AM_INGRESS_LIF_NOF_BITS_PER_BANK);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_count.get(unit, &ingress_lif_count);
        BCMDNX_IF_ERR_EXIT(rv);
        ingress_lif_count--;
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr.ingress_lif.ingress_lif_count.set(unit, ingress_lif_count);
        BCMDNX_IF_ERR_EXIT(rv);

        /* Check if egress bank is empty and sync bank status needs to be changed. */
        rv = dpp_am_res_used_count_get(unit, bank_id, dpp_am_res_obs_eg_encap, &used_count);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!used_count) {
            rv = _bcm_dpp_am_sync_lif_reset_bank(unit, bank_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } 
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* Is exist entry according to pool id */
int
_bcm_dpp_am_ingress_lif_is_allocated(int unit, bcm_dpp_am_ingress_lif_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    int bank_id;
    uint8 bank_is_exist;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use bcm_dpp_am_global_lif_is_alloced.")));
    }

    _BCM_DPP_AM_INGRESS_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    /* Find matched bank */
    bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object);
    /* Validate bank exist and with the correct phase */
    rv = _bcm_dpp_am_ingress_lif_is_exist_bank(unit, bank_id, &bank_is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!bank_is_exist) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, bank_id, dpp_am_res_obs_inlif, count, object));
    
exit:
    BCMDNX_FUNC_RETURN;
}
/* 
 * Ingress LIF DB - End 
 * ARAD only 
 */
/* 
 * Egress encapsulation DB - Start 
 * ARAD only 
 */

/* Each egress encapsulation consists of 3 bits: first bit indication of exists 2 other bits of phase number (0-phase-1) */
#define _BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK (3)
#define _BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MIN (1)
#define _BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MAX (2)
#define _BCM_DPP_AM_EGRESS_ENCAP_PHASE_NOF_BITS (_BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MAX-_BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MIN+1)
#define _BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS (16*_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK)

/* Validate egress encap */
#define _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK \
    do {                                                                \
        uint8 _init; \
        if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not valid\n" ), (unit))); \
        } \
        if (!SOC_DPP_PP_ENABLE(unit)) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not supported device for egress encap alloc\n" ), (unit))); \
        } \
        rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.init.get(unit, &_init); \
        BCMDNX_IF_ERR_EXIT(rv); \
        if (_init == FALSE) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_INIT,(_BSL_BCM_MSG("unit %d is not initialize egress encap alloc\n" ), (unit))); \
        } \
    } while (0);


/* 
 *  Information on allocation manager for egress encapsulation:
 *  Egress encapsulation includes DB support for following application types:
 *  MPLS tunnel, PWE, IPV4 tunnel, Trill, Out-AC, Out-RIF, LL 
 *  16 Banks where each bank with 2K/4K entries
 *  Allocation manager store for each bank its own management.
 *  Following APIs tries to manage between banks and find corresponding bank for each application.
 *  ID returns from allocation manager is directly the Out-LIF ID which access the database.
 */

/* Check is existed bank */
STATIC uint32
_bcm_dpp_am_egress_encap_is_exist_bank(int unit, int bank_id, uint8 *is_exist)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_exist);

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_banks.bit_get(unit,
        bank_id*_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK, is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Retreive phase number. Returns error when bank doesnt exist */
STATIC uint32
_bcm_dpp_am_egress_encap_bank_phase_get(int unit, int bank_id, SOC_PPC_EG_ENCAP_ACCESS_PHASE *access_phase)
{ 
    uint8 is_exist;
    uint32 rv = BCM_E_NONE;
    uint32 phase_number = 0;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(access_phase);

    rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_exist == 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("bank id not exist")));
    }

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_banks.bit_range_read(unit,
            bank_id*_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK+_BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MIN,
            0,
            _BCM_DPP_AM_EGRESS_ENCAP_PHASE_NOF_BITS,
            &phase_number);
    BCMDNX_IF_ERR_EXIT(rv);

    if ((phase_number >= SOC_PPC_NOF_EG_ENCAP_ACCESS_PHASE_TYPES(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("phase number for bank id %d is incorrect, value got %d"), bank_id, phase_number));
    }

    /* 1:1 mapping between phase_number and enum types */
    *access_phase = phase_number;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Allocate new bank according to requested phase number and flags */
STATIC uint32
_bcm_dpp_am_egress_encap_allocate_new_bank(int unit, uint8 is_sync, uint32 flags, SOC_PPC_EG_ENCAP_ACCESS_PHASE phase_access, int *bank_id, uint8 *is_success)
{
    uint32 rv;
    int new_bank, tmp_bank_id;
    uint8 is_exist;
    uint32 phase_number = phase_access;
    SOC_PPC_EG_ENCAP_RANGE_INFO encap_range_info;
    uint32 soc_sand_dev_id, soc_sand_rv;
    uint8 egress_encap_count;
    uint8 sync_bank_exists;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(bank_id);
    BCMDNX_NULL_CHECK(is_success);

    *is_success = TRUE;

    soc_sand_dev_id = (unit);

    if ((phase_number >= SOC_PPC_NOF_EG_ENCAP_ACCESS_PHASE_TYPES(unit))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("phase number %d is incorrect"), phase_number));
    }

    /* Find new bank */
    new_bank = -1;
    if (flags & SW_STATE_RES_ALLOC_WITH_ID) {
        /* Allocate new given bank */
        rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, *bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("bank %d existed even if already suppose to be not existed"), *bank_id));
        } 
        new_bank = *bank_id;                             
    } else {
        /* Find arbitray new bank */        
        for (tmp_bank_id = 0; tmp_bank_id < SOC_PPC_EG_ENCAP_NOF_BANKS(unit); ++tmp_bank_id) {
            rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, tmp_bank_id, &is_exist);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_exist == 0) {
                if (is_sync) {
                    rv = _bcm_dpp_am_sync_lif_is_exist_bank(unit, tmp_bank_id, &is_exist);
                    BCMDNX_IF_ERR_EXIT(rv);
                    if (is_exist) {
                      continue;
                    }
                }
                /* New bank found. */
                new_bank = tmp_bank_id;
                break;
            }
        }
    }

    if (new_bank == -1) { /* No new bank avaiable */
        *is_success = FALSE;
        BCM_EXIT;
    }

#if _DPP_AM_LIF_ALLOCATION_DEBUG_PRINT
    LOG_VERBOSE(BSL_LS_BCM_COMMON,
                (BSL_META_U(unit,
                            new_bank, phase_access)));
#endif 


    /* Set SW DB bank information */
    /* Valid */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_banks.bit_set(unit, new_bank * _BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Phase number */
    phase_number = phase_access;
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_banks.bit_range_write(unit,
        new_bank*_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK+_BCM_DPP_AM_EGRESS_ENCAP_PHASE_BITS_MIN,
        0,
        _BCM_DPP_AM_EGRESS_ENCAP_PHASE_NOF_BITS,
        &phase_number);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Sync bank status */
    rv =_bcm_dpp_am_sync_lif_is_exist_bank(unit, new_bank, &sync_bank_exists);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!sync_bank_exists) {
        rv = _bcm_dpp_am_sync_lif_set_new_bank(unit, new_bank, is_sync);
        BCMDNX_IF_ERR_EXIT(rv);
    }


    /* Count */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_count.get(unit, &egress_encap_count);
    BCMDNX_IF_ERR_EXIT(rv);
    ++egress_encap_count;
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_count.set(unit, egress_encap_count);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Set HW */
    soc_sand_rv = soc_ppd_eg_encap_range_info_get(soc_sand_dev_id, &encap_range_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    encap_range_info.bank_access_phase[new_bank] = phase_access;

    soc_sand_rv = soc_ppd_eg_encap_range_info_set(soc_sand_dev_id, &encap_range_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    *bank_id = new_bank;

    /* Special case: Bank ID is 0, many application do not use OutLIF 0. Allocate it so it won't be used */
    if (new_bank == 0) {
        SOC_PPC_EG_ENCAP_DATA_INFO data_info;

        int object = 0;        
        rv = dpp_am_res_alloc(unit, new_bank, dpp_am_res_obs_eg_encap, SW_STATE_RES_ALLOC_WITH_ID, 1, &object);
        BCMDNX_IF_ERR_EXIT(rv);

        /* 
         * ARAD-A0: outlif 0 must be DATA entry 
         */ 
        if (SOC_IS_ARAD_A0(unit)) {
          /* Null Data entry */
          SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
          
          soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id, object, &data_info, FALSE, 0);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Convert allocation manager pool id to access phase number */
uint32
_bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, SOC_PPC_EG_ENCAP_ACCESS_PHASE *phase_number)
{    
    SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE phase_type;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(allocation_info);

    switch (allocation_info->pool_id) {
        case dpp_am_res_eep_global: /* Link layer */
          phase_type = (allocation_info->application_type == bcm_dpp_am_egress_encap_app_native_arp) ? SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_NATIVE_LL_ARP :
              (allocation_info->application_type == bcm_dpp_am_egress_encap_app_linker_layer) ? SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_2:SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_LL_ARP;
          break;
        case dpp_am_res_eep_ip_tnl: /* IP tunnel */
          phase_type = (allocation_info->application_type == bcm_dpp_am_egress_encap_app_ip_tunnel_roo || 
		  				allocation_info->application_type == bcm_dpp_am_egress_encap_app_l2_encap || 
		  				allocation_info->application_type == bcm_dpp_am_egress_encap_app_ip_tunnel_l2tp) ? SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_1:SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_2;
          break;
        case dpp_am_res_eep_mpls_tunnel: /* PWE or MPLS tunnel simple or MPLS tunnel second */
          if(allocation_info->application_type  ==  bcm_dpp_am_egress_encap_app_third_mpls_tunnel){
               /*4 level mpls encap, 1st mpls encap uses stage 2*/    
               phase_type = SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_NATIVE_LL_ARP;
            } else {
               phase_type = (allocation_info->application_type == bcm_dpp_am_egress_encap_app_mpls_tunnel) ? SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_2:SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_1;      
            }
          break;
        case dpp_am_res_eg_out_ac:
        case dpp_am_res_lif_dynamic:
          /* PON 3 TAGS DATA Manipulation or out ac ttl decrements in Jericho/QMX */
          if ((allocation_info->application_type == bcm_dpp_am_egress_encap_app_3_tags_data)
               || (allocation_info->application_type == bcm_dpp_am_egress_encap_app_phase_3_out_ac)) {
              phase_type = SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_LL_ARP;
          } else if (allocation_info->application_type == bcm_dpp_am_egress_encap_app_native_ac) { /* Native ac. */
              phase_type = SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_NATIVE_AC;
          } else { /* Normal out_ac */
              phase_type = SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_AC;
          }
          break;
        case dpp_am_res_eg_out_rif: /* Out-RIF */
		case dpp_am_res_eg_data_l2tp: /* L2TP data */
          phase_type = SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_2;
          break;
        case dpp_am_res_eg_data_erspan: /* ERSPAN */
        case dpp_am_res_ipv6_tunnel: /* IPv6 tunnel using data entry */
        case dpp_am_res_eg_data_trill_invalid_entry: /* ARAD A0: Need an invalid entry for trill  */
          phase_type = SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_1;
          break;
        case dpp_am_res_eep_trill: /* Trill */
            phase_type = (allocation_info->application_type == bcm_dpp_am_egress_encap_app_trill_roo) ? SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_1:SOC_PPC_EG_ENCAP_ACCESS_PHASE_TYPE_TUNNEL_2; 
            break; 
        default:
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d in the egress encapsulation database"), allocation_info->pool_id));
    }

    if (SOC_IS_JERICHO_PLUS(unit)) {
        *phase_number = qax_eg_encap_access_type_to_phase[phase_type];
    } else if (SOC_IS_JERICHO_AND_BELOW(unit)) {
        *phase_number = arad_eg_encap_access_type_to_phase[phase_type];
    }

    if (*phase_number == SOC_PPC_EG_ENCAP_ACCESS_PHASE_COUNT) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Application not supported on this device.")));
    }


    if ((SOC_DPP_CONFIG(unit)->pp.pon_application_enable) &&
        (allocation_info->application_type != bcm_dpp_am_egress_encap_app_3_tags_data) &&
        (allocation_info->application_type != bcm_dpp_am_egress_encap_app_3_tags_out_ac) &&
        SOC_IS_ARAD_B1_AND_BELOW(unit)) {
        if (SOC_DPP_CONFIG(unit)->trill.mode){
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Trill is Not supported, in case of PON application enable")));
        }

        /* 
         * application implemented in Phase One is not supported with PON 3 TAGs Manipulation,
         * such as, ERSPAN, ip tunnel and trill.
         * Since SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE is 0, (phase = phase -1) is used for checking.
         */
        *phase_number -= 1;
        if (*phase_number < 0) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported pool id %d in the egress encapsulation database, in case of PON application enable"), allocation_info->pool_id));
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Returns all existed banks with the corresponding access phase. Always returns with increasing order */
STATIC uint32
_bcm_dpp_am_egress_encap_banks_by_access_phase(int unit, uint8 is_sync, SOC_PPC_EG_ENCAP_ACCESS_PHASE phase_access, int* nof_valid_banks, int* banks_ids)
{
    uint32 bank_id, phase_number, bank_phase_number, rv;
    uint8 is_exist, is_sync_bank;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(nof_valid_banks);
    BCMDNX_NULL_CHECK(banks_ids);

    /* 1:1 mapping between phase_number and phase_access */
    phase_number = phase_access;

    *nof_valid_banks = 0;
    for (bank_id = 0; bank_id < SOC_PPC_EG_ENCAP_NOF_BANKS(unit); ++bank_id) {
        rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist) {
            rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_sync != is_sync_bank) {
                continue;
            }
           
            /* Validate phase number */
            rv = _bcm_dpp_am_egress_encap_bank_phase_get(unit, bank_id, &bank_phase_number);
            BCMDNX_IF_ERR_EXIT(rv);

            if (phase_number == bank_phase_number) {
                /* We have a matach add it to the valid banks */
                banks_ids[(*nof_valid_banks)] = bank_id;
                (*nof_valid_banks)++;
            }
        }
    } 

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_egress_encap_sw_state_init(int unit)
{
    bcm_error_t rv;
    uint8       init;

    BCMDNX_INIT_FUNC_DEFS;

    /* 
     * If it is warm boot then the egress encap info was initialized
     */
    if (SOC_WARM_BOOT(unit))
    {
        BCM_EXIT;
    }

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.init.get(unit, &init);
    BCMDNX_IF_ERR_EXIT(rv);
    if (init) {
        /* Nothing to do egress encap already initialized */
        BCM_EXIT;
    }

    /*
     * Allcoate egress encap bitmap
     */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_count.set(unit, 0);
    BCMDNX_IF_ERR_EXIT(rv);
    
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_banks.alloc_bitmap(unit, _BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Done initalize */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.init.set(unit, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/* Initalize egress encapsulation allocation */
uint32
_bcm_dpp_am_egress_encap_init(int unit)
{
    bcm_dpp_am_egress_encap_alloc_info_t allocation_info;
    uint32 rv;
    int elem;
    int bank_id = 0;
    int i = 0;
    uint8 is_sync = FALSE;
    SOC_PPC_EG_ENCAP_ACCESS_PHASE bank_phase = 0;
    uint8 is_success = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit))
    {
        sal_memset(&allocation_info,0,sizeof(allocation_info));
        
        if(!soc_property_suffix_num_get(unit,-1,spn_CUSTOM_FEATURE,"pon_without_rif_support",0)) {
            /* 
            * In case Routing is enabled Bank 0 must be reserved. 
            * mapping out-rif to VSI and remark-profile         .
            * Also Bank 0 is used in order to use entry 0 as a NULL entry.                                                  .                                                                                                              .
            * In that case Bank 0 must be not allocated for Out-AC.                                                                                                                                                                                                                             .
            */
            allocation_info.pool_id = dpp_am_res_eg_out_rif;
            rv = _bcm_dpp_am_egress_encap_alloc(unit,&allocation_info,0,4095,&elem);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        
        if (SOC_DPP_CONFIG(unit)->trill.mode && (SOC_IS_ARAD_A0(unit))) {
            /* 
             * In case Trill is enabled, for ARAD A0 we need to allocate entry SW_STATE_RES_ALLOC_WITH_ID to be invalid
             */
            SOC_PPC_EG_ENCAP_DATA_INFO data_info;
            uint32 soc_sand_rv;
            uint32 soc_sand_dev_id;

            allocation_info.pool_id = dpp_am_res_eg_data_trill_invalid_entry;
            elem = SOC_DPP_PP_FRWRD_TRILL_EEDB_INVALID_ENTRY(unit);
            rv = _bcm_dpp_am_egress_encap_alloc(unit,&allocation_info,SW_STATE_RES_ALLOC_WITH_ID,1,&elem);
            BCMDNX_IF_ERR_EXIT(rv);

            /* Null Data entry */
            SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_info);
            soc_sand_dev_id = (unit);
      
            soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id, SOC_DPP_PP_FRWRD_TRILL_EEDB_INVALID_ENTRY(unit), &data_info, FALSE, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }

        /* Allocate bank if Phase access isn't dynamic allocated */
        for (i = 0; i < SOC_PPC_EG_ENCAP_NOF_BANKS(unit); i++) {
            if (SOC_DPP_CONFIG(unit)->pp.egress_encap_bank_phase[i]) {
                switch (SOC_DPP_CONFIG(unit)->pp.egress_encap_bank_phase[i]) {
                case egress_encap_bank_phase_static_1:
                    bank_phase = SOC_PPC_EG_ENCAP_ACCESS_PHASE_ONE;
                    is_sync = FALSE;
                    break;
                case egress_encap_bank_phase_static_2:
                    bank_phase = SOC_PPC_EG_ENCAP_ACCESS_PHASE_TWO;
                    is_sync = FALSE;
                    break;
                case egress_encap_bank_phase_static_3:
                    bank_phase = SOC_PPC_EG_ENCAP_ACCESS_PHASE_THREE;
                    is_sync = FALSE;
                    break;
                case egress_encap_bank_phase_static_4:
                    bank_phase = SOC_PPC_EG_ENCAP_ACCESS_PHASE_FOUR;
                    is_sync = FALSE;
                    break;
                default:
                    BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
                }
        
                bank_id = i;
                rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, SW_STATE_RES_ALLOC_WITH_ID, bank_phase, &bank_id, &is_success);
                BCMDNX_IF_ERR_EXIT(rv);
                if (!is_success) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
                }
            }
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* De-Initalize egress encapsulation allocation */
uint32
_bcm_dpp_am_egress_encap_deinit(int unit)
{
    uint8 init;
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* 
     * If it is warm boot then the egress encap info was initialized
     */
    if (SOC_WARM_BOOT(unit))
    {
        BCM_EXIT;
    }

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.init.get(unit, &init);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!init) {
        /* Nothing to do egress encap already de-initialized */
        BCM_EXIT;
    }

    /* Done de-initalize */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.init.set(unit, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Allocate entry according to pool id */
uint32
_bcm_dpp_am_egress_encap_alloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint32 flags, int count, int* object)
{
    uint32 rv;
    int bank_id, object_id;
    uint8 is_sync_bank;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use _bcm_dpp_gport_alloc_global_and_local_lif.")));
    }
    
    /* Input validation, make sure lif is within valid range. */
    if (flags & (SW_STATE_RES_ALLOC_WITH_ID)) {
        if (*object >= SOC_DPP_DEFS_GET(unit, nof_out_lifs)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given outlif value is too high.")));
        }
    }

    /* Allocate object */
    rv = _bcm_dpp_am_egress_encap_internal_alloc(unit,allocation_info,FALSE,flags,count,object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    object_id = *object;
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit,bank_id,&is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to allocate also ingress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info,0,sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS; /* Sync Ingress */
        sync_info.egress_lif.pool_id = allocation_info->pool_id;
        sync_info.egress_lif.application_type = allocation_info->application_type;

        /* Always WITH_ID since we synchrnoize the other side */
        flags |= SW_STATE_RES_ALLOC_WITH_ID;
        rv = _bcm_dpp_am_sync_lif_alloc(unit, &sync_info,flags,count,object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_egress_encap_internal_alloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int count, int* object)
{
    uint32 rv;
    SOC_PPC_EG_ENCAP_ACCESS_PHASE phase_access = 0;
    int *banks_ids = NULL;
    int nof_valid_banks, bank_id, new_bank;
    uint32 alloc_size;
    uint8 is_exist;
    uint8 is_success;
    uint8 egress_encap_count;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    rv = _bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(unit, allocation_info, &phase_access);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_count.get(unit, &egress_encap_count);
    BCMDNX_IF_ERR_EXIT(rv);
    alloc_size = sizeof(int) * egress_encap_count;
    BCMDNX_ALLOC(banks_ids, alloc_size, "allocate bank ids");
    if (NULL == banks_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation bank ids")));
    }
    sal_memset(banks_ids, 0, alloc_size); 

    /* Try to allocate for each bank id unless we are dealing with case of WITH_ID */
    if (flags & (SW_STATE_RES_ALLOC_WITH_ID)) {
        int object_id;
        SOC_PPC_EG_ENCAP_ACCESS_PHASE bank_phase_access;

        object_id = *object;
        /* Find matched bank */
        bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
        /* Validate bank exist and with the correct phase */
        rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist == 0) {
            uint32 alloc_bank_flags;

            alloc_bank_flags = SW_STATE_RES_ALLOC_WITH_ID;
            /* Allocate new bank since it is not existed */
            rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, alloc_bank_flags, phase_access, &bank_id, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_success) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
            }
        }

        /* Validate phase number */
        rv = _bcm_dpp_am_egress_encap_bank_phase_get(unit, bank_id, &bank_phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        if (bank_phase_access != phase_access) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("given invalid object id %d, bank phase access %d, not matched to phase access %d"), object_id, bank_phase_access, phase_access));
        }

        rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_obs_eg_encap, flags, count, object);
        BCMDNX_IF_ERR_EXIT(rv);

    } else {
        uint8 alloc_success;

        alloc_success = FALSE;

        rv = _bcm_dpp_am_egress_encap_banks_by_access_phase(unit, is_sync, phase_access, &nof_valid_banks, banks_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
            rv = dpp_am_res_silent_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_eg_encap, flags,count,object);
            if (rv == BCM_E_NONE) {
                alloc_success = TRUE;
                /* Allocation succeded no need to find other banks */

                break;
            }
        }

        if (!alloc_success) {
            /* Allocate new bank since no success */
            rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, 0, phase_access, &new_bank, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_success) {
                /* Allocate object */
                rv = dpp_am_res_alloc(unit, new_bank, dpp_am_res_obs_eg_encap, flags, count, object);
                BCMDNX_IF_ERR_EXIT(rv);

                alloc_success = TRUE;
            } else {
                /* Retreive from sync banks as last option (no more bank to allocate) */
                rv = _bcm_dpp_am_egress_encap_banks_by_access_phase(unit, TRUE, phase_access, &nof_valid_banks, banks_ids);
                BCMDNX_IF_ERR_EXIT(rv);

                for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
                    rv = dpp_am_res_silent_alloc(unit, banks_ids[bank_id], dpp_am_res_obs_eg_encap, flags, count, object);
                    if (rv == BCM_E_NONE) {
                        alloc_success = TRUE;
                        /* Allocation succeded no need to find other banks */

                        break;
                    }
                }
            }
        }


        if (!alloc_success) {
            /* FAIL all full :( */
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(banks_ids);
    BCMDNX_FUNC_RETURN;
}

/* Allocate align entry according to pool id */
uint32
_bcm_dpp_am_egress_encap_alloc_align(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint32 flags, int align, int offset, int count, int* object)
{
    uint32 rv;
    int bank_id, object_id;
    uint8 is_sync_bank;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use _bcm_dpp_gport_alloc_global_and_local_lif.")));
    }

    /* Input validation, make sure lif is within valid range. */
    if (flags & (SW_STATE_RES_ALLOC_WITH_ID)) {
        if (*object >= SOC_DPP_DEFS_GET(unit, nof_out_lifs)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Given outlif value is too high.")));
        }
    }

    /* Allocate object */
    rv = _bcm_dpp_am_egress_encap_internal_alloc_align(unit,allocation_info,FALSE,flags,align,offset,count,object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    object_id = *object;
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit,bank_id,&is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to allocate also ingress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info,0,sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS; /* Sync Ingress */
        sync_info.egress_lif.pool_id = allocation_info->pool_id;
        sync_info.egress_lif.application_type = allocation_info->application_type;

        /* Always WITH_ID since we synchrnoize the other side */
        flags |= SW_STATE_RES_ALLOC_WITH_ID;
        rv = _bcm_dpp_am_sync_lif_alloc_align(unit, &sync_info,flags,align,offset,count,object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_egress_encap_internal_alloc_align(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, uint32 flags, int align, int offset, int count, int* object)
{
    uint32 rv;
    SOC_PPC_EG_ENCAP_ACCESS_PHASE phase_access = 0;
    int *banks_ids = NULL;
    int nof_valid_banks, bank_id, new_bank;
    uint32 alloc_size;
    uint8 is_exist;
    uint8 is_success;
    uint8 egress_encap_count;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    rv = _bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(unit, allocation_info, &phase_access);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_count.get(unit, &egress_encap_count);
    BCMDNX_IF_ERR_EXIT(rv);
    alloc_size = sizeof(int) * egress_encap_count;
    BCMDNX_ALLOC(banks_ids, alloc_size, "allocate bank ids");
    if (NULL == banks_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("No memory for allocation bank ids")));
    }
    sal_memset(banks_ids, 0, alloc_size); 

    /* Try to allocate on each bank id unless we are dealing with case of WITH_ID */
    if (flags & (SW_STATE_RES_ALLOC_WITH_ID)) {
        int object_id;
        SOC_PPC_EG_ENCAP_ACCESS_PHASE bank_phase_access = 0;

        object_id = *object;
        /* Find matched bank */
        bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
        /* Validate bank exist and with the correct phase */
        rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
        BCMDNX_IF_ERR_EXIT(rv);

        if (is_exist == 0) {
            uint32 alloc_bank_flags;

            alloc_bank_flags = SW_STATE_RES_ALLOC_WITH_ID;
            /* Allocate new bank since it is not existed */
            rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, alloc_bank_flags, phase_access, &bank_id, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (!is_success) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
            }
        }

        /* Validate phase number */
        rv = _bcm_dpp_am_egress_encap_bank_phase_get(unit, bank_id, &bank_phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        if (bank_phase_access != phase_access) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("given invalid object id %d, bank phase access %d, not matched to phase access %d"), object_id, bank_phase_access, phase_access));
        }

        /* if replace flag is set and exist then we are done */
        if(flags & BCM_DPP_AM_FLAG_ALLOC_REPLACE) {
            rv = dpp_am_res_check(unit, bank_id, dpp_am_res_obs_eg_encap, offset, count);
            if(rv == BCM_E_EXISTS) {
                rv = BCM_E_NONE;
                BCM_EXIT;
            }
        }
        rv = dpp_am_res_alloc_align(unit, bank_id, dpp_am_res_obs_eg_encap, flags, align, offset, count, object);
        BCMDNX_IF_ERR_EXIT(rv);

    } else {
        uint8 alloc_success;

        if (flags & BCM_DPP_AM_FLAG_ALLOC_REPLACE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, 
              (_BSL_BCM_MSG("cannot set replace when ID is not given")));
        }

        rv = _bcm_dpp_am_egress_encap_banks_by_access_phase(unit, is_sync, phase_access, &nof_valid_banks, banks_ids);
        BCMDNX_IF_ERR_EXIT(rv);

        alloc_success = FALSE;
        for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
            rv = dpp_am_res_silent_alloc_align(unit, banks_ids[bank_id], dpp_am_res_obs_eg_encap, flags, align, offset, count, object);
            if (rv == BCM_E_NONE) {
                alloc_success = TRUE;

                /* Allocation succeded no need to find other banks */
                break;
            }
        }

        if (!alloc_success) {
            /* Allocate new bank since no success */
            rv = _bcm_dpp_am_egress_encap_allocate_new_bank(unit, is_sync, 0, phase_access, &new_bank, &is_success);
            BCMDNX_IF_ERR_EXIT(rv);

            if (is_success) {
                /* Allocate object */
                rv = dpp_am_res_alloc_align(unit, new_bank, dpp_am_res_obs_eg_encap, flags, align, offset, count, object);
                BCMDNX_IF_ERR_EXIT(rv);

                alloc_success = TRUE;

            } else {
                /* Retreive from sync banks as last option (no more bank to allocate) */
                rv = _bcm_dpp_am_egress_encap_banks_by_access_phase(unit, TRUE, phase_access, &nof_valid_banks, banks_ids);
                BCMDNX_IF_ERR_EXIT(rv);

                for (bank_id = 0; bank_id < nof_valid_banks; ++bank_id) {
                    rv = dpp_am_res_silent_alloc_align(unit, banks_ids[bank_id], dpp_am_res_obs_eg_encap, flags, align, offset, count, object);
                    if (rv == BCM_E_NONE) {
                        alloc_success = TRUE;

                        /* Allocation succeded no need to find other banks */
                        break;
                    }
                }
            }
        }


        if (!alloc_success) {
            /* FAIL all full :( */
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("egres encap resources didnt find new bank avaiable")));
        }
    }

    BCM_EXIT;
exit:
    BCM_FREE(banks_ids);
    BCMDNX_FUNC_RETURN;
}

/* Deallocate entry according to object */
uint32
_bcm_dpp_am_egress_encap_dealloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    int bank_id;
    uint8 is_sync_bank;
    
    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use _bcm_dpp_gport_delete_global_and_local_lif.")));
    }

    /* Allocate object */
    rv = _bcm_dpp_am_egress_encap_internal_dealloc(unit,allocation_info,FALSE,count,object);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update sync in case of need */
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object);
    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit,bank_id,&is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_sync_bank) { /* allocated in synchronize bank, then needs to allocate also ingress */
        bcm_dpp_am_sync_lif_alloc_info_t sync_info;

        sal_memset(&sync_info,0,sizeof(sync_info));

        sync_info.sync_flags = _BCM_DPP_AM_SYNC_LIF_INGRESS; /* Sync Ingress */
        sync_info.egress_lif.pool_id = allocation_info->pool_id;
        sync_info.egress_lif.application_type = allocation_info->application_type;

        rv = _bcm_dpp_am_sync_lif_dealloc(unit, &sync_info,count,object);
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit:
    BCMDNX_FUNC_RETURN;
}
STATIC uint32
_bcm_dpp_am_egress_encap_internal_dealloc(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, uint8 is_sync, int count, int object)
{
    uint32 rv;
    uint8 is_exist;
    int bank_id;
    uint32 flags = 0;
    uint8 egress_encap_count;
    int used_count;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

   

    /* Find matched bank */
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object);
    /* Validate bank exist and with the correct phase */
    rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!is_exist) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("given invalid object id %d"), object));
    }

    rv = dpp_am_res_free_and_status(unit, bank_id, dpp_am_res_obs_eg_encap, count, object, &flags);
    BCMDNX_IF_ERR_EXIT(rv);

    /* If it's the last element in this bank, clear the "bank is allocated" indication, bank phase indication,
     *  and clear all eedb entries in this bank. 
     *  We won't clear the bank phase if it's been hard set by soc property.
     */
    if (flags & (SW_STATE_RES_FREED_TYPE_LAST_ELEM | SW_STATE_RES_FREED_POOL_LAST_ELEM)) {

        /* First, clear the HW. */
        rv = MBCM_PP_DRIVER_CALL(unit, mbcm_pp_encap_access_clear_bank, (unit, bank_id));
        BCMDNX_IF_ERR_EXIT(rv);

        /* Only clear the egress encap phase if it's not hard coded. */
        if (SOC_DPP_CONFIG(unit)->pp.egress_encap_bank_phase[bank_id] == egress_encap_bank_phase_dynamic) {
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_banks.bit_range_clear(unit,
                bank_id*_BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK, _BCM_DPP_AM_EGRESS_ENCAP_NOF_BITS_PER_BANK);
            BCMDNX_IF_ERR_EXIT(rv);
            
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_count.get(unit, &egress_encap_count);
            BCMDNX_IF_ERR_EXIT(rv);
            egress_encap_count--;
            rv = sw_state_access[unit].dpp.bcm.alloc_mngr.egress_encap.egress_encap_count.set(unit, egress_encap_count);
            BCMDNX_IF_ERR_EXIT(rv);
            
            /* Check if ingress bank is empty and sync bank status needs to be changed. */
            rv = dpp_am_res_used_count_get(unit, bank_id, dpp_am_res_obs_inlif, &used_count);
            BCMDNX_IF_ERR_EXIT(rv);
            
            if (!used_count) {
                rv = _bcm_dpp_am_sync_lif_reset_bank(unit, bank_id);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }
    } 

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Is exist entry according to pool id */
int
_bcm_dpp_am_egress_encap_is_allocated(int unit, bcm_dpp_am_egress_encap_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    int bank_id;
    uint8 bank_is_exist;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_EGRESS_ENCAP_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use bcm_dpp_am_global_lif_is_alloced.")));
    }

    /* Find matched bank */
    bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object);
    /* Validate bank exist and with the correct phase */
    rv = _bcm_dpp_am_egress_encap_is_exist_bank(unit, bank_id, &bank_is_exist);
    BCMDNX_IF_ERR_EXIT(rv);

    if (!bank_is_exist) {
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

    BCM_RETURN_VAL_EXIT(dpp_am_res_check(unit, bank_id, dpp_am_res_obs_eg_encap, count, object));
    
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Egress encapsulation DB - End
 */

/*
 * Sync LIF DB - Start 
 * ARAD only 
 * Synchornization LIF DB is needed in cases where application allocation requires both Ingress & Egress 
 * The handling and management is done by a set of APIs called _bcm_dpp_am_sync_lif_* 
 * APIs can be called directly from application view or indirect from ingress_lif or egress_encap set of APIs  
 */

/* Initalize ingress lif allocation */
uint32
_bcm_dpp_am_sync_lif_init(int unit)
{
    bcm_error_t rv;
    uint8       init;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_WARM_BOOT(unit))
    {
        BCM_EXIT;
    }

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.init.get(unit, &init);
    BCMDNX_IF_ERR_EXIT(rv);
    if (init) {
        /* Nothing to do ingress lif already initialized */
        BCM_EXIT;
    }

    /* Initialize database */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.sync_lif_banks.alloc_bitmap(unit, _BCM_DPP_AM_SYNC_LIF_NOF_BITS);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Done initalize */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.init.set(unit, TRUE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* De-Initalize ingress lif allocation */
uint32
_bcm_dpp_am_sync_lif_deinit(int unit)
{
    bcm_error_t rv;
    uint8       init;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_WARM_BOOT(unit))
    {
        BCM_EXIT;
    }

    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.init.get(unit, &init);
    BCMDNX_IF_ERR_EXIT(rv);
    if (!init) {
        /* Nothing to do ingress lif already de-initialized */
        BCM_EXIT;
    }

    /* Done De-initalize */
    rv = sw_state_access[unit].dpp.bcm.alloc_mngr.sync_lif.init.set(unit, FALSE);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/* Check is sync application */
STATIC uint32
_bcm_dpp_am_sync_lif_is_sync_application(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, uint8 *is_exist)
{
    
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(is_exist);

    *is_exist = FALSE;
    if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) { /* Information is located at ingress */
        switch (allocation_info->ingress_lif.pool_id) {
        case dpp_am_res_lif_dynamic:
          if (allocation_info->ingress_lif.application_type == bcm_dpp_am_ingress_lif_app_vpn_ac) {
              *is_exist = TRUE;
          }
          break;
        case dpp_am_res_lif_pwe:
          *is_exist = TRUE;
          break;
        default:
          break;
        }
    }

    if (*is_exist) {
        BCM_EXIT;
    }

    if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) { /* Information is located at egress */
        switch (allocation_info->egress_lif.pool_id) {
        case dpp_am_res_lif_dynamic: /* Only used for INOUT-AC */
          *is_exist = TRUE;
          break;
        case dpp_am_res_eep_mpls_tunnel:
          if (allocation_info->egress_lif.application_type == bcm_dpp_am_egress_encap_app_pwe) {
              *is_exist = TRUE;
          }
          break;
        default:
          break;
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * SYNC LIF allocation
 */
uint32
_bcm_dpp_am_sync_lif_alloc(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, uint32 flags, int count, int* object)
{
    uint32 rv;
    uint8 is_sync_application = FALSE, is_sync_bank = FALSE;
    int object_id = 0;
    int bank_id = 0;

    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use _bcm_dpp_gport_alloc_global_and_local_lif.")));
    }

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    /* Check it is sync application */
    rv = _bcm_dpp_am_sync_lif_is_sync_application(unit, allocation_info, &is_sync_application);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check it is sync bank */
    /* 
     * Sync bank: request for allocation on synchornization bank in application
     * that doesnt require LIF ID from both sides. 
     * This might be set in case all other banks are full except synchronize banks 
     * In that case, well skip on validation and just allocate ID 
     */
    if (!is_sync_application && (flags & (SW_STATE_RES_ALLOC_WITH_ID))) {
        object_id = *object;
        if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) {
          bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
        }

        if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) {
          bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
        }

        rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* in case of no need of synchrnoziation, well just quit */
    if (!is_sync_bank && !is_sync_application) {
        BCM_EXIT;
    }

    /* Sync bank management */
    if (is_sync_bank) {
      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS)) {
          bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
          rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_obs_inlif, flags, count, object);
          /* In case it is not sync application well we just wanted to allocate ID, in case it is already occupied just ignore */
          if (!is_sync_application && rv == BCM_E_EXISTS) { 
              rv = BCM_E_NONE;
          } else {

            BCMDNX_IF_ERR_EXIT(rv);

          }
  
      }

      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS)) {
          bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
          rv = dpp_am_res_alloc(unit, bank_id, dpp_am_res_obs_eg_encap, flags, count, object);
          /* In case it is not sync application well we just wanted to allocate ID, in case it is already occupied just ignore */
          if (!is_sync_application && rv == BCM_E_RESOURCE) { 
              rv = BCM_E_NONE;
          } else {
            BCMDNX_IF_ERR_EXIT(rv);
          }

      }
    }


    /* Sync application management */
    if (is_sync_application) {
      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS)) {
          rv = _bcm_dpp_am_ingress_lif_internal_alloc(unit,&(allocation_info->ingress_lif), TRUE, flags,count,object);
          BCMDNX_IF_ERR_EXIT(rv);
          flags |= SW_STATE_RES_ALLOC_WITH_ID;
      }

      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS)) {
          /* We cant get into here without WITH ID flag */
          if (!(flags & (SW_STATE_RES_ALLOC_WITH_ID))) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, 
             (_BSL_BCM_MSG("in case of synchrnozie application egress side is only with id, sync_flags %d alloc flags %d"), 
              allocation_info->sync_flags, flags));
          }
          rv = _bcm_dpp_am_egress_encap_internal_alloc(unit,&(allocation_info->egress_lif), TRUE, flags,count,object);
          BCMDNX_IF_ERR_EXIT(rv);
      }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_sync_lif_alloc_align(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, uint32 flags, int align, int offset, int count, int* object)
{
    uint32 rv;
    uint8 is_sync_application = FALSE, is_sync_bank = FALSE;
    int object_id = 0, bank_id = 0;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);
    BCMDNX_NULL_CHECK(object);

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use _bcm_dpp_gport_alloc_global_and_local_lif.")));
    }

    /* Check it is sync application */
    rv = _bcm_dpp_am_sync_lif_is_sync_application(unit, allocation_info, &is_sync_application);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Check it is sync bank */
    /* 
     * Sync bank: request for allocation on synchornization bank in application
     * that doesnt require LIF ID from both sides. 
     * This might be set in case all other banks are full except synchronize banks 
     * In that case, well skip on validation and just allocate ID 
     */
    if (!is_sync_application && (flags & (SW_STATE_RES_ALLOC_WITH_ID))) {
        object_id = *object;
        if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS) {
          bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
        }

        if (allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS) {
          bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
        }

        rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /* in case of no need of synchrnoziation, well just quit */
    if (!is_sync_bank && !is_sync_application) {
        BCM_EXIT;
    }

    /* Sync bank management */
    if (is_sync_bank) {
      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS)) {
          bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object_id);
          rv = dpp_am_res_alloc_align(unit, bank_id, dpp_am_res_obs_inlif, flags, align, offset, count, object);
          BCMDNX_IF_ERR_EXIT(rv);
      }

      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS)) {
          bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object_id);
          rv = dpp_am_res_alloc_align(unit, bank_id, dpp_am_res_obs_eg_encap, flags, align, offset, count, object);
          BCMDNX_IF_ERR_EXIT(rv);
      }
    }


    /* Sync application management */
    if (is_sync_application) {
      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS)) {
          rv = _bcm_dpp_am_ingress_lif_internal_alloc_align(unit,&(allocation_info->ingress_lif), TRUE, flags,align,offset,count,object);
          BCMDNX_IF_ERR_EXIT(rv);
          flags |= SW_STATE_RES_ALLOC_WITH_ID;
      }

      if ((allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS)) {
          /* We cant get into here without WITH ID flag */
          if (SOC_IS_ARAD_B1_AND_BELOW(unit) && (!(flags & (SW_STATE_RES_ALLOC_WITH_ID)))) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, 
             (_BSL_BCM_MSG("in case of synchrnozie application egress side is only with id, sync_flags %d alloc flags %d"), 
              allocation_info->sync_flags, flags));
          }
          rv = _bcm_dpp_am_egress_encap_internal_alloc_align(unit,&(allocation_info->egress_lif), TRUE, flags,align,offset,count,object);
          BCMDNX_IF_ERR_EXIT(rv);
      }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


uint32
_bcm_dpp_am_sync_lif_dealloc(int unit, bcm_dpp_am_sync_lif_alloc_info_t* allocation_info, int count, int object)
{
    uint32 rv;
    uint8 is_sync_bank = FALSE, is_sync_application = FALSE;
    int bank_id = 0;
    uint8 is_ingress, is_egress;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_SYNC_LIF_VALID_CHECK;
    BCMDNX_NULL_CHECK(allocation_info);

    if (SOC_IS_JERICHO(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("This function is not available on this device. "
                                                         "Please use _bcm_dpp_gport_delete_global_and_local_lif.")));
    }

    is_ingress = allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_INGRESS;
    is_egress = allocation_info->sync_flags & _BCM_DPP_AM_SYNC_LIF_EGRESS;


    if (is_ingress) {
      bank_id = _BCM_DPP_AM_INGRESS_LIF_OBJECT_ID_TO_BANK_ID(object);
    }

    if (is_egress) {
      bank_id = _BCM_DPP_AM_EGRESS_ENCAP_OBJECT_ID_TO_BANK_ID(object);
    }

    rv = _bcm_dpp_am_sync_lif_is_sync_bank(unit, bank_id, &is_sync_bank);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Entry might be a sync entry used WITH_ID in a non sync bank. Check by application. */
    if (!is_sync_bank) {
        rv = _bcm_dpp_am_sync_lif_is_sync_application(unit, allocation_info, &is_sync_application);
        BCMDNX_IF_ERR_EXIT(rv);

        if (!is_sync_application) {
            /* No need to deallocate. */
            BCM_EXIT;
        }
    }

    if (is_ingress) {
        rv = _bcm_dpp_am_ingress_lif_internal_dealloc(unit, &(allocation_info->ingress_lif), TRUE, count, object);
        if (rv == BCM_E_NOT_FOUND) { /* there might be a case of request for dealloc twice */
          rv = BCM_E_NONE;
        }
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (is_egress) {
        rv = _bcm_dpp_am_egress_encap_internal_dealloc(unit, &(allocation_info->egress_lif), TRUE, count, object);
        if (rv == BCM_E_NOT_FOUND) {
          rv = BCM_E_NONE;
        }
        BCMDNX_IF_ERR_EXIT(rv);
    }


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}
/* Sync LIF DB - End */

