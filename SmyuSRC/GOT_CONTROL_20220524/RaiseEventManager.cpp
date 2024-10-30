#include "StdAfx.h"
#include "RaiseEventManager.h"

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
RaiseEventManager::RaiseEventManager(void) :
ThreadManager("RaiseEvent")
{
	//// �n���h������
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
RaiseEventManager::~RaiseEventManager(void)
{
	//// �n���h�����
}

//------------------------------------------
// �C�x���g����
// EM_DIV_KIND emEvent �C�x���g���
// int nEventSub �T�u���
// BYTE* pEvent �\���̃|�C���^
//------------------------------------------
void RaiseEventManager::RaiseEvents(EM_DIV_KIND emEvent, int nEventSub, BYTE* pEvent)
{
	////// �V�i���I��������
	switch (emEvent) {
//-----------------------------------------------------------------------------------------------
	case DIV_KIND_PLG:										
		if(true) {
			TYP_EVENT_PLG_SPEED* p = (TYP_EVENT_PLG_SPEED*)pEvent;
			
			mcls_pPlg->SetSaveData().nSpeed[p->nCh] = p->nMpm;
			// �ύX�ʒm�B (True��n���Ă����΁A�֐����ŏ���ɒ�~�J�n�𔻒f���Ă����)			
			mcls_pPlg->SetTimer_SpeedCycle(p->nCh, true);
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_PI:										
		if(0==nEventSub) {
			TYP_EVENT_PI_BIT* p = (TYP_EVENT_PI_BIT*)pEvent;
			
			mcls_pPio->SetValPi( p->nBood, p->nPort, p->nBit, __BOOL p->nMode); 


		} else if(1==nEventSub) {
			TYP_EVENT_PI_PORT* p = (TYP_EVENT_PI_PORT*)pEvent;

			mcls_pPio->SetValPi( p->nBood, p->nPort, p->nBitSet); 


		} else if(2==nEventSub) {
			TYP_EVENT_PI_BITPULS* p = (TYP_EVENT_PI_BITPULS*)pEvent;

			// ��
		
		}
		return;

////-----------------------------------------------------------------------------------------------
//	case DIV_KIND_PO:										
//		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_MEM:										
		if(true) {
			TYP_EVENT_MEM_WRITE* p = (TYP_EVENT_MEM_WRITE*)pEvent;
			
			// ���L�������A�N�Z�X
			HANDLE h;
			BYTE*  t;
			mem_connect(&h, (LPVOID *)t, p->cTblName);
			if(h == NULL) {
				LOG(em_ERR), "[%s] ���L������ �A�N�Z�X���s [%s]", my_sThreadName, p->cTblName);
				return;
			}

			// �o�C�i���t�@�C���Z�b�g
			std::ifstream   is;
			is.open(p->cWriteDataPath, std::ios::binary);
			is.read( (char*)&t[p->nIndex], 1024*1024*1024);
			is.close();

			// ���L�������J��
			mem_close(&h, (LPVOID *)t);
		}
		return;

//-----------------------------------------------------------------------------------------------
	case DIV_KIND_MAIL:				
		if(true) {
			TYP_EVENT_MAIL_SEND* p = (TYP_EVENT_MAIL_SEND*)pEvent;

			send_mail(p->cTaskName, p->cPcName, &p->que);
		}
		return;

////-----------------------------------------------------------------------------------------------
//	case DIV_KIND_TIME:										
//		return;

//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] �s��=%d", my_sThreadName, emEvent );
		return;
	}
}

