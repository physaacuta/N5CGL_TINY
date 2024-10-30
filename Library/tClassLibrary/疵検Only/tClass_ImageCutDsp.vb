'*********************************************************************************
'�s�N�`���[�{�b�N�X�̉摜�𕔕��I�Ɋg��\������N���X (�Ȃ񂿂���Ċg���)
'	[Ver]
'		Ver.01    2008/11/21  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.Windows.Forms
Imports System.Drawing


Namespace tClass

    ''' <summary>
    ''' �s�N�`���[�{�b�N�X�̉摜�𕔕��I�Ɋg��\������N���X
    ''' </summary>
    Public Class tClass_ImageCutDsp
        Implements IDisposable							' �f�X�g���N�^��`




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
        Private m_BaseFrm As Control 'Form					' �x�[�X�t�H�[��
        Private m_BasePic As PictureBox				' �x�[�X�s�N�`���[�{�b�N�X

        Private m_pic As PictureBox					' �g��\���G���A

        Private m_bDspStop As Boolean = False		' �g��\���@�\�ꎞ��~
        Private m_bDspOn As Boolean = False			' �g��\����
        Private m_dDspRitu As Double				' �g�嗦
        Private m_dDefRitu As Double				' �f�t�H���g�{��


        Private m_MemoriInf As MEMORI_INF			' ���������
        Private m_dBaseResW As Double				' �x�[�X�摜�̕\��������\
        Private m_dBaseResH As Double				' �x�[�X�摜�̕\���c����\


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
        ''' �����{��
        ''' </summary>
        Public Property DefRitu() As Double
            Get
                Return m_dDefRitu
            End Get
            Set(ByVal value As Double)
                m_dDefRitu = value
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

        ''' <summary>
        ''' �x�[�X�摜�̕\������\
        ''' </summary>
        ''' <param name="resW">������\</param>
        ''' <param name="resH">�c����\</param>
        Public Sub SetItemRes(ByVal resW As Double, ByVal resH As Double)
            m_dBaseResW = resW
            m_dBaseResH = resH
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ���J ����
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="frm">�x�[�X�t�H�[��</param>
        ''' <param name="pic">�g��Ώۂ̃s�N�`���[�{�b�N�X</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal frm As Control, ByVal pic As PictureBox)
            '''' �f�[�^���Z�b�g
            m_BaseFrm = frm
            m_BasePic = pic

            '''' �g��\���G���A�𐶐�
            m_pic = New PictureBox
            m_pic.Size = New Size(200, 200)
            m_pic.Location = New Point(0, 0)
            m_pic.BorderStyle = BorderStyle.FixedSingle
            m_pic.Visible = False

            m_BaseFrm.Controls.Add(m_pic)                       ' �t���[���摜�\���s�N�`���[�̔z���ɂ���
            m_pic.BringToFront()                                ' �e�s�N�`���[�̏�Ɏ����Ă���


            '''' ���̑�������돉����
            m_dDefRitu = 2.0
            m_dBaseResW = 1.0
            m_dBaseResH = 1.0


            '''' �C�x���g�ǉ�
            AddHandler m_pic.Paint, AddressOf pic_Paint
            AddHandler m_BasePic.MouseDown, AddressOf BasePic_MouseDown
            AddHandler m_BasePic.MouseUp, AddressOf BasePic_MouseUp
            AddHandler m_BasePic.MouseMove, AddressOf BasePic_MouseMove
            AddHandler m_BaseFrm.MouseWheel, AddressOf BaseFrm_MouseWheela

            'Windows.Forms.Cursor.Hide()					' �J�[�\��������

        End Sub



#Region " IDisposable Support "

        Private disposedValue As Boolean = False		' �d������Ăяo�������o����ɂ�

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



        ''' <summary>
        ''' �g��\���G���A �y�C���g
        ''' </summary>
        Private Sub pic_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
            '' �L���v�`�������摜���i�[����G���A
            Using bmp As New Bitmap(m_BasePic.ClientSize.Width, m_BasePic.ClientSize.Height)

                ' �Ώۃs�N�`���[�{�b�N�X�̑S�̂̃L���v�`��(rectangle��bmp�̂ǂ̈ʒu�ɕ`����)
                m_BasePic.DrawToBitmap(bmp, New Rectangle(0, 0, bmp.Size.Width, bmp.Size.Height))

                ' �x�[�X�̕`��ʒu������
                Dim pos As Point = m_BasePic.PointToClient(Windows.Forms.Cursor.Position)		   ' �N���b�N�ʒu (��ʍ��W����Pic�R���g���[���̍��ォ��̍��W�ɕϊ�)
                Dim dspSize As New Size(CInt(m_pic.Size.Width * m_dDspRitu), CInt(m_pic.Size.Height * m_dDspRitu))		' ����\������摜�̃T�C�Y
                Dim dspPnt As New Point(CInt(m_pic.Size.Width / 2) - CInt(dspSize.Width / 2), CInt(m_pic.Size.Height / 2) - CInt(dspSize.Height / 2))

                e.Graphics.CompositingQuality = Drawing2D.CompositingQuality.HighQuality			' �g�傪���C���ׁ̈A�N�H���e�B�[��ǂ����Ă���

                ' �}�E�X�|�C���^�𒆐S�Ƃ��āA�g��`�悷��
                e.Graphics.DrawImage(bmp, _
                  New Rectangle(dspPnt, dspSize), _
                  New Rectangle(pos.X - CInt(m_pic.Size.Width / 2), pos.Y - CInt(m_pic.Size.Height / 2), m_pic.Size.Width, m_pic.Size.Height), GraphicsUnit.Pixel)
            End Using

            '' �������J������
            ' m_BasePic.DrawToBitmap �ŉ摜�T�C�Y�����傾�ƁA�������ɂ����c�[���c��BGC���J������O�Ɏ�L���̈悪�p���N����
            ' ���ׁ̈A�蓮��GC���R�[������悤�ɂ���
            GC.Collect()

            '' ���������֌W�̕`��
            If m_MemoriInf.bIsDspRitu Or m_MemoriInf.bIsDspMemori Then
                Dim Bsh As SolidBrush = New SolidBrush(m_MemoriInf.Color)
                Dim Pen As Pen = New System.Drawing.Pen(m_MemoriInf.Color, 2)
                Dim Fnt As Font = New System.Drawing.Font("�l�r �S�V�b�N", 12, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
                Dim strValWk As String

                ' ���݂̔{����\������
                If m_MemoriInf.bIsDspRitu Then
                    strValWk = String.Format("x {0:0.0}", m_dDspRitu)

                    e.Graphics.DrawString(strValWk, Fnt, Bsh, 2, 2)	' �`��
                End If

                ' �������\��
                If m_MemoriInf.bIsDspMemori Then
                    Dim nPlsLineTop As Integer = m_pic.Size.Height - 20
                    Dim nPlsLineLeft As Integer = 15

                    ' Y��
                    strValWk = String.Format("{0:0.0}", m_MemoriInf.nInteralY * m_dBaseResH / m_dDspRitu)
                    e.Graphics.DrawString(strValWk, Fnt, Bsh, nPlsLineLeft - 10, nPlsLineTop - m_MemoriInf.nInteralY - 15)
                    e.Graphics.DrawLine(Pen, nPlsLineLeft, nPlsLineTop, nPlsLineLeft, nPlsLineTop - m_MemoriInf.nInteralY)

                    ' X��
                    strValWk = String.Format("{0:0.0}", m_MemoriInf.nInteralX * m_dBaseResW / m_dDspRitu)
                    e.Graphics.DrawString(strValWk, Fnt, Bsh, nPlsLineLeft + m_MemoriInf.nInteralX - 10, nPlsLineTop + 3)
                    e.Graphics.DrawLine(Pen, nPlsLineLeft, nPlsLineTop, nPlsLineLeft + m_MemoriInf.nInteralX, nPlsLineTop)

                    ' ���_�\��
                    e.Graphics.DrawString("0mm", Fnt, Bsh, nPlsLineLeft - 10, nPlsLineTop + 3)
                End If
            End If
        End Sub


        ''' <summary>
        ''' �x�[�X�s�N�`���[�{�b�N�X �}�E�X�_�E��
        ''' </summary>
        Private Sub BasePic_MouseDown(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If e.Button <> MouseButtons.Right Then Return
            If m_bDspOn Then Return

            m_pic.Location = GetPicLocation(e)				' �g��\���G���A�̈ʒu������

            ' �g��\���G���A��\��
            m_dDspRitu = m_dDefRitu
            m_pic.Visible = True
            m_bDspOn = True

            ' �}�E�X�J�[�\�����\��
            Cursor.Hide()
            Cursor.Clip = New Rectangle(m_BaseFrm.PointToScreen(m_BasePic.Location), m_BasePic.ClientSize)
        End Sub

        ''' <summary>
        ''' �x�[�X�s�N�`���[�{�b�N�X �}�E�X�A�b�v
        ''' </summary>
        Private Sub BasePic_MouseUp(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return

            m_pic.Visible = False
            m_bDspOn = False

            ' �}�E�X�J�[�\����\��
            Cursor.Show()
            'Cursor.Clip.
            Cursor.Clip = Nothing
        End Sub

        ''' <summary>
        ''' �x�[�X�s�N�`���[�{�b�N�X �}�E�X���[�u
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

            m_pic.Location = GetPicLocation(e)				' �g��\���G���A�̈ʒu������

            ' ������Ȃ��悤�ɕ`��
            m_BasePic.Refresh()
            m_pic.Refresh()
        End Sub

        ''' <summary>
        ''' �x�[�X�t�H�[�� �}�E�X�X�N���[��
        ''' </summary>
        Private Sub BaseFrm_MouseWheela(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
            If m_bDspStop Then Return
            If Not m_bDspOn Then Return
            'If e.Button <> MouseButtons.Right Then Return

            ' �{������
            If e.Delta > 0 Then
                m_dDspRitu += 0.5
                If m_dDspRitu > 10.0 Then m_dDspRitu = 10.0
            Else
                m_dDspRitu -= 0.5
                If m_dDspRitu < 1.0 Then m_dDspRitu = 1.0
            End If

            ' �ĕ`�� (�ʒu�͕ς��Ȃ��̂ŁA�g��\���G���A�̂ݕ`��)
            m_pic.Refresh()
        End Sub



        ''' <summary>
        ''' �x�[�X�t�H�[������̊g��\���G���A�̈ʒu���Z�o
        ''' </summary>
        ''' <param name="e">�x�[�X�s�N�`���[�{�b�N�X �� �}�E�X�ʒu</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function GetPicLocation(ByVal e As System.Windows.Forms.MouseEventArgs) As Point
            Dim pnt As New Point(e.Location.X + m_BasePic.Left - CInt(m_pic.Size.Width / 2), e.Location.Y + m_BasePic.Top - CInt(m_pic.Size.Height / 2))

            ' �x�[�X�s�N�`���[�{�b�N�X ���͂ݏo�邱�Ƃ������悤�ɒ���
            If pnt.X < m_BasePic.Left Then pnt.X = m_BasePic.Left
            If pnt.Y < m_BasePic.Top Then pnt.Y = m_BasePic.Top

            If (pnt.X + m_pic.Width) > (m_BasePic.Left + m_BasePic.Width) Then pnt.X -= (pnt.X + m_pic.Width) - (m_BasePic.Left + m_BasePic.Width)
            If (pnt.Y + m_pic.Height) > (m_BasePic.Top + m_BasePic.Height) Then pnt.Y -= (pnt.Y + m_pic.Height) - (m_BasePic.Top + m_BasePic.Height)


            Return pnt
        End Function
    End Class
End Namespace