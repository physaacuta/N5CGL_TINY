// *********************************************************************************
//	���C�u�����̐ݒ�؂�ւ��X�C�b�`�W�@(�e�V�X�e���Ŏ��R�ɕύX�\)
//	[Ver]
//		Ver.01    2009/08/28  x64 �Ή�
//
//	[����]
//		��{�I�ɁA���������� (�؂�ւ��X�C�b�`�p�̂�)
// *********************************************************************************


#pragma once


//***************************************************************
// ���C�u�����E�^�X�N���� ��`
//***************************************************************
//=======================================
// �؂�ւ��p�萔��`
//=======================================
// �����́A�I�����C�������ׂăR�����g�ɂ��Ă������ƁI�I
//#define LOCAL												// �{�Ԏ��R�����g  (�e�X�g���R�����g�A�E�g)
//#define NO_HW												// �{�Ԏ��R�����g  (�n�[�h�E�F�A������)
//#define PLG_DEBUG											// PLG�e�X�g
//#define PIO_DEBUG											// PIO�e�X�g
//#define NON_DB_SAVE										// ����DB�ۑ�����
//#define DEF_DETECT_LOG									// �r���o���O�o�͖���



//***************************************************************
// ���C�u���� ��`
//***************************************************************
//=======================================
// KizuBcp
//=======================================
//// SQL Native Cliant �̃o�[�W�����ݒ�
//#define KIZUBCP_SQLSERVER_2005
//#define KIZUBCP_SQLSERVER_2008
#define KIZUBCP_SQLSERVER_2014

// VS2010�ł́ALibrary��*.lib��pragma�ň����t����̂����܂������Ȃ��̂ŁA
// �^�X�N����lib�������N���鎞�ɗL�����B  ��VS2010�ł͕K�{����
//   �p�X��lib�����B
#define LIB_MANUAL


//// BCP�������摜�f�[�^�������ݎ��̍ő�C���[�W�T�C�Y�ݒ�
#define KIZUBCP_MAX_IMGSIZE 	4096*4096					// ������Ȃ���΁A���̃V�X�e���̃t���[���T�C�Y���w�肵�Ă����΂Ƃ肠����OK�B



//// �p�X�֌W(�S���R�����g�Ȃ�A�]���ǂ��� KizuBcp�̒��̂���g�p����)
// KizuBcp�̓z�ƈႤ�ꍇ�A�㏑���ł��̃p�X���g�p����
#ifdef KIZUBCP_SQLSERVER_2005
	#define KIZUBCP_PATH_H	"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Include\\sqlncli.h"

  #ifndef LIB_MANUAL
	#ifdef _M_X64			// x64 �R���p�C��
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x64\\sqlncli.lib\""
		#endif
	#else
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\90\\SDK\\Lib\\x86\\sqlncli.lib\""
		#endif
	#endif
  #endif
#endif
#ifdef KIZUBCP_SQLSERVER_2008
	#define KIZUBCP_PATH_H	"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Include\\sqlncli.h"

  #ifndef LIB_MANUAL
	#ifdef _M_X64			// x64 �R���p�C��
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x64\\sqlncli10.lib\""
		#endif
	#else
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x86\\sqlncli10.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\100\\SDK\\Lib\\x86\\sqlncli10.lib\""
		#endif
	#endif
  #endif
#endif
#ifdef KIZUBCP_SQLSERVER_2014
	#define KIZUBCP_PATH_H	"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Include\\sqlncli.h"

  #ifndef LIB_MANUAL
	#ifdef _M_X64			// x64 �R���p�C��
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x64\\sqlncli11.lib\""
		#endif
	#else
		#ifdef _DEBUG
			#define KIZUBCP_PATH_L	"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x86\\sqlncli11.lib"
		#else
			#define KIZUBCP_PATH_L	"\"C:\\Program Files\\Microsoft SQL Server\\110\\SDK\\Lib\\x86\\sqlncli11.lib\""
		#endif
	#endif
  #endif
#endif

//=======================================
// DefectLib
//=======================================
//// IPP ���C�u���� �C���X�g�[���ς� �ݒ�
#ifdef _M_X64			// x64 �R���p�C��
	#define DEFECTLIB_KIZUJPEGMANAGER							// ipp���C���X�g�[�����́A�R�����g�����邱�ƂŁA�R���p�C�����ʂ�悤�ɂȂ�
#else
//	#define DEFECTLIB_KIZUJPEGMANAGER							// ipp���C���X�g�[�����́A�R�����g�����邱�ƂŁA�R���p�C�����ʂ�悤�ɂȂ�
#endif


//// �R���e�b�N WMD�Ńh���C�o�g�p�ݒ�
#define DEFECTLIB_CONTEC_PIO_NON							// ��������PIO�{�[�h�����V�X�e�� �R�����g�A�E�g
#define DEFECTLIB_CONTEC_CNT_NON							// ��������CNT�{�[�h�����V�X�e�� �R�����g�A�E�g

#define DEFECTLIB_CONTEC_PIO_WMD							// PIO�{�[�h WMD�Ńh���C�o�g�p�� �R�����g�A�E�g
#define DEFECTLIB_CONTEC_CNT_WMD							// CNT�{�[�h WMD�Ńh���C�o�g�p�� �R�����g�A�E�g

//-----------------------------
//// �p�X�֌W(�S���R�����g�Ȃ�A�]���ǂ��� ���C�u�����̒��̂���g�p����)
#ifndef DEFECTLIB_CONTEC_CNT_NON
#ifndef DEFECTLIB_CONTEC_CNT_WMD		// �]����
	#define DEFECTLIB_CONTEC_CNT_PATH_H "..\..\include\Other\APICNT8M.H"						// �J�E���^�[�{�[�h �C���N���[�h

  #ifndef LIB_MANUAL
	// VS�̃o�O���ۂ��B
	#ifdef _DEBUG
		#define DEFECTLIB_CONTEC_CNT_PATH_L "C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNT\\Samples\\lib\\APICNT.lib"
	#else
		#define DEFECTLIB_CONTEC_CNT_PATH_L "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNT\\Samples\\lib\\APICNT.lib\""
	#endif
  #endif
#else									// WMD��
	#define DEFECTLIB_CONTEC_CNT_PATH_H "..\..\include\Other\CCnt.h"							// CNT�{�[�h �C���N���[�h

  #ifndef LIB_MANUAL
	// VS�̃o�O���ۂ��B
	#ifdef _M_X64			// x64 �R���p�C��
		#ifdef _DEBUG
			#define DEFECTLIB_CONTEC_CNT_PATH_L "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_amd64\\CCNT.lib"
		#else
			#define DEFECTLIB_CONTEC_CNT_PATH_L "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_amd64\\CCNT.lib\""
		#endif
	#else
		#ifdef _DEBUG
			#define DEFECTLIB_CONTEC_CNT_PATH_L  "C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_i386\\CCNT.lib"
		#else
			#define DEFECTLIB_CONTEC_CNT_PATH_L  "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\CNTWDM\\Samples\\Lib_i386\\CCNT.lib\""
		#endif
	#endif
  #endif
#endif
#endif

//-----------------------------
//// �p�X�֌W(�S���R�����g�Ȃ�A�]���ǂ��� ���C�u�����̒��̂���g�p����)
#ifndef DEFECTLIB_CONTEC_PIO_NON
#ifndef DEFECTLIB_CONTEC_PIO_WMD		// �]����
	#define DEFECTLIB_CONTEC_PIO_PATH_H "..\..\include\Other\APIDIO.H"								// PIO�{�[�h �C���N���[�h
	
  #ifndef LIB_MANUAL
// VS�̃o�O���ۂ��B
	#ifdef _DEBUG
		#define DEFECTLIB_CONTEC_PIO_PATH_L "C:\\Program Files\\CONTEC\\API-PAC(W32)\\Dio\\Samples\\Lib\\APIDIO.lib"
	#else
		#define DEFECTLIB_CONTEC_PIO_PATH_L "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\Dio\\Samples\\Lib\\APIDIO.lib\""
	#endif
  #endif
#else									// WMD��
	#define DEFECTLIB_CONTEC_PIO_PATH_H "..\..\include\Other\CDio.h"								// PIO�{�[�h �C���N���[�h

  #ifndef LIB_MANUAL
	// VS�̃o�O���ۂ��B
	#ifdef _M_X64			// x64 �R���p�C��
		#ifdef _DEBUG			 
			#define DEFECTLIB_CONTEC_PIO_PATH_L "C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_amd64\\CDio.lib"
		#else
			#define DEFECTLIB_CONTEC_PIO_PATH_L "\"C:\\Program Files (x86)\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_amd64\\CDio.lib\""
		#endif
	#else
		#ifdef _DEBUG			 
			#define DEFECTLIB_CONTEC_PIO_PATH_L "C:\\Program Files\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_i386\\CDio.lib"
		#else
			#define DEFECTLIB_CONTEC_PIO_PATH_L "\"C:\\Program Files\\CONTEC\\API-PAC(W32)\\DIOWDM\\Samples\\Lib_i386\\CDio.lib\""
		#endif
	#endif
  #endif
#endif
#endif

//=======================================
// AtlRegExpLib
//=======================================
//// ATL or STD
//#define ATLREGEXPLIB_IS_ATL									// STD���g�p(=VS2008�ȍ~)����ꍇ�R�����g�����邱��

//=======================================
// IportLib
//=======================================
//// MMX�R�s�[ or �ʏ��memcpy
//#define IPORTLIB_IMG_COPY_MMX								// MMX��SwdetectMMX::mem_copy_64 ���g�p����ꍇ�A�R�����g�A�E�g
																// �t�H���_�\���́A�x�[�X�t�H���_\Epc\lib �ł��邱��
//// ���F�����A�Z���u��
#define	IPORTLIB_IMG_CORR_MMX	// �R�����g�A�E�g�ŃA�Z���u��

//=======================================
// Jpeg���k���C�u����
//=======================================
#define JPEGLIB_LJT												// libjpeg-Turbo���g�p
//#define JPEGLIB_IPP											// �C���e�� IPP���g�p
