/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        ft_diag.c
 * Purpose:     The purpose of this file is to set flow tracker group methods.
 * Requires:
 */


#include <bcm_int/esw/flowtracker/ft_group.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)


/* Group Sw state. */
extern
bcmi_ft_group_sw_info_t **bcmi_ft_group_sw_state[BCM_MAX_NUM_UNITS];

int bcmi_alu_opr_conversion[8] =
        {5, 6, 1, 2, 3, 4, 7, 8};

int bcmi_alu_action_conversion[9] =
        {0, 1, 7, 2, 3, 4, 5, 6, -1};

char trackingelements[bcmFlowtrackerTrackingParamTypeCount] [100] =
   {"None",
    "SrcIPv4",
    "DstIPv4",
    "SrcIPv6",
    "DstIPv6",
    "L4SrcPort",
    "L4DstPort",
    "IPProtocol",
    "PktCount",
    "ByteCount",
    "VRF",
    "TTL",
    "IPLength",
    "TcpWindowSize",
    "DosAttack",
    "VxlanNetworkId",
    "NextHeader",
    "HopLimit",
    "InnerSrcIPv4",
    "InnerDstIPv4",
    "InnerSrcIPv6 ",
    "InnerDstIPv6",
    "InnerIPProtocol",
    "InnerTTL ",
    "InnerNextHeader",
    "InnerHopLimit",
    "InnerL4SrcPort",
    "InnerL4DstPort",
    "TcpFlags",
    "OuterVlanTag",
    "IP6Length",
    "InnerIPLength",
    "InnerIP6Length"};


char elementType[3][100] =
   {"Flowchecker",
    "Tracking Param",
    "TimeStamp"};

/*
 * Function:
 *     bcmi_ft_table_lookup
 * Purpose:
 *     Looks up ft entry at 5 tuple.
 * Parameters:
 *     unit - (IN) BCM device id
 *     id   - (IN) FT group id.
 *     v4-5tuple.- (IN) 5 tuple values, DIP,sip,srcport,dstport, protocol
 *     index - (OUT) memory index.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_table_lookup(
    int unit,
    bcm_flowtracker_group_t id,
    bcm_ip_t dip,
    bcm_ip_t sip,
    uint16 l4destport,
    uint16 l4srcport,
    uint8 proto,
    int *index)
{
    uint32 ft_key[50], ft_ret[50];
    uint8 data[40];
    uint8 *p;
    int rv = 0;
    int i;
    uint8 mode = 0;
    soc_mem_t mem;
    int b1=0, b2=0, k1=0, k2=0;
    int p_val = 0;

    if (!index) {
        return BCM_E_FAIL;
    }

    if (!BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Group index (%d) is not created or validated yet.\n "), id));

        return BCM_E_CONFIG;
    }

    sal_memset(data, 0, sizeof(data));
    sal_memset(ft_key, 0, sizeof(ft_key));
    sal_memset(ft_ret, 0, sizeof(ft_ret));

    mode = (BCMI_FT_GROUP_FTFP_DATA(unit, id).session_key_mode ==
               bcmiFtGroupModeDouble) ? 1 :0;

    if (mode) {
        mem = FT_KEY_DOUBLEm;
        b1 = BASE_VALID_0f;
        b2 = BASE_VALID_1f;
        k1 = KEY_0f;
        k2 = KEY_1f;
    } else {
        mem = FT_KEY_SINGLEm;
        b1 = b2 = BASE_VALIDf;
        k1 = k2 = KEYf;
    }

    soc_mem_field_set(unit, mem, ft_key, k2, (uint32 *)data);
    /* Key Type. */
    data[0] = mode;
    p_val = 0;
    p = (uint8 *)&dip;
    p_val++;
    for (i=0; i<4; i++) {
        data[p_val+i] = p[i];
    }
    p_val += 4;

    p = (uint8 *)&sip;
    for (i=0; i<4; i++) {
        data[p_val+i] = p[i];
    }
    p_val += 4;

    if (l4srcport) {
        p = (uint8 *)&l4srcport;
        for (i=0; i<2; i++) {
            data[p_val+i] = p[i];
        }
        p_val += 2;
    }

    if (l4destport) {

        p = (uint8 *)&l4destport;
        for (i=0; i<2; i++) {
            data[p_val+i] = p[i];
        }
        p_val += 2;
    }

    if (proto) {
        data[p_val] = proto;
    }

    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "soc_mem_insert : KEY : DIP=0x%x, SIP=0x%x, l4destport = %d, l4srcport=%d, proto = %d \n"),
         dip, sip, l4destport, l4srcport, proto));

    LOG_CLI((BSL_META_U(unit, "Printing Key")));

    for (i=15; i>=0; i--) {
        LOG_CLI((BSL_META_U(unit, "%x"), data[i]));
    }

    LOG_CLI((BSL_META_U(unit, "\n")));
    soc_mem_field32_set(unit, mem, ft_key, b1, 1);
    soc_mem_field32_set(unit, mem, ft_key, b2, 1);

    soc_mem_field_set(unit, mem, ft_key, k1, (uint32 *)data);

    rv = soc_mem_search(unit, mem, MEM_BLOCK_ANY, index,
                       &ft_key, &ft_ret, 0);

    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "soc_mem_search : rv = %d, index = %d \n"),
        rv, *index));

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmi_ft_table_insert
 * Purpose:
 *     Insert entry into ft table.
 * Parameters:
 *     unit - (IN) BCM device id
 *     v4-5tuple.- (IN) 5 tuple values, DIP,sip,srcport,dstport, protocol
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_table_insert(
    int unit,
    bcm_ip_t dip,
    bcm_ip_t sip,
    uint16 l4destport,
    uint16 l4srcport,
    uint8 proto)
{
    ft_key_single_entry_t ft_key;
    uint8 data[40];
    uint8 *p;
    int rv = 0;
    int i;

    sal_memset(&data, 0, 40);

    /* Key Type. */
    data[0] = 0;

    p = (uint8 *)&dip;

    for (i=0; i<4; i++) {
        data[1+i] = p[i];
    }

    p = (uint8 *)&sip;
    for (i=0; i<4; i++) {
        data[5+i] = p[i];
    }

    p = (uint8 *)&l4srcport;
    for (i=0; i<2; i++) {
        data[9+i] = p[i];
    }

    p = (uint8 *)&l4destport;
    for (i=0; i<2; i++) {
        data[11+i] = p[i];
    }

    data[13] = proto;

    sal_memset(&ft_key, 0, sizeof(ft_key_single_entry_t));

    LOG_ERROR(BSL_LS_BCM_FLOWTRACKER,
        (BSL_META_U(unit, "soc_mem_insert : KEY : DIP=0x%x, SIP=0x%x, l4destport = %d, l4srcport=%d, proto = %d \n"),
        dip, sip, l4destport, l4srcport, proto));


    soc_mem_field32_set(unit, FT_KEY_SINGLEm, &ft_key, BASE_VALIDf, 1);
    soc_mem_field_set(unit, FT_KEY_SINGLEm, (uint32*)&ft_key, KEYf, (uint32*)data);

    rv = soc_mem_insert(unit, FT_KEY_SINGLEm, MEM_BLOCK_ANY, &ft_key);

    return rv;
}

void
bcmi_ft_print_alu_load_type_str(int unit, int type)
{
        LOG_CLI((BSL_META_U(unit, "Type = ")));
        if (type == bcmiFtAluLoadTypeLoad16) {

            LOG_CLI((BSL_META_U(unit, "bcmiFtAluLoadTypeLoad16")));
        } else if (type == bcmiFtAluLoadTypeLoad8) {

            LOG_CLI((BSL_META_U(unit, "bcmiFtAluLoadTypeLoad8")));
        } else if (type == bcmiFtAluLoadTypeAlu32) {

            LOG_CLI((BSL_META_U(unit, "bcmiFtAluLoadTypeAlu32")));
        } else if (type == bcmiFtAluLoadTypeAlu16) {
            LOG_CLI((BSL_META_U(unit, "bcmiFtAluLoadTypeAlu16")));
        } else {
            LOG_CLI((BSL_META_U(unit, "bcmiFtAluLoadTypeNone")));
        }
}

/*
 * Function:
 *     bcmi_ft_group_state_dump
 * Purpose:
 *     Insert entry into ft table.
 * Parameters:
 *     unit - (IN) BCM device id
 *       id - (IN) Group index to dump.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_state_dump(
    int unit,
    bcm_flowtracker_group_t id)
{
    int rv =  BCM_E_NONE;
    int num_alus_loads = 0, i=0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_alu_load_type_t type = bcmiFtAluLoadTypeNone;
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;
    bcm_collector_t cmn_coll_id;
    bcm_flowtracker_collector_t hw_coll_id;
    int total_mems = 0;
    int mem_idx = 0;
    bcm_flowtracker_tracking_param_info_t *t_param;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    char direction_str[20] = {'\0'};

    if (bcmi_ft_group_is_invalid(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Group index (%d) is yet not created.\n "), id));

        return BCM_E_NONE;
    }

    LOG_CLI((BSL_META_U(unit, " ************************************** \n\r")));

    /* Print Group Management Status */
    if (BCMI_FT_GROUP(unit, id)->flags &
            BCM_FLOWTRACKER_GROUP_USER_ENTRIES_ONLY) {
        LOG_CLI((BSL_META_U(unit, "--Software Managed Group--\n\r")));
    } else {
        LOG_CLI((BSL_META_U(unit, "--Hardware Managed Group--\n\r")));
    }

    /* Flowtracker Group Class Id */
    LOG_CLI((BSL_META_U(unit, "Class Identifier              :\t%5d\n\r"),
                BCMI_FT_GROUP_CONFIG(unit, id).class_id));

    /* Flowtracker Group Controls */
    if (BCMI_FT_GROUP_FTFP_DATA(unit, id).direction ==
            bcmFlowtrackerFlowDirectionNone) {
        sal_snprintf(direction_str, 20, "%s", "None");
    } else if (BCMI_FT_GROUP_FTFP_DATA(unit, id).direction ==
            bcmFlowtrackerFlowDirectionForward) {
        sal_snprintf(direction_str, 20, "%s", "Forward");
    } else if (BCMI_FT_GROUP_FTFP_DATA(unit, id).direction ==
            bcmFlowtrackerFlowDirectionReverse) {
        sal_snprintf(direction_str, 20, "%s", "Reverse");
    } else if (BCMI_FT_GROUP_FTFP_DATA(unit, id).direction ==
            bcmFlowtrackerFlowDirectionBidirectional) {
        sal_snprintf(direction_str, 20, "%s", "Bidirectional");
    }

    LOG_CLI((BSL_META_U(unit, "Flowtracker Status            :\t%5d\n\r"),
                BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack));
    LOG_CLI((BSL_META_U(unit, "New Learn Status              :\t%5d\n\r"),
            BCMI_FT_GROUP_FTFP_DATA(unit, id).new_learn));
    LOG_CLI((BSL_META_U(unit, "Direction                     :\t%5s\n\r"),
                direction_str));

   /* Print associated Collector */
    if (BCMI_FT_COLLECTOR_ID_INVALID !=
            BCMI_FT_GROUP(unit, id)->collector_id) {
        hw_coll_id = BCMI_FT_GROUP(unit, id)->collector_id;
        cmn_coll_id = (BCMI_FT_COLLECTOR_INFO(unit, hw_coll_id))->cmn_coll_id;
        LOG_CLI((BSL_META_U(unit, "Associated Collector(s/w Id)  :\t%5d\n\r"),
                    cmn_coll_id));
        LOG_CLI((BSL_META_U(unit, "Associated Collector(h/w Id)  :\t%5d\n\r"),
                    hw_coll_id));
    } else {
        LOG_CLI((BSL_META_U(unit, "Associated Collector          :\t%5s\n\r"),
                "(NA)"));
    }

    /* Print associated Template */
    if (BCMI_FT_TEMPLATE_ID_INVALID !=
            BCMI_FT_GROUP(unit, id)->template_id) {
        LOG_CLI((BSL_META_U(unit, "Associated Template           :\t%5d\n\r"),
                BCMI_FT_GROUP(unit, id)->template_id));
    } else {
        LOG_CLI((BSL_META_U(unit, "Associated Template           :\t%5s\n\r"),
                "(NA)"));
    }

    /* Association with FTFP Entries */
    LOG_CLI((BSL_META_U(unit, "Associated with Field Entries :\t%5d\n\r"),
                BCMI_FT_GROUP(unit, id)->num_ftfp_entries));

    if (!BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
            "Group index (%d) is not validated yet.\n "), id));

        return BCM_E_NONE;
    }

    /* Metering info */
    LOG_CLI((BSL_META_U(unit, "Meter Rate (Kbps/pps)         :\t%5d\n\r"),
                BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_sec));
    LOG_CLI((BSL_META_U(unit, "Meter burst (Kbits/Kpkts)     :\t%5d\n\r"),
                BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_burst));

    LOG_CLI((BSL_META_U(unit, "\n===== Dumping Tracking Parameters =====\n")));

    t_param = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
    for (i=0; i<(BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id)); i++) {

        LOG_CLI((BSL_META_U(unit, "%d. "), i));
        LOG_CLI((BSL_META_U(unit, "flags = %-5d | "), t_param->flags));
        LOG_CLI((BSL_META_U(unit, "Parameter type -")));
        LOG_CLI((BSL_META_U(unit, " %20s"), plist[t_param->param]));

        t_param++;
        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    LOG_CLI((BSL_META_U(unit, "\n ===== Dumping extractor Data ====\n")));

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Get total number of alus in this chunk. */
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    LOG_CLI((BSL_META_U(unit, "Total elements  = %d\n"), num_alus_loads));
    /* allocate the bitmap based on the type, length */
    for (i=0; i<num_alus_loads; i++) {

         /* Now get the type of the memory and take action. */
         LOG_CLI((BSL_META_U
             (unit, " ************ Session Data - Element %d    ************"),
             i+1));

         LOG_CLI((BSL_META_U(unit, "\n")));

         if (temp->flowchecker_id) {
            sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
            LOG_CLI((BSL_META_U(unit, "FlowCheck Id = 0x%x\n"), temp->flowchecker_id));
            BCM_IF_ERROR_RETURN(bcmi_ft_flowchecker_get
                (unit, temp->flowchecker_id, &fc_info));

            LOG_CLI((BSL_META_U(unit,
                "Element1 -")));
            LOG_CLI((BSL_META_U(unit, " %20s, "), plist[fc_info.check1.param]));

            if (fc_info.check2.param) {
                LOG_CLI((BSL_META_U(unit,
                    "Element2 -")));
                LOG_CLI((BSL_META_U(unit, " %20s"), plist[fc_info.check2.param]));
            }
        } else {
            LOG_CLI((BSL_META_U(unit,
                 "Element -")));
            LOG_CLI((BSL_META_U(unit, " %20s"), plist[temp->element_type1]));
         }

        LOG_CLI((BSL_META_U(unit, "\n")));

        type = temp->alu_load_type;
        bcmi_ft_print_alu_load_type_str(unit, type);

        LOG_CLI((BSL_META_U(unit, "\n Element1 - Length = %d, IS_ALU = %d,"
            " location = %d"),
            temp->key1.length, temp->key1.is_alu, temp->key1.location));

        if (temp->flowchecker_id) {
            LOG_CLI((BSL_META_U(unit, "\n Element2 - Length = %d, IS_ALU = %d,"
                " location = %d"),
                temp->key2.length, temp->key2.is_alu, temp->key2.location));
        }

        if (type == bcmiFtAluLoadTypeAlu16) {
            bcmi_ft_alu16_dump(unit, temp->alu_load_index);
        } else if (type == bcmiFtAluLoadTypeAlu32) {
            bcmi_ft_alu32_dump(unit, temp->alu_load_index);
        } else {
            LOG_CLI((BSL_META_U(unit, "\n This element belongs to Load memory\n")));
        }

        LOG_CLI((BSL_META_U(unit, "\n")));
        temp++;
    }

    LOG_CLI((BSL_META_U(unit, "Dump PDD profile entry.\n")));
    /* For all the types of memory types. */
    for (type=0; type<4; type++) {

        total_mems = alu_load_index_info[unit][type].total_mem_count;

        /* Set sub memories for Load memories. */
         if (type == bcmiFtAluLoadTypeLoad8) {
             total_mems = 8;
         } else if (type == bcmiFtAluLoadTypeLoad16) {
             total_mems = 16;
         }

        /* For all the alu/load types, check what memories are set in pdd. */
        for (mem_idx=0; mem_idx<total_mems; mem_idx++) {
            /* Now get if this particular field is set in pdd. */
            pdd_info = alu_load_index_info[unit][type].pdd_bus_info;
            if (!pdd_info) {
                return BCM_E_INTERNAL;
            }
            if (soc_mem_field32_get(unit, BSD_POLICY_ACTION_PROFILEm,
                    (uint32 *)(&(BCMI_FT_GROUP(unit, id)->pdd_entry)),
                    pdd_info[mem_idx].param_id)) {
                LOG_CLI((BSL_META_U(unit, "Index=%d of "),
                    mem_idx));
                bcmi_ft_print_alu_load_type_str(unit, type);
                LOG_CLI((BSL_META_U(unit, " is set.\n")));

            }
        }
    }

    LOG_CLI((BSL_META_U(unit, "Group Dump Completed.\n")));
    return rv;
}

/*
 * Function:
 *     bcmi_ft_group_session_data_dump
 * Purpose:
 *     Dump the session data.
 * Parameters:
 *     unit - (IN) BCM device id
 *       id - (IN) Group index to dump.
 *     index - (IN) FT index to dump.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_group_session_data_dump(
    int unit,
    bcm_flowtracker_group_t id,
    int index)
{
    int rv = BCM_E_NONE;
    int data_bytes;
    bcmi_ft_group_template_list_t *temp;
    uint8 *data = NULL;
    uint32 *buffer = NULL;
    int length = 0, start=32, i;
    uint32 value = 0;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    bcm_flowtracker_tracking_param_type_t element;
    uint8 mode = 0;
    soc_mem_t mem;
    int etype = 0;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    int sub_len=0, first_value=0;

    if (!BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {

        LOG_CLI((BSL_META_U(unit, "Group index (%d) is NOT validated \n "), id));

        return BCM_E_CONFIG;
    } else {
        LOG_CLI((BSL_META_U(unit, "Group index (%d) is validated \n "), id));
    }
    if (BCMI_FT_GROUP_IS_BUSY(unit, id)) {
        LOG_CLI((BSL_META_U(unit, "Group index (%d) is associated with FTFP Entry \n "), id));
    }

    /* Get the head of the templs info plist. */
    temp = ((BCMI_FT_GROUP(unit, id))->template_head);

    mode = (BCMI_FT_GROUP_FTFP_DATA(unit, id).session_key_mode ==
               bcmiFtGroupModeDouble) ? 1 :0;

    if (mode) {
        start = (start << mode);
        mem = BSC_DT_FLEX_SESSION_DATA_DOUBLEm;
    } else {
        mem = BSC_DT_FLEX_SESSION_DATA_SINGLEm;
    }

    data_bytes = soc_mem_field_length(unit, mem, OPAQUE_DATAf);
    data_bytes = BITS2BYTES(data_bytes);
    data = (uint8 *) sal_alloc(data_bytes * sizeof(uint8), "session data");
    if (data == NULL) {
        return BCM_E_MEMORY;
    }
    buffer = (uint32 *) sal_alloc(sizeof(uint32) * SOC_MAX_MEM_WORDS, "data buffer");
    if (buffer == NULL) {
        sal_free(data);
        return BCM_E_MEMORY;
    }

    sal_memset(data, 0, data_bytes * sizeof(uint8));
    sal_memset(buffer, 0, SOC_MAX_MEM_WORDS * sizeof(uint32));

    /* Read the entry also. */
    rv = soc_mem_read(unit, mem,
        MEM_BLOCK_ANY, index, (uint32 *)buffer);
    if (BCM_FAILURE(rv)) {
        sal_free(data);
        sal_free(buffer);
        return rv;
    }

    soc_mem_field_get(unit, mem, (uint32 *) buffer,
            OPAQUE_DATAf, (uint32 *) data);

    start = start -1;

    LOG_CLI((BSL_META_U(unit, "\n ***********************************************\n")));
    LOG_CLI((BSL_META_U(unit, "\n *****      Session Data Dump Follows      *****\n")));
    LOG_CLI((BSL_META_U(unit, "\n ***********************************************\n")));


    LOG_CLI((BSL_META_U(unit, "\n")));
    while (temp) {

        value = 0;
        sub_len = 0;
        length = temp->info.cont_width / 8;

        if ((temp->info.param_type == bcmiFtGroupParamTypeTsNewLearn) ||
            (temp->info.param_type == bcmiFtGroupParamTypeTsFlowStart) ||
            (temp->info.param_type == bcmiFtGroupParamTypeTsFlowEnd) ||
            (temp->info.param_type == bcmiFtGroupParamTypeTsCheckEvent1) ||
            (temp->info.param_type == bcmiFtGroupParamTypeTsCheckEvent2)) {

            etype = 2;
            element = bcmi_ft_group_get_tracking_param
                          (unit, temp->info.param_type);
            sub_len = 2;

        } else if (temp->info.check_id) {
            rv = bcmi_ft_flowchecker_get
                (unit, temp->info.check_id, &fc_info);
            if (BCM_FAILURE(rv)) {
                sal_free(data);
                sal_free(buffer);
                return rv;
            }

            element = fc_info.check1.param;
            etype = 0;

        } else if (temp->info.param_type == bcmiFtGroupParamTypeTracking) {
            etype = 1;
            element = temp->info.param;

        } else {
            start -=  length;
            temp = temp->next;
            continue;
        }
        first_value = 0;
        for (i=0; i<length; i++) {

            if (sub_len && (length > sub_len)) {
                while (i < sub_len) {
                    first_value =
                       ((first_value) |
                           ((data[start-i]) << (((sub_len-1)-i) * 8)));
                    i++;
                    continue;
                }
            }

            value = ((value) | ((data[start-i]) << (((length-1)-i) *8)));
        }
        start -= length;

        LOG_CLI((BSL_META_U(unit,
            "Element -")));
        LOG_CLI((BSL_META_U(unit, " %25s "), plist[element]));
        LOG_CLI((BSL_META_U(unit, "( %-15s)  | "), elementType[etype] ));

        /*bcmi_ft_group_element_print(unit, element);*/
        if ((element == bcmFlowtrackerTrackingParamTypePktCount) ||
            (element == bcmFlowtrackerTrackingParamTypeByteCount)) {
            LOG_CLI((BSL_META_U(unit,
                " Value -  %-15u"), value));
        } else {
            if (sub_len) {
                /* Currently first value is only for timestamping. */
                LOG_CLI((BSL_META_U(unit,
                    " Value -  %dsec, %dnsec  "), first_value, value));
            } else {

                LOG_CLI((BSL_META_U(unit,
                   " Value -  0x%-15x"), value));
            }
        }

        LOG_CLI((BSL_META_U(unit, "\n")));

        temp = temp->next;

        /* We still need to add data for flowchecker. */
    }

    if (data != NULL) {
        sal_free(data);
    }
    if (buffer != NULL) {
        sal_free(buffer);
    }

    return rv;
}

/*
 * Function:
 *     bcmi_ft_alu32_dump
 * Purpose:
 *     Dump the alu32 data.
 * Parameters:
 *     unit - (IN) BCM device id
 *       id - (IN) Group index to dump.
 *     index - (IN) index to dump.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu32_dump(
    int unit,
    int global_index)
{

    int rv = BCM_E_NONE;
    int new_index = -1;
    soc_mem_t mem = -1;
    int a_idx = -1; /* Array index of field/memory array. */
    uint32 alu_fmt[3];
    uint32 alu32_specific_ctrlfmt[4];
    uint32 alu32_ctrlfmt[6];
    bsc_dg_group_alu32_profile_0_entry_t alu32_entry;
    uint32 val0, val1, opr0, opr1, check0_attr, check1_attr, update_opr;
    uint32 gran0, gran1, update_opr_param, export_mode, clear_on_export;
    uint32 export_opr, export_threshold;
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeAlu32;
    uint32 load_trigger = 0, update_attr = 0;
    const char *olist[] = BCM_FT_CHECK_OPERATION_STRINGS;
    const char *alist[] = BCM_FT_CHECK_ACTION_STRINGS;


    sal_memset(alu_fmt, 0, 3 * sizeof(uint32));
    sal_memset(alu32_specific_ctrlfmt, 0, 4*sizeof(uint32));
    sal_memset(alu32_ctrlfmt, 0, 6*sizeof(uint32));


    bcmi_ft_alu_load_mem_index_get
        (unit, global_index, alu_load_type, &new_index, &mem, &a_idx);


    /* The profile is set. Write into the index of group. */
    rv  = soc_mem_read(unit, mem, MEM_BLOCK_ALL, new_index, &alu32_entry);

    if (BCM_FAILURE(rv)) {
        return rv;
    }


    soc_mem_field_get(unit, mem, (uint32 *)&alu32_entry, DATAf,
        alu32_ctrlfmt);


    soc_format_field_get(unit, BSC_TL_DG_TO_DT_ALU32_CONTROL_BUSfmt,
        alu32_ctrlfmt, ALU32_SPECIFIC_CONTROL_BUSf, alu32_specific_ctrlfmt);


    val0 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, CHECK_0_THRESHOLDf);
    val1 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, CHECK_1_THRESHOLDf);

    gran0 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, CHECK_0_ATTR_SELECT_GRANf);
    gran1 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, CHECK_1_ATTR_SELECT_GRANf);

    export_threshold = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, EXPORT_THRESHOLDf);

    soc_format_field_get(unit, BSC_TL_DG_TO_DT_ALU32_CONTROL_BUSfmt,
        alu32_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_fmt);

    check0_attr = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_0_ATTR_SELECTf);

    check1_attr = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_1_ATTR_SELECTf);


    opr0 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_0_OPERATIONf);

    opr1 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_1_OPERATIONf);


    update_attr = soc_format_field32_get(unit,
                  BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                  alu_fmt, UPDATE_ATTR_SELECTf);

    update_opr = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, UPDATE_OPERATIONf);

    update_opr_param = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, UPDATE_OP_PARAMf);

    load_trigger = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, LOAD_TRIGGERf);

    export_opr =  soc_format_field32_get(unit,
                    BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                    alu_fmt, EXPORT_OPERATIONf);

    export_mode =  soc_format_field32_get(unit,
                    BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                    alu_fmt, EXPORT_CHECK_MODEf);

    clear_on_export =  soc_format_field32_get(unit,
                    BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                    alu_fmt, CLEAR_ON_EXPORT_ENABLEf);

    LOG_CLI((BSL_META_U(unit, "\n******* ALu32 Global index = %d ******\n "), global_index));

    LOG_CLI((BSL_META_U(unit, "val0 = 0x%x\n "), val0));
    opr0 = bcmi_alu_opr_conversion[opr0];
    LOG_CLI((BSL_META_U(unit, "opr0 = %s\n "), olist[opr0]));
    LOG_CLI((BSL_META_U(unit, "check0_attr = %d\n "), check0_attr));
    LOG_CLI((BSL_META_U(unit, "check0_gran = %d\n "), gran0));

    LOG_CLI((BSL_META_U(unit, "val1 = 0x%x\n "), val1));
    opr1 = bcmi_alu_opr_conversion[opr1];
    LOG_CLI((BSL_META_U(unit, "opr1 = %s\n "), olist[opr1]));
    LOG_CLI((BSL_META_U(unit, "check1_attr = %d\n "), check1_attr));
    LOG_CLI((BSL_META_U(unit, "check1_gran = %d\n "), gran1));

    LOG_CLI((BSL_META_U(unit, "Update_attr = %d\n "), update_attr));
    update_opr = bcmi_alu_action_conversion[update_opr];
    if (update_opr == -1) {
        LOG_CLI((BSL_META_U(unit, "update_opr = bitwise_or\n ")));
    } else {
        LOG_CLI((BSL_META_U(unit, "update_opr = %s\n "), alist[update_opr]));
    }
    LOG_CLI((BSL_META_U(unit, "update_opr_param = %d\n "), update_opr_param));
    LOG_CLI((BSL_META_U(unit, "load_trigger = %d\n "), load_trigger));

    export_opr = bcmi_alu_opr_conversion[export_opr];
    LOG_CLI((BSL_META_U(unit, "export_opr = %s\n "), olist[export_opr]));
    LOG_CLI((BSL_META_U(unit, "export_threshold = %d\n "), export_threshold));
    LOG_CLI((BSL_META_U(unit, "export data = %d, %d\n "), export_mode, clear_on_export));

    LOG_CLI((BSL_META_U(unit, "************************************************\n ")));

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmi_ft_alu16_dump
 * Purpose:
 *     Dump the alu16 data.
 * Parameters:
 *     unit - (IN) BCM device id
 *       id - (IN) Group index to dump.
 *     index - (IN) index to dump.
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcmi_ft_alu16_dump(
    int unit,
    int global_index)
{

    int rv = BCM_E_NONE;
    int new_index = -1;
    soc_mem_t mem = -1;
    int a_idx = -1; /* Array index of field/memory array. */
    int load_trigger = 0;
    uint32 alu_fmt[3];
    uint32 alu16_specific_ctrlfmt[4];
    uint32 alu16_ctrlfmt[6];
    bsc_dg_group_alu16_profile_0_entry_t alu16_entry;
    uint32 val0, val1, opr0, opr1, check0_attr, check1_attr, update_opr;
    uint32 export_opr = 0, export_threshold = 0, update_attr = 0;
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeAlu16;
    const char *olist[] = BCM_FT_CHECK_OPERATION_STRINGS;
    const char *alist[] = BCM_FT_CHECK_ACTION_STRINGS;

    sal_memset(alu_fmt, 0, 3 * sizeof(uint32));
    sal_memset(alu16_specific_ctrlfmt, 0, 4*sizeof(uint32));
    sal_memset(alu16_ctrlfmt, 0, 6*sizeof(uint32));

    bcmi_ft_alu_load_mem_index_get
        (unit, global_index, alu_load_type, &new_index, &mem, &a_idx);


    /* The profile is set. Write into the index of group. */
    rv  = soc_mem_read(unit, mem, MEM_BLOCK_ALL, new_index, &alu16_entry);

    if (BCM_FAILURE(rv)) {
        return rv;
    }


    soc_mem_field_get(unit, mem, (uint32 *)&alu16_entry, DATAf,
        alu16_ctrlfmt);


    soc_format_field_get(unit, BSC_TL_DG_TO_DT_ALU16_CONTROL_BUSfmt,
        alu16_ctrlfmt, ALU16_SPECIFIC_CONTROL_BUSf, alu16_specific_ctrlfmt);


    val0 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                alu16_specific_ctrlfmt, CHECK_0_THRESHOLDf);
    val1 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                alu16_specific_ctrlfmt, CHECK_1_THRESHOLDf);


    export_threshold = soc_format_field32_get(unit,
                    BSC_TL_DG_TO_DT_ALU16_SPECIFIC_CONTROL_BUSfmt,
                    alu16_specific_ctrlfmt, EXPORT_THRESHOLDf);


    soc_format_field_get(unit, BSC_TL_DG_TO_DT_ALU16_CONTROL_BUSfmt,
        alu16_ctrlfmt, ALU_BASE_CONTROL_BUSf, alu_fmt);

    check0_attr = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_0_ATTR_SELECTf);

    check1_attr = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_1_ATTR_SELECTf);


    opr0 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_0_OPERATIONf);

    opr1 = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_1_OPERATIONf);

    update_attr = soc_format_field32_get(unit,
                  BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                  alu_fmt, UPDATE_ATTR_SELECTf);

    update_opr = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, UPDATE_OPERATIONf);

    load_trigger = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, LOAD_TRIGGERf);

    export_opr =  soc_format_field32_get(unit,
                    BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                    alu_fmt, EXPORT_OPERATIONf);

    LOG_CLI((BSL_META_U(unit, "\n******* ALU16 Global index = %d ******\n "), global_index));


    LOG_CLI((BSL_META_U(unit, "val0 = %d\n "), val0));
    opr0 = bcmi_alu_opr_conversion[opr0];
    LOG_CLI((BSL_META_U(unit, "opr0 = %s\n "), olist[opr0]));
    LOG_CLI((BSL_META_U(unit, "check0_attr = %d\n "), check0_attr));

    LOG_CLI((BSL_META_U(unit, "val1 = %d\n "), val1));
    opr1 = bcmi_alu_opr_conversion[opr1];
    LOG_CLI((BSL_META_U(unit, "opr1 = %s\n "), olist[opr1]));
    LOG_CLI((BSL_META_U(unit, "check1_attr = %d\n "), check1_attr));

    LOG_CLI((BSL_META_U(unit, "Update_attr = %d\n "), update_attr));
    update_opr = bcmi_alu_action_conversion[update_opr];
    if (update_opr == -1) {
        LOG_CLI((BSL_META_U(unit, "update_opr = bitwise_or\n ")));
    } else {
        LOG_CLI((BSL_META_U(unit, "update_opr = %s\n "), alist[update_opr]));
    }
    LOG_CLI((BSL_META_U(unit, "load_trigger = %d\n "), load_trigger));

    export_opr = bcmi_alu_opr_conversion[export_opr];
    LOG_CLI((BSL_META_U(unit, "export_opr = %s\n "), olist[export_opr]));
    LOG_CLI((BSL_META_U(unit, "export_threshold = %d\n "), export_threshold));

    LOG_CLI((BSL_META_U(unit, "**********************************************\n ")));

    return BCM_E_NONE;
}


#else /* BCM_FLOWTRCAKER_SUPPORT*/
int bcmi_ft_group_not_empty;
#endif /* BCM_FLOWTRCAKER_SUPPORT */

