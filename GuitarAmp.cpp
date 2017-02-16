#include "GuitarAmp.h"
#include "IControl.h"
#include "IPlug_include_in_plug_src.h"
#include "Impulses.h"
#include "LabeledKnob.h"
#include "Layout.h"
#include "SelectBox.h"
#include "resource.h"

#include <ATK/Core/InPointerFilter.h>
#include <ATK/Core/OutPointerFilter.h>
#include <cstring>
#include <initializer_list>
#include <sndfile.hh>
#include <thread>
#include <vector>

const int kNumPrograms = 1;
const int kNumParams = 49;

enum ELayout { kWidth = GUI_WIDTH, kHeight = GUI_HEIGHT, kFirst_X = 30, kFirst_Y = 30 };

enum ImageIds {
    IMG_BG = IMG_RESSOURCE_BASE_ID,
    IMG_KNOB_GRAY,
    IMG_KNOB_GRAY_SMALL,
    IMG_KNOB_RED,
    IMG_KNOB_RED_SMALL,
    IMG_KNOB_BLUE,
    IMG_KNOB_BLUE_SMALL,
    IMG_KNOB_LIGHTBLUE,
    IMG_KNOB_LIGHTBLUE_SMALL,
    IMG_KNOB_YELLOW,
    IMG_KNOB_YELLOW_SMALL,
    IMG_KNOB_DARKYELLOW,
    IMG_KNOB_DARKYELLOW_SMALL,
    IMG_KNOB_GREEN,
    IMG_KNOB_GREEN_SMALL,
    IMG_KNOB_PURPLE,
    IMG_KNOB_PURPLE_SMALL,
    IMG_KNOB_69,
    IMG_KNOB_ORG_AMP,
    IMG_SWITCH_TOGGLE,
};

const std::initializer_list<ImgDescriptor> gImageDescriptors = {
    {.ID = IMG_BG, .FN = "resources/img/bg.png", .Frames = 0},
    //{.ID = IMG_BG, .FN = "#202020", .Frames = 0},
    {.ID = IMG_KNOB_GRAY, .FN = "resources/img/knob_3d_gray.png", .Frames = 64},
    {.ID = IMG_KNOB_GRAY_SMALL, .FN = "resources/img/knob_3d_gray_small.png", .Frames = 64},
    {.ID = IMG_KNOB_RED, .FN = "resources/img/knob_3d_red.png", .Frames = 64},
    {.ID = IMG_KNOB_RED_SMALL, .FN = "resources/img/knob_3d_red_small.png", .Frames = 64},
    {.ID = IMG_KNOB_BLUE, .FN = "resources/img/knob_3d_blue.png", .Frames = 64},
    {.ID = IMG_KNOB_BLUE_SMALL, .FN = "resources/img/knob_3d_blue_small.png", .Frames = 64},
    {.ID = IMG_KNOB_LIGHTBLUE, .FN = "resources/img/knob_3d_lightblue.png", .Frames = 64},
    {.ID = IMG_KNOB_LIGHTBLUE_SMALL, .FN = "resources/img/knob_3d_lightblue_small.png", .Frames = 64},
    {.ID = IMG_KNOB_YELLOW, .FN = "resources/img/knob_3d_yellow.png", .Frames = 64},
    {.ID = IMG_KNOB_YELLOW_SMALL, .FN = "resources/img/knob_3d_yellow_small.png", .Frames = 64},
    {.ID = IMG_KNOB_DARKYELLOW, .FN = "resources/img/knob_3d_darkyellow.png", .Frames = 64},
    {.ID = IMG_KNOB_DARKYELLOW_SMALL, .FN = "resources/img/knob_3d_darkyellow_small.png", .Frames = 64},
    {.ID = IMG_KNOB_GREEN, .FN = "resources/img/knob_3d_green.png", .Frames = 64},
    {.ID = IMG_KNOB_GREEN_SMALL, .FN = "resources/img/knob_3d_green_small.png", .Frames = 64},
    {.ID = IMG_KNOB_PURPLE, .FN = "resources/img/knob_3d_purple.png", .Frames = 64},
    {.ID = IMG_KNOB_PURPLE_SMALL, .FN = "resources/img/knob_3d_purple_small.png", .Frames = 64},
    {.ID = IMG_KNOB_69, .FN = "resources/img/knob_69_black.png", .Frames = 64},
    {.ID = IMG_KNOB_ORG_AMP, .FN = "resources/img/org_amp.png", .Frames = 64},
    {.ID = IMG_SWITCH_TOGGLE, .FN = "resources/img/switch_toggle.png", .Frames = 2},
};

GuitarAmp::GuitarAmp(IPlugInstanceInfo instanceInfo)
    : IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), IPlugEasy(this) {
    TRACE;

    IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
    LoadImages(pGraphics, gImageDescriptors);

    Layout layoutGain(kFirst_X, kFirst_Y + 172, 120, 70, 1, 1);
    Layout layoutTone(layoutGain.R() + 20, kFirst_Y + 172, 100 * 3, 70, 3, 1);
    Layout layoutOut(layoutTone.R() + 20, kFirst_Y + 172, 120, 70, 1, 1);
    Layout layoutBypass(kFirst_X + 615, kFirst_Y + 172, 90, 70, 1, 1);
    Layout layoutTubes(kFirst_X + 12, layoutOut.B() + 20, 525, 330, 5, 6);
    Layout layoutCab(5, 3, 400, 20, 1, 1);
    Layout layoutOversampling(kFirst_X + 647, 3, 90, 20, 1, 1);

    LLabel::SetFont(
        new IText(14, &COLOR_GRAY, "Courier", IText::kStyleBold, IText::kAlignCenter, 0, IText::kQualityDefault));
    LLabel::SetHPos(layoutTubes.CellH() - 25);

    LKnob::SetTitleFont(
        new IText(14, &COLOR_BLACK, "Courier", IText::kStyleBold, IText::kAlignNear, 0, IText::kQualityDefault));
    LKnob::SetValueFont(
        new IText(12, &COLOR_BLACK, "Courier", IText::kStyleNormal, IText::kAlignNear, 0, IText::kQualityDefault));

    LSelectBox::SetLabelFont(
        new IText(14, &COLOR_GRAY, "Courier", IText::kStyleBold, IText::kAlignFar, 0, IText::kQualityDefault));
    LSelectBox::SetValueFont(
        new IText(12, &COLOR_GRAY, "Courier", IText::kStyleNormal, IText::kAlignNear, 0, IText::kQualityDefault));

    LSwitch::SetTitleFont(
        new IText(14, &COLOR_BLACK, "Courier", IText::kStyleBold, IText::kAlignFar, 0, IText::kQualityDefault));
    LSwitch::SetLabelFont(
        new IText(12, &COLOR_BLACK, "Courier", IText::kStyleNormal, IText::kAlignCenter, 0, IText::kQualityDefault));

    LKnob::KnobDescriptor knob;
    knob.LabelPos = LabeledKnob::LabelPosition::TOP_BOTTOM;
    knob.ShowValue = false;
    knob.ImgId = IMG_KNOB_ORG_AMP;

    //
    // In Gain
    //
    knob.Name = "Gain";
    knob.Unit = "";
    knob.Step = 0.00001;
    knob.Shape = 10;
    knob.Init = 0.0003;
    knob.Min = 0.00005;
    knob.Max = 1;
    knob.OnChange = UPDATE_FILTER(mGain, set_volume);
    mGainID = LKnob::Create(this, pGraphics, &knob, &layoutGain);

    //
    // Tone Stack
    //
    layoutTone.Index(layoutGain.Index());
    knob.Name = "Low";
    knob.Unit = "";
    knob.Step = 0.01;
    knob.Shape = 1;
    knob.Init = 0.5;
    knob.Min = 0;
    knob.Max = 1;
    knob.OnChange = UPDATE_FILTER(mToneStack, set_low);
    mLowID = LKnob::Create(this, pGraphics, &knob, &layoutTone);

    knob.Name = "Mid";
    knob.OnChange = UPDATE_FILTER(mToneStack, set_middle);
    mMidID = LKnob::Create(this, pGraphics, &knob, &layoutTone);

    knob.Name = "High";
    knob.OnChange = UPDATE_FILTER(mToneStack, set_high);
    mHighID = LKnob::Create(this, pGraphics, &knob, &layoutTone);

    //
    // Out Gain
    //
    layoutOut.Index(layoutTone.Index());
    knob.Name = "Volume";
    knob.Unit = "";
    knob.Step = 0.001;
    knob.Shape = 10;
    knob.Init = 0.001;
    knob.Min = 0;
    knob.Max = 0.05;
    knob.OnChange = UPDATE_FILTER(mVolume, set_volume);
    mVolID = LKnob::Create(this, pGraphics, &knob, &layoutOut);

    //
    // Bypass
    //
    layoutBypass.Index(layoutOut.Index());
    LSwitch::SwitchDescriptor sw;
    sw.ImgId = IMG_SWITCH_TOGGLE;
    sw.Name = "Bypass";
    sw.OnChange = [this](bool b) {
        mBypass = b;
        if (mGain) {
            Setup();
        }
    };
    LSwitch::Create(this, pGraphics, &sw, &layoutBypass);

    if (WITH_TUBE_PARAMS) {
        // Reinit all tubes when a tube parameter changes
        auto reInitTubes = [this](double v) {
            if (mGain) {
                SetupTubes();
            }
        };

        //
        // Tube Parameters
        //
        layoutTubes.Index(layoutBypass.Index());
        layoutTubes.Next();

        LLabel::Create(this, pGraphics, "TUBE 1", &layoutTubes);
        LLabel::Create(this, pGraphics, "TUBE 2", &layoutTubes);
        LLabel::Create(this, pGraphics, "TUBE 3", &layoutTubes);
        LLabel::Create(this, pGraphics, "TUBE 4", &layoutTubes);

        TubeFilterType protoType(mTubeType);

        knob.LabelPos = LabeledKnob::LabelPosition::RIGHT;
        knob.OnChange = reInitTubes;
        knob.ShowValue = true;

        knob.ImgId = IMG_KNOB_PURPLE_SMALL;
        knob.Name = "Ri";
        knob.Unit = "Ohm";
        knob.Step = 1;
        knob.Shape = 1;
        knob.Init = protoType.Ri();
        knob.Min = 1e3;
        knob.Max = 1000e3;
        mRiID[0] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        //
        // Rg
        //
        knob.ImgId = IMG_KNOB_BLUE_SMALL;
        knob.Name = "Rg";
        knob.Unit = "Ohm";
        knob.Step = 1;
        knob.Shape = 1;
        knob.Init = protoType.Rg();
        knob.Min = 100e3;
        knob.Max = 10e6;
        mRgID[0] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_YELLOW_SMALL;
        mRgID[1] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GRAY_SMALL;
        mRgID[2] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GREEN_SMALL;
        mRgID[3] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        //
        // Vb
        //
        knob.ImgId = IMG_KNOB_PURPLE_SMALL;
        knob.Name = "Vb";
        knob.Unit = "V";
        knob.Step = 1;
        knob.Shape = 1;
        knob.Init = protoType.Vb();
        knob.Min = 150;
        knob.Max = 350;
        mVbID = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        //
        // Ra
        //
        knob.ImgId = IMG_KNOB_BLUE_SMALL;
        knob.Name = "Ra";
        knob.Unit = "Ohm";
        knob.Step = 1;
        knob.Shape = 1;
        knob.Init = protoType.Ra();
        knob.Min = 5e3;
        knob.Max = 100e3;
        mRaID[0] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_YELLOW_SMALL;
        mRaID[1] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GRAY_SMALL;
        mRaID[2] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GREEN_SMALL;
        mRaID[3] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        layoutTubes.Next();

        //
        // Ci
        //
        knob.ImgId = IMG_KNOB_BLUE_SMALL;
        knob.Name = "Ci";
        knob.Unit = "F";
        knob.Step = 1e-14;
        knob.Shape = 10;
        knob.Init = protoType.Ci();
        knob.Min = 1e-9;
        knob.Max = 100e-9;
        mCiID[0] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_YELLOW_SMALL;
        mCiID[1] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GRAY_SMALL;
        mCiID[2] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GREEN_SMALL;
        mCiID[3] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        layoutTubes.Next();

        //
        // Co
        //
        knob.ImgId = IMG_KNOB_BLUE_SMALL;
        knob.Name = "Co";
        knob.Unit = "F";
        knob.Step = 1e-14;
        knob.Shape = 10;
        knob.Init = protoType.Co();
        knob.Min = 100e-12;
        knob.Max = 1000e-9;
        mCoID[0] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_YELLOW_SMALL;
        mCoID[1] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GRAY_SMALL;
        mCoID[2] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GREEN_SMALL;
        mCoID[3] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        layoutTubes.Next();

        //
        // Vk
        //
        knob.ImgId = IMG_KNOB_BLUE_SMALL;
        knob.Name = "Vk";
        knob.Unit = "V";
        knob.Step = 1;
        knob.Shape = 1;
        knob.Init = protoType.Vk();
        knob.Min = 0;
        knob.Max = 16;
        mVkID[0] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_YELLOW_SMALL;
        mVkID[1] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GRAY_SMALL;
        mVkID[2] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);

        knob.ImgId = IMG_KNOB_GREEN_SMALL;
        mVkID[3] = LKnob::Create(this, pGraphics, &knob, &layoutTubes);
    }

    //
    // Select Boxes for Cabinet, Oversampling
    //
    layoutCab.Index(WITH_TUBE_PARAMS ? layoutTubes.Index() : layoutBypass.Index());

    LSelectBox::SelectBoxDescriptor sb;
    sb.Label = "Cabinet:";
    sb.LabelWidth = 75;
    sb.Menu = new IPopupMenu();
    sb.InitIdx = 25;
    const char* lastVendor = nullptr;
    const char* lastMic = nullptr;
    IPopupMenu* vendorMenu;
    IPopupMenu* micMenu;
    int idx = 0;
    int subidx = 0;
    for (auto& ir : gImpulses) {
        if (nullptr == lastVendor || std::strcmp(lastVendor, ir.Vendor)) {
            lastVendor = ir.Vendor;
            vendorMenu = new IPopupMenu();
            sb.Menu->AddItem(ir.Vendor, vendorMenu);
            lastMic = nullptr;
        }
        if (nullptr == lastMic || std::strcmp(lastMic, ir.Mic)) {
            lastMic = ir.Mic;
            micMenu = new IPopupMenu();
            vendorMenu->AddItem(ir.Mic, micMenu);
            subidx = 0;
        }
        micMenu->AddItem(new IPopupMenuItemEx(ir.Name, IPopupMenuItem::kNoFlags, idx));
        if (idx == sb.InitIdx) {
            micMenu->SetChosenItemIdx(subidx);
        }
        idx++;
        subidx++;
    }
    sb.Menu->AddSeparator();
    sb.Menu->AddItem(new IPopupMenuItemEx("Custom Impulse Response", IPopupMenuItem::kNoFlags, idx));
    sb.NumItems = gImpulses.size() + 1;
    sb.OnChange = [this](int v) {
        if (v == gImpulses.size()) {
            // custom response
            GetGUI()->PromptForFile(&mCustFile, EFileAction::kFileOpen, &mCustDir, "wav");
            if (nullptr != mCustFile.Get() && LoadIRFromFile(mCustFile.Get())) {
                mCab = v;
            }
        } else {
            mCab = v;
            // std::cout << "Selected " << mCab << std::endl;
        }
        if (mGain) {
            SetupCabinet();
        }
    };
    mCabID = LSelectBox::Create(this, pGraphics, &sb, &layoutCab);

    layoutOversampling.Index(layoutCab.Index());

    sb.Label = "Oversampling:";
    sb.LabelWidth = 60;
    sb.InitIdx = 0;
    sb.Menu = new IPopupMenu();
    sb.Menu->AddItem(new IPopupMenuItemEx("4x", IPopupMenuItem::kNoFlags, 0));
    sb.Menu->AddItem(new IPopupMenuItemEx("8x", IPopupMenuItem::kNoFlags, 1));
    sb.Menu->SetChosenItemIdx(sb.InitIdx);
    sb.NumItems = 2;
    sb.IndexMap[0] = 4;
    sb.IndexMap[1] = 8;
    sb.OnChange = [this](int v) {
        mOversamplingFactor = v;
        if (mGain) {
            Setup();
        }
    };
    LSelectBox::Create(this, pGraphics, &sb, &layoutOversampling);

    //
    // Meters
    //
    int meterX = kWidth - 12;
    mMeterOut = new PeakMeter(this, IRECT(meterX, 10, meterX + 8, kHeight - 7), true);
    pGraphics->AttachControl(mMeterOut);

    // IText txt = pGraphics->
    AttachGraphics(pGraphics);

    // MakePreset("preset 1", ... );
    MakeDefaultPreset((char*)"-", kNumPrograms);

    AddResetHandler([this] { Setup(); });
}

GuitarAmp::~GuitarAmp() {}

void GuitarAmp::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames) {
    // Mutex is already locked for us.
    mInputs->set_inputs(inputs, nFrames);
    mOutputs->set_outputs(outputs, nFrames);
    mOutputs->process(nFrames);
}

void GuitarAmp::Setup() {
    int channels = IsInChannelConnected(1) ? 2 : 1;
    int rate = GetSampleRate();
    int rateOver = rate * mOversamplingFactor;

    // if (IR_SAMPLERATE != rate) {
    //    mResampler.reset(new r8b::CDSPResampler16IR(IR_SAMPLERATE, rate, RESAMPLE_BLOCK_SIZE));
    //}

    mInputs.reset(new InType(channels));
    mInputs->set_output_sampling_rate(rate);

    if (!mBypass) {
        switch (mOversamplingFactor) {
            case 2:
                mOversample.reset(new OversamplingType_2(1));
                break;
            case 4:
                mOversample.reset(new OversamplingType_4(1));
                break;
            case 8:
                mOversample.reset(new OversamplingType_8(1));
                break;
        }
        mOversample->set_input_sampling_rate(rate);
        mOversample->set_output_sampling_rate(rateOver);

        if (channels > 1) {
            mSum.reset(new SumType());
            mSum->set_input_sampling_rate(rate);
            mSum->set_input_port(0, mInputs.get(), 0);
            mSum->set_input_port(1, mInputs.get(), 1);
            mOversample->set_input_port(0, mSum.get(), 0);
        } else {
            mOversample->set_input_port(0, mInputs.get(), 0);
        }

        mLowPass.reset(new LowPassType(1));
        mLowPass->set_cut_frequency(rate);
        mLowPass->set_order(5);
        mLowPass->set_input_sampling_rate(rateOver);

        mDownsample.reset(new DownsampingType(1));
        mDownsample->set_input_sampling_rate(rateOver);
        mDownsample->set_output_sampling_rate(rate);
        mDownsample->set_input_port(0, mLowPass.get(), 0);

        mToneStack.reset(new ToneStackType(ToneStackType::buildJCM800Stack()));
        mToneStack->set_input_sampling_rate(rate);
        mToneStack->set_low(GetParam(mLowID)->Value());
        mToneStack->set_middle(GetParam(mMidID)->Value());
        mToneStack->set_high(GetParam(mHighID)->Value());
        mToneStack->set_input_port(0, mDownsample.get(), 0);

        mVolume.reset(new VolumeType(1));
        mVolume->set_input_sampling_rate(rate);
        mVolume->set_volume(GetParam(mVolID)->Value());
    }

    mPeakOut.reset(new PeakFilterType(1, mMeterOut));
    mPeakOut->set_input_sampling_rate(rate);
    if (!mBypass) {
        mPeakOut->set_input_port(0, mVolume.get(), 0);
    } else {
        mPeakOut->set_input_port(0, mInputs.get(), 0);
    }

    mOutputs.reset(new OutType(channels));
    mOutputs->set_input_sampling_rate(rate);
    mOutputs->set_input_port(0, mPeakOut.get(), 0);
    if (channels > 1) {
        mOutputs->set_input_port(1, mPeakOut.get(), 0);
    }

    if (!mBypass) {
        SetupTubes();

        // Add Gain control between the first and second tube stages
        mGain.reset(new VolumeType(1));
        mGain->set_input_sampling_rate(rateOver);
        mGain->set_volume(GetParam(mGainID)->Value());
        mGain->set_input_port(0, mTube[0].get(), 0);
        mTube[1]->set_input_port(0, mGain.get(), 0);

        SetupCabinet();
    }
}

void GuitarAmp::SetupTubes(bool useTubeParameters) {
    int rate = GetSampleRate();
    int rateOver = rate * mOversamplingFactor;
    for (int i = 0; i < NUM_TUBES; ++i) {
        mTube[i].reset(new TubeFilterType(mTubeType));
        if (WITH_TUBE_PARAMS) {
            if (useTubeParameters) {
                // Reset the UI parameters with the tube default parameters
                if (i == 0) {  // Ri gets calculated for follower stages, so only the first circuit has this param
                    SetUIParam(mRiID[i], mTube[i]->Ri());
                    SetUIParam(mVbID, mTube[i]->Vb());
                }
                SetUIParam(mRgID[i], mTube[i]->Rg());
                SetUIParam(mRaID[i], mTube[i]->Ra());
                SetUIParam(mCiID[i], mTube[i]->Ci());
                SetUIParam(mCoID[i], mTube[i]->Co());
                SetUIParam(mVkID[i], mTube[i]->Vk());
            } else {
                // Initialize the tube with the current parameter values of the UI
                if (i == 0) {  // Ri gets calculated for follower stages, so only the first circuit has this param
                    mTube[i]->Ri(GetParam(mRiID[i])->Value());
                }
                mTube[i]->Rg(GetParam(mRgID[i])->Value());
                mTube[i]->Ra(GetParam(mRaID[i])->Value());
                mTube[i]->Ci(GetParam(mCiID[i])->Value());
                mTube[i]->Co(GetParam(mCoID[i])->Value());
                mTube[i]->Vk(GetParam(mVkID[i])->Value());
                mTube[i]->Vb(GetParam(mVbID)->Value());
            }
        }
        mTube[i]->set_input_sampling_rate(rateOver);
        if (i > 1) {
            // We connect the gain stage between tube 1 and 2, so we skip this here
            mTube[i]->connect_stage(mTube[i - 1].get());
        }
    }
    mTube[0]->set_input_port(0, mOversample.get(), 0);
    mLowPass->set_input_port(0, mTube[NUM_TUBES - 1].get(), 0);
}

void GuitarAmp::SetupCabinet() {
    int rate = GetSampleRate();
    mCabinet.reset(new CabType());
    mCabinet->set_split_size(CONVO_SPLIT_SIZE);
    mCabinet->set_input_sampling_rate(rate);
    Impulse& impulse(mCab < gImpulses.size() ? gImpulses[mCab] : *mCustIR);
    if (impulse.SampleRate != rate) {
        std::vector<double> ir;
        Resample(impulse.Frames, ir, impulse.SampleRate, rate);
        mCabinet->set_impulse(std::move(ir));
    } else {
        std::vector<double> ir(impulse.Frames);
        mCabinet->set_impulse(std::move(ir));
    }
    mCabinet->set_input_port(0, mToneStack.get(), 0);
    mVolume->set_input_port(0, mCabinet.get(), 0);
}

bool GuitarAmp::LoadIRFromFile(const char* file) {
    SndfileHandle sndfile(file);

    if (sndfile.error()) {
        std::string error = sndfile.strError();
        std::thread([this, error] { GetGUI()->ShowMessageBox(error.c_str(), "Failed", MB_OKCANCEL); }).detach();
        return false;
    }
    if (sndfile.channels() > 1) {
        std::thread([this] {
            GetGUI()->ShowMessageBox("File contains more than one channel.", "Failed", MB_OKCANCEL);
        }).detach();
        return false;
    }

    mCustIR.reset(new Impulse());
    mCustIR->Name = mCustIR->Vendor = mCustIR->Mic = "Custom";
    mCustIR->SampleRate = sndfile.samplerate();
    int toread = sndfile.frames();
    mCustIR->Frames.resize(toread);
    do {
        toread -= sndfile.read(mCustIR->Frames.data() + (mCustIR->Frames.size() - toread), sndfile.frames());
    } while (toread > 0);

    return true;
}

void GuitarAmp::Resample(std::vector<double>& in, std::vector<double>& out, int rateSrc, int rateDst) {
    r8b::CDSPResampler16IR resampler(rateSrc, rateDst, RESAMPLE_BLOCK_SIZE);
    out.resize(int(double(rateDst) / rateSrc * double(in.size()) + 0.5), 0);
    double ratio = double(rateSrc) / rateDst;
    double buf[RESAMPLE_BLOCK_SIZE];
    auto to_read = in.size();
    auto to_write = out.size();
    double* p_in = in.data();
    double* p_out = out.data();
    while (to_write > 0) {
        // fill resample buffer
        int n = IPMIN(to_read, RESAMPLE_BLOCK_SIZE);
        std::memcpy(buf, p_in, n * sizeof(double));
        if (n < RESAMPLE_BLOCK_SIZE) {
            std::memset(&buf[n], 0, (RESAMPLE_BLOCK_SIZE - n) * sizeof(double));
        }
        p_in += n;
        to_read -= n;
        // write to output buffer
        double* p;
        n = resampler.process(buf, RESAMPLE_BLOCK_SIZE, p);
        if (n > to_write) {
            n = to_write;
        }
        for (int i = 0; i < n; ++i) {
            *p_out++ = ratio * *p++;
        }
        to_write -= n;
    }
}
