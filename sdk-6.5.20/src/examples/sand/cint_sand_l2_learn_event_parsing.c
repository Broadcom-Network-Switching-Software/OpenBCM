/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~MACT Learn Event Parsing~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_l2_learn_event_parsing.c
 * Purpose: Verify DSP packets are processed correctly
 *
 * Notes:
 *   cint_sand_l2_learn_event_parsing.c is built up on the top of cint_l2_learn_event_parsing.c
 *   which supports dpp devices only, while this new one supports both dpp and
 *   dnx devices.
 *
 * Jericho MACT Learn events when DMA is utilized arrive in ARAD Format.
 * In order to verify that the Learn Event is processed appropriately,
 * The cint allows the operator to create Payload Buffers for various
 * Learn Events according to parameters, transmit the packet to the device,
 * And later manually add the learned entry via BCM API calls in order to compare
 * The entry saved on each occasion
 *
 * CINT Usage:
 *  1.  Initialize global parameters used for buffer construction and LEM payload access
 *  2.  Construct Payload Buffer according to user parameters
 *  3.  Send DSP packet containing the constructed buffer (via TCL Packet Launcher)
 *      In order to trigger a Learn Event
 *  4.  Access the LEM table and save the Payload of the MACT Entry (as saved after parsing of the DSP)
 *  5.  Get l2_addr_t according to MACT and FID in use, clear MACT and add l2_addr using BCM API
 *  6.  Access the LEM table and save the Payload of the MACT Entry (as saved after adding the l2_addr via BCM API)
 *  7.  Compare the saved Payloads
 *
 * Basic example:
 *      BCM> cd ../../../../src
 *      BCM> cint examples/sand/utility/cint_sand_utils_global.c
 *      BCM> cint examples/dpp/utility/cint_utils_l2.c
 *      BCM> cint examples/sand/cint_l2_sand_learn_event_parsing.c
 *      BCM> tx 1 PSRC=200 DATA=0x000000000011ab67ff093e2281000064000000000
 *      BCM> cint
 *      cint> print compare_lem_entries_full(0);
 *
 * The above script runs a basic example of the test (not utilizing the TCL Packet Launcher and with hardcoded parameters)
 */

/* Global variables used for saving LEM Payloads and constructed buffers */
reg_val lem_payload;
reg_val payload_buffer;

/* Internal validation purposes */
char buffer_string[100];

/* DSP Parameters identifiers */
int DESTINATION_FLOW = 0;
int DESTINATION_MC = 1;
int DESTINATION_SYSTEM_PORT = 2;
int DESTINATION_FEC = 3;
int EEI_FEC = 4;
int EEI_MC = 5;

/* The payload buffer is saved on an array of two ints.
   Since the OutLIF is contained on both fields, different
   masks are used to construct it properly */
int outlif_msb_mask =  0x7;
int outlif_lsb_mask = 0x1FFF;
int outlif_msb_shift = 13;
int outlif_lsb_shift = 19;

int sid_fec_lsb_mask = 0x1FFFF;
int sid_fec_lsb_shift = 15;
int sid_fec_msb_mask = 0x7F;
int sid_fec_msb_shift = 17;

int sid_mc_lsb_mask = 0xFFFF;
int sid_mc_lsb_shift = 16;
int sid_mc_msb_mask = 0xFF;
int sid_mc_msb_shift = 16;

int eei_fec_cmd_shift = 3;
int eei_fec_data_lsb_mask = 0x1FFFF;
int eei_fec_data_lsb_shift = 15;
int eei_fec_data_msb_mask = 0x7;
int eei_fec_data_msb_shift = 17;

int eei_mc_cmd_shift = 4;
int eei_mc_data_lsb_mask = 0xFFFF;
int eei_mc_data_lsb_shift = 16;
int eei_mc_data_msb_mask = 0xF;
int eei_mc_data_msb_shift = 16;

/* Masks and values for various destination type parameters */
int valid_outlif_bit = 0x8;
int destination_system_port_flag = 0x40000;
int destination_multicast_flag = 0x50000;
int destination_flow_flag = 0x60000;
int destination_fec_flag = 0x20000;
int eei_fec_flag = 0x80;
int eei_mc_flag = 0x100;

/* Added to be compatible with JR2 */
/* Indicate destination is in msb or lsb of payload buffer */
/* For legacy it is lsb and for JR2 it is msb */
int destination_index_in_payload_buffer = 0;
/* Destination shift in payload buffer (msb or lsb) */
/* For legacy shift is 0 in lsb and for JR2 shift is 5 in msb */
int destination_shift = 0;

/* Verbose printing */
int DEBUG_FLAG = 1;

int show_mact_and_rpi(int unit){
    int rv = BCM_E_NONE;
    int is_jericho2 = 0;

    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (DEBUG_FLAG) {
        if (!is_jericho2) {
            bshell(unit, "diag pp rpi");
        }
        else
        {
            bshell(unit, "pp vis ppi");
        }
        bshell(unit, "l2 show");
    }
    return BCM_E_NONE;
}

int clear_mact(int unit){
    bshell(unit,"l2 clear all");
    return BCM_E_NONE;
}
int init_learn_event_parsing(int unit){
    int rv = BCM_E_NONE;
    int is_jericho2 = 0;
    int is_in_jericho2 = 0;

    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (is_jericho2) {
        is_in_jericho2 = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
    }

    printf("\t\t\t\t\t\t---------------------------\n");
    printf("\t\t\t\t\t\t| Initializing parameters is_in_jericho2 %d is_jericho2 %d |\n", is_in_jericho2, is_jericho2);
    sal_memset(&lem_payload, 0, sizeof(lem_payload));
    sal_memset(&buffer_string, 0, sizeof(buffer_string));
    printf("\t\t\t\t\t\t|           DONE          |\n");
    printf("\t\t\t\t\t\t---------------------------\n");

    if (is_in_jericho2) {
        valid_outlif_bit = 0;
        destination_system_port_flag = 0xc0000;
        destination_multicast_flag = 0x180000;
        destination_flow_flag = 0x100000;
        destination_fec_flag = 0x000000;
        destination_shift = 5;
        destination_index_in_payload_buffer = 1;

        outlif_msb_mask = 0x1f;
        outlif_lsb_mask = 0x1ffff;
        outlif_msb_shift = 17;
        outlif_lsb_shift = 15;

    }

    return BCM_E_NONE;
}

/* Compare the LEM table Payload after DSP learning and manual entry */
int compare_lem_entries(int unit, int dsp0, int manual0, int dsp1, int manual1){
    /* If parsing was done correctly, DSP and Manual entries should be identical */
    return (dsp0 == manual0 && dsp1 == manual1) ? BCM_E_NONE : BCM_E_FAIL;
}

/* Construct an ARAD DSP command type payload buffer according to defined criteria
   The payload is saved first as a reg_val (array of ints), and later as a string for the TCL Packet Launcher */
int construct_buffer(int unit, int payload_type, int valid_outlif, int outlif, int valid_sid, int sid, int eei_cmd, int eei_data, int entry){
    int rv = BCM_E_NONE;
    int is_jericho2 = 0;
    int is_in_jericho2 = 0;
    uint32 lpkgv_shift = 0;
    uint32 lpkgv_mask = 0;
    int lpkgv_with_outlif = 0;
    int lpkgv_wo_outlif = 0;

    rv = is_device_jericho2(unit, &is_jericho2);
    if (rv != BCM_E_NONE) {
        printf("Error, in is_device_jericho2\n");
        return rv;
    }

    if (is_jericho2) {
        is_in_jericho2 = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
    }

    if (is_in_jericho2) {
        lpkgv_shift = *(dnxc_data_get(unit,  "l2", "olp", "lpkgv_shift", NULL));
        lpkgv_mask = *(dnxc_data_get(unit,  "l2", "olp", "lpkgv_mask", NULL));
        dnx_dbal_fields_enum_value_get(unit, "IRPP_FWD_STAGE_FORMATS", "FWD_MACT_RESULT_SINGLE_OUTLIF", &lpkgv_with_outlif);
        dnx_dbal_fields_enum_value_get(unit, "IRPP_FWD_STAGE_FORMATS", "FWD_MACT_RESULT_NO_OUTLIF", &lpkgv_wo_outlif);
    }

    /* Init payload buffer */
    sal_memset(&payload_buffer, 0, sizeof(payload_buffer));

    /* General construction for Encoding type buffers */
    if (payload_type == DESTINATION_FLOW || payload_type == DESTINATION_MC ||
        payload_type == DESTINATION_SYSTEM_PORT || payload_type == DESTINATION_FEC) {

        /* Valid bit for OutLIF is always on for destination encoding
           If no OutLIF is used, OutLIF field is zero */
        payload_buffer[1] |= valid_outlif_bit;

        /* If OutLIF is valid, enter it into the array */
        if (valid_outlif) {
            payload_buffer[1] |= (outlif >> outlif_msb_shift) & outlif_msb_mask;
            payload_buffer[0] |= (outlif & outlif_lsb_mask) << outlif_lsb_shift;
            if (is_in_jericho2) {
                payload_buffer[1] |= (lpkgv_with_outlif & lpkgv_mask) << lpkgv_shift;
            }
        } else {
            if (is_in_jericho2) {
                payload_buffer[1] |= (lpkgv_wo_outlif & lpkgv_mask) << lpkgv_shift;
            }
        }
    }

    /* Construct encoding specific fields in payload buffer */
    switch (payload_type) {
    case DESTINATION_FLOW:

        /* Destination encoding contains indication of Flow ID and specific entry*/
        payload_buffer[destination_index_in_payload_buffer] |= (entry | destination_flow_flag) << destination_shift;
        break;
    case DESTINATION_MC:

        /* Destination encoding contains indication of MC ID and specific entry*/
        payload_buffer[destination_index_in_payload_buffer] |= (entry | destination_multicast_flag) << destination_shift;
        break;
    case DESTINATION_SYSTEM_PORT:

        /* Destination encoding contains indication of System Port and specific entry*/
        payload_buffer[destination_index_in_payload_buffer] |= (entry | destination_system_port_flag) << destination_shift;
        break;
    case DESTINATION_FEC:

        /* Destination encoding contains indication of FEC Pointer and specific entry*/
        payload_buffer[destination_index_in_payload_buffer] |= (entry | destination_fec_flag) << destination_shift;
        break;
    case EEI_FEC:

        /* EEI encoding contains indication of FEC Pointer and specific entry*/
        payload_buffer[0] |= entry;
        payload_buffer[1] |= eei_fec_flag;

        /* SID format */
        if (valid_sid) {
            payload_buffer[1] |= (sid >> sid_fec_msb_shift) & sid_fec_msb_mask;
            payload_buffer[0] |= (sid & sid_fec_lsb_mask) << sid_fec_lsb_shift;
        }

        /* Command and data format */
        else {
            payload_buffer[1] |= (eei_cmd << eei_fec_cmd_shift);
            payload_buffer[1] |= (eei_data >> eei_fec_data_msb_shift) & eei_fec_data_msb_mask;
            payload_buffer[0] |= (eei_data & eei_fec_data_lsb_mask) << eei_fec_data_lsb_shift;
        }
        break;
    case EEI_MC:

        /* EEI encoding contains indication of MC-ID and specific entry*/
        payload_buffer[0] |= entry;
        payload_buffer[1] |= eei_mc_flag;

        /* SID format */
        if (valid_sid) {
            payload_buffer[1] |= (sid >> sid_mc_msb_shift) & sid_mc_msb_mask;
            payload_buffer[0] |= (sid & sid_mc_lsb_mask) << sid_mc_lsb_shift;
        }

        /* Command and data format */
        else {
            payload_buffer[1] |= (eei_cmd << eei_mc_cmd_shift);
            payload_buffer[1] |= (eei_data >> eei_mc_data_msb_shift) & eei_mc_data_msb_mask;
            payload_buffer[0] |= (eei_data & eei_mc_data_lsb_mask) << eei_mc_data_lsb_shift;
        }
        break;
    }

    /* Print result to screen */
    printf("Payload constructed in array: 0x%08x 0x%08x\n", payload_buffer[1], payload_buffer[0]);
    printf("Payload constructed as string: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
            (payload_buffer[1] >> 24) & 0xff,(payload_buffer[1] >> 16) & 0xff,
            (payload_buffer[1] >> 8) & 0x7, (payload_buffer[1]) & 0xff,
            (payload_buffer[0] >> 24) & 0xff,(payload_buffer[0] >> 16) & 0xff,
            (payload_buffer[0] >> 8) & 0xff, (payload_buffer[0]) & 0xff);

    return BCM_E_NONE;
}

/* The functions below are wrapper functions for the different DSP packets available.
 * The user calls the functions from the TCL file according to the needed test.
 */
int construct_buffer_destination_system_port(int unit, int valid_outlif, int outlif, int system_port){
    return construct_buffer(unit, DESTINATION_SYSTEM_PORT, valid_outlif, outlif, 0, 0, 0, 0, system_port);
}

int construct_buffer_destination_mc(int unit, int valid_outlif, int outlif, int mc_id){
    return construct_buffer(unit, DESTINATION_MC, valid_outlif, outlif, 0, 0, 0, 0, mc_id);
}

int construct_buffer_destination_flow(int unit, int valid_outlif, int outlif, int flow_id){
    return construct_buffer(unit, DESTINATION_FLOW, valid_outlif, outlif, 0, 0, 0, 0, flow_id);
}

int construct_buffer_destination_fec(int unit, int valid_outlif, int outlif, int fec_ptr){
    return construct_buffer(unit, DESTINATION_FEC, valid_outlif, outlif, 0, 0, 0, 0, fec_ptr);
}

int construct_buffer_eei_fec(int unit, int valid_sid, int sid, int eei_cmd, int eei_data, int fec_ptr){
    return construct_buffer(unit, EEI_FEC, 0, 0, valid_sid, sid, eei_cmd, eei_data, fec_ptr);
}

int construct_buffer_eei_mc(int unit, int valid_sid, int sid, int eei_cmd, int eei_data, int mc_id){
    return construct_buffer(unit, EEI_MC, 0, 0, valid_sid, sid, eei_cmd, eei_data, mc_id);
}

/* Manual function for full process in cint */
int compare_lem_entries_full(int unit){
    reg_val res_dsp, res_bcm;
    bcm_l2_addr_t addr;
    bcm_mac_t mac = {0xab,0x67,0xff,0x09,0x3e,0x22};
    int i;
    int fid = 100;
    int rv = BCM_E_NONE;

    /* Get Payload */
    rv = get_payload_from_fid_and_mac(unit, fid, mac, &res_dsp);
    if (rv != BCM_E_NONE){
        printf("Error running get_payload_from_fid_and_mac\n");
        return rv;
    }

    /* Read address from LEM */
    rv = bcm_l2_addr_get(unit, mac, fid, &addr);
    if (rv != BCM_E_NONE){
        printf("Error running bcm_l2_addr_get\n");
        return rv;
    }

    /* Clear MACT */
    rv = bshell(unit, "l2 clear all");
    if (rv != BCM_E_NONE){
        printf("Error running clearing L2 table\n");
        return rv;
    }

    /* Add address to LEM using BCM API */
    rv = bcm_l2_addr_add(unit, &addr);
    if (rv != BCM_E_NONE){
        printf("Error running bcm_l2_addr_add\n");
        return rv;
    }

    /* Read Payload again */
    rv = get_payload_from_fid_and_mac(unit, fid, mac, &res_bcm);
    if (rv != BCM_E_NONE){
        printf("Error running get_payload_from_fid_and_mac\n");
        return rv;
    }

    /* Compare both read entries */
    for (i = 0; i < 3; i++) {
        if (res_bcm[i] != res_dsp[i]) {
            printf("Error: Data does not match\n");
            return BCM_E_FAIL;
        }
    }

    return rv;
}

/* Read entry from LEM Table, clear table and manually add the read entry */
int get_and_add_address_manually(int unit){
    int rv;
    reg_val res_dsp, res_bcm;
    bcm_l2_addr_t addr;
    bcm_mac_t mac = {0xab,0x67,0xff,0x09,0x3e,0x22};
    int fid = 100;
    int i;

    /* Get the existing entry */
    rv = bcm_l2_addr_get(unit, mac, fid, &addr);
    if (rv != BCM_E_NONE){
        printf("Error running bcm_l2_addr_get\n");
        return rv;
    }

    /* Clear LEM Table */
    rv = bshell(unit, "l2 clear all");
    if (rv != BCM_E_NONE){
        printf("Error running clearing L2 table\n");
        return rv;
    }

    /* Add the entry that was read earlier */
    rv = bcm_l2_addr_add(unit, &addr);
    if (rv != BCM_E_NONE){
        printf("Error running bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}

/* Get Payload Data for entry in LEM Table (wrapper)*/
int get_lem_data(int unit){
    int rv;
    bcm_mac_t mac = {0xab,0x67,0xff,0x09,0x3e,0x22};
    int fid=100;

    /* Verify OUT parameter is blank */
    sal_memset(&lem_payload, 0, sizeof(lem_payload));

    /* Get payload */
    rv = get_payload_from_fid_and_mac(unit, fid, mac, &lem_payload);
    if (rv != BCM_E_NONE){
        printf("Error running get_payload_from_fid_and_mac\n");
        return rv;
    }

    return rv;
}

/* Transmit constructed packet as OLP Packet */
int tx_packet(int unit, char *data) {
    int ptch[2];
    int rv = BCM_E_NONE;

    /* Set PTCH data. PP-SSP is OLP */
    ptch[0] = 0;
    ptch[1] = 0xf0;

    /* Transmit packet via BCM call */
    rv = tx_packet_via_bcm(unit, data, &ptch, 2);
    if (rv != BCM_E_NONE) {
        printf("Error in tx_packet\n");
        return rv;
    }

    return rv;
}
