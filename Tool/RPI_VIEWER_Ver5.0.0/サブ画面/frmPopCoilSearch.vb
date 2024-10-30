Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports System.Data.SqlClient
Imports System.Text.RegularExpressions

Public Class frmPopCoilSearch

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 可変列対応
    Private Const MC_MAX_COIL_S As Integer = 5              ' 前半最大可変列
    Private Const MC_MAX_COIL_E As Integer = 5              ' 後半最大可変列
    Private Const MC_MAX_COIL As Integer = MC_MAX_COIL_S + MC_MAX_COIL_E

    Private Enum EM_COIL_S
        emKey = 0                   ' 疵検管理No
        emName                      ' コイルNo
        emDay                       ' 登録日付
        emScode                     ' Scode
        emRpiKey                    ' 普通は管理No。大分厚板など、RPIの管理Noと実績管理Noが異なるときよう

        em0                         ' 0～4まで可変
    End Enum
    ' 選択列定義
    Private Structure TYP_COIL_S
        Dim sName As String         ' 表示名称
        Dim sSelect As String       ' Select句
        Dim nWhereCnt As Integer    ' Where句の数
        Dim sWhere As String        ' Where句
    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数

    ' リスト列順
    Private Enum EM_LIST_COL
        emDate = 0
        emCoil
        emKizukenNo
        emScode
        emRpiKey        ' 非表示列
        em0             ' 前半5列

        emRpi = em0 + MC_MAX_COIL_S     ' ボタン
        ' 後半5列は、m_nColumnCoilE を使うこと
    End Enum

    ' RPIファイル紐付け用構造体
    Private Structure TYP_RPI_INF
        Dim sKizukenNo As String
        Dim sKey As String          ' 疵検管理No
        Dim sName As String         ' コイルNo
        Dim sDay As String          ' 日付
        Dim sScode() As String        ' SCODE
        Dim sRpiKey As String       ' 管理No（他所用）
        Dim sExt() As String        ' 拡張部

        ' 結果
        Dim bRpiUmu(,) As Integer   ' 検索結果 (0:まだ 1:有り -1:無し)
        Dim sFPath(,) As String     ' フルパス

        Dim bInputList() As Integer ' 位置連携CSV結果 (0:まだ 1:有り -1:無し)

        Public Sub initialize(ByVal nRpiNum As Integer)
            ReDim bRpiUmu(2, nRpiNum - 1)
            ReDim sFPath(2, nRpiNum - 1)
            ReDim bInputList(2)
            ReDim sScode(2)
            ReDim sExt(MC_MAX_COIL - 1)
        End Sub
        Public Function FormatRpiName(ByVal m As Match) As String
            Dim x As String = m.ToString()
            Select Case x
                Case "{S_KEY}"              ' 疵検管理No
                    Return sKey
                Case "{S_NAME}"             ' コイルNo
                    Return sName
                Case "{S_DAY}"              ' 登録日付
                    Return sDay
                Case "{S_SCODE_T}"          ' SCODE（表）
                    Return sScode(0)
                Case "{S_SCODE_B}"          ' SCODE（裏）
                    Return sScode(1)
                Case "{S_RPI_KEY}"          ' 他所用管理No
                    Return sRpiKey
                Case "{S_FOLDER_NAME}"      ' フォルダ名
                    Return GetFolderName(sKey)
            End Select
            For ii As Integer = 0 To MC_MAX_COIL - 1
                ' {S_X}(X=0,1,2,...)        
                If x = "{S_" & ii.ToString() & "}" Then
                    Return sExt(ii)
                End If
            Next
            Return x
        End Function
    End Structure



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ クラス
    Private mcls_Param As New clsParamManager                       ' パラメータ管理クラス (本体)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー
    Private m_sLine As String                                       ' 選択ライン
    Private m_sRpiForder(2 - 1, 10 - 1) As String                   ' RPIフォルダ(面, 複数フォルダ)
    Private m_nRpiNum As Integer                                    ' RPIファイルの片面辺りの数
    Private m_sRpiFormat As String                                  ' RPIファイル特定フォーマット (疵検管理No,面,角度or色)
    Private m_sCsvPath As String                                    ' 位置連携CSVのパス
    Private m_sRpiOutputPath As String                              ' RPI保存のパス

    ' 可変列対応
    Private m_lstCoilS(5 + MC_MAX_COIL + -1) As TYP_COIL_S          ' Iniファイル読込定義
    Private mcnt_txtS(MC_MAX_COIL - 1) As TextBox
    Private mcnt_lblS(MC_MAX_COIL - 1) As Label
    Private m_nColumnCoilE As Integer                               ' データグリッドビュー 後半部分の列番号

    ' RPIファイル検索関係
    Private mcls_WThread As New tClass_WorkThread                   ' ワーカースレッド本体
    Private m_CoilList As New List(Of TYP_RPI_INF)                  ' 検索結果受渡用
    'Private m_SelectDay(2 - 1) As DateTime							' 日付

    ' 前回選択
    Private m_nLastRow As Integer = -1                              ' 最終
    Private m_nLastColumn As Integer = -1                           ' 最終

    ' 位置連携 機能
    Private m_bInputLine As Boolean                                 ' 位置連携 機能有無
    Private m_nInputLineColumn As Integer                           ' 位置連携 列No

    Private m_nImageSelectColumn As Integer                         ' 画像選択 列No
    Private m_nSaveSelectColumn As Integer                          ' 画像保存選択 列No

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    'Public Event tEv_RpiSelect(ByVal sFPath As String, ByVal bInit As Boolean)			  ' Rpiファイル選択
    Public Event tEv_RpiSelect(ByVal sFPath As String, ByVal bInit As Boolean, ByVal cKanriNo As String) ' Rpiファイル選択


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント

    ''' <summary>
    ''' 外部から対象コイル変更
    ''' </summary>
    ''' <param name="nMode">-1:上 1:下</param>
    ''' <remarks></remarks>
    Public Sub SelectChangeCoil(ByVal nMode As Integer)
        If -1 = m_nLastRow Then Return
        If -1 = m_nLastColumn Then Return ' 未選択の場合 ヤメ

        ' サーチ
        Dim nRow As Integer = m_nLastRow + nMode
        While True
            If nRow < 0 Then Return
            If nRow > dgvList.RowCount - 1 Then Return

            Dim sFPath As String = GetRpiFileName(m_nLastColumn, nRow)


            '' 選択行変更関係
            dgvChangeSelectRow(m_nLastRow, nRow)

            ' イベント発生
            Dim sKanriNo As String = dgvList(EM_LIST_COL.emKizukenNo, nRow).Value.ToString
            RaiseEvent tEv_RpiSelect(sFPath, True, sKanriNo)
            Exit While
        End While
    End Sub

    ''' <summary>
    ''' 外部から対象面変更
    ''' </summary>
    Public Sub SelectChangeMen()
        If -1 = m_nLastRow Then Return
        If -1 = m_nLastColumn Then Return ' 未選択の場合 ヤメ

        Dim nMen As Integer = (m_nLastColumn - EM_LIST_COL.emRpi) \ m_nRpiNum   ' 現時点
        Dim nColumn As Integer = (m_nLastColumn - EM_LIST_COL.emRpi) Mod m_nRpiNum  ' 角度のどれ？

        nMen = CInt(IIf(0 = nMen, 1, 0))
        nColumn = nMen * m_nRpiNum + nColumn + EM_LIST_COL.emRpi    ' 列位置算出

        ' 更新
        Dim sFPath As String = GetRpiFileName(nColumn, m_nLastRow)

        m_nLastColumn = nColumn
        Dim sKanriNo As String = dgvList(EM_LIST_COL.emKizukenNo, m_nLastRow).Value.ToString
        RaiseEvent tEv_RpiSelect(sFPath, False, sKanriNo)
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ

    Public WriteOnly Property tProp_InputLine() As Boolean
        Set(value As Boolean)
            m_bInputLine = value
        End Set
    End Property


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本処理
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPopCoilSearch_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        SetFormLocationAtOnce = True

        '' コントロール初期化
        mcnt_txtS(0) = txtSelect0
        mcnt_txtS(1) = txtSelect1
        mcnt_txtS(2) = txtSelect2
        mcnt_txtS(3) = txtSelect3
        mcnt_txtS(4) = txtSelect4
        mcnt_txtS(5) = txtSelect5
        mcnt_txtS(6) = txtSelect6
        mcnt_txtS(7) = txtSelect7
        mcnt_txtS(8) = txtSelect8
        mcnt_txtS(9) = txtSelect9

        mcnt_lblS(0) = lblSelect0
        mcnt_lblS(1) = lblSelect1
        mcnt_lblS(2) = lblSelect2
        mcnt_lblS(3) = lblSelect3
        mcnt_lblS(4) = lblSelect4
        mcnt_lblS(5) = lblSelect5
        mcnt_lblS(6) = lblSelect6
        mcnt_lblS(7) = lblSelect7
        mcnt_lblS(8) = lblSelect8
        mcnt_lblS(9) = lblSelect9



        '' コントロール初期化
        Call DspMsg(1, "")
        tmrRpiDsp.Interval = 300

        ' 時間
        dtpDateStart.Value = Now
        dtpTimeStart.Text = Format(Now, "yyyy/MM/dd 00:00:00")
        dtpDateEnd.Value = Now
        dtpTimeEnd.Text = Format(Now, "yyyy/MM/dd 23:59:59")


        'RPIファイル出力フォルダ取得（前回パス）
        m_sRpiOutputPath = tMod.tMod_IniFile.ReadIniStr("RPI_VIEWER", "RPI_OUTPUT_PATH", LOCALINI_NAME) ' RPI保存先

        '' ライン読込み
        Dim sWk As String
        Dim sKey As String
        For ii As Integer = 1 To MC_LINE_CNT
            ' iniファイル取得
            sKey = String.Format("L_{0}", ii)
            sWk = tMod.tMod_IniFile.ReadIniStr("LINE", sKey, TASKINI_NAME)
            sWk.Trim()
            If 0 >= sWk.Length Then Continue For

            ' 値分割
            Dim sAry() As String = sWk.Split(CChar(","))
            If 2 <> sAry.Length Then Continue For
            sAry(0).Trim()
            sAry(1).Trim()
            If 0 >= sAry(0).Length Then Continue For
            If 0 >= sAry(1).Length Then Continue For


            ' そもそもDB有効な場合のみ
            If 1 = tMod.tMod_IniFile.ReadIniInt(sAry(1), "ENABLE_DB", TASKINI_NAME) Then
                ' リストに行追加
                Dim item As New tClass_CmbItem(sAry(1), sAry(0))
                Call cmbLine.Items.Add(item)
            End If

        Next
        If cmbLine.Items.Count <> 0 Then cmbLine.SelectedIndex = 0
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ライン変更

    ''' <summary>
    ''' 対象ライン変更
    ''' </summary>
    Private Sub cmbLine_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbLine.SelectedIndexChanged
        m_sLine = CType(cmbLine.SelectedItem, tClass_CmbItem).g_sKey
        m_nLastRow = -1
        m_nLastColumn = -1

        If Not gcls_Log Is Nothing Then
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("Coil選択[{0}]", m_sLine))
        End If

        ' 必要なパラメータ取得
        mcls_Param.Set_DbSession(m_sLine)
        If Not mcls_Param.ReadParam_Scode() Then
            Call DspMsg(0, "scode 取得異常")
        End If


        ' IniファイルからRPIフォルダ特定
        For ii As Integer = 0 To 1
            For jj As Integer = 0 To 10 - 1
                m_sRpiForder(ii, jj) = ""
                m_sRpiForder(ii, jj) = tMod.tMod_IniFile.ReadIniStr(m_sLine, String.Format("S_{0}_{1}", ii, jj + 1), TASKINI_NAME)
            Next
        Next

        ' 共通部読込
        m_nRpiNum = tMod.tMod_IniFile.ReadIniInt(m_sLine, "RPI_NUM", TASKINI_NAME)
        m_sRpiFormat = tMod.tMod_IniFile.ReadIniStr(m_sLine, "RPI_FORMAT", TASKINI_NAME)
        m_sCsvPath = tMod.tMod_IniFile.ReadIniStr("RPI_VIEWER", "INPUT_LIST_PATH", TASKINI_NAME)   ' 位置連携CSV



        ' コントロール初期化
        Call InitControl(m_sLine)           ' こまごま拡張部
        Call SetListScode()                 ' scode リスト
        Call dgvInitialize(dgvList)         ' 一覧リスト

        If 0 = tMod.tMod_IniFile.ReadIniInt(m_sLine, "SEARCH_THREAD_STOP", TASKINI_NAME, 0) Then
            ' SEARCH_THREAD_STOP=0のときはチェック
            chkCheckRpi.Checked = True
        Else
            ' SEARCH_THREAD_STOP=1のときは非チェック
            chkCheckRpi.Checked = False
        End If
    End Sub



    ''' <summary>
    ''' 各ライン用にコントロール初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitControl(ByVal sLine As String)

        ' 固定部 Iniファイル読込
        m_lstCoilS(EM_COIL_S.emKey) = GetTypCoilS(sLine, "COIL_S_KEY")
        m_lstCoilS(EM_COIL_S.emName) = GetTypCoilS(sLine, "COIL_S_NAME")
        m_lstCoilS(EM_COIL_S.emDay) = GetTypCoilS(sLine, "COIL_S_DAY")
        m_lstCoilS(EM_COIL_S.emScode) = GetTypCoilS(sLine, "COIL_S_SCODE")
        m_lstCoilS(EM_COIL_S.emRpiKey) = GetTypCoilS(sLine, "COIL_S_RPI_KEY")
        If m_lstCoilS(EM_COIL_S.emRpiKey).sName Is Nothing Then
            m_lstCoilS(EM_COIL_S.emRpiKey) = m_lstCoilS(EM_COIL_S.emKey)
            m_lstCoilS(EM_COIL_S.emRpiKey).sSelect += " as wkKey"   ' 管理Noを付与する場合、同一列名でNGになるための処置
        End If

        ' 名称反映
        lblKizukenNo.Text = m_lstCoilS(EM_COIL_S.emKey).sName   ' 疵検管理No
        lblCoilNo.Text = m_lstCoilS(EM_COIL_S.emName).sName     ' コイルNo

        ' 拡張部  Iniファイル読込
        For ii As Integer = 0 To MC_MAX_COIL - 1
            m_lstCoilS(EM_COIL_S.em0 + ii) = GetTypCoilS(sLine, "COIL_S_" & ii.ToString)

            If m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then
                mcnt_lblS(ii).Text = ""
                mcnt_txtS(ii).Enabled = False
            Else
                mcnt_lblS(ii).Text = m_lstCoilS(EM_COIL_S.em0 + ii).sName
                mcnt_txtS(ii).Enabled = True
            End If
        Next

        ' アクセス権
        Dim sTar As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "ACCESS_TARGET", TASKINI_NAME)
        If 0 = sTar.Length Then
            btnAccessPass.Enabled = False
        Else
            btnAccessPass.Enabled = True
        End If

        chkRpiSaveSelect.Enabled = False
        btnRpiSave.Enabled = False
    End Sub


    ''' <summary>
    ''' 定義取得
    ''' </summary>
    ''' <param name="sSec">Iniセクション</param>
    ''' <param name="sKey">Iniキー</param>
    ''' <returns>取得情報</returns>
    Private Function GetTypCoilS(ByVal sSec As String, ByVal sKey As String) As TYP_COIL_S
        Dim typWk As TYP_COIL_S = Nothing
        Dim sWk As String
        sWk = tMod.tMod_IniFile.ReadIniStr(sSec, sKey, TASKINI_NAME)

        Dim sAry() As String = sWk.Split(CChar(";"))
        If 4 <> sAry.Length Then Return typWk

        typWk.sName = sAry(0).Trim()
        typWk.sSelect = sAry(1).Trim()
        typWk.nWhereCnt = CInt(sAry(2).Trim())
        typWk.sWhere = sAry(3).Trim()
        Return typWk
    End Function


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ アクセス権設定
    Private Sub btnAccessPass_Click(sender As System.Object, e As System.EventArgs) Handles btnAccessPass.Click
        Dim sTar As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "ACCESS_TARGET", TASKINI_NAME)
        Dim sUser As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "ACCESS_USER", TASKINI_NAME)
        Dim sPass As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "ACCESS_PASS", TASKINI_NAME)


        Dim exe As String = System.Environment.GetEnvironmentVariable("ComSpec")    ' コマンドプロンプトパス
        Dim arg As String = String.Format("cmdkey /add:{0} /user:{1} /pass:{2}", sTar, sUser, sPass)    '引数生成

        ' /C コマンド実行後、終了のオプションがないと プロセスが残りっぱなしになり、コマンドが成功しない。 hideだしぱっと見分からない。
        Shell(exe & " /C " & arg, AppWinStyle.Hide)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ scode
    ''' <summary>
    ''' scodeリスト一覧表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetListScode()
        clbScode.Items.Clear()

        For Each wk As clsParamManager.PARAM_SCODE In mcls_Param.GetParamScode()
            Dim item As TYP_ITEM
            item.sString = wk.sName
            item.nKey = wk.nID
            clbScode.Items.Add(item, False)
        Next
    End Sub

    ''' <summary>
    ''' 全選択、全解除
    ''' </summary>
    Private Sub AllSetList(ByVal clb As CheckedListBox, ByVal bMode As Boolean)
        For ii As Integer = 0 To clb.Items.Count - 1
            clb.SetItemChecked(ii, bMode)
        Next
    End Sub
    Private Sub btnTypeAllOk_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnScodeAllOk.Click
        AllSetList(clbScode, True)
    End Sub
    Private Sub btnTypeAllNg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnScodeAllNg.Click
        AllSetList(clbScode, False)
    End Sub



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 小物

    ''' <summary>
    ''' 日付コントロールに対して ホイールで加算減算が可能とする
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpTimeStart.MouseWheel, dtpTimeEnd.MouseWheel, dtpDateStart.MouseWheel, dtpDateEnd.MouseWheel
        ' 現在のdatetimepicker に↑↓キーを押された事とする
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    ''' <summary>
    ''' 確定
    ''' </summary>
    Private Sub frmPopCoilSearch_KeyDown(ByVal sender As Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles Me.KeyDown
        If e.KeyCode = Keys.F5 Then
            Call CoilSearch(0, "")
        ElseIf e.KeyCode = Keys.F8 Then
            Call CoilBetween()
        ElseIf e.KeyCode = Keys.F12 Then
            Call Me.Close()
        End If
    End Sub
    Private Sub btnSearch_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSearch.Click
        chkRpiSaveSelect.Checked = False
        Call CoilSearch(0, "")
        chkRpiSaveSelect.Enabled = (0 < dgvList.RowCount)
    End Sub
    Private Sub btnBetween_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnBetween.Click
        Call CoilBetween()
    End Sub


    ''' <summary>
    ''' 画面ガイダンス
    ''' </summary>
    ''' <param name="nMode">0:NG  1:正常</param>
    ''' <param name="sMsg">メッセージ</param>
    ''' <remarks></remarks>
    Private Sub DspMsg(ByVal nMode As Integer, ByVal sMsg As String)
        txtMsg.Text = sMsg
        If 0 = nMode Then
            txtMsg.ForeColor = Color.Red
        Else
            txtMsg.ForeColor = Color.Black
        End If

        txtMsg.Refresh()
    End Sub



    ''' <summary>
    ''' セルクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvList_CellContentClick1(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvList.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub


        ' RPIボタン？
        If e.ColumnIndex < EM_LIST_COL.emRpi Then Exit Sub
        If e.ColumnIndex > EM_LIST_COL.emRpi + m_nRpiNum * 2 Then Exit Sub

        ' RPIパス取り出し
        Dim sFPath As String = GetRpiFileName(e.ColumnIndex, e.RowIndex)

        ' 更新
        m_nLastRow = e.RowIndex
        m_nLastColumn = e.ColumnIndex


        ' イベント発生
        Dim sKanriNo As String = dgvList(EM_LIST_COL.emKizukenNo, e.RowIndex).Value.ToString
        RaiseEvent tEv_RpiSelect(sFPath, True, sKanriNo)

    End Sub


    ''' <summary>
    ''' コイル検索リストインデックスからRPIファイル名取得
    ''' </summary>
    ''' <param name="nColId">行</param>
    ''' <param name="nRowId">列</param>
    ''' <returns>ファイル名</returns>
    ''' <remarks></remarks>
    Private Function GetRpiFileName(ByVal nColId As Integer, ByVal nRowId As Integer) As String
        ' 全長画像ファイル有無検索用スレッド動作時は停止させる
        RpiThreadStop()
        ' 既にファイル名取得済みの場合は取得済みのファイル名を返す
        If "" <> CStr(dgvList(nColId, nRowId).Tag) Then
            Return CStr(dgvList(nColId, nRowId).Tag)
        End If
        Dim SelectRpiNo As Integer = nColId - EM_LIST_COL.emRpi ' 対象の全長画像の番号
        Dim nOu As Integer = SelectRpiNo \ m_nRpiNum            ' 表裏区分
        Dim nNum As Integer = SelectRpiNo Mod m_nRpiNum         ' 角度

        ' 全長画像ファイル名取得処理
        RpiMainExec(nRowId, nOu, nNum)
        ' タイマー更新
        tmrRpiDsp_Tick(Nothing, Nothing)

        Return CStr(dgvList(nColId, nRowId).Tag)
    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ DataGridView 関連
    ''' <summary>
    ''' グリッド設定
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        '''' 独自の項目を設定
        With dgv
            '.ColumnCount = 100
            '.RowCount = 100

            '' 定義
            .SelectionMode = DataGridViewSelectionMode.FullRowSelect        ' 行選択
            .AllowUserToResizeColumns = True                                ' ユーザー 列サイズ変更
            .ScrollBars = ScrollBars.Both


            '' 選択セル色定義
            .DefaultCellStyle.SelectionForeColor = Color.Black          '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            '.DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)	'
            .DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle.ForeColor = Color.Black

            '' 列定義
            '.ColumnCount = EM_SELECT_LIST.emEND					 'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            '.ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersHeight = 24                                       ' 列ヘッダサイズ

            '.AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.AllCells		' 可視列サイズ自動調整


            '.DefaultCellStyle().SelectionBackColor = Color.LightPink
            .DefaultCellStyle().SelectionBackColor = Color.Transparent
            .DefaultCellStyle().SelectionForeColor = Color.Blue

            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emDay).sName ' 列ヘッダー名称
                clm.Width = 170              ' 列幅
                clm.Frozen = True       ' 横スクロール時のロック
                clm.ReadOnly = True     ' 読込専用設定
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = True
                clm.DefaultCellStyle.NullValue = ""      ' データがNULL時の表示内容 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL入力時のデータ内容 
                .Columns.Add(clm)
            End Using

            'コイルNo
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emName).sName    ' 列ヘッダー名称
                clm.Width = 120
                clm.Frozen = True       ' 横スクロール時のロック
                clm.ReadOnly = True     ' 読込専用設定
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = True
                clm.DefaultCellStyle.NullValue = ""      ' データがNULL時の表示内容 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL入力時のデータ内容 
                .Columns.Add(clm)
            End Using
            '疵検管理No
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emKey).sName     ' 列ヘッダー名称
                clm.Width = 150
                clm.Frozen = True       ' 横スクロール時のロック
                clm.ReadOnly = True     ' 読込専用設定
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = True
                clm.DefaultCellStyle.NullValue = ""      ' データがNULL時の表示内容 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL入力時のデータ内容 
                .Columns.Add(clm)
            End Using
            '検査表面状態
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emScode).sName      ' 列ヘッダー名称
                clm.Width = 50
                clm.Frozen = False      ' 横スクロール時のロック
                clm.ReadOnly = True     ' 読込専用設定
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = True
                clm.DefaultCellStyle.NullValue = ""      ' データがNULL時の表示内容 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL入力時のデータ内容 
                .Columns.Add(clm)
            End Using
            'RPI管理No 非表示列
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = m_lstCoilS(EM_COIL_S.emRpiKey).sName     ' 列ヘッダー名称
                clm.Width = 150
                clm.Frozen = False      ' 横スクロール時のロック
                clm.ReadOnly = True     ' 読込専用設定
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = False
                clm.DefaultCellStyle.NullValue = ""      ' データがNULL時の表示内容 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL入力時のデータ内容 
                .Columns.Add(clm)
            End Using
            ' 拡張部
            For ii As Integer = 0 To MC_MAX_COIL_S - 1

                Using clm As New DataGridViewTextBoxColumn
                    clm.HeaderText = m_lstCoilS(EM_COIL_S.em0 + ii).sName      ' 列ヘッダー名称
                    clm.Width = 100
                    clm.Frozen = False      ' 横スクロール時のロック
                    clm.ReadOnly = True     ' 読込専用設定
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.DefaultCellStyle.NullValue = ""      ' データがNULL時の表示内容 
                    clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL入力時のデータ内容 

                    If Not m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then
                        clm.Visible = True
                    Else
                        clm.Visible = False
                        clm.Width = 0
                    End If

                    .Columns.Add(clm)
                End Using
            Next


            '' ボタン名称取得
            Dim sWk As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "RPI_IDX_NAME", TASKINI_NAME)
            Dim sAry() As String = sWk.Split(CChar(","))        ' ボタン名

            '画像選択列
            m_nImageSelectColumn = .ColumnCount
            '' RPIファイル 種類
            For ii As Integer = 1 To 2
                For jj As Integer = 1 To m_nRpiNum
                    Using clm As New DataGridViewButtonColumnColored
                        clm.HeaderText = CStr(IIf(ii = 1, "表", "裏"))     ' 列ヘッダー名称
                        clm.Width = 30
                        clm.Frozen = False      ' 横スクロール時のロック
                        clm.ReadOnly = True     ' 読込専用設定

                        'clm.FlatStyle = FlatStyle.Standard
                        clm.DefaultCellStyle.Font = New Font("ＭＳ ゴシック", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

                        clm.SortMode = DataGridViewColumnSortMode.NotSortable
                        clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                        clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                        clm.Visible = True

                        clm.UseColumnTextForButtonValue = True
                        If sAry.Length < jj OrElse "" = sAry(jj - 1) Then
                            clm.Text = CStr(jj)
                        Else
                            clm.Text = sAry(jj - 1)
                        End If
                        'clm.DefaultCellStyle().SelectionBackColor = Color.Transparent
                        'clm.DefaultCellStyle().BackColor = Color.Red
                        .Columns.Add(clm)
                    End Using
                Next
            Next
            m_nSaveSelectColumn = .ColumnCount
            For ii As Integer = 1 To 2
                For jj As Integer = 1 To m_nRpiNum
                    Using clm As New DataGridViewCheckBoxColumn
                        clm.HeaderText = CStr(IIf(ii = 1, "表", "裏"))     ' 列ヘッダー名称
                        clm.Width = 30
                        clm.Frozen = False      ' 横スクロール時のロック

                        clm.FlatStyle = FlatStyle.Standard
                        clm.DefaultCellStyle.Font = New Font("ＭＳ ゴシック", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

                        clm.SortMode = DataGridViewColumnSortMode.NotSortable
                        clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                        clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                        clm.Visible = False

                        .Columns.Add(clm)
                    End Using
                Next
            Next

            '' 位置連携
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "連携"
                clm.Width = 50
                clm.Frozen = False      ' 横スクロール時のロック
                clm.ReadOnly = True     ' 読込専用設定
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.Visible = m_bInputLine
                clm.DefaultCellStyle.NullValue = ""      ' データがNULL時の表示内容 
                clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL入力時のデータ内容 
                .Columns.Add(clm)
            End Using
            m_nInputLineColumn = .ColumnCount - 1


            '' 後半部分
            m_nColumnCoilE = .ColumnCount '- 1	' 後半部分先頭位置をセット
            For ii As Integer = 0 To MC_MAX_COIL_E - 1

                Using clm As New DataGridViewTextBoxColumn
                    clm.HeaderText = m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sName         ' 列ヘッダー名称
                    clm.Width = 100
                    clm.Frozen = False      ' 横スクロール時のロック
                    clm.ReadOnly = True     ' 読込専用設定
                    clm.SortMode = DataGridViewColumnSortMode.NotSortable
                    clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                    clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                    clm.DefaultCellStyle.NullValue = ""      ' データがNULL時の表示内容 
                    clm.DefaultCellStyle.DataSourceNullValue = ""   ' NULL入力時のデータ内容 

                    If Not m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect Is Nothing Then
                        clm.Visible = True
                    Else
                        clm.Visible = False
                        clm.Width = 0
                    End If

                    .Columns.Add(clm)
                End Using
            Next
        End With
    End Sub


    ''' <summary>
    ''' 選択行変更
    ''' </summary>
    ''' <param name="nOldRow">元行</param>
    ''' <param name="nNewRow">新しい行</param>
    ''' <remarks></remarks>
    Private Sub dgvChangeSelectRow(ByVal nOldRow As Integer, ByVal nNewRow As Integer)

        'カレント行変更して、選択のみ解除すると、思ったとおりの動きになる
        Dim a As DataGridViewCell = dgvList(0, nNewRow)
        dgvList.CurrentCell = a
        a.Selected = False

        ' これだとダメ
        'dgvList.Rows(nNewRow).Selected = True
        'dgvList.Rows(nNewRow).Selected = False


        ' 背景色 色替
        dgvList.Rows(nOldRow).DefaultCellStyle.BackColor = Color.Empty      '行の背景をデフォルト色に戻す
        dgvList.Rows(nNewRow).DefaultCellStyle.BackColor = Color.LightPink          '行の背景を変更

        ' 前回値として保持
        m_nLastRow = nNewRow

        ' 見える範囲に移動
        dgvList.FirstDisplayedScrollingRowIndex = nNewRow
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 検索

    ''' <summary>
    ''' コイル検索
    ''' </summary>
    ''' <param name="nMode">モード (0:通常  1:前後10件)</param>
    ''' <param name="sKeyval">前後10件の基準 疵検管理No</param>
    ''' <remarks></remarks>
    Private Sub CoilSearch(ByVal nMode As Integer, ByVal sKeyval As String)

        '-------------------------------------------------------------------
        '' 前準備
        ' DB接続確認
        Dim clsDB As New tClass.tClass_SQLServer(TASKINI_NAME, m_sLine)
        If Not clsDB.Open() Then
            If Not gcls_Log Is Nothing Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB接続異常 (CoilSearch)"))
            End If
            Call DspMsg(0, "DB接続失敗")
            Exit Sub
        End If

        ' 前回スレッドが動いていたら停止
        RpiThreadStop()
        m_CoilList.Clear()


        ' 実行
        Dim strSQL As String
        If 0 = nMode Then
            strSQL = GetSQLSearchCoil()     ' SQL取得
        Else
            strSQL = GetSQLBetweenCoil(sKeyval)
        End If

        If Not gcls_Log Is Nothing Then
            gcls_Log.Write(tClass_LogManager.LNO.MSG, String.Format("コイル検索実行({0})", strSQL))
        End If

        Dim read As SqlDataReader = clsDB.SelectExecute(strSQL)                                 ' コイル検索実行
        If read Is Nothing = True Then
            If Not gcls_Log Is Nothing Then
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル検索実行失敗"))
            End If
            Call DspMsg(0, "コイル検索失敗" & vbCrLf & strSQL)
            Exit Sub
        End If


        '-------------------------------------------------------------------
        ' 取得
        Dim nRow As Integer = 0
        dgvList.RowCount = 0
        With read
            While .Read() = True                                            ' 結果が有る間、読み込み続ける
                dgvList.RowCount += 1                                       ' 行数追加
                ' 受渡リストへ登録
                Dim typ As New TYP_RPI_INF
                typ.initialize(m_nRpiNum)
                typ.sKizukenNo = .GetSqlValue(4).ToString().Trim()
                ' メンバー変数に入力
                ' トリムはさせない
                Dim dDateWk As Date = .GetDateTime(0)
                typ.sDay = dDateWk.ToString("yyyyMMdd HHmmss")          ' 日付
                typ.sName = .GetSqlValue(1).ToString()                  ' コイルNo
                typ.sKey = .GetSqlValue(2).ToString()                   ' 疵検管理No
                Dim sScodeWk As String = .GetSqlValue(3).ToString()
                typ.sScode = sScodeWk.Split(CChar("/"))                 ' SCODE
                typ.sRpiKey = .GetSqlValue(4).ToString()                ' 管理No
                For ii As Integer = 0 To MC_MAX_COIL - 1
                    typ.sExt(ii) = .GetSqlValue(5 + ii).ToString()      ' 拡張部
                Next

                ' データグリッドに入力
                dgvList(EM_LIST_COL.emDate, nRow).Value = dDateWk.ToString("yyyy/MM/dd HH:mm:ss")       ' 登録日付
                dgvList(EM_LIST_COL.emCoil, nRow).Value = typ.sName.Trim()                              ' コイルNo
                dgvList(EM_LIST_COL.emKizukenNo, nRow).Value = typ.sKey.Trim()                          ' 疵検管理No
                'dgvList.Rows(nRow).Tag = .GetSqlValue(2).ToString().Trim()
                dgvList(EM_LIST_COL.emScode, nRow).Value = sScodeWk.Trim()                              ' Scode
                dgvList(EM_LIST_COL.emRpiKey, nRow).Value = typ.sRpiKey.Trim()                          ' 他所用管理No
                dgvList.Rows(nRow).Tag = typ.sKizukenNo

                ' 拡張部
                For ii As Integer = 0 To MC_MAX_COIL_S - 1
                    dgvList(EM_LIST_COL.em0 + ii, nRow).Value = typ.sExt(ii).Trim()
                Next
                '保存画像選択チェックの初期化
                For ii As Integer = 0 To (2 * m_nRpiNum) - 1
                    dgvList(m_nSaveSelectColumn + ii, nRow).Value = False
                Next

                For ii As Integer = 0 To MC_MAX_COIL_E - 1
                    dgvList(m_nColumnCoilE + ii, nRow).Value = typ.sExt(MC_MAX_COIL_S + ii).Trim()
                Next

                ' ボタン背景色
                For ii As Integer = EM_LIST_COL.emRpi To EM_LIST_COL.emRpi + m_nRpiNum * 2 - 1
                    dgvList(ii, nRow).Style.BackColor = Color.Gray
                Next

                m_CoilList.Add(typ)


                nRow += 1
            End While
        End With

        Call DspMsg(1, nRow & "件 コイル検索完了")


        '-------------------------------------------------------------------
        ' 選択行の色変え 初期表示で色がつかないように、行数設定の後にやると良い
        dgvList.DataGridViewSelectionRowColorChange(Color.LightPink)


        '-------------------------------------------------------------------
        ' 前後10件のみ固有処理
        If 1 = nMode Then
            nRow = 0
            For ii As Integer = 0 To dgvList.RowCount - 1
                If sKeyval = dgvList.Rows(ii).Tag.ToString Then
                    nRow = ii
                    Exit For
                End If
            Next

            ' 有り
            If 0 <> nRow Then
                '' 背景色 色替
                'dgvList(1, nRow).Selected = True
                'Focus(dgvList.Rows(nRow))
                'dgvList.Rows(nRow).Selected = True
                dgvChangeSelectRow(0, nRow)
                'dgvList.FirstDisplayedScrollingRowIndex = nRow
            End If
        End If


        '-------------------------------------------------------------------
        '' スレッド起動
        'm_SelectDay(0) = New DateTime(dtpDateStart.Value.Year, dtpDateStart.Value.Month, dtpDateStart.Value.Day, dtpTimeStart.Value.Hour, dtpTimeStart.Value.Minute, dtpTimeStart.Value.Second)
        'm_SelectDay(1) = New DateTime(dtpDateEnd.Value.Year, dtpDateEnd.Value.Month, dtpDateEnd.Value.Day, dtpTimeEnd.Value.Hour, dtpTimeEnd.Value.Minute, dtpTimeEnd.Value.Second)

        ' ファイル検索用スレッド起動なしの場合はスレッド起動を行わない

        If True = chkCheckRpi.Checked Then
            RpiThreadStart()
        End If
    End Sub


    ''' <summary>
    ''' コイル前後10件
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CoilBetween()
        Dim sKeyVal As String = ""                      ' 選択主キー
        If dgvList.CurrentCell Is Nothing Then Return

        Dim nRow As Integer = dgvList.CurrentCell.RowIndex

        sKeyVal = dgvList(EM_LIST_COL.emKizukenNo, nRow).Value.ToString

        Call CoilSearch(1, sKeyVal)
    End Sub





    ''' <summary>
    ''' コイル検索SQL生成
    ''' </summary>
    ''' <returns>コイル検索SQL生成</returns>
    ''' <remarks></remarks>
    Private Function GetSQLSearchCoil() As String
        Dim strFormat As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "COIL_SELECT", TASKINI_NAME)                                            ' コイル検索SQL
        Dim sSelect As String = ""
        Dim sWhere As String = ""
        Dim sWk As String



        '-------------------------------------------------------------------
        ' SELECT句
        ' 順番確定部
        sSelect &= m_lstCoilS(EM_COIL_S.emDay).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emName).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emKey).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emScode).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emRpiKey).sSelect

        '拡張部
        For ii As Integer = 0 To MC_MAX_COIL_S - 1
            If Not m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then
                sSelect &= ", " & m_lstCoilS(EM_COIL_S.em0 + ii).sSelect
            Else
                sSelect &= ", '' as s" & ii & " "   ' 名前をつけてあげる事必須。betweenで異常になる
            End If
        Next
        For ii As Integer = 0 To MC_MAX_COIL_E - 1
            If Not m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect Is Nothing Then
                sSelect &= ", " & m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect
            Else
                sSelect &= ", '' as s" & MC_MAX_COIL_S + ii & " " ' 名前をつけてあげる事必須。betweenで異常になる
            End If
        Next


        '-------------------------------------------------------------------
        ' WHERE句 確定部
        sWhere &= " 1=1 "
        ' 疵検管理No
        If "" <> txtKizukenNo.Text.Trim Then
            sWhere &= " AND "
            sWhere &= GetSQLWhere_1(m_lstCoilS(EM_COIL_S.emKey), txtKizukenNo.Text)
        End If

        ' コイルNo
        If "" <> txtCoilNo.Text.Trim Then
            sWhere &= " AND "
            sWhere &= GetSQLWhere_1(m_lstCoilS(EM_COIL_S.emName), txtCoilNo.Text)
        End If

        ' 開始日
        If chkDay.Checked Then
            sWhere &= " AND "

            sWhere &= GetSQLWhere_2(m_lstCoilS(EM_COIL_S.emDay), _
                String.Format("{0} {1}", dtpDateStart.Value.ToString("yyyy-MM-dd"), dtpTimeStart.Value.ToString("HH:mm:ss")), _
                String.Format("{0} {1}", dtpDateEnd.Value.ToString("yyyy-MM-dd"), dtpTimeEnd.Value.ToString("HH:mm:ss")))
        End If


        ' scode
        Dim nCheckCnt As Integer = clbScode.CheckedIndices.Count()
        If 0 <> nCheckCnt And clbScode.Items.Count <> nCheckCnt Then
            sWk = ""
            For Each item As TYP_ITEM In clbScode.CheckedItems
                If 0 <> sWk.Length Then sWk &= ", "
                sWk &= item.nKey.ToString
            Next

            sWhere &= " AND "
            If 1 = m_lstCoilS(EM_COIL_S.emScode).nWhereCnt Then
                sWhere &= GetSQLWhere_1(m_lstCoilS(EM_COIL_S.emScode), sWk)
            ElseIf 2 = m_lstCoilS(EM_COIL_S.emScode).nWhereCnt Then
                sWhere &= GetSQLWhere_2(m_lstCoilS(EM_COIL_S.emScode), sWk, sWk)
            End If
        End If

        '-------------------------------------------------------------------
        ' WHERE句 拡張部

        For ii As Integer = 0 To MC_MAX_COIL - 1
            If m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then Continue For
            If "" = mcnt_txtS(ii).Text.Trim Then Continue For

            sWhere &= " AND "
            sWhere &= GetSQLWhere_1(m_lstCoilS(EM_COIL_S.em0 + ii), mcnt_txtS(ii).Text)
        Next


        '-------------------------------------------------------------------
        ' 合体
        Return String.Format(strFormat, sSelect, sWhere)
    End Function


    ''' <summary>
    ''' WHERE句定義(文字)
    ''' </summary>
    ''' <param name="item"></param>
    ''' <param name="str"></param>
    ''' <returns></returns>
    Private Function GetSQLWhere_1(ByVal item As TYP_COIL_S, ByVal str As String) As String
        Return String.Format(item.sWhere, str.Trim)
    End Function

    Private Function GetSQLWhere_2(ByVal item As TYP_COIL_S, ByVal str1 As String, ByVal str2 As String) As String
        Return String.Format(item.sWhere, str1.Trim, str2.Trim)
    End Function



    ''' <summary>
    ''' 前後10コイル検索SQL生成
    ''' </summary>
    ''' <returns>コイル検索SQL生成</returns>
    ''' <remarks></remarks>
    Private Function GetSQLBetweenCoil(ByVal sKeyVal As String) As String
        Dim sSelect As String = ""
        Dim sWhere(1) As String
        Dim sSqlBetween(1) As String

        Dim sKey As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "KEY_NO", TASKINI_NAME)  ' 主キー

        '''' 共通で使っている検索SQLの"ORDER BY"句以降を削除
        Dim sFormat As String = tMod.tMod_IniFile.ReadIniStr(m_sLine, "COIL_SELECT", TASKINI_NAME)                                          ' コイル検索SQL
        Dim sSqlBase As String = sFormat.Substring(0, sFormat.LastIndexOf("ORDER BY", StringComparison.OrdinalIgnoreCase))



        '-------------------------------------------------------------------
        ' SELECT句
        ' 
        ' 順番確定部
        sSelect &= m_lstCoilS(EM_COIL_S.emDay).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emName).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emKey).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emScode).sSelect
        sSelect &= ", " & m_lstCoilS(EM_COIL_S.emRpiKey).sSelect

        '拡張部
        For ii As Integer = 0 To MC_MAX_COIL_S - 1
            If Not m_lstCoilS(EM_COIL_S.em0 + ii).sSelect Is Nothing Then
                sSelect &= ", " & m_lstCoilS(EM_COIL_S.em0 + ii).sSelect
            Else
                sSelect &= ", '' as s" & ii & " "   ' 名前をつけてあげる事必須。betweenで異常になる
            End If
        Next
        For ii As Integer = 0 To MC_MAX_COIL_E - 1
            If Not m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect Is Nothing Then
                sSelect &= ", " & m_lstCoilS(EM_COIL_S.em0 + MC_MAX_COIL_S + ii).sSelect
            Else
                sSelect &= ", '' as s" & MC_MAX_COIL_S + ii & " " ' 名前をつけてあげる事必須。betweenで異常になる
            End If
        Next

        '-------------------------------------------------------------------
        ' WHERE句 以降
        sWhere(0) = String.Format("r.{0}>='{1}' ORDER BY r.{0} ASC", sKey, sKeyVal)
        sWhere(1) = String.Format("r.{0}< '{1}' ORDER BY r.{0} DESC", sKey, sKeyVal)


        '-------------------------------------------------------------------
        ' 合体
        ' 前方向と後ろ方向
        sSqlBetween(0) = String.Format(sSqlBase, " TOP 11 " & sSelect, sWhere(0))   ' 自分を含める
        sSqlBetween(1) = String.Format(sSqlBase, " TOP 10 " & sSelect, sWhere(1))   ' 自分は含めない

        ' 全部がった
        sSelect = String.Format("SELECT * FROM ({0} UNION ALL {1} ) AS uni ORDER BY uni.{2} DESC", sSqlBetween(0), sSqlBetween(1), sKey)
        Return sSelect
    End Function



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ RPIファイル検索 別スレッド
    Dim m_ThStop As Boolean = False

    ''' <summary>
    ''' スレッドスタート
    ''' </summary>
    Public Function RpiThreadStart(Optional ByVal priority As Threading.ThreadPriority = Threading.ThreadPriority.Normal) As Boolean
        m_ThStop = False
        tmrRpiDsp.Start()
        '' メインスレッド実行
        mcls_WThread.ThreadStart(AddressOf RpiMainThread, priority, "", True)
        Return True
    End Function

    ''' <summary>
    ''' スレッドストップ
    ''' </summary>
    Public Sub RpiThreadStop()
        '' インスタンス確認
        If Not mcls_WThread Is Nothing Then
            m_ThStop = True
            tmrRpiDsp.Stop()
            '' メインスレッド終了
            mcls_WThread.ThreadStop(2000)
            ' スレッドが停止中のときは「RPIファイル有無チェック」ボタンをコントロール色にする
            chkCheckRpi.BackColor = System.Drawing.SystemColors.Control
        End If
    End Sub

    ''' <summary>
    ''' RPIファイル検索処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub RpiMainThread()
        Try
            For ii As Integer = 0 To m_CoilList.Count - 1
                ' 今回の検索対象
                Dim typ As TYP_RPI_INF = m_CoilList(ii)

                ' =====================================
                ' RPI 検索
                For ou As Integer = 0 To 1
                    For num As Integer = 0 To m_nRpiNum - 1
                        If m_ThStop Then Return ' 中断要求受付

                        ' 前回中断したところまでループ
                        If typ.bRpiUmu(ou, num) <> 0 Then
                            Continue For
                        End If

                        GetRpiFile(typ, ou, num)
                    Next
                Next

                ' 結果セット
                m_CoilList(ii) = typ
            Next
        Catch ex As Threading.ThreadAbortException
            '' ===>> 坂巻 ハイドラ対応 20161219
            ' ★Windows資格情報なしによってファイルアクセス失敗の時にコメントを出力する機能
            ' ★次回改造時に機能追加（この範囲のコメント文を解除すれば機能は追加される）
            'Catch ex As System.IO.IOException
            '    If ex.Message.StartsWith("ログオン失敗") Then
            '        MsgBox("全長画像ファイル参照失敗。" + vbCrLf + "RPI参照フォルダアクセス権限設定ボタン押して再検索してください。")
            '    End If
            '' <<===
        Catch ex As Exception
        Finally
            m_ThStop = True
        End Try
    End Sub

    ''' <summary>
    ''' RPIファイル検索処理（１ファイル）
    ''' </summary>
    ''' <param name="ii">検索リスト行インデックス(0オリジン)</param>
    ''' <param name="ou">表裏区分(表=0,裏=1)</param>
    ''' <param name="num">角度(0オリジン)</param>
    ''' <remarks></remarks>
    Private Sub RpiMainExec(ByVal ii As Integer, ByVal ou As Integer, ByVal num As Integer)
        Try
            ' 今回の検索対象
            ' 全フォルダーサーチ
            GetRpiFile(m_CoilList(ii), ou, num)

        Catch ex As Threading.ThreadAbortException
        Catch ex As Exception
        End Try
    End Sub

    ''' <summary>
    ''' RPIファイル検索処理（１ファイル
    ''' </summary>
    ''' <param name="typ">RPIファイル情報</param>
    ''' <param name="ou">表裏区分</param>
    ''' <param name="num">コイルNo</param>
    ''' <remarks></remarks>
    Private Sub GetRpiFile(ByRef typ As TYP_RPI_INF, ByVal ou As Integer, ByVal num As Integer)
        For jj As Integer = 0 To 10 - 1
            If "" = m_sRpiForder(ou, jj) Then Continue For

            ' フォーマット生成
            Dim sSearchName As String = m_reParen.Replace(m_sRpiFormat, AddressOf typ.FormatRpiName)
            ' ファイル検索文字列
            sSearchName = String.Format(sSearchName, typ.sKizukenNo, ou, num + 1, typ.sName)
            ' 文字列が.rpiで終わっていなければ"*.rpi"を末尾につける
            If sSearchName.EndsWith(".rpi", StringComparison.CurrentCultureIgnoreCase) Then
                Dim FullPath As String = System.IO.Path.Combine(m_sRpiForder(ou, jj), sSearchName)
                If System.IO.File.Exists(FullPath) Then
                    typ.sFPath(ou, num) = FullPath
                    typ.bRpiUmu(ou, num) = 1
                End If
            Else
                sSearchName &= "*.rpi"
                Try
                    Dim sFpath() As String = IO.Directory.GetFiles(m_sRpiForder(ou, jj), sSearchName, IO.SearchOption.TopDirectoryOnly)
                    If 1 = sFpath.Length Then      ' △ 複数ある場合どうしよう。ありえないはずだけど
                        typ.sFPath(ou, num) = sFpath(0)
                        typ.bRpiUmu(ou, num) = 1
                        Exit For
                    End If
                Catch de As System.IO.DirectoryNotFoundException
                End Try
            End If
        Next
        ' 検索結果更新（RPIファイル無し）
        If 0 = typ.bRpiUmu(ou, num) Then typ.bRpiUmu(ou, num) = -1

        ' =====================================
        ' 位置連携CSV 検索
        If m_bInputLine Then
            If typ.bInputList(ou) <> 0 Then Return
            typ.bInputList(ou) = -1
            If Not String.IsNullOrEmpty(typ.sFPath(ou, num)) Then
                '位置連携ファイル検索
                Dim sWk As String = String.Format("{0}@{1}*.csv", typ.sKizukenNo, ou)
                Dim sOldFpath() As String = IO.Directory.GetFiles(m_sCsvPath, sWk, IO.SearchOption.TopDirectoryOnly)
                If 0 < sOldFpath.Length Then
                    typ.bInputList(ou) = 1
                End If
            End If
        End If
    End Sub

    ''' <summary>
    ''' 更新タイマー
    ''' </summary>
    Private Sub tmrRpiDsp_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tmrRpiDsp.Tick
        Dim bStop As Boolean = m_ThStop ' 最初に今の状態を取得


        ' 結果をリストに反映
        For ii As Integer = 0 To dgvList.RowCount - 1
            Dim typ As TYP_RPI_INF = m_CoilList(ii)

            '''' RPI
            For ou As Integer = 0 To 1
                For num As Integer = 0 To m_nRpiNum - 1
                    Dim nCol As Integer = EM_LIST_COL.emRpi + (m_nRpiNum * ou) + num

                    If 1 = typ.bRpiUmu(ou, num) Then
                        dgvList(nCol, ii).Style.BackColor = Color.LightGreen
                        dgvList(nCol, ii).Tag = CStr(typ.sFPath(ou, num))
                    ElseIf -1 = typ.bRpiUmu(ou, num) Then
                        dgvList(nCol, ii).Style.BackColor = Color.Red
                    End If
                Next
            Next

            '''' 位置連携CSV
            Dim sWk As String = ""
            For ou As Integer = 0 To 1
                If 0 = typ.bInputList(ou) Then sWk &= "　"
                If 1 = typ.bInputList(ou) Then sWk &= "●"
                If -1 = typ.bInputList(ou) Then sWk &= "－"
            Next
            dgvList(m_nInputLineColumn, ii).Value = sWk

        Next


        If bStop Then
            ' スレッドが停止中のときは「RPIファイル有無チェック」ボタンをコントロール色にする
            chkCheckRpi.BackColor = System.Drawing.SystemColors.Control
        Else
            ' スレッドが動作中のときは「RPIファイル有無チェック」ボタンを赤色にする
            chkCheckRpi.BackColor = Color.Red
        End If
        ' 中断or完了だったらもうヤメ
        If bStop Then
            tmrRpiDsp.Stop()
        End If
    End Sub

    ''' <summary>
    ''' 全長画像ファイル有無チェックボタン変更
    ''' </summary>
    Private Sub chkCheckRpi_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkCheckRpi.CheckedChanged
        If chkCheckRpi.Checked Then
            ' 全長画像ファイル有無チェック開始
            RpiThreadStart()
        Else
            ' 全長画像ファイル有無チェック停止
            RpiThreadStop()
        End If
    End Sub


    ''' <summary>
    ''' RPI保存選択ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkRpiSaveSelect_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkRpiSaveSelect.CheckedChanged

        btnRpiSave.Enabled = chkRpiSaveSelect.Checked

        '画像選択列／保存選択列の表示切替
        For ii As Integer = 0 To (2 * m_nRpiNum) - 1
            dgvList.Columns.Item(m_nImageSelectColumn + ii).Visible = Not chkRpiSaveSelect.Checked
            dgvList.Columns.Item(m_nSaveSelectColumn + ii).Visible = chkRpiSaveSelect.Checked
        Next

    End Sub

    ''' <summary>
    ''' RPI保存実行ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRpiSave_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRpiSave.Click

        '保存対象ファイルを作成
        Dim lstRpiFiles As New List(Of String)
        Dim lstErrorRpiFile As New List(Of String)
        For ii As Integer = 0 To dgvList.RowCount - 1
            For jj As Integer = 0 To (2 * m_nRpiNum) - 1
                'チェックの付いてる項目のみ対象
                If dgvList(m_nSaveSelectColumn + jj, ii).Value.Equals(True) Then
                    ' RPIパス取り出し
                    Dim sFPath As String = GetRpiFileName(m_nImageSelectColumn + jj, ii)
                    ' 画像存在チェック
                    If Not String.IsNullOrEmpty(sFPath) Then
                        lstRpiFiles.Add(sFPath)
                    Else
                        'ファイルパスはないので、管理NOを表示する
                        lstErrorRpiFile.Add(dgvList(EM_COIL_S.emRpiKey, ii).Value.ToString)
                    End If
                End If
            Next
        Next

        '保存対象チェック
        If 0 = lstRpiFiles.Count Then
            If lstErrorRpiFile.Count = 0 Then
                MsgBox("保存するRPIファイルを選択してください。")
            Else
                MsgBox("保存するRPIファイルが元から存在しない模様です。" & String.Join(vbCrLf, lstErrorRpiFile.ToArray()))
            End If
            Return
        End If

        '保存先選択
        Dim dlgFolderSelect As New SaveFileDialog
        dlgFolderSelect.Title = "RPIファイルを保存するフォルダを選択してください。"
        dlgFolderSelect.FileName = System.IO.Path.GetFileName(lstRpiFiles(0).ToString())
        dlgFolderSelect.AutoUpgradeEnabled = True
        dlgFolderSelect.Filter = "*.rpi|*.rpi"
        dlgFolderSelect.RestoreDirectory = True
        If Not String.IsNullOrEmpty(m_sRpiOutputPath) Then
            dlgFolderSelect.InitialDirectory = m_sRpiOutputPath
        End If
        If DialogResult.OK <> dlgFolderSelect.ShowDialog(Me) Then
            Return
        End If
        m_sRpiOutputPath = System.IO.Path.GetDirectoryName(dlgFolderSelect.FileName)

        'INIファイル更新
        'インスタンス内とINIファイル値の整合性を合わせるため、即時更新
        tMod.tMod_IniFile.WriteIni("RPI_VIEWER", "RPI_OUTPUT_PATH", LOCALINI_NAME, m_sRpiOutputPath)

        '保存実行（実行中は操作不可）
        Me.Enabled = False
        SHAPI.CopyFilesToFolder(Me.Handle, m_sRpiOutputPath, lstRpiFiles.ToArray())
        Me.Enabled = True

        '結果表示
        If lstErrorRpiFile.Count > 0 Then
            MsgBox("RPIファイル保存完了しました。" & vbCrLf & "ただし、以下のRPIファイルは元からない模様です。" & vbCrLf & String.Join(vbCrLf, lstErrorRpiFile.ToArray()))
        Else
            '全てのチェックをクリア
            For ii As Integer = 0 To dgvList.RowCount - 1
                For jj As Integer = 0 To (2 * m_nRpiNum) - 1
                    dgvList(m_nSaveSelectColumn + jj, ii).Value = False
                Next
            Next

            'RPI保存選択モード解除
            chkRpiSaveSelect.Checked = False

            MsgBox("RPIファイル保存完了しました")
        End If

    End Sub
 
End Class
