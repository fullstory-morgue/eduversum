#ifndef  EASYXML_H
#define  EASYXML_H


#include <QObject>
#include <QTreeWidget>
#include <QDomDocument>

class EasyXml : public QObject
{
        Q_OBJECT

    public:
        EasyXml(QString path);
        QStringList getValue(QString value);
        QStringList getValue(QStringList values);
        void readTree(QDomNode, QTreeWidgetItem *parentItem = 0);
 private:

     QTreeWidget *outputTreeWidget;

};

#endif
