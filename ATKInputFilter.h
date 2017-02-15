#ifndef ATKINPUTFILTER_H
#define ATKINPUTFILTER_H

#include <ATK/Core/TypedBaseFilter.h>

template <typename DataType>
class ATKInputFilter : public ATK::TypedBaseFilter<DataType> {
  public:
    using ATK::TypedBaseFilter<DataType>::outputs;

    explicit ATKInputFilter(int channels) : ATK::TypedBaseFilter<DataType>(0, channels), mChannels(channels) {}

    void set_inputs(double** inputs, int size) {
        mInputs = inputs;
        mSize = size;
    }

  protected:
    double** mInputs = nullptr;
    int mSize = 0;
    int mChannels = 1;

    virtual void process_impl(int64_t size) const {
        for (int c = 0; c < mChannels; ++c) {
            for (int64_t i = 0; i < size; ++i) {
                outputs[c][i] = mInputs[c][i];
            }
        }
    }
};

template class ATKInputFilter<float>;
template class ATKInputFilter<double>;

#endif  // ATKINPUTFILTER_H
