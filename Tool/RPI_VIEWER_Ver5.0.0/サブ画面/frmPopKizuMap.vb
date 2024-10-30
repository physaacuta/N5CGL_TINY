Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Public Class frmPopKizuMap

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    Public Event tEv_MapClick(ByVal pos As Point)						' クリック (mm座標系) マップ左上が0mmとしての距離
    Public Event tEv_KizuClick(ByVal pos As Point)						' クリック (mm座標系) マップ左上が0mmとしての距離

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    '/ /////////////////////////////////////////////////////////////////////////////////

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数

    ' クラス
    Private mcls_pParam As clsParamManager = Nothing					' パラメータ管理クラス (ポインタ)
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing				' RPIファイル表示クラス (ポインタ)

    ' データ
    Private m_typCoil As DATA_COIL										' コイル情報
    Private m_lstDetect As List(Of DATA_DETECT_INF) = Nothing           ' 疵情報リスト
    Private m_lstCut As List(Of DATA_CUT_INF) = Nothing                 ' カット情報リスト

    Private m_nHabaCol As Integer                                       ' 幅分割数
    Private m_nInterval As Integer                                      ' 長手サイズ
    Private m_nHabaInterval As Integer                                  ' 幅サイズ
    Private m_nCoilLenRow As Integer                                    ' DataGridView行数(コイル長より算出)

    Private m_nSizeX As Integer                                         ' 画面サイズX（RESIZE時の比較用）
    Private m_nSizeY As Integer                                         ' 画面サイズY（RESIZE時の比較用）

    Private m_nNowRow As Integer                                        ' 全長画像表示位置：今回値（マップ行位置換算）
    Private m_nOldRow As Integer                                        ' 全長画像表示位置：前回値（マップ行位置換算）

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ
    ''' <summary>
    ''' パラメータクラスセット
    ''' </summary>
    ''' <param name="cls">計算クラス</param>
    Public Sub SetCls_Param(ByRef cls As clsParamManager)
        mcls_pParam = cls
    End Sub

    ''' <summary>
    ''' RPIクラス
    ''' </summary>
    ''' <param name="cls">RPIクラス</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub

    ''' <summary>
    ''' コイル情報
    ''' </summary>
    ''' <param name="typ">疵情報リスト</param>
    ''' <remarks></remarks>
    Friend Sub SetData_Coil(ByVal typ As DATA_COIL)
        m_typCoil = typ
    End Sub

    ''' <summary>
    ''' 疵情報リスト
    ''' </summary>
    ''' <param name="lst">疵情報リスト</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstDetect(ByRef lst As List(Of DATA_DETECT_INF))
        m_lstDetect = lst
    End Sub

    ''' <summary>
    ''' カット情報リスト
    ''' </summary>
    ''' <param name="lst">カット情報リスト</param>
    ''' <remarks></remarks>
    Friend Sub SetData_LstCut(ByRef lst As List(Of DATA_CUT_INF))
        m_lstCut = lst
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本
    ''' <summary>
    ''' ロード
    ''' </summary>
    Private Sub frmPopKizuMap_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        '初期値セット(前回値読込）
        Load_last_Control_Ini()
    End Sub

    ''' <summary>
    ''' フォームサイズ変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPopKizuMap_ResizeEnd(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.ResizeEnd
        ' 縦のみのサイズ変更の場合は画面更新無し
        If Me.Size.Width = m_nSizeX Then
            ' 変更後のサイズを保存
            m_nSizeX = Me.Size.Width
            m_nSizeX = Me.Size.Height
            Exit Sub
        End If
        ' 変更後のサイズを保存
        m_nSizeX = Me.Size.Width
        m_nSizeX = Me.Size.Height

        '１列幅サイズ算出
        Dim nSize As Integer = CInt((dgvMap.Width - 50 - 18) / m_nHabaCol)

        '列幅を変更
        For ii As Integer = 1 To m_nHabaCol
            dgvMap.Columns(ii).Width = nSize
        Next ii
    End Sub

    ''' <summary>
    ''' マップ選択(疵JUMP)
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMap_CellMouseDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellMouseEventArgs) Handles dgvMap.CellMouseDown
        Dim dgv As tCnt.tCnt_DataGridView = CType(sender, tCnt.tCnt_DataGridView)   ' 対象のdgv
        Dim dgvCell As DataGridViewCell

        Dim nX As Integer = 0
        Dim nY As Integer = 0
        Try
            If e.RowIndex < 0 Then Exit Sub

            ' 選択セル情報セット
            dgvCell = dgv(e.ColumnIndex, e.RowIndex)
            If 0 = e.ColumnIndex Or "" = dgvCell.Value.ToString Then
                '疵無し
                nX = e.ColumnIndex * m_nHabaInterval * m_nInterval
                nY = e.RowIndex * 1000 * m_nInterval
                RaiseEvent tEv_MapClick(New Point(nX, nY))
            Else
                '疵あり
                nX = CType(dgvCell.Tag, DATA_DETECT_INF).nFrameXc
                nY = CType(dgvCell.Tag, DATA_DETECT_INF).nY
                RaiseEvent tEv_KizuClick(New Point(nX, nY))
            End If

            m_nNowRow = CInt(Math.Floor(nY / (m_nInterval * 1000)))	'△

        Catch ex As Exception

        End Try
    End Sub

    ''' <summary>
    ''' 反映ボタン押下
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnReflect_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnReflect.Click

        MapDisp()

    End Sub

    ''' <summary>
    ''' 描画
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvMap_Paint(ByVal sender As System.Object, ByVal e As System.Windows.Forms.PaintEventArgs) Handles dgvMap.Paint
        Dim penNow As New System.Drawing.Pen(Color.Orange, 3)                '全長画像表示位置色
        Dim penCut As New System.Drawing.Pen(Color.Green, 2)                '全長画像表示位置色
        penCut.DashStyle = Drawing2D.DashStyle.Dash
        Try

            Dim nStartPos As Integer = dgvMap.FirstDisplayedScrollingRowIndex

            For nDspRow As Integer = 0 To dgvMap.DisplayedRowCount(False) - 1
                If m_nNowRow = (nDspRow + nStartPos) Then
                    e.Graphics.DrawRectangle(penNow, 0, dgvMap(0, 0).Size.Height * nDspRow + dgvMap.ColumnHeadersHeight, _
                                                        dgvMap.Width - 18, dgvMap(0, 0).Size.Height)
                End If
            Next

            For nDspRow As Integer = 0 To dgvMap.DisplayedRowCount(False) - 1
                If CType(dgvMap(0, nDspRow + nStartPos).Tag, Integer) = 1 Then
                    e.Graphics.DrawLine(penCut, 0, dgvMap(0, 0).Size.Height * nDspRow + dgvMap.ColumnHeadersHeight, _
                                                        dgvMap.Width - 18, dgvMap(0, 0).Size.Height * nDspRow + dgvMap.ColumnHeadersHeight)

                End If
            Next
        Catch ex As Exception
        Finally
            penNow.Dispose()
            penCut.Dispose()
        End Try
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' マップ描画
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub MapDisp()

        '基本情報セット
        BaseInfSet()
        '画面初期化（距離セット）
        DspInit()

        'コイル情報が無い場合は処理無し
        If m_typCoil.sKizukenNo Is Nothing Then Exit Sub


        Dim typdt As DATA_DETECT_INF
        Dim nRow As Integer
        Dim nCol As Integer

        For ii As Integer = 0 To m_lstDetect.Count - 1
            typdt = m_lstDetect(ii)
            If Not typdt.bDsp Then Continue For

            '対象行位置取得
            'nRow = CInt(typdt.nY / (m_nInterval * 1000))
            nRow = CInt(Math.Floor(typdt.nY / (m_nInterval * 1000)))	'△
            If dgvMap.RowCount <= nRow Then
                nRow = dgvMap.RowCount - 1
            End If
            If nRow < 0 Then
                nRow = 0
            End If
            '対象列位置取得
            nCol = SelectColPosition(1, typdt.nXc, m_typCoil.nWid, m_nHabaCol, m_nHabaInterval) + 1
            Dim bWrite As Boolean = True
            If CType(dgvMap(nCol, nRow).Tag, DATA_DETECT_INF).nNo <> 0 Then
                ' 優先順位
                ' ① グレード優先度
                ' ② 疵種優先度
                ' ③ 長手順
                Dim dNow(2) As Double
                Dim dPer(2) As Double
                Dim bMode(2) As Boolean

                dNow(0) = CDbl(typdt.gYu)
                dPer(0) = CDbl(CType(dgvMap(nCol, nRow).Tag, DATA_DETECT_INF).gYu)
                bMode(0) = False

                dNow(1) = CDbl(typdt.tYu)
                dPer(1) = CDbl(CType(dgvMap(nCol, nRow).Tag, DATA_DETECT_INF).tYu)
                bMode(1) = False

                dNow(2) = CDbl(typdt.nY)
                dPer(2) = CDbl(CType(dgvMap(nCol, nRow).Tag, DATA_DETECT_INF).nY)
                bMode(2) = False

                ' 優先度比較
                If Not tMod.SelectPriorityDetect(dNow, dPer, bMode) Then
                    ' 新旧入れ替えなし
                    bWrite = False
                End If
            End If

            '代表疵セット
            If bWrite Then
                dgvMap(nCol, nRow).Value = mcls_pParam.GetParamType(typdt.tId).sSName
                dgvMap(nCol, nRow).Style.BackColor = mcls_pParam.GetParamGrade(typdt.gId).clrDspColor

                dgvMap(nCol, nRow).Tag = typdt
            End If
        Next

        Dim typCutdt As DATA_CUT_INF
        For ii As Integer = 0 To m_lstCut.Count - 1
            typCutdt = m_lstCut(ii)
            nRow = CInt(Math.Ceiling(typCutdt.nY / (m_nInterval * 1000)))   '△
            If dgvMap.RowCount <= nRow Then
                nRow = dgvMap.RowCount - 1
            End If
            If nRow < 0 Then
                nRow = 0
            End If
            dgvMap(0, nRow).Tag = 1
        Next

    End Sub

    ''' <summary>
    ''' 疵マップ初期化処理
    ''' </summary>
    ''' <param name="dgv">マップデータグリッド</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As tCnt.tCnt_DataGridView)

        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                           ' 選択色無し

        With dgv

            .RowCount = m_nCoilLenRow
            .ColumnHeadersHeight = 24

            '' 動作定義
            .ScrollBars = ScrollBars.Vertical                             ' スクロールバーの表示
            '.ScrollBars = ScrollBars.None
            '.DoubleBuffered = True

            .ReadOnly = True                                                ' 読み取り専用
            .EditMode = DataGridViewEditMode.EditProgrammatically           ' セル書き換え

            .ColumnHeadersVisible = True                                   ' 列ヘッダ表示

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = .DefaultCellStyle().ForeColor      '選択色

            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ ゴシック", 8.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            .DefaultCellStyle().WrapMode = DataGridViewTriState.True

            '' 列定義
            .ColumnCount = CInt(nudHabaCol.Value) + 1           'マップ列数+入側表示欄

            '列0 (アドレス)
            .Columns(0).HeaderText = "距離"
            .Columns(0).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
            .Columns(0).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight
            .Columns(0).Width = 50
            .Columns(0).SortMode = DataGridViewColumnSortMode.NotSortable
            .Columns(0).Frozen = True                                       ' 水平移動禁止
            .Columns(0).HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Dim nSize As Integer = CInt((dgv.Width - 50 - 18) / m_nHabaCol)

            '列1～7 (データ)
            For ii As Integer = 1 To m_nHabaCol
                .Columns(ii).HeaderText = HeaderName(ii, m_typCoil.nWid, m_nHabaCol)
                .Columns(ii).HeaderCell.Style.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                .Columns(ii).Width = nSize
                .Columns(ii).SortMode = DataGridViewColumnSortMode.NotSortable
                .Columns(ii).Frozen = True                                       ' 水平移動禁止
                .Columns(ii).HeaderCell.Style.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
                .Columns(ii).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ ゴシック", 9.0, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            Next ii

        End With

    End Sub

    ''' <summary>
    ''' グリッド位置判定関数 (センター基準)
    ''' </summary>
    ''' <param name="DorW">0:DS基準の疵 1:WS基準の疵 ※１固定</param>
    ''' <param name="nXc">C基準幅位置[mm](DS:-, WS:+)</param>
    ''' <param name="Width">コイル板幅 [mm]</param>
    ''' <param name="col_max">最大グリッド(物理的な最大グリッド数)</param>
    ''' <param name="col_interval">グリッドの幅 [mm]</param>
    ''' <returns>算出グリッド位置 (0:DS～)</returns>
    ''' <remarks></remarks>
    Private Function SelectColPosition(ByVal DorW As Integer, ByVal nXc As Integer, ByVal Width As Integer, ByVal col_max As Integer, ByVal col_interval As Integer) As Integer
        Dim env As Integer = col_max Mod 2                      '// 探傷画面の列数 (0:偶数列  1:奇数列)
        Dim centerWid As Integer = Width \ 2                    '// 板幅の中心
        Dim centerCol As Integer = col_max \ 2                  '// DS基準のグリッド位置算出用 (戻り値)
        Dim nSetCol As Integer = 0                              '// 算出グリッド位置用ワーク

        If 0 > nXc Then
            If 0 = DorW Then
                DorW = 1
            Else
                DorW = 0
            End If
            nXc = System.Math.Abs(nXc)
        End If

        '//-----------------------------------------
        '// コイルセンター基準
        Dim col As Integer = 0
        If 0 = env Then '// 偶数	
            col = (nXc - 1) \ (col_interval)
            If 0 = DorW Then
                nSetCol = centerCol - col - 1   '// 基準側
            Else
                nSetCol = centerCol + col       '// 反対側
            End If
        Else            '// 奇数
            nXc -= col_interval \ 2
            col = (nXc - 1) \col_interval
            If 0 = DorW Then
                nSetCol = centerCol - col - 1   '// 基準側
            Else
                nSetCol = centerCol + col + 1   '// 反対側
            End If
        End If

        '算出結果の上下限チェック
        If nSetCol < 0 Then
            nSetCol = 0
        End If
        If nSetCol > col_max - 1 Then
            nSetCol = col_max - 1
        End If

        Return nSetCol

    End Function

    ''' <summary>
    ''' ヘッダー名称取得
    ''' </summary>
    ''' <param name="nCol">取得ヘッダー列(1～)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function HeaderName(ByVal nCol As Integer, ByVal Width As Integer, ByVal col_max As Integer) As String
        Dim env As Integer = col_max Mod 2                      '// 探傷画面の列数 (0:偶数列  1:奇数列)
        Dim centerCol As Integer = CInt(Int(col_max / 2)) + 1      '// DS基準のグリッド位置算出用 (戻り値)
        Dim nWid As Integer = CInt(Math.Ceiling(Width / col_max))
        Dim nHalfWid As Integer = CInt(Math.Ceiling(nWid / 2))
        Dim strName As String = ""

        If 0 = env Then
            '// 偶数
            If centerCol - 1 = nCol Then
                strName = nWid & "～" '"～C"
            ElseIf centerCol = nCol Then
                strName = "～" & nWid '"C～" & nWid
            Else
                If centerCol > nCol Then
                    strName = nWid * (centerCol - nCol) & "～"
                Else
                    strName = "～" & nWid * (nCol - centerCol + 1)
                End If
            End If
        Else
            '// 奇数
            If centerCol = nCol Then
                strName = nHalfWid & "～C～" & nHalfWid
            Else
                If centerCol > nCol Then
                    strName = nWid * (centerCol - nCol) + nHalfWid & "～"
                Else
                    strName = "～" & nWid * (nCol - centerCol) + nHalfWid
                End If
            End If
        End If

        Return strName
    End Function

    ''' <summary>
    ''' 基本情報セット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub BaseInfSet()
        m_nNowRow = 0
        m_nOldRow = -1
        m_nHabaCol = CInt(nudHabaCol.Value)                                 ' 幅分割数
        m_nInterval = CInt(nudInterval.Value)                               ' 長手サイズ
        m_nCoilLenRow = CInt((m_typCoil.nLen / 1000) / m_nInterval) + 1     ' 表示行数
        m_nHabaInterval = CInt(Math.Ceiling(m_typCoil.nWid / m_nHabaCol))	' 切り上げ
        If (1 > m_nHabaInterval) Then
            m_nHabaInterval = 1

        End If
    End Sub

    ''' <summary>
    ''' マップ初期化（距離はセット）
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInit()
        ' マップ初期化
        dgvInitialize(dgvMap)

        For ii As Integer = 0 To m_nCoilLenRow - 1
            dgvMap(0, ii).Value = m_nInterval * ii
            ' セルの初期化
            For jj As Integer = 1 To m_nHabaCol
                dgvMap(jj, ii).Value = ""
                dgvMap(jj, ii).Style.BackColor = Color.Empty
            Next jj
        Next ii
    End Sub

    ''' <summary>
    ''' 指定位置へジャンプ（中心）
    ''' </summary>
    ''' <param name="nLen">指定位置[mm]</param>
    ''' <remarks></remarks>
    Public Sub DspJumpRow(ByVal nLen As Integer)
        Dim nRow As Integer

        '１行も無い時は処理終了
        If dgvMap.RowCount <= 0 Then
            Exit Sub
        End If

        Try
            'ユーザーに表示される行の数を取得
            Dim nDspRowCnt As Integer = dgvMap.DisplayedRowCount(False)

            '指定位置[mm]を行に変換(切り捨て)
            m_nNowRow = CInt(Math.Floor(nLen / (m_nInterval * 1000)))
            If m_nNowRow < 0 Then
                m_nNowRow = 0
            End If
            If m_nNowRow > dgvMap.RowCount - 1 Then
                m_nNowRow = dgvMap.RowCount - 1
            End If

            '前回値と同じ行位置だったら何もせずに終了
            If m_nNowRow = m_nOldRow Then
                Exit Sub
            End If

            '指定位置が中心になるように先頭位置(左隅)を算出する
            nRow = m_nNowRow - CInt(Math.Floor(nDspRowCnt / 2))
            '最終行を超える場合
            If nRow > (dgvMap.RowCount - nDspRowCnt) Then
                nRow = dgvMap.RowCount - nDspRowCnt
            End If
            '先頭行を超える場合
            If nRow < 0 Then
                nRow = 0
            End If

            '前回値へ今回値を保存
            m_nOldRow = m_nNowRow
            '
            '表示される最初の行のインデックスを設定
            dgvMap.FirstDisplayedScrollingRowIndex = nRow

            '画面リフレッシュ
            dgvMap.Refresh()

        Catch ex As Exception

        End Try

    End Sub

    ''' <summary>
    ''' 前回値保存
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Save_last_Control_Ini()
        'シェーディング補正
        tMod.WriteIni("RPI_VIEWER", "KIZU_MAP_HABA_COL", LOCALINI_NAME, CStr(nudHabaCol.Value))                 '幅分割数
        tMod.WriteIni("RPI_VIEWER", "KIZU_MAP_INTERVAL", LOCALINI_NAME, CStr(nudInterval.Value))                '長手サイズ
    End Sub

    ''' <summary>
    ''' 前回値読込
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Load_last_Control_Ini()
        nudHabaCol.Value = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "KIZU_MAP_HABA_COL", LOCALINI_NAME, 10)   '未指定の場合は、10
        nudInterval.Value = tMod.tMod_IniFile.ReadIniInt("RPI_VIEWER", "KIZU_MAP_INTERVAL", LOCALINI_NAME, 2)   '未指定の場合は、 2
    End Sub

End Class