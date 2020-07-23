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
 */
#ifndef _DPP_FIELD_INT_H_
#define _DPP_FIELD_INT_H_

/*
 *  This file contains private declarations (private to the field API
 *  implementations) that have been moved to here because the module has
 *  been split across a few files in order to certain features.
 *
 *  IT SHOULD NOT BE INCLUDED FROM MODULES OUTSIDE OF FIELD.
 */
#include <bcm/stg.h>
#include <bcm/field.h>
#include <shared/idxres_fl.h>
#include <shared/shr_resmgr.h>
#include <bcm_int/dpp/field.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_dbal.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/ARAD/arad_kbp.h>

/*
 * SW_STATE Access
 */
#define FIELD_ACCESS  sw_state_access[unit].dpp.bcm.field

/*
 *  Right now, warm boot support for field APIs on DPP devices is a work in
 *  progress.  Basically, it is unfinished (backing store space is allocated,
 *  writes to backing store are performed, but recovery is not in place).  For
 *  testing purposes, it has been included in the branches for builds and
 *  validation, to ensure that it does not cause undesirable effects as the
 *  work progresses.
 *
 *  However, since it is unfinished, it merely consumes resources and provides
 *  no particular benefit in the field.  It should therefore remain disabled,
 *  so _BCM_DPP_FIELD_WARM_BOOT_SUPPORT should be set to FALSE.
 */
#define _BCM_DPP_FIELD_WARM_BOOT_SUPPORT TRUE

/******************************************************************************
 *
 *  Exports to submodules (see field module for code comments)
 */

/*
 *  _DPP_PETRA_FIELD_PAGE_WIDTH specifies how wide the console should be for
 *  certain debugging functions that tend to generate lists (so they can be
 *  formatted to make better use of the available console space).  Don't make
 *  it more than 16384; use zero if you want no line joining (each element will
 *  be on its own line).
 */
#define _BCM_DPP_FIELD_PAGE_WIDTH              79

/*
 *  Controls whether certain (perhaps excessively) verbose messages will be
 *  emitted by the diagnostics.  These messages are the FIELD_EVERB messages,
 *  which will be emitted at VVERB level if enabled.
 */
#define _BCM_DPP_FIELD_EXCESS_VERBOSITY      TRUE

/*
 *  Control whether symbolic flags are included in the dumps.  They're ugly,
 *  do not flow properly on the display, but they might be helpful if you
 *  don't want to look up the meaning of the bits in the flags fields.
 */
#define _BCM_DPP_FIELD_DUMP_SYM_FLAGS        TRUE

/*
 *  Control whether underlying PPD state is included in the unit dump.  This is
 *  potentially helpful diagnostics, but maybe confusing?
 */
#define _BCM_DPP_FIELD_DUMP_INCLUDE_PPD      TRUE

/*
 *  Dump PPD layer state intermixed with BCM layer state.  This state is taken
 *  from the cached values.
 */
#define _BCM_DPP_FIELD_DUMP_INTERMIX_PPD     TRUE

/*
 *  Verify PPD layer state when dumping BCM layer state.  This reads back the
 *  PPD layer state and compares it to the BCM layer state.  It displays the
 *  mismatches if there are any.
 *
 *  _BCM_DPP_FIELD_DUMP_VERIFY_PPD does not provide particularly useful
 *  diagnostics if _BCM_DPP_FIELD_DUMP_INTERMIX_PPD is FALSE, but you can guess
 *  by looking up the hardware entry ID later in the PPD layer dump, assuming
 *  you have _BCM_DPP_FIELD_DUMP_INCLUDE_PPD set to TRUE.  If both are FALSE,
 *  you get a message/error but nothing more.
 */
#define _BCM_DPP_FIELD_DUMP_VERIFY_PPD       TRUE

/*
 *  If _BCM_DPP_FIELD_DUMP_VERIFY_PPD is TRUE, setting
 *  _BCM_DPP_FIELD_DUMP_VERIFY_ERROR to TRUE will cause the dump functions to
 *  return an error (BCM_E_INTERNAL) if there is a mismatch.
 *
 *  This has no effect if _BCM_DPP_FIELD_DUMP_VERIFY_PPD is FALSE.
 */

#define _BCM_DPP_FIELD_DUMP_VERIFY_ERROR     FALSE

/*
 *  If _BCM_DPP_FIELD_GET_VERIFY_PPD is TRUE, an entry get operation will read
 *  the entry from PPD and verify that its values are consistent with the
 *  expected values.
 *
 *  If _BCM_DPP_FIELD_GET_VERIFY_PPD is FALSE, a get operation will only
 *  read the cached state and use that (the original behavior).
 *
 *  This functionality only applies to explicit gets; it does not apply to
 *  implied gets (such as setting subset qualifiers) or if a get is not
 *  performed (adding a qualifier or action).
 *
 *  This will reduce performance, since it must read the hardware via PPD layer
 *  even when it is accessing something that can be cached.  If an error occurs
 *  retrieving the entry from the PPD layer, that error will be returned,
 *  instead of using the cached value.
 */
#define _BCM_DPP_FIELD_GET_VERIFY_PPD        TRUE

/*
 *  If _BCM_DPP_FIELD_GET_USE_PPD is TRUE, an entry get operation will read the
 *  entry from PPD and use the values so retrieved, where possible.
 *
 *  If _BCM_DPP_FIELD_GET_USE_PPD is FALSE, a get operation will use the cached
 *  state to fill in the value to be retrieved.
 *
 *  Note this only applies where the BCM layer values can be reasonably derived
 *  from the PPD layer values.  For example, qualifiers can be extracted thus,
 *  but TCAM actions do not map cleanly in both directions, so they will always
 *  use the cached values instead.
 *
 *  This will reduce performance, since it must read the hardware via PPD layer
 *  even when it is accessing something that can be cached.  If an error occurs
 *  retrieving the entry from the PPD layer, that error will be returned,
 *  instead of using the cached value.
 */

#define _BCM_DPP_FIELD_GET_USE_PPD           TRUE

/*
 *  If _BCM_DPP_FIELD_GET_VERIFY_PPD is TRUE, setting
 *  _BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR to TRUE will set the get functions
 *  so they return _BCM_DPP_FIELD_GET_VERIFY_ERROR should there be a mismatch
 *  between the PPD state and the expected PPD state during a get operation.
 *
 *  Even if this is FALSE, if _BCM_DPP_FIELD_GET_VERIFY_PPD is TRUE, at least a
 *  diagnostic messages will be emitted should there be a mismatch (but
 *  diagnostic messages will have to be enabled for it to be shown).
 */

#define _BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR FALSE

/*
 *  If _BCM_DPP_FIELD_GET_VERIFY_PPD is TRUE, and
 *  _BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR is TRUE, then
 *  _BCM_DPP_FIELD_GET_VERIFY_ERROR selects the error code that will be
 *  returned on mismatch.
 *
 *  This has no effect if _BCM_DPP_FIELD_GET_VERIFY_PPD is FALSE, or if
 *  _BCM_DPP_FIELD_GET_VERIFY_RETURN_ERROR is FALSE.
 *
 *  This will not override any other error that occurs.
 */
#define _BCM_DPP_FIELD_GET_VERIFY_ERROR      BCM_E_FAIL

/*
 *  Control whether resource map is included in the unit dump.  Should normally
 *  be FALSE because it does not honor the prefix argument, but can be useful
 *  for some sorts of debugging.
 */
#define _BCM_DPP_UNIT_DUMP_INCLUDE_RES_DUMP  TRUE

/*
 *  _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE controls whether adding the same
 *  action more than once is allowed.  Since Soc_petra only supports one set of
 *  values for an action, this would imply a replacement of the old action's
 *  parameters with the new action's parameters.  If this is TRUE, and both
 *  actions are the same enumeration from bcm_field_ation_t, the parameter
 *  values from the new action will replace those from the old action.  If this
 *  is FALSE, adding the same enumeration from bcm_field_action_t more than
 *  once (without removing the older one) is BCM_E_EXISTS.
 */
#define _BCM_DPP_ALLOW_IMPLIED_ACTION_REPLACE TRUE

/*
 *  Control whether the direct extraction code will permit compound actions to
 *  be used (but even if enabled, will only use the first action).  Some BCM
 *  actions involve more than one action; on TCAM, this is easy to emulate, but
 *  for direct extraction, only the first hardware action will be used, and the
 *  application must explicitly set up both.
 */
#define _BCM_DPP_ALLOW_DIR_EXT_COMPOUND_ACTIONS TRUE

/*
 *  Bias value for IDs of each type of entry.  TCAM entries start with zero;
 *  direct extraction entries start with this value; as other entry types get
 *  included, their IDs will start with some counting integer times this.
 *
 *  This is used instead of simply tacking the block into a contiguous number
 *  space so that WITH_ID calls can possibly work across different devices.
 * 
 *  Extend this number to the maximum number of entries in TCAM (internal
 *  or external)
 */
#define _BCM_DPP_FIELD_ENTRY_TYPE_BIAS(unit)  \
    SOC_SAND_MAX(SOC_TMC_TCAM_NL_88650_MAX_NOF_ENTRIES, SOC_DPP_DEFS_MAX_NOF_ENTRY_IDS)

/*
 *  An earlier version of this file exposed the Dune PPD bit numbering at the
 *  BCM layer for programmable fields, but that numbering is inconsistent with
 *  the standard used in the BCM APIs.  In order to avoid breaking existing
 *  applications, this switch controls whether to use the Dune bit numbering
 *  for programmable fields (TRUE) or the BCM numbering (FALSE).  It only
 *  affects the 'starting bit' number for programmable fields; it does not
 *  affect the order of bits within other arguments.
 *
 *  Dune ordering calls the Most Significant Bit of a byte bit zero, while BCM
 *  ordering calls the Most Significant Bit of a byte bit seven.  The binary
 *  values of each bit in Dune notation is thus 2^(7-bit_number) while in the
 *  BCM notation it is 2^(bit_number), thus:
 *
 *  Bit value     = 128  64  32  16   8   4   2   1
 *  Dune bit num  =   0   1   2   3   4   5   6   7
 *  BCM bit num   =   7   6   5   4   3   2   1   0
 *
 *  The BCM numbering is consistent with Ethernet bit numbering, while the Dune
 *  bit numbering seems more visually consistent across bytes:
 *
 *  Byte         ----------- 0 ----------  ----------- 1 ----------
 *  Bit values   128 64 32 16  8  4  2  1  128 64 32 16  8  4  2  1
 *  Dune bits      0  1  2  3  4  5  6  7    8  9 10 11 12 13 14 15
 *  BCM bits       7  6  5  4  3  2  1  0   15 14 13 12 11 10  9  8
 *
 *  The result of this is, for example, to get the lower nibble of byte 1 in
 *  Dune bit numbering mode requires the programmable field to start at bit 12
 *  and run for four bits; to get the same nibble in BCM bit numbering mode
 *  requires the field to start at bit 11 and run for four bits.  It's more
 *  confusing when spanning bytes.  To get the lower nibble of byte 0 and the
 *  upper nibble of byte 1, Dune mode would start at bit 4 and run for eight
 *  bits (so including bits 4,5,6,7,8,9,10,11), while BCM mode would start at
 *  bit 3 and run for eight bits (so including bits 3,2,1,0,15,14,13,12).  The
 *  numbering continues in these manners for further bytes, so byte 3 is bits
 *  16..23 or 23..16, byte 4 is bits 24..31 or 31..24, and so on.
 *
 *  Note that these programmable fields start at the most significant bit of
 *  the field in these APIs, no matter this setting.
 *
 *  Programmable fields specified in byte mode (where applicable) are not
 *  affected by this setting: a byte starts with its most significant bit.
 *
 *  Note that in both cases, bytes are numbered in network order, and in both
 *  cases the actual bit values does not change (so the value of a byte is the
 *  same no matter the bit order selected).
 */
#define _BCM_DPP_FIELD_BIT_NUMBER_AS_DUNE FALSE

/*
 *  I can't think of why you'd want to turn off certain displays, but there
 *  is a way to do so here...
 */
#define BCM_DPP_FIELD_PRINT 1

#if defined(BROADCOM_DEBUG) && BCM_DPP_FIELD_PRINT
#include <shared/bsl.h>
#define FIELD_PRINT(stuff) bsl_printf stuff
#else
#define FIELD_PRINT(stuff)
#endif


#if (32 != _SHR_PBMP_WORD_WIDTH)
#error PBMP formats are not correct for current PBMP word size
#endif /* (32 != _SHR_PBMP_WORD_WIDTH) */
#if (_SHR_PBMP_WORD_MAX > 1)
#if (_SHR_PBMP_WORD_MAX > 2)
#if (_SHR_PBMP_WORD_MAX > 3)
#if (_SHR_PBMP_WORD_MAX > 4)
#if (_SHR_PBMP_WORD_MAX > 5)
#if (_SHR_PBMP_WORD_MAX > 6)
#if (_SHR_PBMP_WORD_MAX > 7)
#define FIELD_PBMP_FORMAT "%08X %08X %08X %08X %08X %08X %08X %08X"
#define FIELD_PBMP_SHOW(pbmp) \
                             _SHR_PBMP_WORD_GET(pbmp,7), \
                             _SHR_PBMP_WORD_GET(pbmp,6), \
                             _SHR_PBMP_WORD_GET(pbmp,5), \
                             _SHR_PBMP_WORD_GET(pbmp,4), \
                             _SHR_PBMP_WORD_GET(pbmp,3), \
                             _SHR_PBMP_WORD_GET(pbmp,2), \
                             _SHR_PBMP_WORD_GET(pbmp,1), \
                             _SHR_PBMP_WORD_GET(pbmp,0)
#else /* (_SHR_PBMP_WORD_MAX > 7) */
#define FIELD_PBMP_FORMAT "%08X %08X %08X %08X %08X %08X %08X"
#define FIELD_PBMP_SHOW(pbmp) \
                             _SHR_PBMP_WORD_GET(pbmp,6), \
                             _SHR_PBMP_WORD_GET(pbmp,5), \
                             _SHR_PBMP_WORD_GET(pbmp,4), \
                             _SHR_PBMP_WORD_GET(pbmp,3), \
                             _SHR_PBMP_WORD_GET(pbmp,2), \
                             _SHR_PBMP_WORD_GET(pbmp,1), \
                             _SHR_PBMP_WORD_GET(pbmp,0)
#endif /* (_SHR_PBMP_WORD_MAX > 7) */
#else /* (_SHR_PBMP_WORD_MAX > 6) */
#define FIELD_PBMP_FORMAT "%08X %08X %08X %08X %08X %08X"
#define FIELD_PBMP_SHOW(pbmp) \
                             _SHR_PBMP_WORD_GET(pbmp,5), \
                             _SHR_PBMP_WORD_GET(pbmp,4), \
                             _SHR_PBMP_WORD_GET(pbmp,3), \
                             _SHR_PBMP_WORD_GET(pbmp,2), \
                             _SHR_PBMP_WORD_GET(pbmp,1), \
                             _SHR_PBMP_WORD_GET(pbmp,0)
#endif /* (_SHR_PBMP_WORD_MAX > 6) */
#else /* (_SHR_PBMP_WORD_MAX > 5) */
#define FIELD_PBMP_FORMAT "%08X %08X %08X %08X %08X"
#define FIELD_PBMP_SHOW(pbmp) \
                             _SHR_PBMP_WORD_GET(pbmp,4), \
                             _SHR_PBMP_WORD_GET(pbmp,3), \
                             _SHR_PBMP_WORD_GET(pbmp,2), \
                             _SHR_PBMP_WORD_GET(pbmp,1), \
                             _SHR_PBMP_WORD_GET(pbmp,0)
#endif /* (_SHR_PBMP_WORD_MAX > 5) */
#else /* (_SHR_PBMP_WORD_MAX > 4) */
#define FIELD_PBMP_FORMAT "%08X %08X %08X %08X"
#define FIELD_PBMP_SHOW(pbmp) \
                             _SHR_PBMP_WORD_GET(pbmp,3), \
                             _SHR_PBMP_WORD_GET(pbmp,2), \
                             _SHR_PBMP_WORD_GET(pbmp,1), \
                             _SHR_PBMP_WORD_GET(pbmp,0)
#endif /* (_SHR_PBMP_WORD_MAX > 4) */
#else /* (_SHR_PBMP_WORD_MAX > 3) */
#define FIELD_PBMP_FORMAT "%08X %08X %08X"
#define FIELD_PBMP_SHOW(pbmp) \
                             _SHR_PBMP_WORD_GET(pbmp,2), \
                             _SHR_PBMP_WORD_GET(pbmp,1), \
                             _SHR_PBMP_WORD_GET(pbmp,0)
#endif /* (_SHR_PBMP_WORD_MAX > 3) */
#else /* (_SHR_PBMP_WORD_MAX > 2) */
#define FIELD_PBMP_FORMAT "%08X %08X"
#define FIELD_PBMP_SHOW(pbmp) \
                             _SHR_PBMP_WORD_GET(pbmp,1), \
                             _SHR_PBMP_WORD_GET(pbmp,0)
#endif /* (_SHR_PBMP_WORD_MAX > 2) */
#else /* (_SHR_PBMP_WORD_MAX > 1) */
#define FIELD_PBMP_FORMAT "%08X"
#define FIELD_PBMP_SHOW(pbmp) \
                             _SHR_PBMP_WORD_GET(pbmp,0)
#endif /* (_SHR_PBMP_WORD_MAX > 1) */

#define FIELD_MACA_FORMAT "%02X:%02X:%02X:%02X:%02X:%02X"
#define FIELD_MACA_SHOW(maca) \
                             (maca)[0], \
                             (maca)[1], \
                             (maca)[2], \
                             (maca)[3], \
                             (maca)[4], \
                             (maca)[5]


#define FIELD_IPV6A_FORMAT \
    "%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X:%02X%02X"
#define FIELD_IPV6A_SHOW(ipv6a) \
                  (ipv6a)[0], (ipv6a)[1], \
                  (ipv6a)[2], (ipv6a)[3], \
                  (ipv6a)[4], (ipv6a)[5], \
                  (ipv6a)[6], (ipv6a)[7], \
                  (ipv6a)[8], (ipv6a)[9], \
                  (ipv6a)[10], (ipv6a)[11], \
                  (ipv6a)[12], (ipv6a)[13], \
                  (ipv6a)[14], (ipv6a)[15]

/*
 *  Limits...
 *
 *  _BCM_PETRA_FIELD_PREDEF_KEYS = 'predefined' keys count.  Note this is not
 *  the number of predefined keys supported by the device, but the largest
 *  number of them supported at BCM layer.
 *
 *  _BCM_PETRA_FIELD_PRESEL_LIMIT = preselector count
 *
 *  _BCM_PETRA_FIELD_DQ_LIMIT(_u) = data qualifier count
 *
 *
 *  _BCM_DPP_FIELD_MAX_GROUP_TYPES = maximum number of group types (such as L2,
 *  IPv4, IPv6, MPLS)
 *
 *  _BCM_DPP_FIELD_ACTION_CHAIN_MAX = maximum number of PPD actions that can be
 *  used to represent a single BCM layer action.  Specifically, code may need
 *  this changed if there are new actions at BCM layer that require more PPD
 *  layer actions to implement.
 *
 *  _BCM_PETRA_FIELD_QUAL_CHAIN_MAX = maximum number of PPD qualifiers that can
 *  be used to represent a single BCM layer qualifier.  Specifically, code may
 *  need this changed if there are new actions at BCM layer that require more
 *  PPD layer actions to implement.
 */
#define _BCM_PETRA_FIELD_GROUP_LIMIT(_u) SOC_PPC_FP_NOF_DBS
#define _BCM_PETRA_FIELD_PREDEF_KEYS 3
#define _BCM_PETRA_FIELD_PRESEL_LIMIT(_u) 5
#ifdef BCM_ARAD_SUPPORT
/* support Arad and Soc_petraB */
#if (64 > BCM_FIELD_USER_NUM_UDFS)
#error "BCM_FIELD_USER_NUM_UDFS is too low (must be >= 16)"
#endif /* (10 > BCM_FIELD_USER_NUM_UDFS) */
#define _BCM_PETRA_FIELD_DQ_LIMIT(_u) \
    (SOC_PPC_FP_QUAL_HDR_USER_DEF_LAST - SOC_PPC_FP_QUAL_HDR_USER_DEF_0 + 1)
#else
/* support only Soc_petraB */
#if (10 > BCM_FIELD_USER_NUM_UDFS)
#error "BCM_FIELD_USER_NUM_UDFS is too low (must be >= 10)"
#endif /* (10 > BCM_FIELD_USER_NUM_UDFS) */
#define _BCM_PETRA_FIELD_DQ_LIMIT(_u) 0
#endif
#define _BCM_DPP_FIELD_MAX_GROUP_TYPES 4
#define _BCM_DPP_FIELD_ACTION_CHAIN_MAX 2
#define _BCM_DPP_FIELD_QUAL_CHAIN_MAX 2

/*
 *  Error checking -- just to be sure about things
 */
#if (31 < _BCM_DPP_FIELD_MAX_GROUP_TYPES)
#error "_BCM_DPP_FIELD_MAX_GROUP_TYPES must be < 32"
#endif /* (31 < _BCM_DPP_FIELD_MAX_GROUP_TYPES) */

/* maximum number of sub-ranges in a single range */
#define _BCM_DPP_FIELD_RANGE_CHAIN_MAX 2

/*
 *  Send stuff here to drop it
 */
#define _BCM_DPP_FIELD_DROP_DEST(unit) (SOC_DPP_CONFIG((unit))->pp.drop_dest)

/*
 *  Indices.  These are declared here rather than using ints because ints can
 *  be pretty large when used the way we want to use them, particularly if
 *  certain limits are better expressed with more specific size items.
 *
 *  We can never *quite* fill the type because we use the maximum possible
 *  value for the type to indicate NULL (since zero is valid).  Basically this
 *  means that if using a byte index, there can be only 255 (0..254) valid
 *  values with 255 reserved as an internal NULL value.
 */
typedef uint8 _bcm_dpp_field_grp_idx_t;
typedef uint32 _bcm_dpp_field_ent_idx_t;
typedef uint32 _bcm_dpp_field_presel_idx_t;
typedef uint32 _bcm_dpp_field_dq_idx_t;
typedef uint8 _bcm_dpp_field_stage_idx_t;
typedef uint8 _bcm_dpp_field_type_idx_t;
typedef uint8 _bcm_dpp_field_chain_idx_t;
typedef uint8 _bcm_dpp_field_map_idx_t;
typedef uint8 _bcm_dpp_field_range_idx_t;
typedef uint8 _bcm_dpp_field_program_idx_t;
#define _BCM_DPP_FIELD_GROUP_IDX_FORMAT "%8d"
#define _BCM_DPP_FIELD_ENTRY_IDX_FORMAT "%8d"
#define _BCM_DPP_FIELD_STAGE_IDX_FORMAT "%8d"

/*
 *  L4 port range definitions
 */

#define _BCM_DPP_NOF_L4_PORT_RANGES  (24)

/*
 *  Packet length range definitions
 */

#define _BCM_DPP_NOF_PKT_LEN_RANGES (3)

/*
 * Number of local-ports per Device
 */
#define _BCM_DPP_NOF_LOCAL_PORTS(unit) (SOC_DPP_DEFS_GET(unit,nof_local_ports))

#define _BCM_DPP_NOF_PORTS_PER_CORE(unit) (SOC_TMC_NOF_FAP_PORTS_PER_CORE)

/*
 *  Data field definitions
 */

#ifdef BCM_ARAD_SUPPORT
#define _BCM_DPP_NOF_FIELD_DATAS  256 /* hard-coded, since doesn't compile in kernel mode
                                        (SOC_PPC_FP_QUAL_HDR_USER_DEF_LAST - SOC_PPC_FP_QUAL_HDR_USER_DEF_0 + 1) */
#else
#define _BCM_DPP_NOF_FIELD_DATAS  (10)
#endif /* def BCM_ARAD_SUPPORT */
typedef SHR_BITDCL _bcm_dpp_field_datafield_bits[_SHR_BITDCLSIZE(_BCM_DPP_NOF_FIELD_DATAS)];

#ifdef BCM_ARAD_SUPPORT
/* support Arad and Soc_petraB */
#if (_BCM_DPP_NOF_FIELD_DATAS > BCM_FIELD_USER_NUM_UDFS)
#error "BCM_FIELD_USER_NUM_UDFS is too low (must be >= 16)"
#endif /* (10 > BCM_FIELD_USER_NUM_UDFS) */
#define _BCM_PETRA_FIELD_DQ_LIMIT(_u) \
    (SOC_PPC_FP_QUAL_HDR_USER_DEF_LAST - SOC_PPC_FP_QUAL_HDR_USER_DEF_0 + 1)
#else
/* support only Soc_petraB */
#if (10 > BCM_FIELD_USER_NUM_UDFS)
#error "BCM_FIELD_USER_NUM_UDFS is too low (must be >= 10)"
#endif /* (10 > BCM_FIELD_USER_NUM_UDFS) */
#define _BCM_PETRA_FIELD_DQ_LIMIT(_u) (0)
#endif

/*
 *  Preselector definitions
 */
#ifdef BCM_ARAD_SUPPORT
#define _BCM_DPP_NOF_FIELD_PRESELECTORS SOC_PPC_FP_NOF_PFGS_ARAD
#else /* def BCM_ARAD_SUPPORT */
#define _BCM_DPP_NOF_FIELD_PRESELECTORS     (5)
#endif /* def BCM_ARAD_SUPPORT */

#define _BCM_DPP_PRESEL_NOF_PORT_PROFILES   (8) /* 8 profiles for both InPort, OutPort and AppType */

#define _BCM_DPP_PRESEL_ID_INVALID          (-1)

/*
 *  Mapping BCM layer qualifier and action sets to PPD layer.
 */
typedef SHR_BITDCL _bcm_dpp_field_qual_set_t[_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES)];
typedef SHR_BITDCL _bcm_dpp_field_action_set_t[_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_ACTION_TYPES)];
/*typedef struct _bcm_dpp_field_qual_set_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES)]; 
} _bcm_dpp_field_qual_set_t;

typedef struct _bcm_dpp_field_action_set_s {
    SHR_BITDCL w[_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_ACTION_TYPES)]; 
} _bcm_dpp_field_action_set_t;*/


#define _BCM_DPP_FIELD_DEV_INFO(unit)       _bcm_arad_field_device_info

#define _BCM_DPP_FIELD_STAGE_DEV_INFO(unit, _stage) \
       ((_BCM_DPP_FIELD_DEV_INFO(unit)).stage[_stage])

/*
 *  This describes the entry type that will be used for each mode.
 *
 *  Note that when automatically choosing the group mode (for
 *  bcmFieldGroupModeAuto), only those modes using TCAM type entries will be
 *  considered.
 *
 *  TCAM almost certainly must come first, and be equal to zero.
 */
typedef enum _bcm_dpp_field_entry_type_e {
    _bcmDppFieldEntryTypeInternalTcam = 0,          /* Standard internal TCAM based entry */
    _bcmDppFieldEntryTypeDirLookup = _bcmDppFieldEntryTypeInternalTcam, /* Direct Lookup entry */
    _bcmDppFieldEntryTypeDirExt,            /* Direct Extraction entry */
    _bcmDppFieldEntryTypeExternalTcam,          /* External TCAM based entry */
    _bcmDppFieldEntryTypeCount              /* not a valid type; must be last */
} _bcm_dpp_field_entry_type_t;

#define _BCM_DPP_FIELD_ENTRY_TYPE_STRINGS \
    { \
        "TCAM", \
        "DirExt" \
    }

#define _BCM_DPP_FIELD_ENT_BIAS(unit, _x) \
     (_BCM_DPP_FIELD_ENTRY_TYPE_BIAS(unit) * (_bcmDppFieldEntryType ## _x))

/* Internal / External TCAM macros */
#define _BCM_DPP_FIELD_ENT_TYPE_IS_INTTCAM(_entryType) \
     ((_entryType == _bcmDppFieldEntryTypeInternalTcam)? TRUE: FALSE)
#define _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(_entryType) \
     ((_entryType == _bcmDppFieldEntryTypeExternalTcam)? TRUE: FALSE)
#define _BCM_DPP_FIELD_ENT_TYPE_IS_TCAM(_entryType) \
     ((_BCM_DPP_FIELD_ENT_TYPE_IS_INTTCAM(_entryType) || _BCM_DPP_FIELD_ENT_TYPE_IS_EXTTCAM(_entryType))? TRUE: FALSE)

/* Set/Get External TCAM qualifiers in sw_state. Some devices like Arad, Arad+ and QUX do not have enough memory for storing the entries in the sw_state. */
#define _BCM_DPP_EXT_TCAM_ENTRIES_IN_SW_STATE 0

int _BCM_DPP_FIELD_ENT_IS_INTTCAM(int unit, int _entry);
int _BCM_DPP_FIELD_ENT_IS_EXTTCAM(int unit, int _entry);
int _BCM_DPP_FIELD_ENT_IS_DIR_EXT(int unit, int _entry);

#define _BCM_DPP_FIELD_ENT_IS_TCAM(unit, _entry) \
     ((_BCM_DPP_FIELD_ENT_IS_INTTCAM(unit, _entry) || _BCM_DPP_FIELD_ENT_IS_EXTTCAM(unit, _entry))? TRUE: FALSE)

#define _BCM_DPP_FIELD_INTTCAM_LOCAL_ID(unit, _entry) (_entry - _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam))
#define _BCM_DPP_FIELD_EXTTCAM_LOCAL_ID(unit, _entry) (_entry - _BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam))
#define _BCM_DPP_FIELD_TCAM_LOCAL_ID(unit, _entry) (_BCM_DPP_FIELD_ENT_IS_INTTCAM(unit, _entry)? \
    _BCM_DPP_FIELD_INTTCAM_LOCAL_ID(unit, _entry): _BCM_DPP_FIELD_EXTTCAM_LOCAL_ID(unit, _entry))

#define _BCM_DPP_FIELD_INTTCAM_GLOBAL_ID(unit, _entry) (_entry + _BCM_DPP_FIELD_ENT_BIAS(unit, InternalTcam))
#define _BCM_DPP_FIELD_EXTTCAM_GLOBAL_ID(unit, _entry) (_entry + _BCM_DPP_FIELD_ENT_BIAS(unit, ExternalTcam))
#define _BCM_DPP_FIELD_TCAM_GLOBAL_ID(unit, _entry, isExternalTcam) ((!isExternalTcam)? \
    _BCM_DPP_FIELD_INTTCAM_GLOBAL_ID(unit, _entry): _BCM_DPP_FIELD_EXTTCAM_GLOBAL_ID(unit, _entry))

#define _BCM_DPP_FIELD_TCAM_ENTRY_LIMIT(_entryType) \
    ((_entryType == _bcmDppFieldEntryTypeInternalTcam)? FIELD_ACCESS.entryTcLimit.get(unit, &tcam_entry_limit) : FIELD_ACCESS.entryExtTcLimit.get(unit, &tcam_entry_limit))
#define _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE(_entryType) \
    ((_entryType == _bcmDppFieldEntryTypeInternalTcam)? FIELD_ACCESS.entryIntTcFree.get(unit, &next) : FIELD_ACCESS.entryExtTcFree.get(unit, &next));

#define _BCM_DPP_FIELD_TCAM_ENTRY_TC_COUNT_OPERATION(_entryType, _operation){ \
    if  (_entryType == _bcmDppFieldEntryTypeInternalTcam){\
          {FIELD_ACCESS.entryIntTcCount.get(unit, &entryIntTcCount);} \
          {entryIntTcCount _operation} \
          {FIELD_ACCESS.entryIntTcCount.set(unit, entryIntTcCount);}}\
    else {{FIELD_ACCESS.entryExtTcCount.get(unit, &entryExtTcCount);} \
          {entryExtTcCount _operation} \
          {FIELD_ACCESS.entryExtTcCount.set(unit, entryExtTcCount);}}}




#define _BCM_DPP_FIELD_TCAM_ENTRY_TC_FREE_OPERATION(_entryType, _operation){ \
        if ((_entryType == _bcmDppFieldEntryTypeInternalTcam)){\
              {FIELD_ACCESS.entryIntTcFree.get(unit, &entryIntTcFree);} \
              {entryIntTcFree _operation} \
              {FIELD_ACCESS.entryIntTcFree.set(unit, entryIntTcFree);}}\
        else {{FIELD_ACCESS.entryExtTcFree.get(unit, &entryExtTcFree);} \
              {entryExtTcFree _operation} \
              {FIELD_ACCESS.entryExtTcFree.set(unit, entryExtTcFree);}}}
/*
 * Depending on '_qual_ndx', get the element
 *   entryQual[qual_ndx].qualData
 * using
 *   FIELD_ACCESS_ENTRYTC.entryCmn.entryQual.qualData.get
 *   FIELD_ACCESS_EXTTCAMINFO.entryQual.qualData.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_QUALS
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_QUALDATA_GET(unit,isExternalTcam, _entry, _qual_ndx, qualData) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.entryQual.qualData.get(unit,_entry,_qual_ndx,&qualData)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.entryQual.qualData.get(unit,ext_info_idx,_qual_ndx,&qualData)) ; \
        } \
    }
/*
 * Depending on '_qual_ndx', get the element
 *   entryQual[qual_ndx]
 * using
 *   FIELD_ACCESS_ENTRYTC.entryCmn.entryQual.get
 *   FIELD_ACCESS_EXTTCAMINFO.entryQual.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_QUALS
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_GET(unit,isExternalTcam, _entry, _qual_ndx, qual) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.entryQual.get(unit,_entry,_qual_ndx,&qual)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.entryQual.get(unit,ext_info_idx,_qual_ndx,&qual)) ; \
        } \
    }
/*
 * Depending on '_qual_ndx', set the element
 *   entryQual[qual_ndx]
 * using
 *   FIELD_ACCESS_ENTRYTC.entryCmn.entryQual.set
 *   FIELD_ACCESS_EXTTCAMINFO.entryQual.set
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_QUALS
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_QUALS_SET(unit,isExternalTcam, _entry, _qual_ndx, qual) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.entryQual.set(unit,_entry,_qual_ndx,&qual)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.entryQual.set(unit,ext_info_idx,_qual_ndx,&qual)) ; \
        } \
    }



#define _BCM_DPP_FIELD_TCAM_ENTRY_QUALS(isExternalTcam, _entry) \
    ((!isExternalTcam)? \
    unitData->entryTc[_entry].entryCmn.entryQual: \
    unitData->extTcamInfo[unitData->entryExtTc[_entry].ext_info_idx].entryQual)


/*
 * Depending on '_index' and '_entry', get the element
 *   tcActP[_index].hwType
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActP.hwType.get
 *   FIELD_ACCESS_EXTTCAMINFO.tcActP.hwType.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWTYPE_GET(unit,isExternalTcam, _entry, _index, _hwType) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActP.hwType.get(unit,_entry,_index,&_hwType)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActP.hwType.get(unit,ext_info_idx,_index,&_hwType)) ; \
        } \
    }
/*
 * Depending on '_index' and '_entry', set the element
 *   tcActP[_index].hwType
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActP.hwType.set
 *   FIELD_ACCESS_EXTTCAMINFO.tcActP.hwType.set
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWTYPE_SET(unit,isExternalTcam, _entry, _index, _hwType) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActP.hwType.set(unit,_entry,_index,_hwType)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActP.hwType.set(unit,ext_info_idx,_index,_hwType)) ; \
        } \
    }

/*
 * Depending on '_index' and '_entry', get the element
 *   tcActP[_index].hwParam
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActP.hwParam.get
 *   FIELD_ACCESS_EXTTCAMINFO.tcActP.hwParam.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWPARAM_GET(unit,isExternalTcam, _entry, _index, _hwParam) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActP.hwParam.get(unit,_entry,_index,&_hwParam)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActP.hwParam.get(unit,ext_info_idx,_index,&_hwParam)) ; \
        } \
    }
/*
 * Depending on '_index' and '_entry', set the element
 *   tcActP[_index].hwParam
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActP.hwParam.set
 *   FIELD_ACCESS_EXTTCAMINFO.tcActP.hwParam.set
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWPARAM_SET(unit,isExternalTcam, _entry, _index, _hwParam) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActP.hwParam.set(unit,_entry,_index,_hwParam)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActP.hwParam.set(unit,ext_info_idx,_index,_hwParam)) ; \
        } \
    }
/*
 * Depending on '_index' and '_entry', get the element
 *   tcActP[_index].hwFlags
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActP.hwFlags.get
 *   FIELD_ACCESS_EXTTCAMINFO.tcActP.hwFlags.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWFLAGS_GET(unit,isExternalTcam, _entry, _index, _hwFlags) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActP.hwFlags.get(unit,_entry,_index,&_hwFlags)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActP.hwFlags.get(unit,ext_info_idx,_index,&_hwFlags)) ; \
        } \
    }
/*
 * Depending on '_index' and '_entry', set the element
 *   tcActP[_index].hwFlags
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActP.hwFlags.set
 *   FIELD_ACCESS_EXTTCAMINFO.tcActP.hwFlags.set
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_HWFLAGS_SET(unit,isExternalTcam, _entry, _index, _hwFlags) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActP.hwFlags.set(unit,_entry,_index,_hwFlags)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActP.hwFlags.set(unit,ext_info_idx,_index,_hwFlags)) ; \
        } \
    }

/*
 * Depending on '_index' and '_entry', get the element
 *   tcActP[_index]
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActP.get
 *   FIELD_ACCESS_EXTTCAMINFO.tcActP.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_GET(unit,isExternalTcam, _entry, _index, _tcActP) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActP.get(unit,_entry,_index,&_tcActP)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActP.get(unit,ext_info_idx,_index,&_tcActP)) ; \
        } \
    }
/*
 * Depending on '_index' and '_entry', set the element
 *   tcActP[_index]
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActP.set
 *   FIELD_ACCESS_EXTTCAMINFO.tcActP.set
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTP
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTP_SET(unit,isExternalTcam, _entry, _index, _tcActP) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActP.set(unit,_entry,_index,&_tcActP)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActP.set(unit,ext_info_idx,_index,&_tcActP)) ; \
        } \
    }

#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTP(isExternalTcam, _entry) \
    ((!isExternalTcam)? \
    unitData->entryTc[_entry].tcActP: \
    unitData->extTcamInfo[unitData->entryExtTc[_entry].ext_info_idx].tcActP)

/*
 * Depending on '_index' and '_entry', get the element
 *   tcActP[_index]
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActB.get
 *   FIELD_ACCESS_EXTTCAMINFO.tcActB.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTB
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_GET(unit,isExternalTcam, _entry, _index, _tcActB) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActB.get(unit,_entry,_index,&_tcActB)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActB.get(unit,ext_info_idx,_index,&_tcActB)) ; \
        } \
    }
/*
 * Depending on '_index' and '_entry', set the element
 *   tcActB[_index]
 * using
 *   FIELD_ACCESS_ENTRYTC.tcActB.set
 *   FIELD_ACCESS_EXTTCAMINFO.tcActB.set
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_ACTB
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTB_SET(unit,isExternalTcam, _entry, _index, _tcActB) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.tcActB.set(unit,_entry,_index,&_tcActB)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.tcActB.set(unit,ext_info_idx,_index,&_tcActB)) ; \
        } \
    }


#define _BCM_DPP_FIELD_TCAM_ENTRY_ACTB(isExternalTcam, _entry) \
    ((!isExternalTcam)? \
    unitData->entryTc[_entry].tcActB: \
    unitData->extTcamInfo[unitData->entryExtTc[_entry].ext_info_idx].tcActB)

#define _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_PTR(isExternalTcam, _entry) \
    ((!isExternalTcam) ? \
    (&(unitData->entryTc[_entry].entryCmn.entryPriority)): \
    (&(unitData->extTcamInfo[unitData->entryExtTc[_entry].ext_info_idx].entryPriority)))

/*
 * Depending on '_entry', get the element
 *   entryCmn.entryPriority
 * using
 *   FIELD_ACCESS_ENTRYTC.entryCmn.entryPriority.get
 *   FIELD_ACCESS_EXTTCAMINFO.entryCmn.entryPriority.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_GET(unit,isExternalTcam, _entry, _entryPriority) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.entryPriority.get(unit,_entry,&_entryPriority)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.entryPriority.get(unit,ext_info_idx,&_entryPriority)) ; \
        } \
    }
/*
 * Depending on '_entry', set the element
 *   entryCmn.entryPriority
 * using
 *   FIELD_ACCESS_ENTRYTC.entryCmn.entryPriority.get
 *   FIELD_ACCESS_EXTTCAMINFO.entryCmn.entryPriority.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY_SET(unit,isExternalTcam, _entry, _entryPriority) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.entryPriority.set(unit,_entry,_entryPriority)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.entryPriority.set(unit,ext_info_idx,_entryPriority)) ; \
        } \
    }


#define _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY(isExternalTcam, _entry) \
    ((!isExternalTcam) ? \
    (unitData->entryTc[_entry].entryCmn.entryPriority): \
    (unitData->extTcamInfo[unitData->entryExtTc[_entry].ext_info_idx].entryPriority))


#define _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_PTR(isExternalTcam, _entry) \
    ((!isExternalTcam) ? \
    (&(unitData->entryTc[_entry].entryCmn.hwPriority)): \
    (&(unitData->extTcamInfo[unitData->entryExtTc[_entry].ext_info_idx].hwPriority)))

/*
 * Depending on '_entry', get the element
 *   entryCmn.hwPriority
 * using
 *   FIELD_ACCESS_ENTRYTC.entryCmn.hwPriority.get
 *   FIELD_ACCESS_EXTTCAMINFO.entryCmn.hwPriority.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_GET(unit,isExternalTcam, _entry, _hwPriority) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.hwPriority.get(unit,_entry,&_hwPriority)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.hwPriority.get(unit,ext_info_idx,&_hwPriority)) ; \
        } \
    }
/*
 * Depending on '_entry', set the element
 *   entryCmn.hwPriority
 * using
 *   FIELD_ACCESS_ENTRYTC.entryCmn.hwPriority.get
 *   FIELD_ACCESS_EXTTCAMINFO.entryCmn.hwPriority.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY_SET(unit,isExternalTcam, _entry, _hwPriority) \
    { \
        if (!isExternalTcam) \
        { \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYTC.entryCmn.hwPriority.set(unit,_entry,_hwPriority)) ; \
        } \
        else \
        { \
            uint32 ext_info_idx ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_ENTRYEXTTC.ext_info_idx.get(unit,_entry,&ext_info_idx)) ; \
            BCMDNX_IF_ERR_EXIT(FIELD_ACCESS_EXTTCAMINFO.hwPriority.set(unit,ext_info_idx,_hwPriority)) ; \
        } \
    }

#define _BCM_DPP_FIELD_TCAM_ENTRY_HW_PRIORITY(isExternalTcam, _entry) \
    ((!isExternalTcam) ? \
    (unitData->entryTc[_entry].entryCmn.hwPriority): \
    (unitData->extTcamInfo[unitData->entryExtTc[_entry].ext_info_idx].hwPriority))

#define _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_PTR(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (&(unitData->entryTc[_entry].entryCmn.hwHandle)) : (&(unitData->entryExtTc[_entry].entryCmn.hwHandle)))



/*
 * Depending on '_entry', get the element
 *   entryCmn.hwHandle
 * using
 *   FIELD_ACCESS_ENTRYTC.entryCmn.hwHandle.get
 *   FIELD_ACCESS_EXTTCAMINFO.entryCmn.hwHandle.get
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_GET(unit,isExternalTcam, _entry, _hwHandle) \
    ((!isExternalTcam) ? \
        FIELD_ACCESS_ENTRYTC.entryCmn.hwHandle.get(unit,_entry,&_hwHandle) : \
        FIELD_ACCESS_ENTRYEXTTC.entryCmn.hwHandle.get(unit,_entry,&_hwHandle)) ;

/*
 * Depending on '_entry', set the element
 *   entryCmn.hwHandle
 * using
 *   FIELD_ACCESS_ENTRYTC.entryCmn.hwHandle.set
 *   FIELD_ACCESS_EXTTCAMINFO.entryCmn.hwHandle.set
 * See:
 *   _BCM_DPP_FIELD_TCAM_ENTRY_PRIORITY
 */
#define _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE_SET(unit,isExternalTcam, _entry, _hwHandle) \
    ((!isExternalTcam) ? \
        FIELD_ACCESS_ENTRYTC.entryCmn.hwHandle.set(unit,_entry,_hwHandle) : \
        FIELD_ACCESS_ENTRYEXTTC.entryCmn.hwHandle.set(unit,_entry,_hwHandle)) 


#define _BCM_DPP_FIELD_TCAM_ENTRY_HW_HANDLE(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (unitData->entryTc[_entry].entryCmn.hwHandle) : (unitData->entryExtTc[_entry].entryCmn.hwHandle))

#define _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_PTR(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (&(unitData->entryTc[_entry].entryCmn.entryNext)) : (&(unitData->entryExtTc[_entry].entryCmn.entryNext)))


/*
 * get/set 'entryNext' element from Tc/extTc depending on 'isExternalTcam'
 * See _BCM_DPP_FIELD_TCAM_ENTRY_NEXT
 */ 
#define _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_GET(unit,isExternalTcam, _entry, _entryNext) \
    ((!isExternalTcam) ? \
      FIELD_ACCESS_ENTRYTC.entryCmn.entryNext.get(unit,_entry,&_entryNext) : \
      FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryNext.get(unit,_entry,&_entryNext))

#define _BCM_DPP_FIELD_TCAM_ENTRY_NEXT_SET(unit,isExternalTcam, _entry, _entryNext) \
    ((!isExternalTcam) ? \
      FIELD_ACCESS_ENTRYTC.entryCmn.entryNext.set(unit,_entry,_entryNext) : \
      FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryNext.set(unit,_entry,_entryNext))



#define _BCM_DPP_FIELD_TCAM_ENTRY_NEXT(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (unitData->entryTc[_entry].entryCmn.entryNext) : (unitData->entryExtTc[_entry].entryCmn.entryNext))

#define _BCM_DPP_FIELD_TCAM_ENTRY_PREV_PTR(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (&(unitData->entryTc[_entry].entryCmn.entryPrev)) : (&(unitData->entryExtTc[_entry].entryCmn.entryPrev)))


/*
 * get/set 'entryPrev' element from Tc/extTc depending on 'isExternalTcam'
 * See _BCM_DPP_FIELD_TCAM_ENTRY_PREV
 */ 
#define _BCM_DPP_FIELD_TCAM_ENTRY_PREV_GET(unit,isExternalTcam, _entry, _entryPrev) \
    ((!isExternalTcam) ? \
      FIELD_ACCESS_ENTRYTC.entryCmn.entryPrev.get(unit,_entry,&_entryPrev) : \
      FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryPrev.get(unit,_entry,&_entryPrev))

#define _BCM_DPP_FIELD_TCAM_ENTRY_PREV_SET(unit,isExternalTcam, _entry, _entryPrev) \
    ((!isExternalTcam) ? \
      FIELD_ACCESS_ENTRYTC.entryCmn.entryPrev.set(unit,_entry,_entryPrev) : \
      FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryPrev.set(unit,_entry,_entryPrev))


#define _BCM_DPP_FIELD_TCAM_ENTRY_PREV(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (unitData->entryTc[_entry].entryCmn.entryPrev) : (unitData->entryExtTc[_entry].entryCmn.entryPrev))
/*
 * get/set 'flags' element from Tc/extTc depending on 'isExternalTcam'
 * See _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS
 */ 
#define _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_GET(unit,isExternalTcam, _entry, _flags) \
    ((!isExternalTcam) ? \
      FIELD_ACCESS_ENTRYTC.entryCmn.entryFlags.get(unit,_entry,&_flags) : \
      FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryFlags.get(unit,_entry,&_flags))

#define _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_SET(unit,isExternalTcam, _entry, _flags) \
    ((!isExternalTcam) ? \
      FIELD_ACCESS_ENTRYTC.entryCmn.entryFlags.set(unit,_entry,_flags) : \
      FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryFlags.set(unit,_entry,_flags))


#define _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS_PTR(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (&(unitData->entryTc[_entry].entryCmn.entryFlags)) : (&(unitData->entryExtTc[_entry].entryCmn.entryFlags)))

#define _BCM_DPP_FIELD_TCAM_ENTRY_FLAGS(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (unitData->entryTc[_entry].entryCmn.entryFlags) : (unitData->entryExtTc[_entry].entryCmn.entryFlags))

#define _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_PTR(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (&(unitData->entryTc[_entry].entryCmn.entryGroup)) : (&(unitData->entryExtTc[_entry].entryCmn.entryGroup)))

/*
 * get/set 'entryGroup' element from Tc/extTc depending on 'isExternalTcam'
 * See _BCM_DPP_FIELD_TCAM_ENTRY_GROUP
 */ 
#define _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_GET(unit,isExternalTcam, _entry, _entryGroup) \
    ((!isExternalTcam) ? \
      (FIELD_ACCESS_ENTRYTC.entryCmn.entryGroup.get(unit,_entry,&_entryGroup)) : \
      (FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryGroup.get(unit,_entry,&_entryGroup)))

#define _BCM_DPP_FIELD_TCAM_ENTRY_GROUP_SET(unit,isExternalTcam, _entry, _entryGroup) \
    ((!isExternalTcam) ? \
      (FIELD_ACCESS_ENTRYTC.entryCmn.entryGroup.set(unit,_entry,_entryGroup)) : \
      (FIELD_ACCESS_ENTRYEXTTC.entryCmn.entryGroup.set(unit,_entry,_entryGroup)))

#define _BCM_DPP_FIELD_TCAM_ENTRY_GROUP(isExternalTcam, _entry) \
    ((!isExternalTcam) ? (unitData->entryTc[_entry].entryCmn.entryGroup) : (unitData->entryExtTc[_entry].entryCmn.entryGroup))

#define _BCM_DPP_FIELD_TCAM_ENTRY(isExternalTcam, _entry) \
    ((!isExternalTcam) ? ((void*)&(unitData->entryTc[_entry])) : ((void*)&(unitData->entryExtTc[_entry])))

#define _BCM_DPP_FIELD_TCAM_ENTRY_SIZE(isExternalTcam) \
    ((!isExternalTcam) ? (sizeof(_bcm_dpp_field_entry_t)) : (sizeof(_bcm_dpp_field_entry_ext_t)))

/*
 *  This describes the profile type when handling bitmap HW profiles:
 *  - in/out-port profiles, - FLP program processing profile
 */
typedef enum _bcm_dpp_field_profile_type_e {
    _bcmDppFieldProfileTypeInPort = 0,        /* In-Port profile  - mapped to InPort qualifier */
    _bcmDppFieldProfileTypeOutPort,           /* Out-Port profile  - mapped to OutPort qualifier */
    _bcmDppFieldProfileTypeFlpProgramProfile, /* FLP program processing profile - mapped to AppType qualifier */
    _bcmDppFieldProfileTypeInterfaceInPort,
    _bcmDppFieldProfileTypeCount              /* not a valid type; must be last */
} _bcm_dpp_field_profile_type_t;
#ifdef BROADCOM_DEBUG
#endif /* def BROADCOM_DEBUG */
/*
 * Definitions related to the following APIs:
 *   bcm_petra_field_action_width_get
 *   bcm_petra_field_action_width_set
 */
/* { */
/*
 * Number of elements (second index) attached to each action on the tables:
 *
 *   *_pmf_fem_action_type_encoding_runtime[][<second index>]
 *   *_pmf_fem_action_type_encoding[][<second index>]
 * See, for example: Jericho_pmf_fem_action_type_encoding
 */
#define NUM_ACTION_ELEMENTS                  3
/*
 * Index, on 'second index', at which the 'width' of each action is stored.
 * 'width' is the number of bits assigned to 'action'.
 * See NUM_ACTION_ELEMENTS above.
 */
#define INDEX_OF_ACTION_WIDTH                2
/*
 * Index, on 'second index', at which the 'action id' of each action is stored.
 * Type of this element is, effectively, SOC_PPC_FP_ACTION_TYPE.
 * See NUM_ACTION_ELEMENTS above.
 */
#define INDEX_OF_ACTION                      0
/*
 * Index, on 'second index', at which the 'HW id' of each action is stored.
 * This is the hardware identifier of the action.
 * See NUM_ACTION_ELEMENTS above.
 */
#define INDEX_OF_HW_ID                       1
/* } */
/*
 *  There are now multiple flags per qualifier instead of just whether that
 *  qualifier is used for reverse lookup (translation from PPD qualifier into
 *  BCM qualifier).  These are used in the flags field of the qualifier table.
 *  These flags are ORed together if more than one is applicable, but note that
 *  some of them can't be combined (they are noted so in their description).
 *
 *  Because TRUE/FALSE was used before in this field, and most people (probably
 *  most compilers as well) set basic boolean TRUE as 1, we do not use bit 0
 *  for this set of flags.  This allows the startup code to be sure new
 *  qualifiers use this as a flags field instead of as a simple boolean.
 *
 *  NOTE: this is a 32b signed number, and we don't want bit 0 set to avoid
 *  being equal to TRUE.  Basically, this means it should be limited to 30
 *  useful bits, to avoid compiler quirks around bit 31, and to avoid ever
 *  being equal to either of the common values of TRUE (1 and ~0).
 *
 *  _BCM_DPP_QUAL_REVERSE_LOOKUP indicates the qualifier will be used for
 *  reverse lookup (given a PPD layer qualifier, we will use the last BCM
 *  qualifier in the qualifiers table as the mapping).
 *
 *  _BCM_DPP_QUAL_RANGE_BITMAP indicates the qualifier is a range qualifier
 *  implemented as a bitmap, and so more than one of the same range comparison
 *  type can be active on a single entry and the INVERT feature is offered.  It
 *  can not be combined with _BCM_DPP_QUAL_RANGE_INDEX.
 *
 *  _BCM_DPP_QUAL_RANGE_INDEX indicates the qualifier is a range qualifier
 *  implemented as a selector, so only one range per comparison type can be
 *  active on a single entry, and the INVERT feature is not offered.  It can
 *  not be combined with _BCM_DPP_QUAL_RANGE_BITMAP.
 *
 *  _BCM_DPP_QUAL_FLAGS_RNG_VALID_BITS is merely the set of bits that indicate
 *  some format or other of range.
 *
 *  _BCM_DPP_QUAL_FLAGS_ALL_VALID_BITS is merely the set of valid bits.  It
 *  must exactly cover the existing flags.
 */
#define _BCM_DPP_QUAL_REVERSE_LOOKUP        0x00000002
#define _BCM_DPP_QUAL_RANGE_BITMAP          0x00000004
#define _BCM_DPP_QUAL_RANGE_INDEX           0x00000008
#define _BCM_DPP_QUAL_FLAGS_RNG_VALID_BITS  0x0000000C
#define _BCM_DPP_QUAL_FLAGS_ALL_VALID_BITS  0x0000000E

/*
 *  The range support is designed with the assumption that ranges are only
 *  applicable to a single stage.
 *
 *  Certain details about a range qualifier are here, but most of these have to
 *  do with how that range qualifier will be exposed.  The format (bitmap or
 *  index) for a given range qualifier is specified in the qualifier table.
 *
 *  The last entry in the table of these must be bcmFieldQualifyCount.
 */
#define _BCM_DPP_RANGE_MAX_COUNT 64
typedef struct _bcm_dpp_field_device_range_info_s {
    bcm_field_qualify_t qualifier;          /* qualifier for this range type */
    uint32 rangeFlags;                      /* flags in range definition */
    bcm_field_range_t rangeBase;            /* base ID */
    _bcm_dpp_field_range_idx_t count;       /* number of this range type */
} _bcm_dpp_field_device_range_info_t;

/*
 *  This describes the number of bits/trits that can be included in a group
 *  mode in a particular stage.  The hardware description employs an array of
 *  these, with the last one (as an end indicator) specifying stage 0, bits 0,
 *  and mode bcmFieldGroupModeCount.
 *
 *  Do not include elements in the array for modes that are not supported on
 *  a given stage, and do not include elements in the array for stages that are
 *  not supported, even if you set the number of bits to zero.
 *
 *  It is obligatory to include at least one mode per stage, plus
 *  bcmFieldGroupModeAuto per stage (with the largest number of bits supported
 *  in any mode for that particular stage).
 *
 *  stage is the stage index.
 *
 *  mode is the bcm_field_group_mode being described for the stage.
 *
 *  length is the number of bits that stage supports in the specified mode.
 *
 *  entryCount is the number of 'single' hardware entries taken by an entry in
 *  a group of this width.
 *
 *  An array of these is built to define the supported group modes and their
 *  respective trit limits
 */
typedef struct _bcm_dpp_field_device_group_mode_bits_s {
    _bcm_dpp_field_stage_idx_t stage;           /* stage index */
    bcm_field_group_mode_t mode;                /* group mode */
    unsigned int length;                        /* number of bits */
    unsigned int qualLength;                    /* qualifiable num of bits */
    unsigned int entryCount;                    /* number of entries */
    _bcm_dpp_field_entry_type_t entryType;      /* type of entry */
    SOC_PPC_FP_DATABASE_TYPE hwType;            /* hardware database type */
} _bcm_dpp_field_device_group_mode_bits_t;

/*
 *  This describes limits and sharing properties for a stage.  They are defined
 *  in an array.  Exactly one must be included for each stage as configured in
 *  the hardware description.
 *
 *  name is the name for the stage.
 *
 *  hwStageId is the hardware stage ID for this stage.
 *
 *  flags is the flags that apply to this stage (bitmap).
 *
 *    _BCM_DPP_FIELD_DEV_STAGE_FLAG_IMPLIED_ACTION indicates the stage action
 *    set is small enough that it entirely fits in the resolution buffers, or
 *    that the stage has a fixed action set.  If set, a group created in the
 *    stage will assume a default action set.
 *
 *    _BCM_DPP_FIELD_DEV_STAGE_FLAG_ALLOW_CASCADE indicates the stage allows
 *    cascaded groups.  If set, the cascaded group feature can be configured by
 *    the normal procedure; if clear, cascaded groups are not supported by any
 *    explicit mechanism (groups created by any implied mechanism will be
 *    considered parallel; explicit creation will result in an error).
 *
 *    _BCM_DPP_FIELD_DEV_STAGE_FLAG_ALLOW_TYPE_MERGE indicates the stage allows
 *    a group to be of more than one 'type' (without it, a group can only
 *    include qualifiers from one of L2, IPv4, IPv6, MPLS, but with it, a group
 *    can include qualifiers from combinations of these types, with the
 *    exception that it can not include both IPv4 and IPv6).
 *
 *    _BCM_DPP_FIELD_DEV_STAGE_USES_PRESELECTORS indicates the stage uses
 *    preselectors (if not set, the stage neither uses nor supports them).
 *
 *  maxGroups is the maximum number of groups in the stage.
 *
 *  maxEntriesInternalTcam is the maximum number of internal TCAM entries in the stage.
 *
 *  maxEntriesExternalTcam is the maximum number of external TCAM entries in the stage.
 *
 *  maxEntriesDe is the maximum number of direct extraction entries.
 *
 *  maxProgQuals is the maximum number of programmable qualifiers.
 *
 *  entryMaxQuals is the maximum number of qualifiers for a TCAM entry and the
 *  maximum number of qualifiers in a group's qualifier set.
 *
 *  entryMaxActs is the maximum number of actions for an entry and the maximum
 *  number of actions in a group's action set.
 *
 *  entryDeMaxQuals is the maximum number of qualifiers on a direct extraction
 *  entry (that will be used to qualify the entry; it can still use all of the
 *  qualifier(s) from the group's qualifier set for extractions).
 *
 *  impliedActionSet is TRUE if the stage allows action set to be implied (will
 *  be assumed in this case to be all actions permitted to a group according to
 *  the stage and type).  If impliedActionSet is TRUE, a group will not require
 *  an action set to be configured, but if one is configured, it will be
 *  honored.  If impliedActionSet is FALSE, a group must have an action set
 *  explicitly configured before it can be used.
 *
 *  If this stage does not share hardware group ID space with a previously
 *  defined stage, sharesGroups is -1.  If this stage shares hardware group Id
 *  space with a previously defined stage, sharedGroups is the index of the
 *  most recently defined such stage.
 *
 *  If this stage does not share hardware TCAM entry ID space with a previously
 *  defined stage, sharesEntriesTc is -1.  If this stage shares hardware TCAM
 *  entry ID space with a previously defined stage, sharedEntriesTc is the
 *  index of the most recently defined such stage.
 *
 *  If this stage does not share hardware direct extraction entry ID space with
 *  a previously defined stage, sharesEntriesDe is -1.  If this stage shares
 *  hardware direct extraction entry ID space with a previously defined stage,
 *  sharedEntriesDe is the index of the most recently defined such stage.
 */
#define _BCM_DPP_FIELD_DEV_STAGE_FLAG_IMPLIED_ACTION   0x00000001
#define _BCM_DPP_FIELD_DEV_STAGE_FLAG_ALLOW_CASCADE    0x00000002
#define _BCM_DPP_FIELD_DEV_STAGE_FLAG_ALLOW_TYPE_MERGE 0x00000004
#define _BCM_DPP_FIELD_DEV_STAGE_USES_PRESELECTORS     0x00000008
typedef struct _bcm_dpp_field_device_stage_info_s {
    char *stageName;                            /* stage name */
    bcm_field_stage_t bcmStage;                 /* BCM layer stage ID */
    SOC_PPC_FP_DATABASE_STAGE hwStageId;        /* hardware stage ID */
    uint32 stageFlags;                          /* stage flags */
    unsigned int maxGroups;                     /* max groups */
    uint32 maxEntriesInternalTcam;                        /* max internal TCAM entries */
    uint32 maxEntriesExternalTcam;                        /* max external TCAM entries */
    unsigned int maxEntriesDe;                  /* max dir ext entries */
    unsigned int maxProgQuals;                  /* max programmable quals */
    unsigned int entryMaxQuals;                 /* max quals per entry */
    unsigned int entryMaxActs;                  /* max actions per entry */
    unsigned int entryDeMaxQuals;               /* max qualifiers for dir ext */
    int sharesGroups;                           /* HW grp spc sharing */
    int sharesEntriesTc;                        /* HW ent spc sharing */
    int sharesEntriesDe;                        /* HW dir ext ent spc sharing */
} _bcm_dpp_field_device_stage_info_t;

/*
 *  This describes how a cell in a layer maps to the stage and type.  In each
 *  of the BCM qualifier and action descriptors, there is a set of PPD values.
 *  This set of values consists of one qualifier/action for each possible
 *  element of the longest supported chain, for each of the elements in the
 *  array made of these mapping descriptions.  Each chain set corresponds to
 *  the description at the same offset in the table made from these mapping
 *  descriptors.
 *
 *  stage is the stage index.
 *
 *  type is the group type index.  If the flags value does not include
 *  _BCM_DPP_FIELD_GROUP_PRESEL, this must be a valid type number.  If the
 *  flags value does include _BCM_DPP_FIELD_GROUP_PRESEL, it will be ignored,
 *  and the type thus defined will be used for that stage's preselectors.
 *
 *  predefKey is the predefined key for this group type in this stage, or
 *  SOC_PPC_NOF_FP_PREDEFINED_ACL_KEYS if there is no predefined key.
 *
 *  flags indicates which of the group flags should be set out of the set of
 *  flags that indicate the group stage and mode.
 *
 *  These must be filled in the same order as the cells are filled in within a
 *  layer of the qualifier mapping table.
 */
#define _BCM_DPP_FIELD_STAGE_INDEX_INGRESS      0
#define _BCM_DPP_FIELD_STAGE_INDEX_EGRESS       1
#define _BCM_DPP_FIELD_STAGE_INDEX_ING_EXTERNAL 2
#define _BCM_DPP_FIELD_STAGE_INDEX_ING_SLB      3

typedef struct _bcm_dpp_field_device_st_mapping_s {
    _bcm_dpp_field_stage_idx_t stmStage;        /* stage index */
    _bcm_dpp_field_type_idx_t stmType;          /* type index */
    SOC_PPC_FP_PREDEFINED_ACL_KEY predefKey;    /* predefined key */
    uint32 stmFlags;                            /* group type flags */
} _bcm_dpp_field_device_st_mapping_t;

/*
 *  This describes a device (gives certain criteria about the device) and
 *  provides a pointer to the description tables for the device.
 *
 *  stages indicates how many field stages are supported on this device.
 *
 *  types indicates how many group types (not group modes -- a group type is
 *  something like IPv4, L2, MPLS, &c) are supported each stage on the device.
 *
 *  qualChain indicates the longest chain of qualifiers at PPD layer that
 *  correspond to a single BCM layer qualifier.  In most cases, such qualifiers
 *  will generally require special code when adding support for them, since
 *  they will tend to be long and the code only internally supports qualifiers
 *  of up to 64b.
 *
 *  mappings indicates how many mappings are defined by the table built
 *  _bcm_dpp_field_qual_mapping_t, above.
 *
 *  stage points to the table of stage descriptors.
 *
 *  qMapInfo points to the _bcm_dpp_field_qual_mapping_t array that defines how
 *  the mapping is done between qualifier chains and group types within each of
 *  the supported stages.
 *
 *  modeBits points to the table that specifies which group modes are supported
 *  in each stage and how many bits are represented by each mode.
 *
 *  qualMap points to the table that describes each of the supported BCM
 *  qualifiers and how they map to PPD qualifiers for each type of group in
 *  each stage of the device.
 *
 *  typeNames points to an array of strings that give a name to each of the
 *  supported group types.
 */
typedef struct _bcm_dpp_field_device_info_s {
    _bcm_dpp_field_stage_idx_t stages;                 /* number of stages */
    _bcm_dpp_field_type_idx_t types;                   /* number of types */
    _bcm_dpp_field_map_idx_t mappings;                 /* number of mappings */
    _bcm_dpp_field_chain_idx_t qualChain;              /* max qual chain */
    _bcm_dpp_field_chain_idx_t actChain;               /* max action chain */
    _bcm_dpp_field_presel_idx_t presels;               /* number of presels */
    uint32 cascadeKeyLimit;                            /* cascade key max len */
    _bcm_dpp_field_device_stage_info_t *stage;         /* stage information */
    const _bcm_dpp_field_device_st_mapping_t *stMapInfo; /* qual map defs */
    const _bcm_dpp_field_device_group_mode_bits_t *modeBits; /* mode max lens */
    const _bcm_dpp_field_device_range_info_t *ranges;  /* range type table */
    int32 *qualMap;                                    /* qual map table */
    int32 *actMap;                                     /* action map table */
    const char* const* typeNames;                      /* group types */
} _bcm_dpp_field_device_info_t;

/*
 *  Information needed for a group
 *
 *  _BCM_DPP_FIELD_GROUP_IN_USE - group is in use
 *  _BCM_DPP_FIELD_GROUP_IN_HW - group is in hardware (as database)
 *  _BCM_DPP_FIELD_GROUP_PHASE - priority phase of copy in hardware
 *  _BCM_DPP_FIELD_GROUP_WITH_CE_ID - group has ce_id list which set explicit ce
 *  _BCM_DPP_FIELD_GROUP_CHG_ENT - group has changed entries
 *  _BCM_DPP_FIELD_GROUP_ADD_ENT - group has added/reordered entries
 *  _BCM_DPP_FIELD_GROUP_INGRESS - group is ingress stage
 *  _BCM_DPP_FIELD_GROUP_INGRESS - group is egress stage
 *  _BCM_DPP_FIELD_GROUP_AUTO_SZ - group created with 'auto' mode
 *  _BCM_DPP_FIELD_GROUP_L2 - group has L2 qualifiers
 *  _BCM_DPP_FIELD_GROUP_IPV4 - group has IPv4 qualifiers
 *  _BCM_DPP_FIELD_GROUP_IPV6 - group has IPv6 qualifiers
 *  _BCM_DPP_FIELD_GROUP_MPLS - group has MPLS qualifiers
 *  _BCM_DPP_FIELD_GROUP_PRESEL - group has presel set explicitly specified
 *  _BCM_DPP_FIELD_GROUP_CASCADE - group uses cascading features
 *  _BCM_DPP_FIELD_GROUP_TINFO - flags that can change with set
 *
 *  Note CHG_ENT is a 'lazy' bit in that it sticks on whenever an entry in a
 *  group changes for any reason, and is only cleared when committing the
 *  entire group (at this time).  It should not be taken as a 'reliable' state.
 *
 *  BCM considers IPv4 and IPv6 header elements to be the same things (for
 *  example, bcmFieldQualifyIpProtocol == bcmFieldQualifyIp6NextHeader).  PPD
 *  uses different identifiers.  This means that a group must not have both
 *  IPv4 and IPv6 qualifiers.  While this leaves it possible to overlap one of
 *  the IPs with MPLS and L2, it requires the application make additional
 *  checks in the L2 qualifiers (such as EtherType) to ensure accuracy of the
 *  comparison at higher layers.
 */
#define _BCM_DPP_FIELD_GROUP_IN_USE                 0x00000001
#define _BCM_DPP_FIELD_GROUP_IN_HW                  0x00000002
#define _BCM_DPP_FIELD_GROUP_PHASE                  0x00000004
#define _BCM_DPP_FIELD_GROUP_CHG_ENT                0x00000010
#define _BCM_DPP_FIELD_GROUP_ADD_ENT                0x00000020
#define _BCM_DPP_FIELD_GROUP_INGRESS                0x00000040
#define _BCM_DPP_FIELD_GROUP_EGRESS                 0x00000080
#define _BCM_DPP_FIELD_GROUP_AUTO_SZ                0x00000100
#define _BCM_DPP_FIELD_GROUP_L2                     0x00000200
#define _BCM_DPP_FIELD_GROUP_IPV4                   0x00000400
#define _BCM_DPP_FIELD_GROUP_IPV6                   0x00000800
#define _BCM_DPP_FIELD_GROUP_MPLS                   0x00001000
#define _BCM_DPP_FIELD_GROUP_PRESEL                 0x00002000
#define _BCM_DPP_FIELD_GROUP_CASCADE                0x00004000
#define _BCM_DPP_FIELD_GROUP_SMALL                  0x00008000
#define _BCM_DPP_FIELD_GROUP_LOOSE                  0x00010000 /* insertion priority order canceled */
#define _BCM_DPP_FIELD_GROUP_WITH_PRESEL            0x00020000 /* install group in HW only once after presel creation */
#define _BCM_DPP_FIELD_GROUP_SPARSE_PRIO            0x00040000 /* field groups with few priorities */
#define _BCM_DPP_FIELD_GROUP_IS_EQUAL_LSB           0x00080000 /* first DB for compare */
#define _BCM_DPP_FIELD_GROUP_IS_EQUAL_MSB           0x00100000 /* second DB for compare */
#define _BCM_DPP_FIELD_GROUP_WITH_HEADER_SELECTION  0x00200000 /* field groups with header index selection*/
#define _BCM_DPP_FIELD_GROUP_USE_DBAL               0x00400000 /* mapped SOC_PPC_FP_DATABASE_INFO_FLAGS_USE_DBAL */
#define _BCM_DPP_FIELD_GROUP_EXTENDED_DATABASES     0x00800000 
#define _BCM_DPP_FIELD_GROUP_STAGE_VT_EXTENDED      0x01000000 
#define _BCM_DPP_FIELD_GROUP_HANDLE_ENTRIES_BY_KEY  0x02000000 /* SOC_PPC_FP_DATABASE_INFO_FLAGS_HANDLE_ENTRY_BY_KEY */
#define _BCM_DPP_FIELD_GROUP_SINGLE_BANK            0x08000000
#define _BCM_DPP_FIELD_GROUP_TINFO                  0x00001EC5
#define _BCM_DPP_FIELD_GROUP_LARGE                  0x10000000 /* allocate FES for direct extraction */
#define _BCM_DPP_FIELD_GROUP_VT_CLASSIFICATION      0x20000000
#define _BCM_DPP_FIELD_GROUP_WITH_CYCLE             0x40000000 /* cycle value */
#define _BCM_DPP_FIELD_GROUP_WITH_PROGRAM_ID        0x80000000 /*create field group with program ID*/
#define _BCM_DPP_FIELD_GROUP_WITH_CE_ID             0x00000008 /*create field group with ce ID*/

#define _BCM_DPP_FIELD_GROUP_ENTRY_BY_KEY_EMPTY     -1

typedef enum _bcm_field_vt_classification_table_type_e {
    _BCM_DPP_VT_CLASS_IPV4_MATCH = 0,
    _BCM_DPP_VT_CLASS_EFP,
    _BCM_DPP_VT_CLASS_VRRP,
    _BCM_DPP_VT_QINQ_COMPRESSED_OUTER,
    _BCM_DPP_VT_QINQ_COMPRESSED_INNER,
    _BCM_DPP_VT_QINANY_OUTER,
    _BCM_DPP_VT_QINANY_INNER,
    _BCM_DPP_VT_QINANY_PCP_OUTER,
    _BCM_DPP_VT_QINANY_PCP_INNER,
    _BCM_DPP_VT_DOT1Q_OUTER,
    _BCM_DPP_VT_DOT1Q_INNER,
    _BCM_DPP_VT_DOT1Q_COMPRESSED_OUTER,
    _BCM_DPP_VT_DOT1Q_COMPRESSED_INNER,
    _BCM_DPP_VT_DOT1Q_COMPRESSED_PCP_OUTER,
    _BCM_DPP_VT_DOT1Q_COMPRESSED_PCP_INNER,
    _BCM_DPP_VT_UNTAGGED,
    _BCM_DPP_VT_TST2,
    _BCM_DPP_VT_TEST2,
    _BCM_DPP_VT_PORT_DA,
    _BCM_DPP_VT_CLASS_COUNT /* always last, not a valid value */
} _bcm_field_vt_classification_table_type_t;

typedef enum _bcm_field_flp_classification_table_type_e {
    _BCM_DPP_FLP_CLASS_IPV4_MPLS_EXTENDED = 0,
    _BCM_DPP_FLP_CLASS_IPV6_EXTENDED,
    _BCM_DPP_FLP_CLASS_P2P_EXTENDED,
    _BCM_DPP_FLP_CLASS_IPV4_MC_FLEXIBLE,
	_BCM_DPP_FLP_CLASS_INRIF_MAPPING,
    _BCM_DPP_FLP_CLASS_IVL_LEARN,                   /* IVL database access by {MAC;VSI;IVEC.VID} */
    _BCM_DPP_FLP_CLASS_IVL_INNER_LEARN,             /* IVL database access by {MAC;VSI;Inner-Most.VID} */
    _BCM_DPP_FLP_CLASS_IVL_FWD_OUTER_LEARN,         /* IVL database access by {MAC;VSI;FWD-Outer.VID} */

    _BCM_DPP_FLP_CLASS_COUNT /* always last, not a valid value */
} _bcm_field_flp_classification_table_type_t;

typedef struct _bcm_dpp_field_group_s {
    /* derivable from config or hardware */
    bcm_field_qset_t qset;                      /* qualifiers for this group */
    bcm_field_aset_t aset;                      /* actions for this group */
    bcm_field_presel_set_t preselSet;           /* preselector set */
    bcm_field_presel_set_t preselHw;            /* preselector set in hw */
    /*_bcm_dpp_field_qual_set_t pqset;  */
    SHR_BITDCL pqset[_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES)];         /* DPP qualifiers */
    /*_bcm_dpp_field_action_set_t paset;  */        
    SHR_BITDCL paset[_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_ACTION_TYPES)];    /* DPP actions */
    uint32 groupTypes;                          /* this group's types, Bitmap of which types supprted: L2/IPv4/IPv6/MPLS */
    _bcm_dpp_field_ent_idx_t entryHead;         /* first entry in group */
    _bcm_dpp_field_ent_idx_t entryTail;         /* last entry in group */
    _bcm_dpp_field_ent_idx_t entryCount;        /* number of entries */
    _bcm_dpp_field_grp_idx_t groupNext;         /* next group */
    _bcm_dpp_field_grp_idx_t groupPrev;         /* previous group */
    _bcm_dpp_field_grp_idx_t cascadePair;       /* other in cascade pair */
    int priority;                               /* group priority */
    int maxEntryPriorities;                     /* max entry priorities in group, constant value
                                                                                    (entry priority where the enrty will be located in the TCAM bank)*/
    /* kept in backing store to make it easier */
    _bcm_dpp_field_stage_idx_t stage;           /* this group's stage */
    SOC_PPC_FP_PREDEFINED_ACL_KEY predefKey;    /* predefined key (if used)*/
    /* needs to be kept in backing store */
    uint32 groupFlags;                          /* group flags _BCM_DPP_FIELD_GROUP*/
    bcm_field_group_mode_t grpMode;             /* group mode, DE/DT/TCAM... */
    bcm_field_aset_t oaset;                     /* original (caller's) aset */
    /* 
        * vlan translation classification Id 
        * Relevant only in case of Vlan translation stage. 
        */
    uint32          internal_table_id;          
    uint32          handle_by_key_entry_id;     /* a unique entry for groups that handles entry by key */
    SOC_DPP_DBAL_PHYSICAL_DB_TYPES    physicalDB;     /* the type of the physical database to use for VTT FLP stages*/
    
    bcm_field_group_cycle_t groupCycle;         /* cycle value */
    uint32 pgm_bmp;                     /* Bitmap of program-ids which the
                                           current field group will be pointing
                                           to. */
    uint32          share_id;                   /* share ID, groups with same share ID may share TCAM banks */ 
    uint32          num_entry_in_keep_location; /* number of entry in keep location mode */
} _bcm_dpp_field_group_t;

typedef struct _bcm_dpp_field_group_presel_info_s {
    int nof_presel_info;
    bcm_field_group_presel_info_t presel_info[BCM_FIELD_NOF_PRESEL_PER_FG];
} _bcm_dpp_field_group_presel_info_t;

/*
 *  Describe a single qualifier
 *
 *  PPD layer says qualifier >= SOC_PPC_NOF_FP_QUAL_TYPES is invalid, but it
 *  also curiously provides SOC_PPC_FP_QUAL_IRPP_INVALID as an alternative
 *  invalid value, so check both, since (per the example of actions, see
 *  below), it is likely that somewhere the PPD will provide the value of
 *  SOC_PPC_FP_QUAL_IRPP_INVALID instead of SOC_PPC_NOF_FP_QUAL_TYPES.
 */
#define _BCM_DPP_FIELD_PPD_QUAL_VALID(_qual) \
     ((SOC_PPC_FP_QUAL_IRPP_INVALID != (_qual)) && \
       (BCM_FIELD_ENTRY_INVALID != (_qual)) && \
      (SOC_PPC_NOF_FP_QUAL_TYPES > (_qual)))
typedef struct _bcm_dpp_field_qual_s {
    /* can be retrieved from hardware or derived thence */
    bcm_field_qualify_t qualType;               /* BCM qualifier type */
    SOC_PPC_FP_QUAL_TYPE hwType;                /* hardware qualifier type */
    uint64 qualData;                            /* qualifier data (hardware) */
    uint64 qualMask;                            /* qualifier mask (hardware) */
} _bcm_dpp_field_qual_t;

/* Keeps the original input values to the BCM qualifier set API */
typedef struct _bcm_dpp_field_b_qual_s {
    /* must be kept in backing store */
    bcm_field_qualify_t qualType;               /* BCM qualifier type */
    uint32 bcmParam0;                           /* qualifier data (sw) */
    uint32 bcmMask0;                            /* qualifier mask (sw) */
    uint32 bcmParam1;                           /* qualifier data (sw) */
    uint32 bcmMask1;                            /* qualifier mask (sw) */
} _bcm_dpp_field_b_qual_t;

/*
 *  Describe a single TCAM action (BCM view)
 */
typedef struct _bcm_dpp_field_tc_b_act_s {
    /* must be kept in backing store */
    bcm_field_action_t bcmType;                 /* BCM action type */
    uint32 bcmParam0;                           /* action param0 */
    uint32 bcmParam1;                           /* action param1 */
    uint32 bcmParam2;                           /* action param2 */
    uint32 bcmParam3;                           /* action param3 */
} _bcm_dpp_field_tc_b_act_t;
/*
 *  Describe a single TCAM action (PPD view)
 *
 *  The special flags can vary by hwType.  Most hwType values will not use it,
 *  but some will.  For example, SOC_PPC_FP_ACTION_TYPE_EGR_TRAP will use flags
 *  to indicate which value of SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE* is
 *  intended.
 *
 *  PPD layer says action type >= SOC_PPC_NOF_FP_ACTION_TYPES is invalid, but
 *  it also curiously provides SOC_PPC_FP_ACTION_TYPE_INVALID to indicate
 *  invalid action type.  The PPD code usually seems to set
 *  SOC_PPC_FP_ACTION_TYPE_INVALID as the invalid type on reading, but the
 *  other is also possible.  We will set SOC_PPC_FP_ACTION_TYPE_INVALID on
 *  writing as well, but still need to check the other mode when reading.
 */
#define _BCM_DPP_FIELD_PPD_ACTION_VALID(_action) \
     ((SOC_PPC_FP_ACTION_TYPE_INVALID != (_action)) && \
      (BCM_FIELD_ENTRY_INVALID != (_action)) && \
      (SOC_PPC_NOF_FP_ACTION_TYPES > (_action)))
typedef struct _bcm_dpp_field_tc_p_act_s {
    /* can be retrieved from hardware */
    SOC_PPC_FP_ACTION_TYPE hwType;              /* hardware action type */
    uint32 hwParam;                             /* hardware action parameter */
    uint32 hwFlags;                             /* special flags for action */
} _bcm_dpp_field_tc_p_act_t;

/*
 *  Describe a single Direct Extraction action
 */
typedef struct _bcm_dpp_field_de_act_s {
    /* can be retrieved from hardware */
    SOC_PPC_FP_DIR_EXTR_ACTION_VAL  hwParam;     /* hardware parameters */
    /* must be kept in backing store */
    bcm_field_action_t bcmType;                 /* BCM action type */
} _bcm_dpp_field_de_act_t;


/* those defines are used for direct mapping to the table id in the external TCAM.
	in all other cases the table ID should be taken from the DBAL include file */
#define _BCM_DPP_INTERNAL_TABLE_ID_EXTENDED_IPV4_MPLS           13 /*ARAD_KBP_FRWRD_TBL_ID_LSR_IP_SHARED*/       
#define _BCM_DPP_INTERNAL_TABLE_ID_EXTENDED_IPV6                16 /*ARAD_KBP_FRWRD_TBL_ID_EXTENDED_IPV6*/       
#define _BCM_DPP_INTERNAL_TABLE_ID_EXTENDED_P2P                 17 /*ARAD_KBP_FRWRD_TBL_ID_EXTENDED_P2P*/
#define _BCM_DPP_INTERNAL_TABLE_ID_EXTENDED_IPV4_MC             2  /*ARAD_KBP_FRWRD_TBL_ID_IPV4_MC */       
#define _BCM_DPP_INTERNAL_TABLE_ID_INRIF_MAPPING                18 /*ARAD_KBP_FRWRD_TBL_ID_INRIF_MAPPING*/

/*
 *  Information needed for an entry
 *
 *  _BCM_DPP_FIELD_ENTRY_IN_USE - entry is in use
 *  _BCM_DPP_FIELD_ENTRY_IN_HW - entry is in hardware
 *  _BCM_DPP_FIELD_ENTRY_WANT_HW - entry wants to be in hardware
 *  _BCM_DPP_FIELD_ENTRY_UPDATE - specifies which entries to update
 *  _BCM_DPP_FIELD_ENTRY_CHANGED - entry has been changed
 *  _BCM_DPP_FIELD_ENTRY_NEW - entry is new (HW priority not valid)
 *  _BCM_DPP_FIELD_ENTRY_STATS - entry has associated statistics
 *  _BCM_DPP_FIELD_ENTRY_EGR_PRO - entry uses egress action profile
 *  _BCM_DPP_FIELD_ENTRY_ACE_PRO - entry uses an ACE profile
 *
 *  _BCM_DPP_FIELD_ENTRY_UPDATE_REPLACE - (ARAD-only flag) entry was updated but not installed yet
 *  _BCM_DPP_FIELD_ENTRY_IN_HW_VALID - (ARAD-only flag) entry is in HW and it's valid
 *  _BCM_DPP_FIELD_ENTRY_WANT_DESTROY - (ARAD-only flag) entry needs to be removed from HW
 */
#define _BCM_DPP_FIELD_ENTRY_IN_USE  0x00000001
#define _BCM_DPP_FIELD_ENTRY_IN_HW   0x00000002
#define _BCM_DPP_FIELD_ENTRY_WANT_HW 0x00000004
#define _BCM_DPP_FIELD_ENTRY_UPDATE  0x00000008
#define _BCM_DPP_FIELD_ENTRY_CHANGED 0x00000010
#define _BCM_DPP_FIELD_ENTRY_NEW     0x00000020
#define _BCM_DPP_FIELD_ENTRY_STATS   0x00000100
#define _BCM_DPP_FIELD_ENTRY_EGR_PRO 0x00000200
#define _BCM_DPP_FIELD_ENTRY_ACE_PRO 0x00000400
#define _BCM_DPP_FIELD_ENTRY_KEEP_LOCATION 0x00000800


#define _BCM_DPP_FIELD_GROUP_INSTALL    0x00010000
/*
 * 1. The priority of the entries in the HW is in increasing order,
 *    so that an entry with priority 0 is with the highest priority;
 *    as opposed to BCM priority which is in decreasing order.
 * 2. Also, the priority as defined in PPD is 16 bits, whereas here
 *    it is 32 bit, hence the translation. Setting an entry with
 *    priority in the range (0xffff + 1, BCM_FIELD_ENTRY_PRIO_HIGHEST - 1)
 *    will fail.
 */
#define BCM_FIELD_ENTRY_PRIO_HIGHEST_EXT_TCAM (0x3fffff) /* Highest possible entry
                                                            priority in a group which is in external TCAM. */

#define BCM_FIELD_ENTRY_HIGHEST_PRIORITY(isExternalTcam)    \
((isExternalTcam) ? BCM_FIELD_ENTRY_PRIO_HIGHEST_EXT_TCAM : BCM_FIELD_ENTRY_PRIO_HIGHEST)

#define BCM_FIELD_ENTRY_BCM_PRIO_TO_DPP_PRIO(isExternalTcam, prio)  \
((BCM_FIELD_ENTRY_HIGHEST_PRIORITY(isExternalTcam)) - (prio))

/*
 *  Normally, a BCM layer action maps to a single PPD layer action, but there
 *  are cases where a single BCM layer action maps to more than one PPD layer
 *  action, or more than one BCM layer action maps to one PPD layer action.
 *
 *  _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX should be set considering whether the
 *  BCM or PPD actions tend to be more plentiful.
 *
 *  This only applies to TCAM actions; at this point, other action types are
 *  limited exactly by the support at PPD layer.
 */
#define _BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX
/*
 * Number of qualifiers for which SW state parameters keep is implemented.
 * Set APIs update entryBcmQual descriptor.
 * Get APIs rely on it to derive the original parameters.
 * Currently implemented: bcmFieldQualifySrcPort, bcmFieldQualifyDstPort.
 */
#define _BCM_DPP_NOF_BCM_QUALIFIERS_PER_ENTRY_TO_KEEP 3
/* Macro to check whether the provided qualifier was implemented for keeping */
#define _BCM_DPP_QUALIFIER_IS_KEPT_IN_SW_STATE(bcmQual) \
((bcmQual == bcmFieldQualifySrcPort || bcmQual == bcmFieldQualifyDstPort || bcmQual == bcmFieldQualifyPacketRes) ? 1 : 0)
/*
 *  Describe the common elements of entries
 */
typedef struct _bcm_dpp_field_entry_common_s {
    /* derivable from config or hardware */
    _bcm_dpp_field_ent_idx_t entryNext;         /* next entry */
    _bcm_dpp_field_ent_idx_t entryPrev;         /* previous entry */
    _bcm_dpp_field_grp_idx_t entryGroup;        /* this entry's group */
    uint32 hwPriority;                          /* entry priority in hardware */
    _bcm_dpp_field_qual_t entryQual[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]; /* qualifiers */
    /* needs to be kept in backing store */
    uint32 entryFlags;                          /* entry flags */
    int entryPriority;                          /* entry priority */
    uint32 hwHandle;                            /* entry handle in hardware */
    _bcm_dpp_field_b_qual_t entryBcmQual[_BCM_DPP_NOF_BCM_QUALIFIERS_PER_ENTRY_TO_KEEP]; /* BCM qualifiers if kept*/
    /* partially derivable from hardware, part in backing store */
} _bcm_dpp_field_entry_common_t;

/*
 *  Describe a TCAM entry
 */
typedef struct _bcm_dpp_field_entry_s {
    /* derivable from config or hardware */
    _bcm_dpp_field_entry_common_t entryCmn;     /* common data for all types */
    _bcm_dpp_field_tc_p_act_t tcActP[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
    uint32 egrTrapProfile;                      /* hw egr trap profile */
    /* must be kept in backing store */
    _bcm_dpp_field_tc_b_act_t tcActB[_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX];
    uint32 large_direct_lookup_key;             /* Large direct lookup key */
    uint32 large_direct_lookup_mask;            /* Large direct lookup mask */
} _bcm_dpp_field_entry_t;

/*
 *  Describe an external TCAM info entry
 */
typedef struct _bcm_dpp_field_entry_ext_info_s {
    _bcm_dpp_field_qual_t entryQual[SOC_PPC_FP_NOF_QUALS_PER_DB_MAX]; /* qualifiers */
    _bcm_dpp_field_tc_p_act_t tcActP[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX]; /* ppd actions */
    _bcm_dpp_field_tc_b_act_t tcActB[_BCM_DPP_NOF_BCM_ACTIONS_PER_DB_MAX]; /* bcm actions */
    uint32 hwPriority; /* entry priority in hardware */
    int entryPriority; /* entry priority */
} _bcm_dpp_field_entry_ext_info_t;

/*
 *  Describe the common elements of external TCAM entries
 */
typedef struct _bcm_dpp_field_entry_common_ext_s {
    /* derivable from config or hardware */
    _bcm_dpp_field_ent_idx_t entryNext;      /* next entry */
    _bcm_dpp_field_ent_idx_t entryPrev;      /* previous entry */
    _bcm_dpp_field_grp_idx_t entryGroup;     /* this entry's group */
    uint32 entryFlags;                       /* entry flags */
    uint32 hwHandle;                         /* entry handle in hardware */
#if _BCM_DPP_EXT_TCAM_ENTRIES_IN_SW_STATE
    _bcm_dpp_field_b_qual_t entryBcmQual[_BCM_DPP_NOF_BCM_QUALIFIERS_PER_ENTRY_TO_KEEP]; /* BCM qualifiers if kept*/
#endif
} _bcm_dpp_field_entry_common_ext_t;

/*
 *  Describe an external TCAM entry
 */
typedef struct _bcm_dpp_field_entry_ext_s {
    _bcm_dpp_field_entry_common_ext_t entryCmn;/* common data */
    uint32 ext_info_idx; 	/* index to external info array */
} _bcm_dpp_field_entry_ext_t;

/*
 *  Describe a Direct Extraction entry
 */
typedef struct _bcm_dpp_field_entry_dir_ext_s {
    _bcm_dpp_field_entry_common_t entryCmn;     /* common data for all types */
    _bcm_dpp_field_de_act_t deAct[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX]; /* actions */
} _bcm_dpp_field_entry_dir_ext_t;

/*
 *  Stage specific information
 *
 *  The extra qsetType is for preselectors; similar for the extra offset.
 */
#define _BCM_DPP_FIELD_RESOURCES_PER_STAGE 3
typedef struct _bcm_dpp_field_stage_s {
    /* derivable from config or hardware */
    bcm_field_qset_t stgQset;                   /* stage qualifiers */
    bcm_field_qset_t qsetType[_BCM_DPP_FIELD_MAX_GROUP_TYPES + 1];/* type qset */
    SHR_BITDCL ppqset[_SHR_BITDCLSIZE(SOC_PPC_NOF_FP_QUAL_TYPES)];           /* presel PPD qual set */
    bcm_field_aset_t stgAset;                   /* stage actions */
    PARSER_HINT_PTR _bcm_dpp_field_device_group_mode_bits_t *modeBits[bcmFieldGroupModeCount];
                                                /* stage mode information */
    unsigned int ltOffset[_BCM_DPP_FIELD_MAX_GROUP_TYPES + 1];/* lkup tbl ofs */
    _bcm_dpp_field_stage_idx_t groupSh1;        /* first in group share set */
    _bcm_dpp_field_stage_idx_t entryTcSh1;      /* first in TC ent share set */
    _bcm_dpp_field_stage_idx_t entryDeSh1;      /* first in DE ent share set */
    uint32 hwGroupLimit;                        /* hardware group ID limit */
    uint32 hwEntryLimit;                        /* hardware entry ID limit */
    uint32 hwEntryDeLimit;                      /* HW dir ext entry ID limit */
    _bcm_dpp_field_grp_idx_t groupHead;         /* first group this stage */
    _bcm_dpp_field_grp_idx_t groupTail;         /* last group this stage */
    _bcm_dpp_field_grp_idx_t groupCount;        /* active groups this stage */
    _bcm_dpp_field_ent_idx_t entryCount;        /* active entries this stage */
    _bcm_dpp_field_ent_idx_t entryElems;        /* active HW entry elements */
    _bcm_dpp_field_ent_idx_t entryDeCount;      /* active dir ext entries */
    _bcm_dpp_field_ent_idx_t entryDeElems;      /* active dir ext HW elems */
    /* must be kept in backing store  */
    bcm_field_presel_set_t pfgsEther;           /* PFG set for ethernet */
    bcm_field_presel_set_t pfgsIPv4;            /* PFG set for IPv4 */
    bcm_field_presel_set_t pfgsIPv6;            /* PFG set for IPv6 */
    bcm_field_presel_set_t pfgsMPLS;            /* PFG set for MPLS */
} _bcm_dpp_field_stage_t;

/*
 *  Unit-specific information
 *
 *  Note the actual memory for 'group' and 'entry' will be allocated
 *  immediately after the memory for this struct, and it will be allocated in
 *  the same heap cell.
 *
 *  _BCM_DPP_FIELD_UNIT_WB_RESTORE_FAIL is set when restoring from the backing
 *  store and the data therein are inconsistent with either the hardware state
 *  or internally.  Note that inconsistencies are only verified for active
 *  elements, so if the backing store covers things not supported by the new
 *  code, it is only an error here if those things are in use; also, if the
 *  backing store does not cover a certain resource, it will be assumed to be
 *  entirely unused (and not treated as an inconsistency).
 *
 *  _BCM_DPP_FIELD_UNIT_WB_VERSION_FAIL is set whenever the backing store
 *  version is not recognized by some component of the module. This should
 *  never happen and it indicates some component was missed when an update to
 *  the backing store version was made.
 *
 *  _BCM_DPP_FIELD_UNIT_WB_FORMAT_FAIL is set whenever there is a dynamically
 *  sized object that has parts which will not fit in the backing store.  This
 *  should only happen when writing to backing store using an older format.
 *
 *  _BCM_DPP_FIELD_UNIT_WB_COMMIT_FAIL is set whenever there is a failure to
 *  update the backing store.  It indicates the backing store is not in sync
 *  with the hardware state.  For now, the only way to clear this is to reinit
 *  the field APIs or to have a full state sync complete successfully.
 */


typedef struct _bcm_dpp_field_info_OLD_s {
    /* derivable from config or hardware */
    int unit;                                   /* unit ID */
    uint32 unitHandle;                          /* PPD device ID */
    const _bcm_dpp_field_device_info_t *devInfo;/* pointer to device info */
} bcm_dpp_field_info_OLD_t;
extern bcm_dpp_field_info_OLD_t *_bcm_dpp_field_unit_info[BCM_MAX_NUM_UNITS];

#define BCM_FIELD_GROUP_CONFIG_MAX_ID_COUNT 128

typedef struct _bcm_dpp_field_info_s {
    unsigned int                rangeQualTypes;    /* range qualifier types */

        /*
         * Qualifier mappings. Array of bcmFieldQualifyCount entries. Each is an index
         * into selected constant array: bcm_dpp_field_info_t.devInfo->->qualMap[]
         * Note that, currently, temporarily, this is:
         * bcm_dpp_field_info_OLD_t.devInfo->->qualMap[]
         * Note:
         * The whole structure, containing 'bcm_dpp_field_info_t' for 'unit' is pointed
         * by _bcm_dpp_field_unit_info[unit]
         */
    PARSER_HINT_ARR  int32 *qualMaps ;
	/*
         * Action mappings. Array of bcmFieldActionCount entries. Each is an index
         * into selected constant array: bcm_dpp_field_info_t.devInfo->->actMap[]
         * Note that, currently, temporarily, this is:
         * bcm_dpp_field_info_OLD_t.devInfo->->actMap[]
         * Note:
         * The whole structure, containing 'bcm_dpp_field_info_t' for 'unit' is pointed
         * by _bcm_dpp_field_unit_info[unit]
         */
    PARSER_HINT_ARR  int32 *actMaps ;
    bcm_field_qualify_t         ppdQual[SOC_PPC_NOF_FP_QUAL_TYPES];/* PPD -> BCM qual map */
    unsigned int                qualMapCount;      /* qualifier map table size */
    unsigned int                actMapCount;       /* action map table size */
    _bcm_dpp_field_presel_idx_t preselLimit;       /* max preselectors */
        /*
         * unitQset,unitAset -- Structures containing a few arrays of bits.
         * Note:
         * The whole structure, containing 'bcm_dpp_field_info_t' for 'unit' is pointed
         * by _bcm_dpp_field_unit_info[unit]
         */
    bcm_field_qset_t            unitQset;          /* supported qualifiers */
    bcm_field_aset_t            unitAset;          /* supported actions */
    uint8                       cascadedKeyLen;    /* cascaded key length */
    _bcm_dpp_field_ent_idx_t    entryDeCount;      /* active dir ext entries */
    _bcm_dpp_field_ent_idx_t    entryUninstalledTcCount; /* configured but not installed TCAM entries */
    _bcm_dpp_field_grp_idx_t    groupCount;        /* active groups */
    _bcm_dpp_field_grp_idx_t    groupFree;         /* first free group */
    _bcm_dpp_field_grp_idx_t    groupCascaded;     /* groups using cascading */
    _bcm_dpp_field_ent_idx_t    entryIntTcCount;   /* active internal TCAM entries */
    _bcm_dpp_field_ent_idx_t    entryExtTcCount;   /* active external TCAM entries */
    _bcm_dpp_field_ent_idx_t    entryIntTcFree;    /* first free internal TCAM entry */
    _bcm_dpp_field_ent_idx_t    entryExtTcFree;    /* first free external TCAM entry */
    _bcm_dpp_field_ent_idx_t    entryDeFree;       /* first free dir ext entry */

    _bcm_dpp_field_ent_idx_t    entryTcLimit;      /* max internal TCAM entries */
    _bcm_dpp_field_ent_idx_t    entryExtTcLimit;   /* max external TCAM entries */
    _bcm_dpp_field_grp_idx_t    groupLimit;        /* max groups */
    _bcm_dpp_field_ent_idx_t    entryDeLimit;      /* max dir ext entries */
    _bcm_dpp_field_dq_idx_t     dqLimit;           /* max data qualifier */

    SHR_BITDCL dataFieldInUse[_SHR_BITDCLSIZE(_BCM_DPP_NOF_FIELD_DATAS)]; /* programmable fields in use */
    SHR_BITDCL dataFieldOfsBit[_SHR_BITDCLSIZE(_BCM_DPP_NOF_FIELD_DATAS)]; /* offset is bit resolution */
    SHR_BITDCL dataFieldLenBit[_SHR_BITDCLSIZE(_BCM_DPP_NOF_FIELD_DATAS)]; /* length is bit resolution */
    SHR_BITDCL dataFieldDnxOrderBit[_SHR_BITDCLSIZE(_BCM_DPP_NOF_FIELD_DATAS)]; /* Dnx order is bit resolution */
    unsigned int dataFieldRefs[_BCM_DPP_NOF_FIELD_DATAS]; /* reference cnt */
        /*
         * rangeInUse --
         *   Array of pointers (not more than 'bcmFieldQualifyCount' elements)
         *   each pointing to an array of _BCM_DPP_RANGE_MAX_COUNT bits {In
         *   longs: SHR_BITALLOCSIZE(_BCM_DPP_RANGE_MAX_COUNT)}
         *   Esentially, this is a two dimensional array of bits.
         * Note:
         * The whole structure, containing 'bcm_dpp_field_info_t' for 'unit' is pointed
         * by _bcm_dpp_field_unit_info[unit]
         */
    SOC_SAND_OCC_BM_PTR extTcamBmp;        /* pointer to occupation bitmap for external TCAM info table */
    PARSER_HINT_PTR_PTR  SHR_BITDCL **rangeInUse;               /* range in-use information */
    bcm_field_presel_set_t preselInUse;                         /* preselectors in use */
    unsigned int preselRefs[_BCM_DPP_NOF_FIELD_PRESELECTORS];   /* ref count */
    unsigned int preselProfileRefs[_bcmDppFieldProfileTypeCount][_BCM_DPP_PRESEL_NOF_PORT_PROFILES]; /* ref count */
    PARSER_HINT_ARR _bcm_dpp_field_stage_t *stageD;             /* pointer to stage data */
    PARSER_HINT_PTR_SET PARSER_HINT_ARR _bcm_dpp_field_group_t *groupD;
    PARSER_HINT_ARR _bcm_dpp_field_group_presel_info_t *groupHwInfo;
    PARSER_HINT_ARR _bcm_dpp_field_entry_dir_ext_t *entryDe;
    PARSER_HINT_ARR _bcm_dpp_field_entry_t *entryTc;              /* pointer to internal entry table */
    PARSER_HINT_ARR _bcm_dpp_field_entry_ext_t *entryExtTc;       /* pointer to external entry table */
    PARSER_HINT_ARR _bcm_dpp_field_entry_ext_info_t *extTcamInfo; /* pointer to external TCAM info table */
    /*
     * Number of meaningful elements (first index) on Arad_pmf_fem_action_type_encoding_runtime[]
     */
    uint32 num_arad_pmf_fem_action_type_encoding_runtime ;
    uint32 Arad_pmf_fem_action_type_encoding_runtime[SOC_PPC_NOF_FP_ACTION_TYPES][NUM_ACTION_ELEMENTS] ;
    /*
     * Number of meaningful elements (first index) on Arad_plus_pmf_fem_action_type_encoding_runtime[]
     */
    uint32 num_arad_plus_pmf_fem_action_type_encoding_runtime ;
    uint32 Arad_plus_pmf_fem_action_type_encoding_runtime[SOC_PPC_NOF_FP_ACTION_TYPES][NUM_ACTION_ELEMENTS] ;
    /*
     * Number of meaningful elements (first index) on Jericho_pmf_fem_action_type_encoding_runtime[]
     */
    uint32 num_jericho_pmf_fem_action_type_encoding_runtime ;
    uint32 Jericho_pmf_fem_action_type_encoding_runtime[SOC_PPC_NOF_FP_ACTION_TYPES][NUM_ACTION_ELEMENTS] ;
    /*
     * Number of meaningful elements (first index) on Qax_pmf_fem_action_type_encoding_runtime[]
     */
    uint32 num_qax_pmf_fem_action_type_encoding_runtime ;
    uint32 Qax_pmf_fem_action_type_encoding_runtime[SOC_PPC_NOF_FP_ACTION_TYPES][NUM_ACTION_ELEMENTS] ;

    uint8 groupConfigNames[BCM_FIELD_GROUP_CONFIG_MAX_ID_COUNT][BCM_FIELD_MAX_NAME_LEN];    /* Mapping between group config id & name*/
    uint8 dataQualifierNames[bcmFieldQualifyCount][BCM_FIELD_MAX_NAME_LEN];                 /* Mapping between data qualifier id & name*/
    uint8 preselConfigNames[BCM_FIELD_GROUP_CONFIG_MAX_ID_COUNT][BCM_FIELD_MAX_NAME_LEN];   /* Mapping between presel config id & name*/

    uint32 keyGenVar;         /* Key Gen Var */
    uint32 kaps_large_db_bits;
} bcm_dpp_field_info_t ;

/* --------------------- Definitions related to entryTc/entryExtTc/extTcamInfo ------ */
/* { */
#define FIELD_ACCESS_ENTRYTC      FIELD_ACCESS.entryTc
#define FIELD_ACCESS_ENTRYEXTTC   FIELD_ACCESS.entryExtTc
#define FIELD_ACCESS_EXTTCAMINFO  FIELD_ACCESS.extTcamInfo
/* } */
/* ---------------------------------------------------------------------------------- */

/* --------------------- Definitions related to 'runtime' arrays ------ */
/* { */
#define FIELD_ACCESS_ARAD_NUM_ACTION_RUNTIME      FIELD_ACCESS.num_arad_pmf_fem_action_type_encoding_runtime
#define FIELD_ACCESS_ARAD_PLUS_NUM_ACTION_RUNTIME FIELD_ACCESS.num_arad_plus_pmf_fem_action_type_encoding_runtime
#define FIELD_ACCESS_JERICHO_NUM_ACTION_RUNTIME   FIELD_ACCESS.num_jericho_pmf_fem_action_type_encoding_runtime
#define FIELD_ACCESS_QAX_NUM_ACTION_RUNTIME       FIELD_ACCESS.num_qax_pmf_fem_action_type_encoding_runtime
#define FIELD_ACCESS_ARAD_ACTION_RUNTIME          FIELD_ACCESS.Arad_pmf_fem_action_type_encoding_runtime
#define FIELD_ACCESS_ARAD_PLUS_ACTION_RUNTIME     FIELD_ACCESS.Arad_plus_pmf_fem_action_type_encoding_runtime
#define FIELD_ACCESS_JERICHO_ACTION_RUNTIME       FIELD_ACCESS.Jericho_pmf_fem_action_type_encoding_runtime
#define FIELD_ACCESS_QAX_ACTION_RUNTIME           FIELD_ACCESS.Qax_pmf_fem_action_type_encoding_runtime
/* } */
/* ---------------------------------------------------------------------------------- */

/* --------------------- Definitions related to preselInUse/preselRefs/preselProfileRefs ------ */
/* { */
#define FIELD_ACCESS_PRESELPROFILEREFS   FIELD_ACCESS.preselProfileRefs
#define FIELD_ACCESS_PRESELREFS          FIELD_ACCESS.preselRefs
#define FIELD_ACCESS_PRESELINUSE         FIELD_ACCESS.preselInUse
#define FIELD_ACCESS_PRESELINUSE_W       FIELD_ACCESS_PRESELINUSE.w
/* } */
/* --------------------------------------------------------------------------------------------- */

/* --------------------- Definitions related to rangeInUse ------------------ */
/* { */
#define FIELD_ACCESS_RANGEINUSE         FIELD_ACCESS.rangeInUse
/*
 * This macro is added, temporarily, to allow compilation of wb_db_field.c ;
 * Remove in final version (together with the whole file)
 */
#define GET_ADDROF_RANGEINUSE(_unit,_ptr_index,_bitmap_index) \
  &(sw_state[_unit]->dpp.bcm.field->rangeInUse[_ptr_index][_bitmap_index])
/* } */
/* ------------------------------------------------------------------------------------ */

/* --------------------- Definitions related to unitQset ------------------ */
/* { */
#define FIELD_ACCESS_UNITQSET         FIELD_ACCESS.unitQset
#define FIELD_ACCESS_UNITQSET_W       FIELD_ACCESS_UNITQSET.w
#define FIELD_ACCESS_UNITQSET_UDFMAP  FIELD_ACCESS_UNITQSET.udf_map
/* } */
/* ------------------------------------------------------------------------------------ */

/* --------------------- Definitions related to unitAset ------------------ */
/* { */
#define FIELD_ACCESS_UNITASET         FIELD_ACCESS.unitAset
#define FIELD_ACCESS_UNITASET_W       FIELD_ACCESS_UNITASET.w
/* } */
/* ------------------------------------------------------------------------------------ */

/* --------------------- Definitions related to actMaps ------------------ */
/* { */
#define FIELD_ACCESS_ACTMAPS  FIELD_ACCESS.actMaps
/*
 * Verify specified unit has a legal value. If not, software goes to
 * exit with error code.
 * 
 * Notes:
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define SOC_SAND_ACTMAPS_VERIFY_UNIT_IS_LEGAL(unit, _err1) \
  if (((int)unit < 0) || ((int)unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MAX_NUM_DEVICES_OUT_OF_RANGE_ERR, _err1, exit); \
  }
/*
 * Convert input ACTMAPS handle to index in '_bcm_dpp_field_unit_info[unit]->devInfo->actMap' array.
 * Convert input index in actMap array to ACTMAPS handle.
 * Indices go from 0 -> (bcmFieldActionCount - 1)
 * Handles go from 1 -> bcmFieldActionCount
 */
#define SOC_SAND_ACTMAPS_CONVERT_HANDLE_TO_ACTMAPS_INDEX(_actMap_index,_handle) (_actMap_index = _handle - 1)
#define SOC_SAND_ACTMAPS_CONVERT_ACTMAPS_INDEX_TO_HANDLE(_handle,_actMap_index) (_handle = _actMap_index + 1)
/*
 * Convert input ACTMAPS handle to address of element in '_bcm_dpp_field_unit_info[unit]->devInfo->actMap'
 * array (of int32). '_element', then, is of type 'ptr to int32'.
 * Convert input index in actMap array to ACTMAPS handle.
 * Handles go from 1 -> bcmFieldActionCount
 */
#define SOC_SAND_ACTMAPS_CONVERT_HANDLE_TO_ELEMENT(_unit,_handle,_element) \
  { \
    uint32 actMap_index ; \
    SOC_SAND_ACTMAPS_CONVERT_HANDLE_TO_ACTMAPS_INDEX(actMap_index,_handle) ; \
    _element = &(_bcm_dpp_field_unit_info[_unit]->devInfo->actMap[actMap_index]) ; \
  }
/* } */
/* ------------------------------------------------------------------------------------ */

/* --------------------- Definitions related to qualMaps ------------------ */
/* { */
#define FIELD_ACCESS_QUALMAPS  FIELD_ACCESS.qualMaps
/*
 * Verify specified unit has a legal value. If not, software goes to
 * exit with error code.
 * 
 * Notes:
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define SOC_SAND_QUALMAPS_VERIFY_UNIT_IS_LEGAL(unit, _err1) \
  if (((int)unit < 0) || ((int)unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_MAX_NUM_DEVICES_OUT_OF_RANGE_ERR, _err1, exit); \
  }
/*
 * Convert input QUALMAPS handle to index in '_bcm_dpp_field_unit_info[unit]->devInfo->qualMap' array.
 * Convert input index in qualMap array to QUALMAPS handle.
 * Indices go from 0 -> (bcmFieldQualifyCount - 1)
 * Handles go from 1 -> bcmFieldQualifyCount
 */
#define SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_QUALMAPS_INDEX(_qualMap_index,_handle) (_qualMap_index = _handle - 1)
#define SOC_SAND_QUALMAPS_CONVERT_QUALMAPS_INDEX_TO_HANDLE(_handle,_qualMap_index) (_handle = _qualMap_index + 1)
/*
 * Convert input QUALMAPS handle to address of element in '_bcm_dpp_field_unit_info[unit]->devInfo->qualMap'
 * array (of int32). '_element', then, is of type 'ptr to int32'.
 * Convert input index in qualMap array to QUALMAPS handle.
 * Handles go from 1 -> bcmFieldQualifyCount
 */
#define SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_ELEMENT(_unit,_handle,_element) \
  { \
    uint32 qualMap_index ; \
    SOC_SAND_QUALMAPS_CONVERT_HANDLE_TO_QUALMAPS_INDEX(qualMap_index,_handle) ; \
    _element = &(_bcm_dpp_field_unit_info[_unit]->devInfo->qualMap[qualMap_index]) ; \
  }
/* } */
/* ------------------------------------------------------------------------------------ */

#define _BCM_DPP_FIELD_QUALMAP_OFFSET_QUAL 0
#define _BCM_DPP_FIELD_QUALMAP_OFFSET_LENGTH 1
#define _BCM_DPP_FIELD_QUALMAP_OFFSET_SHIFT 2
#define _BCM_DPP_FIELD_QUALMAP_OFFSET_FLAGS 3
#define _BCM_DPP_FIELD_QUALMAP_OFFSET_MAPS 4
#define _BCM_DPP_FIELD_ACTMAP_OFFSET_ACT 0
#define _BCM_DPP_FIELD_ACTMAP_OFFSET_MAPS 1

#ifdef BCM_ARAD_SUPPORT
/*-----------------------------------------------------------------------------
 *
 *  The struct definition below is used in _bcm_arad_field_qual_info deceleration
 *  table in field_hwconfig.c. Please refer to comments of the table's deceleration
 *  for further explanations and clarifications.
 */
typedef struct _bcm_arad_field_device_qual_info_layer_s {
    int32 bcmQual;            /* BCM layer qualifier to define */
    int32 bits;               /* number of bits at BCM layer */
    int32 lshift;             /* left shift BCM LSb to align to PPD LSb */
    int32 flags;              /* see _BCM_DPP_QUAL_* in dpp/field_int.h */
    /* remaining elements are all according to the configuration */
    /* the number of chain members per map is in the device descriptor */
    /* the number of maps is in the device descriptor */
    /* how the maps align to stages,types is in the mappings descriptor */
    /* i_ for ingress PMF, f_ for ingress FLP, e_ for egress PMF, s_ for SLB */

    /* INGRESS stage */
    int32 i_l2_a;
    int32 i_l2_b;   /* chain 0,1 of L2 */
    int32 i_ipv4_a;
    int32 i_ipv4_b; /* chain 0,1 of IPV4 */
    int32 i_ipv6_a;
    int32 i_ipv6_b; /* chain 0,1 of IPV6 */
    int32 i_mpls_a;
    int32 i_mpls_b; /* chain 0,1 of MPLS */
    int32 i_psel_a;
    int32 i_psel_b; /* chain 0,1 of PRESEL */

    /* FLP stage */
    int32 f_l2_a;
    int32 f_l2_b;   /* chain 0,1 of L2 */
    int32 f_ipv4_a;
    int32 f_ipv4_b; /* chain 0,1 of IPV4 */
    int32 f_ipv6_a;
    int32 f_ipv6_b; /* chain 0,1 of IPV6 */
    int32 f_mpls_a;
    int32 f_mpls_b; /* chain 0,1 of MPLS */
    int32 f_psel_a;
    int32 f_psel_b; /* chain 0,1 of PRESEL */

    /* SLB stage */
    int32 s_l2_a;
    int32 s_l2_b;   /* chain 0,1 of L2 */
    int32 s_ipv4_a;
    int32 s_ipv4_b; /* chain 0,1 of IPV4 */
    int32 s_ipv6_a;
    int32 s_ipv6_b; /* chain 0,1 of IPV6 */
    int32 s_mpls_a;
    int32 s_mpls_b; /* chain 0,1 of MPLS */
    int32 s_psel_a;
    int32 s_psel_b; /* chain 0,1 of PRESEL */

    /* EGRESS stage */
    int32 e_l2_a;
    int32 e_l2_b;   /* chain 0,1 of L2 */
    int32 e_ipv4_a;
    int32 e_ipv4_b; /* chain 0,1 of IPV4 */
    int32 e_ipv6_a;
    int32 e_ipv6_b; /* chain 0,1 of IPV6 */
    int32 e_mpls_a;
    int32 e_mpls_b; /* chain 0,1 of MPLS  */
    int32 e_psel_a;
    int32 e_psel_b; /* chain 0,1 of PRESEL */
} _bcm_arad_field_device_qual_info_layer_t;


extern _bcm_arad_field_device_qual_info_layer_t _bcm_arad_field_qual_info[];

/*
 *  Like the qualifier mapping table, the action mapping table lists each of
 *  the supported BCM layer actions and their implied PPD layer actions.  It is
 *  laid out in a similar way, except that it does not include bit information,
 *  and since the actions are common to all types in a stage, there is only one
 *  row per stage rather than one row per mapping type.
 *
 *  Unlike the qualifier mapping table, which is considered absolute by the
 *  code, both in terms of bit width of qualifiers and bit locations, and in
 *  terms of the qualifier mappings, the action table is merely a hint.  It
 *  tells the code, for example, what PPD actions to include when a BCM action
 *  is included in a group's action set, but it does not force this action to
 *  be the one used.  This key difference is because in some cases, actions can
 *  have GPORT arguments and these GPORTs can imply different actions than the
 *  staged one (redirect to TRAP GPORT, for example, requires PPD layer action
 *  TRAP instead of REDIRECT).
 *
 *  Also, the bits are not included because the arguments are most often
 *  mapped, rather than occasionally mapped as the qualifers are.
 *
 *  THE STRUCT BELOW HAS NO EFFECT WHATSOEVER ON WHAT THE ACTION TABLE PARSER
 *  EXPECTS TO FIND; IT IS HERE ONLY FOR CONVENIENCE.
 *
 *  If you want to change the number of stage/type mappings, you need to adjust
 *  the appropriate _bcm_dpp_field_device_st_mapping_t table, and adjust the
 *  mappings field in the appropriate _bcm_dpp_field_device_info_t as well as
 *  adjusting this structure so it provides the correct number of spaces.  You
 *  must also adjust the qualifier table (above) so it has the correct number
 *  of mappings for each of the actions.
 *
 *  If you change the maximum action chain length, you must adjust the actChain
 *  value in the appropriate _bcm_dpp_field_device_info_t, as well as adjusting
 *  every chain here and corresponding adjustments to every chain for every
 *  qualifier in this table.
 *
 *  The struct definition below is used in _bcm_arad_field_action_info Arad mapping deceleration
 *  table in field_hwconfig.c. Please refer to comments of the table's deceleration
 *  for further explanations and clarifications.
 */
typedef struct _bcm_arad_field_device_action_info_layer_s {
    int32 bcmAction;          /* BCM layer action to define */
    /* remaining elements are all according to the configuration */
    /* the number of chain members per map is in the device descriptor */
    /* the number of maps is in the device descriptor */
    /* how the maps align to stages,types is in the mappings descriptor */
    int32 ingress_a;
    int32 ingress_b; /* chain 0,1 of stage 0 */
    int32 egress_a;
    int32 egress_b;   /* chain 0,1 of stage 1 */
    int32 ing_flp_a;
    int32 ing_flp_b;   /* chain 0,1 of stage 2 */
    int32 ing_slb_a;
    int32 ing_slb_b;   /* chain 0,1 of stage 3 */
} _bcm_arad_field_device_action_info_layer_t;

typedef struct _bcm_jericho_added_action_s {
      /*
       * BCM layer action to define
       */
    bcm_field_action_t bcmAction ;
      /* 'added_actions' is a dual index array. One dimention is the 'stage' and
       * the other is the added action.
       * This is, effecctively, a way to extend the length of the chain
       * (See _bcm_arad_field_device_action_info_layer_t) above the hard value
       * of '2' (selectively, on some entries of _bcm_arad_field_action_info).
       * This option is only available for Jericho.
       * Note that, at runtime, in bcm_petra_field_init(), it is verified that
       *   The number of entries on the first index of 'added_actions' is the
       *   same as 'num_stages' on _bcm_jericho_field_added_actions
       *   The number of entries on the first index of 'added_actions'
       *   is the same as 'num_added_actions' on _bcm_jericho_field_added_actions.
       *
       * See _bcm_dpp_field_actions_to_ppd
       */
    SOC_PPC_FP_ACTION_TYPE added_actions[SOC_PPC_NOF_FP_DATABASE_STAGES_ARAD][3] ;
} _bcm_jericho_field_action_t ;

typedef struct _bcm_jericho_field_added_actions_s {
      /*
       * Number of stages in the 'added_actions' array below.
       */
    SOC_PPC_FP_DATABASE_STAGE num_stages ;
      /*
       * Number of stages in the 'added_actions' array below.
       */
    uint32 num_added_actions ;
      /* 'added_actions_p' is an array whose size id set, at runtime.
       *
       * See _bcm_jericho_field_added_actions and added_action_arr
       */
    _bcm_jericho_field_action_t *added_actions_p ;
} _bcm_jericho_field_added_actions_t ;


extern _bcm_arad_field_device_action_info_layer_t _bcm_arad_field_action_info[];
extern _bcm_jericho_field_added_actions_t _bcm_jericho_field_added_actions ;
extern _bcm_jericho_field_action_t _bcm_jericho_field_added_action_arr[] ;

#endif /* def BCM_ARAD_SUPPORT*/

/*
 *  Hardware descriptors
 */
#ifdef BCM_ARAD_SUPPORT
extern _bcm_dpp_field_device_info_t _bcm_arad_field_device_info;
int _bcm_arad_field_get_num_stages(void) ;
#endif /* def BCM_ARAD_SUPPORT*/

/*
 *  These locals are needed in all of the functions; just type it once.
 */
#define _DPP_FIELD_COMMON_LOCALS \
    bcm_dpp_field_info_OLD_t *unitData = NULL;          /* unit information */ \
    int                     result = BCM_E_NONE /* working result */

/*
 *  Initial checks for beginning of exposed (BCM layer) functions
 */
#define _DPP_FIELD_UNIT_CHECK(_unit, _unitData) \
    if ((0 > _unit) || (BCM_MAX_NUM_UNITS <= (_unit))) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("invalid unit"))); \
    } \
    (_unitData) = _bcm_dpp_field_unit_info[_unit]; \
    if (!(_unitData)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INIT, (_BSL_BCM_MSG("Uninitialized"))); \
    }
#define _DPP_FIELD_SET_UNIT_LOCK(_unitData,val) \
    sw_state_sync_db[(_unitData)->unit].dpp.unitLock=val
#define _DPP_FIELD_GET_UNIT_LOCK(_unitData) \
    sw_state_sync_db[(_unitData)->unit].dpp.unitLock
#define _DPP_FIELD_UNIT_LOCK(_unitData) \
    if (sal_mutex_take(sw_state_sync_db[(_unitData)->unit].dpp.unitLock, sal_mutex_FOREVER)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unable to take lock"))); \
    }
#define _DPP_FIELD_UNIT_UNLOCK(_unitData) \
    if (sal_mutex_give(sw_state_sync_db[(_unitData)->unit].dpp.unitLock)) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("unable to release lock"))); \
    }
#define DPP_FIELD_UNIT_VALID_CHECK \
    if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT, (_BSL_BCM_MSG("invalid unit"))); \
    }

#if _BCM_DPP_FIELD_BIT_NUMBER_AS_DUNE
#define _BCM_DPP_BIT_NUMBER(_bitnum) (_bitnum)
#else /* _BCM_DPP_FIELD_BIT_NUMBER_AS_DUNE */
#define _BCM_DPP_BIT_NUM_BIT_PART(_bitnum) ((_bitnum) & 0x7)
#define _BCM_DPP_BIT_NUM_BYTE_PART(_bitnum) ((_bitnum) & (~(0x7)))
#define _BCM_DPP_BIT_NUMBER(_bitnum) \
    (_BCM_DPP_BIT_NUM_BYTE_PART(_bitnum) | \
     (7 - _BCM_DPP_BIT_NUM_BIT_PART(_bitnum)))
#endif /* _BCM_DPP_FIELD_BIT_NUMBER_AS_DUNE */

typedef uint32  _bcm_dpp_core_ports_bitmap_t[SOC_TMC_NOF_FAP_PORTS_PER_CORE];

int
bcm_dpp_field_get_dev_info(int unit, const _bcm_dpp_field_device_info_t **devInfo);
/*
 *   Function
 *      _bcm_dpp_field_entry_common_pointer
 *   Purpose
 *      Get pointer to an entry's common information
 *   Parameters
 *      (in) unitData = pointer to unit information
 *      (in) entry = entry ID
 *      (out) entryType = where to put entry type
 *      (out) entryType = where to put entry handle (enabling
 *                        access into data base depending on entry type)
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      The common information is the part of the entry descriptor that is
 *      common to all entry types, _bcm_dpp_field_entry_common_t.
 */
extern int
_bcm_dpp_field_entry_common_pointer(bcm_dpp_field_info_OLD_t *unitData,
                                    bcm_field_entry_t entry,
                                    _bcm_dpp_field_entry_type_t *entryType,
                                    int32 *entryCommonHandle) ;

extern int
_bcm_dpp_field_qualify_fwd_decision_to_hw_destination_convert(int unit,
                                                              SOC_PPC_FRWRD_DECISION_INFO   *fwdDecision,
                                                              uint8 is_for_destination,
                                                              uint8 is_for_action,
                                                              uint64 *edata_dest,
                                                              uint64 *emask_dest);

extern int
_bcm_dpp_field_entry_qualify_uint32_get(int unit,
                                        bcm_field_entry_t entry,
                                        bcm_field_qualify_t type,
                                        uint32 *data,
                                        uint32 *mask);

/*
 *  Function
 *     _bcm_dpp_field_group_hardware_install
 *  Purpose
 *     Insert a Soc_petra PPD group based upon the BCM field group information
 *  Parameters
 *     (in) unitData = unit information
 *     (in) group = group ID
 *     (in) newGroupData = proposed group data for creation of hardware DB
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Assumes group validity checks (such as qualifier width and action count)
 *     have already been performed.
 *
 *     Will only work if group exists and both qset and aset have at least one
 *     nonzero bit in them (at least one qualifier and one action).
 *
 *     Will not work if the group has entries.
 *
 *     Will destroy and recreate the hardware group if it already exists.
 */
extern int
_bcm_dpp_field_group_hardware_install(bcm_dpp_field_info_OLD_t *unitData,
                                      _bcm_dpp_field_grp_idx_t group,
                                      _bcm_dpp_field_group_t *newGroupData);

/*
 *  Function
 *    _bcm_dpp_field_entry_exists
 *  Purpose
 *    Make sure an entry exists before doing something to/with it
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
 *  Returns
 *    bcm_error_t cast as int
 *      BCM_E_NONE if entry exists
 *      BCM_E_NOT_FOUND if entry does not exist or bad entry ID
 *      BCM_E_* otherwise as appropriate
 */
extern int
_bcm_dpp_field_entry_exists(bcm_dpp_field_info_OLD_t *unitData,
                            bcm_field_entry_t entry);

/*
 *  Function
 *    _bcm_dpp_field_entry_qualifier_get_single
 *  Purpose
 *    Get a single qualifier on an entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
 *    (in) hwType = PPD layer qualifier to add to the entry
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
extern int
_bcm_dpp_field_entry_qualifier_get_single(bcm_dpp_field_info_OLD_t *unitData,
                                          bcm_field_entry_t entry,
                                          SOC_PPC_FP_QUAL_TYPE hwType,
                                          uint64 *data,
                                          uint64 *mask);

/*
 *  Function
 *     _bcm_dpp_field_entry_qualifier_get
 *  Purpose
 *     Get a qualifier from an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) qual = BCM qualifier type
 *     (in) count = number of elements in qualifier data/mask arrays
 *     (out) data = where to put qualifier data
 *     (out) mask = where to put qualifier mask
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Caller must already hold the unit lock.
 *
 *     Considers it BCM_E_CONFIG to manipulate qualifiers that are not in the
 *     QSET for the entry's group.
 *
 *     Performs similar shifting and concatenation to the set function,
 *     shifting the first PPD qualifier right so its appropriate bit aligns to
 *     the LSb of the BCM qualifier, and then concatenating further PPD
 *     qualifiers to the left until it has fully build any complex qualifier.
 *
 *     If the array is not large enough, any more significant bits (leftward)
 *     that do not fit in the provided array will not be returned.  This will
 *     not change the actual value stored, but may omit bits from the read.  It
 *     does not return an error for this condition, but it does emit a warning.
 *     It will also emit a warning if there are too many buffer elements.
 *
 *     The array is little-endian with 64b grains: least significant octbyte is
 *     array[0], then next more significant is array[1], and so on.  Within
 *     array elements, the data are in machine-native order.
 *
 *     Will overwrite caller provided buffer, possibly only partially, even in
 *     certain error situations.
 */
extern int
_bcm_dpp_field_entry_qualifier_get(bcm_dpp_field_info_OLD_t *unitData,
                                   bcm_field_entry_t entry,
                                   bcm_field_qualify_t type,
                                   int count,
                                   uint64 *data,
                                   uint64 *mask);

/*
 *   Function
 *      _bcm_dpp_proc_cntr_from_stat
 *   Purpose
 *      decode stat id into proc and ctr
 *   Parameters
 *      (in) int            unit = the unit
 *      (in)int             stat
 *      (out) unsigned int   proc = the processor
 *      (out) unsigned int   cntr = the cntr index
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_proc_cntr_from_stat(int unit,
                             int stat,
                             unsigned int *proc,
                             unsigned int *cntr);


/******************************************************************************
 *
 *  Exports from submodules (see submodules for code comments)
 */

/* -- field_utils.c -- */

/*
 *   Function
 *      _bcm_dpp_ppd_qual_bits
 *   Purpose
 *      Figure out how many bits in a PPD layer qualifier
 *   Parameters
 *      (in) unitData = unit information
 *      (in) stage = stage for the group
 *      (in) ppdQual = PPD layer qualifier
 *      (out) exposed = API exposed bits
 *      (out) hardwareBestCase = bits taken by the qualifier in hardware in the best case
 *      (out) hardwareWorstCase = bits taken by the qualifier in hardware in the worst case
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if valid
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
extern int
_bcm_dpp_ppd_qual_bits(int  unit,
                       _bcm_dpp_field_stage_idx_t stage,
                       SOC_PPC_FP_QUAL_TYPE ppdQual,
                       unsigned int *exposed,
                       unsigned int *hardwareBestCase,
                       unsigned int *hardwareWorstCase);

/*
 *   Function
 *      _bcm_dpp_ppd_action_bits
 *   Purpose
 *      Figure out how many bits in a PPD layer action
 *   Parameters
 *      (in) unitData = unit information
 *      (in) stage = stage for the action
 *      (in) ppdAct = PPD layer action
 *      (out) bits = action bits
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if valid
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
extern int
_bcm_dpp_ppd_action_bits(int unit,
                         _bcm_dpp_field_stage_idx_t stage,
                         SOC_PPC_FP_ACTION_TYPE ppdAct,
                         unsigned int *bits);

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
                         const char *prefix);
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
                         const char *prefix);
/*
 *  Function
 *    _bcm_dpp_field_hfset_dump
 *  Purpose
 *    Dump action list from an header format set
 *  Parameters
 *    (in) bcm_field_header_format_set_t hfset = the hfset to dump
 *    (in) char *prefix = line prefix
 *  Returns
 *    nothing
 *  Notes
 *    No error checking or locking is done here.
 */
extern void
_bcm_dpp_field_hfset_dump(const bcm_field_header_format_set_t hfset,
                          const char *prefix);
#endif /* def BROADCOM_DEBUG */


/* Conversion from PPD Trap id to HW trap id */
extern int
_bcm_dpp_field_trap_ppd_to_hw(int unit,
                              int trap_id,
                              uint32 *hwTrapCode);

/* Conversion from PPD Trap id from HW trap id */
extern int
_bcm_dpp_field_trap_ppd_from_hw(int unit,
                                uint32 hwTrapCode,
                                SOC_PPC_TRAP_CODE *ppdTrapCode);

/* Convert from gport of type LIF to global LIF-Id */
extern int
_bcm_dpp_field_gport_to_global_lif_convert(int unit,
                                           bcm_gport_t  lif_gport,
                                           uint8        is_global_inlif,
                                           uint32      *global_lif);

/* Convert from gport of type LIF to local LIF-Id */
extern int
_bcm_dpp_field_gport_to_local_lif_convert(int unit,
                                          bcm_gport_t  lif_gport,
                                          uint8        is_local_inlif,
                                          uint32       *local_lif);

/*
 *  Function
 *    _bcm_dpp_field_HeaderFormatExtension_bcm_to_ppd
 *  Purpose
 *    Translate BCM HeaderFormatExtension (PLC - Parser Leaf Context) to correspondig low level (PPD) value.
 *  Parameters
 *    (in) header_format_extension = BCM-level header format extension (PLC)
 *    (out) pfc_sw_p       = PPD level sw value which is the equivalent of 'header_format_extension'
 *  Returns
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 */
int
_bcm_dpp_field_HeaderFormatExtension_bcm_to_ppd(bcm_field_header_format_extension_t header_format_extension,
                                       DPP_PLC_E *plc_sw_p);

/*
 *  Function
 *    _bcm_dpp_field_HeaderFormatExtension_ppd_to_bcm
 *  Purpose
 *    Translate PPD-level PLC (DPP_PLC_E) to BCM-level HeaderFormatExtension
 *  Parameters
 *    (in) plc_sw = PPD-level header format extension (PLC)
 *    (out) header_format_extension_p = BCM-level header format extension
 *  Returns
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 */
int
_bcm_dpp_field_HeaderFormatExtension_ppd_to_bcm(DPP_PLC_E plc_sw,
                                       bcm_field_header_format_extension_t *header_format_extension_p);

/*
 *  Function
 *    _bcm_dpp_field_HeaderFormat_bcm_to_ppd
 *  Purpose
 *    Translate BCM HeaderFormat to correspondig PFC HW parameters (value and mask).
 *    When these values are loaded into HW, BCM HeaderFormat is implemented.
 *  Parameters
 *    (in) header_format = BCM header format
 *    (out) pfc_sw_p       = PPD level sw value which is the equivalent of 'header_format'
 *    (out) pfc_pmf_p      = PFC (headerFormat) value to load into HW to implement 'header_format'
 *    (out) pfc_pmf_mask_p = PFC (headerFormat) mask to load into HW to implement 'header_format'
 *  Returns
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 */
extern int
_bcm_dpp_field_HeaderFormat_bcm_to_ppd(int unit,
                                       bcm_field_header_format_t header_format,
                                       DPP_PFC_E *pfc_sw_p,
                                       uint32 *pfc_pmf_p,
                                       uint32 *pfc_pmf_mask_p);

/*
 *  Function
 *    _bcm_dpp_field_HeaderFormat_ppd_to_bcm
 *  Purpose
 *    Translate PPD PKT_HDR_STK_TYPE to BCM HeaderFormat
 *  Parameters
 *    (in) hdr_stk_type = PPD header stack type
 *    (out) header_format = BCM header format
 *  Returns
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE for success
 *      BCM_E_* otherwise as appropriate
 */
extern int
_bcm_dpp_field_HeaderFormat_ppd_to_bcm(int unit,
                                       uint32 pfc_pmf,
                                       uint32 pfc_pmf_mask,
                                       bcm_field_header_format_t *header_format);

/*
 *  Function
 *    _bcm_dpp_field_color_bcm_to_ppd
 *  Purpose
 *    Translate a BCM frame color to PPD frame color
 *  Arguments
 *    (in) bcmCol = BCM frame color
 *    (out) ppdCol = where to put the PPD color
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_color_bcm_to_ppd(int bcmCol,
                                uint32 *ppdCol);

/*
 *  Function
 *    _bcm_dpp_field_color_bcm_to_ppd
 *  Purpose
 *    Translate a PPD frame color to BCM frame color
 *  Arguments
 *    (in) ppdCol = PPD frame color
 *    (out) bcmCol = where to put the BCM color
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_color_ppd_to_bcm(uint32 ppdCol,
                                int *bcmCol);

extern int
_bcm_dpp_field_arp_opcode_bcm_to_ppd(bcm_field_ArpOpcode_t bcmArpOpcode,
                                uint32 *ppdArpOpcode);
extern int
_bcm_dpp_field_arp_opcode_ppd_to_bcm(bcm_field_ArpOpcode_t ppdArpOpcode,
                                bcm_field_ArpOpcode_t *bcmArpOpcode);

/*
 *  Function
 *    _bcm_dpp_field_stp_state_bcm_to_ppd
 *  Purpose
 *    Translate a BCM STP state to PPD STP state
 *  Arguments
 *    (in) bcmStp = BCM STP state
 *    (out) ppdStp = where to put the PPD STP state
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_stp_state_bcm_to_ppd(bcm_stg_stp_t bcmStp,
                                    SOC_PPC_PORT_STP_STATE_FLD_VAL *ppdStp);

/*
 *  Function
 *    _bcm_dpp_field_stp_state_ppd_to_bcm
 *  Purpose
 *    Translate a BCM STP state from PPD STP state
 *  Arguments
 *    (in) ppdStp = PPD STP state
 *    (out) bcmStp = where to put the BCM STP state
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_stp_state_ppd_to_bcm(SOC_PPC_PORT_STP_STATE_FLD_VAL ppdStp,
                                    bcm_stg_stp_t *bcmStp);

/*
 *  Function
 *    _bcm_dpp_field_ip_type_bcm_to_ppd
 *  Purpose
 *    Translate a BCM IP Type to PPD IP Type
 *  Arguments
 *    (in) bcmIpType = BCM IP Type
 *    (out) ppdIpType = where to put the PPD IP Type
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_ip_type_bcm_to_ppd(bcm_field_IpType_t bcmIpType,
                                  uint8 is_egress,
                                    SOC_PPC_FP_PARSED_ETHERTYPE *ppdIpType);

extern int
_bcm_dpp_field_ip_type_ppd_to_bcm(SOC_PPC_FP_PARSED_ETHERTYPE ppdIpType,
                                  uint8 is_egress,
                                    bcm_field_IpType_t *bcmIpType);

extern int
_bcm_dpp_field_ip_next_protocol_bcm_to_ppd(bcm_field_IpProtocolCommon_t bcmIpNextProtocol,
                                    SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL *ppdIpNextProtocol, uint64 *emask);

extern int
_bcm_dpp_field_ip_next_protocol_ppd_to_bcm(SOC_PPC_FP_PARSED_IP_NEXT_PROTOCOL ppdIpNextProtocol,
                                    bcm_field_IpProtocolCommon_t *bcmIpNextProtocol, int32 emask);

extern int
_bcm_dpp_field_termination_type_bcm_to_ppd(bcm_field_TunnelType_t bcmTerminationType,
                                           SOC_PPC_PKT_TERM_TYPE *ppdTerminationType);

extern int
_bcm_dpp_field_termination_type_ppd_to_bcm(SOC_PPC_PKT_TERM_TYPE ppdTerminationType,
                                           bcm_field_TunnelType_t *bcmTerminationType);

extern int
_bcm_dpp_field_l2_eth_format_ppd_to_bcm(SOC_PPC_FP_ETH_ENCAPSULATION ppdL2Format,
                                        bcm_field_L2Format_t *bcmL2Format);

extern int
_bcm_dpp_field_l2_eth_format_bcm_to_ppd(bcm_field_L2Format_t bcmL2Format,
                                    SOC_PPC_FP_ETH_ENCAPSULATION *ppdL2Format);

extern int
_bcm_dpp_field_forwarding_type_ppd_to_bcm(int unit,
                                          SOC_TMC_PKT_FRWRD_TYPE ppdForwardingType,
                                          uint32 ppdMask,
                                          bcm_field_ForwardingType_t *bcmForwardingType);

extern int
_bcm_dpp_field_forwarding_type_bcm_to_ppd(bcm_field_ForwardingType_t bcmForwardingType,
                                          SOC_TMC_PKT_FRWRD_TYPE *ppdForwardingType,
                                          uint32 *ppdMask);

extern int
_bcm_dpp_field_app_type_bcm_to_ppd(int unit,
                                   bcm_field_AppType_t bcmAppType,
                                   uint32 *ppd_flp_program);
extern int
_bcm_dpp_field_app_type_ppd_to_bcm(int unit,
                                          uint32 ppd_flp_program,
                                          bcm_field_AppType_t *bcmAppType);

extern int
_bcm_dpp_field_offset_ext_ppd_to_bcm(uint32 ppdOffsetExtData, uint32 ppdOffsetExtMask,
                                          bcm_field_ForwardingType_t *bcmForwardingType);

extern int
_bcm_dpp_field_offset_ext_bcm_to_ppd(bcm_field_ForwardingType_t bcmForwardingType,
                                        uint32 *ppdOffsetExtData, uint32 *ppdOffsetExtMask);

extern int
_bcm_dpp_field_vlan_format_bcm_to_ppd(uint8 bcmVlanFormatBitmap,
                                      SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT *ppdVlanFormat);

extern int
_bcm_dpp_field_vlan_format_ppd_to_bcm(SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT ppdVlanFormat,
                                          uint8 *bcmVlanFormatBitmap);

extern int
_bcm_dpp_field_base_header_bcm_to_ppd(bcm_field_data_offset_base_t bcm_base_header,
                                      uint32 *ppd_base_header);

extern int
_bcm_dpp_field_base_header_ppd_to_bcm(uint32 ppd_base_header,
                                      bcm_field_data_offset_base_t *bcm_base_header);

/*
 *  Function
 *    _bcm_dpp_field_ip_frag_bcm_to_ppd
 *  Purpose
 *    Translate a BCM IP Frag to PPD IP Frag
 *  Arguments
 *    (in) bcmFragInfo = BCM IP Frag
 *    (out) ppdIpFragmented = if fragmented or not
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_ip_frag_bcm_to_ppd(int unit, bcm_field_IpFrag_t bcmFragInfo,
                                    uint8 *ppdIpFragmented);

/*
 *  Function
 *    _bcm_dpp_field_ip_frag_ppd_to_bcm
 *  Purpose
 *    Translate a BCM IP Frag from PPD IP Frag
 *  Arguments
 *    (in) ppdIpFragmented = if fragmented or not
 *    (out) bcmFragInfo = BCM IP Frag
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_ip_frag_ppd_to_bcm(int unit, uint8 ppdIpFragmented,
                                    bcm_field_IpFrag_t *bcmFragInfo);

/* Convert from Gport to PP-port and TM-Port */
extern int
_bcm_dpp_field_gport_to_pp_port_tm_port_convert(int unit,
                                                bcm_port_t data,
                                                uint32 *pp_port,
                                                uint32 *tm_port,
                                                int *core);
int
_bcm_dpp_field_group_id_to_name(int unit, 
                                    _bcm_dpp_field_grp_idx_t group,
                                    char *group_name, 
                                    int max_length);

int
_bcm_dpp_field_group_hw_handle_to_name(int unit, 
                                    uint32 hw_handle,
                                    char *group_name, 
                                    int max_length);

int
_bcm_dpp_field_dump_single_qual_name(int unit,
                                     int qual_id,
                                     char *qual_name,
                                     int max_length);
int
_bcm_dpp_field_dump_single_presel_name(int unit,
                                       bcm_field_presel_t presel_id,
                                       char *presel_name,
                                       int max_length);

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_entry_qual_dump
 *   Purpose
 *      Dump the state of a single qualifier for a field entry
 *   Parameters
 *      (in) unitData = unit information
 *      (in) stage = stage where entry exists
 *      (in) qualData = pointer to the qualifier data for the entry
 *      (in) qualHwData = pointer to the hardware qualifier data for the entry
 *      (in) index = which qualifier to dump
 *      (in) dumpRanges = TRUE if L4 port range qualifier is to be dumped
 *      (in) prefix = string to prefix to each line
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
int
_bcm_dpp_field_entry_qual_dump(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_stage_idx_t stage,
                               const _bcm_dpp_field_qual_t *qualData,
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                               const SOC_PPC_FP_QUAL_VAL *qualHwData,
#endif
                               unsigned int index,
                               int dumpRanges,
                               const char *prefix);
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_group_dump
 *   Purpose
 *      Dump the state of a specific field group
 *   Parameters
 *      (in) unitData = unit information
 *      (in) group = group to be dumped
 *      (in) prefix = string to prefix to each line
 *      (in) entries = TRUE to include entries, FALSE to exclude entries
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
extern int
_bcm_dpp_field_group_dump(bcm_dpp_field_info_OLD_t *unitData,
                          _bcm_dpp_field_grp_idx_t group,
                          const char *prefix,
                          int entries);
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_unit_dump
 *   Purpose
 *      Dump the state of the unit
 *   Parameters
 *      (in) unitData = unit information
 *      (in) prefix = string to prefix to each line
 *      (in) stages = TRUE to include stages, FALSE to exclude stages
 *      (in) groups = TRUE to include groups, FALSE to exclude groups
 *      (in) entries = TRUE to include entries, FALSE to exclude entries
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      If groups is FALSE, the value of entries does not matter: entries will
 *      not be displayed if groups are not displayed.
 *
 *      If stages is FALSE, the values of groups and entries do not matter;
 *      neither groups nor entries will be displayed if stages are not
 *      displayed.
 */
extern int
_bcm_dpp_field_unit_dump(bcm_dpp_field_info_OLD_t *unitData,
                         const char *prefix,
                         int stages,
                         int groups,
                         int entries);
#endif /* def BROADCOM_DEBUG */

/*
 *   Function
 *      _bcm_dpp_compare_entry_priority
 *   Purpose
 *      Compare two entry priorities.  This returns -1 if the first is less
 *      than the second, 0 if the first is equal to the second (or if either
 *      priority is not valid), 1 if the first is greater than the second.
 *   Parameters
 *      (in) int pri1 = first priority to compare
 *      (in) int pri2 = second priority to compare
 *   Returns
 *      signed int = 0 if priorities are equal
 *                   >0 if pri1 is higher priority than pri2
 *                   <0 if pri1 is lower priority than pri2
 *   Notes
 *      Valid priorities are: 0..MAXINT.
 *
 *      The actual mapping of the priorities is (from highest priority to
 *      lowest priority): MAXINT down to 0.
 *
 *      If a priority is invalid and the other is valid, the valid one is
 *      considered greater; if they're both invalid, they're considered equal.
 *
 *      Unhappily, this gets called a lot and it involves a lot of branches and
 *      obligatorily sequential decisions as written. It'd be better if this
 *      could be written in assembly, using inline, or made somehow otherwise
 *      more optimal; maybe later?  It does early returns and boolean
 *      short-circuiting where reasonable; this should help some.
 */
extern signed int
_bcm_dpp_compare_entry_priority(int pri1,
                                int pri2);

/*
 *   Function
 *      _bcm_dpp_field_qset_subset
 *   Purpose
 *      Check whether qset2 is a subset of qset1.
 *   Parameters
 *      (in) bcm_field_qset_t qset1 = qualifier set 1
 *      (in) bcm_field_qset_t qset2 = qualifier set 2
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if qset2 is subset of qset1
 *                    BCM_E_FAIL if qset2 is not subset of qset1
 *   Notes
 *      This checks whether qset2 is a subset, either proper and improper, of
 *      qset1, and returns BCM_E_NONE if qset2 is a subset of qset1, but
 *      returns BCM_E_FAIL if qset2 is not a subset of qset1.  Other errors may
 *      be returned under appropriate conditions.
 */
extern int
_bcm_dpp_field_qset_subset(bcm_field_qset_t qset1,
                           bcm_field_qset_t qset2);

/*
 *   Function
 *      _bcm_dpp_field_qset_union
 *   Purpose
 *      Build union of two QSETs
 *   Parameters
 *      (in) qset0 = qualifier set 0
 *      (in) qset1 = qualifier set 1
 *      (out) qset2 = where to put union of qset0 and qset1
 *   Returns
 *      (none)
 *   Notes
 */
extern void
_bcm_dpp_field_qset_union(const bcm_field_qset_t *qset0,
                          const bcm_field_qset_t *qset1,
                          bcm_field_qset_t *qset2);

/*
 *   Function
 *     _bcm_dpp_field_qset_subset_count
 *   Purpose
 *     Determine the number of qualifiers in each of two qsets, and in the
 *     intersection of those two qsets
 *   Parameters
 *     (in) bcm_field_qset_t qset0 = base qset (proposed subset)
 *     (in) bcm_field_qset_t qset1 = one of the qsets to check
 *     (out) unsigned int *qual0 = where to put qset0 qualifier count
 *     (out) unsigned int *qual1 = where to put qset1 qualifier count
 *     (out) unsigned int *qual2 = where to put intersection qualifier count
 *   Returns
 *      (none)
 *   Notes
 */
extern void
_bcm_dpp_field_qset_subset_count(const bcm_field_qset_t *qset0,
                                 const bcm_field_qset_t *qset1,
                                 unsigned int *qual0,
                                 unsigned int *qual1,
                                 unsigned int *qualBoth);

/*
 *   Function
 *      _bcm_dpp_field_aset_subset
 *   Purpose
 *      Check whether aset2 is a subset of aset1.
 *   Parameters
 *      (in) bcm_field_qset_t aset1 = action set 1
 *      (in) bcm_field_qset_t aset2 = action set 2
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if aset2 is subset of aset1
 *                    BCM_E_FAIL if aset2 is not subset of aset1
 *   Notes
 *      This checks whether aset2 is a subset, either proper and improper, of
 *      aset1, and returns BCM_E_NONE if aset2 is a subset of aset1, but
 *      returns BCM_E_FAIL if aset2 is not a subset of aset1.  Other errors may
 *      be returned under appropriate conditions.
 */
extern int
_bcm_dpp_field_aset_subset(bcm_field_aset_t *aset1,
                           bcm_field_aset_t *aset2);

/*
 *   Function
 *      _bcm_dpp_field_aset_union
 *   Purpose
 *      Build union of two ASETs
 *   Parameters
 *      (in) aset0 = action set 0
 *      (in) aset1 = action set 1
 *      (out) aset2 = where to put union of aset0 and aset1
 *   Returns
 *      (none)
 *   Notes
 */
extern void
_bcm_dpp_field_aset_union(bcm_field_aset_t *aset0,
                          bcm_field_aset_t *aset1,
                          bcm_field_aset_t *aset2);

/*
 *   Function
 *      _bcm_dpp_ppd_act_from_bcm_act
 *   Purpose
 *      Translate a BCM layer action to a PPD layer action
 *   Parameters
 *      (in) unitData = unit information
 *      (in) stage = stage for the group
 *      (in) bcmAct = BCM layer action
 *      (out) ppdAct = where to put PPD layer action
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if success
 *                    BCM_E_CONFIG if not supported for the group config
 *                    BCM_E_FAIL if mapping is special
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      Does not require lock to be held.
 *
 *      It could be done with a (very) large table instead.
 *
 *      Some qualifiers are supported in some places but not others.
 *
 *      Does not deal with qualifiers used only as group flags.
 *
 *      Would look simpler and run faster as a table, except that the table has
 *      at least three dimensions and is quite sparse, so is probably more
 *      efficient in terms of overall size to have it in this form (not to
 *      mention the trouble of initializing such a table).
 */
extern int
_bcm_dpp_ppd_act_from_bcm_act(bcm_dpp_field_info_OLD_t *unitData,
                              _bcm_dpp_field_stage_idx_t stage,
                              bcm_field_action_t bcmAct,
                              SOC_PPC_FP_ACTION_TYPE **ppdAct);

/*
 *   Function
 *      _bcm_dpp_ppd_qual_from_bcm_qual
 *   Purpose
 *      Translate a BCM layer qualifier to a PPD layer qualifier
 *   Parameters
 *      (in) unitData = unit information
 *      (in) stage = stage for the group
 *      (in) types = bitmap of types to check for this qualifier
 *      (in) bcmQual = BCM layer qualifier
 *      (out) ppdQual = where to put PPD layer qualifier
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if success
 *                    BCM_E_CONFIG if not supported for the group config
 *                    BCM_E_FAIL if mapping is special
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      Does not require lock to be held.
 *
 *      It could be done with a (very) large table instead.
 *
 *      Some qualifiers are supported in some places but not others.
 *
 *      Does not deal with qualifiers used only as group flags.
 *
 *      Would look simpler and run faster as a table, except that the table has
 *      at least three dimensions and is quite sparse, so is probably more
 *      efficient in terms of overall size to have it in this form (not to
 *      mention the trouble of initializing such a table).
 *
 *      Picks the first of the allowed types that includes the qualifier and
 *      uses that mapping.  To just allow one type, only set one bit.
 */
extern int
_bcm_dpp_ppd_qual_from_bcm_qual(bcm_dpp_field_info_OLD_t *unitData,
                                _bcm_dpp_field_stage_idx_t stage,
                                uint32 types,
                                bcm_field_qualify_t bcmQual,
                                SOC_PPC_FP_QUAL_TYPE **ppdQual);

/*
 *   Function
 *      _bcm_dpp_ppd_stage_from_bcm_stage
 *   Purpose
 *      Translate a BCM layer stage to a PPD layer stage
 *   Parameters
 *      (in) unitData = unit information
 *      (in) bcmStage = BCM layer stage
 *      (out) hwStageId = where to put PPD layer stage
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if success
 *                    BCM_E_PARAM if BCM stage is not supported
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      Does not require lock to be held.
 */
extern int
_bcm_dpp_ppd_stage_from_bcm_stage(bcm_dpp_field_info_OLD_t *unitData,
                                  bcm_field_stage_t bcmStage,
                                  SOC_PPC_FP_DATABASE_STAGE *hwStageId);

/*
 *   Function
 *      _bcm_dpp_bcm_stage_from_ppd_stage
 *   Purpose
 *      Translate a PPD layer layer stage to a BCM stage
 *   Parameters
 *      (in) unitData = unit information
 *      (in) hwStageId = PPD layer stage
 *      (out) bcmStage = where to put BCM layer stage
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if success
 *                    BCM_E_PARAM if PPD stage is not supported
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      Does not require lock to be held.
 */
extern int
_bcm_dpp_bcm_stage_from_ppd_stage(bcm_dpp_field_info_OLD_t *unitData,
                                  SOC_PPC_FP_DATABASE_STAGE hwStageId,
                                  bcm_field_stage_t *bcmStage);

/*
 *  Function
 *     _bcm_dpp_field_group_priority_recalc
 *  Purpose
 *     Recompute hardware priorities for entries in a group after the
 *     addition of a new entry, the destruction of an old entry, or any change
 *     in priority of entries (which would imply the entries are rearranged).
 *  Parameters
 *    (in) unitData = unit information
 *    (in) group = group ID
 *  Returns
 *     nothing
 *  Notes
 *     Assumes group and entry validity checking has been performed.
 *
 *     Will change hardware priority of all entries in a group, marking them
 *     all as changed.
 *
 *     Will not make any changes to hardware.
 *
 *     Assumes priority at PPD layer is only required to be unique per group,
 *     rather than across the entire system.
 *
 *     Assumes priority range of 0..16K-1 for TCAM groups.
 *     Assumes priority range of 0..7 for direct extraction groups.
 */
extern void
_bcm_dpp_field_group_priority_recalc(bcm_dpp_field_info_OLD_t *unitData,
                                     _bcm_dpp_field_grp_idx_t group);

/*
 *   Function
 *      _bcm_dpp_field_stage_type_qset_aset_get
 *   Purpose
 *      Get the qualifier set and action set supported by a particular
 *      type of group within a specified stage
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) _bcm_dpp_field_stage_idx_t stage = which stage
 *      (in) _bcm_dpp_field_type_idx_t type = which type
 *      (out) bcm_field_qset_t *qset = where to put the qualifier set
 *      (out) bcm_field_aset_t *aset = where to put the action set
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Not for use internally; expected to be used by functions outside of the
 *      field API implementation to access specifics of what is supported for a
 *      particular stage and type.
 */
extern int
_bcm_dpp_field_stage_type_qset_aset_get(int unit,
                                        _bcm_dpp_field_stage_idx_t stage,
                                        _bcm_dpp_field_type_idx_t type,
                                        bcm_field_qset_t *qset,
                                        bcm_field_aset_t *aset);

/*
 *  Function
 *    _bcm_dpp_field_group_qset_recover
 *  Purpose
 *    Build the BCM layer QSET from the PPD layer QSET
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN stage = stage number
 *    IN types = group types bitmap
 *    IN ppdQset = pointer to PPD qualifier set
 *    OUT bcmQset = pointer to BCM qualifier set
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    ppdQset should be const but the compiler gripes
 */
extern int
_bcm_dpp_field_group_qset_recover(bcm_dpp_field_info_OLD_t *unitData,
                                  _bcm_dpp_field_stage_idx_t stage,
                                  uint32 types,
                                  _bcm_dpp_field_qual_set_t *ppdQset,
                                  bcm_field_qset_t *bcmQset);

/*
 *  Function
 *    _bcm_dpp_field_group_aset_recover
 *  Purpose
 *    Build the BCM layer ASET from the PPD layer ASET
 *  Arguments
 *    IN unitData = pointer to the unit information
 *    IN stage = stage number
 *    IN ppdAset = pointer to PPD action set
 *    OUT bcmAset = pointer to BCM action set
 *  Results
 *    bcm_error_t (cast as int)
 *      BCM_E_NONE if successful
 *      BCM_E_* otherwise as appropriate
 *  Notes
 *    ppdAset should be const but the compiler gripes
 */
extern int
_bcm_dpp_field_group_aset_recover(bcm_dpp_field_info_OLD_t *unitData,
                                  _bcm_dpp_field_stage_idx_t stage,
                                  _bcm_dpp_field_action_set_t *ppdAset,
                                  bcm_field_aset_t *bcmAset);

/*
 *  Function
 *    _bcm_dpp_field_qualifier_set_single
 *  Purpose
 *    Set a single qualifier on an entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
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
 *    Considers it BCM_E_CONFIG to manipulate qualifiers that are not in the
 *    QSET for the entry's group.
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
 *    Assumes entry exists in a group.
 */
extern int
_bcm_dpp_field_qualifier_set_single(bcm_dpp_field_info_OLD_t *unitData,
                                    bcm_field_entry_t entry,
                                    SOC_PPC_FP_QUAL_TYPE hwType,
                                    const uint64 data,
                                    const uint64 mask,
                                    const uint64 valid);

/*
 *  Function
 *     _bcm_dpp_field_qualifier_set
 *  Purpose
 *     Set a qualifier to an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) bcmQual = BCM qualifier type
 *     (in) count = number of elements in qualifier data/mask arrays
 *     (in) data = pointer to array of qualifier data
 *     (in) mask = pointer to array of qualifier mask
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
 *     Considers it BCM_E_CONFIG to manipulate qualifiers that are not in the
 *     QSET for the entry's group (or not supported for Preselectors in the
 *     preselector's stage).
 *
 *     Marks the entry as changed, and the entry's group as containing changed
 *     entry/entries as appropriate.
 *
 *     Preselectors will be read/replaced during the process and so must not
 *     have any references.
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
 *
 *     Should be called by _bcm_dpp_field_entry_qualifier_general_set_int, or
 *     something below it.
 */
extern int
_bcm_dpp_field_qualifier_set(bcm_dpp_field_info_OLD_t *unitData,
                             bcm_field_entry_t entry,
                             bcm_field_qualify_t bcmQual,
                             int count,
                             const uint64 *data,
                             const uint64 *mask);

/*
 *  Function
 *     _bcm_dpp_field_entry_qualifier_general_set_int
 *  Purpose
 *     Set a qualifier for an entry
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) entry = the entry ID
 *     (in) bcmQual = the qualifier type
 *     (in) count = number of octbytes of data/mask
 *     (in) data = pointer to the data values
 *     (in) mask = pointer to the mask value
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Meant to be called with the lock held
 *
 *     Must be called with a valid qualifier type (okay even if it is not a
 *     supported one, as long as it is valid).
 *
 *     Only checks entries for validity.  Other objects (currently only
 *     preselectors) are checked by their own setup code.
 */
extern int
_bcm_dpp_field_entry_qualifier_general_set_int(bcm_dpp_field_info_OLD_t *unitData,
                                               bcm_field_entry_t entry,
                                               bcm_field_qualify_t bcmQual,
                                               int count,
                                               const uint64 *data,
                                               const uint64 *mask);

/*
 *  Function
 *    _bcm_dpp_field_qualifier_get_single
 *  Purpose
 *    Get a single qualifier on an entry
 *  Arguments
 *    (in) unitData = unit information
 *    (in) entry = entry ID
 *    (in) hwType = PPD layer qualifier to add to the entry
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
extern int
_bcm_dpp_field_qualifier_get_single(bcm_dpp_field_info_OLD_t *unitData,
                                    bcm_field_entry_t entry,
                                    SOC_PPC_FP_QUAL_TYPE hwType,
                                    uint64 *data,
                                    uint64 *mask);

/*
 *  Function
 *     _bcm_dpp_field_entry_qualifier_get
 *  Purpose
 *     Get a qualifier from an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) bcmQual = BCM qualifier type
 *     (in) count = number of elements in qualifier data/mask arrays
 *     (out) data = where to put qualifier data
 *     (out) mask = where to put qualifier mask
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Caller must already hold the unit lock.
 *
 *     Considers it BCM_E_CONFIG to manipulate qualifiers that are not in the
 *     QSET for the entry's group.
 *
 *     Performs similar shifting and concatenation to the set function,
 *     shifting the first PPD qualifier right so its appropriate bit aligns to
 *     the LSb of the BCM qualifier, and then concatenating further PPD
 *     qualifiers to the left until it has fully build any complex qualifier.
 *
 *     If the array is not large enough, any more significant bits (leftward)
 *     that do not fit in the provided array will not be returned.  This will
 *     not change the actual value stored, but may omit bits from the read.  It
 *     does not return an error for this condition, but it does emit a warning.
 *     It will also emit a warning if there are too many buffer elements.
 *
 *     The array is little-endian with 64b grains: least significant octbyte is
 *     array[0], then next more significant is array[1], and so on.  Within
 *     array elements, the data are in machine-native order.
 *
 *     Will overwrite caller provided buffer, possibly only partially, even in
 *     certain error situations.
 *
 *     Should be called by _bcm_dpp_field_entry_qualifier_general_get_int, or
 *     something below it.
 */
extern int
_bcm_dpp_field_qualifier_get(bcm_dpp_field_info_OLD_t *unitData,
                             bcm_field_entry_t entry,
                             bcm_field_qualify_t bcmQual,
                             int count,
                             uint64 *data,
                             uint64 *mask);

/*
 *  Function
 *     _bcm_dpp_field_entry_qualifier_general_get_int
 *  Purpose
 *     Get a qualifier for an entry
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) entry = the entry ID
 *     (in) bcmQual = the qualifier type
 *     (in) count = number of octbytes of data/mask
 *     (out) data = pointer to the data values
 *     (out) mask = pointer to the mask value
 *  Returns
 *     int (implied cast from bcm_error_t)
 *       BCM_E_NONE if successful
 *       BCM_E_* appropriately if not
 *  Notes
 *     Meant to be called with the lock held
 *
 *     Must be called with a valid qualifier type (okay even if it is not a
 *     supported one, as long as it is valid).
 *
 *     Only checks entries for validity.  Other objects (currently only
 *     preselectors) are checked by their own setup code.
 */
extern int
_bcm_dpp_field_entry_qualifier_general_get_int(bcm_dpp_field_info_OLD_t *unitData,
                                               bcm_field_entry_t entry,
                                               bcm_field_qualify_t bcmQual,
                                               unsigned int count,
                                               uint64 *data,
                                               uint64 *mask);

/* -- field_presel.c -- */

typedef enum _bcm_dpp_field_implied_presel_e {
    _BCM_DPP_IMPLIED_PRESEL_L2 = 0,
    _BCM_DPP_IMPLIED_PRESEL_IPV4,
    _BCM_DPP_IMPLIED_PRESEL_IPV6,
    _BCM_DPP_IMPLIED_PRESEL_MPLS,
    _BCM_DPP_IMPLIED_PRESEL_COUNT /* always last; not a valid value */
} _bcm_dpp_field_implied_presel_t;

typedef enum _bcm_dpp_field_presel_alloc_flags_e {
    _BCM_DPP_PRESEL_ALLOC_WITH_ID = 0x00000001,
    _BCM_DPP_PRESEL_ALLOC_DOWN = 0x00000002,
    _BCM_DPP_PRESEL_ALLOC_WITH_STAGE = 0x00000004,
    _BCM_DPP_PRESEL_ALLOC_SECOND_PASS = 0x00000008,
    _BCM_DPP_PRESEL_ALLOC_WITH_PROGRAM = 0x00000010 /*create preselector with program ID*/
} _bcm_dpp_field_presel_alloc_flags_t;

#define _BCM_DPP_FIELD_ENTRY_IS_LARGE_DIRECT_LOOKUP(_e) ((_e & BCM_FIELD_ENTRY_LARGE_DIRECT_LOOKUP) ? TRUE : FALSE)
#define _BCM_DPP_FIELD_ENTRY_IS_ENTRY(_e) ((0 == (_e & BCM_FIELD_QUALIFY_PRESEL))?TRUE:FALSE)
#define _BCM_DPP_FIELD_ENTRY_IS_PRESEL(_e) ((_e & BCM_FIELD_QUALIFY_PRESEL)?TRUE:FALSE)
#define _BCM_DPP_FIELD_ENTRY_IS_PRESEL_STAGGERED(_e) ((_e & 0x01000000)?TRUE:FALSE)
#define _BCM_DPP_FIELD_PRESEL_FROM_ENTRY(_e) (_e & 0x00FFFFFF)
#define _BCM_DPP_FIELD_PRESEL_ID_FROM_ENTRY(_e) (_e & 0x00000FFF)
#define _BCM_DPP_FIELD_ENTRY_IS_PRESEL_ADVANCED_MODE(_e) ((_e & 0x80000000)?TRUE:FALSE)
#define _BCM_DPP_FIELD_PRESEL_STAGE_FROM_ENTRY(_e) ( (_e & 0x8000) ? bcmFieldStageExternal : \
                                                   ( (_e & 0x4000) ? bcmFieldStageHash : \
                                                   ( (_e & 0x2000) ? bcmFieldStageEgress : \
                                                                     bcmFieldStageIngress ) ) )

#define _BCM_DPP_FIELD_PRESEL_FLAG_FROM_STAGE(_e) ( (_e == bcmFieldStageHash)     ? BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_HASH : \
                                                  ( (_e == bcmFieldStageExternal) ? BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EXTERNAL : \
                                                  ( (_e == bcmFieldStageEgress)   ? BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_EGRESS : \
                                                                                    BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS ) ) )

/*
 *   Function
 *      _bcm_dpp_field_presel_init
 *   Purpose
 *      Init the state of the unit's preselectors
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      Assumes PPD sets up PFG 4 on Soc_petraB.
 */
extern int
_bcm_dpp_field_presel_init(bcm_dpp_field_info_OLD_t *unitData);

/*
 *   Function
 *      _bcm_dpp_field_presel_reset
 *   Purpose
 *      Reset the state of the unit's preselectors, at the PPD layer
 *   Parameters
 *      (in) int unit = the unit number to reset
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      This scans the PPD layer and frees resources.  It should not be used
 *      for detach, but must be used for cold init.
 *
 *      Assumes PPD owns PFG 4 on Soc_petraB.
 *
 *      Only returns last error if more than one occurs.
 */
extern int
_bcm_dpp_field_presel_reset(int unit);

/*
 *   Function
 *      _bcm_dpp_field_presel_info_get
 *   Purpose
 *      Get information for a single preselector from PPD
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) _bcm_dpp_field_presel_idx_t presel = which preselector to read
 *      (in) unsigned int qualLimit = max number of qualifiers
 *      (out) _bcm_dpp_field_stage_idx_t *stage = where to put stage index
 *      (out) _bcm_dpp_field_qual_t *bcmQuals = where to put BCM qualifiers
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *     Out arguments which are NULL will simply not be filled in.
 */
extern int
_bcm_dpp_field_presel_info_get(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel,
                               unsigned int qualLimit,
                               uint32 flags,
                               _bcm_dpp_field_stage_idx_t *stage,
                               _bcm_dpp_field_qual_t *bcmQuals);

/*
 *   Function
 *      _bcm_dpp_field_presel_info_set
 *   Purpose
 *      Set information for a single preselector
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) _bcm_dpp_field_presel_idx_t presel = which preselector to write
 *      (in) unsigned int qualLimit = max number of qualifiers
 *      (in) _bcm_dpp_field_stage_idx_t stage = stage index
 *      (in) _bcm_dpp_field_qual_t *bcmQuals = where to get BCM qualifiers
 *      (in) _bcm_dpp_field_program_idx_t program_id = the program that the preselctor will point to
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *     Due to a requirement that the PFG be overwritten with a blank one
 *     ('destroyed') before we can update it (so no direct overwrite), there is
 *     the possibility that a single error in a series of updating a presel
 *     will result in it not having the desired qualifiers.  Unhappily, there
 *     seems to be no easy solution to this since we are not caching them.
 */
extern int
_bcm_dpp_field_presel_info_set(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel,
                               unsigned int qualLimit,
                               uint32 flags,
                               _bcm_dpp_field_stage_idx_t stage,
                               const _bcm_dpp_field_qual_t *bcmQuals,
                               _bcm_dpp_field_program_idx_t program_id);

void _bcm_dpp_clear_core_ports(int                  nof_cores,
                               _bcm_dpp_core_ports_bitmap_t *core_port_bitmap);

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_presel_dump
 *   Purpose
 *      Dump state of all preselectors
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) char *prefix = prefix for each line of the dump
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_presel_dump(bcm_dpp_field_info_OLD_t *unitData,
                           const char *prefix);
#endif /* def BROADCOM_DEBUG */

#ifdef BCM_PETRA_SUPPORT
/*
 *   Function
 *      _bcm_dpp_field_presel_implied_setup
 *   Purpose
 *      Set up prequalifiers for a group that uses implied preselectors
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in/out) _bcm_dpp_petra_field_group_t *groupData = ptr to group data
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      Assumes group is valid and exists.
 */
extern int
_bcm_dpp_field_presel_implied_setup(bcm_dpp_field_info_OLD_t *unitData,
                                    _bcm_dpp_field_group_t *groupData);
#endif /* def BCM_PETRA_SUPPORT */

#ifdef BCM_PETRA_SUPPORT
/*
 *   Function
 *      _bcm_dpp_field_presel_implied_teardown
 *   Purpose
 *      Tear down prequalifiers for a group that uses implied preselectors
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) _bcm_dpp_field_group_t *groupData = pointer to group data
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      Assumes group is valid but has been removed from hardware, as in
 *      preparation for deletion of the group.
 */
extern int
_bcm_dpp_field_presel_implied_teardown(bcm_dpp_field_info_OLD_t *unitData,
                                       _bcm_dpp_field_group_t *groupData);
#endif /* def BCM_PETRA_SUPPORT */

/*
 *   Function
 *      _bcm_dpp_field_presel_qualify
 *   Purpose
 *      Apply a qualifier to a preselector
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) bcm_field_presel_t presel = presel to be manipulated
 *      (in) bcm_field_qualify_t qualifier = qualifier to adjust
 *      (in) unsigned int count = number of octbytes of data/mask
 *      (in) uint64 *data = pointer to qualifier data
 *      (in) uint64 *mask = pointer to qualifier mask
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *     There are some 'special' qualifiers used by preselectors on Arad (such
 *     as the bcmFieldQualifyStage qualifier) and on PetraB all qualifiers must
 *     be explicitly mapped to special purpose elements in the PPD descriptor,
 *     so this function is used as an intermediary to give the preselector code
 *     a chance to do something 'special' with any given qualifier.  If a
 *     qualifier is not 'special', then it passes the responsibility back to the
 *     common qualifier code, which updates the preselector's qualifiers.
 */
extern int
_bcm_dpp_field_presel_qualify(bcm_dpp_field_info_OLD_t *unitData,
                              bcm_field_presel_t presel,
                              bcm_field_stage_t  stage, 
                              uint32 flags,
                              bcm_field_qualify_t qualifier,
                              unsigned int count,
                              const uint64 *data,
                              const uint64 *mask);

/*
 *   Function
 *      _bcm_dpp_field_presel_qualify_get
 *   Purpose
 *      Get a qualifier from a preselector
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) bcm_field_presel_t presel = presel to be manipulated
 *      (in) bcm_field_qualify_t qualifier = qualifier to get
 *      (in) unsigned int count = number of octbytes of data/mask space
 *      (out) uint64 *data = pointer to qualifier data
 *      (out) uint64 *mask = pointer to qualifier mask
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_presel_qualify_get(bcm_dpp_field_info_OLD_t *unitData,
                                  bcm_field_presel_t presel,
                                  bcm_field_stage_t  stage,
                                  uint32 flags,
                                  bcm_field_qualify_t qualifier,
                                  unsigned int count,
                                  uint64 *data,
                                  uint64 *mask);

/*
 *   Function
 *      _bcm_dpp_field_group_presel_set
 *   Purpose
 *      Set preselector set for a group
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) _bcm_dpp_field_grp_idx_t group = group to update
 *      (in) bcm_field_presel_set_t *presel_set = pointer to preselector set
 *      (in) pgm_bmp =  Bitmap of program-ids which the current field group will be pointing to
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_group_presel_set(bcm_dpp_field_info_OLD_t *unitData,
                                _bcm_dpp_field_grp_idx_t group,
                                bcm_field_presel_set_t *presel_set,
                                 uint32 pgm_bmp);

/*
 *  Function
 *    _bcm_dpp_field_presel_port_profile_set
 *  Purpose
 *    Allocate a program selection profile for set of ports
 *  Arguments
 *    (in) unit - the unit ID
 *    (in) profile_type - profile type according to _bcm_dpp_field_profile_type_t
 *    (in) port_bitmap - set of ports to configure
 *    (out) profile = the profile allocated
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_presel_port_profile_set(int unit,
                                       _bcm_dpp_field_profile_type_t profile_type,
                                       int    nof_cores,
                                       _bcm_dpp_core_ports_bitmap_t *core_port_bitmap, 
                                       uint64 *profile);

/*
 *  Function
 *    _bcm_dpp_field_presel_port_profile_get
 *  Purpose
 *    Get the port-set relating to a presel profile
 *  Arguments
 *    (in) unit - the unit ID
 *    (in) profile_type - profile type according to _bcm_dpp_field_profile_type_t
 *    (in) profile - the profile ID
 *    (out) port_bitmap - the set of ports
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_presel_port_profile_get(int unit,
                                       _bcm_dpp_field_profile_type_t profile_type,
                                       uint32 profile,
                                       int    *nof_core,
                                       _bcm_dpp_core_ports_bitmap_t *core_port_bitmap);

/*
 *  Function
 *    _bcm_dpp_field_presel_port_profile_clear
 *  Purpose
 *    Remove a presel profile from ports if configured
 *  Arguments
 *    (in) unitData - pointer to unit data
 *    (in) entry - the presel entry
 *  Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      In case the profile is used by another presel, only
 *      decrease reference counter, otherwise remove completely.
 */
extern int
_bcm_dpp_field_presel_port_profile_clear_type(bcm_dpp_field_info_OLD_t *unitData,
                                              _bcm_dpp_field_profile_type_t profile_type,
                                              bcm_field_entry_t entry);

extern int
_bcm_dpp_field_presel_port_profile_clear_all(bcm_dpp_field_info_OLD_t *unitData,
                                             bcm_field_entry_t entry);

extern int
_bcm_dpp_field_presel_port_entry_profile_get(bcm_dpp_field_info_OLD_t *unitData,
                                             _bcm_dpp_field_profile_type_t profile_type,
                                             bcm_field_entry_t entry,
                                             uint64 *profile);

extern int
_bcm_dpp_field_presel_install(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel,
                              uint8 is_second_pass);

extern int
_bcm_dpp_field_presel_destroy(bcm_dpp_field_info_OLD_t *unitData,
                               _bcm_dpp_field_presel_idx_t presel,
                              uint8 is_second_pass);

extern int
_bcm_dpp_field_presel_to_bitmap(int unit,
                                bcm_field_presel_t presel_id,
                                bcm_field_stage_t  stage,
                                uint8 is_second_pass,
                                bcm_field_presel_t *presel_bitmap_id,
                                _bcm_dpp_field_stage_idx_t *stage_id);

extern int
_bcm_dpp_field_presel_index_to_pfg(int unit,
                             bcm_field_presel_t presel_id,
                             bcm_field_stage_t *stage,
                             int *pfg_ndx);

/* -- field_dataqual.c -- */

/*
 *   Function
 *      _bcm_petra_field_data_qualifier_init
 *   Purpose
 *      Init the state of the unit's data qualifiers
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
extern int
_bcm_petra_field_data_qualifier_init(bcm_dpp_field_info_OLD_t *unitData);

/*
 *   Function
 *      _bcm_petra_field_data_qualifier_reset
 *   Purpose
 *      Reset the state of the unit's data qualifiers, at the PPD layer
 *   Parameters
 *      (in) int unit = the unit number to reset
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      This scans the PPD layer and frees resources.  It should not be used
 *      for detach, but must be used for cold init.
 *
 *      Assumes PPD owns PFG 4 on Soc_petraB.
 *
 *      Only returns last error if more than one occurs.
 */
extern int
_bcm_petra_field_data_qualifier_reset(int unit);

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_petra_field_data_qualifier_dump
 *   Purpose
 *      Dump state of all data qualifiers
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) char *prefix = prefix for each line of the dump
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_petra_field_data_qualifier_dump(bcm_dpp_field_info_OLD_t *unitData,
                                     const char *prefix);
#endif /* def BROADCOM_DEBUG */

/*
 *  Function
 *    _bcm_petra_field_map_ppd_udf_to_bcm
 *  Purpose
 *    Map a PPD layer user-defined qualifier to BCM used-defined index
 *  Parameters
 *    (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *    (in) SOC_PPC_FP_QUAL_TYPE hwType = PPD layer qualifier type
 *    (out) unsigned int *bcmIndex = where to put BCM layer index
 */
extern int
_bcm_petra_field_map_ppd_udf_to_bcm(bcm_dpp_field_info_OLD_t *unitData,
                                    SOC_PPC_FP_QUAL_TYPE hwType,
                                    unsigned int *bcmIndex);

/*
 *  Function
 *    _bcm_petra_field_map_bcm_udf_to_ppd
 *  Purpose
 *    Map a BCM layer used-defined index to PPD user-defined qualifier
 *  Parameters
 *    (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *    (in) SOC_PPC_FP_QUAL_TYPE hwType = PPD layer qualifier type
 *    (out) unsigned int *bcmIndex = where to put BCM layer index
 */
extern int
_bcm_petra_field_map_bcm_udf_to_ppd(bcm_dpp_field_info_OLD_t *unitData,
                                    unsigned int bcmIndex,
                                    SOC_PPC_FP_QUAL_TYPE *hwType);

/*
 *  Function
 *    _bcm_petra_field_data_qualifier_bits_get
 *  Purpose
 *    Get number of bits represented by a data qualifier
 *  Parameters
 *    (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *    (in) SOC_PPC_FP_QUAL_TYPE hwType = PPD layer qualifier type
 *    (out) unsigned int *bcmIndex = where to put BCM layer index
 */
extern int
_bcm_petra_field_data_qualifier_bits_get(bcm_dpp_field_info_OLD_t *unitData,
                                         SOC_PPC_FP_QUAL_TYPE hwType,
                                         unsigned int *bits);


/* -- field_range.c -- */

/*
 *   Function
 *      _bcm_dpp_field_range_init
 *   Purpose
 *      Init the state of the unit's ranges
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
extern int
_bcm_dpp_field_range_init(bcm_dpp_field_info_OLD_t *unitData);

/*
 *   Function
 *      _bcm_dpp_field_range_reset
 *   Purpose
 *      Reset the state of the unit's ranges, at the PPD layer
 *   Parameters
 *      (in) int unit = the unit number to reset
 *      (in) uint32 handle = the hardware handle for the unit
 *      (in) _bcm_dpp_Field_device_info_t *devInfo = pointer to unit dev info
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *      This scans the PPD layer and frees resources.  It should not be used
 *      for detach, but must be used for cold init.
 *
 *      Assumes PPD owns PFG 4 on Soc_petraB.
 *
 *      Only returns last error if more than one occurs.
 */
extern int
_bcm_dpp_field_range_reset(int unit,
                           uint32 handle,
                           const _bcm_dpp_field_device_info_t *devInfo);

/*
 *   Function
 *      _bcm_dpp_field_range_get
 *   Purpose
 *      Get parameters for a range
 *   Parameters
 *      (in) unitData = unit information
 *      (in) bcm_field_range_t range = range ID to get
 *      (out) uint32 flags = flags for the range
 *      (out) uint32 *min = pointer to min port for the range
 *      (out) uint32 *max = pointer to max port for the range
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if valid
 *                    BCM_E_NOT_FOUND if not valid
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
int
_bcm_dpp_field_range_get(bcm_dpp_field_info_OLD_t  *unitData,
                         bcm_field_range_t range,
                         int maxCount,
                         uint32 *flags,
                         uint32 *min,
                         uint32 *max,
                         int *actualCount);

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_range_dump
 *   Purpose
 *      Dump state of all ranges on the unit
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) char *prefix = prefix for each line of the dump
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 */
extern int
_bcm_dpp_field_range_dump(bcm_dpp_field_info_OLD_t *unitData,
                            const char *prefix);
#endif /* def BROADCOM_DEBUG */

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_entry_range_dump
 *   Purpose
 *      Dump all ranges associated with an entry
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) _bcm_dpp_field_qual_t *qualArray = qualifier array from entry
 *      (in) SOC_PPC_FP_QUAL_VAL *qualHwData = qualifier array from hardware
 *      (in) char prefix = prefix for each line of the dump
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 *     Adds four spaces to the prefix (to format for entry dump)
 */
int
_bcm_dpp_field_entry_range_dump(bcm_dpp_field_info_OLD_t  *unitData,
                                 const _bcm_dpp_field_qual_t *qualArray,
#if _BCM_DPP_FIELD_DUMP_VERIFY_PPD
                                const SOC_PPC_FP_QUAL_VAL *qualHwData,
#endif
                                const char *prefix);
#endif /* def BROADCOM_DEBUG */


/* -- field_dir_ext.c -- */

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_dir_ext_entry_dump
 *   Purpose
 *      Dump an entry in a direct extraction group
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) _bcm_dpp_field_ent_idx_t entry = direct extraction entry to dump
 *      (in) char prefix = prefix for each line of the dump
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
extern int
_bcm_dpp_field_dir_ext_entry_dump(bcm_dpp_field_info_OLD_t *unitData,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  const char *prefix);
#endif /* def BROADCOM_DEBUG */

/*
 *  Function
 *     _bcm_dpp_field_dir_ext_entry_clear
 *  Purpose
 *     Clear everything about a direct extraction entry but next,prev linkx.
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) entry = entry ID
 *  Returns
 *     nothing
 *  Notes
 *     This is only called when allocating entries, so it does not adjust the
 *     entry's group's flags (and the entry's flags are cleared).  This also
 *     does not clean up the entry, on the assumption it was not used before
 *     and so the resources it might appear to claim would be bogus.
 */
extern void
_bcm_dpp_field_dir_ext_entry_clear(bcm_dpp_field_info_OLD_t *unitData,
                                   _bcm_dpp_field_ent_idx_t entry);

/*
 *  Function
 *     _bcm_dpp_field_dir_ext_entry_action_clear
 *  Purpose
 *     Remove an action from an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) action = BCM action type
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Assumes the arguments are all valid.
 *
 *     Considers it BCM_E_CONFIG to manipulate actions that are not in the
 *     ASET for the entry's group.
 *
 *     Marks the entry as changed, and the entry's group as containing changed
 *     entry/entries.
 *
 *     Since it is possible that a single action at the BCM layer will require
 *     multiple actions at PPD, this will clear all actions of an entry that
 *     have the specified type (note that in this case the hardware types of
 *     the different actions will be different).
 */
extern int
_bcm_dpp_field_dir_ext_entry_action_clear(bcm_dpp_field_info_OLD_t *unitData,
                                          _bcm_dpp_field_ent_idx_t entry,
                                          bcm_field_action_t type);

/*
 *  Function
 *     _bcm_dpp_field_dir_ext_entry_clear_acts
 *  Purpose
 *     Clear explicit actions from an entry.
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) entry = entry ID
 *  Returns
 *     nothing
 *  Notes
 */
extern int
_bcm_dpp_field_dir_ext_entry_clear_acts(bcm_dpp_field_info_OLD_t *unitData,
                                        _bcm_dpp_field_ent_idx_t entry);

/*
 *   Function
 *      _bcm_dpp_field_dir_ext_entry_alloc
 *   Purpose
 *      Allocate a direct extraction entry
 *   Parameters
 *      (in) bcm_dpp_field_info_OLD_t *unitData = pointer to unit data
 *      (in) int withId = TRUE if caller provides the ID
 *      (in/out) _bcm_dpp_field_ent_idx_t *entry = where to put/get the entry
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
extern int
_bcm_dpp_field_dir_ext_entry_alloc(bcm_dpp_field_info_OLD_t *unitData,
                                   _bcm_dpp_field_grp_idx_t group,
                                   int withId,
                                   _bcm_dpp_field_ent_idx_t *entry);

/*
 *  Function
 *     _bcm_dpp_field_dir_ext_entry_add_to_group
 *  Purpose
 *     Insert an entry into a group.
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) group = group ID
 *     (in) entry = entry ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     If the entry is already in a group, it will remove the entry from that
 *     group and insert it into the specified group, according to the BCM
 *     priority rules.  If the entry is in hardware, this will remove the entry
 *     from hardware.
 *
 *     Marks the entry as new/moved, changed, and in use.
 *
 *     Marks the entry's new group as containing net/moved entry/entries and
 *     changed entry/entries.
 */
extern int
_bcm_dpp_field_dir_ext_entry_add_to_group(bcm_dpp_field_info_OLD_t *unitData,
                                          _bcm_dpp_field_grp_idx_t group,
                                          _bcm_dpp_field_ent_idx_t entry);

/*
 *  Function
 *     _bcm_dpp_field_dir_ext_entry_remove
 *  Purpose
 *     Remove an entry (if needed) from the hardware
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Considers NOP (remove a non-installed entry) as success, but considers
 *     trying to remove an unused entry an error.
 */
extern int
_bcm_dpp_field_dir_ext_entry_remove(bcm_dpp_field_info_OLD_t *unitData,
                                    _bcm_dpp_field_ent_idx_t entry);

/*
 *  Function
 *     _bcm_dpp_field_dir_ext_entry_destroy
 *  Purpose
 *     Destroy an entry, after (if needed) removal from hardware
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 */
extern int
_bcm_dpp_field_dir_ext_entry_destroy(bcm_dpp_field_info_OLD_t *unitData,
                                     _bcm_dpp_field_ent_idx_t entry);

/*
 *   Function
 *      _bcm_dpp_field_dir_ext_group_hw_refresh
 *   Purpose
 *      Recommit (or initially commit) entries of a direct extraction group to
 *      hardware
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_group_t group = which group ID to use
 *      (in) cond = TRUE for conditional update, FALSE otherwise
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Assumes group and entry validity checking has been performed.
 *
 *      This inserts or updates all of the group's entries that WANT_HW to
 *      hardware, and removes any IN_HW that don't WANT_HW.  It is used by
 *      the group install/remove functionality, but it is also invoked for
 *      cases where a single entry is installed but that entry does not have
 *      an assigned hardware priority (so the priorities need to be redone).
 *
 *      This behavior implies a peculiar result when dealing with uncommitted
 *      entries: If inserting even a single 'new' or 'moved' entry, any
 *      uncommitted changes to any other entries in the same group that WANT_HW
 *      will be committed.  This could only be avoided by significant
 *      additional complexity, since either we have to track two states
 *      (committed and not) for each entry, or we have to make the priority
 *      allocation mechanism a lot more complex and capable of being split into
 *      many segments instead of the current implementation's two segments.
 *
 *      Note that it is not obligatory to call this when removing an entry
 *      since the removal of entries does not oblige priority recalculation,
 *      and thence does not require the subsequent refresh.
 *
 *      This function does not change the WANT_HW state of any entries.
 *
 *      Expects to be called with the lock already held and arguments checked.
 *
 *      Conditional install is a special condition that is used when adding a
 *      single entry (or set in theory) whose priority was not assigned or was
 *      stale.  All of the existing entries need to be updated as well but only
 *      the ones marked UPDATE will be transitioned from IN_HW to not, or not
 *      to IN_HW, and all of the entries already IN_HW will be copied from
 *      hardware state instead of having their current software state used.
 *      This is to preserve the API semantic that bcm_field_entry_(re)install
 *      only affects the single specified entry, but it makes the routine of
 *      create, config, commit something approaching an O(n^3) operation; it is
 *      far better to use the create all, config all, commit group sequence.
 */
extern int
_bcm_dpp_field_dir_ext_group_hw_refresh(bcm_dpp_field_info_OLD_t *unitData,
                                        bcm_field_group_t group,
                                        int cond);

/*
 *   Function
 *      _bcm_dpp_field_dir_ext_entry_install
 *   Purpose
 *      Write an entry to the hardware, either 'install' or 'reinstall'
 *   Parameters
 *      (in) unitData = unit information
 *      (in) _bcm_dpp_field_ent_idx_t entry = the entry ID to destroy
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Meant to be called without the lock being held
 *
 *      Curiously, it is not an error to 'install' an entry that is already in
 *      hardware, nor is it an error to 'reinstall' an entry that is not
 *      already in hardware.  Seems these functions are redundant.
 *
 *      This does NOT remove the entry.
 */
extern int
_bcm_dpp_field_dir_ext_entry_install(bcm_dpp_field_info_OLD_t *unitData,
                                     _bcm_dpp_field_ent_idx_t entry);

/*
 *  Function
 *     _bcm_dpp_field_dir_ext_entry_copy_id
 *  Purpose
 *     Copy one field entry to another
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) dest = destination entry ID
 *     (in) source = source entry ID
 *  Returns
 *     nothing
 *  Notes
 *     New entry has copy of stats settings, and the new entry will be in the
 *     stats chain with the old entry if the old entry had a statistics set
 *     associated.
 *
 *     Assumes dest entry is not in use, and source entry is in use.
 *
 *     Dest must be in same group as source.
 */
extern void
_bcm_dpp_field_dir_ext_entry_copy_id(bcm_dpp_field_info_OLD_t *unitData,
                                     _bcm_dpp_field_ent_idx_t dest,
                                     _bcm_dpp_field_ent_idx_t source);

void
   _bcm_dpp_field_ext_info_entry_clear(int unit, int32 extTcamInfoIndex) ;
#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
/*
 *  Function
 *    _bcm_dpp_field_dir_ext_hardware_entry_check
 *  Purpose
 *    Get a direct extraction entry from the PPD layer, verifying (according to
 *    settings) that it is consistent with the expected BCM layer values.
 *  Parameters
 *    (in) unitData = the unit information
 *    (in) entry = the entry ID
 *    (out) entInfoDe = where to put the hardware entry description
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *    Might return an error for mismatch or just display a warning, according
 *    to configuration.
 *
 *    Is also used to retrieve the entry in preparation for using the PPD layer
 *    values for a 'get' operation.
 */
int
_bcm_dpp_field_dir_ext_hardware_entry_check(bcm_dpp_field_info_OLD_t *unitData,
                                            bcm_field_entry_t entry,
                                            SOC_PPC_FP_DIR_EXTR_ENTRY_INFO *entInfoDe);
#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */


/* -- field_tcam.c -- */

#ifdef BROADCOM_DEBUG
/*
 *   Function
 *      _bcm_dpp_field_tcam_entry_dump
 *   Purpose
 *      Dump the state of a specific field entry (TCAM based)
 *   Parameters
 *      (in) unitData = unit information
 *      (in) entry = entry to be dumped
 *      (in) prefix = string to prefix to each line
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately otherwise
 *   Notes
 */
extern int
_bcm_dpp_field_tcam_entry_dump(bcm_dpp_field_info_OLD_t *unitData,
                               uint8 isExternalTcam,
                               _bcm_dpp_field_ent_idx_t entry,
                               const char *prefix);
#endif /* def BROADCOM_DEBUG */
/*
 *   Function
 *     _bcm_dpp_field_ace_entry_add
 *   Purpose
 *     Load ACE tables as per input parameters. Tables are:
 *       EPNI_ACE_TO_OUT_PP_PORT
 *       EPNI_ACE_TABLE
 *       EPNI_ACE_TO_OUT_LIF
 *       (EPNI_ACE_TO_FHEI is, currently, not handled)
 *     Note that this procedure does write into HW.
 *   Parameters
 *      (in)  int unit = the unit number
 *      (in)  _bcm_dpp_field_tc_b_act_t *propActs =
 *              A Pointer to a structure of type _bcm_dpp_field_tc_b_act_t.
 *              Currenly, only one 'bcmType' is allowed and it
 *              must be 'bcmFieldActionStat'.
 *              The 'parameters' are as follows:
 *                bcmParam0 =
 *                  Statistic-Report Counter format as in Jericho.
 *                  It contains both ACE pointer (in the LS 17 bits) and
 *                  'prge' in the following 10 bits. Must be a valid value.
 *                  See _SHR_FIELD_CTR_PROC_SHIFT_GET, _SHR_FIELD_CTR_PROC_SHIFT
 *                bcmParam1 =
 *                  Gport representing TM-Port and PP-Port.
 *                  Must be a valid value.
 *                bcmParam2 =
 *                  If valid (not BCM_ILLEGAL_ACTION_PARAMETER) then this is the outlif
 *                  id to use to replace existing outlif id.
 *      (in)  core_id = core to perform action
 *      (in)  unsigned int ppdIndex =
 *              The index to start writing into 'hwAct'. See below.
 *              This procedure will start loading at hwAct[ppdIndex], then.
 *      (out) _bcm_dpp_field_tc_p_act_t *hwAct =
 *              A pointer to an array of structures of type _bcm_dpp_field_tc_p_act_t.
 *              Each structure contains internal info that is used for the configuration of
 *              the HW. Caller is responsible for available space.
 *              his array is loaded by this procedure starting from index 'ppdIndex'
 *      (out) unsigned int *actCount_p =
 *              Pointer to unsigned int. This procedure loads pointed memory by the number
 *              of information elements loaded into hwAct[]
 *   Returns
 *     int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *     Updates ACE HW tables:
 *       EPNI_ACE_TO_OUT_PP_PORT
 *       EPNI_ACE_TABLE
 *       EPNI_ACE_TO_OUT_LIF
 *       (EPNI_ACE_TO_FHEI is, currently, not handled)
 *
 *   This procedure is for JERICHO/JERICHO_PLUS only!
 *   Caller is assumed to have verified that before calling this procedure.
 */
int
_bcm_dpp_field_ace_entry_add(
    int unit,
    _bcm_dpp_field_tc_b_act_t *propActs,
    int core_id,
    unsigned int ppdIndex,
    _bcm_dpp_field_tc_p_act_t *hwAct,
    unsigned int *actCount_p);
/*
 *   Function
 *      _bcm_dpp_field_tcam_entry_install
 *   Purpose
 *      Write an entry to the hardware, either 'install' or 'reinstall'
 *   Parameters
 *      (in) unitData = unit information
 *      (in) _bcm_dpp_field_ent_idx_t entry = the entry ID to destroy
 *      (out)_bcm_dpp_field_ent_idx_t new_entry = large direct lookup entry ID
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Meant to be called without the lock being held
 *
 *      Curiously, it is not an error to 'install' an entry that is already in
 *      hardware, nor is it an error to 'reinstall' an entry that is not
 *      already in hardware.  Seems these functions are redundant.
 *
 *      This does NOT remove the entry.
 */
extern int
_bcm_dpp_field_tcam_entry_install(bcm_dpp_field_info_OLD_t *unitData,
                                  uint8 isExternalTcam,
                                  _bcm_dpp_field_ent_idx_t entry,
                                  bcm_field_entry_t *new_entry);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_remove
 *  Purpose
 *     Remove an entry (if needed) from the hardware
 *  Parameters
 *     (in) unitData = unit information
 *     (in) is_kaps_sw_remove = Remove large direct lookup software entry
 *     (in) entry = entry number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Considers NOP (remove a non-installed entry) as success, but considers
 *     trying to remove an unused entry an error.
 */
extern int
_bcm_dpp_field_tcam_entry_remove(bcm_dpp_field_info_OLD_t *unitData,
                                 uint8 is_kaps_sw_remove,
                                 bcm_field_entry_t entry);

/*
 *   Function
 *      _bcm_dpp_field_tcam_group_hw_refresh
 *   Purpose
 *      Recommit (or initially commit) entries of a TCAM group to hardware
 *   Parameters
 *      (in) int unit = the unit number
 *      (in) bcm_field_group_t group = which group ID to use
 *      (in) cond = TRUE for conditional update, FALSE otherwise
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Assumes group and entry validity checking has been performed.
 *
 *      This inserts or updates all of the group's entries that WANT_HW to
 *      hardware, and removes any IN_HW that don't WANT_HW.  It is used by
 *      the group install/remove functionality, but it is also invoked for
 *      cases where a single entry is installed but that entry does not have
 *      an assigned hardware priority (so the priorities need to be redone).
 *
 *      This behavior implies a peculiar result when dealing with uncommitted
 *      entries: If inserting even a single 'new' or 'moved' entry, any
 *      uncommitted changes to any other entries in the same group that WANT_HW
 *      will be committed.  This could only be avoided by significant
 *      additional complexity, since either we have to track two states
 *      (committed and not) for each entry, or we have to make the priority
 *      allocation mechanism a lot more complex and capable of being split into
 *      many segments instead of the current implementation's two segments.
 *
 *      Note that it is not obligatory to call this when removing an entry
 *      since the removal of entries does not oblige priority recalculation,
 *      and thence does not require the subsequent refresh.
 *
 *      This function does not change the WANT_HW state of any entries.
 *
 *      Expects to be called with the lock already held and arguments checked.
 *
 *      Conditional install is a special condition that is used when adding a
 *      single entry (or set in theory) whose priority was not assigned or was
 *      stale.  All of the existing entries need to be updated as well but only
 *      the ones marked UPDATE will be transitioned from IN_HW to not, or not
 *      to IN_HW, and all of the entries already IN_HW will be copied from
 *      hardware state instead of having their current software state used.
 *      This is to preserve the API semantic that bcm_field_entry_(re)install
 *      only affects the single specified entry, but it makes the routine of
 *      create, config, commit something approaching an O(n^3) operation; it is
 *      far better to use the create all, config all, commit group sequence.
 */
extern int
_bcm_dpp_field_tcam_group_hw_refresh(bcm_dpp_field_info_OLD_t *unitData,
                                     bcm_field_group_t group,
                                     int cond);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_action_clear
 *  Purpose
 *     Remove an action from an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) action = BCM action type
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Assumes the arguments are all valid.
 *
 *     Considers it BCM_E_CONFIG to manipulate actions that are not in the
 *     ASET for the entry's group.
 *
 *     Marks the entry as changed, and the entry's group as containing changed
 *     entry/entries.
 *
 *     Since it is possible that a single action at the BCM layer will require
 *     multiple actions at PPD, this will clear all actions of an entry that
 *     have the specified type (note that in this case the hardware types of
 *     the different actions will be different).
 */
extern int
_bcm_dpp_field_tcam_entry_action_clear(bcm_dpp_field_info_OLD_t *unitData,
                                       uint8 isExternalTcam,
                                       _bcm_dpp_field_ent_idx_t entry,
                                       bcm_field_action_t type);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_clear_acts
 *  Purpose
 *     Clear explicit actions from an entry.
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) entry = entry ID
 *  Returns
 *     nothing
 *  Notes
 */
extern int
_bcm_dpp_field_tcam_entry_clear_acts(bcm_dpp_field_info_OLD_t *unitData,
                                     uint8 isExternalTcam,
                                     _bcm_dpp_field_ent_idx_t entry);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_clear
 *  Purpose
 *     Clear everything about an entry but next,prev linkx.
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) entry = entry ID
 *  Returns
 *     nothing
 *  Notes
 *     This is only called when allocating entries, so it does not adjust the
 *     entry's group's flags (and the entry's flags are cleared).  This also
 *     does not clean up the entry, on the assumption it was not used before
 *     and so the resources it might appear to claim would be bogus.
 */
extern void
_bcm_dpp_field_tcam_entry_clear(bcm_dpp_field_info_OLD_t *unitData,
                                uint8 isExternalTcam,
                           _bcm_dpp_field_ent_idx_t entry);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_action_set
 *  Purpose
 *     Set an action to an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) core_id = core to perform the action
 *     (in) entry = entry ID
 *     (in) action = BCM qualifier type
 *     (in) param0 = action argument 0
 *     (in) param1 = action argument 1
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Actions can not be removed with this function because it is possible
 *     for both arguments to be zero (or even don't care!).
 *
 *     If _BCM_PETRA_ALLOW_IMPLIED_ACTION_REPLACE is TRUE and the action
 *     already exists, this will replace it.  If FALSE and the action already
 *     exists, this will return BCM_E_EXISTS.
 *
 *     Several BCM layer actions share underlying hardware actions.  These
 *     actions can only be added if there is not already another BCM action
 *     using the underlying hardware action.
 *
 *     Some BCM layer actions require more than one PPD layer action to fully
 *     implement.  These actions are added/manipulated atomically, and will be
 *     considered together for other purposes (get, delete, &c) but can be
 *     split during the add.  They can not be added if any part of them
 *     conflicts with another existing action.
 *
 *     Considers it BCM_E_CONFIG to manipulate actions that are not in the
 *     ASET for the entry's group.
 *
 *     Marks the entry as changed, and the entry's group as containing changed
 *     entry/entries.
 *
 *     OutLif lookups are common between ingress and egress, but different
 *     requirements and capabilities of ingress/egress for destinations require
 *     GPORT lookups be tailored to the stage.
 */
extern int
_bcm_dpp_field_tcam_entry_action_set(bcm_dpp_field_info_OLD_t *unitData,
                                     uint8 isExternalTcam,
                                     int core_id,
                                     _bcm_dpp_field_ent_idx_t entry,
                                     bcm_field_action_t type,
                                     uint32 param0,
                                     uint32 param1,
                                     uint32 param2,
                                     uint32 param3);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_destroy
 *  Purpose
 *     Destroy an entry, after (if needed) removal from hardware
 *  Parameters
 *     (in) unitData = unit information
 *     (in) is_kaps_sw_remove = Remove large direct lookup software entry
 *     (in) entry = entry number
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 */
extern int
_bcm_dpp_field_tcam_entry_destroy(bcm_dpp_field_info_OLD_t *unitData,
                                  uint8 is_kaps_sw_remove,
                                  bcm_field_entry_t entry);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_add_to_group
 *  Purpose
 *     Insert an entry into a group.
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) group = group ID
 *     (in) entry = entry ID
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     If the entry is already in a group, it will remove the entry from that
 *     group and insert it into the specified group, according to the BCM
 *     priority rules.  If the entry is in hardware, this will remove the entry
 *     from hardware.
 *
 *     Marks the entry as new/moved, changed, and in use.
 *
 *     Marks the entry's new group as containing net/moved entry/entries and
 *     changed entry/entries.
 */
int
_bcm_dpp_field_tcam_entry_add_to_group(bcm_dpp_field_info_OLD_t *unitData,
                                       _bcm_dpp_field_grp_idx_t group,
                                       _bcm_dpp_field_ent_idx_t entry);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_action_get
 *  Purpose
 *     Get an action from an entry
 *  Parameters
 *     (in) unitData = unit information
 *     (in) entry = entry ID
 *     (in) action = BCM qualifier type
 *     (out) param0 = where to put arg0 value
 *     (out) param1 = where to put arg1 value
 *     (out) param2 = where to put arg2 value
 *     (out) param3 = where to put arg3 value
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *     Considers it BCM_E_CONFIG to manipulate actions that are not in the
 *     ASET for the entry's group.
 *
 *     Assumes the entry exists.
 *
 *     Returns the data for the first action of the type (since hardware can
 *     have more than one action per BCM action, the first action will be the
 *     one where the BCM layer parameters were kept, even though there may be
 *     hardware parameters for all of the DPP layer actions)
 */
extern int
_bcm_dpp_field_tcam_entry_action_get(bcm_dpp_field_info_OLD_t *unitData,
                                     bcm_field_entry_t entry,
                                     bcm_field_action_t type,
                                     uint32 *param0,
                                     uint32 *param1,
                                     uint32 *param2,
                                     uint32 *param3);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_copy_id
 *  Purpose
 *     Copy one field entry to another
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) dest = destination entry ID
 *     (in) source = source entry ID
 *  Returns
 *     nothing
 *  Notes
 *     New entry has copy of stats settings, and the new entry will be in the
 *     stats chain with the old entry if the old entry had a statistics set
 *     associated.
 *
 *     Assumes dest entry is not in use, and source entry is in use.
 *
 *     Dest must be in same group as source.
 */
extern void
_bcm_dpp_field_tcam_entry_copy_id(bcm_dpp_field_info_OLD_t *unitData,
                                  uint8 isExternalTcam,
                                  _bcm_dpp_field_ent_idx_t dest,
                                  _bcm_dpp_field_ent_idx_t source);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_hit_get
 *  Purpose
 *     Get hit indication info for a specific TCAM entry per core.
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) flags = destination entry ID
 *     (in) entry_handle = source entry ID
 *     (out) entry_hit_core_bmp_p = 1 bit, which indicates that
 *          the entry was hit. In case of Jericho1 and Jericho_Plus,
 *          the entry is valid on both cores, which means that
 *          in case of hit the returned values will be TRUE.
 *          Otherwise FALSE will be returned.
 *  Returns
 *     int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *  Notes
 */
int _bcm_dpp_field_tcam_entry_hit_get(
    bcm_dpp_field_info_OLD_t *unitData,
    uint32 flags,
    uint32 entry_handle,
    uint8 *entry_hit_core_bmp_p);

/*
 *  Function
 *     _bcm_dpp_field_tcam_entry_hit_flush
 *  Purpose
 *     This function flushes hit information of an entry or all entries,
 *     from hardware.
 *  Parameters
 *     (in) unitData = the unit information
 *     (in) flags = Flags to indicate what action should be performed
 *                   by the API, currently in use is:
 *               - BCM_FIELD_ENTRY_HIT_FLUSH_ALL - used for flushing hit
 *                 indication info for all entries. No need to specify an entry ID.
 *               - If no flag is specified, the API will clear information only for
 *                 the given entry_handle.
 *     (in) entry_handle = Unique entry handle for which the hit
 *               information will be flushed.
 *  Returns
 *     int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *  Notes
 */
int _bcm_dpp_field_tcam_entry_hit_flush(
    bcm_dpp_field_info_OLD_t *unitData,
    uint32 flags,
    uint32 entry_handle);

/*
 * Convert PPD-level (internal) stage to BCM stage.
 * Note that, currently, it works only for 4 internal stages:
 *   SOC_PPC_FP_DATABASE_STAGE_INGRESS_PMF
 *   SOC_PPC_FP_DATABASE_STAGE_EGRESS
 *   SOC_PPC_FP_DATABASE_STAGE_INGRESS_FLP
 *   SOC_PPC_FP_DATABASE_STAGE_INGRESS_SLB
 * See:
 *   _bcm_arad_field_stage_info[], bcm_petra_field_stage_info_get(),
 *   _bcm_arad_field_device_info
 */
/* static */
int _bcm_dpp_field_internal_stage_to_bcm_stage(
  int unit,
  SOC_PPC_FP_DATABASE_STAGE internal_stage,
  bcm_field_stage_t         *bcm_stage);

/*
 *   Function
 *      bcm_petra_field_data_qualifier_get
 *   Purpose
 *      Get a data qualifier configuration
 *   Parameters
 *      (in) unitData = unit information
 *      (in) int qual_id = which qualifier ID to use
 *      (out) bcm_field_data_qualifier_t *qual = where to put description
 *   Returns
 *      int (implied cast from bcm_error_t)
 *                    BCM_E_NONE if successful
 *                    BCM_E_* appropriately if not
 *   Notes
 *      Assumes all parameters already verified, except whether the data
 *      qualifier exists.
 */
extern int
_bcm_petra_field_data_qualifier_get(bcm_dpp_field_info_OLD_t *unitData,
                                    int qual_id,
                                    bcm_field_data_qualifier_t *qual);

#if _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD
/*
 *  Function
 *    _bcm_dpp_field_tcam_hardware_entry_check
 *  Purpose
 *    Get a TCAM entry from the PPD layer, verifying (according to settings)
 *    that it is consistent with the expected BCM layer values.
 *  Parameters
 *    (in) unitData = the unit information
 *    (in) entry = the entry ID
 *    (out) entInfoTc = where to put the hardware entry description
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 *  Notes
 *    Might return an error for mismatch or just display a warning, according
 *    to configuration.
 *
 *    Is also used to retrieve the entry in preparation for using the PPD layer
 *    values for a 'get' operation.
 */
int
_bcm_dpp_field_tcam_hardware_entry_check(bcm_dpp_field_info_OLD_t *unitData,
                                         bcm_field_entry_t entry,
                                         SOC_PPC_FP_ENTRY_INFO *entInfoTc);

/*
 *  Function
 *    _bcm_dpp_field_entry_ppd_to_bcm
 *  Purpose
 *    Given a PPD entry ID (hwHandle) find the corresponding BCM entry ID.
 *  Parameters
 *    (in) unitData = the unit information
 *    (in) hwHandle = the PPD entry ID
 *    (out) bcm_entry = the BCM entry ID
 *    (out) found = indication if entry was founds
 *  Returns
 *     int (implied cast from bcm_error_t)
 *        BCM_E_NONE if successful
 *        BCM_E_* appropriately if not
 */
int
_bcm_dpp_field_entry_ppd_to_bcm(int unit,
                                uint32 hwHandle,
                                bcm_field_entry_t *bcm_entry,
                                uint8 *found);

#endif /* _BCM_DPP_FIELD_GET_VERIFY_PPD || _BCM_DPP_FIELD_GET_USE_PPD */


int
_bcm_dpp_field_action_config_add_verify_params(int unit, 
                                               bcm_field_entry_t entry, 
                                               bcm_field_action_t action, 
                                               int core_config_arr_len, 
                                               bcm_field_action_core_config_t *core_config_arr);

/*
 * Internal Output of bcm_petra_field_internal_to_bcm_action_map():
 *
 * Indication on not getting enough space to load output to.
 */
#define INTERNAL_TO_BCM_ACTION_NO_SPACE    -1
/*
 * Internal error for arad_pmf_fem_action_init_if_required()
 * NOTE: This macro MUST be smaller than zero
 *
 * General error
 */
#define ACTION_INIT_IF_REQUIRED_FAIL       -1
/*
 * Internal output of arad_pmf_fem_action_type_array_element_get_unsafe()
 *
 * General failure
 */
#define ACTION_TYPE_ARRAY_ELEMENT_GET_FAIL -2
/*
 * Output of arad_pmf_fem_action_width_set_unsafe()
 *
 * If *db_identifier_p is set to this value then no
 * DB was found to be using input internal action.
 */
#define NO_DB_FOUND                        -1
/*
 * Output of arad_pmf_fem_action_width_get_unsafe()
 *
 * If input internal action is not found on runtime array
 * the *out_action_width is loaded by this value.
 * NOTE: This macro MUST be smaller than zero
 */
#define NOT_ON_RUNTIME_ARRAY               -1

#endif /* ndef _DPP_PETRA_FIELD_INT_H_ */
