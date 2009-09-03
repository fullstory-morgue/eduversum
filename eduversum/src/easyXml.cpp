#include "easyXml.h"

#include <QFile>
#include <QMessageBox>

EasyXml::EasyXml(QString path)
{

    outputTreeWidget = new QTreeWidget;

    QDomDocument doc("mydocument");
    QFile file( path );
    if (!file.open(QIODevice::ReadOnly))
            return;
    if (!doc.setContent(&file)) {
            file.close();
            return;
    }
    file.close();

    QDomElement docElem = doc.documentElement();
    readTree(docElem.firstChild());

}




void EasyXml::readTree(QDomNode n, QTreeWidgetItem *item)
{

    while(!n.isNull()) {
        QDomElement e = n.toElement(); // try to convert the node to an element.
        if(!e.isNull()) {
            QTreeWidgetItem *item2;
            if (item)
                item2 = new QTreeWidgetItem(item);
            else
                item2 = new QTreeWidgetItem(outputTreeWidget);
            item2->setText( 0, e.tagName() );
            item2->setText( 1, e.text() );
            item2->setSelected(true);

            readTree(n.firstChild(), item2);

         }
         n = n.nextSibling();
    }
}





QStringList EasyXml::getValue(QString value)
{

    return getValue(QStringList() << value);
}


QStringList EasyXml::getValue(QStringList values)
{

QStringList result;

    QList<QTreeWidgetItem *> items;
    items = outputTreeWidget->findItems(values[values.count()-1], Qt::MatchExactly | Qt::MatchRecursive, 0);

    values.removeLast();

    foreach(QTreeWidgetItem *item, items) {
        QTreeWidgetItem *item2 = item;
        QStringList parents;
        while(item2->parent()) {
               item2 = item2->parent();
               parents.append(item2->text(0));
        }
        if( values ==  parents)
            result.append(item->text(1));
    }

    return result;


}
