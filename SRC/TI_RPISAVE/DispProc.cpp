#include "StdAfx.h"
#include "DispProc.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
DispProc::DispProc(void) :
ThreadManager("DispP"),
m_bIsExec(false),
m_bIsDisp(false)
{
	Reset();
	InitializeCriticalSection(&my_cs);

	//// �V�O�i������
	m_evShowWin				= CreateEvent(NULL, FALSE, FALSE, NULL);
	m_evCloseWin			= CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
DispProc::~DispProc(void)
{
	DeleteCriticalSection(&my_cs);
	//// �V�O�i�����
	CloseHandle(m_evShowWin);
	CloseHandle(m_evCloseWin);
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int DispProc::ThreadFirst()
{
	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evShowWin,
						m_evCloseWin,
						this->mque_pIn->GetEvQue() 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int DispProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void DispProc::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_SHOW_WIN = 0,				// ��ʕ\���v��
			EV_CLOSE_WIN,					// ��ʏI���v��
			EV_ADD_IMG						// �t���[���摜�L���[�C���O
	};

	////// �V�O�i����������
	switch (nEventNo) {
//-----------------------------------------------------------------------------------------------
	case EV_SHOW_WIN:
		if( ! m_bIsDisp) ShowWindow();
		break;

	case EV_CLOSE_WIN:
		if(m_bIsDisp) CloseWindow();
		break;

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// �t���[���捞
		this->ExecQue();
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// ������
// Tin* pIn		����Que�f�[�^
// �߂�l		NULL:�ԋp���Ȃ� �o��Que�f�[�^:���̃X���b�h�֕ԋp����ꍇ  
//------------------------------------------
FrameDataManager* DispProc::OnAddNewItem( FrameDataManager* p )
{
	if( NULL == p ) {							// �ʏ킠�肦�Ȃ��͂�
		LOG(em_ERR), "[%s] �X���b�h���[�v�L���[����!", my_sThreadName);
		return NULL;
	}

	//// ����
	if(m_bIsExec) {
		Exec(p);

	} else {
		LOG(em_INF), "[%s] �L�����Z�� %d", my_sThreadName, mque_pIn->GetCount());
	}

	//// ��ɕԋp
	return p;
}


//------------------------------------------
// �������s
// FrameDataManager* p ���
//------------------------------------------
void DispProc::Exec(FrameDataManager* p)
{
	int	ii;
	double	dWk;
	int	nIns = p->nIns;
	int nColor = p->nColor;
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[nIns];




	// =========================================================
	// ���Ԍv������ �\��
	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_GetCamLast, FrameDataManager::TIME_GetCamFast );
	g_dTime_Iport[0] = g_dTime_Iport[0]*0.8 + dWk*0.2;
	g_dTime_Iport[1] = __max(g_dTime_Iport[1], dWk);
	

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_PlgSync_END, FrameDataManager::TIME_GetCamLast );
	g_dTime_PlgSync[0] = g_dTime_PlgSync[0]*0.8 + dWk*0.2;
	g_dTime_PlgSync[1] = __max(g_dTime_PlgSync[1], dWk);

	//dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_Coil_END, FrameDataManager::TIME_Coil_START );
	//g_dTime_CoilControl[0] = g_dTime_CoilControl[0]*0.8 + dWk*0.2;
	//g_dTime_CoilControl[1] = __max(g_dTime_CoilControl[1], dWk);

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgUnion_END, FrameDataManager::TIME_ImgUnion_START );
	g_dTime_ImgUnion[0] = g_dTime_ImgUnion[0]*0.8 + dWk*0.2;
	g_dTime_ImgUnion[1] = __max(g_dTime_ImgUnion[1], dWk);

	//dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgColor_END, FrameDataManager::TIME_ImgColor_START );
	//g_dTime_ImgColor[0] = g_dTime_ImgColor[0]*0.8 + dWk*0.2;
	//g_dTime_ImgColor[1] = __max(g_dTime_ImgColor[1], dWk);

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgBuf_END, FrameDataManager::TIME_ImgBuf_START );
	g_dTime_ImgBuf[0] = g_dTime_ImgBuf[0]*0.8 + dWk*0.2;
	g_dTime_ImgBuf[1] = __max(g_dTime_ImgBuf[1], dWk);

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgComp_END, FrameDataManager::TIME_ImgComp_START );
	g_dTime_ImgComp[0] = g_dTime_ImgComp[0]*0.8 + dWk*0.2;
	g_dTime_ImgComp[1] = __max(g_dTime_ImgComp[1], dWk);

	dWk = p->cls_Time.GetTimestamp( FrameDataManager::TIME_RpiWrite_END, FrameDataManager::TIME_RpiWrite_START );
	g_dTime_RpiWrite[nIns][0] = g_dTime_RpiWrite[nIns][0]*0.8 + dWk*0.2;
	g_dTime_RpiWrite[nIns][1] = __max(g_dTime_RpiWrite[nIns][1], dWk);



/*/
	CString s;
	s.Format("c:\\comp%d.bmp", p->nIns);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)p->pImgs, p->nLen );
	os.close();
//*/


	// =========================================================
	// ��ʕ\��
	bool bDisp = false;
	if( m_bIsDisp &&
		GetTickCount() > (m_nDspInterval[nIns][nColor]+TIME_DISP_REFRESH) ) {

		bDisp = true;
	}



	//// �E�B���h�E�\��
	// 2�t���[�����A������Ă���摜��\��
	if( bDisp ) {
		EnterCriticalSection(&my_cs);
		int ch = prm.nImgNumTotal*nIns + nColor;

		// �摜�f�[�^
		mcls_pImgWin->Display(ch, (ImageWindow::BITMAPHEADERS*)prmI.BmpHead, &p->pImgs[ prm.nBmpHeadSize ], false);

		// �I�[�o�[���b�v�� ���E������
		mcls_pImgWin->SetColor(RGB(255, 128, 0));			// �I�����W
		for(ii=0; ii<prm.ins[nIns].nCamsetNum; ii++ ) {
			mcls_pImgWin->DrawLine(ch, p->nImgLapPos[ii][0], 0, p->nImgLapPos[ii][0], 25, false);
			mcls_pImgWin->DrawLine(ch, p->nImgLapPos[ii][1], 0, p->nImgLapPos[ii][1], 25, false);
		}
		mcls_pImgWin->SetColor(RGB(255, 0, 0));				// ��
		mcls_pImgWin->DrawLine(ch, p->nImgClipPos[0], 0, p->nImgClipPos[0], 50, false);
		mcls_pImgWin->DrawLine(ch, p->nImgClipPos[1], 0, p->nImgClipPos[1], 50, true);

		// ���̕\���p
		m_nDspInterval[nIns][nColor] = GetTickCount();

		LeaveCriticalSection(&my_cs);
	}
}

//------------------------------------------
// ���Z�b�g
//------------------------------------------
void DispProc::Reset()
{
	for(int ii=0; ii<2; ii++) {
		g_dTime_Iport[ii] = 0.0;
		//g_dTime_Corr[ii] = 0.0;
		g_dTime_PlgSync[ii] = 0.0;
		//g_dTime_CoilControl[ii] = 0.0;
		g_dTime_ImgUnion[ii] = 0.0;
		//g_dTime_ImgColor[ii] = 0.0;
		g_dTime_ImgBuf[ii] = 0.0;
		g_dTime_ImgComp[ii] = 0.0;
		
		for(int jj=0; jj<MAX_INS; jj++) {
			g_dTime_RpiWrite[jj][ii] = 0.0;
		}
	}

	for(int ii=0; ii<MAX_INS; ii++) {
		for(int jj=0; jj<MAX_COLOR_NUM; jj++) {
			m_nDspInterval[ii][jj] = 0;
		}
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �t���[���摜�\����ʑ���

//------------------------------------------
// ��ʕ\��
//------------------------------------------
void DispProc::ShowWindow()
{
	const ParamManager::P_MAIN& prm = mcls_pParam->GetMain();	// �g���₷���悤�ɃG�C���A�X
	int nInsNum = prm.nIns;		
	int nImgNum = prm.nImgNumTotal;
	int nTotalCnt = prm.nIns * nImgNum;

	// ��ʖ���
	mcls_pImgWin->Init( nTotalCnt, AfxGetApp()->m_hInstance );
	for(int ii=0; ii<nInsNum; ii++ ) {
		for(int jj=0; jj<nImgNum; jj++) {
			int ch = nImgNum*ii+jj;

			if(0 != jj) {
				// ��i��
				mcls_pImgWin->SetZoom(ch, 1.0f, 1.0f);
				mcls_pImgWin->SetScale(ch, 1.0f/prm.nDispWindScaleX, 1.0f/prm.nDispWindScaleY);
				mcls_pImgWin->SetSize(ch, (long)(prm.nMaxWidth/prm.nDispWindScaleX), (long)(prm.nMaxHeight/prm.nDispWindScaleY) );
			} else {
				// ���i��
				mcls_pImgWin->SetZoom(ch, 1.0f/prm.nDispWindScaleX, 1.0f/prm.nDispWindScaleY);
				mcls_pImgWin->SetScale(ch, 1.0f, 1.0f);
				mcls_pImgWin->SetSize(ch, (long)(prm.nMaxWidth/prm.nDispWindScaleX), (long)(prm.nMaxHeight/prm.nDispWindScaleY) );
			}
			Sleep(100);
			
			
			mcls_pImgWin->SetPos(ch, prm.nDispWindPosX*jj, prm.nDispWindPosY*ii + prm.nDispWindPosY/4*jj );


			CString str;
			str.Format("�A���摜 Inst=%d Img=%d", ii, jj);
			mcls_pImgWin->SetTitle(ch, str);
		}
	}
	//mcls_pImgWin->Init( nInsNum, AfxGetApp()->m_hInstance );

	// �����ɉ摜�\�������悤�Ƃ���Ɨ����邽�߁A������Ƃ����\��������̂�����
	for(int ii=0; ii<nInsNum; ii++ ) {
		for(int jj=0; jj<nImgNum; jj++) {
			m_nDspInterval[ii][jj] = GetTickCount()+TIME_DISP_REFRESH;
		}	
	}

	//// �����\���͍ŏ������Ă���
	m_bIsDisp = false;
	for(int ii=0; ii<nInsNum; ii++ ) {
		for(int jj=0; jj<nImgNum; jj++) {
			int ch = nImgNum*ii+jj;
			mcls_pImgWin->ShowWindow(ch, SW_MINIMIZE);
		}
	}
}
//------------------------------------------
// ��ʔ�\��
//------------------------------------------
void DispProc::CloseWindow()
{
	EnterCriticalSection(&my_cs);
	m_bIsDisp = false;
	mcls_pImgWin->Dispose(); 
	LeaveCriticalSection(&my_cs);
}
//------------------------------------------
// ��ʕ`��On/Off
//------------------------------------------
void DispProc::WindowIsOnOff(bool bOnOff)
{
	if(m_bIsDisp == bOnOff) return;

	const ParamManager::P_MAIN& prm = mcls_pParam->GetMain();	// �g���₷���悤�ɃG�C���A�X
	int nInsNum = prm.nIns;		
	int nImgNum = prm.nImgNumTotal;

	for(int ii=0; ii<nInsNum; ii++ ) {
		for(int jj=0; jj<nImgNum; jj++) {
			int ch = nImgNum*ii+jj;

			if(bOnOff)	mcls_pImgWin->ShowWindow(ch, SW_NORMAL);
			else 		mcls_pImgWin->ShowWindow(ch, SW_MINIMIZE);

		}
	}

	m_bIsDisp = bOnOff;
}
