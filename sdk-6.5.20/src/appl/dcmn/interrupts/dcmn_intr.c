/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*************
 * INCLUDES  *
 */
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/switch.h>
#include <bcm_int/common/debug.h>
#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#endif /* BCM_PETRA_SUPPORT */

#include <appl/diag/system.h>
#include <appl/dcmn/interrupts/dcmn_intr.h>

/*************
 * DEFINES   *
 */
#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_APPL_INTR

/*************
 * TYPE DEFS *
 */
static intr_common_params_t interrupt_common_params[SOC_MAX_NUM_DEVICES];

/*************
 * FUNCTIONS *
 */
int
interrupt_appl_ref_log_defaults_set(int unit)
{
     int rv,i;
     soc_interrupt_db_t* interrupts;
     soc_block_types_t  block;
     soc_reg_t reg;
     int blk;
     bcm_switch_event_control_t event;

    BCMDNX_INIT_FUNC_DEFS;
#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit))  
#endif
    {
       event.action = bcmSwitchEventLog;
       event.index = 0;
       event.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    
       rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event, 0x1);
       BCMDNX_IF_ERR_EXIT(rv);

       interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;
       for (i=0 ; interrupt_common_params[unit].int_disable_print_on_init[i] != INVALIDr; i++) {
            event.event_id = interrupt_common_params[unit].int_disable_print_on_init[i];  
            reg = interrupts[event.event_id].reg;

            /*Unsupported interrupts*/
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block) {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,0);
                BCMDNX_IF_ERR_EXIT(rv);
            }
       }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
interrupt_appl_ref_mask_defaults_set(int unit)
{
    int rv, i;
    soc_interrupt_db_t* interrupts;
    soc_block_types_t  block;
    soc_reg_t reg;
    int blk;
    bcm_switch_event_control_t event;
#ifdef BCM_DPP_SUPPORT
    uint32 reg32_val;
#endif

    BCMDNX_INIT_FUNC_DEFS;

    if(!SOC_INTR_IS_SUPPORTED(unit)) {

        LOG_ERROR(BSL_LS_APPL_INTR, (BSL_META_U(unit, "ERROR: No interrupts for device\n")));
        return SOC_E_UNAVAIL;
    }
    
    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

#ifdef BCM_WARM_BOOT_SUPPORT
    if (!SOC_WARM_BOOT(unit)) 
#endif
    {
        for (i=0 ; interrupt_common_params[unit].int_disable_on_init[i] != INVALIDr; i++) { 
            event.event_id = interrupt_common_params[unit].int_disable_on_init[i];
            event.action = bcmSwitchEventMask;
            reg = interrupts[event.event_id].reg;

            /*Unsupported interrupts*/
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block) {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,1);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

        for (i=0 ; interrupt_common_params[unit].int_active_on_init[i] != INVALIDr; i++) {
            event.event_id = interrupt_common_params[unit].int_active_on_init[i]; 
            event.action = bcmSwitchEventMask;
            reg = interrupts[event.event_id].reg;

            /*Unsupported interrupts*/
            if (!SOC_REG_IS_VALID(unit, reg))
            {
                continue;
            }

            block = SOC_REG_INFO(unit, reg).block;
            SOC_BLOCKS_ITER(unit, blk, block) {
                event.index = (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLP || SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLP) ? SOC_BLOCK_PORT(unit, blk) : SOC_BLOCK_NUMBER(unit, blk);
                rv = bcm_switch_event_control_set(unit,BCM_SWITCH_EVENT_DEVICE_INTERRUPT,event,0);
                BCMDNX_IF_ERR_EXIT(rv);
            }
        }

#ifdef BCM_DPP_SUPPORT
        if (SOC_IS_JERICHO(unit)) {
            reg32_val = 0;
            /*clear the PPDB_A_TCAM_PROTECTION_ERROR bit after the TCAM banks are cleared*/
            soc_reg_field_set(unit, PPDB_A_INTERRUPT_REGISTERr, &reg32_val, TCAM_PROTECTION_ERRORf, 1);
            BCMDNX_IF_ERR_EXIT(WRITE_PPDB_A_INTERRUPT_REGISTERr(unit, reg32_val));
        }
#endif
    }

exit:
    BCMDNX_FUNC_RETURN;
}

void interrupt_appl_cb(
    int unit, 
    bcm_switch_event_t event, 
    uint32 arg1, 
    uint32 arg2, 
    uint32 arg3, 
    void *userdata)
{
    if(BCM_SWITCH_EVENT_DEVICE_INTERRUPT != event) {
        return;
    }

    if (soc_property_suffix_num_get(unit,-1, spn_CUSTOM_FEATURE, "ser_reset_cb_en", 0)) {
        LOG_DEBUG(BSL_LS_APPL_INTR, (BSL_META_U(unit, "user callback called for interrupt number %u, instance %u, reset %u\n"), arg1, arg2, arg3));
    } else {
        LOG_DEBUG(BSL_LS_APPL_INTR, (BSL_META_U(unit, "user callback called for interrupt number %u, instance %u\n"), arg1, arg2));
    }
    return;
}

int
interrupt_appl_init(int unit)
{
    int rc, flag = 0;
    bcm_switch_event_control_t bcm_switch_event_control;

    BCMDNX_INIT_FUNC_DEFS;
 
#ifdef BCM_QUX_SUPPORT
    if (SOC_IS_QUX(unit)){
        rc = qux_interrupt_cmn_param_init(unit, &interrupt_common_params[unit]);
        BCMDNX_IF_ERR_EXIT(rc);
    } else
#endif 
#ifdef BCM_QAX_SUPPORT
    if (SOC_IS_QAX(unit)){
        rc = qax_interrupt_cmn_param_init(unit, &interrupt_common_params[unit]);
        BCMDNX_IF_ERR_EXIT(rc);
    } else
#endif
#ifdef BCM_JERICHO_PLUS_SUPPORT
    if (SOC_IS_JERICHO_PLUS(unit)) {
       rc = jerp_interrupt_cmn_param_init(unit, &interrupt_common_params[unit]);
       BCMDNX_IF_ERR_EXIT(rc);
    } else
#endif
#ifdef BCM_JERICHO_SUPPORT
    if (SOC_IS_JERICHO(unit)){
        rc = jer_interrupt_cmn_param_init(unit, &interrupt_common_params[unit]);
        BCMDNX_IF_ERR_EXIT(rc);
    } else
#endif
#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_FE3200(unit)){
        rc = fe3200_interrupt_cmn_param_init(unit, &interrupt_common_params[unit]);
        BCMDNX_IF_ERR_EXIT(rc);
    } else
#endif
    {
        LOG_ERROR(BSL_LS_APPL_INTR, (BSL_META_U(unit, "ERROR: Interrupt not supported\n")));
        return SOC_E_UNAVAIL;
    }

    rc = bcm_switch_event_register(unit, interrupt_appl_cb, NULL);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = interrupt_appl_ref_log_defaults_set(unit);
    BCMDNX_IF_ERR_EXIT(rc);

#ifdef BCM_PETRA_SUPPORT
    flag = (SOC_IS_DPP(unit) && (SOC_DPP_CONFIG(unit)->emulation_system == 0));
#endif

    /* we dont enable  the mask registers  on emulation since not all block exists and its cause access problems*/
    if (flag || SOC_IS_DFE(unit)) {
        rc = interrupt_appl_ref_mask_defaults_set(unit);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    /* Enable all interrupts*/
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x0); 
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:
    BCMDNX_FUNC_RETURN;
}

static int interrupt_appl_deinit_internal(int unit, int cache_upd_req)
{
    int rc;
    bcm_switch_event_control_t bcm_switch_event_control;

    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_switch_event_unregister(unit, interrupt_appl_cb, NULL);
    BCMDNX_IF_ERR_CONT(rc);

    if (cache_upd_req) {
        /* Update cached memories to detect ECC and parity errors */
        rc = bcm_switch_control_set(unit, bcmSwitchCacheTableUpdateAll, 0);
        BCMDNX_IF_ERR_CONT(rc); 
    }
 
    /* Disable all interrupts*/
    bcm_switch_event_control.action = bcmSwitchEventMask;
    bcm_switch_event_control.event_id = BCM_SWITCH_EVENT_CONTROL_ALL;
    rc = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, bcm_switch_event_control, 0x1); 
    BCMDNX_IF_ERR_CONT(rc); 
 
    BCMDNX_FUNC_RETURN;
}

int interrupt_appl_deinit(int unit)
{
    return (interrupt_appl_deinit_internal(unit, 1));
}

int interrupt_appl_nocache_deinit(int unit)
{
    return (interrupt_appl_deinit_internal(unit, 0));
}


uint32 *interrupt_active_on_intr_get(int unit)
{
    return interrupt_common_params[unit].int_active_on_init;
}

#undef _ERR_MSG_MODULE_NAME

