// *********************************************************************************
//	BCP �N���X (������DB�����ݐ���N���X)
//	[Ver]
//		Ver.01    2007/01/30  vs2005 �Ή�
//
//	[����]
//		�g�p����^�X�N�� �wodbc32.lib�x �������N���鎖
//		OdbcManager ���g���₷���悤�ɔ�킹
//
// --------------------------------------------
// �g�p���@
//		KIZUBCP_BCPMANAGER �̃R�����g���O��
//		ODBC�̐ݒ�
//				�ʏ� �uSQL Server�v��I�����Ă��邪�A�uSQL Native Client�v��I������
//
// ���\�z
//	[�ǉ��C���N���[�h�p�X]
//		����
//	[�ǉ����C�u�����p�X]
//		"C:\Program Files\Microsoft SQL Server\90\SDK\Lib\x86"
//	[�ǉ����C�u�����t�@�C��]
//		sqlncli.lib
// *********************************************************************************

#pragma once

#include <stdio.h>
#include "BCPManager.h"														// ODBC�ڑ����N���X


// �r�� �C���N���[�h
#include "..\KizuODBC\OdbcBase.h"											// �e�N���X
#include "..\KizuODBC\OdbcManager.h"										

using namespace KizuODBC;

namespace KizuODBC
{

	class BCPBase// :	public OdbcBase
	{
		public:

		//------------------------------------------
		// BCP�n�󂯓n���\����
		//------------------------------------------

		//-------------
		// �󂯓n���f�[�^�^�C�v
		typedef enum enumBCP_BINDTYPE {
			em_VarChar		= SQLVARCHAR,
			em_Int			= SQLINT4,
			em_Float		= SQLFLT4,
			em_Double		= SQLFLT8,
			em_Day			= SQLDATETIME, //SQLDATETIMN,	// �g�p����^�́ACTime 
			em_Binary		= SQLIMAGE,			// image�^
			em_VerBinary	= SQLVARBINARY
		} emBcpBindType;

		//-------------
		// �o�C�i���󂯓n���̈� (�Œ蒷)
		/*typedef*/ struct typBindData {
			// �̈悪�A���ő����Ă��Ȃ��Ƃ����Ȃ��ׁA�o�C�i���f�[�^��new���Ȃ�
			long    nSize;														// �T�C�Y (�o�C�i���f�[�^�ȊO�͕K�v�����B�o�C���h���́A1���Z�b�g)
			BYTE	bData[KIZUBCP_MAX_IMGSIZE];									// �f�[�^
								// (int�f�[�^�Ƃ��̏ꍇ�́A(BYTE*)&(ntest) ����Ȋ����ŃZ�b�g)
								// (�o�C�i���f�[�^�́A(BYTE*)&(typBindData�\����)���Z�b�g )
		};

		//-------------
		// �o�C�i���󂯓n���̈� (�ϒ��Ή�)
			// �S�̂ɑ΂��Ă� �������͐�΂ɋ֎~�BInit()���g������
			//// �g����
			//BCPBase::typBindDataEx  p(3);
			//memset(p.pData, 0x01, 3);
			//p.SetSize(0xF1F2F3F4);
		struct typBindDataEx {
		  private:
			BYTE *		pBase;				// �|�C���^�{��
		  public:
			__int64		nMaxSize;			// �摜�ő�T�C�Y
			BYTE*		pData;				// �f�[�^���|�C���^

			// �R���X�g���N�^
			typBindDataEx() {
				nMaxSize = 0;
				pData = NULL;
				pBase = NULL;
			};

			// �f�X�g���N�^
			~typBindDataEx(void) {
				if(NULL != pBase) delete [] pBase;
			};

			// �̈�m��
			void Alloc(int nMaxSize)
			{
				if(0 > nMaxSize) return;			
				this->nMaxSize	= nMaxSize;

				// �̈�m��
				this->pBase		= new BYTE[nMaxSize + sizeof(__int64)];

				// �g���₷���悤��
				if(0 != nMaxSize) {
					this->pData	= &pBase[(sizeof(__int64))];
					Init();
				} else {
					// �T�C�Y��0�̂Ƃ��́A�Œ��NULL�f�[�^���Z�b�g�������Ƃ��̂��߂̏��u
					SetNullData();
				}
			};


			//// �����o�[�֐�
			__int64 GetSize()				{ return (__int64)((__int64*)pBase)[0]; }
			void	SetSize(__int64 nSize)	{ (__int64)((__int64*)pBase)[0] = nSize; }	// �T�C�Y���Z�b�g (�擪4�o�C�g���T�C�Y�̈�) (varbinary��8�Ȃ̂Œ���) -1�̂Ƃ���NULL�o�^�ƂȂ�
			void	Init()					{ SetSize(1); }							// �G���A������
			void	SetNullData()			{ SetSize(SQL_NULL_DATA); }				// NULL �o�^�́A(�擪4�o�C�g��ALL FF) (=-1�̂���)

			BYTE*	GetBindPointer() const	{ return pBase; }						// �o�C���h�|�C���g�擾
		};

	public:
		BCPBase(void);
		~BCPBase(void);
		
	//------------------------------------------
	// OdbcManager �̔�킹�֘A
	//------------------------------------------
		// �ڑ�
		bool Connect(LPCSTR cIniPath, LPCSTR cSession = NULL);					// �Z�b�V�����w��ł̐ڑ�
		void DisConnect();														// �ؒf
		bool IsConnect() const {return mycls_pDB->IsConnect();}					// �ڑ������m�F


	//------------------------------------------
	// �����������ݗp (�z���g��TO_DEFECT�����g���K�v����) (���܂ł̃o�C���h���s����10�{���x�����ɂȂ邪�g����������̂Œ���)
	//      BCP�n (���C�u������ odbc32.lib �Ƃ͕ʂ� odbcbcp.lib ��ǉ�����K�v�L��)
	//------------------------------------------
		bool BCP_Init(LPCSTR cTableName);													// BCP�n ������
		bool BCP_Init();																	// BCP�n ������
		bool BCP_Bind_Value (int coulmn, BYTE* pData, emBcpBindType typ);					// BCP�n �o�C���h (�l�^)
		bool BCP_Bind_Binary (int coulmn, typBindData* pData, emBcpBindType typ=em_Binary);		// BCP�n �o�C���h (�o�C�i���z��^)
		bool BCP_Bind_Binary (int coulmn, typBindDataEx* pData, emBcpBindType typ=em_VerBinary);// BCP�n �o�C���h (�o�C�i���z��^ �ϒ��Ή�)
		bool BCP_Bind_VerBinary(int coulmn, BYTE* pData);
		bool BCP_Bind_BinaryNull(int coulmn, typBindData* pData, emBcpBindType typ=em_Binary);
		bool BCP_Bind_String(int coulmn, char* pData);										// BCP�n �o�C���h (������^ �I�[NULL�K�{)
		
		bool BCP_SendRow();																	// BCP�n DB�փf�[�^�]��
		int  BCP_Batch();																	// BCP�n �f�[�^�]�������f�[�^�̃R�~�b�g�݂����ȓz
		bool BCP_Down();																	// BCP�n �J��
		static void Get_BPC_Time(char* cTime);												// BCP�n ���t�o�^�����擾

		bool BCP_ColLen(int coulmn, long nSize);											// BCP�n �o�C���h�̃o�C�i���f�[�^�T�C�Y�w�� (varbinary(MAX) ���� 0xFFFF �ȏ�̃T�C�Y�����܂����Ɠo�^�ł��Ȃ��b��Ή���)

	//------------------------------------------
	// �O���A�N�Z�X
	//------------------------------------------
		const char* GetTableName() const { return m_cTableName; }				// BCP�Őڑ����Ă���e�[�u����

	protected:
		BCPManager* mycls_pDB;													// ODBC�ڑ����N���X (�N���X�𒼂Ŏg��)
		SQLHDBC		m_hdbc;														// �ڑ��n���h��
		char		m_cTableName[256];											// �ڑ�����e�[�u������
	};
};