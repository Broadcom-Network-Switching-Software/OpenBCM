/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * This cint contains functions that are needed for OAM/BFD testing.
 * If a packet is injected into the ingress (using the tx command) a PTCH-2 header is added at the
 * beginning of the packet.  This reconfiguration ensures that the packet will be handled correctly
 * by the OAMP.
 */

/*
 * Configuration:
 *  
 * cint;                                                                  
 * cint_reset();                                                          
 * exit; 
 * cint ../../../src/examples/dnx/field/cint_field_oam_layer_index.c
 * cint
 * field_oam_layer_index_main(0,0);
 *
 *   Trap Type                               |     Trap ID             |  Layer_FWD + 1      | Parsing_start_index
 * FIELD_OAM_TRAP_ETH_OAM                   224                        X                          FWD + 1
 * FIELD_OAM_TRAP_Y1731_O_MPLSTP        225                        X                          FWD
 * FIELD_OAM_TRAP_Y1731_O_PWE            226                        X                          FWD
 * FIELD_OAM_TRAP_BFD_O_IPV4               227                        X                          FWD + 2
 * FIELD_OAM_TRAP_BFD_O_MPLS              228                        X                          FWD + 2
 * FIELD_OAM_TRAP_BFD_O_PWE               229                     IPv4/IPv6                 FWD + 2
 * FIELD_OAM_TRAP_BFDCC_O_MPLSTP       230                     IPv4/IPv6                 FWD + 2
 * FIELD_OAM_TRAP_BFDCV_O_MPLSTP       231                     IPv4/IPv6                 FWD + 2
 * FIELD_OAM_TRAP_BFD_O_PWE               229                        X                          FWD
 * FIELD_OAM_TRAP_BFDCC_O_MPLSTP       230                        X                          FWD
 * FIELD_OAM_TRAP_BFDCV_O_MPLSTP       231                        X                          FWD
 * FIELD_OAM_TRAP_BFD_O_IPV6           232                        X                          FWD + 2
 *
 * Entries are going to be add only for traps that need to increment the Parsing index of Forwarding
 */


int FIELD_OAM_TRAP_ETH_OAM=224;
int FIELD_OAM_TRAP_ETH_OAM_PERFORMANCE=169;
int FIELD_OAM_TRAP_Y1731_O_MPLSTP=225;
int FIELD_OAM_TRAP_Y1731_O_MPLSTP_PERFORMANCE=170;
int FIELD_OAM_TRAP_Y1731_O_PWE=226;
int FIELD_OAM_TRAP_Y1731_O_PWE_PERFORMANCE=171;
int FIELD_OAM_TRAP_BFD_O_IPV4=227;
int FIELD_OAM_TRAP_BFD_O_MPLS=228;
int FIELD_OAM_TRAP_BFD_O_PWE=229;
int FIELD_OAM_TRAP_BFDCC_O_MPLSTP=230;
int FIELD_OAM_TRAP_BFDCV_O_MPLSTP=231;
int FIELD_OAM_TRAP_BFD_O_IPV6 = 232;

/** Contains a list of all created traps*/
int trap_code_list[] = \
{
    FIELD_OAM_TRAP_ETH_OAM ,\
    FIELD_OAM_TRAP_ETH_OAM_PERFORMANCE ,\
    FIELD_OAM_TRAP_Y1731_O_MPLSTP ,\
    FIELD_OAM_TRAP_Y1731_O_MPLSTP_PERFORMANCE ,\
    FIELD_OAM_TRAP_Y1731_O_PWE ,\
    FIELD_OAM_TRAP_Y1731_O_PWE_PERFORMANCE ,\
    FIELD_OAM_TRAP_BFD_O_IPV4 ,\
    FIELD_OAM_TRAP_BFD_O_MPLS ,\
    FIELD_OAM_TRAP_BFD_O_PWE ,\
    FIELD_OAM_TRAP_BFD_O_IPV6 ,\
};

int IPv4_Type = 2;
int IPv6_Type = 3;
int Type_Mask = 0x1F;

bcm_field_group_t oam_fg_id = 0;
bcm_field_entry_t entry_handle[200] = {0};
int entry_handle_count = 0;

int FIELD_OAM_NOF_QUALS = 3;
int FIELD_OAM_NOF_ACTIONS = 2;
int DNX_OAM_FWD_INDEX_DEF_VALUE = 0;

int called_once = 0;


/**
* \brief
*  Add entries per trap code and layer protocol+1 type
* \param [in] unit               - Device ID
* \param [in] trap_id            - Trap id values key
* \param [in] pars_inc           -  Action of how much to increment the FWD layer
* \param [in] fwd_plus_one_type  - Forwarding+1 protocol type
* \param [in] fwd_plus_one_mask  -  Forwarding+1 mask
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_oam_add_entries(int unit, int trap_id, int pars_inc, int fwd_plus_one_type, int fwd_plus_one_mask)
{
    int rv = 0;
    bcm_field_entry_info_t entry_info;
    int ii;

    printf("Add entries for Trap %d Inc %d FWD+1 %d\n", trap_id,pars_inc,fwd_plus_one_type);
    printf("======================================\n");
    
    for(ii = 0 ; ii < 8; ii++)
    {
        bcm_field_entry_info_t_init(&entry_info);
        entry_info.priority = ii;
        entry_info.nof_entry_actions = FIELD_OAM_NOF_ACTIONS;
        entry_info.nof_entry_quals = FIELD_OAM_NOF_QUALS;
        
        entry_info.entry_qual[0].type = bcmFieldQualifyLayerRecordType;
        entry_info.entry_qual[0].value[0] = fwd_plus_one_type;
        entry_info.entry_qual[0].mask[0] = fwd_plus_one_mask;
        entry_info.entry_qual[1].type = bcmFieldQualifyRxTrapCode;
        entry_info.entry_qual[1].value[0] = trap_id;
        entry_info.entry_qual[1].mask[0] = 0x1FF;
        entry_info.entry_qual[2].type = bcmFieldQualifyForwardingLayerIndex;
        entry_info.entry_qual[2].value[0] = ii;
        entry_info.entry_qual[2].mask[0] = 0x7;
        

        entry_info.entry_action[0].type = bcmFieldActionEgressForwardingIndex;
        entry_info.entry_action[0].value[0] = (ii+pars_inc)%8;

        entry_info.entry_action[1].type = bcmFieldActionForwardingLayerIndex;
        entry_info.entry_action[1].value[0] = (ii+pars_inc)%8;

        printf("Quals Trap LR_type %d Trap %d Index %d Action %d \n",entry_info.entry_qual[0].value[0],
                                                            entry_info.entry_qual[1].value[0],
                                                            entry_info.entry_qual[2].value[0],
                                                            entry_info.entry_action.value[0]);
        
        rv = bcm_field_entry_add(unit, 0, oam_fg_id, &entry_info, &entry_handle[entry_handle_count]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_add ipmf1 entry_id(%d)\n", rv,entry_handle[entry_handle_count]);
            return rv;
        }
        entry_handle_count++;
    }

    return 0;
    
}



/**
* \brief
*  Will Add create a field group With relevant entries in TCAM for OAM CPU injection packet handling and attach it to given as param context
* \param [in] unit        - Device ID
* \param [in] context_id  - Context to attach FG to
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_oam_layer_index_main(int unit, bcm_field_context_t context_id)
{
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t entry_info;
    bcm_rx_trap_config_t trap_config;
    void *dest_char;
    int rv = BCM_E_NONE;
    int ii = 0, index = 0;
    int flags=0;
    int trap_code=0;

    /** Running this function twice results in an error */
    if(called_once != 0)
    	return 0;
    called_once = 1;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = FIELD_OAM_NOF_QUALS;
    fg_info.qual_types[0] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[1] = bcmFieldQualifyRxTrapCode;
    fg_info.qual_types[2] = bcmFieldQualifyForwardingLayerIndex;

    /* Set actions */
    fg_info.nof_actions = FIELD_OAM_NOF_ACTIONS;
  
    fg_info.action_types[0] = bcmFieldActionEgressForwardingIndex;
    fg_info.action_types[1] = bcmFieldActionForwardingLayerIndex;

    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "Trap_OAM_LR", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &oam_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add oam_fg_id\n", rv);
        return rv;
    }    
    
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for(ii=0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for(ii=0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }


    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;

    
    rv = bcm_field_group_context_attach(unit, 0, oam_fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach ipmf1\n", rv);
        return rv;
    }

    /** Disbale the EGRESS_INDEX_OVERWRITE for all trap ids, that are created */
    for (index = 0; index < 10; index++)
    {
        bcm_rx_trap_config_t_init(trap_config);
        rv = bcm_rx_trap_get(unit,trap_code_list[index],&trap_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_rx_trap_set for trap (%d)\n", rv , trap_code_list[index]);
            return rv;
        }

        trap_config.flags = BCM_RX_TRAP_TRAP | BCM_RX_TRAP_UPDATE_DEST;
        rv = bcm_rx_trap_set(unit,trap_code_list[index],trap_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_rx_trap_set for trap (%d)\n", rv , trap_code_list[index]);
            return rv;
        }
    }

    /**Entries for FIELD_OAM_TRAP_ETH_OAM*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_ETH_OAM,1,0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_ETH_OAM);
        return rv;
    }

    /**Entries for FIELD_OAM_TRAP_ETH_OAM_PERFORMANCE*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_ETH_OAM_PERFORMANCE,1,0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_ETH_OAM_PERFORMANCE);
        return rv;
    }
    
    /**Entries for FIELD_OAM_TRAP_BFD_O_IPV4*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_BFD_O_IPV4,2,0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_BFD_O_IPV4);
        return rv;
    }

    /**Entries for FIELD_OAM_TRAP_BFD_O_IPV4*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_BFD_O_IPV6,2,0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_BFD_O_IPV4);
        return rv;
    }

    /**Entries for FIELD_OAM_TRAP_BFD_O_MPLS*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_BFD_O_MPLS,2,0,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_BFD_O_MPLS);
        return rv;
    }

    /**Entries for FIELD_OAM_TRAP_BFD_O_PWE*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_BFD_O_PWE,2,IPv4_Type,Type_Mask);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_BFD_O_PWE);
        return rv;
    }
    /**Entries for FIELD_OAM_TRAP_BFD_O_PWE*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_BFD_O_PWE,2,IPv6_Type,Type_Mask);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_BFD_O_PWE);
        return rv;
    }

    /**Entries for FIELD_OAM_TRAP_BFDCC_O_MPLSTP*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_BFDCC_O_MPLSTP,2,IPv4_Type,Type_Mask);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_BFDCC_O_MPLSTP);
        return rv;
    }
    /**Entries for FIELD_OAM_TRAP_BFDCC_O_MPLSTP*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_BFDCC_O_MPLSTP,2,IPv6_Type,Type_Mask);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_BFDCC_O_MPLSTP);
        return rv;
    }

    /**Entries for FIELD_OAM_TRAP_BFDCV_O_MPLSTP*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_BFDCV_O_MPLSTP,2,IPv4_Type,Type_Mask);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_BFDCV_O_MPLSTP);
        return rv;
    }
    /**Entries for FIELD_OAM_TRAP_BFDCV_O_MPLSTP*/
    rv = field_oam_add_entries(unit,FIELD_OAM_TRAP_BFDCV_O_MPLSTP,2,IPv6_Type,Type_Mask);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in field_oam_add_entries for trap (%d)\n", rv , FIELD_OAM_TRAP_BFDCV_O_MPLSTP);
        return rv;
    }
    
    return 0;
}


/**
* \brief
*  Destroy the FG of OAM CPU injected packet handling
* \param [in] unit        - Device ID
* \param [in] context_id  - Context to detach the info from
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int field_oam_layer_index_destroy(int unit, bcm_field_context_t context_id)
{
    int rv = BCM_E_NONE;
    int ii= 0;
    int ent_id;

    rv = bcm_field_group_context_detach(unit,oam_fg_id,context_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error in bcm_field_group_context_detach\n");
        return rv;
    }

    for(ii = 0; ii < entry_handle_count; ii++)
    {
        rv = bcm_field_entry_delete(unit, oam_fg_id, NULL, entry_handle[ii]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_entry_add ipmf1 entry_id(%d)\n", rv,entry_handle[ii]);
            return rv;
        }
    }

    rv = bcm_field_group_delete(unit, oam_fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_delete\n", rv);
        return rv;
    }

    return rv;
}
