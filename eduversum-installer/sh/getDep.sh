
input=$@
su nobody -c "LANG=C apt-get --assume-yes -u --print-uris $input"



