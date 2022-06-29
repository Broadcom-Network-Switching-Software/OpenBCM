
/* *********
  Globals/Aux Variables
 ********** */

/* debug prints */
int verbose = 2;

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
* */
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

    return BCM_E_NONE;
}

/** This function accept a unit, port, and pointer to int, and fills it with system port appropriate for the unit-port combination.*/
int
port_to_system_port(
    int unit,
    int port,
    int *sysport)
{
    int rv;
    if (BCM_GPORT_IS_SYSTEM_PORT(port))
    {
        *sysport = port;
        return BCM_E_NONE;
    }

    rv = bcm_stk_gport_sysport_get(unit, port, sysport);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_stk_gport_sysport_get\n");
        return rv;
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

    return rv;
}

/*
 * System is in JR2 system headers mode
 */
int dnx_utils_is_jr2_system_header_mode(int unit){
     uint32 system_headers_mode;
     uint32 system_headers_mode_jr2;

     system_headers_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
     system_headers_mode_jr2 = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode_jericho2", NULL));
     return (system_headers_mode == system_headers_mode_jr2);
 }

int is_jr2_system_header_mode(int unit){
    return dnx_utils_is_jr2_system_header_mode(unit);
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
*******************************************************************************/
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


/** Returns TRUE if the device is JERICHO2 and above and KBP IPv4 application is enabled. */
int is_jericho2_kbp_ipv4_enabled(int unit) {
    return *dnxc_data_get(unit, "elk", "application", "ipv4", NULL);
}
/** Returns TRUE if the device is JERICHO2 and above and KBP IPv6 application is enabled. */
int is_jericho2_kbp_ipv6_enabled(int unit) {
    return *dnxc_data_get(unit, "elk", "application", "ipv6", NULL);
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

/*
 * Return the max single length in bytes
 */
int
get_adapter_signal_max_length_in_byte()
{
    int MAX_NUM_OF_INTS_IN_SIG_GETTER = 2000;
    return MAX_NUM_OF_INTS_IN_SIG_GETTER;
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

