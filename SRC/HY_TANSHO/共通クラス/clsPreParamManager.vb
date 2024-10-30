'*********************************************************************************
'　前工程パラメータ情報管理クラス
'	[Ver]
'		Ver.01    2012/09/01  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports System.Collections
Imports tClassLibrary
Imports tClassLibrary.tClass
Public Class clsPreParamManager

    Implements IDisposable                          ' デストラクタ定義
    '---------------------------------------------------------------------------
    ' 型定義
    '---------------------------------------------------------------------------
    Public Structure PRE_MASTER_DATA
        Dim nJudge As Integer                                               ' 有効判定
        Dim strPreName As String                                            ' 名称
        Dim nPreDspMode As Integer                                          ' 前工程表示モード
        Dim nImgTyp As Integer                                              ' 画像形式(0：JPEG    1：BMP)
        Dim nOther As Integer                                               ' 他所材(0:自所,1:他所)
    End Structure

    ''' <summary>
    ''' 疵種データ
    ''' </summary>
    ''' <remarks>前工程疵種マスターテーブルから必要なデータだけ保持</remarks>
    Private Structure PRE_TYPE_NAME_DATA
        Dim sFFname As String                                               ' FFName
        Dim nID As Integer                                                  ' ID
        Dim strName As String                                               ' Name
        Dim nPriority As Integer                                            ' 優先度
        Dim strMapName As String                                            ' マップ表示名称
        Dim nVisible As Integer                                             ' 表示有無
    End Structure

    ''' <summary>
    ''' グレードデータ
    ''' </summary>
    ''' <remarks>グレードマスターテーブルから必要なデータだけ保持</remarks>
    Private Structure PRE_GRADE_NAME_DATA
        Dim sFFname As String                                               ' FFName
        Dim nID As Integer                                                  ' グレードID
        Dim strName As String                                               ' Name
        Dim nPriority As Integer                                            ' 優先度
        Dim nMapColor As Integer                                            ' マップ表示色
    End Structure



    '前工程疵種キー構造体
    Public Structure PreTypeNameKey
        Dim strProc As String           '前工程コード
        Dim strFFTypeName As String     'FF名称
    End Structure

    ''前工程疵種キー構造体
    'Public Structure PreTypeNameKey2
    '	Dim strProc As String			'前工程コード
    '	Dim nTid As Integer				'疵種
    'End Structure

	'前工程グレードキー構造体
	Public Structure PreGradeNameKey
		Dim strProc As String			'前工程コード
		Dim strFFGradeName As String	'FF名称
	End Structure

    '前工程グレードキー構造体
    Public Structure PreGradeNameKey2
        Dim strProc As String           '前工程コード
        Dim nGid As Integer             'グレード
    End Structure

	'---------------------------------------------------------------------------
	' メンバ変数
	'---------------------------------------------------------------------------
    Private m_bIsDesposed As Boolean = False                                    ' リソース開放済みフラウグ True:開放済み
    Private mcls_Log As tClass_LogManager                                       ' ログ管理

    Private m_dctPreMaster As Dictionary(Of String, PRE_MASTER_DATA)                    ' 前工程マスター情報 (キーは前工程)
    Private m_dctPreType As Dictionary(Of PreTypeNameKey, PRE_TYPE_NAME_DATA)           ' 前工程疵種データ一覧 (キーは前工程疵種キー構造体)
    Private m_dctPreGrade As Dictionary(Of PreGradeNameKey, PRE_GRADE_NAME_DATA)        ' 前工程グレードデータ一覧 (キーは前工程グレードキー構造体)

    'Private m_dctPreType2 As Dictionary(Of PreTypeNameKey2, PRE_TYPE_NAME_DATA)			' 前工程疵種データ一覧 (キーは前工程疵種キー構造体2)
    Private m_dctPreGrade2 As Dictionary(Of PreGradeNameKey2, PRE_GRADE_NAME_DATA)      ' 前工程グレードデータ一覧 (キーは前工程グレードキー構造体2)

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="log">ログクラス</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef log As tClass.tClass_LogManager)

        'ログクラス
        mcls_Log = log                                                          ' ログマネージャ設定
        m_dctPreMaster = New Dictionary(Of String, PRE_MASTER_DATA)
		m_dctPreType = New Dictionary(Of PreTypeNameKey, PRE_TYPE_NAME_DATA)
		m_dctPreGrade = New Dictionary(Of PreGradeNameKey, PRE_GRADE_NAME_DATA)
        'm_dctPreType2 = New Dictionary(Of PreTypeNameKey2, PRE_TYPE_NAME_DATA)
        m_dctPreGrade2 = New Dictionary(Of PreGradeNameKey2, PRE_GRADE_NAME_DATA)

    End Sub


    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If m_bIsDesposed = False Then
            m_bIsDesposed = True                                                ' Dispose済みとする

            m_dctPreMaster.Clear()
            m_dctPreMaster = Nothing
			m_dctPreType.Clear()
			m_dctPreType = Nothing
			m_dctPreGrade.Clear()
			m_dctPreGrade = Nothing
            'm_dctPreType2.Clear()
            'm_dctPreType2 = Nothing
            m_dctPreGrade2.Clear()
            m_dctPreGrade2 = Nothing
		End If
    End Sub

#Region " IDisposable Support "
    ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド定義
    '/ /////////////////////////////////////////////////////////////////////////////////


    ''' <summary>
    ''' 全て読み込み
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG</returns>
    ''' <remarks></remarks>
    Public Function ReadAll() As Boolean

        ' 疵検定数読み込み
        If Not ReadPreMaster() Then
            Return False
        End If

        '前工程疵種テーブル読み込み
        If Not ReadPreTypeName() Then
            Return False
        End If

        '前工程グレードテーブル読み込み
        If Not ReadPreGradeName() Then
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' 疵検定数取得
    ''' </summary>
    ''' <returns>実行結果 True:成功 False:失敗</returns>
    ''' <remarks></remarks>
    Public Function ReadPreMaster() As Boolean
        Dim bRet As Boolean = True

        Dim tcls_DB As tClass_SQLServer = Nothing
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            '-------------------------------------------------------------------
            ' SQL作成
            '-------------------------------------------------------------------
            strSql = ""
            strSql &= " SELECT 前工程コード"
            strSql &= "        ,有効判定"
            strSql &= "        ,名称"
            strSql &= "        ,前工程表示モード"
            strSql &= "        ,画像形式"
            strSql &= "        ,他所材判定"
            strSql &= " FROM " & DB_MAE_MASTER
            strSql &= " WHERE 有効判定 = 1"

            '-------------------------------------------------------------------
            ' SQL実行
            '-------------------------------------------------------------------
            tcls_DB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim dbret As Boolean = tcls_DB.Open()                               ' DB接続
            If dbret = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "DB接続に失敗")
                bRet = False
                Exit Try
            End If

            sqlRead = tcls_DB.SelectExecute(strSql)                             ' SQL実行
            If sqlRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "前工程マスター取得SQL異常 sql=" & strSql)
                bRet = False
                Exit Try
            End If

            m_dctPreMaster.Clear()
            While sqlRead.Read() = True
                Dim master As New PRE_MASTER_DATA
                Dim strProc As String           '前工程コード
                With master
                    strProc = CStr(sqlRead.GetValue(0))                     ' 前工程コード
                    .nJudge = CInt(sqlRead.GetValue(1))                     ' 有効判定
                    .strPreName = CStr(sqlRead.GetValue(2))                 ' 前工程コード
                    .nPreDspMode = CInt(sqlRead.GetValue(3))                ' 前工程表示モード
                    .nImgTyp = CInt(sqlRead.GetValue(4))                    ' 画像形式
                    .nOther = CInt(sqlRead.GetValue(5))                     ' 他所材判定
                End With
                m_dctPreMaster.Add(strProc, master)
            End While

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("{0}取得に失敗しました。[{1}]", DB_MAE_MASTER, ex.Message))
            bRet = False
        Finally
            If tcls_DB Is Nothing = False Then                                  ' DBに接続資源がある場合
                tcls_DB.Close()
                tcls_DB.Dispose()
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 前工程疵種マスタテーブル
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Public Function ReadPreTypeName() As Boolean
        Dim bRet As Boolean = True

        Dim tcls_DB As tClass_SQLServer = Nothing
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
		Dim typPreTypeKey As PreTypeNameKey

        Try
            '-------------------------------------------------------------------
            ' SQL作成
            '-------------------------------------------------------------------
            strSql = ""
			strSql &= " SELECT "
			strSql &= " 前工程コード, "
			strSql &= " FFName, "
			strSql &= " ID, "
			strSql &= " Name, "
			strSql &= " 優先度, "
			strSql &= " マップ表示名称, "
			strSql &= " 表示有無 "
			strSql &= " FROM " & DB_MAE_TYPE_NAME
            strSql &= " ORDER BY 前工程コード ASC "

            '-------------------------------------------------------------------
            ' SQL実行
            '-------------------------------------------------------------------
            tcls_DB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim dbret As Boolean = tcls_DB.Open()                               ' DB接続
            If dbret = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "DB接続に失敗")
                bRet = False
                Exit Try
            End If

            sqlRead = tcls_DB.SelectExecute(strSql)                             ' SQL実行
            If sqlRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "前工程疵種マスター取得SQL異常 sql=" & strSql)
                bRet = False
                Exit Try
            End If

            m_dctPreType.Clear()
            While sqlRead.Read() = True

                'キーをセット
				typPreTypeKey.strProc = sqlRead.GetString(sqlRead.GetOrdinal("前工程コード")).Trim
				typPreTypeKey.strFFTypeName = sqlRead.GetString(sqlRead.GetOrdinal("FFName")).Trim

				Dim type As New PRE_TYPE_NAME_DATA
                With type
                    .sFFname = sqlRead.GetString(sqlRead.GetOrdinal("FFName")).Trim
                    .nID = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("ID")))
                    .strName = sqlRead.GetString(sqlRead.GetOrdinal("Name")).Trim
                    .nPriority = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("優先度")))
                    .strMapName = sqlRead.GetString(sqlRead.GetOrdinal("マップ表示名称")).Trim
                    .nVisible = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("表示有無")))
                End With
                m_dctPreType.Add(typPreTypeKey, type)
            End While
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("{0}取得に失敗しました。[{1}]", DB_MAE_TYPE_NAME, ex.Message))
            bRet = False
        Finally
            If tcls_DB Is Nothing = False Then                                  ' DBに接続資源がある場合
                tcls_DB.Close()
                tcls_DB.Dispose()
            End If
        End Try

        Return bRet
    End Function
    ''' <summary>
    ''' 前工程グレードマスタテーブル
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Public Function ReadPreGradeName() As Boolean
        Dim bRet As Boolean = True

        Dim tcls_DB As tClass_SQLServer = Nothing
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
        Dim typPreGradeKey As PreGradeNameKey
        Dim typPreGradeKey2 As PreGradeNameKey2

        Try
            '-------------------------------------------------------------------
            ' SQL作成
            '-------------------------------------------------------------------
            strSql = ""
			strSql &= " SELECT "
			strSql &= " 前工程コード, "
			strSql &= " FFName, "
			strSql &= " グレードID, "
			strSql &= " Name, "
			strSql &= " 優先度, "
			strSql &= " マップ表示色 "
            strSql &= " FROM " & DB_MAE_GRADE_NAME
            'strSql &= " ORDER BY 前工程コード ASC "
            strSql &= " ORDER BY 前工程コード ASC, 優先度 ASC "

            '-------------------------------------------------------------------
            ' SQL実行
            '-------------------------------------------------------------------
            tcls_DB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim dbret As Boolean = tcls_DB.Open()                               ' DB接続
            If dbret = False Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "DB接続に失敗")
                bRet = False
                Exit Try
            End If

            sqlRead = tcls_DB.SelectExecute(strSql)                             ' SQL実行
            If sqlRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, "前工程グレードマスター取得SQL異常 sql=" & strSql)
                bRet = False
                Exit Try
            End If

            m_dctPreGrade.Clear()
            m_dctPreGrade2.Clear()
            While sqlRead.Read() = True

                'キーをセット
                typPreGradeKey.strProc = sqlRead.GetString(sqlRead.GetOrdinal("前工程コード")).Trim
                typPreGradeKey.strFFGradeName = sqlRead.GetString(sqlRead.GetOrdinal("FFName")).Trim

                typPreGradeKey2.strProc = sqlRead.GetString(sqlRead.GetOrdinal("前工程コード")).Trim
                typPreGradeKey2.nGid = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("グレードID")))

                Dim gread As New PRE_GRADE_NAME_DATA
                With gread
                    .sFFname = sqlRead.GetString(sqlRead.GetOrdinal("FFName")).Trim
                    .nID = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("グレードID")))
                    .strName = sqlRead.GetString(sqlRead.GetOrdinal("Name")).Trim
                    .nPriority = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("優先度")))
                    .nMapColor = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("マップ表示色")))
                End With
                m_dctPreGrade.Add(typPreGradeKey, gread)
                m_dctPreGrade2.Add(typPreGradeKey2, gread)
            End While
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("{0}取得に失敗しました。[{1}]", DB_MAE_GRADE_NAME, ex.Message))
            bRet = False
        Finally
            If tcls_DB Is Nothing = False Then                                  ' DBに接続資源がある場合
                tcls_DB.Close()
                tcls_DB.Dispose()
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' 前工程コード有効判定取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    ''' <returns>該当項目の有効無効。  （0：無効  1：有効）</returns>
    ''' <remarks></remarks>
    Public Function GetProcJudge(ByVal strProc As String) As Integer

        If False = m_dctPreMaster.ContainsKey(strProc) Then                        ' 対象KEYがない場合
            Return 0
        End If

        Return m_dctPreMaster(strProc).nJudge
    End Function

    ''' <summary>
    ''' 前工程名称取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    ''' <returns>前工程名称</returns>
    ''' <remarks></remarks>
    Public Function GetProcName(ByVal strProc As String) As String

        If False = m_dctPreMaster.ContainsKey(strProc) Then                        ' 対象KEYがない場合
            Return ""
        End If

        Return m_dctPreMaster(strProc).strPreName
    End Function

    ''' <summary>
    ''' 前工程コード表示モード取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    ''' <returns>0：本システムで未管理、1：前工程疵検情報取得 ＆ 疵画像表示、2：FF情報のみ画面表示</returns>
    ''' <remarks></remarks>
    Public Function GetProcDspMode(ByVal strProc As String) As Integer

        If False = m_dctPreMaster.ContainsKey(strProc) Then                        ' 対象KEYがない場合
            Return 0
        End If

        Return m_dctPreMaster(strProc).nPreDspMode
    End Function

    ''' <summary>
    ''' 画像形式取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    ''' <returns>0：JPEG    1：BMP</returns>
    ''' <remarks></remarks>
    Public Function GetProcImgTyp(ByVal strProc As String) As Integer

        If False = m_dctPreMaster.ContainsKey(strProc) Then                        ' 対象KEYがない場合
            Return 0
        End If

        Return m_dctPreMaster(strProc).nImgTyp
    End Function



    ''' <summary>
    ''' 疵種名を取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    '''<param name="strFFName">FFName</param>
    ''' <returns>疵種名</returns>
    ''' <remarks></remarks>
    Public Function GetDspTypeName(ByVal strProc As String, ByVal strFFName As String) As String

        Dim typKey As PreTypeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFTypeName = strFFName.Trim

        If False = m_dctPreType.ContainsKey(typKey) Then                        ' 対象KEYがない場合
            '2014.08.15 川原    所間連携    >>>>>>>>
            '疵種名未登録時は不明とする
            'Return ""
            Return "不明"
            '2014.08.15 川原    所間連携    >>>>>>>>
        End If

        Return m_dctPreType(typKey).strName
    End Function

    ''' <summary>
    ''' マップ表示疵種名を取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns>マップ表示疵種名</returns>
    ''' <remarks></remarks>
    Public Function GetDspMapName(ByVal strProc As String, ByVal strFFName As String) As String

        Dim typKey As PreTypeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFTypeName = strFFName.Trim

        If False = m_dctPreType.ContainsKey(typKey) Then                        ' 対象KEYがない場合
            '2014.08.15 川原    所間連携    >>>>>>>>
            '疵種名未登録時は不明とする
            'Return ""
            Return "不明"
            '2014.08.15 川原    所間連携    >>>>>>>>
        End If

        Return m_dctPreType(typKey).strMapName
    End Function
    ''' <summary>
    ''' 疵種優先度取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns>優先度</returns>
    ''' <remarks></remarks>
    Public Function GetTypePriority(ByVal strProc As String, ByVal strFFName As String) As Integer

        Dim typKey As PreTypeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFTypeName = strFFName.Trim

        If False = m_dctPreType.ContainsKey(typKey) Then                        ' 対象KEYがない場合
            Return 999
        End If

        Return m_dctPreType(typKey).nPriority
    End Function

    '''' <summary>
    '''' 疵種優先度取得
    '''' </summary>
    ''''<param name="strProc">前工程コード</param>
    '''' <param name="nTid">疵種</param>
    '''' <returns>優先度</returns>
    '''' <remarks></remarks>
    'Public Function GetTypePriority2(ByVal strProc As String, ByVal nTid As Integer) As Integer

    '	Dim typKey As PreTypeNameKey2
    '	typKey.strProc = strProc.Trim
    '	typKey.nTid = nTid

    '	If False = m_dctPreType2.ContainsKey(typKey) Then						' 対象KEYがない場合
    '		Return 999
    '	End If
    '	Return m_dctPreType2(typKey).nPriority

    'End Function

    ''' <summary>
    ''' マップ表示色を取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns>マップ表示色</returns>
    ''' <remarks></remarks>
    Public Function GetDspGradeColor(ByVal strProc As String, ByVal strFFName As String) As Integer

        Dim typKey As PreGradeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFGradeName = strFFName.Trim

        If False = m_dctPreGrade.ContainsKey(typKey) Then
            '2014.08.15 川原    所間連携    >>>>>>>>
            'グレード名未登録時は背景色を白色とする
            'Return 0
            Return Color.White.ToArgb
            '2014.08.15 川原    所間連携    <<<<<<<<
        End If

        Return m_dctPreGrade(typKey).nMapColor
    End Function
    ''' <summary>
    ''' グレード名称取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns>グレード名</returns>
    ''' <remarks></remarks>
    Public Function GetDspGradeName(ByVal strProc As String, ByVal strFFName As String) As String

        Dim typKey As PreGradeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFGradeName = strFFName.Trim

        If False = m_dctPreGrade.ContainsKey(typKey) Then
            '2014.08.15 川原    所間連携    >>>>>>>>
            'グレード名未登録時は不明とする
            'Return ""
            Return "不明"
            '2014.08.15 川原    所間連携    <<<<<<<<
        End If

        Return m_dctPreGrade(typKey).strName

    End Function
    ''' <summary>
    ''' グレード優先度取得
    ''' </summary>
    '''<param name="strProc">前工程コード</param>
    ''' <param name="strFFName">FFName</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetGradePriority(ByVal strProc As String, ByVal strFFName As String) As Integer

        Dim typKey As PreGradeNameKey
        typKey.strProc = strProc.Trim
        typKey.strFFGradeName = strFFName.Trim

        If False = m_dctPreGrade.ContainsKey(typKey) Then
            Return 999
        End If

        Return m_dctPreGrade(typKey).nPriority

    End Function
    '''' <summary>
    '''' グレード優先度取得
    '''' </summary>
    ''''<param name="strProc">前工程コード</param>
    '''' <param name="nGid">グレード</param>
    '''' <returns></returns>
    '''' <remarks></remarks>
    'Public Function GetGradePriority2(ByVal strProc As String, ByVal nGid As Integer) As Integer

    '	Dim typKey As PreGradeNameKey2
    '	typKey.strProc = strProc.Trim
    '	typKey.nGid = nGid

    '	If False = m_dctPreGrade2.ContainsKey(typKey) Then
    '		Return 999
    '	End If

    '	Return m_dctPreGrade2(typKey).nPriority

    'End Function

    ''' <summary>
    ''' 優先度比較
    ''' </summary>
    ''' <param name="strPreProc">前回前工程</param>
    ''' <param name="strNowProc">今回前工程</param>
    ''' <param name="strPreFFTypeName">前回FF疵種</param>
    ''' <param name="strNowFFTypeName">今回FF疵種</param>
    ''' <param name="strPreFFGradeName">前回FFグレード</param>
    ''' <param name="strNowFFGradeName">今回FFグレード</param>
    ''' <returns>復帰情報(True:優先度が高い,False:優先度が低い)</returns>
    ''' <remarks></remarks>
    Public Function CheckPriority(ByVal strPreProc As String, ByVal strNowProc As String, ByVal strPreFFTypeName As String, ByVal strNowFFTypeName As String, _
                                    ByVal strPreFFGradeName As String, ByVal strNowFFGradeName As String) As Boolean



        '前回のグレードよりも優先度が小さければ、優先度が高い
        If GetGradePriority(strPreProc, strPreFFGradeName) > GetGradePriority(strNowProc, strNowFFGradeName) Then

            Return True

        ElseIf GetGradePriority(strPreProc, strPreFFGradeName) = GetGradePriority(strNowProc, strNowFFGradeName) Then

            'グレードの優先度が同じなら、疵種の優先度を比較し、優先度が小さければ、優先度が高い
            If GetTypePriority(strPreProc, strPreFFTypeName) > GetTypePriority(strNowProc, strNowFFTypeName) Then
                Return True
            End If

        End If

        Return False

    End Function

    '''' <summary>
    '''' 優先度比較
    '''' </summary>
    '''' <param name="strPreProc">前回前工程</param>
    '''' <param name="strNowProc">今回前工程</param>
    '''' <param name="nPreTid">前回FF疵種</param>
    '''' <param name="nNowTid">今回FF疵種</param>
    '''' <param name="strPreGid">前回FFグレード</param>
    '''' <param name="strNowGid">今回FFグレード</param>
    '''' <returns>復帰情報(True:優先度が高い,False:優先度が低い)</returns>
    '''' <remarks></remarks>
    'Public Function CheckPriority2(ByVal strPreProc As String, ByVal strNowProc As String, ByVal nPreTid As Integer, ByVal nNowTid As Integer, _
    '  ByVal strPreGid As Integer, ByVal strNowGid As Integer) As Boolean



    '	'前回のグレードよりも優先度が小さければ、優先度が高い
    '	If GetGradePriority2(strPreProc, strPreGid) > GetGradePriority2(strNowProc, strNowGid) Then

    '		Return True

    '	ElseIf GetGradePriority2(strPreProc, strPreGid) = GetGradePriority2(strNowProc, strNowGid) Then

    '		'グレードの優先度が同じなら、疵種の優先度を比較し、優先度が小さければ、優先度が高い
    '		If GetTypePriority2(strPreProc, nPreTid) > GetTypePriority2(strNowProc, nNowTid) Then
    '			Return True
    '		End If

    '	End If

    '	Return False

    'End Function
    ''' <summary>
    ''' 疵の優先比較を行います
    ''' </summary>
    ''' <returns>-1:疵1が優先度高 0:同じ優先度 1:疵2が優先度高</returns>
    ''' <remarks></remarks>
    Public Function ComparePriority(ByVal strPreProc As String, ByVal strNowProc As String, ByVal strPreFFTypeName As String, ByVal strNowFFTypeName As String, _
                                    ByVal strPreFFGradeName As String, ByVal strNowFFGradeName As String) As Integer
        Dim nPri1 As Integer = GetGradePriority(strPreProc, strPreFFGradeName)
        Dim nPri2 As Integer = GetGradePriority(strNowProc, strNowFFGradeName)

        If nPri1 > nPri2 Then                                                   ' 疵2のグレード優先度が高い
            Return 1                                                            ' 疵2が優先

        ElseIf nPri1 < nPri2 Then                                               ' 疵1のグレード優先度が高い
            Return -1                                                           ' 疵1が優先

        Else                                                                    ' グレード優先度が等しい
            nPri1 = GetTypePriority(strPreProc, strPreFFTypeName)
            nPri2 = GetTypePriority(strNowProc, strNowFFTypeName)
            If nPri1 > nPri2 Then                                               ' 疵2の疵種優先度が高い
                Return 1                                                        ' 疵2が優先

            ElseIf nPri1 < nPri2 Then                                           ' 疵1の疵種優先度が高い
                Return -1                                                       ' 疵1が優先
            End If
        End If

        Return 0                                                                ' 二つの疵は同じ優先度
    End Function

    ''' <summary>
    ''' 前工程表示有無チェック
    ''' </summary>
    ''' <param name="strProc">前工程</param>
    ''' <param name="strFFType">FF疵種</param>
    ''' <param name="strFFGrade">FFグレード</param>
    ''' <returns>True:表示する,False:表示しない</returns>
    ''' <remarks></remarks>
    Public Function CheckView(ByVal strProc As String, ByVal strFFType As String, ByVal strFFGrade As String) As Boolean
        Dim typKey As PreTypeNameKey
        Dim greKey As PreGradeNameKey
        Dim greKey2 As PreGradeNameKey2
        typKey.strProc = strProc.Trim
        typKey.strFFTypeName = strFFType.Trim
        greKey.strProc = strProc.Trim
        greKey.strFFGradeName = strFFGrade.Trim


		'HACK 2015/04/07 基本的には共有メモリに存在するデータは全て表示するので、必ずTrueで返すようにする
		Return True

        If Not m_dctPreType.ContainsKey(typKey) Then

            '2014.08.15 川原    所間連携    >>>>>>>>
            '疵種名が未登録時でも表示対象とする
            Return True

            '2014.08.15 川原    所間連携    >>>>>>>>
        End If

        If Not m_dctPreGrade.ContainsKey(greKey) Then

            '2014.08.15 川原    所間連携    >>>>>>>>
            'グレード名が未登録時でも表示対象とする
            Return True
            '2014.08.15 川原    所間連携    <<<<<<<<
        End If

        greKey2.strProc = strProc
        greKey2.nGid = m_dctPreType(typKey).nVisible

        If Not m_dctPreGrade2.ContainsKey(greKey2) Then
            Return False
        End If

        If m_dctPreGrade2(greKey2).nPriority >= m_dctPreGrade(greKey).nPriority And _
        m_dctPreGrade(greKey).nPriority > 0 Then
            Return True
        End If

        Return False

    End Function

End Class
