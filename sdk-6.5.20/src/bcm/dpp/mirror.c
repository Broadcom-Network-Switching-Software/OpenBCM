/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Mirror - Broadcom Mirror API.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_MIRROR

#include <shared/bsl.h>

#include "bcm_int/common/debug.h"

#include <soc/drv.h>
#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/stack.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/mbcm_pp.h>
#include <soc/dpp/PPD/ppd_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/ARAD/arad_api_action_cmd.h>
#include <bcm_int/dpp/sw_db.h>
#include <bcm_int/dpp/gport_mgmt.h>
#include <bcm_int/dpp/counters.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/policer.h>
#include <bcm_int/dpp/multicast.h>
#include <bcm_int/dpp/mirror.h>
#include <bcm_int/dpp/tunnel.h>
#include <bcm_int/petra_dispatch.h>
#include <bcm_int/dpp/switch.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>

/*
 *  Ingress mirroring is done using a mirror action table.  This table allows
 *  control over not only the destination for the mirror, but traffic class,
 *  drop probability, metering, and counting.  The mirror action table is used
 *  by several APIs, including the mirroring APIs and the mirror field action.
 *
 *  In general, though, the BCM APIs don't provide a way to get to all of these
 *  extra controls, so most of it will be left in a state indicating these
 *  attributes of the mirror copies will remain unchanged from the original.
 *  We represent the internal features here, in the expectation that some
 *  customer will want them and so an appropriate API adjustment will be made
 *  later to take advantage.
 *
 *  Ingress mirroring is done through templates.  This means that if more than
 *  one thing wants a mirror with the same parameters, they will use the same
 *  template.  Templates (and the underlying resources) are recovered when
 *  nothing else wants to use a particular one.  This means that anything
 *  wanting a mirror should alloc it, and free it after use.  If the mirror
 *  needs to be changed while in use, the change API will try to do this
 *  in-place (if there is only one reference to the mirror) or will try to
 *  allocate a new mirror if it is already shared.  Template zero is reserved
 *  during init for use a 'no mirroring', and will not be edited.
 */

/******************************************************************************
 *
 *  Local functions and data
 */


/*
 *  Some basic macros
 */

/* non recursive safe MACROS for give & take */
#define SOC_PETRA_MIRROR_LOCK_TAKE(_unit) \
    if (sal_mutex_take(sw_state_sync_db[(_unit)].dpp.mirror_lock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unable to take unit %d mirror lock"), (_unit))); \
    }
#define SOC_PETRA_MIRROR_LOCK_GIVE(_unit) \
    if (sal_mutex_give(sw_state_sync_db[(_unit)].dpp.mirror_lock)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unable to release unit %d mirror lock"), (_unit))); \
    }

#define MIRROR_LOCK_DEFS int _in_lock = 0
#define MIRROR_LOCK_TAKE do { \
                             if (!_in_lock) { \
                                 SOC_PETRA_MIRROR_LOCK_TAKE(unit); \
                                 _in_lock = 1; \
                             } \
                         } while (0)
#define MIRROR_LOCK_GIVE do { \
                             if (_in_lock) { \
                                 SOC_PETRA_MIRROR_LOCK_GIVE(unit); \
                                 _in_lock = 0;\
                             } \
                         } while (0)


#define _NON_LOCAL_PORT ((SOC_PPC_PORT)(-1))

/* mirror probability encoding constants */
#define ARAD_MIRROR_PROBABILITY_BITS (SOC_DPP_IMP_DEFS_GET(unit,mirror_probability_bits))
/* #define ARAD_MIRROR_PROBABILITY_BITS ((SOC_IS_JERICHO(unit)) ? 24 : 16) */
#define ARAD_MIRROR_PROBABILITY_DIVISOR (1 << ARAD_MIRROR_PROBABILITY_BITS)
#define ARAD_MIRROR_PROBABILITY_ALLWAYS (ARAD_MIRROR_PROBABILITY_DIVISOR - 1)

#define DPP_MIRROR_SNOOP_PROBABILITY_ALLWAYS(prob_bits) ((1 << prob_bits) - 1)
#define JER_OUTBOUND_MIRROR_SNOOP_PROBABILITY_BITS 32
#define JER_MIRROR_PROBABILITY_DIVISOR 0xffffffff

#define JER_OUTBOUND_MIRROR_CROP_SIZE 128

#define ARAD_MIRROR_CMD_OUTLIF_MAX    0x3ffff

#define MIRROR_ACCESS  sw_state_access[unit].dpp.bcm.mirror

/* calculates the inbound/outbound mirroring/snooping probability
 * The probability of mirroring a packet is:
 * dividend >= divisor ? 1 : dividend / divisor
 * Parameters
 *     (in) uint32 dividend
 *     (in) uint32 divisor
 *     (in) int prob_bits = number of probability bits in HW
 *  Returns
 *     int prob_field
 */
STATIC int
_bcm_petra_mirror_or_snoop_probability_get(uint32 dividend,
                                           uint32 divisor,
                                           int prob_bits);


/*
 * For local ports, returns the local port number in out_loc_port and fail if the port is not valid.
 * For valid non local ports, return _NON_LOCAL_PORT.
 * For invalid non local ports, either return _NON_LOCAL_PORT or fail.
 */

STATIC int
_bcm_petra_get_local_valid_port(int unit,
                               bcm_port_t port,
                               SOC_PPC_PORT *out_loc_port)
{
    SOC_PPC_PORT loc_port;
    _bcm_dpp_gport_info_t gport_info;
    BCMDNX_INIT_FUNC_DEFS;
    *out_loc_port = _NON_LOCAL_PORT;

    /* check if port is local and get the port. If so, does not check its validaty properly */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info));
    if (_BCM_DPP_GPORT_INFO_IS_LOCAL_PORT(gport_info)) {

        BCM_PBMP_ITER(gport_info.pbmp_local_ports,loc_port) {
            *out_loc_port = loc_port;
            break; /* If for some reason the bitmap contains multiple ports, use only the first one */
        }
        if (*out_loc_port == _NON_LOCAL_PORT) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("error getting local port")));
        }
        /* The Macro SOC_PORT_VALID checkes that port >= 0 it may be needed for signed varibles */
        /* coverity[unsigned_compare : FALSE] */
        if (!SOC_PORT_VALID(unit, loc_port)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("local port %lu is not valid"),(unsigned long)loc_port));
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_petra_mirror_detach
 *  Purpose
 *     De-initialise the field APIs.
 *  Parameters
 *     (in) int unit = the unit number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
STATIC int
_bcm_petra_mirror_detach(int unit)
{

    BCMDNX_INIT_FUNC_DEFS;
    /* tear down this unit */
    /* make sure nobody else is using it right now */
    if (sal_mutex_take(sw_state_sync_db[unit].dpp.mirror_lock, sal_mutex_FOREVER)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unable to take unit lock\n")));
        /* can't destroy it now, so put it back */
    } else {
        /* took unit lock, destroy unit information */
        sal_mutex_give(sw_state_sync_db[unit].dpp.mirror_lock);
        sal_mutex_destroy(sw_state_sync_db[unit].dpp.mirror_lock);
    }
    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_petra_ingress_mirror_alloc
 *  Purpose
 *    Allocate an ingress mirror template according to the parameters
 *  Arguments
 *    (in) int unit
 *    (in) bcm_module_t destMod = destination module
 *    (in) bcm_port_t destPort = destination port
 *    (out) int *mirrorId = where to put allocated ingress mirror ID
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Builds a mirror descriptor and uses the template manager to allocate a
 *    mirror template according to that decriptor.
 *
 *    Right now, only configures destination in mirror action so that is all it
 *    compares when looking for a matching template.
 */
STATIC int
_bcm_petra_ingress_mirror_alloc(int unit,
                                bcm_module_t myModule,
                                bcm_module_t destMod,
                                bcm_port_t destPort,int* mirrorId)
{
    ARAD_ACTION_CMD_SNOOP_MIRROR_INFO mirrorInfo;
    unsigned int index;
    unsigned int free;
    int result;
    uint32 ppdr;
    bcm_gport_t dest;
    unsigned int ingressCount;
    unsigned int refCount;
    ARAD_ACTION_CMD_SNOOP_MIRROR_INFO cmd_mirrorInfo;

    BCMDNX_INIT_FUNC_DEFS;

    if (-1 == destMod) {
        /* -1 == destMod means use local module ID */
        destMod = myModule;
    }
    if (BCM_GPORT_IS_SET(destPort)) {
        /* it is a GPORT, so use that */
        dest = destPort;
    } else {
        /* it is not a GPORT, so it's a local module+port */
        BCM_GPORT_MODPORT_SET(dest, destMod, destPort);
    }

    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&mirrorInfo);
    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&cmd_mirrorInfo);
    /* always mirror */
    mirrorInfo.prob = 0x3FF; /* 1023/1023 probability of mirroring */
    /* fill in forwarding decision */
    result = _bcm_dpp_gport_to_tm_dest_info(unit,
                                            dest,
                                            &mirrorInfo.cmd.dest_id);
    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG("unable to compute dest info for unit"
                                " %d module %d port %d (GPORT %08X):"
                                " %d (%s)\n"),
                    unit,
                    destMod,
                    destPort,
                    dest,
                    result,
                    _SHR_ERRMSG(result)));
    }
    BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingressCount.get(unit, &ingressCount));
    for (index = 0, free = ingressCount;
         index < ingressCount;
         index++) {
        BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.get(unit, index, &refCount));
        if (refCount) {
            BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.mirrorInfo.get(unit, index, &cmd_mirrorInfo));
            /* this template is in use */
            if (0 == sal_memcmp(&(cmd_mirrorInfo.cmd.dest_id),
                                &(mirrorInfo.cmd.dest_id),
                                sizeof(mirrorInfo.cmd.dest_id))) {
                /* this template is in use and points to the same place */
                break;
            }
        } else { /* if refCount */
            /* this template is not in use */
            if (ingressCount <= free) {
                /* we have not found a free one yet, keep this one */
                free = index;
            }
        } /* if refCount */
    } /* for (all ingress templates) */
    if (ingressCount > index) {
        /* found one that matches */
        *mirrorId = index;
        refCount++;
        BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.set(unit, index, refCount));
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d reused ingress mirror"
                              " template %d\n"),
                   unit,
                   index));
    } else if (ingressCount > free) {
        /* found a free one */
        ppdr = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_set,(unit, free, SOC_TMC_CMD_TYPE_MIRROR, &mirrorInfo));
        result = handle_sand_result(ppdr);
        if (BCM_E_NONE == result) {
            LOG_DEBUG(BSL_LS_BCM_MIRROR,
                      (BSL_META_U(unit,
                                  "unit %d allocated ingress mirror"
                                  " template %d\n"),
                       unit,
                       free));
            *mirrorId = free;
            refCount++;
            BCM_DPP_UNIT_CHECK(unit);
            BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.set(unit, index, refCount));
            /* set template local copy */
            BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.mirrorInfo.set(unit, free, &cmd_mirrorInfo));
        } else {
            BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG("unit %d unable to set ingress mirror"
                                    " template %d: %d (%s)\n"),
                        unit,
                        free,
                        result,
                        _SHR_ERRMSG(result)));
        }
    } else {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d has no available ingress mirror"
                              " template\n"),
                   unit));
        BCM_RETURN_VAL_EXIT(BCM_E_RESOURCE);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_petra_ingress_mirror_reference
 *  Purpose
 *    Add a reference to an ingress mirror template
 *  Arguments
 *    (in) int unit
 *    (in) int mirrorId = mirror template ID for the reference
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This is intended to be used as part of warm boot or similar.  When other
 *    modules are rebuilding their state, they call this function as they
 *    encounter references to mirror templates.  This allows the mirror module
 *    to know how many outstanding references there are to each template.
 *
 *    Template 0 is reserved (for no-mirror) and should never be freed.
 */
STATIC int
_bcm_petra_ingress_mirror_reference(int unit,
                                    int mirrorId)
{
    unsigned int refCount;
    BCMDNX_INIT_FUNC_DEFS;

    if (0 == mirrorId) {
        /* template zero is reserved and must not be freed */
        BCM_EXIT;
    }
    BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.get(unit, mirrorId, &refCount));
    refCount++;
    BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.set(unit, mirrorId, refCount));

    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "unit %d add one user to ingress mirror"
                          " template %d\n"),
               unit,
               mirrorId));
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_petra_ingress_mirror_free
 *  Purpose
 *    Free a specified ingress mirror
 *  Arguments
 *    (in) int unit
 *    (in) int mirrorId = ingress mirror ID to free
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Frees (one reference to) an ingress mirror template.
 *
 *    Template 0 is reserved (for no-mirror) and should never be freed.
 */
STATIC int
_bcm_petra_ingress_mirror_free(int unit,
                               int mirrorId)
{
    unsigned int refCount;
    BCMDNX_INIT_FUNC_DEFS;

    if (0 == mirrorId) {
        /* template zero is reserved and must not be freed */
        BCM_EXIT;
    }
    BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.get(unit, mirrorId, &refCount));
    if (refCount) {
        refCount--;
        BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.set(unit, mirrorId, refCount));

        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d remove %s user from ingress mirror"
                              " template %d\n"),
                   unit,
                   refCount?"one":"last",
                   mirrorId));
    } else {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror template %d is not"
                              " currently in use\n"),
                   unit,
                   mirrorId));
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_petra_ingress_mirror_change
 *  Purpose
 *    Change an ingress mirror
 *  Arguments
 *    (in) int unit
 *    (in) int oldMirrorId = old ingress mirror ID
 *    (in) bcm_module_t destMod = destination module
 *    (in) bcm_port_t destPort = destination port
 *    (out) int *newMirrorId = where to put new ingress mirror ID
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This will only edit the template in-place if there is exactly one
 *    reference to it.  If there is more than one reference to the template, it
 *    will allocate a replacement template and provide that, releasing one
 *    reference to the old template.  If success and the oldMirrorId and
 *    newMirrorId values are the same, the template was edited in place and
 *    there should be no need to make changes to the reference.
 *
 *    Once the other table (field entry, port descriptor, &c) has been updated
 *    to use the new mirror template, the old one must be freed (this is not
 *    done automatically here).  If such edits fail, the new mirror template
 *    should be freed instead of the old one.  This is done so that a template
 *    will not be overwritten if there are other things happening or more than
 *    one subsystem updates mirror targets concurrently.
 */
STATIC int
_bcm_petra_ingress_mirror_change(int unit,
                                 int oldMirrorId,
                                 bcm_module_t myModule,
                                 bcm_module_t destMod,
                                 bcm_port_t destPort,
                                 int *newMirrorId)
{
    ARAD_ACTION_CMD_SNOOP_MIRROR_INFO mirrorInfo;
    unsigned int index;
    unsigned int free;
    int result;
    uint32 ppdr;
    bcm_gport_t dest;
    unsigned int refCount;
    unsigned int old_refCount;
    unsigned int free_refCount = 0;
    unsigned int ingressCount;
    ARAD_ACTION_CMD_SNOOP_MIRROR_INFO cmd_mirrorInfo;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.get(unit, oldMirrorId, &old_refCount));
    if (!old_refCount) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d can't change non-used mirror template"
                              " %d\n"),
                   unit,
                   oldMirrorId));
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }
    /* at least one reference, so old template in use */
    if (-1 == destMod) {
        /* -1 == destMod means use local module ID */
        destMod = myModule;
    }
    if (BCM_GPORT_IS_SET(destPort)) {
        /* it is a GPORT, so use that */
        dest = destPort;
    } else {
        /* it is not a GPORT, so it's a local module+port */
        BCM_GPORT_MODPORT_SET(dest, destMod, destPort);
    }

    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&mirrorInfo);
    /* always mirror */
    mirrorInfo.prob = 0x3FF; /* 1023/1023 probability of mirroring */
    /* fill in forwarding decision */
    result = _bcm_dpp_gport_to_tm_dest_info(unit,
                                            dest,
                                            &mirrorInfo.cmd.dest_id);
    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG("unable to compute dest info for unit"
                                " %d module %d port %d (GPORT %08X):"
                                " %d (%s)\n"),
                    unit,
                    destMod,
                    destPort,
                    dest,
                    result,
                    _SHR_ERRMSG(result)));
    }
    /* see if there is a match for the new settings */
    BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingressCount.get(unit, &ingressCount));
    for (index = 0, free = ingressCount;
         index < ingressCount;
         index++) {
        BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.get(unit, index, &refCount));
        if (refCount) {
            /* this template is in use */
            BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.mirrorInfo.get(unit, index, &cmd_mirrorInfo));
            if (0 == sal_memcmp(&(cmd_mirrorInfo.cmd.dest_id),
                                &(mirrorInfo.cmd.dest_id),
                                sizeof(mirrorInfo.cmd.dest_id))) {
                /* this template is in use and points to same place */
                break;
            }
        } else { /* if refCount */
            /* this template is not in use */
            if (ingressCount <= free) {
                /* we have not found a free one yet, keep this one */
                free = index;
                free_refCount = refCount;
            }
        } /* if refCount */
    } /* for (all ingress templates) */
    if (ingressCount > index) {
        /* found one that matches, use it */
        *newMirrorId = index;
        /* shift reference to new template */
        refCount++;
        BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.set(unit, index, refCount));

        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d reused ingress mirror"
                              " template %d\n"),
                   unit,
                   index));
    } else if ((1 == old_refCount) ||
               (ingressCount > free)) {
        /* no existing match, == 1 reference or free template found */
        if (1 == old_refCount) {
            /* can update in place, so use that instead of free */
            free = oldMirrorId;
        }
        ppdr = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_set,(unit, free, SOC_TMC_CMD_TYPE_MIRROR, &mirrorInfo));
        result = handle_sand_result(ppdr);
        if (BCM_E_NONE != result) {
            BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG("unit %d unable to set ingress"
                                    " mirror template %d: %d (%s)\n"),
                        unit,
                        free,
                        result,
                        _SHR_ERRMSG(result)));
        }
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d %s ingress mirror"
                              " template %d\n"),
                   unit,
                   (free == oldMirrorId)?"updated":"allocated",
                   free));
        *newMirrorId = free;
        /* add reference to new template */
        free_refCount++;
        BCM_DPP_UNIT_CHECK(unit);
        BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.set(unit, free, free_refCount));
        /* set new template local copy */
        BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.mirrorInfo.set(unit, free, &mirrorInfo));
    } else {
        /* no existing match, >1 reference and no free templates */
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d has no available ingress"
                              " mirror template\n"),
                   unit));
        BCM_RETURN_VAL_EXIT(BCM_E_RESOURCE);
    }

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_petra_ingress_mirror_get
 *  Purpose
 *    Get information about an ingress mirror
 *  Arguments
 *    (in) int unit
 *    (in) int mirrorId = ingress mirror ID
 *    (out) bcm_module_t *destMod = where to put destination module
 *    (out) bcm_port_t *destPort = where to put destination port
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This gets the cached copy, not the hardware state.
 */
STATIC int
_bcm_petra_ingress_mirror_get(int unit,
                              int mirrorId,
                              bcm_module_t *destMod,
                              bcm_port_t *destPort)
{
    ARAD_ACTION_CMD_SNOOP_MIRROR_INFO mirrorInfo;
    int result;
    bcm_gport_t dest;
    bcm_module_t myModule;
    unsigned int refCount;

    BCMDNX_INIT_FUNC_DEFS;

    result = bcm_petra_stk_my_modid_get(unit, &myModule);
    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG("unable to get unit %d module ID: %d (%s)\n"),
                    unit,
                    result,
                    _SHR_ERRMSG(result)));
    }
    BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.refCount.get(unit, mirrorId, &refCount));
    if (!refCount) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d mirror template %d not in use\n"),
                   unit,
                   mirrorId));
        BCM_RETURN_VAL_EXIT(BCM_E_NOT_FOUND);
    }
    /* at least one reference, so template in use */
    BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.ingress.mirrorInfo.get(unit, mirrorId, &mirrorInfo));
    result = _bcm_dpp_gport_from_tm_dest_info(unit,
                                              &dest,
                                              &(mirrorInfo.cmd.dest_id));

    if (BCM_E_NONE != result) {
        BCMDNX_ERR_EXIT_MSG(result, (_BSL_BCM_MSG("unable to compute GPORT from unit %d"
                                " ingress template %d dest info"
                                " %d.%d(%08X): %d (%s)\n"),
                    unit,
                    mirrorId,
                    mirrorInfo.cmd.dest_id.type,
                    mirrorInfo.cmd.dest_id.id,
                    mirrorInfo.cmd.dest_id.id,
                    result,
                    _SHR_ERRMSG(result)));
    }

    if (BCM_GPORT_IS_LOCAL(dest)) {
        *destMod = myModule;
        *destPort = BCM_GPORT_LOCAL_GET(dest);
    } else if (BCM_GPORT_IS_MODPORT(dest)) {
        *destMod = BCM_GPORT_MODPORT_MODID_GET(dest);
        *destPort = BCM_GPORT_MODPORT_PORT_GET(dest);
    } else if (BCM_GPORT_IS_EGRESS_MODPORT(dest)) {
        *destMod = BCM_GPORT_EGRESS_MODPORT_MODID_GET(dest);
        *destPort = BCM_GPORT_EGRESS_MODPORT_PORT_GET(dest);
    } else {
        *destMod = myModule;
        *destPort = dest;
    }

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      _bcm_petra_is_mirror_dest_created
 * Purpose:
 *      Check if given mirror destination is created (allocated by the allocation manager)
 * Parameters:
 *      unit                   (IN) Deivce id
 *      mirr_action_profile_id (IN) mirror destination/profile ID.
 *      is_created            (OUT) is destination created (allocated)
 */
int
 _bcm_petra_is_mirror_dest_created(int unit, uint32 mirr_action_profile_id, int *is_created)
{
    uint32 ref_count;
    /* Check if this mirror destination exists */
    int res;
    BCMDNX_INIT_FUNC_DEFS;

    res = _bcm_dpp_am_template_mirror_action_profile_ref_get(unit, mirr_action_profile_id, &ref_count); /* easy - get the ref count*/
    BCMDNX_IF_ERR_EXIT(res);
    *is_created= ref_count>0;

exit:
    BCMDNX_FUNC_RETURN;
}


/******************************************************************************
 *
 *  Exposed helper functions (see function description comments in .h file)
 */

int
bcm_petra_ingress_mirror_alloc(int unit,
                               bcm_module_t myModule,
                               bcm_module_t destMod,
                               bcm_port_t destPort,
                               int *mirrorId)
{
   int result;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PETRA_MIRROR_LOCK_TAKE(unit);
    result = _bcm_petra_ingress_mirror_alloc(unit,
                                             myModule,
                                             destMod,
                                             destPort,
                                             mirrorId);
    SOC_PETRA_MIRROR_LOCK_GIVE(unit);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ingress_mirror_reference(int unit,
                                   int mirrorId)
{
    bcm_error_t result;
    unsigned int ingressCount;

    BCMDNX_INIT_FUNC_DEFS;
    SOC_PETRA_MIRROR_LOCK_TAKE(unit);
    result = MIRROR_ACCESS.ingressCount.get(unit, &ingressCount);
    if(BCM_FAILURE(result)) {
        SOC_PETRA_MIRROR_LOCK_GIVE(unit);
        BCM_RETURN_VAL_EXIT(result);
    }
    if ((0 > mirrorId) || (ingressCount <= mirrorId)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror %d is not valid\n"),
                   unit,
                   mirrorId));
        result = BCM_E_NOT_FOUND;
    } else {
        result = _bcm_petra_ingress_mirror_reference(unit,
                                                     mirrorId);
    }
    SOC_PETRA_MIRROR_LOCK_GIVE(unit);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ingress_mirror_free(int unit,
                              int mirrorId)
{
    unsigned int ingressCount;
    bcm_error_t result = BCM_E_NONE; BCMDNX_INIT_FUNC_DEFS;
                             /* ah, optimism */

    SOC_PETRA_MIRROR_LOCK_TAKE(unit);
    result = MIRROR_ACCESS.ingressCount.get(unit, &ingressCount);
    if(BCM_FAILURE(result)) {
        SOC_PETRA_MIRROR_LOCK_GIVE(unit);
        BCM_RETURN_VAL_EXIT(result);
    }
    if ((0 > mirrorId) || (ingressCount <= mirrorId)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror %d is not valid\n"),
                   unit,
                   mirrorId));
        result = BCM_E_NOT_FOUND;
    } else {
        result = _bcm_petra_ingress_mirror_free(unit,
                                                mirrorId);
    }
    SOC_PETRA_MIRROR_LOCK_GIVE(unit);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ingress_mirror_change(int unit,
                                int oldMirrorId,
                                bcm_module_t myModule,
                                bcm_module_t destMod,
                                bcm_port_t destPort,
                                int *newMirrorId)
{
    unsigned int ingressCount;
    bcm_error_t result = BCM_E_NONE; BCMDNX_INIT_FUNC_DEFS;
                             /* ah, optimism */

    SOC_PETRA_MIRROR_LOCK_TAKE(unit);
    result = MIRROR_ACCESS.ingressCount.get(unit, &ingressCount);
    if(BCM_FAILURE(result)) {
        SOC_PETRA_MIRROR_LOCK_GIVE(unit);
        BCM_RETURN_VAL_EXIT(result);
    }
    if ((0 > oldMirrorId) || (ingressCount <= oldMirrorId)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror %d is not valid\n"),
                   unit,
                   oldMirrorId));
        result = BCM_E_NOT_FOUND;
    } else {
        result = _bcm_petra_ingress_mirror_change(unit,
                                                  oldMirrorId,
                                                  myModule,
                                                  destMod,
                                                  destPort,
                                                  newMirrorId);
    }
    SOC_PETRA_MIRROR_LOCK_GIVE(unit);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_ingress_mirror_get(int unit,
                             int mirrorId,
                             bcm_module_t *destMod,
                             bcm_port_t *destPort)
{
    unsigned int ingressCount;
    bcm_error_t result = BCM_E_NONE; BCMDNX_INIT_FUNC_DEFS;
                             /* ah, optimism */

    SOC_PETRA_MIRROR_LOCK_TAKE(unit);
    result = MIRROR_ACCESS.ingressCount.get(unit, &ingressCount);
    if(BCM_FAILURE(result)) {
        SOC_PETRA_MIRROR_LOCK_GIVE(unit);
        BCM_RETURN_VAL_EXIT(result);
    }
    if ((1 > mirrorId) || (ingressCount <= mirrorId)) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "unit %d ingress mirror %d is not valid\n"),
                   unit,
                   mirrorId));
        result = BCM_E_NOT_FOUND;
    } else {
        result = _bcm_petra_ingress_mirror_get(unit,
                                               mirrorId,
                                               destMod,
                                               destPort);
    }
    SOC_PETRA_MIRROR_LOCK_GIVE(unit);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}


/******************************************************************************
 *
 *  API functions
 */

/*
 *  Function
 *     bcm_petra_mirror_init
 *  Purpose
 *     Initialise the mirror APIs.
 *  Parameters
 *     (in) int unit = the unit number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_init(int unit)
{
    bcm_error_t result = BCM_E_NONE;
    ARAD_ACTION_CMD_SNOOP_MIRROR_INFO mirror_info;
    uint8 is_allocated;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "(%d) enter\n"), unit));
    BCM_DPP_UNIT_CHECK(unit);

    sw_state_sync_db[unit].dpp.mirror_lock = sal_mutex_create("soc_petra_mirror_unit_lock");
    if (!sw_state_sync_db[unit].dpp.mirror_lock) {
        LOG_ERROR(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "could not create unit %d"
                              " mirror lock\n"),
                   unit));
        BCM_RETURN_VAL_EXIT(BCM_E_RESOURCE);
    }

    if (SOC_WARM_BOOT(unit))
    {
        BCM_EXIT;
    }

    /* If the unit is already initialised, tear it down */
    result = MIRROR_ACCESS.is_allocated(unit, &is_allocated);
    if(BCM_FAILURE(result)) {
        goto fail;
    }
    if(!is_allocated) {
        result = MIRROR_ACCESS.alloc(unit);
        if(BCM_FAILURE(result)) {
            goto fail;
        }
    }

    result = MIRROR_ACCESS.ingress.is_allocated(unit, &is_allocated);
    if(BCM_FAILURE(result)) {
        goto fail;
    }
    if(!is_allocated) {
        result = MIRROR_ACCESS.ingress.alloc(unit, SOC_PETRA_IGR_MIRRORS(unit));
        if(BCM_FAILURE(result)) {
            goto fail;
        }
    }
    result = MIRROR_ACCESS.ingressCount.set(unit, SOC_PETRA_IGR_MIRRORS(unit));
    if(BCM_FAILURE(result)) {
        goto fail;
    }
    /* reserve one action for not mirroring */
    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&mirror_info);
    mirror_info.prob = 0;
    result = MIRROR_ACCESS.ingress.mirrorInfo.set(unit, 0, &mirror_info);
    if(BCM_FAILURE(result)) {
        goto fail;
    }
    result = MIRROR_ACCESS.ingress.refCount.set(unit, 0, 2);
    if(BCM_FAILURE(result)) {
        goto fail;
    }

    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "commit unit %d init\n"), unit));
    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "(%d) return %d (%s)\n"),
               unit,
               result,
               _SHR_ERRMSG(result)));
    BCM_EXIT;

fail:
    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "back out unit %d init\n"), unit));
    BCMDNX_IF_ERR_EXIT(MIRROR_ACCESS.free(unit));
    if (sw_state_sync_db[unit].dpp.mirror_lock) {
        LOG_DEBUG(BSL_LS_BCM_MIRROR,
                  (BSL_META_U(unit,
                              "destroy lock\n")));
        sal_mutex_destroy(sw_state_sync_db[unit].dpp.mirror_lock);
    }
    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "free working unit descriptor\n")));

    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "(%d) return %d (%s)\n"),
               unit,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_detach
 *  Purpose
 *     Initialise the mirror APIs.
 *  Parameters
 *     (in) int unit = the unit number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_detach(int unit)
{
    int result;

    BCMDNX_INIT_FUNC_DEFS;
    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "(%d) enter\n"), unit));
    BCM_DPP_UNIT_CHECK(unit);

    result = _bcm_petra_mirror_detach(unit);


    LOG_DEBUG(BSL_LS_BCM_MIRROR,
              (BSL_META_U(unit,
                          "(%d) return %d (%s)\n"),
               unit,
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_set
 *  Purpose
 *     Set mirroring mode and destination for a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) bcm_module_t dest_mod = destination module for mirror frames
 *     (in) bcm_port_t dest_port = destination port for mirror frames
 *     (in) uint32 flags = mirror control flags
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Ingress mirroring sets the default mirror profile for both tagged and
 *     untagged frames on the port, to the same value, so only one of these
 *     values is used when deciding (for example) whether mirroring is active
 *     on the specified port.
 */
int
bcm_petra_mirror_port_set(int unit,
                          bcm_port_t port,
                          bcm_module_t dest_mod,
                          bcm_port_t dest_port,
                          uint32 flags)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_get
 *  Purpose
 *     Get mirroring mode and destination for a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (out) bcm_module_t *dest_mod = where to put destination module
 *     (out) bcm_port_t *dest_port = where to put destination port
 *     (out) uint32 *flags = where to put mirror control flags
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Ingress mirroring sets the default mirror profile for both tagged and
 *     untagged frames on the port, to the same value, so only one of these
 *     values is used when deciding (for example) whether mirroring is active
 *     on the specified port.
 */
int
bcm_petra_mirror_port_get(int unit,
                          bcm_port_t port,
                          bcm_module_t *dest_mod,
                          bcm_port_t *dest_port,
                          uint32 *flags)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}


/**
*
 *
 * @param unit
 * @param global_outlif
 * @param alloc_flags
 * @param local_outlif
 */
int
bcm_petra_mirror_destination_erspan_lif_alloc(int unit, uint32 alloc_flags, int *global_outlif, int *local_outlif) {

    int rv;
    bcm_dpp_am_local_out_lif_info_t local_outlif_info;
    BCMDNX_INIT_FUNC_DEFS;


    if (SOC_IS_JERICHO(unit)) {

        /* Set the local lif info */
        sal_memset(&local_outlif_info, 0, sizeof(local_outlif_info));
        local_outlif_info.app_alloc_info.pool_id =  dpp_am_res_eg_data_erspan;
        local_outlif_info.app_alloc_info.application_type = 0;
        local_outlif_info.counting_profile_id = BCM_DPP_AM_COUNTING_PROFILE_NONE;

        /* Allocate the global and local lif */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_alloc_global_and_local_lif(unit, alloc_flags, global_outlif, NULL, &local_outlif_info));

        /* Retrieve the allocated local lif */
        *local_outlif = local_outlif_info.base_lif_id;

    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {

        *local_outlif = *global_outlif; /* If the WITH_ID flag is set, the lif value will be in the global lif */
        rv = bcm_dpp_am_eg_data_erspan_alloc(unit, alloc_flags, (SOC_PPC_LIF_ID*)local_outlif);
        BCMDNX_IF_ERR_EXIT(rv);

        *global_outlif = *local_outlif; /* If the WITH_ID flag was not set, the local_outlif will now hold the newly allocated lif */
    }
exit:
    BCMDNX_FUNC_RETURN;
}




/*
 *  Function
 *     bcm_petra_mirror_destination_tunnel_create
 *  Purpose
 *     In case mirror destination is over ip tunnel with/out ERSPAN.
 *     build ERSPAN tunnel if needed, and make mirror profile points to erspan outlif
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_mirror_destination_t *mirror_dest: mirror destination as given by BCM call
 *     (in) uint32  action_profile_id: mirror profile used
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_destination_tunnel_create(int unit,
                                    bcm_mirror_destination_t *mirror_dest, uint32  action_profile_id,int *global_outlif, uint8 *outlif_valid, uint8 update)
{
    uint32 soc_sand_rv;
    uint32 alloc_flags=0;
    int rv = BCM_E_NONE;
    uint32 ip_tunnel_id;    /* Local lif of the ip tunnel */
    int ip_tunnel_id_tmp;    /* Local lif of the ip tunnel */
    SOC_PPC_EG_ENCAP_DATA_INFO
        data_encap_entry;
    SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO
        mirror_encap_info;
    uint32 next_eep, nof_entries;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    int mc_support;
    int local_outlif = 0, tmp_global_outlif = 0;

    int soc_sand_dev_id;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    *global_outlif = 0;
    *outlif_valid = 0;

    /* tunnel build */
    soc_sand_dev_id = (unit);

    /* encap_id to point to egres LIF: can be supplied without setting IP_GRE or RSPAN, which already build using ip tunnel API */
    if(mirror_dest->encap_id != 0) {
        *global_outlif = mirror_dest->encap_id;
        alloc_flags = BCM_DPP_AM_FLAG_ALLOC_WITH_ID;
        if (update) {
            alloc_flags |= BCM_DPP_AM_FLAG_ALLOC_REPLACE;
        }
        *outlif_valid = 1;
    }

    BCMDNX_ALLOC(encap_entry_info, sizeof(*encap_entry_info) * SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
        "bcm_petra_mirror_destination_tunnel_create.encap_entry_info");
    if(encap_entry_info == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Memory allocation failue\n")));
    }

    /* check if destination include IP tunnel encapsulation */
    if(mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_IP_GRE) {
        bcm_tunnel_type_t tunnel_type;
        _bcm_tunnel_subtype_t tunnel_subtype = _bcm_tunnel_subtype_none;
        *outlif_valid = 1;
        if (!BCM_L3_ITF_TYPE_IS_LIF(mirror_dest->tunnel_id)) {/* IP tunnel id  */
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_TUNNEL_IP_GRE flag is set then tunnel_id has to point to ip-tunnel")));
        }

        /* The encap id in the tunnel encapsulation is the global lif of the ip tunnel. Translate it to local lif */
        tmp_global_outlif = BCM_L3_ITF_VAL_GET(mirror_dest->tunnel_id);

        rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, tmp_global_outlif, &ip_tunnel_id_tmp);
        BCMDNX_IF_ERR_EXIT(rv);
        ip_tunnel_id = ip_tunnel_id_tmp;


        /* update IP tunnel to include gre of 8 bits type */
        /* get entry */
        soc_sand_rv =
          soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                 SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                 ip_tunnel_id, 0,
                                 encap_entry_info, &next_eep,
                                 &nof_entries);
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

        /* check that it's really IP tunnel */
        if (encap_entry_info[0].entry_type != SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_TUNNEL_IP_GRE flag is and pointed encap is not an IP tunnel ")));
        }

        /*
         * ERSPAN multicast: IPV4_tunnel.VSI = encap_id
         */
        if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
            mc_support = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "custom_feature_erspan_mc_support ", 0) ;
        } else {
            mc_support = 0;
        }

        /* update ip tunnel to include enhanced GRE */
        /* get current tunnel type */
        rv = _bcm_petra_tunnel_ipv4_type_from_ppd(unit, &(encap_entry_info[0].entry_val.ipv4_encap_info), &tunnel_type, &tunnel_subtype);
        BCMDNX_IF_ERR_EXIT(rv);

        if ((SOC_IS_JERICHO_B0_AND_ABOVE(unit) && ((tunnel_type != bcmTunnelTypeL2Gre) || (tunnel_subtype != _bcm_tunnel_subtype_l2gre_is_erspan)))
            || (SOC_IS_JERICHO_AND_BELOW(unit) && (tunnel_type != bcmTunnelTypeL2Gre))
            || (mc_support && encap_entry_info[0].entry_val.ipv4_encap_info.out_vsi != mirror_dest->encap_id))
        {
            /* fix type to be enhanced */
            rv = _bcm_petra_tunnel_ipv4_type_to_ppd(
               unit,
               bcmTunnelTypeL2Gre,
               _bcm_tunnel_subtype_l2gre_is_erspan, /* indicate that the GRE tunnel is an ERSPAN tunnel (ERSPANoGREoIP) */
               &(encap_entry_info[0].entry_val.ipv4_encap_info),
               TRUE);
            BCMDNX_IF_ERR_EXIT(rv);

            if (mc_support)
            {
                encap_entry_info[0].entry_val.ipv4_encap_info.out_vsi = mirror_dest->encap_id;
            }

            soc_sand_rv =
              soc_ppd_eg_encap_ipv4_entry_add(soc_sand_dev_id,
                                     ip_tunnel_id,
                                     &(encap_entry_info[0].entry_val.ipv4_encap_info),
                                     next_eep);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        }


        /* check if need to add ERSPAN encapsulation */
        if(mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID) {/*span_id*/

            if (SOC_IS_ARADPLUS_AND_BELOW(unit))
            {
                if (0 == (mirror_dest->flags & (BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID is set but BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID is not set")));
                }

                if (0 == (mirror_dest->flags & (BCM_MIRROR_DEST_TUNNEL_L2))) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID is set but SBCM_MIRROR_DEST_TUNNEL_L2 is not set")));
                }

                if (DPP_MIRROR_ACTION_NDX_MAX < mirror_dest->encap_id ||
                    0 > mirror_dest->encap_id) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id is out of range range is %d - %d"), 0, DPP_MIRROR_ACTION_NDX_MAX));
                }

                if(mirror_dest->encap_id != (ip_tunnel_id & 0xf)) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("encap_id is different from tunnel_id[0:3]. encap_id = %d, tunnel_id[0:3] = %d "),
                                                   mirror_dest->encap_id, (ip_tunnel_id & 0xf)));
                }


                SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO_clear(&mirror_encap_info);

                /* L2 source mac address. */
                rv = _bcm_petra_mac_to_sand_mac(mirror_dest->dst_mac, &(mirror_encap_info.tunnel.dest));
                BCMDNX_IF_ERR_EXIT(rv);

                /* L2 destination mac address. */
                rv = _bcm_petra_mac_to_sand_mac(mirror_dest->src_mac, &(mirror_encap_info.tunnel.src));
                BCMDNX_IF_ERR_EXIT(rv);

                mirror_encap_info.tunnel.tpid       = mirror_dest->tpid;
                mirror_encap_info.tunnel.vid        = mirror_dest->vlan_id;
                mirror_encap_info.tunnel.pcp        = mirror_dest->pkt_prio;
                mirror_encap_info.tunnel.ether_type = 0x0800; /* ipv4 ethertype */
                mirror_encap_info.tunnel.erspan_id  = mirror_dest->span_id;
                mirror_encap_info.tunnel.encap_id   = mirror_dest->encap_id;


                /* add enry to allocated place */
                soc_sand_rv = soc_ppd_eg_encap_mirror_entry_set(soc_sand_dev_id, mirror_dest->encap_id, &mirror_encap_info);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                *global_outlif       = ip_tunnel_id;
                *outlif_valid = 1;

            }
            else if (SOC_IS_JERICHO(unit))/* Jericho and above*/
            {
                /* Can't replace non - existing ERSPAN entry */
                if (update) {

                    rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, *global_outlif, &local_outlif);
                    BCMDNX_IF_ERR_EXIT(rv);

                    if (SOC_IS_JERICHO(unit)) {
                        rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, *global_outlif);
                    } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                        rv = bcm_dpp_am_eg_data_erspan_is_alloced(unit,local_outlif);
                    }
                    if (rv != BCM_E_EXISTS) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ERSPAN id %d to be replaced does not exist"),*global_outlif));
                    }


                    /* get entry */
                    soc_sand_rv =
                      soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                             SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                             local_outlif, 0,
                                             encap_entry_info, &next_eep,
                                             &nof_entries);
                    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

                } else {
                    /* allocate entry to store the ERSPAN entry */
                    rv = bcm_petra_mirror_destination_erspan_lif_alloc(unit, alloc_flags, global_outlif, &local_outlif);
                    BCMDNX_IF_ERR_EXIT(rv);
                }

                /* build data entry and fill with ERSPAN info */
                SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_encap_entry);
                SOC_PPC_EG_ENCAP_EEDB_DATA_ERSPAN_FORMAT_SET(soc_sand_dev_id, SOC_PPC_EG_ENCAP_ERSPAN_PRIO_VAL, SOC_PPC_EG_ENCAP_ERSPAN_TRUNC_VAL,
                                                             mirror_dest->span_id, &data_encap_entry);

                if (update) {
                    data_encap_entry.oam_lif_set = encap_entry_info[0].entry_val.data_info.oam_lif_set;
                    data_encap_entry.drop = encap_entry_info[0].entry_val.data_info.drop;
                }

                /* add enry to allocated place */
                soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id,local_outlif,&data_encap_entry,TRUE,ip_tunnel_id);
                BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
            }

        }
    }
    else if(mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_RSPAN){ /* RSPAN */
        *outlif_valid = 1;

        /* Can't replace non - existing ERSPAN entry */
        if (update) {

            rv = _bcm_dpp_global_lif_mapping_global_to_local_get(unit, _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, *global_outlif, &local_outlif);
            BCMDNX_IF_ERR_EXIT(rv);

            if (SOC_IS_JERICHO(unit)) {
                rv = bcm_dpp_am_global_lif_is_alloced(unit, BCM_DPP_AM_FLAG_ALLOC_EGRESS, *global_outlif);
            } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
                rv = bcm_dpp_am_eg_data_erspan_is_alloced(unit,local_outlif);
            }
            if (rv != BCM_E_EXISTS) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("ERSPAN id %d to be replaced does not exist"),*global_outlif));
            }

            /* get entry */
            soc_sand_rv =
              soc_ppd_eg_encap_entry_get(soc_sand_dev_id,
                                     SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                     local_outlif, 0,
                                     encap_entry_info, &next_eep,
                                     &nof_entries);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
        } else {
            /* allocate entry to store the ERSPAN entry */
            rv = bcm_petra_mirror_destination_erspan_lif_alloc(unit, alloc_flags, global_outlif, &local_outlif);
            BCMDNX_IF_ERR_EXIT(rv);
        }

        /* build data entry and fill with RSPAN info */
        SOC_PPC_EG_ENCAP_DATA_INFO_clear(&data_encap_entry);
        SOC_PPD_EG_ENCAP_DATA_RSPAN_FORMAT_SET(soc_sand_dev_id,mirror_dest->meta_data, mirror_dest->tpid,mirror_dest->pkt_prio,mirror_dest->vlan_id,&data_encap_entry);

        if (update) {
            data_encap_entry.oam_lif_set = encap_entry_info[0].entry_val.data_info.oam_lif_set;
            data_encap_entry.drop = encap_entry_info[0].entry_val.data_info.drop;
        }

        /* add enry to allocated place including vlan-tag */
        soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(soc_sand_dev_id, local_outlif, &data_encap_entry, FALSE, 0); /* FIX need to point to RIF entry to update VSI for processing */
        BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
    }

    /* if out-lif not valid then reset outlif value */
    if(*outlif_valid == 0) {
        *global_outlif = 0;
    }
    else{
        mirror_dest->encap_id = *global_outlif;
    }

exit:
    BCM_FREE(encap_entry_info);
    BCMDNX_FUNC_RETURN;
}


/*
 *  Function
 *     bcm_petra_mirror_destination_create
 *  Purpose
 *     Create mirror destination description.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in/out) bcm_mirror_destination_t *mirror_dest = dest description
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Creates a mirror destination for use with bcm_mirror_port{_vlan}_dest_*
 *     The returned destination is specific to the unit and can not be used with other units.
 */
int
bcm_petra_mirror_destination_create(int unit,
                                    bcm_mirror_destination_t *mirror_dest)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(_bcm_petra_mirror_or_snoop_destination_create(unit, mirror_dest, TRUE));

exit:
    BCMDNX_FUNC_RETURN;
}

/*creates mirror or snoop destination profile and sets destination only if set_dest is required*/
int
_bcm_petra_mirror_or_snoop_destination_create(int unit,
                                    bcm_mirror_destination_t *mirror_dest, uint8 set_dest)
{
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO info;
    uint32 action_profile_id = 0;
    uint32 soc_sand_rv;
    uint32 flags; /* hold flags for dpp function */
    int do_alloc = 1, is_created, failed = 1;
    int allocation_helper = 0;
    unsigned int eproc;
    unsigned int ecntr;
    unsigned int command_id;
    SOC_TMC_CNT_SRC_TYPE src_type;
    SOC_TMC_CMD_TYPE cmnd_type;
    SOC_TMC_CNT_MODE_EG_TYPE mode;
    unsigned int range_start, range_end;
    MIRROR_LOCK_DEFS;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(mirror_dest);

    if (( mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_COUNTER) && ((mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_COUNTER_1) ||
                                                                   (mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_COUNTER_2))) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
      (_BSL_BCM_MSG("BCM_MIRROR_DEST_UPDATE_COUNTER can not be in use with BCM_MIRROR_DEST_UPDATE_COUNTER_1 or BCM_MIRROR_DEST_UPDATE_COUNTER_2")));
    }

    if (( mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_EXT_COUNTERS) && !SOC_IS_QAX(unit)) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
      (_BSL_BCM_MSG("BCM_MIRROR_DEST_UPDATE_EXT_COUNTERS is supported only for QAX/QUX")));
    }

    /* set input data for mbcm_dpp_action_cmd_mirror_set and parse input flags */
    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&info);
    info.prob = _bcm_petra_mirror_or_snoop_probability_get(mirror_dest->sample_rate_dividend,
                                                           mirror_dest->sample_rate_divisor,
                                                           SOC_DPP_IMP_DEFS_GET(unit,mirror_probability_bits));

    /* Translate the input gport to a SOC_TMC_DEST_INFO only if set_dest is required. (supports old bcm_petra_rx_snoop_set which allows setting without dest setting) */
    if (set_dest) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_to_tm_dest_info(unit, mirror_dest->gport, &info.cmd.dest_id));
    }

    if ((flags = mirror_dest->flags & ~(BCM_MIRROR_DEST_WITH_ID | BCM_MIRROR_DEST_REPLACE | BCM_MIRROR_DEST_DEST_MULTICAST | BCM_MIRROR_DEST_UPDATE_EXT_COUNTERS |
                                        BCM_MIRROR_DEST_UPDATE_COUNTER | BCM_MIRROR_DEST_UPDATE_COUNTER_1 |BCM_MIRROR_DEST_UPDATE_COUNTER_2 |
                                        BCM_MIRROR_DEST_TUNNEL_IP_GRE | BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID | BCM_MIRROR_DEST_IS_SNOOP |
                                        BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID | BCM_MIRROR_DEST_TUNNEL_RSPAN | BCM_MIRROR_DEST_TUNNEL_L2 |
                                        BCM_MIRROR_DEST_OUT_MIRROR_DISABLE | BCM_MIRROR_DEST_EEI_INVALID
                                        | (SOC_IS_JERICHO(unit) ? BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER | BCM_MIRROR_DEST_EGRESS_TRAP_WITH_SYSTEM_HEADER : 0)))) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror destination flag was specified : 0x%lx"), (unsigned long)flags));
    }
    /* not supporting these flags: BCM_MIRROR_DEST_TUNNEL_L2 BCM_MIRROR_DEST_PAYLOAD_UNTAGGED BCM_MIRROR_DEST_TUNNEL_TRILL BCM_MIRROR_DEST_TUNNEL_NIV */

    if (!SOC_IS_JERICHO(unit) && mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Snooping is only supported on Jericho")));
    } else if ((mirror_dest->flags & (BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER | BCM_MIRROR_DEST_EGRESS_TRAP_WITH_SYSTEM_HEADER))
      == (BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER | BCM_MIRROR_DEST_EGRESS_TRAP_WITH_SYSTEM_HEADER)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER and "
          "BCM_MIRROR_DEST_EGRESS_TRAP_WITH_SYSTEM_HEADER flags are not supported together.")));
    }

    /*  Override up meter pointer, if enabled. Value Range: 0 - 8K-1. Petra-B only since Meter block is after mirroring in Arad */
    if (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_RSPAN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("RSPAN flag unavailable through mirror_destination_create(). For RSPAN, create the tunnel with bcm_tunnel_initiator_create()  and create mirror with "
                                                       "encap_id set to the tunnel_id returned from above API, using BCM_GPORT_TUNNEL_ID_GET(). See mirror_with_rspan_example() in cint_mirror_erspan.c for further detail.")));
    }

    if (mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_COUNTER) {
        ecntr = BCM_FIELD_STAT_ID_COUNTER_GET(mirror_dest->stat_id);
        eproc = BCM_FIELD_STAT_ID_PROCESSOR_GET(mirror_dest->stat_id);

        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_source, eproc, &src_type));
        if (src_type != SOC_TMC_CNT_SRC_TYPE_ING_PP) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Counter engine %d, is not configured to count from the IHB."), eproc));
        }

        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_mode, eproc, &mode));
        if (mode != SOC_TMC_CNT_MODE_EG_TYPE_PMF)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Counter engine %d, is not configured with PMF mode ."), eproc));
        }

        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_range_start, eproc, &range_start));
        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_range_end, eproc, &range_end));
        if ((ecntr > range_end) || (ecntr < range_start))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Counter engine %d, ecntr=0x%X is out of range. range is [0x%X : 0x%X] ."), eproc, ecntr, range_start, range_end));
        }

        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_command_id, eproc, &command_id));
        if (command_id == 1) { /* group 1 */
            info.cmd.counter_ptr_2.value = ecntr;
            info.cmd.counter_ptr_2.enable = TRUE;
        } else { /* group 0 */
            info.cmd.counter_ptr_1.value = ecntr;
            info.cmd.counter_ptr_1.enable = TRUE;
        }
    }

    if (mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_COUNTER_1)
    {
            info.cmd.counter_ptr_1.enable = TRUE; /* set upper bank pointer */
            /* Indication that counting is disabled */
            if (mirror_dest->stat_id == BCM_MIRROR_DEST_NO_COUNT) {
                info.cmd.counter_ptr_1.value = SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER;
            } else
            {
                info.cmd.counter_ptr_1.value = mirror_dest->stat_id;
            }
    }

    if (mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_COUNTER_2)
    {
        info.cmd.counter_ptr_2.enable = TRUE; /* set lower bank pointer */
        /* Indication that counting is disabled */
        if (mirror_dest->stat_id2 == BCM_MIRROR_DEST_NO_COUNT) {
             info.cmd.counter_ptr_2.value = SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER;
        } else if(mirror_dest->stat_id2 == -1)
        {
             info.cmd.counter_ptr_2.value = mirror_dest->stat_id;
        }
        else
        {
            info.cmd.counter_ptr_2.value = mirror_dest->stat_id2;
        }
    }

    if (SOC_IS_QAX(unit))
    {
        if (( mirror_dest->flags & BCM_MIRROR_DEST_UPDATE_EXT_COUNTERS))
        {
            info.ext_stat_id_update=TRUE;
            if (((mirror_dest->ext_stat_id[0] == BCM_MIRROR_DEST_NO_COUNT) || (mirror_dest->ext_stat_id[1] == BCM_MIRROR_DEST_NO_COUNT)) &&
                 (mirror_dest->ext_stat_id[0] != mirror_dest->ext_stat_id[1]))
            {
            /* 
             * When using BCM_MIRROR_DEST_UPDATE_EXT_COUNTERS to disable statistic interface for mirror packets,
             * both stat_id and stat_id2 need to be set to BCM_MIRROR_DEST_NO_COUNT, because the STATISTICS_PTR_OW field
             * is common for both statistics interfaces
             */
               BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
              (_BSL_BCM_MSG("When using BCM_MIRROR_DEST_UPDATE_EXT_COUNTERS to disable statistic interface for mirror packets,"
                      "both stat_id and stat_id2 need to be set to BCM_MIRROR_DEST_NO_COUNT")));
            }
            else 
            {
                 info.ext_stat_id[0]=mirror_dest->ext_stat_id[0];
                 info.ext_stat_id[1]=mirror_dest->ext_stat_id[1];
            }
        }
    }

    flags = 0; /* flags will now hold the input flags for mbcm_dpp_action_cmd_mirror_set */
    MIRROR_LOCK_TAKE;

    if (mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID) { /* is the destination id (mirror action profile id) specified by the user? */
        /* verify that the given mirror destination is legal */
        if (!BCM_GPORT_IS_MIRROR(mirror_dest->mirror_dest_id)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
        }
        /*mirror command type*/
        if (!(mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP)) {
            if (!BCM_GPORT_IS_MIRROR_MIRROR(mirror_dest->mirror_dest_id)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
            }
        } else {
            if (!BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest->mirror_dest_id)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified snoop destination has incorrect gport type")));
            }
        }
        action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id);
        if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified destination ID is out of range")));
        }

        /*check if destination already exists*/
        if (!(mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP)) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        } else {
            is_created = (_bcm_dpp_am_snoop_is_alloced(unit, action_profile_id) == BCM_E_EXISTS ? TRUE : FALSE);
        }

        /* replace an existing destination */
        if (mirror_dest->flags & BCM_MIRROR_DEST_REPLACE) {
            /* verify that the destination already exists */
            if (!is_created) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified destination ID %u is not in use"), (unsigned)action_profile_id));
            }
                do_alloc = 0;
        } else { /* create a new destination */
                if (is_created) { /* verify that the destination does not already exists */
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified destination ID %u is already in use"), (unsigned)action_profile_id));
                }
        }
        flags = BCM_DPP_AM_TEMPLATE_FLAG_ALLOC_WITH_ID;
    } else { /* the destination was not specified, need to allocate a free destination*/
        action_profile_id = 0;
        if (mirror_dest->flags & BCM_MIRROR_DEST_REPLACE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_DEST_REPLACE must be used with BCM_MIRROR_DEST_WITH_ID")));
        }
    }

    /* allocate mirror profile */
    if (do_alloc) {
        /*mirror allocation*/
        if (!(mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP)) {
            if (_bcm_dpp_am_template_mirror_action_profile_alloc(unit, flags | SHR_TEMPLATE_MANAGE_IGNORE_DATA, NULL, NULL, (int *)&action_profile_id) != BCM_E_NONE) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("Failed to allocate mirroring destination")));
            }
            if (flags ?
              action_profile_id != BCM_GPORT_MIRROR_GET(mirror_dest->mirror_dest_id) :
              action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("internal mirror profile allocation error")));
            }
        }
        else{
            /*snoop allocation*/
            flags = 0; /*flags will hold needed data for _bcm_dpp_am_snoop_alloc*/
            flags |= mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID ? BCM_RX_SNOOP_WITH_ID : 0;
            flags |= mirror_dest->flags & BCM_MIRROR_DEST_REPLACE ? BCM_RX_SNOOP_REPLACE : 0;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_am_snoop_alloc(unit,flags,action_profile_id, &allocation_helper));
            action_profile_id = allocation_helper;
        }

        if (flags) {
            do_alloc = 9;
        }
    }
    /* allocate tunnel if needed, return out lif */
    BCMDNX_IF_ERR_EXIT( bcm_petra_mirror_destination_tunnel_create(
      unit, mirror_dest, action_profile_id, (int*)&info.cmd.outlif.value, &info.cmd.outlif.enable,0));

    if (info.cmd.outlif.value > ARAD_MIRROR_CMD_OUTLIF_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Out lif value is invalid")));
    }

    if (SOC_IS_QAX(unit)) {
        switch (mirror_dest->packet_copy_size) {
            case 256:
                info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_256;
                break;
            case 0:
                info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
                break;
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("cropping size is invalid")));
        }
    } else {
        switch (mirror_dest->packet_copy_size) {
            case 64:
                info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_64;
                break;
            case 128:
                info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_128;
                break;
            case 192:
                info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_192;
                break;
            case 0:
                info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;
                break;
            default:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("cropping size is invalid")));
            }
    }

    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_PRIO) {
        info.cmd.tc.value = mirror_dest->packet_control_updates.prio;
    }
    info.cmd.tc.enable = SOC_SAND_NUM2BOOL(mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_PRIO);

    if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_COLOR) {
        info.cmd.dp.value = mirror_dest->packet_control_updates.color;
    }
    info.cmd.dp.enable = SOC_SAND_NUM2BOOL(mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_COLOR);


    if (info.cmd.dest_id.type == SOC_TMC_DEST_TYPE_MULTICAST) {
        /* mark prm_profile_info.cmd.is_ing_mc true if prm_profile_info.cmd.dest_id.id is an ingress MC group */
        BCMDNX_IF_ERR_EXIT(_bcm_petra_multicast_ingress_group_exists(unit, info.cmd.dest_id.id, &info.cmd.is_ing_mc));
    }

    /*Jericho new features*/
    if (SOC_IS_JERICHO(unit)) {
        if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_IN_PORT) {
            info.valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_IN_PORT;
            info.in_port = mirror_dest->packet_control_updates.in_port;
        }
        if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_VSQ) {
            info.valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_VSQ;
            info.vsq = mirror_dest->packet_control_updates.vsq;
        }
        if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_ECN_CNM_CANCEL) {
            info.valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_CNM_CANCEL;
            info.cnm_cancel = mirror_dest->packet_control_updates.cnm_cancel;
        }
        if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_TRUNK_HASH_RESULT) {
            info.valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_TRUNK_HASH_RESULT;
            info.trunk_hash_result = mirror_dest->packet_control_updates.trunk_hash_result;
        }
        if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_ECN_VALUE) {
            info.valid |= SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_ECN_VALUE;
            info.ecn_value = mirror_dest->packet_control_updates.ecn_value;
        }
        /*calculate outbound probability on Jericho*/
        info.outbound_prob = _bcm_petra_mirror_or_snoop_probability_get(mirror_dest->egress_sample_rate_dividend,
                                                                        mirror_dest->egress_sample_rate_divisor,
                                                                        JER_OUTBOUND_MIRROR_SNOOP_PROBABILITY_BITS);
        switch (mirror_dest->egress_packet_copy_size) {
        /*0 - no cropping, 1 - crop to maximum first $JER_OUTBOUND_MIRROR_CROP_SIZE bytes*/
        case 0:
            info.crop_pkt = 0;
            break;
        case JER_OUTBOUND_MIRROR_CROP_SIZE:
            info.crop_pkt = 1;
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("egress_packet_crop_size is invalid")));
        }

        info.add_orig_head = (mirror_dest->flags & BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER) ? 1 :
          ((mirror_dest->flags & BCM_MIRROR_DEST_EGRESS_TRAP_WITH_SYSTEM_HEADER) ? 2 : 0);
        if (mirror_dest->packet_control_updates.valid & BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING) {
            if (mirror_dest->packet_control_updates.header_fabric.internal_valid) {
                info.stamping.cpu_trap_code = mirror_dest->packet_control_updates.header_fabric.internal.trap_id;
                info.stamping.cpu_trap_qualifier = mirror_dest->packet_control_updates.header_fabric.internal.trap_qualifier;
                info.stamping.valid = SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_STAMPING_FHEI;
            }

            if (!(mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP) &&
                (mirror_dest->packet_control_updates.header_fabric.ftmh_valid == TRUE) &&
                ((mirror_dest->packet_control_updates.header_fabric.ftmh.action_type == bcmPktDnxFtmhActionTypeInboundMirror) ||
                 (mirror_dest->packet_control_updates.header_fabric.ftmh.action_type == bcmPktDnxFtmhActionTypeOutboundMirror))) {
                info.action_type = mirror_dest->packet_control_updates.header_fabric.ftmh.action_type;
            }
        }
    }

    cmnd_type = mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP ? SOC_TMC_CMD_TYPE_SNOOP : SOC_TMC_CMD_TYPE_MIRROR; /*set command type*/
    if ((cmnd_type == SOC_TMC_CMD_TYPE_SNOOP) && (mirror_dest->flags & BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID)) {
        if (mirror_dest->encap_id != 0) {
            info.stamping.encap_id = mirror_dest->encap_id;
        }
    }

    if (mirror_dest->flags & BCM_MIRROR_DEST_OUT_MIRROR_DISABLE) {
        info.is_out_mirror_disable = TRUE;
    }

    if (mirror_dest->flags & BCM_MIRROR_DEST_EEI_INVALID) {
        info.is_eei_invalid = TRUE;
    }

    /* call dpp to set/create the mirror action profile */
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_set,(unit, action_profile_id, cmnd_type, &info));
    if (soc_sand_get_error_code_from_error_word(soc_sand_rv) != SOC_SAND_OK) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed setting action profile")));
    }

    if (!flags) { /* return the newly allocated profile */
        BCM_GPORT_MIRROR_SET(mirror_dest->mirror_dest_id, action_profile_id);
    }

    failed = 0;
exit:
    if (do_alloc == 9 && failed) {
        int is_last;
        /* Failure. no need to check the returned value */
        if (mirror_dest->flags & BCM_MIRROR_DEST_IS_SNOOP) {
            (void)_bcm_dpp_am_snoop_dealloc(unit,0,action_profile_id,0);
        } else {
            (void)_bcm_dpp_am_template_mirror_action_profile_free(unit,  action_profile_id,  &is_last);
        }

    }
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_destination_destroy
 *  Purpose
 *     Destroy mirror destination description.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Destroyes a mirror destination for use with bcm_mirror_port{_vlan}_dest_*
 */
int
bcm_petra_mirror_destination_destroy(int unit,
                                     bcm_gport_t mirror_dest_id)
{
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO info;
    uint32 action_profile_id;
    uint32 soc_sand_rv;
    int is_created;
    int is_last;
    SOC_TMC_CMD_TYPE cmnd_type;
    MIRROR_LOCK_DEFS;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (!BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    cmnd_type = (BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest_id) ? SOC_TMC_CMD_TYPE_SNOOP : SOC_TMC_CMD_TYPE_MIRROR);
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    if (cmnd_type == SOC_TMC_CMD_TYPE_MIRROR) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
    } else {
        is_created = _bcm_dpp_am_snoop_is_alloced(unit, action_profile_id) == BCM_E_EXISTS ? TRUE : FALSE;
    }

    if (!is_created) {
            BCMDNX_ERR_EXIT_MSG(is_created, (_BSL_BCM_MSG("specified destination ID %u is not created"), (unsigned)action_profile_id));
    }

    /* The code does not check if the destination is in use in all the possible places, as a requirements/design decision */

    /* set input data for mbcm_dpp_action_cmd_mirror_set and parse input flags */
    SOC_TMC_ACTION_CMD_MIRROR_INFO_clear(&info);
    info.cmd.dest_id.id = -1; /* mark destination as drop */
    info.cmd.dest_id.type = SOC_TMC_DEST_TYPE_QUEUE;
    info.prob = 0; /* 0% probability of mirror execution. */
    info.size = SOC_TMC_ACTION_CMD_SIZE_BYTES_ALL_PCKT;

    if (cmnd_type == SOC_TMC_CMD_TYPE_MIRROR) {
        /* deallocate mirror profile */
        if (_bcm_dpp_am_template_mirror_action_profile_free(unit,  action_profile_id,  &is_last)!= BCM_E_NONE) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to deallocate mirroring destination")));
        }
    } else {
        /* deallocate snoop profile */
        if(_bcm_dpp_am_snoop_dealloc(unit,0,action_profile_id,0) != BCM_E_NONE)
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed to deallocate snooping destination")));
    }


    /* call dpp to set the mirror action profile to not used */
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_set,(unit, action_profile_id, cmnd_type, &info));
    if (soc_sand_get_error_code_from_error_word(soc_sand_rv) != SOC_SAND_OK) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed resetting mirror action profile")));
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}


/*
 *  Function
 *     _bcm_petra_mirror_destination_erspan_get
 *  Purpose
 *     get ERSPAN data from soc level (stopred in 2 prge memory table entries).
 *  Parameters
 *     (in) unit = the unit number
 *     (out) bcm_mirror_destination_t *mirror_dest = dest description
 *     (in) mirror_index = mirror index in preg memory
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
STATIC int
_bcm_petra_mirror_destination_erspan_get(int unit,
                                         bcm_mirror_destination_t *mirror_dest, uint32 mirror_index)
{
    uint32 soc_sand_rv;
    SOC_PPC_EG_ENCAP_MIRROR_ENCAP_INFO
        mirror_encap_info;
    int rv;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(mirror_dest);

    /* get the data ERSPAN from soc lovel */
    soc_sand_rv = soc_ppd_eg_encap_mirror_entry_get(unit, mirror_index, &mirror_encap_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
    mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_L2;
    mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_IP_GRE;
    mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_WITH_SPAN_ID;

    mirror_dest->encap_id = mirror_index;

    mirror_dest->tpid     = mirror_encap_info.tunnel.tpid;
    mirror_dest->vlan_id  = mirror_encap_info.tunnel.vid;
    mirror_dest->pkt_prio = mirror_encap_info.tunnel.pcp;
    mirror_dest->span_id  = mirror_encap_info.tunnel.erspan_id;


    /* L2 source mac address. */
    rv = _bcm_petra_mac_from_sand_mac(mirror_dest->dst_mac, &(mirror_encap_info.tunnel.dest));
    BCMDNX_IF_ERR_EXIT(rv);

    /* L2 destination mac address. */
    rv = _bcm_petra_mac_from_sand_mac(mirror_dest->src_mac, &(mirror_encap_info.tunnel.src));
    BCMDNX_IF_ERR_EXIT(rv);


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_destination_get
 *  Purpose
 *     Get a mirror destination description.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *     (out) bcm_mirror_destination_t *mirror_dest = dest description
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Gets information about a mirror destination for use with
 *     bcm_mirror_port{_vlan}_dest_*
 */
int
bcm_petra_mirror_destination_get(int unit,
                                 bcm_gport_t mirror_dest_id,
                                 bcm_mirror_destination_t *mirror_dest)
{
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO info;
    uint32 action_profile_id = 0;
    int is_created;
    unsigned int proc;
    unsigned int cntr;
    SOC_TMC_CNT_SRC_TYPE                    src_type;
    unsigned int command_id;
    uint32 soc_sand_rv;
    unsigned int local_base_modid = 0;
    int is_erp_port = 0;
    uint32 modid = 0;
    uint32 tm_port = 0;
    SOC_PPC_EG_ENCAP_ENTRY_INFO *encap_entry_info = NULL;
    uint32 next_eep, nof_entries;
    SOC_TMC_CMD_TYPE cmnd_type;
    MIRROR_LOCK_DEFS;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(mirror_dest);

    /* verify that the given mirror destination is legal */
    if (!BCM_GPORT_IS_MIRROR(mirror_dest_id)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }

    sal_memset(&info, 0x0, sizeof(info));
    cmnd_type = BCM_GPORT_IS_MIRROR_SNOOP(mirror_dest_id); /*0 for mirror and 1 for snoop*/

    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest_id);
    if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }

    MIRROR_LOCK_TAKE;
    /* verify that the destination already exists*/
    if (cmnd_type == SOC_TMC_CMD_TYPE_MIRROR) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
    } else {
        /*verify that the snooping destination is created*/
        is_created = _bcm_dpp_am_snoop_is_alloced(unit, action_profile_id) == BCM_E_EXISTS ? TRUE : FALSE;
        mirror_dest->flags |= SOC_TMC_CMD_TYPE_SNOOP;
    }
    if (!is_created) {
        LOG_DEBUG(BSL_LS_BCM_MIRROR, (BSL_META_U(unit, "specified mirroring/snooping destination ID %u is not created"), (unsigned)action_profile_id));
        BCM_GPORT_MIRROR_SET(mirror_dest->mirror_dest_id, BCM_MIRROR_INVALID_PROFILE);
        BCM_EXIT;
    }

    /* call dpp to get the mirror action profile */
    soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_action_cmd_mirror_get,(unit, action_profile_id, cmnd_type, &info));
    MIRROR_LOCK_GIVE;
    if (soc_sand_get_error_code_from_error_word(soc_sand_rv) != SOC_SAND_OK) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed getting mirror action profile %u information"), (unsigned)action_profile_id));
    }

    /*check id destination is ERP port*/
    if (info.cmd.dest_id.type == SOC_TMC_DEST_TYPE_SYS_PHY_PORT) {
        BCM_SAND_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sys_phys_to_local_port_map_get, (unit, info.cmd.dest_id.id, &modid, &tm_port)));
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_system_fap_id_get,(unit, &local_base_modid)));
        if (modid >= local_base_modid && modid < local_base_modid + SOC_DPP_CONFIG(unit)->core_mode.nof_active_cores && tm_port == ARAD_FAP_EGRESS_REPLICATION_IPS_PORT_ID) {
            is_erp_port = 1;
        }
    }

    if (is_erp_port) {
        bcm_mirror_destination_t_init(mirror_dest);
        mirror_dest->mirror_dest_id = mirror_dest_id;
        BCM_GPORT_MCAST_SET((mirror_dest->gport), info.cmd.outlif.value);
    } else {
        /* set the returned information */
        bcm_mirror_destination_t_init(mirror_dest);
        mirror_dest->mirror_dest_id = mirror_dest_id;
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_from_tm_dest_info(unit, &mirror_dest->gport, &info.cmd.dest_id));
    }

    /* return the ingress crop size */
    mirror_dest->packet_copy_size = info.size == SOC_TMC_ACTION_CMD_SIZE_BYTES_64 ? 64 :
                                   (info.size == SOC_TMC_ACTION_CMD_SIZE_BYTES_128 ? 128 :
                                   (info.size == SOC_TMC_ACTION_CMD_SIZE_BYTES_192 ? 192 :
                                   (info.size == SOC_TMC_ACTION_CMD_SIZE_BYTES_256 ? 256 : 0)));

    /* return the sample probability */
    mirror_dest->sample_rate_dividend = info.prob ? info.prob + 1 : 0;
    mirror_dest->sample_rate_divisor = ARAD_MIRROR_PROBABILITY_DIVISOR;

    /*Update ENCAP ID*/
    mirror_dest->encap_id = info.cmd.outlif.value;

   /* in case i ERSPAN mirroring initialize the ERSPAN related data */
    if (SOC_IS_ARAD_B1_AND_BELOW(unit) &&
        1 == info.cmd.outlif.enable) {

        BCMDNX_ALLOC(encap_entry_info, sizeof(SOC_PPC_EG_ENCAP_ENTRY_INFO)*SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_MAX,
            "bcm_petra_mirror_destination_get.encap_entry_info");
        if (encap_entry_info == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("unit %d, failed to allocate memory"), unit));
        }

        /* get entry */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_encap_entry_get(unit,
                                                          SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP,
                                                          info.cmd.outlif.value, 1,
                                                          encap_entry_info, &next_eep,
                                                          &nof_entries));

        /* when entry type is IPV4 it is for sure ERSPAN mirroring */
        if (encap_entry_info[0].entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP) {
            BCM_L3_ITF_SET(mirror_dest->tunnel_id, BCM_L3_ITF_TYPE_LIF, info.cmd.outlif.value);
            mirror_dest->encap_id = mirror_dest->tunnel_id & 0xf;
            BCMDNX_IF_ERR_EXIT(_bcm_petra_mirror_destination_erspan_get(unit,
                                                                     mirror_dest,
                                                                     info.cmd.outlif.value & 0xf));
        }
    }

    if (info.cmd.counter_ptr_1.enable || info.cmd.counter_ptr_2.enable) {
        /* Find the Counter processor and get counter Id */
        cntr = 0;
        for (proc = 0; proc < SOC_DPP_DEFS_GET(unit, nof_counter_processors); proc++) {
            BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_source, proc, &src_type));
            if (src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
                BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_command_id, proc, &command_id));
                if (info.cmd.counter_ptr_2.enable && (command_id == 1)) { /* group 1 */
                    cntr = info.cmd.counter_ptr_2.value;
                    mirror_dest->flags |= BCM_MIRROR_DEST_UPDATE_COUNTER | BCM_MIRROR_DEST_UPDATE_COUNTER_2;
                    break;
                }
                else if (info.cmd.counter_ptr_1.enable && (command_id == 0)) { /* group 0 */
                    cntr = info.cmd.counter_ptr_1.value;
                    mirror_dest->flags |= BCM_MIRROR_DEST_UPDATE_COUNTER | BCM_MIRROR_DEST_UPDATE_COUNTER_1;
                    break;
                }
            }
        }
        if (cntr == SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INVALID_COUNTER) {
            mirror_dest->stat_id=BCM_MIRROR_DEST_NO_COUNT;
        } else {
            BCM_FIELD_STAT_ID_SET(mirror_dest->stat_id, proc, cntr);
        }
    }

    if (info.cmd.dp.enable) {
        mirror_dest->packet_control_updates.color = info.cmd.dp.value;
        mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_COLOR;
    }
    if (info.cmd.tc.enable) {
        mirror_dest->packet_control_updates.prio = info.cmd.tc.value;
        mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_PRIO;
    }

    if (SOC_IS_JERICHO(unit)) {
        if (info.valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_IN_PORT) {
            mirror_dest->packet_control_updates.in_port = info.in_port;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_IN_PORT;
        }
        if (info.valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_VSQ) {
            mirror_dest->packet_control_updates.vsq = info.vsq;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_VSQ;
        }
        if (info.valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_CNM_CANCEL) {
            mirror_dest->packet_control_updates.cnm_cancel = info.cnm_cancel;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_ECN_CNM_CANCEL;
        }
        if (info.valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_TRUNK_HASH_RESULT) {
            mirror_dest->packet_control_updates.trunk_hash_result = info.trunk_hash_result;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_TRUNK_HASH_RESULT;
        }
        if (info.valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_ECN_VALUE) {
            mirror_dest->packet_control_updates.ecn_value = info.ecn_value;
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_ECN_VALUE;
        }
        /* return the outbound sample probability */
        mirror_dest->egress_sample_rate_dividend = info.outbound_prob ? info.outbound_prob + 1 : 0;
        mirror_dest->egress_sample_rate_divisor = JER_MIRROR_PROBABILITY_DIVISOR;

        mirror_dest->flags |= (info.add_orig_head ? (info.add_orig_head == 2 ?
          BCM_MIRROR_DEST_EGRESS_TRAP_WITH_SYSTEM_HEADER : BCM_MIRROR_DEST_EGRESS_ADD_ORIG_SYSTEM_HEADER) : 0);
        /*return egress cropping - 0 for none, 1 for maximum 128B*/
        mirror_dest->egress_packet_copy_size = (info.crop_pkt ? JER_OUTBOUND_MIRROR_CROP_SIZE : 0);

        if (info.stamping.valid & SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO_STAMPING_FHEI) {
            mirror_dest->packet_control_updates.valid |= BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
            mirror_dest->packet_control_updates.header_fabric.internal_valid |= 1;
            mirror_dest->packet_control_updates.header_fabric.internal.trap_id = info.stamping.cpu_trap_code;
            mirror_dest->packet_control_updates.header_fabric.internal.trap_qualifier = info.stamping.cpu_trap_qualifier;
        }

        if (info.stamping.encap_id != 0) {
            mirror_dest->flags |= BCM_MIRROR_DEST_TUNNEL_WITH_ENCAP_ID;
        }

        if (info.is_out_mirror_disable != 0) {
            mirror_dest->flags |= BCM_MIRROR_DEST_OUT_MIRROR_DISABLE;
        }

        if (info.is_eei_invalid != 0) {
            mirror_dest->flags |= BCM_MIRROR_DEST_EEI_INVALID;
        }

        if ((cmnd_type == SOC_TMC_CMD_TYPE_MIRROR) &&
            ((info.action_type == bcmPktDnxFtmhActionTypeInboundMirror) ||
             (info.action_type == bcmPktDnxFtmhActionTypeOutboundMirror))) {
            mirror_dest->packet_control_updates.header_fabric.ftmh_valid = TRUE;
            mirror_dest->packet_control_updates.header_fabric.ftmh.action_type = info.action_type;
        }
    }

    if (SOC_IS_QAX(unit))
    {
        /* Get the IDs of statistic interfaces interfaces for mirrored packets*/
        mirror_dest->ext_stat_id[0]=info.ext_stat_id[0];
        mirror_dest->ext_stat_id[1]=info.ext_stat_id[1];
    }

exit:
    BCM_FREE(encap_entry_info);
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_destination_traverse
 *  Purpose
 *     Traverse existing mirror destination descriptions.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_mirror_destination_traverse_cb cb = ptr to callback handler
 *     (in) void *user_data = user data to pass to callback handler
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Traverses all mirror destinations for use with bcm_mirror_port{_vlan}_dest_*
 */
int
bcm_petra_mirror_destination_traverse(int unit,
                                      bcm_mirror_destination_traverse_cb cb,
                                      void *user_data)
{
    uint32 soc_sand_rv;
    uint32 action_profile_id = 0;
    int is_created, ret;
    bcm_mirror_destination_t mirror_dest;
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO info;
    unsigned int proc;
    unsigned int cntr;
    SOC_TMC_CNT_SRC_TYPE src_type;
    unsigned int command_id;
    MIRROR_LOCK_DEFS;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    BCMDNX_NULL_CHECK(cb);

    for (action_profile_id = DPP_MIRROR_ACTION_NDX_MIN; action_profile_id <= DPP_MIRROR_ACTION_NDX_MAX; ++action_profile_id) {
        MIRROR_LOCK_TAKE;
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        if (is_created) {
            /* call dpp to get the mirror action profile */
            soc_sand_rv = MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_action_cmd_mirror_get,(unit, action_profile_id, SOC_TMC_CMD_TYPE_MIRROR, &info));
            if (soc_sand_get_error_code_from_error_word(soc_sand_rv) != SOC_SAND_OK) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Failed getting mirror destination %u information, stopping"), (unsigned)action_profile_id));
            }
            /* set the destination information for the callback */
            bcm_mirror_destination_t_init(&mirror_dest);
            BCM_GPORT_MIRROR_SET(mirror_dest.mirror_dest_id, action_profile_id);
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_gport_from_tm_dest_info(unit, &mirror_dest.gport, &info.cmd.dest_id));

            if (info.cmd.counter_ptr_1.enable || info.cmd.counter_ptr_2.enable) {
                /* Find the Counter processor and get counter Id */
                cntr = 0;
                for (proc = 0; proc < SOC_DPP_DEFS_GET(unit, nof_counter_processors); proc++) {
                    BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_diag_info_get(unit,bcm_dpp_counter_diag_info_source, proc, &src_type));

                    if (src_type == SOC_TMC_CNT_SRC_TYPE_ING_PP) {
                        BCMDNX_IF_ERR_EXIT(bcm_dpp_counter_diag_info_get(unit, bcm_dpp_counter_diag_info_command_id, proc, &command_id));
                        if (info.cmd.counter_ptr_2.enable && (command_id == 1)) { /* group 1 */
                            cntr = info.cmd.counter_ptr_2.value;
                            break;
                        }
                        else if (info.cmd.counter_ptr_1.enable && (command_id == 0)) { /* group 1 */
                            cntr = info.cmd.counter_ptr_1.value;
                            break;
                        }
                    }
                }
                BCM_FIELD_STAT_ID_SET(mirror_dest.stat_id, proc, cntr);
            }

            MIRROR_LOCK_GIVE;
            ret = cb(unit, &mirror_dest, user_data);
            if (ret) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("callback function returned error value %d for mirror destination %u"),ret, (unsigned)action_profile_id));
            }
        }
    }
exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_dest_add
 *  Purpose
 *     Add a mirroring destination to a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes:
 *     See bcm_petra_mirror_port_destination_add
 */
int
bcm_petra_mirror_port_dest_add(int unit,
                               bcm_port_t port,
                               uint32 flags,
                               bcm_gport_t mirror_dest)
{
    bcm_mirror_options_t options;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_mirror_options_t_init(&options);
    options.forward_strength = options.copy_strength = 1 ;
    if (SOC_IS_JERICHO(unit)) {
        /*
         * Note that a 'recycle_cmd' of value '0' will be ignored by bcm_petra_mirror_port_destination_add()
         * Since this is the default port and should not be touched by standard API.
         *
         * Since this old-style procedure invokes the new-style procedure,
         * bcm_petra_mirror_port_destination_add(), then make sure the recycle_cmd element
         * in 'options' is set to '0' so that even if the BCM_MIRROR_PORT_EGRESS_ACL
         * is set, the egress PMF mirror table is not accessed.
 */
        options.recycle_cmd = 0 ;
    }
    BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_port_destination_add(unit, port, flags, mirror_dest, options));

    exit:
        BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_dest_delete
 *  Purpose
 *     Remove a mirroring destination from a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_port_dest_delete(int unit,
                                  bcm_port_t port,
                                  uint32 flags,
                                  bcm_gport_t mirror_dest)
{
    uint32 action_profile_id, found_profile = 0;
    SOC_PPC_PORT loc_port;
    int is_created, is_destination;
    int  core;
    uint32  pp_port;
    dpp_outbound_mirror_config_t        outbound_mirror_config ;

    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));

    is_destination = BCM_MIRROR_PORT_EGRESS_ACL != (flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL));
    /* check destination validity */
    if (is_destination && !BCM_GPORT_IS_MIRROR(mirror_dest)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest);
    if (is_destination && (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    if (is_destination) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        if (!is_created) { /* verify that the destination already exists */
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not created"), (unsigned)action_profile_id));
        }
    }

    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));

        /* check that the destination profile is the specified one */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_dflt_get((unit), core, pp_port, &dflt_mirroring_info));
        if (dflt_mirroring_info.is_tagged_dflt && dflt_mirroring_info.is_untagged_only) {
            /*Port Mirroring*/
            if ((found_profile = dflt_mirroring_info.tagged_dflt) != dflt_mirroring_info.untagged_dflt) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Inconsistent destination action profiles %u, %u for the port"),
                  (unsigned)found_profile, (unsigned)dflt_mirroring_info.untagged_dflt));
            }
        } else if (dflt_mirroring_info.is_tagged_dflt) {
            /* Tagged mirroring */
            found_profile = dflt_mirroring_info.tagged_dflt;
        } else if (dflt_mirroring_info.is_untagged_only) {
            /* Untagged mirroring*/
            found_profile = dflt_mirroring_info.untagged_dflt;
        }
        /* check retrieved destination validity */
        if (!found_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("inbound port was not added to mirror")));
        } else if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }
        if (found_profile != action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The inbound port has destination profile %u, and not the specified one"),
              (unsigned)found_profile));
        }

        /* call dpp to set the port mirroring to the given profile */
        SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info); /* mark as not mirrored */
        dflt_mirroring_info.is_tagged_dflt = 1;
        dflt_mirroring_info.is_untagged_only = 1;
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_dflt_set((unit), core, pp_port, &dflt_mirroring_info));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */

        /* check that the destination profile is the specified one */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_dflt_get((unit), core, pp_port,  &outbound_mirror_config));
        /* check retrieved destination validity */
        found_profile = outbound_mirror_config.mirror_command;
        if (!found_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outbound port was not added to mirror")));
        }
        if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }
        if (found_profile != action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The outbound port has destination profile %u, and not the specified one"),
              (unsigned)found_profile));
        }
        /* call dpp to set the port mirroring to the given profile */
        outbound_mirror_config.mirror_command = 0;
        outbound_mirror_config.forward_en=0;
        outbound_mirror_config.forward_strength=0;
        outbound_mirror_config.mirror_en=0;
        outbound_mirror_config.mirror_strength=0;

        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_dflt_set((unit), core, pp_port, &outbound_mirror_config));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS_ACL) { /* remove reservation of reassembly context and recycle channel for port for other applications */
        uint8 recycle_enabled;
        /* check if mirroring is already enabled */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_appl_get((unit), loc_port, &recycle_enabled));
        if (!recycle_enabled) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port is not enabled for mirroring by other applications like acl")));
        }

        /* disable mirroring */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_appl_set((unit), loc_port, 0));
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_dest_delete_all
 *  Purpose
 *     Remove all mirroring destinations from a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_port_dest_delete_all(int unit,
                                      bcm_port_t port,
                                      uint32 flags)
{
    SOC_PPC_PORT loc_port;
    uint32 found_profile = 0;
    uint32 pp_port;
    int    core;
    dpp_outbound_mirror_config_t        outbound_mirror_config;

    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));

    MIRROR_LOCK_TAKE;
    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;


        /* check that the port was added */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_dflt_get((unit), core, pp_port, &dflt_mirroring_info));
        if (dflt_mirroring_info.is_tagged_dflt && dflt_mirroring_info.is_untagged_only) {
            /* Port Mirroring*/
            if ((found_profile = dflt_mirroring_info.tagged_dflt) != dflt_mirroring_info.untagged_dflt) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Inconsistent destination action profiles %u, %u for the port"),
                  (unsigned)found_profile, (unsigned)dflt_mirroring_info.untagged_dflt));
            }
        } else if (dflt_mirroring_info.is_tagged_dflt) {
            /* Tagged mirroring */
            found_profile = dflt_mirroring_info.tagged_dflt;
        } else if (dflt_mirroring_info.is_untagged_only) {
            /* Untagged mirroring*/
            found_profile = dflt_mirroring_info.untagged_dflt;
        }
        /* check retrieved destination validity */
        if (!found_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("inbound port was not added to mirror")));
        } else if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }

        /* call dpp to set the port mirroring to the given profile */
        SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info); /* mark as not mirrored */
        dflt_mirroring_info.is_tagged_dflt = 1;
        dflt_mirroring_info.is_untagged_only = 1;
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_dflt_set((unit), core, pp_port, &dflt_mirroring_info));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        SOC_PPC_EG_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;

        /* check that the port was added */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_dflt_get((unit), core, pp_port, &outbound_mirror_config));
        /* check retrieved destination validity */
        found_profile = outbound_mirror_config.mirror_command;
        if (!found_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("outbound port was not added to mirror")));
        }
        if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
        }

        /* call dpp to set the port mirroring to the given profile */
        outbound_mirror_config.mirror_command =  0;
        outbound_mirror_config.forward_en=0;
        outbound_mirror_config.forward_strength=0;
        outbound_mirror_config.mirror_en=0;
        outbound_mirror_config.mirror_strength=0;

        SOC_PPC_EG_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info); /* mark as not mirrored */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_dflt_set((unit), core, pp_port, &outbound_mirror_config));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS_ACL) { /* remove reservation of reassembly context and recycle channel for port for other applications */
        uint8 recycle_enabled;
        /* check if mirroring is already enabled */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_appl_get((unit), loc_port, &recycle_enabled));
        if (!recycle_enabled) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port is not enabled for mirroring by other applications like acl")));
        }

        /* disable mirroring */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_appl_set((unit), loc_port, 0));
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_dest_get
 *  Purpose
 *     Get (all?) mirroring destinations on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) int mirror_dest_size = size of provided buffer
 *     (out) bcm_gport_t *mirror_dest = pointer to buffer for destinations
 *     (out) int *mirror_dest_count = number of mirror dests
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     The value written to mirror_dest_count will be the number of mirror
 *     destinations actually filled in mirror_dest, except if the value of
 *     mirror_dest_size is zero.  In this case, nothing is written to
 *     mirror_dest and mirror_dest_count is populated with the actual number of
 *     mirror destinations on the port.
 */
int
bcm_petra_mirror_port_dest_get(int unit,
                               bcm_port_t port,
                               uint32 flags,
                               int mirror_dest_size,
                               bcm_gport_t *mirror_dest,
                               int *mirror_dest_count)
{
    bcm_mirror_options_t options;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_mirror_options_t_init(&options);
    BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_port_destination_get(unit, port, flags, mirror_dest_size, mirror_dest, mirror_dest_count, &options));

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_add
 *  Purpose
 *     Add a mirroring destination to a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *  Assume only ingress mirroring.
 */
int
bcm_petra_mirror_port_vlan_dest_add(int unit,
                                    bcm_port_t port,
                                    bcm_vlan_t vlan,
                                    uint32 flags,
                                    bcm_gport_t mirror_dest)
{
    bcm_mirror_options_t options;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_mirror_options_t_init(&options);
    options.forward_strength = options.copy_strength = 1;

    BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_port_vlan_destination_add(unit, port, vlan, flags, mirror_dest, options));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_delete
 *  Purpose
 *     Remove a mirroring destination from a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
/* bcm_petra_mirror_port_vlan_dest_delete */
int
bcm_petra_mirror_port_vlan_dest_delete(int unit,
                                       bcm_port_t port,
                                       bcm_vlan_t vlan,
                                       uint32 flags,
                                       bcm_gport_t mirror_dest)
{
    uint32 action_profile_id;
    SOC_PPC_PORT loc_port;
    int is_created;
    int core = SOC_CORE_INVALID;
    uint32  pp_port;
    dpp_outbound_mirror_config_t        outbound_mirror_config;
    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if(!SOC_DPP_PP_ENABLE(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("PP mode must be enabled")));
    }

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));

    /* check destination validity */
    if (!BCM_GPORT_IS_MIRROR(mirror_dest)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest);
    if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
    if (!is_created) { /* verify that the destination already exists */
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not created"), (unsigned)action_profile_id));
    }

    if (vlan > SOC_SAND_PP_VLAN_ID_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN ID is out of range")));
    }

    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        uint32 mirror_profile;
        /* check that the destination profile is the specified one */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_vlan_get((unit), core, pp_port, vlan, &mirror_profile));
        if (!mirror_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified inbound port-vlan was not added")));
        }
        if (mirror_profile > DPP_MIRROR_ACTION_NDX_MAX || mirror_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)mirror_profile));
        }
        if (mirror_profile != action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The inbound port-vlan combination has destination profile %u, and not the specified one"),
              (unsigned)mirror_profile));
        }

        /* call dpp to remove the port-vlan mirroring */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_vlan_remove((unit), core, pp_port, vlan));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */

        /* check that the destination profile is the specified one */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_vlan_get((unit), core, pp_port, vlan,  &outbound_mirror_config));
        if (!outbound_mirror_config.mirror_command) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified outbound port-vlan was not added")));
        }
        if (outbound_mirror_config.mirror_command > DPP_MIRROR_ACTION_NDX_MAX || outbound_mirror_config.mirror_command < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)outbound_mirror_config.mirror_command));
        }
        if (outbound_mirror_config.mirror_command != action_profile_id) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("The outbound port-vlan combination has destination profile %u, and not the specified one"),
              (unsigned)outbound_mirror_config.mirror_command));
        }

        /* call dpp to remove the port-vlan mirroring */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_vlan_remove((unit), core, pp_port, vlan));
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_delete_all
 *  Purpose
 *     Remove all mirroring destinations from a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_port_vlan_dest_delete_all(int unit,
                                           bcm_port_t port,
                                           bcm_vlan_t vlan,
                                           uint32 flags)
{
    SOC_PPC_PORT loc_port;
    int     core;
    uint32  pp_port;
    dpp_outbound_mirror_config_t        outbound_mirror_config;

    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);

    if(!SOC_DPP_PP_ENABLE(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("PP mode must be enabled")));
    }
    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    if (vlan > SOC_SAND_PP_VLAN_ID_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN ID is out of range")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));


    MIRROR_LOCK_TAKE;
    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        uint32 mirror_profile;
        /* check that the port-vlan was added */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_vlan_get((unit), core, pp_port, vlan, &mirror_profile));
        if (!mirror_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified inbound port-vlan was not added")));
        }
        if (mirror_profile > DPP_MIRROR_ACTION_NDX_MAX || mirror_profile < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)mirror_profile));
        }
        /* call dpp to remove the port-vlan mirroring */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_vlan_remove((unit), core, pp_port, vlan));
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        /* check that the port-vlan was added */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_vlan_get((unit), core, pp_port, vlan, &outbound_mirror_config));
        if (!outbound_mirror_config.mirror_command) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("The specified outbound port-vlan was not added")));
        }
        if (outbound_mirror_config.mirror_command > DPP_MIRROR_ACTION_NDX_MAX || outbound_mirror_config.mirror_command < DPP_MIRROR_ACTION_NDX_MIN) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)outbound_mirror_config.mirror_command));
        }

        /* call dpp to remove the port-vlan mirroring */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_vlan_remove((unit), core, pp_port, vlan));
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_get
 *  Purpose
 *     Get (all?) mirroring destinations on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) int mirror_dest_size = size of provided buffer
 *     (out) bcm_gport_t *mirror_dest = pointer to buffer for destinations
 *     (out) int *mirror_dest_count = number of mirror dests
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     The value written to mirror_dest_count will be the number of mirror
 *     destinations actually filled in mirror_dest, except if the value of
 *     mirror_dest_size is zero.  In this case, nothing is written to
 *     mirror_dest and mirror_dest_count is populated with the actual number of
 *     mirror destinations on the port.
 */
int
bcm_petra_mirror_port_vlan_dest_get(int unit,
                                    bcm_port_t port,
                                    bcm_vlan_t vlan,
                                    uint32 flags,
                                    uint32 mirror_dest_size,
                                    bcm_gport_t *mirror_dest,
                                    uint32 *mirror_dest_count)
{
    bcm_mirror_options_t options;

    BCMDNX_INIT_FUNC_DEFS;

    bcm_mirror_options_t_init(&options);
    BCMDNX_IF_ERR_EXIT(bcm_petra_mirror_port_vlan_destination_get(unit, port, vlan, flags, mirror_dest_size, mirror_dest, mirror_dest_count, &options));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_mode_set
 *  Purpose
 *     Set mirroring mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) int mode = the mirroring mode to use
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_mode_set(int unit,
                          int mode)
{
    int rv = 0;
    int mirror_id, is_created;
    int mirror_mode;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_sw_db_petra_mirror_mode_get(unit, &mirror_mode);
    BCMDNX_IF_ERR_EXIT(rv);
    if ((mirror_mode != mode) && (mode == BCM_PETRA_MIRROR_MODE_DESTINATION_CREATE)){

        for (mirror_id = 1; mirror_id < 16; mirror_id++) {
            BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, mirror_id, &is_created));
            if (is_created) {
                BCMDNX_IF_ERR_EXIT(_bcm_petra_ingress_mirror_free(unit, mirror_id));
            }
        }
    }

    rv = _bcm_sw_db_petra_mirror_mode_set(unit, mode);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_mode_get
 *  Purpose
 *     Set mirroring mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (out) int *mode = where to put the mirroring mode
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_mode_get(int unit,
                          int *mode)
{
    int rv = 0;

    BCMDNX_INIT_FUNC_DEFS;
    rv = _bcm_sw_db_petra_mirror_mode_get(unit, mode);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_to_set
 *  Purpose
 *     Set mirroring mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to which mirror frames are to be sent
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_to_set(int unit,
                        bcm_port_t port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_to_get
 *  Purpose
 *     Set mirroring mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t *port = where to put mirror dest port
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_to_get(int unit,
                        bcm_port_t *port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_ingress_set
 *  Purpose
 *     Enable or disable ingress mirroring on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port on which to set ingress mirroring
 *     (in) int val = nonzero to enable ingress mirroring
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_ingress_set(int unit,
                             bcm_port_t port,
                             int val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_ingress_set
 *  Purpose
 *     Get ingress mirroring state on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port on which to get ingress mirroring
 *     (out) int *val = where to put ingress mirror flag for this port
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_ingress_get(int unit,
                             bcm_port_t port,
                             int *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_egress_set
 *  Purpose
 *     Enable or disable egress mirroring on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port on which to set egress mirroring
 *     (in) int val = nonzero to enable egress mirroring
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_egress_set(int unit,
                            bcm_port_t port,
                            int val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_egress_get
 *  Purpose
 *     Get egress mirroring state on a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port on which to set egress mirroring
 *     (out) int *val = where to put port's egress mirroring state
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only used on devices that support exactly one mirror dest
 */
int
bcm_petra_mirror_egress_get(int unit,
                            bcm_port_t port,
                            int *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_pfmt_set
 *  Purpose
 *     Set mirroring format preserve mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) int mode = nonzero to preserve mirrored frame format
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_pfmt_set(int unit,
                          int val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_pfmt_get
 *  Purpose
 *     Get mirroring format preserve mode for the device
 *  Parameters
 *     (in) int unit = the unit number
 *     (out) int *mode = where to put format preserve setting
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 */
int
bcm_petra_mirror_pfmt_get(int unit,
                          int *val)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_to_pbmp_set
 *  Purpose
 *     Set a port to mirror to a set of ports
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) bcm_pbmp_t pbmp = the mirror-to ports for the specified port
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Sets up so a port is mirrored to a *set* of ports.
 */
int
bcm_petra_mirror_to_pbmp_set(int unit,
                             bcm_port_t port,
                             bcm_pbmp_t pbmp)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_to_pbmp_get
 *  Purpose
 *     Get the set of ports to which a port is mirroring
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (out) bcm_pbmp_t *pbmp = where to put the port's the mirror-to ports
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Sets up so a port is mirrored to a *set* of ports.
 */

int
bcm_petra_mirror_to_pbmp_get(int unit,
                             bcm_port_t port,
                             bcm_pbmp_t *pbmp)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_vlan_set
 *  Purpose
 *     Set VLAN tag for mirror frames egressing a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint16 tpid = TPID to use (full 16b)
 *     (in) uint16 vlan = VLAN tag (full 16b)
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     This affects how mirrored frames are encapsulated on a port -- if VLAN
 *     or TPID is zero, mirror frames egressing the port will not have a tag
 *     inserted.  If VLAN and TPID are nonzero, mirror frames egressing the
 *     port will have the specified bytes inserted as outer VLAN tag.
 */
int
bcm_petra_mirror_vlan_set(int unit,
                          bcm_port_t port,
                          uint16 tpid,
                          uint16 vlan)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_vlan_get
 *  Purpose
 *     Get VLAN tag for mirror frames egressing a port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (out) uint16 *tpid = where to put TPID to use (full 16b)
 *     (out) uint16 *vlan = where to put VLAN tag (full 16b)
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     This affects how mirrored frames are encapsulated on a port -- if VLAN
 *     or TPID is zero, mirror frames egressing the port will not have a tag
 *     inserted.  If VLAN and TPID are nonzero, mirror frames egressing the
 *     port will have the specified bytes inserted as outer VLAN tag.
 */

int
bcm_petra_mirror_vlan_get(int unit,
                          bcm_port_t port,
                          uint16 *tpid,
                          uint16 *vlan)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_egress_path_set
 *  Purpose
 *     Configure specific propagation path for mirror frames in stack rings.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_module_t modid = control mirror frames to this module
 *     (in) bcm_port_t port = which port to take to get to this module
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only applicable to devices that operate in stack-ring mode.
 */
int
bcm_petra_mirror_egress_path_set(int unit,
                                 bcm_module_t modid,
                                 bcm_port_t port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_egress_path_get
 *  Purpose
 *     Get specific propagation path for mirror frames in stack rings.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_module_t modid = control mirror frames to this module
 *     (out) bcm_port_t *port = where to put port to take to get to module
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only applicable to devices that operate in stack-ring mode.
 */
int
bcm_petra_mirror_egress_path_get(int unit,
                                 bcm_module_t modid,
                                 bcm_port_t *port)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Not Supported")));

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_info_set
 *  Purpose
 *     Set mirror information per port. Set system port ID per outbound mirror.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = ingress mirror port or egress mirror port
 *     (in) bcm_mirror_port_info_t *info = port mirror information
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only applicable ARAD. Used only for egress mirroring.
 *     The use must set the API after setting an outbound mirror, and set it again if stopping and starting again the mirroring.
 *     Future consideration:
 *     Storing the system port per output port would allow the user to make the configuration only once and possibly before setting the outbound mirror.
 */
int
bcm_petra_mirror_port_info_set(int unit,
                               bcm_port_t port,
                               uint32 flags,
                               bcm_mirror_port_info_t *info)
{
    SOC_PPC_PORT loc_port;
    SOC_PPC_EG_MIRROR_PORT_INFO ppd_port_info;

    int rv = 0;
    bcm_gport_t sysport;
    bcm_gport_t local_port;
    int     core;
    uint32  pp_port;

    BCMDNX_INIT_FUNC_DEFS;

    if (flags & ~(BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));

    if (flags & (BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        SOC_PPC_EG_MIRROR_PORT_INFO_clear(&ppd_port_info);
        /* Return connected In-Port */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_info_get(unit, core, pp_port, &ppd_port_info));

        if (ppd_port_info.outbound_mirror_enable) {
            BCM_GPORT_LOCAL_SET(local_port,ppd_port_info.outbound_port_ndx);
            BCM_GPORT_SYSTEM_PORT_ID_SET(sysport, info->mirror_system_id);

            rv = bcm_petra_stk_sysport_gport_set(unit, sysport, local_port);
            BCMDNX_IF_ERR_EXIT(rv);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Outbound mirror is not enabled")));
        }

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No acceptable flag combination was specified")));
    }



exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_info_get
 *  Purpose
 *     Get mirror information per port. Retreive system port ID per outbound mirror.
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = ingress mirror port or egress mirror port
 *     (in) bcm_mirror_port_info_t *info = port mirror information
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Only applicable for ARAD. Used only for egress mirroring.
 */
int
bcm_petra_mirror_port_info_get(int unit,
                               bcm_port_t port,
                               uint32 flags,
                               bcm_mirror_port_info_t *info)
{
    SOC_PPC_PORT loc_port;
    SOC_PPC_EG_MIRROR_PORT_INFO ppd_port_info;

    int rv = 0;
    bcm_gport_t sysport;
    bcm_gport_t local_port;
    int  core;
    uint32  pp_port;

    BCMDNX_INIT_FUNC_DEFS;

    if (flags & ~(BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }

    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));

    if (flags & (BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL)) {
        SOC_PPC_EG_MIRROR_PORT_INFO_clear(&ppd_port_info);
        /* Return connected In-Port */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_info_get(unit, core, pp_port, &ppd_port_info));

        if (ppd_port_info.outbound_mirror_enable) {
            BCM_GPORT_LOCAL_SET(local_port,ppd_port_info.outbound_port_ndx);

            rv = bcm_petra_stk_gport_sysport_get(unit, local_port, &sysport);
            BCMDNX_IF_ERR_EXIT(rv);

            info->mirror_system_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("Outbound mirror is not enabled")));
        }

    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("No acceptable flag combination was specified")));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     bcm_petra_mirror_port_destination_add
 *  Purpose
 *     extended versions of existing bcm_petra_mirror_port_destination_add API using a bcm_mirror_options_t argument
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *     (in) bcm_mirror_options_t options = input the options for the mirroring of packets
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes:
 *     See bcm_petra_mirror_port_dest_add()
 *     For Jericho and up, if options.recycle_cmd is '0' then egress PMF mirror table
 *     is not accessed.
 *     Also, for Jericho and up, setting 'mirror_dest' such that 'action_profile_id'
 *     turns out to be '0' is considered an error (since it is reserved for 'default').
 */
int
bcm_petra_mirror_port_destination_add(int unit,
                                      bcm_port_t port,
                                      uint32 flags,
                                      bcm_gport_t mirror_dest,
                                      bcm_mirror_options_t options)
{
    uint32 action_profile_id = 0, found_profile;
    SOC_PPC_PORT loc_port;
    int is_created, is_destination;
    int  core;
    uint32  pp_port;
    dpp_outbound_mirror_config_t outbound_mirror_config;

    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_UNTAGGED_ONLY | BCM_MIRROR_PORT_TAGGED_DEFAULT | BCM_MIRROR_PORT_EGRESS_ACL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    if (!(flags & BCM_MIRROR_PORT_INGRESS) && (flags & (BCM_MIRROR_PORT_UNTAGGED_ONLY | BCM_MIRROR_PORT_TAGGED_DEFAULT)) ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_PORT_UNTAGGED_ONLY and BCM_MIRROR_PORT_TAGGED_DEFAULT flags can be used with Ingress mirroring only.")));
    } else if ((flags & BCM_MIRROR_PORT_UNTAGGED_ONLY) && (flags & BCM_MIRROR_PORT_TAGGED_DEFAULT)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("BCM_MIRROR_PORT_UNTAGGED_ONLY and BCM_MIRROR_PORT_TAGGED_DEFAULT can not be used together.")));
    }
    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));
    is_destination = BCM_MIRROR_PORT_EGRESS_ACL != (flags & (BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_EGRESS_ACL));

#ifdef BCM_ARAD_SUPPORT
    /*
     * On Jericho (and up), provided options.recycle_cmd is non-zero, 'mirror_dest' and
     * 'options' play a role even if 'flags' only specifies 'BCM_MIRROR_PORT_EGRESS_ACL'!
     * Otherwise, if only 'BCM_MIRROR_PORT_EGRESS_ACL' is specified then these parameters
     * are not referred to.
     * In other words:
     *   For below-Jericho devices, 'is_destination' is FALSE only if BCM_MIRROR_PORT_EGRESS_ACL
     *   is the only flag which is set.
     * Remark:
     *   if options.recycle_cmd is zero then we assume this procedure was called from older
     *   interface proc bcm_petra_mirror_port_dest_add() and procedure is assumed to operate
     *   in the same way as this older interface.
     */
    if (SOC_IS_JERICHO(unit)) {
        if (options.recycle_cmd > 0) {
            is_destination = TRUE;
        }
    }
#endif
    /* check destination validity */
    if (is_destination && !BCM_GPORT_IS_MIRROR(mirror_dest)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest);
    if (is_destination && (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    if (is_destination) {
        BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
        if (!is_created) { /* verify that the destination already exists */
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not created"), (unsigned)action_profile_id));
        }
    }

    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;
        /* check that the port is not already mirrored */

        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_dflt_get((unit), core, pp_port, &dflt_mirroring_info));
        if (!(flags & (BCM_MIRROR_PORT_UNTAGGED_ONLY | BCM_MIRROR_PORT_TAGGED_DEFAULT))) {
            /*port mirroring*/
            if ((found_profile = dflt_mirroring_info.tagged_dflt) != dflt_mirroring_info.untagged_dflt) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Inconsistent destination action profiles %u, %u for the port"),
                  (unsigned)found_profile, (unsigned)dflt_mirroring_info.untagged_dflt));
            }
            if (found_profile) { /* the port is mirrored or internal error */
                if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
                }
                BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The inbound port is already mirrored")));
            }
            SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info);
            dflt_mirroring_info.tagged_dflt = action_profile_id;
            dflt_mirroring_info.untagged_dflt = action_profile_id;
            dflt_mirroring_info.is_untagged_only = 1;
            dflt_mirroring_info.is_tagged_dflt = 1;
        } else if (flags & BCM_MIRROR_PORT_UNTAGGED_ONLY) {
            if (dflt_mirroring_info.is_untagged_only || dflt_mirroring_info.is_tagged_dflt) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The inbound port is already mirrored")));
            }
            /*untagged traffic mirroring*/
            SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info);
            dflt_mirroring_info.untagged_dflt = action_profile_id;
            dflt_mirroring_info.is_untagged_only = 1;
        } else if (flags & BCM_MIRROR_PORT_TAGGED_DEFAULT) {
            if (dflt_mirroring_info.is_untagged_only || dflt_mirroring_info.is_tagged_dflt) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The inbound port is already mirrored")));
            }
            /*tagged traffic mirroring*/
            SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO_clear(&dflt_mirroring_info);
            dflt_mirroring_info.tagged_dflt = action_profile_id;
            dflt_mirroring_info.is_tagged_dflt = 1;
        }
            /* call dpp to set the port mirroring to the given profile */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_dflt_set((unit), core, pp_port, &dflt_mirroring_info));
    }

    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        /* check that the port is not already mirrored */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_dflt_get((unit), core, pp_port, &outbound_mirror_config));
        found_profile = outbound_mirror_config.mirror_command;

        if (found_profile) {
            if (found_profile > DPP_MIRROR_ACTION_NDX_MAX) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
            }
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port is already mirrored")));
        }

        /* call dpp to set the port mirroring to the given profile */
        outbound_mirror_config.mirror_command = action_profile_id;
        outbound_mirror_config.forward_en = 1;
        outbound_mirror_config.forward_strength = options.forward_strength;
        outbound_mirror_config.mirror_en = 1;
        outbound_mirror_config.mirror_strength = options.copy_strength;

        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_dflt_set((unit), core, pp_port,  &outbound_mirror_config));
    }

    if (flags & BCM_MIRROR_PORT_EGRESS_ACL) { /* enable of outbound mirroring (reserve reassembly context and recycle channel) for other applications */
        uint8 recycle_enabled ;
        dpp_outbound_mirror_config_t config ;
        /* check whether mirroring is already enabled */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_appl_get((unit), loc_port, &recycle_enabled));
        if (recycle_enabled) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port is already enabled for mirroring by other applications like acl")));
        }
        /*
         * enable mirroring
         */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_appl_set((unit), loc_port, 1)) ;
        if (SOC_IS_JERICHO(unit)) {
            /*
             * On Jericho and up:
             * Update table which assigns 'trap parameters' to specified mirror profile.
             * These parameter are mainly used for enabling FWD/MIRROR and for arbitration
             * (using 'strength') on which of the sources to select for FWD/MIRROR operation.
             * Currently, the competing sources are:
             *   Mirror traps
             *   Egress port/vid mirroring
             *   OAM mirror profile
             *   PMF mirror profile (the one we set right below)
             * Note:
             *   A 'recycle_cmd' of '0' is ignored for smooth transition on using the
             *   older style interface of  bcm_petra_mirror_port_dest_add().
             */
            if (options.recycle_cmd > 0) {
                if (action_profile_id == 0) {
                    BCMDNX_ERR_EXIT_MSG(
                        BCM_E_PARAM,
                        (_BSL_BCM_MSG("Specified action profile (%d) is not allowed (reserved)"),action_profile_id)) ;
                }
                config.mirror_command = action_profile_id ;
                config.forward_en = 1 ;
                config.forward_strength = options.forward_strength ;
                config.mirror_en = 1 ;
                config.mirror_strength = options.copy_strength ;
                /*
                 * Call low level access procedure.
                 * For Jericho, call soc_ppd_eg_pmf_mirror_params_set()
                 */
                MBCM_PP_DRIVER_CALL(unit, mbcm_pp_eg_pmf_mirror_params_set, (unit,action_profile_id,&config)) ;
            }
        }
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}
/*
 *  Function
 *     bcm_petra_mirror_port_destination_get
 *  Purpose
 *     extended versions of existing bcm_petra_mirror_port_destination_get API using a bcm_mirror_options_t argument
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) int mirror_dest_size = size of provided buffer
 *     (out) bcm_gport_t *mirror_dest = pointer to buffer for destinations
 *     (out) int *mirror_dest_count = number of mirror dests
 *     (out) bcm_mirror_options_t *options = get the options for the mirroring of packets
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     The value written to mirror_dest_count will be the number of mirror
 *     destinations actually filled in mirror_dest, except if the value of
 *     mirror_dest_size is zero.  In this case, nothing is written to
 *     mirror_dest and mirror_dest_count is populated with the actual number of
 *     mirror destinations on the port.
 */
int
bcm_petra_mirror_port_destination_get(int unit,
                                      bcm_port_t port,
                                      uint32 flags,
                                      int mirror_dest_size,
                                      bcm_gport_t *mirror_dest,
                                      int *mirror_dest_count,
                                      bcm_mirror_options_t *options)
{
    SOC_PPC_PORT loc_port;
    int is_created;
    uint32 found_profile = 0;
    int  core;
    uint32  pp_port;
    dpp_outbound_mirror_config_t        outbound_mirror_config;

    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS | BCM_MIRROR_PORT_UNTAGGED_ONLY | BCM_MIRROR_PORT_TAGGED_DEFAULT | BCM_MIRROR_PORT_EGRESS_ACL)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));

    *mirror_dest_count = 0;
    MIRROR_LOCK_TAKE;
    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO dflt_mirroring_info;

        /* get the destination profile */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_dflt_get((unit), core, pp_port, &dflt_mirroring_info));
        if (dflt_mirroring_info.is_tagged_dflt && dflt_mirroring_info.is_untagged_only) {
            /*port mirroring*/
            if ((found_profile = dflt_mirroring_info.tagged_dflt) != dflt_mirroring_info.untagged_dflt) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Inconsistent destination action profiles %u, %u for the port"),
                  (unsigned)found_profile, (unsigned)dflt_mirroring_info.untagged_dflt));
            }
        } else if (dflt_mirroring_info.is_tagged_dflt) {
            /*tagged mirroring*/
            found_profile = dflt_mirroring_info.tagged_dflt;
        } else if (dflt_mirroring_info.is_untagged_only) {
            /*untagged mirroring*/
            found_profile = dflt_mirroring_info.untagged_dflt;
        }
        /* check retrieved destination validity */
        if (!found_profile) {
            found_profile = BCM_MIRROR_INVALID_PROFILE;
        } else {
            if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
            }
            BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, found_profile, &is_created));
            if (!is_created) { /* verify that the destination already exists */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirroring destination ID %u is not created"), (unsigned)found_profile));
            }
            if (*mirror_dest_count >= mirror_dest_size) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("output size is too small")));
            }
        }
        BCM_GPORT_MIRROR_SET(mirror_dest[*mirror_dest_count], found_profile);
        ++*mirror_dest_count;
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        /* get the destination profile */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_dflt_get((unit), core, pp_port, &outbound_mirror_config));
        found_profile = outbound_mirror_config.mirror_command;
        /* check retrieved destination validity */
        if (!found_profile) {
            found_profile = BCM_MIRROR_INVALID_PROFILE;
        } else {
            if (found_profile > DPP_MIRROR_ACTION_NDX_MAX || found_profile < DPP_MIRROR_ACTION_NDX_MIN) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)found_profile));
            }

            BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, found_profile, &is_created));
            if (!is_created) { /* verify that the destination already exists */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirroring destination ID %u is not created"), (unsigned)found_profile));
            }
            if (*mirror_dest_count >= mirror_dest_size) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("output size is too small")));
            }
            options->copy_strength = outbound_mirror_config.mirror_strength;
            options->forward_strength = outbound_mirror_config.forward_strength;
        }
        BCM_GPORT_MIRROR_SET(mirror_dest[*mirror_dest_count], found_profile);
        ++*mirror_dest_count;
    }
    if (flags & BCM_MIRROR_PORT_EGRESS_ACL) { /* check if reassembly context and recycle channel are reserved for port for other applications */
        uint8 recycle_enabled;
        /* check if mirroring is already enabled */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_appl_get((unit), loc_port, &recycle_enabled));
        if (recycle_enabled) {
          BCM_GPORT_MIRROR_SET(mirror_dest[*mirror_dest_count], 0);
          ++*mirror_dest_count;
        }

    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;

}



/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_add
 *  Purpose
 *     extended versions of existing bcm_petra_mirror_port_vlan_dest_add API using a bcm_mirror_options_t argument
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_gport_t mirror_dest = mirror destination ID
 *     (in) bcm_mirror_options_t options = input the options for the mirroring of packets
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *  Assume only ingress mirroring.
 */
int
bcm_petra_mirror_port_vlan_destination_add(int unit,
                                           bcm_port_t port,
                                           bcm_vlan_t vlan,
                                           uint32 flags,
                                           bcm_gport_t mirror_dest,
                                           bcm_mirror_options_t options)
{

   uint32 action_profile_id;
    SOC_PPC_PORT loc_port;
    int is_created;
    SOC_SAND_SUCCESS_FAILURE success;
    uint32  pp_port;
    int core=SOC_CORE_INVALID;
    dpp_outbound_mirror_config_t        outbound_mirror_config ;

    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if(!SOC_DPP_PP_ENABLE(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("PP mode must be enabled")));
    }

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));

    /* check destination validity */
    if (!BCM_GPORT_IS_MIRROR(mirror_dest)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination has incorrect gport type")));
    }
    action_profile_id = BCM_GPORT_MIRROR_GET(mirror_dest);
    if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("specified mirror destination ID is out of range")));
    }
    MIRROR_LOCK_TAKE;
    BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
    if (!is_created) { /* verify that the destination already exists */
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("specified mirroring destination ID %u is not created"), (unsigned)action_profile_id));
    }

    if (vlan > SOC_SAND_PP_VLAN_ID_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN ID is out of range")));
    }

    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        uint32 mirror_profile;
        /* check that the port+vlan is not already mirrored */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_vlan_get((unit), core, pp_port, vlan, &mirror_profile));
        if (mirror_profile) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The inbound port-vlan combination is already mirrored")));
        }

        /* call dpp to set the port mirroring to the given profile */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_vlan_add((unit),core,
          pp_port, vlan, action_profile_id, &success));
        if (success != SOC_SAND_SUCCESS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("Failed adding an inbound port-vlan mirror destination")));
        }
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        /* check that the port+vlan is not already mirrored */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_vlan_get((unit), core, pp_port, vlan, &outbound_mirror_config));
        if (outbound_mirror_config.mirror_command) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_BUSY, (_BSL_BCM_MSG("The outbound port-vlan combination is already mirrored")));
        }

        /* call dpp to set the port mirroring to the given profile */
        outbound_mirror_config.mirror_command = action_profile_id;
        outbound_mirror_config.forward_en = 1;
        outbound_mirror_config.forward_strength = options.forward_strength;
        outbound_mirror_config.mirror_en = 1;
        outbound_mirror_config.mirror_strength = options.copy_strength;

        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_vlan_add((unit), core,
          pp_port, vlan, &outbound_mirror_config, &success));
        if (success != SOC_SAND_SUCCESS) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_FAIL, (_BSL_BCM_MSG("Failed adding an outbound port-vlan mirror destination, perhaps all outbound mirror vlans are used")));
        }
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;

}


/*
 *  Function
 *     bcm_petra_mirror_port_vlan_dest_get
 *  Purpose
 *     extended versions of existing bcm_petra_mirror_port_vlan_dest_get API using a bcm_mirror_options_t argument
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) bcm_port_t port = port to manipulate
 *     (in) uint32 flags = mirror control flags
 *     (in) int mirror_dest_size = size of provided buffer
 *     (out) bcm_gport_t *mirror_dest = pointer to buffer for destinations
 *     (out) int *mirror_dest_count = number of mirror dests
 *     (out) bcm_mirror_options_t *options = get the options for the mirroring of packets
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     The value written to mirror_dest_count will be the number of mirror
 *     destinations actually filled in mirror_dest, except if the value of
 *     mirror_dest_size is zero.  In this case, nothing is written to
 *     mirror_dest and mirror_dest_count is populated with the actual number of
 *     mirror destinations on the port.
 */
int
bcm_petra_mirror_port_vlan_destination_get(int unit,
                                    bcm_port_t port,
                                    bcm_vlan_t vlan,
                                    uint32 flags,
                                    uint32 mirror_dest_size,
                                    bcm_gport_t *mirror_dest,
                                    uint32 *mirror_dest_count,
                                    bcm_mirror_options_t *options)
{
    SOC_PPC_PORT loc_port;
    int is_created;
    int  core;
    uint32  pp_port;
    uint32 action_profile_id;
    dpp_outbound_mirror_config_t        outbound_mirror_config;

    MIRROR_LOCK_DEFS;
    BCMDNX_INIT_FUNC_DEFS;

    if(!SOC_DPP_PP_ENABLE(unit)){
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("PP mode must be enabled")));
    }

    if (flags & ~(BCM_MIRROR_PORT_INGRESS | BCM_MIRROR_PORT_EGRESS)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("An unsupported bcm mirror port flag was specified")));
    }

    if (vlan > SOC_SAND_PP_VLAN_ID_MAX) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("VLAN ID is out of range")));
    }

    /* translate port to local port*/
    BCMDNX_IF_ERR_EXIT(_bcm_petra_get_local_valid_port(unit, port, &loc_port));
    if (loc_port == _NON_LOCAL_PORT) {
        BCM_EXIT; /* If it is not a local port do nothing and return successfully */
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_pp_port_get, (unit, loc_port, &pp_port, &core)));


    *mirror_dest_count = 0;
    MIRROR_LOCK_TAKE;
    if (flags & BCM_MIRROR_PORT_INGRESS) { /* handle inbound mirroring */
        /* get the destination profile */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_llp_mirror_port_vlan_get((unit), core, pp_port, vlan, &action_profile_id));
        /* check retrieved destination validity */
        if (!action_profile_id) {
            action_profile_id = BCM_MIRROR_INVALID_PROFILE;
        } else {
            if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)action_profile_id));
            }
            BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
            if (!is_created) { /* verify that the destination already exists */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirroring destination ID %u is not created"), (unsigned)action_profile_id));
            }
            if (*mirror_dest_count >= mirror_dest_size) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("output size is too small")));
            }
        }
        BCM_GPORT_MIRROR_SET(mirror_dest[*mirror_dest_count], action_profile_id);
        ++*mirror_dest_count;
    }
    if (flags & BCM_MIRROR_PORT_EGRESS) { /* handle outbound mirroring */
        /* get the destination profile */
        BCM_SAND_IF_ERR_EXIT(soc_ppd_eg_mirror_port_vlan_get((unit), core, pp_port, vlan, &outbound_mirror_config));
        action_profile_id = outbound_mirror_config.mirror_command;
        /* check retrieved destination validity */
        if (!action_profile_id) {
            action_profile_id = BCM_MIRROR_INVALID_PROFILE;
        } else{
            if (action_profile_id > DPP_MIRROR_ACTION_NDX_MAX || action_profile_id < DPP_MIRROR_ACTION_NDX_MIN) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirror destination %u is out of range"),(unsigned)action_profile_id));
            }

            BCMDNX_IF_ERR_EXIT(_bcm_petra_is_mirror_dest_created(unit, action_profile_id, &is_created));
            if (!is_created) { /* verify that the destination already exists */
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("retrieved mirroring destination ID %u is not created"), (unsigned)action_profile_id));
            }
            if (*mirror_dest_count >= mirror_dest_size) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("output size is too small")));
            }
            options->copy_strength = outbound_mirror_config.mirror_strength;
            options->forward_strength = outbound_mirror_config.forward_strength;
        }
        BCM_GPORT_MIRROR_SET(mirror_dest[*mirror_dest_count], action_profile_id);
        ++*mirror_dest_count;
    }

exit:
    MIRROR_LOCK_GIVE;
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_petra_mirror_or_snoop_probability_get(uint32 dividend,
                                           uint32 divisor,
                                           int prob_bits)
{
    uint32 prob_field, prob_always = (prob_bits == JER_OUTBOUND_MIRROR_SNOOP_PROBABILITY_BITS ? JER_MIRROR_PROBABILITY_DIVISOR : DPP_MIRROR_SNOOP_PROBABILITY_ALLWAYS(prob_bits)); /* all 1's*/
    if (dividend >= divisor) {
            prob_field = prob_always; /* 100% probability of mirror execution */
    } else {
#if defined(COMPILER_HAS_LONGLONG_MUL) && !defined(__KERNEL__)
        /* Calculate using 64 bit multiplication and division */
        uint64 val64 = dividend;
        val64 = (val64 << prob_bits) + divisor / 2;
        val64 /= divisor;
        prob_field = val64;
#else /* COMPILER_HAS_LONGLONG_MUL */
        /* Calculate using 32 bit arithmetic division, result may be off by one */
        uint32 val32 = dividend; /* val32 contains the dividend here */
        unsigned shift_left = prob_bits;
        /* multiply (shift) the dividend, but stop before overflowing */
        while ((val32 & (((uint32)1)<<31)) == 0 && shift_left) {
          --shift_left;
          val32 <<=1;
        }
        prob_field = val32 + ((divisor/2) >> shift_left); /* add rounding (0.5 to field value) */
        if (prob_field < val32) {
             prob_field = (uint32)(-1); /* handle overflow */
        }
        prob_field /= shift_left ?
          ((divisor >> (shift_left - 1)) + 1) >> 1 : /* adjust the divisor to the shift, with rounding */
          divisor;
#endif /* COMPILER_HAS_LONGLONG_MUL */

        if (prob_field <= 1) {
            if (dividend) {
                prob_field = 1; /* do not disable mirroring with non zero probability */
            }
        } else if (prob_field > prob_always) {
            prob_field = prob_always; /* fix roundup error */
        } else {
            --prob_field;
        }
    }

    return prob_field;
}

/*
 *  Function
 *     bcm_petra_mirror_port_to_rcy_port_map_get
 *  Purpose
 *     get recycling port from mirror port
 *  Parameters
 *     (in) int unit = the unit number
 *     (in) uint32 flags = mirror control flags
 *     (in) bcm_port_t forward_port = the mirrored port
 *     (out) bcm_gport_t *mirror_dest = pointer to recycling port
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *    When a mirror port is configured using the mirror_destination_create API
 *    a recycle port is allocated dynamicaliy from a pool of free ports
 *    and the mirror port is mapped to this recycle port.
 */
int
bcm_petra_mirror_port_to_rcy_port_map_get(
    int unit,
    uint32 flags,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t *rcy_map_info)
{
    uint32 pp_port;
    int core;
    soc_port_t rcy_port;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(rcy_map_info);

    sal_memset(rcy_map_info, 0x0, sizeof(*rcy_map_info));

    /*get PP port from local port */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(
      unit, mbcm_dpp_local_to_pp_port_get,
        (unit, forward_port, &pp_port, &core)));

    /*get recycle port from forward port */
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(
      unit, mbcm_dpp_ports_mirror_to_rcy_port_get,
        (unit, core, pp_port, &rcy_port)));

    rcy_map_info->rcy_mirror_port = rcy_port;

exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_mirror_port_to_rcy_port_map_set(
    int unit,
    uint32 flags,
    bcm_port_t forward_port,
    bcm_mirror_port_to_rcy_map_info_t *rcy_map_info)
{
    /* Will be added in JR2 */
    return BCM_E_UNAVAIL;
}

