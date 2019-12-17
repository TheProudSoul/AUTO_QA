#include <windows.h>
#include <tchar.h> // _tcschr
#include <stdio.h> // fread
#include <stdlib.h> // random
#include <CommCtrl.h>//定义控件样式
#include "resource.h"
#include "medium_problems.h"
#include "hard_problems.h"
#pragma comment(lib,"comctl32.lib")//样式
//#pragma comment(lib, "WINMM.LIB") // 播放声音
const int width = 1500;
const int height = 800;

#define MAX_NAME_LENGTH 64 // 最大姓名长度
#define LONG_STRING_LENGTH 256 // 长字符串长度
#define HISTORY_COUNT 10 // 历史记录数量

// 设备相关参数
int cxScreen, cyScreen, cyCaption, cxSizeFrame, cySizeFrame;

// 按钮数量
#define MENU_NUM 6
#define OPTION_NUM 7
#define EASY 0
#define MEDIUM 1
#define HARD 2
#define HISTORY 3
#define EXIT 4
#define ABOUT 5
#define A 6
#define B 7
#define C 8
#define D 9
#define E 10
#define F 11
#define G 12

// 控件
struct
{
	TCHAR name[sizeof(TCHAR) == 1U ? 13 : 8];//ansi为10，unicode为8
} menu[MENU_NUM];
int onmouse = -1;//鼠标悬停按钮ID，未悬停为-1

// 窗体
HWND hwnd;
HWND hwndMenu[MENU_NUM];
HWND hwndOption[OPTION_NUM];
// font
const TCHAR FontName[] = TEXT("微软雅黑"); // 黑体
// color
const unsigned long crWhite = RGB(255, 255, 255); // 白色
const unsigned long crBackground = RGB(250, 248, 239); // 背景颜色
const unsigned long crText = RGB(115, 106, 98); // 深色标题
const unsigned long crButton = RGB(245, 235, 226); // 按钮背景
const unsigned long crGray = RGB(187, 173, 160);
const unsigned long cr32 = RGB(246, 124, 95);
const unsigned long cr64 = RGB(246, 94, 59);

// 变量
HINSTANCE hInst;
const int border = 15;
const int round = 5; // 圆角半径
TCHAR szFilePath[255];
TCHAR szUserName[255];
static int option_width, option_height;
static int button_width, button_height;
static TCHAR buffer_temp[255];
int current_ans = -1;

// 矩形块
RECT rectName;
RECT rectQuest;
RECT rectOptions;
RECT rectOption[4];
RECT rectText;

// declare
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // 视窗讯息处理程式
LRESULT CALLBACK MenuWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK OptionWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK HistoryDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void InitUserData();
void GenEasyQuestion();
void GenMediumQuestion();
void GenHardQuestion();
BOOL AskIfExit();
void FreshMainRect();
BOOL judgeAction(int);
unsigned long getPoints();
void FreshHistory();
void SaveGame(); // 保存分数

// 玩家数据
unsigned int total_ans;
unsigned int correct_ans;
unsigned int total_easy;
unsigned int correct_easy;
unsigned int total_medium;
unsigned int correct_medium;
unsigned int total_hard;
unsigned int correct_hard;
// TODO 从随机位置开始
unsigned long medium_sofar;
unsigned long hard_sofar;

// 题目全局备份
static int level;
static TCHAR question[256];
static TCHAR options[7][256];
static unsigned short option_num = 0;
static short answer;

// 历史记录
struct record
{
	SYSTEMTIME st;
	//time_t create_time;
	// TCHAR name[MAX_NAME_LENGTH];
	unsigned int score;
	unsigned int  correct_rate;
	unsigned int easy_correct;
	unsigned int easy_num;
	unsigned int medium_correct;
	unsigned int medium_num;
	unsigned int hard_correct;
	unsigned int hard_num;
} sHistory[HISTORY_COUNT];

void DrawTextAdvance(HDC hdc, TCHAR text[], RECT* rect, long FontSize, int FontWeight, unsigned long color, const TCHAR FontName[], UINT format)
{
	long lfHeight;
	HFONT hf;
	lfHeight = -MulDiv(FontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	hf = CreateFont(lfHeight, 0, 0, 0, FontWeight, 0, 0, 0, 0, 0, 0, 0, 0, FontName);
	SelectObject(hdc, hf);
	SetTextColor(hdc, color);
	DrawText(hdc, text, -1, rect, format);
	DeleteObject(hf);
}

void FillRectAdvance(HDC hdc, RECT* rect, unsigned long color)
{
	HBRUSH hBrush;
	hBrush = CreateSolidBrush(color);
	FillRect(hdc, rect, hBrush);
	DeleteObject(hBrush);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	MSG msg;
	WNDCLASS wndclass;
	static TCHAR szAppName[] = TEXT("The Auto QA");
	static TCHAR szAppTitle[MAX_PATH];
	hInst = hInstance;
	//获得程序路径
	GetModuleFileName(NULL, szFilePath, sizeof(szFilePath));
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;
	lstrcat(szFilePath, TEXT("AUTO_QA.SAV"));

	//获得系统参数
	cxScreen = GetSystemMetrics(SM_CXSCREEN);
	cyScreen = GetSystemMetrics(SM_CYSCREEN);
	cyCaption = GetSystemMetrics(SM_CYCAPTION);
	cxSizeFrame = GetSystemMetrics(SM_CXSIZEFRAME);
	cySizeFrame = GetSystemMetrics(SM_CYSIZEFRAME);

	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	//wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	//wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.hbrBackground = CreateSolidBrush(crBackground);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass(&wndclass))
	{
		MessageBox(NULL, TEXT("Fail to register WndClass registed."), szAppName, MB_ICONERROR);
		return 0;
	}

	wndclass.lpfnWndProc = MenuWndProc;
	wndclass.cbWndExtra = sizeof(long);
	wndclass.hIcon = NULL;
	wndclass.lpszClassName = TEXT("szMenuClass");

	RegisterClass(&wndclass);

	wndclass.lpfnWndProc = OptionWndProc;
	wndclass.cbWndExtra = sizeof(long);
	wndclass.hIcon = NULL;
	wndclass.lpszClassName = TEXT("szOptionClass");

	RegisterClass(&wndclass);

	hwnd = CreateWindow(szAppName,
		TEXT("自动出题程序"),
		WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX,
		(cxScreen - width) / 2,
		(cyScreen - height) / 2,
		width + 2 * cxSizeFrame, // win7下宽度大10，xp下宽度大n（未测量）
		height + cyCaption + 2 * cySizeFrame, // -10
		NULL,
		NULL,
		hInstance,
		NULL);

	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT rect;
	HDC hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	HPEN hPen;

	switch (message)
	{
	case WM_CREATE:
	{
		int dx, dy;
		RECT ClientRect, WindowRect;

		//控件初始化
		InitCommonControls();

		//窗口大小修正，如果没有这段则窗口大小不正确
		GetClientRect(hwnd, &ClientRect);
		GetWindowRect(hwnd, &WindowRect);
		dx = ClientRect.right - width;
		dy = ClientRect.bottom - height;
		SetWindowPos(hwnd, NULL, (cxScreen - width) / 2, (cyScreen - height) / 2, (WindowRect.right - WindowRect.left) - dx, (WindowRect.bottom - WindowRect.top) - dy, 0);

		// 设置欢迎标题矩形
		rectName.left = border;
		rectName.top = border;
		rectName.right = width / 3 - border;
		rectName.bottom = height / 4;

		// 设置题目显示区域矩形
		rectQuest.top = border;
		rectQuest.left = border + width / 3;
		rectQuest.right = width - border;
		rectQuest.bottom = (height - border - border) / 3;

		// 设置选项区域
		rectOptions.top = (height - border - border) / 3;
		rectOptions.left = border + width / 3;
		rectOptions.right = width - border;
		rectOptions.bottom = height - border * 3.5;
		option_width = width / 3 - border * 1.5;
		option_height = height / 6 - border * 1.5;
		for (size_t i = 0; i < 4; i++) {
			rectOption[i].top = rectOptions.top + (i / 2) * option_height + (i / 2 + 1) * border;
			rectOption[i].left = rectOptions.left + (i % 2) * (option_width + border);
			rectOption[i].right = rectOptions.left + (i % 2 + 1) * option_width + (i % 2) * border;
			rectOption[i].bottom = rectOptions.top + (i / 2 + 1) * (option_height + border);
		}

		// 设置相关信息区域
		rectText.top = rectOptions.bottom + border * 0.5;
		rectText.left = rectOptions.left;
		rectText.right = rectOptions.right;
		rectText.bottom = rectOptions.bottom + border * 2.5;

		// 菜单按钮初始化
		LoadString(hInst, IDS_STRING_EASY, menu[0].name, sizeof(menu[0].name));
		LoadString(hInst, IDS_STRING_MEDIUM, menu[1].name, sizeof(menu[1].name));
		LoadString(hInst, IDS_STRING_HARD, menu[2].name, sizeof(menu[2].name));
		LoadString(hInst, IDS_STRING_HISTORY, menu[3].name, sizeof(menu[3].name));
		LoadString(hInst, IDS_STRING_EXIT, menu[4].name, sizeof(menu[4].name));
		LoadString(hInst, IDS_STRING_ABOUT, menu[5].name, sizeof(menu[5].name));
		button_width = width / 4;
		button_height = (height / 4 * 3 - 30 * 6) / MENU_NUM;
		for (int i = 0; i < MENU_NUM; i++)
			hwndMenu[i] = CreateWindow(TEXT("szMenuClass"), menu[i].name,
				WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
				(width / 3 - button_width - border) / 2, button_height * i + height / 5 + 30 * (i + 1),
				button_width, button_height,
				hwnd, (HMENU)i,
				((LPCREATESTRUCT)lParam)->hInstance, NULL);

		// 答题选择按钮初始化
		for (int i = 0; i < OPTION_NUM; i++)
			hwndOption[i] = CreateWindow(TEXT("szOptionClass"), "Option" + i,
				WS_CHILD | BS_OWNERDRAW,
				border + width / 3 + (i % 2) * (option_width + border), (height - border - border) / 3 + (i / 2) * option_height + (i / 2 + 1) * border,
				option_width, option_height,
				hwnd, (HMENU)(i + MENU_NUM),
				((LPCREATESTRUCT)lParam)->hInstance, NULL);

		InitUserData();

		// 加载历史记录文件
		{
			FILE* file;
			if (_tfopen_s(&file, szFilePath, TEXT("r,ccs=UNICODE")) == 0) // 成功则为0
			{
				fread(sHistory, sizeof(sHistory), 1, file);
				_fcloseall();
			}
		}
		return 0;
	}
	case WM_PAINT:
	{
		InvalidateRect(hwnd, &rect, TRUE);
		hdc = BeginPaint(hwnd, &ps);
		SetBkMode(hdc, TRANSPARENT);

		// 画出中间分隔线
		MoveToEx(hdc, width / 3, 0, NULL);
		LineTo(hdc, width / 3, height);

		// 画标题
		DrawTextAdvance(hdc, TEXT("自动出题系统"), &rectName, 34, 0, crText, FontName, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		hBrush = CreateSolidBrush(crGray);
		SelectObject(hdc, hBrush);//分数块背景
		hPen = CreatePen(PS_NULL, 0, 0);
		SelectObject(hdc, hPen);//去掉画笔

		// 要求用户输入信息（算了算了）

		DeleteObject(hBrush);
		DeleteObject(hPen);
		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_CLOSE:
		if (AskIfExit() == TRUE) {
			SendMessage(GetParent(hwnd), WM_DESTROY, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK MenuWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		HDC hdc;

		PAINTSTRUCT ps;
		RECT rect;
		HDC hDCMem;
		HBITMAP hBitmap;
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);

		hDCMem = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(hDCMem, hBitmap);

		FillRectAdvance(hDCMem, &rect, onmouse == GetDlgCtrlID(hwnd) ? cr32 : crGray);
		SetBkMode(hDCMem, TRANSPARENT);
		DrawTextAdvance(hDCMem, menu[GetDlgCtrlID(hwnd)].name, &rect, 10, 700, crWhite, FontName, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hDCMem, 0, 0, SRCCOPY);
		DeleteObject(hBitmap);
		DeleteDC(hDCMem);

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_LBUTTONDOWN:
		switch (GetDlgCtrlID(hwnd))
		{
		case EASY:
			level = EASY;
			FreshMainRect();
			break;
		case MEDIUM:
			level = MEDIUM;
			FreshMainRect();
			break;
		case HARD:
			level = HARD;
			FreshMainRect();
			break;
		case HISTORY:
			// TODO 这里有个BUG：重复点击会出错
			DialogBox(hInst, MAKEINTRESOURCE(IDD_HISTORY), hwnd, HistoryDlgProc);
			break;
		case EXIT:
			if (AskIfExit() == TRUE) {
				SendMessage(GetParent(hwnd), WM_DESTROY, wParam, lParam);
			}
			break;
		case ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUT), hwnd, AboutDlgProc);
			break;
		}
		return 0;
	case WM_MOUSEMOVE:
	{
		int i;
		onmouse = GetDlgCtrlID(hwnd);
		for (i = 0; i < MENU_NUM; i++)
			InvalidateRect(hwndMenu[i], NULL, FALSE);
		return 0;
	}
	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK OptionWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		HDC hdc;

		PAINTSTRUCT ps;
		RECT rect;
		HDC hDCMem;
		HBITMAP hBitmap;
		hdc = BeginPaint(hwnd, &ps);
		GetClientRect(hwnd, &rect);

		hDCMem = CreateCompatibleDC(hdc);
		hBitmap = CreateCompatibleBitmap(hdc, rect.right - rect.left, rect.bottom - rect.top);
		SelectObject(hDCMem, hBitmap);

		FillRectAdvance(hDCMem, &rect, crGray);
		SetBkMode(hDCMem, TRANSPARENT);

		DrawTextAdvance(hDCMem, options[GetDlgCtrlID(hwnd) - MENU_NUM], &rect, 12, 700, crWhite, FontName, DT_CENTER | DT_SINGLELINE | DT_VCENTER);

		BitBlt(hdc, 0, 0, rect.right - rect.left, rect.bottom - rect.top, hDCMem, 0, 0, SRCCOPY);
		DeleteObject(hBitmap);
		DeleteDC(hDCMem);

		EndPaint(hwnd, &ps);
		return 0;
	}
	case WM_LBUTTONDOWN:
	{
		BOOL judgement = judgeAction(GetDlgCtrlID(hwnd) - MENU_NUM);
		if (judgement)
		{

		}
		else
		{
			TCHAR buffer[100];
			TCHAR title[10] = TEXT("回答错误");
			wsprintf(buffer, TEXT("这道题的正确答案是：%s"), options[answer]);

			MessageBox(hwnd, buffer, title, MB_OK | MB_ICONQUESTION);
		}
		FreshMainRect();
	}
	case WM_MOUSEMOVE:
	{
		onmouse = GetDlgCtrlID(hwnd);
		for (int i = 0; i < OPTION_NUM; i++)
			InvalidateRect(hwndOption[i], NULL, FALSE);
		return 0;
	}
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

BOOL CALLBACK AboutDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		static TCHAR szAboutText[LONG_STRING_LENGTH];
		LoadString(hInst, IDS_STRING_ABOUTTEXT, szAboutText, sizeof(szAboutText));
		SetDlgItemText(hDlg, IDD_ABOUT_STATIC, szAboutText);
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

BOOL CALLBACK HistoryDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
	{
		static TCHAR szAboutText[LONG_STRING_LENGTH] = "时间\t\t分数\t正确率(%)\t简单题\t中等题\t困难题";
		TCHAR format[35] = "%d.%d.%d\t%d\t%d\t\t%d/%d\t%d/%d\t%d/%d";
		for (size_t i = 0; i < HISTORY_COUNT && sHistory[i].st.wMilliseconds; i++)
		{
			lstrcat(szAboutText, _T("\r\n"));
			wsprintf(buffer_temp, format, sHistory[i].st.wYear, sHistory[i].st.wMonth, sHistory[i].st.wDay,
				sHistory[i].score, sHistory[i].correct_rate,
				sHistory[i].easy_correct, sHistory[i].easy_num,
				sHistory[i].medium_correct, sHistory[i].medium_num,
				sHistory[i].hard_correct, sHistory[i].hard_num);
			lstrcat(szAboutText, buffer_temp);
		}
		SetDlgItemText(hDlg, IDC_HISTORY_EDIT, szAboutText);
		return TRUE;
	}
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hDlg, 0);
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void InitUserData()
{
	total_ans = 0;
	correct_ans = 0;
	total_easy = 0;
	correct_easy = 0;
	total_medium = 0;
	correct_medium = 0;
	total_hard = 0;
	correct_hard = 0;
	// TODO 从随机位置开始
	medium_sofar = rand() % MEDIUM_COUNT;
	hard_sofar = rand() % HARD_COUNT;
}

void EmptyCache() {
	wsprintf(question, "");
	answer = -1;
	option_num = 0;
	for (size_t i = 0; i < OPTION_NUM; i++)
	{
		wsprintf(options[i], "");
		ShowWindow(hwndOption[i], SW_HIDE);
	}
}

void GenEasyQuestion()
{
	option_num = 4;
	int firstNum = rand() % 1000;
	int secondNum = rand() % 1000;
	int operation = rand() % 4;
	int result;
	int remain;
	answer = rand() % 4;
	if (firstNum < secondNum)
	{
		int temp = firstNum;
		firstNum = secondNum;
		secondNum = temp;
	}
	switch (operation)
	{
	case 0:
		wsprintf(question, TEXT("%d + %d ="), firstNum, secondNum);
		result = firstNum + secondNum;
		for (size_t i = 0; i < 4; i++)
		{
			wsprintf(options[i], TEXT("%d"), result + i - answer);
		}
		break;
	case 1:
		wsprintf(question, TEXT("%d - %d ="), firstNum, secondNum);
		result = firstNum - secondNum;
		for (size_t i = 0; i < 4; i++)
		{
			wsprintf(options[i], TEXT("%d"), result + i - answer);
		}
		break;
	case 2:
		wsprintf(question, TEXT("%d × %d ="), firstNum, secondNum);
		result = firstNum * secondNum;
		for (size_t i = 0; i < 4; i++)
		{
			wsprintf(options[i], TEXT("%d"), result + i - answer);
		}
		break;
	case 3:
		wsprintf(question, TEXT("%d ÷ %d 的商和余数:"), firstNum, secondNum);
		result = firstNum / secondNum;
		remain = firstNum % secondNum;
		for (size_t i = 0; i < 4; i++)
		{
			wsprintf(options[i], TEXT("%d, %d"), result + i - answer, remain);
		}
		break;
	default:
		break;
	}
}

void GenMediumQuestion() {
	option_num = medium_choises[medium_sofar].choise_number;
	wsprintf(question, medium_questions[medium_sofar]);
	answer = medium_answers[medium_sofar];
	for (size_t i = 0; i < option_num; i++)
	{
		wsprintf(options[i], medium_choises[medium_sofar].choise[i]);
	}
	medium_sofar = (medium_sofar + 1) % MEDIUM_COUNT;
}

void GenHardQuestion() {
	option_num = hard_choises[hard_sofar].choise_number;
	wsprintf(question, hard_questions[hard_sofar]);
	answer = hard_answers[hard_sofar];
	for (size_t i = 0; i < option_num; i++)
	{
		wsprintf(options[i], hard_choises[hard_sofar].choise[i]);
	}
	hard_sofar = (hard_sofar + 1) % HARD_COUNT;
}

BOOL AskIfExit()
{
	TCHAR buffer[100];
	TCHAR title[10] = TEXT("退出程序");
	wsprintf(buffer, TEXT("您一共答对了 %d 道题，\n简单题 %d 道\n中等题 %d 道\n困难题 %d 道\n共获得积分 %d\n是否确定保存并退出"), correct_ans, correct_easy, correct_medium, correct_hard, getPoints());
	MessageBeep(0);

	// IDYES IDNO IDCANCEL
	switch (MessageBox(hwnd, buffer, title, MB_YESNOCANCEL | MB_ICONQUESTION))
	{
	case IDYES:
		SaveGame(); // fall throught
	case IDNO:
		InitUserData();
		return TRUE;
	case IDCANCEL:
		return FALSE;
	}
}

void FreshMainRect()
{
	HDC hdc;
	HBRUSH hBrush;
	HPEN hPen;
	hdc = GetDC(hwnd);

	SetBkMode(hdc, TRANSPARENT);
	hBrush = CreateSolidBrush(crGray);
	SelectObject(hdc, hBrush);
	hPen = CreatePen(PS_NULL, 0, 0);
	SelectObject(hdc, hPen);

	// 根据 level 生成题目
	EmptyCache();
	switch (level)
	{
	case EASY:
		GenEasyQuestion();
		break;
	case MEDIUM:
		GenMediumQuestion();
		break;
	case HARD:
		GenHardQuestion();
		break;
	default:
		break;
	}

	// 标题方框
	RoundRect(hdc, rectQuest.left, rectQuest.top, rectQuest.right, rectQuest.bottom, round, round);
	DrawTextAdvance(hdc, question, &rectQuest, 12, 700, crWhite, FontName, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	for (size_t i = 0; i < option_num; i++)
	{
		ShowWindow(hwndOption[i], SW_SHOW);
	}

	TCHAR buffer[100];
	wsprintf(buffer, TEXT("  共答对了 %d 道题，简单题 %d 道，中等题 %d 道\n，困难题 %d 道，共获得积分 %d。"), correct_ans, correct_easy, correct_medium, correct_hard, getPoints());
	RoundRect(hdc, rectText.left, rectText.top, rectText.right, rectText.bottom, round, round);
	DrawTextAdvance(hdc, buffer, &rectText, 10, 700, crWhite, FontName, DT_LEFT | DT_SINGLELINE | DT_VCENTER);


	DeleteObject(hBrush);
	DeleteObject(hPen);
	ReleaseDC(hwnd, hdc);
}

BOOL judgeAction(int chose_ans)
{
	total_ans++;
	// TODO 弹出回答正确提示框
	if (chose_ans == answer)
	{
		correct_ans++;
		switch (level)
		{
		case 0:
			total_easy++;
			correct_easy++;
			break;
		case 1:
			total_medium++;
			correct_medium++;
			break;
		case 2:
			total_hard++;
			correct_hard++;
			break;
		default:
			break;
		}
		return TRUE;
	}
	else
	{
		switch (level)
		{
		case 0:
			total_easy++;
			break;
		case 1:
			total_medium++;
			break;
		case 2:
			total_hard++;
			break;
		default:
			break;
		}
	}
	return FALSE;
}

unsigned long getPoints() {
	return correct_easy + correct_medium * 3 + correct_hard * 5;
}

void FreshHistory()
{
	for (size_t i = HISTORY_COUNT - 1; i > 0; i--)
	{
		sHistory[i] = sHistory[i - 1];
	}
	GetLocalTime(&sHistory[0].st);
	sHistory[0].score = getPoints();
	if (total_ans == 0)
	{
		sHistory[0].correct_rate = 0;
	}
	else
	{
		sHistory[0].correct_rate = correct_ans * 100 / total_ans;

	}
	sHistory[0].easy_num = total_easy;
	sHistory[0].easy_correct = correct_easy;
	sHistory[0].medium_num = total_medium;
	sHistory[0].medium_correct = correct_medium;
	sHistory[0].hard_num = total_hard;
	sHistory[0].hard_correct = correct_hard;
}

void SaveGame()
{
	// 存下当前记录
	FreshHistory();

	FILE* file;
	// w+: Opens an empty file for both reading and writing. If the file exists, its contents are destroyed.
	if (_tfopen_s(&file, szFilePath, TEXT("w+, ccs=UNICODE")) == 0)//成功则为0
	{
		fwrite(sHistory, sizeof(sHistory), 1, file);
		_fcloseall();
	}
	else
	{
		MessageBox(hwnd, TEXT("File could not be opened!"), TEXT("ERROR"), 0);
	}
}