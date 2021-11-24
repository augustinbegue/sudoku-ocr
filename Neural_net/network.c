#include "network.h"


static const int num_inputs = 2;
static const int num_hidden = 5;
static const int num_outputs = 1;

// number of "generation" of AI
static const int EPOCHS = 1000000;

// learning rate
static const double LR = 0.1;

static const int num_training = 4;
static const int NUMBER_OF_TESTS = 100000;

// cost function
double cost(double expected_output, double predicted_output)
{
    return 0.5 * pow((expected_output - predicted_output), 2);
}

/*
 return the result of the network for a XOR (have to be modified for the real network)
 hw -> hidden weights , hb -> hidden bias , ow -> output weights , ob -> ourpur bias
*/
double result_network(double i1, double i2, Matrix *hw, Matrix *hb, Matrix *ow, Matrix *ob)
{
    // matrix with the inputs 
    Matrix input;
    m_init(&input, 1, num_inputs);
    m_setIndex(&input, 0, 0, i1);
    m_setIndex(&input, 0, 1, i2);

    // result of the hidden layer
    Matrix result_hidden;
    m_mult(&input, hw, &result_hidden);
    m_add(&result_hidden, hb);
    m_map(&result_hidden, sigmoid);

    // output layer
    Matrix result_output;
    m_mult(&result_hidden, ow, &result_output);
    m_add(&result_output, ob);
    m_map(&result_output, sigmoid);

    double res = m_get(&result_output, 0, 0);

    // free all memory 
    m_free(&input);
    m_free(&result_hidden);
    m_free(&result_output);

    return res;
}

// Execute random test on a trained network and display the accuracy of the network
void random_test(Matrix *hw, Matrix *hb, Matrix *ow, Matrix *ob)
{
    int correct_predictions = 0;
    double total_error = 0.0;
    for (int trial = 0; trial < NUMBER_OF_TESTS; trial++)
    {
        double i1 = rand_input_XOR();
        double i2 = rand_input_XOR();

        double expected;

        if ((i1 < 0.5 && i2 < 0.5) || (i1 > 0.5 && i2 > 0.5))
        {
            expected = 0.0;
        }
        else
        {
            expected = 1.0;
        }

        double predicted_result = result_network(i1, i2, hw, hb, ow, ob);

        total_error += 0.5 * pow((expected - predicted_result), 2);

        double rounded_predicted = -1.0;

        if (predicted_result < 0.5)
        {
            rounded_predicted = 0.0;
        }
        else
        {
            rounded_predicted = 1.0;
        }

        if (double_equals(expected, rounded_predicted))
        {
            correct_predictions++;
        }

    }

    double accuracy = ((double)(correct_predictions)) / ((double)(NUMBER_OF_TESTS));
    printf("Passed %d out of %d tests. (Accuracy = %f)\n", correct_predictions, NUMBER_OF_TESTS, accuracy);
    printf("Total error over %d random tests: %f\n", NUMBER_OF_TESTS, total_error);
    printf("Average error per trial: %f\n", total_error / ((double)(NUMBER_OF_TESTS)));
}

int train()
{
    //Initializing training sets
    Matrix training_inputs;
    m_init(&training_inputs, num_training, 2);

    m_setIndex(&training_inputs, 0, 0, 1.0);
    m_setIndex(&training_inputs, 0, 1, 1.0);

    m_setIndex(&training_inputs, 1, 0, 0.0);
    m_setIndex(&training_inputs, 1, 1, 1.0);

    m_setIndex(&training_inputs, 2, 0, 1.0);
    m_setIndex(&training_inputs, 2, 1, 0.0);

    m_setIndex(&training_inputs, 3, 0, 0.0);
    m_setIndex(&training_inputs, 3, 1, 0.0);

    
    Matrix training_outputs;
    m_init(&training_outputs, num_training, 1);

    m_setIndex(&training_outputs, 0, 0, 0.0);

    m_setIndex(&training_outputs, 1, 0, 1.0);

    m_setIndex(&training_outputs, 2, 0, 1.0);

    m_setIndex(&training_outputs, 3, 0, 0.0);
    

    Matrix hidden_weights;
    m_init(&hidden_weights, num_inputs, num_hidden);
    for (int i = 0; i < (&hidden_weights)->rows; i++)
    {
        for (int j = 0; j < (&hidden_weights)->cols; j++)
        {
            m_setIndex(&hidden_weights, i, j, rand_weight());
        }
    }

    Matrix hidden_bias;
    m_init(&hidden_bias, 1, num_hidden);
    for (int i = 0; i < (&hidden_bias)->rows; i++)
    {
        for (int j = 0; j < (&hidden_bias)->cols; j++)
        {
            m_setIndex(&hidden_bias, i, j, rand_weight());
        }
    }

    Matrix output_weights;
    m_init(&output_weights, num_hidden, num_outputs);
    for (int i = 0; i < (&output_weights)->rows; i++)
    {
        for (int j = 0; j < (&output_weights)->cols; j++)
        {
            m_setIndex(&output_weights, i, j, rand_weight());
        }
    }

    Matrix output_bias;
    m_init(&output_bias, 1, num_outputs);
    for (int i = 0; i < (&output_bias)->rows; i++)
    {
        for (int j = 0; j < (&output_bias)->cols; j++)
        {
            m_setIndex(&output_bias, i, j, rand_weight());
        }
    }

    //Iterate through epochs
    for (int n = 0; n < EPOCHS; n++)
    {
        //Forward pass
        Matrix inputs;
        m_copy(&training_inputs, &inputs);

        Matrix in_h;
        m_mult(&inputs, &hidden_weights, &in_h);
        m_add(&in_h, &hidden_bias);

        Matrix out_h;
        m_copy(&in_h, &out_h);
        m_map(&out_h, sigmoid);

        Matrix in_o;
        m_mult(&out_h, &output_weights, &in_o);
        m_add(&in_o, &output_bias);

        Matrix out_o;
        m_copy(&in_o, &out_o);
        m_map(&out_o, sigmoid);

        //Back Propagation

        // Error of the output
        Matrix error;
        m_copy(&out_o, &error);
        m_subtract(&error, &training_outputs);

        Matrix derr_douto;
        m_copy(&error, &derr_douto);

        Matrix douto_dino;
        m_copy(&out_o, &douto_dino);
        m_map(&douto_dino, d_sigmoid);

        Matrix derr_dino;
        m_copy(&derr_douto, &derr_dino);
        m_hadamard(&derr_dino, &douto_dino);

        Matrix output_weights_trans;
        m_transpose(&output_weights, &output_weights_trans);

        Matrix error_hidden_layer;
        m_mult(&derr_dino, &output_weights_trans, &error_hidden_layer);

        Matrix douth_dinh;
        m_copy(&out_h, &douth_dinh);
        m_map(&douth_dinh, d_sigmoid);

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

        //Updating weights and biases
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
    }

    // Saving the final weigths and bias

    save(&hidden_weights, &hidden_bias, &output_weights, &output_bias,"save");

    // Printing the different result of the tests
    printf("Result of tests after training\n");
    printf("Result for (0, 0) (Expected: 0): %f\n", result_network(0.0, 0.0, &hidden_weights, &hidden_bias, &output_weights, &output_bias));
    printf("Result for (0, 1) (Expected: 1): %f\n", result_network(0.0, 1.0, &hidden_weights, &hidden_bias, &output_weights, &output_bias));
    printf("Result for (1, 0) (Expected: 1): %f\n", result_network(1.0, 0.0, &hidden_weights, &hidden_bias, &output_weights, &output_bias));
    printf("Result for (1, 1) (Expected: 0): %f\n", result_network(1.0, 1.0, &hidden_weights, &hidden_bias, &output_weights, &output_bias));
    printf("\n");
    
    printf("\nAccuracy of the network:\n");

    random_test(&hidden_weights, &hidden_bias, &output_weights, &output_bias);

    m_free(&hidden_weights);
    m_free(&hidden_bias);
    m_free(&output_weights);
    m_free(&output_bias);
    return 0;
}