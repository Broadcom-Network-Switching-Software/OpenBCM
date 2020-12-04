/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shared/dbx/dbx_pre_compiled_xml_parser_internal.h>
#if defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA)
#include <soc/dpp/dpp_pre_compiled_xml_parser.h>
#endif /* defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA) */
#include <sal/core/libc.h>
#include <shared/dbx/dbx_xml.h>

#if defined(BCM_DNX_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNXF_SUPPORT)

void *
dbx_xml_top_get_internal(
    char *filepath,
    char *topname,
    int flags,
    dbx_pre_compiled_xml_to_buf_cb xml_to_buf)
{
    xmlDocPtr doc;
    xmlNodePtr curTop = NULL;
    char *buf = NULL;
    long int size = 0;

    /*
     * copy file contents into memory buffer
     */
    if (xml_to_buf(filepath, &buf, &size) != _SHR_E_NONE)
    {
        if (flags & CONF_OPEN_CREATE)
            curTop = dbx_xml_top_create(topname);
        goto exit;
    }
    else if (flags & CONF_OPEN_OVERWRITE)
    {
        curTop = dbx_xml_top_create(topname);
        goto exit;
    }

    /*
     * parse buffer: this will build an hierarchy tree
     */
    if ((doc = xmlParseMemory(buf, size)) == NULL)
    {
        goto exit;
    }

    /*
     * get the tree root
     */
    if ((curTop = xmlDocGetRootElement(doc)) == NULL)
    {
        xmlFreeDoc(doc);
        goto exit;
    }

    if (xmlStrcmp(curTop->name, (const xmlChar *) topname))
    {
        /*
         * Bad file format "rules" should be the top entry always
         */
        curTop = NULL;
        xmlFreeDoc(doc);
        goto exit;
    }

exit:
    if (buf)
    {
        /*
         * We shouldn't fix such defects
         */
         /* coverity[tainted_data : FALSE]  */
        sal_free(buf);
    }
    return curTop;
}

static char
decode_char(
    char *input,
    char key)
{
    char buf[4];
    int ascii_code = 0;

    sal_memset(buf, 0, 4);

    if (input[0] != '0')
    {
        sal_strncpy(buf, input, 3);
    }
    else if (input[1] != '0')
    {
        sal_strncpy(buf, input + 1, 2);
    }
    else
    {
        sal_strncpy(buf, input + 2, 1);
    }

    ascii_code = sal_atoi(buf) ^ (int) key;

    return (char) ascii_code;
}

void
pre_compiled_xml_to_decoded_buf(
    char *key,
    char *input,
    char **output,
    long int *size)
{
    char *buf;
    char *decoded_string;
    int index = 0;
    int offset = 0;
    int key_index = 0;
    int coded_len = 0;

    buf = sal_alloc(sal_strlen(input), "buf");
    sal_memset(buf, 0, sal_strlen(input));

    coded_len = sal_strlen(input);

    while (index <= coded_len)
    {
        if (input[index] >= '0' && input[index] <= '9')
        {
            buf[offset] = decode_char(input + index, key[key_index % sal_strlen(key)]);
            key_index++;
            index = index + 3;
        }
        else
        {
            sal_strncpy(buf + offset, input + index, 1);
            index++;
        }
        offset++;
    }

    *size = sal_strlen(buf);

    decoded_string = sal_alloc(index, "buf");
    sal_strncpy(decoded_string, buf, index);
    *output = decoded_string;

    sal_free(buf);
}

#if defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA)
void *
pre_compiled_dbx_xml_top_get(
    int unit,
    char *filepath,
    char *topname,
    int flags)
{
    xmlDocPtr doc;
    xmlNodePtr curTop = NULL;
    char *buf = NULL;
    off_t size = 0;

    /*
     * copy file contents into memory buffer
     */
    if (pre_compiled_xml_to_buf(unit, filepath, &buf, &size) != _SHR_E_NONE)
    {
        if (flags & CONF_OPEN_CREATE)
            curTop = dbx_xml_top_create(topname);
        goto exit;
    }
    else if (flags & CONF_OPEN_OVERWRITE)
    {
        curTop = dbx_xml_top_create(topname);
        goto exit;
    }

    /*
     * parse buffer: this will build an hierarchy tree
     */
    if ((doc = xmlParseMemory(buf, size)) == NULL)
    {
        goto exit;
    }

    /*
     * get the tree root
     */
    if ((curTop = xmlDocGetRootElement(doc)) == NULL)
    {
        xmlFreeDoc(doc);
        goto exit;
    }

    if (xmlStrcmp(curTop->name, (const xmlChar *) topname))
    {
        /*
         * Bad file format "rules" should be the top entry always
         */
        curTop = NULL;
        xmlFreeDoc(doc);
        goto exit;
    }

exit:
    if (buf)
    {
        /*
         * We shouldn't fix such defects
         */
         /* coverity[tainted_data : FALSE]  */
        sal_free(buf);
    }
    return curTop;
}
#endif /* defined(BCM_PETRA_SUPPORT) && defined(BCM_COMPILE_OPENNSA) */
#else /* (BCM_DNX_SUPPORT) || (BCM_PETRA_SUPPORT) || (BCM_DNXF_SUPPORT) */

typedef int make_iso_compilers_happy;

#endif /* (BCM_DNX_SUPPORT) || (BCM_PETRA_SUPPORT) || (BCM_DNXF_SUPPORT) */
