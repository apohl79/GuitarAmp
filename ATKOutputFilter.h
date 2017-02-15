#ifndef ATKOUTPUTFILTER_H
#define ATKOUTPUTFILTER_H

#include <ATK/Core/TypedBaseFilter.h>

template <typename DataType>
class ATKOutputFilter : public ATK::TypedBaseFilter<DataType> {
  public:
    using ATK::TypedBaseFilter<DataType>::converted_inputs;

    explicit ATKOutputFilter(int channels) : ATK::TypedBaseFilter<DataType>(channels, 0), mChannels(channels) {}

    void set_outputs(double** outputs, int size) {
        mOutputs = outputs;
        mSize = size;
    }

  protected:
    double** mOutputs = nullptr;
    int mSize = 0;
    int mChannels = 1;

    virtual void process_impl(int64_t size) const {
        for (int c = 0; c < mChannels; ++c) {
            for (int64_t i = 0; i < size; ++i) {
                mOutputs[c][i] = converted_inputs[c][i];
            }
        }
    }
};

template class ATKOutputFilter<float>;
template class ATKOutputFilter<double>;

#endif  // ATKINPUTFILTER_H
