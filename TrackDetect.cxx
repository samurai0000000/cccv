/*
 * TrackDetect.cxx
 *
 * Copyright (C) 2022, Charles Chiou
 */

#include <iostream>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include "TrackDetect.hxx"

using namespace std;
using namespace cv;

TrackDetect::TrackDetect()
{
    struct termios t_new;

    _use_gui = 0;

    tcgetattr(fileno(stdin), &_t_old);
    t_new = _t_old;
    t_new.c_lflag &= (tcflag_t) ~(ICANON | ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &t_new);
}

TrackDetect::~TrackDetect()
{
    tcsetattr(fileno(stdin), TCSANOW, &_t_old);
}

void TrackDetect::enableGUI(int enable)
{
    if (_vc) {
        delete _vc;
    }

    if (enable) {
        cv::namedWindow("cccv", cv::WINDOW_AUTOSIZE);
        _use_gui = 1;
    } else {
        cv::destroyAllWindows();
        _use_gui = 0;
    }
}

void TrackDetect::setFileSource(const char *filename)
{
    int frames;

    if (filename == NULL)
        return;

    try {
        _vc = new cv::VideoCapture(filename);
    } catch (cv::Exception &e) {
        return;
    }

    if (_vc) {
        frames = _vc->get(cv::CAP_PROP_FRAME_COUNT);

        if (frames == 0) {
            delete _vc;
            _vc = NULL;
        } else {
            cout << filename << " has " << frames << " frames" << endl;
        }
    }
}

int TrackDetect::loop(void) const
{
    int key;

    if (_use_gui) {
        key = cv::waitKey(5);
    } else {
        key = getchar();
    }

    if (key == 'q') {
        return 1;
    }

    if (_vc) {
        Mat frame;

        *_vc >> frame;
        cout << "frame " << _vc->get(cv::CAP_PROP_POS_FRAMES) << endl;

        if (_use_gui) {
            if (cv::getWindowProperty("cccv", WND_PROP_AUTOSIZE) == -1) {
                return 1;
            } else {
                cv::imshow("cccv", frame);
            }
        }
    }

    return 0;
}

/*
 * Local variables:
 * mode: C++
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
