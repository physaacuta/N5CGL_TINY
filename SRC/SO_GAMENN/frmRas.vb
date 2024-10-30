'*********************************************************************************
' 検RAS画面
'	[Ver]
'		Ver.01    2012/03/12  
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary
Imports KizuForm
Imports tClassLibrary.tClass
Imports tClassLibrary.tClass.tClass_LogManager
Imports tClassLibrary.tClass.tClass_SQLServer
Public Class frmRas
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    'ラジオボタン区分
    Private Enum EM_RAD_KUBUN
        nErr = 0      '異常のみ表示
        nDefult = 1   '正常＋異常(ガイダンスは除く)
    End Enum
    '// データグリッド定数
    Public Enum en_KikiList                 ' 縮退状態表示
        iNo = 0                             ' NO
        iKiKiName                           ' 機器名称
        iIjouHani                           ' 異常範囲
        iKensa                              ' 検査状態
        iKiki                               ' 機器状態
        iData                               ' 生値
        iBtn                                ' 強制解除ボタン
        iMsg                                ' 異常メッセージ
        iBikou                              ' 備考
    End Enum
    '// データグリッド定数
    Public Enum en_GuideList                 ' 縮退状態表示
        emNo = 0                             ' NO
        emName                               ' 機器名称
        emData                               ' 生値
        emMsg                                ' 異常メッセージ
    End Enum
    '// データグリッド定数
    Public Enum en_PcList                 ' 縮退状態表示
        emNo = 0                           ' PCID
        emPCkind                           ' PC種別
        emPCName                           ' PC名称
        emPCName2                          ' PC名
        emOnOff                            ' オンライン／オフライン
        emBtn1                             ' ログオフ
        emBtn2                             ' 再起動
        emBtn3                             ' シャットダウン
        emKiki                             ' 機器状態
    End Enum

    Private Enum EM_DGVBTN_TYPE
        emBtnNG = 0   'ボタン押下不可
        emBtnOK = 1   'ボタン押下可能
    End Enum

    Private Structure TYP_SOGYO_STATUS
        Dim emKadou As EM_DIV_KADOU             '稼動状態
        Dim emUnten As EM_DIV_UNTEN             '運転モード
        Dim emSogyo As EM_DIV_SOGYO             '操業状態
        Dim emPlg As EM_DIV_PLG                 'PLG区分
    End Structure

    Private Enum EM_BTN_TYPE
        emBtn_LogOff = 0        'ログオフ
        emBtn_Reset = 1         '再起動
        emBtn_Exit = 2          'シャットダウン
    End Enum

    Private Structure TYP_LOV_ITEM_DATA
        Dim DispData As String
        Dim ItemNo As Integer
    End Structure

    Private Structure TYP_STATUS_INF
        Dim nId As Integer                  ' 機器ID
        Dim nStatus As Integer              ' 機器状態
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const ERR_CHECK_TIME As Integer = 500           'エラー項目ﾌﾞﾘﾝｸ処理タイマー

    Private m_frmSecurityDisp As frmSecurityDisp            ' 立入制限画面

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_nRadKubun As EM_RAD_KUBUN            'ラジオボタン選択(0:異常のみ表示,1:正常＋異常,2:全て)
    Private m_strResetPass As String                                            ' PCリセットパスワード
    Private m_nLineID As Integer = LINE_ID                                      ' ラインID
    Private m_sMasterTask As String = KS_MASTER                                 ' PCマスター管理プロセス名
    Private m_sSubTask As String = KS_SLAVE                                     ' PCスレーブ管理プロセス名
    Private m_sMasterPC As String = KizuLibCLI.KizuMod.MasterPcName             ' PCマスター管理が起動するPC名
    Private m_lstLovItem As New List(Of TYP_LOV_ITEM_DATA)
    Private m_clOK As Color                      '正常時の色
    Private m_clErr As Color                     '異常時の色
    Private m_clCare As Color                    '注意時の色
    Private m_clNormal As Color = Color.FromKnownColor(KnownColor.Control)  'コントロールの背景色
    Private m_clOnLineNG As Color                'オンライン機器の異常色
    Private m_clOffLineNG As Color               'オフライン機器の異常色
    Private m_dctSecurity As New Dictionary(Of Integer, Control)
    Private m_lstStatus As New List(Of TYP_STATUS_INF)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理
    '/ /////////////////////////////////////////////////////////////////////////////////
    Public Sub New()

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        Dim s As Size
        s.Width = 1920
        s.Height = 1080
        Me.MaximumSize = s
        s.Width = 800
        s.Height = 400
        Me.MinimumSize = s

        ' InitializeComponent() 呼び出しの後で初期化を追加します。

    End Sub


    ''' <summary>
    ''' 画面ロード時 初期設定
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub frmRas_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
        Me.Location = s.Bounds.Location
        Me.Height = Me.Owner.Height
        Me.Width = Me.Owner.Width

        'タイトル設定
        modMain.SetTitleName(Me)

        Log(LNO.MSG, "RAS画面起動")
        LogEx(LNO.MSG, "RAS画面起動")

        '遠隔時
        If NOA_ONLINE <> g_NOA Then
            'フォーム状態
            Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
            Me.tProp_EndButtonEnable = False
        End If
        ' 遠隔時
        If NOA_OFFLINE = g_NOA Then
            btnAllPc_ShutDown.Enabled = False
            btnAllPc_ReStart.Enabled = False
            btnAllPc_LogOff.Enabled = False
        End If

        ' 立入制限画面のインスタンス生成
        m_frmSecurityDisp = New frmSecurityDisp

        'RAS表示色を取得
        GetColor()

        'パスワードをセット
        m_strResetPass = PASSWORD
        m_nLineID = KizuLibCLI.KizuMod.GetLineID()

        ' データグリッド 初期設定
        dgvPcInitialize(dgvPc)
        dgvKikiInitialize(dgvKiki)
        dgvGuideInitialize(dgvGuide)

        '異常範囲文言取得
        Dim typ As TYP_LOV_ITEM_DATA
        m_lstLovItem.Clear()
        typ.DispData = "全体"
        typ.ItemNo = 0
        m_lstLovItem.Add(typ)
        typ.DispData = "表面"
        typ.ItemNo = 1
        m_lstLovItem.Add(typ)
        typ.DispData = "裏面"
        typ.ItemNo = 2
        m_lstLovItem.Add(typ)
        typ.DispData = "ｵﾌﾗｲﾝのみ"
        typ.ItemNo = -1
        m_lstLovItem.Add(typ)
        'デフォルト異常情報のみ表示
        m_nRadKubun = EM_RAD_KUBUN.nErr
        SetButtonColor()

        m_dctSecurity.Clear()
        m_lstStatus.Clear()
        m_dctSecurity.Add(DIV_SECURITY_DOOR_ID, m_frmSecurityDisp.lblStat11)
        m_dctSecurity.Add(DIV_SECURITY_KEY_ID, m_frmSecurityDisp.lblStat12)

        'タイマー周期設定
        timGetStatus.Enabled = True
        timGetStatus.Interval = tMod.tMod_IniFile.ReadIniInt(SO_GAMENN, "RAS_TIMER", TASKINI_NAME, 2000)

        timErrCheck.Enabled = True
        timErrCheck.Interval = ERR_CHECK_TIME

        '初期表示
        tmrDsp_Tick(Nothing, Nothing)

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ PC一覧
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' PC一覧 グリッド初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvPcInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .RowCount = 0

            '' 動作定義
            .ReadOnly = True                                             ' 読み取り専用
            .ScrollBars = ScrollBars.Both                                                           'スクロールバー有効

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .AllowUserToResizeColumns = True                               ' ユーザー 列サイズ変更

            '' 選択セル色定義
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            '.AlternatingRowsDefaultCellStyle().BackColor = Color.LightCyan  ' 1行おきに色替え

            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().BackColor = Color.Empty

            ''''' 列追加
            .Columns.Add(New DataGridViewTextBoxColumn)   ' NO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' PC種別
            .Columns.Add(New DataGridViewTextBoxColumn)   ' PC名称
            .Columns.Add(New DataGridViewTextBoxColumn)   ' PC名
            .Columns.Add(New DataGridViewTextBoxColumn)   ' オンライン／オフライン
            .Columns.Add(New DataGridViewButtonColumn)    ' ログオフ(ボタン)
            .Columns.Add(New DataGridViewButtonColumn)    ' 再起動(ボタン)
            .Columns.Add(New DataGridViewButtonColumn)    ' シャットダウン(ボタン)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 機器状態

            '''' 列定義
            '1列目:PCID
            .Columns(en_PcList.emNo).HeaderText = "NO" & vbCrLf
            .Columns(en_PcList.emNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(en_PcList.emNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emNo).Width = 24
            .Columns(en_PcList.emNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_PcList.emNo).Frozen = True


            '2列目:PC種別
            .Columns(en_PcList.emPCkind).HeaderText = "PC種別" & vbCrLf
            .Columns(en_PcList.emPCkind).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_PcList.emPCkind).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emPCkind).Width = 166
            .Columns(en_PcList.emPCkind).SortMode = DataGridViewColumnSortMode.NotSortable

            '3列目:PC名称
            .Columns(en_PcList.emPCName).HeaderText = "PC名称" & vbCrLf
            .Columns(en_PcList.emPCName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_PcList.emPCName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emPCName).Width = 160
            .Columns(en_PcList.emPCName).SortMode = DataGridViewColumnSortMode.NotSortable

            '4列目:PC名(非表示)
            .Columns(en_PcList.emPCName2).HeaderText = "PC名" & vbCrLf
            .Columns(en_PcList.emPCName2).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_PcList.emPCName2).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emPCName2).Width = 0
            .Columns(en_PcList.emPCName2).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_PcList.emPCName2).Visible = False

            '5列目:区分
            .Columns(en_PcList.emOnOff).HeaderText = "区分" & vbCrLf
            .Columns(en_PcList.emOnOff).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emOnOff).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emOnOff).Width = 44
            .Columns(en_PcList.emOnOff).SortMode = DataGridViewColumnSortMode.NotSortable


            '6列目:ボタン
            .Columns(en_PcList.emBtn1).HeaderText = "ﾛｸﾞｵﾌ" & vbCrLf
            .Columns(en_PcList.emBtn1).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn1).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn1).Width = 50
            .Columns(en_PcList.emBtn1).SortMode = DataGridViewColumnSortMode.NotSortable


            '7列目:ボタン
            .Columns(en_PcList.emBtn2).HeaderText = "再起動" & vbCrLf
            .Columns(en_PcList.emBtn2).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn2).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn2).Width = 60
            .Columns(en_PcList.emBtn2).SortMode = DataGridViewColumnSortMode.NotSortable


            '8列目:ボタン
            .Columns(en_PcList.emBtn3).HeaderText = "ｼｬｯﾄﾀﾞｳﾝ" & vbCrLf
            .Columns(en_PcList.emBtn3).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn3).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emBtn3).Width = 70
            .Columns(en_PcList.emBtn3).SortMode = DataGridViewColumnSortMode.NotSortable

            '9列目:機器状態(非表示)
            .Columns(en_PcList.emKiki).HeaderText = "機器状態" & vbCrLf
            .Columns(en_PcList.emKiki).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_PcList.emKiki).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_PcList.emKiki).Width = 0
            .Columns(en_PcList.emKiki).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_PcList.emKiki).Visible = False

        End With
    End Sub

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData_PC()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim nMaxRowCnt As Integer  '最大行数
        Dim nRowCnt As Integer = 0

        'DBオープン
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB接続エラー(PC) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '行数取得SQL作成
        strSQL = GetSQL_PC(False)

        'レコード読み込み
        If Not tcls_DB.GetSelectKey(strSQL, nMaxRowCnt) Then
            Log(LNO.ERR, "データ取得エラー(PC) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '最大行数セット
        dgvPc.RowCount = nMaxRowCnt

        '表示データの項目取得
        strSQL = GetSQL_PC(True)

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' データ表示
            While sqlRead.Read()

                ' データ取り出し
                dgvPc(en_PcList.emNo, nRowCnt).Value = sqlRead.GetInt32(sqlRead.GetOrdinal("PCID"))               'PCID
                dgvPc(en_PcList.emPCkind, nRowCnt).Value = sqlRead.GetString(sqlRead.GetOrdinal("PC種別"))        'PC種別
                dgvPc(en_PcList.emPCName, nRowCnt).Value = sqlRead.GetString(sqlRead.GetOrdinal("パソコン名称"))  'パソコン名称
                dgvPc(en_PcList.emPCName2, nRowCnt).Value = sqlRead.GetString(sqlRead.GetOrdinal("PC名"))         'PC名

                ' 異常範囲
                If 0 = sqlRead.GetInt32(sqlRead.GetOrdinal("PC区分")) Then
                    dgvPc(en_PcList.emOnOff, nRowCnt).Value = "ｵﾝﾗｲﾝ"
                Else
                    dgvPc(en_PcList.emOnOff, nRowCnt).Value = "ｵﾌﾗｲﾝ"
                End If

                'dgvPc(en_PcList.emNo, nRowCnt).Style.BackColor = Color.Empty
                'dgvPc(en_PcList.emPCkind, nRowCnt).Style.BackColor = Color.Empty
                'dgvPc(en_PcList.emPCName, nRowCnt).Style.BackColor = Color.Empty
                'dgvPc(en_PcList.emPCName2, nRowCnt).Style.BackColor = Color.Empty
                'dgvPc(en_PcList.emOnOff, nRowCnt).Style.BackColor = Color.Empty

                dgvPc(en_PcList.emBtn1, nRowCnt).Value = "ﾛｸﾞｵﾌ"        'ログオフ
                dgvPc(en_PcList.emBtn1, nRowCnt).Style.BackColor = Color.FromKnownColor(KnownColor.Control)
                dgvPc(en_PcList.emBtn1, nRowCnt).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                dgvPc(en_PcList.emBtn2, nRowCnt).Value = "再起動"       '再起動
                dgvPc(en_PcList.emBtn2, nRowCnt).Style.BackColor = Color.FromKnownColor(KnownColor.Control)
                dgvPc(en_PcList.emBtn2, nRowCnt).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                dgvPc(en_PcList.emBtn3, nRowCnt).Value = "ｼｬｯﾄﾀﾞｳﾝ"     'シャットダウン
                dgvPc(en_PcList.emBtn3, nRowCnt).Style.BackColor = Color.FromKnownColor(KnownColor.Control)
                dgvPc(en_PcList.emBtn1, nRowCnt).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

                'マスターPCはボタンを押せないようにする。(一括で行うため)
                If m_sMasterPC = sqlRead.GetString(sqlRead.GetOrdinal("PC名")) Then
                    dgvPc(en_PcList.emBtn1, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn1, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    dgvPc(en_PcList.emBtn2, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn2, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    dgvPc(en_PcList.emBtn3, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn3, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                Else
                    dgvPc(en_PcList.emBtn1, nRowCnt).Style.ForeColor = Color.Black
                    dgvPc(en_PcList.emBtn1, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnOK
                    dgvPc(en_PcList.emBtn2, nRowCnt).Style.ForeColor = Color.Black
                    dgvPc(en_PcList.emBtn2, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnOK
                    dgvPc(en_PcList.emBtn3, nRowCnt).Style.ForeColor = Color.Black
                    dgvPc(en_PcList.emBtn3, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnOK
                End If

                '遠隔時は全て無効
                If NOA_OFFLINE = g_NOA Then
                    dgvPc(en_PcList.emBtn1, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn1, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    dgvPc(en_PcList.emBtn2, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn2, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    dgvPc(en_PcList.emBtn3, nRowCnt).Style.ForeColor = Color.Gray
                    dgvPc(en_PcList.emBtn3, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                End If

                dgvPc(en_PcList.emKiki, nRowCnt).Value = 0

                '''' 次行準備
                nRowCnt += 1

            End While

            sqlRead.Close()

            '機器状態取得
            strSQL = GetSQL_PcStatus(True)

            ' Select文実行
            sqlRead = tcls_DB.SelectExecute(strSQL)
            If sqlRead Is Nothing Then Exit Sub
            ' データ表示
            While sqlRead.Read()
                Dim nPcID As Integer = sqlRead.GetInt32(sqlRead.GetOrdinal("PCID"))                 'PCID
                Dim nSts As Integer = sqlRead.GetInt32(sqlRead.GetOrdinal("機器状態"))              '機器状態
                If 0 <> nSts Then
                    For ii As Integer = 0 To dgvPc.RowCount - 1
                        If nPcID = CInt(dgvPc.Item(en_PcList.emNo, ii).Value) Then
                            dgvPc(en_PcList.emKiki, ii).Value = nSts
                        End If
                    Next
                End If

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("PCデータ表示中に例外[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' SQL文取得
    ''' </summary>
    ''' <param name="bMode">項目取得区分(True:通常,False:個数(count(*)))</param>
    ''' <returns>作成したSQL文</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_PC(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '行数取得
            strSQL &= " SELECT COUNT(*) "

        Else

            '項目取得
            strSQL = " SELECT "
            strSQL &= " M.PCID as PCID,"
            strSQL &= " M.パソコン名称 as パソコン名称,"
            strSQL &= " M.PC名 as PC名,"
            strSQL &= " I.PC種別 as PC種別,"
            strSQL &= " I.PC区分 as PC区分"
        End If

        '最後の","を削除
        sqlColumnTrim(strSQL)

        '// 異常のみ表示は現状のまま
        '// すべて表示 ･･･ 機器IDが設定されているデータすべて表示
        strSQL &= " FROM " & DB_PC_MASTER & " M "
        strSQL &= " INNER JOIN " & DB_PC_INF & " I ON I.PCID = M.PCID"
        If bMode Then
            strSQL &= " ORDER BY PCID ASC "
        End If

        '作成したSQLを返す
        Return strSQL

    End Function

    ''' <summary>
    ''' SQL文取得(状態)
    ''' </summary>
    ''' <param name="bMode">項目取得区分(True:通常,False:個数(count(*)))</param>
    ''' <returns>作成したSQL文</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_PcStatus(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '行数取得
            strSQL &= " SELECT COUNT(*) "

        Else

            '項目取得
            strSQL &= " SELECT a.PCID,"
            strSQL &= "        b.機器状態,"
            strSQL &= "        c.PC区分,"

        End If

        '最後の","を削除
        sqlColumnTrim(strSQL)

        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.機器ID = b.機器ID "
        strSQL &= " INNER JOIN " & DB_PC_INF & " c"
        strSQL &= " ON a.PCID = c.PCID "
        strSQL &= " WHERE (a.検査異常度 <> 0 or a.機器異常度 <> 0) "
        strSQL &= " AND a.PCID <> 0 "
        strSQL &= " AND b.機器状態 <> 0 "


        '作成したSQLを返す
        Return strSQL

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ガイダンス一覧
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ガイダンス一覧 グリッド初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvGuideInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .RowCount = 0

            '' 動作定義
            .ReadOnly = True                                             ' 読み取り専用
            .ScrollBars = ScrollBars.Both                                                           'スクロールバー有効

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .AllowUserToResizeColumns = True                               ' ユーザー 列サイズ変更

            '' 選択セル色定義
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.LightCyan  ' 1行おきに色替え

            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().BackColor = Color.Empty

            ''''' 列追加
            .Columns.Add(New DataGridViewTextBoxColumn)   ' NO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 機器名称
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 生値
            .Columns.Add(New DataGridViewTextBoxColumn)   ' メッセージ

            '''' 列定義
            '1列目:NO
            .Columns(en_GuideList.emNo).HeaderText = "NO" & vbCrLf
            .Columns(en_GuideList.emNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emNo).Width = 30
            .Columns(en_GuideList.emNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_GuideList.emNo).Frozen = True


            '2列目:機器名称
            .Columns(en_GuideList.emName).HeaderText = "機器名称" & vbCrLf
            .Columns(en_GuideList.emName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_GuideList.emName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emName).Width = 220
            .Columns(en_GuideList.emName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3列目:生値
            .Columns(en_GuideList.emData).HeaderText = "値" & vbCrLf
            .Columns(en_GuideList.emData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(en_GuideList.emData).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emData).Width = 50
            .Columns(en_GuideList.emData).SortMode = DataGridViewColumnSortMode.NotSortable

            '4列目:メッセージ
            .Columns(en_GuideList.emMsg).HeaderText = "MSG" & vbCrLf
            .Columns(en_GuideList.emMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_GuideList.emMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_GuideList.emMsg).Width = 340
            .Columns(en_GuideList.emMsg).SortMode = DataGridViewColumnSortMode.NotSortable

        End With
    End Sub
    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData_Guidance()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim nMaxRowCnt As Integer  '最大行数
        Dim nRowCnt As Integer = 0

        'DBオープン
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB接続エラー(ガイダンス) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '行数取得SQL作成
        strSQL = GetSQL_Guidance(False)

        'レコード読み込み
        If Not tcls_DB.GetSelectKey(strSQL, nMaxRowCnt) Then
            Log(LNO.ERR, "データ取得エラー(ガイダンス) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '最大行数セット
        dgvGuide.RowCount = nMaxRowCnt

        '表示データの項目取得
        strSQL = ""
        strSQL = GetSQL_Guidance(True)

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' データ表示
            While sqlRead.Read()

                ' データ取り出し
                dgvGuide(en_GuideList.emNo, nRowCnt).Value = sqlRead.GetInt32(0)       '機器ID
                dgvGuide(en_GuideList.emName, nRowCnt).Value = sqlRead.GetString(1)    '機器名


                '生値として機器状態を表示する。
                dgvGuide(en_GuideList.emData, nRowCnt).Value = CStr(sqlRead.GetInt32(6))    '生値

                dgvGuide(en_GuideList.emMsg, nRowCnt).Value = sqlRead.GetString(5)          '異常MSG

                '''' 次行準備
                nRowCnt += 1

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("ガイダンスデータ表示中に例外[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub
    ''' <summary>
    ''' SQL文取得
    ''' </summary>
    ''' <param name="bMode">項目取得区分(True:通常,False:個数(count(*)))</param>
    ''' <returns>作成したSQL文</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_Guidance(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '行数取得
            strSQL &= " SELECT COUNT(*) "

        Else

            '項目取得
            strSQL &= " SELECT a.機器ID,"
            strSQL &= "        a.機器名,"
            strSQL &= "        a.異常範囲,"
            strSQL &= "        a.検査異常度,"
            strSQL &= "        a.機器異常度,"
            strSQL &= "        ISNULL(a.異常MSG,''),"
            strSQL &= "        b.機器状態,"

        End If

        '最後の","を削除
        sqlColumnTrim(strSQL)

        '// 異常のみ表示は現状のまま
        '// すべて表示 ･･･ 機器IDが設定されているデータすべて表示
        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.機器ID = b.機器ID "
        strSQL &= " WHERE (a.検査異常度 = 0 and a.機器異常度 = 0) and a.異常範囲 <> -1"


        '作成したSQLを返す
        Return strSQL

    End Function

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetSecurityData()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        If 0 = m_dctSecurity.Count Then
            Exit Sub
        End If

        'DBオープン
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB接続エラー(ガイダンス) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        m_lstStatus.Clear()
        Dim keycoll As Dictionary(Of Integer, Control).KeyCollection = m_dctSecurity.Keys

        '行数取得SQL作成
        strSQL = ""
        strSQL &= " SELECT a.機器ID, b.機器状態 "
        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.機器ID = b.機器ID "
        strSQL &= " WHERE a.機器ID IN(0"
        For Each key As Integer In keycoll
            strSQL &= ("," & key.ToString)
        Next
        strSQL &= ")"

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' データ表示
            While sqlRead.Read()

                Dim nId As Integer = sqlRead.GetInt32(0)             '機器ID

                If m_dctSecurity.ContainsKey(nId) Then
                    Dim typStat As TYP_STATUS_INF
                    typStat.nId = nId
                    typStat.nStatus = sqlRead.GetInt32(1)
                    m_lstStatus.Add(typStat)
                End If

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("ガイダンスデータ表示中に例外[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 状態一覧 グリッド初期化
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 状態一覧 グリッド初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvKikiInitialize(ByVal dgv As tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .RowCount = 0

            '' 動作定義
            .ReadOnly = True                                             ' 読み取り専用
            .ScrollBars = ScrollBars.Both                                                           'スクロールバー有効

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black
            .AllowUserToResizeColumns = True                               ' ユーザー 列サイズ変更

            '' 選択セル色定義
            '.DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black            '
            .AlternatingRowsDefaultCellStyle().BackColor = Color.LightCyan  ' 1行おきに色替え

            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point) '
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle().BackColor = Color.Empty

            ''''' 列追加
            .Columns.Add(New DataGridViewTextBoxColumn)   ' NO
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 機器名称
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 検査状態
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 検査状態
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 機器状態
            .Columns.Add(New DataGridViewTextBoxColumn)   ' 生値
            .Columns.Add(New DataGridViewButtonColumn)    ' 強制解除(ボタン)
            .Columns.Add(New DataGridViewTextBoxColumn)   ' メッセージ

            '''' 列定義
            '1列目:NO
            .Columns(en_KikiList.iNo).HeaderText = "NO" & vbCrLf
            .Columns(en_KikiList.iNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iNo).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iNo).Width = 30
            .Columns(en_KikiList.iNo).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(en_KikiList.iNo).Frozen = True


            '2列目:機器名称
            .Columns(en_KikiList.iKiKiName).HeaderText = "機器名称" & vbCrLf
            .Columns(en_KikiList.iKiKiName).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiKiName).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKiKiName).Width = 220
            .Columns(en_KikiList.iKiKiName).SortMode = DataGridViewColumnSortMode.NotSortable

            '3列目:異常範囲
            .Columns(en_KikiList.iIjouHani).HeaderText = "異常範囲" & vbCrLf
            .Columns(en_KikiList.iIjouHani).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iIjouHani).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iIjouHani).Width = 72
            .Columns(en_KikiList.iIjouHani).SortMode = DataGridViewColumnSortMode.NotSortable


            '4列目:検査状態
            .Columns(en_KikiList.iKensa).HeaderText = "検査状態" & vbCrLf
            .Columns(en_KikiList.iKensa).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKensa).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKensa).Width = 64
            .Columns(en_KikiList.iKensa).SortMode = DataGridViewColumnSortMode.NotSortable


            '5列目:機器状態
            .Columns(en_KikiList.iKiki).HeaderText = "機器状態" & vbCrLf
            .Columns(en_KikiList.iKiki).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iKiki).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iKiki).Width = 64
            .Columns(en_KikiList.iKiki).SortMode = DataGridViewColumnSortMode.NotSortable


            '6列目:生値
            .Columns(en_KikiList.iData).HeaderText = "生値" & vbCrLf
            .Columns(en_KikiList.iData).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iData).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iData).Width = 50
            .Columns(en_KikiList.iData).SortMode = DataGridViewColumnSortMode.NotSortable

            '7列目:強制解除
            .Columns(en_KikiList.iBtn).HeaderText = "強制解除" & vbCrLf
            .Columns(en_KikiList.iBtn).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iBtn).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(en_KikiList.iBtn).Width = 64
            .Columns(en_KikiList.iBtn).SortMode = DataGridViewColumnSortMode.NotSortable


            '8列目:異常MSG
            .Columns(en_KikiList.iMsg).HeaderText = "異常MSG" & vbCrLf
            .Columns(en_KikiList.iMsg).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iMsg).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(en_KikiList.iMsg).Width = 1000
            .Columns(en_KikiList.iMsg).SortMode = DataGridViewColumnSortMode.NotSortable

        End With
    End Sub

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData_Status()

        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim nMaxRowCnt As Integer  '最大行数
        Dim nRowCnt As Integer = 0

        'DBオープン
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB接続エラー(状態一覧) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If


        '行数取得SQL作成
        strSQL = GetSQL_Status(False)

        'レコード読み込み
        If Not tcls_DB.GetSelectKey(strSQL, nMaxRowCnt) Then
            Log(LNO.ERR, "データ取得エラー(状態一覧) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        '最大行数セット
        dgvKiki.RowCount = nMaxRowCnt

        '表示データの項目取得
        strSQL = ""
        strSQL = GetSQL_Status(True)

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try

            ' データ表示
            While sqlRead.Read()

                ' データ取り出し
                dgvKiki(en_KikiList.iNo, nRowCnt).Value = sqlRead.GetInt32(0)           '機器ID
                dgvKiki(en_KikiList.iKiKiName, nRowCnt).Value = sqlRead.GetString(1)     '機器名

                ' 異常範囲
                Dim nWk As Integer = sqlRead.GetInt32(2)
                dgvKiki(en_KikiList.iIjouHani, nRowCnt).Value = GetErrRangeString(nWk)
                dgvKiki(en_KikiList.iIjouHani, nRowCnt).Style.BackColor = Color.Empty

                '検査状態表示
                If 0 = sqlRead.GetInt32(3) Then
                    '検査異常度なし
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "-"
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 0 <> sqlRead.GetInt32(6) Then
                    '機器状態が異常の場合は検査異常度を見て表示する
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = ucStatusAria.DspKensa(CType(sqlRead.GetInt32(3), KizuForm.EM_DIV_KENSA))
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Pink
                Else

                    '検査異常度あり、機器状態正常
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Value = "正常"
                    dgvKiki(en_KikiList.iKensa, nRowCnt).Style.BackColor = Color.Empty
                End If

                '機器状態表示
                If 0 = sqlRead.GetInt32(4) Then

                    '機器異常度なし
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "-"
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Empty
                ElseIf 0 <> sqlRead.GetInt32(6) Then

                    '機器状態が異常の場合は、機器異常度を見て表示する
                    Dim emKiki As KizuForm.EM_DIV_KIKI = CType(sqlRead.GetInt32(4), KizuForm.EM_DIV_KIKI)
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = ucStatusAria.DspKiki(emKiki)
                    Select Case emKiki
                        Case KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = m_clCare
                        Case KizuForm.EM_DIV_KIKI.DIV_KIKI_JYU
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = m_clErr
                        Case Else
                            dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = Color.Empty
                    End Select
                Else
                    '機器異常度あり、機器状態正常
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Value = "正常"
                    dgvKiki(en_KikiList.iKiki, nRowCnt).Style.BackColor = m_clOK
                End If

                '生値として機器状態を表示する。
                dgvKiki(en_KikiList.iData, nRowCnt).Value = CStr(sqlRead.GetInt32(6))    '生値

                dgvKiki(en_KikiList.iBtn, nRowCnt).Value = "強制解除"    '強制解除
                dgvKiki(en_KikiList.iBtn, nRowCnt).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                dgvKiki(en_KikiList.iBtn, nRowCnt).Style.BackColor = Color.FromKnownColor(KnownColor.Control)
                If 0 = sqlRead.GetInt32(3) And 0 = sqlRead.GetInt32(4) Then
                    dgvKiki(en_KikiList.iBtn, nRowCnt).Style.ForeColor = Color.Gray
                    dgvKiki(en_KikiList.iBtn, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                Else
                    ' オンライン、遠隔時は押せない
                    If Not (NOA_ONLINE = g_NOA Or NOA_OFFLINE = g_NOA) Then
                        dgvKiki(en_KikiList.iBtn, nRowCnt).Style.ForeColor = Color.Black
                        dgvKiki(en_KikiList.iBtn, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnOK
                    Else
                        dgvKiki(en_KikiList.iBtn, nRowCnt).Style.ForeColor = Color.Gray
                        dgvKiki(en_KikiList.iBtn, nRowCnt).Tag = EM_DGVBTN_TYPE.emBtnNG
                    End If

                End If

                dgvKiki(en_KikiList.iMsg, nRowCnt).Value = sqlRead.GetString(5)          '異常MSG

                '''' 次行準備
                nRowCnt += 1

            End While

        Catch ex As Exception
            Log(LNO.ERR, String.Format("機器状態データ表示中に例外[{0}]", ex.Message))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

    End Sub

    ''' <summary>
    ''' SQL文取得
    ''' </summary>
    ''' <param name="bMode">項目取得区分(True:通常,False:個数(count(*)))</param>
    ''' <returns>作成したSQL文</returns>
    ''' <remarks></remarks>
    Private Function GetSQL_Status(ByVal bMode As Boolean) As String

        Dim strSQL As String = ""

        If Not bMode Then

            '行数取得
            strSQL &= " SELECT COUNT(*) "

        Else

            '項目取得
            strSQL &= " SELECT a.機器ID,"
            strSQL &= "        a.機器名,"
            strSQL &= "        a.異常範囲,"
            strSQL &= "        a.検査異常度,"
            strSQL &= "        a.機器異常度,"
            strSQL &= "        ISNULL(a.異常MSG,''),"
            strSQL &= "        b.機器状態,"

        End If

        '最後の","を削除
        sqlColumnTrim(strSQL)

        '// 異常のみ表示は現状のまま
        '// すべて表示 ･･･ 機器IDが設定されているデータすべて表示
        strSQL &= " FROM " & DB_STATUS_MASTER & " a INNER JOIN " & DB_STATUS_INF & " b"
        strSQL &= " ON a.機器ID = b.機器ID "

        ' 選択ボタンによる分岐
        If m_nRadKubun = EM_RAD_KUBUN.nErr Then
            ' 異常情報のみ表示
            strSQL &= " WHERE (a.検査異常度 <> 0 or a.機器異常度 <> 0) "
            strSQL &= " AND b.機器状態 <> 0"
        ElseIf m_nRadKubun = EM_RAD_KUBUN.nDefult Then
            ' 正常＋異常 情報表示
            strSQL &= " WHERE (a.検査異常度 <> 0 or a.機器異常度 <> 0) "
        Else
            ' 全て表示 ･･･ 機器IDが設定されているデータすべて表示
        End If

        '作成したSQLを返す
        Return strSQL

    End Function

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 操業状態表示欄
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData_SogyoStatus()
        Dim sts As TYP_SOGYO_STATUS = Nothing
        If Get_SogyoStatus(sts) Then
            DspSogyoStatus(sts, lblSogyo)
            DspKadouStatus(sts, lblKadou)
            DspUntenStatus(sts, lblUnten)
            DspPlgSelect(sts, lblPlg)
        End If
    End Sub

    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Function Get_SogyoStatus(ByRef sts As TYP_SOGYO_STATUS) As Boolean
        Dim bRet As Boolean = False
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim nRowCnt As Integer = 0
        'DBオープン
        If Not tcls_DB.Open() Then
            Log(LNO.ERR, "DB接続エラー(操業) [" & DB_STATUS_MASTER & "]")
            Return False
        End If



        '行数取得SQL作成
        strSQL = ""
        strSQL &= "SELECT "
        strSQL &= " 機器ID,"
        strSQL &= " 機器状態 "
        strSQL &= " FROM " & DB_STATUS_INF
        strSQL &= " WHERE "
        strSQL &= "    機器ID=" & DIV_KADOU_ID
        strSQL &= " OR 機器ID=" & DIV_UNTEN_ID
        strSQL &= " OR 機器ID=" & DIV_SOGYO_ID
        strSQL &= " OR 機器ID=" & DIV_PLGMODE_ID

        Try


            ' Select文実行
            sqlRead = tcls_DB.SelectExecute(strSQL)
            If sqlRead Is Nothing Then
                Exit Try
            End If


            ' データ表示
            While sqlRead.Read()

                Select Case sqlRead.GetInt32(0)
                    Case DIV_KADOU_ID
                        sts.emKadou = CType(sqlRead.GetInt32(1), EM_DIV_KADOU)
                    Case DIV_UNTEN_ID
                        sts.emUnten = CType(sqlRead.GetInt32(1), EM_DIV_UNTEN)
                    Case DIV_SOGYO_ID
                        sts.emSogyo = CType(sqlRead.GetInt32(1), EM_DIV_SOGYO)
                    Case DIV_PLGMODE_ID
                        sts.emPlg = CType(sqlRead.GetInt32(1), EM_DIV_PLG)
                End Select

            End While

            bRet = True
        Catch ex As Exception
            Log(LNO.ERR, String.Format("操業状態データ取得中に例外[{0}]", ex.Message))
            bRet = False
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try
        Return bRet
    End Function

    ''' <summary>
    ''' 稼動状態の表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspKadouStatus(ByVal sts As TYP_SOGYO_STATUS, ByVal lbl As Label)

        Select Case sts.emKadou

            Case EM_DIV_KADOU.DIV_KADOU_INIT
                lbl.Text = "初期化中"
                lbl.BackColor = g_ColorKadouInit
            Case EM_DIV_KADOU.DIV_KADOU_STOP
                lbl.Text = "停止中"
                lbl.BackColor = g_ColorKadouStop
            Case EM_DIV_KADOU.DIV_KADOU_START

                '運転状態を見る
                If sts.emUnten = EM_DIV_UNTEN.DIV_UNTEN_NOMAL Then
                    lbl.Text = "稼動中"
                Else
                    lbl.Text = "テスト中"
                End If
                lbl.BackColor = g_ColorKadouStart
            Case EM_DIV_KADOU.DIV_KADOU_RETRY
                lbl.Text = "ﾘﾄﾗｲ中"
                lbl.BackColor = g_ColorKadouRetry
        End Select

    End Sub
    ''' <summary>
    ''' 運転モードの表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspUntenStatus(ByVal sts As TYP_SOGYO_STATUS, ByVal lbl As Label)

        Select Case sts.emUnten
            Case EM_DIV_UNTEN.DIV_UNTEN_STOP
                lbl.Text = "停止"
                lbl.BackColor = g_ColorUntenStop
            Case EM_DIV_UNTEN.DIV_UNTEN_NOMAL
                lbl.Text = "通常運転"
                lbl.BackColor = g_ColorUntenNomal
            Case EM_DIV_UNTEN.DIV_UNTEN_SAMP
                lbl.Text = "サンプル撮影"
                lbl.BackColor = g_ColorUntenSamp
            Case EM_DIV_UNTEN.DIV_UNTEN_CAMERA
                lbl.Text = "カメラ調整"
                lbl.BackColor = g_ColorUntenCamera
            Case EM_DIV_UNTEN.DIV_UNTEN_SMYU
                lbl.Text = "シミュレーション"
                lbl.BackColor = g_ColorUntenSmyu
        End Select

    End Sub
    ''' <summary>
    ''' PLG選択の表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspPlgSelect(ByVal sts As TYP_SOGYO_STATUS, ByVal lbl As Label)

        Select Case sts.emPlg
            Case EM_DIV_PLG.DIV_PLG_LINE
                lbl.Text = "ラインPLG"
                lbl.BackColor = g_ColorPlgLine
            Case EM_DIV_PLG.DIV_PLG_TOP
                lbl.Text = "表校正"
                lbl.BackColor = g_ColorPlgTop
            Case EM_DIV_PLG.DIV_PLG_BOT
                lbl.Text = "裏校正"
                lbl.BackColor = g_ColorPlgBot
            Case EM_DIV_PLG.DIV_PLG_TEST
                lbl.Text = "TEST用パルス"
                lbl.BackColor = g_ColorPlgTest
        End Select

    End Sub

    ''' <summary>
    ''' 操業状態の表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspSogyoStatus(ByVal sts As TYP_SOGYO_STATUS, ByVal lbl As Label)

        Select Case sts.emSogyo
            Case EM_DIV_SOGYO.DIV_SOGYO_NON
                lbl.Text = "不明"
                lbl.BackColor = g_ColorSogyoNon
            Case EM_DIV_SOGYO.DIV_SOGYO_STOP
                lbl.Text = "休止中"
                lbl.BackColor = g_ColorSogyoStop
            Case EM_DIV_SOGYO.DIV_SOGYO_SLOW
                lbl.Text = "低速操業"
                lbl.BackColor = g_ColorSogyoSlow
            Case EM_DIV_SOGYO.DIV_SOGYO_RUN
                lbl.Text = "操業中"
                lbl.BackColor = g_ColorSogyoRun
        End Select

    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ イベント
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 選択行チェンジ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvKiki.SelectionChanged

        '選択を解除
        dgvKiki.CurrentCell.Selected = False

    End Sub

    ''' <summary>
    ''' 行選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.RowEnter

        '行の背景を変更
        dgvKiki.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen

    End Sub

    ''' <summary>
    ''' 行移動
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKiki_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.RowLeave

        '行の背景をデフォルト色に戻す
        dgvKiki.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty

    End Sub

    ''' <summary>
    ''' 選択行チェンジ
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGuide_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvGuide.SelectionChanged

        '選択を解除
        dgvGuide.CurrentCell.Selected = False

    End Sub

    ''' <summary>
    ''' 行選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGuide_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvGuide.RowEnter

        '行の背景を変更
        dgvGuide.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen

    End Sub

    ''' <summary>
    ''' 行移動
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGuide_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvGuide.RowLeave

        '行の背景をデフォルト色に戻す
        dgvGuide.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty

    End Sub

    '''' <summary>
    '''' 選択行チェンジ
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub dgvPc_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles dgvPc.SelectionChanged

    '    '選択を解除
    '    dgvPc.CurrentCell.Selected = False

    'End Sub

    '''' <summary>
    '''' 行選択
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub dgvPc_RowEnter(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPc.RowEnter

    '    '行の背景を変更
    '    dgvPc.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.LightGreen

    'End Sub

    '''' <summary>
    '''' 行移動
    '''' </summary>
    '''' <param name="sender"></param>
    '''' <param name="e"></param>
    '''' <remarks></remarks>
    'Private Sub dgvPc_RowLeave(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPc.RowLeave

    '    '行の背景をデフォルト色に戻す
    '    dgvPc.Rows(e.RowIndex).DefaultCellStyle.BackColor = Color.Empty

    'End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 強制解除ボタン
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' 強制解除ボタンが押されたときの処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvkiki_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKiki.CellContentClick
        Dim dgv As DataGridView = CType(sender, DataGridView)

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub
        Dim nKikiNo As Integer = CInt(dgv(en_KikiList.iNo, e.RowIndex).Value)
        Dim strID As String = ""

        ' 列インデックスでどの列がクリックされたか判別
        Select Case e.ColumnIndex
            Case en_KikiList.iBtn      '強制解除ボタン
                If EM_DGVBTN_TYPE.emBtnNG = CType(dgv(en_KikiList.iBtn, e.RowIndex).Tag, EM_DGVBTN_TYPE) Then
                    Exit Sub
                End If
                If Windows.Forms.DialogResult.OK = DspMsg(nKikiNo) Then
                    If Not KikiData_Init(nKikiNo) Then
                        MsgBox("強制解除 失敗 ID=" & nKikiNo)
                    End If
                Else
                End If
        End Select
    End Sub
    ''' <summary>
    ''' 機器状態強制解除
    ''' </summary>
    ''' <param name="nKikiID">機器ID</param>	
    ''' <remarks></remarks>
    Private Function KikiData_Init(ByVal nKikiID As Integer) As Boolean
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sSQL As String
        Try
            '' DB接続
            If Not tcls_DB.Open() Then Return False


            '-------------------------------
            '' SQL生成
            sSQL = "UPDATE " & DB_STATUS_INF
            sSQL &= " SET "
            '''' アップデートは、疵情報のみ

            sSQL &= tClass_SQLServer.sqlAddUpdate("機器状態", 0)
            tClass_SQLServer.sqlColumnTrim(sSQL)

            sSQL &= " WHERE 機器ID=" & nKikiID

            '-------------------------------
            '' 実行
            If 0 <= tcls_DB.DMLExecute(sSQL) Then
                Log(LNO.MSG, "機器状態強制解除 実施 [ID=" & nKikiID & "]")
                LogEx(LNO.MSG, "機器状態強制解除 実施 [ID=" & nKikiID & "]")
                Return True
            Else
                Log(LNO.ERR, "機器状態強制解除 失敗 [ID=" & nKikiID & "]")
                Return False
            End If

        Catch ex As Exception
            Log(LNO.ERR, "機器状態強制解除 異常 [ID=" & nKikiID & "][" & ex.Message & "]")
            Return False
        Finally
            tcls_DB.Close()
            tcls_DB.Dispose()
        End Try
        Return True
    End Function

    ''' <summary>
    ''' 確認メッセージ
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function DspMsg(ByVal nKikiID As Integer) As DialogResult
        Dim strMsg As String    'メッセージ 
        Dim bRet As DialogResult


        'PC名がない場合は、システム終了
        strMsg = "機器ID = " & nKikiID & vbCrLf
        strMsg &= vbCrLf
        strMsg &= "を強制解除します。" & vbCrLf
        strMsg &= vbCrLf
        strMsg &= "よろしいですか？" & vbCrLf
        strMsg &= vbCrLf
        strMsg &= "注）定周期などでセットしている機器IDは" & vbCrLf
        strMsg &= "    すぐに異常になることもあります。" & vbCrLf

        Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "確認", , True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)

        bRet = frmMsg.ShowDialog()

        Return bRet

    End Function



    '***************************************************************************
    ' タイマーイベント
    '***************************************************************************
    ''' <summary>
    ''' 定周期タイマー
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tmrDsp_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timGetStatus.Tick

        'DB接続チェック
        If Not OpenCheck(TASKINI_NAME, INI_DB) Then
            Log(LNO.ERR, "DB接続エラー(Openチェック) [" & DB_STATUS_MASTER & "]")
            Exit Sub
        End If

        'データ表示
        Call DspData_PC()
        Call DspData_Status()
        Call DspData_Guidance()
        Call DspData_SogyoStatus()
        Call GetSecurityData()
    End Sub


    '***************************************************************************
    ' ファンクションキー関連
    '***************************************************************************
    ''' <summary>
    ''' ファンクション切替
    ''' </summary>
    ''' <param name="FncNo">ファンクションNo (0オリジン)</param>
    ''' <param name="enable">ファンクションボタンの有効無効状態</param>
    ''' <param name="onKey">同時に押されているキー</param>
    ''' <remarks></remarks>
    Private Sub frmLog_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton
        '' デバッグログ表示
        If 1 - 1 = FncNo AndAlso onKey = Keys.Shift Then
            ShowDebugLog()
            Exit Sub
        End If

        '' ファンクションボタン有効項目のみ
        If Not enable Then Exit Sub

        '' ファンクション切替
        Select Case FncNo                       ' 0オリジン

            Case 0                              'F1
                ' 故障ログ画面直接表示
                Dim frm As New frmLog
                frm.Owner = Me.my_frmParent
                frm.SetParentForm(Me.my_frmParent)
                Me.SetParentForm(Nothing)
                frm.Show()
                ExecFromClose()

            Case 1                              'F2
                m_frmSecurityDisp.ShowDialog()

            Case 2                              'F3
                m_nRadKubun = EM_RAD_KUBUN.nErr
                DspData_Status()
                SetButtonColor()

            Case 3                              'F4
                m_nRadKubun = EM_RAD_KUBUN.nDefult
                DspData_Status()
                SetButtonColor()

            Case 4                              'F5
                ' 異常状態表示
                Dim frm As New frmStart
                frm.Owner = Me
                frm.ShowDialog()
                frm.Dispose()
                frm = Nothing

            Case 5                              'F6
                ' 基本動作一覧
                Dim frm As New frmBaseList
                frm.SetParentForm(Me)
                frm.Owner = Me
                frm.Show()
                Me.Visible = False

            Case 6                              'F7
                ' エッジ検出履歴画面表示
                Dim frm As New frmEdgeList
                frm.ShowDialog()
                frm.Dispose()
                frm = Nothing

            Case 7                              'F8
                ' 疵個数トレンドグラフ画面表示
                Dim frmGraph As New frmDefIncidenceGraph(gcls_Log)
                frmGraph.SetParentForm(Me)
                frmGraph.Owner = Me
                frmGraph.Show()
                Me.Visible = False

            Case 8                              'F9

            Case 9                              'F10
                ' 画面印刷
                Call PrintOut()

            Case 10                             'F11                
                ' F11 ハードコピー
                Call HardCopy(IMG_PATH, "RAS画面_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)

            Case 11
                MyBase.ExecFromClose()          ' 終了
        End Select
    End Sub

    ''' <summary>
    ''' 異常情報のみボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnErrOnly_Click(sender As System.Object, e As System.EventArgs) Handles btnErrOnly.Click
        m_nRadKubun = EM_RAD_KUBUN.nErr
        DspData_Status()
        SetButtonColor()
    End Sub

    ''' <summary>
    ''' 全てボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnErrAll_Click(sender As System.Object, e As System.EventArgs) Handles btnErrAll.Click
        m_nRadKubun = EM_RAD_KUBUN.nDefult
        DspData_Status()
        SetButtonColor()
    End Sub

    ''' <summary>
    ''' 「異常情報のみ」「全て」ボタン色替え処理
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetButtonColor()
        If m_nRadKubun = EM_RAD_KUBUN.nErr Then
            btnErrOnly.BackColor = Color.Cyan
            btnErrAll.BackColor = Color.Empty
        Else
            btnErrOnly.BackColor = Color.Empty
            btnErrAll.BackColor = Color.Cyan
        End If
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ログオフ、再起動、シャットダウン関連
    '/ /////////////////////////////////////////////////////////////////////////////////

    ''' <summary>
    ''' 全PC一括ログオフボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllPc_LogOff_Click(sender As System.Object, e As System.EventArgs) Handles btnAllPc_LogOff.Click
        ' 遠隔時、デバッグ時は何もしない
        If Not (NOA_ONLINE = g_NOA Or NOA_DEBUG = g_NOA Or NOA_KAISEKI = g_NOA) Then
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' リセット確認
        '-----------------------------------------------------------------------
        Dim nResetMode As Windows.Forms.DialogResult                            ' リセットモード (Yes:ログオフ No:再起動 Cancel:中止)
        nResetMode = CheckAllMsg("ログオフ")                            ' リセット確認ダイアログ表示
        If nResetMode = Windows.Forms.DialogResult.Cancel Then
            Exit Sub
        End If

        If Windows.Forms.DialogResult.OK <> CheckPass() Then                     ' パスワード確認。違ってたらキャンセル
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' リセット要求送信
        '-----------------------------------------------------------------------
        Call SendResetMail(FACT_KS_MASTER_08, "*")
    End Sub

    ''' <summary>
    ''' 全PC一括再起動ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllPc_ReStart_Click(sender As System.Object, e As System.EventArgs) Handles btnAllPc_ReStart.Click
        ' 遠隔時は何もしない
        If NOA_OFFLINE = g_NOA Then
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' リセット確認
        '-----------------------------------------------------------------------
        Dim nResetMode As Windows.Forms.DialogResult                            ' リセットモード (Yes:ログオフ No:再起動 Cancel:中止)
        nResetMode = CheckAllMsg("再起動")                            ' リセット確認ダイアログ表示
        If nResetMode = Windows.Forms.DialogResult.Cancel Then
            Exit Sub
        End If

        If Windows.Forms.DialogResult.OK <> CheckPass() Then                     ' パスワード確認。違ってたらキャンセル
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' リセット要求送信
        '-----------------------------------------------------------------------
        Call SendResetMail(FACT_KS_MASTER_07, "*")
    End Sub

    ''' <summary>
    ''' 全PC一括シャットダウンボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAllPc_ShutDown_Click(sender As System.Object, e As System.EventArgs) Handles btnAllPc_ShutDown.Click
        Dim sts As TYP_SOGYO_STATUS = Nothing
        ' 遠隔時は何もしない
        If NOA_OFFLINE = g_NOA Then
            Exit Sub
        End If

        If Get_SogyoStatus(sts) Then
            'システム状態をチェック
            If EM_DIV_KADOU.DIV_KADOU_STOP = sts.emKadou Then

                '停止中
                Call ExitSystem(True)           ' F08 システム終了
            Else

                '停止でない
                Call ExitSystem(False)          ' システム終了
            End If
        Else
            Call ExitSystem(True)           ' F08 システム終了
        End If
    End Sub



    ''' <summary>
    ''' PC情報データグリッドビュークリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvPc_CellClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvPc.CellContentClick

        Dim nResetMode As Windows.Forms.DialogResult                    ' リセットモード (Yes:実行 Cancel:中止)
        Dim strDspPcName As String                                      ' メッセージに表示するPC名
        Dim strPcName As String = Nothing                               ' PC名取得
        Dim nPcKind As Integer = 0
        Dim emBtnType As EM_BTN_TYPE
        Dim strTypeMsg As String = ""

        ' 行がクリックされていなければ終了
        If e.RowIndex < 0 Then Exit Sub

        If e.ColumnIndex = en_PcList.emBtn1 Then
            emBtnType = EM_BTN_TYPE.emBtn_LogOff
            strTypeMsg = "ログオフ"
        ElseIf e.ColumnIndex = en_PcList.emBtn2 Then
            emBtnType = EM_BTN_TYPE.emBtn_Reset
            strTypeMsg = "再起動"
        ElseIf e.ColumnIndex = en_PcList.emBtn3 Then
            emBtnType = EM_BTN_TYPE.emBtn_Exit
            strTypeMsg = "シャットダウン"
        Else
            Exit Sub
        End If

        ' 遠隔時は何もしない
        If NOA_OFFLINE = g_NOA Then
            Exit Sub
        End If
        '押下不可時は何もしない
        If EM_DGVBTN_TYPE.emBtnNG = CType(dgvPc(e.ColumnIndex, e.RowIndex).Tag, EM_DGVBTN_TYPE) Then
            Exit Sub
        End If

        nPcKind = CInt(dgvPc.Item(en_PcList.emNo, e.RowIndex).Value)
        strPcName = CStr(dgvPc.Item(en_PcList.emPCName2, e.RowIndex).Value)

        'クリックした行からPC名を取得して、リセット及びログオフを行う
        strDspPcName = CStr(dgvPc.Item(en_PcList.emPCName, e.RowIndex).Value)

        '-----------------------------------------------------------------------
        ' リセット確認
        '-----------------------------------------------------------------------
        nResetMode = CheckMsg(strDspPcName, strTypeMsg)                            ' リセット確認ダイアログ表示
        If nResetMode = Windows.Forms.DialogResult.Cancel Then
            Exit Sub
        End If

        If Windows.Forms.DialogResult.OK <> CheckPass() Then                     ' パスワード確認。違ってたらキャンセル
            Exit Sub
        End If

        '-----------------------------------------------------------------------
        ' リセット要求送信
        '-----------------------------------------------------------------------
        Select Case emBtnType
            Case EM_BTN_TYPE.emBtn_LogOff
                Call SendResetMail(FACT_KS_MASTER_08, strPcName)
            Case EM_BTN_TYPE.emBtn_Reset
                Call SendResetMail(FACT_KS_MASTER_07, strPcName)
            Case EM_BTN_TYPE.emBtn_Exit
                Call SendResetMailSub(FACT_KS_SLAVE_04, strPcName)
            Case Else
        End Select

    End Sub

    ''' <summary>
    ''' PCリセットorシステム終了メール送信
    ''' </summary>
    ''' <param name="nEventNo">イベントNo</param>
    ''' <param name="strPcName">PC名</param>
    ''' <remarks></remarks>
    Private Sub SendResetMail(ByVal nEventNo As Integer, Optional ByVal strPcName As String = "")
        Dim que As KizuLibCLI.COMMON_QUE_STR = Nothing

        que.nEventNo = nEventNo                                                 ' PCリセット
        que.nLineNo = m_nLineID
        que.data = strPcName

        Call KizuLibCLI.KizuMod.Send_Mail(m_sMasterTask, m_sMasterPC, que)      ' メール送信 
    End Sub

    ''' <summary>
    ''' マスターPC以外のシャットダウンメール送信
    ''' </summary>
    ''' <param name="nEventNo">イベントNo</param>
    ''' <param name="strPcName">PC名</param>
    ''' <remarks></remarks>
    Private Sub SendResetMailSub(ByVal nEventNo As Integer, ByVal strPcName As String)
        Dim que As KizuLibCLI.COMMON_QUE_STR = Nothing

        que.nEventNo = nEventNo                                                 ' PCリセット
        que.nLineNo = m_nLineID
        que.data = strPcName

        Call KizuLibCLI.KizuMod.Send_Mail(m_sSubTask, strPcName, que)      ' メール送信 
    End Sub

    ''' <summary>
    ''' 確認
    ''' </summary>
    ''' <param name="strPcName">メッセージに表示するPC名</param>
    ''' <param name="strTypMsg">メッセージに表示するイベント(ログオフ、再起動、シャットダウン)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckMsg(ByVal strPcName As String, ByVal strTypMsg As String) As DialogResult
        Dim strMsg As String                                                    ' 表示メッセージ 

        strMsg = strPcName & "の" & strTypMsg & "を実行しますか？"

        ' 共通メッセージ
        strMsg &= vbCrLf & vbCrLf
        strMsg &= "はい   ： パソコンの" & strTypMsg & "を実行" & vbCrLf
        strMsg &= "ｷｬﾝｾﾙ  ： 選択をキャンセル" & vbCrLf

        Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "確認", , True, True, DialogResult.Yes, DialogResult.Cancel)
        Dim bRet As DialogResult = frmMsg.ShowDialog()
        Return bRet
    End Function

    ''' <summary>
    ''' 確認
    ''' </summary>
    ''' <param name="strTypMsg">メッセージに表示するイベント(ログオフ、再起動、シャットダウン)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function CheckAllMsg(ByVal strTypMsg As String) As DialogResult
        Dim strMsg As String                                                    ' 表示メッセージ 

        strMsg = "全PCの" & strTypMsg & "を実行しますか？"

        ' 共通メッセージ
        strMsg &= vbCrLf & vbCrLf
        strMsg &= "はい   ： パソコンの" & strTypMsg & "を実行" & vbCrLf
        strMsg &= "ｷｬﾝｾﾙ  ： 選択をキャンセル" & vbCrLf

        Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "確認", , True, True, DialogResult.Yes, DialogResult.Cancel)
        Dim bRet As DialogResult = frmMsg.ShowDialog()
        Return bRet
    End Function

    ''' <summary>
    ''' システム終了確認
    ''' </summary>
    ''' <param name="bStopFlg">停止フラグ(True:停止,False:停止でない)</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function CheckShatdown(Optional ByVal bStopFlg As Boolean = False) As DialogResult
        Dim strMsg As String    'メッセージ 

        If bStopFlg Then                                                        ' PC名がない場合は、システム終了
            strMsg = "全PCのシャットダウンを実行しますか？"
        Else
            strMsg = "システムが稼動中です。" &
            vbNewLine & "それでも、全PCのシャットダウンを実行しますか？"
        End If

        Dim frmMsg As New tClassForm.frmPop_MsgBox(strMsg, "確認", , True, True, Windows.Forms.DialogResult.OK, Windows.Forms.DialogResult.Cancel)
        Dim bRet As DialogResult = frmMsg.ShowDialog()
        Return bRet
    End Function

    ''' <summary>
    ''' パスワード確認
    ''' </summary>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Private Function CheckPass() As DialogResult
        ' パスワード未設定の場合
        If "" = m_strResetPass Then
            Return DialogResult.OK
        End If

        Dim frmPass As New tClassForm.frmPop_Pass(m_strResetPass)
        Dim bRet As DialogResult = frmPass.ShowDialog()                         ' パスワード確認
        Return bRet
    End Function

    ''' <summary>
    ''' システム終了処理
    ''' <param name="bStopFlg">停止中フラグ(True:停止中,False:それ以外)</param>
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub ExitSystem(ByVal bStopFlg As Boolean)
        If Windows.Forms.DialogResult.OK <> CheckShatdown(bStopFlg) Then         ' リセット確認
            Exit Sub
        End If

        If Windows.Forms.DialogResult.OK <> CheckPass() Then                     ' パスワード確認
            Exit Sub
        End If
        Call SendResetMail(FACT_KS_MASTER_06)                                   ' システム終了メール送信
    End Sub

    ''' <summary>
    ''' 異常範囲文言取得
    ''' </summary>
    ''' <param name="nNo"></param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetErrRangeString(ByVal nNo As Integer) As String
        Dim strItem As String = ""
        For ii As Integer = 0 To m_lstLovItem.Count - 1
            If m_lstLovItem(ii).ItemNo = nNo Then
                strItem = m_lstLovItem(ii).DispData
                Exit For
            End If
        Next
        Return strItem
    End Function

    ''' <summary>
    ''' RAS画面に表示する表示色を取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetColor()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String = ""

        'DBオープン
        If Not tcls_DB.Open() Then Exit Sub

        'SQL文作成
        strSQL = ""
        strSQL &= " SELECT RAS正常色,"
        strSQL &= "        RAS異常色,"
        strSQL &= "        RAS注意色"
        strSQL &= " FROM " & DB_COMMON_MASTER
        'strSQL &= " WHERE LINEID = " & KizuLibCLI.KizuMod.GetLineID()

        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            ' データ表示
            sqlRead.Read()

            m_clOK = Color.FromArgb(CInt(sqlRead.GetValue(0)))
            m_clErr = Color.FromArgb(CInt(sqlRead.GetValue(1)))
            m_clCare = Color.FromArgb(CInt(sqlRead.GetValue(2)))

        Catch ex As Exception

        Finally

            ' DB開放
            If Not sqlRead Is Nothing Then sqlRead.Close()
            Call tcls_DB.Dispose()

        End Try

    End Sub

    ''' <summary>
    ''' 異常機器ﾌﾞﾘﾝｸ処理
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub timErrCheck_Tick(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles timErrCheck.Tick
        Dim ii As Integer

        '色設定(点滅させるため)
        m_clOnLineNG = CType(IIf(m_clErr = m_clOnLineNG, m_clNormal, m_clErr), Color)
        m_clOffLineNG = CType(IIf(m_clCare = m_clOffLineNG, m_clNormal, m_clCare), Color)

        'PC情報点滅
        For ii = 0 To dgvPc.RowCount - 1
            Dim nSts As Integer = CInt(dgvPc(en_PcList.emKiki, ii).Value)
            If 0 <> nSts Then
                If "ｵﾝﾗｲﾝ" = CStr(dgvPc(en_PcList.emOnOff, ii).Value) Then
                    'dgvPc(en_PcList.emOnOff, ii).Style.BackColor = m_clOnLineNG
                    dgvPc.Rows(ii).DefaultCellStyle.BackColor = m_clOnLineNG
                Else
                    'dgvPc(en_PcList.emOnOff, ii).Style.BackColor = m_clOffLineNG
                    dgvPc.Rows(ii).DefaultCellStyle.BackColor = m_clOffLineNG
                End If
            Else
                'dgvPc(en_PcList.emOnOff, ii).Style.BackColor = m_clOK
                dgvPc.Rows(ii).DefaultCellStyle.BackColor = m_clOK
            End If
        Next

        '状態一覧点滅
        'For ii = 0 To dgvKiki.RowCount - 1
        '    Dim str As String = CStr(dgvKiki(en_KikiList.iKiki, ii).Value)
        '    If ucStatusAria.DspKiki(KizuForm.EM_DIV_KIKI.DIV_KIKI_JYU) = str Then
        '        dgvKiki(en_KikiList.iKiki, ii).Style.BackColor = m_clOnLineNG
        '    ElseIf ucStatusAria.DspKiki(KizuForm.EM_DIV_KIKI.DIV_KIKI_KEI) = str Then
        '        dgvKiki(en_KikiList.iKiki, ii).Style.BackColor = m_clOffLineNG
        '    ElseIf ucStatusAria.DspKiki(KizuForm.EM_DIV_KIKI.DIV_KIKI_OK) = str Then
        '        dgvKiki(en_KikiList.iKiki, ii).Style.BackColor = m_clOK
        '    Else
        '        dgvKiki(en_KikiList.iKiki, ii).Style.BackColor = Color.Empty
        '    End If
        'Next

        '立入制限
        For ii = 0 To m_lstStatus.Count - 1
            If m_dctSecurity.ContainsKey(m_lstStatus(ii).nId) Then
                Dim clrDspColor As Color
                If 0 <> m_lstStatus(ii).nStatus Then
                    clrDspColor = m_clOnLineNG
                Else
                    clrDspColor = m_clOK
                End If
                'ステータス状態セット
                m_frmSecurityDisp.SetData(m_dctSecurity(m_lstStatus(ii).nId).Name, m_lstStatus(ii).nStatus, clrDspColor)
            End If

        Next

        'F2ボタン(立入制限)ボタン色変更
        If m_frmSecurityDisp.ChkF02btn Then
            btnF02.BackColor = m_clNormal
        Else
            btnF02.BackColor = m_clOnLineNG
        End If

    End Sub




End Class
