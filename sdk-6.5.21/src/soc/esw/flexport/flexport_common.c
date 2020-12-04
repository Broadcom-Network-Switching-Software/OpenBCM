/*
* 
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2020 Broadcom Inc. All rights reserved.
*
* $File:  flexport_common.c
*/


#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/defs.h>
#include <soc/mem.h>
#include <soc/esw/port.h>
#include <soc/tdm/core/tdm_top.h>


#include <soc/flexport/flexport_common.h>

/*! @file flexport_common.c
 *   @brief FlexPort common (across chips) functions and structures.
 *   Details are shown below.
 */


/*! @fn int soc_detach_flex_phase(int unit, phase_callback_type *flex_phases,
 *                phase_callback_type phase, int *phase_pos)
 *   @param unit Chip unit number.
 *   @param flex_phases Pointer to the array holding the callbacks implementing
 *          FlexPort phases.
 *   @param phase The callback implementing the phase to be detached.
 *   @param phase_pos Position of the detached callback in the array is
 *          returned
 *          in this variable.
 *   @brief Detaches a phase from the FlexPort operations implemented with a
 *          callback function. Detaches the first instance of the callback.
 */
int
soc_detach_flex_phase (
    int                  unit,
    phase_callback_type *flex_phases,
    phase_callback_type  phase,
    int                 *phase_pos
    )
{
    int i;

    *phase_pos = -1;
    for (i = 0; i < MAX_FLEX_PHASES; i++) {
        if (flex_phases[i] == phase) {
            flex_phases[i] = NULL;
            *phase_pos = i;
            break;
        }
    }

    if (*phase_pos < 0) {
        LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit, "Phase not found, noting detached!\n")));
        return SOC_E_NONE;
    }

    return SOC_E_NONE;
}


/*! @fn int soc_attach_flex_phase(int unit, phase_callback_type *flex_phases,
 *               phase_callback_type phase, int phase_pos)
 *   @param unit Chip unit number.
 *   @param flex_phases Pointer to the array holding the callbacks implementing
 *          FlexPort phases.
 *   @param phase The callback implementing the phase to be attached.
 *   @param phase_pos Position where the callback be attached in the array.
 *   @brief Attaches a phase in the FlexPort operations implemented with a
 *          callback function.
 */
int
soc_attach_flex_phase (
    int                  unit,
    phase_callback_type *flex_phases,
    phase_callback_type  phase,
    int                  phase_pos
    )
{
    if (phase_pos < 0 || phase_pos >= MAX_FLEX_PHASES) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                       "Illegal phase position; "
                       "must be in the 0 .. %0d range!\n"),
                   MAX_FLEX_PHASES-1));
        return SOC_E_FAIL;
    }

    flex_phases[phase_pos] = phase;

    return SOC_E_NONE;
}


/*! @fn int soc_check_flex_phase(int unit, phase_callback_type *flex_phases,
 *               phase_callback_type phase, int *present)
 *   @param unit Chip unit number.
 *   @param flex_phases Pointer to the array holding the callbacks implementing
 *          FlexPort phases.
 *   @param phase The presence of callback phase to be checked.
 *   @param present Set to 1 if the callback exists, 0 otherwise.
 *   @brief Checks whether a phase callback function exists in the phase array.
 */
int
soc_check_flex_phase (
    int                  unit,
    phase_callback_type *flex_phases,
    phase_callback_type  phase,
    int                 *present
    )
{
    int i;

    *present = -1;
    for (i = 0; i < MAX_FLEX_PHASES; i++) {
        if (flex_phases[i] == phase) {
            *present = 1;
            break;
        }
    }

    if (*present < 0) {
        *present = 0;
    }

    return SOC_E_NONE;
}
