# -*- coding: utf-8 -*-
from xml.etree.ElementTree import ElementTree

import os

path="../apps/de"  # insert the path to the directory of interest
dirList=os.listdir(path)

seminarixArray  = []
titleArray     = []
descriptionArray     = []


for fileName in dirList:
	if fileName.find (".xml") > 0:
		tree = ElementTree()
		tree.parse("../apps/de/"+fileName)

		title = tree.find("name")
		title = title.text.encode('utf-8')
		titleArray.append(title)

		seminarix = tree.find("seminarix")
		seminarix = seminarix.text.encode('utf-8')
		seminarixArray.append(seminarix)

		description = tree.find("description")
		if hasattr(description.text, "encode"):
			description = description.text.encode('utf-8')
			description = description.replace("Ü","&Uuml;")
			description = description.replace("ü","&uuml;")
			description = description.replace("Ä","&Auml;")
			description = description.replace("ä","&auml;")
			description = description.replace("Ö","&Ouml;")
			description = description.replace("ö","&ouml;")
			description = description.replace("ß","&szlig;")
			description = description.replace("(","")
			description = description.replace(")","")
			description = description.replace("\\n","<br>")
			description = description.replace("\n","")
			description = description.replace("'",'"')
			description = description.replace("\t","") 
		else:
			description = ""
		descriptionArray.append(description)



i = 0
seminarixFunc = "function getSeminarix() {\nvar returnArray = new Array();\n"
for y in seminarixArray:
	seminarixFunc = seminarixFunc+"returnArray["+str(i)+"] = '"+y+"'\n"
	i = i+1
seminarixFunc = seminarixFunc+"return returnArray;\n}"

i=0
titleFunc = "function getNames() {\nvar returnArray = new Array();\n"
for y in titleArray:
	titleFunc = titleFunc+"returnArray["+str(i)+"] = '"+y+"'\n"
	i = i+1
titleFunc = titleFunc+"return returnArray;\n}"


i=0
descriptionFunc = "function getDescriptions() {\nvar returnArray = new Array();\n"
for y in descriptionArray:
	descriptionFunc = descriptionFunc+"returnArray["+str(i)+"] = '"+y+"'\n"
	i = i+1
descriptionFunc = descriptionFunc+"return returnArray;\n}"

ff = open("data.js", "w")
ff.write(seminarixFunc+"\n\n"+titleFunc+"\n\n"+descriptionFunc)
ff.close()



