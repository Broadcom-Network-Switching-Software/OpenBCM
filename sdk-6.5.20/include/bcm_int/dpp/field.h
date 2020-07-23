/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Field Processor APIs
 *
 * Purpose:
 *     'Field Processor' (FP) API for Dune Soc_petra using PPD
 */
#ifndef _DPP_PETRA_FIELD_H_
#define _DPP_PETRA_FIELD_H_

#include <bcm/field.h>
#include <soc/dpp/PPD/ppd_api_fp.h>


/******************************************************************************
 *
 *  Exports to submodules (see field module for code comments)
 */

extern CONST char* CONST _bcm_dpp_field_qual_name[bcmFieldQualifyCount];
extern CONST char* CONST _bcm_dpp_field_action_name[bcmFieldActionCount];
extern CONST char* CONST _bcm_dpp_field_group_mode_name[bcmFieldGroupModeCount];
extern CONST char* CONST _bcm_dpp_field_stats_names[bcmFieldStatCount];
extern CONST char* CONST _bcm_dpp_field_control_names[bcmFieldControlCount];
extern CONST char* CONST _bcm_dpp_field_data_offset_base_names[bcmFieldDataOffsetBaseCount];
extern CONST char* CONST _bcm_dpp_field_header_format_names[bcmFieldHeaderFormatCount];
extern CONST char* CONST _bcm_dpp_field_header_format_extension_names[bcmFieldHeaderFormatExtensionCount];
extern CONST char* CONST _bcm_dpp_field_entry_type_names[];
extern CONST char* CONST _bcm_dpp_field_stage_name[bcmFieldStageCount];

/******************************************************************************
 *
 *  Exports from submodules (see submodules for code comments)
 */


#endif /* ndef _DPP_PETRA_FIELD_H_ */

