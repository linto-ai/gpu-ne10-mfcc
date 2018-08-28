#include "features.h"

float computeEnergy(int16_t* window, VADFeatsParams params)
{
    double sum = 0;
    for (int i = 0; i < params.window_width; i++)
    {
         sum += std::pow(window[i], 2);
    }
    return std::log2(sum / (double)params.window_width);
}

float computeFBAR(int16_t* window, uint fbar_lfreq_index, uint fbar_hfreq_index, VADFeatsParams params)
{
    DArray spectrum = computeSpectrum(window, params.window_width);

    DArray band = spectrum[std::slice(fbar_lfreq_index, fbar_hfreq_index - fbar_lfreq_index, 1)];
    return band.sum() / spectrum.sum();
}

DArray computeSpectrum(CArray x)
{
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
    DArray ret(x.size()/2);
    for (int i = 0; i < x.size()/2; i++)
    {
        ret[i] = std::sqrt(std::pow((double)x[i].real(),2) + std::pow((double)x[i].imag(), 2));
    }
    return ret;
}

DArray computeSpectrum(int16_t* window, uint window_width) 
{
    // DFT
    CArray x = arrayToCArray(window, window_width);  
	return computeSpectrum(x);
}

CArray arrayToCArray(int16_t* window, uint window_width){
    CArray ret(window_width);
    for (int i = 0; i < window_width; i++)
    {
        ret[i] = std::complex<double>((double) window[i], 0.0);
    }
    return ret;
}

FArray mfcc(FArray signal,
			uint sampling_frequency,
			uint window_length,
			uint overlap,
			uint n_cepstral,
			uint n_filter,
			uint fft_length)
{
	assert(overlap < window_length); 
	std::valarray<float> features;
	
	if (signal.size() < window_length) return features;
	features = filterBank(signal, sampling_frequency, window_length, overlap, n_filter, fft_length);
	return features;
}


FArray filterBank(FArray signal,
					uint sampling_frequency,
					uint window_length,
					uint overlap,
					uint n_filter,
					uint fft_length)
{
	assert(overlap < window_length);
	
	int n_frames = int(((signal.size()- window_length) / (window_length - overlap)) + 1);
	std::valarray<FArray> frames;
	frames.resize(n_frames);
	
	// Set frames
	for(int i = 0; i < n_frames ; i++)
	{
		frames[i] = FArray(signal[std::slice(i*(window_length-overlap), window_length, 1)]);
	}

	// If overlap set hanning
	if (overlap > 0)
	{
		for(int i = 0; i < n_frames ; i++)
		{
			frames[i] = hanning(frames[i]);
		}
	}
	return signal;

}

FArray hanning(FArray &window)
{
	FArray hanning(window.size());
	for (int i = 0; i < window.size(); i++)
	{
		hanning[i] = 0.5f - 0.5f * cos(2 * PI * i / window.size());
	}
	hanning *= window;
	return hanning;

}

FArray power_spectrum(FArray &window, uint fft_length)
{
	FArray spectrum = 

}