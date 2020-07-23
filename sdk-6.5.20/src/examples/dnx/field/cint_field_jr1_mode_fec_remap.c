/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* $Id: cint_field_jr1_mode_fec_remap.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The learned FEC ID for JR1 is 15 bits, whereas the FEC ID in JR2 is 18 bits wide.
 * The JR2 range of FECs is dynamically allocated, but generally doesn't overlap with the JR1 range.
 * Mapping from the learned JR1 FEC to the JR2 range of FECs is required
 * in order to forward the packet correctly over the learned entry.
 */
/*
 * An example procedure:
 * ./bcm.user
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/dnx/field/cint_field_utils.c
 * cint src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint src/examples/sand/cint_vpls_mp_basic.c
 * cint src/examples/dnx/field/cint_field_presel_fwd_layer.c
 * cint src/examples/dnx/field/cint_field_jr1_mode_fec_remap.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint src/examples/dxn/tunnel/cint_ip_tunnel_to_tunnel_basic.c
 * cint
 * cint_vpls_basic_info.mpls_encap_fec_id=0xd000;
 * cint_vpls_basic_info.mpls_encap_fec_id_primary=0x2222;
 * cint_vpls_basic_info.skip_auto_fec_allocation=1;
 * jr1_mode_remap = 1;
 * cint_fec_remap.fec_id_start_jr1 = 0x2222;
 * cint_fec_remap.fec_id_start_jr2 = 0xd000;
 * cint_fec_remap.nof_entries = 1;
 * int unit = 0;
 * int ac_port = 200;
 * int pwe_port = 201;
 * field_fec_id_remapping_create(unit, ac_port);
 * vpls_mp_basic_main($unit, ac_port, pwe_port);
 * vpls_mp_basic_ve_swap(unit, ac_port, 1);
 * ip_tunnel_to_tunnel_basic(unit, ac_port, pwe_port);
 *
 * tx 1 psrc=200 data=000c0002000100000000cd1d81000000884700d0504000d80140001100000112000c0002000081000005ffff000102030405060708090a0b0c0d0e0f
 *
 * Received packet on unit 0 should be:
 * Data=0x001100000112000c0002000081000005ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 */

struct field_jr1_fec_remap {
    bcm_field_group_t fg_id;
    bcm_field_entry_t entry_ids[10];
    bcm_field_context_t context_id;
    bcm_field_presel_t presel_id;
    uint32 fec_id_start_jr1;
    uint32 fec_id_start_jr2;
    int nof_entries;
    uint32 mask_15b;
    bcm_field_stage_t ipmf_stage;
};

field_jr1_fec_remap cint_fec_remap = {
    /** fg_id */
    0,
    /** entry_ids */
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    /** context_id */
    24,
    /** presel_id */
    16,
    /** fec_id_start_jr1 */
    0x5,
    /** fec_id_start_jr2 */
    0x10005,
    /** nof_entries */
    5,
    /** mask_15b */
    0x7FFF,
    /** ipmf_stage */
    bcmFieldStageIngressPMF1
};



/*
 *   Create a PMF field group and attach the created context to it.
 *      unit - relevant unit ID
 *      fg_id - the index of the field group
 *      port - the expected in port for the packet
 *      context_id - the ID of the previously created IPMF context.
 */
 int
field_fec_id_remapping_group_create(
    int unit,
    bcm_field_group_t *fg_id,
    bcm_field_context_t context_id)
{
    int rv;
    void *dest_char;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeExactMatch;
    fg_info.stage = cint_fec_remap.ipmf_stage;

    /** Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifyDstPort;
    /** Set actions */
    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionRedirect;
    dest_char = &(fg_info.name[0]);
    sal_strncpy_s(dest_char, "J1_FEC_ID_REMAP_FG", sizeof(fg_info.name));
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_group_add\n");
        return rv;
    }

    bcm_field_group_attach_info_t_init(&attach_info);
    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    attach_info.payload_info.action_types[0] = fg_info.action_types[0];
    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    rv = bcm_field_group_context_attach(unit, 0, *fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_field_group_context_attach\n");
        return rv;
    }

    cint_fec_remap.fg_id = *fg_id;

    return rv;
}

/**
 * Add an entry to the previously created Field group.
 * The packet should be redirected from JR1 FEC to JR2 FEC.
 *      unit - relevant unit ID
 *      fg_id - the index of the field group
 *      fec_id_jr1 - the JR1 FEC index
 *      fec_id_jr2 - the JR2 FEC index
 *      entry_id - the index of the created FG entry
 */
int field_fec_id_remapping_entry_add(
    int unit,
    bcm_field_group_t fg_id,
    int fec_id_jr1,
    int fec_id_jr2,
    bcm_field_entry_t * entry_id)
{
    bcm_field_entry_info_t entry_info;
    int rv = BCM_E_NONE;
    bcm_gport_t local_fec_gport;

    BCM_GPORT_FORWARD_PORT_SET(local_fec_gport, fec_id_jr2);

    bcm_field_entry_info_t_init(&entry_info);
    entry_info.nof_entry_quals = 1;
    entry_info.nof_entry_actions = 1;
    entry_info.entry_qual[0].type = bcmFieldQualifyDstPort;
    entry_info.entry_qual[0].value[0] = fec_id_jr1;
    entry_info.entry_qual[0].mask[0] = 0x7FFF ;
    entry_info.entry_action[0].type = bcmFieldActionRedirect;
    entry_info.entry_action[0].value[0] = local_fec_gport;

    rv = bcm_field_entry_add(unit, 0, fg_id, &entry_info, entry_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    printf("Entry %d is installed in FG %d, convert the global FEC 0x%x to local FEC 0x%x\n",
            *entry_id, fg_id, fec_id_jr1, fec_id_jr2);

    return rv;

}

/**
 * Main configuration function:
 *  1. Create a preselector and a context for the given IPMF stage.
 *  2. Create the field group and attach the context to it.
 *  3. Add all needed entries to the field group.
 */
int field_fec_id_remapping_create(
    int unit,
    int access_port)
{
    int rv = BCM_E_NONE;
    int idx;
    bcm_field_group_t fg_id;
    char *ctx_name = "exem_ctx";
    bcm_field_layer_type_t eth_layer = bcmFieldLayerTypeEth;

    if (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0) {
        rv = field_presel_fwd_layer_main(unit, cint_fec_remap.presel_id, cint_fec_remap.ipmf_stage,
                eth_layer, &cint_fec_remap.context_id, ctx_name);
        if (rv != BCM_E_NONE)
        {
            printf("Error, field_presel_fwd_layer_main\n");
            return rv;
        }
        rv = field_fec_id_remapping_group_create(unit, &cint_fec_remap.fg_id, cint_fec_remap.context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, field_fec_id_remapping_group_create\n");
            return rv;
        }

        for (idx = 0; idx < cint_fec_remap.nof_entries; idx++)
        {
            bcm_field_entry_t entry_id;
            rv = field_fec_id_remapping_entry_add(unit, cint_fec_remap.fg_id, (cint_fec_remap.fec_id_start_jr1 + idx) & cint_fec_remap.mask_15b,
                    cint_fec_remap.fec_id_start_jr2 + idx, &entry_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, field_fec_id_remapping_entry_add\n");
                return rv;
            }
            cint_fec_remap.entry_ids[idx] = entry_id;
        }
    }

    return rv;
}

/**
 * Main clean-up function:
 *  1. Delete all entries from the field group.
 *  2. Detach the context from the field group and destroy the FG.
 *  3. Destroy the preselector and the context.
 */
int field_fec_id_remapping_destroy(
    int unit)
{
    int idx;
    int rv = BCM_E_NONE;
    bcm_field_entry_qual_t entry_qual_info[BCM_FIELD_NUMBER_OF_QUALS_PER_GROUP];
    bcm_field_entry_qual_t_init(&entry_qual_info);
    if (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0) {
        /** Destroy all entries added to the Field group */
        for (idx = 0; idx < cint_fec_remap.nof_entries; idx++)
        {
            entry_qual_info[0].type = bcmFieldQualifyDstPort;
            entry_qual_info[0].value[0] = (cint_fec_remap.fec_id_start_jr1 + idx);
            rv = bcm_field_entry_delete(unit, cint_fec_remap.fg_id, entry_qual_info, cint_fec_remap.entry_ids[idx]);
            if (rv != BCM_E_NONE)
            {
                printf("Error, bcm_field_entry_delete\n");
                return rv;
            }
        }
        /** Detach context from FG */
        rv = bcm_field_group_context_detach(unit, cint_fec_remap.fg_id, cint_fec_remap.context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_field_group_context_detach\n");
            return rv;
        }
        /** Destroy the field group */
        rv = bcm_field_group_delete(unit, cint_fec_remap.fg_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_field_group_delete fg %d \n", rv, cint_fec_remap.fg_id);
            return rv;
        }
        rv = field_presel_fwd_layer_destroy(unit, cint_fec_remap.presel_id, cint_fec_remap.ipmf_stage, cint_fec_remap.context_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in field_presel_fwd_layer_destroy \n", rv);
            return rv;
        }
    }

    return rv;
}
