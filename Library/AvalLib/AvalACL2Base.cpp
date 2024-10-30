#include "StdAfx.h"
#include "AvalACL2Base.h"


#include <AcapLib2.h>
#pragma comment(lib, "AcapLib2.lib")


// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

using namespace AvalLib;


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���[�J���֐�


//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
AvalACL2Base::AvalACL2Base() :

my_hDev(INVALID_HANDLE_VALUE), my_nBoardChId(0),
my_nWidth(0), my_nHeight(0), my_nPixelNBit(0),
my_bRunning(false),
my_nBufNum(0), my_nBufSize(0), my_pBuff(NULL)
{
	ClearStatCounters();
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
AvalACL2Base::~AvalACL2Base(void)
{

}

//------------------------------------------
// ������
// int board_id		�{�[�h�ԍ�(0�I���W��)
// int board_ch_id	�I�[�v���`�����l�� (1�I���W��) ��0:�SCH
// const char* ini_filename	�ݒ�t�@�C���t���p�X
// int nbuff		�o�b�t�@��
//------------------------------------------
BOOL AvalACL2Base::Init(int board_id, int board_ch_id, const char* ini_filename, int nbuff)
{
	my_nBoardChId = board_ch_id;

	// =========================================================
	// �A�N�Z�X�{�[�h����

	// �{�[�h���擾
    ACAPBOARDINFO	bi;
	if( ! AcapGetBoardInfo(&bi) ) {
		return FALSE;		// �{�[�h��� or ���C�u���� �ɃA�N�Z�X���s
	}

	// �{�[�h���擾
	char* cBoardName = NULL;
	int ii;
	for( ii=0; ii<bi.nBoardNum; ii++) {
		if( bi.BOARDINDEX[ii].nBoardID == board_id ) {
			cBoardName = bi.BOARDINDEX[ii].pBoardName; 
			break;
		}
	}

	if( ii == bi.nBoardNum ){
		return FALSE;		// ��v�{�[�h����
	}


	// =========================================================
	// �h���C�o �A�N�Z�X

	// open device
	my_hDev = AcapOpen(cBoardName, board_id, my_nBoardChId );
	if( INVALID_HANDLE_VALUE == my_hDev ) {
		return FALSE;		// �f�o�C�X�A�N�Z�X���s
	}

	// inifile
	if( ! AcapSelectFile(my_hDev, my_nBoardChId, (char*)ini_filename, 0) ) {
		return FALSE;		// ini�t�@�C��NG
	}

	// �ݒ�l�Ǎ�
	//if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_X_SIZE, 0, (int*)&my_nWidth ) ) return FALSE;
	//if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_Y_SIZE, 0, (int*)&my_nHeight ) ) return FALSE;
	//if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_CAM_BIT, 0, (int*)&my_nPixelNBit ) ) return FALSE;
	if( ! reload_size() ) return FALSE;
    if( my_nPixelNBit < 8 || my_nPixelNBit > 32 ) return FALSE;

	// buffer
	my_nBufNum = nbuff;
	if( ! AcapSetInfo(my_hDev, my_nBoardChId, ACL_MEM_NUM, 0, my_nBufNum ) ) return FALSE;

	//int pixel_nbyte = 0;
	//if(8 == my_nPixelNBit)							pixel_nbyte = sizeof(BYTE);
	//if(8 <  my_nPixelNBit && my_nPixelNBit <= 16 )	pixel_nbyte = sizeof(USHORT);
	//if(16<  my_nPixelNBit)							pixel_nbyte = sizeof(ULONG);
	//
	//my_nBufSize = my_nWidth * my_nHeight * my_nPixelNBit;
	my_pBuff.resize(my_nBufNum);					// �̈�m��
	// my_buff��AcapGetBufferAddress�̃A�h���X��DMA�I�v�V�����ŕς�邽�߁AStartGrab���O�ɒl�����邱�Ƃɂ���

	// acap event
	if( ! AcapSetEvent(my_hDev, my_nBoardChId, ACL_INT_FRAMEEND, TRUE ) ) return FALSE;


	ClearStatCounters();
	return TRUE;
}

//------------------------------------------
// SetInfo�ŃT�C�Y���ς�����ꍇ��my_width�����Đݒ�
//------------------------------------------
BOOL AvalACL2Base::reload_size()
{
	if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_X_SIZE, 0, (int*)&my_nWidth ) ) return FALSE;
	if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_Y_SIZE, 0, (int*)&my_nHeight ) ) return FALSE;
	if( ! AcapGetInfo(my_hDev, my_nBoardChId, ACL_BOARD_BIT, 0, (int*)&my_nPixelNBit ) ) return FALSE;

	int pixel_nbyte = 0;
	if(8 == my_nPixelNBit)							pixel_nbyte = sizeof(BYTE);
	if(8 <  my_nPixelNBit && my_nPixelNBit <= 16 )	pixel_nbyte = sizeof(USHORT);
	if(16<  my_nPixelNBit)							pixel_nbyte = sizeof(ULONG);
	
	my_nBufSize = my_nWidth * my_nHeight * my_nPixelNBit / 8;
	return TRUE;
}

//------------------------------------------
// �J��
//------------------------------------------
void AvalACL2Base::Dispose()
{
	if( INVALID_HANDLE_VALUE != my_hDev ) {
		AcapClose(my_hDev, my_nBoardChId);
		my_hDev = INVALID_HANDLE_VALUE;
	}
	my_nBoardChId = 0;
	my_pBuff .clear();
	my_nBufNum	= 0;
	my_nBufSize = 0;

	my_nWidth		= 0;
	my_nHeight		= 0;
	my_nPixelNBit	= 0;
	my_bRunning		= false;
}

//------------------------------------------
// �B���J�n
//------------------------------------------
BOOL AvalACL2Base::StartGrab()
{
	if( my_bRunning ) return FALSE;
	my_nLastFrom =  0;

	// DMA�o�b�t�@�A�h���X�m��
	for(DWORD ii=0; ii<my_nBufNum; ii++ ) {
		if( ! AcapGetBufferAddress(my_hDev, my_nBoardChId, ACL_IMAGE_PTR, ii+1, &my_pBuff[ii] ) ) {
			return FALSE;
		}
	}

	// �B���J�n
	if( ! AcapGrabStart(my_hDev, my_nBoardChId, 0)) {
		return FALSE;
	}

	my_bRunning	 = true;
	return TRUE;
}

//------------------------------------------
// �B����~
//------------------------------------------
BOOL AvalACL2Base::StopGrab()
{
	if( ! my_bRunning ) return FALSE;

	// ������~
	if( ! AcapGrabAbort(my_hDev, my_nBoardChId) ) {
	//	ACAPERRORINFO aei;
	//	AcapGetLastErrorCode(&aei, FALSE);
 //       return FALSE;
	//}

	//// ��~
 //   if( ! AcapGrabStop(my_hDev, my_nBoardChId) ) {
	//	ACAPERRORINFO aei;
	//	AcapGetLastErrorCode(&aei, FALSE);
        return FALSE;
    }

    my_bRunning = false;
    return TRUE;

}


//------------------------------------------
// ���̓t���[�����̎擾
// DWORD& nFrame	�t���[��NO ���Q�ƂȂ̂Œ���
//------------------------------------------
AvalACL2Base::FrameWaitResult AvalACL2Base::GetReadyFrameCount(DWORD& nFrameCount)
{
	nFrameCount = 0;
	DWORD	fp;

	// fp�擾�����t���[�����ł��邪�ADMA�������ݒ��̃������C���f�b�N�X�ƌ����Ă��ǂ�
	if( ! AcapGetFrameNo(my_hDev, my_nBoardChId, (int*)&fp, NULL, NULL)) {
		return FrameWait_ACLError;
	}


	AvalACL2Base::FrameWaitResult ret = FrameWait_Success;

	if( fp >= my_nLastFrom + my_nBufNum) {		// my_last_from+my_last_count���ǂ�ł��Ȃ��f�[�^�̐擪index
		ret = FrameWait_Overrun;
	}
	nFrameCount = fp - my_nLastFrom ;	
	return ret;
}

//------------------------------------------
// �V�K�t���[�����̎擾 (�t���[�����͂܂Ńu���b�N)
// DWORD& nFrame	�t���[��NO ���Q�ƂȂ̂Œ���
// DWORD nTimeout	�ҋ@���� [ms]
//------------------------------------------
AvalACL2Base::FrameWaitResult AvalACL2Base::WaitNFrameReady(DWORD& nFrameCount, DWORD nTimeout)
{
	nFrameCount = 0;
	if( 0 < nTimeout ){
		if( ! AcapWaitEvent(my_hDev, my_nBoardChId, ACL_INT_FRAMEEND,  nTimeout)) {
			ACAPERRORINFO aei;
			AcapGetLastErrorCode(&aei, FALSE);
			if (ACL_ABORT_EVENT_WAIT == aei.dwBoardErrorCode) return FrameWait_Aborted;
			if (ACL_EVENT_TIMEOUT_ERROR ==aei.dwCommonErrorCode)return FrameWait_Timeout;
			if (ACL_WAIT_EVENT_ERROR   ==  aei.dwCommonErrorCode)return FrameWait_Timeout;
			return FrameWait_ACLError;
		}
	}

	return GetReadyFrameCount(nFrameCount); 
}

//------------------------------------------
// �|�C���^�̎擾
//		WaitNFrameReady��葽���Ă����Ȃ��Ă��ǂ�
// void**	ptrs			�摜�|�C���^�z��
// DWORD	ptrslen			�擾�摜��
// DWORD&	nvalid			ptrs�̗L����(<= ptrslen)  �����ۂɎ擾�����摜��
// DWORD&	from_framenum	����̃t���[��No(�����̍ŏ�)
//------------------------------------------
AvalACL2Base::FrameWaitResult AvalACL2Base::GetFramePointers(void** ptrs, DWORD ptrslen, DWORD& nvalid, DWORD& from_framenum)
{
	AvalACL2Base::FrameWaitResult ret = GetReadyFrameCount(nvalid);		// nvalid ���Z�b�g����ċA���Ă���
	if (FrameWait_Overrun == ret) {
		my_nFrameOverrun += nvalid - my_nBufNum + 1;
	}

	// �|�C���^�X�V
	from_framenum	 = my_nLastFrom;
	nvalid			 = __min(nvalid, ptrslen);

	// �摜�|�C���^�Z�b�g
	for(DWORD ii=0; ii<nvalid; ii++) {
		ptrs[ii] = my_pBuff[ (my_nLastFrom + ii) % my_nBufNum ];
	}
	my_nLastFrom	+= nvalid;
	return ret;
}

//------------------------------------------
// �ǂݍ��ݒx��`�F�b�N
// �|�C���^�g�p��̃I�[�o�[�����`�F�b�N�A
// ���O��GetFramePointers�œ����͈͂�DMA���H������ł��Ȃ����ǂ���
//------------------------------------------
BOOL AvalACL2Base::CheckOverrun(int nReadFrameCount)
{
	DWORD	nFrameCount;
	if( FrameWait_Success != GetReadyFrameCount(nFrameCount) ) return FALSE;

	if( nFrameCount > nReadFrameCount + my_nBufNum -1 ) {
		my_nFrameOverrun += nFrameCount + nReadFrameCount - my_nBufNum + 1;
		return FALSE;
	}
	return TRUE;
}

//------------------------------------------
// �t���[���҂�(WaitNFrameReady)��Ԃ̉���
//------------------------------------------
void AvalACL2Base::AbortFrameWait()
{
    AcapSetEvent(my_hDev, my_nBoardChId, ACL_INT_FRAMEEND | ACL_INT_ABORT, TRUE);
}


// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �V���A��
//------------------------------------------
// Byte �� (16�i��HEX�\�L)
// char* buff		�ϊ���
// int buff_len		�ϊ���̈�T�C�Y
// const BYTE* data	��
// int nbytes		���T�C�Y
//------------------------------------------
static bool byte_to_hexstr(char* buff, int buff_len, const BYTE* data, int nbytes)
{
    int idx = 0;
    for(int i=0; i<nbytes; i++)
    {
        const int val[] =
            {
                (data[i] & 0xf0) >> 4,
                data[i] & 0x0f
            };
        const int nval = sizeof(val) / sizeof(val[0]);
        for(int j=0; j<nval; j++)
        {
            if( idx + j >= buff_len ) return false;

            buff[idx + j] = val[j] >= 10
                ? 'A' + val[j] - 10
                : '0' + val[j];
        }
        idx += nval;
    }
    if( idx >= buff_len ) return false;
    buff[idx] = '\0';
    return true;
}
//------------------------------------------
// (16�i��HEX�\�L) �� Byte
// char* buff		�ϊ���
// int buff_len		�ϊ���̈�T�C�Y
// const BYTE* data	��
// int nbytes		���T�C�Y
//------------------------------------------
static bool hexstr_to_byte(BYTE* buff, int buff_len, int& nbytes, const char* hexstr, int strlen)
{
    if( strlen % 2 ) return false;
    nbytes = strlen / 2;
    if( nbytes > buff_len ) return false;

    const int nshift[] = {4 , 0};
    for(int i=0, idx=0; i<strlen; i+=2, idx++)
    {
        buff[idx] = 0;

        for(int j=0; j<2; j++)
        {

            const int dig =
                hexstr[i + j] >= 'a' ? hexstr[i + j] - 'a' + 10 :
                hexstr[i + j] >= 'A' ? hexstr[i + j] - 'A' + 10 :
                hexstr[i + j] - '0';
            buff[idx] += dig << nshift[j];
        }
    }
    return true;
}

//------------------------------------------
// �I�[�v��
//------------------------------------------
BOOL AvalACL2Base::OpenSerialPort()
{
    return AcapSerialOpen(my_hDev, my_nBoardChId);
}

//------------------------------------------
// �V���A�� �p�����[�^�ݒ�
// int baud		�{�[���[�g (9600/19200/38400/.../115200)
//------------------------------------------
BOOL AvalACL2Base::SetSerialParam(int baud)
{
	return AcapSerialSetParameter( my_hDev, my_nBoardChId, baud, 
			/* �ȍ~�́A�h���C�o���x���ŌŒ�̂��ߕύX�s��*/
			8 /*int nDataBit*/,
			0 /*int nParity*/, 
			1 /*int nStopBit*/, 
			0 /*int nFlow*/);
}

//------------------------------------------
// �ؒf
//------------------------------------------
void AvalACL2Base::CloseSerialPort()
{
    AcapSerialClose(my_hDev, my_nBoardChId);
}

//------------------------------------------
// ���M
// const void* data ���M�f�[�^
// int len			���M�� 1024�ȉ�
//------------------------------------------
BOOL AvalACL2Base::SendToSerialPort(const void* data, int len)
{
    char send_buff[1024]; // data��ascii������
    if( !byte_to_hexstr(send_buff, sizeof(send_buff), (BYTE*)data, len) )
    {
        return FALSE;
    }
	// FALSE : 16�i��(HEX) �\�L
    if( !AcapSerialWrite(my_hDev, my_nBoardChId, FALSE, send_buff, NULL, NULL) )
    {
        return FALSE;
    }
    return TRUE;
}

//------------------------------------------
// ��M
// void* buff		��M�o�b�t�@
// int buff_len		�o�b�t�@��
// int& recv_len	��M��
// DWORD timeout	�҂����� [ms]	�� 0�őҋ@�����H�H
//------------------------------------------
BOOL AvalACL2Base::RecvFromSerialPort(void* buff, int buff_len, int&recv_len, DWORD timeout)
{
    char recv_buff[1024];
    int nrecv = 0;
	// FALSE : 16�i��(HEX) �\�L
    if( !AcapSerialRead(my_hDev, my_nBoardChId, FALSE, timeout, sizeof(recv_buff), NULL, recv_buff, &nrecv) )
    {
        return FALSE;
    }
    if( !hexstr_to_byte((BYTE*)buff, buff_len, recv_len, recv_buff, nrecv) )
    {
        return FALSE;
    }
    return TRUE;
}



// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ���܂���
//------------------------------------------
// �p�����[�^�ݒ�
// ULONG ulType	�ݒ�萔 (ACL_MEM_NUM �Ȃ�)
// int nValue	�l
//------------------------------------------
BOOL AvalACL2Base::SetInfo(DWORD ulType, int nValue)
{
	BOOL bRet = AcapSetInfo(my_hDev, my_nBoardChId, ulType, 0, nValue);
	reload_size();
	return bRet;
}

//------------------------------------------
// �p�����[�^�ǂݍ���
// DWORD ulType	�ݒ�萔 (ACL_MEM_NUM �Ȃ�)
// int& nValue	�l
//------------------------------------------
BOOL AvalACL2Base::GetInfo(DWORD ulType, int& nValue)
{
	return AcapGetInfo(my_hDev, my_nBoardChId, ulType, 0, &nValue);
}

//------------------------------------------
// �G���[���擾
// DWORD* err_board	
// DWORD* err_ext	
//------------------------------------------
DWORD AvalACL2Base::GetLastError(DWORD* err_board, DWORD* err_ext)
{
    ACAPERRORINFO err;
    AcapGetLastErrorCode(&err, FALSE);
    my_errcode_common = err.dwCommonErrorCode;
    my_errcode_board  = err.dwBoardErrorCode;
    my_errcode_ext    = err.dwExtendErrorCode;

    if( NULL != err_board ) *err_board = my_errcode_board;
    if( NULL != err_ext   ) *err_ext   = my_errcode_ext;

    return my_errcode_common;
}
