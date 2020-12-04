/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DNXF MULTICAST ID MAP
 */
 
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_MCAST

#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/dnxf/dnxf_multicast_id_map.h>

#include <shared/bitop.h>

#include <sal/core/alloc.h>

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxf/cmn/mbcm.h>

/*
 * Clear multicast id map
 */
STATIC int bcm_dnxf_multicst_id_map_clear(int unit)
{
    bcm_multicast_t i;
    int rc;
    uint32 max_mc_id;
    SHR_FUNC_INIT_VARS(unit);
    
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &max_mc_id));
    SHR_IF_ERR_EXIT(rc);

    for(i=0 ; i < max_mc_id ; i++)
    {
        rc = bcm_dnxf_multicst_id_map_clear_id(unit, i);
        SHR_IF_ERR_EXIT(rc);
    }

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Initialize multicast id map
 */
int bcm_dnxf_multicst_id_map_init(int unit, soc_dnxf_multicast_table_mode_t mc_mode)
{
    int rc;
    SHR_FUNC_INIT_VARS(unit);
    
    /*Clear map*/

    rc = dnxf_state.mc.mode.set(unit, mc_mode);
    SHR_IF_ERR_EXIT(rc);

    /*Clear map*/
    rc = bcm_dnxf_multicst_id_map_clear(unit);
    SHR_IF_ERR_EXIT(rc);

exit:
    SHR_FUNC_EXIT;
}


/*
 * Clear multicast id
 */
int bcm_dnxf_multicst_id_map_clear_id(int unit, bcm_multicast_t id)
{
    int entry_id,
        inner_bit,
        rv;
    uint32 mc_map_entry[1];
    uint32 max_ids;
    SHR_FUNC_INIT_VARS(unit);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &max_ids));
    SHR_IF_ERR_EXIT(rv);
    
    if(id < max_ids) {
        /*Each warm boot entry is uint32*/
        entry_id = id / 32;
        inner_bit = id % 32;

        rv = dnxf_state.mc.id_map.get(unit, entry_id, mc_map_entry);
        SHR_IF_ERR_EXIT(rv);

        SHR_BITCLR(mc_map_entry, inner_bit);
    } else {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Multicast id is out of range"); 
    }

    rv = dnxf_state.mc.id_map.set(unit, entry_id, mc_map_entry[0]);
    SHR_IF_ERR_EXIT(rv);


exit:
    SHR_FUNC_EXIT; 
}

/*
 * Select multicast id
 */
int bcm_dnxf_multicst_id_map_mark_id(int unit, bcm_multicast_t id)
{
    int entry_id,
        inner_bit,
        rv;
    uint32 mc_map_entry[1];
    uint32 max_ids;
    SHR_FUNC_INIT_VARS(unit);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &max_ids));
    SHR_IF_ERR_EXIT(rv);

    if(id < max_ids) {
        /*Each warm boot entry is uint32*/
        entry_id = id / 32;
        inner_bit = id % 32;

        rv = dnxf_state.mc.id_map.get(unit, entry_id, mc_map_entry);
        SHR_IF_ERR_EXIT(rv);

        SHR_BITSET(mc_map_entry, inner_bit);
    } else {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Milticast id is out of range"); 
    }

    rv = dnxf_state.mc.id_map.set(unit, entry_id, mc_map_entry[0]);
    SHR_IF_ERR_EXIT(rv);
exit:
    SHR_FUNC_EXIT; 
}

/*
 * Check whether mc id is legal
 */
int bcm_dnxf_multicst_id_map_is_legal_id(int unit, bcm_multicast_t id, int* is_legal)
{
    uint32 max_ids;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_legal, _SHR_E_PARAM, "is_legal");

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &max_ids));
    SHR_IF_ERR_EXIT(rv);

    *is_legal = (id < max_ids);

exit:
    SHR_FUNC_EXIT;  
}

/*
 * Check whether mc id is free
 */
int bcm_dnxf_multicst_id_map_is_free_id(int unit, bcm_multicast_t id, int* is_free)
{
    uint32 max_ids;
    int entry_id,
        inner_bit,
        rv;
    uint32 mc_map_entry[1];
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_free, _SHR_E_PARAM, "is_free");

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &max_ids));
    SHR_IF_ERR_EXIT(rv);

    if(id < max_ids) {
        /*Each warm boot entry is uint32*/
        entry_id = id / 32;
        inner_bit = id % 32;

        rv = dnxf_state.mc.id_map.get(unit, entry_id, mc_map_entry);
        SHR_IF_ERR_EXIT(rv);

        *is_free = (SHR_BITGET(mc_map_entry, inner_bit) ? 0 :1);
    } else {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Milticast id is out of range"); 
    }

exit:
    SHR_FUNC_EXIT; 
}

/*
 * Get free mc id
 */
int bcm_dnxf_multicst_id_map_get_free_id(int unit, bcm_multicast_t* id)
{
    bcm_multicast_t i;
    int is_free, rc;
    uint32 max_ids;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(id, _SHR_E_PARAM, "id");
    
    rc = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_size_get, (unit, &max_ids));
    SHR_IF_ERR_EXIT(rc);

    for(i=0 ; i< max_ids; i++)
    {
        rc = bcm_dnxf_multicst_id_map_is_free_id(unit, i, &is_free);
        SHR_IF_ERR_EXIT(rc);
        if(is_free)
        {
            *id = i;
            SHR_EXIT();
        }
    }

    /*id not found*/
     SHR_ERR_EXIT(_SHR_E_FULL, "Milticast free id not found"); 
    
exit:
    SHR_FUNC_EXIT; 
}

#undef BSL_LOG_MODULE

