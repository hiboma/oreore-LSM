use Test::More tests => 2;

use strict;
use warnings;
use t::Test::Module;

`rmmod mod_oreore >/dev/null 2>&1`;

insmod_ok();
rmmod_ok();
