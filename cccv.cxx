/*
 * cccv.cxx
 *
 * Copyright (C) 2022, Charles Chiou
 */

#include <unistd.h>
#include <stdio.h>
#include <getopt.h>
#include <opencv2/opencv.hpp>
#include "TrackDetect.hxx"

static int use_gui = 0;
static TrackDetect *td = NULL;


static void print_help(int argc, char **argv)
{
    (void)(argc);

    printf("Usage: %s [OPTIONS]\n", argv[0]);
    printf("  --help,-h        This message\n");
    printf("  --use-gui,-g     Use graphical user interface\n");
}

static const struct option long_options[] = {
    { "help", no_argument, NULL, 'h', },
    { "use-gui", no_argument, NULL, 'g', },
};

static void cleanup(void)
{
    if (td) {
        delete td;
    }
}

int main(int argc, char **argv)
{
    for (;;) {
        int option_index = 0;
        int c = getopt_long(argc, argv, "hg",
                            long_options, &option_index);
        if (c == -1) {
            break;
        }

        switch (c) {
        case 'h':
            print_help(argc, argv);
            return 0;
        case 'g':
            use_gui = 1;
            break;
        default:
            print_help(argc, argv);
            return -1;
            break;
        }
    }

    atexit(cleanup);

    td = new TrackDetect();
    td->enableGUI(use_gui);
    td->setFileSource("sample.mp4");

    for (;;) {
        if (td->loop() != 0) {
            break;
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
