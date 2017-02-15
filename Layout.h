#include "IControl.h"
#include "LabeledKnob.h"
#include "SelectBox.h"

#include <iomanip>
#include <limits>
#include <sstream>

class Layout {
  public:
    Layout(int x, int y, int w, int h, int cols, int rows) : mX(x), mY(y), mW(w), mH(h), mCols(cols), mRows(rows) {
        mCellW = mW / cols;
        mCellH = mH / rows;
    }

    inline void ToCell(int col, int row) {
        if (col >= 0 && col < mCols && row >= 0 && row < mRows) {
            mCol = col;
            mRow = row;
            mIndex = row * mCols + col;
        }
    }

    inline void Next(int n = 1) {
        for (int i = 0; i < n; ++i) {
            mCol++;
            if (mCol == mCols) {
                mCol = 0;
                mRow++;
            }
            mIndex++;
        }
    }

    inline void NewLine() {
        mRow++;
        mCol = 0;
        mIndex = mRow * mCols;
    }

    inline int X() const { return mX; }
    inline int Y() const { return mY; }
    inline int W() const { return mW; }
    inline int H() const { return mH; }
    inline int L() const { return mX; }
    inline int T() const { return mY; }
    inline int R() const { return mX + mW; }
    inline int B() const { return mY + mH; }

    inline int CellX() const { return mX + mCol * mCellW; }
    inline int CellY() const { return mY + mRow * mCellH; }
    inline int CellW() const { return mCellW; }
    inline int CellH() const { return mCellH; }
    inline int CellL() const { return CellX(); }
    inline int CellT() const { return CellY(); }
    inline int CellR() const { return CellX() + CellW(); }
    inline int CellB() const { return CellY() + CellH(); }
    inline int Index() const { return mIndex; }
    inline void Index(int i) { mIndex = i; }

  private:
    int mX, mY, mW, mH, mCols, mRows;
    int mCellW, mCellH;
    int mCol = 0;
    int mRow = 0;
    int mIndex = 0;
};

namespace LLabel {
enum class HPosition { TOP, BOTTOM, MIDDLE, MANUAL };

IText* _font = nullptr;
HPosition _hPos = HPosition::MIDDLE;
int _hPosManual = 0;

void SetFont(IText* f) { _font = f; };
void SetHPos(HPosition hp) { _hPos = hp; }
void SetHPos(int y) {
    _hPos = HPosition::MANUAL;
    _hPosManual = y;
}

void Create(IPlugEasy* pEasy, IGraphics* pGraphics, const std::string& text, Layout* pLayout) {
    int h = _font->mSize + 2;
    int x = pLayout->CellX();
    int y = pLayout->CellY();
    switch (_hPos) {
        case HPosition::TOP:
            break;
        case HPosition::BOTTOM:
            y += pLayout->CellH() - h;
            break;
        case HPosition::MIDDLE:
            y += pLayout->CellH() / 2 - h / 2;
            break;
        case HPosition::MANUAL:
            y += _hPosManual;
            break;
    }
    pGraphics->AttachControl(
        new ITextControl(pEasy->GetIPlugBase(), IRECT(x, y, x + pLayout->CellW(), y + h), _font, text.c_str()));
    pLayout->Next();
}
}  // Label

namespace LKnob {

struct KnobDescriptor {
    int ImgId;
    char* Name;
    char* Unit;
    double Init, Min, Max, Step;
    double Shape = 1;
    LabeledKnob::LabelPosition LabelPos = LabeledKnob::LabelPosition::TOP_BOTTOM;
    bool ShowValue = true;
    std::function<void(double)> OnChange;
};

IText* _titleFont = nullptr;
IText* _valueFont = nullptr;

void SetTitleFont(IText* f) { _titleFont = f; }
void SetValueFont(IText* f) { _valueFont = f; }

int Create(IPlugEasy* pEasy, IGraphics* pGraphics, KnobDescriptor* pKnob, Layout* pLayout) {
    int index = pLayout->Index();
    auto* param = pEasy->GetIPlugBase()->GetParam(index);
    param->InitDouble(pKnob->Name, pKnob->Init, pKnob->Min, pKnob->Max, pKnob->Step, pKnob->Unit);
    param->SetShape(pKnob->Shape);

    auto* pImg = pEasy->GetImage(pKnob->ImgId);
    pGraphics->AttachControl(new LabeledKnob(
        pEasy->GetIPlugBase(), IRECT(pLayout->CellL(), pLayout->CellT(), pLayout->CellR(), pLayout->CellB()), index,
        pImg, pKnob->LabelPos, *_titleFont, *_valueFont, pKnob->Name, pKnob->Unit, pKnob->ShowValue));

    auto onChange = pKnob->OnChange;
    pEasy->AddParamChangeHandler(index, [param, onChange]() {
        double val = param->Value();
        if (onChange) {
            onChange(val);
        }
    });

    pLayout->Next();

    return index;
}

}  // LKnob

namespace LSelectBox {
struct SelectBoxItem {
    SelectBoxItem(char* n, int i = -1) : Name(n), Id(i) {}
    char* Name;
    int Id;
};

struct SelectBoxDescriptor {
    char* Label;
    int LabelWidth = 0;
    IPopupMenu* Menu;
    int NumItems = 0;
    int InitIdx = 0;
    std::unordered_map<int, int> IndexMap;
    std::function<void(int)> OnChange;
};

IText* _labelFont = nullptr;
IText* _valueFont = nullptr;
// std::unordered_map<int, IPopupMenuEx*> _menus;

void SetLabelFont(IText* f) { _labelFont = f; }
void SetValueFont(IText* f) { _valueFont = f; }

int Create(IPlugEasy* pEasy, IGraphics* pGraphics, SelectBoxDescriptor* pBox, Layout* pLayout) {
    int index = pLayout->Index();
    auto* param = pEasy->GetIPlugBase()->GetParam(index);
    param->InitEnum(pBox->Label, pBox->InitIdx, pBox->NumItems);

    //_menus[index] = pBox->Menu;

    pGraphics->AttachControl(new SelectBox(
        pEasy->GetIPlugBase(), IRECT(pLayout->CellL(), pLayout->CellT(), pLayout->CellR(), pLayout->CellB()),
        pBox->LabelWidth, index, pBox->Label, pBox->Menu, pBox->NumItems, *_labelFont, *_valueFont));

    auto sb = *pBox;
    pEasy->AddParamChangeHandler(index, [param, sb] {
        if (sb.OnChange) {
            int menuidx = param->Value();
            auto it = sb.IndexMap.find(menuidx);
            if (it != sb.IndexMap.end()) {
                sb.OnChange(it->second);
            } else {
                sb.OnChange(menuidx);
            }
        }
    });

    pLayout->Next();

    return index;
}

}  // LSelectBox

namespace LSwitch {

enum class LabelPosition { TOP_BOTTOM };

struct SwitchDescriptor {
    int ImgId;
    char* Name;
    char* OffLabel = "Off";
    char* OnLabel = "On";
    LabelPosition LabelPos = LabelPosition::TOP_BOTTOM;
    int TextWidth = 60;
    std::function<void(bool)> OnChange;
};

IText* _titleFont = nullptr;
IText* _labelFont = nullptr;

void SetTitleFont(IText* f) { _titleFont = f; }
void SetLabelFont(IText* f) { _labelFont = f; }

int Create(IPlugEasy* pEasy, IGraphics* pGraphics, SwitchDescriptor* pSwitch, Layout* pLayout) {
    int idx = pLayout->Index();
    auto* param = pEasy->GetIPlugBase()->GetParam(idx);
    param->InitBool(pSwitch->Name, false);

    auto* pImg = pEasy->GetImage(pSwitch->ImgId);

    switch (pSwitch->LabelPos) {
        case LabelPosition::TOP_BOTTOM:
            int w = pSwitch->TextWidth + pImg->W;
            int h = 2 * _labelFont->mSize + pImg->H / pImg->N - 4;
            int x = pLayout->CellX() + pLayout->CellW() / 2 - w / 2;
            int y = pLayout->CellY() + pLayout->CellH() / 2 - h / 2 + 3;
            pGraphics->AttachControl(new ITextControl(pEasy->GetIPlugBase(),
                                                      IRECT(x, y + pImg->H / pImg->N / 2 + _titleFont->mSize / 2 - 3,
                                                            x + pSwitch->TextWidth, y + _titleFont->mSize),
                                                      _titleFont, pSwitch->Name));
            pGraphics->AttachControl(new ITextControl(
                pEasy->GetIPlugBase(), IRECT(pLayout->CellX() + pImg->W, y, pLayout->CellR(), y + _labelFont->mSize),
                _labelFont, pSwitch->OffLabel));
            pGraphics->AttachControl(new ISwitchControl(pEasy->GetIPlugBase(), x + pSwitch->TextWidth,
                                                        y + _labelFont->mSize - 2, idx, pImg));
            pGraphics->AttachControl(new ITextControl(
                pEasy->GetIPlugBase(), IRECT(pLayout->CellX() + pImg->W, y + _labelFont->mSize + pImg->H / pImg->N - 4,
                                             pLayout->CellR(), y + 2 * _labelFont->mSize + pImg->H / pImg->N),
                _labelFont, pSwitch->OnLabel));
            break;
    }

    auto onChange = pSwitch->OnChange;
    pEasy->AddParamChangeHandler(idx, [param, onChange] {
        if (onChange) {
            onChange(param->Bool());
        }
    });

    pLayout->Next();

    return idx;
}
}  // LSwitch
