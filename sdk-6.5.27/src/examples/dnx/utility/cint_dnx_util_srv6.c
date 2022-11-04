/* $Id: cint_dnx_util_srv6.c,v 1.00
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * This file provides srv6 utils functionality
 */

/** \brief Max number of supported SIDs to be terminated in Extended Termination flow (using RCY) */
int MAX_EXTENDED_TERMINATED_NOF_SIDS = 10;
int MAX_EXTENDED_TERMINATED_NOF_SIDS_EXTENDED = 14;
int MAX_EXTENDED_TERMINATED_NOF_SIDS_AFTER_ITPP = 14;
/** \brief Max number of supported extension_lengths to be terminated in Extended Termination flow (using RCY) */
int MAX_EXTENDED_TERMINATED_NOF_EXTENTION_LENGTH = 19;
int MAX_EXTENDED_TERMINATED_NOF_EXTENTION_LENGTH_EXTENDED = 27;
int MAX_EXTENDED_TERMINATED_NOF_EXTENTION_LENGTH_AFTER_ITPP = 27;

/*
 * number of words in IPv6 DIP
 */
int  SRV6_UTILS_WORDS_IN_IP6_DIP     = 4;

/** \brief Structure to convert Last Element (NOF SIDs - 1) in an SRv6 packet to required termination sizes in:
 *   - as part of RCH 2nd Pass extension size
 *   - 1st Pass Egress Termination (in addition to layer start offset) */
struct cint_srv6_last_element_to_termination_sizes_s
{
    int recycle_header_extension_lengths[MAX_EXTENDED_TERMINATED_NOF_SIDS];    /* Size of extension header after recycle header in bytes */
    int additional_egress_termination_sizes[MAX_EXTENDED_TERMINATED_NOF_SIDS]; /* Size of egress additional termination in bytes (to parsing start offset value) */
    int recycle_header_extension_lengths_extended[MAX_EXTENDED_TERMINATED_NOF_SIDS_EXTENDED];    /* Size of extension header after recycle header in bytes for devices supporting 1 and 2 pass USP*/
    int additional_egress_termination_sizes_extended[MAX_EXTENDED_TERMINATED_NOF_SIDS_EXTENDED]; /* Size of egress additional termination in bytes (to parsing start offset value) for devices supporting 1 and 2 pass USP*/
    int recycle_header_extension_lengths_after_itpp[MAX_EXTENDED_TERMINATED_NOF_SIDS_AFTER_ITPP];    /* Size of extension header after recycle header in bytes for devices supporting itpp termination but ecologic*/
    int additional_egress_termination_sizes_after_itpp[MAX_EXTENDED_TERMINATED_NOF_SIDS_AFTER_ITPP]; /* Size of egress additional termination in bytes (to parsing start offset value) for devices supporting itpp termination but ecologic*/

    int recycle_header_extension_lengths_per_extension_length[MAX_EXTENDED_TERMINATED_NOF_EXTENTION_LENGTH];    /* Size of extension header after recycle header in bytes */
    int additional_egress_termination_sizes_per_extension_length[MAX_EXTENDED_TERMINATED_NOF_EXTENTION_LENGTH]; /* Size of egress additional termination in bytes (to parsing start offset value) */
    int recycle_header_extension_lengths_extended_per_extension_length[MAX_EXTENDED_TERMINATED_NOF_EXTENTION_LENGTH_EXTENDED];    /* Size of extension header after recycle header in bytes for devices supporting 1 and 2 pass USP*/
    int additional_egress_termination_sizes_extended_per_extension_length[MAX_EXTENDED_TERMINATED_NOF_EXTENTION_LENGTH_EXTENDED]; /* Size of egress additional termination in bytes (to parsing start offset value) for devices supporting 1 and 2 pass USP*/
    int recycle_header_extension_lengths_after_itpp_per_extension_length[MAX_EXTENDED_TERMINATED_NOF_EXTENTION_LENGTH_AFTER_ITPP];    /* Size of extension header after recycle header in bytes for devices supporting itpp termination but ecologic*/
    int additional_egress_termination_sizes_after_itpp_per_extension_length[MAX_EXTENDED_TERMINATED_NOF_EXTENTION_LENGTH_AFTER_ITPP]; /* Size of egress additional termination in bytes (to parsing start offset value) for devices supporting itpp termination but ecologic*/
};

cint_srv6_last_element_to_termination_sizes_s cint_srv6_last_element_to_termination_sizes =
{
         /*
          * Size of extension header after recycle header in bytes
          */
         { 0,  0,  0,  0,  0,  8, 24, 40, 56, 72},
         /*
          * Size of egress additional termination in bytes (to parsing start offset value)
          */
         { 0, 16, 32, 48, 64, 72, 72, 72, 72, 72},
         /*
          * Size of extension header after recycle header in bytes
          */
         {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 26, 42, 58 },
         /*
          * Size of egress additional termination in bytes (to parsing start offset value)
          */
         {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 150, 150, 150, 150 },
         /*
          * Size of extension header after recycle header in bytes for device with itpp termination
          */
         {0, 0,  0,   0,  0,  0,  0, 0,   0,   0,   8,   24,  40,  56 },
         /*
          * Size of egress additional termination in bytes (to parsing start offset value) or device with itpp termination
          */
         {0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 152, 152, 152, 152 },

         /*
          * per extension_length
          */
         /*
          * Size of extension header after recycle header in bytes
          */
         { 0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  8, 16, 24, 32, 40, 48, 56, 64, 72},
         /*
          * Size of egress additional termination in bytes (to parsing start offset value)
          */
         { 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 72, 72, 72, 72, 72, 72, 72, 72, 72},
         /*
          * Size of extension header after recycle header in bytes
          */
         {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,   0,   0,   8,  10,  18,  26,  34,  42,  50,  58 },
         /*
          * Size of egress additional termination in bytes (to parsing start offset value)
          */
         {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 144, 150, 150, 150, 150, 150, 150, 150 },
         /*
          * Size of extension header after recycle header in bytes for device with itpp termination
          */
         {0, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,   0,   0,   0,   0,  0,   0,   0,    8,  16,  24,  32,  40,  48,  56 },
         /*
          * Size of egress additional termination in bytes (to parsing start offset value) or device with itpp termination
          */
         {0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120, 128, 136, 144, 152, 152, 152, 152, 152, 152, 152, 152 }
};

int srv6_create_extended_termination_create_all_sids_rcy_entries_and_extensions(
    int unit,
    int nof_2pass_recycle_entries,
    int max_nof_terminated_usp_sids_1pass,
    int rch_port)
{
    char error_msg[200]={0,};
    int sid_idx;

    for (sid_idx = 0; sid_idx < nof_2pass_recycle_entries ; sid_idx++)
    {
        bcm_srv6_extension_terminator_mapping_t terminator_info;

        terminator_info.flags = 0;

        /** Convert Port number to GPORT */
        BCM_GPORT_LOCAL_SET(terminator_info.port, rch_port);

        /*
         * Create RCH encapsulation per each NOF SIDs, to take into account relevant:
         * -recycle_header_extension_length (bytes to remove on 2nd pass at RCH termination)
         * -additional_egress_termination_size (additional bytes to remove on 1st pass egress - additional to parsing start offset value)
         */
        int recycle_entry_encap_id_1;
        int recycle_entry_encap_id_2;
        /** Create entry, which will be built the recycle header for the copy going on the 2nd pass*/
        BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_recycle_entries_create(unit, sid_idx, &recycle_entry_encap_id_1, &recycle_entry_encap_id_2), "");

        /** Convert from L3_ITF to GPORT */
        BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(terminator_info.encap_id , recycle_entry_encap_id_1);
        terminator_info.nof_sids = sid_idx + max_nof_terminated_usp_sids_1pass + 1;
        /** call the nof_sids API to map to RCH port and encapsulation */
        snprintf(error_msg, sizeof(error_msg), "for nof_sids:%d", sid_idx);
        BCM_IF_ERROR_RETURN_MSG(bcm_srv6_extension_terminator_add(unit, &terminator_info), error_msg);

        if(sid_idx < (nof_2pass_recycle_entries-1))
        {
            BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(terminator_info.encap_id , recycle_entry_encap_id_2);
            terminator_info.flags = BCM_SRV6_EXTENSION_TERMINATOR_ADD_8B_EXTENSION;
            /** call the nof_sids API to map to RCH port and encapsulation */
            snprintf(error_msg, sizeof(error_msg), "for sid_idx:%d, with 8b add", sid_idx);
            BCM_IF_ERROR_RETURN_MSG(bcm_srv6_extension_terminator_add(unit, &terminator_info), error_msg);
        }
     } /** of for sid_idx*/

    return BCM_E_NONE;
}

int srv6_create_extended_termination_recycle_entries_create(
    int unit,
    int nof_sids,
    int *recycle_entry_encap_id_1,
    int *recycle_entry_encap_id_2)
{

    int max_nof_terminated_usp_sids_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int max_nof_terminated_usp_sids = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usp", NULL));
    int nof_2pass_recycle_entries = max_nof_terminated_usp_sids - max_nof_terminated_usp_sids_1pass;

    BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_recycle_entry(unit, nof_sids, recycle_entry_encap_id_1), "");
    if(nof_sids < (nof_2pass_recycle_entries - 1))
    {
        BCM_IF_ERROR_RETURN_MSG(srv6_create_extended_termination_recycle_entry_per_extension_length(unit, 2*nof_sids+1, recycle_entry_encap_id_2), "");
    }
    else
    {
        *recycle_entry_encap_id_2 = 0;
    }
    return BCM_E_NONE;
}
/*
 * Create a Recycle Entry.
 * unit - Relevant unit
 * nof_sids - Number of SIDs to use for the entry
 *           (this is will deduct the needed RCH extension size and egress additional termination)
 * recycle_entry_encap_id - Returned encap id.
 */
int srv6_create_extended_termination_recycle_entry(
    int unit,
    int nof_sids,
    int *recycle_entry_encap_id)
{
    int terminated_sids_usp_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int ingress_nwk_header_term_enable = *(dnxc_data_get (unit, "dev_init", "general", "network_header_termination", NULL));
    int extension_size_support = *(dnxc_data_get (unit, "l2", "general", "l2_egress_max_extention_size_bytes", NULL));
    uint32 egress_filters_enable_per_recycle_port_supported = *dnxc_data_get(unit, "port_pp", "filters", "egress_filters_enable_per_recycle_port_supported", NULL);
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    int cs_profile_id = 5;
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    /** devices with usp 1 pass support, have extended capabilities*/
    if (terminated_sids_usp_1pass)
    {
        recycle_entry.recycle_header_extension_length = (extension_size_support == 0) ? 0 : cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths_extended[nof_sids];
        recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes_extended[nof_sids];
    }
    else
    {
        if(ingress_nwk_header_term_enable)
        {
            recycle_entry.recycle_header_extension_length = cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths_after_itpp[nof_sids];
            recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes_after_itpp[nof_sids];
        }
        else
        {
            recycle_entry.recycle_header_extension_length = cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths[nof_sids];
            recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes[nof_sids];
        }
    }
    recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedTerm;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_create(unit, &recycle_entry), "");
    *recycle_entry_encap_id = recycle_entry.encap_id;

    /** when ibch1 support the recycleApp per port is default, and need to disable egress filters explicitly, base on the RCH out-lif */
    if (egress_filters_enable_per_recycle_port_supported == 0)
    {
        BCM_IF_ERROR_RETURN_MSG(cint_field_epmf_cs_outlif_profile_disable_egress_filters_main(unit, recycle_entry.encap_id, cs_profile_id), "");
    }
    return BCM_E_NONE;
}

/*
 * Create a Recycle Entry.
 * unit - Relevant unit
 * nof_sids - Number of SIDs to use for the entry
 *           (this is will deduct the needed RCH extension size and egress additional termination)
 * recycle_entry_encap_id - Returned encap id.
 */
int srv6_create_extended_termination_recycle_entry_per_extension_length(
    int unit,
    int extension_length,
    int *recycle_entry_encap_id)
{
    int terminated_sids_usp_1pass = *(dnxc_data_get (unit, "srv6", "termination", "max_nof_terminated_sids_usd_1pass", NULL));
    int ingress_nwk_header_term_enable = *(dnxc_data_get (unit, "dev_init", "general", "network_header_termination", NULL));
    int extension_size_support = *(dnxc_data_get (unit, "l2", "general", "l2_egress_max_extention_size_bytes", NULL));
    uint32 egress_filters_enable_per_recycle_port_supported = *dnxc_data_get(unit, "port_pp", "filters", "egress_filters_enable_per_recycle_port_supported", NULL);
    bcm_l2_egress_t recycle_entry;
    bcm_l2_egress_t_init(&recycle_entry);
    int cs_profile_id = 5;
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    /** devices with usp 1 pass support, have extended capabilities*/
    if (terminated_sids_usp_1pass)
    {
        recycle_entry.recycle_header_extension_length = (extension_size_support == 0) ? 0 : cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths_extended_per_extension_length[extension_length];
        recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes_extended_per_extension_length[extension_length];
    }
    else
    {
        if(ingress_nwk_header_term_enable)
        {
            recycle_entry.recycle_header_extension_length = cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths_after_itpp_per_extension_length[extension_length];
            recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes_after_itpp_per_extension_length[extension_length];
        }
        else
        {
            recycle_entry.recycle_header_extension_length = cint_srv6_last_element_to_termination_sizes.recycle_header_extension_lengths_per_extension_length[extension_length];
            recycle_entry.additional_egress_termination_size = cint_srv6_last_element_to_termination_sizes.additional_egress_termination_sizes_per_extension_length[extension_length];
        }
    }
    recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedTerm;
    BCM_IF_ERROR_RETURN_MSG(bcm_l2_egress_create(unit, &recycle_entry), "");
    *recycle_entry_encap_id = recycle_entry.encap_id;

    /** when ibch1 support the recycleApp per port is default, and need to disable egress filters explicitly, base on the RCH out-lif */
    if (egress_filters_enable_per_recycle_port_supported == 0)
    {
        BCM_IF_ERROR_RETURN_MSG(cint_field_epmf_cs_outlif_profile_disable_egress_filters_main(unit, recycle_entry.encap_id, cs_profile_id), "");
    }
    return BCM_E_NONE;
}

/*
 * Add the PSP by Next DIP entry with SL==1 && NEXT DIP
 * unit - Relevant unit
 *
 * -TCAM itself if configures in SRv6 Appl file
 * -Below is adding an entry to above table, hence all qualifiers, action and TCAM FG ids are
 *  reconstructed by their names in Appl
 */
int srv6_psp_tcam_configure(
    int unit,
    int lif_cs_profile,
    int is_next_sid_psp,
    int is_mydip_psp)
{

    bcm_field_entry_info_t ent_info;
    bcm_field_entry_t ent_id;

    /* following is uint32 lsb to msb representation of below DIP fit for PMF qual structure
     * { 0xab, 0xcd, 0xdb, 0xca, 0x12, 0x34, 0x43, 0x21, 0x10, 0x10, 0x98, 0x98, 0x45, 0x67, 0x9a, 0xbc }
     */
    uint32 ipv6_next_dip_1[4] =      { 0x45679abc, 0x10109898, 0x12344321, 0xabcddbca };
    /* following is uint32 lsb to msb representation of below DIP fit for PMF qual structure
     * {0,0,0,0,0,0,0,0,0x12,0x34,0,0,0,0,0xFF,0x13}
     */
    uint32 ipv6_next_dip_2[4] =      { 0x0000ff13, 0x12340000, 0x00000000, 0x00000000 };
    uint32 ip6_mask[4] =             { 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF };
    uint32 zero_ipv6_next_dip_3[4] = { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };
    uint32 zero_ip6_mask[4] =        { 0x00000000, 0x00000000, 0x00000000, 0x00000000 };

    int ii;


    bcm_field_name_to_id_info_t name_to_id_info;
    int nof_ids;
    uint32 id_get;
    void *dest_char;
    char *proc_name;
    bcm_field_qualify_t tcam_sl_qual;
    bcm_field_qualify_t tcam_next_dip_qual;
    bcm_field_action_t tcam_void_action_is_psp;
    bcm_field_group_t ipmf1_tcam_fg;

    proc_name = "srv6_psp_next_sid_configure";

    /*
     * Get the SL==1 Qual id, of PSP TCAM created by SRv6 appl, in order to configure the entries
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    dest_char = &(name_to_id_info.name[0]);
    sal_strncpy_s(dest_char, "srv6_tcam_sl_q", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_get), "for Qual srv6_tcam_sl_q");


    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "%s bcm_field_name_to_id returned %d IDs for Qual srv6_tcam_sl_q\n", proc_name, nof_ids);
    }
    tcam_sl_qual = id_get;

    /*
     * Get the next DIP Qual id, of PSP TCAM created by SRv6 appl, in order to configure the entries
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    dest_char = &(name_to_id_info.name[0]);
    sal_strncpy_s(dest_char, "srv6_tcam_next_dip_q", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdQualifier;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_get), "for Qual srv6_tcam_next_dip_q");


    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "%s bcm_field_name_to_id returned %d IDs for Qual srv6_tcam_next_dip_q\n", proc_name, nof_ids);
    }
    tcam_next_dip_qual = id_get;

    /*
     * Get the is_psp Action id, of PSP TCAM created by SRv6 appl, in order to configure the entries
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    dest_char = &(name_to_id_info.name[0]);
    sal_strncpy_s(dest_char, "srv6_is_psp_tcam_void_act", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdAction;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_get), "for Action srv6_is_psp_tcam_void_act");


    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "%s bcm_field_name_to_id returned %d IDs for Action srv6_is_psp_tcam_void_act\n", proc_name, nof_ids);
    }
    tcam_void_action_is_psp = id_get;

    /*
     * Get the TCAM FG id, of PSP TCAM created by SRv6 appl, in order to configure the entries
     */
    bcm_field_name_to_id_info_t_init(&name_to_id_info);
    dest_char = &(name_to_id_info.name[0]);
    sal_strncpy_s(dest_char, "SRv6_psp_tcam", sizeof(name_to_id_info.name));
    name_to_id_info.name_to_id_type = bcmFieldNameToIdGroup;
    BCM_IF_ERROR_RETURN_MSG(bcm_field_name_to_id(unit, 0, &name_to_id_info, &nof_ids, &id_get), "for FG SRv6_psp_tcam");


    if (nof_ids != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "%s bcm_field_name_to_id returned %d IDs for FG SRv6_psp_tcam\n", proc_name, nof_ids);
    }
    ipmf1_tcam_fg = id_get;


    /*
     * Set Entry-1 for:
     * - Next SID is PSP:  ipv6_next_dip_1
     * - SL==1
     * - MyDIP CS Profile: masked
     */
    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 5000;

    ent_info.nof_entry_quals = 3;

    /** Represents SL==1 */
    ent_info.entry_qual[0].type = tcam_sl_qual;
    ent_info.entry_qual[0].value[0] = 1;
    ent_info.entry_qual[0].mask[0] = 0x1;

    /** Represents default next DIP */
    ent_info.entry_qual[1].type = tcam_next_dip_qual;

    for (ii=0; ii<SRV6_UTILS_WORDS_IN_IP6_DIP; ii++)
    {
        ent_info.entry_qual[1].value[ii] = ipv6_next_dip_1[ii];
        ent_info.entry_qual[1].mask[ii] = ip6_mask[ii];
    }

    /** Represents MyDIP CS LIF Profile - masked */
    ent_info.entry_qual[2].type = bcmFieldQualifyInVportClass0;
    ent_info.entry_qual[2].value[0] = 0;
    ent_info.entry_qual[2].mask[0] = 0x0;

    ent_info.nof_entry_actions = 1;
    ent_info.entry_action[0].type = tcam_void_action_is_psp;
    ent_info.entry_action[0].value[0] = 1;

    if (is_next_sid_psp)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, ipmf1_tcam_fg, &ent_info, &ent_id), "for ipv6_next_dip_1");

        /*
         * Set Entry-2 for:
         * - Next SID is PSP:  ipv6_next_dip_2
         * - SL==1
         * - MyDIP CS Profile: masked
         */
        for (ii=0; ii<SRV6_UTILS_WORDS_IN_IP6_DIP; ii++)
        {
            ent_info.entry_qual[1].value[ii] = ipv6_next_dip_2[ii];
        }

        BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, ipmf1_tcam_fg, &ent_info, &ent_id), "for ipv6_next_dip_2");
    }

    if (is_mydip_psp)
    {
        /*
         * Set Entry-3 for:
         * - MyDIP CS Profile
         * - SL==1
         * - Next SID is PSP: masked
         */

        for (ii=0; ii<SRV6_UTILS_WORDS_IN_IP6_DIP; ii++)
        {
            ent_info.entry_qual[1].value[ii] = zero_ipv6_next_dip_3[ii];
            ent_info.entry_qual[1].mask[ii] = zero_ip6_mask[ii];
        }


        /** Represents MyDIP CS LIF Profile - set on input parameter */
        ent_info.entry_qual[2].value[0] = lif_cs_profile;
        ent_info.entry_qual[2].mask[0] = 0x3;

        BCM_IF_ERROR_RETURN_MSG(bcm_field_entry_add(unit, 0, ipmf1_tcam_fg, &ent_info, &ent_id), "for lif_cs_profile");
    }

    return BCM_E_NONE;
}
