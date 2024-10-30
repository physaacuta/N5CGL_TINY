#include "StdAfx.h"
#include "MainInctance.h"

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
MainInctance::MainInctance(void) :
m_nUnitNum(0),
mcls_pLog(NULL)
{
	// �V�O�i������
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p

	// ������
	for(int ii=0; ii<MAX_THREAD; ii++) gcls_pCam[ii] = NULL;
}
//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInctance::~MainInctance(void)
{
	// �V�O�i���J��
	CloseHandle(m_evThStart);
}

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
BOOL MainInctance::Init()
{
	//// ���C���C���X�^���X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	WorkThread::Start(MainThread, THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() ) {
		LOG(em_MSG), "[MainInctance] �X���b�hID���蓖�Ē��E�E�E");
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}


	//// ���[�J�[�X���b�h ������
/*
	// �J�����R���g���[���ʐM�N���X
	gcls_pCam = new CameraManager();
	gcls_pCam->SetLogMgr(mcls_pLog);
	gcls_pCam->SetMainPostTid(GetTid());
*/

	CString		sSession;
	CString		sWk;
	char		cKey[64];
	sSession.Format("%s_MAIN", TO_CAMERA);
	for(int ii=0; ii<MAX_THREAD; ii++) {
		sWk.Format("NAME_%d", ii+1);
		GetPrivateProfileString(sSession, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
		if( 0 == ii && 0 == strlen(cKey) ) { strcpy(cKey, TO_CAMERA); }		// ��΂ɂP�͑��݂�����B
		if( 0 == strlen(cKey) ) break;										// �L�[�`�F�b�N

		// �C���X�^���X����
		gcls_pCam[ii] = new CameraManager();
		gcls_pCam[ii]->SetClass(ii, cKey);
		gcls_pCam[ii]->SetLogMgr(mcls_pLog);
		gcls_pCam[ii]->SetMainPostTid(GetTid());

		m_nUnitNum ++;
	}


	//// ���[�J�[�X���b�h �N��
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if( NULL == gcls_pCam[ii] ) continue;
		gcls_pCam[ii]->Start();
	}

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	//// ���[�J�[�X���b�h ��~
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if( NULL == gcls_pCam[ii] ) continue;
		gcls_pCam[ii]->Stop();
	}

	//// ���[�J�[�X���b�h �J��
	for(int ii=0; ii<MAX_THREAD; ii++) {
		if( NULL == gcls_pCam[ii] ) continue;
		SAFE_DELETE( gcls_pCam[ii] );
	}
}


//------------------------------------------
// �X���b�h
//------------------------------------------
UINT WINAPI MainInctance::MainThread(LPVOID param)
{
	MSG	msg;								// MSG�\����

	////// ���C���C���X�^���X���o��
	MainInctance *p = (MainInctance *)param;						// ���N���X�̃C���X�^���X���擾
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);			// ���b�Z�[�W�쐬
	p->SetTid();													// �X���b�hID�Z�b�g

	////// �����[�N�X���b�h�̏����������܂őҋ@
	pLOG(em_MSG), "[MainInctance] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", p->GetTid() );
	WaitForSingleObject(p->m_evThStart, INFINITE);


	////// ���b�Z�[�W�L���[�ǂݍ���
	pLOG(em_MSG), "[MainInctance] �X���b�h�J�n [0x%X]", p->GetTid());
	while( int ret = GetMessage(&msg, NULL, 0, 0) ){
		_ASSERT( -1 != ret );
		switch (msg.message) {


		case WM_USER_TH_END: 					// �X���b�h�I���v��
			pLOG(em_MSG), "[MainInctance] �X���b�h�I�� [0x%X]", p->GetTid());
			return 0;

		default :
			// ���肦�Ȃ��I�I
			break;
		}
	}

	// ���肦�Ȃ�
	pLOG(em_ERR), "[MainInctance] ���C�����[�v���甲�����I�I[0x%X]", p->GetTid());
	return 0;
}

