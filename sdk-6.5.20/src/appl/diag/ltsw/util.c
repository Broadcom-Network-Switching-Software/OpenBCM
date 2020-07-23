/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Functions to support CLI port commands
 */

#if defined(BCM_LTSW_SUPPORT)

#include <shared/pbmp.h>
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <bcm/types.h>
#include <bcm/port.h>
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/property.h>
#include <appl/diag/ltsw/util.h>

/*
 * Format a pbmp into a string, the opposite of what parse_pbmp does.
 * For example, 0x00ffffff ==> "fe"; 0x0b000007 ==> "cpu,ge,fe0-fe2".
 * For clarity, returns "ge,fe" instead of "e" (or on GSL, just "ge"),
 * and returns "cpu,ge,fe" instead of "all" (or on GSL, just "cpu,ge").
 */

static void
_format_pbmp_type(int unit,
                  char *dest,
                  int bufsize,
                  bcm_pbmp_t pbmp,
                  char *pname,
                  bcm_pbmp_t pall)
{
    bcm_pbmp_t  bm;
    soc_port_t  tport, port, dport, pfirst, plast;
    int size = 0;

    if (BCM_PBMP_IS_NULL(pall)) {       /* no such ports */
        return;
    }

    BCM_PBMP_ASSIGN(bm, pbmp);
    BCM_PBMP_AND(bm, pall);
    if (BCM_PBMP_EQ(bm, pall)) {        /* all ports of this type */
        size = sal_strlen(dest);
        assert((bufsize - size) >= 0);
        sal_snprintf(dest + size,(bufsize - size), ",%s", pname);
        return;
    }

    /*
     * The slightly tricky thing here is handling sparse sets of
     * bits in pall (on Tucana or Herc4 for example).
     */
    pfirst = plast = -1;
    port = -1;
    /* coverity[overrun-local] */
    DPORT_PBMP_ITER(unit, pall, dport, tport) {        /* a subset of ports */
        port += 1;
        /* coverity[overrun-local] */
        if (BCM_PBMP_MEMBER(pbmp, tport)) {
            if (pfirst < 0) {
                pfirst = plast = port;
            } else {
                plast = port;
            }
            continue;
        }
        if (pfirst >= 0) {
            size = sal_strlen(dest);
            assert((bufsize - size) >= 0);
            sal_snprintf(dest + sal_strlen(dest), (bufsize - size), ",%s%d", pname, pfirst);
            if (pfirst != plast) {
                size = sal_strlen(dest);
                assert((bufsize - size) >= 0);
                sal_snprintf(dest + sal_strlen(dest), (bufsize - size), "-%s%d", pname, plast);
            }
            pfirst = plast = -1;
        }
    }

    if (pfirst >= 0) {
        size = sal_strlen(dest);
        assert((bufsize - size) >= 0);
        sal_snprintf(dest + sal_strlen(dest), (bufsize - size), ",%s%d", pname, pfirst);
        if (pfirst != plast) {
            size = sal_strlen(dest);
            assert((bufsize - size) >= 0);
            sal_snprintf(dest + sal_strlen(dest), (bufsize - size), "-%s%d", pname, plast);
        }
    }
}


/*
 * Parse a string containing a list of ports into a bitmap string.
 * The input format is:
 *
 *      PBMP    : '0x' {HEXDIGIT}+      { return BITS_SET($2); }
 *              | LIST                  { return $1; }
 *              ;
 *      LIST    : [LIST ','] RANGE STEP         { return UNION($1, $2, $3); }
 *              | [LIST ','] '~' RANGE STEP     { return REMOVE($1, $2, $3); }
 *              ;
 *      RANGE   : PORT '-' PORT         { return $1 .. $2; }
 *              | PORT                  { return $1 .. $1; }
 *              | 'FE'                  { return FE ports; }
 *              | 'GE'                  { return GE ports; }
 *              | 'XE'                  { return XE ports; }
 *              | 'HG'                  { return HG ports; }
 *              | 'E'                   { return ether ports; }
 *              | '*'                   { return ether, HG ports; }
 *              | 'ALL'                 { return ether, HG, CPU ports; }
 *              ;
 *      STEP    : ':' INTEGER           { return $2 };
 *              ;
 *      PORT    : INTEGER{NUM_PORT}             { return $1 }
 *              | 'E' INTEGER{NUM_E_PORT}       { return E port $2 }
 *              | 'FE' INTEGER{NUM_FE_PORT}     { return FE port $2 }
 *              | 'GE' INTEGER{NUM_GE_PORT}     { return GE port $2 }
 *              | 'XE' INTEGER{NUM_GE_PORT}     { return XE port $2 }
 *              | 'HG' INTEGER{NUM_HG_PORT}     { return HG port $2 }
 *              | 'CPU'                         { return CMIC_PORT }
 *              | 'NONE'                        { return NIL }
 *
 * Returns 0 on success, -1 on syntax error.
 */
int
ltsw_parse_pbmp(int unit, char *s, bcm_pbmp_t *pbmp)
{
    bcm_pbmp_t  bmall, pbmp_temp, pbmp_lb, pbmp_mgmt;
    int         complement, plast, pfirst, pstep, port, p, tp, i, rv;
    char        *sn, *se;
    bcm_port_config_t cfg;

    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) {
        return _shr_pbmp_decode(s, pbmp);
    }

    BCM_PBMP_CLEAR(bmall);
    BCM_PBMP_CLEAR(pbmp_temp);
    BCM_PBMP_CLEAR(pbmp_lb);
    BCM_PBMP_CLEAR(pbmp_mgmt);
    BCM_PBMP_CLEAR(*pbmp);

    bcm_port_config_t_init(&cfg);
    rv = bcm_port_config_get(unit, &cfg);
    if (rv < 0) {
        cli_out("Error: Could not get port config, info: %s\n",
                bcm_errmsg(rv));
        return CMD_FAIL;
    }

    (void)bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_LB, &pbmp_lb);
    (void)bcmi_ltsw_port_bitmap(unit, BCMI_LTSW_PORT_TYPE_MGMT, &pbmp_mgmt);
    BCM_PBMP_ASSIGN(pbmp_temp, cfg.all);
    BCM_PBMP_OR(pbmp_temp, pbmp_lb);

    pfirst = -1;
    pstep = 1;
    complement = 0;

    while (*s) {
        if (s[0] == '~' || s[0] == '!') {
            complement = 1;
            s += 1;
        }

        /* To cover for Apache XLPORTB0 and CLG2PORT blocks  */
        if ((sal_strncasecmp("XLB0PORT", s, (sizeof("XLB0PORT") - 1)) == 0) ||
            (sal_strncasecmp("CLG2PORT", s, (sizeof("CLG2PORT") - 1)) == 0)) {
            sn = s + 8;
        } else {
            for (sn = s; *sn && *sn != '-' && *sn != ',' && *sn != ':' &&
                     !isdigit((unsigned)*sn); sn++) {
                ;
            }
        }

        se = sn;
        if (isdigit((unsigned)*sn)) {
            port = 0;
            do {
                port = port * 10 + (*se++ - '0');
            } while (isdigit((unsigned) *se));
        } else {
            port = -1;
        }
        plast = -1;

        if (sn == s) {                          /* unprefixed number */
            if (bcmi_ltsw_property_get(unit, BCMI_CPN_DPORT_MAP_DIRECT, 0)) {
                /* Treat port as logical port number */
                plast = bcmi_ltsw_port_to_dport(unit, port);
            } else {
                tp = bcmi_ltsw_dport_to_port(unit, port);
                if ((tp >= 0) && (tp < BCM_PBMP_PORT_MAX)
                    && BCM_PBMP_MEMBER(pbmp_temp, tp)) {
                    plast = port;
                } else {
                    return -1;                  /* error: port out of range */
                }
            }
        } else {                                /* prefix number */
            tp = *sn;
            *sn = '\0';
            if (sal_strcasecmp(s, "fe") == 0) {
                bmall = cfg.fe;
            } else if (sal_strcasecmp(s, "ge") == 0) {
                bmall = cfg.ge;
            } else if (sal_strcasecmp(s, "xe") == 0) {
                bmall = cfg.xe;
            } else if (sal_strcasecmp(s, "ce") == 0) {
                bmall = cfg.ce;
            } else if (sal_strcasecmp(s, "cd") == 0) {
                bmall = cfg.cd;
            } else if (sal_strcasecmp(s, "hg") == 0) {
                bmall = cfg.hg;
            } else if (sal_strcasecmp(s, "e") == 0) {
                bmall = cfg.e;
            } else if (sal_strcasecmp(s, "cpu") == 0) {
                bmall = cfg.cpu;
            } else if (sal_strcasecmp(s, "cmic") == 0) {
                bmall = cfg.cpu;
            } else if (sal_strcasecmp(s, "lb") == 0) {
                bmall = pbmp_lb;
            } else if (sal_strcasecmp(s, "mgmt") == 0) {
                bmall = pbmp_mgmt;
            } else if (sal_strcasecmp(s, "all") == 0) {
                bmall = cfg.all;
            } else if (sal_strcasecmp(s, "*") == 0) {
                bmall = cfg.port;
            } else if (sal_strcasecmp(s, "none") == 0) {
                BCM_PBMP_CLEAR(bmall);
            }
            *sn = tp;
            if (port >= 0) {
                i = 0;
                DPORT_PBMP_ITER(unit, bmall, p, tp) {
                    if (port == i ++) {
                        plast = p;
                        break;
                    }
                }
                if (plast < 0) {
                    return -1;                  /* error: index out of range */
                }
            }
        }
        s = se;

        switch (*s) {
        case '-':
            pstep = 1;
            if (plast < 0) {
                return -1;                      /* error: range without port */
            } else if (pfirst < 0) {
                pfirst = plast;
            } else {
                return -1;                      /* error: x-y-z */
            }
            break;
        case ':':
            s++;
            if (!isdigit((unsigned)*s)) {
                return -1;
            }
            for (pstep = 0; isdigit((unsigned)*s); s++) {
                pstep = pstep * 10 + (*s - '0');
            }
            if (pstep == 0 || (*s != ',' && *s != '\0')) {
                return -1;
            }
            /* fall through */
        case ',':
        case '\0':
            if (plast < 0) {                    /* complete bitmap op */
                pfirst = 999;
                plast = -1;
                DPORT_PBMP_ITER(unit, bmall, p, tp) {
                    if (p < pfirst) {
                        pfirst = p;
                    }
                    if (p > plast) {
                        plast = p;
                    }
                }
            }
            if (pfirst < 0) {           /* just one port */
                pfirst = plast;
            }
            for (port = pfirst; port <= plast; port += pstep) {  /* a range */
                p = bcmi_ltsw_dport_to_port(unit, port);
                /* coverity[overrun-local] */
                if ((p >= 0) && BCM_PBMP_NOT_NULL(bmall) && !BCM_PBMP_MEMBER(bmall, p)) {
                    continue;   /* skip gaps in range */
                }
                if (bcmi_ltsw_port_gport_validate(unit, p, &p) == 0) {
                    if (complement) {
                        BCM_PBMP_PORT_REMOVE(*pbmp, p);
                    } else {
                        BCM_PBMP_PORT_ADD(*pbmp, p);
                    }
                }
            }
            if (*s == '\0') {
                return 0;
            }
            pfirst = -1;
            pstep = 1;
            complement = 0;
            BCM_PBMP_CLEAR(bmall);
            break;
        default:
            return -1;                          /* error: unexpected char */
        }
        s += 1;
    }
    return -1;                                  /* error: unexpected end */
}


char *
ltsw_format_pbmp(int unit, char *buf, int bufsize, bcm_pbmp_t pbmp)
{
    char        tmp[FORMAT_PBMP_MAX];
    bcm_pbmp_t  tpbm;
    bcm_port_config_t cfg;
    int rv;

    bcm_port_config_t_init(&cfg);
    rv = bcm_port_config_get(unit, &cfg);
    if (rv < 0) {
        cli_out("Error: Could not get port config, info: %s\n",
                bcm_errmsg(rv));
        return buf;
    }

    BCM_PBMP_ASSIGN(tpbm, pbmp);
    BCM_PBMP_REMOVE(tpbm, cfg.all);
    if (BCM_PBMP_NOT_NULL(tpbm)) {
        sal_strncpy(buf, _SHR_PBMP_FMT(pbmp, tmp), bufsize);
    } else if (BCM_PBMP_IS_NULL(pbmp)) {
        /*
         * COVERITY
         *
         * Handled below
         */
        /*    coverity[buffer_size]    */
        sal_strncpy(buf, "none", bufsize);
        if (bufsize < 5) {
            /* Guarantee null termination of buf */
            buf[bufsize - 1] = 0;
        }
    } else {
        BCM_PBMP_ASSIGN(tpbm, pbmp);
        BCM_PBMP_AND(tpbm, cfg.cpu);
        if (BCM_PBMP_NOT_NULL(tpbm)) {
            /* Coverity[secure_coding] */
            sal_strcpy(tmp, ",cpu");
        } else {
            tmp[0] = tmp[1] = 0;
        }

        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "cd", cfg.cd);
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "ce", cfg.ce);
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "fe", cfg.fe);
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "ge", cfg.ge);
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "xe", cfg.xe);
        _format_pbmp_type(unit, tmp, FORMAT_PBMP_MAX, pbmp, "hg", cfg.hg);
        sal_strncpy(buf, tmp + 1, bufsize);
    }
    return buf;
}

/*!
 * \brief Get port name.
 *
 * \param [in] unit Unit number.
 * \param [in] port Port number.
 *
 * \return Port name.
 */
char * ltsw_port_name(int unit, bcm_port_t port)
{
    return bcmi_ltsw_port_name(unit, port);
}

/*!
 * \brief Parses string that identifies a PORT
 *
 * \param [in] unit Unit number.
 * \param [in] s String.
 * \param [out] portp Port id.
 *
 * \return 0 on success, -1 on syntax error.
 */
int ltsw_parse_port(int unit, char *s, bcm_port_t *portp)
{
    bcm_port_t port, port_out, max_port;

    if (isint(s)) {
        *portp = port = parse_integer(s);
        if (bcmi_ltsw_property_get(unit, BCMI_CPN_DPORT_MAP_DIRECT, 0) == 0) {
            *portp = bcmi_ltsw_dport_to_port(unit, port);
        }
        if (*portp < 0) {
            return -1;
        }
        return 0;
    }

    max_port = bcmi_ltsw_dev_logic_port_num(unit);

    for (port = 0; port < max_port; port++) {
        if (bcmi_ltsw_port_gport_validate(unit, port, &port_out)) {
            continue;
        }
        if (sal_strcasecmp(s, "any") == 0) {
            *portp = port;
            return 0;
        }
        if (sal_strcasecmp(s, ltsw_port_name(unit, port)) == 0) {
            *portp = port;
            return 0;
        }
    }

    return -1;
}

#endif /* BCM_LTSW_SUPPORT */
