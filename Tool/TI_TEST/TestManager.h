

#pragma once

// �r�����C�u���� �C���N���[�h
#include "..\..\Library\DefectLib\KizuOpSend.h"	
#include "..\..\Library\DefectLib\DBSaveManager.h"						// DB�����݊�{�N���X
#include "..\..\Library\KizuLibMFC\Class\CreateSQL.h"					// SQL�������⏕�N���X

// �e�X�g�摜�擾�p
#include "..\..\Library\ImgLib\CLASS\KizuAviManager.h"					// AVI�t�@�C��
#include "..\..\Library\KizuLib\CLASS\KizuBitmapManager.h"				// BitMap�t�@�C��


#include "ParamManager.h"	

// �S�������
#include "CullManager.h"

// �C���|�[�g
using namespace ImgLib;

class TestManager : public ThreadManager
{

////�@���J�֐�
public:
	//=============================
	// �{�N���X�󂯓n���p�N���X
	//=============================
	//// ���[�����󂯓n���L���[
	struct DELIVERY_KEY {
		int		nMode;							// �I�����[�h (0:�蓮 1:BMP 2:AVI)
		char	cPath[256];						// �t�@�C���p�X
		int		nCycle;							// ����[ms] 0:��~
		int		nStart;							// �J�nNo
		int		nEnd;							// �I��No
	};
	ThreadQueue<DELIVERY_KEY>	gque_Deli;								// �e�X�g�}�l�[�W���[�󂯓n���L���[

	TestManager(int nNumCam=10, int nCamStart=1);
	virtual ~TestManager(void);


	//=========================================
	enum DELIVERY_ID {
		EM_TIMER = 0,							// �^�C�}�N��
		EM_FRAME_SEND,							// �t���[����񑗐M
		EM_END
	};

	// �B�����
	enum EM_GRAB_MODE {
		GRAB_MODE_INIT = -1,											// �����l
		GRAB_MODE_MAN = 0,												// �}�j���A���Z�b�g
		GRAB_MODE_BMP,													// Bitmap�t�@�C��
		GRAB_MODE_AVI													// AVI�t�@�C��
	};


	//=========================================
	// �Z�b�g
	void SetParamMgr(ParamManager* pCls)		{ mcls_pParam = pCls; };	// �p�����[�^�Ǘ�
	void SetCullMgr(CullManager** pCls)			{ mcls_pCull = pCls; }		// �S��
	void Alloc();														// �m��
	void Free();														// �J��

	void SetFrameNo(int nFrameNo)				{ my_nFrameNum = nFrameNo; }	// �t���[��No�Z�b�g
	void SetChangeFrm(int nChangeFrm)			{ my_nChangeFrm = nChangeFrm; }	// �A���ғ��ؑփt���[��No�Z�b�g

	//=========================================
	// �Q�b�g
	virtual inline int				GetCamIndex(DWORD set, DWORD ang) const						 // �S��0�I���W�� (�J�����P��)
	{
		_ASSERT(set < my_nCamsetNum);
		_ASSERT(ang < my_nAngleNum);
		return set*my_nAngleNum + ang;
	}

	int GetFrameNo()							{ return(my_nFrameNum); }		// �t���[��No�Q�b�g
	int GetChangeFrm()							{ return(my_nChangeFrm); }		// �A���ғ��ؑփt���[��No�Q�b�g

	//=========================================

	//// �ϐ�
	BYTE*			pImgs;												// �摜				(�J�����p�x��)

//// �����o�֐�
private:


	int ThreadFirst();													// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	int ThreadLast();													// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    void ThreadEvent(int nEventNo);				

	void Exec(DELIVERY_KEY* pDeli);										// ���C���������s
	void ExecCycle();													// ������������s


	// �T���v���摜����n
	bool		OpenBmp(char const* cPathFormat, int nNoFrom, int nNoTo, int nCycleTimer);	// �A���r�b�g�}�b�v��`
	void		CloseBmp();											// �A���r�b�g�}�b�v���
	bool		OpenAvi(char const* cPathFormat, int nCycleTimer);	// AVI��`
	void		CloseAvi();											// AVI���
	bool		OpenManual();
	void		CloseManual();
	bool		OnManualFrameGrabbed(BYTE* pImgs, DWORD nBytes);

	void		OnAddCull(int nCamId);								// �S���摜���M�v��

	void		Send_TiRpisave_NextCoil(int nFrmNo);				// ���R�C����񎖑O�ʒm

private:
//// �����o�ϐ�

	//=========================================
	//// �n���h��
	HANDLE					m_evTimerExec;								// ��������s�C�x���g

	//=========================================
	//// �e�C���X�^���X
	ParamManager*			mcls_pParam;	
	//// �S��
	CullManager**			mcls_pCull;									// �Ԉ����N���X

	//// �ϐ�
	int						m_nNumCam;									// �J�����䐔
	int						m_nCamStart;								// �J�����J�n�ʒu



	//// �萔


protected:

	//// �ϐ�
	DWORD							my_nCamsetNum;						// ��������
	DWORD							my_nAngleNum;						// �p�x��
	DWORD							my_nImgSize;						// �t���[���T�C�Y (1�摜�݂̂̃T�C�Y)
	DWORD							my_nFrameW;							// ��   �t���[���摜�T�C�Y
	DWORD							my_nFrameH;							// ���� �t���[���摜�T�C�Y
	DWORD							my_nFrameNum;						// �t���[��No 
	int								my_nChannel;						// �摜��
	DWORD							my_nChangeFrm;						// �ؑփt���[��No 



	// �e�X�g�摜�p
	KizuAviManager*					mycls_Aiv[NUM_CAMERA];				// AVI�t�@�C���ǂݍ��݃N���X
	HANDLE							my_evSmpImgTimer;					// AVI�摜��������ŉ摜��ǂݍ��ޏꍇ
	CString							my_sSmpPathFormat;					// �ǂݍ��݃t�H�[�}�b�g
																		// bmp���A�J�����Z�b�g(0�I���W��), �J�����p�x(0�I���W��), �A��
																		// avi���A�J�����Z�b�g(0�I���W��), �J�����p�x(0�I���W��)
	int								my_nSmpIntervalNo[2];				// �ǂݍ��݊J�nNo,�I��No
	int								my_nSmpNowNo;						// ���݂̓ǂݍ���
	int								my_nSmpCycleTimer;					// �ǂݍ��ݎ��� [ms]

	int								my_nSelectMode;						// �I�����[�h


	//// �֐�
	virtual void		OnBmpFrameGrabbed();							// �S�J�����Z�b�g�A�J�����p�x�ɑ΂��ăr�b�g�}�b�v�摜���Z�b�g
	virtual void		OnAviFrameGrabbed();							// �S�J�����Z�b�g�A�J�����p�x�ɑ΂���AVI�摜���Z�b�g

};
