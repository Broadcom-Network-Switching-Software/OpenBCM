uint32 rcpu_port_class_id = 3;

/*
 * Example of update ipmf1 context for injected y1711 oam from rcpu
 *
 * Purpose: make sure ASE header in injected y1711 not to be rebuilt
 *
 * @param unit
 * @param rcpu_port - the source port of injected y1711 oam packet 
 *
 * @return int
 */
int
cint_dnx_oam_injected_y1711_context_update (
    int unit, 
    int rcpu_port)
{
    bcm_error_t rv;

    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_context_param_info_t param_info;
    bcm_field_presel_entry_id_t presel_entry_id;
    bcm_field_context_t itmh_context_id;

    itmh_context_id = *(dnxc_data_get(unit, "field", "context", "default_itmh_context", NULL));
 
    bcm_field_presel_entry_id_info_init(&presel_entry_id);
    presel_entry_id.presel_id = *(dnxc_data_get(unit, "field", "preselector", "default_itmh_y1711_rcpu_presel_id_ipmf1", NULL));
    presel_entry_id.stage = bcmFieldStageIngressPMF1;

    bcm_field_presel_entry_data_info_init(&presel_entry_data);
    presel_entry_data.nof_qualifiers = 4;
    presel_entry_data.context_id = itmh_context_id;
    presel_entry_data.entry_valid = TRUE;

    /** Check that the FWD type is ITMH. */
    presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[0].qual_arg = 0;
    presel_entry_data.qual_data[0].qual_value = bcmFieldLayerTypeTm;
    presel_entry_data.qual_data[0].qual_mask = -1;
    /** Check that bit 1 (PPH_TYPE) of the LayerQualifier are set to 1. */
    presel_entry_data.qual_data[1].qual_type = bcmFieldQualifyForwardingLayerQualifier;
    presel_entry_data.qual_data[1].qual_arg = 0;
    presel_entry_data.qual_data[1].qual_value = 0x0001;
    presel_entry_data.qual_data[1].qual_mask = 0x0001;
    /**Check that the FWD type     3 is MPLS      */
    presel_entry_data.qual_data[2].qual_type = bcmFieldQualifyForwardingType;
    presel_entry_data.qual_data[2].qual_arg = 3;
    presel_entry_data.qual_data[2].qual_value = bcmFieldLayerTypeMpls;
    presel_entry_data.qual_data[2].qual_mask = 0x1F;

    presel_entry_data.qual_data[3].qual_type = bcmFieldQualifyPortClassPacketProcessing;
    presel_entry_data.qual_data[3].qual_arg = 0;
    presel_entry_data.qual_data[3].qual_value = rcpu_port_class_id;
    presel_entry_data.qual_data[3].qual_mask = 0x7;

    rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    if (rv != BCM_E_NONE) {
        printf("Fail in bcm_field_presel_set\n");
        return rv;
    }

    rv = bcm_port_class_set(unit, rcpu_port, bcmPortClassFieldIngressPMF1PacketProcessingPortCs, rcpu_port_class_id);
    if (rv != BCM_E_NONE) {
        printf("Fail in bcm_port_class_set\n");
        return rv;
    }

    return rv;
}


