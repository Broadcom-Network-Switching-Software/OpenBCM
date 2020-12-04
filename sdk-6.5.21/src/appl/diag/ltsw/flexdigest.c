/*! \file flexdigest.c
 *
 * Flex Digest management.
 * This file contains the management for Flex Digest.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/system.h>

#include <bcm/flexdigest.h>
#include <bcm/error.h>
#include <bcm_int/ltsw/flexdigest.h>

/*
 * Macro:
 *     FD_CHECK_RETURN
 * Purpose:
 *     Check the return value from an API call. Output either a failed
 *     message or okay along with the function name.
 */
#define FD_CHECK_RETURN(unit, retval, funct_name)                  \
    if (BCM_FAILURE(retval)) {                                     \
        cli_out("FD(unit %d) Error: %s() failed: %s\n", (unit),     \
                (funct_name), bcm_errmsg(retval));                 \
        return CMD_FAIL;                                           \
    } else {                                                       \
        LOG_VERBOSE(BSL_LS_APPL_SHELL, \
                    (BSL_META_U(unit, \
                                "FD(unit %d) verb: %s() success \n"),  \
                                (unit), (funct_name)));                \
    }

/*
 * Macro:
 *     FD_GET_NUMB
 * Purpose:
 *     Get a numerical value from stdin.
 */
#define FD_GET_NUMB(numb, str, args) \
    if (((str) = ARG_GET(args)) == NULL) { \
        return CMD_USAGE; \
    } \
    (numb) = parse_integer(str);


#define FD_QUALIFY_WIDTH_MAX 60
#define FD_QSET_WIDTH_MAX \
    ((FD_QUALIFY_WIDTH_MAX + 1) * bcmFlexDigestQualifyCount + 1)

#define FD_ACTION_WIDTH_MAX  60


#define FD_SALT_STR_LEN     80  /* Formatted flexdigest salt configuration */

static int
comma_count(char *s)
{
    int cc = 0;
    char *ts = s;

    while ((ts = sal_strchr(ts, ',')) != NULL) {
        ts++;
        cc++;
    }
    return cc;
}

static int
_fd_salt_parse(char *str, int size, uint16 *salt)
{
    char *s = str;
    int i = 0;;

    while (*s) {
        if (!isxdigit((unsigned)*s) && (*(s) != ',')) {  /* bad character */
            return -1;
        }
        salt[i] = xdigit2i((unsigned)*(s++));
        if (isxdigit((unsigned)*s)) {
            salt[i] *= 16;
            salt[i] += xdigit2i((unsigned)*(s++));
        }
        if (isxdigit((unsigned)*s)) {
            salt[i] *= 16;
            salt[i] += xdigit2i((unsigned)*(s++));
        }
        if (isxdigit((unsigned)*s)) {
            salt[i] *= 16;
            salt[i] += xdigit2i((unsigned)*(s++));
        }
        if (*(s) && (*(s) != ',')) {  /* bad character */
            return -1;
        } else {
            i++;
            s++;
        }
    }

    return 0;
}

#ifdef BROADCOM_DEBUG
static void
_fd_salt_format(char buf[FD_SALT_STR_LEN], int size, uint16 *salt)
{
    int i;

    for (i = 0; i < size; i++) {
        *buf++ = i2xdigit(salt[i] >> 12);
        *buf++ = i2xdigit(salt[i] >> 8);
        *buf++ = i2xdigit(salt[i] >> 4);
        *buf++ = i2xdigit(salt[i]);
        *buf++ = ',';
    }

    *--buf = 0;
}
#endif /* BROADCOM_DEBUG */

static char *
_fd_hash_bin_set_format(char *buf, bcm_flexdigest_hash_bin_set_t bin_set, int brief)
{
    char *bin_text[] = BCM_FLEXDIGEST_HASH_BIN_SET_STRINGS;

    assert(buf != NULL);

    if (0 <= bin_set && bin_set < bcmFlexDigestHashBinCount) {
        if (brief) {
            sal_sprintf(buf, "%s", bin_text[bin_set]);
        } else {
            sal_sprintf(buf, "bcmFlexDigestHash%s", bin_text[bin_set]);
        }
    } else {
        sal_sprintf(buf, "invalid type=%#x", bin_set);
    }

    return buf;
}

static bcm_flexdigest_hash_bin_set_t
_fd_hash_bin_set_parse(char *act_str)
{
    char cannonical_str[64];
    bcm_flexdigest_hash_bin_set_t bin_set;

    assert(act_str != NULL);
    if (sal_strlen(act_str) >=  64 - 1) {
        return -1;
    }

    for (bin_set = 0; bin_set < bcmFlexDigestHashBinCount; bin_set++) {
        /* Test for whole name of the bin set */
        _fd_hash_bin_set_format(cannonical_str, bin_set, 0);
        if (!sal_strcasecmp(cannonical_str, act_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFlexDigestHash"),
                           act_str)) {
            break;
        }
    }

    /* If not found, result is -1 */
    return bin_set;
}

static char *
_fd_norm_seed_control_type_format(char *buf, bcm_flexdigest_norm_seed_control_t type, int brief)
{
    char *type_text[] = BCM_FLEXDIGEST_NORM_SEED_CONTROL_STRINGS;

    assert(buf != NULL);

    if (0 <= type && type < bcmFlexDigestNormSeedControlCount) {
        if (brief) {
            sal_sprintf(buf, "%s", type_text[type]);
        } else {
            sal_sprintf(buf, "bcmFlexDigestNormSeedControl%s", type_text[type]);
        }
    } else {
        sal_sprintf(buf, "invalid type=%#x", type);
    }

    return buf;
}

static bcm_flexdigest_norm_seed_control_t
_fd_norm_seed_control_type_parse(char *act_str)
{
    char cannonical_str[128];
    bcm_flexdigest_norm_seed_control_t type;

    assert(act_str != NULL);
    if (sal_strlen(act_str) >=  128 - 1) {
        return -1;
    }

    for (type = 0; type < bcmFlexDigestNormSeedControlCount; type++) {
        /* Test for whole name of the profile */
        _fd_norm_seed_control_type_format(cannonical_str, type, 0);
        if (!sal_strcasecmp(cannonical_str, act_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFlexDigestNormSeedControl"),
                           act_str)) {
            break;
        }
    }

    /* If not found, result is -1 */
    return type;
}


static char *
_fd_norm_profile_control_type_format(char *buf, bcm_flexdigest_norm_profile_control_t type, int brief)
{
    char *type_text[] = BCM_FLEXDIGEST_NORM_PROFILE_CONTROL_STRINGS;

    assert(buf != NULL);

    if (0 <= type && type < bcmFlexDigestNormProfileControlCount) {
        if (brief) {
            sal_sprintf(buf, "%s", type_text[type]);
        } else {
            sal_sprintf(buf, "bcmFlexDigestNormProfileControl%s", type_text[type]);
        }
    } else {
        sal_sprintf(buf, "invalid type=%#x", type);
    }

    return buf;
}

static bcm_flexdigest_norm_profile_control_t
_fd_norm_profile_control_type_parse(char *act_str)
{
    char cannonical_str[128];
    bcm_flexdigest_norm_profile_control_t type;

    assert(act_str != NULL);
    if (sal_strlen(act_str) >=  128 - 1) {
        return -1;
    }

    for (type = 0; type < bcmFlexDigestNormProfileControlCount; type++) {
        /* Test for whole name of the profile */
        _fd_norm_profile_control_type_format(cannonical_str, type, 0);
        if (!sal_strcasecmp(cannonical_str, act_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFlexDigestNormProfileControl"),
                           act_str)) {
            break;
        }
    }

    /* If not found, result is -1 */
    return type;
}

static char *
_fd_hash_profile_control_type_format(char *buf, bcm_flexdigest_hash_profile_control_t type, int brief)
{
    char *type_text[] = BCM_FLEXDIGEST_HASH_PROFILE_CONTROL_STRINGS;

    assert(buf != NULL);

    if (0 <= type && type < bcmFlexDigestHashProfileControlCount) {
        if (brief) {
            sal_sprintf(buf, "%s", type_text[type]);
        } else {
            sal_sprintf(buf, "bcmFlexDigestHashProfileControl%s", type_text[type]);
        }
    } else {
        sal_sprintf(buf, "invalid type=%#x", type);
    }

    return buf;
}

static bcm_flexdigest_hash_profile_control_t
_fd_hash_profile_control_type_parse(char *act_str)
{
    char cannonical_str[128];
    bcm_flexdigest_hash_profile_control_t type;

    assert(act_str != NULL);
    if (sal_strlen(act_str) >=  128 - 1) {
        return -1;
    }

    for (type = 0; type < bcmFlexDigestHashProfileControlCount; type++) {
        /* Test for whole name of the profile */
        _fd_hash_profile_control_type_format(cannonical_str, type, 0);
        if (!sal_strcasecmp(cannonical_str, act_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFlexDigestHashProfileControl"),
                           act_str)) {
            break;
        }
    }

    /* If not found, result is -1 */
    return type;
}


static char *
_fd_match_format(char *buf, bcm_flexdigest_match_t match, int brief)
{
    char *match_text[] = BCM_FLEXDIGEST_MATCH_STRINGS;

    assert(buf != NULL);

    if (0 <= match && match < bcmFlexDigestMatchCount) {
        if (brief) {
            sal_sprintf(buf, "%s", match_text[match]);
        } else {
            sal_sprintf(buf, "bcmFlexDigestMatch%s", match_text[match]);
        }
    } else {
        sal_sprintf(buf, "invalid match value=%#x", match);
    }

    return buf;
}

static bcm_flexdigest_match_t
_fd_match_parse(char *act_str)
{
    char cannonical_str[128];
    bcm_flexdigest_match_t match;

    assert(act_str != NULL);
    if (sal_strlen(act_str) >=  128 - 1) {
        return -1;
    }

    for (match = 0; match < bcmFlexDigestMatchCount; match++) {
        /* Test for whole name of the action */
        _fd_match_format(cannonical_str, match, 0);
        if (!sal_strcasecmp(cannonical_str, act_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFlexDigestMatch"),
                           act_str)) {
            break;
        }
    }

    /* If not found, result is -1 */
    return match;
}

static char *
_fd_action_format(char *buf, bcm_flexdigest_action_t action, int brief)
{
    char *action_text[] = BCM_FLEXDIGEST_ACTION_STRINGS;

    assert(buf != NULL);

    if (0 <= action && action < bcmFlexDigestActionCount) {
        if (brief) {
            sal_sprintf(buf, "%s", action_text[action]);
        } else {
            sal_sprintf(buf, "bcmFlexDigestAction%s", action_text[action]);
        }
    } else {
        sal_sprintf(buf, "invalid action value=%#x", action);
    }

    return buf;
}

static bcm_flexdigest_action_t
_fd_action_parse(char *act_str)
{
    char cannonical_str[128];
    bcm_flexdigest_action_t action;

    assert(act_str != NULL);
    if (sal_strlen(act_str) >=  128 - 1) {
        return -1;
    }

    for (action = 0; action < bcmFlexDigestActionCount; action++) {
        /* Test for whole name of the action */
        _fd_action_format(cannonical_str, action, 0);
        if (!sal_strcasecmp(cannonical_str, act_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFlexDigestAction"),
                           act_str)) {
            break;
        }
    }

    /* If not found, result is -1 */
    return action;
}

static char *
_fd_qualifier_format(
    char *buf,
    bcm_flexdigest_qualify_t qualifier,
    int brief)
{
    char *qualifier_text[] = BCM_FLEXDIGEST_QUALIFY_STRINGS;

    assert(buf != NULL);

    if (0 <= qualifier && qualifier < bcmFlexDigestQualifyCount) {
        if (brief) {
            sal_sprintf(buf, "%s", qualifier_text[qualifier]);
        } else {
            sal_sprintf(buf, "bcmFlexDigestQualify%s", qualifier_text[qualifier]);
        }
    } else {
        sal_sprintf(buf, "invalid qualifier value=%#x", qualifier);
    }

    return buf;
}

static bcm_flexdigest_qualify_t
_fd_qualifier_parse(char *qual_str)
{
    char cannonical_str[128];
    bcm_flexdigest_qualify_t qual;

    assert(qual_str != NULL);
    if (sal_strlen(qual_str) >=  128 - 1) {
        return -1;
    }

    for (qual = 0; qual < bcmFlexDigestQualifyCount; qual++) {
        /* Test for whole name of the qualifier */
        _fd_qualifier_format(cannonical_str, qual, 0);
        if (!sal_strcasecmp(cannonical_str, qual_str)) {
            break;
        }
        /* Test for the suffix only */
        if(!sal_strcasecmp(cannonical_str + sal_strlen("bcmFlexDigestQualify"),
                           qual_str)) {
            break;
        }
    }

    /* If not found, result is -1 */
    return qual;
}


static char *
_fd_qualifier_name(bcm_flexdigest_qualify_t qual)
{
    /* Text names of the enumerated qualifier IDs. */
    static char *qual_text[] = BCM_FLEXDIGEST_QUALIFY_STRINGS;

    if (qual < bcmFlexDigestQualifyCount) {
        return qual_text[qual];
    }
    return "UnknownQualifier";
}

#ifdef BROADCOM_DEBUG
static void
_fd_qset_dump(
    char *prefix,
    bcm_flexdigest_qset_t qset,
    char *suffix)
{
    int qual;
    int first_qual = 1;

    if (prefix == NULL) {
        prefix = "";
    }
    if (suffix == NULL) {
        suffix = "";
    }

    LOG_CLI((BSL_META("%s{"), prefix));
    for (qual = 0; qual < bcmFlexDigestQualifyCount; qual++) {
        if (BCM_FLEXDIGEST_QSET_TEST(qset, qual)) {
            LOG_CLI((BSL_META("%s%s"),
                    (first_qual ? "" : ", "), _fd_qualifier_name(qual)));
            first_qual = 0;
        }
    }
    LOG_CLI((BSL_META("}%s"), suffix));
}
#endif /* BROADCOM_DEBUG */

static int
_fd_qset_parse(
    char *str,
    bcm_flexdigest_qset_t *qset)
{
    char *buf;
    char *str_copy;
    int input_len;
    bcm_flexdigest_qualify_t qual;
    int retval = 0;
    char delimiters[] = " \t,\n\0{}";
    int nval = _SHR_BITDCLSIZE(BCM_FLEXDIGEST_QUALIFY_MAX);
    int idx;
    char eight[11], *src_str, *temp_str;
    char *tokstr;

    assert(str != NULL);

    BCM_FLEXDIGEST_QSET_INIT(*qset);

    str_copy = (char *) sal_alloc((FD_QSET_WIDTH_MAX * sizeof(char)),
                                   "qset string");
    if (NULL == str_copy) {
        return (BCM_E_MEMORY);
    }

    /* Parse bitmap input in the form of an Integer. */
    if (isint(str)) {
        /* Skip to the last hex digit in the string */
        for (src_str = str + 1; isxdigit((unsigned) src_str[1]); src_str++) {
            ;
        }

        /* Parse backward in groups of 8 digits */
        idx = 0;
        do {
            /* Copy 8 digits backward to form a string "0xdddddddd\0" */
            temp_str = eight + 11;
            *--temp_str = 0;
            while (temp_str > eight + 2 && *src_str != 'x') {
                *--temp_str = *src_str--;
            }
            *--temp_str = 'x';
            *--temp_str = '0';

            qset->w[idx++] = parse_integer(temp_str);
        } while (*src_str != 'x' && idx < nval);
        sal_free(str_copy);
        return 1;
    }

    /* Parse list of qualifiers. */
    input_len = sal_strlen(str);
    if (input_len >= FD_QSET_WIDTH_MAX) {
        sal_free(str_copy);
        return 0;
    }
    sal_strncpy(str_copy, str, input_len + 1);

    buf = sal_strtok_r(str_copy, delimiters, &tokstr);

    do {
        qual = _fd_qualifier_parse(buf);
        if (qual != -1) {
            BCM_FLEXDIGEST_QSET_ADD(*qset, qual);
            retval++;
        }
        buf = sal_strtok_r(NULL, delimiters, &tokstr);
    } while (buf);

    sal_free(str_copy);
    return retval;
}

static char *
_fd_qset_format(char *buf, bcm_flexdigest_qset_t qset, char *separator)
{
    bcm_flexdigest_qualify_t  qual;
    char                 buf_local[30];
    int                  first_print = 1;

    assert(buf != NULL);

    buf[0] = '{';
    buf[1] = '\0';

    /* Iterate over Qset, looking for bits that are set. */
    for (qual = 0; qual < bcmFlexDigestQualifyCount; qual++) {
        if(BCM_FLEXDIGEST_QSET_TEST(qset, qual)) {
            if (first_print == 1) {
                first_print = 0;
            } else {
                sal_strncat(buf, separator, sal_strlen(separator));
            }
            _fd_qualifier_format(buf_local, qual, 1);
            sal_strncat(buf, buf_local, sal_strlen(buf_local)+1);
        }
    }
    sal_strncat(buf, "}", sal_strlen("}")+1);

    return buf;
}

static int
fd_qset_set(
    int unit,
    args_t *args,
    bcm_flexdigest_qset_t *qset)
{
    char *qual_str = NULL;
    char *buf;

    if ((qual_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    BCM_FLEXDIGEST_QSET_INIT(*qset);
    buf = (char *) sal_alloc((FD_QSET_WIDTH_MAX * sizeof(char)),
                              "qset string");
    if (NULL == buf) {
        return CMD_FAIL;
    }

    if (_fd_qset_parse(qual_str, qset) == 0) {
        sal_free(buf);
        return CMD_FAIL;
    }

    cli_out("fd_qset_set(%s) okay\n", _fd_qset_format(buf, *qset, " "));

    sal_free(buf);

    return CMD_OK;
}

static int
fd_qset_add(
    int unit,
    args_t *args,
    bcm_flexdigest_qset_t *qset)
{
    char *qual_str = NULL;
    char buf[FD_QUALIFY_WIDTH_MAX];
    bcm_flexdigest_qualify_t qual;

    if ((qual_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(qual_str)) {
        qual = parse_integer(qual_str);
    } else {
        qual = _fd_qualifier_parse(qual_str);

        if (qual == bcmFlexDigestQualifyCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown qualifier: %s\n"),
                       unit, qual_str));
            return CMD_FAIL;
        }
    }

    BCM_FLEXDIGEST_QSET_ADD(*qset, qual);
    cli_out("BCM_FLEXDIGEST_QSET_ADD(%s) okay\n",
            _fd_qualifier_format(buf, qual, 1));
    return CMD_OK;
}

static int
fd_qset_show(bcm_flexdigest_qset_t *qset)
{
#ifdef BROADCOM_DEBUG
    _fd_qset_dump("qset=", *qset, "\n");
#endif /* BROADCOM_DEBUG */
    return CMD_OK;
}

static int
fd_qset(
    int unit,
    args_t *args,
    bcm_flexdigest_qset_t *qset)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd qset clear */
    if (!sal_strcasecmp(subcmd, "clear")) {
        BCM_FLEXDIGEST_QSET_INIT(*qset);
        return CMD_OK;
    }
    /* BCM.0> fd qset set ...*/
    if (!sal_strcasecmp(subcmd, "set")) {
        return fd_qset_set(unit, args, qset);
    }
    /* BCM.0> fd qset add ...*/
    if (!sal_strcasecmp(subcmd, "add")) {
        return fd_qset_add(unit, args, qset);
    }
    /* BCM.0> fd qset show */
    if (!sal_strcasecmp(subcmd, "show")) {
        return fd_qset_show(qset);
    }

    return CMD_USAGE;
}

static int
fd_group_create(
    int unit,
    args_t *args,
    bcm_flexdigest_qset_t *qset)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int pri;
    bcm_flexdigest_group_t gid;

    FD_GET_NUMB(pri, subcmd, args);

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fd group create 'prio'  */
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _group_create pri=%d\n"), unit, pri));
        retval = bcm_flexdigest_group_create(unit, *qset, pri, &gid);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_group_create");
    } else {
        gid = parse_integer(subcmd);
        if ((subcmd = ARG_GET(args)) == NULL) {
            /* BCM.0> fd group create 'prio' 'gid' */
            LOG_VERBOSE(BSL_LS_APPL_SHELL,
                        (BSL_META_U(unit,
                                    "FD(unit %d) verb: _group_create_id pri=%d gid=%d\n"),
                                    unit, pri, gid));
            retval = bcm_flexdigest_group_create_id(unit, *qset, pri, gid);
            FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_group_create_id");
        } else {
            return CMD_USAGE;
        }
    }

    return CMD_OK;
}

static int
fd_group_destroy(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_group_t gid;

    FD_GET_NUMB(gid, subcmd, args);

    /* BCM.0> fd group destroy 'gid' */
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb:_group_destroy gid=%d\n"), unit, gid));
    retval = bcm_flexdigest_group_destroy(unit, gid);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_group_destroy");

    return CMD_OK;
}

static int
fd_group_get(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_group_t gid;
    bcm_flexdigest_qset_t qset;
    int pri;

    FD_GET_NUMB(gid, subcmd, args);

    /* BCM.0> fd group create 'prio'  */
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: _group_get gid=%d\n"), unit, gid));
    retval = bcm_flexdigest_group_get(unit, gid, &qset, &pri);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_group_get");
    fd_qset_show(&qset);

    return CMD_OK;
}

static int
fd_group(int unit, args_t *args, bcm_flexdigest_qset_t *qset)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    /* BCM.0> fd group create ... */
    if(!sal_strcasecmp(subcmd, "create")) {
        return fd_group_create(unit, args, qset);
    }
    /* BCM.0> fd group destroy ... */
    if(!sal_strcasecmp(subcmd, "destroy")) {
        return fd_group_destroy(unit, args);
    }

    /* BCM.0> fd group get ... */
    if(!sal_strcasecmp(subcmd, "get")) {
        return fd_group_get(unit, args);
    }

    return CMD_USAGE;
}

static int
fd_entry_create(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_group_t gid;
    bcm_flexdigest_entry_t eid;

    FD_GET_NUMB(gid, subcmd, args);

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fd entry create 'gid'  */
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _entry_create gid=%d\n"), unit, gid));
        retval = bcm_flexdigest_entry_create(unit, gid, &eid);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_entry_create");
    } else {
        /* BCM.0> fd entry create 'gid' 'eid' */
        eid = parse_integer(subcmd);
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _entry_create gid=%d, eid=%d\n"), unit, gid, eid));
        retval = bcm_flexdigest_entry_create_id(unit, gid, eid);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_entry_create_id");
    }
    return CMD_OK;
}

static int
fd_entry_destroy(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_entry_t eid;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    } else {
        eid = parse_integer(subcmd);
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: bcm_flexdigest_entry_destroy(eid=%d)\n"), unit, eid));
        retval = bcm_flexdigest_entry_destroy(unit, eid);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_entry_destroy");
        return CMD_OK;
    }
    return CMD_USAGE;
}

static int
fd_entry_install(int unit, args_t *args)
{
    int                         retval = CMD_OK;
    char*                       subcmd = NULL;
    bcm_flexdigest_entry_t           eid;

    /* BCM.0> fd entry install 'eid' */
    FD_GET_NUMB(eid, subcmd, args);

    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: _entry_install eid=%d\n"), unit, eid));
    retval = bcm_flexdigest_entry_install(unit, eid);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_entry_install");

    return CMD_OK;
}

static int
fd_entry_reinstall(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_entry_t eid;

    FD_GET_NUMB(eid, subcmd, args);

    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: _entry_reinstall eid=%d\n"), unit, eid));
    retval = bcm_flexdigest_entry_reinstall(unit, eid);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_entry_reinstall");
    return CMD_OK;
}

static int
fd_entry_remove(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_entry_t eid;

    FD_GET_NUMB(eid, subcmd, args);

    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: _entry_remove eid=%d\n"), unit, eid));
    retval = bcm_flexdigest_entry_remove(unit, eid);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_entry_remove");
    return CMD_OK;
}

static int
fd_entry_prio(int unit, args_t *args)
{
    int retval = CMD_OK;
    int prio;
    char *subcmd = NULL;
    bcm_flexdigest_entry_t eid;

    FD_GET_NUMB(eid, subcmd, args);

    /* BCM.0> fd entry prio <eid> */
    if ((subcmd = ARG_GET(args)) == NULL) {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: bcm_flexdigest_entry_priority_get(eid=%d)\n"), unit, eid));
        retval = bcm_flexdigest_entry_priority_get(unit, eid, &prio);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_entry_priority_get");
        cli_out("FD entry=%d: prio=%d\n", eid, prio);
    } else {
        /* BCM.0> fd entry prio <eid> [prio] */
        if (isint(subcmd)) {
            prio = parse_integer(subcmd);
        } else {
            return CMD_USAGE;
        }

        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: bcm_flexdigest_entry_priority_set(eid=%d, prio=%d)\n"),
                                unit, eid, prio));
        retval = bcm_flexdigest_entry_priority_set(unit, eid, prio);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_entry_priority_set");
    }

    return CMD_OK;
}

static int
fd_entry(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd entry create ... */
    if(!sal_strcasecmp(subcmd, "create")) {
        return fd_entry_create(unit, args);
    }
    /* BCM.0> fd entry destroy ... */
    if(!sal_strcasecmp(subcmd, "destroy")) {
        return fd_entry_destroy(unit, args);
    }
    /* BCM.0> fd entry install ... */
    if(!sal_strcasecmp(subcmd, "install")) {
        return fd_entry_install(unit, args);
    }
    /* BCM.0> fd entry reinstall ... */
    if(!sal_strcasecmp(subcmd, "reinstall")) {
        return fd_entry_reinstall(unit, args);
    }
    /* BCM.0> fd entry remove ... */
    if(!sal_strcasecmp(subcmd, "remove")) {
        return fd_entry_remove(unit, args);
    }
    /* BCM.0> fd entry prio ... */
    if(!sal_strcasecmp(subcmd, "prio")) {
        return fd_entry_prio(unit, args);
    }
    return CMD_USAGE;
}

static int
fd_match_create(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_match_id_t match_id;

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fd match create  */
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _match_id_create\n"), unit));
        retval = bcm_flexdigest_match_id_create(unit, &match_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_match_id_create");
        LOG_CLI((BSL_META("FD(unit %d) The created MatchId is: %d\n"), unit, match_id));
    } else {
        return CMD_USAGE;
    }
    return CMD_OK;
}

static int
fd_match_destroy(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_match_id_t match_id;

    if ((subcmd = ARG_GET(args)) == NULL) {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _match_id_destroy_all()\n"), unit));
        retval = bcm_flexdigest_match_id_destroy_all(unit);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_match_id_destroy_all");
        return CMD_OK;
    } else {
        match_id = parse_integer(subcmd);
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: bcm_flexdigest_entry_destroy(MatchId=%d)\n"), unit, match_id));
        retval = bcm_flexdigest_match_id_destroy(unit, match_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_entry_destroy");
        return CMD_OK;
    }
    return CMD_USAGE;
}

static int
fd_match_add(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_match_id_t match_id;
    bcm_flexdigest_match_t match;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(match_id, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        match = parse_integer(subcmd);
    } else {
        match = _fd_match_parse(subcmd);
        if (match == bcmFlexDigestMatchCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown match: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: match add match_id=%d, match=%s\n"),
                            unit, match_id, _fd_match_format(buf, match, 1)));
#endif /* BROADCOM_DEBUG */
    retval = bcm_flexdigest_match_add(unit, match_id, match);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_match_add");

    return CMD_OK;
}

static int
fd_match_delete(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_match_id_t match_id;

    bcm_flexdigest_match_t match;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(match_id, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        match = parse_integer(subcmd);
    } else {
        match = _fd_match_parse(subcmd);
        if (match == bcmFlexDigestMatchCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown match: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: match delete match_id=%d, match=%s\n"),
                            unit, match_id, _fd_match_format(buf, match, 1)));
#endif /* BROADCOM_DEBUG */
    retval = bcm_flexdigest_match_delete(unit, match_id, match);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_match_delete");

    return CMD_OK;
}

static int
fd_match_clear(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_match_id_t match_id;

    FD_GET_NUMB(match_id, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        retval = bcm_flexdigest_match_delete_all(unit, match_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_match_delete_all");
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

static int
fd_match_show(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_match_id_t match_id;
    bcm_flexdigest_match_t *match_array = NULL;
    int count, ref_count;
#ifdef BROADCOM_DEBUG
    int i;
    char buf[FD_ACTION_WIDTH_MAX];
    bcm_flexdigest_match_t match;
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(match_id, subcmd, args);

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: match show match_id=%ds\n"),
                            unit, match_id));
#endif /* BROADCOM_DEBUG */
    retval = bcm_flexdigest_match_get(unit, match_id, 0, NULL, &count);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_match_get");
    match_array = sal_alloc(sizeof(bcm_flexdigest_match_t) * count, "bcmFlexDigestMatchArray");
    if (match_array == NULL) {
        return CMD_FAIL;
    }
    sal_memset(match_array, 0, sizeof(bcm_flexdigest_match_t) * count);
    retval = bcm_flexdigest_match_get(unit, match_id, count, match_array, &count);
    if (BCM_FAILURE(retval)) {
        cli_out("FD(unit %d) Error: %s() failed: %s\n", (unit),
                "bcm_flexdigest_match_get", bcm_errmsg(retval));
        sal_free(match_array);
        return CMD_FAIL;
    }
    retval = bcmi_ltsw_flexdigest_match_id_ref_count_get(unit, match_id, &ref_count);
    if (BCM_FAILURE(retval)) {
        cli_out("FD(unit %d) Error: %s() failed: %s\n", (unit),
                "bcmi_ltsw_flexdigest_match_id_ref_count_get", bcm_errmsg(retval));
        sal_free(match_array);
        return CMD_FAIL;
    }
    LOG_CLI((BSL_META("FD(unit %d) The matches in MatchId(%d) are: [Ref.%d]\n"), unit, match_id, ref_count));
#ifdef BROADCOM_DEBUG
    for (i = 0; i < count; i++) {
        match = match_array[i];
        LOG_CLI((BSL_META("\t%s\n"), _fd_match_format(buf, match, 1)));
    }
#endif /* BROADCOM_DEBUG */
    sal_free(match_array);
    return CMD_OK;
}

static int
fd_match(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd match create */
    if (!sal_strcasecmp(subcmd, "create")) {
        return fd_match_create(unit, args);
    }
    /* BCM.0> fd match destroy ... */
    if (!sal_strcasecmp(subcmd, "destroy")) {
        return fd_match_destroy(unit, args);
    }
    /* BCM.0> fd match add ... */
    if (!sal_strcasecmp(subcmd, "add")) {
        return fd_match_add(unit, args);
    }
    /* BCM.0> fd match delete ... */
    if (!sal_strcasecmp(subcmd, "delete")) {
        return fd_match_delete(unit, args);
    }
    /* BCM.0> fd match clear ... */
    if (!sal_strcasecmp(subcmd, "clear")) {
        return fd_match_clear(unit, args);
    }
    /* BCM.0> fd match show ... */
    if (!sal_strcasecmp(subcmd, "show")) {
        return fd_match_show(unit, args);
    }

    return CMD_USAGE;
}

static int
fd_qual_MatchId(
    int unit,
    bcm_flexdigest_entry_t eid,
    args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    uint16 match_id;

    FD_GET_NUMB(match_id, subcmd, args);

    /* BCM.0> fd qual 'eid' MatchId 'data' */
    retval = bcm_flexdigest_qualify_MatchId(unit, eid, match_id);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_qualify_MatchId");

    return CMD_OK;
}

static int
fd_qual_8(
    int unit,
    bcm_flexdigest_entry_t eid,
    args_t *args,
    int func(int, bcm_flexdigest_entry_t, uint8, uint8),
    char *qual_str)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    uint8 data, mask;
    char str[256];

    FD_GET_NUMB(data, subcmd, args);
    FD_GET_NUMB(mask, subcmd, args);

    /* BCM.0> fd qual 'eid' Qual 'data' 'mask' */
    retval = func(unit, eid, data, mask);
    strcpy(str, "bcm_flexdigest_qualify_");
    strncat(str, qual_str, 256 - 1 - strlen(str));
    FD_CHECK_RETURN(unit, retval, str);

    return CMD_OK;
}

static int
fd_qual(int unit, args_t *args)
{
    char *subcmd   = NULL;
    char *qual_str = NULL;
    bcm_flexdigest_entry_t eid;
    int rv = CMD_OK;
    bcm_flexdigest_qualify_t qual = bcmFlexDigestQualifyCount;

    FD_GET_NUMB(eid, subcmd, args);
    /* > fd qual 'eid' ...*/

    if ((qual_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /*
     * Argument exception: "clear" does not correspond directly to
     * any bcmFlexDigestQualify* enum.
     */
    if(!sal_strcasecmp(qual_str, "clear")) {
        /* BCM.0> fd qual 'eid' clear  */
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb:fd_qual_delete_all 'eid=%d'\n"), unit, eid));
        rv = bcm_flexdigest_qualifier_delete_all(unit, eid);
        FD_CHECK_RETURN(unit, rv, "bcm_flexdigest_qualifier_delete_all");

        return rv;
    }

    /*
     * Argument exception: "delete" does not correspond directly to
     * any bcmFlexDigestQualify* enum.
     */
    if (!sal_strcasecmp(qual_str, "delete")) {
        if ((qual_str = ARG_GET(args)) == NULL) {
            rv = bcm_flexdigest_qualifier_delete_all(unit, eid);
            FD_CHECK_RETURN(unit, rv, "bcm_flexdigest_qualifier_delete_all");
            return CMD_OK;
        }
        qual = _fd_qualifier_parse(qual_str);
        /* BCM.0> fd qual 'eid' delete 'qual_name' */
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb:fd_qual_delete  'eid=%d qual=%s'\n"), unit, eid, qual_str));
        rv = bcm_flexdigest_qualifier_delete(unit, eid, qual);
        FD_CHECK_RETURN(unit, rv, "bcm_flexdigest_qualifier_delete");

        return rv;
    }

    subcmd = ARG_GET(args);
    if (subcmd != NULL) {
        if ((!sal_strcasecmp(subcmd, "show")) ||
            (!sal_strcasecmp(subcmd, "?"))) {
        } else {
            ARG_PREV(args);
        }
    }

    qual = _fd_qualifier_parse(qual_str);

    /* > fd qual 'eid' bcmFlexDigestQualifyXXX ...*/
    switch (qual) {
    case bcmFlexDigestQualifyMatchId:
        rv = fd_qual_MatchId(unit, eid, args);
        break;
    case bcmFlexDigestQualifyL4Valid:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_L4Valid,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyMyStation1Hit:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_MyStation1Hit,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyTunnelProcessingResults1:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_TunnelProcessingResults1,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyTunnelProcessingResultsRaw:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_TunnelProcessingResultsRaw,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyFlexHve2ResultSet1:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_FlexHve2ResultSet1,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyFixedHve2ResultSet0:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_FixedHve2ResultSet0,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyFlexHve1ResultSet1:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_FlexHve1ResultSet1,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyFixedHve1ResultSet0:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_FixedHve1ResultSet0,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyL2IifFlexDigestCtrlIdB:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_L2IifFlexDigestCtrlIdB,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyL2IifFlexDigestCtrlIdA:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_L2IifFlexDigestCtrlIdA,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyProtocolPktOpaqueCtrlId:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_ProtocolPktOpaqueCtrlId,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyVfiOpaqueCtrlId:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_VfiOpaqueCtrlId,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifySvpOpaqueCtrlId:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_SvpOpaqueCtrlId,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyL3IifOpaqueCtrlId:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_L3IifOpaqueCtrlId,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyL2IifOpaqueCtrlId:
        rv = fd_qual_8(unit, eid, args, bcm_flexdigest_qualify_L2IifOpaqueCtrlId,
                     _fd_qualifier_name(qual));
        break;
    case bcmFlexDigestQualifyCount:
    default:
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "FD(unit %d) Error: Unknown qualifier: %s\n"), unit, qual_str));
        rv = CMD_FAIL;
    }

    if (CMD_OK != rv) {
        LOG_ERROR(BSL_LS_APPL_SHELL,
                  (BSL_META_U(unit,
                              "FD(unit %d) Error: Qualifier installation error: %s\n"),
                              unit, qual_str));
    }

    return rv;
}

static int
fd_action_add(int unit, args_t *args)
{
    char *subcmd = NULL;
    bcm_flexdigest_entry_t eid;
    bcm_flexdigest_action_t action;
    int retval;
    int p0 = 0, p1 = 0;
#ifdef BROADCOM_DEBUG
    char                buf[FD_ACTION_WIDTH_MAX];
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(eid, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        action = parse_integer(subcmd);
    } else {
        action = _fd_action_parse(subcmd);
        if (action == bcmFlexDigestActionCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown action: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }

    /* Read the action parameters (p0 and p1).*/
    switch (action) {
        case bcmFlexDigestActionExtractBinSetACmds0:
        case bcmFlexDigestActionExtractBinSetACmds1:
        case bcmFlexDigestActionExtractBinSetACmds2:
        case bcmFlexDigestActionExtractBinSetACmds3:
        case bcmFlexDigestActionExtractBinSetACmds4:
        case bcmFlexDigestActionExtractBinSetACmds5:
        case bcmFlexDigestActionExtractBinSetACmds6:
        case bcmFlexDigestActionExtractBinSetACmds7:
        case bcmFlexDigestActionExtractBinSetACmds8:
        case bcmFlexDigestActionExtractBinSetACmds9:
        case bcmFlexDigestActionExtractBinSetACmds10:
        case bcmFlexDigestActionExtractBinSetACmds11:
        case bcmFlexDigestActionExtractBinSetACmds12:
        case bcmFlexDigestActionExtractBinSetACmds13:
        case bcmFlexDigestActionExtractBinSetACmds14:
        case bcmFlexDigestActionExtractBinSetACmds15:
            FD_GET_NUMB(p0, subcmd, args);
            FD_GET_NUMB(p1, subcmd, args);
            break;
        case bcmFlexDigestActionExtractBinSetBCmds0:
        case bcmFlexDigestActionExtractBinSetBCmds1:
        case bcmFlexDigestActionExtractBinSetBCmds2:
        case bcmFlexDigestActionExtractBinSetBCmds3:
        case bcmFlexDigestActionExtractBinSetBCmds4:
        case bcmFlexDigestActionExtractBinSetBCmds5:
        case bcmFlexDigestActionExtractBinSetBCmds6:
        case bcmFlexDigestActionExtractBinSetBCmds7:
        case bcmFlexDigestActionExtractBinSetBCmds8:
        case bcmFlexDigestActionExtractBinSetBCmds9:
        case bcmFlexDigestActionExtractBinSetBCmds10:
        case bcmFlexDigestActionExtractBinSetBCmds11:
        case bcmFlexDigestActionExtractBinSetBCmds12:
        case bcmFlexDigestActionExtractBinSetBCmds13:
        case bcmFlexDigestActionExtractBinSetBCmds14:
        case bcmFlexDigestActionExtractBinSetBCmds15:
            FD_GET_NUMB(p0, subcmd, args);
            FD_GET_NUMB(p1, subcmd, args);
            break;
        case bcmFlexDigestActionExtractBinSetCCmds0:
        case bcmFlexDigestActionExtractBinSetCCmds1:
        case bcmFlexDigestActionExtractBinSetCCmds2:
        case bcmFlexDigestActionExtractBinSetCCmds3:
        case bcmFlexDigestActionExtractBinSetCCmds4:
        case bcmFlexDigestActionExtractBinSetCCmds5:
        case bcmFlexDigestActionExtractBinSetCCmds6:
        case bcmFlexDigestActionExtractBinSetCCmds7:
        case bcmFlexDigestActionExtractBinSetCCmds8:
        case bcmFlexDigestActionExtractBinSetCCmds9:
        case bcmFlexDigestActionExtractBinSetCCmds10:
        case bcmFlexDigestActionExtractBinSetCCmds11:
        case bcmFlexDigestActionExtractBinSetCCmds12:
        case bcmFlexDigestActionExtractBinSetCCmds13:
        case bcmFlexDigestActionExtractBinSetCCmds14:
        case bcmFlexDigestActionExtractBinSetCCmds15:
            FD_GET_NUMB(p0, subcmd, args);
            FD_GET_NUMB(p1, subcmd, args);
            break;
        default:
            /* Use defaults of p0 and p1 for other actions */
            break;
    }
#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: action add eid=%d, action=%s, p0=0x%x, p1=0x%x\n"),
                            unit, eid, _fd_action_format(buf, action, 1), p0, p1));
#endif /* BROADCOM_DEBUG */
    retval = bcm_flexdigest_action_add(unit, eid, action, p0, p1);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_action_add");

    return CMD_OK;
}

static int
fd_action_get(int unit, args_t *args)
{
    char *subcmd = NULL;
    bcm_flexdigest_entry_t eid;
    int retval;
    bcm_flexdigest_action_t action;
    uint16 p0 = 0, p1 = 0;
    char buf[FD_ACTION_WIDTH_MAX];

    FD_GET_NUMB(eid, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        action = parse_integer(subcmd);
    } else {
        action = _fd_action_parse(subcmd);
        if (action == bcmFlexDigestActionCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown action: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }

    retval = bcm_flexdigest_action_get(unit, eid, action, &p0, &p1);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_action_get");
    cli_out("FD action get: action=%s, p0=%d, p1=%d\n",
            _fd_action_format(buf, action, 1), p0, p1);

    return CMD_OK;
}

static int
fd_action_remove(int unit, args_t *args)
{
    char *subcmd = NULL;
    bcm_flexdigest_entry_t eid;
    int retval;
    bcm_flexdigest_action_t action;

    FD_GET_NUMB(eid, subcmd, args);

    if ((subcmd = ARG_GET(args)) == NULL) {
        retval = bcm_flexdigest_action_delete_all(unit, eid);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_action_delete_all");
    } else {
        if (isint(subcmd)) {
            action = parse_integer(subcmd);
        } else {
            action = _fd_action_parse(subcmd);
            if (action == bcmFlexDigestActionCount) {
                LOG_ERROR(BSL_LS_APPL_SHELL,
                          (BSL_META_U(unit,
                                      "FD(unit %d) Error: Unrecognized action: %s\n"), unit, subcmd));
                return CMD_FAIL;
            }
        }
        retval = bcm_flexdigest_action_delete(unit, eid, action);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_action_delete");
    }

    return CMD_OK;
}

static int
fd_action(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd action add ... */
    if (!sal_strcasecmp(subcmd, "add")) {
        return fd_action_add(unit, args);
    }
    /* BCM.0> fd action get ... */
    if (!sal_strcasecmp(subcmd, "get")) {
        return fd_action_get(unit, args);
    }
    /* BCM.0> fd action remove... */
    if (!sal_strcasecmp(subcmd, "remove")) {
        return fd_action_remove(unit, args);
    }

    return CMD_USAGE;
}

static int
fd_norm_seed_profile_create(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int seed_profile_id = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fd norm profile create 'profile_id'  */
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _norm_seed_profile_create()\n"), unit));
        retval = bcm_flexdigest_norm_seed_profile_create(unit, 0, &seed_profile_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_seed_profile_create");
    } else {
        /* BCM.0> fd norm profile create 'profile_id' */
        seed_profile_id = parse_integer(subcmd);
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _norm_seed_profile_create seed_profile_id=%d\n"), unit, seed_profile_id));
        retval = bcm_flexdigest_norm_seed_profile_create(unit,
                BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &seed_profile_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_seed_profile_create");
    }

    cli_out("FD norm seed profile create: profile_id = %d\n", seed_profile_id);

    return CMD_OK;
}

static int
fd_norm_seed_profile_destroy(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int seed_profile_id = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _norm_seed_profile_destroy_all()\n"), unit));
        retval = bcm_flexdigest_norm_seed_profile_destroy_all(unit);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_seed_profile_destroy_all");
        return CMD_OK;
    } else {
        seed_profile_id = parse_integer(subcmd);
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _norm_seed_profile_destroy(norm_profile_id=%d)\n"),
                                unit, seed_profile_id));
        retval = bcm_flexdigest_norm_seed_profile_destroy(unit, seed_profile_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_seed_profile_destroy");
        return CMD_OK;
    }
    return CMD_USAGE;
}

static int
fd_norm_seed_profile_set(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int seed_profile_id = 0;
    bcm_flexdigest_norm_seed_control_t type;
    int value;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(seed_profile_id, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        type = parse_integer(subcmd);
    } else {
        type = _fd_norm_seed_control_type_parse(subcmd);
        if (type == bcmFlexDigestNormSeedControlCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown control type: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }
    FD_GET_NUMB(value, subcmd, args);

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: norm seed profile set profile_id=%d, type=%s\n"),
                            unit, seed_profile_id, _fd_norm_seed_control_type_format(buf, type, 1)));
#endif /* BROADCOM_DEBUG */
    retval = bcm_flexdigest_norm_seed_profile_set(unit, seed_profile_id, type, value);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_seed_profile_set");

    return CMD_OK;
}

static int
fd_norm_seed_profile_get(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int seed_profile_id = 0;
    bcm_flexdigest_norm_seed_control_t type;
    int value;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(seed_profile_id, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        type = parse_integer(subcmd);
    } else {
        type = _fd_norm_seed_control_type_parse(subcmd);
        if (type == bcmFlexDigestNormSeedControlCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown control type: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }

    retval = bcm_flexdigest_norm_seed_profile_get(unit, seed_profile_id, type, &value);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_seed_profile_get");
#ifdef BROADCOM_DEBUG
    cli_out("FD(unit %d) norm seed profile: profile_id=%d %s=0x%08x\n",
            unit, seed_profile_id, _fd_norm_seed_control_type_format(buf, type, 1), value);
#endif /* BROADCOM_DEBUG */
    return CMD_OK;
}

static int
fd_norm_seed_profile(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd seed seed profile create ... */
    if (!sal_strcasecmp(subcmd, "create")) {
        return fd_norm_seed_profile_create(unit, args);
    }
    /* BCM.0> fd norm seed profile destroy ... */
    if (!sal_strcasecmp(subcmd, "destroy")) {
        return fd_norm_seed_profile_destroy(unit, args);
    }
    /* BCM.0> fd norm seed profile set... */
    if (!sal_strcasecmp(subcmd, "set")) {
        return fd_norm_seed_profile_set(unit, args);
    }
    /* BCM.0> fd norm seed profile get... */
    if (!sal_strcasecmp(subcmd, "get")) {
        return fd_norm_seed_profile_get(unit, args);
    }

    return CMD_USAGE;
}


static int
fd_norm_seed(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd norm seed profile ... */
    if (!sal_strcasecmp(subcmd, "profile")) {
        return fd_norm_seed_profile(unit, args);
    }

    return CMD_USAGE;
}

static int
fd_norm_profile_create(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int norm_profile_id = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fd norm profile create 'profile_id'  */
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _norm_profile_create()\n"), unit));
        retval = bcm_flexdigest_norm_profile_create(unit, 0, &norm_profile_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_profile_create");
    } else {
        /* BCM.0> fd norm profile create 'profile_id' */
        norm_profile_id = parse_integer(subcmd);
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _norm_profile_create profile_id=%d\n"), unit, norm_profile_id));
        retval = bcm_flexdigest_norm_profile_create(unit,
                BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &norm_profile_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_profile_create");
    }

    cli_out("FD norm profile create: profile_id = %d\n", norm_profile_id);

    return CMD_OK;
}

static int
fd_norm_profile_destroy(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int norm_profile_id = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _norm_profile_destroy_all()\n"), unit));
        retval = bcm_flexdigest_norm_profile_destroy_all(unit);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_profile_destroy_all");
        return CMD_OK;
    } else {
        norm_profile_id = parse_integer(subcmd);
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _norm_profile_destroy(norm_profile_id=%d)\n"),
                                unit, norm_profile_id));
        retval = bcm_flexdigest_norm_profile_destroy(unit, norm_profile_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_profile_destroy");
        return CMD_OK;
    }
    return CMD_USAGE;
}

static int
fd_norm_profile_set(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int norm_profile_id = 0;
    bcm_flexdigest_norm_profile_control_t type;
    int value;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(norm_profile_id, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        type = parse_integer(subcmd);
    } else {
        type = _fd_norm_profile_control_type_parse(subcmd);
        if (type == bcmFlexDigestNormProfileControlCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown control type: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }
    FD_GET_NUMB(value, subcmd, args);

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: norm profile set profile_id=%d, type=%s\n"),
                            unit, norm_profile_id, _fd_norm_profile_control_type_format(buf, type, 1)));
#endif
    retval = bcm_flexdigest_norm_profile_set(unit, norm_profile_id, type, value);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_profile_set");

    return CMD_OK;
}

static int
fd_norm_profile_get(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int norm_profile_id = 0;
    bcm_flexdigest_norm_profile_control_t type;
    int value;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(norm_profile_id, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        type = parse_integer(subcmd);
    } else {
        type = _fd_norm_profile_control_type_parse(subcmd);
        if (type == bcmFlexDigestNormProfileControlCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown control type: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }

    retval = bcm_flexdigest_norm_profile_get(unit, norm_profile_id, type, &value);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_norm_profile_get");
#ifdef BROADCOM_DEBUG
    cli_out("FD(unit %d) norm profile: profile_id=%d, %s=%d\n",
            unit, norm_profile_id, _fd_norm_profile_control_type_format(buf, type, 1), value);
#endif /* BROADCOM_DEBUG */

    return CMD_OK;
}

static int
fd_norm_profile(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd norm profile create ... */
    if (!sal_strcasecmp(subcmd, "create")) {
        return fd_norm_profile_create(unit, args);
    }
    /* BCM.0> fd norm profile destroy ... */
    if (!sal_strcasecmp(subcmd, "destroy")) {
        return fd_norm_profile_destroy(unit, args);
    }
    /* BCM.0> fd norm profile set... */
    if (!sal_strcasecmp(subcmd, "set")) {
        return fd_norm_profile_set(unit, args);
    }
    /* BCM.0> fd norm profile get... */
    if (!sal_strcasecmp(subcmd, "get")) {
        return fd_norm_profile_get(unit, args);
    }

    return CMD_USAGE;
}



static int
fd_norm(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd norm seed ... */
    if (!sal_strcasecmp(subcmd, "seed")) {
        return fd_norm_seed(unit, args);
    }
    /* BCM.0> fd norm profile ... */
    if (!sal_strcasecmp(subcmd, "profile")) {
        return fd_norm_profile(unit, args);
    }

    return CMD_USAGE;
}

static int
fd_hash_profile_create(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int hash_profile_id = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        /* BCM.0> fd norm profile create 'profile_id'  */
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _hash_profile_create()\n"), unit));
        retval = bcm_flexdigest_hash_profile_create(unit, 0, &hash_profile_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_hash_profile_create");
    } else {
        /* BCM.0> fd norm profile create 'profile_id' */
        hash_profile_id = parse_integer(subcmd);
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _hash_profile_create profile_id=%d\n"), unit, hash_profile_id));
        retval = bcm_flexdigest_hash_profile_create(unit,
                BCM_FLEXDIGEST_PROFILE_CREATE_OPTIONS_WITH_ID, &hash_profile_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_hash_profile_create");
    }

    cli_out("FD hash profile create: profile_id = %d\n", hash_profile_id);

    return CMD_OK;
}

static int
fd_hash_profile_destroy(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int hash_profile_id = 0;

    if ((subcmd = ARG_GET(args)) == NULL) {
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _hash_profile_destroy_all()\n"), unit));
        retval = bcm_flexdigest_hash_profile_destroy_all(unit);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_hash_profile_destroy_all");
        return CMD_OK;
    } else {
        hash_profile_id = parse_integer(subcmd);
        LOG_VERBOSE(BSL_LS_APPL_SHELL,
                    (BSL_META_U(unit,
                                "FD(unit %d) verb: _hash_profile_destroy(hash_profile_id=%d)\n"),
                                unit, hash_profile_id));
        retval = bcm_flexdigest_hash_profile_destroy(unit, hash_profile_id);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_hash_profile_destroy");
        return CMD_OK;
    }
    return CMD_USAGE;
}

static int
fd_hash_profile_set(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int hash_profile_id = 0;
    bcm_flexdigest_hash_profile_control_t type;
    int value;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(hash_profile_id, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        type = parse_integer(subcmd);
    } else {
        type = _fd_hash_profile_control_type_parse(subcmd);
        if (type == bcmFlexDigestHashProfileControlCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown control type: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }
    FD_GET_NUMB(value, subcmd, args);

#ifdef BROADCOM_DEBUG
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: hash profile set profile_id=%d, type=%s\n"),
                            unit, hash_profile_id, _fd_hash_profile_control_type_format(buf, type, 1)));
#endif /* BROADCOM_DEBUG */
    retval = bcm_flexdigest_hash_profile_set(unit, hash_profile_id, type, value);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_hash_profile_set");

    return CMD_OK;
}

static int
fd_hash_profile_get(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    int hash_profile_id = 0;
    bcm_flexdigest_hash_profile_control_t type;
    int value;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
#endif /* BROADCOM_DEBUG */

    FD_GET_NUMB(hash_profile_id, subcmd, args);
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        type = parse_integer(subcmd);
    } else {
        type = _fd_hash_profile_control_type_parse(subcmd);
        if (type == bcmFlexDigestHashProfileControlCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown control type: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }

    retval = bcm_flexdigest_hash_profile_get(unit, hash_profile_id, type, &value);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_hash_profile_get");
#ifdef BROADCOM_DEBUG
    cli_out("FD(unit %d) hash profile: profile_id=%d, %s=%d\n",
            unit, hash_profile_id, _fd_hash_profile_control_type_format(buf, type, 1), value);
#endif /* BROADCOM_DEBUG */

    return CMD_OK;
}

static int
fd_hash_profile(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd hash profile create ... */
    if (!sal_strcasecmp(subcmd, "create")) {
        return fd_hash_profile_create(unit, args);
    }
    /* BCM.0> fd hash profile destroy ... */
    if (!sal_strcasecmp(subcmd, "destroy")) {
        return fd_hash_profile_destroy(unit, args);
    }
    /* BCM.0> fd hash profile set... */
    if (!sal_strcasecmp(subcmd, "set")) {
        return fd_hash_profile_set(unit, args);
    }
    /* BCM.0> fd hash profile get... */
    if (!sal_strcasecmp(subcmd, "get")) {
        return fd_hash_profile_get(unit, args);
    }

    return CMD_USAGE;
}

static int
fd_hash_salt_set(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_hash_bin_set_t bin_set;
    int array_size;
    uint16 *salt = NULL;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
    char buf1[FD_SALT_STR_LEN];
#endif /* BROADCOM_DEBUG */

    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        bin_set = parse_integer(subcmd);
    } else {
        bin_set = _fd_hash_bin_set_parse(subcmd);
        if (bin_set == bcmFlexDigestHashBinCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown bin set: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }
    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    array_size = comma_count(subcmd) + 1;
    salt = sal_alloc(sizeof(uint16) * array_size, "FlexDigestSalt");
    if (salt == NULL) {
        return CMD_FAIL;
    }
    retval = _fd_salt_parse(subcmd, array_size, salt);
    if (BCM_FAILURE(retval)) {
        cli_out("FD(unit %d) Error: %s() failed: %s\n", (unit),
                "_fd_salt_parse", bcm_errmsg(retval));
        sal_free(salt);
        return CMD_FAIL;
    }

#ifdef BROADCOM_DEBUG
    _fd_salt_format(buf1, array_size, salt);
    LOG_VERBOSE(BSL_LS_APPL_SHELL,
                (BSL_META_U(unit,
                            "FD(unit %d) verb: hash salt set bin_set=%s, array_size=%d, value={%s}\n"),
                            unit, _fd_hash_bin_set_format(buf, bin_set, 1), array_size,
                            buf1));
#endif /* BROADCOM_DEBUG */

    retval = bcm_flexdigest_hash_salt_set(unit, bin_set, array_size, salt);
    if (BCM_FAILURE(retval)) {
        cli_out("FD(unit %d) Error: %s() failed: %s\n", (unit),
                "bcm_flexdigest_hash_salt_set", bcm_errmsg(retval));
        sal_free(salt);
        return CMD_FAIL;
    }
    sal_free(salt);

    return CMD_OK;
}

static int
fd_hash_salt_get(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    bcm_flexdigest_hash_bin_set_t bin_set;
    int array_size;
    uint16 *salt = NULL;
#ifdef BROADCOM_DEBUG
    char buf[FD_ACTION_WIDTH_MAX];
    char buf1[FD_SALT_STR_LEN];
#endif /* BROADCOM_DEBUG */

    if ((subcmd  = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    if (isint(subcmd)) {
        bin_set = parse_integer(subcmd);
    } else {
        bin_set = _fd_hash_bin_set_parse(subcmd);
        if (bin_set == bcmFlexDigestHashBinCount) {
            LOG_ERROR(BSL_LS_APPL_SHELL,
                      (BSL_META_U(unit,
                                  "FD(unit %d) Error: Unknown bin set: %s\n"), unit, subcmd));
            return CMD_FAIL;
        }
    }
    retval = bcm_flexdigest_hash_salt_get(unit, bin_set, 0, NULL, &array_size);
    FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_hash_profile_get");

    salt = sal_alloc(sizeof(uint16) * array_size, "FlexDigestSalt");
    if (salt == NULL) {
        return CMD_FAIL;
    }

    retval = bcm_flexdigest_hash_salt_get(unit, bin_set, array_size, salt, &array_size);
    if (BCM_FAILURE(retval)) {
        cli_out("FD(unit %d) Error: %s() failed: %s\n", (unit),
                "bcm_flexdigest_hash_salt_get", bcm_errmsg(retval));
        sal_free(salt);
        return CMD_FAIL;
    }

#ifdef BROADCOM_DEBUG
    _fd_salt_format(buf1, array_size, salt);
    cli_out("FD(unit %d) hash salt get bin_set=%s, array_size=%d, salt={%s}\n",
            unit, _fd_hash_bin_set_format(buf, bin_set, 1),
            array_size, buf1);
#endif /* BROADCOM_DEBUG */

    sal_free(salt);

    return CMD_OK;
}


static int
fd_hash_salt(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd hash profile set... */
    if (!sal_strcasecmp(subcmd, "set")) {
        return fd_hash_salt_set(unit, args);
    }
    /* BCM.0> fd hash profile get... */
    if (!sal_strcasecmp(subcmd, "get")) {
        return fd_hash_salt_get(unit, args);
    }

    return CMD_USAGE;
}


static int
fd_hash(int unit, args_t *args)
{
    char *subcmd = NULL;

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd hash profile ... */
    if (!sal_strcasecmp(subcmd, "profile")) {
        return fd_hash_profile(unit, args);
    }

    /* BCM.0> fd hash profile ... */
    if (!sal_strcasecmp(subcmd, "salt")) {
        return fd_hash_salt(unit, args);
    }

    return CMD_USAGE;
}


/*!
 * \brief Manage Flex Digest (FD)
 *
 * \param [in] unit Unit Number.
 * \param [in] args pointer to command line arguments.
 * \param [out] entry The entry identifier.
 *
 * \retval CMD_OK Success.
 * \retval !CMD_OK Failure.
 */
cmd_result_t
cmd_ltsw_flexdigest(int unit, args_t *args)
{
    int retval = CMD_OK;
    char *subcmd = NULL;
    static bcm_flexdigest_qset_t qset;
#ifdef BROADCOM_DEBUG
    bcm_flexdigest_group_t gid;
    bcm_flexdigest_entry_t eid = 0;
#endif /* BROADCOM_DEBUG */

    if ((subcmd = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* BCM.0> fd action ... */
    if (!sal_strcasecmp(subcmd, "action")) {
        return fd_action(unit, args);
    }

    /* BCM.0> fd detach */
    if (!sal_strcasecmp(subcmd, "detach")) {
        retval = bcm_flexdigest_detach(unit);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_detach");
        return CMD_OK;
    }

    /* BCM.0> fd entry ... */
    if (!sal_strcasecmp(subcmd, "entry")) {
        return fd_entry(unit, args);
    }

    /* BCM.0> fd group ... */
    if (!sal_strcasecmp(subcmd, "group")) {
        return fd_group(unit, args, &qset);
    }

    /* BCM.0> fd norm ... */
    if (!sal_strcasecmp(subcmd, "hash")) {
        return fd_hash(unit, args);
    }

    /* BCM.0> fd init */
    if (!sal_strcasecmp(subcmd, "init")) {
        retval = bcm_flexdigest_init(unit);
        FD_CHECK_RETURN(unit, retval, "bcm_flexdigest_init");
        BCM_FLEXDIGEST_QSET_INIT(qset);
        return CMD_OK;
    }

    /* BCM.0> fd match ... */
    if (!sal_strcasecmp(subcmd, "match")) {
        return fd_match(unit, args);
    }

    /* BCM.0> fd norm ... */
    if (!sal_strcasecmp(subcmd, "norm")) {
        return fd_norm(unit, args);
    }

    /* BCM.0> fd qset ... */
    if (!sal_strcasecmp(subcmd, "qset")) {
        return fd_qset(unit, args, &qset);
    }

    /* BCM.0> fd qual ... */
    if (!sal_strcasecmp(subcmd, "qual")) {
        return fd_qual(unit, args);
    }

#ifdef BROADCOM_DEBUG
    /* BCM.0> fd show ... */
    if (!sal_strcasecmp(subcmd, "show")) {
        if ((subcmd = ARG_GET(args)) != NULL) {
            /* BCM.0> fd show group ... */
            if (!sal_strcasecmp(subcmd, "group")) {
                FD_GET_NUMB(gid, subcmd, args);
                bcm_flexdigest_group_dump(unit, gid);
                return CMD_OK;
            }
            /* BCM.0> fd show entry ... */
            if (!sal_strcasecmp(subcmd, "entry")) {
                FD_GET_NUMB(eid, subcmd, args);
                bcm_flexdigest_entry_dump(unit, eid);
                return CMD_OK;
            }
            /* BCM.0> fd show qset */
            if (!sal_strcasecmp(subcmd, "qset")) {
                fd_qset_show(&qset);
                return CMD_OK;
            }
            return CMD_NFND;
        } else {
            /* BCM.0> fd show */
            bcm_flexdigest_show(unit, "FD");
            return CMD_OK;
        }
    }
#endif /* BROADCOM_DEBUG */

    return CMD_USAGE;
}

