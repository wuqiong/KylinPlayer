#include "osdlyricswidget.h"
#include <QMouseEvent>
#include <QPainter>
#include <QApplication>
#include <QScreen>
#include <QDebug>
#ifdef Q_OS_LINUX
#include <QX11Info>
#include <X11/extensions/shape.h>
#endif
#ifdef Q_OS_WIN32
#include <windows.h>
#endif
OSDLyricsWidget::OSDLyricsWidget(QWidget *parent) :
    QWidget(parent)
{
    this->lrcLineList = new QList<LrcLine>();
    this->timer = new QTimer(this);
    this->timer->setInterval(100);
    this->timer->start();
    connect(this->timer, SIGNAL(timeout()), this, SLOT(handleTimeout()));

//    this->currentLyrics = QString("戴维营教育嵌入式Linux Qt5开发培训学习作品");
    this->setFixedSize(1000,100);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground,true);
    this->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    this->setAttribute(Qt::WA_MouseTracking,false);
    this->setWindowOpacity(1);
    QSize size = QApplication::screens().first()->geometry().size();
    this->move( (size.width() - this->width())/2, (size.height() - this->height()) - 100 );
#ifdef Q_OS_LINUX
        XShapeCombineRectangles(QX11Info::display(), winId(), ShapeInput, 0,
                                0, NULL, 0, ShapeSet, YXBanded);
#endif
#ifdef Q_OS_WIN
        SetWindowLong((HWND)winId(), GWL_EXSTYLE, GetWindowLong((HWND)winId(), GWL_EXSTYLE) |
                      WS_EX_TRANSPARENT | WS_EX_LAYERED);
        SetWindowPos((HWND)winId(),HWND_TOP,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
#endif
}

OSDLyricsWidget::~OSDLyricsWidget()
{

    delete this->lrcLineList;
    delete this->timer;

}

void OSDLyricsWidget::mousePressEvent(QMouseEvent *event)
{
    this->windowPos = this->pos();
    QPoint mousePos = event->globalPos();
    this->dPos = mousePos - windowPos;
}

void OSDLyricsWidget::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - this->dPos);
}

void OSDLyricsWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    if(this->currentLyrics.isEmpty())
    {
        return;
    }
    QPainter painter(this);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.setRenderHints(QPainter::TextAntialiasing|QPainter::Antialiasing);
    QFont font = QFont("宋体", 36);
    painter.setFont(font);
    int textPixalLength = 0;
#if 1
    QFontMetrics fontMetrics = QFontMetrics(font);
    textPixalLength = fontMetrics.boundingRect(this->currentLyrics).width();
#else
    textPixalLength = this->rect().width();
#endif
    QRect rect_l = this->rect();
    painter.fillRect(rect_l, Qt::transparent);


    painter.setPen(QColor("lightgreen"));
    painter.drawText(rect_l,Qt::AlignLeft|Qt::AlignVCenter,this->currentLyrics);

    float percent = 1 - (1.0*this->curLyricTimeLeft)/this->curLyricTime;
    QRect rect_h(rect_l.x(), rect_l.y(),textPixalLength*percent,rect_l.height());
    painter.setPen(QColor("transparent"));
    painter.drawRect(rect_h);
    painter.setPen(QColor("red"));
    painter.drawText(rect_h,Qt::AlignLeft|Qt::AlignVCenter,this->currentLyrics);
//    qDebug()<<"lrc:"<<this->currentLyrics << "percent:"<<percent;

}

void OSDLyricsWidget::handleNewLyricsData(QByteArray *lrcData)
{
    this->lrcLineList->clear();
    this->currentLyrics = "";
    if(NULL==lrcData)
    {
        return;
    }
    QStringList lrcList =  QString(*lrcData).split("\n");
    foreach (QString lrc, lrcList) {
       if(lrc.size()<=5) continue;
       lrc = lrc.toLower().simplified();
       if(lrc.startsWith("[ti:") || lrc.startsWith("[ar:") || lrc.startsWith("[al:"))
       {
           continue;
       }
       //process lyric line
       this->processLyricLine(lrc);
    }
    qSort(*this->lrcLineList);
}

 void OSDLyricsWidget::processLyricLine(QString line)
{
    if(!line.startsWith("[")) return;
    int timeStampEndPos = line.lastIndexOf("]");
    if(timeStampEndPos < 9 ) return;
    QString lrcLineStr = line.mid(timeStampEndPos+1);
    QString timeStampStr = line.left(timeStampEndPos+1);

    QStringList timeStrList = timeStampStr.split("]");
    foreach (QString timeStr, timeStrList) {
        if(!timeStr.startsWith("[")) continue;
        timeStr.remove(0,1);
        int min = 0, sec = 0, msec = 0;
        ::sscanf(timeStr.toStdString().data(),"%d:%d.%d", &min, &sec, &msec);
        long milliseconds = min*60*1000 + sec*1000 + msec*10;
        LrcLine lrc_line;
        lrc_line.milliseconds = milliseconds;
        lrc_line.lineStr = lrcLineStr;
        this->lrcLineList->append(lrc_line);

    }


}

void OSDLyricsWidget::updateLyrics(long timePos)
{

    for(int i = 1; i < this->lrcLineList->count(); i++)
    {
        LrcLine line = this->lrcLineList->at(i);
        if(line.milliseconds > timePos)
        {
            if(this->curIndex == i)
            {
                break;
            }
            this->curIndex = i;
            LrcLine prevLine = this->lrcLineList->at(i-1);
            this->currentLyrics = prevLine.lineStr;
            this->curLyricTimeLeft = this->curLyricTime = line.milliseconds - prevLine.milliseconds;
            break;
        }
    }
    this->update();
    this->updateGeometry();
}

void OSDLyricsWidget::handleTimeout()
{

    if(this->curLyricTimeLeft > (unsigned long)this->timer->interval())
    {
        this->curLyricTimeLeft -= this->timer->interval();
    }else
    {
        this->curLyricTimeLeft = 0;
    }
//    qDebug()<<"left:"<<this->curLyricTimeLeft;

}

void OSDLyricsWidget::pauseTimer(bool stop)
{
    if(stop){
        this->timer->stop();
    }else
    {
        this->timer->start();
    }
}


bool LRCLine::operator <(const LRCLine &line) const
{
    return this->milliseconds < line.milliseconds;
}
