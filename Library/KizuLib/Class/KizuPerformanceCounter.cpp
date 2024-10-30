// KizuPerformanceCounter.cpp: KizuPerformanceCounter �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KizuPerformanceCounter.h"
#include <crtdbg.h>

using namespace KizuLib;

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////
//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
KizuPerformanceCounter::KizuPerformanceCounter()
{
	// ���g���擾
	QueryPerformanceFrequency((LARGE_INTEGER *)&m_pf);
	m_pc = 0;

	// ���ʗp
	m_ave = 0.0;								// ���ώ���	[ms] 
	m_min = 9999.0;								// �ŏ��l
	m_max = 0.0;								// �ő�l
	m_bFast = false;
	m_Cycle = 0.0;								// �Ԋu [ms]

	// �X�g�b�v�E�H�b�`�@�\
	m_nIndex = 0;
	memset(m_timestamp, 0x00, sizeof(m_timestamp));
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
KizuPerformanceCounter::~KizuPerformanceCounter()
{

}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 2�_�Ԏ��ԎZ�o�p

//------------------------------------------
// 2�_�Ԏ��ԎZ�o�p ������
//------------------------------------------
void KizuPerformanceCounter::InitAve()
{
	m_pc = 0;

	m_ave = 0.0;								// ���ώ���	[ms] 
	m_min = 9999.0;								// �ŏ��l
	m_max = 0.0;								// �ő�l
	m_bFast = false;
	m_Cycle = 0.0;								// �Ԋu [ms]
}
//------------------------------------------
// ���ԎZ�o�J�n
//------------------------------------------
void KizuPerformanceCounter::CounterStart()
{
	QueryPerformanceCounter((LARGE_INTEGER *)&m_pc);
}
//------------------------------------------
// ���ԎZ�o�I��
//------------------------------------------
void KizuPerformanceCounter::CounterEnd()
{
	// ���ԍ��Z�o
	__int64 pc2;
	QueryPerformanceCounter((LARGE_INTEGER *)&pc2);
	m_Cycle = (double)(pc2 - m_pc)/m_pf * 1000.0;	// [s] �� [ms] 
	
	// �ő�ŏ����ԎZ�o
	if ( m_min > m_Cycle ) m_min = m_Cycle;
	if ( m_max < m_Cycle ) m_max = m_Cycle;

	// ���ώ��ԎZ�o (�ȈՎZ�o)
	if(m_bFast) {
		m_ave = m_ave*0.8 + m_Cycle*0.2;
	} else{
		m_ave = m_Cycle;
		m_bFast = true;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �����Z�o�p

//------------------------------------------
// �����Z�o�p ������
//------------------------------------------
void KizuPerformanceCounter::InitCycle()
{
	m_pc = 0;
	m_Cycle = 0.0;								// �Ԋu [ms]

	m_ave = 0.0;								// ���ώ���	[ms] 
	m_min = 9999.0;								// �ŏ��l
	m_max = 0.0;								// �ő�l
	m_bFast = false;
}
//------------------------------------------
// �Ԋu�Z�o
//------------------------------------------
void KizuPerformanceCounter::CounterCycle()
{
	// ���ԍ��Z�o
	__int64 pc2;
	QueryPerformanceCounter((LARGE_INTEGER *)&pc2);
	m_Cycle = (double)(pc2 - m_pc) / m_pf * 1000.0;	// [s] �� [ms] 
	m_pc = pc2;
	
	// �ő�ŏ����ԎZ�o
	if ( m_min > m_Cycle ) m_min = m_Cycle;
	if ( m_max < m_Cycle ) m_max = m_Cycle;

	// ���ώ��ԎZ�o
	if(m_bFast) {
		m_ave = m_ave*0.8 + m_Cycle*0.2;
	} else{
		m_ave = m_Cycle;
		m_bFast = true;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �X�g�b�v�E�H�b�`�p
//------------------------------------------
// �O�����狭���I�Ɏ��ԋL��
// int no �ʒu (0�I���W��)
// __int64 time �L������
//------------------------------------------
void KizuPerformanceCounter::PutTimestamp(int no, __int64 time)
{
	_ASSERT(0 <= no && mcMaxTimestamp > no);

	m_timestamp[no] = time;
}

//------------------------------------------
// ���ԋL��
// int no �ʒu (0�I���W��)
// bool bChangeIndex=true Index�ύX��true
//------------------------------------------
void KizuPerformanceCounter::PutTimestamp(int no, bool bChangeIndex)
{
	_ASSERT(0 <= no && mcMaxTimestamp > no);

	QueryPerformanceCounter((LARGE_INTEGER *)&m_timestamp[no]);
	
	// �C���f�b�N�X�̕ύX
	if(bChangeIndex) {
		m_nIndex = no+1;
		if(m_nIndex>=mcMaxTimestamp) m_nIndex = 0;
	}
}
//------------------------------------------
// ���ԋL��
//------------------------------------------
void KizuPerformanceCounter::PutTimestamp()
{
	PutTimestamp(m_nIndex);
}
//------------------------------------------
// ���Ԏ擾
// int no �ʒu (0�I���W��)
// �߂�l ���� [ms]
//------------------------------------------
double KizuPerformanceCounter::GetTimestamp(int no)
{
	_ASSERT(0 <= no && mcMaxTimestamp > no);

	return (double)m_timestamp[no] / m_pf * 1000.0;	// [s] �� [ms] 
}
//------------------------------------------
// ���ԍ��擾
// int no2 �ʒu (0�I���W��)
// int no1 �ʒu (0�I���W��)
// �߂�l ���� [ms]
//------------------------------------------
double KizuPerformanceCounter::GetTimestamp(int no2, int no1)
{
	_ASSERT(0 <= no1 && mcMaxTimestamp > no1);
	_ASSERT(0 <= no2 && mcMaxTimestamp > no2);

	return GetTimestamp(no2) - GetTimestamp(no1);
}
