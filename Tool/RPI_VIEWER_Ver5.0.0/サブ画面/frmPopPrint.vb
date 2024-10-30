Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopPrint

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ メンバー変数

    Private mfrm_pMain As frmMain                                       ' フォーム本体 (ポインタ)
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing             ' RPIファイル表示クラス (ポインタ)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ プロパティ

    ''' <summary>
    ''' フォーム本体
    ''' </summary>
    Public Sub SetFrm_Main(ByRef frm As frmMain)
        mfrm_pMain = frm
    End Sub


    ''' <summary>
    ''' RPIクラス
    ''' </summary>
    ''' <param name="cls">RPIクラス</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ 基本

    ''' <summary>
    ''' ロード
    ''' </summary>
    Private Sub frmPopPrint_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        ' プリンタ有無確認
        If Not tClass.tClass_HeadCopy.IsCheckPrinter Then
            btnPrintP.Enabled = False
        Else
            btnPrintP.Enabled = True
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' 印刷関係

    ''' <summary>
    ''' 画面印刷
    ''' </summary>
    Private Sub btnPrintP_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPrintP.Click
        Dim cls_Hcopy As New tClass.tClass_HeadCopy
        Dim bYoko As Boolean = CBool(IIf(rdbPrint1.Checked, False, True)) ' True:横 False:縦

        ' 自分は見えないようにしておく
        Me.Visible = False
        Application.DoEvents()

        Try
            cls_Hcopy.prop_Landscape = bYoko    ' 印刷方向
            cls_Hcopy.prop_Strach = True        ' ストレッチ
            Call cls_Hcopy.Print(frmMain)          ' 表示中の自画面を印刷

        Catch ex As Exception

        Finally
            Me.Visible = True

            If Not cls_Hcopy Is Nothing Then
                cls_Hcopy.Dispose()
                cls_Hcopy = Nothing
            End If
        End Try
    End Sub

    ''' <summary>
    ''' ハードコピー
    ''' </summary>
    Private Sub btnPrintH_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPrintH.Click
        Dim cls_Hcopy As New tClass.tClass_HeadCopy
        Dim strFPath As String
        Dim strMsg As String = ""


        ' 自分は見えないようにしておく
        Me.Visible = False
        Application.DoEvents()


        '''' フルパス
        'Dim sWk As String = Format(Now, "yyyyMMdd_HHmmss") & ".bmp"
        Dim sWk As String = Format(Now, "yyyyMMdd_HHmmss") & ".jpg"
        strFPath = IO.Path.Combine(Application.StartupPath, sWk)


        '''' ハードコピー
        Try
            'Call cls_Hcopy.HCopy(frmMain, strFPath, Drawing.Imaging.ImageFormat.Bmp)
            Call cls_Hcopy.HCopy(frmMain, strFPath, Drawing.Imaging.ImageFormat.Jpeg)
            strMsg = System.IO.Path.GetFullPath(strFPath) & "に画像を保存しました。"
        Catch ex As Exception
            strMsg = "画像保存に失敗しました。"
        Finally
            Me.Visible = True
            MsgBox(strMsg)

            If Not cls_Hcopy Is Nothing Then
                cls_Hcopy.Dispose()
                cls_Hcopy = Nothing
            End If
        End Try

    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' フレーム画像保存
    Enum SaveFrameMethod
        SaveRawAsBmp
        SaveDisplayToJpg
        SaveRaw
        SaveRawAsBmpAsColor
    End Enum
    ''' <summary>
    ''' JPEG画像保存
    ''' </summary>
    Private Sub btnSaveJpeg_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSaveDisplay.Click
        Call SaveImg(SaveFrameMethod.SaveDisplayToJpg)
    End Sub
    Private Sub btnSaveRawAsBmp_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSaveRawAsBmp.Click
        Call SaveImg(SaveFrameMethod.SaveRawAsBmp)
    End Sub
    Private Sub btnSaveBin_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnSaveBin.Click
        Call SaveImg(SaveFrameMethod.SaveRaw)
    End Sub
    Private Sub SaveImg(ByVal nSaveMode As SaveFrameMethod)
        Dim h As tClass_ReadRPI.RPI_HEADER = mcls_pRpiView.GetRpi_Header()

        ' ファイル名決定
        Dim sPath As String
        Dim sNameFormat As String
        If h.IsTHeader Then
            Dim tH As tClass.tClass_ReadRPI.RPI_T_HEADER = mcls_pRpiView.GetRpi_THeader
            sNameFormat = tH.cKizukenNo & "@men=" & tH.nMen & "@ang=" & tH.nCamAngle & "FNo={0}"
        Else
            sNameFormat = "{0}"
        End If


        ' 出力
        Try
            Dim DspInf As tClass_ViewRpiThread.DSP_ARIA_INF = mcls_pRpiView.GetDspAriaInf()
            For iframe As Integer = DspInf.nDspFno + 1 To DspInf.nDspFnoEnd + 1
                If iframe < 1 Then Continue For
                If iframe > h.nframe Then Continue For


                sPath = String.Format(sNameFormat, iframe)
                Dim Buf As tClass_ViewRpiThread.CLS_IMG_INF = mcls_pRpiView.GetCls_Img_inf(iframe)
                Select Case nSaveMode
                    Case SaveFrameMethod.SaveRawAsBmp, SaveFrameMethod.SaveRawAsBmpAsColor
                        Dim b As Bitmap = Buf.pImgRaw
                        If b Is Nothing Then Continue For

                        If h.nchannel = 3 And Not b.PixelFormat = Imaging.PixelFormat.Format24bppRgb Then
                            '三色分の画像を得る
                            Dim original_ch As Integer = mcls_pRpiView.GetRpi_Cls().PropRpiActiveCh

                            mcls_pRpiView.GetRpi_Cls().PropRpiActiveCh = 0
                            Dim bmp1 As Bitmap = mcls_pRpiView.MTDecoder.DecompressFrame(iframe)
                            mcls_pRpiView.GetRpi_Cls().PropRpiActiveCh = 1
                            Dim bmp2 As Bitmap = mcls_pRpiView.MTDecoder.DecompressFrame(iframe)
                            mcls_pRpiView.GetRpi_Cls().PropRpiActiveCh = 2
                            Dim bmp3 As Bitmap = mcls_pRpiView.MTDecoder.DecompressFrame(iframe)

                            '合成して、1枚の画像で保存する
                            SaveAsPackedJpeg(sPath & ".bmp", bmp1, bmp2, bmp3)


                            bmp1.Dispose()
                            bmp2.Dispose()
                            bmp3.Dispose()

                            mcls_pRpiView.GetRpi_Cls().PropRpiActiveCh = original_ch
                        Else
                            b.Save(sPath & ".bmp", System.Drawing.Imaging.ImageFormat.Jpeg)
                        End If

                    Case SaveFrameMethod.SaveDisplayToJpg
                        Dim b As Bitmap = Buf.pImgDsp
                        If b Is Nothing Then b = Buf.pImgRaw '縮小する必要がない場合、pImgRawのみある。
                        If b Is Nothing Then Continue For
                        b.Save(sPath & ".jpg", System.Drawing.Imaging.ImageFormat.Jpeg)

                    Case SaveFrameMethod.SaveRaw
                        Dim bwk() As Byte = mcls_pRpiView.GetRpi_Cls().ReadFile_ImgByte(iframe)
                        IO.File.WriteAllBytes(sPath & ".bin", bwk)

                End Select

            Next

        Catch ex As Exception
        End Try
    End Sub

    Sub SaveAsPackedJpeg(filename As String, bmp1 As Bitmap, bmp2 As Bitmap, bmp3 As Bitmap)
        Using wbmp As New Bitmap(bmp1.Width, bmp1.Height, Imaging.PixelFormat.Format24bppRgb)

            Using w As New LockingBitmap(wbmp, Imaging.ImageLockMode.WriteOnly), _
              b1 As New LockingBitmap(bmp1, Imaging.ImageLockMode.ReadOnly), _
              b2 As New LockingBitmap(bmp2, Imaging.ImageLockMode.ReadOnly), _
              b3 As New LockingBitmap(bmp3, Imaging.ImageLockMode.ReadOnly)

                For yy As Integer = 0 To wbmp.Height - 1
                    Dim d(bmp1.Width * 3 - 1) As Byte
                    Dim s1() As Byte = b1.ToByteArray(yy)
                    Dim s2() As Byte = b2.ToByteArray(yy)
                    Dim s3() As Byte = b3.ToByteArray(yy)

                    For xx As Integer = 0 To wbmp.Width - 1
                        d(xx * 3 + 0) = s1(xx)
                        d(xx * 3 + 1) = s2(xx)
                        d(xx * 3 + 2) = s3(xx)
                    Next
                    w.CopyFrom(yy, d)
                Next
            End Using

            wbmp.Save(filename)
        End Using
    End Sub

End Class
