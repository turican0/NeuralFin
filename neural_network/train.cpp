#include "openner/NeuralNetwork.hpp"

void NeuralNetwork::train(
    vector<double> input,
    vector<double> target,
    double bias,
    double learningRate,
    double momentum
) {
    this->learningRate = learningRate;
    this->momentum = momentum;
    this->bias = bias;
    
    this->setCurrentInput(input);
    this->setCurrentTarget(target);

    this->feedForward();
    this->setErrors();
    this->backPropagation();
}

void NeuralNetwork::saveWeights(
    char* file
) {
    ofstream myfile(file);
    
        if (myfile.is_open())
        {
            for (Matrix* elem : this->weightMatrices) {
                myfile << elem << "\n";
            }
            myfile.close();
        }       
    
    else cout << "Unable to open file";
}