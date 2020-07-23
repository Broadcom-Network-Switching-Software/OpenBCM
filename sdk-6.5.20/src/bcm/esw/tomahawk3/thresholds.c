/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Thresholds API implemenation for Tomahawk3
 */
#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <bcm_int/esw/pfc.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/tomahawk3.h>
#include <soc/feature.h>
#include <soc/drv.h>
#include <soc/types.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/debug.h>
#include <soc/tomahawk3.h>
#include <soc/dma.h>
#include <soc/mmu_config.h>


/*
 * Function:
        _bcm_th3_cosq_port_inp_pri_to_pg_profile_set
* Purpose:
 *      Set the profile for Input priortty to PG mapping for a port
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) Port number.
 *      profile_id - (IN) Profile index
 * Returns      :
 *      BCM_E_XXX
 */

int
_bcm_th3_cosq_port_inp_pri_to_pg_profile_set(
    int unit,
    bcm_port_t port,
    int profile_id)
{
    uint32 rval;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if ((profile_id < 0) || (profile_id >= _TH3_MMU_NUM_MAX_PROFILES)) {
        return BCM_E_PARAM;
    }

    rval = 0;
    BCM_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_get(unit, MMU_THDI_ING_PORT_CONFIGr,
                                    -1, port, 0, &rval));
    /* soc_reg_field_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                      PG_PROFILE_SELf, buf_port->prigroup_profile_idx); */
    soc_reg_field_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                 INPPRI_PROFILE_SELf, profile_id);
    BCM_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDI_ING_PORT_CONFIGr,
                                    -1, port, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
        _bcm_th3_cosq_port_inp_pri_to_pg_profile_get
* Purpose:
 *      Get the profile for Input priortty to PG mapping for a port
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) Port number.
 *      profile_id - (IN) Profile index
 * Returns      :
 *      BCM_E_XXX
 */

int
_bcm_th3_cosq_port_inp_pri_to_pg_profile_get(
    int unit,
    bcm_port_t port,
    int *profile_id)
{
    uint32 rval;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (profile_id == NULL) {
        return BCM_E_PARAM;
    }

    rval = 0;
    BCM_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_get(unit, MMU_THDI_ING_PORT_CONFIGr,
                                    -1, port, 0, &rval));
    *profile_id = soc_reg_field_get(unit, MMU_THDI_ING_PORT_CONFIGr, rval,
                                   INPPRI_PROFILE_SELf);

    return BCM_E_NONE;
}

/*
 * Function:
        _bcm_th3_cosq_port_pg_profile_set
* Purpose:
 *      Set the profile for PG to SPID/HPID/PFCPRI mapping for a port
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) Port number.
 *      profile_id - (IN) Profile index
 * Returns      :
 *      BCM_E_XXX
 */

int
_bcm_th3_cosq_port_pg_profile_set(
    int unit,
    bcm_port_t port,
    int profile_id)
{
    uint32 rval;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if ((profile_id < 0) || (profile_id >= _TH3_MMU_NUM_MAX_PROFILES)) {
        return BCM_E_PARAM;
    }

    rval = 0;
    BCM_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_get(unit, MMU_THDI_ING_PORT_CONFIGr,
                                    -1, port, 0, &rval));
    soc_reg_field_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                      PG_PROFILE_SELf, profile_id);
    BCM_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, MMU_THDI_ING_PORT_CONFIGr,
                                    -1, port, 0, rval));

    return BCM_E_NONE;
}

/*
 * Function:
        _bcm_th3_cosq_port_pg_profile_get
* Purpose:
 *      Get the profile for PG to SPID/HPID/PFCPRI mapping for a port
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) Port number.
 *      profile_id - (OUT) Profile index
 * Returns      :
 *      BCM_E_XXX
 */

int
_bcm_th3_cosq_port_pg_profile_get(
    int unit,
    bcm_port_t port,
    int *profile_id)
{
    uint32 rval;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    if (profile_id == NULL) {
        return BCM_E_PARAM;
    }

    rval = 0;
    BCM_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_get(unit, MMU_THDI_ING_PORT_CONFIGr,
                                    -1, port, 0, &rval));
    *profile_id = soc_reg_field_get(unit, MMU_THDI_ING_PORT_CONFIGr, rval,
                                   PG_PROFILE_SELf);

    return BCM_E_NONE;
}

/*
 * Function:
        bcm_tomahawk3_cosq_priority_group_mapping_profile_set
* Purpose:
 *  Set a profile for the following mappings: Input Pri to priority group UC
 *                                            Input Pri to priority group MC
 *                                            Priority Group to Service Pool
 *                                            Priority Group to Headroom Pool
 * Parameters:
 *      unit       - (IN) device id.
 *      profile_index     - (IN) Profile index.
 *      type       - (IN) Mapping type
 *      array_count   - (IN) actual size of array arg
 *      arg - (IN) Value of ouput to mapping type
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_priority_group_mapping_profile_set(
    int unit,
    int profile_index,
    bcm_cosq_priority_group_mapping_profile_type_t type,
    int array_count,
    int* arg)
{
    int i;
    uint64 rval;
    soc_field_t field_pri[_TH3_MMU_NUM_INT_PRI] = {
            INPPRI0_PGf, INPPRI1_PGf, INPPRI2_PGf,
            INPPRI3_PGf, INPPRI4_PGf, INPPRI5_PGf,
            INPPRI6_PGf, INPPRI7_PGf, INPPRI8_PGf,
            INPPRI9_PGf, INPPRI10_PGf, INPPRI11_PGf,
            INPPRI12_PGf, INPPRI13_PGf, INPPRI14_PGf,
            INPPRI15_PGf};
    soc_field_t field_hpid[_TH3_MMU_NUM_PG] = {
            PG0_HPIDf, PG1_HPIDf, PG2_HPIDf,
            PG3_HPIDf, PG4_HPIDf, PG5_HPIDf,
            PG6_HPIDf, PG7_HPIDf};
    soc_field_t field_spid[_TH3_MMU_NUM_PG] = {
            PG0_SPIDf, PG1_SPIDf, PG2_SPIDf,
            PG3_SPIDf, PG4_SPIDf, PG5_SPIDf,
            PG6_SPIDf, PG7_SPIDf};

    if ((profile_index < 0) || (profile_index >= _TH3_MMU_NUM_MAX_PROFILES)) {
        return BCM_E_PARAM;
    }

    if (arg == NULL) {
        return SOC_E_PARAM;
    }

    switch (type) {
        case bcmCosqInputPriPriorityGroupUcMapping:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, MMU_THDI_UC_INPPRI_PG_PROFILEr,
                               REG_PORT_ANY, profile_index, &rval));
            if ((array_count <= 0) || (array_count > _TH3_MMU_NUM_INT_PRI)) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < array_count; i++) {
                if (arg[i] != -1) {
                    if ((arg[i] < 0) || (arg[i] >= _TH3_MMU_NUM_PG)) {
                        return BCM_E_PARAM;
                    }
                } else {
                    continue;
                }
                soc_reg64_field32_set(unit, MMU_THDI_UC_INPPRI_PG_PROFILEr, &rval,
                    field_pri[i], arg[i]);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, MMU_THDI_UC_INPPRI_PG_PROFILEr,
                               REG_PORT_ANY, profile_index, rval));
            break;
        case bcmCosqInputPriPriorityGroupMcMapping:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, MMU_THDI_NONUC_INPPRI_PG_PROFILEr,
                               REG_PORT_ANY, profile_index, &rval));
            if ((array_count <= 0) || (array_count > _TH3_MMU_NUM_INT_PRI)) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < array_count; i++) {
                if (arg[i] != -1) {
                    if ((arg[i] < 0) || (arg[i] >= _TH3_MMU_NUM_PG)) {
                        return BCM_E_PARAM;
                    }
                } else {
                    continue;
                }
                soc_reg64_field32_set(unit, MMU_THDI_NONUC_INPPRI_PG_PROFILEr, &rval,
                    field_pri[i], arg[i]);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, MMU_THDI_NONUC_INPPRI_PG_PROFILEr,
                               REG_PORT_ANY, profile_index, rval));
            break;
        case bcmCosqPriorityGroupServicePoolMapping:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY,
                               profile_index, &rval));
            if ((array_count <= 0) || (array_count > _TH3_MMU_NUM_PG)) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < array_count; i++) {
                if (arg[i] != -1) {
                    if ((arg[i] < 0) || (arg[i] >= _TH3_MMU_NUM_POOL)) {
                        return BCM_E_PARAM;
                    }
                } else {
                    continue;
                }
                soc_reg64_field32_set(unit, MMU_THDI_PG_PROFILEr, &rval,
                    field_spid[i], arg[i]);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY,
                               profile_index, rval));
            break;
        case bcmCosqPriorityGroupHeadroomPoolMapping:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY,
                               profile_index, &rval));
            if ((array_count <= 0) || (array_count > _TH3_MMU_NUM_PG)) {
                return BCM_E_PARAM;
            }
            for (i = 0; i < array_count; i++) {
                if (arg[i] != -1) {
                    if ((arg[i] < 0) || (arg[i] >= _TH3_MMU_NUM_POOL)) {
                        return BCM_E_PARAM;
                    }
                } else {
                    continue;
                }
                soc_reg64_field32_set(unit, MMU_THDI_PG_PROFILEr, &rval,
                    field_hpid[i], arg[i]);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg64_set(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY,
                               profile_index, rval));
            break;
        default:
            return BCM_E_PARAM;

    }

    return BCM_E_NONE;
}


/*
 * Function:
        bcm_tomahawk3_cosq_priority_group_mapping_profile_get
* Purpose:
 *  Set a profile for the following mappings: Input Pri to priority group UC
 *                                            Input Pri to priority group MC
 *                                            Priority Group to Service Pool
 *                                            Priority Group to Headroom Pool
 * Parameters:
 *      unit       - (IN) device id.
 *      profile_index     - (IN) Profile index.
 *      type       - (IN) Mapping type
 *      array_max   - (IN) Max size of array
 *      array_count - (OUT) actual size of array
 *      arg - (OUT) Value of ouput to mapping type
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_priority_group_mapping_profile_get(
    int unit,
    int profile_index,
    bcm_cosq_priority_group_mapping_profile_type_t type,
    int array_max,
    int* arg,
    int* array_count)
{
    int i;
    uint64 rval;
    soc_field_t field_pri[_TH3_MMU_NUM_INT_PRI] = {
            INPPRI0_PGf, INPPRI1_PGf, INPPRI2_PGf,
            INPPRI3_PGf, INPPRI4_PGf, INPPRI5_PGf,
            INPPRI6_PGf, INPPRI7_PGf, INPPRI8_PGf,
            INPPRI9_PGf, INPPRI10_PGf, INPPRI11_PGf,
            INPPRI12_PGf, INPPRI13_PGf, INPPRI14_PGf,
            INPPRI15_PGf};
    soc_field_t field_hpid[_TH3_MMU_NUM_PG] = {
            PG0_HPIDf, PG1_HPIDf, PG2_HPIDf,
            PG3_HPIDf, PG4_HPIDf, PG5_HPIDf,
            PG6_HPIDf, PG7_HPIDf};
    soc_field_t field_spid[_TH3_MMU_NUM_PG] = {
            PG0_SPIDf, PG1_SPIDf, PG2_SPIDf,
            PG3_SPIDf, PG4_SPIDf, PG5_SPIDf,
            PG6_SPIDf, PG7_SPIDf};

    if ((profile_index < 0) || (profile_index >= _TH3_MMU_NUM_MAX_PROFILES)) {
        return BCM_E_PARAM;
    }

    if (((array_max == 0) && (arg != NULL)) ||
        ((array_max != 0) && (arg == NULL))) {
        return SOC_E_PARAM;
    }

    *array_count = 0;
    switch (type) {
        case bcmCosqInputPriPriorityGroupUcMapping:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, MMU_THDI_UC_INPPRI_PG_PROFILEr,
                               REG_PORT_ANY, profile_index, &rval));
            for (i = 0; ((i < _TH3_MMU_NUM_INT_PRI) && (i < array_max)); i++) {
                arg[i] = soc_reg64_field32_get(unit, MMU_THDI_UC_INPPRI_PG_PROFILEr,
                        rval, field_pri[i]);

            }
            *array_count = i;
            break;
        case bcmCosqInputPriPriorityGroupMcMapping:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, MMU_THDI_NONUC_INPPRI_PG_PROFILEr,
                               REG_PORT_ANY, profile_index, &rval));
            for (i = 0; ((i < _TH3_MMU_NUM_INT_PRI) && (i < array_max)); i++) {
                arg[i] = soc_reg64_field32_get(unit, MMU_THDI_NONUC_INPPRI_PG_PROFILEr,
                        rval, field_pri[i]);
            }
            *array_count = i;
            break;
        case bcmCosqPriorityGroupServicePoolMapping:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY,
                               profile_index, &rval));
            for (i = 0; ((i < _TH3_MMU_NUM_PG) && (i < array_max)); i++) {
                arg[i] = soc_reg64_field32_get(unit, MMU_THDI_PG_PROFILEr,
                        rval, field_spid[i]);
            }
            *array_count = i;
            break;
        case bcmCosqPriorityGroupHeadroomPoolMapping:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg64_get(unit, MMU_THDI_PG_PROFILEr, REG_PORT_ANY,
                               profile_index, &rval));
            for (i = 0; ((i < _TH3_MMU_NUM_PG) && (i < array_max)); i++) {
                arg[i] = soc_reg64_field32_get(unit, MMU_THDI_PG_PROFILEr,
                        rval, field_hpid[i]);
            }
            *array_count = i;
            break;
        default:
            return BCM_E_PARAM;

    }

    return BCM_E_NONE;
}

/*
 * Function:
        bcm_tomahawk3_cosq_service_pool_override_set
* Purpose:
 *      Set service pool overrides for CPU, Multicast and Mirror
 * Parameters:
 *      unit       - (IN) device id.
 *      type       - (IN) Service pool override type
 *      service_pool   - (IN) Service pool id
 *      enable - (IN) Enable/Disable service pool override
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_service_pool_override_set(
    int unit,
    bcm_cosq_service_pool_override_type_t type,
    bcm_service_pool_id_t service_pool,
    int enable)
{
    uint64 rval;

    if ((service_pool < 0) || (service_pool >= _TH3_MMU_NUM_POOL)) {
        return BCM_E_PARAM;
    }

    if ((enable < 0) || (enable > 1)) {
        return BCM_E_PARAM;
    }
    switch (type) {
        case bcmCosqServicePoolOverrideCpu:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_CPU_SPID_OVERRIDE_CTRLr,
                             REG_PORT_ANY, -1, &rval));
            soc_reg64_field32_set(unit, MMU_THDI_CPU_SPID_OVERRIDE_CTRLr, &rval,
                    SPIDf, service_pool);
            soc_reg64_field32_set(unit, MMU_THDI_CPU_SPID_OVERRIDE_CTRLr, &rval,
                    ENABLEf, enable);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit,  MMU_THDI_CPU_SPID_OVERRIDE_CTRLr,
                             REG_PORT_ANY, -1, rval));
            break;
        case bcmCosqServicePoolOverrideMcPkt:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_MC_SPID_OVERRIDE_CTRLr,
                             REG_PORT_ANY, -1, &rval));
            soc_reg64_field32_set(unit, MMU_THDI_MC_SPID_OVERRIDE_CTRLr, &rval,
                    SPIDf, service_pool);
            soc_reg64_field32_set(unit, MMU_THDI_MC_SPID_OVERRIDE_CTRLr, &rval,
                    ENABLEf, enable);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit,  MMU_THDI_MC_SPID_OVERRIDE_CTRLr,
                             REG_PORT_ANY, -1, rval));
            break;
        case bcmCosqServicePoolOverrideMirror:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr,
                             REG_PORT_ANY, -1, &rval));
            soc_reg64_field32_set(unit, MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr, &rval,
                    SPIDf, service_pool);
            soc_reg64_field32_set(unit, MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr, &rval,
                    ENABLEf, enable);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit,  MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr,
                             REG_PORT_ANY, -1, rval));
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


/*
 * Function:
        bcm_tomahawk3_cosq_service_pool_override_get
* Purpose:
 *      Set service pool overrides for CPU, Multicast and Mirror
 * Parameters:
 *      unit       - (IN) device id.
 *      type       - (IN) Service pool override type
 *      service_pool   - (OUT) Service pool id
 *      enable - (OUT) Enable/Disable service pool override
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_tomahawk3_cosq_service_pool_override_get(
    int unit,
    bcm_cosq_service_pool_override_type_t type,
    bcm_service_pool_id_t* service_pool,
    int* enable)
{
    uint64 rval;

    switch (type) {
        case bcmCosqServicePoolOverrideCpu:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_CPU_SPID_OVERRIDE_CTRLr,
                             REG_PORT_ANY, -1, &rval));
            *service_pool = soc_reg64_field32_get(unit,
                    MMU_THDI_CPU_SPID_OVERRIDE_CTRLr, rval, SPIDf);
            *enable = soc_reg64_field32_get(unit,
                    MMU_THDI_CPU_SPID_OVERRIDE_CTRLr, rval, ENABLEf);
            break;
        case bcmCosqServicePoolOverrideMcPkt:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_MC_SPID_OVERRIDE_CTRLr,
                             REG_PORT_ANY, -1, &rval));
            *service_pool = soc_reg64_field32_get(unit,
                    MMU_THDI_MC_SPID_OVERRIDE_CTRLr, rval, SPIDf);
            *enable = soc_reg64_field32_get(unit,
                    MMU_THDI_MC_SPID_OVERRIDE_CTRLr, rval, ENABLEf);
            break;
        case bcmCosqServicePoolOverrideMirror:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr,
                             REG_PORT_ANY, -1, &rval));
            *service_pool = soc_reg64_field32_get(unit,
                    MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr, rval, SPIDf);
            *enable = soc_reg64_field32_get(unit,
                    MMU_THDI_MIRROR_SPID_OVERRIDE_CTRLr, rval, ENABLEf);
            break;
        default:
            return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

/*
 * Function:
        bcm_tomahawk3_cosq_port_priority_group_property_set
* Purpose:
 *      Get per port per priority group properties : Lossless, Pause
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) Port number.
 *      priority_group_id - (IN) Priority group
 *      type       - (IN) Property type (Lossless or Pause enable)
 *      arg        - (IN) Value based on property type
 * Returns      :
 *      BCM_E_XXX
 */

int
bcm_tomahawk3_cosq_port_priority_group_property_set(
    int unit,
    bcm_port_t port,
    int priority_group_id,
    bcm_cosq_port_prigroup_control_t type,
    int arg)
{
    uint64 rval;
    int pg_lossless;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    switch (type) {
        case bcmCosqPriorityGroupLossless:
            if ((priority_group_id < 0) ||
                (priority_group_id >= _TH3_MMU_NUM_PG)) {
                return BCM_E_PARAM;
            }
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_ING_PORT_CONFIGr, port, -1, &rval));
            pg_lossless = soc_reg64_field32_get(unit,
                    MMU_THDI_ING_PORT_CONFIGr, rval, PG_IS_LOSSLESSf);
            if (arg) {
                pg_lossless = pg_lossless | (1 << priority_group_id);
            } else {
                pg_lossless &= (0xff ^ (1 << priority_group_id));
            }
            soc_reg64_field32_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                      PG_IS_LOSSLESSf, pg_lossless);
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, MMU_THDI_ING_PORT_CONFIGr, port, -1, rval));
            break;
        case bcmCosqPauseEnable:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_ING_PORT_CONFIGr, port, -1, &rval));
            soc_reg64_field32_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                      PAUSE_ENABLEf, arg);
            /* Disable PFC if pause is enabled */
            if (arg == 1) {
                soc_reg64_field32_set(unit, MMU_THDI_ING_PORT_CONFIGr, &rval,
                      PFC_PG_ENABLEf, 0);
            }
            SOC_IF_ERROR_RETURN
                (soc_reg_set(unit, MMU_THDI_ING_PORT_CONFIGr, port, -1, rval));
            break;
    }

    return BCM_E_NONE;
}
/*
 * Function:
        bcm_tomahawk3_cosq_port_priority_group_property_get
* Purpose:
 *      Get per port per priority group properties : Lossless, Pause
 * Parameters:
 *      unit       - (IN) device id.
 *      port       - (IN) Port number.
 *      priority_group_id - (IN) Priority group
 *      type       - (IN) Property type (Lossless or Pause enable)
 *      arg        - (OUT) Value based on property type
 * Returns      :
 *      BCM_E_XXX
 */

int
bcm_tomahawk3_cosq_port_priority_group_property_get(
    int unit,
    bcm_port_t port,
    int priority_group_id,
    bcm_cosq_port_prigroup_control_t type,
    int *arg)
{
    uint64 rval;
    int pg_lossless;

    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }

    switch (type) {
        case bcmCosqPriorityGroupLossless:
            if ((priority_group_id < 0) ||
                (priority_group_id >= _TH3_MMU_NUM_PG)) {
                return BCM_E_PARAM;
            }
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_ING_PORT_CONFIGr, port, -1, &rval));
            pg_lossless = soc_reg64_field32_get(unit, MMU_THDI_ING_PORT_CONFIGr,
                    rval, PG_IS_LOSSLESSf);
            *arg = (pg_lossless & (1 << priority_group_id)) >> priority_group_id;
            break;
        case bcmCosqPauseEnable:
            COMPILER_64_ZERO(rval);
            SOC_IF_ERROR_RETURN
                (soc_reg_get(unit, MMU_THDI_ING_PORT_CONFIGr, port, -1, &rval));
            *arg = soc_reg64_field32_get(unit, MMU_THDI_ING_PORT_CONFIGr,
                    rval, PAUSE_ENABLEf);
            break;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_ing_pool_set(int unit, bcm_gport_t gport,
                          bcm_cos_t prigroup,
                          bcm_cosq_control_t type, int arg)
{
    if ((arg < 0) || (arg >= _TH3_MMU_NUM_POOL)) {
            return BCM_E_PARAM;
    }


    /* Expects priority group as input in Tomahawk3
     * Future enhancement is to create gport type for priority group
     */
    if ((prigroup < 0) || (prigroup >= _TH3_MMU_NUM_PG)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlIngressPool) {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_ingress_sp_set_by_pg(unit, gport, prigroup, arg));
    } else if (type == bcmCosqControlIngressHeadroomPool) {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_ingress_hdrm_pool_set_by_pg(unit, gport, prigroup, arg));
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_th3_cosq_ing_pool_get(int unit, bcm_gport_t gport,
                          bcm_cos_t prigroup,
                          bcm_cosq_control_t type, int *arg)
{
    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    /* Expects priority group as input in Tomahawk3
     * Future enhancement is to create gport type for priority group
     */
    if ((prigroup < 0) || (prigroup >= _TH3_MMU_NUM_PG)) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlIngressPool) {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_ingress_sp_get_by_pg(unit, gport, prigroup, arg));
    } else if (type == bcmCosqControlIngressHeadroomPool) {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_cosq_ingress_hdrm_pool_get_by_pg(unit, gport, prigroup, arg));
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


STATIC int
_bcm_th3_cosq_dynamic_thresh_enable_get(int unit,
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int* arg)
{
    bcm_port_t local_port;
    int startq;
    int pipe;
    int from_cos, to_cos, ci;
    int midx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int port_pg;
    /*
    soc_field_t field_pgshared_limit[_TH3_MMU_NUM_PG] = {
            PG0_SHARED_LIMITf, PG1_SHARED_LIMITf, PG2_SHARED_LIMITf,
            PG3_SHARED_LIMITf, PG4_SHARED_LIMITf, PG5_SHARED_LIMITf,
            PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};
    */
    soc_field_t field_pgshared_dynamic[_TH3_MMU_NUM_PG] = {
            PG0_SHARED_DYNAMICf, PG1_SHARED_DYNAMICf, PG2_SHARED_DYNAMICf,
            PG3_SHARED_DYNAMICf, PG4_SHARED_DYNAMICf, PG5_SHARED_DYNAMICf,
            PG6_SHARED_DYNAMICf, PG7_SHARED_DYNAMICf};


    if (!arg) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

        if (local_port < 0) {
            return BCM_E_PORT;
        }
        /* Expects that cosq is priority group in Tomahawk3
         * Future enhancement is to create gport type of priority group
         */
        if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        port_pg = cosq;

        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_SHARED_CONFIGm)[pipe];
        if (mem != INVALIDm) {
            sal_memset(&entry, 0, sizeof(entry));
            midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                             MMU_THDI_PORT_PG_SHARED_CONFIGm,0);
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
            *arg = soc_mem_field32_get(unit, mem, entry, field_pgshared_dynamic[port_pg]);
        }
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, local_port, ci,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));

            }
        }
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
        mem = MMU_THDO_QUEUE_CONFIGm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, LIMIT_DYNAMICf);
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                      _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                      &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, local_port, ci,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         NULL, &startq, NULL));

            }
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
        mem = MMU_THDO_QUEUE_CONFIGm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, LIMIT_DYNAMICf);
    }
    else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_th3_cosq_dynamic_thresh_enable_set(int unit,
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    int startq;
    int pipe;
    int from_cos, to_cos, ci;
    int midx;
    /* uint32 rval; */
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    /* soc_reg_t reg = INVALIDr; */
    int port_pg;
    soc_field_t field_pgshared_limit[_TH3_MMU_NUM_PG] = {
            PG0_SHARED_LIMITf, PG1_SHARED_LIMITf, PG2_SHARED_LIMITf,
            PG3_SHARED_LIMITf, PG4_SHARED_LIMITf, PG5_SHARED_LIMITf,
            PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};
    soc_field_t field_pgshared_dynamic[_TH3_MMU_NUM_PG] = {
            PG0_SHARED_DYNAMICf, PG1_SHARED_DYNAMICf, PG2_SHARED_DYNAMICf,
            PG3_SHARED_DYNAMICf, PG4_SHARED_DYNAMICf, PG5_SHARED_DYNAMICf,
            PG6_SHARED_DYNAMICf, PG7_SHARED_DYNAMICf};


    if (type == bcmCosqControlIngressPortPGSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
            BCM_GPORT_IS_SCHEDULER(gport) ||
            BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

        if (local_port < 0) {
            return BCM_E_PORT;
        }
        /* Expects that cosq is priority group in Tomahawk3
         * Future enhancement is to create gport type of priority group
         */
        if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
            return BCM_E_PARAM;
        }
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        port_pg = cosq;

        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_SHARED_CONFIGm)[pipe];
        if (mem != INVALIDm) {
            sal_memset(&entry, 0, sizeof(entry));
            midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                             MMU_THDI_PORT_PG_SHARED_CONFIGm,0);
            BCM_IF_ERROR_RETURN
                (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
            soc_mem_field32_set(unit, mem, entry,
                            field_pgshared_dynamic[port_pg], arg ? 1 : 0);
            if(arg) {
                /* Set default alpha value*/
                soc_mem_field32_set(unit, mem, entry, field_pgshared_limit[port_pg],
                                    SOC_TH3_COSQ_DROP_LIMIT_ALPHA_2); /* Value: 8 */
            }
            SOC_IF_ERROR_RETURN
                (soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));
        }
    } else if (type == bcmCosqControlEgressUCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, local_port, ci,
                                         _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                         NULL, &startq, NULL));

            }
        }
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
        mem = MMU_THDO_QUEUE_CONFIGm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry,
                            LIMIT_DYNAMICf, arg ? 1 : 0);
        if(arg) {
            soc_mem_field32_set(unit, mem, entry, SHARED_ALPHAf,
                               SOC_TH3_COSQ_DROP_LIMIT_ALPHA_2);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else if (type == bcmCosqControlEgressMCSharedDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                      _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                      &local_port, &startq, NULL));
        }
        else {
            if (cosq == BCM_COS_INVALID) {
                from_cos = to_cos = 0;
            } else {
                from_cos = to_cos = cosq;
            }

            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            for (ci = from_cos; ci <= to_cos; ci++) {
                BCM_IF_ERROR_RETURN
                    (_bcm_th3_cosq_index_resolve(unit, local_port, ci,
                                         _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                         NULL, &startq, NULL));

            }
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
        mem = MMU_THDO_QUEUE_CONFIGm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, LIMIT_DYNAMICf, arg ? 1 : 0);
        if(arg) {
            soc_mem_field32_set(unit, mem, entry, SHARED_ALPHAf,
                                SOC_TH3_COSQ_DROP_LIMIT_ALPHA_2);
        }
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    }
else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_alpha_set(int unit,
                       bcm_gport_t gport, bcm_cos_queue_t cosq,
                       bcm_cosq_control_drop_limit_alpha_value_t arg)
{
    bcm_port_t local_port;
    int startq, midx, pipe;
    /* uint32 rval; */
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int alpha, dynamic_thresh_mode, port_pg;
    /* int phy_port, mmu_port, local_mmu_port; */
    soc_field_t field_pgshared_limit[_TH3_MMU_NUM_PG] = {
            PG0_SHARED_LIMITf, PG1_SHARED_LIMITf, PG2_SHARED_LIMITf,
            PG3_SHARED_LIMITf, PG4_SHARED_LIMITf, PG5_SHARED_LIMITf,
            PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};


    switch(arg) {
    case bcmCosqControlDropLimitAlpha_1_128:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_128;
        break;
    case bcmCosqControlDropLimitAlpha_1_64:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_64;
        break;
    case bcmCosqControlDropLimitAlpha_1_32:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_32;
        break;
    case bcmCosqControlDropLimitAlpha_1_16:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_16;
        break;
    case bcmCosqControlDropLimitAlpha_1_8:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_8;
        break;
    case bcmCosqControlDropLimitAlpha_1_4:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_4;
        break;
    case bcmCosqControlDropLimitAlpha_1_2:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_2;
        break;
    case bcmCosqControlDropLimitAlpha_1:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1;
        break;
    case bcmCosqControlDropLimitAlpha_2:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_2;
        break;
    case bcmCosqControlDropLimitAlpha_4:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_4;
        break;
    case bcmCosqControlDropLimitAlpha_8:
        alpha = SOC_TH3_COSQ_DROP_LIMIT_ALPHA_8;
        break;
    default:
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                   bcmCosqControlEgressUCSharedDynamicEnable,
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry,
                            SHARED_ALPHAf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                   bcmCosqControlEgressMCSharedDynamicEnable,
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;

        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        soc_mem_field32_set(unit, mem, entry, SHARED_ALPHAf, alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    } else if (BCM_GPORT_IS_LOCAL(gport)) {
        /* Shared limits are not applicable for Qgroup in Tomahawk3 */
        /* _bcm_th3_cosq_dynamic_thresh_enable_get will return error for
           bcmCosqControlEgressUCQueueGroupSharedDynamicEnable, type
        */
        return BCM_E_PARAM;
    } else {
        BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                   bcmCosqControlIngressPortPGSharedDynamicEnable,
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        /* Expects that cosq is priority group in Tomahawk3
         * Future enhancement is to create gport type of priority group
         */
        if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
            return BCM_E_PARAM;
        }
        port_pg = cosq;

        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_SHARED_CONFIGm)[pipe];
        sal_memset(&entry, 0, sizeof(entry));
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                         MMU_THDI_PORT_PG_SHARED_CONFIGm,0);
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        soc_mem_field32_set(unit, mem, entry, field_pgshared_limit[port_pg], alpha);
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));
    }

    return BCM_E_NONE;
}



STATIC int
_bcm_th3_cosq_alpha_get(int unit,
                        bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_control_drop_limit_alpha_value_t *arg)
{
    bcm_port_t local_port;
    int startq, midx, pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int alpha, dynamic_thresh_mode, port_pg;
    soc_field_t field_pgshared_limit[_TH3_MMU_NUM_PG] = {
            PG0_SHARED_LIMITf, PG1_SHARED_LIMITf, PG2_SHARED_LIMITf,
            PG3_SHARED_LIMITf, PG4_SHARED_LIMITf, PG5_SHARED_LIMITf,
            PG6_SHARED_LIMITf, PG7_SHARED_LIMITf};


    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                   bcmCosqControlEgressUCSharedDynamicEnable,
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                     &local_port, &startq, NULL));
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;

        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL,
                                 startq, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, SHARED_ALPHAf);

    } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                   bcmCosqControlEgressMCSharedDynamicEnable,
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                     _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                     &local_port, &startq, NULL));
        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, SHARED_ALPHAf);

    } else {
        BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                   bcmCosqControlIngressPortPGSharedDynamicEnable,
                                   &dynamic_thresh_mode));
        if (!dynamic_thresh_mode){
            return BCM_E_CONFIG;
        }

        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
        if (local_port < 0) {
            return BCM_E_PORT;
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        /* Expects that cosq is priority group in Tomahawk3
         * Future enhancement is to create gport type of priority group
         */
        if ((cosq < 0) || (cosq >= _TH3_MMU_NUM_PG)) {
            return BCM_E_PARAM;
        }
        port_pg = cosq;

        mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_SHARED_CONFIGm)[pipe];
        sal_memset(&entry, 0, sizeof(entry));
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port,
                         MMU_THDI_PORT_PG_SHARED_CONFIGm,0);
        BCM_IF_ERROR_RETURN
            (soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        alpha = soc_mem_field32_get(unit, mem, entry, field_pgshared_limit[port_pg]);
    }

    switch(alpha) {
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_128:
        *arg = bcmCosqControlDropLimitAlpha_1_128;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_64:
        *arg = bcmCosqControlDropLimitAlpha_1_64;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_32:
        *arg = bcmCosqControlDropLimitAlpha_1_32;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_16:
        *arg = bcmCosqControlDropLimitAlpha_1_16;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_8:
        *arg = bcmCosqControlDropLimitAlpha_1_8;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_4:
        *arg = bcmCosqControlDropLimitAlpha_1_4;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1_2:
        *arg = bcmCosqControlDropLimitAlpha_1_2;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_1:
        *arg = bcmCosqControlDropLimitAlpha_1;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_2:
        *arg = bcmCosqControlDropLimitAlpha_2;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_4:
        *arg = bcmCosqControlDropLimitAlpha_4;
        break;
    case SOC_TH3_COSQ_DROP_LIMIT_ALPHA_8:
        *arg = bcmCosqControlDropLimitAlpha_8;
        break;
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_th3_cosq_egr_queue_color_limit_set
 * Purpose: Set Red and Yellow color limit per queue
 *          IF color mode dynamic: limit should be between 0-7
 *                                 0: 100% of green threshold
 *                                 1: 12.5% of green threshold
 *                                 2: 25% of green threshold
 *                                 3: 37.5% of green threshold
 *                                 4: 50% of green threshold
 *                                 5: 67.5% of green threshold
 *                                 6: 75% of green threshold
 *                                 7: 87.5% of green threshold
 *          If color mode static: Limit is number of bytes
*/

STATIC int
_bcm_th3_cosq_egr_queue_color_limit_set(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_control_t type,
                                        int arg)
{
    bcm_port_t local_port;
    int startq, pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int color_mode, granularity, max_val;

    if (arg < 0) {
        return BCM_E_PARAM;
    }
    granularity = 8;

    if ((type == bcmCosqControlEgressUCQueueRedLimit) ||
        (type == bcmCosqControlEgressUCQueueYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        if (type == bcmCosqControlEgressUCQueueRedLimit) {
            fld_limit = LIMIT_REDf;
        } else {
            fld_limit = LIMIT_YELLOWf;
        }

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, COLOR_LIMIT_MODEf);

        if (color_mode == 1) { /* Dynamic color limit */
            if (arg > 7) { /* Legal values are 0 - 7 */
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, arg);
            }
        } else { /* Static color limit */
            arg = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;

            /* Check for min/max values */
            max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
            if ((arg < 0) || ((arg/granularity) > max_val)) {
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
            }
        }
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
    } else if ((type == bcmCosqControlEgressMCQueueRedLimit) ||
               (type == bcmCosqControlEgressMCQueueYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        if (type == bcmCosqControlEgressMCQueueRedLimit) {
            fld_limit = LIMIT_REDf;
        } else {
            fld_limit = LIMIT_YELLOWf;
        }

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, COLOR_LIMIT_MODEf);

        if (color_mode == 1) { /* Dynamic color limit */
            if (arg > 7) { /* Legal values are 0 - 7 */
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, arg);
            }
        } else { /* Static color limit */
            arg = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;

            /* Check for min/max values */
            max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
            if ((arg < 0) || ((arg/granularity) > max_val)) {
                return BCM_E_PARAM;
            } else {
                soc_mem_field32_set(unit, mem, entry, fld_limit, (arg/granularity));
            }
        }
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));

    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_th3_cosq_egr_queue_color_limit_get
 * Purpose: Get Red and Yellow color limit per queue
 * Return value: IF color mode dynamic: limit should be between 0-7
 *                                 0: 100% of green threshold
 *                                 1: 12.5% of green threshold
 *                                 2: 25% of green threshold
 *                                 3: 37.5% of green threshold
 *                                 4: 50% of green threshold
 *                                 5: 67.5% of green threshold
 *                                 6: 75% of green threshold
 *                                 7: 87.5% of green threshold
 *          If color mode static: Limit is number of bytes
*/

STATIC int
_bcm_th3_cosq_egr_queue_color_limit_get(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_control_t type,
                                        int* arg)
{
    bcm_port_t local_port;
    int startq, pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int color_mode, granularity;

    if (!arg) {
        return BCM_E_PARAM;
    }

    granularity = 8;

    if ((type == bcmCosqControlEgressUCQueueRedLimit) ||
        (type == bcmCosqControlEgressUCQueueYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        if (type == bcmCosqControlEgressUCQueueRedLimit) {
            fld_limit = LIMIT_REDf;
        } else {
            fld_limit = LIMIT_YELLOWf;
        }

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, COLOR_LIMIT_MODEf);

        if (color_mode == 1) { /* Dynamic color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        } else { /* Static color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
            *arg = (*arg) * granularity * _TH3_MMU_BYTES_PER_CELL;
        }
    } else if ((type == bcmCosqControlEgressMCQueueRedLimit) ||
               (type == bcmCosqControlEgressMCQueueYellowLimit)) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        if (type == bcmCosqControlEgressMCQueueRedLimit) {
            fld_limit = LIMIT_REDf;
        } else {
            fld_limit = LIMIT_YELLOWf;
        }

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        color_mode = soc_mem_field32_get(unit, mem, entry, COLOR_LIMIT_MODEf);

        if (color_mode == 1) { /* Dynamic color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        } else { /* Static color limit */
            *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
            *arg = (*arg) * granularity * _TH3_MMU_BYTES_PER_CELL;
        }

    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


/*
 * Function: _bcm_th3_cosq_egr_queue_color_limit_mode_set
 * Purpose: Set color limit mode per queue to static (0) or
 *          dynamic (1)
*/

STATIC int
_bcm_th3_cosq_egr_queue_color_limit_mode_set(int unit, bcm_gport_t gport,
                                             bcm_cos_queue_t cosq,
                                             bcm_cosq_control_t type,
                                             int arg)
{
    bcm_port_t local_port;
    int startq, pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int enable;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = arg ? 1 : 0;

    if (type == bcmCosqControlEgressUCQueueColorLimitDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, COLOR_LIMIT_MODEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, COLOR_LIMIT_MODEf, arg);
            /* Reset the Red and Yellow limits when color limit mode is changed */
            soc_mem_field32_set(unit, mem, entry, LIMIT_REDf, 0);
            soc_mem_field32_set(unit, mem, entry, LIMIT_YELLOWf, 0);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else if (type == bcmCosqControlEgressMCQueueColorLimitDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, COLOR_LIMIT_MODEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, COLOR_LIMIT_MODEf, arg);
            /* Reset the Red and Yellow limits when color limit mode is changed */
            soc_mem_field32_set(unit, mem, entry, LIMIT_REDf, 0);
            soc_mem_field32_set(unit, mem, entry, LIMIT_YELLOWf, 0);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_th3_cosq_egr_queue_color_limit_mode_get
 * Purpose: Get color limit mode per queue.
 * Returns: 0 (static) or 1 (dynamic)
*/
STATIC int
_bcm_th3_cosq_egr_queue_color_limit_mode_get(int unit, bcm_gport_t gport,
                                             bcm_cos_queue_t cosq,
                                             bcm_cosq_control_t type,
                                             int* arg)
{
    bcm_port_t local_port;
    int startq, pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlEgressUCQueueColorLimitDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, COLOR_LIMIT_MODEf);
    } else if (type == bcmCosqControlEgressMCQueueColorLimitDynamicEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, COLOR_LIMIT_MODEf);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_th3_cosq_egr_queue_limit_enable_set(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_control_t type,
                                        int arg)
{
    bcm_port_t local_port;
    int startq, pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int enable;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = arg ? 1 : 0;

    if (type == bcmCosqControlEgressUCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, LIMIT_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, LIMIT_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else if (type == bcmCosqControlEgressMCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, LIMIT_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, LIMIT_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

STATIC int
_bcm_th3_cosq_egr_queue_limit_enable_get(int unit, bcm_gport_t gport,
                                        bcm_cos_queue_t cosq,
                                        bcm_cosq_control_t type,
                                        int* arg)
{
    bcm_port_t local_port;
    int startq, pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlEgressUCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, LIMIT_ENABLEf);
    } else if (type == bcmCosqControlEgressMCQueueLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, LIMIT_ENABLEf);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_th3_cosq_egr_queue_color_limit_enable_set
 * Purpose: Enable/Disable queue color discard thresholds for color congestion management
 * Set (1) to enable or (0) to disable
*/

STATIC int
_bcm_th3_cosq_egr_queue_color_limit_enable_set(int unit, bcm_gport_t gport,
                                             bcm_cos_queue_t cosq,
                                             bcm_cosq_control_t type,
                                             int arg)
{
    bcm_port_t local_port;
    int startq, pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    int enable;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    arg = arg ? 1 : 0;

    if (type == bcmCosqControlEgressUCQueueColorLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, COLOR_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, COLOR_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else if (type == bcmCosqControlEgressMCQueueColorLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        enable = soc_mem_field32_get(unit, mem, entry, COLOR_ENABLEf);
        if (enable != arg) {
            soc_mem_field32_set(unit, mem, entry, COLOR_ENABLEf, arg);
            BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, startq, entry));
        }
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function: _bcm_th3_cosq_egr_queue_color_limit_enable_get
 * Purpose: Get color limit enable status per queue.
 * Returns: 0 (disabled) or 1 (enabled)
*/
STATIC int
_bcm_th3_cosq_egr_queue_color_limit_enable_get(int unit, bcm_gport_t gport,
                                             bcm_cos_queue_t cosq,
                                             bcm_cosq_control_t type,
                                             int* arg)
{
    bcm_port_t local_port;
    int startq, pipe;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;

    if (arg == NULL) {
        return BCM_E_PARAM;
    }

    if (type == bcmCosqControlEgressUCQueueColorLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve
                 (unit, local_port, cosq, _BCM_TH3_COSQ_INDEX_STYLE_UCAST_QUEUE,
                  NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, COLOR_ENABLEf);
    } else if (type == bcmCosqControlEgressMCQueueColorLimitEnable) {
        if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
            return BCM_E_PARAM;
        } else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            &local_port, &startq, NULL));
        } else {
            if (cosq < 0) {
                return BCM_E_PARAM;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));
            if (local_port < 0) {
                return BCM_E_PORT;
            }
            BCM_IF_ERROR_RETURN
                (_bcm_th3_cosq_index_resolve(unit, local_port, cosq,
                                            _BCM_TH3_COSQ_INDEX_STYLE_MCAST_QUEUE,
                                            NULL, &startq, NULL));
        }

        SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

        mem = MMU_THDO_QUEUE_CONFIGm;
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, startq, entry));
        *arg = soc_mem_field32_get(unit, mem, entry, COLOR_ENABLEf);
    } else {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * This function is used to get ingress pool
 * limit given Ingress [Port, Priority Group]
 */
STATIC int
_bcm_th3_cosq_ing_res_limit_get(int unit, bcm_gport_t gport, bcm_cos_t cosq,
                               bcm_cosq_buffer_id_t buffer,
                               bcm_cosq_control_t type, int* arg)
{
    uint32 rval = 0x0;
    int pool;
    int local_port, pipe, itm, idx;
    soc_reg_t reg = INVALIDr;
    soc_field_t fld_limit = INVALIDf;

    if (!arg) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }
    if (type == bcmCosqControlIngressPoolLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_ing_pool_get(unit, gport, cosq,
                                       bcmCosqControlIngressPool,
                                       &pool));
        reg = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
        fld_limit = LIMITf;
    } else if (type == bcmCosqControlIngressPoolResetOffsetLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_ing_pool_get(unit, gport, cosq,
                                       bcmCosqControlIngressPool,
                                       &pool));
        reg = MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr;
        fld_limit = OFFSETf;
    } else if (type == bcmCosqControlIngressHeadroomPoolLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_ing_pool_get(unit, gport, cosq,
                                       bcmCosqControlIngressHeadroomPool,
                                       &pool));
        reg = MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
        fld_limit = LIMITf;
    } else {
        return BCM_E_PARAM;
    }
    SOC_IF_ERROR_RETURN(
        soc_port_pipe_get(unit, local_port, &pipe));

    itm = 0;
    for (idx = 0; idx < NUM_ITM(unit); idx++) {
        if (SOC_INFO(unit).ipipe_itm_map[pipe] & (1 << idx)) {
            itm = idx;
        }
    }

    if (buffer != BCM_COSQ_BUFFER_ID_INVALID) { 
        if (!(SOC_INFO(unit).ipipe_itm_map[pipe] & (1 << buffer))) {
            return BCM_E_PARAM;
        }
    }

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_get(unit, reg,
                                   itm, itm, pool, &rval));
    *arg = soc_reg_field_get(unit, reg, rval, fld_limit);
    *arg = (*arg) * _TH3_MMU_BYTES_PER_CELL;

    return BCM_E_NONE;
}

/*
 * This function is used to set ingress pool
 * limit given Ingress [Port, Priority Group]
 */
STATIC int
_bcm_th3_cosq_ing_res_limit_set(int unit, bcm_gport_t gport, bcm_cos_t cosq,
                               bcm_cosq_buffer_id_t buffer,
                               bcm_cosq_control_t type, int arg)
{
    uint32 rval = 0x0;
    int pool;
    int local_port, pipe, itm, idx;
    soc_reg_t reg = INVALIDr;
    soc_field_t fld_limit = INVALIDf;
    int max_value;
    int pool_limit = 0;
    int headroom_limit = 0;
    int total_limit = 0;
    int mmu_total_cells = 0;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport) ||
        BCM_GPORT_IS_SCHEDULER(gport) ||
        BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
        return BCM_E_PARAM;
    }
    BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_localport_resolve(unit, gport, &local_port));

    if (!SOC_PORT_VALID(unit, local_port)) {
        return BCM_E_PORT;
    }

    BCM_IF_ERROR_RETURN(_bcm_th3_cosq_ing_res_limit_get(
        unit, gport, cosq, buffer,
        bcmCosqControlIngressPoolLimitBytes, &pool_limit));
    BCM_IF_ERROR_RETURN(_bcm_th3_cosq_ing_res_limit_get(
        unit, gport, cosq, buffer,
        bcmCosqControlIngressHeadroomPoolLimitBytes, &headroom_limit));

    if (type == bcmCosqControlIngressPoolLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_ing_pool_get(unit, gport, cosq,
                                       bcmCosqControlIngressPool,
                                       &pool));
        reg = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
        fld_limit = LIMITf;
        max_value = _TH3_THDI_BUFFER_CELL_LIMIT_SP_MAX;
        total_limit = arg + headroom_limit;
    } else if (type == bcmCosqControlIngressPoolResetOffsetLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_ing_pool_get(unit, gport, cosq,
                                       bcmCosqControlIngressPool,
                                       &pool));
        reg = MMU_THDI_CELL_RESET_LIMIT_OFFSET_SPr;
        fld_limit = OFFSETf;
        max_value = _TH3_THDI_BUFFER_CELL_LIMIT_SP_MAX;
    } else if (type == bcmCosqControlIngressHeadroomPoolLimitBytes) {
        BCM_IF_ERROR_RETURN
            (_bcm_th3_cosq_ing_pool_get(unit, gport, cosq,
                                       bcmCosqControlIngressHeadroomPool,
                                       &pool));
        reg = MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
        fld_limit = LIMITf;
        max_value = _TH3_THDI_HDRM_BUFFER_CELL_LIMIT_HP_MAX;
        total_limit = arg + pool_limit;
    } else {
        return BCM_E_PARAM;
    }
    arg = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;
    if (arg > max_value) {
       LOG_WARN(BSL_LS_BCM_COSQ,
           (BSL_META_U(unit, "Warning: Request of %d bytes exceeds maximum of %d bytes\n"),
            arg * _TH3_MMU_BYTES_PER_CELL,
           max_value * _TH3_MMU_BYTES_PER_CELL));
        return BCM_E_PARAM;
    }

    total_limit = (total_limit + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;
    mmu_total_cells = _TH3_MMU_TOTAL_CELLS_PER_ITM;
    if (total_limit > mmu_total_cells) {
       LOG_WARN(BSL_LS_BCM_COSQ,
           (BSL_META_U(unit, "Warning: Total allocation of %d bytes exceeds maximum of %d bytes\n"),
            total_limit * _TH3_MMU_BYTES_PER_CELL,
           mmu_total_cells * _TH3_MMU_BYTES_PER_CELL));
        return BCM_E_PARAM;
    }

    soc_reg_field_set(unit, reg, &rval, fld_limit, arg);
    SOC_IF_ERROR_RETURN(
        soc_port_pipe_get(unit, local_port, &pipe));

    itm = 0;
    for (idx = 0; idx < NUM_ITM(unit); idx++) {
        if (SOC_INFO(unit).ipipe_itm_map[pipe] & (1 << idx)) {
            itm = idx;
        }
    }

    if (buffer != BCM_COSQ_BUFFER_ID_INVALID) { 
        if (!(SOC_INFO(unit).ipipe_itm_map[pipe] & (1 << buffer))) {
            return BCM_E_PARAM;
        }
    }

    SOC_IF_ERROR_RETURN(
        soc_tomahawk3_itm_reg32_set(unit, reg,
                                   itm, itm, pool, rval));

    if (type == bcmCosqControlIngressPoolLimitBytes) {
        BCM_IF_ERROR_RETURN(
            _bcm_th3_mmu_set_shared_size(unit, itm, arg));

    }

    return BCM_E_NONE;
}


STATIC int
_bcm_th3_cosq_egr_port_pool_get(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type, int* arg)
{
    bcm_port_t local_port;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int pool, midx, pipe;
    int gport_type = 0;

    if (!arg) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_TH3_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));
    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));

    gport_type = BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
                 BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport);

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
       (!gport_type && (cosq >= _bcm_th3_get_num_ucq(unit)))) {
        switch (type) {
            case bcmCosqControlEgressPortPoolYellowLimitBytes:
                fld_limit = SP_YELLOW_LIMITf;
                granularity = 8;
                mem = MMU_THDO_CONFIG_PORTSP_MCm;
                break;
            case bcmCosqControlEgressPortPoolRedLimitBytes:
                fld_limit = SP_RED_LIMITf;
                granularity = 8;
                mem = MMU_THDO_CONFIG_PORTSP_MCm;
                break;
            case bcmCosqControlEgressPortPoolSharedLimitBytes:
                fld_limit = SP_SHARED_LIMITf;
                mem = MMU_THDO_CONFIG_PORTSP_MCm;
                break;
            case bcmCosqControlEgressPortPoolSharedResumeBytes:
                granularity = 8;
                fld_limit = SHARED_RESUMEf;
                mem = MMU_THDO_RESUME_PORT_MC0m;
                break;
            case bcmCosqControlEgressPortPoolYellowResumeBytes:
                granularity = 8;
                fld_limit = YELLOW_RESUMEf;
                mem = MMU_THDO_RESUME_PORT_MC0m;
                break;
            case bcmCosqControlEgressPortPoolRedResumeBytes:
                granularity = 8;
                fld_limit = RED_RESUMEf;
                mem = MMU_THDO_RESUME_PORT_MC0m;
                break;
            default:
                return BCM_E_UNAVAIL;
        }
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

        *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        *arg = (*arg) * granularity * _TH3_MMU_BYTES_PER_CELL;
    } else {
        switch (type) {
            case bcmCosqControlEgressPortPoolYellowLimitBytes:
                mem = MMU_THDO_CONFIG_PORT_UC0m;
                switch (pool) {
                    case 0: fld_limit = SP0_YELLOW_LIMITf; break;
                    case 1: fld_limit = SP1_YELLOW_LIMITf; break;
                    case 2: fld_limit = SP2_YELLOW_LIMITf; break;
                    case 3: fld_limit = SP3_YELLOW_LIMITf; break;
                }
                granularity = 8;
                break;
            case bcmCosqControlEgressPortPoolRedLimitBytes:
                mem = MMU_THDO_CONFIG_PORT_UC0m;
                switch (pool) {
                    case 0: fld_limit = SP0_RED_LIMITf; break;
                    case 1: fld_limit = SP1_RED_LIMITf; break;
                    case 2: fld_limit = SP2_RED_LIMITf; break;
                    case 3: fld_limit = SP3_RED_LIMITf; break;
                }
                granularity = 8;
                break;
            case bcmCosqControlEgressPortPoolSharedLimitBytes:
                mem = MMU_THDO_CONFIG_PORT_UC0m;
                switch (pool) {
                    case 0: fld_limit = SP0_SHARED_LIMITf; break;
                    case 1: fld_limit = SP1_SHARED_LIMITf; break;
                    case 2: fld_limit = SP2_SHARED_LIMITf; break;
                    case 3: fld_limit = SP3_SHARED_LIMITf; break;
                }
                break;
            case bcmCosqControlEgressPortPoolSharedResumeBytes:
                mem = MMU_THDO_RESUME_PORT_UC0m;
                fld_limit = SHARED_RESUMEf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPortPoolYellowResumeBytes:
                mem = MMU_THDO_RESUME_PORT_UC0m;
                fld_limit = YELLOW_RESUMEf;
                granularity = 8;
                break;
            case bcmCosqControlEgressPortPoolRedResumeBytes:
                mem = MMU_THDO_RESUME_PORT_UC0m;
                fld_limit = RED_RESUMEf;
                granularity = 8;
                break;
            default:
                return BCM_E_UNAVAIL;
        }
        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

        *arg = soc_mem_field32_get(unit, mem, entry, fld_limit);
        *arg = (*arg) * granularity * _TH3_MMU_BYTES_PER_CELL;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_th3_cosq_egr_port_pool_set(int unit, bcm_gport_t gport,
                               bcm_cos_queue_t cosq,
                               bcm_cosq_control_t type, int arg)
{
    bcm_port_t local_port;
    uint32 max_val;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;
    soc_field_t fld_limit = INVALIDf;
    int granularity = 1;
    int final_value = 0;
    int pool, midx, pipe;
    int gport_type = 0;

    if (arg < 0) {
        return BCM_E_PARAM;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_th3_cosq_index_resolve(unit, gport, cosq,
                                    _BCM_TH3_COSQ_INDEX_STYLE_EGR_POOL,
                                    &local_port, &pool, NULL));
    SOC_IF_ERROR_RETURN(soc_port_pipe_get(unit, local_port, &pipe));
    gport_type = BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
                 BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport);

    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport) ||
       (!gport_type && (cosq >= _bcm_th3_get_num_ucq(unit)))) {
        switch (type) {
            case bcmCosqControlEgressPortPoolYellowLimitBytes:
                fld_limit = SP_YELLOW_LIMITf;
                granularity = 8;
                mem = MMU_THDO_CONFIG_PORTSP_MCm;
                break;
            case bcmCosqControlEgressPortPoolRedLimitBytes:
                fld_limit = SP_RED_LIMITf;
                granularity = 8;
                mem = MMU_THDO_CONFIG_PORTSP_MCm;
                break;
            case bcmCosqControlEgressPortPoolSharedLimitBytes:
                fld_limit = SP_SHARED_LIMITf;
                mem = MMU_THDO_CONFIG_PORTSP_MCm;
                break;
            case bcmCosqControlEgressPortPoolSharedResumeBytes:
                fld_limit = SHARED_RESUMEf;
                granularity = 8;
                mem = MMU_THDO_RESUME_PORT_MC0m;
                break;
            case bcmCosqControlEgressPortPoolYellowResumeBytes:
                fld_limit = YELLOW_RESUMEf;
                granularity = 8;
                mem = MMU_THDO_RESUME_PORT_MC0m;
                break;
            case bcmCosqControlEgressPortPoolRedResumeBytes:
                fld_limit = RED_RESUMEf;
                granularity = 8;
                mem = MMU_THDO_RESUME_PORT_MC0m;
                break;
            default:
                return BCM_E_UNAVAIL;
        }

        midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
        BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));
        final_value = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;

        /* Check for min/max values */
        max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
        if ((final_value < 0) || ((final_value/granularity) > max_val)) {
             return BCM_E_PARAM;
        } else {
             soc_mem_field32_set(unit, mem, entry, fld_limit, (final_value/granularity));
        }
        BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));
    } else {
       switch (type) {
           case bcmCosqControlEgressPortPoolYellowLimitBytes:
               mem = MMU_THDO_CONFIG_PORT_UC0m;
               switch (pool) {
                  case 0: fld_limit = SP0_YELLOW_LIMITf; break;
                  case 1: fld_limit = SP1_YELLOW_LIMITf; break;
                  case 2: fld_limit = SP2_YELLOW_LIMITf; break;
                  case 3: fld_limit = SP3_YELLOW_LIMITf; break;
               }
               granularity = 8;
               break;
           case bcmCosqControlEgressPortPoolRedLimitBytes:
               mem = MMU_THDO_CONFIG_PORT_UC0m;
               switch (pool) {
                  case 0: fld_limit = SP0_RED_LIMITf; break;
                  case 1: fld_limit = SP1_RED_LIMITf; break;
                  case 2: fld_limit = SP2_RED_LIMITf; break;
                  case 3: fld_limit = SP3_RED_LIMITf; break;
               }
               granularity = 8;
               break;
           case bcmCosqControlEgressPortPoolSharedLimitBytes:
               mem = MMU_THDO_CONFIG_PORT_UC0m;
               switch (pool) {
                  case 0: fld_limit = SP0_SHARED_LIMITf; break;
                  case 1: fld_limit = SP1_SHARED_LIMITf; break;
                  case 2: fld_limit = SP2_SHARED_LIMITf; break;
                  case 3: fld_limit = SP3_SHARED_LIMITf; break;
               }
               break;
           case bcmCosqControlEgressPortPoolSharedResumeBytes:
               mem = MMU_THDO_RESUME_PORT_UC0m;
               fld_limit = SHARED_RESUMEf;
               granularity = 8;
               break;
           case bcmCosqControlEgressPortPoolRedResumeBytes:
               mem = MMU_THDO_RESUME_PORT_UC0m;
               fld_limit = RED_RESUMEf;
               granularity = 8;
               break;
           case bcmCosqControlEgressPortPoolYellowResumeBytes:
               mem = MMU_THDO_RESUME_PORT_UC0m;
               fld_limit = YELLOW_RESUMEf;
               granularity = 8;
               break;
           default:
               return BCM_E_UNAVAIL;
       }

       final_value = (arg + _TH3_MMU_BYTES_PER_CELL - 1) / _TH3_MMU_BYTES_PER_CELL;
       midx = SOC_TH3_MMU_PIPED_MEM_INDEX(unit, local_port, mem, pool);
       BCM_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx, entry));

       /* Check for min/max values */
       max_val = (1 << soc_mem_field_length(unit, mem, fld_limit)) - 1;
       if ((final_value < 0) || ((final_value/granularity) > max_val)) {
           return BCM_E_PARAM;
       } else {
           soc_mem_field32_set(unit, mem, entry, fld_limit, (final_value/granularity));
       }
       BCM_IF_ERROR_RETURN(soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th3_cosq_control_set
 * Purpose:
 *      Set specified feature configuration
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      buffer-(IN) XPE ID
 *      type - (IN) feature
 *      arg  - (IN) feature value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcm_th3_cosq_control_set(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_buffer_id_t buffer,
                        bcm_cosq_control_t type, int arg)
{
    uint32 kbits_burst_min, kbits_burst_max;
    uint32 kbits_burst;

    kbits_burst_min = 0;
    kbits_burst_max = 0;

    COMPILER_REFERENCE(kbits_burst_min);
    COMPILER_REFERENCE(kbits_burst_max);

    switch (type) {
        case bcmCosqControlBandwidthBurstMax:
            kbits_burst = arg & 0x7fffffff;
            BCM_IF_ERROR_RETURN
                (bcm_th3_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_th3_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst_min,
                                                         kbits_burst);
        case bcmCosqControlBandwidthBurstMin:
            kbits_burst = arg & 0x7fffffff;
            BCM_IF_ERROR_RETURN
                (bcm_th3_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kbits_burst_min,
                                                       &kbits_burst_max));
            return bcm_th3_cosq_gport_bandwidth_burst_set(unit, gport, cosq,
                                                         kbits_burst,
                                                         kbits_burst_max);
        case bcmCosqControlEgressPool:
        case bcmCosqControlEgressPoolLimitBytes:
        case bcmCosqControlEgressPoolYellowLimitBytes:
        case bcmCosqControlEgressPoolRedLimitBytes:
        case bcmCosqControlEgressPoolLimitEnable:
        case bcmCosqControlUCEgressPool:
        case bcmCosqControlMCEgressPool:
        case bcmCosqControlEgressPoolSharedLimitBytes:
        case bcmCosqControlEgressPoolResumeLimitBytes:
        case bcmCosqControlEgressPoolYellowSharedLimitBytes:
        case bcmCosqControlEgressPoolYellowResumeLimitBytes:
        case bcmCosqControlEgressPoolRedSharedLimitBytes:
        case bcmCosqControlEgressPoolRedResumeLimitBytes:
        case bcmCosqControlEgressPoolHighCongestionLimitBytes:
        case bcmCosqControlEgressPoolLowCongestionLimitBytes:
            return _bcm_th3_cosq_egr_pool_set(unit, gport, cosq, buffer, type, arg);
        case bcmCosqControlEgressUCQueueSharedLimitBytes:
        case bcmCosqControlEgressMCQueueSharedLimitBytes:
        case bcmCosqControlEgressUCQueueMinLimitBytes:
        case bcmCosqControlEgressMCQueueMinLimitBytes:
        case bcmCosqControlEgressUCQueueResumeOffsetBytes:
        case bcmCosqControlEgressMCQueueResumeOffsetBytes:
            return _bcm_th3_cosq_egr_queue_set(unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPortPGSharedLimitBytes:
        case bcmCosqControlIngressPortPGMinLimitBytes:
        case bcmCosqControlIngressPortPoolMinLimitBytes:
        case bcmCosqControlIngressPortPoolSharedLimitBytes:
        case bcmCosqControlIngressPortPGHeadroomLimitBytes:
        case bcmCosqControlIngressPortPGResetFloorBytes:
        case bcmCosqControlIngressPortPGResetOffsetBytes:
        case bcmCosqControlIngressPortPoolResumeLimitBytes:
            return _bcm_th3_cosq_ing_res_set(unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPool:
        case bcmCosqControlIngressHeadroomPool:
            /*
             * This function is used to set headroom pool
             * or shared pool for given Ingress [Port, Priority]
             */
            return _bcm_th3_cosq_ing_pool_set(unit, gport, cosq, type, arg);
        case bcmCosqControlDropLimitAlpha:
            return _bcm_th3_cosq_alpha_set(unit, gport, cosq,
                                (bcm_cosq_control_drop_limit_alpha_value_t)arg);
        case bcmCosqControlIngressPortPGSharedDynamicEnable:
        case bcmCosqControlEgressUCSharedDynamicEnable:
        case bcmCosqControlEgressMCSharedDynamicEnable:
            return _bcm_th3_cosq_dynamic_thresh_enable_set(unit, gport, cosq,
                                                          type, arg);
        case bcmCosqControlEgressPortPoolYellowLimitBytes:
        case bcmCosqControlEgressPortPoolRedLimitBytes:
        case bcmCosqControlEgressPortPoolSharedLimitBytes:
        case bcmCosqControlEgressPortPoolSharedResumeBytes:
        case bcmCosqControlEgressPortPoolYellowResumeBytes:
        case bcmCosqControlEgressPortPoolRedResumeBytes:
            return _bcm_th3_cosq_egr_port_pool_set(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueLimitEnable:
        case bcmCosqControlEgressMCQueueLimitEnable:
            return _bcm_th3_cosq_egr_queue_limit_enable_set(unit, gport, cosq,
                                                           type, arg);
        case bcmCosqControlIngressPoolLimitBytes:
        case bcmCosqControlIngressPoolResetOffsetLimitBytes:
        case bcmCosqControlIngressHeadroomPoolLimitBytes:
            return _bcm_th3_cosq_ing_res_limit_set(unit, gport, cosq, buffer, type, arg);
        case bcmCosqControlEgressUCQueueGroupMinEnable:
        case bcmCosqControlEgressMCQueueGroupMinEnable:
            return _bcm_th3_cosq_qgroup_limit_enables_set(unit, gport, cosq,
                    type, arg);
        case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
        case bcmCosqControlEgressMCQueueGroupMinLimitBytes:
            return _bcm_th3_cosq_qgroup_limit_set(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueColorLimitDynamicEnable:
        case bcmCosqControlEgressMCQueueColorLimitDynamicEnable:
            return _bcm_th3_cosq_egr_queue_color_limit_mode_set(unit, gport, cosq,
                                                           type, arg);
        case bcmCosqControlEgressUCQueueColorLimitEnable:
        case bcmCosqControlEgressMCQueueColorLimitEnable:
            return _bcm_th3_cosq_egr_queue_color_limit_enable_set(unit, gport, cosq,
                                                           type, arg);
        case bcmCosqControlEgressUCQueueRedLimit:
        case bcmCosqControlEgressUCQueueYellowLimit:
        case bcmCosqControlEgressMCQueueRedLimit:
        case bcmCosqControlEgressMCQueueYellowLimit:
            return _bcm_th3_cosq_egr_queue_color_limit_set(unit, gport, cosq, type, arg);
        default:
            break;
    }
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_th3_cosq_control_get
 * Purpose:
 *      Get specified feature configuration
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT ID.
 *      cosq - (IN) COS queue.
 *      buffer-(IN) XPE ID
 *      type - (IN) feature
 *      arg  - (OUT) feature value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */

int
bcm_th3_cosq_control_get(int unit, bcm_gport_t gport, bcm_cos_queue_t cosq,
                        bcm_cosq_buffer_id_t buffer,
                        bcm_cosq_control_t type, int *arg)
{
    uint32 kburst_tmp, kbits_burst;

    switch (type) {
        case bcmCosqControlBandwidthBurstMax:
            BCM_IF_ERROR_RETURN
                (bcm_th3_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                       &kburst_tmp,
                                                       &kbits_burst));
            *arg = kbits_burst & 0x7fffffff;
            return BCM_E_NONE;
        case bcmCosqControlBandwidthBurstMin:
            return bcm_th3_cosq_gport_bandwidth_burst_get(unit, gport, cosq,
                                                         (uint32*)arg,
                                                         &kburst_tmp);
        case bcmCosqControlEgressPool:
        case bcmCosqControlEgressPoolLimitBytes:
        case bcmCosqControlEgressPoolYellowLimitBytes:
        case bcmCosqControlEgressPoolRedLimitBytes:
        case bcmCosqControlEgressPoolLimitEnable:
        case bcmCosqControlUCEgressPool:
        case bcmCosqControlMCEgressPool:
        case bcmCosqControlEgressPoolSharedLimitBytes:
        case bcmCosqControlEgressPoolResumeLimitBytes:
        case bcmCosqControlEgressPoolYellowSharedLimitBytes:
        case bcmCosqControlEgressPoolYellowResumeLimitBytes:
        case bcmCosqControlEgressPoolRedSharedLimitBytes:
        case bcmCosqControlEgressPoolRedResumeLimitBytes:
        case bcmCosqControlEgressPoolHighCongestionLimitBytes:
        case bcmCosqControlEgressPoolLowCongestionLimitBytes:
            return _bcm_th3_cosq_egr_pool_get(unit, gport, cosq, buffer, type, arg);
        case bcmCosqControlEgressUCQueueSharedLimitBytes:
        case bcmCosqControlEgressMCQueueSharedLimitBytes:
        case bcmCosqControlEgressUCQueueMinLimitBytes:
        case bcmCosqControlEgressMCQueueMinLimitBytes:
        case bcmCosqControlEgressUCQueueResumeOffsetBytes:
        case bcmCosqControlEgressMCQueueResumeOffsetBytes:
            return _bcm_th3_cosq_egr_queue_get(unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPortPGSharedLimitBytes:
        case bcmCosqControlIngressPortPGMinLimitBytes:
        case bcmCosqControlIngressPortPoolMinLimitBytes:
        case bcmCosqControlIngressPortPoolSharedLimitBytes:
        case bcmCosqControlIngressPortPGHeadroomLimitBytes:
        case bcmCosqControlIngressPortPGResetFloorBytes:
        case bcmCosqControlIngressPortPGResetOffsetBytes:
        case bcmCosqControlIngressPortPoolResumeLimitBytes:
            return _bcm_th3_cosq_ing_res_get(unit, gport, cosq, type, arg);
        case bcmCosqControlIngressPool:
        case bcmCosqControlIngressHeadroomPool:
            /*
             * This function is used to set headroom pool
             * or shared pool for given Ingress [Port, Priority]
             */
            return _bcm_th3_cosq_ing_pool_get(unit, gport, cosq, type, arg);
        case bcmCosqControlDropLimitAlpha:
            return _bcm_th3_cosq_alpha_get(unit, gport, cosq,
                                (bcm_cosq_control_drop_limit_alpha_value_t *)arg);
        case bcmCosqControlIngressPortPGSharedDynamicEnable:
        case bcmCosqControlEgressUCSharedDynamicEnable:
        case bcmCosqControlEgressMCSharedDynamicEnable:
            return _bcm_th3_cosq_dynamic_thresh_enable_get(unit, gport, cosq,
                                                          type, arg);
        case bcmCosqControlEgressPortPoolYellowLimitBytes:
        case bcmCosqControlEgressPortPoolRedLimitBytes:
        case bcmCosqControlEgressPortPoolSharedLimitBytes:
        case bcmCosqControlEgressPortPoolSharedResumeBytes:
        case bcmCosqControlEgressPortPoolYellowResumeBytes:
        case bcmCosqControlEgressPortPoolRedResumeBytes:
            return _bcm_th3_cosq_egr_port_pool_get(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueLimitEnable:
        case bcmCosqControlEgressMCQueueLimitEnable:
            return _bcm_th3_cosq_egr_queue_limit_enable_get(unit, gport, cosq,
                                                           type, arg);
        case bcmCosqControlIngressPoolLimitBytes:
        case bcmCosqControlIngressPoolResetOffsetLimitBytes:
        case bcmCosqControlIngressHeadroomPoolLimitBytes:
            return _bcm_th3_cosq_ing_res_limit_get(unit, gport, cosq, buffer, type, arg);
        case bcmCosqControlEgressUCQueueGroupMinEnable:
        case bcmCosqControlEgressMCQueueGroupMinEnable:
            return _bcm_th3_cosq_qgroup_limit_enables_get(unit, gport, cosq,
                    type, arg);
        case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
        case bcmCosqControlEgressMCQueueGroupMinLimitBytes:
            return _bcm_th3_cosq_qgroup_limit_get(unit, gport, cosq, type, arg);
        case bcmCosqControlEgressUCQueueColorLimitDynamicEnable:
        case bcmCosqControlEgressMCQueueColorLimitDynamicEnable:
            return _bcm_th3_cosq_egr_queue_color_limit_mode_get(unit, gport, cosq,
                                                           type, arg);
        case bcmCosqControlEgressUCQueueColorLimitEnable:
        case bcmCosqControlEgressMCQueueColorLimitEnable:
            return _bcm_th3_cosq_egr_queue_color_limit_enable_get(unit, gport, cosq,
                                                           type, arg);
        case bcmCosqControlEgressUCQueueRedLimit:
        case bcmCosqControlEgressUCQueueYellowLimit:
        case bcmCosqControlEgressMCQueueRedLimit:
        case bcmCosqControlEgressMCQueueYellowLimit:
            return _bcm_th3_cosq_egr_queue_color_limit_get(unit, gport, cosq, type, arg);
        default:
            break;
    }
    return BCM_E_UNAVAIL;

}

int bcm_tomahawk3_cosq_control_dynamic_get(int unit,
        bcm_cosq_control_t type, bcm_cosq_dynamic_setting_type_t *dynamic)
{
    int rv;

    switch (type) {
        case bcmCosqControlEgressPool:
        case bcmCosqControlUCEgressPool:
        case bcmCosqControlMCEgressPool:
        case bcmCosqControlEgressUCQueueGroupMinEnable:
        case bcmCosqControlEgressMCQueueGroupMinEnable:
            *dynamic = bcmCosqDynamicTypeConditional;
            rv = BCM_E_NONE;
            break;
        case bcmCosqControlIngressPool:
        case bcmCosqControlIngressHeadroomPool:
            *dynamic = bcmCosqDynamicTypeFixed;
            rv = BCM_E_NONE;
            break;
        case bcmCosqControlBandwidthBurstMax:
        case bcmCosqControlBandwidthBurstMin:
        case bcmCosqControlEgressPoolLimitBytes:
        case bcmCosqControlEgressPoolYellowLimitBytes:
        case bcmCosqControlEgressPoolRedLimitBytes:
        case bcmCosqControlEgressPoolLimitEnable:
        case bcmCosqControlEgressPoolSharedLimitBytes:
        case bcmCosqControlEgressPoolResumeLimitBytes:
        case bcmCosqControlEgressPoolYellowSharedLimitBytes:
        case bcmCosqControlEgressPoolYellowResumeLimitBytes:
        case bcmCosqControlEgressPoolRedSharedLimitBytes:
        case bcmCosqControlEgressPoolRedResumeLimitBytes:
        case bcmCosqControlEgressUCQueueSharedLimitBytes:
        case bcmCosqControlEgressMCQueueSharedLimitBytes:
        case bcmCosqControlEgressUCQueueMinLimitBytes:
        case bcmCosqControlEgressMCQueueMinLimitBytes:
        case bcmCosqControlEgressUCQueueResumeOffsetBytes:
        case bcmCosqControlEgressMCQueueResumeOffsetBytes:
        case bcmCosqControlEgressPoolHighCongestionLimitBytes:
        case bcmCosqControlEgressPoolLowCongestionLimitBytes:
        case bcmCosqControlIngressPortPGSharedLimitBytes:
        case bcmCosqControlIngressPortPGMinLimitBytes:
        case bcmCosqControlIngressPortPoolMinLimitBytes:
        case bcmCosqControlIngressPortPoolSharedLimitBytes:
        case bcmCosqControlIngressPortPGHeadroomLimitBytes:
        case bcmCosqControlIngressPortPGResetFloorBytes:
        case bcmCosqControlIngressPortPGResetOffsetBytes:
        case bcmCosqControlIngressPortPoolResumeLimitBytes:
        case bcmCosqControlDropLimitAlpha:
        case bcmCosqControlIngressPortPGSharedDynamicEnable:
        case bcmCosqControlEgressUCSharedDynamicEnable:
        case bcmCosqControlEgressMCSharedDynamicEnable:
        case bcmCosqControlEgressPortPoolYellowLimitBytes:
        case bcmCosqControlEgressPortPoolRedLimitBytes:
        case bcmCosqControlEgressPortPoolSharedLimitBytes:
        case bcmCosqControlEgressPortPoolSharedResumeBytes:
        case bcmCosqControlEgressPortPoolYellowResumeBytes:
        case bcmCosqControlEgressPortPoolRedResumeBytes:
        case bcmCosqControlEgressUCQueueLimitEnable:
        case bcmCosqControlEgressMCQueueLimitEnable:
        case bcmCosqControlIngressPoolLimitBytes:
        case bcmCosqControlIngressPoolResetOffsetLimitBytes:
        case bcmCosqControlIngressHeadroomPoolLimitBytes:
        case bcmCosqControlEgressUCQueueGroupMinLimitBytes:
        case bcmCosqControlEgressMCQueueGroupMinLimitBytes:
        case bcmCosqControlEgressUCQueueColorLimitDynamicEnable:
        case bcmCosqControlEgressMCQueueColorLimitDynamicEnable:
        case bcmCosqControlEgressUCQueueColorLimitEnable:
        case bcmCosqControlEgressMCQueueColorLimitEnable:
        case bcmCosqControlEgressUCQueueRedLimit:
        case bcmCosqControlEgressUCQueueYellowLimit:
        case bcmCosqControlEgressMCQueueRedLimit:
        case bcmCosqControlEgressMCQueueYellowLimit:
            *dynamic = bcmCosqDynamicTypeFlexible;
            rv = BCM_E_NONE;
            break;
        default:
            rv = BCM_E_UNAVAIL;
            break;
    }
    return rv;
}

int
bcm_th3_cosq_service_pool_get(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t *cosq_service_pool)
{
    uint32 rval;
    soc_field_t fld_enable = INVALIDf;
    int color_limit_enable = 0;

    if (cosq_service_pool == NULL) {
        return BCM_E_PARAM;
    }

    if (id < 0 || (id > _TH3_MMU_NUM_POOL - 1)) {
        return BCM_E_PARAM;
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(
                soc_tomahawk3_itm_reg32_get(unit,
                    MMU_THDO_SHARED_DB_POOL_CONFIGr, -1, -1, -1, &rval));

    switch (cosq_service_pool->type) {
        case bcmCosqServicePoolColorAware:
            fld_enable = POOL_COLOR_LIMIT_ENABLEf;
            break;
        default:
            return BCM_E_PARAM;
    }

    color_limit_enable = soc_reg_field_get(unit, MMU_THDO_SHARED_DB_POOL_CONFIGr, rval,
                                                   fld_enable);
    color_limit_enable &= (1 << id);
    cosq_service_pool->enabled = (color_limit_enable != 0) ? 1 : 0;

    return BCM_E_NONE;
}

int
bcm_th3_cosq_service_pool_set(
    int unit,
    bcm_service_pool_id_t id,
    bcm_cosq_service_pool_t cosq_service_pool)
{
    uint32 rval;
    soc_field_t fld_enable = INVALIDf;
    int color_limit_enable = 0;

    if (id < 0 || (id > _TH3_MMU_NUM_POOL - 1)) {
        return BCM_E_PARAM;
    }

    rval = 0;
    SOC_IF_ERROR_RETURN(
                soc_tomahawk3_itm_reg32_get(unit,
                    MMU_THDO_SHARED_DB_POOL_CONFIGr, -1, -1, -1, &rval));
    switch (cosq_service_pool.type) {
        case bcmCosqServicePoolColorAware:
            fld_enable = POOL_COLOR_LIMIT_ENABLEf;
            break;
        default:
            return BCM_E_PARAM;
    }
    color_limit_enable = soc_reg_field_get(unit, MMU_THDO_SHARED_DB_POOL_CONFIGr, rval,
                                                   fld_enable);
    if (cosq_service_pool.enabled) {
        color_limit_enable |= (1 << id);
    } else {
        color_limit_enable &= (~(1 << id));
        color_limit_enable = color_limit_enable & 0xf;
    }
    soc_reg_field_set(unit, MMU_THDO_SHARED_DB_POOL_CONFIGr, &rval,
                      fld_enable, color_limit_enable);
    SOC_IF_ERROR_RETURN(soc_tomahawk3_itm_reg32_set(unit,
                            MMU_THDO_SHARED_DB_POOL_CONFIGr, -1, -1,
                            -1, rval));

    return BCM_E_NONE;

}

static int
_bcm_th3_adjust_delta_hdrm_service_pools(int unit, int itm, int hdrm_pool, int
        service_pool, uint32 new_hp_limit) {
    soc_reg_t reg;
    int cur_sp_limit[_TH3_ITMS_PER_DEV];
    uint32 old_hp_limit[_TH3_ITMS_PER_DEV] = {0},
           new_sp_limit[_TH3_ITMS_PER_DEV] = {0};
    uint32 rval = 0;

    /* Retrieve old value of headroom pool limit */
    reg = MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
    BCM_IF_ERROR_RETURN(soc_tomahawk3_itm_reg32_get(unit, reg,
                                    itm, itm, hdrm_pool, &old_hp_limit[itm]));
    if (old_hp_limit[itm] != new_hp_limit) {
        /* Delta of headroom pool limit */
        BCM_IF_ERROR_RETURN(_bcm_th3_mmu_get_shared_size(unit, itm,
                    &cur_sp_limit[itm]));
        if (old_hp_limit[itm] > new_hp_limit) {
            new_sp_limit[itm] = cur_sp_limit[itm] + (old_hp_limit[itm] - new_hp_limit);
            new_sp_limit[itm] = MIN(_TH3_THDI_BUFFER_CELL_LIMIT_SP_MAX,
                    new_sp_limit[itm]);
        } else {
            if (cur_sp_limit[itm] > (new_hp_limit - old_hp_limit[itm])) {
                new_sp_limit[itm] = cur_sp_limit[itm] - (new_hp_limit -
                        old_hp_limit[itm]);
            } else {
                LOG_WARN(BSL_LS_BCM_COSQ,
                    (BSL_META_U(unit, "Warning: Given input params result"
                                "in Headroom pool limit addition greater"
                                "than available shared space: %d for itm:"
                                "%d. Try reducing the number of lossless"
                                "classes\n"), cur_sp_limit[itm], itm));
                return BCM_E_RESOURCE;
            }
        }
        /* Update the saved shared limit */
        reg = MMU_THDI_HDRM_BUFFER_CELL_LIMIT_HPr;
        BCM_IF_ERROR_RETURN(_bcm_th3_mmu_set_shared_size(unit, itm,
                    new_sp_limit[itm]));
        BCM_IF_ERROR_RETURN(soc_tomahawk3_itm_reg32_set(unit, reg,
                                    itm, itm, hdrm_pool, new_hp_limit));
        /* Update the Service pool shared limit for SP 0 */
        reg = MMU_THDI_BUFFER_CELL_LIMIT_SPr;
        soc_reg_field_set(unit, reg, &rval, LIMITf, new_sp_limit[itm]);
        BCM_IF_ERROR_RETURN(
            soc_tomahawk3_itm_reg32_set(unit, reg,
                               itm, itm, service_pool, rval));
    }
    return BCM_E_NONE;
}

/* Function to set Headroom Pool limit based on packet distribution and number
 * of lossless classes */
int
bcm_tomahawk3_cosq_hdrm_pool_limit_set(int unit, int hdrm_pool, int num_lossless_class,
    int arr_size, bcm_cosq_pkt_size_dist_t *pkt_dist_array)
{
    int rv, hdrm, i;
    _soc_pkt_size_dist_t *size_dist;
    uint32 fld_val;
    int itm;
    uint32 itm_map;
    int speed = 0, port;
    soc_info_t *si = &SOC_INFO(unit);
#define _TH3_MMU_DEF_HDRM_CABLE_LEN 100

    if ((arr_size > 3) || (pkt_dist_array == NULL) ) {
        return BCM_E_PARAM;
    }
    if (hdrm_pool > _TH3_MMU_NUM_POOL) {
        return BCM_E_PARAM;
    }

    size_dist = sal_alloc(arr_size * sizeof(bcm_cosq_pkt_size_dist_t),
            "size dist");
    if (size_dist == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < arr_size; i++) {
        size_dist[i].pkt_size = pkt_dist_array[i].pkt_size;
        size_dist[i].dist_perc = pkt_dist_array[i].dist_perc;
    }
    PBMP_PORT_ITER(unit, port) {
        speed = MAX(speed, si->port_speed_max[port]);
    }

    rv = soc_th3_get_port_pg_headroom(unit, speed, _TH3_MMU_DEF_HDRM_CABLE_LEN,
            arr_size, size_dist, &hdrm);
    sal_free(size_dist);
    if (rv) {
        return rv;
    }

    fld_val = hdrm * num_lossless_class;
    itm_map = SOC_INFO(unit).itm_map;
    for (itm = 0; itm < NUM_ITM(unit); itm ++) {
        if (itm_map & (1 << itm)) {
            SOC_IF_ERROR_RETURN(_bcm_th3_adjust_delta_hdrm_service_pools(unit, itm,
                    hdrm_pool, 0, fld_val));
        }
    }
#undef _TH3_MMU_DEF_HDRM_CABLE_LEN
    return rv;
}

int
bcm_tomahawk3_cosq_port_prigrp_hdrm_set(int unit, bcm_port_t port, uint32 pri_bmp,
    int cable_len, int arr_size, bcm_cosq_pkt_size_dist_t *pkt_dist_array)
{
    _soc_pkt_size_dist_t *size_dist;
    int i, rv = 0, speed;
    int hdrm;
    int pri_pg_prof, pri_i, uc_pg, mc_pg, pg_i;
    soc_field_t pri_pg_flds[_TH3_MMU_NUM_INT_PRI] = {
        INPPRI0_PGf, INPPRI1_PGf, INPPRI2_PGf,
        INPPRI3_PGf, INPPRI4_PGf, INPPRI5_PGf,
        INPPRI6_PGf, INPPRI7_PGf, INPPRI8_PGf,
        INPPRI9_PGf, INPPRI10_PGf, INPPRI11_PGf,
        INPPRI12_PGf, INPPRI13_PGf, INPPRI14_PGf,
        INPPRI15_PGf};
    soc_field_t field_pghdrm_limit[_TH3_MMU_NUM_PG] = {
        PG0_HDRM_LIMITf, PG1_HDRM_LIMITf, PG2_HDRM_LIMITf,
        PG3_HDRM_LIMITf, PG4_HDRM_LIMITf, PG5_HDRM_LIMITf,
        PG6_HDRM_LIMITf, PG7_HDRM_LIMITf};
    uint32 pg_bmp = 0x0;
    uint64 rval64;
    soc_info_t *si = &SOC_INFO(unit);
    int pipe, midx;
    uint32 entry[SOC_MAX_MEM_WORDS];
    soc_mem_t mem = INVALIDm;

    /* Parameter checks */
    if (!SOC_PORT_VALID(unit, port)) {
        return BCM_E_PORT;
    }
    if ((arr_size > 3) || (pkt_dist_array == NULL) || (pri_bmp > 0xffff)) {
        return BCM_E_PARAM;
    }


    size_dist = sal_alloc(arr_size * sizeof(bcm_cosq_pkt_size_dist_t),
            "size dist");
    if (size_dist == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < arr_size; i++) {
        size_dist[i].pkt_size = pkt_dist_array[i].pkt_size;
        size_dist[i].dist_perc = pkt_dist_array[i].dist_perc;
    }

    rv = bcm_esw_port_speed_get(unit, port, &speed);
    if (rv) {
        goto exit;
    }

    rv = soc_th3_get_port_pg_headroom(unit, speed, cable_len, arr_size,
            size_dist, &hdrm);
    if (rv) {
        goto exit;
    }
    rv = _bcm_th3_cosq_port_inp_pri_to_pg_profile_get(unit, port, &pri_pg_prof);
    if (rv) {
        goto exit;
    }
    for (pri_i = 0; pri_i < _TH3_MMU_NUM_INT_PRI; pri_i++) {
        if (pri_bmp & (1U << pri_i)) {
            COMPILER_64_ZERO(rval64);
            rv = soc_reg64_get(unit, MMU_THDI_UC_INPPRI_PG_PROFILEr,
                                   REG_PORT_ANY, pri_pg_prof, &rval64);
            if (rv) {
                goto exit;
            }
            uc_pg = soc_reg64_field32_get(unit, MMU_THDI_UC_INPPRI_PG_PROFILEr,
                            rval64, pri_pg_flds[pri_i]);
            pg_bmp |= (1U << uc_pg);
            COMPILER_64_ZERO(rval64);
            rv = soc_reg64_get(unit, MMU_THDI_NONUC_INPPRI_PG_PROFILEr,
                                   REG_PORT_ANY, pri_pg_prof, &rval64);
            if (rv) {
                goto exit;
            }
            mc_pg = soc_reg64_field32_get(unit, MMU_THDI_NONUC_INPPRI_PG_PROFILEr,
                            rval64, pri_pg_flds[pri_i]);
            pg_bmp |= (1U << mc_pg);
        }
    }

    pipe = si->port_pipe[port];
    mem = SOC_MEM_UNIQUE_ACC(unit, MMU_THDI_PORT_PG_HDRM_CONFIGm)[pipe];
    midx = SOC_TH3_MMU_LOCAL_PORT(unit, port);
    for (pg_i = 0; pg_i < _TH3_MMU_NUM_PG; pg_i++) {
        if (pg_bmp & (1U << pg_i)) {
            rv = soc_mem_read(unit, mem, MEM_BLOCK_ALL, midx,
                        entry);
            if (rv) {
                goto exit;
            }
            soc_mem_field32_set(unit, mem, entry, field_pghdrm_limit[pg_i], hdrm);
            rv = soc_mem_write(unit, mem, MEM_BLOCK_ALL, midx, entry);
            if (rv) {
                goto exit;
            }
        }
    }

exit:
    sal_free(size_dist);
    return rv;
}

#endif
