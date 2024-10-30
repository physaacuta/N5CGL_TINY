#pragma once
#include "kizudeletemanager.h"

class DeleTable: public KizuDeleteManager
{
public:
	DeleTable(int id, int* nWaitTime, HANDLE evEndCycle, HANDLE evEndSizeMin, CString cSession, CString cOdbcSession);
	virtual ~DeleTable(void);


private:
    // //////////////////////////////////////////////////
    // �e�N���X ���\�b�h�̎���
    // //////////////////////////////////////////////////
	void ExecCycle();													// ��������s���\�b�h
	void ExecSizeMin();													// DB�e�ʒቺ���s���\�b�h
	void ExecFast();													// ������s���\�b�h


    // //////////////////////////////////////////////////
    // �Ǝ� ���\�b�h
    // //////////////////////////////////////////////////
	void Delete_BackUp(const char* cKizukenNo);							// �o�b�N�A�b�v�폜


    // //////////////////////////////////////////////////
    // �����艺������V�X�e�����ɕύX
    // //////////////////////////////////////////////////
	int						m_nDelTblCoil;								// �폜�e�[�u������
	int						m_nDelTblTime;								// �폜�e�[�u������	
	int						m_nDelTblCnt;								// �폜�e�[�u������	
	DEL_TBL*				m_pDelTblCoil;								// �폜�e�[�u��		(�R�C�����тɕR�t�����)
	DEL_TBL*				m_pDelTblTime;								// �폜�e�[�u��		(���t�ɕR�t�����)
	DEL_TBL_CNT*			m_pDelTblCnt;								// �폜�e�[�u��		(�ۑ�������ɕR�t�����)
};
