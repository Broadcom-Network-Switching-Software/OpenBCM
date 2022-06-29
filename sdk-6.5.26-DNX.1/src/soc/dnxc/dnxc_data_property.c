
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DATA

#include <soc/dnxc/dnxc_data_property.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <soc/drv_prop.h>
#include <sal/appl/sal.h>
#include <shared/utilex/utilex_integer_arithmetic.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#endif

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

static shr_error_e
dnxc_data_property_serdes_tx_taps_legacy_read(
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
        nrz_len = sal_strnlen(nrz_prefix, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        pam4_prefix = "pam4";
        pam4_len = sal_strnlen(pam4_prefix, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
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
                         "port %d: invalid serdes_tx_taps configuration - signaling mode - \"nrz\" or \"pam4\" is missing",
                         port);
        }

        tx->tx_tap_mode = bcmPortPhyTxTapMode6Tap;

        prefix = ":";
        len = sal_strnlen(prefix, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
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

static int
dnxc_data_property_csv_dict_str_get(
    int unit,
    char *prop_str,
    char *key_str,
    int *is_found,
    char *value_str)
{
    char *token = NULL;
    char **key_value_tokens = NULL;
    uint32 nof_key_value_tokens = 0;
    int key_value_token_iter;
    char **value_tokens = NULL;
    uint32 nof_value_tokens = 0;

    SHR_FUNC_INIT_VARS(unit);

    *is_found = 0;

    key_value_tokens =
        utilex_str_split(prop_str, ",", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH, &nof_key_value_tokens);
    if (key_value_tokens == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Failed to split property string into key and value tokens\n");
    }

    for (key_value_token_iter = 0; key_value_token_iter < nof_key_value_tokens; ++key_value_token_iter)
    {
        int token_len = 0;
        int key_len = 0;

        token = key_value_tokens[key_value_token_iter];

        token_len = sal_strnlen(token, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (token_len == 0)
        {

            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "emtpy dictionaries are not allowed for check that thre are not two commas in a row\n");
        }

        value_tokens = utilex_str_split(token, ":", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH, &nof_value_tokens);
        if (value_tokens == NULL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Failed to split key-value '%s' into key and value\n", token);
        }

        if (nof_value_tokens != 2)
        {

            SHR_ERR_EXIT(_SHR_E_PARAM, "key=value '%s' pair is invalid. Both key and value must be present\n", token);
        }

        key_len = sal_strnlen(key_str, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);
        if (!sal_strncmp(value_tokens[0], key_str, UTILEX_MAX(key_len, token_len)))
        {

            sal_strncpy(value_str, value_tokens[1], SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH);

            if (sal_strnlen(value_tokens[1], SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "value must be provided\n");
            }

            *is_found = 1;
            SHR_EXIT();
        }

        if (value_tokens != NULL)
        {
            utilex_str_split_free(value_tokens, nof_value_tokens);
            value_tokens = NULL;
        }
    }

exit:

    if (key_value_tokens != NULL)
    {
        utilex_str_split_free(key_value_tokens, nof_key_value_tokens);
    }

    if (value_tokens != NULL)
    {
        utilex_str_split_free(value_tokens, nof_value_tokens);
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_data_property_serdes_tx_tap_val_read(
    int unit,
    char *propval,
    char *arg_name,
    int is_mandatory,
    int defl,
    int *is_found,
    int *value)
{
    char *propval_tmp = NULL;
    char value_str[SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH + 1] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    *is_found = 0;

    *value = defl;
    value_str[SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH] = '\0';

    SHR_IF_ERR_EXIT(dnxc_data_property_csv_dict_str_get(unit, propval, arg_name, is_found, value_str));

    if (*is_found)
    {
        *value = sal_ctoi(value_str, &propval_tmp);
    }
    else if (is_mandatory && !(*is_found))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "'%s' key is mandatory and is missing from soc property '%s' value", arg_name,
                     propval);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnxc_data_property_serdes_tx_taps_dict_read(
    int unit,
    int port,
    char *propval,
    bcm_port_phy_tx_t * tx)
{
    char value_str[SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH + 1] = { 0 };
    int is_found = 0, is_ext_tap_found = 0;

    SHR_FUNC_INIT_VARS(unit);

    bcm_port_phy_tx_t_init(tx);

    if (propval)
    {

        SHR_IF_ERR_EXIT(dnxc_data_property_csv_dict_str_get(unit, propval, "mode", &is_found, value_str));
        if (!is_found)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: invalid serdes_tx_taps configuration - signaling mode - \"nrz\" or \"pam4\" is missing",
                         port);
        }
        else if (!sal_strncasecmp("nrz", value_str, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
        {
            tx->signalling_mode = bcmPortPhySignallingModeNRZ;
        }
        else if (!sal_strncasecmp("pam4", value_str, SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH))
        {
            tx->signalling_mode = bcmPortPhySignallingModePAM4;
        }

        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_tap_val_read(unit, propval, "pre3", 0, 0, &is_found, &tx->pre3));
        is_ext_tap_found |= is_found;

        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_tap_val_read(unit, propval, "pre2", 0, 0, &is_found, &tx->pre2));
        is_ext_tap_found |= is_found;

        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_tap_val_read(unit, propval, "pre", 0, 0, &is_found, &tx->pre));

        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_tap_val_read(unit, propval, "main", 1, 0, &is_found, &tx->main));

        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_tap_val_read(unit, propval, "post", 0, 0, &is_found, &tx->post));

        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_tap_val_read(unit, propval, "post2", 0, 0, &is_found, &tx->post2));
        is_ext_tap_found |= is_found;

        SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_tap_val_read(unit, propval, "post3", 0, 0, &is_found, &tx->post3));
        is_ext_tap_found |= is_found;

        tx->tx_tap_mode = is_ext_tap_found ? bcmPortPhyTxTapMode6Tap : bcmPortPhyTxTapMode3Tap;
    }
    else
    {

        tx->main = DNXC_PORT_TX_FIR_INVALID_MAIN_TAP;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_data_property_serdes_tx_taps_read(
    int unit,
    int port,
    char *propval,
    bcm_port_phy_tx_t * tx)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNXF_SUPPORT
    if (soc_is(unit, DNXF_DEVICE))
    {
        if (dnxf_data_port.
            general.feature_get(unit, dnxf_data_port_general_is_serdes_tx_soc_prop_dict_format_supported))
        {
            SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_dict_read(unit, port, propval, tx));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_legacy_read(unit, port, propval, tx));
        }
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if (soc_is(unit, DNX_DEVICE))
    {
        if (dnx_data_port.general.feature_get(unit, dnx_data_port_general_is_serdes_tx_soc_prop_dict_format_supported))
        {
            SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_dict_read(unit, port, propval, tx));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnxc_data_property_serdes_tx_taps_legacy_read(unit, port, propval, tx));
        }
    }
#endif

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}
