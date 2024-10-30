// *********************************************************************************
//	������RPI�t�@�C���N���X
//	[Ver]
//		Ver.01    2010/03/04	����
//
//	[����]
//		�E�񓯊��ł͂���܂���B�ł��A�����݂͑����ł��B
// *********************************************************************************

#pragma once
// ���ʃw�b�_�[
#include "..\..\Include\KizuCommon.h"									// �V�X�e���p�C���N���[�h


// �r���N���X �C���N���[�h
#include "..\..\Library\KizuLibMFC\CLASS\ThreadManager.h"				// ���[�J�[�X���b�h�}�l�[�W���[
#include "..\..\Library\KizuLib\Class\KizuPerformanceCounter.h"			// �^�C�}�[

#include "..\..\Library\KizuLibMFC\CLASS\CStringCS.h"

#include "..\..\LibPG\LibRpi\rpi.h"
using namespace RpiLib;

class RpiFileManager : public ThreadManager
{
//// ���J�萔
public:
	static const int	MAX_SAVE_FOLDER	= 10;							// �ۑ��ő�t�H���_��

//// ���J�\����
public:
	
	//=====================================================================
	// �V�X�e����

	//// �ݒ��� (����) (�V�X�e���N����ύX��������)
	struct RPI_SETTING {
		// ���������t�@�C������肱�ނ��ߗp --------
		int				nFastFrame;										// �ŏ��̃R�C���P�ʂ̃t���[���i�[�� (0�́A1�R�C��1�t�@�C���B1�`�́A���̖����ɂȂ����玟�t�@�C����)
		int				nCycleFrame;									// �Q��ڈȍ~�̃t���[���i�[��
		int				nFolderNum;										// �ݒ�t�H���_��
		char			cFolderPath[MAX_SAVE_FOLDER][256];				// �t�H���_�p�X (�Ō��\�͖���) 
		int				nWriteMode;										// �����݃��[�h (0:���������݃��[�h 1:�Z�N�^�����݃��[�h)
		int				nSecSize;										// �Z�N�^�T�C�Y (������Ȃ��l��1�ŗǂ�)

		// ��{���w�b�_�p --------
		unsigned long	codec;											// 'JPEG' or 'LAC\0' or 'BMP\0'
        long			width;											// �摜�� (8�̔{��)
        long			height;											// �摜���� (8�̔{��)

		float			reso_x;											// ������\
		float			reso_y;											// �c����\ 


		// �����w�b�_�p --------
		char			cSysName[20];									// �V�X�e������
//		char			cKizukenNo[SIZE_KIZUKEN_NO];					// �r���Ǘ�No 
		short			frame_info_type;								// �t���[����񕔂̊i�[�`�� (0:���� 1:T_RPI_FRAME_INFO_T1)
		short			frame_info_size;								// �t���[����񕔂̃T�C�Y

		long			nLineCenter;									// ���C���Z���^�[��f(�A���摜���[����̃I�t�Z�b�g�l) [pixel]
		long			nStartCamset;									// ���[�J�����Z�b�g�ԍ� (1�I���W��)
		long			nCamsetNum;										// �������t���[�������� (1�I���W��)
		long			nCamAngle;										// �J�����p�x (1�I���W��)
		long			nMen;											// �\���敪 (0:�\ 1:��)

		struct CAM_INFO {
			float		dBaseXoffset;									// ��ʒu�B���C���Z���^�[����̊e�J�������[�̈ʒu(�������́A�}�C�i�X�l) [mm] (PARAM_CAMSET��Xoffset�l���̂���)
			int			nBaseLeft;										// ���f�B������̃t���[�����[���玩�J�����摜���[�܂ł̋���
			int			nOverLap;										// ���J�����Ƃ̉E���̃I�[�o�[���b�v��
		}caminfo[NUM_CAM_INFO];
		long			nColorType;										// �摜�^�C�v (0:�P�x�r��  1:�F���r��)
		long			nSysType;										// �r���^�C�v (0:�A�����C��  1:�o�b�`���C��)

	public:
		RPI_SETTING() {													// �f�t�H���g�R���X�g���N�^
			memset(this, 0x00, sizeof(RPI_SETTING));
		};
	};

	//// �ݒ��� (�R�C���P�ʂŉ�)
	struct RPI_COIL_INFO {
		// ��{���w�b�_�p --------
		float			reso_y;											// �c����\ 

		// �����w�b�_�p --------
		char			cKizukenNo[SIZE_KIZUKEN_NO];					// �r���Ǘ�No
		char			cCoilNo[32];									// �R�C��No
		DWORD			nCFNo;											// �R�C���擪�̃t���[��No	framenum

		struct CAM_INFO {
			float		dBaseXoffset;									// ��ʒu�B���C���Z���^�[����̊e�J�������[�̈ʒu(�������́A�}�C�i�X�l) [mm] (PARAM_CAMSET��Xoffset�l���̂���)
			int			nBaseLeft;										// ���f�B������̃t���[�����[���玩�J�����摜���[�܂ł̋���
			int			nOverLap;										// ���J�����Ƃ̉E���̃I�[�o�[���b�v��
		}caminfo[NUM_CAM_INFO];

		long			nStartLen;										// �R�C���擪�̒���ʒu [mm]
		long			nFTposCfNo[2];									// ���[�ʒu�t���[��No (���̃t�@�C���擪����̃t���[��No (0�I���W��))			�� �o�b�`�̂�
		long			nFTposPixel[2];									// ���[�ʒu�t���[����pixel (���̃t���[����[����̃s�N�Z���ʒu (0�I���W��))
		long			nCompQuality;

	public:
		RPI_COIL_INFO() {												// �f�t�H���g�R���X�g���N�^
			memset(this, 0x00, sizeof(RPI_COIL_INFO));
		};

	};
	//// �ݒ��� (�t���[���P�ʂŉ�)
	struct RPI_FRAME_INFO {
		DWORD			nFNo;											// ���̃t���[��No
	};


//// ���J�֐�
public:
	RpiFileManager(char const* cThreadName);
	virtual ~RpiFileManager(void);


	bool				RpiOpen();										// RPI�t�@�C���I�[�v��
	bool				RpiClose();										// RPI�t�@�C���N���[�Y
	bool				WriteFrame(int nImgSize, int nFrameSize, BYTE* pFrame);	// �t���[������p
	
//	void				UpdateHeader();									// �w�b�_�[���X�V


	//=========================================
	// �O���A�N�Z�X
	RPI_SETTING&		RpiSetting()	{ return my_Setting; }			// RPI�t�@�C���ݒ���v���p�e�B
	RPI_COIL_INFO&		RpiCoilInfo()	{ return my_CoilInfo; }			// RPI�t�@�C���ݒ���v���p�e�B
	RPI_FRAME_INFO&		RpiFrameInfo()	{ return my_FrameInfo; }		// RPI�t�@�C���ݒ���v���p�e�B

	RpiLib::RpiHeader const*	GetRpiHeader() const ;

	bool				IsOpen() const;
	CString				GetRpiFullPath() { return CStringCS::LockCpy( my_sFullPath); }		// CString�ŗ������΍�
	CString				GetRpiFileName() { return my_sFileName; }

	double				GetWriteSpeedAve() const { return my_TimeWrite[0]; }	// �����ݑ��x
	double				GetWriteSpeedMin() const { return my_TimeWrite[1]; }	// �����ݑ��x
	double				GetWriteSpeedMax() const { return my_TimeWrite[2]; }	// �����ݑ��x


	// ���W���[��
	static int			GetSecSize(char const* cDrivePath="c:\\");				// HDD�̃Z�N�^�T�C�Y���擾����
	static int			GetSizeJust(int x, int l) { return ((((x-1)/l)+1)*l); }	// x�T�C�Y��l�̔{���ɑ�����(�K���Ax�̂܂܂��A������Ƒ傫���Ȃ����)
					


//// ��{ (���J���Ȃ�)
private:
	virtual int ThreadFirst() { return 0; };							// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
	virtual int ThreadLast() { return 0; };								// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
    virtual void ThreadEvent(int nEventNo) { return ; };				// �X���b�h�C�x���g����


//// �����o�[�֐�
protected :
	//=========================================
	// ���z�֐�
	// ����ǉ�
	virtual void		OnSetFileName(RpiHeader const& h, RPI_SETTING const& s, RPI_COIL_INFO const& c, CString* strName)=0;	// �t�@�C��������
	virtual void		OnRpiFileSave(char const* strPath, CString* strName)=0;	// RPI�t�@�C����������

	// �����̂�
	void				CreateHeader(RpiHeader& h);					// �w�b�_�[��񐶐�
	void				CreateHeader_Ending(RpiHeader& h);			// �w�b�_�[���X�V(�R�C���m�莞)
	bool				Open(CString sName, RpiHeader* h);			// �t�@�C���̐���
	bool				Next();											// 1�R�C�����ł̃t�@�C���؂�ւ�


//// �����o�[�ϐ�
protected :
	RPI_SETTING			my_Setting;										// �ݒ��� (���ʐݒ�)
	RPI_COIL_INFO		my_CoilInfo;									// �ݒ��� (�R�C���P�ʐݒ�)
	RPI_FRAME_INFO		my_FrameInfo;									// �ݒ��� (�R�C���P�ʐݒ�)



	CString				my_sFullPath;									// �����ݒ���RPI�t�@�C����
	CString				my_sFileName;									// �����ݒ���RPI�t�@�C����
	int					my_nWriteFrameCnt;								// �R�C���P�ʂŃt���[�������ݐ�
	int					my_nSaveFolderIndex;							// �ۑ��t�H���_Index (0�I���W��)

	RpiLib::TRpiWriter *my_pWriter;
	//// �����ݑ��x����
	KizuPerformanceCounter	mycls_Time;									// �����ݎ���
	double				my_TimeWrite[3];								// �����ݑ��x[Mbyte/sec] (���ρA�Œ�A�ō�)
};
