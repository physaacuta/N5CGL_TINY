'*********************************************************************************
' 疵種・グレード設定画面クラス
'	[Ver]
'		Ver.01    2012/06/08  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary.tClass

Public Class frmTypeGr
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 疵種データグリッドビューの列
    Private Enum EM_DATACOLUMNTYPE
        emNo = 0                            ' テキストボックス No
        emTxt_EdasysID                      ' テキストボックス EdasysID
        emTxt_Name                          ' テキストボックス 疵種名称
        emTxt_Score                         ' テキストボックス Score
        emTxt_Map                           ' テキストボックス マップ表示名称
        emTxt_FFSendName                    ' テキストボックス FF伝送名称
        emCmb_ProGroup                      ' コンボボックス   プロコングループ
        emChk_PeriodProcMask                ' チェックボックス PeriodProcMask
        emBtn_MapSymbolColor                ' 疵マップシンボル表示色
        emCmb_MapSynbol                     ' コンボボックス   疵マップシンボル
        emCmb_Pictype1                      ' コンボボックス   切出画像種類1
        emCmb_Pictype2                      ' コンボボックス   切出画像種類2
        emInsButton                         ' 挿入ボタン
        emDelButton                         ' 削除ボタン
    End Enum

    ' グレードデータグリッドビューの列
    Private Enum EM_DATACOLUMNGRADE
        emNo = 0                            ' テキストボックス No
        emTxt_EdasysID                      ' テキストボックス EdasysID
        emTxt_Name                          ' テキストボックス グレード名称
        emTxt_Score                         ' テキストボックス Score
        emBtn_MapButton                     ' ボタン 探傷画面表示色
        emCmb_BadGrade                      ' コンボボックス   欠陥グレード
        emCmb_FFSendName                    ' FF伝送名称
        emCmb_ProGroup                      ' コンボボックス   プロコングループ
        emBtn_MapSynbolColor                ' ボタン 疵マップ表示色
        emTxt_NumLimit                      ' テキストボックス NumLimit
        emInsButton                         ' 挿入ボタン
        emDelButton                         ' 削除ボタン
    End Enum

    '削除対象のグレードEdasysIDと次に優先度の高いグレードEdasysIDを格納
    Private Structure GradeStruct
        Dim nEdasysID As Integer        ' 削除対象のグレードEdasysID
        Dim nNextEdasysID As Integer    ' 次に優先度の高いグレードEdasysID    
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_AdapterType As New SqlClient.SqlDataAdapter       ' アダプターType
    Private m_AdapterGrade As New SqlClient.SqlDataAdapter      ' アダプターGrade
    Private m_AdapterGradeWk As New SqlClient.SqlDataAdapter    ' アダプターGrade コンボボックス設定用
    Private m_DsType As New DataSet                             ' 非接続型テーブルTYPE_NAME
    Private m_DsGrade As New DataSet                            ' 非接続型テーブルGRADE_NAME
    Private m_DsGradeWk As New DataSet                          ' 非接続型テーブルGRADE コンボボックス設定用

    Private m_nSendScode As Integer                             ' メールで送信するScode
    Private m_nDelTypeEdasysID(MAX_TYPE - 1) As Integer         ' 削除対象の疵種EdasysID
    Private mtyp_Grade(MAX_GRADE - 1) As GradeStruct            ' 削除対象のグレードEdasysIDと次にScoreの高いEdasysID格納用

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メニューボタン
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' ※0オリジン
            Case 6                              ' F07 画面印刷
                Call PrintOut()
            Case 7                              ' F08 画面保存
                Call HardCopy(IMG_PATH, "疵種・グレード設定画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
            Case 8
                modMain.ChgProject()            ' F09 KA_PARCOLORへ切替処理
            Case 10
                btnF11.Select()                 ' F11を選択状態にする dgvが書き込み中の時に、入力完了になるように
                subData_UpDate()                ' 設定
            Case 11
                modMain.TaskEnd()               ' F12 終了
        End Select
    End Sub

    ''' <summary>
    ''' 画面変更要求されたときの処理 (継承元クラスよりコールされる)
    ''' </summary>
    ''' <param name="frmID">選択画面ID</param>
    ''' <param name="cancel">選択を取りやめる場合は、Trueをセット</param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_tEv_SelectMenuList(ByVal frmID As Integer, ByRef cancel As Boolean) Handles Me.tEv_SelectMenuList
        ' いろいろ開放処理
        m_DsType.Dispose()
        If Not m_AdapterType Is Nothing Then
            m_AdapterType.Dispose()
            m_AdapterType = Nothing
        End If

        m_DsGrade.Dispose()
        If Not m_AdapterGrade Is Nothing Then
            m_AdapterGrade.Dispose()
            m_AdapterGrade = Nothing
        End If

        m_DsGradeWk.Dispose()
        If Not m_AdapterGradeWk Is Nothing Then
            m_AdapterGradeWk.Dispose()
            m_AdapterGradeWk = Nothing
        End If

        ' 画面切替
        ShowForm(frmID)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メソッド定義
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        modMain.SetListMenu(Me)                                 ' メニューリスト生成
        modMain.SetLabelSysName(Me)                             ' システム名ラベル生成
        MyBase.tProp_MyFrmID = EM_FRM_ID.emfrmTypeGr            ' 疵種・グレード設定画面呼出

        'タイトル設定
        Call SetTitleName(Me)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        'グレードdgv初期化
        dgvInitializeDefGrade(dgvGrade)

        ' このフォームで必要なデータを取得する
        If Not DB_Select() Then
            ' DB 失敗なので、画面強制終了
            frmMsg.SetSelectedFNo(0)

            frmMsg.Text = "異常"
            frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Red.ToArgb, True, False)
            frmMsg.ShowDialog()
            modMain.TaskEnd()                       '終了
        End If

        'コンボボックス用GRADE_NAMEを編集
        GradeData_Init()
        '疵種dgv初期化 GRADE_NAMEを使用する為、データ取得後に実施
        dgvInitializeDefType(dgvType)

        ' データのレコード数を初期化
        Data_Init()

        ' 画面に値を表示
        Data_Disp()

        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvType.DataGridViewSelectionRowColorChange(g_ColorSelection)     ' 疵種dgv
        dgvGrade.DataGridViewSelectionRowColorChange(g_ColorSelection)    ' グレードdgv

        ' 更新ボタンの有効/無効
        Call UpdateEnabled(g_bPassFlg)

        'プリンタがなければ、画面印刷無効
        If Not tClass_HeadCopy.IsCheckPrinter Then
            btnF07.Enabled = False
        Else
            btnF07.Enabled = True
        End If

        frmMsg.Dispose()
        frmMsg = Nothing

    End Sub
    ''' <summary>
    ''' ディスポーズ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmTypeGr_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed

        '現在の表示中の画面が自分の画面ならアプリケーションを終了
        If g_nDspfrmID = MyBase.tProp_MyFrmID Then modMain.TaskEnd()

    End Sub

    ''' <summary>
    ''' データの取得
    ''' </summary>
    ''' <returns>復帰情報(True:正常,False:異常)</returns>
    ''' <remarks></remarks>
    Private Function DB_Select() As Boolean

        Dim strSQL As String = ""
        Dim strErrMsg As String = ""

        ' SQL文生成
        strSQL = "SELECT EdasysID"
        strSQL &= ", ID"
        strSQL &= ", Name"
        strSQL &= ", 優先度"
        strSQL &= ", マップ表示名称"
        strSQL &= ", FF伝送名称"
        strSQL &= ", プロコングループ"
        strSQL &= ", Score"
        strSQL &= ", PeriodProcMask"
        strSQL &= ", 疵マップシンボル表示色"
        strSQL &= ", 疵マップシンボル"
        strSQL &= ", 切出画像種類1"
        strSQL &= ", 切出画像種類2"

        strSQL &= " FROM " & DB_TYPE_NAME
        strSQL &= " ORDER BY ID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsType, m_AdapterType, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_TYPE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        ' SQL文生成
        strSQL = "SELECT EdasysID"
        strSQL &= ", ID"
        strSQL &= ", Name"
        strSQL &= ", 優先度"
        strSQL &= ", マップ表示色"
        strSQL &= ", 欠陥グレード"
        strSQL &= ", FF伝送名称"
        strSQL &= ", プロコングループ"
        strSQL &= ", 疵マップシンボル表示色"
        strSQL &= ", Score"
        strSQL &= ", NumLimit"

        strSQL &= " FROM " & DB_GRADE_NAME
        strSQL &= " ORDER BY ID ASC"

        'SQL文実行
        If 0 > MyBase.Get_SelectData(strSQL, m_DsGrade, m_AdapterGrade, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        'SQL文実行(表示用)
        If 0 > MyBase.Get_SelectData(strSQL, m_DsGradeWk, m_AdapterGradeWk, strErrMsg) Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, DB_GRADE_NAME & " 読込エラー [ " & strErrMsg & "] " & Me.Text)
            Return False
        End If

        Return True

    End Function

    ''' <summary>
    ''' 疵種マスター、グレードマスター、OUTPUT_PCODE、OUTPUT_SCODEテーブルを編集する
    ''' </summary>
    ''' <returns>正常:異常 （TRUE:FALSE）</returns>
    ''' <remarks>疵種マスター、グレードマスターテーブルはデリートインサートのため全削除、
    '''          OUTPUT_PCODE、OUTPUT_SCODEは関連レコードを編集</remarks>
    Private Function DB_Delete() As Boolean
        Dim tcls_DB As tClassLibrary.tClass.tClass_SQLServer = Nothing              ' DB接続
        tcls_DB = New tClassLibrary.tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)

        Dim sqlReader As SqlClient.SqlDataReader = Nothing   'DataReaderオブジェクト
        Dim strSql As String                                 'Sql文

        Dim strPcodeCol() As String
        Dim strScodeCol() As String
        Dim strPallOutCol() As String
        Dim strCommMasterCol() As String

        Try
            '''' データベースオープン
            If Not tcls_DB.Open() Then Return False

            'PCODE_OUTPUTの列を取得
            strSql = "SELECT TOP 1 * "
            strSql &= " FROM " & DB_PCODE_OUTPUT

            'レコードセット
            sqlReader = tcls_DB.SelectExecute(strSql)

            ReDim strPcodeCol(sqlReader.FieldCount - 1)

            For ii As Integer = 0 To strPcodeCol.Length - 1
                strPcodeCol(ii) = sqlReader.GetName(ii)
            Next ii

            sqlReader.Close()

            'SCODE_OUTPUTの列を取得
            strSql = "SELECT TOP 1 * "
            strSql &= " FROM " & DB_SCODE_OUTPUT

            'レコードセット
            sqlReader = tcls_DB.SelectExecute(strSql)

            ReDim strScodeCol(sqlReader.FieldCount - 1)

            For ii As Integer = 0 To strScodeCol.Length - 1
                strScodeCol(ii) = sqlReader.GetName(ii)
            Next ii

            sqlReader.Close()

            'PALL_OUTPUTの列名を取得
            strSql = "SELECT TOP 1 * "
            strSql &= " FROM " & DB_PALL_OUTPUT

            'レコードセット
            sqlReader = tcls_DB.SelectExecute(strSql)

            ReDim strPallOutCol(sqlReader.FieldCount - 1)

            For ii As Integer = 0 To strPallOutCol.Length - 1
                strPallOutCol(ii) = sqlReader.GetName(ii)
            Next ii

            sqlReader.Close()

            'COMMON_MASTERの列名を取得
            strSql = "SELECT TOP 1 * "
            strSql &= " FROM " & DB_COMMON_MASTER

            'レコードセット
            sqlReader = tcls_DB.SelectExecute(strSql)

            ReDim strCommMasterCol(sqlReader.FieldCount - 1)

            For ii As Integer = 0 To strCommMasterCol.Length - 1
                strCommMasterCol(ii) = sqlReader.GetName(ii)
            Next ii

            sqlReader.Close()

            'トランザクション開始
            tcls_DB.TransactStart()

            ' 削除対象の疵種EdasysIDがあるときに、関連するDBを変更
            For ii As Integer = 0 To m_nDelTypeEdasysID.Length - 1
                ' EdasysIDが1より小さければ次
                If m_nDelTypeEdasysID(ii) < 1 Then Continue For

                ' 疵種EdasysIDに関連するDBを変更
                ' PCODE_OUTPUTの対象の疵種EdasysIDがある行を削除する
                'sql作成
                strSql = "DELETE FROM " & DB_PCODE_OUTPUT & " WHERE "
                strSql &= "疵種EdasysID = " & m_nDelTypeEdasysID(ii)
                'SQL文の発行
                If 0 > tcls_DB.DMLExecute(strSql) Then
                    ' エラーならロールバックして終了
                    tcls_DB.TransactRollback()
                    Return False
                End If

                ' SCODE_OUTPUTを検索し変更対象のscodeを取得する
                ' 送信Scodeがすでに全部なら検索しない
                If m_nSendScode <> CH_SCODE_ALL Then
                    'sql生成
                    strSql = "SELECT scode FROM " & DB_SCODE_OUTPUT & " WHERE "
                    strSql &= "疵種EdasysID = " & m_nDelTypeEdasysID(ii)

                    'レコードセット
                    sqlReader = tcls_DB.SelectExecute(strSql)
                    'scodeを取得()
                    While sqlReader.Read()
                        ' 送信Scodeと、変更対象のScodeが異なるか？
                        If m_nSendScode <> CInt(sqlReader.GetValue(0)) Then
                            ' 送信Scodeが0か？
                            If m_nSendScode = 0 Then
                                ' 0なら変更対象のScodeを入れる
                                m_nSendScode = CInt(sqlReader.GetValue(0))
                            Else
                                ' 変更対象のScodeが複数あるなら 送信Scodeを全部に
                                m_nSendScode = CH_SCODE_ALL
                                Exit While
                            End If
                        End If
                    End While
                    sqlReader.Close()
                End If

                ' SCODE_OUTPUTの対象の疵種EdasysIDがある行を削除する
                ' sql作成
                strSql = "DELETE FROM " & DB_SCODE_OUTPUT & " WHERE "
                strSql &= "疵種EdasysID = " & m_nDelTypeEdasysID(ii)
                'SQL文の発行
                If 0 > tcls_DB.DMLExecute(strSql) Then
                    ' エラーならロールバックして終了
                    tcls_DB.TransactRollback()
                    Return False
                End If

                ' 疵種EdasysIDに関連するDBを変更
                ' PALL_OUTPUTの対象の疵種EdasysIDがある行を削除する
                'sql作成
                strSql = "DELETE FROM " & DB_PALL_OUTPUT & " WHERE "
                strSql &= "疵種EdasysID = " & m_nDelTypeEdasysID(ii)
                'SQL文の発行
                If 0 > tcls_DB.DMLExecute(strSql) Then
                    ' エラーならロールバックして終了
                    tcls_DB.TransactRollback()
                    Return False
                End If
            Next ii

            'Scoreが大きい順(同じ場合はIDが小さい順)に優先度を1～設定するために、Score,IDで並び替える
            Dim drowType() As DataRow = m_DsType.Tables(0).Select("", "Score desc, ID")

            ' 削除対象のグレードEdasysIDがあるときに、関連するDBを変更
            For ii As Integer = 0 To mtyp_Grade.Length - 1
                ' EdasysIDが1より小さければ次
                If mtyp_Grade(ii).nEdasysID < 1 Then Continue For
                ' PCODE_OUTPUTのGrEdasysIDを変更
                ' GrEdasysIDを使用している項目数繰り返す
                For kk As Integer = 0 To strPcodeCol.Length - 1

                    '列名がpcodeまたは、疵種EdasysIDの場合は更新しないので次へ
                    If strPcodeCol(kk) = "pcode" Or strPcodeCol(kk) = "疵種EdasysID" Then Continue For

                    '''' SQL文の生成  削除対象と同じGrEdasysIDの項目を次に強いGrEdasysIDにする
                    strSql = "UPDATE " & DB_PCODE_OUTPUT & " SET "
                    strSql &= strPcodeCol(kk) & " = " & mtyp_Grade(ii).nNextEdasysID
                    strSql &= " WHERE " & strPcodeCol(kk) & " = " & mtyp_Grade(ii).nEdasysID
                    'SQL文の発行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                Next kk

                ' PCODE_OUTPUTを検索し変更対象のscodeを取得する
                ' 送信scodeがすでに全部なら検索しない
                If m_nSendScode <> CH_SCODE_ALL Then
                    'sql生成
                    strSql = "SELECT scode FROM " & DB_SCODE_OUTPUT
                    strSql &= " WHERE " & strScodeCol(0) & " = " & mtyp_Grade(ii).nEdasysID

                    'レコードセット
                    sqlReader = tcls_DB.SelectExecute(strSql)

                    'scodeを取得()
                    While sqlReader.Read()
                        ' 送信Scodeと、変更対象のScodeが異なるか？
                        If m_nSendScode <> CInt(sqlReader.GetValue(0)) Then
                            ' 送信Scodeが0か？
                            If m_nSendScode = 0 Then
                                ' 0なら変更対象のScodeを入れる
                                m_nSendScode = CInt(sqlReader.GetValue(0))
                            Else
                                ' 変更対象のScodeが複数あるなら 送信Scodeを全部に
                                m_nSendScode = CH_SCODE_ALL
                                Exit While
                            End If
                        End If
                    End While
                    sqlReader.Close()
                End If

                ' SCODE_OUTPUTのGrEdasysIDを変更
                ' GrEdasysIDを使用している項目数繰り返す
                For kk As Integer = 0 To strScodeCol.Length - 1

                    '列名がscodeまたは、疵種EdasysIDの場合は更新しないので、次へ
                    If strScodeCol(kk) = "scode" Or strScodeCol(kk) = "疵種EdasysID" Then Continue For

                    '''' SQL文の生成  削除対象と同じGrEdasysIDの項目を次に強いGrEdasysIDにする
                    strSql = "UPDATE " & DB_SCODE_OUTPUT & " SET "
                    strSql &= strScodeCol(kk) & " = " & mtyp_Grade(ii).nNextEdasysID
                    strSql &= " WHERE " & strScodeCol(kk) & " = " & mtyp_Grade(ii).nEdasysID
                    'SQL文の発行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                Next kk

                ' PALL_OUTPUTのGrEdasysIDを変更
                ' GrEdasysIDを使用している項目数繰り返す
                For kk As Integer = 0 To strPallOutCol.Length - 1

                    '列名がpcodeまたは、疵種EdasysIDの場合は更新しないので次へ
                    If strPallOutCol(kk) = "pcode" Or strPallOutCol(kk) = "疵種EdasysID" Then Continue For

                    '''' SQL文の生成  削除対象と同じGrEdasysIDの項目を次に強いGrEdasysIDにする
                    strSql = "UPDATE " & DB_PALL_OUTPUT & " SET "
                    strSql &= strPallOutCol(kk) & " = " & mtyp_Grade(ii).nNextEdasysID
                    strSql &= " WHERE " & strPallOutCol(kk) & " = " & mtyp_Grade(ii).nEdasysID
                    'SQL文の発行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If
                Next kk
            Next ii

            ' 疵種マスターテーブルの全項目を削除
            strSql = "DELETE FROM " & DB_TYPE_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            For ii As Integer = 0 To drowType.Length - 1
                With drowType(ii)

                    'TYPE_NAME登録SQL文作成
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_TYPE_NAME & " VALUES ("
                    strSql &= .Item("EdasysID").ToString & ","                  ' EdasysID
                    strSql &= .Item("ID").ToString & ","                        ' ID
                    strSql &= "'" & .Item("Name").ToString & "',"               ' Name
                    strSql &= (ii + 1) & ","                                    ' 優先度
                    strSql &= "'" & .Item("マップ表示名称").ToString & "',"     ' マップ表示名称
                    strSql &= "'" & .Item("FF伝送名称").ToString & "',"         ' FF伝送名称
                    strSql &= .Item("プロコングループ").ToString & ","          ' プロコングループ
                    strSql &= .Item("Score").ToString & ","                     ' Score
                    strSql &= .Item("PeriodProcMask").ToString & ","            ' PeriodProcMask
                    strSql &= .Item("疵マップシンボル表示色").ToString & ","    ' 疵マップシンボル表示色
                    strSql &= .Item("疵マップシンボル").ToString & ","          ' 疵マップシンボル
                    strSql &= .Item("切出画像種類1").ToString & ","             ' 切出画像種類1
                    strSql &= .Item("切出画像種類2").ToString & ")"             ' 切出画像種類2

                    'SQL実行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If

                End With
            Next ii

            ' グレードマスタテーブルの全項目を削除
            strSql = "DELETE FROM " & DB_GRADE_NAME
            If 0 > tcls_DB.DMLExecute(strSql) Then
                ' エラーならロールバックして終了
                tcls_DB.TransactRollback()
                Return False
            End If

            'Scoreが大きい順(同じ場合はIDが小さい順)に優先度を1～設定するために、Score,IDで並び替える
            Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select("", "Score desc, ID")
            For ii As Integer = 0 To drowGrade.Length - 1
                With drowGrade(ii)

                    'GRADE_NAME登録SQL文作成
                    strSql = ""
                    strSql &= "INSERT INTO " & DB_GRADE_NAME & " VALUES ("
                    strSql &= .Item("EdasysID").ToString & ","                      ' EdasysID
                    strSql &= .Item("ID").ToString & ","                            ' ID
                    strSql &= "'" & .Item("Name").ToString & "',"                   ' Name
                    strSql &= (ii + 1) & ","                                        ' 優先度
                    strSql &= .Item("マップ表示色").ToString & ","                  ' マップ表示色
                    strSql &= .Item("欠陥グレード").ToString & ","                  ' 欠陥グレード
                    strSql &= .Item("FF伝送名称").ToString & ","                    ' FF伝送名称
                    strSql &= .Item("プロコングループ").ToString & ","              ' FF伝送名称
                    strSql &= .Item("疵マップシンボル表示色").ToString() & ","      ' 疵マップシンボル表示色
                    strSql &= .Item("Score").ToString & ","                         ' Score
                    strSql &= .Item("NumLimit").ToString & ")"                      ' NumLimit

                    'SQL実行
                    If 0 > tcls_DB.DMLExecute(strSql) Then
                        ' エラーならロールバックして終了
                        tcls_DB.TransactRollback()
                        Return False
                    End If

                End With
            Next ii

            'コミット
            tcls_DB.TransactCommint()

            Return True
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, " 疵種・グレード登録エラー [" & ex.Message & "] " & Me.Text)
            ' エラーならロールバックして終了
            tcls_DB.TransactRollback()
            Return False
        Finally
            '''' 後始末
            If Not sqlReader Is Nothing Then
                sqlReader.Close()
            End If
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
    End Function


    ''' <summary>
    ''' 削除対象のDBのEdasysIDとScoreから、次にScoreの高いEdasysIDをdgvGradeから検索する
    ''' </summary>
    ''' <param name="nEdasysID">削除対象のEdasysID</param>
    ''' <param name="nScore">Score</param>
    ''' <returns>次にScoreの高いEdasysID（無ければ０）</returns>
    ''' <remarks></remarks>
    Private Function Data_SearchNextGr(ByVal nEdasysID As Integer, ByVal nScore As Integer) As Integer
        Dim nNextScore As Integer              ' 次に強いScore
        Dim nNextGrEdasysID As Integer         ' 次に強いEdasyssID

        ' 検索用の初期化
        nNextScore = -1
        nNextGrEdasysID = 0               ' 次に強いEdasysIDがなければそのままこの値となる

        '次に強いScoreのGrEdasysIDを取得、無ければ０になる
        For ii As Integer = 0 To dgvGrade.Rows.Count - 1
            ' EdasysIDが空白でない行が対象
            If CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value) = "" Then Continue For

            ' 削除対象行のScoreより弱くて（同じ値でも可）、その中で一番強い行を検索
            If nScore >= CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value) And _
             CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value) > nNextScore Then
                ' EdasysIDとScoreを取得
                nNextGrEdasysID = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value)
                nNextScore = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value)
            End If
        Next ii
        Return nNextGrEdasysID
    End Function


    ''' <summary>
    ''' メンバ変数の初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Init()
        ' 送信するscodeを初期化
        m_nSendScode = 0

        ' 削除対象の疵種EdasysID配列の初期化
        For ii As Integer = 0 To m_nDelTypeEdasysID.Length - 1
            m_nDelTypeEdasysID(ii) = 0
        Next ii

        ' 削除対象のグレードEdasysID配列の初期化
        For ii As Integer = 0 To mtyp_Grade.Length - 1
            mtyp_Grade(ii).nEdasysID = 0
            mtyp_Grade(ii).nNextEdasysID = 0
        Next ii
    End Sub

    ''' <summary>
    ''' dgvTypeにデータ行を挿入
    ''' </summary>
    ''' <param name="nRowIndex">行の挿入位置</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_TypeData(ByVal nRowIndex As Integer)
        ' dgvTypeにレコードを挿入
        dgvType.Rows.Insert(nRowIndex)

        ' データ初期化
        dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, nRowIndex).Value = ""                             ' 疵種EdasysId
        dgvType(EM_DATACOLUMNTYPE.emTxt_Name, nRowIndex).Value = ""                                 ' 疵種名称
        dgvType(EM_DATACOLUMNTYPE.emTxt_Score, nRowIndex).Value = ""                                ' Score
        dgvType(EM_DATACOLUMNTYPE.emTxt_Map, nRowIndex).Value = ""                                  ' マップ表示名称
        dgvType(EM_DATACOLUMNTYPE.emTxt_FFSendName, nRowIndex).Value = ""                           ' FF伝送名称
        dgvType(EM_DATACOLUMNTYPE.emCmb_ProGroup, nRowIndex).Value = "無し"                         ' プロコングループ
        dgvType(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, nRowIndex).Value = 0                        ' PeriodProcMask
        dgvType(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, nRowIndex).Style.BackColor = Color.White    ' 疵マップシンボル表示色
        dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, nRowIndex).Value = "・"                          ' 疵マップシンボル
        dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, nRowIndex).Value = "初期定数"                     ' 切出画像種類1
        dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, nRowIndex).Value = "初期定数"                     ' 切出画像種類2
    End Sub

    ''' <summary>
    ''' dgcGradeにデータ行を挿入
    ''' </summary>
    ''' <param name="nRowIndex">行の挿入位置</param>
    ''' <remarks></remarks>
    Private Sub InsertAt_GradeData(ByVal nRowIndex As Integer)
        ' dgvレコードを挿入
        dgvGrade.Rows.Insert(nRowIndex)

        ' データ初期化
        dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, nRowIndex).Value = ""                           ' GrEdasysID
        dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, nRowIndex).Value = ""                               ' グレード名称
        dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, nRowIndex).Value = ""                              ' Score
        dgvGrade(EM_DATACOLUMNGRADE.emBtn_MapButton, nRowIndex).Style.BackColor = Color.White       ' マップ表示色に白色を
        dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, nRowIndex).Value = "無害"                       ' 欠陥グレード表示
        dgvGrade(EM_DATACOLUMNGRADE.emCmb_FFSendName, nRowIndex).Value = "無し"                     ' FF伝送名称
        dgvGrade(EM_DATACOLUMNGRADE.emCmb_ProGroup, nRowIndex).Value = "無し"                       ' プロコングループ
        dgvGrade(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, nRowIndex).Style.BackColor = Color.White  ' 疵マップシンボル表示色を白色
        dgvGrade(EM_DATACOLUMNGRADE.emTxt_NumLimit, nRowIndex).Value = ""                           ' NumLimit
    End Sub

    ''' <summary>
    ''' DBの情報を画面に表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Disp()
        '疵種のデータを表示
        Data_DispDgvType(dgvType)
        '疵種のインデックスとボタンの表示設定
        InitIndexDgvType(dgvType)

        'グレードのデータを表示
        Data_DispDgvGrade(dgvGrade)
        'グレードのインデックスとボタンの表示設定
        InitIndexDgvGrade(dgvGrade)

        ' 行の色を変える （初期表示で色が変わる対策）
        dgvType.CurrentRow.DefaultCellStyle.BackColor = Color.Empty
        dgvGrade.CurrentRow.DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' 疵種dgvにデータを表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispDgvType(ByVal dgv As DataGridView)
        Dim drowType() As DataRow = m_DsType.Tables(0).Select()           ' 全レコードを表示するためWHERE句なし

        '行を初期化
        dgv.Rows.Clear()

        ' 画面に表示
        dgv.RowCount = MAX_TYPE                   ' 疵種dgvのレコード数は固定
        For ii As Integer = 0 To dgv.RowCount - 1
            ' 疵種非接続テーブルの値がある間はそれを入れる。無ければ初期値
            If ii < drowType.Length Then
                ' EdasysIDがNULLの時は空白
                If drowType(ii).Item("EdasysID") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value = drowType(ii).Item("EdasysID")
                End If

                ' 疵種名称がNULLの時は空白
                If drowType(ii).Item("Name") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = drowType(ii).Item("Name")
                End If

                ' マップ表示名称がNULLのときは空白
                If drowType(ii).Item("マップ表示名称") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value = drowType(ii).Item("マップ表示名称")
                End If

                'FF伝送名称 がNULLのときは空白
                If drowType(ii).Item("FF伝送名称") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value = drowType(ii).Item("FF伝送名称")
                End If

                ' プロコングループの値がNULLか0
                If (drowType(ii).Item("プロコングループ") Is System.DBNull.Value) OrElse
                    0 = CInt(drowType(ii).Item("プロコングループ")) Then
                    ' プロコングループの値がNULLか0ならリストボックスに"無し"を表示する
                    dgv(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value = "無し"
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value = CStr(drowType(ii).Item("プロコングループ"))
                End If

                'Score がNULLのときは空白
                If drowType(ii).Item("Score") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value = drowType(ii).Item("Score")
                End If

                'PeriodProcMaskの登録
                dgv(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, ii).Value = drowType(ii).Item("PeriodProcMask")

                ' 疵マップシンボル表示色の値がNULLの時は白色
                If drowType(ii).Item("疵マップシンボル表示色") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, ii).Style.BackColor = Color.White            ' 白色
                Else
                    '設定されている色を表示
                    dgv(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, ii).Style.BackColor =
                         Color.FromArgb(CInt(drowType(ii).Item("疵マップシンボル表示色")))          ' 疵マップシンボル表示色
                End If

                ' 疵マップシンボル
                If drowType(ii).Item("疵マップシンボル").ToString.Trim = "0" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "・"
                ElseIf drowType(ii).Item("疵マップシンボル").ToString = "1" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "●"
                ElseIf drowType(ii).Item("疵マップシンボル").ToString = "2" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "■"
                ElseIf drowType(ii).Item("疵マップシンボル").ToString = "3" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "▲"
                ElseIf drowType(ii).Item("疵マップシンボル").ToString = "4" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "▼"
                ElseIf drowType(ii).Item("疵マップシンボル").ToString = "5" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "◆"
                ElseIf drowType(ii).Item("疵マップシンボル").ToString = "6" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "★"
                ElseIf drowType(ii).Item("疵マップシンボル").ToString = "7" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "＊"
                ElseIf drowType(ii).Item("疵マップシンボル").ToString = "8" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "×"
                ElseIf drowType(ii).Item("疵マップシンボル").ToString = "9" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "＋"
                Else
                    dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "・"
                End If

                ' 切出画像種類1
                If drowType(ii).Item("切出画像種類1").ToString.Trim = "0" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "初期定数"
                ElseIf drowType(ii).Item("切出画像種類1").ToString = "1" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "正画像"
                ElseIf drowType(ii).Item("切出画像種類1").ToString = "2" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "乱画像"
                ElseIf drowType(ii).Item("切出画像種類1").ToString = "3" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "乱画像2"
                ElseIf drowType(ii).Item("切出画像種類1").ToString = "4" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "乱画像3"
                Else
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "初期定数"
                End If

                ' 切出画像種類2
                If drowType(ii).Item("切出画像種類2").ToString.Trim = "0" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "初期定数"
                ElseIf drowType(ii).Item("切出画像種類2").ToString = "1" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "正画像"
                ElseIf drowType(ii).Item("切出画像種類2").ToString = "2" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "乱画像"
                ElseIf drowType(ii).Item("切出画像種類2").ToString = "3" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "乱画像2"
                ElseIf drowType(ii).Item("切出画像種類2").ToString = "4" Then
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "乱画像3"
                Else
                    dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "初期定数"
                End If

            Else
                ' データが無いので初期化
                dgv(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value = ""                            ' 疵種EdasysId
                dgv(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = ""                                ' 疵種名称
                dgv(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value = ""                               ' Score
                dgv(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value = ""                                 ' マップ表示名称
                dgv(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value = ""                          ' FF伝送名称
                dgv(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value = "無し"                        ' プロコン疵グループに"無し"を表示する
                dgv(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, ii).Value = 0                       ' PeriodProcMask
                dgv(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, ii).Style.BackColor = Color.White   ' 疵マップシンボル表示色に白色を
                dgv(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value = "・"                         ' 疵マップシンボル
                dgv(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value = "初期定数"                    ' 切出画像種類1
                dgv(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value = "初期定数"                    ' 切出画像種類2
            End If
        Next ii

        ' カーソルスタート位置は入力エリアの先頭
        dgv(1, 0).Selected = True
    End Sub

    ''' <summary>
    ''' グレードdgvにデータを表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_DispDgvGrade(ByVal dgv As DataGridView)
        Dim drowGrade() As DataRow = m_DsGrade.Tables(0).Select()          ' 全レコードを表示するためWHERE句なし

        '行を初期化
        dgv.Rows.Clear()

        ' 画面に表示
        dgv.RowCount = MAX_GRADE                  ' グレードdgvのレコード数は固定
        For ii As Integer = 0 To dgv.RowCount - 1
            If ii < drowGrade.Length Then
                ' GrEdasysIDがNULLの時は空白
                If drowGrade(ii).Item("EdasysID") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value = drowGrade(ii).Item("EdasysID")
                End If

                'グレード名称がNULLの時は空白
                If drowGrade(ii).Item("Name") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = drowGrade(ii).Item("Name")
                End If

                ' マップ表示色の値がNULLの時は白色
                If drowGrade(ii).Item("マップ表示色") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emBtn_MapButton, ii).Style.BackColor = Color.White    ' 白色
                Else
                    '設定されている色を表示
                    dgv(EM_DATACOLUMNGRADE.emBtn_MapButton, ii).Style.BackColor =
                      Color.FromArgb(CInt(drowGrade(ii).Item("マップ表示色")))                   ' マップ表示色
                End If

                ' 欠陥グレード
                If drowGrade(ii).Item("欠陥グレード").ToString.Trim = "0" Then
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "無害"
                ElseIf drowGrade(ii).Item("欠陥グレード").ToString = "1" Then
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "軽欠陥"
                ElseIf drowGrade(ii).Item("欠陥グレード").ToString = "2" Then
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "中欠陥"
                ElseIf drowGrade(ii).Item("欠陥グレード").ToString = "3" Then
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "重欠陥"
                Else
                    dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "無害"
                End If

                ' FF伝送名称の値がNULLか0
                If (drowGrade(ii).Item("FF伝送名称") Is System.DBNull.Value) OrElse
                    0 = CInt(drowGrade(ii).Item("FF伝送名称")) Then
                    ' 値がNULLか0ならリストボックスに"無し"を表示する
                    dgv(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value = "無し"
                Else
                    ' 値が0以外ならそのまま
                    dgv(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value = CStr(drowGrade(ii).Item("FF伝送名称"))  ' FF伝送名称
                End If

                ' プロコングループの値がNULLか0
                If (drowGrade(ii).Item("プロコングループ") Is System.DBNull.Value) OrElse
                    0 = CInt(drowGrade(ii).Item("プロコングループ")) Then
                    ' 値がNULLか0ならリストボックスに"無し"を表示する
                    dgv(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value = "無し"
                Else
                    ' 値が0以外ならそのまま
                    dgv(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value = CStr(drowGrade(ii).Item("プロコングループ"))
                End If

                ' 疵マップシンボル表示色の値がNULLの時は白色
                If drowGrade(ii).Item("疵マップシンボル表示色") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, ii).Style.BackColor = Color.White      ' 白色
                Else
                    '設定されている色を表示
                    dgv(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, ii).Style.BackColor =
                      Color.FromArgb(CInt(drowGrade(ii).Item("疵マップシンボル表示色")))                ' 疵マップシンボル表示色
                End If

                'ScoreがNULLの時は空白
                If drowGrade(ii).Item("Score") Is System.DBNull.Value Then
                    dgv(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value = ""
                Else
                    ' そのまま表示
                    dgv(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value = drowGrade(ii).Item("Score")
                End If

                ' NumLimit
                dgv(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value = drowGrade(ii).Item("NumLimit")
            Else
                dgv(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value = ""                           ' GrEdasysID
                dgv(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = ""                               ' グレード名称
                dgv(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value = ""                              ' Score
                dgv(EM_DATACOLUMNGRADE.emBtn_MapButton, ii).Style.BackColor = Color.White       ' マップ表示色に白色を
                dgv(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value = "無害"                       ' 欠陥グレードに"無害"を表示する
                dgv(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value = "無し"                     ' FF伝送名称
                dgv(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value = "無し"                       ' プロコン疵グループに"無し"を表示する
                dgv(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, ii).Style.BackColor = Color.White  ' 疵マップシンボル表示色に白色を  
                dgv(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value = ""                           ' NumLimit
            End If
        Next ii

        ' カーソルスタート位置は入力エリアの先頭
        dgv(1, 0).Selected = True
    End Sub

    ''' <summary>
    ''' 確定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub subData_UpDate()

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        ' 入力値チェック
        If Not Data_Check() Then Exit Sub

        Try

            ' 登録しますか？
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("登録しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

            '画面から削除されたEdasysIDを取得
            Data_CheckDelete()

            ' 画面のデータを取得
            Data_Get()

            ' ホントに登録        
            If DB_UpDate() Then
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("正常に登録しました", Color.Black.ToArgb, True, False)
                If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub
            Else
                frmMsg.Text = "確定処理"
                frmMsg.ChangeMsg("登録に失敗", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
            End If

            ' このフォームで必要なデータを取得する
            If Not DB_Select() Then
                ' DB 失敗なので、画面強制終了
                frmMsg.Text = "異常"
                frmMsg.ChangeMsg("DB接続失敗の為 画面を強制終了します", Color.Black.ToArgb, True, False)
                frmMsg.ShowDialog()
                modMain.TaskEnd()                       '終了
            End If
            'データを再取得した為、コンボボックス用GRADE_NAMEを編集
            GradeData_Init()

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' 設定の初期化
        Data_Init()

        ' 画面に値を表示
        Data_Disp()
    End Sub

    ''' <summary>
    ''' 入力値のチェック
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function Data_Check() As Boolean
        Dim strWk As String     ' dgvのデータ格納用
        Dim strMsg As String    ' 重複用のエラーメッセージ
        ' 疵種dgv、1024行ループ
        For ii As Integer = 0 To dgvType.Rows.Count - 1
            ' EdasysID
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, True, "疵種設定 No=" & ii + 1 & " EdasysID") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value = strWk

            ' Edasysidが空白の行は無視
            If Trim(strWk) = "" Then Continue For

            ' EdasysIDの重複チェック
            For jj As Integer = ii + 1 To dgvType.Rows.Count - 1
                ' 比較先のEdasysIDが数値なら比較
                If IsNumeric(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, jj).Value) Then
                    ' EdasysIDが同じならメッセージを出して終了
                    If CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value) = CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, jj).Value) Then
                        strMsg = "【 疵種設定 】" & vbNewLine
                        strMsg &= "同じEdasysIDは登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' 疵種名称
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "疵種設定 No=" & ii + 1 & " 疵種名称") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value = strWk

            ' 疵名称の重複チェック
            For jj As Integer = ii + 1 To dgvType.Rows.Count - 1
                ' 比較先のEdasysIDが数値ならグレード名称を比較
                If IsNumeric(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, jj).Value) Then
                    ' グレード名称が同じならメッセージを出して終了
                    If CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value) = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Name, jj).Value) Then
                        strMsg = "【 疵種設定 】" & vbNewLine
                        strMsg &= "同じ疵種名称は登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            'Score
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Integer.MaxValue, False, "疵種設定 No=" & ii + 1 & " Score") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value = CInt(strWk)

            ' マップ表示名称
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 6, "疵種設定 No=" & ii + 1 & " マップ表示名称") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value = strWk

			'FF伝送名称
            strWk = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 0, 2, "疵種設定 No=" & ii + 1 & " FF伝送名称") <> 0 Then Return False
            dgvType(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value = strWk

        Next ii

        ' グレードdgv8行ループ
        For ii As Integer = 0 To dgvGrade.Rows.Count - 1
            ' EdasysID
            strWk = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 1, 9999, True, "グレード設定 No=" & ii + 1 & " EdasysID") <> 0 Then Return False
            dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value = strWk

            ' Edasysidが空白の行は無視
            If Trim(strWk) = "" Then Continue For

            ' EdasysIDの重複チェック
            For jj As Integer = ii + 1 To dgvGrade.Rows.Count - 1
                ' 比較先のEdasysIDが数値なら比較
                If IsNumeric(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, jj).Value) Then
                    ' EdasysIDが同じならメッセージを出して終了
                    If CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value) = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, jj).Value) Then
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "同じEdasysIDは登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' グレード名称
            strWk = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Str(strWk, 1, 64, "グレード設定 No=" & ii + 1 & " グレード名称") <> 0 Then Return False
            dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value = strWk

            ' グレード名称の重複チェック
            For jj As Integer = ii + 1 To dgvGrade.Rows.Count - 1
                ' 比較先のEdasysIDが数値ならグレード名称を比較
                If IsNumeric(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, jj).Value) Then
                    ' グレード名称が同じならメッセージを出して終了
                    If CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value) = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, jj).Value) Then
                        strMsg = "【 グレード設定 】" & vbNewLine
                        strMsg &= "同じグレード名称は登録できません。" & vbNewLine
                        strMsg &= "- - - No." & CStr(ii + 1) & " と No." & CStr(jj + 1)
                        MsgBox(strMsg)
                        Return False
                    End If
                End If
            Next jj

            ' Score
            strWk = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, Integer.MaxValue, False, "グレード設定 No=" & ii + 1 & " Score") <> 0 Then Return False
            dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value = strWk

            ' 欠陥グレード          コンボボックスなのでチェックしない

            ' グレード別送信上限数
            strWk = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value)
            If tClass_ImputCheck.CheckImputMsg_Int(strWk, 0, 99, False, "グレード設定 No=" & ii + 1 & " グレード別送信上限数") <> 0 Then Return False
            dgvGrade(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value = strWk

        Next ii

        Return True
    End Function

    ''' <summary>
    ''' 画面に表示中のEdasysIDとDBのEdasysIDを比べて、削除されたEdasysIDがあるかチェックする
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_CheckDelete()
        Dim bCheck As Boolean   'チェックフラグ TrueならEdasysIDがある

        ' 疵種非接続テーブルで全行ループ
        For ii As Integer = 0 To m_DsType.Tables(0).Rows.Count - 1
            ' チェックフラグの初期化
            bCheck = False
            ' 疵種dgvで全行ループ
            For kk As Integer = 0 To dgvType.Rows.Count - 1
                ' EdasysIDが空白行以外で、比較
                If CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, kk).Value) <> "" AndAlso _
                 CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, kk).Value) = CInt(m_DsType.Tables(0).Rows(ii).Item("EdasysID")) Then
                    ' EdasysIDが一致したとき、チェックOK
                    bCheck = True
                    Exit For
                End If
            Next kk
            ' チェックフラグがFalseのとき消されたIDなので、登録する
            If bCheck = False Then
                m_nDelTypeEdasysID(ii) = CInt(m_DsType.Tables(0).Rows(ii).Item("EdasysID"))
            End If
        Next ii

        ' グレード非接続テーブルで全行ループ
        For ii As Integer = 0 To m_DsGrade.Tables(0).Rows.Count - 1
            ' チェックフラグの初期化
            bCheck = False
            ' グレードdgvで全行ループ
            For kk As Integer = 0 To dgvGrade.Rows.Count - 1
                ' EdasysIDが空白行以外で比較
                If CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, kk).Value) <> "" AndAlso _
                  CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, kk).Value) = CInt(m_DsGrade.Tables(0).Rows(ii).Item("EdasysID")) Then
                    ' EdasysIDが一致したとき、チェックOK
                    bCheck = True
                    Exit For
                End If
            Next kk
            ' チェックフラグがFalseのとき消されたEdasysIDなので、次にScoreの強いEdasysIDを検索する
            If bCheck = False Then
                ' 削除対象のEdasysIDを格納
                mtyp_Grade(ii).nEdasysID = CInt(m_DsGrade.Tables(0).Rows(ii).Item("EdasysID"))
                ' 次に強いScoreのGrEdasysIDを検索
                mtyp_Grade(ii).nNextEdasysID = Data_SearchNextGr(mtyp_Grade(ii).nEdasysID, CInt(m_DsGrade.Tables(0).Rows(ii).Item("Score")))
            End If
            '// <<<--- 2013.03.27 優先度自動設定
        Next ii
    End Sub

    ''' <summary>
    ''' 画面に表示中のデータを非接続型テーブルにセット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Data_Get()
        Dim nCountNo As Integer   'Noカウント用

        ' デリートインサートする為、今もっている行をクリアする
        m_DsType.Tables(0).Rows.Clear()

        'No初期化
        nCountNo = 0

        ' 疵種dgvの値を疵種非接続テーブルへ格納
        For ii As Integer = 0 To dgvType.RowCount - 1
            'EdasysID が空白以外の行だけ取得
            If CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value) = "" Then Continue For
            Dim drowType As DataRow = m_DsType.Tables(0).NewRow()                                                ' 新規行

            drowType.Item("EdasysID") = CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_EdasysID, ii).Value)                ' EdasysID
            drowType.Item("ID") = nCountNo                                                                       ' IDは表示順番
            drowType.Item("Name") = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Name, ii).Value)                        ' Name
            drowType.Item("Score") = CInt(dgvType(EM_DATACOLUMNTYPE.emTxt_Score, ii).Value)                      ' Score
            drowType.Item("マップ表示名称") = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_Map, ii).Value)               ' マップ表示名称
            drowType.Item("FF伝送名称") = CStr(dgvType(EM_DATACOLUMNTYPE.emTxt_FFSendName, ii).Value)            ' FF伝送名称

            'プロコングループ、コンボボックスなので、無しを選択されていたら0を
            If CStr(dgvType(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value) = "無し" Then
                drowType.Item("プロコングループ") = 0
            Else
                drowType.Item("プロコングループ") = CInt(dgvType(EM_DATACOLUMNTYPE.emCmb_ProGroup, ii).Value)
            End If

			'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
			' drowType.Item("PeriodProcMask") = CByte(dgvType(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, ii).Value)   ' PeriodProcMask
			drowType.Item("PeriodProcMask") = CInt(dgvType(EM_DATACOLUMNTYPE.emChk_PeriodProcMask, ii).Value)   ' PeriodProcMask
			'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------


            '疵マップシンボル表示色 ボタンの背景色をARGB形式にして格納
            drowType.Item("疵マップシンボル表示色") =
                dgvType(EM_DATACOLUMNTYPE.emBtn_MapSymbolColor, ii).Style.BackColor.ToArgb()

            ' 疵マップシンボルは、コンボボックスなので、選択した項目により格納値分岐
            If dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "・" Then
                drowType.Item("疵マップシンボル") = 0
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "●" Then
                drowType.Item("疵マップシンボル") = 1
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "■" Then
                drowType.Item("疵マップシンボル") = 2
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "▲" Then
                drowType.Item("疵マップシンボル") = 3
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "▼" Then
                drowType.Item("疵マップシンボル") = 4
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "◆" Then
                drowType.Item("疵マップシンボル") = 5
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "★" Then
                drowType.Item("疵マップシンボル") = 6
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "＊" Then
                drowType.Item("疵マップシンボル") = 7
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "×" Then
                drowType.Item("疵マップシンボル") = 8
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_MapSynbol, ii).Value.ToString.Trim = "＋" Then
                drowType.Item("疵マップシンボル") = 9
            Else
                drowType.Item("疵マップシンボル") = 0
            End If

            ' 切出画像種類1は、コンボボックスなので、選択した項目により格納値分岐
            If dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "初期定数" Then
                drowType.Item("切出画像種類1") = 0
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "正画像" Then
                drowType.Item("切出画像種類1") = 1
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "乱画像" Then
                drowType.Item("切出画像種類1") = 2
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "乱画像2" Then
                drowType.Item("切出画像種類1") = 3
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype1, ii).Value.ToString.Trim = "乱画像3" Then
                drowType.Item("切出画像種類1") = 4
            Else
                drowType.Item("切出画像種類1") = 0
            End If

            ' 切出画像種類2は、コンボボックスなので、選択した項目により格納値分岐
            If dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "初期定数" Then
                drowType.Item("切出画像種類2") = 0
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "正画像" Then
                drowType.Item("切出画像種類2") = 1
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "乱画像" Then
                drowType.Item("切出画像種類2") = 2
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "乱画像2" Then
                drowType.Item("切出画像種類2") = 3
            ElseIf dgvType(EM_DATACOLUMNTYPE.emCmb_Pictype2, ii).Value.ToString.Trim = "乱画像3" Then
                drowType.Item("切出画像種類2") = 4
            Else
                drowType.Item("切出画像種類2") = 0
            End If

            m_DsType.Tables(0).Rows.Add(drowType)     '行を追加
            nCountNo += 1   'No=No+1
        Next ii

        ' デリートインサートする為、今もっている行をクリアする
        m_DsGrade.Tables(0).Rows.Clear()

        'No初期化
        nCountNo = 0

        ' グレードdgvの値をグレード非接続テーブルへ格納
        For ii As Integer = 0 To dgvGrade.RowCount - 1
            'EdasysID が空白以外の行だけ取得
            If CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value) = "" Then Continue For
            Dim drowGrade As DataRow = m_DsGrade.Tables(0).NewRow()                                 ' 新規行

            drowGrade.Item("EdasysID") = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_EdasysID, ii).Value)    ' EdasysID
            drowGrade.Item("ID") = nCountNo                                                             ' IDは振りなおし
            drowGrade.Item("Name") = CStr(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Name, ii).Value)            ' Name
            drowGrade.Item("Score") = CInt(dgvGrade(EM_DATACOLUMNGRADE.emTxt_Score, ii).Value)          ' Score
            'マップ表示色 ボタンの背景色をARGB形式にして格納
            drowGrade.Item("マップ表示色") =
             dgvGrade(EM_DATACOLUMNGRADE.emBtn_MapButton, ii).Style.BackColor.ToArgb()

            '欠陥グレード コンボボックスなので、選択した項目により格納値分岐
            If dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value.ToString.Trim = "無害" Then
                drowGrade.Item("欠陥グレード") = 0
            ElseIf dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value.ToString.Trim = "軽欠陥" Then
                drowGrade.Item("欠陥グレード") = 1
            ElseIf dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value.ToString.Trim = "中欠陥" Then
                drowGrade.Item("欠陥グレード") = 2
            ElseIf dgvGrade(EM_DATACOLUMNGRADE.emCmb_BadGrade, ii).Value.ToString.Trim = "重欠陥" Then
                drowGrade.Item("欠陥グレード") = 3
            Else
                drowGrade.Item("欠陥グレード") = "無害"
            End If

            'FF伝送名称、コンボボックスなので、無しを選択されていたら0を
            If CStr(dgvGrade(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value) = "無し" Then
                drowGrade.Item("FF伝送名称") = 0
            Else
                drowGrade.Item("FF伝送名称") = CInt(dgvGrade(EM_DATACOLUMNGRADE.emCmb_FFSendName, ii).Value)
            End If

            'プロコングループ、コンボボックスなので、無しを選択されていたら0を
            If CStr(dgvGrade(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value) = "無し" Then
                drowGrade.Item("プロコングループ") = 0
            Else
                drowGrade.Item("プロコングループ") = CInt(dgvGrade(EM_DATACOLUMNGRADE.emCmb_ProGroup, ii).Value)
            End If

            '疵マップシンボル表示色 ボタンの背景色をARGB形式にして格納
            drowGrade.Item("疵マップシンボル表示色") =
             dgvGrade(EM_DATACOLUMNGRADE.emBtn_MapSynbolColor, ii).Style.BackColor.ToArgb()

            drowGrade.Item("NumLimit") = CByte(dgvGrade(EM_DATACOLUMNGRADE.emTxt_NumLimit, ii).Value)   ' NumLimit

            m_DsGrade.Tables(0).Rows.Add(drowGrade)       '行を追加
            nCountNo += 1                                 'No = No + 1
        Next ii
    End Sub

	''' <summary>
	''' アップデート
	''' </summary>
	''' <returns>復帰情報(True:正常,False:異常)</returns>
	''' <remarks></remarks>
	Private Function DB_UpDate() As Boolean
		Try
			' 疵種マスターテーブルとグレードマスターテーブルの値を登録
			If Not DB_Delete() Then
				Return False
			End If

			'統括機能へパラメータ変更通知
			modMain.Send_Main(CH_PARAM_TYPE_GRADE, m_nSendScode)		' 疵種・Gr
			modMain.HardCopy(Me)
			Return True
		Catch ex As Exception
			gcls_Log.Write(tClass_LogManager.LNO.ERR, "設定エラー [" & ex.Message & "] " & Me.Text)
			Return False
		End Try
	End Function

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ DataGridView 関連
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' 疵種dgvの列定義
	''' </summary>
	''' <param name="dgv">データグリッドビュー</param>
	''' <remarks></remarks>
	Private Sub dgvInitializeDefType(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' 基本を設定
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)						' 選択色無し

		With dgv
			.ColumnHeadersHeight = 42									' ヘッダーの高さ
			.AllowUserToResizeColumns = True							' ユーザー 列サイズ変更
			.ScrollBars = ScrollBars.Both								' スクロールバーの表示

			' 列ヘッダー 文字サイズ変更,文字色変更
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor		' 選択色
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue				' 1行おきに色替え
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue		' 一行おきに選択色変更
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black
		End With

		'' 列定義
		' No
		dgvColSetText(dgv, "No", 42, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

		' EdasysID
		dgvColSetText(dgv, "EdasysID" & vbNewLine & "(1以上)", 85, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' 疵種名称
        dgvColSetText(dgv, "疵種名称" & vbNewLine & "(半角６４文字)", 148, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

        ' Score
        dgvColSetText(dgv, "Score" & vbNewLine & "※１", 67, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.TopCenter, DataGridViewContentAlignment.TopCenter, "")

		' マップ表示名称
		dgvColSetText(dgv, "マップ" & vbNewLine & "表示名称", 86, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        ' FF伝送名称
        dgvColSetText(dgv, "FF" & vbNewLine & "伝送名称", 80, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, "")

        '  プロコン疵グループ
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "プロコン" & vbNewLine & "疵グループ"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItem As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            lstItem.Items.AddRange("無し", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10")
            lstItem.MaxDropDownItems = lstItem.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' PeriodProcMask
        dgvColSetCheck(dgv, "PeriodProcMask", 70, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter, Nothing, Nothing, False)

        ' 疵マップシンボル表示色
        Using clm As New DataGridViewButtonColumn
            clm.HeaderText = "疵マップ" & vbNewLine & "表示色"
            clm.Width = 86
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            clm.DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            Dim mapBtnText As DataGridViewButtonColumn = CType(clm, DataGridViewButtonColumn)
            mapBtnText.UseColumnTextForButtonValue = True
            mapBtnText.Text = "変更"
            dgv.Columns.Add(clm)
        End Using

        ' 疵マップシンボル
        Using clm As New DataGridViewComboBoxColumn
			clm.HeaderText = "疵マップ" & vbNewLine & "シンボル"
			clm.Width = 80
			clm.SortMode = DataGridViewColumnSortMode.NotSortable
			clm.Frozen = False
			clm.ReadOnly = False
			clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
			clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
			Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("・", "●", "■", "▲", "▼", "◆", "★", "＊", "×", "＋")
			listItemFF.MaxDropDownItems = listItemFF.Items.Count
			dgv.Columns.Add(clm)
        End Using

        ' 切出画像種類1
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "画像表示1"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("初期定数", "正画像", "乱画像", "乱画像2", "乱画像3")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            clm.Visible = True
            dgv.Columns.Add(clm)
        End Using

        ' 切出画像種類2
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "画像表示2"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("初期定数", "正画像", "乱画像", "乱画像2", "乱画像3")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            clm.Visible = True
            dgv.Columns.Add(clm)
        End Using

        ' 上へ挿入
        dgvColSetButton(dgv, "上へ" & vbNewLine & "挿入", 50, False, False, _
						DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
						New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "挿入")

		' 削除
		dgvColSetButton(dgv, "削除", 50, False, False, _
						DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
						New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "削除")

	End Sub

	''' <summary>
	''' グレードdgvの列定義
	''' </summary>
	''' <param name="dgv">データグリッドビュー</param>
	''' <remarks></remarks>
	Private Sub dgvInitializeDefGrade(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
		'''' 基本を設定
		Call dgv.dgvInitialize(dgv)
		Call dgv.DataGridViewSelectionCancel(dgv)						' 選択色無し

		With dgv
			.ColumnHeadersHeight = 42									' ヘッダーの高さ
			.AllowUserToResizeColumns = True							' ユーザー 列サイズ変更
			.ScrollBars = ScrollBars.Both								' スクロールバーの表示

			' 列ヘッダー 文字サイズ変更,文字色変更
			.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

			'' 選択セル色定義
			.DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor		' 選択色
			.DefaultCellStyle().SelectionForeColor = Color.Black
			.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue				' 1行おきに色替え
			.AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue		' 一行おきに選択色変更
			.DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
			.DefaultCellStyle().ForeColor = Color.Black
		End With

		'' 列定義
		' No
		dgvColSetText(dgv, "No", 42, True, True, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

		' EdasysID
		dgvColSetText(dgv, "EdasysID" & vbNewLine & "(1以上)", 80, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)

        ' グレード名称
		'// 2023.11.17 最終工程検査装置高機能化その３ -------------------->>>>>
		'dgvColSetText(dgv, "グレード名称" & vbNewLine & "(半角64文字)", 198, False, False,
		'DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)

		dgvColSetText(dgv, "グレード名称" & vbNewLine & "(半角64文字)", 181, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
		'// 2023.11.17 最終工程検査装置高機能化その３ <<<<<--------------------
        ' Score
        dgvColSetText(dgv, "Score" & vbNewLine & "※１", 57, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.TopCenter, DataGridViewContentAlignment.TopCenter, "")

		' マップ表示色
		dgvColSetButton(dgv, "探傷画面" & vbNewLine & "表示色", 80, False, False, _
		DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
		New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "変更")

        '  欠陥グレード
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "欠陥" & vbNewLine & "グレード"
            clm.Width = 83
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("無害", "軽欠陥", "中欠陥", "重欠陥")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            dgv.Columns.Add(clm)
        End Using

        '  FF伝送名称
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "FF" & vbNewLine & "伝送名称"
            clm.Width = 83
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim listItemFF As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            listItemFF.Items.AddRange("無し", "1", "2", "3", "4", "5", "6", "7", "8")
            listItemFF.MaxDropDownItems = listItemFF.Items.Count
            dgv.Columns.Add(clm)
        End Using

        '  プロコン疵グループ
        Using clm As New DataGridViewComboBoxColumn
            clm.HeaderText = "プロコン" & vbNewLine & "Grグループ"
            clm.Width = 100
            clm.SortMode = DataGridViewColumnSortMode.NotSortable
            clm.Frozen = False
            clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
            Dim lstItemPro As DataGridViewComboBoxColumn = CType(clm, DataGridViewComboBoxColumn)
            lstItemPro.Items.AddRange("無し", "1", "2", "3", "4", "5", "6")
            lstItemPro.MaxDropDownItems = lstItemPro.Items.Count
            dgv.Columns.Add(clm)
        End Using

        ' 疵マップシンボル表示色
        dgvColSetButton(dgv, "疵マップ" & vbNewLine & "表示色", 80, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter,
        New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "変更")

        ' NumLimit
        dgvColSetText(dgv, "グレード別" & vbNewLine & "送信上限数", 94, False, False,
        DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.TopCenter)

        ' 上へ挿入
        dgvColSetButton(dgv, "上へ" & vbNewLine & "挿入", 50, False, False, _
						DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
						New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "挿入")

		' 削除
		dgvColSetButton(dgv, "削除", 50, False, False, _
						DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter, _
						New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point), True, "削除")

	End Sub

    ''' <summary>
    ''' 疵種dgvのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvType_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvType.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case EM_DATACOLUMNTYPE.emBtn_MapSymbolColor     '疵マップシンボル表示色
                GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)
            Case EM_DATACOLUMNTYPE.emInsButton     '上へ挿入ボタン
                InsBtnClick_Type(dgv, e.RowIndex)
            Case EM_DATACOLUMNTYPE.emDelButton     '削除ボタン
                DelBtnClick_Type(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' グレードdgvのセルのボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGrade_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvGrade.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case EM_DATACOLUMNGRADE.emBtn_MapButton, EM_DATACOLUMNGRADE.emBtn_MapSynbolColor  'マップ表示色,疵マップシンボル表示色
                GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)
            Case EM_DATACOLUMNGRADE.emInsButton    '上へ挿入ボタン
                InsBtnClick_Grade(dgv, e.RowIndex)
            Case EM_DATACOLUMNGRADE.emDelButton    '削除ボタン
                DelBtnClick_Grade(dgv, e.RowIndex)
        End Select
    End Sub

    ''' <summary>
    ''' マップ表示色ボタンが押されたら、色選択ダイアログを表示し、ボタンの色を変更
    ''' </summary>
    ''' <param name="dgv">対象のデータグリッドビュー</param>
    ''' <param name="nColIndex">ボタンの列インデックス</param>
    ''' <param name="nRowIndex">ボタンの行インデックス</param>
    ''' <remarks></remarks>
    Private Sub GetDlgColor(ByVal dgv As DataGridView, ByVal nColIndex As Integer, ByVal nRowIndex As Integer)
        '色選択ダイアログの設定
        '色の作成を使用しない
        dlgColor.AllowFullOpen = False

        '色選択ダイアログの選択色を、ボタンの背景色にする
        dlgColor.Color = dgv(nColIndex, nRowIndex).Style.BackColor

        ' 色選択ダイアログでokボタンが押されたら、ボタンの色を変更する 
        If (dlgColor.ShowDialog() = Windows.Forms.DialogResult.OK) Then
            '選択色をボタンの背景色に
            dgv(nColIndex, nRowIndex).Style.BackColor = dlgColor.Color
        End If

        dlgColor.Dispose()
    End Sub

    ''' <summary>
    ''' 疵種dgvの挿入ボタン押されたときの処理
    ''' </summary>
    ''' <param name="dgv">対象のデータグリッドビュー</param>
    ''' <param name="nRowIndex">レコードを挿入する位置</param>
    ''' <remarks></remarks>
    Private Sub InsBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

        ' 挿入ボタンが使用可能かチェック
        If (dgv.Rows.Count - 1 = nRowIndex) Then
            ' 最終行は使用不可なので抜ける
            Exit Sub
        End If

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try
            '最終行のテキストボックスに何かしらのデータが入っている場合は挿入不可
            If Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_EdasysID, dgv.Rows.Count - 1).Value)) <> "" Or
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Name, dgv.Rows.Count - 1).Value)) <> "" Or
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Score, dgv.Rows.Count - 1).Value)) <> "" Or
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_Map, dgv.Rows.Count - 1).Value)) <> "" Or
            Trim(CStr(dgv(EM_DATACOLUMNTYPE.emTxt_FFSendName, dgv.Rows.Count - 1).Value)) <> "" Then

                frmMsg.Text = "条件確認"
                frmMsg.ChangeMsg("最終行にデータが存在します。", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Exit Sub
            End If

            '挿入するかメッセージを出す
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("挿入しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' 疵種dgvにレコードを挿入
        InsertAt_TypeData(nRowIndex)

        ' 疵種dgvの最終行を削除
        dgv.Rows.RemoveAt(dgvType.Rows.Count - 1)

        'Noを振りなおしボタンを表示しなおす
        InitIndexDgvType(dgv)

    End Sub

    ''' <summary>
    ''' 疵種dgvの削除ボタンが押されたときの処理
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Type(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' 削除するかメッセージを出す
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("削除しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' 疵種dgvの最終行にレコードを挿入
        InsertAt_TypeData(dgv.Rows.Count)

        ' 疵種dgvのレコードを削除
        dgv.Rows.RemoveAt(nRowIndex)

        ' Noを振りなおしボタンを表示しなおす
        InitIndexDgvType(dgv)

    End Sub

    ''' <summary>
    ''' グレードdgvの挿入ボタン押されたときの処理
    ''' </summary>
    ''' <param name="dgv">対象のデータグリッドビュー</param>
    ''' <param name="nRowIndex">レコードを挿入する位置</param>
    ''' <remarks></remarks>
    Private Sub InsBtnClick_Grade(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

        ' 挿入ボタンが使用可能かチェック
        If (dgv.Rows.Count - 1 = nRowIndex) Then
            ' 最終行は使用不可なので抜ける
            Exit Sub
        End If

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' 最終行のテキストボックスに何かしらのデータが入っている場合は挿入不可
            If Trim(CStr(dgv(EM_DATACOLUMNGRADE.emTxt_EdasysID, dgv.Rows.Count - 1).Value)) <> "" Or
               Trim(CStr(dgv(EM_DATACOLUMNGRADE.emTxt_Name, dgv.Rows.Count - 1).Value)) <> "" Or
               Trim(CStr(dgv(EM_DATACOLUMNGRADE.emTxt_Score, dgv.Rows.Count - 1).Value)) <> "" Or
               Trim(CStr(dgv(EM_DATACOLUMNGRADE.emTxt_NumLimit, dgv.Rows.Count - 1).Value)) <> "" Then
                frmMsg.Text = "条件確認"
                frmMsg.ChangeMsg("最終行にデータが存在します。", Color.Red.ToArgb, True, False)
                frmMsg.ShowDialog()
                Exit Sub
            End If


            ' 挿入するかメッセージを出す
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("挿入しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try

        ' グレードdgvにレコードを挿入 (表示位置がずれないように挿入→削除の順で行う)
        InsertAt_GradeData(nRowIndex)

        ' グレードdgvの最終行を削除
        dgv.Rows.RemoveAt(dgv.Rows.Count - 1)

        ' Noを振りなおし、ボタンを表示しなおす
        InitIndexDgvGrade(dgv)

    End Sub

    ''' <summary>
    ''' グレードdgvの削除ボタンが押されたときの処理
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <param name="nRowIndex"></param>
    ''' <remarks></remarks>
    Private Sub DelBtnClick_Grade(ByVal dgv As DataGridView, ByVal nRowIndex As Integer)

        Dim frmMsg As New tClassForm.frmPop_MsgBox("", "", Color.Black.ToArgb, True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        frmMsg.SetSelectedFNo(0)

        Try

            ' 削除するかメッセージを出す
            frmMsg.Text = "条件確認"
            frmMsg.ChangeMsg("削除しますか？", Color.Black.ToArgb, True, True)
            If Windows.Forms.DialogResult.Cancel = frmMsg.ShowDialog() Then Exit Sub

        Catch ex As Exception

        Finally
            If Not frmMsg Is Nothing Then
                frmMsg.Dispose()
                frmMsg = Nothing
            End If
        End Try
        ' グレードdgvの最終行にレコードを挿入  (表示位置がずれないように挿入→削除の順で行う)
        InsertAt_GradeData(dgv.Rows.Count)

        ' グレードdgvの行を削除
        dgv.Rows.RemoveAt(nRowIndex)

        ' Noを振りなおし、ボタンを表示しなおす
        InitIndexDgvGrade(dgv)

    End Sub

    ''' <summary>
    ''' dgvTypeのインデックスを振り、ボタンの表示設定をする
    ''' </summary>
    ''' <param name="dgv">疵種dgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvType(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNTYPE.emNo, ii).Value = ii + 1   ' 列のインデックス番号

            ' 挿入ボタンの使用可能状態を設定    (ボタンを使用不可にはできないので、色を変更）
            If ii = dgv.RowCount - 1 Then
                ' 最終行は挿入ボタンが使用不可
                dgv(EM_DATACOLUMNTYPE.emInsButton, ii).Style.ForeColor = Color.Gray
            Else
                ' それ以外は使用可
                dgv(EM_DATACOLUMNTYPE.emInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' dgvGradeのインデックスを振り、ボタンの表示設定をする
    ''' </summary>
    ''' <param name="dgv">グレードdgv</param>
    ''' <remarks></remarks>
    Private Sub InitIndexDgvGrade(ByVal dgv As DataGridView)
        For ii As Integer = 0 To dgv.Rows.Count - 1
            dgv(EM_DATACOLUMNGRADE.emNo, ii).Value = ii + 1   ' 列のインデックス番号

            ' 挿入ボタンの使用可能状態を設定    (ボタンを使用不可にはできないので、色を変更）
            If ii = dgv.RowCount - 1 Then
                ' 最終行は挿入ボタンが使用不可
                dgv(EM_DATACOLUMNGRADE.emInsButton, ii).Style.ForeColor = Color.Gray
            Else
                ' それ以外は使用可
                dgv(EM_DATACOLUMNGRADE.emInsButton, ii).Style.ForeColor = Color.Black
            End If
        Next ii
    End Sub

    ''' <summary>
    ''' dgvの選択が変更されたら、セルの選択状態をキャンセル
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dataGridView_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvGrade.SelectionChanged, dgvType.SelectionChanged
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv
        dgv.CurrentCell.Selected = False
    End Sub


    'セルが選択されたときの処理
    ''' <summary>
    ''' dgvのセルが選択されたときにコンボボックスならドロップダウンする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks>%{DOWN}でのドロップダウン操作が、開発PCだと出来ない、原因不明</remarks>
    Private Sub dataGridView_CellEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvGrade.CellEnter, dgvType.CellEnter
        Dim dgv As DataGridView = CType(sender, DataGridView)   ' 対象のdgv

        ' 列のタイプがコンボボックスか？
        If TypeOf dgv.Columns(e.ColumnIndex) Is DataGridViewComboBoxColumn Then
            ' ドロップダウン表示
            SendKeys.Send("%{DOWN}")
        End If
    End Sub


    ''' <summary>
    ''' 更新ボタンの有効/無効
    ''' </summary>
    ''' <param name="bPassResult">パスワード入力結果 (True:有効 False:無効)</param>
    ''' <remarks></remarks>
    Private Sub UpdateEnabled(ByVal bPassResult As Boolean)

        btnF11.Enabled = bPassResult                    ' ファンクションキーの設定ボタン

    End Sub

    Private Sub dgvGrade_DataError(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewDataErrorEventArgs) Handles dgvGrade.DataError
        If Not (e.Exception Is Nothing) Then
            MessageBox.Show(Me, String.Format("({0},{1})のセルでエラーが発生しました。 " + vbCrLf + vbCrLf + "説明： {2}", _
            e.ColumnIndex, e.RowIndex, e.Exception.Message), "エラーが発生しました", MessageBoxButtons.OK, MessageBoxIcon.Error)
        End If
    End Sub

    ''' <summary>
    ''' DBからとってきたコンボボックス用の値を表示用に加工
    ''' </summary>
    ''' <remarks>グレード名称に以上をつける、無しの行を追加、</remarks>
    Private Sub GradeData_Init()
        Dim drowNewGrade As DataRow     ' 疵種グレードの新規行

        Try
            'グレード非接続テーブルの設定
            ' 全行ループ グレードの名称に以上をつける
            For ii As Integer = 0 To m_DsGradeWk.Tables(0).Rows.Count - 1
                m_DsGradeWk.Tables(0).Rows(ii).Item("Name") = m_DsGradeWk.Tables(0).Rows(ii).Item("Name").ToString & "以上"
            Next ii
            ' グレードの新規行を設定 EdasysID、グレード名称、マップ表示色
            drowNewGrade = m_DsGradeWk.Tables(0).NewRow()
            drowNewGrade.Item("EdasysID") = 0
            drowNewGrade.Item("Name") = "-"
            drowNewGrade.Item("マップ表示色") = -1      ' マップ表示色、使わないけど一応設定
            ' テーブルの一番上に挿入
            m_DsGradeWk.Tables(0).Rows.InsertAt(drowNewGrade, 0)

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "コンボボックス用データ 設定エラー [" & ex.Message & "] " & Me.Text)
        End Try

    End Sub

End Class
