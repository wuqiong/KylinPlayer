#ifndef PLAYERMAINWINDOW_H
#define PLAYERMAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QTimer>
#include <QMediaMetaData>
#include <QAudioRecorder>
#include <QCamera>
#include <QCameraViewfinder>
#include <QCameraImageCapture>
#include "common.h"
#include "database.h"
#include "osdlyricswidget.h"

namespace Ui {
class PlayerMainWindow;
}
class Network;
class PlayerMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit PlayerMainWindow(QWidget *parent = 0);
    ~PlayerMainWindow();
    void setupConnections();
    void closeEvent(QCloseEvent *event);
public slots:
    void openLocalMedia();
    void openInternetURL();
    void playButtonClicked();
    void prevButtonClicked();
    void nextButtonClicked();
    void playSliderValueChanged(int value);
    void volumeDialValueChanged(int value);
    void channelIndexChanged(int index);

    void playerStateChanged(QMediaPlayer::State state);
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);

    void playProgressUpdate();
    void metaDataUpdate();


    void playOnlineMedia(int index);
    void addCurrentSongToFavorites();
    void playSongInFavorites();
    void showAbout();

    //networks
    void channelsDownloaded(QList<KChannel> *channelList);
    void mediaInfoListDownloaded(QList<KMediaInfo> *mediaInfoList);
    void mediaInfoDownloaded(KMediaInfo mediaInfo);
    void AlbumImageDownloaded(QByteArray *imageData);
private:
    Ui::PlayerMainWindow *ui;
    QMediaPlayer    *mediaPlayer;
    QMediaPlaylist  *localMediaPlayList, *onlineMediaPlayList;
    QList<KMediaInfo> *onlineMediaList;
    QTimer  *progressTimer;
    Network *network;
    bool    onlineMode;
    Database db;
    bool    dbOK;

    OSDLyricsWidget *osdLyricsWidget;
};

#endif // PLAYERMAINWINDOW_H
