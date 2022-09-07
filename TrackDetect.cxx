/*
 * TrackDetect.cxx
 *
 * Copyright (C) 2022, Charles Chiou
 */

#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
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

int TrackDetect::loop(void)
{
    int key = 0;
    unsigned int timeout_ms = _vc ? 0 : 50;

    if (_use_gui) {
        if (timeout_ms == 0) {
#if 0
            key = cv::pollKey();
#else
            key = cv::waitKey(1);
#endif
        } else {
            key = cv::waitKey(timeout_ms);
        }
    } else {
        int nfds;
        fd_set readfds;
        struct timeval timeout = {
            .tv_sec = 0,
            .tv_usec = timeout_ms * 1000,
        };

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);
        nfds = STDIN_FILENO + 1;

        select(nfds, &readfds, NULL, NULL, &timeout);

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            key = getchar();
        }
    }

    if (key == 'q') {
        return 1;
    }

    if (_vc) {
        Mat frame;
        unsigned index;

        *_vc >> frame;
        index = _vc->get(cv::CAP_PROP_POS_FRAMES);

        if (_use_gui) {
            if (cv::getWindowProperty("cccv", WND_PROP_AUTOSIZE) == -1) {
                return 1;
            } else {
                cv::imshow("cccv", frame);
            }
        }

        processFrame(frame, index);

        if (index == _vc->get(cv::CAP_PROP_FRAME_COUNT)) {
            delete _vc;
            _vc = NULL;
        }
    }

    return 0;
}

void TrackDetect::processFrame(cv::Mat &frame, unsigned index)
{
    (void)(frame);
    cout << "frame " << index << endl;
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
