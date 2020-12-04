/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Time - Broadcom Time BroadSync API - shared functionality
 */

#include <shared/util.h>
#include <shared/bsl.h>

#include <bcm/time.h>
#include <bcm/error.h>

#include <bcm_int/common/time.h>
#include <bcm_int/common/time-mbox.h>
#include <bcm_int/common/mbox.h>

#include <bcm_int/esw/mbcm.h>


#define FREQ_OFFSET_CMD_LEN  (1 + 4)
#define FREQ_OFFSET_RESP_LEN (1 + 1)
int
_bcm_time_bs_frequency_offset_set(int unit, bcm_time_spec_t new_offset)
{
    uint8  command[FREQ_OFFSET_CMD_LEN]  = {_BCM_TIME_FREQ_OFFSET_SET};
    int command_len = FREQ_OFFSET_CMD_LEN;

    uint8  response[FREQ_OFFSET_RESP_LEN] = {0};
    int response_len = FREQ_OFFSET_RESP_LEN;
    int rv;

    int offset = (new_offset.nanoseconds * 1000);

    /* The maximum drift allowed is 1ms per second */
    if ( (!COMPILER_64_IS_ZERO(new_offset.seconds)) || (new_offset.nanoseconds > 1000000) ) {
        return BCM_E_PARAM;
    }
                                           
    /* units in message to ToP are ppt, nanoseconds per second drift is ppb so convert */
    if (new_offset.isnegative) {
        offset = -offset;
    }

    _shr_uint32_write(&command[1], (uint32) offset);

    rv = _bcm_mbox_txrx(unit, 0, _BCM_MBOX_MESSAGE, command, command_len, response, &response_len);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "_bcm_mbox_txrx failed\n")));
        return BCM_E_INTERNAL;
    }  

    if (response_len != 2) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response_len != 2\n")));
        return BCM_E_INTERNAL; 
    }
    if (response[0] != command[0]) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[0] != command[0]\n")));
        return BCM_E_INTERNAL; 
    }
    if (response[1] != 0x0) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[1] != 0x0\n")));
        return BCM_E_FAIL; 
    }

    return BCM_E_NONE;
}


#define PHASE_OFFSET_CMD_LEN  (1 + 1 + 8 + 4)
#define PHASE_OFFSET_RESP_LEN (1 + 1)
int
_bcm_time_bs_phase_offset_set(int unit, bcm_time_spec_t new_offset)
{
    uint8  command[PHASE_OFFSET_CMD_LEN]  = {_BCM_TIME_PHASE_OFFSET_SET};
    int command_len = PHASE_OFFSET_CMD_LEN;

    uint8  response[PHASE_OFFSET_RESP_LEN] = {0};
    int response_len = PHASE_OFFSET_RESP_LEN;
    int rv;

    command[1] = new_offset.isnegative;
    _shr_uint64_write(&command[2], new_offset.seconds);
    _shr_uint32_write(&command[10], new_offset.nanoseconds);

    rv = _bcm_mbox_txrx(unit, 0, _BCM_MBOX_MESSAGE, command, command_len, response, &response_len);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "_bcm_mbox_txrx failed\n")));
        return BCM_E_INTERNAL;
    }  

    if (response_len != 2) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response_len != 2\n")));
        return BCM_E_INTERNAL; 
    }
    if (response[0] != command[0]) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[0] != command[0]\n")));
        return BCM_E_INTERNAL; 
    }
    if (response[1] != 0x0) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[1] != 0x0\n")));
        return BCM_E_FAIL; 
    }

    return BCM_E_NONE;
}

#define NTP_OFFSET_CMD_LEN  (1 + 1 + 8 + 4)
#define NTP_OFFSET_RESP_LEN (1 + 1)
int
_bcm_time_bs_ntp_offset_set(int unit, bcm_time_spec_t new_offset)
{
    uint8  command[NTP_OFFSET_CMD_LEN]  = {_BCM_TIME_NTP_OFFSET_SET};
    int command_len = NTP_OFFSET_CMD_LEN;

    uint8  response[NTP_OFFSET_RESP_LEN] = {0};
    int response_len = NTP_OFFSET_RESP_LEN;
    int rv;

    command[1] = new_offset.isnegative;
    _shr_uint64_write(&command[2], new_offset.seconds);
    _shr_uint32_write(&command[10], new_offset.nanoseconds);

    rv = _bcm_mbox_txrx(unit, 0, _BCM_MBOX_MESSAGE, command, command_len, response, &response_len);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "_bcm_mbox_txrx failed\n")));
        return BCM_E_INTERNAL;
    }  

    if (response_len != 2) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response_len != 2\n")));
        return BCM_E_INTERNAL; 
    }
    if (response[0] != command[0]) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[0] != command[0]\n")));
        return BCM_E_INTERNAL; 
    }
    if (response[1] != 0x0) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[1] != 0x0\n")));
        return BCM_E_FAIL; 
    }

    return BCM_E_NONE;
}



#define DEBUG_1PPS_CMD_LEN  (1 + 1)
#define DEBUG_1PPS_RESP_LEN (1 + 1)
int
_bcm_time_bs_debug_1pps_set(int unit, uint8 enableOutput)
{
    uint8  command[DEBUG_1PPS_CMD_LEN]  = {_BCM_TIME_DEBUG_1PPS_SET};
    int command_len = DEBUG_1PPS_CMD_LEN;

    uint8  response[DEBUG_1PPS_RESP_LEN] = {0};
    int response_len = DEBUG_1PPS_RESP_LEN;
    int rv;

    command[1] = enableOutput;

    rv = _bcm_mbox_txrx(unit, 0, _BCM_MBOX_MESSAGE, command, command_len, response, &response_len);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "_bcm_mbox_txrx failed\n")));
        return BCM_E_INTERNAL;
    }  

    if (response_len != 2) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response_len != 2\n")));
        return BCM_E_INTERNAL; 
    }
    if (response[0] != command[0]) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[0] != command[0]\n")));
        return BCM_E_INTERNAL; 
    }
    if (response[1] != 0x0) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[1] != 0x0\n")));
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}


#define STATUS_GET_CMD_LEN  (1)
#define STATUS_GET_RESP_LEN (2)
int
_bcm_time_bs_status_get(int unit, int *lock_status)
{
    uint8  command[STATUS_GET_CMD_LEN]  = {_BCM_TIME_STATUS_GET};
    int command_len = STATUS_GET_CMD_LEN;   
    int rv;

    uint8  response[STATUS_GET_RESP_LEN] = {0};
    int response_len = STATUS_GET_RESP_LEN;

    rv = _bcm_mbox_txrx(unit, 0, _BCM_MBOX_MESSAGE, command, command_len, response, &response_len);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "_bcm_mbox_txrx failed\n")));
        return BCM_E_INTERNAL;
    }  

    if (response_len != 2) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response_len != 2\n")));
        return BCM_E_INTERNAL; 
    }
    if (response[0] != command[0]) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[0] != command[0]\n")));
        return BCM_E_INTERNAL; 
    }

    /* Looks valid, return the second byte */
    *lock_status = (int)response[1];

    return BCM_E_NONE;
}


int
_bcm_time_bs_debug(uint32 flags)
{
    return _bcm_mbox_debug_flag_set(flags);
}

#define LOG_SET_CMD_LEN  (40)
#define LOG_SET_RESP_LEN (40)
/*
 * Function:
 *      _bcm_time_bs_log_configure
 * Purpose:
 *      Set the debug level and UDP log info for the firmware.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      debug_mask   - (IN) bitmask of debug levels (textual debug)
 *      udp_log_mask - (IN) bitmask of UDP log levels
 *      src_mac      - (IN) SRC MAC for UDP log packets
 *      dest_mac     - (IN) DEST MAC for UDP log packets
 *      tpid         - (IN) TPID for UDP log packets
 *      vid          - (IN) VID for UDP log packets
 *      ttl          - (IN) TTL for UDP log packets
 *      src_addr     - (IN) IPv4 SRC Address for UDP log packets
 *      dest_addr    - (IN) IPv4 DEST Address for UDP log packets
 *      udp_port     - (IN) UDP port for UDP log packets
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_time_bs_log_configure(int unit,
                           uint32 debug_mask, uint64 udp_log_mask,
                           bcm_mac_t src_mac, bcm_mac_t dest_mac,
                           uint16 tpid, uint16 vid, uint8 ttl,
                           bcm_ip_t src_addr, bcm_ip_t dest_addr,
                           uint16 udp_port)
{
    int rv = BCM_E_UNAVAIL;

    uint8 payload[LOG_SET_CMD_LEN] = {0};
    uint8 *curs = &payload[0];
    uint8 resp[LOG_SET_RESP_LEN];
    int resp_len = LOG_SET_RESP_LEN;


    /*
     * Make payload.
     *    Octet 0:      command
     *    Octet 1..4:   new debug mask level
     *    Octet 5..12:  new log mask level
     *    Octet 13..18: src_mac for log packets
     *    Octet 19..24: dst_mac for log packets
     *    Octet 25..26: tpid for log packets
     *    Octet 27..28: vid for log packets
     *    Octet 29:     TTL for log packets
     *    Octet 30..33: src_ip for log packets
     *    Octet 34..37: dst_ip for log packets
     *    Octet 38..39: UDP port for log packets
     */
    *curs = _BCM_TIME_LOG_SET; curs++;
    soc_htonl_store(curs, debug_mask);   curs += 4;
    soc_htonl_store(curs, COMPILER_64_HI(udp_log_mask)); curs += 4;
    soc_htonl_store(curs, COMPILER_64_LO(udp_log_mask)); curs += 4;
    sal_memcpy(curs, src_mac, 6);        curs += 6;
    sal_memcpy(curs, dest_mac, 6);       curs += 6;
    soc_htons_store(curs, tpid); curs += 2;
    soc_htons_store(curs, vid);  curs += 2;
    *curs = ttl;  curs++;
    soc_htonl_store(curs, src_addr);  curs += 4;
    soc_htonl_store(curs, dest_addr); curs += 4;
    soc_htons_store(curs, udp_port);  curs += 2;

    rv = _bcm_mbox_txrx(unit, 0, _BCM_MBOX_MESSAGE, payload, LOG_SET_CMD_LEN, resp, &resp_len);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "_bcm_mbox_txrx failed\n")));
        return BCM_E_INTERNAL;
    } 
    
    if (resp_len != 2) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "resp_len != 2\n")));
        return BCM_E_INTERNAL; 
    }
    if (resp[0] != payload[0]) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "resp[0] != payload[0]\n")));
        return BCM_E_INTERNAL; 
    }
    if (resp[1] != 0x0) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "resp[1] != 0x0\n")));
        return BCM_E_FAIL; 
    }

    return rv;
}

#define LOG_GET_CMD_LEN  (1)
#define LOG_GET_RESP_LEN (44)
/*
 * Function:
 *      _bcm_time_bs_log_configure_get
 * Purpose:
 *      Gets the debug level and UDP log info for the firmware.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      bs_log_cfg   - (OUT) BroadSync log configuration.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_time_bs_log_configure_get(int unit,
                            bcm_time_bs_log_cfg_t *bs_log_cfg)
{
    int rv = BCM_E_UNAVAIL;

    uint8  command[LOG_GET_CMD_LEN]  = {_BCM_TIME_LOG_GET};
    uint8 resp[LOG_GET_RESP_LEN];
    int resp_len = LOG_GET_RESP_LEN;
    uint8 *curs = &resp[0];

    rv = _bcm_mbox_txrx(unit, 0, _BCM_MBOX_MESSAGE, command, LOG_GET_CMD_LEN, resp, &resp_len);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "_bcm_mbox_txrx failed\n")));
        return BCM_E_INTERNAL;
    } 

     /*
     * Response format:
     *    Octet 0:      command
     *    Octet 1..4:   new debug mask level
     *    Octet 5..12:  new log mask level
     *    Octet 13..18: src_mac for log packets
     *    Octet 19..24: dst_mac for log packets
     *    Octet 25..26: tpid for log packets
     *    Octet 27..28: vid for log packets
     *    Octet 29:     TTL for log packets
     *    Octet 30..33: src_ip for log packets
     *    Octet 34..37: dst_ip for log packets
     *    Octet 38..39: Src UDP port for log packets
     *    Octet 40..41: Dest UDP port for log packets
     */
    
    if (resp_len != 42) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "resp_len != 42\n")));
        return BCM_E_INTERNAL; 
    }
    if (resp[0] != command[0]) { 
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "resp[0] != command[0]\n")));
        return BCM_E_INTERNAL; 
    }

    curs += 1;
    bs_log_cfg->debug_mask = soc_ntohl_load(curs);
    curs += 4;
    bs_log_cfg->udp_log_mask = soc_parse_uint64((char *)curs);
    curs += 8;
    sal_memcpy(bs_log_cfg->dest_mac, curs, 6);
    curs += 6;
    sal_memcpy(bs_log_cfg->src_mac, curs, 6);
    curs += 6;
    bs_log_cfg->tpid = soc_ntohs_load(curs);
    curs += 2;
    bs_log_cfg->vid = soc_ntohs_load(curs);
    curs += 2;
    bs_log_cfg->ttl = *curs;
    curs++;
    bs_log_cfg->src_addr = soc_ntohl_load(curs);
    curs += 4;
    bs_log_cfg->dest_addr = soc_ntohl_load(curs);
    curs += 4;
    bs_log_cfg->udp_port = soc_ntohs_load(curs);
    curs += 2;

    return rv;
}

/*
 * Function:
 *     _bcm_time_bs_time_set 
 * Purpose:
 *      Sets the broadsync time.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      bs_time - (OUT) BroadSync time in seconds and nanoseconds.
 * Returns:
 *      BCM_E_XXX
 */

#define BROADSYNC_TIME_SET_CMD_LEN  (1 + 1 + 8 + 4)
#define BROADSYNC_TIME_SET_RESP_LEN (1 + 1)
int
_bcm_time_bs_time_set(int unit, bcm_time_spec_t bs_time)
{
    uint8  command[BROADSYNC_TIME_SET_CMD_LEN]  = {_BCM_TIME_BS_TIME_SET};
    int command_len = BROADSYNC_TIME_SET_CMD_LEN;

    uint8  response[BROADSYNC_TIME_SET_RESP_LEN] = {0};
    int response_len = BROADSYNC_TIME_SET_RESP_LEN;
    int rv;

    command[1] = 0;
    _shr_uint64_write(&command[2], bs_time.seconds);
    _shr_uint32_write(&command[10], bs_time.nanoseconds);

    rv = _bcm_mbox_txrx(unit, 0, _BCM_MBOX_MESSAGE, command, command_len, response, &response_len);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "_bcm_mbox_txrx failed\n")));
        return BCM_E_INTERNAL;
    }

    if (response_len != 2) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response_len != 2\n")));
        return BCM_E_INTERNAL;
    }
    if (response[0] != command[0]) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[0] != command[0]\n")));
        return BCM_E_INTERNAL;
    }
    if (response[1] != 0x0) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[1] != 0x0\n")));
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

#define BROADSYNC_TIME_CMD_LEN  (1)
#define BROADSYNC_TIME_RESP_LEN (1 + 1 + 8 + 4)
/*
 * Function:
 *     _bcm_time_bs_time_get 
 * Purpose:
 *      Gets the broadsync time.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      bs_time - (OUT) BroadSync time in seconds and nanoseconds.
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_time_bs_time_get(int unit, bcm_time_spec_t* bs_time)
{
    uint8  command[BROADSYNC_TIME_CMD_LEN]  = {_BCM_TIME_BS_TIME_GET};
    int command_len = BROADSYNC_TIME_CMD_LEN;

    uint8  response[BROADSYNC_TIME_RESP_LEN] = {0};
    int response_len = BROADSYNC_TIME_RESP_LEN;
    int rv;

    rv = _bcm_mbox_txrx(unit, 0, _BCM_MBOX_MESSAGE, command, command_len, response, &response_len);
    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "_bcm_mbox_txrx failed\n")));
        return BCM_E_INTERNAL;
    }

    if (response_len != BROADSYNC_TIME_RESP_LEN) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response_len != BROADSYNC_TIME_CMD_LEN\n")));
        return BCM_E_INTERNAL;
    }

    if (response[0] != command[0]) {
        LOG_ERROR(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "response[0] != command[0]\n")));
        return BCM_E_INTERNAL;
    }

    bs_time->isnegative = 0;
    bs_time->seconds = _shr_uint64_read(&response[2]);
    bs_time->nanoseconds = _shr_uint32_read(&response[10]);

    return BCM_E_NONE;
}
