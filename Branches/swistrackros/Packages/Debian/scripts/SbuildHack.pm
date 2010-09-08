package SbuildHack;

use Sbuild qw(binNMU_version);

sub new_binNMU_version {
    my $v = shift;
    my $binNMUver = shift;
    die("Wrong binNMUver!") unless ($binNMUver == 171717);
    die("No NMUTAG set in environment!") unless ($ENV{"NMUTAG"});
    return $v . $ENV{"NMUTAG"};
};

{
    no warnings 'redefine';
    *Sbuild::binNMU_version = \&new_binNMU_version;
}

1;
