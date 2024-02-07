#ifndef GifAnimUI_h__
#define GifAnimUI_h__

#pragma once

namespace DuiLib {
enum
{
    EVENT_TIME_ID = 100,
};

class UILIB_API CGifAnimUI : public CControlUI
{
    DECLARE_DUICONTROL(CGifAnimUI)
public:
    CGifAnimUI();
    ~CGifAnimUI();

    LPCTSTR GetClass() const override;
    LPVOID GetInterface(LPCTSTR pstrName) override;
    void DoInit() override;
    bool DoPaint(HDC hDC, const RECT& rcPaint, CControlUI* pStopControl) override;
    void DoEvent(TEventUI& event) override;
    void SetVisible(bool bVisible = true) override;
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue) override;

    void SetBkImage(LPCTSTR pStrImage);
    LPCTSTR GetBkImage();

    void SetAutoPlay(bool bIsAuto = true);
    bool IsAutoPlay() const;
    void SetAutoSize(bool bIsAuto = true);
    bool IsAutoSize() const;
    void PlayGif();
    void PauseGif();
    void StopGif();

    void OnTimer(UINT_PTR idEvent);

private:
    void InitGifImage();
    void DeleteGif();
    void DrawFrame(HDC hDC); // ����GIFÿ֡

private:
    Gdiplus::Image* m_pGifImage = nullptr;

    // ֡��֮֡����ʱ��
    Gdiplus::PropertyItem* m_pPropertyItem = nullptr; 

    UINT m_nFrameCount = 0;    // gifͼƬ��֡��
    UINT m_nFramePosition = 0; // ��ǰ�ŵ��ڼ�֡
    bool m_bIsAutoPlay = true;  // �Ƿ��Զ�����gif
    bool m_bIsAutoSize = false; // �Ƿ��Զ�����ͼƬ���ô�С
    bool m_bIsPlaying = false;
};
} // namespace DuiLib

#endif // GifAnimUI_h__
