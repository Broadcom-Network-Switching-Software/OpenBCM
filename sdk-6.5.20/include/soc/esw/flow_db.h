/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    flow_db.h
 * Purpose: Header file for flow database module
 */
#ifndef  FLOW_DB_H
#define FLOW_DB_H
#include <soc/esw/flow_db_enum.h>

typedef enum soc_flow_db_mem_view_field_type_e {
    SOC_FLOW_DB_FIELD_TYPE_INVALID = 0,
    SOC_FLOW_DB_FIELD_TYPE_CONTROL,
    SOC_FLOW_DB_FIELD_TYPE_KEY,
    SOC_FLOW_DB_FIELD_TYPE_VALID,
    SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA,
    SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA_ENCODING,
    SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY,
    SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA,
    SOC_FLOW_DB_FIELD_TYPE_DATA_FRAGMENT,
    SOC_FLOW_DB_FIELD_TYPE_MAX
} soc_flow_db_mem_view_field_type_t;

#define BCM_FEILD_TYPE_NAME_INITIALIZER \
{ \
    "SOC_FLOW_DB_FIELD_TYPE_INVALID",\
    "SOC_FLOW_DB_FIELD_TYPE_CONTROL",\
    "SOC_FLOW_DB_FIELD_TYPE_KEY",\
    "SOC_FLOW_DB_FIELD_TYPE_VALID",\
    "SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA",\
    "SOC_FLOW_DB_FIELD_TYPE_POLICY_DATA_ENCODING",\
    "SOC_FLOW_DB_FIELD_TYPE_LOGICAL_KEY",\
    "SOC_FLOW_DB_FIELD_TYPE_LOGICAL_POLICY_DATA",\
    "SOC_FLOW_DB_FIELD_TYPE_DATA_FRAGMENT",\
    "SOC_FLOW_DB_FIELD_TYPE_MAX",\
}


typedef enum  soc_flow_db_mem_view_type_e {
    SOC_FLOW_DB_VIEW_TYPE_INVALID = 0,
    SOC_FLOW_DB_VIEW_TYPE_DIRECT,
    SOC_FLOW_DB_VIEW_TYPE_HASH,
    SOC_FLOW_DB_VIEW_TYPE_TCAM,
    SOC_FLOW_DB_VIEW_TYPE_MAX
} soc_flow_db_mem_view_type_t;


typedef struct soc_flow_db_mem_view_field_info_s {
    soc_field_t  field_id;  /* SOC or SYSTEM unique field ID*/
    soc_flow_db_mem_view_field_type_t  type;
    uint16  offset;    /* Least bit position of the field */
    uint16  v_offset;  /* offset within the virtual field */
    uint16  width;     /* Bits in field */
    uint16  flags;     /* Logical OR of SOCF_* */
    uint32  value;  /* Value for fixed fields */
} soc_flow_db_mem_view_field_info_t;

typedef struct soc_flow_db_mem_view_split_field_info_s {
    int width;   /* user field width */
    int num_fld; /* number of split fields */
    soc_flow_db_mem_view_field_info_t field[6]; /* split field 
                                                   info */
} soc_flow_db_mem_view_split_field_info_t;

typedef struct soc_flow_db_mem_view_info_s {
    uint32 mem_view_id; /* DB mem_view ID */
    soc_mem_t mem;      /* soc mem ID */
    uint32  width;      /* Entry Width */
    soc_flow_db_mem_view_type_t type;
} soc_flow_db_mem_view_info_t;

typedef struct soc_flow_db_ctrl_field_s{
    soc_field_t field_id;
    uint32 value;
} soc_flow_db_ctrl_field_t;

typedef struct soc_flow_db_ffo_s {
    uint32 flow_handle;  /* flow_handle */
    uint32 function_id;  /* function_id */
    uint32 option_id;    /* option_id */
} soc_flow_db_ffo_t;

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

extern int
soc_flow_db_flow_handle_get(int unit,
                       const char *flow_name,
                       uint32 *handle);
/*
 * Function:
 *      soc_flow_db_flow_option_id_get
 * Purpose:
 *      Get the Flow option ID.
 * Parameters:
 *      unit             - (IN)     Unit
 *      flow_handle      - (IN)     Flow hanlde.
 *      flow_option_name - (IN)     Flow option name.
 *      option_id        - (IN/OUT) Flow Option ID.
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
extern int
 soc_flow_db_flow_option_id_get(int unit,
                           uint32 flow_handle,
                           const char *flow_option_name,
                           uint32 *option_id);
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

extern int
 soc_flow_db_mem_view_entry_init(int unit,
                                 uint32 mem_view_id,
                                 uint32 *entry_buf);
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
extern int
 soc_flow_db_ffo_to_mem_view_id_get(int    unit,
                             uint32 flow_handle,
                             uint32 flow_option_id,
                             uint32 function_id,
                             uint32 *mem_view_id);

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
extern int
 soc_flow_db_mem_view_to_ffo_get(int  unit,
                             uint32 mem_view_id,
                             uint32 size,
                             soc_flow_db_ffo_t *ffo,
                             uint32 *num_ffo_entries);


/*
 * Function:
 *      soc_flow_db_mem_to_view_id_get
 * Purpose:
 *      Given a memory , key_type, data_type and control_field
 *      list get the view id.
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
extern int
 soc_flow_db_mem_to_view_id_get(int                     unit,
                             uint32                     mem,
                             int                        key_type,
                             int                        data_type,
                             int                        num_fields,
                             soc_flow_db_ctrl_field_t   *field_list,
                             uint32                     *mem_view_id);

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

extern int
 soc_flow_db_mem_to_view_id_multi_get(int                     unit,
                             uint32                     mem,
                             int                        key_type,
                             int                        data_type,
                             int                        num_fields,
                             soc_flow_db_ctrl_field_t   *field_list,
                             int                        num_ids,
                             uint32                     *mem_view_id);

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
 *      field_count    - (IN.OUT) field count
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
extern int
soc_flow_db_mem_view_field_list_get(int unit,
                                    uint32 mem_view_id,
                                    int field_type,
                                    int size,
                                    uint32  *field_id,
                                    uint32  *field_count);
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
extern int
soc_flow_db_mem_view_field_info_get(int unit,
                                uint32 mem_view_id,
                                uint32 field_id,
                                soc_flow_db_mem_view_field_info_t *field_info);

/*
 * Function:
 *      soc_flow_db_mem_view_split_field_info_get
 * Purpose:
 *      Get a split field information for a given mem_view ID and field ID.
 *   Parameters:
 *      unit          - (IN)  unit.
 *      view_id       - (IN)  Memory mem_view ID.
 *      field_id      - (IN)  Field ID.
 *      sf_info       - (OUT) split field information
 * Returns:
 *      SOC_E_NONE
 *      SOC_E_XXX
 */
extern int
soc_flow_db_mem_view_split_field_info_get(int unit,
                                uint32 mem_view_id,
                                uint32 field_id,
                            soc_flow_db_mem_view_split_field_info_t *sf_info);

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
extern int
soc_flow_db_mem_view_info_get(int unit,
                          int mem_view_id,
                          soc_flow_db_mem_view_info_t *mem_view_info);



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
extern int
soc_flow_db_mem_view_logical_field_id_get(int unit,
                                    uint32 flow_handle,
                                    const char *logical_field_name,
                                    uint32  *field_id);

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
extern int
soc_flow_db_mem_view_field_is_valid(int unit,
                                uint32 mem_view_id,
                                uint32 field_id);
extern int
soc_flow_db_mem_view_field_is_virtual(int unit,
                                uint32 mem_view_id,
                                uint32 field_id);

/*
 * Function:
 *      soc_flow_db_mem_view_is_valid
 * Purpose:
 *      Check if view index is valid.
 *   Parameters:
 *      unit                -  (IN)    unit.
 *      mem_view_id         -  (IN)    View ID.
 * Returns:
 *      TRUE/FALSE
 */
extern int
soc_flow_db_mem_view_is_valid(int unit,
                              uint32 mem_view_id);


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

extern int
soc_flow_db_mem_view_field_name_get(int unit,
                       uint32 mem_view_id,
                       uint32 field_id,
                       char **field_name);
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

extern int
soc_flow_db_mem_view_mem_name_get(int unit,
                       uint32 mem_view_id,
                       char **mem_name);

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

extern int
soc_flow_db_mem_view_option_name_get(int unit,
                       uint32 flow_handle,
                       uint32 flow_option_id,
                       char **option_name);
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
extern int
soc_flow_db_mem_view_entry_is_valid(int unit,
                                 uint32 mem_view_id,
                                 uint32 *entry_buf);
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

extern int
 soc_flow_db_mem_view_flow_handle_valid(int unit,
                                   uint32 flow_handle);

#endif /* FLOW_DB_H */
