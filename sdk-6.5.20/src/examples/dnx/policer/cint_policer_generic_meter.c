/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_policer_generic_meter.c
 * Purpose:     example for generic meter configuration
 *
 * *
 * The cint include:
 *  - configuration for generic policer database. (ingress and egress)
 *  - attach engines (shared with crps) for the policer database
 *  - detach engines (shared with crps) from the policer database
 *  - enable/disable for generic policer database.
 *  - destroy generic policer database.
 *  - several modes of meter configurations
 *  - destroy one meter
 *  - destroy all meters
 *  - select using MEF 10.2 or MEF 10.3 mode
 *  - drop packet based on egress metering using trap action.
 *  - egress policer pointer generation
 *
 * cints dependency:
 *  1. cint_port_misc_configuration.c
 * Note: For testing with IXIA, in order to compensate 4 CRC bytes removed in the device,
 *      need to call API bcm_switch_control_port_set(0,in_port,bcmSwitchMeterAdjust,4);
 */


int policer_generic_qos_map_id;
   
/**
* \brief
*  1. Creta AC LIF for the given port and vlan.
*  2. create qos profile in order to map prio and cfi into tc and dp.
*  3. map the AC LIF to the qos profile
* \param [in] unit             - Device ID
* \param [in] in_port          - in port
* \param [in] vlan             - vlan ID
* \param [in] nof_entries      - number of entries in the qos profile (how many {prio,cfi}=>{tc,dp} to do).
* \param [in] pkt_pri          - pkt_pri
* \param [in] cfi              - cfi
* \param [in] tc               - mapped TC
* \param [in] dp               - mapped DP
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int policer_generic_qos_ingress_port_set(
    int unit,
    int in_port,
    int vlan,
    int nof_entries,
    int *pkt_pri,
    int *pkt_cfi,
    int *tc,
    int *dp)
{
    int rc = BCM_E_NONE;

    int flags, i;
    int qos_ingress;
    int opcode_ingress;
    bcm_qos_map_t l2_in_map;
    bcm_vlan_port_t in_vlan_port;
    bcm_vlan_port_t_init(&in_vlan_port);

    /** first, create AC LIF */
    in_vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    in_vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    in_vlan_port.match_vlan  = vlan;
    /** vsi is the vid in bcm_l2_addr_t_init, it doesn't have to be the same as the incoming vlan on the packet */
    in_vlan_port.vsi  = vlan;
    in_vlan_port.port = in_port;
    rc = bcm_vlan_port_create(unit, &in_vlan_port);
    if (BCM_E_NONE != rc)
    {
     printf("1: Error in bcm_vlan_port_create\n");
     return rc;
    }

     printf("In Vlan Port Create: Port-Id=0x%x\n",  in_vlan_port.vlan_port_id);


    /** create qos profile */
    flags = (BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK);
    rc = bcm_qos_map_create(unit, flags, &policer_generic_qos_map_id);
    if (BCM_E_NONE != rc)
    {
        printf("1: Error in bcm_qos_map_create\n");
        return rc;
    }

    flags = BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE ;
    rc = bcm_qos_map_create(unit, flags, &opcode_ingress);
    if (BCM_E_NONE != rc)
    {
        printf("2: Error in bcm_qos_map_create\n");
        return rc;
    }

    bcm_qos_map_t_init(&l2_in_map);

    l2_in_map.opcode = opcode_ingress;
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_PHB | BCM_QOS_MAP_OPCODE;
    rc = bcm_qos_map_add(unit, flags, &l2_in_map, policer_generic_qos_map_id);
    if (BCM_E_NONE != rc)
    {
        printf("1: Error in bcm_qos_map_add\n");
        return rc;
    }
    bcm_qos_map_t_init(&l2_in_map);
    flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG | BCM_QOS_MAP_PHB;

    /* Set ingress pkt_pri/cfi Priority */
    for(i = 0; i < nof_entries; i++)
    {
        l2_in_map.pkt_pri = pkt_pri[i];
        l2_in_map.pkt_cfi = pkt_cfi[i];
        l2_in_map.int_pri = tc[i];
        l2_in_map.color = dp[i];
        rc = bcm_qos_map_add(unit, flags, &l2_in_map, opcode_ingress);
        if (BCM_E_NONE != rc)
        {
            printf("2: Error in bcm_qos_map_add\n");
            return rc;
        }
        printf("qos_map_id=%d, map {pkt_pri=%d, pkt_cfi=%d} to {int_pri=%d, color=%d} \n",
            policer_generic_qos_map_id, l2_in_map.pkt_pri, l2_in_map.pkt_cfi, l2_in_map.int_pri, dp[i]);
    }
    /** map the vlan pport to the qos profile */
    rc = bcm_qos_port_map_set(unit, in_vlan_port.vlan_port_id, policer_generic_qos_map_id, -1);
    if (BCM_E_NONE != rc)
    {
        printf("1: Error in bcm_qos_port_map_set\n");
        return rc;
    }

    printf("map vlan_port=%d to qos_map_id=%d \n", in_vlan_port.vlan_port_id, policer_generic_qos_map_id);
    return rc;
}


/**
 * \brief
 *  sequence of creating and activate policer database from scratch:
 *      a. create the database using API bcm_policer_database_create
 *      b. attach memory engines for the database using API bcm_policer_engine_database_attach. all meters point to profile 0 which hold max rate and max burst.
 *      c. configure all profiles for all meters in that database using API bcm_policer_create*Nof_meters
 *      d. enable the meter database operation
 * \param [in] unit -unit id
 * \param [in] core_id -core id (All/0/1)
 * \param [in] is_ingress -refer to ingress or egress policer
 * \param [in] database_id -database_id (0..2 for ingress, 0..1 for egress). database id is actually the command id or the statistic_interface that is used.
 * \param [in] base_pointer -the base object-stat-id after expansion if exist for this database.
 *                 In this example,we assume that the pointer range are consecutive, (but it doesn't have to be).
 * \param [in] single_bucket_mode -is the databse used in dual bucket mode (each meter has green and yellow bucket) or single bucket mode (only green bucket).
 * \param [in] expansion_enable -if enable, the database is using a meter_pointer=object-stat-id*nof_expansion_groups as defined by API bcm_policer_expansion_groups_set.
 *                  If not the meter_pointer is the object-stat-id
 * \param [in] total_nof_meters -nof meter used by this database. the cint will allocate each memory according to this number
 * \param [in] configure_meters -if true, the cint will configure all the meters with example configuration. if false, all meters use the driver default configuration (max rate, max burst).
 * \param [in] base_engine -the base engine that will be used for the database
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
policer_generic_meter_database_example(
    int unit,
    bcm_core_t core,
    int is_ingress,
    int database_id,
    int base_pointer,
    int single_bucket_mode,
    int expansion_enable,
    int total_nof_meters,
    int configure_meters,
    int base_engine)
{
    int rv;
    int i, flags = 0;
    int engine_id;
    const uint32 *ratio = dnxc_data_get(unit, "meter", "mem_mgmt", "counters_buckets_ratio_per_engine", NULL);
    const uint32 *ingress_special_engine =
        dnxc_data_get(unit, "meter", "mem_mgmt", "ingress_single_bucket_engine", NULL);
    const uint32 *egress_special_engine =
        dnxc_data_get(unit, "meter", "mem_mgmt", "egress_single_bucket_engine", NULL);
    const uint32 *counters_buckets_ratio_per_engine = dnxc_data_get(unit, "meter", "mem_mgmt", "counters_buckets_ratio_per_engine", NULL);
    uint32 *special_engine_size;
    uint32 *engine_size;
    bcm_policer_database_config_t database_config;
    int database_handle;
    bcm_policer_engine_t engine;
    bcm_policer_database_attach_config_t attach_config, attach_config_get;
    int nof_elements, engine_idx, engine_nof_meters;
    bcm_policer_config_t pol_cfg;
    int meter_idx, policer_id, max_nof_meters;

    bcm_policer_engine_t_init(&engine);
    bcm_policer_database_attach_config_t_init(&attach_config);
    bcm_policer_database_attach_config_t_init(&attach_config_get);
    bcm_policer_database_config_t_init(&database_config);
    printf
        ("policer_generic_meter_database_example called: unit=%d, core=%d, is_ingress=%d, database_id=%d, base_pointer=%d, single_bucket_mode=%d, expansion_enable=%d, total_nof_meters=%d, configure_meters=%d, base_engine=%d\n\n",
         unit, core, is_ingress, database_id, base_pointer, single_bucket_mode, expansion_enable, total_nof_meters,
         configure_meters, base_engine);
    /** step 1: create a database. if expansion_enable=TRUE,
        it is expected that the expansion groups were already configured using API bcm_policer_expansion_groups_set  */
    database_config.expansion_enable = expansion_enable;
    database_config.is_single_bucket = single_bucket_mode;

    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress, 0, core, database_id);

    rv = bcm_policer_database_create(unit, flags, database_handle, &database_config);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_database_create\n");
        return rv;
    }

    attach_config.policer_database_handle = database_handle;

    /** step 2: attach engines for the policer database.
        Use special engine only if all conditions to use it are fulfill. the conditions are:
        database_id=0 and single_bucket_mode=TRUE and total_nof_meters < small_engine_nof_meters and base_pointer=0 */
    if (*ingress_special_engine == base_engine || *egress_special_engine == base_engine)
    {
        special_engine_size =
            dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", *ingress_special_engine);
        engine_nof_meters = (*ratio) * (*special_engine_size);

        engine.engine_id = (is_ingress == TRUE) ? (*ingress_special_engine) : (*egress_special_engine);
        engine.core_id = core;

        attach_config.object_stat_pointer_base = 0;

        rv = bcm_policer_engine_database_attach(unit, flags, &engine, &attach_config);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_policer_engine_database_attach (special_engine) \n");
            return rv;
        }
    }
    /** use big engines that are general and can be used for all policers databases */
    else
    {
        /** as an example, use consecutive engine_id=base_engine,... (according to the total_nof_meters), assuming all engines in the same size */
        /** in this example, assuing that the user is using consecutive object_stat_ids for that policer database. */
        /** but it can also be not consecutive. each engine has its own object_stat_pointer_base */
        engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", base_engine);

        /** verify the given total_nof_meters given can be fitted  */
        max_nof_meters = (*dnxc_data_get(unit, "crps", "engine", "nof_big_engines", NULL)) * (*engine_size ) * (*counters_buckets_ratio_per_engine);
        if (total_nof_meters >max_nof_meters)
        {
            printf("Error in policer_generic_meter_database_example - total_nof_meters=%d is larger than can be fitted in the meter engines, possible max value=%d\n", total_nof_meters, max_nof_meters);
            return BCM_E_FAIL;
        }
        /** round up calculation to get nof_elements. element is a bank if the mode is single bucket and 2 banks if the mode is dual bucket */
        nof_elements = ((total_nof_meters + (*engine_size) - 1) / (*engine_size));
        for (i = 0; i < nof_elements; i++)
        {
            /** if it is single_bucekt_mode, each engine is used twice - one time with section low, and second time with section High */
            engine.engine_id = (single_bucket_mode == FALSE) ? (base_engine + i) : (base_engine + i / 2);
            /** if dual mode, section=ALL. if single mode, section=Low/High */
            engine.section =
                (single_bucket_mode ==
                 FALSE) ? bcmPolicerEngineSectionAll : ((i % 2 ==
                                                         0) ? bcmPolicerEngineSectionLow : bcmPolicerEngineSectionHigh);

            engine.core_id = core;
            attach_config.object_stat_pointer_base = base_pointer + i * (*engine_size);
            /** base is the first object-stat-id after expansion if enabled*/
            rv = bcm_policer_engine_database_attach(unit, flags, &engine, &attach_config);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_policer_engine_database_attach (engine=%d, section=%d) \n", engine.engine_id,
                       engine.section);
                return rv;
            }
        }
    }

    /** step 4: get back the configuration (from one core), just for validation - not mandatory  */
    rv = bcm_policer_engine_database_get(unit, flags, &engine, &attach_config_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_engine_database_get (engine=%d, section=%d) \n", engine.engine_id, engine.section);
        return rv;
    }
    /** verification  */
    if (BCM_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(attach_config_get.policer_database_handle) !=
        BCM_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(attach_config.policer_database_handle))
    {
        printf
            ("API bcm_policer_engine_database_get: missmatch between attach and get for parameter policer_database_handle{is_ingress}: attach=%d, get=%d (engine=%d, section=%d)\n",
             BCM_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(attach_config.policer_database_handle),
             BCM_POLICER_DATABASE_HANDLE_IS_INGRESS_GET(attach_config_get.policer_database_handle), engine.engine_id,
             engine.section);
        return BCM_E_FAIL;
    }
    /** verification  */
    if (BCM_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(attach_config.policer_database_handle) !=
        BCM_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(attach_config_get.policer_database_handle))
    {
        printf
            ("API bcm_policer_engine_database_get: missmatch between attach and get for parameter policer_database_handle{database_id}: attach=%d, get=%d (engine=%d, section=%d)\n",
             BCM_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(attach_config.policer_database_handle),
             BCM_POLICER_DATABASE_HANDLE_DATABASE_ID_GET(attach_config_get.policer_database_handle), engine.engine_id,
             engine.section);
        return BCM_E_FAIL;
    }
    /** verification  */
    if (attach_config_get.object_stat_pointer_base != attach_config.object_stat_pointer_base)
    {
        printf
            ("API bcm_policer_engine_database_get: missmatch between attach and get for parameter object_stat_pointer_base: attach=%d, get=%d (engine=%d, section=%d)\n",
             attach_config.object_stat_pointer_base, attach_config_get.object_stat_pointer_base, engine.engine_id,
             engine.section);
        return BCM_E_FAIL;
    }

    /** step 4: configure all the meters with the desired configuration for each one of them */
    /** in this example, all meters will be created with the same configuration. */
    /** in this example, the assumption is that the meter_idx(=meter_pointer) is consecutive. but it does not mandatory. (it depend on the engines configuration ). */
    /** meters that are not configured, use the defaul profile configuration (which contain max rate and burst).*/
    if (configure_meters == TRUE)
    {
        /** set the meter configuration for offset 0 */
        bcm_policer_config_t_init(&pol_cfg);

        pol_cfg.mode = bcmPolicerModeCommitted; /** global meter is always in committed mode (single bucket, one rate: drop or not) */
        pol_cfg.ckbits_sec = 80000; /** 80MB */
        pol_cfg.ckbits_burst = 1000;

        for (i = 0; i < total_nof_meters; i++)
        {
            meter_idx = base_pointer + i;
            /** set the policer id, using the macro for offset 0*/
            BCM_POLICER_ID_SET(policer_id, database_handle, meter_idx);
            rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error in bcm_policer_create (meter_idx=%d)\n", meter_idx);
                print rv;
                return rv;
            }

        }
    }
    /** step 5: enable the policer database*/
    rv = bcm_policer_database_enable_set(unit, flags, database_handle, TRUE);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_database_enable_set\n");
        return rv;
    }

    printf("meter database was created: database_handle=%d \n", database_handle);

    return BCM_E_NONE;
}

/**
 * \brief
 *  cint example to destory a policer database.
 *  destory database sequence:
 *      a. disable the database
 *      b. detach all the engines belong to this database.
 *      c. destory the database - it will also destory the entire meter profiles in the sw template manager.
 * \param [in] unit -unit id
 * \param [in] core -core id (All/0/1)
 * \param [in] is_ingress -refer to ingress or egress policer
 * \param [in] database_id -database_id (0..2 for ingress, 0..1 for egress). database id is actually the command id or the statistic_interface that is used.
 * \param [in] engine_id -engine id to detach. In this example, assuming the database contain one engine and one section type.
 *                  if database contain more engines/sections, need to detach all of them.
 * \param [in] section -section id to detach. In this example, assuming the database contain one engine and one section type.
 *                  if database contain more engines/sections, need to detach all of them.
 * \return
 *   error indication.
 * \remark
 *   NONE
 * \see
 *   NONE
 */
int
policer_generic_meter_database_destroy(
    int unit,
    bcm_core_t core,
    int is_ingress,
    int database_id,
    int engine_id,
    bcm_policer_engine_section_t section)
{
    int database_handle;
    int rv;
    int flags = 0;
    bcm_policer_engine_t engine;

    bcm_policer_engine_t_init(&engine);
    printf
        ("policer_generic_meter_database_destroy called: unit=%d, core=%d, is_ingress=%d, database_id=%d, engine_id=%d \n",
         unit, core, is_ingress, database_id, engine_id);

    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, is_ingress, 0, core, database_id);
    /** step 1: disable the policer database*/
    rv = bcm_policer_database_enable_set(unit, flags, database_handle, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_database_enable_set\n");
        return rv;
    }

    /** step 2: detach all engines sections belong to the database. */
    /** in this example, we assume that the database contain one engine and one section type (low/high/all). */
    engine.core_id = core;
    engine.engine_id = engine_id;
    engine.section = section;
    rv = bcm_policer_engine_database_detach(unit, flags, &engine);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_dnx_policer_engine_database_detach\n");
        return rv;
    }

    /** step 3: destroy the database, which destroy each one of the meters in that database */
    rv = bcm_policer_database_destroy(unit, flags, database_handle);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_dnx_policer_database_destroy\n");
        return rv;
    }
    return BCM_E_NONE;
}

/*
 * Extract core from input local port. On fail, return '-1'
 */
int
policer_core_from_port_get(
    int unit,
    int port,
    int *core)
{
    bcm_error_t rc;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_gport_t gport = port;

    rc = bcm_port_get(unit, gport, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_FAILURE(rc))
    {
        LOG_ERROR(BSL_LS_APPL_COMMON, (BSL_META_U(unit, "bcm_port_get failed. Error:%d (%s)\n"), rc, bcm_errmsg(rc)));
        return -1;
    }
    else
    {
        *core = mapping_info.core;
    }
    return 0;
}

/*
 * Brief/
 * create a policer configuration example
 */

int
policer_generic_create_example(
    int unit,
    int core_id,
    int database_handle,
    int meter_idx,
    int mode,
    int color_blind,
    uint32 ckbits_sec)
{
    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id;
    int rv;

    bcm_policer_config_t_init(&pol_cfg);

    if (color_blind)
    {
        pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    }
    pol_cfg.ckbits_sec = ckbits_sec;
    pol_cfg.ckbits_burst = 1000;
    pol_cfg.pkbits_burst = 1000;
    pol_cfg.max_pkbits_sec = 20000;     /* 20Mbps max */

    switch (mode)
    {
        case bcmPolicerModeSrTcm:
            pol_cfg.mode = bcmPolicerModeSrTcm; /* single rate three colors mode */
            /*
             * in this mode coupling is always enabled and the excess bucket has no credits of its own, it only
             * receives excess credits from the committed bucket, up to max_pkbits_sec
             */
            break;
        case bcmPolicerModeTrTcmDs:
            pol_cfg.mode = bcmPolicerModeTrTcmDs;       /* two rates three colors mode */
            pol_cfg.pkbits_sec = 20000; /* 20Mbps */
            break;
        case bcmPolicerModeCoupledTrTcmDs:
            pol_cfg.mode = bcmPolicerModeCoupledTrTcmDs;        /* two rates three colors in couple mode */
            pol_cfg.pkbits_sec = 20000; /* 20Mbps */
            pol_cfg.max_pkbits_sec = 30000;     /* 30Mbps max */
            break;
        case bcmPolicerModeCommitted:
            pol_cfg.mode = bcmPolicerModeCommitted;
            pol_cfg.pkbits_burst = 0;
            pol_cfg.max_pkbits_sec = 0;
            break;
        case bcmPolicerModeCascade:
            pol_cfg.mode = bcmPolicerModeCascade;
            pol_cfg.pkbits_sec = 30000;
            pol_cfg.max_ckbits_sec = 60000;     /* when sharing is enabled, max_ckbits_sec is also used */
            pol_cfg.max_pkbits_sec = 40000;
            pol_cfg.pkbits_burst = 6666;
            pol_cfg.ckbits_burst = 5555;
            break;
        case bcmPolicerModeCoupledCascade:
            pol_cfg.mode = bcmPolicerModeCoupledCascade;
            pol_cfg.pkbits_sec = 30000;
            pol_cfg.max_ckbits_sec = 60000;     /* when sharing is enabled, max_ckbits_sec is also used */
            pol_cfg.max_pkbits_sec = 40000;
            pol_cfg.pkbits_burst = 6666;
            pol_cfg.ckbits_burst = 5555;
            break;
        default:
            return BCM_E_PARAM;
            break;
    }

    pol_cfg.core_id = core_id;

    BCM_POLICER_ID_SET(policer_id, database_handle, meter_idx);

    rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id);
        print rv;
        return rv;
    }

    printf("The created policer_id is %d\n", policer_id);
    return rv;
}

/*
 * Brief/
 * create ingress acl rule which generate meter pointer on one database/interface id
 */
int
policer_ingress_meter_id_mapping_by_acl(
    int unit,
    int *database_id,
    int *meter_idx,
    int nof_meter,
    bcm_field_qualify_t qual_type,
    uint32 qual_value)
{
    bcm_gport_t local_gport;
    bcm_port_t port;
    bcm_field_group_info_t fg_info;
    bcm_field_group_attach_info_t attach_info;
    bcm_field_entry_info_t ent_info;
    bcm_field_group_t fg_id;
    bcm_field_entry_t fe_id;
    int action_info_pri;
    void *dest_char;
    int iqual_type;
    char group_name[sizeof(fg_info.name)];
    int idx,rv;
    int bcm_Field_Action_Stat_Id[] = {bcmFieldActionStatId0,
                                      bcmFieldActionStatId1,
                                      bcmFieldActionStatId2};

    int bcm_Field_Action_Stat_Profile[] = {bcmFieldActionStatProfile0,
                                           bcmFieldActionStatProfile1,
                                           bcmFieldActionStatProfile2};

    /** Create and attach first group */
    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    /* Set quals */
    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = qual_type;
    dest_char = &(fg_info.name[0]);
    iqual_type = qual_type;
    snprintf(group_name, sizeof(fg_info.name), "Meter_%d_%d", iqual_type,qual_value);
    sal_strncpy(dest_char, group_name, sizeof(fg_info.name));
    fg_info.name[sizeof(fg_info.name) - 1] = 0;

    /* Set actions */
    fg_info.nof_actions = 2 * nof_meter;
    for(idx=0; idx < nof_meter; idx++) {
        if (database_id[idx]>2) {
            printf("Invalid database_id %d\n",database_id[idx]);
            return BCM_E_PARAM;
        }
        fg_info.action_types[2 * idx] = bcm_Field_Action_Stat_Id[database_id[idx]];
        fg_info.action_types[2 * idx + 1] = bcm_Field_Action_Stat_Profile[database_id[idx]];
    }
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &fg_info, &fg_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_stage_t stage = bcmFieldStageIngressPMF1;
    bcm_field_context_info_t context_info;
    bcm_field_presel_entry_data_t presel_entry_data;
    bcm_field_presel_entry_id_t presel_entry_id;
    int presel_id = 10;
    bcm_field_context_t context_id = 10;

    /* Create the context 10, since the capacity of the default context Id 0 is not enough to create groups
     */
    bcm_field_context_info_t_init(&context_info);
    rv = bcm_field_context_info_get(unit, stage, context_id, &context_info);
    if (rv == BCM_E_NOT_FOUND) {
        stage = bcmFieldStageIngressPMF1;
        bcm_field_context_info_t_init(&context_info);
        bcm_field_presel_entry_data_t presel_entry_data;
        bcm_field_presel_entry_id_t presel_entry_id;
        rv = bcm_field_context_create(unit, BCM_FIELD_FLAG_WITH_ID, stage, &context_info, &context_id);

        /* Create presel_entry to map relevant traffic to the context */
        bcm_field_presel_entry_id_info_init(&presel_entry_id);
        bcm_field_presel_entry_data_info_init(&presel_entry_data);

        presel_entry_id.presel_id = presel_id;
        presel_entry_id.stage = stage;

        presel_entry_data.context_id = context_id;
        presel_entry_data.entry_valid = TRUE;
        presel_entry_data.nof_qualifiers = 1;

        presel_entry_data.qual_data[0].qual_type = bcmFieldQualifyVlanFormat;
        presel_entry_data.qual_data[0].qual_value = BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
        presel_entry_data.qual_data[0].qual_mask = 0;
        presel_entry_data.qual_data[0].qual_arg = 0;
        rv = bcm_field_presel_set(unit, 0, &presel_entry_id, &presel_entry_data);
    }
    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;
    attach_info.key_info.qual_types[0] = fg_info.qual_types[0];
    /*In latest SDK, if the different groups have the same action, it will fail.
     *Here make different groups have different priority
     */
    action_info_pri = BCM_FIELD_ACTION_PRIORITY(0,fg_id % 16);
    for(idx=0; idx < nof_meter; idx++) {
        attach_info.payload_info.action_types[2 * idx] = fg_info.action_types[2 * idx];
        attach_info.payload_info.action_info[2 * idx].priority = action_info_pri;
        attach_info.payload_info.action_types[2 * idx + 1] = fg_info.action_types[2 * idx + 1];
        attach_info.payload_info.action_info[2 * idx + 1].priority = action_info_pri;
    }
    if (qual_type==bcmFieldQualifyVlanId) {
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
        attach_info.key_info.qual_info[0].input_arg = 0;
        attach_info.key_info.qual_info[0].offset = 96;
    } else {
        attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    }
    /* Change to DNX_FIELD_CONTEXT_DEFAULT_ID */
    rv = bcm_field_group_context_attach(unit, 0, fg_id, context_id, &attach_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }


    /* Add entry */
    if (qual_type==bcmFieldQualifyInPort) {
        /*Convert to gport*/
        port = qual_value;
        BCM_GPORT_LOCAL_SET(&local_gport, port);
        qual_value = local_gport;
    }

    bcm_field_entry_info_t_init(&ent_info);
    ent_info.priority = 1;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = qual_value;
    switch(qual_type)
    {
        case bcmFieldQualifyInPort:
            ent_info.entry_qual[0].mask[0] = 0x1FF;
        break;
        case bcmFieldQualifyDstPort:
            ent_info.entry_qual[0].mask[0] = 0xFF;
        break;
        case bcmFieldQualifyIntPriority:
            ent_info.entry_qual[0].mask[0] = 0x7;
        break;
        case bcmFieldQualifyVlanId:
            ent_info.entry_qual[0].mask[0] = 0xFFF;
        break;
        default:
        printf("Error: qual_type=%d is not supported for this cint\n", qual_type);
        return _SHR_E_INTERNAL;
        break;
    }

    ent_info.nof_entry_actions = 2 * nof_meter;
    for(idx=0; idx < nof_meter; idx++) {
        ent_info.entry_action[2 * idx].type = fg_info.action_types[2 * idx];
        ent_info.entry_action[2 * idx].value[0] = meter_idx[idx];
        ent_info.entry_action[2 * idx + 1].type = fg_info.action_types[2 * idx + 1];
        ent_info.entry_action[2 * idx + 1].value[0] = 1;
        ent_info.entry_action[2 * idx + 1].value[1] = 0;
        ent_info.entry_action[2 * idx + 1].value[2] = 0;
        ent_info.entry_action[2 * idx + 1].value[3] = 1;
    }
    rv = bcm_field_entry_add(unit, 0, fg_id, &ent_info, &fe_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }
    for(idx=0; idx < nof_meter; idx++) {
        printf("Entry add: id:(0x%x) database_id:(%d) meter_idx:(%d)\n",
                fg_id ,database_id[idx], meter_idx[idx]);
    }
    return rv;
}


/**
* \brief
*  Creates an ACE format and ACE entry and an ePMF field group that points to the ACE entry.
*  Generate stat-id for ETPP
* \param [in] unit             - Device ID
* \param [in] stat_id        - stat_id to be set
* \param [in] profile        - profile (PP stat profile)
* \return
*   int - Error Type
* \remark
*   * None
* \see
*   * None
*/
int policer_etpp_ace_generate_stat_id(
    int unit,
    int out_port,
    int stat_id,
    int profile)
{
    bcm_field_ace_format_info_t ace_format_info;
    bcm_field_ace_entry_info_t ace_entry_info;
    bcm_field_group_info_t epmf_fg_info;
    bcm_field_group_attach_info_t epmf_attach_info;
    bcm_field_entry_info_t epmf_entry_info;
    int flags = 0;
    int rv = BCM_E_NONE;
    int i = 0;
    bcm_field_ace_format_t ace_format_id;
    uint32 ace_entry_handle;
    bcm_field_group_t epmf_fg_id;
    bcm_field_entry_t epmf_entry_handle;
    bcm_gport_t gport;

    bcm_field_ace_format_info_t_init(&ace_format_info);

    ace_format_info.nof_actions = 1;

    ace_format_info.action_types[0] = bcmFieldActionStat0; /**meter*/

    rv = bcm_field_ace_format_add(unit, 0, &ace_format_info, &ace_format_id);
    if(rv != BCM_E_NONE)
   {
        printf("Error (%d), in bcm_field_ace_format_add\n", rv);
        return rv;
   }

    bcm_field_ace_entry_info_t_init(&ace_entry_info);

    ace_entry_info.nof_entry_actions = ace_format_info.nof_actions;

    ace_entry_info.entry_action[0].type = bcmFieldActionStat0;
    /** The value of the ACE entry is {b'20 stat_id, b'5 CMD} valid is set to 1 as we want to count */
    ace_entry_info.entry_action[0].value[0] = profile & 0x1F;
    ace_entry_info.entry_action[0].value[1] = stat_id;

    rv = bcm_field_ace_entry_add(unit, 0, ace_format_id, &ace_entry_info, &ace_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_ace_entry_add\n", rv);
        return rv;
    }

    /*
     * Create a field group in ePMF that uses the ACE entry.
     */

    bcm_field_group_info_t_init(&epmf_fg_info);
    epmf_fg_info.fg_type = bcmFieldGroupTypeTcam;
    epmf_fg_info.nof_quals = 1;
    epmf_fg_info.stage = bcmFieldStageEgress;
    epmf_fg_info.qual_types[0] = bcmFieldQualifyOutPort;
    epmf_fg_info.nof_actions = 1;
    /* The first action decides the ACE entry that will happen. */
    epmf_fg_info.action_types[0] = bcmFieldActionAceEntryId;
    rv = bcm_field_group_add(unit, BCM_FIELD_FLAG_MSB_RESULT_ALIGN, &epmf_fg_info, &epmf_fg_id);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_add\n", rv);
        return rv;
    }

    bcm_field_entry_info_t_init(&epmf_entry_info);
    epmf_entry_info.nof_entry_quals = epmf_fg_info.nof_quals;
    epmf_entry_info.nof_entry_actions = epmf_fg_info.nof_actions;
    for(i=0; i< epmf_fg_info.nof_quals; i++)
    {
        epmf_entry_info.entry_qual[i].type = epmf_fg_info.qual_types[i];
    }
    for(i=0; i<epmf_fg_info.nof_actions; i++)
    {
        epmf_entry_info.entry_action[i].type = epmf_fg_info.action_types[i];
    }
    BCM_GPORT_LOCAL_SET(gport, out_port);
    epmf_entry_info.entry_qual[0].value[0] = gport;
    epmf_entry_info.entry_qual[0].mask[0] = 0;
    epmf_entry_info.entry_action[0].value[0] = ace_entry_handle;

    rv = bcm_field_entry_add(unit, 0, epmf_fg_id, &epmf_entry_info, &epmf_entry_handle);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_add\n", rv);
        return rv;
    }

    bcm_field_group_attach_info_t_init(&epmf_attach_info);
    epmf_attach_info.key_info.nof_quals = epmf_fg_info.nof_quals;
    epmf_attach_info.payload_info.nof_actions = epmf_fg_info.nof_actions;
    for(i=0; i< epmf_fg_info.nof_quals; i++)
    {
        epmf_attach_info.key_info.qual_types[i] = epmf_fg_info.qual_types[i];
    }
    for(i=0; i<epmf_fg_info.nof_actions; i++)
    {
        epmf_attach_info.payload_info.action_types[i] = epmf_fg_info.action_types[i];
    }
    epmf_attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeMetaData;
    epmf_attach_info.key_info.qual_info[0].input_arg = 0;
    epmf_attach_info.key_info.qual_info[0].offset = 0;

    rv = bcm_field_group_context_attach(unit, 0, epmf_fg_id, BCM_FIELD_CONTEXT_ID_DEFAULT, &epmf_attach_info);
    if(rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_context_attach\n", rv);
        return rv;
    }

    printf("ACE format (%d), ACE entry (%d) and ePMF FG (%d) created.\n",
           ace_format_id, ace_entry_handle, epmf_fg_id);
    return rv;

}


/*
 * \brief
 *  Traffic cint example, which create one meter database.
 * run:
 * cint cint_coq_l2_phb
 * cint policer_egress_generic_two_meter_per_packet_example
 *
 * traffic to run:
 * run ethernet packet with DA 1 and vlan tag id 100 (tag protocol id 0x8100) from in_port, with:
 *
 *
 * mode of single_rate - meter mode is bcmPolicerModeSrTcm (single rate 3 colors), the excess bucket has no credits of its own and
 *                    it only receives excess credits from the committed bucket
 * mode of two_rate - meter mode is bcmPolicerModeTrTcmDs (2 rates 3 colors), the committed bucket and the excess bucket
 *                    each has credits of its own
 * 1) priority = 1:
 *     the stream will go through the committed bucket and afterwards also through the excess bucket
 *     if two_rate: the stream will arrive at out_port with 50M rate (30M CIR + 20M EIR).
 *     if single_rate: the stream will arrive at out_port with 30M rate (EIR has no credits).
 *
 * 2) priority = 4:
 *     if color_blind=0: the stream will go straight to the excess bucket and arrive at out_port with 20M rate
 *                       in both cases (single_rate/two_rate).
 *     if color_blind=1: the packet will go to the CIR and will behave the same as priority=1.
 *
 *  Note: function use service from cint cint_qos_l2_phb.c - please load it first
 *  Note: function use service from cint cint_port_misc_configuration.c - please load it first
 */
int
policer_ingress_generic_traffic_example(
    int unit,
    int in_port,
    int out_port,
    int mode,
    int color_blind)
{
    int rv;
    int inPortValue = in_port;  /* incoming port */
    bcm_field_stat_t stats[2];
    int core_id;                /* core matching the in_port */
    int meter_idx;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x1 };
    int pkt_pri[2] = {1,4};
    int pkt_cfi[2] = {0,0};
    int tc[2] = {0,0};
    int dp[2] = {bcmColorGreen, bcmColorYellow};
    int database_handle;
    int database_id = 1;
    char msg1[100];
    uint32 max_nof_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_engines", NULL));
    uint32 nof_big_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_big_engines", NULL));
    uint32 first_big_engine_id  = max_nof_engines - nof_big_engines;
    /** meter should be allocated in the core in which the ports are allocated */
    rv = policer_core_from_port_get(unit, inPortValue, &core_id);
    if (rv != BCM_E_NONE)
    {
        print rv;
        return rv;
    }

    rv = policer_generic_meter_database_example(unit, core_id, TRUE, database_id, 0 /** base_pointer */ , FALSE    /** single_bucket_mode
                                                                                                                         */ ,
                                                FALSE /* expansion_enable */ , 1024 /* total_nof_meters */ , FALSE      /* configure_meters
                                                 */ , (first_big_engine_id + 3) /* base_engine */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error in policer_generic_meter_database_example\n");
        return rv;
    }
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, TRUE, 0, core_id, database_id);
    if (mode == bcmPolicerModeCascade || mode == bcmPolicerModeCoupledCascade)
    {
        meter_idx = 32;
    }
    else
    {
        meter_idx = 30;
    }
    /* create a single meter */
    rv = policer_generic_create_example(unit, core_id, database_handle, meter_idx, mode, color_blind, 30000);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in policer_generic_create_example \n", rv);
        return rv;
    }

    /** map packet to meter_id, according to in-port */
    rv = policer_ingress_meter_id_mapping_by_acl(unit, &database_id, &meter_idx, 1, bcmFieldQualifyInPort, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in policer_ingress_meter_id_mapping_by_acl\n", rv);
        return rv;
    }
    /** send traffic with DA=1 and Vlan tag id 100 to out_port */
    bcm_l2_addr_t_init(&l2addr, mac, 100);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add with vid 1\n");
        print rv;
        return rv;
    }

    /*
     * map PKT_PRI/CFI into INTERNAL TC/DP in the ingress:
     *           1/0   into           0/GREEN
     *           4/0   into           0/YELLOW
     */
    /*
     * The packet with priority 1 will be marked as green and packets with proiority 4 is marked as yellow.
     * if meter mode is not COLOR_BLIND and packet is yellow, the packet will go straight to the excess bucket
     * (without going through the committed bucket)
     * packet that will arrive with a green color will go through the committed bucket first.
 */
    rv = policer_generic_qos_ingress_port_set(unit, in_port, 100, 2, pkt_pri /* pkt_pri */, pkt_cfi /* cfi */, tc /* tc */, dp /* dp */);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_qos_map_create\n");
        return rv;
    }

    /*
     * set discard DP to drop all packets with DP = 2 or 3
     */
    rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }

    /** in case the in_port=out_port, need to disable filtering */
    if(in_port == out_port)
    {
        disable_same_if_filter_all(unit, out_port);
        if (rv != BCM_E_NONE) {
            printf("Error, disable_same_if_filter_all\n");
            return rv;
        }
    }

    return rv;
}


/*
 * \brief
 *  traffic example for egress meter
 * run:
 * cint cint_policer_generic_meter.c
 *
 *  configure two ingress/egress meter database and create color decision based on two meters which configured in SERIAL or PARALLEL mode.
 *  run traffic with DA=1 and Vlan tag id 100 and pkt_prio=1,cfi=0 at 1G rate,
 *  together with traffic with DA=1 and Vlan tag id 100 and pkt_prio=4,cfi=0 at 1G rate:
 *      both streams will share the same meter in database=1 and have different meters in database=0.
 *      meter in database=1 has a 27M rate and the 2 meters in database=0 both have 25M rate
 *      (one is single rate of 25M and the other is 2 rates of 20M + 5M).
 *
 * Expected results:
 * if mode is SERIAL: database_id=0 meter will return a 25M+20M+5M rate and database_id=1 meter will lower this rate to 27M.
 * if mode is PARALLEL: the worst case of the 2 meters (which is 27M) will be the result.
 * It is assume that in serial mode the order of the meters are: database-0 => database-1;
 */
int
policer_generic_two_meter_per_packet_example(
    int unit,
    int in_port,
    int out_port,
    int is_ingress)
{

    int rv;
    bcm_policer_config_t pol_cfg;
    bcm_policer_t policer_id;
    bcm_policer_config_t pol_cfg_share;
    bcm_policer_t policer_id_share;
    int meter_idx_share, meter_idx,database_id;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac_1 = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x1 };
    int core_id;
    int database_handle0, database_handle1;
    int base_pointer0 = 0, base_pointer1 = 32 * 1024;
    int pkt_pri[2] = {1,4};
    int pkt_cfi[2] = {0,0};
    int tc[2] = {0,1};
    int dp[2] = {bcmColorGreen, bcmColorYellow};
    int egress_pp_stat_profile;
    bcm_stat_pp_profile_info_t egress_stat_pp_profile_info;
    uint32 max_nof_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_engines", NULL));
    uint32 nof_big_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_big_engines", NULL));
    uint32 first_big_engine_id  = max_nof_engines - nof_big_engines;

    /*
     * meters should be allocated in the core in which the ports are allocated
     */
    rv = policer_core_from_port_get(unit, out_port, &core_id);
    if (rv != BCM_E_NONE)
    {
        print rv;
        return rv;
    }
    /** create ingress/egress policer database # 0 */
    rv = policer_generic_meter_database_example(unit, core_id, is_ingress, 0 /* database_id */ ,
        base_pointer0  /* base_pointer */ , FALSE /* single_bucket_mode */ ,FALSE /* expansion_enable */ ,
        1024 /* total_nof_meters */ , FALSE     /* configure_meters */ , (first_big_engine_id + 1) /* base_engine */ );
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle0, is_ingress, 0, core_id, 0);

    /** create ingress/egress policer database # 1 */
    rv = policer_generic_meter_database_example(unit, core_id, is_ingress , 1 /* database_id */ ,
                    base_pointer1  /* base_pointer */ , FALSE /* single_bucket_mode */ , FALSE /* expansion_enable */ ,
                     1024 /* total_nof_meters */ , FALSE     /* configure_meters */ , (first_big_engine_id + 2) /* base_engine */ );
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle1, is_ingress, 0, core_id, 1);

    /*
     * create COLOR_BLIND meter from database_id=1 (child in serial mode) with CIR = 25M
     */
    bcm_policer_config_t_init(&pol_cfg_share);
    pol_cfg_share.flags |= BCM_POLICER_COLOR_BLIND;
    pol_cfg_share.mode = bcmPolicerModeCommitted;
    pol_cfg_share.ckbits_sec = 27000;   /* 27M */
    pol_cfg_share.ckbits_burst = 3000;
    pol_cfg_share.pkbits_burst = 0;
    pol_cfg_share.max_pkbits_sec = 15000;
    pol_cfg_share.core_id = core_id;
    meter_idx_share = 8 + base_pointer1;


    BCM_POLICER_ID_SET(policer_id_share, database_handle1, meter_idx_share);
    rv = bcm_policer_create(unit, &pol_cfg_share, &policer_id_share);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id_share);
        print rv;
        return rv;
    }

    /** create COLOR_BLIND meter from database=0, with CIR = 5M and EIR = 20M */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeTrTcmDs;
    pol_cfg.ckbits_sec = 5000;  /* 5M */
    pol_cfg.ckbits_burst = 3000;
    pol_cfg.pkbits_sec = 20000; /* 20M */
    pol_cfg.pkbits_burst = 5000;
    pol_cfg.max_pkbits_sec = 25000;
    pol_cfg.core_id = core_id;
    meter_idx = 8; /** for egress meter + TC-Qos mapping, meter_idx 8 fits to TC=0 base on the qos configuration */
    BCM_POLICER_ID_SET(policer_id, database_handle0, meter_idx);

    rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id);
        print rv;
        return rv;
    }

    /** map traffic with vlan=100 to meter-idx=8 in database=0 and to meter-id=10 in database=1 using PMF */
    if(is_ingress)
    {
        /** map packet to meter_id, according to port-id (relevant for both streams ) */
        database_id=1;
        rv = policer_ingress_meter_id_mapping_by_acl(unit, &database_id, &meter_idx_share, 1, bcmFieldQualifyInPort, in_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in policer_ingress_meter_id_mapping_by_acl\n", rv);
            return rv;
        }

        /** map packet to meter_id, according to tc */
        database_id=0;
        rv = policer_ingress_meter_id_mapping_by_acl(unit, &database_id, &meter_idx,1, bcmFieldQualifyIntPriority, tc[0]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in policer_ingress_meter_id_mapping_by_acl\n", rv);
            return rv;
        }
    }
    else
    {
        /* set egress meter pointer for database-1 using E-PMF ace */
        bcm_stat_pp_profile_info_t_init(&egress_stat_pp_profile_info);
        egress_stat_pp_profile_info.meter_command_id = 1;
        egress_stat_pp_profile_info.is_meter_enable = 1;
        rv = bcm_stat_pp_profile_create(unit, 0, bcmStatCounterInterfaceEgressTransmitPp,
            &egress_pp_stat_profile, &egress_stat_pp_profile_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in bcm_stat_pp_profile_create\n", rv);
            return rv;
        }
        rv = policer_etpp_ace_generate_stat_id(unit, out_port, meter_idx_share, egress_pp_stat_profile);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in policer_etpp_ace_generate_stat_id\n", rv);
            return rv;
        }

        /** map traffic with vlan=100 to meter database-0 in group=0 and to the a meter in group=1 */
        rv = policer_egress_pointer_generation(unit, core_id, meter_idx, 0, out_port, 100);
        if (rv != BCM_E_NONE)
        {
            printf("Error in policer_egress_pointer_generation \n");
            print rv;
            return rv;
        }
    }

    /** create COLOR_BLIND meter from database=0 with CIR = 25M (single rate) */
    /** may be used by egress meter when doing QOS-TC expansion */
    bcm_policer_config_t_init(&pol_cfg);
    pol_cfg.flags |= BCM_POLICER_COLOR_BLIND;
    pol_cfg.mode = bcmPolicerModeSrTcm;
    pol_cfg.ckbits_sec = 25000;
    pol_cfg.ckbits_burst = 3000;
    pol_cfg.max_pkbits_sec = 25000;
    pol_cfg.core_id = core_id;
    meter_idx = meter_idx + tc[1]; /** for egress meter + TC-Qos mapping, meter_idx 9 fits to TC=1 base on the qos configuration */
    BCM_POLICER_ID_SET(policer_id, database_handle0, meter_idx);

    rv = bcm_policer_create(unit, &pol_cfg, &policer_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_create with policer_id %d\n", policer_id);
        print rv;
        return rv;
    }

    /** create COLOR_BLIND meter from database=1 with CIR = 33M (single rate) */
    /** may be used by egress meter when doing QOS-TC expansion */    
    bcm_policer_config_t_init(&pol_cfg_share);
    pol_cfg_share.flags |= BCM_POLICER_COLOR_BLIND;
    pol_cfg_share.mode = bcmPolicerModeCommitted;
    pol_cfg_share.ckbits_sec = 33000;   /* 33M */
    pol_cfg_share.ckbits_burst = 3000;
    pol_cfg_share.pkbits_burst = 0;
    pol_cfg_share.max_pkbits_sec = 15000;
    pol_cfg_share.core_id = core_id;
    meter_idx_share = meter_idx_share + tc[1];


    BCM_POLICER_ID_SET(policer_id_share, database_handle1, meter_idx_share);
    rv = bcm_policer_create(unit, &pol_cfg_share, &policer_id_share);

    if(is_ingress)
    {
        /** map packet to meter_id, according to tc */
        database_id=0;
        rv = policer_ingress_meter_id_mapping_by_acl(unit, &database_id, &meter_idx, 1, bcmFieldQualifyIntPriority, tc[1]);
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in policer_ingress_meter_id_mapping_by_acl\n", rv);
            return rv;
        }
    }


    /*
     * map PKT_PRI/CFI into INTERNAL TC/DP in the ingress:
     *           1/0   into           0/GREEN
     *           4/0   into           0/YELLOW
     */
    /*
     * The packet with priority 1 will be marked as green and packets with proiority 4 is marked as yellow.
     * if meter mode is not COLOR_BLIND and packet is yellow, the packet will go straight to the excess bucket
     * (without going through the committed bucket)
     * packet that will arrive with a green color will go through the committed bucket first.
 */
    rv = policer_generic_qos_ingress_port_set(unit, in_port, 100, 2, pkt_pri /* pkt_pri */, pkt_cfi /* cfi */, tc /* tc */, dp /* dp */);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_qos_map_create\n");
        return rv;
    }

    /** send all traffic with DA=1 and one of the above Vlans to out_port */
    bcm_l2_addr_t_init(&l2addr, mac_1, 100);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add with vid 100\n");
        print rv;
        return rv;
    }

    if(is_ingress)
    {
        /** set discard DP to drop all packets with DP = 3 */
        rv = bcm_cosq_discard_set(unit,
            (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED));
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_cosq_discard_set\n");
            print rv;
        }
    }
    else
    {
        /** set discard DP to drop all packets with DP = 2 and 3 */
        rv = policer_egress_drop_packet(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error, policer_egress_drop_packet\n");
            print rv;
        }
    }
    /** in case the in_port=out_port, need to disable filtering */
    if(in_port == out_port)
    {
        disable_same_if_filter_all(unit, out_port);
        if (rv != BCM_E_NONE) {
            printf("Error, disable_same_if_filter_all\n");
            return rv;
        }
    }
    return rv;
}

/* Return <0 on error, and the entropy_id of policer on success. */
/* This is for use in the DVAPI. */
int
policer_generic_entropy_id_get(
    int unit,
    bcm_policer_t policer)
{
    bcm_policer_config_t cfg;
    int rv = BCM_E_NONE;

    bcm_policer_config_t_init(&cfg);
    cfg.flags |= BCM_POLICER_REPLACE_SHARED;

    bcm_policer_get(unit, policer, &cfg);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_get for same_prf_1_policer_id \n");
        print rv;
        return rv;
    }

    return cfg.entropy_id;
}

/**
* \brief
*  configure egress pp trap to drop a packet which was marked by egress meter processor as Red or Black.
*/
int
policer_egress_drop_packet(
    int unit)
{
    int rv = BCM_E_NONE;
    int trap_id = BCM_RX_TRAP_EG_TX_TRAP_ID_DROP;
    bcm_rx_trap_config_t config = { 0 };
    bcm_gport_t gport;

    /** 1. set egress discard (drop) enable per DP*/
    /* set discard DP to drop all packets with DP = 3 */
    rv = bcm_cosq_discard_set(unit,
        BCM_COSQ_DISCARD_EGRESS | BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }

    BCM_GPORT_TRAP_SET(gport, trap_id, 15, 0);
    rv = bcm_rx_trap_action_profile_set(unit, 0, bcmRxTrapEgTxMetering, gport);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_rx_trap_action_profile_set\n");
        return rv;
    }

    return rv;
}

/**
* \brief
*  generate egress meter pointer in the etpp.
*  meter_pointer=base_meter_ptr+Qos_offset.
*  Qos_offset is mapping of {Qos_meter_profile,tc}
*/
int
policer_egress_pointer_generation(
    int unit,
    int core_id,
    int base_meter_ptr,
    int database_id,
    int out_port,
    int vlan)
{
    int rv = BCM_E_NONE;
    uint32 flags;
    int map_id;
    int tc_index;
    bcm_qos_map_t map_entry;
    int egress_pp_stat_profile;
    bcm_stat_pp_profile_info_t egress_stat_pp_profile_info;
    bcm_vlan_port_t out_vlan_port;
    bcm_stat_pp_info_t stat_info;

    /** 1. Configure Qos meter offset */
    flags = BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_POLICER;
    bcm_qos_map_t_init(&map_entry);

    rv = bcm_qos_map_create(unit, flags, &map_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_qos_map_create\n");
        return rv;
    }
    /** Map each tc to different offset */
    flags = BCM_QOS_MAP_POLICER;
    for(tc_index = 0; tc_index < 8; tc_index++)
    {
        map_entry.int_pri = tc_index;
        map_entry.policer_offset = tc_index;
        rv =  bcm_qos_map_add(unit, flags, &map_entry, map_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error in bcm_qos_map_add\n");
            return rv;
        }
    }

    /** 2. generate a PP stat profile which contain the Qos_map_id */
    bcm_stat_pp_profile_info_t_init(&egress_stat_pp_profile_info);
    egress_stat_pp_profile_info.meter_command_id = database_id;
    egress_stat_pp_profile_info.is_meter_enable = 1;
    egress_stat_pp_profile_info.meter_qos_map_id = map_id;
    rv = bcm_stat_pp_profile_create(unit, 0, bcmStatCounterInterfaceEgressTransmitPp,
        &egress_pp_stat_profile, &egress_stat_pp_profile_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_stat_pp_profile_create\n", rv);
        return rv;
    }
    printf("stat_pp_profile was create:  profile_id=%d\n", egress_pp_stat_profile);
   /** 3. create ETPP object with the PP profile that was created and base_meter_ptr
            (base_meter_ptr jump in 8 to allow unique pointer when adding the Qos policer offset) */
    /** first, create AC OUT-LIF */
    bcm_vlan_port_t_init(&out_vlan_port);
    out_vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_STAT_EGRESS_ENABLE;
    out_vlan_port.criteria    = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_vlan_port.match_vlan  = 0;
    /** vsi is the vid in bcm_l2_addr_t_init, it doesn't have to be the same as the incoming vlan on the packet */
    out_vlan_port.vsi  = vlan;
    out_vlan_port.port = out_port;
    rv = bcm_vlan_port_create(unit, &out_vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    /** connect the gport to the stat-id */
    stat_info.flags = 0;
    stat_info.stat_id = base_meter_ptr;
    stat_info.stat_pp_profile = egress_pp_stat_profile;
    rv = bcm_gport_stat_set(unit, out_vlan_port.vlan_port_id, core_id, bcmStatCounterInterfaceEgressTransmitPp, stat_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in cint_stat_pp_create_gport_stat_object\n");
        return rv;
    }

    printf("map gport=%d core_id=%d to ETPP stat_profile=%d, stat_id=%d\n", out_vlan_port.vlan_port_id, core_id,
    stat_info.stat_pp_profile, stat_info.stat_id);
    
    return rv;
}






/*
 * \brief
 *  Traffic cint example, which create 3 meter databases.
 * run:
 * cint cint_policer_generic_meter.c
 *
 * traffic to run:
 * run ethernet packet with DA 1 and vlan tag id 100 from in_port
 *
 * expected meter flow for serial mode is:
 *
 *
 *          +----------------+       +--------------------+        +------------------+
 *          |single bucket   |       |dual bucket-cascade |        |dual bucket       |
 *          |color aware     +-----> |50M green           +------> |60M green         |
 *          |10M green       |       |30M yellow          |        |20M yellow        |
 *          +----------------+       +--------------------+        +------------------+
 *  in_port,vlan=100  stream  100M green                   50M green                   50M green
 *                                                        30M yellow                  20M yellow
 *  ------------------------>        +----------------->        +------------------>
 */
int
policer_ingress_multiple_database_example(
    int unit,
    int in_port,
    int out_port)
{
    int rv;
    int inPortValue = in_port;  /* incoming port */
    bcm_field_stat_t stats[2];
    int core_id;                /* core matching the in_port */
    int meter_idx,database_id;
    bcm_l2_addr_t l2addr;
    bcm_mac_t mac = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x1 };
    int pkt_pri[2] = {1,4};
    int pkt_cfi[2] = {0,0};
    int tc[2] = {1,1};
    int dp[2] = {bcmColorGreen, bcmColorYellow};
    int database_handle0, database_handle1, database_handle2;
    uint32 *engine_size;
    uint32 max_nof_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_engines", NULL));
    uint32 nof_big_engines = *(dnxc_data_get(unit, "crps", "engine", "nof_big_engines", NULL));
    uint32 first_big_engine_id  = max_nof_engines - nof_big_engines;

    /** meter should be allocated in the core in which the ports are allocated */
    rv = policer_core_from_port_get(unit, inPortValue, &core_id);
    if (rv != BCM_E_NONE)
    {
        print rv;
        return rv;
    }

    /** create 3 databases */
    /** first database is "Flood containment"  based on the In-port (just for the example) */
    /** meter were all configured for 50M commitee single bucket */
    rv = policer_generic_meter_database_example(unit, core_id, TRUE /** is ingress */, 0 /* database_id */,
            0 /** base_pointer */, TRUE    /** single_bucket_mode*/ , FALSE /* expansion_enable */ ,
            1280 /* (port*fwd-type) total_nof_meters */ , TRUE   /* configure_meters */ , 0 /* base_engine */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error in policer_generic_meter_database_example - database0\n");
        return rv;
    }

    /** second database */
    rv = policer_generic_meter_database_example(unit, core_id, TRUE /** is ingress */, 1 /* database_id */,
            0 /** base_pointer */, FALSE    /** single_bucket_mode*/ , FALSE /* expansion_enable */ ,
            1024 /* total_nof_meters */ , FALSE      /* configure_meters */ , first_big_engine_id /* base_engine */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error in policer_generic_meter_database_example - database1\n");
        return rv;
    }

    /** create third database - requires 2 engines (just as an example)*/
    engine_size = dnxc_data_1d_get(unit, "crps", "engine", "engines_info", "nof_counters", first_big_engine_id);
    rv = policer_generic_meter_database_example(unit, core_id, TRUE /** is ingress */, 2 /* database_id */,
            0 /** base_pointer */, FALSE    /** single_bucket_mode*/ , FALSE /* expansion_enable */ ,
            (*engine_size*2)-1 /* total_nof_meters */ , FALSE      /* configure_meters */ , (first_big_engine_id + 2) /* base_engine */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error in policer_generic_meter_database_example - database2\n");
        return rv;
    }


    /* configure the meter of database-0 */
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle0, TRUE, 0, core_id, 0 /* database_id */);
    meter_idx = in_port; /* use the port as meter_idx */
    /* Note: meter config for database-0 was made in the database create function: 50M bcmPolicerModeCommitted mode*/
    /** map packet to meter_id, according to in-port */
    database_id=0;
    rv = policer_ingress_meter_id_mapping_by_acl(unit, &database_id, &meter_idx, 1, bcmFieldQualifyInPort, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in policer_ingress_meter_id_mapping_by_acl\n", rv);
        return rv;
    }


    /* configure the meter of dtatabase-1 */
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle1, TRUE, 0, core_id, 1 /* database_id */);
    meter_idx = 32;
    /* create a single meter */
    rv = policer_generic_create_example(unit, core_id, database_handle1, meter_idx, bcmPolicerModeCascade, 0, 50000);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in policer_generic_create_example \n", rv);
        return rv;
    }
    /** map packet to meter_id, according to vlan-id */
    database_id=1;
    rv = policer_ingress_meter_id_mapping_by_acl(unit, &database_id, &meter_idx, 1, bcmFieldQualifyIntPriority, tc[0]);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in policer_ingress_meter_id_mapping_by_acl\n", rv);
        return rv;
    }


    /* configure the meter of database-2 */
    BCM_POLICER_DATABASE_HANDLE_SET(database_handle2, TRUE, 0, core_id, 2 /* database_id */);
    meter_idx = (*engine_size)*2 - 1; /* use the last meter_idx of the database */

    /* create a single meter */
    rv = policer_generic_create_example(unit, core_id, database_handle2, meter_idx, bcmPolicerModeTrTcmDs, 0, 60000);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in policer_generic_create_example \n", rv);
        return rv;
    }
    /** map packet to meter_id, according to dest-port */
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info ;
	bcm_gport_t gport = out_port;
    rv = bcm_port_get(unit, gport, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_port_get\n", rv);
        return rv;
    }
    database_id=2;
    rv = policer_ingress_meter_id_mapping_by_acl(unit, &database_id, &meter_idx, 1, bcmFieldQualifyDstPort, mapping_info.tm_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in policer_ingress_meter_id_mapping_by_acl\n", rv);
        return rv;
    }



    /** send traffic with DA=1 and Vlan tag id 100 to out_port */
    bcm_l2_addr_t_init(&l2addr, mac, 100);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add with vid 1\n");
        print rv;
        return rv;
    }

    /*
     * map PKT_PRI/CFI into INTERNAL TC/DP in the ingress:
     *           1/0   into           0/GREEN
     *           4/0   into           0/YELLOW
     */
    /*
     * The packet with priority 1 will be marked as green and packets with proiority 4 is marked as yellow.
     * if meter mode is not COLOR_BLIND and packet is yellow, the packet will go straight to the excess bucket
     * (without going through the committed bucket)
     * packet that will arrive with a green color will go through the committed bucket first.
 */
    rv = policer_generic_qos_ingress_port_set(unit, in_port, 100, 2, pkt_pri /* pkt_pri */, pkt_cfi /* cfi */, tc /* tc */, dp /* dp */);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_qos_map_create\n");
        return rv;
    }

    /*
     * set discard DP to drop all packets with DP = 3
     */
    rv = bcm_cosq_discard_set(unit,
        (BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED));
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }
    /** in case the in_port=out_port, need to disable filtering */
    if(in_port == out_port)
    {
        disable_same_if_filter_all(unit, out_port);
        if (rv != BCM_E_NONE) {
            printf("Error, disable_same_if_filter_all\n");
            return rv;
        }
    }

    return rv;
}

/*
 * \brief
 *  Traffic cint example, which create one meter database for flood conteiment (port x Fwd-Type).
 *  It use dtatabase-0, single bucket mode with expansion.
 */
int
policer_ingress_generic_expansion_example(
    int unit,
    int in_port,
    int out_port,
    int color_blind)
{
    int rv;
    int inPortValue = in_port;  /* incoming port */
    bcm_field_stat_t stats[2];
    int core_id;                /* core matching the in_port */
    int meter_idx, base_meter_idx = 10;
    bcm_l2_addr_t l2addr;
    bcm_mac_t uc_mac = { 0x00, 0x0, 0x0, 0x0, 0x0, 0x1 };
    bcm_mac_t mc_mac = { 0x01, 0x0, 0x5e, 0x0, 0x0, 0x1 };
    bcm_mac_t bc_mac = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    int pkt_pri[2] = {1,4};
    int pkt_cfi[2] = {0,0};
    int tc[2] = {0,0};
    int dp[2] = {bcmColorGreen, bcmColorYellow};
    int database_handle;
    int database_id = 0;
    int ptr;
    char msg1[100];
    int nof_groups = 5;
    bcm_policer_expansion_group_t expansion_group;

    /** meter should be allocated in the core in which the ports are allocated */
    rv = policer_core_from_port_get(unit, inPortValue, &core_id);
    if (rv != BCM_E_NONE)
    {
        print rv;
        return rv;
    }

    bcm_policer_expansion_group_t_init(&expansion_group);
    /** configure expansion groups based on l2 forwarding types */
    /** in this example there are 5 groups per fwd-type */
    expansion_group.config[bcmPolicerFwdTypeUc].offset = 4;
    expansion_group.config[bcmPolicerFwdTypeUc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeMc].offset = 3;
    expansion_group.config[bcmPolicerFwdTypeMc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeUnkownMc].offset = 2;
    expansion_group.config[bcmPolicerFwdTypeUnkownMc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeUnkownUc].offset = 1;
    expansion_group.config[bcmPolicerFwdTypeUnkownUc].valid = TRUE;
    expansion_group.config[bcmPolicerFwdTypeBc].offset = 0;
    expansion_group.config[bcmPolicerFwdTypeBc].valid = TRUE;
    rv = bcm_policer_expansion_groups_set(unit, 0, core_id, &expansion_group);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_expansion_groups_set\n");
        return rv;
    }

    /** create database with expansion */
    rv = policer_generic_meter_database_example(unit, core_id, TRUE, database_id, 0 /** base_pointer */ ,
                                                TRUE /** single_bucket_mode */ , TRUE /* expansion_enable */ ,
                                                1280 /* total_nof_meters */, FALSE /* configure_meters */, 0 /*base_engine */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error in policer_generic_meter_database_example\n");
        return rv;
    }

    BCM_POLICER_DATABASE_HANDLE_SET(database_handle, TRUE, 0, core_id, database_id);

    /* create meter for each type. each one has different rate */
    for(meter_idx = base_meter_idx; meter_idx < (base_meter_idx + bcmPolicerFwdTypeMax); meter_idx++)
    {
        rv = policer_generic_create_example(unit, core_id, database_handle, meter_idx, bcmPolicerModeCommitted, color_blind,
            (30000+(5000*(meter_idx-base_meter_idx))));
        if (rv != BCM_E_NONE)
        {
            printf("Error (%d), in policer_generic_create_example \n", rv);
            return rv;
        }
    }

    /** map packets to the base meter. since expansion enabled,and contain 5 groups, the ptr should be base_meter_idx/nof_groups */
    ptr = base_meter_idx / nof_groups;
    rv = policer_ingress_meter_id_mapping_by_acl(unit, &database_id, &ptr, 1, bcmFieldQualifyInPort, in_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in policer_ingress_meter_id_mapping_by_acl\n", rv);
        return rv;
    }

    /** set uc traffic DA=1 and Vlan tag id 1 to out_port */
    bcm_l2_addr_t_init(&l2addr, uc_mac, 1);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add with vid 1\n");
        print rv;
        return rv;
    }
    /** set mc traffic with DA=1 and Vlan tag id 1 to out_port */
    bcm_l2_addr_t_init(&l2addr, mc_mac, 1);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add with vid 1\n");
        print rv;
        return rv;
    }
    /** set bc traffic and Vlan tag id 1 to out_port */
    bcm_l2_addr_t_init(&l2addr, bc_mac, 1);
    l2addr.port = out_port;
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_addr_add with vid 1\n");
        print rv;
        return rv;
    }

    /*
     * map PKT_PRI/CFI into INTERNAL TC/DP in the ingress:
     *           1/0   into           0/GREEN
     *           4/0   into           0/YELLOW
     */
    /*
     * The packet with priority 1 will be marked as green and packets with proiority 4 is marked as yellow.
     * if meter mode is not COLOR_BLIND and packet is yellow, the packet will go straight to the excess bucket
     * (without going through the committed bucket)
     * packet that will arrive with a green color will go through the committed bucket first.
 */
    rv = policer_generic_qos_ingress_port_set(unit, in_port, 100, 2, pkt_pri /* pkt_pri */, pkt_cfi /* cfi */, tc /* tc */, dp /* dp */);
    if (BCM_E_NONE != rv)
    {
        printf("Error in bcm_qos_map_create\n");
        return rv;
    }

    /*
     * set discard DP to drop all packets with DP = 2 or 3
     */
    rv = bcm_cosq_discard_set(unit, BCM_COSQ_DISCARD_ENABLE | BCM_COSQ_DISCARD_COLOR_BLACK | BCM_COSQ_DISCARD_COLOR_RED);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_cosq_discard_set\n");
        print rv;
    }
    /** in case the in_port=out_port, need to disable filtering */
    if(in_port == out_port)
    {
        disable_same_if_filter_all(unit, out_port);
        if (rv != BCM_E_NONE) {
            printf("Error, disable_same_if_filter_all\n");
            return rv;
        }
    }

    return rv;
}

