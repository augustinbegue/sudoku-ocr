#ifndef SAUVOLA_H
#define SAUVOLA_H

#include "image.h"

void filter_sauvola(Image *in, Image *out, int range, double k,
                    bool verbose_mode, char *verbose_path);

#endif