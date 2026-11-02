#include "mainwindow.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), m_mode(Mode::Cycle)
{
    setupUi();
    m_timer.setInterval(600);
    rebuildTimeoutConnection();
    setStatus(QStringLiteral("Готово. Таймер настроен."));
}

MainWindow::~MainWindow() { m_timer.stop(); }

void MainWindow::setupUi()
{
    setWindowTitle(QStringLiteral("ColorSwitcher — ЛР3"));
    m_rootLayout = new QVBoxLayout(this);
    auto *top = new QHBoxLayout;

    auto *startStop = new QPushButton(QStringLiteral("Старт/Стоп"), this);
    connect(startStop, &QPushButton::clicked, this, &MainWindow::toggleTimer);

    auto *modeBtn = new QPushButton(QStringLiteral("Режим"), this);
    connect(modeBtn, &QPushButton::clicked, this, &MainWindow::cycleMode);

    m_modeLabel = new QLabel(this);

    top->addWidget(startStop);
    top->addWidget(modeBtn);
    top->addWidget(m_modeLabel);
    top->addStretch();

    m_rootLayout->addLayout(top);
    m_rootLayout->addWidget(&m_canvas, 1);

    m_statusLabel = new QLabel(this);
    m_rootLayout->addWidget(m_statusLabel);
    updateModeLabel();
}

void MainWindow::toggleTimer()
{
    if (m_timer.isActive()) { m_timer.stop(); setStatus(QStringLiteral("Таймер остановлен.")); }
    else { m_timer.start(); setStatus(QStringLiteral("Таймер запущен.")); }
}

void MainWindow::cycleMode()
{
    switch (m_mode) {
    case Mode::Cycle:  m_mode = Mode::Random; break;
    case Mode::Random: m_mode = Mode::Blink;  break;
    case Mode::Blink:  m_mode = Mode::Cycle;  break;
    }
    updateModeLabel();
    rebuildTimeoutConnection();
    setStatus(QStringLiteral("Режим: %1").arg(modeName()));
}

void MainWindow::rebuildTimeoutConnection()
{
    if (m_timeoutConn) disconnect(m_timeoutConn);
    switch (m_mode) {
    case Mode::Cycle:  m_timeoutConn = connect(&m_timer, &QTimer::timeout, this, [this]() { m_canvas.stepForward(); }); break;
    case Mode::Random: m_timeoutConn = connect(&m_timer, &QTimer::timeout, this, [this]() { m_canvas.setRandomColor(); }); break;
    case Mode::Blink:  m_timeoutConn = connect(&m_timer, &QTimer::timeout, this, [this]() { m_canvas.toggleBlink(); }); break;
    }
}

void MainWindow::setStatus(const QString &text) { if (m_statusLabel) m_statusLabel->setText(text); }
void MainWindow::updateModeLabel() { if (m_modeLabel) m_modeLabel->setText(QStringLiteral("Режим: %1").arg(modeName())); }
QString MainWindow::modeName() const {
    switch (m_mode) {
    case Mode::Cycle:  return QStringLiteral("Цикл");
    case Mode::Random: return QStringLiteral("Случайный");
    case Mode::Blink:  return QStringLiteral("Мигание");
    }
    return QString();
}