#ifndef OSDLYRICSWIDGET_H
#define OSDLYRICSWIDGET_H

#include <QWidget>
#include <QtCore>
#include <QTimer>
typedef struct LRCLine{
    long milliseconds;
    QString  lineStr;
    bool operator <(const LRCLine &line) const;
}LrcLine;


class OSDLyricsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OSDLyricsWidget(QWidget *parent = 0);
    ~OSDLyricsWidget();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

signals:

public slots:
    void handleNewLyricsData(QByteArray *lrcData);
    void processLyricLine(QString line);
    void updateLyrics(long timePos);
    void handleTimeout();
    void pauseTimer(bool stop);
private:
    QPoint windowPos, dPos;
    QString currentLyrics;
    int curIndex;
    QList <LrcLine>   *lrcLineList;
    unsigned long curLyricTime,curLyricTimeLeft;
    QTimer  *timer;
};

#endif // OSDLYRICSWIDGET_H
