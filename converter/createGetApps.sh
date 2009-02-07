#!/bin/bash
cd dev/eduversum/eduversum/data/apps
j=0

echo "function getApps() {" > ../../../../../getApps.js
echo "	var returnArray = new Array();" >> ../../../../../getApps.js
for i in $( ls *.xml ); do
	 echo "	returnArray[$j] = '$i';" >> ../../../../../getApps.js
	 let "j += 1"

done
echo "	return returnArray;" >> ../../../../../getApps.js
echo "}" >> ../../../../../getApps.js