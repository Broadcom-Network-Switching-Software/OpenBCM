/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Mirror - Broadcom Mirror API.
 */
#ifndef _DPP_PETRA_MIRROR_H_
#define _DPP_PETRA_MIRROR_H_
#include <bcm/mirror.h>
#include <shared/swstate/sw_state.h>
#include <soc/dpp/ARAD/arad_api_action_cmd.h>

/******************************************************************************
 *
 *  Configuration
 */

/*
 *  Controls whether certain (perhaps excessively) verbose messages will be
 *  emitted by the diagnostics.  These messages are the FIELD_EVERB messages,
 *  which will be emitted at VVERB level if enabled.
 */
#define _BCM_PETRA_MIRROR_EXCESS_VERBOSITY      TRUE

/******************************************************************************
 *
 *  Module-wide definitions
 */

/* number of supported ingress mirror templates */
#define SOC_PETRA_IGR_MIRRORS(_unit) 16
#define BCM_PETRA_MIRROR_MODE_PORT_SET 1
#define BCM_PETRA_MIRROR_MODE_DESTINATION_CREATE 0

/*
 *  Describes a single mirror
 */
typedef struct _bcm_petra_mirror_data_s {
    unsigned int refCount;                  /* references to this template */
    SOC_TMC_ACTION_CMD_SNOOP_MIRROR_INFO mirrorInfo;   /* mirror information */
} _bcm_petra_mirror_data_t;

/*
 *  Describes the state per unit for mirroring
 */
typedef struct _bcm_petra_mirror_unit_data_s {
    unsigned int ingressCount;                          /* ingress mirrors */
    PARSER_HINT_ARR _bcm_petra_mirror_data_t *ingress;  /* ingr mirror info */
} _bcm_petra_mirror_unit_data_t;



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
extern int
bcm_petra_mirror_detach(int unit);

/*
 *  Function
 *    bcm_petra_ingress_mirror_alloc
 *  Purpose
 *    Allocate an ingress mirror template according to the parameters
 *  Arguments
 *    (in) int unit = unit number
 *    (in) bcm_module_t myModule = the local unit's module ID
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
 *
 *    On success, this increments the use count for the returned template.
 *
 *    The change function is an optimisation more than anything else.  In the
 *    simple case, the alloc function will suffice.  However, alloc new
 *    template then free old template will not work if there are no free
 *    templates; change can work if there are no free templates but the one
 *    being changed has only one reference.
 */
extern int
bcm_petra_ingress_mirror_alloc(int unit,
                               bcm_module_t myModule,
                               bcm_module_t destMod,
                               bcm_port_t destPort,
                               int *mirrorId);

/*
 *  Function
 *    bcm_petra_ingress_mirror_reference
 *  Purpose
 *    Add a reference to an ingress mirror template
 *  Arguments
 *    (in) int unit = unit number
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
extern int
bcm_petra_ingress_mirror_reference(int unit,
                                   int mirrorId);

/*
 *  Function
 *    bcm_petra_ingress_mirror_free
 *  Purpose
 *    Free a specified ingress mirror
 *  Arguments
 *    (in) int unit = unit number
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
extern int
bcm_petra_ingress_mirror_free(int unit,
                              int mirrorId);

/*
 *  Function
 *    bcm_petra_ingress_mirror_change
 *  Purpose
 *    Change an ingress mirror
 *  Arguments
 *    (in) int unit = unit number
 *    (in) int oldMirrorId = old ingress mirror ID
 *    (in) bcm_module_t myModule = the local unit's module ID
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
 *
 *    The change function is an optimisation more than anything else.  In the
 *    simple case, the alloc function will suffice.  However, alloc new
 *    template then free old template will not work if there are no free
 *    templates; change can work if there are no free templates but the one
 *    being changed has only one reference.
 */
extern int
bcm_petra_ingress_mirror_change(int unit,
                                int oldMirrorId,
                                bcm_module_t myModule,
                                bcm_module_t destMod,
                                bcm_port_t destPort,
                                int *newMirrorId);

/*
 *  Function
 *    _bcm_petra_ingress_mirror_get
 *  Purpose
 *    Get information about an ingress mirror
 *  Arguments
 *    (in) int unit = unit number
 *    (in) int mirrorId = ingress mirror ID
 *    (out) bcm_module_t *destMod = where to put destination module
 *    (out) bcm_port_t *destPort = where to put destination port
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    This will only edit the template in-place if there is exactly one
 *    reference to it.  If there is more than one reference to the template, it
 *    will allocate a replacement template and provide that, releasing one
 *    reference to the old template.
 */
extern int
bcm_petra_ingress_mirror_get(int unit,
                             int mirrorId,
                             bcm_module_t *destMod,
                             bcm_port_t *destPort);


/*  Function
 *     bcm_petra_mirror_destination_tunnel_create
 *  Purpose
 *     build ERSPAN/SPAN tunnel on [given] outlif
 *     - In case mirror destination is over ip tunnel with/out ERSPAN.
 *       build ERSPAN tunnel if needed, and make mirror profile points to erspan outlif
 *     - build RSPAN tunnel
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
bcm_petra_mirror_destination_tunnel_create(
   int unit,
   bcm_mirror_destination_t *mirror_dest,
   uint32  action_profile_id,
   int *global_outlif,
   uint8 *outlif_valid,
   uint8 update);


/*creates mirror or snoop profile and sets it. dest is set if $set_dest*/
int _bcm_petra_mirror_or_snoop_destination_create(int unit,bcm_mirror_destination_t *mirror_dest, uint8 set_dest);

#endif /* ndef _DPP_PETRA_MIRROR_H_ */
