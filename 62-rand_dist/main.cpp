#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std;

using bool_func = function<bool(int)>;

int main() {
  seed_seq seed{12789};
  mt19937 gen{seed};

  uniform_int_distribution<unsigned int> uniInt(0, 1000); // [a, b]
  uniform_real_distribution<double> uniDist(0.0, 1.0);    // [a, b)
  normal_distribution<> stdNorm(0.0, 1.0);                // Default is double
  exponential_distribution<> expDist(1.0);

  cout << uniInt.max() << endl;
  cout << uniInt.min() << endl;

  cout << "Single Examples:\n" << endl;
  cout << "From Uniform int Distribution:     " << uniInt(gen) << endl;
  cout << "From Uniform Distribution:     " << uniDist(gen) << endl;
  cout << "From Normal Distribution:      " << stdNorm(gen) << endl;
  cout << "From Exponential Distribution: " << expDist(gen) << endl;

  cout << "\n" << endl;

  cout << "Calculating Basic Statistics\n" << endl;
  vector<double> Uni(1000, 0.0), Norm(1000, 0.0), Expo(1000, 0.0);
  double uni = 0, norm = 0, expo = 0;
  for (int i = 0; i < 1000; i++) {
    Uni[i] = uniDist(gen);
    uni += Uni[i];

    Norm[i] = stdNorm(gen);
    norm += Norm[i];

    Expo[i] = expDist(gen);
    expo += Expo[i];
  }

  uni /= 1000.0;
  norm /= 1000.0;
  expo /= 1000.0;

  cout << "Sample Mean Uniform     : " << uni << endl;
  cout << "Sample Mean Normal      : " << norm << endl;
  cout << "Sample Mean Exponential : " << expo << endl;

  double Vu = 0, Vn = 0, Ve = 0;

  for (int i = 0; i < 1000; i++) {
    Vu += pow((uni - Uni[i]), 2);
    Vn += pow((norm - Norm[i]), 2);
    Ve += pow((expo - Expo[i]), 2);
  }

  Vu /= 1000.0;
  Vn /= 1000.0;
  Ve /= 1000.0;

  cout << "Sample Variance Uniform     : " << Vu << endl;
  cout << "Sample Variance Normal      : " << Vn << endl;
  cout << "Sample Variance Exponential : " << Ve << endl;

  return 0;
}