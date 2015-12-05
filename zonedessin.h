#ifndef ZONEDESSIN_H
#define ZONEDESSIN_H

#include <QWidget>
#include <QMouseEvent>
#include <QPoint>
#include <QPainter>
#include <list>
#include <QStateMachine>
#include <QEvent>
#include "Transitions.h"
#include <QFinalState>
#include <QDebug>

//enumeration representant les differents modes de dessin
enum Shape {LINE, RECTANGLE, ELLIPSE, POLYLINE, SELECT, MOVE};

/*
 * Classe héritant de QWidget : affiche l'image et gere la partie dessin
 */

class ZoneDessin: public QWidget
{
    Q_OBJECT

public:
    explicit ZoneDessin(QWidget *parent = 0);
    ~ZoneDessin();

    void paintEvent(QPaintEvent * e);

    //setters pour les attrituts de curPen
    void setPenColor(QColor c) {curPen.setColor(c); updateSelect(); update();}
    void setPenWidth(int w) {curPen.setWidth(w); updateSelect(); update();}

    //declenche un signal pour la machine à états
    void setShape(int shape);

    //fonctions agissant sur displayList
    void cancelLast();
    void cancelAll();

    //operations de serialization/deserialization de displayList
    void readDisplayList(QDataStream &in);
    void writeDisplayList(QDataStream *out);

private:
    //les differents pen
    //pen courant, pen pour l'operation de selection, pen pour les objets selectionnes
    QPen curPen, selectPen, selectedPen;

    QPoint lastPoint, endPoint; // clic=lastPoint --> on relache le bouton=endPoint

    QPainterPath curPath, bufPath; // QPainter path en cours de production, avec un buffer necessaire pour les polyline
    QPainterPath select; // QPainterPath permettant la selection

    QPainter painter; //painter


    //aggregation d'un QPath et du QPen dans lequel il doit être tracé pour faciliter la gestion
    struct QPathPen {
        QPainterPath path;
        QPen pen;
        QPathPen(QPainterPath p, QPen pp) : path(p), pen(pp) {}
    };

    //liste de QPathPen (correspond à l'image en cours)
    typedef std::list<QPathPen> DisplayList;
    DisplayList displayList;

    //liste des pointeurs vers les QPathPen de la selection courante
    typedef std::list<QPathPen*> Selected;
    Selected selected;

    QStateMachine * addShape; //machine a etant pour ajouter, selectionne, et deplacer des QPathPen
    void addShapeMachine(); //fonction d'initialisation de la machine à états

public slots:
    /*****************************************************************************
     * slots de creation de shape
     *****************************************************************************/
    // 1->2
    void setLastPoint();

    // 2->2
    void setEndPointLine();
    void setEndPointRect();
    void setEndPointEll();
    void setEndPointPolyLine();
    void setEndPointSelect();
    void setEndPointMove();

    // 2->2 bis
    void addLinePolyLine();

    // 2->1
    void saveLine();
    void saveRect();
    void saveEll();
    void savePolyLine();
    void saveSelect();

    /*****************************************************************************
     * slots pour la selection
     *****************************************************************************/
    void updateSelect();
    void clearSelect();
};

#endif // ZONEDESSIN_H
