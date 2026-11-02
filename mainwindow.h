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
    void addColorButton();
    void clearPaletteButtons();
    void toggleClickHandler();    // <-- ДОБАВЛЕНО

private:
    enum class Mode { Cycle, Random, Blink };
    void setupUi();
    void rebuildTimeoutConnection();
    void reconnectCanvasHandler(); // <-- ДОБАВЛЕНО
    void setStatus(const QString &text);
    void updateModeLabel();
    QString modeName() const;

    QTimer m_timer;
    ColorWidget m_canvas;
    Mode m_mode;
    bool m_clickHandlerAlt;        // <-- ДОБАВЛЕНО

    QMetaObject::Connection m_timeoutConn;
    QMetaObject::Connection m_clickConn; // <-- ДОБАВЛЕНО

    QVBoxLayout *m_rootLayout;
    QGridLayout *m_paletteGrid;
    QLabel *m_statusLabel;
    QLabel *m_modeLabel;

    QList<QPointer<PaletteButton>> m_paletteButtons;
    QList<QMetaObject::Connection> m_dynamicConnections;
};

#endif // MAINWINDOW_H