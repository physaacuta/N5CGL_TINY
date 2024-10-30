// RPI �Ǎ��N���X�̎���
//  �������ɂ���RPI��ǂݍ��ރN���X�B
#include "stdafx.h"
#include "rpi.h"
#include "RpiReaderMemory.h"

namespace RpiLib{

//------------------------------------------
//�^����ꂽ��������RPI���ǂ����A�w�b�_��ǂ�ł݂�
//------------------------------------------
bool RpiReaderMemory::ParseRead()
{
    const int nFirstReadSize = 8192;
	if( m_nSize < nFirstReadSize ){
		m_sErrorMsg.Format("RPI�t�@�C���Ȃ�A������%d�o�C�g���炢�͗~����. nLen = %d", nFirstReadSize, nFirstReadSize);
		return false;
	}

	//RPI�w�b�_�ǂݍ��݁B
	bool bRet = Parse( m_pRpiMemory, nFirstReadSize );
	if( !bRet ){
		m_sErrorMsg.Format("RPI�̃w�b�_����������. type=%x, len=%d", Header.type, Header.header_size );
		return false;
	}

    return ReadIndexPos();
}

//------------------------------------------
//����������J���āA�t���[�������쐬����
//------------------------------------------
bool RpiReaderMemory::ReadIndexPos()
{
    for( int nFrame = 1; nFrame < Header.nframe; nFrame++ )
    {
        int64_t nFramePos = LinkArray[nFrame].nPosInFile;
        if( 32 < nFramePos  && nFramePos < m_nSize ){
            const BYTE *pBuffer =  &m_pRpiMemory [ nFramePos  ];
            LinkArray.UpdateNextPosOnRead( nFrame, pBuffer );
        }
        else{
            m_sErrorMsg.Format("RPI���������Ă��������A�r���Ő؂ꂽ�Bframe=%d, %lld", nFrame,  nFramePos);
            return false;
        }
	}
    return true;
}







//------------------------------------------
//����������t���[����ǂ݁A�Ԃ��N���X�B
//------------------------------------------
class RpiMemoryFrameEnum : public IRpiFrameEnum
{
	RpiReaderMemory *that;

public:

	RpiMemoryFrameEnum(RpiReaderMemory *athat)
		:that(athat)

	{

	}


protected:
	//�C�}�C�`.
    bool ReadToInternalBuffer( int nFrame, bool bRead );
};

//------------------------------------------
// �J���Ă���Rpi�t�@�C���́AnFrame�ڂ�ǂ݂����A�������Ɋi�[����B
// int nFrame	�Ώۃt���[��
// bool bRead	True�F�������ɖ{�̂�ǂށBFalse:�����N�����ǂ�Ŏ��ւ����ށB
//------------------------------------------
bool RpiMemoryFrameEnum::ReadToInternalBuffer( int nFrame, bool bRead)
{
	if( 1 <= nFrame && nFrame < that->LinkArray.size() );
	else	{
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d �͂��������B 1~%d�͈̔�", nFrame, that->LinkArray.size());
		return false;
	}
	

	const RPIFrameLink &link = that->LinkArray[nFrame-1];

	if(link.nPosInFile <= 32 ){
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d �͈ʒu������Ȃ��B", nFrame);
		return false;
	}


    if( link.nPosInFile < that->m_nSize )
    {
        pFrame = &that->m_pRpiMemory[ link.nPosInFile ];
        szImg = link.szImg;
        return true;
    }
    else{
		that->m_sErrorMsg.Format("RpiFrameEnum::ReadBuffer : nFrame=%d �̓������͈͊O�B%lld", nFrame, link.nPosInFile);
    }
	return false;
}

//------------------------------------------
//�t���[���ǂݍ��݃N���X������ĕԂ��B
//------------------------------------------
class IRpiFrameEnum *RpiReaderMemory::CreateSequencialReader()
{
	return new RpiMemoryFrameEnum(this);
}
};