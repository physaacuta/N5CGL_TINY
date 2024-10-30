#include "StdAfx.h"
#include "ImgCompProc.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
// int nId �C���X�^���XNo
//------------------------------------------
ImgCompProc::ImgCompProc(int nId) :
ThreadManager( GetThreadNameArray("CompP", nId) ),
m_bIsExec(false),
m_bIsComp(false),
///mcls_Jpeg(),
mcls_Lac(-1)	// �Ǎ����� 
{
//	m_imgwk = new BYTE[2048*4096];
	m_nCompQuality = 0;
	m_nUionWidth = 0;
	m_nUionHeight = 0;

	mcls_Lac.SetThrId(nId);				// �X���b�hID���Z�b�g(LAC5�ł̓f�o�C�XID�Ɏg�p)
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
ImgCompProc::~ImgCompProc(void)
{
//	delete [] m_imgwk;
}

//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int ImgCompProc::ThreadFirst()
{
	ParamManager::P_MAIN::INS 	const& prmI	= mcls_pParam->GetMain().ins[0];

	//// DLL�Ǎ�
	if (EM_COMP_TYPE::COMP_TYPE_LAC == prmI.emCompType) {
		mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_LAC);
	} else if (EM_COMP_TYPE::COMP_TYPE_NUC == prmI.emCompType) {
		mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_NUC);
	} else if (EM_COMP_TYPE::COMP_TYPE_LAC5 == prmI.emCompType) {
		bool bRet = mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_LAC5);
		bool bSts = true;
		if (bRet) {
			int nRetc = mcls_Lac.InitEncoder(prmI.nUionWidth, prmI.nUionHeight, prmI.nCompQualityBase);		// �G���R�[�_������
			LOG(em_INF), "[%s] �G���R�[�_������ W:%d, H:%d Q:%d ret=%d", my_sThreadName, prmI.nUionWidth, prmI.nUionHeight, prmI.nCompQualityBase, nRetc );
			if (0 != nRetc) bSts = false;
		}
		else {
			LOG(em_ERR), "[%s] LAC5 DLL���[�h���s", my_sThreadName);
			bSts = false;
		}
		if (!bSts) {
			KizuFunction::SetStatus(mcls_pParam->GetMain().nStateId, false, false);		// �@���Ԉُ�
			syslog(510, "[%s] ��:%d ����:%d �i��:%d", my_sThreadName, prmI.nUionWidth, prmI.nUionHeight, prmI.nCompQualityBase);
		}

		m_nCompQuality = prmI.nCompQualityBase;
		m_nUionWidth = prmI.nUionWidth;
		m_nUionHeight = prmI.nUionHeight;
	}

	//// �C�x���g�ݒ�
	HANDLE hArray[] = { this->mque_pIn->GetEvQue() 
						};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);
	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int ImgCompProc::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void ImgCompProc::ThreadEvent(int nEventNo)
{
	////// �V�O�i���̏���
	enum {	EV_ADD_IMG = 0,					// �t���[���摜�L���[�C���O
	};

	////// �V�O�i����������
	switch (nEventNo) {

//-----------------------------------------------------------------------------------------------
	case EV_ADD_IMG: 						// �t���[���捞
		this->ExecQue();
		return;
	
//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] ThreadEvent EventNo NG=%d", my_sThreadName, nEventNo );
		return;
	}
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ����

//------------------------------------------
// ���Z�b�g
//------------------------------------------
void ImgCompProc::Reset()
{

}

//------------------------------------------
// ������
// Tin* pIn		����Que�f�[�^
// �߂�l		NULL:�ԋp���Ȃ� �o��Que�f�[�^:���̃X���b�h�֕ԋp����ꍇ  
//------------------------------------------
FrameDataManager* ImgCompProc::OnAddNewItem( FrameDataManager* p )
{
	if( NULL == p ) {	
		LOG(em_ERR), "[%s] �X���b�h���[�v�L���[����!", my_sThreadName);
		return p;
	}

	//// ����
	// ���̏����ł́A 4096*2048 * 4 �ŁA20msec���x�ŏI���B  ��������񏈗��͕K�v�Ȃ����ȁ[
	if(m_bIsExec) {
		m_bIsComp =true;
		p->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgComp_START);
		Exec(p);
		p->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgComp_END);
		m_bIsComp =false;
	} else {
		LOG(em_INF), "[%s]<%d> �L�����Z��", my_sThreadName, p->nIns );
	}

	return p;
}

//------------------------------------------
// �������s
// FrameDataManager* p �t���[�����
//------------------------------------------
void ImgCompProc::Exec(FrameDataManager* p)
{

	// =========================================================
	// �悭�g���p�����[�^���G�C���A�X
	ParamManager::P_MAIN		const& prm	= mcls_pParam->GetMain();
	ParamManager::P_MAIN::INS   const& prmI	= mcls_pParam->GetMain().ins[ p->nIns ];
	int nMen = prmI.nMen;

//	LOG(em_MSG), "[%s]<%d> start", my_sThreadName, p->nIns);

/*/
	CString s;
	s.Format("c:\\comp%d-%d.bmp", p->nColor, p->UdpData.framenum);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)p->pImgs, p->nLen );
	os.close();
	//CString sWk;
	//sWk.Format("c:\\testt_%d.bmp", p->nIns); 
	//KizuBitmapManager cls;
	//cls.DataToBMPfile_Gray(4096, 2048, p->pImgs, sWk);
//*/


	// =========================================================
	// �t���[����
	//		�{����
	bool bSkip		 = false;									// �摜�A���X�L�b�v�L��
	EM_COMP_TYPE emCompType = p->emCompType ;					// ���k�^�C�v
	int nCompQuality = p->nCompQuality;							// ���k��
	int nExecQueCnt  = mque_pIn->GetCount();

	if (EM_COMP_TYPE::COMP_TYPE_LAC5 == prmI.emCompType) {
		// ���k���̕ύX������ꍇ�AGPU��LAC�ł̓G���R�[�_�̏��������K�v
		if (m_nCompQuality != p->nCompQuality ||
			m_nUionWidth != p->nUnionWidth ||
			m_nUionHeight != p->nUnionHeight) {

			m_nCompQuality = p->nCompQuality;
			m_nUionWidth = p->nUnionWidth;
			m_nUionHeight = p->nUnionHeight;

			// �p�����[�^�ύX���ɑΉ�
			int nRetc = mcls_Lac.InitEncoder(m_nUionWidth, m_nUionHeight, m_nCompQuality);		// �G���R�[�_������
			LOG(em_INF), "[%s] �G���R�[�_������(��) W:%d, H:%d Q:%d ret=%d", my_sThreadName, m_nUionWidth, m_nUionHeight, m_nCompQuality, nRetc );
			if (!mcls_Lac.IsGpuEncoder()) {
				if (0 != nRetc) {
					KizuFunction::SetStatus(mcls_pParam->GetMain().nStateId, false, false);		// �@���Ԉُ�
					syslog(510, "��:%d ����:%d �i��:%d", m_nUionWidth, m_nUionHeight, m_nCompQuality);
				}
			}
			LOG(em_INF), "[%s] �G���R�[�_������(��) W:%d, H:%d Q:%d [�Ċm�ۂ͖��T�|�[�g]", my_sThreadName, m_nUionWidth, m_nUionHeight, m_nCompQuality );
		}
	}

	//// DLL �����[�h
	int nDll = mcls_Lac.IsLoadType();
	if( -1 != nDll) {
		// LAC �� NUC
		if( 0 == nDll && COMP_TYPE_NUC == emCompType ) {
			mcls_Lac.UnLoad();
			mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_NUC);
		}
		// NUC �� LAC
		if( 1 == nDll && COMP_TYPE_LAC == emCompType ) {
			mcls_Lac.UnLoad();
			mcls_Lac.Load(mcls_Lac.EM_DLL_TYPE::DLL_TYPE_LAC);
		}
	}




	// �Ԃɍ���Ȃ��̂ō����߂��ď������y�����Ē��������͂��킹��
	if( nExecQueCnt > (prm.nLoopQueFrameSize * 0.8) ) { 
		if (COMP_TYPE_LAC == emCompType) {
			bSkip = true;
			nCompQuality = 200;
		} else if (COMP_TYPE_NUC == emCompType) {
			bSkip = true;
			nCompQuality = -100;

		} else if (COMP_TYPE_LAC5 == emCompType) {
			bSkip = true;
			nCompQuality = 100;

		} else if (COMP_TYPE_JPEG == emCompType) {
			bSkip = true;
			nCompQuality = 1;
		}
	}
	if(bSkip) {
		LOG(em_ERR), "[%s] ���k�X�L�b�v", my_sThreadName);
		memset(&p->pImgs[prm.nBmpHeadSize], 0x00, p->nUnionWidth * p->nUnionHeight);
	}


	// =========================================================
	// ���k
	BYTE* pWk = &p->pBuf[RPIFrameSizeInfo::LINK_SIZE_Cf];		// �g���₷���悤�ɉ摜�擪�ʒu���擾���Ă���
	int	nCompSize = 0;

//if(0 == pWk ) {
//	_ASSERT(false);
//}



	//// JPEG
	if( EM_COMP_TYPE::COMP_TYPE_JPEG == emCompType) {
/***
		int retc = mcls_Jpeg.BmpToJpeg( p->nUnionWidth, p->nUnionHeight, &p->pImgs[prm.nBmpHeadSize], nCompQuality, &nCompSize, pWk);
		if( 0 != retc) {
			LOG(em_ERR), "[%s]<%d> JPEG���k���s [%d]", my_sThreadName, p->nIns, retc);
		}
***/

	//// RAW
	} else if (EM_COMP_TYPE::COMP_TYPE_RAW == emCompType) {
		nCompSize = p->nUnionWidth * p->nUnionHeight;
		memcpy( pWk, &p->pImgs[prm.nBmpHeadSize], nCompSize);


	//// BMP
	} else if (EM_COMP_TYPE::COMP_TYPE_BMP == emCompType) {
		nCompSize = p->nLen;
		memcpy( pWk, &p->pImgs[0], nCompSize);

	//// LAC
	} else if (EM_COMP_TYPE::COMP_TYPE_LAC == emCompType) {

		LacManager::LacStatus retc = mcls_Lac.LacEncode(nCompQuality, p->nLen, &p->pImgs[0], p->nLen, pWk, &nCompSize);
LOG(em_MSG), "[%s]<%d> %d LAC���k [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, retc, pWk);

//LacManager::LacStatus retc = mcls_Lac.LacEncode(nCompQuality, p->nLen, &p->pImgs[0], p->nLen, m_imgwk, &nCompSize);
//LOG(em_MSG), "[%s]<%d> %d LAC���k [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, retc, m_imgwk);
//memcpy(pWk, m_imgwk, nCompSize);

		if(LacManager::LacStatus::LacStatus_Ok != retc) {
			LOG(em_ERR), "[%s]<%d>(%d) LAC���k���s [%d]", my_sThreadName, p->nIns, p->nCFno, retc);
		}

	//// NUC
	} else if (EM_COMP_TYPE::COMP_TYPE_NUC == emCompType) {
		LacManager::LacStatus retc = mcls_Lac.LacEncode(nCompQuality, p->nLen, &p->pImgs[0], p->nLen, pWk, &nCompSize);
		if(LacManager::LacStatus::LacStatus_Ok != retc) {
			LOG(em_ERR), "[%s]<%d> NUC���k���s [%d]", my_sThreadName, p->nIns, retc);
		}
		//// LAC
	}
	else if (EM_COMP_TYPE::COMP_TYPE_LAC5 == emCompType) {
		if (mcls_Lac.IsGpuEncoder()) {				// GPU��LAC�G���R�[�_�[������

			//// ���̓f�[�^�̃������̈�m�ۏ����́A�X���b�h���[�v�L���[���m�ێ��Ɏ��{
			//////if (p->nLen > m_GpuImgSize) {
			//////	if (m_GpuImgs) mcls_Lac.GpuFreeHost(m_GpuImgs);
			//////	m_GpuImgs = (BYTE*)mcls_Lac.GpuMallocHost(p->nLen);
			//////}
			// ���̓f�[�^��LAC5�p�̃������̈�ɃR�s�[(�r�b�g�}�b�v�w�b�_�[�͕s�v)
			int nRetc = mcls_Lac.LacEncodeGpu(&p->pImgs[prm.nBmpHeadSize], (p->nLen-prm.nBmpHeadSize), pWk, &nCompSize);
			//LOG(em_MSG), "[%s]<%d> %d LAC5���k [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, nRetc, pWk);

			//LacManager::LacStatus retc = mcls_Lac.LacEncode(nCompQuality, p->nLen, &p->pImgs[0], p->nLen, m_imgwk, &nCompSize);
			//LOG(em_MSG), "[%s]<%d> %d LAC5���k [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, retc, m_imgwk);
			//memcpy(pWk, m_imgwk, nCompSize);

			if (LacManager::LacStatus::LacStatus_Ok != nRetc) {
				LOG(em_ERR), "[%s]<%d>(%d) LAC5���k���s [%d]", my_sThreadName, p->nIns, p->nCFno, nRetc);
			}

#if 0		// LAC5�̉𓀎���(���C�u�����̎����p�̂��߁A����������폜�\��)
			{
				KizuBitmapManager clsBmpMgr;
				int nBmpHeadSz = clsBmpMgr.GetBitmapHeaderSize_Gray();
				BYTE* m_bOutImg = new BYTE[p->nLen + nBmpHeadSz];
				int nRetc = mcls_Lac.LacDecodeGpu(pWk, nCompSize, &m_bOutImg[nBmpHeadSz]);
				LOG(em_MSG), "[%s]<%d> %d LAC5�� [rec=%d] [%p]", my_sThreadName, p->nIns, p->nCFno, nRetc, m_bOutImg);

				clsBmpMgr.CreateBitmapHeader_Gray(m_nUionWidth, m_nUionHeight, &m_bOutImg[0], false);	// BMP�w�b�_�[�t�^
				CString s;
				s.Format("c:\\decode%d.bmp", p->nIns);
				std::ofstream   os;
				os.open(s, std::ios::binary);
				os.write((const char*)m_bOutImg, p->nLen);		// BMP�t�@�C���o��
				os.close();
			}
#endif
		}
		else {
			LOG(em_ERR), "[%s]<%d>(%d) LAC5���k���s (�G���R�[�_�[�������s)", my_sThreadName, p->nIns, p->nCFno);
		}
	}
	
	// ���̎��_�ł̓Z�N�^�܂ł͍l�����Ă��Ȃ� (=���Ȃ��ėǂ�)
	p->nCompSize = nCompSize;
	p->nSetSize  = RPIFrameSizeInfo::LINK_SIZE_Cf + nCompSize;	


/*/
	p->cls_Time.PutTimestamp(FrameDataManager::TIME_ImgComp_END);
	double dWk =p->cls_Time.GetTimestamp( FrameDataManager::TIME_ImgComp_END, FrameDataManager::TIME_ImgComp_START );

	LOG(em_MSG), "[%s]<%d> ���k����(%dms) [%d��%d=%0.3f%%]", my_sThreadName, 
		p->nIns, (int)dWk, p->nLen, nCompSize, (double)nCompSize/(double)p->nLen*100);
//*/

	// =========================================================
	// ���̑��f�[�^�t�^
	//// �t�����
	if(0 == prm.nAddFrameInfoType) {
		p->pFdata		= NULL;
		p->nFdataSize	= 0;
		p->nSetSize		+= p->nFdataSize;
	} else if(1 == prm.nAddFrameInfoType) {
		p->pFdata		= &p->pBuf[RPIFrameSizeInfo::LINK_SIZE_Cf + nCompSize];	
		p->nFdataSize	= sizeof(RpiLib::T_RPI_FRAME_INFO_T1);
		p->nSetSize		+= p->nFdataSize;

		// �f�[�^�Z�b�g
		RpiLib::T_RPI_FRAME_INFO_T1* pF = (RpiLib::T_RPI_FRAME_INFO_T1*)p->pFdata;
		pF->nFNo		= p->UdpData.framenum;
		pF->nSpeed		= p->UdpData.speed;
		pF->vRes		= p->UdpData.vRes;
		pF->nFromLen	= p->UdpData.pos[nMen].posFromHead;
		pF->dEdgePos[0]	= p->UdpData.pos[nMen].edgePosL;
		pF->dEdgePos[1]	= p->UdpData.pos[nMen].edgePosR;
		pF->nEdgeIdx[0]	= p->nEdgeIdx[0];
		pF->nEdgeIdx[1]	= p->nEdgeIdx[1];

	}


	//// ���k���I������^�C�~���O�Ŋ��ɃO���o�[OFF�ɂȂ��Ă���H
	if( ! m_bIsExec ) {
		p->bRpiSave = false;
	}


/*/
	CString s;
	s.Format("c:\\comp%d.jpg", p->nIns);
	std::ofstream   os;
	os.open(s, std::ios::binary);
	os.write((const char*)p->pImgComp, p->nCompSize );
	os.close();
//*/
}