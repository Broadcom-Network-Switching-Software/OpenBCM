
/*
 * $Id:
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        flow_db.c
 * Purpose:     Flex flow Database utilities
 */


#include <sal/core/libc.h>
#include <shared/bsl.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/error.h>
#include <soc/esw/flow_db.h>
#include <soc/esw/flow_db_core.h>
#include <soc/esw/flow_db_enum.h>
#include <soc/esw/cancun.h>

/*
 * Function:
 *      soc_flow_db_flow_handle_get
 * Purpose:
 *      Get the flow handle
 * Parameters:
 *      unit           - (IN)     Unit.
 *      flow_name      - (IN)     Flow name.
 *      handle         - (IN/OUT) Flow hanlde.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_flow_handle_get(int unit,
                       const char *flow_name,
                       uint32 *handle)
{
    return _soc_flow_db_flow_handle_get(unit,
                             flow_name, handle);
}

/*
 * Function:
 *      soc_flow_db_mem_view_flow_handle_valid
 * Purpose:
 *      Verify the flow handle if valid
 * Parameters:
 *      unit             - (IN)     Unit
 *      flow_handle      - (IN)     Flow handle.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
 soc_flow_db_mem_view_flow_handle_valid(int unit,
                                   uint32 flow_handle)
{
     if ((SOC_FLOW_DB_FLOW_HANDLE_VALID(unit, flow_handle)) ||
         (flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_VXLAN) ||
         (flow_handle == SOC_FLOW_DB_FLOW_ID_LEGACY_L2GRE)) {
         return SOC_E_NONE;
     }
     return SOC_E_PARAM;
}
/*
 * Function:
 *      soc_flow_db_flow_option_id_get
 * Purpose:
 *      Get the Flow option ID.
 * Parameters:
 *      unit             - (IN)     Unit
 *      flow_handle      - (IN)     Flow handle.
 *      flow_option_name - (IN)     Flow option name.
 *      option_id        - (IN/OUT) Flow Option ID.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
 soc_flow_db_flow_option_id_get(int unit,
                           uint32 flow_handle,
                           const char *flow_option_name,
                           uint32 *option_id)
{

    if (!SOC_FLOW_DB_FLOW_HANDLE_VALID(unit, flow_handle)) {
        return SOC_E_PARAM;
    }

    return _soc_flow_db_flow_option_id_get(unit,
                                           flow_handle,
                                           flow_option_name,
                                           option_id);
}

/*
 * Function:
 *      soc_flow_db_ffo_to_mem_view_id_get
 * Purpose:
 *      Given a flow,function,option ID's
 *      get the memory view ID.
 * Parameters:
 *      unit                - (IN)     unit
 *      flow_handle         - (IN)     flow handle
 *      flow_option_id      - (IN)     flow option id.
 *      function_id         - (IN)     function id.
 *      mem_view_id         - (IN/OUT) list of mem view ID.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
 soc_flow_db_ffo_to_mem_view_id_get(int    unit,
                             uint32 flow_handle,
                             uint32 flow_option_id,
                             uint32 function_id,
                             uint32 *mem_view_id)
{
    uint32 option_id = 0;

    if (!SOC_FLOW_DB_FLOW_HANDLE_VALID(unit, flow_handle)) {
        return SOC_E_PARAM;
    }
    if (flow_option_id == SOC_FLOW_DB_FLOW_OP_MATCH_NONE) {
        SOC_IF_ERROR_RETURN(
                  _soc_flow_db_flow_option_id_get(
                         unit,
                         flow_handle,
                         (char *)SOC_FLOW_DB_FLOW_OP_ANY,
                         &option_id));
        flow_option_id = option_id;

    }
    if (!SOC_FLOW_DB_FLOW_OPTION_ID_VALID(unit, flow_option_id)) {
        return SOC_E_PARAM;
    }

    if (!SOC_FLOW_DB_FUNCTION_ID_VALID(unit, function_id)) {
        return SOC_E_PARAM;
    }

    return _soc_flow_db_ffo_to_mem_view_id_get(unit,
                                        flow_handle,
                                        function_id,
                                        flow_option_id,
                                        mem_view_id);
}

/*
 * Function:
 *      soc_flow_db_mem_to_view_id_get
 * Purpose:
 *      Given a memory , key_type, data_type and control_field
 *      list, get the memory view id.
 *
 * Parameters:
 *      unit                - (IN)     unit
 *      mem                 - (IN)     mem
 *      mem_type            - (IN)     memory type.
 *      key_type            - (IN)     key type for HASH/TCAM
 *      data_type           - (IN)     data type for indexed
 *      num_fields          - (IN)     num of control fields
 *      field_list          - (IN)     control field list
 *      mem_view_id         - (IN/OUT) mem view ID.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */

int
 soc_flow_db_mem_to_view_id_get(int                     unit,
                             uint32                     mem,
                             int                        key_type,
                             int                        data_type,
                             int                        num_fields,
                             soc_flow_db_ctrl_field_t   *field_list,
                             uint32                     *mem_view_id)
{
    if ((num_fields) && (field_list == NULL)) {
        return SOC_E_PARAM;
    }
    return _soc_flow_db_mem_to_view_id_get (unit,
                                  mem, key_type,
                                  data_type,
                                  num_fields,
                                  field_list,
                                  1,
                                  mem_view_id);
}

/*
 * Function:
 *      soc_flow_db_mem_to_view_id_multi_get
 * Purpose:
 *      Given a memory , key_type, data_type and control_field
 *      list, get a list of the memory view ids. 
 *
 * Parameters:
 *      unit                - (IN)     unit
 *      mem                 - (IN)     mem
 *      mem_type            - (IN)     memory type.
 *      key_type            - (IN)     key type for HASH/TCAM
 *      data_type           - (IN)     data type for indexed
 *      num_fields          - (IN)     num of control fields
 *      field_list          - (IN)     control field list
 *      num_ids             - (IN)     the size of the view id array
 *      mem_view_id         - (IN/OUT) view id array.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */

int
 soc_flow_db_mem_to_view_id_multi_get(int                     unit,
                             uint32                     mem,
                             int                        key_type,
                             int                        data_type,
                             int                        num_fields,
                             soc_flow_db_ctrl_field_t   *field_list,
                             int                        num_ids,
                             uint32                     *mem_view_id)
{
    if ((num_fields) && (field_list == NULL)) {
        return SOC_E_PARAM;
    }
    return _soc_flow_db_mem_to_view_id_get (unit,
                                  mem, key_type,
                                  data_type,
                                  num_fields,
                                  field_list,
                                  num_ids,
                                  mem_view_id);
}

/*
 * Function:
 *      soc_flow_db_mem_view_field_list_get
 * Purpose:
 *      Get the list of field IDs
 *      of specific type such as CONTROL/KEY/
 *      for a given mem_view ID, field_type parameter is
 *      a classifier.
 *
 *   Parameters
 *      unit           - (IN)     unit
 *      mem_view_id    - (IN)     Memory view ID.
 *      field_type     - (IN)     field type
 *      size           - (IN)     Max array size
 *      field_id       - (IN/OUT) Field ID list
 *      field_count    - (IN/OUT) field count
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_mem_view_field_list_get(int unit,
                                    uint32 mem_view_id,
                                    int field_type,
                                    int size,
                                    uint32  *field_id,
                                    uint32  *field_count)
{
    if ((field_id == NULL) || (size == 0)) {
        return SOC_E_PARAM;
    }
    return _soc_flow_db_mem_view_field_list_get(
                     unit, mem_view_id, field_type,
                      size, field_id, field_count);
}

/*
 * Function:
 *      soc_flow_db_mem_view_field_info_get
 * Purpose:
 *      Get a field information for a given mem_view ID and field ID.
 *   Parameters:
 *      unit          - (IN)  unit.
 *      view_id       - (IN)  Memory mem_view ID.
 *      field_id      - (IN)  Field ID.
 *      field_info    - (OUT) Field information
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_mem_view_field_info_get(int unit,
                                uint32 mem_view_id,
                                uint32 field_id,
                                soc_flow_db_mem_view_field_info_t *field_info)
{
    if (!SOC_FLOW_DB_VIEW_IDX_VALID(unit, mem_view_id)) {
        return SOC_E_PARAM;
    }

    return  _soc_flow_db_mem_view_field_info_get(
                       unit, mem_view_id, field_id,
                       field_info);
}

/*
 * Function:
 *      soc_flow_db_mem_view_split_field_info_get
 * Purpose:
 *      Get a field information for a given mem_view ID and field ID.
 *   Parameters:
 *      unit          - (IN)  unit.
 *      view_id       - (IN)  Memory mem_view ID.
 *      field_id      - (IN)  Field ID.
 *      field_info    - (OUT) Field information
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_mem_view_split_field_info_get(int unit,
                                uint32 mem_view_id,
                                uint32 field_id,
                                soc_flow_db_mem_view_split_field_info_t *sf_info)
{
    if (!SOC_FLOW_DB_VIEW_IDX_VALID(unit, mem_view_id)) {
        return SOC_E_PARAM;
    }

    return  _soc_flow_db_mem_view_split_field_info_get(
                       unit, mem_view_id, field_id,
                       sf_info);
}
/*
 * Function:
 *      soc_flow_db_mem_view_info_get
 * Purpose:
 *      Get a mem view information for a given memory view ID
 *   Parameters:
 *      mem_view_id       - (IN)  Physical table view ID.
 *      mem_view_info     - (OUT) Physical table view information.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_mem_view_info_get(int unit,
                          int mem_view_id,
                          soc_flow_db_mem_view_info_t *mem_view_info)
{

    if (!SOC_FLOW_DB_VIEW_IDX_VALID(unit, mem_view_id)) {
        return SOC_E_PARAM;
    }

    return   _soc_flow_db_mem_view_info_get(unit,
                                       mem_view_id,
                                       mem_view_info);
}

/*
 * Function:
 *      soc_flow_db_mem_view_logical_field_id_get
 * Purpose:
 *      Given flow handle and logical field name
 *      get the  field ID
 *   Parameters:
 *      unit                -  (IN)    unit.
 *      flow_handle         -  (IN)    flow_handle.
 *      logical_field_name  -  (IN)    logical field Name.
 *      field ID            -  (OUT)   field ID.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_mem_view_logical_field_id_get(int unit,
                                    uint32 flow_handle,
                                    const char *logical_field_name,
                                    uint32  *field_id)
{
    if (!SOC_FLOW_DB_FLOW_HANDLE_VALID(unit, flow_handle)) {
        return SOC_E_PARAM;
    }
    return _soc_flow_db_mem_view_logical_field_id_get(unit, flow_handle,
                                                 logical_field_name,
                                                 field_id);
}

/*
 * Function:
 *      soc_flow_db_mem_view_field_is_valid
 * Purpose:
 *      Check if field ID is valid for the view.
 *   Parameters:
 *      unit                -  (IN)    unit.
 *      mem_view_id         -  (IN)    View ID.
 *      field ID            -  (IN)    field ID.
 * Returns:
 *      TRUE/FALSE
 */
int
soc_flow_db_mem_view_field_is_valid(int unit,
                                uint32 mem_view_id,
                                uint32 field_id)
{
   int rv;

   rv = _soc_flow_db_mem_view_field_is_valid(unit,
                         mem_view_id, field_id);
   if (rv) {
       return TRUE;
   } else {
       return FALSE;
   }
}

/*
 * Function:
 *      soc_flow_db_mem_view_field_is_virtual
 * Purpose:
 *      Check if field ID is a virtual field for the view.
 *   Parameters:
 *      unit                -  (IN)    unit.
 *      mem_view_id         -  (IN)    View ID.
 *      field ID            -  (IN)    field ID.
 * Returns:
 *      TRUE/FALSE
 */
int
soc_flow_db_mem_view_field_is_virtual(int unit,
                                uint32 mem_view_id,
                                uint32 field_id)
{
   int rv;

   rv = _soc_flow_db_mem_view_field_is_valid(unit,
                         mem_view_id, field_id);
   if (rv == SOC_FLOW_DB_FIELD_VIRTUAL_VALID) {
       return TRUE;
   } else {
       return FALSE;
   }
}

/*
 * Function:
 *      soc_flow_db_mem_view_is_valid
 * Purpose:
 *      Check if view index  is valid.
 *   Parameters:
 *      unit                -  (IN)    unit.
 *      mem_view_id         -  (IN)    View ID.
 * Returns:
 *      TRUE/FALSE
 */
int
soc_flow_db_mem_view_is_valid(int unit,
                                uint32 mem_view_id)
{
    return ((SOC_FLOW_DB_VIEW_IDX_VALID(unit, mem_view_id)) ?
            TRUE : FALSE);
}


/*
 * Function:
 *      soc_flow_db_mem_view_entry_init
 * Purpose:
 *      Initialize the entry buffer.
 * Parameters:
 *      unit           - (IN)     Unit
 *      mem_view_id    - (IN)     Memory view ID.
 *      entry_buf      - (IN/OUT) View entry buffer
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
 soc_flow_db_mem_view_entry_init(int unit,
                                 uint32 mem_view_id,
                                 uint32 *entry_buf)
{
    uint32 size = SOC_FLOW_DB_MAX_VIEW_FIELDS;
    uint32 fields[SOC_FLOW_DB_MAX_VIEW_FIELDS];
    uint32 field_count = 0;
    int i = 0;
    soc_flow_db_mem_view_field_info_t field_info;

    if (entry_buf == NULL) {
        return SOC_E_PARAM;
    }

    if (!SOC_FLOW_DB_VIEW_IDX_VALID(unit, mem_view_id)) {
        return SOC_E_PARAM;
    }

    /*Initialize the control fields with value */
    SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_field_list_get(
                                unit,
                                mem_view_id,
                                SOC_FLOW_DB_FIELD_TYPE_CONTROL,
                                size,
                                fields,
                                &field_count));
    for (i = 0; i < field_count; i++) {
        SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_field_info_get(
                             unit,
                             mem_view_id,
                             fields[i],
                             &field_info));
        soc_mem_field32_set(unit,
                             mem_view_id,
                             entry_buf,
                             fields[i],
                             field_info.value);

    }
    /*Initialize the valid fields with value */
    SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_field_list_get(
                                unit,
                                mem_view_id,
                                SOC_FLOW_DB_FIELD_TYPE_VALID,
                                size,
                                fields,
                                &field_count));
    for (i = 0; i < field_count; i++) {
        SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_field_info_get(
                             unit,
                             mem_view_id,
                             fields[i],
                             &field_info));
        soc_mem_field32_set(unit,
                             mem_view_id,
                             entry_buf,
                             fields[i],
                             field_info.value);

    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_flow_db_mem_view_entry_is_valid
 * Purpose:
 *      Check if flex view entry is valid
 * Parameters:
 *      unit           - (IN)     Unit
 *      mem_view_id    - (IN)     Memory view ID.
 *      entry_buf      - (IN/OUT) View entry buffer
 * Returns:
 *      TRUE
 *      FALSE
 */
int
 soc_flow_db_mem_view_entry_is_valid(int unit,
                                 uint32 mem_view_id,
                                 uint32 *entry_buf)
{
    uint32 size = SOC_FLOW_DB_MAX_VIEW_FIELDS;
    uint32 fields[SOC_FLOW_DB_MAX_VIEW_FIELDS];
    uint32 field_count = 0;
    int i = 0;
    soc_flow_db_mem_view_field_info_t field_info;
    uint32 value = 0;

    if (entry_buf == NULL) {
        return SOC_E_PARAM;
    }

    if (!SOC_FLOW_DB_VIEW_IDX_VALID(unit, mem_view_id)) {
        return SOC_E_PARAM;
    }

    /* Check the valid field values */
    SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_field_list_get(
                                unit,
                                mem_view_id,
                                SOC_FLOW_DB_FIELD_TYPE_VALID,
                                size,
                                fields,
                                &field_count));
    for (i = 0; i < field_count; i++) {
        SOC_IF_ERROR_RETURN(soc_flow_db_mem_view_field_info_get(
                             unit,
                             mem_view_id,
                             fields[i],
                             &field_info));
        value = soc_mem_field32_get(unit,
                             mem_view_id,
                             entry_buf,
                             fields[i]);
        if (value != field_info.value) {
            return FALSE;
        }

    }
    return TRUE;
}

/*
 * Function:
 *      soc_flow_db_mem_view_to_ffo_get
 * Purpose:
 *      Given a memory view ID get the ffo tuple.
 * Parameters:
 *      unit                - (IN)     unit
 *      mem_view_id         - (IN)     list of mem view ID.
 *      size                - (IN)     Max array size
 *      ffo                 - (IN/OUT) ffo tuple
 *      num_ffo_entries     - (IN/OUT) num ffo tuples
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_mem_view_to_ffo_get(int  unit,
                            uint32 mem_view_id,
                            uint32 size,
                            soc_flow_db_ffo_t *ffo,
                            uint32 *num_ffo_entries)
{
    if ((size == 0) ||
        (ffo == NULL) ||
        (num_ffo_entries == NULL)) {
        return SOC_E_PARAM;
    }

    if (!SOC_FLOW_DB_VIEW_IDX_VALID(unit, mem_view_id)) {
        return SOC_E_PARAM;
    }

    SOC_IF_ERROR_RETURN(
           _soc_flow_db_mem_view_to_ffo_get(unit, mem_view_id,
                                size, ffo, num_ffo_entries));

    return SOC_E_NONE;

}

/*
 * Function:
 *      soc_flow_db_mem_view_field_name_get
 * Purpose:
 *      Get the fleld name for view ID and field ID.
 * Parameters:
 *      unit            - (IN)     Unit.
 *      mem_view_id     - (IN)     View ID.
 *      field ID        - (IN)     field ID.
 *      field_name      - (IN/OUT) Field name.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_mem_view_field_name_get(int unit,
                       uint32 mem_view_id,
                       uint32 field_id,
                       char **field_name)
{

    if (!SOC_FLOW_DB_VIEW_IDX_VALID(unit, mem_view_id)) {
        return SOC_E_PARAM;
    }

    return _soc_flow_db_mem_view_field_name_get(
                       unit, mem_view_id, field_id,
                       field_name);

}

/*
 * Function:
 *      soc_flow_db_mem_view_mem_name_get
 * Purpose:
 *      Get the memory name for view ID.
 * Parameters:
 *      unit            - (IN)     Unit.
 *      mem_view_id     - (IN)     View ID.
 *      mem_name        - (IN/OUT) Physical memory name.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_mem_view_mem_name_get(int unit,
                       uint32 mem_view_id,
                       char **mem_name)
{

    if (!SOC_FLOW_DB_VIEW_IDX_VALID(unit, mem_view_id)) {
        return SOC_E_PARAM;
    }

    return _soc_flow_db_mem_view_mem_name_get(
                       unit, mem_view_id, mem_name);
}


/*
 * Function:
 *      soc_flow_db_mem_view_option_name_get
 * Purpose:
 *      Get option name given flow handle and option ID.
 * Parameters:
 *      unit               - (IN)     Unit.
 *      flow_handle        - (IN)     Flow handle.
 *      flow_option_id     - (IN)     Flow option ID.
 *      option_name        - (IN/OUT) Flow option name.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
int
soc_flow_db_mem_view_option_name_get(int unit,
                       uint32 flow_handle,
                       uint32 flow_option_id,
                       char **option_name)
{

    if (!SOC_FLOW_DB_FLOW_HANDLE_VALID(unit, flow_handle)) {
        return SOC_E_PARAM;
    }

    if (!SOC_FLOW_DB_FLOW_OPTION_ID_VALID(unit, flow_option_id)) {
        return SOC_E_PARAM;
    }

    return _soc_flow_db_mem_view_option_name_get(unit,
                            flow_handle,
                            flow_option_id,
                            option_name);

}

