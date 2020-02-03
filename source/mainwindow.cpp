#include "mainwindow.h"

MainWindow::MainWindow(const char *simulator,QWidget *parent) : QMainWindow(parent)
{   setFixedSize(500,500);
    setWindowTitle("Robot Simulation Environment");

    simulationWindow = new Dialog(simulator);
    simulationMenu = new QMenu(tr("&Simulation"),this);
    startSimulation = simulationMenu->addAction(tr("&Start Simulation"));
    endSimulation = simulationMenu->addAction(tr("&End Simulation"));
    spikingMenu = new QMenu(tr("&Spiking Network"),this);
    runNetwork = spikingMenu->addAction(tr("&Run"));
    connect(startSimulation, SIGNAL(triggered()), simulationWindow, SLOT(startSimulation()));
    connect(endSimulation, SIGNAL(triggered()), simulationWindow, SLOT(endSimulation()));
    connect(runNetwork, SIGNAL(triggered()), simulationWindow, SLOT(runNetwork()));
    menuBar = new QMenuBar();
    menuBar->addMenu(simulationMenu);
    menuBar->addMenu(spikingMenu);

    hBoxLayout = new QVBoxLayout();
    hBoxLayout->addWidget(simulationWindow);

    centralWidget = new QWidget();
    centralWidget->setLayout(hBoxLayout);
    setCentralWidget(centralWidget);

    setMenuBar(menuBar);

}

MainWindow::~MainWindow()
{
}




