#
# $Id: utest.pl,v 1.13 2012/03/02 16:21:40 yaronm Exp $
# This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
# 
# Copyright 2007-2020 Broadcom Inc. All rights reserved.
#
# File:        utest.pl
# Purpose:     Unit test script for the CINT library
#
#

use IPC::Open2;


my $tdata = shift;
my $tname = shift;

if(!defined($tdata)) {
  $tdata = "utests";
}

open TDATA, "$tdata" || die("Could not open file '$tdata'");
open LOG, ">results.$tdata" || die("Could not open log file");

my @tdata = <TDATA>;
my $tdata = join "", @tdata;

my @tests;

#
# Remove all comments
#
while ($tdata =~ s/#(.*?)\n//g) {};

#
# convert %include to #include
#
while ($tdata =~ s/%include/#include/g) {};

#
# Read all begin/end code blocks
#
my @startcode;
my @endcode;

while($tdata =~ s/STARTCODE\n(.*?)\nENDCODE\n(.*?)\nEND//s) {
  push @startcode, $1;
  push @endcode, $2;
}

#
# Read all named tests cases
#
while ($tdata =~ s/TEST (.*?)\nCODE\n(.*?)\nRESULT\n(.*?)\nEND//s) {
  my $r = {};
  $r->{NAME} = $1;
  $r->{CODE} = $2;
  $r->{RESULT} = $3;
  push @tests, $r;
}

#
# Read all unnamed/new test cases.
#
my $tnum = $#tests+1;
while ($tdata =~ s/CODE\n(.*?)RESULT\n(.*?)END//s) {
  my $r = {};
  $r->{NAME} = "$tnum"; $tnum++;
  $r->{CODE} = $1;
  $r->{RESULT} = $2;
  push @tests, $r;
}

if($tdata !~ /^\s*$/) {
  die("Excess data left after reading testcases: '$tdata'");
}

#
# Execute tests
#
my $pcount = 0;
my $fcount = 0;

if(defined($tname)) {
  run_test($tests[$tname]);
}
else {
  foreach my $test (@tests)
    {
      run_test($test);
    }
}

print "\n$pcount passed, $fcount failed\n\n";
print LOG "$pcount passed, $fcount failed\n";
close LOG;


### SUBROUTINES ##################################################

sub run_test
  {
    my $test = shift;

    printf("$test->{NAME}: ");

    my($rfh, $wfh);
    #my $pid = open2($rfh, $wfh, "valgrind --leak-check=full ./cint --noprompt");
    #my $pid = open2($rfh, $wfh, "valgrind --leak-check=full --track-fds=yes ./cint --noprompt");
    my $pid = open2($rfh, $wfh, "./cint --noprompt");

    print $wfh @startcode;
    print $wfh $test->{CODE};
    print $wfh @endcode;
    close $wfh;

    my @results = <$rfh>;
    my $results = join "", @results;

    if($results eq $test->{RESULT}) {
      $pcount++;
      print "PASS\n";
    }
    else {
      $fcount++;
      # Check for test comment at beginning of test
      my $comment;
      if($test->{CODE} =~ /^\"(.*?)\"/) {
        $comment = $1;
      }
      print "FAIL      $comment\n";
      print LOG "TEST $test->{NAME} FAILED\n";
      print LOG "CODE\n";
      print LOG $test->{CODE};
      print LOG "RESULT\n";
      print LOG $test->{RESULT};
      print LOG "ACTUAL\n";
      print LOG "$results\n\n";
      print LOG "END\n";

      open F, ">$test->{NAME}.expected";

      print F "START CODE:\n";
      print F "$test->{CODE};\n";
      print F "END CODE:\n\n";

      print F "$test->{RESULT}\n";
      close F;
      open F, ">$test->{NAME}.actual";
      print F "START CODE:\n";
      print F "$test->{CODE};\n";
      print F "END CODE:\n\n";
      print F "$results\n";
      close F;
      open F, ">$test->{NAME}.code";
      print F @startcode;
      print F $test->{CODE};
      print F @endcode;
      if(defined($ENV{FD})) {
        system("tkdiff $test->{NAME}.expected $test->{NAME}.actual &");
      }
      close F;
    }
  }



