#ifndef __UIANIMATION_H__
#define __UIANIMATION_H__

#pragma once

#include "UIButton.h"

namespace DuiLib {

class UILIB_API IUIAnimation
{
public:
    virtual ~IUIAnimation()
    {
        NULL;
    }

    virtual bool StartAnimation(int nElapse,
                                int nTotalFrame,
                                int nAnimationID = 0,
                                bool bLoop = FALSE) = 0;
    virtual void StopAnimation(int nAnimationID = 0) = 0;
    virtual bool IsAnimationRunning(int nAnimationID) = 0;
    virtual int GetCurrentFrame(int nAnimationID = 0) = 0;
    virtual bool SetCurrentFrame(int nFrame, int nAnimationID = 0) = 0;

    virtual void
    OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) = 0;
    virtual void OnAnimationStart(int nAnimationID, bool bFirstLoop) = 0;
    virtual void OnAnimationStop(int nAnimationID) = 0;

    virtual void OnAnimationElapse(int nAnimationID) = 0;
};

class UILIB_API CAnimationData
{
public:
    CAnimationData(int nElipse, int nFrame, int nID, bool bLoop)
    {
        m_bFirstLoop = TRUE;
        m_nCurFrame = 0;
        m_nElapse = nElipse;
        m_nTotalFrame = nFrame;
        m_bLoop = bLoop;
        m_nAnimationID = nID;
    }

    // protected:
public:
    friend class CDUIAnimation;

    int m_nAnimationID;
    int m_nElapse;

    int m_nTotalFrame;
    int m_nCurFrame;

    bool m_bLoop;
    bool m_bFirstLoop;
};

class UILIB_API CUIAnimation : public IUIAnimation
{
    struct Imp;

public:
    CUIAnimation();
    ~CUIAnimation();

public:
    void Attach(CControlUI* pOwner);

    bool StartAnimation(int nElapse,
                                int nTotalFrame,
                                int nAnimationID = 0,
                                bool bLoop = FALSE) override;
    void StopAnimation(int nAnimationID = 0) override;
    bool IsAnimationRunning(int nAnimationID) override;
    int GetCurrentFrame(int nAnimationID = 0) override;
    bool SetCurrentFrame(int nFrame, int nAnimationID = 0) override;

    void OnAnimationStart(int nAnimationID, bool bFirstLoop) override;
    void OnAnimationStep(int nTotalFrame, int nCurFrame, int nAnimationID) override;
    void OnAnimationStop(int nAnimationID) override;
    void OnAnimationElapse(int nAnimationID) override;

protected:
    CAnimationData* GetAnimationDataByID(int nAnimationID);

protected:
    CControlUI* m_pControl = nullptr;
    Imp* m_pImp = nullptr;
};

} // namespace DuiLib

#endif // __UIANIMATION_H__