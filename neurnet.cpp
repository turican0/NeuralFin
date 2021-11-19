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
#include <iomanip>  // std::setprecision()
//#include <limits>
//#include <map>

//#include "openner/Matrix.hpp"
//#include "openner/utils/Math.hpp"
//#include "openner/NeuralNetwork.hpp"

#include "SDL.h"
#include <SDL_image.h>

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

int inputsize = 30;//; 30;
int outputsize = 1;
int countoff = 6;// 11;// 4;
int rowtrunc = 5000;// 2500;// 10000;
int countofder = 3;//3;

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
        if (x > rowtrunc)break;
    }
    datavect.pop_back();
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
        if (x > rowtrunc)break;
    }
    countother++;
};

double koef = 1;

void findKoef() {
    double maxx = 0;
    for (data_t actdata : datavect)
        /*(x in datavect)*/ {
        if (actdata.close > maxx)maxx = actdata.close;
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
            input.push_back(datavect[i+ addx].close/ koef);

        vector<double> target;
        for (int i = 0; i < outputsize; i++)
            target.push_back(datavect[i + inputsize + addx].close / koef);

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
                    input[k]=datavect[k + addx].close / koef;
                for (int k = 0; k < outputsize; k++)
                    target[k]=datavect[k + inputsize + addx].close / koef;
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

int cols;
int rows;

void ipusch(int ooindex, vector<double>* input, int x, int y, int index, double value) {
    (*input)[ooindex * cols * rows * countoff + countoff * (cols * x + y) + index] = value;
};

double iget(int ooindex, vector<double>* input, int x, int y, int index) {
    return (*input)[ooindex * cols * rows * countoff + countoff * (cols * x + y) + index];
};

void ipuschw(int ooindex, vector<double>* weight, /*int x,*/ int y, int index, double value) {
    (*weight)[ooindex* countoff* cols +countoff * (/*cols * x +*/ y)+index] = value;
};

double igetw(int ooindex, vector<double>* weight, /*int x,*/ int y, int index) {
    return (*weight)[ooindex * countoff * cols + countoff * (/*cols * x +*/ y)+index];
};

void ipuscha(int ooindex, vector<double>* addkoef, /*int x,*/ int y, int index, double value) {
    (*addkoef)[ooindex * countoff * cols + countoff * (/*cols * x +*/ y)+index] = value;
};

double igeta(int ooindex, vector<double>* addkoef, /*int x,*/ int y, int index) {
    return (*addkoef)[ooindex * countoff * cols + countoff * (/*cols * x +*/ y)+index];
};


int rendx = 640 * 2;
int rendy = 480 * 2;

int countok;
int countno;
double profitx;

void drawgraph(SDL_Renderer* renderer, vector<double>* output, int pos, vector<double>* weight,char* argv2,int plusday) {
    SDL_Event event;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
    
    int oldx, oldy;

    countok=0;
    countno=0;

    profitx = 100;

    for (int i = 0; i < datavect.size(); i++)
    {
        int x1 = ((double)i / datavect.size()) * rendx;
        int y1 = rendy - (datavect[i].close / (koef / 3) * rendy);
        if (i > 0)
            SDL_RenderDrawLine(renderer, x1, y1, oldx, oldy);
        oldx = x1;
        oldy = y1;
    }
    vector<double> input;
    for (int k = 0; k < inputsize; k++)
        input.push_back(0);
    //for (int posi = 0; posi < datavect.size() - outputsize - inputsize; posi++)
    for (int posi = 0; posi < datavect.size() - inputsize- plusday; posi++)
    {
        for (int k = 0; k < inputsize; k++)
            input[k] = datavect[k + posi].close / koef;

        SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

        for (int i = 0; i < outputsize; i++) {
            double y = (*output)[i + posi];
            int x1 = ((double)(i + posi + inputsize+ plusday) / datavect.size()) * rendx;
            int y1 = rendy - (y / (koef / 3) * rendy);
            if (i > 0)
                SDL_RenderDrawLine(renderer, x1, y1, oldx, oldy);
            else
            {
                double prex1 = ((double)(posi + inputsize - 1) / datavect.size()) * rendx;
                double prey1 = rendy - (datavect[posi + inputsize - 1].close / (koef / 3) * rendy);
                //int prex1 = ((double)posi / datavect.size()) * rendx;
                //int prey1 = rendy - (datavect[posi].close / (koef / 3) * rendy);                
                double truey1 = datavect[posi + inputsize - 1].close;
                double truey2 = datavect[posi + inputsize+ plusday].close;
                double predy1 = datavect[posi + inputsize - 1].close;
                double predy2 = y;
                int truepos = 0;
                double diff = abs(truey2 - y);
                if (truey1 < truey2)truepos = 1;
                int predpos = 0;
                if (predy1 < predy2) {
                    predpos = 1; profitx *= truey2 / truey1;
                }

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
                SDL_RenderDrawLine(renderer, x1, 0, x1, 10+ diff);
                //SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawLine(renderer, prex1, prey1, x1, y1);
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            }
            oldx = x1;
            oldy = y1;
        }
    }
    
    int koefxw = 5;
    int koefyw = 500;
    int xweight = 0;
    int yweight = 200;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
    for (int wo = 0; wo < (cols-1)/ countofder; wo++)
    {
        //double w0 = igetw(0, weight, j, k);
        double w0 = igetw(0, weight, wo* countofder, 0);
        double w1 = igetw(0, weight, (wo+1)* countofder, 0);
        SDL_RenderDrawLine(renderer, xweight+wo* koefxw, yweight+ w0* koefyw, xweight + (wo+1) * koefxw, yweight + w1 * koefyw);
    }
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);

    //SDL_RenderDrawLine(renderer, 320, 200, 300, 240);
    //SDL_RenderDrawLine(renderer, 300, 240, 340, 240);
    //SDL_RenderDrawLine(renderer, 340, 240, 320, 200);
    SDL_RenderPresent(renderer);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    Uint32 rmask = 0xff000000;
    Uint32 gmask = 0x00ff0000;
    Uint32 bmask = 0x0000ff00;
    Uint32 amask = 0x000000ff;
#else
    Uint32 rmask = 0x000000ff;
    Uint32 gmask = 0x0000ff00;
    Uint32 bmask = 0x00ff0000;
    Uint32 amask = 0xff000000;
#endif

    //int fenetre = SDL_GetWindowFromId(touche.windowID); // "touche" is a   SDL_KeyboardEvent, "fenetre" is a SDL_window pointer

    //int r_copie = SDL_GetRenderer(fenetre);

    SDL_Surface* s_SnapSource = SDL_CreateRGBSurface(0, rendx, rendy, 32,
        rmask,
        gmask,
        bmask,
        amask); // s_SnapSource is a SDL_Surface pointer
    SDL_LockSurface(s_SnapSource);
    SDL_RenderReadPixels(renderer, NULL, s_SnapSource->format->format,
        s_SnapSource->pixels, s_SnapSource->pitch);

    char path[512];
    sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%d\\%s-screen.png", plusday,argv2);
    SDL_SaveBMP(s_SnapSource, path); // NomFichier is a char*
    SDL_UnlockSurface(s_SnapSource);
    SDL_FreeSurface(s_SnapSource);

    //SDL_SaveBMP(renderer, "screenshot.png");

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            done = SDL_TRUE;
        }
    }
};

void compoutputs(vector<double>* input, vector<double>* output, vector<double>* weight) {
    for (int i = 0; i < rows; i++)
    {
        (*output)[i] = 0;    
        for (int j = 0; j < cols; j++)
        {
            for (int k = 0; k < countoff; k++)
            {
                //(*output)[i] += iget(0, input, i, j, k) * igetw(0, weight, /*i,*/ j, k);
                for (int oo = 0; oo < countother+1; oo++)
                {
                    (*output)[i] += iget(oo/* + 1*/, input, i, j, k) * igetw(oo/* + 1*/, weight, /*i,*/ j, k);
                }
            }
        }
    }
};

double lasterror;

double detecterror(vector<double>* output,int plusday) {
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
        reserror += abs((*output)[k] - datavect[k + inputsize+ plusday].close);
    return reserror;
};

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void detectbest(int oo, int index,vector<double>* input, vector<double>* output, vector<double>* weight, vector<double>* addkoef,int plusday) {
    //for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            //for (int k = 0; k < countoff; k++)
            int k = index;
            {                
                double locrand = fRand(0, 2);
                double origweight = igetw(oo, weight, j, k);//(*weight)[k];
                compoutputs(input, output, weight);
                double geterror = detecterror(output, plusday);

                ipuschw(oo, weight,j, k, origweight + igeta(0, addkoef, j, k));
                compoutputs(input, output, weight);
                double geterror1 = detecterror(output, plusday);

                ipuschw(oo, weight, j, k, origweight - igeta(0, addkoef, j, k));
                compoutputs(input, output, weight);
                double geterror2 = detecterror(output, plusday);

                if ((geterror < geterror1) && (geterror < geterror2))
                {
                    ipuschw(oo, weight, j, k, origweight);
                    ipuscha(oo, addkoef, j, k, 0.99 * locrand * igeta(0, addkoef, j, k));
                    lasterror = geterror;
                    //cout << ":0 " << geterror << endl;
                    //cout << geterror << ":" << geterror1 << ":" << geterror2 <<endl;
                }
                else if (geterror1 < geterror2)
                {
                    ipuschw(oo, weight, j, k, origweight + igeta(0, addkoef, j, k));
                    ipuscha(oo, addkoef, j, k, 1.01 * locrand * igeta(0, addkoef, j, k));
                    lasterror = geterror1;
                    //cout << ":1 " << geterror1 << endl;
                    //cout << geterror << ":" << geterror1 << ":" << geterror2 << endl;
                }
                else
                {
                    ipuschw(oo, weight, j, k, origweight - igeta(0, addkoef, j, k));
                    ipuscha(oo, addkoef, j, k, 1.01 * locrand * igeta(0, addkoef, j, k));
                    lasterror = geterror2;
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
/*
void detectbestoth(int oo,int index, vector<double>* input, vector<double>* output, vector<double>* weight, vector<double>* addkoef) {
    //for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            //for (int k = 0; k < countoff; k++)
            int k = index;
            {
                //double locrand = fRand(0, 2);
                double origweight = igetw(oo+1, weight, j, k);//(*weight)[k];
                compoutputs(oo,input, output, weight);
                double geterror = detecterror(output);

                ipuschw(oo+1, weight, j, k, origweight + igeta(oo + 1, addkoef, j, k));
                compoutputs(oo + 1, input, output, weight);
                double geterror1 = detecterror(output);

                ipuschw(oo+1, weight, j, k, origweight - igeta(oo + 1, addkoef, j, k));
                compoutputs(oo + 1, input, output, weight);
                double geterror2 = detecterror(output);

                if ((geterror < geterror1) && (geterror < geterror2))
                {
                    ipuschw(oo+1, weight, j, k, origweight);
                    //ipuscha(oo + 1, addkoef, j, k, 0.9999 * locrand * igeta(oo+1, addkoef, j, k));
                    //cout << ":0 " << geterror << endl;
                    //cout << geterror << ":" << geterror1 << ":" << geterror2 <<endl;
                }
                else if (geterror1 < geterror2)
                {
                    ipuschw(oo+1, weight, j, k, origweight + igeta(oo + 1, addkoef, j, k));
                    //ipuscha(oo + 1, addkoef, j, k, 1.0001 * locrand * igeta(oo + 1, addkoef, j, k));
                    //cout << ":1 " << geterror1 << endl;
                    //cout << geterror << ":" << geterror1 << ":" << geterror2 << endl;
                }
                else
                {
                    ipuschw(oo+1, weight, j, k, origweight - igeta(oo + 1, addkoef, j, k));
                    //ipuscha(oo + 1, addkoef, j, k, 1.0001 * locrand * igeta(oo + 1, addkoef, j, k));
                    //cout << ":2 " << geterror2 << endl;
                    //cout << geterror << ":" << geterror1 << ":" << geterror2 << endl;
                }
            }
            //(*output)[i] += iget(input, i, j, k) * igetw(weight, i, j, k);
        }
    }
};
*/
void cleanweights(int ooindex, int index,vector<double>* weight)
{
    for (int j = 0; j < cols; j++)
    {
        //for (int k = 0; k < index; k++)
        {
            ipuschw(ooindex, weight, j, index, 0);
        }            
    }    
};

void savedata(vector<double>* weight,char* filename) {
    ofstream myfile;
    myfile.open(filename);
    for(int i=0;i<(*weight).size();i++)
        myfile << std::setprecision(17) << (*weight)[i] <<"\n";
    myfile.close();
};

void savetobestlog(char* buffer, char* filename,int minsize) {
    char buffer2[256]="";
    for (int i = 0; i < 256; i++)buffer2[i]=' ';
    for (int i = 0; i < strlen(buffer); i++)buffer2[i] = buffer[i];
    buffer2[minsize] = 0;
    ofstream myfile;
    myfile.open(filename, std::ios_base::app);
    myfile << buffer2 << endl;
    myfile.close();
};

void loaddata(vector<double>* weight, char* filename) {
    std::ifstream  data(filename);
    std::string line;
    int x = 0;
    while ((std::getline(data, line)))
    {
        (*weight)[x]=std::stod(line);
        x++;
    }
};

//char* othfilenames[20] = { (char*)"0000000000000000" ,(char*)"0000000000000000" ,(char*)"0000000000000000" };
//char* mainfilename = (char*)"0000000000000000";
int countofothfiles = 3;

void optimize(SDL_Renderer* renderer, int argc, char* argv[]) {
    cout << "--- " << argv[3] << " - " << argv[1] << " ---" << endl;
    int plusday = stoi(argv[2]);
    char path[512];
    sprintf_s(path,"c:\\prenos\\NeuralFin\\data\\%s.csv", argv[3]);
    parseCSV(path);
    cout << "date: " << datavect[datavect.size() - 1].ear << "-" << datavect[datavect.size() - 1].moon << "-" << datavect[datavect.size() - 1].day << endl;
    for (int oi = 0; oi < argc - 4; oi++)
    {
        sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%s.csv", argv[4 + oi]);
        parseCSVother(path);
    }

    findKoef();

    cols = 0;
    if (countofder > 0)cols += inputsize;
    if (countofder > 1)cols += inputsize-1;
    if (countofder > 2)cols += inputsize-2;

    //cols = (inputsize + (inputsize - 1) + (inputsize - 2));
    //int cols2 = (inputsize + (inputsize - 1) + (inputsize - 2)) * countoff;
    rows = datavect.size() - inputsize /*- outputsize*/- plusday;
    vector<double> input(cols * rows * countoff*(1+ countother));
    vector<double> weight(cols/* * rows*/ * countoff*(1+countother));
    vector<double> addkoef(cols/* * rows*/ * countoff*(1 + countother));

    //vector<double> inputoth(countother *cols * rows * countoff);

    vector<double> output(rows);

    for (int i = 0; i < weight.size(); i++)
    {
        weight[i] = 1;
        addkoef[i] = 0.5;
    }

    //init
    for (int i = 0; i < rows; i++)
    {
        //if (i == 62)
        //   cout << i;
        if (countofder > 0)
        for (int j = 0; j < inputsize; j++)
        {
            if (countoff > 0)ipusch(0,&input, i, j, 0, datavect[i + j].close);
            if (countoff > 1)ipusch(0,&input, i, j, 1, datavect[i + j].close * datavect[i + j].close);
            if (countoff > 2)ipusch(0,&input, i, j, 2, sqrt(abs(datavect[i + j].close)));
            if (countoff > 3)ipusch(0,&input, i, j, 3, log(1 + abs(datavect[i + j].close)));
            if (countoff > 4)ipusch(0, &input, i, j, 4, datavect[i + j].high);
            if (countoff > 5)ipusch(0, &input, i, j, 5, datavect[i + j].low);
        }
        if (countofder > 1)
        for (int j = 0; j < inputsize - 1; j++)
        {
            double der1 = datavect[i + j + 1].close - datavect[i + j].close;
            if (countoff > 0)ipusch(0, &input, i, j + inputsize, 0, der1);
            if (countoff > 1)ipusch(0, &input, i, j + inputsize, 1, der1 * der1);
            if (countoff > 2)ipusch(0, &input, i, j + inputsize, 2, sqrt(abs(der1)));
            if (countoff > 3)ipusch(0, &input, i, j + inputsize, 3, log(1 + abs(der1)));
            der1 = datavect[i + j + 1].high - datavect[i + j].high;
            if (countoff > 4)ipusch(0, &input, i, j + inputsize, 4, der1);
            der1 = datavect[i + j + 1].low - datavect[i + j].low;
            if (countoff > 5)ipusch(0, &input, i, j + inputsize, 5, der1);
        }
        if (countofder > 2)
        for (int j = 0; j < inputsize - 2; j++)
        {
            double der3 = datavect[i + j + 2].close - 2 * datavect[i + j + 1].close + datavect[i + j].close;
            if (countoff > 0)ipusch(0, &input, i, j + inputsize * 2 - 1, 0, der3);
            if (countoff > 1)ipusch(0, &input, i, j + inputsize * 2 - 1, 1, der3 * der3);
            if (countoff > 2)ipusch(0, &input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
            if (countoff > 3)ipusch(0, &input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
            der3 = datavect[i + j + 2].high - 2 * datavect[i + j + 1].high + datavect[i + j].high;
            if (countoff > 4)ipusch(0, &input, i, j + inputsize * 2 - 1, 4, der3);
            der3 = datavect[i + j + 2].low - 2 * datavect[i + j + 1].high + datavect[i + j].low;
            if (countoff > 5)ipusch(0, &input, i, j + inputsize * 2 - 1, 5, der3);
        }
    }
    //init

    //init2
    for (int oo = 0; oo < countother; oo++)
    for (int i = 0; i < rows; i++)
    {
        //if (i == 62)
        //   cout << i;
        if (countofder > 0)
        for (int j = 0; j < inputsize; j++)
        {
            if (countoff > 0)ipusch(oo + 1,&input, i, j, 0, dataother[oo][i + j].close);
            if (countoff > 1)ipusch(oo + 1,&input, i, j, 1, dataother[oo][i + j].close * dataother[oo][i + j].close);
            if (countoff > 2)ipusch(oo + 1, &input, i, j, 2, sqrt(abs(dataother[oo][i + j].close)));
            if (countoff > 3)ipusch(oo + 1, &input, i, j, 3, log(1 + abs(dataother[oo][i + j].close)));
            if (countoff > 4)ipusch(oo + 1, &input, i, j, 4, dataother[oo][i + j].high);
            if (countoff > 5)ipusch(oo + 1, &input, i, j, 5, dataother[oo][i + j].low);
        }
        if (countofder > 1)
        for (int j = 0; j < inputsize - 1; j++)
        {
            double der1 = dataother[oo][i + j + 1].close - dataother[oo][i + j].close;
            if (countoff > 0)ipusch(oo + 1, &input, i, j + inputsize, 0, der1);
            if (countoff > 1)ipusch(oo + 1, &input, i, j + inputsize, 1, der1 * der1);
            if (countoff > 2)ipusch(oo + 1, &input, i, j + inputsize, 2, sqrt(abs(der1)));
            if (countoff > 3)ipusch(oo + 1, &input, i, j + inputsize, 3, log(1 + abs(der1)));
            der1 = dataother[oo][i + j + 1].high - dataother[oo][i + j].high;
            if (countoff > 4)ipusch(oo + 1, &input, i, j + inputsize, 4, der1);
            der1 = dataother[oo][i + j + 1].low - dataother[oo][i + j].low;
            if (countoff > 5)ipusch(oo + 1, &input, i, j + inputsize, 5, der1);
        }
        if (countofder > 2)
        for (int j = 0; j < inputsize - 2; j++)
        {
            double der3 = dataother[oo][i + j + 2].close - 2*dataother[oo][i + j + 1].close + dataother[oo][i + j].close;
            if (countoff > 0)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 0, der3);
            if (countoff > 1)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 1, der3 * der3);
            if (countoff > 2)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
            if (countoff > 3)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
            der3 = dataother[oo][i + j + 2].high - 2 * dataother[oo][i + j + 1].high + dataother[oo][i + j].high;
            if (countoff > 4)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 4, der3);
            der3 = dataother[oo][i + j + 2].low - 2 * dataother[oo][i + j + 1].low + dataother[oo][i + j].low;
            if (countoff > 5)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 5, der3);
        }
    }
    //init2

    if (countoff > 0)cleanweights(0,0, &weight);
    if (countoff > 1)cleanweights(0,1, &weight);
    if (countoff > 2)cleanweights(0,2, &weight);
    if (countoff > 3)cleanweights(0,3, &weight);
    if (countoff > 4)cleanweights(0, 4, &weight);
    if (countoff > 5)cleanweights(0, 5, &weight);

    for (int oo = 0; oo < countother; oo++)
    {
        if (countoff > 0)cleanweights(oo + 1, 0, &weight);
        if (countoff > 1)cleanweights(oo + 1, 1, &weight);
        if (countoff > 2)cleanweights(oo + 1, 2, &weight);
        if (countoff > 3)cleanweights(oo + 1, 3, &weight);
        if (countoff > 4)cleanweights(oo + 1, 4, &weight);
        if (countoff > 5)cleanweights(oo + 1, 5, &weight);
    //ipusch(&input, i, j, 0, dataother[oo][i + j].close);
    }

    //char path[512];
    sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%d\\%s-weight.csv", plusday,argv[3]);
    loaddata(&weight, path);

    cout << "steps 1" << endl;
    for (int step = 0; step < 5; step++)
    {
        //steps 1
        for (int steps = 0; steps < 20; steps++)
        {
            //for (int i = 0; i < cols * rows * countoff; i++)
            if (countoff > 0)detectbest(0,0, &input, &output, &weight, &addkoef, plusday);
            //if (countoff > 1)detectbest(0,1, &input, &output, &weight, &addkoef);
            //if (countoff > 2)detectbest(0,2, &input, &output, &weight, &addkoef);
            //if (countoff > 3)detectbest(0,3, &input, &output, &weight, &addkoef);
            cout << "0 " << endl;
            cout << steps << " - ";
            drawgraph(renderer, &output, 0, &weight, argv[3], plusday);
            savedata(&weight,path);
            cout << countok << " " << countno <<"("<<profitx<<"|"<<rows<<"):"<< lasterror << endl;
        }
        //steps 1

        cout << "steps 2" << endl;
        //steps 2
        for (int steps = 0; steps < 10; steps++)
        {
            //for (int i = 0; i < cols * rows * countoff; i++)
            if (countoff > 0)detectbest(0, 0, &input, &output, &weight, &addkoef, plusday);
            cout << "0 ";
            if (countoff > 1)detectbest(0, 1, &input, &output, &weight, &addkoef, plusday);
            cout << "1 ";
            if (countoff > 2)detectbest(0, 2, &input, &output, &weight, &addkoef, plusday);
            cout << "2 ";
            if (countoff > 3)detectbest(0, 3, &input, &output, &weight, &addkoef, plusday);
            cout << "3 ";
            if (countoff > 4)detectbest(0, 4, &input, &output, &weight, &addkoef, plusday);
            cout << "4 ";
            if (countoff > 5)detectbest(0, 5, &input, &output, &weight, &addkoef, plusday);
            cout << "5 "<< endl;
            cout << steps << " - ";
            drawgraph(renderer, &output, 0, &weight, argv[3], plusday);
            savedata(&weight,path);
            cout << countok << " " << countno << "(" << profitx << "|" << rows << "):" << lasterror << endl;
        }
        //steps 2

        cout << "steps 3" << endl;
        //steps 3
        for (int steps = 0; steps < 5; steps++)
        {
            //for (int i = 0; i < cols * rows * countoff; i++)
            if (countoff > 0)detectbest(0, 0, &input, &output, &weight, &addkoef, plusday);
            cout << "0 ";
            if (countoff > 1)detectbest(0, 1, &input, &output, &weight, &addkoef, plusday);
            cout << "1 ";
            if (countoff > 2)detectbest(0, 2, &input, &output, &weight, &addkoef, plusday);
            cout << "2 ";
            if (countoff > 3)detectbest(0, 3, &input, &output, &weight, &addkoef, plusday);
            cout << "3 ";
            if (countoff > 4)detectbest(0, 4, &input, &output, &weight, &addkoef, plusday);
            cout << "4 ";
            if (countoff > 5)detectbest(0, 5, &input, &output, &weight, &addkoef, plusday);
            cout << "5 ";
            for (int oo = 0; oo < countother; oo++)
            {
                if (countoff > 0)detectbest(oo + 1, 0, &input, &output, &weight, &addkoef, plusday);
                cout << oo <<"x0 ";
                if (countoff > 1)detectbest(oo + 1, 1, &input, &output, &weight, &addkoef, plusday);
                cout << oo << "x1 ";
                if (countoff > 2)detectbest(oo + 1, 2, &input, &output, &weight, &addkoef, plusday);
                cout << oo << "x2 ";
                if (countoff > 3)detectbest(oo + 1, 3, &input, &output, &weight, &addkoef, plusday);
                cout << oo << "x3 ";
                if (countoff > 4)detectbest(oo + 1, 4, &input, &output, &weight, &addkoef, plusday);
                cout << oo << "x4 ";
                if (countoff > 5)detectbest(oo + 1, 5, &input, &output, &weight, &addkoef, plusday);
                cout << oo << "x5 ";
                //ipusch(&input, i, j, 0, dataother[oo][i + j].close);
            }
            cout << endl;
            //compoutputs(&input,&output,&weight);
            cout << steps << " - ";
            drawgraph(renderer, &output, 0, &weight, argv[3], plusday);
            savedata(&weight,path);
            cout << countok << " " << countno << "(" << profitx << "|" << rows << "):" << lasterror << endl;
        }
        //steps 3
    }
    //return 0;
}

void printscore(SDL_Renderer* renderer, int argc, char* argv[]) {
    cout << "--- " << argv[3] << " - " << argv[1] << " ---" << endl;
    int plusday = stoi(argv[2]);
    char path[512];
    sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%s.csv", argv[3]);
    parseCSV(path);
    cout << "date: " << datavect[datavect.size() - 1].ear << "-" << datavect[datavect.size() - 1].moon << "-" << datavect[datavect.size() - 1].day << endl;
    for (int oi = 0; oi < argc - 4; oi++)
    {
        sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%s.csv", argv[4 + oi]);
        parseCSVother(path);
    }

    findKoef();

    cols = 0;
    if (countofder > 0)cols += inputsize;
    if (countofder > 1)cols += inputsize - 1;
    if (countofder > 2)cols += inputsize - 2;

    //cols = (inputsize + (inputsize - 1) + (inputsize - 2));
    //int cols2 = (inputsize + (inputsize - 1) + (inputsize - 2)) * countoff;
    rows = datavect.size() - inputsize /*- outputsize*/ - plusday;
    vector<double> input(cols * rows * countoff * (1 + countother));
    vector<double> weight(cols/* * rows*/ * countoff * (1 + countother));
    vector<double> addkoef(cols/* * rows*/ * countoff * (1 + countother));

    //vector<double> inputoth(countother *cols * rows * countoff);

    vector<double> output(rows);

    for (int i = 0; i < weight.size(); i++)
    {
        weight[i] = 1;
        addkoef[i] = 0.5;
    }

    //init
    for (int i = 0; i < rows; i++)
    {
        //if (i == 62)
        //   cout << i;
        if (countofder > 0)
            for (int j = 0; j < inputsize; j++)
            {
                if (countoff > 0)ipusch(0, &input, i, j, 0, datavect[i + j].close);
                if (countoff > 1)ipusch(0, &input, i, j, 1, datavect[i + j].close * datavect[i + j].close);
                if (countoff > 2)ipusch(0, &input, i, j, 2, sqrt(abs(datavect[i + j].close)));
                if (countoff > 3)ipusch(0, &input, i, j, 3, log(1 + abs(datavect[i + j].close)));
                if (countoff > 4)ipusch(0, &input, i, j, 4, datavect[i + j].high);
                if (countoff > 5)ipusch(0, &input, i, j, 5, datavect[i + j].low);
            }
        if (countofder > 1)
            for (int j = 0; j < inputsize - 1; j++)
            {
                double der1 = datavect[i + j + 1].close - datavect[i + j].close;
                if (countoff > 0)ipusch(0, &input, i, j + inputsize, 0, der1);
                if (countoff > 1)ipusch(0, &input, i, j + inputsize, 1, der1 * der1);
                if (countoff > 2)ipusch(0, &input, i, j + inputsize, 2, sqrt(abs(der1)));
                if (countoff > 3)ipusch(0, &input, i, j + inputsize, 3, log(1 + abs(der1)));
                der1 = datavect[i + j + 1].high - datavect[i + j].high;
                if (countoff > 4)ipusch(0, &input, i, j + inputsize, 4, der1);
                der1 = datavect[i + j + 1].low - datavect[i + j].low;
                if (countoff > 5)ipusch(0, &input, i, j + inputsize, 5, der1);
            }
        if (countofder > 2)
            for (int j = 0; j < inputsize - 2; j++)
            {
                double der3 = datavect[i + j + 2].close - 2 * datavect[i + j + 1].close + datavect[i + j].close;
                if (countoff > 0)ipusch(0, &input, i, j + inputsize * 2 - 1, 0, der3);
                if (countoff > 1)ipusch(0, &input, i, j + inputsize * 2 - 1, 1, der3 * der3);
                if (countoff > 2)ipusch(0, &input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
                if (countoff > 3)ipusch(0, &input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
                der3 = datavect[i + j + 2].high - 2 * datavect[i + j + 1].high + datavect[i + j].high;
                if (countoff > 4)ipusch(0, &input, i, j + inputsize * 2 - 1, 4, der3);
                der3 = datavect[i + j + 2].low - 2 * datavect[i + j + 1].high + datavect[i + j].low;
                if (countoff > 5)ipusch(0, &input, i, j + inputsize * 2 - 1, 5, der3);
            }
    }
    //init

    //init2
    for (int oo = 0; oo < countother; oo++)
        for (int i = 0; i < rows; i++)
        {
            //if (i == 62)
            //   cout << i;
            if (countofder > 0)
                for (int j = 0; j < inputsize; j++)
                {
                    if (countoff > 0)ipusch(oo + 1, &input, i, j, 0, dataother[oo][i + j].close);
                    if (countoff > 1)ipusch(oo + 1, &input, i, j, 1, dataother[oo][i + j].close * dataother[oo][i + j].close);
                    if (countoff > 2)ipusch(oo + 1, &input, i, j, 2, sqrt(abs(dataother[oo][i + j].close)));
                    if (countoff > 3)ipusch(oo + 1, &input, i, j, 3, log(1 + abs(dataother[oo][i + j].close)));
                    if (countoff > 4)ipusch(oo + 1, &input, i, j, 4, dataother[oo][i + j].high);
                    if (countoff > 5)ipusch(oo + 1, &input, i, j, 5, dataother[oo][i + j].low);
                }
            if (countofder > 1)
                for (int j = 0; j < inputsize - 1; j++)
                {
                    double der1 = dataother[oo][i + j + 1].close - dataother[oo][i + j].close;
                    if (countoff > 0)ipusch(oo + 1, &input, i, j + inputsize, 0, der1);
                    if (countoff > 1)ipusch(oo + 1, &input, i, j + inputsize, 1, der1 * der1);
                    if (countoff > 2)ipusch(oo + 1, &input, i, j + inputsize, 2, sqrt(abs(der1)));
                    if (countoff > 3)ipusch(oo + 1, &input, i, j + inputsize, 3, log(1 + abs(der1)));
                    der1 = dataother[oo][i + j + 1].high - dataother[oo][i + j].high;
                    if (countoff > 4)ipusch(oo + 1, &input, i, j + inputsize, 4, der1);
                    der1 = dataother[oo][i + j + 1].low - dataother[oo][i + j].low;
                    if (countoff > 5)ipusch(oo + 1, &input, i, j + inputsize, 5, der1);
                }
            if (countofder > 2)
                for (int j = 0; j < inputsize - 2; j++)
                {
                    double der3 = dataother[oo][i + j + 2].close - 2 * dataother[oo][i + j + 1].close + dataother[oo][i + j].close;
                    if (countoff > 0)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 0, der3);
                    if (countoff > 1)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 1, der3 * der3);
                    if (countoff > 2)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
                    if (countoff > 3)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
                    der3 = dataother[oo][i + j + 2].high - 2 * dataother[oo][i + j + 1].high + dataother[oo][i + j].high;
                    if (countoff > 4)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 4, der3);
                    der3 = dataother[oo][i + j + 2].low - 2 * dataother[oo][i + j + 1].low + dataother[oo][i + j].low;
                    if (countoff > 5)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 5, der3);
                }
        }
    //init2

    if (countoff > 0)cleanweights(0, 0, &weight);
    if (countoff > 1)cleanweights(0, 1, &weight);
    if (countoff > 2)cleanweights(0, 2, &weight);
    if (countoff > 3)cleanweights(0, 3, &weight);
    if (countoff > 4)cleanweights(0, 4, &weight);
    if (countoff > 5)cleanweights(0, 5, &weight);

    for (int oo = 0; oo < countother; oo++)
    {
        if (countoff > 0)cleanweights(oo + 1, 0, &weight);
        if (countoff > 1)cleanweights(oo + 1, 1, &weight);
        if (countoff > 2)cleanweights(oo + 1, 2, &weight);
        if (countoff > 3)cleanweights(oo + 1, 3, &weight);
        if (countoff > 4)cleanweights(oo + 1, 4, &weight);
        if (countoff > 5)cleanweights(oo + 1, 5, &weight);
        //ipusch(&input, i, j, 0, dataother[oo][i + j].close);
    }

    //char path[512];
    sprintf_s(path, "%s-weight.csv", argv[3]);
    loaddata(&weight, path);

    compoutputs(&input, &output, &weight);
    drawgraph(renderer, &output, 0, &weight, argv[3], plusday);
    cout << "SCORE: " << countok << " " << countno << "(" << profitx << "|" << rows << "):" << lasterror << endl;
}

void computenextday(SDL_Renderer* renderer, int argc, char* argv[]) {
    cout << "--- " << argv[3] << " - " << argv[1] << " ---" << endl;
    int plusday = stoi(argv[2]);
    char buffer[512];
    sprintf_s(buffer, "%s", argv[3]);
    char bestlogpath[512];
    sprintf_s(bestlogpath, "c:\\prenos\\NeuralFin\\data\\%d\\addlog.csv", plusday);
    char path[512];
    savetobestlog(buffer, bestlogpath,4);
    sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%s.csv", argv[3]);
    parseCSV(path);
    cout << "date: " << datavect[datavect.size() - 1].ear << "-" << datavect[datavect.size() - 1].moon << "-" << datavect[datavect.size() - 1].day << endl;

    //test day
    std::time_t t = std::time(0);   // get time now
    errno_t err;
    __time64_t long_time;
    struct tm newtime;
    //std::tm* now = std::localtime_s(&t);
    _time64(&long_time);
    err = _localtime64_s(&newtime, &long_time);
    --newtime.tm_mday; // Move back one day
    mktime(&newtime); // Normalize
    if (newtime.tm_year + 1900 != datavect[datavect.size() - 1].ear) {
        cout << endl << "!!! incorect ear !!!" << endl;
        //cin.ignore(1);
        std::cin.get();
        exit(0);
    }
    if (newtime.tm_mon + 1 != datavect[datavect.size() - 1].moon) {
        cout << endl << "!!! incorect moon !!!" << endl;
        //cin.ignore(1);
        std::cin.get();
        exit(0);
    }
    if (newtime.tm_mday != datavect[datavect.size() - 1].day) {
        cout << endl << "!!! incorect day !!!" << endl;
        //cin.ignore(1);
        std::cin.get();
        exit(0);
    }
    /*std::cout << (now->tm_year + 1900) << '-'
        << (now->tm_mon + 1) << '-'
        << now->tm_mday
        << "\n";*/
    //test date

    sprintf_s(buffer, "date:%d-%d-%d", (int)datavect[datavect.size() - 1].ear, (int)datavect[datavect.size() - 1].moon, (int)datavect[datavect.size() - 1].day);
    savetobestlog(buffer, bestlogpath,14);
    for (int oi = 0; oi < argc - 4; oi++)
    {
        sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%s.csv", argv[4 + oi]);
        parseCSVother(path);
    }

    findKoef();

    cols = 0;
    if (countofder > 0)cols += inputsize;
    if (countofder > 1)cols += inputsize - 1;
    if (countofder > 2)cols += inputsize - 2;

    //cols = (inputsize + (inputsize - 1) + (inputsize - 2));
    //int cols2 = (inputsize + (inputsize - 1) + (inputsize - 2)) * countoff;
    rows = datavect.size() - inputsize+1 /*- outputsize*/ - plusday;
    vector<double> input(cols * rows * countoff * (1 + countother));
    vector<double> weight(cols/* * rows*/ * countoff * (1 + countother));
    vector<double> addkoef(cols/* * rows*/ * countoff * (1 + countother));

    //vector<double> inputoth(countother *cols * rows * countoff);

    vector<double> output(rows);

    for (int i = 0; i < weight.size(); i++)
    {
        weight[i] = 1;
        addkoef[i] = 0.5;
    }

    //init
    for (int i = 0; i < rows; i++)
    {
        //if (i == 62)
        //   cout << i;
        if (countofder > 0)
            for (int j = 0; j < inputsize; j++)
            {
                if (countoff > 0)ipusch(0, &input, i, j, 0, datavect[i + j].close);
                if (countoff > 1)ipusch(0, &input, i, j, 1, datavect[i + j].close * datavect[i + j].close);
                if (countoff > 2)ipusch(0, &input, i, j, 2, sqrt(abs(datavect[i + j].close)));
                if (countoff > 3)ipusch(0, &input, i, j, 3, log(1 + abs(datavect[i + j].close)));
                if (countoff > 4)ipusch(0, &input, i, j, 4, datavect[i + j].high);
                if (countoff > 5)ipusch(0, &input, i, j, 5, datavect[i + j].low);
            }
        if (countofder > 1)
            for (int j = 0; j < inputsize - 1; j++)
            {
                double der1 = datavect[i + j + 1].close - datavect[i + j].close;
                if (countoff > 0)ipusch(0, &input, i, j + inputsize, 0, der1);
                if (countoff > 1)ipusch(0, &input, i, j + inputsize, 1, der1 * der1);
                if (countoff > 2)ipusch(0, &input, i, j + inputsize, 2, sqrt(abs(der1)));
                if (countoff > 3)ipusch(0, &input, i, j + inputsize, 3, log(1 + abs(der1)));
                der1 = datavect[i + j + 1].high - datavect[i + j].high;
                if (countoff > 4)ipusch(0, &input, i, j + inputsize, 4, der1);
                der1 = datavect[i + j + 1].low - datavect[i + j].low;
                if (countoff > 5)ipusch(0, &input, i, j + inputsize, 5, der1);
            }
        if (countofder > 2)
            for (int j = 0; j < inputsize - 2; j++)
            {
                double der3 = datavect[i + j + 2].close - 2 * datavect[i + j + 1].close + datavect[i + j].close;
                if (countoff > 0)ipusch(0, &input, i, j + inputsize * 2 - 1, 0, der3);
                if (countoff > 1)ipusch(0, &input, i, j + inputsize * 2 - 1, 1, der3 * der3);
                if (countoff > 2)ipusch(0, &input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
                if (countoff > 3)ipusch(0, &input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
                der3 = datavect[i + j + 2].high - 2 * datavect[i + j + 1].high + datavect[i + j].high;
                if (countoff > 4)ipusch(0, &input, i, j + inputsize * 2 - 1, 4, der3);
                der3 = datavect[i + j + 2].low - 2 * datavect[i + j + 1].high + datavect[i + j].low;
                if (countoff > 5)ipusch(0, &input, i, j + inputsize * 2 - 1, 5, der3);
            }
    }
    //init

    //init2
    for (int oo = 0; oo < countother; oo++)
        for (int i = 0; i < rows; i++)
        {
            //if (i == 62)
            //   cout << i;
            if (countofder > 0)
                for (int j = 0; j < inputsize; j++)
                {
                    if (countoff > 0)ipusch(oo + 1, &input, i, j, 0, dataother[oo][i + j].close);
                    if (countoff > 1)ipusch(oo + 1, &input, i, j, 1, dataother[oo][i + j].close * dataother[oo][i + j].close);
                    if (countoff > 2)ipusch(oo + 1, &input, i, j, 2, sqrt(abs(dataother[oo][i + j].close)));
                    if (countoff > 3)ipusch(oo + 1, &input, i, j, 3, log(1 + abs(dataother[oo][i + j].close)));
                    if (countoff > 4)ipusch(oo + 1, &input, i, j, 4, dataother[oo][i + j].high);
                    if (countoff > 5)ipusch(oo + 1, &input, i, j, 5, dataother[oo][i + j].low);
                }
            if (countofder > 1)
                for (int j = 0; j < inputsize - 1; j++)
                {
                    double der1 = dataother[oo][i + j + 1].close - dataother[oo][i + j].close;
                    if (countoff > 0)ipusch(oo + 1, &input, i, j + inputsize, 0, der1);
                    if (countoff > 1)ipusch(oo + 1, &input, i, j + inputsize, 1, der1 * der1);
                    if (countoff > 2)ipusch(oo + 1, &input, i, j + inputsize, 2, sqrt(abs(der1)));
                    if (countoff > 3)ipusch(oo + 1, &input, i, j + inputsize, 3, log(1 + abs(der1)));
                    der1 = dataother[oo][i + j + 1].high - dataother[oo][i + j].high;
                    if (countoff > 4)ipusch(oo + 1, &input, i, j + inputsize, 4, der1);
                    der1 = dataother[oo][i + j + 1].low - dataother[oo][i + j].low;
                    if (countoff > 5)ipusch(oo + 1, &input, i, j + inputsize, 5, der1);
                }
            if (countofder > 2)
                for (int j = 0; j < inputsize - 2; j++)
                {
                    double der3 = dataother[oo][i + j + 2].close - 2 * dataother[oo][i + j + 1].close + dataother[oo][i + j].close;
                    if (countoff > 0)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 0, der3);
                    if (countoff > 1)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 1, der3 * der3);
                    if (countoff > 2)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
                    if (countoff > 3)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
                    der3 = dataother[oo][i + j + 2].high - 2 * dataother[oo][i + j + 1].high + dataother[oo][i + j].high;
                    if (countoff > 4)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 4, der3);
                    der3 = dataother[oo][i + j + 2].low - 2 * dataother[oo][i + j + 1].low + dataother[oo][i + j].low;
                    if (countoff > 5)ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 5, der3);
                }
        }
    //init2

    if (countoff > 0)cleanweights(0, 0, &weight);
    if (countoff > 1)cleanweights(0, 1, &weight);
    if (countoff > 2)cleanweights(0, 2, &weight);
    if (countoff > 3)cleanweights(0, 3, &weight);
    if (countoff > 4)cleanweights(0, 4, &weight);
    if (countoff > 5)cleanweights(0, 5, &weight);

    for (int oo = 0; oo < countother; oo++)
    {
        if (countoff > 0)cleanweights(oo + 1, 0, &weight);
        if (countoff > 1)cleanweights(oo + 1, 1, &weight);
        if (countoff > 2)cleanweights(oo + 1, 2, &weight);
        if (countoff > 3)cleanweights(oo + 1, 3, &weight);
        if (countoff > 4)cleanweights(oo + 1, 4, &weight);
        if (countoff > 5)cleanweights(oo + 1, 5, &weight);
        //ipusch(&input, i, j, 0, dataother[oo][i + j].close);
    }

    //char path[512];
    sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%d\\%s-weight.csv", plusday, argv[3]);
    loaddata(&weight, path);
    compoutputs(&input, &output, &weight);

    drawgraph(renderer, &output, 0, &weight, argv[3], plusday);
    //compnextday(&input, &output, &weight);
    cout << datavect[datavect.size() - 1].close << " -> " << (output)[rows - 1] << " $" << endl;
    cout << (output)[rows - 1]- datavect[datavect.size()-1].close << " $" << endl;
    cout << -(1-(output)[rows - 1]/datavect[datavect.size() - 1].close)*100 << " %" << endl;
    cout << "PROFIT: " << profitx << "%" << endl;

    sprintf_s(buffer, "PR:%0.1f", profitx/(plusday+1));
    savetobestlog(buffer, bestlogpath,9);
    
    //if(plusday==0)
        sprintf_s(buffer, "DF:%0.1f$->%0.1f$(%0.1f$)/(%0.1f$-%0.1f$)", datavect[datavect.size() - 1].close, (output)[rows - 1], (output)[rows - 1] - datavect[datavect.size() - 1].close, datavect[datavect.size() - 1].low, datavect[datavect.size() - 1].high);
    /*else
        sprintf_s(buffer, "DF:%0.1f$->%0.1f$(%0.1f$)", datavect[datavect.size() - 1].close, (output)[rows - 1], (output)[rows - 1] - datavect[datavect.size() - 1].close);    */

    savetobestlog(buffer, bestlogpath,45);
    sprintf_s(buffer, "%0.1f%s", -(1 - (output)[rows - 1] / datavect[datavect.size() - 1].close) * 100,"%");
    savetobestlog(buffer, bestlogpath,6);
};

void sortbest(int argc, char* argv[]) {
    int plusday = stoi(argv[2]);
    char buffer[512];
    sprintf_s(buffer, "c:\\prenos\\NeuralFin\\data\\%d\\addlog.csv", plusday);
    std::ifstream  data(buffer);
    std::string line;
    vector<std::string> lines;
    int x = 0;
    while ((std::getline(data, line)))
    {
        lines.push_back(line);
    }
    int percols = 5;
    std::string templine;
    int count = lines.size() / percols;
    for (int i = 1; i < count; i++)
        for (int j = 0; j < i; j++)
        {
            double comp1 = stod(lines[j * percols + 4]);
            double comp2 = stod(lines[i * percols + 4]);
            if (comp1 < comp2)
            {
                templine = lines[j * percols + 0];
                lines[j * percols + 0] = lines[i * percols + 0];
                lines[i * percols + 0] = templine;

                templine = lines[j * percols + 1];
                lines[j * percols + 1] = lines[i * percols + 1];
                lines[i * percols + 1] = templine;

                templine = lines[j * percols + 2];
                lines[j * percols + 2] = lines[i * percols + 2];
                lines[i * percols + 2] = templine;

                templine = lines[j * percols + 3];
                lines[j * percols + 3] = lines[i * percols + 3];
                lines[i * percols + 3] = templine;

                templine = lines[j * percols + 4];
                lines[j * percols + 4] = lines[i * percols + 4];
                lines[i * percols + 4] = templine;
            }
        }
    for (int i = 0; i < lines.size(); i++)
    {
        if (i % percols == 0)cout << " | ";
        if (i % percols == 1)cout << " | ";
        if (i % percols == 2)cout << " | ";
        if (i % percols == 3)cout << " | ";
        if (i % percols == 4) {
            cout << " | " << lines[i-4] <<endl;
        }
    }
};

void multisortbest(int argc, char* argv[]) {
    cout << endl;
    int typecount = stoi(argv[2]);
    char buffer[512];
    std::string line;
    vector<std::string> lines;
    for (int cc=0; cc < typecount; cc++)
    {
        sprintf_s(buffer, "c:\\prenos\\NeuralFin\\data\\%d\\addlog.csv", cc);
        std::ifstream  data(buffer);        
        int x = 0;
        while ((std::getline(data, line)))
        {
            lines.push_back(line);
        }
    }
    int subsize = lines.size() / typecount;
    int percols = 5;
    std::string templine;
    int count = subsize / percols;
    double score[500];
    for (int i = 0; i < count; i++)
    {
        score[i] = 0;
        double weight = 1;
        for (int k = 0; k < typecount; k++)
        {
            score[i] += weight * stod(lines[count * percols * k + i * percols + 4]);
            weight *= 0.5;
        }
    }
    for (int i = 1; i < count; i++)
        for (int j = 0; j < i; j++)
        {
            //double comp1 = stod(lines[j * percols + 4]);
            //double comp2 = stod(lines[i * percols + 4]);
            if (score[j] < score[i])
            {
                double tempsc = score[j];
                score[j] = score[i];
                score[i] = tempsc;

                for (int k = 0; k < typecount; k++)
                {
                    templine = lines[count * percols * k + j * percols + 0];
                    lines[count * percols * k + j * percols + 0] = lines[count * percols * k + i * percols + 0];
                    lines[count * percols * k + i * percols + 0] = templine;

                    templine = lines[count * percols * k + j * percols + 1];
                    lines[count * percols * k + j * percols + 1] = lines[count * percols * k + i * percols + 1];
                    lines[count * percols * k + i * percols + 1] = templine;

                    templine = lines[count * percols * k + j * percols + 2];
                    lines[count * percols * k + j * percols + 2] = lines[count * percols * k + i * percols + 2];
                    lines[count * percols * k + i * percols + 2] = templine;

                    templine = lines[count * percols * k + j * percols + 3];
                    lines[count * percols * k + j * percols + 3] = lines[count * percols * k + i * percols + 3];
                    lines[count * percols * k + i * percols + 3] = templine;

                    templine = lines[count * percols * k + j * percols + 4];
                    lines[count * percols * k + j * percols + 4] = lines[count * percols * k + i * percols + 4];
                    lines[count * percols * k + i * percols + 4] = templine;
                }
            }
        }    

        std::time_t t = std::time(0);   // get time now
        errno_t err;
        __time64_t long_time;
        struct tm newtime;
        //std::tm* now = std::localtime_s(&t);
        _time64(&long_time);
        err = _localtime64_s(&newtime, &long_time);
        --newtime.tm_mday; // Move back one day
        mktime(&newtime); // Normalize

        char path[512];
        sprintf_s(path, "c:\\prenos\\NeuralFin\\log\\%04d-%02d-%02d.txt", newtime.tm_year+1900, newtime.tm_mon+1,newtime.tm_mday+1);
        ofstream myfile;
        myfile.open(path, std::ios_base::trunc);

        for (int i = 0; i < lines.size() / (typecount* percols); i++)
        {
            
            for (int j = 0; j < typecount; j++)
            for (int k = 0; k < percols; k++)
            {
                cout << lines[i*percols + j * (lines.size()/ typecount) + k];
                myfile << lines[i * percols + j * (lines.size() / typecount) + k];
                if (k == 0){
                    cout << " | ";
                    myfile << " | ";
                }
                if (k == 1){
                    cout << " | ";//date
                    myfile << " | ";//date
                }
                if (k == 2){
                    cout << " | ";//PR
                    myfile << " | ";//PR
                }
                if (k == 3){
                    cout << " | ";//DF
                    myfile << " | ";//DF
                }
                if (k == 4) {//percent
                    cout << " | " << lines[i * percols + j * (lines.size() / typecount) + k-4] << endl;
                    myfile << " | " << lines[i * percols + j * (lines.size() / typecount) + k - 4] << endl;
                }                
            }
            cout << endl;
            myfile << endl;
        }

        myfile.close();
};

void makebatches(int argc, char* argv[]) {
    char path[500];
    int count = stoi(argv[2]);

    std::ifstream  data("c:\\prenos\\NeuralFin\\data\\eodapikey.txt");
    std::string apikey;
    std::getline(data, apikey);
    data.close();

    ofstream myfile;

    sprintf_s(path, "c:\\prenos\\NeuralFin\\scripts\\multifindbest.bat");
    myfile.open(path);
    myfile << "REM xx" << endl;
    for (int cc = 0; cc < count; cc++)
    {
        myfile << "del \"c:\\prenos\\NeuralFin\\data\\" << cc << "\\addlog.csv\"" << endl;
        for (int i = 3; i < argc; i++)
        {
            myfile << "c:\\prenos\\NeuralFin\\x64\\Release\\neurnet.exe no " << cc << " " << argv[i];
            for (int j = 3; j < argc; j++)
                if (strcmp(argv[i], argv[j]))
                    myfile << " " << argv[j];
            myfile << endl;
        }
    }
    myfile << "c:\\prenos\\NeuralFin\\x64\\Release\\neurnet.exe multisort " << count << endl;
    myfile << "PAUSE" << endl;
    myfile.close();
    
    myfile.open("c:\\prenos\\NeuralFin\\scripts\\dwn.bat");
    for (int i = 3; i < argc; i++)
    {
        myfile << "del \"c:\\prenos\\NeuralFin\\data\\" << argv[i] << ".US@api_token=" << apikey << "\"" << endl;
        myfile << "c:\\prenos\\NeuralFin\\wget\\bin\\wget -P c:\\prenos\\NeuralFin\\data \"https://eodhistoricaldata.com/api/eod/" << argv[i] << ".US?api_token=" << apikey << "\"" << endl;
        myfile << "copy \"c:\\prenos\\NeuralFin\\data\\" << argv[i] << ".US@api_token=" << apikey << "\" \"c:\\prenos\\NeuralFin\\data\\" << argv[i] << ".csv\"" << endl << endl;
    }
    myfile.close();

    for (int cc = 0; cc < count; cc++)
    {
        sprintf_s(path, "c:\\prenos\\NeuralFin\\scripts\\%d\\findbest.bat", cc);
        myfile.open(path);
        myfile << "REM xx" << endl;
        myfile << "del \"c:\\prenos\\NeuralFin\\data\\"<< cc <<"\\addlog.csv\"" << endl;
        for (int i = 3; i < argc; i++)
        {
            myfile << "c:\\prenos\\NeuralFin\\x64\\Release\\neurnet.exe no " << cc << " " << argv[i];
            for (int j = 3; j < argc; j++)
                if (strcmp(argv[i], argv[j]))
                    myfile << " " << argv[j];
            myfile << endl;
        }
        myfile << "c:\\prenos\\NeuralFin\\x64\\Release\\neurnet.exe sort " << cc << endl;
        myfile << "PAUSE" << endl;
        myfile.close();

        sprintf_s(path, "c:\\prenos\\NeuralFin\\scripts\\%d\\run-all.bat", cc);
        myfile.open(path);
        myfile << "REM xx" << endl;
        for (int i = 3; i < argc; i++)
        {
            myfile << "c:\\prenos\\NeuralFin\\x64\\Release\\neurnet.exe yes " << cc << " " << argv[i];
            for (int j = 3; j < argc; j++)
                if (strcmp(argv[i], argv[j]))
                    myfile << " " << argv[j];
            myfile << endl;
        }
        myfile << "PAUSE" << endl;
        myfile.close();

        for (int i = 3; i < argc; i++)
        {
            sprintf_s(path, "c:\\prenos\\NeuralFin\\scripts\\%d\\run-all-part%d.bat", cc,0 + ((i - 3) / ((argc - 3) / 4)));
            if ((i - 3) % ((argc - 3) / 4) == 0)
            {
                myfile.open(path);
                myfile << "REM xx" << endl;
                myfile << "cd c:\\prenos\\NeuralFin" << endl;
            }
            myfile << "c:\\prenos\\NeuralFin\\x64\\Release\\neurnet.exe yes " << cc << " " << argv[i];
            for (int j = 3; j < argc; j++)
                if (strcmp(argv[i], argv[j]))
                    myfile << " " << argv[j];
            myfile << endl;
            if ((i - 3) % ((argc - 3) / 4) == ((argc - 3) / 4) - 1)
            {
                myfile << "PAUSE" << endl;
                myfile.close();
            }
        }

        sprintf_s(path, "c:\\prenos\\NeuralFin\\scripts\\%d\\run-score.bat", cc);
        myfile.open(path);
        myfile << "REM xx" << endl;
        myfile << "cd c:\\prenos\\NeuralFin" << endl;
        for (int i = 3; i < argc; i++)
        {
            myfile << "c:\\prenos\\NeuralFin\\x64\\Release\\neurnet.exe score " << cc << " " << argv[i];
            for (int j = 3; j < argc; j++)
                if (strcmp(argv[i], argv[j]))
                    myfile << " " << argv[j];
            myfile << endl;
        }
        myfile << "PAUSE" << endl;
        myfile.close();

        for (int i = 3; i < argc; i++)
        {
            sprintf_s(path, "c:\\prenos\\NeuralFin\\scripts\\%d\\run-only%d.bat", cc,i - 2);
            myfile.open(path);
            myfile << "REM xx" << endl;
            myfile << "cd c:\\prenos\\NeuralFin" << endl;
            myfile << "c:\\prenos\\NeuralFin\\x64\\Release\\neurnet.exe yes " << cc << " " << argv[i];
            for (int j = 3; j < argc; j++)
                if (strcmp(argv[i], argv[j]))
                    myfile << " " << argv[j];
            myfile << endl;
            myfile << "PAUSE" << endl;
            myfile.close();
        }

        sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%d\\images.html", cc);
        myfile.open(path);
        myfile << "<body>" << endl;
        myfile << "<table>" << endl;
        for (int i = 3; i < argc; i++)
        {
            myfile << "<tr>" << endl;
            myfile << "<td>" << endl;
            myfile << argv[i] << endl;
            myfile << "</td>" << endl;
            myfile << "<td align = \"right\">" << endl;
            myfile << argv[i] << endl;
            myfile << "</td>" << endl;
            myfile << "</tr>" << endl;
            myfile << "<tr>" << endl;
            myfile << "<td colspan = \"2\">" << endl;
            myfile << "<img src = \""<<argv[i]<<"-screen.png\">" << endl;
            myfile << "</td>" << endl;
            myfile << "</tr>" << endl;
        }
        myfile << "</table>" << endl;
        myfile << "</body>" << endl;
        myfile.close();
    }
};

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) == 0) {
        SDL_Window* window = NULL;
        SDL_Renderer* renderer = NULL;

        if (SDL_CreateWindowAndRenderer(rendx, rendy, 0, &window, &renderer) == 0) {
            //SDL_bool done = SDL_FALSE;
            if ((argc > 1) && (!strcmp("yes", argv[1])))
                optimize(renderer, argc, argv);
            else if ((argc > 1) && (!strcmp("score", argv[1])))
                printscore(renderer, argc, argv);
            else if ((argc > 1) && (!strcmp("sort", argv[1])))
                sortbest(argc, argv);
            else if ((argc > 1) && (!strcmp("multisort", argv[1])))
                multisortbest(argc, argv);
            else if ((argc > 1) && (!strcmp("makebatches", argv[1])))
                makebatches(argc, argv);
            else
                computenextday(renderer, argc, argv);
            
            /*while (!done) {
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
            }*/
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



