#include "addmoviewindow.h"
#include "ui_addmoviewindow.h"
#include "theme.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QDate>

AddMovieWindow::AddMovieWindow(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddMovieWindow), posterPathEscolhido("")
{
    ui->setupUi(this);
    setFixedSize(360, 460);

    // Opções dos combos
    ui->generoCombo->addItems({"Ação", "Aventura", "Comédia", "Drama", "Fantasia",
                                "Ficção Científica", "Romance", "Terror", "Musical",
                                "Suspense", "Animação", "Documentário"});
    ui->tipoCombo->addItems({"Filme", "Série"});
    ui->statusCombo->addItems({"Quero Assistir", "Assistindo", "Assistido"});
    ui->anoSpin->setMaximum(QDate::currentDate().year() + 1);
    ui->anoSpin->setValue(QDate::currentDate().year());

    // Tema azul marinho escuro (igual ao resto do sistema)
    setStyleSheet(Theme::estiloFundoDialog() + Theme::estiloCamposDeEntrada());

    ui->posterPreview->setStyleSheet(
        QString("border: 2px dashed %1; background-color: %2; color: %3;")
            .arg(Theme::Dourado, Theme::FundoCard, Theme::TextoSuave));

    ui->posterBtn->setStyleSheet(Theme::estiloBotaoSecundario());
    ui->salvarBtn->setStyleSheet(Theme::estiloBotaoDestaque());
    ui->cancelarBtn->setStyleSheet(Theme::estiloBotaoSecundario());

    connect(ui->posterBtn, &QPushButton::clicked, this, &AddMovieWindow::onEscolherPoster);
    connect(ui->salvarBtn, &QPushButton::clicked, this, &AddMovieWindow::onSalvar);
    connect(ui->cancelarBtn, &QPushButton::clicked, this, &QDialog::reject);
}

AddMovieWindow::~AddMovieWindow()
{
    delete ui;
}

void AddMovieWindow::onEscolherPoster()
{
    QString arquivo = QFileDialog::getOpenFileName(
        this, "Escolher imagem do pôster", QString(),
        "Imagens (*.png *.jpg *.jpeg *.bmp)");

    if (!arquivo.isEmpty()) {
        posterPathEscolhido = arquivo;
        QPixmap pix(arquivo);
        ui->posterPreview->setPixmap(pix.scaled(100, 140, Qt::KeepAspectRatio,
                                                  Qt::SmoothTransformation));
    }
}

void AddMovieWindow::onSalvar()
{
    if (ui->tituloEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Campo obrigatório",
                              "Por favor, informe o título do filme ou série.");
        return;
    }

    accept(); // fecha a janela com resultado "Accepted"
}

Movie AddMovieWindow::getMovie() const
{
    return Movie(ui->tituloEdit->text().trimmed(),
                 ui->generoCombo->currentText().trimmed(),
                 ui->anoSpin->value(),
                 ui->tipoCombo->currentText(),
                 ui->statusCombo->currentText(),
                 0,                       // nota inicial (ainda não assistido)
                 false,                   // favorito inicial
                 posterPathEscolhido);    // pode ser vazio (sem pôster)
}
