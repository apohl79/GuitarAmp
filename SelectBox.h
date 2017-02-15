//
//  SelectBox.h
//  GuitarAmp
//
//  Created by Andreas Pohl on 10/10/16.
//
//

#ifndef SelectBox_h
#define SelectBox_h

#include "IControl.h"

#include <string>

class IPopupMenuItemEx : public IPopupMenuItem {
  public:
    IPopupMenuItemEx(const char* text, IPopupMenu* pSubMenu) : IPopupMenuItem(text, pSubMenu), mIndex(-1) {}
    IPopupMenuItemEx(const char* text, int flags, int index) : IPopupMenuItem(text, flags), mIndex(index) {}

    int GetIndex() const { return mIndex; }

  protected:
    int mIndex;
};

class SelectBox : public IControl {
  public:
    SelectBox(IPlugBase* pPlug, IRECT pR, int labelWidth, int paramIdx, const char* label, IPopupMenu* menu, int size,
              IText& textLabel, IText& textValue)
        : IControl(pPlug, IRECT(pR.L + labelWidth + 6, pR.T + 4, pR.R - 4, pR.B - 4), paramIdx),
          mR(pR),
          mLabelWidth(labelWidth),
          mLabel(label),
          mMenu(menu),
          mTextLabel(textLabel),
          mTextValue(textValue),
          mColorBg(150, 0, 0, 0),
          mColorBorder(150, 50, 50, 50) {
        mDisablePrompt = false;
        CheckItems(mMenu);
    }

    virtual bool Draw(IGraphics* pGraphics) {
        auto r = IRECT(mR.L, mR.T + 3, mR.L + (mLabelWidth - 4), mR.B);
        pGraphics->DrawIText(&mTextLabel, const_cast<char*>(mLabel), &r);
        r = IRECT(mR.L + mLabelWidth, mR.T, mR.R, mR.B);
        pGraphics->FillIRect(&mColorBorder, &r);
        r = IRECT(mR.L + mLabelWidth + 1, mR.T + 1, mR.R - 1, mR.B - 1);
        pGraphics->FillIRect(&mColorBg, &r);
        pGraphics->DrawIText(&mTextValue, const_cast<char*>(mText.c_str()), &mRECT);
        return true;
    }

    void OnMouseDown(int x, int y, IMouseMod* pMod) {
        if (pMod->L || pMod->R) {
            ShowPopUpMenu();
        }
    }

    void OnMouseDblClick(int x, int y, IMouseMod* pMod) { ShowPopUpMenu(); }

    void ShowPopUpMenu() {
        if (mParamIdx >= 0 && !mDisablePrompt) {
            if (mPlug->GetGUI()->CreateIPopupMenu(mMenu, &mRECT)) {
                CheckItems(mMenu);
            }
            Redraw();
        }
    }

    void SetValueFromUserInput(double value) {
        mValue = -1;  // Make sure we always trigger the chnaged callback, even if the same item gets selected
        IControl::SetValueFromUserInput(value);
    }

  private:
    IRECT mR;
    int mLabelWidth;
    const char* mLabel;
    IPopupMenu* mMenu;
    IText mTextLabel;
    IText mTextValue;
    std::string mText;
    IColor mColorBg, mColorBorder;
    // int mSelectedIndex;

    /// Find the selected item in the menu/submenus, check it, set the value text and uncheck the rest.
    /// Returns the index value of the selected item.
    int CheckItems(IPopupMenu* m) {
        int idx = m->GetChosenItemIdx();
        if (idx > -1 && idx < m->GetNItems()) {
            m->CheckItemAlone(idx);
            m->SetChosenItemIdx(-1);
            auto* item = static_cast<IPopupMenuItemEx*>(m->GetItem(idx));
            mText = item->GetText();
            int index = item->GetIndex();
            SetValueFromUserInput(GetParam()->GetNormalized((double)index));
            return index;
        } else {
            int n = m->GetNItems();
            int index = -1;
            for (int i = 0; i < n; ++i) {
                auto* submenu = m->GetItem(i)->GetSubmenu();
                if (nullptr != submenu) {
                    int subindex = CheckItems(submenu);
                    if (subindex > -1) {
                        auto oldVal = mText;
                        mText = m->GetItem(i)->GetText();
                        mText += " > ";
                        mText += oldVal;
                        index = subindex;
                    }
                } else {
                    m->CheckItem(i, false);
                }
            }
            return index;
        }
    }
};

#endif /* SelectBox_h */
