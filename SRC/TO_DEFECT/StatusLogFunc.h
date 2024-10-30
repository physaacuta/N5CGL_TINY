//=====================================================================
// �X�e�[�^�X���O��o�^����N���X
//	�y�S���C���z
//		�E��{�I�Ɋ��S���p�\�Ȃ悤�ɍ��
//=====================================================================
#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\FuncBase.h"					// ���W���[���̊��N���X
#include "..\..\Library\KizuLibMFC\Class\DivNameManager.h"				// DIV�萔 ���̎擾�N���X

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h

// ���[�J���̃N���X
#include "MainConst.h"

using namespace KizuLib;
class StatusLogFunc : public FuncBase
{
	//=========================================
	// �X�e�[�^�X��ԃ��O
public:
	struct TYP_STATUS_LOG {
		struct CHECK_DATA {
			EM_DIV_KADOU			emKadou;						// �ғ����
			EM_DIV_KENSA			emKensa[NUM_MEN];				// �������
			EM_DIV_KIKI				emKiki[NUM_MEN];				// �@����
			char					cMsg[256];						// �K�C�_���X
		} Data;
		// �g����
		char					cKizukenNo[SIZE_KIZUKEN_NO];			// �r���Ǘ�No
		char					cCoilNo[32];							// �R�C��No
	};	

public:
	StatusLogFunc(void);
	virtual ~StatusLogFunc(void);

	void SetQueAddSQL(ThreadQueue<DELIVERY_DB>* pQue) { mque_pAddSQLData=pQue; }	// DB�o�^�p�̃L���[// 


	void Alloc();												// �����ݒ�
	void CheckStatusLog(TYP_STATUS_LOG* pTyp);					// �X�e�[�^�X���`�F�b�N

private:
	void InitAutoNo();											// AutoNo�ɕK�v�ȃf�[�^�������Z�b�g����
	void RefrashAutoNo(int nDay);								// ���t���`�F�b�N���āA�����ς��΁A�I�[�gNo�����Z�b�g����B


private:
	//// �󂯓n���L���[
	ThreadQueue<DELIVERY_DB>*		mque_pAddSQLData;			// DB�N���X �󂯓n���f�[�^�p�L���[(��Ă�<DBManager::DeliveryDB> ���� �R���p�C�����ʂ��̂�DELIVERY_DB�ɕύX)


	//// ����
	TYP_STATUS_LOG			m_oldStatusLog;					// �O����
	

	// ��L�[�֘A
	//  �E��L�[�P: �o�^���t
	//  �E��L�[�Q: m_nAutoNo*10 + m_nODBCID
	int						m_nDay;										// ���ݓ�
	int						m_nAutoNo;									// ��L�[�̈�ƂȂ�A��
																			//�E��{�I�ɂ́A�����ς��^�C�~���O�Ń��Z�b�g(1)����
																			//�E�����グ���^�C�~���O�ł́A�ŐV���t��AutoNo�̒l�Ƃ���
	int						m_nOdbcID;									// ODBC�̐ݒ�ʒu

};
