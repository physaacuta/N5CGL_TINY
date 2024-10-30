// *********************************************************************************
//	�����g���b�L���O����N���X
//	[Ver]
//		Ver.01    2007/03/14  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************
#pragma once

#pragma warning( disable : 4786 ) 

#include <list>

// �r���N���X �C���N���[�h
#include "..\KizuLib\Class\WorkThread.h"								// ���[�J�[�X���b�h�N���X
#include "..\KizuLib\Class\AutoLock.h"									// �N���e�B�J���Z�N�V�����Ǘ��N���X�w�b�_�[
#include "..\KizuLib\Class\ThreadQueue.h"								// �X���b�h�L���[�N���X�w�b�_�[

using namespace KizuLib;

class TrackingList : public WorkThread  
{
public:
	//// �g���b�L���O�̏I���敪
	enum EM_TR_END_STATE {
		TR_END = 0,														// �w�苗���o�߂����ꍇ
		TR_CANCEL,														// �r�����f�����ꍇ
		TR_STOP															// �X���b�g����~��
	};

	//// �g���b�L���O�`�F�b�N�p�\���� (�X���b�h�L���[���o����ɉ���K�{)
	struct TR_INF {
		// �g���b�L���O�ŕK�v�ȏ��
		long					nDist;									// ��mm��ɒʒm? [mm] (�Œ�)
		double					dDistNow;								// �C���X�^���X�������̋��� [mm]
		double					dDistEnd;								// �C�x���g�������̋��� [mm]
		double					dChangeLengthMax;						// �R�C���ؑ֓��Ń`�F�b�N������0�N���A���ꂽ�ꍇ�̃Q�^�͂�����

		// �X���b�h�L���[�n���Ŏ擾������ŕK�v�ɂȂ鍀��
		EM_TR_END_STATE			emEndState;								// �X���b�h�L���[�ɓo�^�����敪
		int						nKey;									// �L�[��� (0:�L�[�`�F�b�N�����̈Ӗ��ƂȂ�)
		long					nAddr1;									// ���[�U�[�p���[�N�G���A
		long					nAddr2;									// ���[�U�[�p���[�N�G���A
		long*					pAddr;									// ���[�U�[�p���[�N�G���A (�ʏ�N���X�̃|�C���^���Z�b�g)(���g�p����NULL)
	};


public:
	TrackingList(const double* length);
	virtual ~TrackingList();

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(); };								// �X���b�h�I��
	
	// �󂯓n���L���[
	ThreadQueue<TR_INF>			gque_Deli;								// �󂯓n���L���[ (�X���b�h�L���[���o����ɉ���K�{)


	// �O���A�N�Z�X
	void		TrackingAdd_Len(DWORD len, int nKey=0, long nAddr1=0, long nAddr2=0, long const* pAddr=NULL);	// �g���b�L���O�A�C�e�� ���`�F�b�N���āA�w�苗����ʔ��Ă����� TR_END �ŃL���[�C���O
	void		TrackingAdd_Dist(DWORD dist, int nKey=0, long nAddr1=0, long nAddr2=0, long const* pAddr=NULL);	// �g���b�L���O�A�C�e�� ���`�F�b�N���āA�w�苗����ʔ��Ă����� TR_END �ŃL���[�C���O
	void		TrackingCancel(double dLength, int nKey=0);				// ������~ (������R�[������� TR_CANCEL �ŃL���[�C���O����)
	void		TrackingCancelKey(int nKey);							// ������~ �w��L�[�݂̂��L�����Z��
	void		TrackingStop();											// �g���b�L���O�A�C�e�� ��S�ĉ�����L���[�C���O����
	void		TrackingLengthChange(double dOldLen);					// �R�C���ؑ֓���������

	bool		GetCloneKeyItem(int nKey, TR_INF* inf);					// �w��L�[No�̃A�C�e���̃N���[�����擾���� (�����m�肽���ꍇ��)
	double		CheckExecDist(TR_INF const* inf) const					// �㉽���Ŏ��s�H
		{
			return (*m_dLength - (inf->dDistNow - inf->dChangeLengthMax));
		}


	void		QueAdd(DWORD dist, int nKey=0, long nAddr1=0, long nAddr2=0, long const* pAddr=NULL);			// �V�K�L���[ �A�C�e����ǉ����� (�g���b�L���O�����ɁA�����ɃL���[�C���O)
	void		QueAllFree();											// ���ׂẴL���[���J��



private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h
	void		TrackingCheck();										// �g���b�L���O�A�C�e�� ���`�F�b�N���āA�w�苗����ʔ��Ă�����V�O�i�����s����


	// �g���b�L���O�ɕK�v�ȏ��
	const double*				m_dLength;								// �`�F�b�N����l [mm]
	std::list<TR_INF*>			m_List;									// �g���b�L���O�A�C�e��


	// ���̑��������
	CRITICAL_SECTION			m_csLock;								// �N���e�B�J���Z�N�V����
	
};


