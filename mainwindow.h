#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QList>
#include <QMetaObject>
#include <QPointer>
#include <QTimer>
#include <QWidget>
#include "colorwidget.h"
#include "palettebutton.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void toggleTimer();
    void cycleMode();
    void addColorButton();        // <-- ДОБАВЛЕНО
    void clearPaletteButtons();   // <-- ДОБАВЛЕНО

private:
    enum class Mode { Cycle, Random, Blink };
    void setupUi();
    void rebuildTimeoutConnection();
    void setStatus(const QString &text);
    void updateModeLabel();
    QString modeName() const;

    QTimer m_timer;
    ColorWidget m_canvas;
    Mode m_mode;

    QMetaObject::Connection m_timeoutConn;

    QVBoxLayout *m_rootLayout;
    QGridLayout *m_paletteGrid;    // <-- ДОБАВЛЕНО
    QLabel *m_statusLabel;
    QLabel *m_modeLabel;

    QList<QPointer<PaletteButton>> m_paletteButtons; // <-- ДОБАВЛЕНО
    QList<QMetaObject::Connection> m_dynamicConnections; // <-- ДОБАВЛЕНО
};

#endif // MAINWINDOW_H