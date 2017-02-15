//
//  IPlugHandler.h
//  EQ3
//
//  Created by Andreas Pohl on 21/07/16.
//
//

#ifndef IPlugEasy_h
#define IPlugEasy_h

#include <cstdlib>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "IControl.h"
#include "IPlugBase.h"

struct ImgDescriptor {
    int ID;
    char* FN;
    int Frames;
};

#define IMG_RESSOURCE_BASE_ID 101

#define IPLUGEASY_DECL()                       \
    void Reset() {                             \
        TRACE;                                 \
        IPlugBase::IMutexLock lock(mPlugBase); \
        RunResetHandlers();                    \
    }                                          \
    void OnParamChange(int paramIdx) {         \
        IMutexLock lock(this);                 \
        RunParamChangeHandler(paramIdx);       \
    }

class IPlugEasy {
  public:
    explicit IPlugEasy(IPlugBase* base) : mPlugBase(base) {}

    void AddParamChangeHandler(int paramIdx, std::function<void()> handler) { mParamHandlers[paramIdx] = handler; }
    void AddResetHandler(std::function<void()> handler) { mResetHandlers.push_back(handler); }

    void RunParamChangeHandler(int paramIdx) {
        auto it = mParamHandlers.find(paramIdx);
        if (mParamHandlers.end() != it) {
            (it->second)();
        }
    }

    void RunParamChangeHandlers() {
        for (auto& pair : mParamHandlers) {
            (pair.second)();
        }
    }

    void RunResetHandlers() {
        for (auto& handler : mResetHandlers) {
            handler();
        }
    }

    IPlugBase* GetIPlugBase() { return mPlugBase; }

    IBitmap* GetImage(int id) {
        auto it = mImages.find(id);
        if (mImages.end() != it) {
            return &it->second;
        } else {
            return nullptr;
        }
    }

  protected:
    IPlugBase* mPlugBase;
    std::unordered_map<int, std::function<void()>> mParamHandlers;
    std::vector<std::function<void()>> mResetHandlers;
    std::unordered_map<int, IBitmap> mImages;

    void LoadImages(IGraphics* pGraphics, const std::initializer_list<ImgDescriptor>& imageDescriptors) {
        for (auto& imgDesc : imageDescriptors) {
            if (imgDesc.ID == IMG_RESSOURCE_BASE_ID) {
                if (*imgDesc.FN == '#') {
                    int rgb = std::strtol(imgDesc.FN + 1, nullptr, 16);
                    pGraphics->AttachPanelBackground(
                        new IColor(0xff, (rgb & 0xff0000) >> 16, (rgb & 0xff00) >> 8, rgb & 0xff));
                } else {
                    pGraphics->AttachBackground(imgDesc.ID, imgDesc.FN);
                }
            } else {
                mImages[imgDesc.ID] = pGraphics->LoadIBitmap(imgDesc.ID, imgDesc.FN, imgDesc.Frames);
            }
        }
    }
};

#endif /* IPlugEasy_h */
