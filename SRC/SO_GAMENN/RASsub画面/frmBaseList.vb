'*********************************************************************************
' 基本動作一覧画面
'	[Ver]
'		Ver.01    2012/10/02  基本動作一覧画面追加対応(新規作成)
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' 基本動作一覧画面
''' </summary>
''' <remarks></remarks>
Public Class frmBaseList

#Region "宣言部"
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 型宣言
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' INIファイル モジュール変数用 構造体
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure BASE_DATA
        Dim sCode As String     'コード
        Dim sName As String     '表示名称
        Dim nMinute As Integer  '経過判定値（分）
        Dim sCheckAdd As String '内容追加条件
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' グリッド 列項目
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum em_LogListColm
        iName = 0       '表示名称
        idayDspTime     '発生時刻
        iSysNo          'コード
        iMsg            '内容
        iend            '最終（MAX列）
    End Enum

    ''' <summary>
    ''' INIファイル 基本動作一覧項目
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum em_IniBaseData
        iCode = 0       'コード
        iMinute         '経過判定値（分）
        iName           '表示名称
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_dsLogInf As New DataSet           'ログ情報テーブル データセット
    Private m_BaseData() As BASE_DATA           '基本動作一覧 格納用 構造体配列

#End Region

#Region "初期処理"

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    Public Sub New()

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmBaseList_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location
        Me.Height = Me.Owner.Height
        Me.Width = Me.Owner.Width

        'タイトル設定
        Call SetTitleName(Me)

        ' 遠隔時
        If NOA_ONLINE <> g_NOA Then
            ' フォーム状態
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
        End If

        'オンライン日時 初期値セット
        Call SetTimePicker()

        'グリッド設定処理
        Call dgvInitialize(dgvBaseList)

        '基本動作一覧取得処理
        If GetBaseListData() = False Then
            Dim frm As New frmRas
            frm.SetParentForm(Me.my_frmParent)
            Me.SetParentForm(Nothing)
            frm.Show()
            ExecFromClose()
            Exit Sub
        End If


        '一覧画面表示処理（初期表示）
        Call DspGrid()

    End Sub

    ''' <summary>
    ''' オンライン日時 初期値セット処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetTimePicker()

        Dim sWork As String         'INIファイル 一時ワーク変数
        Dim dtNow As Date = Now     '現在時刻

        'INIファイルから 前回設定したオンライン日時を取得する
        sWork = tMod.tMod_IniFile.ReadIniStr(SO_GAMENN, "ONLINE_DATE", LOCALINI_NAME, "")

        '最初10バイトを日付にセット
        If IsDate(Mid(sWork, 1, 10)) Then
            '日付型だったときセット
            dtpStartDay.Text = Mid(sWork, 1, 10)
        Else
            '不正な値のときは現在日付をセットする
            dtpStartDay.Text = Format(dtNow, "yyyy/MM/dd")
        End If

        '13文字目から 8バイトを時刻にセット
        If IsDate(Mid(sWork, 12, 8)) Then
            '日付型だったときセット
            dtpStartTime.Text = Mid(sWork, 12, 8)
        Else
            '不正な値のときは現在時刻をセットする
            dtpStartTime.Text = Format(dtNow, "HH:mm:ss")
        End If

        'オンライン日時 有効／無効 取得
        sWork = tMod.tMod_IniFile.ReadIniStr(SO_GAMENN, "ONLINE_CHECK", LOCALINI_NAME, "")
        If sWork = "0" Then
            '0のときは無効
            chkStartDay.Checked = False
        End If
    End Sub

    ''' <summary>
    ''' 基本動作一覧取得処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetBaseListData() As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim strSQL As String
        Dim Adapter As New SqlClient.SqlDataAdapter      ' アダプター
        Dim bRet As Boolean = False
        Dim nCnt As Integer
        Dim dsData As New DataSet

        Try

            strSQL = ""
            strSQL &= " SELECT "
            strSQL &= "     表示順番, "
            strSQL &= "     アラームNo, "
            strSQL &= "     未実行異常時間, "
            strSQL &= "     表示名称, "
            strSQL &= "     内容追加条件 "
            strSQL &= " FROM "
            strSQL &= DB_BASE_ACTION_MASTER
            strSQL &= " ORDER BY "
            strSQL &= "     表示順番 ASC "

            tcls_DB.Open()

            'SQL実行
            Adapter = tcls_DB.SelectAdapter(strSQL)

            '取得前にデータセットクリア
            dsData.Clear()

            'データをデータセットに登録
            Adapter.Fill(dsData)

            For nCnt = 0 To dsData.Tables(0).Rows.Count - 1

                '構造体配列追加
                ReDim Preserve m_BaseData(nCnt)

                '各項目設定
                m_BaseData(nCnt).sCode = dsData.Tables(0).Rows(nCnt).Item("アラームNo").ToString
                m_BaseData(nCnt).nMinute = CInt(dsData.Tables(0).Rows(nCnt).Item("未実行異常時間").ToString)
                m_BaseData(nCnt).sName = dsData.Tables(0).Rows(nCnt).Item("表示名称").ToString
                m_BaseData(nCnt).sCheckAdd = dsData.Tables(0).Rows(nCnt).Item("内容追加条件").ToString

            Next

            '正常終了
            bRet = True

        Catch ex As Exception
            '例外処理
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "基本動作一覧取得処理失敗。[" & ex.Message & "]")
            DspMsg("基本動作一覧取得処理失敗。[" & ex.Message & "]")
        Finally
            'クラス解放
            If Not tcls_DB Is Nothing Then
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If

            'データセット
            If Not dsData Is Nothing Then
                dsData.Dispose()
                dsData = Nothing
            End If
        End Try

        Return bRet
    End Function



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' グリッド設定
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)

        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            '' 動作定義
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = True                                ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False

            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択

            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            ' 選択セル色定義（選択色無しなのでコメント）
            '.DefaultCellStyle().SelectionBackColor = SelectionBackColor         ' 選択
            '.DefaultCellStyle().SelectionForeColor = SelectionForeColor            '
            '.AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue 'Color.LightSteelBlue         ' 1行おきに色替え

            .ColumnCount = 4        'カラム数：4

            '列ヘッダ中央寄せ
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .RowCount = 0
            .ColumnHeadersHeight = 25                                       ' 列ヘッダサイズ

            '列1 (表示名称)
            .Columns(em_LogListColm.iName).HeaderText = "表示名称"
            .Columns(em_LogListColm.iName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iName).Width = 300
            .Columns(em_LogListColm.iName).SortMode = DataGridViewColumnSortMode.NotSortable
            '列2 (発生時刻)
            .Columns(em_LogListColm.idayDspTime).HeaderText = "発生時刻"
            .Columns(em_LogListColm.idayDspTime).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.idayDspTime).DefaultCellStyle.Format = "yyyy/MM/dd HH:mm:ss"
            .Columns(em_LogListColm.idayDspTime).Width = 160
            .Columns(em_LogListColm.idayDspTime).SortMode = DataGridViewColumnSortMode.NotSortable 'DataGridViewColumnSortMode.Automatic
            '列3 (コード)
            .Columns(em_LogListColm.iSysNo).HeaderText = "ｺｰﾄﾞ"
            .Columns(em_LogListColm.iSysNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(em_LogListColm.iSysNo).Width = 50
            .Columns(em_LogListColm.iSysNo).SortMode = DataGridViewColumnSortMode.NotSortable
            '列4 (内容)
            .Columns(em_LogListColm.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iMsg).HeaderText = "内容"
            .Columns(em_LogListColm.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(em_LogListColm.iMsg).Width = 800
            .Columns(em_LogListColm.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable

        End With
    End Sub
#End Region

#Region "ファンクション"
    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmBaseList_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' 0オリジン
			Case 4		'表示
				Call DspGrid()

			Case 11									  ' RAS画面直接表示
				'画面クローズ 親画面表示
				MyBase.ExecFromClose()
		End Select
    End Sub
#End Region

#Region "データ表示系"
    ''' <summary>
    ''' グリッド表示処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspGrid()

        '基本動作がひとつも登録されていないとき表示処理終了
        If m_BaseData Is Nothing Then
            Exit Sub
        End If

        'グリッドを一旦初期化
        dgvBaseList.RowCount = 0

		'グリッド値設定処理
        If SetGridRow() = False Then
            '表示失敗
            dgvBaseList.RowCount = 0
            Exit Sub
        End If

        'オンライン時間設定処理
        Call SetIniData()

    End Sub

    ''' <summary>
    ''' グリッド値設定処理
    ''' </summary>
    ''' <returns>True:正常終了 False:異常終了</returns>
    ''' <remarks></remarks>
    Private Function SetGridRow() As Boolean

        '一覧から引数のアラームNoで検索しセット
        Dim bRet As Boolean = False
        Dim dtNow As Date = Now     '現在時刻を保存
        Dim dtCheck As Date

        Try

            For nCnt As Integer = 0 To UBound(m_BaseData)

                'グリッド行カウントアップ
                dgvBaseList.RowCount += 1

                '表示名称セット
                dgvBaseList(em_LogListColm.iName, nCnt).Value = m_BaseData(nCnt).sName

                'アラームNoセット
                dgvBaseList(em_LogListColm.iSysNo, nCnt).Value = m_BaseData(nCnt).sCode

				'ログ情報取得処理
				If GetDbData(m_BaseData(nCnt).sCode, m_BaseData(nCnt).sCheckAdd) = False Then
                    Exit Try
                End If

				If m_dsLogInf.Tables(0).Rows.Count = 0 Then
					'検索に該当するデータが存在しなかった時、発生時刻、内容、詳細を
					'空白に設定し、列の背景色を赤(ピンク)に設定する
					dgvBaseList(em_LogListColm.idayDspTime, nCnt).Value = ""          '発生時刻
                    dgvBaseList(em_LogListColm.iMsg, nCnt).Value = ""                 '内容
                    dgvBaseList.Rows(nCnt).DefaultCellStyle.BackColor = Color.Pink	  '背景色
				Else

					If m_BaseData(nCnt).nMinute > 0 Then
						'経過判定値が0を超える値のときのみ実施
						'現在時刻から経過判定値(分)の値を減算
						dtCheck = dtNow.AddMinutes(-1 * m_BaseData(nCnt).nMinute)
						If m_dsLogInf.Tables(0).Rows(0).Item("登録日付").ToString < Format(dtCheck, "yyyy/MM/dd HH:MM:ss") Then
							'該当データが経過判定値を経過していたら背景色を黄色に設定
							dgvBaseList.Rows(nCnt).DefaultCellStyle.BackColor = Color.Yellow
						End If
					End If

					'発生時刻、内容、詳細に値をセット
					dgvBaseList(em_LogListColm.idayDspTime, nCnt).Value = m_dsLogInf.Tables(0).Rows(0).Item("登録日付")

                    If m_dsLogInf.Tables(0).Rows(0).Item("内容1").ToString <> "" Then
                        dgvBaseList(em_LogListColm.iMsg, nCnt).Value = m_dsLogInf.Tables(0).Rows(0).Item("内容1").ToString & " " & m_dsLogInf.Tables(0).Rows(0).Item("内容2").ToString
                    Else
                        dgvBaseList(em_LogListColm.iMsg, nCnt).Value = m_dsLogInf.Tables(0).Rows(0).Item("内容2").ToString
                    End If

                End If

            Next

            '正常終了
            bRet = True

        Catch ex As Exception
            '例外処理
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "グリッド値設定処理失敗。[" & ex.Message & "]")
            DspMsg("グリッド値設定処理失敗。[" & ex.Message & "]")
		Finally

		End Try

        Return bRet
    End Function

	''' <summary>
	''' ログ情報取得処理
	''' </summary>
	''' <param name="sCode">アラームNo</param>
	''' <param name="sCheckAdd">内容追加条件</param>
	''' <returns></returns>
	''' <remarks></remarks>
	Private Function GetDbData(ByVal sCode As String, ByVal sCheckAdd As String) As Boolean

		Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
		Dim strSQL As String
		Dim Adapter As New SqlClient.SqlDataAdapter		 ' アダプター
		Dim bRet As Boolean = False

		Try

			'取得前にデータセットクリア
			m_dsLogInf.Clear()

			strSQL = ""
			strSQL &= "        SELECT "
			strSQL &= "              I.登録日付 AS 登録日付, "
            strSQL &= "              L.内容 AS 内容1, "
            strSQL &= "              I.内容 AS 内容2 "

            strSQL &= "        FROM ("
			' アラームNo と 内容追加条件単位で集約
			strSQL &= "            SELECT "
			strSQL &= "                MAX(I.登録日付) AS 登録日付 "
			strSQL &= "            FROM " & DB_LOG_INF & " AS I "
			strSQL &= "            WHERE I.アラームNo = '" & sCode & "' "
			If sCheckAdd <> "" Then
				strSQL &= "            AND   I.内容 LIKE '" & sCheckAdd & "'  "
			End If
			If chkStartDay.Checked Then
				strSQL &= "            AND 発生日付時刻 >= '" & dtpStartDay.Value.ToString("yyyy/MM/dd ") & dtpStartTime.Value.ToString("HH:mm:ss") & "' "
			End If
			strSQL &= "        ) AS MAX_DATA "

			strSQL &= " INNER JOIN " & DB_LOG_INF & " AS I ON I.登録日付 = MAX_DATA.登録日付 "

			strSQL &= " INNER JOIN " & DB_LOG_MASTER & " AS L ON I.アラームNo = L.アラームNo "

			strSQL &= "        WHERE I.アラームNo = '" & sCode & "' "

			If sCheckAdd <> "" Then
				strSQL &= "        AND   I.内容 LIKE '" & sCheckAdd & "'  "
			End If

			tcls_DB.Open()

			'SQL実行
			Adapter = tcls_DB.SelectAdapter(strSQL)

			'データをデータセットに登録
			Adapter.Fill(m_dsLogInf)

			'正常終了
			bRet = True

		Catch ex As Exception
			'例外処理
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "データ取得処理失敗。[" & ex.Message & "]")
			DspMsg("データ取得処理失敗。[" & ex.Message & "]")
		Finally
			'クラス解放
			If Not tcls_DB Is Nothing Then
				tcls_DB.Close()
				tcls_DB.Dispose()
				tcls_DB = Nothing
			End If
		End Try

		Return bRet
	End Function

    ''' <summary>
    ''' INIファイル オンライン時間設定処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetIniData()
        Try
            'チェックが入っているときのみ登録処理を行う
            If chkStartDay.Checked = True Then
                tMod.tMod_IniFile.WriteIni(SO_GAMENN, "ONLINE_CHECK", LOCALINI_NAME, "1")
            Else
                tMod.tMod_IniFile.WriteIni(SO_GAMENN, "ONLINE_CHECK", LOCALINI_NAME, "0")
            End If

            If tMod.tMod_IniFile.WriteIni(SO_GAMENN, "ONLINE_DATE", LOCALINI_NAME, dtpStartDay.Value.ToString("yyyy/MM/dd") & " " & dtpStartTime.Value.ToString("HH:mm:ss")) = False Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "INIファイル オンライン時間設定処理失敗。")
                DspMsg("INIファイル オンライン時間設定処理失敗。")
            End If
        Catch ex As Exception
            '例外処理
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "INIファイル オンライン時間設定処理失敗。[" & ex.Message & "]")
            DspMsg("INIファイル オンライン時間設定処理失敗。[" & ex.Message & "]")
        End Try
    End Sub
#End Region

#Region "イベント処理"
    ''' <summary>
    ''' 日付コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpStartTime.MouseWheel
        ' 現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    ''' <summary>
    ''' 時刻範囲設定チェックボックス チェック変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkStartEndDay_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkStartDay.CheckedChanged
        'オンライン日時の有効／無効切り替え
        dtpStartDay.Enabled = chkStartDay.Checked
        dtpStartTime.Enabled = chkStartDay.Checked
    End Sub

    ''' <summary>
    ''' 画面クローズ時処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmBaseList_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        'データセットを解放
        If Not m_dsLogInf Is Nothing Then
            m_dsLogInf.Dispose()
            m_dsLogInf = Nothing
        End If
    End Sub
#End Region

#Region "確認メッセージ"
    ''' <summary>
    ''' 確認メッセージ表示
    ''' </summary>
    ''' <param name="strMsg"></param>
    ''' <remarks></remarks>
    Public Overridable Sub DspMsg(ByVal strMsg As String)

        Dim frmMsg As tClassForm.frmPop_MsgBox  'メッセージクラス

        'クラス生成
        frmMsg = New tClassForm.frmPop_MsgBox(strMsg, "確認", , True, False, System.Windows.Forms.DialogResult.OK)

        'メッセージ表示
        frmMsg.ShowDialog()

        'メッセージクラス後始末
        If Not frmMsg Is Nothing Then
            frmMsg.Dispose()
            frmMsg = Nothing
        End If

    End Sub
#End Region

End Class
