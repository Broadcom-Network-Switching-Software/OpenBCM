/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* ********* 
  Globals/Aux Variables
 */

/* debug prints */
int verbose = 2;


enum devices_list_defines_e {
    PETRA         = 0x88640,
    ARAD          = 0x88650,
    ARAD_PLUS     = 0x88660,
    ARDON         = 0x88202,
    JERICHO       = 0x88675,
    JERICHO_B0    = 0x1188675, /*11 is the revision*/
    QUMRAN_MX     = 0x88375,
    QUMRAN_MX_B0  = 0x1188375, /*11 is the revision*/
    JERICHO_PLUS  = 0x88680,
    QUMRAN_AX     = 0x88470,
    QUMRAN_AX_B0  = 0x1188470,   /*11 is the revision*/
    JERICHO2      = 0x88690,
    JERICHO2_B0   = 0x1188690,  /*11 is the revision*/
    QUMRAN_UX     = 0x88270,
    JERICHO2C     = 0x88800,
    QUMRAN2A      = 0x88480,
    QUMRAN2A_B0   = 0x1188480,
    JERICHO2P     = 0x88850,
    QUMRAN2_UX    = 0x88284
};

/** Returns TRUE if the device is Arad*/
int
is_arad_only(
    int unit,
    int *is_arad)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_arad = (info.device == 0x8650) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Arad A0*/
int
is_arad_a0_only(
    int unit,
    int *is_arad_a0)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_arad_a0 = (info.device == 0x8650 &
             info.revision == 0x0) ? 1 : 0;

    return rv;
}
/** Returns TRUE if the device is Arad B0*/
int
is_arad_b0_only(
    int unit,
    int *is_arad_b0)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_arad_b0 = (info.device == 0x8650 &
             info.revision == 0x11) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Arad B1*/
int
is_arad_b1_only(
    int unit,
    int *is_arad_b1)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_arad_b1 = (info.device == 0x8650 &
             info.revision == 0x12) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Arad Plus*/
int
is_arad_plus_only(
    int unit,
    int *is_arad_plus)
{
    bcm_info_t info;
    int rv = BCM_E_NONE;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        print rv;
        return rv;
    }

    *is_arad_plus = (info.device == 0x8660) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is QAX*/
int
is_device_qax_only(
    int unit,
    int *is_qax)
{
    bcm_info_t info;
    int rv;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_qax = (info.device == 0x8470)? 1: 0;
    return rv;
}
/** Returns TRUE if the device is QAX or QUX*/
int
is_qax(
    int unit,
    int *is_qax)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    is_qax = (info.device == 0x8470 || info.device == 0x8270) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is QUX*/
int
is_qux_only(
    int unit,
    int *is_qux)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_qux = (info.device == 0x8270) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is QMX*/
int
is_qmx_only(
    int unit,
    int *is_qmx)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_qmx = (info.device == 0x8375) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is QMX B0*/
int
is_qmx_b0_only(
    int unit,
    int *is_qmx_b0)
{
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_qmx_b0 = (info.device == 0x8375 &
             info.revision == 0x11) ? 1 : 0;

    return rv;
}
/** Returns TRUE if the device is Jericho*/
int
is_device_jericho_only(
    int unit,
    int *is_jer)
{
    bcm_info_t info;
    int rv;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_jer = (info.device == 0x8675)? 1: 0;
    return rv;
}
/** Returns TRUE if the device is Jericho B0*/
int
is_jericho_b0_only(
    int unit,
    int *is_jericho_b0)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_jericho_b0 = (info.device == 0x8675 &
             info.revision == 0x11) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Jericho Plus*/
int
is_device_jericho_plus_only(
    int unit,
    int *is_jer_plus)
{
    bcm_info_t info;
    int rv;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_info_get\n");
        return rv;
    }

    *is_jer_plus = (info.device == 0x8680)? 1: 0;
    return rv;
}
/** Returns TRUE if the device is Ramon*/
int
is_ramon_only(
    int unit,
    int *is_ramon)
{
    bcm_info_t info;
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }
    *is_ramon = (info.device == 0x8790) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Jericho 2*/
int
is_device_jericho2(
    int unit,
    int *is_jericho2)
{
    int rv = BCM_E_NONE;
    char *proc_name;
    int current_device_id_masked, current_device_id_masked_j2c;

    proc_name = "is_device_jericho2";
    bcm_info_t info;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_info_get \n", proc_name);
        return rv;
    }

    current_device_id_masked = (info.device & 0xfff0);
    current_device_id_masked_j2c = (info.device & 0xffc0);
    *is_jericho2 = (current_device_id_masked == 0x8690 || current_device_id_masked_j2c == 0x8800 || current_device_id_masked == 0x8480 || current_device_id_masked == 0x8850 || current_device_id_masked == 0x8280) ? 1 : 0;
    return rv;
}
/** Returns TRUE if the device is Jericho 2*/
int
is_device_j2c(
    int unit,
    int *is_j2c)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "is_device_j2c";
    bcm_info_t info;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_info_get \n", proc_name);
        return rv;
    }

    *is_j2c = ((info.device & 0xffc0) == 0x8800) ? 1 : 0;
    return rv;
}

/** Returns TRUE if the device is Jericho 2*/
int
is__jericho2_a0_only(
    int unit,
    int *is_jericho2_a0)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "is_device_jericho2";
    bcm_info_t info;
    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_info_get \n", proc_name);
        return rv;
    }
    *is_jericho2_a0 = (((info.device & 0xfff0) == 0x8690) && (info.revision == 0x1))  ? 1 : 0;

    return rv;
}

int
is_device_or_above(
    int unit,
    int supported_device_id)
{
    /** General device list - devices must be chronologically ordered.
        New devices should be added at the end */

    int DEVICE_LIST_SIZE = 19;
    int DEVICE_LIST [DEVICE_LIST_SIZE] = {0};
    DEVICE_LIST[0] = PETRA;
    DEVICE_LIST[1] = ARAD;
    DEVICE_LIST[2] = ARAD_PLUS;
    DEVICE_LIST[3] = ARDON;
    DEVICE_LIST[4] = JERICHO;
    DEVICE_LIST[5] = QUMRAN_MX;
    DEVICE_LIST[6] = JERICHO_B0;
    DEVICE_LIST[7] = QUMRAN_MX_B0;
    DEVICE_LIST[8] = JERICHO_PLUS;
    DEVICE_LIST[9] = QUMRAN_AX;
    DEVICE_LIST[10] = QUMRAN_AX_B0;
    DEVICE_LIST[11] = QUMRAN_UX;
    DEVICE_LIST[12] = JERICHO2;
    DEVICE_LIST[13] = JERICHO2_B0;
    DEVICE_LIST[14] = JERICHO2C;
    DEVICE_LIST[15] = QUMRAN2A;
    DEVICE_LIST[16] = QUMRAN2A_B0;
    DEVICE_LIST[17] = QUMRAN2_UX;
    DEVICE_LIST[18] = JERICHO2P;

    bcm_info_t info;
    int i;
    int current_device_id;
    int current_device_id_masked, current_device_id_masked_j2c;
    int current_device_index;
    int supported_device_index = DEVICE_LIST_SIZE;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_info_get\n");
        return rv;
    }

    current_device_id_masked = (info.device & 0xfff0);
    current_device_id_masked_j2c = (info.device & 0xffc0);

    if (current_device_id_masked == 0x8690 || current_device_id_masked == 0x8480 || current_device_id_masked == 0x8850)
    {
        current_device_id = current_device_id_masked;
    }
    else if (current_device_id_masked_j2c == 0x8800)
    {
        current_device_id = current_device_id_masked_j2c;
    }
    else if (current_device_id_masked == 0x8280)
    {
        current_device_id = (QUMRAN2_UX & 0xffff);
    }
    else
    {
        current_device_id = info.device;
    }

    for (i=0;i<DEVICE_LIST_SIZE;i++)
    {
        if (((DEVICE_LIST[i] & 0xffff) == current_device_id) && (info.revision >= (DEVICE_LIST[i] >> 20)))
        {
            current_device_index = i;
        }
        if (DEVICE_LIST[i] == supported_device_id)
        {
            supported_device_index = i;
        }
    }

    return current_device_index >= supported_device_index;
}
/*
* Utility function, which differentiate between FE3200/1600 and Ramon devices.
* Input parameters:
* unit - relevant unit;
* result - the returned result from the function
*/
int
is_device_ramon(
    int unit,
    int *result)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "is_device_ramon";
    bcm_info_t info;

    *result = 0;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error in bcm_info_get \n", proc_name);
        return rv;
    }

    if (info.device == 0x8790)
    {
        *result = 1;
    }

    return rv;
}

/** Returns TRUE if the device is JERICHO2 or bcm886xx_vlan_translate_mode is enabled.*/
int is_advanced_vlan_translation_mode(int unit)
{
    if (is_device_or_above(unit, JERICHO2)) {
        return TRUE;
    } else {
        return soc_property_get(unit, "bcm886xx_vlan_translate_mode", 0);
    }
}

/** Returns TRUE if the device is JERICHO2 and above and KBP IPv4 application is enabled. */
int is_jericho2_kbp_ipv4_enabled(int unit) {
    return (is_device_or_above(unit, JERICHO2) && soc_property_get(unit, "ext_ipv4_fwd_enable", 0));
}
/** Returns TRUE if the device is JERICHO2 and above and KBP IPv6 application is enabled. */
int is_jericho2_kbp_ipv6_enabled(int unit) {
    return (is_device_or_above(unit, JERICHO2) && soc_property_get(unit, "ext_ipv6_fwd_enable", 0));
}
/** Returns TRUE if KBP is using split FWD and RPF */
int is_jericho2_kbp_split_rpf_enabled(int unit) {
    return (*dnxc_data_get(unit, "elk", "application", "split_rpf", NULL) == 0) ? FALSE : TRUE;
}
/** Used for deciding whether to use KBP FWD and/or RPF only flags in the IPv4 route utilities */
int is_jericho2_kbp_ipv4_split_rpf_enabled(int unit) {
    return (is_jericho2_kbp_ipv4_enabled(unit) && is_jericho2_kbp_split_rpf_enabled(unit));
}
/** Used for deciding whether to use KBP FWD and/or RPF only flags in the IPv6 route utilities */
int is_jericho2_kbp_ipv6_split_rpf_enabled(int unit) {
    return (is_jericho2_kbp_ipv6_enabled(unit) && is_jericho2_kbp_split_rpf_enabled(unit));
}

/**
* Function: return 16 bits part of a sinal
* Purpose: return a signal (16 bits part of a signal), used for a TCL function that returns the full signal
* (as this is used by TCL the possible return value is int so only 16 bits)
*
* Params:
* unit      - Unit id
* core      - Core id
* block     - Block name
* from      - Stage/Memory signal originated from or passed through
* to        - Stage/Memory signal destined to
* name      - Signal`s name
 */
int
get_adapter_signal_values_16bit(
    int unit,
    int core,
    char *block,
    char *from,
    char *to,
    char *name,
    int pos_in_16)
{
    int MAX_NUM_OF_INTS_IN_SIG_GETTER = 2000;
    uint32 value[MAX_NUM_OF_INTS_IN_SIG_GETTER] = { 0 };
    int pos_in_32 = pos_in_16 / 2;

    /*
     * Get the signal from the adapter
     */
    dpp_dsig_read(unit, core, block, from, to, name, value, MAX_NUM_OF_INTS_IN_SIG_GETTER);

    return ((value[pos_in_32] >> ((pos_in_16 & 0x1) * 16)) & 0xFFFF);
}
/*
 * Return the max single length in bytes
 */
int
get_adapter_signal_max_length_in_byte()
{
    int MAX_NUM_OF_INTS_IN_SIG_GETTER = 2000;
    return MAX_NUM_OF_INTS_IN_SIG_GETTER;
}

/*
 * Extract core from input local port. On fail, return '-1'
 */
int
get_core_and_tm_port_from_port(
    int unit,
    bcm_port_t port,
    int * core,
    int * tm_port)
{
    int rv = BCM_E_NONE;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_port_get failed: port %d\n", port);
        return rv;
    }
    else
    {
        *core = mapping_info.core;
        *tm_port = mapping_info.tm_port;
    }

    return BCM_E_NONE;
}

/*
 * Get the module id of the device in the specific index
 */
int
get_modid(
    int unit,
    int index,
    bcm_module_t *modid)
{
    int rv = BCM_E_NONE;
    int is_dnx;
    int modid_count;
    int actual_modid_count;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (BCM_FAILURE(rv))
    {
        printf("bcm_device_member_get failed");
        return(rv);
    }

    if (is_dnx)
    {
        rv = bcm_stk_modid_count(unit, &modid_count);
        if (BCM_FAILURE(rv))
        {
            printf("Error, in bcm_stk_modid_count\n");
            return rv;
        }
        bcm_stk_modid_config_t modid_array[modid_count];

        rv = bcm_stk_modid_config_get_all(unit, modid_count, modid_array, &actual_modid_count);
        if (BCM_FAILURE(rv))
        {
            printf("Error, in bcm_stk_modid_config_get_all\n");
            return rv;
        }
        *modid = modid_array[index].modid;
    }
    else
    {
        bcm_module_t base_modid;

        rv = bcm_stk_my_modid_get(unit, base_modid);
        if (BCM_FAILURE(rv))
        {
            printf("Error, in bcm_stk_my_modid_get\n");
            return rv;
        }

        *modid = base_modid + index;
    }

    return BCM_E_NONE;
}

/** This function accept a unit, port, and pointer to int, and fills it with system port appropriate for the unit-port combination.*/
int
port_to_system_port(
    int unit,
    int port,
    int *sysport)
{
    int modid, gport, rv, tm_port, core;
    if (BCM_GPORT_IS_SYSTEM_PORT(port))
    {
        *sysport = port;
        return BCM_E_NONE;
    }

    if (is_device_or_above(unit, JERICHO))
    {
        /* Core should also be considered */
        rv = get_core_and_tm_port_from_port(unit,port,&core,&tm_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
        }
    }
    else
    {
        core = 0;
        tm_port = port;
    }
    /* On JR2, Getting system port from modport is not supported, so set system port directly */
    if (!is_device_or_above(unit, JERICHO2)) {
        rv = get_modid(unit, 0, &modid);
        if (BCM_FAILURE(rv))
        {
            printf("Error, in get_modid\n");
            return rv;
        }

        BCM_GPORT_MODPORT_SET(gport, modid + core, tm_port);

        rv = bcm_stk_gport_sysport_get(unit, gport, sysport);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_gport_sysport_get\n");
            return rv;
        }
    } else {
        gport = port;
        rv = bcm_stk_gport_sysport_get(unit, gport, sysport);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_gport_sysport_get\n");
            return rv;
        }
    }

    return BCM_E_NONE;
}

/** This function accept a unit, system port, and two pointers to int, fills with local port and is local or not.*/
int
system_port_is_local(int unit, int sysport, int *local_port, int *is_local){
    int rv = BCM_E_NONE;
    int my_modid, gport_modid, num_modid, max_modid, modid_idx;
    int gport;

    *is_local = 0;

    if (!BCM_GPORT_IS_SET(sysport) || BCM_GPORT_IS_TRUNK(sysport))
    {
        *local_port = sysport;
        *is_local = 1;
        return BCM_E_NONE;
    }

    rv = bcm_stk_sysport_gport_get(unit, sysport, &gport);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_sysport_gport_get\n");
        return rv;
    }
    printf("Sysport's unit%d  sysport is :%d,gport is: 0x%x\n",unit, sysport, gport);
    gport_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    *local_port = BCM_GPORT_MODPORT_PORT_GET(gport);
    printf("Required modid is: 0x%x\n", gport_modid);
    printf("Sysport's local port is: 0x%x\n", *local_port);
    rv = bcm_stk_modid_count(unit, &max_modid);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_modid_count\n");
    }
    if (is_device_or_above(unit, JERICHO2)) {
        bcm_stk_modid_config_t modid_config[max_modid];
        rv = bcm_stk_modid_config_get_all(unit, max_modid, modid_config, &num_modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_config_get_all\n");
        }

        for (modid_idx = 0; modid_idx < num_modid; ++modid_idx){
            if (gport_modid == modid_config[modid_idx].modid){
                *is_local = 1;
                printf("sysport 0x%x is a local port for unit %d\n", sysport, unit);
                break;
            } 
        }
        if (*is_local == 0) {
            printf("sysport 0x%x is not a local port for unit %d\n", sysport, unit);
        }
    } else {
        rv = bcm_stk_modid_get(unit, &my_modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_get\n");
        }

        /* By default, modids configured on a device are always consecutive, and the modid returned from bcm_stk_modid_get is always the lowest. */
        if ((gport_modid >= my_modid) && (gport_modid < (my_modid + max_modid))){
            *is_local = 1;
            printf("sysport 0x%x is a local port for unit %d\n", sysport, unit);
        } else {
            *is_local = 0;
            printf("sysport 0x%x is not a local port for unit %d\n", sysport, unit);
        }
    }

    return rv;
}

/** This function accept a unit, system port, and two pointers to int, fills with local port and is local or not.*/
int
is_system_port_is_local(int unit, int sysport){
    int rv = BCM_E_NONE;
    int local_port, sysport_is_local = 0;

    rv = system_port_is_local(unit, sysport, &local_port, &sysport_is_local);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in sysport_is_local().\n");
        return 0;
    }

    return sysport_is_local;
}

/** This function accept a unit, gport, and two pointers to int, fills with local port and is local or not.*/
int
gport_is_local(int unit, int gport, int *local_port, int *is_local){
    int rv = BCM_E_NONE;
    int my_modid, gport_modid, num_modid, max_modid, modid_idx;

    *is_local = 0;
    
    gport_modid = BCM_GPORT_MODPORT_MODID_GET(gport);
    *local_port = BCM_GPORT_MODPORT_PORT_GET(gport);
    printf("Required modid is: 0x%x\n", gport_modid);
    printf("gport(0x%x)'s local port is: 0x%x\n", gport, *local_port);
    rv = bcm_stk_modid_count(unit, &max_modid);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_modid_count\n");
    }
    if (is_device_or_above(unit, JERICHO2)) {
        bcm_stk_modid_config_t modid_config[max_modid];
        rv = bcm_stk_modid_config_get_all(unit, max_modid, modid_config, &num_modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_config_get_all\n");
        }

        for (modid_idx = 0; modid_idx < num_modid; ++modid_idx){
            if (gport_modid == modid_config[modid_idx].modid){
                *is_local = 1;
                printf("gport 0x%x is a local port for unit %d\n", gport, unit);
                break;
            } 
        }
        if (*is_local == 0){    
            printf("gport 0x%x is not a local port for unit %d\n", gport, unit);
        }
    } else {
        rv = bcm_stk_modid_get(unit, &my_modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_get\n");
        }

        /* By default, modids configured on a device are always consecutive, and the modid returned from bcm_stk_modid_get is always the lowest. */
        if ((gport_modid >= my_modid) && (gport_modid < (my_modid + max_modid))){
            *is_local = 1;
            printf("gport 0x%x is a local port for unit %d\n", gport, unit);
        } else {
            *is_local = 0;
            printf("gport 0x%x is not a local port for unit %d\n", gport, unit);
        }
    }

    return rv;
}

/* These defines used for in/out port definitions*/
int IN_PORT=0;
int OUT_PORT=1;
int NUMBER_OF_PORTS=2;

/* General port defines*/
int PORT_1 = 0;
int PORT_2 = 1;
int PORT_3 = 2;
int PORT_4 = 3;

/* General RIF defines */
int RIF_1 = 0;
int RIF_2 = 1;
int RIF_3 = 2;
int RIF_4 = 3;

/* General host defines*/
int HOST_1 = 0;
int HOST_2 = 1;
int HOST_3 = 2;
int HOST_4 = 3;
int HOST_5 = 4;

/* port_tpid */
struct port_tpid_info_s {
    int port;
    int outer_tpids[2];
    int nof_outers; /*0, 1, 2*/
    int inner_tpids[2];
    int nof_inners; /*0, 1 */
    int discard_frames; /* BCM_PORT_DISCARD_NONE/ALL/UNTAG/TAG */
    uint32 tpid_class_flags; /* flags for bcm_port_tpid_class_set */
    int vlan_transation_profile_id; /* Default Ingress action command in case of no match in VTT L2 LIF lookup */
};

port_tpid_info_s port_tpid_info1;

/* Global declaration, since many cints use it and some redeclare it, or don't declare it at all, which causes errors. */
int advanced_vlan_translation_mode = 0;

/* indicate if the device is jericho
   Get unit - unit id
   yesno  -  indicate if the device is jericho */ 
int is_jericho_only(int unit, int *yesno) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    *yesno = info.device == 0x8675 ? 1 : 0;

    return rv;
}

/* indicate if the device is qumran_mx
   Get unit - unit id
   yesno  -  indicate if the device is qumran_mx */ 
int is_qumran_mx_only(int unit, int *yesno) {
    bcm_info_t info;

 
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    *yesno = info.device == 0x8375 ? 1 : 0;

    return rv;
}

/* indicate if the device is qumran_mx b0
   Get unit - unit id
   yesno  -  indicate if the device is qumran_mx b0 */ 
int is_qumran_mx_b0_only(int unit, int *yesno) {
    bcm_info_t info;

 
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    *yesno = info.device == 0x8375 &
             info.revision == 0x11 ? 1 : 0;

    return rv;
}

/* indicate if the device is jericho plus
   Get unit - unit id
   yesno  -  indicate if the device is jericho plus */ 
int is_jericho_plus_only(int unit, int *yesno) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    *yesno = info.device == 0x8680 ? 1 : 0;

    return rv;
}

/* indicate if the device is qumran_ax
   Get unit - unit id
   yesno  -  indicate if the device is qumran_ax */ 
int is_qumran_ax_only(int unit, int *yesno) {
    bcm_info_t info;

 
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    *yesno = info.device == 0x8470 ? 1 : 0;

    return rv;
}

/* indicate if the device is qumran_ax_a0
   Get unit - unit id
   yesno  -  indicate if the device is qumran_ax_a0 */ 
int is_qumran_ax_a0_only(int unit, int *yesno) {
    bcm_info_t info;

 
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    *yesno = info.device == 0x8470 &
        info.revision == 0 ? 1 : 0;

    return rv;
}

/* indicate if the device is qumran_ax
   Get unit - unit id
   yesno  -  indicate if the device is qumran_ax */ 
int is_qumran_ux_only(int unit, int *yesno) {
    bcm_info_t info;

 
    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    *yesno = info.device == 0x8270 ? 1 : 0;

    return rv;
}

/* indicate if the device is J2C
   Get unit - unit id
   yesno  -  indicate if the device is J2C */
int is_j2c_only(int unit, int *yesno) {
    bcm_info_t info;


    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
          printf("Error in bcm_info_get\n");
          print rv;
          return rv;
    }

    *yesno = ((info.device & 0xffc0) == 0x8800) ? 1 : 0;

    return rv;
}

/* indicate if the device is qumran_ax
   Get unit - unit id
   yesno  -  indicate if the device is qumran_ax */ 
int is_qumran_ax(int unit, int *yesno) {
    bcm_info_t info;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    if (info.device == 0x8470 || info.device == 0x8270) {
        *yesno = 1;
    } else {
        *yesno = 0;
    }

    return rv;
}

/* port_tpid */

/* 
 *  nof_outers = 1,2
 *  nof_inners = 0,1
 *  nof_outers >= nof_inners
 *  nof_outers + nof_inners = 2
 */ 
void
port_tpid_init(int port, int nof_outers, int nof_inners) {
    port_tpid_info1.port = port;
    port_tpid_info1.outer_tpids[0] = 0x8100;
    port_tpid_info1.outer_tpids[1] = 0x88a0;
    port_tpid_info1.inner_tpids[0] = 0x9100;
    port_tpid_info1.nof_outers = nof_outers;
    port_tpid_info1.nof_inners = nof_inners;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_NONE;
    port_tpid_info1.tpid_class_flags = 0;
    port_tpid_info1.vlan_transation_profile_id = 0;
}

int
port_tpid_set(int unit) {
    return _port_tpid_set(unit, 0);
}

int
port_lif_tpid_set(int unit) {
    return _port_tpid_set(unit, 1);
}

int
_port_tpid_set(int unit, int is_lif) {
    int rv;
    int indx;
    int color_set=0;/* has to be zero */

    advanced_vlan_translation_mode = soc_property_get(unit, "bcm886xx_vlan_translate_mode",0);

    if (advanced_vlan_translation_mode) {
        if (port_tpid_info1.nof_outers == 1 && port_tpid_info1.nof_inners == 1) {
            return port_dt_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[0], port_tpid_info1.inner_tpids[0], is_lif, port_tpid_info1.vlan_transation_profile_id);
        } else if (port_tpid_info1.nof_outers == 1 && port_tpid_info1.nof_inners == 0) {
            return port_outer_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[0], is_lif, port_tpid_info1.vlan_transation_profile_id);
        }
    }
    
    /* assume port_tpid_init was called with legal params*/
    
    /* remove old tpids on port */
    rv = bcm_port_tpid_delete_all(unit,port_tpid_info1.port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all, port=%d, \n", port_tpid_info1.port);
        return rv;
    }
    
    /* set outer tpids of the port */
    for(indx = 0; indx < port_tpid_info1.nof_outers; ++indx){
        rv = bcm_port_tpid_add(unit, port_tpid_info1.port, port_tpid_info1.outer_tpids[indx], color_set);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_tpid_add, tpid=%d, \n", port_tpid_info1.outer_tpids[indx]);
            return rv;
        }
    }
    
    /* set inner tpids of the port */
    for(indx = 0; indx < port_tpid_info1.nof_inners; ++indx){
        rv = bcm_port_inner_tpid_set(unit, port_tpid_info1.port, port_tpid_info1.inner_tpids[indx]);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, tpid=%d, \n", port_tpid_info1.inner_tpids[indx]);
            return rv;
        }
    }
    
    /* Only relevant for physical port */    
    if(!is_lif) {    
        /* set discard frames on port */
        rv = bcm_port_discard_set(unit, port_tpid_info1.port, port_tpid_info1.discard_frames);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_discard_set, discard-type=%d, \n", port_tpid_info1.discard_frames);
            return rv;
        }
    }
    
    return rv;
}

/* Send a packet from string input */
int tx_packet_via_bcm(int unit, char *data, int *ptch, int ptch_version) {
    char tmp, data_iter;
    int data_len, i, pkt_len, ptch_len, ind, data_base;
    bcm_pkt_t *pkt_info;
    bcm_error_t rv;

    /* If string data starts with 0x or 0X, skip it */
    data_base = 0;
    if ((data[0] == '0') && ((data[1] == 'x') || (data[1] == 'X'))) {
        data_base = 2;
    }

    /* PTCH length by version */
    ptch_len = ptch_version == 1 ? 3 : 2;

    /* figure out packet data_length (no strdata_len in cint) */
    data_len=0;
    i=0;
    while (data[data_base+data_len] != 0) ++data_len;

    pkt_len = 64+ptch_len;
    pkt_len = (pkt_len < data_len+1 ? (data_len+ptch_len) : pkt_len);

    rv = bcm_pkt_alloc(unit, pkt_len, BCM_TX_CRC_ALLOC, &pkt_info);
    BCM_IF_ERROR_RETURN(rv);

    sal_memset(pkt_info->_pkt_data.data, 0, pkt_len);

    pkt_info->pkt_data = &pkt_info->_pkt_data;

    /* Determine which core should the packet be injected to */
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    rv = bcm_port_get(unit, ptch[ptch_len-1], &dummy_flags, &interface_info, &mapping_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_APPL_COMMON,
            (BSL_META_U(unit,"bcm_port_get failed. Error:%d (%s)\n"),
                                                    rc, bcm_errmsg(rc)));
        return -1;
    }

    if(mapping_info.core==1) {
        pkt_info->_dpp_hdr[0] = mapping_info.channel;
        pkt_info->flags = pkt_info->flags | BCM_PKT_F_HGHDR;
    }

    printf("Packet will be injected to core %d, local port %d\n",mapping_info.core, mapping_info.tm_port);

    /* PTCH - 2B / 3B */
    pkt_info->_pkt_data.data[0] = ptch[0];
    pkt_info->_pkt_data.data[1] = 0;
    pkt_info->_pkt_data.data[ptch_len-1] = mapping_info.tm_port;

    /* Convert char to value */
    i = 0;
    while (i < data_len) {
        data_iter=data[data_base+i];
        if (('0' <= data_iter) && (data_iter <= '9')) {
            tmp = data_iter - '0';
        } else if (('a' <= data_iter) && (data_iter <= 'f')) {
            tmp = data_iter - 'a' + 10;
        } else if (('A' <= data_iter) && (data_iter <= 'F')) {
            tmp = data_iter - 'A' + 10;
        } else {
            printf("Unexpected char: %c\n", data_iter);
            return BCM_E_PARAM;
        }

        /* String input is in 4b unit. Below we're filling in 8b:
           offset is /2, and we shift by 4b if the input char is odd */
        pkt_info->_pkt_data.data[ptch_len+i/2] |= tmp << (((i+1)%2)*4);
        ++i;
    }

    rv = bcm_tx(unit, pkt_info, NULL);

    return rv;
}

/*****************************************************************************
* Function:  check_eq_sig_val
* Purpose:   Check that the value in signal equals to the required one
* Params:
* unit      - 
* core      - 
* block     - 
* from      - 
* to        - 
* name      - 
* compareTo - 
* Return:    (int)
 */
int check_eq_sig_val(int unit, int core,  char *block, char *from, char *to, char *name, uint32 compareTo)
{
    uint32 value;
    int size = 32;
    int rc = BCM_E_NONE;

    uint32 fetch_value[2] = { 0 };
    int fetch_size = 64;
    
    rc = dpp_dsig_read(unit,core,block,from,to,name,fetch_value,fetch_size);
    value = fetch_value[0];
    if(rc != BCM_E_NONE)
    {
        printf("Error in dpp_dsig_read\n");
        return BCM_E_FAIL;
    }
    printf("Check Signal value: %s %s %s %s value: 0x%X compareTo 0x%X\n",block,from,to,name,value,compareTo);
    if(value == compareTo)
    {
        printf("Copmare Success Values are Equal\n");
        return BCM_E_NONE;
    }
    else
    {
        printf("Copmare Fail Values are NOT Equal\n");
        return BCM_E_PARAM;
    }
    
    return 0;
}

/* Get local port number of device
 *    unit   - unit id
 *    return - int, local port number
 */ 
int get_local_port_number(int unit)
{
    bcm_info_t info;
    int port_num = 256;

    int rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
      printf("Error in bcm_info_get\n");
      print rv;
      return rv;
    }

    print info;
    if (info.device == 0x8675 ||
        info.device == 0x8375 ||
        info.device == 0x8680) 
    {
        port_num = 512;
    }

    return port_num;
}


