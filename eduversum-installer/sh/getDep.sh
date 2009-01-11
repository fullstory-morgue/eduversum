#!/bin/sh
# apllyChanges
# (C) 2009 Fabian Wuertz <xadras@sidux.com>
# License: GPL

input=$@
su nobody -c "LANG=C apt-get --assume-yes -u --print-uris $input"



