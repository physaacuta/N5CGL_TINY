'*********************************************************************************
'　基本情報管理クラス
'	[Ver]
'		Ver.01    2008/10/16  初版
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On

Imports KizuLibCLI
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tMod

Public Class clsDspDefCoil
    Implements IDisposable                          ' デストラクタ定義

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定義
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コイル情報表示コントロール
    ''' </summary>
    Public Structure TYP_COIL_DSP
        Dim dgvCoilInf As tCnt.tCnt_DataGridView
    End Structure

    ' コイル情報グリッド列定義
    Private Enum EM_GRID_COIL_COL
        emOrderNo = 0           '命令No
        emProductNo             '生産番号
        emCoilLen               'コイル長
        emSizeZai               '材料
        emHin                   '品種
        emYouto                 '用途
        emNextLine              '次工程
        emBase                  '検査基準

        emColEnd                ' 最終列 ※列数計算用
    End Enum

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバークラス
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_Log As tClass.tClass_LogManager    ' ログ管理クラス

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False        ' 重複する呼び出しを検出するには
    Private m_typCtrl As TYP_COIL_DSP               ' 基本情報管理クラスコントロール
    ' Private m_typMapInf As DSP_MAP_INF
    Private m_typCoil As clsDataMap.TYP_COIL_INF

    '---------------------------------------------------------------------------
    ' プロパティ
    '---------------------------------------------------------------------------
    '''' <summary>
    '''' コイル情報
    '''' </summary>
    '''' <value></value>
    '''' <returns></returns>
    '''' <remarks></remarks>
    '<System.ComponentModel.Browsable(True) _
    ' , System.ComponentModel.Category("☆基本情報管理クラス専用プロパティ") _
    ' , System.ComponentModel.Description("コイル情報をセット")>
    'Public Property tProp_typMapInf() As DSP_MAP_INF
    '    Get
    '        Return m_typMapInf
    '    End Get
    '    Set(value As DSP_MAP_INF)
    '        m_typMapInf = value
    '    End Set
    'End Property

    ''' <summary>
    ''' コイル情報
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    <System.ComponentModel.Browsable(True) _
     , System.ComponentModel.Category("☆基本情報管理クラス専用プロパティ") _
     , System.ComponentModel.Description("コイル情報をセット")>
    Public Property tProp_typCoil() As clsDataMap.TYP_COIL_INF
        Get
            Return m_typCoil
        End Get
        Set(value As clsDataMap.TYP_COIL_INF)
            m_typCoil = value
        End Set
    End Property

    '///////////////////////////////////////////////////////////////////////////
    'メンバ関数
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="typCoilDsp">基本情報</param>
    ''' <param name="clsLog">ログクラス</param>
    ''' <remarks></remarks>
    Public Sub New(ByRef typCoilDsp As TYP_COIL_DSP, ByRef clsLog As tClass.tClass_LogManager)

        m_typCtrl = typCoilDsp

        'ログクラス
        mcls_Log = clsLog

        InitdgvDefCoilInf(m_typCtrl.dgvCoilInf)
    End Sub

    ' IDisposable
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
    ' このコードは、破棄可能なパターンを正しく実装できるように Visual Basic によって追加されました。
    Public Sub Dispose() Implements IDisposable.Dispose
        ' このコードを変更しないでください。クリーンアップ コードを上の Dispose(ByVal disposing As Boolean) に記述します。
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' 基本情報初期化処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspInit()
        With m_typCtrl.dgvCoilInf
            For ii As Integer = 0 To .ColumnCount - 1
                Select Case ii
                    Case EM_GRID_COIL_COL.emOrderNo             ' 命令No
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emProductNo           ' 生産番号
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emCoilLen             ' コイル長
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emNextLine            ' 次工程
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emSizeZai             ' 材料
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emBase                ' 検査基準
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emHin                 ' 品種
                        .Item(ii, 0).Value = ""
                    Case EM_GRID_COIL_COL.emYouto               ' 用途
                        .Item(ii, 0).Value = ""
                End Select
            Next
        End With
    End Sub

    ''' <summary>
    ''' コイル情報表示処理
    ''' </summary>
    ''' <param name="typCoilInf">基本情報構造体</param>
    ''' <remarks></remarks>
    Public Sub DspData(typCoilInf As COINFO_BASE)

        Try
            '★基本情報表示
            With m_typCtrl.dgvCoilInf
                For ii As Integer = 0 To .ColumnCount - 1
                    Select Case ii
                        Case EM_GRID_COIL_COL.emOrderNo             ' 命令No
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.base.cMeiNo)
                        Case EM_GRID_COIL_COL.emProductNo           ' 生産番号
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.base.cCoilNoIn)
                        Case EM_GRID_COIL_COL.emCoilLen            ' コイル長
                            '.Item(ii, 0).Value = Format(Math.Floor((m_typMapInf.nCut_end - m_typMapInf.nCut_start) / 100) / 10, "0.0")
                        Case EM_GRID_COIL_COL.emNextLine            ' 次工程
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cNextLine)
                        Case EM_GRID_COIL_COL.emSizeZai             ' 材料
                            ''サイズ(板厚[μm⇒mm]×板幅[mm])
                            Dim sThick As String = Format(typCoilInf.data.CoilInf.nAtuZai / 1000, "0.000")
                            Dim sWidth As String = typCoilInf.data.CoilInf.nWidZai.ToString
                            'If 0 <> typCoilInf.data.CoilInf.nWidTrim Then
                            '    'トリム巾が0でなければトリム巾を表示
                            '    sWidth = typCoilInf.data.CoilInf.nWidTrim.ToString
                            'End If
                            .Item(ii, 0).Value = String.Format("{0}×{1,4}", sThick, sWidth)
                        Case EM_GRID_COIL_COL.emBase                ' 検査基準
                            Dim strKensaO As String = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cKenKijun_O)
                            Dim strKensaU As String = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cKenKijun_U)
                            .Item(ii, 0).Value = strKensaO & "/" & strKensaU
                        Case EM_GRID_COIL_COL.emHin                 ' 品種
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cHinCode)
                        Case EM_GRID_COIL_COL.emYouto               ' 用途
                            .Item(ii, 0).Value = tMod_Common.ByteToString(typCoilInf.data.CoilInf.cYouto)
                    End Select
                Next
            End With
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報表示異常 [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' コイル情報表示処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspData()

        Try
            If m_typCoil.strKanriNo = "" Then
                DspInit()
            End If
            '★基本情報表示
            With m_typCtrl.dgvCoilInf
                For ii As Integer = 0 To .ColumnCount - 1
                    Select Case ii
                        Case EM_GRID_COIL_COL.emOrderNo             ' 命令No
                            .Item(ii, 0).Value = m_typCoil.strOrderNo
                        Case EM_GRID_COIL_COL.emProductNo           ' 生産番号
                            .Item(ii, 0).Value = m_typCoil.strCoilNo
                        Case EM_GRID_COIL_COL.emCoilLen            ' コイル長
                            .Item(ii, 0).Value = Format(Math.Floor(m_typCoil.nCoilLen / 100) / 10, "0.0")
                        Case EM_GRID_COIL_COL.emNextLine            ' 次工程
                            .Item(ii, 0).Value = m_typCoil.strNextCode
                        Case EM_GRID_COIL_COL.emSizeZai             ' 材料
                            ''サイズ(板厚[μm⇒mm]×板幅[mm])
                            Dim sThick As String = Format(m_typCoil.nCoilAtu / 1000, "0.000")
                            Dim sWidth As String = m_typCoil.nCoilWidth.ToString
                            'If 0 <> m_typCoil.nTrimWidth Then
                            '    'トリム巾が0でなければトリム巾を表示
                            '    sWidth = m_typCoil.nTrimWidth.ToString
                            'End If
                            .Item(ii, 0).Value = String.Format("{0}×{1,4}", sThick, sWidth)
                        Case EM_GRID_COIL_COL.emBase                ' 検査基準
                            .Item(ii, 0).Value = m_typCoil.strKenKijun(0)    '検査基準(表)
                        Case EM_GRID_COIL_COL.emHin                 ' 品種
                            .Item(ii, 0).Value = m_typCoil.strHinCode
                        Case EM_GRID_COIL_COL.emYouto               ' 用途
                            .Item(ii, 0).Value = m_typCoil.strUse
                    End Select
                Next
            End With
            ' 幅、フォントサイズ調整
            Call ResizeGridCoil()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("コイル情報表示異常 [{0}]", ex.Message))
        End Try

    End Sub

    ''///////////////////////////////////////////////////////////////////////////
    ''データグリッドビュー
    ''///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' コイル情報DataGrid設定(列セット)
    ''' </summary>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub InitdgvDefCoilInf(ByVal dgv As tCnt.tCnt_DataGridView)

        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv
            .RowCount = 0
            .ScrollBars = ScrollBars.None
            .DoubleBuffered = True
            .EditMode = DataGridViewEditMode.EditProgrammatically
            .ColumnHeadersHeight = 28
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 16.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .RowTemplate.Height = .Height - .ColumnHeadersHeight - SystemInformation.Border3DSize.Height
            .RowHeadersVisible = False
            .ColumnHeadersVisible = True
            .DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 26.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

            '' 列定義
            .ColumnCount = 8
            .ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter

            ' 命令No
            With .Columns(EM_GRID_COIL_COL.emOrderNo)
                .HeaderText = "命令No"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' 生産番号
            With .Columns(EM_GRID_COIL_COL.emProductNo)
                .HeaderText = "生産番号"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' コイル長
            With .Columns(EM_GRID_COIL_COL.emCoilLen)
                .HeaderText = "コイル長"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' 材料
            With .Columns(EM_GRID_COIL_COL.emSizeZai)
                .HeaderText = "材料"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' 品種
            With .Columns(EM_GRID_COIL_COL.emHin)
                .HeaderText = "品種"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' 用途
            With .Columns(EM_GRID_COIL_COL.emYouto)
                .HeaderText = "用途"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' 次工程
            With .Columns(EM_GRID_COIL_COL.emNextLine)
                .HeaderText = "次工程"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            ' 検査基準
            With .Columns(EM_GRID_COIL_COL.emBase)
                .HeaderText = "検査基準"
                .DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .DefaultCellStyle.WrapMode = DataGridViewTriState.True
                .SortMode = DataGridViewColumnSortMode.NotSortable
            End With

            .RowCount = 1

            ' 幅、フォントサイズ調整
            Call ResizeGridCoil()
        End With
    End Sub

    ''' <summary>
    ''' コイル情報DataGridリサイズ処理
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ResizeGridCoil()
        ' コントロールサイズに応じたセルの大きさの調整
        Dim nAreaSize As Integer = m_typCtrl.dgvCoilInf.Width - SystemInformation.Border3DSize.Width * 2
        Dim nSum As Integer = 0
        Dim nFontSize As Integer
        Dim strVal As String
        Dim dSizeRate() As Double = {0.084, 0.207, 0.108, 0.209, 0.098, 0.098, 0.098, 0.098}
        'Dim dSizeRate() As Double = {0.084, 0.207, 0.108, 0.209, 0.093, 0.088, 0.078, 0.133}
        Dim nCellSize(EM_GRID_COIL_COL.emColEnd - 1) As Integer

        ' セル幅の計算
        For ii As Integer = 0 To EM_GRID_COIL_COL.emColEnd - 2
            nCellSize(ii) = CInt(Math.Floor(nAreaSize * dSizeRate(ii)))
            nSum += nCellSize(ii)
        Next
        nCellSize(EM_GRID_COIL_COL.emColEnd - 1) = nAreaSize - nSum

        ' 幅に合わせたフォントサイズの調整
        With m_typCtrl.dgvCoilInf
            For nColIdx As Integer = 0 To EM_GRID_COIL_COL.emColEnd - 1
                ' 幅、ヘッダーフォント
                With .Columns(nColIdx)
                    .Width = nCellSize(nColIdx)

                    strVal = .HeaderText
                    nFontSize = CInt(GetGridFontScaleSize(m_typCtrl.dgvCoilInf, strVal, m_typCtrl.dgvCoilInf.ColumnHeadersDefaultCellStyle.Font.Size, .HeaderCell.Size.Width, .HeaderCell.Size.Height))
                    .HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ 明朝", nFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End With

                ' 値フォント
                With .Item(nColIdx, 0)
                    ' 値
                    If .Value IsNot Nothing Then
                        strVal = .Value.ToString
                    Else
                        strVal = ""
                    End If
                    nFontSize = CInt(GetGridFontScaleSize(m_typCtrl.dgvCoilInf, strVal, m_typCtrl.dgvCoilInf.DefaultCellStyle.Font.Size, .Size.Width, .Size.Height))
                    .Style.Font = New System.Drawing.Font("ＭＳ 明朝", nFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                End With
            Next
        End With
    End Sub

End Class
