'*******************************************************************************
'　マップデータ
'	[Ver]
'		Ver.01    2010/11/30  初版
'
'	[メモ]
'		DBから疵マップデータ作成保持する
'*******************************************************************************
Option Strict On

Imports tClassLibrary.tClass
Imports tClassLibrary.tMod
Imports System.Data.SqlClient

''' <summary>
''' マップデータクラス
''' </summary>
''' <remarks></remarks>
Public Class clsDataMap
    Implements IDisposable
    '---------------------------------------------------------------------------
    ' 列挙体定義
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' 読み込みデータ種別
    ''' </summary>
    ''' <remarks></remarks>
    Public Enum DATA_KIND
        emCOIL_POS = 0
        emCOIL_INF
        emCUT_INF
    End Enum

    '---------------------------------------------------------------------------
    ' 型定義
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' 検索キー(疵リストデータ)
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure LIST_KEY
        Dim emStage As EM_LONG_POS                                              ' マップ間隔区分
        Dim nMen As Integer                                                     ' 表裏区分 0:表 1:裏
        Dim nCol As Integer                                                     ' 列番号 0オリジン
        Dim nRow As Integer                                                     ' 行番号 0オリジン

        Shared Operator =(ByVal a As LIST_KEY, ByVal b As LIST_KEY) As Boolean
            If a.emStage <> b.emStage Or a.nMen <> b.nMen Or a.nCol <> b.nCol Or a.nRow <> b.nRow Then
                Return False
            End If
            Return True
        End Operator

        Shared Operator <>(ByVal a As LIST_KEY, ByVal b As LIST_KEY) As Boolean
            If a.emStage <> b.emStage Or a.nMen <> b.nMen Or a.nCol <> b.nCol Or a.nRow <> b.nRow Then
                Return True
            End If
            Return False
        End Operator

        ''' <summary>
        ''' キーの初期化
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub initialize()
            emStage = EM_LONG_POS.MOSTSMALL
            nMen = -1
            nCol = -1
            nRow = -1
        End Sub

        ''' <summary>
        ''' キーの比較
        ''' </summary>
        ''' <param name="obj">比較対象キー</param>
        ''' <returns>True:一致 False:不一致</returns>
        ''' <remarks></remarks>
        Public Overrides Function Equals(ByVal obj As Object) As Boolean
            If TypeOf obj Is LIST_KEY = False Then
                Return False
            End If

            Dim arg As LIST_KEY = CType(obj, LIST_KEY)

            If arg.emStage = emStage _
            AndAlso arg.nMen = nMen _
            AndAlso arg.nCol = nCol _
            AndAlso arg.nRow = nRow Then                                        ' 全部の要素があって初めてOK
                Return True
            Else
                Return False
            End If
        End Function

        ''' <summary>
        ''' 文字列表現
        ''' </summary>
        ''' <returns>文字列表現</returns>
        ''' <remarks></remarks>
        Public Overrides Function ToString() As String
            Return String.Format("Stage={0} Men={1} Col={2} Row={3}", emStage, nMen, nCol, nRow)
        End Function
    End Structure

    ''' <summary>
    ''' 検索キー(疵リストキー)
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure KIZU_KEY
        Dim nKizuNo As Integer                                                  ' 疵No
        Dim nStage As Integer                                                   ' マップ間隔区分
    End Structure


    ''' <summary>
    ''' 検索キー(疵情報)
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure DEFECT_KEY
        Dim nKizuNo As Integer

        Public Sub New(ByVal no As Integer)
            Me.nKizuNo = no
        End Sub

    End Structure


    ''' <summary>
    ''' 読み込みパラメータ
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure READ_PARAM
        Dim strKanriNo As String                                              ' 管理No
        Dim nTidList As List(Of Integer)                                        ' 読み込み疵種EdasysID
        Dim nGidList As List(Of Integer)                                        ' 読み込みグレードEdasysID
        Dim bNagate As Boolean                                                  ' 長手方向反転 True:反転 False:なし
        Dim bDS As Boolean                                                      ' DS方向反転
        Dim bTansho As Boolean                                                  ' 探傷画面と同一疵チェック有無(True:チェックあり False:なし)
        Dim bFFSnd As Boolean                                                   ' ＦＦ送信済みチェック有無(True:チェックあり False:なし)
        Dim bCorrect As Boolean                                                 ' 補正率使用(True:あり False:なし)
    End Structure


    ' コイル情報構造体
    Public Structure TYP_COIL_INF
        Dim strKanriNo As String                    ' 管理No
        Dim strOrderNo As String                    ' 命令No
        Dim strCoilNo As String                     ' 生産No
        Dim strHinCode As String                    ' 品種
        Dim strUse As String                        ' 用途
        Dim strSyuZai As String                     ' 主材質
        Dim strNextCode As String                   ' 計画次工程コード
        Dim strKenKijun() As String                 ' 検査基準
        Dim nCoilAtu As Integer                     ' 材料板厚
        Dim nCoilWidth As Integer                   ' 材料板幅
        Dim nCoilAtuSei As Integer                  ' 注文板厚
        Dim nCoilWidSei As Integer                  ' 注文板幅
        Dim nTrimWidth As Integer                   ' トリム幅
        Dim strMuke As String                       ' 向先
        Dim nCoilLen As Integer                     ' 検査装置実測長 [mm]
        Dim bCorrect As Boolean                     ' 補正率使用(True:使用、False:不使用)
        Dim dCorrect As Double                      ' 補正率
        Dim nOutTopPos As Integer                   ' 出側単位先端位置 [mm]
        Dim nScode() As Integer                     ' 検査表面状態No
        Dim nPcode() As Integer                     ' 検査パターンコード
        Dim nKensa() As Integer                     ' 検査状態
        Dim nKiki() As Integer                      ' 機器状態
        Dim dtToroku As DateTime                    ' 登録日付
        Public Sub initialize()
            strKanriNo = String.Empty
            ReDim strKenKijun(NUM_MEN - 1)
            ReDim nKensa(NUM_MEN - 1)
            ReDim nKiki(NUM_MEN - 1)
            ReDim nScode(NUM_MEN - 1)
            ReDim nPcode(NUM_MEN - 1)
        End Sub
    End Structure

    ''' <summary>
    ''' フレーム情報
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure FRAME_INFO
        Dim nKanriNo As String
        Dim nFNo As Integer
        Dim nCamSet As Integer
        Dim nSSize As Integer
    End Structure

    '---------------------------------------------------------------------------
    ' メンバ変数
    '---------------------------------------------------------------------------
    Private m_bIsDesposed As Boolean = False                                    ' リソース開放済みフラウグ True:開放済み
    Private mcls_Param As clsParamManager                                       ' パラメータマネージャ
    Private mcls_PreParam As clsPreParamManager                                 ' 前工程パラメータ管理クラス
    Private mcls_Log As tClass_LogManager                                       ' ログ管理

    Private m_aryInterval() As Integer                                          ' 現在のマップ表示区分毎の距離
    Private m_bTurnLR As Boolean                                                ' マップデータの左右反転(True:反転あり False:なし)
    Private m_bTurnTB As Boolean                                                ' マップデータの上下反転(True:反転有り False:なし)

    Private m_stMap As DSP_G_MAP                                                ' マップ情報
    Private m_listCutInf As List(Of CUT_INF_DATA)                               ' カット情報

    Private m_bEnaRead As Boolean                                               ' 読み込み許可フラグ

    Private m_nColNum As Integer                                                ' MAP分割数(MAP_COL_NUMが最大)
    Private m_nColStart As Integer                                              ' 開始列
    Private m_nColEnd As Integer                                                ' 終了列

    Private m_dctDefLst As Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE)) ' ディクショナリ(疵リストデータ)
    Private m_dctDefKey As Dictionary(Of KIZU_KEY, LIST_KEY)                    ' ディクショナリ(疵リストキー)

    Private m_dctDefTcmLst As Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE)) ' ディクショナリ(TCM疵リストデータ)
    Private m_dctDefTcmKey As Dictionary(Of KIZU_KEY, LIST_KEY)                    ' ディクショナリ(TCM疵リストキー)

    Private m_dctDefInf() As Dictionary(Of DEFECT_KEY, DETECT_BASE_DATA)        ' ディクショナリ(疵情報) ※周期疵も含む
    Private m_dctDefTcmInf() As Dictionary(Of DEFECT_KEY, TYP_MAE_DEFECT_DATA)  ' ディクショナリ(TCM疵情報)
    Private m_lstFrameInf As New List(Of FRAME_INFO)                            ' フレーム情報リスト

    Private m_typCoilInf As New TYP_COIL_INF                                    ' コイル情報

    '---------------------------------------------------------------------------
    ' イベント
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' データの読み込み開始を通知します
    ''' </summary>
    ''' <param name="nKind">データ種別</param>
    ''' <param name="nMax">データ件数</param>
    ''' <remarks></remarks>
    Public Event StartDataNti(ByVal nKind As DATA_KIND, ByVal nMax As Integer)

    ''' <summary>
    ''' データの読み込みを通知します
    ''' </summary>
    ''' <param name="nKind">データ種別</param>
    ''' <param name="nCount">データ件数</param>
    ''' <remarks></remarks>
    Public Event ReadDataNti(ByVal nKind As DATA_KIND, ByVal nCount As Integer)

    '---------------------------------------------------------------------------
    ' プロパティ
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' 入側管理Noを取得する
    ''' </summary>
    ''' <value></value>
    ''' <returns>存在しない場合は""を返す</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KizukenNo_In() As String
        Get
            Return m_typCoilInf.strKanriNo
        End Get
    End Property


    ''' <summary>
    ''' 出側管理Noを取得する
    ''' </summary>
    ''' <value></value>
    ''' <returns>存在しない場合は""を返す</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KizukenNo_Out() As String
        Get
            Return m_typCoilInf.strKanriNo
        End Get
    End Property

    ''' <summary>
    ''' 登録日時を取得する
    ''' </summary>
    ''' <value></value>
    ''' <returns>存在しない場合は""を返す</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DateTime() As String
        Get
            Return m_typCoilInf.dtToroku.ToString("yyyy/MM/dd HH:mm:ss")
        End Get
    End Property

    ''' <summary>
    ''' コイル長を取得します
    ''' </summary>
    ''' <returns>コイル長[mm]</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property CoilLength() As Integer
        Get
            Return m_typCoilInf.nCoilLen
        End Get
    End Property

    ''' <summary>
    ''' コイル幅を取得します
    ''' </summary>
    ''' <returns>コイル幅[mm]</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property CoilWidth() As Integer
        Get
            Return m_typCoilInf.nCoilWidth
        End Get
    End Property


    ''' <summary>
    ''' マップ表示区分毎の距離を設定取得
    ''' </summary>
    ''' <param name="stage">マップ表示区分 0:最小値 1:表示(小) 2:表示(大)</param>
    ''' <value>距離[m]</value>
    ''' <returns>距離[m]</returns>
    ''' <remarks></remarks>
    Public Property Interval(ByVal stage As EM_LONG_POS) As Integer
        Get
            Return m_aryInterval(stage)
        End Get
        Set(ByVal value As Integer)
            m_aryInterval(stage) = value
        End Set
    End Property

    ''' <summary>
    ''' キャンセル有無を取得します
    ''' </summary>
    ''' <returns>True:キャンセルされた False:読み込み継続</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property IsReadCancel() As Boolean
        Get
            Return Not m_bEnaRead
        End Get
    End Property

    ''' <summary>
    ''' マップ値を取得します
    ''' </summary>
    ''' <param name="key">リストキー</param>
    ''' <param name="nIndex">リストインデックス</param>
    ''' <returns>バリュー</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ListValue(ByVal key As LIST_KEY, ByVal nIndex As Integer) As TYP_MAP_LIST_VALUE
        Get
            If m_dctDefLst.ContainsKey(key) = False Then
                Return Nothing
            ElseIf m_dctDefLst(key).Count <= nIndex Then
                Return Nothing
            Else
                Return m_dctDefLst(key)(nIndex)
            End If
        End Get
    End Property

    ''' <summary>
    ''' リスト内のインデックスを返します(0オリジン)
    ''' </summary>
    ''' <param name="key">リストキー</param>
    ''' <param name="val">バリュー</param>
    ''' <returns>リストインデックス</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ListIndex(ByVal key As LIST_KEY, ByVal val As TYP_MAP_LIST_VALUE) As Integer
        Get
            If m_dctDefLst.ContainsKey(key) = False Then
                Return 0
            Else
                Return m_dctDefLst(key).IndexOf(val)
            End If
        End Get
    End Property

    ''' <summary>
    ''' マップの疵情報リストを取得します
    ''' </summary>
    ''' <param name="nMen">0:表面 1:裏面</param>
    ''' <returns>マップの疵情報一覧</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectDataLst(ByVal nMen As Integer) As Dictionary(Of DEFECT_KEY, DETECT_BASE_DATA)
        Get
            Return m_dctDefInf(nMen)
        End Get
    End Property

    ''' <summary>
    ''' カット情報リストを取得します
    ''' </summary>
    ''' <returns>マップの疵情報一覧</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property CutDataLst() As List(Of CUT_INF_DATA)
        Get
            Return m_listCutInf
        End Get
    End Property

    ''' <summary>
    ''' 疵情報の有無を返します
    ''' </summary>
    ''' <param name="nMen">0:表面 1:裏面</param>
    ''' <param name="nKizuNo">疵No</param>
    ''' <returns>True:有り、1:無し</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectDataUmu(ByVal nMen As Integer, ByVal nKizuNo As clsDataMap.DEFECT_KEY) As Boolean
        Get
            If m_dctDefInf(nMen).ContainsKey(nKizuNo) = False Then
                Return False
            End If
            Return True
        End Get
    End Property

    ''' <summary>
    ''' マップの疵件数を返します
    ''' </summary>
    ''' <param name="nMen">0:表面 1:裏面</param>
    ''' <returns>疵件数</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DefectDataCnt(ByVal nMen As Integer) As Integer
        Get
            If m_dctDefInf Is Nothing = True Then
                Return 0
            End If
            Return m_dctDefInf(nMen).Count
        End Get
    End Property

    ''' <summary>
    ''' 疵Noの有無を返します
    ''' </summary>
    ''' <param name="key">リストキー</param>
    ''' <returns>True:有り、1:無し</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ListKizuNoKeyUmu(ByVal key As KIZU_KEY) As Boolean
        Get
            If m_dctDefKey.ContainsKey(key) = False Then
                Return False
            End If
            Return True
        End Get
    End Property

    ''' <summary>
    ''' キーに対する疵リストを返します
    ''' </summary>
    ''' <param name="key">リストキー</param>
    ''' <returns>バリュー</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property ListGetKizuNoKey(ByVal key As KIZU_KEY) As LIST_KEY
        Get
            If m_dctDefKey.ContainsKey(key) = False Then
                Return Nothing
            End If
            Return m_dctDefKey(key)
        End Get
    End Property

    ''' <summary>
    ''' コイル情報
    ''' </summary>
    ''' <returns></returns>
    Public ReadOnly Property CoilInfo() As TYP_COIL_INF
        Get
            Return m_typCoilInf
        End Get
    End Property

    ''' <summary>
    ''' 長手方向反転
    ''' </summary>
    ''' <returns>True:有り、False:無し</returns>
    Public ReadOnly Property TurnTB() As Boolean
        Get
            Return m_bTurnTB
        End Get
    End Property

    ''' <summary>
    ''' 幅方向反転
    ''' </summary>
    ''' <returns>True:有り、False:無し</returns>
    Public ReadOnly Property TurnLR() As Boolean
        Get
            Return m_bTurnLR
        End Get
    End Property


    '---------------------------------------------------------------------------
    ' メソッド
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' マップデータの作成
    ''' </summary>
    ''' <param name="param">パラメータ管理</param>
    ''' <param name="log">ログ管理</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef param As clsParamManager, ByRef preparam As clsPreParamManager, ByRef log As tClass_LogManager)
        Try
            mcls_Param = param
            mcls_PreParam = preparam
            mcls_Log = log

            ReDim m_aryInterval(MAP_STAGE_NUM - 1)
            For stage As Integer = 0 To m_aryInterval.Length - 1
                m_aryInterval(stage) = MAP_REPRES_MIN
            Next

            m_nColNum = 0
            m_nColStart = 0
            m_nColEnd = MAP_COL_NUM - 1
            m_bTurnLR = False

            Dim sw As New Stopwatch

            ' MAP行数を動的に確保するように変更 初期は100行分あれば十分
            modMain.Log(tClass_LogManager.LNO.INF, "領域の確保前 ")
            sw.Start()
            ' 疵MAP 初期化
            m_stMap.initialize(100)
            sw.Stop()
            modMain.Log(tClass_LogManager.LNO.INF, "領域の確保時間 [{0}]", sw.Elapsed)

            m_listCutInf = New List(Of CUT_INF_DATA)
            m_listCutInf.Clear()

            m_dctDefLst = New Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE))
            m_dctDefKey = New Dictionary(Of KIZU_KEY, LIST_KEY)
            m_dctDefTcmLst = New Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE))
            m_dctDefTcmKey = New Dictionary(Of KIZU_KEY, LIST_KEY)
            ReDim m_dctDefInf(NUM_MEN - 1)
            For ii As Integer = 0 To m_dctDefInf.Length - 1
                m_dctDefInf(ii) = New Dictionary(Of DEFECT_KEY, DETECT_BASE_DATA)
            Next

            ReDim m_dctDefTcmInf(NUM_MEN - 1)
            For ii As Integer = 0 To m_dctDefTcmInf.Length - 1
                m_dctDefTcmInf(ii) = New Dictionary(Of DEFECT_KEY, TYP_MAE_DEFECT_DATA)
            Next

        Catch ex As Exception
            log.Write(tClass_LogManager.LNO.ERR, String.Format("初期化異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' オブジェクトの破棄
    ''' </summary>
    ''' <param name="disposing">True:明示的な呼び出し False:暗黙の呼び出し</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            ' TODO オブジェクト破棄
            If m_bIsDesposed = False Then
                m_bIsDesposed = True                                            ' Dispose済みとする

                If m_listCutInf IsNot Nothing Then
                    m_listCutInf.Clear()
                    m_listCutInf = Nothing
                End If
                If m_dctDefLst IsNot Nothing Then
                    m_dctDefLst.Clear()
                    m_dctDefLst = Nothing
                End If
                If m_dctDefKey IsNot Nothing Then
                    m_dctDefKey.Clear()
                    m_dctDefKey = Nothing
                End If
                If m_dctDefTcmLst IsNot Nothing Then
                    m_dctDefTcmLst.Clear()
                    m_dctDefTcmLst = Nothing
                End If
                If m_dctDefTcmKey IsNot Nothing Then
                    m_dctDefTcmKey.Clear()
                    m_dctDefTcmKey = Nothing
                End If
                If m_dctDefInf IsNot Nothing Then
                    For ii As Integer = 0 To m_dctDefInf.Length - 1
                        If m_dctDefInf(ii) IsNot Nothing Then
                            m_dctDefInf(ii).Clear()
                            m_dctDefInf(ii) = Nothing
                        End If
                    Next
                    m_dctDefInf = Nothing
                End If
                If m_dctDefTcmInf IsNot Nothing Then
                    For ii As Integer = 0 To m_dctDefTcmInf.Length - 1
                        If m_dctDefTcmInf(ii) IsNot Nothing Then
                            m_dctDefTcmInf(ii).Clear()
                            m_dctDefTcmInf(ii) = Nothing
                        End If
                    Next
                    m_dctDefInf = Nothing
                End If
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップデータオブジェクト破棄異常 [{0}]", ex.Message))
        End Try
    End Sub
#Region " IDisposable Support "
    ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' マップデータの作成をキャンセルします
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub CancelRead()
        Try
            m_bEnaRead = False
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップデータ読み込みキャンセル異常 [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' マップデータを読み込みます
    ''' </summary>
    ''' <param name="param"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function Read(ByVal param As READ_PARAM) As Boolean
        Try
            Dim nMax As Integer = 0
            Dim bRet As Boolean = False

            With param

                '-------------------------------------------------------------------
                ' 読み込み開始処理
                '-------------------------------------------------------------------
                ' データをクリア
                m_typCoilInf.initialize()
                m_dctDefKey.Clear()
                m_dctDefLst.Clear()
                m_dctDefTcmKey.Clear()
                m_dctDefTcmLst.Clear()
                For ii As Integer = 0 To m_dctDefInf.Length - 1
                    m_dctDefInf(ii).Clear()
                Next
                For ii As Integer = 0 To m_dctDefTcmInf.Length - 1
                    m_dctDefTcmInf(ii).Clear()
                Next

                ' コイルデータを生成する
                bRet = ReadCoilInfo(.strKanriNo, .bCorrect)
                If Not bRet Then
                    Return False
                End If

                'If .strKanriNoIn <> "" Then
                '    ' コイル情報をログに出力
                '    Log(tClass_LogManager.LNO.MSG, "入側コイル[{0}] コイル長[{1:###,###,##0} mm]", m_lstCoilInf(0).strOutKizuKen, m_lstCoilInf(0).nCoilLen)
                'End If

                ' マップデータイニシャル
                bRet = ReadInitialize(.bNagate)                             ' 読み込み開始処理
                If Not bRet Then
                    Return False
                End If

                '-------------------------------------------------------------------
                ' マップ位置情報読み込み(中央)
                '-------------------------------------------------------------------
                ' データ数を取得
                bRet = GetMapPosCount(m_typCoilInf.strKanriNo, DB_COIL_POSITION, nMax)
                If Not bRet Then
                    Return False
                End If

                RaiseEvent StartDataNti(DATA_KIND.emCOIL_POS, nMax)         ' マップ位置情報最大数を通知する
                Application.DoEvents()

                ' データ読み込み
                bRet = ReadMapPos(m_typCoilInf.strKanriNo, DB_COIL_POSITION, .bNagate, m_typCoilInf.bCorrect)            ' マップ位置情報読み込み
                If Not bRet Then
                    Return False
                End If

				'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>> ※場所移動
				'-------------------------------------------------------------------
				' カット情報読み込み
				'-------------------------------------------------------------------
				' 情報数の取得
				bRet = GetCutInfCount(m_typCoilInf.strKanriNo, nMax)
				If Not bRet Then
					Return False
				End If

				RaiseEvent StartDataNti(DATA_KIND.emCUT_INF, nMax)
				Application.DoEvents()

				' カット情報読み込み
				bRet = ReadCutInf(m_typCoilInf.strKanriNo, .bNagate, m_typCoilInf.bCorrect)                  ' 疵マップ情報読み込み
				If Not bRet Then
					Return False
				Else
					Application.DoEvents()
				End If
				'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

				'-------------------------------------------------------------------
				' マップ情報読み込み
				'-------------------------------------------------------------------
				' 情報数の取得
				bRet = GetMapInfCount(m_typCoilInf.strKanriNo, .nTidList, .nGidList, .bTansho, .bFFSnd, nMax)
                If Not bRet Then
                    Return False
                End If

                RaiseEvent StartDataNti(DATA_KIND.emCOIL_INF, nMax)
                Application.DoEvents()

                ' マップ情報読み込み
                bRet = ReadMapInf(m_typCoilInf.strKanriNo, .nTidList, .nGidList, .bTansho, .bNagate, .bDS, .bFFSnd, m_typCoilInf.bCorrect)   ' 疵マップ情報読み込み
                If Not bRet Then
                    Return False
                Else
                    Application.DoEvents()
                End If

                ' マップ情報読み込み
                bRet = ReadPreMapInf(m_typCoilInf.strKanriNo, .bNagate, .bDS)   ' 疵マップ情報読み込み
                'If Not bRet Then
                '    Return False
                'Else
                '    Application.DoEvents()
                'End If


                Sort_Map_Kizulist(m_dctDefLst, .bNagate)
                ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
                'Sort_Map_Kizulist(m_dctDefTcmLst, .bNagate)
                Sort_Map_KizulistTcm(m_dctDefTcmLst, .bNagate)
				' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

				'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>> ※場所移動
				'-------------------------------------------------------------------
				' カット情報読み込み
				'-------------------------------------------------------------------
				'' 情報数の取得
				'bRet = GetCutInfCount(m_typCoilInf.strKanriNo, nMax)
				'If Not bRet Then
				'	Return False
				'End If
				'
				'RaiseEvent StartDataNti(DATA_KIND.emCUT_INF, nMax)
				'Application.DoEvents()
				'
				'' カット情報読み込み
				'bRet = ReadCutInf(m_typCoilInf.strKanriNo, .bNagate, m_typCoilInf.bCorrect)                  ' 疵マップ情報読み込み
				'If Not bRet Then
				'    Return False
				'Else
				'	Application.DoEvents()
				'End If
				'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

				ReadFinalize(.bNagate)                                           ' 読み込み終了処理

                m_bTurnLR = .bDS
                m_bTurnTB = .bNagate

            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップデータ読み込み異常 [{0}]", ex.Message))
            Return False
        End Try

        Return True
    End Function


    ''' <summary>
    ''' コイル情報を読み込みます
    ''' </summary>
    ''' <param name="strKanriNo">管理No</param>
    ''' <param name="bCorrect">補正率使用(True:あり、False:なし)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ReadCoilInfo(ByVal strKanriNo As String, ByVal bCorrect As Boolean) As Boolean
        Dim tcls_DB As tClass_SQLServer = Nothing
        Dim sqlRead As SqlDataReader = Nothing
        Dim bRet As Boolean = False

        Try

            '-------------------------------------------------------------------
            ' ＳＱＬ作成
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " RES.登録日付 as 登録日付"
            strSQL &= " ,RES.管理No "
            strSQL &= " ,ISNULL(RES.検査実測長,0) as 検査実測長 "
            strSQL &= " ,ISNULL(RES.検査状態_表, 0) as 検査状態_表 "
            strSQL &= " ,ISNULL(RES.検査状態_裏, 0) as 検査状態_裏 "
            strSQL &= " ,ISNULL(RES.機器状態_表, 0) as 機器状態_表 "
            strSQL &= " ,ISNULL(RES.機器状態_裏, 0) as 機器状態_裏 "
            strSQL &= " ,ISNULL(INF.命令No, '') as 命令No"
            strSQL &= " ,ISNULL(INF.生産No,'') as 生産No "
            strSQL &= " ,ISNULL(INF.品種, '') as 品種"
            strSQL &= " ,ISNULL(INF.用途, '') as 用途"
            strSQL &= " ,ISNULL(INF.主材質, '') as 主材質"
            strSQL &= " ,ISNULL(INF.計画次工程コード, '') as 次工程コード "
            strSQL &= " ,ISNULL(INF.検査基準_表, '') as 検査基準_表 "
            strSQL &= " ,ISNULL(INF.検査基準_裏, '') as 検査基準_裏 "
            strSQL &= " ,ISNULL(INF.材料板厚, 0) as 材料板厚 "
            strSQL &= " ,ISNULL(INF.材料板幅, 0) as 材料板幅 "
            strSQL &= " ,ISNULL(INF.注文板厚, 0) as 注文板厚 "
            strSQL &= " ,ISNULL(INF.注文板幅, 0) as 注文板幅 "
            strSQL &= " ,ISNULL(INF.現工程トリム幅, 0) as トリム幅 "
            strSQL &= " ,ISNULL(INF.向先, '') as 向先 "
            strSQL &= " ,ISNULL(RES.検査表面状態No_表, 0) as 検査表面状態_表 "
            strSQL &= " ,ISNULL(RES.検査表面状態No_裏, 0) as 検査表面状態_裏 "
            strSQL &= " ,ISNULL(RES.検査パターンコード_表, 0) as 検査パターンコード_表 "
            strSQL &= " ,ISNULL(RES.検査パターンコード_裏, 0) as 検査パターンコード_裏 "
            strSQL &= " ,ISNULL(RES.補正率, 1) as 補正率 "
            strSQL &= " ,ISNULL(RES.出側単位先端位置, 0) as 出側単位先端位置 "

            ' SQL FROM句
            strSQL &= " FROM " & DB_COIL_RESULT & " RES "
            ' SQL 外部結合句
            strSQL &= " LEFT OUTER JOIN " & DB_COIL_INF & " INF "
            strSQL &= " ON RES.管理No = INF.管理No "
            ' SQL WHERE句
            strSQL &= " WHERE RES.管理No = '" & strKanriNo & "' "

            '-------------------------------------------------------------------
            ' 実行
            '-------------------------------------------------------------------
            tcls_DB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = tcls_DB.Open()
            If Not bDbRet Then                                              ' DB接続失敗
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報取得DB接続失敗"))
                Exit Try
            End If

            sqlRead = tcls_DB.SelectExecute(strSQL)
            If sqlRead Is Nothing Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報取得SQL実行失敗 SQL={0}", strSQL))
                Exit Try
            End If

            If Not sqlRead.Read() Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報データなし={0}", strKanriNo))
                Exit Try
            End If

            '-------------------------------------------------------------------
            ' 格納
            '-------------------------------------------------------------------
            m_typCoilInf.initialize()
            With sqlRead
                m_typCoilInf.strKanriNo = CStr(.GetValue(.GetOrdinal("管理No")))              ' COIL_RESULT 管理No
                m_typCoilInf.strOrderNo = CStr(.GetValue(.GetOrdinal("命令No")))              ' COIL_INF 命令No
                m_typCoilInf.strCoilNo = CStr(.GetValue(.GetOrdinal("生産No")))               ' COIL_INF 生産No
                m_typCoilInf.strHinCode = CStr(.GetValue(.GetOrdinal("品種")))                ' COIL_INF 品種
                m_typCoilInf.strUse = CStr(.GetValue(.GetOrdinal("用途")))                    ' COIL_INF 用途
                m_typCoilInf.strSyuZai = CStr(.GetValue(.GetOrdinal("主材質")))               ' COIL_INF 主材質
                m_typCoilInf.strNextCode = CStr(.GetValue(.GetOrdinal("次工程コード")))       ' COIL_INF 計画次工程コード
                m_typCoilInf.strKenKijun(0) = CStr(.GetValue(.GetOrdinal("検査基準_表")))     ' COIL_INF 検査基準_表
                m_typCoilInf.strKenKijun(1) = CStr(.GetValue(.GetOrdinal("検査基準_裏")))     ' COIL_INF 検査基準_表
                m_typCoilInf.nCoilAtu = .GetInt32(.GetOrdinal("材料板厚"))                    ' COIL_INF 材料板厚
                m_typCoilInf.nTrimWidth = .GetInt32(.GetOrdinal("トリム幅"))                  ' COIL_INF 現工程トリム幅
                m_typCoilInf.nCoilWidth = .GetInt32(.GetOrdinal("材料板幅"))                  ' COIL_INF 材料板幅
                'If 0 = m_typCoilInf.nTrimWidth Then
                '    ' トリム幅が0の場合は、材料板幅を使用する
                '    m_typCoilInf.nCoilWidth = .GetInt32(.GetOrdinal("材料板幅"))            　' COIL_INF 材料板幅
                'Else
                '    m_typCoilInf.nCoilWidth = m_typCoilInf.nTrimWidth
                'End If
                m_typCoilInf.nCoilAtuSei = .GetInt32(.GetOrdinal("注文板厚"))                 ' COIL_INF 注文板厚
                m_typCoilInf.nCoilWidSei = .GetInt32(.GetOrdinal("注文板幅"))            　   ' COIL_INF 注文板幅
                m_typCoilInf.strMuke = CStr(.GetValue(.GetOrdinal("向先")))                   ' COIL_INF 向先
                m_typCoilInf.nCoilLen = .GetInt32(.GetOrdinal("検査実測長"))                  ' COIL_RESULT 検査実測長
                m_typCoilInf.nScode(0) = .GetInt32(.GetOrdinal("検査表面状態_表"))            ' COIL_RESULT 検査表面状態No_表
                m_typCoilInf.nScode(1) = .GetInt32(.GetOrdinal("検査表面状態_裏"))            ' COIL_RESULT 検査表面状態No_表
                m_typCoilInf.nPcode(0) = .GetInt32(.GetOrdinal("検査パターンコード_表"))      ' COIL_RESULT 検査パターンコード_表
                m_typCoilInf.nPcode(1) = .GetInt32(.GetOrdinal("検査パターンコード_裏"))      ' COIL_RESULT 検査パターンコード_表
                m_typCoilInf.nKensa(0) = .GetInt32(.GetOrdinal("検査状態_表"))                ' COIL_RESULT 検査状態_表
                m_typCoilInf.nKensa(1) = .GetInt32(.GetOrdinal("検査状態_裏"))                ' COIL_RESULT 検査状態_表
                m_typCoilInf.nKiki(0) = .GetInt32(.GetOrdinal("機器状態_表"))                 ' COIL_RESULT 機器状態_表
                m_typCoilInf.nKiki(1) = .GetInt32(.GetOrdinal("機器状態_裏"))                 ' COIL_RESULT 機器状態_表

                m_typCoilInf.dtToroku = .GetDateTime(.GetOrdinal("登録日付"))                 ' COIL_RESULT 登録日付
                m_typCoilInf.dCorrect = .GetDouble(.GetOrdinal("補正率"))                     ' COIL_RESULT 補正率
                If 0 >= m_typCoilInf.dCorrect Then
                    m_typCoilInf.dCorrect = 1
                End If

                m_typCoilInf.bCorrect = bCorrect
                If m_typCoilInf.bCorrect Then
                    'コイル長に補正率を設定
                    m_typCoilInf.nCoilLen = CInt(m_typCoilInf.nCoilLen * m_typCoilInf.dCorrect)
                End If
                m_typCoilInf.nOutTopPos = .GetInt32(.GetOrdinal("出側単位先端位置"))        ' COIL_RESULT 出側単位先端位置

            End With

            ' 読み込み成功なので、戻り値にTrueをセット
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報取得読み込み 例外発生 [{0}]", ex.Message))
        Finally
            If tcls_DB IsNot Nothing Then
                tcls_DB.Cancel()
                If sqlRead IsNot Nothing Then
                    sqlRead.Close()
                End If
                tcls_DB.Close()
                tcls_DB.Dispose()
            End If
        End Try

        Return bRet

    End Function


    ''' <summary>
    ''' マップデータ初期化
    ''' </summary>
    ''' <param name="bNagate">長手展開反転(True:あり、False:なし)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ReadInitialize(ByVal bNagate As Boolean) As Boolean
        Try
            m_bEnaRead = True

            '-------------------------------------------------------------------
            ' マップ幅の計算
            '-------------------------------------------------------------------
            Dim nColNum As Integer = SelectGridCol2(CoilWidth, MAP_COL_INTERVAL, MAP_COL_NUM)
            m_nColNum = nColNum
            m_nColStart = (MAP_COL_NUM - nColNum) \ 2
            m_nColEnd = MAP_COL_NUM - m_nColStart - 1

            '-------------------------------------------------------------------
            ' マップ行数を必要な行数に合わせて動的に確保する
            '-------------------------------------------------------------------
            Dim sw As New Stopwatch
            Dim nRow As Integer = CoilLength \ 1000 \ Interval(0) + 1       ' 確保数行数
            modMain.Log(tClass_LogManager.LNO.INF, "領域の確保前 確保する行数[{0}]", nRow)
            sw.Start()
            m_stMap.initialize(nRow)
            sw.Stop()
            modMain.Log(tClass_LogManager.LNO.INF, "領域の確保時間 [{0}] 行数[{1}]", sw.Elapsed, nRow)

            ' 行情報の初期化を行う
            For nStage As Integer = 0 To m_stMap.stage.Length - 1
                For nMen As Integer = 0 To m_stMap.stage(nStage).men.Length - 1
                    For nRow = 0 To CoilLength \ 1000 \ Interval(CType(nStage, EM_LONG_POS))
                        If nRow >= m_stMap.stage(nStage).men(nMen).row.Length Then
                            ' 想定よりも行が長い？ 行情報の領域確保失敗
                            mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("マップ位置情報読み込み異常 入側距離算出時異常 nRow[{0}} マップ行数[{1}]", nRow, m_stMap.stage(0).men(nMen).row.Length))
                            Continue For
                        End If
                        ' 行情報初期値セット
                        With m_stMap.stage(nStage).men(nMen).row(nRow)
                            .nLen_i = (nRow + 1) * Interval(CType(nStage, EM_LONG_POS)) ' 入側距離設定
                            .nLen_o = .nLen_i                                           ' 出側距離設定
                            .emKiki = 0
                            .emKensa = EM_DIV_KENSA.DIV_KENSA_OK
                        End With
                        ' 各列の情報を初期化
                        For col As Integer = 0 To m_stMap.stage(nStage).men(nMen).row(nRow).col.Length - 1                 ' マップ情報初期化
                            If col < m_nColStart OrElse m_nColEnd < col Then
                                ' コイル幅の範囲外の場合
                                m_stMap.stage(nStage).men(nMen).row(nRow).col(col).nKizuNo = -2
                            Else
                                ' コイル幅の範囲内の場合
                                m_stMap.stage(nStage).men(nMen).row(nRow).col(col).nKizuNo = 0
                            End If
                        Next
                        'TCM初期化
                        For col As Integer = 0 To m_stMap.stage(nStage).men(nMen).row(nRow).tcm.Length - 1
                            m_stMap.stage(nStage).men(nMen).row(nRow).tcm(col).nKizuNo = 0
                        Next

                    Next
                Next
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップデータ読み込み初期化異常 [{0}]", ex.Message))
            Return False
        End Try

        Return m_bEnaRead
    End Function


    ''' <summary>
    ''' マップ位置情報数取得
    ''' </summary>
    ''' <param name="strKanriNo">管理No</param>
    ''' <param name="nCount">取得した個数(戻り値)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetMapPosCount(ByVal strKanriNo As String, ByVal strTable As String, ByRef nCount As Integer) As Boolean

        Dim bRet As Boolean = False

        Try
            nCount = 0
            '-------------------------------------------------------------------
            ' ＳＱＬ作成
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " COUNT(*)"
            strSQL &= " FROM " & strTable
            strSQL &= " WHERE"
            strSQL &= " 管理No ='" & strKanriNo & "'"

            '-------------------------------------------------------------------
            ' 実行
            '-------------------------------------------------------------------
            If ExecuteSelectCount(strSQL, nCount) Then
                ' 読み込み成功なので、戻り値にTrueをセット
                bRet = True
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ位置情報数取得異常 [{0}][{1}]", strTable, ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' マップ位置情報読み込み
    ''' </summary>
    ''' <param name="strKanriNo">管理No</param>
    ''' <param name="strTable">テーブル名称</param>
    ''' <param name="bNagate"></param>
    ''' <param name="bCorrect">補正率使用(True:あり、False:なし)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ReadMapPos(ByVal strKanriNo As String, ByVal strTable As String, ByVal bNagate As Boolean, ByVal bCorrect As Boolean) As Boolean
        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Dim bRet As Boolean = False
        Try
            '-------------------------------------------------------------------
            ' ＳＱＬ作成
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " *"
            strSQL &= " FROM"
            strSQL &= " " & strTable
            strSQL &= " WHERE"
            strSQL &= " 管理No ='" & strKanriNo & "'"
            strSQL &= " ORDER BY ID ASC"

            '-------------------------------------------------------------------
            ' 実行
            '-------------------------------------------------------------------
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If Not bDbRet Then                                              ' DB接続失敗
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ位置情報DB接続失敗"))
                Exit Try
            End If

            read = clsDB.SelectExecute(strSQL)
            If read Is Nothing Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ位置情報取得SQL実行失敗 SQL={0}", strSQL))
                Exit Try
            End If

            '-------------------------------------------------------------------
            ' 格納
            '-------------------------------------------------------------------
            Dim nCount As Integer = 0
            Dim nMaxRow As Integer = m_stMap.stage(0).men(0).row.Length - 1 ' 最大行番号
            Dim nCurRow As Integer = 0                                      ' 設定済みの行番号
            If bNagate Then
                '長手方向に反転の場合は、末尾から開始する
                nCurRow = nMaxRow
            End If

            ' 開始行の検査状態を対象外にしておく
            For nMen As Integer = 0 To m_stMap.stage(0).men.Length - 1
                m_stMap.stage(0).men(nMen).row(nCurRow).emKensa = EM_DIV_KENSA.DIV_KENSA_NON
            Next

            While read.Read()
                If Not m_bEnaRead Then                                      ' 読み込み不可の場合
                    '読み込みキャンセル
                    Exit Try
                End If

                Dim nRow As Integer = CInt(read.GetValue(read.GetOrdinal("ID")))
                Dim nPos As Integer = nRow * Interval(0)

                If bCorrect Then
                    '補正率を加味する
                    Dim dHosei As Double = m_typCoilInf.dCorrect
                    nPos = CInt(Int(nPos * dHosei))
                End If
                nRow = nPos \ Interval(0)

                If nRow > nMaxRow Then                                     ' 最大値を超えている場合
                    'mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ位置情報の行数オーバー Row={0} 管理No={1}", nRow, strKizukenNo))
                    nRow = nMaxRow
                End If
                If nRow < 0 Then                                                ' 逆展開したときとかマイナスになる可能性あり
                    'mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ位置情報の行数オーバー Row={0} 管理No={1}", nRow, strKizukenNo))
                    nRow = 0
                End If

                If bNagate Then
                    '長手方向に反転の場合は、行位置を変換
                    nRow = nMaxRow - nRow
                End If

                With m_stMap.stage(0)
                    ' 行情報に歯抜けがある場合は、設定済み行の情報を歯抜け行にセットする。
                    If bNagate AndAlso nCurRow - nRow > 1 Then
                        ' 長手反転あり
                        For ii As Integer = nCurRow - 1 To nRow + 1 Step -1
                            If 0 > ii Or nMaxRow < ii Then
                                Continue For
                            End If
                            For nMen As Integer = 0 To .men.Length - 1
                                .men(nMen).row(ii).emKensa = .men(nMen).row(nCurRow).emKensa
                                .men(nMen).row(ii).emKiki = .men(nMen).row(nCurRow).emKiki
                            Next
                        Next
                    ElseIf Not bNagate AndAlso nRow - nCurRow > 1 Then
                        ' 長手反転なし
                        For ii As Integer = nCurRow + 1 To nRow - 1
                            If 0 > ii Or nMaxRow < ii Then
                                Continue For
                            End If
                            For nMen As Integer = 0 To .men.Length - 1
                                .men(nMen).row(ii).emKensa = .men(nMen).row(nCurRow).emKensa
                                .men(nMen).row(ii).emKiki = .men(nMen).row(nCurRow).emKiki
                            Next
                        Next
                    End If

                    Dim emKensaO As EM_DIV_KENSA = CType(read.GetValue(read.GetOrdinal("検査状態_表")), EM_DIV_KENSA)
                    Dim emKensaU As EM_DIV_KENSA = CType(read.GetValue(read.GetOrdinal("検査状態_裏")), EM_DIV_KENSA)
                    If .men(0).row(nRow).emKensa < emKensaO Then
                        .men(0).row(nRow).emKensa = emKensaO
                    End If
                    If .men(1).row(nRow).emKensa < emKensaU Then
                        .men(1).row(nRow).emKensa = emKensaU
                    End If
                    Dim emKikiO As EM_DIV_KIKI = CType(read.GetValue(read.GetOrdinal("機器状態_表")), EM_DIV_KIKI)
                    Dim emKikiU As EM_DIV_KIKI = CType(read.GetValue(read.GetOrdinal("機器状態_裏")), EM_DIV_KIKI)
                    If .men(0).row(nRow).emKiki < emKikiO Then
                        .men(0).row(nRow).emKiki = emKikiO
                    End If
                    If .men(1).row(nRow).emKiki < emKikiU Then
                        .men(1).row(nRow).emKiki = emKikiU
                    End If

                    nCurRow = nRow
                End With

                nCount += 1                                                     ' 一つはいりましたよー
                If nCount Mod 100 = 0 Then
                    RaiseEvent ReadDataNti(DATA_KIND.emCOIL_POS, nCount)            ' 読み込み件数通知
                    Application.DoEvents()                                          ' つっかえてるイベントを実行する
                End If
            End While

            'COIL_POSITIONの最終行よりも、実績コイル長が長い場合の歯抜け対応
            If bNagate AndAlso 0 < nCurRow Then
                ' 長手反転あり
                For ii As Integer = nCurRow - 1 To 0 Step -1
                    If 0 > ii Or nMaxRow < ii Then
                        Continue For
                    End If
                    With m_stMap.stage(0)
                        For nMen As Integer = 0 To .men.Length - 1
                            .men(nMen).row(ii).emKensa = .men(nMen).row(nCurRow).emKensa
                            .men(nMen).row(ii).emKiki = .men(nMen).row(nCurRow).emKiki
                        Next
                    End With
                Next ii
            ElseIf Not bNagate AndAlso nMaxRow > nCurRow Then
                ' 長手反転なし
                For ii As Integer = nCurRow + 1 To nMaxRow
                    If 0 > ii Or nMaxRow < ii Then
                        Continue For
                    End If
                    With m_stMap.stage(0)
                        For nMen As Integer = 0 To .men.Length - 1
                            .men(nMen).row(ii).emKensa = .men(nMen).row(nCurRow).emKensa
                            .men(nMen).row(ii).emKiki = .men(nMen).row(nCurRow).emKiki
                        Next
                    End With
                Next ii
            End If

            ' 読み込み成功なので、戻り値にTrueをセット
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ位置情報読み込み異常 [{0}]", ex.Message))

        Finally
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' マップ情報数を取得します
    ''' </summary>
    ''' <param name="strKanriNo">管理No</param>
    ''' <param name="nTid">表示対象外疵種EdasysIDリスト</param>
    ''' <param name="nGid">表示対象外グレードEdasysIDリスト</param>
    ''' <param name="bTansho">探傷画面と同一疵チェック有無 True:チェックあり False:なし</param>
    ''' <param name="bFFSnd">仮想疵検用疵のみチェック有無 True:チェックあり False:なし</param>
    ''' <param name="nCount">情報数</param>
    ''' <returns>True:取得性成功 False:取得失敗</returns>
    ''' <remarks></remarks>
    Private Function GetMapInfCount(
     ByVal strKanriNo As String,
     ByVal nTid As List(Of Integer),
     ByVal nGid As List(Of Integer),
     ByVal bTansho As Boolean,
     ByVal bFFSnd As Boolean,
     ByRef nCount As Integer) As Boolean

        Dim bRet As Boolean = False

        Try
            Dim strTable() As String = {DB_DEFECT_INF_T, DB_DEFECT_INF_B}
            nCount = 0
            For ii As Integer = 0 To strTable.Length - 1
                '---------------------------------------------------------------
                ' ＳＱＬ作成
                '---------------------------------------------------------------
                Dim strSQL As String = ""
                strSQL &= "SELECT"
                strSQL &= " COUNT(*)"
                strSQL &= " FROM"
                strSQL &= " " & strTable(ii)
                strSQL &= " d "

                If bFFSnd Then
                    'strSQL &= " ," & DB_ACCESS_NEXT_DEFECT_INF & " adf "
                End If

                strSQL &= " WHERE"
                strSQL &= " d.管理No = '" & strKanriNo & "'"
                strSQL &= " AND d.重複結果 <> 1 "

                'FFサーバ送信疵選択機能追加
                If bFFSnd Then
                    'strSQL &= "AND ((adf.管理No = d.管理No) AND (adf.疵No=d.疵No)) "
                ElseIf bTansho Then
                    strSQL &= " AND d.表示対象区分_探傷画面 = 1 "
                Else
                    For jj As Integer = 0 To nTid.Count - 1                         ' 疵種設定
                        strSQL &= " AND d.判定疵種 = " & CStr(nTid(jj))
                    Next

                    For jj As Integer = 0 To nGid.Count - 1                         ' グレード設定
                        strSQL &= " AND d.判定グレード = " & CStr(nGid(jj))
                    Next
                End If

                '---------------------------------------------------------------
                ' 実行
                '---------------------------------------------------------------
                Dim nCnt As Integer = 0
                If ExecuteSelectCount(strSQL, nCnt) Then
                    nCount += nCnt
                Else
                    Exit Try
                End If
            Next

            ' 読み込み成功なので、戻り値にTrueをセット
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ情報数取得異常 [{0}]", ex.Message))
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' マップ情報読み込み
    ''' </summary>
    ''' <param name="strKanriNo">管理No</param>
    ''' <param name="nLstGid">表示対象外グレードEdasysIDリスト</param>
    ''' <param name="bTansho">探傷画面と同一疵チェック有無 True:チェックあり False:なし</param>
    ''' <param name="bNagate">長手反転 True:反転あり False:なし</param>
    ''' <param name="bDS">DS位置反転 True:反転(右がDS) False:なし(左がDS)</param>
    ''' <param name="bFFSnd">仮想疵検用疵のみチェック有無 True:チェックあり False:なし</param>
    ''' <param name="bCorrect">補正率使用(True:あり、False:なし)</param>
    ''' <returns>True:読み込み成功 False:読み込み失敗</returns>
    ''' <remarks></remarks>
    Private Function ReadMapInf(
      ByVal strKanriNo As String,
      ByVal nLstTid As List(Of Integer),
      ByVal nLstGid As List(Of Integer),
      ByVal bTansho As Boolean,
      ByVal bNagate As Boolean,
      ByVal bDS As Boolean,
      ByVal bFFSnd As Boolean,
      ByVal bCorrect As Boolean) As Boolean

        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Dim bRet As Boolean = False

        Try
            '' フレーム情報を読み込む
            'ReadFrameInf(strKanriNo)

            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If Not bDbRet Then                                          ' DB接続失敗
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ情報DB接続失敗"))
                Exit Try
            End If

            Dim key As LIST_KEY = Nothing
            Dim val As TYP_MAP_LIST_VALUE = Nothing

            Dim strTable() As String = {DB_DEFECT_INF_T, DB_DEFECT_INF_B}           ' 疵テーブル一覧
            Dim nCount As Integer = 0                                               ' 取得情報数
            For ii As Integer = 0 To strTable.Length - 1
                '---------------------------------------------------------------
                ' ＳＱＬ作成
                '---------------------------------------------------------------
                Dim strSQL As String = ""
                strSQL &= "SELECT"
                strSQL &= " d.* "
                strSQL &= " ,INF.材料板幅 as コイル幅"
                strSQL &= " FROM"
                strSQL &= " " & strTable(ii)
                strSQL &= " d "

                '' 入側コイル
                strSQL &= "INNER JOIN " & DB_COIL_INF & " INF on INF.管理No=d.管理No "

                If bFFSnd Then
                    strSQL &= " ," & DB_ACCESS_NEXT_DEFECT_INF & " adf "
                End If

                strSQL &= " WHERE"
                strSQL &= " d.管理No ='" & strKanriNo & "'"
                strSQL &= " AND d.重複結果 <> 1 "

                If bFFSnd Then
                    strSQL &= "AND ((adf.管理No = d.管理No) AND (adf.疵No=d.疵No)) "
                ElseIf bTansho Then
                    strSQL &= " AND d.表示対象区分_探傷画面 = 1 "
                Else
                    For jj As Integer = 0 To nLstTid.Count - 1                      ' 疵種設定
                        strSQL &= " AND NOT d.判定疵種 = " & CStr(nLstTid(jj))
                    Next

                    For jj As Integer = 0 To nLstGid.Count - 1                      ' グレード設定
                        strSQL &= " AND NOT d.判定グレード = " & CStr(nLstGid(jj))
                    Next
                End If

                '---------------------------------------------------------------
                ' ORDER 句 (疵面積、長手位置を後勝ちにする)
                '---------------------------------------------------------------
                strSQL &= " ORDER BY d.疵面積 ASC,"                             ' 小さいものから順に格納していく
                If bNagate = False Then                                         ' 長手方向反転無し
                    strSQL &= " d.長手位置 DESC"                                ' 尾端位置から順に格納していく

                Else                                                            ' 長手方向反転有り
                    strSQL &= " d.長手位置 ASC"                                 ' 先端位置から順に格納していく
                End If
                strSQL &= " ,d.疵No DESC"                                       ' 先端位置から順に格納していく

                '---------------------------------------------------------------
                ' 実行
                '---------------------------------------------------------------
                read = clsDB.SelectExecute(strSQL)
                If read Is Nothing Then
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ情報取得SQL実行失敗 SQL={0}", strSQL))
                    Exit Try
                End If

                While read.Read() = True
                    If Not m_bEnaRead Then
                        ' 読み込みキャンセル
                        Return False
                    End If

                    Dim nMen As Integer = 0
                    Dim nKizuNo As Integer = 0
                    Dim nTid As Integer = 0
                    Dim nGid As Integer = 0
                    Dim nLen As Integer = 0
                    Dim nAria As Integer = 0
                    Dim nDSWS As Integer = 0
                    Dim nDS As Single = 0
                    Dim nWS As Single = 0
                    Dim typDefDt As DETECT_BASE_DATA = Nothing
                    typDefDt.initialize()

                    '-----------------------------------------------------------
                    ' 読み込みデータ取得
                    '-----------------------------------------------------------
                    With read
                        typDefDt.TorB = CUShort(.GetInt32(.GetOrdinal("表裏区分")))
                        typDefDt.nTid = CUShort(.GetInt32(.GetOrdinal("判定疵種")))
                        typDefDt.nGid = CUShort(.GetInt32(.GetOrdinal("判定グレード")))
                        typDefDt.cycle_div = CUShort(.GetInt32(.GetOrdinal("周期フラグ")))
                        typDefDt.dsws = CUShort(.GetInt32(.GetOrdinal("DSWS区分")))
                        typDefDt.camset = CUShort(.GetInt32(.GetOrdinal("カメラセット")))
                        typDefDt.nKizuNo = .GetInt32(.GetOrdinal("疵No"))
                        typDefDt.y = CSng(.GetInt32(.GetOrdinal("長手位置")))
                        typDefDt.nWidDiv = CShort(.GetInt32(.GetOrdinal("幅分割区分")))
                        typDefDt.nWidAbsPos = .GetInt32(.GetOrdinal("幅方向絶対位置"))

                        If bCorrect Then
                            typDefDt.y = CInt(typDefDt.y * m_typCoilInf.dCorrect)
                        End If


                        If bNagate Then
                            ' 長手方向反転のため、Y位置をコイル長からの差分とする
                            typDefDt.y = CoilLength - typDefDt.y
                            If typDefDt.y < 0 Then
                                typDefDt.y = 0
                            End If
                        End If

						'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
						typDefDt.y_out = GetOutLen(typDefDt.y, bNagate, CoilLength)
						'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

						If bDS Then
                            typDefDt.ds = .GetFloat(.GetOrdinal("WS位置"))
                            typDefDt.ws = .GetFloat(.GetOrdinal("DS位置"))
                            If 0 = typDefDt.dsws Then
                                typDefDt.dsws = 1
                            Else
                                typDefDt.dsws = 0
                            End If
                            typDefDt.nWidDiv = CShort((MAP_COL_NUM - 1) - typDefDt.nWidDiv)
                        Else
                            typDefDt.ds = .GetFloat(.GetOrdinal("DS位置"))
                            typDefDt.ws = .GetFloat(.GetOrdinal("WS位置"))
                        End If

                        ' センターからの距離を算出
                        Dim nWid As Integer = .GetInt32(.GetOrdinal("コイル幅"))
                        'Dim nXmax As Integer = CInt(nWid / 2)
                        'Dim nXmin As Integer = nXmax * -1

                        typDefDt.dsc = CSng(nWid) / CSng(2.0) - typDefDt.ds
                        typDefDt.wsc = CSng(nWid) / CSng(2.0) - typDefDt.ws
                        'If 0 = typDefDt.dsws Then
                        '    ' DS基準
                        '    typDefDt.dsc = nXmax - typDefDt.ds
                        '    typDefDt.wsc = nXmin + typDefDt.ds
                        'Else
                        '    ' WS基準
                        '    typDefDt.wsc = nXmax - typDefDt.ws
                        '    typDefDt.dsc = nXmin + typDefDt.ws
                        'End If

                        typDefDt.speed = .GetInt32(.GetOrdinal("ライン速度"))
                        typDefDt.pitch = .GetInt32(.GetOrdinal("ピッチ"))
                        typDefDt.roolkei = .GetFloat(.GetOrdinal("径"))

                        typDefDt.aria = .GetFloat(.GetOrdinal("疵面積"))
                        typDefDt.kizu_wid = .GetInt32(.GetOrdinal("疵幅"))
                        typDefDt.kizu_len = .GetInt32(.GetOrdinal("疵長さ"))

                        typDefDt.h_res = .GetFloat(.GetOrdinal("縦分解能"))
                        typDefDt.w_res = .GetFloat(.GetOrdinal("横分解能"))
                        If 0.0 >= typDefDt.h_res Then typDefDt.h_res = 0.1
                        If 0.0 >= typDefDt.w_res Then typDefDt.w_res = 0.1

                        '' フレームNoの決定は、事前に読み込んだフレーム情報を使用する
                        'typDefDt.nFrameNo = GetFrameImage(.GetString(.GetOrdinal("管理No")), .GetInt32(.GetOrdinal("フレームNo")), .GetInt32(.GetOrdinal("カメラセット")))

                        typDefDt.nFrameNo = .GetInt32(.GetOrdinal("フレームNo"))

                        typDefDt.frm_x_min = CShort(.GetInt32(.GetOrdinal("フレーム外接Xmin")))
                        typDefDt.frm_x_max = CShort(.GetInt32(.GetOrdinal("フレーム外接Xmax")))
                        typDefDt.frm_y_min = CShort(.GetInt32(.GetOrdinal("フレーム外接Ymin")))
                        typDefDt.frm_y_max = CShort(.GetInt32(.GetOrdinal("フレーム外接Ymax")))

                        typDefDt.cull_frm_x_min = CShort(.GetInt32(.GetOrdinal("間引きフレーム外接Xmin")))
                        typDefDt.cull_frm_x_max = CShort(.GetInt32(.GetOrdinal("間引きフレーム外接Xmax")))
                        typDefDt.cull_frm_y_min = CShort(.GetInt32(.GetOrdinal("間引きフレーム外接Ymin")))
                        typDefDt.cull_frm_y_max = CShort(.GetInt32(.GetOrdinal("間引きフレーム外接Ymax")))

                        typDefDt.box_x_min = CShort(.GetInt32(.GetOrdinal("外接Xmin")))
                        typDefDt.box_x_max = CShort(.GetInt32(.GetOrdinal("外接Xmax")))
                        typDefDt.box_y_min = CShort(.GetInt32(.GetOrdinal("外接Ymin")))
                        typDefDt.box_y_max = CShort(.GetInt32(.GetOrdinal("外接Ymax")))

                        typDefDt.center_x = CShort(.GetInt32(.GetOrdinal("疵中心位置X")))
                        typDefDt.center_y = CShort(.GetInt32(.GetOrdinal("疵中心位置Y")))
                        typDefDt.h_size = CUShort(.GetInt32(.GetOrdinal("縦画素数")))
                        typDefDt.w_size = CUShort(.GetInt32(.GetOrdinal("横画素数")))

                        typDefDt.nTyuu = CUShort(mcls_Param.GetTypePriority(typDefDt.nTid))
                        typDefDt.nGyuu = CUShort(mcls_Param.GetGradePriority(typDefDt.nGid))


                        typDefDt.base_box_x_min = CShort(.GetInt32(.GetOrdinal("元画像外接Xmin")))
                        typDefDt.base_box_x_max = CShort(.GetInt32(.GetOrdinal("元画像外接Xmax")))
                        typDefDt.base_box_y_min = CShort(.GetInt32(.GetOrdinal("元画像外接Ymin")))
                        typDefDt.base_box_y_max = CShort(.GetInt32(.GetOrdinal("元画像外接Ymax")))

                        typDefDt.base_center_x = CShort(.GetInt32(.GetOrdinal("元画像疵中心位置X")))
                        typDefDt.base_center_y = CShort(.GetInt32(.GetOrdinal("元画像疵中心位置Y")))
                        typDefDt.base_h_size = CUShort(.GetInt32(.GetOrdinal("元画像縦画素数")))
                        typDefDt.base_w_size = CUShort(.GetInt32(.GetOrdinal("元画像横画素数")))

                        typDefDt.base_h_res = .GetFloat(.GetOrdinal("元縦分解能"))
                        typDefDt.base_w_res = .GetFloat(.GetOrdinal("元横分解能"))
                        If 0.0 >= typDefDt.base_h_res Then typDefDt.base_h_res = 0.1
                        If 0.0 >= typDefDt.base_w_res Then typDefDt.base_w_res = 0.1
                        typDefDt.nShrinkRatio = CUShort(.GetInt32(.GetOrdinal("縮小率")))

                        typDefDt.nObjTansho = CShort(.GetInt32(.GetOrdinal("表示対象区分_探傷画面")))

                        nKizuNo = .GetInt32(.GetOrdinal("疵No"))

                        nMen = typDefDt.TorB
                        nLen = CInt(typDefDt.y)
                        nAria = CInt(typDefDt.aria)
                        nTid = typDefDt.nTid
                        nGid = typDefDt.nGid
                        nDSWS = typDefDt.dsws
                        nDS = typDefDt.ds
                        nWS = typDefDt.ws
                    End With

                    If nKizuNo < 0 Then ' 疵No不正ならば次へ
                        Continue While
                    End If

                    ' 詳細疵情報配列セット
                    If m_dctDefInf(ii).ContainsKey(New DEFECT_KEY(nKizuNo)) = False Then
                        m_dctDefInf(ii).Add(New DEFECT_KEY(nKizuNo), typDefDt)
                    Else
                        mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("詳細疵情報配列セット異常 疵No重複 [疵No={0} 面={1}]", nKizuNo, ii))
                    End If

                    '-----------------------------------------------------------
                    ' 列番号の決定
                    '-----------------------------------------------------------
                    Dim nCol As Integer = 0
                    nCol = typDefDt.nWidDiv

                    If m_nColStart > nCol Then                                  ' コイル幅を超えることもある
                        nCol = m_nColStart
                    ElseIf nCol > m_nColEnd Then                                ' コイル幅を超えることもある
                        nCol = m_nColEnd
                    End If

                    '-----------------------------------------------------------
                    ' 表示間隔毎にデータを格納する
                    '-----------------------------------------------------------
                    For stage As Integer = 0 To m_stMap.stage.Length - 1       ' 各ステージに設定
                        Dim nRow As Integer = 0
                        nRow = (nLen \ 1000) \ Interval(CType(stage, EM_LONG_POS))

                        If nRow < 0 Then                                        ' 行が配列長を超えた場合
                            nRow = 0
                        ElseIf nRow > m_stMap.stage(stage).men(nMen).row.Length - 1 Then
                            nRow = m_stMap.stage(stage).men(nMen).row.Length - 1
                        End If


                        '通常疵
                        With m_stMap.stage(stage).men(nMen).row(nRow).col(nCol)
                            If .nKizuNo > 0 Then                                    ' 既に疵が設定されている場合は代表疵判定をする
                                ' 取得時に面積と長手位置の比較済のため、疵種、グレードの優先度のみ
                                Dim cmp As Integer = mcls_Param.CompDefPriority(.nTid, nTid, .nGid, nGid)
                                If cmp >= 0 Then                                ' 後勝ちに設定する
                                    .strKanriNo = strKanriNo
                                    .nKizuNo = nKizuNo
                                    .nTid = CShort(nTid)
                                    .nGid = CShort(nGid)
                                End If
                            Else                                                ' 初回登録時はそのまま設定
                                .strKanriNo = strKanriNo
                                .nKizuNo = nKizuNo
                                .nTid = CShort(nTid)
                                .nGid = CShort(nGid)
                            End If
                        End With

                        With key
                            .nCol = nCol
                            .nRow = nRow
                            .nMen = nMen
                            .emStage = CType(stage, EM_LONG_POS)
                        End With
                        With val
                            .nPnt = .nKizuNo
                            .nKizuNo = nKizuNo
                            .nGid = nGid
                            .nTid = nTid
                            .nTYuu = typDefDt.nTyuu
                            .nGYuu = typDefDt.nGyuu
                            .nAria = nAria
                            .nY = nLen
                            .nCycDiv = typDefDt.cycle_div
                        End With
                        AddDefLst(key, val)                                     ' 疵リストデータに設定する

                        '-----------------------------------
                        Dim kizuNokey As KIZU_KEY = Nothing
                        With kizuNokey
                            .nKizuNo = nKizuNo
                            .nStage = stage
                        End With
                        If m_dctDefKey.ContainsKey(kizuNokey) = False Then
                            m_dctDefKey.Add(kizuNokey, key)
                        Else
                            mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("疵情報テーブル読み込み異常(通常) 疵No重複 [疵No={0} MAP間隔区分={1}]", kizuNokey.nKizuNo, kizuNokey.nStage))
                        End If

                        '-----------------------------------
                        m_stMap.stage(stage).men(ii).row(nRow).nKizuCnt += 1

                    Next

                    nCount += 1
                    If nCount Mod 100 = 0 Then
                        RaiseEvent ReadDataNti(DATA_KIND.emCOIL_INF, nCount)            ' 読み込み件数通知
                        Application.DoEvents()
                    End If

                End While

                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If

            Next

            ' 読み込み成功なので、戻り値にTrueをセット
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報テーブル読み込み異常 [{0}]", ex.Message))
        Finally
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' マップ情報読み込み
    ''' </summary>
    ''' <param name="strKanriNo">管理No</param>
    ''' <param name="bNagate">長手反転 True:反転あり False:なし</param>
    ''' <param name="bDS">DS位置反転 True:反転(右がDS) False:なし(左がDS)</param>
    ''' <returns>True:読み込み成功 False:読み込み失敗</returns>
    ''' <remarks></remarks>
    Private Function ReadPreMapInf(
      ByVal strKanriNo As String,
      ByVal bNagate As Boolean,
      ByVal bDS As Boolean) As Boolean

        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Dim bRet As Boolean = False

        Try

            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If Not bDbRet Then                                          ' DB接続失敗
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("TCMマップ情報DB接続失敗"))
                Exit Try
            End If

            Dim key As LIST_KEY = Nothing
            Dim val As TYP_MAP_LIST_VALUE = Nothing

            Dim nCount As Integer = 0                                               ' 取得情報数
            For ii As Integer = 0 To m_dctDefTcmInf.Length - 1
                '---------------------------------------------------------------
                ' ＳＱＬ作成
                '---------------------------------------------------------------
                Dim strSQL As String = ""
                strSQL &= "SELECT"
                strSQL &= " ff.管理No"
                strSQL &= ", ff.疵No"
                strSQL &= ", ff.工程種別"
                strSQL &= ", ff.表裏区分"
                strSQL &= ", ff.事前編集フラグ"
                strSQL &= ", ff.前工程管理No"
                strSQL &= ", ff.前工程疵No"
                strSQL &= ", ff.FF疵種"
                strSQL &= ", ff.FFグレード"
                strSQL &= ", ff.FF疵検出工程"
                strSQL &= ", ff.FF疵区分"
                strSQL &= ", ff.FF疵長さ方向位置"
                strSQL &= ", ff.FF疵巾方向位置"
                strSQL &= ", ff.FF疵長さ"
                strSQL &= ", ff.判定疵種"
                strSQL &= ", ff.判定グレード"
                strSQL &= ", ff.最終長手位置"

                'strSQL &= ", def.管理No"
                'strSQL &= ", def.疵No"
                'strSQL &= ", def.工程種別"
                'strSQL &= ", def.表裏区分"
                strSQL &= ", def.周期フラグ"
                strSQL &= ", def.長手位置"
                strSQL &= ", def.DSWS区分"
                strSQL &= ", def.DS位置"
                strSQL &= ", def.WS位置"
                strSQL &= ", def.ライン速度"
                strSQL &= ", def.ピッチ"
                strSQL &= ", def.径"
                strSQL &= ", def.疵幅"
                strSQL &= ", def.疵長さ"
                strSQL &= ", def.縦分解能"
                strSQL &= ", def.横分解能"
                strSQL &= ", def.外接Xmin"
                strSQL &= ", def.外接Xmax"
                strSQL &= ", def.外接Ymin"
                strSQL &= ", def.外接Ymax"
                strSQL &= ", def.疵中心位置X"
                strSQL &= ", def.疵中心位置Y"
                strSQL &= ", def.縦画素数"
                strSQL &= ", def.横画素数"
                strSQL &= ", def.画像サイズ1"
                strSQL &= ", def.画像サイズ2"
                strSQL &= ", def.画像サイズ3"
                strSQL &= ", def.画像サイズ4"

                'strSQL &= ", lin.管理No"
                'strSQL &= ", lin.工程種別"
                'strSQL &= ", lin.表裏区分"
                strSQL &= ", lin.処理ステータス"

                'strSQL &= ", mst.前工程コード"
                strSQL &= ", mst.有効判定"
                strSQL &= ", mst.前工程表示モード"
                strSQL &= ", mst.画像形式"


                strSQL &= " FROM "
                strSQL &= DB_MAE_FF_INF '// 前工程ＦＦ疵情報[ff]
                strSQL &= " ff"

                strSQL &= " INNER JOIN "
                strSQL &= DB_MAE_DEFECT_INF '// 前工程疵情報[def]
                strSQL &= " def"
                strSQL &= " ON"
                strSQL &= " ff.管理No=def.管理No"
                strSQL &= " AND ff.疵No=def.疵No"
                strSQL &= " AND ff.工程種別=def.工程種別"
                strSQL &= " AND ff.表裏区分=def.表裏区分"

                strSQL &= " INNER JOIN "
                strSQL &= DB_MAE_COIL_LINE '// 前工程ライン別コイル情報[lin]
                strSQL &= " lin"
                strSQL &= " ON"
                strSQL &= " ff.管理No=lin.管理No"
                strSQL &= " AND ff.工程種別=lin.工程種別"
                strSQL &= " AND ff.表裏区分=lin.表裏区分"

                strSQL &= " INNER JOIN "
                strSQL &= DB_MAE_MASTER '// 前工程マスター[mst]
                strSQL &= " mst"
                strSQL &= " ON"
                strSQL &= " ff.FF疵検出工程=mst.前工程コード"
                strSQL &= " WHERE"
                strSQL &= " ff.管理No="
                strSQL &= "'" & strKanriNo & "'"
                        strSQL &= " AND"
                strSQL &= " ff.表裏区分="
                strSQL &= ii
                strSQL &= " ORDER BY"
                strSQL &= " ff.工程種別 ASC, ff.疵No ASC"

                '---------------------------------------------------------------
                ' 実行
                '---------------------------------------------------------------
                read = clsDB.SelectExecute(strSQL)
                If read Is Nothing Then
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップ情報取得SQL実行失敗 SQL={0}", strSQL))
                    Exit Try
                End If

                While read.Read() = True
                    If Not m_bEnaRead Then
                        ' 読み込みキャンセル
                        Return False
                    End If


                    Dim nMen As Integer = 0
                    Dim nKizuNo As Integer = 0
                    Dim nTid As Integer = 0
                    Dim nGid As Integer = 0
                    Dim nLen As Integer = 0
                    Dim nAria As Integer = 0
                    Dim strProc As String = ""
                    Dim strTid As String = ""
                    Dim strGid As String = ""


                    Dim typDefDt As TYP_MAE_DEFECT_DATA = Nothing
                    typDefDt.initialize()

                    '-----------------------------------------------------------
                    ' 読み込みデータ取得
                    '-----------------------------------------------------------
                    With read


                        Dim nCheck As Integer = .GetInt32(.GetOrdinal("有効判定"))
                        Dim nMaeDsp As Integer = .GetInt32(.GetOrdinal("前工程表示モード"))
                        Dim nStatus As Integer = .GetInt32(.GetOrdinal("処理ステータス"))

                        If 1 <> nCheck Then Continue While
                        If 1 <> nMaeDsp And 2 <> nMaeDsp Then Continue While
                        If 0 <> nStatus Then Continue While

                        typDefDt.ff.cMaeKizukenNo = tMod_Common.StringToByte(.GetString(.GetOrdinal("前工程管理No")))
                        typDefDt.ff.nMaeKizuNo = .GetInt32(.GetOrdinal("前工程疵No"))
                        typDefDt.ff.cT = tMod_Common.StringToByte(.GetString(.GetOrdinal("FF疵種")))
                        strTid = .GetString(.GetOrdinal("FF疵種"))
                        typDefDt.ff.cG = tMod_Common.StringToByte(.GetString(.GetOrdinal("FFグレード")))
                        strGid = .GetString(.GetOrdinal("FFグレード"))
                        typDefDt.ff.cProc = tMod_Common.StringToByte(.GetString(.GetOrdinal("FF疵検出工程")))
                        strProc = .GetString(.GetOrdinal("FF疵検出工程"))
                        typDefDt.ff.nDiv = .GetInt32(.GetOrdinal("FF疵区分"))
                        typDefDt.ff.nPos = .GetInt32(.GetOrdinal("FF疵長さ方向位置"))
                        typDefDt.ff.nWidth = .GetInt32(.GetOrdinal("FF疵巾方向位置"))
                        typDefDt.ff.nLen = .GetInt32(.GetOrdinal("FF疵長さ"))
                        typDefDt.ff.nY = .GetInt32(.GetOrdinal("最終長手位置"))
                        typDefDt.ff.nTid = CShort(.GetInt32(.GetOrdinal("判定疵種")))
                        typDefDt.ff.nGid = CShort(.GetInt32(.GetOrdinal("判定グレード")))

                        typDefDt.data.nImageFlg = CShort(.GetInt32(.GetOrdinal("画像形式")))
                        typDefDt.data.nDspMode = CShort(.GetInt32(.GetOrdinal("事前編集フラグ")))
                        typDefDt.data.men = CUShort(.GetInt32(.GetOrdinal("表裏区分")))
                        typDefDt.data.cycle_div = CUShort(.GetInt32(.GetOrdinal("周期フラグ")))
                        typDefDt.data.dsws = CUShort(.GetInt32(.GetOrdinal("DSWS区分")))

                        typDefDt.data.speed = CUShort(.GetInt32(.GetOrdinal("ライン速度")))
                        typDefDt.data.pitch = CUShort(.GetInt32(.GetOrdinal("ピッチ")))
                        typDefDt.data.roolkei = .GetFloat(.GetOrdinal("径"))

                        typDefDt.data.box_wid = CShort(.GetInt32(.GetOrdinal("疵幅")))
                        typDefDt.data.box_len = CShort(.GetInt32(.GetOrdinal("疵長さ")))


                        typDefDt.data.h_res = .GetFloat(.GetOrdinal("縦分解能"))
                        typDefDt.data.w_res = .GetFloat(.GetOrdinal("横分解能"))

                        typDefDt.data.box_x_min = CShort(.GetInt32(.GetOrdinal("外接Xmin")))
                        typDefDt.data.box_x_max = CShort(.GetInt32(.GetOrdinal("外接Xmax")))
                        typDefDt.data.box_y_min = CShort(.GetInt32(.GetOrdinal("外接Ymin")))
                        typDefDt.data.box_y_max = CShort(.GetInt32(.GetOrdinal("外接Ymax")))

                        typDefDt.data.center_x = CShort(.GetInt32(.GetOrdinal("疵中心位置X")))
                        typDefDt.data.center_y = CShort(.GetInt32(.GetOrdinal("疵中心位置Y")))

                        typDefDt.data.h_size = CUShort(.GetInt32(.GetOrdinal("縦画素数")))
                        typDefDt.data.w_size = CUShort(.GetInt32(.GetOrdinal("横画素数")))

                        typDefDt.strKanriNo = strKanriNo
                        typDefDt.nKizuNo = .GetInt32(.GetOrdinal("疵No"))
                        nKizuNo = typDefDt.nKizuNo

                        ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
                        typDefDt.ff.nKouteiID = .GetInt32(.GetOrdinal("工程種別"))
                        ' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

                        If bNagate Then
                            ' 長手方向反転のため、Y位置をコイル長からの差分とする
                            typDefDt.ff.nY = CoilLength - typDefDt.ff.nY
                            If typDefDt.ff.nY < 0 Then
                                typDefDt.ff.nY = 0
                            End If
                        End If

						'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
						typDefDt.ff.nY_Out = CInt(GetOutLen(CInt(typDefDt.ff.nY), bNagate, CoilLength))
						'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

						nMen = typDefDt.data.men
                        nLen = typDefDt.ff.nY
                        nAria = 0
                        nTid = typDefDt.ff.nTid
                        nGid = typDefDt.ff.nGid


                        ' センターからの距離を算出
                    End With

                    If nKizuNo < 0 Then ' 疵No不正ならば次へ
                        Continue While
                    End If

                    ' 詳細疵情報配列セット
                    If m_dctDefTcmInf(ii).ContainsKey(New DEFECT_KEY(nKizuNo)) = False Then
                        m_dctDefTcmInf(ii).Add(New DEFECT_KEY(nKizuNo), typDefDt)
                    Else
                        mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("詳細疵情報配列セット異常(TCM) 疵No重複 [疵No={0} 面={1}]", nKizuNo, ii))
                    End If

                    '-----------------------------------------------------------
                    ' 列番号の決定
                    '-----------------------------------------------------------
                    Dim nCol As Integer = 0

                    '-----------------------------------------------------------
                    ' 表示間隔毎にデータを格納する
                    '-----------------------------------------------------------
                    For stage As Integer = 0 To m_stMap.stage.Length - 1       ' 各ステージに設定
                        Dim nRow As Integer = 0
                        nRow = (nLen \ 1000) \ Interval(CType(stage, EM_LONG_POS))

                        If nRow < 0 Then                                        ' 行が配列長を超えた場合
                            nRow = 0
                        ElseIf nRow > m_stMap.stage(stage).men(nMen).row.Length - 1 Then
                            nRow = m_stMap.stage(stage).men(nMen).row.Length - 1
                        End If

                        '周期疵
                        With m_stMap.stage(stage).men(nMen).row(nRow).tcm(0)
                            If .nKizuNo > 0 Then                                    ' 既に疵が設定されている場合は代表疵判定をする
                                ' 取得時に面積と長手位置の比較済のため、疵種、グレードの優先度のみ
                                Dim bcmp As Boolean = mcls_PreParam.CheckPriority(.strProc, strProc, .strFFType, strTid, .strFFGrade, strGid)
                                If bcmp Then                                ' 後勝ちに設定する
                                    .strKanriNo = strKanriNo
                                    .nKizuNo = nKizuNo
                                    .strProc = strProc
                                    .strFFType = strTid
                                    .strFFGrade = strGid
                                End If
                            Else                                                ' 初回登録時はそのまま設定
                                .strKanriNo = strKanriNo
                                .nKizuNo = nKizuNo
                                .strProc = strProc
                                .strFFType = strTid
                                .strFFGrade = strGid
                            End If
                        End With

                        With key
                            .nCol = 0
                            .nRow = nRow
                            .nMen = nMen
                            .emStage = CType(stage, EM_LONG_POS)
                        End With
                        With val
                            .nPnt = .nKizuNo
                            .nKizuNo = nKizuNo
                            .nGid = nGid
                            .nTid = nTid
                            .nAria = nAria
                            .nY = nLen
                            .strFFType = strTid
                            .strFFGrade = strGid
                            .strProc = strProc
                            .nTYuu = mcls_PreParam.GetTypePriority(strProc, strTid)
                            .nGYuu = mcls_PreParam.GetGradePriority(strProc, strGid)
                            .nCycDiv = 0
                            ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
                            .nKouteiID = typDefDt.ff.nKouteiID
                            ' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

                        End With
                        AddDefTcmLst(key, val)                                     ' 疵リストデータに設定する

                        '-----------------------------------
                        Dim kizuNokey As KIZU_KEY = Nothing
                        With kizuNokey
                            .nKizuNo = nKizuNo
                            .nStage = stage
                        End With
                        If m_dctDefTcmKey.ContainsKey(kizuNokey) = False Then
                            m_dctDefTcmKey.Add(kizuNokey, key)
                        Else
                            mcls_Log.Write(tClass_LogManager.LNO.WAR, String.Format("疵情報テーブル読み込み異常(TCM) 疵No重複 [疵No={0} MAP間隔区分={1}]", kizuNokey.nKizuNo, kizuNokey.nStage))
                        End If
                        '周期疵は疵個数にカウントしない。
                        '-----------------------------------
                    Next

                    nCount += 1
                    RaiseEvent ReadDataNti(DATA_KIND.emCOIL_INF, nCount)            ' 読み込み件数通知
                    Application.DoEvents()

                End While

                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If

            Next

            ' 読み込み成功なので、戻り値にTrueをセット
            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵情報テーブル読み込み異常 [{0}]", ex.Message))
        Finally
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                If Not read Is Nothing Then
                    read.Close()
                    read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' カット情報数を取得します
    ''' </summary>
    ''' <param name="strKanriNo">管理No</param>
    ''' <param name="nCount">カット情報件数</param>
    ''' <returns>True:取得成功 False:取得失敗</returns>
    ''' <remarks></remarks>
    Private Function GetCutInfCount(ByVal strKanriNo As String, ByRef nCount As Integer) As Boolean
        Try
            '-------------------------------------------------------------------
            ' ＳＱＬ作成
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " COUNT(*)"
            strSQL &= " FROM"
            strSQL &= " " & DB_COIL_CUT_POS
            strSQL &= " WHERE"
            strSQL &= " 管理No='" & strKanriNo & "'"

            '-------------------------------------------------------------------
            ' 実行
            '-------------------------------------------------------------------
            Dim bRet As Boolean = ExecuteSelectCount(strSQL, nCount)
            If bRet = False Then
                Return False
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("カット情報数取得異常 [{0}]", ex.Message))
            Return False
        End Try

        Return True
    End Function


    ''' <summary>
    ''' カット情報を読み込みます
    ''' </summary>
    ''' <param name="strKizukenNo">管理No</param>
    ''' <param name="bNagate">True:長手方向の反転あり False:反転なし</param>
    ''' <param name="bCorrect">補正率使用(True:あり、False:なし)</param>
    ''' <returns>True:取得成功 False:取得失敗</returns>
    ''' <remarks></remarks>
    Private Function ReadCutInf(ByVal strKizukenNo As String, ByVal bNagate As Boolean, ByVal bCorrect As Boolean) As Boolean
        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing

        Try
            '-------------------------------------------------------------------
            ' ＳＱＬ作成
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL &= "SELECT"
            strSQL &= " *"
            strSQL &= " FROM"
            strSQL &= " " & DB_COIL_CUT_POS
            strSQL &= " WHERE"
            strSQL &= " 管理No='" & strKizukenNo & "'"
            strSQL &= " AND 先頭位置 > 0"
            strSQL &= " ORDER BY 入側子番"

            '-------------------------------------------------------------------
            ' 実行
            '-------------------------------------------------------------------
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If bDbRet = False Then                                              ' DB接続失敗
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("カット情報DB接続失敗"))
                Return False
            End If

            read = clsDB.SelectExecute(strSQL)
            If read Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("カット情報取得SQL実行失敗 SQL={0}", strSQL))
                Return False
            End If

            '-------------------------------------------------------------------
            ' 格納
            '-------------------------------------------------------------------
            m_listCutInf.Clear()
            Dim inf As CUT_INF_DATA = Nothing
            Dim nCount As Integer = 0
            While read.Read() = True
                If m_bEnaRead = False Then                                      ' 読み込み不可の場合
                    Return False
                End If

                With read
                    inf.nKubun = .GetInt32(.GetOrdinal("区分"))
                    Dim dPosS As Integer = .GetInt32(.GetOrdinal("先頭位置"))
                    Dim nPosS As Integer

                    If bCorrect Then
                        '長手位置に補正率を加味する
                        dPosS = CInt(dPosS * m_typCoilInf.dCorrect)
                    End If

                    If bNagate Then
						' 長手反転の場合
						inf.dPosS = CSng((CoilLength - dPosS) / 1000)
						'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
						inf.dPosS_MM = CoilLength - dPosS
						'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
						nPosS = (CoilLength - dPosS) \ 1000
                    Else
						inf.dPosS = CSng(dPosS / 1000)
						'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
						inf.dPosS_MM = dPosS
						'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------
						nPosS = dPosS \ 1000
                    End If
                    m_listCutInf.Add(inf)

                    Dim nRow As Integer = nPosS \ Interval(0)
                    Dim nMaxRow As Integer = m_stMap.stage(0).men(0).row.Length - 1

                    If 0 <= nRow And nMaxRow >= nRow Then
                        For ii As Integer = 0 To m_stMap.stage(0).men.Length - 1
                            If 1 = inf.nKubun Then
                                m_stMap.stage(0).men(ii).row(nRow).nCut = 1
                            End If
                            If 4 = inf.nKubun Then
                                m_stMap.stage(0).men(ii).row(nRow).bYousetu = True
                            End If
                        Next
                    End If
                End With

                nCount += 1
                RaiseEvent ReadDataNti(DATA_KIND.emCUT_INF, nCount)             ' 読み込み件数通知
                Application.DoEvents()
            End While

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("カット情報取得異常 [{0}]", ex.Message))
            Return False
        End Try

        Return True
    End Function


    ''' <summary>
    ''' 読み込み終了処理
    ''' </summary>
    ''' <param name="bNagate">True:長手方向の反転あり False:反転なし</param>
    ''' <remarks></remarks>
    Private Sub ReadFinalize(ByVal bNagate As Boolean)
        Try
            Dim nOutTopPos As Integer = m_typCoilInf.nOutTopPos \ 1000                        ' 出側単位先端位置[m]

            For nMen As Integer = 0 To m_stMap.stage(0).men.Length - 1

                Dim nRowMax As Integer = CoilLength \ 1000 \ Interval(0)
                If nRowMax >= m_stMap.stage(0).men(nMen).row.Length Then
                    nRowMax = m_stMap.stage(0).men(nMen).row.Length - 1
                End If

                For nRow As Integer = 0 To nRowMax
                    For nStage As Integer = 1 To MAP_STAGE_NUM - 1

                        Dim nSetRow As Integer = (nRow * Interval(0)) \ Interval(CType(nStage, EM_LONG_POS))

                        '-------------------------------------------------------
                        ' マップ位置情報の全ステージ反映
                        '-------------------------------------------------------
                        Dim nKensa As EM_DIV_KENSA = m_stMap.stage(0).men(nMen).row(nRow).emKensa
                        Dim nKiki As EM_DIV_KIKI = m_stMap.stage(0).men(nMen).row(nRow).emKiki
                        Dim bYosetsu As Boolean = m_stMap.stage(0).men(nMen).row(nRow).bYousetu
                        Dim nCut As Integer = m_stMap.stage(0).men(nMen).row(nRow).nCut

                        With m_stMap.stage(nStage).men(nMen).row(nSetRow)
                            If nKensa = EM_DIV_KENSA.DIV_KENSA_NON Then         ' 停止(-9)の場合は問答無用
                                .emKensa = nKensa
                            ElseIf .emKensa < nKensa Then                       ' 後は数値に従う
                                If .emKensa <> EM_DIV_KENSA.DIV_KENSA_NON Then  ' 停止は除外
                                    .emKensa = nKensa
                                End If
                            End If
                            If .emKiki < nKiki Then                             ' 機器状態の更新
                                .emKiki = nKiki
                            End If
                            If Not .bYousetu And bYosetsu Then
                                .bYousetu = bYosetsu
                            End If
                            If 0 = .nCut And 0 <> nCut Then
                                .nCut = nCut
                            End If
                        End With

                    Next
                Next
                '-------------------------------------------------------
                ' 出側距離の振りなおし
                '-------------------------------------------------------
                For nStage As Integer = 1 To MAP_STAGE_NUM - 1
                    Dim nEndRow As Integer = CoilLength \ 1000 \ Interval(CType(nStage, EM_LONG_POS))
                    Dim nOffset As Integer = (nOutTopPos \ Interval(CType(nStage, EM_LONG_POS)) + 1) * Interval(CType(nStage, EM_LONG_POS))
                    Dim nCnt As Integer = 0
                    For nRow As Integer = 0 To nEndRow
                        With m_stMap.stage(nStage).men(nMen).row(nRow)
                            .nLen_o = nOffset + nCnt * Interval(CType(nStage, EM_LONG_POS))
                            nCnt = nCnt + 1
                            If 0 <> .nCut Then
                                nOffset = Interval(CType(nStage, EM_LONG_POS))
                                nCnt = 0
                            End If
                        End With
                    Next
                Next nStage


            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("マップデータ読み込み終了異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 情報数取得SQL実行
    ''' </summary>
    ''' <param name="strSQL">SQL</param>
    ''' <param name="nCount">格納先</param>
    ''' <returns>True:正常終了 False:異常終了</returns>
    ''' <remarks></remarks>
    Private Function ExecuteSelectCount(ByVal strSQL As String, ByRef nCount As Integer) As Boolean
        Dim clsDB As tClass_SQLServer = Nothing
        Dim read As SqlDataReader = Nothing
        Try
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If bDbRet = False Then                                          ' DB接続失敗
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("情報数取得DB接続失敗  SQL={0}", strSQL))
                Return False
            End If

            read = clsDB.SelectExecute(strSQL)
            If read Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("情報数取得SQL実行失敗 SQL={0}", strSQL))
                Return False
            End If

            bDbRet = read.Read()
            If bDbRet = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("情報数取得失敗 SQL={0}", strSQL))
                Return False
            End If

            nCount += CInt(read.GetSqlInt32(0))                             ' やっと取得

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("情報数取得異常 [{0}]  SQL={0}", ex.Message, strSQL))
            Return False
        Finally
            If clsDB Is Nothing = False Then
                clsDB.Cancel()
                If read Is Nothing = False Then
                    read.Close()
                    read = Nothing
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return True
    End Function

    ''' <summary>
    ''' 疵リストへ疵を追加します
    ''' </summary>
    ''' <remarks>疵種、疵グレードが同じ場合は、後勝ち順に挿入します</remarks>
    Private Sub AddDefLst(ByVal key As LIST_KEY, ByVal val As TYP_MAP_LIST_VALUE)
        Try
            Dim list As List(Of TYP_MAP_LIST_VALUE)

            If m_dctDefLst.ContainsKey(key) = True Then
                '---------------------------------------------------------------
                ' 既にマップに疵がある場合(ソートは後で実施するため追加登録)
                '---------------------------------------------------------------
                list = m_dctDefLst(key)
                list.Add(val)

            Else
                '---------------------------------------------------------------
                ' 最初の傷の場合
                '---------------------------------------------------------------
                list = New List(Of TYP_MAP_LIST_VALUE)
                list.Add(val)
                m_dctDefLst.Add(key, list)
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ疵リスト疵追加異常 [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' 疵マップ疵リストを取得します
    ''' </summary>
    ''' <param name="nCol">列番号 0オリジン</param>
    ''' <param name="nRow">行番号 1オリジン</param>
    ''' <param name="nMen">0:表、1:裏</param>
    ''' <param name="emStage">1:表示間隔小、2:表示間隔大</param>
    ''' <param name="list">疵マップ疵リスト</param>
    ''' <returns>True:疵リスト有り False:疵リストなし</returns>
    ''' <remarks></remarks>
    Public Function GetMapDefectList(ByVal nCol As Integer, ByVal nRow As Integer, ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByRef list As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = True

        Try
            '-------------------------------------------------------------------
            ' 検索キー作成
            '-------------------------------------------------------------------
            Dim key As LIST_KEY = Nothing                                       ' 検索キー
            key.nCol = nCol
            key.nRow = nRow
            key.emStage = emStage
            key.nMen = nMen

            bRet = GetMapDefectList(key, list)
            If m_dctDefLst.ContainsKey(key) = False Then
                Return False
            End If

            list = m_dctDefLst(key)                                             ' リスト引渡し
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ疵リスト取得1異常 [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="key"></param>
    ''' <param name="list"></param>
    ''' <returns></returns>
    Public Function GetMapDefectList(ByVal key As LIST_KEY, ByRef list As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = True

        Try
            '-------------------------------------------------------------------
            ' 検索キー作成
            '-------------------------------------------------------------------
            If m_dctDefLst.ContainsKey(key) = False Then
                Return False
            End If

            list = m_dctDefLst(key)                                             ' リスト引渡し
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ疵リスト取得2異常 [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 疵リスト(TCM)へ疵を追加します
    ''' </summary>
    ''' <remarks>疵種、疵グレードが同じ場合は、後勝ち順に挿入します</remarks>
    Private Sub AddDefTcmLst(ByVal key As LIST_KEY, ByVal val As TYP_MAP_LIST_VALUE)
        Try
            Dim list As List(Of TYP_MAP_LIST_VALUE)

            If m_dctDefTcmLst.ContainsKey(key) = True Then
                '---------------------------------------------------------------
                ' 既にマップに疵がある場合(ソートは後で実施するため追加登録)
                '---------------------------------------------------------------
                list = m_dctDefTcmLst(key)
                list.Add(val)

            Else
                '---------------------------------------------------------------
                ' 最初の傷の場合
                '---------------------------------------------------------------
                list = New List(Of TYP_MAP_LIST_VALUE)
                list.Add(val)
                m_dctDefTcmLst.Add(key, list)
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ疵リスト(TCM)疵追加異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' 疵マップ疵リスト(TCM)を取得します
    ''' </summary>
    ''' <param name="nCol">列番号 0オリジン</param>
    ''' <param name="nRow">行番号 1オリジン</param>
    ''' <param name="nMen">0:表、1:裏</param>
    ''' <param name="emStage">1:表示間隔小、2:表示間隔大</param>
    ''' <param name="list">疵マップ疵リスト</param>
    ''' <returns>True:疵リスト有り False:疵リストなし</returns>
    ''' <remarks></remarks>
    Public Function GetMapDefectTcmList(ByVal nCol As Integer, ByVal nRow As Integer, ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByRef list As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = True

        Try
            '-------------------------------------------------------------------
            ' 検索キー作成
            '-------------------------------------------------------------------
            Dim key As LIST_KEY = Nothing                                       ' 検索キー
            key.nCol = nCol
            key.nRow = nRow
            key.emStage = emStage
            key.nMen = nMen

            bRet = GetMapDefectTcmList(key, list)
            If m_dctDefTcmLst.ContainsKey(key) = False Then
                Return False
            End If

            list = m_dctDefTcmLst(key)                                             ' リスト引渡し
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ疵リスト(TCM)取得異常1 [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 
    ''' </summary>
    ''' <param name="key"></param>
    ''' <param name="list"></param>
    ''' <returns></returns>
    Public Function GetMapDefectTcmList(ByVal key As LIST_KEY, ByRef list As List(Of TYP_MAP_LIST_VALUE)) As Boolean
        Dim bRet As Boolean = True

        Try
            '-------------------------------------------------------------------
            ' 検索キー作成
            '-------------------------------------------------------------------
            If m_dctDefTcmLst.ContainsKey(key) = False Then
                Return False
            End If

            list = m_dctDefTcmLst(key)                                             ' リスト引渡し
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ疵リスト(TCM)取得異常2 [{0}]", ex.Message))
            bRet = False
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 疵情報を取得します
    ''' </summary>
    ''' <param name="nMen">0:表面 1:裏面</param>
    ''' <returns>疵情報</returns>
    ''' <remarks></remarks>
    Public Function DefectData(ByVal nMen As Integer, ByVal key As clsDataMap.DEFECT_KEY, ByRef typ As DETECT_BASE_DATA) As Boolean
        If Not m_dctDefInf(nMen).ContainsKey(key) Then
            Return False
        End If
        typ = m_dctDefInf(nMen)(key)
        Return True
    End Function

    ''' <summary>
    ''' TCM疵情報を取得します
    ''' </summary>
    ''' <param name="nMen">0:表面 1:裏面</param>
    ''' <returns>疵情報</returns>
    ''' <remarks></remarks>
    Public Function TcmDefectData(ByVal nMen As Integer, ByVal key As clsDataMap.DEFECT_KEY, ByRef typ As TYP_MAE_DEFECT_DATA) As Boolean
        If Not m_dctDefTcmInf(nMen).ContainsKey(key) Then
            Return False
        End If
        typ = m_dctDefTcmInf(nMen)(key)
        Return True
    End Function

    ''' <summary>
    ''' 先頭から順にマップデータを取得します
    ''' </summary>
    ''' <param name="row_data">マップ行情報</param>
    ''' <param name="nMen">0:表面 1:裏面</param>
    ''' <returns>True:取得成功 False:取得失敗</returns>
    ''' <remarks></remarks>
    Public Function ReadRow(ByRef row_data As G_MAP_ROW, ByVal nMen As Integer, ByVal emStage As EM_LONG_POS, ByVal nRow As Integer) As Boolean
        If GetRowCount(emStage) <= nRow Then
            Return False
        End If

        row_data = m_stMap.stage(emStage).men(nMen).row(nRow)

        Return True
    End Function


    ''' <summary>
    ''' 現在の表示間隔におけるコイルの総行数を取得します
    ''' </summary>
    ''' <param name="emStage">1:表示間隔小、2:表示間隔大</param>
    ''' <returns></returns>
    Public Function GetRowCount(ByVal emStage As EM_LONG_POS) As Integer
        Dim nCount As Integer = CInt(CoilLength \ 1000 \ Interval(emStage)) + 1
        If nCount >= m_stMap.stage(emStage).men(0).row.Length Then
            nCount = m_stMap.stage(emStage).men(0).row.Length
        End If
        Return nCount
    End Function

    ''' <summary>
    ''' 疵検索(開始行列指定)
    ''' </summary>
    ''' <param name="nColS">検索開始疵データ上の列番号 0オリジン</param>
    ''' <param name="nRowS">検索開始疵データ上の行番号 0オリジン</param>
    ''' <param name="bDirCol">検索方向 列 True:コイルデータの左から右へ False:コイルデータの右から左へ</param>
    ''' <param name="bDirRow">検索方向 行 True:コイルデータの上から下へ False:コイルデータの下から上へ</param>
    ''' <param name="nNum">検索個数</param>
    ''' <param name="lstKeyR">検索結果格納先疵リストキー 一覧</param>
    ''' <param name="lstValR">検索結果格納先疵リスト値 一覧</param>
    ''' <param name="nMen">0:表、1:裏</param>
    ''' <param name="emStage">1:表示間隔小、2:表示間隔大</param>
    ''' <returns>True:検索成功 False:検索失敗</returns>
    ''' <remarks>データ上のコイル左上から右下へ向かって検索する</remarks>
    Public Function FindDefect(
      ByVal nColS As Integer,
      ByVal nRowS As Integer,
      ByVal bDirCol As Boolean,
      ByVal bDirRow As Boolean,
      ByVal nNum As Integer,
      ByVal lstKeyR As List(Of LIST_KEY),
      ByVal lstValR As List(Of TYP_MAP_LIST_VALUE),
      ByVal nMen As Integer,
      ByVal emStage As EM_LONG_POS) As Boolean

        Dim bRet As Boolean = False

        Try
            Dim key As LIST_KEY = Nothing

            '-------------------------------------------------------------------
            ' 検索方向設定
            '-------------------------------------------------------------------
            Dim nRowE As Integer = GetRowCount(emStage) - 1
            Dim nStepRow As Integer = 1
            If Not bDirRow Then
                nRowE = 0
                nStepRow = -1
                nRowS = GetRowCount(emStage) - 1
            End If

            Dim nColE As Integer = MAP_COL_NUM - 1
            Dim nStepCol As Integer = 1
            If Not bDirCol Then
                nColE = 0
                nStepCol = -1
            End If

            '-------------------------------------------------------------------
            ' 疵があるキーを検索を検索し、疵を検索する
            '-------------------------------------------------------------------
            Dim bFindKey As Boolean = False
            For nRow As Integer = nRowS To nRowE Step nStepRow
                For nCol As Integer = nColS To nColE Step nStepCol
                    key.emStage = emStage
                    key.nMen = nMen
                    key.nCol = nCol
                    key.nRow = nRow
                    If m_dctDefLst.ContainsKey(key) = False Then
                        Continue For
                    End If
                    If 1 > m_dctDefLst(key).Count Then
                        Continue For
                    End If

                    Dim val As TYP_MAP_LIST_VALUE = m_dctDefLst(key)(0)
                    bRet = FindDefect(key, val, bDirCol, bDirRow, nNum, lstKeyR, lstValR, nMen, emStage)
                    Exit Try
                Next
                If bDirCol Then                                          ' 検索方向に合わせて、列の検索開始位置を決定する
                    nColS = 0
                Else
                    nColS = MAP_COL_NUM - 1
                End If
            Next

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ疵検索（座標指定）異常 [{0}]", ex.Message))
        End Try

        Return bRet
    End Function


    ''' <summary>
    ''' 疵検索(開始疵指定)
    ''' </summary>
    ''' <param name="stKeyS">検索開始疵のリストキー</param>
    ''' <param name="stValS">検索開始疵の値</param>
    ''' <param name="bDirCol">検索方向 列 True:データ上コイル左から右へ False:データ上コイル右から左へ</param>
    ''' <param name="bDirRow">検索方向 行 True:データ上コイル上から下へ False:データ上コイル下から上へ</param>
    ''' <param name="nNum">検索個数</param>
    ''' <param name="lstKeyR">検索結果格納先疵リストキー 一覧</param>
    ''' <param name="lstValR">検索結果格納先疵リスト値 一覧</param>
    ''' <param name="nMen">0:表、1:裏</param>
    ''' <param name="emStage">1:表示間隔小、2:表示間隔大</param>
    ''' <returns>True:検索成功 False:検索失敗</returns>
    ''' <remarks>データ上のコイル左上から右下へ向かって検索する</remarks>
    Public Function FindDefect(
     ByVal stKeyS As LIST_KEY,
     ByVal stValS As TYP_MAP_LIST_VALUE,
     ByVal bDirCol As Boolean,
     ByVal bDirRow As Boolean,
     ByVal nNum As Integer,
     ByVal lstKeyR As List(Of LIST_KEY),
     ByVal lstValR As List(Of TYP_MAP_LIST_VALUE),
     ByVal nMen As Integer,
     ByVal emStage As EM_LONG_POS) As Boolean
        Try
            Dim bRet As Boolean = False

            '格納先リストをクリア
            lstKeyR.Clear()
            lstValR.Clear()

            Dim nCnt As Integer = 0                                             ' 発見した疵カウント
            '-------------------------------------------------------------------
            ' 検索開始前条件確認
            '-------------------------------------------------------------------
            If m_dctDefLst.ContainsKey(stKeyS) = False Then
                Exit Try
            End If
            If m_dctDefLst(stKeyS).Contains(stValS) = False Then
                Exit Try
            End If

            '-------------------------------------------------------------------
            ' 同じ疵リスト内を検索
            '-------------------------------------------------------------------
            Dim nIndexS As Integer = m_dctDefLst(stKeyS).IndexOf(stValS)
            Dim nIndexE As Integer = m_dctDefLst(stKeyS).Count - 1
            Dim nStepIdx As Integer = 1
            If bDirRow = False Then
                nIndexE = 0
                nStepIdx = -1
            End If

            For idx As Integer = nIndexS To nIndexE Step nStepIdx               ' 今選択されている疵から検索する
                lstKeyR.Add(stKeyS)
                lstValR.Add(m_dctDefLst(stKeyS)(idx))
                nCnt = nCnt + 1
                If lstKeyR.Count >= nNum Then
                    bRet = True
                    Exit Try
                End If
            Next

            '-------------------------------------------------------------------
            ' 検索方向の設定
            '-------------------------------------------------------------------
            Dim nRowS As Integer = stKeyS.nRow
            Dim nRowE As Integer = GetRowCount(emStage) - 1
            Dim nStepRow As Integer = 1
            If bDirRow = False Then
                nRowE = 0
                nStepRow = -1
            End If

            Dim nColS As Integer = stKeyS.nCol + 1
            Dim nColE As Integer = MAP_COL_NUM - 1
            Dim nStepCol As Integer = 1

            If bDirCol = False Then
                nColS = stKeyS.nCol - 1
                nColE = 0
                nStepCol = -1
            End If

            '-------------------------------------------------------------------
            ' 検索開始
            '-------------------------------------------------------------------
            Dim key As LIST_KEY = Nothing
            key.emStage = emStage
            key.nMen = nMen

            For row As Integer = nRowS To nRowE Step nStepRow
                For col As Integer = nColS To nColE Step nStepCol
                    key.nCol = col
                    key.nRow = row

                    If m_dctDefLst.ContainsKey(key) = False Then                ' キーが無ければ次へ
                        Continue For
                    End If

                    If m_dctDefLst(key).Count <= 0 Then                         ' 疵が無ければ次へ
                        Continue For
                    End If

                    nIndexS = 0                                                 ' 検索するインデックス方向を設定する
                    nIndexE = m_dctDefLst(key).Count - 1
                    If bDirRow = False Then
                        nIndexS = nIndexE
                        nIndexE = 0
                    End If

                    For idx As Integer = nIndexS To nIndexE Step nStepIdx       ' 疵リストから順次格納する
                        lstKeyR.Add(key)
                        lstValR.Add(m_dctDefLst(key)(idx))
                        nCnt += 1

                        If nCnt >= nNum Then                                    ' 検索数分発見した終了する
                            Return True
                        End If
                    Next
                Next

                If bDirCol = True Then                                          ' 開始列位置の再設定
                    nColS = 0
                Else
                    nColS = MAP_COL_NUM - 1
                End If
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("疵マップ疵検索（疵指定）異常 [{0}]", ex.Message))
            Return False
        End Try

        Return True
    End Function

    ''' <summary>
    ''' グリッドの列数を算出する (奇数マップに対応)
    ''' </summary>
    ''' <param name="width">算出したい幅</param>
    ''' <param name="interval">列の間隔</param>
    ''' <param name="maxCol">最大の列数</param>
    ''' <returns>グリッド数</returns>
    ''' <remarks></remarks>
    Private Function SelectGridCol2(ByRef width As Integer, ByRef interval As Integer, ByRef maxCol As Integer) As Integer
        '// 板幅不正により、最大にする
        If 0 >= width Then
            Return maxCol
        End If

        '// 板幅による算出
        Dim nCol As Integer = 0
        Dim env As Integer = maxCol Mod 2          '// マップ画面の列数 (0:偶数列  1:奇数列)
        Dim nJustIndex As Integer = 0
        '// 初回の境界位置決定
        If 0 = env Then
            '// 偶数
            nJustIndex = CInt(Int((width - 1) / (interval * 2)))
            nCol = (nJustIndex + 1) * 2
        Else
            '// 奇数
            width -= interval
            If 0 >= width Then
                Return 1                        '// 1マス以内の場合の特殊処理
            End If

            nJustIndex = CInt(Int((width - 1) / (interval * 2)))
            nCol = (nJustIndex + 1) * 2 + 1
        End If

        '// 整合性チェック
        If maxCol < nCol Or 0 > nCol Then
            nCol = maxCol
        End If

        Return nCol

    End Function


    ''' <summary>
    ''' 疵リストをソートする
    ''' </summary>
    ''' <param name="dicMapDefect"></param>
    ''' <param name="bNagate">True:長手方向の反転あり False:反転なし</param>
    ''' <remarks></remarks>
    Private Sub Sort_Map_Kizulist(ByRef dicMapDefect As Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE)), ByVal bNagate As Boolean)
        Dim lstData As List(Of TYP_MAP_LIST_VALUE)             '疵種リスト
        Try
            For Each key As LIST_KEY In dicMapDefect.Keys
                lstData = dicMapDefect(key)
                Dim Comparer As New List_Comparer()
                lstData.Sort(Comparer)
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "疵リストソートエラー [" & ex.Message & "]")
        End Try
    End Sub


    ''' <summary>
    ''' 検索本体 (検索値を検索する)
    ''' </summary>
    ''' <remarks></remarks>
    Public Class List_Comparer
        Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE) 'IComparerインターフェイスを実装する

        ''' <summary>
        ''' 比較方法 (sortにより実装)
        ''' </summary>
        ''' <param name="x">比較対照</param>
        ''' <param name="y">比較対照</param>
        ''' <returns>0：同じ 0より小さい:Xの方が小さい 0より大きい:Xの方が大きい</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As TYP_MAP_LIST_VALUE, ByVal y As TYP_MAP_LIST_VALUE) As Integer _
         Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE).Compare

            ' 優先順位
            ' ① グレード優先度
            ' ② 疵種優先度
            ' ③ 面積順
            ' ④ 長手順
            ' ⑤ 疵No
            Dim dNow(5) As Double
            Dim dPer(5) As Double
            Dim bMode(5) As Boolean


            dNow(0) = CDbl(x.nGYuu)
            dPer(0) = CDbl(y.nGYuu)
            bMode(0) = False

            dNow(1) = CDbl(x.nTYuu)
            dPer(1) = CDbl(y.nTYuu)
            bMode(1) = False

            dNow(2) = CDbl(x.nAria)
            dPer(2) = CDbl(y.nAria)
            bMode(2) = True

            dNow(3) = CDbl(x.nY)
            dPer(3) = CDbl(y.nY)
            bMode(3) = False

            dNow(4) = CDbl(x.nKizuNo)
            dPer(4) = CDbl(y.nKizuNo)
            bMode(4) = False

            If dNow(0) = dPer(0) And dNow(1) = dPer(1) And dNow(2) = dPer(2) And dNow(3) = dPer(3) And dNow(4) = dPer(4) Then
                Return 0
            End If
            ' 優先度比較
            If Not SelectPriorityDetect(dNow, dPer, bMode) Then
                Return 1
            Else
                Return -1
            End If
        End Function
    End Class

    ''' <summary>
    ''' 検索本体 (検索値を検索する) 長手方向反転用
    ''' </summary>
    ''' <remarks></remarks>
    Public Class List_Comparer_Reverse
        Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE) 'IComparerインターフェイスを実装する

        ''' <summary>
        ''' 比較方法 (sortにより実装)
        ''' </summary>
        ''' <param name="x">比較対照</param>
        ''' <param name="y">比較対照</param>
        ''' <returns>0：同じ 0より小さい:Xの方が小さい 0より大きい:Xの方が大きい</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As TYP_MAP_LIST_VALUE, ByVal y As TYP_MAP_LIST_VALUE) As Integer _
         Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE).Compare

            ' 優先順位
            ' ① グレード優先度
            ' ② 疵種優先度
            ' ③ 面積順
            ' ④ 長手順
            ' ⑤ 疵No
            Dim dNow(5) As Double
            Dim dPer(5) As Double
            Dim bMode(5) As Boolean


            dNow(0) = CDbl(x.nGYuu)
            dPer(0) = CDbl(y.nGYuu)
            bMode(0) = False

            dNow(1) = CDbl(x.nTYuu)
            dPer(1) = CDbl(y.nTYuu)
            bMode(1) = False

            dNow(2) = CDbl(x.nAria)
            dPer(2) = CDbl(y.nAria)
            bMode(2) = True

            dNow(3) = CDbl(x.nY)
            dPer(3) = CDbl(y.nY)
            bMode(3) = True

            dNow(4) = CDbl(x.nKizuNo)
            dPer(4) = CDbl(y.nKizuNo)
            bMode(4) = False

            If dNow(0) = dPer(0) And dNow(1) = dPer(1) And dNow(2) = dPer(2) And dNow(3) = dPer(3) And dNow(4) = dPer(4) Then
                Return 0
            End If
            ' 優先度比較
            If Not SelectPriorityDetect(dNow, dPer, bMode) Then
                Return 1
            Else
                Return -1
            End If
        End Function
    End Class

    ''' <summary>
    ''' 疵情報を読み込みます
    ''' </summary>
    ''' <param name="strKanriNo">管理No</param>
    ''' <param name="nKizuNo">疵No</param>
    ''' <returns>True:読み込み成功 False:読み込み失敗</returns>
    ''' <remarks></remarks>
    Public Function GetTcmImage(ByVal strKanriNo As String, ByVal nKizuNo As Integer, ByVal typData As DSP_TYP_MAE_DEFECTINF, ByVal nImgType As Integer) As Image
        Dim clsDB As tClass_SQLServer = Nothing
        Dim readSQL As SqlDataReader = Nothing
        Dim imgData As Image = Nothing
        Try


            Dim strTableImg() As String = {DB_MAE_DEFECT_IMG}

            '-------------------------------------------------------------------
            ' ＳＱＬ作成 (疵画像)
            '-------------------------------------------------------------------
            Dim strSQL As String = ""
            strSQL = ""
            strSQL &= "SELECT"
            strSQL &= " 画像1"

            strSQL &= " FROM"
            strSQL &= " " & strTableImg(0)
            strSQL &= " WHERE"
            strSQL &= " 管理No='" & strKanriNo & "'"
            strSQL &= " AND 疵No=" & nKizuNo

            '-------------------------------------------------------------------
            ' 実行
            '-------------------------------------------------------------------
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)

            Dim bDbRet As Boolean = clsDB.Open()
            If bDbRet = False Then                                              ' DB接続失敗
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("データ管理 TCM疵画像DB接続失敗 [{0}][{1}]", strKanriNo, nKizuNo))
                Return Nothing
            End If

            readSQL = clsDB.SelectExecute(strSQL)
            If readSQL Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("データ管理 TCM疵画像取得SQL実行失敗 SQL={0}", strSQL))
                Return Nothing
            End If

            '-------------------------------------------------------------------
            ' 格納
            '-------------------------------------------------------------------
            With readSQL
                imgData = Nothing
                If readSQL.Read() = True Then                                   ' 疵画像はあれば格納する
                    If .IsDBNull(0) = False Then
                        If frmJisseki.EM_DSP_IMAGE_FORMAT.nJpeg = nImgType Then

                            'JPEG
                            imgData = tClass_BmpJpeg.DataToImage(CType(.GetValue(0), Byte()))
                        Else
                            'BMP
                            imgData = tClass_BmpJpeg.DataToBmp(typData.w_size, typData.h_size, CType(.GetValue(0), Byte()))
                        End If
                    End If
                End If
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("データ管理 TCM疵拡大画像表示 疵画像読み込み異常 [{0}]", ex.Message))
            imgData = Nothing
            Return Nothing
        Finally
            If clsDB Is Nothing = False Then
                clsDB.Cancel()
                If readSQL Is Nothing = False Then
                    readSQL.Close()
                End If
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try
        Return imgData
    End Function

    ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
    ''' <summary>
    ''' 疵リストをソートする
    ''' </summary>
    ''' <param name="dicMapDefect"></param>
    ''' <param name="bNagate">True:長手方向の反転あり False:反転なし</param>
    ''' <remarks></remarks>
    Private Sub Sort_Map_KizulistTcm(ByRef dicMapDefect As Dictionary(Of LIST_KEY, List(Of TYP_MAP_LIST_VALUE)), ByVal bNagate As Boolean)
        Dim lstData As List(Of TYP_MAP_LIST_VALUE)             '疵種リスト
        Try
            For Each key As LIST_KEY In dicMapDefect.Keys
                lstData = dicMapDefect(key)
                Dim Comparer As New List_ComparerTcm()
                lstData.Sort(Comparer)
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "疵リストソートエラー [" & ex.Message & "]")
        End Try
    End Sub


    ''' <summary>
    ''' 検索本体 (検索値を検索する) TCM用
    ''' </summary>
    ''' <remarks></remarks>
    Public Class List_ComparerTcm
        Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE) 'IComparerインターフェイスを実装する

        ''' <summary>
        ''' 比較方法 (sortにより実装)
        ''' </summary>
        ''' <param name="x">比較対照</param>
        ''' <param name="y">比較対照</param>
        ''' <returns>0：同じ 0より小さい:Xの方が小さい 0より大きい:Xの方が大きい</returns>
        ''' <remarks></remarks>
        Public Function Compare(ByVal x As TYP_MAP_LIST_VALUE, ByVal y As TYP_MAP_LIST_VALUE) As Integer _
         Implements System.Collections.Generic.IComparer(Of TYP_MAP_LIST_VALUE).Compare

            ' 優先順位
            ' ① 工程種別(TCM穴(4)＞TCM疵(6))
            ' ② グレード優先度
            ' ③ 疵種優先度
            ' ④ 面積順
            ' ⑤ 長手順
            ' ⑥ 疵No
            Dim dNow(6) As Double
            Dim dPer(6) As Double
            Dim bMode(6) As Boolean

            dNow(0) = CDbl(x.nKouteiID)
            dPer(0) = CDbl(y.nKouteiID)
            bMode(0) = False

            dNow(1) = CDbl(x.nGYuu)
            dPer(1) = CDbl(y.nGYuu)
            bMode(1) = False

            dNow(2) = CDbl(x.nTYuu)
            dPer(2) = CDbl(y.nTYuu)
            bMode(2) = False

            dNow(3) = CDbl(x.nAria)
            dPer(3) = CDbl(y.nAria)
            bMode(3) = True

            dNow(4) = CDbl(x.nY)
            dPer(4) = CDbl(y.nY)
            bMode(4) = False

            dNow(5) = CDbl(x.nKizuNo)
            dPer(5) = CDbl(y.nKizuNo)
            bMode(5) = False

            If dNow(0) = dPer(0) And dNow(1) = dPer(1) And dNow(2) = dPer(2) And dNow(3) = dPer(3) And dNow(4) = dPer(4) And dNow(5) = dPer(5) Then
                Return 0
            End If
            ' 優先度比較
            If Not SelectPriorityDetect(dNow, dPer, bMode) Then
                Return 1
            Else
                Return -1
            End If
        End Function
    End Class
	' 20221021 2TCM実機化に伴う下工程改造 小島 ---<<<

	'// 2023.11.17 最終工程検査装置高機能化その４ -------------------->>>>>
	''' <summary>
	''' 出側長手位置取得
	''' </summary>
	''' <param name="dInLen">入側長手位置(mm)</param>
	''' <returns>出側長手位置(mm)</returns>
	''' <remarks>※長手反転時は入側長手位置、カット位置は事前に反転されているものとする。</remarks>
	Private Function GetOutLen(ByVal dInLen As Single, ByVal bUpTrun As Boolean, ByVal nResultLen As Integer) As Single
		Dim dOutLen As Single = dInLen

		If m_listCutInf Is Nothing OrElse 0 = m_listCutInf.Count Then
			' カットなし
			Return dInLen
		End If

		' カット有なら直近のカット位置からの距離を返す
		Dim bExistCut As Boolean = False

		For ii As Integer = 0 To m_listCutInf.Count - 1
			Dim nCutIndex As Integer = 0
			If True = bUpTrun Then
				' 長手反転あり -> カット位置は長手反転されて降順にセットされている。
				nCutIndex = ii
			Else
				' 長手反転なし -> カット位置は昇順にセットされている。
				nCutIndex = m_listCutInf.Count - ii - 1
			End If

			If dInLen >= m_listCutInf(nCutIndex).dPosS_MM Then
				' カット位置からの距離
				bExistCut = True
				dOutLen = dInLen - m_listCutInf(nCutIndex).dPosS_MM
				Exit For
			End If
		Next

		If 0 > dOutLen Then
			dOutLen = 0
		End If

		Return dOutLen
	End Function
	'// 2023.11.17 最終工程検査装置高機能化その４ <<<<<--------------------

End Class
