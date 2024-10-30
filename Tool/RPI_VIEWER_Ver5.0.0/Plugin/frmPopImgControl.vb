Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin


Public Class frmPopImgControl
    Implements RpiPlugin.IPluginActivatable, RpiPlugin.IEvToolStrip, RpiPlugin.IFilterBitmap

    Public Sub New()

        ' この呼び出しはデザイナーで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
    End Sub

    Private m_doc As PropImageControl

    Dim m_parentForm As Form
    Public Sub New(parent As Form)
        m_parentForm = parent
        ' この呼び出しはデザイナーで必要です。

        m_doc = GlobalProperties.GetPropertyMember(Of PropImageControl)(parent, "imagecontrol")
        AddHandler m_doc.OnValueChanged, AddressOf OnValueChanged

        InitializeComponent()
        ' InitializeComponent() 呼び出しの後で初期化を追加します。

    End Sub


    Private m_bStopEmit As Boolean = False
    Private m_prev As PropImageControl
    Private Sub OnValueChanged(name As String, doc As PeropertiesBase)
        m_bStopEmit = True
        Dim bNeedRecreate As Boolean = False
        If m_prev IsNot Nothing Then

            If m_doc.bShdMode And Not m_prev.bShdMode Then
                chkShd.Checked = True
                chkShd.BackColor = Color.Orange
                chkShd.Text = "有り"
                bNeedRecreate = True
            ElseIf Not m_doc.bShdMode And m_prev.bShdMode Then
                chkShd.Checked = False
                chkShd.BackColor = SystemColors.Control
                chkShd.Text = "無し"
                bNeedRecreate = True
            End If

            If m_doc.bBinariMode And Not m_prev.bBinariMode Then
                chkBinari.Checked = True
                bNeedRecreate = True
            ElseIf Not m_doc.bBinariMode And m_prev.bBinariMode Then
                chkBinari.Checked = False
                bNeedRecreate = True
            End If

            If m_doc.bContMode <> m_prev.bContMode Then
                bNeedRecreate = True
            End If
            If m_doc.bGammaMode <> m_prev.bGammaMode Then
                bNeedRecreate = True
            End If
            If m_doc.bSigmoidMode <> m_prev.bSigmoidMode Then
                bNeedRecreate = True
            End If

            If m_doc.nShdX <> m_prev.nShdX Or _
               m_doc.nShdY <> m_prev.nShdY Or _
               m_doc.nShdTarget <> m_prev.nShdTarget Then
                bNeedRecreate = m_doc.bShdMode
            End If



            If m_doc.nContrastX <> m_prev.nContrastX Or _
                m_doc.nBright <> m_prev.nBright Then
                lblContrast.Text = CStr(m_doc.nContrastX)
                lblBright.Text = CStr(m_doc.nBright)
                SetDsp(1)
                bNeedRecreate = m_doc.bContMode
            End If

            If m_doc.nGammaX <> m_prev.nGammaX Then
                '設定値格納
                lblGamma.Text = CStr(m_doc.nGammaX)
                lblGammaRitu.Text = CStr(m_doc.dGammaRitu)
                '画面データ描画
                SetDsp(2)
                bNeedRecreate = m_doc.bGammaMode
            End If

            If m_doc.nSigmoidX <> m_prev.nSigmoidX Or _
               m_doc.nSigmoidKei <> m_prev.nSigmoidKei Then
                lblSigmoidX.Text = CStr(m_doc.nSigmoidX)
                lblSigmoidKei.Text = CStr(m_doc.nSigmoidKei)
                SetDsp(3)
                bNeedRecreate = m_doc.bSigmoidMode
            End If

            If m_doc.nBinari01 <> m_prev.nBinari01 Or _
               m_doc.nBinari02 <> m_prev.nBinari02 Then

                lblBinari01.Text = CStr(m_doc.nBinari01)
                lblBinari02.Text = CStr(m_doc.nBinari02)
                '画面データ描画
                SetDsp(4)

                bNeedRecreate = m_doc.bBinariMode
            End If




            If bNeedRecreate Then
                ReCreateImage()
            Else
                If mcls_RpiView IsNot Nothing Then
                    '                    mcls_RpiView.RePaint()
                End If
            End If
        End If

        radContrast.Checked = m_doc.m_Filter = PropImageControl.FilterSelection.CONTRAST
        radGamma.Checked = m_doc.m_Filter = PropImageControl.FilterSelection.GAMMA
        radSigmoid.Checked = m_doc.m_Filter = PropImageControl.FilterSelection.SIGMOID
        radNon.Checked = m_doc.m_Filter = PropImageControl.FilterSelection.NONE
        If m_doc.m_Filter = PropImageControl.FilterSelection.NONE Then
            radNon.BackColor = Color.Orange
        Else
            radNon.BackColor = SystemColors.Control
        End If

        m_prev = New PropImageControl(m_doc)
        m_bStopEmit = False
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数
    Private Const mc_NUM As Integer = 4                                 ' グラフグループ数

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 構造体
    ''' <summary>
    ''' グラフ表示テーブル
    ''' </summary>
    Private Structure TYP_TMP_GRAPH
        Dim nNo As Integer                                              ' No
        Dim clsGraph As tClass.tClass_MapBase                           ' グラフエリア
        Dim picGraph As PictureBox                                      ' グラフ表示エリア
    End Structure


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    Private m_typGraph(mc_NUM - 1) As TYP_TMP_GRAPH
    Private m_bSetEnd As Boolean = False                            ' 初期情報セット完了フラグ(True:セット完了、False:未完了)



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本処理
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPop_Control_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        'イベント追加
        AddHandler radNon.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radContrast.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radGamma.CheckedChanged, AddressOf radbtn_CheckedChanged
        AddHandler radSigmoid.CheckedChanged, AddressOf radbtn_CheckedChanged

        '''' 画面初期設定
        radNon.Checked = True
        Call SetInit()

        '''' マップ初期定義
        For ii As Integer = 0 To mc_NUM - 1
            '''' マップ初期定義
            Call MapInitialize(m_typGraph(ii).clsGraph)
        Next ii
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ シェーディング補正
    ''' <summary>
    ''' シェーディング補正モード変更
    ''' </summary>
    Private Sub ckbShd_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkShd.CheckedChanged
        Call ChangeShdMode(CInt(chkShd.Checked))
    End Sub

    ''' <summary>
    ''' 目標輝度値変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudShdTag_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudShdTag.ValueChanged
        m_doc.nShdTarget = CInt(nudShdTag.Value)
        m_doc.Emit()
    End Sub

    Private Sub ReCreateImage()
        If mcls_RpiView IsNot Nothing Then
            Call mcls_RpiView.ReCreateImg()
            Call mcls_RpiView.RePaint()
        End If
    End Sub

    ''' <summary>
    ''' 横サイズ変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudShdX_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudShdX.ValueChanged
        m_doc.nShdX = CInt(nudShdX.Value)
        '8の倍数のみ有効
        If 0 <> (m_doc.nShdX Mod 8) Then
            m_doc.nShdX = tMod.GET_SIZE_JUST(m_doc.nShdX, 8)
            nudShdX.Value = m_doc.nShdX
        End If
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' 縦サイズ変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudShdY_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles nudShdY.ValueChanged
        m_doc.nShdY = CInt(nudShdY.Value)
        '8の倍数のみ有効
        If 0 <> (m_doc.nShdY Mod 8) Then
            m_doc.nShdY = tMod.GET_SIZE_JUST(m_doc.nShdY, 8)
            nudShdY.Value = m_doc.nShdY
        End If
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' シェーディング補正表示切替
    ''' </summary>
    ''' <param name="nMode"></param>
    ''' <remarks></remarks>
    Private Sub ChangeShdMode(ByVal nMode As Integer)
        m_doc.bShdMode = nMode <> 0

        '画像再生成
        'ReCreateImage()
        m_doc.Emit()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ２値化補正
    ''' <summary>
    ''' ２値化補正モード変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkBinari_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles chkBinari.CheckedChanged
        Call ChangeBinariMode(CInt(chkBinari.Checked))
    End Sub

    ''' <summary>
    ''' ２値化補正表示切替
    ''' </summary>
    ''' <param name="nMode"></param>
    ''' <remarks></remarks>
    Private Sub ChangeBinariMode(ByVal nMode As Integer)
        m_doc.bBinariMode = nMode <> 0

        '画像再生成
        m_doc.Emit()
    End Sub


    ''' <summary>
    ''' 初期情報セット
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetInit()
        'グラフ番号（使わないが・・・）
        m_typGraph(0).nNo = 1
        m_typGraph(1).nNo = 2
        m_typGraph(2).nNo = 3
        m_typGraph(3).nNo = 4

        'マップのメンバー変数設定
        For ii As Integer = 0 To mc_NUM - 1
            m_typGraph(ii).clsGraph = New tClass.tClass_MapBase
        Next ii
        'ピクチャーのメンバー変数設定
        m_typGraph(0).picGraph = picGraph1
        m_typGraph(1).picGraph = picGraph2
        m_typGraph(2).picGraph = picGraph3
        m_typGraph(3).picGraph = picGraph4
        ' イベントハンドラ関連付け
        For ii As Integer = 0 To mc_NUM - 1
            ' マップ
            AddHandler m_typGraph(ii).clsGraph.tEv_DrawPaint_End, AddressOf DrawMap

            ' ピクチャーボックス
            AddHandler m_typGraph(ii).picGraph.Paint, AddressOf PictureBox_Paint

        Next ii


        '初期値反映
        trbContrast.Value = m_doc.nContrastX
        trbBright.Value = m_doc.nBright
        trbGamma.Value = m_doc.nGammaX

        trbSigmoidX.Value = m_doc.nSigmoidX
        trbSigmoidKei.Value = m_doc.nSigmoidKei
        trbBinari01.Value = m_doc.nBinari01
        trbBinari02.Value = m_doc.nBinari02

        nudShdTag.Value = m_doc.nShdTarget
        nudShdX.Value = m_doc.nShdX
        nudShdY.Value = m_doc.nShdY


        'スライドバー値表示
        lblContrast.Text = CStr(trbContrast.Value)
        lblBright.Text = CStr(trbBright.Value)
        lblGamma.Text = CStr(trbGamma.Value)
        lblGammaRitu.Text = CStr(m_doc.dGammaRitu)
        lblSigmoidX.Text = CStr(trbSigmoidX.Value)
        lblSigmoidKei.Text = CStr(trbSigmoidKei.Value)
        lblBinari01.Text = CStr(trbBinari01.Value)
        lblBinari02.Text = CStr(trbBinari02.Value)

        '初期情報セット完了フラグＯＮ
        m_bSetEnd = True
    End Sub

    ''' <summary>
    ''' マップ基本情報定義
    ''' </summary>
    ''' <param name="MapBase">グラフマップ</param>
    ''' <remarks></remarks>
    Private Sub MapInitialize(ByVal MapBase As tClass.tClass_MapBase)
        '''' グラフ描画に必要な情報を設定
        With MapBase
            '' 表示領域情報
            .tProp_AriaInf_BackGroundLineColor = Pens.Black
            .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
            .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
            .tProp_AriaInf_AriaSize = picGraph1.Size
            .tProp_AriaInf_Margin = New Padding(0, 0, 0, 0)

            '' マップ領域
            .tProp_MapInf_IsCenterX = False             'X軸センター基準？(true時センター基準。false時左上基準)
            .tProp_MapInf_IsCenterY = False             'Y軸センター基準？(true時センター基準。false時左上基準)
            .tProp_MapInf_IsBaseX_L = True              'X軸基準(true時左基準。false時右基準)(bIsCenterX=false時のみ有効)
            .tProp_MapInf_IsBaseY_T = False             'Y軸基準(true時上基準。false時下基準)(bIsCenterY=false時のみ有効)
            .tProp_MapInf_IsStrechX = True              'X軸の最小最大をマップの端にあわせる
            .tProp_MapInf_IsStrechY = True              'Y軸の最小最大をマップの端にあわせる
            .tProp_MapInf_LimitXmin = 0                 'X軸最小[mm]
            .tProp_MapInf_LimitXmax = 255               'X軸最大[mm]
            .tProp_MapInf_LimitYmin = 0                 'Y軸最小[mm]
            .tProp_MapInf_LimitYmax = 255               'Y軸最大[mm]
            'オフセット位置 (左上pixel位置の値をセット) (bIsCenterで動作が変わる。falseのみ有効。true時は自動算出)
            .tProp_MapInf_Offset = New tClass.tClass_MapBase.xyValue(0, 0)

            '' マップ対象外領域設定
            .tProp_LimitInf_LimitDraw = False           'マップと対象範囲の境界線

            '' メモリ設定
            Dim p As Pen = New Pen(Color.LightBlue, 1)
            p.DashStyle = Drawing2D.DashStyle.DashDot
            .tProp_MemoriInf_LinePen = p
            .tProp_MemoriInf_StringFont = New Font("ＭＳ ゴシック", 11, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Red)
            .tProp_MemoriInf_PosStringX = 128           'X軸の文字表示位置オフセット(X軸から何ピクセル離す)
            .tProp_MemoriInf_PosStringY = 128           'Y軸の文字表示位置オフセット(Y軸から何ピクセル離す)

            .tProp_MemoriInf_ScaleInterval = New tClass.tClass_MapBase.xyValue(128, 128)    'メモリ間隔 [mm]
            '.tProp_MemoriInf_ScaleStringFormatX = "###0"
            '.tProp_MemoriInf_ScaleStringFormatY = "0"
            .tProp_MemoriInf_ScaleStringRange = New tClass.tClass_MapBase.RATE(1, 1)
            .tProp_MemoriInf_StringDisp_T = False       '目盛り文字の表示非表示 (True:表示 False:非表示)
            .tProp_MemoriInf_StringDisp_B = False       '目盛り文字の表示非表示 (True:表示 False:非表示)
            .tProp_MemoriInf_StringDisp_L = False       '目盛り文字の表示非表示 (True:表示 False:非表示)
            .tProp_MemoriInf_StringDisp_R = False       '目盛り文字の表示非表示 (True:表示 False:非表示)
            .tProp_MemoriInf_LineDsp_X = True           'メモリ線表示有無 (True:表示 False:非表示)
            .tProp_MemoriInf_LineDsp_Y = True           'メモリ線表示有無 (True:表示 False:非表示)
        End With
    End Sub

    ''' <summary>
    ''' フォーム表示
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmPopImgControl_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        Try

            SetDsp()
        Catch ex As Exception
        End Try

    End Sub

    ''' <summary>
    ''' 画面データ描画 (設定変更時にイベント実行) (画面表示時にも)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetDsp()

        '初期情報セット未完了の場合は終了
        If Not m_bSetEnd Then
            Exit Sub
        End If

        For ii As Integer = 0 To mc_NUM - 1

            '''' マップ再生成依頼
            Call m_typGraph(ii).clsGraph.DrawPaint()

            '''' マップ再描画
            Call m_typGraph(ii).picGraph.Refresh()

        Next ii

    End Sub

    ''' <summary>
    ''' 画面データ描画 (設定変更時にイベント実行) (画面表示時にも)
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub SetDsp(ByRef nNo As Integer)

        '引数チェック
        If nNo > mc_NUM Or nNo < 1 Then
            Exit Sub
        End If

        '初期情報セット未完了の場合は終了
        If Not m_bSetEnd Then
            Exit Sub
        End If

        '''' マップ再生成依頼
        Call m_typGraph(nNo - 1).clsGraph.DrawPaint()

        '''' マップ再描画
        Call m_typGraph(nNo - 1).picGraph.Refresh()

    End Sub

    ''' <summary>
    ''' 描画
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' </summary>
    Private Sub PictureBox_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
        For ii As Integer = 0 To mc_NUM - 1
            ' グラフグループの検索
            If sender Is m_typGraph(ii).picGraph Then
                '''' ビットマップ取得
                Dim bmp As Bitmap = m_typGraph(ii).clsGraph.tProp_Bmp
                If bmp Is Nothing Then Return

                '''' 背景をセット
                e.Graphics.DrawImage(bmp, 0, 0)
            End If
        Next ii

    End Sub

    ''' <summary>
    ''' 画面データ描画（グラフ表示）
    ''' </summary>
    ''' <param name="cls"></param>
    ''' <param name="g"></param>
    ''' <remarks></remarks>
    Private Sub DrawMap(ByVal cls As tClassLibrary.tClass.tClass_MapBase, ByVal g As System.Drawing.Graphics)
        Dim nGrpNo As Integer               ' グラフ表示グループＮｏ
        Dim ii As Integer                   ' INDEX
        Try

            ' グラフグループの検索
            For ii = 0 To mc_NUM - 1
                If cls Is m_typGraph(ii).clsGraph Then
                    nGrpNo = ii
                    Exit For
                End If
            Next ii

            '''' 描画
            Dim dVal As Single
            Dim x1, y1, x2, y2 As Integer

            For ii = 0 To 255 - 1

                ' 始点算出 (先頭の座標算出) X軸
                x1 = cls.GetVtoP_X(ii + 0)

                ' 始点算出 (先頭の座標算出) Y軸
                If 0 = nGrpNo Then
                    'コントラスト、明るさ補正
                    dVal = tClass.tClass_BmpJpeg.GetIntensityContrast(ii, m_doc.nContrastX, m_doc.nBright)
                ElseIf 1 = nGrpNo Then
                    ' ガンマ補正
                    dVal = tClass.tClass_BmpJpeg.GetIntensityGamma(ii, m_doc.dGammaRitu)
                ElseIf 2 = nGrpNo Then
                    'シグモイド補正
                    dVal = tClass.tClass_BmpJpeg.GetIntensitySigmoid(ii, m_doc.nSigmoidX, m_doc.nSigmoidKei)
                ElseIf 3 = nGrpNo Then
                    '２値化補正
                    dVal = tClass.tClass_BmpJpeg.GetIntensityBinari(ii, m_doc.nBinari01, m_doc.nBinari02)
                End If
                '上下限値チェック
                If dVal > 255 Then dVal = 255
                If dVal < 0 Then dVal = 0
                y1 = cls.GetVtoP_Y(dVal)

                ' 終点算出 X軸
                x2 = cls.GetVtoP_X(ii + 1)
                ' 終点算出 Y軸
                If 0 = nGrpNo Then
                    'コントラスト、明るさ補正
                    dVal = tClass.tClass_BmpJpeg.GetIntensityContrast(ii + 1, m_doc.nContrastX, m_doc.nBright)
                ElseIf 1 = nGrpNo Then
                    ' ガンマ補正
                    dVal = tClass.tClass_BmpJpeg.GetIntensityGamma(ii + 1, m_doc.dGammaRitu)
                ElseIf 2 = nGrpNo Then
                    'シグモイド補正
                    dVal = tClass.tClass_BmpJpeg.GetIntensitySigmoid(ii + 1, m_doc.nSigmoidX, m_doc.nSigmoidKei)
                ElseIf 3 = nGrpNo Then
                    '２値化補正
                    dVal = tClass.tClass_BmpJpeg.GetIntensityBinari(ii + 1, m_doc.nBinari01, m_doc.nBinari02)
                End If
                '上下限値チェック
                If dVal > 255 Then dVal = 255
                If dVal < 0 Then dVal = 0
                y2 = cls.GetVtoP_Y(dVal)

                Using p As New Pen(Color.Red, 2)
                    ' 描画
                    g.DrawLine(p, x1, y1, x2, y2)
                End Using
            Next ii

        Catch ex As Exception
        End Try
    End Sub

    ''' <summary>
    ''' コントラスト調整スライドバー変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbContrast_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbContrast.ValueChanged
        '設定値格納
        m_doc.nContrastX = trbContrast.Value

        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' 明るさ調整スライドバー変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbBright_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbBright.ValueChanged
        '設定値格納
        m_doc.nBright = trbBright.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' ガンマ補正スライドバー変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbGamma_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbGamma.ValueChanged
        m_doc.nGammaX = trbGamma.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' シグモイド補正輝度スライドバー変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbSigmoidX_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbSigmoidX.ValueChanged
        '設定値格納
        m_doc.nSigmoidX = trbSigmoidX.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' シグモイド補正係数スライドバー変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbSigmoidKei_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbSigmoidKei.ValueChanged
        '設定値格納
        m_doc.nSigmoidKei = trbSigmoidKei.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' ２値化 閾値１スライドバー変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbBinari01_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbBinari01.ValueChanged
        '設定値格納
        m_doc.nBinari01 = trbBinari01.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' ２値化 閾値２スライドバー変更
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub trbBinari02_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles trbBinari02.ValueChanged
        '設定値格納
        m_doc.nBinari02 = trbBinari02.Value
        m_doc.Emit()
    End Sub

    ''' <summary>
    ''' ラジオボタンチェックイベント
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub radbtn_CheckedChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        If m_bStopEmit Then Exit Sub
        If radContrast.Checked Then
            m_doc.m_Filter = PropImageControl.FilterSelection.CONTRAST
        ElseIf radGamma.Checked Then
            m_doc.m_Filter = PropImageControl.FilterSelection.GAMMA
        ElseIf radSigmoid.Checked Then
            m_doc.m_Filter = PropImageControl.FilterSelection.SIGMOID
        Else
            m_doc.m_Filter = PropImageControl.FilterSelection.NONE
        End If
        m_doc.Emit()
    End Sub



    Public ReadOnly Property PluginLevel As Single Implements IPluginActivatable.PluginLevel
        Get
            Return 100
        End Get
    End Property

    Private mcls_RpiView As tClass_ViewRpiThread
    Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
        mcls_RpiView = v
    End Sub

    Public ReadOnly Property PluginName As String Implements IPluginActivatable.PluginName
        Get
            Return "ImageControl"
        End Get
    End Property

    Public ReadOnly Property ToolStripName As String Implements IEvToolStrip.ToolStripName
        Get
            Return "tsbBrite"
        End Get
    End Property

    Public Sub InitToolbar(mainstrip As ToolStripButton, o As Object) Implements IEvToolStrip.InitToolbar
        AddHandler mainstrip.Click, AddressOf tsbBrite_Click
    End Sub

    ''' <summary>
    ''' 輝度調整画面表示
    ''' </summary>
    Private Sub tsbBrite_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        If Not Me.Visible Then
            Call Me.Show(m_parentForm)
        Else
            Me.Visible = False
        End If
        If m_parentForm IsNot Nothing Then
            m_parentForm.Select()
        End If
    End Sub


    Public Sub OnFilterImage(ByRef img As Bitmap, ByVal p As tClass_ViewRpiThread.CLS_IMG_INF) Implements IFilterBitmap.OnFilterImage
        '''' シェーリング補正 有無
        If m_doc.bShdMode Then
            '''' 前処理
            Dim head As tClass.tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Header() ' ヘッダー取得
            Dim clsShdCmp As ImgLibCLI.ShadingComp2 = Nothing                           ' シェーリング補正クラス


            ' ビットマップデータ部の配列を取得
            Dim bBmpArray() As Byte = tClass.tClass_BmpJpeg.ImagToData(img)                                 ' ビットマップデータ部の配列
            Try
                '' シェーディング補正準備
                clsShdCmp = New ImgLibCLI.ShadingComp2
                clsShdCmp.Alloc(head.pitch_nbyte, head.height)
                clsShdCmp.CreateLPF(m_doc.nShdX, m_doc.nShdY, CByte(m_doc.nShdTarget))

                '' エッジ内のみのシェーディング補正が可能？
                If head.frame_info_type = tClass.tClass_ReadRPI.EM_FRM_TYPE.EDGE_PER_FRAME Then
                    ' エッジ内のみにシェーディング補正
                    Dim typ As tClass.tClass_ReadRPI.EDGE_PER_FRAME = mcls_RpiView.ReadFile_FrameInfo_EdgePerFrame(p.nIndex)
                    Call clsShdCmp.Exec(bBmpArray, bBmpArray) ', typ.edge_left, typ.edge_right)

                Else
                    ' 全面に対してシェーディング補正()
                    Call clsShdCmp.Exec(bBmpArray, bBmpArray)
                End If

            Catch ex As Exception
            Finally
                If Not clsShdCmp Is Nothing Then
                    clsShdCmp.Free()    ' シェーリング補正クラス開放
                    clsShdCmp = Nothing
                End If
            End Try


            '' 元のイメージに戻す
            img = tClassLibrary.tClass.tClass_BmpJpeg.DataToBmp(img.Width, img.Height, bBmpArray)
        End If

        '=====================================
        '''' コントラスト調整 有無
        If m_doc.bContMode Then
            tClassLibrary.tClass.tClass_BmpJpeg.ContrastCmpBmp(img, m_doc.nContrastX, m_doc.nBright)
        End If

        '=====================================
        '''' ガンマ補正 有無
        If m_doc.bGammaMode Then
            tClassLibrary.tClass.tClass_BmpJpeg.GammaCmpBmp(img, m_doc.dGammaRitu)
        End If

        '=====================================
        '''' シグモイド補正 有無
        If m_doc.bSigmoidMode Then
            tClassLibrary.tClass.tClass_BmpJpeg.SigmoidCmpBmp(img, m_doc.nSigmoidX, m_doc.nSigmoidKei)
        End If

        '------------------------------------------------------------------------------
        ' ★以下の処理(２値化補正)は必ず最後に実施すること！
        '   他の補正と併用の場合、元輝度に対してカラーパレットを作成するため。
        '------------------------------------------------------------------------------
        '''' ２値化補正 有無
        If m_doc.bBinariMode Then
            tClassLibrary.tClass.tClass_BmpJpeg.BinariCmpBmp(img, m_doc.nBinari01, m_doc.nBinari02)
        End If
    End Sub
End Class



''' <summary>
''' 輝度コントロールプロパティ類
''' </summary>
Public Class PropImageControl
    Inherits PeropertiesBase

    Public Enum FilterSelection
        NONE
        CONTRAST
        GAMMA
        SIGMOID
    End Enum
    '/ /////////////////////////////////////////////////////////////////////////////////
    '''' 公開変数

    ''' <summary>
    ''' シェーディング補正有無
    ''' </summary>
    Public bShdMode As Boolean = False                            ' 


    ''' <summary>
    ''' フィルタは何が有効？
    ''' </summary>
    Public m_Filter As FilterSelection

    ''' <summary>
    ''' コントラスト・輝度補正有効？
    ''' </summary>
    Public ReadOnly Property bContMode As Boolean
        Get
            Return m_Filter = FilterSelection.CONTRAST
        End Get
    End Property


    ''' <summary>
    ''' ガンマ補正有効?
    ''' </summary>
    Public ReadOnly Property bGammaMode As Boolean
        Get
            Return m_Filter = FilterSelection.GAMMA
        End Get
    End Property

    ''' <summary>
    ''' シグモイド補正有無
    ''' </summary>
    Public ReadOnly Property bSigmoidMode As Boolean
        Get
            Return m_Filter = FilterSelection.SIGMOID
        End Get
    End Property

    ''' <summary>２値化有無 </summary>
    Public bBinariMode As Boolean = False

    ''' <summary>シェーディング目標輝度値 </summary>
    Public nShdTarget As Integer = 0
    ''' <summary>シェーディング横サイズ </summary>
    Public nShdX As Integer = 0
    ''' <summary>シェーディング縦サイズ </summary>
    Public nShdY As Integer = 0

    ''' <summary>コントラスト補正 </summary>
    Public nContrastX As Integer = 0
    ''' <summary>明るさ補正 </summary>
    Public nBright As Integer = 0
    ''' <summary>ガンマ補正 </summary>
    Public nGammaX As Integer = 0
    ''' <summary>ガンマ補正率 </summary>
    Public ReadOnly Property dGammaRitu As Double
        Get
            Return GetGammaRitu(nGammaX)
        End Get
    End Property

    ''' <summary>シグモイド補正 X軸 </summary>
    Public nSigmoidX As Integer = 0
    ''' <summary>シグモイド補正 係数 </summary>
    Public nSigmoidKei As Integer = 0
    ''' <summary>２値化閾値 lo </summary>
    Public nBinari01 As Integer = 0
    ''' <summary>２値化閾値 ho</summary>
    Public nBinari02 As Integer = 0


    Public Sub New(r As PropImageControl)
        bShdMode = r.bShdMode
        m_Filter = r.m_Filter
        bBinariMode = r.bBinariMode

        nShdTarget = r.nShdTarget
        nShdX = r.nShdX
        nShdY = r.nShdY

        nContrastX = r.nContrastX
        nBright = r.nBright
        nGammaX = r.nGammaX
        nSigmoidX = r.nSigmoidX
        nSigmoidKei = r.nSigmoidKei
        nBinari01 = r.nBinari01
        nBinari02 = r.nBinari02

    End Sub

    Public Sub New()
        Load()
    End Sub


    Const Section As String = "RPI_VIEWER"
    ''' <summary>
    ''' 前回値保存
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Save()
        'シェーディング補正
        tMod.WriteIni(Section, "SHD_SHDTAG", LOCALINI_NAME, CStr(Me.nShdTarget))           '目標輝度値
        tMod.WriteIni(Section, "SHD_SHDX", LOCALINI_NAME, CStr(Me.nShdX))               '横サイズ
        tMod.WriteIni(Section, "SHD_SHDY", LOCALINI_NAME, CStr(Me.nShdY))               '縦サイズ

        'コントラスト、明るさ補正
        tMod.WriteIni(Section, "CNT_CONTRASTX", LOCALINI_NAME, CStr(Me.nContrastX))     'コントラスト補正スライドBar値
        tMod.WriteIni(Section, "CNT_BRIGHT", LOCALINI_NAME, CStr(Me.nBright))           '明るさ補正スライドBar値
        'ガンマ補正
        tMod.WriteIni(Section, "GMA_GAMMAX", LOCALINI_NAME, CStr(Me.nGammaX))           'ガンマ補正スライドBar値
        'シグモイド補正
        tMod.WriteIni(Section, "SGM_SIGMOIDX", LOCALINI_NAME, CStr(Me.nSigmoidX))       'シグモイド補正スライドBar値(X軸)
        tMod.WriteIni(Section, "SGM_SIGMOIDKEI", LOCALINI_NAME, CStr(Me.nSigmoidKei))   'シグモイド補正スライドBar値(係数)
        '２値化補正
        tMod.WriteIni(Section, "BIN_BINARI01", LOCALINI_NAME, CStr(Me.nBinari01))       '２値化スライドBar値(１)
        tMod.WriteIni(Section, "BIN_BINARI02", LOCALINI_NAME, CStr(Me.nBinari02))       '２値化スライドBar値(２)
    End Sub

    ''' <summary>
    ''' 前回値読込
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Load()
        Me.nShdTarget = tMod.ReadIniInt(Section, "SHD_SHDTAG", LOCALINI_NAME, 128)        '未指定の場合は、128
        Me.nShdX = tMod.ReadIniInt(Section, "SHD_SHDX", LOCALINI_NAME, 32)             '未指定の場合は、32
        Me.nShdY = tMod.ReadIniInt(Section, "SHD_SHDY", LOCALINI_NAME, 32)             '未指定の場合は、32
        Me.nContrastX = tMod.ReadIniInt(Section, "CNT_CONTRASTX", LOCALINI_NAME, 128)  '未指定の場合は、128
        Me.nBright = tMod.ReadIniInt(Section, "CNT_BRIGHT", LOCALINI_NAME, 128)        '未指定の場合は、128
        Me.nGammaX = tMod.ReadIniInt(Section, "GMA_GAMMAX", LOCALINI_NAME, 0)          '未指定の場合は、0
        Me.nSigmoidX = tMod.ReadIniInt(Section, "SGM_SIGMOIDX", LOCALINI_NAME, 128)    '未指定の場合は、128
        Me.nSigmoidKei = tMod.ReadIniInt(Section, "SGM_SIGMOIDKEI", LOCALINI_NAME, 5)  '未指定の場合は、5
        Me.nBinari01 = tMod.ReadIniInt(Section, "BIN_BINARI01", LOCALINI_NAME, 255)    '未指定の場合は、255
        Me.nBinari02 = tMod.ReadIniInt(Section, "BIN_BINARI02", LOCALINI_NAME, 0)      '未指定の場合は、0

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開関数
    ''' <summary>
    ''' ガンマ値
    ''' </summary>
    ''' <param name="nVal">補正値</param>
    Public Shared Function GetGammaRitu(ByVal nVal As Integer) As Double
        ' 指定補正値からガンマ値を算出
        If nVal > 0 Then
            Return CDbl(1.0 + 0.2 * nVal)
        ElseIf nVal <= 0 And nVal >= -5 Then
            Return CDbl(1.0 + 0.1 * nVal)
        Else
            Return CDbl(0.5 + 0.05 * (nVal + 5))
        End If
    End Function
End Class
