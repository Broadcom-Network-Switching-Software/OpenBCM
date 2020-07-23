/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

 
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC


#include <shared/bsl.h>
#include <shared/bitop.h>
#include <shared/shrextend/shrextend_debug.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/types.h>

#include <soc/dnxc/error.h>
#include <soc/dnxc/dnxc_defs.h>

#include <soc/dnxf/cmn/dnxf_cgm.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/dnxf_fabric_flow_control.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/ramon/ramon_cgm.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_device.h>


static const soc_dnxf_cgm_registers_table_t LCM_DTM_ADMIT_TAG_TH_PROFILE_description[] = {
    
    {LCM_DTM_ADMIT_TH_PROFILEr,                           {DTM_CLASS_TAG_DROP_TH_UC_0_PROFILE_Nf, DTM_CLASS_TAG_DROP_TH_UC_1_PROFILE_Nf, DTM_CLASS_TAG_DROP_TH_UC_2_PROFILE_Nf, DTM_CLASS_TAG_DROP_TH_UC_3_PROFILE_Nf, \
                                                           DTM_CLASS_TAG_DROP_TH_MC_0_PROFILE_Nf, DTM_CLASS_TAG_DROP_TH_MC_1_PROFILE_Nf, DTM_CLASS_TAG_DROP_TH_MC_2_PROFILE_Nf, DTM_CLASS_TAG_DROP_TH_MC_3_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_LINK_TH_FRAG_DROP_PROFILE_description[] = {
    
    {LCM_LINK_TH_PROFILEr,                                {LINK_PIPE_FRAG_DROP_TH_P_0_PROFILE_Nf, LINK_PIPE_FRAG_DROP_TH_P_1_PROFILE_Nf, LINK_PIPE_FRAG_DROP_TH_P_2_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_LINK_TH_GRNT_FRAG_PROFILE_description[] = {
    
    {LCM_LINK_TH_PROFILEr,                                {LINK_PIPE_GRNT_FRAG_TH_P_0_PROFILE_Nf, LINK_PIPE_GRNT_FRAG_TH_P_1_PROFILE_Nf, LINK_PIPE_GRNT_FRAG_TH_P_2_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DFL_PIPE_CLASS_SHARED_ADMIT_TH_METHOD_X_description[] = {  
    
    {LCM_DFL_PIPE_CLASS_SHARED_ADMIT_TH_METHOD_1r,        {DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_1_UC_0f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_1_UC_1f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_1_UC_2f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_1_UC_3f, \
                                                           DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_1_MC_0f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_1_MC_1f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_1_MC_2f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_1_MC_3f}},
    {LCM_DFL_PIPE_CLASS_SHARED_ADMIT_TH_METHOD_2r,        {DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_2_UC_0f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_2_UC_1f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_2_UC_2f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_2_UC_3f, \
                                                           DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_2_MC_0f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_2_MC_1f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_2_MC_2f, DFL_PIPE_CLASS_SHARED_DROP_TH_METHOD_2_MC_3f}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DFL_MC_COPIES_ADMIT_THRESHOLDS_description[] = {
    
    {LCM_DFL_MC_COPIES_ADMIT_THRESHOLDSr,                 {DFL_PIPE_COPIES_DROP_TH_MC_0f, DFL_PIPE_COPIES_DROP_TH_MC_1f, DFL_PIPE_COPIES_DROP_TH_MC_2f, DFL_PIPE_COPIES_DROP_TH_MC_3f}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTM_ADMIT_FRAG_TH_PROFILE_description[] = {
    
    {LCM_DTM_ADMIT_TH_PROFILEr,                           {DTM_CLASS_FRAG_DROP_TH_UC_0_PROFILE_Nf, DTM_CLASS_FRAG_DROP_TH_UC_1_PROFILE_Nf, DTM_CLASS_FRAG_DROP_TH_UC_2_PROFILE_Nf, DTM_CLASS_FRAG_DROP_TH_UC_3_PROFILE_Nf, \
                                                           DTM_CLASS_FRAG_DROP_TH_MC_0_PROFILE_Nf, DTM_CLASS_FRAG_DROP_TH_MC_1_PROFILE_Nf, DTM_CLASS_FRAG_DROP_TH_MC_2_PROFILE_Nf, DTM_CLASS_FRAG_DROP_TH_MC_3_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DFL_BANKS_MC_DROP_TH_description[] = {
    
    {LCM_DFL_BANKS_MC_DROP_THr,                           {DFL_BANK_MC_0_DROP_THf, DFL_BANK_MC_1_DROP_THf, DFL_BANK_MC_2_DROP_THf, DFL_BANK_MC_3_DROP_THf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTL_ADMIT_XLR_TAG_TH_PROFILE_description[] = {                 
    
    {LCM_DTL_ADMIT_NLR_TH_PROFILEr,                       {DTL_TAG_DROP_TH_NLR_UC_0_PROFILE_Nf, DTL_TAG_DROP_TH_NLR_UC_1_PROFILE_Nf, DTL_TAG_DROP_TH_NLR_UC_2_PROFILE_Nf, DTL_TAG_DROP_TH_NLR_UC_3_PROFILE_Nf, \
                                                           DTL_TAG_DROP_TH_NLR_MC_0_PROFILE_Nf, DTL_TAG_DROP_TH_NLR_MC_1_PROFILE_Nf, DTL_TAG_DROP_TH_NLR_MC_2_PROFILE_Nf, DTL_TAG_DROP_TH_NLR_MC_3_PROFILE_Nf}},
    {LCM_DTL_ADMIT_LR_TH_PROFILEr,                        {DTL_TAG_DROP_TH_LR_UC_0_PROFILE_Nf, DTL_TAG_DROP_TH_LR_UC_1_PROFILE_Nf, DTL_TAG_DROP_TH_LR_UC_2_PROFILE_Nf, DTL_TAG_DROP_TH_LR_UC_3_PROFILE_Nf, \
                                                           DTL_TAG_DROP_TH_LR_MC_0_PROFILE_Nf, DTL_TAG_DROP_TH_LR_MC_1_PROFILE_Nf, DTL_TAG_DROP_TH_LR_MC_2_PROFILE_Nf, DTL_TAG_DROP_TH_LR_MC_3_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTL_ADMIT_XLR_FRAG_TH_PROFILE_description[] = {                 
    
    {LCM_DTL_ADMIT_NLR_TH_PROFILEr,                       {DTL_FRAG_DROP_TH_NLR_UC_0_PROFILE_Nf, DTL_FRAG_DROP_TH_NLR_UC_1_PROFILE_Nf, DTL_FRAG_DROP_TH_NLR_UC_2_PROFILE_Nf, DTL_FRAG_DROP_TH_NLR_UC_3_PROFILE_Nf, \
                                                           DTL_FRAG_DROP_TH_NLR_MC_0_PROFILE_Nf, DTL_FRAG_DROP_TH_NLR_MC_1_PROFILE_Nf, DTL_FRAG_DROP_TH_NLR_MC_2_PROFILE_Nf, DTL_FRAG_DROP_TH_NLR_MC_3_PROFILE_Nf}},
    {LCM_DTL_ADMIT_LR_TH_PROFILEr,                        {DTL_FRAG_DROP_TH_LR_UC_0_PROFILE_Nf, DTL_FRAG_DROP_TH_LR_UC_1_PROFILE_Nf, DTL_FRAG_DROP_TH_LR_UC_2_PROFILE_Nf, DTL_FRAG_DROP_TH_LR_UC_3_PROFILE_Nf, \
                                                           DTL_FRAG_DROP_TH_LR_MC_0_PROFILE_Nf, DTL_FRAG_DROP_TH_LR_MC_1_PROFILE_Nf, DTL_FRAG_DROP_TH_LR_MC_2_PROFILE_Nf, DTL_FRAG_DROP_TH_LR_MC_3_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTL_GCI_TAG_TH_XLR_PROFILE_description[] = {                 
    
    {LCM_DTL_GCI_TH_P_0_NLR_PROFILEr,                     {DTL_TAG_GCI_TH_0_P_0_NLR_PROFILE_Nf, DTL_TAG_GCI_TH_1_P_0_NLR_PROFILE_Nf, DTL_TAG_GCI_TH_2_P_0_NLR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_1_NLR_PROFILEr,                     {DTL_TAG_GCI_TH_0_P_1_NLR_PROFILE_Nf, DTL_TAG_GCI_TH_1_P_1_NLR_PROFILE_Nf, DTL_TAG_GCI_TH_2_P_1_NLR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_2_NLR_PROFILEr,                     {DTL_TAG_GCI_TH_0_P_2_NLR_PROFILE_Nf, DTL_TAG_GCI_TH_1_P_2_NLR_PROFILE_Nf, DTL_TAG_GCI_TH_2_P_2_NLR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_0_LR_PROFILEr,                      {DTL_TAG_GCI_TH_0_P_0_LR_PROFILE_Nf, DTL_TAG_GCI_TH_1_P_0_LR_PROFILE_Nf, DTL_TAG_GCI_TH_2_P_0_LR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_1_LR_PROFILEr,                      {DTL_TAG_GCI_TH_0_P_1_LR_PROFILE_Nf, DTL_TAG_GCI_TH_1_P_1_LR_PROFILE_Nf, DTL_TAG_GCI_TH_2_P_1_LR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_2_LR_PROFILEr,                      {DTL_TAG_GCI_TH_0_P_2_LR_PROFILE_Nf, DTL_TAG_GCI_TH_1_P_2_LR_PROFILE_Nf, DTL_TAG_GCI_TH_2_P_2_LR_PROFILE_Nf}},
};

static const soc_dnxf_cgm_registers_table_t LCM_DTL_GCI_FRAG_TH_XLR_PROFILE_description[] = {                 
    
    {LCM_DTL_GCI_TH_P_0_NLR_PROFILEr,                     {DTL_FRAG_GCI_TH_0_P_0_NLR_PROFILE_Nf, DTL_FRAG_GCI_TH_1_P_0_NLR_PROFILE_Nf, DTL_FRAG_GCI_TH_2_P_0_NLR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_1_NLR_PROFILEr,                     {DTL_FRAG_GCI_TH_0_P_1_NLR_PROFILE_Nf, DTL_FRAG_GCI_TH_1_P_1_NLR_PROFILE_Nf, DTL_FRAG_GCI_TH_2_P_1_NLR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_2_NLR_PROFILEr,                     {DTL_FRAG_GCI_TH_0_P_2_NLR_PROFILE_Nf, DTL_FRAG_GCI_TH_1_P_2_NLR_PROFILE_Nf, DTL_FRAG_GCI_TH_2_P_2_NLR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_0_LR_PROFILEr,                      {DTL_FRAG_GCI_TH_0_P_0_LR_PROFILE_Nf, DTL_FRAG_GCI_TH_1_P_0_LR_PROFILE_Nf, DTL_FRAG_GCI_TH_2_P_0_LR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_1_LR_PROFILEr,                      {DTL_FRAG_GCI_TH_0_P_1_LR_PROFILE_Nf, DTL_FRAG_GCI_TH_1_P_1_LR_PROFILE_Nf, DTL_FRAG_GCI_TH_2_P_1_LR_PROFILE_Nf}},
    {LCM_DTL_GCI_TH_P_2_LR_PROFILEr,                      {DTL_FRAG_GCI_TH_0_P_2_LR_PROFILE_Nf, DTL_FRAG_GCI_TH_1_P_2_LR_PROFILE_Nf, DTL_FRAG_GCI_TH_2_P_2_LR_PROFILE_Nf}},
};

static const soc_dnxf_cgm_registers_table_t LCM_DTM_GCI_TAG_TH_PROFILE_description[] = {
    
    {LCM_DTM_GCI_TH_PROFILEr,                           {DTM_TAG_GCI_TH_0_P_0_PROFILE_Nf, DTM_TAG_GCI_TH_0_P_1_PROFILE_Nf, DTM_TAG_GCI_TH_0_P_2_PROFILE_Nf, \
                                                         DTM_TAG_GCI_TH_1_P_0_PROFILE_Nf, DTM_TAG_GCI_TH_1_P_1_PROFILE_Nf, DTM_TAG_GCI_TH_1_P_2_PROFILE_Nf, \
                                                         DTM_TAG_GCI_TH_2_P_0_PROFILE_Nf, DTM_TAG_GCI_TH_2_P_1_PROFILE_Nf, DTM_TAG_GCI_TH_2_P_2_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTM_GCI_FRAG_TH_PROFILE_description[] = {
    
    {LCM_DTM_GCI_TH_PROFILEr,                           {DTM_FRAG_GCI_TH_0_P_0_PROFILE_Nf, DTM_FRAG_GCI_TH_0_P_1_PROFILE_Nf, DTM_FRAG_GCI_TH_0_P_2_PROFILE_Nf, \
                                                         DTM_FRAG_GCI_TH_1_P_0_PROFILE_Nf, DTM_FRAG_GCI_TH_1_P_1_PROFILE_Nf, DTM_FRAG_GCI_TH_1_P_2_PROFILE_Nf, \
                                                         DTM_FRAG_GCI_TH_2_P_0_PROFILE_Nf, DTM_FRAG_GCI_TH_2_P_1_PROFILE_Nf, DTM_FRAG_GCI_TH_2_P_2_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DFL_GCI_TH_METHOD_X_description[] = {  
    
    {LCM_DFL_GCI_TH_METHOD_1r,                          {DFL_PIPE_GCI_METHOD_1_TH_0_P_0f, DFL_PIPE_GCI_METHOD_1_TH_0_P_1f, DFL_PIPE_GCI_METHOD_1_TH_0_P_2f, \
                                                         DFL_PIPE_GCI_METHOD_1_TH_1_P_0f, DFL_PIPE_GCI_METHOD_1_TH_1_P_1f, DFL_PIPE_GCI_METHOD_1_TH_1_P_2f, \
                                                         DFL_PIPE_GCI_METHOD_1_TH_2_P_0f, DFL_PIPE_GCI_METHOD_1_TH_2_P_1f, DFL_PIPE_GCI_METHOD_1_TH_2_P_2f}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DFL_GCI_COPIES_THRESHOLDS_description[] = {
    
    {LCM_DFL_GCI_COPIES_THRESHOLDSr,                    {DFL_PIPE_FRAG_COPIES_GCI_TH_0_P_0f, DFL_PIPE_FRAG_COPIES_GCI_TH_0_P_1f, DFL_PIPE_FRAG_COPIES_GCI_TH_0_P_2f, \
                                                         DFL_PIPE_FRAG_COPIES_GCI_TH_1_P_0f, DFL_PIPE_FRAG_COPIES_GCI_TH_1_P_1f, DFL_PIPE_FRAG_COPIES_GCI_TH_1_P_2f, \
                                                         DFL_PIPE_FRAG_COPIES_GCI_TH_2_P_0f, DFL_PIPE_FRAG_COPIES_GCI_TH_2_P_1f, DFL_PIPE_FRAG_COPIES_GCI_TH_2_P_2f }}
};

static const soc_dnxf_cgm_registers_table_t LCM_LINK_RCI_GRNT_TH_PROFILE_description[] = {
    
    {LCM_LINK_RCI_GRNT_TH_PROFILEr,                     {LINK_PIPE_GRNT_FRAG_RCI_TH_0_P_0_PROFILE_Nf, LINK_PIPE_GRNT_FRAG_RCI_TH_0_P_1_PROFILE_Nf, LINK_PIPE_GRNT_FRAG_RCI_TH_0_P_2_PROFILE_Nf, \
                                                         LINK_PIPE_GRNT_FRAG_RCI_TH_1_P_0_PROFILE_Nf, LINK_PIPE_GRNT_FRAG_RCI_TH_1_P_1_PROFILE_Nf, LINK_PIPE_GRNT_FRAG_RCI_TH_1_P_2_PROFILE_Nf, \
                                                         LINK_PIPE_GRNT_FRAG_RCI_TH_2_P_0_PROFILE_Nf, LINK_PIPE_GRNT_FRAG_RCI_TH_2_P_1_PROFILE_Nf, LINK_PIPE_GRNT_FRAG_RCI_TH_2_P_2_PROFILE_Nf }}
};

static const soc_dnxf_cgm_registers_table_t LCM_LINK_RCI_TOTAL_TH_PROFILE_description[] = {
    
    {LCM_LINK_RCI_TOTAL_TH_PROFILEr,                     {LINK_PIPE_TOTAL_FRAG_RCI_TH_0_P_0_PROFILE_Nf, LINK_PIPE_TOTAL_FRAG_RCI_TH_0_P_1_PROFILE_Nf, LINK_PIPE_TOTAL_FRAG_RCI_TH_0_P_2_PROFILE_Nf, \
                                                          LINK_PIPE_TOTAL_FRAG_RCI_TH_1_P_0_PROFILE_Nf, LINK_PIPE_TOTAL_FRAG_RCI_TH_1_P_1_PROFILE_Nf, LINK_PIPE_TOTAL_FRAG_RCI_TH_1_P_2_PROFILE_Nf, \
                                                          LINK_PIPE_TOTAL_FRAG_RCI_TH_2_P_0_PROFILE_Nf, LINK_PIPE_TOTAL_FRAG_RCI_TH_2_P_1_PROFILE_Nf, LINK_PIPE_TOTAL_FRAG_RCI_TH_2_P_2_PROFILE_Nf }}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTL_RCI_TAG_TH_XLR_PROFILE_description[] = {                 
    
    {LCM_DTL_RCI_TH_P_0_NLR_PROFILEr,                     {DTL_TAG_RCI_TH_0_P_0_NLR_PROFILE_Nf, DTL_TAG_RCI_TH_1_P_0_NLR_PROFILE_Nf, DTL_TAG_RCI_TH_2_P_0_NLR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_1_NLR_PROFILEr,                     {DTL_TAG_RCI_TH_0_P_1_NLR_PROFILE_Nf, DTL_TAG_RCI_TH_1_P_1_NLR_PROFILE_Nf, DTL_TAG_RCI_TH_2_P_1_NLR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_2_NLR_PROFILEr,                     {DTL_TAG_RCI_TH_0_P_2_NLR_PROFILE_Nf, DTL_TAG_RCI_TH_1_P_2_NLR_PROFILE_Nf, DTL_TAG_RCI_TH_2_P_2_NLR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_0_LR_PROFILEr,                      {DTL_TAG_RCI_TH_0_P_0_LR_PROFILE_Nf, DTL_TAG_RCI_TH_1_P_0_LR_PROFILE_Nf, DTL_TAG_RCI_TH_2_P_0_LR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_1_LR_PROFILEr,                      {DTL_TAG_RCI_TH_0_P_1_LR_PROFILE_Nf, DTL_TAG_RCI_TH_1_P_1_LR_PROFILE_Nf, DTL_TAG_RCI_TH_2_P_1_LR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_2_LR_PROFILEr,                      {DTL_TAG_RCI_TH_0_P_2_LR_PROFILE_Nf, DTL_TAG_RCI_TH_1_P_2_LR_PROFILE_Nf, DTL_TAG_RCI_TH_2_P_2_LR_PROFILE_Nf}},
};

static const soc_dnxf_cgm_registers_table_t LCM_DTL_RCI_FRAG_TH_XLR_PROFILE_description[] = {                 
    
    {LCM_DTL_RCI_TH_P_0_NLR_PROFILEr,                     {DTL_FRAG_RCI_TH_0_P_0_NLR_PROFILE_Nf, DTL_FRAG_RCI_TH_1_P_0_NLR_PROFILE_Nf, DTL_FRAG_RCI_TH_2_P_0_NLR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_1_NLR_PROFILEr,                     {DTL_FRAG_RCI_TH_0_P_1_NLR_PROFILE_Nf, DTL_FRAG_RCI_TH_1_P_1_NLR_PROFILE_Nf, DTL_FRAG_RCI_TH_2_P_1_NLR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_2_NLR_PROFILEr,                     {DTL_FRAG_RCI_TH_0_P_2_NLR_PROFILE_Nf, DTL_FRAG_RCI_TH_1_P_2_NLR_PROFILE_Nf, DTL_FRAG_RCI_TH_2_P_2_NLR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_0_LR_PROFILEr,                      {DTL_FRAG_RCI_TH_0_P_0_LR_PROFILE_Nf, DTL_FRAG_RCI_TH_1_P_0_LR_PROFILE_Nf, DTL_FRAG_RCI_TH_2_P_0_LR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_1_LR_PROFILEr,                      {DTL_FRAG_RCI_TH_0_P_1_LR_PROFILE_Nf, DTL_FRAG_RCI_TH_1_P_1_LR_PROFILE_Nf, DTL_FRAG_RCI_TH_2_P_1_LR_PROFILE_Nf}},
    {LCM_DTL_RCI_TH_P_2_LR_PROFILEr,                      {DTL_FRAG_RCI_TH_0_P_2_LR_PROFILE_Nf, DTL_FRAG_RCI_TH_1_P_2_LR_PROFILE_Nf, DTL_FRAG_RCI_TH_2_P_2_LR_PROFILE_Nf}},
};

static const soc_dnxf_cgm_registers_table_t LCM_DTM_RCI_TAG_TH_PROFILE_description[] = {
    
    {LCM_DTM_RCI_TH_PROFILEr,                            {DTM_TAG_RCI_TH_0_P_0_PROFILE_Nf, DTM_TAG_RCI_TH_0_P_1_PROFILE_Nf, DTM_TAG_RCI_TH_0_P_2_PROFILE_Nf, \
                                                          DTM_TAG_RCI_TH_1_P_0_PROFILE_Nf, DTM_TAG_RCI_TH_1_P_1_PROFILE_Nf, DTM_TAG_RCI_TH_1_P_2_PROFILE_Nf, \
                                                          DTM_TAG_RCI_TH_2_P_0_PROFILE_Nf, DTM_TAG_RCI_TH_2_P_1_PROFILE_Nf, DTM_TAG_RCI_TH_2_P_2_PROFILE_Nf }}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTM_RCI_FRAG_TH_PROFILE_description[] = {
    
    {LCM_DTM_RCI_TH_PROFILEr,                            {DTM_FRAG_RCI_TH_0_P_0_PROFILE_Nf, DTM_FRAG_RCI_TH_0_P_1_PROFILE_Nf, DTM_FRAG_RCI_TH_0_P_2_PROFILE_Nf, \
                                                          DTM_FRAG_RCI_TH_1_P_0_PROFILE_Nf, DTM_FRAG_RCI_TH_1_P_1_PROFILE_Nf, DTM_FRAG_RCI_TH_1_P_2_PROFILE_Nf, \
                                                          DTM_FRAG_RCI_TH_2_P_0_PROFILE_Nf, DTM_FRAG_RCI_TH_2_P_1_PROFILE_Nf, DTM_FRAG_RCI_TH_2_P_2_PROFILE_Nf }}
};

static const soc_dnxf_cgm_registers_table_t LCM_DFL_RCI_TH_METHOD_X_description[] = {  
    
    {LCM_DFL_RCI_TH_METHOD_1r,                          {DFL_PIPE_RCI_METHOD_1_TH_0_P_0f, DFL_PIPE_RCI_METHOD_1_TH_0_P_1f, DFL_PIPE_RCI_METHOD_1_TH_0_P_2f, \
                                                         DFL_PIPE_RCI_METHOD_1_TH_1_P_0f, DFL_PIPE_RCI_METHOD_1_TH_1_P_1f, DFL_PIPE_RCI_METHOD_1_TH_1_P_2f, \
                                                         DFL_PIPE_RCI_METHOD_1_TH_2_P_0f, DFL_PIPE_RCI_METHOD_1_TH_2_P_1f, DFL_PIPE_RCI_METHOD_1_TH_2_P_2f,}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTM_COPIES_RTP_FC_THRESHOLDS_description[] = {
    
    {LCM_DTM_COPIES_RTP_FC_THRESHOLDSr,                 {DTM_COPIES_RTP_FC_TH_MC_0f, DTM_COPIES_RTP_FC_TH_MC_1f, DTM_COPIES_RTP_FC_TH_MC_2f, DTM_COPIES_RTP_FC_TH_MC_3f, }}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTM_FC_TAG_TH_PROFILE_description[] = {
    
    {LCM_DTM_FC_TH_PROFILEr,                            {DTM_RTP_TAG_FC_TH_P_0_PROFILE_Nf, DTM_RTP_TAG_FC_TH_P_1_PROFILE_Nf, DTM_RTP_TAG_FC_TH_P_2_PROFILE_Nf }}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTM_FC_FRAG_TH_PROFILE_description[] = {
    
    {LCM_DTM_FC_TH_PROFILEr,                            {DTM_RTP_FRAG_FC_TH_P_0_PROFILE_Nf, DTM_RTP_FRAG_FC_TH_P_1_PROFILE_Nf, DTM_RTP_FRAG_FC_TH_P_2_PROFILE_Nf }}
};

static const soc_dnxf_cgm_registers_table_t LCM_DFL_FC_TH_METHOD_X_description[] = {  
    
    {LCM_DFL_FC_TH_METHOD_1r,                           {DFL_PIPE_SHARED_FC_TH_METHOD_1_P_0f, DFL_PIPE_SHARED_FC_TH_METHOD_1_P_1f, DFL_PIPE_SHARED_FC_TH_METHOD_1_P_2f}},
    {LCM_DFL_FC_TH_METHOD_2r,                           {DFL_PIPE_SHARED_FC_TH_METHOD_2_P_0f, DFL_PIPE_SHARED_FC_TH_METHOD_2_P_1f, DFL_PIPE_SHARED_FC_TH_METHOD_2_P_2f}},
};

static const soc_dnxf_cgm_registers_table_t LCM_DTL_TAG_TH_XLR_PROFILE_description[] = {                 
    
    {LCM_DTL_FC_TH_NLR_PROFILEr,                         {DTL_TO_DTM_TAG_FC_TH_P_0_NLR_PROFILE_Nf, DTL_TO_DTM_TAG_FC_TH_P_1_NLR_PROFILE_Nf, DTL_TO_DTM_TAG_FC_TH_P_2_NLR_PROFILE_Nf}},
    {LCM_DTL_FC_TH_LR_PROFILEr,                          {DTL_TO_DTM_TAG_FC_TH_P_0_LR_PROFILE_Nf, DTL_TO_DTM_TAG_FC_TH_P_1_LR_PROFILE_Nf, DTL_TO_DTM_TAG_FC_TH_P_2_LR_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTL_FRAG_TH_XLR_PROFILE_description[] = {                 
    
    {LCM_DTL_FC_TH_NLR_PROFILEr,                         {DTL_TO_DTM_FRAG_FC_TH_P_0_NLR_PROFILE_Nf, DTL_TO_DTM_FRAG_FC_TH_P_1_NLR_PROFILE_Nf, DTL_TO_DTM_FRAG_FC_TH_P_2_NLR_PROFILE_Nf}},
    {LCM_DTL_FC_TH_LR_PROFILEr,                          {DTL_TO_DTM_FRAG_FC_TH_P_0_LR_PROFILE_Nf, DTL_TO_DTM_FRAG_FC_TH_P_1_LR_PROFILE_Nf, DTL_TO_DTM_FRAG_FC_TH_P_2_LR_PROFILE_Nf}}
};

static const soc_dnxf_cgm_registers_table_t LCM_DTM_FC_TH_PROFILE_description[] = {
    
    {LCM_DTM_FC_TH_PROFILEr,                             {DTM_MASK_FC_TH_P_0_PROFILE_Nf, DTM_MASK_FC_TH_P_1_PROFILE_Nf, DTM_MASK_FC_TH_P_2_PROFILE_Nf }}
};

static const soc_dnxf_cgm_registers_table_t LCM_LINK_FC_TH_PROFILE_description[] = {
    
    {LCM_LINK_FC_TH_PROFILEr,                             {LINK_PIPE_FRAG_DCH_FC_TH_P_0_PROFILE_Nf, LINK_PIPE_FRAG_DCH_FC_TH_P_1_PROFILE_Nf, LINK_PIPE_FRAG_DCH_FC_TH_P_2_PROFILE_Nf }}
};

static const soc_dnxf_cgm_registers_table_t DCH_LINK_LEVEL_FLOW_CONTROL_description[] = {
    
    {DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,                    {IFM_FIFO_FULL_P_0f }}, \
    {DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,                    {IFM_FIFO_FULL_P_1f }}, \
    {DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,                    {IFM_FIFO_FULL_P_2f }}
};

static const soc_dnxf_cgm_registers_table_t DCH_LINK_LEVEL_FLOW_CONTROL_LOW_PRIO_CELLS_description[] = {
    
    {DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,                    {LOW_PRI_CELL_DROP_TH_P_0f }}, \
    {DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,                    {LOW_PRI_CELL_DROP_TH_P_1f }}, \
    {DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,                    {LOW_PRI_CELL_DROP_TH_P_2f }}
};

static const soc_dnxf_cgm_registers_table_t DCH_LOCAL_GCI_TYPE_0_P_1_P_description[] = {
    
    {DCH_LOCAL_GCI_TYPE_0_Pr,                            {LOCAL_GCI_0_TYPE_0_TH_P_Nf, LOCAL_GCI_1_TYPE_0_TH_P_Nf, LOCAL_GCI_2_TYPE_0_TH_P_Nf }},
    {DCH_LOCAL_GCI_TYPE_1_Pr,                            {LOCAL_GCI_0_TYPE_1_TH_P_Nf, LOCAL_GCI_1_TYPE_1_TH_P_Nf, LOCAL_GCI_2_TYPE_1_TH_P_Nf }}
};

static const soc_dnxf_cgm_registers_table_t DCH_LINK_LEVEL_FLOW_CONTROL_P_X_description[] = {
    
    {DCH_LINK_LEVEL_FLOW_CONTROL_P_0r,                   {LNK_LVL_FC_TH_0_P_0f, LNK_LVL_FC_TH_1_P_0f}},
    {DCH_LINK_LEVEL_FLOW_CONTROL_P_1r,                   {LNK_LVL_FC_TH_0_P_1f, LNK_LVL_FC_TH_1_P_1f}},
    {DCH_LINK_LEVEL_FLOW_CONTROL_P_2r,                   {LNK_LVL_FC_TH_0_P_2f, LNK_LVL_FC_TH_1_P_2f}}
};

static const soc_dnxf_cgm_registers_table_t DCH_WFQ_DYN_WEIGHT_THD_description[] = {
    
    {DCH_WFQ_DYN_WEIGHT_THD_REGISTEr,                    {WFQ_DYN_WEIGHT_THD_P_0f, WFQ_DYN_WEIGHT_THD_P_1f}},
};

static const soc_dnxf_cgm_registers_table_t DTM_WFQ_DYN_WEIGHT_THD_description[] = {
    
    {LCM_DTM_WFQ_DYN_WEIGHT_THD_REGISTERr,                {DTM_WFQ_DYN_WEIGHT_THD_P_0f, DTM_WFQ_DYN_WEIGHT_THD_P_1f}},
    {LCM_DTM_LCLRT_WFQ_DYN_WEIGHT_THD_REGISTERr,          {DTM_LCLRT_WFQ_DYN_WEIGHT_THD_P_0f, DTM_LCLRT_WFQ_DYN_WEIGHT_THD_P_1f}},
};

static const soc_dnxf_cgm_registers_table_t DTL_WFQ_DYN_WEIGHT_THD_description[] = {
    
    {LCM_DTL_WFQ_DYN_WEIGHT_THD_REGISTERr,                {DTL_WFQ_DYN_WEIGHT_THD_P_0f, DTL_WFQ_DYN_WEIGHT_THD_P_1f }},
    {LCM_DTL_LCLRT_WFQ_DYN_WEIGHT_THD_REGISTERr,          {DTL_LCLRT_WFQ_DYN_WEIGHT_THD_P_0f, DTL_LCLRT_WFQ_DYN_WEIGHT_THD_P_1f}},
};

const soc_dnxf_cgm_threshold_to_reg_binding_t ramon_soc_cgm_threshold_to_register_table[] = {
    
    {_SHR_FABRIC_MID_TAG_DROP_CLASS_TH,            soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_ADMIT_TAG_TH_PROFILE_description                },
    {_SHR_FABRIC_TX_LINK_LOAD_DROP_PIPE_TH,        soc_dnxf_cgm_reg_structure_1,      1,                  LCM_LINK_TH_FRAG_DROP_PROFILE_description               },
    {_SHR_FABRIC_TX_FRAG_GUARANTEED_PIPE_TH,       soc_dnxf_cgm_reg_structure_1,      1,                  LCM_LINK_TH_GRNT_FRAG_PROFILE_description               },
    {_SHR_FABRIC_SHARED_DROP_CLASS_TH,             soc_dnxf_cgm_reg_structure_1,      2,                  LCM_DFL_PIPE_CLASS_SHARED_ADMIT_TH_METHOD_X_description }, 
    {_SHR_FABRIC_SHARED_MC_COPIES_DROP_PRIO_TH,    soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DFL_MC_COPIES_ADMIT_THRESHOLDS_description          },
    {_SHR_FABRIC_MID_FRAG_DROP_CLASS_TH,           soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_ADMIT_FRAG_TH_PROFILE_description               },
    {_SHR_FABRIC_SHARED_BANK_MC_DROP_PRIO_TH,      soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DFL_BANKS_MC_DROP_TH_description                    },
    {_SHR_FABRIC_TX_TAG_DROP_CLASS_TH,             soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTL_ADMIT_XLR_TAG_TH_PROFILE_description            }, 
    {_SHR_FABRIC_TX_FRAG_DROP_CLASS_TH,            soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTL_ADMIT_XLR_FRAG_TH_PROFILE_description           }, 
    {_SHR_FABRIC_TX_TAG_GCI_PIPE_TH,               soc_dnxf_cgm_reg_structure_2,      1,                  LCM_DTL_GCI_TAG_TH_XLR_PROFILE_description              }, 
    {_SHR_FABRIC_TX_FRAG_GCI_PIPE_TH,              soc_dnxf_cgm_reg_structure_2,      1,                  LCM_DTL_GCI_FRAG_TH_XLR_PROFILE_description             }, 
    {_SHR_FABRIC_MID_TAG_GCI_PIPE_TH,              soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_GCI_TAG_TH_PROFILE_description                  },
    {_SHR_FABRIC_MID_FRAG_GCI_PIPE_TH,             soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_GCI_FRAG_TH_PROFILE_description                 },
    {_SHR_FABRIC_SHARED_GCI_PIPE_TH,               soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DFL_GCI_TH_METHOD_X_description                     }, 
    {_SHR_FABRIC_SHARED_FRAG_COPIES_GCI_PIPE_TH,   soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DFL_GCI_COPIES_THRESHOLDS_description               },
    {_SHR_FABRIC_TX_FRAG_GUARANTEED_RCI_PIPE_TH,   soc_dnxf_cgm_reg_structure_1,      1,                  LCM_LINK_RCI_GRNT_TH_PROFILE_description                },
    {_SHR_FABRIC_TX_LINK_LOAD_RCI_PIPE_TH,         soc_dnxf_cgm_reg_structure_1,      1,                  LCM_LINK_RCI_TOTAL_TH_PROFILE_description               },
    {_SHR_FABRIC_TX_TAG_RCI_PIPE_TH,               soc_dnxf_cgm_reg_structure_2,      1,                  LCM_DTL_RCI_TAG_TH_XLR_PROFILE_description              }, 
    {_SHR_FABRIC_TX_FRAG_RCI_PIPE_TH,              soc_dnxf_cgm_reg_structure_2,      1,                  LCM_DTL_RCI_FRAG_TH_XLR_PROFILE_description             }, 
    {_SHR_FABRIC_MID_TAG_RCI_PIPE_TH,              soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_RCI_TAG_TH_PROFILE_description                  },
    {_SHR_FABRIC_MID_FRAG_RCI_PIPE_TH,             soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_RCI_FRAG_TH_PROFILE_description                 },
    {_SHR_FABRIC_SHARED_RCI_PIPE_TH,               soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DFL_RCI_TH_METHOD_X_description                     }, 
    {_SHR_FABRIC_MID_MC_COPIES_FC_PRIO_TH,         soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_COPIES_RTP_FC_THRESHOLDS_description            },
    {_SHR_FABRIC_MID_TAG_FC_PIPE_TH,               soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_FC_TAG_TH_PROFILE_description                   },
    {_SHR_FABRIC_MID_FRAG_FC_PIPE_TH,              soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_FC_FRAG_TH_PROFILE_description                  },
    {_SHR_FABRIC_SHARED_FC_PIPE_TH,                soc_dnxf_cgm_reg_structure_1,      2,                  LCM_DFL_FC_TH_METHOD_X_description                      }, 
    {_SHR_FABRIC_TX_TAG_FC_PIPE_TH,                soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTL_TAG_TH_XLR_PROFILE_description                  }, 
    {_SHR_FABRIC_TX_FRAG_FC_PIPE_TH,               soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTL_FRAG_TH_XLR_PROFILE_description                 }, 
    {_SHR_FABRIC_MID_MASK_FC_PIPE_TH,              soc_dnxf_cgm_reg_structure_1,      1,                  LCM_DTM_FC_TH_PROFILE_description                       },
    {_SHR_FABRIC_TX_FE1_BYPASS_LLFCFC_PIPE_TH,     soc_dnxf_cgm_reg_structure_1,      1,                  LCM_LINK_FC_TH_PROFILE_description                      },
    {_SHR_FABRIC_RX_FRAG_DROP_PIPE_TH,             soc_dnxf_cgm_reg_structure_2,      1,                  DCH_LINK_LEVEL_FLOW_CONTROL_description                 }, 
    {_SHR_FABRIC_RX_MC_LOW_PRIO_DROP_PIPE_TH,      soc_dnxf_cgm_reg_structure_2,      1,                  DCH_LINK_LEVEL_FLOW_CONTROL_LOW_PRIO_CELLS_description  },  
    {_SHR_FABRIC_RX_FRAG_GCI_PIPE_TH,              soc_dnxf_cgm_reg_structure_3,      1,                  DCH_LOCAL_GCI_TYPE_0_P_1_P_description                  },
    {_SHR_FABRIC_RX_LLFCFC_PIPE_TH,                soc_dnxf_cgm_reg_structure_4,      1,                  DCH_LINK_LEVEL_FLOW_CONTROL_P_X_description             },
    {_SHR_FABRIC_RX_WFQ_DYNAMIC_WEIGHT_PIPE_TH,    soc_dnxf_cgm_reg_structure_1,      1,                  DCH_WFQ_DYN_WEIGHT_THD_description                      },
    {_SHR_FABRIC_MID_WFQ_DYNAMIC_WEIGHT_PIPE_TH,   soc_dnxf_cgm_reg_structure_1,      1,                  DTM_WFQ_DYN_WEIGHT_THD_description                      },
    {_SHR_FABRIC_TX_WFQ_DYNAMIC_WEIGHT_PIPE_TH,    soc_dnxf_cgm_reg_structure_1,      1,                  DTL_WFQ_DYN_WEIGHT_THD_description                      }
};


_shr_error_t soc_ramon_cgm_threshold_to_reg_table_get(
    int unit,
    const soc_dnxf_cgm_threshold_to_reg_binding_t **reg_binding_table)
{
    SHR_FUNC_INIT_VARS(unit);

    *reg_binding_table = ramon_soc_cgm_threshold_to_register_table;

    SHR_FUNC_EXIT;
}

_shr_error_t soc_ramon_cgm_link_profile_set(
    int unit,
    int profile_id,
    uint32 flags,
    uint32 links_count,
    soc_port_t* links)
{

    int blk_id, ii;
    soc_port_t link, inner_link;
    uint32 regs_values_link_th_profiles_mapping[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH];
    uint32 regs_values_dtl_th_profiles_mapping[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH];
    uint32 regs_values_dtl_link_type[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH];
    uint32 regs_values_dtl_lclrt_link_type[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH];
    uint32 regs_values_llfc_bmp[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH];

    SHR_FUNC_INIT_VARS(unit);

    for (blk_id = 0; blk_id < dnxf_data_device.blocks.nof_instances_dch_get(unit); blk_id++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_LINK_TH_PROFILES_MAPPINGr, blk_id, 0, &regs_values_link_th_profiles_mapping[blk_id]));
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_DTL_TH_PROFILES_MAPPINGr, blk_id, 0, &regs_values_dtl_th_profiles_mapping[blk_id]));
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_DTL_LINK_TYPEr, blk_id, 0, &regs_values_dtl_link_type[blk_id]));
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_DTL_LCLRT_LINK_TYPEr, blk_id, 0, &regs_values_dtl_lclrt_link_type[blk_id]));
        SHR_IF_ERR_EXIT(soc_reg32_get(unit, DCH_LINK_LEVEL_FLOW_CONTROL_BMPr, blk_id, 0, &regs_values_llfc_bmp[blk_id]));
    }

    
    for (ii = 0; ii<links_count ; ii++)
    {
        link=links[ii];

        
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, link, &blk_id, &inner_link, SOC_BLK_DCH)));

        
        if (profile_id == 0)
        {
           SHR_BITCLR(&regs_values_link_th_profiles_mapping[blk_id], inner_link);
           SHR_BITCLR(&regs_values_dtl_th_profiles_mapping[blk_id], inner_link);
           SHR_BITCLR(&regs_values_dtl_link_type[blk_id], inner_link);
           SHR_BITCLR(&regs_values_dtl_lclrt_link_type[blk_id], inner_link);
           SHR_BITCLR(&regs_values_llfc_bmp[blk_id], inner_link);
        }
        else
        {
           SHR_BITSET(&regs_values_link_th_profiles_mapping[blk_id], inner_link);
           SHR_BITSET(&regs_values_dtl_th_profiles_mapping[blk_id], inner_link);
           SHR_BITSET(&regs_values_dtl_link_type[blk_id], inner_link);
           SHR_BITSET(&regs_values_dtl_lclrt_link_type[blk_id], inner_link);
           SHR_BITSET(&regs_values_llfc_bmp[blk_id], inner_link);
        }

    }

    for (blk_id = 0; blk_id < dnxf_data_device.blocks.nof_instances_dch_get(unit); blk_id++)
    {
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_LINK_TH_PROFILES_MAPPINGr, blk_id, 0, regs_values_link_th_profiles_mapping[blk_id]));
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_DTL_TH_PROFILES_MAPPINGr, blk_id, 0, regs_values_dtl_th_profiles_mapping[blk_id]));
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_DTL_LINK_TYPEr, blk_id, 0, regs_values_dtl_link_type[blk_id]));
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, LCM_DTL_LCLRT_LINK_TYPEr, blk_id, 0, regs_values_dtl_lclrt_link_type[blk_id]));
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, DCH_LINK_LEVEL_FLOW_CONTROL_BMPr, blk_id, 0, regs_values_llfc_bmp[blk_id]));
    }

exit:
    SHR_FUNC_EXIT;
}

_shr_error_t soc_ramon_cgm_link_profile_get(
    int unit,
    int profile_id,
    uint32 flags,
    uint32 links_count_max,
    uint32 *links_count,
    soc_port_t *links)
{

    int blk_id, actual_profile;
    soc_port_t link, inner_link;
    uint32 regs_values_link_th_profiles_mapping[DNXF_DATA_MAX_DEVICE_BLOCKS_NOF_INSTANCES_DCH];
    uint32 local_links_count=0;

    SHR_FUNC_INIT_VARS(unit);

    
    
    for (blk_id = 0; blk_id < dnxf_data_device.blocks.nof_instances_dch_get(unit); blk_id++)
    {
       SHR_IF_ERR_EXIT(soc_reg32_get(unit, LCM_LINK_TH_PROFILES_MAPPINGr, blk_id, 0, &regs_values_link_th_profiles_mapping[blk_id]));
    }

    SOC_PBMP_ITER(SOC_INFO(unit).sfi.bitmap, link)
    {

        
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, link, &blk_id, &inner_link, SOC_BLK_DCH)));

        if (SHR_BITGET(&regs_values_link_th_profiles_mapping[blk_id], inner_link))
        {
            actual_profile = 1;
        } else {
            actual_profile = 0;
        }

        if(actual_profile == profile_id)
        {
            if(local_links_count >= links_count_max)
            {
                local_links_count = 0;
                SHR_ERR_EXIT(_SHR_E_FULL, "links_count_max %d is too small",links_count_max);
            }
            links[local_links_count] = link;
            (local_links_count)++;
        }
    }

    *links_count = local_links_count;

exit:
    SHR_FUNC_EXIT;
}



_shr_error_t soc_ramon_cgm_rci_resolution_set(
        int unit,
        soc_dnxf_fabric_rci_resolution_key_t *key,
        soc_dnxf_fabric_rci_resolution_config_t *config)
{
    int pipe_index, lcm_index;
    uint32 pipe, shared_rci, guaranteed_rci, resolved_rci;
    uint32 pipe_min, pipe_max;
    uint32 nof_rci_bits, nof_rci_levels;
    uint32 rci_tbl_data, rci_tbl_resolved_index;
    uint32 reg_val32;
    int nof_instances_lcm;
    SHR_FUNC_INIT_VARS(unit);

    pipe           = key->pipe;
    shared_rci     = key->shared_rci;
    guaranteed_rci = key->guaranteed_rci;
    resolved_rci   = config->resolved_rci;

    
    nof_rci_bits = dnxf_data_fabric.congestion.nof_rci_bits_get(unit);
    nof_rci_levels = (1 << nof_rci_bits);

    if (pipe == _SHR_FABRIC_PIPE_ALL)
    {
        pipe_min = 0;
        pipe_max = dnxf_data_fabric.pipes.nof_pipes_get(unit) -1;
    }
    else
    {
        pipe_min = pipe;
        pipe_max = pipe;
    }

    
    rci_tbl_resolved_index = (shared_rci * nof_rci_levels + guaranteed_rci) * nof_rci_bits;

    for (pipe_index = pipe_min ; pipe_index <= pipe_max ; pipe_index++)
    {
        SHR_IF_ERR_EXIT(READ_LCM_RCI_TABLE_PIPEr(unit, 0, pipe_index, &reg_val32));

        
        rci_tbl_data = soc_reg_field_get(unit, LCM_RCI_TABLE_PIPEr, reg_val32, RCI_TABLE_PIPE_Nf);
        SHR_BITCOPY_RANGE(&rci_tbl_data, rci_tbl_resolved_index, &resolved_rci, 0, nof_rci_bits);
        soc_reg_field_set(unit, LCM_RCI_TABLE_PIPEr, &reg_val32, RCI_TABLE_PIPE_Nf, rci_tbl_data);

        if (!SAL_BOOT_PLISIM)
        {
            SHR_IF_ERR_EXIT(WRITE_BRDC_LCM_RCI_TABLE_PIPEr(unit, pipe_index, reg_val32));
        }
        else
        {
            nof_instances_lcm = dnxf_data_device.blocks.nof_instances_lcm_get(unit);
            for (lcm_index = 0 ; lcm_index < nof_instances_lcm ; lcm_index++)
            {
                SHR_IF_ERR_EXIT(WRITE_LCM_RCI_TABLE_PIPEr(unit, lcm_index, pipe_index, reg_val32));
            }
        }
    }


exit:
    SHR_FUNC_EXIT;
}



_shr_error_t soc_ramon_cgm_rci_resolution_get(
        int unit,
        soc_dnxf_fabric_rci_resolution_key_t *key,
        soc_dnxf_fabric_rci_resolution_config_t *config)
{
    uint32 pipe, shared_rci, guaranteed_rci, resolved_rci;
    uint32 nof_rci_bits, nof_rci_levels;
    uint32 rci_tbl_data, rci_tbl_resolved_index;
    uint32 reg_val32;
    SHR_FUNC_INIT_VARS(unit);

    pipe           = key->pipe;
    shared_rci     = key->shared_rci;
    guaranteed_rci = key->guaranteed_rci;

    
    nof_rci_bits = dnxf_data_fabric.congestion.nof_rci_bits_get(unit);
    nof_rci_levels = (1 << nof_rci_bits);

    if (pipe == _SHR_FABRIC_PIPE_ALL)
    {
        pipe = 0;
    }

    
    rci_tbl_resolved_index = (shared_rci * nof_rci_levels + guaranteed_rci) * nof_rci_bits;

    SHR_IF_ERR_EXIT(READ_LCM_RCI_TABLE_PIPEr(unit, 0, pipe, &reg_val32));
    rci_tbl_data = soc_reg_field_get(unit, LCM_RCI_TABLE_PIPEr, reg_val32, RCI_TABLE_PIPE_Nf);

    resolved_rci = 0;
    
    SHR_BITCOPY_RANGE(&resolved_rci, 0, &rci_tbl_data, rci_tbl_resolved_index, nof_rci_bits);
    config->resolved_rci = resolved_rci;

exit:
    SHR_FUNC_EXIT;
}


