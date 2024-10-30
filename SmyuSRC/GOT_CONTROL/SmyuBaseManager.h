#pragma once

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuODBC\OdbcBase.h"							// ODBC����N���X

#include "..\..\Include\KizuSmyu.h"										// GOT�C���N���[�h

// STL
#include <vector>														// ���X�g�p


// ���[�J��
#include "MainConst.h"		
#include "PlgControlManager.h"
#include "PioControlManager.h"

class SmyuBaseManager :	public ThreadManager
{

//// ���J�֐�
public:
	SmyuBaseManager(const char* pThreadName, int nMyID);
	virtual ~SmyuBaseManager(void);

	void SetPlgMgr(PlgControlManager* pCls) { mcls_pPlg = pCls;};		// �X�e�[�^�X����N���X�Z�b�g
	void SetPioMgr(PioControlManager* pCls) { mcls_pPio = pCls;};		// �X�e�[�^�X����N���X�Z�b�g

	bool SetCsvFilePath(LPCSTR sPath);									// �V�i���I�ǂݍ���


	virtual void SmyuStart() {};
	virtual void SmyuStop() {};



protected:
	void RaiseEvents(EM_DIV_KIND emEvent, int nEventSub, BYTE* pEvent);	// �C�x���g����



//// �����o�[�֐�
private:

	//================================================
	// �V�i���I������
	bool ReadCsv(LPCSTR sPath, std::vector<CString>* pRow);			// CSV�t�@�C���ǂݍ���
	bool CsvToEvent(std::vector<CString>* pRow, std::vector<TYP_SINA_INF*>* pEvent);
	bool GetColumn(CString& sVal, int& curPos, CString& sToken);	// �P�s�̃f�[�^���� , �P�ʂ̕�������擾

	//-----------------------------
	// ����������
	/// �����쓮�V�i���I
	void RaiseData_Plg_Pos(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	/// �C�x���g�쓮�V�i���I
	void RaiseData_Evt_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void RaiseData_Evt_Po(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	/// ���ԋ쓮�V�i���I
	void RaiseData_Time_Cycle(CString& sVal, int& curPos, TYP_SINA_INF* pInf);

	//-----------------------------
	// �C�x���g��
	void EventData_Plg(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Pi(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Mem(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Mail(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Time(CString& sVal, int& curPos, TYP_SINA_INF* pInf);
	void EventData_Zz(CString& sVal, int& curPos, TYP_SINA_INF* pInf);

//// �����o�[�ϐ�
protected:

	//=========================================
	// �e�C���X�^���X
	PlgControlManager*			mcls_pPlg;								// PLG�����Ǘ��N���X
	PioControlManager*			mcls_pPio;								// PIO�Ǘ��N���X


	//=========================================
	// �V�i���I
	std::vector<TYP_SINA_INF*>	m_lstStory;							// �V�i���I


	//=========================================
	// ���̑�
	int							m_nMyID;								// ������ID (0�I���W��)
};
