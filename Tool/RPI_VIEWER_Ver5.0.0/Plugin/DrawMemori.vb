Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin

Imports LabelInfo = tClassLibrary.tClass.tClass_ImageDspAxisDrawner.LabelInfo


Public Class DrawMemoriProperties
    Inherits tClassLibrary.RpiPlugin.PeropertiesBase
    Public g_nXoffset As Integer = 0                                        ' 横メモリ移動 [DP]

    Public g_nScalCntX As Integer = 100                                     ' メモリ間隔
    Public g_nScalCntY As Integer = 100
    Public g_nScalStringX As Integer = 2                                    ' 文字回数
    Public g_nScalStringY As Integer = 5
    Public g_ScaleFont As New tClassLibrary.FontInfo


    Const Section As String = "DrawMemori"
    Public Sub Save()
        tMod.WriteIni(Section, "nXoffset", LOCALINI_NAME, g_nXoffset.ToString())
        tMod.WriteIni(Section, "nScalCntX", LOCALINI_NAME, g_nScalCntX.ToString())
        tMod.WriteIni(Section, "nScalCntY", LOCALINI_NAME, g_nScalCntY.ToString())
        tMod.WriteIni(Section, "nScalStringX", LOCALINI_NAME, g_nScalStringX.ToString())
        tMod.WriteIni(Section, "nScalStringY", LOCALINI_NAME, g_nScalStringY.ToString())

        tMod.WriteIni(Section, "ScaleFont", LOCALINI_NAME, g_ScaleFont.ToString)

    End Sub

    Public Sub New()
        g_nXoffset = tMod.ReadIniInt(Section, "nXoffset", LOCALINI_NAME, g_nXoffset)
        g_nScalCntX = tMod.ReadIniInt(Section, "nScalCntX", LOCALINI_NAME, g_nScalCntX)
        g_nScalCntY = tMod.ReadIniInt(Section, "nScalCntY", LOCALINI_NAME, g_nScalCntY)
        g_nScalStringX = tMod.ReadIniInt(Section, "nScalStringX", LOCALINI_NAME, g_nScalStringX)
        g_nScalStringY = tMod.ReadIniInt(Section, "nScalStringY", LOCALINI_NAME, g_nScalStringY)
        Try
            g_ScaleFont = New tClassLibrary.FontInfo(tMod.ReadIniStr(Section, "ScaleFont", LOCALINI_NAME, g_ScaleFont.ToString))
        Catch e As Exception
        End Try
    End Sub



End Class


Public Class DrawMemori
    Implements RpiPlugin.IDrawer, RpiPlugin.IPluginActivatable, RpiPlugin.IEvToolStrip, RpiPlugin.IRpiOpen

    Private m_doc As DrawMemoriProperties
    Dim m_bDrawMemori As Boolean = True
    Private mcls_RpiView As tClass_ViewRpiThread

    Public ReadOnly Property PluginLevel As Single Implements IPluginActivatable.PluginLevel
        Get
            Return 100
        End Get
    End Property

    Public ReadOnly Property PluginName As String Implements IPluginActivatable.PluginName
        Get
            Return "DrawMemori"
        End Get
    End Property

    Public ReadOnly Property ToolStripName As String Implements IEvToolStrip.ToolStripName
        Get
            Return "tsbMemori"
        End Get
    End Property


    Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
        mcls_RpiView = v

        m_doc = PeropertiesBase.GetPropertyMember(Of DrawMemoriProperties)(mainfrm, "drawmemori")
        AddHandler m_doc.OnValueChanged, AddressOf OnValueChanged

    End Sub

    Private Sub OnValueChanged(name As String, doc As PeropertiesBase)
        If mcls_RpiView IsNot Nothing Then
            mcls_RpiView.RePaint()
        End If
    End Sub

    Public Sub InitToolbar(mainstrip As ToolStripButton, o As Object) Implements IEvToolStrip.InitToolbar
        AddHandler mainstrip.Click, AddressOf tsbMemori_Click
    End Sub

    ''' <summary>
    ''' メモリ 表示非表示切替
    ''' </summary>
    Private Sub tsbMemori_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
        m_bDrawMemori = Not m_bDrawMemori
        Call mcls_RpiView.RePaint()
    End Sub

    Public Sub OnChangeFrameNo(nf_start As Integer, nf_end As Integer) Implements IDrawer.OnChangeFrameNo
        Return
    End Sub

    Public Sub OnDraw(g As Graphics) Implements IDrawer.OnDraw
        If m_bDrawMemori Then
            Dim ImgInf As tClass.tClass_ImageDsp.IMG_INF = mcls_RpiView.GetImg_ImgInf()
            Dim CalcAns As tClass.tClass_ImageDsp.CALC_ANS = mcls_RpiView.GetImg_CalcAns()
            Dim AriaInf As tClass.tClass_ImageDsp.ARIA_INF = mcls_RpiView.GetImg_AriaInf()
            Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf()

            Dim HojoLineLen As Integer = 5                                      ' 補助メモリ高さ

            Dim nIntervalW As Integer = m_doc.g_nScalCntX                 ' X軸のメモリ間隔 [mm]
            Dim nIntervalH As Integer = m_doc.g_nScalCntY                 ' Y軸のメモリ間隔 [mm]

            Dim rate_dw As Double = (m_original_res_x / ImgInf.dResW)
            Dim rate_dh As Double = (m_original_res_y / ImgInf.dResH)

            If rate_dh > 1.5 Then
                nIntervalH = (CInt(m_doc.g_nScalCntY * rate_dh / 100)) * 100
                If nIntervalH < m_doc.g_nScalCntY Then nIntervalH = m_doc.g_nScalCntY
            End If


            Dim nCycleW As Integer = m_doc.g_nScalStringX
            Dim nCycleH As Integer = m_doc.g_nScalStringY


            Using drawAxis As New tClass_ImageDspAxisDrawner(g, HojoLineLen, New Point(0, 0))
                drawAxis.UpdateMemoriPenBrushFont(New SolidBrush(Color.Cyan), m_doc.g_ScaleFont.CreateFont(), New System.Drawing.Pen(Color.Cyan, 2))


                '幅軸表示
                Dim center As Integer = CInt(ImgInf.nImgW * CalcAns.RateDsp.dW / 2)
                center += mcls_RpiView.GetPos_LenToDp_X(m_doc.g_nXoffset)

                Dim habaLabels As List(Of LabelInfo) = GenerateHabaLabels(DspInf.DspPixelTotalOffset.X, center, nIntervalW, nCycleW, CalcAns.FinalRes.dW, rate_dw)
                    If mcls_RpiView.PropRotate90 Then
                        drawAxis.draw_y_axis(habaLabels, AriaInf.nAriaHeight - 10)
                    Else
                        drawAxis.draw_x_axis(habaLabels, AriaInf.nAriaHeight - 5)
                    End If

                    '長手軸表示
                    Dim nagateLabels As List(Of LabelInfo) = GenerateNagateLabels(DspInf.DspPixelTotalOffset.Y, nIntervalH, nCycleH, CalcAns.FinalRes.dH, rate_dh)
                    If mcls_RpiView.PropRotate90 Then
                        drawAxis.draw_x_axis(nagateLabels, AriaInf.nAriaWidth - 5)
                    Else
                        drawAxis.IsCenterV = False
                        drawAxis.draw_y_axis(nagateLabels, AriaInf.nAriaWidth - 10)
                    End If
                End Using

            End If

    End Sub

    ''' <summary>
    ''' 幅方向軸用に、中心から、Interval間隔（両方向）でラベルを作成する
    ''' </summary>
    ''' <param name="pxCcenter">中心</param>
    ''' <param name="mmInterval">メモリ間隔</param>
    ''' <param name="nCycle">文字線間隔</param>
    ''' <param name="finalres">mm->px比率</param>
    ''' <param name="setrate">分解能強制変更率</param>
    ''' <returns>メモリ座標・ラベル配列</returns>
    ''' <remarks>ラベルはメモリの２点毎で作成</remarks>
    Function GenerateHabaLabels(ByVal dspxTotalOffset As Integer, ByVal pxCcenter As Integer, ByVal mmInterval As Integer, ByVal nCycle As Integer, ByVal finalres As Double, ByVal setrate As Double) _
     As List(Of LabelInfo)

        Dim rlist As New List(Of LabelInfo)

        '中心
        rlist.Add(New LabelInfo(pxCcenter - dspxTotalOffset, "0mm"))

        'プラスマイナス15本
        For ii As Integer = 1 To 30
            For jj As Integer = -1 To 1 Step 2 'プラスマイナス
                Dim pp As Integer = CInt(pxCcenter + jj * ii * mmInterval / finalres) - dspxTotalOffset
                'ラベルは １飛ばしで表示
                'If 0 = (ii Mod 2) Then
                If 0 = (ii Mod nCycle) Then
                    rlist.Add(New LabelInfo(pp, CInt(ii * mmInterval).ToString()))
                Else
                    'メモリのみ
                    rlist.Add(New LabelInfo(pp, ""))
                End If
            Next
        Next
        Return rlist
    End Function


    ''' <summary>
    ''' 長手軸のラベル作成 (各フレームの先頭位置の距離を表示)
    ''' </summary>
    ''' <param name="dspxTotalOffset">スクロール量[px]</param>
    ''' <param name="mmInterval">メモリ間隔[mm]</param>
    ''' <param name="nCycle">文字線間隔</param>
    ''' <returns>メモリ座標・ラベル配列</returns>
    ''' <remarks>ラベルはメモリの５点毎で作成</remarks>
    Function GenerateNagateLabels(ByVal dspxTotalOffset As Integer, ByVal mmInterval As Integer, ByVal nCycle As Integer, ByVal finalres As Double, ByVal setrate As Double) As List(Of LabelInfo)
        Dim AriaInf As tClass.tClass_ImageDsp.ARIA_INF = mcls_RpiView.GetImg_AriaInf()
        Dim mmStartY As Integer = CInt(mcls_RpiView.GetPos_DpToLen_mm(dspxTotalOffset) * setrate / mmInterval) * mmInterval     ' 0.1m単位とする
        Dim mmEndY As Integer = CInt(mcls_RpiView.GetPos_DpToLen_mm(dspxTotalOffset + Math.Max(AriaInf.nAriaWidth, AriaInf.nAriaHeight)) * setrate)
        'CInt((DspInf.DspPixelTotalOffset.Y + picFrameImg.Size.Height) * CalcAns.FinalRes.dH)

        Dim nagateLabels As New List(Of LabelInfo)

        For mmPos As Integer = mmStartY To mmEndY Step mmInterval
            Dim pxPosWkY As Integer = mcls_RpiView.GetPos_LenToDp_mm(mmPos / setrate) - dspxTotalOffset ' mmからpixelに変換

            ' メモリ
            'If 0 = (mmPos Mod (5 * mmInterval)) Then
            If 0 = (mmPos Mod (nCycle * mmInterval)) Then
                Dim sWk As String = String.Format("{0:0.0}m", mmPos / 1000)

                nagateLabels.Add(New LabelInfo(pxPosWkY, sWk))
            Else
                nagateLabels.Add(New LabelInfo(pxPosWkY, ""))
            End If
        Next

        Return nagateLabels
    End Function

    Dim m_original_res_x As Single = 1
    Dim m_original_res_y As Single = 1

    Public Sub OnRpiOpen(rpi As tClass_ReadRPI) Implements IRpiOpen.OnRpiOpen
        m_original_res_x = rpi.GetRpiHeader.res_x
        m_original_res_y = rpi.GetRpiHeader.res_y
    End Sub

    Public Sub OnRpiClose() Implements IRpiOpen.OnRpiClose
        Return
    End Sub
End Class
