/* --------------------------------------------------------------------------------

	XingerProgress.cpp : implementation file
	���ߣ�������
	ʱ�䣺2003��6��15�� �ڱ�����ƽ

 --------------------------------------------------------------------------------*/


#include "stdafx.h"

#include "XingerProgress.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// XingerProgress

XingerProgress::XingerProgress()
{
	m_nyTopSpaceLines	= 6;		//	����Ĭ�ϵ�ͷ���ո�Ϊ 3 ��
	m_bShowTitle		= TRUE;		//	����Ĭ����ʾ����
	m_bShowText		= TRUE;		//	�Ƿ���ʾ����
	m_nLayerHeight		= 2;		//	ÿ��ĸ߶�
	m_nMaxSidestepCount	= 10;		//	�ܹ������ٸ�̨��
	m_bOwndrawBorder	= FALSE;	//	Ĭ��ʹ��ϵͳ�ı߿�
	m_bDrawGlassStyle	= FALSE;	//	�Ƿ񻭲���Ч��

	m_nxSpace		= 0;


	m_crBakColorx		= RGB(0,0,0);	// Ĭ�ϱ�����ɫ
	m_crShadowColorx	= RGB(0,128,0);	// Ĭ����Ӱ��ɫ
	m_crFaceColorx		= RGB(0,255,0);	// Ĭ��������ɫ����ɫ
}


XingerProgress::~XingerProgress()
{
}


BEGIN_MESSAGE_MAP(XingerProgress, CProgressCtrl)
	//{{AFX_MSG_MAP(XingerProgress)
	ON_WM_PAINT()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL XingerProgress::PreTranslateMessage(MSG* pMsg) 
{
	if ( IsWindowVisible() && m_ToolTip.m_hWnd )
	{
		m_ToolTip.RelayEvent( pMsg );
	}

//	if ( m_bFlat )
//	{
//		ModifyStyleEx( WS_EX_STATICEDGE, 0 );
//	}

	return CProgressCtrl::PreTranslateMessage( pMsg );
}

/////////////////////////////////////////////////////////////////////////////
// XingerProgress message handlers


void XingerProgress::OnPaint()
{
	COLORREF crShadow;
	COLORREF crFace;


	try
	{
		CPaintDC dc(this);	// �������������Ļ������
		INT nPos = GetPos();

		if ( nPos < 0 )
			nPos = 0;
		else if ( nPos > 100 )
			nPos = 100;

		if ( nPos > 95 && nPos <= 100 )
		{
			crShadow	= RGB(0xFF,0x00,0x00);
			crFace		= RGB(0xFF,0x00,0x00);
		}
		else if ( nPos > 80 && nPos <= 95 )
		{
			crShadow	= RGB(0xFF,0x99,0x00);
			crFace		= RGB(0xFF,0x99,0x00);
		}
		else
		{
			crShadow	= m_crShadowColorx;
			crFace		= m_crFaceColorx;
		}

		CRect rect;
		INT cxClientWidth	= 0;
		INT ixSplit		= 0;
		INT iXstart1		= 0;
		INT iXend1		= 0;
		INT iXstart2		= 0;
		INT iXend2		= 0;
		INT nySidestepPositive	= 0;
		INT nySidestepReverse	= 0;

		HPEN hPen1		= NULL;
		HPEN hPen2		= NULL;
		HGDIOBJ hOriginal	= NULL;
		INT i			= 0;
		INT j			= 0;
		INT ixSpaceLeft		= 0;
		INT iyLines		= 0;
		INT nySidestep		= 0;	//	̨����
		INT nYInnerHeight;

		hPen1		= CreatePen( PS_SOLID, 0, crShadow );	// ����������Ӱ���Ļ���
		hPen2		= CreatePen( PS_SOLID, 0, crFace );	// �������ƽ������Ļ���

		GetClientRect( &rect );
		dc.FillSolidRect( &rect, m_crBakColorx );			// ������Ϊ��ɫ����

		//
		//	������Ч��
		//
		if ( m_bDrawGlassStyle )
		{
			DrawGlass( dc.GetSafeHdc(), &rect );
		}


		cxClientWidth	= rect.Width();
		ixSplit		= cxClientWidth / 4;

		if ( 0 == m_nxSpace )
		{
			//
			//	���ͼ����ʼ�ڽ����� X λ��
			//
			iXstart1	= ixSplit;
			iXend1		= ixSplit * 2;

			//
			//	�ұ�ͼ����ʼ������� X λ��
			//
			iXstart2	= ixSplit * 2 + 1;
			iXend2		= ixSplit * 3 + 1;
		}
		else
		{
			//
			//	���ͼ����ʼ�ڽ����� X λ��
			//
			iXstart1	= m_nxSpace;
			iXend1		= ixSplit * 2;
			
			//
			//	�ұ�ͼ����ʼ������� X λ��
			//
			iXstart2	= ixSplit * 2 + 1;
			iXend2		= cxClientWidth - m_nxSpace;
		}	

		//
		//	���� nPos = 66
		//	̨��Ӧ����
		//	----------------------------------------
		//	100	10
		//	66	x
		//	x = 10 * 66 / 100
		//	  = 66 / 10
		//	  = 6
		//
		//nySidestepPositive = nPos / 10;		// 0��1��2��3��4��5��6��7��8��9
		nySidestepPositive = ( m_nMaxSidestepCount * nPos ) / 100;
		if ( 0 != nPos )
		{
			nySidestepPositive += 1;	// ����� 0 % ����ʾ�κν��ȣ�nPos>1 and nPos<=10 ��ʾһ��
		}
		nySidestepReverse	= m_nMaxSidestepCount - nySidestepPositive;	// �෴��ֵ 0��1��2��3��4��5��6��7��8��9

		//
		//	�������̨�����������෴��̨����
		//


		iyLines		= 0;
		nySidestep	= 0;
		nYInnerHeight	= m_nLayerHeight * m_nMaxSidestepCount + ( m_nMaxSidestepCount - 1 );
		for ( i = m_nyTopSpaceLines; i < m_nyTopSpaceLines + nYInnerHeight; i++ )
		{
			//	2*10+9=29 ���� y ����

			if ( iyLines == ( nySidestep * ( m_nLayerHeight + 1 ) + m_nLayerHeight ) )
			{
				//
				//	����ÿһ��̨�ף���̨������ 1
				//	һ��̨���� 2 ��
				//
				nySidestep ++;

				//	...
				i ++;
				iyLines ++;
			}

			if ( nySidestep >= nySidestepReverse )
			{
				//
				//	������ʵ��(ÿ��һ��)
				//	Saving the original object
				//
				hOriginal = SelectObject( dc, hPen2 );
				
				if ( 1 == m_nLayerHeight )
				{
					MoveToEx( dc, iXstart1, i, NULL );	// ����һ����
					LineTo( dc, iXend1 - 0, i );
					MoveToEx( dc, iXstart2, i, NULL );	// ���ڶ�����
					LineTo( dc, iXend2 - 0, i );
				}
				else
				{
					MoveToEx( dc, iXstart1, i, NULL );	// ����һ����
					LineTo( dc, iXend1, i );
					MoveToEx( dc, iXstart2, i, NULL );	// ���ڶ�����
					LineTo( dc, iXend2, i );
				}
				
				//	Restoring the original object
				SelectObject( dc, hOriginal );
			}
			else
			{
				//
				//	������(ÿ��һ��)
				//
				if ( 1 == m_nLayerHeight || 0 == iyLines % 3 )
				{
					ixSpaceLeft = 0;
				}
				else
				{
					ixSpaceLeft = 1;
				}

				//	Saving the original object
				hOriginal = SelectObject( dc, hPen1 );

				for ( j = iXstart1 + ixSpaceLeft; j < iXend1; j+=2 )
				{
					//	�������
					MoveToEx( dc, j, i, NULL );		// ����һ����
					LineTo( dc, j+1, i );
				}
				for ( j = iXstart2 + ixSpaceLeft; j<iXend2; j+=2 )
				{
					//	�������
					MoveToEx( dc, j, i, NULL );		// ���ڶ�����
					LineTo( dc, j+1, i );
				}

				//	Restoring the original object
				SelectObject( dc, hOriginal );
			}

			//
			//	���м� 1
			//
			iyLines ++;
		}


		//	��̬���ý��������⣬�ڽ�����������ı�
		if ( m_bShowText )
		{
			CRect rt;
			GetClientRect( &rt );					// ����豸�����С
			dc.SetBkMode( TRANSPARENT );				// �ı������ʽΪ͸��
			dc.SetTextColor(crFace);				// �ı���ɫ��Ĭ��ɫΪ��ɫ

			TCHAR szBuffer[100]	= {0};		// ����������ֻ���
			INT cxChar	= 0;			// ��������ⳤ��
			INT iLength	= 0;
			TEXTMETRIC tm;				// ϵͳ������Ϣ�ṹ��

			GetTextMetrics( dc, &tm );		// ��ȡϵͳ������Ϣ
			cxChar = tm.tmAveCharWidth;		// ��ȡϵͳ����Ŀ��

			iLength = _sntprintf( szBuffer, sizeof(szBuffer)-sizeof(TCHAR), _T("%d%%"), nPos );
			TextOut( dc,
				(rt.Width() - iLength * cxChar -6) / 2,
				m_nyTopSpaceLines + 30, szBuffer, iLength );
		}

		//	...
		if ( hPen1 )
		{
			DeleteObject( hPen1 );
			hPen1 = NULL;
		}
		if ( hPen2 )
		{
			DeleteObject( hPen2 );
			hPen2 = NULL;
		}

		//
		//	���߿�
		//
		if ( m_bOwndrawBorder )
		{
			CDC* pWinDC = GetWindowDC();
			if ( pWinDC )
			{
				DrawBorder( pWinDC );
				ReleaseDC( pWinDC );
			}
		}
	}
	catch (...)
	{
	}
}



int XingerProgress::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CProgressCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void XingerProgress::SetOwndrawBorder( BOOL bOwndrawBorder, COLORREF crBorder )
{
	m_bOwndrawBorder = bOwndrawBorder;
	m_crBorder = crBorder;
}


void XingerProgress::SetTopSpaceLines(int iyLines=5)
{
	m_nyTopSpaceLines	= iyLines;
}


void XingerProgress::SetColorConfig(COLORREF crBakClr=RGB(0,0,0), COLORREF crShadowClr=RGB(0,255,0), COLORREF crFaceClr=RGB(0,128,0))
{
	m_crBakColorx		= crBakClr;
	m_crFaceColorx		= crFaceClr;
	m_crShadowColorx	= crShadowClr;
}

void XingerProgress::SetDrawGlassStyle( BOOL bDrawGlassStyle )
{
	m_bDrawGlassStyle = bDrawGlassStyle;
}

void XingerProgress::InitToolTip()
{
	if ( m_ToolTip.m_hWnd == NULL )
	{
		// Create ToolTip control
		m_ToolTip.Create(this);

		// Create inactive
		m_ToolTip.Activate(TRUE);

		// Enable multiline
		m_ToolTip.SendMessage( TTM_SETMAXTIPWIDTH, 0, 400 );
	}
}


void XingerProgress::SetTooltipText(LPCTSTR lpszText)
{
	// We cannot accept NULL pointer
	if ( lpszText == NULL )
		return;
	
	// Initialize ToolTip
	InitToolTip();

	//	If there is no tooltip defined then add it
	if ( m_ToolTip.GetToolCount() == 0 )
	{
		CRect rectBtn; 
		GetClientRect(rectBtn);
		m_ToolTip.AddTool( this, lpszText, rectBtn, 1);
	}

	//	Set text for tooltip
	m_ToolTip.UpdateTipText( lpszText, this, 1 );
	m_ToolTip.Activate(TRUE);
}

void XingerProgress::DrawBorder(CDC *pDC)
{
	if (m_hWnd )
	{
		CBrush Brush( m_crBorder );
		CBrush* pOldBrush = pDC->SelectObject(&Brush);

		CRect rtWnd;
		GetWindowRect(&rtWnd); 
		
		CPoint point;

		//	��䶥�����
		point.x = rtWnd.Width();
		point.y = 1;	//	GetSystemMetrics(SM_CYFRAME)+1;
		pDC->PatBlt(0, 0, point.x, point.y, PATCOPY);
		
		//��������
		point.x = 1;	//	GetSystemMetrics(SM_CXFRAME);
		point.y = rtWnd.Height();
		pDC->PatBlt(0, 0, point.x, point.y, PATCOPY);
		
		//���ײ����
		point.x = rtWnd.Width(); 
		point.y = 1;	//GetSystemMetrics(SM_CYFRAME) + 1;
		pDC->PatBlt(0, rtWnd.Height()-point.y, point.x, point.y, PATCOPY);
		
		//����Ҳ���
		point.x = 1;	//GetSystemMetrics(SM_CXFRAME);
		point.y = rtWnd.Height();
		pDC->PatBlt(rtWnd.Width()-point.x, 0, point.x, point.y, PATCOPY);
	}
}

VOID XingerProgress::DrawGlass( HDC hDC, CRect rect )
{
	CRect rectGradient;
	HPEN hPen1		= NULL;

	hPen1 = CreatePen( PS_SOLID, 0, RGB( 0xBE, 0xBE, 0xBE ) );

	//
	//	������ֱ��
	//
	SelectObject( hDC, hPen1 );

	MoveToEx( hDC, rect.left + 1, rect.top + 1, NULL );
	LineTo( hDC, rect.right-1, rect.top + 1 );

	if ( hPen1 )
	{
		DeleteObject( hPen1 );
		hPen1 = NULL;
	}

	//
	//	�����ҽ�����
	//
	rectGradient = rect;
	rectGradient.top	+= 1;
	rectGradient.left	+= 1;
	rectGradient.right	= rectGradient.left + 1;
	rectGradient.bottom	/= 2;
	DrawGradient( hDC, &rectGradient, RGB( 0xBE, 0xBE, 0xBE ), RGB( 0x00, 0x00, 0x00 ), 1 );

	rectGradient = rect;
	rectGradient.top	+= 1;
	rectGradient.left	= rectGradient.right - 3;
	rectGradient.right	-= 1;
	rectGradient.bottom	/= 2;
	DrawGradient( hDC, &rectGradient, RGB( 0xBE, 0xBE, 0xBE ), RGB( 0x00, 0x00, 0x00 ), 1 );

	//
	//	������������
	//
	rectGradient = rect;
	rectGradient.top	+= 2;
	rectGradient.left	+= 2;
	rectGradient.right	-= 2;
	rectGradient.bottom	/= 2;

	DrawGradient( hDC, &rectGradient, RGB( 0x80, 0x80, 0x80 ), RGB( 0x00, 0x00, 0x00 ), 1 );

}

/**
 *	@ Private
 *	���ƽ��䴰��
 */
VOID XingerProgress::DrawGradient( HDC hDC, CRect rect, COLORREF StartColor, COLORREF EndColor, long Direction )
{
	//	Direction=0 ����
	//	Direction=1 ����

	//�ڸ�����rect�л��ƽ���ɫ���StartColor----EndColor
	CRect rectFill;			   // Rectangle for filling band
	float fStep;              // How wide is each band?
	HBRUSH brush;			// Brush to fill in the bar	
	rectFill=rect;
	int r, g, b;
	r = (GetRValue(EndColor) - GetRValue(StartColor));
	g = (GetGValue(EndColor) - GetGValue(StartColor));
	b =  (GetBValue(EndColor) - GetBValue(StartColor));
	
	int nSteps = max(abs(r), max(abs(g), abs(b)));
	if(Direction==0)
		nSteps=min(nSteps,rect.right-rect.left);
	else
		nSteps=min(nSteps,rect.bottom-rect.top);
	float rStep, gStep, bStep;
	if(Direction==0)
		fStep = (float)(rect.right - rect.left)/ (float)nSteps;
	else
		fStep = (float)(rect.bottom - rect.top)/ (float)nSteps;
	rStep = r/(float)nSteps;
	gStep = g/(float)nSteps;
	bStep = b/(float)nSteps;
	
	r = GetRValue(StartColor);
	g = GetGValue(StartColor);
	b = GetBValue(StartColor);
	
	
	for (int iOnBand = 0; iOnBand < nSteps; iOnBand++) 
	{
		if(Direction==0)
			::SetRect(&rectFill,
			rect.left+(int)(iOnBand * fStep),       // Upper left X
			rect.top,									 // Upper left Y
			rect.left+(int)((iOnBand+1) * fStep),          // Lower right X
			rect.bottom);			// Lower right Y
		else
			::SetRect(&rectFill,
			rect.left,       // Upper left X
			rect.top+(int)(iOnBand * fStep),			 // Upper left Y
			rect.right,          // Lower right X
			rect.top+(int)((iOnBand+1) * fStep));			// Lower right Y
		brush = ::CreateSolidBrush(RGB(r+rStep*iOnBand, g + gStep*iOnBand, b + bStep *iOnBand));
		::FillRect( hDC, &rectFill, brush);
		
		if ( brush )
		{
			DeleteObject( brush );
			brush = NULL;
		}
	}
}