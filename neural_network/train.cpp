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

void NeuralNetwork::test(vector<double> input)
{
    this->setCurrentInput(input);
    this->feedForward();
}

void NeuralNetwork::saveWeights(char* file) {
    ofstream myfile(file);
    
        if (myfile.is_open())
        {
            //myfile.write((char*)this, sizeof(this));
            for (Matrix* elem : this->weightMatrices) {
                int nrows = elem->getNumRows();
                int ncols = elem->getNumCols();
                for (int nr = 0; nr < nrows; nr++)
                for (int nc = 0; nc < ncols; nc++)
                {
                    myfile << elem->getValue(nr,nc) << "\n";
                }                
            }
            myfile.close();
        }       
    
    else cout << "Unable to open file";
}

void NeuralNetwork::loadWeights(char* file) {
    ifstream myfile(file);

    if (myfile.is_open())
    {
        //myfile.write((char*)this, sizeof(this));
        for (Matrix* elem : this->weightMatrices) {
            int nrows = elem->getNumRows();
            int ncols = elem->getNumCols();
            for (int nr = 0; nr < nrows; nr++)
                for (int nc = 0; nc < ncols; nc++)
                {
                    string line;
                    getline(myfile, line);
                    elem->setValue(nr, nc, std::stod(line));
                }
        }
        myfile.close();
    }

    else cout << "Unable to open file";
}