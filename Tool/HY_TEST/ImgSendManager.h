// �r�摜���M�X���b�h
// �X���b�h�ԃ��b�Z�[�W�ŉ摜���M�v������M������A
// �r���E�r�摜�𑗐M����

#pragma once
#include "..\..\library\kizulibmfc\class\threadmanager.h"
#include "..\..\Library\DefectLib\KizuOpSend.h"	
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB�����݊�{�N���X
//#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X

#include "ParamManager.h"

class ImgSendManager :
	public ThreadManager
{
public:

	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_KEY {
		int		id;

		char	cKizukenNo[SIZE_KIZUKEN_NO];		// ��L�[
		char	cCoilNo[SIZE_COIL_NO];				// �R�C��No
		long	x;									// �������ʒu 0�`7
		char	cMeiNo[SIZE_MEI_NO];				// ����No
		int		nMen;								// �� 0:�\�A1:��
		int		nTid;								// �r��EdasysID
		int		nGid;								// �O���[�hEdasysID
		int		nInY;								// �r����ʒu
		float	dDS;								// DS�`����
		float	dWS;								// WS�`����
		int		nKizuSeq;							// �r�A��
		int		nSpeed;								// ���x
		int		nObjTansho;							// �T���o�͑Ώ�
		int		nObjDeflist;						// �d���׈ꗗ��ʑΏ��r
		int		nObjDefmap;							// �r�}�b�v�o�͑Ώ�
		int		dspmode;							// �d���׈ꗗ�����\���Ώ��r
		int		nSysKbn;							// ���o�V�X�e��
		float	nArea;
	};
	ThreadQueue<DELIVERY_KEY>	gque_Deli;

	ImgSendManager(void);
	virtual ~ImgSendManager(void);


	//=========================================
	// �Z�b�g
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; };	// �p�����[�^�Ǘ�
	void SetOpSend(KizuOpSend *pCls)			{ mcls_pOpSend = pCls; };	// �\�P�b�g�Ǘ�
//	void SetDbQue(ThreadQueue<DBSaveManager::DeliverySQL>*	pQue)			{ mque_AddDB = pQue; };


	//// �����o�[�֐�
protected:
	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);		

	void SetDefectInfo(DELIVERY_KEY* pDeli);							// 1.�r���(�r���+�r�摜) ���M


	//// �����o�ϐ�
	//=========================================
	//// �e�C���X�^���X
	ParamManager*					mcls_pParam;	
	KizuOpSend*						mcls_pOpSend;
//	ThreadQueue<DBSaveManager::DeliverySQL>*	mque_AddDB;

};
