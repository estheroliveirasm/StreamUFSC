#include "startwindow.h"
#include "ui_startwindow.h"
#include "theme.h"

#include <QPixmap>
#include <QKeyEvent>

StartWindow::StartWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::StartWindow)
{
    ui->setupUi(this);
    setFixedSize(500, 620);

    // Coloca a foto da aluna (me.jpg) no centro da tela, em formato redondo
    QPixmap foto(":/images/me.jpg");
    ui->fotoLabel->setPixmap(foto.scaled(160, 160, Qt::KeepAspectRatioByExpanding,
                                          Qt::SmoothTransformation));
    ui->fotoLabel->setStyleSheet(
        QString("border-radius: 80px; border: 3px solid %1;").arg(Theme::Dourado));

    // Deixa tudo no tema azul marinho escuro
    setStyleSheet(Theme::estiloFundoDialog());

    ui->nomeSistemaLabel->setStyleSheet(Theme::estiloTituloDestaque(32));
    ui->subtituloLabel->setStyleSheet(Theme::estiloTextoSecundario(14));
    ui->infoTrabalhoLabel->setStyleSheet(Theme::estiloTextoSecundario(12));
    ui->alunaLabel->setStyleSheet(
        QString("font-size: 13px; font-weight: bold; color: %1; margin-top: 6px;")
            .arg(Theme::TextoClaro));

    ui->entrarBtn->setStyleSheet(Theme::estiloBotaoDestaque() + "font-size: 13px;");

    // Se o usuário clicar no botão, também entra no sistema
    connect(ui->entrarBtn, &QPushButton::clicked, this, &QDialog::accept);

    // Garante que a janela já comece com o "foco" nela, para o ENTER funcionar
    setFocus();
}

StartWindow::~StartWindow()
{
    delete ui;
}

// Essa função é chamada automaticamente pelo Qt toda vez que uma tecla
// é pressionada enquanto esta janela está aberta.
void StartWindow::keyPressEvent(QKeyEvent *evento)
{
    if (evento->key() == Qt::Key_Return || evento->key() == Qt::Key_Enter) {
        accept(); // fecha a janela inicial e libera a MainWindow para abrir
    } else {
        QDialog::keyPressEvent(evento); // comportamento padrão para outras teclas
    }
}
