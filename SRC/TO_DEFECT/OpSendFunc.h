// *********************************************************************************
//	������Ɠ���ȃ��W���[���Q
//	[����]
//		�e�C���X�^���X�́Astatic�� .cpp�Ɏ�����
// *********************************************************************************

#pragma once

// ���Œ�
#include "..\..\Include\LineCommon.h"									// �C���N���[�h
#include "..\..\Include\ToHtCommon.h"									// ���������p�C���N���[�h

#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W

// EPC�C���N���[�h
//#include "..\..\Include\EPC\NCsmemData.h"
//#include "..\..\Include\EPC\NCattr.h"
#include "..\..\Include\EPC\NCL2DETECT_if.h"							// I/F��`
#include "..\..\Include\EPC\NCL2attr.h"									// �����ʒ�`

// ���[�J��
#include "MainConst.h"

using namespace KizuLib;

class OpSendFunc
{
public:
	OpSendFunc(void);
	virtual ~OpSendFunc(void);



///////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[�萔
private:
	static const int			DEFECT_BUF_CNT = MAP_COL_NUM * MAP_NEST_NUM;	// �V�X�e���ŏ��Ԋu�ł̍ő��r�o�b�t�@�����O��


///////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���J�\����
public: 




///////////////////////////////////////////////////////////////////////////////////////////////////////
//// �v���p�e�B
public:
	void SetLogMgr(LogFileManager* pCls) {mcls_pLog = pCls;}							// ���O�N���X�Z�b�g
	void SetLogMarkMgr(LogFileManager* pCls) {mcls_pLogMark = pCls;}					// �}�[�L���O�֘A���O�N���X�Z�b�g
	void SetQueOpSend(ThreadQueue<SOCK_BASE_HEAD>* pQue, int ii) { mque_pOpSend[ii] = pQue; }		// �������\�����M�ʒm�̃L���[
	void SetCsLockCoil(CRITICAL_SECTION * pCS) {m_csLockCoil = pCS;}

	// �e�C���X�^���X (��`�̂݁B���̂� .cpp �ɋL�q)
	void SetStatusMgr(void* p);
	void SetParamMgr(void* p);
	void SetCoilMgr(void* p);
	void SetPlgMgr(void* p);
	void SetHtRecv(int men, void* pCls);



///////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���ʏ���

private:


///////////////////////////////////////////////////////////////////////////////////////////////////////
//// ���M (���ׂĐÓI�ɂ���)
public:
	static void		SendOp_Status();																						// #0 �X�e�[�^�X
	static void		SendOp_CoilInf_Ken(COIL_INF const* pCoilInf);															// #1 �R�C����� (�����p)
	static void		SendOp_CoilResult(COIL_INF const* pCoilInf);															// #2 �R�C������ (1�R�C������������)
	static void		SendOp_CoilInf_Dsp(COIL_INF const* pCoilInf);															// #3 �R�C����� (�\���p)
	static void		SendOp_DetectData(int men, COIL_INF const* pCoilInf, DETECT_BASE_DATA const* pData,
								DEFECT_DATA_IMAGE const* pImg, float dLen);													// #4 �r��� (�r���+�r�摜)
	static void		SendOp_Length(int men=-1, int y=-1);																	// #5 �ʔ���
	static void		SendOp_CutInf(char const* cKizukenNo, TO_SO_DATA_CUT const* pInf);										// #6 �J�b�g���
	static void		SendOp_AlarmData(ALARME_OUTPUT const* pInf);															// #7 �x����
	static void		SendOp_PatoReset(char const* cKizukenNo, int nMen, int nKizuSeq);										// #8 �r�ꗗ�\�r���폜
	static void		SendOp_DetectNum(char const* cKizukenNo, int nMen, long nLen, long nNum);								// #9 �r����� (�ŏ���ԓ�)

	//=========================================
	// �r���
	static void		SendOp_ColRowData(int men, COIL_INF const* pCoilInf, long y, TYP_COLROW_BUF pBufDefCol[], TYP_COLROW_BUF pBufDefAry[][MAP_NEST_NUM], int nDefCnt);// �\���@�\�ɍŏ���Ԃ̍s����𑗐M
	static void		SendOp_ColRowData(int men, COIL_INF const* pCoilInf, /*long y, int x,*/ TYP_COLROW_BUF* pData);				// �\���@�\�ɍŏ���Ԃ̍s����𑗐M

///////////////////////////////////////////////////////////////////////////////////////////////////////
//// �����o�[ �ϐ�
private:

	//=========================================
	// ��������p
	static CRITICAL_SECTION*		m_csLockCoil;							// CoilManager���{��


	//=========================================
	// �󂯓n���L���[
	static ThreadQueue<SOCK_BASE_HEAD>*	mque_pOpSend[HY_SOCK_NUM];			// �������\�����M�ʒm�̃L���[


	//=========================================
	// ���O�p
	static LogFileManager*			mcls_pLog;								// LogFileManager�X���b�h�C���X�^���X
	static LogFileManager*			mcls_pLogMark;							// �}�[�L���O�֘ALogFileManager�X���b�h�C���X�^���X
	static CString					my_sThreadName;							// ���X���b�h��
};


