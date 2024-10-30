#include "StdAfx.h"
#include "FileSaveManager.h"

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
FileSaveManager::FileSaveManager(void) :
mque_pImgSave(NULL),
mque_pImgSaveLow(NULL),
ThreadManager("FileSaveMgr")
{
	//// �n���h������
	m_evClearQue  = CreateEvent(NULL, FALSE, FALSE, NULL);
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
FileSaveManager::~FileSaveManager(void)
{
	//// �n���h�����
	CloseHandle(m_evClearQue);
}


//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int FileSaveManager::ThreadFirst()
{
	_ASSERT(NULL != mque_pImgSave);

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { m_evClearQue,
						mque_pImgSave->g_evSem 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	// �ǉ�
	if(NULL != mque_pImgSaveLow) this->AddEventNum(1, &mque_pImgSaveLow->g_evSem );
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int FileSaveManager::ThreadLast()
{
	// �S�L���[�J��
	if(NULL != mque_pImgSave)	 AllQueFree(mque_pImgSave);
	if(NULL != mque_pImgSaveLow) AllQueFree(mque_pImgSaveLow);
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void FileSaveManager::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_CLEAR = 0,					// �������L���[��S�폜
			EV_QUE,							// �摜�ۑ��˗��L���[ (��\)
			EV_QUE_LOW						// �摜�ۑ��˗��L���[ (���X�g���ŉB����r)
	};

	TYP_SAVE_IMG* pDeli = NULL;				// �󂯓n�����

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_CLEAR: 					// �������L���[��S�폜
		if(NULL != mque_pImgSave)	 AllQueFree(mque_pImgSave);
		if(NULL != mque_pImgSaveLow) AllQueFree(mque_pImgSaveLow);
		return;

//-----------------------------------------------------------------------------------------------
	case EV_QUE: 					// �摜�ۑ��˗��L���[ (��\)
		pDeli = mque_pImgSave->GetFromHead(); 
		
		// �摜�t�@�C����������
		SaveImg(pDeli);

		// �J��
		for(int ii=0; ii<pDeli->nSetCnt; ii++ ) { 
			//SAFE_DELETE(pDeli->pImg[ii]);
			VirtualFree(pDeli->pImg[ii], 0, MEM_RELEASE);
		}
		delete pDeli;
		return;

//-----------------------------------------------------------------------------------------------
	case EV_QUE_LOW: 					// �摜�ۑ��˗��L���[ (���X�g���ŉB����r)
		pDeli = mque_pImgSaveLow->GetFromHead(); 
		
		// �摜�t�@�C����������
		SaveImg(pDeli);

		// �J��
		for(int ii=0; ii<pDeli->nSetCnt; ii++ ) { 
			if(NULL == pDeli->pImg[ii] ) continue;
			//SAFE_DELETE(pDeli->pImg[ii]);
			VirtualFree(pDeli->pImg[ii], 0, MEM_RELEASE);
		}
		delete pDeli;
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
// �S�L���[�J��
// ThreadQueue<TYP_SAVE_IMG>* pQue �L���[�N���X
//------------------------------------------
void FileSaveManager::AllQueFree(ThreadQueue<TYP_SAVE_IMG>* pQue)
{
	TYP_SAVE_IMG* pDeli = NULL;				// �󂯓n�����

	// �󂯓n���L���[�Ɋi�[����Ă��鍀�ڂ�S�ĊJ��
	while(true) {
		pDeli = pQue->GetFromHead(0);
		if( pDeli == NULL ) break;

		// �J��
		for(int ii=0; ii<pDeli->nSetCnt; ii++ ) { 
			if(NULL == pDeli->pImg[ii] ) continue;
//			SAFE_DELETE(pDeli->pImg[ii]);
			VirtualFree(pDeli->pImg[ii], 0, MEM_RELEASE);
		}
		delete pDeli;
	}
}

//------------------------------------------
// �摜�t�@�C����������
// TYP_SAVE_IMG* p �󂯓n�����
//------------------------------------------
void FileSaveManager::SaveImg(TYP_SAVE_IMG* p)
{
	
	// �摜�쐬
	//std::ofstream	stream;
	for(int ii=0; ii<p->nSetCnt; ii++) {
		if( 0 >= p->nSize[ii]) continue;

		//// �ʏ�
		//stream.open(p->cPath[ii], std::ios::out | std::ios::trunc | std::ios::binary);
		//stream.write((char const*)p->pImg[ii], p->nSize[ii]);
		//stream.close();

		// �t�@�C���R�s�[
		FILE *fpw = fopen(p->cPath[ii], "wb");
		if(NULL == fpw) {
			LOG(em_ERR), "[%s] �摜�ǂݍ��ݎ��s(%d, %s)", my_sThreadName, errno, strerror(errno));
			return;
		}
		fwrite((char const*)p->pImg[ii], 1, p->nSize[ii], fpw);
		fclose(fpw);


		//// �摜�p�X�Z�b�g (���ꂪ�����ƁA�p�X���ɃZ�b�g������ŉ摜�ۑ��ƂȂ��Ă��܂�����)
		if( NULL != p->pMemPath[ii] ) {

			char * pOldFile = p->pMemPath[ii];
			if ( NULL != pOldFile[0] ) {
				// ���Ƀp�X���o�^����Ă�����A���̃t�@�C���͍폜���Ă���
				DeleteFile(p->pMemPath[ii]);
			}

			strcpy(p->pMemPath[ii], p->cPath[ii]);
		}

	}
}

