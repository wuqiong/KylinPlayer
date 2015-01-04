#include <QFileDialog>
#include <QInputDialog>
#include <QStringList>
#include <QTextCodec>
#include <QPixmap>
#include <QDebug>
#include <QtCore>
#include <QObject>
#include <QCloseEvent>
#include <QMessageBox>
#include "playermainwindow.h"
#include "ui_playermainwindow.h"
#include "network.h"

#define KMusicChannelsURL   "http://raw.github.com/iosnews/test/master/baidu.json"
#define kMusicPlaylistURL   "http://fm.baidu.com/dev/api/?tn=playlist&special=flash&prepend=&format=json&&id="
#define kMusicURL           "http://music.baidu.com/data/music/fmlink?type=mp3&rate=2&format=json&songIds="
#define kMusicLRCURL        "http://fm.baidu.com/"
PlayerMainWindow::PlayerMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PlayerMainWindow)
{
    ui->setupUi(this);
    this->osdLyricsWidget = new OSDLyricsWidget();
    this->osdLyricsWidget->show();
    this->localMediaPlayList = new QMediaPlaylist(this);
    this->onlineMediaPlayList = new QMediaPlaylist(this);
    this->onlineMediaList = new QList<KMediaInfo>();
    this->mediaPlayer = new QMediaPlayer(this);
    this->mediaPlayer->setPlaylist(this->localMediaPlayList);
    this->mediaPlayer->setVolume(50);
    this->progressTimer = new QTimer(this);
    this->progressTimer->setInterval(100);
    this->network = new Network(this);
    network->fetchChannels(KMusicChannelsURL);

    this->setupConnections();

    this->dbOK = db.initDataBase();


}

PlayerMainWindow::~PlayerMainWindow()
{
    delete ui;
    delete localMediaPlayList;
    delete mediaPlayer;
    delete progressTimer;
    delete network;
    delete onlineMediaList;
    delete onlineMediaPlayList;
    delete osdLyricsWidget;

}

void PlayerMainWindow::setupConnections()
{
    connect(ui->localAction, SIGNAL(triggered()), this, SLOT(openLocalMedia()));
    connect(ui->netAction,  SIGNAL(triggered()), this, SLOT(openInternetURL()));
    connect(ui->playButton, SIGNAL(clicked()), this, SLOT(playButtonClicked()));
    connect(ui->prevButton, SIGNAL(clicked()), this, SLOT(prevButtonClicked()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(nextButtonClicked()));
    connect(ui->playTimeSlider, SIGNAL(sliderMoved(int)), this, SLOT(playSliderValueChanged(int)));
    connect(ui->volumeDial, SIGNAL(valueChanged(int)), this, SLOT(volumeDialValueChanged(int)));
    connect(ui->channelComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(channelIndexChanged(int)));

    connect(this->mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playerStateChanged(QMediaPlayer::State)));
    connect(this->mediaPlayer, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)), this, SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));

    connect(this->progressTimer, SIGNAL(timeout()), this, SLOT(playProgressUpdate()));
    connect(this->mediaPlayer, SIGNAL(metaDataChanged()), this, SLOT(metaDataUpdate()));

    connect(this->ui->aboutAction, SIGNAL(triggered()), this,SLOT(showAbout()));
    connect(this,SIGNAL(destroyed()), this->osdLyricsWidget, SLOT(close()));

    //networks signals
    connect(this->network, SIGNAL(musicChannelsFetched(QList<KChannel>*)), this, SLOT(channelsDownloaded(QList<KChannel>*)));
    connect(this->network, SIGNAL(mediaInfoListFetched(QList<KMediaInfo>*)), this, SLOT(mediaInfoListDownloaded(QList<KMediaInfo>*)));
    connect(this->network, SIGNAL(mediaInfoFetched(KMediaInfo)), this, SLOT(mediaInfoDownloaded(KMediaInfo)));
    connect(this->network, SIGNAL(imageDataFetched(QByteArray*)), this,SLOT(AlbumImageDownloaded(QByteArray*)));
    connect(this->network, SIGNAL(lyricsDataFetched(QByteArray*)), this->osdLyricsWidget, SLOT(handleNewLyricsData(QByteArray*)));

    //favorites
    connect(this->ui->addFavAction, SIGNAL(triggered()), this, SLOT(addCurrentSongToFavorites()));
    connect(this->ui->playFavAction, SIGNAL(triggered()), this, SLOT(playSongInFavorites()));
}

void PlayerMainWindow::closeEvent(QCloseEvent *event)
{
    this->osdLyricsWidget->close();
    event->accept();
}

void PlayerMainWindow::openLocalMedia()
{
    QStringList fileNameList;
    fileNameList = QFileDialog::getOpenFileNames(this,
        tr("播放本地歌曲"), "", tr("MP3/MPEG4音频 (*.mp3 *.m4a);;"));

    if (!fileNameList.isEmpty())
    {
        this->localMediaPlayList->clear();
        foreach (const QString &fileName, fileNameList) {
            QMediaContent media = QMediaContent(QUrl::fromLocalFile(fileName));
            this->localMediaPlayList->addMedia(media);
        }

        this->localMediaPlayList->setCurrentIndex(0);
    }
    else
    {
            //取消
    }
    qDebug() << fileNameList;
    return ;
}

void PlayerMainWindow::openInternetURL()
{
    QString urlString;
    bool ok;
    urlString = QInputDialog::getText(this,
        tr("播放网络歌曲"),tr("请输入歌曲网址(尚未实现)"),QLineEdit::Normal,"http://", &ok);
     if (ok && !urlString.isEmpty())
     {
        //inputed
     }
     qDebug() << urlString;

     return;
}

void PlayerMainWindow::playButtonClicked()
{
    if(this->mediaPlayer->state() == QMediaPlayer::PlayingState)
    {
        this->mediaPlayer->pause();
        qDebug() << "pause";
    }else
    {
        this->mediaPlayer->setVolume(this->ui->volumeDial->value());
        this->mediaPlayer->play();
        qDebug() << "play";
    }
}

void PlayerMainWindow::prevButtonClicked()
{
    qDebug() << "prev";
    QMediaPlaylist *currentPlayList = this->onlineMode?this->onlineMediaPlayList:this->onlineMediaPlayList;
    currentPlayList->previous();
    this->osdLyricsWidget->handleNewLyricsData(NULL);
}

void PlayerMainWindow::nextButtonClicked()
{
    qDebug() << "next";
    QMediaPlaylist *currentPlayList = this->onlineMode?this->onlineMediaPlayList:this->onlineMediaPlayList;
    currentPlayList->next();
    this->osdLyricsWidget->handleNewLyricsData(NULL);
}

void PlayerMainWindow::playSliderValueChanged(int value)
{
    qDebug() << "slider changed " << value;
    float percent = (value * 1.0) / this->ui->playTimeSlider->maximum();
    int64_t pos = this->mediaPlayer->duration() * percent;
    this->mediaPlayer->setPosition(pos);
}

void PlayerMainWindow::volumeDialValueChanged(int value)
{
    qDebug() << "Dial changed " << value;
    this->mediaPlayer->setVolume(value);
}

void PlayerMainWindow::channelIndexChanged(int index)
{
    qDebug() << "combox changed " << index;
    if(index < 0)
    {
        return;
    }else if(index == 0)
    {
        //local media
        this->onlineMode = false;
        this->mediaPlayer->setPlaylist(this->localMediaPlayList);
        return;
    }else if(index == 1)
    {
        //update internet media channel
        network->fetchChannels(KMusicChannelsURL);
        this->onlineMode = true;
        this->onlineMediaPlayList->clear();
        this->mediaPlayer->setPlaylist(this->onlineMediaPlayList);
        return;
    }
    this->onlineMode = true;
    this->onlineMediaPlayList->clear();
    this->mediaPlayer->setPlaylist(this->onlineMediaPlayList);

    QComboBox *channelComboBox = (QComboBox*)sender();
    QString channelID = channelComboBox->itemData(index).toString();
    this->network->fetchMediaInfoList(kMusicPlaylistURL + channelID);
}

void PlayerMainWindow::playerStateChanged(QMediaPlayer::State state)
{
    if(QMediaPlayer::PlayingState == state)
    {
//        this->ui->playButton->setText(tr("暂停"));
        this->ui->playButton->setIcon(QIcon(":/images/pause.png"));
        this->progressTimer->start();
        this->osdLyricsWidget->pauseTimer(false);
    }else
    {
//        this->ui->playButton->setText(tr("播放"));
        this->ui->playButton->setIcon(QIcon(":/images/play.png"));
        this->progressTimer->stop();
        this->osdLyricsWidget->pauseTimer(true);
    }
    qDebug() << "state  changed " << state;
}

void PlayerMainWindow::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    switch(status)
    {
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::EndOfMedia:
        this->mediaPlayer->playlist()->next();
        break;
    default:
        ;
    }

    qDebug() << "status changed " << status;
}

void PlayerMainWindow::playProgressUpdate()
{
    int64_t pos = this->mediaPlayer->position();
    int64_t duration = this->mediaPlayer->duration();
    int value = 100 * (1.0*pos)/duration;
    this->ui->playTimeSlider->setValue(value);
    this->osdLyricsWidget->updateLyrics(pos);
}

void PlayerMainWindow::metaDataUpdate()
{
    QString title, subTitle, albumTitle, albumArtist;
    QImage coverArtImage;
    QPixmap pixmap;
    if(!this->onlineMode)
    {
        title = this->mediaPlayer->metaData("Title").toString();
        subTitle = this->mediaPlayer->metaData("SubTitle").toString();
        albumTitle = this->mediaPlayer->metaData("AlbumTitle").toString();
        albumArtist = this->mediaPlayer->metaData("AlbumArtist").toString();
        coverArtImage = this->mediaPlayer->metaData("CoverArtImage").value<QImage>();
        if(coverArtImage.isNull())
        {
            pixmap = QPixmap(":/images/Qt.png");
        }else
        {
            pixmap.convertFromImage(coverArtImage);
        }
    }else
    {
        int curIndex = this->onlineMediaPlayList->currentIndex();
        if(curIndex < 0 || curIndex > this->onlineMediaList->count() - 1)
        {
            return;
        }
        KMediaInfo curInfo = this->onlineMediaList->at(curIndex);
        title = curInfo.songName;
        albumTitle = curInfo.albumName;
        albumArtist = curInfo.artistName;
        this->network->fetchImage(curInfo.songPicUrl);
        this->network->fetchLyrics(kMusicLRCURL + curInfo.lrcUrl);

    }

    this->ui->singerLabel->setText("歌手: "+albumArtist);
    this->ui->albumLabel->setText("专辑: "+albumTitle);
    this->ui->songLabel->setText("歌名: "+title);
    if(!pixmap.isNull())
    {
        this->ui->artWorkLabel->setPixmap(pixmap.scaled(this->ui->artWorkLabel->size()));
    }
}

void PlayerMainWindow::playOnlineMedia(int index)
{
    this->mediaPlayer->playlist()->setCurrentIndex(index);
    this->mediaPlayer->play();
    qDebug()<<this->mediaPlayer->errorString();
}

void PlayerMainWindow::addCurrentSongToFavorites()
{
    if(this->onlineMode)
    {
        int index=this->mediaPlayer->playlist()->currentIndex();
        if(index<0||index>=this->onlineMediaList->count())
        {
            return;
        }
        this->db.addFavoriteSong(this->onlineMediaList->at(index));
    }
}

void PlayerMainWindow::playSongInFavorites()
{
     QList<KMediaInfo> * mediaInfoList = this->db.getFavoriteSong();
     if(mediaInfoList && mediaInfoList->count()>0)
     {
         this->onlineMediaPlayList->clear();
         this->onlineMediaList->clear();
         this->mediaPlayer->setPlaylist(this->onlineMediaPlayList);
         this->onlineMode = true;
     }
     for(int i = 0; mediaInfoList&&i<mediaInfoList->count();i++)
     {
         KMediaInfo mediaInfo = mediaInfoList->at(i);
         QUrl url = QUrl(mediaInfo.mp3Url);
         QMediaContent content(url);
         this->onlineMediaPlayList->addMedia(content);
         this->onlineMediaList->append(mediaInfo);
         if(this->onlineMediaPlayList->mediaCount() == 1)
         {
             this->playOnlineMedia(0);
         }
     }
}

void PlayerMainWindow::showAbout()
{
    QMessageBox::about(this, tr("关于 ") + qApp->applicationName(),
    tr("<h2>UbuntuKylin简易网络播放器</h2><br/>"
       "        KylinPlayer 是<a href=\"http://www.ubuntukylin.com\">优麒麟</a>为"
       "湖南科技职业学院 <br/>2014年国培班学员学习Qt5开发的简易音乐播放器.<br/>"
       "支持: Ubuntu Kylin/Mac OS X/Windows.<br/><br/>"
       "作者: \t大茶园丁(戴维营教育) <br/>"
       "邮箱: \t<a href=\"mailto:147957232@qq.com?subject=Feedback for KyinPlayer&body=Ubuntu KylinPlayer\">147957232@qq.com</a>.<br/>"
       "网站: \t<a href=\"http://www.ubuntukylin.com\">http://www.ubuntukylin.com.</a><br/>"
       "反馈: \t <a href=\"http://www.ubuntukylin.com/ukylin/\">http://www.ubuntukylin.com/ukylin/</a><br/><br/>"
       "<img src=\":/images/ubuntukylin.png\" ></img>"));
}

void PlayerMainWindow::channelsDownloaded(QList<KChannel> *channelList)
{
    if (channelList->count() <= 0)
    {
        return ;
    }
    this->ui->channelComboBox->clear();
    this->ui->channelComboBox->addItems(QStringList()<<"本地歌曲"<<"网络歌曲");
    for(int i = 0; i < channelList->count(); i++)
    {
        QString id = channelList->value(i).id;
        QString title = channelList->value(i).title;
        this->ui->channelComboBox->addItem(title, QVariant(id));
    }

}

void PlayerMainWindow::mediaInfoListDownloaded(QList<KMediaInfo> *mediaInfoList)
{
    if(mediaInfoList->count() <= 0)
    {
        return;
    }
    int count = mediaInfoList->count();
    this->onlineMediaList->clear();
    this->onlineMediaList->reserve(count);
    for(int i = 0;  i < mediaInfoList->count(); i++)
    {
        KMediaInfo mediaInfo = mediaInfoList->at(i);
        this->network->fetchSongInfo(kMusicURL+ QString("%1").arg(mediaInfo.id));
    }
    qDebug() << mediaInfoList;
}

void PlayerMainWindow::mediaInfoDownloaded(KMediaInfo mediaInfo)
{
    QUrl url = QUrl(mediaInfo.mp3Url);
    QMediaContent content(url);
    this->onlineMediaPlayList->addMedia(content);
    this->onlineMediaList->append(mediaInfo);
    if(this->onlineMediaPlayList->mediaCount() == 1)
    {
        this->playOnlineMedia(0);
    }

    return;
}

void PlayerMainWindow::AlbumImageDownloaded(QByteArray *imageData)
{
    QImage img;
    QPixmap pixmap;
    if(imageData->isEmpty())
    {
        pixmap = QPixmap(":/images/Qt.png");
    }else
    {
        img.loadFromData(*imageData);
        pixmap.convertFromImage(img);
    }
    this->ui->artWorkLabel->setPixmap(pixmap.scaled(this->ui->artWorkLabel->size()));
}
