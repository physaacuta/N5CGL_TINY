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
g_nClassCnt(0),
mcls_pLog(NULL)
{
	//// �V�O�i������
	m_evThStart  = CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p


	//// �e�N���X�֘A������
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		gcls_pContMgr[ii] = NULL;
	}
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

	//// �p�t�H�[�}���X�Ǘ��X���b�h
	gcls_ContDefrag.SetLogMgr(mcls_pLog);


	//// ini�t�@�C���ǂݍ��� 
	char		cWk[1024];
	CString		strVal;
	g_nClassCnt = 0;
	for(int ii=0; ii<CONTROL_MAXCNT; ii++) {
		strVal.Format("SESSION_%d", ii+1);
		GetPrivateProfileString(TI_DELETE, strVal, "", cWk, sizeof(cWk), TASKINI_NAME);		
		if( 0 >= strlen(cWk) ) break;

		// ���[�J�[�X���b�h ������
		gcls_pContMgr[g_nClassCnt] = new ControlManager(cWk, mcls_pLog);
		g_nClassCnt++;

		// �p�t�H�[�}���X�Ǘ� �q�X���b�h
		gcls_ContDefrag.WorkerNew(ii+1, cWk); 
	}


	//// ���[�J�[�X���b�h �N��
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->WorkerInit();
		gcls_pContMgr[ii]->Start();
	}

	//// ���QGr�폜
	gcls_pContLowGr = new ControlLowGr();
	gcls_pContLowGr->SetLogMgr(mcls_pLog); 
	if( gcls_pContLowGr->IsEnable() ) {
		// �L���ȏꍇ�̂�
		gcls_pContLowGr->WorkerInit();
		gcls_pContLowGr->Start();
	}

	//// �p�t�H�[�}���X�Ǘ�
	gcls_ContDefrag.Start();

	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInctance::Exit()
{
	//// �R���g���[���X���b�h ��~
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->Stop();
	}

	//// ���[�J�[�X���b�h ��~
	// �܂��S�̂ɒ�~�w����ʒm����
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->WorkerStop();
	}
	//// ���QGr�폜
	if( gcls_pContLowGr->IsEnable() ) {
		gcls_pContLowGr->Stop();
		gcls_pContLowGr->WorkerStop();
	}
	//// �p�t�H�[�}���X�Ǘ�
	gcls_ContDefrag.WorkerStop(); 

	Sleep(1000);

	//// ���[�J�[�X���b�h �J��
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->WorkerExit();
	}

	//// �R���g���[���X���b�h �J��
	for(int ii=0; ii<g_nClassCnt; ii++) {
		SAFE_DELETE(gcls_pContMgr[ii]);
	}

	//// ���QGr�폜 (�������Ɏ��s��OK)
	gcls_pContLowGr->WorkerExit();
	SAFE_DELETE(gcls_pContLowGr);

	//// �p�t�H�[�}���X�Ǘ�
	gcls_ContDefrag.Stop();
}


//------------------------------------------
// �폜/���� �D�惂�[�h (true:�폜�D��)
// bool mode True�F�폜�D�� False:�����D��
//------------------------------------------
void MainInctance::SetWaitMode(bool mode)
{
	for(int ii=0; ii<g_nClassCnt; ii++) {
		gcls_pContMgr[ii]->SetWaitMode(mode);
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

//-----------------------------------------------------------------------------------------------
		case WM_USER_TH_END: 						// �X���b�h�I���v��
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
