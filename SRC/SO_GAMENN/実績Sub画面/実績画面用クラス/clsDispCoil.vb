'*******************************************************************************
'   コイル情報表示
'
'   [Ver]
'       Ver.01  2017/08/24  初版
'
'   [メモ]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI.KizuMod

Imports KizuForm
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

''' <summary>
''' コイル情報表示
''' </summary>
''' <remarks></remarks>
Public Class clsDispCoil
    Implements IDisposable

    '---------------------------------------------------------------------------
    ' 列挙体
    '---------------------------------------------------------------------------
    ''' <summary>
    ''' コイル情報1
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_COL_COLINF_1
        ORDER_NO = 0        ' 命令No
        COIL_NO             ' 生産番号
        TYPE_DIV            ' 品種
        MATERIAL            ' 材質
        NEXT_STEP           ' 次工程
        KENSA_REF           ' 検査基準
        SIZE                ' 材料厚幅
        ORDER               ' 注文
        MAX_COL             ' 最大列数
    End Enum

    ''' <summary>
    ''' コイル情報2
    ''' </summary>
    ''' <remarks></remarks>
    Private Enum EM_COL_COLINF_2
        MEN = 0                               '表示面
        P_CODE
        S_CODE
        COIL_LEN            ' コイル長
        MAX_COL             ' 最大列数
    End Enum

    ''' <summary>
    ''' コイル情報表示コントロール
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_COIL_DSP
        Dim dgvInf1 As tCnt.tCnt_DataGridView
        Dim dgvInf2 As tCnt.tCnt_DataGridView
        Dim lblKiki As Label
        Dim lblKensa As Label
        Dim tblLayoutPnl As TableLayoutPanel
    End Structure

    '///////////////////////////////////////////////////////////////////////////
    'メンバ変数
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                'リソース開放フラグ[True:開放済,False:未開放]
    Private mcls_Log As tClass_LogManager                   'ログ管理
    Private m_typCoilDsp As TYP_COIL_DSP                    'コイル情報表示コントロール
    Private mcls_Param As clsParamManager                   'パラメータクラス

    '---------------------------------------------------------------------------
    ' プロパティ
    '---------------------------------------------------------------------------

    '///////////////////////////////////////////////////////////////////////////
    'メンバ関数
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="typCoilDsp"></param>
    ''' <param name="clsLog"></param>
    ''' <remarks></remarks>
    Public Sub New(ByRef typCoilDsp As TYP_COIL_DSP, ByRef clsLog As tClass_LogManager, ByRef clsParam As clsParamManager)
        Try
            m_typCoilDsp = typCoilDsp
            mcls_Log = clsLog
            mcls_Param = clsParam

            dgvInit_Inf1()
            dgvInit_Inf2()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報表示生成異常[{0}]", ex.Message))
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
    ''' 画面初期表示
    ''' </summary>
    Public Sub Load()

        Dim dgv As tCnt.tCnt_DataGridView = m_typCoilDsp.dgvInf1
        dgvWidthResize(dgv)
        AddHandler m_typCoilDsp.tblLayoutPnl.Resize, AddressOf Ctrl_ReSize

    End Sub

    ''' <summary>
    ''' 表示初期化
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()
        Try
            'コイル情報表示コントロール
            With m_typCoilDsp

                For ii As Integer = 0 To .dgvInf1.ColumnCount - 1
                    .dgvInf1.Item(ii, 0).Value = ""
                    .dgvInf1.Item(ii, 0).Style.ForeColor = .dgvInf1.DefaultCellStyle().ForeColor
                    .dgvInf1.Item(ii, 0).Style.BackColor = .dgvInf1.DefaultCellStyle().BackColor
                Next

                For ii As Integer = 0 To .dgvInf2.ColumnCount - 1
                    .dgvInf2.Item(ii, 0).Value = ""
                    .dgvInf2.Item(ii, 0).Style.ForeColor = .dgvInf2.DefaultCellStyle().ForeColor
                    .dgvInf2.Item(ii, 0).Style.BackColor = .dgvInf2.DefaultCellStyle().BackColor
                Next

                .lblKiki.Text = ""
                .lblKiki.BackColor = g_ColorControl
                .lblKensa.Text = ""
                .lblKensa.BackColor = g_ColorControl

            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("表示初期化異常 [{0}]", ex.Message))
        End Try
    End Sub


    ''' <summary>
    ''' コイル情報表示
    ''' </summary>
    ''' <param name="nMen">面[0:表,1:裏]</param>
    ''' <remarks></remarks>
    Public Sub DspCoil(ByVal typCoilInf As clsDataMap.TYP_COIL_INF, ByVal nMen As Integer)

        Try
            With m_typCoilDsp

                For ii As Integer = 0 To .dgvInf1.ColumnCount - 1
                    Select Case ii
                        Case EM_COL_COLINF_1.ORDER_NO           ' 命令No
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strOrderNo
                            If KizuForm.EM_DIV_KENSA.DIV_KENSA_OK = typCoilInf.nKensa(nMen) Then
                                .dgvInf1.Item(ii, 0).Style.BackColor = .dgvInf1.DefaultCellStyle().BackColor
                                .dgvInf1.Item(ii, 0).Style.SelectionBackColor = .dgvInf1.Item(ii, 0).Style.BackColor
                            Else
                                '検査状態が正常以外は、背景色を赤にする
                                .dgvInf1.Item(ii, 0).Style.BackColor = Color.Red
                                .dgvInf1.Item(ii, 0).Style.SelectionBackColor = .dgvInf1.Item(ii, 0).Style.BackColor
                            End If

                        Case EM_COL_COLINF_1.COIL_NO            ' 生産番号
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strCoilNo
                        Case EM_COL_COLINF_1.TYPE_DIV           ' 品種
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strHinCode
                        Case EM_COL_COLINF_1.MATERIAL           ' 材質
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strSyuZai
                        Case EM_COL_COLINF_1.NEXT_STEP          ' 次工程
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strNextCode
                        Case EM_COL_COLINF_1.KENSA_REF          ' 検査基準
                            .dgvInf1.Item(ii, 0).Value = typCoilInf.strKenKijun(0) '検査基準_表
                        Case EM_COL_COLINF_1.SIZE
                            ''サイズ(板厚[μm⇒mm]×板幅[mm])
                            Dim sThick As String = Format(typCoilInf.nCoilAtu / 1000, "0.000")
                            Dim sWidth As String = typCoilInf.nCoilWidth.ToString
                            .dgvInf1.Item(ii, 0).Value = String.Format("{0}×{1,4}", sThick, sWidth)
                        Case EM_COL_COLINF_1.ORDER
                            ''サイズ(板厚[μm⇒mm]×板幅[mm])
                            Dim sThickSei As String = Format(typCoilInf.nCoilAtuSei / 1000, "0.000")
                            Dim sWidthSei As String = typCoilInf.nCoilWidSei.ToString
                            .dgvInf1.Item(ii, 0).Value = String.Format("{0}×{1,4}", sThickSei, sWidthSei)
                        Case Else
                            .dgvInf1.Item(ii, 0).Value = ""
                            .dgvInf1.Item(ii, 0).Style.BackColor = .dgvInf1.DefaultCellStyle().BackColor
                    End Select
                Next

                For ii As Integer = 0 To .dgvInf2.ColumnCount - 1
                    Select Case ii
                        Case EM_COL_COLINF_2.MEN             ' パターン
                            '.dgvInf2.Item(ii, 0).Value = IIf(1 = nMen, "裏面", "表面")
                            If 1 = nMen Then
                                .dgvInf2.Item(ii, 0).Value = "裏面"
                                .dgvInf2.Item(ii, 0).Style.ForeColor = Color.Black
                                .dgvInf2.Item(ii, 0).Style.BackColor = Color.Orange
                                .dgvInf2.Item(ii, 0).Style.SelectionForeColor = .dgvInf2.Item(ii, 0).Style.ForeColor
                                .dgvInf2.Item(ii, 0).Style.SelectionBackColor = .dgvInf2.Item(ii, 0).Style.BackColor
                            Else
                                .dgvInf2.Item(ii, 0).Value = "表面"
                                .dgvInf2.Item(ii, 0).Style.ForeColor = Color.Yellow
                                .dgvInf2.Item(ii, 0).Style.BackColor = Color.Blue
                                .dgvInf2.Item(ii, 0).Style.SelectionForeColor = .dgvInf2.Item(ii, 0).Style.ForeColor
                                .dgvInf2.Item(ii, 0).Style.SelectionBackColor = .dgvInf2.Item(ii, 0).Style.BackColor
                            End If
                        Case EM_COL_COLINF_2.P_CODE             ' パターン
                            .dgvInf2.Item(ii, 0).Value = typCoilInf.nPcode(nMen).ToString
                        Case EM_COL_COLINF_2.S_CODE             ' 表面状態
                            .dgvInf2.Item(ii, 0).Value = typCoilInf.nScode(nMen).ToString
                        Case EM_COL_COLINF_2.COIL_LEN           ' コイル長
                            .dgvInf2.Item(ii, 0).Value = Format(typCoilInf.nCoilLen / 1000, "0")
                        Case Else
                            .dgvInf2.Item(ii, 0).Value = ""
                            .dgvInf2.Item(ii, 0).Style.BackColor = .dgvInf1.DefaultCellStyle().BackColor
                    End Select
                Next

                ' 機器状態
                KizuForm.ucStatusAria.DspKiki(.lblKiki, CType(typCoilInf.nKiki(nMen), KizuForm.EM_DIV_KIKI))
                If .lblKiki.Text = "軽故障" Or .lblKiki.Text = "重故障" Then .lblKiki.ForeColor = Color.Yellow

                ' 検査状態
                KizuForm.ucStatusAria.DspKensa(.lblKensa, CType(typCoilInf.nKensa(nMen), KizuForm.EM_DIV_KENSA))
                If .lblKensa.Text <> "停止" And .lblKensa.Text <> "不明" Then .lblKensa.ForeColor = Color.Yellow

            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報表示異常 [{0}]", ex.Message))
        End Try
    End Sub

    ''///////////////////////////////////////////////////////////////////////////
    ''データグリッドビュー
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' グリッド1初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub dgvInit_Inf1()

        Dim dgv As tCnt.tCnt_DataGridView = m_typCoilDsp.dgvInf1

        dgv.dgvInitialize(dgv)
        dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        Try
            With dgv
                .ScrollBars = ScrollBars.Horizontal                             ' スクロールバー(水平のみ)
                .DoubleBuffered = True

                .ReadOnly = True                                                ' 読み取り専用
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え
                .ColumnHeadersVisible = True                                   ' 列ヘッダ表示あり
                .AllowUserToResizeColumns = True                               ' ユーザー 列サイズ変更

                '' 選択セル色定義
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .ColumnHeadersHeight = 30                                       ' 列ヘッダ高さ調整

                For col As EM_COL_COLINF_1 = EM_COL_COLINF_1.ORDER_NO To EM_COL_COLINF_1.MAX_COL
                    Select Case col
                        Case EM_COL_COLINF_1.ORDER_NO
                            ' 命令No
                            dgvColSetText(dgv, "命令No", 78, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.COIL_NO
                            ' 生産番号
                            dgvColSetText(dgv, "生産番号", 198, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.TYPE_DIV
                            ' 品種
                            dgvColSetText(dgv, "品種", 46, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.MATERIAL
                            ' 材質
                            dgvColSetText(dgv, "材質", 46, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.NEXT_STEP
                            ' 次工程
                            dgvColSetText(dgv, "次" & vbCrLf & "工程", 46, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.KENSA_REF
                            ' 検査基準
                            dgvColSetText(dgv, "検査基準", 78, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.SIZE
                            ' 原板
                            dgvColSetText(dgv, "原板", 182, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                        Case EM_COL_COLINF_1.ORDER
                            ' 注文
                            dgvColSetText(dgv, "注文", 182, False, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    End Select
                Next

                .RowCount = 1
                .Rows(0).Height = .ClientSize.Height - .ColumnHeadersHeight


            End With
        Catch ex As Exception
        End Try

    End Sub

    ''' <summary>
    ''' グリッド2初期化
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub dgvInit_Inf2()

        Dim dgv As tCnt.tCnt_DataGridView = m_typCoilDsp.dgvInf2

        Call dgv.dgvInitialize(dgv)

        Try
            With dgv
                .ScrollBars = ScrollBars.None                                   ' スクロールバーなし
                .DoubleBuffered = True

                .ReadOnly = True                                                ' 読み取り専用
                .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え
                .ColumnHeadersVisible = True                                   ' 列ヘッダ表示あり

                '' 選択セル色定義
                .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
                .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色
                .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .ColumnHeadersHeight = 28                                       ' 列ヘッダ高さ調整

                For col As EM_COL_COLINF_2 = EM_COL_COLINF_2.MEN To EM_COL_COLINF_2.MAX_COL
                    Select Case col
                        Case EM_COL_COLINF_2.MEN
                            dgvColSetText(dgv, "面" & vbCrLf & "", 74, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_COLINF_2.P_CODE
                            dgvColSetText(dgv, "パタ" & vbCrLf & "ーン", 46, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_COLINF_2.S_CODE
                            dgvColSetText(dgv, "表面" & vbCrLf & "状態", 46, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleCenter, DataGridViewContentAlignment.MiddleCenter)
                        Case EM_COL_COLINF_2.COIL_LEN
                            ' コイル長
                            dgvColSetText(dgv, "Ｆ～", 116, True, True, DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleRight, DataGridViewContentAlignment.MiddleCenter)
                    End Select
                Next

                .RowCount = 1
                .Rows(0).Height = .ClientSize.Height - .ColumnHeadersHeight


            End With
        Catch ex As Exception
        End Try

    End Sub

    ''' <summary>
    ''' データグリッドビューテキストボックス列設定
    ''' </summary>
    ''' <param name="dgv">コントロール</param>
    ''' <param name="sHeader">列ヘッダー名称</param>
    ''' <param name="nWidth">列幅</param>
    ''' <param name="bFrozen">横スクロール時のロック</param>
    ''' <param name="bRead">読込専用</param>
    ''' <param name="nSort">並べ替えモード</param>
    ''' <param name="nDefaultCellStyle">列スタイル</param>
    ''' <param name="nHeaderCellStyle">列ヘッダースタイル</param>
    ''' <param name="sNullVal">データがNULL時の表示内容</param>
    ''' <param name="sDataNullVal">NULL入力時のデータ内容</param>
    ''' <param name="bDispVis">表示・非表示</param>
    ''' <remarks></remarks>
    Private Sub dgvColSetText(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal sHeader As String, ByVal nWidth As Integer,
     ByVal bFrozen As Boolean, ByVal bRead As Boolean, ByVal nSort As DataGridViewColumnSortMode,
     ByVal nDefaultCellStyle As DataGridViewContentAlignment, ByVal nHeaderCellStyle As DataGridViewContentAlignment,
     Optional ByVal sNullVal As String = Nothing, Optional ByVal sDataNullVal As String = Nothing, Optional ByVal bDispVis As Boolean = True)

        Using clm As New DataGridViewTextBoxColumn
            ' 列ヘッダー名称
            clm.HeaderText = sHeader
            ' 列幅
            clm.Width = nWidth
            ' 横スクロール時のロック
            clm.Frozen = bFrozen
            ' 読込専用設定
            clm.ReadOnly = bRead
            ' 並べ替えモード
            clm.SortMode = nSort
            ' 列スタイル
            clm.DefaultCellStyle.Alignment = nDefaultCellStyle
            ' 列ヘッダースタイル
            clm.HeaderCell.Style.Alignment = nHeaderCellStyle
            ' 表示・非表示
            clm.Visible = bDispVis
            ' データがNULL時の表示内容 
            clm.DefaultCellStyle.NullValue = sNullVal
            ' NULL入力時のデータ内容 
            clm.DefaultCellStyle.DataSourceNullValue = sDataNullVal
            ' 次の行に折り返さない
            clm.DefaultCellStyle.WrapMode = DataGridViewTriState.False

            ' 列作成
            dgv.Columns.Add(clm)
        End Using

    End Sub

    ''' <summary>
    ''' リサイズによる表示変更
    ''' </summary>
    ''' <param name="dgv"></param>
    Private Sub dgvWidthResize(ByVal dgv As tCnt.tCnt_DataGridView)
        Dim hsc As HScrollBar

        hsc = CType(dgv.Controls(0), HScrollBar)
        If hsc.Visible Then
            dgv.ColumnHeadersHeight = 16
            '水平スクロールバー表示
            dgv.Rows(0).Height = dgv.ClientSize.Height - dgv.ColumnHeadersHeight - hsc.Height
        Else
            dgv.ColumnHeadersHeight = 28
            dgv.Rows(0).Height = dgv.ClientSize.Height - dgv.ColumnHeadersHeight
        End If

        For ii As Integer = 0 To dgv.ColumnCount - 1
            If hsc.Visible Then
                '水平スクロールバー表示
                dgv.Item(ii, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 16.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Else
                dgv.Item(ii, 0).Style.Font = New System.Drawing.Font("ＭＳ 明朝", 20.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            End If
        Next

    End Sub


    ''///////////////////////////////////////////////////////////////////////////
    ''イベントハンドラ関数
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' リサイズ
    ''' </summary>
    ''' <param name="sender">イベント発生元</param>
    ''' <param name="e">イベントパラメータ</param>
    ''' <remarks></remarks>
    Private Sub Ctrl_ReSize(ByVal sender As Object, ByVal e As System.EventArgs)
        dgvWidthResize(m_typCoilDsp.dgvInf1)
    End Sub

End Class
