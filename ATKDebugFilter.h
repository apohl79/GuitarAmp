//
//  ATKDebugFilter.h
//  GuitarAmp
//
//  Created by Andreas Pohl on 28/09/16.
//
//

#ifndef ATKDebugFilter_h
#define ATKDebugFilter_h

#include <atomic>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <sstream>
#include <string>
#include <thread>

#include <ATK/Core/TypedBaseFilter.h>

template <typename DataType>
class ATKDebugFilter : public ATK::TypedBaseFilter<DataType> {
  public:
    using ATK::TypedBaseFilter<DataType>::outputs;
    using ATK::TypedBaseFilter<DataType>::converted_inputs;

    ATKDebugFilter(const std::string& name, int start = 0, int end = 0)
        : ATK::TypedBaseFilter<DataType>(1, 1), mStart(start), mEnd(end) {
        std::ostringstream fn;
        fn << "/tmp/" << name << ".dat";
        mFile.open(fn.str());

        mWriter = std::thread([this] {
            int row = 0;
            while (!mTerm) {
                DataType x;
                bool write = false;
                {
                    std::lock_guard<std::mutex> lock(mMtx);
                    if (!mLog.empty()) {
                        x = mLog.front();
                        mLog.pop();
                        write = true;
                    }
                }
                if (write) {
                    mFile << row++ << " " << x << std::endl;
                } else {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
            }
        });
    }

    ~ATKDebugFilter() {
        mTerm = true;
        if (mWriter.joinable()) {
            mWriter.join();
        }
    }

  protected:
    mutable std::ofstream mFile;
    mutable std::queue<DataType> mLog;
    mutable std::mutex mMtx;
    mutable int mStart;
    mutable int mEnd;
    mutable int mI = 0;

    std::atomic_bool mTerm{false};
    std::thread mWriter;

    virtual void process_impl(int64_t size) const override final {
        for (int64_t i = 0; i < size; ++i) {
            auto x = converted_inputs[0][i];
            if (mI >= mStart && (mEnd == 0 || mI <= mEnd)) {
                std::lock_guard<std::mutex> lock(mMtx);
                mLog.push(x);
            }
            outputs[0][i] = x;
            if (mI <= mEnd) {
                mI++;
            }
        }
    }
};

#endif /* ATKDebugFilter_h */
