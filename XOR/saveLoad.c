#include "saveLoad.h"

void save(Matrix *hw, Matrix *hb, Matrix *ow, Matrix *ob, char* filename)
{
FILE *fp;
fp = fopen(filename, "w");



int row = hw -> rows;
int col = hw -> cols;

fprintf(fp, "%d\n", row);
fprintf(fp, "%d\n", col);

    
    for (int i = 0; i < hw->rows; i++)
    {
        for (int j = 0; j < hw->cols; j++)
        {
            fprintf(fp,"%f\n", m_get(hw, i, j));
        }
    }
    
row = hb -> rows;
col = hb -> cols;

fprintf(fp, "%d\n", row);
fprintf(fp, "%d\n", col);

    
    for (int i = 0; i < hb->rows; i++)
    {
        for (int j = 0; j < hb->cols; j++)
        {
            fprintf(fp,"%f\n", m_get(hb, i, j));
        }
    }

row = ow -> rows;
col = ow -> cols;

fprintf(fp, "%d\n", row);
fprintf(fp, "%d\n", col);

    
    for (int i = 0; i < ow->rows; i++)
    {
        for (int j = 0; j < ow->cols; j++)
        {
            fprintf(fp,"%f\n", m_get(ow, i, j));
        }
    }

row = ob -> rows;
col = ob -> cols;

fprintf(fp, "%d\n", row);
fprintf(fp, "%d\n", col);

    
    for (int i = 0; i < ob->rows; i++)
    {
        for (int j = 0; j < ob->cols; j++)
        {
            fprintf(fp,"%f\n", m_get(ob, i, j));
        }
    }
    


fclose(fp);
}


