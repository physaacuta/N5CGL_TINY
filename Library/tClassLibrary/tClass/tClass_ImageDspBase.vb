'*********************************************************************************
'�摜�\��Base�N���X
'	[Ver]
'		Ver.01    2008/09/06  vs2005 �Ή�
'
'	[����]
'		����Ӗ��r����p�H�H
'*********************************************************************************
Option Strict On				'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports system.Drawing

Imports tClassLibrary
Imports System.Windows.Forms
'Imports System.IO
Namespace tClass

    ''' <summary>�摜�\�����N���X</summary>
    Public Class tClass_ImageDspBase
        Inherits tClass_ImageDsp_Calc
        Implements IDisposable							' �f�X�g���N�^��`

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �萔��`
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>�������ݒ���</summary>
        Public Structure MEMORI_INF
            ''' <summary>�������\���L�� (true:�L, false:��)</summary>
            Dim bIsDsp As Boolean
            ''' <summary>�������\���F</summary>
            Dim Color As Color
            ''' <summary>�������t�H���g�T�C�Y</summary>
            Dim FontSize As Integer
            ''' <summary>���������T�C�Y</summary>
            Dim LineWidth As Integer
            ''' <summary>��؂���̍���</summary>
            Dim HojoLineLen As Integer


            ''' <summary>���_�\������</summary>
            Dim strBaseString As String
            ''' <summary>X���̃��C���\���ʒu(�����牽�s�N�Z��)</summary>
            Dim nPosLineBot As Integer
            ''' <summary>Y���̃��C���\���ʒu(�����牽�s�N�Z��)</summary>
            Dim nPosLineLeft As Integer
            ''' <summary>X���̕����\���ʒu(�����牽�s�N�Z��)</summary>
            Dim nPosStringBot As Integer
            ''' <summary>Y���̕����\���ʒu(�����牽�s�N�Z��)</summary>
            Dim nPosStringLeft As Integer

            ''' <summary>X���̃������\���Ԋu[mm]</summary>
            Dim nIntervalX As Integer
            ''' <summary>Y���̃������\���Ԋu[mm]</summary>
            Dim nIntervalY As Integer
            ''' <summary>X���̃������\����</summary>
            Dim nLoopCntX As Integer
            ''' <summary>Y���̃������\����</summary>
            Dim nLoopCntY As Integer

        End Structure



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����o�[�ϐ�
        '/ /////////////////////////////////////////////////////////////////////////////////
        '''''' �`��֌W
        Protected m_cntPic As PictureBox = Nothing

        ''' <summary>�C���[�W�I�u�W�F�N�g</summary>
        Protected m_Img() As Image = Nothing				' �C���[�W�I�u�W�F�N�g
        ''' <summary>�f�t�H���p�g�C���[�W�I�u�W�F�N�g</summary>
        Protected m_DefImg As Image = Nothing				' �f�t�H���g�p�C���[�W�I�u�W�F�N�g
        ''' <summary>�`��L�� (��O��)</summary>
        Protected m_IsPaint As Boolean = False				' �`��L�� (��O��)
        ''' <summary>�f�t�H���g�摜�\��</summary>
        Protected m_IsDefImgDsp As Boolean = True			' �f�t�H���g�摜�\��


        'Private m_OldTurnLR As Boolean = False				' �O�񍶉E���]�t���O�l
        'Private m_OldTurnTB As Boolean = False				' �O��㉺���]�t���O�l


        '''''' �������֌W
        ''' <summary>���������</summary>
        Protected m_Memori As MEMORI_INF					' ���������
        Protected m_memoriBsh As SolidBrush
        Protected m_memoriFnt As Font
        Protected m_memoriPen As Pen



        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �O���C�x���g
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' ���[�U�[�����`��
        ''' </summary>
        ''' <param name="e">�O���t�B�b�N�I�u�W�F�N�g</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_Start(ByVal e As System.Windows.Forms.PaintEventArgs)
        ''' <summary>
        ''' ���[�U�[�ŏI�`��
        ''' </summary>
        ''' <param name="e">�O���t�B�b�N�I�u�W�F�N�g</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)

        ''' <summary>
        ''' ���[�U�[�f�t�H���g�摜�ŏI�`��
        ''' </summary>
        ''' <param name="e">�O���t�B�b�N�I�u�W�F�N�g</param>
        ''' <remarks></remarks>
        Public Event tEv_Paint_NonEnd(ByVal e As System.Windows.Forms.PaintEventArgs)


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �v���p�e�B
        '/ /////////////////////////////////////////////////////////////////////////////////

#Region "�`��֌W"
        ''' <summary>
        ''' �C���[�W�I�u�W�F�N�g
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("���摜�\�����N���X-�`��֌W-��p�v���p�e�B") _
         , System.ComponentModel.Description("�C���[�W�I�u�W�F�N�g")> _
        Public ReadOnly Property tProp_Paint_Img(ByVal index As Integer) As Image
            Get
                If m_Img Is Nothing Then Return Nothing
                Return m_Img(index)
            End Get
        End Property
        Public ReadOnly Property tProp_Paint_Img() As Image()
            Get
                Return m_Img
            End Get
        End Property
        ''' <summary>
        ''' �f�t�H���g�p�C���[�W�I�u�W�F�N�g
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("���摜�\�����N���X-�`��֌W-��p�v���p�e�B") _
         , System.ComponentModel.Description("�f�t�H���g�p�C���[�W�I�u�W�F�N�g")> _
        Public ReadOnly Property tProp_Paint_DefImg() As Image
            Get
                Return m_DefImg
            End Get
        End Property
        ''' <summary>
        ''' �`��L�� (��O��)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("���摜�\�����N���X-�`��֌W-��p�v���p�e�B") _
         , System.ComponentModel.Description("�`��L�� (��O��)")> _
        Public Property tProp_Paint_IsPaint() As Boolean
            Get
                Return m_IsPaint
            End Get
            Set(ByVal value As Boolean)
                m_IsPaint = value
            End Set
        End Property
        ''' <summary>
        ''' �f�t�H���g�摜�\��
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("���摜�\�����N���X-�`��֌W-��p�v���p�e�B") _
         , System.ComponentModel.Description("�f�t�H���g�摜�\��")> _
        Public Property tProp_Paint_IsDefImgDspn() As Boolean
            Get
                Return m_IsDefImgDsp
            End Get
            Set(ByVal value As Boolean)
                m_IsDefImgDsp = value
            End Set
        End Property

#End Region

#Region "�������ݒ���"
        ''' <summary>
        ''' �������ݒ���
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("���摜�\�����N���X-�������ݒ���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�������ݒ���")> _
        Public Property tProp_MemoriInf() As MEMORI_INF
            Get
                Return m_Memori
            End Get
            Set(ByVal value As MEMORI_INF)
                m_Memori = value
                '' �������`��ɕK�v�ȃI�u�W�F�N�g��������
                Call CreateMemoriObj()
            End Set
        End Property
#End Region


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ����
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' �N���X���̕`�揈�������s������
        ''' </summary>
        ''' <param name="pic"></param>
        ''' <remarks></remarks>
        Public Sub AddHandler_PaintEvent(ByVal pic As PictureBox)
            m_cntPic = pic
            AddHandler pic.Paint, AddressOf Paint_Main
        End Sub
        ''' <summary>
        ''' �\���G���A ���T�C�Y �C�x���g�����s
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub AddHandler_PicResize()
            '����₢���킹�Ȃ̂ŕs�v
        End Sub

        ''' <summary>
        ''' �\���G���A���Z�b�g 
        ''' </summary>
        Public Sub SetAriaInf()
            If m_cntPic Is Nothing Then Return
            '�������A��]���̕\���̈��ϊ�����B
            If tProp_ImgKind_Rotate90 Then
                Me.tProp_AriaInf_Aria = New Size(m_cntPic.ClientSize.Height, m_cntPic.ClientSize.Width)
            Else
                Me.tProp_AriaInf_Aria = m_cntPic.ClientSize
            End If
        End Sub

        ''' <summary>
        ''' �������`��ɕK�v�ȃI�u�W�F�N�g��������
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub CreateMemoriObj()
            If Not m_memoriBsh Is Nothing Then m_memoriBsh.Dispose()
            If Not m_memoriPen Is Nothing Then m_memoriPen.Dispose()
            If Not m_memoriFnt Is Nothing Then m_memoriFnt.Dispose()

            m_memoriBsh = New SolidBrush(m_Memori.Color)
            m_memoriPen = New System.Drawing.Pen(m_Memori.Color, m_Memori.LineWidth)
            m_memoriFnt = New System.Drawing.Font("�l�r �S�V�b�N", m_Memori.FontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        End Sub


#Region " IDisposable Support "
        Private disposedValue As Boolean = False        ' �d������Ăяo�������o����ɂ�

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��Ƀ}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
                If Not m_memoriBsh Is Nothing Then m_memoriBsh.Dispose()
                If Not m_memoriPen Is Nothing Then m_memoriPen.Dispose()
                If Not m_memoriFnt Is Nothing Then m_memoriFnt.Dispose()
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
        '/ �C���[�W�I�u�W�F�N�g �֌W
        '/ /////////////////////////////////////////////////////////////////////////////////
#Region "�C���[�W�I�u�W�F�N�g �Z�b�g"
        ''' <summary>
        ''' �C���[�W�I�u�W�F�N�g���Z�b�g (1�摜�݂̂̐�p)
        ''' </summary>
        ''' <param name="img">�C���[�W�N���X</param>
        ''' <param name="bDispose">false:�O�摜�̊J������</param>
        ''' <remarks>Nothing��n���΁A�J���݂̂ƂȂ�</remarks>
        Public Overloads Sub SetImage(ByRef img As Image, Optional ByVal bDispose As Boolean = False)
            Try
                ' 1�摜���[�h���H
                If m_ImgInf.nImgCntX <> 1 Or m_ImgInf.nImgCntY <> 1 Then Return

                ' ���݂̃C���[�W���J��
                If bDispose Then
                    If Not m_Img Is Nothing AndAlso Not m_Img(0) Is Nothing Then m_Img(0).Dispose()
                End If


                ' �V�����C���[�W���Z�b�g
                ReDim m_Img(0)
                m_Img(0) = img

                '' ���]��ԏ�����
                'm_OldTurnLR = False
                'm_OldTurnTB = False
            Catch ex As Exception
                m_Img = Nothing
            End Try
        End Sub

        ''' <summary>
        ''' �C���[�W�I�u�W�F�N�g���Z�b�g (�����摜�̐�p)
        ''' </summary>
        ''' <param name="img">�C���[�W�N���X (�Z�b�g���Ԃ�Z���ƂȂ�悤�ɉ摜���Z�b�g����)</param>
        ''' <param name="bDispose">false:�O�摜�̊J������</param>
        ''' <remarks>Nothing��n���΁A�J���݂̂ƂȂ�</remarks>
        Public Overloads Sub SetImage(ByRef img As Image(), Optional ByVal bDispose As Boolean = False)
            Try
                Dim imgcnt As Integer = m_ImgInf.nImgCntX * m_ImgInf.nImgCntY
                ' �w�薇���摜������H
                If imgcnt <> img.Length Then Return

                '' ���݂̃C���[�W���J��
                If bDispose Then
                    If Not m_Img Is Nothing Then
                        For ii As Integer = 0 To m_Img.Length - 1
                            m_Img(ii).Dispose()
                            m_Img(ii) = Nothing
                        Next ii
                    End If
                End If

                ' �V�����C���[�W���Z�b�g
                ReDim m_Img(imgcnt - 1)
                For ii As Integer = 0 To imgcnt - 1
                    m_Img(ii) = img(ii)
                Next ii

                '' ���]��ԏ�����
                'm_OldTurnLR = False
                'm_OldTurnTB = False
            Catch ex As Exception
                m_Img = Nothing
            End Try
        End Sub

        Public Overloads Sub GetImage(ByRef img As Image())
            img = m_Img
        End Sub



        ''' <summary>
        ''' �o�C�i���f�[�^����C���[�W�I�u�W�F�N�g���쐬(Jepg�`��)
        ''' </summary>
        ''' <param name="bytData"></param>
        ''' <remarks></remarks>
        Public Overloads Sub SetImage(ByVal bytData() As Byte)
            ' �C���[�W�Z�b�g
            Call SetImage(tClass.tClass_BmpJpeg.DataToImage(bytData))
        End Sub

        ''' <summary>
        ''' �o�C�i���f�[�^����C���[�W�I�u�W�F�N�g���쐬(Bitmap�`��)
        ''' </summary>
        ''' <param name="nWidth">�摜�̕�</param>
        ''' <param name="nHeight">�摜�̍���</param>
        ''' <param name="bytData">�o�C�i���f�[�^</param>
        ''' <remarks></remarks>
        Public Overloads Sub SetImage(ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal bytData() As Byte)
            ' �C���[�W�Z�b�g (�r�b�g�}�b�v�w�b�_�����킹�ĕt�^)
            Call SetImage(tClass.tClass_BmpJpeg.DataToBmp(nWidth, nHeight, bytData))
        End Sub

        ''' <summary>
        ''' �t�@�C������C���[�W�I�u�W�F�N�g���Z�b�g
        ''' </summary>
        ''' <param name="strFPath">�t���p�X</param>
        ''' <remarks></remarks>
        Public Overloads Sub SetImage(ByVal strFPath As String)
            ' �C���[�W�Z�b�g
            Call SetImage(tClass.tClass_BmpJpeg.BmpFileLoad(strFPath))
        End Sub



        ''' <summary>
        ''' �f�t�H���g�C���[�W�I�u�W�F�N�g���Z�b�g
        ''' </summary>
        ''' <param name="img">�C���[�W�N���X</param>
        ''' <remarks>Nothing��n���΁A�J���݂̂ƂȂ�</remarks>
        Public Overloads Sub SetDefImage(ByRef img As Image)
            Try
                ' ���݂̃C���[�W���J��
                If Not m_DefImg Is Nothing Then m_DefImg.Dispose()

                ' �V�����C���[�W���Z�b�g
                m_DefImg = img

            Catch ex As Exception
                m_DefImg = Nothing
            End Try
        End Sub

        ''' <summary>
        ''' �o�C�i���f�[�^����f�t�H���g�C���[�W�I�u�W�F�N�g���쐬(Jepg�`��)
        ''' </summary>
        ''' <param name="bytData"></param>
        ''' <remarks></remarks>
        Public Overloads Sub SetDefImage(ByVal bytData() As Byte)
            ' �C���[�W�Z�b�g
            Call SetDefImage(tClass.tClass_BmpJpeg.DataToImage(bytData))
        End Sub

        ''' <summary>
        ''' �o�C�i���f�[�^����f�t�H���g�C���[�W�I�u�W�F�N�g���쐬(Bitmap�`��)
        ''' </summary>
        ''' <param name="nWidth">�摜�̕�</param>
        ''' <param name="nHeight">�摜�̍���</param>
        ''' <param name="bytData">�o�C�i���f�[�^</param>
        ''' <remarks></remarks>
        Public Overloads Sub SetDefImage(ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal bytData() As Byte)
            ' �C���[�W�Z�b�g (�r�b�g�}�b�v�w�b�_�����킹�ĕt�^)
            Call SetDefImage(tClass.tClass_BmpJpeg.DataToBmp(nWidth, nHeight, bytData))
        End Sub

        ''' <summary>
        ''' �t�@�C������f�t�H���g�C���[�W�I�u�W�F�N�g���Z�b�g
        ''' </summary>
        ''' <param name="strFPath">�t���p�X</param>
        ''' <remarks></remarks>
        Public Overloads Sub SetDefImage(ByVal strFPath As String)
            ' �C���[�W�Z�b�g
            Call SetDefImage(tClass.tClass_BmpJpeg.BmpFileLoad(strFPath))
        End Sub



        ''' <summary>
        ''' ���݂̕\���摜
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function GetImage() As Image()
            Return m_Img
        End Function
        ''' <summary>
        ''' �f�t�H���g�C���[�W
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function GetDefImage() As Image
            Return m_DefImg
        End Function
#End Region


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �`�� �֌W
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' �ĕ`��
        ''' </summary>
        Public Sub Refresh()
            If m_cntPic Is Nothing Then Return
            'm_cntPic.Update()
            m_cntPic.Invalidate()   ' ��󂫂Ȃ�`��
            'm_cntPic.Refresh()
        End Sub
        '�����ĕ`��
        Public Sub RefreshManual()
            If m_cntPic Is Nothing Then Return
            m_cntPic.Refresh()
        End Sub

        ''' <summary>
        ''' ���C���`��
        ''' </summary>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Protected Overridable Sub Paint_Main(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
            '' �`��L�� (��O��)
            If Not m_IsPaint Then Exit Sub

            '90�x��]��Resize�ɂ�炸�ς��̂ŁA����₢���킹��B
            '���F�s�N�`���R���g���[���̃T�C�Y�₢���킹�Ȃ̂ł����������Ƃ͂Ȃ�
            SetAriaInf()

            '' �f�[�^��ނɂ��摜���@�ؑ�
            If m_IsDefImgDsp Then
                Paint_Main_Non(e)
            Else
                Paint_Main_Img(e)
            End If
        End Sub

        ''' <summary>
        ''' �f�[�^�����p�C���[�W�`��
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_Main_Non(ByVal e As System.Windows.Forms.PaintEventArgs)
            '�Ƃ肠�����ATransform�͌������Ă����B90�x��]���Ă����m_AriaInf������������B
            e.Graphics.Transform = SetupTransform(New Point(0, 0), New Size(m_AriaInf.nAriaWidth, m_AriaInf.nAriaHeight))

            e.Graphics.DrawImage(m_DefImg, New Rectangle(0, 0, m_AriaInf.nAriaWidth, m_AriaInf.nAriaHeight))

            '' ���[�U�[�ŏI�`��
            RaiseEvent tEv_Paint_NonEnd(e)
            e.Graphics.ResetTransform()
        End Sub

        ''' <summary>
        ''' �f�[�^�L��p�C���[�W�`��
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_Main_Img(ByVal e As System.Windows.Forms.PaintEventArgs)
            '// =========================================
            '// ���Ԃ��厖�ȈׁA�������́A�C�����鎖
            '//

            '' �v�Z
            Calculation()

            '' �����`��
            Paint_Start(e)
            '' ���[�U�[�����`��
            RaiseEvent tEv_Paint_Start(e)

            '' �摜��`��
            If m_ImgInf.nImgCntX = 1 AndAlso m_ImgInf.nImgCntY = 1 Then
                Paint_Img(e)                ' 1���̊G
            Else
                Paint_ImgArray(e)           ' �������̊G
            End If

            '' �������`��
            Paint_Memori(e)

            '' �ŏI�`��
            Paint_End(e)
            '' ���[�U�[�ŏI�`��
            RaiseEvent tEv_Paint_End(e)
        End Sub


        ''' <summary>
        ''' �f�o�b�O�p�F���܂��ɂ��Ă���ϊ��s��ŁAr�͂ǂ̂悤�ɕϊ�����邩�B
        ''' </summary>
        ''' <param name="g">Graphics</param>
        ''' <param name="r">�l�p���W</param>
        ''' <returns>�ϊ���i�ŏI�I�ɕ`�������W�j</returns>
        ''' <remarks></remarks>
        Private Function testtransform(ByVal g As Graphics, ByVal r As RectangleF) As RectangleF
            Dim pts() As PointF = {New PointF(r.Left, r.Top), New PointF(r.Right, r.Bottom)}

            g.Transform().TransformPoints(pts)
            Return New RectangleF(pts(0).X, pts(0).Y, pts(1).X - pts(0).X, pts(1).Y - pts(0).Y)
        End Function


        ''' <summary>
        ''' �摜�]���p�̕ϊ�affine�s���p�ӂ���
        ''' </summary>
        ''' <param name="ptDst">������W�i�ϊ��O��j</param>
        ''' <param name="szDst">�摜�̃T�C�Y�i��]���S�j</param>
        ''' <returns>�ϊ��p�s��</returns>
        ''' <remarks>m_ImgKind�́A��]�A�����]�A���蔽�]�̉e���������܂�</remarks>
        Private Function SetupTransform(ByVal ptDst As Point, ByVal szDst As Size) As System.Drawing.Drawing2D.Matrix

            '�������B[1,0], [1,0], [0,0]
            Dim matrix As New System.Drawing.Drawing2D.Matrix(1, 0, 0, 1, 0, 0)

            '�܂��͉�]�̂��߂ɁA�摜�̒��S�ցB
            Dim ptCenter As PointF = New PointF(Math.Abs(szDst.Width) / 2.0F, Math.Abs(szDst.Height) / 2.0F)
            matrix.Translate(-ptCenter.X, -ptCenter.Y, Drawing2D.MatrixOrder.Append)

            '�������̔��]
            If (m_ImgKind.bIsTurnLR) Then
                matrix.Multiply(New System.Drawing.Drawing2D.Matrix(-1, 0, 0, 1, 0, 0), Drawing2D.MatrixOrder.Append)
            End If

            '��������̔��]
            If (m_ImgKind.bIsTurnTB) Then
                matrix.Multiply(New System.Drawing.Drawing2D.Matrix(1, 0, 0, -1, 0, 0), Drawing2D.MatrixOrder.Append)
            End If

            '��]
            If m_ImgKind.bIsRotate90 Then
                matrix.Multiply(New System.Drawing.Drawing2D.Matrix(0, -1, 1, 0, 0, 0), Drawing2D.MatrixOrder.Append)
            End If

            '�摜���S����A�]����֕ϊ�
            If m_ImgKind.bIsRotate90 Then
                matrix.Translate(ptCenter.Y + ptDst.Y, ptCenter.X + ptDst.X, Drawing2D.MatrixOrder.Append)
            Else
                matrix.Translate(ptCenter.X + ptDst.X, ptCenter.Y + ptDst.Y, Drawing2D.MatrixOrder.Append)
            End If

            Return matrix
        End Function



        ''' <summary>
        ''' �摜�`��
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_Img(ByVal e As System.Windows.Forms.PaintEventArgs)

            '''' ���̂����́A�ȒP�����A���ۂ̉摜�Ɏ��������ׁA�摜�̎g���܂킵�Ȃǂł������Ȃ��ƂƂȂ�ׁA����
            '' ���E���]
            'If m_OldTurnLR <> m_ImgKind.bIsTurnLR Then
            '	m_Img(0).RotateFlip(RotateFlipType.RotateNoneFlipX)
            '	m_OldTurnLR = m_ImgKind.bIsTurnLR
            'End If

            '' �㉺���]
            'If m_OldTurnTB <> m_ImgKind.bIsTurnTB Then
            '	m_Img(0).RotateFlip(RotateFlipType.RotateNoneFlipY)
            '	m_OldTurnTB = m_ImgKind.bIsTurnTB
            'End If

            '' �`��
            'e.Graphics.DrawImage(m_Img(0), m_CalcAns.ImgDspRect)



            'Dim x, y, w, h As Single
            '' ���E���]
            'If m_ImgKind.bIsTurnLR Then
            '    x = m_CalcAns.ImgDspRect.Width + m_CalcAns.ImgDspRect.X
            '    w = m_CalcAns.ImgDspRect.Width * -1
            'Else
            '    x = m_CalcAns.ImgDspRect.X
            '    w = m_CalcAns.ImgDspRect.Width
            'End If
            '' �㉺���]
            'If m_ImgKind.bIsTurnTB Then
            '    y = m_CalcAns.ImgDspRect.Height + m_CalcAns.ImgDspRect.Y
            '    h = m_CalcAns.ImgDspRect.Height * -1
            'Else
            '    y = m_CalcAns.ImgDspRect.Y
            '    h = m_CalcAns.ImgDspRect.Height
            'End If

            '' �`��
            'e.Graphics.DrawImage(m_Img(0), x, y, w, h)

            Try
                Dim dst As Rectangle = m_CalcAns.ImgDspRect
                e.Graphics.Transform = SetupTransform(dst.Location, dst.Size)
                If m_Img(0) IsNot Nothing Then
                    e.Graphics.DrawImage(m_Img(0), New Rectangle(0, 0, dst.Width, dst.Height))
                End If
                e.Graphics.ResetTransform()
            Catch ex As Exception

            End Try
        End Sub

        ''' <summary>
        ''' �摜�`��
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_ImgArray(ByVal e As System.Windows.Forms.PaintEventArgs)
            Dim g As Graphics = e.Graphics

            Try
                ' �`��
                For yy As Integer = 0 To m_ImgInf.nImgCntY - 1
                    For xx As Integer = 0 To m_ImgInf.nImgCntX - 1
                        Dim nIndex As Integer = (yy * m_ImgInf.nImgCntX) + xx
                        If nIndex >= m_Img.Length OrElse m_Img(nIndex) Is Nothing Then Continue For
                        Dim img As Image = m_Img(nIndex)

                        ' Z�̏��ɕ`�悷��
                        Try
                            '���X���b�h��img�͔j�����ꂿ�����
                            If img.PixelFormat = Imaging.PixelFormat.DontCare Then
                                Continue For
                            End If

                            'g.DrawImage(m_Img(nIndex), GetAnsOneImgSize(jj, ii))

                            Dim rect As Rectangle = GetAnsOneImgSize(xx, yy)
                            '����ŁA��]�����蔽�]�����肵�Ȃ���Arect.Left�rect.TOP�֓]�������悤�ɂȂ�
                            g.Transform = SetupTransform(rect.Location, rect.Size)

                            '�����傫������ƕ`��Ɏ��s���邱�Ƃ�����̂ŁA�`��̈�𐧌�����
                            If img.Width > 4096 Then
                                'X�ł̕`��͈͂́A�O�`�E�C���h�E���܂łł悢�B
                                '�ϊ��̋t�s��Ō��摜�͈͂����Ƃ߂�
                                Dim dWidth As Integer = tProp_AriaInf_Aria.Width
                                Dim matInv As System.Drawing.Drawing2D.Matrix = g.Transform
                                matInv.Invert()

                                Dim Pt() As Point = New Point() {New Point(0, 0), New Point(dWidth, dWidth)}
                                matInv.TransformPoints(Pt)

                                '��X���W�����܂���
                                '�i�k����E��]���O���W�j
                                Dim xStSmall As Integer = Math.Min(Pt(0).X, Pt(1).X)
                                Dim xSzSmall As Integer = Math.Abs(Pt(0).X - Pt(1).X)
                                '�i�摜���W�j
                                Dim xStLarge As Integer = Math.Min(Pt(0).X, Pt(1).X) * img.Width \ rect.Width
                                Dim xSzLarge As Integer = Math.Abs(Pt(0).X - Pt(1).X) * img.Width \ rect.Width


                                '���W�v�Z�����Ȃ���
                                g.Transform = SetupTransform(New Point(0, rect.Y), New Size(xSzSmall, img.Height))

                                '���摜�̍��W��Draw
                                g.DrawImage(img, New Rectangle(0, 0, dWidth, rect.Height), _
                                  New Rectangle(xStLarge, 0, xSzLarge, img.Height), GraphicsUnit.Pixel)

                            Else


                                If rect.Width = img.Width And rect.Height = img.Height Then
                                    g.DrawImage(img, New Point(0, 0))
                                Else
                                    '�]������W�ʒu��0,0�ȗ��R�́A���łɍs�񒆂Ɋ܂܂�邩��B
                                    g.DrawImage(img, New Rectangle(0, 0, rect.Width, rect.Height))
                                End If
                            End If

                            '�f�o�b�O�F�摜�̔ԍ��ƍ����̕\��
                            If False Then
                                Dim disp As String = String.Format("{0} : {1}, {2} {3} {4}", _
                                      yy, m_Img(nIndex).Height, rect.Height, IIf(rect.Height = img.Height, "��", "�~"), rect.Width)
                                Using f As Font = New Font("MS Gothic", 20, FontStyle.Bold, GraphicsUnit.Pixel)
                                    g.DrawString(disp, f, Brushes.Blue, New PointF(rect.Width / 4.0F, rect.Height / 2.0F))
                                End Using
                            End If

                            '�f�o�b�O�F���W�ϊ��������Ƃ̊m�F�p
                            If False And nIndex = 0 Then
                                Dim ptR As New PointF(rect.Right, rect.Bottom)
                                Console.WriteLine("{2}: {0} -> {1} ", _
                                   rect, _
                                   testtransform(g, rect), _
                                   nIndex)
                            End If


                            '�ǂݍ��݂ňُ킪������ƁAimg.tag�ɂ��̗�O������Ă���A
                            '������`�悷��. img���Ԃ�Nothing�͍l���Ȃ��B
                            Dim ex As Exception = TryCast(img.Tag, Exception)
                            If ex IsNot Nothing Then
                                Dim rcd As RectangleF = testtransform(g, New RectangleF(0, 0, rect.Width, rect.Height))
                                If rcd.X < 0 Then rcd.X = 0
                                If rcd.Height < 0 Then rcd.Y -= 16

                                Dim exception_1st_stack As String = GetTopFunctionOfStacktrace(ex)
                                Dim disp As String = String.Format("{0} @ {1}", ex.Message, exception_1st_stack)

                                g.ResetTransform()

                                Using f As Font = New Font("MS Gothic", 16, FontStyle.Bold, GraphicsUnit.Pixel)
                                    g.DrawString(disp, f, Brushes.Blue, New PointF(rcd.X, rcd.Y))
                                End Using

                            End If

                            '�f�o�b�O�F�t���[����؂�̕\��()
                            If False Then
                                g.ResetTransform()
                                If tProp_ImgKind_Rotate90 Then
                                    g.DrawRectangle(Pens.Aqua, New Rectangle(rect.Top, rect.Left, rect.Height, rect.Width))
                                Else
                                    g.DrawRectangle(Pens.Aqua, rect)
                                End If
                            End If

                        Catch ex As Exception
                        End Try

                    Next xx
                Next yy
            Catch ex As Exception
            End Try
            g.ResetTransform()
        End Sub


        ''' <summary>
        ''' ��O�̃X�^�b�N�g���[�X�́A�����΂��̊֐������������B
        ''' </summary>
        Private Shared Function GetTopFunctionOfStacktrace(ByVal ex As Exception) As String
            Dim strace As String = ex.StackTrace().Split(vbCr(0))(0)
            Dim stack_for_msg As String = strace

            Dim splitted_msg() As String = strace.Split("\:.".ToCharArray)
            Dim last_item As Integer = splitted_msg.Length - 1
            If splitted_msg.Length > 4 Then
                stack_for_msg = splitted_msg(last_item - 2) & "." & splitted_msg(last_item - 1) & " " & splitted_msg(last_item)
            End If
            If stack_for_msg.Contains("(") Then
                stack_for_msg = splitted_msg(last_item)
            End If
            Return stack_for_msg
        End Function


        ''' <summary>
        ''' �������`��
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Paint_Memori(ByVal e As System.Windows.Forms.PaintEventArgs)
            If Not m_Memori.bIsDsp Then Exit Sub

            Dim ii As Integer
            Dim nPosWk As Integer
            Dim strValWk As String

            'm_AriaInf�́AHeight�G���葤��ʃT�C�Y�AWidth�G��������ʃT�C�Y�ł��邱�Ƃɒ��ӁI
            Dim nPosLineTop As Integer
            If Not tProp_ImgKind_Rotate90 Then
                nPosLineTop = m_AriaInf.nAriaHeight - m_Memori.nPosLineBot
            Else
                nPosLineTop = m_AriaInf.nAriaWidth - m_Memori.nPosLineBot
            End If

            Dim nPosStringTop As Integer = nPosLineTop - m_Memori.nPosStringBot + m_Memori.nPosLineBot

            ' �����������ʒu�v�Z
            Dim habaLabels As New List(Of tClass_ImageDspAxisDrawner.LabelInfo)

            If 0 < m_Memori.nLoopCntX Then
                ' ������
                For ii = 1 To m_Memori.nLoopCntX
                    nPosWk = CInt(ii * m_Memori.nIntervalX * m_CalcAns.RateRes.dW)
                    strValWk = CInt(ii * m_Memori.nIntervalX).ToString                                      ' �`�敶��
                    habaLabels.Add(New tClass_ImageDspAxisDrawner.LabelInfo(nPosWk, strValWk))
                Next ii
            End If

            ' ���莲�������ʒu�v�Z
            Dim nagateLabels As New List(Of tClass_ImageDspAxisDrawner.LabelInfo)
            If 0 < m_Memori.nLoopCntY Then
                For ii = 1 To m_Memori.nLoopCntY
                    nPosWk = CInt(ii * m_Memori.nIntervalY * m_CalcAns.RateRes.dH)
                    strValWk = CInt(ii * m_Memori.nIntervalY).ToString                                      ' �`�敶��
                    nagateLabels.Add(New tClass_ImageDspAxisDrawner.LabelInfo(nPosWk, strValWk))
                Next ii
            End If

            '�������ł����̂Ń�������`�悷��
            Dim axis As New tClass_ImageDspAxisDrawner(e.Graphics, m_Memori.HojoLineLen, New Point(m_Memori.nPosLineLeft, nPosLineTop))
            axis.UpdateMemoriPenBrushFont(Me.m_memoriBsh, Me.m_memoriFnt, Me.m_memoriPen)
            axis.IsFlipV = True
            axis.IsStringOutside = True
            If Not tProp_ImgKind_Rotate90 Then
                axis.draw_x_axis(habaLabels, nPosLineTop)
                axis.draw_x_line(habaLabels, nPosLineTop)
                axis.draw_y_axis(nagateLabels, m_Memori.nPosLineLeft)
                axis.draw_y_line(nagateLabels, m_Memori.nPosLineLeft)
            Else
                axis.draw_x_axis(nagateLabels, nPosLineTop)
                axis.draw_x_line(nagateLabels, nPosLineTop)
                axis.draw_y_axis(habaLabels, m_Memori.nPosLineLeft)
                axis.draw_y_line(habaLabels, m_Memori.nPosLineLeft)
            End If

            ' ���_�\��
            e.Graphics.DrawString(m_Memori.strBaseString, m_memoriFnt, m_memoriBsh, m_Memori.nPosStringLeft, nPosStringTop)
        End Sub



        ''' <summary>
        ''' �����`��
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub Paint_Start(ByVal e As System.Windows.Forms.PaintEventArgs)
            ' ����ɏ���������OK
        End Sub

        ''' <summary>
        ''' �ŏI�`��
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
            ' ����ɏ���������OK
        End Sub

    End Class




    '*********************************************************************************
    '�摜�\��Base�N���X
    '	[Ver]
    '		Ver.01    2008/09/06  vs2005 �Ή�
    '
    '	[����]
    '		����Ӗ��r����p�H�H
    '*********************************************************************************
    ''' <summary>���W�Z�o�N���X</summary>
    Public Class tClass_ImageDsp_Calc
        'Implements IDisposable							' �f�X�g���N�^��`

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �萔��`
        '/ /////////////////////////////////////////////////////////////////////////////////
#Region "�萔��`"
        ''' <summary>�摜�\���`��</summary>
        Public Enum EM_IMG_STRECH
            ''' <summary>0:���{�\��(�c����ێ��B1��f1mm)</summary>
            emNormal = 0
            ''' <summary>1:�\���G���A�ɉ摜�����킹��</summary>
            emStrechBoth
            ''' <summary>2:�\���G���A�̍����ɉ摜���������킹��(�c����ێ�)</summary>
            emStrechHeight
            ''' <summary>3:�\���G���A�̕��ɉ摜�������킹��(�c����ێ�)</summary>
            emStrechWidth
            ''' <summary>4:�摜�T�C�Y�̑傫�������\���̈�Ɉ�v����悤�ɍ��킹��(�c����ێ�)</summary>
            emJustBig
            ''' <summary>5:�摜�T�C�Y�̏����������\���̈�Ɉ�v����悤�ɍ��킹��(�c����ێ�)</summary>
            emJustSmall

            ''' <summary>6:���摜�T�C�Y</summary>
            emRaw

            ''' <summary>7:���̕���\����Ƃ��āA�c�����킹�� (�c����ێ�)</summary>
            emRawWidth
            ''' <summary>8:�c�̕���\����Ƃ��āA�������킹�� (�c����ێ�)</summary>
            emRawHeight
        End Enum

        ''' <summary>�{��[10-2% (100%=1)]</summary>
        Public Structure RATE
            ''' <summary>���{��</summary>
            Public dW As Double
            ''' <summary>�c�{��</summary>
            Public dH As Double

            ''' <summary>������</summary>
            Public Sub Initialize()
                dW = 1.0
                dH = 1.0
            End Sub
            Public Sub New(ByVal dWwk As Double, ByVal dHwk As Double)
                dW = dWwk
                dH = dHwk
            End Sub
        End Structure

        Public Structure SizeD
            Public Height As Double
            Public Width As Double
            Public Sub New(ByVal dWwk As Double, ByVal dHwk As Double)
                Width = dWwk
                Height = dHwk
            End Sub
        End Structure

        ''' <summary>�摜���</summary>
        Public Structure IMG_INF
            ''' <summary>1���̉摜 �� [pixel]</summary>
            Public nOneImgW As Integer
            ''' <summary>1���̉摜 ���� [pixel]</summary>
            Public nOneImgH As Integer

            ''' <summary>�\���摜����</summary>
            Public nImgCntX As Integer
            ''' <summary>�\���摜����</summary>
            Public nImgCntY As Integer

            ''' <summary>���v �摜 �� [pixel]</summary>
            Public nImgW As Integer
            ''' <summary>���v �摜 ���� [pixel]</summary>
            Public nImgH As Integer
            ''' <summary>�摜 ������\ [mm/pixel]</summary>
            Public dResW As Double
            ''' <summary>�摜 �c����\ [mm/pixel]</summary>
            Public dResH As Double

            ''' <summary>���S�ʒuY [pixel]</summary>
            Public nCenterY As Integer
            ''' <summary>���S�ʒuX [pixel]</summary>
            Public nCenterX As Integer


            ''' <summary>�摜 �� [mm] (�����Z�o�B1��f=1mm�ƂȂ��Ă���)</summary>
            Public dSizeW As Double
            ''' <summary>�摜 ���� [mm] (�����Z�o�B1��f=1mm�ƂȂ��Ă���)</summary>
            Public dSizeH As Double
        End Structure

        ''' <summary>�\���`�����</summary>
        Public Structure IMG_KIND
            ''' <summary>�摜�\���`��</summary>
            Public emImgStrech As EM_IMG_STRECH
            ''' <summary>�摜�\���{��(�����Z�o)(����\������������̉摜����̔{��)</summary>
            Public StrechRate As RATE

            ''' <summary>�摜�㉺���]�t���O(Ture:���],False:���]�Ȃ�)</summary>
            Public bIsTurnTB As Boolean
            ''' <summary>�摜���E���]�t���O(Ture:���],False:���]�Ȃ�)</summary>
            Public bIsTurnLR As Boolean

            ''' <summary>�摜��]�t���O(Ture:��],False:�Ȃ�)</summary>
            Public bIsRotate90 As Boolean
        End Structure

        ''' <summary>�\���̈���</summary>
        Public Structure ARIA_INF
            ''' <summary>�\���̈� ����</summary>
            Public nAriaHeight As Integer
            ''' <summary>�\���̈� ��</summary>
            Public nAriaWidth As Integer

            ''' <summary>�\���{�� [mm/pixel]</summary>
            Public Zoom As RATE

            ''' <summary>���S�ʒu��\���G���A�̒��S�Ɉړ�(Ture:���S�ֈړ�,False:�蓮�I�t�Z�b�g)</summary>
            Public bIsCenter As Boolean

            ''' <summary>�I�t�Z�b�g�ʒu (bIsCenter�œ��삪�ς�� true�������Z�o) [DP]</summary>
            Public Offset As Point
        End Structure

        ''' <summary>���W�ŏI����</summary>
        Public Structure CALC_ANS
            ''' <summary>�ŏI����\[mm/pixel] (�\�����Ă���摜�ɑ΂��镪��\)</summary>
            Public FinalRes As RATE

            ''' <summary>�ŏI�{�� [pixel/mm] (����\������������̉摜����̔{��)</summary>
            Public RateRes As RATE

            ''' <summary>�ŏI�\���{�� (���摜����̔{��)</summary>
            Public RateDsp As RATE

            ''' <summary>�{�����f��̑S�̂̉摜�T�C�Y,����̈ʒu</summary>
            Public ImgDspRect As Rectangle


            '''' <summary>�ŏI�摜�T�C�Y(�����Ă��Ȃ��������܂�)</summary>
            'Public ImgSize As Size
            '''' <summary>�\���摜�T�C�Y(�����Ă��镔������)</summary>
            'Public ImgDspSize As Size
            ''' <summary>�\���G���A�ɕ\������Ă��Ȃ������̉摜�T�C�Y</summary>
            Public ImgNonAriaSize As Size

        End Structure
#End Region




        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����o�[�ϐ�
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>�摜���</summary>
        Protected m_ImgInf As IMG_INF                       ' �摜���
        ''' <summary>�\���`�����</summary>
        Protected m_ImgKind As IMG_KIND                     ' �\���`�����
        ''' <summary>�\���̈���</summary>
        Protected m_AriaInf As ARIA_INF                     ' �\���̈���
        ''' <summary>���W�ŏI����</summary>
        Protected m_CalcAns As CALC_ANS                     ' ���W�ŏI����

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �v���p�e�B
        '/ /////////////////////////////////////////////////////////////////////////////////
#Region "�摜���"
        ''' <summary>
        ''' ���W�ŏI����
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�摜���") _
         , System.ComponentModel.Description("�摜���")> _
        Public Property tProp_ImgInf() As IMG_INF
            Get
                Return m_ImgInf
            End Get
            Set(ByVal value As IMG_INF)
                m_ImgInf = value
            End Set
        End Property

#End Region

#Region "�\���`�����"
        ''' <summary>
        ''' ���W�ŏI����
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���`�����") _
         , System.ComponentModel.Description("�\���`�����")> _
        Public Property tProp_ImgKind() As IMG_KIND
            Get
                Return m_ImgKind
            End Get
            Set(ByVal value As IMG_KIND)
                m_ImgKind = value
            End Set
        End Property


        ''' <summary>
        ''' �摜�\���`��
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���`�����-��p�v���p�e�B") _
         , System.ComponentModel.Description("�摜�\���`��")> _
        Public Property tProp_ImgKind_StrechType() As EM_IMG_STRECH
            Get
                Return m_ImgKind.emImgStrech
            End Get
            Set(ByVal value As EM_IMG_STRECH)
                m_ImgKind.emImgStrech = value
            End Set
        End Property

        ''' <summary>
        ''' �摜�㉺���]�t���O(Ture:���],False:���]�Ȃ�)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���`�����-��p�v���p�e�B") _
         , System.ComponentModel.Description("�摜�㉺���]�t���O(Ture:���],False:���]�Ȃ�)")> _
        Public Property tProp_ImgKind_IsTurnTB() As Boolean
            Get
                Return m_ImgKind.bIsTurnTB
            End Get
            Set(ByVal value As Boolean)
                m_ImgKind.bIsTurnTB = value
            End Set
        End Property

        ''' <summary>
        ''' �摜���E���]�t���O(Ture:���],False:���]�Ȃ�)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���`�����-��p�v���p�e�B") _
         , System.ComponentModel.Description("�摜���E���]�t���O(Ture:���],False:���]�Ȃ�)")> _
        Public Property tProp_ImgKind_IsTurnLR() As Boolean
            Get
                Return m_ImgKind.bIsTurnLR
            End Get
            Set(ByVal value As Boolean)
                m_ImgKind.bIsTurnLR = value
            End Set
        End Property


        ''' <summary>
        ''' �摜�X�O�x��]�t���O(True:�R�C���͉��ɗ����,False:�R�C���͏c�ɗ����)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���`�����-��p�v���p�e�B") _
         , System.ComponentModel.Description(" �摜�X�O�x��]�t���O(True:�R�C���͉��ɗ����,False:�R�C���͏c�ɗ����)")> _
        Public Property tProp_ImgKind_Rotate90() As Boolean
            Get
                Return m_ImgKind.bIsRotate90
            End Get
            Set(ByVal value As Boolean)
                m_ImgKind.bIsRotate90 = value
            End Set

        End Property



        ''' <summary>
        ''' �摜�\���{��(�����Z�o)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���`�����-��p�v���p�e�B") _
         , System.ComponentModel.Description("�摜�\���{��(�����Z�o)")> _
        Public ReadOnly Property tProp_ImgKind_StrechRate() As RATE
            Get
                Return m_ImgKind.StrechRate
            End Get
        End Property
#End Region

#Region "�\���̈���"
        ''' <summary>
        ''' �\���̈���
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���") _
         , System.ComponentModel.Description("�\���̈���")> _
        Public Property tProp_AriaInf() As ARIA_INF
            Get
                Return m_AriaInf
            End Get
            Set(ByVal value As ARIA_INF)
                m_AriaInf = value
            End Set
        End Property



        ''' <summary>
        ''' �\���̈�
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�\���̈�")> _
        Public Property tProp_AriaInf_Aria() As Size
            Get
                Dim s As Size
                s.Width = m_AriaInf.nAriaWidth
                s.Height = m_AriaInf.nAriaHeight
                Return s
                'Return New Size(m_AriaInf.nAriaWidth, m_AriaInf.nAriaHeight)
            End Get
            Set(ByVal value As Size)
                m_AriaInf.nAriaHeight = value.Height
                m_AriaInf.nAriaWidth = value.Width
            End Set
        End Property

        ''' <summary>
        ''' �\���̈� ��
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�\���̈� ��")> _
        Public WriteOnly Property tProp_AriaInf_AriaWidth() As Integer
            Set(ByVal value As Integer)
                m_AriaInf.nAriaWidth = value
            End Set
        End Property
        ''' <summary>
        ''' �\���̈� ����
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�\���̈� ����")> _
        Public WriteOnly Property tProp_AriaInf_AriaHeight() As Integer
            Set(ByVal value As Integer)
                m_AriaInf.nAriaHeight = value
            End Set
        End Property

        ''' <summary>
        ''' �\���{��
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�\���{��")> _
        Public Property tProp_AriaInf_Zoom() As RATE
            Get
                Return m_AriaInf.Zoom
            End Get
            Set(ByVal value As RATE)
                m_AriaInf.Zoom = value
            End Set
        End Property
        ''' <summary>
        ''' �\���{�� ��
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�\���{�� ��")> _
        Public Property tProp_AriaInf_ZoomW() As Double
            Get
                Return m_AriaInf.Zoom.dW
            End Get
            Set(ByVal value As Double)
                m_AriaInf.Zoom.dW = value
            End Set
        End Property
        ''' <summary>
        ''' �\���{�� �c
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�\���{�� �c")> _
        Public Property tProp_AriaInf_ZoomH() As Double
            Get
                Return m_AriaInf.Zoom.dH
            End Get
            Set(ByVal value As Double)
                m_AriaInf.Zoom.dH = value
            End Set
        End Property
        ''' <summary>
        ''' ���S�ʒu��\���G���A�̒��S�Ɉړ�(Ture:���S�ֈړ�,False:�蓮�I�t�Z�b�g)
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("���S�ʒu��\���G���A�̒��S�Ɉړ�(Ture:���S�ֈړ�,False:�蓮�I�t�Z�b�g)")> _
        Public Property tProp_AriaInf_IsCenter() As Boolean
            Get
                Return m_AriaInf.bIsCenter
            End Get
            Set(ByVal value As Boolean)
                m_AriaInf.bIsCenter = value
            End Set
        End Property
        ''' <summary>
        ''' �I�t�Z�b�g
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�I�t�Z�b�g")> _
        Public Property tProp_AriaInf_Offset() As Point
            Get
                Dim s As Point
                s.X = m_AriaInf.Offset.X
                s.Y = m_AriaInf.Offset.Y
                Return s
                'Return New Size(m_AriaInf.nAriaWidth, m_AriaInf.nAriaHeight)
            End Get
            Set(ByVal value As Point)
                m_AriaInf.Offset.X = value.X
                m_AriaInf.Offset.Y = value.Y
            End Set
        End Property
        ''' <summary>
        ''' �I�t�Z�b�g
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�I�t�Z�b�gX")> _
        Public Property tProp_AriaInf_OffsetX() As Integer
            Get
                Return m_AriaInf.Offset.X
            End Get
            Set(ByVal value As Integer)
                m_AriaInf.Offset.X = value
            End Set
        End Property
        ''' <summary>
        ''' �I�t�Z�b�g
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-�\���̈���-��p�v���p�e�B") _
         , System.ComponentModel.Description("�I�t�Z�b�gY")> _
        Public Property tProp_AriaInf_OffsetY() As Integer
            Get
                Return m_AriaInf.Offset.Y
            End Get
            Set(ByVal value As Integer)
                m_AriaInf.Offset.Y = value
            End Set
        End Property
#End Region

#Region "���W�ŏI����"
        ''' <summary>
        ''' ���W�ŏI����
        ''' </summary>
        <System.ComponentModel.Browsable(True) _
         , System.ComponentModel.Category("�����W�Z�o�N���X-���W�ŏI����-��p�v���p�e�B") _
         , System.ComponentModel.Description("���W�ŏI����")> _
        Public ReadOnly Property tProp_CalcAns() As CALC_ANS
            Get
                Return m_CalcAns
            End Get
        End Property
#End Region

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ����
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            ' �厖�Ȃ��̂���������
            m_AriaInf.Zoom.dH = 1.0
            m_AriaInf.Zoom.dW = 1.0
        End Sub


        ''' <summary>
        ''' �摜��� �Z�b�g
        ''' </summary>
        ''' <param name="nWidth">�摜 �� [pixel]</param>
        ''' <param name="nHeight">�摜 ���� [pixel]</param>
        ''' <param name="dResW">�摜 ������\ [mm/pixel]</param>
        ''' <param name="dResH">�摜 �c����\ [mm/pixel]</param>
        ''' <param name="nCenterX">���S�ʒuX [pixel] (���S�ʒu���[�h�Ŗ����ꍇ�́A0��OK)</param>
        ''' <param name="nCenterY">���S�ʒuY [pixel] (���S�ʒu���[�h�Ŗ����ꍇ�́A0��OK)</param>
        ''' <param name="nImgCntX">�摜 �\������</param>
        ''' <param name="nImgCntY">�摜 �\������</param>
        ''' <remarks></remarks>
        Public Sub SetImgInf(ByVal nWidth As Integer, ByVal nHeight As Integer, _
           ByVal dResW As Double, ByVal dResH As Double, _
           Optional ByVal nCenterX As Integer = 0, Optional ByVal nCenterY As Integer = 0, _
           Optional ByVal nImgCntX As Integer = 1, Optional ByVal nImgCntY As Integer = 1)
            ' ��{���Z�b�g
            m_ImgInf.nOneImgW = nWidth              ' 1���̉摜�̃T�C�Y
            m_ImgInf.nOneImgH = nHeight
            m_ImgInf.nImgCntX = nImgCntX
            m_ImgInf.nImgCntY = nImgCntY
            m_ImgInf.nImgW = m_ImgInf.nOneImgW * m_ImgInf.nImgCntX      ' �S�摜���l�������T�C�Y
            m_ImgInf.nImgH = m_ImgInf.nOneImgH * m_ImgInf.nImgCntY
            m_ImgInf.dResW = dResW
            m_ImgInf.dResH = dResH
            m_ImgInf.nCenterX = nCenterX
            m_ImgInf.nCenterY = nCenterY


            ' ����\�𔽉f
            m_ImgInf.dSizeH = m_ImgInf.nImgH * m_ImgInf.dResH       ' �摜���� [mm]
            m_ImgInf.dSizeW = m_ImgInf.nImgW * m_ImgInf.dResW       ' �摜�� [mm]
        End Sub

        ''' <summary>
        ''' �摜�\������ �ύX (�摜�\�������̕ύX�̂�)
        ''' </summary>
        ''' <param name="nImgCntX">�摜 �\������</param>
        ''' <param name="nImgCntY">�摜 �\������</param>
        ''' <remarks></remarks> 
        Public Sub ChangeImgCnt(Optional ByVal nImgCntX As Integer = 1, Optional ByVal nImgCntY As Integer = 1)
            m_ImgInf.nImgCntX = nImgCntX
            m_ImgInf.nImgCntY = nImgCntY
            ' �֌W�� �ύX
            m_ImgInf.nImgW = m_ImgInf.nOneImgW * m_ImgInf.nImgCntX      ' �S�摜���l�������T�C�Y
            m_ImgInf.nImgH = m_ImgInf.nOneImgH * m_ImgInf.nImgCntY

            m_ImgInf.dSizeH = m_ImgInf.nImgH * m_ImgInf.dResH       ' �摜���� [mm]
            m_ImgInf.dSizeW = m_ImgInf.nImgW * m_ImgInf.dResW       ' �摜�� [mm]
        End Sub

        ''' <summary>
        ''' ���W�f�[�^�v�Z (���C������)
        ''' </summary>
        ''' <remarks></remarks>
        Public Overridable Sub Calculation()

            '// =========================================
            '// �摜�T�C�Y�֌W
            '//

            '' �ŏI�\���{������
            '' ����\�𔽉f   ���׌y���̈׃���
            'm_ImgInf.dSizeW = m_ImgInf.nImgW * m_ImgInf.dResW		' �摜�� [mm]
            'm_ImgInf.dSizeH = m_ImgInf.nImgH * m_ImgInf.dResH		' �摜���� [mm]

            ' �\���`���𔽉f
            Call SetStrechRate()
            ' �ŏI�{���𔽉f
            m_CalcAns.RateRes.dW = m_AriaInf.Zoom.dW * m_ImgKind.StrechRate.dW
            m_CalcAns.RateRes.dH = m_AriaInf.Zoom.dH * m_ImgKind.StrechRate.dH
            ' �\���{���𔽉f
            m_CalcAns.RateDsp.dW = m_CalcAns.RateRes.dW * m_ImgInf.dResW
            m_CalcAns.RateDsp.dH = m_CalcAns.RateRes.dH * m_ImgInf.dResH
            ' �ŏI����\
            'm_CalcAns.FinalRes.dW = m_AriaInf.Zoom.dW * m_CalcAns.FinalRes.dW
            'm_CalcAns.FinalRes.dH = m_AriaInf.Zoom.dH * m_CalcAns.FinalRes.dH
            m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dW / m_AriaInf.Zoom.dW   ' Ver 1.2.0.0 �ŏC��
            m_CalcAns.FinalRes.dH = m_CalcAns.FinalRes.dH / m_AriaInf.Zoom.dH

            '' �{�����f��̉摜�T�C�Y
            m_CalcAns.ImgDspRect.X = 0              ' �\���ʒu�͌�Ōv�Z����ׁA�Ƃ肠����0�N���A
            m_CalcAns.ImgDspRect.Y = 0
            m_CalcAns.ImgDspRect.Width = CInt(m_ImgInf.nImgW * m_CalcAns.RateDsp.dW)
            m_CalcAns.ImgDspRect.Height = CInt(m_ImgInf.nImgH * m_CalcAns.RateDsp.dH)


            '' �\���G���A�ɕ\������Ă��Ȃ������̉摜�T�C�Y
            '    �X�N���[���o�[��MaxVal�Ƃ��Ɏg�p�ł���
            m_CalcAns.ImgNonAriaSize.Width = m_CalcAns.ImgDspRect.Width - m_AriaInf.nAriaWidth
            If 0 > m_CalcAns.ImgNonAriaSize.Width Then m_CalcAns.ImgNonAriaSize.Width = 0

            m_CalcAns.ImgNonAriaSize.Height = m_CalcAns.ImgDspRect.Height - m_AriaInf.nAriaHeight
            If 0 > m_CalcAns.ImgNonAriaSize.Height Then m_CalcAns.ImgNonAriaSize.Height = 0


            '// =========================================
            '// �摜�\���ʒu�֌W
            '//

            '' ���S�ʒu��\���G���A�̒��S�Ɉړ�
            If m_AriaInf.bIsCenter Then
                ' ���S�ʒu�� �\���G���A�̒����ɗ���悤�ɍ��W���v�Z
                SetCenterPosition()

                ' �I�t�Z�b�g�ʒu �����v�Z
                '   �X�N���[���o�[��Value�Ƃ��Ɏg�p�ł���
                m_AriaInf.Offset.X = m_CalcAns.ImgDspRect.X
                'If 0 > m_AriaInf.Offset.X Then m_AriaInf.Offset.X = 0
                m_AriaInf.Offset.Y = m_CalcAns.ImgDspRect.Y
                'If 0 > m_AriaInf.Offset.Y Then m_AriaInf.Offset.Y = 0

            Else
                ' �I�t�Z�b�g�ʂ��� �摜�ʒu�����炷
                SetOffsetPosition()

            End If

        End Sub

        ''' <summary>
        ''' ���S�ʒu�� �\���G���A�̒����ɗ���悤�ɍ��W���v�Z
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub SetCenterPosition()
            Dim nCenterX As Integer     ' ���S�ʒuX
            Dim nCenterY As Integer     ' ���S�ʒuY
            Dim Pos As Point            ' ����̈ʒu

            '' ���E���]
            If m_ImgKind.bIsTurnLR Then
                nCenterX = CInt((m_ImgInf.nImgW - m_ImgInf.nCenterX) * m_CalcAns.RateDsp.dW)
            Else
                nCenterX = CInt(m_ImgInf.nCenterX * m_CalcAns.RateDsp.dW)
            End If

            '' �㉺���]
            If m_ImgKind.bIsTurnTB Then
                nCenterY = CInt((m_ImgInf.nImgH - m_ImgInf.nCenterY) * m_CalcAns.RateDsp.dH)
            Else
                nCenterY = CInt(m_ImgInf.nCenterY * m_CalcAns.RateDsp.dH)
            End If

            '' ���S�ʒu����A�\���G���A���Z�o
            ' ��
            If m_CalcAns.ImgDspRect.Width < m_AriaInf.nAriaWidth Then                           ' �������� �\���G���A��� �摜��������
                Pos.X = CInt((m_AriaInf.nAriaWidth / 2) - (m_CalcAns.ImgDspRect.Width / 2))

            ElseIf (m_AriaInf.nAriaWidth / 2) > nCenterX Then                                   ' ���[���E
                Pos.X = 0

            ElseIf (m_AriaInf.nAriaWidth / 2) > (m_CalcAns.ImgDspRect.Width - nCenterX) Then    ' �E�[���E
                Pos.X = m_AriaInf.nAriaWidth - m_CalcAns.ImgDspRect.Width

            Else                                                                                ' ���S�ʒu���\���G���A�̒��S��
                Pos.X = CInt((m_AriaInf.nAriaWidth / 2) - nCenterX)
            End If

            ' �c
            If m_CalcAns.ImgDspRect.Height < m_AriaInf.nAriaHeight Then                         ' �������� �\���G���A��� �摜��������
                Pos.Y = CInt((m_AriaInf.nAriaHeight / 2) - (m_CalcAns.ImgDspRect.Height / 2))

            ElseIf (m_AriaInf.nAriaHeight / 2) > nCenterY Then                                  ' ���[���E
                Pos.Y = 0

            ElseIf (m_AriaInf.nAriaHeight / 2) > (m_CalcAns.ImgDspRect.Height - nCenterY) Then  ' �E�[���E
                Pos.Y = m_AriaInf.nAriaHeight - m_CalcAns.ImgDspRect.Height

            Else                                                                                ' ���S�ʒu���\���G���A�̒��S��
                Pos.Y = CInt((m_AriaInf.nAriaHeight / 2) - nCenterY)
            End If

            '' ���ʃZ�b�g
            m_CalcAns.ImgDspRect.X = Pos.X
            m_CalcAns.ImgDspRect.Y = Pos.Y
        End Sub

        ''' <summary>
        ''' �I�t�Z�b�g�ʂ��� �摜�ʒu�����炷
        ''' </summary>
        ''' <remarks></remarks>
        Protected Sub SetOffsetPosition()
            Dim Pos As Point            ' ����̈ʒu

            Pos.X = m_AriaInf.Offset.X
            Pos.Y = m_AriaInf.Offset.Y

            '' ���ʃZ�b�g
            m_CalcAns.ImgDspRect.X = Pos.X
            m_CalcAns.ImgDspRect.Y = Pos.Y
        End Sub



        ''' <summary>
        ''' �摜�\���{���Z�b�g
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub SetStrechRate()
            ' �摜�\���`���ɂ��摜�\���{����ݒ�
            Select Case m_ImgKind.emImgStrech
                Case EM_IMG_STRECH.emNormal                         ' ���{�\�� (1��f=1mm)
                    m_ImgKind.StrechRate.dH = 1.0
                    m_ImgKind.StrechRate.dW = 1.0

                    '' �ŏI����\
                    m_CalcAns.FinalRes.dH = 1.0
                    m_CalcAns.FinalRes.dW = 1.0

                Case EM_IMG_STRECH.emRawWidth
                    m_ImgKind.StrechRate.dW = (1.0 / m_ImgInf.dResW)
                    m_ImgKind.StrechRate.dH = (1.0 / m_ImgInf.dResH) * (m_ImgInf.dResH / m_ImgInf.dResW)

                    '�ŏI����\�͉摜����\���g�p(���ዾ�@�\�X�P�[���\���ɉe��)
                    'm_CalcAns.FinalRes.dH = 1.0
                    'm_CalcAns.FinalRes.dW = 1.0
                    m_CalcAns.FinalRes.dH = m_ImgInf.dResW
                    m_CalcAns.FinalRes.dW = m_ImgInf.dResW

                Case EM_IMG_STRECH.emRawHeight
                    m_ImgKind.StrechRate.dW = (1.0 / m_ImgInf.dResW) * (m_ImgInf.dResW / m_ImgInf.dResH)
                    m_ImgKind.StrechRate.dH = (1.0 / m_ImgInf.dResH)

                    '�ŏI����\�͉摜����\���g�p(���ዾ�@�\�X�P�[���\���ɉe��)
                    'm_CalcAns.FinalRes.dH = 1.0
                    'm_CalcAns.FinalRes.dW = 1.0
                    m_CalcAns.FinalRes.dH = m_ImgInf.dResH
                    m_CalcAns.FinalRes.dW = m_ImgInf.dResH

                Case EM_IMG_STRECH.emRaw    ' ���摜

                    m_ImgKind.StrechRate.dW = (1.0 / m_ImgInf.dResW)
                    m_ImgKind.StrechRate.dH = (1.0 / m_ImgInf.dResH)

                    '�ŏI����\�͉摜����\���g�p(���ዾ�@�\�X�P�[���\���ɉe��)
                    'm_CalcAns.FinalRes.dH = 1.0
                    'm_CalcAns.FinalRes.dW = 1.0
                    m_CalcAns.FinalRes.dH = m_ImgInf.dResH
                    m_CalcAns.FinalRes.dW = m_ImgInf.dResW




                Case EM_IMG_STRECH.emStrechBoth                     ' �\���G���A�ɉ摜�����킹��
                    m_ImgKind.StrechRate.dH = m_AriaInf.nAriaHeight / m_ImgInf.dSizeH
                    m_ImgKind.StrechRate.dW = m_AriaInf.nAriaWidth / m_ImgInf.dSizeW

                    '' �ŏI����\
                    m_CalcAns.FinalRes.dH = m_ImgInf.dSizeH / m_AriaInf.nAriaHeight
                    m_CalcAns.FinalRes.dW = m_ImgInf.dSizeW / m_AriaInf.nAriaWidth


                Case EM_IMG_STRECH.emStrechHeight                   ' �\���G���A�̍����ɉ摜���������킹��
                    m_ImgKind.StrechRate.dH = m_AriaInf.nAriaHeight / m_ImgInf.dSizeH
                    m_ImgKind.StrechRate.dW = m_ImgKind.StrechRate.dH

                    '' �ŏI����\
                    m_CalcAns.FinalRes.dH = m_ImgInf.dSizeH / m_AriaInf.nAriaHeight
                    m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dH


                Case EM_IMG_STRECH.emStrechWidth                    ' �\���G���A�̕��ɉ摜�������킹��

                    m_ImgKind.StrechRate.dH = m_AriaInf.nAriaWidth / m_ImgInf.dSizeW
                    m_ImgKind.StrechRate.dW = m_ImgKind.StrechRate.dH

                    '' �ŏI����\
                    m_CalcAns.FinalRes.dH = m_ImgInf.dSizeW / m_AriaInf.nAriaWidth
                    m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dH

                Case EM_IMG_STRECH.emJustBig                        ' �摜�T�C�Y�̑傫�������\���̈�Ɉ�v����悤�ɍ��킹��
                    m_ImgKind.StrechRate.dH = CDbl(IIf(m_ImgInf.dSizeH > m_ImgInf.dSizeW, m_AriaInf.nAriaHeight / m_ImgInf.dSizeH, m_AriaInf.nAriaWidth / m_ImgInf.dSizeW))
                    m_ImgKind.StrechRate.dW = m_ImgKind.StrechRate.dH

                    '' �ŏI����\
                    m_CalcAns.FinalRes.dH = CDbl(IIf(m_ImgInf.dSizeH > m_ImgInf.dSizeW, m_ImgInf.dSizeH / m_AriaInf.nAriaHeight, m_ImgInf.dSizeW / m_AriaInf.nAriaWidth))
                    m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dH

                Case EM_IMG_STRECH.emJustSmall                      ' �摜�T�C�Y�̏����������\���̈�Ɉ�v����悤�ɍ��킹��
                    m_ImgKind.StrechRate.dH = CDbl(IIf(m_ImgInf.dSizeH < m_ImgInf.dSizeW, m_AriaInf.nAriaHeight / m_ImgInf.dSizeH, m_AriaInf.nAriaWidth / m_ImgInf.dSizeW))
                    m_ImgKind.StrechRate.dW = m_ImgKind.StrechRate.dH

                    '' �ŏI����\
                    m_CalcAns.FinalRes.dH = CDbl(IIf(m_ImgInf.dSizeH < m_ImgInf.dSizeW, m_ImgInf.dSizeH / m_AriaInf.nAriaHeight, m_ImgInf.dSizeW / m_AriaInf.nAriaWidth))
                    m_CalcAns.FinalRes.dW = m_CalcAns.FinalRes.dH



            End Select
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ���W�v�Z
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' ���̉摜���W����\���p�̍��W�ɕϊ� (���]�E�I�t�Z�b�g�����l��) [��pixel]��[�\��pixel]
        ''' </summary>
        ''' <param name="x">���摜X���W</param>
        ''' <param name="y">���摜Y���W</param>
        ''' <returns>�\�����W</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetPosition(ByVal x As Integer, ByVal y As Integer) As Point

            '' ���E���]
            'If m_ImgKind.bIsTurnLR Then x = m_ImgInf.nImgW - x

            '' �㉺���]
            'If m_ImgKind.bIsTurnTB Then y = m_ImgInf.nImgH - y
            ' ���E���]
            If m_ImgKind.bIsTurnLR Then x = m_ImgInf.nImgW - x - 1

            ' �㉺���]
            If m_ImgKind.bIsTurnTB Then y = m_ImgInf.nImgH - y - 1



            ' �\���p���W�ɕϊ� 
            x = CInt(x * m_CalcAns.RateDsp.dW)
            y = CInt(y * m_CalcAns.RateDsp.dH)

            If m_ImgKind.bIsRotate90 Then
                '��]���̍l�������F
                '�@�P�D�\�����W�n�̌��_�́A���摜���炷��ƉE���ɂ���
                x = m_CalcAns.ImgDspRect.Width - x
            End If

            '' �I�t�Z�b�g�l��
            x += m_AriaInf.Offset.X
            y += m_AriaInf.Offset.Y
            Return New Point(x, y)
        End Function
        Public Overloads Function GetPosition(ByVal pos As Point) As Point
            Return GetPosition(pos.X, pos.Y)
        End Function


        ''' <summary>
        ''' ���̉摜���W����\���p�̍��W�ɕϊ� [��pixel]��[�\��pixel]
        ''' </summary>
        ''' <param name="x"></param>
        ''' <param name="y"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function GetSimplePoint_ImgToDsp(ByVal x As Integer, ByVal y As Integer) As Point
            ' �\���p���W�ɕϊ� �{ �I�t�Z�b�g�l��
            x = CInt(x * m_CalcAns.RateDsp.dW)
            y = CInt(y * m_CalcAns.RateDsp.dH)

            Return New Point(x, y)
        End Function
        Public Overloads Function GetSimplePoint_ImgToDsp(ByVal pos As Point) As Point
            Return GetSimplePoint_ImgToDsp(pos.X, pos.Y)
        End Function
        Public Overloads Function GetPos_RpToDp_X(ByVal x As Integer) As Integer
            Return CInt(x * m_CalcAns.RateDsp.dW)
        End Function
        Public Overloads Function GetPos_RpToDp_Y(ByVal y As Integer) As Integer
            Return CInt(y * m_CalcAns.RateDsp.dH)
        End Function


        ''' <summary>
        ''' �\���p�̍��W���猳�̉摜���W�ɕϊ� (���]�E�I�t�Z�b�g�����l��) [�\��pixel]��[��pixel]
        ''' </summary>
        ''' <param name="dx">�\�����WX���W</param>
        ''' <param name="dy">�\�����WY���W</param>
        ''' <returns>���̉摜���W</returns>
        ''' <remarks></remarks>
        Public Overloads Function GetDspPixelToImgPosition(ByVal dx As Integer, ByVal dy As Integer) As Point
            ' �I�t�Z�b�g���l��
            Dim x As Integer = dx - m_AriaInf.Offset.X
            Dim y As Integer = dy - m_AriaInf.Offset.Y
            If m_ImgKind.bIsRotate90 Then
                '��]���̍l�������F
                '�@�P�D�\�����W�n�̌��_�́A���摜���炷��ƉE���ɂ���
                '�@�Q�D�\�����W�n�́A��]���Ă���̂ŁA X:����AY�F��
                '�@�R�Dm_AriaInf��Offset(�X�N���[����)�́AX:���@�AY�F����
                x = m_CalcAns.ImgDspRect.Width - (dy - m_AriaInf.Offset.X) - 1
                y = dx - m_AriaInf.Offset.Y
            End If

            ' �\���p���W ���� ���摜���W �ɕϊ�
            x = CInt(x / m_CalcAns.RateDsp.dW)
            y = CInt(y / m_CalcAns.RateDsp.dH)

            '' ���E���]
            'If m_ImgKind.bIsTurnLR Then x = m_ImgInf.nImgW - x
            '' �㉺���]
            'If m_ImgKind.bIsTurnTB Then y = m_ImgInf.nImgH - y
            ' ���E���]
            If m_ImgKind.bIsTurnLR Then x = m_ImgInf.nImgW - x - 1
            ' �㉺���]
            If m_ImgKind.bIsTurnTB Then y = m_ImgInf.nImgH - y - 1


            Return New Point(x, y)

        End Function
        Public Overloads Function GetDspPixelToImgPosition(ByVal pos As Point) As Point
            Return GetDspPixelToImgPosition(pos.X, pos.Y)
        End Function

        ''' <summary>
        ''' �\���p�̍��W���猳�̉摜���W�ɕϊ� [�\��pixel]��[��pixel]
        ''' </summary>
        ''' <param name="x"></param>
        ''' <param name="y"></param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Overloads Function GetSimplePoint_DspToImg(ByVal x As Integer, ByVal y As Integer) As Point
            ' �\���p���W ���� ���摜���W �ɕϊ�
            x = CInt(x / m_CalcAns.RateDsp.dW)
            y = CInt(y / m_CalcAns.RateDsp.dH)

            Return New Point(x, y)
        End Function
        Public Overloads Function GetSimplePoint_DspToImg(ByVal pos As Point) As Point
            Return GetSimplePoint_DspToImg(pos.X, pos.Y)
        End Function
        Public Overloads Function GetPos_DpToRp_X(ByVal x As Integer) As Integer
            Return CInt(x / m_CalcAns.RateDsp.dW)
        End Function
        Public Overloads Function GetPos_DpToRp_Y(ByVal y As Integer) As Integer
            Return CInt(y / m_CalcAns.RateDsp.dH)
        End Function



        ''' <summary>
        ''' �ŏI�I�ȌX�̉摜�T�C�Y (�S���ꏏ�Ƃ͌���Ȃ�)
        ''' </summary>
        ''' <param name="RecX">�Z�o�������摜��X�\���ʒu (0�I���W��)</param>
        ''' <param name="RecY">�Z�o�������摜��Y�\���ʒu (0�I���W��)</param>
        ''' <remarks>GDI+�̏k���́A�s��̂������Ȃǂɂ���āA�Ƃ��ǂ�-1�����̂ŁA
        ''' ���E�����͂�������z���ēK���ɂP�`�Qpx�傫���Ȃ��Ă�</remarks>
        Public Function GetAnsOneImgSize(ByVal RecX As Integer, ByVal RecY As Integer) As Rectangle
            Dim ans As Rectangle                        ' �ŏI����
            Dim posNowX, posNowY As Double             ' ����̊J�n�ʒu
            Dim posNextX, posNextY As Double ' ����̊J�n�ʒu


            '' ����Ǝ���̕\���ʒu ���Z�o
            ' �c����
            If True AndAlso m_ImgKind.bIsTurnTB Then
                posNowY = m_ImgInf.nImgH - (RecY * m_ImgInf.nOneImgH)
                posNextY = m_ImgInf.nImgH - ((RecY + 1) * m_ImgInf.nOneImgH)
            Else
                posNowY = RecY * m_ImgInf.nOneImgH
                posNextY = (RecY + 1) * m_ImgInf.nOneImgH
            End If
            posNowY = (posNowY * m_CalcAns.RateDsp.dH + m_CalcAns.ImgDspRect.Y)
            posNextY = (posNextY * m_CalcAns.RateDsp.dH + m_CalcAns.ImgDspRect.Y)


            ' �\���ʒu
            If True AndAlso m_ImgKind.bIsTurnLR Then
                posNowX = m_ImgInf.nImgW - (RecX * m_ImgInf.nOneImgW)
                posNextX = m_ImgInf.nImgW - ((RecX + 1) * m_ImgInf.nOneImgW)
            Else
                posNowX = RecX * m_ImgInf.nOneImgW
                posNextX = (RecX + 1) * m_ImgInf.nOneImgW
            End If
            posNowX = (posNowX * m_CalcAns.RateDsp.dW + m_CalcAns.ImgDspRect.X)
            posNextX = (posNextX * m_CalcAns.RateDsp.dW + m_CalcAns.ImgDspRect.X)

            '' ����f�[�^�Z�b�g
            ans.X = CInt(Math.Floor(Math.Min(posNowX, posNextX)))
            ans.Y = CInt(Math.Floor(Math.Min(posNowY, posNextY)))

            '' ������
            ans.Width = CInt(Math.Floor(Math.Abs(posNextX - posNowX))) + 1 + CInt(IIf(m_ImgKind.bIsTurnLR, 1, 0))
            ans.Height = CInt(Math.Floor(Math.Abs(posNextY - posNowY))) + 1 + CInt(IIf(m_ImgKind.bIsTurnTB, 1, 0))

            'If False Then
            '    ans.X = posNowX
            '    ans.Y = posNowY

            '    '' ������
            '    ans.Width = (posNextX - posNowX) + CInt(IIf(m_ImgKind.bIsTurnLR, -1, 0))
            '    ans.Height = (posNextY - posNowY) + CInt(IIf(m_ImgKind.bIsTurnTB, -1, 0))
            'End If

            Return ans
        End Function




        ''' <summary>
        ''' [�\��pixel]��[mm]
        ''' </summary>
        Public Overloads Function GetPos_DpToLen_Y(ByVal v As Integer) As Single
            Return CSng(v * m_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_DpToLen_X(ByVal v As Integer) As Single
            Return CSng(v * m_CalcAns.FinalRes.dW)
        End Function


        ''' <summary>
        ''' [mm]��[�\��pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToDp_Y(ByVal v As Double) As Integer
            Return CInt(v / m_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_Y(ByVal v As Integer) As Integer
            Return CInt(v / m_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_LenToDp_X(ByVal v As Double) As Integer
            Return CInt(v / m_CalcAns.FinalRes.dW)
        End Function
        Public Overloads Function GetPos_LenToDp_X(ByVal v As Integer) As Integer
            Return CInt(v / m_CalcAns.FinalRes.dW)
        End Function

        ''' <summary>
        ''' [��pixel]��[mm]
        ''' </summary>
        Public Overloads Function GetPos_RpToLen_Y(ByVal v As Integer) As Single
            Return CSng((v * m_CalcAns.RateDsp.dH) * m_CalcAns.FinalRes.dH)
        End Function
        Public Overloads Function GetPos_RpToLen_X(ByVal v As Integer) As Single
            Return CSng(v * (m_CalcAns.RateDsp.dW * m_CalcAns.FinalRes.dW))
        End Function

        ''' <summary>
        ''' [mm]��[��pixel]
        ''' </summary>
        Public Overloads Function GetPos_LenToRp_Y(ByVal v As Double) As Integer
            Return CInt(v / (m_CalcAns.RateDsp.dH * m_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_Y(ByVal v As Integer) As Integer
            Return CInt(v / (m_CalcAns.RateDsp.dH * m_CalcAns.FinalRes.dH))
        End Function
        Public Overloads Function GetPos_LenToRp_X(ByVal v As Double) As Integer
            Return CInt(v / (m_CalcAns.RateDsp.dW * m_CalcAns.FinalRes.dW))
        End Function
        Public Overloads Function GetPos_LenToRp_X(ByVal v As Integer) As Integer
            Return CInt(v / (m_CalcAns.RateDsp.dW * m_CalcAns.FinalRes.dW))
        End Function
    End Class


    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �q�N���X
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' ���������`��w���p
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_ImageDspAxisDrawner
        Implements IDisposable

        ''' <summary>
        ''' ���������������W�ƁA���̃��x���̃y�A
        ''' </summary>
        ''' <remarks></remarks>
        Public Structure LabelInfo
            Public pos As Integer
            Public label As String
            Sub New(ByVal p As Integer, ByVal l As String)
                pos = p
                label = l
            End Sub
            Public Overrides Function ToString() As String
                Return String.Format("{0}:{1}", pos, label)
            End Function
        End Structure

        Dim g As Graphics
        Dim m_Bsh As Brush = New SolidBrush(Color.Cyan)
        Dim m_Fnt As Font = New System.Drawing.Font("�l�r �S�V�b�N", 16, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
        Dim m_Pen As Pen = New System.Drawing.Pen(Color.Cyan, 2)

        Public Sub Dispose() Implements IDisposable.Dispose
            m_Bsh.Dispose()
            m_Bsh = Nothing
            m_Fnt.Dispose()
            m_Fnt = Nothing
            m_Pen.Dispose()
            m_Pen = Nothing
        End Sub

        ''' <summary>
        ''' �`��Ɏg�p����t�H���g�E�u���V�E�y����ύX����
        ''' </summary>
        ''' <param name="bsh">�t�H���g</param>
        ''' <param name="fnt">�u���V</param>
        ''' <param name="pen">�y��</param>
        ''' <remarks>�O�Ɏg�����Ă����̂�Dispose�����</remarks>
        Sub UpdateMemoriPenBrushFont(ByVal bsh As Brush, ByVal fnt As Font, ByVal pen As Pen)
            Dispose()

            m_Bsh = bsh
            m_Fnt = fnt
            m_Pen = pen
        End Sub


        Dim m_ptOffset As Point     '���_
        Dim m_axisLength As Integer '����������

        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="g">�`��Ώ�</param>
        ''' <param name="axisLength">���ɐ����Ȓ���</param>
        ''' <param name="ptOffset">���_</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal g As Graphics, ByVal axisLength As Integer, ByVal ptOffset As Point)
            Me.g = g
            Me.m_ptOffset = ptOffset
            Me.m_axisLength = axisLength
        End Sub

        ''' <summary>
        ''' ������̕`��T�C�Y�����߂�
        ''' </summary>
        ''' <param name="s">������</param>
        ''' <returns>�O���t�B�b�N��̃T�C�Y</returns>
        ''' <remarks></remarks>
        Function MeasureString(ByVal s As String) As SizeF
            Return g.MeasureString(s, m_Fnt, 0, Nothing)
        End Function


        '���x���̕����͕⏕���̒��S�ɗ���ׂ���
        Public IsCenterV As Boolean = True

        'Y����LabelInfo�̐��l���A���]���邩
        Public IsFlipV As Boolean = False

        '�ڐ����x�������C���̊O���ɂ����B
        Public IsStringOutside As Boolean = False

        ''' <summary>
        ''' Y�����������̘A����`�悷��
        ''' </summary>
        ''' <param name="labels">���x���Q</param>
        ''' <param name="basepos">�c���̈ʒu�iX�j</param>
        ''' <remarks></remarks>
        Public Sub draw_y_axis(ByVal labels As IEnumerable(Of LabelInfo), ByVal basepos As Integer)
            For Each label As LabelInfo In labels
                Dim yy As Integer = m_ptOffset.Y + label.pos
                If IsFlipV Then yy = m_ptOffset.Y - label.pos

                '�ڐ�����
                Dim axisLength As Integer = m_axisLength
                If Not String.IsNullOrEmpty(label.label) Then
                    axisLength = axisLength * 3 \ 2
                End If

                '�`�敶����̃T�C�Y
                Dim box As SizeF = MeasureString(label.label)

                If basepos > 100 Then
                    '�E����Y���Ȃ̂ŁA�ڐ����̓x�[�X���獶�ɓ˂��o��
                    g.DrawLine(m_Pen, basepos - axisLength, yy, basepos, yy)

                    '�����T�C�Y���͍��ɗ���ׂ�
                    Dim xx As Integer = CInt(basepos - box.Width)
                    If IsCenterV Then
                        '�Z���^�����O���́A�ڐ����Ƃ��Ԃ�̂�X�𗣂�
                        xx = xx - m_axisLength - 10
                        'Y�͒��S��
                        yy = yy - CInt(box.Height / 2)
                    End If

                    If IsStringOutside Then
                        '�O���i�x�[�X����E�j�ɕ������o���ꍇ
                        xx = basepos
                    End If

                    g.DrawString(label.label, m_Fnt, m_Bsh, xx, yy + 2)
                Else
                    '������Y���Ȃ̂ŁA�ڐ����̓x�[�X����E�֏o��
                    Dim xx As Integer = basepos
                    g.DrawLine(m_Pen, basepos, yy, basepos + axisLength, yy)
                    If IsCenterV Then
                        '�Z���^�����O���́A�ڐ����Ƃ��Ԃ�̂�X�𗣂�
                        xx = basepos + 10 + axisLength
                        yy = yy - CInt(box.Height / 2)
                    End If
                    If IsStringOutside Then
                        '�O���i�x�[�X���獶�j�ɕ������o��
                        xx = basepos - CInt(box.Width)
                    End If

                    g.DrawString(label.label, m_Fnt, m_Bsh, xx, yy + 2)
                End If
            Next
        End Sub

        ''' <summary>
        ''' Y���̎�����`�悷��B���_�`labels�̍ő�܂Œ����Ō���
        ''' </summary>
        ''' <param name="labels"></param>
        ''' <param name="basepos"></param>
        ''' <remarks></remarks>
        Public Sub draw_y_line(ByVal labels As IEnumerable(Of LabelInfo), ByVal basepos As Integer)
            Dim max_pos As Integer = 0
            For Each label As LabelInfo In labels
                max_pos = Math.Max(label.pos, max_pos)
            Next

            Dim yy As Integer = m_ptOffset.Y + max_pos
            If IsFlipV Then yy = m_ptOffset.Y - max_pos
            g.DrawLine(m_Pen, basepos, m_ptOffset.Y, basepos, yy)

        End Sub


        ''' <summary>
        ''' X�����������̘A����`�悷��
        ''' </summary>
        ''' <param name="labels">���x���Q</param>
        ''' <param name="basepos">�����̈ʒu�iY�j</param>
        ''' <remarks></remarks>
        Public Sub draw_x_axis(ByVal labels As IEnumerable(Of LabelInfo), ByVal basepos As Integer)
            For Each label As LabelInfo In labels
                Dim xx As Integer = m_ptOffset.X + label.pos
                Dim axisLength As Integer = m_axisLength
                If Not String.IsNullOrEmpty(label.label) Then
                    axisLength = axisLength * 3 \ 2
                End If

                Dim box As SizeF = MeasureString(label.label)
                Dim lx As Integer = xx
                If IsCenterV Then
                    '�Z���^�����O
                    lx = xx - CInt(box.Width / 2)
                End If

                If basepos > 100 Then
                    '���ɂ�X���B�ڐ����̓x�[�X������
                    g.DrawLine(m_Pen, xx, basepos, xx, basepos - axisLength)

                    '�ڐ����̂���ɏ�ɕ������z�u
                    Dim yy As Integer = CInt(basepos - m_axisLength - box.Height)
                    If IsStringOutside Then
                        '�O���i�x�[�X�̉��j�ɔz�u���Ȃ���
                        yy = basepos + 1
                    End If

                    g.DrawString((label.label), m_Fnt, m_Bsh, lx, yy)
                Else
                    '��ɂ�X���B�ڐ����̓x�[�X���牺��
                    g.DrawLine(m_Pen, xx, basepos, xx, basepos + axisLength)


                    '�f�t�H���g�͖ڐ����̂���ɉ�
                    Dim yy As Integer = CInt(basepos + m_axisLength)
                    If IsStringOutside Then
                        '�O���i�x�[�X�̉��j�ɔz�u���Ȃ���
                        yy = basepos + 1
                    End If
                    g.DrawString((label.label), m_Fnt, m_Bsh, lx, yy)
                End If
            Next
        End Sub

        ''' <summary>
        ''' Y���̎�����`�悷��B���_�`labels�̍ő�܂Œ����Ō���
        ''' </summary>
        ''' <param name="labels"></param>
        ''' <param name="basepos"></param>
        ''' <remarks></remarks>
        Public Sub draw_x_line(ByVal labels As IEnumerable(Of LabelInfo), ByVal basepos As Integer)
            Dim max_pos As Integer = 0
            For Each label As LabelInfo In labels
                max_pos = Math.Max(label.pos, max_pos)
            Next
            g.DrawLine(m_Pen, m_ptOffset.X, basepos, m_ptOffset.X + max_pos, basepos)
        End Sub
    End Class

End Namespace