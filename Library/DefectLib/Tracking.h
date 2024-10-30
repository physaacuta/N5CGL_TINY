// *********************************************************************************
//	�g���b�L���O����N���X
//	[Ver]
//		Ver.01    2007/03/14  vs2005 �Ή�
//		Ver.02	  2009/02/26  ���[�J�[�X���b�h�̖����~�J�n�́A�����B�S�ʍ�蒼���B
//
//	[����]
//		m_evTrackingEnd, m_evTrackingCancel �� �V�O�i���ƂȂ�ƁA�����I�Ƀg���b�L���O����~���܂��B
// *********************************************************************************
#pragma once

// �r���N���X �C���N���[�h
#include "..\KizuLib\Class\WorkThread.h"								// ���[�J�[�X���b�h�N���X

using namespace KizuLib;

class Tracking : public WorkThread
{

private:
	//// ���[�J���萔
	static const int PARAM_TIME_TRACKING		= 20;					// �g���b�L���O�����Ǎ��������s �^�C�}���� [ms]

public:
	Tracking(DWORD dist, const double* length, const LPHANDLE evEnd, const LPHANDLE evCancel = NULL);	// �R���X�g���N�^
	virtual ~Tracking();																				// �f�X�g���N�^

	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(); };								// �X���b�h�I��


	//// �g���b�L���O����
	BOOL TrackingStart();												// �g���b�L���O���J�n
	void TrackingStop();												// �g���b�L���O��������~ (�g���b�L���O���~�B�V�O�i�����s����Ȃ��B=Exe��~���Ȃǂɔ��s)
	void TrackingCancel() { if(m_bTkRun)  SetEvent(m_evTkCancelStop); }	// �g���b�L���O�𒆒f (���f�V�O�i�����s��A�g���b�L���O��~)


	//// ���̑��������
	long*			g_pAddr;											// �󂯓n�����

	//// �C�x���g�V�O�i��
	HANDLE			GetEv_TrEnd()    const { return (const HANDLE)*m_evTrackingEnd; }		// �g���b�L���O�����V�O�i��
	HANDLE			GetEv_TrCancel() const { return (const HANDLE)*m_evTrackingCancel; }	// �g���b�L���O���f�V�O�i��


	//// �O���A�N�Z�X
	// Get
	double			GetDist() const {return *m_dLength - (m_dDistNow - m_dChangeLengthMax );}	// �㉽mm
	double			GetDistNow() const {return m_dDistNow;}				// �J�n���̋��� mm
	double			GetLength() const {return *m_dLength;}				// ����mm
	bool			IsRunning() const {return m_bTkRun;}				// ���s���H
	// Set
	void			ChangeDist(DWORD dist)	{ m_nDist = dist; }			// �㉽mm ��ɒʒm?�������ύX
	bool			TrackingLengthChange(double dOldLen) 				// �R�C���ؑ֓���������
						{ 
							if(0.0 <= GetDist()) return false;
							m_dChangeLengthMax  = dOldLen; 
							return true;
						}


private:
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	
	// �g���b�L���O�ɕK�v�ȎҒB
	long			m_nDist;											// ��mm��ɒʒm? [mm]
	double			m_dDistNow;											// �g���b�L���O�J�n���̋��� [mm]
	double			m_dChangeLengthMax;									// �R�C���ؑ֓��Ń`�F�b�N������0�N���A���ꂽ�ꍇ�̃Q�^�͂�����

	const double*	m_dLength;											// �`�F�b�N����l [mm]
	const HANDLE*	m_evTrackingEnd;									// �w�苗���o�ߌ� ON����n���h��
	const HANDLE*	m_evTrackingCancel;									// �r�����f�����ꍇ�� ON����n���h��


	// �����V�O�i��
	HANDLE			m_evTkCancelStop;									// �g���b�L���O�𒆒f (���f�V�O�i�����s��A�g���b�L���O��~)
	HANDLE			m_evTkTimer;										// �g���b�L���O�^�C�}�[

	// ���̑��������
	bool			m_bTkRun;											// �g���b�L���O���s���� True
};
