/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

#ifndef __ARAD_TCAM_KEY_INCLUDED__

#define __ARAD_TCAM_KEY_INCLUDED__




#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_tcam.h>

#include <soc/dpp/TMC/tmc_api_tcam_key.h>






#define  ARAD_TCAM_RULE_NOF_UINT32S_MAX (SOC_TMC_TCAM_RULE_NOF_UINT32S_MAX)


#define  ARAD_TCAM_KEY_FLD_NOF_UINT32S_MAX (SOC_TMC_TCAM_KEY_FLD_NOF_UINT32S_MAX)



#define ARAD_EGR_ACL_DB_TYPE_ETH                             SOC_TMC_EGR_ACL_DB_TYPE_ETH
#define ARAD_EGR_ACL_DB_TYPE_IPV4                            SOC_TMC_EGR_ACL_DB_TYPE_IPV4
#define ARAD_EGR_ACL_DB_TYPE_MPLS                            SOC_TMC_EGR_ACL_DB_TYPE_MPLS
#define ARAD_EGR_ACL_DB_TYPE_TM                              SOC_TMC_EGR_ACL_DB_TYPE_TM
#define ARAD_EGR_NOF_ACL_DB_TYPES                            SOC_TMC_EGR_NOF_ACL_DB_TYPES
typedef SOC_TMC_EGR_ACL_DB_TYPE                                ARAD_EGR_ACL_DB_TYPE;

#define ARAD_PMF_TCAM_KEY_SRC_L2                          SOC_TMC_PMF_TCAM_KEY_SRC_L2
#define ARAD_PMF_TCAM_KEY_SRC_L3_IPV4                     SOC_TMC_PMF_TCAM_KEY_SRC_L3_IPV4
#define ARAD_PMF_TCAM_KEY_SRC_L3_IPV6                     SOC_TMC_PMF_TCAM_KEY_SRC_L3_IPV6
#define ARAD_PMF_TCAM_KEY_SRC_B_A                         SOC_TMC_PMF_TCAM_KEY_SRC_B_A
#define ARAD_PMF_TCAM_KEY_SRC_A_71_0                      SOC_TMC_PMF_TCAM_KEY_SRC_A_71_0
#define ARAD_PMF_TCAM_KEY_SRC_A_103_32                    SOC_TMC_PMF_TCAM_KEY_SRC_A_103_32
#define ARAD_PMF_TCAM_KEY_SRC_A_143_0                     SOC_TMC_PMF_TCAM_KEY_SRC_A_143_0
#define ARAD_PMF_TCAM_KEY_SRC_A_175_32                    SOC_TMC_PMF_TCAM_KEY_SRC_A_175_32
#define ARAD_PMF_TCAM_KEY_SRC_B_71_0                      SOC_TMC_PMF_TCAM_KEY_SRC_B_71_0
#define ARAD_PMF_TCAM_KEY_SRC_B_103_32                    SOC_TMC_PMF_TCAM_KEY_SRC_B_103_32
#define ARAD_PMF_TCAM_KEY_SRC_B_143_0                     SOC_TMC_PMF_TCAM_KEY_SRC_B_143_0
#define ARAD_PMF_TCAM_KEY_SRC_B_175_32                    SOC_TMC_PMF_TCAM_KEY_SRC_B_175_32
#define ARAD_NOF_PMF_TCAM_KEY_SRCS                        SOC_TMC_NOF_PMF_TCAM_KEY_SRCS
typedef SOC_TMC_PMF_TCAM_KEY_SRC                               ARAD_PMF_TCAM_KEY_SRC;

#define ARAD_TCAM_KEY_FORMAT_TYPE_PMF                        SOC_TMC_TCAM_KEY_FORMAT_TYPE_PMF
#define ARAD_TCAM_KEY_FORMAT_TYPE_EGR_ACL                    SOC_TMC_TCAM_KEY_FORMAT_TYPE_EGR_ACL
#define ARAD_TCAM_NOF_KEY_FORMAT_TYPES                       SOC_TMC_TCAM_NOF_KEY_FORMAT_TYPES
typedef SOC_TMC_TCAM_KEY_FORMAT_TYPE                           ARAD_TCAM_KEY_FORMAT_TYPE;

#define ARAD_PMF_TCAM_FLD_L2_LLVP                         SOC_TMC_PMF_TCAM_FLD_L2_LLVP
#define ARAD_PMF_TCAM_FLD_L2_STAG                         SOC_TMC_PMF_TCAM_FLD_L2_STAG
#define ARAD_PMF_TCAM_FLD_L2_CTAG_IN_AC                   SOC_TMC_PMF_TCAM_FLD_L2_CTAG_IN_AC
#define ARAD_PMF_TCAM_FLD_L2_SA                           SOC_TMC_PMF_TCAM_FLD_L2_SA
#define ARAD_PMF_TCAM_FLD_L2_DA                           SOC_TMC_PMF_TCAM_FLD_L2_DA
#define ARAD_PMF_TCAM_FLD_L2_ETHERTYPE                    SOC_TMC_PMF_TCAM_FLD_L2_ETHERTYPE
#define ARAD_PMF_TCAM_FLD_L2_IN_PP_PORT                   SOC_TMC_PMF_TCAM_FLD_L2_IN_PP_PORT
#define ARAD_NOF_PMF_TCAM_FLD_L2S                         SOC_TMC_NOF_PMF_TCAM_FLD_L2S
typedef SOC_TMC_PMF_TCAM_FLD_L2                                ARAD_PMF_TCAM_FLD_L2;

#define ARAD_PMF_TCAM_FLD_L3_MODE_IPV4                    SOC_TMC_PMF_TCAM_FLD_L3_MODE_IPV4
#define ARAD_PMF_TCAM_FLD_L3_MODE_IPV6                    SOC_TMC_PMF_TCAM_FLD_L3_MODE_IPV6
#define ARAD_NOF_PMF_TCAM_FLD_L3_MODES                    SOC_TMC_NOF_PMF_TCAM_FLD_L3_MODES
typedef SOC_TMC_PMF_TCAM_FLD_L3_MODE                           ARAD_PMF_TCAM_FLD_L3_MODE;

#define ARAD_PMF_FLD_IPV4_L4OPS                           SOC_TMC_PMF_FLD_IPV4_L4OPS
#define ARAD_PMF_FLD_IPV4_NEXT_PRTCL                      SOC_TMC_PMF_FLD_IPV4_NEXT_PRTCL
#define ARAD_PMF_FLD_IPV4_DF                              SOC_TMC_PMF_FLD_IPV4_DF
#define ARAD_PMF_FLD_IPV4_MF                              SOC_TMC_PMF_FLD_IPV4_MF
#define ARAD_PMF_FLD_IPV4_FRAG_NON_0                      SOC_TMC_PMF_FLD_IPV4_FRAG_NON_0
#define ARAD_PMF_FLD_IPV4_L4OPS_FLAG                      SOC_TMC_PMF_FLD_IPV4_L4OPS_FLAG
#define ARAD_PMF_FLD_IPV4_SIP                             SOC_TMC_PMF_FLD_IPV4_SIP
#define ARAD_PMF_FLD_IPV4_DIP                             SOC_TMC_PMF_FLD_IPV4_DIP
#define ARAD_PMF_FLD_IPV4_L4OPS_OPT                       SOC_TMC_PMF_FLD_IPV4_L4OPS_OPT
#define ARAD_PMF_FLD_IPV4_SRC_PORT                        SOC_TMC_PMF_FLD_IPV4_SRC_PORT
#define ARAD_PMF_FLD_IPV4_DEST_PORT                       SOC_TMC_PMF_FLD_IPV4_DEST_PORT
#define ARAD_PMF_FLD_IPV4_TOS                             SOC_TMC_PMF_FLD_IPV4_TOS
#define ARAD_PMF_FLD_IPV4_TCP_CTL                         SOC_TMC_PMF_FLD_IPV4_TCP_CTL
#define ARAD_PMF_FLD_IPV4_IN_AC_VRF                       SOC_TMC_PMF_FLD_IPV4_IN_AC_VRF
#define ARAD_PMF_FLD_IPV4_IN_PP_PORT                      SOC_TMC_PMF_FLD_IPV4_IN_PP_PORT
#define ARAD_PMF_FLD_IPV4_IN_VID                          SOC_TMC_PMF_FLD_IPV4_IN_VID
#define ARAD_NOF_PMF_FLD_IPV4S                            SOC_TMC_NOF_PMF_FLD_IPV4S
typedef SOC_TMC_PMF_FLD_IPV4                                   ARAD_PMF_FLD_IPV4;

#define ARAD_PMF_FLD_IPV6_L4OPS                           SOC_TMC_PMF_FLD_IPV6_L4OPS
#define ARAD_PMF_FLD_IPV6_SIP_HIGH                        SOC_TMC_PMF_FLD_IPV6_SIP_HIGH
#define ARAD_PMF_FLD_IPV6_SIP_LOW                         SOC_TMC_PMF_FLD_IPV6_SIP_LOW
#define ARAD_PMF_FLD_IPV6_DIP_HIGH                        SOC_TMC_PMF_FLD_IPV6_DIP_HIGH
#define ARAD_PMF_FLD_IPV6_DIP_LOW                         SOC_TMC_PMF_FLD_IPV6_DIP_LOW
#define ARAD_PMF_FLD_IPV6_NEXT_PRTCL                      SOC_TMC_PMF_FLD_IPV6_NEXT_PRTCL
#define ARAD_PMF_FLD_IPV6_PP_PORT_TCP_CTL                 SOC_TMC_PMF_FLD_IPV6_PP_PORT_TCP_CTL
#define ARAD_PMF_FLD_IPV6_IN_AC_VRF                       SOC_TMC_PMF_FLD_IPV6_IN_AC_VRF
#define ARAD_NOF_PMF_FLD_IPV6S                            SOC_TMC_NOF_PMF_FLD_IPV6S
typedef SOC_TMC_PMF_FLD_IPV6                                   ARAD_PMF_FLD_IPV6;

#define ARAD_PMF_TCAM_FLD_A_B_LOC_IRPP                    SOC_TMC_PMF_TCAM_FLD_A_B_LOC_IRPP
#define ARAD_PMF_TCAM_FLD_A_B_LOC_HDR                     SOC_TMC_PMF_TCAM_FLD_A_B_LOC_HDR
#define ARAD_NOF_PMF_TCAM_FLD_A_B_LOCS                    SOC_TMC_NOF_PMF_TCAM_FLD_A_B_LOCS
typedef SOC_TMC_PMF_TCAM_FLD_A_B_LOC                           ARAD_PMF_TCAM_FLD_A_B_LOC;

#define ARAD_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE               SOC_TMC_EGR_ACL_TCAM_FLD_L2_ETH_TYPE_CODE
#define ARAD_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT              SOC_TMC_EGR_ACL_TCAM_FLD_L2_ETH_TAG_FORMAT
#define ARAD_EGR_ACL_TCAM_FLD_L2_OUTER_TAG                   SOC_TMC_EGR_ACL_TCAM_FLD_L2_OUTER_TAG
#define ARAD_EGR_ACL_TCAM_FLD_L2_INNER_TAG                   SOC_TMC_EGR_ACL_TCAM_FLD_L2_INNER_TAG
#define ARAD_EGR_ACL_TCAM_FLD_L2_SA                          SOC_TMC_EGR_ACL_TCAM_FLD_L2_SA
#define ARAD_EGR_ACL_TCAM_FLD_L2_DA                          SOC_TMC_EGR_ACL_TCAM_FLD_L2_DA
#define ARAD_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA        SOC_TMC_EGR_ACL_TCAM_FLD_L2_OUT_PP_PORT_ACL_DATA
#define ARAD_EGR_NOF_ACL_TCAM_FLD_L2S                        SOC_TMC_EGR_NOF_ACL_TCAM_FLD_L2S
typedef SOC_TMC_EGR_ACL_TCAM_FLD_L2                            ARAD_EGR_ACL_TCAM_FLD_L2;

#define ARAD_EGR_ACL_TCAM_FLD_IPV4_TOS                       SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_TOS
#define ARAD_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE               SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_L4_PRO_CODE
#define ARAD_EGR_ACL_TCAM_FLD_IPV4_SIP                       SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_SIP
#define ARAD_EGR_ACL_TCAM_FLD_IPV4_DIP                       SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_DIP
#define ARAD_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID                 SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_OUTER_VID
#define ARAD_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA      SOC_TMC_EGR_ACL_TCAM_FLD_IPV4_OUT_PP_PORT_ACL_DATA
#define ARAD_EGR_NOF_ACL_TCAM_FLD_IPV4S                      SOC_TMC_EGR_NOF_ACL_TCAM_FLD_IPV4S
typedef SOC_TMC_EGR_ACL_TCAM_FLD_IPV4                          ARAD_EGR_ACL_TCAM_FLD_IPV4;

#define ARAD_EGR_ACL_TCAM_FLD_MPLS_FTMH                      SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_FTMH
#define ARAD_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA                  SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_HDR_DATA
#define ARAD_EGR_ACL_TCAM_FLD_MPLS_LABEL                     SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_LABEL
#define ARAD_EGR_ACL_TCAM_FLD_MPLS_EXP                       SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_EXP
#define ARAD_EGR_ACL_TCAM_FLD_MPLS_TTL                       SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_TTL
#define ARAD_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA      SOC_TMC_EGR_ACL_TCAM_FLD_MPLS_OUT_PP_PORT_ACL_DATA
#define ARAD_EGR_NOF_ACL_TCAM_FLD_MPLSS                      SOC_TMC_EGR_NOF_ACL_TCAM_FLD_MPLSS
typedef SOC_TMC_EGR_ACL_TCAM_FLD_MPLS                          ARAD_EGR_ACL_TCAM_FLD_MPLS;

#define ARAD_EGR_ACL_TCAM_FLD_TM_FTMH                        SOC_TMC_EGR_ACL_TCAM_FLD_TM_FTMH
#define ARAD_EGR_ACL_TCAM_FLD_TM_HDR_DATA                    SOC_TMC_EGR_ACL_TCAM_FLD_TM_HDR_DATA
#define ARAD_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA        SOC_TMC_EGR_ACL_TCAM_FLD_TM_OUT_PP_PORT_ACL_DATA
#define ARAD_EGR_NOF_ACL_TCAM_FLD_TMS                        SOC_TMC_EGR_NOF_ACL_TCAM_FLD_TMS
typedef SOC_TMC_EGR_ACL_TCAM_FLD_TM                            ARAD_EGR_ACL_TCAM_FLD_TM;

typedef SOC_TMC_TCAM_RULE_INFO                                 ARAD_TCAM_RULE_INFO;
typedef SOC_TMC_TCAM_KEY_FORMAT                                ARAD_TCAM_KEY_FORMAT;
typedef SOC_TMC_TCAM_KEY                                       ARAD_TCAM_KEY;
typedef SOC_TMC_TCAM_OUTPUT                                    ARAD_TCAM_OUTPUT;
typedef SOC_TMC_PMF_TCAM_FLD_L3                                ARAD_PMF_TCAM_FLD_L3;
typedef SOC_TMC_PMF_TCAM_FLD_A_B                               ARAD_PMF_TCAM_FLD_A_B;
typedef SOC_TMC_TCAM_KEY_FLD_TYPE                          ARAD_TCAM_KEY_FLD_TYPE;
typedef SOC_TMC_TCAM_KEY_FLD_VAL                           ARAD_TCAM_KEY_FLD_VAL;
















uint32
  arad_tcam_key_size_get(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_TCAM_KEY_FORMAT                   *key_format,
    SOC_SAND_OUT ARAD_TCAM_BANK_ENTRY_SIZE              *key_size,
    SOC_SAND_OUT uint32                             *key_size_in_bits
  );



uint32
  arad_tcam_key_clear_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_TCAM_KEY_FORMAT            *key_format,
    SOC_SAND_OUT ARAD_TCAM_KEY                   *tcam_key
  );

uint32
  arad_tcam_key_clear_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_TCAM_KEY_FORMAT             *key_format
  );


uint32
  arad_tcam_key_val_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT ARAD_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_VAL           *fld_val
  );

uint32
  arad_tcam_key_val_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT ARAD_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_VAL           *fld_val
  );

uint32
  arad_tcam_key_val_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE          *fld_type_ndx
  );


uint32
  arad_tcam_key_val_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_OUT ARAD_TCAM_KEY_FLD_VAL           *fld_val
  );


uint32
  arad_tcam_key_masked_val_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT ARAD_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_VAL           *fld_val,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_VAL           *mask_val
  );

uint32
  arad_tcam_key_masked_val_set_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_INOUT ARAD_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_VAL           *fld_val,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_VAL           *mask_val
  );

uint32
  arad_tcam_key_masked_val_get_verify(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE          *fld_type_ndx
  );


uint32
  arad_tcam_key_masked_val_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  ARAD_TCAM_KEY                   *tcam_key,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE          *fld_type_ndx,
    SOC_SAND_OUT ARAD_TCAM_KEY_FLD_VAL           *fld_val,
    SOC_SAND_OUT ARAD_TCAM_KEY_FLD_VAL           *mask_val
  );


uint32
  ARAD_TCAM_RULE_INFO_verify(
    SOC_SAND_IN  ARAD_TCAM_RULE_INFO *info
  );

uint32
  ARAD_TCAM_KEY_FORMAT_verify(
    SOC_SAND_IN  ARAD_TCAM_KEY_FORMAT *info
  );

uint32
  ARAD_TCAM_KEY_verify(
    SOC_SAND_IN  ARAD_TCAM_KEY *info
  );

uint32
  ARAD_TCAM_OUTPUT_verify(
    SOC_SAND_IN  ARAD_TCAM_OUTPUT *info
  );

uint32
  ARAD_TCAM_KEY_FLD_TYPE_verify(
    SOC_SAND_IN  ARAD_TCAM_KEY_FORMAT   *key_format,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE *info
  );

uint32
  ARAD_PMF_TCAM_FLD_L3_verify(
    SOC_SAND_IN  ARAD_PMF_TCAM_FLD_L3 *info
  );

uint32
  ARAD_PMF_TCAM_FLD_A_B_verify(
    SOC_SAND_IN  ARAD_PMF_TCAM_FLD_A_B *info
  );

uint32
  ARAD_TCAM_KEY_FLD_VAL_verify(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  ARAD_TCAM_KEY_FORMAT   *key_format,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE *fld_type,
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_VAL  *info
  );


void
  ARAD_TCAM_RULE_INFO_clear(
    SOC_SAND_OUT ARAD_TCAM_RULE_INFO *info
  );

void
  ARAD_TCAM_KEY_FORMAT_clear(
    SOC_SAND_OUT ARAD_TCAM_KEY_FORMAT *info
  );

void
  ARAD_TCAM_KEY_clear(
    SOC_SAND_OUT ARAD_TCAM_KEY *info
  );

void
  ARAD_TCAM_OUTPUT_clear(
    SOC_SAND_OUT ARAD_TCAM_OUTPUT *info
  );

void
  ARAD_TCAM_KEY_FLD_TYPE_clear(
    SOC_SAND_IN  ARAD_TCAM_KEY_FORMAT   *key_format,
    SOC_SAND_OUT ARAD_TCAM_KEY_FLD_TYPE *info
  );

void
  ARAD_PMF_TCAM_FLD_L3_clear(
    SOC_SAND_OUT ARAD_PMF_TCAM_FLD_L3 *info
  );

void
  ARAD_PMF_TCAM_FLD_A_B_clear(
    SOC_SAND_OUT ARAD_PMF_TCAM_FLD_A_B *info
  );

void
  ARAD_TCAM_KEY_FLD_VAL_clear(
    SOC_SAND_OUT ARAD_TCAM_KEY_FLD_VAL *info
  );


#if ARAD_DEBUG_IS_LVL1

const char*
  ARAD_EGR_ACL_DB_TYPE_to_string(
    SOC_SAND_IN  ARAD_EGR_ACL_DB_TYPE enum_val
  );

const char*
  ARAD_PMF_TCAM_KEY_SRC_to_string(
    SOC_SAND_IN  ARAD_PMF_TCAM_KEY_SRC enum_val
  );

const char*
  ARAD_TCAM_KEY_FORMAT_TYPE_to_string(
    SOC_SAND_IN  ARAD_TCAM_KEY_FORMAT_TYPE enum_val
  );

const char*
  ARAD_PMF_TCAM_FLD_L2_to_string(
    SOC_SAND_IN  ARAD_PMF_TCAM_FLD_L2 enum_val
  );

const char*
  ARAD_PMF_TCAM_FLD_L3_MODE_to_string(
    SOC_SAND_IN  ARAD_PMF_TCAM_FLD_L3_MODE enum_val
  );

const char*
  ARAD_PMF_FLD_IPV4_to_string(
    SOC_SAND_IN  ARAD_PMF_FLD_IPV4 enum_val
  );

const char*
  ARAD_PMF_FLD_IPV6_to_string(
    SOC_SAND_IN  ARAD_PMF_FLD_IPV6 enum_val
  );

const char*
  ARAD_PMF_TCAM_FLD_A_B_LOC_to_string(
    SOC_SAND_IN  ARAD_PMF_TCAM_FLD_A_B_LOC enum_val
  );

const char*
  ARAD_EGR_ACL_TCAM_FLD_L2_to_string(
    SOC_SAND_IN  ARAD_EGR_ACL_TCAM_FLD_L2 enum_val
  );

const char*
  ARAD_EGR_ACL_TCAM_FLD_IPV4_to_string(
    SOC_SAND_IN  ARAD_EGR_ACL_TCAM_FLD_IPV4 enum_val
  );

const char*
  ARAD_EGR_ACL_TCAM_FLD_MPLS_to_string(
    SOC_SAND_IN  ARAD_EGR_ACL_TCAM_FLD_MPLS enum_val
  );

const char*
  ARAD_EGR_ACL_TCAM_FLD_TM_to_string(
    SOC_SAND_IN  ARAD_EGR_ACL_TCAM_FLD_TM enum_val
  );

void
  ARAD_TCAM_RULE_INFO_print(
    SOC_SAND_IN  ARAD_TCAM_RULE_INFO *info
  );

void
  ARAD_TCAM_KEY_FORMAT_print(
    SOC_SAND_IN  ARAD_TCAM_KEY_FORMAT *info
  );

void
  ARAD_TCAM_KEY_print(
    SOC_SAND_IN  ARAD_TCAM_KEY *info
  );

void
  ARAD_TCAM_OUTPUT_print(
    SOC_SAND_IN  ARAD_TCAM_OUTPUT *info
  );

void
  ARAD_TCAM_KEY_FLD_TYPE_print(
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_TYPE *info
  );

void
  ARAD_PMF_TCAM_FLD_L3_print(
    SOC_SAND_IN  ARAD_PMF_TCAM_FLD_L3 *info
  );

void
  ARAD_PMF_TCAM_FLD_A_B_print(
    SOC_SAND_IN  ARAD_PMF_TCAM_FLD_A_B *info
  );

void
  ARAD_TCAM_KEY_FLD_VAL_print(
    SOC_SAND_IN  ARAD_TCAM_KEY_FLD_VAL *info
  );


#endif



#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif

