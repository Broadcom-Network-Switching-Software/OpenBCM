/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PCIe hot swap implementation
 */

#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/boot.h>
#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/error.h>
#include <sal/core/sync.h>
#include <soc/util.h>
#include <soc/cmic.h>

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_IPROC_SUPPORT)
#include <soc/iproc.h>
#endif
/*
 * Function:
 *      soc_pcie_hot_swap_handling
 * Purpose:
 *      Handle the iproc hot swap mechanism at startup.
 *      Must be called in device stsartup if the iproc hotswap mechanism is active.
 *      The flags control the function's behaviour.
 *      de-configure previously defined DMA operations and abort pending operation.
 *      The hot swap handling will be done if the iproc hot-swap state machine
 *      shows it is needed.
 * Parameters:
 *      unit - SOC unit #
 *      flags - SOC_PCIE_HOTSWAP_HANDLING_FLAG_* flags
 * Returns:
 *      SOC_E_NONE - success
 *      SOC_E_XXXX - error code
 * Notes:
 */
int
soc_pcie_hot_swap_handling(int unit, uint32 flags)
{
    int rv = SOC_E_NONE;

#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    int hot_swap_usec = 100;
    uint32 hot_swap_cause = 0;
    uint32 rval = 0;
    uint32 int_en;
    uint32 fsm_state;
    soc_timeout_t to;
    sal_usecs_t stime, etime;

    if ((soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) == 0 ||
        !SOC_REG_IS_VALID(unit, PAXB_0_PAXB_HOTSWAP_STATr)) {
        return SOC_E_NONE;
    }

    /* Suggested default time out is 100 usec for FSM_STATE to be DUMMYRESP or IDLE */
    hot_swap_usec = soc_property_get(unit, spn_PCIE_HOT_SWAP_TIMEOUT_USEC, 100);
    LOG_INFO(BSL_LS_SOC_COMMON,
            (BSL_META_U(unit,
                        "HOT_SWAP_TIMEOUT_USEC = %d\n"), hot_swap_usec));

    soc_timeout_init(&to, hot_swap_usec, 0);

    /* READ HOTSWAP_CAUSE field to determine if hot swap event has happened */
    SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_HOTSWAP_STATr(unit, &rval));
    hot_swap_cause = soc_reg_field_get(unit, PAXB_0_PAXB_HOTSWAP_STATr,
                                  rval, HOTSWAP_CAUSEf);
    fsm_state = soc_reg_field_get(unit, PAXB_0_PAXB_HOTSWAP_STATr, rval, FSM_STATEf);

    /* Hot Swap status before Recovery */
    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                      "Hot Swap status before Recovery = 0x%x\n"),
                      rval));

    /* hot swap handling is needed if the FSM state is AXI dummy response, or a cause is marked */
    if (hot_swap_cause != 0 || fsm_state == 0x3 /* DUMMYRESP */) {
        if ((flags & SOC_PCIE_HOTSWAP_HANDLING_FLAG_DONOT_DISABLE_INTERRUPTS) == 0) {
            /* Clear and Disable PAXB interrupts */
            SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_INTR_STATUSr(unit, &rval));
            SOC_IF_ERROR_RETURN(WRITE_PAXB_0_PAXB_INTR_STATUSr(unit, rval));
            SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_INTR_ENr(unit, &int_en));
            SOC_IF_ERROR_RETURN(WRITE_PAXB_0_PAXB_INTR_ENr(unit, 0));
        }

        /* Poll for the HotSwap FSM_STATE to be DUMMYRESP or IDLE */
        stime = sal_time_usecs();
        while (fsm_state != 0x0 &&  /* IDLE */
               fsm_state != 0x3 &&  /* DUMMYRESP */
               !soc_timeout_check(&to)) {
            SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_HOTSWAP_STATr(unit, &rval));
            fsm_state = soc_reg_field_get(unit, PAXB_0_PAXB_HOTSWAP_STATr,
                                      rval, FSM_STATEf);
        }
        etime = sal_time_usecs();
        /* Check if time out has happened */
        if (SAL_USECS_SUB(etime, stime) > hot_swap_usec) {
            LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Hot Swap Wait Time Out for IDLE/DUMMYRESP\n")));
            rv = SOC_E_TIMEOUT;
            SOC_IF_ERROR_RETURN(rv);
        } else {
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                        "Hot Swap logic IDLE/DUMMYRESP  done in %d usec\n"),
                        SAL_USECS_SUB(etime, stime)));
        }
        if ((flags & SOC_PCIE_HOTSWAP_HANDLING_FLAG_DONOT_ABORT) == 0) {
            /* Only Abort the DMA for CMC0 */
#ifdef BCM_CMICX_SUPPORT
            if (soc_feature(unit, soc_feature_cmicx)) {
                rv = soc_cmicx_dma_abort(unit, SOC_DMA_ABORT_CMC0);
            }
#endif /* BCM_CMICX_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
            if (soc_feature(unit, soc_feature_cmicm)) {
                rv = soc_cmicm_dma_abort(unit, SOC_DMA_ABORT_CMC0);
            }
#endif /* BCM_CMICM_SUPPORT */
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "Error: Hot Swap recovery DMA Abort = %d\n"), rv));
                SOC_IF_ERROR_RETURN(soc_event_generate(unit,
                            SOC_SWITCH_EVENT_UNCONTROLLED_SHUTDOWN,
                            0, 0, 0));
            }
        }
        /* Assert SERVICE_COMPLETE bit in HOTSWAP CONTROL register to communicate to
         *      PCIE subsystem that all CMIC DMA channels have been aborted
         */
        if (SOC_REG_IS_VALID(unit, PAXB_0_PAXB_HOTSWAP_CTRLr) &&
             SOC_REG_FIELD_VALID(unit, PAXB_0_PAXB_HOTSWAP_CTRLr, SERVICE_COMPLETEf)) {
             SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, &rval));
             soc_reg_field_set(unit, PAXB_0_PAXB_HOTSWAP_CTRLr, &rval, SERVICE_COMPLETEf, 1);
             SOC_IF_ERROR_RETURN( WRITE_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, rval));
        }

        SDK_CONFIG_MEMORY_BARRIER;

        /* Wait for PAXB_RDY bit to get set in HOTSWAP_STAT register to ensure that PCIE
         *      subsystem is ready to accept new requests
         */
        if (SOC_REG_FIELD_VALID(unit, PAXB_0_PAXB_HOTSWAP_STATr, PAXB_RDYf)) {
            int ready;
            soc_timeout_init(&to, hot_swap_usec, 0);
            stime = sal_time_usecs();
            while (!soc_timeout_check(&to)) {
                SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_HOTSWAP_STATr(unit, &rval));
                ready = soc_reg_field_get(unit, PAXB_0_PAXB_HOTSWAP_STATr,
                                           rval, PAXB_RDYf);
                if (ready) {
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "PAXB Ready to accept new requests.\n")));
                    /* Reset PAXB_RDY and HOTSWAP_CAUSE bit */
                    rval = 0;
                    soc_reg_field_set(unit, PAXB_0_PAXB_HOTSWAP_STATr,
                                      &rval, PAXB_RDYf, 0x01);
                    soc_reg_field_set(unit, PAXB_0_PAXB_HOTSWAP_STATr,
                                      &rval, HOTSWAP_CAUSEf, 0x07);
                    SOC_IF_ERROR_RETURN(WRITE_PAXB_0_PAXB_HOTSWAP_STATr(unit, rval));
                    break;
                }
            }
            etime = sal_time_usecs();
            /* Hot Swap status after Recovery */
            SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_HOTSWAP_STATr(unit, &rval));
            LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                     "Hot Swap status after Recovery = 0x%x\n"), rval));
            if ((flags & SOC_PCIE_HOTSWAP_HANDLING_FLAG_DONOT_DISABLE_INTERRUPTS) == 0) {
                /* Clear and Restore PAXB interrupts */
                SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_INTR_STATUSr(unit, &rval));
                SOC_IF_ERROR_RETURN(WRITE_PAXB_0_PAXB_INTR_STATUSr(unit, rval));
                SOC_IF_ERROR_RETURN(WRITE_PAXB_0_PAXB_INTR_ENr(unit, int_en));
            }

            /* Check if time out has happened */
            if (SAL_USECS_SUB(etime, stime) > hot_swap_usec) {
                LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                          "Hot Swap Wait Time Out for READY\n")));
                /* Debug information */
                if (SOC_REG_IS_VALID(unit, CMICX_M0_IDM_IDM_RESET_STATUSr)) {
                    SOC_IF_ERROR_RETURN
                        (READ_CMICX_M0_IDM_IDM_RESET_STATUSr(unit, &rval));
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "CMICX_M0_IDM_IDM_RESET_STATUSr=0x%x \r\n"), rval));
                } else if (SOC_REG_IS_VALID(unit, CMICD_M0_IDM_IDM_RESET_STATUSr)) {
                    SOC_IF_ERROR_RETURN
                        (READ_CMICD_M0_IDM_IDM_RESET_STATUSr(unit, &rval));
                    LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                            "CMICD_M0_IDM_IDM_RESET_STATUSr=0x%x \r\n"), rval));
                }
                SOC_IF_ERROR_RETURN
                    (READ_AXI_PCIE_S0_IDM_IDM_RESET_STATUSr(unit, &rval));
                LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                         "AXI_PCIE_S0_IDM_IDM_RESET_STATUSr=0x%x \r\n"), rval));
                rv = SOC_E_TIMEOUT;
                SOC_IF_ERROR_RETURN(rv);
            } else {
                LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit,
                         "PCI subsystem READY in %d usec\n"),
                         SAL_USECS_SUB(etime, stime)));
            }
        }
    } else {
        LOG_INFO(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "No Hot Swap event recorded\n")));
    }
#endif /* defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT) */
    return rv;
}


/*
 * Function:
 *      soc_pcie_hot_swap_disable
 * Purpose:
 *      Disable hot swap manager.
 * Parameters:
 *      unit - unit #
 * Returns:
 *      SOC_E_XXX
*/
int
soc_pcie_hot_swap_disable(int unit)
{
    int rv = SOC_E_UNAVAIL;

#ifdef BCM_CMICX_SUPPORT
    uint32  rval;
    if (soc_feature(unit, soc_feature_cmicx) &&
            (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE)) {
        /* Disable Hot Swap manager*/
        if (SOC_REG_IS_VALID(unit, PAXB_0_PAXB_HOTSWAP_CTRLr) &&
            SOC_REG_FIELD_VALID(unit, PAXB_0_PAXB_HOTSWAP_CTRLr, ENABLEf)) {
            SOC_IF_ERROR_RETURN(READ_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, &rval));
            soc_reg_field_set(unit, PAXB_0_PAXB_HOTSWAP_CTRLr, &rval, ENABLEf, 0);
            SOC_IF_ERROR_RETURN(
                WRITE_PAXB_0_PAXB_HOTSWAP_CTRLr(unit, rval));
            /* Enable reset to propogate to paxb data path logic when pcie link is down. */
            SOC_IF_ERROR_RETURN
                (READ_PAXB_0_RESET_ENABLE_IN_PCIE_LINK_DOWNr(unit, &rval));
            soc_reg_field_set(unit, PAXB_0_RESET_ENABLE_IN_PCIE_LINK_DOWNr, &rval,
                              EN_PAXB_RESETf, 1);
            SOC_IF_ERROR_RETURN
                (WRITE_PAXB_0_RESET_ENABLE_IN_PCIE_LINK_DOWNr(unit, rval));
            LOG_INFO(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit, "PCIE HOTSWAP Manager is disabled!\n")));
            rv = SOC_E_NONE;
        }
    }
#endif
    return rv;
}
