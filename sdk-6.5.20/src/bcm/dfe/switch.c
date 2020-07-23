/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE STAT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_INTR

#include <shared/bsl.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm/debug.h>
#include <bcm/switch.h>

#include <soc/drv.h>
#include <soc/intr.h>

#include <soc/dcmn/dcmn_mem.h>
#include <soc/sand/sand_mem.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>

#ifdef BCM_88750_SUPPORT
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#endif

/*
 * Function:
 *    bcm_dfe_switch_event_register
 * Description:
 *    Registers a call back function for switch critical events
 * Parameters:
 *    unit        - Device unit number
 *  cb          - The desired call back function to register for critical events.
 *  userdata    - Pointer to any user data to carry on.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      
 *    Several call back functions could be registered, they all will be called upon
 *    critical event. If registered callback is called it is adviced to log the 
 *  information and reset the chip. 
 *  Same call back function with different userdata is allowed to be registered. 
 */
int 
bcm_dfe_switch_event_register(int unit, bcm_switch_event_cb_t cb, void *userdata)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE(unit); 
    rc = soc_event_register(unit, (soc_event_cb_t)cb, userdata);
     BCMDNX_IF_ERR_EXIT(rc);

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}


/*
 * Function:
 *    bcm_dfe_switch_event_unregister
 * Description:
 *    Unregisters a call back function for switch critical events
 * Parameters:
 *    unit        - Device unit number
 *  cb          - The desired call back function to unregister for critical events.
 *  userdata    - Pointer to any user data associated with a call back function
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      
 *  If userdata = NULL then all matched call back functions will be unregistered,
 */
int 
bcm_dfe_switch_event_unregister(int unit, bcm_switch_event_cb_t cb, void *userdata)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE(unit); 
    rc = soc_event_unregister(unit, (soc_event_cb_t)cb, userdata);
     BCMDNX_IF_ERR_EXIT(rc);

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *    bcm_dfe_switch_event_control_set
 * Description:
 *    Set event control
 * Parameters:
 *    unit        - Device unit number
 *  type        - Event action.
 *  value       - Event value
 * Returns:
 *      BCM_E_xxx
 */
int bcm_dfe_switch_event_control_set(
    int unit, 
    bcm_switch_event_t switch_event,
    bcm_switch_event_control_t type, 
    uint32 value)
{
    int rc = BCM_E_NONE, nof_interrupts;
    soc_interrupt_db_t* interrupts;
    int inter = 0;
    soc_block_info_t *bi;
    int bi_index = 0, port;
    int is_valid;
    soc_block_types_t block_types;

    BCMDNX_INIT_FUNC_DEFS;
    
    DFE_UNIT_LOCK_TAKE(unit);

    if (switch_event != BCM_SWITCH_EVENT_DEVICE_INTERRUPT ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_SOC_MSG("supports only interrupts event")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

    /*verify interrupt id*/
    rc = soc_nof_interrupts(unit, &nof_interrupts);
    BCMDNX_IF_ERR_EXIT(rc);

    if (type.event_id != BCM_SWITCH_EVENT_CONTROL_ALL) {
        if(type.event_id >= nof_interrupts || type.event_id<0) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid interrupt")));
        }

        /*verify block instance*/
        if(!SOC_REG_IS_VALID(unit, interrupts[type.event_id].reg)){
            BCMDNX_ERR_EXIT_MSG(SOC_E_INTERNAL, (_BSL_SOC_MSG("Invalid interrupt register for the device")));
        }
        block_types = SOC_REG_INFO(unit, interrupts[type.event_id].reg).block;
        rc = soc_is_valid_block_instance(unit, block_types, type.index, &is_valid);
        BCMDNX_IF_ERR_EXIT(rc);
        if (!is_valid) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported block instance")));
        }
    }
    
    /*switch case for all*/
    switch(type.action) {
    
    case bcmSwitchEventMask:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_interrupt_all_enable_set, (unit, value ? 0 : 1));
            BCMDNX_IF_ERR_EXIT(rc);
            
            } else {
                /* Set per interrupt */
                if (value) {
                    rc = soc_interrupt_disable(unit, type.index, &(interrupts[type.event_id]));
                } else {
                    rc = soc_interrupt_enable(unit, type.index, &(interrupts[type.event_id]));
                }
                BCMDNX_IF_ERR_EXIT(rc);
            }
            break;

    case bcmSwitchEventForce:
        /* Set/clear per interrupt */
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            for (inter = 0; inter < nof_interrupts; inter++) {

                for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++) {

                    if (!SOC_INFO(unit).block_valid[bi_index]) {
                        continue;
                    }

                    bi = &(SOC_BLOCK_INFO(unit, bi_index));

                    rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                    BCMDNX_IF_ERR_EXIT(rc);
                    if (is_valid) {
                        port = soc_interrupt_get_intr_port_from_index(unit, inter, bi->number);
                        rc = soc_interrupt_force(unit, port, &(interrupts[inter]), 1 - value);
                        BCMDNX_IF_ERR_EXIT(rc);
                    }
                }
            }
        } else {
            rc = soc_interrupt_force(unit, type.index, &(interrupts[type.event_id]), 1 - value);
            BCMDNX_IF_ERR_EXIT(rc);
        }

        break;

    case bcmSwitchEventClear:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            if (value) {
                rc = soc_interrupt_clear_all(unit);
                BCMDNX_IF_ERR_EXIT(rc);
            } else {
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid event clear parameter")));
            }
        } else {
            /* Set per interrupt */
            if (value) {
                if (NULL == interrupts[type.event_id].interrupt_clear) {
                    if (interrupts[type.event_id].vector_id == 0) {
                       BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("Interrupt not cleared, NULL pointer of interrupt_clear, no vector_id")));
                    } else {
                       LOG_WARN(BSL_LS_BCM_INTR,
                                (BSL_META_U(unit,
                                            "Warning: call to interrupt clear for vector pointer, nothing done\n")));
                    }
                } else {
                    rc = interrupts[type.event_id].interrupt_clear(unit, type.index, type.event_id);
                    BCMDNX_IF_ERR_EXIT(rc);
                }
            } else {
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid event clear parameter")));
            }
        }
        break;

    case bcmSwitchEventStormTimedCount:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            for (inter = 0; inter < nof_interrupts; inter++) {
                for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++ ) {

                bi = &(SOC_BLOCK_INFO(unit, bi_index));
                rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                BCMDNX_IF_ERR_EXIT(rc);

                    if (is_valid) {
                        rc = soc_interrupt_storm_timed_count_set(unit, inter, value);
                        BCMDNX_IF_ERR_EXIT(rc);
                        (interrupts[inter].storm_detection_occurrences)[bi->number] = 0x0;
                    }
                }
            }
        } else {
            /* Set per interrupt */
            rc = soc_interrupt_storm_timed_count_set(unit, type.event_id, value);
            BCMDNX_IF_ERR_EXIT(rc);
           (interrupts[type.event_id].storm_detection_occurrences)[type.index] = 0x0;
        }
        break;

    case bcmSwitchEventStormTimedPeriod:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            for (inter = 0; inter < nof_interrupts; inter++) {
                for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++ ) {

                bi = &(SOC_BLOCK_INFO(unit, bi_index));
                rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                BCMDNX_IF_ERR_EXIT(rc);

                    if (is_valid) {
                        rc = soc_interrupt_storm_timed_period_set(unit, inter, value);
                        BCMDNX_IF_ERR_EXIT(rc);
                        (interrupts[inter].storm_detection_start_time)[bi->number] = 0x0;
                    }
                }
            }
        } else {
            /* Set per interrupt */
                rc = soc_interrupt_storm_timed_period_set(unit, type.event_id, value);
                BCMDNX_IF_ERR_EXIT(rc);
               (interrupts[type.event_id].storm_detection_start_time)[type.index] = 0;
        }
        break;

    case bcmSwitchEventStormNominal:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            if (value>0) {
                SOC_SWITCH_EVENT_NOMINAL_STORM(unit) = value;
                /*Warm boot buffer updating*/
                rc = SOC_DFE_WARM_BOOT_VAR_SET(unit, INTR_STORM_NOMINAL, &value);
                SOCDNX_IF_ERR_EXIT(rc);
            } else if (value == 0) {
                    SOC_SWITCH_EVENT_NOMINAL_STORM(unit) = 0;
                    /*Warm boot buffer updating*/
                    rc = SOC_DFE_WARM_BOOT_VAR_SET(unit, INTR_STORM_NOMINAL, &value);
                    SOCDNX_IF_ERR_EXIT(rc);
                    for (inter = 0; inter < nof_interrupts; inter++) {
                        for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++ ) {

                            bi = &(SOC_BLOCK_INFO(unit, bi_index));
                            rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                            BCMDNX_IF_ERR_EXIT(rc);

                            if (is_valid) {
                                 (interrupts[inter].storm_nominal_count)[bi->number] = 0x0;
                                 (interrupts[inter].storm_detection_occurrences)[bi->number] = 0x0;
                            }
                        }
                    }

            }
        } 
        else {
            /* Set per interrupt */
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid nominal storm detection parameter (event_id)")));
        }
        break;

    case bcmSwitchEventStat:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            if (value) {
                for (inter = 0; inter < nof_interrupts; inter++) {
                    for (bi_index = 0;  SOC_BLOCK_INFO(unit, bi_index).type >= 0; bi_index++ ) {

                        bi = &(SOC_BLOCK_INFO(unit, bi_index));
                        rc = soc_interrupt_is_valid(unit, bi, &(interrupts[inter]), &is_valid);
                        BCMDNX_IF_ERR_EXIT(rc);

                        if (is_valid) {
                             (interrupts[inter].statistics_count)[bi->number] = 0x0;
                        }
                    }
                }
            } else {
                BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid statistics parameter (value")));
            }
        } else {
           (interrupts[type.event_id].statistics_count)[type.index] = 0x0;
        }
        break;

    case bcmSwitchEventLog:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            
            uint32 flags;            

            for (inter = 0; inter < nof_interrupts; inter++) {
            
                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                if (value == 0) {
                    SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
                } else {
                    SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
                }
                
                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);
            }
        } else {
        
            uint32 flags; 

            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            /* Set per interrupt */
            if (value == 0) {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
            } else {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE);
            }
            
            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case bcmSwitchEventCorrActOverride:
        /* Value - 0 : Only bcm callback will be called for this interrupt.
           Value - 1 : Only user callback will be called for this interrupt. At this mode BCM driver will only print the interrupt information for logging.
           Value - 2 : User call back will be called immediately after bcm callback. */
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
        
            uint32 flags;

            for (inter = 0; inter < nof_interrupts; inter++) {
            
                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                if (value == 0) {
                    SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
                } else {
                    SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
                }
                
                if(SOC_IS_FE3200(unit)) {
                    if (value == 2) {
                        flags |= SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
                    } else {
                        flags &= ~SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
                    }
                }

                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);

            }
        } else {
        
            uint32 flags;
            
            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            /* Set per interrupt */
            if (value == 0) {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
            } else {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE);
            }

            if(SOC_IS_FE3200(unit)) {
                if (value == 2) {
                    flags |= SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
                } else {
                    flags &= ~SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB;
                }
            }
            
            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case bcmSwitchEventPriority:
    
        if ( value > SOC_INTERRUPT_DB_FLAGS_PRIORITY_MAX_VAL) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Priority value is out af range")));
        }
    
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
        
            uint32 flags;

            for (inter = 0; inter < nof_interrupts; inter++) {
            
                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);            

                SHR_BITCOPY_RANGE( &flags, SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB, &value, 0, SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LEN); 
                
                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);
                
            }
        } else {
        
            uint32 flags;
            
            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            SHR_BITCOPY_RANGE( &flags, SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB, &value, 0, SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LEN); 
            
            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);

        }
        break;

    case bcmSwitchEventUnmaskAndClearDisable:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {

            uint32 flags;

            for (inter = 0; inter < nof_interrupts; inter++) {

                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                if (value == 0) {
                    SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
                } else {
                    SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
                }

                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);

            }
        } else {

            uint32 flags;
            /* Set per interrupt */
            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            if (value == 0) {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
            } else {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS);
            }

            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;
    case bcmSwitchEventForceUnmask:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {

            uint32 flags;

            for (inter = 0; inter < nof_interrupts; inter++) {

                rc = soc_interrupt_flags_get(unit, inter, &flags);
                BCMDNX_IF_ERR_EXIT(rc);

                if (value == 0) {
                    SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
                } else {
                    SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
                }

                rc = soc_interrupt_flags_set(unit, inter, flags);
                BCMDNX_IF_ERR_EXIT(rc);

            }
        } else {

            uint32 flags;
            /* Set per interrupt */
            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            if (value == 0) {
                SHR_BITCLR(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
            } else {
                SHR_BITSET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS);
            }

            rc = soc_interrupt_flags_set(unit, type.event_id, flags);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported control")));
        break;
    }

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

/*
 * Function:
 *    bcm_dfe_switch_event_control_get
 * Description:
 *    Get event control
 * Parameters:
 *    unit        - Device unit number
 *  type        - Event action.
 *  value       - Event value
 * Returns:
 *      BCM_E_xxx
 */
int bcm_dfe_switch_event_control_get(
    int unit, 
    bcm_switch_event_t switch_event,
    bcm_switch_event_control_t type, 
    uint32 *value)
{
    int rc = BCM_E_NONE, nof_interrupts;
    soc_interrupt_db_t* interrupts;
    int inter_get;
    int is_enable;
    soc_block_types_t block_types;
    int is_valid;
    int enable;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(value);
     DFE_UNIT_LOCK_TAKE(unit); 

    if (switch_event != BCM_SWITCH_EVENT_DEVICE_INTERRUPT ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_SOC_MSG("supports only interrupts event")));
    }

    if(!SOC_INTR_IS_SUPPORTED(unit)) {
        BCMDNX_ERR_EXIT_MSG(SOC_E_UNAVAIL, (_BSL_SOC_MSG("No interrupts for device"))); 
    }

    interrupts = SOC_CONTROL(unit)->interrupts_info->interrupt_db_info;

    rc = soc_nof_interrupts(unit, &nof_interrupts);
    BCMDNX_IF_ERR_EXIT(rc);    

    if (type.event_id != BCM_SWITCH_EVENT_CONTROL_ALL) {
        if(type.event_id >= nof_interrupts || type.event_id<0) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid interrupt")));
        }

        /*verify block instance*/
        if(!SOC_REG_IS_VALID(unit, interrupts[type.event_id].reg)){
            BCMDNX_ERR_EXIT_MSG(SOC_E_INTERNAL, (_BSL_SOC_MSG("Invalid interrupt register for the device")));
        }
        block_types = SOC_REG_INFO(unit, interrupts[type.event_id].reg).block;
        rc = soc_is_valid_block_instance(unit, block_types, type.index, &is_valid);
        BCMDNX_IF_ERR_EXIT(rc);
        if (!is_valid) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported block instance")));
        }
    }

    LOG_VERBOSE(BSL_LS_BCM_INTR,
                (BSL_META_U(unit,
                            "%s(): interrupt id=%3d, name=%s\n"), FUNCTION_NAME(), type.event_id, interrupts[type.event_id].name));

    switch(type.action) {
    case bcmSwitchEventMask:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_interrupt_all_enable_get, (unit, &enable));
            BCMDNX_IF_ERR_EXIT(rc);
            *value = enable ? 0 : 1;
        } else {
            /* Get per interrupt */
            rc = soc_interrupt_is_enabled(unit, type.index, &(interrupts[type.event_id]), &is_enable);
            BCMDNX_IF_ERR_EXIT(rc);
            *value = (is_enable == 0);
        }
        break;

    case bcmSwitchEventForce:
        /* Set/clear per interrupt */
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid <controll all> event parameter for force option")));
        } else {
            rc = soc_interrupt_force_get(unit, type.index, &(interrupts[type.event_id]), &is_enable);
            BCMDNX_IF_ERR_EXIT(rc);
            *value = is_enable;
        }
        break;

    case bcmSwitchEventClear:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            rc = soc_interrupt_is_all_clear(unit, (int *)value);
            BCMDNX_IF_ERR_EXIT(rc);
        } else {
            /* Get per interrupt */
            rc = soc_interrupt_get(unit, type.index, &(interrupts[type.event_id]), &inter_get);
            BCMDNX_IF_ERR_EXIT(rc);
            *value = (inter_get == 0);
        }
        break;

    case bcmSwitchEventRead:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid read parameter (event_id)")));
        } else {
            /* Get per interrupt */
            rc = soc_interrupt_get(unit, type.index, &(interrupts[type.event_id]), &inter_get);
            BCMDNX_IF_ERR_EXIT(rc);
            *value = (inter_get == 1);
        }
        break;

    case bcmSwitchEventStormTimedPeriod:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Storm Timed Period parameter (event_id)")));
        } else {
            /* Get per interrupt */            
            rc =  soc_interrupt_storm_timed_period_get(unit, type.event_id, value);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case bcmSwitchEventStormTimedCount:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Storm Timed Count parameter (event_id)")));
        } else {
            /* Get per interrupt */
            rc =  soc_interrupt_storm_timed_count_get(unit, type.event_id, value);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        break;

    case bcmSwitchEventStormNominal:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            *value = SOC_SWITCH_EVENT_NOMINAL_STORM(unit);
        } else {
            /* Get per interrupt */
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid nominal storm parameter (event_id)")));
        }
        break;

    case bcmSwitchEventStat:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Statistics Control parameter (event_id)")));
        } else {
            /* Get per interrupt */
            *value = (interrupts[type.event_id].statistics_count)[type.index];
        }
        break;

    case bcmSwitchEventLog:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {
             uint32 flags;
            /* Get per interrupt */
            rc =  soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            *value =  (SHR_BITGET(&flags , SOC_INTERRUPT_DB_FLAGS_PRINT_ENABLE)!= 0) ? 0x1 : 0x0;
         }       
        break;

    case bcmSwitchEventCorrActOverride:
    /* Value - 0 : Only bcm callback will be called for this interrupt.
       Value - 1 : Only user callback will be called for this interrupt. At this mode BCM driver will only print the interrupt information for logging.
       Value - 2 : User call back will be called immediately after bcm callback. */
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {
            /* Get per interrupt */
            uint32 flags;

            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            if(SOC_IS_FE3200(unit)) {
                if (flags & SOC_INTERRUPT_DB_FLAGS_BCM_AND_USR_CB) {
                    *value = 0x2;
                    break;
                } 
            }
            *value = (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_CORR_ACT_OVERRIDE_ENABLE) != 0) ? 0x1 : 0x0;
        }     
        break;

    case bcmSwitchEventPriority:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
            BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {
            uint32 flags;
            /* Get per interrupt */
            rc =  soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);
            
            *value = ( (flags & SOC_INTERRUPT_DB_FLAGS_PRIORITY_MASK) >> SOC_INTERRUPT_DB_FLAGS_PRIORITY_BITS_LSB);
        }
        break;

    case bcmSwitchEventUnmaskAndClearDisable:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {

            uint32 flags;
            /* Get per interrupt */

            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            *value =  (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE_BITS) != 0) ? 0x1 : 0x0;
        }
        break;
    case bcmSwitchEventForceUnmask:
        if (type.event_id == BCM_SWITCH_EVENT_CONTROL_ALL) {
           BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Invalid Print Control parameter (event_id)")));
        } else {

            uint32 flags;
            /* Get per interrupt */

            rc = soc_interrupt_flags_get(unit, type.event_id, &flags);
            BCMDNX_IF_ERR_EXIT(rc);

            *value =  (SHR_BITGET(&flags, SOC_INTERRUPT_DB_FLAGS_FORCE_UNMASK_BITS) != 0) ? 0x1 : 0x0;
        }
        break;

    default:
        BCMDNX_ERR_EXIT_MSG(SOC_E_PARAM, (_BSL_SOC_MSG("Unsupported control")));
        break;
    }

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define _BCM_SYNC_SUCCESS(unit) \
  (BCM_SUCCESS(rv) || (BCM_E_INIT == rv) || (BCM_E_UNAVAIL == rv))


STATIC int _dfe_switch_control_sync(int unit, int arg)
{
    int rv = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rv = soc_dfe_warm_boot_sync(unit);
    BCMDNX_IF_ERR_EXIT(rv);

    /* now commit the scache to persistent storage*/
    rv = soc_scache_commit(unit);    

    /* Mark scache as clean */
    SOC_CONTROL(unit)->scache_dirty = 0;   

    BCMDNX_IF_ERR_EXIT(rv);

exit: 
    BCMDNX_FUNC_RETURN; 
}
#endif /* BCM_WARM_BOOT_SUPPORT */



/*
 * Function:
 *      bcm_dfe_switch_control_get
 * Purpose:
 *      Configure port-specific and device-wide operating modes.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - (IN) <UNDEF>
 *      arg - (OUT) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_dfe_switch_control_get(int unit, bcm_switch_control_t bcm_type, int *arg)
{
    BCMDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE(unit);

    switch (bcm_type) {

#ifdef BCM_WARM_BOOT_SUPPORT
    case bcmSwitchStableSelect:
    {
        uint32 flags = 0;

        BCMDNX_IF_ERR_EXIT(soc_stable_get(unit, arg, &flags));
    }
    break;
#endif
    case bcmSwitchControlAutoSync:
        BCM_ERR_EXIT_NO_MSG(BCM_E_UNAVAIL);
        break;
    case bcmSwitchSynchronousPortClockSource:  
        BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_sync_e_link_get, (unit, 1, arg)));
        break;
    case bcmSwitchSynchronousPortClockSourceBkup:
        BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_sync_e_link_get, (unit, 0, arg)));
        break;
    case bcmSwitchSynchronousPortClockSourceDivCtrl: 
        BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_sync_e_divider_get, (unit, arg)));
        break;

    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_SOC_MSG("Unsupported type %d"), bcm_type));
    }

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}   

/*
 * Function:
 *      bcm_dfe_switch_control_set
 * Purpose:
 *      Configure port-specific and device-wide operating modes.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - (IN) <UNDEF>
 *      arg - (IN) <UNDEF>
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int bcm_dfe_switch_control_set(int unit, bcm_switch_control_t bcm_type, int arg)
{
    int rv;
#if defined(BCM_WARM_BOOT_SUPPORT)
    int stable_select;
#endif

    BCMDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE(unit);

    rv = BCM_E_NONE;

    switch (bcm_type) {

#if defined(BCM_WARM_BOOT_SUPPORT)
    case bcmSwitchControlSync:
        if (!SOC_WARM_BOOT(unit) && !SOC_AUTOSYNC_IS_ENABLE(unit)) {
            /* don't sync if in autosync mode or during warm reboot */
            rv = _dfe_switch_control_sync(unit, arg);
            BCMDNX_IF_ERR_EXIT(rv);
        }
        break;
    case bcmSwitchControlAutoSync:
        BCMDNX_IF_ERR_EXIT(bcm_dfe_switch_control_get(unit,
                                                    bcmSwitchStableSelect, &stable_select));
        if (BCM_SWITCH_STABLE_NONE != stable_select) {
            SOC_CONTROL_LOCK(unit);
            arg = arg ? 1 : 0;

            /* perform a one time sync to make sure we start autosyncying when everything is in sync */
            /* important - need to call the sync before enabling the autosync flag */
            rv = _dfe_switch_control_sync(unit, arg);
            BCMDNX_IF_ERR_EXIT(rv);

            /* this flag is a common soc_control flag that may be used by common modules */
            /* it is not recovered in wb, need to reconfigure after init */
            SOC_CONTROL(unit)->autosync = arg;

            SOC_CONTROL_UNLOCK(unit);
        } else {
            rv = BCM_E_NOT_FOUND;
        }
        break;
#endif
    case bcmSwitchCacheTableUpdateAll:
        rv = soc_sand_cache_table_update_all(unit);
        BCMDNX_IF_ERR_EXIT(rv);
        break;
    case bcmSwitchSynchronousPortClockSource: /* update master link for synce */
        BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_sync_e_link_set, (unit, 1, arg)));
        break;
    case bcmSwitchSynchronousPortClockSourceBkup: /* update slave link for synce */
        BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_sync_e_link_set, (unit, 0, arg)));
        break;
    case bcmSwitchSynchronousPortClockSourceDivCtrl: /* updated desired divider */
        BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_sync_e_divider_set, (unit, arg)));
        break;

    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,(_BSL_SOC_MSG("Unsupported type")));
    }

exit: 
   DFE_UNIT_LOCK_RELEASE(unit);
   BCMDNX_FUNC_RETURN; 
}

int 
bcm_dfe_switch_temperature_monitor_get(
    int unit, 
    int temperature_max, 
    bcm_switch_temperature_monitor_t *temperature_array, 
    int *temperature_count)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(temperature_array);
    BCMDNX_NULL_CHECK(temperature_count);
    if (temperature_max <= 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_SOC_MSG("temperature_max expected to be bigger than 0.\n")));
    }
    DFE_UNIT_LOCK_TAKE(unit);

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_temperature_monitor_get, (unit, temperature_max, temperature_array, temperature_count));
    BCMDNX_IF_ERR_EXIT(rv);

exit: 
   DFE_UNIT_LOCK_RELEASE(unit);
   BCMDNX_FUNC_RETURN; 
}

int bcm_dfe_switch_rov_get(
    int unit,
    uint32 flags,
    uint32 *rov){

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(rov);

    BCMDNX_IF_ERR_EXIT(soc_dfe_avs_value_get(unit, rov));

exit:
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

