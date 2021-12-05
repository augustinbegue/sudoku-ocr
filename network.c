#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include "network.h"
#include "image.h"
#include "result_network.h"
// 176 Epochs 40 hidden = 0.803976
// 236 Epochs 100 hidden acc = 0.896942
// Accuracy: 0.901835 Epochs: 242acc = 0.901835 120 hidden

// Epochs: 234 Accuracy: 0.880734 100

// Epochs: 246 Accuracy: 0.896636 80

// 900 -> 84

// 800 -> 83

// 1000 -> 80

// 850 -> 85
char *concat(const char *s1, const char *s2)
{
    char *result
        = malloc(strlen(s1) + strlen(s2) + 1); //+1 for the null-terminator
    // in real code you would check for errors in malloc here
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

// number of "generation" of AI
static const int EPOCHS = 1000;

// learning rate
static double LR = 0.01;

static const int num_training = 3377;

void trainingInput(Matrix *training_inputs, size_t numTest, char *path)
{

    Image image = SDL_Surface_to_Image(load_image(path));

    int count = 0;
    int width = image.width;
    int height = image.height;

    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            Pixel pix = image.pixels[x][y];

            // color 0 or 255
            int color = pix.r;
            if (color == 0)
            {
                m_setIndex(training_inputs, numTest, count, 0.0);
            }
            else
            {
                m_setIndex(training_inputs, numTest, count, 1.0);
            }
            count++;
        }
    }

    free_Image(&image);
}

void trainingOutput(Matrix *training_outputs, size_t testNumber, int expected)
{
    for (size_t i = 0; i < 10; i++)
    {
        m_setIndex(training_outputs, testNumber, i, 0.0);
    }
    m_setIndex(training_outputs, testNumber, expected, 1.0);
}

void shuffle(int *array)
{
    size_t n = (size_t)(num_training);
    size_t i;
    for (i = 0; i < n - 1; i++)
    {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void train()
{
    // Initializing training sets
    //Matrix training_inputs;
    //m_init(&training_inputs, num_training, __num_inputs);

    //Matrix training_outputs;
    //m_init(&training_outputs, num_training, __num_outputs);

    int acc[num_training];

    for (int i = 0; i < num_training; i++)
    {
        acc[i] = i;
    }

    struct dirent *de; // Pointer for directory entry

    char *dir = "./assets/training_set/";

    DIR *dr = opendir(dir);

    if (dr == NULL) // opendir returns NULL if couldn't open directory
    {
        printf("Could not open directory: %s", dir);
    }

    int count = 0;
    char *training[num_training];
    while ((de = readdir(dr)) != NULL && count < num_training)
    {

        char *s = de->d_name;

        if (s[0] != '.')
        {
            training[count] = malloc(100);

            snprintf(training[count], 100, "%s", s);

            count += 1;
        }
    }
    closedir(dr);

    // printf("\nTraining set initialized\n");
    // printf("Opened and loaded %d files.\n\n", count);

    Matrix hidden_weights;
    m_init(&hidden_weights, __num_inputs, __num_hidden);
    for (int i = 0; i < (&hidden_weights)->rows; i++)
    {
        for (int j = 0; j < (&hidden_weights)->cols; j++)
        {
            m_setIndex(&hidden_weights, i, j, rand_weight());
        }
    }

    Matrix hidden_bias;
    m_init(&hidden_bias, 1, __num_hidden);
    for (int i = 0; i < (&hidden_bias)->rows; i++)
    {
        for (int j = 0; j < (&hidden_bias)->cols; j++)
        {
            m_setIndex(&hidden_bias, i, j, rand_weight());
        }
    }

    Matrix output_weights;
    m_init(&output_weights, __num_hidden, __num_outputs);
    for (int i = 0; i < (&output_weights)->rows; i++)
    {
        for (int j = 0; j < (&output_weights)->cols; j++)
        {
            m_setIndex(&output_weights, i, j, rand_weight());
        }
    }

    Matrix output_bias;
    m_init(&output_bias, 1, __num_outputs);
    for (int i = 0; i < (&output_bias)->rows; i++)
    {
        for (int j = 0; j < (&output_bias)->cols; j++)
        {
            m_setIndex(&output_bias, i, j, rand_weight());
        }
    }

    // Iterate through epochs
    for (int n = 0; n < EPOCHS; n++)
    {
        printf("Epochs: %d\n", n);
        shuffle(acc);
        double accuracy = 0;
        /*
                struct dirent *de; // Pointer for directory entry

                char *dir = "./assets/training_set/";

                DIR *dr = opendir(dir);

                if (dr == NULL) // opendir returns NULL if couldn't open
           directory
                {
                    printf("Could not open directory: %s", dir);
                }*/

        int count__ = 0;

        while (count__ < num_training)
        {
            int count_ = acc[count__];

            char src[40];
            strcpy(src, training[count_]);
            count__++;

            if (src[0] != '.')
            {
                char *s1 = concat(dir, src);

                Matrix _inputs;
                m_init(&_inputs, 1, __num_inputs);

                trainingInput(&_inputs, 0, s1);

                Matrix _outputs;
                m_init(&_outputs, 1, __num_outputs);
                trainingOutput(&_outputs, 0, (int)(src[0] - '0'));

                free(s1);

                // Forward pass
                Matrix inputs;
                m_copy(&_inputs, &inputs);

                Matrix in_h;
                m_mult(&inputs, &hidden_weights, &in_h);
                m_add(&in_h, &hidden_bias);

                Matrix out_h;
                m_copy(&in_h, &out_h);
                m_map(&out_h, relu);

                Matrix in_o;
                m_mult(&out_h, &output_weights, &in_o);
                m_add(&in_o, &output_bias);

                Matrix out_o;
                m_copy(&in_o, &out_o);
                softmax(&out_o);

                if (max_mat(&out_o) == max_mat(&_outputs))
                {
                    accuracy += 1;
                }

                // Back Propagation

                // Error of the output
                Matrix error;
                m_copy(&out_o, &error);
                m_subtract(&error, &_outputs);

                Matrix derr_douto;
                m_copy(&error, &derr_douto);

                Matrix douto_dino;
                m_copy(&out_o, &douto_dino);

                Matrix derr_dino;
                m_copy(&derr_douto, &derr_dino);
                m_hadamard(&derr_dino, &douto_dino);

                Matrix output_weights_trans;
                m_transpose(&output_weights, &output_weights_trans);

                Matrix error_hidden_layer;
                m_mult(&derr_dino, &output_weights_trans, &error_hidden_layer);

                Matrix douth_dinh;
                m_copy(&out_h, &douth_dinh);
                m_map(&douth_dinh, d_relu);

                Matrix derr_dinh;
                m_copy(&error_hidden_layer, &derr_dinh);
                m_hadamard(&derr_dinh, &douth_dinh);

                Matrix input_trans;
                m_transpose(&inputs, &input_trans);

                Matrix d_hidden_layer;
                m_mult(&input_trans, &derr_dinh, &d_hidden_layer);

                Matrix out_h_trans;
                m_transpose(&out_h, &out_h_trans);

                Matrix d_output_layer;
                m_mult(&out_h_trans, &derr_dino, &d_output_layer);

                // Updating weights and biases
                m_scalar_mult(&d_output_layer, LR);
                m_subtract(&output_weights, &d_output_layer);

                Matrix d_output_bias;
                m_colsum(&derr_dino, &d_output_bias);
                m_scalar_mult(&d_output_bias, LR);
                m_subtract(&output_bias, &d_output_bias);

                m_scalar_mult(&d_hidden_layer, LR);
                m_subtract(&hidden_weights, &d_hidden_layer);

                Matrix d_hidden_bias;
                m_colsum(&derr_dinh, &d_hidden_bias);
                m_scalar_mult(&d_hidden_bias, LR);
                m_subtract(&hidden_bias, &d_hidden_bias);

                // free all matrix
                m_free(&inputs);
                m_free(&in_h);
                m_free(&out_h);
                m_free(&in_o);
                m_free(&out_o);
                m_free(&error);
                m_free(&derr_douto);
                m_free(&douto_dino);
                m_free(&derr_dino);
                m_free(&output_weights_trans);
                m_free(&error_hidden_layer);
                m_free(&douth_dinh);
                m_free(&derr_dinh);
                m_free(&input_trans);
                m_free(&d_hidden_layer);
                m_free(&out_h_trans);
                m_free(&d_output_layer);
                m_free(&d_output_bias);
                m_free(&d_hidden_bias);
                m_free(&_outputs);
                m_free(&_inputs);
            }
        }
        // closedir(dr);
        accuracy /= num_training;

        printf("Accuracy: %f\n", accuracy);
    }

    // Saving the final weigths and bias

    save(&hidden_weights, &hidden_bias, &output_weights, &output_bias, "save");

    printf("\nFinish training.\n");

    // m_free(&training_outputs);
    // m_free(&training_inputs);
    m_free(&hidden_weights);
    m_free(&hidden_bias);
    m_free(&output_weights);
    m_free(&output_bias);
    for (int i = 0; i < num_training; i++)
    {
        free(training[i]);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <train/test> <filename>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "train") == 0)
    {
        if (argc != 2)
        {
            printf("Usage: %s <train/test> <filename>\n", argv[0]);
            return 1;
        }

        train();
        return 0;
    }
    else if (strcmp(argv[1], "test") == 0)
    {
        if (argc != 3)
        {
            printf("Usage: %s <train/test> <filename>\n", argv[0]);
            return 1;
        }

        Image image = SDL_Surface_to_Image(load_image(argv[2]));

        int res = neural_network_execute(&image);

        free_Image(&image);
        return res;
    }
    else
    {
        printf("Usage: %s <save/train> <filename>\n", argv[0]);
        return 1;
    }
}