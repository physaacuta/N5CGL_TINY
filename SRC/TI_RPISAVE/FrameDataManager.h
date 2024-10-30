#pragma once

// ���Œ�
#include "..\..\Include\KizuCommon.h"									// �r���C���N���[�h
#include "..\..\Library\KizuLib\KizuLib.h"								// �r�����C�u����
#include "..\..\Library\KizuLib\Class\KizuMacro.h"						// �}�N���W
#include "..\..\Library\KizuLib\Class\KizuFunction.h"					// �X�^�e�B�b�N���C�u�����W
#include "..\..\Library\KizuLibMFC\Class\LogFileManager.h"				// ���O�Ǘ��N���X


// �摜�����p���C�u���� �C���N���[�h
#include "..\..\Library\IportLib\IportCommon.h"							// iPort�p�N���X�萔


// ���[�J��
#include "MainConst.h"													// ���N���X�萔�w�b�_�[

using namespace KizuLib;
using namespace IportLib;

class FrameDataManager : implements FRAME_DATA
{
	//// ���� ����́A�p�����Œ�`����Ă���
	//DWORD			nCamset;										// �J�����Z�b�g		�� (1�I���W��) (set:PlgSyncProc)
	//BYTE*			pImgs;											// �摜				�� �A����̐��摜��� (�擪�ɂ̓r�b�g�}�b�v�w�b�_�[�t�^�ς�)(set:ImgUnionProc)
	//DWORD			nLen;											// �摜�T�C�Y		�� (�r�b�g�}�b�v�w�b�_�[�T�C�Y���t�^) (set:ImgUnionProc) 


	// ���g�p -----------
	//DWORD			nNo;											// iPort�t���[��No	(0�I���W��) (1�R����)
	//WORD			nSeqNum[32];									// iPort�V�[�P���X�ԍ�

	//HANDLE		hInfo[4];										// SetAddInfo �ŕt�^�������
	
	// �s�x�Z�b�g -----------
	//KizuPerformanceCounter	cls_Time;							// �擾��������(�X�g�b�v�E�H�b�`�@�\���g�p)
																			// ���C�u�������ŃZ�b�g����^�C���X�^���v
																			// ���O�F�ŏ��̃J�����p�x���擾��������
																			// ���P�F�S���̃J�����p�x�������āA�摜���R�s�[������������

public:
	// �X�g�b�v�E�H�b�`
	//KizuPerformanceCounter	cls_Time;							// �擾��������(�X�g�b�v�E�H�b�`�@�\���g�p)
																		// ���C�u�������ŃZ�b�g����^�C���X�^���v

	static const int		TIME_GetCamFast			= 10;				// ��10�F�ŏ��̃J�������擾��������
	static const int		TIME_GetCamLast			= 11;				// ��11�F�Ō�̃J�������擾��������

	static const int		TIME_PlgSync_END		= 15;				// ��19�FPlgSyncProc������������

	//static const int		TIME_Coil_START			= 16;				// ��16�FCoilControlProc�������J�n��
	//static const int		TIME_Coil_END			= 17;				// ��17�FCoilControlProc������������ 

	static const int		TIME_ImgUnion_START		= 20;				// ��20�FImgUnionProc�������J�n��
	static const int		TIME_ImgUnion_END		= 21;				// ��21�FImgUnionProc������������  (���܂ł́AImgUnionProc�ŃZ�b�g)

//	static const int		TIME_ImgColor_START		= 22;				// ��20�FImgCompProc�������J�n��
//	static const int		TIME_ImgColor_END		= 23;				// ��21�FImgCompProc������������

	static const int		TIME_ImgBuf_START		= 24;				// ��20�FImgBufProc�������J�n��
	static const int		TIME_ImgBuf_END			= 25;				// ��21�FImgBufProc������������

	static const int		TIME_ImgComp_START		= 26;				// ��20�FImgCompProc�������J�n��
	static const int		TIME_ImgComp_END		= 27;				// ��21�FImgCompProc������������

	static const int		TIME_RpiWrite_START		= 30;				// ��20�FRpiWriteProc�������J�n��
	static const int		TIME_RpiWrite_END		= 31;				// ��21�FRpiWriteProc������������



//// �{���̌��J�ϐ�
public:
	// 1�t���[���̃f�[�^�̈�
	int						nBufSize;									// ���k��̈�̃o�b�t�@�T�C�Y(�Z�N�^���l�������T�C�Y) (set:�\�z��)
	BYTE*					pBuf;										// ���k��̗̈�{�� (�擪�ɂ�RpiFileManager::LINK����t�^) (�̈�m��:�\�z��) (set:ImgCompProc)
																			// ���̂܂� nSetSize�� RPI�t�@�C���ɏ�����
	int						nCompSize;									// ���k��T�C�Y (set:ImgCompProc)
	int						nSetSize;									// �f�[�^�̃Z�b�g�T�C�Y (set:ImgCompProc) (�X�V:RpiWriteProc)
	
	BYTE*					pFdata;										// �t����񕔂̐擪�|�C���^�B�����̏ꍇNULL
	int						nFdataSize;									// �t����񕔂̃T�C�Y�B�����̏ꍇ0
	

	//// �ʏ�̃f�[�^ (set:ImgUnionProc)
	// �t���[���P�ʂŎ����Ă��� ��{��� 
	DWORD					nIns;										// �C���X�^���X�ԍ� (0�I���W��)
	DWORD					nStartCamset;								// ��ԍ��[�̃J�����Z�b�g�ԍ� (1�I���W��)
	DWORD					nAngle;										// �J�����p�x (0�I���W��)
	DWORD					nColor;										// �摜�F�ԍ� (0�I���W��)
	DWORD					nMen;										// �\���敪 (0:�\ 1:��)
	DWORD					nUnionWidth;								// �t���[���摜�A����̉摜��
	DWORD					nUnionHeight;								// �t���[���摜�A����̉摜����


	// �t���[�����̏��
	DWORD					nCFno;										// �R�C���擪����̃t���[��No (0�I���W��)
	int						nEdgeIdx[2];								// ���[����̍��E�G�b�WPixel�ʒu [pixel]
	bool					bRpiSave;									// �摜�ۑ��I��
	EM_IMG_SAVE_STAT		emImgSaveStat;								// �摜��ԔF��


	//// �t�ѓI�ȏ��
	//int						nFTset;										// ���[�ʒu�Z�b�g��  (��[1  ��[2 ����0)
	//long					nFTposCfNo;									// ���[�ʒu�t���[��No (���̃t�@�C���擪����̃t���[��No (0�I���W��))			�� IMG_SAVE_STAT_F,IMG_SAVE_STAT_T�̎��̂�
	//long					nFTposPixel;								// ���[�ʒu�t���[����pixel (���̃t���[����[����̃s�N�Z���ʒu (0�I���W��))	�� IMG_SAVE_STAT_F,IMG_SAVE_STAT_T�̎��̂�



	// �R�C�����
	char					cKizukenNo[SIZE_KIZUKEN_NO];				// �Ǘ�No
	char					cCoilNo[32];								// �R�C��No
	EM_COMP_TYPE			emCompType;									// ���k�`��
	int						nCompQuality;								// ���k��
	bool					bDummyCoil;									// true:�_�~�[�R�C��

	// UDP���
	UDP_DATA				UdpData;									// UDP���
//	int						nUnionEdgePos[2];							// �t���[���摜�A����̃G�b�W�ʒu [pixel] (������̃G�b�W)


	//// ��ʕ\���p (set:ImgUnionProc)
	int						nImgLapPos[MAX_CAMSET][2];					// �t���[���ԉ摜�̏d�Ȃ�ʒu [������摜�̍������pixel]
	int						nImgClipPos[2];								// ���E�̍폜��f�ʒu [������摜�̍������pixel]


	//// ���̑�
	int						nWriteMode;									// �����݃��[�h (0:���������݃��[�h 1:�Z�N�^�����݃��[�h) (set:�\�z��)
	
//// ���J�֐�
public:
	FrameDataManager(void);
	virtual ~FrameDataManager(void);

	void			Clear();											// ������

//// �����o�[�֐�
protected:
	virtual void	Alloc(int const* p=0);								// �퐶���� (IportManager��Init���ꂽ���R�[���o�b�N)
	virtual void	Free();												// ������ (IportManager��Dispose���ꂽ���R�[���o�b�N)
};
