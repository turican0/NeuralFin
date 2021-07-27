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
    koef = maxx*3;
};

SDL_bool done = SDL_FALSE;

int inputsize = 30;
int outputsize = 1;
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

void mainx(SDL_Renderer* renderer/*int argc, char **argv*/) {
    double olderror3=0;
    double olderror2=0;
    double olderror=0;

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

#include <stdlib.h>
#include <stdio.h>
#include "tensorflow/c/c_api.h"

void NoOpDeallocator(void* data, size_t a, void* b) {}

int mainy(int argc, char* argv[])
{
    //********* Read model
    TF_Graph* Graph = TF_NewGraph();
    TF_Status* Status = TF_NewStatus();

    TF_SessionOptions* SessionOpts = TF_NewSessionOptions();
    TF_Buffer* RunOpts = NULL;

    const char* saved_model_dir = "lstm2/";
    const char* tags = "serve"; // default model serving tag; can change in future
    int ntags = 1;

    TF_Session* Session = TF_LoadSessionFromSavedModel(SessionOpts, RunOpts, saved_model_dir, &tags, ntags, Graph, NULL, Status);
    if (TF_GetCode(Status) == TF_OK)
    {
        printf("TF_LoadSessionFromSavedModel OK\n");
    }
    else
    {
        printf("%s", TF_Message(Status));
    }

    //****** Get input tensor
    //TODO : need to use saved_model_cli to read saved_model arch
    int NumInputs = 1;
    TF_Output* Input = (TF_Output*)malloc(sizeof(TF_Output) * NumInputs);

    TF_Output t0 = { TF_GraphOperationByName(Graph, "serving_default_input_1"), 0 };
    if (t0.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName serving_default_input_1\n");
    else
        printf("TF_GraphOperationByName serving_default_input_1 is OK\n");

    Input[0] = t0;

    //********* Get Output tensor
    int NumOutputs = 1;
    TF_Output* Output = (TF_Output*)malloc(sizeof(TF_Output) * NumOutputs);

    TF_Output t2 = { TF_GraphOperationByName(Graph, "StatefulPartitionedCall"), 0 };
    if (t2.oper == NULL)
        printf("ERROR: Failed TF_GraphOperationByName StatefulPartitionedCall\n");
    else
        printf("TF_GraphOperationByName StatefulPartitionedCall is OK\n");

    Output[0] = t2;

    //********* Allocate data for inputs & outputs
    TF_Tensor** InputValues = (TF_Tensor**)malloc(sizeof(TF_Tensor*) * NumInputs);
    TF_Tensor** OutputValues = (TF_Tensor**)malloc(sizeof(TF_Tensor*) * NumOutputs);

    int ndims = 2;
    int64_t dims[] = { 1,30 };
    float data[1 * 30];//= {1,1,1,1,1,1,1,1,1,1};
    for (int i = 0; i < (1 * 30); i++)
    {
        data[i] = 1.00;
    }
    int ndata = sizeof(float) * 1 * 30;// This is tricky, it number of bytes not number of element

    TF_Tensor* int_tensor = TF_NewTensor(TF_FLOAT, dims, ndims, data, ndata, &NoOpDeallocator, 0);
    if (int_tensor != NULL)
    {
        printf("TF_NewTensor is OK\n");
    }
    else
        printf("ERROR: Failed TF_NewTensor\n");

    InputValues[0] = int_tensor;

    // //Run the Session
    TF_SessionRun(Session, NULL, Input, InputValues, NumInputs, Output, OutputValues, NumOutputs, NULL, 0, NULL, Status);

    if (TF_GetCode(Status) == TF_OK)
    {
        printf("Session is OK\n");
    }
    else
    {
        printf("%s", TF_Message(Status));
    }

    // //Free memory
    TF_DeleteGraph(Graph);
    TF_DeleteSession(Session, Status);
    TF_DeleteSessionOptions(SessionOpts);
    TF_DeleteStatus(Status);


    void* buff = TF_TensorData(OutputValues[0]);
    float* offsets = (float*)buff;
    printf("Result Tensor :\n");
    for (int i = 0; i < 10; i++)
    {
        printf("%f\n", offsets[i]);
    }

    return 0;
}