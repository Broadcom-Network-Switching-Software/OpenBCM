/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        multicast.c
 * Purpose:     Trident2Plus multicast functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>
#if defined(BCM_TRIDENT2PLUS_SUPPORT) && defined(INCLUDE_L3)
#include <soc/drv.h>
#include <soc/scache_dictionary.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm_int/esw/trident2plus.h>
#define _TD2P_NUM_TRUNKS 1024


#define _TD2P_NUM_AGGID_PER_PIPE(unit)  \
        (soc_mem_field_length(unit, MMU_REPL_GROUP_INFO0m, PIPE_MEMBER_BMPf))

typedef struct _bcm_td2p_aggid_used_bmp_per_pipe_s {
    SHR_BITDCL* pipe_aggid_bmp;
} _bcm_td2p_aggid_used_bmp_per_pipe_t;

STATIC _bcm_td2p_aggid_used_bmp_per_pipe_t *td2p_aggid_used_bmp[BCM_MAX_NUM_UNITS];

typedef struct _bcm_td2p_trunk_aggid_info_s {
    int agg_id;
    int ref_cnt;
} _bcm_td2p_trunk_aggid_info_t;

typedef struct _bcm_td2p_trunk_aggid_per_pipe_info_s {
    _bcm_td2p_trunk_aggid_info_t trunk_aggid_info[_TD2P_NUM_TRUNKS];
} _bcm_td2p_trunk_aggid_per_pipe_info_t;

typedef struct _bcm_td2p_port_aggid_info_s {
    int agg_id;
    int ref_cnt;   /* The number of the pair <mc group, nexthop> pointing to it. */
    int trunk_id;
} _bcm_td2p_port_aggid_info_t;

STATIC _bcm_td2p_trunk_aggid_per_pipe_info_t *td2p_trunk_aggid[BCM_MAX_NUM_UNITS];

STATIC _bcm_td2p_port_aggid_info_t *td2p_port_aggid[BCM_MAX_NUM_UNITS];

/*
 * Function:
 *      bcm_td2p_aggid_info_detach
 * Purpose:
 *      Free aggregation ID list.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_aggid_info_detach(int unit)
{
    int num_pipes;
    int i;
    SHR_BITDCL *pipe_aggid_bmp;

    num_pipes = NUM_PIPE(unit);

    if (td2p_aggid_used_bmp[unit] != NULL) {
        for (i = 0; i < num_pipes; i++) {
            pipe_aggid_bmp = td2p_aggid_used_bmp[unit][i].pipe_aggid_bmp;
            if (pipe_aggid_bmp != NULL) {
                sal_free(pipe_aggid_bmp);
                pipe_aggid_bmp = NULL;
            }
        }
        sal_free(td2p_aggid_used_bmp[unit]);
        td2p_aggid_used_bmp[unit] = NULL;
    }

    if (td2p_trunk_aggid[unit] != NULL) {
        sal_free(td2p_trunk_aggid[unit]);
        td2p_trunk_aggid[unit] = NULL;
    }

    if (td2p_port_aggid[unit] != NULL) {
        sal_free(td2p_port_aggid[unit]);
        td2p_port_aggid[unit] = NULL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_aggid_info_init
 * Purpose:
 *      Initialize aggregation ID to trunk map array.
 * Parameters:
 *      unit  - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_aggid_info_init(int unit)
{
    int   num_pipes;
    int   i, j;
    _bcm_td2p_trunk_aggid_info_t *trunk_aggid_info;
    _bcm_td2p_port_aggid_info_t *port_aggid_info;


    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }

    num_pipes = NUM_PIPE(unit);

    bcm_td2p_aggid_info_detach(unit);


    td2p_aggid_used_bmp[unit] =
        sal_alloc((num_pipes * sizeof(_bcm_td2p_aggid_used_bmp_per_pipe_t)),
                  "aggid_used_bmp_info");
    if (td2p_aggid_used_bmp[unit] == NULL) {
        return BCM_E_MEMORY;
    }
    for (i = 0; i < num_pipes; i++) {
        td2p_aggid_used_bmp[unit][i].pipe_aggid_bmp =
            sal_alloc(SHR_BITALLOCSIZE(_TD2P_NUM_AGGID_PER_PIPE(unit)),
                      "aggid_used_bmp_per_piep_info");
        if (td2p_aggid_used_bmp[unit][i].pipe_aggid_bmp == NULL) {
            bcm_td2p_aggid_info_detach(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(td2p_aggid_used_bmp[unit][i].pipe_aggid_bmp, 0,
                   SHR_BITALLOCSIZE(_TD2P_NUM_AGGID_PER_PIPE(unit)));
    }

    /* Init trunk aggid info. */
    td2p_trunk_aggid[unit] =
        sal_alloc((num_pipes * sizeof(_bcm_td2p_trunk_aggid_per_pipe_info_t)),
                  "trunk_aggid_info");
    if (td2p_trunk_aggid[unit] == NULL) {
        bcm_td2p_aggid_info_detach(unit);
        return BCM_E_MEMORY;
    }
    for (i = 0; i < num_pipes; i++) {
        for (j = 0; j < _TD2P_NUM_TRUNKS; j++) {
            trunk_aggid_info = &(td2p_trunk_aggid[unit][i].trunk_aggid_info[j]);
            trunk_aggid_info->agg_id = TD2P_AGG_ID_INVALID;
            trunk_aggid_info->ref_cnt = 0;
        }
    }

    /* Init port aggid info. */
    td2p_port_aggid[unit] =
        sal_alloc((SOC_MAX_NUM_PORTS * sizeof(_bcm_td2p_port_aggid_info_t)),
                   "port_aggid_info");
    if (td2p_port_aggid[unit] == NULL) {
        bcm_td2p_aggid_info_detach(unit);
        return BCM_E_MEMORY;
    }
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        port_aggid_info = &td2p_port_aggid[unit][i];
        port_aggid_info->agg_id = TD2P_AGG_ID_INVALID;
        port_aggid_info->ref_cnt = 0;
        port_aggid_info->trunk_id = BCM_TRUNK_INVALID;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_get_free_aggid
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
bcm_td2p_get_free_aggid(int unit, int pipe, int *aggid)
{
    SHR_BITDCL *aggid_bmp;
    int i;

    aggid_bmp = td2p_aggid_used_bmp[unit][pipe].pipe_aggid_bmp;
    for (i = 0; i < _TD2P_NUM_AGGID_PER_PIPE(unit); i++) {
        if (!SHR_BITGET(aggid_bmp, i)) {
            *aggid = i;
            SHR_BITSET(aggid_bmp, i);
            return BCM_E_NONE;
        }
    }

    if (i == _TD2P_NUM_AGGID_PER_PIPE(unit)) {
        return BCM_E_FULL;
    }

    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_td2p_set_free_aggid
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
bcm_td2p_set_free_aggid(int unit, int pipe, int aggid)
{
    SHR_BITDCL *aggid_bmp;

    aggid_bmp = td2p_aggid_used_bmp[unit][pipe].pipe_aggid_bmp;
    if (aggid >= _TD2P_NUM_AGGID_PER_PIPE(unit)) {
        return BCM_E_PARAM;
    }
    SHR_BITCLR(aggid_bmp, aggid);

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_set_port_hw_agg_map
 * Purpose:
 *      Set HW aggid map register for a port.
 *      Since L3MC-Port-Agg-ID is used per egress pipe, the maximum L3MC-Port-Agg-IDs
 *      needs to match the maximum number of ports per pipe, which is 53.
 *      Therefore, only the lower six bits are needed for L3MC-Port-Agg-ID by the hardware.
 *      But for the ENQ stage regregiter used in cut-through mode,
 *      PIPE_NUM field must be programmed because L3MC-Port-Agg-ID is used per chip.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN) Logical Port.
 *      aggid - (IN) Aggregation ID.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_set_port_hw_agg_map(int unit, bcm_port_t port, int aggid)
{
    uint32 regval;
    int pipe_num = SOC_INFO(unit).port_pipe[port];

    BCM_IF_ERROR_RETURN(READ_MMU_TOQ_REPL_PORT_AGG_MAPr(unit, port, &regval));
    soc_reg_field_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr, &regval,
                      L3MC_PORT_AGG_IDf, aggid);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, MMU_TOQ_REPL_PORT_AGG_MAPr, port, 0, regval));

    soc_reg_field_set(unit, MMU_ENQ_REPL_PORT_AGG_MAPr, &regval,
                      L3MC_PORT_AGG_IDf, aggid);
    soc_reg_field_set(unit, MMU_ENQ_REPL_PORT_AGG_MAPr, &regval,
                      PIPE_NUMf, pipe_num);
    SOC_IF_ERROR_RETURN(
        soc_reg32_set(unit, MMU_ENQ_REPL_PORT_AGG_MAPr, port, 0, regval));

    soc_reg_field_set(unit, MMU_ENQ_LOGICAL_PORT_TO_PORT_AGG_MAPr, &regval,
                      L3MC_PORT_AGG_IDf, aggid);
    soc_reg_field_set(unit, MMU_ENQ_REPL_PORT_AGG_MAPr, &regval,
                      PIPE_NUMf, pipe_num);
    SOC_IF_ERROR_RETURN(WRITE_MMU_ENQ_LOGICAL_PORT_TO_PORT_AGG_MAPr(unit,
                        port, regval));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_aggid_add
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
bcm_td2p_aggid_add(int unit, bcm_port_t port, bcm_trunk_t tgid, int *aggid)
{
    int rv = BCM_E_NONE;
    int pipe;
    _bcm_td2p_trunk_aggid_info_t *trunk_aggid_info;
    _bcm_td2p_port_aggid_info_t *port_aggid_info;

    if (((tgid != BCM_TRUNK_INVALID) && (tgid < 0 || tgid >= _TD2P_NUM_TRUNKS)) ||
        (port < 0 || port >= SOC_MAX_NUM_PORTS) ||
        (aggid == NULL)) {
        return BCM_E_PARAM;
    }

    pipe = SOC_INFO(unit).port_pipe[port];
    if (tgid != BCM_TRUNK_INVALID) {
        trunk_aggid_info = &(td2p_trunk_aggid[unit][pipe].trunk_aggid_info[tgid]);
        if (trunk_aggid_info->agg_id == TD2P_AGG_ID_INVALID) {
            BCM_IF_ERROR_RETURN(bcm_td2p_get_free_aggid(unit, pipe, aggid));
            trunk_aggid_info->agg_id = *aggid;
            trunk_aggid_info->ref_cnt = 1;
        } else {
            *aggid = trunk_aggid_info->agg_id;
            trunk_aggid_info->ref_cnt++;
        }
    } else {
        port_aggid_info = &(td2p_port_aggid[unit][port]);
        if (port_aggid_info->agg_id == TD2P_AGG_ID_INVALID) {
            BCM_IF_ERROR_RETURN(bcm_td2p_get_free_aggid(unit, pipe, aggid));
            rv = bcm_td2p_set_port_hw_agg_map(unit, port, *aggid);
            if (BCM_FAILURE(rv)) {
                bcm_td2p_set_free_aggid(unit, pipe, *aggid);
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
 *      bcm_td2p_aggid_del
 * Purpose:
 *      Decrease aggid refcnt of a independ port or trunk.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      port  - (IN)  Logical Port.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_td2p_aggid_del(int unit, bcm_port_t port)
{
    int pipe;
    bcm_trunk_t tgid;
    int rv;
    _bcm_td2p_trunk_aggid_info_t *trunk_aggid_info;
    _bcm_td2p_port_aggid_info_t *port_aggid_info;

    if ((port < 0) || (port >= SOC_MAX_NUM_PORTS)) {
        return BCM_E_PARAM;
    }
    port_aggid_info = &(td2p_port_aggid[unit][port]);
    tgid = port_aggid_info->trunk_id;
    pipe = SOC_INFO(unit).port_pipe[port];
    if (tgid != BCM_TRUNK_INVALID) {
        trunk_aggid_info = &(td2p_trunk_aggid[unit][pipe].trunk_aggid_info[tgid]);
        if (trunk_aggid_info->ref_cnt <= 0) {
            return BCM_E_PARAM;
        }
        trunk_aggid_info->ref_cnt--;
        if (trunk_aggid_info->ref_cnt == 0) {
            bcm_td2p_set_free_aggid(unit, pipe, trunk_aggid_info->agg_id);
            trunk_aggid_info->agg_id = TD2P_AGG_ID_INVALID;
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
            rv = bcm_td2p_set_port_hw_agg_map(unit, port, TD2P_AGG_ID_HW_INVALID);
            if (BCM_FAILURE(rv)) {
                port_aggid_info->ref_cnt++;
                return rv;
            }
            bcm_td2p_set_free_aggid(unit, pipe, port_aggid_info->agg_id);
            port_aggid_info->agg_id = TD2P_AGG_ID_INVALID;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_td2p_aggid_ref_inc_for_member
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
bcm_td2p_aggid_ref_inc_for_member(int unit, bcm_port_t port, bcm_trunk_t tgid)
{
    int aggid, pipe;
    _bcm_td2p_port_aggid_info_t *port_aggid_info;

    if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
        return BCM_E_PARAM;
    }

    port_aggid_info = &(td2p_port_aggid[unit][port]);
    if (port_aggid_info->agg_id != TD2P_AGG_ID_INVALID) {
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
        pipe = SOC_INFO(unit).port_pipe[port];
        aggid = td2p_trunk_aggid[unit][pipe].trunk_aggid_info[tgid].agg_id;
        BCM_IF_ERROR_RETURN(bcm_td2p_set_port_hw_agg_map(unit, port, aggid));
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
 *      bcm_td2p_aggid_ref_dec_for_member
 * Purpose:
 *      Decrease aggid refcount for a trunk member port.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN)  Logical Port.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
bcm_td2p_aggid_ref_dec_for_member(int unit, bcm_port_t port)
{
    _bcm_td2p_port_aggid_info_t *port_aggid_info;
    int rv;

    if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
        return BCM_E_PARAM;
    }

    port_aggid_info = &(td2p_port_aggid[unit][port]);
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
        rv = bcm_td2p_set_port_hw_agg_map(unit, port, TD2P_AGG_ID_HW_INVALID);
        if (BCM_FAILURE(rv)) {
            port_aggid_info->ref_cnt++;
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_port_aggid_add
 * Purpose:
 *      Add a aggid for a logical port.
 *      If there is no aggid allocated for the logical port, a new aggid
 *      will be allocated for it. Otherwise, the reference count will be
 *      incremented.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      port  - (IN)  Logical Port.
 *      tgid  - (IN)  Trunk id.
 *      aggid - (OUT) Aggid returned.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_port_aggid_add(int unit, bcm_port_t port, bcm_trunk_t tgid, int *aggid)
{
    int rv;

    if (tgid != BCM_TRUNK_INVALID) {
        rv = bcm_td2p_aggid_add(unit, port, tgid, aggid);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = bcm_td2p_aggid_ref_inc_for_member(unit, port, tgid);
        if (BCM_FAILURE(rv)) {
            bcm_td2p_aggid_del(unit, port);
            return rv;
        }
    } else {
        rv = bcm_td2p_aggid_add(unit, port, tgid, aggid);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_port_aggid_del
 * Purpose:
 *      Del aggid of a logical port.
 *      If there is the reference count is greater than 1, the reference count
 *      will be decreased. Otherwise, the aggid will be freed.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      port  - (IN)  Logical Port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_port_aggid_del(int unit, bcm_port_t port)
{
    _bcm_td2p_port_aggid_info_t *port_aggid_info;
    bcm_trunk_t tgid;
    int rv;

    if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
        return BCM_E_PARAM;
    }

    port_aggid_info = &(td2p_port_aggid[unit][port]);
    tgid = port_aggid_info->trunk_id;

    if (tgid != BCM_TRUNK_INVALID) {
        rv = bcm_td2p_aggid_ref_dec_for_member(unit, port);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
        rv = bcm_td2p_aggid_del(unit, port);
        if (BCM_FAILURE(rv)) {
            bcm_td2p_aggid_ref_inc_for_member(unit, port, tgid);
            return rv;
        }
    } else {
        rv = bcm_td2p_aggid_del(unit, port);
        if (BCM_FAILURE(rv)) {
            return rv;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_port_trunkid_get
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
bcm_td2p_port_trunkid_get(int unit, bcm_port_t port, int *tgid)
{
    _bcm_td2p_port_aggid_info_t *port_aggid_info;

    if ((port < 0 || port >= SOC_MAX_NUM_PORTS) ||
        (tgid == NULL)) {
        return BCM_E_PARAM;
    }

    port_aggid_info = &(td2p_port_aggid[unit][port]);
    *tgid = port_aggid_info->trunk_id;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_port_last_member_check
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
bcm_td2p_port_last_member_check(int unit, bcm_port_t port, int *val)
{
    int pipe;
    bcm_trunk_t tgid;
    _bcm_td2p_port_aggid_info_t *port_aggid_info;
    int i;
    int member_cnt = 0;

    if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
        return BCM_E_PARAM;
    }

    pipe = SOC_INFO(unit).port_pipe[port];
    port_aggid_info = &(td2p_port_aggid[unit][port]);
    tgid = port_aggid_info->trunk_id;
    if (tgid == BCM_TRUNK_INVALID) {
        return BCM_E_PARAM;
    }

    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        if (member_cnt > 1) {
            *val = FALSE;
            return BCM_E_NONE;
        }
        port_aggid_info = &(td2p_port_aggid[unit][i]);
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
 *      bcm_td2p_port_to_aggid
 * Purpose:
 *      Get aggid value for a logical port.
 * Parameters:
 *      unit  - (IN)  Unit number.
 *      aggid - (OUT) Returned aggid.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_td2p_port_to_aggid(int unit, bcm_port_t port, int *aggid)
{
    int pipe;
    bcm_trunk_t tgid;
    _bcm_td2p_port_aggid_info_t *port_aggid_info;
    _bcm_td2p_trunk_aggid_info_t *trunk_aggid_info;

    if (port < 0 || port >= SOC_MAX_NUM_PORTS) {
        return BCM_E_PARAM;
    }

    pipe = SOC_INFO(unit).port_pipe[port];
    port_aggid_info = &(td2p_port_aggid[unit][port]);
    tgid = port_aggid_info->trunk_id;
    if (tgid == BCM_TRUNK_INVALID) {
        *aggid = port_aggid_info->agg_id;
    } else {
        trunk_aggid_info = &(td2p_trunk_aggid[unit][pipe].trunk_aggid_info[tgid]);
        *aggid = trunk_aggid_info->agg_id;
    }
    if (*aggid == TD2P_AGG_ID_INVALID) {
        return BCM_E_INTERNAL;
    }

    return BCM_E_NONE;
}


#ifdef BCM_WARM_BOOT_SUPPORT
/*
 * Function:
 *      bcm_td2p_ipmc_aggid_info_scache_size_get
 * Purpose:
 *      Get the required scache size for storing aggid info.
 * Parameters:
 *      unit - (IN)  StrataSwitch unit #
 *      size - (OUT) Number of bytes
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2p_ipmc_aggid_info_scache_size_get(
    int unit, uint32 *size)
{
    int   num_pipes;
    int aggid_used_bmp_size, trunk_aggid_size, port_aggid_size;

    *size = 0;

    num_pipes = NUM_PIPE(unit);

    aggid_used_bmp_size =
        num_pipes * SHR_BITALLOCSIZE(_TD2P_NUM_AGGID_PER_PIPE(unit));
    *size += aggid_used_bmp_size;

    trunk_aggid_size =
        num_pipes * sizeof(_bcm_td2p_trunk_aggid_per_pipe_info_t);
    *size += trunk_aggid_size;

    port_aggid_size = SOC_MAX_NUM_PORTS * sizeof(_bcm_td2p_port_aggid_info_t);
    *size += port_aggid_size;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_td2p_ipmc_aggid_info_sync
 * Purpose:
 *      Sync aggid info to scache.
 * Parameters:
 *      unit       - (IN)     StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int bcm_td2p_ipmc_aggid_info_sync(int unit, uint8 **scache_ptr)
{
    int num_pipes;
    int aggid_used_bmp_size, trunk_aggid_size, port_aggid_size;
    int i;

    num_pipes = NUM_PIPE(unit);

    aggid_used_bmp_size = SHR_BITALLOCSIZE(_TD2P_NUM_AGGID_PER_PIPE(unit));
    for (i = 0; i < num_pipes; i++) {
        sal_memcpy((*scache_ptr), td2p_aggid_used_bmp[unit][i].pipe_aggid_bmp,
                   aggid_used_bmp_size);
        (*scache_ptr) += aggid_used_bmp_size;
    }

    trunk_aggid_size = sizeof(_bcm_td2p_trunk_aggid_per_pipe_info_t);
    for (i = 0; i < num_pipes; i++) {
        sal_memcpy((*scache_ptr), td2p_trunk_aggid[unit] + i, trunk_aggid_size);
        (*scache_ptr) += trunk_aggid_size;
    }

    port_aggid_size = SOC_MAX_NUM_PORTS * sizeof(_bcm_td2p_port_aggid_info_t);
    sal_memcpy((*scache_ptr), td2p_port_aggid[unit], port_aggid_size);
    (*scache_ptr) += port_aggid_size;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcm_td2p_ipmc_aggid_info_recover
 * Purpose:
 *      Recover aggid info from scache.
 * Parameters:
 *      unit       - (IN)     StrataSwitch unit #
 *      scache_ptr - (IN/OUT) Scache pointer
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_td2p_ipmc_aggid_info_recover(int unit, uint8 **scache_ptr)
{
    int   num_pipes;
    int aggid_used_bmp_size, trunk_aggid_size, port_aggid_size;
    int i;
    int max_num_ports, scache_sz;

    max_num_ports = soc_scache_dictionary_entry_get(unit, 
                                 ssden_SOC_MAX_NUM_PORTS,
                                 SOC_MAX_NUM_PORTS);
    num_pipes = NUM_PIPE(unit);

    aggid_used_bmp_size = SHR_BITALLOCSIZE(_TD2P_NUM_AGGID_PER_PIPE(unit));
    for (i = 0; i < num_pipes; i++) {
        sal_memcpy(td2p_aggid_used_bmp[unit][i].pipe_aggid_bmp,
                   (*scache_ptr), aggid_used_bmp_size);
        (*scache_ptr) += aggid_used_bmp_size;
    }

    trunk_aggid_size = sizeof(_bcm_td2p_trunk_aggid_per_pipe_info_t);
    for (i = 0; i < num_pipes; i++) {
        sal_memcpy(td2p_trunk_aggid[unit] + i, (*scache_ptr), trunk_aggid_size);
        (*scache_ptr) += trunk_aggid_size;
    }

    port_aggid_size = SOC_MAX_NUM_PORTS * sizeof(_bcm_td2p_port_aggid_info_t);
    scache_sz = max_num_ports * sizeof(_bcm_td2p_port_aggid_info_t);

    SOC_SCACHE_SIZE_MIN_RECOVER(*scache_ptr, td2p_port_aggid[unit], 
                                scache_sz, port_aggid_size);

    return BCM_E_NONE;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

#endif

