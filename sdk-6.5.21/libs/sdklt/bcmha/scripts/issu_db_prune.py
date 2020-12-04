# Prune the ISSU DB to have only the desired devices and variants
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
import sys
import os
import os.path
import shutil
import tempfile
import getopt
from os import walk

ISSU_DB_DIR = '/bcmissu/dll/db'
LTID_PREFIX = 'ltids_'
LTID_SUFFIX = 'ltdb'
GENERIC_NAME = 'gen'
DEV_VARIANT_DELIM = '_s_'
ENUM_PREFIX = 'enums_'
ANY_VARIANT = '*'


def parse_file_name(f):
    if f.startswith(LTID_PREFIX):
        i = len(LTID_PREFIX)
    else:
        i = len(ENUM_PREFIX)
    dev = f[i:]
    i = f.find(DEV_VARIANT_DELIM)
    if i == -1 or f[i + len(DEV_VARIANT_DELIM):].startswith(GENERIC_NAME):
        variant = ANY_VARIANT
    else:
        i = i + len(DEV_VARIANT_DELIM)
        variant = f[i:]
    return dev, variant


def dev_or_variant_match(f, item_list):
    if f.startswith(LTID_PREFIX + 'db') or len(item_list) == 0:
        return False

    dev, variant = parse_file_name(f)
    for item in item_list:
        if f.startswith(LTID_PREFIX):
            if dev.startswith(item[0]) and \
                (variant.startswith(item[1]) or variant == ANY_VARIANT):
                return True
        elif f.startswith(ENUM_PREFIX) and dev.startswith(item[0]):
            if variant.startswith(item[1]) or variant == ANY_VARIANT:
                return True

    return False


def delete_excluded_files(db_dir, out_dir, exclude, item_list):
    for f in os.listdir(db_dir):
        if not f.startswith(LTID_PREFIX) and not f.startswith(ENUM_PREFIX):
            if out_dir != None:
                shutil.copy(db_dir + '/' + f, out_dir + '/' + f)
            continue

        file_match = dev_or_variant_match(f, item_list)
        if out_dir == None:
            if file_match:
                if exclude:
                    os.remove(db_dir + '/' + f)
            elif not exclude:
                os.remove(db_dir + '/' + f)
        else:
            if file_match:
                if not exclude:
                    shutil.copy(db_dir + '/' + f, out_dir + '/' + f)
            elif exclude:
                shutil.copy(db_dir + '/' + f, out_dir + '/' + f)


def cleanup_ltid_file(fh, exclude, item_list, dev, out_fh):
    variant_list_sect = False
    for l in fh.readlines():
        if l.startswith('extern const issu_variant_info_t'):
            words = l.split()
            variant = words[3][len('bcmissu_lt_ids_variant_'):]
            found = False
            for item in item_list:
                if dev.startswith(item[0]) and variant.startswith(item[1]):
                    found = True
                    break
            if found:
                if not exclude:
                    out_fh.write(l)
            elif exclude:
                out_fh.write(l)
        elif l.find('*variant_list') != -1:
            out_fh.write(l)
            variant_list_sect = True
        elif variant_list_sect:
            i = l.find('ids_variant_')
            if i == -1:
                out_fh.write(l)
                variant_list_sect = False
            else:
                variant = l[i + len('ids_variant_'):]
                found = False
                for item in item_list:
                    if dev.startswith(item[0]) and variant.startswith(item[1]):
                        found = True
                        break
                if found:
                    if not exclude:
                        out_fh.write(l)
                elif exclude:
                    out_fh.write(l)
        else:
            out_fh.write(l)


def patch_ltids_files(db_dir, out_dir, exclude, item_list):
    for f in os.listdir(db_dir):
        if not f.startswith(LTID_PREFIX):
            continue
        dev, variant = parse_file_name(f)
        if variant != ANY_VARIANT:
            continue

        # Make sure that the device included in the list
        found = False
        for item in item_list:
            if dev.startswith(item[0]):
                found = True
        if found and exclude:
            continue
        elif not found and not exclude:
            continue

        tmp_file = tempfile.mktemp('.ltid')
        in_file = db_dir + '/' + f
        try:
            fh = open(in_file, 'r')
            out_fh = open (tmp_file, 'w')
        except:
            print('Error: Failed to open temp file\n')
            return
        cleanup_ltid_file(fh, exclude, item_list, dev, out_fh)
        fh.close()
        out_fh.close()
        if out_dir == None:
            shutil.move(tmp_file, in_file)
        else:
            shutil.move(tmp_file, out_dir + '/' + f)


def usage():
    print('python issu_db_prune.py <options>\n')
    print('options: -v <current_version> [-x] [-o <dest-dir>]')
    print('         -c <dev-id> -t <variant-id>*\n')

    print('The -v parameter is required to determine the current version.\n')

    print('The -x indicates that the specified devices and variants should be')
    print('excluded from the database. If the -x is option not present then the')
    print('specified devices and variant are included in the database, and')
    print('everything else is excluded.\n')

    print('The -o option specifies the output root directory. By')
    print('default the output directory is $SDK/bcmissu/dll/db/ver_<version>\n')

    print('The -c option adds a device to the list of devices to be included in')
    print('(or excluded from) the database. The -c option may be specified multiple')
    print('times, except if the -t option is also present, in which case the input')
    print('should contain a single -c <device> followed by one or more -t <variant> pairs.')
    print('This combination of a single -c <device> followed by 0 or more -t <variant>')
    print('pairs might repeat itself multiple times.\n')

    print('The -t option adds a variant to the list of variants to be included in,')
    print('(or excluded from) the database. The -t option may be specified multiple')
    print('times but always after the -c option.')


def main(argv):
    version = None
    item_list = []
    exclude = False
    dev = None
    variant = ''
    output_dir = None

    try:
        sdk = os.environ['SDK']
    except:
        print('environment variable SDK required')
        sys.exit(-1)

    # Check if there are any input parameters
    try:
        opts, args = getopt.getopt(argv, "hxv:o:c:t:", ["help"])
    except getopt.GetoptError:
        usage()
        sys.exit(-1)
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            usage()
            sys.exit(0);
        elif opt == '-o':
            output_dir = arg
        elif opt == '-v':
            version = arg
        elif opt == '-x':
            exclude = True
        elif opt == '-c':
            # Check if the device has no variants
            if variant == None:
                item = (dev, ANY_VARIANT)
                # Add it to the list if it is not already there
                if item not in item_list:
                    item_list.append(item)
            dev = arg.lower()
            variant = None
        elif opt == '-t':
            if dev == None:
                print('Variant parameter must be preceded by a device parameter (-c)')
                usage()
                sys.exit(-1)
            variant = arg.lower()
            item = (dev, variant)
            if item not in item_list:
                item_list.append(item)
        else:
            usage()
            sys.exit(-1)

    # Take care of the case where the last input was a device without variant
    if variant == None:
        item = (dev, ANY_VARIANT)
        # Add it to the list if it is not already there
        if item not in item_list:
            item_list.append(item)

    if version == None:
        print('Error: version must be defined')
        usage()
        sys.exit(-1)

    input_db_dir = sdk + ISSU_DB_DIR + '/ver_' + version
    delete_excluded_files(input_db_dir, output_dir, exclude, item_list)
    patch_ltids_files(input_db_dir, output_dir, exclude, item_list)


if __name__ == "__main__":
    if sys.version_info[0] < 2 or \
       (sys.version_info[0] == 2 and sys.version_info[1] < 7):
        print('Error: Unsupported python version, min version 2.7.x')
    else:
        main(sys.argv[1:])
