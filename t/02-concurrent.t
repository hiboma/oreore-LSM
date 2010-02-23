use Test::More qw/no_plan/;

use strict;
use warnings;
use FindBin;
use File::Spec::Functions;
use t::Test::Module;

insmod_ok();
add_ok(catdir($FindBin::Bin));
is system("sh t/thread.c"), 0;
rmmod_ok();

insmod_ok();
add_ok(catdir($FindBin::Bin));
is system("sh t/thread-void.c"), 0;
rmmod_ok();



