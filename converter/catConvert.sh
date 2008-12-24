#!/bin/bash
for i in $( ls *.desktop ); do
	name=$(cat $i | grep 'Name=')
	nameRu=$(cat $i | grep 'Name[ru]=')
	icon=$(cat $i | grep 'Icon=')

	name=${name/'Name='/}
	nameRu=${nameRu/'Name[ru]='/}
	icon=${icon/'Icon='/}

	newfile=${i/.desktop/}.xml

	echo '<?xml version="1.0" encoding="UTF-8"?>' > $newfile
	echo '<!DOCTYPE eduversum>' >> $newfile
	echo '<eduversum>' >> $newfile
	echo "	<name lang=\"de\">$name</name>" >> $newfile
	echo "	<name lang=\"ru\">$nameRu</name>" >> $newfile
	echo "	<icon>$icon</icon>" >> $newfile
	echo '</eduversum>' >> $newfile
done
 
