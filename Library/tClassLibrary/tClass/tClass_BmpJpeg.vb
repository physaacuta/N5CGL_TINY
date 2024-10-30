'*********************************************************************************
'Bitmap��Jpeg�̉摜�f�[�^�ϊ��N���X
'	[Ver]
'		Ver.01    2007/06/20  vs2005 �Ή�
'
'	[����]
'		����Ӗ��r����p�H�H
'*********************************************************************************
Option Strict On				'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.IO
Imports System.Runtime.InteropServices
Imports System.Drawing
Imports System.Drawing.Imaging


Namespace tClass
    ''' <summary>
    ''' Bitmap��Jpeg�̉摜�f�[�^�ϊ��N���X
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_BmpJpeg

        ''' <summary>�r�b�g�}�b�v�t�@�C���w�b�_</summary>
        Public Const BITMAPFILEHEADER_SIZE As Integer = 14
        <StructLayout(LayoutKind.Sequential, Size:=BITMAPFILEHEADER_SIZE, pack:=1)> _
        Public Structure BITMAPFILEHEADER
            <VBFixedArray(2 - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=2)> _
            Dim bfType As Byte()												' �t�@�C���^�C�v
            Dim bfSize As UInteger												' �t�@�C���T�C�Y [byte]
            Dim bfReserved1 As UShort											' �\�� (0�Œ�)
            Dim bfReserved2 As UShort											' �\�� (0�Œ�)
            Dim bfOffBits As UInteger											' �t�@�C���擪����f�[�^���܂ł̃I�t�Z�b�g [byte]

            ''' <summary>
            ''' ������
            ''' </summary>
            ''' <remarks></remarks>
            Public Sub initialize()
                ReDim bfType(2 - 1)
            End Sub
        End Structure

        ''' <summary>�r�b�g�}�b�v���w�b�_</summary>
        Public Const BITMAPINFOHEADER_SIZE As Integer = 40
        <StructLayout(LayoutKind.Sequential, Size:=BITMAPINFOHEADER_SIZE, pack:=1)> _
        Public Structure BITMAPINFOHEADER
            Dim biSize As UInteger												' ���w�b�_�T�C�Y [byte] (40�Œ�)
            Dim biWidth As Integer												' �摜�̕� [pixel]
            Dim biHeight As Integer												' �摜�̍��� [pixel]
            Dim biPlanes As UShort												' �v���[���� (1�Œ�)
            Dim biBitCount As UShort											' �F�r�b�g�� [bit]
            Dim biCompression As UInteger										' ���k�`��
            Dim biSizeImage As UInteger											' �摜�f�[�^�T�C�Y [byte]
            Dim biXPelsPerMeter As Integer										' �����𑜓x [dot/m]
            Dim biYPelsPerMeter As Integer										' �����𑜓x [dot/m]
            Dim biClrUsed As UInteger											' �i�[�p���b�g��(�g�p�F��)
            Dim biClrImportant As UInteger										' �d�v�F��
        End Structure

        ''' <summary>�J���[�e�[�u��</summary>
        Public Const RGBQUAD_SIZE As Integer = 4
        <StructLayout(LayoutKind.Sequential, Size:=RGBQUAD_SIZE, pack:=1)> _
        Public Structure RGBQUAD
            Dim rgbBlue As Byte													' ��
            Dim rgbGreen As Byte												' ��
            Dim rgbRed As Byte													' ��
            Dim rgbReserved As Byte												' �\��
        End Structure

        ''' <summary>�r�b�g�}�b�v���{��</summary>
        Public Const BITMAPINFO_SIZE As Integer = BITMAPINFOHEADER_SIZE + RGBQUAD_SIZE * 256
        <StructLayout(LayoutKind.Sequential, Size:=BITMAPINFO_SIZE, pack:=1)> _
        Public Structure BITMAPINFO
            Dim bmiHeader As BITMAPINFOHEADER
            <MarshalAs(UnmanagedType.ByValArray, SizeConst:=256)> _
            Dim bmiColors() As RGBQUAD
        End Structure

        '====================================================================================================
        ' ��{����

        ''' <summary>
        ''' �ʏ�̏����߂̉摜�t�@�C���\��
        ''' </summary>
        ''' <param name="strFPath">�t���p�X</param>
        ''' <returns>image�I�u�W�F�N�g</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function ImgFileLoad(ByVal strFPath As String) As Bitmap
            Try
                '' WIC�Ή�
                If WicAccessMgr.CheckWicAccess Then
                    Return WicAccessMgr.ImgFileLoadWic(strFPath)
                End If


                Return New Bitmap(strFPath)
            Catch ex As Exception
                Return Nothing
            Finally
            End Try
        End Function


        ''' <summary>
        ''' Bitmap�t�@�C�������\�� (�摜�t�@�C����2,3M�Ƃ�����ȂƂ��p)
        ''' </summary>
        ''' <param name="strFPath">�t���p�X</param>
        ''' <param name="bCopyPalette">True:�J���[�p���b�g���R�s�[(���܂ɃG���[�ɂȂ�摜������)</param>
        ''' <returns>image�I�u�W�F�N�g</returns>
        ''' <remarks>�����t�@�C���Ǎ��łނ���ނ���\���𑁂����܂��B</remarks>
        Public Overloads Shared Function BmpFileLoad(ByVal strFPath As String, Optional ByVal bCopyPalette As Boolean = False) As Bitmap
            '' Ver1
            '         Dim stream As FileStream
            '         Dim img As Image
            '         stream = IO.File.OpenRead(strFPath)
            'img = Image.FromStream(stream, False, True)		   '��R�������~�\�B�t�@�C�����؂��Ȃ��B �� �K�v�Ȏ��ɁA�����t�@�C������f�[�^��\��
            '         '�t�@�C���N���[�Y������ǉ�
            'stream.Close()

            '         Return img	' ���ꂾ�ƂȂ����A�摜�̔��]�Ƃ�����Ɨ�����ꍇ������


            '' Ver2
            'Dim stream As FileStream
            'Dim img As Image
            'stream = IO.File.OpenRead(strFPath)
            'img = Image.FromStream(stream, False, True)       '��R�������~�\�B�t�@�C�����؂��Ȃ��B �� �K�v�Ȏ��ɁA�����t�@�C������f�[�^��\��
            ''�t�@�C���N���[�Y������ǉ�
            'stream.Close()



            'If Not bCopyPalette Then
            '    ' �ʏ�͂������B�J���[�p���b�g�̓R�s�[����Ȃ��B
            '    Return New Bitmap(img)
            'Else
            '    ' �Ƃ���ƃJ���[�p���b�g�̏���Y��Ă��܂��悤�Ȃ̂ŁA�΍� 
            '    ' �K���}�␳�Ƃ�����Ƃ��́A������B
            '    ' �������A�悭�킩��Ȃ����A�摜�ɂ���ẮA�G���[�ɂȂ�
            '    Dim palet As Drawing.Imaging.ColorPalette = img.Palette
            '    Dim bitmapWk As New Bitmap(img)
            '    bitmapWk.Palette = palet
            '    Return bitmapWk
            'End If


            '' WIC�Ή�         ��������
            Dim img As Image
            If WicAccessMgr.CheckWicAccess Then
                img = WicAccessMgr.ImgFileLoadWic(strFPath)
            Else
                Using stream As FileStream = IO.File.OpenRead(strFPath)
                    img = Bitmap.FromStream(stream, False, True)          '��R�������~�\�B�t�@�C�����؂��Ȃ��B �� �K�v�Ȏ��ɁA�����t�@�C������f�[�^��\��
                    '�t�@�C���N���[�Y������ǉ�
                End Using
            End If


            If Not bCopyPalette Then
                ' �ʏ�͂������B�J���[�p���b�g�̓R�s�[����Ȃ��B
                Return New Bitmap(img)
            Else
                ' �Ƃ���ƃJ���[�p���b�g�̏���Y��Ă��܂��悤�Ȃ̂ŁA�΍� 
                ' �K���}�␳�Ƃ�����Ƃ��́A������B
                ' �������A�悭�킩��Ȃ����A�摜�ɂ���ẮA�G���[�ɂȂ�
                Dim palet As Drawing.Imaging.ColorPalette = img.Palette
                Dim bitmapWk As New Bitmap(img)
                bitmapWk.Palette = palet
                Return bitmapWk
            End If

        End Function

        ''' <summary>
        ''' �F������A�s�N�Z���`����ԋp����N���X
        ''' </summary>
        ''' <param name="nc">�F��(2,16,256)</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Shared Function PixelFormatFromBpp(ByVal nc As Integer) As PixelFormat
            Dim pf As Imaging.PixelFormat
            If 2 = nc Then
                pf = Imaging.PixelFormat.Format1bppIndexed
            ElseIf 16 = nc Then
                pf = Imaging.PixelFormat.Format4bppIndexed
            ElseIf 256 = nc Then
                pf = Imaging.PixelFormat.Format8bppIndexed
            End If
            Return pf
        End Function

        ''' <summary>
        ''' ���m�N���摜�i�f�[�^���̂݁j����ABitmap�Ƃ��Ă��쐬����N���X
        ''' </summary>
        ''' <param name="bW">��������Bitmap��</param>
        ''' <param name="bH">��������Bitmap����</param>
        ''' <param name="dataBmp">�o�C�i���f�[�^(4�o�C�g�X�g���C�h)</param>
        ''' <param name="nc">�F��(2,16,256)</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function DataToBmp(ByVal bW As Integer, ByVal bH As Integer, ByVal dataBmp As IntPtr, Optional ByVal nc As Integer = 256) As Drawing.Bitmap
            '' �o�͂���Bitmap����
            Dim bitmap As New Bitmap(bW, bH, PixelFormatFromBpp(nc))     '���m�N���摜�p

            '' �p���b�g�Z�b�g
            SetGrayPalette(bitmap)

            '' Bitmap�f�[�^�Z�b�g
            Using bd As New LockingBitmap(bitmap, ImageLockMode.WriteOnly)
                RtlMoveMemory(bd.PtrAt(), dataBmp, New IntPtr(bd.Stride * bH))
            End Using

            Return bitmap
        End Function

        ''' <summary>
        ''' ���m�N���摜�i�f�[�^���̂݁j����ABitmap�Ƃ��Ă��쐬����N���X
        ''' </summary>
        ''' <param name="bW">��������Bitmap��</param>
        ''' <param name="bH">��������Bitmap����</param>
        ''' <param name="dataBmp">�o�C�i���f�[�^</param>
        ''' <param name="nc">�F��(2,16,256)</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function DataToBmp(ByVal bW As Integer, ByVal bH As Integer, ByVal dataBmp() As Byte, Optional ByVal nc As Integer = 256) As Drawing.Bitmap
            '' �o�͂���Bitmap����
            Dim bitmap As New Bitmap(bW, bH, PixelFormatFromBpp(nc))     '���m�N���摜�p

            '' �p���b�g�Z�b�g
            SetGrayPalette(bitmap)

            '' Bitmap�f�[�^�Z�b�g
            Using bd As New LockingBitmap(bitmap, ImageLockMode.WriteOnly)
                System.Runtime.InteropServices.Marshal.Copy(dataBmp, 0, bd.PtrAt(), bd.Stride * bH)
            End Using

            Return bitmap
        End Function



        <DllImport("KERNEL32", SetLastError:=True, CallingConvention:=CallingConvention.Winapi)> _
        Public Shared Function RtlMoveMemory(dst As IntPtr, src As IntPtr, len As IntPtr) As Integer
        End Function



        ''' <summary>
        ''' BMP�t�@�C���̃w�b�_�{�f�[�^������ABitmap�C���X�^���X���쐬����
        ''' </summary>
        Public Overloads Shared Function BMPDataToBitmap(ByVal bmp() As Byte) As Bitmap
            '�z����|�C���^������B
            Using g1 As New PinnedPtr(bmp)
                Return BMPDataToBitmap(g1.Ptr)
            End Using
                
        End Function

        ''' <summary>
        ''' BMP�t�@�C���̃w�b�_�{�f�[�^������ABitmap�C���X�^���X���쐬����
        ''' </summary>
        Public Overloads Shared Function BMPDataToBitmap(ByVal srcptr As IntPtr) As Bitmap

   
            Dim bf As BITMAPFILEHEADER = tMod.ReadStructure(Of BITMAPFILEHEADER)(srcptr, 0)
            Dim bi As BITMAPINFOHEADER = tMod.ReadStructure(Of BITMAPINFOHEADER)(srcptr, BITMAPFILEHEADER_SIZE)


            Dim pf As Imaging.PixelFormat
            Select Case bi.biBitCount
                Case 1
                    pf = Imaging.PixelFormat.Format1bppIndexed
                Case 4
                    pf = Imaging.PixelFormat.Format4bppIndexed
                Case 8
                    pf = Imaging.PixelFormat.Format8bppIndexed
                Case 24
                    pf = Imaging.PixelFormat.Format24bppRgb
                Case 32
                    pf = Imaging.PixelFormat.Format32bppRgb
                Case Else
                    Return Nothing
            End Select

            Dim nW As Integer = bi.biWidth
            Dim nH As Integer = Math.Abs(bi.biHeight)
            Dim bReverseHeight As Boolean = bi.biHeight > 0

            '' �o�͂���Bitmap����
            Dim bitmap As New Bitmap(nW, nH, pf)

            If bi.biBitCount <= 8 Then
                '' �p���b�g�Z�b�g( Or Load Actureal Palette?)
                SetGrayPalette(bitmap)
            End If

           '' Bitmap�f�[�^�Z�b�g
            Using bd As New LockingBitmap(bitmap, ImageLockMode.WriteOnly)
                If bReverseHeight Then
                    For y As Integer = 0 To nH - 1
                        Dim src_addr As New IntPtr(srcptr.ToInt64() + CInt(bf.bfOffBits) + bd.Stride * (nH - y - 1))
                        RtlMoveMemory(bd.PtrAt(y), src_addr, New IntPtr(bd.Stride))
                    Next
                Else
                    Dim src_addr As New IntPtr(srcptr.ToInt64() + CInt(bf.bfOffBits))
                    RtlMoveMemory(bd.PtrAt(), src_addr, New IntPtr(bd.Stride * nH))
                End If
            End Using

            Return bitmap


        End Function

        ''' <summary>
        ''' �J���[�p���b�g�𐶐�
        ''' </summary>
        ''' <remarks></remarks>
        Public Shared Sub SetGrayPalette(ByVal bitmap As Image)
            Dim nColor As Integer = 256
            Select Case bitmap.PixelFormat
                Case Imaging.PixelFormat.Format8bppIndexed
                    nColor = 256
                Case Imaging.PixelFormat.Format4bppIndexed
                    nColor = 16
                Case Imaging.PixelFormat.Format1bppIndexed
                    nColor = 2
            End Select


            Dim palette As Imaging.ColorPalette = bitmap.Palette
            Dim alpha As Integer                   ' �����x
            Dim intensity As Integer
            '' �J���[�p���b�g�̃f�[�^�Z�b�g
            For ii As Integer = 0 To nColor - 1
                alpha = &HFF
                intensity = (ii * &HFF) \ (nColor - 1)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next ii


            bitmap.Palette = palette
        End Sub

        ''' <summary>
        ''' Jpeg�f�[�^����C���[�W�N���X�𐶐�
        ''' </summary>
        ''' <param name="dataJpeg"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function DataToImage(ByVal dataJpeg() As Byte) As Drawing.Bitmap
            Try
                '' WIC�Ή�
                If WicAccessMgr.CheckWicAccess Then
                    Return WicAccessMgr.DataToImageWic(dataJpeg)
                End If

                Return New Bitmap(New System.IO.MemoryStream(dataJpeg))
            Catch ex As Exception
                Return Nothing
            Finally

            End Try
        End Function

        ''' <summary>
        ''' �J���[�p���b�g�𐶐�
        ''' </summary>
        ''' <param name="nColor">�K��</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Protected Shared Function GetColortPalette(ByVal nColor As Integer) As Drawing.Imaging.ColorPalette
            Dim pixFormat As Drawing.Imaging.PixelFormat        '�J���[�f�[�^�`��
            If nColor > 16 Then
                pixFormat = Imaging.PixelFormat.Format8bppIndexed
            ElseIf nColor > 2 Then
                pixFormat = Imaging.PixelFormat.Format4bppIndexed
            Else
                pixFormat = Imaging.PixelFormat.Format1bppIndexed
            End If

            '' �J���[�p���b�g����
            Dim bitmap As New Bitmap(1, 1, pixFormat)
            Dim palette As Imaging.ColorPalette
            palette = bitmap.Palette
            bitmap.Dispose()

            Dim alpha As Integer                   ' �����x
            Dim intensity As Integer

            '' �J���[�p���b�g�̃f�[�^�Z�b�g
            For ii As Integer = 0 To nColor - 1
                alpha = &HFF
                intensity = (ii * &HFF) \ (nColor - 1)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next ii


            Return palette
        End Function

        ''' <summary>
        ''' Image�N���X(���ۂɂ͔h�����Bitmap�N���X���g�p)����r�b�g�}�b�v�f�[�^�����̃o�C�g�z��ɕϊ�
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
         Public Overloads Shared Function ImagToData(ByVal objBmp As Bitmap) As Byte()
            ' BitmapData�։摜�f�[�^�������Z�b�g�������������b�N����
            Using bd As New LockingBitmap(objBmp, ImageLockMode.ReadOnly)

                '�r�b�g�}�b�v�p�o�C�g�z��̃T�C�Y���摜�ɍ��킹��
                Dim bytBitmap(bd.Height * bd.Stride - 1) As Byte

                '�r�b�g�}�b�v�̐擪�A�h���X����o�C�g�z��։摜�f�[�^��ǂݍ���
                System.Runtime.InteropServices.Marshal.Copy(bd.PtrAt(), bytBitmap, 0, bd.Height * bd.Stride)

                Return bytBitmap
                '���������A�����b�N
            End Using
        End Function

        ''' <summary>
        ''' Jpeg�摜�̃o�C�g�z����r�b�g�}�b�v�f�[�^�����̃o�C�g�z��ɕϊ�
        ''' </summary>
        ''' <param name="bytJpeg">Jpeg�摜�̃o�C�g�z��</param>
        ''' <returns>�r�b�g�}�b�v�f�[�^�����̃o�C�g�z��</returns>
        ''' <remarks>�A���}�l�[�W�R�[�h���g�p�A512*256�̉摜�Ŗ�5ms������</remarks>
        Public Overloads Shared Function JpegToData(ByVal bytJpeg() As Byte) As Byte()
            'Dim ms As IO.MemoryStream = New IO.MemoryStream(bytJpeg)    'jpg�̃o�C�g�f�[�^�Ǎ��p�X�g���[��
            'Dim bmp As Bitmap = New Bitmap(ms)                          '�r�b�g�}�b�v�ɃZ�b�g  �������������d���� jpeg����ϊ����Ă��邩��d���������H
            Dim bmp As Bitmap = DataToImage(bytJpeg)

            Try
                Return ImagToData(bmp)
            Catch ex As Exception
                Return Nothing
            Finally
                bmp.Dispose()
            End Try
        End Function

        ''' <summary>
        ''' Jpeg�t�@�C�����r�b�g�}�b�v�f�[�^�����̃o�C�g�z��ɕϊ�
        ''' </summary>
        ''' <param name="strJpegFilePath">Jpeg�t�@�C���t�@�C���p�X</param>
        ''' <returns>�r�b�g�}�b�v�f�[�^�����̃o�C�g�z��</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function JpegToData(ByVal strJpegFilePath As String) As Byte()
            '' WIC�Ή�
            'Dim bmp As Bitmap = New Bitmap(strJpegFilePath)							 '�r�b�g�}�b�v�ɃZ�b�g  �������������d���� jpeg����ϊ����Ă��邩��d���������H
            Dim bmp As Bitmap
            If WicAccessMgr.CheckWicAccess Then
                bmp = WicAccessMgr.ImgFileLoadWic(strJpegFilePath)
            Else
                bmp = New Bitmap(strJpegFilePath)
            End If

            Try
                Return ImagToData(bmp)
            Catch ex As Exception
                Return Nothing
            Finally
                bmp.Dispose()
            End Try
        End Function

        ''' <summary>
        ''' Bitmap�摜����Jpeg�摜�ɕϊ�����memstream�ŕԂ�
        ''' </summary>
        ''' <param name="bmp">Bitmap�摜</param>
        ''' <param name="quality">�i��</param>
        ''' <returns>Jpeg�ɕϊ������o�C�g�z��</returns>
        Public Overloads Shared Function BmpToJpegData(ByVal bmp As Bitmap, ByVal quality As Integer) As MemoryStream
            Dim ms As New MemoryStream

            ' �G���R�[�f�B���O�w��
            Dim eps As New System.Drawing.Imaging.EncoderParameters(1)
            eps.Param(0) = New System.Drawing.Imaging.EncoderParameter(System.Drawing.Imaging.Encoder.Quality, quality)

            ' �G���R�[�_�[
            Dim encs() As System.Drawing.Imaging.ImageCodecInfo = System.Drawing.Imaging.ImageCodecInfo.GetImageEncoders()
            Dim enc As System.Drawing.Imaging.ImageCodecInfo = Nothing
            For Each enc In encs
                If enc.FormatDescription = "JPEG" Then
                    Exit For
                End If
            Next

            ' �ϊ�
            bmp.Save(ms, enc, eps)
            Return ms


            '''' �e�X�g
            'Dim b() As Byte = ms.ToArray
            'IO.File.WriteAllBytes("c:\bbb.jpg", b)
        End Function

        '====================================================================================================
        ' �g������

        ''' <summary>
        ''' �K���}�␳��̋P�x�l���擾
        ''' </summary>
        ''' <param name="nIntensity">���̋P�x�l</param>
        ''' <param name="dGamma">�K���}�␳�l (> 0.0)</param>
        ''' <returns>�K���}�␳��̋P�x�l</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetIntensityGamma(ByVal nIntensity As Integer, ByVal dGamma As Double) As Integer

            Return CInt(Int(((nIntensity / (256 - 1)) ^ (1 / dGamma)) * (256 - 1)))             ' �����_�ȉ��͐؎̂�
        End Function

        ''' <summary>
        ''' �K���}�␳�K�p�J���[�p���b�g�𐶐�
        ''' </summary>
        ''' <param name="dGamma">�K���}�␳�l (> 0.0)
        ''' dGamma �� 1.0 �F �Â������ւ̕␳
        ''' dGamma �� 1.0 �F �f�t�H���g�̖��邳
        ''' dGamma �� 1.0 �F ���邢�����ւ̕␳
        ''' </param>
        ''' <returns>�K���}�␳�K�p�J���[�p���b�g</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteGamma(ByVal dGamma As Double) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' �����x
            Dim intensity As Integer

            '' �J���[�p���b�g����
            palette = GetColortPalette(256)         ' 256�K���̃J���[�p���b�g����

            '' �K���}�␳
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensityGamma(ii, dGamma)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' �K���}�␳
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <param name="dGamma">�K���}�␳�l (> 0.0)
        ''' dGamma �� 1.0 �F �Â������ւ̕␳
        ''' dGamma �� 1.0 �F �f�t�H���g�̖��邳
        ''' dGamma �� 1.0 �F ���邢�����ւ̕␳
        ''' </param>
        ''' <returns>True:���� False:���s</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GammaCmpBmp(ByRef objBmp As Bitmap, ByVal dGamma As Double) As Boolean

            '' 256�K���̃J���[�p���b�g�̂ݑΏ�
            If 256 <> objBmp.Palette.Entries.Length Then
                Return False
            End If

            '' �K���}�␳�l�͈̔̓`�F�b�N
            If dGamma <= 0.0 Then
                Return False
            End If

            '' �K���}�␳
            objBmp.Palette = GetColorPaletteGamma(dGamma)

            Return True
        End Function

        ''' <summary>
        ''' Bitmap�w�b�_�T�C�Y�擾
        ''' </summary>
        ''' <returns>Bitmap�w�b�_�T�C�Y</returns>
        ''' <remarks></remarks>
        Public Shared Function GetBitmapHeaderSize_Gray() As Integer
            Return BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE + RGBQUAD_SIZE * 256
        End Function

        Public Overloads Shared Function CreateBitmapHeader_Gray( _
                    ByVal nWidth As Integer, _
                    ByVal nHeight As Integer, _
                    ByVal aryBmpData() As Byte, _
                    Optional ByVal bRevTB As Boolean = False) As Integer

            Using pinptr As New PinnedPtr(aryBmpData)
                Return CreateBitmapHeader_Gray(nWidth, nHeight, pinptr.Ptr, bRevTB)
            End Using

        End Function

        ''' <summary>
        ''' �O���[�摜��Bitmap�w�b�_����
        ''' </summary>
        ''' <param name="nWidth">�摜��</param>
        ''' <param name="nHeight">�摜����</param>
        ''' <param name="aryBmpData">Bitmap�w�b�_</param>
        ''' <param name="bRevTB">�㉺���]�t���O True:���]���� False:���]���Ȃ�</param>
        ''' <returns>���A��� 0;���� 1�`:�e���G���[</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function CreateBitmapHeader_Gray( _
        ByVal nWidth As Integer, _
        ByVal nHeight As Integer, _
                    ByVal aryBmpData As IntPtr, _
        Optional ByVal bRevTB As Boolean = False) As Integer
            Dim nBitCount As UShort = 8                                     ' �r�b�g�̐[��

            '' �t�@�C���w�b�_�쐬
            Dim bmpFile As New BITMAPFILEHEADER
            bmpFile.initialize()
            With bmpFile
                .bfType = tMod.StringToByte("BM")                           ' �t�@�C�����ʎq ("BM"�Œ�)
                .bfSize = CUInt((nWidth * nHeight * nBitCount / 8) + GetBitmapHeaderSize_Gray())
                .bfOffBits = CUInt(GetBitmapHeaderSize_Gray())              ' �r�b�g�}�b�v�C���[�W�f�[�^�J�n�ʒu
            End With

            '' ���w�b�_�쐬
            Dim bmpInfo As New BITMAPINFOHEADER
            With bmpInfo
                .biSize = 40                                                ' �w�b�_�T�C�Y(40�Œ�)
                .biWidth = nWidth                                           ' �C���[�W�̉���(�s�N�Z���P��)
                .biHeight = CInt(IIf(bRevTB, nHeight, -nHeight))            ' �C���[�W�̍���(�s�N�Z���P��) (Windows�͉摜���㉺���]�̈�)
                .biPlanes = 1                                               ' �v���[����(1�Œ�)
                .biBitCount = nBitCount                                     ' �P�s�N�Z��������̃r�b�g��(1,4,8,24)
                .biCompression = 0                                          ' ���k�`��(BI_RGB:0->�񈳏k, BI_RLE8:1->RLE���k)
                .biSizeImage = CUInt(nWidth * nHeight * nBitCount / 8)      ' ���k��̃C���[�W�T�C�Y(�o�C�g�P��)
                .biClrUsed = 0                                              ' �i�[�p���b�g��(�g�p�F��) (0�Œ�)
                .biClrImportant = 0                                         ' �d�v�F�� (0�Œ�)
            End With


            tMod.WriteStructure(aryBmpData, 0, bmpFile)
            tMod.WriteStructure(aryBmpData, BITMAPFILEHEADER_SIZE, bmpInfo)
            '' �J���[�p���b�g�쐬
            For ii As Integer = 0 To 256 - 1
                Dim rgb As New RGBQUAD
                rgb.rgbBlue = CByte(ii)
                rgb.rgbRed = CByte(ii)
                rgb.rgbGreen = CByte(ii)
                tMod.WriteStructure(aryBmpData, BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE + RGBQUAD_SIZE * ii, rgb)
            Next

            Return 0
        End Function

        ''' <summary>
        ''' �r�b�g�}�b�v�t�@�C����ۑ����܂�
        ''' </summary>
        ''' <param name="strPath">�ۑ���t�@�C���p�X</param>
        ''' <param name="aryHead">�r�b�g�}�b�v�w�b�_��</param>
        ''' <param name="aryData">�r�b�g�}�b�v�f�[�^��</param>
        ''' <returns>True:�ۑ����� False:�ۑ����s</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function BmpFileSave(ByVal strPath As String, ByVal aryHead() As Byte, ByVal aryData() As Byte) As Boolean
            Dim out As FileStream = Nothing
            Try
                out = IO.File.OpenWrite(strPath)
                out.Write(aryHead, 0, aryHead.Length)
                out.Write(aryData, 0, aryData.Length)
            Catch ex As Exception
                Return False
            Finally
                If out IsNot Nothing Then
                    out.Close()
                End If
            End Try

            Return True
        End Function

        ''' <summary>
        ''' �r�b�g�}�b�v�t�@�C����ۑ����܂�
        ''' </summary>
        ''' <param name="strPath">�ۑ���t�@�C���p�X</param>
        ''' <param name="bmp">�r�b�g�}�b�v�N���X</param>
        ''' <param name="bRevTB">�㉺���]�t���O True:���]���� False:���]���Ȃ�</param>
        ''' <returns>True:�ۑ����� False:�ۑ����s</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function BmpFileSave(ByVal strPath As String, ByVal bmp As Bitmap, Optional ByVal bRevTB As Boolean = False) As Boolean
            Dim aryHead() As Byte
            Dim nHeadSize As Integer = GetBitmapHeaderSize_Gray()
            ReDim aryHead(nHeadSize - 1)

            CreateBitmapHeader_Gray(bmp.Width, bmp.Height, aryHead, bRevTB)
            Dim aryData() As Byte = ImagToData(bmp)

            Dim bRet As Boolean = BmpFileSave(strPath, aryHead, aryData)

            Return bRet
        End Function

        ''' <summary>
        ''' �R���g���X�g������̋P�x�l���擾
        ''' </summary>
        ''' <param name="nIntensity">���̋P�x�l</param>
        ''' <param name="nContrastX">�R���g���X�g�␳�l(0�`255)</param>
        ''' <param name="nBright">���邳�␳�l(0�`255)</param>
        ''' <returns>�R���g���X�g������̋P�x�l</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetIntensityContrast(ByVal nIntensity As Integer, ByVal nContrastX As Integer, ByVal nBright As Integer) As Integer
            Dim nMax As Integer         '�ő�P�x
            Dim nMin As Integer         '�ŏ��P�x
            Dim dContrast As Double     '�R���g���X�g
            Dim nVal As Integer         '������̋P�x�l�p���[�N

            nMax = nContrastX + 128
            nMin = 128 - nContrastX
            dContrast = (nMax - nMin) / (nMax + nMin)

            nVal = CInt(Int((nIntensity - 128) * dContrast) + (nBright * 2 - 128))

            '�㉺���`�F�b�N
            If nVal < 0 Then nVal = 0
            If nVal > 255 Then nVal = 255

            Return nVal

        End Function

        ''' <summary>
        ''' �R���g���X�g�����K�p�J���[�p���b�g�𐶐�
        ''' </summary>
        ''' <param name="nContrastX">�R���g���X�g�␳�l(0�`255)</param>
        ''' <param name="nBright">���邳�␳�l(0�`255)</param>
        ''' <returns>�R���g���X�g�����K�p�J���[�p���b�g</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteContrast(ByVal nContrastX As Integer, ByVal nBright As Integer) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' �����x
            Dim intensity As Integer

            '' �J���[�p���b�g����
            palette = GetColortPalette(256)         ' 256�K���̃J���[�p���b�g����

            '' �R���g���X�g����
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensityContrast(ii, nContrastX, nBright)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' �R���g���X�g����
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <param name="nContrastX">�R���g���X�g�␳�l(0�`255)</param>
        ''' <param name="nBright">���邳�␳�l(0�`255)</param>
        ''' <returns>True:���� False:���s</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function ContrastCmpBmp(ByRef objBmp As Bitmap, ByVal nContrastX As Integer, ByVal nBright As Integer) As Boolean

            '' 256�K���̃J���[�p���b�g�̂ݑΏ�
            If 256 <> objBmp.Palette.Entries.Length Then
                Return False
            End If

            '' �R���g���X�g����
            objBmp.Palette = GetColorPaletteContrast(nContrastX, nBright)

            Return True
        End Function

        ''' <summary>
        ''' �V�O���C�h�␳��̋P�x�l���擾
        ''' </summary>
        ''' <param name="nIntensity">���̋P�x�l</param>
        ''' <param name="nKidoX">��l</param>
        ''' <param name="nKeisu">�W��</param>
        ''' <returns>�V�O���C�h�␳��̋P�x�l</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetIntensitySigmoid(ByVal nIntensity As Integer, ByVal nKidoX As Integer, ByVal nKeisu As Integer) As Integer

            Dim dSigmoid As Double

            '�O���΍�
            If 0 = nKidoX Then
                Return 255
            End If

            '�ʏ��
            'dSigmoid = 1 / (1 + mc_Sigmoid_e ^ (-1 * nKeisu * (nIntensity - nKidoX) / nKidoX))
            '�����ł��邪�A��l(nKidoX)�����ɃO���t���ς�邽�߈ȉ��̂悤�ɕ�����B
            If nIntensity < nKidoX Then
                dSigmoid = 1 / (1 + Math.E ^ (-1 * nKeisu * (nIntensity - nKidoX) / nKidoX))
            ElseIf nIntensity = nKidoX Then
                dSigmoid = 0.5
            Else
                dSigmoid = 1 / (1 + Math.E ^ (-1 * nKeisu * (nIntensity - nKidoX) / (255 - nKidoX)))
            End If

            Return CInt(Int(255 * dSigmoid))

        End Function

        ''' <summary>
        ''' �V�O���C�h�␳�K�p�J���[�p���b�g�𐶐�
        ''' </summary>
        ''' <param name="nKidoX">��l</param>
        ''' <param name="nKeisu">�W��</param>
        ''' <returns>�V�O���C�h�␳�K�p�J���[�p���b�g</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteSigmoid(ByVal nKidoX As Integer, ByVal nKeisu As Integer) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' �����x
            Dim intensity As Integer

            '' �J���[�p���b�g����
            palette = GetColortPalette(256)         ' 256�K���̃J���[�p���b�g����

            '' �V�O���C�h�␳
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensitySigmoid(ii, nKidoX, nKeisu)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' �V�O���C�h�␳
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <param name="nKidoX">��l</param>
        ''' <param name="nKeisu">�W��</param>
        ''' <returns>True:���� False:���s</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function SigmoidCmpBmp(ByRef objBmp As Bitmap, ByVal nKidoX As Integer, ByVal nKeisu As Integer) As Boolean

            '' 256�K���̃J���[�p���b�g�̂ݑΏ�
            If 256 <> objBmp.Palette.Entries.Length Then
                Return False
            End If

            '' �V�O���C�h�␳
            objBmp.Palette = GetColorPaletteSigmoid(nKidoX, nKeisu)

            Return True
        End Function

        ''' <summary>
        ''' �Q�l���␳��̋P�x�l���擾
        ''' </summary>
        ''' <param name="nIntensity">���̋P�x�l</param>
        ''' <param name="nBinari01">臒l�P</param>
        ''' <param name="nBinari02">臒l�Q</param>
        ''' <returns>�Q�l���␳��̋P�x�l</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetIntensityBinari(ByVal nIntensity As Integer, ByVal nBinari01 As Integer, ByVal nBinari02 As Integer) As Integer

            '臒l�P��臒l�Q���r
            If nBinari01 > nBinari02 Then
                If nBinari01 >= nIntensity And nBinari02 <= nIntensity Then
                    Return 255
                Else
                    Return 0
                End If
            Else
                If nBinari02 >= nIntensity And nBinari01 <= nIntensity Then
                    Return 255
                Else
                    Return 0
                End If
            End If

        End Function

        ''' <summary>
        ''' �Q�l���␳�K�p�J���[�p���b�g�𐶐�
        ''' </summary>
        ''' <param name="nBinari01">臒l�P</param>
        ''' <param name="nBinari02">臒l�Q</param>
        ''' <returns>�Q�l���␳�K�p�J���[�p���b�g</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteBinari(ByVal nBinari01 As Integer, ByVal nBinari02 As Integer) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' �����x
            Dim intensity As Integer

            '' �J���[�p���b�g����
            palette = GetColortPalette(256)         ' 256�K���̃J���[�p���b�g����

            '' �Q�l���␳
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensityBinari(ii, nBinari01, nBinari02)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' �Q�l���␳�K�p�J���[�p���b�g�𐶐�
        ''' </summary>
        ''' <param name="nBinari01">臒l�P</param>
        ''' <param name="nBinari02">臒l�Q</param>
        ''' <returns>�Q�l���␳�K�p�J���[�p���b�g</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function GetColorPaletteBinari(ByRef objBmp As Bitmap, ByVal nBinari01 As Integer, ByVal nBinari02 As Integer) As Drawing.Imaging.ColorPalette
            Dim palette As Imaging.ColorPalette
            Dim alpha As Integer                    ' �����x
            Dim intensity As Integer

            '' �J���[�p���b�g����
            palette = GetColortPalette(256)         ' 256�K���̃J���[�p���b�g����

            '' �Q�l���␳
            For ii As Integer = 0 To 256 - 1
                alpha = &HFF
                intensity = GetIntensityBinari(objBmp.Palette.Entries(ii).R, nBinari01, nBinari02)
                palette.Entries(ii) = Color.FromArgb(alpha, intensity, intensity, intensity)
            Next

            Return palette
        End Function

        ''' <summary>
        ''' �Q�l���␳
        ''' </summary>
        ''' <param name="objBmp"></param>
        ''' <param name="nBinari01">臒l�P</param>
        ''' <param name="nBinari02">臒l�Q</param>
        ''' <returns>True:���� False:���s</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function BinariCmpBmp(ByRef objBmp As Bitmap, ByVal nBinari01 As Integer, ByVal nBinari02 As Integer) As Boolean

            '' 256�K���̃J���[�p���b�g�̂ݑΏ�
            If 256 <> objBmp.Palette.Entries.Length Then
                Return False
            End If

            '' �Q�l���␳
            objBmp.Palette = GetColorPaletteBinari(objBmp, nBinari01, nBinari02)

            Return True
        End Function

    End Class



    ''' <summary>
    ''' �r�b�g�}�b�v��LockData��ێ�����N���X
    ''' </summary>
    ''' <remarks>�K���A�uUsing�\���Ŏg���vor�uDispose��Y�ꂸ�Ăԁv����</remarks>
    Public Class LockingBitmap
        Implements IDisposable

        Private bmp As Bitmap
        Private bd As BitmapData
        Private pixelformat As PixelFormat

        ''' <summary>
        ''' �摜�̕�
        ''' </summary>
        Public ReadOnly Property Width() As Integer
            Get
                Return bmp.Width
            End Get
        End Property

        ''' <summary>
        ''' �摜�̍���
        ''' </summary>
        Public ReadOnly Property Height() As Integer
            Get
                Return bmp.Height
            End Get
        End Property

        ''' <summary>
        ''' �摜�́A���̃o�C�g���B�i�p�f�B���O���܂ށj
        ''' </summary>
        Public ReadOnly Property Stride() As Integer
            Get
                Return bd.Stride
            End Get
        End Property

        ''' <summary>
        ''' �w����W�̉�f�̃|�C���^��Ԃ�
        ''' </summary>
        ''' <param name="y">Y���W</param>
        ''' <param name="x">X���W</param>
        ''' <returns>��f�̃|�C���^</returns>
        ''' <remarks></remarks>
        Public Function PtrAt(Optional ByVal y As Integer = 0, Optional ByVal x As Integer = 0) As IntPtr
            If IntPtr.Size = 8 Then
                Return New IntPtr(bd.Scan0.ToInt64() + y * bd.Stride + x)
            Else
                Return New IntPtr(bd.Scan0.ToInt32() + y * bd.Stride + x)
            End If
        End Function

        ''' <summary>
        ''' �w����W����A�w�蕝���̉摜��z��ŕԂ�
        ''' </summary>
        ''' <param name="y">Y���W</param>
        ''' <param name="x">X���W</param>
        ''' <param name="width">�擾���镝</param>
        ''' <returns>��f�̔z��</returns>
        ''' <remarks>1byte�����̒[�����ł�悤�ȕ����w�肵�Ȃ����� ( 1bpp�ŁAwidth=20�Ƃ��j</remarks>
        Public Function ToByteArray(ByVal y As Integer, Optional ByVal x As Integer = 0, Optional ByVal width As Integer = 0) As Byte()
            If width = 0 Then
                width = Me.Width
            End If

            Dim byte_width As Integer = bd.Stride * width \ bd.Width
            Dim rv(byte_width - 1) As Byte
            Marshal.Copy(PtrAt(y, x), rv, 0, byte_width)
            Return rv
        End Function

        ''' <summary>
        ''' �摜�̃��C�����A�o�C�g�z�񂩂�R�s�[���ď���������
        ''' </summary>
        ''' <param name="y">y���W</param>
        ''' <param name="src">�R�s�[���z��</param>
        Public Sub CopyFrom(ByVal y As Integer, src As Byte())
            Marshal.Copy(src, 0, PtrAt(y), src.Length)
        End Sub


        ''' <summary>
        ''' �R���X�g���N�^�B�摜�����b�N����
        ''' </summary>
        ''' <param name="Bmp">�摜(�r�b�g�}�b�v)</param>
        ''' <param name="lockmode">���b�N����(��������/�ǂݍ���)</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal Bmp As Bitmap, ByVal lockmode As ImageLockMode)

            Me.bmp = Bmp
            Me.pixelformat = Bmp.PixelFormat
            Me.bd = Bmp.LockBits(New Rectangle(0, 0, Bmp.Width, Bmp.Height), lockmode, pixelformat)
        End Sub


        ''' <summary>
        ''' �r�b�g�}�b�v�̃��b�N����������B�ȍ~�͎g�p�s��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            If bd IsNot Nothing Then
                bmp.UnlockBits(bd)
                bd = Nothing
                bmp = Nothing
            End If
        End Sub
    End Class
End Namespace

