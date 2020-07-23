/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        multicast.c
 * Purpose:     Tomahawk multicast functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TOMAHAWK_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/scache_dictionary.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/esw/tomahawk.h>
#include <bcm_int/esw/subport.h>
#include <include/bcm_int/esw/xgs5.h>

#ifdef BCM_FIREBOLT6_SUPPORT
#include <soc/firebolt6.h>
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
#include <bcm_int/esw/tomahawk3.h>
#endif

#define _TH_NUM_TRUNKS(unit) \
        (SOC_IS_TOMAHAWK3(unit) ? 144 : 1024)
#define _TH_NUM_AGGID_PER_PIPE(unit)  \
        (SOC_IS_TOMAHAWK3(unit) ? \
        (soc_mem_field_length(unit, MMU_REPL_GROUP_INFO_TBLm, MEMBER_BMPf)) : \
        (soc_mem_field_length(unit, MMU_REPL_GROUP_INFO_TBLm, PIPE_MEMBER_BMPf)))
#define _TH_MAX_NUM_TRUNKS 1024

#define _FB6_TOTAL_NUM_PORT 512

#define _TH_DEVICE_NUM_PORTS(unit) \
        ((soc_feature(unit, soc_feature_channelized_switching)) ? \
        _FB6_TOTAL_NUM_PORT : SOC_MAX_NUM_PORTS)


typedef
 struct _bcm_th_aggid_used_bmp_per_pipe_s {
    SHR_BITDCL* pipe_aggid_bmp;
} _bcm_th_aggid_used_bmp_per_pipe_t;

STATIC _bcm_th_aggid_used_bmp_per_pipe_t *th_aggid_used_bmp[BCM_MAX_NUM_UNITS];

typedef struct _bcm_th_trunk_aggid_info_s {
    int agg_id;
    int ref_cnt;
} _bcm_th_trunk_aggid_info_t;

typedef struct _bcm_th_trunk_aggid_per_pipe_info_s {
    _bcm_th_trunk_aggid_info_t trunk_aggid_info[_TH_MAX_NUM_TRUNKS];
} _bcm_th_trunk_aggid_per_pipe_info_t;

typedef struct _bcm_th_port_aggid_info_s {
    int agg_id;
    int ref_cnt;   /* The number of the pair <mc group, nexthop> pointing to it. */
    int trunk_id;
} _bcm_th_port_aggid_info_t;

STATIC _bcm_th_trunk_aggid_per_pipe_info_t *th_trunk_aggid[BCM_MAX_NUM_UNITS];

STATIC _bcm_th_port_aggid_info_t *th_port_aggid[BCM_MAX_NUM_UNITS];


/*
 * Function:
 *      bcm_th_aggid_info_detach
 * Purpose:
 *      Free aggregation ID list.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_aggid_info_detach(int unit)
{
    int num_pipes;
    int i;
    SHR_BITDCL *pipe_aggid_bmp;

    num_pipes = NUM_PIPE(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        /* IPMC repl is pipe independent in TH3 */
        num_pipes = 1;
    }
#endif
    if (th_aggid_used_bmp[unit] != NULL) {
        for (i = 0; i < num_pipes; i++) {
            pipe_aggid_bmp = th_aggid_used_bmp[unit][i].pipe_aggid_bmp;
            if (pipe_aggid_bmp != NULL) {
                sal_free(pipe_aggid_bmp);
                pipe_aggid_bmp = NULL;
            }
        }
        sal_free(th_aggid_used_bmp[unit]);
        th_aggid_used_bmp[unit] = NULL;
    }

	if (th_trunk_aggid[unit] != NULL) {
        sal_free(th_trunk_aggid[unit]);
		th_trunk_aggid[unit] = NULL;
	}

	if (th_port_aggid[unit] != NULL) {
		sal_free(th_port_aggid[unit]);
		th_port_aggid[unit] = NULL;
	}

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_aggid_info_init
 * Purpose:
 *      Initialize aggregation ID to trunk map array.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_aggid_info_init(int unit)
{
    int   num_pipes;
    int   i, j;
    _bcm_th_trunk_aggid_info_t *trunk_aggid_info;
    _bcm_th_port_aggid_info_t *port_aggid_info;


    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    num_pipes = NUM_PIPE(unit);

#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        /* IPMC repl is pipe independent in TH3 */
        num_pipes = 1;
    }
#endif
    bcm_th_aggid_info_detach(unit);


    th_aggid_used_bmp[unit] =
        sal_alloc((num_pipes * sizeof(_bcm_th_aggid_used_bmp_per_pipe_t)),
                  "aggid_used_bmp_info");
    if (th_aggid_used_bmp[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    for (i = 0; i < num_pipes; i++) {
        th_aggid_used_bmp[unit][i].pipe_aggid_bmp =
            sal_alloc(SHR_BITALLOCSIZE(_TH_NUM_AGGID_PER_PIPE(unit)),
                      "aggid_used_bmp_per_piep_info");
        if (th_aggid_used_bmp[unit][i].pipe_aggid_bmp == NULL) {
            bcm_th_aggid_info_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(th_aggid_used_bmp[unit][i].pipe_aggid_bmp, 0,
                   SHR_BITALLOCSIZE(_TH_NUM_AGGID_PER_PIPE(unit)));
    }

    /* Init trunk aggid info. */
    th_trunk_aggid[unit] =
        sal_alloc((num_pipes * sizeof(_bcm_th_trunk_aggid_per_pipe_info_t)),
                  "trunk_aggid_info");
    if (th_trunk_aggid[unit] == NULL) {
        bcm_th_aggid_info_detach(unit);
        return BCM_E_MEMORY;
    }
    for (i = 0; i < num_pipes; i++) {
        for (j = 0; j < _TH_NUM_TRUNKS(unit); j++) {
            trunk_aggid_info = &(th_trunk_aggid[unit][i].trunk_aggid_info[j]);
            trunk_aggid_info->agg_id = TH_AGG_ID_INVALID;
            trunk_aggid_info->ref_cnt = 0;
        }
    }

    /* Init port aggid info. */
    th_port_aggid[unit] =
        sal_alloc(((_TH_DEVICE_NUM_PORTS(unit)) * sizeof(_bcm_th_port_aggid_info_t)),
                   "port_aggid_info");
    if (th_port_aggid[unit] == NULL) {
        bcm_th_aggid_info_detach(unit);
        return BCM_E_MEMORY;
    }
    for (i = 0; i < _TH_DEVICE_NUM_PORTS(unit); i++) {
        port_aggid_info = &th_port_aggid[unit][i];
        port_aggid_info->agg_id = TH_AGG_ID_INVALID;
        port_aggid_info->ref_cnt = 0;
        port_aggid_info->trunk_id = BCM_TRUNK_INVALID;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_get_free_aggid
 * Purpose:
 *      Get a free aggid for given pipe.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      pipe  - (IN)  Pipe id.
 *      aggid - (OUT) Returned aggid.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th_get_free_aggid(int unit, int pipe, int *aggid)
{
    SHR_BITDCL *aggid_bmp;
    int i;

    aggid_bmp = th_aggid_used_bmp[unit][pipe].pipe_aggid_bmp;
    for (i = 0; i < _TH_NUM_AGGID_PER_PIPE(unit); i++) {
        if (!SHR_BITGET(aggid_bmp, i)) {
            *aggid = i;
            SHR_BITSET(aggid_bmp, i);
            return BCM_E_NONE;
        }
    }

    if (i == _TH_NUM_AGGID_PER_PIPE(unit)) {
        return BCM_E_FULL;
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_th_set_free_aggid
 * Purpose:
 *      Free aggid for given pipe.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      pipe  - (IN)  Pipe id.
 *      aggid - (IN)  Aggregation ID.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th_set_free_aggid(int unit, int pipe, int aggid)
{
    SHR_BITDCL *aggid_bmp;

    aggid_bmp = th_aggid_used_bmp[unit][pipe].pipe_aggid_bmp;
    if (aggid >= _TH_NUM_AGGID_PER_PIPE(unit)) {
        return BCM_E_PARAM;
    }
    SHR_BITCLR(aggid_bmp, aggid);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_set_port_hw_agg_map
 * Purpose:
 *      Set HW aggid map register for a port.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN) Logical Port.
 *      aggid - (IN) Aggregation ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_set_port_hw_agg_map(int unit, bcm_port_t port, int aggid)
{
    uint32 regval;
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        BCM_IF_ERROR_RETURN(READ_MMU_RQE_REPL_PORT_AGG_MAPr(unit, port,
                                                            &regval));
        soc_reg_field_set(unit, MMU_RQE_REPL_PORT_AGG_MAPr, &regval,
                                    L3MC_PORT_AGG_IDf, aggid);
        SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit, MMU_RQE_REPL_PORT_AGG_MAPr, port, 0, regval));
        return BCM_E_NONE;
    }
#endif
#if defined(BCM_FIREBOLT6_SUPPORT)
   if (soc_feature(unit, soc_feature_repl_port_agg_map_is_mem)) {
        soc_info_t *si;
        soc_port_t phy_port, mmu_port;
        mmu_dqs_repl_port_agg_map_entry_t  dqs_entry;
        mmu_rqe_repl_port_agg_map_entry_t  rqe_entry;

        si = &SOC_INFO(unit);
        if (port < FB6_NUM_PORT) {
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
        } else {
           phy_port = mmu_port = port;
        }

        sal_memset(&dqs_entry, 0, sizeof(mmu_dqs_repl_port_agg_map_entry_t));
        sal_memset(&rqe_entry, 0, sizeof(mmu_rqe_repl_port_agg_map_entry_t));

        SOC_IF_ERROR_RETURN (soc_mem_read(unit, MMU_DQS_REPL_PORT_AGG_MAP_PIPE0m,
            MEM_BLOCK_ALL, mmu_port, &dqs_entry));

        soc_mem_field32_set(unit, MMU_DQS_REPL_PORT_AGG_MAP_PIPE0m, &dqs_entry,
            L3MC_PORT_AGG_IDf, aggid);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit,
               MMU_DQS_REPL_PORT_AGG_MAP_PIPE0m,
               MEM_BLOCK_ALL, mmu_port, &dqs_entry));

        /* Write into the RQE table now. */
        SOC_IF_ERROR_RETURN (soc_mem_read(unit, MMU_RQE_REPL_PORT_AGG_MAP_SC0m,
            MEM_BLOCK_ALL, mmu_port, &rqe_entry));

       soc_mem_field32_set(unit, MMU_RQE_REPL_PORT_AGG_MAP_SC0m, &rqe_entry,
            L3MC_PORT_AGG_IDf, aggid);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit,
               MMU_RQE_REPL_PORT_AGG_MAP_SC0m,
               MEM_BLOCK_ALL, mmu_port, &rqe_entry));


    } else
#endif
    {

        BCM_IF_ERROR_RETURN(READ_MMU_DQS_REPL_PORT_AGG_MAPr(unit, port, &regval));
        soc_reg_field_set(unit, MMU_DQS_REPL_PORT_AGG_MAPr, &regval,
                      L3MC_PORT_AGG_IDf, aggid);
        SOC_IF_ERROR_RETURN(
            soc_reg32_set(unit, MMU_DQS_REPL_PORT_AGG_MAPr, port, 0, regval));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_aggid_add
 * Purpose:
 *      Increase aggid refcnt of a independ port or trunk.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      port  - (IN)  Logical Port.
 *      tgid  - (IN)  Trunk id.
 *      aggid - (OUT) Returned aggid.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th_aggid_add(int unit, bcm_port_t port, bcm_trunk_t tgid, int *aggid)
{
    int rv = BCM_E_NONE;
    int pipe;
    _bcm_th_trunk_aggid_info_t *trunk_aggid_info;
    _bcm_th_port_aggid_info_t *port_aggid_info;
    bcm_port_t pipe_port = port;

    if (((tgid != BCM_TRUNK_INVALID) && (tgid < 0 || tgid >= _TH_NUM_TRUNKS(unit))) ||
        (aggid == NULL)) {
        return BCM_E_PARAM;
    }
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN(
            _bcmi_coe_subport_physical_port_get(unit, port, &pipe_port));


        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&port));

    } else
#endif
    {
        if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
            return BCM_E_PARAM;
        }
    }

    pipe = SOC_INFO(unit).port_pipe[pipe_port];
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        pipe = 0;
    }
#endif
    if (tgid != BCM_TRUNK_INVALID) {
        trunk_aggid_info = &(th_trunk_aggid[unit][pipe].trunk_aggid_info[tgid]);
        if (trunk_aggid_info->agg_id == TH_AGG_ID_INVALID) {
            BCM_IF_ERROR_RETURN(bcm_th_get_free_aggid(unit, pipe, aggid));
            trunk_aggid_info->agg_id = *aggid;
            trunk_aggid_info->ref_cnt = 1;
        } else {
            *aggid = trunk_aggid_info->agg_id;
            trunk_aggid_info->ref_cnt++;
        }
    } else {
        port_aggid_info = &(th_port_aggid[unit][port]);
        if (port_aggid_info->agg_id == TH_AGG_ID_INVALID) {
            BCM_IF_ERROR_RETURN(bcm_th_get_free_aggid(unit, pipe, aggid));
            rv = bcm_th_set_port_hw_agg_map(unit, port, *aggid);
            if (BCM_FAILURE(rv)) {
                bcm_th_set_free_aggid(unit, pipe, *aggid);
                return rv;
            }
            port_aggid_info->agg_id = *aggid;
            port_aggid_info->ref_cnt = 1;
            port_aggid_info->trunk_id = BCM_TRUNK_INVALID;
        } else {
            *aggid = port_aggid_info->agg_id;
            port_aggid_info->ref_cnt++;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_aggid_del
 * Purpose:
 *      Decrease aggid refcnt of a independ port or trunk.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      port  - (IN)  Logical Port.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th_aggid_del(int unit, bcm_port_t port)
{
    int pipe;
    bcm_trunk_t tgid;
    int rv;
    _bcm_th_trunk_aggid_info_t *trunk_aggid_info;
    _bcm_th_port_aggid_info_t *port_aggid_info;
    bcm_port_t pipe_port = port;


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN(
            _bcmi_coe_subport_physical_port_get(unit, port, &pipe_port));

        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&port));

    } else
#endif
    {
        if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
            return BCM_E_PARAM;
        }
    }

    port_aggid_info = &(th_port_aggid[unit][port]);
    tgid = port_aggid_info->trunk_id;
    pipe = SOC_INFO(unit).port_pipe[pipe_port];
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        pipe = 0;
    }
#endif
    if (tgid != BCM_TRUNK_INVALID) {
        trunk_aggid_info = &(th_trunk_aggid[unit][pipe].trunk_aggid_info[tgid]);
        if (trunk_aggid_info->ref_cnt <= 0) {
            return BCM_E_PARAM;
        }
        trunk_aggid_info->ref_cnt--;
        if (trunk_aggid_info->ref_cnt == 0) {
            bcm_th_set_free_aggid(unit, pipe, trunk_aggid_info->agg_id);
            trunk_aggid_info->agg_id = TH_AGG_ID_INVALID;
        }
        if (port_aggid_info->ref_cnt == 0) {
            port_aggid_info->trunk_id = BCM_TRUNK_INVALID;
        }
    } else {
        if (port_aggid_info->ref_cnt <= 0) {
            return BCM_E_PARAM;
        }
        port_aggid_info->ref_cnt--;
        if (port_aggid_info->ref_cnt == 0) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
               rv = bcm_th_set_port_hw_agg_map(unit, port, TH3_AGG_ID_HW_INVALID);

            } else
#endif
            {
                rv = bcm_th_set_port_hw_agg_map(unit, port, TH_AGG_ID_HW_INVALID);
            }
            if (BCM_FAILURE(rv)) {
                port_aggid_info->ref_cnt++;
                return rv;
            }
            bcm_th_set_free_aggid(unit, pipe, port_aggid_info->agg_id);
            port_aggid_info->agg_id = TH_AGG_ID_INVALID;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th_aggid_ref_inc_for_member
 * Purpose:
 *      Increase aggid refcount for a trunk member port.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      port  - (IN)  Logical Port.
 *      tgid  - (IN)  Trunk id.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th_aggid_ref_inc_for_member(int unit, bcm_port_t port, bcm_trunk_t tgid)
{
    int aggid, pipe;
    _bcm_th_port_aggid_info_t *port_aggid_info;
    bcm_port_t pipe_port = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN(
            _bcmi_coe_subport_physical_port_get(unit, port, &pipe_port));

        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&port));

    } else
#endif
    {
        if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
            return BCM_E_PARAM;
        }
    }

    port_aggid_info = &(th_port_aggid[unit][port]);
    if (port_aggid_info->agg_id != TH_AGG_ID_INVALID) {
        return BCM_E_PARAM;
    }

    if (port_aggid_info->trunk_id == BCM_TRUNK_INVALID) {
        /* This is the first time that member port joined trunk tgid.
         * For member port, it just uses trunk's agg_id not itself.
         */
        if (port_aggid_info->ref_cnt != 0) {
            return BCM_E_PARAM;
        }
        port_aggid_info->trunk_id = tgid;
        port_aggid_info->ref_cnt = 1;
        pipe = SOC_INFO(unit).port_pipe[pipe_port];
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            pipe = 0;
        }
#endif
        aggid = th_trunk_aggid[unit][pipe].trunk_aggid_info[tgid].agg_id;
        BCM_IF_ERROR_RETURN(bcm_th_set_port_hw_agg_map(unit, port, aggid));
    } else {
        /* member port has joined one trunk. */
        if (port_aggid_info->trunk_id != tgid) {
            return BCM_E_PARAM;
        }
        port_aggid_info->ref_cnt++;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_aggid_ref_dec_for_member
 * Purpose:
 *      Decrease aggid refcount for a trunk member port.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN)  Logical Port.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_th_aggid_ref_dec_for_member(int unit, bcm_port_t port)
{
    _bcm_th_port_aggid_info_t *port_aggid_info;
    int rv;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&port));

    } else
#endif
    {
        if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
            return BCM_E_PARAM;
        }
    }

    port_aggid_info = &(th_port_aggid[unit][port]);
    if (port_aggid_info->trunk_id == BCM_TRUNK_INVALID) {
        return BCM_E_PARAM;
    }

    if (port_aggid_info->ref_cnt <= 0) {
        return BCM_E_PARAM;
    }

    port_aggid_info->ref_cnt--;
    if (port_aggid_info->ref_cnt == 0) {
        /* All (port, mc group) has can leave the trunk.
         * Do not set port_aggid_info->trunk_id to BCM_TRUNK_INVALID here.
         */
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            rv = bcm_th_set_port_hw_agg_map(unit, port, TH3_AGG_ID_HW_INVALID);
        } else
#endif
        {
            rv = bcm_th_set_port_hw_agg_map(unit, port, TH_AGG_ID_HW_INVALID);
        }
        if (BCM_FAILURE(rv)) {
            port_aggid_info->ref_cnt++;
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_port_aggid_add
 * Purpose:
 *      Add a aggid for a logical port.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      port  - (IN)  Logical Port.
 *      tgid  - (IN)  Trunk id.
 *      aggid - (OUT) Returned aggid.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_port_aggid_add(int unit, bcm_port_t port, bcm_trunk_t tgid, int *aggid)
{
    int rv;

    if (tgid != BCM_TRUNK_INVALID) {
        rv = bcm_th_aggid_add(unit, port, tgid, aggid);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = bcm_th_aggid_ref_inc_for_member(unit, port, tgid);
        if (BCM_FAILURE(rv)) {
            bcm_th_aggid_del(unit, port);
            return rv;
        }
    } else {
        rv = bcm_th_aggid_add(unit, port, tgid, aggid);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_port_aggid_del
 * Purpose:
 *      Del aggid of a logical port.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      port  - (IN)  Logical Port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_port_aggid_del(int unit, bcm_port_t port)
{
    _bcm_th_port_aggid_info_t *port_aggid_info;
    bcm_trunk_t tgid;
    int rv;
    bcm_port_t local_port = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        /* Now get the subport number associated with this sub-port. */
        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&local_port));

    } else
#endif
    {
        if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
            return BCM_E_PARAM;
        }
    }

    port_aggid_info = &(th_port_aggid[unit][local_port]);
    tgid = port_aggid_info->trunk_id;

    if (tgid != BCM_TRUNK_INVALID) {
        rv = bcm_th_aggid_ref_dec_for_member(unit, port);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = bcm_th_aggid_del(unit, port);
        if (BCM_FAILURE(rv)) {
            bcm_th_aggid_ref_inc_for_member(unit, port, tgid);
            return rv;
        }
    } else {
        rv = bcm_th_aggid_del(unit, port);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_port_trunkid_get
 * Purpose:
 *      When port has been removed from trunk in trunk module,
 *      we could only get trunkid of port from previous saved DB.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN) Logical Port
 *      tgid  - (OUT) returned Trunk id.
 * Returns:
 *      BCM_E_XXX
 * Note: This function only can be used when deleting port from trunk.
 */
int
bcm_th_port_trunkid_get(int unit, bcm_port_t port, int *tgid)
{
    _bcm_th_port_aggid_info_t *port_aggid_info;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        /* Now get the subport number associated with this sub-port. */
        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&port));
    } else
#endif
    {
        if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
            return BCM_E_PARAM;
        }
    }


    if (tgid == NULL) {
        return BCM_E_PARAM;
    }

    port_aggid_info = &(th_port_aggid[unit][port]);
    *tgid = port_aggid_info->trunk_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_port_last_member_check
 * Purpose:
 *      Check whether a member port is the last member of its trunk.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      port  - (IN)  Logical Port
 *      val   - (OUT) returned val.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_port_last_member_check(int unit, bcm_port_t port, int *val)
{
    int pipe;
    bcm_trunk_t tgid;
    _bcm_th_port_aggid_info_t *port_aggid_info;
    int i;
    int member_cnt = 0;
    bcm_port_t local_port, pipe_port;
    int total_num_ports = SOC_MAX_NUM_PORTS;

    local_port = port;
    pipe_port = port;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN(
            _bcmi_coe_subport_physical_port_get(unit, port, &pipe_port));

        /* Now get the subport number associated with this sub-port. */
        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, (int *)&local_port));

        total_num_ports = _FB6_TOTAL_NUM_PORT;
    } else
#endif
    {
        if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
            return BCM_E_PARAM;
        }
    }


    pipe = SOC_INFO(unit).port_pipe[pipe_port];

    port_aggid_info = &(th_port_aggid[unit][local_port]);
    tgid = port_aggid_info->trunk_id;
    if (tgid == BCM_TRUNK_INVALID) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < total_num_ports; i++) {
        if (member_cnt > 1) {
            *val = FALSE;
            return BCM_E_NONE;
        }
        port_aggid_info = &(th_port_aggid[unit][i]);
#ifdef BCM_TOMAHAWK3_SUPPORT
       if (SOC_IS_TOMAHAWK3(unit)) {
           if (port_aggid_info->trunk_id == tgid) {
               member_cnt++;
           }
       } else
#endif
        if ((port_aggid_info->trunk_id == tgid) &&
            (SOC_INFO(unit).port_pipe[i] == pipe)) {
            member_cnt++;
        }
    }

    if (member_cnt > 1) {
        *val = FALSE;
    } else {
        *val = TRUE;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_port_to_aggid
 * Purpose:
 *      Get aggid value for a logical port.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      aggid - (OUT) Returned aggid.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_th_port_to_aggid(int unit, bcm_port_t port, int *aggid)
{
    int pipe;
    bcm_trunk_t tgid;
    _bcm_th_port_aggid_info_t *port_aggid_info;
    _bcm_th_trunk_aggid_info_t *trunk_aggid_info;
    int port_num = 0, pipe_port = port;


#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    if ((soc_feature(unit, soc_feature_channelized_switching)) &&
        BCM_GPORT_IS_SET(port) &&
        _BCM_COE_GPORT_IS_SUBTAG_SUBPORT_PORT(unit, port)) {
        int subport_num = 0;

        BCM_IF_ERROR_RETURN(
            _bcmi_coe_subport_physical_port_get(unit, port, &pipe_port));


        BCM_IF_ERROR_RETURN(
            _bcm_coe_subtag_subport_port_subport_num_get(unit,
            port, NULL, &subport_num));

        port_num = subport_num;
    }  else
#endif
    {
        port_num = port;
    }
    pipe = SOC_INFO(unit).port_pipe[pipe_port];
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        pipe = 0;
    }
#endif
    port_aggid_info = &(th_port_aggid[unit][port_num]);
    tgid = port_aggid_info->trunk_id;
    if (tgid == BCM_TRUNK_INVALID) {
        *aggid = port_aggid_info->agg_id;
    } else {
        trunk_aggid_info = &(th_trunk_aggid[unit][pipe].trunk_aggid_info[tgid]);
        *aggid = trunk_aggid_info->agg_id;
    }
    if (*aggid == TH_AGG_ID_INVALID) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      bcm_th_ipmc_aggid_info_scache_size_get
 * Purpose:
 *      Get the required scache size for storing aggid info.
 * Parameters:
 *      unit - (IN)  StrataSwitch unit #
 *      size - (OUT) Number of bytes
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th_ipmc_aggid_info_scache_size_get(
    int unit, uint32 *size)
{
    int   num_pipes;
    int aggid_used_bmp_size, trunk_aggid_size, port_aggid_size;

    *size = 0;

    num_pipes = NUM_PIPE(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        /* IPMC repl is pipe independent in TH3 */
        num_pipes = 1;
    }
#endif
    aggid_used_bmp_size =
        num_pipes * SHR_BITALLOCSIZE(_TH_NUM_AGGID_PER_PIPE(unit));
    *size += aggid_used_bmp_size;

    trunk_aggid_size =
        num_pipes * (_TH_NUM_TRUNKS(unit) * sizeof(_bcm_th_trunk_aggid_info_t));
    *size += trunk_aggid_size;

    port_aggid_size = _TH_DEVICE_NUM_PORTS(unit) * sizeof(_bcm_th_port_aggid_info_t);
    *size += port_aggid_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_th_ipmc_aggid_info_sync
 * Purpose:
 *      Sync aggid info to scache.
 * Parameters:
 *      unit       - (IN)     StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int bcm_th_ipmc_aggid_info_sync(int unit, uint8 **scache_ptr)
{
    int num_pipes;
    int aggid_used_bmp_size, trunk_aggid_size, port_aggid_size;
    int i;

    num_pipes = NUM_PIPE(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        /* IPMC repl is pipe independent in TH3 */
        num_pipes = 1;
    }
#endif
    aggid_used_bmp_size = SHR_BITALLOCSIZE(_TH_NUM_AGGID_PER_PIPE(unit));
    for (i = 0; i < num_pipes; i++) {
        sal_memcpy((*scache_ptr), th_aggid_used_bmp[unit][i].pipe_aggid_bmp,
                   aggid_used_bmp_size);
        (*scache_ptr) += aggid_used_bmp_size;
    }

    trunk_aggid_size = _TH_NUM_TRUNKS(unit) * sizeof(_bcm_th_trunk_aggid_info_t);
    for (i = 0; i < num_pipes; i++) {
        sal_memcpy((*scache_ptr), th_trunk_aggid[unit] + i, trunk_aggid_size);
        (*scache_ptr) += trunk_aggid_size;
    }

    port_aggid_size = _TH_DEVICE_NUM_PORTS(unit) * sizeof(_bcm_th_port_aggid_info_t);
    sal_memcpy((*scache_ptr), th_port_aggid[unit], port_aggid_size);
    (*scache_ptr) += port_aggid_size;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_th_ipmc_aggid_info_recover
 * Purpose:
 *      Recover aggid info from scache.
 * Parameters:
 *      unit       - (IN)     StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_th_ipmc_aggid_info_recover(int unit, uint8 **scache_ptr)
{
    int   num_pipes;
    int aggid_used_bmp_size, trunk_aggid_size, port_aggid_size;
    int i;
    int max_num_ports, scache_sz;
    int max_ports = _TH_DEVICE_NUM_PORTS(unit);

    max_num_ports = soc_scache_dictionary_entry_get(unit, 
                                 ssden_SOC_MAX_NUM_PORTS,
                                 max_ports);

    if (SOC_IS_FIREBOLT6(unit)) {
        max_num_ports = _TH_DEVICE_NUM_PORTS(unit);
    }

    num_pipes = NUM_PIPE(unit);
#ifdef BCM_TOMAHAWK3_SUPPORT
    if(SOC_IS_TOMAHAWK3(unit)) {
        /* IPMC repl is pipe independent in TH3 */
        num_pipes = 1;
    }
#endif
    aggid_used_bmp_size = SHR_BITALLOCSIZE(_TH_NUM_AGGID_PER_PIPE(unit));
    for (i = 0; i < num_pipes; i++) {
        sal_memcpy(th_aggid_used_bmp[unit][i].pipe_aggid_bmp,
                   (*scache_ptr), aggid_used_bmp_size);
        (*scache_ptr) += aggid_used_bmp_size;
    }

    trunk_aggid_size = _TH_NUM_TRUNKS(unit) * sizeof(_bcm_th_trunk_aggid_info_t);
    for (i = 0; i < num_pipes; i++) {
        sal_memcpy(th_trunk_aggid[unit] + i, (*scache_ptr), trunk_aggid_size);
        (*scache_ptr) += trunk_aggid_size;
    }

    port_aggid_size = _TH_DEVICE_NUM_PORTS(unit) * sizeof(_bcm_th_port_aggid_info_t);
    scache_sz = max_num_ports * sizeof(_bcm_th_port_aggid_info_t);

    SOC_SCACHE_SIZE_MIN_RECOVER(*scache_ptr, th_port_aggid[unit], 
                                scache_sz, port_aggid_size);

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#endif

