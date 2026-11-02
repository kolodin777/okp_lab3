#include "mainwindow.h"
#include <QDebug>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRandomGenerator>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent), m_mode(Mode::Cycle), m_clickHandlerAlt(false)
{
    setupUi();
    m_timer.setInterval(600);
    rebuildTimeoutConnection();
    reconnectCanvasHandler(); // <-- ДОБАВЛЕНО
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

    auto *addBtn = new QPushButton(QStringLiteral("Добавить цвет"), this);
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addColorButton);

    auto *clearBtn = new QPushButton(QStringLiteral("Очистить"), this);
    connect(clearBtn, &QPushButton::clicked, this, &MainWindow::clearPaletteButtons);

    auto *toggleHandlerBtn = new QPushButton(QStringLiteral("Переподключить обработчик"), this);
    connect(toggleHandlerBtn, &QPushButton::clicked, this, &MainWindow::toggleClickHandler);

    m_modeLabel = new QLabel(this);

    top->addWidget(startStop);
    top->addWidget(modeBtn);
    top->addWidget(addBtn);
    top->addWidget(clearBtn);
    top->addWidget(toggleHandlerBtn);
    top->addWidget(m_modeLabel);
    top->addStretch();

    m_rootLayout->addLayout(top);
    m_rootLayout->addWidget(&m_canvas, 1);

    m_paletteGrid = new QGridLayout;
    m_rootLayout->addLayout(m_paletteGrid);

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

void MainWindow::addColorButton()
{
    const QColor c = QColor::fromHsv(QRandomGenerator::global()->bounded(360), 200, 230);
    auto *btn = new PaletteButton(c, this);
    const int idx = m_paletteButtons.size();
    m_paletteGrid->addWidget(btn, idx / 6, idx % 6);
    m_paletteButtons.append(btn);

    QMetaObject::Connection conn = connect(btn, &QPushButton::clicked, this, [this, c]() {
        m_canvas.addColor(c);
        setStatus(QStringLiteral("Добавлен цвет %1").arg(c.name()));
    });
    m_dynamicConnections.append(conn);
    setStatus(QStringLiteral("Создана кнопка-цвет %1").arg(c.name()));
}

void MainWindow::clearPaletteButtons()
{
    for (const auto &conn : m_dynamicConnections) disconnect(conn);
    m_dynamicConnections.clear();

    for (const auto &p : m_paletteButtons) {
        if (p) p->deleteLater();
    }
    m_paletteButtons.clear();
    setStatus(QStringLiteral("Все кнопки-цвета удалены, соединения разорваны."));
}

void MainWindow::toggleClickHandler()
{
    m_clickHandlerAlt = !m_clickHandlerAlt;
    reconnectCanvasHandler();
    setStatus(QStringLiteral("Обработчик colorChanged переключен."));
}

void MainWindow::reconnectCanvasHandler()
{
    if (m_clickConn) disconnect(m_clickConn);

    if (!m_clickHandlerAlt) {
        m_clickConn = connect(&m_canvas, &ColorWidget::colorChanged, this, [this](const QColor &c) {
            setStatus(QStringLiteral("Цвет холста: %1").arg(c.name()));
        });
    } else {
        m_clickConn = connect(&m_canvas, &ColorWidget::colorChanged, this, [this](const QColor &c) {
            setStatus(QStringLiteral("[ALT] Цвет холста: %1 (яркость %2)").arg(c.name()).arg(c.lightness()));
        });
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