#ifndef NETWORK_H
#define NETWORK_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "maths_extra.h"
#include "matrix.h"
#include "saveLoad.h"

#define __num_inputs 28 * 28
// 1000: probability 1 on every output (LR 0.01)
// 100: probability 1 on every output (LR 0.01)
// 50: probability 1 on every output (LR 0.01)
// 10: probability 1 on every output (LR 0.01)
#define __num_hidden 15
#define __num_outputs 10

#endif