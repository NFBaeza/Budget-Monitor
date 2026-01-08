#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <memory>

// Forward declaration de la clase generada por el archivo .ui
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT // Macro obligatoria para usar señales y slots

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Slot para manejar el clic de un botón (ejemplo)
    void alPresionarBotonGo();
    void showMainView();

private:
    Ui::MainWindow *ui; // Puntero a la interfaz

    // Aquí podrías añadir un método privado para configurar la tabla de la BD
    void configurarVistaDatos();
    void debugTabla();
};

#endif // MAINWINDOW_H