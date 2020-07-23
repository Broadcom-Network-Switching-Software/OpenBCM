/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE EYESCAN
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_PORT
#include <shared/bsl.h>
#include <soc/eyescan.h>
#include <shared/alloc.h>
#include <bcm_int/common/debug.h>
#include <bcm/error.h>
#include <bcm/stat.h>

#include <soc/dfe/cmn/mbcm.h>
#include <soc/dfe/cmn/dfe_config_defs.h>
#include <soc/dfe/cmn/dfe_config_imp_defs.h>


static uint64* dfe_saved_counter_1[BCM_LOCAL_UNITS_MAX];
static uint64* dfe_saved_counter_2[BCM_LOCAL_UNITS_MAX];

STATIC int
_dfe_eyescan_mac_prbs_counter_clear(int unit, soc_port_t port)
{
    int  rc, err_count;
    BCMDNX_INIT_FUNC_DEFS;

   rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_rx_status_get,(unit, port, 1, &err_count));
   BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_prbs_counter_get(int unit, soc_port_t port, uint32* err_count)
{
    int  rc;
    BCMDNX_INIT_FUNC_DEFS;

   rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_prbs_rx_status_get,(unit, port, 1, (int*)err_count));
   BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_crc_counter_clear(int unit, soc_port_t port)
{
    int  rc;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_stat_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_stat_get(unit, port, snmpBcmRxCrcErrors, &(dfe_saved_counter_1[unit][port]));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_crc_counter_get(int unit, soc_port_t port, uint32* err_count)
{
    int  rc;
    uint64 data;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_stat_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_stat_get(unit, port, snmpBcmRxCrcErrors, &data);
    BCMDNX_IF_ERR_EXIT(rc);

    COMPILER_64_SUB_64(data, dfe_saved_counter_1[unit][port]);
   *err_count =  COMPILER_64_LO(data);
exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_ber_counter_clear(int unit, soc_port_t port)
{
    int  rc, counter_type_1, counter_type_2;
    int pcs;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_port_control_get(unit, port, bcmPortControlPCS, &pcs);
    BCMDNX_IF_ERR_EXIT(rc);

    switch(pcs) {
    case soc_dcmn_port_pcs_64_66_bec:
        counter_type_1 = snmpBcmRxBecCrcErrors;
        counter_type_2 = snmpBcmRxBecRxFault;
        break;
    case soc_dcmn_port_pcs_8_9_legacy_fec:
    case soc_dcmn_port_pcs_64_66_fec:
        counter_type_1 = snmpBcmRxFecCorrectable;
        counter_type_2 = snmpBcmRxFecUncorrectable;
        break;
    case soc_dcmn_port_pcs_8_10:
        counter_type_1 = snmpBcmRxDisparityErrors;
        counter_type_2 = snmpBcmRxCodeErrors;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unsupported PCS")));
        break;
    }

    rc = bcm_stat_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_stat_get(unit, port, counter_type_1, &(dfe_saved_counter_1[unit][port]));
    BCMDNX_IF_ERR_EXIT(rc);
    rc = bcm_stat_get(unit, port, counter_type_2, &(dfe_saved_counter_2[unit][port]));
    BCMDNX_IF_ERR_EXIT(rc);

exit:
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_dfe_eyescan_mac_ber_counter_get(int unit, soc_port_t port, uint32* err_count)
{
    int  rc, counter_type_1, counter_type_2;
    int pcs;
    uint64 data1, data2;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_port_control_get(unit, port, bcmPortControlPCS, &pcs);
    BCMDNX_IF_ERR_EXIT(rc);

    switch(pcs) {
    case soc_dcmn_port_pcs_64_66_bec:
        counter_type_1 = snmpBcmRxBecCrcErrors;
        counter_type_2 = snmpBcmRxBecRxFault;
        break;
    case soc_dcmn_port_pcs_8_9_legacy_fec:
    case soc_dcmn_port_pcs_64_66_fec:
        counter_type_1 = snmpBcmRxFecCorrectable;
        counter_type_2 = snmpBcmRxFecUncorrectable;
        break;
    case soc_dcmn_port_pcs_8_10:
        counter_type_1 = snmpBcmRxDisparityErrors;
        counter_type_2 = snmpBcmRxCodeErrors;
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Unsupported PCS")));
        break;
    }

    rc = bcm_stat_sync(unit);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = bcm_stat_get(unit, port, counter_type_1, &data1);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = bcm_stat_get(unit, port, counter_type_2, &data2);
    BCMDNX_IF_ERR_EXIT(rc);

    COMPILER_64_SUB_64(data1, dfe_saved_counter_1[unit][port]);
    COMPILER_64_SUB_64(data2, dfe_saved_counter_2[unit][port]);
   *err_count =  COMPILER_64_LO(data1) + COMPILER_64_LO(data2);
exit:
    BCMDNX_FUNC_RETURN; 
}

int 
bcm_dfe_eyescan_init(int unit)
{
    int  rv;
    soc_port_phy_eyescan_counter_cb_t cbs;
    int nof_links;
    BCMDNX_INIT_FUNC_DEFS;

    nof_links = SOC_DFE_DEFS_GET(unit, nof_links);

    BCMDNX_ALLOC(dfe_saved_counter_1[unit], sizeof(uint64)*nof_links, "Eyescan counters set1");
    BCMDNX_ALLOC(dfe_saved_counter_2[unit], sizeof(uint64)*nof_links, "Eyescan counters set2");

    cbs.clear_func = _dfe_eyescan_mac_prbs_counter_clear;
    cbs.get_func = _dfe_eyescan_mac_prbs_counter_get;
    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterPrbsMac, &cbs);
    BCMDNX_IF_ERR_EXIT(rv);

    cbs.clear_func = _dfe_eyescan_mac_crc_counter_clear;
    cbs.get_func = _dfe_eyescan_mac_crc_counter_get;
    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterCrcMac, &cbs);
    BCMDNX_IF_ERR_EXIT(rv);

    cbs.clear_func = _dfe_eyescan_mac_ber_counter_clear;
    cbs.get_func = _dfe_eyescan_mac_ber_counter_get;
    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterBerMac, &cbs);
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN; 

}
 
int 
bcm_dfe_eyescan_deinit(int unit)
{
    int  rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterPrbsMac, NULL);
    BCMDNX_IF_ERR_CONT(rv);

    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterCrcMac, NULL);
    BCMDNX_IF_ERR_CONT(rv);

    rv = soc_port_phy_eyescan_counter_register(unit, socPortPhyEyescanCounterBerMac, NULL);
    BCMDNX_IF_ERR_CONT(rv);

    BCM_FREE(dfe_saved_counter_1[unit]);
    BCM_FREE(dfe_saved_counter_2[unit]);

    BCMDNX_FUNC_RETURN; 

}

#undef _ERR_MSG_MODULE_NAME

