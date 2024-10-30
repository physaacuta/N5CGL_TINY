Imports tClassLibrary
Imports tClassLibrary.tClass

Public Class frmPopPrint

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�

    Private mfrm_pMain As frmMain                                       ' �t�H�[���{�� (�|�C���^)
    Private mcls_pRpiView As tClass_ViewRpiThread = Nothing             ' RPI�t�@�C���\���N���X (�|�C���^)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �v���p�e�B

    ''' <summary>
    ''' �t�H�[���{��
    ''' </summary>
    Public Sub SetFrm_Main(ByRef frm As frmMain)
        mfrm_pMain = frm
    End Sub


    ''' <summary>
    ''' RPI�N���X
    ''' </summary>
    ''' <param name="cls">RPI�N���X</param>
    ''' <remarks></remarks>
    Public Sub SetCls_RpiView(ByRef cls As tClass_ViewRpiThread)
        mcls_pRpiView = cls
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ ��{

    ''' <summary>
    ''' ���[�h
    ''' </summary>
    Private Sub frmPopPrint_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        SetFormLocationAtOnce = True
        ' �v�����^�L���m�F
        If Not tClass.tClass_HeadCopy.IsCheckPrinter Then
            btnPrintP.Enabled = False
        Else
            btnPrintP.Enabled = True
        End If
    End Sub


    '/ /////////////////////////////////////////////////////////////////////////////////
    ' ����֌W

    ''' <summary>
    ''' ��ʈ��
    ''' </summary>
    Private Sub btnPrintP_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPrintP.Click
        Dim cls_Hcopy As New tClass.tClass_HeadCopy
        Dim bYoko As Boolean = CBool(IIf(rdbPrint1.Checked, False, True)) ' True:�� False:�c

        ' �����͌����Ȃ��悤�ɂ��Ă���
        Me.Visible = False
        Application.DoEvents()

        Try
            cls_Hcopy.prop_Landscape = bYoko    ' �������
            cls_Hcopy.prop_Strach = True        ' �X�g���b�`
            Call cls_Hcopy.Print(frmMain)          ' �\�����̎���ʂ����

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
    ''' �n�[�h�R�s�[
    ''' </summary>
    Private Sub btnPrintH_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles btnPrintH.Click
        Dim cls_Hcopy As New tClass.tClass_HeadCopy
        Dim strFPath As String
        Dim strMsg As String = ""


        ' �����͌����Ȃ��悤�ɂ��Ă���
        Me.Visible = False
        Application.DoEvents()


        '''' �t���p�X
        'Dim sWk As String = Format(Now, "yyyyMMdd_HHmmss") & ".bmp"
        Dim sWk As String = Format(Now, "yyyyMMdd_HHmmss") & ".jpg"
        strFPath = IO.Path.Combine(Application.StartupPath, sWk)


        '''' �n�[�h�R�s�[
        Try
            'Call cls_Hcopy.HCopy(frmMain, strFPath, Drawing.Imaging.ImageFormat.Bmp)
            Call cls_Hcopy.HCopy(frmMain, strFPath, Drawing.Imaging.ImageFormat.Jpeg)
            strMsg = System.IO.Path.GetFullPath(strFPath) & "�ɉ摜��ۑ����܂����B"
        Catch ex As Exception
            strMsg = "�摜�ۑ��Ɏ��s���܂����B"
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
    ' �t���[���摜�ۑ�
    Enum SaveFrameMethod
        SaveRawAsBmp
        SaveDisplayToJpg
        SaveRaw
        SaveRawAsBmpAsColor
    End Enum
    ''' <summary>
    ''' JPEG�摜�ۑ�
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

        ' �t�@�C��������
        Dim sPath As String
        Dim sNameFormat As String
        If h.IsTHeader Then
            Dim tH As tClass.tClass_ReadRPI.RPI_T_HEADER = mcls_pRpiView.GetRpi_THeader
            sNameFormat = tH.cKizukenNo & "@men=" & tH.nMen & "@ang=" & tH.nCamAngle & "FNo={0}"
        Else
            sNameFormat = "{0}"
        End If


        ' �o��
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
                            '�O�F���̉摜�𓾂�
                            Dim original_ch As Integer = mcls_pRpiView.GetRpi_Cls().PropRpiActiveCh

                            mcls_pRpiView.GetRpi_Cls().PropRpiActiveCh = 0
                            Dim bmp1 As Bitmap = mcls_pRpiView.MTDecoder.DecompressFrame(iframe)
                            mcls_pRpiView.GetRpi_Cls().PropRpiActiveCh = 1
                            Dim bmp2 As Bitmap = mcls_pRpiView.MTDecoder.DecompressFrame(iframe)
                            mcls_pRpiView.GetRpi_Cls().PropRpiActiveCh = 2
                            Dim bmp3 As Bitmap = mcls_pRpiView.MTDecoder.DecompressFrame(iframe)

                            '�������āA1���̉摜�ŕۑ�����
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
                        If b Is Nothing Then b = Buf.pImgRaw '�k������K�v���Ȃ��ꍇ�ApImgRaw�݂̂���B
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
