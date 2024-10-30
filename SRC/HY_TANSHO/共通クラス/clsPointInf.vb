'*******************************************************************************
'  ポインタ情報クラス
'
' [Ver]
'      Ver.01  2013/12/20  初版
'
' [メモ]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

''' <summary>
''' ポインタ情報クラス
''' </summary>
''' <remarks></remarks>
Public Class clsPointInf
    Implements IDisposable
    '///////////////////////////////////////////////////////////////////////////
    '列挙体
    '///////////////////////////////////////////////////////////////////////////
    '参照レコード位置
    Public Enum EM_DIV_RECORD
        DIV_RECORD_WRITE = 1                                '書込みレコード
        DIV_RECORD_SOUCHI                                   '各検査装置単位での現在レコード
        DIV_RECORD_BASE                                     '基準点位置での現在表示レコード
        DIV_RECORD_KENSA                                    '各検査台位置での現在表示レコード
    End Enum

    '///////////////////////////////////////////////////////////////////////////
    'メンバ変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                                    'リソース開放フラグ[True:開放済,False:未開放]

    Private mcls_Log As tClass.tClass_LogManager                                'ログ管理

    Private m_typHyPoint As TBL_HY_POINT                                        'ポインタ情報
	Private mcls_DispCoil As clsDispCoil										'コイル情報表示

    '///////////////////////////////////////////////////////////////////////////
    'プロパティ
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 書込みレコード
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecWriteBase() As Integer
        Get
            Return m_typHyPoint.nRecWriteBase
        End Get
    End Property

    ''' <summary>
    ''' 各検査装置単位での現在レコード
    ''' </summary>
    ''' <param name="nMen">面[0:表,1:裏]</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecWrite(ByVal nMen As Integer) As Integer
        Get
            Dim nRecWrite As Integer = -1

            If nMen >= 0 And nMen < NUM_MEN Then
                nRecWrite = m_typHyPoint.nRecWrite(nMen)
            End If

            Return nRecWrite
        End Get
    End Property

    ''' <summary>
    ''' 表示基点位置での現在表示レコード
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecDispBase() As Integer
        Get
            Return m_typHyPoint.nRecDispBase
        End Get
    End Property

    ''' <summary>
    ''' 各検査台位置での現在表示レコード
    ''' </summary>
    ''' <param name="nKensa">検査台位置</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecDisp(ByVal nKensa As Integer) As Integer
        Get
            Dim nRecDisp As Integer = -1

            If nKensa >= 0 And nKensa < MAX_DISP_POS Then
                nRecDisp = m_typHyPoint.nRecDisp(nKensa)
            End If

            Return nRecDisp
        End Get
    End Property

    ''' <summary>
    ''' 各検査台位置での現在表示コイルレコード
    ''' </summary>
    ''' <param name="nKensa">検査台位置</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecCoilDisp(ByVal nKensa As Integer) As Integer
        Get
            Dim nRecDisp As Integer = -1

            'If nKensa >= 0 And nKensa < MAX_DISP_POS Then
            '    nRecDisp = m_typHyPoint.nRecCoilDisp(nKensa)
            'End If

            Return nRecDisp
        End Get
    End Property

    '''' <summary>
    '''' 溶接点残長
    '''' </summary>
    '''' <param name="nKensa">検査台位置</param>
    '''' <value></value>
    '''' <returns></returns>
    '''' <remarks></remarks>
    'Public ReadOnly Property LenZan(ByVal nKensa As Integer) As Integer
    '    Get
    '        Dim nLenZan As Integer = -1

    '        If nKensa >= 0 And nKensa < MAX_DISP_POS Then
    '            nLenZan = m_typHyPoint.inf(nKensa).nLenZan
    '        End If

    '        Return nLenZan
    '    End Get
    'End Property

    ''' <summary>
    ''' 管理No
    ''' </summary>
    ''' <param name="nRec">レコード</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KizukenNo(ByVal nRec As Integer) As String
        Get
            Dim strKizukenNo As String = Nothing

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                strKizukenNo = tMod_Common.ByteToString(m_typHyPoint.pnt(nRec - 1).cKizukenNo)
            End If

            Return strKizukenNo
        End Get
    End Property

	''' <summary>
	''' 疵権管理番号で指定されたレコード番号取得
	''' </summary>
	''' <param name="sKizukenNo"></param>
	''' <value></value>
	''' <returns>存在しない:0 該当レコード番号:1～TNL_HY_REC_MAX </returns>
	''' <remarks></remarks>
	Public ReadOnly Property recNo(ByVal sKizukenNo As String) As Integer
		Get
			Dim nRec As Integer = 0
			For ii As Integer = 1 To TBL_HY_REC_MAX
				If sKizukenNo = tMod.ByteToString(m_typHyPoint.pnt(ii - 1).cKizukenNo) Then
					nRec = ii
				End If
			Next
			Return nRec
		End Get
	End Property

    ''' <summary>
    ''' 画面表示状態
    ''' </summary>
    ''' <param name="nRec">レコード</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DispStatus(ByVal nRec As Integer) As EM_DIV_DSP_STATUS
        Get
            Dim emDispStatus As EM_DIV_DSP_STATUS = Nothing

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                emDispStatus = m_typHyPoint.pnt(nRec - 1).emDispStatus
            End If

            Return emDispStatus
        End Get
    End Property


    '''' <summary>
    '''' 各通板ポイントの通板距離
    '''' </summary>
    '''' <param name="nRec">レコード</param>
    '''' <param name="nKensa">通板ポイント</param>
    '''' <value></value>
    '''' <returns></returns>
    '''' <remarks></remarks>
    'Public ReadOnly Property LenDisp(ByVal nRec As Integer, ByVal nKensa As Integer) As Integer
    '    Get
    '        Dim nLenDisp As Integer = -1

    '        If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
    '            If nKensa >= 0 And nKensa < MAX_DISP_POS Then
    '                nLenDisp = m_typHyPoint.pnt(nRec - 1).typ(0).nLenDisp(nKensa)
    '            End If
    '        End If

    '        Return nLenDisp
    '    End Get
    'End Property


    ''' <summary>
    ''' 各通板ポイントでの現在表示行
    ''' </summary>
    ''' <param name="nRec">レコード</param>
    ''' <param name="nKensa">検査台位置</param>
    ''' <param name="nStage">マップ切替[0:最小,1:表示小,2:表示大]</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RowDisp(ByVal nRec As Integer, ByVal nKensa As Integer, ByVal nStage As Integer) As Integer
        Get
            Dim nRowDisp As Integer = -1

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                If nKensa >= 0 And nKensa < MAX_DISP_POS Then
                    If nStage >= 0 And nStage < MAP_STAGE_NUM Then
                        nRowDisp = m_typHyPoint.pnt(nRec - 1).typ.nRowDisp(nKensa).nStage(nStage)
                    End If
                End If
            End If

            Return nRowDisp
        End Get
    End Property

    ''' <summary>
    ''' 現在書き込み行
    ''' </summary>
    ''' <param name="nRec">レコード</param>
    ''' <param name="nMen">面[0:表,1:裏]</param>
    ''' <param name="nStage">マップ切替[0:最小,1:表示小,2:表示大]</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RowWrite(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nStage As Integer) As Integer
        Get
            Dim nRowWrite As Integer = -1

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                If nMen >= 0 And nMen < NUM_MEN Then
                    If nStage >= 0 And nStage < MAP_STAGE_NUM Then
                        nRowWrite = m_typHyPoint.pnt(nRec - 1).typ.nRowWrite(nMen).nStage(nStage)
                    End If
                End If
            End If

            Return nRowWrite
        End Get
    End Property


    ''' <summary>
    ''' 検査完了からコイル長に変えたため、コイル長のＥＮＤ行を見つけるに変更
    ''' </summary>
    ''' <param name="nRec">レコード</param>
    ''' <param name="nStage">マップ切替[0:最小,1:表示小,2:表示大]</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RowLast(ByVal nRec As Integer, ByVal nStage As Integer) As Integer
        Get
            Dim nRowLast As Integer = -1

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                If nStage >= 0 And nStage < MAP_STAGE_NUM Then
                    nRowLast = m_typHyPoint.pnt(nRec - 1).typ.nRowLast(nStage)
                End If
            End If

            Return nRowLast
        End Get
    End Property


    ''' <summary>
    ''' 更新中フラグ
    ''' </summary>
    ''' <value></value>
    ''' <returns>TRUE:参照禁止、FALSE:参照可</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property Refresh() As Boolean
        Get
            Return CBool(m_typHyPoint.nRefresh)
        End Get
    End Property

    '///////////////////////////////////////////////////////////////////////////
    'スタティック関数
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 次レコード取得
    ''' </summary>
    ''' <param name="nRecNo">レコード</param>
    ''' <returns>次レコード</returns>
    ''' <remarks></remarks>
    Public Shared Function GetNextRecNo(ByVal nRecNo As Integer) As Integer
        nRecNo += 1

        If nRecNo < 1 Then                                  '不正値
            nRecNo = 1                                      '先頭
        End If

        If TBL_HY_REC_MAX < nRecNo Then                     '最大値オーバー
            nRecNo = 1                                      '先頭
        End If

        Return nRecNo
    End Function

    ''' <summary>
    ''' 前レコード取得
    ''' </summary>
    ''' <param name="nRecNo">レコード</param>
    ''' <returns>前レコード</returns>
    ''' <remarks></remarks>
    Public Shared Function GetPrevRecNo(ByVal nRecNo As Integer) As Integer
        nRecNo -= 1

        If nRecNo < 1 Then                                  '最小値アンダー
            nRecNo = TBL_HY_REC_MAX                         '末尾
        End If

        If TBL_HY_REC_MAX < nRecNo Then                     '不正値
            nRecNo = TBL_HY_REC_MAX                         '末尾
        End If

        Return nRecNo
    End Function

    '///////////////////////////////////////////////////////////////////////////
    'メンバ関数
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="clsLog"></param>
    ''' <remarks></remarks>
    Public Sub New(ByVal clsLog As tClass_LogManager)
        '初期化
        mcls_Log = clsLog                                                       'ログ管理

        m_typHyPoint = Nothing                                                  'ポインタ情報
        m_typHyPoint.initialize()
    End Sub

    ''' <summary>
    ''' デストラクタ
    ''' </summary>
    ''' <param name="disposing">True:明示的な呼び出し False:暗黙の呼び出し</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            If Not m_bIsDesposed Then
                'オブジェクト破棄
                m_typHyPoint = Nothing
            End If

            m_bIsDesposed = True                                                'Dispose済みとする
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オブジェクト破棄異常[{0}]", ex.Message))
        End Try
    End Sub
#Region " IDisposable Support "
    'このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        'このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' ポインタ情報読み込み
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadHyPoint()
        Dim bRet As Boolean = False

        Try
            'ポインタ情報読み込み
            m_typHyPoint = CType(ksMemRead(TBL_HY_POINT_NAME, GetType(TBL_HY_POINT)), TBL_HY_POINT)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]読み込み異常[{1}]", TBL_HY_POINT_NAME, ex.Message))
        End Try
    End Sub
End Class
