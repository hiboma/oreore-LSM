use Test::More qw/no_plan/;

use strict;
use warnings;
use t::Test::Module;

`rmmod mod_oreore >/dev/null 2>&1`;

insmod_ok();

add_ok '/usr/';
add_ok '/usr/local/';
add_ok '/usr/local/bin/';

get_ok();

delete_ok '/usr/';
delete_ok '/usr/local/';
delete_ok '/usr/local/bin/';

# add_ok '/usr';
# add_ok '/usr/local';
# add_ok '/usr/local/bin';

# get_ok();

# delete_ok '/usr';
# delete_ok '/usr/local';
# delete_ok '/usr/local/bin';
