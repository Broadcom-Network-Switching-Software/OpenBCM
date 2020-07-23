my $in = $ARGV[0];
my $out = $ARGV[1];

my $str_hex;
my $str_ascii;
my $num;
my $num2;
       open(my $F, "<", "$in") or die "can't open in file";
       open(my $FF, ">:raw", "$out") or die "can't open out file";
LOOP:   while($str_ascii = <$F>) {
          next LOOP if(($str_ascii =~ m/^#/) || ($str_ascii =~ m/^\s*$/)); # Discard comments or white lines
          $str_ascii =~ m/0x(.*)/;
          $num = $1;
          $num2=join("",(split(//,$num))[6,7,4,5,2,3,0,1]);
          print $num2;
          print $FF pack('H8',$num2);
       }
          print "\n";
close($F);

close($FF);
