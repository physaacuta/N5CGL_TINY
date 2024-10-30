Option Strict On
'*********************************************************************************
' 輝度補正画面クラス
'	[Ver]
'		Ver.01    2014/07/25  初版
'
'
Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmKidoHosei

    ''' <summary>
    ''' メンバ変数
    ''' </summary>
    ''' <remarks></remarks>
    ''' 
    Private mcls_ImgDsp As tClass_ImageDsp         ' 画像表示クラス
    Private m_PicBox As PictureBox                 ' 画像表示領域
    Private mcls_Log As tClass_LogManager          ' ログ管理
    Private m_pgb As ProgressBar                   ' プログレスバー

    Dim m_CorSave As String                        ' 補正値保存先

    Dim m_PicSouce() As Byte                       ' 元の画像
    Dim m_PicCorrected() As Byte                   ' 補正済み画像

    Dim m_KidoHAve() As Integer                    ' 縦平均輝度配列
    Dim m_KidoMvAve() As Integer                   ' 縦平均輝度配列(輝度移動平均)
    Dim m_KidoHosei() As Integer                   ' 輝度補正値配列
    Dim m_KidoBasePoint() As Integer               ' 基点輝度値配列

    Dim m_bRaw As Boolean = False                  ' 輝度計算完了Flag（False：未完了、True：完了）
    Dim m_bAve As Boolean = False                  ' 輝度（移動平均）計算完了Flag（False：未完了、True：完了）
    Dim m_bCorPic As Boolean = False               ' 輝度補正済み画像Flag
    Dim m_bCorCalc As Boolean = False              ' 輝度補正計算完了Flag
    Dim m_bBasePoint As Boolean = False            ' 基点輝度値Flag

    Dim m_nMvAveSize As Integer                    ' 移動平均サイズ
    Dim m_nCameraSet As Integer                    ' カメラセット
    Dim m_nBasePoint As Integer                    ' 基点輝度値
    Dim m_nKidoMax As Integer = 0                  ' 最大輝度値
    Dim m_nKidoMax_A As Integer = 0                ' 移動平均最大輝度値
    Dim m_nPicWidth As Integer                     ' 画像の幅サイズ
    Dim m_nPicHeight As Integer                    ' 画像の縦サイズ
    Dim m_nSelectX As Integer                      ' 幅方向の選択位置

    Dim mclr_LineRaw As Color                      ' 線の色（縦プロジェクション線生）
    Dim mclr_LineAve As Color                      ' 線の色（縦プロジェクション線移動平均）
    Dim mclr_LineCor As Color                      ' 線の色（補正輝度線）
    Dim mclr_LineSel As Color                      ' 線の色（幅方向選択線）

    ''' <summary>
    ''' 定数定義
    ''' </summary>
    ''' <remarks></remarks>
    Private Const ROW_KIDO_RAW As Integer = 1             ' 縦プロジェクション（生）の行番号
    Private Const ROW_KIDO_AVE As Integer = 2             ' 縦プロジェクション線（移動平均）の行番号
    Private Const ROW_KIDO_COR As Integer = 3             ' 補正値の行番号
    Private Const ROW_KIDO_BP As Integer = 4              ' 基点輝度値の行番号

    Private Const RAD_INIT As Integer = 0                 ' ラジオボタン（初期状態）
    Private Const RAD_BEFORE As Integer = 1               ' ラジオボタン（補正前）
    Private Const RAD_AFTER As Integer = 2                ' ラジオボタン（補正後）
    Private Const RAD_COR As Integer = 3                  ' ラジオボタン（補正値）

    Private Const MY_SECTION As String = "KIDO_HOSEI"     ' iniファイルに使用する自セクション名
    Private Const KEY_MV_AVE As String = "MV_AVE_SIZE"    ' iniファイルに使用する移動平均サイズのKEY
    Private Const HOSEI_SAVE As String = "HOSEI_SAVE"     ' 補正値出力先KEY
    Private Const PATH_COR_DEF As String = ".\KidoHosei"  ' 補正値の出力先(Default値)
    Private Const PATH_PIC_DEF As String = "..\TEMP\BMP"  ' 表示画像の出力先(Default値)

    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <param name="log"></param>
    ''' <remarks></remarks>
    Public Sub New(ByVal log As tClass_LogManager)

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
        mcls_Log = log

    End Sub

    ''' <summary>
    ''' Loadイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKidoHosei_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load

        mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("輝度補正フォームイベント開始 "))

        mcls_ImgDsp = New tClass_ImageDsp()
        m_pgb = pgbProcess
        m_PicBox = pbxBitMap
        mcls_ImgDsp.AddHandler_PaintEvent(m_PicBox)

        ' PictureBoxのプロパティーでは変更できないためここで変更する
        pbxBitMap.AllowDrop = True
        ' iniﾌｧｲﾙ読込
        ReadIniFile()
        ' dgvの設定
        dgvInitialize(dgvKido)
        ' 縦プロジェクション（移動平均）のCheckBoxはDefaultあり
        chkBoxHProMvAve.Checked = True
        ' CheckBoxのテキストの色設定
        chkBoxHProRow.ForeColor = mclr_LineRaw
        chkBoxHProMvAve.ForeColor = mclr_LineAve
        chkBoxCorrected.ForeColor = mclr_LineCor
        chkBoxSelect.ForeColor = mclr_LineSel
        ' PictureBoxの設定
        mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth
        pbxBitMap.Dock = DockStyle.Fill
        HScrollBM.Visible = False
        VScrollBM.Visible = False
        ' ラジオボタンの設定
        ChgRadBtn(RAD_INIT)
        ' 最大値移動へ移動ボタン
        btnRawMax.Enabled = False
        btnAveMax.Enabled = False

        ' イベント定義
        AddHandler mcls_ImgDsp.tEv_Paint_End, AddressOf PaintPicBoxFrame
    End Sub

    ''' <summary>
    ''' FormCloseイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKidoHosei_FormClosed(ByVal sender As System.Object, ByVal e As System.Windows.Forms.FormClosedEventArgs) Handles MyBase.FormClosed
        SaveData()
        modMain.TaskEnd()
    End Sub

    ''' <summary>
    ''' ドラッグイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub EV_DragEnter(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles pbxBitMap.DragEnter, Me.DragEnter
        Dim file() As String
        Dim ex As String

        If e.Data.GetDataPresent(DataFormats.FileDrop) Then
            file = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
            ex = IO.Path.GetExtension(file(0))
            ' BitMapとJpeg以外は受け付けない
            If ex = ".bmp" Or ex = ".BMP" Or ex = ".jpg" Or ex = ".JPG" Or ex = ".jpeg" Or ex = ".JPEG" Then
                e.Effect = DragDropEffects.Copy
            End If
        End If
    End Sub

    ''' <summary>
    ''' ドロップイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub EV_DragDrop(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DragEventArgs) Handles pbxBitMap.DragDrop, Me.DragDrop
        Dim file As String()
        Dim byImg As Byte()

        Try
            file = CType(e.Data.GetData(DataFormats.FileDrop, False), String())
            Dim BmPath As String = file(0)
            Dim ex As String = IO.Path.GetExtension(BmPath)
            Dim img As Image = tClass.tClass_BmpJpeg.ImgFileLoad(BmPath)

            ' 取り込み画像のメンバを設定
            Dim width As Integer = img.Width
            Dim height As Integer = img.Height

            ' 画像データからバイト配列への変換データを取得する
            If ex = ".bmp" Or ex = ".BMP" Then
                byImg = tClass.tClass_BmpJpeg.ImagToData(CType(img, Bitmap))
            Else
                byImg = tClass.tClass_BmpJpeg.JpegToData(BmPath)
            End If

            If Not byImg Is Nothing Then
                ' 画像初期表示は全体にする
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emRaw Then
                    RateChg()
                End If

                '' メンバの初期化
                m_PicCorrected = Nothing
                m_bRaw = False
                m_bAve = False
                m_bCorPic = False
                m_bCorCalc = False
                m_bBasePoint = False
                m_KidoHAve = Nothing
                m_KidoMvAve = Nothing
                m_KidoHosei = Nothing
                m_KidoBasePoint = Nothing

                m_nKidoMax = 0
                m_nKidoMax_A = 0

                m_nSelectX = 0

                ' MAX表示ボタン
                btnRawMax.Enabled = False
                btnAveMax.Enabled = False

                ' グリッドのHスクロールバー
                HScrollDgv.Value = 0

                ' 画像表示
                DisplayImage(width, height, byImg)

                ' グリッド更新
                dgvKido.Rows.Clear()
                dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)

                ' 取り込み画像のメンバを設定する
                m_PicSouce = byImg
                m_nPicWidth = width
                m_nPicHeight = height

                ' ラジオボタンの初期化
                ChgRadBtn(RAD_INIT)
                btnHoseitiRead.Enabled = False
            Else
                Dim frm As New tClassForm.frmPop_MsgBox("画像の取り込みに失敗しました", "ドラッグドロップ")
                frm.Show()
            End If
        Catch ex As Exception
            ' 8bit画像以外は例外発生します
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ドロップイベントで例外が発生しました [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox("画像の取り込みに失敗しました", "ドラッグドロップ")
            frm.Show()
        End Try
    End Sub

    ''' <summary>
    ''' PictureBoxへの描画イベント
    ''' </summary>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub PaintPicBoxFrame(ByVal e As System.Windows.Forms.PaintEventArgs)
        Dim g As Graphics = e.Graphics
        Dim pen_Law As New Pen(mclr_LineRaw, 2)
        Dim pen_Ave As New Pen(mclr_LineAve, 2)
        Dim pen_Cor As New Pen(mclr_LineCor, 2)
        Dim pen_Sel As New Pen(mclr_LineSel, 2)
        Dim pbx_width As Integer = pbxBitMap.ClientSize.Width
        Dim startP As Integer
        Dim endP As Integer

        ' PicBox高さに合わせて0~255の範囲でLineをひく
        Dim yy As Double = pbxBitMap.Height / 255     ' PicBox高さを0~255の範囲にした場合のY軸1単位の値
        Try
            ' 縦プロジェクション（生）描画
            If m_bRaw = True AndAlso chkBoxHProRow.Checked = True Then
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                    ' 画像全体表示時
                    startP = 0               ' 画像の左端
                    endP = m_nPicWidth - 1   ' 画像の右端
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Law, CInt((i - 1) * (pbx_width / m_nPicWidth)), CInt(pbxBitMap.Height - (m_KidoHAve(i - 1) * yy)), CInt((i) * (pbx_width / m_nPicWidth)), CInt(pbxBitMap.Height - (m_KidoHAve(i) * yy)))
                        End If
                    Next
                Else
                    ' 実画像表示時
                    startP = HScrollBM.Value                        ' 画像の表示位置の左端
                    endP = pbxBitMap.Size.Width + startP - 1        ' 画像の表示位置の右端
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Law, i - startP - 1, pbxBitMap.Height - CInt((m_KidoHAve(i - 1) * yy)), i - startP, pbxBitMap.Height - CInt((m_KidoHAve(i)) * yy))
                        End If
                    Next

                End If
            End If

            ' 縦プロジェクション（移動平均）描画
            If m_bAve = True AndAlso chkBoxHProMvAve.Checked = True Then
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                    ' 画像全体表示時
                    startP = 0               ' 画像の左端
                    endP = m_nPicWidth - 1   ' 画像の右端
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Ave, CInt((i - 1) * (pbx_width / m_nPicWidth)), pbxBitMap.Height - CInt((m_KidoMvAve(i - 1)) * yy), CInt((i) * (pbx_width / m_nPicWidth)), pbxBitMap.Height - CInt(m_KidoMvAve(i) * yy))
                        End If
                    Next
                Else
                    ' 実画像表示時
                    startP = HScrollBM.Value                         ' 画像の表示位置の左端
                    endP = pbxBitMap.Size.Width + startP - 1         ' 画像の表示位置の右端
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Ave, i - startP - 1, pbxBitMap.Height - CInt((m_KidoMvAve(i - 1) * yy)), i - startP, pbxBitMap.Height - CInt((m_KidoMvAve(i) * yy)))
                        End If
                    Next

                End If
            End If

            ' 補正輝度線描画
            If Not m_KidoBasePoint Is Nothing AndAlso chkBoxCorrected.Checked = True Then
                'g.DrawLine(pen_Cor, 0, pbxBitMap.Height - CInt(m_KidoBasePoint(0) * yy), pbx_width, pbxBitMap.Height - CInt(m_KidoBasePoint(0) * yy))
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                    ' 画像全体表示時
                    startP = 0               ' 画像の左端
                    endP = m_nPicWidth - 1   ' 画像の右端
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Cor, CInt((i - 1) * (pbx_width / m_nPicWidth)), pbxBitMap.Height - CInt((m_KidoBasePoint(i - 1)) * yy), CInt((i) * (pbx_width / m_nPicWidth)), pbxBitMap.Height - CInt(m_KidoBasePoint(i) * yy))
                        End If
                    Next
                Else
                    ' 実画像表示時
                    startP = HScrollBM.Value                         ' 画像の表示位置の左端
                    endP = pbxBitMap.Size.Width + startP - 1         ' 画像の表示位置の右端
                    For i As Integer = startP To endP
                        If Not i = 0 Then
                            g.DrawLine(pen_Cor, i - startP - 1, pbxBitMap.Height - CInt((m_KidoBasePoint(i - 1) * yy)), i - startP, pbxBitMap.Height - CInt((m_KidoBasePoint(i) * yy)))
                        End If
                    Next

                End If
            End If

            ' 補正輝度線描画
            If m_nSelectX > 0 AndAlso chkBoxSelect.Checked = True Then
                If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                    ' 画像全体表示時
                    Dim nSel As Integer = CInt(pbx_width / m_nPicWidth * m_nSelectX)
                    g.DrawLine(pen_Sel, nSel, 0, nSel, pbxBitMap.Height)
                    'g.DrawLine(pen_Law, CInt((i - 1) * (pbx_width / m_nPicWidth)), CInt(pbxBitMap.Height - (m_KidoHAve(i - 1) * yy)), CInt((i) * (pbx_width / m_nPicWidth)), CInt(pbxBitMap.Height - (m_KidoHAve(i) * yy)))
                Else
                    ' 実画像表示時
                    If m_nSelectX >= HScrollBM.Value And m_nSelectX < (HScrollBM.Value + pbxBitMap.Size.Width) Then
                        Dim nSel As Integer = m_nSelectX - HScrollBM.Value
                        g.DrawLine(pen_Sel, nSel, 0, nSel, pbxBitMap.Height)
                    End If
                End If
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ペイントイベント中に例外が発生しました [{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' 画像の表示方法変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRateChg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRateChg.Click
        InformLbl("", False, lblInform)
        RateChg()
    End Sub

    ''' <summary>
    ''' サイズ切替を行う
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub RateChg()
        HScrollBM.Minimum = 0
        HScrollBM.Maximum = m_nPicWidth - 1

        VScrollBM.Minimum = 0
        VScrollBM.Maximum = m_nPicHeight - 1

        If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
            ' 全体サイズから×1サイズへ切替
            btnRateChg.Text = "全体"
            HScrollBM.Visible = True
            VScrollBM.Visible = True
            mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emRaw
            HScrollBM.LargeChange = pbxBitMap.Size.Width
            VScrollBM.LargeChange = pbxBitMap.Size.Height
            pbxBitMap.Height = pbxBitMap.Parent.ClientSize.Height - VScrollBM.Width
            pbxBitMap.Width = pbxBitMap.Parent.ClientSize.Width - HScrollBM.Height
            pbxBitMap.Dock = DockStyle.None
        Else
            ' ×１サイズから全体サイズへ切替
            btnRateChg.Text = "×1"
            mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth
            pbxBitMap.Dock = DockStyle.Fill
            HScrollBM.Visible = False
            VScrollBM.Visible = False
            HScrollBM.LargeChange = m_nPicWidth
            VScrollBM.LargeChange = m_nPicHeight
        End If

        mcls_ImgDsp.tProp_AriaInf_Aria = m_PicBox.ClientSize
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' 表示設定
    ''' </summary>
    ''' <param name="dgv">データグリッドビュー</param>
    ''' <remarks></remarks>
    Private Overloads Sub dgvInitialize(ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        '''' 基本を設定
        Call dgv.dgvInitialize(dgv)
        Call dgv.DataGridViewSelectionCancel(dgv)                         ' 選択色無し
        Call dgv.DataGridViewDataExec(dgv)

        With dgv
            .RowTemplate.Height = 27                                      ' 改行されてる文字が見えてしまうので、デフォルトの行高さを変更
            .ScrollBars = ScrollBars.None                                 ' スクロールバーの表示

            '''' 列ヘッダー 文字サイズ変更,文字色変更
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '' 選択セル色定義
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor      '選択色
            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue              ' 1行おきに色替え
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue     '一行おきに選択色変更
            .DefaultCellStyle().Font = New System.Drawing.Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .DefaultCellStyle().ForeColor = Color.Black
            .DefaultCellStyle.WrapMode = DataGridViewTriState.True

            .ColumnHeadersVisible = False                                 ' 列ヘッダ表示
            .RowHeadersVisible = False                                    ' 行ヘッダ表示

        End With
    End Sub

    ''' <summary>
    ''' ラジオボタンの変更
    ''' 指定のボタンを有効にする（RAD_INITは初期状態）
    ''' </summary>
    ''' <param name="i_btn">ラジオボタン</param>
    ''' <remarks></remarks>
    Private Sub ChgRadBtn(ByVal i_btn As Integer)
        Select Case i_btn
            Case RAD_INIT
                radBefore.Checked = True
                radBefore.Enabled = False
                radAfter.Checked = False
                radAfter.Enabled = False
                radCor.Checked = False
                radCor.Enabled = False
                Exit Select
            Case RAD_BEFORE
                radBefore.Checked = False
                radBefore.Enabled = True
                radAfter.Checked = True
                radAfter.Enabled = False
                Exit Select
            Case RAD_AFTER
                radBefore.Checked = True
                radBefore.Enabled = False
                radAfter.Checked = False
                radAfter.Enabled = True
                Exit Select
            Case RAD_COR
                radCor.Checked = False
                radCor.Enabled = True
                Exit Select
        End Select

    End Sub

    ''' <summary>
    ''' 輝度計算ボタンクリックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btn_brightnessCheck_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnKidoCheck.Click
        InformLbl("", False, lblInform)
        Dim s1 As String = ""
        Dim s2 As String = ""
        Dim s3 As String = ""
        Dim s4 As String = ""
        If Not m_PicSouce Is Nothing Then
            If m_bRaw = False Then
                ' 縦プロジェクション線（生）の輝度を取得
                m_KidoHAve = GetHeightBrightness(m_PicSouce, m_nPicWidth, m_nPicHeight)

                If Not m_KidoHAve Is Nothing Then
                    m_nKidoMax = GetArrayMax(m_KidoHAve)
                    ' テキストボックスに輝度最大値を表示
                    If m_nKidoMax <> 0 Then
                        txtBasePointKido.Text = CStr(m_nKidoMax)
                    End If
                    btnRawMax.Enabled = True
                    m_bRaw = True
                    s1 = "縦プロジェクション生 "
                End If
            End If

            '縦プロジェクション（移動平均）の輝度を取得
            If txtMoveAveraveSize.Text IsNot Nothing AndAlso txtMoveAveraveSize.Text <> "" Then
                If IsNumeric(txtMoveAveraveSize.Text) Then
                    If GetOddNumber(CInt(txtMoveAveraveSize.Text), 3) <= (m_nPicWidth - 1) \ 2 Then
                        ' 移動平均サイズは奇数のみ
                        m_nMvAveSize = GetOddNumber(CInt(txtMoveAveraveSize.Text), 3)
                        ' 移動平均サイズテキストを更新
                        txtMoveAveraveSize.Text = CStr(m_nMvAveSize)
                        ' 移動平均輝度配列を取得する
                        m_KidoMvAve = GetMoveAvePro(m_KidoHAve, m_nMvAveSize)
                        If m_bAve = False Then
                            ' 移動平均計算後に基点輝度値を手入力した場合は手入力優先
                            If Not m_KidoMvAve Is Nothing Then
                                m_nKidoMax_A = GetArrayMax(m_KidoMvAve)
                                ' 基点輝度を更新する
                                txtBasePointKido.Text = CStr(m_nKidoMax_A)
                            End If
                            btnAveMax.Enabled = True
                            m_bAve = True
                            s2 = "縦プロジェクション移動平均 "
                        End If
                    Else
                        Dim frm As New tClassForm.frmPop_MsgBox(String.Format("移動平均サイズは[{0}]以下の値を入力してください", CStr((m_nPicWidth - 1) \ 2)), "輝度計算ボタン")
                        frm.Show()
                    End If
                End If
            Else
                Dim frm As New tClassForm.frmPop_MsgBox("移動平均サイズを入力してください", "輝度計算ボタン")
                frm.Show()
            End If

            ' 補正値の計算
            If m_bAve = True Then
                If Not txtBasePointKido.Text Is Nothing AndAlso Not txtBasePointKido.Text = "" Then
                    If IsNumeric(txtMoveAveraveSize.Text) Then
                        If Not CInt(txtBasePointKido.Text) > 255 Then       ' 基点輝度最大値は255まで
                            m_KidoHosei = GetKidoHosei(m_KidoMvAve, CInt(txtBasePointKido.Text))
                        Else
                            Dim frm As New tClassForm.frmPop_MsgBox("基点輝度値は0～255の範囲で入力してください", "輝度計算ボタン")
                            frm.Show()
                        End If
                    End If
                    m_bCorCalc = True
                    ' 修正画像を表示していたら、補正前画像に戻す
                    If m_bCorPic = True Then
                        DisplayImage(m_nPicWidth, m_nPicHeight, m_PicSouce)
                        m_bCorPic = False
                    End If
                    ChgRadBtn(RAD_AFTER)
                    ChgRadBtn(RAD_COR)
                    btnHoseitiRead.Enabled = True

                    s3 = "補正値 "
                Else
                    Dim frm As New tClassForm.frmPop_MsgBox("基点輝度値を入力してください", "輝度計算ボタン")
                    frm.Show()
                End If
            End If

            ' 基点輝度値の計算
            If Not m_KidoHosei Is Nothing AndAlso Not m_KidoMvAve Is Nothing Then
                m_KidoBasePoint = GetKidoBasePoint(m_nPicWidth, m_KidoHosei, m_KidoMvAve)
                m_bBasePoint = True
                s4 = "基点輝度値 "
            End If
            ' グリッド更新
            dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
            ' 画面更新
            mcls_ImgDsp.Refresh()


            InformLbl(String.Format("{0}{1}{2}{3}の計算が完了しました", s1, s2, s3, s4), True, lblInform)
        Else
            Dim frm As New tClassForm.frmPop_MsgBox("画像がありません", "輝度計算ボタン")
            frm.Show()
        End If
    End Sub

    ''' <summary>
    ''' 基点輝度値を取得する
    ''' </summary>
    ''' <param name="nWidth">画像横幅</param>
    ''' <param name="nKidoHosei">補正値配列</param>
    ''' <param name="nKidoMvAve">移動平均輝度配列</param>
    ''' <returns>基点輝度値配列</returns>
    ''' <remarks></remarks>
    Private Function GetKidoBasePoint(ByVal nWidth As Integer, ByVal nKidoHosei() As Integer, ByVal nKidoMvAve() As Integer) As Integer()
        Dim nKidoBasePoint(nWidth - 1) As Integer
        Dim iKidoBasePoint As Integer
        Try
            For ii As Integer = 0 To nWidth - 1
                iKidoBasePoint = (nKidoHosei(ii) + nKidoMvAve(ii))
                nKidoBasePoint(ii) = iKidoBasePoint
            Next
        Catch ex As Exception
            nKidoBasePoint = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("基点輝度値計算中に例外が発生しました [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox("基点輝度値計算中に例外が発生しました", "基点輝度値計算中")
            frm.Show()
        End Try

        Return nKidoBasePoint

    End Function

    ''' <summary>
    ''' 輝度補正値を取得する
    ''' </summary>
    ''' <param name="i_mvave">移動平均輝度配列</param>
    ''' <param name="i_bp">基点輝度値</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetKidoHosei(ByVal i_mvave() As Integer, ByVal i_bp As Integer) As Integer()
        Dim i_cor(i_mvave.Length - 1) As Integer    ' 補正値配列
        Dim value As Integer                        ' 補正値

        If Not i_mvave Is Nothing Then
            For i As Integer = 0 To i_mvave.Length - 1
                value = (i_bp - i_mvave(i))
                i_cor(i) = value
            Next
        Else
            i_cor = Nothing
        End If
        Return i_cor
    End Function

    ''' <summary>
    ''' カメラセットキー入力
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtCameraSet_KeyPress(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles txtCameraSet.KeyPress
        '数字とBackspaceのみ入力可
        If (e.KeyChar < "0"c OrElse "9"c < e.KeyChar) AndAlso e.KeyChar <> ControlChars.Back Then
            e.Handled = True
        End If
    End Sub

    ''' <summary>
    ''' 基点輝度値入力
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtBasePointKido_KeyPress(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles txtBasePointKido.KeyPress
        '数字とBackspaceのみ入力可
        If (e.KeyChar < "0"c OrElse "9"c < e.KeyChar) AndAlso e.KeyChar <> ControlChars.Back Then
            e.Handled = True
        End If
        m_bCorCalc = False
    End Sub

    ''' <summary>
    ''' 移動平均サイズ入力
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub txtMoveAveraveSize_KeyPress(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyPressEventArgs) Handles txtMoveAveraveSize.KeyPress
        '数字とBackspaceのみ入力可
        If (e.KeyChar < "0"c OrElse "9"c < e.KeyChar) AndAlso e.KeyChar <> ControlChars.Back Then
            e.Handled = True
        End If
        m_bAve = False
    End Sub

    ''' <summary>
    ''' 出力ボタンをクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btn_export_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnExport.Click
        Dim path As String
        Dim filename As String

        InformLbl("", False, lblInform)
        path = txtCorPath.Text

        If Not txtCameraSet.Text = "" Then  ' カメラセットが入力済みであること
            If m_bCorCalc = True Then       ' 輝度補正計算済みであること

                filename = String.Format("KidoHosei_{0}.csv", txtCameraSet.Text)

                ' TextBoxのPathを優先
                If CreateCsvFile(path, filename, m_KidoHosei) = True Then
                    InformLbl(String.Format("ファイル出力完了しました。" & vbCrLf & "[{0}]", System.IO.Path.GetFullPath(IO.Path.Combine(path, filename))), True, lblInform)
                    m_CorSave = path
                    tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, HOSEI_SAVE, LOCALINI_NAME, m_CorSave)
                End If
            Else
                Dim frm As New tClassForm.frmPop_MsgBox("輝度計算後に補正値出力を行ってください", "出力ボタン")
                frm.Show()
            End If
        Else
            Dim frm As New tClassForm.frmPop_MsgBox("カメラセットを入力してください", "出力ボタン")
            frm.Show()
        End If

    End Sub

    ''' <summary>
    ''' ハードコピーボタンをクリック
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btn_HardCopy_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHardCopy.Click
        InformLbl("", False, lblInform)     ' ラベル通知をクリア

        Dim path As String = modMain.HardCopy(Me)
        Dim frm As New tClassForm.frmPop_MsgBox(String.Format("出力先：[{0}]", System.IO.Path.GetFullPath(path)), "ハードコピー完了")
        frm.Show()
    End Sub

    ''' <summary>
    ''' プロジェクション線（生）CheckedChanged
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub CheckedChanged_HeightPro_Raw(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBoxHProRow.CheckedChanged
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' プロジェクション線（移動平均）CheckedChanged
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub CheckedChanged_HeightPro_MoveAverage_(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBoxHProMvAve.CheckedChanged
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' 補正輝度線 CheckedChanged
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub CheckedChanged_BoxCorrectedKido(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBoxCorrected.CheckedChanged
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' 幅方向選択線 CheckedChanged
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub CheckedChanged_BoxSelectX(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBoxSelect.CheckedChanged
        mcls_ImgDsp.Refresh()
    End Sub

    ''' <summary>
    ''' CSVファイル作成
    ''' </summary>
    ''' <param name="strPath">保存するフォルダ名</param>
    ''' <param name="strTitle">File名</param>
    ''' <returns>True:成功 False:失敗</returns>
    ''' <remarks></remarks>
    Public Function CreateCsvFile(ByVal strPath As String, ByVal strTitle As String, ByVal i_x() As Integer) As Boolean
        Dim bRet As Boolean = True
        Dim enc As System.Text.Encoding = System.Text.Encoding.GetEncoding("Shift_JIS")
        Dim fullpath As String
        Try
            ' フォルダが無ければ作成する
            tMod.CheckFolder(strPath)

            fullpath = IO.Path.Combine(strPath, strTitle)


            Using sr As New System.IO.StreamWriter(fullpath, False, enc)
                Dim s As String
                ' セクション
                s = "[KIDO]"
                sr.WriteLine(s)
                For i As Integer = 0 To i_x.Length - 1
                    s = CStr(i_x(i))
                    sr.WriteLine(s)
                Next

            End Using

        Catch ex As Exception
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("CSVファイルの出力に失敗しました[{0}] [{1}]", strPath, ex.Message), "ドラッグドロップ")
            frm.Show()
            bRet = False
        End Try

        Return bRet

    End Function


    ''' <summary>
    ''' 補正画像作成
    ''' </summary>
    ''' <param name="img">画像補正前のデータ</param>
    ''' <param name="i_cor">補正値配列</param>
    ''' <param name="i_Height">画像の高さ</param>
    ''' <returns>補正画像データ</returns>
    ''' <remarks></remarks>
    Private Function CreateCorrectedPic(ByVal img() As Byte, ByVal i_cor() As Integer, ByVal i_Height As Integer) As Byte()
        Dim tmpImg(img.Length - 1) As Byte
        Dim i_value As Integer

        Try
            Dim i As Integer = 0
            Do Until i = img.Length
                For x As Integer = 0 To i_cor.Length - 1
                    ' 下限値は0とする
                    i_value = Math.Max(0, img(i) + i_cor(x))
                    ' 最大輝度値を超えた場合は255とする
                    tmpImg(i) = CByte(Math.Min(255, i_value))
                    i += 1
                Next
            Loop

        Catch ex As Exception
            tmpImg = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("CreateCorrectedPicで例外が発生しました [{0}]", ex.Message))
        End Try

        Return tmpImg

    End Function

    ''' <summary>
    ''' 補正値画像を作成する（0を基点に補正値を描画した画像）
    ''' </summary>
    ''' <param name="img">画像補正前のデータ</param>
    ''' <param name="i_cor">補正値配列</param>
    ''' <returns>補正値画像データ</returns>
    ''' <remarks></remarks>
    Private Function CreateCorrectionPic(ByVal img() As Byte, ByVal i_cor() As Integer) As Byte()
        Dim tmpImg(img.Length - 1) As Byte
        Dim i_value As Integer
        Try
            Dim i As Integer = 0
            Do Until i = img.Length
                For x As Integer = 0 To i_cor.Length - 1
                    ' 下限値は0とする
                    i_value = Math.Max(0, i_cor(x))
                    ' 最大輝度値を超えた場合は255とする
                    tmpImg(i) = CByte(Math.Min(255, i_value))
                    i += 1
                Next
            Loop
        Catch ex As Exception
            tmpImg = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("CreateCorrectionPicで例外が発生しました [{0}]", ex.Message))
        End Try

        Return tmpImg

    End Function


    ''' <summary>
    ''' 偶数の場合は+１した奇数の値を返す
    ''' </summary>
    ''' <param name="i">値</param>
    ''' <param name="min">下限値</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetOddNumber(ByVal i As Integer, ByVal min As Integer) As Integer
        Dim oddNumber As Integer = i

        If Not CBool(i Mod 2) Then
            oddNumber += 1
        End If

        If min > oddNumber Then
            oddNumber = min
        End If

        Return oddNumber
    End Function

    ''' <summary>
    ''' 画像（縦）の輝度平均を返す
    ''' </summary>
    ''' <param name="m_brightness">輝度配列</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function Average(ByVal m_brightness As Integer()) As Integer
        Dim ave As Integer = 0
        Dim sum As Integer = 0

        '合計
        For a As Integer = 0 To m_brightness.Length - 1
            sum += m_brightness(a)
        Next
        '平均
        ave = CInt(sum / m_brightness.Length)

        Return ave

    End Function

    ''' <summary>
    ''' BitMapの縦軸方向の輝度平均を計算する
    ''' </summary>
    ''' <param name="byPic">入力画像</param>
    ''' <param name="iWidth">画像幅</param>
    ''' <param name="iHeight">画像高さ</param>
    ''' <returns>輝度平均配列</returns>
    ''' <remarks></remarks>
    Private Function GetHeightBrightness(ByVal byPic() As Byte, ByVal iWidth As Integer, ByVal iHeight As Integer) As Integer()
        Dim i_x(iWidth - 1) As Integer         ' X軸方向の配列
        Dim i_y(iHeight - 1) As Integer        ' Y軸方向の配列
        Dim point As Integer
        ' プログレスバー開始
        InformLbl(String.Format(vbCrLf & "輝度平均計算中"), True, lblInform)
        StartProgressBar(0, 0, m_pgb)
        Try
            For x As Integer = 0 To iWidth - 1
                If 0 = x Mod 100 Then
                    ' プログレスバー更新
                    UpdateProgressBar(x, iWidth - 1, m_pgb)
                End If

                ' 縦の輝度を取得
                For y As Integer = 0 To iHeight - 1
                    point = x + y * iWidth
                    i_y(y) = byPic(point)
                Next
                ' 縦の輝度平均を求める
                i_x(x) = Average(i_y)
            Next

        Catch ex As Exception
            i_x = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("GetHeightBrightnessで例外発生しました [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("輝度平均計算中に例外が発生しました"), "輝度平均計算")
            frm.Show()
        End Try

        InformLbl("", False, lblInform)
        FinishProgressBar(m_pgb)
        Return i_x

    End Function

    ''' <summary>
    ''' 縦プロジェクション（移動平均）を取得する
    ''' </summary>
    ''' <param name="kidoH">縦平均輝度データ</param>
    ''' <param name="mvave">移動平均サイズ</param>
    ''' <returns>移動平均配列</returns>
    ''' <remarks></remarks>
    Private Function GetMoveAvePro(ByVal kidoH() As Integer, ByVal mvave As Integer) As Integer()
        Dim mv As Integer = mvave \ 2               ' 現在地から前後に移動する値
        Dim i_ave(kidoH.Length - 1) As Integer      ' X軸方向移動平均の配列
        Dim total As Integer

        Try
            ' 移動平均の計算処理
            For i As Integer = 0 To kidoH.Length - 1     ' i:現在位置
                total = 0
                For ii As Integer = i - mv To i + mv     ' ii:現在地から前後移動位置
                    If 0 <= ii Then
                        If kidoH.Length - 1 >= ii Then
                            total += kidoH(ii)
                        Else
                            ' 右側へ範囲外の場合、直前の移動平均を加算
                            If i = kidoH.Length - 1 Then
                                total += kidoH(i)
                            Else
                                total += kidoH(i + 1)
                            End If
                        End If
                    Else
                        ' 左側へ範囲外の場合
                        If i = 0 Then   ' 左端は、基準位置の輝度を加算
                            total += kidoH(i)
                        Else
                            total += kidoH(i - 1)  '直前の輝度を加算 
                        End If
                    End If
                Next
                ' 縦移動平均を求める
                i_ave(i) = CInt(total / m_nMvAveSize)
            Next
        Catch ex As Exception
            i_ave = Nothing
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("GetMoveAveProで例外発生しました [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("移動平均計算中に例外が発生しました"), "移動平均計算")
            frm.Show()
        End Try

        Return i_ave

    End Function

    ''' <summary>
    ''' 配列の最大値を求める
    ''' </summary>
    ''' <param name="i_x">配列</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function GetArrayMax(ByVal i_x() As Integer) As Integer
        Dim max As Integer
        Dim ar As New ArrayList(i_x)
        ar.Sort()
        max = CInt(ar(ar.Count() - 1))
        ar.Clear()

        Return max
    End Function

    ''' <summary>
    ''' INIファイル読込
    ''' KizuTask.iniに設定がなければ、Default設定値で出力します
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReadIniFile()
        Dim strCollor As String
        Dim strMvAveSize As String
        Const COLLOR_RAW As String = "COLLOR_RAW"     '// 縦プロジェクション生線キー
        Const COLLOR_AVE As String = "COLLOR_AVE"     '// 縦プロジェクション移動平均線キー
        Const COLLOR_COR As String = "COLLOR_COR"     '// 補正輝度線キー
        Const COLLOR_SEL As String = "COLLOR_SEL"     '// 幅位置選択線キー
        Const RED As String = "255,0,0"
        Const BLUE As String = "0,0,255"
        Const GREEN As String = "0,128,0"
        Const PINK As String = "255,174,201"

        ' 補正値保管先取得
        m_CorSave = tMod.ReadIniStr(MY_SECTION, HOSEI_SAVE, LOCALINI_NAME, "")
        If m_CorSave = "" Then
            m_CorSave = PATH_COR_DEF
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, HOSEI_SAVE, LOCALINI_NAME, m_CorSave)
        End If
        txtCorPath.Text = System.IO.Path.GetFullPath(m_CorSave)

        ' 縦プロジェクション生のLine color取得
        strCollor = tMod.ReadIniStr(MY_SECTION, COLLOR_RAW, LOCALINI_NAME, "")
        If StringToColor(strCollor, mclr_LineRaw) Then
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("縦プロジェクション生ラインカラー正常読込[{0}]", mclr_LineRaw))
        Else
            strCollor = RED
            StringToColor(strCollor, mclr_LineRaw)
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, COLLOR_RAW, LOCALINI_NAME, strCollor)
        End If

        ' 縦プロジェクション移動平均のLine color取得
        strCollor = tMod.ReadIniStr(MY_SECTION, COLLOR_AVE, LOCALINI_NAME, "")
        If StringToColor(strCollor, mclr_LineAve) Then
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("縦プロジェクション移動平均ラインカラー正常読込[{0}]", mclr_LineAve))
        Else
            strCollor = BLUE
            StringToColor(strCollor, mclr_LineAve)
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, COLLOR_AVE, LOCALINI_NAME, strCollor)
        End If

        ' 補正輝度線のLine color取得
        strCollor = tMod.ReadIniStr(MY_SECTION, COLLOR_COR, LOCALINI_NAME, "")
        If StringToColor(strCollor, mclr_LineCor) Then
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("補正輝度線ラインカラー正常読込 [{0}]", mclr_LineCor))
        Else
            strCollor = GREEN
            StringToColor(strCollor, mclr_LineCor)
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, COLLOR_COR, LOCALINI_NAME, strCollor)
        End If

        ' 幅方向位置のLine color取得
        strCollor = tMod.ReadIniStr(MY_SECTION, COLLOR_SEL, LOCALINI_NAME, "")
        If StringToColor(strCollor, mclr_LineSel) Then
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("幅方向選択線ラインカラー正常読込 [{0}]", mclr_LineSel))
        Else
            strCollor = PINK
            StringToColor(strCollor, mclr_LineSel)
            tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, COLLOR_SEL, LOCALINI_NAME, strCollor)
        End If

        ' 前回終了時の引継ぎデータを取得(KizuLocal.ini)
        strMvAveSize = tMod.ReadIniStr(MY_SECTION, KEY_MV_AVE, LOCALINI_NAME, "")
        If Not strMvAveSize = "" Then
            If IsNumeric(strMvAveSize) Then
                If Not CInt(strMvAveSize) = 0 Then
                    m_nMvAveSize = CInt(strMvAveSize)
                    txtMoveAveraveSize.Text = CStr(m_nMvAveSize)
                End If
            End If
        End If

    End Sub

    ''' <summary>
    ''' R,G,B文字列から色変換（R,G,Bは1～255）
    ''' </summary>
    ''' <param name="str">文字列(例:255,235,0)</param>
    ''' <returns>True:変換成功、False:変換失敗</returns>
    ''' <remarks></remarks>
    Private Function StringToColor(ByVal str As String, ByRef clr As Color) As Boolean
        Dim bolRtn As Boolean = False                                                       ' 実行結果
        Dim nClr(2) As Integer

        Try
            Dim sInt As String() = str.Split(New String() {","}, StringSplitOptions.None)   ' 文字列分解

            If 3 = sInt.Length Then

                If Not (IsNumeric(sInt(0)) And IsNumeric(sInt(1)) And IsNumeric(sInt(2))) Then
                    Exit Try                                                                ' 数値でない場合は抜ける
                End If

                For nCnt As Integer = 0 To 2
                    nClr(nCnt) = Integer.Parse(sInt(nCnt))
                    If Not (0 <= nClr(nCnt) And 255 >= nClr(nCnt)) Then                     ' 範囲チェック
                        Exit Try
                    End If
                Next

                clr = Color.FromArgb(255, nClr(0), nClr(1), nClr(2))                        ' 色変換 

                bolRtn = True
            End If

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("色変換異常 value=({0}) [{1}]", str, ex.Message))
        End Try
        Return bolRtn
    End Function

    ''' <summary>
    ''' 画面で設定したDataを保存する
    ''' 保存するData:移動平均サイズ
    ''' </summary>
    ''' <returns>True:成功、False:失敗</returns>
    ''' <remarks></remarks>
    Private Function SaveData() As Boolean
        Dim bRet As Boolean = True
        Try
            If Not txtMoveAveraveSize.Text = "" Then
                tClassLibrary.tMod.tMod_IniFile.WriteIni(MY_SECTION, KEY_MV_AVE, LOCALINI_NAME, txtMoveAveraveSize.Text)
            End If
        Catch ex As Exception
            bRet = False
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SaveDataで例外が発生しました [{0}]", ex.Message))
        End Try
        Return bRet
    End Function

    ''' <summary>
    ''' プログレスバーを開始処理
    ''' </summary>
    ''' <param name="min">Minimum</param>
    ''' <param name="v">Value</param>
    ''' <param name="pgb">ProgressBar</param>
    ''' <remarks></remarks>
    Private Sub StartProgressBar(ByVal min As Integer, ByVal v As Integer, ByVal pgb As ProgressBar)
        pgb.Minimum = min
        pgb.Value = v
        pgb.Visible = True
        Application.DoEvents()
    End Sub

    ''' <summary>
    ''' プログレスバーを更新する
    ''' </summary>
    ''' <param name="v">Value</param>
    ''' <param name="max">Maximum</param>
    ''' <param name="pgb">ProgressBar</param>
    ''' <remarks></remarks>
    Private Sub UpdateProgressBar(ByVal v As Integer, ByVal max As Integer, ByVal pgb As ProgressBar)
        pgb.Value = v
        pgb.Maximum = max
        pgb.Refresh()
        Application.DoEvents()
    End Sub

    ''' <summary>
    ''' プログレスバーを終了処理
    ''' ラベルとプログレスバーを非表示にする
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub FinishProgressBar(ByVal pgb As ProgressBar)
        pgb.Visible = False
    End Sub

    ''' <summary>
    ''' BitMapイメージを表示する
    ''' </summary>
    ''' <param name="iWidth">横幅</param>
    ''' <param name="iHeight">高さ</param>
    ''' <param name="img">画像データ</param>
    ''' <remarks></remarks>
    Private Sub DisplayImage(ByVal iWidth As Integer, ByVal iHeight As Integer, ByVal img() As Byte)
        Dim tmpImg() As Image = Nothing
        Try
            ' 画像の表示
            With mcls_ImgDsp
                tmpImg = .GetImage()
                If tmpImg IsNot Nothing AndAlso tmpImg(0) IsNot Nothing Then
                    tmpImg(0).Dispose()
                    tmpImg(0) = Nothing
                End If
                .tProp_AriaInf_Aria = m_PicBox.ClientSize
                .tProp_Paint_IsPaint = True
                .tProp_Paint_IsDefImgDspn = False
                .tProp_AriaInf_IsCenter = False

                .SetImgInf(iWidth, iHeight, 1, 1)
                .SetImage(iWidth, iHeight, img)
                .Calculation()

                .Refresh()
            End With

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DisplayImageで例外が発生しました [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("画面表示中に例外が発生しました"), "画面表示")
            frm.Show()
        End Try
    End Sub

    ''' <summary>
    ''' スクロールバーのValue Change
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub scrollValueChange(ByVal sender As System.Object, ByVal e As System.Boolean) Handles HScrollBM.ValueChanged, VScrollBM.ValueChanged
        Dim offset_x As Integer = HScrollBM.Value
        Dim offset_y As Integer = VScrollBM.Value

        mcls_ImgDsp.tProp_AriaInf_Offset = New Point(-1 * offset_x, -1 * offset_y)
        mcls_ImgDsp.Refresh()

    End Sub

    ''' <summary>
    ''' PictureBoxのリサイズイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub PicBoxKidoHosei_Resize(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles pbxBitMap.Resize, pnlBitMap.Resize
        If Not mcls_ImgDsp Is Nothing AndAlso Not m_PicBox Is Nothing Then
            If mcls_ImgDsp.tProp_ImgKind_StrechType = tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                ' ×1モード
                HScrollBM.LargeChange = m_nPicWidth
                VScrollBM.LargeChange = m_nPicHeight
                mcls_ImgDsp.tProp_AriaInf_Offset = New Point(0, 0)
            Else
                ' 全体モード
                pbxBitMap.Height = pbxBitMap.Parent.ClientSize.Height - VScrollBM.Width
                pbxBitMap.Width = pbxBitMap.Parent.ClientSize.Width - HScrollBM.Height
                HScrollBM.LargeChange = pbxBitMap.Size.Width
                VScrollBM.LargeChange = pbxBitMap.Size.Height
            End If

            ' x軸方向の右端とスクロールの終点を合わせる
            If HScrollBM.Value > HScrollBM.Maximum - HScrollBM.LargeChange Then
                HScrollBM.Value = HScrollBM.Maximum - HScrollBM.LargeChange + 1
            End If

            ' y軸方向の最下辺とスクロールの終点を合わせる
            If VScrollBM.Value > VScrollBM.Maximum - VScrollBM.LargeChange Then
                VScrollBM.Value = VScrollBM.Maximum - VScrollBM.LargeChange + 1
            End If

            mcls_ImgDsp.tProp_AriaInf_Aria = m_PicBox.ClientSize
            mcls_ImgDsp.Refresh()
        End If

    End Sub

    ''' <summary>
    ''' 通知ラベルへ表示、非表示を行う
    ''' </summary>
    ''' <param name="s">通知内容</param>
    ''' <param name="bVisible">true表示/false非表示</param>
    ''' <param name="lbl">ラベル</param>
    ''' <remarks></remarks>
    Private Sub InformLbl(ByVal s As String, ByVal bVisible As Boolean, ByVal lbl As Label)
        lbl.Text = s
        lbl.Visible = bVisible
        lbl.Refresh()
    End Sub

    ''' <summary>
    ''' 表示画像保存ボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnPicSave_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPicSave.Click
        InformLbl("", False, lblInform)

        Dim header(tClass.tClass_BmpJpeg.GetBitmapHeaderSize_Gray) As Byte
        Dim path As String
        Dim filename As String

        If Not m_PicSouce Is Nothing Then
            If radAfter.Checked = True Then
                filename = "輝度補正後_" & txtBasePointKido.Text & ".bmp"
                path = IO.Path.Combine(PATH_PIC_DEF, filename)
                tMod.CheckFolder(PATH_PIC_DEF)
                tClass.tClass_BmpJpeg.CreateBitmapHeader_Gray(m_nPicWidth, m_nPicHeight, header, False)
                tClass.tClass_BmpJpeg.BmpFileSave(path, header, m_PicCorrected)
            Else
                filename = "輝度補正前.bmp"
                path = IO.Path.Combine(PATH_PIC_DEF, filename)
                tMod.CheckFolder(PATH_PIC_DEF)
                tClass.tClass_BmpJpeg.CreateBitmapHeader_Gray(m_nPicWidth, m_nPicHeight, header, False)
                tClass.tClass_BmpJpeg.BmpFileSave(path, header, m_PicSouce)
            End If

            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("出力先：[{0}]", System.IO.Path.GetFullPath(path)), "表示画像保存完了")
            frm.Show()

        End If
    End Sub

    ''' <summary>
    ''' 補正前の画像表示ボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radBefore_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radBefore.CheckedChanged
        InformLbl("", False, lblInform)
        ' 補正前の画像を表示する
        If radBefore.Checked = True Then
            radBefore.Enabled = False
            If Not m_PicSouce Is Nothing Then
                DisplayImage(m_nPicWidth, m_nPicHeight, m_PicSouce)
                m_bCorPic = False
            End If
        Else
            radBefore.Enabled = True
        End If
    End Sub

    ''' <summary>
    ''' 補正後の画像表示ボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radAfter_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radAfter.CheckedChanged
        InformLbl("", False, lblInform)
        ' 補正した画像を表示する
        If radAfter.Checked = True Then
            radAfter.Enabled = False
            If Not m_PicSouce Is Nothing Then
                m_PicCorrected = CreateCorrectedPic(m_PicSouce, m_KidoHosei, m_nPicHeight)
                If Not m_PicCorrected Is Nothing Then
                    DisplayImage(m_nPicWidth, m_nPicHeight, m_PicCorrected)
                    m_bCorPic = True
                End If
            End If
        Else
            radAfter.Enabled = True
        End If
    End Sub

    ''' <summary>
    ''' 補正値の画像表示ボタン
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radCor_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles radCor.CheckedChanged
        If radCor.Checked = True Then
            radCor.Enabled = False
            If Not m_PicSouce Is Nothing Then
                m_PicCorrected = CreateCorrectionPic(m_PicSouce, m_KidoHosei)
                If Not m_PicCorrected Is Nothing Then
                    DisplayImage(m_nPicWidth, m_nPicHeight, m_PicCorrected)
                    'm_bCorPic = True
                End If
            End If
        Else
            radCor.Enabled = True
        End If
    End Sub

    ''' <summary>
    ''' DGVを更新
    ''' </summary>
    ''' <param name="i_R">縦平均輝度配列</param>
    ''' <param name="i_Ave">縦移動平均配列</param>
    ''' <param name="i_Cor">補正値配列</param>
    ''' <param name="i_BasePt">基点輝度値配列</param>
    ''' <param name="dgv"></param>
    ''' <remarks></remarks>
    Private Sub dgvUpdate(ByVal i_R As Integer(), ByVal i_Ave As Integer(), ByVal i_Cor As Integer(), ByVal i_BasePt As Integer(), ByVal dgv As tClassLibrary.tCnt.tCnt_DataGridView)
        Dim clm As Integer              ' 表示する列数
        Dim titleW As Integer = 135     ' Title列の幅（Base）
        Dim dataW As Integer = 27       ' Data列の幅（Base）
        Try
            clm = ((dgv.Width - titleW) \ dataW) + 1

            ' スクロールバーの設定
            If Not i_R Is Nothing Then
                HScrollDgv.Minimum = 0
                HScrollDgv.Maximum = i_R.Length
                HScrollDgv.LargeChange = clm
            End If

            ' 行の追加
            If dgv.RowCount = 0 Then
                With dgv
                    ' タイトル行を追加

                    dgvColSetText(dgv, "", titleW, True, True, _
                    DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
                    .Columns(0).Frozen = True       ' タイトル列はスクロール固定
                    .Rows.Add("No.")
                    .Rows.Add("縦Proj（生）")
                    .Rows.Add("縦Proj（移動平均）")
                    .Rows.Add("補正値")
                    .Rows.Add("基点輝度値")
                    dgvKido.Rows(0).DefaultCellStyle.Font = New System.Drawing.Font("ＭＳ 明朝", 7.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                    .Item(0, 0).Style.Font = New Font("ＭＳ 明朝", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                End With
            End If

            dgvFixColumn(dgv, titleW, dataW, clm)

            ' データ終点がグリッドの右端になるようにする
            If HScrollDgv.Value > HScrollDgv.Maximum - HScrollDgv.LargeChange Then
                HScrollDgv.Value = HScrollDgv.Maximum - HScrollDgv.LargeChange + 1
            End If

            ' Scrollの位置
            Dim start As Integer = HScrollDgv.Value

            If start >= 0 Then

                ' 生画像の輝度を表示
                If m_bRaw = True Then
                    ' No.を表示
                    For ii As Integer = start To start + clm - 2  ' Title分もマイナス

                        dgvKido.Item(ii + 1 - start, 0).Value = ii

                    Next

                    DisplayBrightness(m_nPicWidth, i_R, ROW_KIDO_RAW, start, clm)
                End If
                ' 縦プロジェクション（移動平均)を表示
                If m_bAve = True Then
                    DisplayBrightness(m_nPicWidth, i_Ave, ROW_KIDO_AVE, start, clm)
                End If
                ' 補正値
                If m_bCorCalc = True Then
                    DisplayBrightness(m_nPicWidth, i_Cor, ROW_KIDO_COR, start, clm)
                End If
                ' 基点輝度値
                If m_bBasePoint = True Then
                    DisplayBrightness(m_nPicWidth, i_BasePt, ROW_KIDO_BP, start, clm)
                End If

                ' 生画像の輝度と移動平均最大値のCellを赤色にする
                ChgCellCollorMAX(m_nKidoMax, ROW_KIDO_RAW, i_R, start, clm)
                ChgCellCollorMAX(m_nKidoMax_A, ROW_KIDO_AVE, i_Ave, start, clm)

            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("dgvUpdateで例外が発生しました[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' グリッドの表示に合わせて列の追加削除を行う
    ''' </summary>
    ''' <param name="dgv">DataGridView</param>
    ''' <param name="i_TW">Title幅のBase</param>
    ''' <param name="i_DW">Data幅のBase</param>
    ''' <param name="i_Col">表示列数</param>
    ''' <remarks></remarks>
    Private Sub dgvFixColumn(ByRef dgv As tClassLibrary.tCnt.tCnt_DataGridView, ByVal i_TW As Integer, ByVal i_DW As Integer, ByVal i_Col As Integer)
        ' グリッドの幅に合わせて列の追加/削除
        If dgv.ColumnCount < i_Col Then
            ' DataGridViewのサイズより画像サイズが大きい場合は列を追加する
            For ii As Integer = 1 To i_Col - dgv.ColumnCount
                dgvColSetText(dgv, "", i_DW, True, True, _
                DataGridViewColumnSortMode.NotSortable, DataGridViewContentAlignment.MiddleLeft, DataGridViewContentAlignment.TopCenter)
            Next

        ElseIf dgv.ColumnCount > i_Col Then
            ' DataGridViewのサイズより画像サイズが小さい場合は列を削除する
            For ii As Integer = 1 To dgv.ColumnCount - i_Col
                dgv.Columns.RemoveAt(1)
            Next
        End If

        ' 列幅初期化
        For i As Integer = 1 To i_Col - 1
            dgv.Columns(i).Width = i_DW
        Next

        ' DGVの幅にFixするまでData部分の幅を順番に+１していく
        If dgv.Width - 3 > i_TW + i_DW * (i_Col - 1) Then
            Dim i As Integer = 1    ' 列位置
            Dim j As Integer = 0    ' 幅調整数カウント
            Do Until dgv.Width - 3 = i_TW + i_DW * (i_Col - 1) + j  ' グリッドの幅から-3した値に列幅がFixしたのでそこに合わせる
                dgv.Columns(i).Width = dgv.Columns(i).Width + 1
                If Not i = i_Col - 1 Then
                    i += 1
                Else
                    i = 1
                End If
                j += 1
            Loop
        End If
    End Sub

    ''' <summary>
    ''' DataGridViewへの表示
    ''' </summary>
    ''' <param name="iWidth">横幅</param>
    ''' <param name="i_x">データ配列</param>
    ''' <param name="i_row">行番号</param>
    ''' <returns>True:成功 False:失敗</returns>
    ''' <remarks></remarks>
    Private Function DisplayBrightness(ByVal iWidth As Integer, ByVal i_x() As Integer, ByVal i_row As Integer, ByVal iStart As Integer, ByVal iclm As Integer) As Boolean
        Dim bRet As Boolean = True      '// 実行結果
        Try
            For ii As Integer = iStart To iStart + iclm - 2  ' Title分もマイナス
                dgvKido.Item(ii + 1 - iStart, i_row).Value = i_x(ii)
            Next
        Catch ex As Exception
            bRet = False
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DisplayBrightnessで例外が発生しました [{0}]", ex.Message))
            Dim frm As New tClassForm.frmPop_MsgBox(String.Format("データ表示中に例外が発生しました"), "データ表示")
            frm.Show()
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' 最大値のCellを赤色にする
    ''' </summary>
    ''' <param name="i_Max">最大値</param>
    ''' <param name="i_Row">行番号</param>
    ''' <param name="i_x">配列</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function ChgCellCollorMAX(ByVal i_Max As Integer, ByVal i_Row As Integer, ByVal i_x() As Integer, ByVal iStart As Integer, ByVal iclm As Integer) As Boolean
        Try
            If i_Max <> 0 Then
                For ii As Integer = iStart To iStart + iclm - 2  ' Title分もマイナス
                    With dgvKido
                        If i_x(ii) = i_Max Then
                            .Item(ii + 1 - iStart, i_Row).Style.BackColor = Color.Red
                        Else
                            .Item(ii + 1 - iStart, i_Row).Style.BackColor() = .Item(0, i_Row).Style.BackColor()
                        End If
                    End With
                Next
            End If
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("ChgCellCollorMAXで例外が発生しました[{0}]", ex.Message))
        End Try

    End Function


    ''' <summary>
    ''' スクロールバーValueChangedイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub scrDgvValueChaned(ByVal sender As System.Object, ByVal e As System.Boolean) Handles HScrollDgv.ValueChanged
        dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
    End Sub

    ''' <summary>
    ''' DGVリサイズイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKido_Resize(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles dgvKido.Resize
        If Not dgvKido.Width = 0 Then
            dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
        End If
    End Sub

    ''' <summary>
    ''' DGVでのキーダウンイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKido_KeyDown(ByVal sender As System.Object, ByVal e As System.Windows.Forms.KeyEventArgs) Handles dgvKido.KeyDown
        Dim x As Integer = dgvKido.CurrentCell.ColumnIndex
        Dim y As Integer = dgvKido.CurrentCell.ColumnIndex

        Select Case e.KeyCode
            Case Keys.Left
                ' データ左端の列で左カーソル操作した場合は-1スクロールする
                If dgvKido.CurrentCell.ColumnIndex = 1 Then
                    If Not dgvKido.Item(1, 0).Value Is Nothing Then
                        If Not CInt(dgvKido.Item(1, 0).Value) = 0 Then
                            e.Handled = True
                            HScrollDgv.Value = HScrollDgv.Value - 1
                            dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
                        End If
                    End If

                End If
            Case Keys.Right
                ' データ左端の列で右カーソル操作した場合は＋1スクロールする
                If dgvKido.CurrentCell.ColumnIndex = dgvKido.Columns.Count - 1 Then
                    If Not dgvKido.Item(dgvKido.Columns.Count - 1, 0).Value Is Nothing Then
                        If Not dgvKido.Item(dgvKido.Columns.Count - 1, 0).Value.ToString = CStr(m_nPicWidth - 1) Then
                            e.Handled = True
                            HScrollDgv.Value = HScrollDgv.Value + 1
                            dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
                        End If
                    End If

                End If
        End Select

    End Sub

    ''' <summary>
    ''' DGVでのマウスクリックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvKido_CellClick(ByVal sender As System.Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs) Handles dgvKido.CellClick
        Dim x As Integer = dgvKido.CurrentCell.ColumnIndex

        If x > 0 Then
            m_nSelectX = CInt(dgvKido.Item(x, 0).Value)
            If mcls_ImgDsp.tProp_ImgKind_StrechType <> tClass_ImageDsp_Calc.EM_IMG_STRECH.emStrechBoth Then
                ' 全体モード
                Dim nSel As Integer = m_nSelectX - CInt(pbxBitMap.Size.Width / 2)
                If nSel < 0 Then
                    nSel = 0
                End If
                HScrollBM.Value = nSel
            End If

            mcls_ImgDsp.Refresh()
        End If

    End Sub

    ''' <summary>
    ''' 縦Projection生の最大値へ移動する
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnRawMax_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnRawMax.Click
        Dim start As Integer    ' DGVの開始位置
        Dim max As Integer      ' 輝度最大値
        max = GetArrayMax(m_KidoHAve)
        start = GetMaxValueClm(max, m_KidoHAve)
        HScrollDgv.Value = start

        dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
    End Sub

    ''' <summary>
    ''' 縦Projection移動平均の最大値へ移動する
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnAveMax_Click(ByVal sender As System.Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles btnAveMax.MouseClick
        Dim start As Integer    ' DGVの開始位置
        Dim max As Integer      ' 輝度最大値
        max = GetArrayMax(m_KidoMvAve)
        start = GetMaxValueClm(max, m_KidoMvAve)
        HScrollDgv.Value = start
        dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)

    End Sub

    ''' <summary>
    ''' 最大値の配列Indexを返す
    ''' 最大値が複数ある場合は先頭の最大値の配列Indexとなります
    ''' </summary>
    ''' <param name="i_max">輝度最大値</param>
    ''' <param name="i_x">輝度配列</param>
    ''' <returns>輝度配列Index</returns>
    ''' <remarks></remarks>
    Private Function GetMaxValueClm(ByVal i_max As Integer, ByVal i_x() As Integer) As Integer
        Dim i_clm As Integer = 0    ' 輝度最大値のIndex

        For i As Integer = 0 To i_x.Length - 1
            If i_x(i) = i_max Then
                i_clm = i
                Exit For
            End If
        Next

        Return i_clm

    End Function

    ''' <summary>
    ''' 補正値読込ボタンクリックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnHoseitiRead_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnHoseitiRead.Click
        InformLbl("", False, lblInform)
        Dim nHoseiti() As Integer   '補正値
        Dim ofd As New OpenFileDialog           'オープンダイアログ

        ofd.InitialDirectory = m_CorSave        '補正値保存先をInitial directoryにする
        ofd.Filter = "csv files (*.csv)|*.csv|All files (*.*)|*.*"
        ofd.FilterIndex = 1
        ofd.RestoreDirectory = True

        If m_nPicWidth = 0 Then Exit Sub ' 横幅サイズが0は処理できないので何もしない
        If ofd.ShowDialog() = DialogResult.OK Then
            Dim strPath As String = ofd.FileName
            nHoseiti = ReadHoseiti(m_nPicWidth, strPath)  'CSVファイルから読み込んだ補正値を取得する
            If nHoseiti Is Nothing Then Exit Sub '補正値取得に失敗しているので終了
            m_KidoHosei = nHoseiti      'メンバ更新

            ' 基点輝度値の計算
            If Not m_KidoHosei Is Nothing AndAlso Not m_KidoMvAve Is Nothing Then
                m_KidoBasePoint = GetKidoBasePoint(m_nPicWidth, m_KidoHosei, m_KidoMvAve)
                m_bBasePoint = True
            End If

            ' 補正した画像を表示する
            If Not m_PicSouce Is Nothing Then
                m_PicCorrected = CreateCorrectedPic(m_PicSouce, m_KidoHosei, m_nPicHeight)      '補正画像取得
                If Not m_PicCorrected Is Nothing Then
                    DisplayImage(m_nPicWidth, m_nPicHeight, m_PicCorrected) '画像表示
                    dgvUpdate(m_KidoHAve, m_KidoMvAve, m_KidoHosei, m_KidoBasePoint, dgvKido)
                    m_bCorPic = True '補正画像表示Flag更新
                    ChgRadBtn(RAD_BEFORE)       'ボタンは補正前画像に戻せるようにしとく
                    InformLbl("補正値読込処理が完了しました", True, lblInform)       '画像表示完了の通知
                End If
            End If
        End If

        ofd.Dispose()

    End Sub

    ''' <summary>
    ''' 補正値ファイル（CSV）の読込
    ''' </summary>
    ''' <param name="nWidth">画像横幅</param>
    ''' <returns>補正値配列</returns>
    ''' <remarks></remarks>
    Private Function ReadHoseiti(ByVal nWidth As Integer, ByVal strPath As String) As Integer()
        Dim nHoseiti(nWidth - 1) As Integer     '補正値配列

        Using cls_Stram As New tClass.tClass_FileStream(strPath)
            Try
                'オープン
                cls_Stram.Open()
                Dim aryHoseiti As New ArrayList
                aryHoseiti = cls_Stram.ReadAll      'ファイルの全読込データを取得

                '読込データのサイズチェック。元画像の横幅と合っていないと不正とみなす。セクション名1行分考慮。
                If aryHoseiti.Count <> m_nPicWidth + 1 Then
                    mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("読込データと画像データ幅サイズと一致しない。画像データ幅：[{0}] 読込データ：[{1}]", m_nPicWidth, aryHoseiti.Count - 1))
                    Dim frm As New tClassForm.frmPop_MsgBox(String.Format("読込データと画像データ幅サイズが一致しませんでした。" & vbCrLf & " 画像データ幅：[{0}]" & vbCrLf & " 読込データ：[{1}]", m_nPicWidth, aryHoseiti.Count - 1), "補正値読込")
                    frm.Show()
                    cls_Stram.Close()
                    cls_Stram.Dispose()
                    Return Nothing
                End If

                '読込データの補正値データを取り出す
                For ii As Integer = 0 To aryHoseiti.Count - 2  '先頭行はセクション名なので除外
                    nHoseiti(ii) = CInt(aryHoseiti(ii + 1))
                Next

                cls_Stram.Close()
                cls_Stram.Dispose()

            Catch ex As Exception
                nHoseiti = Nothing
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("補正値読み取り中に例外が発生しました [{0}]", ex.Message))
                Dim frm As New tClassForm.frmPop_MsgBox(String.Format("補正値読込中に例外が発生しました。読込ファイルを開いている場合はファイルを閉じてから再度実行してください。"), "補正値読込")
                frm.Show()
            End Try
        End Using

        Return nHoseiti

    End Function

End Class