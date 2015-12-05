#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <iostream>
#include <QFileDialog>
#include <QCloseEvent>
#include <QToolButton>
#include "zonedessin.h"
#include <QColorDialog>

//mettre a false pour utiliser l'implementation sans Qt Designer (début du projet, moins de fonctionnalités)
const bool USE_DESIGNER=true;

namespace Ui {
class MainWindow;
}

/*
 * Classe principale : fenetre contenant les différents menus et une zoneDessin
 */

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    //redefinition du closeEvent pour avoir le dialogue de confirmation meme en cliquant sur la croix
    void closeEvent(QCloseEvent *event) {event->ignore(); quitApp(); }

private:
    //Partie dessin
    ZoneDessin * zoneDessin;

    /*****************************************************************************
     * Implementation sans Qt Designer
     * note :   version du début du projet, obsolète avec moins des fonctionnalités, mais qui fonctionne
     *          déclarer WITH_DESIGNER=fase au début du fichier pour l'utiliser
     *****************************************************************************/
    QToolBar * toolBar;
    QAction * openAction, *saveAction, *quitAction; //FileMenu
    QAction * cancelLastAction, *cancelAllAction;

    //Actions de dessin
    QMenu *color, *width;
    QAction * shape1, *shape2, *shape3, *shape4, *select, *move;    // selection d'une forme
    QAction * width1, *width2, *width3;                             // selection d'une epaisseur de trait (parmi trois différentes)
    QAction * color1, *color2, *color3;                             // selection d'une couleur (parmi trois)

    //Fonctions pour initialiser les attributs (à appeler dans l'ordre)
    void addFileActions();              // menu File
    void addOutilsZoneDessin();         // initialise les shapex
    void addParamZoneDessin();          // initialise les width/color


    /*****************************************************************************
     * Implementation sans Qt Designer
     * note :   version plus complète et utilisée par défaut (WITH_DESIGNER=true)
     *****************************************************************************/
    Ui::MainWindow *ui;

    /*  comme il manque beaucoup de fonctionalités dans Qt Designer (par exemple les menus exclusifs),
        on les code à la main dans cette fonction, appelée après ui->setupUi(this) */
    void designerFix();


    /*****************************************************************************
     * Menu contextuel circulaire
     *****************************************************************************/
    QWidget * contextMenu;
    QToolButton *contextButton1, *contextButton2, *contextButton3, *contextButton4, *contextButton5, *contextButton6;

public slots:
    //slots pour les opérations du menu File
    void openFile();
    void saveFile();
    void quitApp();

    //slots gérant le menu contextuel
    void showContextMenu();
    void hideContextMenu();

    /* slots appelant les setters de zoneDessin (avec des noms explicites)
     * lorsqu'une méthode est déclarée une deuxième fois (nomDeMethode2()) il s'agit d'une adaptation pour Qt Designer */
    void setPenWidth(QAction* sender);
    void setPenWidth2();

    void setPenColor(QAction* sender);
    void setPenColor2();

    void setShape(QAction* sender);
    void setShape2(QAction* sender);

    void cancelLast() {zoneDessin->cancelLast();}
    void cancelAll() {zoneDessin->cancelAll();}
};

#endif // MAINWINDOW_H
