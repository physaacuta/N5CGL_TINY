Imports System.Windows.Forms
Imports System.Drawing
Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.RpiPlugin

Public Class DrawHotRoll
    Implements RpiPlugin.IPluginActivatable, RpiPlugin.IRpiOpen, RpiPlugin.IDrawer, RpiPlugin.IMouseToolStrip

    Dim mcls_RpiView As tClass_ViewRpiThread

    Public ReadOnly Property PluginLevel As Single Implements IPluginActivatable.PluginLevel
        Get
            Return 200
        End Get
    End Property


    Public ReadOnly Property PluginName As String Implements IPluginActivatable.PluginName
        Get
            Return "DrawHotRoll"
        End Get
    End Property

    Public Sub InitPlugin(v As tClass_ViewRpiThread, mainfrm As System.Windows.Forms.Form) Implements IPluginActivatable.InitPlugin
        mcls_RpiView = v
        AddHandler HotRollInfo.DrawProp.OnValueChanged, AddressOf OnValueChanged
    End Sub

    Private Sub OnValueChanged(name As String, doc As PeropertiesBase)
        If mcls_RpiView IsNot Nothing Then
            mcls_RpiView.RePaint()
        End If
    End Sub

    Public Function InitToolbar() As ToolStripButton Implements IMouseToolStrip.InitToolbar
        Dim resources As System.ComponentModel.ComponentResourceManager = New System.ComponentModel.ComponentResourceManager(GetType(frmPopHotJump))
        Dim tsbHotRoll As New System.Windows.Forms.ToolStripButton()
        tsbHotRoll.CheckOnClick = True
        tsbHotRoll.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image
        tsbHotRoll.Image = CType(resources.GetObject("LabelIconForDraw.Image"), System.Drawing.Image)
        tsbHotRoll.ImageTransparentColor = System.Drawing.Color.Magenta
        tsbHotRoll.Name = "tsbHotRoll"
        tsbHotRoll.Size = New System.Drawing.Size(23, 23)
        tsbHotRoll.Text = "熱延ロール長起点"
        Return tsbHotRoll

    End Function





    Const TASKINI_NAME As String = ".\Rpi_Viewer.ini"

    ''' <summary>
    ''' 自分の（＝RPIを作った）ラインに問い合わせて、HOTコイルNOをもらう。
    ''' </summary>
    ''' <param name="rpi">ラインID, 管理NOを使用</param>
    ''' <returns>HOTコイルNO</returns>
    Public Function GetCoilNo(rpi As tClass_ReadRPI, ByRef sSql As String) As String
        Dim sSession As String = "DB_" & rpi.GetRpiTHeader.cSysName
        Dim sSqlFormat As String = tMod.tMod_IniFile.ReadIniStr(sSession, "SQL_COIL_DATA", TASKINI_NAME)

        Dim sCoilS0 As String = tMod.tMod_IniFile.ReadIniStr(sSession, "COIL_S_0", TASKINI_NAME).Split(";"c)(0)
        If sCoilS0.Contains("HOTコイル") Then
            Dim sKeyNo As String = tMod.tMod_IniFile.ReadIniStr(sSession, "KEY_NO", TASKINI_NAME)
            Dim sKeyCoilInf As String = tMod.tMod_IniFile.ReadIniStr(sSession, "KEY_COILINF", TASKINI_NAME)

            '2,3,4PL
            sSQL = String.Format("SELECT {0} FROM {1} WHERE {2}='{3}'", sCoilS0, sKeyCoilInf, sKeyNo, rpi.GetRpiTHeader.cKizukenNo)
        Else
            'HOTMAKI,SIAGE
            sSQL = String.Format(sSqlFormat, rpi.GetRpiTHeader.cKizukenNo)
        End If

        Using tcls_DB As New tClass_SQLServer(TASKINI_NAME, sSession)

            If Not tcls_DB.Open() Then Return ""
            Dim sCoilNo As String = ""
            tcls_DB.GetSelectKey(sSQL, sCoilNo)
            Return sCoilNo
        End Using
    End Function

    Shared Function IsTargetLine(sysname As String) As Integer
        Select Case sysname
            Case "N2PL", "N3PL", "N4PL_C"
                Return 60
            Case "NHOT_ZENTYO"
                Return 2
            Case "NHOT_MAKI"
                Return 1
            Case Else
                Return -1
        End Select

    End Function

    Dim m_original_res_x As Single = 1
    Dim m_original_res_y As Single = 1
    Public Sub OnRpiOpen(rpi As tClass_ReadRPI) Implements IRpiOpen.OnRpiOpen

        m_RolInfo = Nothing
        m_original_res_x = rpi.GetRpiHeader.res_x
        m_original_res_y = rpi.GetRpiHeader.res_y

        If String.IsNullOrEmpty(rpi.GetRpiTHeader.cKizukenNo) Then
            m_RolInfo = New HotRollInfo("?", String.Format("RPIには検査管理NOが存在しません"))
            Return
        End If
        Dim nDays As Integer = IsTargetLine(rpi.GetRpiTHeader.cSysName)
        If nDays < 0 Then
            m_RolInfo = New HotRollInfo("?", String.Format("{0} は対象外ラインです", rpi.GetRpiTHeader.cSysName))
            Return
        End If

        'まず、自分(RPI)のラインのDBにつないで、検査管理NO→コイルNOを取る。（RPIファイルには検査管理NOしかないので）
        '  厳密には、自分(RPI)が巻き取り前なら、一気に疵検査NOをつかって取れるが,まずはプログラム性重視で。
        Dim sSql As String = ""
        Dim sCoilno As String = ""
        Try
            sCoilno = GetCoilNo(rpi, sSql)
            If String.IsNullOrEmpty(sCoilno) Then
                m_RolInfo = New HotRollInfo("?", String.Format("{0} は{1}では見つかりませんでした。{2}", rpi.GetRpiTHeader.cKizukenNo, rpi.GetRpiTHeader.cSysName, sSql))
                Return
            End If
        Catch e As Exception
            m_RolInfo = New HotRollInfo("?", String.Format("{0}を{1}で探している最中に、エラーが発生。{2} {3}", rpi.GetRpiTHeader.cKizukenNo, rpi.GetRpiTHeader.cSysName, sSql, e.ToString))
            Return
        End Try


        '巻取前からコイルNOを検索する、
        Using tcls_DB As New tClass_SQLServer(TASKINI_NAME, "DB_NHOT_MAKI")
            Dim kkBegin As String = HotRollInfo.GetKanriNoDatePart(rpi.GetRpiTHeader.cKizukenNo, -nDays * 86400)
            Dim kkEnd As String = HotRollInfo.GetKanriNoDatePart(rpi.GetRpiTHeader.cKizukenNo, 3600)
            m_RolInfo = New HotRollInfo()
            If Not m_RolInfo.ReadFromMakitori(tcls_DB, sCoilno & "      ", kkBegin, kkEnd) Then

            End If
        End Using
    End Sub
    Public Sub OnRpiClose() Implements IRpiOpen.OnRpiClose
        m_RolInfo = Nothing
    End Sub



    Public Function Activate(bActive As Boolean) As Boolean Implements IMouseToolStrip.Activate
        Return True
    End Function


    Dim m_bDraw As Boolean
    Dim m_RolInfo As HotRollInfo
    Dim m_nFrameNo As Integer
    Dim m_nPixelY As Integer



    Public Function OnMouseClick(e As EventArgs) As Boolean Implements IMouseToolStrip.OnMouseClick
        Dim rate_dh As Double = 1 / (m_original_res_y / mcls_RpiView.GetImg_ImgInf.dResH)

        Dim evtMouse As MouseEventArgs = DirectCast(e, MouseEventArgs)
        If evtMouse.Button = MouseButtons.Right Then
            m_bDraw = False
        Else
            m_bDraw = True
            '''' 元画像の座標を算出
            ' 指定位置の表示座標 算出

            Dim rp_y As Integer = mcls_RpiView.GetPos_DpToRp_Y(mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y + evtMouse.Y) 'CInt((DspInf.DspPixelTotalOffset.Y / CalcAns.RateDsp.dH) + (nowPos.Y / CalcAns.RateDsp.dH))
            rp_y = CInt(rp_y / rate_dh)

            m_nFrameNo = rp_y \ mcls_RpiView.GetImg_ImgInf.nOneImgH
            m_nPixelY = rp_y Mod mcls_RpiView.GetImg_ImgInf.nOneImgH
        End If
        mcls_RpiView.RePaint()

        Return True
    End Function



    Public Sub OnChangeFrameNo(nf_start As Integer, nf_end As Integer) Implements IDrawer.OnChangeFrameNo

    End Sub

    Public Sub OnDraw(g As System.Drawing.Graphics) Implements IDrawer.OnDraw
        If Not m_bDraw Then Exit Sub
        If m_RolInfo Is Nothing Then Exit Sub
        Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_RpiView.GetDspAriaInf()
        PaintPos(g, DspInf.nDspFno, DspInf.nDspFrameCnt)
    End Sub



    ''' <remarks>各ロールの長さ事にの横線を表示する</remarks>
    Public Sub PaintPos(ByVal g As Graphics, ByVal nNowStartFNo As Integer, ByVal nDspFrameRowCnt As Integer)
        Dim rate_dh As Double = 1 / (m_original_res_y / mcls_RpiView.GetImg_ImgInf.dResH)

        Dim width As Integer = mcls_RpiView.GetCls_ImgDsp.tProp_AriaInf_Aria.Width
        Using dfont As New Font("Calibri", HotRollInfo.DrawProp.fontsize, GraphicsUnit.Pixel)
            Using pen As New Pen(Color.Yellow, HotRollInfo.DrawProp.linewidth)
                pen.DashStyle = HotRollInfo.DrawProp.linestyle

                Dim rY As Integer = m_nPixelY
                rY += (m_nFrameNo) * mcls_RpiView.GetImg_ImgInf.nOneImgH


                '原点表示
                Dim base_y As Integer = mcls_RpiView.GetPos_RpToDp_Y(CInt(rY * rate_dh)) - mcls_RpiView.GetDspAriaInf.DspPixelTotalOffset.Y
                g.DrawLine(pen, 100, base_y, width, base_y)

                Dim sz As SizeF = g.MeasureString("0", dfont)
                g.DrawString("0", dfont, Brushes.Yellow, 0, base_y - sz.Height)

                For Each p As RollPitch In m_RolInfo
                    If p.mmPitch = 0 Then Continue For
                    '横線表示
                    For Each direction As Integer In New Integer() {-1, 1}

                        Dim y As Integer = base_y + direction * mcls_RpiView.GetPos_LenToDp_Y(p.mmPitch * rate_dh)
                        g.DrawLine(pen, 0, y, width, y)

                        'ラベル表示（ロール名と、ロールの長さ）
                        sz = g.MeasureString(p.Name, dfont)
                        Dim msg As String = String.Format("{0}{1}({2})", IIf(direction < 0, "-", ""), p.Name, (p.mmPitch / 1000).ToString("#,###.0m"))
                        g.DrawString(msg, dfont, Brushes.Yellow, 0, y - sz.Height + 2)

                        'g.DrawString(p.Name, dfont, Brushes.Yellow, 0, y - sz.Height)
                        'g.DrawString((p.mmPitch / 1000).ToString("#,###.0m"), dfont, Brushes.Yellow, 0, y)
                    Next
                Next
            End Using
        End Using


    End Sub


End Class
