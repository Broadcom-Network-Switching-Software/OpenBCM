/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Soc_petra-B Multicast Implementation
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MCAST
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <sal/core/alloc.h>

#include <soc/drv.h>

#include "bcm_int/common/debug.h"
#include <bcm_int/control.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/multicast.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/common/trunk.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/l3.h>
#include <bcm_int/dpp/trill.h>
#include <bcm_int/dpp/switch.h>

#include <soc/sand/sand_mem.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/TMC/tmc_api_multicast_egress.h>
#include <soc/dpp/TMC/tmc_api_general.h>
#include <soc/dpp/multicast_imp.h>

#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/QAX/qax_multicast_imp.h>


#ifdef CRASH_RECOVERY_SUPPORT
#include <soc/hwstate/hw_log.h>
#include <soc/dcmn/dcmn_crash_recovery.h>
#endif /* CRASH_RECOVERY_SUPPORT */

/* defines */
#define _BCM_PETRA_MULTICAST_TYPE_UNSUPPORTED_FLAGS \
( BCM_MULTICAST_TYPE_WLAN | BCM_MULTICAST_TYPE_VLAN )

 
#define SOC_SAND_TRUE  1
#define SOC_SAND_FALSE 0

#define _BCM_TRILL_ENCAP_PORT_SHIFT         16

#define BCM_PETRA_MULTICAST_NOF_BRIDGE_ROUTE    (4096)

#define BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE 8 /* use arrays of this size on the stack to prevent the need of small allocs */

/*
 * Function:
 *      _bcm_petra_multicast_is_supported_mc_group
 * Purpose:
 *      Derive mc group and check is it a supported mc type
 * Parameters:
 *      group        (IN) BCM Multicast
 */
int
_bcm_petra_multicast_is_supported_mc_group(bcm_multicast_t group)
{
    int type;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    type = _BCM_MULTICAST_TYPE_GET(group);
    if (type != _BCM_MULTICAST_TYPE_L2 && type != _BCM_MULTICAST_TYPE_L3 
        && type != _BCM_MULTICAST_TYPE_VPLS && type != _BCM_MULTICAST_TYPE_TRILL
        && type != _BCM_MULTICAST_TYPE_MIM && type != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_NO_UNIT("MC group not supported")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_arad_eg_multicast_is_supported_mc_group
 * Purpose:
 *      Check if the type of the multicast id is supported and report of the given type is a bitmap group
 * Parameters:
 *      group        (IN) BCM Multicast
 */
int
_bcm_arad_eg_multicast_is_supported_mc_group(int unit, bcm_multicast_t group, int *is_bitmap)
{
    int type;

    BCMDNX_INIT_FUNC_DEFS;
    *is_bitmap = 0;
    type = _BCM_MULTICAST_TYPE_GET(group);
    if (type == _BCM_MULTICAST_TYPE_PORTS_GROUP
      ) {
        *is_bitmap = 1;
    } else if (type != _BCM_MULTICAST_TYPE_L2 && type != _BCM_MULTICAST_TYPE_L3 
        && type != _BCM_MULTICAST_TYPE_VPLS && type != _BCM_MULTICAST_TYPE_TRILL
        && type != _BCM_MULTICAST_TYPE_MIM && type != 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG_NO_UNIT("MC group not supported")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_group_to_id
 * Purpose:
 *      Convert a BCM-Multicast ID to Soc_petra-Multicast ID
 * Parameters:
 *      group        (IN) BCM Multicast
 *      multicast_id (OUT) Soc_petra Multicast ID
 */
int
 _bcm_petra_multicast_group_to_id(bcm_multicast_t group, SOC_TMC_MULT_ID *multicast_id)
{
     BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
     *multicast_id = _BCM_MULTICAST_ID_GET(group);

     BCM_EXIT;
exit:
     BCMDNX_FUNC_RETURN;
}

/*
 * Function_bcm_petra_multicast_group_from_id
 *      _bcm_petra_multicast_group_to_id
 * Purpose:
 *      Convert a BCM-Multicast ID from Soc_petra-Multicast ID
 * Parameters:
 *      group        (IN) BCM Multicast
 *      multicast_id (OUT) Soc_petra Multicast ID
 */
int
 _bcm_petra_multicast_group_from_id(bcm_multicast_t *group, int type, SOC_TMC_MULT_ID multicast_id)
{
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    _BCM_MULTICAST_GROUP_SET(*group, type, multicast_id);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Convert an egress bitmap group from a SOC_TMC TM ports bitmap to a local port bitmap.
 */
int
 _bcm_petra_multicast_ppd_port_bitmap_from_mult_vlan_pbmp(
    int unit,           /* input: unit/device */
    const SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *bitmap_group, /* input: TM port bitmap */
    bcm_pbmp_t *pbmp)                                          /* output: local port bitmap */
{
    unsigned words_left = (SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    uint32 bits, tm_port = 0;
    const uint32 *bits_ptr = bitmap_group->bitmap;
    int core = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(pbmp);
    BCMDNX_NULL_CHECK(bitmap_group);
    
    BCM_PBMP_CLEAR(*pbmp);

    do { /* loop over all the TM ports in the bitmap */
        bits = *(bits_ptr++);
        do { /* loop over the TM ports in the current word */
            if (bits & 1) { /* convert the found TM port to local port */
                soc_port_t local_port;
                soc_error_t res = soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &local_port);
                if (res == SOC_E_NOT_FOUND) {
                    _bsl_error(_BSL_BCM_MSG("TM port %u in core %d is replicated to, but not mapped to a local port"), tm_port, core);
                } else { /* If a local port is found, set it in the output bitmap */
                    BCMDNX_IF_ERR_EXIT(res);
                    BCM_PBMP_PORT_ADD(*pbmp, local_port);
                }
            }
            bits >>= 1;
        } while (++tm_port % SOC_SAND_NOF_BITS_IN_UINT32);
        if (--words_left % (SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32) == 0) {
            ++core; /* If finished going over the TM ports of a core, increase the core ID */
            tm_port = 0;
        }
    } while (words_left);

exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Convert local ports bitmap to a SOC_TMC TM/PP ports bitmap.
 */
int
_bcm_petra_multicast_local_ports_to_mult_vlan_pbmp(
    int unit,
    const bcm_pbmp_t *pbmp,                              /* input: local ports bitmap */
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP *bitmap_group) /* output: TM/PP ports bitmap */
{
    int i;
    unsigned nof_tm_port_words = (SOC_TMC_NOF_FAP_PORTS_PER_CORE / SOC_SAND_NOF_BITS_IN_UINT32) * SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores;
    uint32 tm_port;
    int core;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(pbmp);
    BCMDNX_NULL_CHECK(bitmap_group);
    
    for (i = 0; i < nof_tm_port_words; ++i) { /* init output bitmap to no ports */
        bitmap_group->bitmap[i] = 0;
    }

    /* Jericho needs conversion from TM ports + core to local ports, Arad needs it for OLP,ERP,... */
    BCM_PBMP_ITER(*pbmp, i) { /* loop over the local ports */
        /* convert the found local port to a TM port+core ID */
        BCMDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, i, &tm_port, &core));
        tm_port += SOC_TMC_NOF_FAP_PORTS_PER_CORE * core;
        bitmap_group->bitmap[tm_port / SOC_SAND_NOF_BITS_IN_UINT32] |= 1 << (tm_port % SOC_SAND_NOF_BITS_IN_UINT32);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_vlan_membership_range_id_check
 * Purpose:
 *      Check if given multicast id is in mc egress vlan membership range.
 * Parameters:
 *      
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is multicast group opened
 */
int
 _bcm_petra_multicast_egress_vlan_membership_range_id_check(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_in_range)
{
    BCMDNX_INIT_FUNC_DEFS;
    *is_in_range = (int32)multicast_id_ndx <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high
       ? TRUE : FALSE;

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_arad_multicast_is_group_created
 * Purpose:
 *      Check if given multicast group is open
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN)  Multicast ID.
 *      is_egress        (IN)  non zero if this is an egress group
 *      is_open          (OUT) is multicast group opened
 */
int
 _bcm_arad_multicast_is_group_created(int unit, SOC_TMC_MULT_ID mcid, int is_egress, uint8 *is_open)
{
    uint32 nof_mc_ids = is_egress ? SOC_DPP_CONFIG(unit)->tm.nof_mc_ids : SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids;
    BCMDNX_INIT_FUNC_DEFS;

    if (mcid >= nof_mc_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("multicast ID 0x%x is out of range (0 - 0x%x)"), mcid, nof_mc_ids - 1));
    }
    /* Check if this multicast group exists (possibly empty) */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_does_group_exist, (unit, mcid, is_egress, is_open)));

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_multicast_egress_is_group_exist
 * Purpose:
 *      Check if given multicast egress group id is existed.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is multicast group opened
 */
int
 _bcm_petra_multicast_egress_is_group_exist(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_open)
{
    /* Check if this multicast group exists (possibly empty) */
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, is_open));
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_is_group_exist
 * Purpose:
 *      Check if given multicast ingress group id exists.
 * Parameters: 
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is multicast group opened
 */
int
_bcm_petra_multicast_ingress_is_group_exist(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_open)
{
    /* Check if this multicast group exists (possibly empty) */
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, FALSE, is_open));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_group_exists
 * Purpose:
 *      Check if given multicast ingress group id is open, returning no errors if possible.
 * Parameters: 
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      is_open          (OUT) is ingress multicast group opened
 */
int
_bcm_petra_multicast_ingress_group_exists(int unit, SOC_TMC_MULT_ID multicast_id_ndx, uint8 *is_open)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (multicast_id_ndx >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("multicast ID 0x%x is out of range (0 - 0x%x)"),multicast_id_ndx, SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1));
    } else {
        /* Check if this multicast group exists (possibly empty) according to software data structure */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_is_group_exist(unit, multicast_id_ndx, is_open));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_is_group_exist
 * Purpose:
 *      Check if the given multicast group exists or not according to expected_open.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      type             (IN) group type.
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      expected_open    (IN) Expected multicast group status.
 */
int
_bcm_petra_multicast_is_group_exist(int unit,BCM_DPP_MULTICAST_TYPE type,SOC_TMC_MULT_ID multicast_id_ndx,uint8 expected_open)
{
    uint8 is_open_ingress, is_open_egress;
    SOC_TMC_MULT_ID nof_mc_ids = (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || (type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS &&
      SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids)) ?
      SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids : SOC_DPP_CONFIG(unit)->tm.nof_mc_ids;

    BCMDNX_INIT_FUNC_DEFS;


    is_open_egress = is_open_ingress = expected_open = expected_open ? SOC_SAND_TRUE : SOC_SAND_FALSE;
    /* Check that multicast_id_ndx is not out of range */
    if (multicast_id_ndx >= nof_mc_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("multicast ID 0x%x is out of range (0 - 0x%x)"), multicast_id_ndx, nof_mc_ids - 1));
    }

    /* Check Ingress multicast */
    if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, FALSE, &is_open_ingress));
    }
    /* Check Egress multicast */
    if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, &is_open_egress));
    }

     if (is_open_ingress != expected_open || is_open_egress != expected_open) {
        if (expected_open == SOC_SAND_TRUE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("MC group %u not open"), multicast_id_ndx));
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("MC group %u open"), multicast_id_ndx));
        }
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_close
 * Purpose:
 *      Close multicast ingress group.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_ingress_close(int unit,SOC_TMC_MULT_ID multicast_id_ndx)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_group_close,(unit,multicast_id_ndx)));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_close
 * Purpose:
 *      Close multicast egress group.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id 
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_egress_close(int unit,SOC_TMC_MULT_ID multicast_id_ndx)
{
    bcm_error_t rv;
    uint8 is_in_range;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_petra_multicast_egress_vlan_membership_range_id_check(unit,multicast_id_ndx,&is_in_range);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_in_range == TRUE) {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_group_close,(unit,multicast_id_ndx)));
    } else {
       BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_group_close,(unit,multicast_id_ndx)));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_group_close
 * Purpose:
 *      Close multicast group.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      type             (IN) group type.
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_group_close(int unit,BCM_DPP_MULTICAST_TYPE type,SOC_TMC_MULT_ID multicast_id_ndx)
{
    BCMDNX_INIT_FUNC_DEFS;
    if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_close(unit, multicast_id_ndx));
    }
    if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_close(unit, multicast_id_ndx));
    } 
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_open
 * Purpose:
 *      Open multicast ingress group.
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 */
int
_bcm_petra_multicast_ingress_open(int unit,SOC_TMC_MULT_ID multicast_id_ndx)
{
    SOC_TMC_ERROR mc_err = BCM_E_NONE;
    SOC_TMC_MULT_ING_ENTRY mc_group;
    uint32 mc_group_size = 0;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_TMC_MULT_ING_ENTRY_clear(&mc_group);

     /* Open empty multicast ingress group */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_group_open,(unit,multicast_id_ndx,&mc_group,mc_group_size,&mc_err)));

    if (mc_err) {
        if (mc_err == (SOC_TMC_ERROR) BCM_E_FULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("not enough multicast entries to create the ingress group")));
        }
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("unexpected error")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_group_open
 * Purpose:
 *      Open multicast group.
 * Parameters:
 *      unit             (IN) Deivce id
 *      type             (IN) group type.
 *      multicast_id_ndx (IN) Multicast ID.
 */
int
_bcm_petra_multicast_group_open(int unit,BCM_DPP_MULTICAST_TYPE type,SOC_TMC_MULT_ID multicast_id_ndx)
{
    uint8 is_open;
    BCMDNX_INIT_FUNC_DEFS;

        if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, FALSE, &is_open));
            if (is_open) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Ingress multicast group already created")));
            }
        }
        if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, &is_open));
            if (is_open) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("Egress multicast group already created")));
            }
        }
        /* Open Ingress group */
        if (type == BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_open(unit,multicast_id_ndx));
        }
        /* Open Egress group */
        if (type == BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY || type == BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS) {

            if ((int32)multicast_id_ndx <= SOC_DPP_CONFIG(unit)->tm.multicast_egress_bitmap_group_range.mc_id_high
              ) {

                /* if this is a bitmap, open empty multicast egress group vlan membership */
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_eg_bitmap_group_create, (unit, multicast_id_ndx)));

            } else { /* open a non bitmap group */

                SOC_TMC_ERROR mc_err = BCM_E_NONE;
                BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_eg_group_set,
                    (unit, multicast_id_ndx, 1, 0, 0, &mc_err)));
                if (mc_err) {
                    if (mc_err == (SOC_TMC_ERROR) BCM_E_FULL) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("not enough multicast entries to create the egress group")));
                    }
                    BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("unexpected error")));
                }
            }
        }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_encap_id_from_cud
 * Purpose:
 *      Convert CUD to encap_id.
 * Parameters:
 *      encap_id (OUT) bcm_if_t
 *      cud     (IN) 
 */
int
_bcm_petra_multicast_encap_id_from_cud(bcm_if_t *encap_id,uint32 cud)
{
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    *encap_id = (bcm_if_t)cud;
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_mult_eg_entry_to_local_port
 * Purpose:
 *      Convert multicast egress entry from local port
 * Parameters:
 *      entry      (IN) multicast entry
 *      local_port (OUT) local port
 *      encap_id   (OUT)
 */
int
_bcm_petra_multicast_egress_mult_eg_entry_to_local_port(SOC_TMC_MULT_EG_ENTRY entry, bcm_port_t *local_port,bcm_if_t *encap_id)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    BCMDNX_NULL_CHECK(local_port);
    BCMDNX_NULL_CHECK(encap_id);

    if (entry.type == SOC_TMC_MULT_EG_ENTRY_TYPE_OFP)
    {
        rv = _bcm_petra_multicast_encap_id_from_cud(encap_id,entry.cud);
        BCMDNX_IF_ERR_EXIT(rv);
    
        *local_port = entry.port;
    } else {
        /* Type not supported yet */
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG_NO_UNIT("Type is not supported yet")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Add/remove a replication to/from an egress multicast linked list group. Non Petra.
 */
int _bcm_petra_multicast_egress_entry_port_add_remove(
    int unit,                               /* input: unit/device */
    SOC_TMC_MULT_ID multicast_id_ndx,       /* input: egress MCID to work on */
    const uint32 nof_reps,                  /* input: number of replications */
    const dpp_mc_replication_t *reps,       /* input: replications using local logical ports */ 
    uint8 is_delete)                        /* input: if non zero this is a delete operation, otherwise an add */
{
    SOC_TMC_ERROR mc_err = BCM_E_NONE;
    int existance_error;

    BCMDNX_INIT_FUNC_DEFS;

    if (is_delete) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_reps_remove, (unit, multicast_id_ndx, nof_reps, reps, &mc_err)));
        existance_error = mc_err == (SOC_TMC_ERROR)BCM_E_NOT_FOUND;
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_reps_add, (unit, multicast_id_ndx, nof_reps, reps, &mc_err)));
        existance_error = mc_err == (SOC_TMC_ERROR)BCM_E_EXISTS;
    }
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ? "not enough multicast entries for the operation" :
          (existance_error ? "Wrong existence of the specified replication in the egress MC group" : "unexpected error")) ));
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Remove or Add a replication/port in an egress multicast bitmap.  */
STATIC int _bcm_petra_multicast_egress_vlan_membership_port_add_or_remove(
    int unit,                               /* input: unit/device */
    SOC_TMC_MULT_ID multicast_id_ndx,       /* input: egress MC group to set */
    bcm_pbmp_t *port_bm,                    /* input: replication local logical port bitmap, used instead of local_port if not NULL */
    uint8 is_delete)                        /* input: if non zero this is a delete operation, otherwise an add */
{
    SOC_TMC_ERROR mc_err = BCM_E_NONE;
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP tm_ports_bm;
    BCMDNX_INIT_FUNC_DEFS;

    /* received port bitmap, used in non Petra devices */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_local_ports_to_mult_vlan_pbmp(unit, port_bm, &tm_ports_bm)); /* Convert bitmap to TM ports */


    if (is_delete) {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_bm_remove, (unit, multicast_id_ndx, &tm_ports_bm, &mc_err)));
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_bm_add, (unit, multicast_id_ndx, &tm_ports_bm, &mc_err)));
    }
    
    if (mc_err == (SOC_TMC_ERROR)BCM_E_NOT_FOUND) {
        LOG_VERBOSE(BSL_LS_BCM_L2, 
                    (BSL_META_U(unit,
                                "_bcm_petra_multicast_egress_vlan_membership_port_add_or_remove address not found\n")));
    }
    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(mc_err);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info
 * Purpose:
 *      Convert multicast egress entry from tm dest info and encap id
 * Parameters:
 *      unit       (IN) Deivce id
 *      entry      (OUT) multicast entry
 *      local_port (IN) local port
 *      encap_id   (IN)
 */
int
_bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info(int unit, SOC_TMC_MULT_ING_ENTRY *entry, SOC_TMC_DEST_INFO dest_info,bcm_if_t encap_id)
{
    int is_sys_port = 0, rv;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(entry);

    entry->destination = dest_info;

    /* Convert encap id to CUD. */
    if (encap_id == BCM_IF_INVALID) {
        entry->cud = SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud;
    } else if (encap_id < 0 || encap_id > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid ingress encapsulation ID: 0x%x, should be between 0 and 0x%x"), encap_id, SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_cud));
    } else {
        entry->cud = encap_id;
    }

    /* In Arad check for legal destination and encode the hardware fields here. Needed for Arad+ */
        if (entry->destination.type == SOC_TMC_DEST_TYPE_QUEUE) { /* verify direct Queue_id range */
            if (entry->destination.id > SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Queue %u, is over %u."),
                  (unsigned)entry->destination.id, (unsigned)SOC_DPP_CONFIG(unit)->tm.ingress_mc_max_queue));
            }
        } else if (entry->destination.type == SOC_TMC_DEST_TYPE_MULTICAST) { /* verify multicast_id range */
#ifndef SUPPORT_ARAD_MC_DEST_ININGRESS_MC_BUT_DOES_NOT_WORK_IN_HW
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("A ingress multicast destination of type multicast is not allowed on this device")));
#else
            if (SOC_DPP_CONFIG(unit)->tm.mc_ing_encoding_mode != ARAD_MC_DEST_ENCODING_0) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("MC destinations are not allowed in non allowed in this ingress multicast encoding mode.")));
            } else if (entry->destination.id >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("multicast ID 0x%x is above 0x%x)"),
                  (unsigned)entry->destination.id, (unsigned)(SOC_DPP_CONFIG(unit)->tm.nof_mc_ids - 1)));
            }
#endif
        } else {
            is_sys_port = 1;
        }
        rv = handle_sand_result(MBCM_DPP_DRIVER_CALL(unit, /* encode hardware fields */
            mbcm_dpp_mult_ing_encode_entry,
            (unit, entry, &entry->destination.id, &entry->cud)));
        if (rv != BCM_E_NONE) {
            if (is_sys_port) { /* verify destination system port range */
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Destination system port is probably greater than or equal to %u, or LAG destination used in an unsupported mode."),
                  (unsigned)SOC_DPP_CONFIG(unit)->tm.ingress_mc_nof_sysports));
            } else {
                BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("Error encoding ingress multicast destination/cud.")));
            }
        }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_mult_ing_entry_to_tm_dest_info
 * Purpose:
 *      Convert multicast egress entry to tm dest info and encap id
 * Parameters:
 *      entry      (OUT) multicast entry
 *      local_port (IN) local port
 *      encap_id   (IN)
 */
int
_bcm_petra_multicast_ingress_mult_ing_entry_to_tm_dest_info(SOC_TMC_MULT_ING_ENTRY entry,SOC_TMC_DEST_INFO *dest_info,bcm_if_t *encap_id)
{
    bcm_error_t rv;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    BCMDNX_NULL_CHECK(dest_info);
    BCMDNX_NULL_CHECK(encap_id);

    *dest_info = entry.destination;

    rv = _bcm_petra_multicast_encap_id_from_cud(encap_id,entry.cud);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_entry_add
 * Purpose:
 *      Add new entry to multicast ingress
 * Parameters:
 *      unit             (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      dest_info        (IN) dest_info to add
 *      encap_id         (IN) encap_id
 */
int
_bcm_petra_multicast_ingress_entry_add(int unit,SOC_TMC_MULT_ID multicast_id_ndx,SOC_TMC_DEST_INFO dest_info,bcm_if_t encap_id)
{
    bcm_error_t rv;
    SOC_TMC_MULT_ING_ENTRY entry;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_TMC_MULT_ING_ENTRY_clear(&entry);

    rv =  _bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info(unit, &entry, dest_info, encap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_destination_add,(unit, multicast_id_ndx, &entry, &mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ? "not enough multicast entries to add the replication" :
          (mc_err == (SOC_TMC_ERROR) BCM_E_EXISTS ? "The added destination+encap already exists in the egress MC group " : "unexpected error")) ));
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_entry_remove
 * Purpose:
 *      Remove entry from multicast ingress
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      dest_info        (IN) dest_info to add
 *      encap_id         (IN) encap_id
 */
int
_bcm_petra_multicast_ingress_entry_remove(int unit,SOC_TMC_MULT_ID multicast_id_ndx,SOC_TMC_DEST_INFO dest_info,bcm_if_t encap_id)
{
    bcm_error_t rv;
    SOC_TMC_MULT_ING_ENTRY entry;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_TMC_MULT_ING_ENTRY_clear(&entry);

    rv =  _bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info(unit, &entry, dest_info, encap_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_destination_remove,(unit, multicast_id_ndx, &entry, &mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_NOT_FOUND ?
          "The removed replication does not exist in the ingress MC group " : "unexpected error") ));
    }
    
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}       

/*
 * Function:
 *      _bcm_petra_multicast_egress_vlan_membership_set
 * Purpose:
 *      Set entries to multicast egress vlan membership
 * Parameters:
 *      unit             (IN)
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      local_ports      (IN) local_ports bitmap
 */
int
_bcm_petra_multicast_egress_vlan_membership_set(int unit,SOC_TMC_MULT_ID multicast_id_ndx,bcm_pbmp_t local_ports)
{
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP vlan_membership_group;
    BCMDNX_INIT_FUNC_DEFS;
    
    /* Convert pbmp to multicast vlan bitmap */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_local_ports_to_mult_vlan_pbmp(unit, &local_ports, &vlan_membership_group));

    /* Update group */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_eg_vlan_membership_group_update, (unit, multicast_id_ndx, &vlan_membership_group)));

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_egress_vlan_membership_get
 * Purpose:
 *      Get entries from multicast egress vlan membership
 * Parameters:
 *      unit             (IN)
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      local_ports      (OUT) local ports bitmap
 */
int
_bcm_petra_multicast_egress_vlan_membership_get(int unit, SOC_TMC_MULT_ID multicast_id_ndx, bcm_pbmp_t *local_ports)
{
    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP vlan_membership_group;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    SOC_TMC_MULT_EG_VLAN_MEMBERSHIP_GROUP_clear(&vlan_membership_group);

    /* Get the pbmp port bitmap */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_eg_vlan_membership_group_get,(unit, multicast_id_ndx, &vlan_membership_group)));

    /* Convert multicast (TM ports) vlan bitmap to local port bitmap */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ppd_port_bitmap_from_mult_vlan_pbmp(unit, &vlan_membership_group, local_ports));
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      _bcm_petra_multicast_ingress_entries_set
 * Purpose:
 *      Set entries to multicast ingress
 * Parameters:
 *      soc_sand_dev_id      (IN) Deivce id
 *      multicast_id_ndx (IN) Soc_petra Multicast ID.
 *      dest_info_arr   (IN) array of dest info tm
 *      encap_id_arr     (IN) array of encap id
 */
int
_bcm_petra_multicast_ingress_entries_set(int unit,SOC_TMC_MULT_ID multicast_id_ndx, int port_count, SOC_TMC_DEST_INFO *dest_info_arr,bcm_if_t *encap_id_arr)
{
    SOC_TMC_MULT_ING_ENTRY *entries = NULL;
    int ndx;
    uint32 mc_group_size;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ALLOC(entries, sizeof(SOC_TMC_MULT_ING_ENTRY) * port_count, "multicast eg entries");
    if (entries == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failure")));
    }
    
    /* Clear entries */
    for (ndx = 0; ndx < port_count; ndx++)
    {
        SOC_TMC_MULT_ING_ENTRY_clear(&(entries[ndx]));
    }

    for (ndx = 0; ndx < port_count; ndx++)
    {
        /* Convert local port to mult egress entry */
       BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_mult_ing_entry_from_tm_dest_info(unit, &entries[ndx], dest_info_arr[ndx], encap_id_arr[ndx]));
    }
    
    /* Set new linked list */
    mc_group_size = port_count;
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_group_update,(unit, multicast_id_ndx, entries, mc_group_size, &mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ? "not enough multicast entries for the operation" :
          (mc_err == (SOC_TMC_ERROR) BCM_E_PARAM ? "The same replication (destination + encap) may not appear multiple times in the input" : "unexpected error")) ));
    }

exit:
    if (entries) {
        BCM_FREE(entries);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Set the contents of an ingress multicast group for QAX and later devices
 * This function is needed to be used for mbcm_dpp_mult_ing_group_update functions taking a soc_multicast_replication_t array.
 * Does not convert destination. this will be done by mbcm_dpp_mult_ing_group_update.
 */
int _bcm_qax_ingress_multicast_set(
    int unit, 
    SOC_TMC_MULT_ID mcid,
    int nof_replications, 
    bcm_multicast_replication_t *rep_array, /* input replications, if NULL use dest_array and cud_array instead */
    bcm_gport_t *dest_array, 
    bcm_if_t *cud_array)
{
    int i;
    soc_multicast_replication_t small_reps_array[BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE], *reps = NULL, *reps_i;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY, mcid, SOC_SAND_TRUE)); /* Check multicast ID exists */
    if (rep_array != NULL) { /* The input replications are in the structure we need */
        reps = (soc_multicast_replication_t *)rep_array;
    } else { /* We need to convert the structure of the input replications */
        if (nof_replications > BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE) {
            BCMDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &reps, sizeof(soc_multicast_replication_t) * nof_replications, "ing_reps")); /* Allocate ingress replicaitons */
        } else {
            reps = small_reps_array;
        }
        for (i = 0; i < nof_replications; ++i) { /* convert replications to the input needed by the soc function */
            reps_i = reps + i;
            reps_i->flags = 0;
            reps_i->port =  dest_array[i];
            reps_i->encap1 = cud_array[i];
            reps_i->encap2 = DPP_MC_NO_2ND_CUD; /* not really needed since it is not marked as valid in flags */
        }
    }

    /* Set new linked list */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_group_update,(unit, mcid, (SOC_TMC_MULT_ING_ENTRY*)reps, nof_replications, &mc_err)));
    if (mc_err) {
        BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ? "not enough multicast entries for the operation" :
          (mc_err == (SOC_TMC_ERROR) BCM_E_PARAM ? "The same replication (destination + encap) may not appear multiple times in the input" : "unexpected error")) ));
    }

exit:
    if (reps && reps != small_reps_array && reps != (soc_multicast_replication_t *)rep_array) {
        BCMDNX_IF_ERR_REPORT(sand_free_mem(unit, (void*)&reps));
    }

    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_create
 * Purpose:
 *      Allocate a multicast group index
 * Parameters:
 *      unit       - (IN)   Device Number
 *      flags      - (IN)   BCM_MULTICAST_*
 *      group      - (OUT)  Group ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_create(
    int unit, 
    uint32 flags, 
    bcm_multicast_t *group)
{
    bcm_error_t rv;    
    int type;
    SOC_TMC_MULT_ID multicast_id;
    BCM_DPP_MULTICAST_TYPE tm_mc_type = BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS;
    uint8 expected_open;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    rv = BCM_E_NONE;    

    /* verify input parameters */
    BCMDNX_NULL_CHECK(group);
    
    if (flags & _BCM_PETRA_MULTICAST_TYPE_UNSUPPORTED_FLAGS)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags")));
    }
    
    type = flags & BCM_MULTICAST_TYPE_MASK;

    if (!(flags & BCM_MULTICAST_WITH_ID))
    {
        /* Multicast without ID is not supported */
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("flag BCM_MULTICAST_WITH_ID must be used")));
    } else if ((flags & (BCM_MULTICAST_TYPE_TRILL | BCM_MULTICAST_INGRESS_GROUP)) == (BCM_MULTICAST_TYPE_TRILL | BCM_MULTICAST_INGRESS_GROUP)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Trill multicast supported only for egress")));
    }

    /* Type is ECMP/flooding port => create virtual MC_ID*/
    if (flags & BCM_MULTICAST_TYPE_SUBPORT) {
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_SUBPORT, *group);
        BCM_EXIT;
    }

    rv = _bcm_petra_multicast_group_to_id(*group, &multicast_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (flags & BCM_MULTICAST_TYPE_PORTS_GROUP) { /* handle the case where we are not creating a group, but just returning a representation of a bitmap */
        if ((flags & ~(BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID)) != BCM_MULTICAST_TYPE_PORTS_GROUP) {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("flags containing BCM_MULTICAST_TYPE_PORTS_GROUP should contain BCM_MULTICAST_WITH_ID and nothing else")));
        }

        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_PORTS_GROUP, multicast_id);
        BCM_EXIT;
    }

    /* Multicast ID is global, Indicate Flag decide which groups to create (by default ingress and egress) */
    if (flags & BCM_MULTICAST_INGRESS_GROUP) {
        if (~flags & BCM_MULTICAST_EGRESS_GROUP) {
            tm_mc_type = BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY;
        }
    } else if (flags & BCM_MULTICAST_EGRESS_GROUP) {
        tm_mc_type = BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY;
    }
    expected_open = SOC_SAND_FALSE;
    rv = _bcm_petra_multicast_is_group_exist(unit, tm_mc_type, multicast_id, expected_open);
    BCMDNX_IF_ERR_EXIT(rv);
    
    switch(type) {
    case BCM_MULTICAST_TYPE_L2:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L2, multicast_id);      
        break;

    case BCM_MULTICAST_TYPE_L3:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_L3, multicast_id);
        break;

    case BCM_MULTICAST_TYPE_VPLS:   
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_VPLS, multicast_id);
        break; 

    case BCM_MULTICAST_TYPE_TRILL:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_TRILL, multicast_id);  
        break;

    case BCM_MULTICAST_TYPE_MIM:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_MIM, multicast_id);      
        break;

    case BCM_MULTICAST_TYPE_EGRESS_OBJECT:
        _BCM_MULTICAST_GROUP_SET(*group, _BCM_MULTICAST_TYPE_EGRESS_OBJECT, multicast_id);      
        break;
                  
    case 0:
        /* TM type only */
        break;
    default:
        /* Configuration type is invalid */     
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified multicast type flags are not supported")));
    }

    /*
     * buffer type should be determined according to BCM_MULTICAST_INGRESS_MMC_BUFFERS flags
     */
    if (SOC_IS_JERICHO_PLUS_AND_BELOW(unit) && (flags & BCM_MULTICAST_INGRESS_GROUP))
    {
        if (flags & BCM_MULTICAST_INGRESS_MMC_BUFFERS)
        {
            BCMDNX_IF_ERR_EXIT(dpp_mult_properties_set_unsafe(unit, multicast_id, irdb_value_group_open_mmc));
        }
        else
        {
            BCMDNX_IF_ERR_EXIT(dpp_mult_properties_set_unsafe(unit, multicast_id, irdb_value_group_open));
        }
    }

    rv = _bcm_petra_multicast_group_open(unit,tm_mc_type, multicast_id);
    BCMDNX_IF_ERR_EXIT(rv);


    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_destroy
 * Purpose:
 *      Free a multicast group index
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Group ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_destroy(
    int unit, 
    bcm_multicast_t group)
{
    int rv;
    SOC_TMC_MULT_ID multicast_id;
    int type;
    BCM_DPP_MULTICAST_TYPE tm_mc_type;
    uint8 ingress_open = FALSE;
    uint8 egress_open = FALSE;
    uint8 egress_only = FALSE;
    uint8 ingress_only = FALSE;
       
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    type = _BCM_MULTICAST_TYPE_GET(group);
    if (type != _BCM_MULTICAST_TYPE_L2 && type !=  _BCM_MULTICAST_TYPE_VPLS
        && type != _BCM_MULTICAST_TYPE_L3 && type != 0) {    
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid multicast group type")));
    }

    rv = _bcm_petra_multicast_group_to_id(group, &multicast_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (multicast_id >= SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids) {
        egress_only = TRUE;
    }
    if (multicast_id >= SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
        ingress_only = TRUE;
    }
    if (ingress_only && egress_only) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid multicast ID")));
    }
    /* Multicast ID is global, both ingress and egress multicast groups need to be destroyed if they exist.
       Check what exists. */
        if (!egress_only) {
            rv = _bcm_petra_multicast_ingress_is_group_exist(unit, multicast_id, &ingress_open); 
            BCMDNX_IF_ERR_EXIT(rv);
        }

    if (!ingress_only) {
        rv = _bcm_petra_multicast_egress_is_group_exist(unit, multicast_id, &egress_open); 
        BCMDNX_IF_ERR_EXIT(rv);
    }

    if (egress_open == FALSE) {
        if (ingress_open == FALSE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The specified multicast group was not created")));
        } else {
            tm_mc_type = BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY;
        }
    } else {
        if (ingress_open == FALSE) {
            tm_mc_type = BCM_DPP_MULTICAST_TYPE_EGRESS_ONLY;
        } else {
            tm_mc_type = BCM_DPP_MULTICAST_TYPE_INGRESS_AND_EGRESS;
        }
    }
   
    rv = _bcm_petra_multicast_group_close(unit, tm_mc_type, multicast_id);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Store a replication, while checking we are not out of replications, advance to next replication in the array.
 * Handle field encodings for Arad+ large CUD modes.
 */
STATIC int _bcm_arad_encode_and_store_mc_replication(
    int unit,                   /* input: unit/device */
    uint32 local_port,          /* input: replication destination port */
    uint32 cud,                 /* input: replication CUD */
    uint32 cud2,                /* input: replication 2nd CUD */
    uint32 max_reps,            /* input: max replication number */
    int is_17b_cud_mode,        /* input: is Arad+ 17bit CUD mode */
    uint32 *nof_reps,           /* inout: number of replications in replication array */
    dpp_mc_replication_t **rep) /* inout: pointer in replication array */
{
    BCMDNX_INIT_FUNC_DEFS;

    if ((*nof_reps)++ >= max_reps) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Too many replications in input")));
    }

    /* encode the destination port, and change encodings for Arad+ large CUD modes */
    if (local_port == BCM_GPORT_INVALID) {
        if (is_17b_cud_mode) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("CUD only replications are not supported in egress multicast with 17 bit CUDs.")));
        }
    } else if (local_port & ARAD_MC_EGR_IS_BITMAP_BIT) { /* This is a bitmap replication */
        uint32 bitmap = local_port & ~ARAD_MC_EGR_IS_BITMAP_BIT;
        if (bitmap == 0 || bitmap > ARAD_MC_MAX_BITMAPS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid bitmap: %d, should be between 1 and %d."), bitmap, ARAD_MC_MAX_BITMAPS));
        } else if (is_17b_cud_mode) {
            local_port |= (cud & (1 << 16)) >> 2;
        cud &= ~(1 << 16);
        }
    } else if (is_17b_cud_mode) {
        local_port |= (cud & (1 << 16)) >> 9;
        cud &= ~(1 << 16);
    }

    /* store the encoded replication and advance the replications pointer */
    (*rep)->dest = local_port;
    (*rep)->cud = cud;
    (*rep)->additional_cud = cud2;
    ++*rep;
exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * convert a given array/s of gport based replications to replications with local logical ports
 * Handles non Petra devices.
 */
STATIC int _bcm_arad_convert_egress_mc_reps_to_ports(
    int unit,                               /* input: unit/device */
    int rep_count,                          /* input: number of entries/replications in the following arrays */
    bcm_gport_t *port_array,                /* input: port/destination array, used if rep_array==NULL */
    bcm_if_t *encap_id_array,               /* input: encap/CUD array, used if rep_array==NULL */
    bcm_multicast_replication_t *rep_array, /* input: replication array */
    uint8 support_outlif_only_reps,         /* input: are outlif only replications supported */
    uint8 support_bitmap_reps,              /* input: are bitmap replications supported */
    uint32 *out_nof_reps,                   /* output: number of replications in output */
    dpp_mc_replication_t **out_reps)        /* output: pointer to output replications array allocated by this function */
{
    int use_reps = rep_array != NULL;
    int is_17b_cud_mode = SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE;
    uint32 local_port, max_reps = DPP_MULT_MAX_EGRESS_REPLICATIONS, i, nof_reps = 0;
    uint32 cud, cud2, cud2_type_bit;
    uint32 max_local_port = SOC_IS_JERICHO(unit) ? SOC_TMC_NOF_FAP_PORTS_JERICHO - 1 /*511*/ :
      (is_17b_cud_mode ? ARAD_MULT_EGRESS_SMALL_PORT_INVALID - 1  /*126*/ : /*254*/ DPP_MULT_EGRESS_PORT_INVALID - 1);
    bcm_gport_t dest;
    dpp_mc_replication_t *rep;
    bcm_multicast_replication_t *r;
    _bcm_dpp_gport_info_t gport_info;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    /* verify input parameters */
    if (!out_nof_reps || !out_reps || (rep_count > 0 && !use_reps && (!port_array || !encap_id_array))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("null pointer" )));
    }


    /* allocate a replication array to hold the replications of the linked list group whose destinations are converted to local ports */
    local_port = rep_count * BCM_TRUNK_MAX_PORTCNT; /* calculate max possible number of replications */
    if (local_port < DPP_MULT_MAX_EGRESS_REPLICATIONS) {
        max_reps = local_port;
    }
    BCMDNX_ALLOC(*out_reps, sizeof(dpp_mc_replication_t) * max_reps, "local ports from gports");

    /* Fill out_reps with the replications converted to local ports, LAG destinations may result in multiple local ports/replications */
    /* Each replication has one or two CUDs. */
    rep = *out_reps;
    for (i = 0; i < rep_count ; i++) {
        if (use_reps) { /* get the destination and CUD/s from the correct input source */
            r = rep_array + i;
            dest = r->port;
            if (r->flags & BCM_MUTICAST_REPLICATION_ENCAP2_VALID) {
                if (r->flags & BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF) {
                    cud = r->encap2;
                    cud2 = r->encap1;
                    cud2_type_bit = 0;
                } else {
                    cud = r->encap1;
                    cud2 = r->encap2;
                    cud2_type_bit = SOC_IS_QAX(unit) ? 0 : DPP_MC_2ND_CUD_IS_EEI;
                }
            } else {
                cud = r->encap1;
                cud2 = DPP_MC_NO_2ND_CUD;
                cud2_type_bit = 0;
            }
        } else {
            dest = port_array[i];
            cud = encap_id_array[i];
            cud2 = DPP_MC_NO_2ND_CUD;
            cud2_type_bit = 0;
        }

        /* test and update CUDs */
        if (cud == BCM_IF_INVALID) {
            cud = SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud;
        } else if (cud > SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid egress encapsulation ID: 0x%x, should be between 0 and 0x%x."), (unsigned)cud, SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud));
        }
        if (cud2 == BCM_IF_INVALID) {
            cud2 = SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud;
        } else if (cud2 > SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid egress additional encapsulation ID: 0x%x, should be between 0 and 0x%x."), (unsigned)cud2, SOC_DPP_CONFIG(unit)->tm.egress_mc_max_cud));
        }
        cud2 |= cud2_type_bit; /* Mark 2nd CUD type as EEI if needed */

        if (support_outlif_only_reps && dest == BCM_GPORT_INVALID) { /* outlif only replication (port mapped from outlif), discontinued in QAX */
            if (!cud) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outlif only replication can not use outlif 0")));
            }
            BCMDNX_IF_ERR_EXIT(_bcm_arad_encode_and_store_mc_replication(unit, BCM_GPORT_INVALID, cud, cud2,
              max_reps, is_17b_cud_mode, &nof_reps, &rep));
        } else if (support_bitmap_reps && BCM_GPORT_IS_MCAST(dest)) { /* bitmap replication */
            BCMDNX_IF_ERR_EXIT(_bcm_arad_encode_and_store_mc_replication(unit, BCM_GPORT_MCAST_GET(dest) | ARAD_MC_EGR_IS_BITMAP_BIT, cud, cud2,
              max_reps, is_17b_cud_mode, &nof_reps, &rep));
        } else { /* port+CUD replication */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, dest, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY | _BCM_DPP_GPORT_TO_PHY_OP_IGNORE_DISABLED_PORTS, &gport_info)); 
            BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {
                if (local_port > max_local_port) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid local port: %u, should be between 0 and %u."), (unsigned)local_port, (unsigned)max_local_port));
                }
                BCMDNX_IF_ERR_EXIT(_bcm_arad_encode_and_store_mc_replication(unit, local_port, cud, cud2,
                  max_reps, is_17b_cud_mode, &nof_reps, &rep));
            }
        }
    }

    *out_nof_reps = nof_reps;
exit:
    if (_rv != BCM_E_NONE && out_reps) {
        BCM_FREE(*out_reps);
    }
    BCMDNX_FUNC_RETURN;
}

/*
 * Add or Delete an egress multicast replication. Called from both bcm_petra_multicast_egress_add/delete and bcm_petra_multicast_add/delete.
 * Handles non Petra devices.
 */
int _bcm_arad_multicast_egress_add_or_delete(
    int unit,                               /* input: unit/device */
    bcm_multicast_t group,                  /* input: egress MC group to set */
    int rep_count,                          /* input: number of entries/replications in the following arrays */
    bcm_multicast_replication_t *rep_array, /* input: replication array */
    uint8 is_delete)                        /* input: if non zero this is a delete operation, otherwise an add */
{
    SOC_TMC_MULT_ID mcid;
    SOC_TMC_MULT_EG_ENTRY entry;
    int i, is_bitmap_group;
    uint32 nof_reps = 0; /* number of reps to send to soc function */
    uint8 is_in_range = 0, open, support_outlif_only_reps = TRUE, support_bitmap_reps = TRUE;
    dpp_mc_replication_t *rep_arr = NULL;
    int rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      
    SOC_TMC_MULT_EG_ENTRY_clear(&entry);
    BCMDNX_NULL_CHECK(rep_array);

    if (rep_count != 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("nof_replications != 1 not currently supported")));
    }
    BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group)); /* Check if mc group type is supported */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &mcid));
    BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, mcid, TRUE, &open)); /* Check if multicast group exists (was created) */
    if (is_bitmap_group) {
        is_in_range = 1; /* Mark as bitmap/vlan group since the caller specified this is a bitmap group */
    } else {
        if (!open) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress multicast group %u was not created"), mcid));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, mcid, &is_in_range)); /* check if this is a vlan group */
    }

    if (is_in_range) { /* Handle bitmap groups. They do not have CUDs */
        _bcm_dpp_gport_info_t gport_info;
        bcm_pbmp_t pbmp;
        BCM_PBMP_CLEAR(pbmp);
        for (i = 0; i < rep_count ; i++) { /* construct the local ports bitmap of the MC group */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, rep_array[i].port, _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
            BCM_PBMP_OR(pbmp, gport_info.pbmp_local_ports);
        }

        rv = _bcm_petra_multicast_egress_vlan_membership_port_add_or_remove(unit, mcid, &pbmp, is_delete);
        if (rv == BCM_E_NOT_FOUND) {
            LOG_VERBOSE(BSL_LS_BCM_L2, 
                    (BSL_META_U(unit,
                                "_bcm_arad_multicast_egress_add_or_delete address not found\n")));
        }
        BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(rv);

    } else { /* Handle linked list groups. Each replication has one or two CUDs. */

        /* Fill rep_arr with the replications converted to local ports, LAG destinations may result in multiple local ports/replications */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_convert_egress_mc_reps_to_ports(unit, rep_count, 0, 0, rep_array,
          support_outlif_only_reps, support_bitmap_reps, &nof_reps, &rep_arr));
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_entry_port_add_remove(unit, mcid, nof_reps, rep_arr, is_delete)); /* configure the MC group in HW */
    }

exit:
    BCM_FREE(rep_arr);
    BCMDNX_FUNC_RETURN;
}

/*
 *  add/remove a replication to/from an egress multicast group
 */
STATIC int bcm_petra_multicast_egress_add_remove(
    int unit,               /* input: unit/device */
    bcm_multicast_t group,  /* input: egress MC group to work on */
    bcm_gport_t port,       /* input: replication port */
    bcm_if_t encap_id,      /* input: replication CUD */
    uint8 is_delete)        /* input: if non zero this is a delete operation, otherwise an add */
{
    bcm_multicast_replication_t rep = {0};
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      

    rep.port = port;
    rep.encap1 = encap_id;
    BCMDNX_IF_ERR_NOT_E_NOT_FOUND_EXIT(_bcm_arad_multicast_egress_add_or_delete(unit, group, 1, &rep, is_delete));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Delete replication from the given egress multicast group.
 */
int bcm_petra_multicast_egress_add(
    int unit,               /* input: unit/device */
    bcm_multicast_t group,  /* input: egress MC group to work on */
    bcm_gport_t port,       /* input: replication port */
    bcm_if_t encap_id)      /* input: replication CUD */
{
    return bcm_petra_multicast_egress_add_remove(unit, group, port, encap_id, FALSE);
}

/*
 *  Add replication to the given egress multicast group.
 */
int bcm_petra_multicast_egress_delete(
    int unit,               /* input: unit/device */
    bcm_multicast_t group,  /* input: egress MC group to work on */
    bcm_gport_t port,       /* input: replication port */
    bcm_if_t encap_id)      /* input: replication CUD */
{
    return bcm_petra_multicast_egress_add_remove(unit, group, port, encap_id, TRUE);
}


/*
 * Function:
 *      bcm_petra_multicast_egress_delete_all
 * Purpose:
 *      Delete all GPORTs from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_egress_delete_all(
    int unit, 
    bcm_multicast_t group)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    BCMDNX_IF_ERR_EXIT(bcm_petra_multicast_egress_set(unit,group,0,NULL,NULL));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Get the replications of an egress multicast group.
 * The none Petra implementation called from bcm_petra_multicast_egress_get and from bcm_petra_multicast_get.
 * If the input parameter rep_max is 0 return only the number of replications in the group in rep_count.
 * Otherwise the number of replications actually returned (up to rep_max) is returned in rep_count.
 */
int _bcm_arad_multicast_egress_get(
    int unit,                               /* input: unit/device */
    bcm_multicast_t group,                  /* input: egress MC group to get */
    int rep_max,                            /* input: max number of entries/replications in the following arrays */
    bcm_gport_t *port_array,                /* output: port/destination array, used if rep_array==NULL */
    bcm_if_t *encap_id_array,               /* output: encap/CUD array, used if rep_array==NULL */
    bcm_multicast_replication_t *rep_array, /* output: replication array */
    int *rep_count)                         /* output: number of entries/replications in the group or rep_max */
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    uint32 group_size = 0;
    uint8 is_in_range, is_open;
    int nof_reps, is_bitmap_group;

    BCMDNX_INIT_FUNC_DEFS;
    /* verify input parameters */
    if (rep_max > 0) {
        if (rep_array == NULL) {
            BCMDNX_NULL_CHECK(port_array);
            BCMDNX_NULL_CHECK(encap_id_array);
        }
    } else {
        rep_max = 0;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id_ndx)); /* get mcid of group */
    BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group)); /* Check if mc group type is supported */
    /* Check multicast group exists (was created) */
    BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, TRUE, &is_open));
    if (is_bitmap_group) {
        is_in_range = 1;
    } else {
        if (!is_open) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress multicast group %u was not created"), multicast_id_ndx));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, multicast_id_ndx, &is_in_range)); /* check if this is a vlan group */
    }

    if (is_in_range) { /* this is a bitmap */
        bcm_pbmp_t l2_pbmp;
        bcm_port_t port;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_get(unit, multicast_id_ndx, &l2_pbmp)); /* do the actual work */

        /* Convert pbmp to gport */
        PBMP_ITER(l2_pbmp, port) {
            if (rep_max == 0) {
                break;
            }
            if (rep_array != NULL) {
                bcm_multicast_replication_t_init(rep_array);
                BCM_GPORT_LOCAL_SET(rep_array->port, port);
                
                ++rep_array;
            } else {
                /* Convert local port to GPORT */
                BCM_GPORT_LOCAL_SET(port_array[group_size],port);
                encap_id_array[group_size] = BCM_IF_INVALID;
            }
            ++group_size;
            --rep_max;
        }
        nof_reps = group_size;

        if (rep_max == 0) {
            BCM_PBMP_COUNT(l2_pbmp, nof_reps);
        }
        
    } else { /* group is not a bitmap */

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_eg_get_group,(unit, multicast_id_ndx, rep_max, port_array, encap_id_array, (soc_multicast_replication_t*)rep_array, &group_size, &is_open))); /* do the actual work */
        if (!is_open) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not clear if egress multicast group %u was created"), multicast_id_ndx));
        }
    
        /* if rep_max is non zero and not all group replications could be returned,
         * return the number of returned replications (rep_max).
         * Otherwise return the exact group size in replications (a bitmap pointer counts as one replication).
         */
        nof_reps = rep_max && group_size > rep_max ? rep_max : group_size;
    }
    if (rep_count != NULL) {
        *rep_count = nof_reps;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/* 
 * Function:
 *      bcm_petra_multicast_egress_get
 * Purpose:
 *      Retrieve a set of egress multicast GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      mc_index   - (IN) Multicast index
 *      port_max   - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of multicast GPORTs 
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of multicasts returned
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter port_max = 0, return in the output parameter
 *      port_count the total number of ports/encapsulation IDs in the 
 *      specified multicast group's replication list.
 */
int 
bcm_petra_multicast_egress_get(
    int unit,
    bcm_multicast_t group,
    int port_max,
    bcm_gport_t *port_array,
    bcm_if_t *encap_id_array,
    int *port_count)
{
    int is_bitmap_group;

    BCMDNX_INIT_FUNC_DEFS;
    /* verify input parameters */
    BCMDNX_NULL_CHECK(port_count);
    if (port_max > 0) {
        BCMDNX_NULL_CHECK(port_array);
        BCMDNX_NULL_CHECK(encap_id_array);
    }   else if (port_max < 0) {
        port_max = 0;
    }
    BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group)); /* Check if mc group type is supported */

    BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_egress_get(unit, group, port_max, port_array, encap_id_array, NULL, port_count));

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Set an egress multicast group. Called from both bcm_petra_multicast_egress_set and bcm_petra_multicast_set.
 * Handles non Petra devices.
 */
int _bcm_arad_multicast_set_egress(
    int unit,                               /* input: unit/device */
    bcm_multicast_t group,                  /* input: egress MC group to set */
    int rep_count,                          /* input: number of entries/replications in the following arrays */
    bcm_gport_t *port_array,                /* input: port/destination array, used if rep_array==NULL */
    bcm_if_t *encap_id_array,               /* input: encap/CUD array, used if rep_array==NULL */
    bcm_multicast_replication_t *rep_array) /* input: replication array */
{
    SOC_TMC_MULT_ID mcid;
    int ndx;
    bcm_gport_t gport;
    uint32 nof_reps = 0; /* number of reps to send to soc function */
    int is_bitmap_group, use_reps = rep_array != NULL;
    SOC_TMC_ERROR mc_err = BCM_E_NONE;
    uint8 open = FALSE, is_in_range = 0, support_outlif_only_reps = SOC_IS_QAX(unit) ? FALSE : TRUE, support_bitmap_reps = TRUE;
    dpp_mc_replication_t *rep_arr = NULL;
    bcm_pbmp_t pbmp;
    _bcm_dpp_gport_info_t gport_info;
    
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    /* verify input parameters */
    if (rep_count > 0 && !use_reps && (!port_array || !encap_id_array)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("null replications pointer" )));
    }

    {
        BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group)); /* Check if mc group type is supported */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &mcid));
        /* Check if multicast group exists (was created) */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, mcid, TRUE, &open));
        if (is_bitmap_group) {
            is_in_range = 1; /* Mark as bitmap/vlan group since the caller specified this is a bitmap group */
        } else {
            if (!open) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress multicast group %u was not created"), mcid));
            }
            BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_range_id_check(unit, mcid, &is_in_range)); /* check if this is a vlan group */
        }
    }

    if (is_in_range) { /* Handle bitmap groups. They do not have CUDs */
        BCM_PBMP_CLEAR(pbmp);
        for (ndx = 0; ndx < rep_count ; ndx++) { /* construct the local ports bitmap of the MC group */
            gport = (!port_array) ? rep_array[ndx].port : port_array[ndx];
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, gport , _BCM_DPP_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info)); 
            BCM_PBMP_OR(pbmp, gport_info.pbmp_local_ports);
        }
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_vlan_membership_set(unit, mcid, pbmp)); /* configure the MC group in HW */

    } else { /* Handle linked list groups. Each replication has one or two CUDs. */

        /* Fill rep_arr with the replications converted to local ports, LAG destinations may result in multiple local ports/replications */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_convert_egress_mc_reps_to_ports(unit, rep_count, port_array, encap_id_array, rep_array,
          support_outlif_only_reps, support_bitmap_reps, &nof_reps, &rep_arr));

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_mult_eg_group_set, /* configure the MC group in HW */
            (unit, mcid, 0, nof_reps, rep_arr, &mc_err)));
        if (mc_err) {
            BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ? "not enough multicast entries for the operation" :
              (mc_err == (SOC_TMC_ERROR) BCM_E_PARAM ? "The same group member (port + encap) may not appear multiple times in the input" : "unexpected error")) ));
        }
    }

exit:
    BCM_FREE(rep_arr);
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_petra_multicast_egress_set
 * Purpose:
 *      Assign the complete set of egress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count   - (IN) Number of ports in replication list
 *      port_array   - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_egress_set(
    int unit, 
    bcm_multicast_t group, 
    int port_count, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    /* verify input parameters */
    if (port_count > 0) {
        BCMDNX_NULL_CHECK(port_array);
        BCMDNX_NULL_CHECK(encap_id_array);
    }

    BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_set_egress(unit, group, port_count, port_array, encap_id_array, NULL));

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_fabric_distribution_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_fabric_distribution_t *ds_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_multicast_fabric_distribution_get is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_fabric_distribution_set(
    int unit, 
    bcm_multicast_t group, 
    bcm_fabric_distribution_t ds_id)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_multicast_fabric_distribution_set is not available")));
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_group_get(
    int unit, 
    bcm_multicast_t group, 
    uint32 *flags)
{
    int rv;    
    SOC_TMC_MULT_ID multicast_id_ndx;
    int type, in_ingress_range = TRUE, in_egress_range = TRUE;
    uint8 is_open = FALSE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    type = _BCM_MULTICAST_TYPE_GET(group);
    if (_BCM_MULTICAST_IS_L2(group)) {
        *flags = BCM_MULTICAST_TYPE_L2;
    } else if (_BCM_MULTICAST_IS_VPLS(group)) {
        *flags = BCM_MULTICAST_TYPE_VPLS;
    } else if (_BCM_MULTICAST_IS_L3(group)) {
        *flags = BCM_MULTICAST_TYPE_L3;
    } else if (_BCM_MULTICAST_IS_PORTS_GROUP(group)) {
        *flags = BCM_MULTICAST_TYPE_PORTS_GROUP;
    } else if (type == 0) {
        *flags = 0;
    } else {
        /* This is not supported type, nor type 0 (TM) */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Unsupported group type")));
    }
    *flags |= BCM_MULTICAST_WITH_ID; 

    rv = _bcm_petra_multicast_group_to_id(group, &multicast_id_ndx);
    BCMDNX_IF_ERR_EXIT(rv);

    in_ingress_range =
      (multicast_id_ndx < SOC_DPP_CONFIG(unit)->tm.nof_ingr_mc_ids);
    in_egress_range = (multicast_id_ndx < SOC_DPP_CONFIG(unit)->tm.nof_mc_ids);
    if (!(in_egress_range | in_ingress_range)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("multicast ID 0x%x is out of range"), multicast_id_ndx));
    }

    if (in_ingress_range) {
        rv = _bcm_petra_multicast_ingress_is_group_exist(unit,multicast_id_ndx,&is_open);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_open != FALSE) {
            *flags |= BCM_MULTICAST_INGRESS_GROUP;
        }
    }
    if (in_egress_range) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_egress_is_group_exist(unit,multicast_id_ndx,&is_open));
        if (is_open != FALSE) {
            *flags |= BCM_MULTICAST_EGRESS_GROUP;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_init(
    int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_multicast_t multicast_id, temp_id = 0;
    uint32 flags = 0;
    
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);                      
    if (!SOC_DPP_CONFIG(unit)->tm.nof_mc_ids) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NONE, (_BSL_BCM_MSG("Multicast is not being initialized")));
    }
    if (SOC_DPP_PP_ENABLE(unit)) {

        if (!SOC_WARM_BOOT(unit)) {  /* if coldboot */
            /* Multicast egress groups 0-4K bitmap types are opened by default since they are reserved for vlan(VSI) 0-4K */
            if (SAL_BOOT_QUICKTURN) {         
              for (multicast_id = 0; multicast_id < BCM_PETRA_MULTICAST_NOF_BRIDGE_ROUTE; ++multicast_id) {     
                  /* In case of link list needs to call to regular open sequence */
                  temp_id = multicast_id;
                  flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP;
                                 
                  rv = bcm_petra_multicast_create(unit, flags, &temp_id);
                  BCMDNX_IF_ERR_EXIT(rv);
	      }        
            } else {
                for (multicast_id = 0; multicast_id < BCM_PETRA_MULTICAST_NOF_BRIDGE_ROUTE; ++multicast_id) {
                    temp_id = multicast_id;
                    flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP;
                    rv = bcm_petra_multicast_create(unit,flags,&temp_id);
                      BCMDNX_IF_ERR_EXIT(rv);
                }
            }      
        }              
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_multicast_detach(
    int unit)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_l2_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_vlan_t vlan, 
    bcm_if_t *encap_id)
{
    bcm_error_t rv;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    _bcm_dpp_gport_resolve_type_e encap_type;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    rv = _bcm_dpp_gport_resolve(unit,port, 0 ,&dest_id,encap_id,&encap_type);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_l3_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t intf, 
    bcm_if_t *encap_id)
{
    SOC_PPC_VSI_ID vsi;
    bcm_vlan_t vid;


    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    unit = (unit);

    /* encap id is simply intf id (RIF) */ 
    vsi = intf;
    vid = vsi;

    *encap_id = SOC_PPD_CUD_VSI_GET(unit,vid);
 
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_multicast_egress_object_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_if_t intf, 
    bcm_if_t *encap_id)
{
    SOC_PPC_FEC_ID fec_index;
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE
      protect_type;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
      working_fec;
    SOC_PPC_FRWRD_FEC_ENTRY_INFO
      protect_fec;
    SOC_PPC_FRWRD_FEC_PROTECT_INFO
      protect_info;
    bcm_if_t tmp = 0;
    int rv;

    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(encap_id);

    unit = (unit);

    /* get encap id from FEC entry */ 

    /* intf egress object can be FEC or Tunnel or RIF. For RIF use bcm_petra_multicast_l3_encap_get */
    if (BCM_L3_ITF_TYPE_IS_LIF(intf)) {
        tmp = BCM_L3_ITF_VAL_GET(intf);
        
        rv = _bcm_l3_encap_id_to_eep(unit,tmp,encap_id);
        BCM_IF_ERROR_RETURN(rv);
    } else {
        /* FEC */
        rv = _bcm_l3_intf_to_fec(unit,intf,&fec_index);
        BCM_IF_ERROR_RETURN(rv);

        /* read FEC */
        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(
            unit,
            fec_index,
            &protect_type,
            &working_fec,
            &protect_fec,
            &protect_info
          );
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *encap_id = working_fec.eep;
    }
    

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_multicast_mim_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t mim_port_id, 
    bcm_if_t *encap_id)
{
    int rv;
    _bcm_dpp_gport_resolve_type_e gport_type;
    SOC_SAND_PP_DESTINATION_ID  dest_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    /* Resolve fec and encap_id (EEP/I-SID) from mim port id */
    rv = _bcm_dpp_gport_resolve(unit, mim_port_id, _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK, &dest_id, (int*)encap_id, &gport_type);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* The encap for trill : port - fec eep */
int 
bcm_petra_multicast_trill_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t intf, 
    bcm_if_t *encap_id)
{
    bcm_error_t rv;
    SOC_SAND_PP_DESTINATION_ID  dest_id;
    _bcm_dpp_gport_resolve_type_e encap_type;
    SOC_PPC_FEC_ID              fec_index;
    bcm_trill_port_t        trill_port;
    
    unsigned int soc_sand_dev_id;
   
    SOC_PPC_FRWRD_FEC_ENTRY_INFO    fec_entry[2];
    SOC_PPC_FRWRD_FEC_PROTECT_INFO  protect_info;
    SOC_PPC_FRWRD_FEC_PROTECT_TYPE  protect_type;
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    soc_sand_dev_id = (unit);

    BCMDNX_NULL_CHECK(encap_id);
   
   
    if (BCM_GPORT_IS_TRILL_PORT(port)) {
        bcm_trill_port_t_init(&trill_port);
        trill_port.trill_port_id = port;

        rv = bcm_petra_trill_port_get(unit,&trill_port);
        BCMDNX_IF_ERR_EXIT(rv);

        if (trill_port.flags & BCM_TRILL_PORT_EGRESS) {
            /* get the "encap id" part of the gport */
            *encap_id = BCM_GPORT_TRILL_PORT_ID_GET(port);
        } else {
            rv = _bcm_l3_intf_to_fec(unit, trill_port.egress_if, &fec_index );
            BCMDNX_IF_ERR_EXIT(rv);

            /* get encap ID from FEC - */
            soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,fec_index,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

            *encap_id = fec_entry[0].eep;
        }
    } else {
        rv = _bcm_l3_intf_to_fec(unit, intf, &fec_index );
        BCMDNX_IF_ERR_EXIT(rv);

        rv = _bcm_dpp_gport_resolve(unit,port,0, &dest_id,encap_id,&encap_type);
        BCMDNX_IF_ERR_EXIT(rv);

        if(dest_id.dest_type != SOC_SAND_PP_DEST_SINGLE_PORT) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("TODO err message")));
        }

        /* get encap ID from FEC - */
        soc_sand_rv = soc_ppd_frwrd_fec_entry_get(soc_sand_dev_id,fec_index,&protect_type,&fec_entry[0],&fec_entry[1],&protect_info);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        *encap_id = (dest_id.dest_val << _BCM_TRILL_ENCAP_PORT_SHIFT)  | fec_entry[0].eep;
    }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_vlan_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t vlan_port_id, 
    bcm_if_t *encap_id)
{
    bcm_error_t rv;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    _bcm_dpp_gport_resolve_type_e encap_type;
    int encap_val;


    BCMDNX_INIT_FUNC_DEFS;
    unit = (unit);

    BCMDNX_NULL_CHECK(encap_id);
    if(!BCM_GPORT_IS_VLAN_PORT(vlan_port_id)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("given gport is not vlan port")));
    }
    rv = _bcm_dpp_gport_resolve(unit,vlan_port_id,0, &dest_id,encap_id,&encap_type);
    BCMDNX_IF_ERR_EXIT(rv);

    if(encap_type == _bcmDppGportResolveTypeAC || _BCM_DPP_GPORT_RESOLVE_TYPE_IS_FEC(encap_type)) {
        encap_val = *encap_id;

        *encap_id = SOC_PPD_CUD_AC_GET(unit,encap_val);

    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_multicast_vpls_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t mpls_port_id, 
    bcm_if_t *encap_id)
{
    bcm_error_t rv;
    SOC_SAND_PP_DESTINATION_ID dest_id;
    _bcm_dpp_gport_resolve_type_e encap_type;
    _bcm_dpp_gport_hw_resources gport_hw_resources;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    unit = (unit);

    /* Resolve encap id by mpls port */
    rv = _bcm_dpp_gport_resolve(unit,mpls_port_id,0, &dest_id,encap_id,&encap_type);
    BCMDNX_IF_ERR_EXIT(rv);

    /* if this is FEC+VC, then get EEP from SW-state */
    if(encap_type == _bcmDppGportResolveTypeFecVC) {
        rv = _bcm_dpp_gport_to_hw_resources(unit, mpls_port_id, _BCM_DPP_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS | _BCM_DPP_GPORT_HW_RESOURCES_STRICT_CHECK,
                                            &gport_hw_resources);
        BCMDNX_IF_ERR_EXIT(rv);
        *encap_id = SOC_PPD_CUD_EEP_GET(unit,gport_hw_resources.global_out_lif);
    }
    else if (encap_type == _bcmDppGportResolveTypePhyEEP || _BCM_DPP_GPORT_RESOLVE_TYPE_IS_FEC(encap_type)) {
        /* Translate AC to CUD */
        *encap_id = SOC_PPD_CUD_AC_GET(unit,*encap_id);
    } else {
        /* Trnslate EEP to CUD */
        *encap_id = SOC_PPD_CUD_EEP_GET(unit,*encap_id);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_multicast_l2gre_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t l2gre_port_id, 
    bcm_if_t *encap_id)
{
    int rv;
    _bcm_dpp_gport_resolve_type_e gport_type;
    SOC_SAND_PP_DESTINATION_ID  dest_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    /* Resolve fec and encap_id (EEP/I-SID) from mim port id */
    rv = _bcm_dpp_gport_resolve(unit, l2gre_port_id, _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK, &dest_id, (int*)encap_id, &gport_type);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


int 
_bcm_petra_multicast_vxlan_encap_get_verify(
    int unit, 
    bcm_gport_t vxlan_port_id) {

    int rv; 
    _BCM_GPORT_PHY_PORT_INFO phy_port;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_sw_db_hash_vlan_find(unit,
                        (sw_state_htb_key_t) &(vxlan_port_id), 
                        (sw_state_htb_data_t) &phy_port,FALSE); 
    /* check vxlan port has an entry in hash table */
    if (GPORT_HASH_VLAN_NOT_FOUND(phy_port,rv)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("vxlan port doesn't exist")));
    } 
    /* check if vxlan port has an ECMP entry */
    else {
        /* check if it's a FEC entry and it's in ECMP range */
        if (phy_port.type == _bcmDppGportResolveTypeVxlanEgFec) {
            if (phy_port.phy_gport < SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG(
                   "Can't get encap from vxlan port, egress_if (%d) from vxlan port is an ECMP (in range 0 to %d). Call bcm_multicast_egress_object_encap_get on tunnel_id from tunnel initiator port. \n"),phy_port.phy_gport, SOC_DPP_CONFIG(unit)->l3.fec_ecmp_reserved));
            }
        }
    }
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_multicast_vxlan_encap_get(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_gport_t vxlan_port_id, 
    bcm_if_t *encap_id)
{
    int rv;
    _bcm_dpp_gport_resolve_type_e gport_type;
    SOC_SAND_PP_DESTINATION_ID  dest_id;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_NULL_CHECK(encap_id);

    /* check case where vxlan port contain ECMP.
       In this case, no encap can be resolved, return an error */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_vxlan_encap_get_verify(unit, vxlan_port_id)); 


    rv = _bcm_dpp_gport_resolve(unit, vxlan_port_id, _BCM_DPP_GPORT_RESOLVE_FLAG_NO_CHECK, &dest_id, (int*)encap_id, &gport_type);
    BCMDNX_IF_ERR_EXIT(rv);
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Convert the ingress replication (destination + CUD) to a replication as expected by the soc functions.
 * The CUD may be changed.
 */
int _bcm_dpp_proccess_ingress_multicast_replication(
    int unit,                    /* input: unit */
    bcm_gport_t in_dest,         /* input: destination gport */
    bcm_if_t *inout_cud,         /* inout: CUD (encap) */
    SOC_TMC_DEST_INFO *out_dest) /* output: destination */
{
    uint32 fap_id, fap_port_id = 0;
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_tm_dest_info(unit, in_dest, out_dest)); /* Convert GPORT to TM dest */

    /* A CUD of 0 for a destination which is not an ERP system port or FMQ needs to be changed */
    if (!*inout_cud && !BCM_GPORT_IS_MCAST_QUEUE_GROUP(in_dest)) {
        if (out_dest->type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
            /* get module+port from system port, in order to check if this is an ERP port*/
            BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_sys_phys_to_local_port_map_get,(unit, out_dest->id, &fap_id, &fap_port_id)));
            if (BCM_SAND_FAP_PORT_ID_TO_BCM_PORT(fap_port_id) != SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID) {
                *inout_cud = BCM_IF_INVALID;
            }
        } else {
            *inout_cud = BCM_IF_INVALID;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Add a replication to the specified ingress multicast group.
 */
int bcm_petra_multicast_ingress_add(
    int unit,               /* input: unit/device */
    bcm_multicast_t group,  /* input: ingress MC group to work on */
    bcm_gport_t port,       /* input: replication port */
    bcm_if_t encap_id)      /* input: replication CUD */
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    SOC_TMC_DEST_INFO soc_petra_dest_info;
    bcm_if_t cud = encap_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group)); /* Check mc group type is supported */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id_ndx));
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY, multicast_id_ndx, SOC_SAND_TRUE)); /* Check if group exists */

    if (SOC_IS_QAX(unit)) {  /* QAX has a different implementation, and different soc functions usage */
        soc_multicast_replication_t rep;
        SOC_TMC_ERROR mc_err = BCM_E_NONE;
        rep.flags = 0;
        rep.port = port;
        rep.encap1 = encap_id;
        rep.encap2 = DPP_MC_NO_2ND_CUD;
        BCMDNX_IF_ERR_EXIT(qax_mult_ing_add_replications(unit, multicast_id_ndx, 1, &rep, &mc_err)); /* Add the replication */
        if (mc_err) {
            BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR)BCM_E_FULL ? "not enough multicast entries for the operation" :
              (mc_err == (SOC_TMC_ERROR)BCM_E_EXISTS ? "the same replication already appears in the group" :
              (mc_err == (SOC_TMC_ERROR)BCM_E_PARAM ? "Too many replications in group" : "unexpected error"))) ));
        }

    } else {
        SOC_TMC_DEST_INFO_clear(&soc_petra_dest_info);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_proccess_ingress_multicast_replication(unit, port, &cud, &soc_petra_dest_info)); /* Convert destination and CUD for soc function */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_entry_add(unit, multicast_id_ndx, soc_petra_dest_info, cud)); /* Add multicast replication */
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_ingress_delete
 * Purpose:
 *      Delete GPORT from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 *      port      - (IN) GPORT Identifier
 *      encap_id  - (IN) Encap ID.
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_ingress_delete(
    int unit, 
    bcm_multicast_t group, 
    bcm_gport_t port, 
    bcm_if_t encap_id)
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    SOC_TMC_DEST_INFO soc_petra_dest_info;
    bcm_if_t cud = encap_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group)); /* Check mc group type is supported */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id_ndx));
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY, multicast_id_ndx, SOC_SAND_TRUE)); /* Check if group exists */

    if (SOC_IS_QAX(unit)) {  /* QAX has a different implementation, and different soc functions usage */
        soc_multicast_replication_t rep;
        SOC_TMC_ERROR mc_err = BCM_E_NONE;
        rep.flags = 0;
        rep.port = port;
        rep.encap1 = encap_id;
        rep.encap2 = DPP_MC_NO_2ND_CUD;
        BCMDNX_IF_ERR_EXIT(qax_mult_ing_remove_replications(unit, multicast_id_ndx, 1, &rep, &mc_err)); /* Add the replication */
        if (mc_err) {
            BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_PARAM ? "replication not found in group" : "unexpected error") ));
        }

    } else {
        SOC_TMC_DEST_INFO_clear(&soc_petra_dest_info);
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_proccess_ingress_multicast_replication(unit, port, &cud, &soc_petra_dest_info)); /* Convert destination and CUD for soc function */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_entry_remove(unit, multicast_id_ndx, soc_petra_dest_info, cud)); /* Remove replication */
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_ingress_delete_all
 * Purpose:
 *      Delete all GPORTs from the replication list
 *      for the specified multicast index.
 * Parameters:
 *      unit      - (IN) Device Number
 *      group     - (IN) Multicast group ID
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_ingress_delete_all(
    int unit, 
    bcm_multicast_t group)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_multicast_ingress_set(unit,group,0,NULL,NULL));
exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 * Function:
 *      bcm_petra_multicast_ingress_get
 * Purpose:
 *      Retrieve a set of ingress multicast GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      mc_index   - (IN) Multicast index
 *      port_max   - (IN) Number of entries in "port_array"
 *      port_array     - (OUT) List of multicast GPORTs 
 *      encap_id_array - (OUT) List of encap identifiers
 *      port_count     - (OUT) Actual number of multicasts returned
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      If the input parameter port_max = 0, return in the output parameter
 *      port_count the total number of ports/encapsulation IDs in the 
 *      specified multicast group's replication list.
 */
int 
bcm_petra_multicast_ingress_get(
    int unit, 
    bcm_multicast_t group, 
    int port_max, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array, 
    int *port_count)
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    uint32 group_size;
    uint8 open;
       
    BCMDNX_INIT_FUNC_DEFS;

    /* verify input parameters */
    BCMDNX_NULL_CHECK(port_count);
    if (port_max > 0) {
        BCMDNX_NULL_CHECK(port_array);
        BCMDNX_NULL_CHECK(port_count);
    } else if (port_max < 0) {
        port_max = 0;
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group)); /* Check mc group type is supported */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group,&multicast_id_ndx)); /* get group mcid */

    /* Check multicast group exists (was created) */
    BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, FALSE, &open));
    if (!open) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Egress multicast group %u was not created"), multicast_id_ndx));
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_get_group,(unit, multicast_id_ndx, port_max, port_array, encap_id_array, NULL, &group_size, &open))); /* do the actual work */
    if (!open) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Not clear if ingress multicast group %u was created"), multicast_id_ndx));
    }
    /* if port_max is non zero and not all group replications could be returned,
     * return the number of returned replications (port_max).
     * Otherwise return the exact group size in replications (a bitmap pointer counts as one replication).
     */
    *port_count = port_max && group_size > port_max ? port_max : group_size;
    
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_petra_multicast_ingress_set
 * Purpose:
 *      Assign the complete set of ingress GPORTs in the
 *      replication list for the specified multicast index.
 * Parameters:
 *      unit       - (IN) Device Number
 *      group      - (IN) Multicast group ID
 *      port_count   - (IN) Number of ports in replication list
 *      port_array   - (IN) List of GPORT Identifiers
 *      encap_id_array - (IN) List of encap identifiers
 * Returns:
 *      BCM_E_XXX
 */
int 
bcm_petra_multicast_ingress_set(
    int unit, 
    bcm_multicast_t group, 
    int port_count, 
    bcm_gport_t *port_array, 
    bcm_if_t *encap_id_array)
{
    bcm_error_t rv;
    SOC_TMC_MULT_ID multicast_id_ndx;
    int ndx;
    SOC_TMC_DEST_INFO *dest_info_arr = NULL;
    bcm_if_t *encap_id_array_copy = NULL;
    uint8 expected_open;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    if (port_count > 0) {
        BCMDNX_NULL_CHECK(port_array);
        BCMDNX_NULL_CHECK(encap_id_array);
    }
    
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group)); /* Check mc group type is supported */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group,&multicast_id_ndx)); /* extract the MCID */

    if (SOC_IS_QAX(unit)) {  /* QAX has a different implementation, and different soc functions usage */
        BCMDNX_IF_ERR_EXIT(_bcm_qax_ingress_multicast_set(unit, multicast_id_ndx, port_count, NULL, port_array, encap_id_array));
        SOC_EXIT;
    }

    /* Allocate dest info entries */
    BCMDNX_ALLOC(dest_info_arr, sizeof(SOC_TMC_DEST_INFO) * port_count, "dest info entries");
    if (dest_info_arr == NULL) {
        BCMDNX_IF_ERR_EXIT(BCM_E_MEMORY);
    }

    for (ndx = 0; ndx < port_count; ndx ++) { /* convert replications to the input needed by the soc function */
        bcm_if_t cud = encap_id_array[ndx];
        SOC_TMC_DEST_INFO_clear(&dest_info_arr[ndx]); /* init destination structure */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_proccess_ingress_multicast_replication(unit, port_array[ndx], &cud, dest_info_arr + ndx));
        if (cud != encap_id_array[ndx]) {
            if (!encap_id_array_copy) { /* if the encapsulation ids copy array was not allocated, allocate it and populate it from the original */
                int i;
                BCMDNX_ALLOC(encap_id_array_copy, sizeof(bcm_if_t) * port_count, "encap_ids");
                if (encap_id_array_copy == NULL) {
                    BCMDNX_IF_ERR_EXIT(BCM_E_MEMORY);
                }
                for (i = 0; i < port_count; ++i) {
                    encap_id_array_copy[i] = encap_id_array[i];
                }
            }
            encap_id_array_copy[ndx] = cud;
        }
    }

    /* Check multicast ID exists */
    expected_open = SOC_SAND_TRUE;
    rv = _bcm_petra_multicast_is_group_exist(unit,BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY,multicast_id_ndx,expected_open);
    BCMDNX_IF_ERR_EXIT(rv);

    /* Update existed multicast ing group */
    rv = _bcm_petra_multicast_ingress_entries_set(unit, multicast_id_ndx, port_count, dest_info_arr,
                                                  encap_id_array_copy ? encap_id_array_copy : encap_id_array);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCM_FREE(dest_info_arr);
    BCM_FREE(encap_id_array_copy);
    BCMDNX_FUNC_RETURN;
}


/* New APIs needed for Jericho */

/* get the content (replications) of a multicast group */
int _bcm_dpp_multicast_get_ingress(
    int unit, 
    SOC_TMC_MULT_ID mcid, 
    int max_replications, 
    bcm_multicast_replication_t *out_rep_array, 
    int *rep_count)
{
    uint32 group_size, i, reps_array_size = max_replications > 0 ? max_replications : 0;
    uint8 open;
    bcm_multicast_replication_t *rep = out_rep_array;
    soc_gport_t *dest_arr = NULL, small_dest_array[BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE];
    bcm_if_t *cud_array = NULL, small_cud_array[BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE];
    BCMDNX_INIT_FUNC_DEFS;

    if (reps_array_size > BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE) {
        BCMDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &dest_arr, sizeof(soc_gport_t) * reps_array_size, "destinations")); /* Allocate dests */
        BCMDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &cud_array, sizeof(bcm_if_t) * reps_array_size, "CUDs")); /* Allocate CUD array */
    } else {
        soc_sand_os_memset(small_dest_array, BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE, sizeof(soc_gport_t));
        soc_sand_os_memset(small_cud_array, BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE, sizeof(bcm_if_t));
        dest_arr = small_dest_array;
        cud_array = small_cud_array;
    }
    
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mult_ing_get_group, (unit, mcid, reps_array_size, dest_arr, cud_array, (soc_multicast_replication_t*)rep, &group_size, &open))); /* do the actual work */
    if (!open) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Not clear if ingress multicast group %u was created"), mcid));
    }

    if (reps_array_size > 0) {
        if (reps_array_size < group_size) {
            group_size = reps_array_size;
        }
        if (!SOC_IS_QAX(unit)) {
            for (i = 0; i < group_size; ++i, ++rep) { /* copy replications to output array */
                rep->flags = 0;
                rep->port = dest_arr[i];
                rep->encap1 = cud_array[i];
                rep->encap2 = DPP_MC_NO_2ND_CUD;
            }
        }
    }
    if (rep_count != NULL) {
        *rep_count = group_size;
    }
    
exit:
    if (dest_arr && dest_arr != small_dest_array) {
        BCMDNX_IF_ERR_REPORT(sand_free_mem(unit, (void*)&dest_arr));
    }
    if (cud_array && cud_array != small_cud_array) {
        BCMDNX_IF_ERR_REPORT(sand_free_mem(unit, (void*)&cud_array));
    }
    BCMDNX_FUNC_RETURN;
}

/* get the content (replications) of a multicast group */
int bcm_petra_multicast_get(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int max_replications, 
    bcm_multicast_replication_t *out_rep_array, 
    int *rep_count)
{
    SOC_TMC_MULT_ID multicast_id_ndx;
    uint8 open;
    int is_egress = !(flags & BCM_MULTICAST_INGRESS);
    int is_bitmap_group;
       
    BCMDNX_INIT_FUNC_DEFS;

    /* verify input parameters */
    if (max_replications > 0) {
        BCMDNX_NULL_CHECK(out_rep_array);
    } else {
        BCMDNX_NULL_CHECK(rep_count);
        if (max_replications < 0) {
            max_replications = 0;
        }
    }
    if (flags & ~(uint32)BCM_MULTICAST_INGRESS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags")));
    }

    BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group)); /* Check if mc group type is supported */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id_ndx)); /* get group mcid */
    /* Check multicast group exists (was created) */
    BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_is_group_created(unit, multicast_id_ndx, is_egress, &open));
    if (!open) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("%sgress multicast group %u was not created"), is_egress ? "E" : "In", multicast_id_ndx));
    }

    if (is_egress) { /* egress multicast group */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_egress_get(unit, group, max_replications, NULL, NULL, out_rep_array, rep_count));
    } else { /* ingress multicast group */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_multicast_get_ingress(unit, multicast_id_ndx, max_replications, out_rep_array, rep_count));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* set the content (replications) of an ingress multicast group (Arad and up) */
int _bcm_dpp_ingress_multicast_set(
    int unit, 
    SOC_TMC_MULT_ID mcid,
    int nof_replications, 
    bcm_multicast_replication_t *rep_array)
{
    int i;
    SOC_TMC_DEST_INFO *dest_info_arr = NULL, *dest_info, small_dests_array[BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE];
    bcm_if_t *cud_array = NULL, small_cud_array[BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE];
    BCMDNX_INIT_FUNC_DEFS;

    if (nof_replications > BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE) {
        BCMDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &dest_info_arr, sizeof(SOC_TMC_DEST_INFO) * nof_replications,
          "dest info entries")); /* Allocate dest info entries */
        BCMDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &cud_array, sizeof(bcm_if_t) * nof_replications, "CUDs")); /* Allocate CUD array */
    } else {
        dest_info_arr = small_dests_array;
        cud_array = small_cud_array;
    }

    for (i = 0; i < nof_replications; ++i) { /* convert replications to the input needed by the soc function */
        dest_info = dest_info_arr + i;
        cud_array[i] = rep_array[i].encap1;
        SOC_TMC_DEST_INFO_clear(dest_info); /* init destination structure */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_proccess_ingress_multicast_replication(unit, rep_array[i].port, cud_array + i, dest_info));
    }

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY, mcid, SOC_SAND_TRUE)); /* Check multicast ID exists */

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_entries_set(unit, mcid, nof_replications, dest_info_arr, cud_array)); /* Update existed multicast ing group */

exit:
    if (dest_info_arr && dest_info_arr != small_dests_array) {
        BCMDNX_IF_ERR_REPORT(sand_free_mem(unit, (void*)&dest_info_arr));
    }
    if (cud_array && cud_array != small_cud_array) {
        BCMDNX_IF_ERR_REPORT(sand_free_mem(unit, (void*)&cud_array));
    }

    BCMDNX_FUNC_RETURN;
}

/* set the content (replications) of a multicast group */
int bcm_petra_multicast_set(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int nof_replications, 
    bcm_multicast_replication_t *rep_array)
{
    SOC_TMC_MULT_ID mcid;
    int is_bitmap_group;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    if (nof_replications > 0) {
        BCMDNX_NULL_CHECK(rep_array);
    }

    if (flags & ~BCM_MULTICAST_INGRESS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags")));
    }

    {
        BCMDNX_IF_ERR_EXIT(_bcm_arad_eg_multicast_is_supported_mc_group(unit, group, &is_bitmap_group));
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &mcid));
    }

    if (flags & BCM_MULTICAST_INGRESS) { /* ingress multicast group */
        if (SOC_IS_QAX(unit)) {  /* QAX has a different implementation, and different soc functions usage */
            BCMDNX_IF_ERR_EXIT(_bcm_qax_ingress_multicast_set(unit, mcid, nof_replications, rep_array, NULL, NULL));
        } else {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_ingress_multicast_set(unit, mcid, nof_replications, rep_array));
        }
    } else { /* egress multicast group */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_set_egress(unit, group, nof_replications, NULL, NULL, rep_array));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Add or delete an ingress multicast replication */
int _bcm_dpp_ingress_multicast_add_or_delete(
    int unit,
    bcm_multicast_t group,
    int nof_replications, 
    bcm_multicast_replication_t *rep_array,
    uint8 operation_is_delete)
{
    int i;
    SOC_TMC_MULT_ID multicast_id_ndx;
    SOC_TMC_DEST_INFO *dest_info_arr = NULL, *dest_info, small_dests_array[BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE];
    bcm_if_t *cud_array = NULL, small_cud_array[BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE];
    SOC_TMC_ERROR mc_err = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_supported_mc_group(group)); /* Check that MC group type is supported */
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_group_to_id(group, &multicast_id_ndx));
    BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_is_group_exist(unit, BCM_DPP_MULTICAST_TYPE_INGRESS_ONLY, multicast_id_ndx, SOC_SAND_TRUE)); /* Check if group exists */
    if (nof_replications <= 0) {
        BCM_EXIT;
    } else if (nof_replications > 1) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("nof_replications > 1 not currently supported")));
    } else if (SOC_IS_QAX(unit)) {
        if (operation_is_delete) {
            BCMDNX_IF_ERR_EXIT(qax_mult_ing_remove_replications(unit, multicast_id_ndx,
              nof_replications, (soc_multicast_replication_t *)rep_array, &mc_err)); /* Remove the replication */
        } else {
            BCMDNX_IF_ERR_EXIT(qax_mult_ing_add_replications(unit, multicast_id_ndx,
              nof_replications, (soc_multicast_replication_t *)rep_array, &mc_err)); /* Add the replication */
        }
        if (mc_err) {
            BCMDNX_ERR_EXIT_MSG(mc_err, (_BSL_BCM_MSG("%s"), (mc_err == (SOC_TMC_ERROR) BCM_E_FULL ? "not enough multicast entries for the operation" :
              (mc_err == (SOC_TMC_ERROR) BCM_E_PARAM ? "Too many replications, or the same replication appears multiple times in the input" : "unexpected error")) ));
        }
        SOC_EXIT;
    }

    /* coverity[dead_error_begin] */
    if (nof_replications > BCM_DPP_MULTICAST_SMALL_ARRAY_SIZE) {
        BCMDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &dest_info_arr, sizeof(SOC_TMC_DEST_INFO) * nof_replications,
          "dest info entries")); /* Allocate dest info entries */
        BCMDNX_IF_ERR_EXIT(sand_alloc_mem(unit, &cud_array, sizeof(bcm_if_t) * nof_replications, "CUDs")); /* Allocate CUD array */
    } else {
        dest_info_arr = small_dests_array;
        cud_array = small_cud_array;
    }

    for (i = 0; i < nof_replications; ++i) { /* convert replications to the input needed by the soc function */
        dest_info = dest_info_arr + i;
        SOC_TMC_DEST_INFO_clear(dest_info); /* init destination structure */
        cud_array[i] = rep_array[i].encap1;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_proccess_ingress_multicast_replication(unit, rep_array[i].port, cud_array + i, dest_info));
    }

    if (operation_is_delete) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_entry_remove(unit, multicast_id_ndx, dest_info_arr[0], cud_array[0])); /* Remove the replication */
    } else {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_entry_add(unit, multicast_id_ndx, dest_info_arr[0], cud_array[0])); /* Add the replication */
    }

exit:
    if (dest_info_arr && dest_info_arr != small_dests_array) {
        BCMDNX_IF_ERR_REPORT(sand_free_mem(unit, (void*)&dest_info_arr));
    }
    if (cud_array && cud_array != small_cud_array) {
        BCMDNX_IF_ERR_REPORT(sand_free_mem(unit, (void*)&cud_array));
    }

    BCMDNX_FUNC_RETURN;
}

/* remove multicast destinations */
int bcm_petra_multicast_delete(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int nof_replications, 
    bcm_multicast_replication_t *rep_array)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    if (nof_replications > 0) {
        BCMDNX_NULL_CHECK(rep_array);
    } else if (flags & ~(uint32)BCM_MULTICAST_INGRESS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags")));
    }

    if (flags & BCM_MULTICAST_INGRESS) { /* ingress multicast group */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_ingress_multicast_add_or_delete(unit, group, nof_replications, rep_array, TRUE));
    } else { /* egress multicast group */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_egress_add_or_delete(unit, group, nof_replications, rep_array, TRUE));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Add multicast destinations */
int bcm_petra_multicast_add(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int nof_replications, 
    bcm_multicast_replication_t *rep_array)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);                      
    if (nof_replications > 0) {
        BCMDNX_NULL_CHECK(rep_array);
    } else if (flags & ~(uint32)BCM_MULTICAST_INGRESS) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("unsupported flags")));
    }

    if (flags & BCM_MULTICAST_INGRESS) { /* ingress multicast group */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_ingress_multicast_add_or_delete(unit, group, nof_replications, rep_array, FALSE));
    } else { /* egress multicast group */
        BCMDNX_IF_ERR_EXIT(_bcm_arad_multicast_egress_add_or_delete(unit, group, nof_replications, rep_array, FALSE));
    }

exit:
    BCMDNX_FUNC_RETURN;
}
