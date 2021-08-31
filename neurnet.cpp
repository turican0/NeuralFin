#include <iostream>
#include <vector>
#include <cstdio>
#include <fstream>
#include <ostream>
#include <streambuf>
#include <ctime> 
#include <string>

#include <iostream>
#include <sstream>

#include <chrono>
#include <thread>
//#include <map>

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
std::vector<data_t> dataother[100];

int countother=0;
void parseCSV(char* file) {
    std::ifstream  data(file);
    std::string line;
    bool first = true;
    int x = 0;
    while ((std::getline(data, line))/*&&(x<15)*/)
    {
        data_t data;
        std::stringstream lineStream(line);
        if (first)first = false;
        else
        {
            lineStream >> data;
            datavect.push_back(data);
        }
        x++;
        //if (x > 6)break;
    }
};

void parseCSVother(char* file) {
    std::ifstream  data(file);
    std::string line;
    bool first = true;
    int x = 0;
    while ((std::getline(data, line))/*&&(x<15)*/)
    {
        data_t data;
        std::stringstream lineStream(line);
        if (first)first = false;
        else
        {
            lineStream >> data;            
            dataother[countother].push_back(data);
        }
        x++;
        //if (x > 6)break;
    }
    countother++;
};

double koef = 1;

void findKoef() {
    double maxx = 0;
    for (data_t actdata : datavect)
        /*(x in datavect)*/ {
        if (actdata.open > maxx)maxx = actdata.open;
    }
    koef = maxx * 3;
};
/*
int mainz(int argc, char **argv) {

        parseCSV((char*)"c:\\prenos\\NeuralFin\\tsla.csv");

        findKoef();

        int inputsize = 30;
        int outputsize = 1;
        int addx = 0;
       // Segundo teste:
        vector<double> input;
        for(int i=0;i< inputsize;i++)
            input.push_back(datavect[i+ addx].open/ koef);

        vector<double> target;
        for (int i = 0; i < outputsize; i++)
            target.push_back(datavect[i + inputsize + addx].open / koef);

        double learningRate  = 0.05;
        double momentum      = 1;
        double bias          = 1;

        vector<int> topology;
        topology.push_back(650);
        topology.push_back(213);
        topology.push_back(650);

        NeuralNetwork *n = new NeuralNetwork(topology, 2, 3, 1, 1, 0.05, 1);

        for (int i = 0; i < 1000; i++) {
            // cout << "Training at index " << i << endl;
            addx = 0;
            for (int j = 0; j < 5; j++)
            {
                for (int k = 0; k < inputsize; k++)
                    input[k]=datavect[k + addx].open / koef;
                for (int k = 0; k < outputsize; k++)
                    target[k]=datavect[k + inputsize + addx].open / koef;
                n->train(input, target, bias, learningRate, momentum);
                addx++;
                cout << "Index: " << j << endl;
            }
            cout << "Error: " << n->error << endl;
            n->saveWeights((char*)"c:\\prenos\\NeuralFin\\tslaW.csv");
        }
       // Primeiro teste:
    return 0;
}*/

SDL_bool done = SDL_FALSE;

int inputsize = 15;
int outputsize = 1;
int countoff = 4;// 4;
int cols;
int rows;

void ipusch(vector<double>* input, int x, int y, int index, double value) {
    (*input)[countoff * (cols * x + y) + index] = value;
};

double iget(vector<double>* input, int x, int y, int index) {
    return (*input)[countoff * (cols * x + y) + index];
};

void ipuschw(vector<double>* weight, /*int x,*/ int y, int index, double value) {
    (*weight)[countoff * (/*cols * x +*/ y)+index] = value;
};

double igetw(vector<double>* weight, /*int x,*/ int y, int index) {
    return (*weight)[countoff * (/*cols * x +*/ y)+index];
};

void ipuscha(vector<double>* addkoef, /*int x,*/ int y, int index, double value) {
    (*addkoef)[countoff * (/*cols * x +*/ y)+index] = value;
};

double igeta(vector<double>* addkoef, /*int x,*/ int y, int index) {
    return (*addkoef)[countoff * (/*cols * x +*/ y)+index];
};


int rendx = 640 * 2;
int rendy = 480 * 2;

int countok;
int countno;

void drawgraph(SDL_Renderer* renderer, vector<double>* output, int pos) {
    SDL_Event event;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    
    int oldx, oldy;

    countok=0;
    countno=0;

    for (int i = 0; i < datavect.size(); i++)
    {
        int x1 = ((double)i / datavect.size()) * rendx;
        int y1 = rendy - (datavect[i].open / (koef / 3) * rendy);
        if (i > 0)
            SDL_RenderDrawLine(renderer, x1, y1, oldx, oldy);
        oldx = x1;
        oldy = y1;
    }
    vector<double> input;
    for (int k = 0; k < inputsize; k++)
        input.push_back(0);
    //for (int posi = 0; posi < datavect.size() - outputsize - inputsize; posi++)
    for (int posi = 0; posi < datavect.size() - inputsize; posi++)
    {
        for (int k = 0; k < inputsize; k++)
            input[k] = datavect[k + posi].open / koef;

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

        for (int i = 0; i < outputsize; i++) {
            double y = (*output)[i + posi];
            int x1 = ((double)(i + posi + inputsize) / datavect.size()) * rendx;
            int y1 = rendy - (y / (koef / 3) * rendy);
            if (i > 0)
                SDL_RenderDrawLine(renderer, x1, y1, oldx, oldy);
            else
            {
                int prex1 = ((double)(posi + inputsize - 1) / datavect.size()) * rendx;
                int prey1 = rendy - (datavect[posi + inputsize - 1].open / (koef / 3) * rendy);
                //int prex1 = ((double)posi / datavect.size()) * rendx;
                //int prey1 = rendy - (datavect[posi].open / (koef / 3) * rendy);                
                int truey1 = datavect[posi + inputsize - 1].open;
                int truey2 = datavect[posi + inputsize].open;
                int predy1 = datavect[posi + inputsize - 1].open;
                int predy2 = y;
                int truepos = 0;
                if (truey1 < truey2)truepos = 1;
                int predpos = 0;
                if (predy1 < predy2)predpos = 1;

                if (truepos == predpos)
                {
                    countok++;
                    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
                }
                else
                {   
                    countno++;
                    SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);                    
                }
                SDL_RenderDrawLine(renderer, x1, 0, x1, 10);
                //SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
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

void compoutputs(vector<double>* input, vector<double>* output, vector<double>* weight) {
    for (int i = 0; i < rows; i++) {
        (*output)[i] = 0;
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k < countoff; k++)
                (*output)[i] += iget(input, i, j, k) * igetw(weight, /*i,*/ j, k);
        }
    }
};

double detecterror(vector<double>* output) {
    /*for (int i = 0; i < rows; i++) {
        (*output)[i] = 0;
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k < countoff; k++)
                (*output)[i] += iget(input, i, j, k) * igetw(weight, i, j, k);
        }
    }*/
    double reserror = 0;
    for (int k = 0; k < (*output).size(); k++)
        reserror += abs((*output)[k] - datavect[k + inputsize].open);
    return reserror;
};

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void detectbest(int index,vector<double>* input, vector<double>* output, vector<double>* weight, vector<double>* addkoef) {    
    //for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            //for (int k = 0; k < countoff; k++)
            int k = index;
            {                
                double locrand = fRand(0, 2);
                double origweight = igetw(weight, j, k);//(*weight)[k];
                compoutputs(input, output, weight);
                double geterror = detecterror(output);

                ipuschw(weight, j, k, origweight + igeta(addkoef, j, k));
                compoutputs(input, output, weight);
                double geterror1 = detecterror(output);

                ipuschw(weight, j, k, origweight - igeta(addkoef, j, k));
                compoutputs(input, output, weight);
                double geterror2 = detecterror(output);

                if ((geterror < geterror1) && (geterror < geterror2))
                {
                    ipuschw(weight, j, k, origweight);
                    ipuscha(addkoef, j, k, 0.91 * locrand * igeta(addkoef, j, k));
                    //cout << ":0 " << geterror << endl;
                    //cout << geterror << ":" << geterror1 << ":" << geterror2 <<endl;
                }
                else if (geterror1 < geterror2)
                {
                    ipuschw(weight, j, k, origweight + igeta(addkoef, j, k));
                    ipuscha(addkoef, j, k, 1.99 * locrand * igeta(addkoef, j, k));
                    //cout << ":1 " << geterror1 << endl;
                    //cout << geterror << ":" << geterror1 << ":" << geterror2 << endl;
                }
                else
                {
                    ipuschw(weight, j, k, origweight - igeta(addkoef, j, k));
                    ipuscha(addkoef, j, k, 1.99 * locrand * igeta(addkoef, j, k));
                    //cout << ":2 " << geterror2 << endl;
                    //cout << geterror << ":" << geterror1 << ":" << geterror2 << endl;
                }
            }
            /*
            std::this_thread::sleep_for(std::chrono::nanoseconds(10));
            std::this_thread::sleep_until(std::chrono::system_clock::now() + std::chrono::seconds(1));
            */
            //(*output)[i] += iget(input, i, j, k) * igetw(weight, i, j, k);
        }
    }
    /*
    for (int k = 0; k < (*input).size(); k++) {
        double origweight = (*weight)[k];
        //igetw(vector<double>* weight, int y, int index)
        compoutputs(input, output, weight);
        double geterror = detecterror(output);

        (*weight)[k]= origweight + (*addkoef)[k];
        compoutputs(input, output, weight);
        double geterror1 = detecterror(output);

        (*weight)[k] = origweight - (*addkoef)[k];
        compoutputs(input, output, weight);
        double geterror2 = detecterror(output);

        if ((geterror < geterror1) && (geterror < geterror2))
        {
            (*weight)[k]= origweight;
            (*addkoef)[k] *= 0.9;
        }
        else if (geterror1 < geterror2)
        {
            (*weight)[k] = origweight + (*addkoef)[k];
            (*addkoef)[k] *= 1.1;
        }
        else
        {
            (*weight)[k] = origweight - (*addkoef)[k];
            (*addkoef)[k] *= 1.1;
        }
    }*/
};

void cleanweights(int index,vector<double>* weight)
{
    for (int j = 0; j < cols; j++)
    {
        //for (int k = 0; k < index; k++)
        {
            ipuschw(weight, j, index, 0);
        }            
    }    
};


int mainx(SDL_Renderer* renderer) {

    parseCSV((char*)"c:\\prenos\\NeuralFin\\TSLA.csv");
    parseCSVother((char*)"c:\\prenos\\NeuralFin\\GOOG.csv");
    parseCSVother((char*)"c:\\prenos\\NeuralFin\\MSFT.csv");
    parseCSVother((char*)"c:\\prenos\\NeuralFin\\AMZN.csv");

    findKoef();

    cols = (inputsize + (inputsize - 1) + (inputsize - 2));
    //int cols2 = (inputsize + (inputsize - 1) + (inputsize - 2)) * countoff;
    rows = datavect.size() - inputsize /*- outputsize*/;
    vector<double> input(cols * rows * countoff);
    vector<double> weight(cols/* * rows*/ * countoff);
    vector<double> addkoef(cols/* * rows*/ * countoff);

    vector<double> output(rows);
    for (int i = 0; i < weight.size(); i++)
    {
        weight[i] = 1;
        addkoef[i] = 0.1;
    }
    //init
    for (int i = 0; i < rows; i++)
    {
        //if (i == 62)
        //   cout << i;
        for (int j = 0; j < inputsize; j++)
        {
            ipusch(&input, i, j, 0, datavect[i + j].open);
            ipusch(&input, i, j, 1, datavect[i + j].open * datavect[i + j].open);
            ipusch(&input, i, j, 2, sqrt(abs(datavect[i + j].open)));
            ipusch(&input, i, j, 3, log(1 + abs(datavect[i + j].open)));
        }
        for (int j = 0; j < inputsize - 1; j++)
        {
            double der1 = datavect[i + j].open + datavect[i + j + 1].open;
            ipusch(&input, i, j + inputsize, 0, der1);
            ipusch(&input, i, j + inputsize, 1, der1 * der1);
            ipusch(&input, i, j + inputsize, 2, sqrt(abs(der1)));
            ipusch(&input, i, j + inputsize, 3, log(1 + abs(der1)));
        }
        for (int j = 0; j < inputsize - 2; j++)
        {
            double der1 = datavect[i + j].open + datavect[i + j + 1].open;
            double der2 = datavect[i + j + 1].open + datavect[i + j + 2].open;
            double der3 = der1 - der2;
            ipusch(&input, i, j + inputsize * 2 - 1, 0, der3);
            ipusch(&input, i, j + inputsize * 2 - 1, 1, der3 * der3);
            ipusch(&input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
            ipusch(&input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
        }
    }
    //init

    //init2
    for (int oo = 0; oo < countother; oo++)
    for (int i = 0; i < rows; i++)
    {
        //if (i == 62)
        //   cout << i;
        for (int j = 0; j < inputsize; j++)
        {
            ipusch(&input, i, j, 0, dataother[oo][i + j].open);
            ipusch(&input, i, j, 1, dataother[oo][i + j].open * dataother[oo][i + j].open);
            ipusch(&input, i, j, 2, sqrt(abs(dataother[oo][i + j].open)));
            ipusch(&input, i, j, 3, log(1 + abs(dataother[oo][i + j].open)));
        }
        for (int j = 0; j < inputsize - 1; j++)
        {
            double der1 = dataother[oo][i + j].open + dataother[oo][i + j + 1].open;
            ipusch(&input, i, j + inputsize, 0, der1);
            ipusch(&input, i, j + inputsize, 1, der1 * der1);
            ipusch(&input, i, j + inputsize, 2, sqrt(abs(der1)));
            ipusch(&input, i, j + inputsize, 3, log(1 + abs(der1)));
        }
        for (int j = 0; j < inputsize - 2; j++)
        {
            double der1 = dataother[oo][i + j].open + dataother[oo][i + j + 1].open;
            double der2 = dataother[oo][i + j + 1].open + dataother[oo][i + j + 2].open;
            double der3 = der1 - der2;
            ipusch(&input, i, j + inputsize * 2 - 1, 0, der3);
            ipusch(&input, i, j + inputsize * 2 - 1, 1, der3 * der3);
            ipusch(&input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
            ipusch(&input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
        }
    }
    //init2

    cleanweights(0, &weight);
    cleanweights(1, &weight);
    cleanweights(2, &weight);
    cleanweights(3, &weight);

    //steps
    for (int steps = 0; steps < 1000000; steps++)
    {
        //for (int i = 0; i < cols * rows * countoff; i++)
        detectbest(0, &input, &output, &weight, &addkoef);
        detectbest(1, &input, &output, &weight, &addkoef);
        detectbest(2, &input, &output, &weight, &addkoef);
        detectbest(3, &input, &output, &weight, &addkoef);

        //compoutputs(&input,&output,&weight);
        drawgraph(renderer, &output, 0);
        cout << countok << " " << countno << endl;
    }
    //steps

    return 0;
}


int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;

        if (SDL_CreateWindowAndRenderer(rendx, rendy, 0, &window, &renderer) == 0) {
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