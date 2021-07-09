#include "openner/NeuralNetwork.hpp"
#include "openner/utils/Math.hpp"

void NeuralNetwork::feedForward() {
    Matrix *a; // Matrix of neurons to the left
    Matrix *b; // Matrix of weights to the right of the layer 
    Matrix *c; // Matrix of neurons to the right

    for (int i = 0; i < (this->topologySize - 1); i++) {
        a = this->getNeuronMatrix(i);
        b = this->getWeightMatrix(i);
        c = new Matrix(
            a->getNumRows(),
            b->getNumCols(),
            false
        );

        if (i != 0) {
            a = this->getActivatedNeuronMatrix(i);
        }

        utils::Math::multiplyMatrix(a, b, c);

        for (int c_index = 0; c_index < c->getNumCols(); c_index++) {
            this->setNeuronValue(i + 1, c_index, c->getValue(0, c_index) + this->bias);
        }
    
    delete a;
    delete b;
    delete c;
    }
}
 
