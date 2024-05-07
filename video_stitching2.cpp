
// Importing necessary libraries
#include <QApplication>
#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>
#include <QVideoWidget>
#include <QMediaPlayer>

// Class for video GUI
class VideoGUI : public QMainWindow {
    Q_OBJECT

public:
    VideoGUI(QWidget* parent = nullptr) : QMainWindow(parent) {
        // Set window title
        setWindowTitle("Video Stitching");

        // Create top frame
        QWidget* topFrame = new QWidget(this);
        QVBoxLayout* topLayout = new QVBoxLayout(topFrame);

        // Create video widget
        videoWidget = new QVideoWidget(topFrame);
        topLayout->addWidget(videoWidget);

        // Create bottom frame
        QWidget* bottomFrame = new QWidget(this);
        QHBoxLayout* bottomLayout = new QHBoxLayout(bottomFrame);

        // Create file path label and line edit
        QLabel* filePathLabel = new QLabel("File Path: ", this);
        filePathLineEdit = new QLineEdit(this);
        bottomLayout->addWidget(filePathLabel);
        bottomLayout->addWidget(filePathLineEdit);

        // Create select button
        QPushButton* selectButton = new QPushButton("SELECT", this);
        connect(selectButton, &QPushButton::clicked, this, &VideoGUI::openFile);
        bottomLayout->addWidget(selectButton);

        // Create play button
        QPushButton* playButton = new QPushButton("PLAY", this);
        connect(playButton, &QPushButton::clicked, this, &VideoGUI::playVideo);
        bottomLayout->addWidget(playButton);

        // Create pause button
        QPushButton* pauseButton = new QPushButton("PAUSE", this);
        connect(pauseButton, &QPushButton::clicked, this, &VideoGUI::pauseVideo);
        bottomLayout->addWidget(pauseButton);

        // Create resume button
        QPushButton* resumeButton = new QPushButton("RESUME", this);
        connect(resumeButton, &QPushButton::clicked, this, &VideoGUI::resumeVideo);
        bottomLayout->addWidget(resumeButton);

        // Set central widget
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
        mainLayout->addWidget(topFrame);
        mainLayout->addWidget(bottomFrame);
        setCentralWidget(centralWidget);

        // Create media player
        mediaPlayer = new QMediaPlayer(this);
        mediaPlayer->setVideoOutput(videoWidget);

        // Connect media player signals
        connect(mediaPlayer, &QMediaPlayer::stateChanged, this, &VideoGUI::handleStateChanged);
    }

private slots:
    void openFile() {
        QString fileName = QFileDialog::getOpenFileName(this, "Select Video File");
        if (!fileName.isEmpty()) {
            filePathLineEdit->setText(fileName);
            mediaPlayer->setMedia(QUrl::fromLocalFile(fileName));
        }
    }

    void playVideo() {
        mediaPlayer->play();
    }

    void pauseVideo() {
        mediaPlayer->pause();
    }

    void resumeVideo() {
        mediaPlayer->play();
    }

    void handleStateChanged(QMediaPlayer::State state) {
        if (state == QMediaPlayer::StoppedState) {
            QMessageBox::information(this, "Video Finished", "Video playback has finished.");
        }
    }

private:
    QVideoWidget* videoWidget;
    QLineEdit* filePathLineEdit;
    QMediaPlayer* mediaPlayer;
};

// Main function
int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    VideoGUI videoGUI;
    videoGUI.show();

    return app.exec();
}
