#include "easyXml.h"
#include "settings.h"

#include <QFile>
#include <QMessageBox>
#include <QXmlStreamReader>
#include <QDir>


/*
@name: xmlToList
@description: This function import a xml file into a list.
@author: Fabian Wuertz
*/

QMap<QString,QString> EasyXml::xmlToList(QString inputString) {
    QString language = Settings::language();
    QMap<QString,QString> list;
    QFile* file = new QFile(inputString);
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text)) {return list;}
    QXmlStreamReader xml(file);
    while(!xml.atEnd() && !xml.hasError()) {
        /* Read next element.*/
        QXmlStreamReader::TokenType token = xml.readNext();
        /* If token is just StartDocument, we'll go to next.*/
        if(token == QXmlStreamReader::StartDocument) {continue;}
        /* If token is StartElement, we'll see if we can read it.*/
        if(token == QXmlStreamReader::StartElement) {
            QString name = xml.name().toString();
            if(name == "eduversum") {
                continue;
            } else if( !name.isEmpty() ) {
                QString lang = xml.attributes().value("xml:lang").toString();
                if( lang == language ) {
                    xml.readNext();
                    list.insert(name, xml.text().toString() );
                } else if ( lang.isEmpty() and !list.contains(name) ) {
                    xml.readNext();
                    list.insert(name, xml.text().toString() );
                }
            }
        }
    }
    return list;
}

/*
@name: xmlsToList
@description: This function import all xml files in a given directory into a list.
@author: Fabian Wuertz
*/

QList<QTreeWidgetItem *> EasyXml::xmlsToList(QString path, QStringList values) {
    // list diretory
    path += "/";
    QStringList xmls = QDir(path).entryList( QDir::Files );

    // import a xml file into a QTreeWidget
    QList<QTreeWidgetItem *> items;
    foreach(QString xml, xmls) {
        QMap<QString,QString> list = xmlToList(path+xml);
        QTreeWidgetItem *item = new QTreeWidgetItem();
        int i = 0;
        foreach(QString value, values) {
            item->setText( i, list.value(value) );
            i++;
        }
        items.append(item);
    }
    return items;
}
