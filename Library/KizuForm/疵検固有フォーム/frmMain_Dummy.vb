'*********************************************************************************
' �}���`���j�^���̃_�~�[��ʏo�̓N���X
'	[Ver]
'		Ver.01    2007/03/31  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On



''' <summary>
''' �_�~�[�t�H�[�� �i���ڎQ�Ƌ֎~�j
''' </summary>
''' <remarks></remarks>
Friend Class frmMain_Dummy


End Class


''' <summary>
''' �_�~�[�t�H�[���Ǘ�
''' </summary>
''' <remarks></remarks>
Public Class frmMain_DummyManager

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �����o �ϐ�
	'/ /////////////////////////////////////////////////////////////////////////////////
	Private m_nNotMonitorNo() As Integer									' �\���ΏۊO�̃��j�^�[No (1�I���W��) (����ȊO�̑S���j�^�[�ɖ{��ʂ�\������)
	Private mfrm_Dummy() As frmMain_Dummy									' �z���̃_�~�[�t�H�[�� (2��ʈȏ�Ń_�~�[�t�H�[����\������ꍇ�ɗL��)


	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���J ���\�b�h
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ����ʂ��w�胂�j�^�ʒu�ɕ\��
	''' </summary>
	''' <param name="nNotMonitorNo">�\���ΏۊO�̃��j�^�[No (1�I���W��)</param>
	''' <remarks></remarks>
	Public Overloads Sub Show(ByVal nNotMonitorNo As Integer)
		'''' �����o�[�ϐ��Z�b�g
		ReDim m_nNotMonitorNo(0)
		m_nNotMonitorNo(0) = nNotMonitorNo

		' ��ʕ\�����C������
		Call DspForm()
	End Sub
	''' <summary>
	''' ����ʂ��w�胂�j�^�ʒu�ɕ\��
	''' </summary>
	''' <param name="nNotMonitorNo">�\���ΏۊO�̃��j�^�[No (1�I���W��) </param>
	''' <remarks>�����w�莞�́A�uDim a As New KizuForm.frmMain_Dummy(New Integer() {1, 1})�v �ł�������</remarks>
	Public Overloads Sub Show(ByVal nNotMonitorNo() As Integer)
		'''' �����o�[�ϐ��Z�b�g
		'm_nNotMonitorNo = nNotMonitorNo		�Q�ƂɂȂ邩�烄��
		ReDim m_nNotMonitorNo(nNotMonitorNo.Length - 1)
		For ii As Integer = 0 To nNotMonitorNo.Length - 1
			m_nNotMonitorNo(ii) = nNotMonitorNo(ii)
		Next ii

		' ��ʕ\�����C������
		Call DspForm()
	End Sub


	''' <summary>
	''' �S��ʂ����
	''' </summary>
	''' <remarks></remarks>
	Public Sub Close()
		If mfrm_Dummy Is Nothing Then Return

		'' �S��ʕ���
		For ii As Integer = 0 To mfrm_Dummy.Length - 1
			mfrm_Dummy(ii).Close()
			mfrm_Dummy(ii).Dispose()
			mfrm_Dummy(ii) = Nothing
		Next ii
		mfrm_Dummy = Nothing
	End Sub



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ����
	'/ /////////////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' ��ʕ\�����C������
	''' </summary>
	''' <remarks></remarks>
	Private Sub DspForm()
		Dim ii, jj As Integer

		'' �\�����������j�^�̏��
		Dim monitor() As System.Windows.Forms.Screen
		monitor = System.Windows.Forms.Screen.AllScreens()

		Dim nDummyFromCnt As Integer					' �_�~�[�t�H�[����
		nDummyFromCnt = monitor.Length - m_nNotMonitorNo.Length
		If nDummyFromCnt <= 0 Then Return ' �\���_�~�[�t�H�[������

		'' �_�~�[�t�H�[���𐶐�
		ReDim mfrm_Dummy(nDummyFromCnt - 1)
		For ii = 0 To nDummyFromCnt - 1
			mfrm_Dummy(ii) = New frmMain_Dummy
		Next ii


		'' �_�~�[�t�H�[����\��
		Dim bDspFlg As Boolean
		Dim nCnt As Integer = 0
		For ii = 0 To monitor.Length - 1
			' �\���Ώۈʒu���`�F�b�N
			bDspFlg = True
			For jj = 0 To m_nNotMonitorNo.Length - 1
				If m_nNotMonitorNo(jj) = ii + 1 Then
					bDspFlg = False
					Exit For
				End If
			Next jj
			If Not bDspFlg Then Continue For

			' �\��
			mfrm_Dummy(nCnt).Show(ii + 1, True)
			nCnt += 1
		Next ii
	End Sub


End Class
