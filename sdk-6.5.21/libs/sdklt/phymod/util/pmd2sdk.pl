#!/usr/local/bin/perl 

################################ N O T E S #####################################

=head1 Name: PMD2SDK. Make PMD API PHYMod friendly 

This program converts PMD API code delivered by the Serdes folks in Austin to a
version friendly for BRCM SDK consumption. It is assumed that PMD API code aka
PMD tier1 functions are only consumed by 

- PHYMod PHY driver framework post conversion.
- MT2 firmware (arm based compiler)

Please contact hoangngo@broadcom.com or ravick@broadcom.com  for more details.


=item 1. Copy over all the files to interim location.

=item 2. Now things are copied over. Build file lists to operate on. This is a
simple 'find' operation in the relevent directories. We create a catchall list,
a .c only list and a .h only list.

=item 3. Remove unwanted control (non -ASCII) chars in the copied over files.

=item 4. Fix doxygen annotations. We need to do this because we add an argument
phymod_access_t, which must be accounted for in the doxygen. 

=item 5. Run uncrustify on source to have functions and curlies on same line
 This maes it easy to search and replace function artuments later. Without this
 step, regex based search for functions is pretty much impossible.

=item 6. Now add the phymod_access_t struct in all functions, and function calls. 
As a first step add the 'pa' arg. to reg. macro functions. The PMD APIs have
reg. field based macros which are auto (script) mapped to specific set of
reg/mem access functions. The functions are stored in this script in the arrays
@regexArrayReg and @regexArrayUC

=item 7. Now find  all the Tier1 functions by scouring the C files.

=item 8. Insert the phymod_access_t argument in all the C files

=item 9. Insert the phymod_access_t argument in all the H files

=item 10.  Rename specific 'C' files so SDK won't complain. PMD folks include C files 
within C files to protect scoping (they have file scope static functions which
must be in the same file. Using include they can make it the same file. 
However SDK compilation will try to compile the C file and complain.

=cut

### END ######################## N O T E S #####################################

############################## G L O B A L S ###################################
$main::eagle = 0;
$main::falcon = 0;
$main::mt2 = 0;
$main::monterey = 0;
$skipCopy = 0;
$finalMv = 0;
$destDir = undef();

use vars qw/*name *dir *prune/;
*name   = *File::Find::name;
*dir    = *File::Find::dir;
*prune  = *File::Find::prune;

### the token YESDASH will be replaced by _eagle_tsc, _falcon_tsc, _falcon2_mt2 _falcon2_monterey
@regexArrayReg = qw(
YESDASH_pmd_rde_field_byte
YESDASH_pmd_rde_field_signed
YESDASH_pmd_rde_field_signed_byte
YESDASH_pmd_rde_reg
YESDASH_pmd_rde_field
YESDASH_pmd_mwr_reg_byte
NODASH_pmd_mwr_reg
NODASH_pmd_wr_reg
);

### the token NODASH will be replaced by eagle_tsc, falcon_tsc, falcon2_mt2 falcon2_monterey
@regexArrayUC = qw(
NODASH_rdbc_uc_var
NODASH_rdbl_uc_var
NODASH_rdwc_uc_var
NODASH_rdwl_uc_var
NODASH_wrbc_uc_var
NODASH_wrbl_uc_var
NODASH_wrwc_uc_var
NODASH_wrwl_uc_var
NODASH_rdwls_uc_var
NODASH_rdbls_uc_var
NODASH_wrbls_uc_var
);

$SUB_SUBST   = 0x1000;
$SUB_PREADD  = 0x2000;
$SUB_POSTADD = 0x4000;
$SUB_POSTNWL = 0x0001;
$SUB_PRENWL  = 0x0002;

### END ###################### G L O B A L S ###################################

############################# P A C K A G E S ##################################
use Pod::Usage;
use File::Find;
use File::Copy::Recursive qw (fcopy rcopy dircopy);
use File::Copy;
### END ##################### P A C K A G E S ##################################

########################## S U B  R O U T I N E S ##############################

## auto include the dir. from where the script was called
## even if the calling location is different.
BEGIN {
  (my $loc = $0) =~ s/(.*)\/mmm/$1/;
  push (@INC, $loc);
}

END {
  # temp file created.
  unlink ("/tmp/q$user") if (-f "/tmp/q$user");
}

sub Usage($)
{
  my $emsg = $_[0];
  $emsg = "ERROR: $emsg\n" if ($emsg);
  print "$emsg
USAGE:  pmd2sdk <args>

###################### G E N E R A L   O P T I O N S #####################
-h[e[l[p]]]    Don't execute. Display this help msg
-show          Don't execute. Show the command that would be executed
-eagle         only for eagle related files
-falcon        only for falcon related files
-mt2           only for mt2 related files
-all           All PMD files.
-debug|-dbg    More Debug msgs
-skipCopy      Do not copy over files (most likely already done)
-srcDir        Source Directory (relative to CWD or absolute)
-intDir        Destination Directory (relative to CWD or absolute)
-mv2destDir    Specify final dir. for transformed files and move them.
--             Ignore all succeeding options
";
exit;
}

sub wanted
{
  my ($dev,$ino,$mode,$nlink,$uid,$gid);
  /.*\.[ch]\z/s &&
    (($dev,$ino,$mode,$nlink,$uid,$gid) = lstat($_)) &&
    -f _ &&
      push (@filelist, $name);
}

# This function gets two regular expressions, a file, and an action hint.
# It searches for regex1 in the file and does one or more following actions
#$SUB_SUBST:   Do a direct substitution
#$SUB_PREADD:  Add regex2 before regex1
#$SUB_POSTADD: Add regex2 after regex1
#$SUB_POSTNWL: Add regex2 after regex1 and add a newline
#$SUB_PRENWL:  Add regex2 before regex1 and add a newline

sub substGenFile($$$$)
{
  my ($rgx1, $rgx2, $file,$bfrAftr) = ($_[0], $_[1], $_[2],$_[3]);
  my $tmpF = "$file.tmp.$$";
  open  (IFH, "$file")  || die ("C O r file:$file");
  open  (OFH, ">$tmpF") || die ("C O r tmpF:$tmpF");
  while (<IFH>) {
    $line = $_;
    if ($line =~ /$rgx1/) {
      my $prenewln  = ($bfrAftr & $SUB_PRENWL)  ? "\n" : "";
      my $postnewln = ($bfrAftr & $SUB_POSTNWL) ? "\n" : "";
      if      ($bfrAftr & $SUB_SUBST) {
        $line =~ s/$rgx1/$rgx2/;
      } elsif ($bfrAftr & $SUB_PREADD) {
        $line = $prenewln.$rgx2.$postnewln.$line;
      } elsif ($bfrAftr & $SUB_POSTADD) {
        ## assume $line has a new line...
        $line = $prenewln.$line.$rgx2.$postnewln;
      }
    }
    print (OFH $line);
  }
  close (IFH) || die ("C C r file:$file");
  close (OFH) || die ("C C r tmpF:$tmpF");
  move ($tmpF,$file);
  print("Successful regex replacements in file:$file\n");
}

#### this is for specific reg/uc macro replacements
sub substFunc($$$)
{
  my ($file,$type,@array)=($_[0],$_[1],@{$_[2]});
  for (@array) { s/YESDASH/_$type/; s/NODASH/$type/; };
  $tmpF="hfile"."$$";
  open  (IFH, "$file") || die ("C O r file:$file");
  open  (OFH, ">$tmpF") || die ("C O r tmpF:$tmpF");
  while (<IFH>) {
    $line = $_;
    foreach my $tmp (@array) {
      $line =~ s/$tmp\s*\(/$tmp\(pa, / if ($line =~ /$tmp/);
    }
    print (OFH $line);
  }
  close (IFH) || die ("C C r file:$file");
  close (OFH) || die ("C C r tmpF:$tmpF");
  move ($tmpF,$file);
  print ("Successful reg/uc macro changes in file:$file\n");
}

# cParse gets a C source filename and searches for functions to add the phymod
# context struct arg.  These functions are added to a hash. Actual addition of
# the pa arg will happen in cReplace and hReplace routines.  If the function is not
# used in a C file, it won't be in the hash

sub cParse($)
{
  my ($file) = ($_[0]);
  my ($vc,$nvc)=(0,0);
  open  (IFH, "$file")    || pmsg ('f', "C O r file:$file",2);
  print ("Open file $file for parsing\n");
  while (<IFH>) {
    if      ((/^\s*(\S+)\s*(\S+)\s*\(\s*void\s*\)\s*\{.*/) || # int fn(void)
             (/^\s*(\S+)\s*(\S+)\s*\(\s*\)\s*\{.*/))     {    # int fn()
      $voidfunc{$2} = 1;
    } elsif ((/^\s*(\S+)\s*(\S+)\s*(\S+)(\s*\(\s*void\s*\)\s*\{.*)/) || # static int fn(void)
             (/^\s*(\S+)\s*(\S+)\s*(\S+)(\s*\(\s*\)\s*\{.*)/))     { # static int fn()
      $voidfunc{$3} = 1;
    } elsif (/^\S+\s*(\S+)\s*\(.*\{.*/)         { # int foo (,,)
      $nvoidfunc{$1} = 1;
    } elsif (/^\S+\s+\S+\s+(\S+)(\s*\(\s*.*\s*\)\s*\{.*)/) { # static int foo (,,)
      $nvoidfunc{$1} = 1;
    }
  }
  close (IFH) || die ("C C r file:$file");

  # Add these extra functions from serdes_dependencies.h.  PMD API customer must
  # implement these functions, which are mandated by declaring in the header
  # files. Now, at the time of running this script, these functions are not
  # defined. It is added in a C file in SDK. Hence we won't pick them up by
  # looking for them in existing C files. 

  if ($eagle) {
    $nvoidfunc{"eagle_tsc_pmd_mwr_reg"}  = 1;
    $nvoidfunc{"eagle_tsc_pmd_rd_reg"}   = 1;
    $nvoidfunc{"eagle_tsc_pmd_rdt_reg"}  = 1;
    $nvoidfunc{"eagle_tsc_pmd_wr_reg"}   = 1;
    $nvoidfunc{"eagle_pmd_mdio_mwr_reg"} = 1;
    $voidfunc {"eagle_tsc_get_lane"}     = 1;
  }
  if ($falcon) {
    $nvoidfunc{"falcon_tsc_pmd_mwr_reg"}  = 1;
    $nvoidfunc{"falcon_tsc_pmd_rd_reg"}   = 1;
    $nvoidfunc{"falcon_tsc_pmd_rdt_reg"}  = 1;
    $nvoidfunc{"falcon_tsc_pmd_wr_reg"}   = 1;
    $nvoidfunc{"falcon_pmd_mdio_mwr_reg"} = 1;
    $voidfunc {"falcon_tsc_get_lane"}     = 1;
  }
  if ($mt2) {
    $nvoidfunc{"falcon2_mt2_pmd_mwr_reg"}      = 1;
    $nvoidfunc{"falcon2_mt2_pmd_rd_reg"}       = 1;
    $nvoidfunc{"falcon2_mt2_pmd_rdt_reg"}      = 1;
    $nvoidfunc{"falcon2_mt2_pmd_wr_reg"}       = 1;
    $nvoidfunc{"falcon2_mt2_pmd_mdio_mwr_reg"} = 1;
    $voidfunc {"falcon2_mt2_get_lane"}         = 1;
  }
  if ($monterey) {
    $nvoidfunc{"falcon2_monterey_pmd_mwr_reg"}      = 1;
    $nvoidfunc{"falcon2_monterey_pmd_rd_reg"}       = 1;
    $nvoidfunc{"falcon2_monterey_pmd_rdt_reg"}      = 1;
    $nvoidfunc{"falcon2_monterey_pmd_wr_reg"}       = 1;
    $nvoidfunc{"falcon2_monterey_pmd_mdio_mwr_reg"} = 1;
    $voidfunc {"falcon2_monterey_get_lane"}         = 1;
  }
  $vc  = scalar keys %voidfunc;
  $nvc = scalar keys %nvoidfunc;
  print ("Cumulative:: Void Func:$vc, NonVoid Func:$nvc. File::$file\n");
  if ($main::debug) {print "Voids:  " . join (" ", keys %voidfunc); print"\n";}
  if ($main::debug) {print "NVoids: " . join (" ", keys %nvoidfunc); print"\n";}
}

# Insert the phymod_access_t struct arg. in C files.
sub cReplace($)
{
  my ($file) = ($_[0]);
  my ($vc,$nvc) = (0,0);
  my $tmpF = "$file.tmp.$$";
  my @vfunc = keys %voidfunc;
  my @nvfunc = keys %nvoidfunc;

  open  (OFH, ">$tmpF") || die ("C O w file:$tmpF");
  open  (IFH, "$file")  || die ("C O r file:$file");
  while (<IFH>) {
    foreach my $nvf (@nvfunc) {
      if (/(.*)\b$nvf\s*\(.*\{/)     { $nvc++; #### } declaration
        if ($main::debug) {$file."<<<".$_;}
        $FatalErrorTwiceAdd = 1 if (/const phymod_access_t/);
        $_ =~ s/(.*)$nvf\s*\((.*\s*\{.*)/$1$nvf (const phymod_access_t *pa, $2/;#}
        print ("Arg added Twice $file,$nvf:$_\n") if($FatalErrorTwiceAdd);
        if ($main::debug) {$file.">>>".$_;}
      # it's possible the function is called multiple times on the same line.
      # Assume no more than twice. So here, we'll just check to see if
      # function is called twice.
      } elsif (/.*$nvf\s*\(.*$nvf\s*\(.*/) { $nvc++; #### invocation
        if ($main::debug) {$file."nvf <<<".$_;}
        $_ =~ s/(.*)$nvf\s*\((.*)$nvf\s*\((.*)/$1$nvf (pa, $2$nvf (pa, $3/;
        print("Arg added twice. $file:$nvf:$_\n")if(/\(\s*pa\s*,\s*pa\s*,/);
      } elsif (/(.*)$nvf\s*\(.*/) { $nvc++; #### invocation
        if ($main::debug) {$file."nvf <<<".$_;}
        $_ =~ s/(.*)$nvf\s*\((.*)/$1$nvf (pa, $2/;
        if ($main::debug) {$file."nvf >>>".$_;}
        if ($main::debug) {$file."nvf >>>".$_;}
      }
    }
    foreach my $vf (@vfunc) {
      if (/(.*)\b$vf\s*\(.*{/)     { $vc++ ; #### }decln
        if ($main::debug) {print $file."<<<".$_;}
        print ("$file:$_. Arg added twice\n") if (/phymod_access_t/);
        $_ =~ s/(.*)$vf\s*\((.*\s*{.*)/$1$vf (const phymod_access_t *pa $2/;#}
        $_ =~ s,\s*void,,;
        if ($main::debug) {print $file.">>>".$_;}
      } elsif (/(.*)$vf\s*\(.*/) { $vc++ ; #### usage
        if ($main::debug) {print $file."<<<".$_;}
        $_ =~ s/(.*)$vf\s*\((.*)/$1$vf (pa $2/;
        $_ =~ s,\s*void,,;
        print("Arg added twice. $file:$vf:$_\n")if(/\(\s*pa\s*,\s*pa\s*,/);
        if ($main::debug) {print $file.">>>".$_;}
      }
    }
    print OFH $_;
  }
  close (IFH) || die ("C C r file:$file");
  close (OFH) || die ("C C w file:$tmpF");
  move ($tmpF,$file);
  print ("Replaced in cfile:$file :: Void Func:$vc, NonVoid Func:$nvc\n");
}

# Insert the phymod_access_t struct arg. in h files.
sub hReplace($)
{
  my ($file) = ($_[0]);
  my ($vc,$nvc) = (0,0);
  my $tmpF = "$file.tmp.$$";
  my @vfunc = keys %voidfunc;
  my @nvfunc = keys %nvoidfunc;

  open  (OFH, ">$tmpF")|| die ("C O w file:$tmpF");
  open  (IFH, "$file") || die ("C O r file:$file");
  while (<IFH>) {
    foreach my $vf (@vfunc) {
      if (/$vf\s*\(.*/) { $vc++;#### prototype decl
        if ($main::debug) {print "vf <<<".$_;}
        $_ =~ s/(.*)$vf\s*\((.*\s*;.*)/$1$vf (const phymod_access_t *pa $2/;
        $_ =~ s,\s*void,,;
        if ($main::debug) {print "vf >>>".$_;}
      }
    }
    foreach my $nvf (@nvfunc) {
      if (/$nvf\s*\(.*/) { $nvc++; #### prototype decl
        if ($main::debug) {print "nvf <<<".$_;}
        $_ =~ s/(.*)$nvf\s*\((.*\s*;.*)/$1$nvf (const phymod_access_t *pa, $2/;
        if ($main::debug) {print "nvf >>>".$_;}
      }
    }
    print OFH $_;
  }
  close (IFH) || die ("C C r file:$file");
  close (OFH) || die ("C C w file:$tmpF");
  move ($tmpF,$file);
  print ("Replaced in hfile:$file :: Void Func:$vc, NonVoid Func:$nvc\n");
}

### END ################## S U B  R O U T I N E S ##############################

################################## M A I N #####################################

$ARGC = $#ARGV;
&Usage("Must supply one or more args") if ($ARGC < 1);

for ($i=0; $i<$ARGC;$i++) {
  if      ($ARGV[$i] =~ /-h(|help)$/i)   { &Usage("");
  } elsif ($ARGV[$i] =~ /--$/)           { last;         # ignore args
  } elsif ($ARGV[$i] =~ /-show$/)        { $show   = 1;  # only show cmds
  } elsif ($ARGV[$i] =~ /-eagle2$/i)     { $eagle2  = 1;  # Eagle2 PMD Only
  } elsif ($ARGV[$i] =~ /-eagle$/i)      { $eagle  = 1;  # Eagle PMD Only
  } elsif ($ARGV[$i] =~ /-falcon$/i)     { $falcon = 1;  #  Falcon PMD Only
  } elsif ($ARGV[$i] =~ /-mt2$/i)        { $mt2    = 1;  # MT2 PMD only
  } elsif ($ARGV[$i] =~ /-monterey$/i)   { $monterey = 1;  # MONTEREY PMD only
  } elsif ($ARGV[$i] =~ /-(debug)$/i)    { $debug  = 1;  # debug on
  } elsif ($ARGV[$i] =~ /-srcDir$/i)     { $srcDir = $ARGV[++$i];
  } elsif ($ARGV[$i] =~ /-intDir$/i)     { $intDir= $ARGV[++$i];
  } elsif ($ARGV[$i] =~ /-skipcopy$/i)   { $skipCopy= 1;
  } elsif ($ARGV[$i] =~ /-mv2destDir$/i) { $destDir= $ARGV[++$i];$finalMv=1;
  } elsif ($ARGV[$i] =~ /-all$/i)        { $mt2 = 1; $eagle = 1; $falcon = 1; $eagle2 = 1;
  } else                                 { Usage("Bad Arg $ARGV[$i]");
  }
}

$uniqDir    = "$srcDir/uniquify";
$CFMT       = "/home/hoangngo/bin/cformat.pl";
$DOXY       = "/home/hoangngo/bin/perl/doxyfix.pl";
$UNCRUSTIFY="/tools/bin/uncrustify -c /projects/ntsw-sw7/home/hoang/uncrustify-cfg/linux.cfg --no-backup ";


if ($finalMv) {
  die("Cannot find FinalDir:$destDir") unless(-d $destDir);
  die("Bad FinalDir:$destDir/tier1")   unless($destDir =~ /tier1/);
  die("Interim Dir doesn't exists:$intDir") if (!-d $intDir);
} else {
  die ("Cannot find Uniquify Dir:$uniqDir") unless (-d $uniqDir);
  die ("Interim Dir exists:$intDir") if (-e $intDir);
}

# sources of the APIs (C and header files uniquified by PMD folks)
$eagleSSrc  = "$uniqDir/eagle_tsc_src";
$eagleSInc  = "$uniqDir/eagle_tsc_include";
$eagle2SSrc  = "$uniqDir/eagle2_tsc2pll_src";
$eagle2SInc  = "$uniqDir/eagle2_tsc2pll_include";
#$falconSSrc = "$uniqDir/falcon_tsc_src";
#$falconSInc = "$uniqDir/falcon_tsc_include";
$falconSSrc = "$uniqDir/falcon_tsc_tdt_src";
$falconSInc = "$uniqDir/falcon_tsc_tdt_include";
$falmt2SSrc = "$uniqDir/falcon2_mt2_src";
$falmt2SInc = "$uniqDir/falcon2_mt2_include";
$falmonSSrc = "$uniqDir/falcon2_monterey_src";
$falmonSInc = "$uniqDir/falcon2_monterey_include";

# interim destinations of the APIs (C and header files)
# Note, the 'api' dir. is removed here. 
$eagleDSrc  = "$intDir/eagle_tsc_src";
$eagleDInc  = "$intDir/eagle_tsc_include";
$eagle2DSrc  = "$intDir/eagle2_tsc2pll_src";
$eagle2DInc  = "$intDir/eagle2_tsc2pll_include";
$falconDSrc = "$intDir/falcon_tsc_src";
$falconDInc = "$intDir/falcon_tsc_include";
$falmt2DSrc = "$intDir/falcon2_mt2_src";
$falmt2DInc = "$intDir/falcon2_mt2_include";
$falmonDSrc = "$intDir/falcon2_monterey_src";
$falmonDInc = "$intDir/falcon2_monterey_include";

if ($finalMv) {
  print ("Beginning final move. Eagle:$eagle, Falcon:$falcon, Fal2:$mt2\n");
  if ($eagle) {
    # There are a bunch of other files which are added in SDK. But this file 
    # is a PMD delivered file hand edited for SDK. Hence save it.
    system ("mv $destDir/eagle_tsc_usr_includes.h $destDir/eagle_tsc_usr_includes.xh");
    system ("mkdir -p $destDir/{common,public}");
    system ("cp -pr $eagleDInc/common/* $destDir/common/");
    system ("cp -pr $eagleDInc/public/* $destDir/public/");
    system ("cp -pr $eagleDInc/*.* $destDir/");
    system ("cp -pr $eagleDSrc/*.* $destDir/");
    ## restore this file.
    system ("mv $destDir/eagle_tsc_usr_includes.xh $destDir/eagle_tsc_usr_includes.h");
  }
  if ($eagle2) {
    # There are a bunch of other files which are added in SDK. But this file 
    # is a PMD delivered file hand edited for SDK. Hence save it.
    system ("mv $destDir/eagle_tsc_usr_includes.h $destDir/eagle_tsc_usr_includes.xh");
    system ("mkdir -p $destDir/{common,public}");
    system ("cp -pr $eagle2DInc/common/* $destDir/common/");
    system ("cp -pr $eagle2DInc/public/* $destDir/public/");
    system ("cp -pr $eagle2DInc/*.* $destDir/");
    system ("cp -pr $eagle2DSrc/*.* $destDir/");
    ## restore this file.
    system ("mv $destDir/eagle2_tsc2pll_usr_includes.xh $destDir/eagle2_tsc2pll_usr_includes.h");
  }
  if ($falcon) {
    system ("mv $destDir/falcon_tsc_usr_includes.h $destDir/falcon_tsc_usr_includes.tmp");
    system ("mkdir -p $destDir/{common,public}");
    system ("cp -pr $falconDInc/common/* $destDir/common/");
    system ("cp -pr $falconDInc/public/* $destDir/public/");
    system ("cp -pr $falconDInc/* $destDir/");
    system ("cp -pr $falconDSrc/* $destDir/");
    system ("mv $destDir/falcon_tsc_usr_includes.tmp $destDir/falcon_tsc_usr_includes.h");
  }
  if ($monterey) {
    system ("mv $destDir/falcon2_monterey_includes.h $destDir/falcon2_montereyusr_includes.tmp");
    system ("mkdir -p $destDir/{common,public}");
    system ("cp -pr $falmonDInc/common/* $destDir/common/");
    system ("cp -pr $falmonDInc/public/* $destDir/public/");
    system ("cp -pr $falmonDInc/* $destDir/");
    system ("cp -pr $falmonDSrc/* $destDir/");
   }
  exit (0);
}

## 1. Copy over all the files to interim location.
if (!$skipCopy) {
  if (!-d $intDir) {
    mkdir ($intDir) || pmsg ('f', "Cannot make interim dir: $intDir", 2);
    print ("Making Dest. Dir:$intDir\n");
  } else {
    print ("Dir. exists (so no need to create):$intDir");
  }
  if ($eagle) {
    print "Copying Eagle files\n";
    unlink qw ($eagleDSrc $eagleDInc);
    dircopy ($eagleSSrc, $eagleDSrc);
    dircopy ($eagleSInc, $eagleDInc);
    unlink qw($eagleDSrc/.svn $eagleDInc/.svn $eagleDInc/common/.svn $eagleDInc/public/.svn);
  }
  if ($eagle2) {
    print "Copying Eagle2 files\n";
    unlink qw ($eagle2DSrc $eagle2DInc);
    dircopy ($eagle2SSrc, $eagle2DSrc);
    dircopy ($eagle2SInc, $eagle2DInc);
    unlink qw($eagle2DSrc/.svn $eagle2DInc/.svn $eagle2DInc/common/.svn $eagle2DInc/public/.svn);
  }
  if ($falcon) {
    unlink qw ($falconDSrc $falconDInc);
    dircopy ($falconSSrc, $falconDSrc);
    dircopy ($falconSInc, $falconDInc);
    unlink qw($falconDSrc/.svn $falconDInc/.svn $falconDInc/common/.svn $falconDInc/public/.svn);
  }
  if ($mt2) {
    unlink qw ($falmt2DSrc $falmt2DInc);
    dircopy ($falmt2SSrc, $falmt2DSrc);
    dircopy ($falmt2SInc, $falmt2DInc);
    unlink qw($falmt2DSrc/.svn $falmt2DInc/.svn $falmt2DInc/common/.svn $falmt2DInc/public/.svn);
  }
  if ($monterey) {
    unlink qw ($falmonDSrc $falmonDInc);
    dircopy ($falmonSSrc, $falmonDSrc);
    dircopy ($falmonSInc, $falmonDInc);
    unlink qw($falmonDSrc/.svn $falmonDInc/.svn $falmonDInc/common/.svn $falmonDInc/public/.svn);
  }
}

#pmsg ("Change Dir to dir:$intDir");
#chdir ($intDir) || pmsg ('f',  "Cannot chdir to $intDir", 2);

## 2. Now things are copied over. Build file lists to operate on
@allFiles = undef();
if ($eagle) {
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $eagleDInc);
  push (@e_filelist, @filelist);
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $eagleDSrc);
  push (@e_filelist, @filelist);
  @e_filelist = grep { /\.[ch]/ } @e_filelist;
  @e_filelist_h = grep { /\.h/ } @e_filelist;
  @e_filelist_c = grep { /\.c/ } @e_filelist;
  push (@allFiles, @e_filelist);
  push (@allFiles_c, @e_filelist_c);
  push (@allFiles_h, @e_filelist_h);
  if ($main::debug) {print (join ("\n#### ", @e_filelist));}
}

if ($eagle2) {
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $eagle2DInc);
  push (@e_filelist, @filelist);
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $eagle2DSrc);
  push (@e_filelist, @filelist);
  @e_filelist = grep { /\.[ch]/ } @e_filelist;
  @e_filelist_h = grep { /\.h/ } @e_filelist;
  @e_filelist_c = grep { /\.c/ } @e_filelist;
  push (@allFiles, @e_filelist);
  push (@allFiles_c, @e_filelist_c);
  push (@allFiles_h, @e_filelist_h);
  if ($main::debug) {print (join ("\n#### ", @e_filelist));}
}

if ($falcon) {
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $falconDInc);
  push (@f_filelist, @filelist);
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $falconDSrc);
  push (@f_filelist, @filelist);
  @f_filelist = grep { /\.[ch]/ } @f_filelist;
  @f_filelist_h = grep { /\.h/ } @f_filelist;
  @f_filelist_c = grep { /\.c/ } @f_filelist;
  push (@allFiles, @f_filelist);
  push (@allFiles_c, @f_filelist_c);
  push (@allFiles_h, @f_filelist_h);
  if ($main::debug) {print join ("\n## ", @f_filelist);}
}

if ($mt2) {
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $falmt2DInc);
  push (@mt2_filelist, @filelist);
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $falmt2DSrc);
  push (@mt2_filelist, @filelist);
  @mt2_filelist = grep { /\.[ch]/ } @mt2_filelist;
  @mt2_filelist_h = grep { /\.h/ } @mt2_filelist;
  @mt2_filelist_c = grep { /\.c/ } @mt2_filelist;
  push (@allFiles, @mt2_filelist);
  push (@allFiles_c, @mt2_filelist_c);
  push (@allFiles_h, @mt2_filelist_h);
  if ($main::debug) {print join ("\n#### ", @f_filelist);}
}

if ($monterey) {
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $falmonDInc);
  push (@monterey_filelist, @filelist);
  @filelist = undef();
  File::Find::find({wanted => \&wanted}, $falmonDSrc);
  push (@monterey_filelist, @filelist);
  @monterey_filelist = grep { /\.[ch]/ } @monterey_filelist;
  @monterey_filelist_h = grep { /\.h/ } @monterey_filelist;
  @monterey_filelist_c = grep { /\.c/ } @monterey_filelist;
  push (@allFiles, @monterey_filelist);
  push (@allFiles_c, @monterey_filelist_c);
  push (@allFiles_h, @monterey_filelist_h);
  if ($main::debug) {print join ("\n#### ", @f_filelist);}
}




@allFiles   = grep { /\.[ch]/ } @allFiles;
@allFiles_c = grep { /\.c/ }    @allFiles_c;
@allFiles_h = grep { /\.h/ }    @allFiles_h;

# 3. Remove unwanted control (non -ASCII) chars in the copied over files.
system ("$CFMT -f @allFiles");

# 4. Fix doxygen annotations
system ("$DOXY -f @allFiles");

# 5. Runs uncrustify on source to have functions and curlies on same line
# Needed to search and replace function artuments later...
foreach my $tmp (@allFiles) {
  next if ( $tmp eq "" || !defined $tmp );
  system("$UNCRUSTIFY $tmp");
}
print ("Successfully uncrustified all files");

## Add the 'pa' arg. to reg. macro functions.
## Macros wont be caught by regex searches in hReplace.
if ($eagle) {
  substFunc ("$eagleDInc/eagle_tsc_fields.h","eagle_tsc", \@regexArrayReg);
  substFunc ("$eagleDInc/eagle_merlin_api_uc_vars_rdwr_defns.h", "eagle_tsc", \@regexArrayUC);
  substGenFile("\\s*#define EAGLE_TSC_FIELDS_H", "#define __ERR &__err",
     "$eagleDInc/eagle_tsc_fields.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define SRDS_API_UC_COMMON_H", "#include <phymod/phymod.h>",
     "$eagleDInc/common/srds_api_uc_common.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define EAGLE_TSC_API_DEPENDENCIES_H", "#include <phymod/phymod.h>",
     "$eagleDInc/eagle_tsc_dependencies.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define EAGLE_TSC_API_FUNCTIONS_H", "#include <phymod/phymod.h>",
     "$eagleDInc/eagle_tsc_functions.h", ($SUB_POSTADD|$SUB_POSTNWL));

  substGenFile("eagle_tsc_field_access.c","eagle_tsc_field_access_c.h",
                "$eagleDSrc/eagle_tsc_functions.c",$SUB_SUBST);
  substGenFile("eagle_tsc_internal.c"    ,"eagle_tsc_internal_c.h"    ,
                "$eagleDSrc/eagle_tsc_functions.c",$SUB_SUBST);
  substGenFile("eagle_pll_config.c"      ,"eagle_tsc_pll_config_c.h"  ,
                "$eagleDSrc/eagle_tsc_functions.c",$SUB_SUBST);
  substGenFile("eagle_tsc_dv_functions.c","eagle_tsc_dv_functions_c.h",
                "$eagleDSrc/eagle_tsc_functions.c",$SUB_SUBST);
}

if ($eagle2) {
  substFunc ("$eagle2DInc/eagle2_tsc2pll_fields.h","eagle2_tsc2pll", \@regexArrayReg);
  substFunc ("$eagle2DInc/eagle_merlin_api_uc_vars_rdwr_defns.h", "eagle2_tsc2pll", \@regexArrayUC);
  substGenFile("\\s*#define EAGLE2_TSC2PLL_FIELDS_H", "#define __ERR &__err",
     "$eagle2DInc/eagle2_tsc2pll_fields.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define SRDS_API_UC_COMMON_H", "#include <phymod/phymod.h>",
     "$eagle2DInc/common/srds_api_uc_common.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define EAGLE2_TSC2PLL_API_DEPENDENCIES_H", "#include <phymod/phymod.h>",
     "$eagle2DInc/eagle2_tsc2pll_dependencies.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define EAGLE2_TSC2PLL_API_FUNCTIONS_H", "#include <phymod/phymod.h>",
     "$eagle2DInc/eagle2_tsc2pll_functions.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("eagle2_tsc2pll_field_access.c","eagle2_tsc2pll_field_access_c.h",
                "$eagle2DSrc/eagle2_tsc2pll_functions.c",$SUB_SUBST);
  substGenFile("eagle2_tsc2pll_internal.c"    ,"eagle2_tsc2pll_internal_c.h"    ,
                "$eagle2DSrc/eagle2_tsc2pll_functions.c",$SUB_SUBST);
  substGenFile("eagle2_tsc2pll_pll_config.c"      ,"eagle2_tsc2pll_pll_config_c.h"  ,
                "$eagle2DSrc/eagle2_tsc2pll_functions.c",$SUB_SUBST);
  substGenFile("eagle2_tsc2pll_dv_functions.c","eagle2_tsc2pll_dv_functions_c.h",
                "$eagle2DSrc/eagle2_tsc2pll_functions.c",$SUB_SUBST);
}

if ($falcon) {
  substFunc ("$falconDInc/falcon_tsc_fields.h","falcon_tsc", \@regexArrayReg);
  substFunc ("$falconDInc/falcon_api_uc_vars_rdwr_defns.h", "falcon_tsc", \@regexArrayUC);
  substGenFile("\\s*#define FALCON_TSC_FIELDS_H", "#define __ERR &__err",
    "$falconDInc/falcon_tsc_fields.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define SRDS_API_UC_COMMON_H", "#include <phymod/phymod.h>",
    "$falconDInc/common/srds_api_uc_common.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define FALCON_TSC_API_DEPENDENCIES_H", "#include <phymod/phymod.h>",
    "$falconDInc/falcon_tsc_dependencies.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define FALCON_TSC_API_FUNCTIONS_H", "#include <phymod/phymod.h>",
    "$falconDInc/falcon_tsc_functions.h", ($SUB_POSTADD|$SUB_POSTNWL));

  substGenFile("falcon_tsc_field_access.c","falcon_tsc_field_access_c.h",
                "$falconDSrc/falcon_tsc_functions.c",$SUB_SUBST);
  substGenFile("falcon_tsc_internal.c"    ,"falcon_tsc_internal_c.h"    ,
                "$falconDSrc/falcon_tsc_functions.c",$SUB_SUBST);
  substGenFile("falcon_tsc_pll_config.c"  ,"falcon_tsc_pll_config_c.h"  ,
                "$falconDSrc/falcon_tsc_functions.c",$SUB_SUBST);
  substGenFile("falcon_tsc_dv_functions.c","falcon_tsc_dv_functions_c.h",
                "$falconDSrc/falcon_tsc_functions.c",$SUB_SUBST);
}

if ($mt2) {
  substFunc("$falmt2DInc/falcon2_mt2_fields.h","falcon2_mt2", \@regexArrayReg);
  substFunc("$falmt2DInc/falcon_api_uc_vars_rdwr_defns.h", "falcon2_mt2", \@regexArrayUC);
  substGenFile("\\s*#define FALCON2_MT2_FIELDS_H", "#define __ERR &__err",
    "$falmt2DInc/falcon2_mt2_fields.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define SRDS_API_UC_COMMON_H", "#include <phymod/phymod.h>",
    "$falmt2DInc/common/srds_api_uc_common.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define FALCON2_MT2_API_DEPENDENCIES_H", "#include <phymod/phymod.h>",
    "$falmt2DInc/falcon2_mt2_dependencies.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define FALCON2_MT2_API_DEPENDENCIES_H", "#include <phymod/phymod.h>",
    "$falmt2DInc/falcon2_mt2_functions.h", ($SUB_POSTADD|$SUB_POSTNWL));

  substGenFile("falcon2_mt2_field_access.c","falcon2_mt2_field_access_c.h",
                "$falmt2DSrc/falcon2_mt2_functions.c",$SUB_SUBST);
  substGenFile("falcon2_mt2_internal.c"    ,"falcon2_mt2_internal_c.h"    ,
                "$falmt2DSrc/falcon2_mt2_functions.c",$SUB_SUBST);
  substGenFile("falcon_tsc_pll_config.c"   ,"falcon_tsc_pll_config_c.h"  ,
                "$falmt2DSrc/falcon2_mt2_functions.c",$SUB_SUBST);
  substGenFile("falcon_tsc_dv_functions.c","falcon_tsc_dv_functions_c.h",
                "$falmt2DSrc/falcon2_mt2_functions.c",$SUB_SUBST);
}

if ($monterey) {
  substFunc ("$falmonDInc/falcon2_monterey_fields.h","falcon2_monterey", \@regexArrayReg);
  substFunc ("$falmonDInc/falcon_api_uc_vars_rdwr_defns.h", "falcon2_monterey", \@regexArrayUC);
  substGenFile("\\s*#define FALCON2_MONTEREY_FIELDS_H", "#define __ERR &__err",
    "$falmonDInc/falcon2_monterey_fields.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define SRDS_API_UC_COMMON_H", "#include <phymod/phymod.h>",
    "$falmonDInc/common/srds_api_uc_common.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define FALCON2_MONTEREY_API_DEPENDENCIES_H", "#include <phymod/phymod.h>",
    "$falmonDInc/falcon2_monterey_dependencies.h", ($SUB_POSTADD|$SUB_POSTNWL));
  substGenFile("\\s*#define FALCON2_MONTEREY_API_FUNCTIONS_H", "#include <phymod/phymod.h>",
    "$falmonDInc/falcon2_monterey_functions.h", ($SUB_POSTADD|$SUB_POSTNWL));

  substGenFile("falcon2_monterey_field_access.c","falcon2_monterey_field_access_c.h",
                "$falmonDSrc/falcon2_monterey_functions.c",$SUB_SUBST);
  substGenFile("falcon2_monterey_internal.c"    ,"falcon2_monterey_internal_c.h"    ,
                "$falmonDSrc/falcon2_monterey_functions.c",$SUB_SUBST);
  substGenFile("falcon2_monterey_pll_config.c"  ,"falcon2_monterey_pll_config_c.h"  ,
                "$falmonDSrc/falcon2_monterey_functions.c",$SUB_SUBST);
  substGenFile("falcon2_monterey_dv_functions.c","falcon_monterey_dv_functions_c.h",
                "$falmonDSrc/falcon2_monterey_functions.c",$SUB_SUBST);

}


foreach my $cfile (@allFiles_c) { cParse($cfile); }
foreach my $cfile (@allFiles_c) { cReplace($cfile); }
foreach my $hfile (@allFiles_h) { hReplace($hfile); }

# Rename the files so SDK won't complain. PMD API folks include c files into
# other C files to protect scoping (they have file scope static functions which
# must be in the same file. Using #include they can make it the same file. 
# However SDK compilation will try to compile the C file and 
if ($eagle) {
  system("mv $eagleDSrc/eagle_tsc_internal.c $eagleDSrc/eagle_tsc_internal_c.h");
  system("mv $eagleDSrc/eagle_tsc_field_access.c  $eagleDSrc/eagle_tsc_field_access_c.h");
  system("mv $eagleDSrc/eagle_pll_config.c $eagleDSrc/eagle_tsc_pll_config_c.h")  ;
  system("mv $eagleDSrc/eagle_tsc_dv_functions.c $eagleDSrc/eagle_tsc_dv_functions_c.h");
}

if ($eagle2) {
  system("mv $eagle2DSrc/eagle2_tsc2pll_internal.c $eagle2DSrc/eagle2_tsc2pll_internal_c.h");
  system("mv $eagle2DSrc/eagle2_tsc2pll_field_access.c  $eagle2DSrc/eagle2_tsc2pll_field_access_c.h");
  system("mv $eagle2DSrc/eagle2_tsc2pll_pll_config.c $eagle2DSrc/eagle2_tsc2pll_pll_config_c.h")  ;
  system("mv $eagle2DSrc/eagle2_tsc2pll_dv_functions.c $eagle2DSrc/eagle2_tsc2pll_dv_functions_c.h");
}

if ($falcon) {
  # Rename the files so SDK won't complain
  move($falconDSrc/falcon_tsc_internal.c, $falconDSrc/falcon_tsc_internal_c.h);
  move($falconDSrc/falcon_tsc_field_access.c, $falconDSrc/falcon_tsc_field_access_c.h);
  move($falconDSrc/falcon_tsc_pll_config.c, $falconDSrc/falcon_tsc_pll_config_c.h)  ;
  move($falconDSrc/falcon_tsc_dv_functions.c, $falconDSrc/falcon_tsc_dv_functions_c.h);
}

if ($mt2) {
  # Rename the files so SDK won't complain
  move($falmt2DSrc/falcon2_mt2_internal.c, $falmt2DSrc/falcon2_mt2_internal_c.h)    ;
  move($falmt2DSrc/falcon2_mt2_field_access.c, $falmt2DSrc/falcon2_mt2_field_access_c.h);
  move($falmt2DSrc/falcon2_mt2_pll_config.c, $falmt2DSrc/falcon2_mt2_pll_config_c.h)  ;
  move($falmt2DSrc/falcon2_mt2_dv_functions.c, $falmt2DSrc/falcon2_mt2_dv_functions_c.h);
}
if ($monterey) {
  # Rename the files so SDK won't complain
system("mv $falmonDSrc/falcon2_monterey_dv_functions.c $falmonDSrc/falcon2_monterey_dv_functions_c.h\n");
#move($falmonDSrc/falcon2_monterey_dv_functions.c, $falmonDSrc/falcon2_monterey_dv_functions_c.h);

system("mv $falmonDSrc/falcon2_monterey_field_access.c $falmonDSrc/falcon2_monterey_field_access_c.h\n");
#move($falmonDSrc/falcon2_monterey_field_access.c, $falmonDSrc/falcon2_monterey_field_access_c.h);

system("mv $falmonDSrc/falcon2_monterey_pll_config.c $falmonDSrc/falcon2_monterey_pll_config_c.h\n");
#move($falmonDSrc/falcon2_monterey_pll_config.c, $falmonDSrc/falcon2_monterey_pll_config_c.h)  ;

system("mv $falmonDSrc/falcon2_monterey_internal.c $falmonDSrc/falcon2_internal_c.h\n");
#move($falmonDSrc/falcon2_monterey_internal.c, $falmonDSrc/falcon2_internal_c.h);
}
### END ########################## M A I N #####################################

=head1 SYNOPSIS

=over 2

=item * -h[e[l[p]]]    Don't execute. Display this help msg

=item * -show          Don't execute. Show the command that would be executed

=item * -eagle         only for eagle related files

=item * -falcon        only for falcon related files

=item * -mt2           only for mt2 related files

=item * -monterey      only for monterey related files

=item * -all           All PMD files.

=item * -debug|-dbg    More Debug msgs

=item * -skipCopy      Do not copy over files (most likely already done)

=item * -srcDir        Source Directory (relative to CWD or absolute)

=item * -intDir        Destination Directory (relative to CWD or absolute)

=item * -mv2destDir    Specify final dir. for transformed files and move them.

=item * --             Ignore all succeeding options

=back

=cut
