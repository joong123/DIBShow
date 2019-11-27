
// DIBShowDlg.h : ͷ�ļ�
//

#pragma once

#include "afxwin.h"

#include "DIB.h"
#include "strsafe.h"


// CDIBShowDlg �Ի���
class CDIBShowDlg : public CDialogEx
{
// ����
public:
	CDIBShowDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIBSHOW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	void Init();
public:
	CDC		*pDCDisplay;
	CDC memDC;
	CStatic ctrlDisplay;

	bool	bDrag;
	int		xDest;
	int		yDest;
	POINT	ptDragLast;

	bool	bDragEnable;
	bool	bBkgInited;

	DibBase	dibBuffer;
	DibBase	dibBlank;
	DibBase	dibBkg;
	DibBase	dibBkgAlpha;
	DibBase	dibSrc;
	CBitmap bmpSrc;


	inline bool IsInside(const POINT &pt, const RECT &rc)
	{
		return !(pt.x<rc.left || pt.x >= rc.right || pt.y<rc.top || pt.y >= rc.bottom);
	}
	bool InsidePreview(const POINT &pt);

	// ����ͼƬ��Ⱦ
	bool RenderBkg();
	void PostRenderBkg();
	bool RenderMem(bool bEraseBkg = true);	// ͼƬ ���Ƶ� �ڴ�preview
	bool RenderToDisplay();							// �ڴ�preview ���Ƶ� preview�ؼ�

	void Render(bool bEraseBkg = true);		// RenderMem �����Ƶ� preview�ؼ�

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCancel2();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCancel3();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
protected:
	int nRenders;
	int nPaints;
	int nMouseMoves;
	double dTime;
	double dTime1;
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
