'*********************************************************************************
' エッジ検出履歴画面
'	[Ver]
'		Ver.01    2011/06/01  新規作成
'
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Imports System.IO

Public Class frmEdgeList
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////

    'エッジ検出実績構造体 
    Private Structure EDGE_RESULT_INF
        Dim strKizuKenNo As String          '管理No
        Dim strCoilNo As String             'コイル番号
        Dim strInDate As String             '登録日付

        Dim strMinItaHaba_O As String       '板幅最小(表)
        Dim strMaxItaHaba_O As String       '板幅最大(表)
        Dim strOutSideN_O As String         '板幅許容範囲外数(表)
        Dim strNgN_Left_O As String         'エッジ検出失敗個数_左(表)
        Dim strNgN_Right_O As String        'エッジ検出失敗個数_右(表)
        Dim strMinItaHaba_U As String       '板幅最小(裏)
        Dim strMaxItaHaba_U As String       '板幅最大(裏)
        Dim strOutSideN_U As String         '板幅許容範囲外数(裏)
        Dim strNgN_Left_U As String         'エッジ検出失敗個数_左(裏)
        Dim strNgN_Right_U As String        'エッジ検出失敗個数_右(裏)

        'イニシャライズ
        Public Sub initialize()
            strKizuKenNo = ""               '管理No
            strCoilNO = ""                  'コイル番号
            strInDate = ""                  '登録日付

            strMinItaHaba_O = ""            '板幅最小(表)
            strMaxItaHaba_O = ""            '板幅最大(表)
            strOutSideN_O = ""              '板幅許容範囲外数(表)
            strNgN_Left_O = ""              'エッジ検出失敗個数_左(表)
            strNgN_Right_O = ""             'エッジ検出失敗個数_右(表)

            strMinItaHaba_U = ""            '板幅最小(裏)
            strMaxItaHaba_U = ""            '板幅最大(裏)
            strOutSideN_U = ""              '板幅許容範囲外数(裏)
            strNgN_Left_U = ""              'エッジ検出失敗個数_左(裏)
            strNgN_Right_U = ""             'エッジ検出失敗個数_右(裏)
        End Sub

    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_proID As System.Diagnostics.Process = Nothing                 'ExcelのプロセスID

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEdgeList_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        'タイトル設定
        Call SetTitleName(Me)

        MyBase.SetButtonEnable(0, True)
        MyBase.SetButtonEnable(1, True)
        MyBase.SetButtonEnable(2, True)
        Me.TopMost = False

    End Sub
    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmEdgeList_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

    End Sub
    ''' <summary>
    ''' ファンクションボタン押下処理
    ''' </summary>
    ''' <param name="FncNo"></param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmEdgeList_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton

        Dim bRet As Boolean

        If 0 = FncNo Or 4 = FncNo Then

            'CSV作成
            Select Case FncNo
                Case 0  '表示
                    bRet = MakeCSV(0)
                Case 4  '保存
                    bRet = MakeCSV(1)
            End Select

            If Not bRet Then

                Dim strMsg As String

                strMsg = "CSVの作成に失敗しました。"
                '''' ガイダンス表示
                Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
                    frmMsg.ShowDialog()
                End Using
            End If
            Exit Sub
        End If

        Me.Close()

        '呼び元へ通知
        'RaiseEvent tEv_FunctionButton(FncNo, onKey)

    End Sub

    ''' <summary>
    ''' 日付,時間コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTimeStart.MouseWheel, dtpTimeEnd.MouseWheel, dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
        ' 現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub


    ''' <summary>
    ''' CSV作成
    ''' </summary>
    ''' <param name="nMode">0:表示,1:発行</param>
    ''' <returns>復帰情報(Ture:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function MakeCSV(ByVal nMode As Integer) As Boolean

        Dim strPath As String = ""                      'パス
        Dim lstData As New List(Of EDGE_RESULT_INF)     'エッジ検出実績データリスト
        Dim strDateStart As String                      '開始日付
        Dim strDateEnd As String                        '終了日付

        'リスト初期化
        lstData.Clear()
        strDateStart = Format(dtpDateStart.Value, "yyyy/MM/dd ") & Format(dtpTimeStart.Value, "HH:mm:ss")
        strDateEnd = Format(dtpDateEnd.Value, "yyyy/MM/dd ") & Format(dtpTimeEnd.Value, "HH:mm:ss")

        'データ取得
        If Not GetData(strDateStart, strDateEnd, lstData) Then Return False

        '''' フォルダの有無チェック
        tMod.CheckFolder(CSV_PATH)

        Select Case nMode

            Case 0  '表示
                '作業領域へ保存
                strPath = CSV_PATH & "\エッジ検出履歴画面.csv"
                strPath = IO.Path.GetFullPath(strPath)
            Case 1  '保存
                'TEMPフォルダへ保存
                strPath = CSV_PATH & "\エッジ検出履歴画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".csv"
                strPath = IO.Path.GetFullPath(strPath)

        End Select

        '同一ファイル読み取り専用解除
        Dim fi As FileInfo = New FileInfo(strPath)
        If IO.File.Exists(strPath) Then
            '読み取り専用解除
            fi.IsReadOnly = False
        End If

        ''CSVデータセット
        If Not SetData(strPath, lstData) Then Return False

        Select Case nMode

            Case 0  '表示
                '読み取り専用で表示
                fi.IsReadOnly = True

                If tClassLibrary.tClass.tClass_Excel_Base.Check_Install_Excel Then
                    m_proID = tClassLibrary.tClass.tClass_Excel_Base.Start_ExcelFile(strPath)
                Else
                    ' Execlがインストールされていない場合は、メモ帳で開く。
                    System.Diagnostics.Process.Start("notepad.exe", strPath)
                End If
                'm_proID = tClassLibrary.tClass.tClass_Excel_Base.Start_ExcelFile(strPath)

            Case 1  '保存

                Dim strMsg As String

                strMsg = strPath & "に保存しました。"
                '''' ガイダンス表示
                Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
                    frmMsg.ShowDialog()
                End Using
        End Select

        lstData.Clear()
        lstData = Nothing

        Return True

    End Function

    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <param name="strStartDate">開始日付</param>
    ''' <param name="strEndDate">終了日付</param>
    ''' <param name="lstData">エッジ検出実績リスト</param>
    ''' <returns>復帰情報(Ture:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strStartDate As String, ByVal strEndDate As String, _
                             ByRef lstData As List(Of EDGE_RESULT_INF)) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""
        Dim typData As EDGE_RESULT_INF = Nothing            'エッジ検出実績

        'DBオープン
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_EDGE_COIL_INF & "]")
            Return False
        End If

        'SQL文作成
        strSql = ""

        strSql &= " SELECT "
        strSql &= " R.管理No as 管理No, "
        strSql &= " ISNULL(C.生産No, '') as コイル番号, "
        strSql &= " ISNULL(R.登録日付, '') as 登録日付, "

        strSql &= " EO.板幅最小 as 表_板幅最小, "
        strSql &= " EO.板幅最大 as 表_板幅最大, "
        strSql &= " EO.板幅許容範囲外数 as 表_板幅許容範囲外数, "
        strSql &= " EO.エッジ検出失敗個数_左 as 表_エッジ検出失敗個数_左, "
        strSql &= " EO.エッジ検出失敗個数_右 as 表_エッジ検出失敗個数_右, "

        strSql &= " EU.板幅最小 as 裏_板幅最小, "
        strSql &= " EU.板幅最大 as 裏_板幅最大, "
        strSql &= " EU.板幅許容範囲外数 as 裏_板幅許容範囲外数, "
        strSql &= " EU.エッジ検出失敗個数_左 as 裏_エッジ検出失敗個数_左, "
        strSql &= " EU.エッジ検出失敗個数_右 as 裏_エッジ検出失敗個数_右 "

        strSql &= " FROM  " & DB_COIL_RESULT & " R "

        strSql &= " LEFT OUTER JOIN " & DB_COIL_INF & " C "
        strSql &= " ON C.管理No = R.管理No "

        strSql &= " LEFT OUTER JOIN "
        strSql &= " (SELECT * FROM " & DB_EDGE_COIL_INF & " WHERE 表裏区分=0 ) EO"
        strSql &= " ON EO.管理No = R.管理No "

        strSql &= " LEFT OUTER JOIN "
        strSql &= " (SELECT * FROM " & DB_EDGE_COIL_INF & " WHERE 表裏区分=1 ) EU"
        strSql &= " ON EU.管理No = R.管理No "

        strSql &= " WHERE 1 = 1"

        If Not chkALL.Checked Then
            strSql &= " AND NOT ( EO.管理No IS  NULL AND EU.管理No IS NULL ) "
        End If

        strSql &= " AND R.登録日付 >= '" & strStartDate & "'"

        strSql &= " AND R.登録日付 <= '" & strEndDate & "'"

        strSql &= " Order By R.登録日付 DESC "

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSql)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_EDGE_COIL_INF & "]")
            Return False
        End If

        Try

            While sqlRead.Read()

                typData.initialize()
                typData.strKizuKenNo = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("管理No")))
                typData.strCoilNo = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("コイル番号")))
                typData.strInDate = sqlRead.GetDateTime(sqlRead.GetOrdinal("登録日付")).ToString("yyyy/MM/dd HH:mm:ss")

                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("表_板幅最小")) Then
                    typData.strMinItaHaba_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("表_板幅最小")))
                End If

                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("表_板幅最大")) Then
                    typData.strMaxItaHaba_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("表_板幅最大")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("表_板幅許容範囲外数")) Then
                    typData.strOutSideN_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("表_板幅許容範囲外数")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("表_エッジ検出失敗個数_左")) Then
                    typData.strNgN_Left_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("表_エッジ検出失敗個数_左")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("表_エッジ検出失敗個数_右")) Then
                    typData.strNgN_Right_O = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("表_エッジ検出失敗個数_右")))
                End If

                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("裏_板幅最小")) Then
                    typData.strMinItaHaba_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("裏_板幅最小")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("裏_板幅最大")) Then
                    typData.strMaxItaHaba_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("裏_板幅最大")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("裏_板幅許容範囲外数")) Then
                    typData.strOutSideN_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("裏_板幅許容範囲外数")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("裏_エッジ検出失敗個数_左")) Then
                    typData.strNgN_Left_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("裏_エッジ検出失敗個数_左")))
                End If
                If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("裏_エッジ検出失敗個数_右")) Then
                    typData.strNgN_Right_U = CStr(sqlRead.GetValue(sqlRead.GetOrdinal("裏_エッジ検出失敗個数_右")))
                End If

                'リストへ追加
                lstData.Add(typData)

            End While

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "エッジ検出実績取得エラー [" & ex.Message & "]")
            Return False
        Finally

            tcls_DB.Cancel()
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing

        End Try

    End Function

    ''' <summary>
    ''' CSVへデータセット
    ''' </summary>
    ''' <param name="strPath">パス</param>
    ''' <param name="lstData">リスト</param>
    ''' <returns>復帰情報(Ture:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function SetData(ByVal strPath As String, ByVal lstData As List(Of EDGE_RESULT_INF)) As Boolean
        Dim strData As String = ""                      '1行のデータ

        '' CSV出力
        Using cls_Stram As New tClass.tClass_FileStream(strPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                ' オープン
                cls_Stram.Open()

                strData = ""
                'ヘッダー出力
                strData &= "管理No,"
                strData &= "コイル番号,"
                strData &= "登録日付,"

                strData &= "表_板幅最小,"
                strData &= "表_板幅最大,"
                strData &= "表_板幅許容範囲外数,"
                strData &= "表_エッジ検出失敗個数_左,"
                strData &= "表_エッジ検出失敗個数_右,"

                strData &= "裏_板幅最小,"
                strData &= "裏_板幅最大,"
                strData &= "裏_板幅許容範囲外数,"
                strData &= "裏_エッジ検出失敗個数_左,"
                strData &= "裏_エッジ検出失敗個数_右,"

                '最後の","を排除
                'strData = strData.Remove(strData.Length - 1, 1)

                cls_Stram.WriteLine(strData)


                'データ出力
                For ii As Integer = 0 To lstData.Count - 1
                    strData = ""
                    strData &= CStr(lstData(ii).strKizuKenNO) & ","
                    strData &= CStr(lstData(ii).strCoilNo) & ","
                    strData &= CStr(lstData(ii).strInDate) & ","

                    strData &= CStr(lstData(ii).strMinItaHaba_O) & ","
                    strData &= CStr(lstData(ii).strMaxItaHaba_O) & ","
                    strData &= CStr(lstData(ii).strOutSideN_O) & ","
                    strData &= CStr(lstData(ii).strNgN_Left_O) & ","
                    strData &= CStr(lstData(ii).strNgN_Right_O) & ","

                    strData &= CStr(lstData(ii).strMinItaHaba_U) & ","
                    strData &= CStr(lstData(ii).strMaxItaHaba_U) & ","
                    strData &= CStr(lstData(ii).strOutSideN_U) & ","
                    strData &= CStr(lstData(ii).strNgN_Left_U) & ","
                    strData &= CStr(lstData(ii).strNgN_Right_U) & ","

                    cls_Stram.WriteLine(strData)
                Next ii

                ' クローズ
                cls_Stram.Close()

            Catch ex As Exception
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "CSVデータセットエラー [" & ex.Message & "]")
                Return False
            End Try
        End Using

        Return True

    End Function

End Class
