/*! \file types.h
 *
 * The headfile is to declare internal types definitions for XGS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef XGS_LTSW_TYPES_H
#define XGS_LTSW_TYPES_H

/*! CPU managed learning flags */
#define BCMI_XGS_CML_FLAGS_DROP            1
#define BCMI_XGS_CML_FLAGS_COPY_TO_CPU     2
#define BCMI_XGS_CML_FLAGS_LEARN           4

/*! EGR VFI Tag action control flags */
#define BCMI_XGS_TAG_ACT_OTAG_ADD            1
#define BCMI_XGS_TAG_ACT_ITAG_ADD            2
#define BCMI_XGS_TAG_ACT_CLASS_ID_VALID      4

/*!
 * \brief Default value for VLAN TAG Preserve control.
 */
#define BCMI_XGS_VLAN_TAG_PRESERVE_CTRL_DEF 0x3

/*! Field definition of process_ctrl in egr_dvp. */
#define BCMI_XGS_VP_PROCESS_CTRL_ITAG_DEL_SFT 0x0
#define BCMI_XGS_VP_PROCESS_CTRL_ITAG_DEL_MSK 0x1
#define BCMI_XGS_VP_PROCESS_CTRL_EVXLT2_EN_SFT 0x1
#define BCMI_XGS_VP_PROCESS_CTRL_EVXLT2_EN_MSK 0x1
#define BCMI_XGS_VP_PROCESS_CTRL_EVXLT1_DISABLE_SFT 0x2
#define BCMI_XGS_VP_PROCESS_CTRL_EVXLT1_DISABLE_MSK 0x1

#endif /* XGS_LTSW_TYPES_H */
