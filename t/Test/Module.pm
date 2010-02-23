package t::Test::Module;

use strict;
use warnings;
use Test::More;

use base qw/Exporter/;
our @EXPORT = qw/
add_ok
delete_ok
get_ok
insmod_ok
rmmod_ok
/;

our $COMMAND = '/usr/local/bin/oreore-ioctl';

sub insmod_ok {
    is system("insmod mod_oreore.ko"), 0;
}

sub rmmod_ok {
    is system("rmmod mod_oreore"), 0;
}

sub add_ok {
    my ($dir) = @_;
    is system($COMMAND, 'add', $dir), 0, "add $dir";
}

sub delete_ok {
    my ($dir) = @_;
    is system($COMMAND, 'delete', $dir), 0, "delete $dir";
}

sub get_ok {
    my ($dir) = @_;
    is system($COMMAND, 'get'), 0;
}

1;
