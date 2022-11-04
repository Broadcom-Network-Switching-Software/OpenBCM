
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_UTILS

#include <soc/sand/shrextend/shrextend_debug.h>

#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/sand/sand_aux_access.h>
#include <soc/drv.h>
#include <soc/dnx/drv.h>
#include <soc/dnxc/drv.h>
#include <soc/dnxc/drv_dnxc_utils.h>
#include <bcm/time.h>
#include <shared/error.h>
#include <bcm_int/common/time.h>
#include <soc/feature.h>
#include <soc/counter.h>
#include <soc/dnxc/dnxc_verify.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/recovery/generic_state_journal.h>
#include <bcm_int/dnx/dram/hbmc/hbmc.h>
#endif
#ifdef BCM_ACCESS_SUPPORT
#include <soc/dnxc/dnxc_access.h>
#endif
#include <soc/dnxc/dnxc_mem.h>

#include <soc/dnx/dnx_fifodma.h>

#ifdef BCM_DNX_SUPPORT

typedef struct dnx_algo_port_er_soc_info_port_rollback_s
{
    bcm_port_t logical_port;
    soc_ptype_e ptype;
} dnx_algo_port_er_soc_info_port_rollback_t;
#endif

uint32
dnxc_drv_soc_flags_get(
    int unit)
{
    uint32 ret;

    ret = SOC_FLAGS_GET(unit);
    return (ret);
}

int
dnx_drv_soc_flags_set(
    int unit,
    uint32 value)
{
    int ret;

    ret = SOC_FLAGS_SET(unit, value);
    return (ret);
}

const char *
dnx_drv_soc_dev_name(
    int unit)
{

    const char *name = soc_cm_get_name(unit);
    return (name == NULL ? "<UNKNOWN>" : name);
}

int
dnx_drv_soc_block_in_list(
    int unit,
    int *blk,
    int type)
{
    int ret;

    ret = SOC_BLOCK_IN_LIST(unit, blk, type);
    return (ret);
}

shr_error_e
dnx_drv_soc_is_valid_block_instance(
    int unit,
    soc_block_types_t block_types,
    int block_instance,
    int *is_valid)
{
    shr_error_e ret;

    ret = soc_is_valid_block_instance(unit, block_types, block_instance, is_valid);
    return (ret);
}

int
dnx_drv_soc_block_is_type(
    int unit,
    int blk_idx,
    int *list)
{
    int ret;

    ret = SOC_BLOCK_IS_TYPE(unit, blk_idx, list);
    return (ret);
}

void *
dnx_drv_reg_info_ptr_get(
    int unit,
    int reg)
{
    void *ret;

    ret = (void *) (SOC_DRIVER(unit)->reg_info[(soc_reg_t) reg]);
    return (ret);
}

shr_error_e
dnx_drv_soc_event_unregister(
    int unit,
    bcm_switch_event_cb_t cb,
    void *userdata)
{
    shr_error_e ret;

    ret = soc_event_unregister(unit, (soc_event_cb_t) cb, userdata);
    return (ret);
}

shr_error_e
dnx_drv_soc_event_register(
    int unit,
    bcm_switch_event_cb_t cb,
    void *userdata)
{
    shr_error_e ret;

    ret = soc_event_register(unit, (soc_event_cb_t) cb, userdata);
    return (ret);
}

shr_error_e
dnx_drv_soc_event_generate(
    int unit,
    bcm_switch_event_t event,
    uint32 arg1,
    uint32 arg2,
    uint32 arg3)
{
    shr_error_e ret;

    ret = soc_event_generate(unit, (soc_switch_event_t) event, arg1, arg2, arg3);
    return (ret);
}

void
dnx_drv_soc_control_unlock(
    int unit)
{
    SOC_CONTROL_UNLOCK(unit);
    return;
}

void
dnx_drv_soc_control_lock(
    int unit)
{
    SOC_CONTROL_LOCK(unit);
    return;
}

int
dnx_drv_is_e_port(
    int unit,
    bcm_port_t port)
{
    int ret;
    ret = IS_E_PORT(unit, port);
    return (ret);
}

int
dnx_drv_is_hg_port(
    int unit,
    bcm_port_t port)
{
    int ret;
    ret = IS_HG_PORT(unit, port);
    return (ret);
}

int
dnx_drv_is_spi_subport_port(
    int unit,
    bcm_port_t port)
{
    int ret;
    ret = IS_SPI_SUBPORT_PORT(unit, port);
    return (ret);
}

int
dnx_drv_soc_controlled_counter_use(
    int unit,
    bcm_port_t port)
{
    int ret;
    ret = _SOC_CONTROLLED_COUNTER_USE(unit, port);
    return (ret);
}

void
dnx_drv_soc_detach(
    int unit,
    int do_detach)
{
    SOC_DETACH(unit, do_detach);
    return;
}

int
dnx_drv_soc_property_name_max(
    void)
{
    int ret;
    ret = SOC_PROPERTY_NAME_MAX;
    return (ret);
}

int
dnx_drv_ile_block(
    int unit,
    int instance)
{
    int ret;
    ret = ILE_BLOCK(unit, instance);
    return (ret);
}

int
dnx_drv_soc_reg_is_above_32(
    int unit,
    int reg)
{
    int ret;
    ret = SOC_REG_IS_ABOVE_32(unit, reg);
    return (ret);
}

int
dnx_drv_soc_reg_is_above_64(
    int unit,
    int reg)
{
    int ret;
    ret = SOC_REG_IS_ABOVE_64(unit, reg);
    return (ret);
}

int
dnx_drv_soc_reg_is_64(
    int unit,
    int reg)
{
    int ret;
    ret = SOC_REG_IS_64(unit, reg);
    return (ret);
}

int
dnx_drv_soc_reg_words(
    int unit,
    soc_reg_t reg)
{
    int reg_words;

    if (SOC_REG_IS_ABOVE_64(unit, reg))
    {
        reg_words = SOC_REG_ABOVE_64_INFO(unit, reg).size;
    }
    else if (SOC_REG_IS_64(unit, reg))
    {
        reg_words = 2;
    }
    else
    {
        reg_words = 1;
    }

    return reg_words;
}

int
dnx_drv_is_il_port(
    int unit,
    bcm_port_t port)
{
    int ret;

    ret = IS_IL_PORT(unit, port);
    return (ret);
}

int
dnx_drv_is_sfi_port(
    int unit,
    bcm_port_t port)
{
    int ret;

    ret = IS_SFI_PORT(unit, port);
    return (ret);
}

int
dnx_drv_soc_warm_boot(
    int unit)
{
    int ret;

    ret = SOC_WARM_BOOT(unit);
    return (ret);
}

int
dnx_drv_clport_block(
    int unit,
    int instance)
{
    int ret;

    ret = CLPORT_BLOCK(unit, instance);
    return (ret);
}

int
dnx_drv_cdport_block(
    int unit,
    int instance)
{
    int ret;

    ret = CDPORT_BLOCK(unit, instance);
    return (ret);
}

int
dnx_drv_dc3port_block(
    int unit,
    int instance)
{
    int ret;

    ret = DC3PORT_BLOCK(unit, instance);
    return (ret);
}

int
dnx_drv_soc_block_port(
    int unit,
    int block)
{
    soc_block_t ret;
    ret = SOC_BLOCK_PORT(unit, block);
    return (ret);
}

int
dnx_drv_soc_port_idx_info_blk(
    int unit,
    int port,
    int idx)
{
    int ret;
    ret = SOC_PORT_IDX_INFO(unit, port, idx).blk;
    return (ret);
}

int
dnx_drv_soc_core_default(
    void)
{
    int ret;
    ret = SOC_CORE_DEFAULT;
    return (ret);
}

int
time_interface_id_max(
    int unit)
{
    int ret;
    ret = TIME_INTERFACE_ID_MAX(unit);
    return (ret);
}

soc_custom_reg_access_t *
dnx_drv_soc_info_custom_reg_access_get(
    int unit)
{
    soc_custom_reg_access_t *ret;
    ret = &(SOC_INFO(unit).custom_reg_access);
    return (ret);
}

void *
dnx_drv_soc_info_framer_access_drv_get(
    int unit)
{
    return SOC_INFO(unit).framer_access_drv;
}

void
dnx_drv_soc_info_framer_access_drv_set(
    int unit,
    void *framer_access_drv)
{
    SOC_INFO(unit).framer_access_drv = framer_access_drv;
}

void *
dnx_drv_soc_info_flr_sw_linkscan_info_get(
    int unit)
{
    return SOC_INFO(unit).flr_sw_linkscan_info;
}

void
dnx_drv_soc_info_flr_sw_linkscan_info_set(
    int unit,
    void *flr_sw_linkscan_info)
{
    SOC_INFO(unit).flr_sw_linkscan_info = flr_sw_linkscan_info;
}

shr_error_e
soc_info_int_address_get(
    int unit,
    int_in_soc_info_e int_id,
    int **int_p_p)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (int_id)
    {
        case BLOCK_PORT_INT:
        {
            *int_p_p = &(SOC_INFO(unit).block_port[0]);
            break;
        }
        case PORT_TYPE_INT:
        {
            *int_p_p = &(SOC_INFO(unit).port_type[0]);
            break;
        }
        case PORT_L2P_MAPPING_INT:
        {
            *int_p_p = &(SOC_INFO(unit).port_l2p_mapping[0]);
            break;
        }
        case PORT_NUM_LANES_INT:
        {
            *int_p_p = &(SOC_INFO(unit).port_num_lanes[0]);
            break;
        }
        case PORT_P2L_MAPPING_INT:
        {
            *int_p_p = &(SOC_INFO(unit).port_p2l_mapping[0]);
            break;
        }
        case FABRIC_LOGICAL_PORT_BASE_INT:
        {
            *int_p_p = &(SOC_INFO(unit).fabric_logical_port_base);
            break;
        }
        case NUM_TIME_INTERFACE_INT:
        {
            *int_p_p = &(SOC_INFO(unit).num_time_interface);
            break;
        }
        case NUM_UCS_INT:
        {
            *int_p_p = &(SOC_INFO(unit).num_ucs);
            break;
        }
        case HBC_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).hbc_block[0]);
            break;
        }
        case FSRD_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).fsrd_block[0]);
            break;
        }
        case FMAC_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).fmac_block[0]);
            break;
        }

        case BRDC_HBC_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).brdc_hbc_block[0]);
            break;
        }
        case DCC_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).dcc_blocks[0]);
            break;
        }
        case HAS_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).has_block[0]);
            break;
        }
        case FASIC_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).fasic_block);
            break;
        }
        case FLEXEWP_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).flexewp_block);
            break;
        }
        case FSAR_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).fsar_block);
            break;
        }
        case FSCL_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).fscl_block);
            break;
        }
        case MACSEC_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).macsec_blocks[0]);
            break;
        }
        case CDMAC_BLOCK_INT:
        {
            *int_p_p = &(SOC_INFO(unit).cdmac_block[0]);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal int_id (%d). Must be between %d and %d", int_id,
                         FIRST_INT_IN_SOC_INFO_E, NUM_INT_IN_SOC_INFO_E - 1);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_info_uint8_address_get(
    int unit,
    uint8_in_soc_info_e uint8_id,
    uint8 **uint8_p_p)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (uint8_id)

    {
        case PORT_NAME_ALTER_VALID:
        {
            *uint8_p_p = &(SOC_INFO(unit).port_name_alter_valid[0]);
            break;
        }
        case BLOCK_VALID:
        {
            *uint8_p_p = (uint8 *) (&(SOC_INFO(unit).block_valid[0]));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal int_id (%d). Must be between %d and %d", uint8_id,
                         FIRST_UINT8_IN_SOC_INFO_E, NUM_UINT8_IN_SOC_INFO_E - 1);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_info_char_2dimensions_address_size_get(
    int unit,
    char2d_in_soc_info_e char_2d_id,
    int first_index,
    char **char_p_p,
    int *sizeof_second_dimension)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (char_2d_id)
    {
        case BLOCK_NAME:
        {
            if (first_index == SOC_BLOCK_ALL)
            {
                static char *wild_p = "*";
                *char_p_p = wild_p;
                if (sizeof_second_dimension != NULL)
                {
                    *sizeof_second_dimension = sal_strnlen(wild_p, sizeof(SOC_INFO(unit).block_name[0]));
                }
            }
            else
            {
                *char_p_p = &(SOC_INFO(unit).block_name[first_index][0]);
                if (sizeof_second_dimension != NULL)
                {
                    *sizeof_second_dimension = sizeof(SOC_INFO(unit).block_name[first_index]);
                }
            }
            break;
        }
        case PORT_NAME:
        {
            *char_p_p = &(SOC_INFO(unit).port_name[first_index][0]);
            if (sizeof_second_dimension != NULL)
            {
                *sizeof_second_dimension = sizeof(SOC_INFO(unit).port_name[first_index]);
            }
            break;
        }
        case PORT_NAME_ALTER:
        {
            *char_p_p = &(SOC_INFO(unit).port_name_alter[first_index][0]);
            if (sizeof_second_dimension != NULL)
            {
                *sizeof_second_dimension = sizeof(SOC_INFO(unit).port_name_alter[first_index]);
            }
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal char_2d_id (%d). Must be between %d and %d", char_2d_id,
                         FIRST_CHAR2D_IN_SOC_INFO_E, NUM_CHAR2D_IN_SOC_INFO_E - 1);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
soc_info_ptype_address_get(
    int unit,
    soc_ptype_e ptype,
    soc_ptype_t ** ptype_p_p)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (ptype)

    {
        case FE_PTYPE:

        {
            *ptype_p_p = &(SOC_INFO(unit).fe);
            break;
        }
        case GE_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).ge);
            break;
        }
        case CE_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).ce);
            break;
        }
        case CDE_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).cde);
            break;
        }
        case TPON_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).tpon);
            break;
        }
        case PON_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).pon);
            break;
        }
        case LLID_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).llid);
            break;
        }
        case XE_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).xe);
            break;
        }
        case LE_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).le);
            break;
        }

        case CC_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).cc);
            break;
        }
        case D3CE_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).d3c);
            break;
        }
        case IL_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).il);
            break;
        }
        case SCH_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).sch);
            break;
        }
        case HG_SUBPORT_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).hg_subport);
            break;
        }
        case HL_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).hl);
            break;
        }

        case ST_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).st);
            break;
        }
        case GX_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).gx);
            break;
        }
        case XL_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).xl);
            break;
        }
        case XLB0_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).xlb0);
            break;
        }
        case QSGMII_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).qsgmii);
            break;
        }

        case MXQ_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).mxq);
            break;
        }
        case XG_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).xg);
            break;
        }
        case XQ_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).xq);
            break;
        }

        case RSVD4_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).rsvd4);
            break;
        }

        case AXP_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).axp);
            break;
        }
        case HYPLITE_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).hyplite);
            break;
        }
        case MMU_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).mmu);
            break;
        }
        case SPI_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).spi);
            break;
        }
        case SPI_SUBPORT_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).spi);
            break;
        }

        case SCI_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).sci);
            break;
        }
        case SFI_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).sfi);
            break;
        }
        case REQ_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).req);
            break;
        }
        case TDM_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).tdm);
            break;
        }
        case RCY_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).rcy);
            break;
        }

        case CD_PTYPE:

        {
            *ptype_p_p = &(SOC_INFO(unit).cd);
            break;
        }
        case PORT_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).port);
            break;
        }
        case ETHER_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).ether);
            break;
        }
        case ALL_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).all);
            break;
        }
        case PP_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).pp);
            break;
        }

        case LP_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).lp);
            break;
        }
        case SUBTAG_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).subtag);
            break;
        }
        case INTP_PTYPE:

        {
            *ptype_p_p = &(SOC_INFO(unit).intp);
            break;
        }

        case LBGPORT_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).lbgport);
            break;
        }

        case FLEXE_CLIENT_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).flexe_client);
            break;
        }

        case FLEXE_MAC_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).flexe_mac);
            break;
        }

        case FLEXE_SAR_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).flexe_sar);
            break;
        }

        case FLEXE_PHY_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).flexe_phy);
            break;
        }

        case OTN_PHY_PTYPE:
        {
            *ptype_p_p = &(SOC_INFO(unit).otn);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal ptype (%d). Must be between %d and %d", ptype, FIRST_SOC_PTYPE_E,
                         NUM_SOC_PTYPE_E - 1);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
soc_is(
    int unit,
    soc_is_device_e soc_device)
{
    int ret;

    ret = FALSE;
    switch (soc_device)
    {
        case DNX_RAMON:
        {
            ret = SOC_IS_RAMON(unit);
            break;
        }
        case DNXF_DEVICE:
        {
            ret = SOC_IS_DNXF(unit);
            break;
        }
        case DNX_DEVICE:
        {
            ret = SOC_IS_DNX(unit);
            break;
        }
        case DNX2_DEVICE:
        {
            ret = SOC_IS_DNX2(unit);
            break;
        }
        case J2C_DEVICE:
        {
            ret = SOC_IS_J2C(unit);
            break;
        }
        case J2C_A0_DEVICE:
        {
            ret = SOC_IS_J2C_A0(unit);
            break;
        }
        case Q2A_DEVICE:
        {
            ret = SOC_IS_Q2A(unit);
            break;
        }
        case Q2A_B_DEVICE:
        {
            ret = SOC_IS_Q2A_B(unit);
            break;
        }
        case Q2A_B0_DEVICE:
        {
            ret = SOC_IS_Q2A_B0(unit);
            break;
        }
        case Q2A_B1_DEVICE:
        {
            ret = SOC_IS_Q2A_B1(unit);
            break;
        }
        case JERICHO2_A0_DEVICE:
        {
            ret = SOC_IS_JERICHO2_A0(unit);
            break;
        }
        case JERICHO2_B0_DEVICE:
        {
            ret = SOC_IS_JERICHO2_B0(unit);
            break;
        }
        case JERICHO2_ONLY_DEVICE:
        {
            ret = SOC_IS_JERICHO2(unit);
            break;
        }
        case JERICHO2_B1_ONLY:
        {
            ret = SOC_IS_JERICHO2_B1(unit);
            break;
        }
        case J2P_DEVICE:
        {
            ret = SOC_IS_J2P(unit);
            break;
        }

        case J2P_A0_DEVICE:
        {
            ret = SOC_IS_J2P_A0(unit);
            break;
        }

        case J2P_A1_DEVICE:
        {
            ret = SOC_IS_J2P_A1(unit);
            break;
        }

        case J2P_A2_DEVICE:
        {
            ret = SOC_IS_J2P_A2(unit);
            break;
        }

        case J2X_DEVICE:
        {
            ret = SOC_IS_J2X(unit);
            break;
        }

        default:
        {
            ret = FALSE;
            break;
        }
    }
    return (ret);
}

shr_error_e
soc_control_element_address_get(
    int unit,
    soc_control_e soc_control_element,
    void **soc_control_element_p_p)
{
    soc_control_t *soc;
    SHR_FUNC_INIT_VARS(unit);
    soc = SOC_CONTROL(unit);
    switch (soc_control_element)
    {
        case SWITCH_EVENT_NOMINAL_STORM:
        {
            *soc_control_element_p_p = &(soc->switch_event_nominal_storm);
            break;
        }
#ifdef BCM_WARM_BOOT_SUPPORT

        case SCACHE_DIRTY:
        {
            *soc_control_element_p_p = &(soc->scache_dirty);
            break;
        }

#endif
        case INTERRUPTS_INFO:
        {
            *soc_control_element_p_p = &(soc->interrupts_info);
            break;
        }
        case TIME_CALL_REF_COUNT:
        {
            *soc_control_element_p_p = &(soc->time_call_ref_count);
            break;
        }
        case SOC_TIME_CALLOUT:
        {
            *soc_control_element_p_p = &(soc->soc_time_callout);
            break;
        }
        case SOC_FLAGS:
        {
            *soc_control_element_p_p = &(soc->soc_flags);
            break;
        }
#ifdef INCLUDE_MEM_SCAN

        case MEM_SCAN_PID:
        {
            *soc_control_element_p_p = (void *) (&(soc->mem_scan_pid));
            break;
        }
        case MEM_SCAN_INTERVAL:
        {
            *soc_control_element_p_p = (void *) (&(soc->mem_scan_interval));
            break;
        }

#endif
        case COUNTER_INTERVAL:
        {
            *soc_control_element_p_p = (void *) (&(soc->counter_interval));
            break;
        }
        case CONTROLLED_COUNTERS:
        {
            *soc_control_element_p_p = (void *) (&(soc->controlled_counters));
            break;
        }
        case COUNTER_PBMP:
        {
            *soc_control_element_p_p = (void *) (&(soc->counter_pbmp));
            break;
        }
        case DMA_RBYT:
        {
            *soc_control_element_p_p = (void *) (&(soc->stat.dma_rbyt));
            break;
        }
        case DMA_RPKT:
        {
            *soc_control_element_p_p = (void *) (&(soc->stat.dma_rpkt));
            break;
        }
        case DMA_TBYT:
        {
            *soc_control_element_p_p = (void *) (&(soc->stat.dma_tbyt));
            break;
        }
        case DMA_TPKT:
        {
            *soc_control_element_p_p = (void *) (&(soc->stat.dma_tpkt));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal soc_control_element (%d). Must be between %d and %d",
                         soc_control_element, FIRST_SOC_CONTROL_E, NUM_SOC_CONTROL_E - 1);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_info_pbmp_address_get(
    int unit,
    soc_pbmp_e pbmp,
    pbmp_t ** pbmp_p_p)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (pbmp)

    {
        case CUSTOM_PORT_PBMP:
        {
            *pbmp_p_p = &(SOC_INFO(unit).custom_reg_access.custom_port_pbmp);
            break;
        }
        case ROE_COMPRESSION:
        {
            *pbmp_p_p = &(SOC_INFO(unit).roe_compression);
            break;
        }
        case BLOCK_BITMAP:
        {
            *pbmp_p_p = &(SOC_INFO(unit).block_bitmap[0]);
            break;
        }
        case PBMP_IL_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_IL_ALL(unit));
            break;
        }
        case PBMP_XL_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_XL_ALL(unit));
            break;
        }
        case PBMP_CE_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_CE_ALL(unit));
            break;
        }
        case PBMP_XE_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_XE_ALL(unit));
            break;
        }
        case PBMP_HG_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_HG_ALL(unit));
            break;
        }
        case PBMP_SFI_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_SFI_ALL(unit));
            break;
        }
        case PBMP_PORT_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_PORT_ALL(unit));
            break;
        }
        case PBMP_CMIC_BITMAP:
        {
            *pbmp_p_p = &(PBMP_CMIC(unit));
            break;
        }
        case PBMP_RCY_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_RCY_ALL(unit));
            break;
        }
        case PBMP_E_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_E_ALL(unit));
            break;
        }
        case PBMP_GE_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_GE_ALL(unit));
            break;
        }
        case PBMP_ALL_BITMAP:
        {
            *pbmp_p_p = &(PBMP_ALL(unit));
            break;
        }
        case PBMP_CDE_BITMAP:
        {
            *pbmp_p_p = &(SOC_PORT_BITMAP(unit, cde));
            break;
        }
        case PBMP_D3CE_BITMAP:
        {
            *pbmp_p_p = &(SOC_PORT_BITMAP(unit, d3c));
            break;
        }
        case PBMP_LE_BITMAP:
        {
            *pbmp_p_p = &(SOC_PORT_BITMAP(unit, le));
            break;
        }
        case PBMP_CC_BITMAP:
        {
            *pbmp_p_p = &(SOC_PORT_BITMAP(unit, cc));
            break;
        }
        case PBMP_FLEXE_CLIENT_BITMAP:
        {
            *pbmp_p_p = &(SOC_PORT_BITMAP(unit, flexe_client));
            break;
        }
        case PBMP_FLEXE_MAC_BITMAP:
        {
            *pbmp_p_p = &(SOC_PORT_BITMAP(unit, flexe_mac));
            break;
        }
        case PBMP_FLEXE_SAR_BITMAP:
        {
            *pbmp_p_p = &(SOC_PORT_BITMAP(unit, flexe_sar));
            break;
        }
        case PBMP_FLEXE_PHY_BITMAP:
        {
            *pbmp_p_p = &(SOC_PORT_BITMAP(unit, flexe_phy));
            break;
        }
        case PBMP_OTN_PHY_BITMAP:
        {
            *pbmp_p_p = &(SOC_PORT_BITMAP(unit, otn));
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal pbmp (%d). Must be between %d and %d", pbmp, FIRST_SOC_PBMP_E,
                         NUM_SOC_PBMP_E - 1);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_drv_soc_block_info_port_block_address_get(
    int unit,
    soc_block_info_e soc_block_info,
    int port_block,
    int **soc_block_info_p_p)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (soc_block_info)
    {
        case TYPE:
        {
            *soc_block_info_p_p = &(SOC_BLOCK_INFO(unit, port_block).type);
            break;
        }
        case NUMBER:
        {
            *soc_block_info_p_p = &(SOC_BLOCK_INFO(unit, port_block).number);
            break;
        }
        case SCHAN:
        {
            *soc_block_info_p_p = &(SOC_BLOCK_INFO(unit, port_block).schan);
            break;
        }
        case CMIC:
        {
            *soc_block_info_p_p = &(SOC_BLOCK_INFO(unit, port_block).cmic);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Illegal soc_block_info (%d). Must be between %d and %d", soc_block_info,
                         FIRST_SOC_BLOCK_INFO_E, NUM_SOC_BLOCK_INFO_E - 1);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_drv_soc_block_info_block_address_get(
    int unit,
    int port_block,
    void **soc_block_info_p_p)
{
    SHR_FUNC_INIT_VARS(unit);
    *soc_block_info_p_p = (void *) &(SOC_BLOCK_INFO(unit, port_block));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_drv_soc_port_min(
    int unit,
    soc_ptype_e ptype,
    int *port_min)
{
    soc_ptype_t *ptype_p;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_info_ptype_address_get(unit, ptype, &ptype_p));
    *port_min = ptype_p->min;
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_drv_soc_port_max(
    int unit,
    soc_ptype_e ptype,
    int *port_max)
{
    soc_ptype_t *ptype_p;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_info_ptype_address_get(unit, ptype, &ptype_p));
    *port_max = ptype_p->max;
exit:
    SHR_FUNC_EXIT;
}
#ifdef BCM_DNX_SUPPORT

static shr_error_e
dnx_algo_port_er_soc_info_port_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *is_add = NULL;
    dnx_algo_port_er_soc_info_port_rollback_t *rollback_entry = NULL;

    SHR_FUNC_INIT_VARS(unit);

    is_add = (int *) metadata;
    rollback_entry = (dnx_algo_port_er_soc_info_port_rollback_t *) payload;

    if (*is_add)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_remove(unit, rollback_entry->ptype, rollback_entry->logical_port));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_soc_info_port_add(unit, rollback_entry->ptype, rollback_entry->logical_port));
    }
exit:
    SHR_FUNC_EXIT;
}
#endif

shr_error_e
dnx_algo_port_soc_info_port_add(
    int unit,
    soc_ptype_e ptype,
    bcm_port_t logical_port)
{
    soc_ptype_t *ptype_p;
#ifdef BCM_DNX_SUPPORT
    dnx_algo_port_er_soc_info_port_rollback_t rollback_entry;
    int is_add = 1;
#endif
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_info_ptype_address_get(unit, ptype, &ptype_p));

    if (!BCM_PBMP_MEMBER(ptype_p->bitmap, logical_port))
    {
        BCM_PBMP_PORT_ADD(ptype_p->bitmap, logical_port);
        if ((ptype_p->min) > logical_port || (ptype_p->min) == -1)
        {
            (ptype_p->min) = logical_port;
        }
        if ((ptype_p->max) < logical_port)
        {
            (ptype_p->max) = logical_port;
        }
#ifdef BCM_DNX_SUPPORT
        rollback_entry.logical_port = logical_port;
        rollback_entry.ptype = ptype;
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                            (uint8 *) &is_add,
                                                            sizeof(int),
                                                            (uint8 *) &rollback_entry,
                                                            sizeof(dnx_algo_port_er_soc_info_port_rollback_t),
                                                            &dnx_algo_port_er_soc_info_port_rollback, FALSE));
#endif
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_port_soc_info_port_remove(
    int unit,
    soc_ptype_e ptype,
    bcm_port_t logical_port)
{
    soc_ptype_t *ptype_p;
#ifdef BCM_DNX_SUPPORT
    dnx_algo_port_er_soc_info_port_rollback_t rollback_entry;
    int is_add = 0;
#endif
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(soc_info_ptype_address_get(unit, ptype, &ptype_p));
    if (BCM_PBMP_MEMBER(ptype_p->bitmap, logical_port))
    {
        BCM_PBMP_PORT_REMOVE(ptype_p->bitmap, logical_port);
        if (logical_port == ptype_p->min)
        {
            _SHR_PBMP_FIRST(ptype_p->bitmap, (ptype_p->min));
        }
        if (logical_port == ptype_p->max)
        {
            _SHR_PBMP_LAST(ptype_p->bitmap, (ptype_p->max));
        }
#ifdef BCM_DNX_SUPPORT
        rollback_entry.logical_port = logical_port;
        rollback_entry.ptype = ptype;
        SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                            (uint8 *) &is_add,
                                                            sizeof(int),
                                                            (uint8 *) &rollback_entry,
                                                            sizeof(dnx_algo_port_er_soc_info_port_rollback_t),
                                                            &dnx_algo_port_er_soc_info_port_rollback, FALSE));
#endif
    }
exit:
    SHR_FUNC_EXIT;
}

uint32
dnx_drv_soc_property_get(
    int unit,
    const char *name,
    uint32 defl)
{
    uint32 ret;
    ret = soc_property_get(unit, name, defl);
    return ret;
}

uint32
dnx_drv_soc_property_suffix_num_get(
    int unit,
    int num,
    const char *name,
    const char *suffix,
    uint32 defl)
{
    uint32 ret;
    ret = soc_property_suffix_num_get(unit, num, name, suffix, defl);
    return ret;
}

char *
dnx_drv_soc_property_suffix_num_str_get(
    int unit,
    int num,
    const char *name,
    const char *suffix)
{
    char *ret;

    ret = soc_property_suffix_num_str_get(unit, num, name, suffix);
    return (ret);
}

char *
dnx_drv_soc_property_get_str(
    int unit,
    const char *name)
{
    char *ret;

    ret = soc_property_get_str(unit, name);
    return (ret);
}

uint32
dnx_drv_soc_property_port_get(
    int unit,
    soc_port_t port,
    const char *name,
    uint32 defl)
{
    uint32 ret;
    ret = soc_property_port_get(unit, port, name, defl);
    return (ret);
}

uint32
dnx_drv_soc_property_suffix_num_get_only_suffix(
    int unit,
    int num,
    const char *name,
    const char *suffix,
    uint32 defl)
{
    uint32 ret;
    ret = soc_property_suffix_num_get_only_suffix(unit, num, name, suffix, defl);
    return (ret);
}

char *
dnx_drv_soc_property_suffix_num_only_suffix_str_get(
    int unit,
    int num,
    const char *name,
    const char *suffix)
{
    char *ret;
    ret = soc_property_suffix_num_only_suffix_str_get(unit, num, name, suffix);
    return (ret);
}

char *
dnx_drv_soc_property_port_get_str(
    int unit,
    soc_port_t port,
    const char *name)
{
    char *ret;
    ret = soc_property_port_get_str(unit, port, name);
    return (ret);
}

int
dnx_drv_soc_reg_is_valid(
    int unit,
    int reg)
{
    int ret;
    ret = SOC_REG_IS_VALID(unit, reg);
    return (ret);
}

int
dnx_drv_soc_feature(
    int unit,
    soc_feature_t soc_feature)
{
    int ret;
    ret = soc_feature(unit, soc_feature);
    return (ret);
}

void
dnx_drv_soc_feature_clear(
    int unit,
    soc_feature_t soc_feature)
{
    SOC_FEATURE_CLEAR(unit, soc_feature);
    return;
}

uint32
dnx_drv_soc_intr_enable(
    int unit,
    uint32 mask)
{
    uint32 ret;
    ret = soc_intr_enable(unit, mask);
    return (ret);
}

uint32
dnx_drv_soc_intr_disable(
    int unit,
    uint32 mask)
{
    uint32 ret;
    ret = soc_intr_disable(unit, mask);
    return (ret);
}

int
dnx_drv_soc_port_valid(
    int unit,
    bcm_gport_t gport)
{
    int ret;

    ret = SOC_PORT_VALID(unit, gport);
    return (ret);
}

int
dnx_drv_soc_mem_block_min(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_BLOCK_MIN(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_block_max(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_BLOCK_MAX(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_index_min(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_INFO(unit, mem).index_min;
    return (ret);
}

int
dnx_drv_soc_mem_index_max(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = soc_mem_index_max(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_is_array(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_IS_ARRAY(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_numels(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_NUMELS(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_first_array_index(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_FIRST_ARRAY_INDEX(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_entry_words(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = soc_mem_entry_words(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_is_valid(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_IS_VALID(unit, mem);
    return (ret);
}

int
dnx_drv_soc_unit_valid(
    int unit)
{
    int ret;

    ret = SOC_UNIT_VALID(unit);
    return (ret);
}

uint16
dnx_drv_soc_pci_vendor(
    int unit)
{
    uint16 ret;

    ret = SOC_PCI_VENDOR(unit);
    return (ret);
}

uint16
dnx_drv_soc_pci_device(
    int unit)
{
    uint16 ret;

    ret = SOC_PCI_DEVICE(unit);
    return (ret);
}

uint8
dnx_drv_soc_pci_revision(
    int unit)
{
    uint8 ret;

    ret = SOC_PCI_REVISION(unit);
    return (ret);
}

int
dnx_drv_soc_mem_flags(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = soc_mem_flags(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_words(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_WORDS(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_bytes(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_BYTES(unit, mem);
    return (ret);
}

int
dnx_drv_soc_mem_table_bytes(
    int unit,
    soc_mem_t mem)
{
    int ret;

    ret = SOC_MEM_TABLE_BYTES(unit, mem);
    return (ret);
}

int
dnxc_drv_soc_f_all_modules_inited(
    void)
{
    int ret;

    ret = SOC_F_ALL_MODULES_INITED;
    return (ret);
}

int
dnxc_drv_soc_f_inited(
    void)
{
    int ret;

    ret = SOC_F_INITED;
    return (ret);
}

int
dnxc_init_is_init_done_get(
    int unit)
{
    int ret;
    ret = (dnxc_drv_soc_flags_get(unit) & dnxc_drv_soc_f_all_modules_inited())? TRUE : FALSE;
    return (ret);
}

int
dnx_drv_mem_block_all(
    void)
{
    int ret;

    ret = MEM_BLOCK_ALL;
    return (ret);
}

#ifdef BCM_DNX_SUPPORT

shr_error_e
dnx_drv_soc_dnx_rcpu_init(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnx_rcpu_init(unit);
    return (ret);
}
#endif

shr_error_e
dnx_drv_soc_dnx_info_config(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnxc_info_config_init(unit);

    dnxc_verify_allowed_init(unit);

    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_info_config_deinit(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnxc_info_config_deinit(unit);
    return (ret);
}

#ifdef BCM_DNX_SUPPORT

shr_error_e
dnx_drv_soc_dnx_endian_config(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnxc_endian_config(unit);
    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_ring_config(
    int unit)
{
    shr_error_e ret;
#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        ret = access_cast_config_init(unit);
    }
    else
#endif
    {
        ret = soc_dnx_ring_config(unit);
    }
    return (ret);
}

int
dnx_drv_soc_dnx_init_reset_cmic_regs(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnx_init_reset_cmic_regs(unit);
    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_iproc_config(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnx_iproc_config(unit);
    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_soft_reset(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnx_soft_reset(unit);
    return (ret);
}

shr_error_e
dnx_drv_soc_redo_soft_reset_soft_init(
    int unit,
    int perform_soft_reset,
    int without_fabric,
    int without_ile)
{
    shr_error_e ret;

    ret = dnx_redo_soft_reset_soft_init(unit, perform_soft_reset, without_fabric, without_ile);
    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_dma_init(
    int unit)
{
    shr_error_e ret;
#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        access_runtime_info_t *runtime_info = ACCESS_RUNTIME_INFO(unit);
        ret = access_dnxc_dma_init(unit);
        if (ret < 0)
        {
            return ret;
        }

        SOC_CONTROL(unit)->max_sbusdma_channels = SOC_DNXC_MAX_SBUSDMA_CHANNELS;
        SOC_CONTROL(unit)->tdma_ch = SOC_DNXC_TDMA_CHANNEL;
        SOC_CONTROL(unit)->tslam_ch = SOC_DNXC_TSLAM_CHANNEL;
        SOC_CONTROL(unit)->desc_ch = SOC_DNXC_DESC_CHANNEL;

        ret = soc_sbusdma_init(unit, runtime_info->sbusDmaDescTimeout, runtime_info->sbusDmaDescIntrEnb);
        if (ret < 0)
        {
            return ret;
        }

        ret = soc_dnx_fifodma_init(unit);
        if (ret < 0)
        {
            return ret;
        }
    }
    else
#endif

        ret = soc_dnx_dma_init(unit);
    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_dma_deinit(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnx_dma_deinit(unit);
    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_mutexes_init(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnxc_mutexes_init(unit);
    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_mutexes_deinit(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnxc_mutexes_deinit(unit);
    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_perform_bist_tests(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnx_perform_bist_tests(unit);
    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_unmask_mem_writes(
    int unit)
{
    shr_error_e ret;

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        ret = access_dnx_unmask_mem_writes(unit);
    }
    else
#endif
    {
        ret = soc_dnx_unmask_mem_writes(unit);
    }
    return (ret);
}

#endif

int
dnxc_drv_soc_dnx_drv_sbus_broadcast_config(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(access_dnxc_sbus_broadcast_config(unit));
    }
    else
#endif
    {
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(soc_dnx_drv_sbus_broadcast_config(unit));
        }
#endif
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT

shr_error_e
dnx_drv_soc_dnx_mark_not_inited(
    int unit)
{
    shr_error_e ret;

    ret = soc_dnx_mark_not_inited(unit);
    return (ret);
}

static shr_error_e
dnx_drv_soc_dnx_soft_init_restore_mrs(
    int unit,
    int without_fabric,
    int without_ile)
{
    dnx_hbmc_mrs_values_t mrs_values;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_hbmc_soft_init_mrs_save(unit, &mrs_values));

    SHR_IF_ERR_EXIT(soc_dnx_soft_init(unit, without_fabric, without_ile));

    SHR_IF_ERR_EXIT(dnx_hbmc_soft_init_mrs_restore(unit, &mrs_values));

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_drv_soc_dnx_soft_init(
    int unit)
{
    shr_error_e ret;

    ret = dnx_drv_soc_dnx_soft_init_restore_mrs(unit, 0, 0);

    return (ret);
}

shr_error_e
dnx_drv_soc_dnx_soft_init_no_fabric(
    int unit)
{
    shr_error_e ret;

    ret = dnx_drv_soc_dnx_soft_init_restore_mrs(unit, 1, 0);

    return (ret);
}
int
dnx_drv_soc_dnx_read_hbm_temp(
    const int unit,
    const unsigned hbm_number,
    uint32 *out_temp)
{
    SHR_FUNC_INIT_VARS(unit);
    if (SOC_IS_J2C(unit))
    {
        if (hbm_number)
        {
            SHR_ERR_EXIT(SOC_E_PARAM, "device has only one HBM.\n");
        }
        SHR_IF_ERR_EXIT(soc_read_j2c_hbm_temp(unit, out_temp));
    }
    else if (SOC_IS_JERICHO2(unit))
    {
        if (hbm_number > 1)
        {
            SHR_ERR_EXIT(SOC_E_PARAM, "device has only two HBMs.\n");
        }
        SHR_IF_ERR_EXIT(soc_read_jr2_hbm_temp(unit, hbm_number, out_temp));
    }
    else
    {
        SHR_ERR_EXIT(SOC_E_PARAM, "Device not supported.\n");
    }

exit:
    SHR_FUNC_EXIT;
}

#endif

shr_error_e
dnxc_drv_soc_block_enable_set(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_ACCESS_SUPPORT
    if (ACCESS_IS_INITIALIZED(unit))
    {
        SHR_IF_ERR_EXIT(access_dnxc_blocks_disable_set(unit));
    }
    else
#endif
    {
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(soc_dnx_block_enable_set(unit));
        }
#endif
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT

sal_vaddr_t
dnx_drv_soc_dcb_addr_get(
    int unit,
    dcb_t * dcb)
{
    return (sal_vaddr_t) SOC_DCB(unit)->addr_get(unit, dcb);
}

uint32
dnx_drv_soc_dcb_xfercount_get(
    int unit,
    dcb_t * dcb)
{
    return SOC_DCB(unit)->xfercount_get(dcb);
}

void
dnx_drv_soc_dcb_hg_set(
    int unit,
    dcb_t * dcb,
    uint32 hg)
{
    SOC_DCB(unit)->hg_set(dcb, hg);
    return;
}

#endif
