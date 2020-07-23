/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PFC implemenation for Tomahawk3
 */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/pfc.h>
#include <bcm_int/esw/port.h>
#include <soc/pfc.h>
#include <soc/feature.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/tomahawk.h>
#include <soc/dma.h>
#include <soc/mmu_config.h>
#include <soc/init/tomahawk3_idb_init.h>

#define TH3_PFC_PRIORITY_GROUP_ID_MIN 0
#define TH3_PFC_PRIORITY_GROUP_ID_MAX 7
#define TH3_PORT_PG_ENABLE_MASK 0xff



/*
 * Function:
 *      bcm_th3_port_priority_group_config_set
 * Purpose:
 *      Set the port priority group configuration.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      priority_group - (IN) priority group id.
 *      prigrp_config  - (IN) structure describes port PG configuration.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_port_priority_group_config_set(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config)
{
    bcm_port_t port;
    uint32 rval, pri_bmp;

    if(!soc_feature(unit, soc_feature_priority_flow_control)) {
        return BCM_E_UNAVAIL;
    }
    if ((priority_group < TH3_PFC_PRIORITY_GROUP_ID_MIN) ||
        (priority_group > TH3_PFC_PRIORITY_GROUP_ID_MAX)) {
        return BCM_E_PARAM;
    }
    if (prigrp_config == NULL) {
        return BCM_E_PARAM;
    }
    /* The following fields should not used in TH3 */
    if (prigrp_config->shared_pool_xoff_enable ||
        prigrp_config->shared_pool_discard_enable ||
        prigrp_config->priority_enable_vector_mask) {
        return BCM_E_UNAVAIL;
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_cosq_localport_resolve(unit, gport, &port));
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    SOC_IF_ERROR_RETURN(READ_MMU_THDI_ING_PORT_CONFIGr(unit, port, &rval));
    pri_bmp = soc_reg_field_get(unit, MMU_THDI_ING_PORT_CONFIGr, rval,
                                PFC_PG_ENABLEf);

    if (prigrp_config->pfc_transmit_enable) {
        pri_bmp |= (1 << priority_group);
    } else {
        pri_bmp &= ~(1 << priority_group);
    }

    soc_reg_field_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                                PFC_PG_ENABLEf, pri_bmp);
    BCM_IF_ERROR_RETURN(WRITE_MMU_THDI_ING_PORT_CONFIGr(unit, port, rval));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th3_port_priority_group_config_get
 * Purpose:
 *      Get the port priority group configuration.
 * Parameters:
 *      unit       - (IN) device id.
 *      gport     - (IN) generic port.
 *      priority_group - (IN) priority group id.
 *      prigrp_config  - (OUT) structure describes port PG configuration.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_port_priority_group_config_get(int unit, bcm_gport_t gport,
    int priority_group, bcm_port_priority_group_config_t *prigrp_config)
{
    bcm_port_t port;
    uint32 rval, pri_bmp;

    if (!soc_feature(unit, soc_feature_priority_flow_control)) {
        return BCM_E_UNAVAIL;
    }
    if ((priority_group < TH3_PFC_PRIORITY_GROUP_ID_MIN) ||
        (priority_group > TH3_PFC_PRIORITY_GROUP_ID_MAX)) {
        return BCM_E_PARAM;
    }
    if (prigrp_config == NULL) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_cosq_localport_resolve(unit, gport, &port));
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    SOC_IF_ERROR_RETURN(READ_MMU_THDI_ING_PORT_CONFIGr(unit, port, &rval));
    pri_bmp = soc_reg_field_get(unit, MMU_THDI_ING_PORT_CONFIGr, rval,
                                PFC_PG_ENABLEf);
    if (pri_bmp & (1U << priority_group)) {
        prigrp_config->pfc_transmit_enable = 1;
    } else {
        prigrp_config->pfc_transmit_enable = 0;
    }

    return BCM_E_NONE;
}
/*
 * Function:
        bcm_th3_mmu_pfc_tx_config_set
* Purpose:
 *      Enable/diable PFC tx.
 * Parameters:
 *      unit       - (IN) device id.
 *      port     - (IN) logical port number.
 *      pfc_enable - (IN) PFC enable, 0: disable, 1: enable.
 *      pri_bmp  - (OUT) priority Group Bitmap.
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_th3_mmu_pfc_tx_config_set(int unit, bcm_port_t port, int pfc_enable,
                              uint32 pri_bmp)
{
    soc_reg_t reg = INVALIDr;
    uint32 rval;

    /* only 8 bits are needed, higher-position bits are masked out */
    pri_bmp = pri_bmp & TH3_PORT_PG_ENABLE_MASK;
    reg = MMU_THDI_ING_PORT_CONFIGr;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));

    soc_reg_field_set(unit, reg, &rval, PFC_PG_ENABLEf,
            pfc_enable ? pri_bmp : 0);

    soc_reg_field_set(unit, reg, &rval, PAUSE_ENABLEf,
            pfc_enable ? 0 : 1);

    SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, port, 0, rval));

    if (IS_CD_PORT(unit, port)) {
        soc_info_t *si = &SOC_INFO(unit);
        uint64 rval64;
        int pipe, pm_num, phy_port, subp;
        const soc_reg_t obm_fc_config_regs[_TH3_PIPES_PER_DEV]
           [_TH3_PBLKS_PER_PIPE] =
           {{IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE0r, IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE0r,
             IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE0r, IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE0r},
            {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE1r, IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE1r,
             IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE1r, IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE1r},
            {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE2r, IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE2r,
             IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE2r, IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE2r},
            {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE3r, IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE3r,
             IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE3r, IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE3r},
            {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE4r, IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE4r,
             IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE4r, IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE4r},
            {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE5r, IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE5r,
             IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE5r, IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE5r},
            {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE6r, IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE6r,
             IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE6r, IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE6r},
            {IDB_OBM0_FLOW_CONTROL_CONFIG_PIPE7r, IDB_OBM1_FLOW_CONTROL_CONFIG_PIPE7r,
             IDB_OBM2_FLOW_CONTROL_CONFIG_PIPE7r, IDB_OBM3_FLOW_CONTROL_CONFIG_PIPE7r},
        };

        phy_port = si->port_l2p_mapping[port];
        pipe = soc_tomahawk3_get_pipe_from_phy_port(phy_port);
        pm_num = soc_tomahawk3_get_pipe_pm_from_phy_port(phy_port);
        subp = soc_tomahawk3_get_subp_from_phy_port(phy_port);
        reg = obm_fc_config_regs[pipe][pm_num];
        COMPILER_64_SET(rval64, 0, 0);
        SOC_IF_ERROR_RETURN
            (soc_reg_rawport_get(unit, reg, REG_PORT_ANY,
                                 subp, &rval64));
        soc_reg64_field32_set(unit, reg,
                              &rval64, FC_TYPEf, pfc_enable);
        SOC_IF_ERROR_RETURN
            (soc_reg_rawport_set(unit, reg, REG_PORT_ANY,
                                 subp, rval64));
     }
     return BCM_E_NONE;
}

/*
 * Function:
        bcm_th3_mmu_pfc_rx_config_set
* Purpose:
 *      Enable/diable PFC rx.
 * Parameters:
 *      unit       - (IN) device id.
 *      port     - (IN) logical port number.
 *      pfc_enable - (IN) PFC enable, 0: disable, 1: enable.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th3_mmu_pfc_rx_config_set(int unit, bcm_port_t port, int pfc_enable)
{
    uint32 rval;
    soc_reg_t reg = MMU_INTFI_EGR_PORT_CFGr;

    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));

    soc_reg_field_set(unit, reg, &rval, DISABLE_Q_FCf,
            pfc_enable ? 0 : 1);

    SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, port, 0, rval));
    return BCM_E_NONE;
}

/*
 * Function:
        bcm_tomahawk3_cosq_pfc_class_config_profile_get
* Purpose:
 *      Get an entire PFC class (RX) profile, tomahawk3 dispatch function
 * Parameters:
 *      unit       - (IN) device id.
 *      profile_id     - (IN) Profile ID.
 *      max_count - (IN) Maximum number of elements in config array
 *      config_array - (OUT) PFC class config array
 *      count   - (OUT) actual size of array
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tomahawk3_cosq_pfc_class_config_profile_get(
    int unit,
    int profile_id,
    int max_count,
    bcm_cosq_pfc_class_map_config_t *config_array,
    int *count)
{
    return (soc_th3_pfc_rx_priority_mapping_profile_get
             (unit, profile_id, max_count,
                (soc_cosq_pfc_class_map_config_t *) config_array, count));
}

/*
 * Function:
        bcm_tomahawk3_cosq_pfc_class_config_profile_set
* Purpose:
 *      Set an entire PFC class (RX) profile, tomahawk3 dispatch function.
 * Parameters:
 *      unit       - (IN) device id.
 *      profile_id     - (IN) Profile ID.
 *      count   - (IN) actual size of array
 *      config_array - (IN) PFC class config array
 * Returns:
 *      BCM_E_XXX
 */

int
bcm_tomahawk3_cosq_pfc_class_config_profile_set(
    int unit,
    int profile_id,
    int count,
    bcm_cosq_pfc_class_map_config_t *config_array)
{

    return (soc_th3_pfc_rx_priority_mapping_profile_set
                (unit, profile_id,
                    (soc_cosq_pfc_class_map_config_t *) config_array, count));
}

/*
 * Function:
        bcm_tomahawk3_cosq_priority_group_pfc_priority_mapping_profile_get
* Purpose:
 *      Get a PFC-tx priority to PG mapping profile, tomahawk3 dispatch function
 * Parameters:
 *      unit       - (IN) device id.
 *      profile_id     - (IN) Profile ID.
 *      array_max - (IN) Maximum number of elements in PG array
 *      config_array - (OUT) PG array, indexed by PFC priority
 *      array_count   - (OUT) actual size of array
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_priority_group_pfc_priority_mapping_profile_get(
    int unit,
    int profile_id,
    int array_max,
    int *pg_array,
    int *array_count)
{
    return (soc_th3_pfc_tx_priority_mapping_profile_op(unit, profile_id,
                array_max, SOC_TH3_PFC_PROFILE_OP_GET, array_count, pg_array));
}

/*
 * Function:
        bcm_tomahawk3_cosq_priority_group_pfc_priority_mapping_profile_set
* Purpose:
 *      Set a PFC-tx priority to PG mapping profile, tomahawk3 dispatch function
 * Parameters:
 *      unit       - (IN) device id.
 *      profile_id     - (IN) Profile ID.
 *      array_count   - (IN) actual size of array
 *      config_array - (IN) PG array, indexed by PFC priority

 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_priority_group_pfc_priority_mapping_profile_set(
    int unit,
    int profile_id,
    int array_count,
    int *pg_array)
{
    return (soc_th3_pfc_tx_priority_mapping_profile_op(unit, profile_id,
                0, SOC_TH3_PFC_PROFILE_OP_SET, &array_count, pg_array));
}


/*
 * Function:
        bcm_tomahawk3_cosq_port_optimized_pfc_group_num_get
 * Purpose:
 *      Get number of optimized PFC groups on a port, tomahawk3 dispatch function
 * Parameters:
 *      unit    - (IN) device id.
 *      port    - (IN) logical port number.
 *      count   - (OUT) number of optimized PFC groups supported.
 *
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_port_optimized_pfc_group_num_get(
    int unit,
    bcm_port_t port,
    int *count)
{
    return (soc_th3_pfc_num_optimized_group_get(unit, port, count));
}
#endif
