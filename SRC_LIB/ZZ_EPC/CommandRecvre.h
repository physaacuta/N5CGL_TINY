#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X

// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLib\Class\WorkThread.h"						// ���[�J�[�X���b�h�N���X
#include "..\..\include\ToHtCommon.h"									// ��������p�̒�`�w�b�_

// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCcommand.h"								// ����R�}���h�̒�`
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`


#include "DataSender.h"

using namespace KizuLib;


class CommandRecvre : public WorkThread
{
public:
	CommandRecvre(void);
	virtual ~CommandRecvre(void);


	void Start() { WorkThread::Start(MainThread); };					// �X���b�h�J�n
	void Stop()  { WorkThread::Stop(); };								// �X���b�h�I��
	void SetLogMgr(LogFileManager* pLog) { mcls_pLog = pLog; };			// ���O�t�@�C���Z�b�g
	
	void SetDataSender(DataSender* pCls) { mcls_pData = pCls; };			//


	int SendCommand(LPVOID senddata, DWORD sendsize);

	void SetExp(int cam, float* exp) { for (int ii = 0; ii<3; ii++)m_exptime[ii][cam] = exp[ii]; }
	void SetGain(int cam, float* gain) { for (int ii = 0; ii<3; ii++)m_gain[ii][cam] = gain[ii]; }
	void SetTemp(int cam, float* temp) { m_temp[cam] = temp[cam]; }
	void SetVolt(int cam, float* volt) { m_volt[cam] = volt[cam]; }

	float GetExp(int color, int cam) { return m_exptime[color][cam]; }
	float GetGain(int color, int cam) { return m_gain[color][cam]; }
	float GetTemp(int cam) { return m_temp[cam]; }
	float GetVolt(int cam) { return m_volt[cam]; }

private :
	static UINT WINAPI MainThread(LPVOID param);						// ���C���X���b�h

	void GetCommandExec(int command, BYTE* pBuf);									// �R�}���h����

	//// �e�C���X�^���X
	LogFileManager*			mcls_pLog;									// LogFileManager�X���b�h�C���X�^���X
	DataSender*				mcls_pData;									// �f�[�^���M

	HANDLE					m_hPipe;									// �p�C�v�{��
	OVERLAPPED				m_ovlSend;									// ��M�p�񓯊�IO
	int						m_RetCode;									// ���^�[���R�[�h�\����
	NCL_ANSWER_QUERY_STATE	m_RetState;									// ���^�[���R�[�h�\����(NCL_QUERY_STATUS)

	float					m_exptime[3][NCL_ncam_max];					// �F�ʘI������
	float					m_gain[3][NCL_ncam_max];					// �F�ʃQ�C��
	float					m_temp[NCL_ncam_max];						// �J�������x
	float					m_volt[NCL_ncam_max];						// �J�����d��
};
