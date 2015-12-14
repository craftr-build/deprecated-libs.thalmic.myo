// Copyright (c) 2015  Niklas Rosenstein
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// ===========================================================================
//
// This is an example Myo DeviceListener implementation that measures the
// rate of incoming EMG data.

#include <numeric>
#include <ctime>
#include <iostream>
#include <list>
#include <myo/myo.hpp>

// ===========================================================================
// ===========================================================================
class EmgRateMeasure : public myo::DeviceListener
{
public:
  EmgRateMeasure(int n);
  double getRate() const;
  void onEmgData(myo::Myo*, uint64_t, const int8_t*) override;
private:
  int _n;
  double _last_time;
  std::list<double> _times;
};

// ===========================================================================
// ===========================================================================
EmgRateMeasure::EmgRateMeasure(int n) : _n(n), _last_time(0.0), _times()
{
}

// ===========================================================================
// ===========================================================================
double EmgRateMeasure::getRate() const
{
  if (_times.size()) {
    return 1.0 / std::accumulate(_times.begin(), _times.end(), 0.0) * double(_times.size());
  }
  return 0.0;
}

// ===========================================================================
// ===========================================================================
void EmgRateMeasure::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
  double const t = clock() / double(CLOCKS_PER_SEC);
  if (_last_time != 0.0) {
    _times.push_back(t - _last_time);
    while (_times.size() > _n) {
      _times.pop_front();
    }
  }
  _last_time = t;
}

// ===========================================================================
// ===========================================================================
int main()
{
  try {
    std::cerr << "Creating Hub and waiting for a Myo device..." << std::endl;
    myo::Hub hub("com.nr.emg_rate");
    myo::Myo* myo = hub.waitForMyo(1000);
    if (!myo) {
      std::cerr << "No Myo found." << std::endl;
      return 1;
    }

    // Enable EMG streaming.
    myo->setStreamEmg(myo::Myo::streamEmgEnabled);

    // Add the listener and pring the result every 100 ms.
    EmgRateMeasure listener(50);
    hub.addListener(&listener);
    for (;;) {
      hub.run(100);
      std::cout << "\r\033[KEMG Rate: " << listener.getRate();
    }
  }
  catch (const std::exception& e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }
  return 0;
}
