'*********************************************************************************
' ���j���[��ʂ̐e�t�H�[��
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On

''' <summary>
''' ���j���[��ʂ̐e�t�H�[��
''' </summary>
''' <remarks></remarks>
Public Class frmMain_Menu2

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_clMenuSelectColor As Color = Color.FromArgb(255, 64, 128)	' ���j���[�I�����̐F
	Private m_clMenuEnableColor As Color = Color.FromArgb(0, 0, 192)		' ���j���[���I�����̐F
	Private m_clMenuDesableColor As Color = Color.DimGray				' ���j���[�������̐F

	Private m_cMenuText(11) As String									' ���j���[�̃��x������
	Private m_bMenuEnable(11) As Boolean									' ���j���[�̗L������
	Protected Friend my_lblMenuF(11) As Label							' ���j���[�����R���g���[��
	Protected Friend my_lblMenuF_Line(11) As Label						' ���j���[�����R���g���[��


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���v���p�e�B
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ���j���[�I�����̐F
	''' </summary>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("�����j���[��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("���j���[�I�����̐F")> _
	Public Property tProp_MenuSelectColor() As Color
		Get
			Return m_clMenuSelectColor
		End Get
		Set(ByVal Value As Color)
			m_clMenuSelectColor = Value
			Call setMenuText_Format()
		End Set
	End Property
	''' <summary>
	''' ���j���[���I�����̐F
	''' </summary>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("�����j���[��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("���j���[���I�����̐F")> _
	Public Property tProp_MenuEnableColor() As Color
		Get
			Return m_clMenuEnableColor
		End Get
		Set(ByVal Value As Color)
			m_clMenuEnableColor = Value
			Call setMenuText_Format()
		End Set
	End Property
	''' <summary>
	''' ���j���[�������̐F
	''' </summary>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.Category("�����j���[��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("���j���[�������̐F")> _
	Public Property tProp_MenuDesableColor() As Color
		Get
			Return m_clMenuDesableColor
		End Get
		Set(ByVal Value As Color)
			m_clMenuDesableColor = Value
			Call setMenuText_Format()
		End Set
	End Property

	''' <summary>
	''' ���j���[�̃��x������
	''' </summary>
	''' <value></value>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Content) _
	 , System.ComponentModel.Category("�����j���[��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("���j���[�̃��x������ (����label�R���g���[����text�̋֎~ ���������g����)")> _
	Public Property tProp_MenuText() As String()
		Get
			Return m_cMenuText
		End Get
		Set(ByVal Value As String())
			If Value.Length <> m_cMenuText.Length Then Exit Property

			m_cMenuText = Value
			Call setMenuText_Format()
		End Set
	End Property
	Public Property tProp_MenuText(ByVal index As Integer) As String
		Get
			Return m_cMenuText(index)
		End Get
		Set(ByVal Value As String)
			m_cMenuText(index) = Value
			Call setMenuText_Format()
		End Set
	End Property
	
	''' <summary>
	''' ���j���[�̏��
	''' </summary>
	''' <value></value>
	''' <remarks></remarks>
	<System.ComponentModel.Browsable(True) _
	 , System.ComponentModel.DesignerSerializationVisibility(System.ComponentModel.DesignerSerializationVisibility.Content) _
	 , System.ComponentModel.Category("�����j���[��ʐ�p�v���p�e�B") _
	 , System.ComponentModel.Description("���j���[�̏�� (����label�R���g���[����text�̋֎~ ���������g����)")> _
	Public Property tProp_MenuEnable() As Boolean()
		Get
			Return m_bMenuEnable
		End Get
		Set(ByVal Value As Boolean())
			If Value.Length <> m_cMenuText.Length Then Exit Property

			m_bMenuEnable = Value
			Call setMenuText_Format()
		End Set
	End Property
	Public Property tProp_MenuEnable(ByVal index As Integer) As Boolean
		Get
			Return m_bMenuEnable(index)
		End Get
		Set(ByVal Value As Boolean)

			m_bMenuEnable(index) = Value
			Call setMenuText_Format()
		End Set
	End Property



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �C�x���g
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �R���X�g���N�^
	''' </summary>
	''' <remarks></remarks>
	Public Sub New()

		' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
		InitializeComponent()

		' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
		'''' ���j���[�̃R���g���[���z��𐶐�����
		my_lblMenuF(0) = lblMenuF01
		my_lblMenuF(1) = lblMenuF02
		my_lblMenuF(2) = lblMenuF03
		my_lblMenuF(3) = lblMenuF04
		my_lblMenuF(4) = lblMenuF05
		my_lblMenuF(5) = lblMenuF06
		my_lblMenuF(6) = lblMenuF07
		my_lblMenuF(7) = lblMenuF08
		my_lblMenuF(8) = lblMenuF09
		my_lblMenuF(9) = lblMenuF10
		my_lblMenuF(10) = lblMenuF11
		my_lblMenuF(11) = lblMenuF12

		my_lblMenuF_Line(0) = lblMenuF01_l
		my_lblMenuF_Line(1) = lblMenuF02_l
		my_lblMenuF_Line(2) = lblMenuF03_l
		my_lblMenuF_Line(3) = lblMenuF04_l
		my_lblMenuF_Line(4) = lblMenuF05_l
		my_lblMenuF_Line(5) = lblMenuF06_l
		my_lblMenuF_Line(6) = lblMenuF07_l
		my_lblMenuF_Line(7) = lblMenuF08_l
		my_lblMenuF_Line(8) = lblMenuF09_l
		my_lblMenuF_Line(9) = lblMenuF10_l
		my_lblMenuF_Line(10) = lblMenuF11_l
		my_lblMenuF_Line(11) = lblMenuF12_l
		'' ���j���[�ԍ��Z�b�g
		For ii As Integer = 0 To my_lblMenuF.Length - 1
			my_lblMenuF(ii).Tag = ii
			my_lblMenuF_Line(ii).Tag = ii

			m_cMenuText(ii) = ""
			m_bMenuEnable(ii) = False

			' �C�x���g�R�[�h�Œǉ� �ʂ��������߂߂�ǂ���������
''			AddHandler m_lblMenuF(ii).MouseHover, AddressOf lblMenuF_MouseHover		' ���j���[���x���Ƀ}�E�X������� (�C�x���g������̂ݎ��Ԃ��|����׃���)
			AddHandler my_lblMenuF(ii).MouseMove, AddressOf lblMenuF_MouseMove	   ' ���j���[���x���Ƀ}�E�X�������
			AddHandler my_lblMenuF(ii).MouseLeave, AddressOf lblMenuF_MouseLeave		' ���j���[���x���Ƀ}�E�X�����ꂽ
			AddHandler my_lblMenuF(ii).Click, AddressOf lblMenuF_Click				' ���j���[���x�����N���b�N

			' �}�E�X�|�C���^�Z�b�g
			my_lblMenuF(ii).Cursor = Cursors.Hand

		Next
	End Sub

	''' <summary>
	''' �t�H�[�����[�h
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub frmMain_Menu_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
		'' ���x���Z�b�g
		Call setMenuText_Format()
	End Sub

	''' <summary>
	''' ���j���[���x���Ƀ}�E�X��������
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub lblMenuF_MouseMove(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs)
		Dim FncNo As Integer = CInt(CType(sender, Label).Tag)		' No���o��
		If Not m_bMenuEnable(FncNo) Then Exit Sub
		If my_lblMenuF(FncNo).ForeColor = m_clMenuSelectColor Then Exit Sub
		'�F�ւ�
		my_lblMenuF(FncNo).ForeColor = m_clMenuSelectColor
		my_lblMenuF_Line(FncNo).BackColor = m_clMenuSelectColor
		'�t�H���g���� (�W����)
		my_lblMenuF(FncNo).Font = New Font(my_lblMenuF(FncNo).Font, FontStyle.Italic)
	End Sub

	''' <summary>
	''' ���j���[���x���Ƀ}�E�X�����ꂽ
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub lblMenuF_MouseLeave(ByVal sender As Object, ByVal e As System.EventArgs)
		Dim FncNo As Integer = CInt(CType(sender, Label).Tag)		' No���o��
		If Not m_bMenuEnable(FncNo) Then Exit Sub
		'�F�ւ�
		my_lblMenuF(FncNo).ForeColor = m_clMenuEnableColor
		my_lblMenuF_Line(FncNo).BackColor = m_clMenuEnableColor
		'�t�H���g���� (�΂߂�)
		my_lblMenuF(FncNo).Font = New Font(my_lblMenuF(FncNo).Font, FontStyle.Bold)
	End Sub
	''' <summary>
	''' ���j���[���x�����N���b�N
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub lblMenuF_Click(ByVal sender As Object, ByVal e As System.EventArgs)
		Dim FncNo As Integer = CInt(CType(sender, Label).Tag)		' No���o��
		If Not m_bMenuEnable(FncNo) Then Exit Sub
		'' �t�@���N�V�����{�^���������ꂽ�Ɠ��������������
		Call MyBase.SetEvFunctionButton(FncNo, my_btnFunction(FncNo).Enabled, Control.ModifierKeys())
	End Sub

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �O���A�N�Z�X ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ���j���[�e�L�X�g���Z�b�g
	''' </summary>
	''' <remarks></remarks>
	Protected Friend Overridable Sub SetMenuText_Format()
		For ii As Integer = 0 To my_lblMenuF.Length - 1

			my_lblMenuF(ii).Text = String.Format(" F{0,-6}{1}", ii + 1, m_cMenuText(ii))

			If m_bMenuEnable(ii) Then
				my_lblMenuF(ii).ForeColor = m_clMenuEnableColor
				my_lblMenuF_Line(ii).BackColor = m_clMenuEnableColor
			Else
				my_lblMenuF(ii).ForeColor = m_clMenuDesableColor
				my_lblMenuF_Line(ii).BackColor = m_clMenuDesableColor
			End If
		Next
	End Sub


End Class
