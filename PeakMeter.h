//
//  PeakMeter.h
//  EQ3
//
//  Created by Andreas Pohl on 20/07/16.
//
//

#ifndef PeakMeter_h
#define PeakMeter_h

#include <ATK/Core/TypedBaseFilter.h>
#include <vector>

#include "IControl.h"

class PeakMeter : public IControl {
  public:
    PeakMeter(IPlugBase* pPlug, IRECT pR, bool showScale = false, bool dbScale = true)
        : IControl(pPlug, pR),
          mShowScale(showScale),
          mColorFg(200, 100, 200, 0),
          mColorFg2(100, 100, 200, 0),
          mColorFgMax(200, 50, 255, 50),
          mColorFgMax2(35, 50, 255, 50),
          mColorFgHot(200, 185, 125, 0),
          mColorFgHot2(35, 185, 125, 0),
          mColorFgClip(200, 255, 0, 0),
          mColorFgClip2(35, 255, 0, 0),
          mColorBg(200, 25, 25, 25),
          mColorScale(COLOR_GRAY),
          mColorClipIndOff(200, 80, 0, 0),
          mColorClipIndOn(250, 250, 0, 0),
          mScaleFont(9, &mColorScale, "Courier", IText::kStyleNormal, IText::kAlignFar, 0, IText::kQualityDefault),
          mDbScale(dbScale) {
        mNormRect = IRECT(pR.L, pR.T + 4, pR.R, pR.B);
        mClipRect = IRECT(pR.L, pR.T, pR.R, pR.T + 4);
        mClampHi = 1.99526231496888;  // +6dB max
        mClampLo = 0.001;             // -60dB min
        mValue = mClampLo;
        mScaleCurve = 100.;
        mScaleFactor = float(mRECT.H() - 1) / (pow(10, 66. / mScaleCurve) - 1);
        SetVMax(20);
    }

    ~PeakMeter() {}

    /// Process method to be used from IPlugs ProcessDoubleReplacing()
    void Process(double** inputs, int nFrames, int channels) {
        double peakL = 0.0, peakR = 0.0;
        double* L = inputs[0];
        double* R = inputs[1];
        for (int s = 0; s < nFrames; ++s, ++L, ++R) {
            peakL = IPMAX(peakL, fabs(*L));
            peakR = IPMAX(peakR, fabs(*R));
        }
        if (channels == 1) {
            peakR = peakL;
        }
        SetValue(0, peakL);
        SetValue(1, peakR);
    }

    void SetStereo(bool b) { mStereo = b; }

    void SetVMax(int v) {
        mMaxVScale = v;
        mScaleVStep = float(mNormRect.H() - 2) / mMaxVScale;
    }

    void OnMouseUp(int x, int y, IMouseMod* pMod) { mClipped = false; }

    void SetValue(int channel, double value) {
        double x_peak = (value > mPrevPeak[channel] ? mAttack : mDecay);
        double x_max = (value > mPrevMax[channel] ? mAttackMax : mDecayMax);
        double max = value * x_max + mPrevMax[channel] * (1. - x_max);
        mPrevMax[channel] = max;
        mMax[channel] = max;
        value = value * x_peak + mPrevPeak[channel] * (1. - x_peak);
        mPrevPeak[channel] = value;
        mVal[channel] = value;
        if (value >= mClampHi) {
            mClipped = true;
        }
    }

    int DbToHeight(float db) { return (pow(10, (db + 60.) / mScaleCurve) - 1) * mScaleFactor; }

    int VToHeight(float v) { return v * mScaleVStep; }

    IRECT ScaleTextRectByDb(float db) {
        int y = (mNormRect.T + (mNormRect.H() - DbToHeight(db))) - 6;
        return IRECT(mNormRect.L - 17, y, mNormRect.L - 1, y + 15);
    }

    IRECT ScaleTextRectByV(float v) {
        int y = (mNormRect.T + (mNormRect.H() - VToHeight(v))) - 6;
        return IRECT(mNormRect.L - 17, y, mNormRect.L - 1, y + 15);
    }

    void DrawChannel(IGraphics* pGraphics, int channel, IRECT& rect, IRECT& rectClip) {
        float val = mDbScale ? 20 * log10(mVal[channel]) : mVal[channel];
        float max = mDbScale ? 20 * log10(mMax[channel]) : mMax[channel];

        int val_height = mDbScale ? DbToHeight(val) : VToHeight(val);
        int max_height = mDbScale ? DbToHeight(max) : VToHeight(max);

        pGraphics->FillIRect(&mColorBg, &rect);
        pGraphics->FillIRect(&mColorBg, &rectClip);

        IRECT tmp = IRECT(rectClip.L + 1, rectClip.T + 1, rect.R - 1, rectClip.B - 1);
        if (mClipped) {
            pGraphics->FillIRect(&mColorClipIndOn, &tmp);
        } else {
            pGraphics->FillIRect(&mColorClipIndOff, &tmp);
        }

        auto h_val = IPMAX(rect.B - (val_height - 1), rect.T + 1);
        h_val = IPMIN(h_val, rect.B - 2);
        tmp = IRECT(rect.L + 1, h_val, rect.R - 1, h_val + 2);
        pGraphics->FillIRect(&mColorFg, &tmp);
        tmp = IRECT(rect.L + 1, h_val + 2, rect.R - 1, rect.B - 1);
        pGraphics->FillIRect(&mColorFg2, &tmp);

        auto h_max = IPMAX(rect.B - (max_height - 1), rect.T + 1);
        h_max = IPMIN(h_max, rect.B - 2);
        tmp = IRECT(rect.L + 1, h_max, rect.R - 1, h_max + 2);
        if (mDbScale) {
            if (max > 0.) {
                pGraphics->FillIRect(&mColorFgClip, &tmp);
            } else if (max > -6.) {
                pGraphics->FillIRect(&mColorFgHot, &tmp);
            } else {
                pGraphics->FillIRect(&mColorFgMax, &tmp);
            }
        } else {
            pGraphics->FillIRect(&mColorFgMax, &tmp);
        }
        tmp = IRECT(rect.L + 1, h_max + 2, rect.R - 1, h_val);
        if (mDbScale) {
            if (max > 0.) {
                pGraphics->FillIRect(&mColorFgClip2, &tmp);
            } else if (max > -6.) {
                pGraphics->FillIRect(&mColorFgHot2, &tmp);
            } else {
                pGraphics->FillIRect(&mColorFgMax2, &tmp);
            }
        } else {
            pGraphics->FillIRect(&mColorFgMax2, &tmp);
        }
    }

    bool Draw(IGraphics* pGraphics) {
        if (mStereo) {
            IRECT tmp = IRECT(mNormRect.L, mNormRect.T, mNormRect.L + mNormRect.W() / 2, mNormRect.B);
            IRECT tmpClip = IRECT(mClipRect.L, mClipRect.T, mClipRect.L + mClipRect.W() / 2, mClipRect.B);
            DrawChannel(pGraphics, 0, tmp, tmpClip);
            tmp = IRECT(mNormRect.L + mNormRect.W() / 2, mNormRect.T, mNormRect.R, mNormRect.B);
            tmpClip = IRECT(mClipRect.L + mClipRect.W() / 2, mClipRect.T, mClipRect.R, mClipRect.B);
            DrawChannel(pGraphics, 1, tmp, tmpClip);
        } else {
            DrawChannel(pGraphics, 0, mNormRect, mClipRect);
        }
        if (mShowScale) {
            if (mDbScale) {
                IRECT tmp = ScaleTextRectByDb(6.);
                pGraphics->DrawIText(&mScaleFont, "6-", &tmp);
                tmp = ScaleTextRectByDb(0.);
                pGraphics->DrawIText(&mScaleFont, "0-", &tmp);
                tmp = ScaleTextRectByDb(-6.);
                pGraphics->DrawIText(&mScaleFont, "6-", &tmp);
                tmp = ScaleTextRectByDb(-18.);
                pGraphics->DrawIText(&mScaleFont, "18-", &tmp);
                tmp = ScaleTextRectByDb(-30.);
                pGraphics->DrawIText(&mScaleFont, "30-", &tmp);
                tmp = ScaleTextRectByDb(-42.);
                pGraphics->DrawIText(&mScaleFont, "42-", &tmp);
                tmp = ScaleTextRectByDb(-60.);
                pGraphics->DrawIText(&mScaleFont, "60-", &tmp);
            } else {
                for (int i = 0; i < mMaxVScale + 1; ++i) {
                    auto s = std::to_string(i);
                    s += "-";
                    IRECT tmp = ScaleTextRectByV(i);
                    pGraphics->DrawIText(&mScaleFont, const_cast<char*>(s.c_str()), &tmp);
                }
            }
        }
        return true;
    }

    bool IsDirty() { return true; }

  protected:
    IColor mColorFg, mColorFg2, mColorFgMax, mColorFgMax2, mColorFgHot, mColorFgHot2, mColorFgClip, mColorFgClip2,
        mColorBg, mColorScale, mColorClipIndOn, mColorClipIndOff;
    IRECT mNormRect, mClipRect;
    bool mStereo;
    double mVal[2];
    double mMax[2];
    bool mShowScale;
    IText mScaleFont;
    float mScaleFactor;
    float mScaleCurve;
    bool mDbScale;
    int mMaxVScale = 20;
    float mScaleVStep;
    double mPrevPeak[2] = {0., 0.};
    double mPrevMax[2] = {0., 0.};
    double mAttack = 0.8;
    double mDecay = 0.02;
    double mAttackMax = 1.;
    double mDecayMax = 0.001;
    bool mClipped = false;
};

/// ATK filter to be able to plug a peakmeter into a workflow
template <typename DataType>
class PeakMeterFilter : public ATK::TypedBaseFilter<DataType> {
  public:
    using Parent = ATK::TypedBaseFilter<DataType>;
    using Parent::outputs;
    using Parent::converted_inputs;
    using Parent::set_nb_input_ports;
    using Parent::get_nb_input_ports;
    using Parent::set_nb_output_ports;
    using Parent::get_nb_output_ports;

    PeakMeterFilter(int channels, PeakMeter* meter) : Parent(channels, channels), mMeter(meter) {}

  protected:
    void process_impl(int64_t size) const override final {
        DataType peak[2] = {0., 0.};
        int channels = get_nb_input_ports();
        for (int c = 0; c < channels; ++c) {
            for (int64_t i = 0; i < size; ++i) {
                auto x = converted_inputs[c][i];
                peak[c] = IPMAX(peak[c], fabs(x));
                outputs[c][i] = x;
            }
        }
        if (channels == 1) {
            peak[1] = peak[0];
        }
        mMeter->SetValue(0, peak[0]);
        mMeter->SetValue(1, peak[1]);
    }

  private:
    PeakMeter* mMeter;
};

template class PeakMeterFilter<float>;
template class PeakMeterFilter<double>;

#endif /* PeakMeter_h */
