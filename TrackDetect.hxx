/*
 * TrackDetect.hxx
 *
 * Copyright (C) 2022, Charles Chiou
 */

#ifndef TRACKDETECT_HXX
#define TRACKDETECT_HXX

#include <termios.h>

class TrackDetect {

public:

    TrackDetect();
    ~TrackDetect();

    void enableGUI(int enable);
    void setFileSource(const char *filename);
    int loop(void);

private:

    void processFrame(cv::Mat &frame, unsigned int index);

    int _use_gui;
    struct termios _t_old;
    cv::VideoCapture *_vc;
    unsigned int _width;
    unsigned int _height;

};

#endif

/*
 * Local variables:
 * mode: C++
 * c-file-style: "BSD"
 * c-basic-offset: 4
 * tab-width: 4
 * indent-tabs-mode: nil
 * End:
 */
