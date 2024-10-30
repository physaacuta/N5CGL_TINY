'*********************************************************************************
'　パラメータ情報管理クラス
'	[Ver]
'		Ver.01    2007/08/03  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On
Imports System.Collections
Imports tClassLibrary

Public Class clsParamManager

    Implements IDisposable                          ' デストラクタ定義

    Private disposedValue As Boolean = False        ' 重複する呼び出しを検出するには
    Private m_dctDspTypeName As New Dictionary(Of Integer, String)          '疵種名
    Private m_dctDspMapName As New Dictionary(Of Integer, String)           'マップ表示疵種名
    Private m_dctTypePriority As New Dictionary(Of Integer, Integer)        '疵種優先度
    Private m_dctDspGradeColor As New Dictionary(Of Integer, Integer)       'グレード色
    Private m_dctDspGradeName As New Dictionary(Of Integer, String)         'グレード名
    Private m_dctGradePriority As New Dictionary(Of Integer, Integer)       'グレード優先度
    Private m_dctDefectGrade As New Dictionary(Of Integer, Integer)         '欠陥グレード
    Private m_dctMapSymbol As New Dictionary(Of Integer, Integer)           '疵マップシンボル
    Private m_dctMapSymbolColor As New Dictionary(Of Integer, Integer)      '疵マップシンボル表示色
    Private m_Pout As PARAM_OUTPUT    'パラメータ設定画面情報保存用
    Private mcls_Log As tClass.tClass_LogManager           ' ログ管理クラス
    Private m_pLockObject As Object                 ' SyncLock同期オブジェクト

    '//==========================================================
    '// 検査パターン別パラメータ設定構造体（画面表示対象判定用）
    '//==========================================================
    '//疵種毎の表示情報構造体
    Private Structure PARAM_OUTPUT_TYPE
        Dim edasysID As Integer
        Dim disp As Integer
        Public Sub initialize()
            edasysID = 0
            disp = 0
        End Sub
    End Structure

    '//パターンコード毎の情報構造体
    Private Structure PARAM_OUTPUT_PCODE
        Dim num As Integer
        Dim pcode As Integer
        Dim type() As PARAM_OUTPUT_TYPE
        Dim def As PARAM_OUTPUT_TYPE
        Public Sub initialize()
            ReDim type(MAX_TYPE - 1)
            num = 0
            pcode = 0
        End Sub
    End Structure

    '//検査パターン別パラメータ情報構造体
    Private Structure PARAM_OUTPUT
        Dim pcode() As PARAM_OUTPUT_PCODE
        Public Sub initialize()
            ReDim pcode(MAX_PCODE - 1)
        End Sub
    End Structure

    '********************************************************************
    '定数定義
    '********************************************************************

    '====================================================================
    '	疵点マップ表示画面（散布図）用の疵シンボル定義
    ' 0：・  1:●  2:■  3:▲  4:▼ 5:◆  6:★  7:＊  8:×  9:＋
    '====================================================================
    ''// 疵マップ表示画面（散布図）用の疵シンボル定義
    Private PLOT_SYMBOL_TYPE() As String = { _
     "・", "●", "■", "▲", "▼", "◆", "★", "＊", "×", "＋"}             ' 疵シンボル定義


    '********************************************************************
    'プロパティー定義
    '********************************************************************

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="clsLog">ログクラス</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef clsLog As tClass.tClass_LogManager)

        'ログクラス
        mcls_Log = clsLog

        m_pLockObject = New Object

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド定義
    '/ /////////////////////////////////////////////////////////////////////////////////

    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
            End If

            ' TODO: 共有のアンマネージ リソースを解放します
            m_dctDspTypeName = Nothing
            m_dctDspMapName = Nothing
            m_dctTypePriority = Nothing
            m_dctDspGradeColor = Nothing
            m_dctDspGradeName = Nothing
            m_dctGradePriority = Nothing
            m_dctDefectGrade = Nothing
            m_pLockObject = Nothing

            m_dctMapSymbol = Nothing
            m_dctMapSymbolColor = Nothing

        End If
        Me.disposedValue = True
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
    ''' 全て読み込み
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG</returns>
    ''' <remarks></remarks>
    Public Function ReadAll() As Boolean

        '疵種テーブル読み込み
        If Not ReadTypeName() Then Return False

        'グレードテーブル読み込み
        If Not ReadGradeName() Then Return False

        '検査パターン別出力設定テーブル
        If Not ReadParam_OutPut() Then Return False

        Return True

    End Function

    ''' <summary>
    ''' 疵種マスタテーブル
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Public Function ReadTypeName() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
        Dim bRet As Boolean = False

        'SQL文作成
        strSql = ""
        strSql &= " select EdasysID"
        strSql &= "        ,マップ表示名称"
        strSql &= "        ,Name"
        strSql &= "        ,優先度"
        strSql &= "        ,疵マップシンボル"
        strSql &= " from " & DB_TYPE_NAME
        strSql &= " order by ID ASC "

        Try
            ' DB接続
            If Not tcls_DB.Open() Then Exit Function

            'SQL実行
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            SyncLock m_pLockObject

                m_dctDspTypeName.Clear()
                m_dctDspMapName.Clear()
                m_dctTypePriority.Clear()
                m_dctMapSymbol.Clear()

                While sqlRead.Read()

                    'マップ表示名称セット
                    m_dctDspMapName.Add(CInt(sqlRead.GetValue(0)), sqlRead.GetString(1))

                    '疵種名セット
                    m_dctDspTypeName.Add(CInt(sqlRead.GetValue(0)), sqlRead.GetString(2))

                    '優先度セット
                    m_dctTypePriority.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(3)))

                    '疵マップシンボルセット
                    m_dctMapSymbol.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(4)))

                End While

            End SyncLock

            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "TYPE_NAME取得失敗")
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' グレードマスタテーブル
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Public Function ReadGradeName() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
        Dim bRet As Boolean = False

        'SQL文作成
        strSql = ""
        strSql &= " select EdasysID"
        strSql &= "        ,マップ表示色"
        strSql &= "        ,Name"
        strSql &= "        ,優先度"
        strSql &= "        ,欠陥グレード"
        strSql &= "        ,疵マップシンボル表示色"
        strSql &= " from " & DB_GRADE_NAME
        strSql &= " order by ID ASC "

        Try
            ' DB接続
            If Not tcls_DB.Open() Then Exit Function

            ' SQL実行
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            SyncLock m_pLockObject

                m_dctDspGradeColor.Clear()
                m_dctDspGradeName.Clear()
                m_dctGradePriority.Clear()
                m_dctDefectGrade.Clear()
                m_dctMapSymbolColor.Clear()

                While sqlRead.Read()

                    'マップ表示色セット
                    m_dctDspGradeColor.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(1)))

                    'グレード名
                    m_dctDspGradeName.Add(CInt(sqlRead.GetValue(0)), sqlRead.GetString(2))

                    'グレード疵種情報
                    m_dctGradePriority.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(3)))

                    '欠陥グレード
                    m_dctDefectGrade.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(4)))

                    '疵マップシンボル表示色セット
                    m_dctMapSymbolColor.Add(CInt(sqlRead.GetValue(0)), CInt(sqlRead.GetValue(5)))

                End While

            End SyncLock

            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "GRADE_NAME取得失敗")
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

        Return bRet


    End Function

    ''' <summary>
    ''' 検査パターン別出力設定パラメータ読込（HY_DEFMAP用）
    ''' </summary>
    ''' <returns>復帰情報(True:読み込みOK,False:読み込みNG)</returns>
    ''' <remarks></remarks>
    Public Function ReadParam_OutPut() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String
        Dim bRet As Boolean = False
        Dim pcode As String = ""             'pcode
        Dim o_pcode As String = ""           '直線のpcode
        Dim cd As Integer = 0                'pcode(添え字変換後)
        Dim cnt As Integer = 0               'pcode毎の疵種EdasysID個数（添え字用）

        Dim ii As Integer
        Dim jj As Integer
        Dim id As Integer = 0                '疵種EdasysID


        'SQL文作成
        strSql = ""
        strSql &= " SELECT pcode, 疵種EdasysID, 入側単位疵点マップ画面表示 "
        strSql &= " from " & DB_PCODE_OUTPUT
        strSql &= " ORDER BY pcode, 疵種EdasysID "

        Try
            ' DB接続
            If Not tcls_DB.Open() Then Exit Function

            ' SQL実行
            sqlRead = tcls_DB.SelectExecute(strSql)
            If sqlRead Is Nothing Then Exit Try

            SyncLock m_pLockObject

                '保存用構造体初期化
                m_Pout.initialize()
                For ii = 0 To MAX_PCODE - 1
                    m_Pout.pcode(ii).initialize()
                    For jj = 0 To MAX_TYPE - 1
                        m_Pout.pcode(ii).type(jj).initialize()
                    Next jj
                    m_Pout.pcode(ii).def.initialize()
                Next ii

                While sqlRead.Read()

                    'PCODE読み込み
                    pcode = CStr(sqlRead.GetValue(0))
                    If pcode <> o_pcode Then    'PCODEが前回値と異なる場合
                        cd = ChangePcode(pcode) 'PCODEの値を添え字変換
                        If -1 = cd Then Continue While '-1の場合は次のレコードへ

                        'PCODE（添え字）を保存
                        m_Pout.pcode(cd).pcode = cd

                        'PCODE値を保存
                        o_pcode = pcode

                    End If

                    '疵種EdasysID読み込み
                    id = CInt(sqlRead.GetValue(1))
                    If id = -1 Then     '疵種EdasysIDが-1の場合はデフォルトに保存（IDと疵マップ表示）
                        m_Pout.pcode(cd).def.edasysID = id
                        m_Pout.pcode(cd).def.disp = CInt(sqlRead.GetValue(2))
                    Else                '通常保存（IDと疵マップ表示）
                        '疵種毎の個数を取得
                        cnt = m_Pout.pcode(cd).num
                        m_Pout.pcode(cd).type(cnt).edasysID = id
                        m_Pout.pcode(cd).type(cnt).disp = CInt(sqlRead.GetValue(2))
                        m_Pout.pcode(cd).num = cnt + 1
                    End If

                End While

            End SyncLock

            bRet = True

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "M_PCODE_OUTPUT取得失敗")
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try

        Return bRet


    End Function

    ''' <summary>
    ''' 疵種名を取得
    ''' </summary>
    ''' <param name="pcode">パターンコード</param>
    ''' <returns>疵種名</returns>
    ''' <remarks></remarks>
    Private Function ChangePcode(ByVal pcode As String) As Integer

        'パラメータチェック
        If 1 > Asc(pcode) Or MAX_PCODE < Asc(pcode) Then
            Return -1
        End If

        Return Asc(pcode) - 1

    End Function

    ''' <summary>
    ''' 画面表示対象の疵種か判定
    ''' </summary>
    ''' <param name="pcode">パターンコード</param>
    ''' <param name="tid">疵種EdasysID</param>
    ''' <param name="gid">グレードEdasysID</param>
    ''' <returns>疵種名</returns>
    ''' <remarks></remarks>
    Public Function Check_Output_DISP(ByVal pcode As String, ByVal tid As Integer, _
                                        ByVal gid As Integer) As Boolean

        Dim ii As Integer
        Dim pcd As Integer = 0      'PCODEの添え字変換用

        'PCODE添え字変換
        pcd = ChangePcode(pcode) 'PCODEの値を添え字変換

        '疵種一致を確認
        For ii = 0 To m_Pout.pcode(pcd).num - 1
            If m_Pout.pcode(pcd).type(ii).edasysID = tid Then
                '疵種一致で表示対象疵種EdasysIDが0の場合は対象外
                If m_Pout.pcode(pcd).type(ii).disp = 0 Then Return False
                '優先度比較
                If GetGradePriority(m_Pout.pcode(pcd).type(ii).disp) >= GetGradePriority(gid) Then
                    Return True
                Else
                    Return False
                End If
            End If
        Next ii

        '疵種が一致しない場合
        'デフォルト表示の表示対象疵種EdasysIDが0の場合は対象外
        If m_Pout.pcode(pcd).def.disp = 0 Then Return False
        '優先度比較
        If GetGradePriority(m_Pout.pcode(pcd).def.disp) >= GetGradePriority(gid) Then
            Return True
        Else
            Return False
        End If

    End Function


    ''' <summary>
    ''' 疵種名を取得
    ''' </summary>
    ''' <param name="nEdasysID">疵種EdasysID</param>
    ''' <returns>疵種名</returns>
    ''' <remarks></remarks>
    Public Function GetDspTypeName(ByVal nEdasysID As Integer) As String

        Dim strRet As String = ""

        If m_dctDspTypeName.ContainsKey(nEdasysID) Then
            strRet = m_dctDspTypeName(nEdasysID)
        End If
        Return strRet

    End Function

    ''' <summary>
    ''' マップ表示疵種名を取得
    ''' </summary>
    ''' <param name="nEdasysID">疵種EdasysID</param>
    ''' <returns>マップ表示疵種名</returns>
    ''' <remarks></remarks>
    Public Function GetDspMapName(ByVal nEdasysID As Integer) As String

        Dim strRet As String = ""

        If m_dctDspMapName.ContainsKey(nEdasysID) Then
            strRet = m_dctDspMapName(nEdasysID)
        End If

        Return strRet

    End Function
    ''' <summary>
    ''' 疵種優先度取得
    ''' </summary>
    ''' <param name="nEdasysID"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function getTypePriority(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 999

        If m_dctTypePriority.ContainsKey(nEdasysID) Then
            nRet = m_dctTypePriority(nEdasysID)
        End If

        Return nRet

    End Function

    ''' <summary>
    ''' マップ表示色を取得
    ''' </summary>
    ''' <param name="nEdasysID">グレードEdasysID</param>
    ''' <returns>マップ表示色</returns>
    ''' <remarks></remarks>
    Public Function GetDspGradeColor(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 0

        If m_dctDspGradeColor.ContainsKey(nEdasysID) Then
            nRet = m_dctDspGradeColor(nEdasysID)
        End If

        Return nRet

    End Function
    ''' <summary>
    ''' グレード名称取得
    ''' </summary>
    ''' <param name="nEdasysID">グレードEdasysID</param>
    ''' <returns>グレード名</returns>
    ''' <remarks></remarks>
    Public Function GetDspGradeName(ByVal nEdasysID As Integer) As String

        Dim strRet As String = ""

        If m_dctDspGradeName.ContainsKey(nEdasysID) Then
            strRet = m_dctDspGradeName(nEdasysID)
        End If

        Return strRet

    End Function
    ''' <summary>
    ''' グレード優先度取得
    ''' </summary>
    ''' <param name="nEdasysID">グレードEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetGradePriority(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 999

        If m_dctGradePriority.ContainsKey(nEdasysID) Then
            nRet = m_dctGradePriority(nEdasysID)
        End If

        Return nRet

    End Function

    ''' <summary>
    ''' 疵マップシンボル取得
    ''' </summary>
    ''' <param name="nEdasysID">疵種EdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetMapSymbol(ByVal nEdasysID As Integer) As String

        Dim strRet As String = ""
        Dim nSymbolIdx As Integer = 0

        If m_dctMapSymbol.ContainsKey(nEdasysID) Then
            nSymbolIdx = m_dctMapSymbol(nEdasysID)
            strRet = PLOT_SYMBOL_TYPE(nSymbolIdx)
        End If

        Return strRet
    End Function

    ''' <summary>
    ''' 疵マップシンボル表示色取得
    ''' </summary>
    ''' <param name="nEdasysID">グレードEdasysID</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function GetMapSymbolColor(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 0

        If m_dctMapSymbolColor.ContainsKey(nEdasysID) Then
            nRet = m_dctMapSymbolColor(nEdasysID)
        End If

        Return nRet
    End Function

    ''' <summary>
    ''' 優先度比較
    ''' </summary>
    ''' <param name="nPreTypeEdasysID">前回疵種EdasysID</param>
    ''' <param name="nNowTypeEdasysID">今回疵種EdasysID</param>
    ''' <param name="nPreGaradeEdasysID">前回グレードEdasysID</param>
    ''' <param name="nNowGradeEdasysID">今回グレードEdasysID</param>
    ''' <returns>復帰情報(True:優先度が高い,False:優先度が低い)</returns>
    ''' <remarks></remarks>
    Public Function CheckPriority(ByVal nPreTypeEdasysID As Integer, ByVal nNowTypeEdasysID As Integer, _
                                    ByVal nPreGaradeEdasysID As Integer, ByVal nNowGradeEdasysID As Integer) As Boolean



        '前回のグレードよりも優先度が小さければ、優先度が高い
        If GetGradePriority(nPreGaradeEdasysID) > GetGradePriority(nNowGradeEdasysID) Then

            Return True

        ElseIf GetGradePriority(nPreGaradeEdasysID) = GetGradePriority(nNowGradeEdasysID) Then

            'グレードの優先度が同じなら、疵種の優先度を比較し、優先度が小さければ、優先度が高い
            If getTypePriority(nPreTypeEdasysID) > getTypePriority(nNowTypeEdasysID) Then
                Return True
            End If

        End If

        Return False

    End Function


    ''' <summary>
    ''' 欠陥グレード取得
    ''' </summary>
    ''' <param name="nEdasysID">グレードEdasysID</param>
    ''' <returns>欠陥グレード(0:無害,1:軽欠陥,2:中欠陥,3:重欠陥)</returns>
    ''' <remarks></remarks>
    Public Function GetDefectGrade(ByVal nEdasysID As Integer) As Integer

        Dim nRet As Integer = 0

        If m_dctDefectGrade.ContainsKey(nEdasysID) Then
            nRet = m_dctDefectGrade(nEdasysID)
        End If

        Return nRet

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
        For Each nEdasyID As Integer In m_dctGradePriority.Keys

            If nWkPriority < m_dctGradePriority(nEdasyID) Then
                nWkEdasysID = nEdasyID
                nWkPriority = m_dctGradePriority(nEdasyID)
            End If

        Next

        Return nWkEdasysID

    End Function
End Class
