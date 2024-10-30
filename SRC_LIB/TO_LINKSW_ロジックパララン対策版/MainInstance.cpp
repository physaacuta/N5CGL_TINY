#include "StdAfx.h"
#include "MainInstance.h"

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
MainInstance::MainInstance(void) : 
m_nUnitNum(0),
ThreadManager("MI")
{
	// �V�O�i������
	m_evThStart	= CreateEvent(NULL, FALSE, FALSE, NULL);				// ���C���X���b�h�����p
	
	// ������
	for(int ii=0; ii<MAX_THREAD; ii++) gcls_pLinksw[ii] = NULL;
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MainInstance::~MainInstance(void)
{
	// �V�O�i�����
	CloseHandle(m_evThStart);
}

//------------------------------------------
// ���������� (��x�̂݋N��)
//------------------------------------------
BOOL MainInstance::Init()
{
	//// ���C���C���X���^�X�̂݁A�X���b�h�����͍���Ă��� (�����ɔ�V�O�i���Ƃ���)
	//// �p�����N���X���s (���C������X���b�h�ׁ̈A������ƗD��x����)
	ThreadManager::Start( THREAD_PRIORITY_ABOVE_NORMAL );

	// �X���b�hID��������܂ł�����Ƌx�e
	while( 0==GetTid() )
	{
		LOG(em_MSG), "[%s] �X���b�hID���蓖�Ē��E�E�E", my_sThreadName);
		Sleep(1);			// �X���b�h�N���܂ł�����Ƒ҂�
	}

	////////////////////////////////////
	//// ���[�J�[�X���b�h ������
	////////////////////////////////////
	//// �J���������N�X�C�b�`�ʐM�N���X	
	CString		sSession;
	CString		sWk;
	char		cKey[64];
	sSession.Format("%s_MAIN", TO_LINKSW);
	for(int ii=0; ii<MAX_THREAD; ii++) {
		sWk.Format("NAME_%d", ii+1);
		GetPrivateProfileString(sSession, sWk, "", cKey, sizeof(cKey), TASKINI_NAME);
		if( 0 == ii && 0 == strlen(cKey) ) { strcpy(cKey, TO_LINKSW); }		// ��΂ɂP�͑��݂�����B
		if( 0 == strlen(cKey) ) break;										// �L�[�`�F�b�N

		// �C���X�^���X����
		gcls_pLinksw[ii] = new LinkswManager();
		gcls_pLinksw[ii]->SetLogMgr(mcls_pLog);
		gcls_pLinksw[ii]->SetClass(ii, cKey);
		gcls_pLinksw[ii]->SetMainPostTid(GetTid());

		m_nUnitNum ++;
	}


	////////////////////////////////////
	//// ���[�J�[�X���b�h �N��
	////////////////////////////////////
	for (int ii = 0; ii<MAX_THREAD; ii++) {
		if (NULL == gcls_pLinksw[ii]) continue;
		gcls_pLinksw[ii]->Start();
	}


	return TRUE;
}

//------------------------------------------
// �I������ (��x�̂݋N��)
//------------------------------------------
void MainInstance::Exit()
{
	// ���[�J�[�X���b�h��~
	for (int ii = 0; ii<MAX_THREAD; ii++) {
		if (NULL == gcls_pLinksw[ii]) continue;
		gcls_pLinksw[ii]->Stop();
	}

	// ���[�J�[�X���b�h���
	for (int ii = 0; ii<MAX_THREAD; ii++) {
		if (NULL == gcls_pLinksw[ii]) continue;
		SAFE_DELETE(gcls_pLinksw[ii]);
	}
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h�����`


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MainInstance::ThreadFirst()
{

	//// �����[�N�X���b�h�̏����������܂őҋ@ (MainInstance�̂�)
	LOG(em_MSG), "[%s] �����[�N�X���b�h�̏����������܂őҋ@���E�E�E [0x%X]", my_sThreadName, GetTid() );
	WaitForSingleObject(m_evThStart, INFINITE);

	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int MainInstance::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MainInstance::ThreadEvent(int nEventNo)
{
	if( nEventNo == 0 ){

	}

}
