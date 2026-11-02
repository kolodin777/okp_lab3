#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMetaObject>
#include <QTimer>
#include <QWidget>
#include "colorwidget.h"

class QLabel;
class QVBoxLayout;
class QHBoxLayout;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void toggleTimer();
    void cycleMode();

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
    QLabel *m_statusLabel;
    QLabel *m_modeLabel;
};

#endif // MAINWINDOW_H