
// Imported necessary libraries
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

class VideoGUI : public QMainWindow {
    Q_OBJECT

public:
    VideoGUI(QWidget* parent = nullptr) : QMainWindow(parent) {
        // Create main layout
        QWidget* centralWidget = new QWidget(this);
        QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

        // Create path text area
        pathTextArea = new QTextEdit(this);
        pathTextArea->setReadOnly(true);
        mainLayout->addWidget(pathTextArea);

        // Create button layout
        QHBoxLayout* buttonLayout = new QHBoxLayout();

        // Create select button
        QPushButton* selectButton = new QPushButton("Select", this);
        connect(selectButton, &QPushButton::clicked, this, &VideoGUI::openFileDialog);
        buttonLayout->addWidget(selectButton);

        // Create stitch row button
        QPushButton* stitchRowButton = new QPushButton("Stitch in Rows", this);
        connect(stitchRowButton, &QPushButton::clicked, this, &VideoGUI::stitchInRows);
        buttonLayout->addWidget(stitchRowButton);

        // Create stitch column button
        QPushButton* stitchColButton = new QPushButton("Stitch in Columns", this);
        connect(stitchColButton, &QPushButton::clicked, this, &VideoGUI::stitchInColumns);
        buttonLayout->addWidget(stitchColButton);

        mainLayout->addLayout(buttonLayout);

        setCentralWidget(centralWidget);
    }

private slots:
    void openFileDialog() {
        pathList = QFileDialog::getOpenFileNames(this, "Select Video Files", "", "Video Files (*.mp4 *.avi *.mov)");
        pathTextArea->clear();
        pathTextArea->insertPlainText(pathList.join("\n"));
    }

    void stitchInRows() {
        std::vector<cv::VideoCapture> clipList;
        for (const QString& path : pathList) {
            clipList.emplace_back(path.toStdString());
        }

        std::vector<cv::VideoWriter> writers;
        for (const auto& clip : clipList) {
            cv::VideoWriter writer("row_stitch.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), clip.get(cv::CAP_PROP_FPS), cv::Size(clip.get(cv::CAP_PROP_FRAME_WIDTH), clip.get(cv::CAP_PROP_FRAME_HEIGHT)));
            writers.push_back(writer);
        }

        cv::Mat frame;
        while (true) {
            bool allEmpty = true;
            for (size_t i = 0; i < clipList.size(); ++i) {
                if (clipList[i].read(frame)) {
                    allEmpty = false;
                    writers[i].write(frame);
                }
            }
            if (allEmpty) {
                break;
            }
        }

        for (auto& writer : writers) {
            writer.release();
        }
    }

    void stitchInColumns() {
        std::vector<cv::VideoCapture> clipList;
        for (const QString& path : pathList) {
            clipList.emplace_back(path.toStdString());
        }

        cv::VideoWriter writer;
        int totalHeight = 0;
        int maxWidth = 0;
        for (const auto& clip : clipList) {
            totalHeight += clip.get(cv::CAP_PROP_FRAME_HEIGHT);
            maxWidth = std::max(maxWidth, static_cast<int>(clip.get(cv::CAP_PROP_FRAME_WIDTH)));
        }

        writer.open("col_stitch.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), clipList[0].get(cv::CAP_PROP_FPS), cv::Size(maxWidth, totalHeight));

        std::vector<cv::Mat> frames(clipList.size());
        cv::Mat output(totalHeight, maxWidth, CV_8UC3);

        while (true) {
            bool allEmpty = true;
            int y = 0;
            for (size_t i = 0; i < clipList.size(); ++i) {
                if (clipList[i].read(frames[i])) {
                    allEmpty = false;
                    cv::Rect roi(0, y, frames[i].cols, frames[i].rows);
                    frames[i].copyTo(output(roi));
                    y += frames[i].rows;
                }
            }
            if (allEmpty) {
                break;
            }
            writer.write(output);
        }

        writer.release();
    }

private:
    QStringList pathList;
    QTextEdit* pathTextArea;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    VideoGUI videoGUI;
    videoGUI.show();
    return app.exec();
}



