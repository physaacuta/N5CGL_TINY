// *********************************************************************************
//	�t�@�C������RPI�t�@�C����ǂݍ��ރN���X�B
//	[Ver]
//		Ver.01    2014/09/15  
//
//	[����]
//
// *********************************************************************************
#pragma once
#include "RpiReader.h"

namespace RpiLib
{
class RpiReaderFile : public RpiReader
{
	HANDLE m_hFile;
	CString m_sFilename;
	bool m_bDirect;
	bool bIndexAccess;

public:
	RpiReaderFile() : m_hFile(0)
	{};

	~RpiReaderFile(void)
	{	Close();	}

	//�t�@�C�������
	void Close();


	//�t�@�C���I�[�v���B
	//sFilename�F�J��RPI�t�@�C���B
	//bDirect�FFILE_FLAG_NO_BUFFERING���w�肵�����ꍇ�ATrue�B
	//�߂�F������true�B�G���[���b�Z�[�W�́AGetError�Ŏ���B
	bool Open( const char *sFilename, bool bDirect )
	{
		m_bDirect = bDirect;
		m_sFilename = sFilename;
		if( !doOpen( bDirect ) ) {
			return false;
		}
		return ParseRead();
	}
	HANDLE handle() { return m_hFile; }

protected:
	bool doOpen( bool bDirect );
	bool ParseRead();
	bool ReadIndexPos(bool bForce = false);

public:
	//�����ǂݍ��ݐ�p���[�_������ĕԂ��B
	//�g���I�������DELETE���邱�ƁB
	class IRpiFrameEnum *CreateSequencialReader();

	//�����_���ǂݍ��ݐ�p���[�_������ĕԂ��B
	//�g���I�������DELETE���邱�ƁB
	class IRpiFrameEnum *CreateRandomReader();

	friend class RpiFrameEnum;
};

};