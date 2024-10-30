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
Imports System.IO

Namespace tClass


    Public Class tClass_ImageDsp
        Inherits tClass_ImageDspBase
        Implements IDisposable	' �f�X�g���N�^��`


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �O���C�x���g
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �I�[�o�[���C���N���b�N
        ''' </summary>
        ''' <param name="sender">�I�[�o�[���C���</param>
        ''' <param name="e">�\������Ă���I�[�o�[���C�ʒu���</param>
        ''' <remarks></remarks>
        Public Event tEv_OverLay_Click(ByVal sender As OVERLAY_INF, ByVal e As Rectangle)

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �萔��`
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>�I�[�o�[���C �ŏI����</summary>
        Protected Structure OVERLAY_ANS
            Dim ovlInf As OVERLAY_INF				' ��{���

            Dim ansRect As Rectangle				' �ŏI���ʍ��W
        End Structure

        ''' <summary>�I�[�o�[���C���</summary>
        Public Structure OVERLAY_INF
            ''' <summary>�\���L�� (true:�L, false:��)</summary>
            Dim bIsDsp As Boolean

            ''' <summary>���ݒ�</summary>
            Dim pen As Pen
            ''' <summary>�\���F (Pen���w�莞�B�ȈՕ\����)</summary>
            Dim Color As Color
            ''' <summary>���T�C�Y (Pen���w�莞�B�ȈՕ\����)</summary>
            Dim LineWidth As Integer


            ''' <summary>�I�[�o�[���C���</summary>
            Dim ovlPos1 As Point
            Dim ovlPos2 As Point

            ''' <summary>���̑��g�����</summary>
            Dim Tag As Object

            ''' <summary>���̕␳�� [DP]</summary>
            Dim HoseiX As Integer
            ''' <summary>�c�̕␳�� [DP]</summary>
            Dim HoseiY As Integer


            ' �����\�� =======================================
            ''' <summary>�r��\���L�� (true:�L, false:��)</summary>
            Dim bIsDspType As Boolean
            Dim cBsh As SolidBrush
            Dim cFnt As Font
            Dim sTypeName As String		' �r��

        End Structure


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �����o�[�ϐ�
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>�I�[�o�[���C���</summary>
        Protected m_OverLayInf As New List(Of OVERLAY_ANS)

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �v���p�e�B
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �I�[�o�[���C�̃Z�b�g��
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property tProp_OverLayCnt() As Integer
            Get
                Return m_OverLayInf.Count
            End Get
        End Property
        ''' <summary>
        ''' �I�[�o�[���C�̏��
        ''' </summary>
        ''' <param name="nIndex"></param>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public ReadOnly Property tProp_OverLayInf(ByVal nIndex As Integer) As OVERLAY_INF
            Get
                Return m_OverLayInf(nIndex).ovlInf
            End Get
        End Property

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ ����
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()

        End Sub


        ''' <summary>
        ''' Dispose�f�X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Shadows Sub Dispose()
            m_OverLayInf.Clear()
            MyBase.Dispose()
        End Sub

        ''' <summary>
        ''' �N���b�N�ɂ�� �I�[�o�[���C �`�F�b�N�����L��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub AddHandler_OverLayClick()
            AddHandler m_cntPic.Click, AddressOf PictureBox_Click
        End Sub
        ''' <summary>
        ''' �N���b�N�ɂ�� �I�[�o�[���C �`�F�b�N��������
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub RemoveHandler_OverLayClick()
            RemoveHandler m_cntPic.Click, AddressOf PictureBox_Click
        End Sub

        ''' <summary>
        ''' �I�[�o�[���C���o�^
        ''' </summary>
        ''' <param name="ovl">�I�[�o�[���C���</param>
        ''' <remarks></remarks>
        Public Sub SetOverLayInf(ByVal ovl As OVERLAY_INF)
            ' ���o�^
            Dim ans As OVERLAY_ANS
            ans.ovlInf = ovl

            m_OverLayInf.Add(ans)
        End Sub
        ''' <summary>
        ''' �I�[�o�[���C���J��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ResetOverLayInf()
            ' ���J��
            m_OverLayInf.Clear()
        End Sub

        ''' <summary>
        ''' �I�[�o�[���C�\����\���ݒ�
        ''' </summary>
        ''' <param name="dsp">�I�[�o�[���C�\����\���ݒ�</param>
        ''' <remarks></remarks>
        Public Sub OverLayDspChange(ByVal dsp As Boolean)
            Dim ovl As OVERLAY_ANS
            For ii As Integer = 0 To m_OverLayInf.Count - 1
                ovl = m_OverLayInf(ii)
                ovl.ovlInf.bIsDsp = dsp
                m_OverLayInf(ii) = ovl
            Next
            ' �ĕ`��
            Me.Refresh()
        End Sub


        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �I�[�o�[���C�I��
        '/ /////////////////////////////////////////////////////////////////////////////////
        ''' <summary>
        ''' �I�[�o�[���C�N���b�N����
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub PictureBox_Click(ByVal sender As System.Object, ByVal e As System.EventArgs)
            ''Dim pos As Point = m_cntPic.PointToClient(Windows.Forms.Cursor.Position)		 ' �N���b�N�ʒu (��ʍ��W����Pic�R���g���[���̍��ォ��̍��W�ɕϊ�)

            ''For Each ans As OVERLAY_ANS In m_OverLayInf
            ''	' ���������\�����̃I�[�o�[���C��
            ''	If Not ans.ovlInf.bIsDsp Then Continue For
            ''	' �͈͓����`�F�b�N
            ''	If ans.ansRect.X > pos.X Then Continue For
            ''	If ans.ansRect.Y > pos.Y Then Continue For
            ''	If ans.ansRect.X + ans.ansRect.Width < pos.X Then Continue For
            ''	If ans.ansRect.Y + ans.ansRect.Height < pos.Y Then Continue For

            ''	' �����܂ł����� �C�x���g����
            ''	RaiseEvent tEv_OverLay_Click(ans.ovlInf, ans.ansRect)
            ''	Exit Sub
            ''Next

            Dim pos As Point = m_cntPic.PointToClient(Windows.Forms.Cursor.Position)		 ' �N���b�N�ʒu (��ʍ��W����Pic�R���g���[���̍��ォ��̍��W�ɕϊ�)

            Dim min As OVERLAY_ANS = Nothing		  ' ��ԏ���������
            Dim bSet As Boolean = False

            For Each ans As OVERLAY_ANS In m_OverLayInf
                ' ���������\�����̃I�[�o�[���C��
                If Not ans.ovlInf.bIsDsp Then Continue For
                ' �͈͓����`�F�b�N
                If ans.ansRect.X > pos.X Then Continue For
                If ans.ansRect.Y > pos.Y Then Continue For
                If ans.ansRect.X + ans.ansRect.Width < pos.X Then Continue For
                If ans.ansRect.Y + ans.ansRect.Height < pos.Y Then Continue For

                ' ��ԏ��������̂�I��
                If Not bSet OrElse min.ansRect.Width * min.ansRect.Height > ans.ansRect.Width * ans.ansRect.Height Then
                    min = ans
                    bSet = True
                End If
            Next

            If bSet Then
                ' �����܂ł����� �C�x���g����
                RaiseEvent tEv_OverLay_Click(min.ovlInf, min.ansRect)
            End If
        End Sub

        '/ /////////////////////////////////////////////////////////////////////////////////
        '/ �C���[�W�I�u�W�F�N�g �֌W
        '/ /////////////////////////////////////////////////////////////////////////////////

        ''' <summary>
        ''' �ŏI�`�� (�p��)
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overrides Sub Paint_End(ByVal e As System.Windows.Forms.PaintEventArgs)
            ' �I�[�o�[���C�`��
            Paint_OverLay(e)
        End Sub


        ''' <summary>
        ''' �I�[�o�[���C�`��
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overridable Sub Paint_OverLay(ByVal e As System.Windows.Forms.PaintEventArgs)
            Dim rct As Rectangle = Nothing
            Dim ovl As OVERLAY_INF
            Dim wkPos1 As Point
            Dim wkPos2 As Point
            Dim wkVal As Integer

            '''' �S�� �\��
            For ii As Integer = 0 To m_OverLayInf.Count - 1
                ovl = m_OverLayInf(ii).ovlInf
                If Not ovl.bIsDsp Then Continue For

                ' �\�����W���v�Z
                wkPos1 = GetPosition(ovl.ovlPos1)
                wkPos2 = GetPosition(ovl.ovlPos2)

                ' �召�`�F�b�N
                If wkPos1.X > wkPos2.X Then
                    wkVal = wkPos1.X
                    wkPos1.X = wkPos2.X
                    wkPos2.X = wkVal
                End If
                If wkPos1.Y > wkPos2.Y Then
                    wkVal = wkPos1.Y
                    wkPos1.Y = wkPos2.Y
                    wkPos2.Y = wkVal
                End If

                ' �I�t�Z�b�g�l���f
                wkPos1.X -= ovl.HoseiX
                wkPos2.X += ovl.HoseiX
                wkPos1.Y -= ovl.HoseiY
                wkPos2.Y += ovl.HoseiY

                ' �S���\���G���A�O�H
                Dim bDspOk As Boolean = True
                If (wkPos1.X < 0 AndAlso wkPos2.X < 0) OrElse _
                   (wkPos1.X > m_AriaInf.nAriaWidth And wkPos2.X > m_AriaInf.nAriaWidth) OrElse _
                  (wkPos1.Y < 0 AndAlso wkPos2.Y < 0) OrElse _
                   (wkPos1.Y > m_AriaInf.nAriaHeight And wkPos2.Y > m_AriaInf.nAriaHeight) Then
                    bDspOk = False
                End If


                ' �\���G���A�����`�F�b�N
                If wkPos1.X < 0 Then wkPos1.X = 0
                If wkPos1.X > m_AriaInf.nAriaWidth Then wkPos1.X = m_AriaInf.nAriaWidth
                If wkPos2.X < 0 Then wkPos2.X = 0
                If wkPos2.X > m_AriaInf.nAriaWidth Then wkPos2.X = m_AriaInf.nAriaWidth

                If wkPos1.Y < 0 Then wkPos1.Y = 0
                If wkPos1.Y > m_AriaInf.nAriaHeight Then wkPos1.Y = m_AriaInf.nAriaHeight
                If wkPos2.Y < 0 Then wkPos2.Y = 0
                If wkPos2.Y > m_AriaInf.nAriaHeight Then wkPos2.Y = m_AriaInf.nAriaHeight

                ' ���ʔ��f
                Dim ans As OVERLAY_ANS = m_OverLayInf(ii)

                '��]���Ă�����
                If tProp_ImgKind_Rotate90 Then
                    '90�x��]�ŁAX��Y���]�n����B�V����Y�i��X)�́A�㉺�����]���Ă���
                    ans.ansRect = New Rectangle(wkPos1.Y, wkPos1.X, wkPos2.Y - wkPos1.Y, wkPos2.X - wkPos1.X)
                Else
                    ans.ansRect = New Rectangle(wkPos1.X, wkPos1.Y, wkPos2.X - wkPos1.X, wkPos2.Y - wkPos1.Y)
                End If

                m_OverLayInf(ii) = ans
                ' �y��
                If ovl.pen Is Nothing Then
                    Using pen As New System.Drawing.Pen(ovl.Color, ovl.LineWidth)
                        e.Graphics.DrawRectangle(pen, ans.ansRect)
                    End Using
                Else
                    e.Graphics.DrawRectangle(ovl.pen, ans.ansRect)
                End If



                ' �r�햼�\��==============================
                If ovl.bIsDspType AndAlso bDspOk Then
                    Dim sizeF As SizeF = e.Graphics.MeasureString(ovl.sTypeName, ovl.cFnt)
                    e.Graphics.DrawString(ovl.sTypeName, ovl.cFnt, ovl.cBsh, ans.ansRect.X, ans.ansRect.Y - sizeF.Height)
                End If
            Next ii
        End Sub
    End Class
End Namespace