#include "aboutwindow.h"
#include "ui_aboutwindow.h"
#include "theme.h"

#include <QPixmap>

AboutWindow::AboutWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::AboutWindow)
{
    ui->setupUi(this);
    setFixedSize(340, 390);

    // Foto do desenvolvedor (arquivo me.jpg incluído nos recursos do projeto)
    QPixmap foto(":/images/me.jpg");
    if (!foto.isNull()) {
        QPixmap quadrada = foto.scaled(150, 150, Qt::KeepAspectRatioByExpanding,
                                      Qt::SmoothTransformation);
        const int x = (quadrada.width() - 150) / 2;
        const int y = (quadrada.height() - 150) / 2;
        ui->fotoLabel->setPixmap(quadrada.copy(x, y, 150, 150));
    }
    ui->fotoLabel->setAlignment(Qt::AlignCenter);
    ui->fotoLabel->setScaledContents(false);
    ui->layoutPrincipal->setAlignment(ui->fotoLabel, Qt::AlignHCenter);

    // Tema azul marinho escuro (igual ao resto do sistema)
    setStyleSheet(QString("background-color: %1;").arg(Theme::FundoEscuro));
    ui->fotoLabel->setStyleSheet(
        QString("border-radius: 75px; border: 3px solid %1;").arg(Theme::Dourado));
    ui->nomeLabel->setStyleSheet(Theme::estiloTituloDestaque(18));
    ui->linha->setStyleSheet(QString("color: %1;").arg(Theme::Borda));
    ui->sistemaLabel->setStyleSheet(Theme::estiloTextoSecundario(13));

    ui->fecharBtn->setStyleSheet(Theme::estiloBotaoDestaque());

    connect(ui->fecharBtn, &QPushButton::clicked, this, &QDialog::close);
}

AboutWindow::~AboutWindow()
{
    delete ui;
}
