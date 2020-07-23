/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    dbx_xml.c
 * Purpose:    Routines for handling XML data
 */

#include <sal/core/libc.h>
#include <sal/core/alloc.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/types.h>
#include <shared/bsl.h>
#include <shared/util.h>

#include <shared/dbx/dbx_xml.h>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>

#define BSL_LOG_MODULE BSL_LS_SHAREDSWDNX_UTILSDNX

static shr_error_e
xml_to_buf(
    char *filePath,
    char **buf,
    off_t * size_p)
{
    FILE *in = NULL;
    int nread;
    int size;
    char *file_buf;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    /*
     * Open file for reading
     */
    if ((in = sal_fopen(filePath, "r")) == NULL)
    {
        /*
         * failure to open existing file is error
         */
        SHR_SET_CURRENT_ERR(_SHR_E_EXISTS);
        SHR_EXIT();
    }

    sal_fseek(in, 0, SEEK_END);
    size = sal_ftell(in);
    if ((sal_fseek(in, 0, SEEK_SET) != 0) || (size <= 0))
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }

    /*
     * Allocate memory buffer for file contents
     */
    if ((file_buf = sal_alloc(size, "buf")) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_MEMORY);
        SHR_EXIT();
    }
    /*
     * Read file into memory
     */
    nread = sal_fread(file_buf, sizeof(char), size, in);
    if (nread != size)
    {
        /*
         * We shouldn't fix such defects
         */
         /* coverity[tainted_data : FALSE]  */
        sal_free(file_buf);
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }

    *size_p = size;
    *buf = file_buf;

exit:
    if (in != NULL)
    {
        sal_fclose(in);
    }
    SHR_FUNC_EXIT;
}

void *
dbx_xml_top_get(
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

void *
dbx_xml_top_create(
    char *topname)
{
    xmlDocPtr doc;
    xmlNodePtr curTop = NULL;

    if ((doc = xmlNewDoc((const xmlChar *) "1.0")) == NULL)
    {
        goto exit;
    }

    if ((curTop = xmlNewNode(NULL, (const xmlChar *) topname)) == NULL)
    {
        goto exit;
    }

    xmlDocSetRootElement(doc, curTop);

    dbx_xml_node_add_str(curTop, "\n");

exit:
    return curTop;
}

void
dbx_xml_top_close(
    void *top)
{
    xmlNodePtr node = (xmlNodePtr) top;
    if (node && node->doc)
        xmlFreeDoc(node->doc);
    return;
}

void
dbx_xml_top_save(
    void *top,
    char *filepath)
{
    xmlNodePtr node = (xmlNodePtr) top;

    /*
     * save file!
     */
    xmlKeepBlanksDefault(1);

    if (node && node->doc)
        xmlSaveFormatFile(filepath, node->doc, 1);

    return;
}

void *
dbx_xml_child_get_first(
    void *parent,
    char *name)
{
    xmlNodePtr node;

    for (node = ((xmlNodePtr) parent)->xmlChildrenNode; node; node = node->next)
    {

        if (xmlStrcmp(node->name, (const xmlChar *) name))
            continue;
        else
            return node;
    }

    return NULL;
}

void *
dbx_xml_child_get_next(
    void *prev)
{
    xmlNodePtr node;

    if (prev == NULL)
        return NULL;

    for (node = ((xmlNodePtr) prev)->next; node; node = node->next)
    {

        if (xmlStrcmp(node->name, ((xmlNodePtr) prev)->name))
            continue;
        else
            return node;
    }

    return NULL;
}

void *
dbx_xml_child_get_last(
    void *parent,
    char *name)
{
    xmlNodePtr node;

    for (node = ((xmlNodePtr) parent)->last; node; node = node->prev)
    {
        if (xmlStrcmp(node->name, (const xmlChar *) name))
            continue;
        else
            return node;
    }

    return NULL;
}

void *
dbx_xml_child_get_prev(
    void *next)
{
    xmlNodePtr node;

    if (next == NULL)
        return NULL;

    for (node = ((xmlNodePtr) next)->prev; node; node = node->prev)
    {
        if (xmlStrcmp(node->name, ((xmlNodePtr) next)->name))
            continue;
        else
            return node;
    }

    return NULL;
}

void *
dbx_xml_child_get_first_any(
    void *parent)
{
    if (parent == NULL)
        return NULL;

    return ((xmlNodePtr) parent)->xmlChildrenNode;
}

void *
dbx_xml_child_get_next_any(
    void *prev)
{
    if (prev == NULL)
        return NULL;

    return ((xmlNodePtr) prev)->next;
}

char *
dbx_xml_node_get_name(
    void *node)
{
    if (node == NULL)
        return NULL;

    return (char *) (((xmlNodePtr) node)->name);

}

void *
dbx_xml_child_get_content_str(
    void *parent,
    char *nodename,
    char *target,
    int target_size)
{
    xmlNodePtr cur, textnode;

    if (target == NULL)
    {
        cur = NULL;
        goto exit;
    }

    if ((cur = dbx_xml_child_get_first(parent, nodename)) == NULL)
    {
        sal_strncpy(target, "", target_size - 1);
        goto exit;
    }

    /*
     * There should be text node inside that will reveal its content
     */
    if ((textnode = dbx_xml_child_get_first(cur, "text")) == NULL)
    {
        sal_strncpy(target, "", target_size - 1);
        cur = NULL;
        goto exit;
    }

    if (target_size <= sal_strlen((char *) textnode->content))
    {
        cli_out("Target size:%d Content size:%d for %s\n", target_size, (int) sal_strlen((char *) textnode->content),
                textnode->content);
        sal_strncpy(target, "", target_size - 1);
        cur = NULL;
        goto exit;
    }

    sal_snprintf(target, target_size - 1, "%s", textnode->content);

exit:
    return cur;
}

void *
dbx_xml_child_get_content_int(
    void *parent,
    char *nodename,
    int *target)
{
    xmlNodePtr cur, textnode;

    if ((cur = dbx_xml_child_get_first(parent, nodename)) == NULL)
        goto exit;

    /*
     * There should be text node inside that will reveal its content
     */
    if ((textnode = dbx_xml_child_get_first(cur, "text")) == NULL)
        goto exit;

    *target = _shr_ctoi((char *) textnode->content);

exit:
    return cur;
}

void *
dbx_xml_child_set_content_str(
    void *parent,
    char *nodename,
    char *source,
    int depth)
{
    xmlNodePtr cur;

    if ((cur = dbx_xml_child_add(parent, nodename, depth)) == NULL)
        goto exit;
    /*
     * There should be text node inside that will reveal its content
     */
    dbx_xml_node_add_str(cur, source);
exit:
    return cur;
}

void *
dbx_xml_child_set_content_int(
    void *parent,
    char *nodename,
    int source,
    int depth)
{
    xmlNodePtr cur;
    char temp[RHSTRING_MAX_SIZE];
    if ((cur = dbx_xml_child_add(parent, nodename, depth)) == NULL)
        goto exit;
    /*
     * There should be text node inside that will reveal its content
     */
    sal_snprintf(temp, RHSTRING_MAX_SIZE - 1, "%d", source);
    dbx_xml_node_add_str(cur, temp);

exit:
    return cur;
}

void *
dbx_xml_child_add(
    void *parent,
    char *name,
    int indent_num)
{
    xmlNodePtr node;
    int i;

    /*
     * Check if it is the first node in parent, add one more new line
     */
    if (((xmlNodePtr) parent)->xmlChildrenNode == NULL)
    {
        dbx_xml_node_add_str(parent, "\n");
    }

    /*
     * Each node should start with indent
     */
    for (i = 0; i < indent_num; i++)
    {
        if ((node = xmlNewText((const xmlChar *) "    ")) == NULL)
        {
            goto exit;
        }

        if ((node = xmlAddChild(parent, node)) == NULL)
        {
            goto exit;
        }
    }

    node = xmlNewTextChild(parent, NULL, (const xmlChar *) name, NULL);
    if (!node)
        cli_out("xmlNewChild() failed\n");

    dbx_xml_node_add_str(parent, "\n");

exit:
    return node;
}

void *
dbx_xml_comment_add(
    void *parent,
    char *str)
{
    xmlNodePtr node;

    node = xmlNewComment((const xmlChar *) str);
    if (!node)
    {
        goto exit;
    }

    node = xmlAddChild(parent, node);
    if (!node)
    {
        goto exit;
    }

    dbx_xml_node_add_str(parent, "\n");

exit:
    return node;
}

void *
dbx_xml_child_add_prev(
    void *parent,
    void *sibling,
    char *name,
    int indent_num)
{
    xmlNodePtr node, tmp_node;
    int i;

    /*
     * Each node should start with indent
     */
    node = xmlNewTextChild(parent, NULL, (const xmlChar *) name, NULL);
    if (!node)
    {
        goto exit;
    }

    if (!(node = xmlAddPrevSibling(sibling, node)))
    {
        goto exit;
    }

    for (i = 0; i < indent_num; i++)
    {
        if (!(tmp_node = xmlNewText((const xmlChar *) "    ")))
        {
            goto exit;
        }

        if (!(tmp_node = xmlAddNextSibling(node, tmp_node)))
        {
            goto exit;
        }
    }

    tmp_node = xmlNewText((const xmlChar *) "\n");
    if (!tmp_node)
    {
        goto exit;
    }

    tmp_node = xmlAddNextSibling(node, tmp_node);
    if (!tmp_node)
    {
        goto exit;
    }

exit:
    return node;
}

int
dbx_xml_node_get_content_str(
    void *cur,
    char *target,
    int target_size)
{
    xmlNodePtr textnode;
    int real_size = 0;

    /*
     * There should be text node inside that will reveal its content
     */
    if ((textnode = dbx_xml_child_get_first(cur, "text")) == NULL)
    {
        sal_strncpy(target, "", target_size - 1);
        goto exit;
    }

    if (target_size <= sal_strlen((char *) textnode->content))
    {
        cli_out("Target size:%d Content size:%d for %s\n", target_size, (int) sal_strlen((char *) textnode->content),
                textnode->content);
        sal_strncpy(target, "", target_size - 1);
        goto exit;
    }

    real_size = sal_strlen((char *) textnode->content);
    sal_snprintf(target, target_size, "%s", textnode->content);

exit:
    return real_size;
}

void *
dbx_xml_node_add_str(
    void *parent,
    char *str)
{
    xmlNodePtr node;

    node = xmlNewText((const xmlChar *) str);
    if (!node)
    {
        goto exit;
    }

    node = xmlAddChild(parent, node);
    if (!node)
    {
        goto exit;
    }

exit:
    return node;
}

shr_error_e
dbx_xml_node_end(
    void *parent,
    int depth)
{
    int i;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    for (i = 0; i < depth; i++)
    {
        if (dbx_xml_node_add_str(parent, "    ") == NULL)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbx_xml_node_end_space(
    void *parent,
    int depth)
{
    int i;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    for (i = 0; i < depth; i++)
    {
        if (dbx_xml_node_add_str(parent, "    ") == NULL)
        {
            SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
            SHR_EXIT();
        }
    }

exit:
    SHR_FUNC_EXIT;
}

void
dbx_xml_node_delete(
    void *node)
{
    xmlNodePtr nodePrev = ((xmlNodePtr) node)->prev;

    /*
     * We have found the entry to be deleted
     */
    xmlUnlinkNode(node);
    xmlFreeNode(node);
    /*
     * We need to delete the previous empty node as well to avoid empty lines
     */
    if (nodePrev)
    {
        xmlUnlinkNode(nodePrev);
        xmlFreeNode(nodePrev);
    }
}

shr_error_e
dbx_xml_property_set_str(
    void *node,
    char *property,
    char *source)
{
    xmlAttrPtr attr;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    attr = xmlNewProp((xmlNodePtr) node, (const xmlChar *) property, (const xmlChar *) source);
    if (!attr)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbx_xml_property_set_int(
    void *node,
    char *property,
    int source)
{
    char tmp[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(NO_UNIT);

    sal_snprintf(tmp, RHSTRING_MAX_SIZE - 1, "%d", source);
    if (xmlNewProp((xmlNodePtr) node, (const xmlChar *) property, (const xmlChar *) tmp) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbx_xml_property_set_hex(
    void *node,
    char *property,
    int source)
{
    char tmp[RHSTRING_MAX_SIZE];

    SHR_FUNC_INIT_VARS(NO_UNIT);

    sal_snprintf(tmp, RHSTRING_MAX_SIZE - 1, "0x%x", source);
    if (xmlNewProp((xmlNodePtr) node, (const xmlChar *) property, (const xmlChar *) tmp) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbx_xml_property_mod_str(
    void *node,
    char *property,
    char *source)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);

    if (xmlSetProp((xmlNodePtr) node, (const xmlChar *) property, (const xmlChar *) source) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbx_xml_property_mod_int(
    void *node,
    char *property,
    int source)
{
    char tmp[1024];
    xmlAttrPtr attr;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    sal_snprintf(tmp, sizeof(tmp), "%d", source);
    attr = xmlHasProp((xmlNodePtr) node, (const xmlChar *) property);
    if (attr == NULL)
    {
        attr = xmlNewProp((xmlNodePtr) node, (const xmlChar *) property, (const xmlChar *) tmp);
    }
    else
    {
        attr = xmlSetProp((xmlNodePtr) node, (const xmlChar *) property, (const xmlChar *) tmp);
    }
    if (attr == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        SHR_EXIT();
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dbx_xml_property_delete(
    void *node,
    char *property)
{
    xmlAttrPtr attr;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    attr = xmlHasProp((xmlNodePtr) node, (const xmlChar *) property);
    if (attr != NULL)
    {
        xmlRemoveProp(attr);
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbx_xml_property_get_all(
    void *node,
    attribute_param_t * attribute_param,
    int max_num)
{
    xmlNodePtr int_node = (xmlNodePtr) node;
    xmlAttrPtr prop;
    int i = 0;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    /*
     * Check on the properties attached to the node
     */
    prop = int_node->properties;

    while ((prop != NULL) && (i < max_num))
    {
        RHDATA_GET_STR_CONT(node, (char *) prop->name, attribute_param->value);
        sal_strncpy(attribute_param->name, (char *) prop->name, RHNAME_MAX_SIZE - 1);

        attribute_param++;
        i++;
        prop = prop->next;
    }

    SHR_FUNC_EXIT;
}

shr_error_e
dbx_xml_property_get_str(
    void *node,
    char *property,
    char *target,
    uint32 target_size)
{
    char *prop;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    if ((prop = (char *) xmlGetProp((xmlNodePtr) node, (const xmlChar *) property)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    if (target_size <= sal_strlen(prop))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Target size:%d Property size:%d for %s\n", target_size, (int) sal_strlen(prop),
                     property);
    }

    if (target != NULL)
    {
        sal_snprintf(target, target_size - 1, "%s", prop);
    }

exit:
    if (prop)
    {
        xmlFree(prop);
    }
    SHR_FUNC_EXIT;
}

shr_error_e
dbx_xml_property_get_int(
    void *node,
    char *property,
    int *target)
{
    char *prop;

    SHR_FUNC_INIT_VARS(NO_UNIT);

    if ((prop = (char *) xmlGetProp((xmlNodePtr) node, (const xmlChar *) property)) == NULL)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_NOT_FOUND);
        SHR_EXIT();
    }

    *target = _shr_ctoi(prop);
    xmlFree(prop);

exit:
    SHR_FUNC_EXIT;
}

#if 0
shr_error_e
dbx_xml_property_get_num(
    void *node,
    char *property,
    void *target,
    int target_size)
{
    int res = _SHR_E_NONE;
    char *prop;

    prop = (char *) xmlGetProp(node, (const xmlChar *) property);
    if (!prop)
    {
        res = _SHR_E_INTERNAL;
        goto exit;
    }

    if (((prop[0] == '0') && (prop[1] == 'x')) || ((prop[0] == '0') && (prop[1] == 'X')))
    {
        switch (target_size)
        {
            case 1:
                sscanf(prop, "0x%hhx", (uint8 *) target);
                break;
            case 2:
                sscanf(prop, "0x%hx", (uint16 *) target);
                break;
            case 4:
                sscanf(prop, "0x%x", (uint32 *) target);
                break;
            case 8:
                sscanf(prop, "0x%llx", (unsigned long long *) target);
                break;
            default:
                cli_out("Bad target size for %d for:\"%s\"", target_size, property);
                res = _SHR_E_PARAM;
                break;
        }
    }
    else
    {
        switch (target_size)
        {
            case 1:
                *(char *) target = (char) _shr_ctoi(prop);
                break;
            case 2:
                *(short *) target = (short) _shr_ctoi(prop);
                break;
            case 4:
                *(int *) target = _shr_ctoi(prop);
                break;
            case 8:
                sscanf(prop, "%lld", (long long *) target);
                break;
            default:
                cli_out("Bad target size for %d for:\"%s\"", target_size, property);
                res = _SHR_E_PARAM;
                break;
        }
    }

    xmlFree(prop);

exit:
    return res;
}
#endif
