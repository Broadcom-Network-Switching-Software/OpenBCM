/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * chenr 08/08/2018
 *
 * kbp flexible mode application reference
 * flexible mode allows the user to access KBP driver directly instead of using DNX APIs
 * enable feature by SOC - custom_feature_ext_flexible_mode=1
 *  
 * flexible_mode_ref_app() - main function, includes getting all kbp info from SDK, add customer config and set kbp APIs direcly.
 * flexible_mode_acl_add() - add acl fields to master key.
 * bcm_switch_kbp_info_get() -get all kbp device pointer from SDK.
 * bcm_switch_kbp_apptype_info_get() - get apptype info per apptype
 * flexible_mode_kbp_configuration() -
 *  (a) build master key from Fwd fields and ACL fields.
 *  (b) add new DBs to kbp.
 *  (c) add Fwd/RPF (from SDK) DBs to kbp.
 *  (d) install instruction.
 * kbp_device_lock() - perfrom device lock.
 * add_em_kbp_entry() - add exact match entry to kbp.
 * add_acl_kbp_entry() - add acl entry to kbp.
 */

#if defined(INCLUDE_KBP)


/*************
 * INCLUDES  *
 */
/* shell.h is needed for sh_process_command(), which here is used to init and deinit the KBP appl */
#include <appl/diag/shell.h>
/* arad_kbp.h is needed for utility structures and functions used for diagnostic logging */
#include <soc/dpp/ARAD/arad_kbp.h>
#include <soc/kbp/alg_kbp/include/device.h>
#include <soc/kbp/alg_kbp/include/tap.h>
#include <include/bcm/field.h>

/* IPv4 shared pkt forwarding header segments attributes */
static ARAD_KBP_LTR_SINGLE_SEARCH ipv4_sharing_forwarding_header_dynamic_segments =
       {2, {{"SIP", 4, KBP_KEY_FIELD_EM}, {"DIP", 4, KBP_KEY_FIELD_EM}}};

/* IPv4 shared pkt forwarding next header segments attributes */
static ARAD_KBP_LTR_SINGLE_SEARCH ipv4_sharing_forwarding_next_header_dynamic_segments =
       {2, {{"ipv4_src_port", 2, KBP_KEY_FIELD_TERNARY}, {"ipv4_dest_port", 2, KBP_KEY_FIELD_TERNARY}}};

/* Definitions for simplifying logging for the flexible mode reference application */
/* The macros assume that the variable 'verbosity_level' exists in the same scope as them */
#define FLEXIBLE_REF_APP_LOG_LEVEL_INFO     1
#define FLEXIBLE_REF_APP_LOG_LEVEL_VERBOSE  2
#define FLEXIBLE_REF_APP_LOG_LEVEL_DEBUG    3

#define FLEXIBLE_REF_APP_LOG_INFO(...) \
    if(verbosity_level >= FLEXIBLE_REF_APP_LOG_LEVEL_INFO) \
        FLEXIBLE_REF_APP_LOG(__VA_ARGS__, "")

#define FLEXIBLE_REF_APP_LOG_VERBOSE(...) \
    if(verbosity_level >= FLEXIBLE_REF_APP_LOG_LEVEL_VERBOSE) \
        FLEXIBLE_REF_APP_LOG(__VA_ARGS__, "")

#define FLEXIBLE_REF_APP_LOG_DEBUG(...) \
    if(verbosity_level >= FLEXIBLE_REF_APP_LOG_LEVEL_DEBUG) \
        FLEXIBLE_REF_APP_LOG(__VA_ARGS__, "")

#define FLEXIBLE_REF_APP_LOG(_format_, ...) \
    LOG_CLI((BSL_META(_format_ "%s"), ##__VA_ARGS__))

int reference_tap_app(int unit, bcm_switch_kbp_info_t *kbp_info, uint8 verbosity_level) {

    int res = 0;
    int db_id;
    int nof_dbs = 1;
    struct kbp_tap_db *db[2];
    struct kbp_tap_db_pointer *db_ptr[2];
    struct kbp_tap_db_value *db_value;
    struct kbp_tap_db_attribute *db_attr[2];

    struct kbp_tap_instruction *inst_th[2];
    struct kbp_tap_record *rec_th[2];

    int attr_value, counter_offset, hole_len;
    int nof_queues = 16*1024;
    int set_size = 8;
    int ptr_len = 8;
    int attr_len = 8;
    int packet_size = 14;
    int ing_rec_size = 80;
    int null_rec_size = 80;

    FLEXIBLE_REF_APP_LOG_INFO("Creating TAP configuration\n");

    /** Set the ingress and null record lengths */
    res = kbp_device_set_property((struct kbp_device*)kbp_info->device_p, KBP_DEVICE_PROP_TAP_INGRESS_REC_SIZE, ing_rec_size);
    if(res)
    {
        printf("Error, kbp_device_set_property KBP_DEVICE_PROP_TAP_INGRESS_REC_SIZE failed: %s\n", kbp_get_status_string(res));
        return  res;
    }
    res = kbp_device_set_property((struct kbp_device*)kbp_info->device_p, KBP_DEVICE_PROP_TAP_NULL_REC_SIZE, null_rec_size);
    if (res)
    {
        printf("Error, kbp_device_set_property KBP_DEVICE_PROP_TAP_NULL_REC_SIZE failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /** Enable counter compression since packet and byte count mode */
    res = kbp_device_set_property((struct kbp_device*)kbp_info->device_p, KBP_DEVICE_PROP_COUNTER_COMPRESSION, 1);
    if (res)
    {
        printf("Error, kbp_device_set_property KBP_DEVICE_PROP_COUNTER_COMPRESSION failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /** Create tap databases */
    for (db_id = 0; db_id < nof_dbs; db_id++)
    {
        /** Port-0 is mapped to Thread-0 and Port-2 is mapped to Thread-1 */
        /** Entry type is PAIR since packet and byte count mode */
        res = kbp_tap_db_init((struct kbp_device*)kbp_info->device_p, db_id, nof_queues, KBP_TAP_ENTRY_TYPE_PAIR, set_size, &db[db_id]);
        if (res)
        {
            printf("Error, kbp_tap_db_init failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
        /** Initialize pointer */
        ptr_len = 16;
        res = kbp_tap_db_pointer_init(db[db_id], &db_ptr[db_id]);
        if (res)
        {
            printf("Error, kbp_tap_db_pointer_init failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_db_pointer_add_field(db_ptr[db_id], "QUEUE_ID", ptr_len);
        if (res)
        {
            printf("Error, kbp_tap_db_pointer_add_field failed: %s db%d QUEUE_ID\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_db_set_pointer(db[db_id], db_ptr[db_id]);
        if (res)
        {
            printf("Error, kbp_tap_db_set_pointer failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
        /*
         * Add value field
         */
        res = kbp_tap_db_value_init(db[db_id], &db_value);
        if (res)
        {
            printf("Error, kbp_tap_db_value_init failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_db_value_set_field(db_value, "PACKET_SIZE", packet_size);
        if (res)
        {
            printf("Error, kbp_tap_db_value_set_field failed: %s db%d PACKET_SIZE\n", kbp_get_status_string(res), db_id);
            return res;
        }

        /** Initialize attribute, add fields. attr_len = 1 + 2 */
        attr_len = 3;
        res = kbp_tap_db_attribute_init(db[db_id], &db_attr[db_id]);
        if (res)
        {
            printf("Error, kbp_tap_db_attribute_init failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_db_attribute_add_field(db_attr[db_id], "DISPOSITION", 1);
        if (res)
        {
            printf("Error, kbp_tap_db_attribute_add_field failed: %s db%d DISPOSITION\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_db_attribute_add_field(db_attr[db_id], "DROP_PRECEDENCE", 2);
        if (res)
        {
            printf("Error, kbp_tap_db_attribute_add_field failed: %s db%d DROP_PRECEDENCE\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_db_set_attribute(db[db_id], db_attr[db_id]);
        if (res)
        {
            printf("Error, kbp_tap_db_set_attribute failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }

        /*
         * Attribute value to counter offset mapping:
         *
         * Offset-0: Forward, DP = 0, Offset-1: Forward, DP = 1
         * Offset-2: Forward, DP = 2, Offset-3: Forward, DP = 3
         * Offset-4: Drop,    DP = 0, Offset-5: Drop, DP = 1
         * Offset-6: Drop,    DP = 2, Offset-7: Drop, DP = 3
         */
        for (attr_value = 0, counter_offset = 0; attr_value < set_size; attr_value++, counter_offset++)
        {
            res = kbp_tap_db_map_attr_value_to_entry(db[db_id], attr_value, counter_offset);
            if (res)
            {
                printf("Error, kbp_tap_db_map_attr_value_to_entry failed: %s db%d\n", kbp_get_status_string(res), db_id);
                return res;
            }
        }
        /*
         * instructions, on Th-i
         * The ingress record looks as follows:
         *
         * Bits 78..79: record type
         * Bits 64..77: packet size
         * Bits 19..63: hole
         * Bits 17..18: DP
         * Bits 16: disposition (forward = 0, drop = 1)
         * Bits 0..15: queue_id.
         */
        res = kbp_tap_instruction_init((struct kbp_device*)kbp_info->device_p, db_id, KBP_TAP_INSTRUCTION_TYPE_INGRESS, &inst_th[db_id]);
        if (res)
        {
            printf("Error, kbp_tap_instruction_init failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_record_init((struct kbp_device*)kbp_info->device_p, KBP_TAP_RECORD_TYPE_INGRESS, &rec_th[db_id]);
        if (res)
        {
            printf("Error, kbp_tap_record_init failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
        hole_len = ing_rec_size - (2 + packet_size + attr_len + ptr_len);

        res = kbp_tap_record_add_field(rec_th[db_id], "RECORD_TYPE", 2, KBP_TAP_RECORD_FIELD_OPCODE);
        if (res)
        {
            printf("Error, kbp_tap_record_add_field failed: %s db%d RECORD_TYPE\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_record_add_field(rec_th[db_id], "PACKET_SIZE", packet_size, KBP_TAP_RECORD_FIELD_VALUE);
        if (res)
        {
            printf("Error, kbp_tap_record_add_field failed: %s db%d PACKET_SIZE\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_record_add_field(rec_th[db_id], "HOLE", hole_len, KBP_TAP_RECORD_FIELD_HOLE);
        if (res)
        {
            printf("Error, kbp_tap_record_add_field failed: %s db%d HOLE\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_record_add_field(rec_th[db_id], "DROP_PRECEDENCE", 2, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (res)
        {
            printf("Error, kbp_tap_record_add_field failed: %s db%d DROP_PRECEDENCE\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_record_add_field(rec_th[db_id], "DISPOSITION", 1, KBP_TAP_RECORD_FIELD_ATTRIBUTE);
        if (res)
        {
            printf("Error, kbp_tap_record_add_field failed: %s db%d DISPOSITION\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_record_add_field(rec_th[db_id], "QUEUE_ID", ptr_len, KBP_TAP_RECORD_FIELD_POINTER);
        if (res)
        {
            printf("Error, kbp_tap_record_add_field failed: %s db%d QUEUE_ID\n", kbp_get_status_string(res), db_id);
            return res;
        }

        /*
         * Associate record with the instruction. Add database and finally install the instruction.
         */
        res = kbp_tap_instruction_set_record(inst_th[db_id], rec_th[db_id]);
        if (res)
        {
            printf("Error, kbp_tap_instruction_set_record failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_instruction_add_db(inst_th[db_id], db[db_id], 0, nof_queues);
        if (res)
        {
            printf("Error, kbp_tap_instruction_add_db failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
        res = kbp_tap_instruction_install(inst_th[db_id]);
        if (res)
        {
            printf("Error, kbp_tap_instruction_install failed: %s db%d\n", kbp_get_status_string(res), db_id);
            return res;
        }
    }

    return res;
}

/* Utility functions with logging purpose */
static const char *
apptype_to_string(bcm_field_AppType_t apptype)
{
    const char *apptype_text[bcmFieldAppTypeCount] = BCM_FIELD_APPTYPE_STRINGS;

    if(apptype >= bcmFieldAppTypeCount)
        return "Unknown";
    else
        return apptype_text[apptype];
}

/* Utility function to dump the retrieved apptype information */
static void apptype_info_dump(
    bcm_field_AppType_t apptype,
    bcm_switch_kbp_apptype_info_t *apptype_info)
{
    int i;
    bcm_switch_kbp_apptype_info_t *info = &apptype_info[apptype];

    LOG_CLI((BSL_META("\tApptype %d %s\n"), apptype, apptype_to_string(apptype)));
    LOG_CLI((BSL_META("\t\tValid %s\tOpcode %d\tLTR Index %d\n"),
            (info->is_valid ? "TRUE" : "FALSE"), info->opcode, info->ltr_index));
    LOG_CLI((BSL_META("\t\tNumber of segments %d\tTotal key size %d\n"),
            info->master_key_nof_segments, info->total_segments_size));

    for(i = 0; i < info->master_key_nof_segments; i++) {
        LOG_CLI((BSL_META("\t\t\t<\"%s\"> <Bits %d> <Type %s>\n"),
                info->master_key_segment[i].segment_name,
                info->master_key_segment[i].segment_size_bits,
                ARAD_KBP_KEY_FIELD_TYPE_to_string(info->master_key_segment[i].segment_type)));
    }
}

/* return shared pkt header segments attributes from application type and concat header type */
ARAD_KBP_LTR_SINGLE_SEARCH *
    kbp_pkt_header_for_sharing(int opcode, bcm_switch_concat_header_t concat_header)
{
    /* concat header type is forwarding header */
    if (concat_header == bcmSwitchConcatHeaderForwarding)
    {
        /* applicaion type */
        switch(opcode)
        {
            case PROG_FLP_IPV4UC:
                return &ipv4_sharing_forwarding_header_dynamic_segments;
            default:
                return NULL;
        }
    }

    /* concat header type is next forwarding header */
    if (concat_header == bcmSwitchConcatHeaderNextForwarding)
    {
        /* application type */
        switch(opcode)
        {
            case PROG_FLP_IPV4UC:
                return &ipv4_sharing_forwarding_next_header_dynamic_segments;
            default:
                return NULL;
        }
    }

    return NULL;
}

/*************
* FUNCTIONS *
 */

/* example function for creating KBP ACL field groups and and adding their key to the master key of the apptypes used */
int flexible_mode_acl_add(int unit, bcm_switch_concat_header_t concat_header, uint8 verbosity_level)
{
    bcm_field_group_config_t group;
    int presel_id = 0;              /* used for bcmFieldAppTypeIp4Ucast */
    int presel_id2 = 0;             /* used for bcmFieldAppTypeL2 */
    int presel_flags = 0;
    int res = 0;

    /* create preselector id */
    res = bcm_field_presel_create(unit, &presel_id);
    if(res)
    {
        printf("Error, bcm_field_presel_create failed\n");
        return  res;
    }

    /* set preselectors value */
    bcm_field_qualify_Stage(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageExternal);
    bcm_field_qualify_AppType(unit, presel_id | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldAppTypeIp4Ucast);

    bcm_field_group_config_t_init(&group);

    /* add preselectors to preselector set for first field-group */
    BCM_FIELD_PRESEL_ADD(group.preselset, presel_id);

    group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |BCM_FIELD_GROUP_CREATE_WITH_ID;
    group.mode = bcmFieldGroupModeAuto;
    group.group = 80;
    group.priority = 40;

    /* add qualifiers for first field-group */
    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageExternal);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyIntPriority);

    /* L4SrcPort get from packet next forwarding header for IPv4 in bcmSwitchConcatHeaderNextForwarding mode */
    if(concat_header != bcmSwitchConcatHeaderNextForwarding) 
    {
        BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyL4SrcPort);
    }

    /* add actions for first field-group */
    BCM_FIELD_ASET_INIT(group.aset);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionExternalValue0Set);

    /* create first field-group for bcmFieldAppTypeIp4Ucast */
    FLEXIBLE_REF_APP_LOG_DEBUG("Creating first ACL group for apptype Ip4Ucast\n");
    res = bcm_field_group_config_create(unit, &group);
    if(res)
    {
        printf("Error, bcm_field_group_config_create failed\n");
        return  res;
    }

    bcm_field_group_config_t_init(&group);

    /* add preselectors to preselector set for second field-group */
    BCM_FIELD_PRESEL_ADD(group.preselset, presel_id);

    group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |BCM_FIELD_GROUP_CREATE_WITH_ID;
    group.mode = bcmFieldGroupModeAuto;
    group.group = 82;
    group.priority = 42;

    /* add qualifiers for second field-group */
    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageExternal);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyColor);

    /* add actions for second field-group */
    BCM_FIELD_ASET_INIT(group.aset);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionExternalValue1Set);

    /* create second field-group for bcmFieldAppTypeIp4Ucast */
    FLEXIBLE_REF_APP_LOG_DEBUG("Creating second ACL group for apptype Ip4Ucast\n");
    res = bcm_field_group_config_create(unit, &group);
    if(res)
    {
        printf("Error, bcm_field_group_config_create failed\n");
        return  res;
    }

    /* create preselector id */
    res = bcm_field_presel_create(unit, &presel_id2);
    if(res)
    {
        printf("Error, bcm_field_presel_create failed\n");
        return  res;
    }

    /* set preselectors value */
    bcm_field_qualify_Stage(unit, presel_id2 | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldStageExternal);
    bcm_field_qualify_AppType(unit, presel_id2 | BCM_FIELD_QUALIFY_PRESEL | presel_flags, bcmFieldAppTypeL2);

    bcm_field_group_config_t_init(&group);

    /* add preselectors to preselector set for third field-group */
    BCM_FIELD_PRESEL_ADD(group.preselset, presel_id2);

    group.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET |BCM_FIELD_GROUP_CREATE_WITH_ID;
    group.mode = bcmFieldGroupModeAuto;
    group.group = 84;
    group.priority = 44;

    /* add qualifiers for third field-group */
    BCM_FIELD_QSET_INIT(group.qset);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyStageExternal);
    BCM_FIELD_QSET_ADD(group.qset, bcmFieldQualifyDstMac);

    /* add actions for third field-group */
    BCM_FIELD_ASET_INIT(group.aset);
    BCM_FIELD_ASET_ADD(group.aset, bcmFieldActionExternalValue0Set);

    /* create third field-group for bcmFieldAppTypeL2 */
    FLEXIBLE_REF_APP_LOG_DEBUG("Creating ACL group for apptype L2\n");
    res = bcm_field_group_config_create(unit, &group);
    if(res)
    {
        printf("Error, bcm_field_group_config_create failed\n");
        return  res;
    }

    return res;
}

/* build master key for all instructions */
int build_kbp_master_key(int unit, bcm_core_t core, bcm_switch_kbp_info_t* kbp_info, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t* apptype_info)
{
    struct kbp_key *master_key;
    int total_segments_size_bits = 0;
    int segment = 0;
    int reminder_bits = 0;
    int srch_indx = 0;
    int res = 0;

    /* initialize kbp master key */
    res = kbp_key_init((struct kbp_device*)(kbp_info->device_p), &master_key);
    if(res)
    {
        printf("Error, kbp_key_init for master_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize total_segments_size_bits per apptype to 0 */
    total_segments_size_bits = 0;

    /* run over all segments */
    for (segment = 0; segment < apptype_info[apptype].master_key_nof_segments; ++segment)
    {
        /* if one of the segments is above 32b, it is divded to 2 segments. both segments will have the same name.
           in order to add both, need to change the name of the segment */
        for (srch_indx = segment + 1; srch_indx < apptype_info[apptype].master_key_nof_segments; ++srch_indx)
        {
            if (sal_strcmp(((char*)apptype_info[apptype].master_key_segment[segment].segment_name),((char*)apptype_info[apptype].master_key_segment[srch_indx].segment_name)) == 0)
            {
                sal_strncat(((char*)apptype_info[apptype].master_key_segment[segment].segment_name), "1", 1);
            }
        }

        /* if the qualifer is not byte align, do not add this sigment. will add it when segemnt is byte align in next iteration.
           aligned segment will be original one and zero padding. for example dp will be 2b dp then 6b zeros */
        if (apptype_info[apptype].master_key_segment[segment].segment_size_bits%8 != 0)
        {
            apptype_info[apptype].master_key_segment[segment+1].segment_size_bits += apptype_info[apptype].master_key_segment[segment].segment_size_bits;
            sal_strncpy(((char*)apptype_info[apptype].master_key_segment[segment+1].segment_name), 
                        ((char*)apptype_info[apptype].master_key_segment[segment].segment_name), 
                        BCM_SEGMENT_NAME_SIZE);
            continue;
        }

        /* in flex mode acl only, add dummy segment only for fwd dummy db */
        /* check if segment holds "DUMMY" */
        if (sal_strstr(((char*)apptype_info[apptype].master_key_segment[segment].segment_name), "DUMMY") != NULL)
        {
            /* do not add dummy segment */
            if (apptype_info[apptype].ltr_index != ARAD_KBP_FRWRD_LTR_DUMMY_FRWRD)
            {
                continue;
            }
            /* add dummy segment only for dummy fwd db */
            else
            {
                apptype_info[apptype].master_key_segment[segment].segment_type = 2;
            }
        }

        /* add fields to kbp master key */
        res = kbp_key_add_field(master_key, ((char*)apptype_info[apptype].master_key_segment[segment].segment_name),
                                apptype_info[apptype].master_key_segment[segment].segment_size_bits,
                                apptype_info[apptype].master_key_segment[segment].segment_type);
        if (res)
        {
            printf("Error, [kbp master key] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
            return  res;
        }

        /* update total segments size */
        total_segments_size_bits = total_segments_size_bits + apptype_info[apptype].master_key_segment[segment].segment_size_bits;
    }

    /* need to padd master key to 80b multipication alignment
       if total number size of segments is not 80b align, add field to align to 80b multiply */
    if ((total_segments_size_bits)%BCM_ALIGN_MASTER_KEY != 0)
    {
        reminder_bits = BCM_ALIGN_MASTER_KEY - (total_segments_size_bits)%BCM_ALIGN_MASTER_KEY;
        res = kbp_key_add_field(master_key, "PADD_80b_multiply", reminder_bits, KBP_KEY_FIELD_HOLE);
        if (res)
        {
            printf("Error, [kbp master key] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }

    /* update total segments size */
    apptype_info[apptype].total_segments_size = total_segments_size_bits + reminder_bits;

    /* set master key */
    res = kbp_instruction_set_key((kbp_instruction_t*)(apptype_info[apptype].inst_p), master_key);
    if(res)
    {
        printf("Error, kbp_instruction_set_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return  res;
}

/* add fw/rpf/dummy dbs to all instructions */
int add_kbp_db(bcm_switch_kbp_info_t kbp_info, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t* apptype_info)
{
    int res = 0;

    /* check if fw db is valid */
    if (apptype_info[apptype].fwd_db)
    {
        /* add fwd db to instruction */
        res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[apptype].inst_p), (struct kbp_db*)(apptype_info[apptype].fwd_db), 0);
        if(res)
        {
            printf("Error, [fwd_db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }

    /* check if dummy db is valid */
    if (apptype_info[apptype].dummy_db)
    {
        /* add dummy db to instruction */
        res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[apptype].inst_p), (struct kbp_db*)(apptype_info[apptype].dummy_db), 1);
        if(res)
        {
            printf("Error, [dummy_db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }

    /* check if rpf db is valid */
    if (apptype_info[apptype].rpf_db)
    {
        /* add rpf db to instruction */
        res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[apptype].inst_p), (struct kbp_db*)(apptype_info[apptype].rpf_db), 2);
        if(res)
        {
            printf("Error, [rpf_db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }

    return  res;
}

/* add new em db, relevant for bcmFieldAppTypeIp4Ucast */
int add_kbp_new_em_db(bcm_core_t core, bcm_switch_kbp_info_t* kbp_info, bcm_switch_kbp_apptype_info_t* apptype_info, struct kbp_db **kbp_db_em, struct kbp_ad_db **ad_db_em)
{
    struct kbp_key *key_em;
    int db_id_em = 1;
    int db_size = 1024;
    int res = 0;

    /* initialize em db for customer specific configurations */
    res = kbp_db_init((struct kbp_device*)(kbp_info->device_p), KBP_DB_EM, db_id_em, db_size, kbp_db_em);
    if(res)
    {
        printf("Error, [em db] kbp_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize kbp em key */
    res = kbp_key_init((struct kbp_device*)(kbp_info->device_p), &key_em);
    if(res)
    {
        printf("Error, [key_em] kbp_key_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add DIP(32b) field to em kbp key - key format: DIP(32b) */
    res = kbp_key_add_field(key_em, "DIP", 32, KBP_KEY_FIELD_EM);
    if(res)
    {
        printf("Error, [em kbp key - DIP] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add em key to em kbp db */
    res = kbp_db_set_key(*kbp_db_em, key_em);
    if(res)
    {

        printf("Error, [em key] kbp_db_set_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize the em ad db */
    res = kbp_ad_db_init((struct kbp_device*)(kbp_info->device_p), db_id_em, db_size, NLM_TBL_ADLEN_24B, ad_db_em);
    if(res)
    {

        printf("Error, [em ad db] kbp_ad_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate the em ad db with em kbp db lookup */
    res = kbp_db_set_ad(*kbp_db_em, *ad_db_em);
    if(res)
    {

        printf("Error, [em ad] kbp_db_set_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* adding new db to the IPv4 unicast instruction */
    res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[bcmFieldAppTypeIp4Ucast].inst_p), *kbp_db_em, db_id_em);
    if(res)
    {
        printf("Error, [em db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return  res;
}

/* add new ternary db, relevant for bcmFieldAppTypeIp4Ucast */
int add_kbp_new_ternary_db(bcm_core_t core, bcm_switch_kbp_info_t* kbp_info, bcm_switch_kbp_apptype_info_t* apptype_info, struct kbp_db **kbp_db_ternary, struct kbp_ad_db **ad_db_ternary)
{
    struct kbp_key *key_ternary;
    int db_id_ternary = 4;
    int db_size = 1024;
    int res = 0;

     /* initialize ternary db for customer specific configurations */
    res = kbp_db_init((struct kbp_device*)(kbp_info->device_p), KBP_DB_ACL, db_id_ternary, db_size, kbp_db_ternary);
    if(res)
    {
        printf("Error, [ternary db] kbp_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize kbp ternary key */
    res = kbp_key_init((struct kbp_device*)(kbp_info->device_p), &key_ternary);
    if(res)
    {
        printf("Error, [key_ternary] kbp_key_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* key format: ipv4_src_port(16b), tc(3b tc + 5b padding for sigment align) */
    /* add tc(3b tc + 5b padding for sigment align) field to ternary kbp key */
    res = kbp_key_add_field(key_ternary, "fwd_dec_tc", 8, KBP_KEY_FIELD_TERNARY);
    if(res)
    {
        printf("Error, [ternary kbp key - fwd_dec_tc] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ipv4_src_port(16b) field to ternary kbp key */
    res = kbp_key_add_field(key_ternary, "ipv4_src_port", 16, KBP_KEY_FIELD_TERNARY);
    if(res)
    {
        printf("Error, [ternary kbp key - IPv4SrcPort] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ternary key to ternary kbp db  */
    res = kbp_db_set_key(*kbp_db_ternary, key_ternary);
    if(res)
    {

        printf("Error, [ternary key] kbp_db_set_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize the ternary ad db */
    res = kbp_ad_db_init((struct kbp_device*)(kbp_info->device_p), db_id_ternary, db_size, NLM_TBL_ADLEN_24B, ad_db_ternary);
    if(res)
    {

        printf("Error, [ternary ad db] kbp_ad_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate the ternary ad db with ternary kbp db lookup */
    res = kbp_db_set_ad(*kbp_db_ternary, *ad_db_ternary);
    if(res)
    {

        printf("Error, [ternary ad] kbp_db_set_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ternary db to instruction */
    res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[bcmFieldAppTypeIp4Ucast].inst_p), *kbp_db_ternary, db_id_ternary);
    if(res)
    {
        printf("Error, [ternary db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* add new ternary db - relevant for bcmFieldAppTypeL2 */
int add_kbp_new_ternary_db_l2(bcm_core_t core, bcm_switch_kbp_info_t* kbp_info, bcm_switch_kbp_apptype_info_t* apptype_info, struct kbp_db **kbp_db_ternary_l2, struct kbp_ad_db **ad_db_ternary_l2)
{
    struct kbp_key *key_ternary;
    int db_id_ternary = 8;
    int db_size = 1024;
    int res = 0;
    int db_id_result = 5;

     /* initialize ternary db for customer specific configurations */
    res = kbp_db_init((struct kbp_device*)(kbp_info->device_p), KBP_DB_ACL, db_id_ternary, db_size, kbp_db_ternary_l2);
    if(res)
    {
        printf("Error, [l2 ternary db] kbp_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize kbp ternary key */
    res = kbp_key_init((struct kbp_device*)(kbp_info->device_p), &key_ternary);
    if(res)
    {
        printf("Error, [l2 key_ternary] kbp_key_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* key format: da (48b). each segment is up to 32b. so need to add 2 segments for da */
    /* add first segment for da (16b) */
    res = kbp_key_add_field(key_ternary, "da1", 16, KBP_KEY_FIELD_TERNARY);
    if(res)
    {
        printf("Error, [l2 ternary kbp key - da1] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add second segment for da (32b) */
    res = kbp_key_add_field(key_ternary, "da", 32, KBP_KEY_FIELD_TERNARY);
    if(res)
    {
        printf("Error, [l2 ternary kbp key - da] kbp_key_add_field failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ternary key to ternary kbp db  */
    res = kbp_db_set_key(*kbp_db_ternary_l2, key_ternary);
    if(res)
    {

        printf("Error, [l2 ternary key] kbp_db_set_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* initialize the ternary ad db */
    res = kbp_ad_db_init((struct kbp_device*)(kbp_info->device_p), db_id_ternary, db_size, NLM_TBL_ADLEN_24B, ad_db_ternary_l2);
    if(res)
    {

        printf("Error, [l2 ternary ad db] kbp_ad_db_init failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate the ternary ad db with ternary kbp db lookup */
    res = kbp_db_set_ad(*kbp_db_ternary_l2, *ad_db_ternary_l2);
    if(res)
    {

        printf("Error, [l2 ternary ad] kbp_db_set_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ternary db to instruction */
    res = kbp_instruction_add_db((kbp_instruction_t*)(apptype_info[bcmFieldAppTypeL2].inst_p), *kbp_db_ternary_l2, db_id_result);
    if(res)
    {
        printf("Error, [l2 ternary db] kbp_instruction_add_db failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* kbp driver configuration */
int flexible_mode_kbp_configuration(int unit, bcm_core_t core, bcm_switch_kbp_info_t* kbp_info, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t* apptype_info, struct kbp_db **kbp_db_em, struct kbp_db **kbp_db_ternary, struct kbp_db **kbp_db_ternary_l2, struct kbp_ad_db **ad_db_em, struct kbp_ad_db **ad_db_ternary, struct kbp_ad_db **ad_db_ternary_l2)
{
    int res = 0;

    /* build master key for all instructions */
    res = build_kbp_master_key(unit, core, kbp_info, apptype, apptype_info);
    if(res)
    {
        printf("Error, build_kbp_master_key failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add new dbs only for bcmFieldAppTypeIp4Ucast apptype */
    if (apptype == bcmFieldAppTypeIp4Ucast) 
    {
        /* in case new ltr was created (for example in acl mode only) do not add em db. "DIP" sigment is not in master key */
        if (apptype_info[apptype].ltr_index == 0) 
        {
            /* add new em db */
            res = add_kbp_new_em_db(core, kbp_info, apptype_info, kbp_db_em, ad_db_em);
            if(res)
            {
                printf("Error, add_kbp_new_db failed: %s\n", kbp_get_status_string(res));
                return  res;
            }
        }

        /* add new ternary db */
        res = add_kbp_new_ternary_db(core, kbp_info, apptype_info, kbp_db_ternary, ad_db_ternary);
        if(res)
        {
            printf("Error, add_kbp_new_ternary_db failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }
    
    /* add new db only for bcmFieldAppTypeL2 apptype */
    if (apptype == bcmFieldAppTypeL2) 
    { 
        /* add new ternary db */
        res = add_kbp_new_ternary_db_l2(core, kbp_info, apptype_info, kbp_db_ternary_l2, ad_db_ternary_l2);
        if(res)
        {
            printf("Error, add_kbp_new_ternary_db_l2 failed: %s\n", kbp_get_status_string(res));
            return  res;
        }
    }

    /* add fw/rpf/dummy dbs to all instructions */
    res = add_kbp_db(*kbp_info, apptype, apptype_info);
    if(res)
    {
        printf("Error, add_kbp_db failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* install instruction */
    res = kbp_instruction_install((kbp_instruction_t*)(apptype_info[apptype].inst_p));
    if(res)
    {
        printf("Error, kbp_instruction_install failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* update lut data per apptype */
int update_kbp_lut_access_info(int unit, bcm_core_t core, bcm_switch_kbp_info_t kbp_info, bcm_field_AppType_t apptype, bcm_switch_kbp_apptype_info_t* apptype_info)
{
    int res = 0;

    res = kbp_instruction_set_opcode((kbp_instruction_t*)(apptype_info[apptype].inst_p),apptype_info[apptype].opcode);
    if(res)
    {
        printf("Error, kbp_instruction_set_opcode failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* add em entry to kbp */
int add_em_kbp_entry(struct kbp_db *kbp_db_em, struct kbp_ad_db *ad_db_em)
{
    struct kbp_entry *kbp_entry_em;                     /* kbp em entry */
    struct kbp_ad *ad_entry_em;                         /* kbp ad em entry */
    uint8_t em_data[4] = {0x7f,0xff,0xff,0x03};         /* kbp em data lookup - 4B for DIP */
    uint8_t ad_data = 0x11;                             /* ad data result from kbp */
    int res = 0;

    /* add em entry to em ad db */
    res = kbp_ad_db_add_entry(ad_db_em, &ad_data, &ad_entry_em);
    if(res)
    {
        printf("Error, [em dip] kbp_ad_db_add_entry failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add em entry to kbp em db */
    res = kbp_db_add_em(kbp_db_em, (uint8*)em_data, &kbp_entry_em);
    if(res)
    {
        printf("Error, [em dip] kbp_db_add_em failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate kbp em entry with em ad entry */
    res = kbp_entry_add_ad(kbp_db_em, kbp_entry_em, ad_entry_em);
    if(res)
    {
        printf("Error, [em dip] kbp_entry_add_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* install kbp em db */
    res = kbp_db_install(kbp_db_em);
    if(res)
    {
        printf("Error, [em dip] kbp_db_install failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* add acl entry to kbp */
int add_acl_kbp_entry(struct kbp_db *kbp_db_ternary, struct kbp_ad_db *ad_db_ternary)
{

    struct kbp_entry *kbp_entry_ternary;                /* kbp ternary entry */
    struct kbp_ad *ad_entry_ternary;                    /* kbp ad ternary entry */
    uint8_t ternary_data[3] = {0x00,0x33,0x44};         /* kbp ternary data lookup - 8b (3b tc + 5b zero padding), 16b for L4 src port */
    uint8_t ternary_mask[3] = {0xf8,0x00,0x00};         /* kbp ternary data mask lookup - 8b (3b tc + 5b zero padding), 16b for L4 src port */
    uint32_t priority = 1;                              /* priority for acl entry in kbp */
    uint8_t ad_data = 0x22;                             /* ad data result from kbp */
    int res = 0;

    /* add ternary entry to ternary ad db */
    res = kbp_ad_db_add_entry(ad_db_ternary, &ad_data, &ad_entry_ternary);
    if(res)
    {
        printf("Error, [ternary ipv4+tc] kbp_ad_db_add_entry failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ternary entry to kbp ternary db */
    res = kbp_db_add_ace(kbp_db_ternary, (uint8*)ternary_data, (uint8*)ternary_mask, priority, &kbp_entry_ternary);
    if(res)
    {
        printf("Error, [ternary ipv4+tc] kbp_db_add_ace failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate kbp ternary entry with ternary ad entry */
    res = kbp_entry_add_ad(kbp_db_ternary, kbp_entry_ternary, ad_entry_ternary);
    if(res)
    {
        printf("Error, [ternary ipv4+tc] kbp_entry_add_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* install kbp ternary db */
    res = kbp_db_install(kbp_db_ternary);
    if(res)
    {
        printf("Error, [ternary ipv4+tc] kbp_db_install failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* add acl entry to kbp */
int add_acl_kbp_entry_l2(struct kbp_db *kbp_db_ternary_l2, struct kbp_ad_db *ad_db_ternary_l2)
{
    struct kbp_entry *kbp_entry_ternary;                        /* kbp ternary entry */
    struct kbp_ad *ad_entry_ternary;                            /* kbp ad ternary entry */
    uint8_t ternary_data[6] = {0x00,0x0c,0x00,0x02,0x00,0x00};  /* da (48b) */
    uint8_t ternary_mask[6] = {0x00,0x00,0x00,0x00,0x00,0x00};  /* da mask (48b) */
    uint32_t priority = 1;                                      /* priority for acl entry in kbp */
    uint8_t ad_data = 0x33;                                     /* ad data result from kbp */
    int res = 0;

    /* add ternary entry to ternary ad db */
    res = kbp_ad_db_add_entry(ad_db_ternary_l2, &ad_data, &ad_entry_ternary);
    if(res)
    {
        printf("Error, [ternary da] kbp_ad_db_add_entry failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* add ternary entry to kbp ternary db */
    res = kbp_db_add_ace(kbp_db_ternary_l2, (uint8*)ternary_data, (uint8*)ternary_mask, priority, &kbp_entry_ternary);
    if(res)
    {
        printf("Error, [ternary da] kbp_db_add_ace failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* associate kbp ternary entry with ternary ad entry */
    res = kbp_entry_add_ad(kbp_db_ternary_l2, kbp_entry_ternary, ad_entry_ternary);
    if(res)
    {
        printf("Error, [ternary da] kbp_entry_add_ad failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    /* install kbp ternary db */
    res = kbp_db_install(kbp_db_ternary_l2);
    if(res)
    {
        printf("Error, [ternary da] kbp_db_install failed: %s\n", kbp_get_status_string(res));
        return  res;
    }

    return res;
}

/* set concat applicaiton header type */
int update_concat_apptype_header(
    int unit,
    bcm_field_AppType_t apptype,
    bcm_switch_concat_header_t concat_header,
    bcm_switch_kbp_apptype_info_t *apptype_info)
{
    int res = 0;
    bcm_switch_control_key_t key;
    bcm_switch_control_info_t info;
    ARAD_KBP_LTR_SINGLE_SEARCH * concat_segments = NULL;
    int concat_base, concat_ndx, srch_ndx;

    /* Concat application header to master key in ELK */
    key.type = bcmSwitchConcatAppHeader;
    key.index = apptype;
    info.value = concat_header;
    /* This API decide to use concat forwarding header or forwarding next header */
    res = bcm_switch_control_indexed_set(unit, key, info);
    if(res)
    {
        printf("Error, bcm_switch_control_indexed_set failed in update_concat_apptype_header.\n");
        return  res;
    }

    /* get shared pkt header segments attributes from concat application header type */
    concat_segments = kbp_pkt_header_for_sharing(apptype_info->opcode, info.value);
    if (concat_segments != NULL)
    {
        concat_base = apptype_info->master_key_nof_segments;
        /* update master key segments attributes */
        for(concat_ndx = 0; concat_ndx < concat_segments->nof_key_segments; concat_ndx++)
        {
            int seg_size = 0;
            srch_ndx = concat_base + concat_ndx;
            seg_size = sizeof(concat_segments->key_segment[concat_ndx].name);

            /* update apptype_info struct with the segment name */
            sal_strncpy((char*)apptype_info->master_key_segment[srch_ndx].segment_name, concat_segments->key_segment[concat_ndx].name, seg_size);

            /* update segment size in bytes */
            apptype_info->master_key_segment[srch_ndx].segment_size_bits = (concat_segments->key_segment[concat_ndx].nof_bytes) * 8;
            apptype_info->total_segments_size += apptype_info->master_key_segment[srch_ndx].segment_size_bits;

            /* update segment type */
            apptype_info->master_key_segment[srch_ndx].segment_type = concat_segments->key_segment[concat_ndx].type;

            /* update master key number of segments */
            ++(apptype_info->master_key_nof_segments);
        }
    }

    return res;
}

/* application refernece for kbp flexible mode */
int flexible_mode_ref_app(int unit, bcm_switch_concat_header_t concat_header, uint8 verbosity_level, uint8 tap_init)
{
    int res = 0;
    bcm_core_t core = 0;                                                /* core id */
    bcm_switch_kbp_info_t kbp_info;                                     /* hold all kbp configutarion */
    struct kbp_db *kbp_db_em;                                           /* kbp em db */
    struct kbp_db *kbp_db_ternary;                                      /* kbp ternary db */
    struct kbp_db *kbp_db_ternary_l2;                                   /* kbp ternary db */
    struct kbp_ad_db *ad_db_em;                                         /* kbp ad em db */
    struct kbp_ad_db *ad_db_ternary;                                    /* kbp ad ternary db */
    struct kbp_ad_db *ad_db_ternary_l2;                                 /* kbp ad ternary db */
    bcm_switch_kbp_apptype_info_t *apptype_info;                        /* app type info */
    bcm_field_AppType_t apptype;                                        /* apptype */

    /* create KBP ACL field groups and and add their key to the master key */
    FLEXIBLE_REF_APP_LOG_INFO("Calling ACL add example\n");
    res = flexible_mode_acl_add(unit, concat_header, verbosity_level);
    if(res)
    {
        printf("Error, flexible_mode_acl_add failed\n");
        return  res;
    }

    /* need to restart application for acl changes to apply */
    FLEXIBLE_REF_APP_LOG_INFO("Resetting the KBP application\n");
    sh_process_command(unit, "kbp deinit_appl");
    sh_process_command(unit, "kbp init_appl");

    /* get kbp pointer from SDK */
    FLEXIBLE_REF_APP_LOG_INFO("Retrieving KBP device pointer\n");
    res = bcm_switch_kbp_info_get(unit, core, &kbp_info);
    if(res)
    {
        printf("Error, bcm_switch_kbp_info_get failed\n");
        return  res;
    }

    apptype_info = (bcm_switch_kbp_apptype_info_t *)sal_alloc(sizeof(bcm_switch_kbp_apptype_info_t)*bcmFieldAppTypeCount, "apptype_info array");
    if (apptype_info == NULL) {
        cli_out("Memory allocation failure for apptype_info\n");
        return BCM_E_INTERNAL;
    }
    sal_memset(apptype_info, 0, sizeof(bcm_switch_kbp_apptype_info_t)*bcmFieldAppTypeCount);
    /* run over all apptypes */
    FLEXIBLE_REF_APP_LOG_INFO("Updating all apptypes\n");
    for (apptype = 0; apptype < bcmFieldAppTypeCount; ++apptype) 
    {
        int seg_index;
        /* allocate the seg_name in the apptype with the max size BCM_SEGMENT_NAME_SIZE */
        for(seg_index = 0; seg_index < BCM_MAX_NOF_SEGMENTS; seg_index++)
        {
            apptype_info[apptype].master_key_segment[seg_index].segment_name = sal_alloc(BCM_SEGMENT_NAME_SIZE, "segment name");
            if(apptype_info[apptype].master_key_segment[seg_index].segment_name == NULL) {
                cli_out("Memory allocation failure, seg_index %d\n", seg_index);
                res = BCM_E_INTERNAL;
                goto exit;
            }
        }
        
        /* get apptype info per apptype */
        res = bcm_switch_kbp_apptype_info_get(unit, core, apptype, &(apptype_info[apptype]));
        if(res)
        {
            printf("Error, bcm_switch_kbp_apptype_info_get failed\n");
            apptype_info_dump(apptype, apptype_info);
            goto exit;
        }

        /* check apptype is valid */
        if (apptype_info[apptype].is_valid)
        {
            /* bcmSwitchConcatHeaderNone will not update concat application header */
            if (concat_header)
            {
                /* update concat application header type */
                FLEXIBLE_REF_APP_LOG_INFO("Updating concat apptype header\n");
                res = update_concat_apptype_header(unit, bcmFieldAppTypeIp4Ucast, concat_header, &apptype_info[apptype]);
                if(res)
                {
                    printf("Error, update_concat_apptype_header failed\n");
                    goto exit;
                }
            }

            FLEXIBLE_REF_APP_LOG_VERBOSE("\tConfiguring apptype %d %s\n", apptype, apptype_to_string(apptype));
            if (verbosity_level == FLEXIBLE_REF_APP_LOG_LEVEL_DEBUG)
            {
                apptype_info_dump(apptype, apptype_info);
            }
            /* set kbp configuration - customer specific configurations
               also add fwd/rpf dbs and example of em and acl dbs */
            res = flexible_mode_kbp_configuration(unit, core, &kbp_info, apptype, apptype_info, &kbp_db_em, &kbp_db_ternary, &kbp_db_ternary_l2, &ad_db_em, &ad_db_ternary, &ad_db_ternary_l2);
            if(res)
            {
                printf("Error, flexible_kbp_user_specific_configurations failed\n");
                goto exit;
            }

            /* update lut info*/
            FLEXIBLE_REF_APP_LOG_VERBOSE("\tUpdating the LUT, opcode %d\n", apptype_info[apptype].opcode);
            res = update_kbp_lut_access_info(unit, core, kbp_info, apptype, apptype_info);
            if(res)
            {
                printf("Error, update_kbp_lut_access_info failed\n");
                goto exit;
            }
        }
    }

    /* TAP init validation */
    if (tap_init) {
        if(ARAD_KBP_IS_OP2) {
            res = reference_tap_app(unit, &kbp_info, verbosity_level);
            if (res)
            {
                printf("Error, reference_tap_app failed\n");
                goto exit;
            }
        } else {
            printf("Error, tap configuration is allowed only with OP2\n");
            res = BCM_E_PARAM;
            goto exit;
        }
    }

    /* device lock */
    FLEXIBLE_REF_APP_LOG_INFO("Performing device lock\n");
    res = kbp_device_lock((struct kbp_device*)(kbp_info.device_p));
    if(res)
    {
        printf("Error, kbp_device_lock failed: %s\n", kbp_get_status_string(res));
        goto exit;
    }

    /* in case new ltr was created (for example in acl mode only) do not add em entry. "DIP" sigment is not in master key */
    if (apptype_info[bcmFieldAppTypeIp4Ucast].ltr_index == 0)
    {
        /* example - add em entry to kbp */
        FLEXIBLE_REF_APP_LOG_INFO("Adding EM entry\n");
        res = add_em_kbp_entry(kbp_db_em, ad_db_em);
        if(res)
        {
            printf("Error, add_em_kbp_entry failed: %s\n", kbp_get_status_string(res));
            goto exit;
        }
    }

    /* example - add acl entry to kbp */
    FLEXIBLE_REF_APP_LOG_INFO("Adding ACL entry\n");
    res = add_acl_kbp_entry(kbp_db_ternary, ad_db_ternary);
    if(res)
    {
        printf("Error, add_acl_kbp_entry failed: %s\n", kbp_get_status_string(res));
        goto exit;
    }

    /* example - add acl entry to kbp */
    FLEXIBLE_REF_APP_LOG_INFO("Adding ACL L2 entry\n");
    res = add_acl_kbp_entry_l2(kbp_db_ternary_l2, ad_db_ternary_l2);
    if(res)
    {
        printf("Error, add_acl_kbp_entry failed: %s\n", kbp_get_status_string(res));
        goto exit;
    }

exit:
    /* dallocate the seg_name in the apptype */
    for (apptype = 0; apptype < bcmFieldAppTypeCount; ++apptype) 
    {
        int seg_index;
        for(seg_index = 0; seg_index < BCM_MAX_NOF_SEGMENTS; seg_index++)
        {
            if (apptype_info[apptype].master_key_segment[seg_index].segment_name != NULL) {
                sal_free(apptype_info[apptype].master_key_segment[seg_index].segment_name);
            }
        }
    }

    sal_free(apptype_info);
    return  res;
}

#else /* not defined(INCLUDE_KBP) */

/*
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
int
dummy_empty_function_to_make_compiler_happy(
    int unit)
{
    return 0;
}

#endif
