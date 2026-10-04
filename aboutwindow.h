#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QDialog>

namespace Ui {
class AboutWindow;
}

// Janela "Sobre o Desenvolvedor".
// A interface (foto, nome, matrícula, etc.) é definida no arquivo
// aboutwindow.ui e pode ser editada visualmente no Qt Designer.
class AboutWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AboutWindow(QWidget *parent = nullptr);
    ~AboutWindow();

private:
    Ui::AboutWindow *ui;
};

#endif // ABOUTWINDOW_H
