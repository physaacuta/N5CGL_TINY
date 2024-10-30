'*********************************************************************************
'��ʂ̃n�[�h�R�s�[���s���N���X
'   [Ver]
'       Ver.01    2005/04/13  vs2005 �Ή�
'
'   [����]
'       �V���A�������s���N���X��<System.Serializable()>�����ɐݒ肷��K�v������
'*********************************************************************************
Option Strict On                'tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.Drawing
Imports System.Drawing.Printing
Imports System.Windows.Forms


Namespace tClass
    Public Class tClass_HeadCopy
        Implements IDisposable                          '�f�X�g���N�^��`

        '********************************************************************
        'API��`
        '********************************************************************
        '��ʂ̃C���[�W���擾
        Private Declare Auto Function BitBlt Lib "gdi32.dll" (ByVal hdcDest As IntPtr, ByVal nXDest As Integer, ByVal nYDest As Integer, ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal hdcSrc As IntPtr, ByVal nXSrc As Integer, ByVal nYSrc As Integer, ByVal dwRop As System.Int32) As Boolean
        Const SRCCOPY As Integer = &HCC0020
        ' 20180702 �L���v�`���[�s��Ή� ----->>>
        Const CAPTUREBLT As Integer = &H40000000
        ' 20180702 �L���v�`���[�s��Ή� <<<-----
        '��ʃE�B���h�E�̃f�o�C�X�R���e�L�X�g���擾
        Private Declare Auto Function GetWindowDC Lib "user32.dll" (ByVal hwnd As IntPtr) As IntPtr
        '�n���h���J��
        Private Declare Auto Function ReleaseDC Lib "user32.dll" (ByVal hwnd As IntPtr, ByVal hdc As IntPtr) As IntPtr


        '********************************************************************
        '�萔 ��`
        '********************************************************************
        Private mc_DefMagin_TOP As Integer = 4          '�v�����^�ň���o���Ȃ������̗]��[mm]
        Private mc_DefMagin_BOT As Integer = 10         '�v�����^�ň���o���Ȃ������̗]��[mm]
        Private mc_DefMagin_LEFT As Integer = 4         '�v�����^�ň���o���Ȃ������̗]��[mm]
        Private mc_DefMagin_RIGHT As Integer = 10       '�v�����^�ň���o���Ȃ������̗]��[mm]

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private m_PrintDoc As System.Drawing.Printing.PrintDocument     '�v�����g����R���g���[��
        Private m_Bitmap As Bitmap      '����C���[�W�r�b�g�}�b�v

        '' �I�v�V����
        Private m_PageSetup As System.Windows.Forms.PageSetupDialog     '����ݒ�_�C�����O
        Private m_PrintDialog As System.Windows.Forms.PrintDialog       '�v�����g�_�C�����O
        Private m_Preview As System.Windows.Forms.PrintPreviewDialog    '����v���r���[�_�C�����O

        '' ����v���p�e�B
        Private m_Strach As Boolean = False                             '�X�g���b�`�L�� (True:1�y�[�W�Ɏ��܂�悤�Ɋg��k������ False:�R���g���[���̃T�C�Y�̂܂�)


        '********************************************************************
        '�v���p�e�B�[��`
        '********************************************************************
#Region "�Ǝ��v���p�e�B�["

        ''' <summary>
        ''' �v�����^����
        ''' </summary>
        ''' <value>�v�����^��</value>
        Public Overloads Property prop_Printer() As String
        Get
            Return m_PrintDoc.PrinterSettings.PrinterName
        End Get
        Set(ByVal Value As String)
            m_PrintDoc.PrinterSettings.PrinterName = Value
        End Set
        End Property

        ''' <summary>
        ''' �p���T�C�Y "A4" �����w�� �������v�����^�@��ɂ��� ���ӂ��鎖
        ''' </summary>
        ''' <value>"A4"���̗p���T�C�Y</value>
        Public Overloads Property prop_PrintSize() As String
        Get
            Return m_PrintDoc.DefaultPageSettings.PaperSize.PaperName
        End Get
        Set(ByVal Value As String)
            '' �p���T�C�Y�ݒ�
            Dim PSize As System.Drawing.Printing.PaperSize          '�p���T�C�Y�ύX
            For Each PSize In m_PrintDoc.PrinterSettings.PaperSizes
                If PSize.PaperName.StartsWith(Value) Then
                    '�p���T�C�Y�ݒ�
                    m_PrintDoc.DefaultPageSettings.PaperSize = PSize
                    Exit Property
                End If
                Next
                '' �Y���Ȃ��̏ꍇ ��ԍŏ����w�肷��
                m_PrintDoc.DefaultPageSettings.PaperSize = m_PrintDoc.PrinterSettings.PaperSizes.Item(0)
            End Set
        End Property

        ''' <summary>
        ''' ������� (True:�� False:�c)
        ''' </summary>
        ''' <value>(True:��  false:�c)</value>
        Public Overloads Property prop_Landscape() As Boolean
        Get
            Return m_PrintDoc.DefaultPageSettings.Landscape
        End Get
        Set(ByVal Value As Boolean)
            m_PrintDoc.DefaultPageSettings.Landscape = Value
        End Set
        End Property

        ''' <summary>
        ''' �������
        ''' </summary>
        ''' <value>����</value>
        Public Overloads Property prop_PrintCount() As Short
        Get
            Return m_PrintDoc.PrinterSettings.Copies
        End Get
        Set(ByVal Value As Short)
            m_PrintDoc.PrinterSettings.Copies = Value
        End Set
        End Property

        ''' <summary>
        ''' �X�g���b�`�@�\ �L������ (1�y�[�W�𒴂��Ȃ��ő�T�C�Y�ň��)
        ''' </summary>
        ''' <value>(True:�L��  false:����)</value>
        Public Overloads Property prop_Strach() As Boolean
        Get
            Return m_Strach
        End Get
        Set(ByVal Value As Boolean)
            m_Strach = Value
        End Set
        End Property

        ''' <summary>
        ''' �]�� (���A�E�A��A�� �̏�) [1/100�C���`]
        ''' </summary>
        ''' <value></value>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Overloads Property prop_Margins() As Printing.Margins
        Get
            Return m_PrintDoc.DefaultPageSettings.Margins
        End Get
        Set(ByVal Value As Printing.Margins)
            m_PrintDoc.DefaultPageSettings.Margins = Value
        End Set
        End Property
#End Region

        ''' <summary>
        ''' �v�����^�̗L���𒲍�����
        ''' </summary>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function IsCheckPrinter() As Boolean
            '' �C���X�g�[������Ă���v�����^�[������H�H
            If 0 = PrinterSettings.InstalledPrinters.Count Then
                Return False
            End If
            '' �f�t�H���g�v�����^������H
            Using pd As New System.Drawing.Printing.PrintDocument
                If Not pd.DefaultPageSettings.PrinterSettings.IsDefaultPrinter Then
                    Return False
                End If

                '' �f�t�H���g�v�����^���g�p�s�\�ȏꍇ
                If False = pd.PrinterSettings.IsValid Then
                    Return False
                End If

            End Using

            '' �����܂ŗ�����g�p�\�ȃv�����^�L��
            Return True
        End Function


        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            '' �v�����^�[���� (����͂ł��Ȃ����n�[�h�R�s�[�͉\�Ȉ�)
            If Not IsCheckPrinter() Then Return

            ''�C���X�^���X�쐬
            m_PrintDoc = New System.Drawing.Printing.PrintDocument
            AddHandler m_PrintDoc.PrintPage, AddressOf PrintDoc_PrintPage   ' PrintDoc �̃C�x���g�n���h����ǉ�
            '' ����� �_�C�����O�\���֎~
            m_PrintDoc.PrintController = New System.Drawing.Printing.StandardPrintController

            '' ����v���p�e�B ������
            Me.prop_Printer = m_PrintDoc.DefaultPageSettings.PrinterSettings.PrinterName    '�f�t�H���g�v�����^�ň������  
            Me.setMargins(10, 10, 10, 10)           '�]�� (0�ɂ��Ă��A�v�����^�����Ή��Ȃ�A������Ɨ]�����󂢂ĕ\�������B�����āA���ꂽ�� �Ōオ�؂��)

            Me.prop_PrintSize = "A4"        '�p���T�C�Y�FA4
            Me.prop_Landscape = True        '��������F��
            Me.prop_PrintCount = 1          '��������F1��
        End Sub

        ''' <summary>
        ''' �f�X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub Dispose() Implements System.IDisposable.Dispose

            '' �C���X�^���X�J��
            If Not m_Bitmap Is Nothing Then m_Bitmap.Dispose()
            If Not m_Preview Is Nothing Then m_Preview.Dispose()
            If Not m_PageSetup Is Nothing Then m_PageSetup.Dispose()
            If Not m_PrintDialog Is Nothing Then m_PrintDialog.Dispose()

            If Not m_PrintDoc Is Nothing Then m_PrintDoc.Dispose()

            '' GC��Finalize���s��Ȃ��悤�ɒʒm
            GC.SuppressFinalize(Me)
        End Sub
        Protected Overrides Sub Finalize()
            Dispose()

            MyBase.Finalize()
        End Sub

        ''' <summary>
        ''' �v�����g�_�C�����O �C���X�^���X����
        ''' </summary>
        ''' <remarks>�ǂ̃v�����^�Ɉ�����邩</remarks>
        Private Sub PrintDialog_Inctance()
            '' �C���X�^���X�����ς݂��`�F�b�N
            If Not m_PrintDialog Is Nothing Then Exit Sub

            '' �C���X�^���X���� & �ݒ�
            m_PrintDialog = New System.Windows.Forms.PrintDialog
            m_PrintDialog.Document = m_PrintDoc
        End Sub

        ''' <summary>
        ''' ����ݒ�_�C�����O �C���X�^���X����
        ''' </summary>
        ''' <remarks>�ǂ̗p���ɂ��邩</remarks>
        Private Sub PageSetupDialog_Inctance()
            '' �C���X�^���X�����ς݂��`�F�b�N
            If Not m_PageSetup Is Nothing Then Exit Sub

            '' �C���X�^���X���� & �ݒ�
            m_PageSetup = New System.Windows.Forms.PageSetupDialog
            m_PageSetup.PrinterSettings = m_PrintDoc.PrinterSettings
            m_PageSetup.Document = m_PrintDoc
        End Sub

        ''' <summary>
        ''' ����v���r���[ �C���X�^���X����
        ''' </summary>
        ''' <remarks>����C���[�W</remarks>
        Private Sub PrintPreviewDialog_Inctance()
            '' �C���X�^���X�����ς݂��`�F�b�N
            If Not m_Preview Is Nothing Then Exit Sub

            '' �C���X�^���X���� & �ݒ�
            m_Preview = New System.Windows.Forms.PrintPreviewDialog
            m_Preview.Document = m_PrintDoc
        End Sub

        ''' <summary>
        ''' �v�����g�_�C�����O �\��
        ''' </summary>
        ''' <returns>True:������� False:������Ȃ�</returns>
        ''' <remarks>�ǂ̃v�����^�Ɉ�����邩</remarks>
        Public Function PrintDialog_show() As Boolean
            Dim retc As System.Windows.Forms.DialogResult
            '' �C���X�^���X����
            PrintDialog_Inctance()
            retc = m_PrintDialog.ShowDialog()
            '' �߂�l�Z�b�g
            If retc = DialogResult.OK Then Return True
            Return False
        End Function

        ''' <summary>
        ''' ����ݒ�_�C�����O �\��
        ''' </summary>
        ''' <returns>True:������� False:������Ȃ�</returns>
        ''' <remarks>�ǂ̗p���ɂ��邩</remarks>
        Public Function PageSetupDialog_show() As Boolean
            Dim retc As System.Windows.Forms.DialogResult
            '' �C���X�^���X���� & �ݒ�
            PageSetupDialog_Inctance()
            retc = m_PageSetup.ShowDialog()
            '' �߂�l�Z�b�g
            If retc = DialogResult.OK Then Return True
            Return False
        End Function

        ''' <summary>
        ''' ����v���r���[ �\��
        ''' </summary>
        ''' <param name="cntObj">�n�[�h�R�s�[���Ƃ�R���g���[��</param>
        ''' <returns>True:������� False:������Ȃ�</returns>
        ''' <remarks>����C���[�W</remarks>
        Public Function PrintPreviewDialog_show(ByVal cntObj As Control) As Boolean
            Dim retc As System.Windows.Forms.DialogResult

            Try
                HCopy(cntObj)
                '' �C���X�^���X���� & �ݒ�
                PrintPreviewDialog_Inctance()
                retc = m_Preview.ShowDialog()
                '' �߂�l�Z�b�g
                If retc = DialogResult.OK Then Return True
                Return False
            Catch ex As Exception
                Return False
            End Try
        End Function

        '----------------------------------------------------------------
        ' �T�v�F
        ' �����F(in :Control) cnt = 
        ' �ߒl�F�Ȃ�
        ' ���l�F
        '----------------------------------------------------------------
        ''' <summary>
        ''' ����J�n
        ''' </summary>
        ''' <param name="cntObj">�n�[�h�R�s�[���Ƃ�R���g���[�� (�ʏ�:Me���w�肵�Ă�����OK)</param>
        ''' <remarks></remarks>
        Public Sub Print(ByVal cntObj As Control)
            Dim ii As Integer

            '' �t�H�[���C���[�W�擾
            HCopy(cntObj)
            '' ���
            '�Ȃ��� �ŏ���1��ڂ��G���[�ɂȂ�
            '�f�o�b�N��1�s�Â������Ă����ƁA���܂ɐ���ɍs����������E�E�E�H
            For ii = 1 To 3
                Try
                    m_PrintDoc.Print()      '����J�n
                    Exit For
                Catch ex As Exception
                End Try
            Next ii
        End Sub

        '�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P�P
        ' �T�v�F
        '�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q�Q
        ''' <summary>
        ''' ����C�x���g
        ''' </summary>
        ''' <param name="sender"></param>
        ''' <param name="e"></param>
        ''' <remarks></remarks>
        Private Sub PrintDoc_PrintPage(ByVal sender As System.Object, ByVal e As System.Drawing.Printing.PrintPageEventArgs)
            '' ����{������
            Dim ritu As Single = 1
            If m_Strach Then ritu = ImageSize(e)

            '' �O���t�B�b�N�I�u�W�F�N�g�ɃZ�b�g
            e.Graphics.DrawImage(m_Bitmap, e.MarginBounds.X, e.MarginBounds.Y, m_Bitmap.Width * ritu, m_Bitmap.Height * ritu)
            '���̃y�[�W���Ȃ����Ƃ�ʒm����
            e.HasMorePages = False
        End Sub

        ''' <summary>
        ''' �X�g���b�`�ݒ� (1�y�[�W�𒴂��Ȃ��ő�T�C�Y���Z�o����)
        ''' </summary>
        ''' <param name="e">����C�x���g�I�u�W�F�N�g</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Private Function ImageSize(ByVal e As System.Drawing.Printing.PrintPageEventArgs) As Single
            Dim rituX As Single         'X�{��
            Dim rituY As Single         'Y�{��
            Dim margin As System.Drawing.Printing.Margins
            '' �g��k���{�����Z�o
            margin = Me.prop_Margins
            rituX = CSng((e.Graphics.VisibleClipBounds.Width - margin.Top - margin.Bottom) / m_Bitmap.Width)
            rituY = CSng((e.Graphics.VisibleClipBounds.Height - margin.Left - margin.Right) / m_Bitmap.Height)

            '' �r�b�g�}�b�v�̊g��k���{�������肷�� (�{���̂�菬������)
            If rituX < rituY Then
                Return rituX
            Else
                Return rituY
            End If
        End Function

        ''' <summary>
        ''' �]���T�C�Y�����肷��
        ''' </summary>
        ''' <param name="left">[mm]</param>
        ''' <param name="right"></param>
        ''' <param name="top"></param>
        ''' <param name="bottom"></param>
        ''' <remarks>�ݒ�f�[�^�� [1/100�C���`]�ׁ̈A�ϊ�����֐�(0�ɂ��Ă��A�v�����^�����Ή��Ȃ�A������Ɨ]�����󂢂ĕ\�������B�����āA���ꂽ�� �Ōオ�؂��)</remarks>
        Public Sub setMargins(ByVal left As Integer, ByVal right As Integer, ByVal top As Integer, ByVal bottom As Integer)
            '' �v�����^�ŕ\���o���Ȃ���������A�]���ݒ肳���� ���̕�����������
            If left < mc_DefMagin_LEFT Then      '�\���o���Ȃ�����
                left = mc_DefMagin_LEFT
            Else
                left -= mc_DefMagin_LEFT
            End If
            If top < mc_DefMagin_TOP Then      '�\���o���Ȃ�����
                top = mc_DefMagin_TOP
            Else
                top -= mc_DefMagin_TOP
            End If
            If right < mc_DefMagin_RIGHT Then      '�\���o���Ȃ�����
                right = mc_DefMagin_RIGHT
            Else
                right -= mc_DefMagin_RIGHT
            End If
            If bottom < mc_DefMagin_BOT Then      '�\���o���Ȃ�����
                bottom = mc_DefMagin_BOT
            Else
                bottom -= mc_DefMagin_BOT
            End If

            '' [mm] �� [1/100�C���`]
            left = CInt(left / 0.254)
            right = CInt(right / 0.254)
            top = CInt(top / 0.254)
            bottom = CInt(bottom / 0.254)

            '' �]���ݒ�
            Me.prop_Margins = New System.Drawing.Printing.Margins(left, right, top, bottom)
        End Sub

        ''' <summary>
        ''' �w��R���g���[���̃n�[�h�R�s�[���擾����
        ''' </summary>
        ''' <param name="cnt">�n�[�h�R�s�[���Ƃ�R���g���[��</param>
        ''' <param name="bitImg">�n�[�h�R�s�[���擾�����r�b�g�}�b�v</param>
        ''' <remarks></remarks>

        Public Overloads Shared Sub HCopy(ByVal cnt As Control, ByRef bitImg As Bitmap)
            Dim gp As Graphics = cnt.CreateGraphics                  '�t�H�[����Graphic�I�u�W�F�N�g�̍쐬
            bitImg = New Bitmap(cnt.Width, cnt.Height, gp)  '�r�b�g�}�b�v�̈�m��
            Dim fomG As Graphics = Graphics.FromImage(bitImg)
            'Dim Hdc1 As IntPtr = gp.GetHdc()   '�|�C���^�擾  (���ꂾ�ƃ^�C�g���o�[�̕������擾�o���Ȃ�)
            ' 20180702 �L���v�`���[�s��Ή� ----->>>
            'Dim Hdc1 As IntPtr = GetWindowDC(cnt.Handle)
            Dim Hdc1 As IntPtr = GetWindowDC(IntPtr.Zero)
            ' 20180702 �L���v�`���[�s��Ή� <<<-----
            Dim Hdc2 As IntPtr = fomG.GetHdc()  '�|�C���^�擾

            '' �C���[�W�擾
            ' 20180702 �L���v�`���[�s��Ή� ----->>>
            'BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, 0, 0, SRCCOPY)
            'BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, 0, 0, SRCCOPY Or CAPTUREBLT)
            BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, cnt.Left, cnt.Top, SRCCOPY Or CAPTUREBLT)
            ' 20180702 �L���v�`���[�s��Ή� <<<-----

            '' �g�����C���X�^���X�J��
            'gp.ReleaseHdc(Hdc1)
            'ReleaseDC(cnt.Handle, Hdc1)
            ' 20180702 �L���v�`���[�s��Ή� ----->>>
            ReleaseDC(IntPtr.Zero, Hdc1)
            ' 20180702 �L���v�`���[�s��Ή� <<<-----
            fomG.ReleaseHdc(Hdc2)
            gp.Dispose()
        End Sub

        ''' <summary>
        ''' �w��R���g���[���̃n�[�h�R�s�[��ۑ� (bitmap)
        ''' </summary>
        ''' <param name="cnt">�n�[�h�R�s�[���Ƃ�R���g���[��</param>
        ''' <param name="strPath">�t�@�C���t���p�X</param>
        ''' <remarks></remarks>
        Public Overloads Sub HCopy(ByVal cnt As Control, ByVal strPath As String)
            Dim bitImg As Bitmap = Nothing
            HCopy(cnt, bitImg)
            bitImg.Save(strPath, System.Drawing.Imaging.ImageFormat.Bmp)
            bitImg.Dispose()
        End Sub

        ''' <summary>
        ''' �w��R���g���[���̃n�[�h�R�s�[��ۑ� (�`���w��)
        ''' </summary>
        ''' <param name="cnt">�n�[�h�R�s�[���Ƃ�R���g���[��</param>
        ''' <param name="strPath">�t�@�C���t���p�X</param>
        ''' <param name="format">�t�H�[�}�b�g(ImageFormat.jpeg, ImageFormat.bmp) </param>       
        ''' <remarks></remarks>
        Public Overloads Sub HCopy(ByVal cnt As Control, ByVal strPath As String, ByVal format As System.Drawing.Imaging.ImageFormat)
            Dim bitImg As Bitmap = Nothing
            HCopy(cnt, bitImg)
            bitImg.Save(strPath, format)
            bitImg.Dispose()
        End Sub

        ''' <summary>
        ''' �w��R���g���[���̃n�[�h�R�s�[���擾����
        ''' </summary>
        ''' <param name="cnt">�n�[�h�R�s�[���Ƃ�R���g���[��</param>
        ''' <remarks></remarks>
        Private Overloads Sub HCopy(ByVal cnt As Control)
            Dim gp As Graphics = cnt.CreateGraphics                 '�t�H�[����Graphic�I�u�W�F�N�g�̍쐬
            m_Bitmap = New Bitmap(cnt.Width, cnt.Height, gp)  '�r�b�g�}�b�v�̈�m��
            Dim fomG As Graphics = Graphics.FromImage(m_Bitmap)
            'Dim Hdc1 As IntPtr = gp.GetHdc()   '�|�C���^�擾
            ' 20200514 �L���v�`���[�s��Ή� ----->>>
            'Dim Hdc1 As IntPtr = GetWindowDC(cnt.Handle)
            Dim Hdc1 As IntPtr = GetWindowDC(IntPtr.Zero)
            ' 20200514 �L���v�`���[�s��Ή� <<<-----
            Dim Hdc2 As IntPtr = fomG.GetHdc()  '�|�C���^�擾

            '' �C���[�W�擾
            ' 20200514 �L���v�`���[�s��Ή� ----->>>
            'BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, 0, 0, SRCCOPY)
            BitBlt(Hdc2, 0, 0, cnt.Width, cnt.Height, Hdc1, cnt.Left, cnt.Top, SRCCOPY Or CAPTUREBLT)
            ' 20200514 �L���v�`���[�s��Ή� <<<-----

            '' �g�����C���X�^���X�J��
            'gp.ReleaseHdc(Hdc1)
            ' 20200514 �L���v�`���[�s��Ή� ----->>>
            'ReleaseDC(cnt.Handle, Hdc1)
            ReleaseDC(IntPtr.Zero, Hdc1)
            ' 20200514 �L���v�`���[�s��Ή� <<<-----
            fomG.ReleaseHdc(Hdc2)
            gp.Dispose()
        End Sub

    End Class
End Namespace





#Region "�o�b�N"

''''Namespace tClasstest


''''    Public Class tClass_HeadCopy
''''        Implements IDisposable                          '�f�X�g���N�^��`


''''        '********************************************************************
''''        'API��`
''''        '********************************************************************
''''        '��ʂ̃C���[�W���擾
''''        Private Declare Auto Function BitBlt Lib "gdi32.dll" (ByVal hdcDest As IntPtr, ByVal nXDest As Integer, ByVal nYDest As Integer, ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal hdcSrc As IntPtr, ByVal nXSrc As Integer, ByVal nYSrc As Integer, ByVal dwRop As System.Int32) As Boolean
''''        Const SRCCOPY As Integer = &HCC0020

''''        '********************************************************************
''''        '�����o�[�ϐ���`
''''        '********************************************************************
''''        Private m_PrintDoc As System.Drawing.Printing.PrintDocument      '�v�����g����R���g���[��
''''        Private m_PrintDialog As System.Windows.Forms.PrintDialog       '�v�����g�_�C�����O
''''        'Private mPrintSet As System.Windows.Forms.PageSetupDialog      '����ݒ�_�C�����O
''''        'Private mPrintView As System.Windows.Forms.PrintPreviewDialog      '����v���r���[
''''        Private m_PrintPoint As System.Drawing.Point                     '����J�n�ʒu

''''       ' Private m_bmp As Bitmap         '��ʃC���[�W
''''        Private m_cntObj As Control
''''        Private m_pointF As PointF      '�C���[�W�\���ʒu(����̍��W)

''''        Public Sub New()
''''            ''' �C���[�W�\���ʒu
''''            m_pointF = New PointF(0, 0)








''''                ''' �v�����^�ݒ�
''''                Dim PSize As System.Drawing.Printing.PaperSize          '�p���T�C�Y�ύX

''''                '�C���X�^���X�쐬
''''                m_PrintDoc = New System.Drawing.Printing.PrintDocument
'''' '               m_PrintDialog = New System.Windows.Forms.PrintDialog
''''        '        mPrintSet = New System.Windows.Forms.PageSetupDialog
''''        'mPrintView = New System.Windows.Forms.PrintPreviewDialog

''''                'PrintDoc �̃C�x���g�n���h����ǉ�
''''                AddHandler m_PrintDoc.PrintPage, AddressOf PrintDoc_PrintPage

''''                '����v���r���[�ƃv�����g�h�L�������g���֘A�t��
''''  '              m_PrintDialog.Document = m_PrintDoc
''''        '        mPrintSet.Document = mPrintDoc
''''        'mPrintView.Document = mPrintDoc
''''                '''�f�[�^�Z�b�g
''''                '������� (True:��  false:�c)
''''                'mPrintSet.PageSettings.Landscape = True
''''                '����v���r���[�\���t���O

''''                '�p���T�C�Y�擾
''''                For Each PSize In m_PrintDoc.PrinterSettings.PaperSizes
''''                    If PSize.PaperName.StartsWith("A4") Then
''''                        '�p���T�C�Y�ݒ�
''''                        m_PrintDoc.PrinterSettings.DefaultPageSettings.PaperSize = PSize
''''                        Exit For
''''                    End If
''''                Next

''''                '��������Z�b�g
''''                m_PrintDoc.PrinterSettings.Copies = 1

''''        End Sub

''''        Public Sub Dispose() Implements System.IDisposable.Dispose
''''           ' If Not m_bmp Is Nothing Then m_bmp.Dispose()

''''        End Sub


''''        Private Sub PrintDoc_PrintPage(ByVal sender As System.Object, ByVal e As System.Drawing.Printing.PrintPageEventArgs)
''''            Dim bmp As Bitmap         '��ʃC���[�W
''''            HCopy(m_cntObj, bmp)
''''            e.Graphics.DrawImage(bmp, m_pointF)
''''            bmp.Dispose()
''''        End Sub



''''        Public Sub Print(ByVal cntObj As Control)
''''            m_cntObj = cntObj
''''            Try
'''' '               ''' �v�����^�ݒ�
'''' '               Dim PSize As System.Drawing.Printing.PaperSize          '�p���T�C�Y�ύX

'''' '               '�C���X�^���X�쐬
'''' '               m_PrintDoc = New System.Drawing.Printing.PrintDocument
'''' ''               m_PrintDialog = New System.Windows.Forms.PrintDialog
'''' '       '        mPrintSet = New System.Windows.Forms.PageSetupDialog
'''' '       'mPrintView = New System.Windows.Forms.PrintPreviewDialog

'''' '               'PrintDoc �̃C�x���g�n���h����ǉ�
'''' '               AddHandler m_PrintDoc.PrintPage, AddressOf PrintDoc_PrintPage

'''' '               '����v���r���[�ƃv�����g�h�L�������g���֘A�t��
'''' ' '              m_PrintDialog.Document = m_PrintDoc
'''' '       '        mPrintSet.Document = mPrintDoc
'''' '       'mPrintView.Document = mPrintDoc
'''' '               '''�f�[�^�Z�b�g
'''' '               '������� (True:��  false:�c)
'''' '               'mPrintSet.PageSettings.Landscape = True
'''' '               '����v���r���[�\���t���O

'''' '               '�p���T�C�Y�擾
'''' '               For Each PSize In m_PrintDoc.PrinterSettings.PaperSizes
'''' '                   If PSize.PaperName.StartsWith("A4") Then
'''' '                       '�p���T�C�Y�ݒ�
'''' '                       m_PrintDoc.PrinterSettings.DefaultPageSettings.PaperSize = PSize
'''' '                       Exit For
'''' '                   End If
'''' '               Next

'''' '               '��������Z�b�g
'''' '               m_PrintDoc.PrinterSettings.Copies = 1
'''' m_PrintDoc.DocumentName = "test1"
''''                m_PrintDoc.Print()
''''            Catch ex As Exception
''''MsgBox("")
''''    End Try


''''        End Sub




''''        '----------------------------------------------------------------
''''        ' �T�v�F�n�[�h�R�s�[���擾����
''''        ' �����F(in :)  = 
''''        ' �ߒl�F�Ȃ�
''''        ' ���l�F
''''        '----------------------------------------------------------------
''''        Public Shared Sub HCopy(ByVal cnt As Control, ByRef bitImg As Bitmap)
''''            Dim gp As Graphics = cnt.CreateGraphics                 '�t�H�[����Graphic�I�u�W�F�N�g�̍쐬
''''            bitImg = New Bitmap(cnt.ClientRectangle.Width, cnt.ClientRectangle.Height, gp)  '�r�b�g�}�b�v�̈�m��
''''            Dim fomG As Graphics = gp.FromImage(bitImg)
''''            Dim Hdc1 As IntPtr = gp.GetHdc()    '�|�C���^�擾
''''            Dim Hdc2 As IntPtr = fomG.GetHdc()  '�|�C���^�擾

''''            ''' �C���[�W�擾
''''            BitBlt(Hdc2, 0, 0, cnt.ClientRectangle.Width, cnt.ClientRectangle.Height, Hdc1, 0, 0, SRCCOPY)

''''            ''' �g�����C���X�^���X�J��
''''            gp.ReleaseHdc(Hdc1)
''''            fomG.ReleaseHdc(Hdc2)
''''            gp.Dispose()
''''        End Sub
''''    End Class
''''End Namespace

#End Region
#Region "�o�b�N"

''''Namespace tClasstest2


''''    Public Class tClass_HeadCopy
''''        Implements IDisposable                          '�f�X�g���N�^��`


''''        '********************************************************************
''''        'API��`
''''        '********************************************************************
''''        '��ʂ̃C���[�W���擾
''''        Private Declare Auto Function BitBlt Lib "gdi32.dll" (ByVal hdcDest As IntPtr, ByVal nXDest As Integer, ByVal nYDest As Integer, ByVal nWidth As Integer, ByVal nHeight As Integer, ByVal hdcSrc As IntPtr, ByVal nXSrc As Integer, ByVal nYSrc As Integer, ByVal dwRop As System.Int32) As Boolean
''''        Const SRCCOPY As Integer = &HCC0020

''''        '********************************************************************
''''        '�����o�[�ϐ���`
''''        '********************************************************************
''''        Private m_PrintDoc As System.Drawing.Printing.PrintDocument      '�v�����g����R���g���[��
''''        Private m_PrintDialog As System.Windows.Forms.PrintDialog       '�v�����g�_�C�����O

''''        Private m_PrintPoint As System.Drawing.Point                     '����J�n�ʒu


''''        Private m_cntObj As Control
''''        Private m_pointF As PointF      '�C���[�W�\���ʒu(����̍��W)

''''        Public Sub New()
''''            ''' �C���[�W�\���ʒu
''''            m_pointF = New PointF(0, 0)

''''                ''' �v�����^�ݒ�
''''                Dim PSize As System.Drawing.Printing.PaperSize          '�p���T�C�Y�ύX

''''                '�C���X�^���X�쐬
''''                m_PrintDoc = New System.Drawing.Printing.PrintDocument
''''                'PrintDoc �̃C�x���g�n���h����ǉ�
''''                AddHandler m_PrintDoc.PrintPage, AddressOf PrintDoc_PrintPage

''''                '�p���T�C�Y�擾
''''                For Each PSize In m_PrintDoc.PrinterSettings.PaperSizes
''''                    If PSize.PaperName.StartsWith("A4") Then
''''                        '�p���T�C�Y�ݒ�
''''                        m_PrintDoc.PrinterSettings.DefaultPageSettings.PaperSize = PSize
''''                        Exit For
''''                    End If
''''                Next

''''                '��������Z�b�g
''''                m_PrintDoc.PrinterSettings.Copies = 1
''''        End Sub

''''        Public Sub Dispose() Implements System.IDisposable.Dispose
''''        End Sub


''''        Private Sub PrintDoc_PrintPage(ByVal sender As System.Object, ByVal e As System.Drawing.Printing.PrintPageEventArgs)
''''            Dim bmp As Bitmap         '��ʃC���[�W
''''            HCopy(m_cntObj, bmp)
''''            e.Graphics.DrawImage(bmp, m_pointF)
''''            bmp.Dispose()
''''        End Sub



''''        Public Sub Print(ByVal cntObj As Control)
''''            m_cntObj = cntObj
''''            Try
'''' m_PrintDoc.DocumentName = "test2"
''''                m_PrintDoc.Print()
''''            Catch ex As Exception
''''MsgBox("")
''''    End Try


''''        End Sub

''''        '----------------------------------------------------------------
''''        ' �T�v�F�n�[�h�R�s�[���擾����
''''        ' �����F(in :)  = 
''''        ' �ߒl�F�Ȃ�
''''        ' ���l�F
''''        '----------------------------------------------------------------
''''        Public Shared Sub HCopy(ByVal cnt As Control, ByRef bitImg As Bitmap)
''''            Dim gp As Graphics = cnt.CreateGraphics                 '�t�H�[����Graphic�I�u�W�F�N�g�̍쐬
''''            bitImg = New Bitmap(cnt.ClientRectangle.Width, cnt.ClientRectangle.Height, gp)  '�r�b�g�}�b�v�̈�m��
''''            Dim fomG As Graphics = gp.FromImage(bitImg)
''''            Dim Hdc1 As IntPtr = gp.GetHdc()    '�|�C���^�擾
''''            Dim Hdc2 As IntPtr = fomG.GetHdc()  '�|�C���^�擾

''''            ''' �C���[�W�擾
''''            BitBlt(Hdc2, 0, 0, cnt.ClientRectangle.Width, cnt.ClientRectangle.Height, Hdc1, 0, 0, SRCCOPY)

''''            ''' �g�����C���X�^���X�J��
''''            gp.ReleaseHdc(Hdc1)
''''            fomG.ReleaseHdc(Hdc2)
''''            gp.Dispose()
''''        End Sub
''''    End Class
''''End Namespace
#End Region