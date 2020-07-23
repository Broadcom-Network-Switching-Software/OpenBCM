/*! \file bcmfp_strings_internal.h
 *
 *  APIs to print meaningful names to different
 *  enumerations in BCMFP. These APIs can be used
 *  in LOG messages.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_STRINGS_INTERNAL_H
#define BCMFP_STRINGS_INTERNAL_H

#include <bcmfp/bcmfp_stage_internal.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>

extern char *
bcmfp_stage_string(bcmfp_stage_id_t stage_id);

extern char *
bcmfp_group_mode_string(bcmfp_group_mode_t group_mode);

#endif /* BCMFP_STRINGS_INTERNAL_H */
