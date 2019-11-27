
// DIBShowDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "DIBShow.h"
#include "DIBShowDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDIBShowDlg �Ի���



CDIBShowDlg::CDIBShowDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIBSHOW_DIALOG, pParent)
	, nRenders(0)
	, nPaints(0)
	, nMouseMoves(0)
	, dTime(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDIBShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DISPLAY, ctrlDisplay);
	DDX_Text(pDX, IDC_STATIC_FRAMES, nRenders);
	DDX_Text(pDX, IDC_STATIC_PAINTS, nPaints);
	DDX_Text(pDX, IDC_STATIC_MOUSES, nMouseMoves);
	DDX_Text(pDX, IDC_STATIC_TIME1, dTime1);
	DDX_Text(pDX, IDC_STATIC_TIME, dTime);
}

BEGIN_MESSAGE_MAP(CDIBShowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDCANCEL2, &CDIBShowDlg::OnBnClickedCancel2)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_BN_CLICKED(IDCANCEL3, &CDIBShowDlg::OnBnClickedCancel3)
	ON_WM_ACTIVATE()
	ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDIBShowDlg ��Ϣ�������

BOOL CDIBShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	Init();

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDIBShowDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDIBShowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
		Render();

		nPaints++;
		UpdateData(FALSE);
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDIBShowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CDIBShowDlg::Init()
{
	Ticker::Prepare();

	ModifyStyle(0, WS_MINIMIZEBOX);	// �����������ʹ������С��
	ModifyStyle(0, WS_MAXIMIZEBOX);	// �����������ʹ�������

	// Բ�Ǿ�����ʽ
	CRect rcWnd;
	GetWindowRect(rcWnd);
	HRGN hrgn = CreateRoundRectRgn(0, 0, rcWnd.Width() + 1, rcWnd.Height() + 1, 2, 2);
	SetWindowRgn(hrgn, TRUE);

	CRect rcDisplay;
	ctrlDisplay.GetClientRect(&rcDisplay);

	bDrag = false;
	xDest = 0;
	yDest = 0;
	ptDragLast = { 0,0 };

	bDragEnable = true;
	bBkgInited = false;

	pDCDisplay = ctrlDisplay.GetDC();// ???TODO
	dibBuffer.Build(WIDTHOF(rcDisplay), HEIGHTOF(rcDisplay));
	dibBuffer.SetDC(pDCDisplay, WIDTHOF(rcDisplay), HEIGHTOF(rcDisplay));
	BOOL rs = dibBkgAlpha.LoadStandard(L"..\\Debug\\res\\bkg.bmp");
	rs = dibBlank.LoadStandard(L"..\\Debug\\res\\empty.bmp");
	rs = dibBkg.LoadStandard(L"..\\Debug\\res\\bkg.bmp");

	bmpSrc.CreateBitmap(640, 480, 1, 32, NULL);
	memDC.CreateCompatibleDC(NULL);
	CBitmap *oldMemBmp = memDC.SelectObject(&bmpSrc);

	//Render();// ���ã���


	// test
	Img img;
	img.IsEmpty();
}

bool CDIBShowDlg::InsidePreview(const POINT & pt)
{
	RECT rcDisplay;
	ctrlDisplay.GetWindowRect(&rcDisplay);
	ScreenToClient(&rcDisplay);

	return IsInside(pt, rcDisplay);
}

bool CDIBShowDlg::RenderBkg()
{
	BOOL rs = dibBkg.Draw(GetDC());
	PostRenderBkg();

	return TRUE == rs;
}

void CDIBShowDlg::PostRenderBkg()
{
	Render();
}

bool CDIBShowDlg::RenderMem(bool bEraseBkg)
{
	if (!dibBuffer.IsEmpty())
	{
		if (bEraseBkg)
		{
			dibBkgAlpha.Draw(&dibBuffer);
		}

		if (!dibSrc.IsEmpty())
		{
			dibSrc.Draw(&dibBuffer, xDest, yDest);
		}
		else
		{
			dibBlank.Draw(&dibBuffer);
		}
		return true;
	}

	return false;
}

bool CDIBShowDlg::RenderToDisplay()
{
	dibBuffer.Draw();

	return true;
}

void CDIBShowDlg::Render(bool bEraseBkg)
{
	RenderMem(bEraseBkg);
	RenderToDisplay();
}

void CDIBShowDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	tickc ts = 0;
	utick ut = 0;
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	RECT rcDisplay;
	ctrlDisplay.GetWindowRect(&rcDisplay);
	ScreenToClient(&rcDisplay);
	POINT ptCursorReview = { point.x - rcDisplay.left, point.y - rcDisplay.top };

	double sec = 0, sec1 = 0, sec2 = 0;
	if (bDrag)
	{
		xDest = xDest + point.x - ptDragLast.x;
		yDest = yDest + point.y - ptDragLast.y;
		ptDragLast = point;
		//memBuffer.SetDCDrawPos(xDest, yDest);

		//UpdatePreview();// TODO
		//Render();

		Ticker::GetTickCounter(ts);
		//Ticker::GetTickSystem(ut);
		ut = Ticker::GetTickCycle();
		RenderMem(true);
		//memDC.BitBlt(0, 0, 640, 480, pDCDisplay, 0, 0, SRCCOPY);
		Ticker::GetTickCycleSecond(ut, sec1);
		//Ticker::GetTickSystemSec(ut, sec);
		Ticker::GetTickCounterSec(ts, sec);

		Ticker::GetTickCounter(ts);
		RenderToDisplay();
		Ticker::GetTickCounterSec(ts, sec2);
		nRenders++;
	}

	if (bDrag)
	{
		dTime1 = sec1*1000;
		dTime = (sec)*1000;
		/*wchar_t txt[256] = L"";
		StringCchPrintf(txt, 256, L"%lf, %lf", dTime, sec * 1000000);
		MessageBox(txt);*/
		nMouseMoves++;
	}
	UpdateData(FALSE);

	CDialogEx::OnMouseMove(nFlags, point);
}


void CDIBShowDlg::OnBnClickedCancel2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog dlgFile(TRUE, NULL, NULL
		, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT
		, L"All Files|*.*||*.png|Bitmap(*.bmp)|*.bmp|JPeg(*.jpg)|*.jpg|Png(*.png)|");
	CString fileName;
	const int c_cMaxFiles = 100;
	const int c_cbBuffSize = (c_cMaxFiles * (MAX_PATH + 1)) + 1;
	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(c_cbBuffSize);
	dlgFile.GetOFN().nMaxFile = c_cMaxFiles;

	if (dlgFile.DoModal() == IDOK)
	{
		LARGE_INTEGER s, e, f;
		QueryPerformanceFrequency(&f);
		QueryPerformanceCounter(&s);


		Bitmap *pIn = ::new Bitmap(dlgFile.GetPathName());

		BOOL rs = dibSrc.Clone(pIn);
		SAFE_GLOBALDELETE(pIn);
		//UpdatePreview();
		Render();

		if (rs!=TRUE)
		{
			MessageBox(L"Load File Failed!", L"Error");
		}
		QueryPerformanceCounter(&e);
		//infoTimeSource = 1000 * (double)(e.QuadPart - s.QuadPart) / f.QuadPart;
		//UpdateData(FALSE);
	}

	// TODO
	fileName.ReleaseBuffer();
}


void CDIBShowDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (InsidePreview(point))
	{
		ptDragLast = point;
		bDrag = true;

		SetCapture();
	}
	else
	{
		if (bDragEnable)// �����϶�
		{
			PostMessage(WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(point.x, point.y));
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CDIBShowDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (bDrag)
	{
		bDrag = false;
		ReleaseCapture();

		UpdateData(FALSE);
	}

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CDIBShowDlg::OnBnClickedCancel3()
{
	dibSrc.Clear();
	Render();
}


void CDIBShowDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	CDialogEx::OnActivate(nState, pWndOther, bMinimized);

	// TODO: �ڴ˴������Ϣ����������
	if (WA_INACTIVE == nState)
	{
		bDrag = false;
	}
}


BOOL CDIBShowDlg::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;

	return CDialogEx::OnEraseBkgnd(pDC);
}


HBRUSH CDIBShowDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
	if (!bBkgInited)
	{
		RenderBkg();

		bBkgInited = true;
	}

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}


void CDIBShowDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	switch (nType)
	{
	case SIZE_MINIMIZED:
		bBkgInited = false;
		break;
	default:
		break;
	}
}
