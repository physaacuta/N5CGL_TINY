Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin

Public Class frmPlDspLen
    Implements IPluginActivatable, IMouseToolStrip, IDrawer

    Private mcls_RpiView As tClass_ViewRpiThread
    Private mcls_pCalc As tClass_ImageDsp_Calc  ' 画像計算クラス (ポインタ)

    Public ReadOnly Property PluginLevel As Single Implements IPluginActivatable.PluginLevel
        Get
            Return 100
        End Get
    End Property

    Public ReadOnly Property PluginName As String Implements IPluginActivatable.PluginName
        Get
            Return "frmPlDspLen"
        End Get
    End Property

    Dim maindoc As DisplayShrinker
    Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
        mcls_RpiView = v
        mcls_pCalc = mcls_RpiView.GetCls_ImgDsp()

        maindoc = PeropertiesBase.GetPropertyMember(Of DisplayShrinker)(mainfrm)

    End Sub
    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 定数 構造体定義

    ''' <summary>
    ''' 計算座標の構造体
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure INFO_LEN
        Dim bIsData As Boolean          ' 有効なデータ？

        Dim nFrameNo As Integer         ' 指定した点のフレームNoをセット
        Dim nPixelX As Integer          ' フレーム左上からの距離 [RP] [pixel]
        Dim nPixelY As Integer          ' フレーム左上からの距離 [RP] [pixel]
    End Structure

    ''' <summary>
    ''' 結果構造体
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure ANS_LEN
        Dim dY As Double                ' 長さ [mm]
        Dim dX As Double                ' 幅 [mm]
        Dim dL As Double                ' 斜辺 [mm]
    End Structure



    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数
    Private mfrm_pPopCont As frmPopControl      ' コントロール画面 (ポインタ)


    Private m_InfoLen(1) As INFO_LEN            ' 計算座標
    Private m_AnsLen As ANS_LEN                 ' 計算結果


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ

    Public Sub InitFrm_Control(ByVal frm As frmPopControl)
        mfrm_pPopCont = frm
    End Sub



    ''' <summary>
    ''' 計算座標 セット
    ''' </summary>
    ''' <param name="Index">0:始点 1:終点</param>
    ''' <param name="nPixelX">生画像先頭からの長手位置 [pixel] [RP]</param>
    ''' <param name="nPixelY">生画像先頭からの幅位置 [pixel] [RP]</param>
    ''' <returns>復帰情報</returns>
    ''' <remarks></remarks>
    Public Function SetInfoLen(ByVal Index As Integer, ByVal nPixelX As Integer, ByVal nPixelY As Integer) As Boolean
        If 0 > Index Or 1 < Index Then Return False

        m_InfoLen(Index).nFrameNo = nPixelY \ mcls_pCalc.tProp_ImgInf.nOneImgH
        m_InfoLen(Index).nPixelY = nPixelY Mod mcls_pCalc.tProp_ImgInf.nOneImgH
        m_InfoLen(Index).nPixelX = nPixelX
        m_InfoLen(Index).bIsData = True

        '' データセットイベントを発生させる
        'RaiseEvent EV_DataSet()
        Return True
    End Function

    ''' <summary>
    ''' 計算座標 アクセス
    ''' </summary>
    ''' <param name="Index">0:始点 1:終点</param>
    Public ReadOnly Property GetInfoLen(ByVal Index As Integer) As INFO_LEN
        Get
            If 0 > Index Or 1 < Index Then Return Nothing

            Return m_InfoLen(Index)
        End Get
    End Property

    ''' <summary>
    ''' 計算結果 アクセス
    ''' </summary>
    Public ReadOnly Property GetAnsLen() As ANS_LEN
        Get
            Return m_AnsLen
        End Get
    End Property


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本

    Dim m_parentForm As Form
    Public Sub New(parent As Form)
        Me.New()
        m_parentForm = parent
    End Sub
    ''' <summary>
    ''' コンストラクタ
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub New()

        ' この呼び出しは、Windows フォーム デザイナで必要です。
        InitializeComponent()

        ' InitializeComponent() 呼び出しの後で初期化を追加します。
        ' データの初期化
        Call InitData(True)
    End Sub

    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPop_DspLength_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        ' 画面の更新もしておく
        Call Dsp()
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理

    ''' <summary>
    ''' 長さ計算
    ''' </summary>
    ''' <param name="LenS">始点の座標</param>
    ''' <param name="LenE">終点の座標</param>
    ''' <returns>計算結果</returns>
    ''' <remarks></remarks>
    Public Function ExecLength(ByVal LenS As INFO_LEN, ByVal LenE As INFO_LEN, ByRef ans As ANS_LEN) As Boolean
        ' データが有効
        If Not LenS.bIsData Or Not LenE.bIsData Then Return False

        Dim CalcAns As tClass_ImageDsp_Calc.CALC_ANS = mcls_pCalc.tProp_CalcAns

        '''' 計算 pixel
        ' 幅方向 (１フレーム内のpixel数)
        Dim x As Integer = Math.Abs(LenE.nPixelX - LenS.nPixelX)
        ' 長手方向 (0フレーム目からのトータルpixel数に直す。その後、計算)
        Dim y As Integer = Math.Abs((LenE.nFrameNo * mcls_pCalc.tProp_ImgInf.nOneImgH + LenE.nPixelY) - (LenS.nFrameNo * mcls_pCalc.tProp_ImgInf.nOneImgH + LenS.nPixelY))

        '''' 結果
        ans.dX = mcls_pCalc.GetPos_RpToLen_X(x)
        ans.dY = mcls_pCalc.GetPos_RpToLen_Y(y)
        ans.dL = Math.Sqrt(ans.dX ^ 2 + ans.dY ^ 2)
        Return True
    End Function

    ''' <summary>
    ''' 長さ計算後、画面表示
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspLength()
        '' 計算
        If Not ExecLength(m_InfoLen(0), m_InfoLen(1), m_AnsLen) Then
            Call InitData(False)
        End If

        ' 画面表示
        Call Dsp()
    End Sub

    ''' <summary>
    ''' メンバー変数初期化
    ''' </summary>
    ''' <param name="bMode">true:計算結果+計算座標クリア false:計算結果のみクリア</param>
    ''' <remarks></remarks>
    Public Sub InitData(Optional ByVal bMode As Boolean = False)
        ' 計算座標クリア
        If bMode Then
            For ii As Integer = 0 To m_InfoLen.Length - 1
                m_InfoLen(ii) = New INFO_LEN
            Next
        End If

        ' 計算結果クリア
        m_AnsLen = New ANS_LEN

        ' 画面の更新もしておく
        Call Dsp()
    End Sub

    ''' <summary>
    ''' 計算結果画面表示
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub Dsp()
        If ckbRes.Checked Then
            ' 元分解能で長さ計測
            Dim inf As tClass_ImageDsp_Calc.IMG_INF = mcls_pCalc.tProp_ImgInf
            Dim dX As Double = (maindoc.nBaseResX / inf.dResW) * m_AnsLen.dX
            Dim dY As Double = (maindoc.nBaseResY / inf.dResH) * m_AnsLen.dY

            lblY.Text = String.Format("{0:####0.0}", dY)
            lblX.Text = String.Format("{0:####0.0}", dX)
            lblL.Text = String.Format("{0:####0.0}", Math.Sqrt(dX ^ 2 + dY ^ 2))
        Else
            lblY.Text = String.Format("{0:####0.0}", m_AnsLen.dY)
            lblX.Text = String.Format("{0:####0.0}", m_AnsLen.dX)
            lblL.Text = String.Format("{0:####0.0}", m_AnsLen.dL)
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 描画

    ''' <summary>
    ''' 指定地点描画
    ''' </summary>
    ''' <param name="g">グラフィックス</param>
    ''' <remarks></remarks>
    Public Sub PaintPos(ByVal g As Graphics, ByVal nNowStartFNo As Integer, ByVal nDspFrameRowCnt As Integer)
        Dim nHosei As Integer = 10
        Dim pos As Point        ' 表示座標位置
        Dim infoLen As INFO_LEN

        Dim pen(1) As Pen
        pen(0) = New System.Drawing.Pen(Color.Red, 1)
        pen(1) = New System.Drawing.Pen(Color.Yellow, 1)

        '' 始点位置
        For ii As Integer = 0 To 1
            infoLen = GetInfoLen(ii)
            If Not infoLen.bIsData Then Continue For ' データの有無

            ' 表示しているフレーム？
            If infoLen.nFrameNo >= nNowStartFNo And infoLen.nFrameNo <= nNowStartFNo + nDspFrameRowCnt - 1 Then
                ' 表示座標に変換
                infoLen.nPixelY += (infoLen.nFrameNo - nNowStartFNo) * mcls_pCalc.tProp_ImgInf.nOneImgH
                pos = mcls_pCalc.GetPosition(infoLen.nPixelX, infoLen.nPixelY)

                ' 表示
                g.DrawLine(pen(ii), pos.X - nHosei, pos.Y, pos.X + nHosei, pos.Y)
                g.DrawLine(pen(ii), pos.X, pos.Y - nHosei, pos.X, pos.Y + nHosei)
            End If
        Next ii

        '' 後始末
        pen(0).Dispose()
        pen(1).Dispose()
    End Sub

    ''' <summary>
    ''' 結果反映
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub ckbRes_CheckedChanged(sender As System.Object, e As System.EventArgs) Handles ckbRes.CheckedChanged
        Call Dsp()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 他へ


    ''' <summary>
    ''' ジャンプ画面に結果反映
    ''' </summary>
    Private Sub btnSetCalc_Click(sender As System.Object, e As System.EventArgs) Handles btnSetCalc.Click
        'mfrm_pPopConttsbDspLen
        mfrm_pPopCont.SetVal_Calc(m_AnsLen.dY)
    End Sub

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プラグインインターフェース

    Public Function InitToolbar() As ToolStripButton Implements IMouseToolStrip.InitToolbar
        'Dim resources As New System.ComponentModel.ComponentResourceManager(GetType(frmMain))
        Dim tsbDspLen As New System.Windows.Forms.ToolStripButton()
        tsbDspLen.CheckOnClick = True
        tsbDspLen.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        tsbDspLen.Image = LabelIcon.Image 'CType(resources.GetObject("tsbDspLen.Image"), System.Drawing.Image)
        tsbDspLen.ImageTransparentColor = System.Drawing.Color.Magenta
        tsbDspLen.Name = "tsbDspLen"
        tsbDspLen.Size = New System.Drawing.Size(23, 23)
        tsbDspLen.Text = "長さ計測"
        Return tsbDspLen
        
    End Function

    Public Shadows Function OnMouseClick(e As EventArgs) As Boolean Implements IMouseToolStrip.OnMouseClick
        '''' 始点/終点位置

        Dim Index As Integer
        Dim evtMouse As MouseEventArgs = CType(e, MouseEventArgs)
        If evtMouse.Button = Windows.Forms.MouseButtons.Left Then       ' 始点位置
            Index = 0
        ElseIf evtMouse.Button = Windows.Forms.MouseButtons.Right Then  ' 終点位置
            Index = 1
        Else
            Return False
        End If


        '''' 元画像の座標を算出
        ' 指定位置の表示座標 算出
        Dim pos As Point = mcls_RpiView.GetCls_ImgDsp.GetDspPixelToImgPosition(evtMouse.X, evtMouse.y)
        pos.Y += mcls_RpiView.GetDspAriaInf.nDspFno * mcls_RpiView.GetImg_ImgInf.nOneImgH   ' 見えていない分を加算


        '''' データセット
        Call Me.SetInfoLen(Index, pos.X, pos.Y)
        ' 計算&結果表示
        Call Me.DspLength()

        mcls_RpiView.RePaint()

        Return True
    End Function

    Public Shadows Function Activate(bActive As Boolean) As Boolean Implements IMouseToolStrip.Activate
        If Me.ShowToggle(m_parentForm) Then
            Me.InitData(True)
        End If
        Return True
    End Function

    Public Sub OnDraw(g As Graphics) Implements IDrawer.OnDraw
        If Me.Visible Then
            Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf()
            PaintPos(g, DspInf.nDspFno, DspInf.nDspFrameCnt)
        End If
    End Sub

    Public Sub OnChangeFrameNo(nf_start As Integer, nf_end As Integer) Implements IDrawer.OnChangeFrameNo
        Return
    End Sub


End Class