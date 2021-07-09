#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>
#include <iomanip>      // std::setprecision

using namespace std;

class TrainingData
{
public:
	TrainingData(const string filename);
	void setBegin(const string filename)
	{
		m_trainingDataFile.close();
		m_trainingDataFile.open(filename.c_str());
		string line;
		getline(m_trainingDataFile, line);
	}
	bool isEof(void)
	{
		return m_trainingDataFile.eof();
	}
	void getTopology(vector<unsigned>& topology);

	// Returns the number of input values read from the file:
	unsigned getNextInputs(vector<double>& inputVals);
	unsigned getTargetOutputs(vector<double>& targetOutputVals);

private:
	ifstream m_trainingDataFile;
};

void TrainingData::getTopology(vector<unsigned>& topology)
{
	string line;
	string label;

	getline(m_trainingDataFile, line);
	stringstream ss(line);
	ss >> label;
	if (this->isEof() || label.compare("topology:") != 0)
	{
		abort();
	}

	while (!ss.eof())
	{
		unsigned n;
		ss >> n;
		topology.push_back(n);
	}
	return;
}

TrainingData::TrainingData(const string filename)
{
	m_trainingDataFile.open(filename.c_str());
}


unsigned TrainingData::getNextInputs(vector<double>& inputVals)
{
	inputVals.clear();

	string line;
	getline(m_trainingDataFile, line);
	stringstream ss(line);

	string label;
	ss >> label;
	if (label.compare("in:") == 0) {
		double oneValue;
		while (ss >> oneValue) {
			inputVals.push_back(oneValue);
		}
	}

	return inputVals.size();
}

unsigned TrainingData::getTargetOutputs(vector<double>& targetOutputVals)
{
	targetOutputVals.clear();

	string line;
	getline(m_trainingDataFile, line);
	stringstream ss(line);

	string label;
	ss >> label;
	if (label.compare("out:") == 0) {
		double oneValue;
		while (ss >> oneValue) {
			targetOutputVals.push_back(oneValue);
		}
	}

	return targetOutputVals.size();
}

struct Connection
{
	double weight;
	double deltaWeight;
};

class Neuron;

typedef vector<Neuron> Layer;

// ****************** class Neuron ******************

class Neuron
{
public:
	Neuron(unsigned numOutputs, unsigned myIndex);
	void setOutputVal(double val) { m_outputVal = val; }
	double getOutputVal(void) const { return m_outputVal; }
	void feedForward(const Layer& prevLayer);
	void calcOutputGradients(double targetVals);
	void calcHiddenGradients(const Layer& nextLayer);
	void updateInputWeights(Layer& prevLayer);
private:
	static double eta; // [0.0...1.0] overall net training rate
	static double alpha; // [0.0...n] multiplier of last weight change [momentum]
	static double transferFunction(double x);
	static double transferFunctionDerivative(double x);
	// randomWeight: 0 - 1
	static double randomWeight(void) { return rand() / double(RAND_MAX); }
	double sumDOW(const Layer& nextLayer) const;
	double m_outputVal;
	vector<Connection> m_outputWeights;
	unsigned m_myIndex;
	double m_gradient;
};

double Neuron::eta = 0.15; // overall net learning rate
double Neuron::alpha = 0.5; // momentum, multiplier of last deltaWeight, [0.0..n]


void Neuron::updateInputWeights(Layer& prevLayer)
{
	// The weights to be updated are in the Connection container
	// in the nuerons in the preceding layer

	for (unsigned n = 0; n < prevLayer.size(); ++n)
	{
		Neuron& neuron = prevLayer[n];
		double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

		double newDeltaWeight =
			// Individual input, magnified by the gradient and train rate:
			eta
			* neuron.getOutputVal()
			* m_gradient
			// Also add momentum = a fraction of the previous delta weight
			+ alpha
			* oldDeltaWeight;
		neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
		neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
	}
}
double Neuron::sumDOW(const Layer& nextLayer) const
{
	double sum = 0.0;

	// Sum our contributions of the errors at the nodes we feed

	for (unsigned n = 0; n < nextLayer.size() - 1; ++n)
	{
		sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
	}

	return sum;
}

void Neuron::calcHiddenGradients(const Layer& nextLayer)
{
	double dow = sumDOW(nextLayer);
	m_gradient = dow * Neuron::transferFunctionDerivative(m_outputVal);
}
void Neuron::calcOutputGradients(double targetVals)
{
	double delta = targetVals - m_outputVal;
	m_gradient = delta * Neuron::transferFunctionDerivative(m_outputVal);
}

double Neuron::transferFunction(double x)
{
	// tanh - output range [-1.0..1.0]
	return tanh(x);
}

double Neuron::transferFunctionDerivative(double x)
{
	// tanh derivative
	return 1.0 - x * x;
}

void Neuron::feedForward(const Layer& prevLayer)
{
	double sum = 0.0;

	// Sum the previous layer's outputs (which are our inputs)
	// Include the bias node from the previous layer.

	for (unsigned n = 0; n < prevLayer.size(); ++n)
	{
		sum += prevLayer[n].getOutputVal() *
			prevLayer[n].m_outputWeights[m_myIndex].weight;
	}
	sum /= prevLayer.size();

	m_outputVal = Neuron::transferFunction(sum);
}

Neuron::Neuron(unsigned numOutputs, unsigned myIndex)
{
	for (unsigned c = 0; c < numOutputs; ++c) {
		m_outputWeights.push_back(Connection());
		m_outputWeights.back().weight = randomWeight();
	}

	m_myIndex = myIndex;
}
// ****************** class Net ******************
class Net
{
public:
	Net(const vector<unsigned>& topology);
	void feedForward(const vector<double>& inputVals);
	void backProp(const vector<double>& targetVals);
	void getResults(vector<double>& resultVals) const;
	double getRecentAverageError(void) const { return m_recentAverageError; }

private:
	vector<Layer> m_layers; //m_layers[layerNum][neuronNum]
	double m_error;
	double m_recentAverageError;
	static double m_recentAverageSmoothingFactor;
};

double Net::m_recentAverageSmoothingFactor = 100.0; // Number of training samples to average over

void Net::getResults(vector<double>& resultVals) const
{
	resultVals.clear();

	for (unsigned n = 0; n < m_layers.back().size() - 1; ++n)
	{
		resultVals.push_back(m_layers.back()[n].getOutputVal());
	}
}

void Net::backProp(const std::vector<double>& targetVals)
{
	// Calculate overal net error (RMS of output neuron errors)

	Layer& outputLayer = m_layers.back();
	m_error = 0.0;

	for (unsigned n = 0; n < outputLayer.size() - 1; ++n)
	{
		double delta = targetVals[n] - outputLayer[n].getOutputVal();
		m_error += delta * delta;
	}
	m_error /= outputLayer.size() - 1; // get average error squared
	m_error = sqrt(m_error); // RMS

	// Implement a recent average measurement:

	m_recentAverageError =
		(m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
		/ (m_recentAverageSmoothingFactor + 1.0);
	// Calculate output layer gradients

	for (unsigned n = 0; n < outputLayer.size() - 1; ++n)
	{
		outputLayer[n].calcOutputGradients(targetVals[n]);
	}
	// Calculate gradients on hidden layers

	for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; --layerNum)
	{
		Layer& hiddenLayer = m_layers[layerNum];
		Layer& nextLayer = m_layers[layerNum + 1];

		for (unsigned n = 0; n < hiddenLayer.size(); ++n)
		{
			hiddenLayer[n].calcHiddenGradients(nextLayer);
		}
	}

	// For all layers from outputs to first hidden layer,
	// update connection weights

	for (unsigned layerNum = m_layers.size() - 1; layerNum > 0; --layerNum)
	{
		Layer& layer = m_layers[layerNum];
		Layer& prevLayer = m_layers[layerNum - 1];

		for (unsigned n = 0; n < layer.size() - 1; ++n)
		{
			layer[n].updateInputWeights(prevLayer);
		}
	}
}

void Net::feedForward(const vector<double>& inputVals)
{
	// Check the num of inputVals euqal to neuronnum expect bias
	assert(inputVals.size() == m_layers[0].size() - 1);

	// Assign {latch} the input values into the input neurons
	for (unsigned i = 0; i < inputVals.size(); ++i) {
		m_layers[0][i].setOutputVal(inputVals[i]);
	}

	// Forward propagate
	for (unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum) {
		Layer& prevLayer = m_layers[layerNum - 1];
		for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n) {
			m_layers[layerNum][n].feedForward(prevLayer);
		}
	}
}
Net::Net(const vector<unsigned>& topology)
{
	unsigned numLayers = topology.size();
	for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum) {
		m_layers.push_back(Layer());
		// numOutputs of layer[i] is the numInputs of layer[i+1]
		// numOutputs of last layer is 0
		unsigned numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];

		// We have made a new Layer, now fill it ith neurons, and
		// add a bias neuron to the layer:
		for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum) {
			m_layers.back().push_back(Neuron(numOutputs, neuronNum));
			cout << "Mad a Neuron!" << endl;
		}

		// Force the bias node's output value to 1.0. It's the last neuron created above
		m_layers.back().back().setOutputVal(1.0);
	}
}

void showVectorVals(string label, vector<double>& v)
{
	cout << label << " ";
	for (unsigned i = 0; i < v.size(); ++i)
	{
		cout << /*setprecision(15) << fixed <<*/ v[i] << " ";
	}
	cout << endl;
}

void generate()
{
	// Random training sets for XOR -- two inputs and one output

	cout << "topology: 2 4 1" << endl;
	for (int i = 2000; i >= 0; --i)
	{
		int n1 = (int)(2.0 * rand() / double(RAND_MAX));
		int n2 = (int)(2.0 * rand() / double(RAND_MAX));
		int t = n1 ^ n2; // should be 0 or 1
		cout << "in: " << n1 << ".0 " << n2 << ".0 " << endl;
		cout << "out: " << t << ".0" << endl;
	}
}

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

void makeTrainingData() {
	std::ofstream ofs("c:\\prenos\\NeuralFin\\trainingData2.txt", std::ofstream::out);
	int incount = 30;
	int outcount = 5;
	int coef = 1000;
	ofs << "topology: "<< incount << " " << incount+ outcount+1 << " " << incount + outcount + 1 << " " << incount + outcount + 1 << " " << outcount << endl;
	/*
topology: 2 4 1
in : 1.0 0.0
out : 1.0
*/
	int lend = datavect.size();
	for (int i = 0; i < lend - incount - outcount+1; i++)
	{
		ofs << "in: ";
		for (int j = 0; j < incount; j++)
			ofs << datavect[i+j].open/ coef << " ";
		ofs << endl;
		ofs << "out: ";
		for (int j = 0; j < outcount; j++)
			ofs << datavect[i + j+ incount].open/ coef << " ";
		ofs << endl;
	}

	ofs.close();
};

#include <tensorflow/c/c_api.h> // TensorFlow C API header.
#include <scope_guard.hpp>
#include <cstring>
#include <array>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

/*int main() {
	printf("Hello from TensorFlow C library version %s\n", TF_Version());
	return 0;
}*/
/*
int main() {
	const std::vector<std::int64_t> dims = { 1, 5, 12 };
	const auto data_size = std::accumulate(dims.begin(), dims.end(), sizeof(float), std::multiplies<std::int64_t>{});

	const std::vector<float> data = {
	  -0.4809832f, -0.3770838f, 0.1743573f, 0.7720509f, -0.4064746f, 0.0116595f, 0.0051413f, 0.9135732f, 0.7197526f, -0.0400658f, 0.1180671f, -0.6829428f,
	  -0.4810135f, -0.3772099f, 0.1745346f, 0.7719303f, -0.4066443f, 0.0114614f, 0.0051195f, 0.9135003f, 0.7196983f, -0.0400035f, 0.1178188f, -0.6830465f,
	  -0.4809143f, -0.3773398f, 0.1746384f, 0.7719052f, -0.4067171f, 0.0111654f, 0.0054433f, 0.9134697f, 0.7192584f, -0.0399981f, 0.1177435f, -0.6835230f,
	  -0.4808300f, -0.3774327f, 0.1748246f, 0.7718700f, -0.4070232f, 0.0109549f, 0.0059128f, 0.9133330f, 0.7188759f, -0.0398740f, 0.1181437f, -0.6838635f,
	  -0.4807833f, -0.3775733f, 0.1748378f, 0.7718275f, -0.4073670f, 0.0107582f, 0.0062978f, 0.9131795f, 0.7187147f, -0.0394935f, 0.1184392f, -0.6840039f,
	};

	auto tensor = TF_AllocateTensor(TF_FLOAT, dims.data(), static_cast<int>(dims.size()), data_size);
	SCOPE_EXIT{ TF_DeleteTensor(tensor); }; // Auto-delete on scope exit.

	if (tensor != nullptr && TF_TensorData(tensor) != nullptr) {
		std::memcpy(TF_TensorData(tensor), data.data(), std::min(data_size, TF_TensorByteSize(tensor)));
	}
	else {
		std::cout << "Wrong creat tensor" << std::endl;
		return 1;
	}

	if (TF_TensorType(tensor) != TF_FLOAT) {
		std::cout << "Wrong tensor type" << std::endl;
		return 2;
	}

	if (TF_NumDims(tensor) != static_cast<int>(dims.size())) {
		std::cout << "Wrong number of dimensions" << std::endl;
		return 3;
	}

	for (std::size_t i = 0; i < dims.size(); ++i) {
		if (TF_Dim(tensor, static_cast<int>(i)) != dims[i]) {
			std::cout << "Wrong dimension size for dim: " << i << std::endl;
			return 4;
		}
	}

	if (TF_TensorByteSize(tensor) != data_size) {
		std::cout << "Wrong tensor byte size" << std::endl;
		return 5;
	}

	auto tensor_data = static_cast<float*>(TF_TensorData(tensor));

	if (tensor_data == nullptr) {
		std::cout << "Wrong data tensor" << std::endl;
		return 6;
	}

	for (std::size_t i = 0; i < data.size(); ++i) {
		if (tensor_data[i] != data[i]) {
			std::cout << "Element: " << i << " does not match" << std::endl;
			return 7;
		}
	}

	std::cout << "Success allocate tensor" << std::endl;

	return 0;
}*/



/*
int main()
{
	parseCSV((char*)"c:\\prenos\\NeuralFin\\tsla.csv");
	makeTrainingData();
	//exit(0);
	TrainingData trainData("trainingData2.txt");
	//e.g., {3, 2, 1 }
	vector<unsigned> topology;
	//topology.push_back(3);
	//topology.push_back(2);
	//topology.push_back(1);

	trainData.getTopology(topology);
	Net myNet(topology);

	vector<double> inputVals, targetVals, resultVals;
	int trainingPass = 0;

	double error = 0;
	
	for (int ii = 0; ii < 1000; ii++)
	{
		trainData.setBegin("trainingData2.txt");
		while (!trainData.isEof())
		{
			++trainingPass;
			//cout << endl << "Pass" << trainingPass;

			// Get new input data and feed it forward:
			if (trainData.getNextInputs(inputVals) != topology[0])
				break;
			//showVectorVals(": Inputs :", inputVals);
			myNet.feedForward(inputVals);

			// Collect the net's actual results:
			myNet.getResults(resultVals);
			//showVectorVals("Outputs:", resultVals);

			// Train the net what the outputs should have been:
			trainData.getTargetOutputs(targetVals);
			//showVectorVals("Targets:", targetVals);
			assert(targetVals.size() == topology.back());

			myNet.backProp(targetVals);

			// Report how well the training is working, average over recnet
			//cout << "Net recent average error: " << myNet.getRecentAverageError() << endl;
			error += myNet.getRecentAverageError();
		}
		cout << "Net recent average error: " << error << endl;
		error = 0;
	}

	cout << endl << "Done" << endl;

}*/

//https://www.geeksforgeeks.org/ml-neural-network-implementation-in-c-from-scratch/

// NeuralNetwork.hpp
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <vector>

// use typedefs for future ease for changing data types like : float to double
typedef float Scalar;
typedef Eigen::MatrixXf Matrix;
typedef Eigen::RowVectorXf RowVector;
typedef Eigen::VectorXf ColVector;

// neural network implementation class!
class NeuralNetwork {
public:
	// constructor
	NeuralNetwork(std::vector<int> topology, Scalar learningRate = Scalar(0.005));

	// function for forward propagation of data
	void propagateForward(RowVector& input);

	// function for backward propagation of errors made by neurons
	void propagateBackward(RowVector& output);

	// function to calculate errors made by neurons in each layer
	void calcErrors(RowVector& output);

	// function to update the weights of connections
	void updateWeights();

	// function to train the neural network give an array of data points
	void train(std::vector<RowVector*> data);

	// storage objects for working of neural network
	/*
		use pointers when using std::vector<Class> as std::vector<Class> calls destructor of
		Class as soon as it is pushed back! when we use pointers it can't do that, besides
		it also makes our neural network class less heavy!! It would be nice if you can use
		smart pointers instead of usual ones like this
		*/
	std::vector<RowVector*> neuronLayers; // stores the different layers of out network
	std::vector<RowVector*> cacheLayers; // stores the unactivated (activation fn not yet applied) values of layers
	std::vector<RowVector*> deltas; // stores the error contribution of each neurons
	std::vector<Matrix*> weights; // the connection weights itself
	Scalar learningRate;

	std::vector<int> topology;
};

// constructor of neural network class
NeuralNetwork::NeuralNetwork(std::vector<int> topology, Scalar learningRate)
{
	this->topology = topology;
	this->learningRate = learningRate;
	for (int i = 0; i < topology.size(); i++) {
		// initialize neuron layers
		if (i == topology.size() - 1)
			neuronLayers.push_back(new RowVector(topology[i]));
		else
			neuronLayers.push_back(new RowVector(topology[i] + 1));

		// initialize cache and delta vectors
		cacheLayers.push_back(new RowVector(neuronLayers.size()));
		deltas.push_back(new RowVector(neuronLayers.size()));

		// vector.back() gives the handle to recently added element
		// coeffRef gives the reference of value at that place
		// (using this as we are using pointers here)
		if (i != topology.size() - 1) {
			neuronLayers.back()->coeffRef(topology[i]) = 1.0;
			cacheLayers.back()->coeffRef(topology[i]) = 1.0;
		}

		// initialize weights matrix
		if (i > 0) {
			if (i != topology.size() - 1) {
				weights.push_back(new Matrix(topology[i - 1] + 1, topology[i] + 1));
				weights.back()->setRandom();
				weights.back()->col(topology[i]).setZero();
				weights.back()->coeffRef(topology[i - 1], topology[i]) = 1.0;
			}
			else {
				weights.push_back(new Matrix(topology[i - 1] + 1, topology[i]));
				weights.back()->setRandom();
			}
		}
	}
};

void NeuralNetwork::propagateForward(RowVector& input)
{
	// set the input to input layer
	// block returns a part of the given vector or matrix
	// block takes 4 arguments : startRow, startCol, blockRows, blockCols
	neuronLayers.front()->block(0, 0, 1, neuronLayers.front()->size() - 1) = input;

	// propagate the data forawrd
	for (int i = 1; i < topology.size(); i++) {
		// already explained above
		(*neuronLayers[i]) = (*neuronLayers[i - 1]) * (*weights[i - 1]);
	}

	// apply the activation function to your network
	// unaryExpr applies the given function to all elements of CURRENT_LAYER
	for (int i = 1; i < topology.size() - 1; i++) {
		neuronLayers[i]->block(0, 0, 1, topology[i]).unaryExpr(std::ptr_fun(activationFunction));
	}
}

void NeuralNetwork::calcErrors(RowVector& output)
{
	// calculate the errors made by neurons of last layer
	(*deltas.back()) = output - (*neuronLayers.back());

	// error calculation of hidden layers is different
	// we will begin by the last hidden layer
	// and we will continue till the first hidden layer
	for (int i = topology.size() - 2; i > 0; i--) {
		(*deltas[i]) = (*deltas[i + 1]) * (weights[i]->transpose());
	}
}

void NeuralNetwork::updateWeights()
{
	// topology.size()-1 = weights.size()
	for (int i = 0; i < topology.size() - 1; i++) {
		// in this loop we are iterating over the different layers (from first hidden to output layer)
		// if this layer is the output layer, there is no bias neuron there, number of neurons specified = number of cols
		// if this layer not the output layer, there is a bias neuron and number of neurons specified = number of cols -1
		if (i != topology.size() - 2) {
			for (int c = 0; c < weights[i]->cols() - 1; c++) {
				for (int r = 0; r < weights[i]->rows(); r++) {
					weights[i]->coeffRef(r, c) += learningRate * deltas[i + 1]->coeffRef(c) * activationFunctionDerivative(cacheLayers[i + 1]->coeffRef(c)) * neuronLayers[i]->coeffRef(r);
				}
			}
		}
		else {
			for (int c = 0; c < weights[i]->cols(); c++) {
				for (int r = 0; r < weights[i]->rows(); r++) {
					weights[i]->coeffRef(r, c) += learningRate * deltas[i + 1]->coeffRef(c) * activationFunctionDerivative(cacheLayers[i + 1]->coeffRef(c)) * neuronLayers[i]->coeffRef(r);
				}
			}
		}
	}
}

void NeuralNetwork::propagateBackward(RowVector& output)
{
	calcErrors(output);
	updateWeights();
}

Scalar activationFunction(Scalar x)
{
	return tanhf(x);
}

Scalar activationFunctionDerivative(Scalar x)
{
	return 1 - tanhf(x) * tanhf(x);
}

void NeuralNetwork::train(std::vector<RowVector*> input_data, std::vector<RowVector*> output_data)
{
	for (int i = 0; i < input_data.size(); i++) {
		std::cout << "Input to neural network is : " << *input_data[i] << std::endl;
		propagateForward(*input_data[i]);
		std::cout << "Expected output is : " << *output_data[i] << std::endl;
		std::cout << "Output produced is : " << *neuronLayers.back() << std::endl;
		propagateBackward(*output_data[i]);
		std::cout << "MSE : " << std::sqrt((*deltas.back()).dot((*deltas.back())) / deltas.back()->size()) << std::endl;
	}
}

void ReadCSV(std::string filename, std::vector<RowVector*>& data)
{
	data.clear();
	std::ifstream file(filename);
	std::string line, word;
	// determine number of columns in file
	getline(file, line, '\n');
	std::stringstream ss(line);
	std::vector<Scalar> parsed_vec;
	while (getline(ss, word, ', ')) {
		parsed_vec.push_back(Scalar(std::stof(&word[0])));
	}
	int cols = parsed_vec.size();
	data.push_back(new RowVector(cols));
	for (int i = 0; i < cols; i++) {
		data.back()->coeffRef(1, i) = parsed_vec[i];
	}

	// read the file
	if (file.is_open()) {
		while (getline(file, line, '\n')) {
			std::stringstream ss(line);
			data.push_back(new RowVector(1, cols));
			int i = 0;
			while (getline(ss, word, ', ')) {
				data.back()->coeffRef(i) = Scalar(std::stof(&word[0]));
				i++;
			}
		}
	}
}

void genData(std::string filename)
{
	std::ofstream file1(filename + "-in");
	std::ofstream file2(filename + "-out");
	for (int r = 0; r < 1000; r++) {
		Scalar x = rand() / Scalar(RAND_MAX);
		Scalar y = rand() / Scalar(RAND_MAX);
		file1 << x << ", " << y << std::endl;
		file2 << 2 * x + 10 + y << std::endl;
	}
	file1.close();
	file2.close();
}

// main.cpp

// don't forget to include out neural network
//#include "NeuralNetwork.hpp"

//... data generator code here

typedef std::vector<RowVector*> data;
int main()
{
	NeuralNetwork n({ 2, 3, 1 });
	data in_dat, out_dat;
	genData("test");
	ReadCSV("test-in", in_dat);
	ReadCSV("test-out", out_dat);
	n.train(in_dat, out_dat);
	return 0;
}











