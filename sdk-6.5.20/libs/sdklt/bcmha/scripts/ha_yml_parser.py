#
# \file ha_yaml_parser.py
#
# Parsing YAML file describing HA data structure and generating
# 1. Header file to be included by the HA application
# 2. Data structure migration code from one version to higher one
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
import sys
import os
import getopt
import yaml  # YAML parser
from collections import OrderedDict  # keep YAML parser output in order
import hashlib  # support md5 hashing
import issu_db_gen
import tempfile
import os.path
import shutil
import filecmp
import re  # Regular expression

# This is a list of known types. Newly defined types are added to this list
# during parsing. The list is used to validate the type used.
variable_type_list = {'int', 'char', 'short', 'long', 'bool',
                      'unsigned', 'unsigned char', 'unsigned short',
                      'unsigned long', 'long long', 'unsigned long long',
                      'uint8_t', 'uint16_t', 'uint32_t', 'uint64_t',
                      'int8_t', 'int16_t', 'int32_t', 'int64_t',
                      'shr_ha_ptr_t', 'shr_ha_lptr_t', 'SHR_BITDCL'}
const_dict = {}

output_file = ''

comp_cls = None


# Validate type. Type might be one or two segments. The first must be
# one of the types defined in variable_type_list and the second (if exist)
# must be * (for pointer)
def validate_type (var_type):
    global variable_type_list

    # remove pointer if exist
    segments = var_type.split()
    n_of_elem = len(segments)
    if n_of_elem > 2:
        print("too many elements in type %s" % var_type)
        return False
    elif (n_of_elem == 2) and (segments[1] == '*'):
        print("type %s can't be a pointer" % var_type)
        return False
    return var_type in variable_type_list


# Add new data type into the list of 'known' types (variable_type_list)
def update_valid_types (data_type):
    global variable_type_list
    variable_type_list.add(data_type)


# Parse structure attribute block.
# Params:
# elem is the attribute object
# struct_name is the structure typedef name
# out_fh is the output file handler
def struct_attrib_parse(elem, struct_name, out_fh):
    for val in elem:
        if val == 'version':
            if type(elem[val]) != int:
                print("Syntax error version %s must be integer" % elem[val])
                return False
    return True


# Parse enum structure.
# Params:
# elem is the enum object
# name is the enum typedef name
# file_name is the currently processed file
# out_fh is the output file handler
# comp_cls Component class to define the enum type with it.
def enum_parse(elem, name, file_name, out_fh, comp_cls):
    expected_val = 0
    # sanity check - verify that the type is not already defined
    if validate_type(name):
        print('error: type %s already defined\n' % name)
        return False

    # Build the structure ID
    m = hashlib.md5()
    struct_name = file_name + name
    m.update(struct_name.encode())

    sdb = issu_db_gen.structure_db(file_name, name, m.hexdigest()[:16],
                                   comp_cls, False)

    out_fh.write('typedef enum {\n')
    idx = 1
    for iter in elem:
        if iter == 'type':
            continue
        elif iter == 'fields':
            fields = elem[iter]
            if type(fields) != OrderedDict:
                continue;
            for field in fields:
                if field != field.upper():
                    print('enum values must be in upper case %s' % field)
                    return False
                val = fields[field]

                line = ''
                if val == None:
                    line = '   ' + field
                else:
                    line = '   ' + field + ' = ' + str(val)
                    expected_val = val

                if idx < len(fields):  # last element doesn't followed with comma
                    line = line + ','

                out_fh.write('%s\n' % line)
                idx = idx + 1
                sdb.add_enum_val(field, expected_val)
                expected_val += 1

    out_fh.write('} %s;\n' % name)

    # update the valid types with the new structure
    update_valid_types (name)

    return sdb.output_to_file()


# Process field attributes object
# This function process the field attributes object. As a result of processing
# the function sets the following global parameters:
# is_table - Indicating if the field is a table.
# is_array - Indicating if the field is an array.
# array_size - If the field is an array specifies the array size.
# table_size_var - The variable name that its value indicates the number of
# elements in the table. This variable only set if is_table == True
# Params:
# attr is the field attribute object
# comment is a list where the comment string can be added if available
def process_field_attrib(attr, comment):
    global is_table
    global is_array
    global array_size
    global table_size_var
    global is_volatile
    global const_dict
    global is_ltid
    global is_fid
    global ltid_field
    global enum_name

    field_type = None
    table_size_var = None
    for field in attr:
        if field == 'type':
            field_type = attr[field]
        elif field == 'length':
            is_array = True
            if type(attr[field]) == str and attr[field] not in const_dict:
                print('undefined const %s' % attr[field])
                return None
            array_size = attr[field]
        elif field == 'comment':
            comment.append('/*! ' + attr[field] + ' */')
        elif field == 'volatile':
            is_volatile = True
        elif field == 'table':
            is_table = True
        elif field == 'size':
            table_size_var = attr[field]
        elif field == 'ltid':
            is_ltid = True
        elif field == 'fid':
            is_fid = True
            ltid_field = attr[field]
        elif field == 'enum':
            enum_name = attr[field]
        else:
            print('unknown field attribute ', field)
            return None
    return field_type


# Process field.
# Params:
# field - Is the field type + comment (if present).
# comment is a list where the comment string can be added if available.
def process_field(field, comment):
    idx = field.find('/*')
    if idx != -1:
        comment.append(field[idx:])

    words = field.split()
    if len(words) > 1 and words[1] == '*':
        return words[0] + ' ' + words[1]
    else:
        return words[0]


# Process the object 'fields'.
# Params:
# fl - Is the field object.
# out_fh - Is the output file handler.
def struct_fields_parse(fl, out_fh, sdb):
    global is_table
    global is_array
    global array_size
    global table_size_var
    global is_volatile
    global is_ltid
    global is_fid
    global ltid_field
    global enum_name

    for field in fl:
        comment = ['']
        is_table = False
        is_array = False
        array_size = None
        is_volatile = False
        is_ltid = False
        is_fid = False
        array_size_num = 0
        ltid_field = 0
        enum_name = None

        if type(fl[field]) == OrderedDict:  # Var has attached attributes
            data_type = process_field_attrib(fl[field], comment)
            if not data_type:
                print('Missing type for ', field)
                return False
        else:
            data_type = process_field(fl[field], comment)

        if (not validate_type(data_type)):  # validate data type
            print("invalid type %s" % data_type)
            return False

        if is_array:
            if not array_size:
                print('Invalid array size for field ', field)
                return False
            else:
                field = field + '[' + str(array_size) + ']'
                if type(array_size) == str:
                    array_size_num = const_dict[array_size]
                else:
                    array_size_num = array_size
        elif is_table:
            if not table_size_var:
                print('Missing variable to indicate size for ', field)
                return False
            else:
                field = field + '[]'
        elif is_fid and not ltid_field:
            print('Missing value to indicate parent SID for ', field)
            return False

        if len(comment) > 1:
            out_fh.write('   %s\n' % comment[1])

        if data_type.find('*') != -1:
            is_volatile = True
            out_fh.write('   %s%s;\n' % (data_type, field))
        else:
            out_fh.write('   %s %s;\n' % (data_type, field))

        # Update the ISSU class with the field information
        sdb.add_field(field, data_type, is_array, array_size, array_size_num,
                      is_table, table_size_var, is_ltid, is_fid, ltid_field, enum_name)

    return True


# Extract comment at the data type level and write it into the output file
# The output is doxygen format comment. To have multiple lines the input should
# have \n as line delineator.
# Params:
# fl - Is the structure object.
# out_fh - Is the output file handler.
def extract_comment(fl, out_fh):
    comment_str = None
    for field in fl:
        if field == 'comment':
            comment_str = fl[field]
            break

    if comment_str == None:
        return
    out_fh.write('/*!\n')
    lines = comment_str.split('\\n')
    for l in lines:
        if l[0] == '\n':
            l = l[1:]
        if len(l) > 77:
            idx = 0
            idx1 = l.find(' ')
            line_len = idx1
            line_ready = False
            while idx1 != -1:
                if line_ready:
                    out_fh.write(' * %s\n' % l[idx:idx + line_len])
                    idx = idx + line_len + 1
                    line_len = 0
                    line_ready = False
                else:
                    idx1 = l[idx + line_len + 1:].find(' ')
                    if idx1 > 0:
                        if line_len + idx1 > 77:
                            line_ready = True
                            continue
                        else:
                            line_len = line_len + idx1 + 1
                    elif idx1 == 0:
                        line_len = line_len + 1
            if line_len > 0:  # Write the rest of the line.
                # for multiple lines the last line will have cr linefeed
                idx1 = l[idx:].find('\n')
                if idx1 == -1:
                    out_fh.write(' * %s\n' % l[idx:])
                else:
                    out_fh.write(' * %s\n' % l[idx:idx + idx1])
        else:
            if len(l) == 0:
                out_fh.write(' *\n')
            else:
                out_fh.write(' * %s\n' % l)
    out_fh.write(' */\n')


# Process structure object.
# Params:
# fl - Is the structure object.
# name - Is the name of the data structure.
# file_name - Is the name of the input file. It is used as part of the structure
# ID.
# out_fh - Is the output file handler.
def struct_parse(fl, name, file_name, out_fh, comp_cls):

    # Sanity check - verify that the type is not already defined
    if validate_type(name):
        print('error: type %s already defined\n' % name)
        return False

    # Build the structure ID
    m = hashlib.md5()
    struct_name = file_name + name
    m.update(struct_name.encode())

    sdb = issu_db_gen.structure_db(file_name, name, m.hexdigest()[:16],
                                   comp_cls, True)

    out_fh.write('typedef struct {\n')
    for field in fl:
        if field == 'attrib':
            attrib_field = fl[field]
        elif field == 'fields':
            if not struct_fields_parse(fl[field], out_fh, sdb):
                return False
        elif field == 'type' or field == 'comment':
            continue  # type was processed before
        else:
            print('unknown key ', field)
            return False

    out_fh.write('} %s;\n\n' % name)

 #   struct_attrib_parse(attrib_field, name, out_fh)
    out_fh.write('#define %s_ID 0x%s\n' %
                 (name.upper(), m.hexdigest()[:16]))

    # update the valid types with the new structure
    update_valid_types (name)

    return sdb.output_to_file()


# Find data type of a data type declaration
# Params:
# fl - Is the structure object.
def find_data_type(fl):
    rv = None
    for item in fl:
        if item == 'type':
            rv = fl[item]
            break
    return rv


# Process constant definition and add the constant and its value into
# a global dictionary.
# Params:
# fl - Is the structure object.
def process_constants(fl):
    global const_dict

    constants = ''
    for item in fl:
        if item != item.upper():
            print('Constants musts be upper case %s' % item)
            return ''
        constants = constants + '#define ' + item + ' ' + str(fl[item]) + '\n'
        const_dict[item] = fl[item]
    return constants


# Write the fixed header to the output file.
# Params:
# out_fh - Is the output file handler
def write_output_banner(out_fh):
    out_fh.write('/**************************************************************************\n')
    out_fh.write(' *\n')
    out_fh.write(' * DO NOT EDIT THIS FILE!\n')
    out_fh.write(' * This file is auto-generated by HA parser from YAML formated file.\n')
    out_fh.write(' * Edits to this file will be lost when it is regenerated.\n')
    out_fh.write(' * Tool: bcmha/scripts/ha_yml_parser.py\n')
    out_fh.write(' *\n')
    out_fh.write(' * $Copyright:.$\n')
    out_fh.write(' *\n')
    out_fh.write(' **************************************************************************/\n')


# Standard yaml.load mixes the order of the dictionary so it doesn't
# match the order in the YAML file. This loader makes the trick.
def ordered_load(stream, Loader = yaml.Loader, object_pairs_hook = OrderedDict):

    class OrderedLoader(Loader):
        pass

    def construct_mapping(loader, node):
        loader.flatten_mapping(node)
        return object_pairs_hook(loader.construct_pairs(node))

    OrderedLoader.add_constructor(
        yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG, construct_mapping)
    rv = None
    try:
        rv = yaml.load(stream, OrderedLoader)
    except:
        e = sys.exc_info()[0]
        print('exception = ', e)
    return rv


# Top level function to parse the entire xml file. It start processing the
# tree where every child tag being processed separately.
def parse_file(file_name, out_fh, nest_level, ver):
    global output_file
    global comp_cls

    try:
        file = open(file_name, 'r')
    except:
        print('invalid source file ', file_name)
        return False

    root = ordered_load(file)

    if root == None:
        print('Failed to parse yaml file - check file format')
        return False

    constants = ''

    for declr in root:
        if declr == 'consts':
            constants = process_constants(root[declr])
        elif declr == 'include':
            parse_file(root[declr], out_fh, nest_level + 1, ver)
        else:
            if nest_level == 0:
                if declr != 'component':
                    print("%s Expected component <name> for component"
                          "declaration\n" % declr)
                    return False
                if len(root[declr]) != 1:
                    print("expecting single component declaration"
                          "found %d\n" % len(root[declr]))
                    return False
                component = root[declr]
                for elem in component:
                    break

                comp_cls = issu_db_gen.component_module(os.environ['SDK'], ver,
                                                        elem, output_file)
                tree = component[elem]
            else:
                tree = root[declr]

            if len(constants) > 1:
                out_fh.write ('%s\n' % constants)

            for items in tree:
                data_type = find_data_type(tree[items])
                if data_type == None:
                    print("missing type for data declaration")
                    return False

                extract_comment(tree[items], out_fh)
                if data_type == 'enum':
                    rv = enum_parse(tree[items], items, file_name,
                                    out_fh, comp_cls)
                elif data_type == 'struct':
                    rv = struct_parse(tree[items], items, file_name,
                                      out_fh, comp_cls)
                else:
                    print("unknown declaration word %s. Expected struct"
                          "or enum\n" % data_type)
                    rv = False
                if not rv:
                    return False

                out_fh.write('\n')

    return True


def usage():
    print("python ha_yml_parser.py -g <yaml-file> -o <output-header-file>\
          -v <version>")


# The main function.
# Parse input parameters, open output file and call the file
# parser.
def main(argv):
    global comp_cls
    yaml_file = ''
    ver = ''
    global output_file
    tmp_file = ''
    out_file_path = None

    try:
        opts, args = getopt.getopt(argv, "hg:o:v:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit(0);
        elif opt == '-g':
            yaml_file = arg
        elif opt == '-o':
            output_file = arg
        elif opt == '-v':
            ver = arg
        else:
            usage()
            sys.exit(0);

    if yaml_file == '' or output_file == '':
        usage()
        sys.exit(0);

    if ver == '':
        try:
            ver = os.environ['SDK_VERSION']
        except:
            ver = ''

    if ver != '':
        # Validate that the SDK version has a legal string
        p = re.compile('[^a-zA-Z0-9_.-]')
        if p.search(ver) != None:
            print('Invalid SDK_VERSION string\n')
            sys.exit(0);

    # The output file is generated as temp file first.
    tmp_file = tempfile.mktemp('.gen')
    try:
        out_fh = open (tmp_file, 'w')
    except:
        print('Error: Failed to open temp file\n')
        return

    # Write the banner of the output file first
    write_output_banner(out_fh)

    index = output_file.rfind('/')
    if index == -1:
        header_protect = output_file.upper()
    else:
        header_protect = output_file[index + 1:].upper()
        out_file_path = output_file[:index]
    header_protect = header_protect.replace('.', '_')

    # Protect the header file from being included multiple times
    out_fh.write ("#ifndef %s\n" % header_protect)
    out_fh.write ("#define %s\n\n" % header_protect)

    # Add mandatory include header
    out_fh.write('#include <shr/shr_ha.h>\n\n')

    rv = parse_file(yaml_file, out_fh, 0, ver)

    out_fh.write ("#endif /* %s */\n" % header_protect)

    out_fh.close()
    if not rv:
        os.remove(tmp_file)
        sys.exit(-1)
    else:
        # Validate that the output directory is available. Create it if not.
        if (not os.path.isdir(out_file_path)):
            try:
                os.makedirs(out_file_path)
            except OSError:
                print('Failed to create target directory\n')

        # If the destination file does not exist or different from the temporary
        # file then move the temporary file over. Otherwise the temporary file
        # is not required (since identical file already present) so delete it.
        if (not os.path.isfile(output_file)) or \
           (not filecmp.cmp(tmp_file, output_file)):
            shutil.move(tmp_file, output_file)
        else:
            os.remove(tmp_file)

        comp_cls.save_generated_header(output_file)
        comp_cls.save_source_yaml(yaml_file)
        comp_cls.shutdown()


if __name__ == "__main__":


    if sys.version_info[0] < 2 or \
       (sys.version_info[0] == 2 and sys.version_info[1] < 7):
        print('Error: Unsupported python version, min version  2.7.x')
    else:
        main(sys.argv[1:])

