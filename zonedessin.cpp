#include "zonedessin.h"


/*****************************************************************************
 * Constructeurs/destructeur
 *****************************************************************************/
ZoneDessin::ZoneDessin(QWidget *parent) :
    QWidget(parent)
{
    setMinimumSize(800,600);

    selectPen=QPen(Qt::black, 1, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin);
    selectedPen=QPen(Qt::yellow, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    curPen=QPen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);

    this->setMouseTracking(true);

    addShapeMachine();
}

ZoneDessin::~ZoneDessin() {

}

/*****************************************************************************
 * Machine à états
 * Voici un petit arbre représentant les états pour y voir un peu plus clair :
 *  sGlobal
 *      * sAddShape
 *          sIdle
 *          sAddLine
 *              -> sAddLine1-2
 *          sAddRect
 *              -> sAddRect1-2
 *          sAddEllipse
 *              -> sAddEllipse1-2
 *          sAddPolyline
 *              -> sAddPolyline1-2
 *      * sSelection
 *          sSelectShape
 *              -> sSelectShape1-2
 *          sMoveSelect
 *              -> sMoveSelect1-2
 *
 *****************************************************************************/
void ZoneDessin::addShapeMachine() {
    addShape = new QStateMachine();

    QState * sGlobal=new QState();

    //etat pere
    QState * sAddShape=new QState(sGlobal);
    sGlobal->setInitialState(sAddShape);

    //idle
    QState * sIdle = new QState(sAddShape);

    //line
    QState * sAddLine = new QState(sAddShape);

    QState * sAddLine1 = new QState(sAddLine);
    QState * sAddLine2 = new QState(sAddLine);
    sAddLine->setInitialState(sAddLine1);
    addMouseTrans(sAddLine1, sAddLine2, this, QEvent::MouseButtonPress, Qt::LeftButton, this, SLOT(setLastPoint()));
    addMouseTrans(sAddLine2, sAddLine2, this, QEvent::MouseMove, Qt::NoButton, this, SLOT(setEndPointLine()));
    addMouseTrans(sAddLine2, sAddLine1, this, QEvent::MouseButtonRelease, Qt::LeftButton, this, SLOT(saveLine()));

    //rectangle
    QState * sAddRect = new QState(sAddShape);

    QState * sAddRect1 = new QState(sAddRect);
    QState * sAddRect2 = new QState(sAddRect);
    sAddRect->setInitialState(sAddRect1);
    addMouseTrans(sAddRect1, sAddRect2, this, QEvent::MouseButtonPress, Qt::LeftButton, this, SLOT(setLastPoint()));
    addMouseTrans(sAddRect2, sAddRect2, this, QEvent::MouseMove, Qt::NoButton, this, SLOT(setEndPointRect()));
    addMouseTrans(sAddRect2, sAddRect1, this, QEvent::MouseButtonRelease, Qt::LeftButton, this, SLOT(saveRect()));

    //ellipse
    QState * sAddEll = new QState(sAddShape);

    QState * sAddEll1 = new QState(sAddEll);
    QState * sAddEll2 = new QState(sAddEll);
    sAddEll->setInitialState(sAddEll1);
    addMouseTrans(sAddEll1, sAddEll2, this, QEvent::MouseButtonPress, Qt::LeftButton, this, SLOT(setLastPoint()));
    addMouseTrans(sAddEll2, sAddEll2, this, QEvent::MouseMove, Qt::NoButton, this, SLOT(setEndPointEll()));
    addMouseTrans(sAddEll2, sAddEll1, this, QEvent::MouseButtonRelease, Qt::LeftButton, this, SLOT(saveEll()));

    //line
    QState * sAddPolyLine = new QState(sAddShape);

    QState * sAddPolyLine1 = new QState(sAddPolyLine);
    QState * sAddPolyLine2 = new QState(sAddPolyLine);
    sAddPolyLine->setInitialState(sAddPolyLine1);
    addMouseTrans(sAddPolyLine1, sAddPolyLine2, this, QEvent::MouseButtonPress, Qt::LeftButton, this, SLOT(setLastPoint()));
    addMouseTrans(sAddPolyLine2, sAddPolyLine2, this, QEvent::MouseMove, Qt::NoButton, this, SLOT(setEndPointPolyLine()));
    addMouseTrans(sAddPolyLine2, sAddPolyLine2, this, QEvent::MouseButtonPress, Qt::LeftButton, this, SLOT(addLinePolyLine()));
    addMouseTrans(sAddPolyLine2, sAddPolyLine1, this, QEvent::MouseButtonDblClick, Qt::LeftButton, this, SLOT(savePolyLine()));

    //select global
    QState *sSelection = new QState(sGlobal);

    //selection
    QState * sSelectShape = new QState(sSelection);

    QState * sSelectShape1 = new QState(sSelectShape);
    QState * sSelectShape2 = new QState(sSelectShape);
    sSelectShape->setInitialState(sSelectShape1);
    addMouseTrans(sSelectShape1, sSelectShape2, this, QEvent::MouseButtonPress, Qt::LeftButton, this, SLOT(setLastPoint()));
    addMouseTrans(sSelectShape2, sSelectShape2, this, QEvent::MouseMove, Qt::NoButton, this, SLOT(setEndPointSelect()));
    addMouseTrans(sSelectShape2, sSelectShape1, this, QEvent::MouseButtonRelease, Qt::LeftButton, this, SLOT(saveSelect()));

    QObject::connect(sAddShape, SIGNAL(entered()), this, SLOT(clearSelect())); //on deselectionne pour les autres actions

    //move
    QState * sMoveSelect = new QState(sSelection);

    QState * sMoveSelect1 = new QState(sMoveSelect);
    QState * sMoveSelect2 = new QState(sMoveSelect);
    sMoveSelect->setInitialState(sMoveSelect1);
    sSelection->setInitialState(sMoveSelect);
    addMouseTrans(sMoveSelect1, sMoveSelect2, this, QEvent::MouseButtonPress, Qt::LeftButton, this, SLOT(setLastPoint()));
    addMouseTrans(sMoveSelect2, sMoveSelect2, this, QEvent::MouseMove, Qt::NoButton, this, SLOT(setEndPointMove()));
    addMouseTrans(sMoveSelect2, sMoveSelect1, this, QEvent::MouseButtonRelease, Qt::LeftButton, this, SLOT(setLastPoint()));

    //transitions parent->shape
    addCustomTrans(sGlobal,sAddLine,LINE);
    addCustomTrans(sGlobal,sAddRect,RECTANGLE);
    addCustomTrans(sGlobal,sAddEll,ELLIPSE);
    addCustomTrans(sGlobal,sAddPolyLine,POLYLINE);
    addCustomTrans(sGlobal,sSelectShape,SELECT);
    addCustomTrans(sGlobal,sMoveSelect,MOVE);

    sAddShape->setInitialState(sIdle);
    addShape->addState(sGlobal);
    addShape->setInitialState(sGlobal);

    addShape->start();
}
void ZoneDessin::setShape(int shape) {
    addShape->postEvent(new CustomEvent(shape));
}

/*****************************************************************************
 * slots de creation de shape
 *****************************************************************************/
void ZoneDessin::setLastPoint() {
    lastPoint = cursorPos(this);
    endPoint = cursorPos(this);
    update();
}
/* line **********************************************************************/
void ZoneDessin::setEndPointLine() {
    endPoint = cursorPos(this);

    QPainterPath path;
    path.moveTo(lastPoint);
    path.lineTo(endPoint);

    curPath=path;

    update();
}
void ZoneDessin::saveLine() {
    endPoint = cursorPos(this);
    QPainterPath path;
    curPath=path;
    path.moveTo(lastPoint);
    path.lineTo(endPoint);

    displayList.push_back(QPathPen(path,curPen));
    update();
}
/* rectangle *****************************************************************/
void ZoneDessin::setEndPointRect() {
    endPoint = cursorPos(this);

    QPainterPath path;
    bufPath=path;
    path.addRect(lastPoint.x(),lastPoint.y(),endPoint.x()-lastPoint.x(),endPoint.y()-lastPoint.y());
    curPath=path;

    update();
}
void ZoneDessin::saveRect() {
    endPoint = cursorPos(this);
    QPainterPath path;
    curPath=path;
    path.addRect(lastPoint.x(),lastPoint.y(),endPoint.x()-lastPoint.x(),endPoint.y()-lastPoint.y());

    displayList.push_back(QPathPen(path,curPen));
    update();
}
/* ellipse *******************************************************************/
void ZoneDessin::setEndPointEll() {
    endPoint = cursorPos(this);

    QPainterPath path;
    path.addEllipse(lastPoint.x(),lastPoint.y(),endPoint.x()-lastPoint.x(),endPoint.y()-lastPoint.y());
    curPath=path;

    update();
}
void ZoneDessin::saveEll() {
    endPoint = cursorPos(this);

    QPainterPath path;
    curPath=path;
    path.addEllipse(lastPoint.x(),lastPoint.y(),endPoint.x()-lastPoint.x(),endPoint.y()-lastPoint.y());

    displayList.push_back(QPathPen(path,curPen));
    update();
}
/* polyline ******************************************************************/
void ZoneDessin::setEndPointPolyLine() {
    endPoint = cursorPos(this);

    curPath=bufPath;
    curPath.moveTo(lastPoint);
    curPath.lineTo(endPoint);

    update();
}
void ZoneDessin::addLinePolyLine() {
    endPoint = cursorPos(this);

    bufPath.moveTo(lastPoint);
    bufPath.lineTo(endPoint);
    lastPoint=endPoint;

    update();
}
void ZoneDessin::savePolyLine() {
    endPoint = cursorPos(this);
    QPainterPath path;
    curPath=path;
    path=bufPath;
    bufPath=curPath;

    path.moveTo(lastPoint);
    path.lineTo(endPoint);

    displayList.push_back(QPathPen(path,curPen));
    update();
}

/*****************************************************************************
 * slots de selection
 *****************************************************************************/
/* select ********************************************************************/
void ZoneDessin::setEndPointSelect() {
    endPoint = cursorPos(this);

    QPainterPath path;
    select=path;
    select.addRect(lastPoint.x(),lastPoint.y(),endPoint.x()-lastPoint.x(),endPoint.y()-lastPoint.y());

    update();
}
void ZoneDessin::saveSelect() {
    //qDebug() << "Selecting the following elements" << endl;
    selected.clear();
    for (DisplayList::iterator ci = displayList.begin(); ci != displayList.end(); ++ci) {
        if(select.contains(ci->path)) {
            //qDebug() << ci->path;
            selected.push_back(&(*ci));
        }
    }
    select=QPainterPath();
    update();
}
void ZoneDessin::updateSelect() {
    for (Selected::iterator ci = selected.begin(); ci != selected.end(); ++ci) {
        (*ci)->pen=curPen;
    }
    update();
}
void ZoneDessin::clearSelect() {
    QPainterPath path;
    select=path;
    selected.clear();
    //qDebug() << "clearing selection";
}
/* move **********************************************************************/
void ZoneDessin::setEndPointMove() {
    endPoint = cursorPos(this);
    int x=endPoint.x()-lastPoint.x();
    int y=endPoint.y()-lastPoint.y();
    for (Selected::iterator ci = selected.begin(); ci != selected.end(); ++ci) {
        (*ci)->path.translate(x,y);
    }
    lastPoint=endPoint;
    update();
}

/*****************************************************************************
 * displaylist
 *****************************************************************************/
void ZoneDessin::cancelLast() {
    if(!displayList.empty()) {
        selected.clear();
        displayList.pop_back();
        curPath=QPainterPath();
        update();
    }
}
void ZoneDessin::cancelAll() {
    selected.clear();
    displayList.clear();
    curPath=QPainterPath();
    update();
}

/* serialization *************************************************************/
void ZoneDessin::readDisplayList(QDataStream &in) {
    displayList.clear();
    int n;
    in >> n;
    for(int i=0; i<n ; i++) {
        //qDebug << in.status;
        QPainterPath path;
        QPen pen;
        in >> path;
        //qDebug() << path << endl;
        in >> pen;
        //qDebug() << pen << endl;
        //qDebug() << "LECTURE " << path << pen;

        displayList.push_back(QPathPen(path,pen));
    }
    update();
}
void ZoneDessin::writeDisplayList(QDataStream* out) {
    int n=displayList.size();
    *out << n;
    for (DisplayList::const_iterator ci = displayList.begin(); ci != displayList.end(); ++ci) {
        *out << (ci->path) << (ci->pen);
        //qDebug() << "ECRITURE " << (ci->path) << (ci->pen);
    }
}

/*****************************************************************************
 * paintEvent
 *****************************************************************************/
void ZoneDessin::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);

    //affichage de la selection
    for (Selected::const_iterator ci = selected.begin(); ci != selected.end(); ++ci) {
        QPen pen=selectedPen;
        pen.setWidth(((*ci)->pen.width())+5); //mise en surbrillance
        painter.setPen(pen);
        painter.drawPath((*ci)->path);
    }

    //affichage de displaylist
    for (DisplayList::const_iterator ci = displayList.begin(); ci != displayList.end(); ++ci) {
        painter.setPen(ci->pen);
        painter.drawPath(ci->path);
    }

    //affichage du rectangle de selection
    painter.setPen(selectPen);
    painter.drawPath(select);

    //affichage du path courant
    painter.setPen(curPen);
    painter.drawPath(curPath);
}
