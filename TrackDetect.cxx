/*
 * TrackDetect.cxx
 *
 * Copyright (C) 2022, Charles Chiou
 */

#include <unistd.h>
#include <stdio.h>
#include <stdarg.h>
#include <sys/select.h>
#include <iostream>
#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video/background_segm.hpp>
#include "TrackDetect.hxx"

TrackDetect::TrackDetect()
{
    struct termios t_new;

    _use_gui = 0;

    tcgetattr(fileno(stdin), &_t_old);
    t_new = _t_old;
    t_new.c_lflag &= (tcflag_t) ~(ICANON | ECHO);
    tcsetattr(fileno(stdin), TCSANOW, &t_new);

    _backsub = cv::createBackgroundSubtractorMOG2();
    _backsub->setVarThreshold(30.0);
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
    int width;
    int height;

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

    // Set rescaled width/height
    width = _vc->get(cv::CAP_PROP_FRAME_WIDTH);
    height = _vc->get(cv::CAP_PROP_FRAME_HEIGHT);
    if (width > 250) {
        double scale;
        _width = 250;
        scale = ((double) _width) / ((double) width);
        _height = (int) ((double) height * scale);
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

        if (cv::getWindowProperty("cccv", WND_PROP_AUTOSIZE) == -1) {
            return 1;
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
    } else if (key == 'r' && _vc) {
        _vc->set(cv::CAP_PROP_POS_FRAMES, 0);
    }

    if (_vc) {
        Mat frame;
        unsigned index;

        *_vc >> frame;
        index = _vc->get(cv::CAP_PROP_POS_FRAMES);

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
    Mat procframe;

    cout << "frame " << index << endl;
    // Down-sample
    cv::resize(frame, frame, cv::Size(_width, _height));
    // Convert to gray-scale
    cv::cvtColor(frame, procframe, cv::COLOR_RGB2GRAY);
    // Filter background using MOG2 subtractor
    _backsub->apply(procframe, procframe);

    if (_use_gui) {
        Mat outframe;

        cv::cvtColor(procframe, procframe, cv::COLOR_GRAY2RGB);
        cv::hconcat(frame, procframe, outframe);
        cv::imshow("cccv", outframe);
    }
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
