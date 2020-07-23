/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Module: Field Processor APIs
 *
 * Purpose:
 *     'Field Processor' (FP) API for Dune Packet Processor devices
 *      Utilities functions for other field modules
 */

/*
 *  The functions in this module deal with programmable data qualifiers on PPD
 *  devices.  They are invoked by the main field module when certain things
 *  occur, such as calls that explicitly involve data qualifiers.
 */

/*
 *  Everything in this file assumes it is safely called behind a lock.
 */

/*
 *  Exported functions have their descriptions in the field_int.h file.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FP
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <shared/swstate/sw_state_sync_db.h>
#include <bcm_int/common/debug.h>

#include <bcm/types.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/field.h>
#include <bcm/stg.h>

#include <soc/drv.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <bcm_int/dpp/field_int.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/utils.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_fp_key.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_flp_init.h>
#include <shared/bitop.h>

/*
 *  For now, if _BCM_DPP_FIELD_WARM_BOOT_SUPPORT is FALSE, disable warm boot
 *  support here.
 */
#ifdef BCM_WARM_BOOT_SUPPORT
#if !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT
#undef BCM_WARM_BOOT_SUPPORT
#endif  /* BCM_WARM_BOOT_SUPPORT */
#endif /* !_BCM_DPP_FIELD_WARM_BOOT_SUPPORT */

/******************************************************************************
 *
 *  Functions and data shared with other field modules
 */


CONST char* CONST _bcm_dpp_field_qual_name[bcmFieldQualifyCount] = BCM_FIELD_QUALIFY_STRINGS;
CONST char* CONST _bcm_dpp_field_action_name[bcmFieldActionCount] = BCM_FIELD_ACTION_STRINGS;
CONST char* CONST _bcm_dpp_field_group_mode_name[bcmFieldGroupModeCount] = BCM_FIELD_GROUP_MODE_STRINGS;
CONST char* CONST _bcm_dpp_field_stats_names[bcmFieldStatCount] = BCM_FIELD_STAT;
CONST char* CONST _bcm_dpp_field_control_names[bcmFieldControlCount] = BCM_FIELD_CONTROL_STRINGS;
CONST char* CONST _bcm_dpp_field_data_offset_base_names[bcmFieldDataOffsetBaseCount] = BCM_FIELD_DATA_OFFSET_BASE;
CONST char* CONST _bcm_dpp_field_header_format_names[bcmFieldHeaderFormatCount] = BCM_FIELD_HEADER_FORMAT;
CONST char* CONST _bcm_dpp_field_header_format_extension_names[bcmFieldHeaderFormatExtensionCount] = BCM_FIELD_HEADER_FORMAT_EXTENSION;
CONST char* CONST _bcm_dpp_field_entry_type_names[_bcmDppFieldEntryTypeCount] = _BCM_DPP_FIELD_ENTRY_TYPE_STRINGS;
CONST char* CONST _bcm_dpp_field_stage_name[bcmFieldStageCount] = BCM_FIELD_STAGE_STRINGS;

int
_bcm_dpp_ppd_qual_bits(int unit,
                       _bcm_dpp_field_stage_idx_t stage,
                       SOC_PPC_FP_QUAL_TYPE ppdQual,
                       unsigned int *exposed,
                       unsigned int *hardwareBestCase,
                       unsigned int *hardwareWorstCase)
{
    uint32 ppdExposed;
    uint32 ppdHardwareBestCase;
    uint32 ppdHardwareWorstCase;
    uint32 ppdr;
    uint32 found;
    int result;
    BCMDNX_INIT_FUNC_DEFS;


    ppdr = arad_pp_fp_qual_length_get(unit,
                                      _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).hwStageId,
                                      ppdQual,
                                      &found,
                                      &ppdHardwareBestCase,
                                      &ppdHardwareWorstCase,
                                      &ppdExposed);
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to resolve PPD stage"
                                          " %u qualifier type %d (%s) length:"
                                          " %d (%s)"),
                         unit,
                         stage,
                         ppdQual,
                         SOC_PPC_FP_QUAL_TYPE_to_string(ppdQual),
                         result,
                         _SHR_ERRMSG(result)));
    if (!found) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unable to find stage %u"
                                           " PPD qualifier %d (%s)"),
                          unit,
                          stage,
                          ppdQual,
                          SOC_PPC_FP_QUAL_TYPE_to_string(ppdQual)));
    }
    if (exposed) {
        *exposed = ppdExposed;
    }
    if (hardwareBestCase) {
        *hardwareBestCase = ppdHardwareBestCase;
    }
    if (hardwareWorstCase) {
        *hardwareWorstCase = ppdHardwareWorstCase;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_ppd_action_bits(int unit,
                         _bcm_dpp_field_stage_idx_t stage,
                         SOC_PPC_FP_ACTION_TYPE ppdAct,
                         unsigned int *bits)
{
    uint32 ppdBits;
    uint32 ppdLsbEgress;
    uint32 ppdr;
    int result;
    BCMDNX_INIT_FUNC_DEFS;


    ppdr = arad_pmf_db_fes_action_size_get_unsafe(unit,
                                                  ppdAct,
                                                  _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).hwStageId,
                                                  &ppdBits,
                                                  &ppdLsbEgress);
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to resolve PPD"
                                          " action type %d (%s) length:"
                                          " %d (%s)"),
                         unit,
                         ppdAct,
                         SOC_PPC_FP_ACTION_TYPE_to_string(ppdAct),
                         result,
                         _SHR_ERRMSG(result)));
    if (bits) {
        *bits = ppdBits;
    }
exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BROADCOM_DEBUG
/*
 *  Function
 *    _bcm_dpp_field_qset_dump
 *  Purpose
 *    Dump qualifier list from a qset
 *  Parameters
 *    (in) bcm_field_qset_t qset = the qset to dump
 *    (in) char *prefix = line prefix
 *  Returns
 *    nothing
 *  Notes
 *    No error checking or locking is done here.
 */
void
_bcm_dpp_field_qset_dump(const bcm_field_qset_t qset,
                         const char *prefix)
{
    bcm_field_qualify_t qualifier;
    unsigned int column = 0;
    unsigned int size;
    unsigned int temp;
	int unit = BSL_UNIT_UNKNOWN;

    /* for each qualifier potentially in the qset  */
    for (qualifier = 0; qualifier < bcmFieldQualifyCount; qualifier++) {
        /* if that qualifier actually is in the qset */
        if (BCM_FIELD_QSET_TEST(qset, qualifier)) {
            /* display the qualifier */
            if (0 == column) {
                /* just starting out */
                LOG_CLI((BSL_META_U(unit,
                                    "%s%s"),
                         prefix,
                         _bcm_dpp_field_qual_name[qualifier]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_qual_name[qualifier]));
            } else if ((3 + column +
                       sal_strlen(_bcm_dpp_field_qual_name[qualifier])) >=
                       _BCM_DPP_FIELD_PAGE_WIDTH) {
                /* this would wrap */
                LOG_CLI((BSL_META_U(unit,
                                    ",\n%s%s"),
                         prefix,
                         _bcm_dpp_field_qual_name[qualifier]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_qual_name[qualifier]));
            } else {
                /* this fits on the line */
                LOG_CLI((BSL_META_U(unit,
                                    ", %s"),
                         _bcm_dpp_field_qual_name[qualifier]));
                column += (2 +
                           sal_strlen(_bcm_dpp_field_qual_name[qualifier]));
            }
        } /* if (BCM_FIELD_QSET_TEST(thisGroup->qset, qualifier)) */
    } /* for (qualifier = 0; qualifier < bcmFieldQualifyCount; qualifier++) */
    for (qualifier = 0; qualifier < BCM_FIELD_USER_NUM_UDFS; qualifier++) {
        /* if that qualifier actually is in the qset */
        if (SHR_BITGET(qset.udf_map, qualifier)) {
            /* display the qualifier */
            for (temp = qualifier, size = 1;
                 temp >= 10;
                 temp /= 10, size++) {
                /* just size the number */
            }
            if (0 == column) {
                /* just starting out */
                LOG_CLI((BSL_META_U(unit,
                                    "%sDataField(%d)"), prefix, qualifier));
                column = sal_strlen(prefix) + 11 + size;
            } else if ((3 + column + 11 + size) >=
                       _BCM_DPP_FIELD_PAGE_WIDTH) {
                /* this would wrap */
                LOG_CLI((BSL_META_U(unit,
                                    ",\n%sDataField(%d)"), prefix, qualifier));
                column = sal_strlen(prefix) + 11 + size;
            } else {
                /* this fits on the line */
                LOG_CLI((BSL_META_U(unit,
                                    ", DataField(%d)"), qualifier));
                column += (13 + size);
            }
        } /* if (SHR_BITGET(qset.udf_map, qualifier)) */
    } /* for (all suported programmable qualifiers) */
    if (0 < column) {
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s(none)\n"), prefix));
    }
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
/*
 *  Function
 *    _bcm_dpp_field_aset_dump
 *  Purpose
 *    Dump action list from an aset
 *  Parameters
 *    (in) bcm_field_aset_t aset = the aset to dump
 *    (in) char *prefix = line prefix
 *  Returns
 *    nothing
 *  Notes
 *    No error checking or locking is done here.
 */
void
_bcm_dpp_field_aset_dump(const bcm_field_aset_t *aset,
                         const char *prefix)
{
    bcm_field_action_t action;
    unsigned int column = 0;
    int unit;

    unit = BSL_UNIT_UNKNOWN;

    /* for each action potentially in the aset  */
    for (action = 0; action < bcmFieldActionCount; action++) {
        /* if that action actually is in the aset */
        if (BCM_FIELD_ASET_TEST(*aset, action)) {
            /* display the action */
            if (0 == column) {
                /* just starting out */
                LOG_CLI((BSL_META_U(unit,
                                    "%s%s"),
                         prefix,
                         _bcm_dpp_field_action_name[action]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_action_name[action]));
            } else if ((3 + column +
                       sal_strlen(_bcm_dpp_field_action_name[action])) >=
                       _BCM_DPP_FIELD_PAGE_WIDTH) {
                /* this would wrap */
                LOG_CLI((BSL_META_U(unit,
                                    ",\n%s%s"),
                         prefix,
                         _bcm_dpp_field_action_name[action]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_action_name[action]));
            } else {
                /* this fits on the line */
                LOG_CLI((BSL_META_U(unit,
                                    ", %s"),
                         _bcm_dpp_field_action_name[action]));
                column += (2 +
                           sal_strlen(_bcm_dpp_field_action_name[action]));
            }
        } /* if (BCM_FIELD_ASET_TEST(thisGroup->aset, action)) */
    } /* for (action = 0; action < bcmFieldActionCount; action++) */
    if (0 < column) {
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s(none)\n"), prefix));
    }
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
void
_bcm_dpp_field_hfset_dump(const bcm_field_header_format_set_t hfset,
                          const char *prefix)
{
    bcm_field_header_format_t hf;
    unsigned int column = 0;
    int unit;

    unit = BSL_UNIT_UNKNOWN;

    /* for each qualifier potentially in the qset  */
    for (hf = 0; hf < bcmFieldHeaderFormatCount; hf++) {
        /* if that qualifier actually is in the qset */
        if (BCM_FIELD_HEADER_FORMAT_SET_TEST(hfset, hf)) {
            /* display the qualifier */
            if (0 == column) {
                /* just starting out */
                LOG_CLI((BSL_META_U(unit,
                                    "%s%s"),
                         prefix,
                         _bcm_dpp_field_header_format_names[hf]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_header_format_names[hf]));
            } else if ((3 + column +
                       sal_strlen(_bcm_dpp_field_header_format_names[hf])) >=
                       _BCM_DPP_FIELD_PAGE_WIDTH) {
                /* this would wrap */
                LOG_CLI((BSL_META_U(unit,
                                    ",\n%s%s"),
                         prefix,
                         _bcm_dpp_field_header_format_names[hf]));
                column = (sal_strlen(prefix) +
                          sal_strlen(_bcm_dpp_field_header_format_names[hf]));
            } else {
                /* this fits on the line */
                LOG_CLI((BSL_META_U(unit,
                                    ", %s"),
                         _bcm_dpp_field_header_format_names[hf]));
                column += (2 +
                           sal_strlen(_bcm_dpp_field_header_format_names[hf]));
            }
        } /* if (BCM_FIELD_HEADER_FORMAT_SET_TEST(thisGroup->aset, hf)) */
    } /* for (hf = 0; hf < bcmFieldHeaderFormatCount; hf++) */
    if (0 < column) {
        LOG_CLI((BSL_META_U(unit,
                            "\n")));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s(none)\n"), prefix));
    }
}
#endif /* def BROADCOM_DEBUG */

/* Conversion from PPD Trap id to HW trap id */
int
_bcm_dpp_field_trap_ppd_to_hw(int unit,
                              int trap_id,
                              uint32 *hwTrapCode)
{
    uint32 ppdr;
    int result;
    SOC_PPC_TRAP_CODE ppdTrapCode;
    SOC_PPC_TRAP_CODE_INTERNAL intTrapCode_arad;
    ARAD_SOC_REG_FIELD dc0_arad, dc1_arad; /* scratch space */

    BCMDNX_INIT_FUNC_DEFS ;

    *hwTrapCode = 0;


    if (!_BCM_RX_EXPOSE_HW_ID(unit)) 
    {

            BCMDNX_IF_ERR_EXIT_MSG(_bcm_rx_ppd_trap_code_from_trap_id(unit, trap_id, &ppdTrapCode),
                     (_BSL_BCM_MSG_NO_UNIT("Unable to resolve PPD"
                                      " Trap id %d)"), trap_id));

            ppdr = arad_pp_trap_mgmt_trap_code_to_internal(unit,
                                                           ppdTrapCode,
                                                           &intTrapCode_arad,
                                                           &dc0_arad,
                                                           &dc1_arad);
    }
    else /* no need of converting to ppd first*/
    {
        ppdr = arad_pp_trap_mgmt_trap_code_to_internal(unit,
                                                     trap_id,
                                                     &intTrapCode_arad,
                                                     &dc0_arad,
                                                     &dc1_arad);
    }


    *hwTrapCode = intTrapCode_arad;
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to resolve PPD"
                                          " Trap code %d (%s):"
                                          " %d (%s)"),
                         unit,
                         ppdTrapCode,
                         SOC_PPC_TRAP_CODE_to_string(ppdTrapCode),
                         result,
                         _SHR_ERRMSG(result)));
exit:
    BCMDNX_FUNC_RETURN;
}

/* Conversion from PPD Trap id from HW trap id */
int
_bcm_dpp_field_trap_ppd_from_hw(int unit,
                                uint32 hwTrapCode,
                                SOC_PPC_TRAP_CODE *ppdTrapCode)
{
    uint32 ppdr;
    int result;

    BCMDNX_INIT_FUNC_DEFS ;

    *ppdTrapCode = SOC_PPC_NOF_TRAP_CODES;
    ppdr = arad_pp_trap_cpu_trap_code_from_internal_unsafe(
            unit,
            hwTrapCode,
            ppdTrapCode);
    result = handle_sand_result(ppdr);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to resolve PPD"
                                          " Trap code from HW %d:"
                                          " %d (%s)"),
                         unit,
                         hwTrapCode,
                         result,
                         _SHR_ERRMSG(result)));
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * Utility procedures related to PLC (Parser Leaf Context) which
 * is also referred to as 'hesder format Extension'
 * {
 */

/*
 *  See header on field_int.h
 */
int
_bcm_dpp_field_HeaderFormatExtension_bcm_to_ppd(bcm_field_header_format_extension_t header_format_extension,
                                       DPP_PLC_E *plc_sw_p)
{
    DPP_PLC_E plc_sw ;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    /*
     * Just load some initial (illegal) value.
     */
    *plc_sw_p = DPP_PLC_NA;
    switch (header_format_extension)
    {
        case bcmFieldHeaderFormatExtensionDontCare:
        {
            plc_sw = DPP_PLC_DONTCARE ;
            break;
        }
        case bcmFieldHeaderFormatExtensionFtmh:
        {
            plc_sw = DPP_PLC_FTMH ;
            break;
        }
        case bcmFieldHeaderFormatExtensionIpUdpGtp1:
        {
            plc_sw = DPP_PLC_IP_UDP_GTP1 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionIpUdpGtp2:
        {
            plc_sw = DPP_PLC_IP_UDP_GTP2 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionFcoe:
        {
            plc_sw = DPP_PLC_FCOE ;
            break;
        }



        case bcmFieldHeaderFormatExtensionMplsNonL4IPV4IsH3:
        {
            plc_sw = DPP_PLC_IPv4isH3 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsNonL4IPV6IsH3:
        {
            plc_sw = DPP_PLC_IPv6isH3 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsL4IPV4isH3:
        {
            plc_sw = DPP_PLC_L4_IPv4isH3 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsL4IPV6isH3:
        {
            plc_sw = DPP_PLC_L4_IPv6isH3 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsETHisH3:
        {
            plc_sw = DPP_PLC_ETHisH3 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsNonL4IPV4isH4:
        {
            plc_sw = DPP_PLC_IPv4isH4 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsNonL4IPV6isH4:
        {
            plc_sw = DPP_PLC_IPv6isH4 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsL4IPV4isH4:
        {
            plc_sw = DPP_PLC_L4_IPv4isH4 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsL4IPV6isH4:
        {
            plc_sw = DPP_PLC_L4_IPv6isH4 ;
            break;
        }


        case bcmFieldHeaderFormatExtensionMplsNonL4IPV4IsAny:
        {
            plc_sw = DPP_PLC_IPv4isHAny ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsNonL4IPV6IsAny:
        {
            plc_sw = DPP_PLC_IPv6isHAny ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsAnyL4IPV4IsH3:
        {
            plc_sw = DPP_PLC_Any_IPv4isH3 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsAnyL4IPV6IsH3:
        {
            plc_sw = DPP_PLC_Any_IPv6isH3 ;
            break;
        }

        case bcmFieldHeaderFormatExtensionMplsNonL4IPV4isH5:
        {
            plc_sw = DPP_PLC_IPv4isH5 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsNonL4IPV6isH5:
        {
            plc_sw = DPP_PLC_IPv6isH5 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsAnyL4IPV4IsH5:
        {
            plc_sw = DPP_PLC_Any_IPv4isH5 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsAnyL4IPV6IsH5:
        {
            plc_sw = DPP_PLC_Any_IPv6isH5 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsETHisH4:
        {
            plc_sw = DPP_PLC_ETHisH4 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsAnyEthIsH3H4:
        {
            plc_sw = DPP_PLC_EthisHAny ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsAnyL4IPV4IsH4:
        {
            plc_sw = DPP_PLC_Any_IPv4isH4 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsAnyL4IPV6IsH4:
        {
            plc_sw = DPP_PLC_Any_IPv6isH4 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsL4IPV4isH5:
        {
            plc_sw = DPP_PLC_L4_IPv4isH5 ;
            break;
        }
        case bcmFieldHeaderFormatExtensionMplsL4IPV6isH5:
        {
            plc_sw = DPP_PLC_L4_IPv6isH5 ;
            break;
        }

        default:
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                        (_BSL_BCM_MSG_NO_UNIT("BCM-level header format extension %d is not valid"),
                         header_format_extension));
        }
    }
    *plc_sw_p = plc_sw ;
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  See header on field_int.h
 */
int
_bcm_dpp_field_HeaderFormatExtension_ppd_to_bcm(DPP_PLC_E plc_sw,
                                       bcm_field_header_format_extension_t *header_format_extension_p)
{
    bcm_field_header_format_extension_t header_format_extension ;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
    /*
     * Just load some initial (illegal) value.
     */
    *header_format_extension_p = bcmFieldHeaderFormatExtensionCount ;
    switch (plc_sw)
    {
        case DPP_PLC_DONTCARE:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionDontCare ;
            break;
        }
        case DPP_PLC_FTMH:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionFtmh ;
            break;
        }
        case DPP_PLC_IP_UDP_GTP1:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionIpUdpGtp1 ;
            break;
        }
        case DPP_PLC_IP_UDP_GTP2:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionIpUdpGtp2 ;
            break;
        }
        case DPP_PLC_FCOE:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionFcoe ;
            break;
        }



        case DPP_PLC_IPv4isH3:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsNonL4IPV4IsH3 ;
            break;
        }
        case DPP_PLC_IPv6isH3:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsNonL4IPV6IsH3 ;
            break;
        }
        case DPP_PLC_L4_IPv4isH3:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsL4IPV4isH3 ;
            break;
        }
        case DPP_PLC_L4_IPv6isH3:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsL4IPV6isH3 ;
            break;
        }
        case DPP_PLC_ETHisH3:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsETHisH3 ;
            break;
        }
        case DPP_PLC_IPv4isH4:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsNonL4IPV4isH4 ;
            break;
        }
        case DPP_PLC_IPv6isH4:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsNonL4IPV6isH4 ;
            break;
        }
        case DPP_PLC_L4_IPv4isH4:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsL4IPV4isH4 ;
            break;
        }
        case DPP_PLC_L4_IPv6isH4:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsL4IPV6isH4 ;
            break;
        }



        case DPP_PLC_IPv4isHAny:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsNonL4IPV4IsAny ;
            break;
        }
        case DPP_PLC_IPv6isHAny:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsNonL4IPV6IsAny ;
            break;
        }
        case DPP_PLC_Any_IPv4isH3:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsAnyL4IPV4IsH3 ;
            break;
        }
        case DPP_PLC_Any_IPv6isH3:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsAnyL4IPV6IsH3 ;
            break;
        }


        case DPP_PLC_IPv4isH5:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsNonL4IPV4isH5 ;
            break;
        }
        case DPP_PLC_IPv6isH5:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsNonL4IPV6isH5 ;
            break;
        }
        case DPP_PLC_Any_IPv4isH5:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsAnyL4IPV4IsH5 ;
            break;
        }
        case DPP_PLC_Any_IPv6isH5:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsAnyL4IPV6IsH5 ;
            break;
        }
        case DPP_PLC_ETHisH4:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsETHisH4 ;
            break;
        }
        case DPP_PLC_EthisHAny:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsAnyEthIsH3H4 ;
            break;
        }
        case DPP_PLC_Any_IPv4isH4:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsAnyL4IPV4IsH4 ;
            break;
        }
        case DPP_PLC_Any_IPv6isH4:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsAnyL4IPV6IsH4 ;
            break;
        }
        case DPP_PLC_L4_IPv4isH5:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsL4IPV4isH5 ;
            break;
        }
        case DPP_PLC_L4_IPv6isH5:
        {
            header_format_extension = bcmFieldHeaderFormatExtensionMplsL4IPV6isH5 ;
            break;
        }

        default:
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("PPD-level header format extension %d is not valid"),
                              plc_sw));
        }
    }
    *header_format_extension_p = header_format_extension ;
exit:
    BCMDNX_FUNC_RETURN;
}
/*
 * }
 */
int
_bcm_dpp_field_HeaderFormat_bcm_to_ppd(int unit,
                                       bcm_field_header_format_t header_format,
                                       DPP_PFC_E *pfc_sw_p,
                                       uint32 *pfc_pmf_p,
                                       uint32 *pfc_pmf_mask_p)
{
    DPP_PFC_E pfc_sw;
    BCMDNX_INIT_FUNC_DEFS;

    switch (header_format) {
    case bcmFieldHeaderFormatL2:
        pfc_sw = DPP_PFC_ETH;
        break;
    case bcmFieldHeaderFormatIp4AnyL2L3:
        pfc_sw = DPP_PFC_ANY_IPV4; /* Match on all the IPv4 packets */
        break;
    case bcmFieldHeaderFormatIp4:
        pfc_sw = DPP_PFC_IPV4_ETH;
        break;
    case bcmFieldHeaderFormatIp6AnyL2L3:
        pfc_sw = DPP_PFC_ANY_IPV6; /* Match on all the IPv6 packets */
        break;
    case bcmFieldHeaderFormatIp6:
        pfc_sw = DPP_PFC_IPV6_ETH;
        break;
    case bcmFieldHeaderFormatMplsLabel1AnyL2L3:
        pfc_sw = DPP_PFC_ANY_MPLS1; /* Match on all the MPLS 1 label packets */
        break;
    case bcmFieldHeaderFormatMplsLabel1:
        pfc_sw = DPP_PFC_MPLS1_ETH;
        break;
    case bcmFieldHeaderFormatMplsLabel2AnyL2L3:
        pfc_sw = DPP_PFC_ANY_MPLS2; /* Match on all the MPLS 2 labels packets */
        break;
    case bcmFieldHeaderFormatMplsLabel2:
        pfc_sw = DPP_PFC_MPLS2_ETH;
        break;
    case bcmFieldHeaderFormatMplsLabel3AnyL2L3:
        pfc_sw = DPP_PFC_ANY_MPLS3; /* Match on all the MPLS 3 labels packets */
        break;
    case bcmFieldHeaderFormatMplsLabel3:
        pfc_sw = DPP_PFC_MPLS3_ETH;
        break;
    case bcmFieldHeaderFormatMplsAnyLabelAnyL2L3:
        pfc_sw = DPP_PFC_ANY_MPLS; /* Match on all the MPLS Labeled packets */
        break;
    case bcmFieldHeaderFormatEthEth:
        pfc_sw = DPP_PFC_ETH_ETH;
        break;
    case bcmFieldHeaderFormatTrill:
    case bcmFieldHeaderFormatEthTrillEth:
        pfc_sw = DPP_PFC_ETH_TRILL_ETH;
        break;
    case bcmFieldHeaderFormatIp4Ip4:
        pfc_sw = DPP_PFC_IPV4_IPV4_ETH;
        break;
    case bcmFieldHeaderFormatIp6Ip4:
        pfc_sw = DPP_PFC_IPV6_IPV4_ETH;
        break;
    case bcmFieldHeaderFormatIp4MplsLabel1:
        pfc_sw = DPP_PFC_IPV4_MPLS1_ETH;
        break;
    case bcmFieldHeaderFormatIp4MplsLabel2:
        pfc_sw = DPP_PFC_IPV4_MPLS2_ETH;
        break;
    case bcmFieldHeaderFormatIp4MplsLabel3:
        pfc_sw = DPP_PFC_IPV4_MPLS3_ETH;
        break;
    case bcmFieldHeaderFormatIp6MplsLabel1:
        pfc_sw = DPP_PFC_IPV6_MPLS1_ETH;
        break;
    case bcmFieldHeaderFormatIp6MplsLabel2:
        pfc_sw = DPP_PFC_IPV6_MPLS2_ETH;
        break;
    case bcmFieldHeaderFormatIp6MplsLabel3:
        pfc_sw = DPP_PFC_IPV6_MPLS3_ETH;
        break;
    case bcmFieldHeaderFormatEthMplsLabel1:
        pfc_sw = DPP_PFC_ETH_MPLS1_ETH;
        break;
    case bcmFieldHeaderFormatEthMplsLabel2:
        pfc_sw = DPP_PFC_ETH_MPLS2_ETH;
        break;
    case bcmFieldHeaderFormatEthMplsLabel3:
        pfc_sw = DPP_PFC_ETH_MPLS3_ETH;
        break;
    case bcmFieldHeaderFormatEthIp4Eth:
        pfc_sw = DPP_PFC_ETH_IPV4_ETH;
        break;
    case bcmFieldHeaderFormatFCoE:
        pfc_sw = DPP_PFC_FCOE_ENCAP_ETH;
        break;
    case bcmFieldHeaderFormatFCoENoEncap:
        pfc_sw = DPP_PFC_FCOE_STD_ETH;
        break;


    case bcmFieldHeaderFormatAfterTunnelTerminatedNonL4IPV4IsH3:
        pfc_sw = DPP_PFC_IPV4isH3;
        break;
    case bcmFieldHeaderFormatAfterTunnelTerminatedNonL4IPV6IsH3:
        pfc_sw = DPP_PFC_IPV6isH3;
        break;
    case bcmFieldHeaderFormatAfterTunnelTerminatedL4IPV4IsH3:
        pfc_sw = DPP_PFC_L4_IPV4isH3;
        break;
    case bcmFieldHeaderFormatAfterTunnelTerminatedL4IPV6IsH3:
        pfc_sw = DPP_PFC_L4_IPV6isH3;
        break;
    case bcmFieldHeaderFormatAfterTunnelTerminatedEthIsH3:
        pfc_sw = DPP_PFC_ETHisH3;
        break;
    case bcmFieldHeaderFormatAfterTunnelTerminatedNonL4IPV4IsH4ETHisH3:
        pfc_sw = DPP_PFC_IPV4isH4_ETHisH3;
        break;
    case bcmFieldHeaderFormatAfterTunnelTerminatedNonL4IPV6IsH4ETHisH3:
        pfc_sw = DPP_PFC_IPV6isH4_ETHisH3;
        break;
    case bcmFieldHeaderFormatIp4Ip6:
        pfc_sw = DPP_PFC_IPV4_IPV6_ETH;
        break;
    case bcmFieldHeaderFormatIp6Ip6:
        pfc_sw = DPP_PFC_IPV6_IPV6_ETH;
        break;
    case bcmFieldHeaderFormatAfterTunnelTerminatedL4IPV4IsH4ETHisH3:
    case bcmFieldHeaderFormatAfterTunnelTerminatedL4IPV6IsH4ETHisH3:
        /* Above 2 cases should be removed via PAPI (PLC_TBR) */
    default:
        if (bcmFieldHeaderFormatCount > header_format) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("header format %d not supported"),
                              header_format));
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("header format %d not valid"),
                              header_format));
        }
    }
    *pfc_sw_p = pfc_sw ;
    BCMDNX_IF_ERR_EXIT(dpp_parser_pfc_get_acl_by_sw(unit, pfc_sw, pfc_pmf_p, pfc_pmf_mask_p));

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_HeaderFormat_ppd_to_bcm(int unit,
                                       uint32 pfc_pmf,
                                       uint32 pfc_pmf_mask,
                                       bcm_field_header_format_t *header_format)
{
    DPP_PFC_E pfc_sw;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(dpp_parser_pfc_get_sw_by_acl(unit, pfc_pmf, pfc_pmf_mask, &pfc_sw));

    switch (pfc_sw)
    {
        case DPP_PFC_ANY_IPV4:
        {
            *header_format = bcmFieldHeaderFormatIp4AnyL2L3;
            break;
        }
        case DPP_PFC_ANY_IPV6:
        {
            *header_format = bcmFieldHeaderFormatIp6AnyL2L3;
            break;
        }
        case DPP_PFC_ANY_MPLS1:
        {
            *header_format = bcmFieldHeaderFormatMplsLabel1AnyL2L3;
            break;
        }
        case DPP_PFC_ANY_MPLS2:
        {
            *header_format = bcmFieldHeaderFormatMplsLabel2AnyL2L3;
            break;
        }
        case DPP_PFC_ANY_MPLS3:
        {
            *header_format = bcmFieldHeaderFormatMplsLabel3AnyL2L3;
            break;
        }
       case DPP_PFC_ANY_MPLS:
        {
            *header_format = bcmFieldHeaderFormatMplsAnyLabelAnyL2L3;
            break;
        }
        case DPP_PFC_ETH:
        {
            *header_format = bcmFieldHeaderFormatL2;
            break;
        }
        case DPP_PFC_IPV4_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp4;
            break;
        }
        case DPP_PFC_IPV6_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp6;
            break;
        }
        case DPP_PFC_MPLS1_ETH:
        {
            *header_format = bcmFieldHeaderFormatMplsLabel1;
            break;
        }
        case DPP_PFC_MPLS2_ETH:
        {
            *header_format = bcmFieldHeaderFormatMplsLabel2;
            break;
        }
        case DPP_PFC_MPLS3_ETH:
        {
            *header_format = bcmFieldHeaderFormatMplsLabel3;
            break;
        }
        case DPP_PFC_ETH_ETH:
        {
            *header_format = bcmFieldHeaderFormatEthEth;
            break;
        }
        case DPP_PFC_ETH_TRILL_ETH:
        {
            *header_format = bcmFieldHeaderFormatEthTrillEth;
            break;
        }
        case DPP_PFC_IPV4_IPV4_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp4Ip4;
            break;
        }
        case DPP_PFC_IPV6_IPV4_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp6Ip4;
            break;
        }
        case DPP_PFC_IPV4_MPLS1_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp4MplsLabel1;
            break;
        }
        case DPP_PFC_IPV4_MPLS2_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp4MplsLabel2;
            break;
        }
        case DPP_PFC_IPV4_MPLS3_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp4MplsLabel3;
            break;
        }
        case DPP_PFC_IPV6_MPLS1_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp6MplsLabel1;
            break;
        }
        case DPP_PFC_IPV6_MPLS2_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp6MplsLabel2;
            break;
        }
        case DPP_PFC_IPV6_MPLS3_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp6MplsLabel3;
            break;
        }
        case DPP_PFC_ETH_MPLS1_ETH:
        {
            *header_format = bcmFieldHeaderFormatEthMplsLabel1;
            break;
        }
        case DPP_PFC_ETH_MPLS2_ETH:
        {
            *header_format = bcmFieldHeaderFormatEthMplsLabel2;
            break;
        }
        case DPP_PFC_ETH_MPLS3_ETH:
        {
            *header_format = bcmFieldHeaderFormatEthMplsLabel3;
            break;
        }
        case DPP_PFC_ETH_IPV4_ETH:
        {
            *header_format = bcmFieldHeaderFormatEthIp4Eth;
            break;
        }
        case DPP_PFC_FCOE_ENCAP_ETH:
        {
            *header_format = bcmFieldHeaderFormatFCoE;
            break;
        }
        case DPP_PFC_FCOE_STD_ETH:
        {
            *header_format = bcmFieldHeaderFormatFCoENoEncap;
            break;
        }


        case DPP_PFC_IPV4isH3:
        {
            *header_format = bcmFieldHeaderFormatAfterTunnelTerminatedNonL4IPV4IsH3;
            break;
        }
        case DPP_PFC_IPV6isH3:
        {
            *header_format = bcmFieldHeaderFormatAfterTunnelTerminatedNonL4IPV6IsH3;
            break;
        }
        case DPP_PFC_L4_IPV4isH3:
        {
            *header_format = bcmFieldHeaderFormatAfterTunnelTerminatedL4IPV4IsH3;
            break;
        }
        case DPP_PFC_L4_IPV6isH3:
        {
            *header_format = bcmFieldHeaderFormatAfterTunnelTerminatedL4IPV6IsH3;
            break;
        }
        case DPP_PFC_ETHisH3:
        {
            *header_format = bcmFieldHeaderFormatAfterTunnelTerminatedEthIsH3;
            break;
        }
        case DPP_PFC_IPV4isH4_ETHisH3:
        {
            *header_format = bcmFieldHeaderFormatAfterTunnelTerminatedNonL4IPV4IsH4ETHisH3;
            break;
        }
        case DPP_PFC_IPV6isH4_ETHisH3:
        {
            *header_format = bcmFieldHeaderFormatAfterTunnelTerminatedNonL4IPV6IsH4ETHisH3;
            break;
        }
        case DPP_PFC_IPV4_IPV6_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp4Ip6;
            break;
        }
        case DPP_PFC_IPV6_IPV6_ETH:
        {
            *header_format = bcmFieldHeaderFormatIp6Ip6;
            break;
        }
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unexpected SW PFC:%d"),
                              pfc_sw));
    }
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_field_color_bcm_to_ppd(int bcmCol,
                                uint32 *ppdCol)
{
    switch (bcmCol) {
    case BCM_FIELD_COLOR_GREEN:
        *ppdCol = 0;
        break;
    case BCM_FIELD_COLOR_YELLOW:
        *ppdCol = 1;
        break;
    case BCM_FIELD_COLOR_RED:
        *ppdCol = 2;
        break;
    case BCM_FIELD_COLOR_BLACK:
        *ppdCol = 3;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_color_ppd_to_bcm(uint32 ppdCol,
                                int *bcmCol)
{
    switch (ppdCol) {
    case 0:
        *bcmCol = BCM_FIELD_COLOR_GREEN;
        break;
    case 1:
        *bcmCol = BCM_FIELD_COLOR_YELLOW;
        break;
    case 2:
        *bcmCol = BCM_FIELD_COLOR_RED;
        break;
    case 3:
        *bcmCol = BCM_FIELD_COLOR_BLACK;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_arp_opcode_bcm_to_ppd(bcm_field_ArpOpcode_t bcmArpOpcode,
                                uint32 *ppdArpOpcode)
{
    switch (bcmArpOpcode) {
    case bcmFieldArpOpcodeRequest:
        *ppdArpOpcode = 1;
        break;
    case bcmFieldArpOpcodeReply:
        *ppdArpOpcode = 2;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

int
_bcm_dpp_field_arp_opcode_ppd_to_bcm(bcm_field_ArpOpcode_t ppdArpOpcode,
                                bcm_field_ArpOpcode_t *bcmArpOpcode)
{
    switch (ppdArpOpcode) {
    case 1:
        *bcmArpOpcode = bcmFieldArpOpcodeRequest;
        break;
    case 2:
        *bcmArpOpcode = bcmFieldArpOpcodeReply;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_stp_state_bcm_to_ppd(bcm_stg_stp_t bcmStp,
                                    SOC_PPC_PORT_STP_STATE_FLD_VAL *ppdStp)
{
    switch (bcmStp) {
    case BCM_STG_STP_BLOCK:
    case BCM_STG_STP_DISABLE:
    case BCM_STG_STP_LISTEN:
        *ppdStp = SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK;
        break;
    case BCM_STG_STP_LEARN:
        *ppdStp = SOC_PPC_PORT_STP_STATE_FLD_VAL_LEARN;
        break;
    case BCM_STG_STP_FORWARD:
        *ppdStp = SOC_PPC_PORT_STP_STATE_FLD_VAL_FORWARD;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_stp_state_ppd_to_bcm(SOC_PPC_PORT_STP_STATE_FLD_VAL ppdStp,
                                    bcm_stg_stp_t *bcmStp)
{
    switch (ppdStp) {
    case SOC_PPC_PORT_STP_STATE_FLD_VAL_BLOCK:
        *bcmStp = BCM_STG_STP_BLOCK;
        break;
    case SOC_PPC_PORT_STP_STATE_FLD_VAL_LEARN:
        *bcmStp = BCM_STG_STP_LEARN;
        break;
    case SOC_PPC_PORT_STP_STATE_FLD_VAL_FORWARD:
        *bcmStp = BCM_STG_STP_FORWARD;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_forwarding_type_bcm_to_ppd(bcm_field_ForwardingType_t bcmForwardingType,
                                          SOC_TMC_PKT_FRWRD_TYPE *ppdForwardingType,
                                          uint32 *ppdMask)
{
    *ppdMask = 0xF;
    switch(bcmForwardingType) {
    case bcmFieldForwardingTypeL2:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_BRIDGE;
        break;
    case bcmFieldForwardingTypeIp4Ucast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC;
        break;
    case bcmFieldForwardingTypeIp4Mcast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC;
        break;
    case bcmFieldForwardingTypeIp6Ucast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC;
        break;
    case bcmFieldForwardingTypeIp6Mcast:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC;
        break;
    case bcmFieldForwardingTypeMpls:
    case bcmFieldForwardingTypeMplsLabel1:
    case bcmFieldForwardingTypeMplsLabel2:
    case bcmFieldForwardingTypeMplsLabel3:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_MPLS;
        break;
    case bcmFieldForwardingTypeTrill:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_TRILL;
        break;
    case bcmFieldForwardingTypeRxReason:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP;
        break;
    case bcmFieldForwardingTypeFCoE:
    case bcmFieldForwardingTypeCustom1:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1;
        break;
    case bcmFieldForwardingTypeCustom2:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_CUSTOM2;
        break;
    case bcmFieldForwardingTypeTrafficManagement:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_TM;
        break;
    case bcmFieldForwardingTypeSnoop:
        *ppdForwardingType = SOC_TMC_PKT_FRWRD_TYPE_SNOOP;
        break;
    case bcmFieldForwardingTypeAny:
        /* 
         * Special case: select maximum of Ethernet-based packets. 
         * In practice, all PP packets except FCoE 
         */
        *ppdForwardingType = 0;
        *ppdMask = 0x8; /* MSB unset */
        break;
    default:
        return BCM_E_PARAM;
    };
    return BCM_E_NONE;
}


int
_bcm_dpp_field_forwarding_type_ppd_to_bcm(int unit,
                                          SOC_TMC_PKT_FRWRD_TYPE ppdForwardingType,
                                          uint32 ppdMask,
                                          bcm_field_ForwardingType_t *bcmForwardingType)
{
    if (ppdMask == 0xF) {
        switch(ppdForwardingType) {
        case SOC_TMC_PKT_FRWRD_TYPE_BRIDGE:
            *bcmForwardingType = bcmFieldForwardingTypeL2;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_IPV4_UC:
            *bcmForwardingType = bcmFieldForwardingTypeIp4Ucast;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_IPV4_MC:
            *bcmForwardingType = bcmFieldForwardingTypeIp4Mcast;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_IPV6_UC:
            *bcmForwardingType = bcmFieldForwardingTypeIp6Ucast;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_IPV6_MC:
            *bcmForwardingType = bcmFieldForwardingTypeIp6Mcast;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_MPLS:
            *bcmForwardingType = bcmFieldForwardingTypeMpls;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_TRILL:
            *bcmForwardingType = bcmFieldForwardingTypeTrill;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_CPU_TRAP:
            *bcmForwardingType = bcmFieldForwardingTypeRxReason;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_CUSTOM1:
            if (SOC_DPP_CONFIG(unit)->pp.fcoe_enable) {
                *bcmForwardingType = bcmFieldForwardingTypeFCoE;
            } else {
                *bcmForwardingType = bcmFieldForwardingTypeCustom1;
            }
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_CUSTOM2:
            *bcmForwardingType = bcmFieldForwardingTypeCustom2;

            break;
        case SOC_TMC_PKT_FRWRD_TYPE_TM:
            *bcmForwardingType = bcmFieldForwardingTypeTrafficManagement;
            break;
        case SOC_TMC_PKT_FRWRD_TYPE_SNOOP:
            *bcmForwardingType = bcmFieldForwardingTypeSnoop;
            break;
        default:
            return BCM_E_PARAM;
        };
    }
    else if (ppdMask == 0x8) {
        if (ppdForwardingType == 0) {
            *bcmForwardingType = bcmFieldForwardingTypeAny;
        }
        else {
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}


int
_bcm_dpp_field_app_type_bcm_to_ppd(int unit,
                                   bcm_field_AppType_t bcmAppType,
                                   uint32 *ppd_flp_program)
{
    uint32
        sandResult,
        ppdAppType = 0;
    uint8  
        prog_index = 0;
    BCMDNX_INIT_FUNC_DEFS ;

    *ppd_flp_program = 0;


    switch(bcmAppType) {
    case bcmFieldAppTypeL2:
        ppdAppType = PROG_FLP_ETHERNET_ING_LEARN;
        break;
    case bcmFieldAppTypeTrafficManagement:
        ppdAppType = PROG_FLP_TM;
        break;
    case bcmFieldAppTypeIp4Ucast:
        ppdAppType = PROG_FLP_IPV4UC;
        break;
    case bcmFieldAppTypeIp4DoubleCapacity:
        ppdAppType = PROG_FLP_IPV4_DC;
        break;
    case bcmFieldAppTypeIp4UcastRpf:
        ppdAppType = PROG_FLP_IPV4UC_RPF;
        break;
    case bcmFieldAppTypeIp4McastRpf:
        ppdAppType = PROG_FLP_IPV4MC_BRIDGE;
        break;
    case bcmFieldAppTypeCompIp4McastRpf:
        ppdAppType = PROG_FLP_IPV4COMPMC_WITH_RPF;
        break;
    case bcmFieldAppTypeIp6Ucast:
        ppdAppType = PROG_FLP_IPV6UC;
        break;
    case bcmFieldAppTypeIp6Mcast:
        ppdAppType = PROG_FLP_IPV6MC;
        break;
    case bcmFieldAppTypeTrillUcast:
        ppdAppType = PROG_FLP_TRILL_UC;
        break;
    case bcmFieldAppTypeTrillMcast:
        ppdAppType = PROG_FLP_TRILL_MC_ONE_TAG;
        break;
    case bcmFieldAppTypeFglTrillMcast:
        ppdAppType = PROG_FLP_TRILL_MC_TWO_TAGS;
        break;
    case bcmFieldAppTypeMpls:
        ppdAppType = PROG_FLP_LSR;
        break;
    case bcmFieldAppTypeIp4SrcBind:
        ppdAppType = PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC;
        break;
    case bcmFieldAppTypeIp6SrcBind:
        ppdAppType = PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC;
        break;
    case bcmFieldAppTypeIp4MacSrcBind:
        ppdAppType = PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP;
        break;
    case bcmFieldAppTypeIp6MacSrcBind:
        ppdAppType = PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP;
        break;
    case bcmFieldAppTypeMiM:
        ppdAppType = PROG_FLP_ETHERNET_MAC_IN_MAC;
        break;
    case bcmFieldAppTypeL2VpnDirect:
        ppdAppType = PROG_FLP_P2P;
        break;
    case bcmFieldAppTypeL2MiM:
        ppdAppType = PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM;
        break;
    case bcmFieldAppTypeL2TrillMcast:
        ppdAppType = PROG_FLP_TRILL_AFTER_TERMINATION;
        break;
    case bcmFieldAppTypeFCoETransit:
        ppdAppType = PROG_FLP_FC_TRANSIT;
        break;
    case bcmFieldAppTypeFCoE:
        ppdAppType = PROG_FLP_FC;
        break;
    case bcmFieldAppTypeFCoEVft:
        ppdAppType = PROG_FLP_FC_WITH_VFT;
        break;
    case bcmFieldAppTypeFCoERemote:
        ppdAppType = PROG_FLP_FC_REMOTE;
        break;
    case bcmFieldAppTypeFCoEVftRemote:
        ppdAppType = PROG_FLP_FC_WITH_VFT_REMOTE;
        break;
    case bcmFieldAppTypeIp6UcastRpf:
        if (SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists) 
        {
            /* FLP Program FWD_PRCESSING_PROFILE of the IPv6-UC RPF 2PASS solution program */
            ppdAppType = PROG_FLP_IPV6UC_WITH_RPF_2PASS;
        }
        else
        {
                /* The standard IPv6-UC RPF FLP Program */
                ppdAppType = PROG_FLP_IPV6UC_RPF;
        }
        break;
    case bcmFieldAppTypeIp4McastBidir:
        ppdAppType = PROG_FLP_BIDIR;
        break;
    case bcmFieldAppTypeVplsGreIp:
        ppdAppType = PROG_FLP_VPLSOGRE;
        break;
    case bcmFieldAppTypeBfdIp4SingleHop:
        ppdAppType = PROG_FLP_BFD_IPV4_SINGLE_HOP;
        break;
    case bcmFieldAppTypeBfdIp6SingleHop:
        ppdAppType = PROG_FLP_BFD_IPV6_SINGLE_HOP;
        break;
    case bcmFieldAppTypeIp6UcastPublic:
        ppdAppType = PROG_FLP_IPV6UC_PUBLIC;
        break;
    case bcmFieldAppTypeIp6UcastRpfPublic:
        ppdAppType = PROG_FLP_IPV6UC_PUBLIC_RPF;
        break;
    case bcmFieldAppTypeIp4UcastPublic:
        ppdAppType = PROG_FLP_IPV4UC_PUBLIC;
        break;
    case bcmFieldAppTypeIp4UcastRpfPublic:
        ppdAppType = PROG_FLP_IPV4UC_PUBLIC_RPF;
        break;
    case bcmFieldAppTypeIp4McastL2Ssm:
        ppdAppType = PROG_FLP_IPV4_MC_SSM;
        break;
    case bcmFieldAppTypeIp6McastL2Ssm:
        ppdAppType = PROG_FLP_IPV6_MC_SSM_EUI;
        break;
    case bcmFieldAppTypeIp6McastL3Ssm:
        ppdAppType = PROG_FLP_IPV6_MC_SSM;
        break;
    case bcmFieldAppTypeIp6McastL3SsmCompressSip:
        ppdAppType = PROG_FLP_IPV6_MC_SSM_COMPRESS_SIP;
        break;
    default:
        return BCM_E_PARAM;
    };

    /* Translate the Application type to the FLP program */
    sandResult = arad_pp_flp_app_to_prog_index_get((unit), ppdAppType, &prog_index);
    BCMDNX_IF_ERR_EXIT(handle_sand_result(sandResult));
    *ppd_flp_program = prog_index;

exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_dpp_field_app_type_ppd_to_bcm(int unit,
                                          uint32 ppd_flp_program,
                                          bcm_field_AppType_t *bcmAppType)
{
    uint8
        ppdAppType;
    BCMDNX_INIT_FUNC_DEFS ;

    *bcmAppType = bcmFieldAppTypeCount;
    /* Translate the FLP program to the application */
    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.pp.fec.flp_progs_mapping.get(unit, ppd_flp_program, &ppdAppType));

    switch(ppdAppType) {
    case PROG_FLP_ETHERNET_ING_LEARN:
        *bcmAppType = bcmFieldAppTypeL2;
        break;
    case PROG_FLP_TM:
        *bcmAppType = bcmFieldAppTypeTrafficManagement;
        break;
    case PROG_FLP_IPV4_DC:
        ppdAppType = bcmFieldAppTypeIp4DoubleCapacity;
        break;
    case PROG_FLP_IPV4UC:
        *bcmAppType = bcmFieldAppTypeIp4Ucast;
        break;
    case PROG_FLP_BFD_IPV4_SINGLE_HOP:
        *bcmAppType = bcmFieldAppTypeBfdIp4SingleHop;
        break;
    case PROG_FLP_BFD_IPV6_SINGLE_HOP:
        *bcmAppType = bcmFieldAppTypeBfdIp6SingleHop;
        break;
    case PROG_FLP_IPV4UC_RPF:
        *bcmAppType = bcmFieldAppTypeIp4UcastRpf;
        break;
    case PROG_FLP_IPV4MC_BRIDGE:
        *bcmAppType = bcmFieldAppTypeIp4McastRpf;
        break;
    case PROG_FLP_IPV4COMPMC_WITH_RPF:
        *bcmAppType = bcmFieldAppTypeCompIp4McastRpf;
        break;
    case PROG_FLP_IPV6UC:
        *bcmAppType = bcmFieldAppTypeIp6Ucast;
        break;
    case PROG_FLP_IPV6MC:
        *bcmAppType = bcmFieldAppTypeIp6Mcast;
        break;
    case PROG_FLP_TRILL_UC:
    /* Same value case PROG_FLP_ETHERNET_TK_EPON_UNI_V4_STATIC: */
        if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
            *bcmAppType = bcmFieldAppTypeIp4SrcBind;
        }
        else {
            *bcmAppType = bcmFieldAppTypeTrillUcast;
        }
        break;
    case PROG_FLP_TRILL_MC_ONE_TAG:
    /* Same value: case PROG_FLP_ETHERNET_TK_EPON_UNI_V6_STATIC: */
        if (SOC_DPP_CONFIG(unit)->pp.pon_application_enable) {
            *bcmAppType = bcmFieldAppTypeIp6SrcBind;
        }
        else {
            *bcmAppType = bcmFieldAppTypeTrillMcast;
        }
        break;
    case PROG_FLP_LSR:
        *bcmAppType = bcmFieldAppTypeMpls;
        break;
    case PROG_FLP_ETHERNET_TK_EPON_UNI_V4_DHCP:
        *bcmAppType = bcmFieldAppTypeIp4MacSrcBind;
        break;
    case PROG_FLP_ETHERNET_TK_EPON_UNI_V6_DHCP:
        *bcmAppType = bcmFieldAppTypeIp6MacSrcBind;
        break;
    case PROG_FLP_ETHERNET_MAC_IN_MAC:
        *bcmAppType = bcmFieldAppTypeMiM;
        break;
    case PROG_FLP_P2P:
        *bcmAppType = bcmFieldAppTypeL2VpnDirect;
        break;
    case PROG_FLP_MAC_IN_MAC_AFTER_TERMINATIOM:
        /* Same value: case PROG_FLP_TRILL_MC_TWO_TAGS */
        if (SOC_DPP_CONFIG(unit)->trill.mode) {
            *bcmAppType = bcmFieldAppTypeFglTrillMcast;
        }
        else {
            *bcmAppType = bcmFieldAppTypeL2MiM;
        }
        break;
        break;
    case PROG_FLP_TRILL_AFTER_TERMINATION:
        /* Same value: case PROG_FLP_FC_TRANSIT */
        if (SOC_DPP_CONFIG(unit)->trill.mode) {
            *bcmAppType = bcmFieldAppTypeL2TrillMcast;
        }
        else {
            *bcmAppType = bcmFieldAppTypeFCoETransit;
        }
        break;
    case PROG_FLP_IPV6UC_RPF:
        *bcmAppType = bcmFieldAppTypeIp6UcastRpf;
        break;
    case PROG_FLP_IPV6UC_WITH_RPF_2PASS:
        if (SOC_DPP_CONFIG(unit)->pp.ipv6_with_rpf_2pass_exists) 
        {
            *bcmAppType = bcmFieldAppTypeIp6UcastRpf;
        }
        break; /* If not 2pass exists and still FLP program is IPV6UC_WITH_RPF_2PASS, return def value set already */
    case PROG_FLP_FC:
        *bcmAppType = bcmFieldAppTypeFCoE;
        break;
    case PROG_FLP_FC_WITH_VFT:
        *bcmAppType = bcmFieldAppTypeFCoEVft;
        break;
    case PROG_FLP_FC_REMOTE:
        *bcmAppType = bcmFieldAppTypeFCoERemote;
        break;
    case PROG_FLP_FC_WITH_VFT_REMOTE:
        *bcmAppType = bcmFieldAppTypeFCoEVftRemote;
        break;
    case PROG_FLP_VPLSOGRE:
        *bcmAppType = bcmFieldAppTypeVplsGreIp;
        break;
    case PROG_FLP_BIDIR:
        *bcmAppType = bcmFieldAppTypeIp4McastBidir;
        break;
    case PROG_FLP_IPV6UC_PUBLIC:
        *bcmAppType = bcmFieldAppTypeIp6UcastPublic;
        break;
    case PROG_FLP_IPV6UC_PUBLIC_RPF:
        *bcmAppType = bcmFieldAppTypeIp6UcastRpfPublic;
        break;
    case PROG_FLP_IPV4UC_PUBLIC:
        *bcmAppType = bcmFieldAppTypeIp4UcastPublic;
        break;
    case PROG_FLP_IPV4UC_PUBLIC_RPF:
        *bcmAppType = bcmFieldAppTypeIp4UcastRpfPublic;
        break;
    case PROG_FLP_IPV4_MC_SSM:
        *bcmAppType = bcmFieldAppTypeIp4McastL2Ssm;
        break;
    case PROG_FLP_IPV6_MC_SSM_EUI:
        *bcmAppType = bcmFieldAppTypeIp6McastL2Ssm;
        break;
    case PROG_FLP_IPV6_MC_SSM:
        *bcmAppType = bcmFieldAppTypeIp6McastL3Ssm;
        break;
    case PROG_FLP_IPV6_MC_SSM_COMPRESS_SIP:
        *bcmAppType = bcmFieldAppTypeIp6McastL3SsmCompressSip;
        break;
    default:
        return BCM_E_PARAM;
    };

exit:
    BCMDNX_FUNC_RETURN;
}

/* Which MPLS label in case it is MPLS */
int
_bcm_dpp_field_offset_ext_ppd_to_bcm(uint32 ppdOffsetExtData, uint32 ppdOffsetExtMask,
                                          bcm_field_ForwardingType_t *bcmForwardingType)
{
    if (ppdOffsetExtMask) {
        switch(ppdOffsetExtData) {
        case 0:
            *bcmForwardingType = bcmFieldForwardingTypeMplsLabel1;
            break;
        case 1:
            *bcmForwardingType = bcmFieldForwardingTypeMplsLabel2;
            break;
        case 2:
            *bcmForwardingType = bcmFieldForwardingTypeMplsLabel3;
            break;
        default:
            return BCM_E_PARAM;
        };
    }
    else {
        *bcmForwardingType = bcmFieldForwardingTypeMpls;
    }
    return BCM_E_NONE;
}

int
_bcm_dpp_field_offset_ext_bcm_to_ppd(bcm_field_ForwardingType_t bcmForwardingType,
                                        uint32 *ppdOffsetExtData, uint32 *ppdOffsetExtMask)
{
    switch(bcmForwardingType) {
    case bcmFieldForwardingTypeMplsLabel1:
        *ppdOffsetExtData = 0;
        *ppdOffsetExtMask = 0x3;
        break;
    case bcmFieldForwardingTypeMplsLabel2:
        *ppdOffsetExtData = 1;
        *ppdOffsetExtMask = 0x3;
        break;
    case bcmFieldForwardingTypeMplsLabel3:
        *ppdOffsetExtData = 2;
        *ppdOffsetExtMask = 0x3;
        break;
    default:
        *ppdOffsetExtData = 0;
        *ppdOffsetExtMask = 0;
    };
    return BCM_E_NONE;
}



int
_bcm_dpp_field_vlan_format_bcm_to_ppd(uint8 bcmVlanFormatBitmap,
                                      SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT *ppdVlanFormat)
{
    /* See the VLAN Tag handle summary to understand the mapping - or the get function below */
    if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED) {
        if ((bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO)
            || (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED_VID_ZERO)) {
            /* Invalid bitmap */
            return BCM_E_PARAM;
        }
        else {
            if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED) {
                *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG;
            }
            else {
                *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG ;
            }
        }
    }
    else {
        /* No Outer-TAG */
        if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO) {
            if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED_VID_ZERO) {
                /* Invalid bitmap */
                return BCM_E_PARAM;
            }
            else {
                if (bcmVlanFormatBitmap & BCM_FIELD_VLAN_FORMAT_INNER_TAGGED) {
                    *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG;
                }
                else {
                    *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG;
                }
            }
        }
        else {
            if (bcmVlanFormatBitmap) {
                /* Invalid bitmap */
                return BCM_E_PARAM;
            }
            else {
                *ppdVlanFormat = SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE ;
            }
        }
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_vlan_format_ppd_to_bcm(SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT ppdVlanFormat,
                                          uint8 *bcmVlanFormatBitmap)
{
    *bcmVlanFormatBitmap = 0;
    switch(ppdVlanFormat) {
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE:
        break;
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_TAG:
        *bcmVlanFormatBitmap = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED;
        break;
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_TAG:
        *bcmVlanFormatBitmap = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO;
        break;
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_PRIORITY_C_TAG:
        *bcmVlanFormatBitmap = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED_VID_ZERO
            | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
        break;
    case SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_S_C_TAG:
        *bcmVlanFormatBitmap = BCM_FIELD_VLAN_FORMAT_OUTER_TAGGED
            | BCM_FIELD_VLAN_FORMAT_INNER_TAGGED;
        break;
    default:
        return BCM_E_PARAM;
    };
    return BCM_E_NONE;
}


int
_bcm_dpp_field_ip_type_bcm_to_ppd(bcm_field_IpType_t bcmIpType,
                                  uint8 is_egress,
                                    SOC_PPC_FP_PARSED_ETHERTYPE *ppdIpType)
{
    if (!is_egress) {
        switch (bcmIpType) {
        case bcmFieldIpTypeIpv4Any:
        case bcmFieldIpTypeIpv4NoOpts:
        case bcmFieldIpTypeIpv4WithOpts:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_IPV4;
            break;
        case bcmFieldIpTypeIpv6NoExtHdr:
        case bcmFieldIpTypeIpv6OneExtHdr:
        case bcmFieldIpTypeIpv6:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_IPV6;
            break;
        case bcmFieldIpTypeArp:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_ARP;
            break;
        case bcmFieldIpTypeCfm:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_CFM;
            break;
        case bcmFieldIpTypeTrill:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_TRILL;
            break;
        case bcmFieldIpTypeMim:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_MAC_IN_MAC;
            break;
        case bcmFieldIpTypeMplsUnicast:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_MPLS;
            break;
        case bcmFieldIpTypeFCoE:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_FC_E;
            break;
        case bcmFieldIpTypeMplsMulticast:
            *ppdIpType = (SOC_PPC_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST + 6);
            break;
        /* Special case for User-Header applications */
        case bcmFieldIpTypeCount:
            *ppdIpType = (SOC_PPC_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST + 4);
            break;
        case bcmFieldIpTypeAny:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_NO_MATCH;
            break;
        default:
            return BCM_E_PARAM;
        }
    }
    else {
        switch (bcmIpType) {
        case bcmFieldIpTypeIpv4Any:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_IPV4;
            break;
        case bcmFieldIpTypeIpv6:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_IPV6;
            break;
        case bcmFieldIpTypeArp:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_ARP;
            break;
        case bcmFieldIpTypeTrill:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_TRILL;
            break;
        case bcmFieldIpTypeMim:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_MAC_IN_MAC;
            break;
        case bcmFieldIpTypeCfm:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_CFM;
            break;
        case bcmFieldIpTypeMplsUnicast:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_MPLS;
            break;
        case bcmFieldIpTypeMplsMulticast:
            *ppdIpType = (SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_MPLS + 1);
            break;
        case bcmFieldIpTypeAny:
            *ppdIpType = SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_NO_MATCH;
            break;
        /* Egress ACL does not support qualifying IPv4/6 options */
        case bcmFieldIpTypeIpv4NoOpts:
        case bcmFieldIpTypeIpv4WithOpts:
        case bcmFieldIpTypeIpv6NoExtHdr:
        case bcmFieldIpTypeIpv6OneExtHdr:
        default:
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}


int
_bcm_dpp_field_ip_type_ppd_to_bcm(SOC_PPC_FP_PARSED_ETHERTYPE ppdIpType,
                                  uint8 is_egress,
                                    bcm_field_IpType_t *bcmIpType)
{
    if (!is_egress) {
        /* Special case for User-Header applications */
        if((uint32) ppdIpType == (uint32) SOC_PPC_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST + 4) {
            *bcmIpType = bcmFieldIpTypeCount;
        } else {
            switch (ppdIpType) {
            case SOC_PPC_FP_PARSED_ETHERTYPE_IPV4:
                *bcmIpType = bcmFieldIpTypeIpv4Any;
                break;
            case SOC_PPC_FP_PARSED_ETHERTYPE_IPV6:
                *bcmIpType = bcmFieldIpTypeIpv6;
                break;
            case SOC_PPC_FP_PARSED_ETHERTYPE_ARP:
                *bcmIpType = bcmFieldIpTypeArp;
                break;
            case SOC_PPC_FP_PARSED_ETHERTYPE_CFM:
                *bcmIpType = bcmFieldIpTypeCfm;
                break;
            case SOC_PPC_FP_PARSED_ETHERTYPE_TRILL:
                *bcmIpType = bcmFieldIpTypeTrill;
                break;
            case SOC_PPC_FP_PARSED_ETHERTYPE_MAC_IN_MAC:
                *bcmIpType = bcmFieldIpTypeMim;
                break;
            case SOC_PPC_FP_PARSED_ETHERTYPE_FC_E:
                *bcmIpType = bcmFieldIpTypeFCoE;
                break;
            case SOC_PPC_FP_PARSED_ETHERTYPE_MPLS:
                *bcmIpType = bcmFieldIpTypeMplsUnicast;
                break;
            case (SOC_PPC_FP_PARSED_ETHERTYPE_USER_DEFINED_FIRST + 6):
                *bcmIpType = bcmFieldIpTypeMplsMulticast;
                break;
            case SOC_PPC_FP_PARSED_ETHERTYPE_NO_MATCH:
                *bcmIpType = bcmFieldIpTypeAny;
                break;
            default:
                return BCM_E_PARAM;
            }
        }
    }
    else {
        switch ((SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS)ppdIpType) {
        case SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_IPV4:
            *bcmIpType = bcmFieldIpTypeIpv4Any;
            break;
        case SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_IPV6:
            *bcmIpType = bcmFieldIpTypeIpv6;
            break;
        case SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_ARP:
            *bcmIpType = bcmFieldIpTypeArp;
            break;
        case SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_TRILL:
            *bcmIpType = bcmFieldIpTypeTrill;
            break;
        case SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_MAC_IN_MAC:
            *bcmIpType = bcmFieldIpTypeMim;
            break;
        case SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_CFM:
            *bcmIpType = bcmFieldIpTypeCfm;
            break;
        case SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_MPLS:
            *bcmIpType = bcmFieldIpTypeMplsUnicast;
            break;
        case (SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_MPLS + 1):
            *bcmIpType = bcmFieldIpTypeMplsMulticast;
            break;
        case SOC_PPC_FP_PARSED_ETHERTYPE_EGRESS_NO_MATCH:
            *bcmIpType = bcmFieldIpTypeAny;
            break;
        default:
            return BCM_E_PARAM;
        }
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_l2_eth_format_bcm_to_ppd(bcm_field_L2Format_t bcmL2Format,
                                    SOC_PPC_FP_ETH_ENCAPSULATION *ppdL2Format)
{
    switch (bcmL2Format) {
    case bcmFieldL2FormatAny:
        *ppdL2Format = SOC_PPC_FP_ETH_ENCAPSULATION_UNDEF;
        break;
    case bcmFieldL2FormatEthII:
        *ppdL2Format = SOC_PPC_FP_ETH_ENCAPSULATION_ETH_II;
        break;
    case bcmFieldL2FormatSnap:
        *ppdL2Format = SOC_PPC_FP_ETH_ENCAPSULATION_LLC_SNAP;
        break;
    case bcmFieldL2FormatLlc:
        *ppdL2Format = SOC_PPC_FP_ETH_ENCAPSULATION_LLC;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_l2_eth_format_ppd_to_bcm(SOC_PPC_FP_ETH_ENCAPSULATION ppdL2Format,
                                        bcm_field_L2Format_t *bcmL2Format)
{
    switch (ppdL2Format) {
    case SOC_PPC_FP_ETH_ENCAPSULATION_UNDEF:
        *bcmL2Format = bcmFieldL2FormatAny;
        break;
    case SOC_PPC_FP_ETH_ENCAPSULATION_ETH_II:
        *bcmL2Format = bcmFieldL2FormatEthII;
        break;
    case SOC_PPC_FP_ETH_ENCAPSULATION_LLC_SNAP:
        *bcmL2Format = bcmFieldL2FormatSnap;
        break;
    case SOC_PPC_FP_ETH_ENCAPSULATION_LLC:
        *bcmL2Format = bcmFieldL2FormatLlc;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


/* Translation during the ForwardingHeader Offset qualifier to know which offset */
int
_bcm_dpp_field_base_header_bcm_to_ppd(bcm_field_data_offset_base_t bcm_base_header,
                                      uint32 *ppd_base_header)
{
    switch (bcm_base_header) {
    case bcmFieldDataOffsetBasePacketStart:
        *ppd_base_header = 0;
        break;
    case bcmFieldDataOffsetBaseL2Header:
        *ppd_base_header = 1;
        break;
    case bcmFieldDataOffsetBaseFirstHeader:
        *ppd_base_header = 2;
        break;
    case bcmFieldDataOffsetBaseSecondHeader:
        *ppd_base_header = 3;
        break;
    case bcmFieldDataOffsetBaseThirdHeader:
        *ppd_base_header = 4;
        break;
    case bcmFieldDataOffsetBaseFourthHeader:
        *ppd_base_header = 5;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

int
_bcm_dpp_field_base_header_ppd_to_bcm(uint32 ppd_base_header,
                                      bcm_field_data_offset_base_t *bcm_base_header)
{
    switch (ppd_base_header) {
    case 0:
        *bcm_base_header = bcmFieldDataOffsetBasePacketStart;
        break;
    case 1:
        *bcm_base_header = bcmFieldDataOffsetBaseL2Header;
        break;
    case 2:
        *bcm_base_header = bcmFieldDataOffsetBaseFirstHeader;
        break;
    case 3:
        *bcm_base_header = bcmFieldDataOffsetBaseSecondHeader;
        break;
    case 4:
        *bcm_base_header = bcmFieldDataOffsetBaseThirdHeader;
        break;
    case 5:
        *bcm_base_header = bcmFieldDataOffsetBaseFourthHeader;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}



int
_bcm_dpp_field_ip_next_protocol_bcm_to_ppd(bcm_field_IpProtocolCommon_t bcmIpNextProtocol,
                                    SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL *ppdIpNextProtocol, uint64 *emask)
{
    /* All masked */
    COMPILER_64_SET(*emask, ~0, ~0);
    switch (bcmIpNextProtocol) {
    case bcmFieldIpProtocolCommonTcpUdp:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_TCP;
        COMPILER_64_SET(*emask, 0, 0xe);
        break;
    case bcmFieldIpProtocolCommonTcp:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_TCP;
        break;
    case bcmFieldIpProtocolCommonUdp:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_UDP;
        break;
    case bcmFieldIpProtocolCommonIgmp:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_IGMP;
        break;
    case bcmFieldIpProtocolCommonIcmp:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_ICMP;
        break;
    case bcmFieldIpProtocolCommonIp6Icmp:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_ICMP_V6;
        break;
    case bcmFieldIpProtocolCommonIpInIp:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_IPV4;
        break;
    case bcmFieldIpProtocolCommonIp6InIp:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_IPV6;
        break;
    case bcmFieldIpProtocolCommonMplsInIp:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_MPLS;
        break;
    case bcmFieldIpProtocolCommonUnknown:
        *ppdIpNextProtocol = SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_NO_MATCH;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_ip_next_protocol_ppd_to_bcm(SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL ppdIpNextProtocol,
                                    bcm_field_IpProtocolCommon_t *bcmIpNextProtocol, int32 emask)
{
    switch (ppdIpNextProtocol) {
    case SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_TCP:
        if (emask == 0xe) {
            *bcmIpNextProtocol = bcmFieldIpProtocolCommonTcpUdp;
        } else {
            *bcmIpNextProtocol = bcmFieldIpProtocolCommonTcp;
        }
        break;
    case SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_UDP:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonUdp;
        break;
    case SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_IGMP:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIgmp;
        break;
    case SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_ICMP:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIcmp;
        break;
    case SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_ICMP_V6:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIp6Icmp;
        break;
    case SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_IPV4:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIpInIp;
        break;
    case SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_IPV6:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonIp6InIp;
        break;
    case SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_MPLS:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonMplsInIp;
        break;
    case SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL_NO_MATCH:
        *bcmIpNextProtocol = bcmFieldIpProtocolCommonUnknown;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_termination_type_bcm_to_ppd(bcm_field_TunnelType_t bcmTerminationType,
                                           SOC_PPC_PKT_TERM_TYPE *ppdTerminationType)
{
    switch (bcmTerminationType) {
    case bcmFieldTunnelTypeIp:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_IPV4_ETH;
        break;
    case bcmFieldTunnelTypeIp6:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_IPV6_ETH;
        break;
    case bcmFieldTunnelTypeMpls:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_MPLS_ETH;
        break;
    case bcmFieldTunnelTypeMplsControlWord:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH;
        break;
    case bcmFieldTunnelTypeMplsLabel2:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH;
        break;
    case bcmFieldTunnelTypeMplsLabel2ControlWord:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH;
        break;
    case bcmFieldTunnelTypeMplsLabel3:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_MPLS3_ETH;
        break;
    case bcmFieldTunnelTypeMplsLabel3ControlWord:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH;
        break;
    case bcmFieldTunnelTypeTrill:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_TRILL;
        break;
    case bcmFieldTunnelTypeL2Gre:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_ETH;
        break;
    case bcmFieldTunnelTypeNone:
        *ppdTerminationType = SOC_PPC_PKT_TERM_TYPE_NONE;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_termination_type_ppd_to_bcm(SOC_PPC_PKT_TERM_TYPE ppdTerminationType,
                                           bcm_field_TunnelType_t *bcmTerminationType)
{
    switch (ppdTerminationType) {
    case SOC_PPC_PKT_TERM_TYPE_IPV4_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeIp;
        break;
    case SOC_PPC_PKT_TERM_TYPE_IPV6_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeIp6;
        break;
    case SOC_PPC_PKT_TERM_TYPE_MPLS_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMpls;
        break;
    case SOC_PPC_PKT_TERM_TYPE_CW_MPLS_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsControlWord;
        break;
    case SOC_PPC_PKT_TERM_TYPE_MPLS2_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsLabel2;
        break;
    case SOC_PPC_PKT_TERM_TYPE_CW_MPLS2_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsLabel2ControlWord;
        break;
    case SOC_PPC_PKT_TERM_TYPE_MPLS3_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsLabel3;
        break;
    case SOC_PPC_PKT_TERM_TYPE_CW_MPLS3_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeMplsLabel3ControlWord;
        break;
    case SOC_PPC_PKT_TERM_TYPE_TRILL:
        *bcmTerminationType = bcmFieldTunnelTypeTrill;
        break;
    case SOC_PPC_PKT_TERM_TYPE_ETH:
        *bcmTerminationType = bcmFieldTunnelTypeL2Gre;
        break;
    default:
        *bcmTerminationType = bcmFieldTunnelTypeNone;
    }
    return BCM_E_NONE;
}


int
_bcm_dpp_field_ip_frag_bcm_to_ppd(int unit, bcm_field_IpFrag_t bcmFragInfo,
                                    uint8 *ppdIpFragmented)
{

    if (SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_FIELD_IP_FIRST_FRAGMENT_PARSED, TRUE)) )
    {
        switch (bcmFragInfo) {
        case bcmFieldIpFragNonOrFirst:
            *ppdIpFragmented = 0;
            break;
        case bcmFieldIpFragNotFirst:
            *ppdIpFragmented = 1;
            break;
        default:
            return BCM_E_PARAM;
        }
    }
    else {
        switch (bcmFragInfo) {
        case bcmFieldIpFragNon:
            *ppdIpFragmented = 0;
            break;
        case bcmFieldIpFragAny:
            *ppdIpFragmented = 1;
            break;

        default:
            return BCM_E_PARAM;
        }
    }
    return BCM_E_NONE;
}

STATIC int
_bcm_dpp_field_presel_profile_type_convert(bcm_dpp_field_info_OLD_t *unitData,
                                          _bcm_dpp_field_profile_type_t profile_type,
                                          SOC_PPC_FP_CONTROL_TYPE *control_ndx_type) 
{
    *control_ndx_type = SOC_PPC_NOF_FP_CONTROL_TYPES;
    switch (profile_type) {
    case _bcmDppFieldProfileTypeInPort:
        *control_ndx_type = SOC_PPC_FP_CONTROL_TYPE_IN_PORT_PROFILE;
        break;
    case _bcmDppFieldProfileTypeOutPort:
        *control_ndx_type = SOC_PPC_FP_CONTROL_TYPE_OUT_PORT_PROFILE;
        break;
    case _bcmDppFieldProfileTypeFlpProgramProfile:
        *control_ndx_type = SOC_PPC_FP_CONTROL_TYPE_FLP_PGM_PROFILE;
        break;
    case _bcmDppFieldProfileTypeInterfaceInPort:  
        *control_ndx_type = SOC_PPC_FP_CONTROL_TYPE_IN_TM_PORT_PROFILE;  
        break; 
    default:
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}


STATIC int
_bcm_dpp_field_presel_port_profile_hw_set(bcm_dpp_field_info_OLD_t *unitData,
                                          int core_id,
                                          _bcm_dpp_field_profile_type_t profile_type,
                                          uint8 clear,
                                          uint32 profile,
                                          uint32 port_bitmap[SOC_PPC_FP_NOF_CONTROL_VALS]) 
{
    uint32
        success, 
        sandResult;
    SOC_PPC_FP_CONTROL_INDEX 
        control_ndx;
    SOC_PPC_FP_CONTROL_INFO 
        control_info;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_profile_type_convert(unitData, profile_type, &(control_ndx.type)));
    control_ndx.val_ndx = profile;
    control_ndx.clear_val = clear? TRUE: FALSE;
    sal_memcpy(control_info.val, port_bitmap, sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS);
    sandResult = soc_ppd_fp_control_set(unitData->unitHandle,
                                        core_id,
                                        &control_ndx,
                                        &control_info,
                                        &success);

    BCMDNX_IF_ERR_EXIT(handle_sand_result(sandResult));
    BCMDNX_IF_ERR_EXIT(translate_sand_success_failure(success));
    
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dpp_field_presel_port_profile_alloc_dealloc(bcm_dpp_field_info_OLD_t *unitData,
                                                 _bcm_dpp_field_profile_type_t profile_type,
                                                 uint8 alloc,
                                                 int   nof_cores,
                                                 _bcm_dpp_core_ports_bitmap_t *core_ports_bitmap, 
                                                 uint64 *profile) /* if alloc=0 then IN else OUT */
{
    uint32 
        is_last,
        profile_idx,
        nof_profile_objects,
        empty_bitmap[SOC_PPC_FP_NOF_CONTROL_VALS];
    int core, get_nof_cores;
    _bcm_dpp_core_ports_bitmap_t  
        tmp_bitmap[MAX_NUM_OF_CORES];
    int eq, empty;
    unsigned int preselProfileRefs_element ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    sal_memset(empty_bitmap, 0, sizeof(empty_bitmap[0]) * SOC_PPC_FP_NOF_CONTROL_VALS);

    if(!alloc)
    {
        /*
         * Was:
         *   unitData->preselProfileRefs[profile_type][COMPILER_64_LO(*profile)]-- ;
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELPROFILEREFS.get(unit, profile_type, (COMPILER_64_LO(*profile)), &preselProfileRefs_element)) ;
        if (!SOC_WARM_BOOT(unit)) {  /*Cold boot -  allocate sw-state resources */
            preselProfileRefs_element-- ;
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELPROFILEREFS.set(unit, profile_type, (COMPILER_64_LO(*profile)), preselProfileRefs_element)) ;
        }
        is_last = (preselProfileRefs_element == 0) ? TRUE :FALSE;
        if(is_last)
        {
            for (core = 0; core < SOC_DPP_DEFS_GET(unit, nof_cores); core++) {
                BCMDNX_IF_ERR_EXIT(
                   _bcm_dpp_field_presel_port_profile_hw_set(unitData, 
                                                          core,
                                                          profile_type, 
                                                          TRUE, /* clear */
                                                          COMPILER_64_LO(*profile), 
                                                          empty_bitmap));
            }

        }
    }
    else {
        /* 
         * In case of allocation, init the profile to 0 
         * Run over the possible profiles (1 to 7): 
         * - if not used, init the profile to it 
         * - if used, and equal, use this profile and stop the loop 
         * - if used and not equal, continue if no intersection, 
         * otherwise return error 
         */ 
        /* Init the profile to 0, to detect if no profile was found */
        COMPILER_64_ZERO(*profile);
        
        /* For each profile, make sure there is no intersection with port bitmap */
        for(profile_idx = 1; profile_idx < _BCM_DPP_PRESEL_NOF_PORT_PROFILES; profile_idx++) 
        {
            switch (profile_type) {
            case _bcmDppFieldProfileTypeInterfaceInPort:
            case _bcmDppFieldProfileTypeInPort:
            case _bcmDppFieldProfileTypeOutPort:
                nof_profile_objects = SOC_TMC_NOF_FAP_PORTS_ARAD;
                break;
            case _bcmDppFieldProfileTypeFlpProgramProfile:
                nof_profile_objects = SOC_DPP_DEFS_GET(unit, nof_flp_programs);
                break;
            default:
                return BCM_E_PARAM;
            }

            eq = TRUE;
            empty = TRUE;

            /* Check if there is an intersection between the two port bitmaps */
            sal_memset(tmp_bitmap, 0, sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS);
            BCMDNX_IF_ERR_EXIT(
                _bcm_dpp_field_presel_port_profile_get(unit,
                                                       profile_type,
                                                       profile_idx,
                                                       &get_nof_cores,
                                                       tmp_bitmap));
            if (get_nof_cores != nof_cores) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG_NO_UNIT("Num of cores mismatch get=%d set=%d"),
                                  get_nof_cores,
                                  nof_cores));


            }
            /* Check on all cores : If equal on all cores use this profile, otherwise take an empty one.
               intersection on any core is considred error.*/
            for (core=0; core < nof_cores; core++) {

                if(0 != sal_memcmp(tmp_bitmap[core], empty_bitmap, sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS))
                {
                    empty = FALSE;
                /* If port bitmaps are equal, then the same profile
                 * should be given to this PFG as well.
                 */
                    if(0 != sal_memcmp(core_ports_bitmap[core], tmp_bitmap[core], sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS)) {
                        eq = FALSE;
                        /* Return error if intersection */
                        SHR_BITAND_RANGE(tmp_bitmap[core], core_ports_bitmap[core], 0, nof_profile_objects, tmp_bitmap[core]);
                        if (0 != sal_memcmp(tmp_bitmap[core], empty_bitmap, sizeof(uint32) * SOC_PPC_FP_NOF_CONTROL_VALS)) {
                            BCM_ERR_EXIT_NO_MSG(BCM_E_EXISTS);
                        }
                        break;
                    }
                }
            }


            if ( eq || empty ) {
                COMPILER_64_SET(*profile, 0, profile_idx);
            }

            if (eq) {
                break;
            }

        }

        if(COMPILER_64_IS_ZERO(*profile)) {
            BCM_ERR_EXIT_NO_MSG(BCM_E_RESOURCE);
        }
        for (core=0; core < nof_cores; core++) {
            BCMDNX_IF_ERR_EXIT(
                _bcm_dpp_field_presel_port_profile_hw_set(unitData, 
                                                          core,
                                                          profile_type, 
                                                          FALSE, /* don't clear */
                                                          COMPILER_64_LO(*profile), 
                                                          core_ports_bitmap[core]));
        }
        /*
         * Was:
         *   unitData->preselProfileRefs[profile_type][COMPILER_64_LO(*profile)]++;
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELPROFILEREFS.get(unit, profile_type, (COMPILER_64_LO(*profile)), &preselProfileRefs_element)) ;
        if (!SOC_WARM_BOOT(unit)) {  /*Cold boot -  allocate sw-state resources */
            preselProfileRefs_element++ ;
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELPROFILEREFS.set(unit, profile_type, (COMPILER_64_LO(*profile)), preselProfileRefs_element)) ;
        }
    }

#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
    _bcm_dpp_field_presel_all_wb_save(unitData, NULL, NULL);
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_port_profile_get(int unit,
                                       _bcm_dpp_field_profile_type_t profile_type,
                                       uint32 profile,
                                       int    *nof_cores,
                                       _bcm_dpp_core_ports_bitmap_t *core_port_bitmap)
{
    _DPP_FIELD_COMMON_LOCALS;
    
    uint32 
        sandResult;
    SOC_PPC_FP_CONTROL_INDEX 
        control_ndx;
    SOC_PPC_FP_CONTROL_INFO 
        control_info;
	int core;
    int
        dpp_field_unit_lock_was_taken ;

    BCMDNX_INIT_FUNC_DEFS;
    dpp_field_unit_lock_was_taken = 0 ;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    _DPP_FIELD_UNIT_LOCK(unitData);
    dpp_field_unit_lock_was_taken = 1 ;

    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
    
    if (SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit)) {
        *nof_cores = 1;
    } else {
        *nof_cores = SOC_DPP_DEFS_GET(unit, nof_cores);
    }
	
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_profile_type_convert(unitData, profile_type, &(control_ndx.type)));
    control_ndx.val_ndx = profile;

    for (core = 0; core < *nof_cores; core++) {
        sal_memset(core_port_bitmap[core], 0, sizeof(_bcm_dpp_core_ports_bitmap_t));

        sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                        core,
                                        &control_ndx,
                                        &control_info);

        result = handle_sand_result(sandResult);


        BCMDNX_IF_ERR_EXIT(result);

        sal_memcpy(core_port_bitmap[core], control_info.val, sizeof(control_info.val[0]) * SOC_PPC_FP_NOF_CONTROL_VALS);
    }

     _DPP_FIELD_UNIT_UNLOCK(unitData);
    dpp_field_unit_lock_was_taken = 0 ;

exit:
    if (dpp_field_unit_lock_was_taken)
    {
        _DPP_FIELD_UNIT_UNLOCK(unitData);
    }
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_port_profile_set(int unit,
                                       _bcm_dpp_field_profile_type_t profile_type,
                                       int  nof_cores,
                                       _bcm_dpp_core_ports_bitmap_t *core_port_bitmap, 
                                       uint64 *profile)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);

    _DPP_FIELD_UNIT_LOCK(unitData);

    if (SOC_DPP_CORE_MODE_IS_SINGLE_CORE(unit)) {
        nof_cores = 1;
    }

    result = _bcm_dpp_field_presel_port_profile_alloc_dealloc(unitData, 
                                                              profile_type,
                                                              TRUE, /* Alloc */
															  nof_cores,
                                                              core_port_bitmap, 
                                                              profile);
    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_dpp_clear_core_ports
 *  Purpose
 *     Clear core ports array.
 */
void _bcm_dpp_clear_core_ports(int                  nof_cores,
                               _bcm_dpp_core_ports_bitmap_t *core_port_bitmap)

{
    int core; 
	

    for (core = 0; core < nof_cores; core++) {
        sal_memset(core_port_bitmap[core], 0x00, sizeof(_bcm_dpp_core_ports_bitmap_t));
    }
}
/* 
 * Get the profile for a specific profile type for this entry. 
 * If the entry is preselector, all the profile types are relevant 
 * For other types of entries, only the FLP processing profile is relevant. 
 *  Get the profile value from the SW state, since the entry may not be in the HW.
 * The returned profile is 0 of not found or found 0. 
 */
int
_bcm_dpp_field_presel_port_entry_profile_get(bcm_dpp_field_info_OLD_t *unitData,
                                             _bcm_dpp_field_profile_type_t profile_type,
                                             bcm_field_entry_t entry,
                                             uint64 *profile)
{
    _bcm_dpp_field_profile_type_t profile_type_lcl;
    uint32 
        qual_ndx,
        sandResult;
    _bcm_dpp_field_entry_type_t entryType ;
    _bcm_dpp_field_qual_t entryQual ;
    bcm_field_stage_t bcm_stage;
    _bcm_dpp_field_stage_idx_t stage_id;
    int index;
    int pfg_ndx;
    uint64 entryDe_qualData;
    uint8 is_second_pass;
    SOC_PPC_FP_QUAL_TYPE entryDe_hwType;
    bcm_field_entry_t ent;

    SOC_PPC_PMF_PFG_INFO pfgInfo;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* Init the profile to zero */
    COMPILER_64_SET(*profile, 0, 0);    
     
    /* Preselector case */
    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        
        if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(entry)) {
            bcm_stage =  _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(entry) ; 
            is_second_pass = _BCM_DPP_FIELD_ENTRY_IS_PRESEL_STAGGERED(entry);
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit,entry, bcm_stage, is_second_pass,&index, &stage_id)); 
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_index_to_pfg(unit, index,&bcm_stage ,&pfg_ndx));
        }
        else {
            pfg_ndx = (entry & (~BCM_FIELD_QUALIFY_PRESEL));
            bcm_stage = bcmFieldStageIngress;
        }

        /*convert bcm stage to SOC stage*/
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_ppd_stage_from_bcm_stage(unitData,bcm_stage,&(pfgInfo.stage)));

        sandResult = soc_ppd_fp_packet_format_group_get(unitData->unitHandle,
                                                        pfg_ndx,
                                                        &pfgInfo);
        BCMDNX_IF_ERR_EXIT(handle_sand_result(sandResult));

        for(qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX; qual_ndx++)
        {
            switch (pfgInfo.qual_vals[qual_ndx].type) {
            case SOC_PPC_FP_QUAL_IRPP_IN_PORT_KEY_GEN_VAR_PS:
                profile_type_lcl = _bcmDppFieldProfileTypeInPort;
                break;
            case SOC_PPC_FP_QUAL_ERPP_OUT_PP_PORT_PMF_DATA_PS:
                profile_type_lcl = _bcmDppFieldProfileTypeOutPort;
                break;
            case SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE:
                profile_type_lcl = _bcmDppFieldProfileTypeFlpProgramProfile;
                break;
            default:
                continue; /* continue to next index */
            }

            if (profile_type_lcl == profile_type) {
                COMPILER_64_SET(*profile, 0, pfgInfo.qual_vals[qual_ndx].val.arr[0]);     
                break;
            }
        }
    }
    else {
        if (profile_type == _bcmDppFieldProfileTypeFlpProgramProfile) {
            int32 commonHandle ;
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_common_pointer(unitData,
                                                                        entry,
                                                                        &entryType,&commonHandle));
            ent = commonHandle - 1;
            if (entryType == _bcmDppFieldEntryTypeDirExt) {
                /*entryQual = (common)->entryQual;*/
                for (qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_ndx++) {
                    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.get(unit, ent, qual_ndx, &entryDe_hwType));  

                    if (entryDe_hwType == SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE) {
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.qualData.get(unit, ent, qual_ndx, &entryDe_qualData));
                        COMPILER_64_SET(*profile, 0, COMPILER_64_LO(entryDe_qualData));  
                        break;
                    }
                }
            }
            else { /* internal/external TCAM */
                /*
                 * Was:
                 *
                 *  entryQual = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                 *                                        _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
                 *  for (qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_ndx++) {
                 *      if (entryQual[qual_ndx].hwType == SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE) {
                 *          COMPILER_64_SET(*profile, 0, COMPILER_64_LO(entryQual[qual_ndx].qualData));     
                 *          break;
                 *      }
                 *  }
                 */
                 for (qual_ndx = 0; qual_ndx < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX; qual_ndx++) {
                    _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,
                        _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                        _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry),
                        qual_ndx,
                        entryQual);
                    if (entryQual.hwType == SOC_PPC_FP_QUAL_FWD_PRCESSING_PROFILE) {
                        COMPILER_64_SET(*profile, 0, COMPILER_64_LO(entryQual.qualData));     
                        break;
                    }
                 }
            }
        }
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_port_profile_clear_type(bcm_dpp_field_info_OLD_t *unitData,
                                              _bcm_dpp_field_profile_type_t profile_type,
                                              bcm_field_entry_t entry)
{
    uint64 profile;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* Get the current profile */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_port_entry_profile_get(unitData, profile_type, entry, &profile));

    /* Dealloc this profile if not null */
    if (COMPILER_64_IS_ZERO(profile) == FALSE) {
        BCMDNX_IF_ERR_EXIT(
            _bcm_dpp_field_presel_port_profile_alloc_dealloc(unitData, 
                                                             profile_type, 
                                                             FALSE, /* Dealloc */
															 0,
                                                             NULL, /* For dealloc, should not be used */
                                                             &profile));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_presel_port_profile_clear_all(bcm_dpp_field_info_OLD_t *unitData,
                                              bcm_field_entry_t entry)
{
    _bcm_dpp_field_profile_type_t profile_type;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    /* Clear all the profiles */
    for (profile_type = 0; profile_type < _bcmDppFieldProfileTypeCount; profile_type++) {
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_port_profile_clear_type(unitData, profile_type, entry));
    }


exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *   Function
 *      _bcm_dpp_field_presel_to_bitmap
 *   Purpose
 *      map between preselector id with stage to a specific preselector id inside a bitmap
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) int with_id = TRUE if caller supplies ID, FALSE otherwise
 *      (in/out) bcm_field_presel_t *presel = pointer to the (place to put) ID
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
int
_bcm_dpp_field_presel_to_bitmap(int unit,
                                bcm_field_presel_t presel,
                                bcm_field_stage_t  stage,
                                uint8 is_second_pass,
                                bcm_field_presel_t *presel_bitmap_id,
                                _bcm_dpp_field_stage_idx_t *stage_id)
{
    int ing_pmf_start_offset, eg_pmf_start_offset, flp_start_offset, slb_start_offset, start_offset, second_pass_start_offset;
    int presel_id  = _BCM_DPP_FIELD_PRESEL_ID_FROM_ENTRY(presel); /* mask the stage bits from presel-ids */

    BCMDNX_INIT_FUNC_DEFS;
    
   

    if ( (stage != bcmFieldStageIngress ) && (stage != bcmFieldStageEgress ) && (stage != bcmFieldStageHash) && (stage != bcmFieldStageExternal)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("not supported for stage %d"),
                              stage));
    }

    /*Do not change order - oreder is important*/
    ing_pmf_start_offset     = 0;
    eg_pmf_start_offset      = ing_pmf_start_offset + SOC_DPP_DEFS_GET(unit,nof_ingress_pmf_program_selection_lines);
    flp_start_offset         = eg_pmf_start_offset  + SOC_DPP_DEFS_GET(unit,nof_egress_pmf_program_selection_lines);
    slb_start_offset         = flp_start_offset     + SOC_DPP_DEFS_GET(unit,nof_flp_program_selection_lines);
    second_pass_start_offset = slb_start_offset     + SOC_DPP_DEFS_GET(unit,nof_slb_program_selection_lines);

    switch (stage)
    {
    case bcmFieldStageEgress:
            if (presel_id >= SOC_DPP_DEFS_GET(unit,nof_egress_pmf_program_selection_lines))
            {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " preselector ID %d for stage %d"),
                              unit,
                              presel_id, 
                              stage));

            }
            start_offset = eg_pmf_start_offset ;
            *stage_id = _BCM_DPP_FIELD_STAGE_INDEX_EGRESS;
            break;
    case bcmFieldStageExternal:
            if (presel_id >= SOC_DPP_DEFS_GET(unit,nof_flp_program_selection_lines))
            {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " preselector ID %d for stage %d"),
                              unit,
                              presel_id, 
                              stage));

            }
            start_offset = flp_start_offset ;
            *stage_id = _BCM_DPP_FIELD_STAGE_INDEX_ING_EXTERNAL;
            break;
    case bcmFieldStageHash:
            if (presel_id >= SOC_DPP_DEFS_GET(unit,nof_slb_program_selection_lines))
            {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " preselector ID %d for stage %d"),
                              unit,
                              presel_id, 
                              stage));

            }
            start_offset = slb_start_offset ;
            *stage_id = _BCM_DPP_FIELD_STAGE_INDEX_ING_SLB;
            break;
    default:
            if (presel_id >= SOC_DPP_DEFS_GET(unit,nof_ingress_pmf_program_selection_lines))
            {
                 BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " preselector ID %d for stage %d"),
                              unit,
                              presel_id, 
                              stage));

            }
            start_offset = 0 ;
            *stage_id = _BCM_DPP_FIELD_STAGE_INDEX_INGRESS;
            break;
    }

    *presel_bitmap_id = presel_id  + start_offset + ((is_second_pass) ? second_pass_start_offset : 0 );

exit:
    BCMDNX_FUNC_RETURN;
}


/*
 *   Function
 *      _bcm_dpp_field_presel_index_to_pfg
 *   Purpose
 *      map between preselector id with stage to a packet field group (relative presel id)
*/

int
_bcm_dpp_field_presel_index_to_pfg(int unit,
                             bcm_field_presel_t presel_id,
                             bcm_field_stage_t *stage,
                             int *pfg_ndx)
{
    int ing_pmf_start_offset, eg_pmf_start_offset, flp_start_offset, slb_start_offset, second_pass_start_offset ;

    BCMDNX_INIT_FUNC_DEFS;

    /*Do not change order - oreder is important*/
    ing_pmf_start_offset     = 0;
    eg_pmf_start_offset      = ing_pmf_start_offset + SOC_DPP_DEFS_GET(unit,nof_ingress_pmf_program_selection_lines);
    flp_start_offset         = eg_pmf_start_offset  + SOC_DPP_DEFS_GET(unit,nof_egress_pmf_program_selection_lines);
    slb_start_offset         = flp_start_offset  + SOC_DPP_DEFS_GET(unit,nof_flp_program_selection_lines);
    second_pass_start_offset = slb_start_offset     + SOC_DPP_DEFS_GET(unit,nof_slb_program_selection_lines);


    if ( ( presel_id < ing_pmf_start_offset ) ||  ( presel_id > ( second_pass_start_offset + SOC_DPP_DEFS_GET(unit,nof_ingress_pmf_program_selection_lines) ) ) )
    {
           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " preselector ID %d "),
                              unit,
                              presel_id));
    }
    else
    {
        if ( presel_id < eg_pmf_start_offset )  
        {
            *stage = bcmFieldStageIngress;
            *pfg_ndx = presel_id ; 
        }
        else if ( presel_id < flp_start_offset)  
        {
            *stage = bcmFieldStageEgress;
            *pfg_ndx = presel_id - eg_pmf_start_offset ; 
        }
        else if ( presel_id < slb_start_offset)  
        {
            *stage = bcmFieldStageExternal;
            *pfg_ndx = presel_id - flp_start_offset ; 
        }
        else if ( presel_id < second_pass_start_offset)  
        {
            *stage = bcmFieldStageHash;
            *pfg_ndx = presel_id - slb_start_offset ; 
        }
        else 
        {
            *stage = bcmFieldStageIngress;
            *pfg_ndx = presel_id - second_pass_start_offset ; 
        }
    }

exit:
    BCMDNX_FUNC_RETURN;
}



int
_bcm_dpp_field_ip_frag_ppd_to_bcm(int unit, uint8 ppdIpFragmented,
                                    bcm_field_IpFrag_t *bcmFragInfo)
{
    switch (ppdIpFragmented) {
    case 0:
        *bcmFragInfo = ( SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_FIELD_IP_FIRST_FRAGMENT_PARSED, TRUE)) ) ? bcmFieldIpFragNonOrFirst : bcmFieldIpFragNon;
        break;
    case 1:
        *bcmFragInfo = ( SOC_IS_JERICHO(unit) && (soc_property_get(unit, spn_FIELD_IP_FIRST_FRAGMENT_PARSED, TRUE)) ) ? bcmFieldIpFragNotFirst : bcmFieldIpFragAny;
        break;
    default:
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_entry_qual_dump(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_stage_idx_t stage,
                               const _bcm_dpp_field_qual_t *qualDataBase,
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                               const SOC_PPC_FP_QUAL_VAL *qualHwData,
#endif
                               unsigned int index,
                               int dumpRanges,
                               const char *prefix)
{
    bcm_mac_t macData;
    bcm_mac_t macMask;
    bcm_ip6_t ipv6Data;
    bcm_ip6_t ipv6Mask;
    bcm_field_header_format_t hdrFmt;
    uint64 data;
    uint64 mask;
    char format[32];
    unsigned int limit;
    unsigned int bits;
    unsigned int offset;
    int result;
    int retVal = 0;
    int unit = unitData->unit;
#ifdef _BCM_SPECIAL_SHOW_FOR_RANGE
    int32 *qualMap ;
    int32 qualMaps_handle ;
#endif  /* _BCM_SPECIAL_SHOW_FOR_RANGE */
    _bcm_dpp_field_qual_t qualData ;

    qualData = qualDataBase[index] ;
    if (BCM_FIELD_ENTRY_INVALID == qualData.hwType) {
        /* nothing to display; don't do anything */
        retVal = 0;
        goto exit ;
    }
#ifdef _BCM_SPECIAL_SHOW_FOR_RANGE
    retVal = FIELD_ACCESS_QUALMAPS.get(unit, (qualData.qualType), &qualMaps_handle) ;
    if (retVal != BCM_E_NONE) {
        goto exit ;
    }
    SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_ELEMENT(unit,qualMaps_handle,qualMap) ;
    if ((BCM_FIELD_ENTRY_INVALID != qualData.qualType) &&
        (qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS] &
         _BCM_DPP_QUAL_FLAGS_RNG_VALID_BITS)) {
        /* it is a range type */
        if (dumpRanges) {
            result = _bcm_dpp_field_entry_range_dump(unitData,
                                                     &qualDataBase[0],
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                                                     qualHwData,
#endif
                                                     prefix);
            if (BCM_E_NONE == result) {
                retVal = 2; /* dumped the L4OPS range qualifier */
            } else {
                retVal = result; /* return the error that was encountered */
            }
        }
    } else
#endif  /* _BCM_SPECIAL_SHOW_FOR_RANGE */
    {
        if (BCM_FIELD_ENTRY_INVALID != qualData.qualType) {
            /* most stuff below uses this header */
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
            LOG_CLI((BSL_META_U(unit,
                                "%s    %s (%d) -> %s (%d)\n"),
                     prefix,
                     _bcm_dpp_field_qual_name[qualData.qualType],
                     qualData.qualType,
                     SOC_PPC_FP_QUAL_TYPE_to_string(qualData.hwType),
                     qualData.hwType));
#else /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
            LOG_CLI((BSL_META_U(unit,
                                "%s    %s (%d)\n"),
                     prefix,
                     _bcm_dpp_field_qual_name[qualData.qualType],
                     qualData.qualType));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
        }
        /* build generic format based upon qualifier exposed size */
        result = _bcm_dpp_ppd_qual_bits(unit,
                                        stage,
                                        qualData.hwType,
                                        &bits,
                                        NULL,
                                        NULL);
        if (BCM_E_NONE != result) {
            retVal = result;
            goto exit ;
        }
        if (bits > 32) {
            sal_snprintf(&(format[0]),
                         sizeof(format) - 1,
                         "%%s      %%0%dX%%08X/%%0%dX%%08X\n",
                         (bits - 29) >> 2,
                         (bits - 29) >> 2);
        } else {
            sal_snprintf(&(format[0]),
                         sizeof(format) - 1,
                         "%%s      %%0%dX/%%0%dX\n",
                         (bits + 3) >> 2,
                         (bits + 3) >> 2);
        }
        switch (qualData.qualType) {
        case bcmFieldQualifyCount:
            if (_BCM_DPP_FIELD_PPD_QUAL_VALID(qualData.hwType)) {
                limit = 0;
                /* try mapping as (user-defined) data qualifier */
                result = _bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                             qualData.hwType,
                                                             &offset);
                if (BCM_E_NONE == result) {
                    /* mapped PPD data field to BCM data field */
                    limit = 1;
                }
                switch (limit) {
                case 1:
                    /* programmable fields */
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
                    LOG_CLI((BSL_META_U(unit,
                                        "%s    DataField(%d) -> %s (%d)\n"),
                             prefix,
                             offset,
                             SOC_PPC_FP_QUAL_TYPE_to_string(qualData.hwType),
                             qualData.hwType));
#else /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
                    LOG_CLI((BSL_META_U(unit,
                                        "%s    DataField(%d)\n"),
                             prefix,
                             offset));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
                    if (bits > 32) {
                        LOG_CLI((format,
                                 prefix,
                                 COMPILER_64_HI(qualData.qualData),
                                 COMPILER_64_LO(qualData.qualData),
                                 COMPILER_64_HI(qualData.qualMask),
                                 COMPILER_64_LO(qualData.qualMask)));
                    } else {
                            LOG_CLI((format,
                                     prefix,
                                     COMPILER_64_LO(qualData.qualData),
                                     COMPILER_64_LO(qualData.qualMask)));
                    }
                    retVal = 1; /* displayed a qualifier */
                default:
                    /* nothing to display here */
                    break;
                } /* switch (limit) */
            } /* if (hardware type is valid) */
            break;
        case bcmFieldQualifySrcMac:
        case bcmFieldQualifyDstMac:
            /* MAC address based qualifiers */
            data = qualData.qualData;
            mask = qualData.qualMask;
            for (offset = 0; offset < 6; offset++) {
                macData[5 - offset] = COMPILER_64_LO(data) & 0xFF;
                macMask[5 - offset] = COMPILER_64_LO(mask) & 0xFF;
                COMPILER_64_SHR(data, 8);
                COMPILER_64_SHR(mask, 8);
            }
            LOG_CLI((BSL_META_U(unit,
                                "%s      "
                     FIELD_MACA_FORMAT
                                "/"
                                FIELD_MACA_FORMAT
                                "\n"),
                     prefix,
                     FIELD_MACA_SHOW(macData),
                     FIELD_MACA_SHOW(macMask)));
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifySrcIp6High:
        case bcmFieldQualifyDstIp6High:
            /* IPv6 (upper half) address based qualifiers */
            data = qualData.qualData;
            mask = qualData.qualMask;
            for (offset = 0; offset < 8; offset++) {
                ipv6Data[7 - offset] = COMPILER_64_LO(data) & 0xFF;
                ipv6Mask[7 - offset] = COMPILER_64_LO(mask) & 0xFF;
                ipv6Data[15 - offset] = 0;
                ipv6Mask[15 - offset] = 0;
                COMPILER_64_SHR(data, 8);
                COMPILER_64_SHR(mask, 8);
            }
            LOG_CLI((BSL_META_U(unit,
                                "%s      "
                     FIELD_IPV6A_FORMAT
                                "/\n%s      "
                                FIELD_IPV6A_FORMAT
                                "\n"),
                     prefix,
                     FIELD_IPV6A_SHOW(ipv6Data),
                     prefix,
                     FIELD_IPV6A_SHOW(ipv6Mask)));
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifySrcIp6Low:
        case bcmFieldQualifyDstIp6Low:
            /* IPv6 (lower half) address based qualifiers */
            data = qualData.qualData;
            mask = qualData.qualMask;
            for (offset = 0; offset < 8; offset++) {
                ipv6Data[15 - offset] = COMPILER_64_LO(data) & 0xFF;
                ipv6Mask[15 - offset] = COMPILER_64_LO(mask) & 0xFF;
                ipv6Data[7 - offset] = 0;
                ipv6Mask[7 - offset] = 0;
                COMPILER_64_SHR(data, 8);
                COMPILER_64_SHR(mask, 8);
            }
            LOG_CLI((BSL_META_U(unit,
                                "%s      "
                     FIELD_IPV6A_FORMAT
                                "/\n%s      "
                                FIELD_IPV6A_FORMAT
                                "\n"),
                     prefix,
                     FIELD_IPV6A_SHOW(ipv6Data),
                     prefix,
                     FIELD_IPV6A_SHOW(ipv6Mask)));
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifySrcIp:
        case bcmFieldQualifyDstIp:
        case bcmFieldQualifyInnerSrcIp:
        case bcmFieldQualifyInnerDstIp:
            /* IPv4 address based qualifiers */
            LOG_CLI((BSL_META_U(unit,
                                "%s      %08X/%08X (%u.%u.%u.%u/%u.%u.%u.%u)\n"),
                     prefix,
                     (uint32)(COMPILER_64_LO(qualData.qualData) & 0xFFFFFFFF),
                     (uint32)(COMPILER_64_LO(qualData.qualMask) & 0xFFFFFFFF),
                     (COMPILER_64_LO(qualData.qualData) >> 24) & 0xFF,
                     (COMPILER_64_LO(qualData.qualData) >> 16) & 0xFF,
                     (COMPILER_64_LO(qualData.qualData) >> 8) & 0xFF,
                     COMPILER_64_LO(qualData.qualData) & 0xFF,
                     (COMPILER_64_LO(qualData.qualMask) >> 24) & 0xFF,
                     (COMPILER_64_LO(qualData.qualMask) >> 16) & 0xFF,
                     (COMPILER_64_LO(qualData.qualMask) >> 8) & 0xFF,
                     COMPILER_64_LO(qualData.qualMask) & 0xFF));
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifyHeaderFormat:
            result = _bcm_dpp_field_HeaderFormat_ppd_to_bcm(unit,
                                                            COMPILER_64_LO(qualData.qualData),
                                                            COMPILER_64_LO(qualData.qualMask) & 0x3F,
                                                            &hdrFmt);
            if (BCM_E_NONE == result) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s      %s (%d)\n"),
                         prefix,
                         _bcm_dpp_field_header_format_names[hdrFmt],
                         hdrFmt));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "%s      PPD PKT_HDR_STK_TYPE %d\n"),
                         prefix,
                         hdrFmt));
            }
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifyColor:
            /*
             *  NOTE: does not translate back to BCM because there is no
             *  set of BCM strings and the PPD type is simple enough to
             *  parse out inline.
             */
            switch (COMPILER_64_LO(qualData.qualData) ) {
            case 0:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      GREEN (%d)\n"),
                         prefix,
                         BCM_FIELD_COLOR_GREEN));
                break;
            case 1:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      YELLOW (%d)\n"),
                         prefix,
                         BCM_FIELD_COLOR_YELLOW));
                break;
            case 2:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      RED (%d)\n"),
                         prefix,
                         BCM_FIELD_COLOR_RED));
                break;
            case 3:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      BLACK (%d)\n"),
                         prefix,
                         BCM_FIELD_COLOR_RED));
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      unknown PPD color %d\n"),
                         prefix,
                         COMPILER_64_LO(qualData.qualData)));
            }
            retVal = 1; /* displayed a qualifier */
            break;
        case bcmFieldQualifyIngressStpState:
            switch (COMPILER_64_LO(qualData.qualData)) {
            case SOC_PPC_PORT_STP_STATE_BLOCK:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      BLOCK (%d)\n"),
                         prefix,
                         BCM_STG_STP_BLOCK));
                break;
            case SOC_PPC_PORT_STP_STATE_LEARN:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      LEARN (%d)\n"),
                         prefix,
                         BCM_STG_STP_LEARN));
                break;
            case SOC_PPC_PORT_STP_STATE_FORWARD:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      FORWARD (%d)\n"),
                         prefix,
                         BCM_STG_STP_FORWARD));
                break;
            default:
                LOG_CLI((BSL_META_U(unit,
                                    "%s      unknown PPD STP state %d\n"),
                         prefix,
                         COMPILER_64_LO(qualData.qualData)));
            }
            retVal = 1; /* displayed a qualifier */
            break;
        default:
            /* general case: display according to nybble count */
            if (bits > 32) {
                LOG_CLI((format,
                         prefix,
                         COMPILER_64_HI(qualData.qualData),
                         COMPILER_64_LO(qualData.qualData),
                         COMPILER_64_HI(qualData.qualMask),
                         COMPILER_64_LO(qualData.qualMask)));
            } else {
                LOG_CLI((format,
                         prefix,
                         COMPILER_64_LO(qualData.qualData),
                         COMPILER_64_LO(qualData.qualMask)));
            }
            retVal = 1; /* displayed a qualifier */
        } /* switch (entryData[index].qual[index].type) */
    }
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD || _BCM_DPP_FIELD_DUMP_VERIFY_PPD
    if (1 == retVal) {
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
        if (qualHwData) {
            for (offset = 0;
                 offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX;
                 offset++) {
                if (qualHwData[offset].type == qualData.hwType) {
                    /* found the hardware qual matching this one */
                    break;
                }
            }
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
            LOG_CLI((BSL_META_U(unit,
                                "%s      %08X%08X/%08X%08X (expected)\n"),
                     prefix,
                     COMPILER_64_HI(qualData.qualData),
                     COMPILER_64_LO(qualData.qualData),
                     COMPILER_64_HI(qualData.qualMask),
                     COMPILER_64_LO(qualData.qualMask)));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
            if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) {
#if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD
                LOG_CLI((BSL_META_U(unit,
                                    "%s      %08X%08X/%08X%08X (actual)\n"),
                         prefix,
                         qualHwData[offset].val.arr[1],
                         qualHwData[offset].val.arr[0],
                         qualHwData[offset].is_valid.arr[1],
                         qualHwData[offset].is_valid.arr[0]));
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD */
                if ((COMPILER_64_HI(qualData.qualData) !=
                     qualHwData[offset].val.arr[1]) ||
                    (COMPILER_64_LO(qualData.qualData) !=
                     qualHwData[offset].val.arr[0]) ||
                    (COMPILER_64_HI(qualData.qualMask) !=
                     qualHwData[offset].is_valid.arr[1]) ||
                    (COMPILER_64_LO(qualData.qualMask) !=
                     qualHwData[offset].is_valid.arr[0])) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      (BCM and PPD data mismatch)\n"),
                             prefix));
#if _BCM_DPP_FIELD_DUMP_VERIFY_ERROR
                    retVal = BCM_E_INTERNAL;
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_ERROR */
                }
            } else { /* if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) */
                LOG_CLI((BSL_META_U(unit,
                                    "%s      (unable to find hardware value)\n"),
                         prefix));
#if _BCM_DPP_FIELD_DUMP_VERIFY_ERROR
                retVal = BCM_E_INTERNAL;
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_ERROR */
            } /* if (offset < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX) */
        } /* if (qualHwData) */
#else /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
        LOG_CLI((BSL_META_U(unit,
                            "%s      %08X%08X/%08X%08X\n"),
                 prefix,
                 COMPILER_64_HI(qualData.qualData),
                 COMPILER_64_LO(qualData.qualData),
                 COMPILER_64_HI(qualData.qualMask),
                 COMPILER_64_LO(qualData.qualMask)));
#endif /* _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
    }
#endif /* _BCM_DPP_FIELD_DUMP_INTERMIX_PPD || _BCM_DPP_FIELD_DUMP_VERIFY_PPD */
exit:
    return retVal;
}
#endif /* def BROADCOM_DEBUG */

int
_bcm_dpp_field_group_id_to_name(int unit, 
                                    _bcm_dpp_field_grp_idx_t group,
                                    char *group_name, 
                                    int max_length)
{
    int i;

    BCMDNX_INIT_FUNC_DEFS;

    max_length = (max_length > BCM_FIELD_MAX_NAME_LEN)?BCM_FIELD_MAX_NAME_LEN:max_length;
    
    for (i=0; i<max_length-1; ++i ) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupConfigNames.get(unit, group, i,  (uint8 *)(&group_name[i]))); 
        if (group_name[i] == 0) break;
    }

    group_name[i] = '\0';

    if (i==0) {
        if (max_length < 32) {
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        
        sprintf(group_name, "Group %3d", group);
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dump_single_qual_name(int unit,
                                     int qual_id,
                                     char *qual_name,
                                     int max_length)
{
    int i = 0;

    BCMDNX_INIT_FUNC_DEFS;

    max_length = (max_length > BCM_FIELD_MAX_NAME_LEN) ? BCM_FIELD_MAX_NAME_LEN : max_length;

    for (i = 0; i < BCM_FIELD_MAX_NAME_LEN; ++i) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.dataQualifierNames.get(unit, qual_id, i, (uint8 *)&(qual_name[i])));
        if (qual_name[i] == 0) {
            break;
        }
    }

    qual_name[i] = '\0';
    if (i == 0) {
        if (max_length < 32) {
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        sprintf(qual_name, "%s", SOC_PPC_FP_QUAL_TYPE_to_string(qual_id));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_dump_single_presel_name(int unit,
                                       bcm_field_presel_t presel_id,
                                       char *presel_name,
                                       int max_length)
{
    int i;

    BCMDNX_INIT_FUNC_DEFS;

    max_length = (max_length > BCM_FIELD_MAX_NAME_LEN) ? BCM_FIELD_MAX_NAME_LEN : max_length;

    for (i = 0; i < BCM_FIELD_MAX_NAME_LEN; ++i) {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselConfigNames.get(unit, presel_id, i, (uint8 *)&(presel_name[i])));
        if (presel_name[i] == 0) {
            break;
        }
    }

    presel_name[i] = '\0';
    if (i == 0) {
        if (max_length < 32) {
            BCMDNX_IF_ERR_EXIT(BCM_E_PARAM);
        }
        sprintf(presel_name, "Presel %d", presel_id);
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_group_hw_handle_to_name(int unit,
                                    uint32 hw_handle,
                                    char *group_name,
                                    int max_length)
{
    /* coverity[stack_use_local_overflow:FALSE] */
    SOC_PPC_FP_DATABASE_INFO
      fp_database_info;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.arad.tm.pmf.fp_info.db_info.get(
            unit,
            SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF, /* Default stage */
            hw_handle,
            &fp_database_info
          ));

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_group_id_to_name(unit, fp_database_info.sw_db_id, group_name, max_length));

exit:
    BCMDNX_FUNC_RETURN;
}

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_group_dump(bcm_dpp_field_info_OLD_t *unitData,
                          _bcm_dpp_field_grp_idx_t group,
                          const char *prefix,
                          int entries)
{
    CONST _bcm_dpp_field_group_t *groupData = NULL;
    _bcm_dpp_field_ent_idx_t entry;
    unsigned int entryBias;
    unsigned int count;
    char groupName[BCM_FIELD_MAX_NAME_LEN] = "";
    int result = BCM_E_NONE;
    char *newPrefix = NULL;
    _bcm_dpp_field_entry_type_t entryType;    
    _bcm_dpp_field_ent_idx_t tcam_entry_limit;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    _bcm_dpp_field_grp_idx_t groupLimit;
    _bcm_dpp_field_presel_idx_t preselLimit;
    _bcm_dpp_field_ent_idx_t  entryDe_entryNext;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.get_ptr(unit, group, &groupData));
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_group_id_to_name(unit, group, groupName, sizeof(groupName)));

    LOG_CLI((BSL_META_U(unit,
                        "%sGroup "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        " [%s]"
                        "\n"),
             prefix,
             group,
             groupName));

    LOG_CLI((BSL_META_U(unit,
                        "%s  Flags       = %08X\n"), prefix, groupData->groupFlags));
#if _BCM_DPP_FIELD_DUMP_SYM_FLAGS
    LOG_CLI((BSL_META_U(unit,
                        "%s    %s %s %s %s %s %s %s %s\n"),
             prefix,
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE)?"+USED":"-used",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW)?"+IN_HW":"-in_hw",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PHASE)?"PH1":"PH0",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_WITH_CE_ID)?"+HW_ID":"-hw_id",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_CHG_ENT)?"+CHGENT":"-chgent",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_ADD_ENT)?"+ADDENT":"-addent",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_INGRESS)?"IGR":(groupData->groupFlags & _BCM_DPP_FIELD_GROUP_EGRESS)?"EGR":"?d?",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_AUTO_SZ)?"+ASZ":"-asz"));
    LOG_CLI((BSL_META_U(unit,
                        "%s    %s %s %s %s %s %s %s %s %s\n"),
             prefix,
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_L2)?"+L2":"-l2",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IPV4)?"+IP4":"-ip4",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IPV6)?"+IP6":"-ip6",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_MPLS)?"+MPLS":"-mpls",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PRESEL)?"-ips":"+IPS",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_CASCADE)?"+CAS":"-cas",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_SMALL)?"+SMALL":"-small",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_LOOSE)?"+LOOSE":"-loose",
             (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_SPARSE_PRIO)?"+SPARSE":"-sparse"));
#endif /* _BCM_PETRA_FIELD_DUMP_SYM_FLAGS */
    if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_USE) {
        FIELD_ACCESS.stageD.modeBits.entryType.get(unit, groupData->stage, groupData->grpMode, &entryType);
        switch (entryType) {
        case _bcmDppFieldEntryTypeDirExt:
            entryBias = _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);
            break;
        case _bcmDppFieldEntryTypeExternalTcam:
            entryBias = _BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam);
            break;
        case _bcmDppFieldEntryTypeInternalTcam:
        default:
            /* default is TCAM */
            entryBias = _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam);
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Stage       = %8d (types %08X)\n"),
                 prefix,
                 groupData->stage,
                 groupData->groupTypes));
        if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_AUTO_SZ) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  Mode        = Auto (%s)\n"),
                     prefix,
                     _bcm_dpp_field_group_mode_name[groupData->grpMode]));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%s  Mode        = %s\n"),
                     prefix,
                     _bcm_dpp_field_group_mode_name[groupData->grpMode]));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Priority    = %8d\n"),
                 prefix,
                 groupData->priority));
        if (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_IN_HW) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  HW group ID = %08X\n"),
                     prefix,
                     group));
        }
        FIELD_ACCESS.groupLimit.get(unit, &groupLimit);
        if (groupLimit > groupData->cascadePair) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  CascadePair = "
                     _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                                "\n"),
                     prefix,
                     groupData->cascadePair));
        }
        LOG_CLI((BSL_META_U(unit,
                            "%s  Prev/Next   = "
                 _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                            "\n"),
                 prefix,
                 groupData->groupPrev,
                 groupData->groupNext));
        LOG_CLI((BSL_META_U(unit,
                            "%s  Entry count = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 groupData->entryCount));
        LOG_CLI((BSL_META_U(unit,
                            "%s  First/Last  = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 groupData->entryHead + entryBias,
                 groupData->entryTail + entryBias));
        BCMDNX_ALLOC(newPrefix, sal_strlen(prefix) + 6,
                              "field group dump prefix");
        if (newPrefix) {
            sal_snprintf(newPrefix, sal_strlen(prefix) + 5, "%s    ", prefix);
        } else {
            result = BCM_E_MEMORY;
        }
        if (BCM_E_NONE == result) {
            if (SOC_PPC_NOF_FP_PREDEFINED_ACL_KEYS != groupData->predefKey) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s  PredefKey   = %s\n"),
                         prefix,
                         SOC_PPC_FP_PREDEFINED_ACL_KEY_to_string(groupData->predefKey)));
            } else {
                LOG_CLI((BSL_META_U(unit,
                                    "%s  PredefKey   = (none)\n"), prefix));
            }
            LOG_CLI((BSL_META_U(unit,
                                "%s  Group QSET:\n"), prefix));
            _bcm_dpp_field_qset_dump(groupData->qset, newPrefix);
            LOG_CLI((BSL_META_U(unit,
                                "%s  Group ASET:\n"), prefix));
            _bcm_dpp_field_aset_dump(&(groupData->aset), newPrefix);
            LOG_CLI((BSL_META_U(unit,
                                "%s  Group preselectors (%s):\n"),
                     prefix,
                     (groupData->groupFlags & _BCM_DPP_FIELD_GROUP_PRESEL)?"configured":"implied"));
            LOG_CLI((BSL_META_U(unit,
                                "%s    "), prefix));
            for (entry = 0, count = 0;
                 entry < preselLimit;
                 entry++) {
                if (BCM_FIELD_PRESEL_TEST(groupData->preselSet, entry)) {
                    if (count) {
                        LOG_CLI((BSL_META_U(unit,
                                            ", %d"), entry));
                    } else {
                        LOG_CLI((BSL_META_U(unit,
                                            "%d"), entry));
                    }
                    count++;
                }
            } /* for (all possible preselector IDs) */
            if (!count) {
                LOG_CLI((BSL_META_U(unit,
                                    "(none)")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n%s  Group preselectors (%s):\n"),
                     prefix,
                     "hardware"));
            LOG_CLI((BSL_META_U(unit,
                                "%s    "), prefix));
            for (entry = 0, count = 0;
                 entry < preselLimit;
                 entry++) {
                if (BCM_FIELD_PRESEL_TEST(groupData->preselHw, entry)) {
                    if (count) {
                        LOG_CLI((BSL_META_U(unit,
                                            ", %d"), entry));
                    } else {
                        LOG_CLI((BSL_META_U(unit,
                                            "%d"), entry));
                    }
                    count++;
                }
            } /* for (all possible preselector IDs) */
            if (!count) {
                LOG_CLI((BSL_META_U(unit,
                                    "(none)")));
            }
            LOG_CLI((BSL_META_U(unit,
                                "\n")));
        } /* if (BCM_E_NONE == result) */
        if ((BCM_E_NONE == result) && entries) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  Entries:\n"), prefix));

            switch (entryType) {
                case _bcmDppFieldEntryTypeDirExt:
                    FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);
                    if (groupData->entryHead < entryDeLimit) {
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, groupData->entryHead, &entryDe_entryNext));
                    }
                    for (entry = groupData->entryHead, count = 0;
                         (BCM_E_NONE == result) && (entry < entryDeLimit);
                         entry = entryDe_entryNext) {
                        result = _bcm_dpp_field_dir_ext_entry_dump(unitData,
                                                                   entry,
                                                                   newPrefix);
                        count++;
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext));
                    }
                    break;
                default:
                    /* default is TCAM */
                    for (entry = groupData->entryHead, count = 0, _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType);
                           (BCM_E_NONE == result) && (entry < tcam_entry_limit) ;
                         /*
                          * Was:
                          *   entry = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry)
                          */
                         _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entry) ) {
                        result = _bcm_dpp_field_tcam_entry_dump(unitData,
                                                                _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
                                                                entry,
                                                                newPrefix);
                        count++;
                    }
            }
            if ((BCM_E_NONE == result) && (0 == count)) {
                LOG_CLI((BSL_META_U(unit,
                                    "%s(no entries in this group)\n"), newPrefix));
            }
        } /* if ((BCM_E_NONE == result) && entries) */
        if (BCM_E_NONE != result) {
            LOG_CLI((BSL_META_U(unit,
                                "%s  (error %d (%s) displaying group)\n"),
                     prefix,
                     result,
                     _SHR_ERRMSG(result)));
        }
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s  (group is not in use)\n"), prefix));
        result = BCM_E_NOT_FOUND;
    }

    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCM_FREE(newPrefix);
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_stage_dump
 *   Purpose
 *      Dump the state of a specific field stage
 *   Parameters
 *      (in) unitData = unit information
 *      (in) stage = stage to be dumped
 *      (in) prefix = string to prefix to each line
 *      (in) groups = TRUE to include groups, FALSE to exclude groups
 *      (in) entries = TRUE to include entries, FALSE to exclude entries
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if sccessful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      If groups is FALSE, the value of entries does not matter: entries will
 *      not be displayed if groups are not displayed.
 */
STATIC int
_bcm_dpp_field_stage_dump(bcm_dpp_field_info_OLD_t *unitData,
                          int stage,
                          const char *prefix,
                          int groups,
                          int entries)
{
    _bcm_dpp_field_grp_idx_t group;
    unsigned int count;
    char *newPrefix = NULL;
    bcm_field_qset_t stgQset;
    bcm_field_aset_t stgAset;
    uint8            is_allocated;
    _bcm_dpp_field_device_group_mode_bits_t modeBits;
    _bcm_dpp_field_stage_idx_t groupSh1;
    _bcm_dpp_field_stage_idx_t entryTcSh1;
    _bcm_dpp_field_stage_idx_t entryDeSh1;
    _bcm_dpp_field_ent_idx_t entryCount;
    _bcm_dpp_field_ent_idx_t entryElems;
    _bcm_dpp_field_ent_idx_t entryDeCount;
    _bcm_dpp_field_grp_idx_t groupCount;
    _bcm_dpp_field_grp_idx_t groupHead;
    _bcm_dpp_field_grp_idx_t groupTail;
    _bcm_dpp_field_grp_idx_t groupLimit;
    _bcm_dpp_field_grp_idx_t groupNext;
    uint32 hwGroupLimit;
    uint32 hwEntryLimit;
    uint32 hwEntryDeLimit;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 > stage) || (stage > unitData->devInfo->stages)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support stage %d"),
                          unit,
                          stage));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%sStage "
             _BCM_DPP_FIELD_STAGE_IDX_FORMAT
                        " (%s) (%u)\n"),
             prefix,
             stage,
             _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).stageName,
             _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).hwStageId));
    BCMDNX_ALLOC(newPrefix,
              sal_strlen(prefix) + 6,
              "field stage dump prefix");
    if (newPrefix) {
        sal_snprintf(newPrefix, sal_strlen(prefix) + 5, "%s    ", prefix);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate %u"
                                           " bytes for prefix buffer"),
                          unit,
                          (uint32)sal_strlen(prefix) + 6));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s  Supported %s qualifiers:\n"),
             prefix,
             _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).stageName));
    FIELD_ACCESS.stageD.stgQset.get(unit, stage, &stgQset);
    _bcm_dpp_field_qset_dump(stgQset, newPrefix);

    LOG_CLI((BSL_META_U(unit,
                        "%s  Supported %s actions:\n"),
             prefix,
             _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).stageName));
    FIELD_ACCESS.stageD.stgAset.get(unit, stage, &stgAset);
    _bcm_dpp_field_aset_dump(&stgAset, newPrefix);
    LOG_CLI((BSL_META_U(unit,
                        "%s  Supported %s group modes and bit widths:\n"),
             prefix,
             _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).stageName));
    for (group = 0, count = 0; group < bcmFieldGroupModeCount; group++) {
        FIELD_ACCESS.stageD.modeBits.is_allocated(unit, stage, group, &is_allocated);
        if (is_allocated && (bcmFieldGroupModeAuto != group)) {
            FIELD_ACCESS.stageD.modeBits.get(unit, stage, group, &modeBits);
            LOG_CLI((BSL_META_U(unit,
                                "%s%-18s (%d): %3ub in, %3ub qual, %u elem/ent, %s\n"),
                     newPrefix,
                     _bcm_dpp_field_group_mode_name[group],
                     group,
                     modeBits.length,
                     modeBits.qualLength,
                     modeBits.entryCount,
                     _bcm_dpp_field_entry_type_names[modeBits.entryType]));
            count++;
        }
    }
    if (!count) {
        LOG_CLI((BSL_META_U(unit,
                            "%s(no supported group modes)\n"), newPrefix));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s  Qual in grp = %8d\n"),
             prefix,
             _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).entryMaxQuals));
    LOG_CLI((BSL_META_U(unit,
                        "%s  Act in grp  = %8d\n"),
             prefix,
             _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).entryMaxActs));
    if (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).maxEntriesDe) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Qual DE lim = %8d\n"),
                 prefix,
                 _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).entryDeMaxQuals));
    }
    FIELD_ACCESS.stageD.groupSh1.get(unit, stage, &groupSh1);
    FIELD_ACCESS.stageD.groupCount.get(unit, groupSh1, &groupCount);
    FIELD_ACCESS.stageD.hwGroupLimit.get(unit, groupSh1, &hwGroupLimit);
    LOG_CLI((BSL_META_U(unit,
                        "%s  Groups      = "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             groupCount,
             hwGroupLimit));
    FIELD_ACCESS.stageD.groupHead.get(unit, stage, &groupHead);
    FIELD_ACCESS.stageD.groupTail.get(unit, stage, &groupTail);

    LOG_CLI((BSL_META_U(unit,
                        "%s    first/last= "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             groupHead,
             groupTail));
    FIELD_ACCESS.stageD.entryTcSh1.get(unit, stage, &entryTcSh1);
    FIELD_ACCESS.stageD.entryCount.get(unit, entryTcSh1, &entryCount);
    FIELD_ACCESS.stageD.hwEntryLimit.get(unit, entryTcSh1, &hwEntryLimit);

    LOG_CLI((BSL_META_U(unit,
                        "%s  Entries     = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             entryCount,
             hwEntryLimit));
    FIELD_ACCESS.stageD.hwEntryLimit.get(unit, entryTcSh1, &hwEntryLimit);
    FIELD_ACCESS.stageD.entryElems.get(unit, entryTcSh1, &entryElems);
    LOG_CLI((BSL_META_U(unit,
                        "%s    available = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             hwEntryLimit -
             entryElems));
        FIELD_ACCESS.stageD.entryDeSh1.get(unit, stage, &entryDeSh1);
        FIELD_ACCESS.stageD.hwEntryDeLimit.get(unit, entryDeSh1, &hwEntryDeLimit);
    if (hwEntryDeLimit) {
        FIELD_ACCESS.stageD.entryDeCount.get(unit, entryDeSh1, &entryDeCount);
        LOG_CLI((BSL_META_U(unit,
                            "%s  DirExt Ents = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 entryDeCount,
                 hwEntryDeLimit));
    }
    if (groups) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Stage %s group information:\n"), prefix, _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).stageName));
        FIELD_ACCESS.stageD.groupHead.get(unit, stage, &groupHead);
        FIELD_ACCESS.groupLimit.get(unit, &groupLimit);

        
		for (group = groupHead, count = 0;
             group < groupLimit;
             
             group = groupNext) {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_group_dump(unitData,
                                                              group,
                                                              newPrefix,
                                                              entries));
            count++;
			BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupNext.get(unit, group, &groupNext));
        }
        if (!count) {
            LOG_CLI((BSL_META_U(unit,
                                "%s(no groups in this stage)\n"), newPrefix));
        }
    } /* if (groups) */

exit:
    BCM_FREE(newPrefix);
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
int
_bcm_dpp_field_unit_dump(bcm_dpp_field_info_OLD_t *unitData,
                         const char *prefix,
                         int stages,
                         int groups,
                         int entries)
{
    _bcm_dpp_field_stage_idx_t stage;
    unsigned int count;
    int result = BCM_E_NONE;
    char *newPrefix = NULL;
#if _BCM_DPP_FIELD_DUMP_INCLUDE_PPD
    uint32 soc_sandResult;
    uint32 soc_sandIndex;
    uint32 soc_sandOffset;
    uint8 okay;
    SOC_PPC_PMF_PFG_INFO *pfgInfo = NULL;
    SOC_PPC_FP_DATABASE_INFO *dbInfo = NULL;
    SOC_PPC_FP_ENTRY_INFO *entInfoTc = NULL;
    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *entInfoDe = NULL;
    SOC_PPC_FP_CONTROL_INDEX control_ndx;
    SOC_PPC_FP_CONTROL_INFO control_info;
    unsigned int i;
    unsigned int j;
    unsigned int k;
    unsigned int l;
    const char *str;
#endif /* _BCM_PETRA_FIELD_DUMP_INCLUDE_PPD */
    _bcm_dpp_field_ent_idx_t entryIntTcCount;
    _bcm_dpp_field_ent_idx_t entryExtTcCount;
    _bcm_dpp_field_ent_idx_t entryIntTcFree;
    _bcm_dpp_field_ent_idx_t entryExtTcFree;
    _bcm_dpp_field_ent_idx_t entryTcLimit;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    _bcm_dpp_field_ent_idx_t entryExtTcLimit;
    _bcm_dpp_field_grp_idx_t groupLimit;
    _bcm_dpp_field_dq_idx_t dqLimit;
    _bcm_dpp_field_presel_idx_t preselLimit;
    _bcm_dpp_field_grp_idx_t    groupCascaded ;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;


    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    FIELD_ACCESS.dqLimit.get(unit, &dqLimit);

    /* dump the BCM layer state */
/*    LOG_CLI((BSL_META_U(unit,
                        "%sBCM unit %d field support: %08X (%u bytes)\n"),
             prefix,
             unit,
             PTR_TO_INT(unitData),
             unitData->totalSize));*/
    LOG_CLI((BSL_META_U(unit,
                        "%s  Lock        = %08X\n"),
             prefix,
             PTR_TO_INT(sw_state_sync_db[unitData->unit].dpp.unitLock)));

    FIELD_ACCESS.groupLimit.get(unit, &groupLimit);
    FIELD_ACCESS.entryTcLimit.get(unit, &entryTcLimit);
    {
        _bcm_dpp_field_grp_idx_t groupCount ;
        _bcm_dpp_field_grp_idx_t groupFree ;

        FIELD_ACCESS.groupCount.get(unit, &groupCount) ;
        LOG_CLI((BSL_META_U(unit,
                    "%s  Groups      = "
            _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                    " / "
                    _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                    "\n"),
            prefix,
            groupCount,
            groupLimit));
        FIELD_ACCESS.groupFree.get(unit, &groupFree) ;
        LOG_CLI((BSL_META_U(unit,
                        "%s    next free = "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             groupFree));
    }
    FIELD_ACCESS.entryIntTcCount.get(unit, &entryIntTcCount);
    LOG_CLI((BSL_META_U(unit,
                        "%s  Entries     = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             entryIntTcCount,
             entryTcLimit));
    FIELD_ACCESS.entryIntTcFree.get(unit, &entryIntTcFree);
    LOG_CLI((BSL_META_U(unit,
                        "%s    next free = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             entryIntTcFree));
    FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);

    if (entryDeLimit) {
        _bcm_dpp_field_ent_idx_t entryDeCount ;

        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDeCount.get(unit, &entryDeCount)) ;
        LOG_CLI((BSL_META_U(unit,
                            "%s  DirExt Ents = "
                 _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            " / "
                            _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                            "\n"),
                 prefix,
                 entryDeCount,
                 entryDeLimit));
    }
    FIELD_ACCESS.entryExtTcCount.get(unit, &entryExtTcCount);
    FIELD_ACCESS.entryExtTcLimit.get(unit, &entryExtTcLimit);
    LOG_CLI((BSL_META_U(unit,
                        "%s  ExtTcam Ents = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        " / "
                        _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             entryExtTcCount,
             entryExtTcLimit));
    FIELD_ACCESS.entryExtTcFree.get(unit, &entryExtTcFree);
    LOG_CLI((BSL_META_U(unit,
                        "%s    next free = "
             _BCM_DPP_FIELD_ENTRY_IDX_FORMAT
                        "\n"),
             prefix,
             entryExtTcFree));
    {
        uint8 cascadedKeyLen;

        FIELD_ACCESS.cascadedKeyLen.get(unit, &cascadedKeyLen);
        LOG_CLI((BSL_META_U(unit,
            "%s  Cascade Len = %8d bits\n"),
            prefix,
            cascadedKeyLen));
    }
    {
        FIELD_ACCESS.groupCascaded.get(unit, &groupCascaded) ;
        LOG_CLI((BSL_META_U(unit,
                        "%s    casc grps = "
             _BCM_DPP_FIELD_GROUP_IDX_FORMAT
                        "\n"),
             prefix,
             groupCascaded));
    }
    BCMDNX_ALLOC(newPrefix, sal_strlen(prefix) + 6,
                          "field unit dump prefix");
    if (newPrefix) {
        sal_snprintf(newPrefix, sal_strlen(prefix) + 5, "%s    ", prefix);
    } else {
        result = BCM_E_MEMORY;
    }
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Preselectors:\n"), prefix));
        result = _bcm_dpp_field_presel_dump(unitData, newPrefix);
    }
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Ranges:\n"), prefix));
        result = _bcm_dpp_field_range_dump(unitData, newPrefix);
    }
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Data Field Qualifiers:\n"), prefix));
        result = _bcm_petra_field_data_qualifier_dump(unitData, newPrefix);
    }
    if ((BCM_E_NONE == result) && stages) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  Stages:\n"), prefix));
        for (stage = 0, count = 0;
             (BCM_E_NONE == result) && (stage < unitData->devInfo->stages);
             stage++) {
            result = _bcm_dpp_field_stage_dump(unitData,
                                               stage,
                                               newPrefix,
                                               groups,
                                               entries);
            if (BCM_E_NONE == result) {
                count++;
            }
        }
        if ((BCM_E_NONE == result) && (0 == count)) {
            LOG_CLI((BSL_META_U(unit,
                                "%s(no groups in this unit)\n"), newPrefix));
        }
    } /* if ((BCM_E_NONE == result) && groups) */
#if _BCM_DPP_FIELD_DUMP_INCLUDE_PPD
    /* dump the PPD layer state */
    LOG_CLI((BSL_META_U(unit,
                        "%s  PPD state:\n"), prefix));
    LOG_CLI((BSL_META_U(unit,
                        "%s    PFG information:\n"), prefix));
    BCMDNX_ALLOC(pfgInfo, sizeof(SOC_PPC_PMF_PFG_INFO), "_bcm_dpp_field_unit_dump.pfgInfo");
    if (pfgInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }
    for (soc_sandIndex = 0;
         soc_sandIndex < preselLimit;
         soc_sandIndex++) {
        SOC_PPC_PMF_PFG_INFO_clear(pfgInfo);
        soc_sandResult = soc_ppd_fp_packet_format_group_get(unitData->unitHandle,
                                                            soc_sandIndex,
                                                            pfgInfo);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
            if (pfgInfo->is_array_qualifier) {
                /* Arad uses this to indicate PFG is present */
                LOG_CLI((BSL_META_U(unit,
                                    "%s      PFG %2d: stage=%s (%d)\n"),
                         prefix,
                         soc_sandIndex,
                         SOC_PPC_FP_DATABASE_STAGE_to_string(pfgInfo->stage),
                         pfgInfo->stage));
                for (i = 0, j = 0;
                     i < SOC_PPC_FP_NOF_QUALS_PER_PFG_MAX;
                     i++) {
                    if (_BCM_DPP_FIELD_PPD_QUAL_VALID(pfgInfo->qual_vals[i].type)) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%s        %s (%d)\n"
                                 "%s          %08X%08X/%08X%08X\n"),
                                 prefix,
                                 SOC_PPC_FP_QUAL_TYPE_to_string(pfgInfo->qual_vals[i].type),
                                 pfgInfo->qual_vals[i].type,
                                 prefix,
                                 pfgInfo->qual_vals[i].val.arr[1],
                                 pfgInfo->qual_vals[i].val.arr[0],
                                 pfgInfo->qual_vals[i].is_valid.arr[1],
                                 pfgInfo->qual_vals[i].is_valid.arr[0]));
                        j++;
                    } /* if (qualifier type is valid) */
                } /* for (all possible qualifiers) */
                if (0 == j) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s              (none)\n"),
                             prefix));
                }
            } else { /* if (pfgInfo->is_array_qualifier) */
                /* Arad uses this to indicate PFG not present */
                LOG_CLI((BSL_META_U(unit,
                                    "%s      PFG %2d: (not in use)\n"),
                         prefix,
                         soc_sandIndex));
            } /* if (pfgInfo->is_array_qualifier) */
        } else { /* if (BCM_E_NONE == result) */
            LOG_CLI((BSL_META_U(unit,
                                "%s      PFG %d: unable to read: %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     result,
                     _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } /* for (soc_sandIndex = 0; soc_sandIndex < 5; soc_sandIndex++) */
    LOG_CLI((BSL_META_U(unit,
                        "%s    L4OPS (TCP/UDP range) information:\n"), prefix));
    for (soc_sandIndex = 0;
         soc_sandIndex < _BCM_DPP_NOF_L4_PORT_RANGES;
         soc_sandIndex++) {
        SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
        SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_L4OPS_RANGE;
        control_ndx.val_ndx = soc_sandIndex;
        soc_sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                                SOC_CORE_INVALID,
                                                &control_ndx,
                                                &control_info);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
            LOG_CLI((BSL_META_U(unit,
                                "%s      L4OPS %2d: Src = %5d through %5d, Dst"
                     " = %5d through %5d\n"),
                     prefix,
                     soc_sandIndex,
                     control_info.val[0],
                     control_info.val[1],
                     control_info.val[2],
                     control_info.val[3]));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%s      L4OPS %2d: unable to read:"
                     " %08X -> %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     soc_sandResult,
                     result,
                     _SHR_ERRMSG(result)));
        }
    } /* for (all L4OPS ranges) */
    LOG_CLI((BSL_META_U(unit,
                        "%s    Packet length range information:\n"), prefix));
    for (soc_sandIndex = 0;
         soc_sandIndex < _BCM_DPP_NOF_PKT_LEN_RANGES;
         soc_sandIndex++) {
        SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
        SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_PACKET_SIZE_RANGE;
        control_ndx.val_ndx = soc_sandIndex;
        soc_sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                                SOC_CORE_INVALID,  /*Not used*/
                                                &control_ndx,
                                                &control_info);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
            LOG_CLI((BSL_META_U(unit,
                                "%s      Packet Length %1d: Min = %3d, Max = %3d\n"),
                     prefix,
                     soc_sandIndex,
                     control_info.val[0],
                     control_info.val[1]));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%s      Packet Length %1d: unable to read:"
                     " %08X -> %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     soc_sandResult,
                     result,
                     _SHR_ERRMSG(result)));
        }
    } /* for (all packet length ranges) */
    LOG_CLI((BSL_META_U(unit,
                        "%s    User defined fields information:\n"), prefix));
    for (soc_sandIndex = 0;
         soc_sandIndex < dqLimit;
         soc_sandIndex++) {
        SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
        SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
        control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_HDR_USER_DEF;
        control_ndx.val_ndx = soc_sandIndex;
        soc_sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                                SOC_CORE_INVALID,  /*Not used*/
                                                &control_ndx,
                                                &control_info);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
            LOG_CLI((BSL_META_U(unit,
                                "%s      Data field %2d: Hdr = %1d, Ofs = %3d,"
                     " Len = %2d\n"),
                     prefix,
                     soc_sandIndex,
                     control_info.val[0],
                     control_info.val[1],
                     control_info.val[2]));
        } else {
            LOG_CLI((BSL_META_U(unit,
                                "%s      Data field %1d: unable to read: %08X ->"
                     " %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     soc_sandResult,
                     result,
                     _SHR_ERRMSG(result)));
        }
    } /* for (all data fields) */
    SOC_PPC_FP_CONTROL_INDEX_clear(&control_ndx);
    SOC_PPC_FP_CONTROL_INFO_clear(&control_info);
    control_ndx.type = SOC_PPC_FP_CONTROL_TYPE_KEY_CHANGE_SIZE;
    control_ndx.val_ndx = 0;
    soc_sandResult = soc_ppd_fp_control_get(unitData->unitHandle,
                                            SOC_CORE_INVALID,  /*Not used*/
                                            &control_ndx,
                                            &control_info);
    result = handle_sand_result(soc_sandResult);
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s    Cascade length = %d\n"),
                 prefix,
                 control_info.val[0]));
    } else {
        LOG_CLI((BSL_META_U(unit,
                            "%s    Cascade length = (error %08X retrieving)\n"),
                 prefix,
                 soc_sandResult));
    }
    LOG_CLI((BSL_META_U(unit,
                        "%s    Database information:\n"), prefix));
    BCMDNX_ALLOC(dbInfo, sizeof(SOC_PPC_FP_DATABASE_INFO), "_bcm_dpp_field_unit_dump.dbInfo");
    if (dbInfo == NULL) {        
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
    }
    for (soc_sandIndex = 0; soc_sandIndex < 128; soc_sandIndex++) {
        SOC_PPC_FP_DATABASE_INFO_clear(dbInfo);
        soc_sandResult = soc_ppd_fp_database_get(unitData->unitHandle,
                                                 soc_sandIndex,
                                                 dbInfo);
        result = handle_sand_result(soc_sandResult);
        if (BCM_E_NONE == result) {
            if (SOC_PPC_NOF_FP_DATABASE_TYPES > dbInfo->db_type) {
                /* the type is valid, so it's in use[?] */
                LOG_CLI((BSL_META_U(unit,
                                    "%s      DB %3d: type=%s(%d), strength=%d\n"),
                         prefix,
                         soc_sandIndex,
                         SOC_PPC_FP_DATABASE_TYPE_to_string(dbInfo->db_type),
                         dbInfo->db_type,
                         dbInfo->strength));
                LOG_CLI((BSL_META_U(unit,
                                    "%s        PFG set (A)  = %08X%08X\n"),
                         prefix,
                         dbInfo->supported_pfgs_arad[1],
                         dbInfo->supported_pfgs_arad[0]));
                LOG_CLI((BSL_META_U(unit,
                                    "%s        Cascade Pair = %3d\n"),
                         prefix,
                         dbInfo->cascaded_coupled_db_id));
                LOG_CLI((BSL_META_U(unit,
                                    "%s        Qualifiers:\n"), prefix));
                for (i = 0, j = 0, k = 0;
                     i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX;
                     i++) {
                    if (_BCM_DPP_FIELD_PPD_QUAL_VALID(dbInfo->qual_types[i])) {
                        str = SOC_PPC_FP_QUAL_TYPE_to_string(dbInfo->qual_types[i]);
                        if (0 == k) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s          %s"), prefix, str));
                            k = sal_strlen(prefix) + sal_strlen(str) + 10;
                        } else if (k + 3 + sal_strlen(str) <
                                   _BCM_DPP_FIELD_PAGE_WIDTH) {
                            LOG_CLI((BSL_META_U(unit,
                                                ", %s"), str));
                            k += sal_strlen(str) + 2;
                        } else {
                            LOG_CLI((BSL_META_U(unit,
                                                ",\n%s          %s"), prefix, str));
                            k = sal_strlen(prefix) + sal_strlen(str) + 10;
                        }
                        j++;
                    }
                }
                if (0 == j) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s          (none)\n"), prefix));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                }
                LOG_CLI((BSL_META_U(unit,
                                    "%s        Actions:\n"), prefix));
                for (i = 0, j = 0, k = 0;
                     i < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX;
                     i++) {
                    if (_BCM_DPP_FIELD_PPD_ACTION_VALID(dbInfo->action_types[i])) {
                        str = SOC_PPC_FP_ACTION_TYPE_to_string(dbInfo->action_types[i]);
                        if (0 == k) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s          %s"), prefix, str));
                            k = sal_strlen(prefix) + sal_strlen(str) + 10;
                        } else if (k + 3 + sal_strlen(str) <
                                   _BCM_DPP_FIELD_PAGE_WIDTH) {
                            LOG_CLI((BSL_META_U(unit,
                                                ", %s"), str));
                            k += sal_strlen(str) + 2;
                        } else {
                            LOG_CLI((BSL_META_U(unit,
                                                ",\n%s          %s"), prefix, str));
                            k = sal_strlen(prefix) + sal_strlen(str) + 10;
                        }
                        
                        j++;
                    }
                }
                if (0 == j) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s          (none)\n"), prefix));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                                        "\n")));
                }
                if ((SOC_PPC_FP_DB_TYPE_TCAM == dbInfo->db_type) ||
                    (SOC_PPC_FP_DB_TYPE_DIRECT_TABLE == dbInfo->db_type) ||
                    (SOC_PPC_FP_DB_TYPE_FLP == dbInfo->db_type) ||
                    (SOC_PPC_FP_DB_TYPE_EGRESS == dbInfo->db_type)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s        Entries:\n"), prefix));
                    BCM_FREE(entInfoTc);
                    BCMDNX_ALLOC(entInfoTc, sizeof(SOC_PPC_FP_ENTRY_INFO), "_bcm_dpp_field_unit_dump.entInfoTc");
                    if (entInfoTc == NULL) {        
                        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
                    }
                    for (soc_sandOffset = 0, l = 0;
                         soc_sandOffset < entryTcLimit;
                         soc_sandOffset++) {
                        LOG_CLI((BSL_META_U(unit,
                                            "entry %d\r"), soc_sandOffset));
                        SOC_PPC_FP_ENTRY_INFO_clear(entInfoTc);
                        soc_sandResult = soc_ppd_fp_entry_get(unitData->unitHandle,
                                                              soc_sandIndex,
                                                              soc_sandOffset,
                                                              &okay,
                                                              entInfoTc);
                        result = handle_sand_result(soc_sandResult);
                        if ((BCM_E_NONE == result) && okay) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s          Entry %5u: priority"
                                     " %u\n"),
                                     prefix,
                                     soc_sandOffset,
                                     entInfoTc->priority));
                            LOG_CLI((BSL_META_U(unit,
                                                "%s            Qualifiers:\n"),
                                     prefix));
                            for (i = 0, j = 0;
                                 i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX;
                                 i++) {
                                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entInfoTc->qual_vals[i].type)) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%s              %s (%d)\n"
                                             "%s              "
                                                        "  %08X%08X/%08X%08X\n"),
                                             prefix,
                                             SOC_PPC_FP_QUAL_TYPE_to_string(entInfoTc->qual_vals[i].type),
                                             entInfoTc->qual_vals[i].type,
                                             prefix,
                                             entInfoTc->qual_vals[i].val.arr[1],
                                             entInfoTc->qual_vals[i].val.arr[0],
                                             entInfoTc->qual_vals[i].is_valid.arr[1],
                                             entInfoTc->qual_vals[i].is_valid.arr[0]));
                                    j++;
                                }
                            }
                            if (0 == j) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s              (none)\n"),
                                         prefix));
                            }
                            LOG_CLI((BSL_META_U(unit,
                                                "%s            Actions:\n"), prefix));
                            for (i = 0, j = 0;
                                 i < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX;
                                 i++) {
                                if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entInfoTc->actions[i].type)) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%s              %s (%d)\n"
                                             "%s              "
                                                        "  %08X\n"),
                                             prefix,
                                             SOC_PPC_FP_ACTION_TYPE_to_string(entInfoTc->actions[i].type),
                                             entInfoTc->actions[i].type,
                                             prefix,
                                             entInfoTc->actions[i].val));
                                    j++;
                                }
                            }
                            if (0 == j) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s              (none)\n"),
                                         prefix));
                            }
                            l++;
                        } /* if ((BCM_E_NONE == result) && okay) */
                    } /* for (soc_sandOffset = 0; soc_sandOffset < 4K; soc_sandOffset++) */
                    if (0 == l) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%s            (none)\n"),
                                 prefix));
                    }
                } else if (SOC_PPC_FP_DB_TYPE_DIRECT_EXTRACTION == dbInfo->db_type) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s        Entries:\n"), prefix));
                    BCM_FREE(entInfoDe);
                    BCMDNX_ALLOC(entInfoDe, sizeof(SOC_PPC_FP_DIR_EXTR_ENTRY_INFO), "_bcm_dpp_field_unit_dump.entInfoDe");
                    if (entInfoDe == NULL) {        
                        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
                    }

                    for (soc_sandOffset = 0, l = 0;
                         soc_sandOffset < entryDeLimit;
                         soc_sandOffset++) {
                        LOG_CLI((BSL_META_U(unit,
                                            "entry %d\r"), soc_sandOffset));
                        SOC_PPC_FP_DIR_EXTR_ENTRY_INFO_clear(entInfoDe);
                        soc_sandResult = soc_ppd_fp_direct_extraction_entry_get(unitData->unitHandle,
                                                                                soc_sandIndex,
                                                                                soc_sandOffset,
                                                                                &okay,
                                                                                entInfoDe);
                        result = handle_sand_result(soc_sandResult);
                        if ((BCM_E_NONE == result) && okay) {
                            LOG_CLI((BSL_META_U(unit,
                                                "%s          Entry %5u: priority"
                                     " %u\n"),
                                     prefix,
                                     soc_sandOffset,
                                     entInfoDe->priority));
                            LOG_CLI((BSL_META_U(unit,
                                                "%s            Qualifiers:\n"),
                                     prefix));
                            for (i = 0, j = 0;
                                 i < SOC_PPC_FP_NOF_QUALS_PER_DB_MAX;
                                 i++) {
                                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(entInfoDe->qual_vals[i].type)) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%s              %s (%d)\n"
                                             "%s              "
                                                        "  %08X%08X/%08X%08X\n"),
                                             prefix,
                                             SOC_PPC_FP_QUAL_TYPE_to_string(entInfoDe->qual_vals[i].type),
                                             entInfoDe->qual_vals[i].type,
                                             prefix,
                                             entInfoDe->qual_vals[i].val.arr[1],
                                             entInfoDe->qual_vals[i].val.arr[0],
                                             entInfoDe->qual_vals[i].is_valid.arr[1],
                                             entInfoDe->qual_vals[i].is_valid.arr[0]));
                                    j++;
                                }
                            } /* for (all qualifiers this entry) */
                            if (0 == j) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s              (none)\n"),
                                         prefix));
                            }
                            LOG_CLI((BSL_META_U(unit,
                                                "%s            Actions:\n"), prefix));
                            for (i = 0, j = 0;
                                 i < SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX;
                                 i++) {
                                if (_BCM_DPP_FIELD_PPD_ACTION_VALID(entInfoDe->actions[i].type)) {
                                    LOG_CLI((BSL_META_U(unit,
                                                        "%s              %s (%d):"
                                             " %2d location%s\n"
                                                        "%s              "
                                                        "  bias=%08X\n"),
                                             prefix,
                                             SOC_PPC_FP_ACTION_TYPE_to_string(entInfoDe->actions[i].type),
                                             entInfoDe->actions[i].type,
                                             entInfoDe->actions[i].nof_fields,
                                             (1 == entInfoDe->actions[i].nof_fields)?"":"s",
                                             prefix,
                                             entInfoDe->actions[i].base_val));
                                    j++;
                                    for (k = 0;
                                         k < entInfoDe->actions[i].nof_fields;
                                         k++) {
                                        if ((entInfoDe->actions[i].fld_ext[k].cst_val) ||
                                            (BCM_FIELD_ENTRY_INVALID == entInfoDe->actions[i].fld_ext[k].type)) {
                                            LOG_CLI((BSL_META_U(unit,
                                                                "%s                "
                                                     "const %08X, %d"
                                                                " LSbs\n"),
                                                     prefix,
                                                     entInfoDe->actions[i].fld_ext[k].cst_val,
                                                     entInfoDe->actions[i].fld_ext[k].nof_bits));
                                        } else {
                                            LOG_CLI((BSL_META_U(unit,
                                                                "%s                "
                                                     "%s (%d), bits"
                                                                " %d..%d\n"),
                                                     prefix,
                                                     SOC_PPC_FP_QUAL_TYPE_to_string(entInfoDe->actions[i].fld_ext[k].type),
                                                     entInfoDe->actions[i].fld_ext[k].type,
                                                     entInfoDe->actions[i].fld_ext[k].nof_bits + entInfoDe->actions[i].fld_ext[k].fld_lsb - 1,
                                                     entInfoDe->actions[i].fld_ext[k].fld_lsb));
                                        }
                                    } /* for (all extractions this action) */
                                } /* if (this action is valid) */
                            } /* for (all actions this entry) */
                            if (0 == j) {
                                LOG_CLI((BSL_META_U(unit,
                                                    "%s              (none)\n"),
                                         prefix));
                            }
                            l++;
                        } /* if ((BCM_E_NONE == result) && okay) */
                    } /* for (soc_sandOffset = 0; soc_sandOffset < 4K; soc_sandOffset++) */
                    if (0 == l) {
                        LOG_CLI((BSL_META_U(unit,
                                            "%s            (none)\n"),
                                 prefix));
                    }
                }
                
            } else { /* if (SOC_PPC_NOF_FP_DATABASE_TYPES > dbInfo->db_type) */
                if (SOC_PPC_NOF_FP_DATABASE_TYPES < dbInfo->db_type) {
                    LOG_CLI((BSL_META_U(unit,
                                        "%s      DB %3d: unexpected type %d\n"),
                             prefix,
                             soc_sandIndex,
                             dbInfo->db_type));
                }
            } /* if (SOC_PPC_NOF_FP_DATABASE_TYPES > dbInfo->db_type) */
        } else { /* if (BCM_E_NONE == result) */
            LOG_CLI((BSL_META_U(unit,
                                "%s      DB %3d: unable to read %d (%s)\n"),
                     prefix,
                     soc_sandIndex,
                     result,
                     _SHR_ERRMSG(result)));
        } /* if (BCM_E_NONE == result) */
    } /* for (soc_sandIndex = 0; soc_sandIndex < 128; soc_sandIndex++) */
#endif /* _BCM_PETRA_FIELD_DUMP_INCLUDE_PPD */
#ifdef BCM_WARM_BOOT_SUPPORT
#if (0)
/* { */
    /*
     * All 'save' and 'sync' of old sw state are dropped.
     */
    if (BCM_E_NONE == result) {
        _bcm_dpp_wb_field_state_dump(unitData, prefix);
    }
/* } */
#endif
#endif /* def BCM_WARM_BOOT_SUPPORT */
#if _BCM_DPP_UNIT_DUMP_INCLUDE_RES_DUMP
    
    if (BCM_E_NONE == result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s                "
                            "Allocation info is available using command \"diag alloc\"."
                            "\n"),
                 prefix
                 ));
    }
#endif /* _BCM_DPP_UNIT_DUMP_INCLUDE_RES_DUMP */

    if (BCM_E_NONE != result) {
        LOG_CLI((BSL_META_U(unit,
                            "%s  (error %d (%s) displaying unit)\n"),
                 prefix,
                 result,
                 _SHR_ERRMSG(result)));
    }

    BCMDNX_IF_ERR_EXIT(result);

exit:
    BCM_FREE(newPrefix);
    BCM_FREE(pfgInfo);
    BCM_FREE(entInfoTc);
    BCM_FREE(entInfoDe);
    BCM_FREE(dbInfo);
    BCMDNX_FUNC_RETURN;
}
#endif /* def BROADCOM_DEBUG */

signed int
_bcm_dpp_compare_entry_priority(int pri1,
                                int pri2)
{
    int unit;

    unit = BSL_UNIT_UNKNOWN;

    /* validate parameters */
    /* note that we don't allow *any* negative priorities on this platform */
    if (0 > pri1) {
        /* pri1 is not valid */
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "pri1 (%d) invalid\n"), pri1));
        if (0 > pri2) {
            /* pri2 is not valid either, so equal   */
            LOG_ERROR(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "pri2 (%d) invalid\n"), pri1));
            return 0;
        }
        /* pri2 is valid, so it's greater */
        return -1;
    }
    if (0 > pri2) {
        /* pri2 is not valid, so pri1 is greater (since it's valid) */
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "pri2 (%d) invalid\n"), pri1));
        return 1;
    }
    /*
     *  All other cases degenerated into this, with the change from the special
     *  values being negative to them being just really wide-flung positive
     *  values.
     */
    return pri1 - pri2;
}

int
_bcm_dpp_field_qset_subset(bcm_field_qset_t qset1,
                           bcm_field_qset_t qset2)
{
    unsigned int index;                 /* working index for loops */
    int result;                         /* result for caller */
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
 
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(*,*)\n")));

    /* be optimistic */
    result = BCM_E_NONE;

    /* check all qualifiers */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "scanning qualifiers\n")));
    for (index = 0;
         (index < bcmFieldQualifyCount) && (BCM_E_NONE == result);
         index++) {
        if (BCM_FIELD_QSET_TEST(qset2, index)) {
            if (!BCM_FIELD_QSET_TEST(qset1, index)) {
                result = BCM_E_FAIL;
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "qualifier %d (%s) in qset 2"
                                      " but not in qset 1\n"),
                           index,
                           _bcm_dpp_field_qual_name[index]));
            }
        } /* if (BCM_FIELD_QSET_TEST(qset2, index)) */
    } /* for (all possible fixed qualifiers as long as no error) */
    for (index = 0;
         (index < BCM_FIELD_USER_NUM_UDFS) && (BCM_E_NONE == result);
         index++) {
        if (SHR_BITGET(qset2.udf_map, index)) {
            if (!SHR_BITGET(qset1.udf_map, index)) {
                result = BCM_E_FAIL;
                LOG_DEBUG(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "data field qualifier %d in qset 2"
                                      " but not in qset 1\n"),
                           index));
            }
        } /* if (SHR_BITGET(qset2.udf_map, index)) */
    } /* for (all possible programmable qualifiers as long as no error) */

    /* then tell the caller the result */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(*,*) = %d (%s)\n"),
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_NOT_E_FAIL_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_qset_union(const bcm_field_qset_t *qset0,
                          const bcm_field_qset_t *qset1,
                          bcm_field_qset_t *qset2)
{
    /* We can't use directly qset2, because the result will be wrong in case qset2 is the same as qset0 */
    bcm_field_qset_t result;
    bcm_field_qset_t_init(&result);
    SHR_BITOR_RANGE(qset0->w,
                    qset1->w,
                    0,
                    bcmFieldQualifyCount,
                    result.w);
    SHR_BITOR_RANGE(qset0->udf_map,
                    qset1->udf_map,
                    0,
                    BCM_FIELD_USER_NUM_UDFS,
                    result.udf_map);
    *qset2 = result;
}

void
_bcm_dpp_field_qset_subset_count(const bcm_field_qset_t *qset0,
                                 const bcm_field_qset_t *qset1,
                                 unsigned int *qual0,
                                 unsigned int *qual1,
                                 unsigned int *qualBoth)
{
    unsigned int index;

    *qual0 = 0;
    *qual1 = 0;
    *qualBoth = 0;
    for (index = 0; index < bcmFieldQualifyCount; index++) {
        if (BCM_FIELD_QSET_TEST(*qset0, index)) {
            (*qual0)++;
            if (BCM_FIELD_QSET_TEST(*qset1, index)) {
                (*qualBoth)++;
            }
        }
        if (BCM_FIELD_QSET_TEST(*qset1, index)) {
            (*qual1)++;
        }
    }
    for (index = 0; index < BCM_FIELD_USER_NUM_UDFS; index++) {
        if (SHR_BITGET(qset0->udf_map, index)) {
            (*qual0)++;
            if (SHR_BITGET(qset1->udf_map, index)) {
                (*qualBoth)++;
            }
        }
        if (SHR_BITGET(qset1->udf_map, index)) {
            (*qual1)++;
        }
    }
}

int
_bcm_dpp_field_aset_subset(bcm_field_aset_t *aset1,
                           bcm_field_aset_t *aset2)
{
    unsigned int index;                 /* working index for loops */
    int result;                         BCMDNX_INIT_FUNC_DEFS_NO_UNIT;
                                        /* result for caller */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META("(*,*)\n")));

    /* be optimistic */
    result = BCM_E_NONE;

    /* check all action */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META("scanning actions\n")));
    for (index = 0;
         (index < bcmFieldActionCount) && (BCM_E_NONE == result);
         index++) {
        if (BCM_FIELD_ASET_TEST(*aset2,index)) {
            if (!BCM_FIELD_ASET_TEST(*aset1,index)) {
                result = BCM_E_FAIL;
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META("action %d (%s) in aset 2"
" but not in aset 1\n"),
                           index,
                           _bcm_dpp_field_action_name[index]));
            }
        } /* if (BCM_FIELD_ASET_TEST(aset2,index)) */
    } /* for (index = 0; index < bcmFieldActionCount; index++) */

    /* then tell the caller the result */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META("(*,*) = %d (%s)\n"),
               result,
               _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_aset_union(bcm_field_aset_t *aset0,
                          bcm_field_aset_t *aset1,
                          bcm_field_aset_t *aset2)
{
    bcm_field_aset_t_init(aset2);
    SHR_BITOR_RANGE(aset0->w, aset1->w, 0, bcmFieldActionCount, aset2->w);
}

int
_bcm_dpp_ppd_act_from_bcm_act(bcm_dpp_field_info_OLD_t *unitData,
                              _bcm_dpp_field_stage_idx_t stage,
                              bcm_field_action_t bcmAct,
                              SOC_PPC_FP_ACTION_TYPE **ppdAct)
{
    unsigned int base;
    int32 *actMap ;
    int32 actMaps_handle ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 > bcmAct) || (bcmFieldActionCount <= bcmAct)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("action %d is not valid"),
                          bcmAct));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ACTMAPS.get(unit, bcmAct, &actMaps_handle)) ;
    if (!actMaps_handle) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support action"
                                           " %d (%s)"),
                          unit,
                          bcmAct,
                          _bcm_dpp_field_action_name[bcmAct]));
    }
    /* compute the number of elements before that chain */
    base = stage * unitData->devInfo->actChain;
    /* offset for BCM action type */
    base += _BCM_DPP_FIELD_ACTMAP_OFFSET_MAPS;
    /* return the pointer to it to the caller */
    SOC_SAND_ACTMAPS_CONVERT_HANDLE_TO_ELEMENT(unit,actMaps_handle,actMap) ;
    (*ppdAct) = (SOC_PPC_FP_ACTION_TYPE*)(&(actMap[base]));
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_ppd_qual_from_bcm_qual(bcm_dpp_field_info_OLD_t *unitData,
                                _bcm_dpp_field_stage_idx_t stage,
                                uint32 types,
                                bcm_field_qualify_t bcmQual,
                                SOC_PPC_FP_QUAL_TYPE **ppdQual)
{
    unsigned int base;
    _bcm_dpp_field_type_idx_t type;
    _bcm_dpp_field_type_idx_t ltype;
    _bcm_dpp_field_type_idx_t limit;
    uint32 typeLimit;
    bcm_field_qset_t qsetType;
    int32 *qualMap ;
    int32 qualMaps_handle ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 > bcmQual) || (bcmFieldQualifyCount <= bcmQual)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier %d is not valid"),
                          bcmQual));
    }
    typeLimit = (1 << unitData->devInfo->types) - 1;
    if (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, stage).stageFlags &
        _BCM_DPP_FIELD_DEV_STAGE_USES_PRESELECTORS) {
        /* stage uses preselectors; consider them */
        limit = _BCM_DPP_FIELD_MAX_GROUP_TYPES + 1;
        typeLimit |= (1 << _BCM_DPP_FIELD_MAX_GROUP_TYPES);
    } else {
        /* stage does not use preselectors; do not consider them */
        limit = unitData->devInfo->types;
    }
    if (0 == (types & typeLimit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("must specify at least one valid"
                                           " type in search bitmap (valid"
                                           " types include %08X)"),
                          typeLimit));
    }
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, bcmQual, &qualMaps_handle)) ;
    if (!qualMaps_handle) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d does not support qualifier"
                                           " %d (%s)"),
                          unit,
                          bcmQual,
                          _bcm_dpp_field_qual_name[bcmQual]));
    }
    type = limit;
    ltype = limit;
    while (type > 0) {
        type--;
        FIELD_ACCESS.stageD.qsetType.get(unit, stage, type, &qsetType);
        if ((types & (1 << type)) &&
            BCM_FIELD_QSET_TEST(qsetType, bcmQual)) {
            ltype = type;
            break;
        }
    }
    /*
     *  We pick the *last* (highest type number) match because we expect the
     *  types to get more specific as the number increases.
     *
     *  If there are divergences between overlapped types, we expect this to
     *  provide the 'most appropriate' version of the qualifier in the context.
     */
    if (ltype >= limit) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d stage %u types %08X does"
                                           " not include qualifier %d (%s)"),
                          unit,
                          stage,
                          types,
                          bcmQual,
                          _bcm_dpp_field_qual_name[bcmQual]));
    }
    /* get the qualifier chain that we need to use */
    FIELD_ACCESS.stageD.ltOffset.get(unit, stage, ltype, &base);
    /* compute the number of elements before that chain */
    base *= unitData->devInfo->qualChain;
    /* offset for BCM qual type, width, shift distance */
    base += _BCM_DPP_FIELD_QUALMAP_OFFSET_MAPS;
    /* return the pointer to it to the caller */
    SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_ELEMENT(unit,qualMaps_handle,qualMap) ;
    (*ppdQual) = (SOC_PPC_FP_QUAL_TYPE*)(&(qualMap[base]));
exit:
    BCMDNX_FUNC_RETURN;
}

void
_bcm_dpp_field_group_priority_recalc(bcm_dpp_field_info_OLD_t *unitData,
                                     _bcm_dpp_field_grp_idx_t group)
{
    _bcm_dpp_field_ent_idx_t entry;
    unsigned int offset;
    unsigned int count;
    unsigned int phasePrio;
    _bcm_dpp_field_entry_type_t entryType;
	uint32 entryFlags;    
	uint32 hwPriority;
    _bcm_dpp_field_ent_idx_t tcam_entry_limit;
    _bcm_dpp_field_ent_idx_t entryDeLimit;
    uint32 groupFlags;   
    _bcm_dpp_field_ent_idx_t entryTail;
    _bcm_dpp_field_ent_idx_t entryHead;
    _bcm_dpp_field_stage_idx_t stage;  
    bcm_field_group_mode_t grpMode; 
    _bcm_dpp_field_ent_idx_t  entryDe_entryPrev;
    _bcm_dpp_field_ent_idx_t  entryDe_entryNext;
    uint32 entryDe_entryFlags;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;    

    FIELD_ACCESS.groupD.groupFlags.get(unit, group, &groupFlags);
    FIELD_ACCESS.groupD.stage.get(unit, group, &stage);
    FIELD_ACCESS.groupD.grpMode.get(unit, group, &grpMode);
    FIELD_ACCESS.groupD.entryHead.get(unit, group, &entryHead);
    FIELD_ACCESS.groupD.entryTail.get(unit, group, &entryTail);
    FIELD_ACCESS.stageD.modeBits.entryType.get(unit, stage, grpMode, &entryType);
    /* toggle phase */
    groupFlags ^= _BCM_DPP_FIELD_GROUP_PHASE;
    FIELD_ACCESS.groupD.groupFlags.set(unit, group, groupFlags);
    /* compute base priority from phase */
    if (groupFlags & _BCM_DPP_FIELD_GROUP_PHASE) {
        phasePrio = 1 << 13;
    } else {
        phasePrio = 0;
    }
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d group %d entering phase %d\n"),
               unit,
               group,
               (groupFlags & _BCM_DPP_FIELD_GROUP_PHASE)?1:0));
    /* set hardware priority for all existing entries in the group */
    switch (entryType) {
    case _bcmDppFieldEntryTypeDirExt:
        /* direct extraction */
        FIELD_ACCESS.entryDeLimit.get(unit, &entryDeLimit);

        if (groupFlags & _BCM_DPP_FIELD_GROUP_PHASE) {
            /* phase 1, start at high/tail and work down/to head */
            FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entryTail, &entryDe_entryPrev);
            for (entry = entryTail, offset = 7, count = 0;
                 entry < entryDeLimit;
                 entry = entryDe_entryPrev, offset--, count++) {
                FIELD_ACCESS.entryDe.entryCmn.hwPriority.set(unit, entry, offset);
                /* entry is no longer 'new' (moved or added since priority update) */
                FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags);
                entryDe_entryFlags &= (~_BCM_DPP_FIELD_ENTRY_NEW);
                /* entry has been changed since last time it was written to hardware */
                entryDe_entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
                FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, entryDe_entryFlags);
                FIELD_ACCESS.entryDe.entryCmn.entryPrev.get(unit, entry, &entryDe_entryPrev);
            }
        } else { /* if (groupData->flags & _BCM_DPP_FIELD_GROUP_PHASE) */
		/* phase 0, start at low/head and work up/to tail */
            if (entryHead < entryDeLimit) {
                FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entryHead, &entryDe_entryNext);
            }
            for (entry = entryHead, offset = 0, count = 0;
                 entry < entryDeLimit;
                 entry = entryDe_entryNext, offset++, count++) {
                FIELD_ACCESS.entryDe.entryCmn.hwPriority.set(unit, entry, offset);
                /* entry is no longer 'new' (moved or added since priority update) */
                FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, entry, &entryDe_entryFlags);
                entryDe_entryFlags &= (~_BCM_DPP_FIELD_ENTRY_NEW);
                /* entry has been changed since last time it was written to hardware */
                entryDe_entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
                FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, entry, entryDe_entryFlags);
                FIELD_ACCESS.entryDe.entryCmn.entryNext.get(unit, entry, &entryDe_entryNext);
            }
        } /* if (groupData->flags & _BCM_DPP_FIELD_GROUP_PHASE) */
        break;
    default:
        /* default is TCAM */
        for (entry = entryHead, offset = 0, count = 0, _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(entryType);
                (entry < tcam_entry_limit);
                /*
                 * Was:
                 *   entry = _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry), offset++, count++
                 */
                 _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit,_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entry), offset++, count++) {
            /*
             * Was:
             *    hwPriority = _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
             *    *hwPriority = ((_BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry) & 1) |
             *                                         (phasePrio + (offset << 1)));
             */
            /* entry is no longer 'new' (moved or added since priority update)
             *    entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry);
             *    *entryFlags &= (~_BCM_DPP_FIELD_ENTRY_NEW);
             */
            /* entry has been changed since last time it was written to hardware
             *    *entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
             */
            _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_GET(unit, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, hwPriority);
            hwPriority = (hwPriority & 1) | (phasePrio + (offset << 1));
            if (!SOC_WARM_BOOT(unit)) {  /*Cold boot -  Update sw-state resources */
                _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_SET(unit, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, hwPriority);
            }
            /* entry is no longer 'new' (moved or added since priority update) */
            _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entryFlags);
            entryFlags &= (~_BCM_DPP_FIELD_ENTRY_NEW);
            /* entry has been changed since last time it was written to hardware */
            entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
            if (!SOC_WARM_BOOT(unit)) {  /*Cold boot -  Update sw-state resources */
                _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit, _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), entry, entryFlags);
            }
        }
    }
    /* no entries have been added/moved since the hw priorities update now */
    groupFlags &= (~_BCM_DPP_FIELD_GROUP_ADD_ENT);
    FIELD_ACCESS.groupD.groupFlags.set(unit, group, groupFlags);
#ifdef BCM_WARM_BOOT_SUPPORT
    /*_bcm_dpp_field_group_wb_save(unitData, group, NULL, NULL);*/
#endif /* def BCM_WARM_BOOT_SUPPORT */
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d group %d updated hardware priority on"
                          " %d entries\n"),
               unit,
               group,
               count));
exit:
    BCMDNX_FUNC_RETURN_VOID ;
}

int
_bcm_dpp_field_stage_type_qset_aset_get(int unit,
                                        _bcm_dpp_field_stage_idx_t stage,
                                        _bcm_dpp_field_type_idx_t type,
                                        bcm_field_qset_t *qset,
                                        bcm_field_aset_t *aset)
{
    _DPP_FIELD_COMMON_LOCALS;
    BCMDNX_INIT_FUNC_DEFS;

    _DPP_FIELD_UNIT_CHECK(unit, unitData);
    if ((!qset) || (!aset)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("obligatory OUT args must not be NULL")));
    }
    _DPP_FIELD_UNIT_LOCK(unitData);

    if (stage > unitData->devInfo->stages) {
        result = BCM_E_NOT_FOUND;
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d does not have stage %u\n"),
                   unit,
                   stage));
    }
    if (type > unitData->devInfo->types) {
        result = BCM_E_NOT_FOUND;
        LOG_ERROR(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d does not have type %d\n"),
                   unit,
                   type));
    }
    if (BCM_E_NONE == result) {
        FIELD_ACCESS.stageD.qsetType.get(unit, stage, type, qset);
        FIELD_ACCESS.stageD.stgAset.get(unit, stage, aset);
    }

    _DPP_FIELD_UNIT_UNLOCK(unitData);
    BCMDNX_IF_ERR_EXIT(result);
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_group_qset_recover(bcm_dpp_field_info_OLD_t *unitData,
                                  _bcm_dpp_field_stage_idx_t stage,
                                  uint32 types,
                                  _bcm_dpp_field_qual_set_t *ppdQset,
                                  bcm_field_qset_t *bcmQset)
{
    unsigned int qualifier;
    unsigned int type;
    unsigned int chain;
    unsigned int base;
    unsigned int offset;
    unsigned int ppdQual;
    unsigned int bcmQual;
    int include;
    int result;
    int unit;
    unsigned int ltOffset, qualMapCount;

    unit = unitData->unit;
    result = BCM_E_NONE ;

    BCM_FIELD_QSET_INIT(*bcmQset);
    result = FIELD_ACCESS.qualMapCount.get(unit, &qualMapCount) ;
    if (BCM_E_NONE != result) {
        goto exit ;
    }
    /* scan for standard qualifiers */
    for (qualifier = 0; qualifier < qualMapCount; qualifier++) {
        base = qualifier * ((unitData->devInfo->mappings *
                             unitData->devInfo->qualChain) +
                            _BCM_DPP_FIELD_QUALMAP_OFFSET_MAPS);
        for (type = 0; type < unitData->devInfo->types; type++) {
            if (0 == (types & (1 << type))) {
                /* the group does not include this type */
                continue;
            }
            FIELD_ACCESS.stageD.ltOffset.get(unit, stage, type, &ltOffset);
            offset = (base +
                      (ltOffset * unitData->devInfo->qualChain) +
                      _BCM_DPP_FIELD_QUALMAP_OFFSET_MAPS);
            /*
             *  Consider including this qualifier if it is valid here.  Also,
             *  consider including this qualifier if it is tagged as being
             *  only a flag qualifier for this particular type in this
             *  particular stage (in this case, it will be included unless
             *  there is another qualifier required that is neither implied nor
             *  included in the hardware qualifiers).
             */
            include = ((_BCM_DPP_FIELD_PPD_QUAL_VALID(unitData->devInfo->qualMap[offset])) ||
                       (SOC_PPC_NOF_FP_QUAL_TYPES + 1 ==
                        unitData->devInfo->qualMap[offset]));
            for (chain = 0;
                 include && (chain < unitData->devInfo->qualChain);
                 chain++) {
                ppdQual = unitData->devInfo->qualMap[offset + chain];
                if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual)) {
                    /* this chain element is a valid type */
                    if (!SHR_BITGET(*ppdQset, ppdQual)) {
                        /* but this group does not have this qualifier */
                        include = FALSE;
                    } /* if (!SHR_BITGET(groupData->pqset, ppdQual)) */
                } /* if (this qualifier is valid) */
            } /* for (all qualifiers in this chain) */
            if (!include) {
                /* should not be included; try the next type */
                continue;
            }
            bcmQual = unitData->devInfo->qualMap[base + _BCM_DPP_FIELD_QUALMAP_OFFSET_QUAL];

            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "PPD qualifier chain at qual %u type %s"
                                  " (%d) implies %s (%d) should be in qset"
                                  " for unit %d stage %s (%u) types %08X\n"),
                       qualifier,
                       unitData->devInfo->typeNames[type],
                       type,
                       _bcm_dpp_field_qual_name[bcmQual],
                       bcmQual,
                       unit,
                       unitData->devInfo->stage[stage].stageName,
                       stage,
                       types));
            BCM_FIELD_QSET_ADD(*bcmQset, bcmQual);
        } /* for (type = 0; type < unitData->devInfo->types; type++) */
    } /* for (all qualifier maps supported by this unit) */
    /* scan for data qualifiers */
    for (ppdQual = 0; ppdQual < SOC_PPC_NOF_FP_QUAL_TYPES; ppdQual++) {
        if (SHR_BITGET(*ppdQset, ppdQual)) {
            /* this qualifier is included for this group */
            if(SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(ppdQual)) {
                /* this qualifier mapped to a data qualifier */
                result = _bcm_petra_field_map_ppd_udf_to_bcm(unitData,
                                                             ppdQual,
                                                             &bcmQual);
                if (BCM_E_NONE == result) {
                    LOG_DEBUG(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "PPD qualifier %s %d implies data"
                                          " qualifier %u should be in qset"
                                          " for unit %u stage %s (%u)"
                                          " (types %08X)\n"),
                               SOC_PPC_FP_QUAL_TYPE_to_string(ppdQual),
                               ppdQual,
                               bcmQual,
                               unit,
                               unitData->devInfo->stage[stage].stageName,
                               stage,
                               types));
                    SHR_BITSET(bcmQset->udf_map, bcmQual);
                } /* if (BCM_E_NONE == result) */
            } /* if (SOC_PPC_FP_IS_QUAL_TYPE_USER_DEFINED(ppdQual)) */
        } /* if (SHR_BITGET(groupData->pqset, ppdQual)) */
    } /* for (all PPD layer qualifiers) */
    result = BCM_E_NONE ;
exit:
    return (result);
}

int
_bcm_dpp_field_group_aset_recover(bcm_dpp_field_info_OLD_t *unitData,
                                  _bcm_dpp_field_stage_idx_t stage,
                                  _bcm_dpp_field_action_set_t *ppdAset,
                                  bcm_field_aset_t *bcmAset)
{
    unsigned int action;
    unsigned int chain;
    unsigned int base;
    unsigned int offset;
    unsigned int ppdAct, actMapCount ;
    int include;
    int unit;
    int result ;

    unit = unitData->unit;
    result = BCM_E_NONE ;
    BCM_FIELD_ASET_INIT(*bcmAset);
    result = FIELD_ACCESS.actMapCount.get(unit, &actMapCount) ;
    if (result != BCM_E_NONE) {
        goto exit ;
    }
    for (action = 0; action < actMapCount; action++) {
        base = action * ((unitData->devInfo->stages *
                          unitData->devInfo->actChain) +
                         _BCM_DPP_FIELD_ACTMAP_OFFSET_MAPS);
            offset = (base +
                  (stage * unitData->devInfo->actChain) +
                      _BCM_DPP_FIELD_ACTMAP_OFFSET_MAPS);
            /* assume the group might include this qualifier */
            include = _BCM_DPP_FIELD_PPD_ACTION_VALID(unitData->devInfo->actMap[offset]);
            for (chain = 0;
                 include && (chain < unitData->devInfo->actChain);
                 chain++) {
                ppdAct = unitData->devInfo->actMap[offset + chain];
                if (_BCM_DPP_FIELD_PPD_ACTION_VALID(ppdAct)) {
                    /* this chain element is a valid type */
                    if (!SHR_BITGET(*ppdAset, ppdAct)) {
                        /* but this group does not have this qualifier */
                        include = FALSE;
                    } /* if (!SHR_BITGET(groupData->pqset, ppdQual)) */
                } /* if (this qualifier is valid) */
            } /* for (all qualifiers in this chain) */
            if (!include) {
                /* should not be included; try the next type */
                continue;
            }

        LOG_VERBOSE(BSL_LS_BCM_FP,
                    (BSL_META_U(unit,
                                "PPD action chain at action %u"
                                " implies %s (%d) should be in aset"
                                " for unit %d stage %s (%u)\n"),
                     action,
                     _bcm_dpp_field_action_name[unitData->devInfo->actMap[base + _BCM_DPP_FIELD_ACTMAP_OFFSET_ACT]],
                     unitData->devInfo->actMap[base + _BCM_DPP_FIELD_ACTMAP_OFFSET_ACT],
                     unit,
                     unitData->devInfo->stage[stage].stageName,
                     stage));

            BCM_FIELD_ASET_ADD(*bcmAset,
                               unitData->devInfo->actMap[base + _BCM_DPP_FIELD_ACTMAP_OFFSET_ACT]);
    } /* for (all action maps supported by this unit) */
exit:
    return (result) ;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_set_single_int
 *  Purpose
 *    Set a single qualifier on an entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
 *    (in) qual = pointer to qualifier array for the entry
 *    (in) qualLimit = max qualifier index applicable to this entry
 *    (in) hwType = PPD layer qualifier to add to the entry
 *    (in) data = data value for qualifier
 *    (in) mask = mask bits for qualifier
 *    (in) valid = valid bits for qualifier
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Caller must already hold the unit lock.
 *
 *    Warns if data or mask bits are outside of valid bits, or if data bits are
 *    outside of mask bits.  The data and mask are the values for the PPD
 *    layer, and are therefore not shifted or concatenated or sliced here.
 *
 *    Sets the BCM layer qualifier according to the reverse mapping.
 *
 *    Removes a qualifier from the list if its mask is all zeroes.  When a BCM
 *    qualifier is a proper subset of a PPD qualifier, it only affects those
 *    bits in the PPD qualifier, so if there are other valid bits at PPD, the
 *    qualifier will remain.
 *
 *    Caller must update entry flags, group flags, &c, as appropriate.
 */
STATIC int
_bcm_dpp_field_qualifier_set_single_int(bcm_dpp_field_info_OLD_t *unitData,
                                        bcm_field_entry_t entry,
                                        _bcm_dpp_field_qual_t *qual,
                                        bcm_field_qualify_t bcmQual,
                                        unsigned int qualLimit,
                                        SOC_PPC_FP_QUAL_TYPE hwType,
                                        const uint64 data,
                                        const uint64 mask,
                                        const uint64 valid)
{
    uint64 edata;
    uint64 emask;
    uint64 ivalid;
    unsigned int index;
    unsigned int free;
    _bcm_dpp_field_entry_type_t entryType;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(*(%d),%d,%d;%s(%d;%s),"
                          "%08X%08X,%08X%08X,%08X%08X)\n"),
               unit,
               entry,
               hwType,
               SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
               bcmQual,
               (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
               COMPILER_64_HI(data),
               COMPILER_64_LO(data),
               COMPILER_64_HI(mask),
               COMPILER_64_LO(mask),
               COMPILER_64_HI(valid),
               COMPILER_64_LO(valid)));
    edata = data;
    emask = mask;
    ivalid = valid;
    COMPILER_64_NOT(ivalid);
    COMPILER_64_AND(edata, ivalid);
    if (!COMPILER_64_IS_ZERO(edata)) {
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d entry %d qualifier %d;%s (%d;%s)"
                             " data %08X%08X has bits not marked"
                             " as valid %08X%08X\n"),
                  unit,
                  entry,
                  hwType,
                  SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                  bcmQual,
                  (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
                  COMPILER_64_HI(data),
                  COMPILER_64_LO(data),
                  COMPILER_64_HI(valid),
                  COMPILER_64_LO(valid)));
    }
    COMPILER_64_AND(emask, ivalid);
    if (!COMPILER_64_IS_ZERO(emask)) {
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d entry %d qualifier %d;%s (%d;%s)"
                             " mask %08X%08X has bits not marked"
                             " as valid %08X%08X\n"),
                  unit,
                  entry,
                  hwType,
                  SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                  bcmQual,
                  (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
                  COMPILER_64_HI(mask),
                  COMPILER_64_LO(mask),
                  COMPILER_64_HI(valid),
                  COMPILER_64_LO(valid)));
    }
    edata = data;
    emask = mask;
    COMPILER_64_NOT(emask);
    COMPILER_64_AND(edata, emask);
    if (!COMPILER_64_IS_ZERO(edata)) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d qualifier %d;%s (%d;%s)"
                              " data %08X%08X has bits not included"
                              " in mask %08X%08X; these bits will"
                              " be ignored in matching\n"),
                   unit,
                   entry,
                   hwType,
                   SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                   bcmQual,
                   (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
                   COMPILER_64_HI(data),
                   COMPILER_64_LO(data),
                   COMPILER_64_HI(mask),
                   COMPILER_64_LO(mask)));
    }
    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        int32 commonHandle ;
        _bcm_dpp_field_qual_t qual_at_index ;
        _bcm_dpp_field_qual_t qual_at_free ;
        int isExternalTcam ;
        /*
         * Original in _bcm_dpp_field_qualifier_set_prep() was:
         *   *qualData = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
         *                                            _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
         *
         * The rule for changing into new sw state: qual[index] ==> qual_at_index
         * The rule for changing into new sw state: qual[free] ==> qual_at_free
         */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_common_pointer(unitData,
                                                                    entry,
                                                                    &entryType,&commonHandle));
        isExternalTcam = _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType) ;
        _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), 0, qual_at_index) ;

        /* find either the old value or a free slot */
        for (index = 0, free = qualLimit;
             (index < qualLimit) && (qual_at_index.hwType != hwType);
             index++) {
            /* look for free quals until scanned all or hit requested qual */
            if ((!_BCM_DPP_FIELD_PPD_QUAL_VALID(qual_at_index.hwType)) &&
                (qualLimit <= free)) {
                /* first free one seen; keep it */
                free = index;
            }
            if ((index + 1) < qualLimit) {
                _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), (index + 1), qual_at_index) ;
            }
        } /* for (all possible qualifier slots this stage) */
        _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), index, qual_at_index) ;
        edata = data;
        emask = mask;
        COMPILER_64_AND(edata, valid);
        COMPILER_64_AND(emask, valid);
        COMPILER_64_AND(edata, mask);
        if (index < qualLimit) {
            /* found existing instance, update it */
            /*
             *  Note that some qualifiers are subsets of others, and we consider
             *  these subsets to be just a masked part of the superset. This means
             *  that we need to allow multiple subsets of a superset, thence we
             *  must only overwrite the subset at hand whenever updating a subset
             *  of an existing qualifier.
             */
            COMPILER_64_AND(qual_at_index.qualData, ivalid);
            COMPILER_64_AND(qual_at_index.qualMask, ivalid);
            COMPILER_64_OR(qual_at_index.qualData, edata);
            COMPILER_64_OR(qual_at_index.qualMask, emask);
            _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_SET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), index, qual_at_index) ;
        } else if (free < qualLimit) {
            /* did not find existing instance, use the free slot */
            /*
             *  No need to worry particularly about being a subset of an
             *  existing qualifier if there is no existing qualifier.
             */
            _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), free, qual_at_free) ;
            qual_at_free.hwType = hwType;
            qual_at_free.qualType = bcmQual;
            qual_at_free.qualData = edata;
            qual_at_free.qualMask = emask;
            _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_SET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), free, qual_at_free) ;
            index = free;
            _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), index, qual_at_index) ;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have hw"
                                               " qualifier %d (%s) but already has"
                                               " as many hw qualifiers as it is"
                                               " allowed to have %d"),
                              unit,
                              entry,
                              hwType,
                              SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                              qualLimit));
        }
        /* Remove the zeroes qualifiers except for the direct table entries */
        if (COMPILER_64_IS_ZERO(qual_at_index.qualMask) 
              && _BCM_DPP_FIELD_ENT_IS_DIR_EXT(unit, entry)) {
            /* the significant bits are all zeroes, so remove it */
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d qualifier %d (%s) (%d (%s))"
                                  " removed since it has no significant bits\n"),
                       unit,
                       entry,
                       bcmQual,
                       (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
                       hwType,
                       SOC_PPC_FP_QUAL_TYPE_to_string(hwType)));
            COMPILER_64_ZERO(qual_at_index.qualData);
            COMPILER_64_ZERO(qual_at_index.qualMask);
            qual_at_index.hwType = BCM_FIELD_ENTRY_INVALID;
            qual_at_index.qualType = BCM_FIELD_ENTRY_INVALID;
            _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_SET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), index, qual_at_index) ;
        } else {
            LOG_DEBUG(BSL_LS_BCM_FP,
                      (BSL_META_U(unit,
                                  "unit %d entry %d qualifier %d (%s) (%d (%s))"
                                  " now %08X%08X/%08X%08X\n"),
                       unit,
                       entry,
                       bcmQual,
                       (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
                       hwType,
                       SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                       COMPILER_64_HI(qual_at_index.qualData),
                       COMPILER_64_LO(qual_at_index.qualData),
                       COMPILER_64_HI(qual_at_index.qualMask),
                       COMPILER_64_LO(qual_at_index.qualMask)));
        }
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /* find either the old value or a free slot */
        for (index = 0, free = qualLimit;
             (index < qualLimit) && (qual[index].hwType != hwType);
             index++) {
            /* look for free quals until scanned all or hit requested qual */
            if ((!_BCM_DPP_FIELD_PPD_QUAL_VALID(qual[index].hwType)) &&
                (qualLimit <= free)) {
                /* first free one seen; keep it */
                free = index;
            }
        } /* for (all possible qualifier slots this stage) */
        edata = data;
        emask = mask;
        COMPILER_64_AND(edata, valid);
        COMPILER_64_AND(emask, valid);
        COMPILER_64_AND(edata, mask);
        if (index < qualLimit) {
            /* found existing instance, update it */
            /*
             *  Note that some qualifiers are subsets of others, and we consider
             *  these subsets to be just a masked part of the superset. This means
             *  that we need to allow multiple subsets of a superset, thence we
             *  must only overwrite the subset at hand whenever updating a subset
             *  of an existing qualifier.
             */
            COMPILER_64_AND(qual[index].qualData, ivalid);
            COMPILER_64_AND(qual[index].qualMask, ivalid);
            COMPILER_64_OR(qual[index].qualData, edata);
            COMPILER_64_OR(qual[index].qualMask, emask);
        } else if (free < qualLimit) {
            /* did not find existing instance, use the free slot */
            /*
             *  No need to worry particularly about being a subset of an
             *  existing qualifier if there is no existing qualifier.
             */
            qual[free].hwType = hwType;
            qual[free].qualType = bcmQual;
            qual[free].qualData = edata;
            qual[free].qualMask = emask;
            index = free;
        } else {
            BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d PRESEL entry %d does not have hw"
                                               " qualifier %d (%s) but already has"
                                               " as many hw qualifiers as it is"
                                               " allowed to have %d"),
                              unit,
                              entry,
                              hwType,
                              SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                              qualLimit));
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not valid"),
                          unit,
                          entry));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 * This procedure is the samje as _bcm_dpp_field_qualifier_set_single_int() but
 * to be used for 'entryDe' only.
 */
STATIC int
_bcm_dpp_field_qualifier_set_single_int_new(bcm_dpp_field_info_OLD_t *unitData,
                                        bcm_field_entry_t entry,
                                        bcm_field_qualify_t bcmQual,
                                        unsigned int qualLimit,
                                        SOC_PPC_FP_QUAL_TYPE hwType,
                                        const uint64 data,
                                        const uint64 mask,
                                        const uint64 valid)
{
    uint64 edata;
    uint64 emask;
    uint64 ivalid;
    int u64_ndx;
    unsigned int first_valid_bit = 0;
    unsigned int last_valid_bit = 0;
    unsigned int index;
    unsigned int free;
    uint64 entryDe_qualMask;
    uint64 entryDe_qualData;
    SOC_PPC_FP_QUAL_TYPE entryDe_hwType;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    entry = entry - _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt);

    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(*(%d),%d,%d;%s(%d;%s),"
                          "%08X%08X,%08X%08X,%08X%08X)\n"),
               unit,
               entry,
               hwType,
               SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
               bcmQual,
               (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
               COMPILER_64_HI(data),
               COMPILER_64_LO(data),
               COMPILER_64_HI(mask),
               COMPILER_64_LO(mask),
               COMPILER_64_HI(valid),
               COMPILER_64_LO(valid)));
    edata = data;
    emask = mask;
    ivalid = valid;
    COMPILER_64_NOT(ivalid);
    COMPILER_64_AND(edata, ivalid);
    if (!COMPILER_64_IS_ZERO(edata)) {
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d entry %d qualifier %d;%s (%d;%s)"
                             " data %08X%08X has bits not marked"
                             " as valid %08X%08X\n"),
                  unit,
                  entry,
                  hwType,
                  SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                  bcmQual,
                  (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
                  COMPILER_64_HI(data),
                  COMPILER_64_LO(data),
                  COMPILER_64_HI(valid),
                  COMPILER_64_LO(valid)));
    }
    COMPILER_64_AND(emask, ivalid);
    if (!COMPILER_64_IS_ZERO(emask)) {
        LOG_WARN(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "unit %d entry %d qualifier %d;%s (%d;%s)"
                             " mask %08X%08X has bits not marked"
                             " as valid %08X%08X\n"),
                  unit,
                  entry,
                  hwType,
                  SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                  bcmQual,
                  (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
                  COMPILER_64_HI(mask),
                  COMPILER_64_LO(mask),
                  COMPILER_64_HI(valid),
                  COMPILER_64_LO(valid)));
    }
    edata = data;
    emask = mask;
    COMPILER_64_NOT(emask);
    COMPILER_64_AND(edata, emask);
    if (!COMPILER_64_IS_ZERO(edata)) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                              "unit %d entry %d qualifier %d;%s (%d;%s)"
                              " data %08X%08X has bits not included"
                              " in mask %08X%08X; these bits will"
                              " be ignored in matching\n"),
                   unit,
                   entry,
                   hwType,
                   SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                   bcmQual,
                   (BCM_FIELD_ENTRY_INVALID != bcmQual) ? _bcm_dpp_field_qual_name[bcmQual] : "-",
                   COMPILER_64_HI(data),
                   COMPILER_64_LO(data),
                   COMPILER_64_HI(mask),
                   COMPILER_64_LO(mask)));
    }

    /* find either the old value or a free slot */
    FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.get(unit, entry, 0, &entryDe_hwType); 
    for (index = 0, free = qualLimit;
         (index < qualLimit) && (entryDe_hwType != hwType);
         index++) {
        /* look for free quals until scanned all or hit requested qual */
        if ((!_BCM_DPP_FIELD_PPD_QUAL_VALID(entryDe_hwType)) &&
            (qualLimit <= free)) {
            /* first free one seen; keep it */
            free = index;
        }
        if (index + 1 < qualLimit)
        	FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.get(unit, entry, index + 1, &entryDe_hwType);
    } /* for (all possible qualifier slots this stage) */
    edata = data;
    emask = mask;
    COMPILER_64_AND(edata, valid);
    COMPILER_64_AND(emask, valid);
    COMPILER_64_AND(edata, mask);
    if (index < qualLimit) {
        /* found existing instance, update it */
        /*
         *  Note that some qualifiers are subsets of others, and we consider
         *  these subsets to be just a masked part of the superset. This means
         *  that we need to allow multiple subsets of a superset, thence we
         *  must only overwrite the subset at hand whenever updating a subset
         *  of an existing qualifier.
         */
        FIELD_ACCESS.entryDe.entryCmn.entryQual.qualData.get(unit, entry, index, &entryDe_qualData); 
        FIELD_ACCESS.entryDe.entryCmn.entryQual.qualMask.get(unit, entry, index, &entryDe_qualMask);
        COMPILER_64_AND(entryDe_qualData, ivalid);
        COMPILER_64_AND(entryDe_qualMask, ivalid);
        COMPILER_64_OR(entryDe_qualData, edata);
        COMPILER_64_OR(entryDe_qualMask, emask);
    } else if (free < qualLimit) {
        /* did not find existing instance, use the free slot */
        /*
         *  No need to worry particularly about being a subset of an
         *  existing qualifier if there is no existing qualifier.
         */
        entryDe_qualData = edata;
        entryDe_qualMask = emask;
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d does not have hw"
                                           " qualifier %d (%s) but already has"
                                           " as many hw qualifiers as it is"
                                           " allowed to have %d"),
                          unit,
                          entry,
                          hwType,
                          SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
                          qualLimit));
    }
    
    /* Direct extraction qualifier can select up to 4 consequent bits */
    if (!COMPILER_64_IS_ZERO(entryDe_qualMask)) {
        for (u64_ndx=(sizeof(entryDe_qualMask)*8-1); u64_ndx>=0; --u64_ndx) {
            if (u64_ndx >= 32) {
                if (COMPILER_64_HI(entryDe_qualMask) & (0x1L << (u64_ndx-32))) {
                    if (first_valid_bit == 0) first_valid_bit = u64_ndx;
                    last_valid_bit = u64_ndx;
                }
            }
            else {
                if (COMPILER_64_LO(entryDe_qualMask) & (0x1L << u64_ndx)) {
                    if (first_valid_bit == 0) first_valid_bit = u64_ndx;
                    last_valid_bit = u64_ndx;
                }
            }
        }
    
        if ((first_valid_bit - last_valid_bit) > 3) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG_NO_UNIT("Qualifier of a direct extract entry can only select up to 4 bits")));
        }
    }

    if (index >= qualLimit) {
        /* New */
        FIELD_ACCESS.entryDe.entryCmn.entryQual.hwType.set(unit, entry, free, hwType);  
        FIELD_ACCESS.entryDe.entryCmn.entryQual.qualType.set(unit, entry, free, bcmQual);
        index = free;
    }
    
    FIELD_ACCESS.entryDe.entryCmn.entryQual.qualMask.set(unit, entry, index, entryDe_qualMask);
    FIELD_ACCESS.entryDe.entryCmn.entryQual.qualData.set(unit, entry, index, entryDe_qualData);

exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_set_prep
 *  Purpose
 *    Prepare for manipulating the qualifiers for some kind of entry/presel
 *  Arguments
 *    (in) unitData = pointer to unit data
 *    (in) entry = BCM layer entry ID
 *    (out) qualData = where to put pointer to qual data array
 *    (out) qualLimit = where to put max quals in qual data array
 *    (out) stage = where to put the applicable stage ID
 *    (out) bqset = where to put a pointer to the applicable BCM QSET
 *                    If NULL then do not apply this output.
 *    (out) pqset = where to put a pointer to the applicable PPD QSET
 *                    If NULL then do not apply this output.
 *    (out) groupTypes = where to put the group types bitmap
 *                    If NULL then do not apply this output.
 *  Returns
 *    int (implied cast from bcm_error_t)
 *      BCM_E_NONE if the proposed qset is acceptable
 *      BCM_E_* appropriately if not
 */

STATIC int
_bcm_dpp_field_qualifier_set_prep(bcm_dpp_field_info_OLD_t *unitData,
                                  bcm_field_entry_t entry,
                                  _bcm_dpp_field_qual_t **qualData,
                                  unsigned int *qualLimit,
                                  _bcm_dpp_field_stage_idx_t *stage,
                                  _bcm_dpp_field_grp_idx_t *entryGroup)
{
    _bcm_dpp_field_entry_type_t entryType;
    uint32 entryFlags;
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint8 bit_value ;
    uint8 is_second_pass;
    _bcm_dpp_field_stage_idx_t groupD_stage;
    uint32 entryDe_entryFlags;
    uint32 presel_flags = 0;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    bcm_field_entry_t ent;
    bcm_field_stage_t bcm_stage;
    int index;
    int isExternalTcam ;
    _bcm_dpp_field_grp_idx_t localEntryGroup;        /* this entry's group */
    _bcm_dpp_field_grp_idx_t groupLimit;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupLimit.get(unit, &groupLimit));
    
    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        int32 commonHandle ;
        /* 'normal' entries are cached and so we use the cache */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_common_pointer(unitData,
                                                                    entry,
                                                                    &entryType,&commonHandle));
        ent = commonHandle - 1;
        switch (entryType) {
        case _bcmDppFieldEntryTypeInternalTcam:
        case _bcmDppFieldEntryTypeExternalTcam:
            /*
             * Was:
             *   *entryGroup = _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
             *         _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
             */
            isExternalTcam = _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType) ;
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), localEntryGroup)) ;
            *entryGroup = localEntryGroup ;
            if (groupLimit <= *entryGroup) {
                BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: entry doesn't belong to any group")));
            }

            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, *entryGroup, &groupD_stage));
            *qualLimit = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryMaxQuals;
            /*
             * Was:
             *   entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
             *                                        _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
             */
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), entryFlags)) ;
            /*
             * Was:
             *   *qualData = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
             *                                            _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
             * See explanation right below.
             */
            /*
             * The only user of this piece of data (qualData) for _bcmDppFieldEntryTypeInternalTcam/_bcmDppFieldEntryTypeExternalTcam
             * is  _bcm_dpp_field_qualifier_set_single_int()  (See 'paths' below) so move it there and
             * eliminate usage of 'qualData' (Since it is a pointer, it presents a problem for new SW state.
             * Paths:
             * A.
             *   _bcm_dpp_field_qualifier_set_single()
             *     _bcm_dpp_field_qualifier_set_prep()
             *     _bcm_dpp_field_qualifier_set_single_int()
             * B.
             *   _bcm_dpp_field_qualifier_set()
             *     _bcm_dpp_field_qualifier_set_prep()
             *     _bcm_dpp_field_qualifier_set_int()
             *     _bcm_dpp_field_qualifier_set_single_int()
             * Note:
             *   _bcm_dpp_field_qualifier_set_cleanup() is also a user of 'qualData' but only for 'PRESEL'.
             */
            break;
        case _bcmDppFieldEntryTypeDirExt:
            /*entryGroup = (common)->entryGroup;*/
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, ent, &entryDe_entryGroup));
            *entryGroup = entryDe_entryGroup;
            if (groupLimit <= *entryGroup) {
                BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: entry doesn't belong to any group")));
            }
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, ent, &entryDe_entryFlags));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, entryDe_entryGroup, &groupD_stage));
            *qualLimit = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, groupD_stage).entryDeMaxQuals;
            entryFlags = entryDe_entryFlags;
            /*qualData = &((common)->entryQual[0]);*/
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d unknown"
                                               " type %d"),
                              unit,
                              entry,
                              entryType));
        } /* switch (entryType) */
        if (0 == (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use"),
                              unit,
                              entry));
        } /* if (0 == (common->entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) */
        *stage = groupD_stage;
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /* preselectors are not cached, so need to fake it for them */
        *qualData = NULL;
        *qualLimit = SOC_PPC_FP_NOF_QUALS_PER_DB_MAX;
        index = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry) ;
        if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(entry)) {
            bcm_stage =   _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(entry) ;
            is_second_pass = _BCM_DPP_FIELD_ENTRY_IS_PRESEL_STAGGERED(entry);
            presel_flags =  _BCM_DPP_PRESEL_ALLOC_WITH_STAGE; 
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit,entry, bcm_stage, is_second_pass,&index, stage)); 
        }
          
        if ((0 > index) || (preselLimit <= index)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %d invalid"),
                              unit,
                              index));
        }
        /*
         * Was:
         *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, entry))
         * Which was equivalent to:
         *   if (!SHR_BITGET(unitData->preselInUse.w, entry))
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
        if (!bit_value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %d not in use"),
                              unit,
                              index));
        }
        *qualData = sal_alloc(sizeof(**qualData) * (*qualLimit),
                              "preselector 'set' working space");
        if (!(*qualData)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate %u"
                                               " bytes presel workspace"),
                              unit,
                              (uint32)sizeof(**qualData) * (*qualLimit)));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                               index,
                                                               *qualLimit,
                                                               presel_flags , 
                                                               stage,
                                                               *qualData));
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not valid"),
                          unit,
                          entry));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_set_cleanup
 *  Purpose
 *    Perform any work that needs to be done after updating the qualifiers on
 *    an entry or preselector or whatever else
 *  Arguments
 *    (in) unitData = pointer to unit data
 *    (in) entry = BCM layer entry ID
 *    (in) qualData = new qualifier data array
 *    (in) changed = TRUE if something was changed
 *  Returns
 *    nothing
 */
STATIC int
_bcm_dpp_field_qualifier_set_cleanup(bcm_dpp_field_info_OLD_t *unitData,
                                     bcm_field_entry_t entry,
                                     _bcm_dpp_field_qual_t *qualData,
                                     _bcm_dpp_field_stage_idx_t stage,
                                     int changed)
{
    _bcm_dpp_field_entry_type_t entryType;
    uint32 groupD_groupFlags;
    _bcm_dpp_field_grp_idx_t entryDe_entryGroup;
    int result = BCM_E_NONE;
    int  index;
    uint32 entryDe_entryFlags;
    uint32 flags = 0;
    uint8 is_second_pass;
    bcm_field_stage_t bcm_stage;
    bcm_field_entry_t ent;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        if (changed) {
            int32 commonHandle ;
            uint32 entryFlags ;
            /* update entry and group flags */
            result = _bcm_dpp_field_entry_common_pointer(unitData,
                                                         entry,
                                                         &entryType,&commonHandle);
            ent = commonHandle - 1;
            /* that call should NOT fail at this point... */
            if (BCM_E_NONE == result) {
                if (entryType == _bcmDppFieldEntryTypeExternalTcam) {
                    /* this entry has changed */
                    /*
                     * Was:
                     *   extCommon->entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
                     */
                    result = FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryFlags.get(unit,ent,&entryFlags) ;
                    if (BCM_E_NONE == result) {
                        entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED ;
                        result = FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryFlags.set(unit,ent,entryFlags) ;
                        if (BCM_E_NONE == result) {
                            _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */

                             /* the group contains changed entry/entries */
                            result = FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryGroup.get(unit,ent,&entryGroup) ;
                            /*
                             * Was:
                             *   FIELD_ACCESS.groupD.groupFlags.get(unit, extCommon->entryGroup, &groupD_groupFlags);
                             *   groupD_groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
                             *   FIELD_ACCESS.groupD.groupFlags.set(unit, extCommon->entryGroup, groupD_groupFlags);
                             */
                            /*
                             * OR all results to get indication on error (It will not properly indicate
                             * the type of error.
                             */
                            result |= FIELD_ACCESS.groupD.groupFlags.get(unit, entryGroup, &groupD_groupFlags);
                            groupD_groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
                            result |= FIELD_ACCESS.groupD.groupFlags.set(unit, entryGroup, groupD_groupFlags);
                        }
                    }
                }
                else if (entryType ==_bcmDppFieldEntryTypeDirExt) /* direct extraction*/ {

                    FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, ent, &entryDe_entryFlags);
                    entryDe_entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
                    FIELD_ACCESS.entryDe.entryCmn.entryFlags.set(unit, ent, entryDe_entryFlags);

                    
                    FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, ent, &entryDe_entryGroup);

                    /* the group contains changed entry/entries */
                    FIELD_ACCESS.groupD.groupFlags.get(unit, entryDe_entryGroup, &groupD_groupFlags);
                    groupD_groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
                    FIELD_ACCESS.groupD.groupFlags.set(unit, entryDe_entryGroup, groupD_groupFlags);


                } else {/* internal TCAM  */
                    _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */

                    result = FIELD_ACCESS_ENTRYTC.entryCmn.entryGroup.get(unit,ent,&entryGroup) ;
                    /* this entry has changed */
                    /*
                     * Was:
                     *   common->entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED;
                     */
                    /*
                     * OR all results to get indication on error (It will not properly indicate
                     * the type of error.
                     */
                    result |= FIELD_ACCESS_ENTRYTC.entryCmn.entryFlags.get(unit,ent,&entryFlags) ;
                    entryFlags |= _BCM_DPP_FIELD_ENTRY_CHANGED ;
                    result |= FIELD_ACCESS_ENTRYTC.entryCmn.entryFlags.set(unit,ent,entryFlags) ;
                    /* the group contains changed entry/entries */
                    /*
                     * Was:
                     *   FIELD_ACCESS.groupD.groupFlags.get(unit, common->entryGroup, &groupD_groupFlags);
                     *   groupD_groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
                     *   FIELD_ACCESS.groupD.groupFlags.set(unit, common->entryGroup, groupD_groupFlags);
                     */
                    result |= FIELD_ACCESS.groupD.groupFlags.get(unit, entryGroup, &groupD_groupFlags);
                    groupD_groupFlags |= _BCM_DPP_FIELD_GROUP_CHG_ENT;
                    result |= FIELD_ACCESS.groupD.groupFlags.set(unit, entryGroup, groupD_groupFlags);
                }
            }
        } /* if (changed) */
        /* 'normal' entries are manipulated in cache, so no other cleanup */
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /* presel entries are not cached, so clean up from faking it */
        index = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(entry)) {
            bcm_stage = _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(entry);
            is_second_pass = _BCM_DPP_FIELD_ENTRY_IS_PRESEL_STAGGERED(entry);
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit,entry, bcm_stage, is_second_pass, &index, &stage)); 
        }
        if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_STAGGERED(entry))
        {
            flags |= _BCM_DPP_PRESEL_ALLOC_SECOND_PASS;
        }
        
        if (qualData) {
            if (changed) {
                result = _bcm_dpp_field_presel_info_set(unitData,
                                                        index,
                                                        SOC_PPC_FP_NOF_QUALS_PER_DB_MAX,
                                                        flags,
                                                        stage,
                                                        qualData,
                                                        0);
                if (BCM_E_NONE != result) {
                    LOG_ERROR(BSL_LS_BCM_FP,
                              (BSL_META_U(unit,
                                          "unit %d preselector %d write after"
                                          " changes failed: %d (%s)\n"),
                               unit,
                               entry,
                               result,
                               _SHR_ERRMSG(result)));
                }
            } /* if (changed) */
            sal_free(qualData);
        }
    }
exit:
    /* final else omitted because it should not matter -- prep errored out */
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_qualifier_set_single(bcm_dpp_field_info_OLD_t *unitData,
                                    bcm_field_entry_t entry,
                                    SOC_PPC_FP_QUAL_TYPE hwType,
                                    const uint64 data,
                                    const uint64 mask,
                                    const uint64 valid)
{
    _bcm_dpp_field_qual_t *qualData;
    SHR_BITDCL pqset[_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES)];
    _bcm_dpp_field_stage_idx_t stage = 0;
    _bcm_dpp_field_grp_idx_t entryGroup;
    unsigned int qualLimit = 0;
    int changed = FALSE;
    int result = 0;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    SHR_BITCLR_RANGE( pqset, 0, (_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES) * SHR_BITWID));

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              &entryGroup));

    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        /*  *pqset = &(groupData->pqset);*/
        FIELD_ACCESS.groupD.pqset.bit_range_read(unit, entryGroup, 0, 0, _SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES) * (8 * sizeof(int)/*32bits in int*/), pqset);
     } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /*  *pqset = &(unitData->stageD[*stage].ppqset);*/
        FIELD_ACCESS.stageD.ppqset.bit_range_read(unit, stage, 0, 0, _SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES) * (8 * sizeof(int)/*32bits in int*/), pqset);
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG("unit %d entry %d stage %d is not valid"),
                          unit,
                          entry,
                          stage));
    }

    if (!SHR_BITGET(pqset, hwType)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG("unit %d entry %d stage %d entryGroup %d hardware"
                                           " qualifier set does"
                                           " not include %d (%s)\n"),
                          unit,
                          entry,
                          stage,
						  entryGroup,
                          hwType,
                          SOC_PPC_FP_QUAL_TYPE_to_string(hwType)));
    } /* if (!SHR_BITGET(*pqset, hwType)) */
    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry) && _BCM_DPP_FIELD_ENT_IS_DIR_EXT(unit, entry)) {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set_single_int_new(unitData,
                                                                            entry,
                                                                            BCM_FIELD_ENTRY_INVALID,
                                                                            qualLimit,
                                                                            hwType,
                                                                            data,
                                                                            mask,
                                                                            valid));
    } else {
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set_single_int(unitData,
                                                                        entry,
                                                                        qualData,
                                                                        BCM_FIELD_ENTRY_INVALID,
                                                                        qualLimit,
                                                                        hwType,
                                                                        data,
                                                                        mask,
                                                                        valid));
    }

    /* if we get to here, the entry was changed */
    changed = TRUE;
exit:
    result = _bcm_dpp_field_qualifier_set_cleanup(unitData,
                                                  entry,
                                                  qualData,
                                                  stage,
                                                  changed);
    if (BCM_E_NONE != result) {
        _rv = result;
    }
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_dpp_field_qualifier_set_int
 *  Purpose
 *     Set a qualifier to an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) stage = stage in which the entry exists
 *     (in/out) qualData = pointer to the entry qualifier data
 *     (in) qualLimit = max qualifiers for this type of entry
 *     (in) bcmQual = BCM qualifier type
 *     (in) ppdQual = pointer to appropriate PPD qualifier chain
 *     (in) count = number of elements in qualifier data/mask arrays
 *     (in) data = pointer to array of qualifier data
 *     (in) mask = pointer to array of qualifier mask
 *     (out) changed = poitner to where to put the 'changed' indicator
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     If the qualifier mask and data are zero, and the qualifier exists, will
 *     remove the qualifier from the entry.
 *
 *     If the qualifier mask or data is nonzero, and the qualifier exists, this
 *     will update the mask and data values for the qualifier.  If the
 *     qualifier mask is nonzero and the qualifier does not exist, this will
 *     add the qualifier.
 *
 *     Does not return error at bits being set in data that are not set in
 *     mask, but it does emit a warning.  It does return BCM_E_PARAM at bits
 *     being in mask or data that are not valid for the qualifier.
 *
 *     Caller must already hold the unit lock.
 *
 *     This manipulates BCM layer qualifiers, so will shift left if needed
 *     (padding with zeroes) to align BCM LSb to appropriate bit of PPD, plus
 *     it supports complex BCM qualifiers (those made of more than one PPD
 *     qualifier)  by slicing the bits into appropriate parts for use as PPD
 *     layer qualifiers.
 *
 *     Bits added to the LSb end to move a qualifier left before applying it
 *     are not considered significant, but all bits from the least significant
 *     bit of the argument and going leftward are significant, even if they are
 *     not included (so if setting a qualifier that requires array of two and
 *     only one is provided, it is as if providing data=0 and mask=0 for the
 *     missing upper element of the array).
 *
 *     The array is little-endian with 64b grains: least significant octbyte is
 *     array[0], then next more significant is array[1], and so on.  Within
 *     array elements, the data are in machine-native order.
 */
STATIC int
_bcm_dpp_field_qualifier_set_int(bcm_dpp_field_info_OLD_t *unitData,
                                 bcm_field_entry_t entry,
                                 _bcm_dpp_field_stage_idx_t stage,
                                 _bcm_dpp_field_qual_t *qualData,
                                 unsigned int qualLimit,
                                 bcm_field_qualify_t bcmQual,
                                 const SOC_PPC_FP_QUAL_TYPE *ppdQual,
                                 int count,
                                 const uint64 *data,
                                 const uint64 *mask,
                                 int *changed)
{
    uint64 edata0, edata1;
    uint64 emask0, emask1;
    uint64 evalid0, evalid1;
    uint64 etemp;
    unsigned int *len = NULL;
    int index;
    int length;
    int bias;
    int dist;
    int shift;
    int offset;
    int result;
    int32 *qualMap ;
    int32 qualMaps_handle ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit; 

    result = BCM_E_NONE;
    *changed = FALSE;
    len = sal_alloc(sizeof(unsigned int)*unitData->devInfo->qualChain, "_bcm_dpp_field_qualifier_set_int");
    if (len == NULL) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                          (_BSL_BCM_MSG_NO_UNIT("Allocation failed")));

    }
    /* collect the lengths for all of the PPD qualifiers in the chain */
    for (index = 0, length = 0;
         (BCM_E_NONE == result) &&
         (index < unitData->devInfo->qualChain);
         index++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[index])) {
            result = _bcm_dpp_ppd_qual_bits(unit,
                                            stage,
                                            ppdQual[index],
                                            &(len[index]),
                                            NULL,
                                            NULL);
            length += len[index];
        }
    }
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("was unable to query qualifier %d"
                                          " (%s) chain %d (PPD qual %d (%s))"
                                          " length for unit %d stage %u:"
                                          " %d (%s)"),
                         bcmQual,
                         _bcm_dpp_field_qual_name[bcmQual],
                         index,
                         ppdQual[index],
                         SOC_PPC_FP_QUAL_TYPE_to_string(ppdQual[index]),
                         unit,
                         stage,
                         result,
                         _SHR_ERRMSG(result)));
    /*
     *  If that worked, we have the PPD claimed length for each of the
     *  qualifiers in the chain that will be used for this BCM layer qualifier.
     *  We also have the total PPD qualifier chain length.
     *
     *  We want to verify the settings, though, and allow emulated and shifted
     *  and other features, to map the qualifiers in a more versatile manner.
     *  To this end, the table also specifies a length.  We will use that
     *  length to verify the arguments, unless it says to not do so or it says
     *  to specifically use the PPD layer length.
     *
     *  The intent is that the BCM layer will explicitly provide a length for
     *  almost all qualifiers.  This comes from the idea that we want the
     *  versatile mapping and some sanity checking as we go.
     *
     *  However, there are cases where we do not want to bother validating the
     *  arguments here.  Mostly these would be cases where the function
     *  immediately (or nearly so) up the call stack has already performed this
     *  validation, such as the case for a translated qualifier, where a
     *  qualifier is mapped from BCM layer constant values to PPD layer
     *  constant values (look for bcmFieldQualifyColor as an example).  This is
     *  achieved by setting zero as the BCM layer length in the table.
     *
     *  Another exception to using the BCM layer bit count is when that count
     *  will be inconsistent between stages, or can be adjusted by some kind of
     *  configuration setting.  An example of this is the chained key feature
     *  that allows data to be passed from one field group to another.  In this
     *  case, the validation is to be done using the PPD layer qualifier
     *  lengths instead of the constant from the table.  This is achieved by
     *  setting -1 as the BCM layer length in the table.
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, bcmQual, &qualMaps_handle)) ;
    SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_ELEMENT(unit,qualMaps_handle,qualMap) ;
    if (0 == qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* caller must ensure values fit within bit width limits */
        length = 0;
    } else if (0 < qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* use BCM layer width */
        length = qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH];
    } /* else just keep the computed length above */
    shift = qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_SHIFT];
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d entry %d qual %s (%d):"
                          " length %d bits, shift %d\n"),
               unit,
               entry,
               _bcm_dpp_field_qual_name[bcmQual],
               bcmQual,
               length,
               shift));
    /*
     *  Before starting on the commit of the new/updated qualifier, we need to
     *  make sure the data and mask are within the constraints of the API.
     *
     *  At this point, all of the data/mask bits are pushed far right, so we
     *  only need to count bits until we get to the end of the qualifier as the
     *  API expects, and then make sure the last octbyte does not contain any
     *  unexpected bits.
     *
     *  We only check this is the arguments should have come from the
     *  Application; for translated or otherwise special types, this is skipped.
     *
     *  In these cases, the function that provides the translated values must
     *  verify its inbound arguments and provide sensible data here.  We assume
     *  here that all of the provided bits matter, but will quietly discard
     *  excess bits (those not consumed by the hardware qualifiers) later.
     */
    if (length) {
        /* arguments directly from application; verify their sizes */
        if (shift > 0) {
            /* argument will be shifted right; bits below LSb are invalid */
            offset = shift;
        } else {
            /* argument will be shifted left or unshifted; LSb is bit zero */
            offset = 0;
        }
        for (index = 0, dist = 0; index < count; index++, dist += 64) {
            /* assume all bits are valid */
            COMPILER_64_SET(etemp, 0, 0);
            if ((dist + 63) < offset) {
                /* this octbyte is below the LSb */
                COMPILER_64_SET(etemp, ~0, ~0);
            } else if (dist < offset) {
                /* this octbyte contains bits below the LSb */
                COMPILER_64_SET(evalid0, 0, 1);
                COMPILER_64_SHL(evalid0, offset - dist);
                COMPILER_64_SUB_32(evalid0, 1);
                COMPILER_64_OR(etemp, evalid0);
            }
            if (dist >= (offset + length)) {
                /* this octbyte is above the MSb */
                COMPILER_64_SET(etemp, ~0, ~0);
            } else if ((dist + 64) > (offset + length)) {
                /* this octbyte contains bits above the MSb */
                COMPILER_64_SET(evalid0, ~0, ~0);
                COMPILER_64_SHL(evalid0, (offset + length) - dist);
                COMPILER_64_OR(etemp, evalid0);
            }
            /* at this point, etemp contains invalid bits for this octbyte */
            edata0 = data[index];
            emask0 = mask[index];
            COMPILER_64_AND(edata0, etemp);
            COMPILER_64_AND(emask0, etemp);
            if (!COMPILER_64_IS_ZERO(edata0)) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "max bits for qualifier %d (%s) is %d"
                                      " for unit %d entry %d; provided data"
                                      " value %d %08X%08X has extra bit(s)"
                                      " %08X%08X set\n"),
                           bcmQual,
                           _bcm_dpp_field_qual_name[bcmQual],
                           length,
                           unit,
                           entry,
                           index,
                           COMPILER_64_HI(data[index]),
                           COMPILER_64_LO(data[index]),
                           COMPILER_64_HI(edata0),
                           COMPILER_64_LO(edata0)));
            }
            if (!COMPILER_64_IS_ZERO(emask0)) {
                LOG_ERROR(BSL_LS_BCM_FP,
                          (BSL_META_U(unit,
                                      "max bits for qualifier %d (%s) is %d"
                                      " for unit %d entry %d; provided mask"
                                      " value %d %08X%08X has extra bit(s)"
                                      " %08X%08X set\n"),
                           bcmQual,
                           _bcm_dpp_field_qual_name[bcmQual],
                           length,
                           unit,
                           entry,
                           index,
                           COMPILER_64_HI(mask[index]),
                           COMPILER_64_LO(mask[index]),
                           COMPILER_64_HI(emask0),
                           COMPILER_64_LO(emask0)));
            }
            if (!COMPILER_64_IS_ZERO(edata0) || !COMPILER_64_IS_ZERO(emask0)) {
                BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
            }
        } /* for (index = 0, dist = 0; index < count; index++, dist += 64) */
    } else { /* if (length) */
        /* translated arguments; assume all bits matter for now */
        length = count * 64;
    } /* if (length) */
    /*
     *  The arguments did not contain bogus bits; can keep going...
     */
    for (index = 0, offset = -1, dist = -64; /* qual #0, source octbyte #-1 */
         (index < unitData->devInfo->qualChain) &&
         (BCM_E_NONE == result);
         /* increment inline */) {
        if (shift > (dist + 63)) {
            /* look at next source octbyte */
            dist += 64;
            offset++;
            continue;
        }
        if (!_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[index])) {
            break;
        }
        if (0 > offset) {
            COMPILER_64_ZERO(edata0);
            COMPILER_64_ZERO(emask0);
            COMPILER_64_ZERO(evalid0);
        } else if (offset < count) {
            edata0 = data[offset];
            emask0 = mask[offset];
            COMPILER_64_SET(evalid0, ~0, ~0);
        } else {
            COMPILER_64_ZERO(edata0);
            COMPILER_64_ZERO(emask0);
            COMPILER_64_SET(evalid0, ~0, ~0);
        }
        if (offset >= (count - 1)) {
            COMPILER_64_ZERO(edata1);
            COMPILER_64_ZERO(emask1);
            COMPILER_64_SET(evalid1, ~0, ~0);
        } else {
            edata1 = data[offset + 1];
            emask1 = mask[offset + 1];
            COMPILER_64_SET(evalid1, ~0, ~0);
        }
        bias = shift - dist;
        if (bias) {
            /* will need mask value since SHR is ASR not LSR */
            COMPILER_64_SET(etemp, ~0, ~0);
            COMPILER_64_SHL(etemp, 64 - bias);
            COMPILER_64_NOT(etemp);
            /* shift current octbyte right by the needed distance */
            COMPILER_64_SHR(edata0, bias);
            COMPILER_64_SHR(emask0, bias);
            COMPILER_64_SHR(evalid0, bias);
            /* mask off dragged 'sign' bit (despite being unsigned value) */
            COMPILER_64_AND(edata0, etemp);
            COMPILER_64_AND(emask0, etemp);
            COMPILER_64_AND(evalid0, etemp);
            /* shift next octbyte left by (64 - the needed distance) */
            COMPILER_64_SHL(edata1, 64 - bias);
            COMPILER_64_SHL(emask1, 64 - bias);
            COMPILER_64_SHL(evalid1, 64 - bias);
            /* merge the two octbytes */
            COMPILER_64_OR(edata0, edata1);
            COMPILER_64_OR(emask0, emask1);
            COMPILER_64_OR(evalid0, evalid1);
        } else {
            COMPILER_64_ZERO(etemp);
        }
        if (length < (64 - bias)) {
            /* remaining valid bits fit in this octbyte; mask off unwanted */
            COMPILER_64_NOT(etemp);
            COMPILER_64_SHL(etemp, length);
            COMPILER_64_NOT(etemp);
            COMPILER_64_AND(edata0, etemp);
            COMPILER_64_AND(emask0, etemp);
            COMPILER_64_AND(evalid0, etemp);
        }
        /* now adjust the base values to lie within this qualifier */
        if (64 > len[index]) {
            COMPILER_64_SET(etemp, ~0, ~0);
            COMPILER_64_SHL(etemp, len[index]);
            COMPILER_64_NOT(etemp);
            COMPILER_64_AND(edata0, etemp);
            COMPILER_64_AND(emask0, etemp);
            COMPILER_64_AND(evalid0, etemp);
        }
        if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry) && _BCM_DPP_FIELD_ENT_IS_DIR_EXT(unit, entry)) {
            result =_bcm_dpp_field_qualifier_set_single_int_new(unitData,
                                                         entry,
                                                         bcmQual,
                                                         qualLimit,
                                                         ppdQual[index],
                                                         edata0,
                                                         emask0,
                                                         evalid0);
        } else {
            result =_bcm_dpp_field_qualifier_set_single_int(unitData,
                                                         entry,
                                                         qualData,
                                                         bcmQual, 
                                                         qualLimit,
                                                         ppdQual[index],
                                                         edata0,
                                                         emask0,
                                                         evalid0);
        }

        if (BCM_E_NONE == result) {
            /* note that the entry has been changed */
            *changed = TRUE;
        } else {
            /* stop on errors */
            _rv = result;
            break;
        }
        /* remove this qualifier's bits from what we have to do */
        shift += len[index];
        length -= len[index];
        /* if LSb now in next octbyte, work from that point */
        if (((dist + len[index]) >> 6) != (dist >> 6)) {
            offset++;
            dist += 64;
        }
        /* move to next qualifier in chain */
        index++;
    } /* for (all qualifiers in chain as long as no error) */
    if (qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* only check this if non-translated arguments */
        if (length > 0) {
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "still have %d bits left over!\n"),
                      length));
        }
    }
exit:
    if (len) {
        sal_free(len);
    }
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_qualifier_set(bcm_dpp_field_info_OLD_t *unitData,
                             bcm_field_entry_t entry,
                             bcm_field_qualify_t bcmQual,
                             int count,
                             const uint64 *data,
                             const uint64 *mask)
{
    _bcm_dpp_field_qual_t *qualData;
    bcm_field_qset_t bqset;
    SOC_PPC_FP_QUAL_TYPE *ppdQual;
    _bcm_dpp_field_stage_idx_t stage = 0;
    _bcm_dpp_field_grp_idx_t entryGroup;
    unsigned int qualLimit;
    uint32 groupTypes;
    uint32 groupFlags;
    int changed = FALSE;
    int result;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    sal_memset( &bqset, 0, sizeof(bqset));

    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              &entryGroup));
    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {

            /*  *bqset = &(groupData->qset);*/

            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.qset.get(unit, entryGroup, &bqset));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupTypes.get(unit, entryGroup, &groupTypes));
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
 /*            *bqset = &(unitData->stageD[*stage].qsetType[_BCM_DPP_FIELD_MAX_GROUP_TYPES]);*/
            FIELD_ACCESS.stageD.qsetType.get(unit, stage, _BCM_DPP_FIELD_MAX_GROUP_TYPES, &bqset);
            groupTypes = 1 << _BCM_DPP_FIELD_MAX_GROUP_TYPES;
    }

    if ((bcmFieldQualifyCount > bcmQual) && (BCM_FIELD_ENTRY_INVALID !=  bcmQual)) {
        uint8 bit_value ;

        /* standard BCM qualifier */
        /*
         * Was:
         *   if (!BCM_FIELD_QSET_TEST(unitData->unitQset, bcmQual))
         * Which was equivalent to:
         *   if (!SHR_BITGET((unitData->unitQset.w), bcmQual) 
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_UNITQSET_W.bit_get(unit,bcmQual,&bit_value)) ;
        if (!bit_value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " qualifier %d (%s)"),
                              unit,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        }
        if (!BCM_FIELD_QSET_TEST(bqset, bcmQual)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d QSET does not"
                                               " include %d (%s)"),
                              unit,
                              entry,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        } /* if (!BCM_FIELD_QSET_TEST(gropData->qset, type)) */
    } else { /* if (bcmFieldQualifyCount > type) || BCM_FIELD_ENTRY_INVALID */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("invalid BCM qualifier type %d"),
                          bcmQual));
    } /* if (bcmFieldQualifyCount > type) */
    /* get the qualifier mapping */
    result = _bcm_dpp_ppd_qual_from_bcm_qual(unitData,
                                             stage,
                                             groupTypes,
                                             bcmQual,
                                             &ppdQual);
    if(_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry))
    {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupFlags.get(unit, entryGroup, &groupFlags));

        if ((bcmQual == bcmFieldQualifyOuterVlanId) && (stage == _BCM_DPP_FIELD_STAGE_INDEX_INGRESS) ) 
        {
            *ppdQual = ( groupFlags & _BCM_DPP_FIELD_GROUP_VT_CLASSIFICATION ) ? SOC_PPC_FP_QUAL_INITIAL_VID : SOC_PPC_FP_QUAL_HDR_VLAN_TAG_ID ; 
        }
    }  
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("was unable to map qualifier %d (%s)"
                                          " to PPD for unit %d entry %d (stage"
                                          " %u types %08X: %d (%s)"),
                         bcmQual,
                         _bcm_dpp_field_qual_name[bcmQual],
                         unit,
                         entry,
                         stage,
                         groupTypes,
                         result,
                         _SHR_ERRMSG(result)));
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set_int(unitData,
                                                             entry,
                                                             stage,
                                                             qualData,
                                                             qualLimit,
                                                             bcmQual,
                                                             ppdQual,
                                                             count,
                                                             data,
                                                             mask,
                                                             &changed));

exit:
    result = _bcm_dpp_field_qualifier_set_cleanup(unitData,
                                                  entry,
                                                  qualData,
                                                  stage,
                                                  changed);
    if (BCM_E_NONE != result) {
        _rv = result;
    }
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_entry_qualifier_general_set_int(bcm_dpp_field_info_OLD_t *unitData,
                                               bcm_field_entry_t entry,
                                               bcm_field_qualify_t bcmQual,
                                               int count,
                                               const uint64 *data,
                                               const uint64 *mask)
{
    bcm_field_presel_t presel;
    int32 qualMaps_handle ;
    bcm_field_stage_t stage;
    uint32 flags = 0;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    
    if ((0 > bcmQual) || (bcmFieldQualifyCount <= bcmQual)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier type %d not valid"),
                          bcmQual));
    }
    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /*
         *  There are special cases for preselectors (bcmFieldQualifyStage;
         *  and maybe others), so
         *  give the preselector code a chance to filter the request.  If the
         *  device supports the qualifier as a 'special' case, it will be
         *  handled by _bcm_petra_field_presel_qualify, but if it should be a
         *  'general' case, _bcm_petra_field_presel_qualify will in turn call
         *  _bcm_dpp_field_qualifier_set to do the actual work.
         */
        presel = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        stage =  bcmFieldStageCount;
        if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(entry)) 
        {
            stage =  _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(entry);
            flags =  _BCM_DPP_PRESEL_ALLOC_WITH_STAGE;
            if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_STAGGERED(entry)) 
            {
                flags |= _BCM_DPP_PRESEL_ALLOC_SECOND_PASS;
            }
        }
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_qualify(unitData,
                                                              presel,
                                                              stage, 
                                                              flags,
                                                              bcmQual,
                                                              count,
                                                              data,
                                                              mask));
    } else {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, bcmQual, &qualMaps_handle)) ;
        if (qualMaps_handle) {
            /* this unit supports this qualifier */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_set(unitData,
                                                                 entry,
                                                                 bcmQual,
                                                                 count,
                                                                 data,
                                                                 mask));
        } else { /* if (qualMaps_handle) */
            /* this unit does not support this qualifier */
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               "qualifier %d (%s)"),
                              unit,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        } /* if (qualMaps_handle) */
    }
exit:
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_get_single_int
 *  Purpose
 *    Get a single qualifier on an entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
 *    (in) qual = pointer to qualifier array for this entry
 *    (in) qualLimit = max qualifier index applicable to this entry
 *    (in) hwType = PPD layer qualifier to add to the entry
 *    (in) qualHwData = pointer to hardware qualifier data
 *    (out) data = where to put data value
 *    (out) mask = where to put mask value
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Caller must already hold the unit lock.
 *
 *    Assumes entry exists in a group.
 *
 *    Caller can choose what bits are desired after return.
 */
STATIC int
_bcm_dpp_field_qualifier_get_single_int(bcm_dpp_field_info_OLD_t *unitData,
                                        bcm_field_entry_t entry,
                                        const _bcm_dpp_field_qual_t *qual,
#if _BCM_DPP_FIELD_GET_USE_PPD
                                        const SOC_PPC_FP_QUAL_VAL *qualHwData,
#endif /* _BCM_DPP_FIELD_GET_USE_PPD */
                                        unsigned int qualLimit,
                                        SOC_PPC_FP_QUAL_TYPE hwType,
                                        uint64 *data,
                                        uint64 *mask)
{
    unsigned int index;
    bcm_field_qualify_t ppdQual;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.ppdQual.get(unit, hwType, &ppdQual));
#if _BCM_DPP_FIELD_GET_USE_PPD
    if (qualHwData) {
        /* using the hardware view */
        for (index = 0;
             (index < qualLimit) && (qualHwData[index].type != hwType);
             index++) {
            /* look until scanned all or hit requested qual */
        }
        if (index < qualLimit) {
            COMPILER_64_SET(*data, qualHwData[index].val.arr[1],
                                     qualHwData[index].val.arr[0]);
            COMPILER_64_SET(*mask, qualHwData[index].is_valid.arr[1],
                                     qualHwData[index].is_valid.arr[0]);
        }
    } else { /* if (qualHwData) */
        /* using the cached state */
#endif /* _BCM_DPP_FIELD_GET_USE_PPD */

    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        int32 commonHandle ;
        bcm_field_entry_t ent;
        _bcm_dpp_field_entry_type_t entryType;
        int isExternalTcam ;
        _bcm_dpp_field_qual_t qual_at_index ;

        /* 'normal' entries are cached and so we use the cache */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_common_pointer(unitData,
                                                                    entry,
                                                                    &entryType,&commonHandle));
        isExternalTcam = _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType) ;
        switch (entryType) {
            case _bcmDppFieldEntryTypeInternalTcam:
            case _bcmDppFieldEntryTypeExternalTcam:
            {
                /*
                 * Original in _bcm_dpp_field_qualifier_get_prep() was:
                 *   *qualData = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
                 *                                                    _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
                 *
                 * The rule for changing into new sw state: qual[index] ==> qual_at_index
                 */
                _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), 0, qual_at_index) ;
                for (index = 0;
                     (index < qualLimit) && (qual_at_index.hwType != hwType);
                     index++) {
                    /* look until scanned all or hit requested qual */
                    if ((index + 1) < qualLimit) {
                        _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), (index + 1), qual_at_index) ;
                    }
                } /* for (all possible qualifier slots this stage) */
                if (index < qualLimit) {
					_BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), index, qual_at_index) ;
                    *data = qual_at_index.qualData;
                    *mask = qual_at_index.qualMask;
                }
            }
            break;
            case _bcmDppFieldEntryTypeDirExt:
            {
                /*
                 * Original in _bcm_dpp_field_qualifier_get_prep() was:
                 *   qualData = &((common)->entryQual[0]);
                 * Where
                 *   common = &(unitData->entryDe[entry - _BCM_DPP_FIELD_ENT_BIAS(unit, DirExt)].entryCmn);
                 */
                ent = commonHandle - 1;
                BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.get(unit, ent, 0, &qual_at_index)) ;
                for (index = 0;
                     (index < qualLimit) && (qual_at_index.hwType != hwType);
                     index++) {
                    /* look until scanned all or hit requested qual */
                    if ((index + 1) < qualLimit) {
                        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryQual.get(unit, ent, (index + 1), &qual_at_index));
                    }
                } /* for (all possible qualifier slots this stage) */
                if (index < qualLimit) {
                    _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), index, qual_at_index) ;
                    *data = qual_at_index.qualData;
                    *mask = qual_at_index.qualMask;
                }
            }
            break;
            default:
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d unknown"
                                               " type %d"),
                              unit,
                              entry,
                              entryType));
            }
        } /* switch (entryType) */
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        for (index = 0;
             (index < qualLimit) && (qual[index].hwType != hwType);
             index++) {
            /* look until scanned all or hit requested qual */
        } /* for (all possible qualifier slots this stage) */
        if (index < qualLimit) {
            *data = qual[index].qualData;
            *mask = qual[index].qualMask;
        }
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not valid"),
                          unit,
                          entry));
    }

#if _BCM_DPP_FIELD_GET_USE_PPD
    } /* if (qualHwData) */
#endif /* _BCM_DPP_FIELD_GET_USE_PPD */
    if (index >= qualLimit) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                         (BSL_META_U(unit,
                                     "unit %d entry %d does not have"
                                           " hw qualifier %d (%s)"),
                          unit,
                          entry,
                          hwType,
                          SOC_PPC_FP_QUAL_TYPE_to_string(hwType)));
    }
exit:
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "(%d,%d,%d;%s(%d;%s),&(%08X%08X),&(%08X%08X)):"
                          " %d (%s)\n"),
               unit,
               entry,
               hwType,
               SOC_PPC_FP_QUAL_TYPE_to_string(hwType),
               ppdQual,
               (BCM_FIELD_ENTRY_INVALID != ppdQual) ? _bcm_dpp_field_qual_name[ppdQual] : "-",
               COMPILER_64_HI(*data),
               COMPILER_64_LO(*data),
               COMPILER_64_HI(*mask),
               COMPILER_64_LO(*mask),
               _rv,
               _SHR_ERRMSG(_rv)));
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_get_prep
 *  Purpose
 *    Get (and check) hardware values for qualifiers on an entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
 *    (out) qualData = where to put pointer to qual data array
 *    (out) qualLimit = where to put max quals in qual data array
 *    (out) stage = where to put the applicable stage ID
 *                    If NULL then do not apply this output.
 *    (out) bqset = where to put a pointer to the applicable BCM QSET
 *                    If NULL then do not apply this output.
 *    (out) pqset = where to put a pointer to the applicable PPD QSET
 *                    If NULL then do not apply this output.
 *    (out) groupTypes = where to put the group types bitmap
 *                    If NULL then do not apply this output.
 *    (out) qualHwData = where to put hardware qualifier data
 *    (out) useData = where to put boolean indicating whether to use hw quals
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE if success
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    Assumes entry is valid, lock is held.
 *
 *    The value at useData is TRUE if and only if the entry is valid, in
 *    hardware, and has not been updated since it was committed to hardware.
 */

STATIC int
_bcm_dpp_field_qualifier_get_prep(bcm_dpp_field_info_OLD_t *unitData,
                                  bcm_field_entry_t entry,
                                  _bcm_dpp_field_qual_t **qualData,
                                  unsigned int *qualLimit,
                                  _bcm_dpp_field_stage_idx_t *stage,
                                  _bcm_dpp_field_grp_idx_t *entryGroup
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
                                  ,SOC_PPC_FP_QUAL_VAL *qualHwData,
                                  int *useData
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
                                  )
{
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    SOC_PPC_FP_ENTRY_INFO *entInfoTc = NULL;
    SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *entInfoDe = NULL;
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    _bcm_dpp_field_entry_type_t entryType;
    uint32 entryFlags;
    _bcm_dpp_field_presel_idx_t preselLimit;
    uint8 bit_value ;
    uint8 is_second_pass, is_found;
    _bcm_dpp_field_stage_idx_t tmp_stage;
    bcm_field_stage_t bcm_stage;
    int index, result;
    uint32 soc_sand_rv;
    uint32 presel_flags = 0;
    bcm_field_entry_t ent;
    int isExternalTcam ;
    _bcm_dpp_field_ent_idx_t new_entry;
    _bcm_dpp_field_grp_idx_t localEntryGroup;        /* this entry's group */
    _bcm_dpp_field_grp_idx_t groupLimit;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.preselLimit.get(unit, &preselLimit)) ;
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupLimit.get(unit, &groupLimit));

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    if (_BCM_DPP_FIELD_ENT_IS_TCAM(unit, entry) || _BCM_DPP_FIELD_ENTRY_IS_LARGE_DIRECT_LOOKUP(entry)) {
        BCMDNX_ALLOC(entInfoTc, sizeof(SOC_PPC_FP_DIR_EXTR_ENTRY_INFO), "TCAM entry temp");
        if (entInfoTc == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
        }
    }
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */

    if (_BCM_DPP_FIELD_ENTRY_IS_LARGE_DIRECT_LOOKUP(entry)) {
        BCM_FIELD_LARGE_DIRECT_ENTRY_ID_UNPACK(entry, *entryGroup, new_entry);
        if (groupLimit <= *entryGroup) {
            BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM,
                             (_BSL_BCM_MSG("Error: entry doesn't belong to any group")));
        }
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, *entryGroup, &tmp_stage));
        *qualLimit = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, tmp_stage).entryMaxQuals;
        *stage = tmp_stage;
        *qualData = NULL;

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
        entInfoTc->large_direct_lookup_key = new_entry;
        entInfoTc->large_direct_lookup_mask = 0;
        soc_sand_rv = soc_ppd_fp_entry_get(unit,
                                *entryGroup,
                                new_entry,
                                &is_found,
                                entInfoTc);

         result = handle_sand_result(soc_sand_rv);
         BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("unit %d unable to fetch"
                                          " entry %u hardware"
                                          " information (%u):"
                                          " %d (%s)\n"),
                         unit,
                         entry,
                         new_entry,
                         result,
                         _SHR_ERRMSG(result)));
         if (is_found == TRUE) {
             sal_memcpy(qualHwData,
               &(entInfoTc->qual_vals[0]),
               sizeof(*qualHwData) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
             *useData = TRUE;
         } else {
             *useData = FALSE;
         }
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    } else if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        int32 commonHandle ;
        /* 'normal' entries are cached and so we use the cache */
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_entry_common_pointer(unitData,
                                                                    entry,
                                                                    &entryType,&commonHandle));
        ent = commonHandle - 1;
        switch (entryType) {
        case _bcmDppFieldEntryTypeInternalTcam:
        case _bcmDppFieldEntryTypeExternalTcam:
            /*
             * Was:
             *   *entryGroup = _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType),
             *                    _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
             *   entryFlags = _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
             *                    _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
             */
            isExternalTcam = _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType) ;
            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), localEntryGroup)) ;
            *entryGroup = localEntryGroup ;
            if (groupLimit <= *entryGroup) {
                BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: entry doesn't belong to any group")));
            }

            BCMDNX_IF_ERR_EXIT(_BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,isExternalTcam, _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry), entryFlags)) ;

            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, *entryGroup, &tmp_stage));
            *qualLimit = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, tmp_stage).entryMaxQuals;
            /*
             * Was:
             *   *qualData = _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(_BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(entryType), 
             *                                            _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, entry));
             * See explanation right below.
             */
            /*
             * The only user of this piece of data (qualData) for _bcmDppFieldEntryTypeInternalTcam/_bcmDppFieldEntryTypeExternalTcam
             * is  _bcm_dpp_field_qualifier_get_single_int()  (See 'paths' below) so move it there and
             * eliminate usage of 'qualData' (Since it is a pointer, it presents a problem for new SW state.
             * Paths:
             * A.
             *   _bcm_dpp_field_qualifier_get_single()
             *     _bcm_dpp_field_qualifier_get_prep()
             *     _bcm_dpp_field_qualifier_get_single_int()
             * B.
             *   _bcm_dpp_field_qualifier_get()
             *     _bcm_dpp_field_qualifier_get_prep()
             *     _bcm_dpp_field_qualifier_get_int()
             *     _bcm_dpp_field_qualifier_get_single_int()
             * Note:
             *   _bcm_dpp_field_qualifier_set_cleanup() is also a user of 'qualData' but only for 'PRESEL'.
             */
            break;
        case _bcmDppFieldEntryTypeDirExt:
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryGroup.get(unit, ent, entryGroup));
            if (groupLimit <= *entryGroup) {
                BCMDNX_ERR_EXIT_VERB_MSG(BCM_E_PARAM,
                                 (_BSL_BCM_MSG("Error: entry doesn't belong to any group")));
            }
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.entryDe.entryCmn.entryFlags.get(unit, ent, &entryFlags));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, *entryGroup, &tmp_stage));
            *qualLimit = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, tmp_stage).entryDeMaxQuals;
            /* *qualData = &(common->entryQual[0]);*/
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
            BCMDNX_ALLOC(entInfoDe, sizeof(SOC_PPC_FP_DIR_EXTR_ENTRY_INFO), "Dir Ext entry temp");
            if (entInfoDe == NULL) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG_NO_UNIT("failed to allocate memory")));
            }
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d unknown"
                                               " type %d"),
                              unit,
                              entry,
                              entryType));
        } /* switch (entryType) */
        if (0 == (entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d not in use"),
                              unit,
                              entry));
        } /* if (0 == (common->entryFlags & _BCM_DPP_FIELD_ENTRY_IN_USE)) */
        if (stage) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.stage.get(unit, *entryGroup, &tmp_stage));
            *stage = tmp_stage;
        }
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
        *useData = FALSE;
        if ((entryFlags & (_BCM_DPP_FIELD_ENTRY_IN_HW |
                                   _BCM_DPP_FIELD_ENTRY_CHANGED |
                                   _BCM_DPP_FIELD_ENTRY_NEW)) ==
             (_BCM_DPP_FIELD_ENTRY_IN_HW)) {
            if (entInfoTc) {
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_tcam_hardware_entry_check(unitData,
                                                                                 entry,
                                                                                 entInfoTc));
                sal_memcpy(qualHwData,
                           &(entInfoTc->qual_vals[0]),
                           sizeof(*qualHwData) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                *useData = TRUE;
            }
            if (entInfoDe) {
                BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_dir_ext_hardware_entry_check(unitData,
                                                                                    entry,
                                                                                    entInfoDe));
                sal_memcpy(qualHwData,
                           &(entInfoDe->qual_vals[0]),
                           sizeof(*qualHwData) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX);
                *useData = TRUE;
            }
        } /* if (entry is in hardware, and not changed, and not new) */
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
        *useData = FALSE;
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
        /* preselectors are not cached, so need to fake it for them */
        *qualData = NULL;
        *qualLimit = SOC_PPC_FP_NOF_QUALS_PER_DB_MAX;
        index = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry) ;
        if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(entry)) {
            bcm_stage =   _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(entry) ;
            is_second_pass = _BCM_DPP_FIELD_ENTRY_IS_PRESEL_STAGGERED(entry);
            presel_flags =  _BCM_DPP_PRESEL_ALLOC_WITH_STAGE; 
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_to_bitmap(unit,entry, bcm_stage, is_second_pass, &index, stage)); 
        }
          
        if ((0 > index) || (preselLimit <= index)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %d invalid"),
                              unit,
                              index));
        }
        /*
         * Was:
         *   if (!BCM_FIELD_PRESEL_TEST(unitData->preselInUse, entry))
         * Which was equivalent to:
         *   if (!SHR_BITGET(unitData->preselInUse.w, entry))
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_PRESELINUSE_W.bit_get(unit, index, &bit_value)) ;
        if (!bit_value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d presel %d not in use"),
                              unit,
                              entry));
        }
        *qualData = sal_alloc(sizeof(**qualData) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX,
                              "preselector 'set' working space");
        if (!(*qualData)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d unable to allocate %u"
                                               " bytes presel workspace"),
                              unit,
                              (uint32)sizeof(*qualData) * SOC_PPC_FP_NOF_QUALS_PER_DB_MAX));
        }
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_info_get(unitData,
                                                               index,
                                                               *qualLimit,
                                                               presel_flags,
                                                               stage,
                                                               *qualData));
    } else {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d is not valid"),
                          unit,
                          entry));
    }
exit:
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    BCM_FREE(entInfoTc);
    BCM_FREE(entInfoDe);
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *    _bcm_dpp_field_qualifier_get_cleanup
 *  Purpose
 *    Perform any work that needs to be done after reading the qualifiers on an
 *    entry or preselector or whatever else
 *  Arguments
 *    (in) unitData = pointer to unit data
 *    (in) entry = BCM layer entry ID
 *    (in) qualData = new qualifier data array
 *  Returns
 *    nothing
 */
STATIC void
_bcm_dpp_field_qualifier_get_cleanup(bcm_dpp_field_info_OLD_t *unitData,
                                     bcm_field_entry_t entry,
                                     _bcm_dpp_field_qual_t *qualData)
{
    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
        /* 'normal' entries are manipulated in cache, so no other cleanup */
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /* presel entries are not cached, so clean up from faking it */
        entry = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        if (qualData) {
            sal_free(qualData);
        }
    }
    /* final else omitted because it should not matter -- prep errored out */
}

int
_bcm_dpp_field_qualifier_get_single(bcm_dpp_field_info_OLD_t *unitData,
                                    bcm_field_entry_t entry,
                                    SOC_PPC_FP_QUAL_TYPE hwType,
                                    uint64 *data,
                                    uint64 *mask)
{
    _bcm_dpp_field_qual_t *qualData;
    SHR_BITDCL pqset[_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES)];
    unsigned int qualLimit = 0;
    _bcm_dpp_field_stage_idx_t stage = 0;
    _bcm_dpp_field_grp_idx_t entryGroup = 0;
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    SOC_PPC_FP_QUAL_VAL qualHwData[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int useData;
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    SHR_BITCLR_RANGE( pqset, 0, (_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES) * SHR_BITWID));

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              &entryGroup,
                                                              &(qualHwData[0]),
                                                              &useData));
#else /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              &entryGroup));
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */

    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.pqset.bit_range_read(unit, entryGroup, 0, 0, _SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES) * (8 * sizeof(int))/*32bits in int*/, pqset));
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
/*            *pqset = &(unitData->stageD[*stage].ppqset);*/
            FIELD_ACCESS.stageD.ppqset.bit_range_read(unit, stage, 0, 0, _SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES) * (8 * sizeof(int)/*32bits in int*/), pqset);
    }


    if (!SHR_BITGET(pqset, hwType)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                         (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d hardware"
                                           " qualifier set does"
                                           " not include %d (%s)"),
                          unit,
                          entry,
                          hwType,
                          SOC_PPC_FP_QUAL_TYPE_to_string(hwType)));
    } /* if (!SHR_BITGET(*pqset, hwType)) */
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_single_int(unitData,
                                                                    entry,
                                                                    qualData,
                                                                    useData?(&(qualHwData[0])):NULL,
                                                                    qualLimit,
                                                                    hwType,
                                                                    data,
                                                                    mask));
#else /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_single_int(unitData,
                                                                    entry,
                                                                    qualData,
                                                                    qualLimit,
                                                                    hwType,
                                                                    data,
                                                                    mask));
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
exit:
    _bcm_dpp_field_qualifier_get_cleanup(unitData,
                                         entry,
                                         qualData);
    BCMDNX_FUNC_RETURN;
}

/*
 *  Function
 *     _bcm_dpp_field_qualifier_get_int
 *  Purpose
 *     Get a qualifier from an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) stage = stage in which the entry exists
 *     (in) qualData = pointer to the entry qualifier data
 *     (in) qualLimit = max qualifiers for this type of entry
 *     (in) bcmQual = BCM qualifier type
 *     (in) ppdQual = pointer to appropriate PPD qualifier chain
 *     (in) count = number of elements in qualifier data/mask arrays
 *     (in) data = pointer to array of qualifier data
 *     (in) mask = pointer to array of qualifier mask
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Caller must already hold the unit lock.
 *
 *     This manipulates BCM layer qualifiers, so will shift left if needed
 *     (padding with zeroes) to align BCM LSb to appropriate bit of PPD, plus
 *     it supports complex BCM qualifiers (those made of more than one PPD
 *     qualifier)  by slicing the bits into appropriate parts for use as PPD
 *     layer qualifiers.
 *
 *     Bits added to the LSb end to move a qualifier left before applying it
 *     are not considered significant, but all bits from the least significant
 *     bit of the argument and going leftward are significant, even if they are
 *     not included (so if setting a qualifier that requires array of two and
 *     only one is provided, it is as if providing data=0 and mask=0 for the
 *     missing upper element of the array).
 *
 *     The array is little-endian with 64b grains: least significant octbyte is
 *     array[0], then next more significant is array[1], and so on.  Within
 *     array elements, the data are in machine-native order.
 */
STATIC int
_bcm_dpp_field_qualifier_get_int(bcm_dpp_field_info_OLD_t *unitData,
                                 bcm_field_entry_t entry,
                                 _bcm_dpp_field_stage_idx_t stage,
                                 _bcm_dpp_field_qual_t *qualData,
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
                                 SOC_PPC_FP_QUAL_VAL *qualHwData,
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
                                 unsigned int qualLimit,
                                 bcm_field_qualify_t bcmQual,
                                 const SOC_PPC_FP_QUAL_TYPE *ppdQual,
                                 int count,
                                 uint64 *data,
                                 uint64 *mask)
{
    uint64 edata0, edata1;
    uint64 emask0, emask1;
    uint64 etemp;
    unsigned int *len = NULL;
    unsigned int index;
    unsigned int offset;
    signed int length;
    signed int shift;
    signed int over;
    int result = BCM_E_NONE;
    int found;
    int32 *qualMap ;
    int32 qualMaps_handle ;

    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    COMPILER_64_ZERO(edata0);
    COMPILER_64_ZERO(emask0);

    len = sal_alloc(sizeof(unsigned int)*unitData->devInfo->qualChain, "_bcm_dpp_field_qualifier_get_int");
    if (len == NULL) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY,
                          (_BSL_BCM_MSG_NO_UNIT("Alocation failed")));

    }

    
    for (index = 0; index < count; index ++) {
        COMPILER_64_ZERO(data[index]);
        COMPILER_64_ZERO(mask[index]);
    }
    /* collect the lengths for all of the PPD qualifiers in the chain */
    for (index = 0, length = 0;
         (BCM_E_NONE == result) &&
         (index < unitData->devInfo->qualChain);
         index++) {
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[index])) {
            result = _bcm_dpp_ppd_qual_bits(unit,
                                            stage,
                                            ppdQual[index],
                                            &(len[index]),
                                            NULL,
                                            NULL);
            length += len[index];
        }
    }
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("was unable to query qualifier %d"
                                          " (%s) chain %d (PPD qual %d (%s))"
                                          " length for unit %d stage %u:"
                                          " %d (%s)"),
                         bcmQual,
                         _bcm_dpp_field_qual_name[bcmQual],
                         index,
                         ppdQual[index],
                          SOC_PPC_FP_QUAL_TYPE_to_string(ppdQual[index]),
                         unit,
                         stage,
                         result,
                         _SHR_ERRMSG(result)));
    /*
     *  If that worked, we have the PPD claimed length for each of the
     *  qualifiers in the chain that will be used for this BCM layer qualifier.
     *  We also have the total PPD qualifier chain length.
     *
     *  We want to verify the settings, though, and allow emulated and shifted
     *  and other features, to map the qualifiers in a more versatile manner.
     *  To this end, the table also specifies a length.  We will use that
     *  length to verify the arguments, unless it says to not do so or it says
     *  to specifically use the PPD layer length.
     *
     *  The intent is that the BCM layer will explicitly provide a length for
     *  almost all qualifiers.  This comes from the idea that we want the
     *  versatile mapping and some sanity checking as we go.
     *
     *  However, there are cases where we do not want to bother validating the
     *  arguments here.  Mostly these would be cases where the function
     *  immediately (or nearly so) up the call stack has already performed this
     *  validation, such as the case for a translated qualifier, where a
     *  qualifier is mapped from BCM layer constant values to PPD layer
     *  constant values (look for bcmFieldQualifyColor as an example).  This is
     *  achieved by setting zero as the BCM layer length in the table.
     *
     *  Another exception to using the BCM layer bit count is when that count
     *  will be inconsistent between stages, or can be adjusted by some kind of
     *  configuration setting.  An example of this is the chained key feature
     *  that allows data to be passed from one field group to another.  In this
     *  case, the validation is to be done using the PPD layer qualifier
     *  lengths instead of the constant from the table.  This is achieved by
     *  setting -1 as the BCM layer length in the table.
     */
    BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, bcmQual, &qualMaps_handle)) ;
    SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_ELEMENT(unit,qualMaps_handle,qualMap) ;
    if (0 == qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* assume worst case length */
        length = count * 64;
    } else if (0 < qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        /* use BCM layer width */
        length = qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH];
    } /* else (-1) just keep the computed length above */
    shift = qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_SHIFT];
    LOG_DEBUG(BSL_LS_BCM_FP,
              (BSL_META_U(unit,
                          "unit %d entry %d qual %s (%d):"
                          " length %d bits, shift %d\n"),
               unit,
               entry,
               _bcm_dpp_field_qual_name[bcmQual],
               bcmQual,
               length,
               shift));
    for (index = 0, offset = 0, found = 0;
         (index < unitData->devInfo->qualChain) &&
         (offset < count) &&
         (BCM_E_NONE == result);
         index++) {
        if (63 < shift) {
            /* skip to next octbyte of buffer (initial condition only) */
            COMPILER_64_ZERO(data[offset]);
            COMPILER_64_ZERO(mask[offset]);
            shift -= 64;
            
            /* SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH qualifier requires first 64 bit element of the set */
            if (ppdQual[index] != SOC_PPC_FP_QUAL_HDR_IPV6_SIP_HIGH && ppdQual[index] != SOC_PPC_FP_QUAL_HDR_IPV6_DIP_HIGH) {
                length -= 64;
                offset++;
                continue;
            }
        }
        if (_BCM_DPP_FIELD_PPD_QUAL_VALID(ppdQual[index])) {
            result = _bcm_dpp_field_qualifier_get_single_int(unitData,
                                                             entry,
                                                             qualData,
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
                                                             qualHwData,
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
                                                             qualLimit,
                                                             ppdQual[index],
                                                             &edata0,
                                                             &emask0);
            if (BCM_E_NOT_FOUND == result) {
                /* if not found, assume the qualifier is zero data, zero mask */
                COMPILER_64_ZERO(edata0);
                COMPILER_64_ZERO(emask0);
                result = BCM_E_NONE;
            } else if (BCM_E_NONE != result) {
                /* stop on any other error */
                break;
            } else {
                /* no error */
                found++;
            }
        } else {
            COMPILER_64_ZERO(edata0);
            COMPILER_64_ZERO(emask0);
            len[index] = 64;
        }
        edata1 = edata0;
        emask1 = emask0;
        if (0 > shift) {
            /* shifting right (initial condition only) */
            COMPILER_64_SHR(edata1, -shift);
            COMPILER_64_SHR(emask1, -shift);
            COMPILER_64_SET(etemp, ~0, ~0);
            /* may need to mask value since SHR is ASR not LSR */
            COMPILER_64_SHL(etemp, len[index]);
            COMPILER_64_NOT(etemp);
            COMPILER_64_AND(edata1, etemp);
            COMPILER_64_AND(emask1, etemp);
            /* set initial data and mask values */
            data[offset] = edata1;
            mask[offset] = emask1;
            /* adjust shift distance for next qualifier */
            shift += len[index];
        } else if (0 == shift) {
            /* first qualifier this octbyte of the buffer */
            data[offset] = edata1;
            mask[offset] = emask1;
            /* adjust shift distance for next qualifier */
            shift = len[index];
        } else /* (0 < shift) */ {
            /* shifting left */
            COMPILER_64_SHL(edata1, shift);
            COMPILER_64_SHL(emask1, shift);
            /* add these bits to the data and mask */
            COMPILER_64_OR(data[offset], edata1);
            COMPILER_64_OR(mask[offset], emask1);
            shift += len[index];
            if ((64 < shift) && ((offset + 1) < count)) {
                /* there were bits left over */
                over = shift & 0x3F;
                COMPILER_64_SHR(edata0, len[index] - over);
                COMPILER_64_SHR(emask0, len[index] - over);
                /* may need to mask value since SHR is ASR not LSR */
                COMPILER_64_SET(etemp, ~0, ~0);
                COMPILER_64_SHL(etemp, over);
                COMPILER_64_NOT(etemp);
                COMPILER_64_AND(edata0, etemp);
                COMPILER_64_AND(emask0, etemp);
                /* store remaining bits of this qualifier in next buffer */
                data[offset] = edata0;
                mask[offset] = emask0;
            }
        }
        if ((length < 64) && (offset < count)) {
            /* end of qualifier occurs here; throw out extra bits */
            COMPILER_64_SET(etemp, ~0, ~0);
            /* coverity explaination: the value of index will always */
            /* less than 64, there is checking at the enterance of */
            /* the loop */
            /* coverity[large_shift:FALSE] */
            COMPILER_64_SHL(etemp, shift);
            COMPILER_64_NOT(etemp);
            COMPILER_64_AND(data[offset], etemp);
            COMPILER_64_AND(mask[offset], etemp);
            length = 0;
        }
        if (63 < shift) {
            /* need to move to next octbyte */
            shift -= 64;
            length -= 64;
            offset++;
        }
    } /* for (all qualifiers in chain as long as buffer and no erorr) */
    if (qualMap[_BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH]) {
        if (length > 0) {
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "still have %d bits left over!\n"), length));
        }
        if (offset < (count - 1)) {
            LOG_WARN(BSL_LS_BCM_FP,
                     (BSL_META_U(unit,
                                 "still have %d buffer elements left over!\n"),
                      (count - 1) - offset));
            for (offset++; offset < count; offset++) {
                COMPILER_64_ZERO(data[offset]);
                COMPILER_64_ZERO(mask[offset]);
            } /* for (offset++; offset < count; offset++) */
        } /* if (offset < (count - 1)) */
    }
    /*
     *  We discarded BCM_E_NOT_FOUND above because it is possible that for a
     *  compound qualifier, there will only be one part present.  However, the
     *  semantics of the call require BCM_E_NOT_FOUND if a qualifier is not
     *  present, so we need to emulate it now (despite having clobbered the
     *  caller's buffer!).  We shall do so by asserting BCM_E_NOT_FOUND in the
     *  case where *none* of the parts of a qualifier was present, so if any
     *  part of a compound qualifier (or the single part of a simple qualifier)
     *  is present, it is considered found.
     */
    if (!found) {
        LOG_DEBUG(BSL_LS_BCM_FP,
                  (BSL_META_U(unit,
                         "unit %d entry %d does not have"
                                           " qualifier %d (%s)"),
                          unit,
                          entry,
                          bcmQual,
                          _bcm_dpp_field_qual_name[bcmQual]));
    }
exit:
    if ((len != NULL)) {
        sal_free(len);
    }

    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_qualifier_get(bcm_dpp_field_info_OLD_t *unitData,
                             bcm_field_entry_t entry,
                             bcm_field_qualify_t bcmQual,
                             int count,
                             uint64 *data,
                             uint64 *mask)
{
    _bcm_dpp_field_qual_t *qualData;
    bcm_field_qset_t bqset;
    SOC_PPC_FP_QUAL_TYPE *ppdQual;
    _bcm_dpp_field_stage_idx_t stage = 0;
    _bcm_dpp_field_grp_idx_t entryGroup = 0;
    unsigned int qualLimit = 0;
    uint32 groupTypes = 0;
    int result = 0;

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    SOC_PPC_FP_QUAL_VAL qualHwData[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX];
    int useData;
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;
    sal_memset( &bqset, 0, sizeof(bqset));

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              &entryGroup,
                                                              &(qualHwData[0]),
                                                              &useData));
#else /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_prep(unitData,
                                                              entry,
                                                              &qualData,
                                                              &qualLimit,
                                                              &stage,
                                                              &entryGroup));
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */


    if (_BCM_DPP_FIELD_ENTRY_IS_ENTRY(entry)) {

            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.qset.get(unit, entryGroup, &bqset));
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS.groupD.groupTypes.get(unit, entryGroup, &groupTypes));
    } else if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {

/*            *bqset = &(unitData->stageD[*stage].qsetType[_BCM_DPP_FIELD_MAX_GROUP_TYPES]);*/
        FIELD_ACCESS.stageD.qsetType.get(unit, stage, _BCM_DPP_FIELD_MAX_GROUP_TYPES, &bqset);
        groupTypes = 1 << _BCM_DPP_FIELD_MAX_GROUP_TYPES;
    }

    if ((bcmFieldQualifyCount > bcmQual) && (BCM_FIELD_ENTRY_INVALID != bcmQual)) {
        uint8 bit_value ;

        /* standard BCM qualifier */
        /*
         * Was:
         *   if (!BCM_FIELD_QSET_TEST(unitData->unitQset, bcmQual))
         * Which was equivalent to:
         *   if (!SHR_BITGET((unitData->unitQset.w), bcmQual) )
         */
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_UNITQSET_W.bit_get(unit,bcmQual,&bit_value)) ;
        if (!bit_value) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               " qualifier %d (%s)"),
                              unit,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        }
        if (!BCM_FIELD_QSET_TEST(bqset, bcmQual)) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d entry %d QSET does not"
                                               " include %d (%s)"),
                              unit,
                              entry,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        } /* if (!BCM_FIELD_QSET_TEST(gropData->qset, type)) */
    } else { /* if (bcmFieldQualifyCount > type) */
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("invalid BCM qualifier type %d"),
                          bcmQual));
    } /* if (bcmFieldQualifyCount > type) */
    /* get the qualifier mapping */
    result = _bcm_dpp_ppd_qual_from_bcm_qual(unitData,
                                             stage,
                                             groupTypes,
                                             bcmQual,
                                             &ppdQual);
    BCMDNX_IF_ERR_EXIT_MSG(result,
                        (_BSL_BCM_MSG_NO_UNIT("was unable to map qualifier %d (%s)"
                                          " to PPD for unit %d entry %d (stage"
                                          " %u types %08X: %d (%s)"),
                         bcmQual,
                         _bcm_dpp_field_qual_name[bcmQual],
                         unit,
                         entry,
                         stage,
                         groupTypes,
                         result,
                         _SHR_ERRMSG(result)));
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_int(unitData,
                                                             entry,
                                                             stage,
                                                             qualData,
                                                             useData?(&qualHwData[0]):NULL,
                                                             qualLimit,
                                                             bcmQual,
                                                             ppdQual,
                                                             count,
                                                             data,
                                                             mask));
#else /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get_int(unitData,
                                                             entry,
                                                             stage,
                                                             qualData,
                                                             qualLimit,
                                                             bcmQual,
                                                             ppdQual,
                                                             count,
                                                             data,
                                                             mask));
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */
exit:
    _bcm_dpp_field_qualifier_get_cleanup(unitData,
                                         entry,
                                         qualData);
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_field_entry_qualifier_general_get_int(bcm_dpp_field_info_OLD_t *unitData,
                                               bcm_field_entry_t entry,
                                               bcm_field_qualify_t bcmQual,
                                               unsigned int count,
                                               uint64 *data,
                                               uint64 *mask)
{
    bcm_field_presel_t presel;
    bcm_field_stage_t stage;
    int32 qualMaps_handle ;
    uint32 flags = 0;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT;

    unit = unitData->unit;

    if ((0 > bcmQual) || (bcmFieldQualifyCount <= bcmQual)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("qualifier type %d not valid"),
                          bcmQual));
    }
    if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL(entry)) {
        /*
         *  There are special cases for preselectors (bcmFieldQualifyStage;
         *  and maybe others), so
         *  give the preselector code a chance to filter the request.  If the
         *  device supports the qualifier as a 'special' case, it will be
         *  handled by _bcm_petra_field_presel_qualify, but if it should be a
         *  'general' case, _bcm_petra_field_presel_qualify will in turn call
         *  _bcm_dpp_field_qualifier_set to do the actual work.
         */
        presel = _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(entry);
        stage =  bcmFieldStageCount;
        if (_BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(entry)) 
        {
            stage =  _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(entry);
            flags =  _BCM_DPP_PRESEL_ALLOC_WITH_STAGE;
        }
        BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_presel_qualify_get(unitData,
                                                                  presel,
                                                                  stage,
                                                                  flags,
                                                                  bcmQual,
                                                                  count,
                                                                  data,
                                                                  mask));
    } else {
        BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_QUALMAPS.get(unit, bcmQual, &qualMaps_handle)) ;
        if (qualMaps_handle) {
            /* this unit supports this qualifier */
            BCMDNX_IF_ERR_EXIT(_bcm_dpp_field_qualifier_get(unitData,
                                                                 entry,
                                                                 bcmQual,
                                                                 count,
                                                                 data,
                                                                 mask));
        } else { /* if (qualMaps_handle) */
            /* this unit does not support this qualifier */
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL,
                             (_BSL_BCM_MSG_NO_UNIT("unit %d does not support"
                                               "qualifier %d (%s)"),
                              unit,
                              bcmQual,
                              _bcm_dpp_field_qual_name[bcmQual]));
        } /* if (qualMaps_handle) */
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_ppd_stage_from_bcm_stage(bcm_dpp_field_info_OLD_t *unitData,
                                  bcm_field_stage_t bcmStage,
                                  SOC_PPC_FP_DATABASE_STAGE *hwStageId)
{
    int index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT

    unit = unitData->unit;

    /* default stage */
    *hwStageId = SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF; 

    for (index = 0; index < unitData->devInfo->stages; index++) {
        if (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, index).bcmStage == bcmStage) {
            *hwStageId = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, index). hwStageId;
            break;
        }
    }

    if (index == unitData->devInfo->stages) {
    	BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);

/*
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("bcm stage %d is not valid. return default stage."),
                          bcmStage));
*/
    }
exit:
    BCMDNX_FUNC_RETURN;
}

int
_bcm_dpp_bcm_stage_from_ppd_stage(bcm_dpp_field_info_OLD_t *unitData,
                                  SOC_PPC_FP_DATABASE_STAGE hwStageId,
                                  bcm_field_stage_t *bcmStage)
{
    int index;
    BCMDNX_INIT_FUNC_DEFS_NO_UNIT

    unit = unitData->unit;

    /* default stage */
    *bcmStage = bcmFieldStageIngress; 

    for (index = 0; index < unitData->devInfo->stages; index++) {
        if (_BCM_DPP_FIELD_STAGE_DEV_INFO(unit, index).hwStageId == hwStageId) {
            *bcmStage = _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, index).bcmStage;
                break;
        }
    }

    if (index == unitData->devInfo->stages) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,
                         (_BSL_BCM_MSG_NO_UNIT("ppd stage %d is not valid. return default stage."),
                          hwStageId));
    }
exit:
    BCMDNX_FUNC_RETURN;
}

