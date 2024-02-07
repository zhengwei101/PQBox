#include "stdafx.h"
#include "UILoading.h"
#include <Gdiplus.h>

using namespace DuiLib;

namespace {

    void DrawLine(Gdiplus::Graphics* _objGraphics,
        Gdiplus::PointF _objPointOne,
        Gdiplus::PointF _objPointTwo,
        Gdiplus::Color _objColor,
        int _intLineThickness)
    {
        Gdiplus::SolidBrush brush(_objColor);
        Gdiplus::Pen objPen(&brush, Gdiplus::REAL(_intLineThickness));
        {
            objPen.SetStartCap(Gdiplus::LineCap::LineCapRound);
            objPen.SetEndCap(Gdiplus::LineCap::LineCapRound);
            _objGraphics->DrawLine(&objPen, _objPointOne, _objPointTwo);
        }
    }

Gdiplus::PointF GetCoordinate(Gdiplus::PointF _objCircleCenter, int _intRadius, double _dblAngle)
{
    double dblAngle = 3.14 * _dblAngle / 180;
    Gdiplus::PointF pf(_objCircleCenter.X + Gdiplus::REAL(_intRadius * cos(dblAngle)),
        _objCircleCenter.Y + Gdiplus::REAL(_intRadius * sin(dblAngle)));
    return pf;
}

Gdiplus::Color Darken(Gdiplus::Color _objColor, int _intPercent)
{
    int intRed = _objColor.GetR();
    int intGreen = _objColor.GetG();
    int intBlue = _objColor.GetB();
    Gdiplus::Color color((BYTE)_intPercent, (BYTE)min(intRed, 255), (BYTE)min(intGreen, 255), (BYTE)min(intBlue, 255));
    return color;
}

double* GetSpokesAngles(int _intNumberSpoke)
{
    double* Angles = new double[_intNumberSpoke];
    double dblAngle = (double)360 / _intNumberSpoke;
    for (int shtCounter = 0; shtCounter < _intNumberSpoke; shtCounter++)
    {
        Angles[shtCounter] =
            (shtCounter == 0 ? dblAngle : Angles[shtCounter - 1] + dblAngle);
    }
    return Angles;
}
} // namespace

IMPLEMENT_DUICONTROL(CLoadingUI)

CLoadingUI::CLoadingUI()
    : m_bStop(true)
    , m_nTime(100)
    , m_nNumber(0)
    , m_Angles(nullptr)
    , m_Colors(nullptr)
    , m_NumberOfSpoke(10)
    , m_SpokeThickness(4)
    , m_OuterCircleRadius(10)
    , m_InnerCircleRadius(8)
{
}

CLoadingUI::~CLoadingUI()
{
    Stop();

    if (m_Angles)
        delete m_Angles;
    if (m_Colors)
        delete m_Colors;
}

LPCTSTR CLoadingUI::GetClass() const
{
    return DUI_CTR_LOADINGCIRCLE;
}

LPVOID CLoadingUI::GetInterface(LPCTSTR pstrName)
{
    if (_tcscmp(pstrName, DUI_CTR_LOADINGCIRCLE) == 0)
        return static_cast<CLoadingUI*>(this);
    return CControlUI::GetInterface(pstrName);
}

void CLoadingUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if (_tcsicmp(pstrName, _T("style")) == 0)
    {
        LPTSTR pstr = NULL;
        m_nNumber = _tcstol(pstrValue, &pstr, 10);
    }
    else if (_tcsicmp(pstrName, _T("time")) == 0)
    {
        LPTSTR pstr = NULL;
        m_nTime = _tcstol(pstrValue, &pstr, 10);
    }
    else if (_tcsicmp(pstrName, _T("spoke")) == 0)
    {
        LPTSTR pstr = NULL;
        m_NumberOfSpoke = _tcstol(pstrValue, &pstr, 0);
    }
    else if (_tcsicmp(pstrName, _T("thickness")) == 0)
    {
        LPTSTR pstr = NULL;
        m_SpokeThickness = _tcstol(pstrValue, &pstr, 0);
    }
    else if (_tcsicmp(pstrName, _T("outradius")) == 0)
    {
        LPTSTR pstr = NULL;
        m_OuterCircleRadius = _tcstol(pstrValue, &pstr, 0);
    }
    else if (_tcsicmp(pstrName, _T("innerradius")) == 0)
    {
        LPTSTR pstr = NULL;
        m_InnerCircleRadius = _tcstol(pstrValue, &pstr, 0);
    }
    else if (_tcsicmp(pstrName, _T("color")) == 0)
    {
        while (*pstrValue > _T('\0') && *pstrValue <= _T(' '))
            pstrValue = ::CharNext(pstrValue);
        if (*pstrValue == _T('#'))
            pstrValue = ::CharNext(pstrValue);
        LPTSTR pstr = NULL;
        DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
        m_Color.SetValue(clrColor);
    }
    else
    {
        __super::SetAttribute(pstrName, pstrValue);
    }
}

void CLoadingUI::PaintBkImage(HDC hDC)
{
    m_CenterPoint.X = Gdiplus::REAL(this->GetWidth() / 2);
    m_CenterPoint.Y = Gdiplus::REAL(this->GetHeight() / 2);
    if (m_NumberOfSpoke > 0)
    {
        Gdiplus::Image* img = new Gdiplus::Bitmap(this->GetWidth(), this->GetHeight());
        Gdiplus::Graphics g(img);
        g.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);

        int intPosition = m_ProgressValue;
        for (int intCounter = 0; intCounter < m_NumberOfSpoke; intCounter++)
        {
            intPosition = intPosition % m_NumberOfSpoke;
            DrawLine(
                &g,
                GetCoordinate(
                    m_CenterPoint, m_InnerCircleRadius, m_Angles[intPosition]),
                GetCoordinate(
                    m_CenterPoint, m_OuterCircleRadius, m_Angles[intPosition]),
                m_Colors[intCounter],
                m_SpokeThickness);
            intPosition++;
        }
        // ��ô��ڵ�Graphics����
        Gdiplus::Graphics gh(hDC);
        // ���軭�õ�CacheImage����������
        gh.DrawImage(img, Gdiplus::REAL(m_rcItem.left), Gdiplus::REAL(m_rcItem.top));
        delete img;
    }
}

Gdiplus::Color* CLoadingUI::GenerateColorsPallet(Gdiplus::Color _objColor,
                                        bool _blnShadeColor,
                                        int _intNbSpoke)
{
    Gdiplus::Color* objColors = new Gdiplus::Color[m_NumberOfSpoke];
    byte bytIncrement = (byte)(255 / m_NumberOfSpoke);
    byte PERCENTAGE_OF_DARKEN = 0;
    for (int intCursor = 0; intCursor < m_NumberOfSpoke; intCursor++)
    {
        if (_blnShadeColor)
        {
            if (intCursor == 0 || intCursor < m_NumberOfSpoke - _intNbSpoke)
            {
                objColors[intCursor] = _objColor;
            }
            else
            {
                PERCENTAGE_OF_DARKEN += bytIncrement;
                if (PERCENTAGE_OF_DARKEN > 255)
                {
                    PERCENTAGE_OF_DARKEN = 255;
                }
                objColors[intCursor] = Darken(_objColor, PERCENTAGE_OF_DARKEN);
            }
        }
        else
        {
            objColors[intCursor] = _objColor;
        }
    }
    return objColors;
}

void CLoadingUI::Start()
{
    if (m_nTime > 0 && m_pManager && m_bStop == true)
    {
        m_pManager->SetTimer(this, kTimerLoadingId, m_nTime);
    }
    m_bStop = false;
}

void CLoadingUI::Stop()
{
    m_bStop = true;
    if (m_pManager)
    {
        m_pManager->KillTimer(this, kTimerLoadingId);
    }
}

void CLoadingUI::Init()
{
    CControlUI::Init();
    m_Angles = GetSpokesAngles(m_NumberOfSpoke);
    m_Colors = GenerateColorsPallet(m_Color, true, m_NumberOfSpoke);
    Start();
}

void DuiLib::CLoadingUI::DoEvent(TEventUI& event)
{
    if (event.Type == UIEVENT_TIMER && event.wParam == kTimerLoadingId)
    {
        m_ProgressValue = ++m_ProgressValue % m_NumberOfSpoke;
        Invalidate();
    }
    else
    {
        CControlUI::DoEvent(event);
    }
}

CControlUI* CreateLoadingControl(LPCTSTR pstrType)
{
    if (_tcsicmp(pstrType, DUI_CTR_LOADINGCIRCLE) == 0)
    {
        return new CLoadingUI();
    }
    return nullptr;
}
