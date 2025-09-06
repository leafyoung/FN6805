// https://replit.com/@YeKunlun/62-randdist?v=1

#include <algorithm>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>

using namespace std;

using bool_func = function<bool(int)>;

int q3();

int main() {

  q3();

  seed_seq seed{12789};
  mt19937 mtgen{seed};

  {
    uniform_real_distribution<> uid(0, 10);
    cout << "seed: ";
    seed.param(std::ostream_iterator<int>(std::cout, " "));
    cout << '\n';

    // save status of the generator to oss / restore from iss
    // alternatively, we can save to a file with ofstream/ifstream
    ostringstream oss;
    oss << mtgen;

    cout << uid(mtgen) << '\n';
    cout << uid(mtgen) << '\n';

    // restore the status of the generator
    istringstream iss{oss.str()};
    iss >> mtgen;

    cout << "same sequence after restore the sequence\n";
    cout << uid(mtgen) << '\n';
    cout << uid(mtgen) << '\n' << '\n';
  }

  uniform_int_distribution<unsigned int> uniInt(0, 1000); // [a, b]
  uniform_real_distribution<double> uniDist(0.0, 1.0);    // [a, b)
  normal_distribution<> stdNorm(0.0, 1.0);                // Default is double
  exponential_distribution<> expDist(1.0);

  cout << uniInt.min() << ", " << uniInt.max() << "\n";

  cout << "Single Examples:\n"
       << "\n";
  cout << "From Uniform int Distribution:     " << uniInt(mtgen) << "\n";
  cout << "From Uniform Distribution:     " << uniDist(mtgen) << "\n";
  cout << "From Normal Distribution:      " << stdNorm(mtgen) << "\n";
  cout << "From Exponential Distribution: " << expDist(mtgen) << "\n";

  cout << "\n\n";

  cout << "Calculating Basic Statistics\n\n";
  vector<double> Uni(1000, 0.0), Norm(1000, 0.0), Expo(1000, 0.0);
  double uni = 0, norm = 0, expo = 0;
  for (int i = 0; i < 1000; ++i) {
    Uni[i] = uniDist(mtgen);
    uni += Uni[i];

    Norm[i] = stdNorm(mtgen);
    norm += Norm[i];

    Expo[i] = expDist(mtgen);
    expo += Expo[i];
  }

  uni /= 1000.0;
  norm /= 1000.0;
  expo /= 1000.0;

  cout << "Sample Mean Uniform     : " << uni << '\n';
  cout << "Sample Mean Normal      : " << norm << '\n';
  cout << "Sample Mean Exponential : " << expo << '\n';

  double Vu = 0, Vn = 0, Ve = 0;

  for (int i = 0; i < 1000; ++i) {
    Vu += pow((uni - Uni[i]), 2);
    Vn += pow((norm - Norm[i]), 2);
    Ve += pow((expo - Expo[i]), 2);
  }

  Vu /= 1000.0;
  Vn /= 1000.0;
  Ve /= 1000.0;

  cout << "Sample Variance Uniform     : " << Vu << '\n';
  cout << "Sample Variance Normal      : " << Vn << '\n';
  cout << "Sample Variance Exponential : " << Ve << '\n';

  return 0;
}