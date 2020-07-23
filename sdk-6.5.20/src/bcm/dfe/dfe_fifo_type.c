/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC DFE FIFO TYPE
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FABRIC
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>

#include <bcm_int/dfe/dfe_fifo_type.h>

#include <soc/dfe/cmn/dfe_warm_boot.h>
#include <soc/dfe/cmn/dfe_drv.h>


typedef struct bcm_dfe_fifo_type_s
{
    _bcm_dfe_fifo_type_handle_t rx_fe1_primary:1,           
                                rx_fe1_secondary:1,              
                                rx_fe3_primary:1,
                                rx_fe3_secondary:1,
                                tx_fe1_primary:1,           
                                tx_fe1_secondary:1,              
                                tx_fe3_primary:1,
                                tx_fe3_secondary:1,         
                                is_handle_flag:1, /* required for the new attach/set implementation */         
                                spare:15,
                                fifo_id:8;
            
} bcm_dfe_fifo_type_t;

/*
 * Reset fifo_type handle
 */
int bcm_dfe_fifo_type_clear(int unit, _bcm_dfe_fifo_type_handle_t *h)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(h);

    rc = bcm_dfe_fifo_type_set(unit,h,0,0,0,0,0,0);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = bcm_dfe_fifo_type_set_id(unit,h,0);
    BCMDNX_IF_ERR_EXIT(rc);
    ((bcm_dfe_fifo_type_t*)h)->spare = 0;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Check whether fifo_type configure rx thresholds
 */
int bcm_dfe_fifo_type_is_rx(int unit, _bcm_dfe_fifo_type_handle_t h, int *is_rx)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_rx);

    *is_rx = ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe1_primary ||
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe1_secondary || 
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe3_primary || 
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe3_secondary;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Check whether fifo_type configure tx thresholds
 */
int bcm_dfe_fifo_type_is_tx(int unit, _bcm_dfe_fifo_type_handle_t h, int *is_tx)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_tx);

    *is_tx = ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe1_primary ||
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe1_secondary || 
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe3_primary || 
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe3_secondary;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Check whether fifo_type configure fe1 thresholds
 */
int bcm_dfe_fifo_type_is_fe1(int unit, _bcm_dfe_fifo_type_handle_t h, int *is_fe1)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_fe1);

    *is_fe1 = ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe1_primary ||
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe1_secondary || 
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe1_primary || 
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe1_secondary;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Check whether fifo_type configure fe3 thresholds
 */
int bcm_dfe_fifo_type_is_fe3(int unit, _bcm_dfe_fifo_type_handle_t h, int *is_fe3)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_fe3);

    *is_fe3 = ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe3_primary ||
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe3_secondary || 
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe3_primary || 
                ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe3_secondary;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Check whether fifo_type configure primary pipe thresholds
 */
int bcm_dfe_fifo_type_is_primary(int unit, _bcm_dfe_fifo_type_handle_t h, int *is_primary)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_primary);

    *is_primary = ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe1_primary ||
                    ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe3_primary || 
                    ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe1_primary || 
                    ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe3_primary;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Check whether fifo_type configure secondary pipe thresholds
 */
int bcm_dfe_fifo_type_is_secondary(int unit, _bcm_dfe_fifo_type_handle_t h, int *is_secondary)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_secondary);

    *is_secondary = ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe1_secondary ||
                        ((bcm_dfe_fifo_type_t*)(void *)(&h))->rx_fe3_secondary || 
                        ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe1_secondary || 
                        ((bcm_dfe_fifo_type_t*)(void *)(&h))->tx_fe3_secondary;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Set fifo_type parameters
 */
int bcm_dfe_fifo_type_set(int unit, _bcm_dfe_fifo_type_handle_t *h, int is_rx, int is_tx, int is_fe1, int is_fe3, int is_primary, int is_secondary)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(h);

    ((bcm_dfe_fifo_type_t*)h)->rx_fe1_primary = ((is_rx && is_fe1 && is_primary) ? 1 : 0);
    ((bcm_dfe_fifo_type_t*)h)->rx_fe1_secondary = ((is_rx && is_fe1 && is_secondary) ? 1 : 0);
    ((bcm_dfe_fifo_type_t*)h)->rx_fe3_primary = ((is_rx && is_fe3 && is_primary) ? 1 : 0);
    ((bcm_dfe_fifo_type_t*)h)->rx_fe3_secondary = ((is_rx && is_fe3 && is_secondary) ? 1 : 0);
    ((bcm_dfe_fifo_type_t*)h)->tx_fe1_primary = ((is_tx && is_fe1 && is_primary) ? 1 : 0);
    ((bcm_dfe_fifo_type_t*)h)->tx_fe1_secondary = ((is_tx && is_fe1 && is_secondary) ? 1 : 0); 
    ((bcm_dfe_fifo_type_t*)h)->tx_fe3_primary = ((is_tx && is_fe3 && is_primary) ? 1 : 0);
    ((bcm_dfe_fifo_type_t*)h)->tx_fe3_secondary = ((is_tx && is_fe3 && is_secondary) ? 1 : 0);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Get fifo_type id
 */
int bcm_dfe_fifo_type_get_id(int unit, _bcm_dfe_fifo_type_handle_t h, int *id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(id);

    *id = ((bcm_dfe_fifo_type_t*)(void *)(&h))->fifo_id;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Set fifo_type id
 */
int bcm_dfe_fifo_type_set_id(int unit, _bcm_dfe_fifo_type_handle_t* h, int fifo_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(h);

    ((bcm_dfe_fifo_type_t*)h)->fifo_id = fifo_id;

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Set is_handle_flag
 */ 
int bcm_dfe_fifo_type_set_handle_flag(int unit, _bcm_dfe_fifo_type_handle_t *h)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(h);

    ((bcm_dfe_fifo_type_t*)h)->is_handle_flag = 1;

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * get is_handle_flag
 */
int bcm_dfe_fifo_type_get_handle_flag(int unit, _bcm_dfe_fifo_type_handle_t h, int *is_handle_flag)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_handle_flag);

    if ( ((bcm_dfe_fifo_type_t*)(void *)(&h))->is_handle_flag == 1)
    {
        *is_handle_flag = 1;
    }
    else
    {
        *is_handle_flag = 0;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Compare two fifo_types whether they overlap
 */
int bcm_dfe_fifo_type_is_overlap(int unit, _bcm_dfe_fifo_type_handle_t h1, _bcm_dfe_fifo_type_handle_t h2, int* is_overlap)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(is_overlap);

    if(((bcm_dfe_fifo_type_t*)(void *)(&h1))->rx_fe1_primary && ((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe1_primary){
        *is_overlap = 1;
        BCM_EXIT;
    }

    if(((bcm_dfe_fifo_type_t*)(void *)(&h1))->rx_fe1_secondary && ((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe1_secondary){
        *is_overlap = 1;
        BCM_EXIT;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h1))->rx_fe3_primary && ((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe3_primary){
        *is_overlap = 1;
        BCM_EXIT;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h1))->rx_fe3_secondary && ((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe3_secondary){
        *is_overlap = 1;
        BCM_EXIT;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h1))->tx_fe1_primary && ((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe1_primary){
        *is_overlap = 1;
        BCM_EXIT;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h1))->tx_fe1_secondary && ((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe1_secondary){
        *is_overlap = 1;
        BCM_EXIT;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h1))->tx_fe3_primary && ((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe3_primary){
        *is_overlap = 1;
        BCM_EXIT;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h1))->tx_fe3_secondary && ((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe3_secondary){
        *is_overlap = 1; 
        BCM_EXIT;     
    }
      
    *is_overlap = 0;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Add fifo_type h2 to fifo_type h1
 */
int bcm_dfe_fifo_type_add(int unit, soc_dfe_fabric_link_fifo_type_index_t fifo_type, _bcm_dfe_fifo_type_handle_t h2)
{
    _bcm_dfe_fifo_type_handle_t h1;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, FIFO_HANDLERS, fifo_type, &h1);
    BCMDNX_IF_ERR_EXIT(rv);

    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe1_primary){
        ((bcm_dfe_fifo_type_t*)(void *) &h1)->rx_fe1_primary = 1;
    }
  
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe1_secondary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->rx_fe1_secondary = 1;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe3_primary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->rx_fe3_primary = 1;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe3_secondary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->rx_fe3_secondary = 1;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe1_primary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->tx_fe1_primary = 1;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe1_secondary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->tx_fe1_secondary = 1;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe3_primary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->tx_fe3_primary = 1;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe3_secondary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->tx_fe3_secondary = 1;   
    }

    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, FIFO_HANDLERS, fifo_type, &h1);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Sub fifo_type h2 from fifo_type h1
 */
int bcm_dfe_fifo_type_sub(int unit, soc_dfe_fabric_link_fifo_type_index_t fifo_type, _bcm_dfe_fifo_type_handle_t h2)
{
    _bcm_dfe_fifo_type_handle_t h1;
    int rv;
    BCMDNX_INIT_FUNC_DEFS;
    
    rv = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, FIFO_HANDLERS, fifo_type, &h1);
    BCMDNX_IF_ERR_EXIT(rv);

    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe1_primary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->rx_fe1_primary = 0;
    }
  
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe1_secondary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->rx_fe1_secondary = 0;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe3_primary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->rx_fe3_primary = 0;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->rx_fe3_secondary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->rx_fe3_secondary = 0;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe1_primary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->tx_fe1_primary = 0;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe1_secondary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->tx_fe1_secondary = 0;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe3_primary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->tx_fe3_primary = 0;
    }
    
    if(((bcm_dfe_fifo_type_t*)(void *)(&h2))->tx_fe3_secondary){
        ((bcm_dfe_fifo_type_t*)(void *)&h1)->tx_fe3_secondary = 0;
    }

    rv = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, FIFO_HANDLERS, fifo_type, &h1);
    BCMDNX_IF_ERR_EXIT(rv);
exit:
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

