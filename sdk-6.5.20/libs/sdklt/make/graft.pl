#!/usr/bin/env perl
#
# graft.pl
#
# Graft separately delivered variant directories into an SDKLT tree.
#
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
#
# This tool updates and C and YAML files in an SDKLT source tree to
# accommodate inclusions and removals of device variant directories.
#
# Generally, this tool is run without arguments after variant directories
# are added or removed.
#
# Device/variant name canonicalization
# Device and variant names are canonicalized as follows:
#   Names are all *lower case* on file system paths
#   Names are all *UPPER CASE* in Perl and YAML data structures
#
# This implies that functions operating on file system paths generally
# handle both upper and lower case forms.
#
#

use strict;
use warnings;
use English qw( -no_match_vars );
use Carp;
use Getopt::Std;
use File::Basename;
use File::Find;
use File::Temp;
use Data::Dumper;
use Cwd 'realpath';
use YAML::XS;

our $VERSION = '0.0.1';

my %opt;
my %info;
my $sdk;
my $lt_count = {}; # dev variant name
my $lt_by_name = {}; # dev variant name
my $lt_initial_id = {}; # dev variant
my $max_field_per_table;
my $table_count;
my $variant_max;
my $variant_devdb = {}; # from devdb by device
my $variant_changes = {}; # variant add/remove from fs by device
my $sym_needs_preservation = {};

# From Fltg ComposableHeaders
my $composable_headers = [
    "bcmltd/include/bcmltd/chip/bcmltd_str.h",
    "bcmltd/include/bcmltd/chip/bcmltd_internal_id.h",
    "bcmltd/include/bcmltd/chip/bcmltd_table_frag.h",
    "bcmltd/chip/bcmltd_ltconf.h",
    "bcmltd/include/bcmltd/chip/bcmltd_config_variant_internal.h",
    "bcmltd/include/bcmltd/chip/bcmltd_variant_defs_internal.h",
    "bcmlrd/include/bcmlrd/chip/bcmlrd_variant_defs_internal.h",
    "bcmltd/include/bcmltd/chip/bcmltd_variant_entry.h",
    ];

# bcmltd table and variant IDs sections in update_data_header
my $data_headers = [
    "id",
    "variant",
    "id_limit",
    "variant_limit",
    ];

################################################################################
#
# sub: HELP_MESSAGE
#
# Issue help message.
#
# Only 'public' arguments are described; test and development arguments
# are not.
#

sub HELP_MESSAGE
{
    my $msg = <<"EOF";
Options:
  -i                   Show device and variant information.
  -n                   Perform operations but do not update filesystem.
  -C [dir]             Perform operations in directory [dir].
  -v                   Turn verbose messages on.
  -h                   This help message.
EOF
    printf $msg;

    return 0;
}

################################################################################
#
# sub: msg
#
# Issue a message when verbose
#

sub msg
{
    my ($s) = @_;

    if (defined $opt{v}) {
        print $s."\n"
            or croak "Couldn't write stdout: $OS_ERROR";
    }

    return 0;
}

################################################################################
#
# sub: variant_sort
#
# Sort filenames such that BASE files precede VARIANT files.
#
# Count number of path separators and sort files with smaller numbers
# of separators before larger numbers of path separators. Paths with
# the same number of separators are themselves sorted alphabetically.
#

sub variant_sort
{
    my ($a, $b) = @_;
    my @la = split /\//, $a;
    my @lb = split /\//, $b;
    my $sa = scalar @la;
    my $sb = scalar @lb;
    my $cmp = $sa <=> $sb;

    if ($cmp == 0) {
        # same number of separators
        $cmp = $a cmp $b;
    }

    return $cmp;
}

################################################################################
#
# sub: ordered_files
#
# Return an array of input files according to the configured
# ordering parameter.
#

sub ordered_files
{
    my ($order, $in) = @_;
    my @out;

    if ($order eq "forward") {
        @out = sort @{$in};
    } elsif ($order eq "reverse") {
        @out = reverse sort @{$in};
    } elsif ($order eq "variant") {
        @out = sort {variant_sort($a,$b)} @{$in};
    } else {
        confess "unsupported ordering $order";
    }

    return @out;
}

################################################################################
#
# sub: composable_includes
#
# Generate a composable #includes from the files found in 'dir' according
# to the given sort order.
#
#

sub composable_includes
{
    my ($dir, $order, $output) = @_;
    my $data;
    my @files;
    my $base_dir = basename($dir);

    croak "oops" if !defined $dir;
    croak "oops" if !defined $order;

    msg "dir: $dir";
    msg "order: $order";

    # $dir must exist
    File::Find::find(
        sub {push @files, $File::Find::name if -f $_}, $dir);

    # There must be files
    # return 0 if (scalar @files) == 0;

    $data = "";

    foreach my $file (ordered_files($order, \@files)) {
        $file =~ s%${dir}%%; # $file already has leading path separator
        $data .= "#include \"../".$base_dir.$file."\"\n";
    }

    ${$output} = $data;

    return 0;
}

################################################################################
#
# sub: composable_section
#
# Generate a composable section.
#

sub composable_section
{
    my ($dir, $order, $template, $output) = @_;
    my @refinc;
    my $incdir = $dir;

    if ($template =~ /#include\s+"(\S+)"/) {
        @refinc = split /\//, $1;
        $incdir .= "/" . $refinc[0] . "/" . $refinc[1];
    }
    if (scalar @refinc == 0) {
        croak "Could not match reference include file $template"
    }

    composable_includes($incdir, $order, $output);

    return 0;
}

################################################################################
#
# sub: safe_update_file
#
# Update a file safely if changed
#

sub safe_update_file
{
    my ($file, $outfile) = @_;

    system "cmp -s ${file} ${outfile}";
    my $result = $CHILD_ERROR;
    msg "cmp -s ${file} ${outfile} = $result";
    if (defined $opt{n}) {
        my $str = "No update mode: $file was";
        if ($result == 0) {
            $str .= " not";
        }
        $str .= " changed";
        msg($str);
        system "rm -f $outfile";
    } else {
        if ($result == 0) {
            msg("Removing $outfile");
            system "rm -f $outfile";
        } else {
            my $bak = "${file}.bak";
            msg("Creating $bak");
            system "cp $file ${file}.bak";
            msg("Moving $outfile to $file");
            system "mv $outfile $file";
        }
    }

    return 0;
}

################################################################################
#
# sub: update_composable_header_body
#
# Composable header state machine
#

sub update_composable_header_body
{
    my ($IN, $file, $OUT, $outfile) = @_;
    my $dir = dirname($file);
    my $state = 'scan';
    my $order;
    my $changed = 0;

    while (<$IN>) {
        if ($state eq 'scan') {
            print {$OUT} $_
                or croak "Couldn't write ${file}: $OS_ERROR";
            if ($_ =~ /[+]replace (\w+)/) {
                $order = $1;
                $state = 'replace';
            }
        } elsif ($state eq 'replace') {
            confess "oops" if !defined $order;
            my $section;
            composable_section($dir, $order, $_, \$section);
            print {$OUT} $section
                or croak "Couldn't write ${outfile}: $OS_ERROR";

            $state = 'skip';
            $changed = 1;

        } elsif ($state eq 'skip') {
            if ($_ =~ /-replace/) {
                print {$OUT} $_
                    or croak "Couldn't write ${outfile}: $OS_ERROR";
                $state = 'copy';
            }
        } elsif ($state eq 'copy') {
            print {$OUT} $_
                or croak "Couldn't write ${outfile}: $OS_ERROR";
        } else {
            confess "? $state";
        }
    }

    croak "Error updating $file: unexpected state $state." if $state ne 'copy';

    if ($changed) {
        msg "Changed $file to $outfile";
    } else {
        msg "Unchanged $file";
    }

    return $changed;
}

################################################################################
#
# sub: update_composable_header
#
# Update a given composable header file
#

sub update_composable_header
{
    my ($file) = @_;

    open my $IN, '<', $file
        or croak "Couldn't open ${file}: $OS_ERROR";

    my ($OUT, $outfile) = File::Temp::tempfile($file.".XXXXXXXX");

    update_composable_header_body($IN, $file, $OUT, $outfile);

    close $IN or croak "Couldn't close ${file}: $OS_ERROR";
    close $OUT or croak "Couldn't close ${outfile}: $OS_ERROR";

    safe_update_file($file, $outfile);

    return 0;
}

################################################################################
#
# sub: update_defined_value_body
#
# Update defined value state machine
#

sub update_defined_value_body
{
    my ($IN, $file, $OUT, $outfile, $sym_value) = @_;
    my $state = 'scan';
    my $checked = {};
    my $changed = 0;
    my @syms = keys %{$sym_value};
    my $num_syms = scalar @syms;
    # test for .bak
    # copy mv? in to bak
    while (<$IN>) {
        my $line = $_;
        if ($state eq 'scan') {
            my $num_checked = 0;
            foreach my $sym (@syms) {
                if (!defined $checked->{$sym}) {
                    if ($_ =~ /^(#define\s+${sym}\s+)(\w+)(.*)$/) {
                        my $value = $sym_value->{$sym};
                        my $pre = $1;
                        my $old_value = $2;
                        my $post = $3;
                        if ($value != $old_value) {
                            $line = $pre.$value.$post."\n";
                            $changed = 1;
                        }
                        $checked->{$sym} = 1;
                    }
                }
                if (defined $checked->{$sym}) {
                    $num_checked++;
                }
            }
            if ($num_checked == $num_syms) {
                $state = 'copy';
            }
        } elsif ($state eq 'copy') {
            # do nothing...
        } else {
            confess "? $state";
        }
        print {$OUT} $line
            or croak "Couldn't write ${outfile}: $OS_ERROR";

    }

    croak "Error updating $file: unexpected state $state." if $state ne 'copy';

    return $changed;
}

################################################################################
#
# sub: update_defined_value
#
# Update a #define symbol to a value
#
# sym_value is a hash of sym,value pairs
#

sub update_defined_value
{
    my ($file, $sym_value) = @_;

    open my $IN, '<', $file
        or croak "Couldn't open ${file}: $OS_ERROR";

    my ($OUT, $outfile) = File::Temp::tempfile($file.".XXXXXXXX");
    my $changed = update_defined_value_body($IN, $file,
                                            $OUT, $outfile,
                                            $sym_value);

    close $IN or croak "Couldn't close ${file}: $OS_ERROR";
    close $OUT or croak "Couldn't close ${outfile}: $OS_ERROR";

    safe_update_file($file, $outfile);

    return 0;
}

################################################################################
#
# sub: update_id_header
#
# Update the table ID headers
#

sub update_id_header
{
    my $dir = "${sdk}/bcmltd/include/bcmltd/id";
    my @files;

    File::Find::find(
        sub {push @files, $File::Find::name if -f $_}, $dir);

    foreach my $file (@files) {
        my $relfile = $file;
        $relfile =~ s%${dir}/%%;
        my @fcomp = split /\//, $relfile;
        my $num = scalar @fcomp;
        if ($num == 1) {
            # BASE - ignore
        } elsif ($num == 3) {
            # VARIANT
            my $device = $fcomp[0];
            my $variant = $fcomp[1];
            my $udevice = uc $device;
            my $uvariant = uc $variant;
            my $initial_id = $lt_initial_id->{$udevice}->{$uvariant};
            my $sym = "${udevice}_${uvariant}_LTD_INITIAL_TABLE_ID";
            croak "oops" if !defined $initial_id;
            update_defined_value($file, {$sym => $initial_id});
        } else {
            croak "Unexpected path for $file";
        }
    }

    return 0;
}

################################################################################
#
# sub: update_variant_header
#
# Update the variant ID headers
#

sub update_variant_header
{

    my $dir = "${sdk}/bcmltd/include/bcmltd/defs";
    my @files;
    my $v_id = 1;
    my $lvid_per_dev = {};

    File::Find::find(
        sub {push @files, $File::Find::name if -f $_}, $dir);

    foreach my $file (ordered_files("variant", \@files)) {
        my $relfile = $file;
        $relfile =~ s%${dir}/%%;
        my @fcomp = split /\//, $relfile;
        my $num = scalar @fcomp;
        my $device;
        my $variant;
        if ($num == 2) {
            # BASE
            $device = $fcomp[0];
            $variant = 'BASE';
            $lvid_per_dev->{$device} = 0;
        } elsif ($num == 3) {
            # VARIANT
            $device = $fcomp[0];
            $variant = $fcomp[1];
            $lvid_per_dev->{$device}++;
        } else {
            croak "Unexpected path for $file";
        }

        my $udevice = uc $device;
        my $uvariant = uc $variant;
        my $gsym = "BCMLTD_VARIANT_${udevice}_${uvariant}";
        my $lsym = "BCMLTD_VARIANT_LOCAL_${udevice}_${uvariant}";
        my $match_data = {
            $gsym => $v_id,
            $lsym => $lvid_per_dev->{$device},
        };

        update_defined_value($file, $match_data);
        $v_id++;
    }

    return 0;
}

################################################################################
#
# sub: update_id_limit_header
#
# Update the table ID limit header
#

sub update_id_limit_header
{
    my $file;
    my $sym_value = {
        "BCMLTD_TABLE_COUNT" => $table_count,
        "BCMLTD_MAX_FIELD_PER_TABLE" => $max_field_per_table,
    };

    $file .= "${sdk}/bcmltd/include/bcmltd/chip/bcmltd_limits.h";
    update_defined_value($file, $sym_value);

    return 0;
}

################################################################################
#
# sub: update_variant_limit_header
#
# Update the variant limit header
#

sub update_variant_limit_header
{
    my $file;
    my $sym_value = {
        "BCMLTD_VARIANT_MAX" => $variant_max,
    };

    $file .= "${sdk}/bcmltd/include/bcmltd/chip/bcmltd_variant_limits.h";
    update_defined_value($file, $sym_value);

    return 0;
}

################################################################################
#
# sub: update_data_header
#
# Update a header containing LT or variant data
#

sub update_data_header
{
    my ($section) = @_;
    my $rc = 0;

    if ($section eq "id") {
        update_id_header();
    } elsif ($section eq "variant") {
        update_variant_header();
    } elsif ($section eq "id_limit") {
        update_id_limit_header();
    } elsif ($section eq "variant_limit") {
        update_variant_limit_header();
    } else {
        croak "unknown section $section";
    }

    return $rc;
}

################################################################################
#
# sub: analyze_devdb_dir
#
# Analyze devdb LT files.
#

sub analyze_devdb_dir
{
    my $devdb = "${sdk}/devdb";
    my $bcmltd_id = "${devdb}/bcmltd_id.yml";
    my $devdb_lt = "${devdb}/lt";
    my @files;

    if (! -d "$sdk") {
        croak "directory $sdk not found";
    }
    if (! -d $devdb) {
        croak "directory $devdb not found";
    }
    if (! -f $bcmltd_id) {
        croak "file $bcmltd_id not found";
    }
    if (! -d $devdb_lt) {
        croak "directory $devdb_lt not found";
    }
    File::Find::find(
        sub {push @files, $File::Find::name if -f $_}, $devdb_lt);

    foreach my $file (@files) {
        $file =~ s%${devdb_lt}/%%;
        my @fcomp = split /\//, $file;
        my $num = scalar @fcomp;
        my $device;
        my $variant;
        my $udevice;
        my $uvariant;
        my $yml;

        if ($num == 2) {
            $device = $fcomp[0];
            $variant = 'BASE';
            $yml = $fcomp[1];
        } elsif ($num == 3) {
            $device = $fcomp[0];
            $variant = $fcomp[1];
            $yml = $fcomp[2];
        } else {
            croak "unexpected $num separators in $file";
        }
        croak "oops" if !defined $device;
        croak "oops" if !defined $variant;
        croak "oops $num $file" if !defined $yml;
        $udevice = uc $device;
        $uvariant = uc $variant;
        if (!defined $info{$udevice}) {
            $info{$udevice} = {};
        }
        if (!defined $info{$udevice}->{$uvariant}) {
            $info{$udevice}->{$uvariant} = {};
        }
        $info{$udevice}->{$uvariant}->{yml} = "${devdb_lt}/${file}";
    }

    return 0;
}

################################################################################
#
# sub: analyze_devdb
#
# Analyze devdb LT files.
#

sub analyze_devdb
{
    analyze_devdb_dir();
    calculate_initial_variant_ids();

    return 0;
}

################################################################################
#
# sub: show_info
#
# Show info.
#

sub show_info
{
    my $info = {
      max_field_per_table => $max_field_per_table,
      table_count => $table_count,
      variant_max => $variant_max,
      lt_count => $lt_count,
      info => \%info,
      variant_devdb => $variant_devdb,
      variant_changes => $variant_changes,
      sym_needs_preservation => $sym_needs_preservation,
      lt_initial_id => $lt_initial_id
    };

    print YAML::XS::Dump($info)."\n" or croak "Couldn't write: $OS_ERROR";

    return 0;
}

################################################################################
#
# sub: record_table_id
#
# Record Table ID in globals
#

sub record_table_id
{
    my ($device, $variant, $name, $table_id) = @_;

    croak "oops" if !defined $table_id;
    $lt_by_name->{$device}->{$variant}->{$name} = $table_id;

    return 0;
}

################################################################################
#
# sub: record_field_limits
#
# Record field limits in globals
#

sub record_field_limits
{
    my ($device, $variant, $field_data) = @_;
    my $field_count = scalar keys %{$field_data};

    if (!defined $max_field_per_table || $max_field_per_table < $field_count) {
        $max_field_per_table = $field_count;
    }

    return 0;
}

################################################################################
#
# sub: record_variant
#
# Record variant presence in variant_devdb
#

sub record_variant
{
    my ($device, $variant, $yenum) = @_;

    if (!defined $variant_devdb->{$device}) {
        $variant_devdb->{$device} = {};
    }

    $variant_devdb->{$device}->{$variant} = $yenum;

    return 0;
}

################################################################################
#
# sub: calculate_initial_table_ids
#
# Calculate table IDs based on devdb data present. This only uses devdb
# table counts.
#

sub calculate_initial_table_ids
{
    my $table_id = 0;

    $lt_initial_id->{common}->{BASE} = $table_id;
    $table_id += $lt_count->{'common'}->{'BASE'};

    foreach my $device (sort keys %{$lt_count}) {
        next if $device eq 'common';
        foreach my $variant (sort keys %{$lt_count->{$device}}) {
            $lt_initial_id->{$device}->{$variant} = $table_id;
            my $count = $lt_count->{$device}->{$variant};
            $table_id += $count;
        }
    }

    $table_count = $table_id;

    return 0;
}

################################################################################
#
# sub: calculate_initial_variant_ids
#
# Calculate variant IDs based on devdb data present. This only uses devdb
# YAML file existence from the initial devdb analysis.
#

sub calculate_initial_variant_ids
{
    my $variant_id = 1;
    my @devdb_yml;
    my %file_to_id;

    # collect devdb files
    foreach my $device (keys %info) {
        foreach my $variant (keys %{$info{$device}}) {
            push @devdb_yml, $info{$device}->{$variant}->{yml};
        }
    }

    # create a file->id hash from sort
    foreach my $file (ordered_files("variant", \@devdb_yml)) {
        $file_to_id{$file} = $variant_id++;
    }

    $variant_max = $variant_id - 1;
    # update calculated variant IDs, check for removals from fs
    foreach my $device (keys %info) {
        foreach my $variant (keys %{$info{$device}}) {
            my $file = $info{$device}->{$variant}->{yml};
            $info{$device}->{$variant}->{id} = $file_to_id{$file};
        }
    }

    return 0;
}

################################################################################
#
# sub: record_variant_change
#
# Record variant change operation
#

sub record_variant_change
{
    my ($device, $variant, $op) = @_;

    if (!defined $variant_changes->{$device}) {
        $variant_changes->{$device} = {};
    } else {
        my $current_op = $variant_changes->{$device}->{$variant};
        # consistency check - not expecting conflicting ops
        if (defined $current_op) {
            if ($current_op ne $op) {
                croak "Unexpected op: $device $variant $current_op -> $op";
            }
        }
    }

    $variant_changes->{$device}->{$variant} = $op;

    return 0;
}

################################################################################
#
# sub: analyze_variant_changes
#
# Determine variant changes from devdb analysis.
# Changes are recorded in $variant_changes.
#
# $info indicates which devices and variants are present on the filesystem
# to be integrated.
#
# $variant_devdb represents the DEVICE_VARIANT_T LTL enum variant
# information in the devdb files at the point that the particular
# devdb file was last integrated. Note that not every devdb file was
# necessarily integrated at the same time as any other devdb file, so
# each devdb must be treated independently. Variant change analysis
# facilitates updating the devdb files to match the integration state
# of the file system. Only after the devdb files are updated can they
# be used to regenerate any needed data.
#
# Variants are considered added if there are any variants for a
# device in $info that do not have a symbol in any
# $variant_devdb->{$device}->{BASE}.
#
# Variants are considered removed if there are any symbols in *any* variant
# of a device that is not present as $info{$device}->{$variant}.
#
# The reason for this asymmetry is that the devdb data reflects the
# implementation of variant composition, where variant data
# incorporates any device BASE data. In particular, the
# DEVICE_VARIANT_T LTL enum for a variant incorporates enum symbols
# for BASE, itself, and any other variants for the given device at the
# time of integration. This is simply a consequence of LTL enums and not
# a special property of DEVICE_VARIANT_T.
#

sub analyze_variant_changes
{
    foreach my $device (keys %info) {
        foreach my $variant (keys %{$info{$device}}) {
            croak "oops" if !defined $variant_devdb->{$device};
            croak "oops" if !defined $variant_devdb->{$device}->{BASE};
            if (!defined $variant_devdb->{$device}->{BASE}->{$variant}) {
                record_variant_change($device, $variant, 'add');
            }
        }
    }

    foreach my $device (keys %{$variant_devdb}) {
        foreach my $variant (keys %{$variant_devdb->{$device}}) {
            foreach my $sym (keys %{$variant_devdb->{$device}->{$variant}}) {
                if (!defined $info{$device}->{$sym}) {
                    record_variant_change($device, $sym, 'remove');
                }
            }
        }
    }

    return 0;
}

################################################################################
#
# sub: record_enum_edits
#
# Record data needed for enum edits
#
# (1) Preserve devdb DEVICE_VARIANT_T data
#
# (2) Determine if there are any DEVICE_VARIANT_T symbols that are
# also in any other ENUMs other than ENUM_NAME_T. If so, then they
# must be preserved in ENUM_NAME_T, otherwise the DEVICE_VARIANT_T
# symbols must be removed if the associated variant is no longer
# present.
#

sub record_enum_edits
{
    my ($device, $variant, $data) = @_;
    my %needs_preservation;
    my $dvt = $data->{DEVICE_VARIANT_T};
    my @dvt_keys = keys %{$dvt};

    croak "oops" if !defined $data;
    croak "oops" if !defined $data->{DEVICE_VARIANT_T};
    croak "oops" if !defined $data->{ENUM_NAME_T};

    foreach my $enum_type (keys %{$data}) {
        next if $enum_type eq "ENUM_NAME_T";
        next if $enum_type eq "DEVICE_VARIANT_T";
        foreach my $dvt_sym (@dvt_keys) {
            next if defined $needs_preservation{$dvt_sym};
            if (defined $data->{$enum_type}->{$dvt_sym}) {
                $needs_preservation{$dvt_sym} = $enum_type;
            }
        }
    }

    my $npk = scalar keys %needs_preservation;
    if ($npk > 0) {
        if (!defined $sym_needs_preservation->{$device}) {
            $sym_needs_preservation->{$device} = {};
        }
        $sym_needs_preservation->{$device}->{$variant} = \%needs_preservation;
    }

    return 0;
}

################################################################################
#
# sub: analyze_ids
#
# Analyze LT and VARIANT IDs.
#

sub analyze_ids
{
    my $bcmltd_id = "${sdk}/devdb/bcmltd_id.yml";
    croak "?${bcmltd_id}" if !-f $bcmltd_id;
    my $ydata;
    my $ytable;

    $lt_count = {};
    $lt_by_name = {};
    $lt_by_name->{common} = { BASE => {}};

    $ydata = YAML::XS::LoadFile($bcmltd_id);
    $ytable = $ydata->{BCMLTD_ID}->{TABLE};
    croak "oops" if !defined $ytable;
    $lt_count->{common}->{BASE} = scalar keys %{$ytable};
    foreach my $name (keys %{$ytable}) {
        my $table_data = $ytable->{$name};
        my $table_id = $table_data->{id};
        record_table_id('common','BASE',$name,$table_id);
        record_field_limits('common','BASE',$table_data->{FIELD});
    }

    foreach my $device (keys %info) {
        foreach my $variant (keys %{$info{$device}}) {
            my $yfile = $info{$device}->{$variant}->{yml};
            croak "Cannot locate $yfile" if !-f $yfile;
            $ydata = YAML::XS::LoadFile($yfile);

            my $ltd_id = $ydata->{BCMLTD_ID};
            my $ltd_enum;

            if ($variant eq 'BASE') {
                $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
            } else {
                $ltd_id = $ltd_id->{VARIANT}->{$device}->{$variant};
                $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
                $ytable = $ltd_id->{TABLE};

                if (defined $ytable) {
                    $lt_count->{$device}->{$variant} = scalar keys %{$ytable};
                } else {
                    $lt_count->{$device}->{$variant} = 0
                }

                foreach my $name (keys %{$ytable}) {
                    my $table_data = $ytable->{$name};
                    my $table_id = $table_data->{id};
                    my $table_name = "${device}_${variant}_${name}";
                    record_table_id($device,$variant,$table_name,$table_id);
                    record_field_limits($device,$variant,$table_data->{FIELD});
                }
            }
            record_enum_edits($device, $variant, $ltd_enum);
            record_variant($device, $variant, $ltd_enum->{DEVICE_VARIANT_T});
        }
    }
    calculate_initial_table_ids();

    return 0;
}

################################################################################
#
# sub: update_devdb_ltid_t
#
# Update devdb LTID_T data
#

sub update_devdb_ltid_t
{
    my ($device, $variant, $ydata) = @_;
    my $rc = 0;
    my $ytable;
    my $ltd_id = $ydata->{BCMLTD_ID};
    my $id = $lt_initial_id->{$device}->{$variant};
    my $ltd_enum;
    my $ltid_enum;

    croak "oops" if !defined $id;
    croak "oops" if $variant eq 'BASE';
    $ltd_id = $ltd_id->{VARIANT}->{$device}->{$variant};
    $ytable = $ltd_id->{TABLE};
    croak "oops $device $variant" if !defined $ytable;

    $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
    croak "oops $device $variant" if !defined $ltd_enum;
    $ltid_enum = $ltd_enum->{LTID_T};
    croak "oops $device $variant" if !defined $ltid_enum;

    # update table data
    foreach my $table (sort keys %{$ytable}) {
        croak "oops" if !defined $ytable->{$table}->{id};
        $ytable->{$table}->{id} = $id;
        # Not all tables have maps. Not desirable, but needs to be
        # tolerated.
        if (defined $ltid_enum->{$table}) {
            $ltid_enum->{$table} = $id;
        }
        $id++;
    }

    return $rc;
}

################################################################################
#
# sub: update_devdb_device_variant_t
#
# Update devdb DEVICE_VARIANT_T data
#

sub update_devdb_device_variant_t
{
    my ($device, $variant, $changes, $ydata) = @_;
    my $rc = 0;
    my $ltd_id = $ydata->{BCMLTD_ID};
    my $ltd_enum;
    my $dvt_enum;
    my $enum_action;

    if ($variant eq 'BASE') {
        $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
    } else {
        $ltd_id = $ltd_id->{VARIANT}->{$device}->{$variant};
        $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
    }

    $dvt_enum = $ltd_enum->{DEVICE_VARIANT_T};

    confess "oops" if !defined $ltd_enum;
    confess "oops" if !defined $dvt_enum;

    # Remove stale variants
    if (defined $changes) {
        foreach my $changed_variant (keys %{$changes}) {
            my $op = $changes->{$changed_variant};
            if ($op eq "remove") {
                delete $dvt_enum->{$changed_variant};
            }
        }
    }

    # Update remaining IDs
    foreach my $device_variant (keys %{$info{$device}}) {
        $dvt_enum->{$device_variant} = $info{$device}->{$device_variant}->{id};
    }

    return $rc;
}

################################################################################
#
# sub: preserve_enum_name_t_sym
#
# Return TRUE if the symbol needs to be preserved.
#

sub preserve_enum_name_t_sym
{
    my ($device, $variant) = @_;
    my $preserve = $sym_needs_preservation->{$device};
    my $rc = 0;

    return $rc;
}

################################################################################
#
# sub: update_devdb_enum_name_t
#
# Update ENUM_NAME_T data
#

sub update_devdb_enum_name_t
{
    my ($device, $variant, $changes, $ydata) = @_;
    my $rc = 0;
    my $ltd_id = $ydata->{BCMLTD_ID};
    my $ltd_enum;
    my $ent_enum;
    my $enum_action;
    my $id = 0;

    if ($variant eq 'BASE') {
        $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
    } else {
        $ltd_id = $ltd_id->{VARIANT}->{$device}->{$variant};
        $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
    }

    $ent_enum = $ltd_enum->{ENUM_NAME_T};
    croak "oops" if !defined $ent_enum;

    # implement changes
    foreach my $changed_variant (keys %{$changes}) {
        my $op = $changes->{$changed_variant};
        if ($op eq "remove") {
            if (!preserve_enum_name_t_sym($device, $changed_variant)) {
                delete $ent_enum->{$changed_variant};
            }
        } elsif ($op eq "add") {
            $ent_enum->{$changed_variant} = 0; # will be renumbered below
        } else {
            croak "Unexpected op $op";
        }
    }

    # renumber
    foreach my $name (sort keys %{$ent_enum}) {
        if ($name ne "INVALID_ENUM_NAME") {
            # preserve value for INVALID_ENUM_NAME
            $ent_enum->{$name} = $id++;
        }
    }

    return $rc;
}

################################################################################
#
# sub: update_new_devdb_body
#
# Update devdb file contents
#

sub update_new_devdb_body
{
    my ($IN, $file, $OUT, $outfile, $data) = @_;
    my $dir = dirname($file);
    my $state = 'scan';
    my $order;
    my $changed = 0;

    # Copy header comment to outfile
    while (<$IN>) {
        if ($_ =~ /^#/) {
            print {$OUT} $_
                or croak "Couldn't write ${outfile}: $OS_ERROR";
        } else {
            last;
        }
    }

    print {$OUT} YAML::XS::Dump($data)
        or croak "Couldn't write ${outfile}: $OS_ERROR";

    return 0;
}

################################################################################
#
# sub: update_new_devdb_file
#
# Update a new devdb file with recalculated data
#

sub update_new_devdb_file
{
    my ($device, $variant, $file, $ydata) = @_;
    my $rc = 0;
    open my $IN, '<', $file
        or croak "Couldn't open ${file}: $OS_ERROR";

    my ($OUT, $outfile) = File::Temp::tempfile($file.".XXXXXXXX");

    update_new_devdb_body($IN, $file, $OUT, $outfile, $ydata);

    close $IN or croak "Couldn't close ${file}: $OS_ERROR";
    close $OUT or croak "Couldn't close ${outfile}: $OS_ERROR";

    safe_update_file($file, $outfile);

    return $rc;
}

################################################################################
#
# sub: update_enumpool_file_body
#
# Update enumpool file contents
#

sub update_enumpool_file_body
{
    my ($IN, $file, $OUT, $outfile, $match_data) = @_;
    my $state = 'scan';
    my $replacement;
    my $checked = {};
    my $num_match = scalar keys %{$match_data};

    while (<$IN>) {
        my $line = $_;
        if ($state eq 'scan') {
            foreach my $sym (keys %{$match_data}) {
                if (!defined $checked->{$sym}) {
                    if ($line =~ /\[${sym}\]/) {
                        $state = 'replace';
                        $replacement = $match_data->{$sym};
                        $checked->{$sym} = 1;
                    }
                }
            }
        } elsif ($state eq 'replace') {
            print {$OUT} $replacement
                or croak "Couldn't write ${outfile}: $OS_ERROR";

            $state = 'skip';
        } elsif ($state eq 'skip') {
            if ($_ =~ /^};$/) {
                my $num_checked = 0;
                foreach my $sym (keys %{$checked}) {
                    if (defined $checked->{$sym}) {
                        $num_checked++;
                    }
                }
                if ($num_checked == $num_match) {
                    $state = 'copy';
                } else {
                    $state = 'scan';
                }
            }
        }

        if ($state ne 'skip') {
            print {$OUT} $line
                or croak "Couldn't write ${outfile}: $OS_ERROR";
        }
    }

    croak "Error updating $file: unexpected state $state." if $state ne 'copy';

    return 0;
}

################################################################################
#
# sub: enumpool_data
#
# Return enumpool data for a given type.
#
# Invert the hash so the numeric value is the key.
# Reinvert the hash on sorted value order, converting the new
# value to a  symbol.
#

sub enumpool_data
{
    my ($enum_data, $symbol) = @_;
    my %ival;
    my @data;
    my $str;

    foreach my $sym (keys %{$enum_data}) {
        my $value = $enum_data->{$sym};
        $ival{$value} = $sym;
    }

    foreach my $v (sort {$a <=> $b} keys %ival) {
        my $k = $ival{$v};
        my $vv = $v;

        if (defined $symbol) {
            $vv = "${symbol}_${k}";
        }

        push @data, "    { \"${k}\", $vv },";
    }

    $str = join "\n", @data;

    $str .= "\n";

    return $str;
}

################################################################################
#
# sub: device_variant_t_data
#
# Return DEVICE_VARIANT_T specific enumpool data.
#

sub device_variant_t_data
{
    my ($device, $enum_data) = @_;

    return enumpool_data($enum_data, "BCMLTD_VARIANT_${device}");
}

################################################################################
#
# sub: update_enumpool_file
#
# Update specific enum pool file
#

sub update_enumpool_file
{
    my ($device, $variant, $ydata) = @_;
    my $rc = 0;
    my $ldevice = lc $device;
    my $lvariant = lc $variant;
    my $file = "${sdk}/bcmlrd/chip/${ldevice}/";
    my $ltd_id = $ydata->{BCMLTD_ID};
    my $match_data;

    if ($variant eq 'BASE') {
        $file .= $ldevice;
        my $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
        my $dvt_enum = $ltd_enum->{DEVICE_VARIANT_T};
        my $ent_enum = $ltd_enum->{ENUM_NAME_T};
        my $dvt_data = device_variant_t_data($device, $dvt_enum);
        my $ent_data = enumpool_data($ent_enum);
        $match_data = {
            "${device}_LRD_DEVICE_VARIANT_T_ENUM_COUNT" => $dvt_data,
            "${device}_LRD_ENUM_NAME_T_ENUM_COUNT" => $ent_data,
        };
    } else {
        $file .= "${lvariant}/${ldevice}_${lvariant}";
        $ltd_id = $ltd_id->{VARIANT}->{$device}->{$variant};
        my $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
        my $ent_enum = $ltd_enum->{ENUM_NAME_T};
        my $ent_data = enumpool_data($ent_enum);

        $match_data = {
            "${device}_${variant}_LRD_ENUM_NAME_T_ENUM_COUNT" => $ent_data,
        };
    }

    $file .= "_lrd_enumpool.c";

    open my $IN, '<', $file
        or croak "Couldn't open ${file}: $OS_ERROR";

    my ($OUT, $outfile) = File::Temp::tempfile($file.".XXXXXXXX");

    update_enumpool_file_body($IN, $file, $OUT, $outfile, $match_data);

    close $IN or croak "Couldn't close ${file}: $OS_ERROR";
    close $OUT or croak "Couldn't close ${outfile}: $OS_ERROR";

    safe_update_file($file, $outfile);

    return $rc;
}

################################################################################
#
# sub: update_enum_limits_file_body
#
# Update enum limits file contents
#

sub update_enum_limits_file_body
{
    my ($IN, $file, $OUT, $outfile, $match_data) = @_;
    my $state = 'scan';
    my $num_match = scalar keys %{$match_data};
    my $checked = {};

    while (<$IN>) {
        my $line = $_;
        if ($state eq 'scan') {
            my $num_checked = 0;
            foreach my $sym (keys %{$match_data}) {
                if (!defined $checked->{$sym}) {
                    if ($line =~ /^(#define\s+${sym}\s+)(\w+)(.*)$/) {
                        my $value = $match_data->{$sym};
                        my $pre = $1;
                        # ignore current value $2;
                        my $post = $3;
                        $line = $pre.$value.$post."\n";
                        $checked->{$sym} = 1;
                    }
                }
                if (defined $checked->{$sym}) {
                    $num_checked++;
                }
            }
            if ($num_checked == $num_match) {
                $state = 'copy';
            }
        }

        print {$OUT} $line
            or croak "Couldn't write ${outfile}: $OS_ERROR";
    }

    croak "Error updating $file: unexpected state $state." if $state ne 'copy';

    return 0;
}

################################################################################
#
# sub: update_enum_limits_file
#
# Update specific enum limits file
#

sub update_enum_limits_file
{
    my ($device, $variant, $ydata) = @_;
    my $rc = 0;
    my $ldevice = lc $device;
    my $lvariant = lc $variant;
    my $file = "${sdk}/bcmlrd/include/bcmlrd/chip/${ldevice}/";
    my $ltd_id = $ydata->{BCMLTD_ID};
    my $match_data;

    if ($variant eq 'BASE') {
        $file .= $ldevice;
        my $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
        my $dvt_enum = $ltd_enum->{DEVICE_VARIANT_T};
        my $ent_enum = $ltd_enum->{ENUM_NAME_T};
        my $dvt_count = scalar keys %{$dvt_enum};
        my $ent_count = scalar keys %{$ent_enum};

        $match_data = {
            "${device}_LRD_DEVICE_VARIANT_T_ENUM_COUNT" => $dvt_count,
            "${device}_LRD_ENUM_NAME_T_ENUM_COUNT" => $ent_count,
        };
    } else {
        $file .= "${lvariant}/${ldevice}_${lvariant}";
        $ltd_id = $ltd_id->{VARIANT}->{$device}->{$variant};
        my $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
        my $ent_enum = $ltd_enum->{ENUM_NAME_T};
        my $ent_count = scalar keys %{$ent_enum};

        $match_data = {
            "${device}_${variant}_LRD_ENUM_NAME_T_ENUM_COUNT" => $ent_count,
        };
    }

    $file .= "_lrd_enum_limits.h";


    open my $IN, '<', $file
        or croak "Couldn't open ${file}: $OS_ERROR";

    my ($OUT, $outfile) = File::Temp::tempfile($file.".XXXXXXXX");

    update_enum_limits_file_body($IN, $file, $OUT, $outfile, $match_data);

    close $IN or croak "Couldn't close ${file}: $OS_ERROR";
    close $OUT or croak "Couldn't close ${outfile}: $OS_ERROR";

    safe_update_file($file, $outfile);

    return $rc;
}

################################################################################
#
# sub: update_strpool_file_body
#
# Update strpool file contents
#

sub update_strpool_file_body
{
    my ($IN, $file, $OUT, $outfile, $replacement) = @_;
    my $state = 'start';

    while (<$IN>) {
        my $line = $_;
        if ($state eq 'start') {
            if ($line =~ /#ifndef DOXYGEN_IGNORE_AUTOGEN/) {
                $state = 'scan';
            }
        } elsif ($state eq 'scan') {
            if ($line =~ /#ifndef.*s$/) {
                print {$OUT} $replacement
                    or croak "Couldn't write ${outfile}: $OS_ERROR";
                $state = 'skip';
            }
        } elsif ($state eq 'skip') {
            if ($_ =~ /DOXYGEN_IGNORE_AUTOGEN/) {
                $state = 'copy';
            }
        }

        if ($state ne 'skip') {
            print {$OUT} $line
                or croak "Couldn't write ${outfile}: $OS_ERROR";
        }
    }

    croak "Error updating $file: unexpected state $state." if $state ne 'copy';

    return 0;
}

################################################################################
#
# sub: map_strpool_data
#
# Convert ENUM_NAME_T symbols and table data to strpool data.
#

sub map_strpool_data
{
    my ($yenum, $ytable) = @_;
    my $str;
    my @data;
    my %strdata;

    # Convert enumname keys to upper case
    foreach my $str (keys %{$yenum}) {
        my $ustr = uc $str;
        $strdata{$ustr} = 1;
    }

    # Add table name and field definitions
    if (defined $ytable) {
        foreach my $table (keys %{$ytable}) {
            my $ustr = uc $table;
            $strdata{$ustr} = 1;
            foreach my $field (keys %{$ytable->{$table}->{FIELD}}) {
                $ustr = uc $field;
                $strdata{$ustr} = 1;
            }
        }
    }

    # Emit strpool data on uppercase sort
    foreach my $ustr (sort keys %strdata) {
        push @data, "#ifndef ${ustr}s";
        push @data, "#define ${ustr}s (\"${ustr}\")";
        push @data, "#endif";
    }
    push @data, "";

    $str = join "\n", @data;

    return $str;
}

################################################################################
#
# sub: update_strpool_file
#
# Update specific string pool file. strpool data simply contains
# string forms of ENUM_NAME_T symbols.
#

sub update_strpool_file
{
    my ($device, $variant, $changes, $ydata) = @_;
    my $rc = 0;
    my $ldevice = lc $device;
    my $lvariant = lc $variant;
    my $file = "${sdk}/bcmltd/include/bcmltd/str/${ldevice}/";
    my $ltd_id = $ydata->{BCMLTD_ID};
    my $ltd_enum;
    my $ent_enum;
    my $ltd_table;
    my $data;

    if ($variant eq 'BASE') {
        $file .= "${ldevice}";
        $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
    } else {
        $file .= "${lvariant}/${ldevice}_${lvariant}";
        $ltd_id = $ltd_id->{VARIANT}->{$device}->{$variant};
        $ltd_enum = $ltd_id->{MAP}->{$device}->{ENUM};
        $ltd_table = $ltd_id->{TABLE};
    }

    $file .= "_ltd_str.h";
    $ent_enum = $ltd_enum->{ENUM_NAME_T};
    $data = map_strpool_data($ent_enum, $ltd_table);

    open my $IN, '<', $file
        or croak "Couldn't open ${file}: $OS_ERROR";

    my ($OUT, $outfile) = File::Temp::tempfile($file.".XXXXXXXX");

    update_strpool_file_body($IN, $file, $OUT, $outfile, $data);

    close $IN or croak "Couldn't close ${file}: $OS_ERROR";
    close $OUT or croak "Couldn't close ${outfile}: $OS_ERROR";

    safe_update_file($file, $outfile);

    return $rc;
}

################################################################################
#
# sub: update_devdb_file
#
# Update specific devdb file and associated enum pool files
#

sub update_devdb_file
{
    my ($device, $variant, $file) = @_;
    my $changes = $variant_changes->{$device};
    my $ydata;
    my $rc = 0;

    # Read YAML file
    $ydata = YAML::XS::LoadFile($file);
    # Update LTID_T
    if ($variant ne 'BASE') {
        update_devdb_ltid_t($device, $variant, $ydata);
    }

    update_devdb_device_variant_t($device, $variant, $changes, $ydata);

    if (defined $changes) {
        # Update ENUM_NAME_T
        update_devdb_enum_name_t($device, $variant, $changes, $ydata);
        update_strpool_file($device, $variant, $changes, $ydata);
    }

    # Write updated YAML file
    update_new_devdb_file($device, $variant, $file, $ydata);
    # Update ...enumpool.c
    update_enumpool_file($device, $variant, $ydata);
    # Update ...enum_limits.h
    update_enum_limits_file($device, $variant, $ydata);

    return $rc;
}

################################################################################
#
# sub: update_devdb
#
# Update devdb
#

sub update_devdb
{
    my $rc = 0;

    foreach my $device (keys %info) {
        foreach my $variant (keys %{$info{$device}}) {
            $rc = update_devdb_file($device,
                                    $variant,
                                    $info{$device}->{$variant}->{yml});
        }
    }

    return $rc;
}

################################################################################
#
# sub: standard_grafts
#
# Perform standard graft operations on specified entities.
#

sub standard_grafts
{
    my $rc = 0;

    foreach my $file (@{$composable_headers}) {
        my $path = "${sdk}/${file}";
        $rc = update_composable_header($path);
        if ($rc) {
            return $rc;
        }
    }

    foreach my $section (@{$data_headers}) {
        $rc = update_data_header($section);
        if ($rc) {
            return $rc;
        }
    }

    $rc = update_devdb();

    return $rc;
}

################################################################################
#
# sub: graft
#
# Perform graft operation.
#

sub graft
{
    my $rc = 0;
    my $test_mode = 0;

    # analyze devdb
    analyze_devdb();
    analyze_ids();
    analyze_variant_changes();

    if (defined $opt{f}) {
        $test_mode = 1;
        $rc = update_composable_header($opt{f});
    }
    if (defined $opt{U}) {
        $test_mode = 1;
        $rc = update_data_header($opt{U});
    }

    if (!$test_mode) {
        $rc = standard_grafts();
    }

    if (defined $opt{i}) {
        show_info();
    }

    return $rc;
}

################################################################################
#
# sub: main
#
# main entry point
#

sub main
{
    my $rc;

    $rc = !Getopt::Std::getopts("C:U:f:hinv ", \%opt);

    if ($rc != 0 || defined $opt{h}) {
        HELP_MESSAGE();
    } else {
        $sdk = $opt{C} || $ENV{SDK};
        croak "No SDK directory specified." if !defined $sdk;
        croak "SDK directory '${sdk}' not found." if !-d $sdk;
        $sdk =~ s%/$%%; # strip trailing / if any
        $rc = graft();
    }

    return $rc;
}

exit main();
