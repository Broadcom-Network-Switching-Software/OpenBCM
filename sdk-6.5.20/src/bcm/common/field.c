
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Field - Broadcom StrataSwitch Field Processor switch common API.
 */
#include <soc/types.h>
#include <soc/ll.h>

#include <bcm/field.h>

/*
 * Function:
 *      bcm_field_udf_spec_t_init
 * Purpose:
 *      Initialize a UDF data type.
 */

void 
bcm_field_udf_spec_t_init(bcm_field_udf_spec_t *udf_spec)
{
    if (NULL != udf_spec) {
        sal_memset(udf_spec, 0, sizeof(bcm_field_udf_spec_t));
    }
    return;
}

/*
 * Function:
 *      bcm_field_llc_header_t_init
 * Purpose:
 *      Initialize the bcm_field_llc_header_t structure.
 * Parameters:
 *      llc_header - Pointer to llc header structure.
 * Returns:
 *      NONE
 */
void
bcm_field_llc_header_t_init(bcm_field_llc_header_t *llc_header)
{
    if (llc_header != NULL) {
        sal_memset(llc_header, 0, sizeof (*llc_header));
    }
    return;
}

/*
 * Function:
 *      bcm_field_snap_header_t_init
 * Purpose:
 *      Initialize the bcm_field_snap_header_t structure.
 * Parameters:
 *      snap_header - Pointer to snap header structure.
 * Returns:
 *      NONE 
 */
void
bcm_field_snap_header_t_init(bcm_field_snap_header_t *snap_header)
{
    if (snap_header != NULL) {
        sal_memset(snap_header, 0, sizeof (*snap_header));
    }
    return;
}

/*
 * Function:
 *      bcm_field_qset_t_init
 * Purpose:
 *      Initialize the bcm_field_qset_t structure.
 * Parameters:
 *      qset - Pointer to field qset structure.
 * Returns:
 *      NONE
 */
void
bcm_field_qset_t_init(bcm_field_qset_t *qset)
{
    if (qset != NULL) {
        sal_memset(qset, 0, sizeof (*qset));
    }
    return;
}

/*
 * Function:
 *      bcm_field_aset_t_init
 * Purpose:
 *      Initialize the bcm_field_aset_t structure.
 * Parameters:
 *      aset - Pointer to field aset structure.
 * Returns:
 *      NONE
 */
void
bcm_field_aset_t_init(bcm_field_aset_t *aset)
{
    if (aset != NULL) {
        sal_memset(aset, 0, sizeof (*aset));
    }
    return;
}

/*
 * Function:
 *      bcm_field_presel_set_t_init
 * Purpose:
 *      Initialize the bcm_field_presel_set_t structure.
 * Parameters:
 *      presel_set - Pointer to field aset structure.
 * Returns:
 *      NONE
 */
void
bcm_field_presel_set_t_init(bcm_field_presel_set_t *presel_set)
{
    if (presel_set != NULL) {
        sal_memset(presel_set, 0, sizeof (*presel_set));
    }
    return;
}

/*
 * Function:
 *      bcm_field_group_status_t_init
 * Purpose:
 *      Initialize the Field Group Status structure.
 * Parameters:
 *      fgroup - Pointer to Field Group Status structure.
 * Returns:
 *      NONE
 */
void
bcm_field_group_status_t_init(bcm_field_group_status_t *fgroup)
{
    if (fgroup != NULL) {
        sal_memset(fgroup, 0, sizeof (*fgroup));
    }
    return;
}

/*
 * Function:
 *      bcm_field_class_info_t_init
 * Purpose:
 *      Initialize the Field Class Info structure.
 * Parameters:
 *      fclass - Pointer to Field Class Info structure.
 * Returns:
 *      NONE
 */
void
bcm_field_class_info_t_init(bcm_field_class_info_t *fclass)
{
    if (fclass != NULL) {
        sal_memset(fclass, 0, sizeof (*fclass));
    }
    return;
}

/*
 * Function:
 *      bcm_field_data_qualifier_t
 * Purpose:
 *      Initialize the Field Data Qualifier structure.
 * Parameters:
 *      data_qual - Pointer to field data qualifier structure.
 * Returns:
 *      NONE
 */
void
bcm_field_data_qualifier_t_init(bcm_field_data_qualifier_t *data_qual)
{
    if (data_qual != NULL) {
        sal_memset(data_qual, 0, sizeof (bcm_field_data_qualifier_t));
    }
    return;
}

/*
 * Function:
 *      bcm_field_data_ethertype_t_init
 * Purpose:
 *      Initialize ethertype based field data qualifier. 
 * Parameters:
 *      etype - Pointer to ethertype based data qualifier structure.
 * Returns:
 *      NONE
 */
void
bcm_field_data_ethertype_t_init (bcm_field_data_ethertype_t *etype)
{
    if (etype != NULL) {
        sal_memset(etype, 0, sizeof (bcm_field_data_ethertype_t));
        etype->l2 = BCM_FIELD_DATA_FORMAT_L2_ANY;
        etype->vlan_tag = BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY; 
    }
    return;
}

/*
 * Function:
 *      bcm_field_data_ip_protocol_t_init
 * Purpose:
 *      Initialize ip protocol based field data qualifier. 
 * Parameters:
 *      etype - Pointer to ip_protocol based data qualifier structure.
 * Returns:
 *      NONE
 */
void
bcm_field_data_ip_protocol_t_init (bcm_field_data_ip_protocol_t *ip_protocol)
{
    if (ip_protocol != NULL) {
        sal_memset(ip_protocol, 0, sizeof (bcm_field_data_ip_protocol_t));
        ip_protocol->flags = BCM_FIELD_DATA_FORMAT_IP_ANY;
        ip_protocol->ip = 0xFF; /* Reserved  IP Protocol ID */
        ip_protocol->l2 = BCM_FIELD_DATA_FORMAT_L2_ANY;
        ip_protocol->vlan_tag = BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY; 
    }
    return;
}

/*
 * Function:
 *      bcm_field_data_packet_format_t_init
 * Purpose:
 *      Initialize packet format based field data qualifier. 
 * Parameters:
 *      packet_format - Pointer to packet_format based data qualifier structure.
 * Returns:
 *      NONE
 */
void
bcm_field_data_packet_format_t_init (bcm_field_data_packet_format_t *packet_format)
{
    if (packet_format != NULL) {
        sal_memset(packet_format, 0, sizeof (bcm_field_data_packet_format_t));
        packet_format->l2  = BCM_FIELD_DATA_FORMAT_L2_ANY;
        packet_format->vlan_tag  = BCM_FIELD_DATA_FORMAT_VLAN_TAG_ANY;
        packet_format->outer_ip  = BCM_FIELD_DATA_FORMAT_IP_ANY;
        packet_format->inner_ip  = BCM_FIELD_DATA_FORMAT_IP_ANY;
        packet_format->tunnel  = BCM_FIELD_DATA_FORMAT_TUNNEL_ANY;
        packet_format->mpls  = BCM_FIELD_DATA_FORMAT_MPLS_ANY;
        packet_format->fibre_chan_outer  = BCM_FIELD_DATA_FORMAT_FIBRE_CHAN_ANY;
        packet_format->fibre_chan_inner  = BCM_FIELD_DATA_FORMAT_FIBRE_CHAN_ANY;
        packet_format->flags = 0x0;
    }
    return;
}

/*
 * Function:
 *      bcm_field_group_config_t_init
 * Purpose:
 *      Initialize the Field Group Config structure.
 * Parameters:
 *      data_qual - Pointer to field group config structure.
 * Returns:
 *      NONE
 */
void
bcm_field_group_config_t_init(bcm_field_group_config_t *group_config)
{
    if (group_config != NULL) {
        sal_memset(group_config, 0, sizeof (bcm_field_group_config_t));
        group_config->action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
    }

    return;
}

void
bcm_field_group_config_extension_t_init(bcm_field_group_config_extension_t *group_config)
{
    if (group_config != NULL) {
        sal_memset(group_config, 0, sizeof (bcm_field_group_config_extension_t));
        group_config->action_res_id = BCM_FIELD_GROUP_ACTION_RES_ID_DEFAULT;
    }

    return;
}

void bcm_field_group_presel_info_init(
    int nof_presel_info,
    bcm_field_group_presel_info_t *presel_info
)
{
    int hw_index, qual_index, presel_index;

    for (presel_index = 0; presel_index < nof_presel_info; presel_index++)
    {
        presel_info[presel_index].presel_id = BCM_FIELD_ENTRY_INVALID;
        for (qual_index = 0; qual_index < BCM_FIELD_NOF_QUALS_PER_FG; qual_index++)
        {
            presel_info[presel_index].qual_info[qual_index].qual = BCM_FIELD_ENTRY_INVALID;
            for (hw_index = 0; hw_index < BCM_FIELD_NOF_CE_ID_PER_QUAL; hw_index++)
            {
                presel_info[presel_index].qual_info[qual_index].ce_id[hw_index] = BCM_FIELD_ENTRY_INVALID;
            }
        }
    }
    return;
}

/*
 * Function:
 *     bcm_field_entry_oper_t_init
 * Purpose:
 *     Initialize a field entry operation structure
 * Parameters:
 *     entry_oper - Pointer to field entry operation structure
 * Returns:
 *     NONE
 */ 
void
bcm_field_entry_oper_t_init(bcm_field_entry_oper_t *entry_oper)
{
    if (entry_oper != NULL) {
        sal_memset(entry_oper, 0, sizeof (bcm_field_entry_oper_t));
    }
    return;
}

/*
 * Function:
 *      bcm_field_extraction_action_t_init
 * Purpose:
 *      Initialize the Field Extraction Action structure.
 * Parameters:
 *      action - Pointer to field extraction action structure.
 * Returns:
 *      NONE
 */
void
bcm_field_extraction_action_t_init(bcm_field_extraction_action_t *action)
{
    if (NULL != action) {
        sal_memset(action, 0x00, sizeof(*action));
    }
}

/*
 * Function:
 *      bcm_field_extraction_field_t_init
 * Purpose:
 *      Initialize the Field Extraction Field structure
 * Parameters:
 *      extraction - pointer to field extraction field structure
 * Returns:
 *      NONE
 */
void
bcm_field_extraction_field_t_init(bcm_field_extraction_field_t *extraction)
{
    if (NULL != extraction) {
        sal_memset(extraction, 0x00, sizeof(*extraction));
    }
}

/* Function: bcm_field_hint_t_init
 *
 * Purpose:
 *     Initialize hint data structure.
 * Parameters:
 *     unit - (IN) BCM device number.
 *     hint - (IN) Pointer to bcm_field_hint_t structure
 * Returns:
 *     None.
 */
void bcm_field_hint_t_init(bcm_field_hint_t *hint)
{
    if (hint != NULL) {
        sal_memset (hint, 0, sizeof (bcm_field_hint_t));
    }
    return;
}

/* Function: bcm_field_oam_stat_action_t_init
 *
 * Purpose:
 *     Initialize oam stat action data structure.
 * Parameters:
 *     unit - (IN) BCM device number.
 *     oam_action - (IN) Pointer to bcm_field_oam_stat_action_t structure

 * Returns:
 *     None.
 */
void bcm_field_oam_stat_action_t_init (bcm_field_oam_stat_action_t *oam_action)
{
    if (oam_action != NULL) {
        sal_memset (oam_action, 0, sizeof (bcm_field_oam_stat_action_t));
    }
    return;
}

/* Function: bcm_field_src_class_t_init
 *
 * Purpose:
 *     Initialize source class mode structure.
 * Parameters:
 *     unit - (IN) BCM device number.
 *     src_class - (IN) Pointer to bcm_field_src_class_t structure

 * Returns:
 *     None.
 */
void bcm_field_src_class_t_init (bcm_field_src_class_t *src_class)
{
    if (src_class != NULL) {
        sal_memset (src_class, 0, sizeof (bcm_field_src_class_t));
    }
    return;
}

/* Function: bcm_field_copytocpu_config_t_init
 *
 * Purpose:
 *     Initialize CopyToCpu config structure.
 * Parameters:
 *     unit - (IN) BCM device number.
 *     CopyToCpu_config - (IN) Pointer to bcm_field_CopyToCpu_config_t structure
 * Returns:
 *     None.
 */
void bcm_field_copytocpu_config_t_init (bcm_field_CopyToCpu_config_t *CopyToCpu_config)
{
    if (CopyToCpu_config != NULL) {
        sal_memset (CopyToCpu_config, 0, sizeof (bcm_field_CopyToCpu_config_t));
        CopyToCpu_config->flags = BCM_FIELD_COPYTOCPU_ALL_PACKET;
    }
    return;
}

/* Function: bcm_field_redirect_config_t_init
 *
 * Purpose:
 *     Initialize redirect config structure.
 * Parameters:
 *     unit - (IN) BCM device number.
 *     redirect_config - (IN) Pointer to bcm_field_redirect_config_t structure
 * Returns:
 *     None.
 */
void bcm_field_redirect_config_t_init (bcm_field_redirect_config_t *redirect_config)
{
    if (redirect_config != NULL) {
        sal_memset (redirect_config, 0, sizeof (bcm_field_redirect_config_t));
        redirect_config->flags = BCM_FIELD_REDIRECT_ALL_PACKET;
        redirect_config->destination_type = bcmFieldRedirectDestinationInvalid;
        redirect_config->destination = BCM_GPORT_INVALID;
        redirect_config->source_port = BCM_GPORT_INVALID;
    }
    return;
}

/*
 * Function:
 *      bcm_field_presel_config_t_init
 * Purpose:
 *      Initialize Field Presel Config structure.
 * Parameters:
 *      presel_config - (INOUT) Presel name.
 * Returns:
 *      None.
 */
extern void bcm_field_presel_config_t_init(
    bcm_field_presel_config_t *presel_config)
{
    if (presel_config != NULL) {
        sal_memset (presel_config, 0, sizeof (bcm_field_presel_config_t));
    }
    return;
}

/*
 * Function:
 *      bcm_field_entry_config_t_init
 * Purpose:
 *      Initialize Field Entry Config structure.
 * Parameters:
 *      entry_config - (INOUT) configuration for entry creation.
 * Returns:
 *      None.
 */
extern void bcm_field_entry_config_t_init(
    bcm_field_entry_config_t *entry_config)
{
    if (entry_config != NULL) {
        sal_memset (entry_config, 0, sizeof (bcm_field_entry_config_t));
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_context_info_t
 * \param [out] context_info  -
 *  Pointer to input structure of bcm_field_context_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_context_info_t struct
 * \see
 *  Reference to bcm_field_context_create/get
 */
extern void bcm_field_context_info_t_init(
    bcm_field_context_info_t * context_info)
{
    /** Set default values for relevant fields in bcm_field_context_info_t */
    if (context_info != NULL)
    {
        context_info->context_compare_modes.compare_1_mode = bcmFieldContextCompareTypeNone;
        context_info->context_compare_modes.compare_2_mode = bcmFieldContextCompareTypeNone;
        context_info->hashing_enabled = FALSE;
        context_info->state_table_enabled = FALSE;
        context_info->cascaded_from = BCM_FIELD_CONTEXT_ID_INVALID;
        context_info->name[0] = 0;
    }
    return;
}

/**
* \brief
*  Initialize the bcm_field_group_info_t
* \param [in] fg_info  - Pointer to input structure of bcm_field_group_info_t that needs to be initialized
* \return
*  void
* \remark
*  This function should be called prior of using bcm_field_group_info_t struct
* \see
*  Reference to bcm_field_group_add
*/
extern void
bcm_field_group_info_t_init(
    bcm_field_group_info_t * fg_info)
{
    int ii;
    if (fg_info != NULL)
    {
        sal_memset (fg_info, 0, sizeof (bcm_field_group_info_t));
        fg_info->stage = bcmFieldStageCount;
        fg_info->fg_type = bcmFieldGroupTypeCount;
		fg_info->tcam_info.bank_allocation_mode = bcmFieldTcamBankAllocationModeAuto;
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP; ii++)
        {
            fg_info->action_types[ii] = bcmFieldActionCount;
            fg_info->action_with_valid_bit[ii] = TRUE;
        }
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; ii++)
        {
            fg_info->qual_types[ii] = bcmFieldQualifyCount;
            fg_info->qual_is_ranged[ii] = FALSE;
        }
    }

    return;
}

/**
 * \brief
 *  Initialize the bcm_field_qualify_attach_info_t
 * \param [out] qual_info -
 *  Pointer to input structure of bcm_field_qualify_attach_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_qualify_attach_info_t struct
 * \see
 *  Reference to:
 *    - bcm_field_group_attach_info_t_init
 *    - bcm_field_group_context_attach
 *    - bcm_field_group_context_get
 */
extern void
bcm_field_qual_attach_info_t_init(
    bcm_field_qualify_attach_info_t * qual_info)
{
    if (qual_info != NULL) {
        sal_memset (qual_info, 0, sizeof (bcm_field_qualify_attach_info_t));
        qual_info->input_type = BCM_FIELD_INVALID;
        qual_info->input_arg = BCM_FIELD_INVALID;
        qual_info->offset = BCM_FIELD_INVALID;
    }
    return;
}


/**
 * \brief
 *  Initialize the bcm_field_group_attach_info_t
 * \param [out] attach_info  -
 *  Pointer to input structure of bcm_field_group_attach_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_group_attach_info_t struct
 * \see
 *  Reference to:
 *    - bcm_field_group_attach_info_t_init
 *    - bcm_field_group_context_attach
 *    - bcm_field_group_context_get
 */
extern void
bcm_field_group_attach_info_t_init(
    bcm_field_group_attach_info_t * attach_info)
{
    int ii;
    if (attach_info != NULL) {
        sal_memset (attach_info, 0, sizeof (bcm_field_group_attach_info_t));
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP; ii++)
        {
            attach_info->payload_info.action_types[ii] = bcmFieldActionCount;
            attach_info->payload_info.action_info[ii].priority = BCM_FIELD_ACTION_DONT_CARE;
            attach_info->payload_info.action_info[ii].valid_bit_polarity = 1;
        }
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; ii++)
        {
            attach_info->key_info.qual_types[ii] = bcmFieldQualifyCount;
            bcm_field_qual_attach_info_t_init(&attach_info->key_info.qual_info[ii]);
        }
        attach_info->payload_info.invalidate_fems = NULL;
        attach_info->compare_id = BCM_FIELD_INVALID;
    }
    return;
}

/**
* \brief
*  Initialize the bcm_field_action_info_t
* \param [out] action_info  - Pointer to input structure of bcm_field_action_info_t that needs to be initialized
* \return
*  void
* \remark
*  This function should be called prior of using bcm_dnx_field_action_info_t_init struct
* \see
*  Reference to:
*  - bcm_field_action_create
*/
extern void bcm_field_action_info_t_init(
    bcm_field_action_info_t *action_info)
{
    if (action_info != NULL) {
        sal_memset (action_info, 0, sizeof (bcm_field_action_info_t));
        action_info->stage = bcmFieldStageCount;
        action_info->action_type = bcmFieldActionCount;
    }
    return;
}

/**
* \brief
*  Initialize the bcm_field_qualifier_info_create_t
* \param [out] qual_info  - Pointer to input structure of bcm_field_qualifier_info_create_t that needs to be initialized
* \return
*  void
* \remark
*  This function should be called prior of using bcm_field_qualifier_info_create_t struct
* \see
*  Reference to:
*  - bcm_field_qualifier_create
*/
extern void bcm_field_qualifier_info_create_t_init(
    bcm_field_qualifier_info_create_t *qual_info)
{
    if (qual_info != NULL) {
        sal_memset (qual_info, 0, sizeof (bcm_field_qualifier_info_create_t));
    }
    return;
}

/**
* \brief
*  Initialize the bcm_field_qualifier_info_get_t
* \param [out] qual_info  - Pointer to input structure of bcm_field_qualifier_info_get_t that needs to be initialized
* \return
*  void
* \remark
*  This function should be called prior of using bcm_field_qualifier_info_get_t struct
* \see
*  Reference to:
*  - bcm_field_qualifier_create
*/
extern void bcm_field_qualifier_info_get_t_init(
    bcm_field_qualifier_info_get_t *qual_info)
{
    if (qual_info != NULL) {
        sal_memset (qual_info, 0, sizeof (bcm_field_qualifier_info_get_t));
        qual_info->qual_class = bcmFieldQualifierClassCount;
    }
    return;
}

/**
 * \brief
 *  Initialize an array of bcm_field_entry_qual_t
 * \param [out] entry_qual  -
 *  Pointer to an array of BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP structures of bcm_field_entry_qual_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using an array of bcm_field_entry_qual_t structs
 * \see
 *  Reference to:
 *    - bcm_field_entry_delete
 */
extern void bcm_field_entry_qual_t_init(
    bcm_field_entry_qual_t entry_qual[BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP])
{
    int ii;
    if (entry_qual != NULL) {
        sal_memset (entry_qual, 0, sizeof (bcm_field_entry_qual_t));
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; ii++)
        {
            entry_qual[ii].type = bcmFieldQualifyCount;
        }
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_entry_info_t
 * \param [out] entry_info  -
 *  Pointer to input structure of bcm_field_entry_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_entry_info_t struct
 * \see
 *  Reference to:
 *    - bcm_field_entry_add
 *    - bcm_field_entry_info_get
 */
extern void bcm_field_entry_info_t_init(
    bcm_field_entry_info_t *entry_info)
{
    int ii;
    if (entry_info != NULL) {
        sal_memset (entry_info, 0, sizeof (bcm_field_entry_info_t));
        entry_info->nof_entry_quals = BCM_FIELD_INVALID;
        entry_info->nof_entry_actions = BCM_FIELD_INVALID;
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP; ii++)
        {
            entry_info->entry_action[ii].type = bcmFieldActionCount;
        }
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; ii++)
        {
            entry_info->entry_qual[ii].type = bcmFieldQualifyCount;
        }
        entry_info->core = BCM_CORE_ALL;
        entry_info->valid_bit = TRUE;
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_context_hash_info_t
 * \param [out] hash_info  -
 *  Pointer to input structure of bcm_field_context_hash_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_context_hash_info_t struct
 * \see
 *  Reference to:
 *    - bcm_field_context_hash_create
 */
extern void bcm_field_context_hash_info_t_init(
    bcm_field_context_hash_info_t *hash_info)
{
    int ii;
    if (hash_info != NULL)
    {
        sal_memset (hash_info, 0, sizeof (bcm_field_context_hash_info_t));
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; ii++)
        {
            hash_info->key_info.qual_types[ii] = bcmFieldQualifyCount;
            hash_info->key_info.qual_info[ii].input_type = BCM_FIELD_INVALID;
            hash_info->key_info.qual_info[ii].input_arg = BCM_FIELD_INVALID;
            hash_info->key_info.qual_info[ii].offset = BCM_FIELD_INVALID;
        }
        hash_info->hash_function = bcmFieldContextHashFunctionCount;
        hash_info->hash_config.action_key = bcmFieldContextHashActionKeyCount;
        hash_info->hash_config.function_select = bcmFieldContextHashActionValueNone;
        hash_info->hash_config.crc_select = bcmFieldCrcSelectInvalid;
        hash_info->nof_additional_hash_config = 0;
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_HASH_KEYS; ii++)
        {
            hash_info->additional_hash_config[ii].action_key = bcmFieldContextHashActionKeyCount;
            hash_info->additional_hash_config[ii].function_select = bcmFieldContextHashActionValueNone;
            hash_info->additional_hash_config[ii].crc_select = bcmFieldCrcSelectInvalid;
        }
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_context_compare_info_t
 * \param [out] compare_info  -
 *  Pointer to input structure of bcm_field_context_compare_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_context_compare_info_t struct
 * \see
 *  Reference to:
 *    - bcm_field_context_compare_create
 */
extern void bcm_field_context_compare_info_t_init(
    bcm_field_context_compare_info_t *compare_info)
{
    int ii;
    if (compare_info != NULL)
    {
        sal_memset (compare_info, 0, sizeof (bcm_field_context_compare_info_t));
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; ii++)
        {
            compare_info->first_key_info.qual_types[ii] = bcmFieldQualifyCount;
            compare_info->first_key_info.qual_info[ii].input_type = BCM_FIELD_INVALID;
            compare_info->first_key_info.qual_info[ii].input_arg = BCM_FIELD_INVALID;
            compare_info->first_key_info.qual_info[ii].offset = BCM_FIELD_INVALID;
        }
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; ii++)
        {
            compare_info->second_key_info.qual_types[ii] = bcmFieldQualifyCount;
            compare_info->second_key_info.qual_info[ii].input_type = BCM_FIELD_INVALID;
            compare_info->second_key_info.qual_info[ii].input_arg = BCM_FIELD_INVALID;
            compare_info->second_key_info.qual_info[ii].offset = BCM_FIELD_INVALID;
        }
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_ace_format_info_t
 * \param [out] ace_format_info -
 *  Pointer to input structure of bcm_field_ace_format_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_ace_format_info_t struct
 * \see
 *  Reference to:
 *    - bcm_field_ace_format_add
 *    - bcm_field_ace_format_info_get
 */
extern void bcm_field_ace_format_info_t_init(
    bcm_field_ace_format_info_t *ace_format_info)
{
    int ii;
    if (ace_format_info != NULL)
    {
        sal_memset (ace_format_info, 0, sizeof (bcm_field_ace_format_info_t));
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP; ii++)
        {
            ace_format_info->action_types[ii] = bcmFieldActionCount;
            ace_format_info->action_with_valid_bit[ii] = TRUE;
        }
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_ace_entry_info_t
 * \param [out] entry_info  -
 *  Pointer to input structure of bcm_field_ace_entry_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_ace_entry_info_t struct
 * \see
 *  Reference to:
 *    - bcm_field_ace_entry_add
 *    - bcm_field_ace_entry_info_get
 */
extern void bcm_field_ace_entry_info_t_init(
    bcm_field_ace_entry_info_t *entry_info)
{
    int ii;
    if (entry_info != NULL)
    {
        sal_memset (entry_info, 0, sizeof (bcm_field_ace_entry_info_t));
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP; ii++)
        {
            entry_info->entry_action[ii].type = bcmFieldActionCount;
        }
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_presel_entry_id_t
 * \param [out] entry_id  -
 *  Pointer to input structure of bcm_field_presel_entry_id_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_presel_entry_id_t struct
 * \see
 *  Reference to:
 *    - bcm_field_presel_set
 *    - bcm_field_presel_get
 */
extern void bcm_field_presel_entry_id_info_init(
    bcm_field_presel_entry_id_t *entry_id)
{
    if (entry_id != NULL)
    {
        entry_id->presel_id = BCM_FIELD_ID_INVALID;
        entry_id->stage = bcmFieldStageCount;
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_presel_entry_data_t
 * \param [out] entry_data  -
 *  Pointer to input structure of bcm_field_presel_entry_data_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_presel_entry_data_t struct
 * \see
 *  Reference to:
 *    - bcm_field_presel_set
 *    - bcm_field_presel_get
 */
extern void bcm_field_presel_entry_data_info_init(
    bcm_field_presel_entry_data_t *entry_data)
{
    int ii;
    if (entry_data != NULL)
    {
        sal_memset (entry_data, 0, sizeof (bcm_field_presel_entry_data_t));
        entry_data->context_id = BCM_FIELD_CONTEXT_ID_INVALID;
        for (ii = 0; ii < BCM_FIELD_MAX_NOF_CS_QUALIFIERS; ii++)
        {
            entry_data->qual_data[ii].qual_type = bcmFieldQualifyCount;
        }
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_fem_action_info_t
 * \param [out] fem_action_info  -
 *  Pointer to input structure of bcm_field_fem_action_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_fem_action_info_t struct
 * \see
 *    * bcm_field_fem_action_add
 *    * bcm_field_fem_action_info_get
 */
extern void bcm_field_fem_action_info_t_init(
    bcm_field_fem_action_info_t *fem_action_info)
{
    int map_bits_per_fem_iter, extr_per_fem_iter;
    if (fem_action_info != NULL)
    {
        sal_memset (fem_action_info, 0, sizeof (bcm_field_fem_action_info_t));
        fem_action_info->fem_input.overriding_fg_id = BCM_FIELD_ID_INVALID;
        for (extr_per_fem_iter = 0; extr_per_fem_iter < BCM_FIELD_NUMBER_OF_EXTRACTIONS_PER_FEM; extr_per_fem_iter++)
        {
            fem_action_info->fem_extraction[extr_per_fem_iter].action_type = bcmFieldActionCount;

            for (map_bits_per_fem_iter = 0; map_bits_per_fem_iter < BCM_FIELD_NUMBER_OF_MAPPING_BITS_PER_FEM; map_bits_per_fem_iter++)
            {
                fem_action_info->fem_extraction[extr_per_fem_iter].output_bit[map_bits_per_fem_iter].source_type = bcmFieldFemExtractionOutputSourceTypeCount;
            }
        }
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_efes_action_info_t
 * \param [out] efes_action_info  -
 *  Pointer to input structure of bcm_field_efes_action_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_efes_action_info_t struct
 * \see
 *    * bcm_field_efes_action_add
 *    * bcm_field_efes_action_info_get
 */
extern void bcm_field_efes_action_info_t_init(
    bcm_field_efes_action_info_t *efes_action_info)
{
    int efes_2msb_condition;
    if (efes_action_info != NULL)
    {
        sal_memset (efes_action_info, 0, sizeof (bcm_field_efes_action_info_t));
        efes_action_info->condition_msb = BCM_FIELD_INVALID;
        for (efes_2msb_condition=0; efes_2msb_condition < BCM_FIELD_NUMBER_OF_CONDITIONS_PER_EFES; efes_2msb_condition++)
        {
            efes_action_info->efes_condition_conf[efes_2msb_condition].action_type = BCM_FIELD_INVALID;
            efes_action_info->efes_condition_conf[efes_2msb_condition].action_lsb = BCM_FIELD_INVALID;
            efes_action_info->efes_condition_conf[efes_2msb_condition].action_nof_bits = BCM_FIELD_INVALID;
            efes_action_info->efes_condition_conf[efes_2msb_condition].valid_bit_polarity = 1;
        }
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_context_param_info_t
 * \param [out] context_params -
 *  Pointer to input structure of bcm_field_context_param_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_context_param_info_t struct
 * \see
 *  Reference to:
 *   - bcm_field_context_param_set
 */
extern void bcm_field_context_param_info_t_init(
    bcm_field_context_param_info_t *context_params)
{
    if (context_params != NULL)
    {
        sal_memset (context_params, 0, sizeof (bcm_field_context_param_info_t));
        context_params->param_type = bcmFieldContextParamCount;
    }
    return;
}

/**
 * \brief
 *  Initialize the bcm_field_range_info_t
 * \param [out] range_info -
 *  Pointer to input structure of bcm_field_range_info_t
 *  that needs to be initialized
 * \return
 *  void
 * \remark
 *  This function should be called prior of using bcm_field_range_info_t struct
 * \see
 *  Reference to:
 *   - bcm_field_range_set
 */
extern void bcm_field_range_info_t_init(
    bcm_field_range_info_t *range_info)
{
    if (range_info != NULL)
    {
        sal_memset (range_info, 0, sizeof (bcm_field_range_info_t));
        range_info->range_type = BCM_FIELD_ID_INVALID;
        /* 
         * We use (-1) explicitly, as BCM_FIELD_INVALID (0xFFFF) isn't large enough to be an invalid value.
         */
        range_info->min_val = -1;
        range_info->max_val = -1;
    }
    return;
}

/**
 * \brief
 *  Inits the bcm_field_tcam_bank_info_t struct.
 *
 * \param [in] bank_info -
 *   Struct to init
 * \return
 *   void
 * \remark
 *   None
 * \see
 *   None
 */
extern void bcm_field_tcam_bank_info_t_init(
    bcm_field_tcam_bank_info_t * bank_info)
{
    int bank_iter;
    if (bank_info != NULL)
    {
        bank_info->fg_id = BCM_FIELD_ID_INVALID;
        bank_info->pp_app = bcmFieldAppDbInvalid;
        bank_info->nof_banks = 0;
        for (bank_iter = 0; bank_iter < BCM_FIELD_NUMBER_OF_TCAM_BANKS; bank_iter++)
        {
            bank_info->tcam_bank_ids[bank_iter] = BCM_FIELD_INVALID;
        }
    }

    return;
}

/*
 * Function:
 *      bcm_field_presel_group_config_t_init
 * Purpose:
 *      Initialize the Field Presel Group Config structure.
 * Parameters:
 *      data_qual - Pointer to field group config structure.
 * Returns:
 *      NONE
 */
void
bcm_field_presel_group_config_t_init(bcm_field_presel_group_config_t *group_config)
{
    if (group_config != NULL) {
        sal_memset(group_config, 0, sizeof (bcm_field_presel_group_config_t));
    }

    return;
}

/*
 * Function:
 *      bcm_field_presel_group_config_t_init
 * Purpose:
 *      Initialize the bcm_field_name_to_id_info_t structure.
 * Parameters:
 *      data_qual - Pointer to a bcm_field_name_to_id_info_t structure.
 * Returns:
 *      NONE
 */
void
bcm_field_name_to_id_info_t_init(bcm_field_name_to_id_info_t *name_to_id_info)
{
    if (name_to_id_info != NULL) {
        sal_memset(name_to_id_info, 0, sizeof (bcm_field_name_to_id_info_t));
        name_to_id_info->stage = bcmFieldStageCount;
        name_to_id_info->name_to_id_type = bcmFieldNameToIdCount;
    }

    return;
}

/*
 * Function:
 *     bcm_field_destination_match_t_init
 * Purpose:
 *      Initialize the field destination match structure.
 * Parameters:
 *      match - Pointer to the field destination match structure.
 * Returns:
 *      NONE
 */
void
bcm_field_destination_match_t_init(bcm_field_destination_match_t *match)
{
    if (match != NULL) {
        sal_memset(match, 0, sizeof(bcm_field_destination_match_t));
    }

    return;
}

/*
 * Function:
 *     bcm_field_destination_action_t_init
 * Purpose:
 *      Initialize the field destination action structure.
 * Parameters:
 *      match - Pointer to the field destination action structure.
 * Returns:
 *      NONE
 */
void
bcm_field_destination_action_t_init(bcm_field_destination_action_t *action)
{
    if (action != NULL) {
        sal_memset(action, 0, sizeof(bcm_field_destination_action_t));
    }

    return;
}

/*
 * Function:
 *     bcm_field_flush_entry_info_t_init
 * Purpose:
 *      Initialize the field flush entry structure.
 * Parameters:
 *      flush_entry_info - Pointer to the field flush entry structure.
 * Returns:
 *      NONE
 */
void
bcm_field_flush_entry_info_t_init(bcm_field_flush_entry_info_t *flush_entry_info)
{
    int ii;
    if (flush_entry_info != NULL) {
        sal_memset(flush_entry_info, 0, sizeof(bcm_field_flush_entry_info_t));

        flush_entry_info->key.nof_entry_actions = BCM_FIELD_INVALID;
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP; ii++)
        {
            flush_entry_info->key.entry_action[ii].type = bcmFieldActionCount;
        }
        flush_entry_info->key.nof_entry_quals = BCM_FIELD_INVALID;
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP; ii++)
        {
            flush_entry_info->key.entry_qual[ii].type = bcmFieldQualifyCount;
        }
        flush_entry_info->payload.nof_entry_actions = BCM_FIELD_INVALID;
        for (ii = 0; ii < BCM_FIELD_NUMBER_OF_ACTIONS_PER_GROUP; ii++)
        {
            flush_entry_info->payload.entry_action[ii].type = bcmFieldActionCount;
        }
        
    }

    return;
}

/*
 * Function:
 *     bcm_field_flush_profile_info_t_init
 * Purpose:
 *      Initialize the field flush profile structure.
 * Parameters:
 *      flush_profile_info - Pointer to the field flush profile structure.
 * Returns:
 *      NONE
 */
void
bcm_field_flush_profile_info_t_init(bcm_field_flush_profile_info_t *flush_profile_info)
{
    if (flush_profile_info != NULL) {
        sal_memset(flush_profile_info, 0, sizeof(bcm_field_flush_profile_info_t));
    }

    return;
}

