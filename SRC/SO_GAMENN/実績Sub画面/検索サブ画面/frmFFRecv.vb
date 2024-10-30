Option Strict Off
Imports tClassLibrary


Public Class frmFFRecv

    Private Const MAX_KEN_COL As Integer = 5
    ' 0.1m化 対応 --->>>
    Private Const MAX_DEFECT_COL As Integer = 11           'データ列MAX
    Private Const MAX_COIL_ROW As Integer = 21             'コイル情報行MAX
    Private Const MAX_TUHAN_COL As Integer = 6
    Private Const MAX_RCL_ROW As Integer = 5               '
    Private Const MAX_BUNKATU_COL As Integer = 6
    ' 0.1m化 対応 <<<---

    '前工程疵情報
    Private Structure FF_MAE_DEFECT_INF
        Dim SetNo As String             'FF前工程疵情報格納位置(ツール用の情報)
        Dim Kind As String              '疵種
        Dim Grade As String             '疵グレード
        Dim Kizukbn As String           '疵検出工程
        Dim kbn As String               '疵区分
        Dim DefectPos As String         '疵長さ方向位置[0.1m]
        Dim WidPos As String            '疵巾方向位置
        Dim Len As String               '疵長さ[0.1m]
        Dim No As String                '疵NO
        Dim SysKbn As String            'システム区分
        Dim Filler As String            '予備
    End Structure

    'コイル情報グリッド設定
    Private Enum EM_COIL_COL_ID
        iTiTle = 0      'コイル情報名
        iData           'データ
    End Enum

    Private Enum EM_COIL_ROW_ID
        iReiCoilNo = 0     '// 冷延コイル番号
        iReiCoilG          '// 冷延コイル重量
        iReiCoilL          '// 冷延コイル長さ
        iReiCoilAtu        '// 冷延厚
        iItaatuMin         '// 板厚　公差　MIN
        iItaatuMax         '// 板厚　公差　MAX
        iMukesaki1i        '// 向け先１　板巾
        iKensa             '// 検査基準
        iMaki              '// 巻き方向
        iZairyou           '// 材料巾
        iMukesaki1kMin     '// 向け先１　板巾公差　MIN
        iMukesaki1kMax     '// 向け先１　板巾公差　MAX
        ' 0.1m化 対応 --->>>
        'iSplit             '// スプリット認識
        ' 0.1m化 対応 <<<---
        iMukesaki1L        '// 向け先１　長さ(ロッド枚数)
        iMukesaki1N        '// 向け先１　内径
        iItaatu            '// 板厚
        iIrigawa           '// 入側内径
        iMukesaki1KL       '// 向け先１　剪断長
        iMakoutei          '// 前工程コード
        iRealMakoutei      '// 実前工程コード
        iTCMFlag           '// 厚手/TCM直送材フラグ
        'iYobi             '// 予備
    End Enum

    ' 0.1m化 対応 --->>>
    Private Enum EM_RCL_ROW_ID
        iKizukenNo = 0      '仮想RCL疵検管理No
        iCoilNo             '冷延コイルNo
        iFromSai            '採取開始位置長さ（From） [0.1m]
        iToSai              '採取終了位置長さ（To）	[0.1m]
        iKoban              '採取子番
    End Enum
    ' 0.1m化 対応 <<<---

    ' 検出情報グリッド設定
    Private Enum EM_KEN_COL_ID
        iProc = 0           '検出工程
        iKizuKenNo          '疵検管理No
        iMode               '疵検処理モード
        iCnt                '疵設定個数
        iStatus             '疵個数オーバーステータス
    End Enum

    ' 疵情報グリッド設定
    Private Enum EM_DEFECT_COL_ID
        iNO = 0             'NO
        iTypeID             '疵種
        iGradeID            '疵グレード
        iKizuOut            '検出区分
        iKizuDiv            '疵区分
        iTotalLen           '疵総長さ方向位置
        iHaba               '疵巾方向位置
        iLen                '疵長さ
        iKizuNo             '疵No
        iSysKbn             'システム区分
        iSetNO              '格納位置
        iFiller             '予備
    End Enum

    '通板実績
    Private Enum EM_TUHAN_COL_ID
        iJissekiCode = 0    '実績工程コード
        iMeireiNo           '命令番号
        iThuhanCnt          '通板回数
        iGenpinNo           '入側現品管理NO
        iCoilLen            '入側コイル長さ
        ' 0.1m化 対応 --->>>
        iFiller             '予備
        ' 0.1m化 対応 <<<---
    End Enum

    '分割情報
    Private Enum EM_BUNKATU_COL_ID
        iNo = 0
        iMakiHoukou         '実績巻方向(1:上出上捲, 2:上出下捲, 3:下出上捲, 4:下出下捲)
        iCoilLen            'コイル長
        iCutkbn             'カット場所区分(I:入側カット, O:出側カット)
        iGenpinNo           '分割現品管理NO
        ' 0.1m化 対応 --->>>
        iFiller             '予備
        ' 0.1m化 対応 <<<---
    End Enum

    ' 0.1m化 対応 --->>>
    ' 仮想RCL疵検管理No
    Private Enum EM_RCL_COL_ID
        iKizukenNo = 0      '仮想RCL疵検管理No
        iCoilNo             '疵検管理No
        iFromSai            '冷延コイルNo
        iToSai              '採取開始位置長さ（From） [0.1m]
        iKoban              '採取終了位置長さ（To）	[0.1m]
    End Enum
    ' 0.1m化 対応 <<<---

    Private m_mode As Integer               '要求モード(1:オンライン用、0:ツール用）
    Private m_strKizukenNo As String        '疵検管理No

    Private m_typData_O As FF_MAE_JISSEKI = Nothing     '前工程実績_表
    Private m_typData_U As FF_MAE_JISSEKI = Nothing     '前工程実績_裏

    ''' <summary>
    ''' パラメータに要求モード(1:オンライン用、0:ツール用）
    ''' </summary>
    ''' <param name="mode">1:オンライン用、0:ツール用</param>
    ''' <param name="strKizukenNo">管理No</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal mode As Integer, Optional ByVal strKizukenNo As String = "")
        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
        'メンバ変数セット
        m_mode = mode                   'パラメータに要求モード(1:オンライン用、0:ツール用）
        m_strKizukenNo = strKizukenNo   '管理No

        '-------------------------------------------------------------------
        ' ファンクションキー設定
        '-------------------------------------------------------------------
        AddFunctionButton(btnEnd, 12 - 1)
    End Sub

    '***************************************************************************
    ' キーイベント
    '***************************************************************************
    ''' <summary>
    ''' ファンクションボタン
    ''' </summary>
    ''' <param name="FncNo">ファンクションボタンNo 0オリジン</param>
    ''' <param name="onKey"></param>
    Private Sub frm_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        Try
            Select Case FncNo                                                   ' ファンクションボタン (0オリジン)
                '---------------------------------------------------------------
                Case 12 - 1                                                     ' 終了
                    btnEnd_Click(Nothing, Nothing)
            End Select
        Catch ex As Exception
        End Try
    End Sub
    ''' <summary>
    ''' 画面終了
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)

        '画面終了
        Me.Close()

    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmFFRecv_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load

        Dim ii As Integer

        Me.tProp_Moveable = True

        'タイトル設定
        Call SetTitleName(Me)

        'ポップアップ画面 低解像度対応
        Call SetPcHeight(Me)

        ''画面初期化
        Call InitDsp()

        '工程種別
        cmbKindO.Items.Add("CC疵情報")
        cmbKindO.Items.Add("HOT疵情報")
        cmbKindO.Items.Add("PL疵情報")
        cmbKindO.Items.Add("TCM穴情報")
        ' 0.1m化 対応 --->>>
        'cmbKindO.Items.Add("TCM疵情報")
        cmbKindO.Items.Add("厚手疵情報 CAPL")
        ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
        'cmbKindO.Items.Add("厚手疵情報 無し 2FLLラミ前")
        cmbKindO.Items.Add("TCM疵情報")
        ' 20221021 2TCM実機化に伴う下工程改造 小島 <<<---
        cmbKindO.Items.Add("厚手疵情報 無し 2FLLラミ後")
        'cmbKindO.Items.Add("疵情報予備")   '2013.03.19 川原
        ' 0.1m化 対応 <<<---
        cmbKindO.SelectedIndex = 0

        cmbKindU.Items.Add("CC疵情報")
        cmbKindU.Items.Add("HOT疵情報")
        cmbKindU.Items.Add("PL疵情報")
        cmbKindU.Items.Add("TCM穴情報")
        ' 0.1m化 対応 --->>>
        'cmbKindU.Items.Add("TCM疵情報")
        cmbKindU.Items.Add("厚手疵情報 CAPL")
        ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
        'cmbKindU.Items.Add("厚手疵情報 無し 2FLLラミ前")
        cmbKindU.Items.Add("TCM疵情報")
        ' 20221021 2TCM実機化に伴う下工程改造 小島 <<<---
        cmbKindU.Items.Add("厚手疵情報 無し 2FLLラミ後")
        'cmbKindU.Items.Add("疵情報予備")   '2013.03.19 川原
        ' 0.1m化 対応 <<<---
        cmbKindU.SelectedIndex = 0

        For ii = 0 To MAX_FF_MAE_THAN_CNT - 1
            cmbTuhanO.Items.Add(CStr(ii + 1))
            cmbTuhanU.Items.Add(CStr(ii + 1))
        Next

        cmbTuhanO.SelectedIndex = 0
        cmbTuhanU.SelectedIndex = 0

        'IDリスト作成
        Call MakeIDList()

        If 1 = m_mode Then                  ' オンラインモード
            btnRead.Visible = False         ' 読込ボタン「非表示」
            cmbID.DropDownStyle = ComboBoxStyle.DropDownList
            If cmbID.Items.Count <> 0 Then
                cmbID.SelectedIndex = 0
            End If
        Else                                ' ツールモード
            btnRead.Visible = True          ' 読込ボタン「表示」
        End If

    End Sub

    ''' <summary>
    ''' IDリスト作成
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub MakeIDList()

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文

        strSQL = ""

        If m_mode = 1 Then           'オンラインモード
            ' オンラインモードの場合は、管理No.から検索する
            strSQL &= " SELECT A.ID"
            strSQL &= " FROM " & DB_BUF_FF_R_MAEINF & " as A"
            strSQL &= " INNER JOIN " & DB_COIL_INF & " as B"
            strSQL &= " ON A.キー情報 = B.キー情報"
            strSQL &= " AND B.管理No = '" & m_strKizukenNo & "'"
            strSQL &= " ORDER BY A.登録日付 DESC"
        Else                         'ツールモード
            'strSQL &= " SELECT TOP 200 ID"
            strSQL &= " SELECT ID"
            strSQL &= " FROM " & DB_BUF_FF_R_MAEINF
            strSQL &= " ORDER BY 登録日付 DESC"
        End If

        'DBオープン
        If Not tcls_DB.Open() Then
            Exit Sub
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Exit Sub

        Try
            cmbID.Items.Clear()
            ' データ取得
            While sqlRead.Read()

                cmbID.Items.Add(sqlRead.GetValue(0).ToString)

            End While

        Catch ex As Exception
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try


    End Sub

    ''' <summary>
    ''' 画面初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub InitDsp()

        '工程コード
        lblProcO.Text = ""

        '命令No
        lblMeiNoO.Text = ""

        '工程コード
        lblProcU.Text = ""

        '命令No
        lblMeiNoU.Text = ""

        'コイル情報グリッド
        Call dgvCoilInitialize(dgvCoilO)
        Call dgvCoilInitialize(dgvCoilU)

        '疵検出グリッド初期化
        Call dgvKenInitialize(dgvKenO)
        Call dgvKenInitialize(dgvKenU)

        '疵情報グリッド初期化
        Call dgvDefectInitialize(dgvDefectO)
        Call dgvDefectInitialize(dgvDefectU)

        ' 0.1m化 対応 --->>>
        Call dgvRclInitialize(dgvRclO)
        Call dgvRclInitialize(dgvRclU)
        ' 0.1m化 対応 <<<---

        '通板実績
        Call dgvTuhanInitialize(dgvJisO)
        Call dgvTuhanInitialize(dgvJisU)

        '分割
        Call dgvBunkatuInitialize(dgvBunKatuO)
        Call dgvBunkatuInitialize(dgvBunKatuU)


    End Sub


    ''' <summary>
    ''' データ取得
    ''' </summary>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strID As String) As Boolean

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String    'SQL文
        Dim bytData1() As Byte  '生伝文バイナリデータ
        Dim bytData2() As Byte  '生伝文バイナリデータ


        '疵検管理Noあり
        strSQL = ""
        strSQL &= " SELECT 生伝文1,"
        strSQL &= "        生伝文2 "
        strSQL &= " FROM " & DB_BUF_FF_R_MAEINF
        strSQL &= " WHERE ID = '" & strID & "'"
        strSQL &= " ORDER BY 登録日付 DESC"

        'DBオープン
        If Not tcls_DB.Open() Then
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then Return False

        Try

            ' データ取得
            sqlRead.Read()

            '生伝文
            bytData1 = CType(sqlRead.GetValue(0), Byte())

            '生伝文
            bytData2 = CType(sqlRead.GetValue(1), Byte())

            tMod.MemSet(bytData1, 0, m_typData_O, tMod.SizeOf(m_typData_O))
            tMod.MemSet(bytData2, 0, m_typData_U, tMod.SizeOf(m_typData_U))

        Catch ex As Exception
            Return False
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        Return True

    End Function

    ''' <summary>
    ''' IDリスト選択
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbID_SelectedIndexChanged(ByVal sender As Object, ByVal e As System.EventArgs) Handles cmbID.SelectedIndexChanged
        Call btnRead_Click2(sender, e)
    End Sub

    Private Sub btnRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click

        Dim cmbIDText As String

        cmbIDText = cmbID.Text

        'IDリスト作成
        Call MakeIDList()

        cmbID.Text = cmbIDText

        Call btnRead_Click2(sender, e)

    End Sub

    Private Sub btnRead_Click2(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRead.Click

        'データ取得
        If GetData(cmbID.Text) Then

            'ヘッダー情報
            Call DspHeader()

            'コイル情報
            Call SetCoilData(m_typData_O.Hd, dgvCoilO)
            Call SetCoilData(m_typData_U.Hd, dgvCoilU)

            ' 0.1m化 対応 --->>>
            Call SetRclData(m_typData_O.VRcl, m_typData_O.FromTo, dgvRclO)
            Call SetRclData(m_typData_U.VRcl, m_typData_U.FromTo, dgvRclU)
            ' 0.1m化 対応 <<<---

            If -1 <> cmbKindO.SelectedIndex Then
                Call SetKenData(cmbKindO.SelectedIndex, 0, dgvKenO)
                Call SetDefectData(cmbKindO.SelectedIndex, 0, dgvDefectO)
            End If
            If -1 <> cmbKindU.SelectedIndex Then
                Call SetKenData(cmbKindU.SelectedIndex, 1, dgvKenU)
                ' 0.1m化 対応 --->>>
                Call SetDefectData(cmbKindU.SelectedIndex, 1, dgvDefectU)
                ' 0.1m化 対応 <<<---
            End If

            If -1 <> cmbTuhanO.SelectedIndex Then
                Call SetJisData(cmbTuhanO.SelectedIndex, dgvJisO, m_typData_O, dgvBunKatuO)
            End If

            If -1 <> cmbTuhanU.SelectedIndex Then
                Call SetJisData(cmbTuhanU.SelectedIndex, dgvJisU, m_typData_U, dgvBunKatuU)
            End If

        End If
    End Sub

    'ヘッダー情報表示
    Private Sub DspHeader()

        '工程コード
        lblProcO.Text = tMod.ByteToString(m_typData_O.Hd.Line_Code)

        '命令No
        lblMeiNoO.Text = tMod.ByteToString(m_typData_O.Hd.Meino)

        '工程コード
        lblProcU.Text = tMod.ByteToString(m_typData_U.Hd.Line_Code)

        '命令No
        lblMeiNoU.Text = tMod.ByteToString(m_typData_U.Hd.Meino)

    End Sub

    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvDefectInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
        With dgv
            .RowCount = MAX_FF_DFCT_CNT

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
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing         '

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = MAX_DEFECT_COL           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 35                                       ' 列ヘッダサイズ

            '列0 (アドレス) 疵検出区分
            .Columns(EM_DEFECT_COL_ID.iNO).HeaderText = "NO"
            .Columns(EM_DEFECT_COL_ID.iNO).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iNO).Width = 40
            .Columns(EM_DEFECT_COL_ID.iNO).SortMode = DataGridViewColumnSortMode.NotSortable

            '列1 (アドレス) 疵種
            .Columns(EM_DEFECT_COL_ID.iTypeID).HeaderText = "疵種"
            .Columns(EM_DEFECT_COL_ID.iTypeID).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iTypeID).Width = 50
            .Columns(EM_DEFECT_COL_ID.iTypeID).SortMode = DataGridViewColumnSortMode.NotSortable


            '列2 (アドレス) 疵グレード
            .Columns(EM_DEFECT_COL_ID.iGradeID).HeaderText = "疵" & vbNewLine & "ｸﾞﾚｰﾄﾞ"
            .Columns(EM_DEFECT_COL_ID.iGradeID).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iGradeID).Width = 60
            .Columns(EM_DEFECT_COL_ID.iGradeID).SortMode = DataGridViewColumnSortMode.NotSortable

            '列3 (アドレス) 疵検出工程
            .Columns(EM_DEFECT_COL_ID.iKizuOut).HeaderText = "疵検出" & vbNewLine & "工程"
            .Columns(EM_DEFECT_COL_ID.iKizuOut).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iKizuOut).Width = 60
            .Columns(EM_DEFECT_COL_ID.iKizuOut).SortMode = DataGridViewColumnSortMode.NotSortable


            '列4(アドレス) 疵区分
            .Columns(EM_DEFECT_COL_ID.iKizuDiv).HeaderText = "疵区分"
            .Columns(EM_DEFECT_COL_ID.iKizuDiv).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iKizuDiv).Width = 60
            .Columns(EM_DEFECT_COL_ID.iKizuDiv).SortMode = DataGridViewColumnSortMode.NotSortable


            '列5 (アドレス) 疵総長さ方向位置
            .Columns(EM_DEFECT_COL_ID.iTotalLen).HeaderText = "長手" & vbNewLine & "位置"
            .Columns(EM_DEFECT_COL_ID.iTotalLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iTotalLen).Width = 60
            .Columns(EM_DEFECT_COL_ID.iTotalLen).SortMode = DataGridViewColumnSortMode.NotSortable

            '列6 (アドレス) 疵巾方向位置
            .Columns(EM_DEFECT_COL_ID.iHaba).HeaderText = "巾" & vbNewLine & "位置"
            .Columns(EM_DEFECT_COL_ID.iHaba).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iHaba).Width = 60
            .Columns(EM_DEFECT_COL_ID.iHaba).SortMode = DataGridViewColumnSortMode.NotSortable


            '列7 (アドレス) 疵長さ
            .Columns(EM_DEFECT_COL_ID.iLen).HeaderText = "疵" & vbNewLine & "長さ"
            .Columns(EM_DEFECT_COL_ID.iLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iLen).Width = 50
            .Columns(EM_DEFECT_COL_ID.iLen).SortMode = DataGridViewColumnSortMode.NotSortable


            '列8 (アドレス) 疵No
            .Columns(EM_DEFECT_COL_ID.iKizuNo).HeaderText = "疵NO"
            .Columns(EM_DEFECT_COL_ID.iKizuNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iKizuNo).Width = 90
            .Columns(EM_DEFECT_COL_ID.iKizuNo).SortMode = DataGridViewColumnSortMode.NotSortable

            '列9 (アドレス) 疵No
            .Columns(EM_DEFECT_COL_ID.iSysKbn).HeaderText = "ｼｽﾃﾑ" & vbNewLine & "区分"
            .Columns(EM_DEFECT_COL_ID.iSysKbn).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iSysKbn).Width = 50
            .Columns(EM_DEFECT_COL_ID.iSysKbn).SortMode = DataGridViewColumnSortMode.NotSortable

            '列10 (アドレス) 格納位置
            .Columns(EM_DEFECT_COL_ID.iSetNO).HeaderText = "格納" & vbNewLine & "位置"
            .Columns(EM_DEFECT_COL_ID.iSetNO).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_DEFECT_COL_ID.iSetNO).Width = 60
            .Columns(EM_DEFECT_COL_ID.iSetNO).SortMode = DataGridViewColumnSortMode.NotSortable

            ''列11(アドレス) 
            '.Columns(EM_DEFECT_COL_ID.iFiller).HeaderText = "予備"
            '.Columns(EM_DEFECT_COL_ID.iFiller).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            '.Columns(EM_DEFECT_COL_ID.iFiller).Width = 60
            '.Columns(EM_DEFECT_COL_ID.iFiller).SortMode = DataGridViewColumnSortMode.NotSortable


        End With
    End Sub

    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvKenInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
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
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            .RowCount = 1

            '' 列定義
            .ColumnCount = MAX_KEN_COL           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 35                                       ' 列ヘッダサイズ

            '列0 (アドレス) 疵検出工程
            .Columns(EM_KEN_COL_ID.iProc).HeaderText = "疵検出" & vbNewLine & "工程"
            .Columns(EM_KEN_COL_ID.iProc).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iProc).Width = 60
            .Columns(EM_KEN_COL_ID.iProc).SortMode = DataGridViewColumnSortMode.NotSortable


            '列１ (アドレス) 疵検管理NO
            .Columns(EM_KEN_COL_ID.iKizuKenNo).HeaderText = "疵検管理NO"
            .Columns(EM_KEN_COL_ID.iKizuKenNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iKizuKenNo).Width = 150
            .Columns(EM_KEN_COL_ID.iKizuKenNo).SortMode = DataGridViewColumnSortMode.NotSortable


            '列２ (アドレス) 疵検処理モード
            .Columns(EM_KEN_COL_ID.iMode).HeaderText = "疵検処理ﾓｰﾄﾞ"
            .Columns(EM_KEN_COL_ID.iMode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iMode).Width = 80
            .Columns(EM_KEN_COL_ID.iMode).SortMode = DataGridViewColumnSortMode.NotSortable


            '列３ (アドレス) 疵設定個数
            .Columns(EM_KEN_COL_ID.iCnt).HeaderText = "疵設定" & vbNewLine & "個数"
            .Columns(EM_KEN_COL_ID.iCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iCnt).Width = 60
            .Columns(EM_KEN_COL_ID.iCnt).SortMode = DataGridViewColumnSortMode.NotSortable


            '列４ (アドレス) 疵個数オーバーステータス
            .Columns(EM_KEN_COL_ID.iStatus).HeaderText = "疵個数ｵｰﾊﾞｰｽﾃｰﾀｽ"
            .Columns(EM_KEN_COL_ID.iStatus).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_KEN_COL_ID.iStatus).Width = 101
            .Columns(EM_KEN_COL_ID.iStatus).SortMode = DataGridViewColumnSortMode.NotSortable


        End With
    End Sub

    ''' <summary>
    ''' 疵検出工程
    ''' </summary>
    ''' <param name="nIndex"></param>
    ''' <param name="nOuDiv"></param>
    ''' <param name="dgvData"></param>
    ''' <remarks></remarks>
    Private Sub SetKenData(ByVal nIndex As Integer, ByVal nOuDiv As Integer, ByVal dgvData As tCnt.tCnt_DataGridView)

        Dim typData As FF_MAE_TYPE_INF

        If 0 = nOuDiv Then
            typData = m_typData_O.Inf(nIndex)
        Else
            typData = m_typData_U.Inf(nIndex)
        End If

        dgvData(EM_KEN_COL_ID.iProc, 0).Value = tMod.ByteToString(typData.Kizukbn)
        dgvData(EM_KEN_COL_ID.iKizuKenNo, 0).Value = tMod.ByteToString(typData.Id)
        dgvData(EM_KEN_COL_ID.iMode, 0).Value = tMod.ByteToString(typData.Mode)
        dgvData(EM_KEN_COL_ID.iCnt, 0).Value = tMod.ByteToString(typData.DataDefault)
        dgvData(EM_KEN_COL_ID.iStatus, 0).Value = tMod.ByteToString(typData.OverStatus)

    End Sub

    Private Sub cmbKindO_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbKindU.SelectedIndexChanged, cmbKindO.SelectedIndexChanged

        If m_typData_O.Hd.Meino Is Nothing Then Exit Sub

        If -1 <> cmbKindO.SelectedIndex Then
            Call SetKenData(cmbKindO.SelectedIndex, 0, dgvKenO)
            Call SetDefectData(cmbKindO.SelectedIndex, 0, dgvDefectO)
        End If

    End Sub

    Private Sub cmbKindU_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbKindU.SelectedIndexChanged, cmbKindO.SelectedIndexChanged

        If m_typData_U.Hd.Meino Is Nothing Then Exit Sub

        If -1 <> cmbKindU.SelectedIndex Then
            Call SetKenData(cmbKindU.SelectedIndex, 1, dgvKenU)
            Call SetDefectData(cmbKindU.SelectedIndex, 1, dgvDefectU)
        End If

    End Sub

    Private Sub SetDefectData(ByVal nIndex As Integer, ByVal nOuDiv As Integer, ByVal dgvData As tCnt.tCnt_DataGridView)

        Dim ii As Integer

        Dim strInfProc As String
        Dim strDefProc As String

        Dim typInf As FF_MAE_TYPE_INF
        Dim typData As FF_MAE_TYPE_DATA

        Dim typDefInf As FF_MAE_DEFECT_INF
        Dim lstDefInf As New List(Of FF_MAE_DEFECT_INF)

        '初期化
        dgvData.RowCount = 0
        lstDefInf.Clear()

        '疵検出工程情報取得
        If nOuDiv = 0 Then  '表
            typInf = m_typData_O.Inf(nIndex)
        Else                '裏
            typInf = m_typData_U.Inf(nIndex)
        End If
        strInfProc = tMod.ByteToString(typInf.Kizukbn)  '疵検出工程

        If strInfProc = "" Or strInfProc = "  " Then
            Exit Sub
        End If

        '疵情報検索
        For ii = 0 To MAX_FF_MAE_DEF_CNT - 1
            '疵情報取得
            If nOuDiv = 0 Then  '表
                typData = m_typData_O.def(ii)
            Else                '裏
                typData = m_typData_U.def(ii)
            End If
            strDefProc = tMod.ByteToString(typData.Kizukbn) '疵検出工程
            If strInfProc = strDefProc Then '工程一致
                ' 20221021 2TCM実機化に伴う下工程改造 小島 --->>>
                If 4 - 1 = nIndex Then
                    If ii >= (MAX_FF_MAE_CC_CNT + MAX_FF_MAE_HOT_CNT + MAX_FF_MAE_PL_CNT + MAX_FF_MAE_TCM_ANA_CNT + MAX_FF_MAE_YOBI_CNT + MAX_FF_MAE_ATU_CNT) Then
                        Continue For
                    End If

                ElseIf 6 - 1 = nIndex Then
                    If ii < (MAX_FF_MAE_CC_CNT + MAX_FF_MAE_HOT_CNT + MAX_FF_MAE_PL_CNT + MAX_FF_MAE_TCM_ANA_CNT + MAX_FF_MAE_YOBI_CNT + MAX_FF_MAE_ATU_CNT) Then
                        Continue For
                    End If
                End If
                ' 20221021 2TCM実機化に伴う下工程改造 小島 <<<---
                typDefInf.Kind = tMod.ByteToString(typData.Kind)            '疵種
                typDefInf.Grade = tMod.ByteToString(typData.Grade)          '疵グレード
                    typDefInf.Kizukbn = tMod.ByteToString(typData.Kizukbn)      '疵検出工程
                    typDefInf.kbn = tMod.ByteToString(typData.kbn)              '疵区分
                    typDefInf.DefectPos = tMod.ByteToString(typData.DefectPos)  '疵長さ方向位置[0.1m]
                    typDefInf.WidPos = tMod.ByteToString(typData.WidPos)        '疵巾方向位置
                    typDefInf.Len = tMod.ByteToString(typData.Len)              '疵長さ[0.1m]

                    Dim sNoHex As String = tMod.ByteToString(typData.No)
                    Dim nNo As Integer
                    Dim sNo As String = sNoHex

                    If Integer.TryParse(sNoHex, Globalization.NumberStyles.HexNumber, Nothing, nNo) = True Then
                        sNo &= String.Format("({0,5})", nNo)
                    End If

                    typDefInf.No = sNo

                    typDefInf.SysKbn = tMod.ByteToString(typData.SysKbn)        'システム区分
                    typDefInf.Filler = tMod.ByteToString(typData.Filler)        '予備
                    lstDefInf.Add(typDefInf)
                End If
        Next

        '疵情報表示
        dgvData.RowCount = lstDefInf.Count
        For ii = 0 To dgvData.RowCount - 1
            dgvData(EM_DEFECT_COL_ID.iNO, ii).Value = CStr(ii + 1)
            dgvData(EM_DEFECT_COL_ID.iTypeID, ii).Value = lstDefInf(ii).Kind
            dgvData(EM_DEFECT_COL_ID.iGradeID, ii).Value = lstDefInf(ii).Grade
            dgvData(EM_DEFECT_COL_ID.iKizuOut, ii).Value = lstDefInf(ii).Kizukbn
            dgvData(EM_DEFECT_COL_ID.iKizuDiv, ii).Value = lstDefInf(ii).kbn
            dgvData(EM_DEFECT_COL_ID.iTotalLen, ii).Value = Format(CSng(IIf(IsNumeric(lstDefInf(ii).DefectPos), lstDefInf(ii).DefectPos.Trim, "0")) / 10, "0.0")
            dgvData(EM_DEFECT_COL_ID.iHaba, ii).Value = lstDefInf(ii).WidPos
            dgvData(EM_DEFECT_COL_ID.iLen, ii).Value = Format(CSng(IIf(IsNumeric(lstDefInf(ii).Len), lstDefInf(ii).Len.Trim, "0")) / 10, "0.0")
            dgvData(EM_DEFECT_COL_ID.iKizuNo, ii).Value = lstDefInf(ii).No
            dgvData(EM_DEFECT_COL_ID.iSysKbn, ii).Value = lstDefInf(ii).SysKbn
            dgvData(EM_DEFECT_COL_ID.iSetNO, ii).Value = lstDefInf(ii).SetNo
            'dgvData(EM_DEFECT_COL_ID.iFiller, ii).Value = lstDefInf(ii).Filler
        Next ii

    End Sub

    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvCoilInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
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
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = False                                   ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            .RowCount = 1

            '' 選択セル色定義
            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = 2           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 35                                       ' 列ヘッダサイズ

            .RowCount = MAX_COIL_ROW
            .Columns(EM_COIL_COL_ID.iTiTle).Width = 140
            .Columns(EM_COIL_COL_ID.iData).Width = 125
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iReiCoilNo).Value = "冷延コイル番号"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iReiCoilG).Value = "冷延コイル重量"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iReiCoilL).Value = "冷延コイル長さ"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iReiCoilAtu).Value = "冷延厚"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iItaatuMin).Value = "板厚公差MIN"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iItaatuMax).Value = "板厚公差MAX"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1i).Value = "向け先1板巾"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iKensa).Value = "検査基準"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMaki).Value = "巻き方向"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iZairyou).Value = "材料巾"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1kMin).Value = "向け先1板巾公差　MIN"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1kMax).Value = "向け先1板巾公差　MAX"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1L).Value = "向け先1長さ(ロッド枚数)"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1N).Value = "向け先1内径"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iItaatu).Value = "板厚"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iIrigawa).Value = "入側内径"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMukesaki1KL).Value = "向け先1剪断長"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iMakoutei).Value = "前工程コード"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iRealMakoutei).Value = "実前工程コード"
            .Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iTCMFlag).Value = "厚手/TCM直送フラグ"
            '.Item(EM_COIL_COL_ID.iTiTle, EM_COIL_ROW_ID.iYobi).Value = "予備"

        End With
    End Sub


    ''' <summary>
    ''' コイル情報セット
    ''' </summary>
    ''' <param name="m_typData">データ</param>
    ''' <param name="dgv">グリッド</param>
    ''' <remarks></remarks>
    Private Sub SetCoilData(ByVal m_typData As FF_MAE_TYPE_HEAD, ByVal dgv As DataGridView)

        ' 0.1m化 対応 --->>>
        Dim Maki(2) As Byte

        Maki(0) = m_typData.Maki(0)
        ' 0.1m化 対応 <<<---

        With dgv
            ' 0.1m化 対応 --->>>
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iReiCoilNo).Value = tMod.ByteToString(m_typData.ReiCoilNo)

            Dim wkReiCoilG As String = tMod.ByteToString(m_typData.ReiCoilG)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iReiCoilG).Value = Format(CSng(IIf(IsNumeric(wkReiCoilG), wkReiCoilG.Trim, "0")) / 1000, "0.000")

            Dim wkReiLen As String = tMod.ByteToString(m_typData.ReiCoilL)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iReiCoilL).Value = Format(CSng(IIf(IsNumeric(wkReiLen), wkReiLen.Trim, "0")) / 10, "0.0")

            Dim wkReiCoilAtu As String = tMod.ByteToString(m_typData.ReiCoilAtu)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iReiCoilAtu).Value = Format(CSng(IIf(IsNumeric(wkReiCoilAtu), wkReiCoilAtu.Trim, "0")) / 1000, "0.000")

            Dim wkItaatuMin As String = tMod.ByteToString(m_typData.ItaatuMin)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iItaatuMin).Value = Format(CSng(IIf(IsNumeric(wkItaatuMin), wkItaatuMin.Trim, "0")) / 1000, "0.000")

            Dim wkItaatuMax As String = tMod.ByteToString(m_typData.ItaatuMax)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iItaatuMax).Value = Format(CSng(IIf(IsNumeric(wkItaatuMax), wkItaatuMax.Trim, "0")) / 1000, "0.000")

            Dim wkMukesaki1i As String = tMod.ByteToString(m_typData.Mukesaki1i)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1i).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1i), wkMukesaki1i.Trim, "0")) / 10, "0.0")

            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iKensa).Value = tMod.ByteToString(m_typData.Kensa)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMaki).Value = tMod.ByteToString(Maki)

            Dim wkZairyou As String = tMod.ByteToString(m_typData.Zairyou)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iZairyou).Value = Format(CSng(IIf(IsNumeric(wkZairyou), wkZairyou.Trim, "0")) / 10, "0.0")

            Dim wkMukesaki1kMin As String = tMod.ByteToString(m_typData.Mukesaki1kMin)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1kMin).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1kMin), wkMukesaki1kMin.Trim, "0")) / 10, "0.0")

            Dim wkMukesaki1kMax As String = tMod.ByteToString(m_typData.Mukesaki1kMax)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1kMax).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1kMax), wkMukesaki1kMax.Trim, "0")) / 10, "0.0")

            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1L).Value = tMod.ByteToString(m_typData.Mukesaki1L)

            Dim wkMukesaki1N As String = tMod.ByteToString(m_typData.Mukesaki1N)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1N).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1N), wkMukesaki1N.Trim, "0")) / 10, "0.0")

            Dim wkItaatu As String = tMod.ByteToString(m_typData.Itaatu)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iItaatu).Value = Format(CSng(IIf(IsNumeric(wkItaatu), wkItaatu.Trim, "0")) / 1000, "0.000")

            Dim wkIrigawa As String = tMod.ByteToString(m_typData.Irigawa)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iIrigawa).Value = Format(CSng(IIf(IsNumeric(wkIrigawa), wkIrigawa.Trim, "0")) / 10, "0.0")

            Dim wkMukesaki1KL As String = tMod.ByteToString(m_typData.Mukesaki1KL)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMukesaki1KL).Value = Format(CSng(IIf(IsNumeric(wkMukesaki1KL), wkMukesaki1KL.Trim, "0")) / 10, "0.0")

            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iMakoutei).Value = tMod.ByteToString(m_typData.Makoutei)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iRealMakoutei).Value = tMod.ByteToString(m_typData.RealMakoutei)
            .Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iTCMFlag).Value = tMod.ByteToString(m_typData.TCMFlag)
            '.Item(EM_COIL_COL_ID.iData, EM_COIL_ROW_ID.iYobi).Value = tMod.ByteToString(m_typData.Yobi)
            ' 0.1m化 対応 <<<---

        End With


    End Sub

    ' 0.1m化 対応 --->>>
    ''' <summary>
    ''' コイル情報セット
    ''' </summary>
    ''' <param name="m_typData">データ</param>
    ''' <param name="dgv">グリッド</param>
    ''' <remarks></remarks>
    Private Sub SetRclData(ByVal m_typData As FF_MAE_TYPE_VRCLINF, ByVal m_typData2 As FF_MAE_TYPE_FROMTO, ByVal dgv As DataGridView)

        With dgv
            ' 0.1m化 対応 --->>>
            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iKizukenNo).Value = tMod.ByteToString(m_typData.KizukenNo)
            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iCoilNo).Value = tMod.ByteToString(m_typData2.CoilNo)

            Dim wkFromSai As String = tMod.ByteToString(m_typData2.From)
            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iFromSai).Value = Format(CSng(IIf(IsNumeric(wkFromSai), wkFromSai.Trim, "0")) / 10, "0.0")

            Dim wkToSai As String = tMod.ByteToString(m_typData2.To2)
            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iToSai).Value = Format(CSng(IIf(IsNumeric(wkToSai), wkToSai.Trim, "0")) / 10, "0.0")

            .Item(EM_COIL_COL_ID.iData, EM_RCL_ROW_ID.iKoban).Value = tMod.ByteToString(m_typData2.Koban)
            ' 0.1m化 対応 <<<---
        End With
    End Sub
    ' 0.1m化 対応 <<<---

    ' 0.1m化 対応 --->>>
    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvRclInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
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
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = False                                   ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            .RowCount = 1

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = 2           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 35                                       ' 列ヘッダサイズ

            .RowCount = MAX_RCL_ROW
            .Columns(EM_COIL_COL_ID.iTiTle).Width = 240
            .Columns(EM_COIL_COL_ID.iData).Width = 150
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iKizukenNo).Value = "仮想RCL疵検管理No"
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iCoilNo).Value = "冷延コイルNo"
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iFromSai).Value = "採取開始位置長さ（From） [0.1m]"
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iToSai).Value = "採取終了位置長さ（To）	[0.1m]"
            .Item(EM_COIL_COL_ID.iTiTle, EM_RCL_COL_ID.iKoban).Value = "採取子番"
        End With
    End Sub
    ' 0.1m化 対応 <<<---

    Protected Friend Overridable Sub dgvTuhanInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
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
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            .RowCount = 1

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = MAX_TUHAN_COL           'textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' 列ヘッダサイズ

            '列0 (アドレス) 実績工程コード
            .Columns(EM_TUHAN_COL_ID.iJissekiCode).HeaderText = "実績工程コード"
            .Columns(EM_TUHAN_COL_ID.iJissekiCode).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iJissekiCode).Width = 130
            .Columns(EM_TUHAN_COL_ID.iJissekiCode).SortMode = DataGridViewColumnSortMode.NotSortable

            '列1 (アドレス) 命令番号
            .Columns(EM_TUHAN_COL_ID.iMeireiNo).HeaderText = "命令番号"
            .Columns(EM_TUHAN_COL_ID.iMeireiNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iMeireiNo).Width = 100
            .Columns(EM_TUHAN_COL_ID.iMeireiNo).SortMode = DataGridViewColumnSortMode.NotSortable


            '列2 (アドレス) 通板回数
            .Columns(EM_TUHAN_COL_ID.iThuhanCnt).HeaderText = "通板回数"
            .Columns(EM_TUHAN_COL_ID.iThuhanCnt).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iThuhanCnt).Width = 100
            .Columns(EM_TUHAN_COL_ID.iThuhanCnt).SortMode = DataGridViewColumnSortMode.NotSortable

            '列3 (アドレス) 入側現品管理NO
            .Columns(EM_TUHAN_COL_ID.iGenpinNo).HeaderText = "入側現品管理NO"
            .Columns(EM_TUHAN_COL_ID.iGenpinNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iGenpinNo).Width = 130
            .Columns(EM_TUHAN_COL_ID.iGenpinNo).SortMode = DataGridViewColumnSortMode.NotSortable


            '列4(アドレス) 入側コイル長さ
            .Columns(EM_TUHAN_COL_ID.iCoilLen).HeaderText = "入側コイル長さ"
            .Columns(EM_TUHAN_COL_ID.iCoilLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iCoilLen).Width = 130
            .Columns(EM_TUHAN_COL_ID.iCoilLen).SortMode = DataGridViewColumnSortMode.NotSortable


            ' 0.1m化 対応 --->>>
            '列4(アドレス) 入側コイル長さ
            .Columns(EM_TUHAN_COL_ID.iFiller).HeaderText = "予備"
            .Columns(EM_TUHAN_COL_ID.iFiller).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_TUHAN_COL_ID.iFiller).Width = 130
            .Columns(EM_TUHAN_COL_ID.iFiller).SortMode = DataGridViewColumnSortMode.NotSortable
            ' 0.1m化 対応 <<<---

        End With
    End Sub

    ''' <summary>
    ''' グリッドの初期化
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvBunkatuInitialize(ByVal dgv As DataGridView)
        Dim ii As Integer = 0
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
            .ScrollBars = ScrollBars.Both


            .SelectionMode = DataGridViewSelectionMode.CellSelect           ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                    ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing

            .RowCount = MAX_FF_MAE_BNKT_CNT

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = Color.LightSkyBlue     ' 選択
            '.DefaultCellStyle().SelectionBackColor = Color.Transparent		' 選択色 無効化 (dgvLog_SelectionChangedが必須)
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = MAX_BUNKATU_COL          ' textbox のみを追加する場合はこれの方が楽
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .ColumnHeadersHeight = 20                                       ' 列ヘッダサイズ

            '列0 (アドレス) NO
            .Columns(EM_BUNKATU_COL_ID.iNo).HeaderText = "NO"
            .Columns(EM_BUNKATU_COL_ID.iNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iNo).Width = 60
            .Columns(EM_BUNKATU_COL_ID.iNo).SortMode = DataGridViewColumnSortMode.NotSortable


            '列1 (アドレス) 実績巻方向
            .Columns(EM_BUNKATU_COL_ID.iMakiHoukou).HeaderText = "実績巻方向"
            .Columns(EM_BUNKATU_COL_ID.iMakiHoukou).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iMakiHoukou).Width = 110
            .Columns(EM_BUNKATU_COL_ID.iMakiHoukou).SortMode = DataGridViewColumnSortMode.NotSortable

            '列2 (アドレス) コイル長
            .Columns(EM_BUNKATU_COL_ID.iCoilLen).HeaderText = "コイル長"
            .Columns(EM_BUNKATU_COL_ID.iCoilLen).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iCoilLen).Width = 90
            .Columns(EM_BUNKATU_COL_ID.iCoilLen).SortMode = DataGridViewColumnSortMode.NotSortable


            '列3 (アドレス) カット場所区分
            .Columns(EM_BUNKATU_COL_ID.iCutkbn).HeaderText = "カット場所区分"
            .Columns(EM_BUNKATU_COL_ID.iCutkbn).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iCutkbn).Width = 120
            .Columns(EM_BUNKATU_COL_ID.iCutkbn).SortMode = DataGridViewColumnSortMode.NotSortable

            '列4 (アドレス) 分割現品管理NO
            .Columns(EM_BUNKATU_COL_ID.iGenpinNo).HeaderText = "分割現品管理NO"
            .Columns(EM_BUNKATU_COL_ID.iGenpinNo).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iGenpinNo).Width = 130
            .Columns(EM_BUNKATU_COL_ID.iGenpinNo).SortMode = DataGridViewColumnSortMode.NotSortable

            ' 0.1m化 対応 --->>>
            '列5 予備
            .Columns(EM_BUNKATU_COL_ID.iFiller).HeaderText = "予備"
            .Columns(EM_BUNKATU_COL_ID.iFiller).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_BUNKATU_COL_ID.iFiller).Width = 83
            .Columns(EM_BUNKATU_COL_ID.iFiller).SortMode = DataGridViewColumnSortMode.NotSortable
            ' 0.1m化 対応 <<<---
        End With
    End Sub

    Private Sub SetJisData(ByVal nIndex As Integer, ByVal dgvData As tCnt.tCnt_DataGridView,
                            ByVal typFFData As FF_MAE_JISSEKI, ByVal dgvBukatu As tCnt.tCnt_DataGridView)

        Dim typData As FF_MAE_TYPE_THUHAN
        Dim ii As Integer

        typData = typFFData.Th(nIndex)

        dgvData(EM_TUHAN_COL_ID.iJissekiCode, 0).Value = tMod.ByteToString(typData.JissekiCode)
        dgvData(EM_TUHAN_COL_ID.iMeireiNo, 0).Value = tMod.ByteToString(typData.MeireiNo)
        dgvData(EM_TUHAN_COL_ID.iThuhanCnt, 0).Value = tMod.ByteToString(typData.ThuhanCnt)
        dgvData(EM_TUHAN_COL_ID.iGenpinNo, 0).Value = tMod.ByteToString(typData.GenpinNo)

        Dim wkCoilLen As String = tMod.ByteToString(typData.CoilLen)
        dgvData(EM_TUHAN_COL_ID.iCoilLen, 0).Value = Format(CSng(IIf(IsNumeric(wkCoilLen), wkCoilLen.Trim, "0")) / 10, "0.0")

        ' 0.1m化 対応 --->>>
        dgvData(EM_TUHAN_COL_ID.iFiller, 0).Value = tMod.ByteToString(typData.Filler)
        ' 0.1m化 対応 <<<---

        For ii = 0 To MAX_FF_MAE_BNKT_CNT - 1

            dgvBukatu(EM_BUNKATU_COL_ID.iNo, ii).Value = CStr(ii + 1)
            dgvBukatu(EM_BUNKATU_COL_ID.iMakiHoukou, ii).Value = tMod.ByteToString(typData.Bu(ii).MakiHoukou)

            Dim wkCoilTmp As String = tMod.ByteToString(typData.Bu(ii).CoilLen)
            dgvBukatu(EM_BUNKATU_COL_ID.iCoilLen, ii).Value = Format(CSng(IIf(IsNumeric(wkCoilTmp), wkCoilTmp.Trim, "0")) / 10, "0.0")

            dgvBukatu(EM_BUNKATU_COL_ID.iCutkbn, ii).Value = tMod.ByteToString(typData.Bu(ii).Cutkbn)
            dgvBukatu(EM_BUNKATU_COL_ID.iGenpinNo, ii).Value = tMod.ByteToString(typData.Bu(ii).GenpinNo)
            ' 0.1m化 対応 --->>>
            dgvBukatu(EM_BUNKATU_COL_ID.iFiller, ii).Value = tMod.ByteToString(typData.Bu(ii).Filler)
            ' 0.1m化 対応 <<<---

        Next ii

    End Sub

    Private Sub cmbTuhanO_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbTuhanU.SelectedIndexChanged, cmbTuhanO.SelectedIndexChanged

        If m_typData_O.Hd.Meino Is Nothing Then Exit Sub

        If -1 <> cmbTuhanO.SelectedIndex Then
            Call SetJisData(cmbTuhanO.SelectedIndex, dgvJisO, m_typData_O, dgvBunKatuO)
        End If

    End Sub

    Private Sub cmbTuhanU_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles cmbTuhanU.SelectedIndexChanged, cmbTuhanO.SelectedIndexChanged

        If m_typData_U.Hd.Meino Is Nothing Then Exit Sub

        If -1 <> cmbTuhanU.SelectedIndex Then
            Call SetJisData(cmbTuhanU.SelectedIndex, dgvJisU, m_typData_U, dgvBunKatuU)
        End If

    End Sub

End Class