#include "mainwindow.h"
#include "worker.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QCheckBox>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QThread>
#include <QClipboard>
#include <QGuiApplication>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_inputFileLine(nullptr),
      m_variableNameLine(nullptr),
      m_cExprCheck(nullptr),
      m_asStringCheck(nullptr),
      m_unicodeCheck(nullptr),
      m_sortCheck(nullptr),
      m_generateButton(nullptr),
      m_copyButton(nullptr),
      m_saveButton(nullptr),
      m_outputText(nullptr)
{
    setupUI();
    setupConnections();
    setWindowTitle("bin2cpp20");
    resize(800, 600);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    {
        QHBoxLayout *layout = new QHBoxLayout;
        QLabel *label = new QLabel("Input File:", central);
        m_inputFileLine = new QLineEdit(central);
        QPushButton *browseBtn = new QPushButton("Browse...", central);

        layout->addWidget(label);
        layout->addWidget(m_inputFileLine);
        layout->addWidget(browseBtn);
        mainLayout->addLayout(layout);

        connect(browseBtn, &QPushButton::clicked, this, &MainWindow::onBrowse);
    }

    {
        QHBoxLayout *layout = new QHBoxLayout;
        QLabel *label = new QLabel("Variable Name:", central);
        m_variableNameLine = new QLineEdit(central);

        layout->addWidget(label);
        layout->addWidget(m_variableNameLine);
        mainLayout->addLayout(layout);
    }

    {
        QHBoxLayout *layout = new QHBoxLayout;

        m_cExprCheck    = new QCheckBox("Use constexpr", central);
        m_asStringCheck = new QCheckBox("String Output", central);
        m_unicodeCheck  = new QCheckBox("Unicode", central);
        m_sortCheck     = new QCheckBox("Sort Strings", central);

        layout->addWidget(m_cExprCheck);
        layout->addWidget(m_asStringCheck);
        layout->addWidget(m_unicodeCheck);
        layout->addWidget(m_sortCheck);
        mainLayout->addLayout(layout);
    }

    {
        QHBoxLayout *layout = new QHBoxLayout;
        m_generateButton = new QPushButton("Generate", central);
        m_copyButton = new QPushButton("Copy Output", central);
        m_saveButton = new QPushButton("Save Output", central);

        layout->addWidget(m_generateButton);
        layout->addWidget(m_copyButton);
        layout->addWidget(m_saveButton);
        mainLayout->addLayout(layout);
    }

    m_outputText = new QTextEdit(central);
    m_outputText->setReadOnly(true);
    mainLayout->addWidget(m_outputText);

    setCentralWidget(central);
    setGeometry(150, 150, 800, 600);
}

void MainWindow::setupConnections()
{
    connect(m_generateButton, &QPushButton::clicked, this, &MainWindow::onGenerate);
    connect(m_copyButton, &QPushButton::clicked, this, &MainWindow::onCopy);
    connect(m_saveButton, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(m_cExprCheck, &QCheckBox::toggled, this, &MainWindow::onCheckCExprChanged);
}

void MainWindow::onBrowse()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Select Input File");
    if (!filePath.isEmpty()) {
        m_inputFileLine->setText(filePath);
    }
}

void MainWindow::onGenerate()
{
    m_outputText->clear();

    QString inputFile     = m_inputFileLine->text().trimmed();
    QString variableName  = m_variableNameLine->text().trimmed();
    bool cExpr            = m_cExprCheck->isChecked();
    bool asString         = m_asStringCheck->isChecked();
    bool unicode          = m_unicodeCheck->isChecked();
    bool sorted           = m_sortCheck->isChecked();

    if (inputFile.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please select an input file.");
        return;
    }
    if (variableName.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please provide a variable name.");
        return;
    }
    QThread *thread = new QThread(this);
    Worker *worker = new Worker(inputFile, variableName, cExpr, asString, unicode, sorted);
    worker->moveToThread(thread);
    connect(thread, &QThread::started, worker, &Worker::doWork);
    connect(worker, &Worker::finished, this, &MainWindow::handleWorkerFinished);
    connect(worker, &Worker::finished, thread, &QThread::quit);
    connect(thread, &QThread::finished, worker, &Worker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    // Start
    thread->start();
}

void MainWindow::onCopy()
{
    QString text = m_outputText->toPlainText();
    if (!text.isEmpty()) {
        QGuiApplication::clipboard()->setText(text);
    }
}

void MainWindow::onSave()
{
    QString text = m_outputText->toPlainText();
    if (text.isEmpty()) {
        QMessageBox::information(this, "Info", "No output to save.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Save Output", QString(), "*.hpp");
    if (!fileName.isEmpty()) {
        if (!fileName.endsWith(".hpp", Qt::CaseInsensitive)) {
            fileName += ".hpp";
        }
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            file.write(text.toUtf8());
            file.close();
        } else {
            QMessageBox::warning(this, "Error", "Could not open file for writing.");
        }
    }
}

void MainWindow::onCheckCExprChanged()
{
    if (m_cExprCheck->isChecked()) {
        m_asStringCheck->setChecked(false);
        m_asStringCheck->setEnabled(false);
        m_unicodeCheck->setChecked(false);
        m_unicodeCheck->setEnabled(false);
        m_sortCheck->setChecked(false);
        m_sortCheck->setEnabled(false);
    } else {
        m_asStringCheck->setEnabled(true);
        m_unicodeCheck->setEnabled(true);
        m_sortCheck->setEnabled(true);
    }
}

void MainWindow::handleWorkerFinished(const QString &result, const QString &error)
{
    if (!error.isEmpty()) {
        QMessageBox::critical(this, "Error", error);
    } else {
        m_outputText->setPlainText(result);
    }
}
