
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopSetting

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 公開イベント
    Public Event tEv_ReCreate()                                                                         ' 画像再生成


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' クラス
    Private mcls_RpiView As tClass_ViewRpiThread = Nothing             ' RPIファイル表示クラス (ポインタ)


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ
    Const displayfmt As String = "0.000000"
    Private m_sedoc As DisplayShrinker
    Sub OnValueChanged(ByVal name As String, doc As RpiPlugin.PeropertiesBase)
        If name = "DisplayShrinker" Then
            lblNowResY.Text = m_sedoc.nResY.ToString(displayfmt)
            lblNowResX.Text = m_sedoc.nResX.ToString(displayfmt)
            lblBaseResY.Text = m_sedoc.nBaseResY.ToString(displayfmt)
            lblBaseResX.Text = m_sedoc.nBaseResX.ToString(displayfmt)

            If Not mcls_RpiView.IsRpi Then Return

            Dim bUpdated As Boolean = False
            Dim t As tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Cls().PropRpiHeader()
            If t.res_y <> CSng(m_sedoc.nResY) Then
                t.res_y = CSng(m_sedoc.nResY)
                bUpdated = True
            End If
            If t.res_x <> CSng(m_sedoc.nResX) Then
                t.res_x = CSng(m_sedoc.nResX)
                bUpdated = True
            End If
            If bUpdated Then
                mcls_RpiView.GetRpi_Cls().PropRpiHeader() = t      ' ヘッダー変更
                ReCalc()
                RaiseEvent tEv_ReCreate()
            End If
        End If
    End Sub

    Public Sub SetDocument(doc As DisplayShrinker)
        m_sedoc = doc
        AddHandler m_sedoc.OnValueChanged, AddressOf OnValueChanged
    End Sub

    ''' <summary>
    ''' RPIクラス
    ''' </summary>
    ''' <param name="cls">RPIクラス</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_RpiView = cls
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本
    ''' <summary>
    ''' フォームロード
    ''' </summary>
    Private Sub frmPopSetting_Load(sender As Object, e As System.EventArgs) Handles Me.Load
    End Sub

    ''' <summary>
    ''' 最前面じゃない 且つ 後ろにいっちゃうフォーム
    ''' </summary>
    Private Sub frmPopSetting_Deactivate(sender As Object, e As System.EventArgs) Handles Me.Deactivate
        '' フォーカスが外れたら、閉じちゃおう
        'Me.Visible = False
    End Sub




    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 処理

    ''' <summary>
    ''' 再計算
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ReCalc()
        Dim t As tClass_ReadRPI.RPI_HEADER = mcls_RpiView.GetRpi_Cls().PropRpiHeader()

        '' 描画関係再設定
        mcls_RpiView.PropCls_ImgDsp.SetImgInf(t.width, t.height, t.res_x, t.res_y, 0, 0, 1, 1)     ' 表示倍率

        Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)        ' LenToRpを現状のものにする。

        mcls_RpiView.tProp_LabelOffsetYmm = 0
        If mcls_RpiView.GetRpi_Header.IsTHeader Then
            Dim tHeader As tClass_ReadRPI.RPI_T_HEADER = mcls_RpiView.GetRpi_THeader

            Dim tpos_px As Integer = tHeader.nFTposCfNo(0) * mcls_RpiView.GetRpi_Header.height + tHeader.nFTposPixel(0)
            If tpos_px <> 0 Then
                Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)     ' スクロールバー量再計算
                mcls_RpiView.tProp_LabelOffsetYmm = -mcls_RpiView.GetPos_RpToLen_Y(tpos_px)
            Else
                'If tHeader.nStartLen < 3 * mcls_RpiView.GetRpi_Header.res_y * mcls_RpiView.GetRpi_Header.height Then
                If tHeader.nStartLen < (mcls_RpiView.GetDspAriaInf.nDspFrameCnt - 1) * mcls_RpiView.GetRpi_Header.res_y * mcls_RpiView.GetRpi_Header.height Then
                    mcls_RpiView.tProp_ImgOffY = mcls_RpiView.GetPos_LenToRp_Y(mcls_RpiView.GetRpi_THeader.nStartLen * t.res_y / m_sedoc.nBaseResY)
                    Call mcls_RpiView.ResizePaintAria(Nothing, Nothing)        ' スクロールバー量再計算
                End If
            End If
        End If


        ' '''' 倍率変更による縮尺
        'Dim ImgInf As tClass.tClass_ImageDsp.IMG_INF = mcls_RpiView.GetImg_ImgInf()
        'res = nudResY_Dev.Value
        'res = CSng(res * mcls_RpiView.PropCls_ImgDsp.tProp_AriaInf_ZoomH)
        'mcls_RpiView.PropCls_ImgDsp.tProp_AriaInf_ZoomH = res


        'RaiseEvent tEv_ReCreate()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ Y
    ''' <summary>
    ''' 縦分解能変更(倍率指定)
    ''' </summary>
    Private Sub btnChageY_Dev_Click(sender As System.Object, e As System.EventArgs) Handles btnChageY_Dev.Click
        m_sedoc.nResY *= nudResY_Dev.Value
        m_sedoc.Emit()
    End Sub

    ''' <summary>
    ''' 縦分解能変更(値指定)
    ''' </summary>
    Private Sub btnChageY_Val_Click(sender As System.Object, e As System.EventArgs) Handles btnChageY_Val.Click
        m_sedoc.nResY = nudResY_Val.Value
        m_sedoc.Emit()
    End Sub

    ''' <summary>
    ''' 縦分解能変更(デフォルトに)
    ''' </summary>
    Private Sub btnChageY_Init_Click(sender As System.Object, e As System.EventArgs) Handles btnChageY_Init.Click
        m_sedoc.nResY = m_sedoc.nBaseResY
        m_sedoc.Emit()
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ X

    ''' <summary>
    ''' 横分解能変更(倍率指定)
    ''' </summary>
    Private Sub btnChageX_Dev_Click(sender As System.Object, e As System.EventArgs) Handles btnChageX_Dev.Click
        m_sedoc.nResX *= nudResX_Dev.Value
        m_sedoc.Emit()
    End Sub

    Private Sub btnChageX_Val_Click(sender As System.Object, e As System.EventArgs) Handles btnChageX_Val.Click
        m_sedoc.nResX = nudResX_Val.Value
        m_sedoc.Emit()
    End Sub

    Private Sub btnChageX_Init_Click(sender As System.Object, e As System.EventArgs) Handles btnChageX_Init.Click
        m_sedoc.nResX = m_sedoc.nBaseResX
        m_sedoc.Emit()
    End Sub
End Class