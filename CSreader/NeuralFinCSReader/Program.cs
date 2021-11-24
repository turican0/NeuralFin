using System;

namespace NeuralFinCSReader
{
    class Program
    {
        std::vector<data_t> datavect;
        static void savetobestlog(string buffer, string filename, int minsize)
        {
            /*string buffer2 = "";
            for (int i = 0; i < 256; i++) buffer2[i] = ' ';
            for (int i = 0; i < strlen(buffer); i++) buffer2[i] = buffer[i];
            buffer2[minsize] = 0;
            ofstream myfile;
            myfile.open(filename, std::ios_base::app);
            myfile << buffer2 << endl;
            myfile.close();*/
        }

        static void parseCSV(string file)
        {
            std::ifstream data(file);
            std::string line;
            bool first = true;
            int x = 0;
            while ((std::getline(data, line))/*&&(x<15)*/)
            {
                data_t data;
                std::stringstream lineStream(line);
                if (first) first = false;
                else
                {
                    lineStream >> data;
                    datavect.push_back(data);
                }
                x++;
                if (x > rowtrunc) break;
            }
            datavect.pop_back();
        }

        static void computenextday(string[] args)
        {
            Console.WriteLine("--- " + args[4] + " - " + args[1] + " ---";
            int plusday = int.Parse(args[3]);
            //char buffer[512];
            //sprintf_s(buffer, "%s", args[4]);
            string buffer = args[4];
            //char bestlogpath[512];
            //sprintf_s(bestlogpath, "c:\\prenos\\NeuralFin\\data\\%d\\addlog.csv", plusday);
            string bestlogpath ="c:\\prenos\\NeuralFin\\data\\{plusday}\\addlog.csv");
            //char path[512];
            savetobestlog(buffer, bestlogpath, 4);
            string path = "c:\\prenos\\NeuralFin\\data\\{args[4]}addlog.csv";
            //sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%s.csv", args[4]);
            parseCSV(path);
            Console.WriteLine("date: " + datavect[datavect.size() - 1].ear + "-" + datavect[datavect.size() - 1].moon + "-" + datavect[datavect.size() - 1].day";

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
        if (!strcmp(argv[3], "nodetect"))
        {
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
    if (newtime.tm_mday != datavect[datavect.size() - 1].day)
    {
        cout << endl << "!!! incorect day !!!" << endl;
        //cin.ignore(1);
        std::cin.get();
        exit(0);
    }
        }
        else
    {
        newtime.tm_year = datavect[datavect.size() - 1].ear - 1900;
        newtime.tm_mon = datavect[datavect.size() - 1].moon - 1;
        newtime.tm_mday = datavect[datavect.size() - 1].day;
    }
    /*std::cout << (now->tm_year + 1900) << '-'
        << (now->tm_mon + 1) << '-'
        << now->tm_mday
        << "\n";*/
    //test date

    sprintf_s(buffer, "date:%d-%d-%d", (int)datavect[datavect.size() - 1].ear, (int)datavect[datavect.size() - 1].moon, (int)datavect[datavect.size() - 1].day);
    savetobestlog(buffer, bestlogpath, 14);
    for (int oi = 0; oi < argc - 5; oi++)
    {
        sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%s.csv", argv[5 + oi]);
        parseCSVother(path);
    }

    findKoef();

    cols = 0;
    if (countofder > 0) cols += inputsize;
    if (countofder > 1) cols += inputsize - 1;
    if (countofder > 2) cols += inputsize - 2;

    //cols = (inputsize + (inputsize - 1) + (inputsize - 2));
    //int cols2 = (inputsize + (inputsize - 1) + (inputsize - 2)) * countoff;
    rows = datavect.size() - inputsize + 1 /*- outputsize*/ - plusday;
    vector<double> input(cols* rows * countoff* (1 + countother));
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
                if (countoff > 0) ipusch(0, &input, i, j, 0, datavect[i + j].close);
                if (countoff > 1) ipusch(0, &input, i, j, 1, datavect[i + j].close * datavect[i + j].close);
                if (countoff > 2) ipusch(0, &input, i, j, 2, sqrt(abs(datavect[i + j].close)));
                if (countoff > 3) ipusch(0, &input, i, j, 3, log(1 + abs(datavect[i + j].close)));
                if (countoff > 4) ipusch(0, &input, i, j, 4, datavect[i + j].high);
                if (countoff > 5) ipusch(0, &input, i, j, 5, datavect[i + j].low);
            }
        if (countofder > 1)
            for (int j = 0; j < inputsize - 1; j++)
            {
                double der1 = datavect[i + j + 1].close - datavect[i + j].close;
                if (countoff > 0) ipusch(0, &input, i, j + inputsize, 0, der1);
                if (countoff > 1) ipusch(0, &input, i, j + inputsize, 1, der1 * der1);
                if (countoff > 2) ipusch(0, &input, i, j + inputsize, 2, sqrt(abs(der1)));
                if (countoff > 3) ipusch(0, &input, i, j + inputsize, 3, log(1 + abs(der1)));
                der1 = datavect[i + j + 1].high - datavect[i + j].high;
                if (countoff > 4) ipusch(0, &input, i, j + inputsize, 4, der1);
                der1 = datavect[i + j + 1].low - datavect[i + j].low;
                if (countoff > 5) ipusch(0, &input, i, j + inputsize, 5, der1);
            }
        if (countofder > 2)
            for (int j = 0; j < inputsize - 2; j++)
            {
                double der3 = datavect[i + j + 2].close - 2 * datavect[i + j + 1].close + datavect[i + j].close;
                if (countoff > 0) ipusch(0, &input, i, j + inputsize * 2 - 1, 0, der3);
                if (countoff > 1) ipusch(0, &input, i, j + inputsize * 2 - 1, 1, der3 * der3);
                if (countoff > 2) ipusch(0, &input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
                if (countoff > 3) ipusch(0, &input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
                der3 = datavect[i + j + 2].high - 2 * datavect[i + j + 1].high + datavect[i + j].high;
                if (countoff > 4) ipusch(0, &input, i, j + inputsize * 2 - 1, 4, der3);
                der3 = datavect[i + j + 2].low - 2 * datavect[i + j + 1].high + datavect[i + j].low;
                if (countoff > 5) ipusch(0, &input, i, j + inputsize * 2 - 1, 5, der3);
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
                    if (countoff > 0) ipusch(oo + 1, &input, i, j, 0, dataother[oo][i + j].close);
                    if (countoff > 1) ipusch(oo + 1, &input, i, j, 1, dataother[oo][i + j].close * dataother[oo][i + j].close);
                    if (countoff > 2) ipusch(oo + 1, &input, i, j, 2, sqrt(abs(dataother[oo][i + j].close)));
                    if (countoff > 3) ipusch(oo + 1, &input, i, j, 3, log(1 + abs(dataother[oo][i + j].close)));
                    if (countoff > 4) ipusch(oo + 1, &input, i, j, 4, dataother[oo][i + j].high);
                    if (countoff > 5) ipusch(oo + 1, &input, i, j, 5, dataother[oo][i + j].low);
                }
            if (countofder > 1)
                for (int j = 0; j < inputsize - 1; j++)
                {
                    double der1 = dataother[oo][i + j + 1].close - dataother[oo][i + j].close;
                    if (countoff > 0) ipusch(oo + 1, &input, i, j + inputsize, 0, der1);
                    if (countoff > 1) ipusch(oo + 1, &input, i, j + inputsize, 1, der1 * der1);
                    if (countoff > 2) ipusch(oo + 1, &input, i, j + inputsize, 2, sqrt(abs(der1)));
                    if (countoff > 3) ipusch(oo + 1, &input, i, j + inputsize, 3, log(1 + abs(der1)));
                    der1 = dataother[oo][i + j + 1].high - dataother[oo][i + j].high;
                    if (countoff > 4) ipusch(oo + 1, &input, i, j + inputsize, 4, der1);
                    der1 = dataother[oo][i + j + 1].low - dataother[oo][i + j].low;
                    if (countoff > 5) ipusch(oo + 1, &input, i, j + inputsize, 5, der1);
                }
            if (countofder > 2)
                for (int j = 0; j < inputsize - 2; j++)
                {
                    double der3 = dataother[oo][i + j + 2].close - 2 * dataother[oo][i + j + 1].close + dataother[oo][i + j].close;
                    if (countoff > 0) ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 0, der3);
                    if (countoff > 1) ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 1, der3 * der3);
                    if (countoff > 2) ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 2, sqrt(abs(der3)));
                    if (countoff > 3) ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 3, log(1 + abs(der3)));
                    der3 = dataother[oo][i + j + 2].high - 2 * dataother[oo][i + j + 1].high + dataother[oo][i + j].high;
                    if (countoff > 4) ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 4, der3);
                    der3 = dataother[oo][i + j + 2].low - 2 * dataother[oo][i + j + 1].low + dataother[oo][i + j].low;
                    if (countoff > 5) ipusch(oo + 1, &input, i, j + inputsize * 2 - 1, 5, der3);
                }
        }
    //init2

    if (countoff > 0) cleanweights(0, 0, &weight);
    if (countoff > 1) cleanweights(0, 1, &weight);
    if (countoff > 2) cleanweights(0, 2, &weight);
    if (countoff > 3) cleanweights(0, 3, &weight);
    if (countoff > 4) cleanweights(0, 4, &weight);
    if (countoff > 5) cleanweights(0, 5, &weight);

    for (int oo = 0; oo < countother; oo++)
    {
        if (countoff > 0) cleanweights(oo + 1, 0, &weight);
        if (countoff > 1) cleanweights(oo + 1, 1, &weight);
        if (countoff > 2) cleanweights(oo + 1, 2, &weight);
        if (countoff > 3) cleanweights(oo + 1, 3, &weight);
        if (countoff > 4) cleanweights(oo + 1, 4, &weight);
        if (countoff > 5) cleanweights(oo + 1, 5, &weight);
        //ipusch(&input, i, j, 0, dataother[oo][i + j].close);
    }

    //char path[512];
    sprintf_s(path, "c:\\prenos\\NeuralFin\\data\\%d\\%s-weight.csv", plusday, argv[4]);
    loaddata(&weight, path);
    compoutputs(&input, &output, &weight);

    drawgraph(renderer, &output, 0, &weight, argv[4], plusday);
    //compnextday(&input, &output, &weight);
    cout << datavect[datavect.size() - 1].close << " -> " << (output)[rows - 1] << " $" << endl;
    cout << (output)[rows - 1] - datavect[datavect.size() - 1].close << " $" << endl;
    cout << -(1 - (output)[rows - 1] / datavect[datavect.size() - 1].close) * 100 << " %" << endl;
    cout << "PROFIT: " << profitx << "%" << endl;

    sprintf_s(buffer, "PR:%0.1f", profitx / (plusday + 1));
    savetobestlog(buffer, bestlogpath, 9);

    //if(plusday==0)
    sprintf_s(buffer, "DF:%0.1f$->%0.1f$(%0.1f$)/(%0.1f$-%0.1f$)", datavect[datavect.size() - 1].close, (output)[rows - 1], (output)[rows - 1] - datavect[datavect.size() - 1].close, datavect[datavect.size() - 1].low, datavect[datavect.size() - 1].high);
    /*else
        sprintf_s(buffer, "DF:%0.1f$->%0.1f$(%0.1f$)", datavect[datavect.size() - 1].close, (output)[rows - 1], (output)[rows - 1] - datavect[datavect.size() - 1].close);    */

    savetobestlog(buffer, bestlogpath, 45);
    sprintf_s(buffer, "%0.1f%s", -(1 - (output)[rows - 1] / datavect[datavect.size() - 1].close) * 100, "%");
    savetobestlog(buffer, bestlogpath, 6);
    };

static void Main(string[] args)
        {
            Console.WriteLine("Hello World!");
        }
    }
}
