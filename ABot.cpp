
// ABot.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "ABot.h"
#include "ABotDlg.h"
#include "CDKHOpenAPI.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CABotApp

BEGIN_MESSAGE_MAP(CABotApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CABotApp 생성

CABotApp::CABotApp()
{
	// 다시 시작 관리자 지원
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CABotApp 개체입니다.

CABotApp theApp;


// CABotApp 초기화

BOOL CABotApp::InitInstance()
{
	// 응용 프로그램 매니페스트가 ComCtl32.dll 버전 6 이상을 사용하여 비주얼 스타일을
	// 사용하도록 지정하는 경우, Windows XP 상에서 반드시 InitCommonControlsEx()가 필요합니다.
	// InitCommonControlsEx()를 사용하지 않으면 창을 만들 수 없습니다.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 응용 프로그램에서 사용할 모든 공용 컨트롤 클래스를 포함하도록
	// 이 항목을 설정하십시오.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	AfxInitRichEdit(); //for RichEdit 2.0

	// 대화 상자에 셸 트리 뷰 또는
	// 셸 목록 뷰 컨트롤이 포함되어 있는 경우 셸 관리자를 만듭니다.
	CShellManager *pShellManager = new CShellManager;

	// MFC 컨트롤의 테마를 사용하기 위해 "Windows 원형" 비주얼 관리자 활성화
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 표준 초기화
	// 이들 기능을 사용하지 않고 최종 실행 파일의 크기를 줄이려면
	// 아래에서 필요 없는 특정 초기화
	// 루틴을 제거해야 합니다.
	// 해당 설정이 저장된 레지스트리 키를 변경하십시오.
	// TODO: 이 문자열을 회사 또는 조직의 이름과 같은
	// 적절한 내용으로 수정해야 합니다.
	SetRegistryKey(_T("로컬 응용 프로그램 마법사에서 생성된 응용 프로그램"));

	//////////////////////////////////////////////////////////////////////////
	// 경고 메시지 추가
	int ret = AfxMessageBox("[경고]!!!\
							\n본 프로그램은 여러 가지 오류를 포함하고 있으며, 본 프로그램을 사용하였을때 손실이 발생 할 수 있습니다.\
							\n투자 손실이 있을 경우라도 본 프로그램의 작성자 및 배포자는 책임 지지 않으며, 가상 계좌에만 사용을 허락 합니다.\
							\n\
							\n위의 모든 내용에 동의 하였고, 사용을 원한다면, ''예''를 눌러주세요."
	,MB_YESNO | MB_ICONEXCLAMATION | MB_DEFBUTTON2);
	if (ret == IDNO) { // NO를 누를 경우 종료한다.
		return FALSE;
	}
	//////////////////////////////////////////////////////////////////////////
	
	{
		SYSTEM_INFO    info;
		::GetSystemInfo(&info);
		HANDLE    handlethis;
		handlethis = GetCurrentProcess();

		if (info.dwNumberOfProcessors > 1)
		{
			DWORD ProcessAffinityMask = 0, SystemAffinityMask = 0;
			//Preocess Affinity test용
			GetProcessAffinityMask(
				handlethis,            // handle to the process of interest
				&ProcessAffinityMask,    // pointer to structure to receive process affinity mask
				&SystemAffinityMask        // pointer to structure to receive system affinity mask
				);

			ProcessAffinityMask = 0x1;

			SetProcessAffinityMask(
				handlethis,            // handle to the process of interest
				ProcessAffinityMask        // pointer to structure to receive process affinity mask
				);

			GetProcessAffinityMask(
				handlethis,            // handle to the process of interest
				&ProcessAffinityMask,    // pointer to structure to receive process affinity mask
				&SystemAffinityMask        // pointer to structure to receive system affinity mask
				);

			SetPriorityClass(handlethis, HIGH_PRIORITY_CLASS);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	CABotDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{
		// TODO: 여기에 [확인]을 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 여기에 [취소]를 클릭하여 대화 상자가 없어질 때 처리할
		//  코드를 배치합니다.
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "경고: 대화 상자를 만들지 못했으므로 응용 프로그램이 예기치 않게 종료됩니다.\n");
		TRACE(traceAppMsg, 0, "경고: 대화 상자에서 MFC 컨트롤을 사용하는 경우 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS를 수행할 수 없습니다.\n");
	}

	// 위에서 만든 셸 관리자를 삭제합니다.
	SafeDelete(pShellManager);

	// 대화 상자가 닫혔으므로 응용 프로그램의 메시지 펌프를 시작하지 않고  응용 프로그램을 끝낼 수 있도록 FALSE를
	// 반환합니다.
	return FALSE;
}

int CABotApp::ExitInstance()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	return CWinApp::ExitInstance();
}
