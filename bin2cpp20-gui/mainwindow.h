#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLineEdit;
class QPushButton;
class QCheckBox;
class QTextEdit;
class QLabel;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onBrowse();           // Browse for input file
    void onGenerate();         // Start background generation
    void onCopy();             // Copy output text to clipboard
    void onSave();             // Save output text to a file
    void onCheckCExprChanged(); // If "constexpr" is checked/unchecked

    void handleWorkerFinished(const QString &result, const QString &error);

private:
    // UI elements
    QLineEdit *m_inputFileLine;
    QLineEdit *m_variableNameLine;
    QCheckBox *m_cExprCheck;
    QCheckBox *m_asStringCheck;
    QCheckBox *m_unicodeCheck;
    QCheckBox *m_sortCheck;
    QPushButton *m_generateButton;
    QPushButton *m_copyButton;
    QPushButton *m_saveButton;
    QTextEdit *m_outputText;
    void setupUI();
    void setupConnections();
};

#endif 
