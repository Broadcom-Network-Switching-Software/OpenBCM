

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA


#include <soc/dnxc/dnxc_data_property.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/drv.h>
#include <sal/appl/sal.h>


#define DNXC_DATA_PROPERTY_FW_CRC_CHECK_BITS_OFFSET          (8)
#define DNXC_DATA_PROPERTY_FW_CRC_CHECK_BITS_MASK            (0x100)
#define DNXC_DATA_PROPERTY_FW_LOAD_VERIFY_BITS_OFFSET        (12)
#define DNXC_DATA_PROPERTY_FW_LOAD_VERIFY_BITS_MASK          (0x1000)
#define DNXC_DATA_PROPERTY_FW_LOAD_METHOD_BITS_MASK          (0x03)
#define DNXC_DATA_PROPERTY_FW_LOAD_METHOD_MIN_VAL            (0x1)
#define DNXC_DATA_PROPERTY_FW_LOAD_METHOD_MAX_VAL            (0x2)

#define DNXC_DATA_PROPERTY_MAX_TX_TAPS                       (6)


shr_error_e
dnxc_data_property_fw_load_method_read(
    int unit,
    uint32 *fw_load_method,
    const char *suffix)
{
    uint32 mask =
        DNXC_DATA_PROPERTY_FW_LOAD_VERIFY_BITS_MASK | DNXC_DATA_PROPERTY_FW_CRC_CHECK_BITS_MASK |
        DNXC_DATA_PROPERTY_FW_LOAD_METHOD_BITS_MASK;
    uint32 load_firmware_prop_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    load_firmware_prop_val = soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, suffix, *fw_load_method);

    
    SHR_MASK_VERIFY(load_firmware_prop_val, mask, _SHR_E_CONFIG, "LOAD_FIRMWARE soc property value is not valid");

    *fw_load_method = load_firmware_prop_val & DNXC_DATA_PROPERTY_FW_LOAD_METHOD_BITS_MASK;

    
    if (*fw_load_method == 0)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG,
                     "Invalid value %d for load_firmware_%s SOC property. It is not supported to omit FW load on DNX/DNXF devices ",
                     *fw_load_method, suffix);
    }
    else
    {
        
        
         
        SHR_RANGE_VERIFY(*fw_load_method,
                         DNXC_DATA_PROPERTY_FW_LOAD_METHOD_MIN_VAL, DNXC_DATA_PROPERTY_FW_LOAD_METHOD_MAX_VAL,
                         _SHR_E_CONFIG, "FW Load Method value should be between %d-%d",
                         DNXC_DATA_PROPERTY_FW_LOAD_METHOD_MIN_VAL, DNXC_DATA_PROPERTY_FW_LOAD_METHOD_MAX_VAL);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_property_fw_crc_check_read(
    int unit,
    uint32 *fw_crc_check,
    const char *suffix)
{
    uint32 load_firmware_prop_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    load_firmware_prop_val =
        soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, suffix,
                                    *fw_crc_check << DNXC_DATA_PROPERTY_FW_CRC_CHECK_BITS_OFFSET);

    *fw_crc_check =
        (load_firmware_prop_val & DNXC_DATA_PROPERTY_FW_CRC_CHECK_BITS_MASK) >>
        DNXC_DATA_PROPERTY_FW_CRC_CHECK_BITS_OFFSET;

    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_property_fw_load_verify_read(
    int unit,
    uint32 *fw_load_verify,
    const char *suffix)
{
    uint32 load_firmware_prop_val = 0;
    SHR_FUNC_INIT_VARS(unit);

    load_firmware_prop_val =
        soc_property_suffix_num_get(unit, -1, spn_LOAD_FIRMWARE, suffix,
                                    *fw_load_verify << DNXC_DATA_PROPERTY_FW_LOAD_VERIFY_BITS_OFFSET);

    *fw_load_verify =
        (load_firmware_prop_val & DNXC_DATA_PROPERTY_FW_LOAD_VERIFY_BITS_MASK) >>
        DNXC_DATA_PROPERTY_FW_LOAD_VERIFY_BITS_OFFSET;

    SHR_FUNC_EXIT;
}


shr_error_e
dnxc_data_property_serdes_tx_taps_read(
    int unit,
    int port,
    char *propval,
    bcm_port_phy_tx_t * tx)
{
    char *prefix, *nrz_prefix, *pam4_prefix, *propval_tmp;
    int len, nrz_len, pam4_len;
    int tap_index;
    int taps[DNXC_DATA_PROPERTY_MAX_TX_TAPS];
    SHR_FUNC_INIT_VARS(unit);

    bcm_port_phy_tx_t_init(tx);

    
    if (propval)
    {
        
        nrz_prefix = "nrz";
        nrz_len = strlen(nrz_prefix);
        pam4_prefix = "pam4";
        pam4_len = strlen(pam4_prefix);
        if (!sal_strncasecmp(propval, nrz_prefix, nrz_len))
        {
            tx->signalling_mode = bcmPortPhySignallingModeNRZ;
            propval += nrz_len;
        }
        else if (!sal_strncasecmp(propval, pam4_prefix, pam4_len))
        {
            tx->signalling_mode = bcmPortPhySignallingModePAM4;
            propval += pam4_len;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: invalid serdes_tx_taps configuration - signalling mode - \"nrz\" or \"pam4\" is missing",
                         port);
        }

        
        tx->tx_tap_mode = bcmPortPhyTxTapMode6Tap;

        prefix = ":";
        len = strlen(prefix);
        for (tap_index = 0; tap_index < DNXC_DATA_PROPERTY_MAX_TX_TAPS; ++tap_index)
        {
            if (sal_strncasecmp(propval, prefix, len))
            {
                
                if (tap_index == 3)
                {
                    tx->tx_tap_mode = bcmPortPhyTxTapMode3Tap;
                    break;
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG,
                                 "port %d: invalid serdes_tx_taps configuration - \":\" is missing between the taps",
                                 port);
                }
            }
            propval += len;

            taps[tap_index] = sal_ctoi(propval, &propval_tmp);
            if (propval == propval_tmp)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG,
                             "port %d: invalid serdes_tx_taps configuration - tap is missing after \":\"", port);
            }

            propval = propval_tmp;
        }

        
        tx->pre = taps[0];
        tx->main = taps[1];
        tx->post = taps[2];
        
        if (tx->tx_tap_mode == bcmPortPhyTxTapMode6Tap)
        {
            tx->pre2 = taps[3];
            tx->post2 = taps[4];
            tx->post3 = taps[5];
        }
    }
    
    else
    {
        tx->main = DNXC_PORT_TX_FIR_INVALID_MAIN_TAP;
    }

exit:
    SHR_FUNC_EXIT;
}


