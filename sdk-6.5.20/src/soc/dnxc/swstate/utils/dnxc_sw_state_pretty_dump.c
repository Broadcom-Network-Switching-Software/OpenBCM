
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWSTATEDNX

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnxc/swstate/dnx_sw_state_pretty_dump.h>

#if defined(DNX_SW_STATE_DIAGNOSTIC)

shr_error_e
dnx_sw_state_pretty_print(
    int unit,
    dnx_sw_state_pretty_print_t * print_cb_data,
    void* data)
{
    int sub_struct_offset;
    char string_to_print[SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE] = { 0 };
    DNXC_SW_STATE_INIT_FUNC_DEFS;

    DNX_SW_STATE_PRINT(unit, "\n\t\t\t      ");

    sub_struct_offset = 0;
    sub_struct_offset = print_cb_data->sub_struct_level;
    while (sub_struct_offset--)
    {
        DNX_SW_STATE_PRINT(unit, "  ");
    }
    DNX_SW_STATE_PRINT(unit, "|");

    if (print_cb_data->name != NULL)
    {
        DNX_SW_STATE_PRINT(unit, " Field Name :");
        if (print_cb_data->is_arr_flag)
        {
            sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, "%s[%d]", print_cb_data->name,
                        print_cb_data->arr_index);
            DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
        }
        else
        {
            DNX_SW_STATE_PRINT(unit, " %s |", print_cb_data->name);
        }
    }

    DNX_SW_STATE_PRINT(unit, " Value :");
    switch (print_cb_data->type)
    {
        case SW_STATE_PRETTY_PRINT_TYPE_UINT8:
        {
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string, *(uint8 *) data);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %u |", *(uint8 *) data);
            }
            break;
        }
        case SW_STATE_PRETTY_PRINT_TYPE_UINT16:
        {
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string, *(uint16 *) data);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %u |", *(uint16 *) data);
            }
            break;
        }
        case SW_STATE_PRETTY_PRINT_TYPE_UINT32:
        {
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string, *(uint32 *) data);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %u |", *(uint32 *) data);
            }
            break;
        }
        case SW_STATE_PRETTY_PRINT_TYPE_CHAR:
        {
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string, *(char *) data);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %c |", *(char *) data);
            }
            break;
        }
        case SW_STATE_PRETTY_PRINT_TYPE_SHORT:
        {
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string, *(short *) data);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %hu |", *(short *) data);
            }
            break;
        }
        case SW_STATE_PRETTY_PRINT_TYPE_INT:
        {
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string, *(int *) data);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %d |", *(int *) data);
            }
            break;
        }
        case SW_STATE_PRETTY_PRINT_TYPE_MAC:
        {
            bcm_mac_t mac;
            sal_memcpy(&mac, data, sizeof(bcm_mac_t));
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string, mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %02X:%02X:%02X:%02X:%02X:%02X |", mac[5], mac[4], mac[3], mac[2], mac[1], mac[0]);
            }
            break;
        }
        case SW_STATE_PRETTY_PRINT_TYPE_IPV4:
        {
            bcm_ip_t ipv4;
            sal_memcpy(&ipv4, data, sizeof(bcm_ip_t));
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string, (ipv4 >> 24) & 0xff,
                            (ipv4 >> 16) & 0xff, (ipv4 >> 8) & 0xff, ipv4 & 0xff);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %d.%d.%d.%d |",
                                   (ipv4 >> 24) & 0xff, (ipv4 >> 16) & 0xff, (ipv4 >> 8) & 0xff, ipv4 & 0xff);
            }
            break;
        }
        case SW_STATE_PRETTY_PRINT_TYPE_IPV6:
        {
            bcm_ip6_t ipv6;
            sal_memcpy(ipv6, data, sizeof(ipv6));
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string,
                            (((uint16) ipv6[0] << 8) | ipv6[1]), (((uint16) ipv6[2] << 8) | ipv6[3]),
                            (((uint16) ipv6[4] << 8) | ipv6[5]), (((uint16) ipv6[6] << 8) | ipv6[7]),
                            (((uint16) ipv6[8] << 8) | ipv6[9]), (((uint16) ipv6[10] << 8) | ipv6[11]),
                            (((uint16) ipv6[12] << 8) | ipv6[13]), (((uint16) ipv6[14] << 8) | ipv6[15]));
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %x:%x:%x:%x:%x:%x:%x:%x |",
                                   (((uint16) ipv6[0] << 8) | ipv6[1]), (((uint16) ipv6[2] << 8) | ipv6[3]),
                                   (((uint16) ipv6[4] << 8) | ipv6[5]), (((uint16) ipv6[6] << 8) | ipv6[7]),
                                   (((uint16) ipv6[8] << 8) | ipv6[9]), (((uint16) ipv6[10] << 8) | ipv6[11]),
                                   (((uint16) ipv6[12] << 8) | ipv6[13]), (((uint16) ipv6[14] << 8) | ipv6[15]));
            }
            break;
        }
        case SW_STATE_PRETTY_PRINT_TYPE_STRING:
        {
            if (print_cb_data->format_string != NULL)
            {
                sal_snprintf(string_to_print, SW_STATE_PRETTY_PRINT_MAX_PRINT_STRING_SIZE, print_cb_data->format_string, (char *) data);
                DNX_SW_STATE_PRINT(unit, " %s |", string_to_print);
            }
            else
            {
                DNX_SW_STATE_PRINT(unit, " %s |", (char *) data);
            }
            break;
        }
        default:
        {
            DNX_SW_STATE_PRINT(unit, " Invalid Type. |");
            break;
        }
    }

    if (print_cb_data->comment != NULL)
    {
        DNX_SW_STATE_PRINT(unit, " Info: %s |", print_cb_data->comment);
    }

    DNX_SW_STATE_PRINT(unit, "\n");

    DNXC_SW_STATE_FUNC_RETURN;
}

void
dnx_sw_state_pretty_print_example(
    int unit,
    void *data)
{
    dnx_sw_state_pretty_print_example_t *example = (dnx_sw_state_pretty_print_example_t *) data;

    SW_STATE_PRETTY_PRINT_INIT_VARIABLES();

    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_INT, "Integer Value", example->integer, NULL, "0x%08x");
    SW_STATE_PRETTY_PRINT_ADD_LINE(SW_STATE_PRETTY_PRINT_TYPE_MAC, "Mac Adress", example->mac, NULL, NULL);

    SW_STATE_PRETTY_PRINT_FINISH();
}


#else 
typedef int make_iso_compilers_happy;
#endif
