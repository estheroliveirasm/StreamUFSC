#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addmoviewindow.h"
#include "aboutwindow.h"
#include "startwindow.h"
#include "theme.h"

#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QMessageBox>
#include <QInputDialog>
#include <QPixmap>
#include <QTimer>
#include <QStandardPaths>
#include <QDir>
#include <QFileInfo>
#include <QResizeEvent>
#include <QUuid>
#include <QSettings>
#include <QDialog>
#include <QIcon>
#include <QFrame>

// Quantidade de colunas usada na grade de pôsteres.
// Deixamos um número fixo para simplificar o código (versão amadora).
static const int LARGURA_CARD = 196;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), filtroAtual("Todos"), tipoAtual("Todos")
{
    ui->setupUi(this);
    resize(1100, 700);
    ui->layoutPrincipal->setContentsMargins(22, 18, 22, 16);
    ui->layoutPrincipal->setSpacing(14);
    ui->layoutFiltros->setSpacing(8);
    ui->gradeLayout->setContentsMargins(12, 12, 12, 18);
    ui->gradeLayout->setHorizontalSpacing(14);
    ui->gradeLayout->setVerticalSpacing(16);

    // Conecta os botões de filtro (funcionam como "abas" clicáveis)
    connect(ui->btnTodos, &QPushButton::clicked, this, &MainWindow::onFiltroTodos);
    connect(ui->btnFavoritos, &QPushButton::clicked, this, &MainWindow::onFiltroFavoritos);
    connect(ui->btnMinhaLista, &QPushButton::clicked, this, &MainWindow::onFiltroMinhaLista);
    connect(ui->btnAssistindo, &QPushButton::clicked, this, &MainWindow::onFiltroAssistindo);
    connect(ui->btnAssistidos, &QPushButton::clicked, this, &MainWindow::onFiltroAssistidos);
    connect(ui->btnPopulares, &QPushButton::clicked, this, &MainWindow::onFiltroPopulares);

    connect(ui->btnAdicionar, &QPushButton::clicked, this, &MainWindow::onAdicionar);
    connect(ui->btnSobre, &QPushButton::clicked, this, &MainWindow::onSobre);
    connect(ui->btnVoltar, &QPushButton::clicked, this, &MainWindow::onVoltarInicio);
    connect(ui->pesquisaEdit, &QLineEdit::textChanged, this, &MainWindow::onTextoPesquisaMudou);
    auto *tipoCombo = new QComboBox(this);
    tipoCombo->addItems({"Todos os tipos", "Filmes", "Séries"});
    tipoCombo->setToolTip("Filtrar por filme ou série");
    ui->layoutCabecalho->insertWidget(3, tipoCombo);
    connect(tipoCombo, &QComboBox::currentTextChanged, this, [this](const QString &texto) {
        tipoAtual = texto == "Filmes" ? "Filme" : (texto == "Séries" ? "Série" : "Todos");
        atualizarGrade();
    });

    aplicarEstiloAzulMarinho();
    popularListaDePopulares();
    carregarDados();
    atualizarGrade();
}

MainWindow::~MainWindow()
{
    salvarDados();
    delete ui;
}

int MainWindow::colunasDaGrade() const
{
    const int largura = ui->scrollArea->viewport()->width();
    return qMax(1, largura / LARGURA_CARD);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    if (ui && ui->gradeLayout)
        atualizarGrade();
}

// ---------------------------------------------------------------
// Tema visual: azul marinho escuro
// ---------------------------------------------------------------
void MainWindow::aplicarEstiloAzulMarinho()
{
    setStyleSheet(Theme::estiloFundoMainWindow() + Theme::estiloCamposDeEntrada());

    ui->tituloLabel->setStyleSheet(Theme::estiloTituloDestaque(24));
    ui->statsLabel->setStyleSheet(Theme::estiloTextoSecundario(12) + "padding: 6px 2px;");

    ui->btnAdicionar->setStyleSheet(Theme::estiloBotaoDestaque());
    ui->btnSobre->setStyleSheet(Theme::estiloBotaoSecundario());
    ui->btnVoltar->setStyleSheet(QString(
        "QPushButton { background: %1; color: %2; border: 1px solid %3; border-radius: 10px; "
        "font-size: 22px; font-weight: bold; }"
        "QPushButton:hover { background: %3; color: %4; }")
        .arg(Theme::FundoCard, Theme::TextoClaro, Theme::Borda, Theme::Dourado));

    for (QPushButton *b : {ui->btnTodos, ui->btnFavoritos, ui->btnMinhaLista,
                            ui->btnAssistindo, ui->btnAssistidos, ui->btnPopulares}) {
        b->setStyleSheet(Theme::estiloBotaoFiltro());
    }
}

// ---------------------------------------------------------------
// Persistência de dados (arquivo de texto)
// ---------------------------------------------------------------
QString MainWindow::caminhoArquivoDados() const
{
    QString diretorio = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(diretorio);
    return diretorio + "/dados_filmes.txt";
}

void MainWindow::carregarDados()
{
    QSettings configuracoes;
    if (!configuracoes.value("colecaoVaziaInicializada", false).toBool()) {
        // A primeira inicialização da versão limpa qualquer coleção de
        // demonstração antiga para deixar a experiência realmente zerada.
        movies.clear();
        salvarDados();
        configuracoes.setValue("colecaoVaziaInicializada", true);
        return;
    }

    QFile arquivo(caminhoArquivoDados());

    if (!arquivo.exists()) {
        const QString legado = QCoreApplication::applicationDirPath() + "/dados_filmes.txt";
        if (QFileInfo::exists(legado)) {
            QFile::copy(legado, arquivo.fileName());
            arquivo.setFileName(caminhoArquivoDados());
        }
    }

    if (!arquivo.exists()) {
        // A coleção pessoal começa vazia; os títulos ficam no catálogo.
        salvarDados();
        return;
    }

    if (!arquivo.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Aviso",
                              "Não foi possível abrir o arquivo de dados. "
                              "Uma lista vazia será usada.");
        return;
    }

    QTextStream in(&arquivo);
    while (!in.atEnd()) {
        QString linha = in.readLine();
        if (linha.trimmed().isEmpty())
            continue;
        Movie m = Movie::fromFileLine(linha);
        if (!m.getTitle().isEmpty())
            movies.append(m);
    }
    arquivo.close();
}

void MainWindow::salvarDados()
{
    QFile arquivo(caminhoArquivoDados());
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Aviso",
                              "Não foi possível salvar o arquivo de dados.");
        return;
    }

    QTextStream out(&arquivo);
    for (const Movie &m : movies) {
        out << m.toFileLine() << "\n";
    }
    arquivo.close();
}

// Catálogo de títulos com pôsteres incluídos no projeto. Não entram na
// coleção pessoal e começam sem nota ou favorito.
void MainWindow::popularListaDePopulares()
{
    populares.append(Movie("Stranger Things", "Ficção Científica", 2016, "Série", "Catálogo", 0, false,
                            ":/images/posters/Stranger_Things.jpg"));
    populares.append(Movie("Wednesday", "Comédia de Terror", 2022, "Série", "Catálogo", 0, false,
                            ":/images/posters/Wednesday.jpg"));
    populares.append(Movie("Jogos Vorazes", "Ação", 2012, "Filme", "Catálogo", 0, false,
                            ":/images/posters/The_hunger_games.jpg"));
    populares.append(Movie("It: Capítulo Dois", "Terror", 2019, "Filme", "Catálogo", 0, false,
                            ":/images/posters/IT.jpg"));
    populares.append(Movie("Harry Potter e a Pedra Filosofal", "Fantasia", 2001, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Harry-Potter.jpg"));
    populares.append(Movie("Pânico", "Terror", 1996, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Scream.jpg"));
    populares.append(Movie("Piratas do Caribe: Navegando em Águas Misteriosas", "Aventura", 2011, "Filme",
                            "Catálogo", 0, false, ":/images/posters/Pirates_of_the_Caribbean.jpg"));
    populares.append(Movie("Mamma Mia!", "Musical", 2008, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Mamma_Mia.jpg"));
    populares.append(Movie("Gossip Girl", "Drama", 2007, "Série", "Catálogo", 0, false,
                            ":/images/posters/Gossip_Girl.jpg"));
    populares.append(Movie("The Big Bang Theory", "Comédia", 2007, "Série", "Catálogo", 0, false,
                            ":/images/posters/The_Bigbang_Theory.jpg"));
    populares.append(Movie("High School Musical", "Musical", 2006, "Filme", "Catálogo", 0, false,
                            ":/images/posters/High_School_Musical.jpg"));
    populares.append(Movie("Malévola", "Fantasia", 2014, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Maleficent.jpg"));
    populares.append(Movie("Meninas Malvadas", "Comédia", 2004, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Mean_Girls.jpg"));
    populares.append(Movie("As Crônicas de Nárnia", "Fantasia", 2005, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Narnia.jpg"));
    populares.append(Movie("Um Lugar Chamado Notting Hill", "Romance", 1999, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Nothing_Hill.jpg"));
    populares.append(Movie("Uma Linda Mulher", "Romance", 1990, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Pretty_Woman.jpg"));
    populares.append(Movie("Scooby-Doo", "Comédia", 2002, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Scooby-Doo.jpg"));
    populares.append(Movie("Duas Vidas, Uma Só Filha", "Comédia", 1998, "Filme", "Catálogo", 0, false,
                            ":/images/posters/The_Parent_Trap.jpg"));
    populares.append(Movie("Crepúsculo", "Romance", 2008, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Twilight.jpg"));
    populares.append(Movie("Diário de uma Paixão", "Romance", 2004, "Filme", "Catálogo", 0, false,
                            ":/images/posters/_3.jpg"));
    populares.append(Movie("The Summer I Turned Pretty", "Romance", 2022, "Série", "Catálogo", 0, false,
                            ":/images/posters/Conrad, Belly e Jeremiah.jpg"));
    populares.append(Movie("The Mentalist", "Policial", 2008, "Série", "Catálogo", 0, false,
                            ":/images/posters/download (1).jpg"));
    populares.append(Movie("10 Coisas que Eu Odeio em Você", "Romance", 1999, "Filme", "Catálogo", 0, false,
                            ":/images/posters/download (2).jpg"));
    populares.append(Movie("Legalmente Loira", "Comédia", 2001, "Filme", "Catálogo", 0, false,
                            ":/images/posters/download (3).jpg"));
    populares.append(Movie("Como Eu Era Antes de Você", "Drama", 2016, "Filme", "Catálogo", 0, false,
                            ":/images/posters/download (4).jpg"));
    populares.append(Movie("Sr. e Sra. Smith", "Ação", 2005, "Filme", "Catálogo", 0, false,
                            ":/images/posters/download (5).jpg"));
    populares.append(Movie("Todos Menos Você", "Romance", 2023, "Filme", "Catálogo", 0, false,
                            ":/images/posters/download (6).jpg"));
    populares.append(Movie("O Rei do Show", "Musical", 2017, "Filme", "Catálogo", 0, false,
                            ":/images/posters/download (7).jpg"));
    populares.append(Movie("Pretty Little Liars", "Drama", 2010, "Série", "Catálogo", 0, false,
                            ":/images/posters/download.jpg"));
    populares.append(Movie("La La Land: Cantando Estações", "Musical", 2016, "Filme", "Catálogo", 0, false,
                            ":/images/posters/La La Land - 2016.jpg"));
    populares.append(Movie("Como Perder um Homem em 10 Dias", "Romance", 2003, "Filme", "Catálogo", 0, false,
                            ":/images/posters/Paty'sDreams.jpg"));
    populares.append(Movie("Teen Wolf", "Fantasia", 2011, "Série", "Catálogo", 0, false,
                            ":/images/posters/Teen Wolf.jpg"));
    populares.append(Movie("O Diabo Veste Prada", "Comédia", 2006, "Filme", "Catálogo", 0, false,
                            ":/images/posters/The Devil Wears Prada (2006).jpg"));
    populares.append(Movie("The Vampire Diaries", "Drama", 2009, "Série", "Catálogo", 0, false,
                            ":/images/posters/the vampire diaries.jpg"));
}

// ---------------------------------------------------------------
// Monta a grade de pôsteres, de acordo com o filtro e a pesquisa atuais
// ---------------------------------------------------------------
void MainWindow::atualizarGrade()
{
    // Primeiro removemos todos os cartões que já estavam na grade
    QLayoutItem *item;
    while ((item = ui->gradeLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    QString textoPesquisa = ui->pesquisaEdit->text().trimmed().toCaseFolded();
    const auto correspondePesquisa = [&textoPesquisa](const Movie &m) {
        if (textoPesquisa.isEmpty()) return true;
        const QString indice = QStringLiteral("%1 %2 %3 %4")
            .arg(m.getTitle(), m.getGenre(), m.getType()).arg(m.getYear()).toCaseFolded();
        return indice.contains(textoPesquisa);
    };
    int linha = 0;
    int coluna = 0;
    int quantidadeMostrada = 0;

    if (filtroAtual == "Populares") {
        // Mostra a lista falsa de populares do mês (não é a coleção real)
        for (const Movie &m : populares) {
            if (tipoAtual != "Todos" && m.getType() != tipoAtual)
                continue;
            if (!correspondePesquisa(m))
                continue;

            QWidget *card = criarCardPopular(m);
            ui->gradeLayout->addWidget(card, linha, coluna);
            quantidadeMostrada++;

            coluna++;
            if (coluna >= colunasDaGrade()) {
                coluna = 0;
                linha++;
            }
        }
    } else {
        // Mostra a coleção real da usuária, filtrada pela aba selecionada
        for (int i = 0; i < movies.size(); ++i) {
            const Movie &m = movies.at(i);

            bool passaFiltro = false;
            if (filtroAtual == "Todos") {
                passaFiltro = true;
            } else if (filtroAtual == "Favoritos") {
                passaFiltro = m.isFavorite();
            } else {
                passaFiltro = (m.getStatus() == filtroAtual);
            }

            if (!passaFiltro)
                continue;

            if (tipoAtual != "Todos" && m.getType() != tipoAtual)
                continue;
            if (!correspondePesquisa(m))
                continue;

            QWidget *card = criarCard(i);
            ui->gradeLayout->addWidget(card, linha, coluna);
            quantidadeMostrada++;

            coluna++;
            if (coluna >= colunasDaGrade()) {
                coluna = 0;
                linha++;
            }
        }
    }

    // Se não encontrou nada, mostra uma mensagem no lugar da grade
    if (quantidadeMostrada == 0) {
        QLabel *vazio = new QLabel(filtroAtual == "Todos" && movies.isEmpty()
                                       ? "Sua coleção está vazia. Explore o Catálogo para começar."
                                       : "Nenhum filme ou série encontrado aqui.");
        vazio->setStyleSheet(Theme::estiloTextoSecundario(14) + "padding: 30px;");
        vazio->setAlignment(Qt::AlignCenter);
        ui->gradeLayout->addWidget(vazio, 0, 0, 1, colunasDaGrade());
    }

    atualizarEstatisticas();
}

// Cria o texto de estrelas (ex: "★★★★☆") a partir de uma nota de 0 a 5
QString MainWindow::gerarTextoEstrelas(int nota) const
{
    QString texto;
    for (int i = 0; i < 5; ++i) {
        if (i < nota)
            texto += "★";
        else
            texto += "☆";
    }
    return texto;
}

void MainWindow::atualizarEstatisticas()
{
    int total = movies.size();
    int assistidos = 0;
    int favoritos = 0;
    int minhaLista = 0;

    for (const Movie &m : movies) {
        if (m.getStatus() == "Assistido") assistidos++;
        if (m.isFavorite()) favoritos++;
        if (m.getStatus() == "Quero Assistir") minhaLista++;
    }

    ui->statsLabel->setText(QString("Total cadastrados: %1   |   Assistidos: %2   |   "
                                     "Favoritos: %3   |   Minha Lista: %4")
                                 .arg(total).arg(assistidos).arg(favoritos).arg(minhaLista));
}

void MainWindow::marcarFiltroAtivo(QPushButton *ativo)
{
    for (QPushButton *b : {ui->btnTodos, ui->btnFavoritos, ui->btnMinhaLista,
                            ui->btnAssistindo, ui->btnAssistidos, ui->btnPopulares}) {
        b->setChecked(b == ativo);
    }
}

// ---------------------------------------------------------------
// Parte comum a qualquer cartão da grade: pôster + título + estrelas.
// Tanto o cartão interativo (criarCard) quanto o cartão só-visual da
// aba "Populares" (criarCardPopular) partem daqui, evitando duplicar
// esse bloco de código nas duas funções.
// ---------------------------------------------------------------
QWidget *MainWindow::criarCardBase(const Movie &m, QVBoxLayout **layoutSaida)
{
    QWidget *card = new QWidget();
    card->setFixedWidth(LARGURA_CARD);
    card->setObjectName("movieCard");
    card->setStyleSheet(QString(
        "QWidget#movieCard { background-color: %1; border: 1px solid %2; border-radius: 12px; }"
        "QWidget#movieCard:hover { border-color: %3; }")
        .arg(Theme::FundoCard, Theme::Borda, Theme::Dourado));

    QVBoxLayout *layoutCard = new QVBoxLayout(card);
    layoutCard->setContentsMargins(8, 8, 8, 8);

    // ---- pôster ----
    QPushButton *posterLabel = new QPushButton();
    posterLabel->setFixedSize(LARGURA_CARD - 20, 240);
    posterLabel->setFlat(true);
    posterLabel->setCursor(Qt::PointingHandCursor);
    posterLabel->setToolTip("Abrir detalhes");
    posterLabel->setStyleSheet(QString(
        "QPushButton { border-radius: 8px; background-color: %1; padding: 0; border: none; }"
        "QPushButton:hover { border: 2px solid %2; }").arg(Theme::FundoEscuro, Theme::Dourado));
    bool posterCarregado = false;
    if (!m.getPosterPath().isEmpty()) {
        QPixmap pix(m.getPosterPath());
        if (!pix.isNull()) {
            const QSize tamanho = posterLabel->size();
            const QPixmap ampliada = pix.scaled(tamanho, Qt::KeepAspectRatioByExpanding,
                                                Qt::SmoothTransformation);
            const QPixmap recortada = ampliada.copy((ampliada.width() - tamanho.width()) / 2,
                                                    (ampliada.height() - tamanho.height()) / 2,
                                                    tamanho.width(), tamanho.height());
            posterLabel->setIcon(QIcon(recortada));
            posterLabel->setIconSize(tamanho);
            posterCarregado = true;
        }
    }
    if (!posterCarregado) {
        posterLabel->setText("🎬  Sem pôster");
    }
    connect(posterLabel, &QPushButton::clicked, this, [this, m]() { abrirDetalhes(m); });
    layoutCard->addWidget(posterLabel);

    // ---- título ----
    QLabel *tituloLabel = new QLabel(m.getTitle());
    tituloLabel->setWordWrap(true);
    tituloLabel->setStyleSheet(QString("font-weight: bold; font-size: 12px; color: %1;").arg(Theme::TextoClaro));
    tituloLabel->setFixedHeight(36);
    layoutCard->addWidget(tituloLabel);

    QLabel *detalhesLabel = new QLabel(QString("%1 · %2 · %3")
        .arg(m.getType(), m.getGenre(), QString::number(m.getYear())));
    detalhesLabel->setWordWrap(true);
    detalhesLabel->setStyleSheet(QString("font-size: 10px; color: %1;").arg(Theme::TextoSuave));
    layoutCard->addWidget(detalhesLabel);

    // ---- estrelas (nota) ----
    QLabel *estrelasLabel = new QLabel(m.getRating() > 0 ? gerarTextoEstrelas(m.getRating()) : "Sem avaliação");
    estrelasLabel->setStyleSheet(QString("color: %1; font-size: 14px;").arg(Theme::Dourado));
    layoutCard->addWidget(estrelasLabel);

    if (layoutSaida)
        *layoutSaida = layoutCard;
    return card;
}

// ---------------------------------------------------------------
// Cartão de um filme/série da coleção da usuária (interativo)
// ---------------------------------------------------------------
QWidget *MainWindow::criarCard(int indiceReal)
{
    const Movie &m = movies.at(indiceReal);

    QVBoxLayout *layoutCard = nullptr;
    QWidget *card = criarCardBase(m, &layoutCard);

    // ---- status (combo box) ----
    QComboBox *statusCombo = new QComboBox();
    statusCombo->addItems({"Quero Assistir", "Assistindo", "Assistido"});
    statusCombo->setCurrentText(m.getStatus());
    connect(statusCombo, &QComboBox::currentTextChanged, this,
            [this, indiceReal](const QString &novoStatus) {
                mudarStatusFilme(indiceReal, novoStatus);
            });
    layoutCard->addWidget(statusCombo);

    // ---- linha de botões: favoritar / avaliar / remover ----
    QHBoxLayout *linhaBotoes = new QHBoxLayout();

    QPushButton *coracaoBtn = new QPushButton(m.isFavorite() ? "♥" : "♡");
    coracaoBtn->setToolTip("Favoritar / Desfavoritar");
    QString corCoracao = m.isFavorite() ? Theme::Vermelho : Theme::TextoSuave;
    coracaoBtn->setStyleSheet(QString(
        "QPushButton { background: transparent; color: %1; font-size: 16px; border: none; }"
        "QPushButton:hover { color: %2; }").arg(corCoracao, Theme::Vermelho));
    connect(coracaoBtn, &QPushButton::clicked, this, [this, indiceReal]() {
        favoritarFilme(indiceReal);
    });

    QPushButton *avaliarBtn = new QPushButton("⭐");
    avaliarBtn->setToolTip("Avaliar");
    avaliarBtn->setStyleSheet(QString(
        "QPushButton { background: transparent; color: %1; font-size: 16px; border: none; }"
        "QPushButton:hover { color: %2; }").arg(Theme::Dourado, Theme::DouradoHover));
    connect(avaliarBtn, &QPushButton::clicked, this, [this, indiceReal]() {
        avaliarFilme(indiceReal);
    });

    QPushButton *removerBtn = new QPushButton("🗑");
    removerBtn->setToolTip("Remover da lista");
    removerBtn->setStyleSheet(QString(
        "QPushButton { background: transparent; color: %1; font-size: 14px; border: none; }"
        "QPushButton:hover { color: %2; }").arg(Theme::TextoSuave, Theme::Vermelho));
    connect(removerBtn, &QPushButton::clicked, this, [this, indiceReal]() {
        removerFilme(indiceReal);
    });

    linhaBotoes->addWidget(coracaoBtn);
    linhaBotoes->addWidget(avaliarBtn);
    linhaBotoes->addStretch();
    linhaBotoes->addWidget(removerBtn);

    layoutCard->addLayout(linhaBotoes);

    return card;
}

// ---------------------------------------------------------------
// Cartão da lista de "populares do mês" (apenas visual, não interativo)
// ---------------------------------------------------------------
QWidget *MainWindow::criarCardPopular(const Movie &m)
{
    QVBoxLayout *layoutCard = nullptr;
    QWidget *card = criarCardBase(m, &layoutCard);

    QLabel *badgeLabel = new QLabel("🎬 Disponível no catálogo");
    badgeLabel->setStyleSheet(QString(
        "background-color: %1; color: %2; font-size: 10px; font-weight: bold; "
        "padding: 3px 6px; border-radius: 8px;").arg(Theme::Dourado, Theme::FundoEscuro));
    badgeLabel->setAlignment(Qt::AlignCenter);
    layoutCard->addWidget(badgeLabel);

    QPushButton *adicionarBtn = new QPushButton("＋ Adicionar");
    adicionarBtn->setStyleSheet(Theme::estiloBotaoSecundario());
    connect(adicionarBtn, &QPushButton::clicked, this, [this, m]() {
        for (const Movie &existente : movies) {
            if (existente.getTitle().compare(m.getTitle(), Qt::CaseInsensitive) == 0) {
                QMessageBox::information(this, "Já está na coleção",
                                         "Este título já está na sua coleção.");
                return;
            }
        }
        Movie novo = m;
        novo.setStatus("Quero Assistir");
        movies.append(novo);
        salvarDados();
        filtroAtual = "Todos";
        marcarFiltroAtivo(ui->btnTodos);
        atualizarGrade();
    });
    layoutCard->addWidget(adicionarBtn);

    return card;
}

void MainWindow::abrirDetalhes(const Movie &m)
{
    QDialog dialog(this);
    dialog.setWindowTitle(QString("StreamUFSC · %1").arg(m.getTitle()));
    dialog.setMinimumSize(650, 450);
    dialog.resize(700, 470);
    dialog.setStyleSheet(Theme::estiloFundoDialog());

    auto *principal = new QHBoxLayout(&dialog);
    principal->setContentsMargins(24, 22, 24, 22);
    principal->setSpacing(24);

    auto *poster = new QLabel;
    poster->setFixedSize(250, 370);
    poster->setAlignment(Qt::AlignCenter);
    poster->setStyleSheet(QString("background: %1; border: 1px solid %2; border-radius: 12px;")
                              .arg(Theme::FundoCard, Theme::Borda));
    const QPixmap imagem(m.getPosterPath());
    if (!imagem.isNull()) {
        const QSize tamanho = poster->size();
        const QPixmap ampliada = imagem.scaled(tamanho, Qt::KeepAspectRatioByExpanding,
                                               Qt::SmoothTransformation);
        poster->setPixmap(ampliada.copy((ampliada.width() - tamanho.width()) / 2,
                                        (ampliada.height() - tamanho.height()) / 2,
                                        tamanho.width(), tamanho.height()));
    } else {
        poster->setText("🎬\nPôster indisponível");
    }
    principal->addWidget(poster, 0, Qt::AlignVCenter);

    auto *informacoes = new QVBoxLayout;
    informacoes->setSpacing(12);
    auto *voltarBtn = new QPushButton("←  Voltar");
    voltarBtn->setCursor(Qt::PointingHandCursor);
    voltarBtn->setStyleSheet(Theme::estiloBotaoSecundario());
    connect(voltarBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    informacoes->addWidget(voltarBtn, 0, Qt::AlignLeft);

    auto *titulo = new QLabel(m.getTitle());
    titulo->setWordWrap(true);
    titulo->setStyleSheet(Theme::estiloTituloDestaque(23));
    informacoes->addWidget(titulo);

    auto *subtitulo = new QLabel(QString("%1  ·  %2  ·  %3")
                                     .arg(m.getType(), QString::number(m.getYear()), m.getGenre()));
    subtitulo->setWordWrap(true);
    subtitulo->setStyleSheet(Theme::estiloTextoSecundario(13));
    informacoes->addWidget(subtitulo);

    auto *linha = new QFrame;
    linha->setFrameShape(QFrame::HLine);
    linha->setStyleSheet(QString("color: %1;").arg(Theme::Borda));
    informacoes->addWidget(linha);

    const QString status = m.getStatus() == "Catálogo" ? "Ainda não está na sua coleção" : m.getStatus();
    auto *statusLabel = new QLabel(QString("Status\n%1").arg(status));
    statusLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(Theme::TextoClaro));
    informacoes->addWidget(statusLabel);

    auto *notaLabel = new QLabel(m.getRating() > 0
                                     ? QString("Avaliação\n%1 / 5  %2")
                                           .arg(m.getRating()).arg(gerarTextoEstrelas(m.getRating()))
                                     : QStringLiteral("Avaliação\nAinda não avaliado"));
    notaLabel->setStyleSheet(QString("font-size: 13px; color: %1;").arg(Theme::TextoClaro));
    informacoes->addWidget(notaLabel);

    if (m.getStatus() == "Catálogo") {
        auto *adicionarBtn = new QPushButton("＋  Adicionar à minha coleção");
        adicionarBtn->setCursor(Qt::PointingHandCursor);
        adicionarBtn->setStyleSheet(Theme::estiloBotaoDestaque());
        connect(adicionarBtn, &QPushButton::clicked, &dialog, [this, m, &dialog]() {
            for (const Movie &existente : movies) {
                if (existente.getTitle().compare(m.getTitle(), Qt::CaseInsensitive) == 0) {
                    QMessageBox::information(&dialog, "Já está na coleção",
                                             "Este título já está na sua coleção.");
                    return;
                }
            }
            Movie novo = m;
            novo.setStatus("Quero Assistir");
            movies.append(novo);
            salvarDados();
            atualizarGrade();
            dialog.accept();
        });
        informacoes->addWidget(adicionarBtn);
    }

    informacoes->addStretch();
    principal->addLayout(informacoes, 1);
    dialog.exec();
}

// ---------------------------------------------------------------
// Ações disparadas a partir dos botões de cada cartão
// ---------------------------------------------------------------
void MainWindow::favoritarFilme(int indice)
{
    if (indice < 0 || indice >= movies.size())
        return;

    movies[indice].setFavorite(!movies[indice].isFavorite());
    salvarDados();

    // Atualizamos a grade "depois", com um pequeno atraso de 0 milissegundos.
    // Isso evita problemas, já que o botão clicado está dentro do cartão
    // que será destruído e recriado.
    QTimer::singleShot(0, this, &MainWindow::atualizarGrade);
}

void MainWindow::avaliarFilme(int indice)
{
    if (indice < 0 || indice >= movies.size())
        return;

    bool ok = false;
    int nota = QInputDialog::getInt(this, "Avaliar",
                                     "Qual nota você dá para este título? (0 a 5)",
                                     movies.at(indice).getRating(), 0, 5, 1, &ok);
    if (!ok)
        return;

    movies[indice].setRating(nota);
    movies[indice].setStatus("Assistido");
    salvarDados();

    QTimer::singleShot(0, this, &MainWindow::atualizarGrade);
}

void MainWindow::removerFilme(int indice)
{
    if (indice < 0 || indice >= movies.size())
        return;

    QString titulo = movies.at(indice).getTitle();
    auto resposta = QMessageBox::question(this, "Confirmar remoção",
                                           QString("Deseja remover \"%1\" da sua lista?").arg(titulo));
    if (resposta != QMessageBox::Yes)
        return;

    movies.removeAt(indice);
    salvarDados();

    QTimer::singleShot(0, this, &MainWindow::atualizarGrade);
}

void MainWindow::mudarStatusFilme(int indice, const QString &novoStatus)
{
    if (indice < 0 || indice >= movies.size())
        return;

    movies[indice].setStatus(novoStatus);
    salvarDados();

    QTimer::singleShot(0, this, &MainWindow::atualizarGrade);
}

// ---------------------------------------------------------------
// Slots dos filtros (abas clicáveis)
// ---------------------------------------------------------------
void MainWindow::onFiltroTodos()
{
    filtroAtual = "Todos";
    marcarFiltroAtivo(ui->btnTodos);
    atualizarGrade();
}

void MainWindow::onFiltroFavoritos()
{
    filtroAtual = "Favoritos";
    marcarFiltroAtivo(ui->btnFavoritos);
    atualizarGrade();
}

void MainWindow::onFiltroMinhaLista()
{
    filtroAtual = "Quero Assistir";
    marcarFiltroAtivo(ui->btnMinhaLista);
    atualizarGrade();
}

void MainWindow::onFiltroAssistindo()
{
    filtroAtual = "Assistindo";
    marcarFiltroAtivo(ui->btnAssistindo);
    atualizarGrade();
}

void MainWindow::onFiltroAssistidos()
{
    filtroAtual = "Assistido";
    marcarFiltroAtivo(ui->btnAssistidos);
    atualizarGrade();
}

void MainWindow::onFiltroPopulares()
{
    filtroAtual = "Populares";
    marcarFiltroAtivo(ui->btnPopulares);
    atualizarGrade();
}

void MainWindow::onTextoPesquisaMudou(const QString &texto)
{
    Q_UNUSED(texto);
    atualizarGrade();
}

// ---------------------------------------------------------------
// Botões do cabeçalho
// ---------------------------------------------------------------
void MainWindow::onAdicionar()
{
    AddMovieWindow janela(this);
    if (janela.exec() == QDialog::Accepted) {
        Movie novo = janela.getMovie();
        const QString posterOriginal = novo.getPosterPath();
        if (!posterOriginal.isEmpty() && !posterOriginal.startsWith(":/")) {
            const QFileInfo info(posterOriginal);
            const QString pastaPosteres = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/posters";
            QDir().mkpath(pastaPosteres);
            const QString nome = QUuid::createUuid().toString(QUuid::WithoutBraces) + "." + info.suffix();
            const QString destino = pastaPosteres + "/" + nome;
            if (QFile::copy(posterOriginal, destino))
                novo.setPosterPath(destino);
        }
        movies.append(novo);
        salvarDados();
        atualizarGrade();
    }
}

void MainWindow::onSobre()
{
    AboutWindow janela(this);
    janela.exec();
}

void MainWindow::onVoltarInicio()
{
    hide();
    StartWindow telaInicial(this);
    if (telaInicial.exec() == QDialog::Accepted)
        show();
    else
        close();
}
