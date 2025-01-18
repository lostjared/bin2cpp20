#include "worker.h"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QByteArray>

#include <algorithm>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

static bool is_Valid(const std::string &name) {
    QRegularExpression re(QStringLiteral("^[a-zA-Z_][a-zA-Z0-9_]*$"));
    return re.match(QString::fromStdString(name)).hasMatch();
}

static std::string convertString(const std::string &s) {
    std::string temp;
    for(std::size_t i = 0; i < s.length(); ++i) {
        if(s[i] == '\r') {
            continue; // eat token
        } else if(s[i] == '\"' || s[i] == '\\') {
            temp += '\\';
            temp += s[i];
        } else {
            temp += s[i];
        }
    }
    return temp;
}

static QString convertStreamToArray(bool c_expr,
                                    const std::string &name,
                                    const char *data,
                                    std::size_t length)
{
 
    std::string e_type{"inline"};
    if(c_expr) {
        e_type = "constexpr";
    }

 
    std::ostringstream out;
    out << e_type << " std::array<unsigned char, " << (length + 1) << "> " << name << " {";
    for(std::size_t i = 0; i < length; ++i) {
        out << "0x" << std::uppercase << std::hex 
            << static_cast<unsigned int>(static_cast<unsigned char>(data[i]))
            << ",";
    }
 
    out << "0x0};\n";

    return QString::fromStdString(out.str());
}

static QString convertStreamToString(bool unicode, 
                                     bool sorted,
                                     const std::string &name,
                                     const QStringList &lines)
{
 
    std::ostringstream out;
    // "inline const std::string name[] = {" or "inline const std::wstring name[] = {"
    if(!unicode) {
        out << "inline const std::string " << name << "[] = {";
    } else {
        out << "inline const std::wstring " << name << "[] = {";
    }

 
    std::vector<std::string> v;
    v.reserve(lines.size());
    for(const auto &l : lines) {
        std::string line = l.toStdString();
        if(!line.empty()) {
            v.push_back(line);
        }
    }

 
    if(sorted) {
        std::sort(v.begin(), v.end());
    }

    std::size_t totalSize = v.size();
    std::size_t index = 0;
    for(const auto &line : v) {
        out << (unicode ? "L\"" : "\"") 
            << convertString(line) 
            << "\"";
        if(index < totalSize - 1) {
            out << ",";
        }
        ++index;
    }
    out << "};\n";
    out << "inline unsigned long " << name << "_length {" << v.size() << "};\n";

    return QString::fromStdString(out.str());
}


Worker::Worker(const QString &inputFile,
               const QString &variableName,
               bool cExpr,
               bool asString,
               bool unicode,
               bool sorted,
               QObject *parent)
    : QObject(parent),
      m_inputFile(inputFile),
      m_variableName(variableName),
      m_cExpr(cExpr),
      m_asString(asString),
      m_unicode(unicode),
      m_sorted(sorted)
{
}

void Worker::doWork()
{
    if(!is_Valid(m_variableName.toStdString())) {
        emit finished(QString(), QString("Invalid variable name: %1").arg(m_variableName));
        return;
    }

    QFile file(m_inputFile);
    if(!file.open(QIODevice::ReadOnly)) {
        emit finished(QString(), QString("Could not open file: %1").arg(m_inputFile));
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    if(!m_asString) {
        std::string finalName = "bin_" + m_variableName.toStdString();
        QString result = convertStreamToArray(
            m_cExpr,
            finalName,
            data.constData(),
            static_cast<std::size_t>(data.size())
        );
        emit finished(result, QString());
        return;
    }
    else {
        std::string finalName = "str_" + m_variableName.toStdString();
        QString text = QString::fromUtf8(data);
        QStringList lines = text.split('\n', Qt::SkipEmptyParts);
        QString result = convertStreamToString(
            m_unicode,
            m_sorted,
            finalName,
            lines
        );
        emit finished(result, QString());
        return;
    }
}
