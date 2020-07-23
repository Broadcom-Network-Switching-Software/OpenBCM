/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    dbx_xml.h
 * Purpose:    Types and structures used when working with data IO
 */

#ifndef DBX_XML_H_INCLUDED
#define DBX_XML_H_INCLUDED

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_str.h>

typedef void *xml_node;

/**
 * Flags used when opening XML files
 */
/**
 * Use to create the XML file when it does not exist
 */
#define  CONF_OPEN_CREATE               0x0001
/**
 * Use to overwrite XML file when it exists
 */
#define  CONF_OPEN_OVERWRITE            0x0002
/**
 * Use to overwrite XML file when it exists
 */
#define  CONF_OPEN_NO_ERROR_REPORT      0x0004
/**
 * Find DB location as per DNX-Devices.xml, do not use for global DB files
 */
#define  CONF_OPEN_PER_DEVICE           0x0008
/**
 * Look into alternative specific file location first
 */
#define  CONF_OPEN_ALTER_LOC            0x0010
/**
 * Do not look for DB - open/create file in current location
 */
#define  CONF_OPEN_CURRENT_LOC          0x0020

/**
 * \brief get handle for top node inside XML file
 * \par DIRECT INPUT
 *   \param [in] filepath - full path, either relative or absolute for XML file
 *   \param [in] topname - name of top XML node
 *   \param [in] flags - options for file opening, see above CONF_OPEN_*
 * \par DIRECT OUTPUT:
 *   \retval xml_node if XML file exists and has top node with this name
 *   \retval NULL for any failure
 */
xml_node dbx_xml_top_get(
    char *filepath,
    char *topname,
    int flags);

/**
 * \brief create XML document
 * \par DIRECT INPUT
 *   \param [in] topname - name of top XML node for this document
 * \par DIRECT OUTPUT:
 *   \retval xml_node if XML document was successfully created
 *   \retval NULL for any failure
 */
xml_node dbx_xml_top_create(
    char *topname);

/**
 * \brief close XML document
 * \par DIRECT INPUT
 *   \param [in] top - handle to top XML node of this document
 * \remark
 *   Nothing to do with failure - so no need for failure sign, LOG will be from inside
 */
void dbx_xml_top_close(
    xml_node top);

/**
 * \brief Save XML document to the file
 * \par DIRECT INPUT
 *   \param [in] filepath - full path, either relative or absolute for XML file
 *   \param [in] top - top node handle of XML document
 * \remark
 *   Nothing to do with failure - so no need for failure sign, LOG will be from inside
 */
void dbx_xml_top_save(
    xml_node top,
    char *filepath);

/**
 * \brief Get handle for the first child with specified name under parent node
 * \par DIRECT INPUT
 *   \param [in] parent - xml handle for parent node
 *   \param [in] childname - name of child to look for under the parent
 * \par DIRECT OUTPUT:
 *   \retval xml_node if child with this name exists
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_get_first(
    xml_node parent,
    char *childname);

/**
 * \brief Get handle for the next child with the same name
 * \par DIRECT INPUT
 *   \param [in] sibling - xml handle for previous sibling with the same name
 * \par DIRECT OUTPUT:
 *   \retval xml_node if next sibling with this name exists
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_get_next(
    xml_node sibling);

/**
 * \brief Get handle for the last child with specified name under parent node
 * \par DIRECT INPUT
 *   \param [in] parent - xml handle for parent node
 *   \param [in] childname - name of child to look for under the parent
 * \par DIRECT OUTPUT:
 *   \retval xml_node if child with this name exists
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_get_last(
    xml_node parent,
    char *childname);

/**
 * \brief Get handle for the prev child with the same name
 * \par DIRECT INPUT
 *   \param [in] sibling - xml handle for next sibling with the same name
 * \par DIRECT OUTPUT:
 *   \retval xml_node if prev sibling with this name exists
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_get_prev(
    xml_node sibling);

/**
 * \brief Get name of the node
 * \par DIRECT INPUT
 *   \param [in] node  - xml handle for node, which name we request
 * \par DIRECT OUTPUT:
 *   \retval pointer to node name if node is not NULL
 *   \retval NULL for any failure
 */
char *dbx_xml_node_get_name(
    void *node);

/**
 * \brief Get handle for the first child under parent node
 * \par DIRECT INPUT
 *   \param [in] parent - xml handle for parent node
 * \par DIRECT OUTPUT:
 *   \retval xml_node if parent is not NULL
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_get_first_any(
    xml_node parent);

/**
 * \brief Get handle for the next child
 * \par DIRECT INPUT
 *   \param [in] sibling - xml handle for previous sibling
 * \par DIRECT OUTPUT:
 *   \retval xml_node if prev node is not NULL
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_get_next_any(
    xml_node sibling);

/**
 * \brief Get content string of specified child
 * \par DIRECT INPUT
 *   \param [in] parent - xml handle for parent node
 *   \param [in] childname - name of child to look for under the parent
 *   \param [in] target - pointer to allocated space where content should be copied
 *   \param [in] size - allocation size
 * \par DIRECT OUTPUT:
 *   \retval xml_node - handle for child node if found
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_get_content_str(
    xml_node parent,
    char *childname,
    char *target,
    int size);

/**
 * \brief Assuming content of the child node is "int" get the value
 * \par DIRECT INPUT
 *   \param [in] parent - xml handle for parent node
 *   \param [in] childname - name of child to look for under the parent
 *   \param [in] target - pointer to int where number should be assigned
 * \par DIRECT OUTPUT:
 *   \retval xml_node - handle for child node if found and it is "int"
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_get_content_int(
    xml_node parent,
    char *childname,
    int *target);

/**
 * \brief Create XML node and assign its string content
 * \par DIRECT INPUT
 *   \param [in] parent - xml handle for parent node
 *   \param [in] childname - name of child to be created
 *   \param [in] source - pointer to the content to be assigned to the newly created node
 *   \param [in] depth - indentation offset inside XML file
 * \par DIRECT OUTPUT:
 *   \retval xml_node - handle for newly created child node
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_set_content_str(
    xml_node parent,
    char *childname,
    char *source,
    int depth);

/**
 * \brief Create XML node and assign its "int" content
 * \par DIRECT INPUT
 *   \param [in] parent - xml handle for parent node
 *   \param [in] childname - name of child to be created
 *   \param [in] source - content value to be assigned to the newly created node
 *   \param [in] depth - indentation offset inside XML file
 * \par DIRECT OUTPUT:
 *   \retval xml_node - handle for newly created child node
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_set_content_int(
    xml_node parent,
    char *childname,
    int source,
    int depth);

/**
 * \brief Create XML node under parent one
 * \par DIRECT INPUT
 *   \param [in] parent - xml handle for parent node
 *   \param [in] childname - name of child to be created
 *   \param [in] depth - indentation offset inside XML file
 * \par DIRECT OUTPUT:
 *   \retval xml_node - handle for newly created child node
 *   \retval NULL for any failure
 */
xml_node dbx_xml_child_add(
    xml_node parent,
    char *childname,
    int depth);

/**
 * \brief Create XML node under parent before specified sibling
 * \par DIRECT INPUT
 *   \param [in] parent - xml handle for parent node
 *   \param [in] sibling - xml handle for sibling
 *   \param [in] childname - name of child to be created
 *   \param [in] depth - indentation offset inside XML file
 * \par DIRECT OUTPUT:
 *   \retval xml_node - handle for newly created child node
 *   \retval NULL for any failure
 * \remark
 *  Serves for ordered lists
 */
xml_node dbx_xml_child_add_prev(
    xml_node parent,
    xml_node sibling,
    char *childname,
    int depth);

/**
 * \brief Create comment inside XML document
 * \par DIRECT INPUT
 *   \param [in] top - xml handle for top node
 *   \param [in] comment - comment string
 * \par DIRECT OUTPUT:
 *   \retval xml_node - handle for newly created comment node
 *   \retval NULL for any failure
 */
void *dbx_xml_comment_add(
    void *top,
    char *comment);

/**
 * \brief Get content of specified node
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] target - pointer to the place where content should be copied
 *   \param [in] size - size of allocated place
 * \par DIRECT OUTPUT:
 *   \retval size - actual content size
 *   \retval 0 for any failure
 */
int dbx_xml_node_get_content_str(
    xml_node node,
    char *target,
    int size);

/**
 * \brief Add child with Get content of specified node
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for parent node under which child should be added
 *   \param [in] str - name for child node
 * \par DIRECT OUTPUT:
 *   \retval xml_node - child node
 *   \retval NULL - for any failure
 */
xml_node dbx_xml_node_add_str(
    xml_node node,
    char *str);

/**
 * \brief End node usually means make right indentation in XML file
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node to be ended
 *   \param [in] depth - indentation offset inside XML file
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_node_end(
    xml_node node,
    int depth);

/**
 * \brief End node make right indentation in XML file using 4 spaces for 1 pffset count
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node to be ended
 *   \param [in] depth - indentation offset inside XML file
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_node_end_space(
    xml_node node,
    int depth);

/**
 * \brief Delete node from XML document/file
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node to be deleted
 */
void dbx_xml_node_delete(
    xml_node node);

/**
 * \brief Set property of specified node to specified string value
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] property - property tpo be set in the node
 *   \param [in] source - pointer to string value of property to be set
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_property_set_str(
    xml_node node,
    char *property,
    char *source);

/**
 * \brief Set property of specified node to specified int value
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] property - property tpo be set in the node
 *   \param [in] source - integer value of property to be set
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_property_set_int(
    xml_node node,
    char *property,
    int source);

/**
 * \brief Set property of specified node to specified int value in hex format
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] property - property tpo be set in the node
 *   \param [in] source - integer value of property to be set
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_property_set_hex(
    xml_node node,
    char *property,
    int source);

/**
 * \brief Modify property of specified node to specified string value
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] property - property tpo be set in the node
 *   \param [in] source - pointer to string value of property to be modified
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_property_mod_str(
    xml_node node,
    char *property,
    char *source);

/**
 * \brief Modify property of specified node to specified int value
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] property - property tpo be set in the node
 *   \param [in] source - integer value of property to be set
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_property_mod_int(
    xml_node node,
    char *property,
    int source);

/**
 * \brief Delete specified property from the node
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] property - property tpo be set in the node
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_property_delete(
    xml_node node,
    char *property);

/**
 * \brief Obtain all properties of specified node to preallocated structure
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] attribute_param - pointer to array of structures allocated for properties to be obtained
 *   \param [in] max_num - maximum number of properties to be read from the node
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 * \remark
 *   Reaching maximum number of properties does not create error, just limit the list
 *   Array is initialized to NULL, so on return the only non-NULL entries will be related to
 */
shr_error_e dbx_xml_property_get_all(
    xml_node node,
    attribute_param_t * attribute_param,
    int max_num);
/**
 * \brief Get string property of specified node to specified string value
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] property - property tpo be set in the node
 *   \param [in] target - pointer to string value of property to be modified
 *   \param [in] target_size - size for target memeory allocation
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_property_get_str(
    xml_node node,
    char *property,
    char *target,
    uint32 target_size);
/**
 * \brief Get int property of specified node to specified string value
 * \par DIRECT INPUT
 *   \param [in] node - xml handle for node
 *   \param [in] property - property tpo be set in the node
 *   \param [in] target - pointer to integer where property integer value will be stored
 * \par DIRECT OUTPUT:
 *   \retval _SHR_E_NONE - success
 *   \retval _SHR_E_INTERNAL - only failure
 */
shr_error_e dbx_xml_property_get_int(
    xml_node node,
    char *property,
    int *target);

/**
 * \brief Iterator that is going through all child nodes with certain name inside designated parent from the start
 * \param [out] node_mac - xml_node - handle for node found
 * \param [in] parent_mac - xml_nide - handle for parent node
 * \param [in] name_mac - char* - name that child should have in order to included in iterator
 * \remark
 *   Iterator is not safe one - you cannot modify the list of childs inside parent node when iterating
 * \see
 *   RHDATA_SAFE_ITERATOR
 */
#define RHDATA_ITERATOR(node_mac, parent_mac, name_mac)                        \
    for(node_mac = dbx_xml_child_get_first(parent_mac, name_mac); node_mac; node_mac = dbx_xml_child_get_next(node_mac))

/**
 * \brief Iterator that is going through all child nodes with certain name inside designated parent from the end
 * \param [out] node_mac - xml_node - handle for node found
 * \param [in] parent_mac - xml_nide - handle for parent node
 * \param [in] name_mac - char* - name that child should have in order to included in iterator
 * \remark
 *   Iterator is not safe one - you cannot modify the list of childs inside parent node when iterating
 * \see
 *   RHDATA_SAFE_ITERATOR
 */
#define RHDATA_BACK_ITERATOR(node_mac, parent_mac, name_mac)                        \
    for(node_mac = dbx_xml_child_get_last(parent_mac, name_mac); node_mac; node_mac = dbx_xml_child_get_prev(node_mac))

/**
 * \brief Iterator that is going through all child nodes under the specified parent
 * \param [out] node_mac - xml_node - handle for node found
 * \param [in] parent_mac - xml_nide - handle for parent node
 * \remark
 *   Iterator is not safe one - you cannot modify the list of childs inside parent node when iterating
 * \see
 *   RHDATA_SAFE_ITERATOR
 */
#define RHDATA_ITERATOR_ANY(node_mac, parent_mac)                           \
    for(node_mac = dbx_xml_child_get_first_any(parent_mac); node_mac; node_mac = dbx_xml_child_get_next_any(node_mac))

/**
 * \brief Iterator that is going through all child nodes with certain name inside designated parent
 * \param [out] node_mac - xml_node - handle for node found
 * \param [in] parent_mac - xml_node - handle for parent node
 * \param [in] name_mac - char* - name that child should have in order to included in iterator
 * \remark
 *   Iterator is slower but safe one, you may remove found node
 * \see
 *   RHDATA_ITERATOR
 */
#define RHDATA_SAFE_ITERATOR(node_mac, parent_mac, name_mac)                   \
  xml_node next_node_mac;\
  for(node_mac = dbx_xml_child_get_first(parent_mac, name_mac), next_node_mac = dbx_xml_child_get_next(node_mac); node_mac; node_mac = next_node_mac, next_node_mac = xml_get_next(node_mac))

/**
 * \brief Obtain number of child nodes with certain name inside parent node
 * \param [out] num_mac - number of nodes with specific name under specific parent node
 * \param [in] parent_mac - xml handle for parent node
 * \param [in] name_mac - name that child should have in order to be included in count
 */
#define RHDATA_GET_NODE_NUM(num_mac, parent_mac, name_mac) {      \
            xml_node node_mac;                                    \
            num_mac = 0;                                          \
            RHDATA_ITERATOR(node_mac, parent_mac, name_mac)       \
                num_mac++;                                        \
        }

/**
 * \brief Get content string of specified child
 * \param [in] parent_mac - xml_node - handle for parent node
 * \param [in] childname_mac - char* - name of child to look for under the parent
 * \param [in] target_mac - char* - pointer to allocated space where content should be copied,
 *                          allocated size should cover expected target size, larger size is treated as an error
 * \param [in] def_str_mac - char* - default string to  value to be assigned to target in the case of failure
 * \return
 *   When content fetching fails, target_mac is filled by def_str_mac, no indication will be produced
 * \remark
 *   Used when content existence itself is no important, so that target may be set to default
 * \see
 *   RHCONTENT_GET_STR_STOP
 */
#define RHCONTENT_GET_STR_DEF(parent_mac, childname_mac, target_mac, def_str_mac)                         \
    if((dbx_xml_child_get_content_str(parent_mac, childname_mac, target_mac, RHNAME_MAX_SIZE)) == NULL) { \
        strcpy(target_mac, def_str_mac);                                                                  \
    }

/**
 * \brief Get content string of specified child
 * \param [in] parent_mac - xml_node - handle for parent node
 * \param [in] childname_mac - char* - name of child to look for under the parent
 * \param [in] target_mac - char* - pointer to allocated space where content should be copied,
 *                          allocated size should cover expected target size, larger size is treated as an error
 * \param [in] def_str_mac - char* - default string to  value to be assigned to target in the case of failure
 * \param [in] target_max_size - max size in chars of target string
 * \return
 *   When content fetching fails, target_mac is filled by def_str_mac, no indication will be produced
 * \remark
 *   Used when content existence itself is no important, so that target may be set to default
 * \see
 *   RHCONTENT_GET_STR_STOP
 */
#define RHCONTENT_GET_XSTR_DEF(parent_mac, childname_mac, target_mac, def_str_mac, target_max_size)                         \
    if((dbx_xml_child_get_content_str(parent_mac, childname_mac, target_mac, target_max_size)) == NULL) { \
        strcpy(target_mac, def_str_mac);                                                                  \
    }

/**
 * \brief Assuming content of the child node is "int" get the value
 * \param [in] parent_mac - xml_node - handle for parent node
 * \param [in] childname_mac - char* - name of child to look for under the parent
 * \param [in] target_mac - char* - pointer to int where number should be assigned
 * \param [in] def_value_mac - char* - default int value to be assigned to target in teh case of failure
 * \return
 *   When content fetching fails, target_mac is assigned by def_value_mac, no indication will be produced
 * \remark
 *   Used when content existence itself is no important, so that target may be set to default
 * \see
 *   RHCONTENT_GET_INT_STOP
 */
#define RHCONTENT_GET_INT_DEF(parent_mac, childname_mac, target_mac, def_value_mac)         \
    if(dbx_xml_child_get_content_int(parent_mac, childname_mac, target_mac) == NULL){       \
        *target_mac = def_value_mac;                                                        \
    }

/**
 * \brief Get content string of specified child
 * \param [in] parent_mac - xml_node - handle for parent node
 * \param [in] childname_mac - char* - name of child to look for under the parent
 * \param [in] target_mac - char* - pointer to allocated space where content should be copied,
 *                          allocated size should cover expected target size, larger size is treated as an error
 * \return
 *   Goes to exit, if content fetching fails
 * \remark
 *   Used when content itself is important, so that failure will result in exit
 * \see
 *   RHCONTENT_GET_STR_DEF
 */
#define RHCONTENT_GET_STR_STOP(parent_mac, childname_mac, target_mac)                                     \
    if((dbx_xml_child_get_content_str(parent_mac, childname_mac, target_mac, RHNAME_MAX_SIZE)) == NULL) { \
       goto exit;                                                                                         \
    }

/**
 * \brief Get content string of specified child
 * \param [in] parent_mac - xml_node - handle for parent node
 * \param [in] childname_mac - char* - name of child to look for under the parent
 * \param [in] target_mac - char* - pointer to allocated space where content should be copied,
 *                          allocated size should cover expected target size, larger size is treated as an error
 * \param [in] target_max_size - max size in chars of target string
 * \return
 *   Goes to exit, if content fetching fails
 * \remark
 *   Used when content itself is important, so that failure will result in exit
 * \see
 *   RHCONTENT_GET_STR_DEF
 */
#define RHCONTENT_GET_XSTR_STOP(parent_mac, childname_mac, target_mac, target_max_size)                                     \
    if((dbx_xml_child_get_content_str(parent_mac, childname_mac, target_mac, target_max_size)) == NULL) { \
       goto exit;                                                                                         \
    }

/**
 * \brief Assuming content of the child node is "int" get the value
 * \param [in] parent_mac - xml_node - handle for parent node
 * \param [in] childname_mac - char* - name of child to look for under the parent
 * \param [in] target_mac - char* - pointer to int where number should be assigned
 * \return
 *   Goes to exit, if content fetching fails
 * \remark
 *   Used when content itself is important, so that failure will result in exit
 * \see
 *   RHCONTENT_GET_INT_DEF
 */
#define RHCONTENT_GET_INT_STOP(parent_mac, childname_mac, target_mac)                       \
    if((dbx_xml_child_get_content_int(parent_mac, childname_mac, target_mac)) == NULL) {    \
        goto exit;                                                                          \
    }

/**
 * \brief Get property string of specified child
 * \param [in] node_mac - xml handle for parent node
 * \param [in] childname_mac - name of child to look for under the parent
 * \param [in] string_mac - property name of child to look for
 * \param [in] target_mac - pointer to allocated space where content should be copied
 * \param [in] target_max_size - max size chars of target string
 * \return
 *   Goes to exit, if content fetching fails or child node does not exist
 * \remark
 *   Used when property and child existence are crucial, so that failure will result in exit
 * \see
 *   RHCHDATA_GET_STR_DEF
 */
#define RHCHDATA_GET_STR_STOP(node_mac, childname_mac ,string_mac, target_mac, target_max_size)                        \
{                                                                                                                      \
    xml_node child_node_mac;                                                                                           \
    child_node_mac = dbx_xml_child_get_first(node_mac,childname_mac);                                                  \
    if(child_node_mac != NULL)                                                                                         \
    {                                                                                                                  \
        SHR_IF_ERR_EXIT(dbx_xml_property_get_str(child_node_mac, string_mac, target_mac, target_max_size));            \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
      goto exit;                                                                                                       \
    }                                                                                                                  \
}

/**
 * \brief Get int value of property of specified child
 * \param [in] node_mac - xml handle for parent node
 * \param [in] childname_mac - name of child to look for under the parent
 * \param [in] string_mac - property name of child to look for
 * \param [in] target_mac - pointer to int variable, that should be assigned
 * \return
 *   Goes to exit, if content fetching fails or child node does not exist
 * \remark
 *   Used when property and child existence are crucial, so that failure will result in exit
 * \see
 *   RHCHDATA_GET_INT_DEF
 */
#define RHCHDATA_GET_INT_STOP(node_mac, childname_mac ,string_mac, target_mac)                                         \
{                                                                                                                      \
    xml_node child_node_mac;                                                                                           \
    child_node_mac = dbx_xml_child_get_first(node_mac,childname_mac);                                                  \
    if(child_node_mac != NULL)                                                                                         \
    {                                                                                                                  \
        SHR_IF_ERR_EXIT(dbx_xml_property_get_int(child_node_mac, string_mac, target_mac));                             \
    }                                                                                                                  \
    else                                                                                                               \
    {                                                                                                                  \
        goto exit;                                                                                                     \
    }                                                                                                                  \
}

/**
 * \brief Get property string of specified child
 * \param [in] node_mac - xml handle for parent node
 * \param [in] childname_mac - name of child to look for under the parent
 * \param [in] string_mac - property name of child to look for
 * \param [in] target_mac - pointer to allocated space where content should be copied
 * \param [in] default_str - pointer to default string to be copied to the target in case of failure
 * \param [in] target_max_size - size in chars of target string
 * \return
 *   No sign of failure, once it happens, default string is used
 * \remark
 *   Used when property and child existence are not crucial, so that we may use default string instead
 * \see
 *   RHCHDATA_GET_STR_STOP
 */
#define RHCHDATA_GET_STR_DEF(node_mac, childname_mac ,string_mac, target_mac, default_str, target_max_size)            \
{                                                                                                                      \
    xml_node child_node_mac = dbx_xml_child_get_first(node_mac,childname_mac);                                         \
    if((child_node_mac == NULL) ||                                                                                     \
                 (dbx_xml_property_get_str(child_node_mac, string_mac, target_mac, target_max_size) != _SHR_E_NONE))   \
    {                                                                                                                  \
        strcpy(target_mac, default_str);                                                                               \
    }                                                                                                                  \
}

/**
 * \brief Get int value of property of specified child
 * \param [in] node_mac - xml handle for parent node
 * \param [in] childname_mac - name of child to look for under the parent
 * \param [in] string_mac - property name of child to look for
 * \param [in] target_mac - pointer to int variable, that should be assigned
 * \param [in] def_value_mac - default value to be assign to target in case of failure
 * \return
 *   No change in return on failure, default value is used
 * \remark
 *   Used when property and child existence are crucial, so that failure will result in exit
 * \see
 *   RHCHDATA_GET_INT_STOP
 */
#define RHCHDATA_GET_INT_DEF(node_mac, childname_mac ,string_mac, target_mac, def_value_mac)                           \
{                                                                                                                      \
    xml_node child_node_mac = dbx_xml_child_get_first(node_mac,childname_mac);                                         \
    if((child_node_mac == NULL) || (dbx_xml_property_get_int(child_node_mac, string_mac, &target_mac) != _SHR_E_NONE)) \
    {                                                                                                                  \
        target_mac = def_value_mac;                                                                                    \
    }                                                                                                                  \
}

#define RHDATA_GET_STR_CONT(node_mac, string_mac, target_mac)                                                          \
    if((dbx_xml_property_get_str(node_mac, string_mac, target_mac, RHNAME_MAX_SIZE)) != _SHR_E_NONE) {                 \
        continue;                                                                                                      \
    }

#define RHDATA_GET_STR_CONT_EXT(node_mac, string_mac, target_mac, target_size)                                         \
    if((dbx_xml_property_get_str(node_mac, string_mac, target_mac, target_size)) != _SHR_E_NONE) {                     \
        continue;                                                                                                      \
    }

#define RHDATA_GET_LSTR_CONT(node_mac, string_mac, target_mac)                                                         \
    if((dbx_xml_property_get_str(node_mac, string_mac, target_mac, RHSTRING_MAX_SIZE)) != _SHR_E_NONE) {               \
        continue;                                                                                                      \
    }

#define RHDATA_GET_STR_STOP(node_mac, string_mac, target_mac)                                                          \
        SHR_IF_ERR_EXIT(dbx_xml_property_get_str(node_mac, string_mac, target_mac, RHSTRING_MAX_SIZE))

#define RHDATA_GET_XSTR_STOP(node_mac, string_mac, target_mac, max_target_size)                                                          \
        SHR_IF_ERR_EXIT(dbx_xml_property_get_str(node_mac, string_mac, target_mac, max_target_size))

#define RHDATA_GET_LSTR_STOP(node_mac, string_mac, target_mac)                                                         \
        SHR_IF_ERR_EXIT(dbx_xml_property_get_str(node_mac, string_mac, target_mac, RHSTRING_MAX_SIZE))

#define RHDATA_GET_STR_DEF_NULL(node_mac, string_mac, target_mac)                                                      \
    if((dbx_xml_property_get_str(node_mac, string_mac, target_mac, RHNAME_MAX_SIZE)) != _SHR_E_NONE) {                 \
        target_mac[0] = 0;                                                                                             \
    }

#define RHDATA_GET_XSTR_DEF(node_mac, string_mac, target_mac, default_str, max_target_size)                                              \
    if((dbx_xml_property_get_str(node_mac, string_mac, target_mac, max_target_size)) != _SHR_E_NONE) {                 \
        strcpy(target_mac, default_str);                                                                               \
    }

#define RHDATA_GET_STR_DEF(node_mac, string_mac, target_mac, default_str)                                              \
    if((dbx_xml_property_get_str(node_mac, string_mac, target_mac, RHNAME_MAX_SIZE)) != _SHR_E_NONE) {                 \
        strcpy(target_mac, default_str);                                                                               \
    }

#define RHDATA_GET_LSTR_DEF(node_mac, string_mac, target_mac, default_str)                                             \
    if((dbx_xml_property_get_str(node_mac, string_mac, target_mac, RHSTRING_MAX_SIZE)) != _SHR_E_NONE) {               \
        strcpy(target_mac, default_str);                                                                               \
    }
#define RHDATA_GET_INT_BREAK(node_mac, string_mac, target_mac)                                                          \
    if((dbx_xml_property_get_int(node_mac, string_mac, (int *)(&(target_mac)))) != _SHR_E_NONE) {                      \
        break;                                                                                                      \
    }

#define RHDATA_GET_INT_CONT(node_mac, string_mac, target_mac)                                                          \
    if((dbx_xml_property_get_int(node_mac, string_mac, (int *)(&(target_mac)))) != _SHR_E_NONE) {                      \
        continue;                                                                                                      \
    }

#define RHDATA_GET_INT_STOP(node_mac, string_mac, target_mac)                                                          \
    SHR_IF_ERR_EXIT(dbx_xml_property_get_int(node_mac, string_mac, &(target_mac)))

#define RHDATA_GET_INT_DEF(node_mac, string_mac, target_mac, def_value_mac)                                            \
    if((dbx_xml_property_get_int(node_mac, string_mac, (int *)(&(target_mac)))) != _SHR_E_NONE) {                      \
        target_mac = def_value_mac;                                                                                    \
    }

#define RHDATA_GET_NUM(node_mac, string_mac, target_mac)                                                               \
    if((dbx_xml_property_get_num(node_mac, string_mac, (void *)&(target_mac), sizeof(target_mac))) != _SHR_E_NONE) {   \
        continue;                                                                                                      \
    }

#define RHDATA_GET_NUM_CONT(node_mac, string_mac, target_mac)                                                          \
    if((dbx_xml_property_get_num(node_mac, string_mac, (void *)&(target_mac), sizeof(target_mac))) != _SHR_E_NONE) {   \
        continue;                                                                                                      \
    }

#define RHDATA_GET_NUM_STOP(node_mac, string_mac, target_mac)                                                          \
    SHR_IF_ERR_EXIT(dbx_xml_property_get_num(node_mac, string_mac, (void *)&(target_mac), sizeof(target_mac)))

#define RHDATA_GET_NUM_DEF(node_mac, string_mac, target_mac, def_value_mac)                                            \
    if((dbx_xml_property_get_num(node_mac, string_mac, (void *)&(target_mac), sizeof(target_mac))) != _SHR_E_NONE) {   \
        target_mac = def_value_mac;                                                                                    \
    }

#define RHDATA_SET_INT(node_mac, string_mac, source_mac)                                                               \
    SHR_IF_ERR_EXIT(dbx_xml_property_set_int(node_mac, string_mac, source_mac))

#define RHDATA_SET_HEX(node_mac, string_mac, source_mac)                                                               \
    SHR_IF_ERR_EXIT(dbx_xml_property_set_hex(node_mac, string_mac, source_mac))

#define RHDATA_SET_STR(node_mac, string_mac, source_mac)                                                               \
    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(node_mac, string_mac, source_mac))

#define RHDATA_SET_BOOL(node_mac, string_mac, source_mac)                                                              \
    SHR_IF_ERR_EXIT(dbx_xml_property_set_str(node_mac, string_mac, (source_mac != 0) ? "Yes":"No"))

#define RHDATA_MOD_INT(node_mac, string_mac, source_mac)                                                               \
    SHR_IF_ERR_EXIT(dbx_xml_property_mod_int(node_mac, string_mac, source_mac))

#define RHDATA_MOD_STR(node_mac, string_mac, source_mac)                                                               \
    SHR_IF_ERR_EXIT(dbx_xml_property_mod_str(node_mac, string_mac, source_mac))

#define RHDATA_DELETE(node_mac, string_mac)                                                                            \
    if((dbx_xml_property_delete(node_mac, string_mac)) != _SHR_E_NONE) {                                               \
        continue;                                                                                                      \
    }

#endif /* DBX_XML_H_INCLUDED */
