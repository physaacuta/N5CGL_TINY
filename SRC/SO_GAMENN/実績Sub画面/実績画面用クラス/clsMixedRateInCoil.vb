'*********************************************************************************
'　入側コイル単位　疵混入率管理クラス
'	[Ver]
'		Ver.01    2018/12/12  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict Off
Imports tClassLibrary
Imports System.Runtime.InteropServices

Public Class clsMixedRateInCoil
    Implements IDisposable                          ' デストラクタ定義

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Const DgvMaxCol_MRare As Integer = 2           'データ列MAX

    'コイル情報
    Public Structure RATE_CONTROL_DATA
        Dim dgvMixedRateIn As DataGridView       '入側コイル単位 混入率データグリッド
        Dim lblNgMsg As Label                    '異常ガイダンス表示用
        Dim bCorrect As Boolean                  '補正率使用(True:使用、False:不使用)
    End Structure

    '入側コイル単位　疵情報構造体
    Private Structure IN_COIL_KIZU_DATA
        Dim nLen As Integer                     '検査実測長
        Dim dCorrect As Double                  '補正率
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nKizuCnt() As Integer               '疵個数(0:表、1:裏)
        Public Sub initialize()
            nLen = 0
            ReDim nKizuCnt(NUM_MEN - 1)
        End Sub
    End Structure

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager    ' ログ管理クラス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private disposedValue As Boolean = False                ' 重複する呼び出しを検出するには
    Private m_typRate As RATE_CONTROL_DATA                  ' 混入率クラスコントロール

    ' グリッド　列設定
    Private Enum EM_MRATE_COL_ID
        iTitle = 0              '項目見出し
        iRate                   '混入率
    End Enum

    ' 入側コイル単位 混入率グリッド 行設定
    Private Enum EM_MRATE_ROW_ID_IN
        iOmote = 0          '表
        iUra                '裏
    End Enum


    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="typRatedata">コイル情報</param>
    ''' <param name="clsLog">ログクラス</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef typRatedata As RATE_CONTROL_DATA, ByRef clsLog As tClass.tClass_LogManager)

        '探傷画面
        m_typRate = typRatedata

        'ログクラス
        mcls_Log = clsLog

    End Sub

    ' IDisposable
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        If Not Me.disposedValue Then
            If disposing Then
                ' TODO: 明示的に呼び出されたときにアンマネージ リソースを解放します
            End If

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
    ''' 混入率初期化処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()

        'データグリッド初期設定
        Call dgvInitialize(m_typRate.dgvMixedRateIn)

        '疵混入率初期表示
        Call dgvClear(m_typRate.dgvMixedRateIn)

    End Sub

    ''' <summary>
    ''' 疵混入率表示（入側コイル単位）グリッドの初期化
    ''' </summary>
    ''' <remarks></remarks>
    Protected Friend Overridable Sub dgvInitialize(ByVal dgv As DataGridView)

        Dim ii As Integer = 0
        With dgv
            .RowCount = NUM_MEN

            '' 動作定義
            .AutoGenerateColumns = False                                    ' 列自動生成 (falseにするとDataSourceが効かなくなるので注意)
            .AllowUserToAddRows = False                                     ' ユーザー 行追加
            .AllowUserToDeleteRows = False                                  ' ユーザー 行削除
            .AllowUserToOrderColumns = False                                ' ユーザー 列移動
            .AllowUserToResizeColumns = False                               ' ユーザー 列サイズ変更
            .AllowUserToResizeRows = False                                  ' ユーザー 行サイズ変更
            .ShowCellErrors = False
            .ShowCellToolTips = False
            .ShowEditingIcon = False
            .ShowRowErrors = False
            .ScrollBars = ScrollBars.None

            .SelectionMode = DataGridViewSelectionMode.CellSelect        ' セル選択
            .MultiSelect = False                                            ' 複数行選択
            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = False                                   ' 列ヘッダ表示
            .RowHeadersVisible = False                                      ' 行ヘッダ表示
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None     ' 可視列サイズ自動調整
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None           ' 可視行サイズ自動調整
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue      ' 1行おきに色替え
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black

            '' 列定義
            .ColumnCount = DgvMaxCol_MRare           'textbox のみを追加する場合はこれの方が楽

            '列１ (アドレス) 項目見出し
            '.Columns(EM_MRATE_COL_ID.iTitle).HeaderText = ""
            .Columns(EM_MRATE_COL_ID.iTitle).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
            .Columns(EM_MRATE_COL_ID.iTitle).Width = 60
            .Columns(EM_MRATE_COL_ID.iTitle).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iTitle).Frozen = True                                       ' 水平移動禁止

            '列２ (アドレス) グレード１
            '.Columns(EM_MRATE_COL_ID.iRate).HeaderText = ""
            .Columns(EM_MRATE_COL_ID.iRate).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(EM_MRATE_COL_ID.iRate).Width = 116
            .Columns(EM_MRATE_COL_ID.iRate).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(EM_MRATE_COL_ID.iRate).Frozen = True
        End With

        '項目見出し設定
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID_IN.iOmote).Value = "表"
        dgv(EM_MRATE_COL_ID.iTitle, EM_MRATE_ROW_ID_IN.iUra).Value = "裏"

        '各行セル高さ設定
        For ii = EM_MRATE_ROW_ID_IN.iOmote To EM_MRATE_ROW_ID_IN.iUra
            dgv.Rows(ii).Height = 15
        Next ii

    End Sub

    ''' <summary>
    ''' グリッドのクリア処理
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvClear(ByVal dgv As DataGridView)
        Dim ii As Integer = 0

        '桁数確認用
        For ii = 0 To dgv.RowCount - 1
            dgv(EM_MRATE_COL_ID.iRate, ii).Value = ""
        Next ii

    End Sub

    ''' <summary>
    ''' データ表示
    ''' </summary>
    ''' <param name="strKizuken">管理No</param>
    ''' <param name="blnSyslog">syslog出力区分(True:出力, False:出力なし)</param>
    ''' <remarks></remarks>
    Public Function DspData(ByVal strKizuken As String, Optional ByVal blnSyslog As Boolean = False) As Boolean

        Dim inCoil As IN_COIL_KIZU_DATA = Nothing   '入側コイル単位　疵情報

        'データ取得
        inCoil.initialize()
        If Not GetData(strKizuken, inCoil, blnSyslog) Then
            Call DspInit()
            Return False
        End If

        'データセット
        SetData(inCoil)

        'データグリッドをリフレッシュイベントで表示
        m_typRate.dgvMixedRateIn.Refresh()

        Return True

    End Function

    ''' <summary>
    ''' 入側コイル単位　疵混入率取得
    ''' </summary>
    ''' <param name="strKizuken">管理No</param>
    ''' <param name="inCoil">疵情報</param>
    ''' <param name="blnSyslog">syslog出力区分(True:出力, False:出力なし)</param>
    ''' <remarks></remarks>
    Private Function GetData(ByVal strKizuken As String, ByRef inCoil As IN_COIL_KIZU_DATA, ByVal blnSyslog As Boolean) As Boolean
        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSQL As String                                    'SQL文
        Dim bRet As Boolean = False

        '-------------------------------------------------------------------
        ' SELECT句
        '-------------------------------------------------------------------
        strSQL = "SELECT"
        strSQL &= "  ISNULL(RES.検査実測長,0) as 検査実測長"
        strSQL &= " ,ISNULL(RES.補正率,1) as 補正率"
        strSQL &= " ,ISNULL(CNT_T.個数, 0) as 表示疵個数表"
        strSQL &= " ,ISNULL(CNT_B.個数, 0) as 表示疵個数裏"

        '-------------------------------------------------------------------
        ' FROM句 出側物理コイル実績テーブル
        '-------------------------------------------------------------------
        strSQL &= " FROM " & DB_COIL_RESULT & " RES "

        '-------------------------------------------------------------------
        ' 外部結合  表 表示疵個数
        '-------------------------------------------------------------------
        strSQL &= " LEFT OUTER JOIN (SELECT count(*) as 個数, d.管理No from "
        strSQL &= DB_DEFECT_INF_T & " d "
        strSQL &= " WHERE d.重複結果 <> 1 "
        strSQL &= " Group by d.管理No ) CNT_T "
        strSQL &= " ON RES.管理No = CNT_T.管理No "

        '-------------------------------------------------------------------
        ' 外部結合  裏 表示疵個数
        '-------------------------------------------------------------------
        strSQL &= " LEFT OUTER JOIN (SELECT count(*) as 個数, d.管理No from "
        strSQL &= DB_DEFECT_INF_B & " d "
        strSQL &= " WHERE d.重複結果 <> 1 "
        strSQL &= " Group by d.管理No ) CNT_B "
        strSQL &= " ON RES.管理No = CNT_B.管理No "

        '-------------------------------------------------------------------
        ' 条件句
        '-------------------------------------------------------------------
        strSQL &= " WHERE 1 = 1"
        strSQL = strSQL & " and RES.管理No = '" & strKizuken & "' "

        'DBオープン
        If Not tcls_DB.Open() Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB接続エラー [" & DB_COIL_RESULT & "]")
            '異常ガイダンス表示
            m_typRate.lblNgMsg.Text = "DB接続エラー [" & DB_COIL_RESULT & "]"
            'syslog出力
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_CONNECT_ERR, String.Format("[{0}]", "[" & DB_COIL_RESULT & "]"))
            End If
            Return False
        End If

        ' Select文実行
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB読み込みエラー [" & DB_COIL_RESULT & "]")
            '異常ガイダンス表示
            m_typRate.lblNgMsg.Text = "DB読み込みエラー [" & DB_COIL_RESULT & "]"
            'syslog出力
            If blnSyslog Then
                KizuLibCLI.KizuMod.SysLog(SYSNO_DB_EXECUTEDIRECT_ERR, String.Format("[{0}]", "[" & DB_COIL_RESULT & "]"))
            End If
            Return False
        End If

        Try

            ' データ取得
            sqlRead.Read()
            If sqlRead.HasRows Then

                inCoil.nLen = CInt(sqlRead.GetSqlInt32(sqlRead.GetOrdinal("検査実測長")))
                inCoil.dCorrect = CDbl(sqlRead.GetDouble(sqlRead.GetOrdinal("補正率")))
                inCoil.nKizuCnt(EM_MRATE_ROW_ID_IN.iOmote) = CInt(sqlRead.GetSqlInt32(sqlRead.GetOrdinal("表示疵個数表")))
                inCoil.nKizuCnt(EM_MRATE_ROW_ID_IN.iUra) = CInt(sqlRead.GetSqlInt32(sqlRead.GetOrdinal("表示疵個数裏")))

                '正常終了
                bRet = True
            Else
                mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "入側コイル単位　疵混入率取得エラー [管理No = " & strKizuken & "]")
                '異常ガイダンス表示
                m_typRate.lblNgMsg.Text = "入側コイル単位　疵混入率情報取得に失敗しました。"
                ''syslog出力
                'If blnSyslog Then
                '    KizuLibCLI.KizuMod.SysLog(SYSNO_ZISEKI_NG, String.Format("[{0}]", "[管理No = " & strKizuken & "]"))
                'End If
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "入側コイル単位　疵混入率取得エラー [" & ex.Message & "]")
            '異常ガイダンス表示
            m_typRate.lblNgMsg.Text = "疵混入率情報取得に失敗しました。"
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
    ''' データセット
    ''' </summary>
    '''<param name="inCoil">入側コイル単位　疵情報データ</param>
    ''' <remarks></remarks>
    Private Sub SetData(ByVal inCoil As IN_COIL_KIZU_DATA)

        Dim ii As Integer   'ループカウンタ
        Dim nLen As Integer

        If m_typRate.bCorrect Then
            '補正率使用
            nLen = CInt(inCoil.nLen * inCoil.dCorrect)
        Else
            nLen = inCoil.nLen
        End If

        '疵混入率設定
        For ii = 0 To m_typRate.dgvMixedRateIn.RowCount - 1
            m_typRate.dgvMixedRateIn(EM_MRATE_COL_ID.iRate, ii).Value = Format(inCoil.nKizuCnt(ii) / CSng(nLen / 1000) * 100, "#0.00")
        Next ii

    End Sub

    ''' <summary>
    ''' 疵混入率サイズ調整
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ResizeMixedRate()
        Dim nColumnHeaderHeight As Integer = 0
        Dim nColumnHeight As Integer = 0
        Dim nColumnHeaderFontSize As Integer = 0
        Dim nColumnFontSize As Integer = 0

        ' コントロールサイズから行の高さを算出する。
        nColumnHeight = CInt(Math.Floor(m_typRate.dgvMixedRateIn.Height / NUM_MEN))

        ' 行の高さからフォントサイズを調整する。
        nColumnFontSize = CInt(GetGridFontScaleSize(m_typRate.dgvMixedRateIn, "99.99", 20, 110, nColumnHeight))

        ' 調整
        With m_typRate.dgvMixedRateIn

            For ii As Integer = 0 To .RowCount - 1
                .Rows(ii).Height = nColumnHeight
            Next ii
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", nColumnFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
        End With
    End Sub

End Class
