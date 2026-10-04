#include "movie.h"
#include <QStringList>
#include <algorithm>

namespace {
// Caractere "Unit Separator" (0x1F): não aparece no teclado, então é um
// separador de campo mais seguro que "|" (que pode aparecer em títulos
// como "Duas Vidas | Uma Só Filha").
const QChar SEPARADOR = QChar(0x1F);

// Remove quebras de linha do campo (o arquivo é lido/salvo uma linha por
// filme, então uma quebra de linha dentro de um campo corromperia o
// arquivo) e espaços extras nas pontas.
QString sanitizarCampo(const QString &texto)
{
    QString limpo = texto;
    limpo.replace('\n', ' ').replace('\r', ' ');
    return limpo.trimmed();
}
} // namespace

Movie::Movie()
    : m_title(""), m_genre(""), m_year(0), m_type("Filme"),
      m_status("Quero Assistir"), m_rating(0), m_favorite(false),
      m_posterPath("")
{
}

Movie::Movie(const QString &title, const QString &genre, int year,
             const QString &type, const QString &status, int rating,
             bool favorite, const QString &posterPath)
    : m_title(title), m_genre(genre), m_year(year), m_type(type),
      m_status(status), m_rating(rating), m_favorite(favorite),
      m_posterPath(posterPath)
{
}

QString Movie::getTitle() const { return m_title; }
QString Movie::getGenre() const { return m_genre; }
int Movie::getYear() const { return m_year; }
QString Movie::getType() const { return m_type; }
QString Movie::getStatus() const { return m_status; }
int Movie::getRating() const { return m_rating; }
bool Movie::isFavorite() const { return m_favorite; }
QString Movie::getPosterPath() const { return m_posterPath; }

void Movie::setTitle(const QString &value) { m_title = value; }
void Movie::setGenre(const QString &value) { m_genre = value; }
void Movie::setYear(int value) { m_year = value; }
void Movie::setType(const QString &value) { m_type = value; }
void Movie::setStatus(const QString &value) { m_status = value; }
void Movie::setRating(int value) { m_rating = std::clamp(value, 0, 5); }
void Movie::setFavorite(bool value) { m_favorite = value; }
void Movie::setPosterPath(const QString &value) { m_posterPath = value; }

QString Movie::toFileLine() const
{
    QString fav = m_favorite ? "1" : "0";
    return QString("%1%9%2%9%3%9%4%9%5%9%6%9%7%9%8")
        .arg(sanitizarCampo(m_title))
        .arg(sanitizarCampo(m_genre))
        .arg(m_year)
        .arg(sanitizarCampo(m_type))
        .arg(sanitizarCampo(m_status))
        .arg(m_rating)
        .arg(fav)
        .arg(sanitizarCampo(m_posterPath))
        .arg(SEPARADOR);
}

Movie Movie::fromFileLine(const QString &line)
{
    // Tenta primeiro o formato atual (separador invisível). Se a linha não
    // tiver esse separador, cai para o formato antigo (separado por "|"),
    // para continuar lendo arquivos salvos por versões anteriores do app.
    QStringList campos = line.split(SEPARADOR);
    if (campos.size() < 8) {
        campos = line.split('|');
    }

    // Se a linha estiver corrompida ou incompleta, devolve um filme vazio.
    if (campos.size() < 8) {
        return Movie();
    }

    QString title = campos.at(0);
    QString genre = campos.at(1);
    int year = campos.at(2).toInt();
    QString type = campos.at(3);
    QString status = campos.at(4);
    int rating = std::clamp(campos.at(5).toInt(), 0, 5);
    bool favorite = (campos.at(6) == "1");
    QString posterPath = campos.at(7);

    return Movie(title, genre, year, type, status, rating, favorite, posterPath);
}
