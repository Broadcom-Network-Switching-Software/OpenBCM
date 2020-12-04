/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_diag.c
 * Purpose:     The purpose of this file is to set flow tracker group methods.
 * Requires:
 */

#include <bcm_int/esw/flowtracker/ft_group.h>
#include <bcm_int/esw/flowtracker/ft_export.h>
#include <bcm_int/esw/flowtracker/ft_user.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

/* Export software state. */
extern bcmi_ft_export_state_t *bcmi_ft_export_state[BCM_MAX_NUM_UNITS];

#define TBL_TYPE_SINGLE                 0
#define TBL_TYPE_DOUBLE                 1
#define TBL_TYPE_AGG0                   2
#define TBL_TYPE_AGG1                   3
#define TBL_TYPE_AGG2                   4
#define TBL_TYPE_AGG3                   5

#define F_SHOW_BRIEF             (1 << 0)
#define F_GRP_NO_SHOW            (1 << 1)

typedef struct bcmi_ft_group_hw_tbl_trvrse_s {
    uint32 flags;
    bcm_flowtracker_group_t id;
    int in_tbl_type;

    /* Dynamic */
    bcm_flowtracker_group_t curr_gid;
    bcm_flowtracker_group_type_t group_type;
    int base_index;
    uint8 *u8buffer1;
    uint8 *u8buffer2;
    uint8 *u8buffer3;
    uint8 *u8buffer4;
    uint32 *u32buffer1;
    uint32 *u32buffer2;
    uint32 *u32buffer3;
    uint32 *u32buffer4;
} bcmi_ft_group_hw_tbl_trvrse_t;

typedef int (*bcmi_ft_group_hw_entry_decode_cb_f)(int unit,
                soc_mem_t mem, uint32 index, void *entry,
                void *user_data, int *jump);

int bcmi_alu_opr_conversion[9] =
        {bcmFlowtrackerCheckOpGreater,
         bcmFlowtrackerCheckOpGreaterEqual,
         bcmFlowtrackerCheckOpEqual,
         bcmFlowtrackerCheckOpNotEqual,
         bcmFlowtrackerCheckOpSmaller,
         bcmFlowtrackerCheckOpSmallerEqual,
         bcmFlowtrackerCheckOpPass,
         bcmFlowtrackerCheckOpFail,
         bcmFlowtrackerCheckOpMask};

int bcmi_alu_action_conversion[9] =
        {bcmFlowtrackerCheckActionNone,
         bcmFlowtrackerCheckActionCounterIncr,
         bcmFlowtrackerCheckActionCounterIncrByPktBytes,
         bcmFlowtrackerCheckActionCounterIncrByValue,
         bcmFlowtrackerCheckActionUpdateLowerValue,
         bcmFlowtrackerCheckActionUpdateHigherValue,
         bcmFlowtrackerCheckActionUpdateValue,
         bcmFlowtrackerCheckActionUpdateAverageValue,
         -1};

char elementType[3][20] =
   {"Flowchecker",
    "Tracking Param",
    "TimeStamp"};

/*
 * Function:
 *   bcmi_ft_table_lookup
 * Purpose:
 *   Looks up ft entry at 5 tuple.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id   - (IN) FT group id.
 *   v4-5tuple.- (IN) 5 tuple values, DIP,sip,srcport,dstport, protocol
 *   index - (OUT) memory index.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_table_lookup(int unit,
                     bcm_flowtracker_group_t id,
                     bcm_ip_t dip,
                     bcm_ip_t sip,
                     uint16 l4destport,
                     uint16 l4srcport,
                     uint8 proto,
                     int *index)
{
    uint8 *p;
    int rv = 0;
    int i = 0;
    uint8 mode = 0;
    soc_mem_t mem;
    int p_val = 0;
    uint8 data[40];
    int b1=0, b2=0, k1=0, k2=0;
    uint32 ft_key[50], ft_ret[50];

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
 *   bcmi_ft_table_insert
 * Purpose:
 *   Insert entry into ft table.
 * Parameters:
 *   unit - (IN) BCM device id
 *   v4-5tuple.- (IN) 5 tuple values, DIP,sip,srcport,dstport, protocol
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_table_insert(int unit,
                     bcm_ip_t dip,
                     bcm_ip_t sip,
                     uint16 l4destport,
                     uint16 l4srcport,
                     uint8 proto)
{
    uint8 *p;
    int i = 0;
    int rv = 0;
    uint8 data[40];
    ft_key_single_entry_t ft_key;

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
        } else if (type == bcmiFtAluLoadTypeAggAlu32) {
            LOG_CLI((BSL_META_U(unit, "bcmiFtAluLoadTypeAggAlu32")));
        } else if (type == bcmiFtAluLoadTypeAlu16) {
            LOG_CLI((BSL_META_U(unit, "bcmiFtAluLoadTypeAlu16")));
        } else {
            LOG_CLI((BSL_META_U(unit, "bcmiFtAluLoadTypeNone")));
        }
}

/*
 * Function:
 *   bcmi_ft_group_export_trigger_info_dump
 * Purpose:
 *   Dump export triggers associated with FT Group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Group index to dump.
 * Returns:
 *   None
 */
void
bcmi_ft_group_export_trigger_info_dump(int unit,
        bcm_flowtracker_group_t id)
{
    bcm_flowtracker_export_trigger_info_t export_trigger_info;

    BCM_FLOWTRACKER_TRIGGER_CLEAR_ALL(export_trigger_info);

    (void) bcmi_ft_group_export_trigger_get(unit, id, &export_trigger_info);

    LOG_CLI((BSL_META_U(unit, "\t")));

    if (BCM_FLOWTRACKER_TRIGGER_GET(export_trigger_info,
            bcmFlowtrackerExportTriggerNewLearn)) {
        LOG_CLI((BSL_META_U(unit, "%10s,"),"NewLearn"));
    }
    if (BCM_FLOWTRACKER_TRIGGER_GET(export_trigger_info,
            bcmFlowtrackerExportTriggerTimer)) {
        LOG_CLI((BSL_META_U(unit, "%10s,"),"Timer"));
    }
    if (BCM_FLOWTRACKER_TRIGGER_GET(export_trigger_info,
            bcmFlowtrackerExportTriggerAgeOut)) {
        LOG_CLI((BSL_META_U(unit, "%10s,"),"AgeOut"));
    }
    if (BCM_FLOWTRACKER_TRIGGER_GET(export_trigger_info,
            bcmFlowtrackerExportTriggerDrop)) {
        LOG_CLI((BSL_META_U(unit, "%10s,"),"Drop"));
    }

    LOG_CLI((BSL_META_U(unit, "\r\n")));
}

/*
 * Function:
 *   bcmi_ft_group_ts_events_info_dump
 * Purpose:
 *   Dump Timestamp events associated with FT Group.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Group index to dump.
 * Returns:
 *   None
 */
int
bcmi_ft_group_ts_events_info_dump(int unit,
        bcm_flowtracker_group_t id)
{
    int i = 1, ts_type = 0, evt_idx = 0;
    const char *ft_type_list[] = BCMI_FT_TYPE_STRINGS;
    char *ts_events_str[] = {
        "NewLearn", "FlowStart", "FlowEnd",
        "IPAT(Internal)", "IPDT(Internal)", "CheckEvent1",
        "CheckEvent2", "CheckEvent3", "CheckEvent4",
        "IngressTs", "EgressTs"
    };

    /* Timestamp trigger info */
    if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) != 0) {
        LOG_CLI((BSL_META_U(unit, "ts_triggers = 0x%08x\n"),
                    BCMI_FT_GROUP_TS_TRIGGERS(unit, id)));

        for (i = 0, ts_type = 0; ts_type < BCMI_FT_GROUP_MAX_TS_TYPE; ts_type++) {
            if (BCMI_FT_GROUP_TS_TRIGGERS(unit, id) & (1 << ts_type)) {

                /* Skip for below events as they are for other ts type */
                if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, ts_type) &
                        (BCMI_FT_TS_RESERVED_NEW_LEARN |
                         BCMI_FT_TS_RESERVED_FLOW_START |
                         BCMI_FT_TS_RESERVED_FLOW_END |
                         BCMI_FT_TS_RESERVED_INGRESS |
                         BCMI_FT_TS_RESERVED_EGRESS)) {
                    continue;
                }
                if (ts_type == BCMI_FT_GROUP_TS_NEW_LEARN) {
                    evt_idx = 0;
                } else if (ts_type == BCMI_FT_GROUP_TS_FLOW_START) {
                    evt_idx = 1;
                } else if (ts_type == BCMI_FT_GROUP_TS_FLOW_END) {
                    evt_idx = 2;
                } else if (ts_type == BCMI_FT_GROUP_TS_INGRESS) {
                    evt_idx = 9;
                } else if (ts_type == BCMI_FT_GROUP_TS_EGRESS) {
                    evt_idx = 10;
                } else if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, ts_type) &
                        BCMI_FT_TS_INTERNAL_IPAT) {
                    evt_idx = 3;
                } else if (BCMI_FT_GROUP_TS_TRIGGER_FLAGS(unit, id, ts_type) &
                        BCMI_FT_TS_INTERNAL_IPDT) {
                    evt_idx = 4;
                } else if (ts_type == BCMI_FT_GROUP_TS_CHECK_EVENT1) {
                    evt_idx = 5;
                } else if (ts_type == BCMI_FT_GROUP_TS_CHECK_EVENT2) {
                    evt_idx = 6;
                } else if (ts_type == BCMI_FT_GROUP_TS_CHECK_EVENT3) {
                    evt_idx = 7;
                } else { /* ts_type == BCMI_FT_GROUP_TS_CHECK_EVENT4 */
                    evt_idx = 8;
                }
                LOG_CLI((BSL_META_U(unit, "%d. "), i++));
                LOG_CLI((BSL_META_U(unit, "%15s | "), ts_events_str[evt_idx]));
                LOG_CLI((BSL_META_U(unit, "%10s |"),
                            ft_type_list[
                            BCMI_FT_GROUP_TS_TRIGGER_FT_TYPE(unit, id, ts_type)]));
                LOG_CLI((BSL_META_U(unit, " bank<%d> | "),
                            BCMI_FT_GROUP_TS_TRIGGER_BANK(unit, id, ts_type)));
                LOG_CLI((BSL_META_U(unit, " ref_count<%d> |"),
                            BCMI_FT_GROUP_TS_TRIGGER_REF_COUNT(unit, id, ts_type)));
                LOG_CLI((BSL_META_U(unit, "\n\r")));
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_group_state_dump
 * Purpose:
 *   Dump group state.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Group index to dump.
 *   brief - (IN) Brief show
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_state_dump(int unit,
                         bcm_flowtracker_group_t id,
                         int brief)
{
    soc_mem_t mem;
    int rv =  BCM_E_NONE;
    int num_alus_loads = 0, i=0;
    bcmi_ft_group_alu_info_t *temp = NULL;
    bcmi_ft_alu_load_type_t type = bcmiFtAluLoadTypeNone;
    bcmi_ft_group_pdd_bus_info_t *pdd_info = NULL;
    bcm_collector_t cmn_coll_id;
    bcm_flowtracker_collector_t hw_coll_id;
    int total_mems = 0;
    int mem_idx = 0;
    bcm_flowtracker_group_type_t group_type;
    bcm_flowtracker_tracking_param_info_t *t_param;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    const char *ft_type_list[] = BCMI_FT_TYPE_STRINGS;
    char *group_type_str[] = {
        "None",
        "AggregateIngress",
        "AggregateMmu",
        "AggregateEgress"};
    char *direction_str[] = {"None", "Fwd", "Rev", "Bidir"};

    if (bcmi_ft_group_is_invalid(unit, id)) {
        LOG_ERROR(BSL_LS_BCM_FLOWTRACKER, (BSL_META_U(unit,
                        "Group index (%d) is yet not created.\n "), id));

        return BCM_E_NONE;
    }

    LOG_CLI((BSL_META_U(unit, " ************************************** \n\r")));

    /* Print Group Management Status */
    if (BCMI_FT_GROUP(unit, id)->flags &
            BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
        LOG_CLI((BSL_META_U(unit, "--Software Managed Group--\n\r")));
    } else {
        LOG_CLI((BSL_META_U(unit, "--Hardware Managed Group--\n\r")));
    }

    /* Flowtracker Group Type */
    group_type = BCMI_FT_GROUP_TYPE_GET(id);
    LOG_CLI((BSL_META_U(unit, "Group Type                    :\t%s\n\r"),
                group_type_str[group_type]));

    /* Flowtracker Group Class Id */
    LOG_CLI((BSL_META_U(unit, "Class Identifier              :\t%5d\n\r"),
                BCMI_FT_GROUP_CONFIG(unit, id).class_id));

    /* Flowtracker Group Controls */
    LOG_CLI((BSL_META_U(unit, "Flowtracker Status            :\t%5d\n\r"),
                BCMI_FT_GROUP_FTFP_DATA(unit, id).flowtrack));
    LOG_CLI((BSL_META_U(unit, "New Learn Status              :\t%5d\n\r"),
                BCMI_FT_GROUP_FTFP_DATA(unit, id).new_learn));
    LOG_CLI((BSL_META_U(unit, "Direction                     :\t%5s\n\r"),
                direction_str[BCMI_FT_GROUP_FTFP_DATA(unit, id).direction]));

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

    /* Print associated Export triggers */
    LOG_CLI((BSL_META_U(unit, "Export Trigger(s)             :")));
    bcmi_ft_group_export_trigger_info_dump(unit, id);

    LOG_CLI((BSL_META_U(unit, "Validated                     :\t%5s\n\r"),
                (BCMI_FT_GROUP_IS_VALIDATED(unit, id) == 0) ? "No" : "Yes"));

    if (!BCMI_FT_GROUP_IS_VALIDATED(unit, id)) {
        return BCM_E_NONE;
    }

    /* Association with FTFP Entries */
    LOG_CLI((BSL_META_U(unit, "Associated with Field Entries :\t%5d\n\r"),
                BCMI_FT_GROUP(unit, id)->num_ftfp_entries));

    if (brief == TRUE) {
        return BCM_E_NONE;
    }

    /* Metering info */
    LOG_CLI((BSL_META_U(unit, "Meter Rate (Kbps/pps)         :\t%5d\n\r"),
                BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_sec));
    LOG_CLI((BSL_META_U(unit, "Meter burst (Kbits/Kpkts)     :\t%5d\n\r"),
                BCMI_FT_GROUP_METER_INFO(unit, id).sw_ckbits_burst));

    /* Tracking Param info */
    LOG_CLI((BSL_META_U(unit, "\n===== Dumping Tracking Parameters =====\n")));
    t_param = BCMI_FT_GROUP_TRACK_PARAM(unit, id);
    for (i = 0; i < (BCMI_FT_GROUP_TRACK_PARAM_NUM(unit, id)); i++) {

        LOG_CLI((BSL_META_U(unit, "%02d. "), i));
        LOG_CLI((BSL_META_U(unit, "Flags: %2d | "), t_param->flags));
        LOG_CLI((BSL_META_U(unit, "Param:")));
        LOG_CLI((BSL_META_U(unit, " %20s"), plist[t_param->param]));
        if (t_param->param == bcmFlowtrackerTrackingParamTypeFlowtrackerCheck) {
            LOG_CLI((BSL_META_U(unit, " |  Id: 0x%x"), t_param->check_id));
        } else if (t_param->param == bcmFlowtrackerTrackingParamTypeCustom) {
            LOG_CLI((BSL_META_U(unit, " (%d) |  Dir: %9s"), t_param->custom_id,
                        direction_str[t_param->direction]));
        } else {
            LOG_CLI((BSL_META_U(unit, " | Dir: %9s"),
                        direction_str[t_param->direction]));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));
        t_param++;
    }

    LOG_CLI((BSL_META_U(unit, "\n ===== Dumping timestamp trigger info ====\n")));
    bcmi_ft_group_ts_events_info_dump(unit, id);

    /* Extractor Data info */
    LOG_CLI((BSL_META_U(unit, "\n ===== Dumping extractor Data ====\n")));

    /* assign the ALU memory chunk to local pointer. */
    temp = BCMI_FT_GROUP_EXT_DATA_INFO(unit, id);

    /* Get total number of alus in this chunk. */
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, id)).num_data_info;

    LOG_CLI((BSL_META_U(unit, "Total elements  = %d\n"), num_alus_loads));
    /* allocate the bitmap based on the type, length */
    for (i = 0; i < num_alus_loads; i++, temp++) {

        /* Now get the type of the memory and take action. */
        LOG_CLI((BSL_META_U
                    (unit, " ************ Session Data - Element %d    ************"),
                    i+1));

        LOG_CLI((BSL_META_U(unit, "\n")));

        type = temp->alu_load_type;
        bcmi_ft_print_alu_load_type_str(unit, type);

        LOG_CLI((BSL_META_U(unit, "\nStage = %s  -- "), ft_type_list[temp->alu_ft_type]));

        if (temp->flowchecker_id) {
            sal_memset(&fc_info, 0, sizeof(bcmi_flowtracker_flowchecker_info_t));
            LOG_CLI((BSL_META_U(unit, "FlowCheck Id = 0x%x"), temp->flowchecker_id));
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
                int fc_idx = 0;
                if (BCMI_FT_IDX_IS_INT_CHECK(temp->flowchecker_id)) {
                    LOG_CLI((BSL_META_U(unit, " (Internal - ")));
                    fc_idx = BCMI_FT_INT_CHECK_IDX_GET(temp->flowchecker_id);
                    if (fc_idx == BCMI_FT_INT_CHECK_FLOWSTART_IDX) {
                        LOG_CLI((BSL_META_U(unit, " FlowStart)")));
                    } else if (fc_idx == BCMI_FT_INT_CHECK_FLOWEND_IDX) {
                        LOG_CLI((BSL_META_U(unit, " FlowEnd)")));
                    } else if (fc_idx == BCMI_FT_INT_CHECK_CHECKEVENT1_IDX) {
                        LOG_CLI((BSL_META_U(unit, " CheckEvent1)")));
                    } else if (fc_idx == BCMI_FT_INT_CHECK_CHECKEVENT2_IDX) {
                        LOG_CLI((BSL_META_U(unit, " CheckEvent2)")));
                    } else if (fc_idx == BCMI_FT_INT_CHECK_CHECKEVENT3_IDX) {
                        LOG_CLI((BSL_META_U(unit, " CheckEvent3)")));
                    } else if (fc_idx == BCMI_FT_INT_CHECK_CHECKEVENT4_IDX) {
                        LOG_CLI((BSL_META_U(unit, " CheckEvent4)")));
                    }
                }
            }
#endif
            rv = bcmi_ft_flowchecker_get(unit, temp->flowchecker_id, &fc_info);
            if (BCM_FAILURE(rv)) {
                LOG_CLI((BSL_META_U(unit, "\n\rCheck Unavailable in s/w.\n\r")));
                continue;
            }
        } else {
            LOG_CLI((BSL_META_U(unit, "Tracking Parameter")));
        }

        LOG_CLI((BSL_META_U(unit, "\n\rParam1 - %20s"),
                    plist[temp->element_type1]));
        if (temp->element_type1 == bcmFlowtrackerTrackingParamTypeCustom) {
            LOG_CLI((BSL_META_U(unit, " (%d) "), temp->custom_id1));
        }
        LOG_CLI((BSL_META_U(unit, " - Ext_Pos = %d Width = %d isAlu = %d"),
                    temp->key1.location, temp->key1.length, temp->key1.is_alu));

        LOG_CLI((BSL_META_U(unit, "\n\rParam2 - %20s"),
                        plist[temp->element_type2]));
        if (temp->element_type2 == bcmFlowtrackerTrackingParamTypeCustom) {
            LOG_CLI((BSL_META_U(unit, " (%d) "), temp->custom_id2));
        }
        LOG_CLI((BSL_META_U(unit, " - Ext_Pos = %d Width = %d isAlu = %d"),
                    temp->key2.location, temp->key2.length, temp->key2.is_alu));

        LOG_CLI((BSL_META_U(unit, "\n\rAction - %20s"),
                    plist[temp->action_element_type]));
        if (temp->action_element_type == bcmFlowtrackerTrackingParamTypeCustom) {
            LOG_CLI((BSL_META_U(unit, " (%d) "), temp->action_custom_id));
        }
        LOG_CLI((BSL_META_U(unit, " - Ext_Pos = %d Width = %d isAlu = %d"),
                    temp->action_key.location, temp->action_key.length,
                    temp->action_key.is_alu));

        LOG_CLI((BSL_META_U(unit, "\n")));

        if (type == bcmiFtAluLoadTypeAlu16) {
            bcmi_ft_alu16_dump(unit, temp->alu_load_index);
        } else if (type == bcmiFtAluLoadTypeAlu32) {
            bcmi_ft_alu32_dump(unit, bcmiFtAluLoadTypeAlu32,
                    temp->alu_load_index);
        } else if(type == bcmiFtAluLoadTypeAggAlu32) {
            bcmi_ft_alu32_dump(unit, bcmiFtAluLoadTypeAggAlu32,
                    temp->alu_load_index);
        } else {
            LOG_CLI((BSL_META_U(unit, "\n This element belongs to Load memory\n")));
        }

        LOG_CLI((BSL_META_U(unit, "\n")));
    }

    /* retreive relevant pdd mem for device */
    bcmi_ft_alu_load_pdd_mem_get(unit, 0, &mem);

    LOG_CLI((BSL_META_U(unit, "Dump PDD profile entry.\n")));
    /* For all the types of memory types. */
    for (type = 0; type < BCMI_FT_ALU_LOAD_MEMS; type++) {

        pdd_info = BCMI_FT_ALU_LOAD_PDD_INFO(unit, type);
        if (!pdd_info) {
            continue;
        }

        total_mems = BCMI_FT_ALU_LOAD_MEM_COUNT(unit, type);

        /* Set sub memories for Load memories. */
        if (type == bcmiFtAluLoadTypeLoad8) {
            total_mems = TOTAL_GROUP_LOAD8_DATA_NUM;
        } else if (type == bcmiFtAluLoadTypeLoad16) {
            total_mems = TOTAL_GROUP_LOAD16_DATA_NUM;
        }

        /* For all the alu/load types, check what memories are set in pdd. */
        for (mem_idx = 0; mem_idx < total_mems; mem_idx++) {
            /* Now get if this particular field is set in pdd. */
            if (soc_mem_field32_get(unit, mem,
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
 *   bcmi_ft_group_session_data_dump
 * Purpose:
 *   Dump the session data.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Group index to dump.
 *   index - (IN) FT index to dump.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_group_session_data_dump(int unit,
                                bcm_flowtracker_group_t id,
                                int index)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem;
    int etype = 0;
    uint8 mode = 0;
    uint32 value = 0;
    int data_bytes = 0;
    uint8 *data = NULL;
    uint32 *buffer = NULL;
    int sub_len = 0, first_value = 0;
    int length = 0, start = 32, i = 0;
    bcmi_ft_group_template_list_t *temp = NULL;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    bcm_flowtracker_tracking_param_type_t element;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;

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

    start = start - 1;

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
            (temp->info.param_type == bcmiFtGroupParamTypeTsCheckEvent2) ||
            (temp->info.param_type == bcmiFtGroupParamTypeTsCheckEvent3) ||
            (temp->info.param_type == bcmiFtGroupParamTypeTsCheckEvent4)) {

            etype = 2;
            element = bcmi_ft_group_get_tracking_param(unit,
                                                       temp->info.param_type);
            sub_len = 2;

        } else if (temp->info.check_id) {
            rv = bcmi_ft_flowchecker_get(unit,
                                         temp->info.check_id, &fc_info);
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
 *   bcmi_ft_alu_mask_shift_dump
 * Purpose:
 *   Dump the alu mask profile.
 * Parameters:
 *   unit         - (IN) BCM device id
 *   mask_type    - (IN) Mask Type
 *   profile_idx  - (IN) ALU Mask profile index
 *   enable       - (IN) Is Mask profile enabled
 *   index        - (IN) index in ALU Mask profile
 * Returns:
 *   BCM_E_XXX
 */
STATIC int
bcmi_ft_alu_mask_shift_dump(int unit,
                            int mask_type,
                            int profile_idx,
                            int enable,
                            int index)
{
    int rv = BCM_E_NONE;
    soc_mem_t mem = INVALIDm;
    uint32 mask = 0, shift = 0;
    bsc_dt_alu_mask_profile_entry_t entry;

    if (profile_idx == 0) {
        mem = BSC_DT_ALU_MASK_PROFILEm;
    } else {
        mem = BSC_DT_AGG_ALU_MASK_PROFILEm;
    }

    rv  = soc_mem_read(unit, mem, MEM_BLOCK_ALL, index, &entry);
    BCM_IF_ERROR_RETURN(rv);

    if (enable) {
        mask = soc_mem_field32_get(unit, mem, &entry, ATTR_MASKf);
        shift = soc_mem_field32_get(unit, mem, &entry, ATTR_RIGHT_SHIFTf);
    }

    LOG_CLI((BSL_META_U(unit, "(Mask = 0x%08x Shift=%d)\n "), mask, shift));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu32_dump
 * Purpose:
 *   Dump the alu32 data.
 * Parameters:
 *   unit         - (IN) BCM device id
 *   id           - (IN) Group id to dump.
 *   alu_load_type- (IN) ALU Type
 *   global_index - (IN) index to dump.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu32_dump(int unit,
                   bcmi_ft_alu_load_type_t alu_load_type,
                   int global_index)
{
    int rv = BCM_E_NONE;
    int a_idx = -1, new_index = -1;
    soc_mem_t mem = -1;
    uint32 alu_fmt[5];
    uint32 alu32_specific_ctrlfmt[5];
    uint32 alu32_ctrlfmt[10];
    bsc_dg_group_alu32_profile_0_entry_t alu32_entry;
    uint32 val0, val1, opr0, opr1, check0_attr, check1_attr, update_opr;
    uint32 gran0, gran1, update_opr_param, export_mode, clear_on_export;
    uint32 export_opr, export_threshold;
    uint32 load_trigger = 0, update_attr = 0;
    const char *olist[] = BCM_FT_CHECK_OPERATION_STRINGS;
    const char *alist[] = BCM_FT_CHECK_ACTION_STRINGS;
    uint8 check_bit_select[4] = {0};
    uint16 check_logic_kmap_control = 0;
    uint8 timestamp_trigger[4] = { 0 };
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    int profile_idx = 0;
    uint8 check_slice_mode = 0, update_slice_mode = 0;
    uint8 update_bitmap_slice_mode = 0;
    uint16 flex_check_0_kmap_control = 0;
    uint16 flex_check_1_kmap_control = 0;
    uint16 flex_export_kmap_control = 0;
    uint8 mask_en[3] = {0};
    uint8 mask_profile_idx[3] = {0};
    uint8 src_ts = 0, dst_ts = 0, delay_mode = 0;
    uint8 delay_gran = 0;
    int delay_offset = 0;
    uint8 ts_select[3] = {0};
    uint32 clear_on_periodic_export = 0;
    const char *delayoperandlist[] = {
        "PKT_TS0",
        "PKT_TS1",
        "SESSION_BANK_TS0",
        "SESSION_BANK_TS1",
        "SESSION_BANK_TS2",
        "SESSION_BANK_TS3",
        "CMIC",
        "NTP",
        "PTP",
        "LEGACY",
        "LTS"
    };
    const char *delaymodelist[] = {
        "PTP 64Bit Mode",
        "NTP 64Bit Mode",
        "Generic 48Bit Mode",
        "LTS Mode"
    };
#endif
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    uint32 check0_use_bytecount = 0;
    uint32 check1_use_bytecount = 0;
    uint32 update_use_bytecount = 0;
#endif

    sal_memset(alu_fmt, 0, 5 * sizeof(uint32));
    sal_memset(alu32_specific_ctrlfmt, 0, 5 * sizeof(uint32));
    sal_memset(alu32_ctrlfmt, 0, 10 * sizeof(uint32));

    if (!soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
        (alu_load_type == bcmiFtAluLoadTypeAggAlu32)) {
        LOG_CLI((BSL_META_U(unit, "\n - Aggregate ALU32 Not supported -\n ")));
        return BCM_E_NONE;
    }

    bcmi_ft_alu_load_mem_index_get
        (unit, global_index, alu_load_type, &new_index, &mem, &a_idx);

    /* The profile is set. Write into the index of group. */
    rv  = soc_mem_read(unit, mem, MEM_BLOCK_ALL, new_index, &alu32_entry);
    BCM_IF_ERROR_RETURN(rv);

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

    /* check 0/1 logic kmap */
    check_bit_select[0] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_0f);
    check_bit_select[1] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_1f);
    check_bit_select[2] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_2f);
    check_bit_select[3] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_3f);
    check_logic_kmap_control = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, FLEX_CHECK_LOGIC_KMAP_CONTROLf);

    /* timestamp triggers */
    timestamp_trigger[0] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, TIMESTAMP_0_TRIGf);
    timestamp_trigger[1] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, TIMESTAMP_1_TRIGf);

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        timestamp_trigger[2] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, TIMESTAMP_2_TRIGf);
        timestamp_trigger[3] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, TIMESTAMP_3_TRIGf);

        clear_on_periodic_export = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CLEAR_ON_PERIODIC_EXPORT_ENABLEf);

        /* check slice */
        check_slice_mode = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_SLICE_MODEf);
        update_slice_mode = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, UPDATE_SLICE_MODEf);
        update_bitmap_slice_mode = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, UPDATE_BITMAP_SLICE_MODEf);
        flex_check_0_kmap_control = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, FLEX_CHECK_0_KMAP_CONTROLf);
        flex_check_1_kmap_control = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, FLEX_CHECK_1_KMAP_CONTROLf);
        flex_export_kmap_control = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, FLEX_EXPORT_KMAP_CONTROLf);

        /* check mask */
        mask_en[0] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_EN_CHECK_0f);
        mask_en[1] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_EN_CHECK_1f);
        mask_en[2] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_EN_CHECK_UPDATEf);
        mask_profile_idx[0] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_PROFILE_INDEX_CHECK_0f);
        mask_profile_idx[1] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_PROFILE_INDEX_CHECK_1f);
        mask_profile_idx[2] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_PROFILE_INDEX_UPDATEf);

        /* delay params */
        ts_select[0] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, CHECK_ATTR_0_TS_SELECTf);
        ts_select[1] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, CHECK_ATTR_1_TS_SELECTf);
        ts_select[2] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, UPDATE_ATTR_1_TS_SELECTf);
        src_ts = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, FIRST_DELAY_OPERANDf);
        dst_ts = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, SECOND_DELAY_OPERANDf);
        delay_mode = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, DELAY_MODEf);
        delay_gran = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, DELAY_GRANULARITYf);
        delay_offset = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU32_SPECIFIC_CONTROL_BUSfmt,
                alu32_specific_ctrlfmt, DELAY_OFFSETf);
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        check0_use_bytecount = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, BYTE_COUNT_CHECK_0_SELECTf);
        check1_use_bytecount = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, BYTE_COUNT_CHECK_1_SELECTf);
        update_use_bytecount = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, BYTE_COUNT_UPDATE_ALU_SELECTf);
    }
#endif

    LOG_CLI((BSL_META_U(unit, "\n******* ALu32 Global index = %d ******\n "), global_index));

    LOG_CLI((BSL_META_U(unit, "val0 = 0x%x\n "), val0));
    opr0 = bcmi_alu_opr_conversion[opr0];
    LOG_CLI((BSL_META_U(unit, "opr0 = %s\n "), olist[opr0]));
    LOG_CLI((BSL_META_U(unit, "check0_attr = %d\n "), check0_attr));
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        LOG_CLI((BSL_META_U(unit, "check0_use_bytecount = %d\n "), check0_use_bytecount));
    }
#endif
    LOG_CLI((BSL_META_U(unit, "check0_gran = %d\n "), gran0));

    LOG_CLI((BSL_META_U(unit, "val1 = 0x%x\n "), val1));
    opr1 = bcmi_alu_opr_conversion[opr1];
    LOG_CLI((BSL_META_U(unit, "opr1 = %s\n "), olist[opr1]));
    LOG_CLI((BSL_META_U(unit, "check1_attr = %d\n "), check1_attr));
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        LOG_CLI((BSL_META_U(unit, "check1_use_bytecount = %d\n "), check1_use_bytecount));
    }
#endif
    LOG_CLI((BSL_META_U(unit, "check1_gran = %d\n "), gran1));

    LOG_CLI((BSL_META_U(unit, "BitSelect [0] = %d, [1] = %d,[2] = %d,[3] = %d,"
                              " kmap_control = 0x%04x\n "),
                check_bit_select[0],check_bit_select[1],check_bit_select[2],
                check_bit_select[3],check_logic_kmap_control));

    LOG_CLI((BSL_META_U(unit, "Update_attr = %d\n "), update_attr));
#ifdef BCM_FLOWTRACKER_V3_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
        LOG_CLI((BSL_META_U(unit, "Update_use_bytecount = %d\n "), update_use_bytecount));
    }
#endif
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
    LOG_CLI((BSL_META_U(unit, "export_mode = %s\n "),
                export_mode ? "Prev Session Data": "Threshold"));
    if (export_mode == 1) {
        LOG_CLI((BSL_META_U(unit, "clear_on_Export = %d (dont care)\n "),
                    clear_on_export));
    } else {
        LOG_CLI((BSL_META_U(unit, "clear_on_Export = %d\n "), clear_on_export));
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        LOG_CLI((BSL_META_U(unit, "clear_on_Periodic_Export = %d\n "),
                    clear_on_periodic_export));
        LOG_CLI((BSL_META_U(unit, "Timestamp Trig0 = %d, Trig1 = %d, "
                        "Trig2 = %d, Trig3 = %d\n "),
                    timestamp_trigger[0],timestamp_trigger[1],
                    timestamp_trigger[2], timestamp_trigger[3]));
    } else
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
    {
        LOG_CLI((BSL_META_U(unit, "Timestamp Trig0 = %d, Trig1 = %d\n "),
                    timestamp_trigger[0],timestamp_trigger[1]));
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        if (alu_load_type == bcmiFtAluLoadTypeAggAlu32) {
            profile_idx = 1;
        }

        LOG_CLI((BSL_META_U(unit, "slice - check_slice_mode = %d\n ") ,
                    check_slice_mode));
        LOG_CLI((BSL_META_U(unit, "slice - flex_check_0_kmap_control "
                        "= 0x%04x\n "),
                    flex_check_0_kmap_control));
        LOG_CLI((BSL_META_U(unit, "slice - flex_check_1_kmap_control "
                        "= 0x%04x\n "),
                    flex_check_1_kmap_control));
        LOG_CLI((BSL_META_U(unit, "slice - update_slice_mode = %d, "
                        "update_bitmap_slice_mode = 0x%02x\n "),
                    update_slice_mode, update_bitmap_slice_mode));
        LOG_CLI((BSL_META_U(unit, "slice - flex_export_kmap_control "
                        "= 0x%04x\n "),
                    flex_export_kmap_control));

        LOG_CLI((BSL_META_U(unit, "attrib - mask check0 enable = %d, "
                        "mask index = %d - "),
                    mask_en[0],mask_profile_idx[0]));
        bcmi_ft_alu_mask_shift_dump(unit, 0,
                profile_idx, mask_en[0], mask_profile_idx[0]);
        LOG_CLI((BSL_META_U(unit, "attrib - mask check1 enable = %d, "
                        "mask index = %d - "),
                    mask_en[1],mask_profile_idx[1]));
        bcmi_ft_alu_mask_shift_dump(unit, 1,
                profile_idx, mask_en[1], mask_profile_idx[1]);
        LOG_CLI((BSL_META_U(unit, "attrib - mask update enable = %d, "
                        "mask index = %d - "),
                    mask_en[2],mask_profile_idx[2]));
        bcmi_ft_alu_mask_shift_dump(unit, 2,
                profile_idx, mask_en[2], mask_profile_idx[2]);

        LOG_CLI((BSL_META_U(unit, "delay - ts_select check0_attr[0] = %d, "
                        "check1_attr[1] = %d, update_attr[2] = %d\n "),
                    ts_select[0],ts_select[1],ts_select[2]));
        LOG_CLI((BSL_META_U(unit, "delay - src_ts = %d(%s), dst_ts = %d(%s)\n "),
                    src_ts, delayoperandlist[src_ts],
                    dst_ts, delayoperandlist[dst_ts]));
        LOG_CLI((BSL_META_U(unit, "delay - mode = %d(%s),"
                        " gran = %d, offset = %d\n"),
                    delay_mode, delaymodelist[delay_mode],
                    delay_gran, delay_offset));
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    LOG_CLI((BSL_META_U(unit, "************************************************\n ")));

    return BCM_E_NONE;
}

/*
 * Function:
 *   bcmi_ft_alu16_dump
 * Purpose:
 *   Dump the alu16 data.
 * Parameters:
 *   unit - (IN) BCM device id
 *   id - (IN) Group index to dump.
 *   index - (IN) index to dump.
 * Returns:
 *   BCM_E_XXX
 */
int
bcmi_ft_alu16_dump(int unit,
                   int global_index)
{
    int rv = BCM_E_NONE;
    int a_idx = -1, new_index = -1;
    soc_mem_t mem = -1;
    int load_trigger = 0;
    uint32 alu_fmt[5];
    uint32 alu16_specific_ctrlfmt[2];
    uint32 alu16_ctrlfmt[7];
    bsc_dg_group_alu16_profile_0_entry_t alu16_entry;
    uint32 val0, val1, opr0, opr1, check0_attr, check1_attr, update_opr;
    uint32 export_mode, clear_on_export;
    uint32 export_opr = 0, export_threshold = 0, update_attr = 0;
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeAlu16;
    const char *olist[] = BCM_FT_CHECK_OPERATION_STRINGS;
    const char *alist[] = BCM_FT_CHECK_ACTION_STRINGS;
    uint8 check_bit_select[4] = {0};
    uint16 check_logic_kmap_control = 0;
    uint8 timestamp_trigger[4] = { 0 };
    uint32 update_opr_param;
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    uint8 check_slice_mode = 0, update_slice_mode = 0;
    uint8 update_bitmap_slice_mode = 0;
    uint16 flex_check_0_kmap_control = 0;
    uint16 flex_check_1_kmap_control = 0;
    uint16 flex_export_kmap_control = 0;
    uint8 mask_en[3] = {0};
    uint8 mask_profile_idx[3] = {0};
    uint32 clear_on_periodic_export = 0;
#endif

    sal_memset(alu_fmt, 0, 5 * sizeof(uint32));
    sal_memset(alu16_specific_ctrlfmt, 0, 2 * sizeof(uint32));
    sal_memset(alu16_ctrlfmt, 0, 7 * sizeof(uint32));
    sal_memset(&alu16_entry, 0, sizeof(alu16_entry));

    bcmi_ft_alu_load_mem_index_get
        (unit, global_index, alu_load_type, &new_index, &mem, &a_idx);

    /* The profile is set. Write into the index of group. */
    rv  = soc_mem_read(unit, mem, MEM_BLOCK_ALL, new_index, &alu16_entry);
    BCM_IF_ERROR_RETURN(rv);

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

    /* check 0/1 logic kmap */
    check_bit_select[0] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_0f);
    check_bit_select[1] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_1f);
    check_bit_select[2] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_2f);
    check_bit_select[3] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, CHECK_LOGIC_BIT_SELECT_3f);
    check_logic_kmap_control = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, FLEX_CHECK_LOGIC_KMAP_CONTROLf);

    /* timestamp triggers */
    timestamp_trigger[0] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, TIMESTAMP_0_TRIGf);
    timestamp_trigger[1] = soc_format_field32_get(unit,
            BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
            alu_fmt, TIMESTAMP_1_TRIGf);

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {

        timestamp_trigger[2] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, TIMESTAMP_2_TRIGf);
        timestamp_trigger[3] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, TIMESTAMP_3_TRIGf);

        clear_on_periodic_export = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CLEAR_ON_PERIODIC_EXPORT_ENABLEf);

        /* check slice */
        check_slice_mode = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, CHECK_SLICE_MODEf);
        update_slice_mode = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, UPDATE_SLICE_MODEf);
        update_bitmap_slice_mode = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, UPDATE_BITMAP_SLICE_MODEf);
        flex_check_0_kmap_control = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, FLEX_CHECK_0_KMAP_CONTROLf);
        flex_check_1_kmap_control = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, FLEX_CHECK_1_KMAP_CONTROLf);
        flex_export_kmap_control = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, FLEX_EXPORT_KMAP_CONTROLf);

        /* check mask */
        mask_en[0] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_EN_CHECK_0f);
        mask_en[1] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_EN_CHECK_1f);
        mask_en[2] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_EN_CHECK_UPDATEf);
        mask_profile_idx[0] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_PROFILE_INDEX_CHECK_0f);
        mask_profile_idx[1] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_PROFILE_INDEX_CHECK_1f);
        mask_profile_idx[2] = soc_format_field32_get(unit,
                BSC_TL_DG_TO_DT_ALU_BASE_CONTROL_BUSfmt,
                alu_fmt, MASK_PROFILE_INDEX_UPDATEf);
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    LOG_CLI((BSL_META_U(unit, "\n******* ALU16 Global index = %d ******\n "), global_index));

    LOG_CLI((BSL_META_U(unit, "val0 = %d\n "), val0));
    opr0 = bcmi_alu_opr_conversion[opr0];
    LOG_CLI((BSL_META_U(unit, "opr0 = %s\n "), olist[opr0]));
    LOG_CLI((BSL_META_U(unit, "check0_attr = %d\n "), check0_attr));

    LOG_CLI((BSL_META_U(unit, "val1 = %d\n "), val1));
    opr1 = bcmi_alu_opr_conversion[opr1];
    LOG_CLI((BSL_META_U(unit, "opr1 = %s\n "), olist[opr1]));
    LOG_CLI((BSL_META_U(unit, "check1_attr = %d\n "), check1_attr));

    LOG_CLI((BSL_META_U(unit, "BitSelect [0] = %d, [1] = %d,[2] = %d,[3] = %d,"
                              " kmap_control = 0x%04x\n "),
                check_bit_select[0],check_bit_select[1],check_bit_select[2],
                check_bit_select[3],check_logic_kmap_control));

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
    LOG_CLI((BSL_META_U(unit, "export_mode = %s\n "),
                export_mode ? "Prev Session Data": "Threshold"));
    if (export_mode == 1) {
        LOG_CLI((BSL_META_U(unit, "clear_on_Export = %d (dont care)\n "),
                    clear_on_export));
    } else {
        LOG_CLI((BSL_META_U(unit, "clear_on_Export = %d\n "), clear_on_export));
    }
#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        LOG_CLI((BSL_META_U(unit, "clear_on_Periodic_Export = %d\n "),
                    clear_on_periodic_export));
        LOG_CLI((BSL_META_U(unit, "Timestamp Trig0 = %d, Trig1 = %d, "
                        "Trig2 = %d, Trig3 = %d\n "),
                    timestamp_trigger[0],timestamp_trigger[1],
                    timestamp_trigger[2], timestamp_trigger[3]));
    } else
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */
    {
        LOG_CLI((BSL_META_U(unit, "Timestamp Trig0 = %d, Trig1 = %d\n "),
                    timestamp_trigger[0],timestamp_trigger[1]));
    }

#ifdef BCM_FLOWTRACKER_V2_SUPPORT
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_2)) {
        LOG_CLI((BSL_META_U(unit, "slice - check_slice_mode = %d\n ") ,
                    check_slice_mode));
        LOG_CLI((BSL_META_U(unit, "slice - flex_check_0_kmap_control "
                        "= 0x%04x\n "),
                    flex_check_0_kmap_control));
        LOG_CLI((BSL_META_U(unit, "slice - flex_check_1_kmap_control "
                        "= 0x%04x\n "),
                    flex_check_1_kmap_control));
        LOG_CLI((BSL_META_U(unit, "slice - update_slice_mode = %d, "
                        "update_bitmap_slice_mode = 0x%02x\n "),
                    update_slice_mode, update_bitmap_slice_mode));
        LOG_CLI((BSL_META_U(unit, "slice - flex_export_kmap_control "
                        "= 0x%04x\n "),
                    flex_export_kmap_control));

        LOG_CLI((BSL_META_U(unit, "attrib - mask check0 enable = %d, "
                        "mask index = %d - "),
                    mask_en[0],mask_profile_idx[0]));
        bcmi_ft_alu_mask_shift_dump(unit, 0,
                0, mask_en[0], mask_profile_idx[0]);
        LOG_CLI((BSL_META_U(unit, "attrib - mask check1 enable = %d, "
                        "mask index = %d - "),
                    mask_en[1],mask_profile_idx[1]));
        bcmi_ft_alu_mask_shift_dump(unit, 1,
                0, mask_en[1], mask_profile_idx[1]);
        LOG_CLI((BSL_META_U(unit, "attrib - mask update enable = %d, "
                        "mask index = %d - "),
                    mask_en[2],mask_profile_idx[2]));
        bcmi_ft_alu_mask_shift_dump(unit, 2,
                0, mask_en[2], mask_profile_idx[2]);
    }
#endif /* BCM_FLOWTRACKER_V2_SUPPORT */

    LOG_CLI((BSL_META_U(unit, "**********************************************\n ")));

    return BCM_E_NONE;
}

void
bcmi_ft_group_hw_entry_buf_allocate(
        int unit,
        bcmi_ft_group_hw_tbl_trvrse_t *tbl_trv)
{
    int size;

    size = 128;
    BCMI_FT_ALLOC(tbl_trv->u32buffer1, sizeof (uint32) * size,
                "trv u32buffer1");
    BCMI_FT_ALLOC(tbl_trv->u32buffer2, sizeof (uint32) * size,
                "trv u32buffer2");
    BCMI_FT_ALLOC(tbl_trv->u32buffer3, sizeof (uint32) * size,
                "trv u32buffer3");
    BCMI_FT_ALLOC(tbl_trv->u32buffer4, sizeof (uint32) * size,
                "trv u32buffer4");

    size = 64;
    BCMI_FT_ALLOC(tbl_trv->u8buffer1, sizeof (uint8) * size,
                "trv u8buffer1");
    BCMI_FT_ALLOC(tbl_trv->u8buffer2, sizeof (uint8) * size,
                "trv u8buffer2");
    BCMI_FT_ALLOC(tbl_trv->u8buffer3, sizeof (uint8) * size,
                "trv u8buffer3");
    BCMI_FT_ALLOC(tbl_trv->u8buffer4, sizeof (uint8) * size,
                "trv u8buffer4");
}

void
bcmi_ft_group_hw_tbl_trvse_t_buf_free(
        int unit,
        bcmi_ft_group_hw_tbl_trvrse_t *tbl_trv)
{
    BCMI_FT_FREE(tbl_trv->u32buffer1);
    BCMI_FT_FREE(tbl_trv->u32buffer2);
    BCMI_FT_FREE(tbl_trv->u32buffer3);
    BCMI_FT_FREE(tbl_trv->u32buffer4);
    BCMI_FT_FREE(tbl_trv->u8buffer1);
    BCMI_FT_FREE(tbl_trv->u8buffer2);
    BCMI_FT_FREE(tbl_trv->u8buffer3);
    BCMI_FT_FREE(tbl_trv->u8buffer4);

    tbl_trv->u32buffer1 = NULL;
    tbl_trv->u32buffer2 = NULL;
    tbl_trv->u32buffer3 = NULL;
    tbl_trv->u32buffer4 = NULL;
    tbl_trv->u8buffer1  = NULL;
    tbl_trv->u8buffer2  = NULL;
    tbl_trv->u8buffer3  = NULL;
    tbl_trv->u8buffer4  = NULL;
}

void
bcmi_ft_group_hw_entry_group_show(
        int unit,
        bcmi_ft_group_hw_tbl_trvrse_t *tbl_trv)
{
    bcmi_ft_group_key_data_mode_t mode;
    bcm_flowtracker_group_type_t group_type;
    char *group_type_str[] = {
        "Normal",
        "AggregateIngress",
        "AggregateMmu",
        "AggregateEgress"};


    LOG_CLI((BSL_META_U(unit, "*****************************************\n")));

    LOG_CLI((BSL_META_U(unit, "Flowtracker Group             :\t"
                    "%d\n\r"), tbl_trv->curr_gid));

    /* Print Group Management Status */
    if (BCMI_FT_GROUP(unit, tbl_trv->curr_gid)->flags &
            BCMI_FT_GROUP_INFO_F_USER_ENTRIES_ONLY) {
        LOG_CLI((BSL_META_U(unit, "Learning Method               :\t"
                        "%s \n\r"), "Static"));
    } else {
        LOG_CLI((BSL_META_U(unit, "Learning Method               :\t"
                        "%s \n\r"), "Dynamic"));
    }

    /* Flowtracker Group Type */
    group_type = BCMI_FT_GROUP_TYPE_GET(tbl_trv->curr_gid);
    LOG_CLI((BSL_META_U(unit, "Group Type                    :\t%s\n\r"),
                group_type_str[group_type]));

    /* Group Mode */
    mode = BCMI_FT_GROUP_FTFP_DATA(unit, tbl_trv->curr_gid).session_key_mode;
    LOG_CLI((BSL_META_U(unit, "Group Mode                    :\t%s\n\r"),
                (mode == bcmiFtGroupModeSingle) ? "Single" : "Double"));

    LOG_CLI((BSL_META_U(unit, "*****************************************\n")));
}

int
bcmi_ft_group_hw_entry_key_decode(
        int unit,
        bcmi_ft_group_hw_tbl_trvrse_t *tbl_trv,
        int index)
{
    int rv = BCM_E_NONE;
    int i, ii;
    int base_index;
    bcm_flowtracker_group_t gid;
    bcmi_ft_group_key_data_mode_t mode;

    soc_mem_t key_mem = INVALIDm;
    int num_bits = 0, num_bytes = 0;
    uint8 *ptr = NULL;
    uint32 *ftk_entry = NULL;
    uint32 *ftk_sfield = NULL;
    uint32 *ftk_dfield = NULL;

    uint8 ele_data[BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN];
    bcmi_ft_alu_key_t *key_alu = NULL;
    int start = 0, length = 0, num_key_els = 0;
    int key_offset = 0, keytype_offset = 0;
    bcm_flowtracker_tracking_param_type_t param;
    bcmi_ft_group_alu_info_t *key_ext_info = NULL;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;

    gid = tbl_trv->curr_gid;
    mode = BCMI_FT_GROUP_FTFP_DATA(unit, gid).session_key_mode;

    if (mode == bcmiFtGroupModeSingle) {
        base_index = index;
        LOG_CLI((BSL_META_U(unit, "Index                         :\t"
                        "%d \n\r"), base_index));
    } else {
        base_index = index / 2;
        if (tbl_trv->in_tbl_type == TBL_TYPE_SINGLE) {
            LOG_CLI((BSL_META_U(unit, "Indices                       :\t"
                            "{ %d, %d }\n\r"), index, index + 1));
        } else {
            LOG_CLI((BSL_META_U(unit, "Index                         :\t"
                            "%d \n\r"), base_index));
        }
    }
    tbl_trv->base_index = base_index;

    if (BCMI_FT_GROUP_TYPE_IS_AGGREGATE(gid)) {
        LOG_CLI((BSL_META_U(unit, " - No Key -\n")));
        return BCM_E_NONE;
    }

    if (mode == bcmiFtGroupModeSingle) {
        key_mem = FT_KEY_SINGLEm;
    } else {
        key_mem = FT_KEY_DOUBLEm;
    }

    ftk_entry = tbl_trv->u32buffer1;
    ftk_sfield = tbl_trv->u32buffer2;
    ftk_dfield = tbl_trv->u32buffer3;

    num_bits = soc_mem_field_length(unit, DO_NOT_FT_ENTRYm, KEY_0f);
    num_bytes = BITS2BYTES(num_bits) * 2;

    rv = soc_mem_read(unit, key_mem, MEM_BLOCK_ANY, base_index, ftk_entry);
    BCM_IF_ERROR_RETURN(rv);

    ptr = tbl_trv->u8buffer1;
    if (mode == bcmiFtGroupModeSingle) {
        soc_mem_field_get(unit, key_mem, ftk_entry, KEYf, ftk_sfield);
        SHR_BITCOPY_RANGE(ftk_dfield, 0, ftk_sfield, 0, num_bits);
    } else {

        /* High part of the Key */
        soc_mem_field_get(unit, key_mem, ftk_entry, KEY_1f, ftk_sfield);
        SHR_BITCOPY_RANGE(ftk_dfield, num_bits, ftk_sfield, 0, num_bits);

        /* Low part of the Key */
        soc_mem_field_get(unit, key_mem, ftk_entry, KEY_0f, ftk_sfield);
        SHR_BITCOPY_RANGE(ftk_dfield, 0, ftk_sfield, 0, num_bits);
    }
    for (i = 0; i < BYTES2WORDS(num_bytes); i++) {
        _SHR_PACK_U32(ptr, ftk_dfield[(BYTES2WORDS(num_bytes) - i - 1)]);
    }

    LOG_CLI((BSL_META_U(unit, "****   Session Key                 *****\n")));

    key_ext_info = BCMI_FT_GROUP_EXT_KEY_INFO(unit, gid);
    num_key_els = BCMI_FT_GROUP_EXT_INFO(unit, gid).num_key_info;

    keytype_offset = 8;
    num_bits = BYTES2BITS(num_bytes);
    ptr = tbl_trv->u8buffer1;
    for (i = 0; i < num_key_els; i++) {
        key_alu = &(key_ext_info->key1);
        start = key_alu->location;
        length = key_alu->length;

        sal_memset(ele_data, 0, sizeof(ele_data));
        num_bytes = BITS2BYTES(length);
        key_offset = (num_bits - (keytype_offset + start + length));
        param = key_ext_info->element_type1;
        sal_memcpy(ele_data, &ptr[BITS2BYTES(key_offset)], num_bytes);

        if (param == bcmFlowtrackerTrackingParamTypeCustom) {
            LOG_CLI((BSL_META_U(unit, " %20s(%d) - "), plist[param],
                        key_ext_info->custom_id1));
        } else {
            LOG_CLI((BSL_META_U(unit, "%20s - "), plist[param]));
        }
        for (ii = 0; ii < num_bytes; ii++) {
            LOG_CLI((BSL_META_U(unit, "%02x "), ele_data[ii]));
        }
        LOG_CLI((BSL_META_U(unit, "\n")));

        key_ext_info++;
    }

    return BCM_E_NONE;
}

int
bcmi_ft_group_session_data_element_dump(
        int unit,
        int index,
        int offset,
        int length,
        bcmi_ft_group_param_type_t type,
        bcmi_ft_group_alu_info_t *temp,
        uint8 *ft_data)
{
    int rv = BCM_E_NONE;
    int ft_type = 0, i = 0;
    int eng_type = 0, elem_type = 0, action = 0;
    int nbytes = 0, data_offset = 0;
    uint16 custom_id = 0;
    uint8 ele_data[BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN];
    bcm_flowtracker_check_t flowcheck_id = 0;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    bcm_flowtracker_tracking_param_type_t element = 0;
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    const char *alist[] = BCM_FT_CHECK_ACTION_STRINGS;
    char *ft_type_str[] = {"IFT", "MFT", "EFT"};
    char *elem_type_str[] = {"Track", "Check"};
    char *data_engine_str[] = { "TIMESTAMP",
        "ALU32", "ALU16", "LOAD16", "LOAD8"};

    ft_type = bcmiFtTypeIFT;
    if ((type == bcmiFtGroupParamTypeTsNewLearn) ||
            (type == bcmiFtGroupParamTypeTsFlowStart) ||
            (type == bcmiFtGroupParamTypeTsFlowEnd) ||
            (type == bcmiFtGroupParamTypeTsCheckEvent1) ||
            (type == bcmiFtGroupParamTypeTsCheckEvent2) ||
            (type == bcmiFtGroupParamTypeTsCheckEvent3) ||
            (type == bcmiFtGroupParamTypeTsCheckEvent4) ||
            (type == bcmiFtGroupParamTypeTsIngress) ||
            (type == bcmiFtGroupParamTypeTsEgress)) {
        eng_type = 0; elem_type = 0;
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        ft_type = BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, index);
#endif
        element = bcmi_ft_group_get_tracking_param(unit, type);
    } else if ((temp != NULL) && (temp->flowchecker_id)) {
        sal_memset(&fc_info, 0,
                sizeof (bcmi_flowtracker_flowchecker_info_t));
        rv = bcmi_ft_flowchecker_get(unit,
                temp->flowchecker_id, &fc_info);
        BCM_IF_ERROR_RETURN(rv);

        elem_type = 1;
        flowcheck_id = temp->flowchecker_id;
        element = fc_info.action_info.param;
        action = fc_info.action_info.action;
        custom_id = fc_info.action_info.custom_id;
    } else if (type == bcmiFtGroupParamTypeTracking) {
        elem_type = 0;
        if (temp != NULL) {
            element = temp->element_type1;
            custom_id = temp->custom_id1;
        }
    } else {
        return rv;
    }

    if (temp != NULL) {
        if (temp->alu_load_type == bcmiFtAluLoadTypeAlu32) {
            eng_type = 1;
        } else if (temp->alu_load_type == bcmiFtAluLoadTypeAlu16) {
            eng_type = 2;
        } else if (temp->alu_load_type == bcmiFtAluLoadTypeLoad16) {
            eng_type = 3;
        } else {
            eng_type = 4;
        }
        ft_type = temp->alu_ft_type;
        index = temp->alu_load_index;
    }

    nbytes = BITS2BYTES(length);
    data_offset = (offset*8);
    sal_memcpy(ele_data, &ft_data[BITS2BYTES(data_offset)], nbytes);

    LOG_CLI((BSL_META_U(unit, "[%s]"), ft_type_str[ft_type - 1]));
    LOG_CLI((BSL_META_U(unit, "%10s"), data_engine_str[eng_type]));
    LOG_CLI((BSL_META_U(unit, "(%3d)"), index));
    LOG_CLI((BSL_META_U(unit, " %5s"), elem_type_str[elem_type]));

    if (flowcheck_id) {
        LOG_CLI((BSL_META_U(unit, "(0x%08x)-"), flowcheck_id));
        if (element == bcmFlowtrackerTrackingParamTypeCustom) {
            LOG_CLI((BSL_META_U(unit, " (%s(%d), %s)"), plist[element],
                        custom_id, alist[action]));
        } else {
            LOG_CLI((BSL_META_U(unit, " (%s, %s)"), plist[element],
                        alist[action]));
        }
    } else {
        LOG_CLI((BSL_META_U(unit, "%11s -"), " "));
        LOG_CLI((BSL_META_U(unit, " %20s"), plist[element]));
        if (element == bcmFlowtrackerTrackingParamTypeCustom) {
            LOG_CLI((BSL_META_U(unit, "(%d)"), custom_id));
        }
    }

    LOG_CLI((BSL_META_U(unit, " - ")));
    for (i = 0; i < nbytes; i++) {
        LOG_CLI((BSL_META_U(unit, "%02x "), ele_data[i]));
    }

    LOG_CLI((BSL_META_U(unit, "\n")));

    return BCM_E_NONE;
}

int
bcmi_ft_group_session_flow_state_dump(
        int unit,
        int offset,
        int length,
        uint8 *ft_data)
{
    int i = 0;
    int nbytes = 0, data_offset = 0;
    uint8 ele_data[BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN];

    nbytes = BITS2BYTES(length);
    data_offset = (offset*8);
    sal_memcpy(ele_data, &ft_data[BITS2BYTES(data_offset)], nbytes);

    LOG_CLI((BSL_META_U(unit, "[%s]"), "IFT"));
    LOG_CLI((BSL_META_U(unit, "%10s"), "FLOW_STT"));
    LOG_CLI((BSL_META_U(unit, "(%3d)"), 0));
    LOG_CLI((BSL_META_U(unit, " %5s"), "Local"));
    LOG_CLI((BSL_META_U(unit, "%11s -"), " "));
    LOG_CLI((BSL_META_U(unit, " %20s"), "(CurrState)"));
    LOG_CLI((BSL_META_U(unit, " - ")));

    for (i = 0; i < nbytes; i++) {
        LOG_CLI((BSL_META_U(unit, "%02x "), ele_data[i]));
    }

    return BCM_E_NONE;
}

int
bcmi_ft_group_hw_entry_data_decode(
        int unit,
        bcmi_ft_group_hw_tbl_trvrse_t *tbl_trv)
{
    int rv = BCM_E_NONE;
    int num_alus_loads = 0;
    int i, size, mode = 0;
    uint32 *global_pdd_entry;
    uint32 *ftd_entry = NULL, *ftd_field = NULL;
    soc_mem_t pdd_mem = INVALIDm;
    soc_mem_t data_mem = INVALIDm;
    bcm_flowtracker_group_t gid;
    bcmi_ft_group_alu_info_t *start = NULL;
    bcmi_ft_group_alu_info_t *alu_info = NULL;
    bcmi_ft_alu_load_type_t alu_load_type = bcmiFtAluLoadTypeNone;
    bcmi_ft_group_param_type_t ts_type = 0;
    uint8 *ptr, *ftd_u8data, *ft_u8pdd_data[3];
    int pdd_pos[3] = {0};
    bcmi_ft_type_t ft_type;

    gid = tbl_trv->curr_gid;
    mode = BCMI_FT_GROUP_FTFP_DATA(unit, gid).session_key_mode;

    if (mode == bcmiFtGroupModeSingle) {
        data_mem = BSC_DT_FLEX_SESSION_DATA_SINGLEm;
    } else {
        data_mem = BSC_DT_FLEX_SESSION_DATA_DOUBLEm;
    }

    ftd_entry = tbl_trv->u32buffer1;
    ftd_field = tbl_trv->u32buffer2;
    ftd_u8data = tbl_trv->u8buffer1;
    ft_u8pdd_data[0] = tbl_trv->u8buffer2;
    ft_u8pdd_data[1] = tbl_trv->u8buffer3;
    ft_u8pdd_data[2] = tbl_trv->u8buffer4;

    rv = soc_mem_read(unit, data_mem, MEM_BLOCK_ANY,
                tbl_trv->base_index, ftd_entry);
    BCM_IF_ERROR_RETURN(rv);

    soc_mem_field_get(unit, data_mem, ftd_entry, OPAQUE_DATAf, ftd_field);

    size = BITS2WORDS(soc_mem_field_length(unit, data_mem, OPAQUE_DATAf));

    ptr = ftd_u8data;
    for (i = 0; i < size; i++) {
        _SHR_PACK_U32(ptr, ftd_field[size - i - 1]);
    }

    if (SOC_IS_HELIX5(unit)) {
        ptr = ftd_u8data;
        ft_type = bcmiFtTypeIFT;
        sal_memcpy(ft_u8pdd_data[ft_type-1], ptr, sizeof(uint32) * size);
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
    } else {
        i = (mode == bcmiFtGroupModeSingle) ? 4: 8;
        size = 8;
        ptr = ftd_u8data;
        for (; i > 0; i--) {
            ft_type = BCMI_FT_SESSION_DATA_BANK_FT_TYPE(unit, i - 1);

            sal_memcpy(ft_u8pdd_data[ft_type-1] + pdd_pos[ft_type-1],
                    ptr, size);
            pdd_pos[ft_type-1] += size; ptr += size;
        }
#endif
    }
    pdd_pos[0] = pdd_pos[1] = pdd_pos[2] = 0;

    /* Retreive Global PDD Entry from Group Structure */
    global_pdd_entry = (uint32 *)(&(BCMI_FT_GROUP(unit, gid)->pdd_entry));

    /* Retreive Relevant Memory */
    bcmi_ft_alu_load_pdd_mem_get(unit, bcmiFtTypeIFT, &pdd_mem);
    num_alus_loads = (BCMI_FT_GROUP_EXT_INFO(unit, gid)).num_data_info;
    start = BCMI_FT_GROUP_EXT_DATA_INFO(unit, gid);
    ft_type = bcmiFtTypeIFT;

    LOG_CLI((BSL_META_U(unit, "****      Session Data             *****\n")));


    /* Timestamp 0 */
    if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                BSD_FLEX_TIMESTAMP_0f)) {

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        ft_type = BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 0);
#endif
        ts_type = bcmi_ft_group_get_ts_type(unit, gid, 0);
        bcmi_ft_group_session_data_element_dump(unit, 0, pdd_pos[ft_type-1],
                48, ts_type, NULL, ft_u8pdd_data[ft_type-1]);
        pdd_pos[ft_type-1] += 6;
    }

    /* Timestamp 1 */
    if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_1f)) {

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        ft_type = BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 1);
#endif
        ts_type = bcmi_ft_group_get_ts_type(unit, gid, 1);
        bcmi_ft_group_session_data_element_dump(unit, 1, pdd_pos[ft_type-1],
                48, ts_type, NULL, ft_u8pdd_data[ft_type-1]);
        pdd_pos[ft_type-1] += 6;
    }

    /* Timestamp 2 */
    if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_2f)) {

#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        ft_type = BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 2);
#endif
        ts_type = bcmi_ft_group_get_ts_type(unit, gid, 2);
        bcmi_ft_group_session_data_element_dump(unit, 2, pdd_pos[ft_type-1],
                48, ts_type, NULL, ft_u8pdd_data[ft_type-1]);
        pdd_pos[ft_type-1] += 6;
    }

    /* Timestamp 3 */
    if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                                      BSD_FLEX_TIMESTAMP_3f)) {
#if defined(BCM_FLOWTRACKER_V2_SUPPORT)
        ft_type = BCMI_FT_TIMESTAMP_ENGINE_FT_TYPE(unit, 3);
#endif
        ts_type = bcmi_ft_group_get_ts_type(unit, gid, 3);
        bcmi_ft_group_session_data_element_dump(unit, 3, pdd_pos[ft_type-1],
                48, ts_type, NULL, ft_u8pdd_data[ft_type-1]);
        pdd_pos[ft_type-1] += 6;
    }

    /* Meter Entry - It always belongs to IFT */
    if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                                      BSD_FLEX_FLOW_METERf)) {
        pdd_pos[bcmiFtTypeIFT-1] += 1;
    }

    /* Add ALU32s into the template list based on PDD. */
    for (i = 0; i < TOTAL_GROUP_ALU32_MEM; i++) {

        alu_load_type = bcmiFtAluLoadTypeAlu32;
        if (bcmi_ft_group_alu_load_pdd_status_get(unit,
                    gid, global_pdd_entry, alu_load_type, i)) {

            /* Check for this index and get corresponding data. */
            rv = bcmi_ft_group_alu_load_index_match_data_get
                (unit, gid, alu_load_type, i, &alu_info);
            if (BCM_FAILURE(rv)) {
                continue;
            }
            ft_type = BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, alu_load_type, i, 0);
            bcmi_ft_group_session_data_element_dump(unit, i,
                    pdd_pos[ft_type-1], 32, bcmiFtGroupParamTypeTracking,
                    alu_info, ft_u8pdd_data[ft_type-1]);
            pdd_pos[ft_type-1] += 4;
        }
    }

    /* LOAD16s. */
    for (i = 0, alu_info = start; i < num_alus_loads; i++) {
        if (alu_info->alu_load_type == bcmiFtAluLoadTypeLoad16) {
            ft_type = alu_info->alu_ft_type;
            bcmi_ft_group_session_data_element_dump(unit, 0,
                    pdd_pos[ft_type-1], 16, bcmiFtGroupParamTypeTracking,
                    alu_info, ft_u8pdd_data[ft_type-1]);
            pdd_pos[ft_type-1] += 2;
        }
        alu_info++;
    }

    /* Add ALU16s into the template list based on PDD. */
    for (i = 0; i < TOTAL_GROUP_ALU16_MEM; i++) {

        alu_load_type = bcmiFtAluLoadTypeAlu16;
        if (bcmi_ft_group_alu_load_pdd_status_get(unit,
                    gid, global_pdd_entry, alu_load_type, i)) {

            /* Check for this index and get corresponding data. */
            rv = bcmi_ft_group_alu_load_index_match_data_get
                (unit, gid, alu_load_type, i, &alu_info);
            if (BCM_FAILURE(rv)) {
                continue;
            }
            ft_type = BCMI_FT_ALU_LOAD_MEM_FT_TYPE(unit, alu_load_type, i, 0);
            bcmi_ft_group_session_data_element_dump(unit, i,
                    pdd_pos[ft_type-1], 16, bcmiFtGroupParamTypeTracking,
                    alu_info, ft_u8pdd_data[ft_type-1]);
            pdd_pos[ft_type-1] += 2;
        }
    }

    /* COUNT TO CPU - It always belongs to IFT */
    if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                                      BSD_FLEX_FLOW_COUNT_TO_CPUf)) {
        pdd_pos[bcmiFtTypeIFT-1] += 1;
    }

    /* LOAD8s. */
    for (i = 0, alu_info = start; i < num_alus_loads; i++) {
        if (alu_info->alu_load_type == bcmiFtAluLoadTypeLoad8) {
            ft_type = alu_info->alu_ft_type;
            bcmi_ft_group_session_data_element_dump(unit, 0,
                    pdd_pos[ft_type-1], 8, bcmiFtGroupParamTypeTracking,
                    alu_info, ft_u8pdd_data[ft_type-1]);
            pdd_pos[ft_type-1] += 1;
        }
        alu_info++;
    }

    /* FLOW STATE - It always belongs to IFT */
    if (soc_mem_field32_get(unit, pdd_mem, global_pdd_entry,
                                      BSD_FLEX_FLOW_STATEf)) {
        bcmi_ft_group_session_flow_state_dump(unit,
                    pdd_pos[ft_type-1], 8, ft_u8pdd_data[ft_type-1]);
        pdd_pos[bcmiFtTypeIFT-1] += 1;
    }

    LOG_CLI((BSL_META_U(unit, "\n*****************************************\n")));

    return BCM_E_NONE;
}

int
bcmi_ft_group_micro_hw_entry_decode(
        int unit,
        soc_mem_t mem,
        uint32 index,
        void *entry,
        void *user_data,
        int *jmp)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_t ent_gid;
    bcmi_ft_group_key_data_mode_t mode;
    bcm_flowtracker_group_type_t group_type;
    bcmi_ft_group_hw_tbl_trvrse_t *tbl_trv = NULL;

    *jmp = 1;
    tbl_trv = (bcmi_ft_group_hw_tbl_trvrse_t *) user_data;

    if (!soc_mem_field32_get(unit, mem, entry, ENTRY_VALIDf)) {
        return BCM_E_NONE;
    }

    group_type = bcmFlowtrackerGroupTypeNone;
    ent_gid = soc_mem_field32_get(unit, mem, entry, GROUP_IDf);

    /* Skip if group is not configured */
    if (!SHR_BITGET(BCMI_FT_GROUP_BITMAP(unit, group_type), ent_gid)) {
        return BCM_E_NONE;
    }

    mode = BCMI_FT_GROUP_FTFP_DATA(unit, ent_gid).session_key_mode;
    *jmp = (mode == bcmiFtGroupModeSingle) ? 1 : 2;

    if (tbl_trv->id != -1) {
        if (ent_gid != tbl_trv->id) {
            return BCM_E_NONE;
        }
    }

    tbl_trv->curr_gid = ent_gid;

    if (!(tbl_trv->flags & F_GRP_NO_SHOW)) {
        bcmi_ft_group_hw_entry_group_show(unit, tbl_trv);

        if (tbl_trv->id != -1) {
            tbl_trv->flags |= F_GRP_NO_SHOW;
        }
    }

    rv = bcmi_ft_group_hw_entry_key_decode(unit, tbl_trv, index);
    BCM_IF_ERROR_RETURN(rv);

    return bcmi_ft_group_hw_entry_data_decode(unit, tbl_trv);
}

int
bcmi_ft_group_hw_tbl_traverse(
        int unit,
        soc_mem_t mem,
        int start_idx,
        int end_idx,
        bcmi_ft_group_hw_entry_decode_cb_f cb,
        void *user_data)
{
#define DMA_CHUNK_SIZE 1024

    int rv = BCM_E_NONE;
    int jmp = 0;
    int8 *buffer = NULL;
    void *entry = NULL;
    uint32 idx = 0, e_idx = 0;
    int mem_min = 0, mem_max = 0;
    uint32 chunk_size = 0, mem_entry_size = 0;

    mem_min = soc_mem_index_min(unit, mem);
    mem_max = soc_mem_index_max(unit, mem);

    end_idx = (end_idx < 0) ? mem_max : end_idx;
    start_idx = (start_idx < 0) ? mem_min : start_idx;

    if ((start_idx > mem_max) ||
            (end_idx > mem_max) ||
            (start_idx > end_idx)) {
        LOG_CLI((BSL_META_U(unit, "Table Index range not valid.\n ")));
        return BCM_E_NONE;
    }

    chunk_size = ((end_idx - start_idx + 1) > DMA_CHUNK_SIZE) ?
                DMA_CHUNK_SIZE : (end_idx - start_idx + 1);
    mem_entry_size = SOC_MEM_WORDS(unit, mem) * sizeof(uint32);

    buffer = soc_cm_salloc(unit, mem_entry_size * chunk_size, "mem buffer");
    if (buffer == NULL)  {
        return BCM_E_MEMORY;
    }
    for (idx = start_idx; idx <= end_idx; idx += chunk_size) {
        chunk_size = ((end_idx - idx + 1) > DMA_CHUNK_SIZE) ?
                    DMA_CHUNK_SIZE : (end_idx - idx + 1);

        if ((rv = soc_mem_read_range(unit, mem, MEM_BLOCK_ALL,
                    idx, idx + chunk_size - 1, buffer)) != SOC_E_NONE) {
            break;
        }
        for (e_idx = 0; e_idx < chunk_size; e_idx += jmp) {
            entry = soc_mem_table_idx_to_pointer(unit, mem, void *,
                            buffer, e_idx);
            rv = (*cb)(unit, mem, idx + e_idx, entry, user_data, &jmp);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
        if (BCM_FAILURE(rv)) {
            break;
        }
    }

    if (buffer != NULL) {
        soc_cm_sfree(unit, buffer);
    }
    return rv;
}

void
bcmi_ft_group_micro_hw_entries_decode(
        int unit,
        uint32 flags,
        bcm_flowtracker_group_t id,
        int in_tbl_type,
        int index,
        int count)
{
    int rv = BCM_E_NONE;
    int start_idx, end_idx;
    bcmi_ft_group_hw_tbl_trvrse_t tbl_trv;

    sal_memset(&tbl_trv, 0, sizeof (tbl_trv));

    tbl_trv.flags = flags;
    tbl_trv.id    = id;
    tbl_trv.in_tbl_type = in_tbl_type;

    if (in_tbl_type == TBL_TYPE_DOUBLE) {
        if (index != -1)  {
            index = index * 2;
        }
        if (count != -1) {
            count = count * 2;
        }
    }
    start_idx = (index == -1) ? 0 : index;
    end_idx = (count == -1) ? -1 : (start_idx + count - 1);

    /* allocate buffers for key/data fields */
    bcmi_ft_group_hw_entry_buf_allocate(unit, &tbl_trv);

    rv = bcmi_ft_group_hw_tbl_traverse(unit, BSC_AG_AGE_TABLEm,
            start_idx, end_idx, bcmi_ft_group_micro_hw_entry_decode,
            (void *) &tbl_trv);
    if (BCM_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit, "Error encounterd.\n ")));
    }

    /* Free all allocated buffers */
    bcmi_ft_group_hw_tbl_trvse_t_buf_free(unit, &tbl_trv);
}

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
int
bcmi_ft_group_hw_agg_entry_data_decode(
        int unit,
        bcmi_ft_group_hw_tbl_trvrse_t *tbl_trv)
{
    int i = 0;
    int rv = BCM_E_NONE;
    uint16 custom_id = 0;
    uint32 valid = 0;
    uint32 data = 0;
    int new_index = 0, a_idx = 0;
    soc_mem_t mem = INVALIDm;
    bcmi_ft_group_alu_info_t *alu_info = NULL;
    bcmi_ft_alu_load_type_t alu_load_type;
    uint32 *ftd_bank_buf[4] = {NULL};
    int elem_type = 0, action = 0;
    bcm_flowtracker_tracking_param_type_t element;
    bcmi_flowtracker_flowchecker_info_t fc_info;
    char *elem_type_str[] = {"Track", "Check"};
    const char *plist[] = BCM_FT_TRACKING_PARAM_STRINGS;
    const char *alist[] = BCM_FT_CHECK_ACTION_STRINGS;

    soc_mem_t data_bank_mem[] = {
        BSC_DT_FLEX_AFT_SESSION_DATA_BANK_0m,
        BSC_DT_FLEX_AFT_SESSION_DATA_BANK_1m,
        BSC_DT_FLEX_AFT_SESSION_DATA_BANK_2m,
        BSC_DT_FLEX_AFT_SESSION_DATA_BANK_3m
    };
    soc_field_t agg_alu_field[] = {
        ALU32_0f, ALU32_1f, ALU32_2f, ALU32_3f};

    ftd_bank_buf[0] = tbl_trv->u32buffer1;
    ftd_bank_buf[1] = tbl_trv->u32buffer2;
    ftd_bank_buf[2] = tbl_trv->u32buffer3;
    ftd_bank_buf[3] = tbl_trv->u32buffer4;

    for (i = 0; i < COUNTOF(data_bank_mem); i++) {
        rv = soc_mem_read(unit, data_bank_mem[i], MEM_BLOCK_ANY,
                tbl_trv->base_index, ftd_bank_buf[i]);
        BCM_IF_ERROR_RETURN(rv);
    }

    alu_load_type = bcmiFtAluLoadTypeAggAlu32;

    LOG_CLI((BSL_META_U(unit, "*****      Session Data             *****\n")));

    for (i = 0; i < TOTAL_GROUP_AGG_ALU32_MEM; i++) {

        rv = bcmi_ft_group_alu_load_index_match_data_get(unit,
                tbl_trv->curr_gid, alu_load_type, i, &alu_info);

        if (BCM_FAILURE(rv)) {
            continue;
        }

        bcmi_ft_alu_load_mem_index_get(unit,
                alu_info->alu_load_index,
                alu_load_type, &new_index, &mem, &a_idx);

        valid = soc_mem_field32_get(unit, data_bank_mem[a_idx/4],
                ftd_bank_buf[a_idx/4], VALIDf);
        if (valid == 0) {
            LOG_CLI((BSL_META_U(unit, " - No HIT -\n\n\r")));
            return BCM_E_NONE;
        }

        data = soc_mem_field32_get(unit, data_bank_mem[a_idx/4],
                ftd_bank_buf[a_idx/4], agg_alu_field[a_idx%4]);

        if ((alu_info->flowchecker_id)) {
            sal_memset(&fc_info, 0,
                    sizeof (bcmi_flowtracker_flowchecker_info_t));
            rv = bcmi_ft_flowchecker_get(unit,
                    alu_info->flowchecker_id, &fc_info);
            BCM_IF_ERROR_RETURN(rv);

            elem_type = 1;
            element = fc_info.action_info.param;
            action = fc_info.action_info.action;
            custom_id = fc_info.action_info.custom_id;
        } else {
            elem_type = 0;
            element = alu_info->element_type1;
            custom_id = alu_info->custom_id1;
        }

        LOG_CLI((BSL_META_U(unit, "[%d, %d]"), a_idx/4, a_idx%4));
        LOG_CLI((BSL_META_U(unit, " %5s"), elem_type_str[elem_type]));

        if (alu_info->flowchecker_id) {
            LOG_CLI((BSL_META_U(unit, "(0x%08x)-"), alu_info->flowchecker_id));
            if (element == bcmFlowtrackerTrackingParamTypeCustom) {
                LOG_CLI((BSL_META_U(unit, " (%s(%d), %s)"), plist[element],
                            custom_id, alist[action]));
            } else {
                LOG_CLI((BSL_META_U(unit, " (%s, %s)"), plist[element],
                            alist[action]));
            }
        } else {
            LOG_CLI((BSL_META_U(unit, "%11s -"), " "));
            LOG_CLI((BSL_META_U(unit, " %20s"), plist[element]));
            if (element == bcmFlowtrackerTrackingParamTypeCustom) {
                LOG_CLI((BSL_META_U(unit, "(%d)"), custom_id));
            }
        }

        LOG_CLI((BSL_META_U(unit, " - ")));
        LOG_CLI((BSL_META_U(unit, "0x%08x\n"), data));
    }
    LOG_CLI((BSL_META_U(unit, "\n*****************************************\n")));

    return BCM_E_NONE;
}

int
bcmi_ft_group_agg_hw_entry_decode(
        int unit,
        int index,
        bcmi_ft_group_hw_tbl_trvrse_t *tbl_trv)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_t ent_gid;

    ent_gid = BCMI_FTK_USER_ENTRY_INFO(unit, \
            tbl_trv->group_type, index)->group_id;

    if (tbl_trv->id != -1) {
        if (ent_gid != tbl_trv->id) {
            return BCM_E_NONE;
        }
    }

    tbl_trv->curr_gid = ent_gid;

    if (!(tbl_trv->flags & F_GRP_NO_SHOW)) {
        bcmi_ft_group_hw_entry_group_show(unit, tbl_trv);

        if (tbl_trv->id != -1) {
            tbl_trv->flags |= F_GRP_NO_SHOW;
        }
    }

    rv = bcmi_ft_group_hw_entry_key_decode(unit, tbl_trv, index);
    BCM_IF_ERROR_RETURN(rv);

    return bcmi_ft_group_hw_agg_entry_data_decode(unit, tbl_trv);
}

void
bcmi_ft_group_agg_hw_entries_decode(
        int unit,
        uint32 flags,
        bcm_flowtracker_group_t id,
        int in_tbl_type,
        int index,
        int count)
{
    int rv = BCM_E_NONE;
    int i, bank, start_idx, end_idx;
    bcmi_ft_type_t ft_type;
    bcm_flowtracker_group_type_t group_type;
    bcmi_ft_group_hw_tbl_trvrse_t tbl_trv;

    sal_memset(&tbl_trv, 0, sizeof (tbl_trv));

    if (in_tbl_type == TBL_TYPE_AGG0) {
        bank = 0;
    } else if (in_tbl_type == TBL_TYPE_AGG1) {
        bank = 1;
    } else if (in_tbl_type == TBL_TYPE_AGG2) {
        bank = 2;
    } else {
        bank = 3;
    }

    ft_type = BCMI_FT_AGG_SESSION_DATA_BANK_FT_TYPE(unit, bank, 0);

    if (ft_type == bcmiFtTypeAIFT) {
        group_type = bcmFlowtrackerGroupTypeAggregateIngress;
    } else if (ft_type == bcmiFtTypeAMFT) {
        group_type = bcmFlowtrackerGroupTypeAggregateMmu;
    } else {
        group_type = bcmFlowtrackerGroupTypeAggregateEgress;
    }

    if (id != -1) {
        if (group_type != BCMI_FT_GROUP_TYPE_GET(id)) {
            LOG_CLI((BSL_META_U(unit, "table/group type mismatch.\n ")));
            return;
        }
    }

    if (BCMI_FTK_USER_ENTRY_TYPE(unit, group_type) == NULL) {
        return;
    }

    tbl_trv.flags = flags;
    tbl_trv.id    = id;
    tbl_trv.in_tbl_type = in_tbl_type;
    tbl_trv.group_type = group_type;

    /* allocate buffers for key/data fields */
    bcmi_ft_group_hw_entry_buf_allocate(unit, &tbl_trv);

    start_idx = (index == -1) ? 0 : index;
    if (count == -1) {
        end_idx = BCMI_FTK_USER_ENTRY_MAX_COUNT(unit, group_type) - 1;
    } else {
        end_idx = (start_idx + count - 1);

        /* end_idx cannot exceed table size */
        if (end_idx >= BCMI_FTK_USER_ENTRY_MAX_COUNT(unit, group_type)) {
            end_idx = BCMI_FTK_USER_ENTRY_MAX_COUNT(unit, group_type) - 1;
        }
    }

    for (i = start_idx; (i <= end_idx) && (rv == BCM_E_NONE); i++) {
        if (!BCMI_FTK_USER_ENTRY_INFO(unit, group_type, i)) {
            continue;
        }

        rv = bcmi_ft_group_agg_hw_entry_decode(unit, i, &tbl_trv);
    }
    if (BCM_FAILURE(rv)) {
        LOG_CLI((BSL_META_U(unit, "Error encounterd.\n ")));
    }

    /* Free all allocated buffers */
    bcmi_ft_group_hw_tbl_trvse_t_buf_free(unit, &tbl_trv);
}
#endif

void
bcmi_ft_group_hw_entries_decode(
        int unit,
        uint32 flags,
        bcm_flowtracker_group_t id,
        char *table_name,
        int index,
        int count)
{
    int in_tbl_type = 0;

    if (!strcasecmp(table_name, "single")) {
        in_tbl_type = TBL_TYPE_SINGLE;
    } else if (!strcasecmp(table_name, "double")) {
        in_tbl_type = TBL_TYPE_DOUBLE;
    } else if ((!strcasecmp(table_name, "agg")) ||
            (!strcasecmp(table_name, "agg0"))) {
        in_tbl_type =  TBL_TYPE_AGG0;
    } else if (!strcasecmp(table_name, "agg1")) {
        in_tbl_type =  TBL_TYPE_AGG1;
    } else if (!strcasecmp(table_name, "agg2")) {
        in_tbl_type =  TBL_TYPE_AGG2;
    } else if (!strcasecmp(table_name, "agg3")) {
        in_tbl_type =  TBL_TYPE_AGG3;
    } else {
        in_tbl_type = -1;
    }

    switch(in_tbl_type) {
        case TBL_TYPE_SINGLE:
        case TBL_TYPE_DOUBLE:
            bcmi_ft_group_micro_hw_entries_decode(
                    unit, flags, id, in_tbl_type, index, count);
            break;
#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
        case TBL_TYPE_AGG0:
        case TBL_TYPE_AGG1:
        case TBL_TYPE_AGG2:
        case TBL_TYPE_AGG3:
            if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3)) {
                bcmi_ft_group_agg_hw_entries_decode(
                        unit, flags, id, in_tbl_type, index, count);
            }
            break;
#endif
        default:
            LOG_CLI((BSL_META_U(unit, "Table not supported.\n ")));
            break;
    }

    return;
}

int
bcmi_ft_export_fifo_alloc_show(int unit)
{
    int ix = 0;
    int num_fifo_entries;
    int *fifo_coll_map = NULL;
    int entry_coll_list[BCMI_FT_FIFO_ENTRIES_MAX] = {0};
    bcmi_ft_collector_info_t *ft_coll_info = NULL;

    num_fifo_entries = BCMI_FT_EXPORT_INFO(unit)->num_fifo_entries;
    fifo_coll_map = BCMI_FT_EXPORT_INFO(unit)->fifo_collector_map;

    LOG_CLI((BSL_META_U(unit, "FT Export FIFO to Collector Mapping:\n\r")));
    LOG_CLI((BSL_META_U(unit, " | Index | Count | Collector\n\r")));
    for (ix = 0; ix < BCMI_FT_EXPORT_INFO(unit)->max_collectors; ix++) {
        ft_coll_info = BCMI_FT_COLLECTOR_INFO(unit, ix);

        if (ft_coll_info->coll_hw_id == BCMI_FT_COLLECTOR_ID_INVALID) {
            continue;
        }

        entry_coll_list[ft_coll_info->min_pointer] = ft_coll_info->cmn_coll_id;
    }

    for (ix = 0; ix < num_fifo_entries; ix++) {
        if (fifo_coll_map[ix] == 0) {
            continue;
        }
        if (fifo_coll_map[ix] < 0) {
            LOG_CLI((BSL_META_U(unit, " |   %3d |   %3d | %4d\n\r"),
                        ix, -fifo_coll_map[ix], entry_coll_list[ix]));
        } else {
            LOG_CLI((BSL_META_U(unit, " |   %3d |   %3d |   ---\n\r"),
                        ix, fifo_coll_map[ix]));
        }
    }

    LOG_CLI((BSL_META_U(unit, "\n\r")));

#if defined(BCM_FLOWTRACKER_V3_SUPPORT)
    if (soc_feature(unit, soc_feature_flex_flowtracker_ver_3) &&
        soc_feature(unit, soc_feature_flowtracker_sw_agg_periodic_export)) {
        bcmi_ftv3_aft_export_sw_ctrl_show(unit);
    }
#endif /* BCM_FLOWTRACKER_V3_SUPPORT */

    return BCM_E_NONE;
}

#else /* BCM_FLOWTRACKER_SUPPORT */
typedef int bcmi_ft_diag_not_empty; /* Make ISO compilers happy. */
#endif /* BCM_FLOWTRACKER_SUPPORT */
