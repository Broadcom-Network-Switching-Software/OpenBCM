/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RX LOS
 */
#include <shared/bsl.h>
#if (defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT))
#include <appl/dcmn/rx_los/rx_los.h>
#include <appl/dcmn/rx_los/rx_los_db.h>
#include <appl/dcmn/interrupts/interrupt_handler.h>
#endif /* BCM_PETRA_SUPPORT || BCM_DFE_SUPPORT */
#include <sal/appl/io.h>
#include <bcm/debug.h>
#include <bcm_int/common/debug.h>
#include <soc/debug.h>
#include <bcm/port.h>
#include <sal/core/time.h>
#include <sal/core/sync.h>
#include <sal/core/spl.h>
#include <soc/intr.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <bcm/error.h>
#include <soc/maxdef.h>
#include <bcm/switch.h>
#include <soc/intr.h>
#include <bcm_int/control.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>

#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/ARAD/arad_serdes.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_interrupts.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/mbcm.h>
#endif

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/fe1600/fe1600_fabric_links.h>
#include <soc/dfe/fe1600/fe1600_interrupts.h>
#endif

/*local defines*/
#define RX_LOS_SHORT_SLEEP_ONE_FACTOR       (10)

/*detach/attach unit requitres both locks: rx_los_lock, rx_los_lock_per_unit*/
/*read/write to unit requires rx_los_lock_per_unit only*/
static sal_mutex_t rx_los_lock = NULL;
static sal_mutex_t rx_los_lock_per_unit[SOC_MAX_NUM_DEVICES] = {0};
static sal_sem_t rx_los_sem = NULL;
static int exit_thread = 0;
static int rx_los_thread_exist = 0;

int rx_loss_appl_enable(int unit)
{
    return (rx_los_db_unit_null_check(unit) == BCM_E_NONE);
}

int rx_los_wake_up(sal_usecs_t timestamp,uint32 wait_time){
    sal_usecs_t curr_time;
    int timediff;
    
    curr_time = sal_time_usecs();

    if (curr_time>timestamp) {
        timediff = curr_time-timestamp;
    } else {
        timediff = ((uint32)(-1)) - timestamp;
        timediff += curr_time;
    }

    return (timediff > wait_time);
}

/*
 * This function main role is to identify recurring interrupt according to unit, port and state,
 */
int
rx_los_recurring_get(int unit, bcm_port_t port, int *is_recurring)
{
    int rv;
    sal_usecs_t curr_time;
    sal_usecs_t cycle_start, cycle_time;
    uint32 cycle_count, cycle_max_count;
    int timediff;
    rx_los_state_t state;

    curr_time = sal_time_usecs();
    state = rx_los_db_state_get(unit, port);

    /*time diff calc*/
    if (state == rx_los_state_ideal_state) {
        rv = rx_los_db_link_down_cycle_get(unit, port, &cycle_start, &cycle_count);
    } else {
        rv = rx_los_db_rx_seq_change_cycle_get(unit, port, &cycle_start, &cycle_count);
    }
    BCM_FUNC_IF_ERR(rv) {
        return rv;
    }

    if (curr_time>cycle_start) {
        timediff = curr_time-cycle_start;
    } else {
        timediff = ((uint32)(-1)) - cycle_start;
        timediff += curr_time;
    }

    /*check if there is a storm*/
    if (state == rx_los_state_ideal_state) {
        rv = rx_los_db_link_down_cycle_config_get(unit, port, &cycle_time, &cycle_max_count);
    } else {
        rv = rx_los_db_rx_seq_change_cycle_config_get(unit, port, &cycle_time, &cycle_max_count);
    }
    BCM_FUNC_IF_ERR(rv) {
        return rv;
    }

    if (timediff > cycle_time) {
        cycle_count = 1;
        cycle_start = curr_time;
    } else {
        cycle_count++;
    }

    *is_recurring = (cycle_count >= cycle_max_count);

    /*update link_down values*/
    if (state == rx_los_state_ideal_state) {
        rv = rx_los_db_link_down_cycle_set(unit, port, cycle_start, cycle_count);
    } else {
        rv = rx_los_db_rx_seq_change_cycle_set(unit, port, cycle_start, cycle_count);
    }
    BCM_FUNC_IF_ERR(rv) {
        return rv;
    }

    return BCM_E_NONE;
}

int
rx_los_rx_seq_done_get(int unit, bcm_port_t port, uint32 *rx_seq_done)
{
    int rv;


    if (IS_SFI_PORT(unit, port)) {
        rv = bcm_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_RX_SEQ_DONE, rx_seq_done);
        BCM_FUNC_IF_ERR(rv) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_rx_seq_done_get - bcm_port_phy_control_get FAILED\n")));
            return rv;
        }
    } else if (SOC_IS_ARAD(unit)) {
#ifdef BCM_PETRA_SUPPORT
        uint32 seq_done;
        uint32 phy_port;
        bcm_port_if_t interface_type;
        uint32 reg32_val;

        rv = soc_port_sw_db_interface_type_get(unit, port, &interface_type);
        BCM_FUNC_IF_ERR(rv) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_rx_seq_done_get - soc_port_sw_db_interface_type_get FAILED\n")));
            return rv;
        }
        if (interface_type != BCM_PORT_IF_CAUI) {
            rv = bcm_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_RX_SEQ_DONE, rx_seq_done);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_rx_seq_done_get - bcm_port_phy_control_get FAILED\n")));
                return rv;
            }
        } else {
            /*mld swap consideration*/
            rv = soc_port_sw_db_first_phy_port_get(unit, port, &phy_port);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_rx_seq_done_get - soc_port_sw_db_first_phy_port_get FAILED\n")));
                return rv;
            }

            seq_done = 0;
            rv = READ_NBI_NIF_WC_RX_SEQ_DONEr(unit, &reg32_val);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "Failed to read NBI_NIF_WC_RX_SEQ_DONE register\n")));
                return rv;
            }

            /*checking the first core*/
            SHR_BITCOPY_RANGE(&seq_done, 0, &reg32_val, phy_port-1, 4);
            *rx_seq_done = seq_done ? 1 : 0;
        }
#endif /*BCM_PETRA_SUPPORT*/
    }
    return BCM_E_NONE;
}

int
rx_los_mac_rxlostofsync_int_mask_enable(int unit, int port, int enable)
{
    int rv;
    bcm_switch_event_control_t type;
    int interrupt_id = 0, parent_interrupt_id = 0;
    int block_instance = 0;
    int link;

    link = port;

    /*get block instance and interrupt id:*/

    if (SOC_IS_ARAD(unit)) {
#ifdef BCM_PETRA_SUPPORT
        /* get link number: */
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        block_instance = link / 4;
        interrupt_id = link % 4 + ARAD_INT_FMAC_RXLOSTOFSYNC_0;
        parent_interrupt_id = ARAD_INT_FMAC_INTREG2;
#endif
    } else if (SOC_IS_DFE(unit)) {
#ifdef BCM_DFE_SUPPORT
        block_instance = link / 4;
        interrupt_id = link % 4 + FE1600_INT_MAC_RXLOSTOFSYNC_0;
        parent_interrupt_id = FE1600_INT_MAC_INTREG2;
#endif
    }


    type.event_id = interrupt_id;
    type.index = block_instance;
    type.action = bcmSwitchEventMask;
    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, enable ? 0 : 1);
    BCM_FUNC_IF_ERR(rv) {
        return rv;
    }

    if (enable) {
        type.event_id = parent_interrupt_id;
        type.index = block_instance;
        type.action = bcmSwitchEventMask;
        rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0);
        BCM_FUNC_IF_ERR(rv) {
            return rv;
        }
    }

    return rv;
}

int
rx_los_nif_link_status_interrupt_clear(int unit, int port){
    int rv;
    bcm_switch_event_control_t type;
    int interrupt_id1 = 0;
    int block_instance1 = 0;
#ifdef BCM_PETRA_SUPPORT
    uint32 offset;
    soc_port_if_t interface_type;
#endif

    /*get block instance and interrupt id:*/

    if (SOC_IS_ARAD(unit)) {
#ifdef BCM_PETRA_SUPPORT
        uint32 phy_port;

        /*nif port*/
        rv = soc_port_sw_db_first_phy_port_get(unit, port, &phy_port);
        BCM_FUNC_IF_ERR(rv) {
            return rv;
        }
		rv = soc_port_sw_db_interface_type_get(unit,port,&interface_type);
		BCM_FUNC_IF_ERR(rv)
		{
			return rv;
		}
		if (interface_type != SOC_PORT_IF_ILKN)
		{
			interrupt_id1 = ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_0 + (phy_port - 1 /*1 base to 0 base*/);
		}
		else /* ilkn port */
		{
            rv=soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset);
			BCM_FUNC_IF_ERR(rv) {
		        return rv;
	        }
			if (offset == 0)
			{
				interrupt_id1 = ARAD_INT_NBI_ILKNRXPORT0STATUSCHANGEINT;
			}
			else if (offset == 1)
			{
				interrupt_id1 = ARAD_INT_NBI_ILKNRXPORT1STATUSCHANGEINT;
			}
				
		}
        
        block_instance1 = 0;

#endif
    } else {
        return BCM_E_UNAVAIL;    
    }


    type.event_id = interrupt_id1;
    type.index = block_instance1;
    type.action = bcmSwitchEventClear;
    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 1);
    BCM_FUNC_IF_ERR(rv) {
        return rv;
    }

    return rv;
}

int
rx_los_nif_link_status_change_mask_enable(int unit, int port, int enable){
    int rv;
    bcm_switch_event_control_t type;
    int interrupt_id = 0, parent_interrupt_id = 0;
    int block_instance = 0;
#ifdef BCM_PETRA_SUPPORT
    soc_port_if_t interface_type;
    uint32 offset;
#endif

    /*get block instance and interrupt id:*/

    if (SOC_IS_ARAD(unit)) {
#ifdef BCM_PETRA_SUPPORT
        uint32 phy_port;

        /*nif port*/
        rv = soc_port_sw_db_first_phy_port_get(unit, port, &phy_port);
        BCM_FUNC_IF_ERR(rv) {
            return rv;
        }
		rv= soc_port_sw_db_interface_type_get(unit,port,&interface_type);
		BCM_FUNC_IF_ERR(rv)
		{
			return rv;
		}

		if (interface_type != SOC_PORT_IF_ILKN) /* not ilkn port*/
		{
			interrupt_id = ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_0 + (phy_port - 1 /*1 base to 0 base*/);
			parent_interrupt_id = ARAD_INT_NBI_LINKSTATUSCHANGEINT;
			block_instance = 0;
		}
		else /* ilkn port */
		{
			rv=soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset);
			BCM_FUNC_IF_ERR(rv) {
				return rv;
			}
			if (offset == 0)
			{
				interrupt_id = ARAD_INT_NBI_ILKNRXPORT0STATUSCHANGEINT;
			}
			else if (offset == 1)
			{
				interrupt_id = ARAD_INT_NBI_ILKNRXPORT1STATUSCHANGEINT;	
			}

			parent_interrupt_id = ARAD_INT_NBI_ILKNINT;
			block_instance = 0;
		}

#endif
    } else {
        return BCM_E_UNAVAIL;    
    }


    type.event_id = interrupt_id;
    type.index = block_instance;
    type.action = bcmSwitchEventMask;
    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, enable ? 0 : 1);
    BCM_FUNC_IF_ERR(rv) {
        return rv;
    }

    if (enable) {
        type.event_id = parent_interrupt_id;
        type.index = block_instance;
        type.action = bcmSwitchEventMask;
        rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0);
        BCM_FUNC_IF_ERR(rv) {
            return rv;
        }
    }

    return rv;
}

int rx_los_fsrd_syncstatuschanged_int_mask_enable(int unit, int port,int enable){
    int rv;
    int interrupt_id = 0, parent_interrupt_id = 0;
    int block_instance = 0;
    int link;
    bcm_switch_event_control_t type;

    link = port;

    /*get block instance and interrupt id:*/

    if (SOC_IS_ARAD(unit)) {
#ifdef BCM_PETRA_SUPPORT

        /* get link number: */
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        block_instance = link / 12;
        interrupt_id = link % 12 + ARAD_INT_FSRD_FSRD_0_SYNCSTATUSCHANGED_0;
        parent_interrupt_id = ((link % 12) / 4) + ARAD_INT_FSRD_INTREG0; 


#endif
    } else if (SOC_IS_DFE(unit)) {
#ifdef BCM_DFE_SUPPORT

        block_instance = link / 16;
        interrupt_id = link % 16 + FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0;
        parent_interrupt_id = ((link % 16) / 4) + FE1600_INT_SRD_INTREG0;
#endif
    }
   
    type.event_id = interrupt_id;
    type.index = block_instance;
    type.action = bcmSwitchEventMask;
    rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, enable ? 0 : 1);
    BCM_FUNC_IF_ERR(rv) {
        return rv;
    }

    if (enable) {
        type.event_id = parent_interrupt_id;
        type.index = block_instance;
        type.action = bcmSwitchEventMask;
        rv = bcm_switch_event_control_set(unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, type, 0 );
        BCM_FUNC_IF_ERR(rv) {
            return rv;
        }
    }

    return rv;
}


/*
 * Function:    
 *      rx_los_port_enable
 * Purpose:
 *      enable/ disable ports for rx_los feature.
 * Parameters:  
 *      unit - unit numbrt
 *      port - port number
 *      enable - 1 for enable, 0 for disable
 * Returns:
 *      SOC_E_XXX
 * NOTE:
 */
int rx_los_port_enable(int unit, bcm_port_t port, int enable, int warm_boot) {
    return rx_los_generic_port_enable(unit, port, enable, warm_boot, 0);
}

/*
 * Function:    
 *      rx_los_generic_port_enable
 * Purpose:
 *      enable/ disable ports for rx_los feature.
 *      Support SW and SW&HW disable of the port.
 *      SW disable remove port from data base and avoid from access to the device.
 * Parameters:  
 *      unit - unit numbrt
 *      port - port number
 *      enable - 1 for enable, 0 for disable
 *      flags - supported flags: RX_LOS_GENERIC_PORT_ENABLE_F_SW_ONLY
 * Returns:
 *      SOC_E_XXX
 * NOTE:
 */
int rx_los_generic_port_enable(int unit, bcm_port_t port, int enable, int warm_boot, uint32 flags) {

    int rv;
    int link =0;
    int interrupt_id1 = -1, interrupt_id2 = -1;
    uint32 intr_flags;
    bcm_pbmp_t pbmp_supported;
#ifdef BCM_PETRA_SUPPORT
    soc_port_if_t interface_type;
    uint32 offset;
#endif
    int sw_disable_only = 0;
    bcm_switch_event_control_t event_bcm_switch_event_print;
    int index1 = 0, index2 = 0;

    if (!enable)
    {
        sw_disable_only = (flags & RX_LOS_GENERIC_PORT_ENABLE_F_SW_ONLY ? 1 : 0);
    }

    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_port_enable - invalid unit\n")));
        return BCM_E_UNIT;
    }

    if (!SOC_PORT_VALID(unit, port)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_port_enable - invalid port\n")));
        return BCM_E_PORT;
    }

    if (rx_los_lock_per_unit[unit] == NULL) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_port_enable - rx los app uninitialized \n")));
        return BCM_E_FAIL;
    }

    rv = sal_mutex_take(rx_los_lock_per_unit[unit], sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_port_enable - sal_mutex_take FAILED\n")));
        return BCM_E_FAIL;
    }

    rv = rx_los_db_unit_null_check(unit);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_port_enable - rx los app uninitialized \n")));
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return rv;
    }

    /*check if supported*/
    RX_LOS_PBMP_DEFAULT(unit, pbmp_supported);
    if (!BCM_PBMP_MEMBER(pbmp_supported, port)) {
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return BCM_E_PORT;
    }

    if (enable) {
        
        /* 
         * Manage interrupt in rx_los application -
         *      Interrupt thread should not clear or unmask this interrupts
         *      cancel interrupt thread related printing
         */

        if (SOC_IS_ARAD(unit)) {
#ifdef BCM_PETRA_SUPPORT
            if (IS_SFI_PORT(unit, port)) {
                link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
                interrupt_id1 = link % 4 + ARAD_INT_FMAC_RXLOSTOFSYNC_0;
                index1 = link/4;
                interrupt_id2 = link % 12 + ARAD_INT_FSRD_FSRD_0_SYNCSTATUSCHANGED_0;
                index2 = link/12;
            } else {
                uint32 phy_port;
                

                /*nif port*/
                rv = soc_port_sw_db_first_phy_port_get(unit, port, &phy_port);
                BCM_FUNC_IF_ERR(rv) {
                    LOG_ERROR(BSL_LS_APPL_PORT,
                              (BSL_META_U(unit,
                                          "ERROR: rx_los_port_enable - soc_port_sw_db_first_phy_port_get FAILED\n")));
                    sal_mutex_give(rx_los_lock_per_unit[unit]);
                    return BCM_E_FAIL;
                }

				rv= soc_port_sw_db_interface_type_get(unit,port,&interface_type);
				BCM_FUNC_IF_ERR(rv)
				{
				    sal_mutex_give(rx_los_lock_per_unit[unit]);
					return rv;
				}
				if (interface_type != SOC_PORT_IF_ILKN)
				{
					interrupt_id1 = ARAD_INT_NBI_LINKSTATUSCHANGEINTERRUPTREGISTER_0 + (phy_port - 1 /*1 base to 0 base*/);
				}
				else /* ilkn port */
				{
				    rv=soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset);
			        BCM_FUNC_IF_ERR(rv) {
                        sal_mutex_give(rx_los_lock_per_unit[unit]);
				        return rv;
			        }
					if (offset == 0)
					{
						interrupt_id1 = ARAD_INT_NBI_ILKNRXPORT0STATUSCHANGEINT;
					}
					else if (offset == 1)
					{
						interrupt_id1 = ARAD_INT_NBI_ILKNRXPORT1STATUSCHANGEINT;	
					}
				}
                index1 = 0;
                interrupt_id2 = -1;
                index2 = 0;
            }
#endif

        } else if (SOC_IS_DFE(unit)) {
#ifdef BCM_DFE_SUPPORT
            link = port;
            interrupt_id1 = link % 4 + FE1600_INT_MAC_RXLOSTOFSYNC_0;
            index1 = link / 4;
            interrupt_id2 = link % 16 + FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0;
            index2 = link / 16;
#endif
        }
        if (interrupt_id1 != -1) {
            
            rv = soc_interrupt_flags_get(unit, interrupt_id1, &intr_flags);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_port_enable - soc_interrupt_flags_get FAILED\n")));
                sal_mutex_give(rx_los_lock_per_unit[unit]);
                return rv;
            }
            intr_flags |= SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE;
            rv = soc_interrupt_flags_set(unit, interrupt_id1, intr_flags);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_port_enable - soc_interrupt_flags_set FAILED\n")));
                sal_mutex_give(rx_los_lock_per_unit[unit]);
                return rv;
            }

            if (!warm_boot) {
                /*cancel prints*/
                event_bcm_switch_event_print.event_id = interrupt_id1;
                event_bcm_switch_event_print.index = index1;
                event_bcm_switch_event_print.action = bcmSwitchEventLog;
                rv = bcm_switch_event_control_set( unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event_print, 0);
                BCM_FUNC_IF_ERR(rv) {
                    LOG_ERROR(BSL_LS_APPL_PORT,
                              (BSL_META_U(unit,
                                          "ERROR: rx_los_port_enable - bcm_switch_event_control_set FAILED\n")));
                    sal_mutex_give(rx_los_lock_per_unit[unit]);
                    return rv;
                }
            }
        }

        if (interrupt_id2 != -1) {
            rv = soc_interrupt_flags_get(unit, interrupt_id2, &intr_flags);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_port_enable - soc_interrupt_flags_get FAILED\n")));
                sal_mutex_give(rx_los_lock_per_unit[unit]);
                return rv;
            }
            intr_flags |= SOC_INTERRUPT_DB_FLAGS_UNMASK_AND_CLEAR_DISABLE;
            rv = soc_interrupt_flags_set(unit, interrupt_id2, intr_flags);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_port_enable - interrupt_handle_data_base_flags_set FAILED\n")));
                sal_mutex_give(rx_los_lock_per_unit[unit]);
                return rv;
            }

            if (!warm_boot) {
                event_bcm_switch_event_print.event_id = interrupt_id2;
                event_bcm_switch_event_print.index = index2;
                event_bcm_switch_event_print.action = bcmSwitchEventLog;
                rv = bcm_switch_event_control_set( unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event_print, 0);
                BCM_FUNC_IF_ERR(rv) {
                    LOG_ERROR(BSL_LS_APPL_PORT,
                              (BSL_META_U(unit,
                                          "ERROR: rx_los_port_enable - bcm_switch_event_control_set FAILED\n")));
                    sal_mutex_give(rx_los_lock_per_unit[unit]);
                    return rv;
                }
            }
        }


        /*set initial rx_los state*/
        rx_los_db_move_link_to_support(unit, port);
        rx_los_db_remove_link_from_steady(unit,port);
        if (!warm_boot) {
            rx_los_db_state_set(unit,port,rx_los_state_rx_seq_change);
        } else {
            rx_los_db_state_set(unit,port,rx_los_state_link_up_check);
        }
    } else {
        /*set unsupported state*/
        rx_los_db_remove_link_from_support(unit, port);
        rx_los_db_move_link_to_steady(unit, port);
        rx_los_db_state_set(unit, port, rx_los_state_no_signal);
    }

    /*mask relevant interrupts - initial state*/
    if (!sw_disable_only)
    {
        if (IS_SFI_PORT(unit, port)) {
            rv = rx_los_fsrd_syncstatuschanged_int_mask_enable(unit, port, 0);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_port_enable - rx_los_fsrd_syncstatuschanged_int_mask_enable FAILED\n")));
                sal_mutex_give(rx_los_lock_per_unit[unit]);
                return rv;
            }

            rv = rx_los_mac_rxlostofsync_int_mask_enable(unit, port, 0);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_port_enable - rx_los_fsrd_syncstatuschanged_int_mask_enable FAILED\n")));
                sal_mutex_give(rx_los_lock_per_unit[unit]);
                return rv;
            }
        } else {
            /*nif port*/
            rv = rx_los_nif_link_status_change_mask_enable(unit, port, 0);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                           (BSL_META_U(unit,
                                      "ERROR: rx_los_port_enable - rx_los_nif_link_status_change_mask_enable FAILED\n")));
                sal_mutex_give(rx_los_lock_per_unit[unit]);
                return rv;
            }
        }
    }

    sal_mutex_give(rx_los_lock_per_unit[unit]);
    if (enable) {
        sal_sem_give(rx_los_sem);
    }

    return BCM_E_NONE;
}

int rx_los_set_config(uint32 short_sleep_usec, uint32 long_sleep_usec, uint32 allowed_retries, uint32 priority, int link_down_count_max, int link_down_cycle_time)
{
    /* set sleeping time for default if not user defined */
    return rx_los_db_set_config((short_sleep_usec == 0)     ? RX_LOS_SHORT_SLEEP_DEFAULT            : short_sleep_usec, 
                                (long_sleep_usec == 0)      ? RX_LOS_LONG_SLEEP_DEFAULT             : long_sleep_usec,
                                (allowed_retries == 0)      ? RX_LOS_ALLOWED_RETRIES_DEFAULT        : allowed_retries,
                                (priority == 0)             ? RX_LOS_THREAD_PRIORITY_DEFAULT        : priority,
                                (link_down_count_max == 0)  ? RX_LOS_LINK_DOWN_CYCLE_COUNT_DEFAULT  : link_down_count_max,
                                (link_down_cycle_time == 0) ? RX_LOS_LINK_DOWN_CYCLE_TIME_DEFAULT   : link_down_cycle_time);

}

int rx_los_set_active_sleep_config(uint32 active_sleep_usec)
{
	return rx_los_db_set_active_sleep_config(active_sleep_usec);
}

/*
 *  Clears Rx Seq change interrupt.
 */
int rx_los_rx_seq_done_clear(int unit, int port)
{
    int rc;
    bcm_switch_event_control_t event_bcm_switch_event_clear;
    int link = 0;

    event_bcm_switch_event_clear.event_id = 0;
    event_bcm_switch_event_clear.index = 0;
    event_bcm_switch_event_clear.action = bcmSwitchEventClear;

    if (SOC_IS_DFE(unit)) {
#ifdef BCM_DFE_SUPPORT        
        link = port;

        event_bcm_switch_event_clear.event_id = FE1600_INT_SRD_FSRD_0_SYNCSTATUSCHANGED_0 + (link % 16);
        event_bcm_switch_event_clear.index = link / 16;
#endif
    } else if (SOC_IS_ARAD(unit)) {

#ifdef BCM_PETRA_SUPPORT
        link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
        event_bcm_switch_event_clear.event_id = ARAD_INT_FSRD_FSRD_0_SYNCSTATUSCHANGED_0 + (link%12);
        event_bcm_switch_event_clear.index = link / 12;
#endif
    }

    rc = bcm_switch_event_control_set( unit, BCM_SWITCH_EVENT_DEVICE_INTERRUPT, event_bcm_switch_event_clear, 1);
    
    return rc;
    
}

void rx_los_main_thread(uint32 allowed_retries, uint32 short_sleep_usec, uint32 long_sleep_usec, uint32 active_sleep_usec_input) {    
    int unit, port, wake_up, rv,
        all_steady,         /* (all_steady == 1) indicates all ports in system are in a steady state  */
        no_sleep,           /* (no_sleep == 1) indicates that some port is in a state that requires immediate handling */
        short_sleep_1,      /* (short_sleep_1 == 1) indicates that some port is in a short_sleep_one state */
        short_sleep_2,      /* (short_sleep_2 == 1) indicates that some port is in a short_sleep_two state */
        active_sleep ,      /* (active_sleep == 1) indicates that some port is in a active_sleep state */
        link_is_up;
    uint32 rx_seq_done;
    pbmp_t units_pbmp, not_steady_state, pbmp_enabled;
    rx_los_state_t link_state;
    uint32 short_sleep_1_usec, short_sleep_2_usec;
    int notify_callback;    /*indicates if the currnet port state requires call to rx los callback*/
    uint32 active_sleep_usec;

#ifdef BCM_PETRA_SUPPORT
    bcm_port_mapping_info_t mapping_info;
    bcm_port_interface_info_t interface_info;
    uint32 flags;
#endif

    exit_thread = 0;
    rx_los_thread_exist = 1;
    active_sleep_usec = ((active_sleep_usec_input == 0) ? long_sleep_usec : active_sleep_usec_input);

    short_sleep_1_usec = short_sleep_usec/RX_LOS_SHORT_SLEEP_ONE_FACTOR;
    short_sleep_2_usec = short_sleep_usec;

    while (!exit_thread) {

        all_steady = 1;
        no_sleep = 0;
        short_sleep_1 = 0;
        short_sleep_2 = 0;
        active_sleep = 0;

        rx_los_db_units_pbmp_get(&units_pbmp);

        /* handle units */
        BCM_PBMP_ITER(units_pbmp, unit) {

            if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
                continue;
            }

            if (rx_los_lock_per_unit[unit] == NULL) {
                continue;
            }
            rv = sal_mutex_take(rx_los_lock_per_unit[unit], sal_mutex_FOREVER);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_main_thread - sal_mutex_take FAILED\n")));
                continue;
            }


            rv = rx_los_db_unit_null_check(unit);
            BCM_FUNC_IF_ERR(rv) {
                sal_mutex_give(rx_los_lock_per_unit[unit]);
                continue;
            }

            rx_los_db_not_steady_state_pbmp_get(unit, &not_steady_state);

            rv = rx_los_db_supported_pbmp_get(unit, &pbmp_enabled);
            BCM_FUNC_IF_ERR(rv) {
                sal_mutex_give(rx_los_lock_per_unit[unit]);
                continue;
            }
            BCM_PBMP_AND(not_steady_state, pbmp_enabled);
            /* Handle links */
            PBMP_ITER(not_steady_state, port) {

                /*Init RX LOS callback mechanism status*/
                notify_callback = 0;

                link_state = rx_los_db_state_get(unit, port);

                switch(link_state) {

                case rx_los_state_ideal_state: /*Link was up*/

                    /*Mask relevant interrupts*/
                    if (IS_SFI_PORT(unit, port)) {
                        rv = rx_los_mac_rxlostofsync_int_mask_enable(unit, port, 0);
                    } else {
                        /*nif port*/
                        rv = rx_los_nif_link_status_change_mask_enable(unit, port, 0);
                    }
                    BCM_FUNC_IF_ERR(rv) {
                        LOG_ERROR(BSL_LS_APPL_PORT,
                                  (BSL_META_U(unit,
                                              "ERROR: rx_los_main_thread - unit %d port %d  - failed to mask interrupt\n"), unit, port));
                        break;
                    }

                    /*Set new state*/
                    rx_los_db_state_set( unit, port, rx_los_state_rx_seq_change);
                    rx_los_db_timestamp_update(unit,port);
                    all_steady = 0;
                    no_sleep = 1;

                    break;

                case rx_los_state_no_signal: /*Link was down - wake up by interrupt*/
                    /*fabric links state only*/
                    if (!IS_SFI_PORT(unit, port)) {
                        LOG_ERROR(BSL_LS_APPL_PORT,
                                  (BSL_META_U(unit,
                                              "ERROR: rx_los_main_thread - unit %d port %d  - rx_los_state_no_signal for fabric links only\n"), unit, port));
                    }

                    /* Mask relevant interrupt */
                    rv = rx_los_fsrd_syncstatuschanged_int_mask_enable(unit, port, 0);
                    BCM_FUNC_IF_ERR(rv) {
                        LOG_ERROR(BSL_LS_APPL_PORT,
                                  (BSL_META_U(unit,
                                              "ERROR: rx_los_main_thread - unit %d port %d  - failed to mask interrupt\n"), unit, port));
                        break;
                    }

                    /*Set new state*/
                    rx_los_db_state_set(unit, port, rx_los_state_rx_seq_change);
                    rx_los_db_timestamp_update(unit, port);
                    all_steady = 0;
                    no_sleep = 1;
                    break;

                case rx_los_state_no_signal_active: /*Link was down - wake up by long interval polling*/

                    wake_up = rx_los_wake_up(rx_los_db_timestamp_get(unit, port), active_sleep_usec);
                    all_steady = 0;
                    if (wake_up) {
                        rx_seq_done = 0;

                        /*polling over rx_seq_done*/
                        rv = rx_los_rx_seq_done_get(unit, port, &rx_seq_done);
                        BCM_FUNC_IF_ERR(rv) {
                            LOG_ERROR(BSL_LS_APPL_PORT,
                                      (BSL_META_U(unit,
                                                  "ERROR: rx_los_main_thread - rx_los_rx_seq_done_get FAILED\n")));
                            break;
                        }

                        if (rx_seq_done) {
                            /*Set new state*/
                            rx_los_db_state_set(unit, port, rx_los_state_rx_seq_change);
                            no_sleep = 1;
                        }
                        
                    }

                    active_sleep = 1;
                    rx_los_db_timestamp_update(unit, port);

                    break;

                case rx_los_state_rx_seq_change:

                    /* clear interrupt */
                    if (IS_SFI_PORT(unit, port)) {
                        rv = rx_los_rx_seq_done_clear(unit, port);
                        BCM_FUNC_IF_ERR(rv) {
                            LOG_ERROR(BSL_LS_APPL_PORT,
                                      (BSL_META_U(unit,
                                                  "ERROR: rx_los_main_thread - unit %d port %d  - failed to clear interrupt\n"), unit, port));
                            break;
                        }
                    }

                    /* check for rx seq done*/
                    rv = rx_los_rx_seq_done_get(unit, port, &rx_seq_done);
                    BCM_FUNC_IF_ERR(rv) {
                        LOG_ERROR(BSL_LS_APPL_PORT,
                                  (BSL_META_U(unit,
                                              "ERROR: rx_los_main_thread - rx_los_rx_seq_done_get FAILED\n")));
                        break;
                    }

                    if (rx_seq_done) {
                        rx_los_db_state_set(unit, port, rx_los_state_sleep_one);
                        all_steady = 0;
                        short_sleep_1 = 1;
                    } else {
                        rx_los_db_tries_initialize(unit, port);
                        if (IS_SFI_PORT(unit, port)) {
                            rv = rx_los_fsrd_syncstatuschanged_int_mask_enable(unit, port,1);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - failed to unmask interrupt\n"), unit, port));
                                break;
                            }
                            rx_los_db_state_set(unit, port, rx_los_state_no_signal);
                            rx_los_db_move_link_to_steady(unit, port);
                        } else {
                            /*no rx seq change interrupt for ARAD nif - polling*/
                            all_steady = 0;
                            active_sleep = 1;
                            rx_los_db_state_set(unit, port, rx_los_state_no_signal_active);
                        }

                        /*Move to no signal state - enable callback mechanism*/
                        notify_callback = 1;
                    }
                    rx_los_db_timestamp_update(unit, port);
                    break;

                case rx_los_state_sleep_one:

                    wake_up = rx_los_wake_up(rx_los_db_timestamp_get(unit, port), short_sleep_1_usec);
                    if (wake_up) {
                        /* Make sure interrupt is masked */
                        if (IS_SFI_PORT(unit, port)) {
                            rv = rx_los_fsrd_syncstatuschanged_int_mask_enable(unit, port, 0);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - failed to unmask interrupt\n"), unit, port));
                                break;
                            }
                        }

                        rx_los_db_state_set(unit, port, rx_los_state_restart);
                        rx_los_db_timestamp_update(unit, port);
                        no_sleep = 1;
                    } else {
                        short_sleep_1 = 1;
                    }
                    all_steady = 0;
                    break;

                case rx_los_state_restart:

                    /* Make sure interrupt is masked */
                    if (IS_SFI_PORT(unit, port)) {
                        rv = rx_los_fsrd_syncstatuschanged_int_mask_enable(unit, port, 0);
                        BCM_FUNC_IF_ERR(rv) {
                            LOG_ERROR(BSL_LS_APPL_PORT,
                                      (BSL_META_U(unit,
                                                  "ERROR: rx_los_main_thread - unit %d port %d  - failed to unmask interrupt\n"), unit, port));
                            break;
                        }
                    }

                    /* RX RESTART */
                    /* disable local and remote faults before RX restart */
#ifdef BCM_PETRA_SUPPORT
                    if (SOC_IS_ARAD(unit) && !IS_SFI_PORT(unit, port)) {
                        rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info); 
                        BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - bcm_port_get\n"), unit, port));
                                break;
                        }
                        if ( interface_info.interface != BCM_PORT_IF_ILKN) {
                            rv = soc_pm_fault_remote_enable_set(unit, port, 0);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - disable remote faults\n"), unit, port));
                                break;
                            }
                            rv = soc_pm_fault_local_enable_set(unit, port, 0);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - disable local faults\n"), unit, port));
                                break;
                            }
                        }
                    }
#endif
                    rv = bcm_port_phy_control_set(unit, port, BCM_PORT_PHY_CONTROL_RX_SEQ_TOGGLE, 1);
                    BCM_FUNC_IF_ERR(rv) {
                        LOG_ERROR(BSL_LS_APPL_PORT,
                                  (BSL_META_U(unit,
                                              "ERROR: rx_los_main_thread - rx seq restart FAILED\n")));
                        break;
                    }

                    rx_los_db_state_set(unit, port, rx_los_state_sleep_two);
                    rx_los_db_timestamp_update(unit, port);
                    /* continue to short sleep 2 - need to wait short sleep 2 */
                    short_sleep_2 = 1;
                    all_steady = 0;
                    
                    break;

                case rx_los_state_sleep_two:
                    
                    wake_up = rx_los_wake_up(rx_los_db_timestamp_get(unit, port), short_sleep_2_usec);
                    if (wake_up) {
                        all_steady = 0;
                        no_sleep = 1;
                        rx_los_db_state_set(unit, port, rx_los_state_link_up_check);
                        rx_los_db_timestamp_update(unit, port);
                    } else {
                        all_steady = 0;
                        short_sleep_2 = 1;
                    }
                    break;

                case rx_los_state_link_up_check:

                    /*Clears relvant interrupt before link up get*/
                    if (!IS_SFI_PORT(unit, port)) {
                        rv = rx_los_nif_link_status_interrupt_clear(unit, port);
                        BCM_FUNC_IF_ERR(rv) {
                            LOG_ERROR(BSL_LS_APPL_PORT,
                                      (BSL_META_U(unit,
                                                  "ERROR: rx_los_main_thread - unit %d port %d  - failed to unmask interrupt\n"), unit, port));
                            break;
                        }
                    }

                    rv = bcm_port_link_status_get(unit, port, &link_is_up);

                    BCM_FUNC_IF_ERR(rv) {
                        LOG_ERROR(BSL_LS_APPL_PORT,
                                  (BSL_META_U(unit,
                                              "ERROR: rx_los_main_thread - bcm_port_link_status_get FAILED\n")));
                        break;
                    }

                    /* double read to avoid sticky bit */

                    rv = bcm_port_link_status_get(unit, port, &link_is_up);

                    BCM_FUNC_IF_ERR(rv) {
                        LOG_ERROR(BSL_LS_APPL_PORT,
                                  (BSL_META_U(unit,
                                              "ERROR: rx_los_main_thread - bcm_port_link_status_get FAILED\n")));
                        break;
                    }

                    if (link_is_up) {
                        rx_los_db_tries_initialize(unit, port);
                        rx_los_db_state_set(unit, port, rx_los_state_ideal_state);
                        rx_los_db_move_link_to_steady(unit, port);

                        /* unmask rxlostofsync interrupt */
                        if (IS_SFI_PORT(unit, port)) {
                            rv = rx_los_mac_rxlostofsync_int_mask_enable(unit, port, 1);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - rx_los_mac_rxlostofsync_int_mask_enable FAILED\n")));
                            }
                        } else {
                            /*ARAD nif port*/
                            rv = rx_los_nif_link_status_change_mask_enable(unit, port, 1);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - rx_los_mac_rxlostofsync_int_mask_enable FAILED\n")));
                            }
                        }

                        /*Move to ideal state - enable callback mechanism*/
                        notify_callback = 1;

                    } else if (rx_los_db_tries_get(unit,port) > allowed_retries) {
                        rx_los_db_state_set(unit, port, rx_los_state_long_sleep);
                        all_steady = 0;
                    } else {
                        rx_los_db_state_set(unit, port, rx_los_state_rx_seq_change);      
                        all_steady = 0;
                        no_sleep = 1;
                        rx_los_db_tries_increment(unit,port);
                    }

#ifdef BCM_PETRA_SUPPORT
                    if (SOC_IS_ARAD(unit) && !IS_SFI_PORT(unit, port)) {
                        /* enable local and remote faults after RX restart */
                        rv = bcm_port_get(unit, port, &flags, &interface_info, &mapping_info); 
                        BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - bcm_port_get\n"), unit, port));
                            break;
                        }
                        if (interface_info.interface != BCM_PORT_IF_ILKN) {
                            /* clear faults status */
                            rv = bcm_port_control_set(unit, port, bcmPortControlLinkFaultRemote, 0);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - clear remote faults status\n"), unit, port));
                                break;
                            }

                            rv = bcm_port_control_set(unit, port, bcmPortControlLinkFaultLocal, 0);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - clear local faults status\n"), unit, port));
                                break;
                            }

                            rv = soc_pm_fault_remote_enable_set(unit, port, 1);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - enable remote faults\n"), unit, port));
                                break;
                            }

                            rv = soc_pm_fault_local_enable_set(unit, port, 1);
                            BCM_FUNC_IF_ERR(rv) {
                                LOG_ERROR(BSL_LS_APPL_PORT,
                                          (BSL_META_U(unit,
                                                      "ERROR: rx_los_main_thread - unit %d port %d  - enable local faults\n"), unit, port));
                                break;
                            }
                        }
                    }
#endif

                    rx_los_db_timestamp_update(unit, port);
                    break;

                case rx_los_state_long_sleep:

                    wake_up = rx_los_wake_up(rx_los_db_timestamp_get(unit, port), long_sleep_usec);
                    all_steady = 0;
                    if (wake_up) {
                        rx_los_db_state_set(unit, port, rx_los_state_rx_seq_change);
                        rx_los_db_timestamp_update(unit, port);
                        no_sleep = 1;
                    }
                    break;

                default:

                    break;
                }


                /*RX LOS callback mechanism*/
                if (notify_callback) {
                    rx_los_callback_t callback_func;

                    callback_func = NULL;

                    rv = rx_los_db_callback_get(unit, &callback_func);
                    BCM_FUNC_IF_ERR(rv) {
                        LOG_ERROR(BSL_LS_APPL_PORT,
                                  (BSL_META_U(unit,
                                              "ERROR: rx_los_main_thread - rx_los_callback_get FAILED\n")));
                    }

                    if (callback_func != NULL) {
                        link_state = rx_los_db_state_get(unit, port);
                        callback_func(unit, port, link_state);
                    }

                }
            }

            sal_mutex_give(rx_los_lock_per_unit[unit]);
        }


        if (all_steady) {
            sal_sem_take(rx_los_sem, sal_sem_FOREVER);
        } else {
            if (!no_sleep) {
                if (short_sleep_1) {
                    sal_sem_take(rx_los_sem, short_sleep_1_usec);
                } else if (short_sleep_2) {
                    sal_sem_take(rx_los_sem, short_sleep_2_usec);
                } else if (active_sleep) {
                    sal_sem_take(rx_los_sem, active_sleep_usec);
                } else {
                    sal_sem_take(rx_los_sem, long_sleep_usec);
                }
            }
        }


    }


    rx_los_thread_exist = 0;
    return;
}

void rx_los_main_thread_call(void *args){
    uint32 allowed_retries,
        short_sleep_usec,
        active_sleep_usec,
        long_sleep_usec;

    /* get parameters: */
    long_sleep_usec =  rx_los_db_long_sleep_get();
    short_sleep_usec = rx_los_db_short_sleep_get();
    allowed_retries = rx_los_db_allowed_retries_get();
    active_sleep_usec = rx_los_db_active_sleep_get();

    rx_los_main_thread(allowed_retries, short_sleep_usec, long_sleep_usec, active_sleep_usec);

}

int
rx_los_handle(int unit, int port) {
    int rv;
    int is_recurring;
    rx_los_state_t state;
    bcm_pbmp_t pbmp_enabled;


    LOG_DEBUG(BSL_LS_APPL_PORT,
              (BSL_META_U(unit,
                          "unit %d: rx_los_handle port %d\n"),unit, port));

    /*Verification*/
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_handle - invalid unit\n")));
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_handle - invalid port\n")));
        return BCM_E_PORT;
    }

    if ((rx_los_lock == NULL) || (rx_los_sem == NULL) || (rx_los_lock_per_unit[unit] == NULL)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_handle - lock or semaphore not initialized\n")));
        return BCM_E_FAIL;
    }

    rv = sal_mutex_take(rx_los_lock_per_unit[unit], sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_handle - sal_mutex_take FAILED\n")));
        return rv;
    }

    rv = rx_los_db_unit_null_check(unit);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_handle - rx los app uninitialized \n")));
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return rv;
    }

    rv = rx_los_db_supported_pbmp_get(unit, &pbmp_enabled);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_handle - rx los app uninitialized \n")));
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return rv;
    }
    if (!BCM_PBMP_MEMBER(pbmp_enabled, port)) {
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return BCM_E_NONE;
    }

    /*Avoid restarting link in case of temp lost of sync*/
    rv = rx_los_recurring_get(unit, port, &is_recurring);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_handle - rx_los_recurring_get FAILED\n")));
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return rv;
    }
    state = rx_los_db_state_get(unit, port);
    if (state == rx_los_state_ideal_state && !is_recurring) {
        /*handle just recurring interrupt*/
        /*umask relevant interrupt*/
        if (IS_SFI_PORT(unit, port)) {
            int link_is_up;
             
            rv = bcm_port_link_status_get(unit, port, &link_is_up);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_handle - failed to clear interrupt\n")));
            }
            rv = rx_los_mac_rxlostofsync_int_mask_enable(unit, port, 1);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_handle - rx_los_mac_rxlostofsync_int_mask_enable FAILED\n")));
            }
        } else {
            /*ARAD nif port*/

            /*Clears relvant interrupt before link up get*/
            rv = rx_los_nif_link_status_interrupt_clear(unit, port);
            BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_handle - failed to clear interrupt\n")));
            }
            rv = rx_los_nif_link_status_change_mask_enable(unit, port, 1);
                BCM_FUNC_IF_ERR(rv) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_handle - rx_los_mac_rxlostofsync_int_mask_enable FAILED\n")));
            }
        }

        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return BCM_E_NONE;

    }

    /*Avoid storm over rx seq change interrupt - might happen in case of two links in different speed*/
    if ((state == rx_los_state_no_signal) && is_recurring) {
        rx_los_db_state_set(unit, port, rx_los_state_no_signal_active);
    }

    rv = rx_los_db_remove_link_from_steady(unit, port);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_handle - rx_los_db_remove_link_from_steady FAILED\n")));
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return rv;
    }

    /*Make sure the relevant inetrrupt are masked and cleared*/
    if (IS_SFI_PORT(unit, port)) {
        rv = rx_los_fsrd_syncstatuschanged_int_mask_enable(unit, port, 0);
        BCM_FUNC_IF_ERR(rv) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_handle - rx_los_fsrd_syncstatuschanged_int_mask_enable FAILED\n")));
        }
        rv = rx_los_mac_rxlostofsync_int_mask_enable(unit, port, 0);
        BCM_FUNC_IF_ERR(rv) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_handle - rx_los_mac_rxlostofsync_int_mask_enable FAILED\n")));
        }
    } else {
        /*ARAD nif port*/
        rv = rx_los_nif_link_status_change_mask_enable(unit, port, 0);
        BCM_FUNC_IF_ERR(rv) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_handle - rx_los_mac_rxlostofsync_int_mask_enable FAILED\n")));
        }
    }

    sal_mutex_give(rx_los_lock_per_unit[unit]);

    rv = sal_sem_give(rx_los_sem);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_handle - sal_sem_give FAILED\n")));
        return rv;
    }

    LOG_DEBUG(BSL_LS_APPL_PORT,
              (BSL_META_U(unit,
                          "unit %d: interrupt handled successfully: port %d\n"), unit, port));

    return rv;
}


int rx_los_exit_thread(){
    exit_thread = 1;
    if (rx_los_sem!=NULL) {
        sal_sem_give(rx_los_sem);
    }
    return BCM_E_NONE;
}

/*
 * Function: 
 *       rx_los_unit_detach
 * Purpose: 
 *       SW&HW detach unit from RX LOS application.
 *       Remove unit from RX LOS data base and mask interrupts.
 * Parameters: 
 *      unit -  (IN) unit #
 */
int rx_los_unit_detach(int unit){
    return rx_los_generic_unit_detach(unit, 0);
}
    /*
 * Function: 
 *       rx_los_sw_unit_detach
 * Purpose: 
 *       SW only  detach unit from RX LOS application.
 *       SW only detach will avoid from access to device and will not mask the relevant interrupts
 * Parameters: 
 *      unit -  (IN) unit #
 */
int rx_los_sw_unit_detach(int unit){
    return rx_los_generic_unit_detach(unit, RX_LOS_GENERIC_UNIT_DETACH_F_SW_ONLY);
}

/*
 * Function: 
 *       rx_los_generic_unit_detach
 * Purpose: 
 *       SW or SW&HW detach unit from RX LOS application.
 *       SW only detach will avoid from access to device and will not mask the relevant interrupts
 * Parameters: 
 *      unit -  (IN) unit #
 *      flags - (IN) supported flags RX_LOS_GENERIC_UNIT_DETACH_F_SW_ONLY
 */
int rx_los_generic_unit_detach(int unit, uint32 flags){
    pbmp_t pbmp;
    int port;
    int rv;
    soc_timeout_t to;
    int sw_detach_only;

    sw_detach_only = (flags & RX_LOS_GENERIC_UNIT_DETACH_F_SW_ONLY ? 1 : 0);

    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_detach - invalid unit\n")));
        return BCM_E_UNIT;
    }

    if (!SOC_IS_ARAD(unit) && !SOC_IS_FE1600(unit))
    {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_detach - feature unavail\n")));
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit))
    {
        /*Do nothing*/
        return BCM_E_NONE;
    }
#endif


    /* null checks*/
    if (rx_los_lock == NULL) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "rx_los_unit_detach - rx_los_lock is NULL\n")));
        return BCM_E_FAIL;
    }
    rv = sal_mutex_take(rx_los_lock, sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_detach - sal_mutex_take FAILED\n")));
        return rv;
    }

    if (!rx_loss_appl_enable(unit)) {
        sal_mutex_give(rx_los_lock);
        return BCM_E_NONE;
    }


    /* disable all supported ports*/
    rv =  rx_los_db_supported_pbmp_get(unit, &pbmp);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_detach - rx_los_db_supported_pbmp_get FAILED\n")));
        sal_mutex_give(rx_los_lock);
        return rv;
    }

    PBMP_ITER(pbmp,port){
        rv = rx_los_generic_port_enable(unit, port, 0, 0, sw_detach_only ? RX_LOS_GENERIC_PORT_ENABLE_F_SW_ONLY : 0);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_unit_detach - rx_los_port_enable FAILED\n")));
            continue;
        }
    }

    /* remove from db */
    rv = sal_mutex_take(rx_los_lock_per_unit[unit], sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_detach - sal_mutex_take FAILED\n")));
        sal_mutex_give(rx_los_lock);
        return rv;
    }
    rv = rx_los_db_unit_deinit(unit);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_detach - rx_los_db_unit_deinit FAILED\n")));
    }
    sal_mutex_give(rx_los_lock_per_unit[unit]);

    /* if unit is last one, exit thread */
    rx_los_db_units_pbmp_get(&pbmp);

    if (BCM_PBMP_IS_NULL(pbmp)) {
        exit_thread = 1;
        sal_sem_give(rx_los_sem);

        /*polling over exit thread*/
                        
        soc_timeout_init(&to, 2000000 /*2 sec*/, 100);  
        while (rx_los_thread_exist) {
            if (soc_timeout_check(&to)) {
                LOG_ERROR(BSL_LS_APPL_PORT,
                          (BSL_META_U(unit,
                                      "ERROR: rx_los_unit_detach - thread did not exit FAILED\n")));
                break;
            }
            sal_sem_give(rx_los_sem);
            sal_usleep(1000);
        }
    }

    sal_mutex_give(rx_los_lock);

    return rv;
}

int rx_los_unit_attach(int unit, const pbmp_t pbmp, int warm_boot){

    int rv = BCM_E_NONE, spl_level, port;
    bcm_pbmp_t pbmp_updated;
    uint32 priority;

    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_attach - invalid unit\n")));
        return BCM_E_UNIT;
    }

    if (!SOC_IS_ARAD(unit) && !SOC_IS_FE1600(unit))
    {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_attach - feature unavail\n")));
        return BCM_E_UNAVAIL;
    }

#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit))
    {
        /*Do nothing*/
        return BCM_E_NONE;
    }
#endif

    /*Make sure interrupt mechanism is initialized*/
    if(SAL_BOOT_NO_INTERRUPTS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_attach - RX LOS App requires interrupt to be init\n")));
        return BCM_E_FAIL;
    }

    spl_level = sal_splhi();

    /* create the lock */
    if (rx_los_lock == NULL) {
        rx_los_lock = sal_mutex_create("rx_los_lock");
        if (rx_los_lock == NULL){
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_unit_attach - sal_mutex_create FAILED\n")));
            sal_spl(spl_level);
            return BCM_E_FAIL;
        }
        /* init db and create thread */
        rx_los_db_init();
    
        /* create the semaphore*/
        rx_los_sem = sal_sem_create("rx_los_sem", 0, 0);
        if (rx_los_sem == NULL){
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_unit_attach - sal_sem_create FAILED\n")));
            sal_spl(spl_level);
            return BCM_E_FAIL;
        }
    }

    rv = sal_mutex_take(rx_los_lock, sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_attach - sal_mutex_take FAILED\n")));
        return rv;
    }

    /* handle first time initlialization */
    if(!rx_los_thread_exist) {

        priority = rx_los_db_thread_priority_get();

        if (priority > 255) {
            priority = 100;
        }

        rx_los_thread_exist = 1;
        sal_thread_create("rx_los_main_thread", SAL_THREAD_STKSZ, priority, rx_los_main_thread_call, NULL);
    } 
    
    /* add unit to database */
    /* create the lock */
    if (rx_los_lock_per_unit[unit] == NULL){
        rx_los_lock_per_unit[unit] = sal_mutex_create("rx_los_lock_per_unit");
        if (rx_los_lock_per_unit[unit] == NULL){
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_db_unit_attach - sal_mutex_create FAILED\n")));
            sal_mutex_give(rx_los_lock);
            sal_spl(spl_level);
            return BCM_E_FAIL;
        }
    }


    rv = sal_mutex_take(rx_los_lock_per_unit[unit], sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_attach - sal_mutex_take FAILED\n")));
        sal_mutex_give(rx_los_lock);
        sal_spl(spl_level);
        return rv;
    }

    BCM_PBMP_CLEAR(pbmp_updated);
    rv = rx_los_db_unit_init(unit, pbmp_updated);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_attach - rx_los_db_unit_init FAILED\n")));
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        sal_mutex_give(rx_los_lock);
        sal_spl(spl_level);
        return BCM_E_FAIL;
    }

    /*pbmp default values in case of an empty bitmap*/
    BCM_PBMP_ASSIGN(pbmp_updated, pbmp);
    if (BCM_PBMP_IS_NULL(pbmp_updated)) {
        RX_LOS_PBMP_DEFAULT(unit, pbmp_updated);
    }

    sal_mutex_give(rx_los_lock_per_unit[unit]);
    sal_mutex_give(rx_los_lock);
    sal_spl(spl_level);

    rv = sal_mutex_take(rx_los_lock, sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_unit_attach - sal_mutex_take FAILED\n")));
        return rv;
    }

    rv = rx_los_db_unit_null_check(unit);
    BCM_FUNC_IF_ERR(rv) {
        /*chip already detached*/

        /*release RX LOS lock*/
        sal_mutex_give(rx_los_lock);
        return BCM_E_NONE;
    }
    
    PBMP_ITER(pbmp_updated, port) {
        
        rv = rx_los_port_enable(unit, port, 1, warm_boot);
        BCM_FUNC_IF_ERR(rv) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "rx_los_unit_attach - rx_los_port_enable FAILED\n")));
        }
    }

    sal_mutex_give(rx_los_lock);

    sal_sem_give(rx_los_sem);

    LOG_VERBOSE(BSL_LS_APPL_PORT,
                (BSL_META_U(unit,
                            "unit %d: rx_los_unit_attach finished successfully\n"), unit));
    
    return rv;
}

int
rx_los_dump_port(int unit, bcm_port_t port) 
{
    int rv;

    /*input verification*/
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_dump_port - invalid unit\n")));
        return BCM_E_UNIT;
    }
    if (!SOC_PORT_VALID(unit, port)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_dump_port - invalid port\n")));
        return BCM_E_PORT;
    }

    /*Lock rx los data base*/
    if (rx_los_lock_per_unit[unit] == NULL) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_dump - lock not initialized\n")));
        return BCM_E_FAIL;
    }
    rv = sal_mutex_take(rx_los_lock_per_unit[unit], sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_dump_port - sal_mutex_take FAILED\n")));
        return BCM_E_FAIL;
    }

    rv = rx_los_db_unit_null_check(unit);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_dump_port - rx los app uninitialized \n")));

        /*Release RX LOS data base*/
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return rv;
    }


    /*data base dump*/
    rx_los_db_dump_port(unit, port);

    /*Release rx los data base*/
    sal_mutex_give(rx_los_lock_per_unit[unit]);

    return rv;
}

int
rx_los_dump(int unit) {

    int rv;

    /*input verification*/
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_dump - invalid unit\n")));
        return BCM_E_UNIT;
    }

    /*Lock rx los data base*/
    if (rx_los_lock_per_unit[unit] == NULL) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_dump - lock not initialized\n")));
        return BCM_E_FAIL;
    }

    rv = sal_mutex_take(rx_los_lock_per_unit[unit], sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_dump - sal_mutex_take FAILED\n")));

        return BCM_E_FAIL;
    }

    rv = rx_los_db_unit_null_check(unit);
    BCM_FUNC_IF_ERR(rv) {

        /*Release RX LOS data base*/
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_dump - rx los app uninitialized \n")));
        return rv;
    }

    /*data base dump*/
    rx_los_db_dump(unit);

    /*Release rx los data base*/
    sal_mutex_give(rx_los_lock_per_unit[unit]);

    return rv;

}

int rx_los_register(int unit, rx_los_callback_t callback)
{
    int rv;

    /*Input verification*/
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_register - invalid unit\n")));
        return BCM_E_UNIT;
    }

    /*Lock RX LOS data base*/
    if (rx_los_lock_per_unit[unit] == NULL) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_register - lock not initialized\n")));
        return BCM_E_FAIL;
    }

    rv = sal_mutex_take(rx_los_lock_per_unit[unit], sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_register - sal_mutex_take FAILED\n")));
        return BCM_E_FAIL;
    }

    rv = rx_los_db_unit_null_check(unit);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_register - rx los app uninitialized \n")));

        /*Release RX LOS data base*/
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return rv;
    }

    rv = rx_los_db_register(unit, callback);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_register - rx_los_db_register FAILED\n")));

        /*Release RX LOS data base*/
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return rv;
    }

    /*Release RX LOS data base*/
    sal_mutex_give(rx_los_lock_per_unit[unit]);


    return rv;

}

int rx_los_enable_get(int unit, bcm_pbmp_t *pbmp, int *enable) {

    int rv;
     /*Input verification*/
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_enable_get - invalid unit\n")));
        return BCM_E_UNIT;
    }

    if (pbmp == NULL || enable == NULL) {
        return BCM_E_PARAM;
    }

    /*Lock rx los data base*/
    if (rx_los_lock_per_unit[unit] == NULL) {
        /*RX LOS disabled*/
        *enable = 0;
        BCM_PBMP_CLEAR(*pbmp);
        return BCM_E_NONE;
    }

    rv = sal_mutex_take(rx_los_lock_per_unit[unit], sal_mutex_FOREVER);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_enable_get - sal_mutex_take FAILED\n")));
        return BCM_E_FAIL;
    }

    rv = rx_los_db_unit_null_check(unit);
    BCM_FUNC_IF_ERR(rv) {
        /*RX LOS disabled*/
        *enable = 0;
        BCM_PBMP_CLEAR(*pbmp);

        /*Release RX LOS data base*/
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return BCM_E_NONE;
    }

    *enable=1;
    rv = rx_los_db_supported_pbmp_get(unit, pbmp);
    BCM_FUNC_IF_ERR(rv) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_enable_get - rx_los_db_supported_pbmp_get FAILED\n")));

        /*Release RX LOS data base*/
        sal_mutex_give(rx_los_lock_per_unit[unit]);
        return BCM_E_FAIL;
    }

    /*Release RX LOS data base*/
    sal_mutex_give(rx_los_lock_per_unit[unit]);

    return BCM_E_NONE;


}

int
rx_los_port_stable(int unit, bcm_port_t port, int timeout_usec, rx_los_state_t *state)
{
    int rv;
    soc_timeout_t to;
    bcm_pbmp_t pbmp;
    rx_los_state_t temp_state;

    /* 
     *Input verification
     */
    if (unit < 0 || unit >= BCM_MAX_NUM_UNITS) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_enable_get - invalid unit\n")));
        return BCM_E_UNIT;
    }

    if (state == NULL) {
        return BCM_E_PARAM;
    }

    if (!SOC_PORT_VALID(unit, port)) {
        LOG_ERROR(BSL_LS_APPL_PORT,
                  (BSL_META_U(unit,
                              "ERROR: rx_los_port_enable - invalid port\n")));
        return BCM_E_PORT;
    }

    *state = rx_los_states_count;

    /* 
     *Check RX LOS  lock data base exist
     */
     
    if (rx_los_lock_per_unit[unit] == NULL) {
        /*RX LOS disabled*/
        *state = rx_los_states_count;
        return BCM_E_NONE;
    }

    /* 
     *Polling on RX LOS port state - max timeout
     */
    soc_timeout_init(&to, timeout_usec, 0);
    
    while (1)
    {
        /*Giving a chance to rx_los_handle to change the port state*/
        sal_usleep(20000); /*sleep 20 milisec*/

        /*Take RX LOS unit db lock*/
        rv = sal_mutex_take(rx_los_lock_per_unit[unit], timeout_usec);
        if (BCM_FAILURE(rv)) {
            *state = rx_los_states_count;
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_port_stable - sal_mutex_take FAILED\n")));
            return BCM_E_FAIL;
        }

        /*Check if unit supports RX LOS*/
        rv = rx_los_db_unit_null_check(unit);
        BCM_FUNC_IF_ERR(rv) {
            /*RX LOS disabled*/
            *state = rx_los_states_count;

            /*Release RX LOS data base*/
            sal_mutex_give(rx_los_lock_per_unit[unit]);
            return BCM_E_NONE;
        }

        /*Check if port supports RX LOS*/
        rv = rx_los_db_supported_pbmp_get(unit, &pbmp);
        BCM_FUNC_IF_ERR(rv) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_port_stable - rx_los_db_supported_pbmp_get FAILED\n")));

            /*Release RX LOS data base*/
            sal_mutex_give(rx_los_lock_per_unit[unit]);
            return BCM_E_FAIL;
        }
        if (!BCM_PBMP_MEMBER(pbmp, port))
        {
            *state = rx_los_states_count;

            /*Release RX LOS data base*/
            sal_mutex_give(rx_los_lock_per_unit[unit]);
            return BCM_E_NONE;
        }


        /*Check if port is stable*/
        temp_state = rx_los_db_state_get(unit, port);

        /*Release RX LOS data base*/
        sal_mutex_give(rx_los_lock_per_unit[unit]);

        if (temp_state == rx_los_state_ideal_state      ||
            temp_state == rx_los_state_no_signal        ||
            temp_state == rx_los_state_no_signal_active)
        {
            *state = temp_state;
            break;
        /* Check if timeout*/
        } else if (soc_timeout_check(&to)) {
            LOG_ERROR(BSL_LS_APPL_PORT,
                      (BSL_META_U(unit,
                                  "ERROR: rx_los_port_stable - port %d is not stable\n"), port));
            return BCM_E_FAIL;
        }
    }

    return BCM_E_NONE;
}
