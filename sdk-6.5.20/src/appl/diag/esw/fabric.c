/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Fabric and Higig related CLI commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/stack.h>
#include <bcm/debug.h>

#ifdef BCM_XGS_SUPPORT
#include <soc/higig.h>
#endif

#ifdef	BCM_XGS_SUPPORT
#define PACK_LONG(buf, val) \
    do {                                               \
        uint32 v2;                                     \
        v2 = bcm_htonl(val);                           \
        sal_memcpy(buf, &v2, sizeof(uint32));          \
    } while (0)

char if_h2higig_usage[] =
    "H2Higig [<word0> <word1> <word2>]:  Convert hex to higig info\n";

cmd_result_t
if_h2higig(int unit, args_t *args)
{
    uint32 val;
    char *arg;
    soc_higig_hdr_t hghdr;
    int i;

    sal_memset(&hghdr, 0, sizeof(soc_higig_hdr_t));
    for (i = 0; i < 3; i++) {
        if ((arg = ARG_GET(args)) == NULL) {
            break;
        }

        val = sal_strtoul(arg, NULL, 0);
        PACK_LONG(&hghdr.overlay0.bytes[i * 4], val);
    }

    soc_higig_dump(unit, "", &hghdr);

    return CMD_OK;
}

#ifdef BCM_HIGIG2_SUPPORT

char if_h2higig2_usage[] =
    "H2Higig2 [<word0> <word1> <word2> <word3> [<word4>]]:  Convert hex to higig2 info\n";

cmd_result_t
if_h2higig2(int unit, args_t *args)
{
    uint32 val;
    char *arg;
    soc_higig2_hdr_t hghdr;
    int i;

    sal_memset(&hghdr, 0, sizeof(soc_higig2_hdr_t));
    for (i = 0; i < 4; i++) {
        if ((arg = ARG_GET(args)) == NULL) {
            break;
        }

        val = sal_strtoul(arg, NULL, 0);
        PACK_LONG(&hghdr.overlay0.bytes[i * 4], val);
    }

    soc_higig2_dump(unit, "", &hghdr);

    /* Decode extension header if present */
    if ((arg = ARG_GET(args)) != NULL) {
        int eh_type;
        val = sal_strtoul(arg, NULL, 0);
        eh_type = val >> 28;
        cli_out("0x%08x <EHT=%d", val, eh_type);
        if (eh_type == 0) {
            cli_out(" TM=%d", (val >> 24) & 0x1);
            cli_out(" SEG_SEL=%d", (val >> 21) & 0x7);
            cli_out(" TAG_TYPE=%d", (val >> 19) & 0x3);
            cli_out(" QTAG=0x%04x", val & 0xffff);
        }
        cli_out(">\n");
    }

    return CMD_OK;
}
#endif /* BCM_HIGIG2_SUPPORT */

#endif	/* BCM_XGS_SUPPORT */

typedef int _bcm_diag_fabric_not_empty; /* Make ISO compilers happy. */
