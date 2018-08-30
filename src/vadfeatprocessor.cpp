#include "../include/vadfeatprocessor.h"

VADFeatProcessor::VADFeatProcessor()
{
    // Setting cut frequencies index
    float i = 0.0;
    float step = (params.samplingRate / 2.0) / (params.window_width / 2.0);
    for (i;i * step < params.fbar_lfreq; i++) {}
    fbar_lfreq_index = (i-1);
    for (i;i *step < params.fbar_hfreq; i++) {}
    fbar_hfreq_index = i;

    isActive = true;
}

void VADFeatProcessor::set_input(BlockingQueue<int16_t*>* queue){
    input_queue = queue;
}

void VADFeatProcessor::run(){
    int16_t* input;
    std::valarray<float> res(2);
    while(true)
    {
        input = input_queue->pop();
        //TODO add buffer
        if (isActive) 
        {
            res[0] = _computeEnergy(input);
            res[1] = _computeFBAR(input);
            //std::cout << "E: " << res[0] << std::endl;
            //std::cout << "FBAR: " << res[1] << std::endl;
        }
        
        for (BlockingQueue<std::valarray<float>>* queue : outputQueues)
        {
            queue->push(res);
        }
    }
}

BlockingQueue<std::valarray<float>>* VADFeatProcessor::subscribe()
{
    BlockingQueue<std::valarray<float>>* queue = new BlockingQueue<std::valarray<float>>(10);
    outputQueues.push_back(queue);
    return queue;
}


double VADFeatProcessor::_computeEnergy(int16_t* window)
{
    double sum = 0;
    for (int i = 0; i < params.window_width; i++)
    {
         sum += std::pow(window[i], 2);
    }
    return std::log2(sum / (double)params.window_width);
}

float VADFeatProcessor::_computeFBAR(int16_t* window)
{
    DArray spectrum = _computeSpectrum(window);

    DArray band = spectrum[std::slice(fbar_lfreq_index, fbar_hfreq_index - fbar_lfreq_index, 1)];
    return band.sum() / spectrum.sum();
}

DArray VADFeatProcessor::_computeSpectrum(int16_t* window) 
{
    // DFT
    CArray x = _arrayToCArray(window);  
	unsigned int N = x.size(), k = N, n;
	double thetaT = PI / N;
	Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
	while (k > 1)
	{
		n = k;
		k >>= 1;
		phiT = phiT * phiT;
		T = 1.0L;
		for (unsigned int l = 0; l < k; l++)
		{
			for (unsigned int a = l; a < N; a += n)
			{
				unsigned int b = a + k;
				Complex t = x[a] - x[b];
				x[a] += x[b];
				x[b] = t * T;
			}
			T *= phiT;
		}
	}
	// Decimate
	unsigned int m = (unsigned int)log2(N);
	for (unsigned int a = 0; a < N; a++)
	{
		unsigned int b = a;
		// Reverse bits
		b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
		b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
		b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
		b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
		b = ((b >> 16) | (b << 16)) >> (32 - m);
		if (b > a)
		{
			Complex t = x[a];
			x[a] = x[b];
			x[b] = t;
		}
	}

    //Spectrum
    DArray ret(params.window_width/2);
    for (int i = 0; i < params.window_width/2; i++)
    {
        ret[i] = std::sqrt(std::pow((double)x[i].real(),2) + std::pow((double)x[i].imag(), 2));
    }
    return ret;
}

CArray VADFeatProcessor::_arrayToCArray(int16_t* window){
    CArray ret(params.window_width);
    for (int i = 0; i < params.window_width; i++)
    {
        ret[i] = std::complex<double>((double) window[i], 0.0);
    }
    return ret;
}