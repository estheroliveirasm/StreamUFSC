#ifndef MOVIE_H
#define MOVIE_H

#include <QString>

// Classe que representa um filme ou serie cadastrado no StreamUFSC.
// E uma classe simples (encapsulamento com getters/setters) para
// atender ao requisito de Orientacao a Objetos do trabalho.
class Movie
{
public:
    Movie();
    Movie(const QString &title, const QString &genre, int year,
          const QString &type, const QString &status, int rating,
          bool favorite, const QString &posterPath);

    // ---- Getters ----
    QString getTitle() const;
    QString getGenre() const;
    int getYear() const;
    QString getType() const;      // "Filme" ou "Serie"
    QString getStatus() const;    // "Quero Assistir", "Assistindo", "Assistido"
    int getRating() const;        // 0 a 5 (sempre dentro desse intervalo)
    bool isFavorite() const;
    QString getPosterPath() const;

    // ---- Setters ----
    void setTitle(const QString &value);
    void setGenre(const QString &value);
    void setYear(int value);
    void setType(const QString &value);
    void setStatus(const QString &value);
    void setRating(int value);
    void setFavorite(bool value);
    void setPosterPath(const QString &value);

    // Converte o filme em uma linha de texto para salvar no arquivo.
    // Os campos são separados por um caractere invisível (US, 0x1F) em vez
    // de "|", para que títulos/gêneros que contenham "|" não quebrem o
    // arquivo. Campos com quebra de linha são sanitizados antes de salvar.
    QString toFileLine() const;

    // Cria um objeto Movie a partir de uma linha lida do arquivo.
    // Também entende o formato antigo (campos separados por "|"), para que
    // arquivos "dados_filmes.txt" salvos por versões anteriores continuem
    // funcionando normalmente.
    static Movie fromFileLine(const QString &line);

private:
    QString m_title;
    QString m_genre;
    int m_year;
    QString m_type;
    QString m_status;
    int m_rating;
    bool m_favorite;
    QString m_posterPath;
};

#endif // MOVIE_H
