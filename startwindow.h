#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QDialog>

namespace Ui {
class StartWindow;
}

// Tela inicial (splash) do programa.
// Mostra a foto da aluna e as informações do trabalho.
// O usuário aperta ENTER (ou clica no botão) para abrir a janela principal.
class StartWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);
    ~StartWindow();

protected:
    // Sobrescrevemos essa função para "escutar" quando o usuário aperta
    // uma tecla do teclado nesta janela.
    void keyPressEvent(QKeyEvent *evento) override;

private:
    Ui::StartWindow *ui;
};

#endif // STARTWINDOW_H
