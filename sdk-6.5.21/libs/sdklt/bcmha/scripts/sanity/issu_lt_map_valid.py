#
# \file issu_lt_map_valid.py
#
# This script contains a class that verifies the LT maps. The class has two modes:
# a. Save the version information within the repository
# b. For a given start and target versions printout all the potential ISSU
# compatibility violations.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#

import sys
import os
from os import walk
import os.path
import yaml  # YAML parser
from collections import OrderedDict  # keep YAML parser output in order
from yaml.resolver import Resolver
import re

SANTY_INFO = '/sanity_info'
DB_FILE_PREFIX = 'sanity_'
DB_FILE_SUFFIX = '.yml'
BASE_FILE_NAME = '/bcmltd_id.yml'


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


class lt_map_validate:
    """Class to validate lt map definition changes between version"""

    def __init__(self, base_db_path, base_input_dir, devs, variants):
        self.issu_db_base_dir = base_db_path
        self.base_input_path = base_input_dir
        self.dev_included = devs
        self.var_included = variants

    def __check_dir(self, dest_dir):
        if not os.path.isdir(dest_dir):
            try:
                os.makedirs(dest_dir)
            except OSError:
                print('Failed to create target directory %s' % dest_dir)

    # Check if a given device and variant should be processed or filtered out
    def __filter(self, dev, variant):
        if dev == None or self.dev_included == None:  # no filtering
            return False
        if dev not in self.dev_included:
            return True

        if variant == None or self.var_included == None:
            return False
        else:
            return variant not in self.var_included

    # Determine the device and variant associated with the file name
    def __parse_file_name(self, name):
        d = name.find('_')
        if d == -1:
            return None, None
        v = name.find('_s_')
        if (v == -1):
            v = name.find('_v')
            if v == -1:
                return None, None
            dev = name[d + 1:v]
            return dev, None
        else:
            dev = name[d + 1:v]
            d = v + 3
            v = name.find('_v')
            if v == -1:
                return None, None
            variant = name[d:v]
            return dev, variant

    def __find_map_section(self, map_sect):
        for dev in map_sect:
            return map_sect[dev]
        return None

    def __find_variant_root(self, root):
        if not 'VARIANT' in root:
            if 'MAP' in root:
                return self.__find_map_section(root['MAP'])
            else:
                return None
        var = root['VARIANT']
        for dev in var:
            device = var[dev]
            break
        for var in device:
            variant = device[var]
            break
        return variant

    # Find the section of the yaml file that contains the TABLE sub-section.
    # This function returns the TABLE sub-section (if was found).
    def __find_main_tbl_section(self, root):
        sect = root['BCMLTD_ID']
        if 'TABLE' in sect:
            return sect['TABLE']
        var_root = self.__find_variant_root(sect)
        if var_root == None:
            return None

        if 'TABLE' in var_root:
            return var_root['TABLE']
        else:
            return None

    def __find_sanity_tbl_section(self, root):
        sect = root['BCMLTD_ID']
        if 'VARIANT' in sect:
            var_node = self.__find_variant_root(sect)
        else:
            var_node = sect
        if not 'MAP' in var_node:
            return None
        map = var_node['MAP']
        for dev in map:
            device = map[dev]
            break
        if 'TABLE' in device:
            return device['TABLE']
        else:
            return None

    def __process_file(self, root, out_f, is_root, base_root):
        if not is_root:
            main_tbl_sect = self.__find_main_tbl_section(root)
            if main_tbl_sect == None:
                return
            sanity_tbl_sect = self.__find_sanity_tbl_section(root)
            if sanity_tbl_sect == None:
                return
            for t in sanity_tbl_sect:
                if t in base_root:
                    b_tbl = base_root[t]
                    b_fld = b_tbl['FIELD']
                    for f in b_fld:
                        if 'key' in b_fld[f]:
                            tbl = sanity_tbl_sect[t]
                            fld = tbl['FIELD']
                            if f in fld:
                                fld[f]['key'] = 1
        else:
            sanity_tbl_sect = root

        try:
            fh = open(out_f, 'w')
        except:
            print('Failed to open source file %s' % out_f)
            return

        fh.write(yaml.dump(sanity_tbl_sect, encoding = 'utf-8'))
        fh.close()

    def __load_base_root(self):
        input_file = self.base_input_path + BASE_FILE_NAME
        try:
            fh = open(input_file, 'r')
        except:
            print('Failed to open an input file %s' % input_file)
            return None

        root = ordered_load(fh)
        fh.close()
        if root == None:
            print('Input file %s is not a good YAML file' % input_file)
            return root
        sect = root['BCMLTD_ID']
        if 'TABLE' in sect:
            return sect['TABLE']
        else:
            return None

    # Check for sane min and max field values
    def __validate_fields_attrib(self, s_fld, d_fld, s_f, tbl, ver, is_key):
        if 'min' in s_fld:
            val = s_fld['min']
            if 'min' in d_fld:
                d_val = d_fld['min']
                # Check if the minimum value had increased
                if int(val) < int(d_val):
                    if is_key:
                        print('The min value of the KEY field %s in table %s had increased from %s to %s in version %s' %
                              (s_f, tbl, val, d_val, ver))
                    else:
                        print('The min value of the field %s in table %s had increased from %s to %s in version %s' %
                              (s_f, tbl, val, d_val, ver))
                    return -1
        if 'max' in s_fld:
            val = s_fld['max']
            if 'max' in d_fld:
                d_val = d_fld['max']
                # Check if the minimum value had increased
                if int(val) > int(d_val):
                    if is_key:
                        print('The max value of the KEY field %s in table %s had decreased from %s to %s in version %s' %
                              (s_f, tbl, val, d_val, ver))
                    else:
                        print('The max value of the field %s in table %s had decreased from %s to %s in version %s' %
                              (s_f, tbl, val, d_val, ver))
                    return -2
        return 0

    # Validates field presence and attributes of old and new versions
    def __validate_fields(self, s_flds, d_flds, ver, dest, tbl):
        s_fld_sect = s_flds['FIELD']
        d_fld_sec = d_flds['FIELD']
        for s_f in s_fld_sect:
            if s_f in d_fld_sec:
                is_key = False
                if 'key' in s_fld_sect[s_f]:
                    if 'key' not in d_fld_sec[s_f]:
                        print('The field %s in %s is not a key anymore in version %s at file %s' %
                               (s_f, tbl, ver, dest))
                        continue
                    else:
                        is_key = True
                self.__validate_fields_attrib(s_fld_sect[s_f], d_fld_sec[s_f],
                                                   s_f, tbl, ver, is_key)
            else:
                print('Field %s in %s is missing from version %s file %s' % (s_f, tbl, ver, dest))

    # Validates source and destination files by validating each of their tables
    def __validate_files(self, src, dest, ver):
        try:
            src_fh = open(src, 'r')
            dest_fh = open(dest, 'r')
        except:
            print('Failed to open an input file %s or %s' % (src, dest))
            return
        s_root = ordered_load(src_fh)
        src_fh.close()
        d_root = ordered_load(dest_fh)
        dest_fh.close()
        if s_root == None or d_root == None:
            print('Input file %s or %s is not a good YAML file' % (src, dest))
            return

        for tbl in s_root:
            if tbl not in d_root:
                continue
            self.__validate_fields(s_root[tbl], d_root[tbl], ver, dest, tbl)

    # Public method to save the LT portion of the device DB files. The saved
    # output will be later compared between two different versions.
    def save_db_info(self, ver):
        dest_dir = self.issu_db_base_dir + '/ver_' + ver + SANTY_INFO
        self.__check_dir(dest_dir)
        src_path = self.base_input_path + '/lt'

        # Process the root file
        print('Processing base file %s' % BASE_FILE_NAME[1:])
        base_root = self.__load_base_root()
        if base_root != None:
            out_f = dest_dir + '/' + DB_FILE_PREFIX + 'v' + \
                    ver + DB_FILE_SUFFIX
            self.__process_file(base_root, out_f, True, None)
        else:
            print('no root was found')

        for dev in os.listdir(src_path):
            dev_path = src_path + '/' + dev
            if not os.path.isdir(dev_path):
                continue

            for (dirpath, dirnames, filenames) in walk(dev_path):
                for f in filenames:
                    if not f.endswith('.yml'):
                        continue;

                    variant = None
                    idx = f.find('_variant.')
                    if idx != -1:
                        variant = f[len(dev):idx]

                    if variant == None:
                        out_f = dest_dir + '/' + DB_FILE_PREFIX + dev + '_' + \
                                'v' + ver + DB_FILE_SUFFIX
                    else:
                        out_f = dest_dir + '/' + DB_FILE_PREFIX + dev + '_s' + \
                                variant + '_' + 'v' + ver + DB_FILE_SUFFIX

                    if os.path.isfile(out_f):  # Skip files that were already generated
                        continue

                    if self.__filter(dev, variant):
                        continue

                    in_file = dirpath + '/' + f
                    print('\nProcessing %s' % in_file)
                    try:
                        fh = open(in_file, 'r')
                    except:
                        print('Failed to open source file', in_file)
                        return -1
                    root = ordered_load(fh)
                    fh.close()
                    if root == None:
                        print('Failed to parse yaml file %s - check file format' % in_file)
                        return -1
                    self.__process_file(root, out_f, False, base_root)

    # Public method to compare and validate two versions
    def validate_lt_data(self, start_ver, to_ver):
        dest_dir = self.issu_db_base_dir + '/ver_' + to_ver + SANTY_INFO
        if not os.path.isdir(dest_dir):
            return False
        src_dir = self.issu_db_base_dir + '/ver_' + start_ver + SANTY_INFO
        if not os.path.isdir(src_dir):
            return False

        # Find data file pairs within the src and dest directories
        for src in os.listdir(src_dir):
            if (not src.endswith('.yml')) or (not src.startswith(DB_FILE_PREFIX)):
                continue
            basic_f_name = src.find('v' + start_ver)
            if basic_f_name == -1:
                print('Strange file name %s' % src)
                continue

            # Check if the source file should be filtered out
            dev, variant = self.__parse_file_name(src)
            if self.__filter(dev, variant):
                continue

            for dest in os.listdir(dest_dir):
                # Needs to include the _v in the comparison
                if dest[:basic_f_name + 1] == src[:basic_f_name + 1]:
                    print('\nValidates files %s vs. %s' % (src, dest))
                    self.__validate_files(src_dir + '/' + src,
                                         dest_dir + '/' + dest,
                                         to_ver)


def clean_python_default_boolean():
    for ch in "OoYyNn":
        if len(Resolver.yaml_implicit_resolvers[ch]) == 1:
            del Resolver.yaml_implicit_resolvers[ch]
        else:
            Resolver.yaml_implicit_resolvers[ch] = [x for x in
                    Resolver.yaml_implicit_resolvers[ch] if x[0] != 'tag:yaml.org,2002:bool']


def usage():
    print("python issu_lt_map_valid.py [-s <start-ver>] -v <version> -r [-h]")
    print("       [-d <root-db-dir>] [-c <dev-id>]* [-t <variant-id>]*")
    print('root-db-dir allows to overwrite the default ISSU DB dir ' \
          '$SDK/bcmissu/dll/db')
    print('Use the -r to generate report. -s and -v are mandatory parameters')
    print('The -c restricts the DB generation of only the desired device.')
    print('The -c option can be used multiple times for multiple devices.')
    print('By default all the devices are included.')
    print('After specifying a device the -t can be used to select specific')
    print('variant or a list of them by reusing the -t flag. All the variants')
    print('are included by default.')


import getopt

DEFAULT_ISSU_DB_DIR = '/bcmissu/dll/db'
FLTG_INPUT_DIR = '/devdb'


def main(argv):
    start_ver = None
    to_ver = None
    base_dir = None
    input_dir = None
    gen_report = False
    dev_included = []
    variant_included = []

    try:
        opts, args = getopt.getopt(argv, "hs:v:rd:c:t:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(-2)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit(0);
        elif opt == '-v':
            to_ver = arg
        elif opt == '-s':
            start_ver = arg
        elif opt == '-d':
            base_dir = arg
        elif opt == '-r':
            gen_report = True
        elif opt == '-c':
            if arg.lower() not in dev_included:
                dev_included.append(arg.lower())
        elif opt == '-t':
            if arg.lower() not in variant_included:
                variant_included.append(arg.lower())
        else:
            usage()
            sys.exit(0);

    if to_ver == None:
        usage()
        sys.exit(0);
    if gen_report and start_ver == None:
        usage()
        sys.exit(0);

    try:
        input_dir = os.environ['SDK'] + FLTG_INPUT_DIR
    except:
        print('the environment variable SDK must be defined')
        sys.exit(-1)

    if base_dir == None:
        try:
            base_dir = os.environ['SDK'] + DEFAULT_ISSU_DB_DIR
        except:
            print('the environment variable SDK must be defined')
            sys.exit(-1)

    # Make sure that our YAML files will be processed correctly
    clean_python_default_boolean()
    if len(dev_included) == 0:
        d = None
    else:
        d = dev_included
    if len(variant_included) == 0:
        v = None
    else:
        v = variant_included

    san_c = lt_map_validate(base_dir, input_dir, d, v)
    if gen_report:
        san_c.validate_lt_data(start_ver, to_ver)
    else:
        san_c.save_db_info(to_ver)


if __name__ == "__main__":
    if sys.version_info[0] < 2 or \
       (sys.version_info[0] == 2 and sys.version_info[1] < 7):
        print('Error: Unsupported python version, min version  2.7.x')
    else:
        main(sys.argv[1:])

