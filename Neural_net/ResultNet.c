#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "matrix.h"
#include "maths_extra.h"
#include "network.h"



int main()
{
int retur = 0;
char* filename = "save";
FILE *fp;
fp = fopen(filename, "r");

if(fp==NULL)
        return 1;
 
int num_inputs;
int num_hidden;
int num_outputs;

 
retur = fscanf(fp, "%d", &num_inputs);
retur = fscanf(fp, "%d", &num_hidden);

Matrix hidden_weights;
m_init(&hidden_weights, num_inputs, num_hidden);

Matrix *hw =  &hidden_weights;
double tmp = 0.00;

for (int i = 0; i <  hw->rows; i++)
    {
        for (int j = 0; j < hw->cols; j++)
        {
            retur =fscanf(fp,"%lf", &tmp);
            m_setIndex(hw,i,j,tmp);
        }
    }

retur =fscanf(fp, "%d", &num_inputs);
retur =fscanf(fp, "%d", &num_hidden);

Matrix hidden_bias;
m_init(&hidden_bias, 1, num_hidden);

Matrix *hb =  &hidden_bias;


for (int i = 0; i <  hb->rows; i++)
    {
        for (int j = 0; j < hb->cols; j++)
        {
            retur =fscanf(fp,"%lf", &tmp);
            m_setIndex(hb,i,j,tmp);
        }
    }

retur =fscanf(fp, "%d", &num_hidden);
retur =fscanf(fp, "%d", &num_outputs);

Matrix output_weights;
m_init(&output_weights, num_hidden, num_outputs);

Matrix *ow =  &output_weights;


for (int i = 0; i <  ow->rows; i++)
    {
        for (int j = 0; j < ow->cols; j++)
        {
            retur =fscanf(fp,"%lf", &tmp);
            m_setIndex(ow,i,j,tmp);
        }
    }

retur =fscanf(fp, "%d", &num_hidden);
retur =fscanf(fp, "%d", &num_outputs);

Matrix output_bias;
m_init(&output_bias, num_hidden, num_outputs);

Matrix *ob =  &output_bias;


for (int i = 0; i <  ob->rows; i++)
    {
        for (int j = 0; j < ob->cols; j++)
        {
            retur =fscanf(fp,"%lf", &tmp);
            m_setIndex(ob,i,j,tmp);
        }
    }


fclose(fp);

printf("Result of tests after training\n");
    printf("Result for (0, 0) (Expected: 0): %f\n", result_network(0.0, 0.0, hw, hb, ow, ob));
    printf("Result for (0, 1) (Expected: 1): %f\n", result_network(0.0, 1.0, hw, hb, ow, ob));
    printf("Result for (1, 0) (Expected: 1): %f\n", result_network(1.0, 0.0, hw, hb, ow, ob));
    printf("Result for (1, 1) (Expected: 0): %f\n", result_network(1.0, 1.0, hw, hb, ow, ob));
    printf("\n");

return retur;
}