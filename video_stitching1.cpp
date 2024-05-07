
// Imported necessary libraries
#include <iostream>
#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

// For GUI
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Text_Display.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_File_Chooser.H>

// For video processing
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

// Class videoGUI for playing video
class videoGUI {
private:
    Fl_Window* window;
    Fl_Input* entry1;
    Fl_Text_Display* path_text;
    std::vector<std::string> path_list;
    std::vector<cv::VideoCapture> clip_list;

    void open_file() {
        Fl_File_Chooser chooser(".", "*.*", Fl_File_Chooser::MULTI, "Select files");
        chooser.show();

        while (chooser.visible()) {
            Fl::wait();
        }

        if (chooser.count() > 0) {
            path_list.clear();
            path_text->buffer()->remove_all();

            for (int i = 1; i <= chooser.count(); i++) {
                path_list.push_back(chooser.value(i));
                path_text->buffer()->insert(path_text->buffer()->length(), chooser.value(i));
                path_text->buffer()->insert(path_text->buffer()->length(), " ");
            }
        }
    }

    void row_fun() {
        clip_list.clear();

        for (const auto& path : path_list) {
            clip_list.emplace_back(path);
        }

        std::vector<cv::VideoWriter> writers;
        for (const auto& clip : clip_list) {
            cv::VideoWriter writer("row_stitch.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), clip.get(cv::CAP_PROP_FPS), cv::Size(clip.get(cv::CAP_PROP_FRAME_WIDTH), clip.get(cv::CAP_PROP_FRAME_HEIGHT)));
            writers.push_back(writer);
        }

        cv::Mat frame;
        while (true) {
            bool allEmpty = true;
            for (size_t i = 0; i < clip_list.size(); ++i) {
                if (clip_list[i].read(frame)) {
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

    void col_fun() {
        clip_list.clear();

        for (const auto& path : path_list) {
            clip_list.emplace_back(path);
        }

        cv::VideoWriter writer;
        int totalHeight = 0;
        int maxWidth = 0;
        for (const auto& clip : clip_list) {
            totalHeight += clip.get(cv::CAP_PROP_FRAME_HEIGHT);
            maxWidth = std::max(maxWidth, static_cast<int>(clip.get(cv::CAP_PROP_FRAME_WIDTH)));
        }

        writer.open("col_stitch.mp4", cv::VideoWriter::fourcc('m', 'p', '4', 'v'), clip_list[0].get(cv::CAP_PROP_FPS), cv::Size(maxWidth, totalHeight));

        std::vector<cv::Mat> frames(clip_list.size());
        cv::Mat output(totalHeight, maxWidth, CV_8UC3);

        while (true) {
            bool allEmpty = true;
            int y = 0;
            for (size_t i = 0; i < clip_list.size(); ++i) {
                if (clip_list[i].read(frames[i])) {
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

public:
    videoGUI() {
        window = new Fl_Window(1000, 700, "Video Stitching");

        Fl_Button* start_btn = new Fl_Button(180, 600, 100, 50, "START");
        start_btn->callback([](Fl_Widget* widget, void* data) {
            static_cast<Fl_Window*>(data)->hide();
        }, window);

        Fl_Button* exit_btn = new Fl_Button(700, 600, 100, 50, "EXIT");
        exit_btn->callback([](Fl_Widget* widget, void* data) {
            static_cast<Fl_Window*>(data)->hide();
        }, window);

        window->end();
        window->show();

        Fl_Window* window1 = new Fl_Window(1000, 700, "Video Stitching");

        Fl_Button* select_btn = new Fl_Button(80, 600, 100, 50, "SELECT");
        select_btn->callback([this](Fl_Widget* widget, void* data) {
            this->open_file();
        }, nullptr);

        Fl_Button* stitch_row_btn = new Fl_Button(265, 600, 200, 50, "STITCH IN ROWS");
        stitch_row_btn->callback([this](Fl_Widget* widget, void* data) {
            this->row_fun();
        }, nullptr);

        Fl_Button* stitch_col_btn = new Fl_Button(585, 600, 200, 50, "STITCH IN COLUMNS");
        stitch_col_btn->callback([this](Fl_Widget* widget, void* data) {
            this->col_fun();
        }, nullptr);

        path_text = new Fl_Text_Display(80, 310, 840, 250);
        path_text->textfont(FL_COURIER);

        window1->end();
        window1->show();
    }

    ~videoGUI() {
        delete window;
    }
};

int main() {
    videoGUI app;
    return Fl::run();
}


