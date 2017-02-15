#ifndef __GUITARAMP__
#define __GUITARAMP__

#include "ATKDebugFilter.h"
#include "ATKInputFilter.h"
#include "ATKOutputFilter.h"
#include "IPlugEasy.h"
#include "IPlug_include_in_plug_hdr.h"
#include "PeakMeter.h"
#include "r8brain-free-src/CDSPResampler.h"

#include <ATK/EQ/ButterworthFilter.h>
#include <ATK/EQ/IIRFilter.h>
#include <ATK/EQ/ToneStackFilter.h>
#include <ATK/Preamplifier/Triode3Filter.h>
#include <ATK/Special/ConvolutionFilter.h>
#include <ATK/Tools/DecimationFilter.h>
#include <ATK/Tools/OversamplingFilter.h>
#include <ATK/Tools/SumFilter.h>
#include <ATK/Tools/VolumeFilter.h>

#include <memory>

// Helper to avoid some code duplication
#define UPDATE_FILTER(f, p) \
    [this](double v) {      \
        if (f) {            \
            f->p(v);        \
        }                   \
    }

class Impulse;

class GuitarAmp : public IPlug, IPlugEasy {
  public:
    explicit GuitarAmp(IPlugInstanceInfo instanceInfo);
    ~GuitarAmp();

    IPLUGEASY_DECL();

    void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

  private:
    using InType = ATKInputFilter<double>;
    using OutType = ATKOutputFilter<double>;
    using BaseFilterType = ATK::TypedBaseFilter<double>;
    using SumType = ATK::SumFilter<double>;
    using VolumeType = ATK::VolumeFilter<double>;
    using OversamplingType_2 = ATK::OversamplingFilter<double, ATK::Oversampling6points5order_2<double>>;
    using OversamplingType_4 = ATK::OversamplingFilter<double, ATK::Oversampling6points5order_4<double>>;
    using OversamplingType_8 = ATK::OversamplingFilter<double, ATK::Oversampling6points5order_8<double>>;
    using DownsampingType = ATK::DecimationFilter<double>;
    using LowPassType = ATK::IIRFilter<ATK::ButterworthLowPassCoefficients<double>>;
    using TubeFilterType = ATK::Triode3Filter<double>;
    using ToneStackType = ATK::IIRFilter<ATK::ToneStackCoefficients<double>>;
    using CabType = ATK::ConvolutionFilter<double>;
    using PeakFilterType = PeakMeterFilter<double>;

    static constexpr int NUM_TUBES = 4;
    static constexpr int CONVO_SPLIT_SIZE = 128;
    static constexpr int RESAMPLE_BLOCK_SIZE = 64;
    // static constexpr int IR_SAMPLERATE = 44100;

    static constexpr bool WITH_TUBE_PARAMS = false;

    int mOversamplingFactor = 4;
    ATK::Triode3Type mTubeType = ATK::Triode3Type::ECC83;

    std::unique_ptr<InType> mInputs;
    std::unique_ptr<OutType> mOutputs;
    std::unique_ptr<SumType> mSum;
    std::unique_ptr<VolumeType> mGain;
    std::unique_ptr<VolumeType> mVolume;
    std::unique_ptr<BaseFilterType> mOversample;
    std::unique_ptr<DownsampingType> mDownsample;
    std::unique_ptr<LowPassType> mLowPass;
    std::unique_ptr<TubeFilterType> mTube[NUM_TUBES];
    std::unique_ptr<ToneStackType> mToneStack;
    std::unique_ptr<CabType> mCabinet;
    std::unique_ptr<PeakFilterType> mPeakOut;
    // std::unique_ptr<r8b::CDSPResampler16IR> mResampler;

    // Use raw pointers here, IPlug takes care of the cleanup.
    PeakMeter* mMeterOut;

    // Parameter ID's
    int mRiID[NUM_TUBES];
    int mRgID[NUM_TUBES];
    int mRaID[NUM_TUBES];
    int mCiID[NUM_TUBES];
    int mCoID[NUM_TUBES];
    int mVkID[NUM_TUBES];
    int mVbID, mGainID, mVolID, mLowID, mMidID, mHighID, mCabID;
    int mCab = 0;
    bool mBypass = false;

    // Custom IR
    std::unique_ptr<Impulse> mCustIR;
    WDL_String mCustFile;
    WDL_String mCustDir;

    void Setup();
    void SetupTubes(bool useTubeParameters = false);
    void SetupCabinet();
    bool LoadIRFromFile(const char* file);
    void Resample(std::vector<double>& in, std::vector<double>& out, int rateSrc, int rateDst);

    inline void SetUIParam(int idx, double val) {
        GetParam(idx)->Set(val);
        auto* gui = GetGUI();
        if (gui) {
            gui->SetParameterFromPlug(idx, val, false);
        }
    }
};

#endif
