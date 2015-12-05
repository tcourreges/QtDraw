#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTextEdit>
#include <iostream>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QPixmap>
#include <QTimer>
#include <QPushButton>

using namespace std;


/*****************************************************************************
 * Constructeurs/destructeur
 *****************************************************************************/
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    setWindowTitle("QtDraw");
    if(USE_DESIGNER) //IMPLEMENTATION AVEC QT DESIGNER
    {
        ui->setupUi(this);
        zoneDessin=ui->zoneDessin;
        designerFix();
    }
    else //VIEILLE IMPLEMENTATION
    {
        addFileActions();
        addOutilsZoneDessin();
        addParamZoneDessin();
        zoneDessin=new ZoneDessin();
        setCentralWidget(zoneDessin);
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

/*****************************************************************************
 * Slots pour les opérations du menu File
 *****************************************************************************/
void MainWindow::openFile() {
    QString fileName =
            QFileDialog::getOpenFileName(   this, tr("Open Image"),
                                            ":/draw/" // répertoire initial
                                            //tr("Text Files (*.txt *.html)") // filtre
                                            );
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    QDataStream in(&file);
    zoneDessin->readDisplayList(in);
    file.close();
}

void MainWindow::saveFile(){
    QString fileName =
            QFileDialog::getOpenFileName(   this, tr("Open Image"), // titre
                                            ":/draw/"//, // répertoire initial
                                            //tr("Text Files (*.txt *.html)") // filtre
                                            );
    QFile outfile(fileName);
    if(!outfile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

    QDataStream out(&outfile);
    zoneDessin->writeDisplayList(&out);
    outfile.close();
}

void MainWindow::quitApp(){
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Quit?", QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        //qDebug() << "Yes";
        QApplication::quit();
    }
    else {
        //qDebug() << "No";
    }
}

/*****************************************************************************
 * Implementation sans Qt Designer
 *****************************************************************************/
void MainWindow::addFileActions() {
    QMenuBar * menuBar = this->menuBar( );
    QMenu * fileMenu = menuBar->addMenu( tr ("&File") );
    toolBar = this->addToolBar( tr("File") );

    //new
    openAction = new QAction( QIcon(":/images/open.png"), tr("&Open..."), this);
    openAction->setShortcut( tr("Ctrl+O")); // accélérateur clavier
    openAction->setToolTip( tr("Open file")); // bulle d’aide
    openAction->setStatusTip( tr("Open File")); // barre de statut
    connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));
    //save
    saveAction = new QAction( QIcon(":/images/save.png"), tr("&Save..."), this);
    saveAction->setShortcut( tr("Ctrl+S"));
    saveAction->setToolTip( tr("Save"));
    saveAction->setStatusTip( tr("Save"));
    connect(saveAction, SIGNAL(triggered()), this, SLOT(saveFile()));
    //quit
    quitAction = new QAction( QIcon(":/images/quit.png"), tr("&Quit..."), this);
    quitAction->setShortcut( tr("Ctrl+Q"));
    quitAction->setToolTip( tr("Quit"));
    quitAction->setStatusTip( tr("Quit"));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quitApp()));

    fileMenu->addAction(openAction);
    fileMenu->addAction(saveAction);
    fileMenu->addAction(quitAction);

    toolBar->addAction(openAction);
    toolBar->addAction(saveAction);
    toolBar->addAction(quitAction);
}

void MainWindow::addOutilsZoneDessin() {
    QActionGroup *sgroup=new QActionGroup(this);
    sgroup->setExclusive(false);
    connect(sgroup, SIGNAL(triggered(QAction*)),this,SLOT(setShape(QAction*)));
    shape1=sgroup->addAction(tr("Line"));
    shape1->setIcon(QIcon(":/images/line.png"));
    shape2=sgroup->addAction(tr("Rectangle"));
    shape2->setIcon(QIcon(":/images/rectangle.png"));
    shape3=sgroup->addAction(tr("Ellipse"));
    shape3->setIcon(QIcon(":/images/ellipse.png"));
    shape4=sgroup->addAction(tr("PolyLine"));
    select=sgroup->addAction(tr("Select"));
    move=sgroup->addAction(tr("Move"));

    toolBar->addAction(shape1);
    toolBar->addAction(shape2);
    toolBar->addAction(shape3);
    toolBar->addAction(shape4);
    toolBar->addAction(select);
    toolBar->addAction(move);
}

void MainWindow::addParamZoneDessin() {
    //width -------------------------------------------------------
    QActionGroup *wgroup=new QActionGroup(this);
    wgroup->setExclusive(false);
    connect(wgroup, SIGNAL(triggered(QAction*)),this,SLOT(setPenWidth(QAction*)));

    width1=wgroup->addAction(tr("1"));
    width2=wgroup->addAction(tr("3"));
    width3=wgroup->addAction(tr("5"));

    width = new QMenu();
    width->addAction(width1);
    width->addAction(width2);
    width->addAction(width3);
    toolBar->addAction(width->menuAction());
    width->menuAction()->setText(width1->text());
    //color --------------------------------------------------------
    QActionGroup *cgroup=new QActionGroup(this);
    cgroup->setExclusive(false);
    connect(cgroup, SIGNAL(triggered(QAction*)),this,SLOT(setPenColor(QAction*)));

    color1=cgroup->addAction(tr("red"));
    color2=cgroup->addAction(tr("green"));
    color3=cgroup->addAction(tr("blue"));
    //list --------------------------------------------------------
    cancelLastAction = new QAction(tr("&Cancel"), this);
    cancelAllAction = new QAction(tr("&Cancel All"), this);
    connect(cancelLastAction, SIGNAL(triggered()), this, SLOT(cancelLast()));
    connect(cancelAllAction, SIGNAL(triggered()), this, SLOT(cancelAll()));

    color = new QMenu();
    color->addAction(color1);
    color->addAction(color2);
    color->addAction(color3);
    toolBar->addAction(color->menuAction());
    color->menuAction()->setText(color1->text());

    toolBar->addAction(cancelLastAction);
    toolBar->addAction(cancelAllAction);
}

/*****************************************************************************
 * Implémentation avec Qt Designer
 *****************************************************************************/
//certaines fonctionalités sont inacessibles avec qt designer (par exemple les boutons exclusifs)... :(
void MainWindow::designerFix() {
    QActionGroup * group = new QActionGroup( this );
    group->addAction( ui->actionLine );
    group->addAction( ui->actionRectangle );
    group->addAction( ui->actionEllipse );
    group->addAction( ui->actionPolyline );
    group->addAction( ui->actionSelect);
    group->addAction( ui->actionMove );
    group->setExclusive(true);

    QPixmap px(33, 33);
    px.fill(Qt::black);
    ui->actionColor->setIcon(px);

    //menu circulaire
    contextMenu =new QWidget(this);
    contextButton1=new QToolButton(contextMenu);
    contextButton2=new QToolButton(contextMenu);
    contextButton3=new QToolButton(contextMenu);
    contextButton4=new QToolButton(contextMenu);
    contextButton5=new QToolButton(contextMenu);
    contextButton6=new QToolButton(contextMenu);

    contextButton1->setDefaultAction(ui->actionSelect);
    contextButton2->setDefaultAction(ui->actionMove);
    contextButton3->setDefaultAction(ui->actionLine);
    contextButton4->setDefaultAction(ui->actionRectangle);
    contextButton5->setDefaultAction(ui->actionEllipse);
    contextButton6->setDefaultAction(ui->actionPolyline);

    contextButton1->setIconSize(QSize(32,32));
    contextButton2->setIconSize(QSize(32,32));
    contextButton3->setIconSize(QSize(32,32));
    contextButton4->setIconSize(QSize(32,32));
    contextButton5->setIconSize(QSize(32,32));
    contextButton6->setIconSize(QSize(32,32));

    contextButton1->setStyleSheet("width: 32; height: 32; background: transparent; border-width: 0");
    contextButton2->setStyleSheet("width: 32; height: 32; background: transparent; border-width: 0");
    contextButton3->setStyleSheet("width: 32; height: 32; background: transparent; border-width: 0");
    contextButton4->setStyleSheet("width: 32; height: 32; background: transparent; border-width: 0");
    contextButton5->setStyleSheet("width: 32; height: 32; background: transparent; border-width: 0");
    contextButton6->setStyleSheet("width: 32; height: 32; background: transparent; border-width: 0");

    contextMenu->setStyleSheet("background-image: url(:/images/donut.png)");

    contextButton1->move(75-16  , 25-16);
    contextButton2->move(125-16 , 50-16);
    contextButton3->move(125-16 , 104-16);
    contextButton4->move(75-16  , 130-16);
    contextButton5->move(25-16  , 50-16);
    contextButton6->move(25-16  , 104-16);

    hideContextMenu();

    //mini machine à états avec les étants on et off
    QStateMachine * context=new QStateMachine();
    QState *off = new QState();
    QState *on = new QState();
    addEventTrans(on,off, contextMenu, QEvent::Leave, this, SLOT(hideContextMenu()));
    addMouseTrans(on,off,this, QEvent::MouseButtonRelease, Qt::LeftButton, this, SLOT(hideContextMenu()));
    addMouseTrans(off,on,this, QEvent::MouseButtonPress, Qt::RightButton, this, SLOT(showContextMenu()));
    context->addState(off);
    context->addState(on);
    context->setInitialState(off);
    context->start();
}

/*****************************************************************************
 * slots appelant zoneDessin
 *****************************************************************************/
void MainWindow::setPenWidth(QAction* sender) {
    width->menuAction()->setText(sender->text());
    if(sender == width1) {zoneDessin->setPenWidth(1);}
    else if(sender == width2) {zoneDessin->setPenWidth(3);}
    else if(sender == width3) {zoneDessin->setPenWidth(10);}
}

void MainWindow::setPenColor(QAction* sender) {
    color->menuAction()->setText(sender->text());
    if(sender == color1) {zoneDessin->setPenColor(Qt::red);}
    else if(sender == color2) {zoneDessin->setPenColor(Qt::green);}
    else if(sender == color3) {zoneDessin->setPenColor(Qt::blue);;}
}

void MainWindow::setShape(QAction* sender) {
    if(sender == shape1) {zoneDessin->setShape(LINE);}
    else if(sender == shape2) {zoneDessin->setShape(RECTANGLE);}
    else if(sender == shape3) {zoneDessin->setShape(ELLIPSE);}
    else if(sender == shape4) {zoneDessin->setShape(POLYLINE);}
    else if(sender == select) {zoneDessin->setShape(SELECT);}
    else if(sender == move) {zoneDessin->setShape(MOVE);}
}

void MainWindow::setShape2(QAction* sender) {
    if(sender == ui->actionLine) {zoneDessin->setShape(LINE);}
    else if(sender == ui->actionRectangle) {zoneDessin->setShape(RECTANGLE);}
    else if(sender == ui->actionEllipse) {zoneDessin->setShape(ELLIPSE);}
    else if(sender == ui->actionPolyline) {zoneDessin->setShape(POLYLINE);}
    else if(sender == ui->actionSelect) {zoneDessin->setShape(SELECT);}
    else if(sender == ui->actionMove) {zoneDessin->setShape(MOVE);}

}

void MainWindow::setPenColor2() {
    QColor color = QColorDialog::getColor(Qt::black, this);
    zoneDessin->setPenColor(color);

    //on actualise l'icone avec la bonne couleur
    QPixmap px(33, 33);
    px.fill(color);
    ui->actionColor->setIcon(px);
}

void MainWindow::setPenWidth2() {
    zoneDessin->setPenWidth(ui->widthSlider->value());
}

/*****************************************************************************
 * Menu contextuel circulaire
 *****************************************************************************/

void MainWindow::showContextMenu() {
    //qDebug() << "affichage du menu contextuel";

    QPoint p=cursorPos(this);
    int r=80;
    contextMenu->setGeometry(p.x()-r,p.y()-r,2*r,2*r);
    contextMenu->show();
}

void MainWindow::hideContextMenu() {
    //qDebug()<<"hiding context menu";
    contextMenu->hide();
}
