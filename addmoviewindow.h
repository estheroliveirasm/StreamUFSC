#ifndef ADDMOVIEWINDOW_H
#define ADDMOVIEWINDOW_H

#include <QDialog>
#include "movie.h"

namespace Ui {
class AddMovieWindow;
}

// Janela para cadastrar um novo filme ou série.
// A interface é definida no arquivo addmoviewindow.ui (Qt Designer) e
// possui vários tipos de componentes: QLineEdit, QComboBox, QSpinBox,
// QLabel e QPushButton, atendendo ao requisito de "componentes variados".
class AddMovieWindow : public QDialog
{
    Q_OBJECT

public:
    explicit AddMovieWindow(QWidget *parent = nullptr);
    ~AddMovieWindow();

    // Retorna o filme montado com os dados preenchidos pelo usuário.
    Movie getMovie() const;

private slots:
    void onEscolherPoster();
    void onSalvar();

private:
    Ui::AddMovieWindow *ui;
    QString posterPathEscolhido;
};

#endif // ADDMOVIEWINDOW_H
