#ifndef SAVELOAD_H
#define SAVELOAD_H

#include <stdio.h>
#include <stdlib.h>
#include "matrix.h"


void saveMatrix(Matrix* a, FILE *fp);

void save(Matrix *hw, Matrix *hb, Matrix *ow, Matrix *ob, char* filename);


#endif