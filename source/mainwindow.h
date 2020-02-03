#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include<spikingnetwork.h>

#include<QMainWindow>
#include<QPushButton>
#include<QVBoxLayout>
#include<QWidget>
#include<QMenuBar>
#include<QMenu>
#include<QAction>
#include<QObject>
#include<QGroupBox>
#include<QPainter>
#include<QPaintEvent>
#include<math.h>
#include<dialog.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(const char *simulator,QWidget *parent = 0);
    ~MainWindow();

private:
    QVBoxLayout * hBoxLayout;
    QGroupBox * groupBoxLayout;
    QWidget * centralWidget;
    QMenuBar * menuBar;
    QMenu * simulationMenu;
    QMenu * spikingMenu;
    QAction * startSimulation;
    QAction * endSimulation;
    QAction * runNetwork;
    Dialog * simulationWindow;

public slots:

};

#endif // MAINWINDOW_H
