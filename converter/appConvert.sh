#!/bin/bash

cd txt

for i in $( ls *.txt ); do

	 sed '1d;2d' $i > ../txt2/$i
done

cd ../txt2/

for i in $( ls *.txt ); do

 	perl -pi -e 's/^\.$//g'  $i
 	perl -pi -e 's/\n/\\n\\n\n/g'  $i
 	perl -pi -e 's/^\\n\\n//g'  $i
	perl -pi -e 's/^\n/\n\t/g'  $i

done


cd ../pmap
for i in $( ls *.pmap ); do

	icon=$(cat $i | grep 'Icon=')
	seminarix=$(cat $i | grep 'Seminarix=')
 	homepage=$(cat $i | grep 'Homepage=')
	desktoppath=$(cat $i | grep 'DesktopPath=')
	exec=$(cat $i | grep 'Exec=')
	terminal=$(cat $i | grep 'Terminal=')
	package=$(cat $i | grep 'Package=')


	icon=${icon/'Icon='/}
	seminarix=${seminarix/'Seminarix='/}
	homepage=${homepage/'Homepage='/}
	desktoppath=${desktoppath/'DesktopPath='/}
	exec=${exec/'Exec='/}
	terminal=${terminal/'Terminal='/}
	package=${package/'Package='/}



	name=${i/.pmap/}
	newfile=../new/${i/.pmap/}.xml


	description=$(cat ../txt2/$name.txt)

	echo '<?xml version="1.0" encoding="UTF-8"?>' > $newfile
	echo '<!DOCTYPE eduversum>' >> $newfile
	echo '<eduversum>' >> $newfile
	if [ "$name" != "" ]; then
		echo "	<name>$name</name>" >> $newfile
	fi
	if [ "$icon" != "" ]; then
		echo "	<icon>$icon</icon>" >> $newfile
	fi
	if [ "$seminarix" != "" ]; then
		echo "	<seminarix>$seminarix</seminarix>" >> $newfile
	fi
	if [ "$homepage" != "" ]; then
		echo "	<homepage>$homepage</homepage>" >> $newfile
	fi
	if [ "$desktoppath" != "" ]; then
		echo "	<desktoppath>$desktoppath</desktoppath>" >> $newfile
	fi
	if [ "$exec" != "" ]; then
		echo "	<exec>$exec</exec>" >> $newfile
	fi
	if [ "$terminal" != "" ]; then
		echo "	<terminal>$terminal</terminal>" >> $newfile
	fi
	if [ "$package" != "" ]; then
		echo "	<package>$package</package>" >> $newfile
	fi
	if [ "description" != "" ]; then
		echo "	<description lang=\"de\">$description</description>" >> $newfile
	fi


	echo '</eduversum>' >> $newfile

	perl -pi -e 's/\\n\\n<\/description>/<\/description>/g'  $newfile
	 

done
 
