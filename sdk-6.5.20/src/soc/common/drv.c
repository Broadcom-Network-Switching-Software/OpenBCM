/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * StrataSwitch driver
 */

#include <soc/drv.h>
#ifdef _SHR_BSL_H_
#error Header file soc/drv.h must not include bsl.h directly or indirectly
#endif

#include <shared/bsl.h>

#include <sal/appl/sal.h>
#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <shared/switch.h>
#include <shared/bitop.h>
#include <sal/core/spl.h>
#include <sal/core/sync.h>
#include <sal/core/boot.h>
#include <sal/core/dpc.h>

#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/dma.h>
#include <soc/register.h>
#include <soc/debug.h>
#include <soc/led.h>

#if defined(BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#endif
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/drv.h>
#include <soc/dnxc/drv.h>
#endif
#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/mem.h>
#endif
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#ifdef BCM_CMICX_SUPPORT
#include <shared/cmicfw/cmicx_led.h>
#endif
#ifdef BCM_IPROC_SUPPORT
#include <soc/iproc.h>
#endif
#ifdef INCLUDE_AVS
#include <soc/avs.h>
#endif /* INCLUDE_AVS */
#if defined(PORTMOD_SUPPORT) && defined(BCM_ESW_SUPPORT)
#include <soc/esw/portctrl.h>
#include <soc/phy/phymod_sim.h>
#endif

#ifndef NO_SAL_APPL
#include <sal/appl/config.h>
#endif

#ifdef ADAPTER_SERVER_MODE
#define DRV_ADAPTER_SERVER_SUFFIX "_ADAPTER"
#endif /*ADAPTER_SERVER_MODE*/
#ifdef BCM_DNX_SUPPORT
#define JERICHO2_B1_VERSION_SUFFIX  "_B1"
#endif
#define INVALID_SOC_PROPERTY_SUFFIX_NUM -1

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
#include <soc/dnxc/dnxc_verify.h>
#endif /* defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT) */

/*
 * Driver global variables
 *
 *   soc_control: per-unit driver control structure
 *   soc_persist: persistent per-unit driver control structure
 *   soc_ndev: the number of units created
 *   soc_ndev_attached: the number of units attached
 *   soc_family_suffix: a family suffix, configured by soc property soc_family.
 *     Used what parsing soc properties.
 *     If configured, and property.unit, property.chip and property.group was not
 *     found, property.family is searched for.
 */
soc_control_t   *soc_control[SOC_MAX_NUM_DEVICES];
soc_persist_t   *soc_persist[SOC_MAX_NUM_DEVICES];
int             soc_ndev = 0;
/* soc_ndev_idx2dev_map maps the devices logical id to the physical device id
 * e.g. if we have two devides with ids 0 and 2 and hence soc_ndev is 2,
 * we will have this status:
 * soc_ndev_idx2dev_map[0] will be 0, and
 * soc_ndev_idx2dev_map[1] will be 2.
 */
int             soc_ndev_idx2dev_map[SOC_MAX_NUM_DEVICES];
int             soc_eth_ndev = 0;
int             soc_ndev_attached = 0;
int             soc_all_ndev;
uint32          soc_state[SOC_MAX_NUM_DEVICES];
char            *soc_family_suffix[SOC_MAX_NUM_DEVICES] = {0};

/*   soc_wb_mim_state: indicate that all virtual ports / VPNs are MiM */
/*                     Level 1 warm boot only */
#ifdef BCM_WARM_BOOT_SUPPORT
int             soc_wb_mim_state[SOC_MAX_NUM_DEVICES];
#endif

#if defined(BCM_SAND_SUPPORT)

int soc_get_reg_first_block_id(int unit,soc_reg_t reg,uint32 *block)
{
  soc_reg_info_t reg_info = SOC_REG_INFO(unit, reg);
  int block_type = SOC_REG_FIRST_BLK_TYPE(reg_info.block);
  int blk=-1;
  int schan;

  SOC_BLOCK_ITER(unit, blk, block_type) {
      schan =  SOC_BLOCK_INFO(unit,blk).schan;
      *block = schan;
      return SOC_E_NONE;
  }

  LOG_ERROR(BSL_LS_SOC_COMMON,
    (BSL_META_U(unit,
                "Cant Find block-id for reg %d\n"),
     reg));

  return SOC_E_UNAVAIL;

}
#endif /* defined(BCM_SAND_SUPPORT) */

/*
 * Function:
 *      soc_misc_init
 * Purpose:
 *      Initialize miscellaneous chip registers
 * Parameters:
 *      unit - unit number
 * Returns:
 *      SOC_E_XXX
 */
int
soc_misc_init(int unit)
{
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "soc_misc_init\n")));

    if (!soc_attached(unit)) {
        return SOC_E_INIT;
    }

    if (SOC_IS_RCPU_ONLY(unit)) {
        return SOC_E_NONE;
    }

    /* Perform Chip-Dependent Initializations */

    if (SOC_FUNCTIONS(unit)) {
        if (SOC_FUNCTIONS(unit)->soc_misc_init) {
#ifdef BCM_XGS_SUPPORT
            if (SOC_IS_XGS(unit)) {
                /* Enable accelerated mem clear in case misc init is called
                   again */
                SOC_MEM_CLEAR_HW_ACC_SET(unit, 1);
            }
#endif /* BCM_XGS_SUPPORT */
            SOC_IF_ERROR_RETURN(SOC_FUNCTIONS(unit)->soc_misc_init(unit));
#ifdef BCM_XGS_SUPPORT
            if (SOC_IS_XGS(unit)) {
                /* Disable accelerated mem clear after misc init */
                SOC_MEM_CLEAR_HW_ACC_SET(unit, 0);
            }
#endif /* BCM_XGS_SUPPORT */
        }

#ifdef INCLUDE_AVS
        if (soc_feature(unit, soc_feature_avs_openloop)) {
            soc_avs_openloop_main(unit);
        }
#endif /* INCLUDE_AVS  */

    }
#if defined(PORTMOD_SUPPORT) && defined(BCM_ESW_SUPPORT)
    if (SOC_USE_PORTCTRL(unit)) {
        SOC_IF_ERROR_RETURN(soc_esw_portctrl_init(unit));
        if (soc_property_get(unit, spn_PHY_SIMUL, 0) || SAL_BOOT_PLISIM) {
#if defined (BCM_WARM_BOOT_SUPPORT)
            if (SOC_WARM_BOOT(unit)) {
                    SOC_IF_ERROR_RETURN(soc_physim_scache_recovery(unit));
            } else {
                SOC_IF_ERROR_RETURN(soc_physim_scache_allocate(unit));
            }
#endif /* BCM_WARM_BOOT_SUPPORT */
        }
    }
#endif
    return SOC_E_NONE;
}

void
soc_family_suffix_update(int unit)
{
    soc_family_suffix[unit] = soc_property_get_str(unit, spn_SOC_FAMILY);
}

/*
 * Function:
 *      soc_init
 * Purpose:
 *      Initialize a StrataSwitch without resetting it.
 * Parameters:
 *      unit - StrataSwitch unit #
 * Returns:
 *      SOC_E_XXX
 */
int
soc_init(int unit)
{
    soc_family_suffix_update(unit);

#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_DPP(unit)) {
        return (soc_dpp_init(unit, SOC_DPP_RESET_ACTION_OUT_RESET));
    }
#endif /* defined(BCM_PETRA_SUPPORT) */
#if defined(BCM_DFE_SUPPORT)
    if(SOC_IS_DFE(unit)) {
        return (soc_dfe_init(unit,FALSE));
    }
#endif /*  defined(BCM_DFE_SUPPORT)  */
#if defined(BCM_DNXF_SUPPORT)
    if(SOC_IS_DNXF(unit)) {
        return (soc_dnxf_init(unit,FALSE));
    }
#endif /*  defined(BCM_DNXF_SUPPORT)  */
    {
#ifdef BCM_ESW_SUPPORT
        return(soc_do_init(unit, FALSE));
#endif
    }

    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *      soc_deinit
 * Purpose:
 *      DeInitialize a Device.
 * Parameters:
 *      unit - Device unit #
 * Returns:
 *      SOC_E_XXX
 */
int
soc_deinit(int unit)
{
#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_DPP(unit)) {
        return (soc_dpp_deinit(unit));
    }
#endif /* defined(BCM_PETRA_SUPPORT) */

#if defined(BCM_DFE_SUPPORT)
    if (SOC_IS_DFE(unit)) {
        return (soc_dfe_deinit(unit));
    }
#endif /* defined(BCM_DFE_SUPPORT) */
#if defined(BCM_DNXF_SUPPORT)
    if (SOC_IS_DNXF(unit)) {
        return (soc_dnxf_deinit(unit));
    }
#endif /* defined(BCM_DNXF_SUPPORT) */
    /*inform that detaching device is done*/
    SOC_DETACH(unit,0);

    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *      soc_device_reset
 * Purpose:
 *      Perform different device reset modes/actions.
 * Parameters:
 *      unit - Device unit #
 *      mode - reset mode: hard reset, soft reset, init ....
 *      action - im/out/inout.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_device_reset(int unit, int mode, int action)
{

#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_DPP(unit)) {
        return (soc_dpp_device_reset(unit, mode, action));
    }
#endif /* defined(BCM_PETRA_SUPPORT) */

#if defined(BCM_DNX_SUPPORT)
    if (SOC_IS_DNX(unit)) {
        if ((SOC_IS_J2P(unit)) && ((mode == SOC_DNX_RESET_MODE_BLOCKS_SOFT_RESET) || (mode == SOC_DNX_RESET_MODE_BLOCKS_AND_FABRIC_SOFT_RESET)))
        {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit, "Command failed. Numeric arguments are depracated except for access only mode \n")));
            return SOC_E_UNAVAIL;
        }
        else
        {
            return (soc_dnx_device_reset(unit, mode, 0, action));
        }
    }
#endif

#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit))
    {
        return (soc_dfe_device_reset(unit, mode, action));
    }
#endif

#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        return (soc_dnxf_device_reset(unit, mode, action));
    }
#endif

    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *      soc_reset_init
 * Purpose:
 *      Reset and initialize a StrataSwitch.
 * Parameters:
 *      unit - StrataSwitch unit #
 * Returns:
 *      SOC_E_XXX
 */
int
soc_reset_init(int unit)
{
    soc_family_suffix[unit] = soc_property_get_str(unit, spn_SOC_FAMILY);
#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_DPP(unit)) {
        return (soc_dpp_init(unit, SOC_DPP_RESET_ACTION_INOUT_RESET));
    } else
#endif /* defined(BCM_PETRA_SUPPORT) */
#if defined(BCM_DFE_SUPPORT)
    if (SOC_IS_DFE(unit)) {
        return (soc_dfe_init(unit, TRUE));
    } else
#endif /* BCM_DFE_SUPPORT */
#if defined(BCM_DNXF_SUPPORT)
    if (SOC_IS_DNXF(unit)) {
        return (soc_dnxf_init(unit, TRUE));
    } else
#endif /* BCM_DNXF_SUPPORT */
    {
#ifdef BCM_ESW_SUPPORT
        return(soc_do_init(unit, TRUE));
#endif
    }

    return SOC_E_UNAVAIL;
}

/*
 * Function:
 *    soc_dev_name
 * Purpose:
 *    Return official name (or optionally nickname) of attached chip.
 * Parameters:
 *      unit - unit number
 * Returns:
 *    Pointer to static string
 */

const char *
soc_dev_name(int unit)
{
    const char *name = soc_cm_get_name(unit);
    return (name == NULL ? "<UNKNOWN>" : name);
}

/*
 * Function:
 *    soc_attached
 * Purpose:
 *    Check if a specified SOC device is probed and attached.
 * Returns:
 *    TRUE if attached, FALSE otherwise.
 */
int
soc_attached(int unit)
{
    if (!SOC_UNIT_VALID(unit)) {
        return(0);
    }

    return (SOC_CONTROL(unit) && ((SOC_CONTROL(unit)->soc_flags & SOC_F_ATTACHED) != 0));
}

/*
 * Function:
 *      _str_to_val
 * Purpose:
 *      Convert string to value
 * Parameters:
 *      str - (IN) value string
 *      val - (OUT) converted value
 *      scale - (IN) scale up factor (for decimal only)
 *                   1 means to multiply by 10 before return
 *                   2 means to multiply by 100 before return
 *                   ...
 *      suffix - (OUT) the unit specified in the property (for decimal only)
 *                     'b' for 100b, or 'c' for 500c, ...
 * Notes:
 *      The string is parsed as a C-style numeric constant.
 */
STATIC char *
_str_to_val(const char *str, int *val, int scale, char *suffix)
{
    uint32 abs_val;
    int neg, base, shift;
    unsigned char symbol;

    if (*str == '0' && str[1] != '.' && str[1] != ',') {
        str++;

        symbol = *str | 0x20;
        if (symbol == 'x') { /* hexadecimal */
            str++;
            for (abs_val = 0; *str != '\0'; str++) {
                symbol = *str - '0';
                if (symbol < 10) {
                    abs_val = (abs_val << 4) | symbol;
                    continue;
                }
                symbol = (*str | 0x20) - 'a';
                if (symbol < 6) {
                    abs_val = (abs_val << 4) | (symbol + 10);
                    continue;
                }
                break;
            }
        } else {
            if (symbol == 'b') { /* binary */
                base = 2;
                shift = 1;
                str++;
            } else { /* octal */
                base = 8;
                shift = 3;
            }
            for (abs_val = 0; *str != '\0'; str++) {
                symbol = *str - '0';
                if (symbol < base) {
                    abs_val = (abs_val << shift) | symbol;
                    continue;
                }
                break;
            }
        }
        *val = abs_val;
        *suffix = (*str == ',') ? ',' : '\0';
    } else {
        if (*str == '-') {
            str++;
            neg = TRUE;
        } else {
            neg = FALSE;
        }
        for (abs_val = 0; *str != '\0'; str++) {
            symbol = *str - '0';
            if (symbol < 10) {
                abs_val = abs_val * 10 + symbol;
                continue;
            }
            break;
        }
        if (*str == '.') {
            str++;
            for (; scale > 0 && *str != '\0'; scale--, str++) {
                symbol = *str - '0';
                if (symbol < 10) {
                    abs_val = abs_val * 10 + symbol;
                    continue;
                }
                break;
            }
        }
        for (; scale > 0; scale--) {
            abs_val *= 10;
        }

        *val = neg ? -(int)abs_val : (int)abs_val;
        *suffix = *str;
    }

    return (char *)str;
}

/*
 * Function:
 *  soc_property_get_str
 * Purpose:
 *  Retrieve a global configuration property string.
 * Parameters:
 *  unit - unit number
 *  name - base name of property to look up
 * Notes:
 *  Each property requested is looked up with one suffix after
 *  another until it is found.  Suffixes are tried in the following
 *  order:
 *  .<unit-num>     (e.g. ".0")
 *  .<CHIP_TYPE>    (e.g. BCM5680_B0)
 *  .<CHIP_GROUP>   (e.g. BCM5680)
 *  <nothing>
 */
char *
soc_property_get_str(int unit, const char *name)
{
    if (unit >= 0) {
        /* length of name + \0 */
        int     l = sal_strlen(name) + 1;
        int     ltemp;
        char    name_exp[SOC_PROPERTY_NAME_MAX], *s;

        if (sal_snprintf(name_exp, SOC_PROPERTY_NAME_MAX,
                         "%s.%d", name, unit) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.%d. "
                          "Max soc property length:%d\n"),
               name, unit, SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        if ((s = soc_cm_config_var_get(unit, name_exp)) != NULL) {
            return s;
        }

        /* Remaining operations require that the unit is attached */
        if (soc_attached(unit)) {

            ltemp = sal_strlen(SOC_UNIT_NAME(unit));

            /* check the length of <name>.<unit_name>\0 */
            if ((l + 1 + ltemp) > SOC_PROPERTY_NAME_MAX) {
                return NULL;
            }
            sal_strncpy(name_exp + l, SOC_UNIT_NAME(unit), ltemp);
            if (ltemp)
                *(name_exp + l + ltemp) = '\0';
            if ((s = soc_cm_config_var_get(unit, name_exp)) != NULL) {
                return s;
            }

            ltemp = sal_strlen(SOC_UNIT_GROUP(unit));

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPORT)
            /*
             * Reading <name>.<group name>_<revision name>
             * For example: BCM8869X_B0,BCM8869X_B1
             * Assuming: <group name> and <chip name without revision> are different but has the same length
             */
            if (soc_feature(unit, soc_feature_prop_suffix_group_with_revision))
            {
                int ltemp_unit_name;
                /*
                 * For JR2_B1,SOC_DRIVER(unit)->type is SOC_CHIP_BCM88690_B0. It is the same as JR2_B0.
                 * So, SOC_UNIT_GROUP(unit) and SOC_UNIT_GROUP(unit) are the same for JR2 B0 and B1.
                 * In order to distinguish B0 and B1,deal with BCM8869X_B1 specically.
                 */
                if (SOC_IS_JERICHO2_B1_ONLY(unit)) {
                    int len_version;
                    len_version = sal_strlen(JERICHO2_B1_VERSION_SUFFIX);
                    /* check the length of <name>.<unit_group>_B1 */
                    if ((l + 1 + ltemp + len_version) > SOC_PROPERTY_NAME_MAX) {
                        return NULL;
                    }
                    sal_strncpy(name_exp + l, SOC_UNIT_GROUP(unit), ltemp);
                    sal_strncpy(name_exp + l + ltemp, JERICHO2_B1_VERSION_SUFFIX, len_version);
                    if (ltemp)
                        *(name_exp + l + ltemp + len_version) = '\0';
                    if ((s = soc_cm_config_var_get(unit, name_exp)) != NULL) {
                        return s;
                    }
                }
                ltemp_unit_name = sal_strlen(SOC_UNIT_NAME(unit));
                sal_strncpy(name_exp + l, SOC_UNIT_NAME(unit), ltemp_unit_name);
                sal_strncpy(name_exp + l, SOC_UNIT_GROUP(unit), ltemp);
                if (ltemp)
                    *(name_exp + l + ltemp_unit_name) = '\0';
                if ((s = soc_cm_config_var_get(unit, name_exp)) != NULL) {
                    return s;
                }
            }
#endif /*BCM_DNX_SUPPORT || BCM_DNXF_SUPPORT */

#if defined(BCM_DNX_SUPPORT) && defined(ADAPTER_SERVER_MODE)
            if (SOC_IS_DNX(unit)){
                int len_adapter;
                len_adapter = sal_strlen(DRV_ADAPTER_SERVER_SUFFIX);

                /* check the length of <name>.<unit_group>_ADAPTER */
                if ((l + 1 + ltemp + len_adapter) > SOC_PROPERTY_NAME_MAX) {
                    return NULL;
                }
                sal_strncpy(name_exp + l, SOC_UNIT_GROUP(unit), ltemp);
                sal_strncpy(name_exp + l + ltemp, DRV_ADAPTER_SERVER_SUFFIX, len_adapter);
                if (ltemp)
                    *(name_exp + l + ltemp + len_adapter) = '\0';
                if ((s = soc_cm_config_var_get(unit, name_exp)) != NULL) {
                    return s;
                }
            }
#endif /*BCM_DNX_SUPPORT && ADAPTER_SERVER_MODE*/

            /* check the length of <name>.<unit_group> */
            if ((l + 1 + ltemp) > SOC_PROPERTY_NAME_MAX) {
                return NULL;
            }
            sal_strncpy(name_exp + l, SOC_UNIT_GROUP(unit), ltemp);
            if (ltemp)
                *(name_exp + l + ltemp) = '\0';
            if ((s = soc_cm_config_var_get(unit, name_exp)) != NULL) {
                return s;
            }

#if defined(BCM_PETRA_SUPPORT)
            /* workaround to include 88375,88680 in 88675 */
            if ( (sal_strstr(SOC_UNIT_GROUP(unit),"BCM88375")) || (sal_strstr(SOC_UNIT_GROUP(unit),"BCM88680")) ) {
                sal_strncpy(name_exp + l, "BCM88675", ltemp);
            } else if ( sal_strstr(SOC_UNIT_GROUP(unit),"BCM88270") ) {
            /* workaround to include 88270 in 88470 */
                sal_strncpy(name_exp + l, "BCM88470", ltemp);
            }
            if (ltemp)
                *(name_exp + l + ltemp) = '\0';
            if ((s = soc_cm_config_var_get(unit, name_exp)) != NULL) {
                return s;
            }
#endif

            if (soc_family_suffix[unit]) {
                ltemp = sal_strlen(soc_family_suffix[unit]);

                /* check the length of <name>.<soc_family_suffix> */
                if ((l + 1 + ltemp) > SOC_PROPERTY_NAME_MAX) {
                    return NULL;
                }
                sal_strncpy(name_exp + l, soc_family_suffix[unit], ltemp);
                if (ltemp)
                    *(name_exp + l + ltemp) = '\0';
                if ((s = soc_cm_config_var_get(unit, name_exp)) != NULL) {
                    return s;
                }
            }
        }
    }

    if (unit == SOC_UNIT_NONE_SAL_CONFIG) {
#ifndef NO_SAL_APPL
        return sal_config_get(name);
#else
        return NULL;
#endif
    }

    return soc_cm_config_var_get(unit, name);
}

/*
 * Function:
 *      soc_property_get
 * Purpose:
 *      Retrieve a global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      See soc_property_get_str for suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */

uint32
soc_property_get(int unit, const char *name, uint32 defl)
{
    char        *s;

    if ((s = soc_property_get_str(unit, name)) == NULL) {
        return defl;
    }

    return _shr_ctoi(s);
}

/*
 * Function:
 *      soc_property_obj_attr_get
 * Purpose:
 *      Retrieve a global configuration numeric value and its unit for the
 *      specified prefix, object and attribute.
 * Parameters:
 *      unit - unit number
 *      prefix - prefix name of the numeric property to look up
 *      obj - object name of the numeric property to look up
 *      index - object index of the numeric property to look up
 *      attr - attribute name of the numeric property to look up
 *      scale - scale up factor (see _str_to_val)
 *      suffix - the unit specified in the property (see _str_to_val)
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of object suffixes
 *      until it is found.
 *              prefix.obj1.attr    attribute of a particular object instance
 *              prefix.obj.attr     attribute of the object type
 *      If none are found then the default value in defl is returned.
 *      See soc_property_get_str for other suffix handling in property name.
 *      See _str_to_val for value string handling.
 *      The string is parsed as a C-style numeric constant.
 */

uint32
soc_property_obj_attr_get(int unit, const char *prefix, const char *obj,
                          int index, const char *attr, int scale,
                          char *suffix, uint32 defl)
{
    char        *str;
    int         val;
    char        prop[SOC_PROPERTY_NAME_MAX];

    str = NULL;


    if (index != -1) {
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s.%s%d.%s", prefix, obj, index, attr) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.%s%d.%s. "
                          "Max soc property length:%d\n"),
               prefix, obj, index, attr, SOC_PROPERTY_NAME_MAX));
            return defl;
        }
        str = soc_property_get_str(unit, prop);
    }

    if (str == NULL) {
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s.%s.%s", prefix, obj, attr) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.%s.%s. "
                          "Max soc property length:%d\n"),
               prefix, obj, attr, SOC_PROPERTY_NAME_MAX));
            return defl;
        }
        str = soc_property_get_str(unit, prop);
    }

    if (str == NULL) {
        return defl;
    }

    (void)_str_to_val(str, &val, scale, suffix);
    return (uint32)val;
}

/*
 * Function:
 *      soc_property_get_pbmp
 * Purpose:
 *      Retrieve a global configuration bitmap value.
 * Parameters:
 *      unit - unit number
 *      name - base name of numeric property to look up
 *      defneg - should default bitmap be all 1's?
 * Notes:
 *      See soc_property_get_str for suffix handling.
 *      The string can be more than 32 bits worth of
 *      data if it is in hex format (0x...).  If not
 *      hex, it is treated as a 32 bit value.
 */

pbmp_t
soc_property_get_pbmp(int unit, const char *name, int defneg)
{
    pbmp_t      bm;
    char        *s;

    if ((s = soc_property_get_str(unit, name)) == NULL) {
        SOC_PBMP_CLEAR(bm);
        if (defneg) {
            SOC_PBMP_NEGATE(bm, bm);
        }
        return bm;
    }
    if (_shr_pbmp_decode(s, &bm) < 0) {
        SOC_PBMP_CLEAR(bm);
    }
    return bm;
}

/*
 * Function:
 *      soc_property_get_pbmp_default
 * Purpose:
 *      Retrieve a global configuration bitmap value with specific default.
 * Parameters:
 *      unit - unit number
 *      name - base name of numeric property to look up
 *      def_pbmp - default bitmap
 * Notes:
 *      See soc_property_get_str for suffix handling.
 *      The string can be more than 32 bits worth of
 *      data if it is in hex format (0x...).  If not
 *      hex, it is treated as a 32 bit value.
 */

pbmp_t
soc_property_get_pbmp_default(int unit, const char *name,
                              pbmp_t def_pbmp)
{
    pbmp_t      bm;
    char        *s;

    if ((s = soc_property_get_str(unit, name)) == NULL) {
        SOC_PBMP_ASSIGN(bm, def_pbmp);
        return bm;
    }
    if (_shr_pbmp_decode(s, &bm) < 0) {
        SOC_PBMP_CLEAR(bm);
    }
    return bm;
}

/*
 * Function:
 *      soc_property_suffix_num_str_get
 * Purpose:
 *      Retrieve a per-enumerated suffix global configuration bitmap value.
 * Parameters:
 *      unit - unit number
 *      num - enumerated suffix for which property is applicable
 *      name - base name of numeric property to look up
 *      suffix - suffix of numeric property to look up
 *      pbmp_def - default value of bitmap property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"0  enumerated suffix name
 *              name            plain name
 *      If none are found then the default value in pbmp_def is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as an hexadecimal value
 */

pbmp_t
soc_property_suffix_num_pbmp_get(int unit, int num, const char *name,
                            const char *suffix, soc_pbmp_t pbmp_def)
{
    pbmp_t      bm;
    char        *s;

    if ((s = soc_property_suffix_num_str_get(unit, num, name, suffix)) == NULL) {
        SOC_PBMP_ASSIGN(bm, pbmp_def);
        return bm;
    }
    if (_shr_pbmp_decode(s, &bm) < 0) {
        SOC_PBMP_ASSIGN(bm, pbmp_def);
    }
    return bm;
}


/*
 * Function:
 *      soc_property_get_bitmap_default
 * Purpose:
 *      Retrieve a global configuration bitmap value with specific default.
 * Parameters:
 *      unit - unit number
 *      name - base name of numeric property to look up
 *      def_bitmap - default bitmap
 *      bitmap - reterived bitmap
 * Notes:
 *      See soc_property_get_str for suffix handling.
 *      The string can be more than 32 bits worth of
 *      data if it is in hex format (0x...).  If not
 *      hex, it is treated as a 32 bit value.
 */

void
soc_property_get_bitmap_default(int unit, const char *name,
                              uint32 *bitmap, int max_words, uint32 *def_bitmap)
{
    char        *s;

    if ((s = soc_property_get_str(unit, name)) == NULL) {
        sal_memcpy(bitmap, def_bitmap, sizeof(uint32) * max_words);
        return;
    }
    if (shr_bitop_str_decode((const char *)s, bitmap, max_words) < 0) {
        sal_memset(bitmap, 0, sizeof(uint32) * max_words);
        return;
    }
}

/*
 * Function:
 *      soc_property_get_csv
 * Purpose:
 *      Retrieve a global configuration comma-separated value in an array.
 * Parameters:
 *      unit - unit number or SOC_UNIT_NONE_SAL_CONFIG
 *      name - base name of numeric property to look up
 *      val_array - Integer array to put the values
 *      val_max - Maximum number of elements to put into the array
 * Notes:
 *      Returns the count of values read.
 */

int
soc_property_get_csv(int unit, const char *name,
                          int val_max, int *val_array)
{
    char *str, suffix;
    int count;

    str = soc_property_get_str(unit, name);
    if (str == NULL) {
        return 0;
    }

    count = 0;
    for (count = 0; count < val_max; count++) {
        str = _str_to_val(str, &val_array[count], 0, &suffix);
        if (suffix == ',') {
            str++;
        } else {
            count++;
            break;
        }
    }

    return count;
}

/*
 * Function:
 *      soc_property_port_get
 * Purpose:
 *      Retrieve a per-port global configuration value
 * Parameters:
 *      unit - unit number
 *      port - Zero-based port number.
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_fe0        port name
 *              name_fe         port type
 *              name_port1      logical port number (one based)
 *              name.port1      physical port number (one based)
 *              name_1          BCM API port number (untranslated)
 *              name            plain name
 *      If none are found then NULL is returned
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */

char *
soc_property_port_get_str(int unit, soc_port_t port,
                          const char *name)
{
    char        *s, *p;
    char        prop[SOC_PROPERTY_NAME_MAX], prop_alter[SOC_PROPERTY_NAME_MAX];
    int         pno;

    /*
     * Try "name_fe0" if in valid range (can be out of range for
     * subport / internal)
     */
    if (port < SOC_MAX_NUM_PORTS) {

        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s_%s", name, SOC_PORT_NAME(unit, port)) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s_%s. "
                          "Max soc property length:%d\n"),
               name, SOC_PORT_NAME(unit, port), SOC_PROPERTY_NAME_MAX));
            return NULL;
        }

        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return s;
        }

        /* Try alternative name if available */
        if (SOC_PORT_NAME_ALTER_VALID(unit, port))
        {
            if (sal_snprintf(prop_alter, SOC_PROPERTY_NAME_MAX,
                             "%s_%s", name, SOC_PORT_NAME_ALTER(unit, port)) >= SOC_PROPERTY_NAME_MAX) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Unsupported soc_property length for %s_%s. "
                              "Max soc property length:%d\n"),
                   name, SOC_PORT_NAME_ALTER(unit, port), SOC_PROPERTY_NAME_MAX));
                return NULL;
            }

            if ((s = soc_property_get_str(unit, prop_alter)) != NULL) {
                return s;
            }
        }

        /* Try "name_fe" (strip off trailing digits) */
        p = &prop[sal_strlen(prop)-1];
        while (*p >= '0' && *p <= '9') {
          --p;
        }
        *++p = '\0';
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
          return s;
        }

        /* Try alternative name if available */
        if (SOC_PORT_NAME_ALTER_VALID(unit, port))
        {
            p = &prop_alter[sal_strlen(prop_alter)-1];
            while (*p >= '0' && *p <= '9') {
              --p;
            }
            *++p = '\0';
            if ((s = soc_property_get_str(unit, prop_alter)) != NULL) {
              return s;
            }
        }
    }

    /* Try "name.port%d" for explicit match first */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s.port%d", name, port+1) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s.port%d. "
                      "Max soc property length:%d\n"),
           name, port+1, SOC_PROPERTY_NAME_MAX));
        return NULL;
    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return s;
    }

    /*
     * Try "name_port1": search for matching port number.
     * In the case of the SOC_PORT macros, "port" is being
     * passed in as a ptype, not a variable.
     */
    for (pno = 0; pno < SOC_PORT_NUM(unit, port); pno++) {
        if (SOC_PORT(unit, port, pno) == port) {
            if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                             "%s_port%d", name, pno+1) >= SOC_PROPERTY_NAME_MAX) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META_U(unit,
                              "Unsupported soc_property length for %s.port%d. "
                              "Max soc property length:%d\n"),
                   name, pno+1, SOC_PROPERTY_NAME_MAX));
                return NULL;
            }
            if ((s = soc_property_get_str(unit, prop)) != NULL) {
                return s;
            }
            break;
        }
    }


    /* Try "name_%d" for raw logical port match */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_%d", name, port) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_%d. "
                      "Max soc property length:%d\n"),
           name, port, SOC_PROPERTY_NAME_MAX));
        return NULL;
    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return s;
    }

    /* try plain "name" */

    if ((s = soc_property_get_str(unit, name)) != NULL) {
        return s;
    }

    return NULL;
}

/*
 * Function:
 *      soc_property_port_get
 * Purpose:
 *      Retrieve a per-port global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      port - Zero-based port number.
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      See soc_property_port_get_str for parsing details.
 */

uint32
soc_property_port_get(int unit, soc_port_t port,
                      const char *name, uint32 defl)
{
    char        *s;

    s = soc_property_port_get_str(unit, port, name);
    if (s) {
      return _shr_ctoi(s);
    }

    return defl;
}

/*
 * Function:
 *      soc_property_port_num_get
 * Purpose:
 *      Retrieve a per-port global configuration value
 * Parameters:
 *      unit - unit number
 *      port - Zero-based port number.
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_1          BCM API port number (untranslated)
 *      If none are found then NULL is returned
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */

char *
soc_property_port_num_get_str(int unit, soc_port_t port,
                          const char *name)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

    /* Try "name_%d" for raw logical port match */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_%d", name, port) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_%d. "
                      "Max soc property length:%d\n"),
           name, port, SOC_PROPERTY_NAME_MAX));
        return NULL;
    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return s;
    }

    return NULL;
}

/*
 * Function:
 *      soc_property_port_num_get
 * Purpose:
 *      Retrieve a per-port global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      port - Zero-based port number.
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      See soc_property_port_get_str for parsing details.
 */

uint32
soc_property_port_num_get(int unit, soc_port_t port,
                      const char *name, uint32 defl)
{
    char        *s;

    s = soc_property_port_num_get_str(unit, port, name);
    if (s) {
      return _shr_ctoi(s);
    }

    return defl;
}

/*
 * Function:
 *      soc_property_phy_get_str
 * Purpose:
 *      Retrieve a per-phy global configuration value
 * Parameters:
 *      unit - unit number
 *      pport - physical port number
 *      phy_num - PHY number in chain (1: innermost external PHY)
 *      phy_port - PHY port (0:line, 1:system)
 *      lane - PHY lane number on selected PHY port
 *      name - base name of numeric property to look up
 * Notes:
 *      The variable name is looked up up with a number of suffixes
 *      until a match is found:
 *              name{1.2}       physical port number + PHY number
 *              name{1}         physical port number (match any PHY number)
 *              name{1.2.0.1}   physical port number + PHY number/port/lane
 *      If no match is found, then the physical port number is
 *      converted to a logical port number and sent through
 *      soc_property_port_get_str for additional matching.
 */

char *
soc_property_phy_get_str(int unit, int pport,
                         int phy_num, int phy_port, int lane,
                         const char *name)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];
    soc_port_t  port;
    int         len;

    if (phy_num >= 0) {
        /* Try "name{%d.%d.%d.%d}" for exact PHY lane match */
        len = sal_snprintf(prop, SOC_PROPERTY_NAME_MAX, "%s{%d.%d.%d.%d}",
                           name, pport, phy_num, phy_port, lane);
        if (len >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "SOC property %s{%d.%d.%d.%d} too long. "
                                  "Max soc property length: %d\n"),
                       name, pport, phy_num, phy_port, lane,
                       SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return s;
        }
        /* Try "name{%d.%d.%d}" for exact PHY port match */
        len = sal_snprintf(prop, SOC_PROPERTY_NAME_MAX, "%s{%d.%d.%d}",
                           name, pport, phy_num, phy_port);
        if (len >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "SOC property %s{%d.%d.%d} too long. "
                                  "Max soc property length: %d\n"),
                       name, pport, phy_num, phy_port,
                       SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return s;
        }
        /* Try "name{%d.%d}" for exact PHY match */
        len = sal_snprintf(prop, SOC_PROPERTY_NAME_MAX, "%s{%d.%d}",
                           name, pport, phy_num);
        if (len >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "SOC property %s{%d.%d} too long. "
                                  "Max soc property length: %d\n"),
                       name, pport, phy_num,
                       SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return s;
        }
    }

    /* Try "name{%d}" for specific physical port match */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s{%d}", name, pport) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "SOC property %s{%d} too long. "
                      "Max soc property length: %d\n"),
           name, pport, SOC_PROPERTY_NAME_MAX));
        return NULL;
    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return s;
    }

    if (SOC_PORT_VALID_RANGE(unit, pport)) {
        port = pport;
        if (soc_feature(unit, soc_feature_logical_port_num)) {
            port = SOC_INFO(unit).port_p2l_mapping[pport];
        }
        return soc_property_port_get_str(unit, port, name);
    }

    return NULL;
}

/*
 * Function:
 *      soc_property_phy_get
 * Purpose:
 *      Retrieve a per-PHY global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      port - physical port number
 *      phy_num - PHY number in chain
 *      phy_port - PHY port
 *      lane - PHY lane number on selected PHY port
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      See soc_property_phy_get_str for parsing details.
 */

uint32
soc_property_phy_get(int unit, int port,
                     int phy_num, int phy_port, int lane,
                     const char *name, uint32 defl)
{
    char        *s;

    s = soc_property_phy_get_str(unit, port, phy_num, phy_port, lane, name);
    if (s) {
        return _shr_ctoi(s);
    }

    return defl;
}

/*
 * Function:
 *      soc_property_phys_port_get
 * Purpose:
 *      Retrieve a per-physical port global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      pport - physical port number
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      See soc_property_phy_get_str for parsing details.
 */

uint32
soc_property_phys_port_get(int unit, int pport,
                           const char *name, uint32 defl)
{
    char        *s;

    s = soc_property_phy_get_str(unit, pport, -1, -1, -1, name);
    if (s) {
        return _shr_ctoi(s);
    }

    return defl;
}

/*
 * Function:
 *      soc_property_port_obj_attr_get
 * Purpose:
 *      Retrieve a global configuration numeric value and its unit for the
 *      specified prefix, object and attribute.
 * Parameters:
 *      unit - unit number
 *      port - Zero-based port number.
 *      prefix - prefix name of the numeric property to look up
 *      obj - object name of the numeric property to look up
 *      index - object index of the numeric property to look up
 *      attr - attribute name of the numeric property to look up
 *      scale - scale up factor (see _str_to_val)
 *      suffix - the unit specified in the property (see _str_to_val)
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of object suffixes
 *      until it is found.
 *              prefix.obj1.attr    attribute of a particular object instance
 *              prefix.obj.attr     attribute of the object type
 *      If none are found then the default value in defl is returned.
 *      See soc_property_port_get_str for other suffix handling in property
 *      name.
 *      See _str_to_val for value string handling.
 *      The string is parsed as a C-style numeric constant.
 */

uint32
soc_property_port_obj_attr_get(int unit, soc_port_t port, const char *prefix,
                               const char *obj, int index, const char *attr,
                               int scale, char *suffix, uint32 defl)
{
    char        *str;
    int         val;
    char        prop[SOC_PROPERTY_NAME_MAX];

    str = NULL;

    if (index != -1) {
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s.%s%d.%s", prefix, obj, index, attr) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.%s%d.%s. "
                          "Max soc property length:%d\n"),
               prefix, obj, index, attr, SOC_PROPERTY_NAME_MAX));
            return defl;

        }
        str = soc_property_port_get_str(unit, port, prop);
    }

    if (str == NULL) {
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s.%s.%s", prefix, obj, attr) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.%s.%s. "
                          "Max soc property length:%d\n"),
               prefix, obj, attr, SOC_PROPERTY_NAME_MAX));
            return defl;

        }
        str = soc_property_port_get_str(unit, port, prop);
    }

    if (str == NULL) {
        return defl;
    }

    (void)_str_to_val(str, &val, scale, suffix);
    return (uint32)val;
}

int
soc_property_port_get_csv(int unit, soc_port_t port, const char *name,
                          int val_max, int *val_array)
{
    char *str, suffix;
    int count;

    str = soc_property_port_get_str(unit, port, name);
    if (str == NULL) {
        return 0;
    }

    count = 0;
    for (count = 0; count < val_max; count++) {
        str = _str_to_val(str, &val_array[count], 0, &suffix);
        if (suffix == ',') {
            str++;
        } else {
            count++;
            break;
        }
    }

    return count;
}

/*
 * Function:
 *      soc_property_cos_get
 * Purpose:
 *      Retrieve a per-port global configuration value
 * Parameters:
 *      unit - unit number
 *      cos - Zero-based cos number.
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name.cos1       cos number (one based)
 *              name            plain name
 *      If none are found then NULL is returned
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */

char *
soc_property_cos_get_str(int unit, soc_cos_t cos,
                          const char *name)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

    /* try "name.cos%d" for explicit match first */
    /* check length of <name>.cos<cos>\0 */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s.cos%d", name, (cos + 1)) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.cos%d. "
                          "Max soc property length:%d\n"),
               name, (cos + 1), SOC_PROPERTY_NAME_MAX));
            return NULL;
    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return(s);
    }
    /* try plain "name" */
    if ((s = soc_property_get_str(unit, name)) != NULL) {
        return(s);
    }

    return(NULL);
}


/*
 * Function:
 *      soc_property_cos_get
 * Purpose:
 *      Retrieve a per-cos global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      cos - Zero-based cos number.
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_cos1       cos number (one based)
 *              name            plain name
 *      If none are found then the default value in defl is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */
uint32
soc_property_cos_get(int unit, soc_cos_t cos,
                                    const char *name, uint32 defl)
{
    char        *s;

    s = soc_property_cos_get_str(unit, cos, name);
    if (s) {
      return _shr_ctoi(s);
    }

    return defl;
}


/*
 * Function:
 *      soc_property_suffix_num_get
 * Purpose:
 *      Retrieve a per-enumerated suffix global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      num - enumerated suffix for which property is applicable
 *      name - base name of numeric property to look up
 *      suffix - suffix of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"0  enumerated suffix name
 *              name            plain name
 *      If none are found then the default value in defl is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */

uint32
soc_property_suffix_num_get(int unit, int num, const char *name,
                            const char *suffix, uint32 defl)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/* skip searching with suffix num, if suffix num < 0 */
    if(!(soc_attached(unit)) || !(SOC_IS_DNX(unit) ||  SOC_IS_DNXF(unit)) || (num != INVALID_SOC_PROPERTY_SUFFIX_NUM ))
#endif
    {
        /* try "name_'suffix'0" */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s_%s%1d", name, suffix, num) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s_%s%1d. "
                          "Max soc property length:%d\n"),
               name, suffix, num, SOC_PROPERTY_NAME_MAX));
            return defl;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return _shr_ctoi(s);
        }

        /* try "name.port%d.suffix" for explicit match first */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s.port%d.%s", name, num+1, suffix) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.port%d.%s. "
                          "Max soc property length:%d\n"),
               name, num+1, suffix, SOC_PROPERTY_NAME_MAX));
            return defl;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return _shr_ctoi(s);
        }
    }
    /* try "name_'suffix'" */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_%s", name, suffix) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_%s. "
                      "Max soc property length:%d\n"),
           name, suffix, SOC_PROPERTY_NAME_MAX));
        return defl;
    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return _shr_ctoi(s);
    }

    /* try plain "name" */
    if ((s = soc_property_get_str(unit, name)) != NULL) {
        return _shr_ctoi(s);
    }

    return defl;
}

/*
 * Function:
 *      soc_property_suffix_num_get_only_suffix
 * Purpose:
 *      Retrieve a per-enumerated suffix global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      num - enumerated suffix for which property is applicable
 *      name - base name of numeric property to look up
 *      suffix - suffix of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"0  enumerated suffix name
 *      If none are found then the default value in defl is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */

uint32
soc_property_suffix_num_get_only_suffix(int unit, int num, const char *name,
                            const char *suffix, uint32 defl)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/* skip searching with suffix num, if suffix num < 0 */
    if(!(soc_attached(unit)) || !(SOC_IS_DNX(unit) ||  SOC_IS_DNXF(unit)) || (num != INVALID_SOC_PROPERTY_SUFFIX_NUM ))
#endif
    {
        /* try "name_'suffix'0" */
        /* check length of <name>_<suffix><num>\0 */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s_%s%1d", name, suffix, num) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s_%s%1d. "
                          "Max soc property length:%d\n"),
               name, suffix, num, SOC_PROPERTY_NAME_MAX));
            return defl;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return _shr_ctoi(s);
        }

        /* try "name.port%d.suffix" for explicit match first */
        /* check length of <name>.port<num>.<suffix>\0 */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s.port%d.%s", name, num+1, suffix) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.port%d.%s. "
                          "Max soc property length:%d\n"),
               name, num+1, suffix, SOC_PROPERTY_NAME_MAX));
            return defl;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return _shr_ctoi(s);
        }
    }
    /* try "name_'suffix'" */
    /* check length of <name>_<suffix>\0 */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_%s", name, suffix) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_%s. "
                      "Max soc property length:%d\n"),
           name, suffix, SOC_PROPERTY_NAME_MAX));
        return defl;
    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return _shr_ctoi(s);
    }

    return defl;
}

/*
 * Function:
 *      soc_property_suffix_num_str_get
 * Purpose:
 *      Retrieve a per-enumerated suffix global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      num - enumerated suffix for which property is applicable
 *      name - base name of numeric property to look up
 *      suffix - suffix of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"0  enumerated suffix name
 *              name            plain name
 *      If none are found then the default value in defl is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */


char*
soc_property_suffix_num_str_get(int unit, int num, const char *name,
                            const char *suffix)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/* skip searching with suffix num, if suffix num < 0 */
    if(!(soc_attached(unit)) || !(SOC_IS_DNX(unit) ||  SOC_IS_DNXF(unit)) || (num != INVALID_SOC_PROPERTY_SUFFIX_NUM ))
#endif
    {
        /* try "name_'suffix'0" */
        /* check length of <name>_<suffix>\0 */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s_%s%1d", name, suffix, num) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s_%s%1d. "
                          "Max soc property length:%d\n"),
               name, suffix, num, SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return s;
        }


        /* try "name.port%d.suffix" for explicit match first */
        /* check length of <name>.port<num>.<suffix>\0 */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s.port%d.%s", name, num+1, suffix) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.port%d.%s. "
                          "Max soc property length:%d\n"),
               name, num+1, suffix, SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return s;
        }
    }
    /* try "name_'suffix'" */
    /* check length of <name>_<suffix>\0 */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_%s", name, suffix) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_%s. "
                      "Max soc property length:%d\n"),
           name, suffix, SOC_PROPERTY_NAME_MAX));
        return NULL;
    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return s;
    }

    /* try plain "name" */
    if ((s = soc_property_get_str(unit, name)) != NULL) {
        return s;
    }

    return NULL;
}


/*
 * Function:
 *      soc_property_suffix_num_only_suffix_str_get
 * Purpose:
 *      Retrieve a per-enumerated suffix global configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      num - enumerated suffix for which property is applicable
 *      name - base name of numeric property to look up
 *      suffix - suffix of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"0  enumerated suffix name
 *              name            plain name
 *      If none are found then the default value in defl is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */


char*
soc_property_suffix_num_only_suffix_str_get(int unit, int num, const char *name,
                            const char *suffix)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/* skip searching with suffix num, if suffix num < 0 */
    if(!(soc_attached(unit)) || !(SOC_IS_DNX(unit) ||  SOC_IS_DNXF(unit)) || (num != INVALID_SOC_PROPERTY_SUFFIX_NUM ))
#endif
    {
        /* try "name_'suffix'0" */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s_%s%1d", name, suffix, num) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s_%s%1d. "
                          "Max soc property length:%d\n"),
               name, suffix, num, SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return s;
        }

        /* try "name.port%d.suffix" for explicit match first */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s.port%d.%s", name, num+1, suffix) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s.port%d.%s. "
                          "Max soc property length:%d\n"),
               name, num+1, suffix, SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return s;
        }
    }
    /* try "name_'suffix'" */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_%s", name, suffix) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_%s. "
                      "Max soc property length:%d\n"),
           name, suffix, SOC_PROPERTY_NAME_MAX));
        return NULL;

    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return s;
    }

    return NULL;
}
/*
 * Function:
 *      soc_property_port_suffix_num_get
 * Purpose:
 *      Retrieve a per-port, per-enumerated suffix global
 *      configuration numeric value.
 * Parameters:
 *      unit - unit number
 *      port - Zero-based port number.
 *      num - enumerated suffix for which property is applicable
 *      name - base name of numeric property to look up
 *      suffix - suffix of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"0_port  enumerated suffix name per port
 *              name_"suffix"0       enumerated suffix name
 *              name_port            plain name per port
 *              name                 plain name
 *      If none are found then the default value in defl is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */

uint32
soc_property_port_suffix_num_get(int unit, soc_port_t port,
                                 int num, const char *name,
                                 const char *suffix, uint32 defl)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/* skip searching with suffix num, if suffix num < 0 */
    if(!(soc_attached(unit)) || !(SOC_IS_DNX(unit) ||  SOC_IS_DNXF(unit)) || (num != INVALID_SOC_PROPERTY_SUFFIX_NUM ))
#endif
    {
        /* "name_'suffix'#" */
        /* check length of <name>_<suffix><num>\0 */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s_%s%1d", name, suffix, num) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s_%s%1d. "
                          "Max soc property length:%d\n"),
               name, suffix, num, SOC_PROPERTY_NAME_MAX));
            return defl;
        }

        /* try "name_'suffix'#" per port*/
        if ((s = soc_property_port_get_str(unit, port, prop)) != NULL) {
            return _shr_ctoi(s);
        }

        /* try "name_'suffix'#" */
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return _shr_ctoi(s);
        }
    }
    /* "name_'suffix'" */
    /* check length of <name>_<suffix>\0 */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_%s", name, suffix) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_%s. "
                      "Max soc property length:%d\n"),
           name, suffix, SOC_PROPERTY_NAME_MAX));
        return defl;
    }
    /* try "name_'suffix'" per port */
    if ((s = soc_property_port_get_str(unit, port, prop)) != NULL) {
        return _shr_ctoi(s);
    }

    /* try "name_'suffix'"*/
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return _shr_ctoi(s);
    }


    /* try plain "name" per port*/
    if ((s = soc_property_port_get_str(unit, port, name)) != NULL) {
        return _shr_ctoi(s);
    }

    /* try plain "name" */
    if ((s = soc_property_get_str(unit, name)) != NULL) {
        return _shr_ctoi(s);
    }

    return defl;
}


uint32
soc_property_port_suffix_num_match_port_get(int unit, soc_port_t port,
                                 int num, const char *name,
                                 const char *suffix, uint32 defl)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

    /* "name_'suffix'" */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_%s_%d", name, suffix, port) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_%s. "
                      "Max soc property length:%d\n"),
           name, suffix, SOC_PROPERTY_NAME_MAX));
        return defl;
    }
    /* only check "name_'suffix'" per port */
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return _shr_ctoi(s);
    }

    return defl;

}


/*
 * Function:
 *      soc_property_port_suffix_num_get_str
 * Purpose:
 *      Retrieve a per-port, per-enumerated suffix global
 *      configuration string value.
 * Parameters:
 *      unit - unit number
 *      port - Zero-based port number.
 *      num - enumerated suffix for which property is applicable
 *      name - base name of numeric property to look up
 *      suffix - suffix of numeric property to look up
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"0_port  enumerated suffix name per port
 *              name_"suffix"0       enumerated suffix name
 *              name_port            plain name per port
 *              name                 plain name
 *      If none are found then the NULL is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */

char*
soc_property_port_suffix_num_get_str(int unit, soc_port_t port,
                                 int num, const char *name,
                                 const char *suffix)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

#if defined(BCM_DNX_SUPPORT) || defined(BCM_DNXF_SUPPORT)
/* skip searching with suffix num, if suffix num < 0 */
    if(!(soc_attached(unit)) || !(SOC_IS_DNX(unit) ||  SOC_IS_DNXF(unit)) || (num != INVALID_SOC_PROPERTY_SUFFIX_NUM ))
#endif
    {
        /* "name_'suffix'#" */
        /* check length of <name>_<suffix><num>\0 */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s_%s%1d", name, suffix, num) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s_%s%1d. "
                          "Max soc property length:%d\n"),
               name, suffix, num, SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        /* try "name_'suffix'#" per port*/
        if ((s = soc_property_port_get_str(unit, port, prop)) != NULL) {
            return s;
        }

        /* try "name_'suffix'#" */
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return s;
    }
    }

    /* "name_'suffix'" */
    /* check length of <name>_<suffix>\0 */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_%s", name, suffix) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_%s. "
                      "Max soc property length:%d\n"),
           name, suffix, SOC_PROPERTY_NAME_MAX));
        return NULL;
    }
    /* try "name_'suffix'" per port */
    if ((s = soc_property_port_get_str(unit, port, prop)) != NULL) {
        return s;
    }
    /* try "name_'suffix'"*/
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return s;
    }

    /* try plain "name" per port*/
    if ((s = soc_property_port_get_str(unit, port, name)) != NULL) {
        return s;
    }

    /* try plain "name" */
    if ((s = soc_property_get_str(unit, name)) != NULL) {
        return s;
    }

    return NULL;
}


/*
 * Function:
 *      soc_property_uc_get_str
 * Purpose:
 *      Retrieve a per-uc global configuration value
 * Parameters:
 *      unit - unit number
 *      uc - Microcontroller number. 0 is PCI Host.
 *      name - base name of numeric property to look up
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"   suffix is pci | uc0 | uc1 ...
 *              name            plain name
 *      If none are found then NULL is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */
char *
soc_property_uc_get_str(int unit, int uc,
                          const char *name)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

    /* try "name_uc%d" for explicit match first */
    if (0 == uc) {
        /* check length of <name>_pci\0 */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s_pci", name) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s_pci. "
                          "Max soc property length:%d\n"),
               name, SOC_PROPERTY_NAME_MAX));
            return NULL;

        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return(s);
        }
    } else {

        /* check length of <name>_uc<uc>\0 */
        if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                         "%s_uc%d", name, (uc - 1)) >= SOC_PROPERTY_NAME_MAX) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "Unsupported soc_property length for %s_uc%d. "
                          "Max soc property length:%d\n"),
               name, (uc - 1), SOC_PROPERTY_NAME_MAX));
            return NULL;
        }
        if ((s = soc_property_get_str(unit, prop)) != NULL) {
            return(s);
        }
    }

    /* try plain "name" */
    if ((s = soc_property_get_str(unit, name)) != NULL) {
        return(s);
    }

    return(NULL);
}

/*
 * Function:
 *      soc_property_uc_get
 * Purpose:
 *      Retrieve a per-uc global configuration value
 * Parameters:
 *      unit - unit number
 *      uc - Microcontroller number. 0 is PCI Host.
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"   suffix is pci | uc0 | uc1 ...
 *              name            plain name
 *      If none are found then the default value in defl is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */
uint32
soc_property_uc_get(int unit, int uc,
                    const char *name, uint32 defl)
{
    char        *s;

    s = soc_property_uc_get_str(unit, uc, name);
    if (s) {
      return _shr_ctoi(s);
    }

    return defl;
}

/*
 * Function:
 *      soc_property_ci_get_str
 * Purpose:
 *      Retrieve a per-ci global configuration value
 * Parameters:
 *      unit - unit number
 *      ci - Chip Intergface Number.
 *      name - base name of numeric property to look up
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"   suffix is ci0 | ci1 ...
 *              name            plain name
 *      If none are found then NULL is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */
char *
soc_property_ci_get_str(int unit, int ci,
                          const char *name)
{
    char        *s;
    char        prop[SOC_PROPERTY_NAME_MAX];

    /* try "name_ci%d" for explicit match first */
    /* check length of <name>_ci<ci>\0 */
    if (sal_snprintf(prop, SOC_PROPERTY_NAME_MAX,
                     "%s_ci%d", name, ci) >= SOC_PROPERTY_NAME_MAX) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
          (BSL_META_U(unit,
                      "Unsupported soc_property length for %s_ci%d. "
                      "Max soc property length:%d\n"),
           name, ci, SOC_PROPERTY_NAME_MAX));
        return NULL;
    }
    if ((s = soc_property_get_str(unit, prop)) != NULL) {
        return(s);
    }
    /* try plain "name" */
    if ((s = soc_property_get_str(unit, name)) != NULL) {
        return(s);
    }

    return(NULL);
}

/*
 * Function:
 *      soc_property_ci_get
 * Purpose:
 *      Retrieve a per-ci global configuration value
 * Parameters:
 *      unit - unit number
 *      ci - Chip Intergface Number.
 *      name - base name of numeric property to look up
 *      defl - default value of numeric property
 * Notes:
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"   suffix is ci0 | ci1 ...
 *              name            plain name
 *      If none are found then the default value in defl is returned.
 *      See soc_property_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */
uint32
soc_property_ci_get(int unit, int ci,
                    const char *name, uint32 defl)
{
    char        *s;

    s = soc_property_ci_get_str(unit, ci, name);
    if (s) {
      return _shr_ctoi(s);
    }

    return defl;
}

/*
 * Function:
 *      soc_property_ci_get_csv
 * Purpose:
 *      Retrieve a per-ci global configuration comma-separated value in an array.
 * Parameters:
 *      unit - unit number
 *      ci - Chip Intergface Number.
 *      name - base name of numeric property to look up
 *      val_array - Integer array to put the values
 *      val_max - Maximum number of elements to put into the array
 * Notes:
 *      Returns the count of values read.
 *      The variable name is lookup up with a number of suffixes
 *      until it is found.
 *              name_"suffix"   suffix is ci0 | ci1 ...
 *              name            plain name
 *      If none are found then the default value in defl is returned.
 *      See soc_property_ci_get_str for additional suffix handling.
 *      The string is parsed as a C-style numeric constant.
 */
int
soc_property_ci_get_csv(int unit, int ci, const char *name,
                          int val_max, int *val_array)
{
    char *str, suffix;
    int count;

    str = soc_property_ci_get_str(unit, ci, name);
    if (str == NULL) {
        return 0;
    }

    count = 0;
    for (count = 0; count < val_max; count++) {
        str = _str_to_val(str, &val_array[count], 0, &suffix);
        if (suffix == ',') {
            str++;
        } else {
            count++;
            break;
        }
    }

    return count;
}

soc_block_name_t        soc_block_port_names[] =
        SOC_BLOCK_PORT_NAMES_INITIALIZER;
soc_block_name_t        soc_block_names[] =
        SOC_BLOCK_NAMES_INITIALIZER;


soc_block_name_t        soc_dpp_block_port_names[] =
        SOC_BLOCK_DPP_PORT_NAMES_INITIALIZER;
soc_block_name_t        soc_dpp_block_names[] =
        SOC_BLOCK_DPP_NAMES_INITIALIZER;

char *
soc_block_port_name_lookup_ext(soc_block_t blk, int unit)
{
    int         i;

    {
        for (i = 0; soc_block_port_names[i].blk != SOC_BLK_NONE; i++) {
            if (soc_block_port_names[i].blk == blk) {
                if (blk == SOC_BLK_GXPORT) {
                    if (SOC_IS_TD_TT(unit) && !SOC_IS_HELIX5(unit)
                        && !SOC_IS_HURRICANE4(unit)) {
                        return "xlport";
                    }
                } else if (blk == SOC_BLK_BSAFE) {
                    if (SOC_IS_TD_TT(unit)) {
                        return "otpc";
                    }
                }
                return soc_block_port_names[i].name;
            }
        }
    }
    return "?";
}

char *
soc_block_name_lookup_ext(soc_block_t blk, int unit)
{
    int         i;

#if defined(BCM_SAND_SUPPORT)
    if (SOC_IS_SAND(unit)) {
        for (i = 0; soc_dpp_block_names[i].blk != SOC_BLK_NONE; i++) {
            if (soc_dpp_block_names[i].blk == blk) {
                return soc_dpp_block_names[i].name;
            }
        }
    }
    else
#endif

    {
        for (i = 0; soc_block_names[i].blk != SOC_BLK_NONE; i++) {
            if (soc_block_names[i].blk == blk) {
                if (blk == SOC_BLK_GXPORT) {
                    if (SOC_IS_TD_TT(unit) && !SOC_IS_HELIX5(unit)
                        && !SOC_IS_HURRICANE4(unit)) {
                        return "xlport";
                    }
                } else if (blk == SOC_BLK_BSAFE) {
                    if (SOC_IS_TD_TT(unit)) {
                        return "otpc";
                    }
                }
                return soc_block_names[i].name;
            }
        }
    }
    return "?";
}

soc_block_t
soc_block_port_name_match(char *name)
{
    int         i;

    
    for (i = 0; soc_block_port_names[i].blk != SOC_BLK_NONE; i++) {
        if (sal_strcmp(soc_block_port_names[i].name, name) == 0) {
            return soc_block_port_names[i].blk;
        }
    }
    return SOC_BLK_NONE;
}

soc_block_t
soc_block_name_match(int unit, char *name)
{
    int         i;

#if defined(BCM_SAND_SUPPORT)
    if (SOC_IS_SAND(unit))
    {
        for (i = 0; soc_dpp_block_names[i].blk != SOC_BLK_NONE; i++)
        {
            if (sal_strncasecmp(soc_dpp_block_names[i].name, name, strlen(soc_dpp_block_names[i].name)) == 0)
            {
                return soc_dpp_block_names[i].blk;
            }
        }
    }
    else
#endif
    {
        
        for (i = 0; soc_block_names[i].blk != SOC_BLK_NONE; i++) {
            if (sal_strcmp(soc_block_names[i].name, name) == 0) {
                return soc_block_names[i].blk;
            }
        }
    }
    return SOC_BLK_NONE;
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT)
/*
 * Function:
 *      soc_xport_type_update
 * Purpose:
 *      Rewrite the SOC control port structures to reflect the 10G port mode,
 *      and reinitialize the port
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      mode - (0 fo iee,1 for higig variants,encap value for other modes )
 * Returns:
 *      None.
 * Notes:
 *      Must pause linkscan and take COUNTER_LOCK before calling this.
 */
void
soc_xport_type_update(int unit, soc_port_t port, int mode)
{
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    soc_info_t          *si;
    soc_port_t          it_port;
    int                 blk, blktype;
    int                 port_speed;

    si = &SOC_INFO(unit);

    /* We need to lock the SOC structures until we finish the update */
    SOC_CONTROL_LOCK(unit);

    port_speed = (0 != si->port_init_speed[port]) ?
                 si->port_init_speed[port] : si->port_speed_max[port];
    if (mode == 1) {
        SOC_PBMP_PORT_ADD(si->st.bitmap, port);
        SOC_PBMP_PORT_ADD(si->hg.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ce.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->cpri.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->rsvd4.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->roe.bitmap, port);
    } else if( mode == SOC_ENCAP_CPRI) {
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
        SOC_PBMP_PORT_ADD(si->cpri.bitmap, port);
        SOC_PBMP_PORT_ADD(si->roe.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ce.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->rsvd4.bitmap, port);
    } else if( mode == SOC_ENCAP_RSVD4) {
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
        SOC_PBMP_PORT_ADD(si->rsvd4.bitmap, port);
        SOC_PBMP_PORT_ADD(si->roe.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ce.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->cpri.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
    } else {
        SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
        if ((port_speed >= 100000) && (si->port_speed_max[port] >= 100000)){
            SOC_PBMP_PORT_ADD(si->ce.bitmap, port);
        } else if ((port_speed < 10000) && (si->port_speed_max[port] < 10000)) {
            SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
        } else {
            SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
        }
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->cpri.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->rsvd4.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->roe.bitmap, port);
    }

#define RECONFIGURE_PORT_TYPE_INFO(ptype) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, it_port) { \
        si->ptype.port[si->ptype.num++] = it_port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = it_port; \
        } \
        if (it_port > si->ptype.max) { \
            si->ptype.max = it_port; \
        } \
    }

    /* Recalculate port type data */
    RECONFIGURE_PORT_TYPE_INFO(ether);
    RECONFIGURE_PORT_TYPE_INFO(st);
    RECONFIGURE_PORT_TYPE_INFO(hg);
    RECONFIGURE_PORT_TYPE_INFO(xe);
    RECONFIGURE_PORT_TYPE_INFO(ge);
    RECONFIGURE_PORT_TYPE_INFO(ce);
    RECONFIGURE_PORT_TYPE_INFO(cpri);
    RECONFIGURE_PORT_TYPE_INFO(rsvd4);
#undef  RECONFIGURE_PORT_TYPE_INFO

    soc_dport_map_update(unit);

    /* Resolve block membership for registers */
    if (SOC_IS_FB_FX_HX(unit)) { /* Not HBS */
        for (blk = 0; SOC_BLOCK_INFO(unit, blk).type >= 0; blk++) {
            blktype = SOC_BLOCK_INFO(unit, blk).type;
            switch (blktype) {
            case SOC_BLK_IPIPE:
                if (!mode) {
                    SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
                } else {
                    SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk], port);
                }
                break;
            case SOC_BLK_IPIPE_HI:
                if (mode) {
                    SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
                } else {
                    SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk], port);
                }
                break;
            case SOC_BLK_EPIPE:
                if (!mode) {
                    SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
                } else {
                    SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk], port);
                }
                break;
            case SOC_BLK_EPIPE_HI:
                if (mode) {
                    SOC_PBMP_PORT_ADD(si->block_bitmap[blk], port);
                } else {
                    SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk], port);
                }
                break;
            }
        }
    }

    /* Release SOC structures lock */
    SOC_CONTROL_UNLOCK(unit);

#else /* BCM_FIREBOLT_SUPPORT || BCM_BRADLEY_SUPPORT */
    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(port);
    COMPILER_REFERENCE(mode);
#endif
}

/*
 * Function:
 *      soc_xport_type_mode_update
 * Purpose:
 *      Rewrite the SOC control port structures to reflect the current port
 *      mode, and reinitialize the port.
 *
 *      This function is simular to soc_xport_type_update() but the differnce
 *      here is the encap type updated will check higig encap modes of
 *      {HG/HG2/HG2-LITE/IEEE} for different encap bitmap assignment.
 * Parameters:
 *      unit - XGS unit #.
 *      port - XGS port # on unit.
 *      hg_mode - (IEEE/HG/HG2/HG2-LITE)
 * Returns:
 *      None.
 * Notes:
 *      Must pause linkscan and take COUNTER_LOCK before calling this.
 */
void
soc_xport_type_mode_update(int unit, soc_port_t port, int hg_mode)
{
    soc_info_t          *si;
    soc_port_t          it_port;

    si = &SOC_INFO(unit);

    /* We need to lock the SOC structures until we finish the update */
    SOC_CONTROL_LOCK(unit);

    if (hg_mode != SOC_ENCAP_IEEE) {
        SOC_PBMP_PORT_ADD(si->st.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, port);
        /* special process for HG-Lite support to avoid wrong PBMP assignment.
        *
        * Note :
        *  the 'to_hg_port' for this special design must carry proper encap mode
        *  instead of TRUE/FALSE only.
        */
        if (hg_mode == SOC_ENCAP_HIGIG2_LITE) {
            SOC_PBMP_PORT_ADD(si->hl.bitmap, port);
            /* for XE or GE no more pbmp update,
             *  for HG remove HG and add XE
             */
            if (IS_HG_PORT(unit, port)) {
                SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
                SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
            }
        } else {
            /* for HG/HG2 case (GE port expected not allowed here) */
            SOC_PBMP_PORT_ADD(si->hg.bitmap, port);
            SOC_PBMP_PORT_REMOVE(si->hl.bitmap, port);
            SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
        }
    } else {
        /* To IEEE mode */
        if (IS_HG_PORT(unit, port)) {
            SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
        }
        SOC_PBMP_PORT_ADD(si->ether.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->hg.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->hl.bitmap, port);
    }

#define RECONFIGURE_PORT_TYPE_INFO(ptype) \
    si->ptype.num = 0; \
    si->ptype.min = si->ptype.max = -1; \
    PBMP_ITER(si->ptype.bitmap, it_port) { \
        si->ptype.port[si->ptype.num++] = it_port; \
        if (si->ptype.min < 0) { \
            si->ptype.min = it_port; \
        } \
        if (it_port > si->ptype.max) { \
            si->ptype.max = it_port; \
        } \
    }

    /* Recalculate port type data */
    RECONFIGURE_PORT_TYPE_INFO(ether);
    RECONFIGURE_PORT_TYPE_INFO(st);
    RECONFIGURE_PORT_TYPE_INFO(hg);
    RECONFIGURE_PORT_TYPE_INFO(xe);
#undef  RECONFIGURE_PORT_TYPE_INFO

    soc_dport_map_update(unit);

    /* Release SOC structures lock */
    SOC_CONTROL_UNLOCK(unit);
}

/*
 * Function:
 *      soc_pci_burst_enable
 * Purpose:
 *      Turn on read/write bursting in the cmic
 * Parameters:
 *      unit - unit number
 *
 * Our hardware by default does not accept burst transactions.
 * These transactions need to be enabled in the CMIC_CONFIG register
 * before a burst is performed or the cmic will hang.
 *
 * This code should be called directly after endian configuration to
 * enable bursting as soon as possible.
 */

void
soc_pci_burst_enable(int unit)
{
    uint32 reg;

    /* Make sure these reads/writes are not combined */
    sal_usleep(1000);

    /* Enable Read/Write bursting in the CMIC */
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(unit, soc_feature_cmicm)) {
        return;
    }
#endif /* CMICM Support */
#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
        return;
    }
#endif /* CMICX Support */

    reg = soc_pci_read(unit, CMIC_CONFIG);
    reg |= (CC_RD_BRST_EN | CC_WR_BRST_EN);
    soc_pci_write(unit, CMIC_CONFIG, reg);

    /* Make sure our previous write is not combined */
    sal_usleep(1000);
}
/*
 * Function:    soc_endian_get
 * Purpose:     Returns the required endian configuration for this device.
 *              For iProc-based switch designs the CMIC PIO endianness
 *              configuration is shared for the internal and external
 *              CPU hosts, and in this case we force the internal PIO
 *              interface to little endian and perform any required PCI
 *              endianness conversion in the iProc PCI interfce.
 *
 * Parameters:  dev     - device handle
 *              pio     - (OUTPUT) receives the PIO endian configuration.
 *              packet  - (OUTPUT) receives the PACKET endian configuration.
 *              other   - (OUTPUT) receives the OTHER endian configuration.
 *
 * Returns:     SOC_E_XXX
 */

void
soc_endian_get(int unit, int *pio, int *packet, int *other)
{
    soc_cm_get_endian(unit, pio, packet, other);

#ifdef BCM_IPROC_SUPPORT
    if (soc_feature(unit, soc_feature_iproc)) {
        if ((soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE) && *pio) {
            /*
             * If big_pio is set, then we assume that the iProc
             * PCI bridge will byte-swap all access types,
             * i.e. both PIO and DMA access.
             *
             * In order to avoid double-swapping, we then need to
             * invert the DMA endianness setting in the CMIC.
             */
            *other ^= 1;
            *packet ^= 1;
            *pio ^= 1;
        }
    }
#endif
#if defined(CMIC_SOFT_BYTE_SWAP)
    *pio ^= 1;
#endif /* CMIC_SOFT_BYTE_SWAP */
}

/*
 * Function:
 *      soc_endian_config (private)
 * Purpose:
 *      Utility routine for configuring CMIC endian select register.
 * Parameters:
 *      unit - unit number
 */

void
soc_endian_config(int unit)
{
    uint32          val = 0;
    int             big_pio, big_packet, big_other;
#if defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICX_SUPPORT)
    int cmc, cmc_start, cmc_end;
#endif

    soc_endian_get(unit, &big_pio, &big_packet, &big_other);

#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        int y;
        cmc_start = 0;
        cmc_end = SOC_CMCS_NUM(unit) - 1;


        for (cmc = cmc_start; cmc <= cmc_end; cmc++) {
            if (big_packet) {
                for(y = 0; y < CMICX_N_DMA_CHAN; y++) {
                    soc_cmicx_pktdma_data_endian_set(unit, cmc, y, big_packet);
                }
                /* Set Tx header endian same as packet DMA endian */
                soc_cmicx_pktdma_hdr_endian_set(unit, 0, 0, big_packet);
            }
            if (big_other) {
                /* Set Packet DMA Descriptor and EP_TO_CPU_HDR Endianness */
                for(y = 0; y < CMICX_N_DMA_CHAN; y++) {
                    soc_cmicx_pktdma_desc_endian_set(unit, cmc, y, big_other);
                    /*
                     * For DMA channel0 in CMC0 which is used for transmission,
                     * header endian setting should be same as packet DMA endian.
                     * For other CMCs' channels which are used for receive,
                     * EP_TO_CPU_HDR endian setting should be same as DCB endian.
                     */
                    if (cmc != 0 || y != 0) {
                        /* Set Rx EP_TO_CPU_HDR endian same as DCB endian */
                        soc_cmicx_pktdma_hdr_endian_set(unit, cmc, y, big_other);
                    }
                }
#ifdef BCM_SBUSDMA_SUPPORT
                if (soc_feature(unit, soc_feature_sbusdma)) {
                    uint32 num_sbusdma;
                    num_sbusdma = soc_sbusdma_cmc_max_num_ch_get(unit);
                    for(y = 0; y < num_sbusdma; y++) {
                        soc_sbusdma_ch_endian_set(unit, cmc, y, 1);
                    }
                }
#endif
#ifdef BCM_FIFODMA_SUPPORT
                for(y = 0; y < CMICX_N_FIFODMA_CHAN; y++) {
                    soc_fifodma_ch_endian_set(unit, y, 1);
                }
#endif
            }
        }
    }
#endif

#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
        int y;
        uint32 pio_endian = (big_pio) ? 0x01010101 : 0;

        /*
         * Configure endianness for PCI PIO accesss.
         *
         * Note that this register has no effect for iProc-based
         * switch designs.
         */
        soc_pci_write(unit, CMIC_COMMON_PCIE_PIO_ENDIANESS_OFFSET, pio_endian);

#ifdef BCM_IPROC_SUPPORT
        if (soc_feature(unit, soc_feature_iproc)) {
           WRITE_CMIC_COMMON_UC0_PIO_ENDIANESSr(unit, pio_endian);
        }
#endif
        if (soc_feature(unit, soc_feature_cmicm_multi_dma_cmc)) {
            cmc_start = 0;
            cmc_end = SOC_CMCS_NUM(unit) - 1;
        } else {
            cmc_start = SOC_PCI_CMC(unit);
            cmc_end = cmc_start;
        }

        for (cmc = cmc_start; cmc <= cmc_end; cmc++) {
            if (big_packet) {
                for(y = 0; y < N_DMA_CHAN; y++) {
                    val = soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,y));
                    val |= PKTDMA_BIG_ENDIAN;
                    soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,y), val);
                }
            }
            if (big_other) {
                /* Set Packet DMA Descriptor Endianness */
                for(y = 0; y < N_DMA_CHAN; y++) {
                    val = soc_pci_read(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,y));
                    val |= PKTDMA_DESC_BIG_ENDIAN;
                    soc_pci_write(unit, CMIC_CMCx_CHy_DMA_CTRL_OFFSET(cmc,y), val);
                }
#ifdef BCM_SBUSDMA_SUPPORT
                if (soc_feature(unit, soc_feature_sbusdma)) {
                    uint32 sbusdma_chan_max  = CMIC_CMCx_SBUSDMA_CHAN_MAX;

#ifdef BCM_TRIDENT3_SUPPORT
                    uint32 ifa_cc_num_sbusdma = soc_property_get(unit,
                                                spn_IFA_CC_NUM_SBUS_DMA, 2);
                    if ((SOC_IS_TRIDENT3(unit)) && (ifa_cc_num_sbusdma > 2)) {
                        /* CMC_0 2 SBUS DMA Channels are used by IFA CC Eapp on M0 */
                        sbusdma_chan_max = (CMIC_CMCx_SBUSDMA_CHAN_MAX - 2);
                    }
#endif

                    for(y=0; y < sbusdma_chan_max ; y++) {
                        /* dma/slam */
                        val = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_REQUEST(cmc, y));
                        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &val,
                                HOSTMEMWR_ENDIANESSf, 1);
                        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_REQUESTr, &val,
                                HOSTMEMRD_ENDIANESSf, 1);
                        soc_pci_write(unit, CMIC_CMCx_SBUSDMA_CHy_REQUEST(cmc,y), val);

                        val = soc_pci_read(unit, CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, y));

                        soc_reg_field_set(unit, CMIC_CMC0_SBUSDMA_CH0_CONTROLr,
                                     &val, DESCRIPTOR_ENDIANESSf, 1);
                        soc_pci_write(unit,  CMIC_CMCx_SBUSDMA_CHy_CONTROL(cmc, y), val);
                    }
                } else
#endif
                {
                    /* Set SLAM DMA Endianness */
                    val = soc_pci_read(unit, CMIC_CMCx_SLAM_DMA_CFG_OFFSET(cmc));
                    val |= SLDMA_BIG_ENDIAN;
                    soc_pci_write(unit, CMIC_CMCx_SLAM_DMA_CFG_OFFSET(cmc), val);

                    /* Set STATS DMA Endianness */
                    val = soc_pci_read(unit, CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc));
                    val |= STDMA_BIG_ENDIAN;
                    soc_pci_write(unit, CMIC_CMCx_STAT_DMA_CFG_OFFSET(cmc), val);

                    /* Set TABLE DMA Endianness */
                    val = soc_pci_read(unit, CMIC_CMCx_TABLE_DMA_CFG_OFFSET(cmc));
                    val |= TDMA_BIG_ENDIAN;
                    soc_pci_write(unit, CMIC_CMCx_TABLE_DMA_CFG_OFFSET(cmc), val);
                }

                /* Set FIFO DMA Endianness */
                for(y = 0; y < N_DMA_CHAN; y++) {
                    val = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc,y));
                    val |= FIFODMA_BIG_ENDIAN;
                    soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_CFG_OFFSET(cmc,y), val);
                }

                /* Set Cross Coupled Memory Endianness */
                /* NA for PCI */
            }
        }
    } else
#endif
    {
        if (big_pio) {
            val |= ES_BIG_ENDIAN_PIO;
        }

        if (big_packet) {
            val |= ES_BIG_ENDIAN_DMA_PACKET;
        }

        if (big_other) {
            val |= ES_BIG_ENDIAN_DMA_OTHER;
        }

        if ((SOC_IS_HAWKEYE(unit) || SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) &&
             soc_property_get(unit, spn_EB2_2BYTES_BIG_ENDIAN, 0)) {
            val |= EN_BIG_ENDIAN_EB2_2B_SEL;
        }
#ifdef BCM_SHADOW_SUPPORT
        if (SOC_IS_SHADOW(unit) && soc_property_get(unit, spn_DEVICE_EB_VLI, 0)) {
            val = 0x30000030;
        }
#endif
        soc_pci_write(unit, CMIC_ENDIAN_SELECT, val);
    }
}

void
soc_pci_ep_config(int unit, int pcie)
{
#if defined(BCM_IPROC_SUPPORT) && defined(BCM_CMICM_SUPPORT)
    uint32 rval;
    int pci_num = pcie;
    int base_cmc = SOC_PCI_CMC(unit);
    int num_of_cmc = 1;
    int cmc;
    char *propkey;
    int configure_one_to_one_remap[CMIC_CMC_NUM_MAX] = {TRUE, TRUE, TRUE};
    uint32 remap_entries_reserved;

    if(SOC_IS_DPP(unit)){
        num_of_cmc = SOC_PCI_CMCS_NUM(unit);
    }

    if (soc_feature(unit, soc_feature_iproc) &&
        soc_feature(unit, soc_feature_cmicm) &&
        (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE)) {

        if (pcie == -1) { /* Auto-Detect PCI # */
            if ((soc_cm_get_bus_type(unit) & SOC_DEV_BUS_ALT)) {
                pci_num = 1;
            } else {
                pci_num = 0;
            }
        }
        /* consider remap_entries_reserved!=0 only for cmc=1 and pci_cmc=0 */
        /* this is the only constelation that the remap management is allowed */
        propkey = spn_HOST_MEMORY_ADDRESS_REMAP_ENTRIES_RESERVED_CMC;
        for(cmc=base_cmc;cmc<num_of_cmc+base_cmc;cmc++)
        {
            remap_entries_reserved = soc_property_port_get(unit, cmc, propkey, 0);
            if(remap_entries_reserved != 0)
            {
               if ((cmc == 1) && (base_cmc == 0))
               {
                   configure_one_to_one_remap[cmc] = FALSE;
               }
            }
        }

        if (pci_num == 0) {
            for(cmc=base_cmc;cmc<num_of_cmc+base_cmc;cmc++){
                if ((SOC_IS_JERICHO(unit)) && (configure_one_to_one_remap[cmc] == FALSE))
                {
                    /** don't remap at all here. it will be made once allocating new host_buff and setting HOSTMEM_START_ADDRESS */
                    /** see function _soc_mem_sbus_fifo_dma_start_memreg and _soc_mem_address_remap_allocation_init  */
                }
                else
                {
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc), 0x144D2450);
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc), 0x19617595);
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc), 0x1E75C6DA);
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(cmc), 0x1f);
                }

            }
        } else {
            for(cmc=base_cmc;cmc<num_of_cmc+base_cmc;cmc++){
                if ((SOC_IS_JERICHO(unit)) && (configure_one_to_one_remap[cmc] == FALSE))
                {
                    /** don't remap at all here. it will be made once allocating new host_buff and setting HOSTMEM_START_ADDRESS */
                    /** see function _soc_mem_sbus_fifo_dma_start_memreg and _soc_mem_address_remap_allocation_init */
                }
                else
                {
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_0_OFFSET(cmc), 0x248e2860);
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_1_OFFSET(cmc), 0x29a279a5);
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_2_OFFSET(cmc), 0x2eb6caea);
                    soc_pci_write(unit, CMIC_CMCx_HOSTMEM_ADDR_REMAP_3_OFFSET(cmc), 0x2f);
                }

                rval = soc_pci_read(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc));
                soc_reg_field_set(unit, CMIC_CMC0_PCIE_MISCELr,
                                &rval, MSI_ADDR_SELf, 1);
                soc_pci_write(unit, CMIC_CMCx_PCIE_MISCEL_OFFSET(cmc), rval);
            }
        }
    }
#endif
}

/*
 * Function:
 *      soc_autoz_set
 * Purpose:
 *      Set the autoz bit in the active mac
 * Parameters:
 *      unit - SOC unit #
 *      port - port number on device.
 *      enable - Boolean value to set state
 * Returns:
 *      SOC_E_XXX
 */
int
soc_autoz_set(int unit, soc_port_t port, int enable)
{
    /* XGS (Hercules): AutoZ not supported */
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_autoz_get
 * Purpose:
 *      Get the state of the autoz bit in the active MAC
 * Parameters:
 *      unit - SOC unit #
 *      port - port number on device.
 *      enable - (OUT) Boolean pointer to return state
 * Returns:
 *      SOC_E_XXX
 */
int
soc_autoz_get(int unit, soc_port_t port, int *enable)
{
    /* XGS (Hercules): AutoZ not supported */
    *enable = 0;
    return SOC_E_NONE;
}

#endif /* defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) */


#if defined(BCM_ESW_SUPPORT) || defined(BCM_SAND_SUPPORT) || defined(PORTMOD_SUPPORT)

int
soc_event_register(int unit, soc_event_cb_t cb, void *userdata)
{
    soc_control_t       *soc;
    soc_event_cb_list_t *curr, *prev;


    /* Input validation */
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    if (NULL == cb) {
        return SOC_E_PARAM;
    }

    soc = SOC_CONTROL(unit);
    curr = prev = soc->ev_cb_head;

    if (NULL == curr) { /* Fist time a call back registered */
        curr = (soc_event_cb_list_t *)sal_alloc(sizeof(soc_event_cb_list_t),
                                               "event call back linked list");
        if (NULL == curr) {
            return SOC_E_MEMORY;
        }
        curr->cb = cb;
        curr->userdata = userdata;
        curr->next = NULL;
        soc->ev_cb_head = curr;
    } else { /* Not a first registered callback */
        while (NULL != curr) {
            if ((curr->cb == cb) && (curr->userdata == userdata)) {
                /* call back with exact same userdata exists - nothing to be done */
                return SOC_E_NONE;
            }
            prev = curr;
            curr = prev->next;
        }
        curr = (soc_event_cb_list_t *)sal_alloc(sizeof(soc_event_cb_list_t),
                                               "event call back linked list");
        if (NULL == curr) {
            return SOC_E_MEMORY;
        }
        curr->cb = cb;
        curr->userdata = userdata;
        curr->next = NULL;
        prev->next = curr;
    }

    return SOC_E_NONE;
}

int
soc_event_unregister(int unit, soc_event_cb_t cb, void *userdata)
{
    soc_control_t       *soc;
    soc_event_cb_list_t *curr, *prev;

    /* Input validation */
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

    if (NULL == cb) {
        return SOC_E_PARAM;
    }

    soc = SOC_CONTROL(unit);
    curr = prev = soc->ev_cb_head;

    while (NULL != curr) {
        if (curr->cb == cb) {
            if ((NULL != userdata) && (curr->userdata != userdata)) {
                /* No match keep searching */
                prev = curr;
                curr = curr->next;
                continue;
            }
            /* if cb & userdata matches or userdata is NULL -> delete */
            if (curr == soc->ev_cb_head) {
                soc->ev_cb_head = curr->next;
                sal_free((void *)curr);
                break;
            }
            prev->next = curr->next;
            sal_free((void *)curr);
            break;
        }
        prev = curr;
        curr = curr->next;
    }

    return (SOC_E_NONE);
}

int
soc_event_generate(int unit,  soc_switch_event_t event, uint32 arg1,
                   uint32 arg2, uint32 arg3)
{
    soc_control_t       *soc;
    soc_event_cb_list_t *curr, *curr_next;
    soc_event_cb_t      curr_cb;

    /* Input validation */
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

#ifdef _SOC_SER_ENABLE_CLI_DBG
    if ((event == SOC_SWITCH_EVENT_PARITY_ERROR) &&
        SOC_MEM_IS_VALID(unit, arg2)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                            "unit %d SOC_SWITCH_EVENT_PARITY_ERROR, "
                            "correction_type = %d mem %s, index %d\n"),
                 unit, arg1, SOC_MEM_NAME(unit, arg2), arg3));
    }
    if ((event == SOC_SWITCH_EVENT_PARITY_ERROR) &&
        (arg1 == SOC_SWITCH_EVENT_DATA_ERROR_LOG)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                            "unit %d SOC_SWITCH_EVENT_DATA_ERROR_LOG, "
                            "log_id = %0d, arg3 = %0d \n"),
                 unit, arg2, arg3));
    }
#endif /* _SOC_SER_ENABLE_CLI_DBG */

    soc = SOC_CONTROL(unit);
    curr = soc->ev_cb_head;
    if (NULL != curr) {
        curr_cb = curr->cb;
        /* coverity[copy_paste_error] */
        curr_next = curr->next;

        while ((NULL != curr) && (curr->next == curr_next)) {
            curr_cb(unit, event, arg1, arg2, arg3, curr->userdata);

            if (curr_next != NULL) {
                curr_cb = curr_next->cb;
                curr_next = curr_next->next;
            }
            curr = curr->next;
        }
    }

    return (SOC_E_NONE);
}

void
soc_event_assert(const char *expr, const char *file, int line)
{
    uint32 arg1, arg3 = 0;

    arg1 = PTR_TO_INT(file);
#ifdef PTRS_ARE_64BITS
    arg3 = PTR_HI_TO_INT(file);
#endif
    soc_event_generate(0, SOC_SWITCH_EVENT_ASSERT_ERROR, arg1, line, arg3);
   _default_assert(expr, file, line);
}

#endif /*defined(BCM_ESW_SUPPORT) || defined(PORTMOD_SUPPORT) */


#if defined(BCM_XGS3_SWITCH_SUPPORT) || defined(BCM_PETRA_SUPPORT)
/*
 * LCPLL lock check for FBX devices
 */
void
soc_xgxs_lcpll_lock_check(int unit)
{
    if ((!SAL_BOOT_PLISIM) && (!SAL_BOOT_QUICKTURN) &&
        soc_feature(unit, soc_feature_xgxs_lcpll)) {
        uint32 val;
        int pll_lock_usec = 500000;
        int locked = 0;
        int retry = 3;
        soc_timeout_t to;

        /* Check if LCPLL locked */
        while (!locked && retry--) {
            soc_timeout_init(&to, pll_lock_usec, 0);
            while (!soc_timeout_check(&to)) {
#if defined(BCM_RAVEN_SUPPORT)
                if (SOC_REG_IS_VALID(unit, CMIC_XGXS_PLL_STATUSr)) {
                    READ_CMIC_XGXS_PLL_STATUSr(unit, &val);
                    locked = soc_reg_field_get(unit, CMIC_XGXS_PLL_STATUSr,
                                            val, CMIC_XG_PLL_LOCKf);
                } else
#endif
                {
                    READ_CMIC_XGXS_PLL_CONTROL_2r(unit, &val);
                    locked = soc_reg_field_get(unit, CMIC_XGXS_PLL_CONTROL_2r,
                                               val, PLL_SM_FREQ_PASSf);
                }
                if (locked) {
                    break;
                }
            }
            if (!locked) {
                READ_CMIC_XGXS_PLL_CONTROL_1r(unit, &val);
                soc_reg_field_set(unit, CMIC_XGXS_PLL_CONTROL_1r, &val,
                                  RESETf, 1);
                WRITE_CMIC_XGXS_PLL_CONTROL_1r(unit, val);
                sal_usleep(100);

                READ_CMIC_XGXS_PLL_CONTROL_1r(unit, &val);
                val |= 0xf0000000;
                WRITE_CMIC_XGXS_PLL_CONTROL_1r(unit, val);
                sal_usleep(100);

                READ_CMIC_XGXS_PLL_CONTROL_1r(unit, &val);
                soc_reg_field_set(unit, CMIC_XGXS_PLL_CONTROL_1r, &val,
                                  RESETf, 0);
                WRITE_CMIC_XGXS_PLL_CONTROL_1r(unit, val);
                sal_usleep(50);
            }
        }

        if (!locked) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit,
                                  "fbx_lcpll_lock_check: LCPLL not locked on unit %d "
                                  "status = 0x%08x\n"),
                       unit, val));
        }
    }
}
#endif /* BCM_XGS3_SWITCH_SUPPORT || BCM_PETRA_SUPPORT*/


#if defined(BCM_BIGMAC_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_XMAC_SUPPORT)
/* Reset XGXS (unicore, hyperlite, ...) via BigMAC fusion interface or port
 * registers */
int
soc_xgxs_reset(int unit, soc_port_t port)
{
    soc_reg_t           reg;
    uint64              rval64;
#ifdef BCM_IPROC_SUPPORT
    uint32              rval;
#endif
    int                 reset_sleep_usec;
    soc_field_t rstb_hw, rstb_mdioregs, rstb_pll,
                txd1_g_fifo_restb, txd10_g_fifo_restb,
                pwrdwn, pwrdwn_pll;

    reset_sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;

    reg = MAC_XGXS_CTRLr;
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit) && !SOC_IS_TRIUMPH2(unit) && !SOC_IS_APOLLO(unit) &&
         !SOC_IS_ENDURO(unit) && !SOC_IS_HURRICANE(unit) && !SOC_IS_VALKYRIE2(unit) &&
        (port == 6 || port == 7 || port == 18 || port == 19 ||
         port == 35 || port == 36 || port == 46 || port == 47)) {
        reg = XGPORT_XGXS_CTRLr;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_SC_CQ(unit) && (port >= 25 && port <= 28)) {
        reg = QGPORT_MAC_XGXS_CTRLr;
    }
#endif /* BCM_SCORPION_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
    if (SOC_IS_TD_TT(unit) || SOC_IS_SHADOW(unit)) {
        reg = XLPORT_XGXS_CTRL_REGr;
    }
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANAX(unit) && !SOC_IS_SABER2(unit)) {
        reg = XPORT_XGXS_CTRLr;
    }
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        if(IS_MXQ_PORT(unit, port)) {
            reg = XPORT_XGXS_CTRLr;
        } else {
            reg = XLPORT_XGXS0_CTRL_REGr;
        }
    }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        reg = PORT_XGXS0_CTRL_REGr;
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_ARAD(unit)) {
        reg = PORT_XGXS_0_CTRL_REGr;
    }
#endif /* BCM_PETRA_SUPPORT */

    if (((SOC_IS_HELIX4(unit) && SOC_INFO(unit).port_l2p_mapping[port] == 49) ||
         (SOC_IS_KATANA2(unit) && port == 40)) &&
         (soc_cm_get_bus_type(unit) & SOC_PCI_DEV_TYPE)) {
            /* REFSEL=2'b01, REFDIV=2'b00, REF_TERM_SEL=1'b1. */
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
            if (SOC_IS_HELIX4(unit)) {
                soc_reg64_field32_set(unit, reg, &rval64, REFSELf, 1);
            }
            if (SOC_IS_KATANA2(unit)) {
                soc_reg64_field32_set(unit, reg, &rval64, REFSELf, 0);
            }
            soc_reg64_field32_set(unit, reg, &rval64, REF_TERM_SELf, 1);
            soc_reg64_field32_set(unit, reg, &rval64, REFDIVf, 0);
            soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
#ifdef BCM_IPROC_SUPPORT
            SOC_IF_ERROR_RETURN(READ_IPROC_WRAP_MISC_CONTROLr(unit, &rval));
            if (SOC_IS_HELIX4(unit)) {
                soc_reg_field_set(unit, IPROC_WRAP_MISC_CONTROLr, &rval,
                                               QUAD_SERDES_CTRL_SELf, 0);
            } else if (SOC_IS_KATANA2(unit)) {
                soc_reg_field_set(unit, IPROC_WRAP_MISC_CONTROLr, &rval,
                                            UNICORE_SERDES_CTRL_SELf, 0);
            }
            SOC_IF_ERROR_RETURN(WRITE_IPROC_WRAP_MISC_CONTROLr(unit, rval));
#endif
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    if (SOC_REG_FIELD_VALID(unit, reg, LCREFENf) ||
        SOC_REG_FIELD_VALID(unit, reg, LCREF_ENf)) {
        int lcpll;
        soc_field_t lcpll_f;

        lcpll_f = SOC_REG_FIELD_VALID(unit, reg, LCREFENf) ? LCREFENf : LCREF_ENf;
        /*
         * Reference clock selection
         */
        lcpll = soc_property_port_get(unit, port, spn_XGXS_LCPLL,
                                      SAL_BOOT_QUICKTURN ? 0 : 1);
        if (lcpll && !SOC_IS_ARAD(unit)) { /* Internal LCPLL reference clock */
            /* Double-check LCPLL lock */
            soc_xgxs_lcpll_lock_check(unit);
        }
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, lcpll_f, lcpll ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT || BCM_PETRA_SUPPORT*/

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        pwrdwn = PWR_DWNf;
        pwrdwn_pll = PWR_DWN_PLLf;
        rstb_hw = RST_B_HWf;
        rstb_mdioregs = RST_B_MDIOREGSf;
        rstb_pll = RST_B_PLLf;
        txd1_g_fifo_restb = TXD_1_G_FIFO_RST_Bf;
        txd10_g_fifo_restb = TXD_10_G_FIFO_RST_Bf;
    } else
#endif /* BCM_PETRA_SUPPORT */
    {
        pwrdwn = PWRDWNf;
        pwrdwn_pll = PWRDWN_PLLf;
        rstb_hw = RSTB_HWf;
        rstb_mdioregs = RSTB_MDIOREGSf;
        rstb_pll = RSTB_PLLf;
        txd1_g_fifo_restb = TXD1G_FIFO_RSTBf;
        txd10_g_fifo_restb = TXD10G_FIFO_RSTBf;
    }

    /*
     * XGXS MAC initialization steps.
     *
     * A minimum delay is required between various initialization steps.
     * There is no maximum delay.  The values given are very conservative
     * including the timeout for PLL lock.
     */
    /* Release reset (if asserted) to allow bigmac to initialize */
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
    if (soc_reg_field_valid(unit, reg, pwrdwn)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 0);
    }
    if (soc_reg_field_valid(unit, reg, pwrdwn_pll)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 0);
    }
    if (soc_reg_field_valid(unit,reg, HW_RSTLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, HW_RSTLf, 1);
    } else if (soc_reg_field_valid(unit,reg, rstb_hw)) {
        soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    /* Power down and reset */
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    if (soc_reg_field_valid(unit, reg, pwrdwn)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 1);
    }
    if (soc_reg_field_valid(unit, reg, pwrdwn_pll)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 1);
    }
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 1);
    if (soc_reg_field_valid(unit, reg, HW_RSTLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, HW_RSTLf, 0);
    } else if (soc_reg_field_valid(unit, reg, rstb_hw)) {
        soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 0);
    }
    if (soc_reg_field_valid(unit, reg, TXFIFO_RSTLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, TXFIFO_RSTLf, 0);
    } else if (soc_reg_field_valid(unit, reg, txd1_g_fifo_restb)) {
        soc_reg64_field32_set(unit, reg, &rval64,
                              txd1_g_fifo_restb, 0);
        soc_reg64_field32_set(unit, reg, &rval64,
                              txd10_g_fifo_restb, 0);
    }
    if (soc_reg_field_valid(unit, reg, AFIFO_RSTf)) {
        soc_reg64_field32_set(unit, reg, &rval64, AFIFO_RSTf, 1);
    }

    if (SOC_IS_TRX(unit) || SOC_IS_ARAD(unit)) { /* How about Bradley */
        if (soc_reg_field_valid(unit,reg, rstb_mdioregs)) {
        soc_reg64_field32_set(unit, reg, &rval64, rstb_mdioregs, 0);
        }
        if (soc_reg_field_valid(unit,reg, rstb_pll)) {
        soc_reg64_field32_set(unit, reg, &rval64, rstb_pll, 0);
        }
        if (soc_reg_field_valid(unit,reg, BIGMACRSTLf)) {
            soc_reg64_field32_set(unit, reg, &rval64, BIGMACRSTLf, 0);
        }
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    /*
     * Bring up both digital and analog clocks
     *
     * NOTE: Many MAC registers are not accessible until the PLL is locked.
     * An S-Channel timeout will occur before that.
     */
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    if (soc_reg_field_valid(unit, reg, pwrdwn)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 0);
    }
    if (soc_reg_field_valid(unit, reg, pwrdwn_pll)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 0);
    }
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    /* Bring XGXS out of reset, AFIFO_RST stays 1.  */
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    if (soc_reg_field_valid(unit, reg, HW_RSTLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, HW_RSTLf, 1);
    } else if (soc_reg_field_valid(unit, reg, rstb_hw)) {
        soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    if (SOC_IS_TRX(unit) || SOC_IS_ARAD(unit)) { /* How about Bradley */
        /* Bring MDIO registers out of reset */
        if (soc_reg_field_valid(unit,reg, rstb_mdioregs)) {
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, rstb_mdioregs, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
        }

        /* Activate all clocks */
        if (soc_reg_field_valid(unit,reg, rstb_pll)) {
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, rstb_pll, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
        }

        /* Bring BigMac out of reset*/
        if (soc_reg_field_valid(unit,reg, BIGMACRSTLf)) {
            SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
            soc_reg64_field32_set(unit, reg, &rval64, BIGMACRSTLf, 1);
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
        }
    }

    /* Bring Tx FIFO out of reset */
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    if (soc_reg_field_valid(unit, reg, TXFIFO_RSTLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, TXFIFO_RSTLf, 1);
    } else if (soc_reg_field_valid(unit, reg, txd1_g_fifo_restb)) {
        soc_reg64_field32_set(unit, reg, &rval64,
                              txd1_g_fifo_restb, 0xf);
        soc_reg64_field32_set(unit, reg, &rval64,
                              txd10_g_fifo_restb, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

#if defined(BCM_HELIX15_SUPPORT) || defined(BCM_FELIX15_SUPPORT) || \
    defined(BCM_FIREBOLT2_SUPPORT)
    /* Release LMD reset */
    if (soc_feature(unit, soc_feature_lmd)) {
#if defined(BCM_FIREBOLT2_SUPPORT)
        if (SOC_REG_IS_VALID(unit, CMIC_1000_BASE_X_MODEr)) {
            uint32 lmd_cmic, lmd_bmp, lmd_ether_bmp;
            int hg_offset;

            hg_offset = port - SOC_HG_OFFSET(unit);

            /*
             * COVERITY
             * It is kept intentional, API may be modified to return error
             */
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN
                (READ_CMIC_1000_BASE_X_MODEr(unit, &lmd_cmic));
            lmd_bmp =
                soc_reg_field_get(unit, CMIC_1000_BASE_X_MODEr,
                                  lmd_cmic, LMD_ENABLEf);
            lmd_ether_bmp =
                soc_reg_field_get(unit, CMIC_1000_BASE_X_MODEr,
                                  lmd_cmic, LMD_1000BASEX_ENABLEf);
            if (IS_LMD_ENABLED_PORT(unit, port)) {
                lmd_bmp |= (1 << hg_offset);
                if (IS_XE_PORT(unit,port)) {
                    lmd_ether_bmp |= (1 << hg_offset);
                } else {
                    lmd_ether_bmp &= ~(1 << hg_offset);
                }
            } else {
                lmd_bmp &= ~(1 << hg_offset);
                lmd_ether_bmp &= ~(1 << hg_offset);
            }
            soc_reg_field_set(unit, CMIC_1000_BASE_X_MODEr,
                              &lmd_cmic, LMD_ENABLEf, lmd_bmp);
            soc_reg_field_set(unit, CMIC_1000_BASE_X_MODEr, &lmd_cmic,
                              LMD_1000BASEX_ENABLEf, lmd_ether_bmp);

            /*
             * COVERITY
             * It is kept intentional, API may be modified to return error
             */
            /* coverity[result_independent_of_operands] */
            SOC_IF_ERROR_RETURN
                (WRITE_CMIC_1000_BASE_X_MODEr(unit, lmd_cmic));
        }
#endif /* BCM_FIREBOLT2_SUPPORT */

        SOC_IF_ERROR_RETURN(READ_MAC_CTRLr(unit, port, &rval64));
        soc_reg64_field32_set(unit, MAC_CTRLr, &rval64, LMD_RSTBf, 1);
        SOC_IF_ERROR_RETURN(WRITE_MAC_CTRLr(unit, port, rval64));
    }
#endif /* BCM_HELIX15_SUPPORT || BCM_FELIX15_SUPPORT ||
        * BCM_FIREBOLT2_SUPPORT */
    return SOC_E_NONE;
}

int
soc_xgxs_in_reset(int unit, soc_port_t port)
{
    soc_reg_t           reg;
    uint64              rval64;
    int                 reset_sleep_usec;
    soc_field_t rstb_hw, rstb_mdioregs, rstb_pll,
                txd1_g_fifo_restb, txd10_g_fifo_restb,
                pwrdwn, pwrdwn_pll;

    reset_sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;

    reg = MAC_XGXS_CTRLr;
#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_IS_TR_VL(unit) && !SOC_IS_TRIUMPH2(unit) && !SOC_IS_APOLLO(unit) &&
         !SOC_IS_ENDURO(unit) && !SOC_IS_HURRICANE(unit) && !SOC_IS_VALKYRIE2(unit) &&
        (port == 6 || port == 7 || port == 18 || port == 19 ||
         port == 35 || port == 36 || port == 46 || port == 47)) {
        reg = XGPORT_XGXS_CTRLr;
    }
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
    if (SOC_IS_SC_CQ(unit) && (port >= 25 && port <= 28)) {
        reg = QGPORT_MAC_XGXS_CTRLr;
    }
#endif /* BCM_SCORPION_SUPPORT */
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_SHADOW_SUPPORT)
    if (SOC_IS_TD_TT(unit) || SOC_IS_SHADOW(unit)) {
        reg = XLPORT_XGXS_CTRL_REGr;
    }
#endif /* BCM_TRIDENT_SUPPORT */
#if defined(BCM_KATANA_SUPPORT)
    if (SOC_IS_KATANAX(unit)) {
        reg = XPORT_XGXS_CTRLr;
    }
#endif /* BCM_KATANA_SUPPORT */
#if defined(BCM_SABER2_SUPPORT)
    if(SOC_IS_SABER2(unit)) {
        if(IS_MXQ_PORT(unit, port)) {
            reg = XPORT_XGXS_CTRLr;
        } else {
            reg = XLPORT_XGXS0_CTRL_REGr;
        }
    }
#endif
#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        reg = PORT_XGXS0_CTRL_REGr;
    }
#endif /* BCM_TRIUMPH3_SUPPORT */
#if defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_ARAD(unit)) {
        reg = PORT_XGXS_0_CTRL_REGr;
    }
#endif /* BCM_PETRA_SUPPORT */

#if defined(BCM_XGS3_SWITCH_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    if (SOC_REG_FIELD_VALID(unit, reg, LCREFENf) ||
        SOC_REG_FIELD_VALID(unit, reg, LCREF_ENf)) {
        int lcpll;
        soc_field_t lcpll_f;

        lcpll_f = SOC_REG_FIELD_VALID(unit, reg, LCREFENf) ? LCREFENf : LCREF_ENf;
        /*
         * Reference clock selection
         */
        lcpll = soc_property_port_get(unit, port, spn_XGXS_LCPLL,
                                      SAL_BOOT_QUICKTURN ? 0 : 1);
        if (lcpll && !SOC_IS_ARAD(unit)) { /* Internal LCPLL reference clock */
            /* Double-check LCPLL lock */
            soc_xgxs_lcpll_lock_check(unit);
        }
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
        soc_reg64_field32_set(unit, reg, &rval64, lcpll_f, lcpll ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT || BCM_PETRA_SUPPORT*/

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        pwrdwn = PWR_DWNf;
        pwrdwn_pll = PWR_DWN_PLLf;
        rstb_hw = RST_B_HWf;
        rstb_mdioregs = RST_B_MDIOREGSf;
        rstb_pll = RST_B_PLLf;
        txd1_g_fifo_restb = TXD_1_G_FIFO_RST_Bf;
        txd10_g_fifo_restb = TXD_10_G_FIFO_RST_Bf;
    } else
#endif /* BCM_PETRA_SUPPORT */
    {
        pwrdwn = PWRDWNf;
        pwrdwn_pll = PWRDWN_PLLf;
        rstb_hw = RSTB_HWf;
        rstb_mdioregs = RSTB_MDIOREGSf;
        rstb_pll = RSTB_PLLf;
        txd1_g_fifo_restb = TXD1G_FIFO_RSTBf;
        txd10_g_fifo_restb = TXD10G_FIFO_RSTBf;
    }

    /*
     * XGXS MAC initialization steps.
     *
     * A minimum delay is required between various initialization steps.
     * There is no maximum delay.  The values given are very conservative
     * including the timeout for PLL lock.
     */
    /* Release reset (if asserted) to allow bigmac to initialize */
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
    if (soc_reg_field_valid(unit, reg, pwrdwn)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 0);
    }
    if (soc_reg_field_valid(unit, reg, pwrdwn_pll)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 0);
    }
    if (soc_reg_field_valid(unit,reg, HW_RSTLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, HW_RSTLf, 1);
    } else if (soc_reg_field_valid(unit,reg, rstb_hw)) {
        soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    /* Power down and reset */
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    if (soc_reg_field_valid(unit, reg, pwrdwn)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 1);
    }
    if (soc_reg_field_valid(unit, reg, pwrdwn_pll)) {
        soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 1);
    }
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 1);
    if (soc_reg_field_valid(unit, reg, HW_RSTLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, HW_RSTLf, 0);
    } else if (soc_reg_field_valid(unit, reg, rstb_hw)) {
        soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 0);
    }
    if (soc_reg_field_valid(unit, reg, TXFIFO_RSTLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, TXFIFO_RSTLf, 0);
    } else if (soc_reg_field_valid(unit, reg, txd1_g_fifo_restb)) {
        soc_reg64_field32_set(unit, reg, &rval64,
                              txd1_g_fifo_restb, 0);
        soc_reg64_field32_set(unit, reg, &rval64,
                              txd10_g_fifo_restb, 0);
    }
    if (soc_reg_field_valid(unit, reg, AFIFO_RSTf)) {
        soc_reg64_field32_set(unit, reg, &rval64, AFIFO_RSTf, 1);
    }

    if (SOC_IS_TRX(unit) || SOC_IS_ARAD(unit)) { /* How about Bradley */
        soc_reg64_field32_set(unit, reg, &rval64, rstb_mdioregs, 0);
        soc_reg64_field32_set(unit, reg, &rval64, rstb_pll, 0);
        if (soc_reg_field_valid(unit,reg, BIGMACRSTLf)) {
            soc_reg64_field32_set(unit, reg, &rval64, BIGMACRSTLf, 0);
        }
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    return SOC_E_NONE;
}

#endif /* BCM_BIGMAC_SUPPORT || BCM_PETRA_SUPPORT*/

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_PETRA_SUPPORT)
int
soc_wc_xgxs_reset(int unit, soc_port_t port, int reg_idx)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    const static soc_reg_t tr3_regs[] = {
        PORT_XGXS0_CTRL_REGr,
        PORT_XGXS1_CTRL_REGr,
        PORT_XGXS2_CTRL_REGr
    };
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_PETRA_SUPPORT
    const static soc_reg_t arad_regs[] = {
        PORT_XGXS_0_CTRL_REGr,
        PORT_XGXS_1_CTRL_REGr,
        PORT_XGXS_2_CTRL_REGr
    };
#endif /* BCM_TRIUMPH3_SUPPORT */
    soc_reg_t   reg;
    uint64      rval64;
    int         reset_sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    int         lcpll;
    soc_field_t rstb_hw, rstb_mdioregs, rstb_pll,
                txd1_g_fifo_restb, txd10_g_fifo_restb,
                pwrdwn, pwrdwn_pll;

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        reg = tr3_regs[reg_idx];
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        reg = arad_regs[reg_idx];
    } else
#endif /* BCM_PETRA_SUPPORT */
    {
        reg = XLPORT_XGXS_CTRL_REGr;
    }

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        pwrdwn = PWR_DWNf;
        pwrdwn_pll = PWR_DWN_PLLf;
        rstb_hw = RST_B_HWf;
        rstb_mdioregs = RST_B_MDIOREGSf;
        rstb_pll = RST_B_PLLf;
        txd1_g_fifo_restb = TXD_1_G_FIFO_RST_Bf;
        txd10_g_fifo_restb = TXD_10_G_FIFO_RST_Bf;
    } else
#endif /* BCM_PETRA_SUPPORT */
    {
        pwrdwn = PWRDWNf;
        pwrdwn_pll = PWRDWN_PLLf;
        rstb_hw = RSTB_HWf;
        rstb_mdioregs = RSTB_MDIOREGSf;
        rstb_pll = RSTB_PLLf;
        txd1_g_fifo_restb = TXD1G_FIFO_RSTBf;
        txd10_g_fifo_restb = TXD10G_FIFO_RSTBf;
    }

    /*
     * Reference clock selection
     */
    lcpll = soc_property_port_get(unit, port, spn_XGXS_LCPLL,
                                  SAL_BOOT_QUICKTURN ? 0 : 1);
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, LCREF_ENf, lcpll ? 1 : 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

    /*
     * XGXS MAC initialization steps.
     *
     * A minimum delay is required between various initialization steps.
     * There is no maximum delay.  The values given are very conservative
     * including the timeout for PLL lock.
     */
    /* Release reset (if asserted) to allow xmac/cmac to initialize */
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 0);
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 0);
    soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 1);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    /* Power down and reset */
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 1);
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 1);
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 1);
    soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 0);
    soc_reg64_field32_set(unit, reg, &rval64, rstb_mdioregs, 0);
    soc_reg64_field32_set(unit, reg, &rval64, rstb_pll, 0);
    soc_reg64_field32_set(unit, reg, &rval64, txd1_g_fifo_restb, 0);
    soc_reg64_field32_set(unit, reg, &rval64, txd10_g_fifo_restb, 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    /*
     * Bring up both digital and analog clocks
     *
     * NOTE: Many MAC registers are not accessible until the PLL is locked.
     * An S-Channel timeout will occur before that.
     */
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 0);
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 0);
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    /* Bring XGXS out of reset */
    soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 1);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    /* Bring MDIO registers out of reset */
    soc_reg64_field32_set(unit, reg, &rval64, rstb_mdioregs, 1);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

    /* Activate all clocks */
    soc_reg64_field32_set(unit, reg, &rval64, rstb_pll, 1);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

    /* Bring Tx FIFO out of reset */
    soc_reg64_field32_set(unit, reg, &rval64, txd1_g_fifo_restb, 0xf);
    soc_reg64_field32_set(unit, reg, &rval64, txd10_g_fifo_restb, 1);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

    return SOC_E_NONE;
}

int
soc_wc_xgxs_in_reset(int unit, soc_port_t port, int reg_idx)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    const static soc_reg_t tr3_regs[] = {
        PORT_XGXS0_CTRL_REGr,
        PORT_XGXS1_CTRL_REGr,
        PORT_XGXS2_CTRL_REGr
    };
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_PETRA_SUPPORT
    const static soc_reg_t arad_regs[] = {
        PORT_XGXS_0_CTRL_REGr,
        PORT_XGXS_1_CTRL_REGr,
        PORT_XGXS_2_CTRL_REGr
    };
#endif /* BCM_TRIUMPH3_SUPPORT */
    soc_reg_t   reg;
    uint64      rval64;
    int         reset_sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    int         lcpll;
    soc_field_t rstb_hw, rstb_mdioregs, rstb_pll,
                txd1_g_fifo_restb, txd10_g_fifo_restb,
                pwrdwn, pwrdwn_pll;

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        reg = tr3_regs[reg_idx];
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        reg = arad_regs[reg_idx];
    } else
#endif /* BCM_PETRA_SUPPORT */
    {
        reg = XLPORT_XGXS_CTRL_REGr;
    }

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        pwrdwn = PWR_DWNf;
        pwrdwn_pll = PWR_DWN_PLLf;
        rstb_hw = RST_B_HWf;
        rstb_mdioregs = RST_B_MDIOREGSf;
        rstb_pll = RST_B_PLLf;
        txd1_g_fifo_restb = TXD_1_G_FIFO_RST_Bf;
        txd10_g_fifo_restb = TXD_10_G_FIFO_RST_Bf;
    } else
#endif /* BCM_PETRA_SUPPORT */
    {
        pwrdwn = PWRDWNf;
        pwrdwn_pll = PWRDWN_PLLf;
        rstb_hw = RSTB_HWf;
        rstb_mdioregs = RSTB_MDIOREGSf;
        rstb_pll = RSTB_PLLf;
        txd1_g_fifo_restb = TXD1G_FIFO_RSTBf;
        txd10_g_fifo_restb = TXD10G_FIFO_RSTBf;
    }

    /*
     * Reference clock selection
     */
    lcpll = soc_property_port_get(unit, port, spn_XGXS_LCPLL,
                                  SAL_BOOT_QUICKTURN ? 0 : 1);
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, LCREF_ENf, lcpll ? 1 : 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

    /*
     * XGXS MAC initialization steps.
     *
     * A minimum delay is required between various initialization steps.
     * There is no maximum delay.  The values given are very conservative
     * including the timeout for PLL lock.
     */
    /* Release reset (if asserted) to allow xmac/cmac to initialize */
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 0);
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 0);
    soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 1);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    /* Power down and reset */
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn, 1);
    soc_reg64_field32_set(unit, reg, &rval64, pwrdwn_pll, 1);
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 1);
    soc_reg64_field32_set(unit, reg, &rval64, rstb_hw, 0);
    soc_reg64_field32_set(unit, reg, &rval64, rstb_mdioregs, 0);
    soc_reg64_field32_set(unit, reg, &rval64, rstb_pll, 0);
    soc_reg64_field32_set(unit, reg, &rval64, txd1_g_fifo_restb, 0);
    soc_reg64_field32_set(unit, reg, &rval64, txd10_g_fifo_restb, 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(reset_sleep_usec);

    return SOC_E_NONE;
}

int
soc_wc_xgxs_pll_check(int unit, soc_port_t port, int reg_idx)
{
#ifdef BCM_TRIUMPH3_SUPPORT
    const static soc_reg_t tr3_regs[] = {
        PORT_XGXS0_STATUS_GEN_REGr,
        PORT_XGXS1_STATUS_GEN_REGr,
        PORT_XGXS2_STATUS_GEN_REGr
    };
#endif /* BCM_TRIUMPH3_SUPPORT */
    soc_reg_t   reg;
    uint32      rval;
    int         phy_port, block, retry;
    int         lock_sleep_usec = 5000;

#ifdef BCM_TRIUMPH3_SUPPORT
    if (SOC_IS_TRIUMPH3(unit)) {
        reg = tr3_regs[reg_idx];
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        reg = XLPORT_XGXS_STATUS_GEN_REGr;
    }

    retry = 10;

    /* Check TxPLL lock status */
    while (retry > 0) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
        if (soc_reg_field_get(unit, reg, rval, TXPLL_LOCKf)) {
            return SOC_E_NONE;
        }
        sal_usleep(lock_sleep_usec);
        retry--;
    }

    phy_port = SOC_INFO(unit).port_l2p_mapping[port];
    block = SOC_PORT_BLOCK(unit, phy_port);
    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d %s TXPLL not locked\n"),
               unit, SOC_BLOCK_NAME(unit, block)));
    return SOC_E_NONE;
}

int
soc_wc_xgxs_power_down(int unit, soc_port_t port, int reg_idx)
{
#if defined(BCM_TRIUMPH3_SUPPORT)
    const static soc_reg_t tr3_regs[] = {
        PORT_XGXS0_CTRL_REGr,
        PORT_XGXS1_CTRL_REGr,
        PORT_XGXS2_CTRL_REGr
    };
#endif /* BCM_TRIUMPH3_SUPPORT */
    soc_reg_t   reg;
    uint64      rval64;

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (SOC_IS_TRIUMPH3(unit)) {
        reg = tr3_regs[reg_idx];
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
    {
        reg = XLPORT_XGXS_CTRL_REGr;
    }

    /* Power down and reset */
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, PWRDWNf, 1);
    soc_reg64_field32_set(unit, reg, &rval64, PWRDWN_PLLf, 1);
    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 1);
    soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 0);
    soc_reg64_field32_set(unit, reg, &rval64, RSTB_MDIOREGSf, 0);
    soc_reg64_field32_set(unit, reg, &rval64, RSTB_PLLf, 0);
    soc_reg64_field32_set(unit, reg, &rval64, TXD1G_FIFO_RSTBf, 0);
    soc_reg64_field32_set(unit, reg, &rval64, TXD10G_FIFO_RSTBf, 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                            "Power down wc for port: %d\n"), port));
    return SOC_E_NONE;
}
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || defined(BCM_SABER2_SUPPORT) || defined(BCM_KATANA2_SUPPORT)
/*
 * function: _soc_xgxs_reset_single_tsc
 * purpose:  reset a single TSC associate with a TD2/TD2+ port
*/
STATIC soc_error_t
_soc_xgxs_reset_single_tsc(int unit, soc_port_t port, soc_reg_t reg) {

    int         sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    int         lcpll;
    uint64      rval64;
    /*
     * Reference clock selection
     */
    lcpll = soc_property_port_get(unit, port, spn_XGXS_LCPLL,
                                  SAL_BOOT_QUICKTURN ? 0 : 1);
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {
#if 0
        
        soc_reg64_field32_set(unit, reg, &rval64, PLL0_REFIN_ENf, lcpll ? 1 : 0);
        soc_reg64_field32_set(unit, reg, &rval64, PLL1_REFIN_ENf, lcpll ? 1 : 0);
#endif
    } else {
    soc_reg64_field32_set(unit, reg, &rval64, REFIN_ENf, lcpll ? 1 : 0);
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
#if 0
    
    /* For TH3 field is named differently */
    if (SOC_IS_TOMAHAWK3(unit)) {
        if (soc_reg_field_valid(unit, reg, TSC_IDDQf)) {
            soc_reg64_field32_set(unit, reg, &rval64, TSC_IDDQf, 0);
        }
    }
#endif
    /*
     * For TH+/TH2/Apache B0 default behaviour of CLPORT port IDDQ is changed,
     * TH2 default behaviour of XLPORT IDDQ is changed,
     * port init sequence must be adjusted to bring tsc out of reset properly
     */
    if (soc_feature(unit, soc_feature_iddq_new_default) &&
        ((reg == CLPORT_XGXS0_CTRL_REGr) ||
        (SOC_IS_TOMAHAWK2(unit) && reg == XLPORT_XGXS0_CTRL_REGr) ||
        (SOC_IS_MONTEREY(unit) && reg == XLPORT_XGXS0_CTRL_REGr) ||
        (SOC_IS_TOMAHAWK3(unit) && reg == XLPORT_XGXS0_CTRL_REGr))) {
        if (soc_reg_field_valid(unit, reg, IDDQf)) {
            soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
        }
    }

    if (SOC_IS_GREYHOUND2(unit)) {
        if ((reg == XLPORT_XGXS0_CTRL_REGr) ||
            (reg == CLPORT_XGXS0_CTRL_REGr)) {
            if (soc_reg_field_valid(unit, reg, IDDQf)) {
                soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
            }
        }
    }

    /* Deassert power down */
    /* For TH3 field is named differently */
    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {
        soc_reg64_field32_set(unit, reg, &rval64, TSC_PWRDWNf, 0);
    } else {
    soc_reg64_field32_set(unit, reg, &rval64, PWRDWNf, 0);
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(sleep_usec);

    /*for TH3 PM8x50, we need to choose TVCO clk source, chip default is PLL0
    however for TH3 need to use PLL1*/
    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {
        soc_reg64_field32_set(unit, reg, &rval64, TSC_CLK_SELf, 1);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(sleep_usec);
    }

    /* Reset XGXS */
    /* For TH3 field is named differently */
    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {
        soc_reg64_field32_set(unit, reg, &rval64, TSC_RSTBf, 0);
    } else {
    soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 0);
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(sleep_usec+10000);

    /* Bring XGXS out of reset */
    /* For TH3 field is named differently */
    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {
        soc_reg64_field32_set(unit, reg, &rval64, TSC_RSTBf, 1);
    } else {
    soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 1);
    }
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(sleep_usec);

    /* Bring reference clock out of reset */
    if (soc_reg_field_valid(unit, reg, RSTB_REFCLKf)) {
        soc_reg64_field32_set(unit, reg, &rval64, RSTB_REFCLKf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    }
    /* Activate clocks */
    if (soc_reg_field_valid(unit, reg, RSTB_PLLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, RSTB_PLLf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    }
    return SOC_E_NONE;
}

/*
 * function: _soc_xgxs_powerdown_single_tsc
 * purpose:  Powerdown a single TSC associate with a TD2/TH/TD2+/TH2 port
 */
STATIC soc_error_t
_soc_xgxs_powerdown_single_tsc(int unit, soc_port_t port, soc_reg_t reg) {

    int         sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    int         lcpll;
    uint64      rval64;
    /*
     * Reference clock selection
     */
    lcpll = soc_property_port_get(unit, port, spn_XGXS_LCPLL,
                                  SAL_BOOT_QUICKTURN ? 0 : 1);
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, REFIN_ENf, lcpll ? 1 : 0);

    /*
     * For TH+/TH2/Apache B0 default behaviour of CLPORT port IDDQ is changed,
     * TH2 default befhavior of XLPORT IDDQ is changed,
     * port init sequence must be adjusted to bring tsc out of reset properly
     */
    if (soc_feature(unit, soc_feature_iddq_new_default) &&
        ((reg == CLPORT_XGXS0_CTRL_REGr) || (reg == PMQ_XGXS0_CTRL_REGr) ||
#if defined(BCM_FIRELIGHT_SUPPORT)
         (SOC_IS_FIRELIGHT(unit) && (reg == XLPORT_XGXS0_CTRL_REGr)) ||
#endif
         ((SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) &&
          (reg == XLPORT_XGXS0_CTRL_REGr)))) {
        if (soc_reg_field_valid(unit, reg, IDDQf)) {
            soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
        }
    }

    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

    if (soc_feature(unit, soc_feature_iddq_new_default) &&
        ((reg == CLPORT_XGXS0_CTRL_REGr) ||
#if defined(BCM_FIRELIGHT_SUPPORT)
         (SOC_IS_FIRELIGHT(unit) &&
          ((reg == XLPORT_XGXS0_CTRL_REGr) || (reg == PMQ_XGXS0_CTRL_REGr))) ||
#endif
         ((SOC_IS_TOMAHAWK2(unit) || SOC_IS_TRIDENT3X(unit)) &&
          (reg == XLPORT_XGXS0_CTRL_REGr)))) {
        sal_usleep(sleep_usec);
    }

    /* Deassert power down */
    soc_reg64_field32_set(unit, reg, &rval64, PWRDWNf, 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(sleep_usec);

    /* Reset XGXS */
    soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 0);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));

    return SOC_E_NONE;
}

/*
 * function: _soc_xgxs_powerup_single_tsc
 * purpose:  Powerup a single TSC associate with a TD2/TH/TD2+/TH2 port
 */
STATIC soc_error_t
_soc_xgxs_powerup_single_tsc(int unit, soc_port_t port, soc_reg_t reg) {

    int         sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    uint64      rval64;

    /* Bring XGXS out of reset */
    SOC_IF_ERROR_RETURN(soc_reg_get(unit, reg, port, 0, &rval64));
    soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 1);
    SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    sal_usleep(sleep_usec);

    /* Bring reference clock out of reset */
    if (soc_reg_field_valid(unit, reg, RSTB_REFCLKf)) {
        soc_reg64_field32_set(unit, reg, &rval64, RSTB_REFCLKf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    }
    /* Activate clocks */
    if (soc_reg_field_valid(unit, reg, RSTB_PLLf)) {
        soc_reg64_field32_set(unit, reg, &rval64, RSTB_PLLf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, reg, port, 0, rval64));
    }
    return SOC_E_NONE;
}

/*
 * function: _soc_xgxs_reset_tsc_triplet
 * purpose:  reset a TSC-12 associate with a TD2+ 100G port
 */
STATIC soc_error_t
_soc_xgxs_reset_tsc_triplet(int unit, soc_port_t port) {
    /*
     * TD2+ 100G case, PGWs have TSCs mapped as follows.
     * (This mapping is common for all PGWs regardless of Odd/Even)
     *  PGW_TSC0_CTRL_REGr,->|       |
     *  PGW_TSC1_CTRL_REGr,->|TSC-12 |
     *  PGW_TSC2_CTRL_REGr,->|       |
     *  PGW_TSC3_CTRL_REGr -> TSC-4
     * All 3 Control registers associated with a TSC-12 must be reset for
     * a 100G port to be reset.
     */
    const static soc_reg_t ctrl_regs_td2plus[] = {
        PGW_TSC0_CTRL_REGr,
        PGW_TSC1_CTRL_REGr,
        PGW_TSC2_CTRL_REGr,
        PGW_TSC3_CTRL_REGr,
    };
    int         sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    int         lcpll;
    int         i;
    uint64      rval64;

    /*This sequence is the same as in _soc_xgxs_reset_single_tsc, but across
     *all 3 TSC-4s within a TSC-12
     */
    /* Reference clock selection */
    for (i = 0; i < 3; i++) {
        lcpll = soc_property_port_get(unit, port, spn_XGXS_LCPLL,
                                      SAL_BOOT_QUICKTURN ? 0 : 1);
        SOC_IF_ERROR_RETURN(soc_reg_get(unit, ctrl_regs_td2plus[i], port, 0,
                            &rval64));
        soc_reg64_field32_set(unit, ctrl_regs_td2plus[i], &rval64, REFIN_ENf,
                              lcpll ? 1 : 0);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, ctrl_regs_td2plus[i], port, 0,
                            rval64));
    }

    /* Deassert power down */
    for (i = 0; i < 3; i++) {
        soc_reg64_field32_set(unit, ctrl_regs_td2plus[i], &rval64, PWRDWNf, 0);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, ctrl_regs_td2plus[i], port, 0,
                            rval64));
        sal_usleep(sleep_usec);
    }

    /* Reset XGXS */
    for (i = 0; i < 3; i++) {
        soc_reg64_field32_set(unit, ctrl_regs_td2plus[i], &rval64, RSTB_HWf, 0);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, ctrl_regs_td2plus[i], port, 0,
                            rval64));
        sal_usleep(sleep_usec+10000);
    }

    /* Bring XGXS out of reset */
    for (i = 0; i < 3; i++) {
        soc_reg64_field32_set(unit, ctrl_regs_td2plus[i], &rval64, RSTB_HWf, 1);
        SOC_IF_ERROR_RETURN(soc_reg_set(unit, ctrl_regs_td2plus[i], port, 0,
                            rval64));
        sal_usleep(sleep_usec);
    }

    /* Bring reference clock out of reset */
    for (i = 0; i < 3; i++) {
        if (soc_reg_field_valid(unit, ctrl_regs_td2plus[i], RSTB_REFCLKf)) {
            soc_reg64_field32_set(unit, ctrl_regs_td2plus[i], &rval64,
                                  RSTB_REFCLKf, 1);
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, ctrl_regs_td2plus[i], port, 0,
                                            rval64));
        }
    }

    /* Activate clocks */
    for (i = 0; i < 3; i++) {
        if (soc_reg_field_valid(unit, ctrl_regs_td2plus[i], RSTB_PLLf)) {
            soc_reg64_field32_set(unit, ctrl_regs_td2plus[i], &rval64,
                                  RSTB_PLLf, 1);
            SOC_IF_ERROR_RETURN(soc_reg_set(unit, ctrl_regs_td2plus[i], port, 0,
                                            rval64));
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_txc_xgxs_sbus_broadcast_reset
 * Purpose:
 *      Given an array of sbus broadcast blocks, bring the associated Port
 *      Macros on each sbus out of reset.  Broadcasting to all the Port
 *      Macros is much faster than unicasting to each one.
 *
 * Parameters:
 *      unit                - (IN) Unit number
 *      reg                 - (IN) The register (ex. CDPORT_XGXS0_CTRL_REG)
 *      lcpll               - (IN OPTIONAL) Array of lcpll
 *      phy_ports           - (IN) Array of physical ports associated with each
 *                              sbus. Not really necessary other than for
 *                              fetching an instance of the register from hw
 *      sbus_bcast_blocks   - (IN) Array of the broadcast blocks for each sbus
 *                              the Port Macros are on
 *      num_rings           - (IN) The size of sbus_bcast_blocks and phy_ports;
 *                              the number of sbus rings being broadcast to
 * Returns:
 *      SOC_E_XXX
 */
soc_error_t
soc_tsc_xgxs_sbus_broadcast_reset(int unit, soc_reg_t reg, int *lcpll,
                    int *phy_ports, int *sbus_bcast_blocks, int num_rings)
{
#define SOC_MAX_SBUS 16
    uint8 at;
    uint32 raddrs[SOC_MAX_SBUS];
    uint32 reg_val;
    int sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    int i;
    soc_block_t block;

    /* Increase size of array if need more */
    if (num_rings > SOC_MAX_SBUS) {
        return SOC_E_PARAM;
    }
#undef SOC_MAX_SBUS

    /* Broadcast to each SBUS first, then do the necessary sleeps */

    /* Get a register address on each SBUS. The fetched parameters don't matter,
     * only raddr */
    for (i = 0; i < num_rings; i++) {
        raddrs[i] = soc_reg_addr_get(unit, reg, phy_ports[i], 0,
                SOC_REG_ADDR_OPTION_PRESERVE_PORT, &block, &at);
    }

    /* Just get one instance of the register and fill it once; it'll be the same for all
     * active PMs in the device */
    SOC_IF_ERROR_RETURN(_soc_reg32_get(unit, block, at, raddrs[0], &reg_val));


    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {

    } else {
        if (NULL != lcpll) {
            /* Broadcast to each SBUS ring */
            for (i = 0; i < num_rings; i++) {
                soc_reg_field_set(unit, reg, &reg_val, REFIN_ENf, lcpll[i] ? 1: 0);
                SOC_IF_ERROR_RETURN(
                    _soc_reg32_set(unit, sbus_bcast_blocks[i], at, raddrs[i], reg_val));
            }
        }
    }

    /*
     * For TH+/TH2/Apache B0 default behaviour of CLPORT port IDDQ is changed,
     * TH2 default behaviour of XLPORT IDDQ is changed,
     * port init sequence must be adjusted to bring tsc out of reset properly
     */
    if (soc_feature(unit, soc_feature_iddq_new_default) &&
        ((reg == CLPORT_XGXS0_CTRL_REGr) ||
        (SOC_IS_TOMAHAWK2(unit) && reg == XLPORT_XGXS0_CTRL_REGr) ||
        (SOC_IS_MONTEREY(unit) && reg == XLPORT_XGXS0_CTRL_REGr) ||
        (SOC_IS_TOMAHAWK3(unit) && reg == XLPORT_XGXS0_CTRL_REGr))) {
        if (soc_reg_field_valid(unit, reg, IDDQf)) {
            soc_reg_field_set(unit, reg, &reg_val, IDDQf, 0);
        }
    }

    if (SOC_IS_GREYHOUND2(unit)) {
        if ((reg == XLPORT_XGXS0_CTRL_REGr) ||
            (reg == CLPORT_XGXS0_CTRL_REGr)) {
            if (soc_reg_field_valid(unit, reg, IDDQf)) {
                soc_reg_field_set(unit, reg, &reg_val, IDDQf, 0);
            }
        }
    }

    /* Deassert power down */
    /* For TH3 field is named differently */
    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {
        soc_reg_field_set(unit, reg, &reg_val, TSC_PWRDWNf, 0);
    } else {
        soc_reg_field_set(unit, reg, &reg_val, PWRDWNf, 0);
    }
    /* Broadcast to each SBUS ring */
    for (i = 0; i < num_rings; i++) {
        SOC_IF_ERROR_RETURN(_soc_reg32_set(unit, sbus_bcast_blocks[i], at, raddrs[i], reg_val));
    }
    sal_usleep(sleep_usec);


    /* for TH3 PM8x50, we need to choose TVCO clk source, chip default is PLL0
    however for TH3 need to use PLL1*/
    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {
        soc_reg_field_set(unit, reg, &reg_val, TSC_CLK_SELf, 1);
        /* Broadcast to each SBUS ring */
        for (i = 0; i < num_rings; i++) {
            SOC_IF_ERROR_RETURN(_soc_reg32_set(unit, sbus_bcast_blocks[i], at, raddrs[i], reg_val));
        }
        sal_usleep(sleep_usec);
    }


    /* Reset XGXS */
    /* For TH3 field is named differently */
    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {
        soc_reg_field_set(unit, reg, &reg_val, TSC_RSTBf, 0);
    } else {
        soc_reg_field_set(unit, reg, &reg_val, RSTB_HWf, 0);
    }
    /* Broadcast again to each SBUS ring */
    for (i = 0; i < num_rings; i++) {
        SOC_IF_ERROR_RETURN(_soc_reg32_set(unit, sbus_bcast_blocks[i], at, raddrs[i], reg_val));
    }
    sal_usleep(sleep_usec+10000);


    /* Bring XGXS out of reset */
    /* For TH3 field is named differently */
    if (SOC_IS_TOMAHAWK3(unit) && reg == CDPORT_XGXS0_CTRL_REGr) {
        soc_reg_field_set(unit, reg, &reg_val, TSC_RSTBf, 1);
    } else {
        soc_reg_field_set(unit, reg, &reg_val, RSTB_HWf, 1);
    }
    /* Broadcast again to each SBUS ring */
    for (i = 0; i < num_rings; i++) {
        SOC_IF_ERROR_RETURN(_soc_reg32_set(unit, sbus_bcast_blocks[i], at, raddrs[i], reg_val));
    }
    sal_usleep(sleep_usec);


    /* Bring reference clock out of reset */
    if (soc_reg_field_valid(unit, reg, RSTB_REFCLKf)) {
        soc_reg_field_set(unit, reg, &reg_val, RSTB_REFCLKf, 1);
        /* Broadcast again to each SBUS ring */
        for (i = 0; i < num_rings; i++) {
            SOC_IF_ERROR_RETURN(_soc_reg32_set(unit, sbus_bcast_blocks[i], at, raddrs[i], reg_val));
        }
    }


    /* Activate clocks */
    if (soc_reg_field_valid(unit, reg, RSTB_PLLf)) {
        soc_reg_field_set(unit, reg, &reg_val, RSTB_PLLf, 1);
        /* Broadcast again to each SBUS ring */
        for (i = 0; i < num_rings; i++) {
            SOC_IF_ERROR_RETURN(_soc_reg32_set(unit, sbus_bcast_blocks[i], at, raddrs[i], reg_val));
        }
    }

    return SOC_E_NONE;
}

/* Check the port argument is indicating a block id */
#define SOC_IS_BLKID_PORT(_blkid_port)  \
    (_blkid_port & SOC_REG_ADDR_BLOCK_ID_MASK)
/* Get the block type from the port argument which is a block id */
#define SOC_BLOCK_TYPE_BLKID_PORT(_u, _blkid_port)   \
    (SOC_BLOCK_TYPE(_u, (_blkid_port & ~SOC_REG_ADDR_BLOCK_ID_MASK)))

/*
 * function: soc_tsc_xgxs_reset
 * purpose:  Reset all TSCs associated with the passed port.
*/
soc_error_t
soc_tsc_xgxs_reset(int unit, soc_port_t port, int reg_idx)
{
    soc_error_t rv = SOC_E_NONE;
    const static soc_reg_t ctrl_regs[] = {
        XLPORT_XGXS0_CTRL_REGr,
        XLPORT_XGXS0_CTRL_REGr,
        XLPORT_XGXS0_CTRL_REGr,
        XLPORT_XGXS0_CTRL_REGr,
    };
    const static soc_reg_t cxxport_ctrl_regs[] = {
        CXXPORT_XGXS0_CTRL0_REGr,
        CXXPORT_XGXS0_CTRL1_REGr,
        CXXPORT_XGXS0_CTRL2_REGr,
    };

    soc_reg_t reg = ctrl_regs[reg_idx];

    if ((port > 0) && SOC_IS_BLKID_PORT(port)) {
        if (SOC_BLOCK_TYPE_BLKID_PORT(unit, port) == SOC_BLK_PMQ) {
            reg = GPORT_XGXS0_CTRL_REGr;
        }
    } else if (IS_CXX_PORT(unit, port)) {
        reg = cxxport_ctrl_regs[reg_idx];
    } else if (IS_CL_PORT(unit, port) || IS_CLG2_PORT(unit, port)) {
        reg = CLPORT_XGXS0_CTRL_REGr;
    } else if (IS_CD_PORT(unit, port)) {
        reg = CDPORT_XGXS0_CTRL_REGr;
    }

    if ((!SOC_IS_TRIDENT2X(unit)
            && !SOC_IS_TITAN2PLUS(unit))
                    || SOC_IS_APACHE(unit)) {
        rv = _soc_xgxs_reset_single_tsc(unit, port, reg);
    } else {
        int block_num;
        int pgw_block;
        const static soc_reg_t ctrl_regs_td2plus[] = {
            PGW_TSC0_CTRL_REGr,
            PGW_TSC1_CTRL_REGr,
            PGW_TSC2_CTRL_REGr,
            PGW_TSC3_CTRL_REGr,
        };

        /* For blocks which are not valid resetting the TSC can crash the SDK */
        block_num = (SOC_INFO(unit).port_l2p_mapping[port]-1)/16;
        pgw_block =PGW_CL_BLOCK(unit, block_num);
        if ( pgw_block < 0 || !SOC_INFO(unit).block_valid[pgw_block]) {
            return rv;
        }
        if (!SOC_IS_TD2P_TT2P(unit)) {
            reg = ctrl_regs[reg_idx];
        } else {
            reg = ctrl_regs_td2plus[reg_idx];
        }
        /* for TD2 and TD2+ non-100G ports, reset only one TSC*/
        if (!SOC_IS_TD2P_TT2P(unit) ||
            SOC_INFO(unit).port_speed_max[port] < 100000) {
            rv = _soc_xgxs_reset_single_tsc(unit, port, reg);
        } else {
            rv = _soc_xgxs_reset_tsc_triplet(unit, port);
        }
    }
    return rv;
}

/*
 * function: soc_tsc_xgxs_power_mode
 * purpose:  Reset all TSCs associated with the passed port.
*/
soc_error_t
soc_tsc_xgxs_power_mode(int unit, soc_port_t port, int reg_idx, int power_down)
{
    soc_error_t rv = SOC_E_NONE;
    soc_reg_t reg = XLPORT_XGXS0_CTRL_REGr;

    /* must check if "port" is a block before IS_CL_PORT */
    if (SOC_IS_BLKID_PORT(port)) {
        if (SOC_BLOCK_TYPE_BLKID_PORT(unit, port) == SOC_BLK_PMQPORT) {
            reg = PMQ_XGXS0_CTRL_REGr;
        }
    } else if (IS_CL_PORT(unit, port)) {
        reg = CLPORT_XGXS0_CTRL_REGr;

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
        /* CLPORT act as refclk master should always be active */
        if (soc_feature(unit, soc_feature_pm_refclk_master)) {
            int clport_blk, phy_port;
            soc_info_t *si = &SOC_INFO(unit);

            phy_port = si->port_l2p_mapping[port];
            clport_blk = SOC_PORT_BLOCK(unit, phy_port);
            if (SHR_BITGET(si->pm_ref_master_bitmap, clport_blk)) {
                return SOC_E_NONE;
            }
        }
#endif
    }

    if (power_down) {
        rv = _soc_xgxs_powerdown_single_tsc(unit, port, reg);
    } else {
        rv = _soc_xgxs_powerup_single_tsc(unit, port, reg);
    }
    return rv;
}

int
soc_tsc_xgxs_pll_check(int unit, soc_port_t port, int reg_idx)
{
    const static soc_field_t status_fields[] = {
        TSC_0_AFE_PLL_LOCKf, TSC_1_AFE_PLL_LOCKf,
        TSC_2_AFE_PLL_LOCKf, TSC_3_AFE_PLL_LOCKf,
        TSC_4_AFE_PLL_LOCKf, TSC_5_AFE_PLL_LOCKf,
        TSC_6_AFE_PLL_LOCKf, TSC_7_AFE_PLL_LOCKf,
        TSC_8_AFE_PLL_LOCKf, TSC_9_AFE_PLL_LOCKf,
        TSC_10_AFE_PLL_LOCKf, TSC_11_AFE_PLL_LOCKf,
        TSC_12_AFE_PLL_LOCKf, TSC_13_AFE_PLL_LOCKf,
        TSC_14_AFE_PLL_LOCKf, TSC_15_AFE_PLL_LOCKf,
        TSC_16_AFE_PLL_LOCKf, TSC_17_AFE_PLL_LOCKf,
        TSC_18_AFE_PLL_LOCKf, TSC_19_AFE_PLL_LOCKf,
        TSC_20_AFE_PLL_LOCKf, TSC_21_AFE_PLL_LOCKf,
        TSC_22_AFE_PLL_LOCKf, TSC_23_AFE_PLL_LOCKf,
        TSC_24_AFE_PLL_LOCKf, TSC_25_AFE_PLL_LOCKf,
        TSC_26_AFE_PLL_LOCKf, TSC_27_AFE_PLL_LOCKf,
        TSC_28_AFE_PLL_LOCKf, TSC_29_AFE_PLL_LOCKf,
        TSC_30_AFE_PLL_LOCKf, TSC_31_AFE_PLL_LOCKf,
    };
    soc_reg_t   reg;
    soc_field_t field;
    uint32      rval;
    int         retry;
    int         lock_sleep_usec = 5000;

    reg = TOP_TSC_AFE_PLL_STATUSr;
    field = status_fields[SOC_INFO(unit).port_serdes[port]];

    retry = 10;

    /* Check TxPLL lock status */
    while (retry > 0) {
        SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &rval));
        if (soc_reg_field_get(unit, reg, rval, field)) {
            return SOC_E_NONE;
        }
        sal_usleep(lock_sleep_usec);
        retry--;
    }

    LOG_ERROR(BSL_LS_SOC_COMMON,
              (BSL_META_U(unit,
                          "unit %d TSC %d TXPLL not locked\n"),
               unit, SOC_INFO(unit).port_serdes[port]));
    return SOC_E_NONE;
}
#endif /* BCM_TRIDENT2_SUPPORT || BCM_HURRICANE2_SUPPORT */

#if defined(BCM_TOMAHAWK2_SUPPORT) || defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      soc_xgxs_reset_master_tsc
 * Purpose:
 *      Reset TSCs that need to be initialized to master mode.
 *      In Tomahawk2, there are 8 port macros that will driven directly from
 *      the system board and they will act as the refclk master to drive the
 *      other instances. These port macros should always be initialized to
 *      master mode and should always be active.
 * Parameters:
 *      unit - SOC unit #
 * Returns:
 *      SOC_E_XXX
 */

int
soc_xgxs_reset_master_tsc(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int clport_blk, block;
    uint64 rval64;
    uint32 addr;
    uint8 at;
    soc_reg_t reg = CLPORT_XGXS0_CTRL_REGr;
    int sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;

    if (soc_feature(unit, soc_feature_pm_refclk_master)) {

        addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0, TRUE, &block, &at);

        SHR_BIT_ITER(si->pm_ref_master_bitmap, SOC_MAX_NUM_BLKS, clport_blk) {
            block = SOC_BLOCK_INFO(unit, clport_blk).cmic;

            /* Set TSC in master mode */
            SOC_REG_RST_VAL_GET(unit, CLPORT_XGXS0_CTRL_REGr, rval64);
            soc_reg64_field32_set(unit, reg, &rval64, REFIN_ENf, 0);
            soc_reg64_field32_set(unit, reg, &rval64, REFOUT_ENf, 1);
            SOC_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));

            /*
             * For TH2 default behaviour of CLPORT port IDDQ is changed, port
             * init sequence must be adjusted to bring tsc out of reset properly
             */
            if (soc_feature(unit, soc_feature_iddq_new_default)) {
                if (soc_reg_field_valid(unit, reg, IDDQf)) {
                    soc_reg64_field32_set(unit, reg, &rval64, IDDQf, 0);
                }
            }

            /* Deassert power down */
            soc_reg64_field32_set(unit, reg, &rval64, PWRDWNf, 0);
            SOC_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));
            sal_usleep(sleep_usec);

            /* Reset XGXS */
            soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 0);
            SOC_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));

            sal_usleep(sleep_usec + 10000);

            /* Bring XGXS out of reset */
            soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 1);
            SOC_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));
            sal_usleep(sleep_usec);
        }
    }

    return SOC_E_NONE;
}
#endif /* BCM_TOMAHAWK2_SUPPORT || BCM_TRIDENT3_SUPPORT */

#ifdef BCM_XGS5_SWITCH_PORT_SUPPORT
int
soc_pm_power_mode_set(int unit, soc_port_t port, int reg_idx, int power_down)
{
#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_HURRICANE2_SUPPORT) || \
    defined(BCM_SABER2_SUPPORT)
    SOC_IF_ERROR_RETURN
        (soc_tsc_xgxs_power_mode(unit, port, reg_idx, power_down));
#endif
    return SOC_E_NONE;
}
#endif /* BCM_XGS5_SWITCH_PORT_SUPPORT */

#if defined(BCM_GREYHOUND2_SUPPORT)
/*
 * function: soc_sgmii4px2_reset
 * purpose:  Reset all SGMII4PX2.
 */
soc_error_t
soc_sgmii4px2_reset(int unit, soc_port_t port, int reg_idx)
{
    const static soc_reg_t ctrl_regs[] = {
        GPORT_SGMII0_CTRL_REGr,
        GPORT_SGMII1_CTRL_REGr
    };
    uint32      rval;
    int         sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;

    soc_reg_t reg = ctrl_regs[reg_idx];

    /*
     * Reference clock selection
     */
    SOC_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
    soc_reg_field_set(unit, reg, &rval, IDDQf, 1);
    soc_reg_field_set(unit, reg, &rval, PWRDWNf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    sal_usleep(sleep_usec);

    /* Analog section powered */
    soc_reg_field_set(unit, reg, &rval, IDDQf, 0);
    /* Deassert power down */
    soc_reg_field_set(unit, reg, &rval, PWRDWNf, 0);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    sal_usleep(sleep_usec);

    /* Bring SGMII out of reset */
    soc_reg_field_set(unit, reg, &rval, RSTB_HWf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    sal_usleep(sleep_usec);

    /* Activate MDIO on SGMII */
    soc_reg_field_set(unit, reg, &rval, RSTB_MDIOREGSf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    sal_usleep(sleep_usec);

    /* Activate clocks */
    soc_reg_field_set(unit, reg, &rval, RSTB_PLLf, 1);
    SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));

    return SOC_E_NONE;
}
#endif /* BCM_GREYHOUND2_SUPPORT */

int soc_is_valid_block_instance(int unit, soc_block_types_t block_types,
                                int block_instance, int *is_valid)
{

    int nof_block_instances;

    if (is_valid == NULL) {
        return SOC_E_PARAM;
    }

    if (block_instance < 0) {
        return SOC_E_PARAM;
    }

    /*default*/
    nof_block_instances = 0;

#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        int rv = soc_dfe_nof_block_instances(unit, block_types, &nof_block_instances);
        SOC_IF_ERROR_RETURN(rv);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)) {
        int rv = soc_dnxf_nof_block_instances(unit, block_types, &nof_block_instances);
        SOC_IF_ERROR_RETURN(rv);
    }
#endif
#ifdef BCM_PETRA_SUPPORT
    if(SOC_IS_ARAD(unit)) {
        int rv = soc_dpp_nof_block_instances(unit, block_types, &nof_block_instances);
        SOC_IF_ERROR_RETURN(rv);
    }
#endif

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit)) {
        int rv = soc_dnxc_nof_block_instances(unit, block_types, &nof_block_instances);
        SOC_IF_ERROR_RETURN(rv);
    }
#endif
    *is_valid = (block_instance < nof_block_instances );

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phy_firmware_load
 * Purpose:
 *      Load firmware into internal SerDes core.
 * Parameters:
 *      unit - unit number
 *      port - port number on device.
 *      fw_data - firmware (binary byte array)
 *      fw_size - size of firmware (in bytes)
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      This function is simply a wrapper for a device-specific implementation.
 */

int
soc_phy_firmware_load(int unit, int port, uint8 *fw_data, int fw_size)
{
    soc_phy_firmware_load_f firmware_load;

    firmware_load = SOC_FUNCTIONS(unit)->soc_phy_firmware_load;
    if (firmware_load != NULL) {
        return firmware_load(unit, port, fw_data, fw_size);
    }
    return SOC_E_UNAVAIL;
}


#ifdef BCM_LEDPROC_SUPPORT

/*
 * Function:
 *      soc_ledproc_config
 * Purpose:
 *      Load a program into the LED microprocessor from a buffer
 *      and start it running.
 * Parameters:
 *      unit - StrataSwitch unit #
 *      program - Array of up to 256 program bytes
 *      bytes - Number of bytes in array, 0 to disable ledproc, upper 16bit
 *              specifies which LED processor(0,1,2, ...).
 * Notes:
 *      Also clears the LED processor data RAM from 0x80-0xff
 *      so the LED program has a known state at startup.
 */

int
soc_ledproc_config(int unit, const uint8 *program, int bytes)
{
    int         offset;
    uint32      val;
    uint32      led_ctrl;
    uint32      led_prog_base;
    uint32      led_data_base;
#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    int         led_num;
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_TRIUMPH3_SUPPORT */

    if (!soc_feature(unit, soc_feature_led_proc)) {
        return SOC_E_UNAVAIL;
    }

#if defined(BCM_TRIDENT_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
    led_num = (bytes >> 16) & 0xff;  /* led processor instance */
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_TRIUMPH3_SUPPORT */
    bytes &= 0xffff;

#ifdef BCM_TOMAHAWK_SUPPORT
    if (SOC_IS_TOMAHAWK(unit) && soc_feature(unit, soc_feature_led_cmicd_v2)) {
        /* CMICd v2 supports 3 LED processors */
        if (led_num == 0) {
            led_ctrl = CMIC_LEDUP0_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP0_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
        } else if (led_num == 1) {
            led_ctrl = CMIC_LEDUP1_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP1_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP1_DATA_RAM_OFFSET;
        } else if (led_num == 2) {
            led_ctrl = CMIC_LEDUP2_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP2_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP2_DATA_RAM_OFFSET;
        } else {
            return SOC_E_PARAM;
        }
    } else
#endif /* BCM_TOMAHAWK_SUPPORT */
#ifdef BCM_TOMAHAWK2_SUPPORT
    if (SOC_IS_TOMAHAWK2(unit) && soc_feature(unit, soc_feature_led_cmicd_v4)) {
        /* CMICd v4 supports 5 LED processors */
        if (led_num == 0) {
            led_ctrl = CMIC_LEDUP0_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP0_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
        } else if (led_num == 1) {
            led_ctrl = CMIC_LEDUP1_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP1_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP1_DATA_RAM_OFFSET;
        } else if (led_num == 2) {
            led_ctrl = CMIC_LEDUP2_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP2_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP2_DATA_RAM_OFFSET;
        } else if (led_num == 3) {
            led_ctrl = CMIC_LEDUP3_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP3_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP3_DATA_RAM_OFFSET;
        } else if (led_num == 4) {
            led_ctrl = CMIC_LEDUP4_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP4_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP4_DATA_RAM_OFFSET;
        } else {
            return SOC_E_PARAM;
        }
    } else
#endif /* BCM_TOMAHAWK2_SUPPORT */
#ifdef BCM_CMICM_SUPPORT
    if (soc_feature(unit, soc_feature_cmicm)) {
#if defined(BCM_TRIDENT2_SUPPORT)
        /* CMICm supports 2 LED processors */
        if (SOC_IS_TD2_TT2(unit)) {
            if (led_num == 0) {
                led_ctrl = CMIC_LEDUP0_CTRL_OFFSET;
                led_prog_base = CMIC_LEDUP0_PROGRAM_RAM_OFFSET;
                led_data_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
            } else if (led_num == 1) {
                led_ctrl = CMIC_LEDUP1_CTRL_OFFSET;
                led_prog_base = CMIC_LEDUP1_PROGRAM_RAM_OFFSET;
                led_data_base = CMIC_LEDUP1_DATA_RAM_OFFSET;
            } else {
                return SOC_E_PARAM;
            }
        } else
#endif
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit) || SOC_IS_GREYHOUND2(unit)) {
            if (led_num == 0) {
                led_ctrl = CMIC_LEDUP0_CTRL_OFFSET;
                led_prog_base = CMIC_LEDUP0_PROGRAM_RAM_OFFSET;
                led_data_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
            } else if (led_num == 1) {
                led_ctrl = CMIC_LEDUP1_CTRL_OFFSET;
                led_prog_base = CMIC_LEDUP1_PROGRAM_RAM_OFFSET;
                led_data_base = CMIC_LEDUP1_DATA_RAM_OFFSET;
            } else {
                return SOC_E_PARAM;
            }
        } else
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_GREYHOUND2_SUPPORT */
        {
            led_ctrl = CMIC_LEDUP0_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP0_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
        }
    } else
#endif

    /* use soc_feature if more switches use two ledprocs */
#ifdef BCM_TRIDENT_SUPPORT
    if (SOC_IS_TD_TT(unit)) {
        if (led_num == 0) {
            led_ctrl = CMICE_LEDUP0_CTRL;
            led_prog_base = CMICE_LEDUP0_PROGRAM_RAM_BASE;
            led_data_base = CMICE_LEDUP0_DATA_RAM_BASE;
        } else if (led_num == 1) { /* Trident only has two */
            led_ctrl = CMICE_LEDUP1_CTRL;
            led_prog_base = CMICE_LEDUP1_PROGRAM_RAM_BASE;
            led_data_base = CMICE_LEDUP1_DATA_RAM_BASE;
        } else {
            return SOC_E_PARAM;
        }
    } else
#endif /* BCM_TRIDENT_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_GREYHOUND2_SUPPORT)
#ifdef BCM_CMICM_SUPPORT
    if (SOC_IS_TRIUMPH3(unit) || SOC_IS_GREYHOUND2(unit)) {
        if (led_num == 0) {
            led_ctrl = CMIC_LEDUP0_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP0_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP0_DATA_RAM_OFFSET;
        } else if (led_num == 1) {
            led_ctrl = CMIC_LEDUP1_CTRL_OFFSET;
            led_prog_base = CMIC_LEDUP1_PROGRAM_RAM_OFFSET;
            led_data_base = CMIC_LEDUP1_DATA_RAM_OFFSET;
        } else {
            return SOC_E_PARAM;
        }
    } else
#endif
#endif /* BCM_TRIUMPH3_SUPPORT || BCM_GREYHOUND2_SUPPORT */
    {
        /* Only Single LED Processor used.. */
        led_ctrl = CMIC_LED_CTRL;
        led_prog_base = CMIC_LED_PROGRAM_RAM_BASE;
        led_data_base = CMIC_LED_DATA_RAM_BASE;
    }

    val = soc_pci_read(unit, led_ctrl);
    val &= ~LC_LED_ENABLE;
    soc_pci_write(unit, led_ctrl, val);

    if (bytes == 0) {
        return SOC_E_NONE;
    }

    for (offset = 0; offset < CMIC_LED_PROGRAM_RAM_SIZE; offset++) {
        soc_pci_write(unit,
                        led_prog_base + CMIC_LED_REG_SIZE * offset,
                        (offset < bytes) ? (uint32) program[offset] : 0);
    }

    for (offset = 0x80; offset < CMIC_LED_DATA_RAM_SIZE; offset++) {
        soc_pci_write(unit,
                        led_data_base + CMIC_LED_REG_SIZE * offset,
                        0);
    }

    val = soc_pci_read(unit, led_ctrl);
    val |= LC_LED_ENABLE;
    soc_pci_write(unit, led_ctrl, val);

    return SOC_E_NONE;
}

#endif /* BCM_LEDPROC_SUPPORT */



#ifdef BCM_CMICX_SUPPORT
static soc_led_driver_t cmicx_led_driver = {
    soc_cmicx_led_uc_num_get,
    soc_cmicx_led_fw_load,
    soc_cmicx_led_fw_start_get,
    soc_cmicx_led_fw_start_set,
    soc_cmicx_led_control_data_write,
    soc_cmicx_led_control_data_read,
    soc_cmicx_led_port_to_uc_port_get,
    soc_cmicx_led_port_to_uc_port_set,
};
#endif

/*
 * Function:
 *     soc_led_driver_init
 * Purpose:
 *     Init LED driver.
 * Parameters:
 *     unit Unit number
 * Returns:
 *     SOC_E_xxx
 */
int
soc_led_driver_init(int unit)
{
    SOC_LED_DRIVER(unit) = NULL;
#ifdef BCM_CMICX_SUPPORT
    if (soc_feature(unit, soc_feature_cmicx)) {
        SOC_LED_DRIVER(unit) = &cmicx_led_driver;
    }
#endif
    return SOC_E_NONE;
}

/*
 * Function:
 *     soc_led_control_data_read
 * Purpose:
 *     Read control data of LED firmware.
 * Parameters:
 *     unit Unit number.
 *     led_uc LED microcontroller number.
 *     offset Position to start led control data read.
 *     data Read data buffer.
 *     len Length of read data.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_led_control_data_read(int unit, int led_uc, int offset, uint8 *data, int len)
{
    return SOC_LED_CONTROL_DATA_READ(unit, led_uc, offset, data, len);
}

/*
 * Function:
 *     soc_led_control_data_write
 * Purpose:
 *     Write control data of LED firmware.
 * Parameters:
 *     unit Unit number.
 *     led_uc LED microcontroller number.
 *     offset Position to start led control data write.
 *     data Write data buffer.
 *     len Length of write data.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_led_control_data_write(int unit, int led_uc, int offset, const uint8 *data, int len)
{
    return SOC_LED_CONTROL_DATA_WRITE(unit, led_uc, offset, data, len);
}

/*
 * Function:
 *     soc_led_fw_load
 * Purpose:
 *     Load LED firmware binary.
 * Parameters:
 *     unit Unit number
 *     led_uc LED microcontroller number.
 *     data Firmware binary content.
 *     len Length of firmware binary.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_led_fw_load(int unit, int led_uc, const uint8 *data, int len)
{
    return SOC_LED_FW_LOAD(unit, led_uc, data, len);
}

/*
 * Function:
 *     soc_led_fw_start_get
 * Purpose:
 *     Get if LED firmware is started or not.
 * Parameters:
 *     unit Unit number.
 *     led_uc LED microcontroller number.
 *     start 1, fw started and 0, fw stopped.
 *     len Length of firmware binary.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_led_fw_start_get(int unit, int led_uc, int *start)
{
    return SOC_LED_FW_START_GET(unit, led_uc, start);
}

/*
 * Function:
 *     soc_led_fw_start_set
 * Purpose:
 *     Start/stop LED firmware.
 * Parameters:
 *     unit Unit number.
 *     led_uc LED microcontroller number.
 *     start 1, request fw start and 0, request fw stop.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_led_fw_start_set(int unit, int led_uc, int start)
{
    return SOC_LED_FW_START_SET(unit, led_uc, start);
}

/*
 * Function:
 *     soc_led_port_to_uc_port_get
 * Purpose:
 *     Map physical port to LED microcontroller number and port index.
 * Parameters:
 *     unit Unit number.
 *     port Physical port number.
 *     led_uc LED microcontroller number.
 *     led_uc_port Port index controlled by correponding LED microcontroller.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_led_port_to_uc_port_get(int unit, int port, int *led_uc, int *led_uc_port)
{
    return SOC_LED_PORT_TO_UC_PORT_GET(unit, port, led_uc, led_uc_port);
}

/*
 * Function:
 *     soc_led_port_to_uc_port_set
 * Purpose:
 *     Configure the mapping from physical port to LED microcontroller number and port index.
 * Parameters:
 *     unit Unit number.
 *     port Physical port number.
 *     led_uc LED microcontroller number.
 *     led_uc_port Port index controlled by correponding LED microcontroller.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_led_port_to_uc_port_set(int unit, int port, int led_uc, int led_uc_port)
{
    return SOC_LED_PORT_TO_UC_PORT_SET(unit, port, led_uc, led_uc_port);
}

/*
 * Function:
 *     soc_led_uc_num_get
 * Purpose:
 *     Get number of LED microcontrollers.
 * Parameters:
 *     unit Unit number.
 *     led_uc_num Number of LED microcontrollers.
 * Returns:
 *     SOC_E_xxx
 */
int
soc_led_uc_num_get(int unit, int *led_uc_num)
{
    return SOC_LED_UC_NUM_GET(unit, led_uc_num);
}

/*******************************************
* @function soc_sbusdma_lock_init
* purpose API to Initialize SBUSDMA
* locks and Semaphores. This will also be used
* for TSLAMDMA and TABLEDMA
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
* @returns SOC_E_XXX
*
* @end
 */
int soc_sbusdma_lock_init(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc, ch;

    /* init locks */
    soc->tdma_enb = 0;
    soc->tslam_enb = 0;
    for (cmc = 0; cmc < SOC_CMCS_NUM_MAX; cmc++) {
        for (ch = 0; ch < SOC_SBUSDMA_CH_PER_CMC; ch++) {
            soc->sbusDmaMutexs[cmc][ch] = NULL;
            soc->sbusDmaIntrs[cmc][ch] = NULL;
        }
    }
    if (SAL_BOOT_QUICKTURN) {
        soc->sbusDmaTimeout = SBUS_TIMEOUT_QT;
    } else {
        soc->sbusDmaTimeout = SBUS_TIMEOUT;
    }
    soc->sbusDmaTimeout = soc_property_get(unit, spn_DMA_DESC_TIMEOUT_USEC,
                                       soc->sbusDmaTimeout);
    SOC_STAT(unit)->err_sbusdma_intr_res = 0;
    /* by default it will be cmc0 when there is no multi pci cmc property */
    if (soc->sbusDmaTimeout) {
        for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
            for (ch = 0; ch < SOC_SBUSDMA_CH_PER_CMC; ch++) {
                soc->sbusDmaMutexs[cmc][ch] = sal_mutex_create("SbusDMA");
                if (soc->sbusDmaMutexs[cmc][ch] == NULL) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                          "failed to allocate sbusDmaMutexs")));
                    (void)soc_sbusdma_lock_deinit(unit);
                    return SOC_E_MEMORY;
                }
                soc->sbusDmaIntrs[cmc][ch] = sal_sem_create("SBUSDMA interrupt",
                                                            sal_sem_BINARY, 0);
                if (soc->sbusDmaIntrs[cmc][ch] == NULL) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META_U(unit,
                          "failed to allocate sbusDmaIntrs")));
                    (void)soc_sbusdma_lock_deinit(unit);
                    return SOC_E_MEMORY;
                }
            }
        }

        soc->sbusDmaIntrEnb = soc_property_get(unit,
                                               spn_DMA_DESC_INTR_ENABLE, 0);
    }

    /* Init TDMA/ TSLAM */

    if (soc_feature(unit, soc_feature_table_dma) &&
        soc_property_get(unit, spn_TABLE_DMA_ENABLE, 1)) {
        if (SAL_BOOT_QUICKTURN) {
            soc->tableDmaTimeout = TDMA_TIMEOUT_QT;
        } else {
            soc->tableDmaTimeout = TDMA_TIMEOUT;
#ifdef BCM_CMICX_SUPPORT
        if(soc_feature(unit, soc_feature_cmicx)) {
            soc->tableDmaTimeout = 3 * TDMA_TIMEOUT;
        }
#endif /* CMICX Support */
        }
        soc->tableDmaTimeout = soc_property_get(unit, spn_TDMA_TIMEOUT_USEC,
                                                soc->tableDmaTimeout);
        soc->tableDmaIntrEnb = soc_property_get(unit,
                                                    spn_TDMA_INTR_ENABLE, 1);
        soc->tdma_enb = 1;
    }

    if (soc_feature(unit, soc_feature_tslam_dma) &&
        soc_property_get(unit, spn_TSLAM_DMA_ENABLE, 1)) {
        if (SAL_BOOT_QUICKTURN) {
            soc->tslamDmaTimeout = TSLAM_TIMEOUT_QT;
        } else {
            soc->tslamDmaTimeout = TSLAM_TIMEOUT;
#ifdef BCM_CMICX_SUPPORT
    if(soc_feature(unit, soc_feature_cmicx)) {
        soc->tslamDmaTimeout = 3 * TSLAM_TIMEOUT;
    }
#endif /* CMICX Support */
        }
        soc->tslamDmaTimeout = soc_property_get(unit, spn_TSLAM_TIMEOUT_USEC,
                                                soc->tslamDmaTimeout);
        soc->tslamDmaIntrEnb = soc_property_get(unit,
                                              spn_TSLAM_INTR_ENABLE, 1);
        soc->tslam_enb = 1;
    }

    /* Init default channels */
    soc->tdma_ch = 0;
    soc->tslam_ch = 1;
#ifdef BCM_SBUSDMA_SUPPORT
    soc->desc_ch = 2;
#endif

#if defined(SOC_PCI_DEBUG) && defined(INCLUDE_CPU_I2C)
        /* Disable DMA for I2C access */
        if (SAL_BOOT_I2C_ACCESS) {
            SOC_CONTROL(unit)->tdma_enb = 0;
            SOC_CONTROL(unit)->tslam_enb = 0;
        }
#endif
#if defined(BCM_SAND_SUPPORT)
    /* In the current DMA implementation, SLAM and table DMA need to both be enabled or disabled */
    if (soc->tslam_enb != soc->tdma_enb && SOC_IS_SAND(unit)) {
        LOG_ERROR(BSL_LS_SOC_COMMON, (BSL_META_U(unit, "SLAM DMA and table DMA need to both be either enabled or disabled\n")));
        return SOC_E_PARAM;
    }
#endif /* defined(BCM_SAND_SUPPORT) */
    return SOC_E_NONE;
}

/*******************************************
* @function soc_sbusdma_lock_deinit
* purpose API to deinit SBUSDMA
* locks and Semaphores
*
* @param unit [in] unit
*
* @returns SOC_E_NONE
*
* @end
 */
int soc_sbusdma_lock_deinit(int unit)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    int cmc, ch;

    for (cmc = 0; cmc < SOC_PCI_CMCS_NUM(unit); cmc++) {
        for (ch = 0; ch < SOC_SBUSDMA_CH_PER_CMC; ch++) {
            if (soc->sbusDmaMutexs[cmc][ch]) {
                sal_mutex_destroy(soc->sbusDmaMutexs[cmc][ch]);
                soc->sbusDmaMutexs[cmc][ch] = NULL;
            }
            if (soc->sbusDmaIntrs[cmc][ch]) {
                sal_sem_destroy(soc->sbusDmaIntrs[cmc][ch]);
                soc->sbusDmaIntrs[cmc][ch] = NULL;
            }
        }
    }
    soc->tdma_enb = 0;
    soc->tslam_enb = 0;
    return SOC_E_NONE;
}
static soc_chip_info_vectors_t chip_info_vect;

int
soc_chip_info_vect_config(soc_chip_info_vectors_t *vect)
{
    if (vect == NULL) {
        return SOC_E_PARAM;
    }

    chip_info_vect = *vect;

    return SOC_E_NONE;
}

int
soc_icid_default_get(int unit, uint8 *data, int size)
{
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META("Get default ICID data length "
                          "%d bytes.\n"), size));

    if (!data) {
        return SOC_E_PARAM;
    }

    if (chip_info_vect.icid_get) {
        if (chip_info_vect.icid_get(unit, data, size) < 0) {
            return SOC_E_UNAVAIL;
        }
    } else {
        return SOC_E_UNAVAIL;
    }

    return SOC_E_NONE;
}

#ifdef BCM_MIXED_LEGACY_AND_PORTMOD_SUPPORT
int soc_port_use_portctrl(int unit, int port)
{
    if (SOC_FUNCTIONS(unit) &&
        SOC_FUNCTIONS(unit)->soc_port_use_portctrl) {
        return SOC_FUNCTIONS(unit)->soc_port_use_portctrl(unit, port);
    }
    return TRUE;
}
#endif /* BCM_MIXED_LEGACY_AND_PORTMOD_SUPPORT */

/*
 * Function:
 *      soc_cpu_flow_ctrl
 * Purpose:
 *      Enable/Disable flow control assertion from CMIC to MMU.
 * Parameters:
 *      unit - Unit to init
 *      enable - Enable/Disable flow control assertion.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_cpu_flow_ctrl(int unit, int enable) {

    /* Input validation */
    if (!SOC_UNIT_VALID(unit)) {
        return SOC_E_UNIT;
    }

#if defined(BCM_HURRICANE2_SUPPORT)
    if (SOC_IS_HURRICANE2(unit) || SOC_IS_HURRICANE3(unit) ||
        SOC_IS_GREYHOUND(unit)) {
        uint32 addr, fval, rval = 0;

        if (enable) {
            fval = 0x00000000;
        } else {
            fval = 0xffffffff;
        }
        soc_reg_field_set(unit, CMIC_PKT_COS_0r, &rval, COSf, fval);
        addr = soc_reg_addr(unit, CMIC_PKT_COS_0r, REG_PORT_ANY, 0);
        soc_pci_write(unit, addr, rval);
        soc_reg_field_set(unit, CMIC_PKT_COS_1r, &rval, COSf, fval);
        addr = soc_reg_addr(unit, CMIC_PKT_COS_1r, REG_PORT_ANY, 0);
        soc_pci_write(unit, addr, rval);
    }
#endif /* BCM_HURRICANE2_SUPPORT */
    return SOC_E_NONE;
}

#if defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT)
void
soc_verify_fast_init_set(
    int enable)
{
    dnxc_verify_fast_init_enable(enable);
}
#endif /* defined (BCM_DNX_SUPPORT) || defined (BCM_DNXF_SUPPORT) */
