#include <cmath>
#include <complex>
#include <valarray>

const double PI = 3.14159265358979323846264338328L;
typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;
typedef std::valarray<double> DArray;

struct VADFeatsParams{
    u_int32_t samplingRate = 16000;
    u_int16_t window_width = 1024; //TODO: Change to 2048
    u_int16_t overlap = 0;
    u_int16_t fbar_lfreq = 80;
    u_int16_t fbar_hfreq = 1000;
};

double computeEnergy(int16_t* window, VADFeatsParams params);
float computeFBAR(int16_t* window, uint fbar_lfreq_index, uint fbar_hfreq_index, VADFeatsParams params);
DArray computeSpectrum(int16_t* window, VADFeatsParams params);
CArray arrayToCArray(int16_t* window, VADFeatsParams params);