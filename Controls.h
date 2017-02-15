//
//  LabeledKnob.hpp
//  EQ3
//
//  Created by Andreas Pohl on 21/07/16.
//
//

#ifndef LabeledKnob_h
#define LabeledKnob_h

#include <iomanip>
#include <sstream>

#include "IControl.h"
#include "IPlugEasy.h"

struct Position {
    Position(int x_, int y_) : x(x_), y(y_), start_x(x), start_y(y){};
    void NewLine() {
        x = start_x;
        y += height;
    }
    void Next() { x += width; }
    int x;
    int y;
    int start_x;
    int start_y;
    int width = 0;
    int height = 0;
};

namespace LabeledKnob {

enum class LabelPosition { RIGHT, LEFT, BOTTOM };

void Create(IPlugHandler* pHandler, IGraphics* pGraphics, IBitmap* pImg, int idx, Position& position,
            const std::string& name, const std::string& unit, double init, double min, double max, double shape,
            LabelPosition labelPos, std::function<void(double)> onChangeHandler) {
    auto* param = pHandler->GetIPlugBase()->GetParam(idx);
    param->InitDouble(name.c_str(), init, min, max, 0.1, unit.c_str());
    param->SetShape(shape);

    int x = position.x;
    int y = position.y;
    int x_name, x_val, y_name, y_val, width;
    IText::EAlign align;
    switch (labelPos) {
        case LabelPosition::RIGHT:
            x_name = x_val = position.x + pImg->W + 5;
            y_name = position.y + (pImg->H / pImg->N / 2 - 15);
            width = 45;
            align = IText::kAlignNear;
            position.width = pImg->W + width + 10;
            position.height = pImg->H / pImg->N + 5;
            break;
        case LabelPosition::LEFT:
            x_name = x_val = position.x;
            x += 50;
            y_name = position.y + (pImg->H / pImg->N / 2 - 15);
            width = 45;
            align = IText::kAlignFar;
            position.width = pImg->W + width + 10;
            position.height = pImg->H / pImg->N + 5;
            break;
        case LabelPosition::BOTTOM:
            x_name = x_val = position.x;
            y_name = position.y + pImg->H / pImg->N;
            width = pImg->W;
            align = IText::kAlignCenter;
            position.width = pImg->W + 5;
            position.height = pImg->H / pImg->N + 20;
            break;
    }
    if (name == "") {
        y_val = y_name;
    } else {
        y_val = y_name + 20;
    }

    if (name != "") {
        pGraphics->AttachControl(new ITextControl(
            pHandler->GetIPlugBase(), IRECT(x_name, y_name, x_name + width, y_name + 15),
            new IText(15, &COLOR_GRAY, "Courier", IText::kStyleBold, align, 0, IText::kQualityDefault), name.c_str()));
    }

    pGraphics->AttachControl(new IKnobMultiControl(pHandler->GetIPlugBase(), x, y, idx, pImg));

    ITextControl* tc = new ITextControl(
        pHandler->GetIPlugBase(), IRECT(x_val, y_val, x_val + width, y_val + 20),
        new IText(12, &COLOR_GRAY, "Courier", IText::kStyleNormal, align, 0, IText::kQualityDefault), "");
    pGraphics->AttachControl(tc);

    pHandler->AddParamChangeHandler(idx, [param, unit, tc, onChangeHandler] {
        double val = param->Value();
        if (onChangeHandler) {
            onChangeHandler(val);
        }
        int precision = 1;
        std::string unit_str = unit;
        if (unit == "Hz" && val >= 1000.) {
            val /= 1000;
            unit_str = "KHz";
            precision = 2;
        }
        std::ostringstream s;
        s << std::setprecision(precision) << std::fixed << val << unit_str;
        tc->SetTextFromPlug(const_cast<char*>(s.str().c_str()));
    });

    position.Next();
}
}  // LabeledKnob

namespace LabeledSwitch {

enum class LabelPosition { LEFT, TOP_BOTTOM };

void Create(IPlugHandler* pHandler, IGraphics* pGraphics, IBitmap* pImg, int idx, int x, int y, const std::string& name,
            const std::string& offLabel, const std::string& onLabel, LabelPosition pos,
            std::function<void(bool)> onChangeHandler) {
    auto* param = pHandler->GetIPlugBase()->GetParam(idx);
    param->InitBool(name.c_str(), false);
    pGraphics->AttachControl(new ISwitchControl(pHandler->GetIPlugBase(), x + 60, y + 5, idx, pImg));
    switch (pos) {
        case LabelPosition::LEFT:
            pGraphics->AttachControl(new ITextControl(
                pHandler->GetIPlugBase(), IRECT(x, y + 10, x + 60, y + 10 + 15),
                new IText(12, &COLOR_GRAY, "Courier", IText::kStyleNormal, IText::kAlignFar, 0, IText::kQualityDefault),
                offLabel.c_str()));
            pGraphics->AttachControl(new ITextControl(
                pHandler->GetIPlugBase(), IRECT(x, y + 46, x + 60, y + 46 + 15),
                new IText(12, &COLOR_GRAY, "Courier", IText::kStyleNormal, IText::kAlignFar, 0, IText::kQualityDefault),
                onLabel.c_str()));
            break;
        case LabelPosition::TOP_BOTTOM:
            pGraphics->AttachControl(new ITextControl(
                pHandler->GetIPlugBase(), IRECT(x, y + 25, x + 56, y + 15),
                new IText(15, &COLOR_GRAY, "Courier", IText::kStyleBold, IText::kAlignFar, 0, IText::kQualityDefault),
                name.c_str()));
            pGraphics->AttachControl(new ITextControl(pHandler->GetIPlugBase(),
                                                      IRECT(x + 75, y - 2, x + 75 + 30, y + 15),
                                                      new IText(12, &COLOR_GRAY, "Courier", IText::kStyleNormal,
                                                                IText::kAlignNear, 0, IText::kQualityDefault),
                                                      offLabel.c_str()));
            pGraphics->AttachControl(new ITextControl(pHandler->GetIPlugBase(),
                                                      IRECT(x + 75, y + 56, x + 75 + 30, y + 60 + 15),
                                                      new IText(12, &COLOR_GRAY, "Courier", IText::kStyleNormal,
                                                                IText::kAlignNear, 0, IText::kQualityDefault),
                                                      onLabel.c_str()));
            break;
    }
    pHandler->AddParamChangeHandler(idx, [param, onChangeHandler] {
        if (onChangeHandler) {
            onChangeHandler(param->Bool());
        }
    });
}
}  // LabeledSwitch

#endif /* Controls_h */
