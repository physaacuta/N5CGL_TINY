'*********************************************************************************
'�s�N�`���[�{�b�N�X�̌��摜�𕔕��I�Ɋg��\������N���X
'	[Ver]
'		Ver.01    2008/11/21  vs2005 �Ή�
'		Ver.02	  2015/11/2   �����g�咆�ɍ��N���b�N�ŃN���b�v�{�[�h�ւ̃R�s�[�@�\�ǉ�
'
'	[����]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.Windows.Forms
Imports System.Drawing

Imports tClassLibrary.tClass


Namespace tClass

    ''' <summary>
    ''' �s�N�`���[�{�b�N�X�̉摜�𕔕��I�Ɋg��\������N���X
    ''' </summary>
    Public Class tClass_ImageRawCutDsp
        Implements IDisposable                          ' �f�X�g���N�^��`

        Public Shared ReadOnly MAX_WINDOW_SIZE As Integer = 1000
        Public Shared ReadOnly MAX_ZOOM As Double = 20.0
        Public Shared ReadOnly MIN_ZOOM As Double = 1.0

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �C�x���g
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �\���f�[�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Event tEv_DispDataSet(ByRef tCls_Calc As tClass_ImageDsp_Calc, ByRef img() As Image)


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �萔��`
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>�������ݒ���</summary>
        Public Structure MEMORI_INF
            ''' <summary>�{���\���L�� (true:�L, false:��)</summary>
            Dim bIsDspRitu As Boolean
            ''' <summary>�������\���L�� (true:�L, false:��)</summary>
            Dim bIsDspMemori As Boolean
            ''' <summary>�\���F</summary>
            Dim Color As Color

            ''' <summary>X���̃������\���Ԋu[pixel]</summary>
            Dim nInteralX As Integer
            ''' <summary>Y���̃������\���Ԋu[pixel]</summary>
            Dim nInteralY As Integer


            ''' <summary>�f�t�H���g��Ԑݒ�</summary>
            Public Sub initialize()
                bIsDspRitu = True
                bIsDspMemori = True
                Color = Drawing.Color.Cyan
                nInteralX = 80
                nInteralY = 80
            End Sub
        End Structure


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����o�[�ϐ�
        '/ /////////////////////////////////////////////////////////////////////////////////
        'Private m_BaseFrm As Form                  ' �x�[�X�t�H�[��
        Private m_BaseFrm As Control                ' �x�[�X�t�H�[��
        Private m_BasePic As PictureBox             ' �x�[�X�s�N�`���[�{�b�N�X
        Private m_pic As PictureBox                 ' �g��\���G���A

        Private m_bDspStop As Boolean = False       ' �g��\���@�\�ꎞ��~
        Private m_bDspOn As Boolean = False         ' �g��\����
        Private m_dDspRitu As Double                ' �g�嗦
        Private m_MemoriInf As MEMORI_INF           ' ���������


        '''' ������̃f�[�^
        Private mcls_pCalc As tClass_ImageDsp_Calc  ' �摜�v�Z�N���X (�|�C���^)
        Private m_pDspImg() As Image                        ' ���摜�z��


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �v���p�e�B
        '/ /////////////////////////////////////////////////////////////////////////////////


        ''' <summary>
        ''' �g��\���@�\�ꎞ��~
        ''' </summary>
        Public Property DspStop() As Boolean
            Get
                Return m_bDspStop
            End Get
            Set(ByVal value As Boolean)

                ' �g��\�����Ȃ�A����͒��f����
                If value And m_bDspOn Then
                    Call BasePic_MouseUp(Nothing, Nothing)
                End If

                ' ���f������ɁA�l��ύX����
                m_bDspStop = value
            End Set
        End Property

        ''' <summary>
        ''' �g��\���G���A��\������true
        ''' </summary>
        ''' <value></value>
        Public ReadOnly Property IsDsp() As Boolean
            Get
                Return m_bDspOn
            End Get
        End Property

        ''' <summary>
        ''' �g��摜�G���A�T�C�Y
        ''' </summary>
        Public Property DispSize() As Size
            Get
                Return m_pic.Size
            End Get
            Set(ByVal value As Size)
                m_pic.Size = value
            End Set
        End Property

        ''' <summary>
        ''' ���������A�N�Z�X
        ''' </summary>
        Public Property MemoriInf() As MEMORI_INF
            Get
                Return m_MemoriInf
            End Get
            Set(ByVal value As MEMORI_INF)
                m_MemoriInf = value
            End Set
        End Property

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ���J ����
        '/ /////////////////////////////////////////////////////////////////////////////////

        Private m_DefaultSize As Size = New Size(200, 200)
        Private m_nDefRitu As Double = 1.0

        Public Comment As String = String.Empty

        Public Property DefaultSize As Size
            Get
                Return m_DefaultSize
            End Get
            Set(value As Size)
                m_DefaultSize = value
            End Set
        End Property

        Public Property DefaultZoom As Double
            Get
                Return m_nDefRitu
            End Get
            Set(value As Double)
                m_nDefRitu = value
            End Set
        End Property

        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="frm">�x�[�X�t�H�[��</param>
        ''' <param name="pic">�g��Ώۂ̃s�N�`���[�{�b�N�X</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal frm As Control, ByVal pic As PictureBox, Optional ByVal WindowSize As Integer = 200)
            'Public Sub New(ByRef frm As Form, ByRef pic As PictureBox)
            '''' �f�[�^���Z�b�g
            m_BaseFrm = frm
            m_BasePic = pic
            DefaultSize = New Size(WindowSize, WindowSize)


            '''' �g��\���G���A�𐶐�
            m_pic = New PictureBox
            m_pic.Size = DefaultSize
            m_pic.Location = New Point(0, 0)
            m_pic.BorderStyle = BorderStyle.FixedSingle
            m_pic.Visible = False
            m_pic.Enabled = False                               ' ���ዾ�\�����Ƀt�H�[�J�X���Ƃ�Ȃ��悤�ɂ��邽�߂̑΍�

            m_BaseFrm.Controls.Add(m_pic)                       ' �t���[���摜�\���s�N�`���[�̔z���ɂ���
            m_pic.BringToFront()                                ' �e�s�N�`���[�̏�Ɏ����Ă���

            '''' ���̑�������돉����


            'm_MemoriInf.bIsDspRitu = m_MemoriInf.bIsDspMemori = True

            '''' �C�x���g�ǉ�
            AddHandler m_pic.Paint, AddressOf pic_Paint
            AddHandler m_BasePic.MouseDown, AddressOf BasePic_MouseDown
            AddHandler m_BasePic.MouseUp, AddressOf BasePic_MouseUp
            AddHandler m_BasePic.MouseMove, AddressOf BasePic_MouseMove
            AddHandler m_BaseFrm.MouseWheel, AddressOf BaseFrm_MouseWheela


        End Sub


#Region " IDisposable Support "

        Private disposedValue As Boolean = False        ' �d������Ăяo�������o����ɂ�

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��Ƀ}�l�[�W ���\�[�X��������܂�
                End If

                '''' ���L�̃A���}�l�[�W ���\�[�X��������܂�

                '''' �C�x���g�J��
                RemoveHandler m_pic.Paint, AddressOf pic_Paint
                RemoveHandler m_BasePic.MouseDown, AddressOf BasePic_MouseDown
                RemoveHandler m_BasePic.MouseUp, AddressOf BasePic_MouseUp
                RemoveHandler m_BasePic.MouseMove, AddressOf BasePic_MouseMove
                RemoveHandler m_BaseFrm.MouseWheel, AddressOf BaseFrm_MouseWheela

                '''' �g��\���G���A �J��
                m_BaseFrm.Controls.Remove(m_pic)
                m_pic.Dispose()

            End If
            Me.disposedValue = True
        End Sub

        ' ���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
        Public Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub


#End Region


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ����
        '/ /////////////////////////////////////////////////////////////////////////////////
        Shared sh_ippResize As tClass_ImgResize_IPP

        Shared Function hasPalette(pf As Imaging.PixelFormat) As Boolean
            If pf = Imaging.PixelFormat.Format8bppIndexed Then Return True

            If pf = Imaging.PixelFormat.Format4bppIndexed Then Return True
            If pf = Imaging.PixelFormat.Format1bppIndexed Then Return True
            Return False
        End Function

        Private Function PrepareResizedBitmap(ByVal rrp As Rectangle, ByVal DisplaySize As Size) As Bitmap

            If sh_ippResize Is Nothing Then
                sh_ippResize = New tClass_ImgResize_IPP()
                sh_ippResize.Resize = tClass_ImgResize_IPP.ResizeMethod.ippLinear
            End If


            Dim refimage As Image = Nothing
            For ii As Integer = 0 To m_pDspImg.Length - 1
                If m_pDspImg(ii) IsNot Nothing Then
                    refimage = m_pDspImg(ii)
                    Exit For
                End If
            Next
            If refimage Is Nothing Then
                Return Nothing
            End If

            Dim bmp As New Bitmap(DisplaySize.Width, DisplaySize.Height, refimage.PixelFormat)
            If hasPalette(refimage.PixelFormat) Then
                bmp.Palette = refimage.Palette
            End If

            Dim rp As Rectangle = New Rectangle(rrp.Location, New Size(Math.Abs(rrp.Width), Math.Abs(rrp.Height)))
            Dim FrameHeight As Integer = mcls_pCalc.tProp_ImgInf.nOneImgH
            Try

                Using dmem As New LockingBitmap(bmp, Imaging.ImageLockMode.WriteOnly)
                    Dim ry As Integer = rp.Top
                    Do While ry < rp.Bottom

                        Dim nFrame As Integer = ry \ FrameHeight
                        Dim SrcY As Integer = ry Mod FrameHeight
                        Dim SrcHeight As Integer = Math.Min(FrameHeight - SrcY, rp.Bottom - ry)
                        If SrcHeight = 0 Then Exit Do

                        Dim DstY As Integer = CInt((ry - rp.Y) * DisplaySize.Height / rp.Height)
                        Dim DstYN As Integer = CInt((ry + SrcHeight - rp.Y) * DisplaySize.Height / rp.Height)
                        Dim DstHeight As Integer = DstYN - DstY


                        Dim srcbmp As Bitmap = TryCast(m_pDspImg(nFrame), Bitmap)
                        If srcbmp IsNot Nothing Then
                            Using smem As New LockingBitmap(srcbmp, Imaging.ImageLockMode.ReadOnly)
                                sh_ippResize.ippResize(smem.PtrAt(SrcY, rp.X), smem.Stride, rp.Width, SrcHeight, _
                                                       dmem.PtrAt(DstY, 0), dmem.Stride, dmem.Width, DstHeight, bmp.PixelFormat)
                            End Using
                        End If

                        ' ����
                        ry = ry + SrcHeight
                    Loop

                End Using
            Catch ex As Exception

            End Try

            RotateByImgKind(bmp)
            Return bmp


        End Function

        ''' <summary>
        ''' �\���ɉ������ϊ��B��]�E���]
        ''' </summary>
        ''' <param name="bmp"></param>
        Private Sub RotateByImgKind(bmp As Bitmap)
            Dim RotTable() As RotateFlipType = New RotateFlipType() { _
                RotateFlipType.RotateNoneFlipNone, _
                RotateFlipType.RotateNoneFlipY, _
                RotateFlipType.RotateNoneFlipX, _
                RotateFlipType.RotateNoneFlipXY, _
                RotateFlipType.Rotate90FlipXY, _
                RotateFlipType.Rotate90FlipY, _
                RotateFlipType.Rotate90FlipX, _
                RotateFlipType.Rotate90FlipNone _
            }
            Dim nIndex As Integer = IIf(mcls_pCalc.tProp_ImgKind_Rotate90, 4, 0) _
                                    + IIf(mcls_pCalc.tProp_ImgKind_IsTurnLR, 2, 0) _
                                    + IIf(mcls_pCalc.tProp_ImgKind_IsTurnTB, 1, 0)

            If RotTable(nIndex) <> 0 Then
                bmp.RotateFlip(RotTable(nIndex))
            End If
        End Sub

        ''' <summary>
        ''' �w�肵���̈�̃t���[���摜���擾
        ''' </summary>
        ''' <param name="rrp">�擾����摜�̃G���A�B�t���[���������W</param>
        ''' <returns>�w��G���A�̉摜</returns>
        ''' <remarks></remarks>
        Private Function PrepareRawBitmap(ByVal rrp As Rectangle, ByVal DisplaySize As Size) As Bitmap
            Dim rp As Rectangle = New Rectangle(rrp.Location, New Size(Math.Abs(rrp.Width), Math.Abs(rrp.Height)))

            If rp.Height > 4096 Or rp.Width > 4096 Then
                Return PrepareResizedBitmap(rrp, DisplaySize)
            End If

            Dim bmp As New Bitmap(rp.Width, rp.Height, Imaging.PixelFormat.Format32bppRgb)

            Dim FrameHeight As Integer = mcls_pCalc.tProp_ImgInf.nOneImgH
            Try
                Using g As Graphics = Graphics.FromImage(bmp)

                    Dim nIndexX As Integer = (rp.X + rp.Width - 1) \ mcls_pCalc.tProp_ImgInf.nOneImgW
                    If nIndexX <> 0 Then Throw New System.NotSupportedException("X�����ɕ����摜����`��ɑ΂��Ă̕����g��\���͖��Ή��B")
                    Dim nIndexYS As Integer = rp.Y \ FrameHeight
                    Dim nIndexYE As Integer = (rp.Y + rp.Height - 1) \ FrameHeight


                    '''' �`��
                    Dim Dst As New Rectangle(0, 0, rp.Width, 0)        ' �`��̈�
                    Dim Src As New Rectangle(rp.X, rp.Y Mod FrameHeight, rp.Width, 0)       ' ���摜�͈�


                    For ii As Integer = nIndexYS To nIndexYE
                        If ii <> nIndexYE Then
                            Src.Height = FrameHeight - Src.Y
                        Else
                            Src.Height = (rp.Y + rp.Height - 1) Mod FrameHeight
                        End If
                        Dst.Height = Src.Height

                        If m_pDspImg(ii) IsNot Nothing Then
                            g.DrawImage(m_pDspImg(ii), Dst, Src, GraphicsUnit.Pixel)
                        End If

                        ' ����
                        Src.Y = 0
                        Dst.Y += Dst.Height
                    Next
                End Using
            Catch ex As Exception

            End Try

            '�\���ɉ������ϊ��B��]�E���]
            RotateByImgKind(bmp)

            Return bmp
        End Function


        ''' <summary>
        ''' �����g��摜�̕`��
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub pic_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
            Dim ptMouse As Point = m_BasePic.PointToClient(Windows.Forms.Cursor.Position)          ' �N���b�N�ʒu (��ʍ��W����Pic�R���g���[���̍��ォ��̍��W�ɕϊ�)
            Dim g As Graphics = e.Graphics

            '''' ���摜�̈ʒu���擾
            Dim Rp As Rectangle = GetRpAria(ptMouse)


            Try
                '�t���[������(���邩������Ȃ�)�摜���擾
                Using bmp As Bitmap = PrepareRawBitmap(Rp, m_pic.Size)
                    '�t���[���摜���k���\���B
                    g.CompositingQuality = Drawing2D.CompositingQuality.HighQuality
                    g.InterpolationMode = Drawing2D.InterpolationMode.HighQualityBilinear
                    g.DrawImage(bmp, New Rectangle(0, 0, m_pic.Width, m_pic.Height))
                End Using

                '' ���������֌W�̕`��
                If m_MemoriInf.bIsDspRitu Or m_MemoriInf.bIsDspMemori Then
                    Dim Bsh As SolidBrush = New SolidBrush(m_MemoriInf.Color)
                    Dim Pen As Pen = New System.Drawing.Pen(m_MemoriInf.Color, 2)
                    Dim Fnt As Font = New System.Drawing.Font("�l�r �S�V�b�N", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)

                    ' ���݂̔{����\������
                    Dim dsp_string_left As Integer = 0
                    If m_MemoriInf.bIsDspRitu Then
                        Dim strValWk As String = String.Format("x {0:0.0}", m_dDspRitu)
                        dsp_string_left = CInt(g.MeasureString(strValWk, Fnt).Width)
                        g.DrawString(strValWk, Fnt, Bsh, 2, 2) ' �`��
                    End If

                    If Not String.IsNullOrEmpty(Comment) Then
                        Dim r As New Rectangle(0, 0, m_pic.Width, 32)
                        If dsp_string_left + 32 < m_pic.Width Then
                            r.X += dsp_string_left
                            r.Width -= dsp_string_left
                        Else
                            r.Y += 18
                        End If
                        Dim sf As New StringFormat
                        sf.Alignment = StringAlignment.Center
                        g.DrawString(Comment, Fnt, Brushes.Yellow, r, sf)
                    End If

                    ' �������\��
                    If m_MemoriInf.bIsDspMemori Then
                        Dim memori As New SimpleMemori(m_pic.ClientSize, mcls_pCalc)
                        memori.MemoriInf = Me.MemoriInf
                        memori.zoom_modifier_x = 1 / m_dDspRitu
                        memori.zoom_modifier_y = 1 / m_dDspRitu
                        memori.DrawMemori(g, Bsh, Pen, Fnt)
                    End If
                End If
            Catch ex As Exception
            End Try
        End Sub

        Function StartDrawing(pt As Point) As Boolean
            '''' �`��ɕK�v�ȏ����擾. pDspImg�́A�{�̑��̉摜���Q�Ƃ��Ă���̂ŁADispose���Ă͂����Ȃ�
            mcls_pCalc = Nothing
            m_pDspImg = Nothing
            RaiseEvent tEv_DispDataSet(mcls_pCalc, m_pDspImg)


            '''' �g��\���G���A�̈ʒu������
            m_pic.Location = GetPicLocation(pt)

            ' �g��\���G���A��\��
            m_pic.Visible = True
            m_bDspOn = True

            ' �}�E�X�J�[�\�����\��
            Cursor.Clip = New Rectangle(m_BaseFrm.PointToScreen(m_BasePic.Location), m_BasePic.ClientSize)

            Return True

        End Function

        ''' <summary>
        ''' �e�s�N�`���{�b�N�X�ł̃}�E�X������������肵�āA�����̕\�����J�n
        ''' </summary>
        Private Sub BasePic_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            'If e.Button <> MouseButtons.Right Then Return

            '' ���ዾ�\�� �J�n
            If e.Button = MouseButtons.Right Then
                If m_bDspOn Then Return
                Cursor.Hide()

                m_dDspRitu = DefaultZoom
                StartDrawing(e.Location)
            End If



            '' ���ዾ�\�����̂�
            If e.Button = MouseButtons.Left Then
                If Not m_bDspOn Then Return

                ' �O���t�B�b�N�ŕ`�悵�����̂��擾
                Dim rect As Rectangle = m_pic.ClientRectangle
                Dim bmp As New Bitmap(rect.Width, rect.Height)
                Dim g As Graphics = Graphics.FromImage(bmp)     ' �`���m��
                Dim pea As New PaintEventArgs(g, rect)          ' �`��֐�����
                ' ��������pea�ɑ΂��āA�`�悳����B
                pic_Paint(m_pic, pea)
                ' �N���b�v�{�[�h��
                Clipboard.SetImage(bmp)
                ' ��n��
                g.Dispose()
                bmp.Dispose()
            End If
        End Sub

        ''' <summary>
        ''' �e�s�N�`���{�b�N�X�ł̃}�E�X����ɂ��A�\�����I��
        ''' </summary>
        Private Sub BasePic_MouseUp(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return

            '' ���ዾ�\�� ����
            'If Not m_bDspOn Then Return
            If e.Button = MouseButtons.Right Then
                If m_bDspOn Then
                    m_pic.Visible = False
                    m_bDspOn = False

                    ' �}�E�X�J�[�\����\��
                    Cursor.Show()
                    Cursor.Clip = Nothing
                End If
            End If
        End Sub



        ''' <summary>
        ''' ��������\�ύX(�k���\�������j�����{�����Ƃ��ɁA�ύX�㕪��\����A���̕���\�ɖ߂��W���ݒ�
        ''' </summary>
        ''' <param name="comment">��������\�ύX�̃R�����g�B�g�厞�Ƀ^�C�g���Ƃ��ĕ\��</param>
        Public Sub SetComment(comment As String)
            Me.Comment = comment
        End Sub

        ''' <summary>
        ''' �e�s�N�`���{�b�N�X�ł̃}�E�X�ړ��Ɋ�Â��A�g��̊�ʒu�𓮂���
        ''' </summary>
        Private Sub BasePic_MouseMove(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return

            ' windows�L�[�Ƃ��Ńt�H�[�J�X�����t�H�[���Ɉڂ��Ă���A�}�E�X�A�b�v���ꂽ�ꍇ
            ' �}�E�X�A�b�v�C�x���g������Ȃ��΍�B
            '    ���������Ƃ��ɁA�}�E�X�E��������Ă��Ȃ�������A�g�勭������
            If e.Button <> MouseButtons.Right Then
                Call BasePic_MouseUp(sender, e)
                Return
            End If

            m_pic.Location = GetPicLocation(e.Location)             ' �g��\���G���A�̈ʒu������

            ' ������Ȃ��悤�ɕ`��
            m_BasePic.Refresh()
            m_pic.Refresh()
            'm_pic.Invalidate()
        End Sub

        ''' <summary>
        ''' �e�s�N�`���{�b�N�X�ł̃}�E�X�z�C�[��������肵�āA�g��{����ύX����BCtrl�L�[��������Ă�����A���E�C���h�E���g��
        ''' </summary>
        Private Sub BaseFrm_MouseWheela(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return

            'Ctrl + Wheel = �g��E�C���h�E�T�C�Y�̕ύX
            If Control.ModifierKeys = Keys.Control Then
                Dim dxdy As Integer = 20
                If e.Delta < 0 Then
                    dxdy *= -1
                End If

                '�E�C���h�E�T�C�Y�͐����`��O��Ƃ���
                Dim newsize As Integer = Saturate(m_pic.Width + dxdy, 100, MAX_WINDOW_SIZE)
                m_pic.Size = New Size(newsize, newsize)
                DefaultSize = m_pic.Size

                '�E�C���h�E�ʒu�́A�J�[�\�����S����B�T�C�Y���ς�����̂ł���Ă�
                Dim pt As Point = Control.MousePosition
                Dim ptbase As Point = m_BaseFrm.PointToClient(pt)
                StartDrawing(ptbase)
                Return
            End If

            '�ӂ���Wheel = �g�嗦�̕ύX
            If e.Delta > 0 Then
                m_dDspRitu += 0.5
            Else
                m_dDspRitu -= 0.5
            End If
            LimitValue(m_dDspRitu, MIN_ZOOM, MAX_ZOOM)

            ' �ĕ`�� (�ʒu�͕ς��Ȃ��̂ŁA�g��\���G���A�̂ݕ`��)
            m_pic.Refresh()
        End Sub

        ''' <summary>
        ''' �x�[�X�t�H�[�� �}�E�X�N���b�N
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub BaseFrm_MouseClick(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return

            If e.Button = MouseButtons.Middle Then
                'MsgBox("")
            End If
        End Sub



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ����
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' [DP]�x�[�X�t�H�[������̊g��\���G���A�̈ʒu���Z�o
        ''' </summary>
        ''' <param name="ptLocation">�x�[�X�s�N�`���[�{�b�N�X �� �}�E�X�ʒu</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function GetPicLocation(ByVal ptLocation As Point) As Point
            Dim pnt As New Point(ptLocation.X + m_BasePic.Left - CInt(m_pic.Size.Width / 2), ptLocation.Y + m_BasePic.Top - CInt(m_pic.Size.Height / 2))

            ' �x�[�X�s�N�`���[�{�b�N�X ���͂ݏo�邱�Ƃ������悤�ɒ���
            If pnt.X < m_BasePic.Left Then pnt.X = m_BasePic.Left
            If pnt.Y < m_BasePic.Top Then pnt.Y = m_BasePic.Top

            If (pnt.X + m_pic.Width) > (m_BasePic.Left + m_BasePic.Width) Then pnt.X -= (pnt.X + m_pic.Width) - (m_BasePic.Left + m_BasePic.Width)
            If (pnt.Y + m_pic.Height) > (m_BasePic.Top + m_BasePic.Height) Then pnt.Y -= (pnt.Y + m_pic.Height) - (m_BasePic.Top + m_BasePic.Height)

            Return pnt
        End Function

        ''' <summary>
        ''' [DP]�x�[�X�t�H�[������̊g��\���G���A�̈ʒu���Z�o
        ''' </summary>
        ''' <param name="e">�x�[�X�s�N�`���[�{�b�N�X �� �}�E�X�ʒu</param>
        Private Function GetRpAria(ByVal e As Point) As Rectangle
            Dim CalcAns As tClass_ImageDsp_Calc.CALC_ANS = mcls_pCalc.tProp_CalcAns

            ' �}�E�X�ʒu [RP]
            Dim nRpPoint As Point = mcls_pCalc.GetDspPixelToImgPosition(e.X, e.Y)

            ' �\���G���A�T�C�Y [RP]
            Dim nDspSizeX As Integer = (CInt((m_pic.Size.Width / m_dDspRitu) / CalcAns.RateDsp.dW) + 1) '* CInt(IIf(mcls_pCalc.tProp_ImgKind_IsTurnLR, -1, 1))
            Dim nDspSizeY As Integer = (CInt((m_pic.Size.Height / m_dDspRitu) / CalcAns.RateDsp.dH) + 1) ' * CInt(IIf(mcls_pCalc.tProp_ImgKind_IsTurnTB, -1, 1))

            ' ���摜�̕\���Ώۈʒu [RP]
            Dim rectRp As New Rectangle(nRpPoint.X - CInt(nDspSizeX / 2), nRpPoint.Y - CInt(nDspSizeY / 2), nDspSizeX, nDspSizeY)

            ' �㉺���`�F�b�N
            Dim nMinX As Integer = mcls_pCalc.GetPos_DpToRp_X(CalcAns.ImgDspRect.X)
            Dim nMinY As Integer = mcls_pCalc.GetPos_DpToRp_Y(CalcAns.ImgDspRect.Y)
            Dim nMaxX As Integer = mcls_pCalc.tProp_ImgInf.nImgW
            Dim nMaxY As Integer = mcls_pCalc.tProp_ImgInf.nImgH

            If rectRp.X < nMinX Then rectRp.X = nMinX
            If rectRp.Y < nMinY Then rectRp.Y = nMinY
            If (rectRp.X + rectRp.Width) > nMaxX Then rectRp.X -= (rectRp.X + rectRp.Width) - nMaxX
            If (rectRp.Y + rectRp.Height) > nMaxY Then rectRp.Y -= (rectRp.Y + rectRp.Height) - nMaxY

            Return rectRp


            '''' [DP]���W�n�ł������A���{�����Ɍ덷���傫������̂ł��

            '' ���摜���ォ��}�E�X�ʒu�܂ł̋������擾 [DP]
            'Dim nDpPoint As Point					' ���摜���ォ��}�E�X�ʒu�܂ł̋������擾 [DP]
            'nDpPoint = New Point(e.X, e.Y)	 ' �\���摜���ォ��}�E�X�ʒu�܂ł̋������擾 [DP]
            'Dim DspSize As New Size(CInt(m_pic.Size.Width / m_dDspRitu), CInt(m_pic.Size.Height / m_dDspRitu))

            'Debug.Print("DP={0},{1} ", nDpPoint.X, nDpPoint.Y)

            '' ���摜���ォ��DP�ł̕\���Ώۈʒu
            'Dim rectDp As New Rectangle(nDpPoint.X - CInt(DspSize.Width / 2), nDpPoint.Y - CInt(DspSize.Height / 2), DspSize.Width, DspSize.Height)
            'If rectDp.X < CalcAns.ImgDspRect.X Then rectDp.X = mcls_pCalc.tProp_CalcAns.ImgDspRect.X
            'If rectDp.Y < CalcAns.ImgDspRect.Y Then rectDp.Y = mcls_pCalc.tProp_CalcAns.ImgDspRect.Y
            'If (rectDp.X + rectDp.Width) > CInt(mcls_pCalc.tProp_ImgInf.nImgW * CalcAns.RateDsp.dW) Then rectDp.X -= (rectDp.X + rectDp.Width) - CInt(mcls_pCalc.tProp_ImgInf.nImgW * CalcAns.RateDsp.dW)
            'If (rectDp.Y + rectDp.Height) > CInt(mcls_pCalc.tProp_ImgInf.nImgH * CalcAns.RateDsp.dH) Then rectDp.Y -= (rectDp.Y + rectDp.Height) - CInt(mcls_pCalc.tProp_ImgInf.nImgH * CalcAns.RateDsp.dH)


            '' DP��RP
            'Dim PointRp1 As Point = mcls_pCalc.GetDspPixelToImgPosition(rectDp.X, rectDp.Y)
            'Dim PointRp2 As Point = mcls_pCalc.GetDspPixelToImgPosition(rectDp.X + rectDp.Width - 1, rectDp.Y + rectDp.Height - 1)

            'Debug.Print("XY1={0},{1}  XY2={2},{3}", PointRp1.X, PointRp1.Y, PointRp2.X, PointRp2.Y)


            'Return New Rectangle(PointRp1.X, PointRp1.Y, PointRp2.X - PointRp1.X + 1, PointRp2.Y - PointRp1.Y + 1)
        End Function

    End Class

End Namespace
