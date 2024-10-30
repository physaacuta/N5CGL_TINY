// *********************************************************************************
//	���ԑ���N���X
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή� (�R���X�g���N�^�ŃT�C�Y�w��ɕύX)
//
//	[����]
//		
// *********************************************************************************

#pragma once

#include <windows.h>

namespace KizuLib
{
	class KizuPerformanceCounter  
	{
	// �����o�[�萔
	private:
		static const int		mcMaxTimestamp = 64;						// �ő�^�C���X�^���v


	// ���J
	public:
		KizuPerformanceCounter();
		virtual ~KizuPerformanceCounter();
		

		//// ���ԍ�
		// 2�_�Ԏ��ԎZ�o
		void	InitAve();													// 2�_�Ԏ��ԎZ�o�p ������
		void	CounterStart();												// ���ԎZ�o�J�n
		void	CounterEnd();												// ���ԎZ�o�I��
		
		// �������ԎZ�o
		void	InitCycle();												// �����Z�o�p ������
		void	CounterCycle();												// �Ԋu�Z�o
		
		// �O���A�N�Z�X�p
		double	GetAveTime()		const { return m_ave; }					// ���ώ���	[ms] 
		double	GetCycleTime()		const { return m_Cycle; }				// �Ԋu		[ms] 
		double	GetMinTime()		const { return m_min; }					// �ŏ����� [ms]
		double	GetMaxTime()		const { return m_max; }					// �ő厞�� [ms]


		//// �X�g�b�v�E�H�b�`�@�\
		void	PutTimestamp(int no, __int64 time);							// �O�����狭���I�Ɏ��ԋL��
		void	PutTimestamp(int no, bool bChangeIndex=true);				// ���ԋL��
		void	PutTimestamp();												// ���ԋL��
		double	GetTimestamp(int no);										// ���Ԏ擾 [ms]
		double	GetTimestamp(int no2, int no1);								// ���ԍ��擾 [ms]
		__int64	GetRawTimestamp(int no) const { return m_timestamp[no];};	// ������

	private:
		__int64					m_pf, m_pc;									// PC���g��, ����_

		// ����
		double					m_min;										// �ŏ��l	[ms]
		double					m_max;										// �ő�l	[ms]
		double					m_ave;										// ���ώ���	[ms] 
		bool					m_bFast;									// �����s��True
		double					m_Cycle;									// �Ԋu [ms]
		

		// �X�g�b�v�E�H�b�`�@�\
		__int64					m_timestamp[mcMaxTimestamp];				// �^�C���X�^���v
		int						m_nIndex;									// �^�C���X�^���v�C���f�b�N�X
	};
};
