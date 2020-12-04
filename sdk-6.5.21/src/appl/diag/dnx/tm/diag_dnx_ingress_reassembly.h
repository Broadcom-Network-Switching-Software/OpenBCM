/** \file diag_dnx_ingress_reassembly.h
 * 
 * DNX TM ingress reassembly diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef DIAG_DNX_INGRESS_REASSEMBLY_H_INCLUDED
/*
 * {
 */
#define DIAG_DNX_INGRESS_REASSEMBLY_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

extern sh_sand_option_t sh_dnx_ingress_reassembly_options[];
extern sh_sand_man_t sh_dnx_ingress_reassembly_man;

/**
 * \brief - dump reassembly and port termination context table
 */
shr_error_e sh_dnx_ingr_reassembly_cmd (
        int unit,
        args_t * args,
        sh_sand_control_t *sand_control);


#endif /** DIAG_DNX_INGRESS_REASSEMBLY_H_INCLUDED */
