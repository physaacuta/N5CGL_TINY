'*********************************************************************************
' �S��ʂ̊��t�H�[��
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

Imports tClassLibrary

''' <summary>
''' �S��ʂ̊��t�H�[��
''' </summary>
''' <remarks></remarks>
Public Class frmBase
	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ��`
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ��ʏ�Ԃ̃f�t�H���g��`
	''' </summary>
	''' <remarks></remarks>
	Public Enum gem_FromFormatMode
		''' <summary>�w�薳��</summary>
		iNON = 0
		''' <summary>�I�����C���p</summary>
		iONLINE = 1
		''' <summary>�I�t���C��(���u)�p</summary>
		iOFFLINE = 2
	End Enum

	Private Delegate Sub dele_LoadEnding()								' �񓯊��C�x���g���s�p�B(shown�C�x���g�̑��݂�m��Ȃ������ׁA������B��������Ȃ����ǁA�݊��̈׎c���Ă�����)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_nFromFormatMode As gem_FromFormatMode = gem_FromFormatMode.iNON		' ��ʏ�Ԃ̃f�t�H���g��` (0:�w�薳�� 1:�I�����C���p 2:�I�t���C��(���u)�p)					
	Private m_bMoveable As Boolean = False								' ��ʈړ��֎~ (VB6��Moveable�ɑ���������)
	Private m_bAltF4Enable As Boolean = False							' Windows�L�[�ɂ���ʏI���֎~ (Alt + F4 ������)
	Private m_bEndButtonEnable As Boolean = True						' �~�{�^���L������


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���v���p�e�B
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ��ʈړ��֎~ (VB6��Moveable�ɑ���������)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(False) _
	 , System.ComponentModel.Category("��kizuForm��p�v���p�e�B") _
	 , System.ComponentModel.Description("��ʈړ��֎~ (VB6��Moveable�ɑ���������)")> _
	Public Property tProp_Moveable() As Boolean
		Get
			Return m_bMoveable
		End Get
		Set(ByVal Value As Boolean)
			m_bMoveable = Value
		End Set
	End Property
	''' <summary>
	''' Windows�L�[�ɂ���ʏI���֎~ (Alt + F4 ������)
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(False) _
	 , System.ComponentModel.Category("��kizuForm��p�v���p�e�B") _
	 , System.ComponentModel.Description("Windows�L�[�ɂ���ʏI���֎~ (Alt + F4 ������)")> _
	Public Property tProp_AltF4Enable() As Boolean
		Get
			Return m_bAltF4Enable
		End Get
		Set(ByVal Value As Boolean)
			m_bAltF4Enable = Value
		End Set
	End Property
	''' <summary>
	''' �~�{�^���L�������؂�ւ�
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(True) _
	 , System.ComponentModel.Category("��kizuForm��p�v���p�e�B") _
	 , System.ComponentModel.Description("�~�{�^���L�������؂�ւ� (false:����)")> _
	Public Property tProp_EndButtonEnable() As Boolean
		Get
			Return m_bEndButtonEnable
		End Get
		Set(ByVal Value As Boolean)
			m_bEndButtonEnable = Value
			' ���s���̐؂�ւ��������瑦�����s
			If Not Value Then
				subEndButtonEnable()
			End If
		End Set
	End Property
	''' <summary>
	''' ��ʏ�Ԓ�`
	''' </summary>
	''' <value></value>
	''' <returns></returns>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DefaultValue(gem_FromFormatMode.iNON) _
	 , System.ComponentModel.Category("��kizuForm��p�v���p�e�B") _
	 , System.ComponentModel.Description("��ʏ�Ԃ̃f�t�H���g��`" & vbCrLf & _
										 "  NON:�w�薳��" & vbCrLf & _
										 "  ONLINE:�I�����C���p(�t�H�[���T�C�Y�ύX�s�E�ő�ŏ��{�^���폜��)" & vbCrLf & _
										 "  OFFLINE:�I�t���C���p(�t�H�[���T�C�Y�ύX�E�ő�ŏ��{�^���\����))")> _
	Public Property tProp_FromFormatMode() As gem_FromFormatMode
		Get
			Return m_nFromFormatMode
		End Get
		Set(ByVal Value As gem_FromFormatMode)
			m_nFromFormatMode = Value
			Call setFormFormatMode(Value)
		End Set
	End Property




	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���A�N�Z�X ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ��ʃ��[�h�ݒ�
	''' </summary>
	''' <param name="mode">iONLINE:�I�����C���p iOFFLINE:�I�t���C��(���u)�p</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetFormFormatMode(ByVal mode As gem_FromFormatMode)
		If gem_FromFormatMode.iNON = mode Then						' �w�薳��

		ElseIf gem_FromFormatMode.iONLINE = mode Then				' �I�����C��
			Me.ControlBox = False													' �R���g���[���{�^�� ����
			Me.MaximizeBox = False													' �{�^���������Ȃ������ŁA�^�C�g���o�[�N���b�N������ő剻�����
			Me.MinimizeBox = False
			Me.FormBorderStyle = Windows.Forms.FormBorderStyle.FixedSingle			' �t�H�[���T�C�Y�ύX�s��
			Me.tProp_Moveable = False												' ��ʈړ��֎~

		ElseIf gem_FromFormatMode.iOFFLINE = mode Then				' ���u
			Me.ControlBox = True													' �R���g���[���{�^�� �L��
			Me.MaximizeBox = True
			Me.MinimizeBox = True
			Me.FormBorderStyle = Windows.Forms.FormBorderStyle.Sizable				' �t�H�[���T�C�Y�ύX�\
			Me.tProp_Moveable = True												' ��ʈړ�����
		End If
	End Sub

	''' <summary>
	''' Me.Activated �ł͎g�����ɂȂ�Ȃ��ׁALoad�̏I����񂾂��񓯊��Ń��\�b�h���R�[�����Ă�����C�x���g���������B
	''' </summary>
	''' <remarks></remarks>
	Protected Overridable Sub LoadEnding()
		'' �~�{�^���̓���
		subEndButtonEnable()
	End Sub

	''' <summary>
	''' ����ʂ��w�胂�j�^�ʒu�ɕ\��
	''' </summary>
	''' <param name="nMonitorNo">���j�^�ԍ� (1�I���W��)</param>
	''' <param name="bFit">�t�H�[�������j�^�[�T�C�Y�ɍ��킹��</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetLocationForm(ByVal nMonitorNo As Integer, Optional ByVal bFit As Boolean = False)
		'' �\�����������j�^�̏��ɂ��킹�ăt�H�[���T�C�Y��ύX
		Dim monitor() As System.Windows.Forms.Screen
		monitor = System.Windows.Forms.Screen.AllScreens()

		nMonitorNo -= 1	'�Ȃ����̂���1�I���W���ɂȂ��Ă����ׁA�݊����ׂ̈ɂ��̂܂܂Ƃ���

		'' �ԍ��`�F�b�N
		If 0 > nMonitorNo Or nMonitorNo >= monitor.Length Then
			nMonitorNo = 0
		End If

		'' �ʒu�̂݁A �T�C�Y���܂߂� �����H
		If bFit Then
			Me.Bounds = monitor(nMonitorNo).Bounds
		Else
			Me.Location = monitor(nMonitorNo).Bounds.Location
		End If
	End Sub

	''' <summary>
	''' ��ʈ��
	''' </summary>
	''' <param name="bYoko">True:�� False:�c</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub PrintOut(Optional ByVal bYoko As Boolean = True)
		Dim cls_Hcopy As New tClass.tClass_HeadCopy

		Try
			cls_Hcopy.prop_Landscape = bYoko	' �������
			cls_Hcopy.prop_Strach = True		' �X�g���b�`
			Call cls_Hcopy.Print(Me)			' �\�����̎���ʂ����

		Catch ex As Exception

		Finally
			If Not cls_Hcopy Is Nothing Then
				cls_Hcopy.Dispose()
				cls_Hcopy = Nothing
			End If
		End Try

	End Sub

	''' <summary>
	''' �n�[�h�R�s�[
	''' </summary>
	''' <param name="strFolder">�ۑ��t�@�C���̃t�H���_�p�X</param>
	''' <param name="strName">�ۑ��t�@�C���̃t�@�C����</param>
	''' <param name="emSaveMode">�ۑ��`�� (jpeg��:System.Drawing.Imaging.ImageFormat.Jpeg)</param>
	''' <remarks></remarks>
	Protected Friend Overridable Sub HardCopy(ByVal strFolder As String, ByVal strName As String, ByVal emSaveMode As System.Drawing.Imaging.ImageFormat)
		Dim cls_Hcopy As New tClass.tClass_HeadCopy
		Dim strFPath As String
		Dim strMsg As String

		'''' �t���p�X
		strFPath = IO.Path.Combine(strFolder, strName)

		'''' �t�H���_�̗L���`�F�b�N(�����������낤)
		tMod.CheckFolder(strFolder)

		'''' �n�[�h�R�s�[
		Try
			Call cls_Hcopy.HCopy(Me, strFPath, emSaveMode)
			strMsg = System.IO.Path.GetFullPath(strFPath) & "�ɉ摜��ۑ����܂����B"
		Catch ex As Exception
			strMsg = "�摜�ۑ��Ɏ��s���܂����B"
		Finally
			If Not cls_Hcopy Is Nothing Then
				cls_Hcopy.Dispose()
				cls_Hcopy = Nothing
			End If
		End Try

		'''' �K�C�_���X�\��
		Using frmMsg As New frmPop_MsgBox(strMsg, Me.Text)
			frmMsg.ShowDialog()
		End Using
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBaseMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        ' ���o�X�^�C���𖳌��ɂ���  (DataGridView�Ń{�^������ǉ����Ă��鎞�ɁA���o�X�^�C�����L�����Ɣw�i�F���ύX�ł��Ȃ�����)
        Application.VisualStyleState = VisualStyles.VisualStyleState.NonClientAreaEnabled

        '' �񓯊��Ńt�H�[���I���������_�ŃC�x���g�𔭐����Ă�����
        'Dim dele As New dele_LoadEnding(AddressOf LoadEnding)
        'Me.BeginInvoke(dele) '�ʃX���b�h����̃A�N�Z�X�p
    End Sub

	''' <summary>
	''' ���[�h����
	''' </summary>
	Private Sub frmBase_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
		' �񓯊��Ńt�H�[���I���������_�ŃC�x���g�𔭐����Ă�����
		Dim dele As New dele_LoadEnding(AddressOf LoadEnding)
		Me.BeginInvoke(dele) '�ʃX���b�h����̃A�N�Z�X�p
	End Sub

	''' <summary>
	''' �J��
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmBaseMain_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
		' ����Ȃɂ�����
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���� ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �~�{�^�� ���������� (�f�U�C�i�̍Ō�ŃR�[�������)
	''' </summary>
	''' <remarks></remarks>
	Private Sub subEndButtonEnable()
		' �f�U�C�i�̏ꍇ�́A�����ɂ����ق����悢�H

		' �t�H�[���̕\��������������łȂ��ƁA�n���h���������ׁ̈A���ӂ��鎖
		Try
			If Not m_bEndButtonEnable Then
				tClassLibrary.tMod.subFormEndButton_Desebled(Me.Handle)
			End If
		Catch ex As Exception
		End Try
	End Sub


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �I�[�o�[���C�h ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ����ʂ��w�胂�j�^�ʒu�ɕ\��
	''' </summary>
	''' <param name="nMonitorNo">���j�^�ԍ� (1�I���W��)</param>
	''' <param name="bFit">�t�H�[�������j�^�[�T�C�Y�ɍ��킹��</param>
	''' <remarks></remarks>
	Public Overloads Sub Show(ByVal nMonitorNo As Integer, Optional ByVal bFit As Boolean = False)
		' �Ǝ�
		Call SetLocationForm(nMonitorNo, bFit)

		' ������̏��������s
		MyBase.Show()
	End Sub
	Public Overloads Sub ShowDialog(ByVal nMonitorNo As Integer, Optional ByVal bFit As Boolean = False)
		' �Ǝ�
		Call SetLocationForm(nMonitorNo, bFit)

		' ������̏��������s
		MyBase.ShowDialog()
	End Sub

	'' �f�o�b�N����΍�
	' �f�o�b�N�ł��ꂪ�L���Ƃ��Ă���ƁA�f�o�b�N���Ƀt�b�N����ăe�X�g���ɂ����ׁA�f�o�b�N���̂ݖ�����
	' ���ׁ̈A�ȉ��̏������A�f�o�b�N���ɂ́A�L���ƂȂ�Ȃ��ׁA���ӂ��鎖
	'	�E ��ʈړ��̋֎~
	'	�E Alt + F4 �ɂ�邪��ʏI�� ������
#If Not Debug Then
	''' <summary>
	''' WndProc �I�[�o�[���C�h
	''' </summary>
	''' <param name="m"></param>
	''' <remarks></remarks>
	Protected Overrides Sub WndProc(ByRef m As Message)
		Const WM_SYSCOMMAND As Integer = &H112		' �V�X�e���R�}���h	
		Const SC_MOVE As Integer = &HF010		   ' �E�B���h�E�ړ�

		Try
			'''''' �J�X�^�}�C�Y
			' ��ʈړ��֎~
			If Not m_bMoveable AndAlso m.Msg = WM_SYSCOMMAND AndAlso (m.WParam.ToInt32() And &HFFF0) = SC_MOVE Then
				m.Result = IntPtr.Zero
			Else
				'' �S���b�Z�[�W ����
				MyBase.WndProc(m)
			End If
		Catch ex As Exception

		End Try
	End Sub

	''' <summary>
	''' ProcessCmdKey �I�[�o�[���C�h
	''' </summary>
	''' <param name="msg"></param>
	''' <param name="keyData"></param>
	''' <returns></returns>
	''' <remarks></remarks>
	Protected Overrides Function ProcessCmdKey(ByRef msg As System.Windows.Forms.Message, ByVal keyData As System.Windows.Forms.Keys) As Boolean
		Const WM_SYSKEYDOWN As Integer = &H104		' Alt�L�[����������Ԃő��̃L�[����������
		Const VK_F4 As Integer = &H73				' F4�L�[

		Try
			'''''' �J�X�^�}�C�Y
			' Alt + F4 �ɂ�邪��ʏI�� ������
			If Not m_bAltF4Enable AndAlso msg.Msg = WM_SYSKEYDOWN AndAlso msg.WParam.ToInt32() = VK_F4 Then
				msg.Result = IntPtr.Zero
				Return True
			Else
				'' �S���b�Z�[�W ����
				MyBase.ProcessCmdKey(msg, keyData)
			End If
		Catch ex As Exception

		End Try
	End Function
#End If



End Class