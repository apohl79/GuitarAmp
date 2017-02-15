//
//  LabeledKnob.h
//  GuitarAmp
//
//  Created by Andreas Pohl on 20/10/16.
//
//

#ifndef LabeledKnob_h
#define LabeledKnob_h

#include <sstream>
#include <string>

#include "IControl.h"

class LabeledKnob : public IKnobMultiControl {
  public:
    enum class LabelPosition { RIGHT, LEFT, BOTTOM, TOP_BOTTOM };

    LabeledKnob(IPlugBase* pPlug, IRECT rect, int paramIdx, IBitmap* pImg, LabelPosition lpos, IText& textTitle,
                IText& textValue, char* name, char* unit, bool showValue)
        : IKnobMultiControl(pPlug, rect.L, rect.T, paramIdx, pImg),
          mLabelPos(lpos),
          mTextTitle(textTitle),
          mTextValue(textValue),
          mName(name),
          mUnit(unit),
          mShowValue(showValue) {
        int x = rect.L, y = rect.T, nameX = 0, nameY = 0, valX = 0, valY = -1, width = 0;
        // TODO: this stuff should by more dynamic...
        switch (mLabelPos) {
            case LabelPosition::RIGHT:
                nameX = valX = x + pImg->W + 5;
                nameY = y + (pImg->H / pImg->N / 2 - (mTextTitle.mSize + 1));
                width = 45;
                mTextTitle.mAlign = mTextValue.mAlign = IText::kAlignNear;
                break;
            case LabelPosition::LEFT:
                nameX = valX = x;
                x += 50;
                nameY = y + (pImg->H / pImg->N / 2 - (mTextTitle.mSize + 1));
                width = 45;
                mTextTitle.mAlign = mTextValue.mAlign = IText::kAlignFar;
                break;
            case LabelPosition::BOTTOM:
                nameX = valX = x;
                nameY = y + pImg->H / pImg->N;
                width = pImg->W;
                mTextTitle.mAlign = mTextValue.mAlign = IText::kAlignCenter;
                break;
            case LabelPosition::TOP_BOTTOM:
                nameX = valX = rect.L;
                nameY = rect.T;
                x = rect.L + rect.W() / 2 - pImg->W / 2;
                y += 15;
                valY = y + pImg->H / pImg->N;
                width = rect.W();
                mTextTitle.mAlign = mTextValue.mAlign = IText::kAlignCenter;
                break;
        }
        if (valY == -1) {
            if (*name == 0) {
                valY = nameY;
            } else {
                valY = nameY + mTextTitle.mSize + 6;
            }
        }
        mRECT = IRECT(x, y, pImg);
        mTargetRECT = mRECT;
        mNameR = IRECT(nameX, nameY, nameX + width, nameY + mTextTitle.mSize + 1);
        mValR = IRECT(valX, valY, valX + width, valY + mTextTitle.mSize + 6);
    }

    virtual void SetValueFromPlug(double value) {
        IControl::SetValueFromPlug(value);
        UpdateValueString();
    }

    virtual void SetValueFromUserInput(double value) {
        IControl::SetValueFromUserInput(value);
        UpdateValueString();
    }

    virtual void OnMouseDrag(int x, int y, int dX, int dY, IMouseMod* pMod) {
        IKnobControl::OnMouseDrag(x, y, dX, dY, pMod);
        UpdateValueString();
    }

    virtual void OnMouseWheel(int x, int y, IMouseMod* pMod, int d) {
        IKnobControl::OnMouseWheel(x, y, pMod, d);
        UpdateValueString();
    }

    virtual bool Draw(IGraphics* pGraphics) {
        if (*mName != 0) {
            pGraphics->DrawIText(&mTextTitle, mName, &mNameR);
        }
        if (mShowValue) {
            pGraphics->DrawIText(&mTextValue, const_cast<char*>(mValStr.c_str()), &mValR);
        }
        return IKnobMultiControl::Draw(pGraphics);
    }

  private:
    LabelPosition mLabelPos;
    IText mTextTitle, mTextValue;
    char* mName;
    char* mUnit;
    IRECT mNameR, mValR;
    std::string mValStr;
    bool mShowValue;

    void UpdateValueString() {
        if (mShowValue) {
            int precision = 3;
            auto val = GetParam()->Value();
            std::string unit = mUnit;
            if (unit == "Hz" && val >= 1000.) {
                val /= 1000;
                unit = "KHz";
                precision = 2;
            } else if (unit == "Ohm") {
                precision = 0;
                if (val >= 1000.) {
                    val /= 1000;
                    unit = "KOhm";
                }
                if (val >= 1000.) {
                    val /= 1000;
                    unit = "MOhm";
                }
            } else if (unit == "F") {
                precision = 0;
                if (val <= 1) {
                    val *= 1000;
                    unit = "mF";
                }
                if (val <= 1) {
                    val *= 1000;
                    unit = "µF";
                }
                if (val <= 1) {
                    val *= 1000;
                    unit = "nF";
                }
                if (val <= 1) {
                    val *= 1000;
                    unit = "pF";
                }
            } else if (unit == "V") {
                precision = 1;
                if (val < 1) {
                    val *= 1000;
                    unit = "mV";
                }
            } else {
                precision = 1;
                double x = 0.1;
                while (x > 0.00000001 && val < x) {
                    precision++;
                    x /= 10;
                }
            }
            std::ostringstream s;
            s << std::setprecision(precision) << std::fixed << val << unit;
            mValStr = s.str();
        }
    }
};

#endif /* LabeledKnob_h */
