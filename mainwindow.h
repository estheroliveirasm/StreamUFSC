#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include "movie.h"

namespace Ui {
class MainWindow;
}

QT_BEGIN_NAMESPACE
class QPushButton;
class QWidget;
class QVBoxLayout;
class QResizeEvent;
QT_END_NAMESPACE

// Janela principal do StreamUFSC.
// Mostra os filmes/séries em forma de "grade de pôsteres" (parecido com
// o site Letterboxd), permite filtrar, pesquisar, favoritar, marcar como
// assistido/quero assistir e adicionar ou remover itens.
// Os dados são salvos em um arquivo de texto (dados_filmes.txt).
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAdicionar();
    void onSobre();
    void onVoltarInicio();
    void onTextoPesquisaMudou(const QString &texto);

    void onFiltroTodos();
    void onFiltroFavoritos();
    void onFiltroMinhaLista();
    void onFiltroAssistindo();
    void onFiltroAssistidos();
    void onFiltroPopulares();

private:
    Ui::MainWindow *ui;

    QVector<Movie> movies;    // filmes/séries cadastrados pela usuária
    QVector<Movie> populares; // lista falsa de "populares do mês" (só para exibir)

    QString filtroAtual; // "Todos", "Favoritos", "Quero Assistir", "Assistindo", "Assistido", "Populares"
    QString tipoAtual;
    int colunasDaGrade() const;
    void resizeEvent(QResizeEvent *event) override;

    // ---- funções de tema / estilo ----
    void aplicarEstiloAzulMarinho();

    // ---- persistência (arquivo de texto) ----
    void carregarDados();
    void salvarDados();
    void popularListaDePopulares();
    QString caminhoArquivoDados() const;

    // ---- montagem da grade de pôsteres ----
    void atualizarGrade();
    // Monta a parte comum a todo cartão (pôster + título + estrelas) e
    // devolve, por "out param", o layout vertical do cartão para que quem
    // chamou possa adicionar os elementos específicos de cada tipo de
    // cartão (combo de status, botões, badge de "popular", etc.).
    QWidget *criarCardBase(const Movie &m, QVBoxLayout **layoutSaida);
    QWidget *criarCard(int indiceReal);       // card de um filme da lista da usuária
    QWidget *criarCardPopular(const Movie &m); // card (somente visual) da lista de populares
    void abrirDetalhes(const Movie &m);
    QString gerarTextoEstrelas(int nota) const;
    void atualizarEstatisticas();
    void marcarFiltroAtivo(QPushButton *ativo);

    // ---- ações feitas a partir de um card ----
    void favoritarFilme(int indice);
    void avaliarFilme(int indice);
    void removerFilme(int indice);
    void mudarStatusFilme(int indice, const QString &novoStatus);
};
#endif // MAINWINDOW_H
