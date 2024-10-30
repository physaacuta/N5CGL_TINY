'*******************************************************************************
'   パラメータ管理
'
'   [Ver]
'       Ver.01  2017/08/31  初版
'
'   [メモ]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

''' <summary>
''' パラメータ管理
''' </summary>
''' <remarks></remarks>
Public Class clsParamManager
    Implements IDisposable
    '/////////////////////////////////////////////////////////////////////////////////
    '構造体
    '/////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 疵検定数マスター
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_COMMON_MASTER
        Dim nMapMin As Integer                              '探傷画面表示区間小
        Dim nMapMax As Integer                              '探傷画面表示区間大
        Dim nCntMapColorH As Integer                        '重複疵マップ背景色_重
        Dim nCntMapColorM As Integer                        '重複疵マップ背景色_中
        Dim nCntMapColorL As Integer                        '重複疵マップ背景色_軽
        Dim nCntMapCountH As Integer                        '重複疵マップ疵個数閾値_重
        Dim nCntMapCountM As Integer                        '重複疵マップ疵個数閾値_中
        Dim nCntMapCountL As Integer                        '重複疵マップ疵個数閾値_軽
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nLenDsp_Kensa() As Integer                      '設備長_表示基点_検査台(0：表 1：裏）
        Dim nLenDsp_Cutter As Integer                       '設備長_表示基点_シャーカット位置

		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
		Dim nLongPd_GuidanceAvailable As Integer            ' 長周期欠陥ガイダンス有無
		Dim nLongPd_NearFurnaceRangeFrom As Integer         ' 長周期欠陥炉チカ判定範囲From
		Dim nLongPd_NearFurnaceRangeTo As Integer           ' 長周期欠陥炉チカ判定範囲To
		Dim nLongPd_PeriodicPitchTolerance As Integer       ' 長周期欠陥周期ピッチ画面表示用公差
		Dim nLongPd_GuidanceForeColor As Integer            ' 長周期欠陥ガイダンス 文字色
		Dim nLongPd_GuidanceBackColor As Integer            ' 長周期欠陥ガイダンス 背景色
		Dim strLongPd_GuidanceMessage As String             ' 長周期欠陥ガイダンス メッセージ
		'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

		'イニシャライズ
		Public Sub initialize()
            ReDim nLenDsp_Kensa(NUM_MEN - 1)
        End Sub
    End Structure

    ''' <summary>
    ''' 疵種マスター
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_TYPE_NAME
        Dim nEdasysID As Integer                            'EdasysID
        Dim strName As String                               'Name
        Dim nPriority As Integer                            '優先度
        Dim strMapName As String                            'マップ表示名称
        Dim nSymbolColor As Integer                         '疵マップシンボル表示色
        Dim nMapSymbol As Integer                           '疵マップシンボル
        Dim nImageType1 As Integer                          '切出画像種類1
        Dim nImageType2 As Integer                          '切出画像種類2
    End Structure

    ''' <summary>
    ''' グレードマスター
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_GRADE_NAME
        Dim nEdasysID As Integer                            'グレードEdasysID
        Dim strName As String                               'Name
        Dim nPriority As Integer                            '優先度
        Dim nMapColor As Integer                            'マップ表示色
        Dim nDefGrade As Integer                            '欠陥グレード
        Dim nSymbolColor As Integer                         '疵マップシンボル表示色
    End Structure


    '///////////////////////////////////////////////////////////////////////////
    'メンバ変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                                    'リソース開放フラグ[True:開放済,False:未開放]

    Private mcls_Log As tClass_LogManager                                       'ログ管理

    Private m_dctType As Dictionary(Of Integer, TYP_TYPE_NAME)                  '疵種マスター(キーはEdasysID)
    Private m_dctGrade As Dictionary(Of Integer, TYP_GRADE_NAME)                'グレードマスター(キーはEdasysID)
    Private m_typCommonMaster As TYP_COMMON_MASTER                              '定数マスター

    '///////////////////////////////////////////////////////////////////////////
    'プロパティ
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 探傷画面表示区間大
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property MapStageBig() As Integer
        Get
            Return m_typCommonMaster.nMapMax
        End Get
    End Property

    ''' <summary>
    ''' 探傷画面表示区間小
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property MapStageSmall() As Integer
        Get
            Return m_typCommonMaster.nMapMin
        End Get
    End Property

    ''' <summary>
    ''' 探傷画面表示区間最小
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property MapStageMostSmall() As Integer
        Get
            Return m_typCommonMaster.nMapMin
        End Get
    End Property


    ''' <summary>
    ''' 設備長_表示基点_検査台位置[m]
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property PosKensa(ByVal nMen As Integer) As Integer
        Get
            Dim nMenWk As Integer = nMen
            If 0 > nMen OrElse NUM_MEN <= nMen Then        ' 面が異常値の場合
                nMenWk = 0
            End If

            Return CInt(Math.Ceiling(m_typCommonMaster.nLenDsp_Kensa(nMenWk) / 1000))
        End Get
    End Property

    ''' <summary>
    ''' 設備長_表示基点_シャーカット位置[m]
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property PosCutter() As Integer
        Get
            Return CInt(Math.Ceiling(m_typCommonMaster.nLenDsp_Cutter / 1000))
        End Get
    End Property

	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
	''' <summary>
	''' 長周期欠陥ガイダンス有無
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_GuidanceAvailable() As Integer
		Get
			Return m_typCommonMaster.nLongPd_GuidanceAvailable
		End Get
	End Property

	''' <summary>
	''' 長周期欠陥炉チカ判定範囲From
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_NearFurnaceRangeFrom() As Integer
		Get
			Return m_typCommonMaster.nLongPd_NearFurnaceRangeFrom
		End Get
	End Property

	''' <summary>
	''' 長周期欠陥炉チカ判定範囲To
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_NearFurnaceRangeTo() As Integer
		Get
			Return m_typCommonMaster.nLongPd_NearFurnaceRangeTo
		End Get
	End Property

	''' <summary>
	''' 長周期欠陥周期ピッチ画面表示用公差
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_PeriodicPitchTolerance() As Integer
		Get
			Return m_typCommonMaster.nLongPd_PeriodicPitchTolerance
		End Get
	End Property

	''' <summary>
	''' 長周期欠陥ガイダンス メッセージ
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_GuidanceMessage() As String
		Get
			Return m_typCommonMaster.strLongPd_GuidanceMessage
		End Get
	End Property

	''' <summary>
	''' 長周期欠陥ガイダンス 文字色
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_GuidanceForeColor() As Integer
		Get
			Return m_typCommonMaster.nLongPd_GuidanceForeColor
		End Get
	End Property

	''' <summary>
	''' 長周期欠陥ガイダンス 背景色
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_GuidanceBackColor() As Integer
		Get
			Return m_typCommonMaster.nLongPd_GuidanceBackColor
		End Get
	End Property
	'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

	'///////////////////////////////////////////////////////////////////////////
	'メンバ関数
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' コンストラクタ
	''' </summary>
	''' <param name="clsLog">ログ管理</param>
	''' <remarks></remarks>
	Public Sub New(ByRef clsLog As tClass_LogManager)
        Try
            mcls_Log = clsLog                                                       'ログ管理
            m_dctType = New Dictionary(Of Integer, TYP_TYPE_NAME)
            m_dctGrade = New Dictionary(Of Integer, TYP_GRADE_NAME)
            m_typCommonMaster.initialize()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("パラメータ管理生成異常[{0}]", ex.Message))
        End Try
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
                m_dctType.Clear()
                m_dctType = Nothing
                m_dctGrade.Clear()
                m_dctGrade = Nothing
            End If
            m_bIsDesposed = True                                                'Dispose済みとする
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("オブジェクト破棄異常 [{0}]", ex.Message))
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
    ''' 全パラメータ取得
    ''' </summary>
    ''' <returns>取得結果[True:成功,False:失敗]</returns>
    ''' <remarks></remarks>
    Public Function ReadAllParam() As Boolean
        '探傷画面表示区間取得
        If Not ReadMapInterval() Then
            Return False
        End If

        '疵検定数マスター取得
        If Not ReadCommonMaster() Then
            Return False
        End If

        '疵種マスター取得
        If Not ReadTypeName() Then
            Return False
        End If

        'グレードマスター取得
        If Not ReadGradeName() Then
            Return False
        End If

        Return True
    End Function

    ''' <summary>
    ''' 探傷画面表示区間取得
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ReadMapInterval() As Boolean
        Dim bRet As Boolean = False
        Dim clsDB As tClass.tClass_SQLServer = Nothing
        Dim clsRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            'DB接続
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            If Not clsDB.Open() Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB接続失敗"))
                Exit Try
            End If

            'SQL実行
            strSql = "SELECT"
            strSql &= " 探傷画面表示区間小"
			strSql &= ",探傷画面表示区間大"
			strSql &= " FROM " & DB_COMMON_MASTER
            clsRead = clsDB.SelectExecute(strSql)
            If clsRead Is Nothing Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL実行失敗[{0}]", strSql))
                Exit Try
            End If

            'DB取得
            clsRead.Read()
			With m_typCommonMaster
				'探傷画面表示区間小
				.nMapMin = clsRead.GetInt32(clsRead.GetOrdinal("探傷画面表示区間小"))
				'探傷画面表示区間大
				.nMapMax = clsRead.GetInt32(clsRead.GetOrdinal("探傷画面表示区間大"))
			End With

			'取得成功
			bRet = True
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]取得異常[{1}]", DB_COMMON_MASTER, ex.Message))
        Finally
            If Not clsRead Is Nothing Then
                clsRead.Close()
                clsRead = Nothing
            End If
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 疵検定数マスター取得
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ReadCommonMaster() As Boolean
        Dim bRet As Boolean = False
        Dim clsDB As tClass.tClass_SQLServer = Nothing
        Dim clsRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            'DB接続
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            If Not clsDB.Open() Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB接続失敗"))
                Exit Try
            End If

            'SQL実行
            strSql = "SELECT"
            strSql &= " 重複疵マップ背景色_重"
            strSql &= ",重複疵マップ背景色_中"
            strSql &= ",重複疵マップ背景色_軽"
            strSql &= ",重複疵マップ疵個数閾値_重"
            strSql &= ",重複疵マップ疵個数閾値_中"
            strSql &= ",重複疵マップ疵個数閾値_軽"
            strSql &= ",設備長_表示基点_裏検査台"
            strSql &= ",設備長_表示基点_表検査台"
			strSql &= ",設備長_表示基点_シャー"
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
			strSql &= ",LongPd_GuidanceAvailable"               ' 長周期欠陥ガイダンス有無
			strSql &= ",LongPd_NearFurnaceRangeFrom"            ' 長周期欠陥炉チカ判定範囲From
			strSql &= ",LongPd_NearFurnaceRangeTo"              ' 長周期欠陥炉チカ判定範囲To
			strSql &= ",LongPd_PeriodicPitchTolerance"          ' 長周期欠陥周期ピッチ画面表示用公差
			strSql &= ",LongPd_GuidanceForeColor"               ' 長周期欠陥ガイダンス 文字色
			strSql &= ",LongPd_GuidanceBackColor"               ' 長周期欠陥ガイダンス 背景色
			strSql &= ",LongPd_GuidanceMessage"                 ' 長周期欠陥ガイダンス メッセージ
			'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------

			strSql &= " FROM " & DB_COMMON_MASTER
            clsRead = clsDB.SelectExecute(strSql)
            If clsRead Is Nothing Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL実行失敗[{0}]", strSql))
                Exit Try
            End If

            'DB取得
            clsRead.Read()
            With m_typCommonMaster
                '重複疵マップ背景色_重
                .nCntMapColorH = clsRead.GetInt32(clsRead.GetOrdinal("重複疵マップ背景色_重"))
                '重複疵マップ背景色_中
                .nCntMapColorM = clsRead.GetInt32(clsRead.GetOrdinal("重複疵マップ背景色_中"))
                '重複疵マップ背景色_軽
                .nCntMapColorL = clsRead.GetInt32(clsRead.GetOrdinal("重複疵マップ背景色_軽"))
                '重複疵マップ疵個数閾値_重
                .nCntMapCountH = clsRead.GetInt32(clsRead.GetOrdinal("重複疵マップ疵個数閾値_重"))
                '重複疵マップ疵個数閾値_中
                .nCntMapCountM = clsRead.GetInt32(clsRead.GetOrdinal("重複疵マップ疵個数閾値_中"))
                '重複疵マップ疵個数閾値_軽
                .nCntMapCountL = clsRead.GetInt32(clsRead.GetOrdinal("重複疵マップ疵個数閾値_軽"))
                '設備長_表示基点_表検査台
                .nLenDsp_Kensa(0) = clsRead.GetInt32(clsRead.GetOrdinal("設備長_表示基点_表検査台"))
                '設備長_表示基点_裏検査台
                .nLenDsp_Kensa(1) = clsRead.GetInt32(clsRead.GetOrdinal("設備長_表示基点_裏検査台"))
				'設備長_表示基点_シャーカット位置
				.nLenDsp_Cutter = clsRead.GetInt32(clsRead.GetOrdinal("設備長_表示基点_シャー"))
				'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) ---------------->>>>>
				'長周期欠陥ガイダンス有無
				.nLongPd_GuidanceAvailable = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_GuidanceAvailable"))
				'長周期欠陥炉チカ判定範囲From
				.nLongPd_NearFurnaceRangeFrom = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_NearFurnaceRangeFrom"))
				'長周期欠陥炉チカ判定範囲To
				.nLongPd_NearFurnaceRangeTo = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_NearFurnaceRangeTo"))
				'長周期欠陥周期ピッチ画面表示用公差
				.nLongPd_PeriodicPitchTolerance = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_PeriodicPitchTolerance"))
				' 長周期欠陥ガイダンス 文字色
				.nLongPd_GuidanceForeColor = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_GuidanceForeColor"))
				' 長周期欠陥ガイダンス 背景色
				.nLongPd_GuidanceBackColor = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_GuidanceBackColor"))
				' 長周期欠陥ガイダンス メッセージ
				.strLongPd_GuidanceMessage = clsRead.GetString(clsRead.GetOrdinal("LongPd_GuidanceMessage"))
				'// 2024.07.26 Kouno 2CGL検査装置φω精度向上機能導入(長周期) <<<<<----------------
			End With

            '取得成功
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]取得異常[{1}]", DB_COMMON_MASTER, ex.Message))
        Finally
            If Not clsRead Is Nothing Then
                clsRead.Close()
                clsRead = Nothing
            End If
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 疵種マスター取得
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ReadTypeName() As Boolean
        Dim bRet As Boolean = False
        Dim clsDB As tClass.tClass_SQLServer = Nothing
        Dim clsRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            'DB接続
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            If Not clsDB.Open() Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB接続失敗"))
                Exit Try
            End If

            'SQL実行
            strSql = "SELECT"
            strSql &= " EdasysID"
            strSql &= ",Name"
            strSql &= ",優先度"
            strSql &= ",マップ表示名称"
            strSql &= ",疵マップシンボル表示色"
            strSql &= ",疵マップシンボル"
            strSql &= ",ISNULL(切出画像種類1,1) as 切出画像種類1"
            strSql &= ",ISNULL(切出画像種類2,2) as 切出画像種類2"
            strSql &= " FROM " & DB_TYPE_NAME
            strSql &= " ORDER BY ID ASC"
            clsRead = clsDB.SelectExecute(strSql)
            If clsRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL実行失敗[{0}]", strSql))
                Exit Try
            End If

            'DB取得
            m_dctType.Clear()
            While clsRead.Read() = True
                Dim typTypeName As New TYP_TYPE_NAME
                With typTypeName
                    'EdasysID
                    .nEdasysID = clsRead.GetInt32(clsRead.GetOrdinal("EdasysID"))
                    'Name
                    .strName = clsRead.GetString(clsRead.GetOrdinal("Name"))
                    '優先度
                    .nPriority = clsRead.GetInt32(clsRead.GetOrdinal("優先度"))
                    'マップ表示名称
                    .strMapName = clsRead.GetString(clsRead.GetOrdinal("マップ表示名称"))
                    ' 疵マップシンボル表示色
                    .nSymbolColor = clsRead.GetInt32(clsRead.GetOrdinal("疵マップシンボル表示色"))
                    ' 疵マップシンボル
                    .nMapSymbol = clsRead.GetInt32(clsRead.GetOrdinal("疵マップシンボル"))
                    If "" <> clsRead.GetValue(5).ToString() Then
                        .nImageType1 = clsRead.GetInt32(clsRead.GetOrdinal("切出画像種類1"))
                    Else
                        .nImageType1 = 0
                    End If
                    If "" <> clsRead.GetValue(6).ToString() Then
                        .nImageType2 = clsRead.GetInt32(clsRead.GetOrdinal("切出画像種類2"))
                    Else
                        .nImageType2 = 0
                    End If

                End With
                'ディクショナリ登録
                m_dctType.Add(typTypeName.nEdasysID, typTypeName)
            End While

            '取得成功
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]取得異常[{1}]", DB_TYPE_NAME, ex.Message))
        Finally
            If Not clsRead Is Nothing Then
                clsRead.Close()
                clsRead = Nothing
            End If
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' グレードマスター取得
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ReadGradeName() As Boolean
        Dim bRet As Boolean = False
        Dim clsDB As tClass.tClass_SQLServer = Nothing
        Dim clsRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            'DB接続
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim dbret As Boolean = clsDB.Open()
            If Not clsDB.Open() Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB接続失敗"))
                Exit Try
            End If

            'SQL実行
            strSql = "SELECT"
            strSql &= " EdasysID"
            strSql &= ",Name"
            strSql &= ",優先度"
            strSql &= ",マップ表示色"
            strSql &= ",欠陥グレード"
            strSql &= ",疵マップシンボル表示色"
            strSql &= " FROM " & DB_GRADE_NAME
            strSql &= " ORDER BY ID ASC"
            clsRead = clsDB.SelectExecute(strSql)
            If clsRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL実行失敗[{0}]", strSql))
                Exit Try
            End If

            'DB取得
            m_dctGrade.Clear()
            While clsRead.Read() = True
                Dim typGradeName As New TYP_GRADE_NAME
                With typGradeName
                    'グレードEdasysID
                    .nEdasysID = clsRead.GetInt32(clsRead.GetOrdinal("EdasysID"))
                    'Name
                    .strName = clsRead.GetString(clsRead.GetOrdinal("Name"))
                    '優先度
                    .nPriority = clsRead.GetInt32(clsRead.GetOrdinal("優先度"))
                    'マップ表示色
                    .nMapColor = clsRead.GetInt32(clsRead.GetOrdinal("マップ表示色"))
                    '欠陥グレード
                    .nDefGrade = clsRead.GetInt32(clsRead.GetOrdinal("欠陥グレード"))
                    '疵マップシンボル表示色 
                    .nSymbolColor = clsRead.GetInt32(clsRead.GetOrdinal("疵マップシンボル表示色"))
                End With
                m_dctGrade.Add(typGradeName.nEdasysID, typGradeName)
            End While

            '取得成功
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]取得異常[{1}]", DB_GRADE_NAME, ex.Message))
        Finally
            If Not clsRead Is Nothing Then
                clsRead.Close()
                clsRead = Nothing
            End If
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 疵名称取得
    ''' </summary>
    ''' <param name="nEdasysID">疵種EdasysID</param>
    ''' <returns>疵名称</returns>
    ''' <remarks>対象がない場合は空文字</remarks>
    Public Function GetTypeName(ByVal nEdasysID As Integer) As String
        Dim strName As String = ""                          '対象無し

        If m_dctType.ContainsKey(nEdasysID) Then            '対象有り
            strName = m_dctType(nEdasysID).strName
        End If

        Return strName
    End Function

    ''' <summary>
    ''' 疵マップ表示名取得
    ''' </summary>
    ''' <param name="nEdasysID">疵種EdasysID</param>
    ''' <returns>疵マップ表示名</returns>
    ''' <remarks>対象がない場合は空文字</remarks>
    Public Function GetMapName(ByVal nEdasysID As Integer) As String
        Dim strMapName As String = ""                       '対象無し

        If m_dctType.ContainsKey(nEdasysID) Then            '対象有り
            strMapName = m_dctType(nEdasysID).strMapName
        End If

        Return strMapName
    End Function

    ''' <summary>
    ''' 疵優先度取得
    ''' </summary>
    ''' <param name="nEdasysID">疵種EdasysID</param>
    ''' <returns>疵優先度</returns>
    ''' <remarks>対象がない場合は[999]</remarks>
    Public Function GetTypePriority(ByVal nEdasysID As Integer) As Integer
        Dim nPriority As Integer = 999                      '対象無し

        If m_dctType.ContainsKey(nEdasysID) Then            '対象有り
            nPriority = m_dctType(nEdasysID).nPriority
        End If

        Return nPriority
    End Function

    ''' <summary>
    ''' マップ表示色取得
    ''' </summary>
    ''' <param name="nEdasysID">グレードEdasysID</param>
    ''' <returns>マップ表示色</returns>
    ''' <remarks>対象がない場合は白色</remarks>
    Public Function GetGradeColor(ByVal nEdasysID As Integer) As Color
        Dim typMapColor As Color = Color.White              '対象無し

        If m_dctGrade.ContainsKey(nEdasysID) Then           '対象有り
            typMapColor = tMod_Common.GetIntToColor(m_dctGrade(nEdasysID).nMapColor)
        End If

        Return typMapColor
    End Function

    ''' <summary>
    ''' グレード名称取得
    ''' </summary>
    ''' <param name="nEdasysID">グレードEdasysID</param>
    ''' <returns>グレード名称</returns>
    ''' <remarks>対象がない場合は空文字</remarks>
    Public Function GetGradeName(ByVal nEdasysID As Integer) As String
        Dim strName As String = ""                          '対象無し

        If m_dctGrade.ContainsKey(nEdasysID) Then           '対象有り
            strName = m_dctGrade(nEdasysID).strName
        End If

        Return strName
    End Function

    ''' <summary>
    ''' グレード優先度取得
    ''' </summary>
    ''' <param name="nEdasysID">グレードEdasysID</param>
    ''' <returns>グレード優先度</returns>
    ''' <remarks>対象がない場合は[999]</remarks>
    Public Function GetGradePriority(ByVal nEdasysID As Integer) As Integer
        Dim nPriority As Integer = 999                      '対象無し

        If m_dctGrade.ContainsKey(nEdasysID) Then           '対象有り
            nPriority = m_dctGrade(nEdasysID).nPriority
        End If

        Return nPriority
    End Function


    ''' <summary>
    ''' 欠陥グレード取得
    ''' </summary>
    ''' <param name="nEdasysID">グレードEdasysID</param>
    ''' <returns>欠陥グレード</returns>
    ''' <remarks>対象がない場合は[EM_DETECT_GR2.DETECT_GR_NON(無害)]</remarks>
    Public Function GetDefectGrade(ByVal nEdasysID As Integer) As Integer
        Dim nDefGrade As Integer = EM_DETECT_GR2.DETECT_GR_NON      '無害

        If m_dctGrade.ContainsKey(nEdasysID) Then                   '対象有り
            nDefGrade = m_dctGrade(nEdasysID).nDefGrade
        End If

        Return nDefGrade
    End Function

    ''' <summary>
    ''' 疵優先度比較
    ''' </summary>
    ''' <param name="nType1">疵1の疵種EdasysID</param>
    ''' <param name="nType2">疵2の疵種EdasysID</param>
    ''' <param name="nGrade1">疵1のグレードEdasysID</param>
    ''' <param name="nGrade2">疵2のグレードEdasysID</param>
    ''' <returns>比較結果</returns>
    ''' <remarks>-1:疵1が優先度高 0:同じ優先度 1:疵2が優先度高</remarks>
    Public Function CompDefPriority(ByVal nType1 As Integer, ByVal nType2 As Integer, ByVal nGrade1 As Integer, ByVal nGrade2 As Integer) As Integer
        Dim nComp As Integer = 0                            '同じ優先度
        Dim nPri1 As Integer = GetGradePriority(nGrade1)    'グレード優先度1
        Dim nPri2 As Integer = GetGradePriority(nGrade2)    'グレード優先度2

        '優先度は小さいほうが高い
        If nPri1 > nPri2 Then                               '疵2のグレード優先度が高い
            nComp = 1                                       '疵2が優先
        ElseIf nPri1 < nPri2 Then                           '疵1のグレード優先度が高い
            nComp = -1                                      '疵1が優先
        Else                                                'グレード優先度が等しい
            nPri1 = GetTypePriority(nType1)                 '疵優先度1
            nPri2 = GetTypePriority(nType2)                 '疵優先度2
            If nPri1 > nPri2 Then                           '疵2の疵種優先度が高い
                nComp = 1                                   '疵2が優先
            ElseIf nPri1 < nPri2 Then                       '疵1の疵種優先度が高い
                nComp = -1                                  '疵1が優先
            End If
        End If

        Return nComp
    End Function

    ''' <summary>
    ''' 重複疵マップ背景色取得
    ''' </summary>
    ''' <param name="nDefGrade">欠陥グレード</param>
    ''' <returns>重複疵マップ背景色</returns>
    ''' <remarks>対象がない場合は白色</remarks>
    Public Function GetCntMapColor(ByVal nDefGrade As Integer) As Color
        Dim typBackColor As Color = Color.White             '対象無し

        Select Case nDefGrade
            Case EM_DETECT_GR.DETECT_GR_KEI                 '軽欠陥
                typBackColor = tMod_Common.GetIntToColor(m_typCommonMaster.nCntMapColorL)
            Case EM_DETECT_GR.DETECT_GR_TYU                 '中欠陥
                typBackColor = tMod_Common.GetIntToColor(m_typCommonMaster.nCntMapColorM)
            Case EM_DETECT_GR.DETECT_GR_JU                 '重欠陥
                typBackColor = tMod_Common.GetIntToColor(m_typCommonMaster.nCntMapColorH)
        End Select

        Return typBackColor
    End Function

    ''' <summary>
    ''' 重複疵マップ疵個数閾値取得
    ''' </summary>
    ''' <param name="nDefGrade">欠陥グレード</param>
    ''' <returns>重複疵マップ疵個数閾値</returns>
    ''' <remarks>対象がない場合は軽欠陥閾値</remarks>
    Public Function GetCntMapCount(ByVal nDefGrade As Integer) As Integer
        Dim nDefCount As Integer = m_typCommonMaster.nCntMapCountL  '対象無し

        Select Case nDefGrade
            Case EM_DETECT_GR.DETECT_GR_KEI                 '軽欠陥
                nDefCount = m_typCommonMaster.nCntMapCountL
            Case EM_DETECT_GR.DETECT_GR_TYU                 '中欠陥
                nDefCount = m_typCommonMaster.nCntMapCountM
            Case EM_DETECT_GR.DETECT_GR_JU                 '重欠陥
                nDefCount = m_typCommonMaster.nCntMapCountH
        End Select

        Return nDefCount
    End Function


    ''' <summary>
    ''' 画像表示番号を返します。
    ''' </summary>
    ''' <param name="nEdasysID">取得したい疵のEdasysID</param>
    ''' <param name="emImagePos">取得したい画像位置</param>
    ''' <returns>疵種名 ない場合は空文字</returns>
    ''' <remarks></remarks>
    Public Function GetTypeImage(ByVal nEdasysID As Integer, ByVal emImagePos As EM_IMAGE_POS) As Integer
        If m_dctType.ContainsKey(nEdasysID) = False Then                        ' EdasysIDがない場合
            Return -1
        End If
        If emImagePos = EM_IMAGE_POS.IMG1 Then                                ' 上
            Return m_dctType(nEdasysID).nImageType1
        ElseIf emImagePos = EM_IMAGE_POS.IMG2 Then                            ' 下
            Return m_dctType(nEdasysID).nImageType2
        Else                                                                    ' その他
            Return -1
        End If
    End Function


    ''' <summary>
    ''' グレードのシンボル色を取得します
    ''' </summary>
    ''' <param name="nEdasysID">取得したいグレードのEdasysID</param>
    ''' <returns>グレードのシンボル色</returns>
    ''' <remarks></remarks>
    Public Function GetGradeSymbolColor(ByVal nEdasysID As Integer) As Color
        If m_dctGrade.ContainsKey(nEdasysID) = False Then                       ' EdasysIDがない場合
            Return Color.White
        End If
        Return tMod.GetIntToColor(m_dctGrade(nEdasysID).nSymbolColor)           ' 色を戻す
    End Function

    ''' <summary>
    ''' 疵種のシンボル色を取得します
    ''' </summary>
    ''' <param name="nEdasysID">取得したいグレードのEdasysID</param>
    ''' <returns>疵種のシンボル色</returns>
    ''' <remarks></remarks>
    Public Function GetTypeSymbolColor(ByVal nEdasysID As Integer) As Color
        If m_dctType.ContainsKey(nEdasysID) = False Then                       ' EdasysIDがない場合
            Return Color.White
        End If
        Return tMod.GetIntToColor(m_dctType(nEdasysID).nSymbolColor)           ' 色を戻す
    End Function


    ''' <summary>
    ''' 疵マップ表示画面（散布図）用の疵シンボル取得
    ''' </summary>
    ''' <param name="nEdasysID">取得したい疵のEdasysID</param>
    ''' <returns>疵マップシンボル番号</returns>
    ''' <remarks></remarks>
    Public Function GetMapSymbolNo(ByVal nEdasysID As Integer) As Integer
        If m_dctType.ContainsKey(nEdasysID) = False Then                        ' EdasysIDがない場合
            Return -1
        End If
        Return m_dctType(nEdasysID).nMapSymbol
    End Function

    ''' <summary>
    ''' グレード情報を取得します
    ''' </summary>
    ''' <returns>グレードの優先度</returns>
    ''' <remarks></remarks>
    Public Function GetParamGrade() As Dictionary(Of Integer, TYP_GRADE_NAME).ValueCollection
        Return m_dctGrade.Values
    End Function

    ''' <summary>
    ''' 最も優先度が低いグレードを取得
    ''' </summary>
    ''' <returns>最も優先度が低いグレードのEdasysID</returns>
    ''' <remarks></remarks>
    Public Function GetPriorityMostLowGrade() As Integer

        Dim nWkEdasysID As Integer = 0
        Dim nWkPriority As Integer = 0

        '最も優先度が低いのを検索
        For Each nEdasyID As Integer In m_dctGrade.Keys

            If nWkPriority < m_dctGrade(nEdasyID).nPriority Then
                nWkEdasysID = nEdasyID
                nWkPriority = m_dctGrade(nEdasyID).nPriority
            End If

        Next

        Return nWkEdasysID

    End Function

End Class
