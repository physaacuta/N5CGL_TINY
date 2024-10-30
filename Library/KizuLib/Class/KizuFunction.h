// *********************************************************************************
//	�X�^�e�B�b�N�֐��W
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//
// *********************************************************************************

#pragma once

#include <windows.h>

//#include "..\..\KizuODBC\OdbcBase.h"							// ODBC����N���X

namespace KizuLib
{
	class KizuFunction
	{
	public:
		// �@���Ԋ֌W
		static bool SetStatus(int id, int state, bool bLogDsp =true, char const * cMsg = "" );			// DB_SYSTEM_STATE�ɋ@���Ԃ��Z�b�g���� (0:����)
		static bool SetStatus(int id, bool state, bool bLogDsp =true, char const * cMsg  = "" );		//										 (true:����)
			// ini�t�@�C������s�x�Ǎ���  �Z�N�V�����͊�{
		static bool SetStatus(char const * cKey, bool state, bool bLogDsp =true, char const * cMsg  = "" );// ini�t�@�C������@��ID���擾���� (true:����)
		static bool SetStatus(char const * cKey, int state, bool bLogDsp =true, char const * cMsg  = "" );// ini�t�@�C������@��ID���擾���� (true:����)
		static bool SetStatus(char const * cKeyFormat, int id, int state, bool bLogDsp =true, char const * cMsg = "" );

			// �Z�N�V�������w���
		static bool SetStatus(char const* cSec, char const * cKey, bool state, bool bLogDsp =true, char const * cMsg  = "" ); // ini�t�@�C������@��ID���擾���� (true:����)
		static bool SetStatus(char const* cSec, char const * cKey, int state, bool bLogDsp =true, char const * cMsg  = "" );  // ini�t�@�C������@��ID���擾���� (true:����)
		static bool SetStatus(char const* cSec, char const * cKeyFormat, int id, int state, bool bLogDsp =true, char const * cMsg = "" );



		// �O��I/F�֌W
		static void DumpData(LPCSTR filename, long len, const BYTE* data, bool addTime = false);	// �_���v
		static BOOL ByteHanten4(long len, BYTE* data);						// 4�o�C�g���] (���f�[�^���������̈� ����)
		static BOOL ByteHanten2(long len, BYTE* data);						// 2�o�C�g���] (���f�[�^���������̈� ����)
		static long ByteHanten4(BYTE* data);								// 4�o�C�g���]��̃f�[�^�擾 (���f�[�^�͕ς��Ȃ�)
		static short ByteHanten2(BYTE* data);								// 2�o�C�g���]��̃f�[�^�擾 (���f�[�^�͕ς��Ȃ�)
		static void SpaceToNull(char* cVal, int size);						// �X�y�[�X��NULL�ɕύX����
		static void Trim(char* cVal,int nMode=0);							// �X�y�[�X���g��������
		static void TrimSpaceNull(char* cT, int nTSize, char const* cF, int nFSize, int nMode=0);	// �Œ蒷��������String�ɕϊ��B

		static int AtoIex(char const* c, int size);							// �w�萔���̕������g�p����Int�ɕϊ�
		static float AtoFex(char const* c, int size);						// �w�萔���̕������g�p����Float�ɕϊ�
		static double AtoDex(char const* c, int size);						// �w�萔���̕������g�p����Double�ɕϊ�

		// ��L�[����
		static void GetKizukenNo(char* cNo);								// ��{�ƂȂ��r���Ǘ�No�𐶐� (ID+yyyymmdd hhnnss\0)
		static void GetSerialNo(char* cNo);									// ��{�ƂȂ�V���A��No�𐶐� (yyyymmdd hhnnss\0)
		static void GetSerialNoFF(char* cNo);								// ��{�ƂȂ�IDNo�𐶐�       (yyyymmdd hhnnssff\0)

		// ���������֌W
		static bool SetNetBiosComputerName(char const* cNewNetBiosComputerName); // NetBIOS�̃R���s���[�^�[���O
		static long GetFileSize(char const* cFPath);						// �t�@�C���T�C�Y���擾����

		// �r���Ǝ��ŗǂ��g�p����֐�
		static bool SelectPriorityDetect(double nowLen, int nowYusenT, int nowYusenG, double preLen, int preYusenT, int preYusenG ); // ���חD��x��r
		static bool SelectPriorityDetect(int nCnt, double const* pNow, double const* pPre, bool const* bMode);
		static int  SelectGridCol(int width, int interval, int maxCol);		// �O���b�h�̗񐔂��Z�o����
		static int  SelectColPosition(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval);		// �O���b�h�ʒu����֐� (�Z���^�[�)
			// (��Ή���)
		static int  SelectGridCol2( int width, int interval, int maxCol);	// �O���b�h�̗񐔂��Z�o����
		static int	SelectColPosition2(int DorW, double edge_D, double edge_W, long width, long col_width, long col_max, long col_interval);
			// (�Œ���)
		static int SelectColPositionFix(int DorW, double edge_D, double edge_W, long width, long col_max); // �O���b�h�ʒu����֐� (�񐔌Œ�o�[�W����)

	};
};
