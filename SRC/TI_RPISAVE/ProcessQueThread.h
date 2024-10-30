// *********************************************************************************
//	Proc�֌W�̃x�[�X�N���X
//	[Ver]
//		Ver.01    2013/07/09	����
//
//	[����]
// *********************************************************************************

#pragma once

// �r���N���X �C���N���[�h
#include "ProcessQueImpl.h"												// ��

/*/ �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
#define new DEBUG_NEW
#endif
//*/// <<<------------------------


//======================================================================
// �v���Z�X�֌W�X���b�h�̃x�[�X�N���X
template <class Tin, class Tout >			// ����Que, �o��Que
class ProcessQueThread 
{
//// �K�{
protected:
	IQueue<Tin > *mque_pIn;						// ����Que
	IQueue<Tout> *mque_pOut;					// �o��Que


//// ���J�֐�
public:
	//------------------------------------------
	// �R���X�g���N�^
	//------------------------------------------
	ProcessQueThread()
	{
		mque_pIn  = NULL;
		mque_pOut = NULL;
	}

	//=========================================
	// ��n�L���[�o�^
	virtual void SetQueIn ( IQueue<Tin >  *pQueIn ) { mque_pIn  = pQueIn; }		// ����Que�Z�b�g
	virtual void SetQueOut( IQueue<Tout>  *pQueOut) { mque_pOut = pQueOut;}		// �o��Que�Z�b�g


//// �����o�[����
protected:
	//=========================================
	// ���z�֐�

	//------------------------------------------
	// ������
	// Tin* pIn		����Que�f�[�^
	// �߂�l		NULL:�ԋp���Ȃ� �o��Que�f�[�^:���̃X���b�h�֕ԋp����ꍇ  
	//------------------------------------------
	virtual Tout* OnAddNewItem( Tin* pIn ) = 0;


	//------------------------------------------
	// Que�f�[�^��t���̃f�t�H���g����
	//------------------------------------------
	virtual void ExecQue() {
		HANDLE	evEnd = NULL;				// ���񏈗��p

		// ����Que �擾
		Tin* pInData = mque_pIn->GetFromHead(&evEnd);

		// �p������s
		Tout*	pOutData = OnAddNewItem(pInData);

		// �L���[�ԋp
		// evEnd ������ꍇ�́A���񏈗��Ȃ̂ŁA��΂Ɋ����ʒm�𑗂�
		// pOutData ��NULL�̏ꍇ�́A���̏����ňꎞ���f�Ȃ̂ŁA���̏����ւ̒ʒm�͂��Ȃ�
		if(NULL != evEnd || NULL != pOutData) mque_pOut->AddToTail(pOutData, evEnd);
	}
};


//======================================================================
// �X���b�h�ԃL���[�ڑ�		�� ��������ǂ��܂�g��Ȃ��B
// �L���[��K�v�Ƃ��Ă���X���b�h�Q���A�L���[�ɂ��ڑ�����B
// FromThread -> pQue -> ToThread
template< class T1, class T2, class T3>		// �O�����A��nQue�A�㏈��
void ConnectQueue( ProcessQueThread<T1, T2> *pFromThread = NULL, 
				   IQueue<T2> *pQue = NULL,
				   ProcessQueThread<T2, T3> *pToThread = NULL )
{
	_ASSERT(pQue);
	if( NULL != pFromThread) pFromThread->SetQueOut( pQue );				// �O�����X���b�h�̏o��Que�ɃZ�b�g
	if( NULL != pToThread)	 pToThread->SetQueIn( pQue );					// �㏈���X���b�h�̓���Que�ɃZ�b�g
}

// pQue -> ToThread
template< class T2, class T3>		// ��nQue�A�㏈��
void ConnectQueueIn( IQueue<T2> *pQue = NULL,
					   ProcessQueThread<T2, T3> *pToThread = NULL )
{
	_ASSERT(pQue);
	if( NULL != pToThread)	 pToThread->SetQueIn( pQue );					// �㏈���X���b�h�̓���Que�ɃZ�b�g
}
// FromThread -> pQue
template< class T1, class T2>		// �O�����A��nQue
void ConnectQueueOut( ProcessQueThread<T1, T2> *pFromThread = NULL, 
					   IQueue<T2> *pQue = NULL)
{
	_ASSERT(pQue);
	if( NULL != pFromThread) pFromThread->SetQueOut( pQue );				// �O�����X���b�h�̏o��Que�ɃZ�b�g
}



