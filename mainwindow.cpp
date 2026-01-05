#include "mainwindow.h"
#include "dashboardwidget.h"
#include "./ui_mainwindow.h" // Archivo generado automáticamente por CMake
#include <QMessageBox>
#include <QSqlTableModel>
#include <QLayout>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->go, &QPushButton::clicked, this, &MainWindow::alPresionarBotonGo);

    // BUENA PRÁCTICA: Conexión moderna de señales y slots
    // Supongamos que en el Designer pusiste un botón llamado 'btnConectar'
    // connect(ui->btnConectar, &QPushButton::clicked, this, &MainWindow::alPresionarBotonConectar);
}

MainWindow::~MainWindow()
{
    delete ui; // Liberamos la memoria de la interfaz
}

void MainWindow::alPresionarBotonGo() {
    // Ejemplo de interacción rápida
    //QMessageBox::information(this, "Base de Datos", "Intentando refrescar datos...");
    MonthView *dashboard = new MonthView(this);
    this->setCentralWidget(dashboard);
    //this->layout()->setSizeConstraint(QLayout::SetDefaultConstraint);
    
    // CONEXIÓN CLAVE: Cuando la vista diga "volver", la MainWindow ejecuta "mostrarMenuPrincipal"
    connect(dashboard, &MonthView::backbutton_was_pressed, this, &MainWindow::showMainView);
    
    this->setCentralWidget(dashboard);
}

void MainWindow::showMainView() {
    // Para volver al estado inicial, lo más fácil es crear un widget 
    // que contenga lo que tenías antes, o simplemente limpiar:
    
    QWidget *contenedorPrincipal = new QWidget(this);
    // Aquí tendrías que volver a crear tus botones/layouts si no están en un .ui aparte
    // ...

    this->setCentralWidget(contenedorPrincipal); 
}

void MainWindow::configurarVistaDatos() {
    // Lógica para mostrar la tabla de la BD en un QTableView
    // Esto lo usaremos cuando tengas tu tabla creada
}