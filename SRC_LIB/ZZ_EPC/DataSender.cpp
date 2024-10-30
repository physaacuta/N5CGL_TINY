#include "StdAfx.h"
#include "DataSender.h"

//#include <fstream>	
#include <random>
#define RANDOM(x)	(!m_bRand ? 0 : rand() % x)

DataSender::DataSender(void) :
m_nFrameNo(0),
m_nSmemBase(FIRST_SHMEM_SIZE),
mcls_pLog(NULL),
m_bSHMemSet(false)
{
	for(int ii=0; ii<5; ii++){
		g_bPutKizu[ii]  = true;
		g_nPutKizu[ii]  = 0;
		g_nPutFrm[ii]   = 1;
		g_nPutType[ii]  = 1;
		g_nPutGrade[ii] = 1;
		g_nPutCycle[ii] = 0;
	}
	memset(m_camset, 0x00, sizeof(m_camset));
	memset(&m_EdgeRestrict, 0x00, sizeof(m_EdgeRestrict));
	m_camsetnum = 1;

	m_hPipe = CreateNamedPipe(	EPC_PIPE_DATA, 
								PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
								PIPE_WAIT | PIPE_READMODE_MESSAGE | PIPE_TYPE_MESSAGE,
								1,	256, 256, 0, NULL);
	ASSERT(INVALID_HANDLE_VALUE != m_hPipe);

	// 送信用のOVERLAPPED
	memset(&m_ovlSend, 0, sizeof(OVERLAPPED));
	m_ovlSend.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL); // manual, reset
	ASSERT(NULL != m_ovlSend.hEvent);


	// 共有メモリ生成 (CreateFileMappingは 未生成時は、新規作成ハンドル。 生成済みの場合は、そのハンドルが返る)
	m_hFrame = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, MAX_MEMSIZE, EPC_SMEM_DATA);
	ASSERT(NULL != m_hFrame);
	gtbl_pFrame = (BYTE *)MapViewOfFile(m_hFrame, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	memset(gtbl_pFrame, 0x00, MAX_MEMSIZE);


	// 初期化
	InitFrameNo();
	InitPosCmd();

	for(int ii=0; ii<4; ii++){
		for(int jj=0; jj<3; jj++){
			m_nBrightness[ii][jj] = 128;
		}
	}
	for(int ii=0; ii<3; ii++){
		m_dCamGain[ii] = 1.00;
	}
	g_bPutEdge = false;
	g_bPutSkip = false;
	for (int ii = 0; ii < 2;ii++) m_dEdge[ii] = 0;

	mcls_Time.InitAve();


	//--------------------------
	// 画像取得
    memset( m_imgTemp, 0x00, sizeof(m_imgTemp) );
    memset( m_frmTemp, 0x00, sizeof(m_frmTemp) );
	int nWk = 0;
    for ( int ii = 0; ii<FILE_IMG_NUM; ii++ )
    {
		CString	cPath;
		cPath.Format(FILE_FORMAT_IMG, ii);

	    std::ifstream   is;
	    is.open(cPath, std::ios::binary);
        if ( is.is_open() == false )
        {   
            LOG(em_ERR), "[DataSender] 切り出しFile Open失敗 <%s>", cPath );
        }
	    
        // 8bit
        is.seekg(1078);
        is.read((char*)&m_imgTemp[ nWk ], IMG_SIZE_X*IMG_SIZE_Y*MAX_IMAGE_CNT);
	    is.close();
        nWk += IMG_SIZE_X*IMG_SIZE_Y*MAX_IMAGE_CNT;
	}

	nWk = 0;
    for ( int ii = 0; ii < FILE_FRM_NUM; ii++ )
    {
		CString	cPath;
		cPath.Format(FILE_FORMAT_FRM, ii);

		std::ifstream   is;
		is.open(cPath, std::ios::binary);
        if ( is.is_open() == false )
        {   
            LOG(em_ERR), "[DataSender] フレームFile Open失敗 <%s>", cPath );
        }
		    
        // 8bit
        is.seekg(1078);
        is.read((char*)&m_frmTemp[ nWk ], SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y);
		is.close();
        nWk += SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y;
	}

	m_shrinkratio = 1.0;
}

DataSender::~DataSender(void)
{
	if (m_hPipe) CloseHandle(m_hPipe);
	if (gtbl_pFrame) UnmapViewOfFile(gtbl_pFrame);
	if (m_hFrame) CloseHandle(m_hFrame);
	CloseHandle(m_ovlSend.hEvent);
}


//------------------------------------------
// スレッド
//------------------------------------------
UINT WINAPI DataSender::MainThread(LPVOID param)
{
	////// メインインスタンス取り出し
	DataSender *p = (DataSender *)param;					// 自クラスのインスタンスを取得
	p->SetTid();								// 自スレッドIDセット

	OVERLAPPED ovlRecv;			// 非同期IO
	OVERLAPPED ovlConnect;
	memset(&ovlRecv, 0, sizeof(OVERLAPPED));
	memset(&ovlConnect, 0, sizeof(OVERLAPPED));
	ovlRecv.hEvent		= CreateEvent(NULL, TRUE, FALSE, NULL); // manual, reset
	ovlConnect.hEvent	= CreateEvent(NULL, FALSE, FALSE, NULL); // auto, reset

	DWORD readdata;
	DWORD dwTransByte, dwRetCode;
	BOOL bRet;

	// WaitForMultipleObjectsイベント
	HANDLE	hArray[] = {p->my_evStop,
						ovlConnect.hEvent,
						ovlRecv.hEvent
						};
	enum {	EV_STOP = WAIT_OBJECT_0,		// スレッド終了
			EV_OPEN,						// オープン
			EV_RECV_END,					// 受信完了通知
			EV_END	};


	// 最初の接続
	if ( ! ConnectNamedPipe(p->m_hPipe, &ovlConnect)) {
		dwRetCode = GetLastError();
		ASSERT(ERROR_IO_PENDING == dwRetCode || ERROR_PIPE_CONNECTED == dwRetCode);
		if (ERROR_PIPE_CONNECTED == dwRetCode) {

			if ( ! ReadFile(p->m_hPipe, &readdata, sizeof(readdata), NULL, &ovlRecv)) {
				dwRetCode = GetLastError();
				ASSERT(ERROR_IO_PENDING == dwRetCode);
			}
		}
	}

	pLOG(em_MSG), "[DataSender] スレッド開始 [0x%X]", p->GetTid());
	while (1) {
		DWORD ret = WaitForMultipleObjects(EV_END, hArray, FALSE, INFINITE);
		switch (ret) {

//-----------------------------------------------------------------------------------------------
		case EV_STOP:												// スレッド終了
			pLOG(em_MSG), "[DataSender] スレッド終了 [0x%X]", p->GetTid());
			DisconnectNamedPipe(p->m_hPipe);
			CloseHandle(ovlRecv.hEvent);
			CloseHandle(ovlConnect.hEvent);
			return 0;

//-----------------------------------------------------------------------------------------------
		case EV_OPEN:												// オープン
			pLOG(em_MSG), "[DataSender] データPipe オープン完了");
			if (!ReadFile(p->m_hPipe, &readdata, sizeof(readdata), NULL, &ovlRecv)) {
				dwRetCode = GetLastError();
				ASSERT(ERROR_IO_PENDING == dwRetCode);
			}
			break;

//-----------------------------------------------------------------------------------------------
		case EV_RECV_END:											// 受信完了通知
			bRet = GetOverlappedResult(p->m_hPipe, &ovlRecv, &dwTransByte, TRUE);
			ResetEvent(ovlRecv.hEvent);
			if (bRet) {
				//////pLOG(em_MSG), "[DataSender] オフセット返却 [%d]", readdata);
				// 次のリード発行
				ReadFile(p->m_hPipe, &readdata, sizeof(readdata), NULL, &ovlRecv);
			} else {
				pLOG(em_ERR), "[DataSender] データPipe 切断");
			}
			break;


//-----------------------------------------------------------------------------------------------
		default :
			// ありえない！！
			break;
		}
	}
	return 0;
}

//------------------------------------------
// コマンド送信
//------------------------------------------
int DataSender::SendData(LPVOID senddata, DWORD sendsize)
{
	WriteFile(m_hPipe, senddata, sendsize, NULL, &m_ovlSend);
	return 0;
}


//------------------------------------------
// ダミーフレーム情報セット
//------------------------------------------
int DataSender::SetFrameData(DEBUG_SPEED_INFO* readInf)
{
	int nWk = 0;
	int nFrmIdx = 0;		// フレームセット配列位置
	int nImgIdx = 0;

	int		nPutKizu[5];
	int		nType[5];
	int		nGrade[5];
	int		nCycle[5];
	int		nTotalCnt = 0;
	int		nThresHold[5];
	CString sDefectMsg = "";
	CString sWk = "";
	//int nPutKizu		= g_bPutKizu ? g_nPutKizu : 0;
	int ou				= (m_camset[0]-1) / (MAX_CAMSET/NUM_MEN);
	float dLen			= SIZE_FRAME_IMG_Y * readInf->vRes;

	// フレームNoチェック
	if( m_nFrameNo != readInf->framenum ) {
		LOG(em_ERR), "[DataSender] <%s> フレームNo <%05d != %05d>", ou==0?"表":"裏", m_nFrameNo, readInf->framenum);
		m_nFrameNo = readInf->framenum; 
	}


	for(int ii=0; ii<5; ii++){
		nPutKizu[ii] = g_bPutKizu[ii] ? g_nPutKizu[ii] : 0;
		nType[ii]    = g_nPutType[ii];
		nGrade[ii]   = g_nPutGrade[ii];
		nCycle[ii]   = g_nPutCycle[ii];

		if(0 < g_nPutFrm[ii]){
			if(0 != (m_nFrameNo % g_nPutFrm[ii]))	nPutKizu[ii] = 0;	// 送信フレーム以外
		}

		nTotalCnt += nPutKizu[ii];
		sWk.Format(", (Type:%d, Gr:%d)x%d個", nType[ii], nGrade[ii], nPutKizu[ii]);
		
		sDefectMsg += sWk;

		for(int jj=0; jj<=ii; jj++)	nThresHold[ii] += nPutKizu[jj];
	}

	//--------------------------
	// 距離チェック
	DWORD dwMask = 0;

	int idx = -1;
	for(int ii=0; ii<MAX_NEXT_POS_CMD; ii++) {
		if( 0 != m_NextPosCmd[ii].cmd && 
			(0 == m_NextPosCmd[ii].framenum || m_nFrameNo == m_NextPosCmd[ii].framenum)) {
		
			LOG(em_MSG), "[DataSender] <%s> フレームNo <%05d> 距離反映=%04x", ou==0?"表":"裏", m_nFrameNo, m_NextPosCmd[ii].update_mask);

			// 距離反映
			if( (m_NextPosCmd[ii].update_mask & FRONT) == FRONT )		m_NowPos.front_pos		= m_NextPosCmd[ii].front_pos;
			if( (m_NextPosCmd[ii].update_mask & TAIL) == TAIL )			m_NowPos.tail_pos		= m_NextPosCmd[ii].tail_pos;
			if( (m_NextPosCmd[ii].update_mask & SHEAR) == SHEAR )		m_NowPos.shear_cut_pos	= m_NextPosCmd[ii].shear_cut_pos;
			if( (m_NextPosCmd[ii].update_mask & UNIV_UP0) == UNIV_UP0 )	m_NowPos.univ_cnt_up_pos[0]	= m_NextPosCmd[ii].univ_cnt_up_pos[0];
			if( (m_NextPosCmd[ii].update_mask & UNIV_UP1) == UNIV_UP1 )	m_NowPos.univ_cnt_up_pos[1]	= m_NextPosCmd[ii].univ_cnt_up_pos[1];
			if( (m_NextPosCmd[ii].update_mask & UNIV_UP2) == UNIV_UP2 )	m_NowPos.univ_cnt_up_pos[2]	= m_NextPosCmd[ii].univ_cnt_up_pos[2];
			if( (m_NextPosCmd[ii].update_mask & UNIV_UP3) == UNIV_UP3 )	m_NowPos.univ_cnt_up_pos[3]	= m_NextPosCmd[ii].univ_cnt_up_pos[3];
			if( (m_NextPosCmd[ii].update_mask & UNIV_DN0) == UNIV_DN0 )	m_NowPos.univ_cnt_dn_pos[0]	= m_NextPosCmd[ii].univ_cnt_dn_pos[0];
			if( (m_NextPosCmd[ii].update_mask & UNIV_DN1) == UNIV_DN1 )	m_NowPos.univ_cnt_dn_pos[1]	= m_NextPosCmd[ii].univ_cnt_dn_pos[1];
			if( (m_NextPosCmd[ii].update_mask & UNIV_DN2) == UNIV_DN2 )	m_NowPos.univ_cnt_dn_pos[2]	= m_NextPosCmd[ii].univ_cnt_dn_pos[2];
			if( (m_NextPosCmd[ii].update_mask & UNIV_DN3) == UNIV_DN3 )	m_NowPos.univ_cnt_dn_pos[3]	= m_NextPosCmd[ii].univ_cnt_dn_pos[3];

			dwMask = m_NextPosCmd[ii].update_mask;

			m_NextPosCmd[ii].cmd = (NCL_CMDCODE)0;		// 反映終わり
		}
	}








	//// 今回の書き込み位置
	int	base			= m_nSmemBase;
	if(base > MAX_MEMSIZE-(ONE_MEMSIZE*m_camsetnum)) base = FIRST_SHMEM_SIZE;

	//// データセット
	for(int ii=0; ii<m_camsetnum; ii++) {

		//--------------------------
		// ヘッダー部生成
		NCL_SMEM_FRAME_HEADER&	nh = (NCL_SMEM_FRAME_HEADER&)gtbl_pFrame[base];
		memset(&nh, 0x00, sizeof(NCL_SMEM_FRAME_HEADER));

		nh.frame_num	= m_nFrameNo;				// 0始まり	// 送信フレームシリアル番号（探傷開始より）
		nh.camid		= m_camset[ii];				// カメラセット
		nh.status		= STATUS_VALID;
		if(g_bPutSkip) nh.status = STATUS_TOOMANY;	// 探傷状態セット（未検査区間）
		//nh.defect_num	= nPutKizu;					// このフレームの疵個数（＝結果配列と特徴量配列の数）
		//nh.image_num	= nPutKizu;					// このフレームの中の画像の数（＝画像配列の数）
		nh.defect_num	= nTotalCnt;				// このフレームの疵個数（＝結果配列と特徴量配列の数）
		nh.image_num	= nTotalCnt;				// このフレームの中の画像の数（＝画像配列の数）

		nh.executed_position_mask = dwMask;			// MASK値を折り返しセット


		nh.frame_brightness[0] = 50 +  (m_nFrameNo % 20);
		nh.frame_brightness[1] = 120 + (m_nFrameNo % 20);
		nh.frame_brightness[2] = 130 + (m_nFrameNo % 20);


		//--------------------------
		// オフセット
		nh.edge_offset	= sizeof(NCL_SMEM_FRAME_HEADER);
		nh.frame_offset		= GET_JUST_SIZE(nh.edge_offset + sizeof(float)*SIZE_FRAME_IMG_Y * 2, 16);
		nh.result_offset	= nh.frame_offset + SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*4;
		//nh.attr_offset		= nh.result_offset + sizeof(NCL_DEFECT_RESULT)*nPutKizu;
		//nh.image_offset		= nh.attr_offset + sizeof(double)*(NCL_nattr_common+NCL_nattr_angle*4) * nPutKizu;
		nh.attr_offset		= nh.result_offset + sizeof(NCL_DEFECT_RESULT)*nTotalCnt;
		nh.image_offset		= nh.attr_offset + sizeof(double)*(NCL_nattr_common+NCL_nattr_angle*4) * nTotalCnt;


		//--------------------------
		// 現カウンタ値
		nh.front_pos		= m_NowPos.front_pos;
		nh.tail_pos			= m_NowPos.tail_pos;
		nh.shear_cut_pos	= m_NowPos.shear_cut_pos;
		for(int ii=0; ii<4; ii++) {
			nh.univ_cnt_up_pos[ii]	= m_NowPos.univ_cnt_up_pos[ii];
			nh.univ_cnt_dn_pos[ii]	= m_NowPos.univ_cnt_dn_pos[ii];
		}


		//--------------------------
		// エッジ	幅方向連結後の座標
		float* pEdge = (float*) &gtbl_pFrame[ base + nh.edge_offset ];
		for(int yy=0; yy<SIZE_FRAME_IMG_Y; yy++) {
			if(g_bPutEdge) {
				pEdge[yy + 0] = (float)(m_dEdge[0] * 1.0);	// 左 
				pEdge[yy + SIZE_FRAME_IMG_Y] = (float)(m_dEdge[1] * 1.0);		// 右
			} else {
				pEdge[ yy + 0 ]				   = (float)(-600.0 + (yy*0.01) + (float)(m_nFrameNo % 20));	// 左 
				pEdge[ yy + SIZE_FRAME_IMG_Y ] = (float)(+500.0 + (yy*0.01) + (float)(m_nFrameNo % 20));	// 右
			}
			//LOG(em_MSG), "[DataSender] <%s> Frm:%d Edge[%.1f %.1f]", ou==0?"表":"裏", m_nFrameNo, pEdge[ 2*yy + 0 ], pEdge[ 2*yy + 1 ]);
		}


		//--------------------------
		// フレームRAW画像
        //BYTE* pFrm = (BYTE*) &gtbl_pFrame[ base + nh.frame_offset ];
		NCL_FRAME_IMAGE* pFrm = (NCL_FRAME_IMAGE*)&gtbl_pFrame[base + nh.frame_offset];
		nWk = 0;
		// フレーム画像ヘッダ部
		pFrm->width = SIZE_FRAME_IMG_X / FRM_SHRINK_X;
		pFrm->height = SIZE_FRAME_IMG_Y / FRM_SHRINK_Y;
		pFrm->stride = pFrm->width;
		pFrm->channel = MAX_IMAGE_CNT;
		for (int jj = 0; jj<(int)pFrm->channel; jj++) {
			//memcpy( &pFrm[nWk], &m_frmTemp[ nFrmIdx*SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y ], SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y );
			memcpy(&pFrm->image[nWk], &m_frmTemp[nFrmIdx*pFrm->stride*pFrm->height], pFrm->stride*pFrm->height);

			nWk += pFrm->stride*pFrm->height;
			nFrmIdx ++;
			if(FILE_FRM_NUM <= nFrmIdx) nFrmIdx = 0;
		}



		//--------------------------
		// 結果
		//for(int jj=0; jj<nPutKizu; jj++) {
		for(int jj=0; jj<nTotalCnt; jj++) {

			srand((unsigned int)time(NULL));
			NCL_DEFECT_RESULT&		nr = (NCL_DEFECT_RESULT&)gtbl_pFrame[base + nh.result_offset	+ sizeof(NCL_DEFECT_RESULT)*jj];
			double*					na = (double*)&gtbl_pFrame[base + nh.attr_offset				+ sizeof(double)*(NCL_nattr_common+NCL_nattr_angle*MAX_CAMANGLE)*jj];
			NCL_IMAGE_HEADER&		ni = (NCL_IMAGE_HEADER&)gtbl_pFrame[base + nh.image_offset		+ (sizeof(DWORD)*4 + IMG_SIZE_X*IMG_SIZE_Y*MAX_CAMANGLE)*jj];


			//// 結果配列
			nr.group = 0;
			nr.flags = NCL_DEFECT_FLAGS::NORMAL;
			nr.im_idx = jj;			// この疵は画像配列の何番目の画像に含まれるか（０～）。

			//// 疵種Gr
			//nr.type  = 1;
			//nr.grade = 1;

			//if(0==jj && 0==m_nFrameNo % 50) {
			//	nr.type  = 2;
			//	nr.grade = 3;
			//}

			//if(1<=jj) {
			//	nr.type  = 2;
			//	nr.grade = 2;
			//}
			// この疵の疵種・疵Grを決定
			int nThres =0;
			for(int kk=0; kk<5; kk++){
				if(0==nPutKizu[kk]) continue;

				nThres+=nPutKizu[kk];
				if(jj<nThres){
					nr.type = nType[kk];
					nr.grade= nGrade[kk];
					nr.flags= nCycle[kk];
					break;
				}
			}
			



			// 基本特徴量
			srand((unsigned)time( NULL ));
			memset(na, 0x00, sizeof(double)*(NCL_nattr_common+NCL_nattr_angle*MAX_CAMANGLE));

			na[DEFECT_TYPE]		= nr.type;
			na[DEFECT_GRADE]	= nr.grade;
			na[DEFECT_FLAG]		= nr.flags;
			na[FRAME_NUM]		= nh.frame_num;
			na[AREA]			= rand();
			//na[MERGED_CENTER_Y] = m_nFrameNo*(SIZE_FRAME_IMG_Y*0.25) + 128*jj;
			na[MERGED_CENTER_Y] = m_NowPos.front_pos + (int)rand()/100.0 + ii;
//デバッグ
//if (2 == nr.type) na[MERGED_CENTER_Y] = na[MERGED_CENTER_Y] - (MAP_REPRES_MIN * 1000);
//if (3 == nr.type) na[MERGED_CENTER_Y] = na[MERGED_CENTER_Y] + (MAP_REPRES_MIN * 1000);
//if (4 == nr.type) na[MERGED_CENTER_Y] = na[MERGED_CENTER_Y] + (MAP_REPRES_MIN * 1000*2);
			na[FRAME_POS]   = m_NowPos.front_pos + RANDOM(1000);
			na[POS_FROM_TAIL]   = m_NowPos.tail_pos + RANDOM(1000);
			na[POS_FROM_SHEAR_CUT] = m_NowPos.shear_cut_pos;
			na[FERET_X]   = 3;
			na[FERET_Y]   = 5;
			na[LINE_SPEED] = (5000 * 60.0f / 1000.0f)+0.5f;

			int dsws = (((int)m_camset[ii]-1) % NUM_CAM_POS) / (NUM_CAM_POS / 2);
			int cam = ((int)m_camset[ii]-1) % (NUM_CAM_POS/2);
			int x = ((cam+1) *150) + (m_nFrameNo % 200) + jj*10 + RANDOM(1000);
			//int x = ((cam+1) *10) + (m_nFrameNo % 200) + jj*10;

			if (0 != g_nPosX)	x = g_nPosX;

			if( 0 == dsws ) {
				na[MERGED_CENTER_X]		 = x * -1;
				na[CENTER_X_FROM_L_EDGE] = x;
				na[CENTER_X_FROM_R_EDGE] = 1750 - na[CENTER_X_FROM_L_EDGE];
			} else {
				na[MERGED_CENTER_X]		 = x;
				na[CENTER_X_FROM_R_EDGE] = x;
				na[CENTER_X_FROM_L_EDGE] = 1750 - na[CENTER_X_FROM_R_EDGE];
			}
			int nX = 1750 / 4;
			na[ABS_X_POS] = (double)m_camset[ii] * 100.0 - (double)nX;

			na[CAMSET]			= m_camset[ii];
			na[HORZ_RES]		= 0.048;
			na[VERT_RES]		= readInf->vRes;
			
			//na[FERET_X_PIXEL]	= ((m_nFrameNo % 5) + 1)*1000;
			//na[FERET_Y_PIXEL]	= (int)rand()/100.0;
			na[FERET_X_PIXEL]	= IMG_SIZE_X / 2;
			na[FERET_Y_PIXEL]	= IMG_SIZE_Y / 6;

			LOG(em_MSG), "[DataSender] <%s> x=%d, cam=%d, m_nFrameNo=%d, jj=%d, m_camset[%d]=%d, dsws=%d, cam=%d, m_bRand=%d, absX=%.0f hRes=%.4f vRes=%.4f",
				ou == 0 ? "表" : "裏", x, cam, m_nFrameNo, jj, ii, m_camset[ii], dsws, cam, m_bRand, na[ABS_X_POS], na[HORZ_RES], na[VERT_RES]);

			nWk					= (jj*128>=1800? 1800 : jj*128);
			// 代表疵確認用
			//na[IMAGE_X]			= 0;
			//na[IMAGE_Y]			= 0;
			na[IMAGE_X]			= nWk*na[HORZ_RES];
			na[IMAGE_Y]			= nWk*na[VERT_RES];

			//na[BOX_X_MIN]		= nWk + 32 + (m_nFrameNo % 3)*10;
			//na[BOX_Y_MIN]		= nWk +128 + (m_nFrameNo % 3)*20;	
			na[BOX_X_MIN]		= IMG_SIZE_X / 3;
			na[BOX_Y_MIN]		= IMG_SIZE_Y / 5 * 2;
			na[BOX_X_MAX]		= na[BOX_X_MIN] + na[FERET_X_PIXEL];
			na[BOX_Y_MAX]		= na[BOX_Y_MIN] + na[FERET_Y_PIXEL];
			// 代表疵確認用
			//na[BOX_X_MIN]		= 0;
			//na[BOX_Y_MIN]		= 0;	
			//na[BOX_X_MAX]		= SIZE_FRAME_IMG_X;
			//na[BOX_Y_MAX]		= SIZE_FRAME_IMG_Y;

			na[CENTER_X]		= (na[BOX_X_MIN] + na[FERET_X_PIXEL]/2)*na[HORZ_RES];
			na[CENTER_Y]		= (na[BOX_Y_MIN] + na[FERET_Y_PIXEL]/2)*na[VERT_RES];




			//// 切り出し画像
			ni.left   = 0;
			ni.top	  = 0;
			ni.height = IMG_SIZE_Y;
			ni.width  = IMG_SIZE_X;
			ni.shrinkratio = m_shrinkratio;
			for(int kk=0; kk<MAX_IMAGE_CNT; kk++ ) {
				
				memcpy(&ni.image[IMG_SIZE_Y*IMG_SIZE_X*kk], 
						&m_imgTemp[ nImgIdx* IMG_SIZE_Y*IMG_SIZE_X*kk], 
						IMG_SIZE_Y*IMG_SIZE_X);

				nImgIdx ++;
				if(FILE_IMG_NUM <= nImgIdx) nImgIdx = 0;
			}
		}



		//// パイプデータ
		NCL_SMEM_PIPE_DATA 	pd;
		memset(&pd, 0x00, sizeof(pd));

		int nSize =   sizeof(NCL_SMEM_FRAME_HEADER) 
					+ sizeof(float)*2*SIZE_FRAME_IMG_Y
					+ SIZE_FRAME_IMG_X*SIZE_FRAME_IMG_Y*MAX_CAMANGLE
					+ sizeof(NCL_DEFECT_RESULT)*nTotalCnt
					+ sizeof(double)*(NCL_nattr_common+NCL_nattr_angle*MAX_CAMANGLE)*nTotalCnt
					+ (sizeof(DWORD)*4 + IMG_SIZE_X*IMG_SIZE_Y*MAX_CAMANGLE)*nTotalCnt;
		nSize = GET_JUST_SIZE(nSize, 16);


		pd.frame_num = nh.frame_num;		// 0始まり
		pd.camid = nh.camid;
		pd.smem_offset = base;
		pd.smem_size = nSize;


		//mcls_Time.PutTimestamp(0);
		//__int64 nWk = mcls_Time.GetRawTimestamp(0);
		//memcpy(&pd.timestamp_start, (void*)&nWk, sizeof(__int64));		// エンジン送信時のタイムスタンプ
		//nWk += 10;
		//memcpy(&pd.timestamp_end, (void*)&nWk, sizeof(__int64));		// エンジン送信時のタイムスタンプ

		//// 輝度
		//for(int cam=0; cam<4; cam++){
		//	pd.br[cam].br_center = m_nBrightness[cam][0];
		//	pd.br[cam].br_r_ovl  = m_nBrightness[cam][1];
		//	pd.br[cam].br_l_ovl  = m_nBrightness[cam][2];
		//}

		//// カメラゲイン
		//for(int cam=0; cam<3; cam++){
		//	pd.camgain[cam] = m_dCamGain[cam];
		//}


		// パイプで通知
		SendData(&pd, sizeof(pd));	

		// ベース位置変更
		base += nSize;

	}

	// エッジ範囲指定
	// ※ログが毎フレーム出力されるため、削除
	//////LOG(em_MSG), "[DataSender] <%s> フレームNo <%05d>(%05d)セット <%s> [L:%.1f～%.1fmm] [R:%.1f～%.1fmm]",
	//////	ou==0?"表":"裏", m_nFrameNo, m_EdgeRestrict.frame_num, (TRUE==m_EdgeRestrict.limit[ou].use_limit ? "範囲指定":"判定エッジ"), 
	//////	m_EdgeRestrict.limit[ou].left_outer, m_EdgeRestrict.limit[ou].left_inner,
	//////	m_EdgeRestrict.limit[ou].right_inner, m_EdgeRestrict.limit[ou].right_outer);

	//LOG(em_MSG), "[DataSender] <%s> フレームNo <%05d>セット 統括からのデータ %dmm 疵個数=%d", ou==0?"表":"裏", m_nFrameNo, (int)m_NowPos.front_pos, nPutKizu);
	LOG(em_MSG), "[DataSender] <%s> フレームNo <%05d>セット 統括からのデータ %dmm 疵個数=%d 縮小率=%.0f", ou==0?"表":"裏", m_nFrameNo, (int)m_NowPos.front_pos, nTotalCnt, m_shrinkratio, sDefectMsg);


	//--------------------------
	// 次
	m_nFrameNo ++;
	m_nSmemBase = base;


	//--------------------------
	// 距離加減算
	m_NowPos.front_pos += dLen;
	m_NowPos.tail_pos -= dLen;
	m_NowPos.shear_cut_pos += dLen;
	for(int ii=0; ii<4; ii++) {
		m_NowPos.univ_cnt_up_pos[ii] += dLen;
		m_NowPos.univ_cnt_dn_pos[ii] -= dLen;
	}

	return 0;
}

//------------------------------------------
// 次アイテム
//------------------------------------------
void DataSender::SetNextPosCmd(NCL_CMD_SET_POSITION* p)
{
	int idx = -1;
	
	// 空いているエリアを調査
	for(int ii=0; ii<MAX_NEXT_POS_CMD; ii++) {
		if( 0 == m_NextPosCmd[ii].cmd ) {
			idx = ii;
			break;
		}
	}
	if(-1== idx) {
		LOG(em_ERR), "[DataSender] 次アイテム距離情報 空き無し frame=%d", p->framenum);
		return;
	}

	// セット
	memcpy( &m_NextPosCmd[idx], p, sizeof(NCL_CMD_SET_POSITION));
}

//------------------------------------------
// 共有メモリに書き込み（先頭）
//------------------------------------------
void DataSender::SetFirstSHMem(bool bSHMemSet, FIRST_SHMEM_INFOv1* p)
{
	m_bSHMemSet = bSHMemSet;
	SetFirstSHMem(p);
}
void DataSender::SetFirstSHMem(FIRST_SHMEM_INFOv1* p)
{
	LOG(em_MSG), "[DataSender] カメラ情報を共有メモリ%s", (m_bSHMemSet ? "にセット" : "からクリア"));
	FIRST_SHMEM_INFOv1 *pSmemH = (FIRST_SHMEM_INFOv1 *)gtbl_pFrame;
	memcpy(pSmemH, p, sizeof(FIRST_SHMEM_INFOv1));
}
