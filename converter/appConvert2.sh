#!/bin/bash



for i in $( ls *.xml ); do

#  	perl -pi -e 's/&auml;/ä/g'  $i
#  	perl -pi -e 's/&Auml;/Ä/g'  $i
#  	perl -pi -e 's/&ouml;/ö/g'  $i
#  	perl -pi -e 's/&Ouml;/Ö/g'  $i
#  	perl -pi -e 's/&uuml;/ü/g'  $i
#  	perl -pi -e 's/&Uuml;/Ü/g'  $i
#  	perl -pi -e 's/&szlig;/ß/g'  $i
 	rr=$(cat $i | grep 'Anwenderhanbuch')
	if [ "$rr" != "" ]; then
		echo $i
	fi



done

