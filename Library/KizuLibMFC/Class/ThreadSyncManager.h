// *********************************************************************************
//	����X���b�h�ԏ����ۏᐧ��N���X(������ۏႷ��X���b�h�L���[�݂����Ȃ���)
//	[Ver]
//		Ver.01    2009/12/15  vs2005 �Ή�
//		Ver.02	  2011/11/10  ���i���񏈗��ɑΉ�
//
//	[����]
//		�E���̃N���X�́A���Ԃ񂻂̂܂܃C���X�^���X�����B�������́A�p�����ĕ��񏈗��̃��[�J�[�X���b�h���ꏏ�ɊǗ�����悤�ɂ���B
//		�E�Ŏ��ۂɃ��[�J�[�X���b�h�ŁA�ʏ��ThreadManager�ō���āAThreadEvent���R�[�����ꂽ��ňȉ��̂悤�ɂȂ�B
//
//	[�T���v��]
//		<�O��>
//		�EA�N���X �� �摜��荞�݃N���X
//		�EB�N���X �� �摜�������������X���b�h
//		�EC�N���X �� �摜����������̌��ʔ���N���X
//		�E�e.h�ɁAThreadSyncManager�̃C���X�^���X mcls ��ێ�
//		�E�v�f�� p
//		�Ƃ����ꍇ
//
//
//		<A�N���X�ł�>
//			mcls->AddToTail(p)
//
//		<B�N���X�ł�>
//			int ThreadFirst() {
//				HANDLE hArray[] = { mcls->GetExecQueEvSem(), *** }
//			}
//
//			void ThreadEvent(int nEventNo) {
//				// �K�{�̃L�[�擾����
//				HANDLE evEnd;
//				p = mcls->GetExecQueFromHead(&evEnd);
//
//				//�����ȏ���
//				p->***
//
//				// �K�{�̃G���h����
//				mcls->SetExecQueEnd(evEnd);
//			}
//
//		<C�N���X�ł�>
//			int ThreadFirst() {
//				HANDLE hArray[] = { mcls->GetEndQueEvSem(), *** }
//			}
//
//			void ThreadEvent(int nEventNo) {
//				// �L�[�擾����
//				p = mcls->GetEndQueFromHead();
//
//				// ��͂����ʂ�
//			}
//
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "..\..\KizuLib\Class\KizuMacro.h"								// �}�N���W
#include "..\..\KizuLib\Class\KizuFunction.h"							// �X�^�e�B�b�N���C�u�����W
#include "..\..\KizuLibMFC\Class\LogFileManager.h"						// ���O�Ǘ��N���X

#include "ThreadManager.h"												// ���[�J�[�X���b�h�}�l�[�W���[

using namespace KizuLib;

namespace KizuLib
{

	/// �X���b�h�ԓ����}�l�[�W���[�N���X
	template <class data_t>			// �� in,out���قȂ�ꍇ�́Avoid�^�ŗǂ�
	class ThreadSyncManager : public ThreadManager
	{

	//// �\����
	public:
		//=============================
		// �{�N���X�󂯓n���p�N���X
		//=============================
		//// �X���b�h�Ԏ󂯓n���L���[
		struct DELIVERY_QUE {
			HANDLE				evExecEnd;									// �X���b�h�ԓ����p�V�O�i��(�����N���X�ŏ����������ɃV�O�i���ɂ��邱��)
			data_t*				p;											// �󂯓n�����f�[�^
		};


	//// ��{
	public:
		ThreadSyncManager(const char* pThreadName="", int InSemCnt = 256, int OutSemCnt = 512, int nMaxWaitNum = 50);	// �R���X�g���N�^
		virtual ~ThreadSyncManager(void);									// �f�X�g���N�^

		void AttachNextSyncMgr(ThreadSyncManager<void>* cls) {mcls_pNextSyncMgr=cls;}		// ���i���񏈗� (�����O�Ɏ��s���邱��)

		void ReSetMaxCount()		{ myque_Sync.ReSetMaxCount(); myque_Exec.ReSetMaxCount(); myque_End.ReSetMaxCount(); };	// �u�ԍő�o�^����������



	//// �v�f���� (myque_Sync)
		BOOL		AddToTail(data_t * newData);							// �v�f�𖖔��Ɉ�ǉ�

		// �K�v�����ȃv���p�e�B
		int  GetSyncQueCount()const		{return myque_Sync.GetCount();};	// ���݂̓o�^�����擾
		int  GetSyncQueSemCount() const {return myque_Sync.GetSemCount();};	// �o�^�o����ő匏��
		int  GetSyncQueMaxCount() const {return myque_Sync.GetMaxCount();};	// �u�ԍő�o�^�����擾
		int  GetSyncQueFreeCount()const {return min(myque_Sync.GetFreeCount(),myque_Exec.GetFreeCount());}	// �c�o�^����



	//// �v�f���� (myque_Exec)
		HANDLE		GetExecQueEvSem() const { return myque_Exec.g_evSem; }	// ����X���b�h�Ŏg�p����L���[�҂��V�O�i��
		data_t *	GetExecQueFromHead(HANDLE* evExecEnd);					// �擪�v�f���o��(�҂��Ȃ��o�[�W����) (�Z�}�t�H������Ȃ����璍�ӂ��鎖)
		void*		GetExecQueFromHead_InOutChange();
		void		SetExecQueEnd(HANDLE evExecEnd) { SetEvent(evExecEnd);} // ���񏈗������ʒm (�K�{)
	
		// �K�v�����ȃv���p�e�B
		int  GetEexcQueCount()const		{return myque_Exec.GetCount();};	// ���݂̓o�^�����擾


	//// �v�f���� (myque_End)
		HANDLE		GetEndQueEvSem() const { return myque_End.g_evSem; }	// ����X���b�h�Ŏg�p����L���[�҂��V�O�i��
		data_t *	GetEndQueFromHead()	{ return myque_End.GetFromHead();}	// �擪�v�f���o��(�҂��Ȃ��o�[�W����) (�Z�}�t�H������Ȃ����璍�ӂ��鎖)

		// �K�v�����ȃv���p�e�B
		int  GetEndQueCount()const		{return myque_End.GetCount();};		// ���݂̓o�^�����擾
		int  GetEndQueMaxCount()const	{return myque_End.GetMaxCount();};	// �u�ԍő�o�^�����擾




	//// �p�����v���e�N�g�֐�
	protected:
		int			ThreadFirst();											// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
		int			ThreadLast();											// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
		void		ThreadEvent(int nEventNo);								// �X���b�h�C�x���g����
		


	//// ��������
	protected:
		ThreadQueue<DELIVERY_QUE>	myque_Sync;								// ����X���b�h�ԏ����ۏ�Ǘ��p�L���[
		ThreadQueue<DELIVERY_QUE>	myque_Exec;								// ���񏈗��p�L���[
		ThreadQueue<data_t>			myque_End;								// ���񏈗������������p�L���[(�L���[�C���O�������Ԓʂ�)
		
		ThreadSyncManager*			mcls_pNextSyncMgr;						// ���i���񏈗� (���Ɏ��s������񏈗��N���X�̃|�C���^)
	};



	// //////////////////////////////////////////////////////////////////////////////
	// �e���v���[�g�Ȃ̂Ńw�b�_�t�@�C���ɓ���Ă����K�v����
	// //////////////////////////////////////////////////////////////////////////////



	//------------------------------------------
	// �R���X�g���N�^
	// const char* pThreadName ���X���b�h��
	// int InSemCnt �L���[�̍ő�T�C�Y (�f�t�H���g256)
	// int OutSemCnt �L���[�̍ő�T�C�Y (�f�t�H���g512)
	// int nMaxWaitNum WaitForMultipleObjects�̍ő�҂��s��
	//------------------------------------------
	template <class data_t>
	ThreadSyncManager<data_t>::ThreadSyncManager(const char* pThreadName, int InSemCnt, int OutSemCnt, int nMaxWaitNum) :
	ThreadManager(pThreadName, nMaxWaitNum),
	myque_Sync(InSemCnt),
	myque_Exec(InSemCnt),
	myque_End(OutSemCnt),
	mcls_pNextSyncMgr(NULL)
	{

	}

	//------------------------------------------
	// �f�X�g���N�^
	//------------------------------------------
	template <class data_t>
	ThreadSyncManager<data_t>::~ThreadSyncManager()
	{
	}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �v�f���� (myque_Sync)

	//------------------------------------------
	// �v�f�𖖔��Ɉ�ǉ�
	// data_t * p �o�^����C���X�^���X
	// �߂�l ���A���
	//------------------------------------------
	template <class data_t>
	BOOL ThreadSyncManager<data_t>::AddToTail(data_t * newData)
	{
		//// �Ǘ��p�̍\���̂𐶐�
		DELIVERY_QUE* pDeli = new DELIVERY_QUE;
		pDeli->evExecEnd	= CreateEvent(NULL, FALSE, FALSE, NULL);		// ���񏈗������V�O�i��
		pDeli->p			= newData;

		//// �L���[�C���O
		// �ŏ��� �Ǘ��p
		if( ! myque_Sync.AddToTail(pDeli) ) {
			CloseHandle(pDeli->evExecEnd);
			delete pDeli;
			return FALSE;
		}
		// ���ɏ����p
		if( ! myque_Exec.AddToTail(pDeli) ) {
			CloseHandle(pDeli->evExecEnd);
			delete pDeli;
			return FALSE;
		}

		return TRUE;
	}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �v�f���� (myque_Exec)

	//------------------------------------------
	// �擪�v�f���o��(�҂��Ȃ��o�[�W����) (�Z�}�t�H������Ȃ����璍�ӂ��鎖)
	// HANDLE * evExecEnd �󂯓n���p�B�i���o�����ł͉������Ȃ��j
	// �߂�l �擾�����C���X�^���X
	//------------------------------------------
	template <class data_t>
	data_t * ThreadSyncManager<data_t>::GetExecQueFromHead(HANDLE * evExecEnd)
	{
		//// �f�[�^���o��
		DELIVERY_QUE* pDeli = myque_Exec.GetFromHead();
		if(NULL == pDeli) return NULL;

		//// ���Ƃ��Ƃ̌`�ɕ���
		* evExecEnd = pDeli->evExecEnd;
		return pDeli->p;
	}

	//------------------------------------------
	// �擪�v�f���o��(�҂��Ȃ��o�[�W����) (�Z�}�t�H������Ȃ����璍�ӂ��鎖) (IN,OUT�̕���ύX����ꍇ�p)
	// HANDLE * evExecEnd �󂯓n���p�B�i���o�����ł͉������Ȃ��j
	// �߂�l �擾�����C���X�^���X
	//------------------------------------------
	template <class data_t>
	void * ThreadSyncManager<data_t>::GetExecQueFromHead_InOutChange()
	{
		// �g���ق��Ńo���X����
		return myque_Exec.GetFromHead();
	}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �v�f���� (myque_Exec)



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X���b�h�ԓ������䏈�� (���C��)


	//------------------------------------------
	// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	//------------------------------------------
	template <class data_t>
	int ThreadSyncManager<data_t>::ThreadFirst()
	{
		//// �C�x���g�ݒ�
		HANDLE hArray[] = { myque_Sync.g_evSem
							};
		SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
		return 0;
	}
	//------------------------------------------
	// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
	//------------------------------------------
	template <class data_t>
	int ThreadSyncManager<data_t>::ThreadLast()
	{

		return 0;
	}


	//------------------------------------------
	// �X���b�h�C�x���g����
	// int nEventNo �C�x���gNo (0�I���W��)
	//------------------------------------------
	template <class data_t>
	void ThreadSyncManager<data_t>::ThreadEvent(int nEventNo)
	{
		DELIVERY_QUE* pDeli = NULL;

		////// �V�O�i���̏���
		enum {	EV_QUE = 0,				// ���[���X���b�g �ʒm (���[���͈�ԍŌ�ɂ��悤����)
		};

		////// �V�O�i����������
		switch (nEventNo) {

	//-----------------------------------------------------------------------------------------------
		case EV_QUE: 						// ���[���X���b�g �ʒm
			// ����X���b�h�ԏ����ۏ�
			if(true) {
				//// ����X���b�h�ŏ�������������܂őҋ@
				pDeli = myque_Sync.GetFromHead();
				WaitForSingleObject( pDeli->evExecEnd, INFINITE );


				////// ����X���b�h�ŏ���������������A�I�������L���[�ɃL���[�C���O
				//// ��Γo�^
				//if( ! myque_End.AddToTail(pDeli->p, 20, 15) ) {
				//	// ����
				//	SAFE_DELETE(pDeli->p);
				//	LOG(em_ERR), "[%s] �Ǘ��f�[�^�����I�Imyque_End�L���[�t��", my_sThreadName);
				//}
				
				//// ���i���񏈗��Ή�
				if(NULL == mcls_pNextSyncMgr) {
					// �]���ʂ�B������End�L���[�ɓo�^�B

					//// ����X���b�h�ŏ���������������A�I�������L���[�ɃL���[�C���O
					// ��Γo�^
					if( ! myque_End.AddToTail(pDeli->p, 20, 15) ) {
						// ����
						SAFE_DELETE(pDeli->p);
						LOG(em_ERR), "[%s] �Ǘ��f�[�^�����I�Imyque_End�L���[�t��", my_sThreadName);
					}
				
				} else {

					// ���i�p�ɁA���ڎ��̕��񏈗��ɓo�^
					if( NULL != pDeli->p ) {
						if( ! mcls_pNextSyncMgr->AddToTail(pDeli->p)) {
							// ����
							SAFE_DELETE(pDeli->p);
							LOG(em_ERR), "[%s] ���i���񏈗��󂯓n���ŃL���[�t��", my_sThreadName);
						}
					} else {
						LOG(em_WAR), "[%s] ���i���񏈗����ԂŃL���[�ԋp�̉\���L��", my_sThreadName);
					}
				}


				//// ��n��
				CloseHandle(pDeli->evExecEnd);
				SAFE_DELETE(pDeli);
			}
			break;

	//-----------------------------------------------------------------------------------------------
		default :
			// ���肦�Ȃ��I�I
			LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
			return;
		}
	}

}
