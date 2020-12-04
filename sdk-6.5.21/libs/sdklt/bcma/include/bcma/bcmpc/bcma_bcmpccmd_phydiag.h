/*! \file bcma_bcmpccmd_phydiag.h
 *
 * CLI 'PHYDiag' command for PHY based diagnostics.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_BCMPCCMD_PHYDIAG_H
#define BCMA_BCMPCCMD_PHYDIAG_H

#include <bcma/cli/bcma_cli.h>

/*! Brief description for CLI command. */
#define BCMA_BCMPCCMD_PHYDIAG_DESC  "PHY based diagnostics commands."

/*! Syntax for CLI command. */
#define BCMA_BCMPCCMD_PHYDIAG_SYNOP \
    "<port-range> dsc <val>\n" \
    "<port-range> prbs set Polynomial=<pval> [Invert=true|1|false|0]\\\n" \
    "                      [dir=tx|rx|tx,rx] [lane=<lane-range>]\n" \
    "<port-range> prbs get|clear [lane=<lane-range>]\n" \
    "<port> eyescan type=fast|1|berproj|4\n" \
    "<port-range> berproj HistogramErrorThreshold=<tval>\\\n" \
    "                     SampleTime=<time>\n" \
    "<port-range> prbsstat\\\n" \
    "             [STArt Interval=<seconds>|Counters|Ber|CLear|STOp]\n" \
    "<port-range> fecstat\\\n" \
    "             [STArt Interval=<seconds>|Counters|Ber|CLear|STOp]\n" \
    "<port-range> linkcat mode=tx|rx|lpbk lane=<lane-range>"

/*! Help for CLI command. */
#define BCMA_BCMPCCMD_PHYDIAG_HELP \
    "\n" \
    "The 'dsc' command displays diagnostics information.\n" \
    "\n" \
    "The 'eyescan' command runs the eyescan process and display the result\n" \
    "when done.\n" \
    "\n" \
    "The 'prbs' command performs various pseudo random binary sequence\n" \
    "(PRBS) functions according to the configuration parameters.\n" \
    "\n" \
    "The 'berproj' command projects the bit error rate (BER). It requires\n" \
    "PRBS to be running and being locked.\n" \
    "\n" \
    "The 'prbsstat' command periodically collects PRBS error counters and\n" \
    "computes the BER based on the port configuration and the observed\n" \
    "error counters.\n" \
    "The 'prbsstat' command assumes that PRBS is already running on\n" \
    "the ports that PRBSstat will monitor.\n" \
    "\n" \
    "The 'fecstat' command periodically collects forward error correction\n" \
    "(FEC) status counters such as corrected and uncorrected FEC codewords\n" \
    "for a given port. The command then uses the corrected codewords to\n" \
    "compute the pre-FEC BER for the channel.\n" \
    "\n" \
    "The 'linkcat' command runs channel analysis tool to collect data\n" \
    "on links.\n" \
    "\n" \
    "PHY diagnostics command input:\n" \
    "<port-range>: Logical port number range, 1-3, port 1 to port 3.\n" \
    "              Logical port number list 1,2,3, port 1, 2 and 3.\n" \
    "lane=<lane-range>: Logical lane index range on the port, starting with\n" \
    "                   0. If the 'prbs set' command specifies lane range,\n" \
    "                   then the 'prbs get' and 'prbs clear' commands\n" \
    "                   should specify lane range as well.\n" \
    "HistogramErrorThreshold=<tval>: Histogram error threshold range is\n" \
    "                                3-7. Use 0 for auto mode.\n" \
    "SampleTime=<time>: Timeout value for the PRBS errors are accumulated."

/*! Examples for CLI command. */
#define BCMA_BCMPCCMD_PHYDIAG_EXAMPLES \
    "1 dsc config\n"\
    "1-3 prbs set p=3 lane=0,1\n"\
    "1,2,3 prbs get lane=0,1\n"\
    "1,2,3 prbs clear lane=0,1\n"\
    "1 eyescan type=1\n"\
    "1,2 prbsstat start interval=60\n"\
    "1,2 prbsstat counters\n"\
    "1,2 prbsstat ber\n"\
    "1,2 prbsstat stop\n"\
    "1,2 fecstat start interval=60\n"\
    "1,2 fecstat counters\n"\
    "1,2 fecstat ber\n"\
    "1,2 fecstat stop\n" \
    "1-3 linkcat mode=lpbk lane=0,1"

/*!
 * \brief CLI 'phydiag' command implementation.
 *
 * \param [in] cli CLI object
 * \param [in] args Argument list
 *
 * \return BCMA_CLI_CMD_xxx return values.
 */
extern int
bcma_bcmpccmd_phydiag(bcma_cli_t *cli, bcma_cli_args_t *args);

/*!
 * \brief Clean up the resource used in the phydiag command.
 *
 * This function can be called before the CLI object is about to be destroyed
 * to prevent resource leak.
 *
 * \param[in] cli CLI object
 *
 * \return Always 0.
 */
extern int
bcma_bcmpccmd_phydiag_cleanup(bcma_cli_t *cli);

#endif /* BCMA_BCMPCCMD_PHY_H */
