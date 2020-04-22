/* --------------------------------------------------------------------------------

	XingerProgress.h : header file
	���ߣ�������
	ʱ�䣺2003��6��15�� �ڱ�����ƽ

 --------------------------------------------------------------------------------*/



#ifndef __MY_PROGRESS_HEAD__
#define __MY_PROGRESS_HEAD__



#if _MSC_VER > 1000
#pragma once
#endif





class XingerProgress : public CProgressCtrl
{

public:
	XingerProgress();
	virtual ~XingerProgress();

public:
	BOOL m_bShowTitle;		//	�Ƿ���ʾ����
	BOOL m_bShowText;		//	�Ƿ���ʾ����
	BOOL m_bOwndrawBorder;		//	�Ի��߿�
	BOOL m_bDrawGlassStyle;		//	�Ƿ񻭲���Ч��
	INT  m_nLayerHeight;		//	ÿ��ĸ߶�
	INT  m_nMaxSidestepCount;	//	�ܹ������ٸ�̨��


	INT m_nyTopSpaceLines;	//	�����յ�����
	INT m_nxSpace;

public:

	void InitToolTip();
	void SetTooltipText( LPCTSTR lpszText );
	void SetTopSpaceLines( int iyLines );
	void SetOwndrawBorder( BOOL bOwndrawBorder, COLORREF crBorder );
	void SetDrawGlassStyle( BOOL bDrawGlassStyle );
	void SetColorConfig( COLORREF crBakClr, COLORREF crShadowClr, COLORREF crFaceClr );

private:
	void DrawBorder( CDC * pDC );
	VOID DrawGlass( HDC hDC, CRect rect );
	VOID DrawGradient( HDC hDC, CRect rect, COLORREF StartColor, COLORREF EndColor, long Direction );

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	//{{AFX_MSG(XingerProgress)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()


private:
	COLORREF	m_crBorder;
	COLORREF	m_crBakColorx;		//	������ɫ
	COLORREF	m_crShadowColorx;	//	��Ӱ��ɫ
	COLORREF	m_crFaceColorx;		//	��������ɫ

	CToolTipCtrl	m_ToolTip;		//	Tooltip
};



#endif	//end of define __MY_PROGRESS_HEAD__
