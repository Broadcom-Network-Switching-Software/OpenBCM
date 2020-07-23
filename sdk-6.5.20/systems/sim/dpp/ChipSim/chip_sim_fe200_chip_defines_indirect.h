/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


#ifndef __FE_CHIP_DEFINES_INDIRECT_H__
/* { */
#define __FE_CHIP_DEFINES_INDIRECT_H__

#ifdef _MSC_VER
  /*
   * On MS-Windows platform this attribute is not defined.
   */
  #define __ATTRIBUTE_PACKED__
  #pragma pack(push)
  #pragma pack(1)

#else
  #ifdef __GNUC__
    /*
     * GNUC packing attribute
     */
    #define __ATTRIBUTE_PACKED__  __attribute__ ((packed))
  #else
    #error  "Add your system support for packed attribute."
  #endif
#endif

#include "chip_sim_fe200_chip_consts.h"

#define FE_INDIRECT_BASE 0x30000000


#define FE_ROUTING_MS_BIT    31
#define FE_ROUTING_NUM_BITS  32
#define FE_ROUTING_LS_BIT   (FE_ROUTING_MS_BIT + 1 - FE_ROUTING_NUM_BITS)
#define FE_ROUTING_MASK   \
    (((unsigned long)BIT(FE_ROUTING_MS_BIT) - BIT(FE_ROUTING_LS_BIT)) + \
    BIT(FE_ROUTING_MS_BIT))
#define FE_ROUTING_SHIFT     FE_ROUTING_LS_BIT

/*
 * the size of the first spatail table 2048*4 = 8192 = 0x2000
 */
#define FE_DRM_TABLE_SIZE   (0x2000)

typedef struct
{
  unsigned char   route[FE_DRM_TABLE_SIZE] ;
} __ATTRIBUTE_PACKED__ FE_DRM_TABLE_DETAIL  ;
typedef union
{
  FE_DRM_TABLE_DETAIL   detail ;
  unsigned char         fill[FE_DRM_TABLE_SIZE] ;
} __ATTRIBUTE_PACKED__ FE_DRM_TABLE_REGS  ;


/*
 * the size of the first spatail table 2048*4 = 8192 = 0x2000
 */
#define FE_MULTICAST_TABLE_A_SIZE   SOC_SAND_NOF_SPATIAL_MULTI_GROUPS

typedef struct
{
  unsigned char   group[SOC_SAND_NOF_SPATIAL_MULTI_GROUPS] ;
} __ATTRIBUTE_PACKED__ FE_MULTICAST_TABLE_A_DETAIL  ;
typedef union
{
  FE_MULTICAST_TABLE_A_DETAIL   detail ;
  unsigned char            fill[FE_MULTICAST_TABLE_A_SIZE] ;
} __ATTRIBUTE_PACKED__ FE_MULTICAST_TABLE_A_REGS  ;

/*
 * the size of the second spatail table 2048*4 = 8192 = 0x2000
 */
#define FE_MULTICAST_TABLE_B_SIZE   (SOC_SAND_NOF_SPATIAL_MULTI_GROUPS)
typedef struct
{
  unsigned char   group[SOC_SAND_NOF_SPATIAL_MULTI_GROUPS] ;
} __ATTRIBUTE_PACKED__ FE_MULTICAST_TABLE_B_DETAIL  ;
typedef union
{
  FE_MULTICAST_TABLE_B_DETAIL   detail ;
  unsigned char                 fill[FE_MULTICAST_TABLE_B_SIZE] ;
} __ATTRIBUTE_PACKED__ FE_MULTICAST_TABLE_B_REGS  ;

/*
 * the size of the routing table 2048*4 = 8192 = 0x2000
 */
#define FE_ROUTING_TABLE_SIZE   (SOC_SAND_MAX_NOF_PIPES)
typedef struct
{
  unsigned char   route[SOC_SAND_MAX_NOF_PIPES] ;
} __ATTRIBUTE_PACKED__ FE_ROUTING_TABLE_DETAIL  ;
typedef union
{
  FE_ROUTING_TABLE_DETAIL  detail ;
  unsigned char            fill[FE_ROUTING_TABLE_SIZE] ;
} __ATTRIBUTE_PACKED__ FE_ROUTING_TABLE_REGS  ;

/*
 * the size of the FE3 routing table 32*4 = 128 = 0x80
 */
#define FE3_UNI_ROUTING_TABLE_SIZE   (FE_NUM_LINKS_IN_FE13)
typedef struct
{
  unsigned char   route[FE_NUM_LINKS_IN_FE13] ;
} __ATTRIBUTE_PACKED__ FE3_UNI_ROUTING_TABLE_DETAIL  ;
typedef union
{
  FE3_UNI_ROUTING_TABLE_DETAIL  detail ;
  unsigned char                 fill[FE3_UNI_ROUTING_TABLE_SIZE] ;
} __ATTRIBUTE_PACKED__ FE3_UNI_ROUTING_TABLE_REGS  ;



typedef struct
{
  FE_DRM_TABLE_REGS             fe_drm_tables;              /* 0x30000000 - 0x30001FFF */
  FE_MULTICAST_TABLE_A_REGS     fe_multicast_table_a_regs ; /* 0x30002000 - 0x300027FF */
  FE_MULTICAST_TABLE_B_REGS     fe_multicast_table_b_regs ; /* 0x30002800 - 0x30002FFF */
  FE_ROUTING_TABLE_REGS         fe_routing_table_regs ;     /* 0x30003000 - 0x300037FF */
  FE3_UNI_ROUTING_TABLE_REGS    fe3_uni_routing_table_regs; /* 0x300037FF - 0x3000381F */
} __ATTRIBUTE_PACKED__ FE_INDIRECT_REGS ;


#ifdef _MSC_VER
  #pragma pack(pop)
#endif

/* } __FE_CHIP_DEFINES_INDIRECT_H__*/
#endif
