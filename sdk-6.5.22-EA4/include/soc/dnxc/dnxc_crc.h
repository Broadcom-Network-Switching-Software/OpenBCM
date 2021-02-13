
#ifndef __DNXC_CRC_H__
#define __DNXC_CRC_H__

shr_error_e soc_dnxc_crc_calc(
    int unit,
    uint8 *payload,
    int payload_size,
    uint16 *crc_result);

#endif
