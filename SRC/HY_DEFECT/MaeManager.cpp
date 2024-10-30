#include "StdAfx.h"
#include "MaeManager.h"

// �������[���[�N���o ----->>>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
// <<<------------------------

//------------------------------------------
// �R���X�g���N�^
//------------------------------------------
MaeManager::MaeManager(void) :
ThreadManager("MaeManager")
{
}

//------------------------------------------
// �f�X�g���N�^
//------------------------------------------
MaeManager::~MaeManager(void)
{
}

////////////////////////////////////////////////////////////////////////////////
// �X���b�h
////////////////////////////////////////////////////////////////////////////////
//------------------------------------------
// �X���b�h�J�n�O���� (�X���b�h���s����(�V�O�i���Z�b�g�O)�ɃR�[���o�b�N)
//------------------------------------------
int MaeManager::ThreadFirst()
{
	// �C�x���g�ݒ�
	HANDLE hArray[] = {
		mque_pMaeRequest->g_evSem
	};
	SetEventNum(sizeof(hArray)/sizeof(HANDLE), hArray);

	return 0;
}

//------------------------------------------
// �X���b�h�I���O���� (�I���V�O�i����ɃR�[���o�b�N)
//------------------------------------------
int MaeManager::ThreadLast()
{
	return 0;
}

//------------------------------------------
// �X���b�h�C�x���g����
// int nEventNo �C�x���gNo (0�I���W��)
//------------------------------------------
void MaeManager::ThreadEvent(int nEventNo)
{
	// �V�O�i���̏���
	enum {
		EV_MAE_READ = 0,									// �O�H�����Ǎ�
	};

	// �V�O�i����������
	switch(nEventNo){
	//-----------------------------------------------------------------------------------------------
	case EV_MAE_READ: 										// �O�H�����Ǎ�
		LOG(em_MSG), "[%s] EventNo=%d OK", my_sThreadName, nEventNo );
		TYP_MAE_REQUEST* pQue;								// �L���[���
		pQue = mque_pMaeRequest->GetFromHead();				// �L���[���擾
		if( 0 == pQue->nMode ){								// �O�H�����Ǎ�
			ReadMaeInf(pQue);
		}
		delete pQue;										// �L���[���J��
		return;

	//-----------------------------------------------------------------------------------------------
	default :
		// ���肦�Ȃ��I�I
		LOG(em_ERR), "[%s] EventNo=%d NG", my_sThreadName, nEventNo );
		return;
	}
}

//------------------------------------------
// �O�H���擾�󋵎擾
//------------------------------------------
// TYP_MAE_REQUEST*	pQue	�L���[���
//------------------------------------------
int MaeManager::GetEnableData(TYP_MAE_REQUEST* pQue)
{
	int	nRet;							// ���^�[���R�[�h
	int	nFFRecvFlg;						// FF�O�H���r����M�t���O
	int	nEnableData;					// �O�H���擾��

	CString	sSql;						// SQL��
	CString	sTbl;						// �e�[�u����
	CString	sTmp;						// �����񃏁[�N
	CString	sTmp2;						// �����񃏁[�N

	OdbcBase	clsDB;					// ODBC�N���X

	// ������
	nEnableData	= 0;					// �O�H���擾��[���擾]

	LOG(em_INF), "�A�O�H���擾�󋵎擾 �J�n");

	// �f�[�^�x�[�X�ڑ�
	if( false == clsDB.Connect(TASKINI_NAME) ){				// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
	} else{
		// �e�[�u����
		sTbl	= DB_MAE_COIL_INF;							// �O�H���R�C�����
		sTmp.Format("'%s'", pQue->cKanriNo);				// �Ǘ���
															// SQL���쐬
		sSql	= "SELECT";
		sSql	+= " FF�O�H���r����M�t���O";
		sSql	+= " FROM ";
		sSql	+= sTbl;									// �O�H���R�C�����
		sSql	+= " WHERE";
		sSql	+= " �Ǘ�No=";
		sSql	+= sTmp;

		// SQL���s
		if( false == clsDB.ExecuteDirect(sSql) ){			// ���s
			LOG(em_ERR), "[%s] SQL���s�G���[", my_sThreadName);
			LOG(em_NON), "%s", sSql);
			syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
		} else{												// ����
			nRet	= clsDB.Fetch();						// ���R�[�h�Q��
			if( KIZUODBC_FETCH_OK == nRet ){				// ���R�[�h�L��
				clsDB.GetData(1, &nFFRecvFlg);				// FF�O�H���r����M�t���O
				if( 0 == nFFRecvFlg ){						// FF�O�H���r����M�ς�
					nEnableData	= 1;						// �O�H���擾��[�擾]
				}
			} else if( KIZUODBC_FETCH_NODATE == nRet ){		// ���R�[�h����

			} else {										// �Q�ƃG���[
				LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
				LOG(em_NON), "%s", sSql);
				syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
			}
		}
	}

	LOG(em_INF), "�A�O�H���擾�󋵎擾 �����@�擾����=[%d]", nEnableData);

	return nEnableData;
}

//------------------------------------------
// �O�H���e�e���o�^
//------------------------------------------
// �O�H���e�e�������L�������֓o�^����
//------------------------------------------
// TYP_FF_INF*	pFFInf	�O�H���e�e���
//------------------------------------------
// �߂�l	�o�^�ʒu[1:�I���W��]
//------------------------------------------
int MaeManager::RegFFInf(TYP_FF_INF* pFFInf, const int nSetPnt, const int nMode)
{
	LOG(em_INF), "�C�O�H���e�e���o�^ �J�n�@FF�Ǘ�No=[%s] FF�rNo=[%d]", pFFInf->cMaeKizukenNo, pFFInf->nMaeKizuNo);

	int	nMenID	= pFFInf->nMenID;							// �\���敪
	//int	nPnt	= m_pMaeInf->rec[m_nRec].men[nMenID].num;	// �o�^�ʒu
	int nPnt = 0;
	if (1 == nMode) {
		nPnt = nSetPnt;
	}
	else {
		nPnt = m_pMaeInf->rec[m_nRec].men[nMenID].num;	// �o�^�ʒu
	}
	if( nPnt < TBL_HY_MAE_INF_MAX ){	// �L���o�^�ʒu
		// �O�H���r�̕\����� (0�F�r����   1:�O�H���r���A�r�摜�\��  2:FF���̂ݕ\��) �� MAE_MASTER����擾�����l
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].nMaeDsp			= pFFInf->nMaeDsp;
		// FF�Ǘ�No (�O�H���ł̊Ǘ�No)
		memcpy( m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.cMaeKizukenNo,	pFFInf->cMaeKizukenNo,	SIZE_KIZUKEN_NO);
		// FF�rNo       (�O�H���ł��rNo)
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nMaeKizuNo	= pFFInf->nMaeKizuNo;
		// FF�r��
		memcpy( m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.cT, pFFInf->cFF_Type,	2);
		// FF�O���[�h
		memcpy( m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.cG, pFFInf->cFF_Grade,	2);
		// FF�r���o�H��
		memcpy( m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.cProc,	pFFInf->cFF_Koutei,	2);
		// FF�r�敪
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nDiv = pFFInf->nFF_Kubun;
		// FF�r���������ʒu [mm]	������͎g��Ȃ�
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nPos = pFFInf->nFF_LPos;
		// FF�r�Е����ʒu [mm]		(DS����̈ʒu)
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nWidth = pFFInf->nFF_WPos;
		// FF�r���� [mm]
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nLen = pFFInf->nFF_Length;
		// �ŏI����ʒu [mm]
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nY = pFFInf->nLastPos;
		// MAE_TYPE_NAME  �� ID  (0:�s��   1�`�F�O�H���̑Ώ��r) ���s���̏ꍇ�́AFF���ŕ\���B�}�X�^�[����`��
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nTid = pFFInf->nType;
		// MAE_GRADE_NAME �� ID  (0:�s��)
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nGid = pFFInf->nGrade;
		// <20221030><�a��><2TCM���@���ɔ������H���̉���> --->>>
		m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].ff.nKouteiID = pFFInf->nKouteiID;
		// <20221030><�a��><2TCM���@���ɔ������H���̉���> <<<---

		// �o�^�ʒu�X�V
		if (0 == nMode) {
			m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt].nKizuNo = pFFInf->nKizuNo;

			nPnt++;
			m_pMaeInf->rec[m_nRec].men[nMenID].num = nPnt;
		}
	} else{								// �����o�^�ʒu[�I�[�o�[]
		nPnt	= 0;
	}

	LOG(em_INF), "�C�O�H���e�e���o�^ �����@�o�^�ʒu=[%d]", nPnt);

	return	nPnt;
}

static int GetKizuKenNoLen(char *cKizuKenNo)
{
	int ii;
	ii = 0;
	for(ii = SIZE_KIZUKEN_NO - 1; ii >= 0; ii--) {
		if((0x00 != cKizuKenNo[ii]) &&
           (0x20 != cKizuKenNo[ii])){
			// NULL�A�X�y�[�X�ȊO�̂Ƃ�
			break;
		}
	}
	return ii+1;
}

//------------------------------------------
// �O�H���r���o�^
//------------------------------------------
// �O�H���r�������L�������֓o�^����
//------------------------------------------
// TYP_FF_INF*		pFFInf	�e�e�r���
// TYP_MAE_DEFECT*	pMaeDef	�O�H���r���
//------------------------------------------
void MaeManager::RegMaeDefect(TYP_FF_INF* pFFInf, TYP_MAE_DEFECT* pMaeDef)
{
	int	nMenID	= pFFInf->nMenID;							// �\���敪
	int	nPnt	= m_pMaeInf->rec[m_nRec].men[nMenID].num;	// �o�^�ʒu[1:�I���W��]

	// �摜�`�� (0:Jpeg 1:BMP)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.nImageFlg	= pFFInf->nImgFmt;
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� ----->>>
	if (0 == memcmp(pFFInf->cMwk01, KOUTEI_CODE_2TCM, 2)) {
		if (0 == strncmp(pFFInf->cMaeKizukenNo, KIZUKEN_CODE_2TCM, SIZE_KIZUKEN_CODE)) {
			m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt - 1].inf.data.nImageFlg = 1;
		}
		else {
			m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt - 1].inf.data.nImageFlg = pFFInf->nImgFmt;
		}
	}
	// 20200830 �≄�@�������u�����@�@�n�[�h�E�\�t�g�i2TCM�����X�V�j �a�� <<<-----

	// ���O�ҏW�t���O (-1:��\��  0:�ʏ�\��  1:�����\��) ���v���r�W����
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.nDspMode		= pFFInf->nPreEdit;
	// �\���敪  (0:�\ 1:��)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.men			= pMaeDef->nMenID;
	// �����r�敪(0:����r, 1:�ȈՎ����r,�A���r 2:��������)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.cycle_div	= pMaeDef->nCycle;
	// DSWS�敪  (0:DS 1:WS)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.dsws			= pMaeDef->nDSWS;

	// ���x[mpm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.speed		= pMaeDef->nLSpeed;
	// �s�b�`[mm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.pitch		= pMaeDef->nPitch;
	// ���[���a[mm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.roolkei		= pMaeDef->fKei;
	// �r�� [mm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_wid		= pMaeDef->nKizuW;
	// �r���� [mm]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_len		= pMaeDef->nKizuL;

	// �c����\[mm/pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.h_res		= pMaeDef->fVDenc;
	// ������\[mm/pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.w_res		= pMaeDef->fHDenc;
	// �O��Xmin[pixel]		(�؂�o���摜�̍��ォ��̈ʒu) (�}�C�i�X�l���肦��)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_x_min	= pMaeDef->nEXmin;
	// �O��Xmax[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_x_max	= pMaeDef->nEXmax;
	// �O��Ymin[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_y_min	= pMaeDef->nEYmin;
	// �O��Ymax[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.box_y_max	= pMaeDef->nEYmax;
	// �r���S�ʒuX [pixel]  (�؂�o���摜�̍��ォ��̈ʒu)
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.center_x		= pMaeDef->nCPosX;
	// �r���S�ʒuY [pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.center_y		= pMaeDef->nCPosY;
	// �c��f��[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.h_size		= pMaeDef->nVPixel;
	// ����f��[pixel]
	m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.w_size		= pMaeDef->nHPixel;

	//// �ŏI����ʒu [mm]
	//m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data[nSpm].nY			= pMaeDef->nLPos;
	//// MAE_TYPE_NAME  �� ID  (0:�s��   1�`�F�O�H���̑Ώ��r)
	//m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data[nSpm].nTid			= pFFInf->nType;
	//// MAE_GRADE_NAME �� ID  (0:�s��)
	//m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data[nSpm].nGid			= pFFInf->nGrade;
}



//------------------------------------------
// �O�H���r�摜�Ǎ�
//------------------------------------------
// �O�H���r�摜��ǂݍ��݃��[�J���t�@�C���֊i�[����
//------------------------------------------
// TYP_FF_INF*		pFFInf	�e�e�r���
// TYP_MAE_DEFECT*	pMaeDef	�O�H���r���
//------------------------------------------
bool MaeManager::ReadMaeImg(TYP_FF_INF* pFFInf, TYP_MAE_DEFECT* pMaeDef)
{
	int	nImgFmt;						// �摜�`��
	int	nMenID;							// �\���敪
	int	nPnt;							// �o�^�ʒu[1:�I���W��]
	int	nRet;							// ���^�[���R�[�h
	int	nSize[MAX_IMAGE_CNT];			// �摜�T�C�Y

	char		cPath[SIZE_IMAGE_PATH];	// �摜�p�X
	char*		pPath[MAX_IMAGE_CNT];	// �摜�p�X�|�C���^
	char const*	pFolder;				// �O�H���摜�t�H���_�[

	CString		sPath;					// �摜�p�X
	CString		sSql;					// SQL��
	CString		sTbl;					// �e�[�u����

	OdbcBase	clsDB;					// ODBC�N���X

	static const char	cImgExt[2][4]={
		"jpg", "bmp"
	};

	LOG(em_INF), "�E�O�H���r�摜�Ǎ� �J�n�@FF�Ǘ�No=[%s] FF�rNo=[%d]", pFFInf->cMaeKizukenNo, pFFInf->nMaeKizuNo);

	nMenID	= pFFInf->nMenID;							// �\���敪
	nPnt	= m_pMaeInf->rec[m_nRec].men[nMenID].num;	// �o�^�ʒu[1:�I���W��]

	// �O�H���摜�t�H���_�[
	pFolder	= mcls_pDataSet->GetMaeImgPath();

	// �摜�`��
	nImgFmt	= (int)m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.data.nImageFlg;

	// �摜�p�X
	sPath.Format("%s\\rec%d\\%d_%d", pFolder, m_nRec, pMaeDef->nMenID, pMaeDef->nKizuNo);

	// �摜�T�C�Y
	nSize[0]	= pMaeDef->nPSize1;
	nSize[1]	= pMaeDef->nPSize2;
	nSize[2]	= pMaeDef->nPSize3;
	nSize[3]	= pMaeDef->nPSize4;

	// �摜�p�X�|�C���^
	pPath[0]	= m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.image_path_1;
	pPath[1]	= m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.image_path_2;
	pPath[2]	= m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.image_path_3;
	pPath[3]	= m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[nPnt-1].inf.image_path_4;

	// �f�[�^�x�[�X�ڑ�
	if( false == clsDB.Connect(TASKINI_NAME) ){				// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// �e�[�u����
	sTbl	= DB_MAE_DEFECT_IMG;							// �O�H���r�摜

	// SQL���쐬
	sSql.Format(
		"SELECT �摜1, �摜2, �摜3, �摜4 FROM %s WHERE �Ǘ�No='%s' AND �rNo=%d",
		sTbl, pMaeDef->cKizukenNo, pMaeDef->nKizuNo
	);

	// SQL���s
	if( false == clsDB.ExecuteDirect(sSql) ){
		LOG(em_ERR), "[%s] SQL���s�G���[ ", my_sThreadName);
		LOG(em_NON), "%s", sSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
		return false;
	}

	// �e�[�u���f�[�^�擾
	while( TRUE ){											// �L�����R�[�h��
		nRet	= clsDB.Fetch();							// ���R�[�h�Q��
		if( KIZUODBC_FETCH_OK == nRet ){					// ���R�[�h�L��
			// ���R�[�h�f�[�^�擾
			for(int ii=0; ii<MAX_IMAGE_CNT; ii++){			// �摜����
				if( nSize[ii] > 0 ){						// �摜�L��
					// ������
					memset(cPath, 0x00, sizeof(cPath));
					// �摜�G���A�m��
					BYTE*	pImg	= new BYTE[nSize[ii]];
					// �摜�擾
					clsDB.GetDataImage(ii + 1, pImg, nSize[ii]);
					// �摜�p�X
					sprintf(cPath, "%s_%d.%s", (LPCTSTR)sPath, ii, cImgExt[nImgFmt]);
					// �摜�t�@�C���o��
					if (0 == nImgFmt){						// JPEG
						CFile	clsFile;					// �t�@�C���A�N�Z�X�N���X
						clsFile.Open(cPath, CFile::modeCreate | CFile::modeWrite);
						clsFile.Write(pImg, nSize[ii]);
						clsFile.Close();
						//std::ofstream	clsStream;
						//clsStream.open(cPath, std::ios::out | std::ios::trunc | std::ios::binary);
						//clsStream.write((char const*)pImg, nSize[ii]);
						//clsStream.close();
					} else{									// BMP

						KizuBitmapManager	clsBmp;			// Bitmap�摜�����N���X
						clsBmp.DataToBMPfile_Gray(pMaeDef->nHPixel, pMaeDef->nVPixel, pImg, cPath);
					}
					// �摜�p�X�o�^
					strcpy(pPath[ii], cPath);
					// �摜�G���A�J��
					SAFE_DELETE(pImg);
				}
			}
		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// ���R�[�h����
			break;
		} else {											// �Q�ƃG���[
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sSql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
			return false;
		}
	}

	LOG(em_INF), "�E�O�H���r�摜�Ǎ� ����");

	return true;
}

//------------------------------------------
// �O�H���r���Ǎ�
//------------------------------------------
// �O�H���r���e�[�u������Y���Ǘ������r����ǂݍ��݋��L�������֓o�^����
//------------------------------------------
// TYP_FF_INF*	pFFInf	�e�e���
//------------------------------------------
bool MaeManager::ReadMaeDefect(TYP_FF_INF* pFFInf)
{
	int				nRet;				// ���^�[���R�[�h

	CString			sSql;				// SQL��
	CString			sTbl;				// �e�[�u����
	CString			sTmp;				// �����񃏁[�N

	TYP_MAE_DEFECT	typMaeDef;			// �O�H���r���

	OdbcBase	clsDB;					// ODBC�N���X

	LOG(em_INF), "�D�O�H���r���o�^ �J�n�@FF�Ǘ�No=[%s] FF�rNo=[%d]", pFFInf->cMaeKizukenNo, pFFInf->nMaeKizuNo);

	// �f�[�^�x�[�X�ڑ�
	if( false == clsDB.Connect(TASKINI_NAME) ){				// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// �e�[�u����
	sTbl	= DB_MAE_DEFECT_INF;							// �O�H���r���

	// SQL���쐬
	sSql	= "SELECT";
	sSql	+= " �Ǘ�No";
	sSql	+= ",�rNo";
	sSql	+= ",�H�����";
	sSql	+= ",�\���敪";
	sSql	+= ",�����t���O";
	sSql	+= ",����ʒu";
	sSql	+= ",DSWS�敪";
	sSql	+= ",DS�ʒu";
	sSql	+= ",WS�ʒu";
	sSql	+= ",���C�����x";
	sSql	+= ",�s�b�`";
	sSql	+= ",�a";
	sSql	+= ",�r��";
	sSql	+= ",�r����";
	sSql	+= ",�c����\";
	sSql	+= ",������\";
	sSql	+= ",�O��Xmin";
	sSql	+= ",�O��Xmax";
	sSql	+= ",�O��Ymin";
	sSql	+= ",�O��Ymax";
	sSql	+= ",�r���S�ʒuX";
	sSql	+= ",�r���S�ʒuY";
	sSql	+= ",�c��f��";
	sSql	+= ",����f��";
	sSql	+= ",�摜�T�C�Y1";
	sSql	+= ",�摜�T�C�Y2";
	sSql	+= ",�摜�T�C�Y3";
	sSql	+= ",�摜�T�C�Y4";
	sTmp.Format(
		" FROM %s WHERE �Ǘ�No='%s' AND �rNo=%d AND �H�����=%d AND �\���敪=%d",
		sTbl, pFFInf->cKizukenNo, pFFInf->nKizuNo, pFFInf->nKouteiID, pFFInf->nMenID
	);
	sSql	+= sTmp;

	// SQL���s
	if( false == clsDB.ExecuteDirect(sSql) ){
		LOG(em_ERR), "[%s] SQL���s�G���[ ", my_sThreadName);
		LOG(em_NON), "%s", sSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
		return false;
	}

	// �e�[�u���f�[�^�擾
	while( TRUE ){											// �L�����R�[�h��
		nRet	= clsDB.Fetch();							// ���R�[�h�Q��
		if( KIZUODBC_FETCH_OK == nRet ){					// ���R�[�h�L��
			// ���R�[�h�f�[�^�擾
															// �Ǘ���
			clsDB.GetData( 1, typMaeDef.cKizukenNo, SIZE_KIZUKEN_NO);
			clsDB.GetData( 2, &typMaeDef.nKizuNo);			// �r��
			clsDB.GetData( 3, &typMaeDef.nKouteiID);		// �H�����
			clsDB.GetData( 4, &typMaeDef.nMenID);			// �\���敪[0:�\�A1:��]
			clsDB.GetData( 5, &typMaeDef.nCycle);			// �����t���O
			clsDB.GetData( 6, &typMaeDef.nLPos);			// ����ʒu[mm]
			clsDB.GetData( 7, &typMaeDef.nDSWS);			// DSWS�敪[0:DS�A1:WS]
			clsDB.GetData( 8, &typMaeDef.fDSPos);			// DS�ʒu[mm]
			clsDB.GetData( 9, &typMaeDef.fWSPos);			// WS�ʒu[mm]
			clsDB.GetData(10, &typMaeDef.nLSpeed);			// ���C�����x[mpm]
			clsDB.GetData(11, &typMaeDef.nPitch);			// �s�b�`[mm]
			clsDB.GetData(12, &typMaeDef.fKei);				// �a[mm]
			clsDB.GetData(13, &typMaeDef.nKizuW);			// �r��[mm]
			clsDB.GetData(14, &typMaeDef.nKizuL);			// �r����[mm]
			clsDB.GetData(15, &typMaeDef.fVDenc);			// �c����\[mm/pixel]
			clsDB.GetData(16, &typMaeDef.fHDenc);			// ������\[mm/pixel]
			clsDB.GetData(17, &typMaeDef.nEXmin);			// �O��Xmin[pixel]
			clsDB.GetData(18, &typMaeDef.nEXmax);			// �O��Xmax[pixel]
			clsDB.GetData(19, &typMaeDef.nEYmin);			// �O��Ymin[pixel]
			clsDB.GetData(20, &typMaeDef.nEYmax);			// �O��Ymax[pixel]
			clsDB.GetData(21, &typMaeDef.nCPosX);			// �r���S�ʒuX[pixel]
			clsDB.GetData(22, &typMaeDef.nCPosY);			// �r���S�ʒuY[pixel]
			clsDB.GetData(23, &typMaeDef.nVPixel);			// �c��f��[pixel]
			clsDB.GetData(24, &typMaeDef.nHPixel);			// ����f��[pixel]
			clsDB.GetData(25, &typMaeDef.nPSize1);			// �摜�T�C�Y1[byte]
			clsDB.GetData(26, &typMaeDef.nPSize2);			// �摜�T�C�Y2[byte]
			clsDB.GetData(27, &typMaeDef.nPSize3);			// �摜�T�C�Y3[byte]
			clsDB.GetData(28, &typMaeDef.nPSize4);			// �摜�T�C�Y4[byte]

			RegMaeDefect(pFFInf, &typMaeDef);				// �O�H���r���o�^[���L������]

			// �摜�L��(=1�͕ۑ����Ȃ�)
			if (1 != mcls_pDataSet->GetnMaeImgSaveOff())
			{
				ReadMaeImg(pFFInf, &typMaeDef);				// �O�H���r�摜�Ǎ�
			}

		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// ���R�[�h����
			break;
		} else {											// �Q�ƃG���[
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sSql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
			return false;
		}
	}

	LOG(em_INF), "�D�O�H���r���o�^ ����");

	return true;
}

//------------------------------------------
// �O�H���e�e���Ǎ�
//------------------------------------------
// �O�H���e�e�r���e�[�u������Y���Ǘ����̂e�e����ǂݍ��݋��L�������֓o�^����
//------------------------------------------
// TYP_MAE_REQUEST*	pQue		�L���[���
//------------------------------------------
bool MaeManager::ReadFFInf(TYP_MAE_REQUEST* pQue)
{
	int	nPnt;							// �o�^�ʒu[1:�I���W��]
	int	nRet;							// ���^�[���R�[�h

	CString	sSql;						// SQL��
	CString	sTbl;						// �e�[�u����
	CString	sTmp;						// �����񃏁[�N

	TYP_FF_INF	typFFInf;				// �O�H���e�e���

	OdbcBase	clsDB;					// ODBC�N���X

	LOG(em_INF), "�B�O�H���e�e���Ǎ� �J�n");

	// �f�[�^�x�[�X�ڑ�
	if( false == clsDB.Connect(TASKINI_NAME) ){				// �f�[�^�x�[�X�ڑ��G���[
		LOG(em_ERR), "[%s] DB�ڑ��G���[", my_sThreadName);
		syslog(SYSNO_DB_CONNECT_ERR, "");
		return false;
	}

	// �e�[�u����
	sTbl	= DB_MAE_FF_INF;								// �O�H���e�e�r���

	sTmp.Format("'%s'", pQue->cKanriNo);					// �Ǘ���

	// SQL���쐬
	sSql	= "SELECT";
	sSql	+= " i.�Ǘ�No";
	sSql	+= ",i.�rNo";
	sSql	+= ",i.�H�����";
	sSql	+= ",i.�\���敪";
	sSql	+= ",i.���O�ҏW�t���O";
	sSql	+= ",i.�O�H���Ǘ�No";
	sSql	+= ",i.�O�H���rNo";
	sSql	+= ",i.FF�r��";
	sSql	+= ",i.FF�O���[�h";
	sSql	+= ",i.FF�r���o�H��";
	sSql	+= ",i.FF�r�敪";
	sSql	+= ",i.FF�r���������ʒu";
	sSql	+= ",i.FF�r�Е����ʒu";
	sSql	+= ",i.FF�r����";
	sSql	+= ",i.�����r��";
	sSql	+= ",i.����O���[�h";
	sSql	+= ",i.�ŏI����ʒu";
	sSql	+= ",c.�Ǘ�No";
	sSql	+= ",c.�H�����";
	sSql	+= ",c.�\���敪";
	sSql	+= ",c.�����X�e�[�^�X";
	sSql	+= ",m.�O�H���R�[�h";
	sSql	+= ",m.�L������";
	sSql	+= ",m.�O�H���\�����[�h";
	sSql	+= ",m.�摜�`��";
	sSql	+= " FROM (";
	sSql	+= sTbl;										// �O�H���e�e�r���[i]
	sSql	+= " i";
	sSql	+= " INNER JOIN ";
	sSql	+= DB_MAE_COIL_LINE;							// �O�H�����C���ʃR�C�����[c]
	sSql	+=" c";
	sSql	+= " ON";
	sSql	+= " i.�Ǘ�No=c.�Ǘ�No";
	sSql	+= " AND i.�H�����=c.�H�����";
	sSql	+= " AND i.�\���敪=c.�\���敪";
	sSql	+= ")";
	sSql	+= " INNER JOIN ";
	sSql	+= DB_MAE_MASTER;								// �O�H���}�X�^�[[m]
	sSql	+= " m";
	sSql	+= " ON";
	sSql	+= " i.FF�r���o�H��=m.�O�H���R�[�h";
	sSql	+= " WHERE";
	sSql	+= " i.�Ǘ�No=";
	sSql	+= sTmp;
	sSql	+= " ORDER BY";
	sSql	+= " i.�H����� ASC, i.�rNo ASC";

	// SQL���s
	if( false == clsDB.ExecuteDirect(sSql) ){
		LOG(em_ERR), "[%s] SQL���s�G���[ ", my_sThreadName);
		LOG(em_NON), "%s", sSql);
		syslog(SYSNO_DB_EXECUTEDIRECT_ERR, "[%s]", sTbl);
		return false;
	}

	// �e�[�u���f�[�^�擾
	while( TRUE ){											// �L�����R�[�h��
		nRet	= clsDB.Fetch();							// ���R�[�h�Q��
		if( KIZUODBC_FETCH_OK == nRet ){					// ���R�[�h�L��
			// �O�H���e�e�r���
															// �Ǘ���
			clsDB.GetData( 1, typFFInf.cKizukenNo, SIZE_KIZUKEN_NO);
			clsDB.GetData( 2, &typFFInf.nKizuNo);			// �r��
			clsDB.GetData( 3, &typFFInf.nKouteiID);			// �H�����
			clsDB.GetData( 4, &typFFInf.nMenID);			// �\���敪[0:�\�A1:��]
			clsDB.GetData( 5, &typFFInf.nPreEdit);			// ���O�ҏW�t���O
															// �O�H���Ǘ���
			clsDB.GetData( 6, typFFInf.cMaeKizukenNo, SIZE_KIZUKEN_NO);
			clsDB.GetData( 7, &typFFInf.nMaeKizuNo);		// �O�H���r��
			clsDB.GetData( 8, typFFInf.cFF_Type, 4);		// �e�e�r��
			clsDB.GetData( 9, typFFInf.cFF_Grade, 4);		// �e�e�r�O���[�h
			clsDB.GetData(10, typFFInf.cFF_Koutei, 4);		// �e�e�r���o�H��
			clsDB.GetData(11, &typFFInf.nFF_Kubun);			// �e�e�r�敪
			clsDB.GetData(12, &typFFInf.nFF_LPos);			// �e�e�r���������ʒu[mm]
			clsDB.GetData(13, &typFFInf.nFF_WPos);			// �e�e�r�Е����ʒu[mm]
			clsDB.GetData(14, &typFFInf.nFF_Length);		// �e�e�r����[mm]
			clsDB.GetData(15, &typFFInf.nType);				// �����r��
			clsDB.GetData(16, &typFFInf.nGrade);			// �����r�O���[�h
			clsDB.GetData(17, &typFFInf.nLastPos);			// �ŏI����ʒu[mm]
			// �O�H�����C���ʃR�C�����
															// �Ǘ�No
			clsDB.GetData(18, typFFInf.cLwk01, SIZE_KIZUKEN_NO);
			clsDB.GetData(19, &typFFInf.nLwk02);			// �H�����
			clsDB.GetData(20, &typFFInf.nLwk03);			// �\���敪
			clsDB.GetData(21, &typFFInf.nStatus);			// �����X�e�[�^�X
			// �O�H���}�X�^�[
			clsDB.GetData(22, typFFInf.cMwk01, 4);			// �O�H���R�[�h
			clsDB.GetData(23, &typFFInf.nCheck);			// �L������
			clsDB.GetData(24, &typFFInf.nMaeDsp);			// �O�H���\�����[�h
			clsDB.GetData(25, &typFFInf.nImgFmt);			// �摜�`��

			if( 1 == typFFInf.nCheck ){						// �L���H��
				// �O�H���\���Ώ�
				if( 1 == typFFInf.nMaeDsp || 2 == typFFInf.nMaeDsp ){
					int nMode = 0;
					int nSetPos = GetMaeInfSetPos(&typFFInf, &nMode);
					nPnt	= RegFFInf(&typFFInf, nSetPos, nMode);			// �O�H���e�e���o�^[���L������]
					if( nPnt > 0 && 0 == nMode){							// �o�^����
						if( 1 == typFFInf.nMaeDsp ){		// �O�H���r���\��
							if( 0 == typFFInf.nStatus ){	// �O�H���r���[�擾�ς�]
								ReadMaeDefect(&typFFInf);	// �O�H���r���Ǎ�����
							}
						}
					}
				}
			}
		} else if( KIZUODBC_FETCH_NODATE == nRet ){			// ���R�[�h����
			break;
		} else {											// �Q�ƃG���[
			LOG(em_ERR), "[%s] FETCH ERR SQL", my_sThreadName);
			LOG(em_NON), "%s", sSql);
			syslog(SYSNO_DB_FETCH_ERR, "[%s]", sTbl);
			return false;
		}
	}

	LOG(em_INF), "�B�O�H���e�e���Ǎ� ����");

	return true;
}

int MaeManager::GetMaeInfSetPos(TYP_FF_INF* pFFInf, int* nMode )
{
	int	nMenID = pFFInf->nMenID;							// �\���敪
	int	nPnt = m_pMaeInf->rec[m_nRec].men[nMenID].num;		// �o�^�ʒu

	// 5CGL�� DB �O�H���e�e�r��� �̃L�[�l�͊Ǘ�No���rNo�݂̂ł���ׁA
	// �Ǘ�No���rNo���d�����鎖�͂Ȃ��̂ňȉ��̔���͎��{����Ȃ��B
	// ���p����1CAPL�ł� DB �O�H���e�e�r��� �̃L�[�l�ɁuSPM�����L���v�����݂��Ă����̂�
	// �Ǘ�No���rNo���d������\�����������B
	*nMode = 0;
	for (int ii = 0; ii < (int)m_pMaeInf->rec[m_nRec].men[nMenID].num; ii++) {
		if (pFFInf->nKizuNo == m_pMaeInf->rec[m_nRec].men[nMenID].maeinf[ii].nKizuNo) {
			nPnt = ii;
			*nMode = 1;
		}
	}
	return nPnt;
}
//------------------------------------------
// �O�H���}�b�v���o�^
//------------------------------------------
// �O�H���e�e�r�������L�������֓o�^����
//------------------------------------------
void MaeManager::RegMaeMap()
{
	LOG(em_INF), "�F�O�H���}�b�v���o�^ �J�n");

	TBL_HY_MAPROW*	pTblMapRow = mcls_pDataSet->GetMapRowTbl();
	for (int nMen = 0; nMen < NUM_MEN; nMen++) {
		int	nCnt = m_pMaeInf->rec[m_nRec].men[nMen].num;	// �r���o�^��
		for (int nNum = 0; nNum < nCnt; nNum++) {			// �r���o�^����

			int	nLen = m_pMaeInf->rec[m_nRec].men[nMen].maeinf[nNum].ff.nY / 1000;	// �ŏI����ʒu[mm]��[m]
	
			// �r�|�C���^��o�^�i�ŏ��A�ő���X�V�j
			for (int nStage = 0; nStage<MAP_STAGE_NUM; nStage++) {
				// �s��[0:�I���W��]
				int	nRow = mcls_pDataSet->GetStageLenToIndex(nStage, nLen);
				DSP_TYP_ROW& pWk = pTblMapRow->rec[m_nRec].stage[nStage].men[nMen].row[nRow].typ;
				if (0 == pWk.pntMae[0] || pWk.pntMae[0] > nNum + 1) pWk.pntMae[0] = nNum + 1;			// �ŏ��r���|�C���^(1�I���W��)
				if (0 == pWk.pntMae[1] || pWk.pntMae[1] < nNum + 1) pWk.pntMae[1] = nNum + 1;			// �ő��r���|�C���^(1�I���W��)
			}
			
		}
	}

	LOG(em_INF), "�F�O�H���}�b�v���o�^ ����");
}

//------------------------------------------
// �O�H�����Ǎ�
//------------------------------------------
// TYP_MAE_REQUEST*	pQue	�L���[���
//------------------------------------------
void MaeManager::ReadMaeInf(TYP_MAE_REQUEST* pQue)
{
	LOG(em_INF), "�@�O�H�����Ǎ� �J�n�@�Ǘ�No=[%s]", pQue->cKanriNo);

	// �i�[���R�[�h����
/***
	TBL_HY_POINT const*	pTblPoint	= mcls_pDataSet->GetPointTbl();
	m_nRec	= mcls_pDataSet->CheckKizukenNo(pQue->cKizukenNo, pTblPoint);
	if( m_nRec < 0 ){										// �Y�����R�[�h����
		LOG(em_ERR), "[%s] �Y�����R�[�h���� �Ǘ���=%s", my_sThreadName, pQue->cKizukenNo);
		return;
	}
***/
	//// TCP/IP�ƃ��[���X���b�g�̋t�]�����Ή�
	TBL_HY_POINT const*	pTblPoint	= mcls_pDataSet->GetPointTbl();
	int ii;
	const int cCNT = 3;
	for(ii=0; ii<cCNT; ii++) {
		m_nRec	= mcls_pDataSet->CheckKanriNo(pQue->cKanriNo, pTblPoint);
		if( m_nRec < 0 ){										// �Y�����R�[�h����
			LOG(em_ERR), "[%s] �Y�����R�[�h���� �Ǘ���=%s �̂��߁A������Ƒ҂�", my_sThreadName, pQue->cKanriNo);
			Sleep(500);
		} else {
			break;
		}
	}
	if( ii == cCNT ) {										// �Y�����R�[�h����
		LOG(em_ERR), "[%s] �Y�����R�[�h���� �Ǘ���=%s", my_sThreadName, pQue->cKanriNo);
		return;
	}

	// ���L�������i�[���R�[�h������������
	m_pMaeInf	= mcls_pDataSet->GetMaeInf();				// �O�H���}�b�v���|�C���^�擾
	// �O�H���r���
	memset(&m_pMaeInf->rec[m_nRec], 0x00, sizeof(m_pMaeInf->rec[m_nRec]));

	// �O�H���擾��
	if (1 == GetEnableData(pQue)) {			// �擾�ς�
											// �O�H���e�e���Ǎ�
		if (true == ReadFFInf(pQue)) {		// �Ǎ�����
		}
	}
// 2023.11.17 �ŏI�H���������u���@�\�����̂S -------------------->>>>>
	// �O�H���r���̎擾���������C���ɒʒm
	COMMON_QUE que;
	que.nEventNo = FACT_HY_DEFECT_04;
	que.nLineNo = getlineid();
	que.fl.data[0] = m_nRec;
	send_mail(getmytaskname(), ".", &que);
// 2023.11.17 �ŏI�H���������u���@�\�����̂S <<<<<--------------------
	
	RegMaeMap();									// �O�H���}�b�v���o�^

	LOG(em_INF), "�@�O�H�����Ǎ� ����");
}
