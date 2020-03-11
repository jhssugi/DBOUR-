// XTPColorPageCustom.h : header file
//
// This file is a part of the XTREME CONTROLS MFC class library.
// (c)1998-2011 Codejock Software, All Rights Reserved.
//
// THIS SOURCE FILE IS THE PROPERTY OF CODEJOCK SOFTWARE AND IS NOT TO BE
// RE-DISTRIBUTED BY ANY MEANS WHATSOEVER WITHOUT THE EXPRESSED WRITTEN
// CONSENT OF CODEJOCK SOFTWARE.
//
// THIS SOURCE CODE CAN ONLY BE USED UNDER THE TERMS AND CONDITIONS OUTLINED
// IN THE XTREME TOOLKIT PRO LICENSE AGREEMENT. CODEJOCK SOFTWARE GRANTS TO
// YOU (ONE SOFTWARE DEVELOPER) THE LIMITED RIGHT TO USE THIS SOFTWARE ON A
// SINGLE COMPUTER.
//
// CONTACT INFORMATION:
// support@codejock.com
// http://www.codejock.com
//
/////////////////////////////////////////////////////////////////////////////

//{{AFX_CODEJOCK_PRIVATE
#if !defined(__XTPCOLORCUSTOM_H__)
#define __XTPCOLORCUSTOM_H__
//}}AFX_CODEJOCK_PRIVATE

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CXTPColorDialog;

//===========================================================================
// Summary:
//     CXTPColorBase is a CStatic derived base class. It is used to create
//     custom color selection windows.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorBase : public CStatic
{
public:
	// ----------------------------------------------------------------------
	// Summary:
	//     Enumeration used to determine window focus.
	// Remarks:
	//     FocusControl type defines the constants used by CXTPColorBase for
	//     determining which area of the color window control has focus.
	// See Also:
	//     CXTPColorBase
	//
	// <KEYWORDS focusNone, focusColorWheel, focusLumination>
	// ----------------------------------------------------------------------
	enum FocusedControl
	{
		focusNone,       // Color window does not have input focus.
		focusColorWheel, // Color wheel window has focus.
		focusLumination  // Lumination window has focus.
	};

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPColorBase object
	//-----------------------------------------------------------------------
	CXTPColorBase();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPColorBase object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPColorBase();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to update the cursor position.
	// Parameters:
	//     point   - Current cursor location relative to the device context.
	//-----------------------------------------------------------------------
	virtual void UpdateCursorPos(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to convert an RGB color value to
	//     an HSL value.
	// Parameters:
	//     color - An RGB value that represents the value to convert to HSL.
	//     h     - Represents the color hue.
	//     s     - Represents the color saturation.
	//     l     - Represents the color illumination.
	//-----------------------------------------------------------------------
	static void RGBtoHSL(COLORREF color, double *h, double *s, double* l);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to convert an HLS value to an RGB
	//     color value.
	// Parameters:
	//     h - Represents the color hue.
	//     l - Represents the color illumination.
	//     s - Represents the color saturation.
	// Returns:
	//     A COLORREF value.
	//-----------------------------------------------------------------------
	static COLORREF HLStoRGB(double h, double l, double s);

protected:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called by the CXTPColorBase class to
	//     perform initialization when the window is created or sub-classed.
	// Returns:
	//     TRUE if the window was successfully initialized, otherwise FALSE.
	//-----------------------------------------------------------------------
	virtual bool Init();

//{{AFX_CODEJOCK_PRIVATE

	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorBase)
	virtual void PreSubclassWindow();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorBase)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	static
	FocusedControl m_eHasFocus;        // Determines which property page has input focus.
	bool           m_bPreSubclassInit; // true when initializing from PreSubclassWindow.

public:
	double m_nLum;       // Current illumination value.
	double m_nSat;       // Current saturation value.
	double m_nHue;       // Current hue value.
	CPoint m_ptMousePos; // Current mouse position relative to the device context.
};

//===========================================================================
// Summary:
//     CXTPColorWnd is a CXTPColorBase derived class. It is used to create a
//     color selection window.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorWnd : public CXTPColorBase
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPColorWnd object
	//-----------------------------------------------------------------------
	CXTPColorWnd();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPColorWnd object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPColorWnd();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the color for the selection window.
	// Parameters:
	//     nHue     - Current hue value that represents the color.
	//     nSat     - Current saturation value that represents the color.
	//-----------------------------------------------------------------------
	virtual void SetColor(double nHue, double nSat);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to update the cursor position.
	// Parameters:
	//     point   - Current cursor location relative to the device context.
	//-----------------------------------------------------------------------
	virtual void UpdateCursorPos(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to update the cross hair cursor.
	// Parameters:
	//     pDC - A CDC pointer that represents the current device context.
	//-----------------------------------------------------------------------
	void DrawCrossHair(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the hue for the color
	//     window.
	// Parameters:
	//     h - Represents the new hue value to set.
	//-----------------------------------------------------------------------
	void SetHue(double h);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the saturation for the
	//     color window.
	// Parameters:
	//     s - Represents the new saturation value to set.
	//-----------------------------------------------------------------------
	void SetSaturation(double s);

protected:
//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorWnd)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorWnd)
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
	CBitmap        m_bmpPicker;         // Background device context.
};

//===========================================================================
// Summary:
//     CXTPColorLum is a CXTPColorBase derived class. It is used to create a
//     color luminance selection bar.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorLum : public CXTPColorBase
{
public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPColorLum object
	//-----------------------------------------------------------------------
	CXTPColorLum();

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPColorLum object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPColorLum();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to set the color for the selection window.
	// Parameters:
	//     nHue     - Current hue value that represents the color.
	//     nSat     - Current saturation value that represents the color.
	//-----------------------------------------------------------------------
	virtual void SetColor(double nHue, double nSat);

	//-----------------------------------------------------------------------
	// Summary:
	//     Call this member function to update the cursor position.
	// Parameters:
	//     point   - Current cursor location relative to the device context.
	//-----------------------------------------------------------------------
	virtual void UpdateCursorPos(CPoint point);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw the indicator arrow.
	// Parameters:
	//     pDC - A CDC pointer that represents the current device context.
	//-----------------------------------------------------------------------
	void DrawSliderArrow(CDC* pDC);

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to draw the selection bar.
	// Parameters:
	//     pDC - A CDC pointer that represents the current device context.
	//-----------------------------------------------------------------------
	void DrawLuminanceBar(CDC* pDC);

	// ---------------------------------------------------------------------
	// Summary:
	//     This member function is called to set the luminance for the color
	//     window.
	// Parameters:
	//     l -  Represents the new luminance value to set.
	// ---------------------------------------------------------------------
	void SetLuminance(double l);

	// ----------------------------------------------------------------------
	// Summary:
	//     This member function is called to retrieve the display size of the
	//     luminance bar.
	// Parameters:
	//     rect -  Address to a CRect object.
	// ----------------------------------------------------------------------
	void GetLumBarRect(CRect& rect);

protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorLum)
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorLum)
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
};


//===========================================================================
// Summary:
//     CXTPColorPageCustom is derived from CPropertyPage. It is used
//     to create a CXTPColorPageCustom dialog.
//===========================================================================
class _XTP_EXT_CLASS CXTPColorPageCustom : public CPropertyPage
{
public:
	//-----------------------------------------------------------------------
	// Summary:
	//     Constructs a CXTPColorPageCustom object
	// Parameters:
	//     pParentSheet - A pointer to the parent sheet.
	//-----------------------------------------------------------------------
	CXTPColorPageCustom(CXTPColorDialog* pParentSheet = NULL);

	//-----------------------------------------------------------------------
	// Summary:
	//     Destroys a CXTPColorPageCustom object, handles cleanup and deallocation
	//-----------------------------------------------------------------------
	virtual ~CXTPColorPageCustom();

public:

	//-----------------------------------------------------------------------
	// Summary:
	//     This member function is called to retrieve the HSL values of the RGB
	//     specified by color.
	// Parameters:
	//     color - An RGB value that represents the color.
	//     xtFocusLumination   - Address of the integer to receive the illumination value (0-255).
	//     sat   - Address of the integer to receive the saturation value (0-255).
	//     hue   - Address of the integer to receive the hue value (0-255).
	//-----------------------------------------------------------------------
	void RGBtoHSL(COLORREF color, int* xtFocusLumination, int* sat, int* hue);

protected:

//{{AFX_CODEJOCK_PRIVATE
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CXTPColorPageCustom)
	virtual BOOL OnSetActive();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CXTPColorPageCustom)
	void UpdateRGB(COLORREF clr);
	afx_msg void OnChangeEdit();
	afx_msg void OnChangeEditLum();
	afx_msg void OnChangeEditHue();
	afx_msg void OnChangeEditSat();
	afx_msg LRESULT OnUpdateColor(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnColorDblClick(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
//}}AFX_CODEJOCK_PRIVATE

protected:
//{{AFX_CODEJOCK_PRIVATE
	//{{AFX_DATA(CXTPColorPageCustom)
	enum { IDD = XTP_IDD_COLORCUSTOM };
	CXTPColorWnd m_colorWnd;
	CXTPColorLum m_colorLum;
	CStatic m_txtSat;
	CStatic m_txtRed;
	CStatic m_txtLum;
	CStatic m_txtHue;
	CStatic m_txtGreen;
	CStatic m_txtBlue;
	CSpinButtonCtrl m_spinSat;
	CSpinButtonCtrl m_spinRed;
	CSpinButtonCtrl m_spinLum;
	CSpinButtonCtrl m_spinHue;
	CSpinButtonCtrl m_spinGreen;
	CSpinButtonCtrl m_spinBlue;
	CEdit m_editHue;
	CEdit m_editGreen;
	CEdit m_editBlue;
	CEdit m_editLum;
	CEdit m_editRed;
	CEdit m_editSat;
	int     m_nR;
	int     m_nB;
	int     m_nG;
	int     m_nH;
	int     m_nL;
	int     m_nS;
	//}}AFX_DATA
//}}AFX_CODEJOCK_PRIVATE

protected:
	CXTPColorDialog*     m_pParentSheet; // Points to the parent property sheet


	friend class CXTPColorLum;
};

#endif // !defined(__XTPCOLORCUSTOM_H__)
