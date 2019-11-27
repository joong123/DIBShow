
// DIBShowDlg.h : 头文件
//

#pragma once

#include "afxwin.h"

#include "DIB.h"
#include "strsafe.h"


// CDIBShowDlg 对话框
class CDIBShowDlg : public CDialogEx
{
// 构造
public:
	CDIBShowDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIBSHOW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

	// 界面图片渲染
	bool RenderBkg();
	void PostRenderBkg();
	bool RenderMem(bool bEraseBkg = true);	// 图片 绘制到 内存preview
	bool RenderToDisplay();							// 内存preview 绘制到 preview控件

	void Render(bool bEraseBkg = true);		// RenderMem 并绘制到 preview控件

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
