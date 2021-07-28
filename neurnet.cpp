#include <iostream>
#include <vector>
#include <cstdio>
#include <fstream>
#include <ostream>
#include <streambuf>
#include <ctime> 
#include <string>

#include <iostream>     // std::cout
#include <sstream>      // std::stringstream, std::stringbuf

//#include "openner/Matrix.hpp"
//#include "openner/utils/Math.hpp"
//#include "openner/NeuralNetwork.hpp"

#include "SDL.h"

using namespace std;

struct data_t
{
    double ear;
    double moon;
    double day;
    double open;
    double high;
    double low;
    double close;
    double aclose;
    double volume;
};

std::istream& operator>>(std::istream& ist, data_t& data)
{
    char comma;
    ist >> data.ear >> comma
        >> data.moon >> comma
        >> data.day >> comma
        >> data.open >> comma
        >> data.high >> comma
        >> data.low >> comma
        >> data.close >> comma
        >> data.aclose >> comma
        >> data.volume
        ;
    return ist;
}

std::vector<data_t> datavect;

void parseCSV(char* file) {
    std::ifstream  data(file);
    std::string line;
    bool first = true;
    while (std::getline(data, line))
    {
        data_t data;
        std::stringstream lineStream(line);
        if (first)first = false;
        else
        {
            lineStream >> data;
            datavect.push_back(data);
        }
    }
};

double koef=1;

void findKoef() {
    double maxx=0;
    for (data_t actdata : datavect)
        /*(x in datavect)*/ {
        if (actdata.open > maxx)maxx = actdata.open;
    }
    koef = maxx*3;
};

SDL_bool done = SDL_FALSE;

int inputsize = 30;
int outputsize = 1;
int addx = 0;
/*
void drawgraph(SDL_Renderer* renderer,NeuralNetwork* n, int pos) {
    SDL_Event event;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    int rendx = 640;
    int rendy = 480;
    int oldx, oldy;

    for (int i=0;i<datavect.size();i++)
    {
        int x1 = ((double)i / datavect.size()) * rendx;
        int y1 = rendy-(datavect[i].open / (koef/3) * rendy);
        if(i>0)
            SDL_RenderDrawLine(renderer, x1, y1, oldx, oldy);
        oldx = x1;
        oldy = y1;
    }
    vector<double> input;
    for (int k = 0; k < inputsize; k++)
        input.push_back(0);
    for (int posi = 0; posi <datavect.size()- outputsize- inputsize; posi++)
    {
        for (int k = 0; k < inputsize; k++)
            input[k]=datavect[k + posi].open / koef;
        n->test(input);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

        int outputLayerIndex = n->layers.size() - 1;
        vector<Neuron*> outputNeurons = n->layers.at(outputLayerIndex)->getNeurons();

        for (int i = 0; i < outputsize; i++) {
            double y = outputNeurons.at(i)->getActivatedVal();
            int x1 = ((double)(i+posi+ inputsize) / datavect.size()) * rendx;
            int y1 = rendy - (y * rendy)*3.0;
            if (i > 0)
                SDL_RenderDrawLine(renderer, x1, y1, oldx, oldy);
            else
            {
                int prex1 = ((double)(posi + inputsize-1) / datavect.size()) * rendx;
                int prey1 = rendy - (datavect[posi + inputsize - 1].open / (koef / 3) * rendy);
                //int prex1 = ((double)posi / datavect.size()) * rendx;
                //int prey1 = rendy - (datavect[posi].open / (koef / 3) * rendy);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawLine(renderer, prex1, prey1, x1, y1);
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            }
            oldx = x1;
            oldy = y1;
        }
    }
    
    
    //SDL_RenderDrawLine(renderer, 320, 200, 300, 240);
    //SDL_RenderDrawLine(renderer, 300, 240, 340, 240);
    //SDL_RenderDrawLine(renderer, 340, 240, 320, 200);
    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            done = SDL_TRUE;
        }
    }
};
*/

/*
void mainx(SDL_Renderer* renderer) {
    double olderror3=0;
    double olderror2=0;
    double olderror=0;

        parseCSV((char*)"c:\\prenos\\NeuralFin\\tsla.csv");

        findKoef();

        
       // Segundo teste:
        vector<double> input;
        for(int i=0;i< inputsize;i++)
            input.push_back(datavect[i+ addx].open/ koef);
       
        vector<double> target;
        for (int i = 0; i < outputsize; i++)
            target.push_back(datavect[i + inputsize + addx].open / koef);
    
        
        double learningRate = 0.01;
        double momentum = 1;
        double bias = 1;

        vector<int> topology;

        topology.push_back(65);
        topology.push_back(21);
        topology.push_back(65);

        NeuralNetwork *n = new NeuralNetwork(topology, 2, 3, 1, bias, learningRate, momentum);

        n->loadWeights((char*)"c:\\prenos\\NeuralFin\\tslaW.csv");

        drawgraph(renderer,n,0);

        for (int i = 0; i < 1000; i++) {
            // cout << "Training at index " << i << endl;
            addx = 0;
            double allerror = 0;
            for (int j = 0; j < datavect.size()- inputsize- outputsize; j++)
            {
                for (int k = 0; k < inputsize; k++)
                    input[k]=datavect[k + addx].open / koef;
                for (int k = 0; k < outputsize; k++)
                    target[k]=datavect[k + inputsize + addx].open / koef;
                n->train(input, target, bias, learningRate, momentum);
                addx++;
                cout << " " << j;
                allerror += n->error * koef;
            }
            drawgraph(renderer, n, 0);
            cout << endl << "Error: " << allerror << " | "<< learningRate <<endl;
            olderror3 = olderror2;
            olderror2 = olderror;
            olderror = allerror;
            //if(abs(olderror3-olderror)< learningRate)learningRate = learningRate * 0.99;
            n->saveWeights((char*)"c:\\prenos\\NeuralFin\\tslaW.csv");
        }
       // Primeiro teste:

    //return 0;
}
*/
int mainz(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;

        if (SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer) == 0) {
            //SDL_bool done = SDL_FALSE;
            //mainx(renderer);
            while (!done) {
                SDL_Event event;

                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderClear(renderer);

                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawLine(renderer, 320, 200, 300, 240);
                SDL_RenderDrawLine(renderer, 300, 240, 340, 240);
                SDL_RenderDrawLine(renderer, 340, 240, 320, 200);
                SDL_RenderPresent(renderer);

                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_QUIT) {
                        done = SDL_TRUE;
                    }
                }
            }
        }

        if (renderer) {
            SDL_DestroyRenderer(renderer);
        }
        if (window) {
            SDL_DestroyWindow(window);
        }
    }
    SDL_Quit();
    return 0;
}

#include <stdlib.h>
#include <stdio.h>

void NoOpDeallocator(void* data, size_t a, void* b) {}

#include <MiniDNN.h>
using namespace MiniDNN;

typedef Eigen::MatrixXd Matrix;
typedef Eigen::VectorXd Vector;


int main(int argc, char* argv[])
{
    // Create two dimensional input data
    Vector x1 = Vector::LinSpaced(1000, 0.0, 3.15);//count,begin, end
    //std::cout << x1;
    Vector x2 = Vector::LinSpaced(1000, 0.0, 3.15);
    // Predictors -- each column is an observation
    Matrix x = Matrix::Random(2, 1000);//two rows,1000 cols
    std::cout << x;
    x.row(0) = x1;
    x.row(1) = x2;
    std::cout << x;
    // Response variables -- each column is an observation
    Matrix y = Matrix::Random(1, 1000);

    // Fill the output for the training
    for (int i = 0; i < y.cols(); i++)
    {
        y(0, i) = std::pow(x(0, i), 2) + std::pow(x(1, i), 2);
    }

    // Fill the output for the test
    Matrix xt = (Matrix::Random(2, 1000).array() + 1.0) / 2 * 3.15;
    Matrix yt = Matrix::Random(1, 1000);

    for (int i = 0; i < yt.cols(); i++)
    {
        yt(0, i) = std::pow(xt(0, i), 2) + std::pow(xt(1, i), 2);
    }

    // Construct a network object
    Network net;
    // Create three layers
    // Layer 1 -- FullyConnected, input size 2x200
    Layer* layer1 = new FullyConnected<Identity>(2, 200);
    // Layer 2 -- max FullyConnected, input size 200x200
    Layer* layer2 = new FullyConnected<ReLU>(200, 200);
    // Layer 4 -- fully connected, input size 200x1
    Layer* layer3 = new FullyConnected<Identity>(200, 1);
    // Add layers to the network object
    net.add_layer(layer1);
    net.add_layer(layer2);
    net.add_layer(layer3);
    // Set output layer
    net.set_output(new RegressionMSE());
    // Create optimizer object
    Adam opt;
    opt.m_lrate = 0.01;
    // (Optional) set callback function object
    VerboseCallback callback;
    net.set_callback(callback);
    // Initialize parameters with N(0, 0.01^2) using random seed 123
    net.init(0, 0.01, 000);
    // Fit the model with a batch size of 100, running 10 epochs with random seed 123
    net.fit(opt, x, y, 1000, 1000, 000);
    // Obtain prediction -- each column is an observation
    Eigen::MatrixXd pred = net.predict(xt);
    // Export the network to the NetFolder folder with prefix NetFile
    net.export_net("./NetFolder/", "NetFile");
    // Create a new network
    Network netFromFile;
    // Read structure and paramaters from file
    netFromFile.read_net("./NetFolder/", "NetFile");
    // Test that they give the same prediction
    std::cout << (pred - netFromFile.predict(xt)).norm() << std::endl;
    // Layer objects will be freed by the network object,
    // so do not manually delete them
    return 0;
}