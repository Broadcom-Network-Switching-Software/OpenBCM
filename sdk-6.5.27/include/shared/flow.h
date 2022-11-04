/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This file defines FLOW constants
 *
 * Its contents are not used directly by applications; it is used only
 * by the main flow header file.
 */

#ifndef _SHR_FLOW_H
#define _SHR_FLOW_H

/* SPECIAL_FIELD_XXX macros: sets the field id, field value and update nof special fields */

/* add uint32 data field */
#define _SHR_FLOW_SPECIAL_FIELDS_UINT32_ADD(special_fields, field_id, uint32_data) \
{ \
    special_fields->special_fields[special_fields->actual_nof_special_fields].field_id = field_id; \
    special_fields->special_fields[special_fields->actual_nof_special_fields].shr_var_uint32 = uint32_data; \
    special_fields->actual_nof_special_fields++; \
}

/* add uint32 arr data field */
#define _SHR_FLOW_SPECIAL_FIELDS_UINT32_ARR_ADD(special_fields, field_id, uint32_arr_data) \
{ \
    special_fields->special_fields[special_fields->actual_nof_special_fields].field_id = field_id; \
    sal_memcpy(special_fields->special_fields[special_fields->actual_nof_special_fields].shr_var_uint32_arr, uint32_arr_data,\ sizeof(BCM_FLOW_SPECIAL_FIELD_UIN32_ARR_MAX_SIZE)*sizeof(uint32)); \
    special_fields->actual_nof_special_fields++; \
}

/* add symbol data field */
#define _SHR_FLOW_SPECIAL_FIELD_SYMBOL_ADD(special_fields_struct, field_id, symbol_data) \
{ \
    special_fields->special_fields[special_fields->actual_nof_special_fields].field_id = field_id; \
    special_fields->special_fields[special_fields->actual_nof_special_fields].symbol = symbol_data; \
    special_fields->actual_nof_special_fields++; \
}

/* add uint8 arr data field */
#define _SHR_FLOW_SPECIAL_FIELDS_UINT8_ARR_ADD(special_fields_struct, field_id, uint8_arr_data) \
{ \
    special_fields->special_fields[special_fields->actual_nof_special_fields].field_id = field_id; \
    sal_memcpy(special_fields->special_fields[special_fields->actual_nof_special_fields].shr_var_uint8_arr, uint8_arr_data, sizeof(BCM_FLOW_SPECIAL_FIELD_UIN8_ARR_MAX_SIZE)); \
    special_fields->actual_nof_special_fields++; \
}

/* clear a special field, should be used for replace operation */
#define _SHR_FLOW_SPECIAL_FIELDS_CLEAR(special_fields, field_id) \
{ \
    special_fields->special_fields[special_fields->actual_nof_special_fields].field_id = field_id; \
    special_fields->special_fields[special_fields->actual_nof_special_fields].is_clear = TRUE; \
    special_fields->actual_nof_special_fields++; \
}


#endif  /* _SHR_FLOW_H */
