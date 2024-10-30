#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

#include "..\..\Include\KizuSmyu.h"										// GOT�C���N���[�h

// STL
#include <vector>														// ���X�g�p

// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[


class CsvReader :	public ThreadManager
{
//// ���J�֐�
public:
	CsvReader(void);
	virtual ~CsvReader(void);


	//// CSV����
	bool CreatEvents(CString sPath, std::vector<TYP_SINA_INF*>* pEvent);	// �V�i���I���


//// �����o�[�֐�
private:
	int ThreadFirst() {return 0;};										// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast() {return 0;};										// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo) {return;};							// �X���b�h�C�x���g����
	

	bool ReadCsv(CString sPath, std::vector<CString>* pRow);		// CSV�t�@�C���ǂݍ���
	bool CsvToEvent(std::vector<CString>* pRow, std::vector<TYP_SINA_INF*>* pEvent);
	bool GetColumn(CString& sVal, int& curPos, CString& sToken);	// �P�s�̃f�[�^���� , �P�ʂ̕�������擾


	//================================================
	// ����������
	//// �����쓮�V�i���I
	void RaiseData_Plg_Pos(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	//// �C�x���g�쓮�V�i���I
	void RaiseData_Evt_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void RaiseData_Evt_Po(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	//// ���ԋ쓮�V�i���I
	void RaiseData_Time_Cycle(CString& sVal, int& curPos, TYP_SINA_INF* pInf);

	//================================================
	// �C�x���g��
	void EventData_Plg(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Pi(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Mem(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
};
