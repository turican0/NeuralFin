#include <iostream>
#include <vector>
#include <cstdio>
#include <fstream>
#include <ostream>
#include <streambuf>
#include <ctime> 

#include "openner/Matrix.hpp"
#include "openner/utils/Math.hpp"
#include "openner/NeuralNetwork.hpp"

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
    koef = maxx;
};

SDL_bool done = SDL_FALSE;

int inputsize = 30;
int outputsize = 5;
int addx = 0;

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
        int y1 = rendy-(datavect[i].open / koef * rendy);
        if(i>0)
            SDL_RenderDrawLine(renderer, x1, y1, oldx, oldy);
        oldx = x1;
        oldy = y1;
    }
    vector<double> input;
    {
        for (int k = 0; k < inputsize; k++)
            input.push_back(datavect[k + pos].open / koef);
        n->test(input);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

        int outputLayerIndex = n->layers.size() - 1;
        vector<Neuron*> outputNeurons = n->layers.at(outputLayerIndex)->getNeurons();

        for (int i = 0; i < outputsize; i++) {
            double y = outputNeurons.at(i)->getActivatedVal();

            int x1 = ((double)(i+pos) / datavect.size()) * rendx;
            int y1 = rendy - (y * rendy);
            if (i > 0)
                SDL_RenderDrawLine(renderer, x1, y1, oldx, oldy);
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

void mainx(SDL_Renderer* renderer/*int argc, char **argv*/) {

        parseCSV((char*)"c:\\prenos\\NeuralFin\\tsla.csv");

        findKoef();

        
       // Segundo teste:
        vector<double> input;
        for(int i=0;i< inputsize;i++)
            input.push_back(datavect[i+ addx].open/ koef);
        /*
        input.push_back(0.2);
        input.push_back(0.5);
        input.push_back(0.1);*/
       
        vector<double> target;
        for (int i = 0; i < outputsize; i++)
            target.push_back(datavect[i + inputsize + addx].open / koef);
        /*target.push_back(0.2); 
        target.push_back(0.5); 
        target.push_back(0.9);*/
    
        /*double learningRate  = 0.05;
        double momentum      = 1;
        double bias          = 1;*/
        
        double learningRate = 0.01;
        double momentum = 1;
        double bias = 1;

        vector<int> topology;
        /*topology.push_back(650);
        topology.push_back(213);
        topology.push_back(650);*/

        topology.push_back(65);
        topology.push_back(21);
        topology.push_back(65);

        NeuralNetwork *n = new NeuralNetwork(topology, 2, 3, 1, 1, 0.05, 1);

        n->loadWeights((char*)"c:\\prenos\\NeuralFin\\tslaW.csv");

        drawgraph(renderer,n,0);

        for (int i = 0; i < 1000; i++) {
            // cout << "Training at index " << i << endl;
            addx = 0;
            for (int j = 0; j < datavect.size()- inputsize- outputsize; j++)
            {
                for (int k = 0; k < inputsize; k++)
                    input[k]=datavect[k + addx].open / koef;
                for (int k = 0; k < outputsize; k++)
                    target[k]=datavect[k + inputsize + addx].open / koef;
                n->train(input, target, bias, learningRate, momentum);
                addx++;
                cout << " " << j;
            }
            drawgraph(renderer, n, 0);
            cout << endl << "Error: " << n->error*koef << endl;
            n->saveWeights((char*)"c:\\prenos\\NeuralFin\\tslaW.csv");
        }
       // Primeiro teste:
/*     for (int i = 0; i < 100; i++) {
        Matrix *a = new Matrix(100, 100, true);
        Matrix *b = new Matrix(100, 100, true);
        Matrix *c = new Matrix(a->getNumRows(), b->getNumRows(), false);
        cout << "Multiplying matrix at index " << i << endl;
        utils::Math::multiplyMatrix(a, b, c);

        delete a;
        delete b;
        delete c;
    } */

    //return 0;
}

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;

        if (SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer) == 0) {
            //SDL_bool done = SDL_FALSE;
            mainx(renderer);
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