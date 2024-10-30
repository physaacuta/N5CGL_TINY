'*******************************************************************************
'   �X�e�[�^�X���\��
'
'   [Ver]
'       Ver.01  2013/12/19  ����
'
'   [����]
'       tMod_MemSet.MemSet�֐����g�p���Ă��邽��
'       [Option Strict Off]�Ƃ���(�G���[���)
'*******************************************************************************
Option Strict On

Imports KizuLibCLI.KizuMod

Imports KizuForm

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod


Imports System.Runtime.InteropServices

''' <summary>
''' �X�e�[�^�X���\��
''' </summary>
''' <remarks></remarks>
Public Class clsDispStatus
    Implements IDisposable
    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////
    '�X�e�[�^�X���\���R���g���[��
    Public Structure TYP_STATUS_DSP
        Dim lblGaidans As Label                             '�K�C�_���X���b�Z�[�W
        Dim lblKadou As Label                               '�ғ����
        Dim lblKiki As Label                                '�@����
        Dim lblKensa As Label                               '�������
    End Structure


    '�_�ŊǗ��p�\����
    Private Structure BLINK
		Dim cnt As Control
		Dim text As String
		Dim colorOrg As Color
		Public Sub New(ByVal cnt As Control, ByVal text As String, ByVal clr1 As Color)
			Me.cnt = cnt
			Me.text = text
			Me.colorOrg = clr1
		End Sub
	End Structure


    '///////////////////////////////////////////////////////////////////////////
    '�����o�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                        '���\�[�X�J���t���O[True:�J����,False:���J��]

    Private mcls_Log As tClass_LogManager                           '���O�Ǘ�

    Private m_typStatusDsp As TYP_STATUS_DSP                        '�X�e�[�^�X�\��

    Private m_bDBError As Boolean = False                           'DB�ڑ��ُ�[True:�ُ�,False:����]

    Private WithEvents m_tmBlk As New Windows.Forms.Timer           'BLINK�p�^�C�}�[
	Private lstBlk As New List(Of BLINK)
	Private m_blkStatus As Boolean

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	' �������K�C�_���X�\��
	Private m_strLongPdGuidanceMessage As String = ""               '���b�Z�[�W���e
	Private m_nLongPdGuidanceBackColor As Integer                   '�w�i�F
	Private m_nLongPdGuidanceForeColor As Integer                   '�����F
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	'///////////////////////////////////////////////////////////////////////////
	'�v���p�e�B
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' DB�ڑ����
	''' </summary>
	''' <returns>True:�ڑ� False:�ؒf</returns>
	''' <remarks></remarks>
	Public Property DbStatus() As Boolean
        Get
            Return Not m_bDBError
        End Get
        Set(ByVal value As Boolean)
            m_bDBError = Not value
        End Set
	End Property

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	''' <summary>
	''' �������K�C�_���X�\�� ���b�Z�[�W���e
	''' </summary>
	Public WriteOnly Property LongPdGuidanceMessage() As String
		Set(value As String)
			m_strLongPdGuidanceMessage = value
		End Set
	End Property

	''' <summary>
	''' �������K�C�_���X�\�� �w�i�F
	''' </summary>
	Public WriteOnly Property LongPdGuidanceBackColor() As Integer
		Set(value As Integer)
			m_nLongPdGuidanceBackColor = value
		End Set
	End Property

	''' <summary>
	''' �������K�C�_���X�\�� �����F
	''' </summary>
	Public WriteOnly Property LongPdGuidanceForeColor() As Integer
		Set(value As Integer)
			m_nLongPdGuidanceForeColor = value
		End Set
	End Property
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------


	'///////////////////////////////////////////////////////////////////////////
	'�����o�֐�
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �R���X�g���N�^
	''' </summary>
	''' <param name="typStatusDsp">�X�e�[�^�X�\��</param>
	''' <param name="clsLog">���O�Ǘ�</param>
	''' <remarks></remarks>
	Public Sub New(ByRef typStatusDsp As TYP_STATUS_DSP, ByRef clsLog As tClass_LogManager)
        Try
            mcls_Log = clsLog
			m_typStatusDsp = typStatusDsp

			' �_�ŕ\��������
			InitBlink()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�e�[�^�X���\�������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �f�X�g���N�^
    ''' </summary>
    ''' <param name="disposing">True:�����I�ȌĂяo�� False:�Öق̌Ăяo��</param>
    ''' <remarks></remarks>
    Protected Overridable Sub Dispose(ByVal disposing As Boolean)
        Try
            If Not m_bIsDesposed Then
                '�I�u�W�F�N�g�j��
            End If
            m_bIsDesposed = True                                                'Dispose�ς݂Ƃ���
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�u�W�F�N�g�j���ُ�[{0}]", ex.Message))
        End Try
    End Sub
#Region " IDisposable Support "
    '���̃R�[�h�́A�j���\�ȃp�^�[���𐳂��������ł���悤�� Visual Basic �ɂ���Ēǉ�����܂����B
    Public Sub Dispose() Implements IDisposable.Dispose
        '���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
        Dispose(True)
        GC.SuppressFinalize(Me)
    End Sub
#End Region

    ''' <summary>
    ''' �\��������
    ''' </summary>
    ''' <remarks></remarks>
	Public Sub DspInit()
		m_tmBlk.Interval = 2000

		'�K�C�_���X���\��
		ucStatusAria.InitLabel(m_typStatusDsp.lblGaidans)
		'�ғ���ԕ\��
		ucStatusAria.InitLabel(m_typStatusDsp.lblKadou)
		'������ԕ\��
		ucStatusAria.InitLabel(m_typStatusDsp.lblKensa)
		'�@���ԕ\��
        ucStatusAria.InitLabel(m_typStatusDsp.lblKiki)

    End Sub

    ''' <summary>
    ''' �X�e�[�^�X�\��
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub DspStatus(ByVal nMen As Integer)
        Dim typHyStatus As TBL_HY_STATUS = Nothing
        Dim typNowSystem As KizuForm.TYP_STATUS_NOWSYSTEM = Nothing

        Try
            '������
            typHyStatus.status.initialize()
            typNowSystem.initialize()

            '�X�e�[�^�X���ǂݍ���[���L������]
            typHyStatus = CType(ksMemRead(TBL_HY_STATUS_NAME, GetType(TBL_HY_STATUS)), TBL_HY_STATUS)

            '�X�e�[�^�X�R�s�[
            Dim p As IntPtr
			Try
				p = Marshal.AllocHGlobal(SizeOf(typHyStatus.status))					'�������擾
				Marshal.StructureToPtr(typHyStatus.status, p, True)					'��������Ƀf�[�^���Z�b�g
				typNowSystem = DirectCast(Marshal.PtrToStructure(p, typNowSystem.GetType), KizuForm.KizuCommon.TYP_STATUS_NOWSYSTEM)		'����������f�[�^���擾���A�\���̂փZ�b�g
			Catch ex As Exception
				typNowSystem = Nothing
			Finally
				Marshal.FreeHGlobal(p)									'�������J��
			End Try

            'DB�ڑ��ُ�
            If m_bDBError Then
                typNowSystem.nCnt = 1
                typNowSystem.typErr(0).msg = "�f�[�^�x�[�X�ڑ��ُ킪�����������ߌ����ł��܂���"
                typNowSystem.st_kensa(nMen) = CType(EM_DIV_KENSA.DIV_KENSA_NG, KizuForm.EM_DIV_KENSA)
                typNowSystem.st_kiki(nMen) = CType(EM_DIV_KIKI.DIV_KIKI_JYU, KizuForm.EM_DIV_KIKI)
            End If

            '�K�C�_���X�\��
            ucStatusAria.DspGaidans(m_typStatusDsp.lblGaidans, nMen, typNowSystem)

			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
			' �������K�C�_���X�\��
			Call DspLongPdGuidance(typNowSystem)
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

			'�ғ����
			ucStatusAria.DspKadou(m_typStatusDsp.lblKadou, CType(typHyStatus.emKadou, KizuForm.EM_DIV_KADOU), CType(typHyStatus.emUnten, KizuForm.EM_DIV_UNTEN))

            '�@����
            ucStatusAria.DspKiki(m_typStatusDsp.lblKiki, typNowSystem.st_kiki(nMen))

            '�������
            ucStatusAria.DspKensa(m_typStatusDsp.lblKensa, typNowSystem.st_kensa(nMen))

            ' �_�ŏ��Z�b�g
            For i As Integer = 0 To lstBlk.Count - 1
                With lstBlk(i)
                    If .text = .cnt.Text Then
                        lstBlk(i) = New BLINK(.cnt, .text, .cnt.BackColor)
                    End If
                End With
            Next
            DispBlink()

        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�X�e�[�^�X�\���ُ�[{0}]", ex.Message))
        End Try
	End Sub

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	''' <summary>
	'''  �������K�C�_���X�\��
	''' </summary>
	Private Sub DspLongPdGuidance(ByVal typNowSystem As KizuForm.TYP_STATUS_NOWSYSTEM)

		Dim bError As Boolean = False
		If typNowSystem.nCnt > 0 Then
			bError = True
		End If

		If True = bError Then
			' �����ُ탁�b�Z�[�W���\�����Ȃ當���F�����ɖ߂��ďI���B
			m_typStatusDsp.lblGaidans.ForeColor = Color.Black
			Exit Sub
		End If

		'���L�������Ǎ�
		Dim typLongPd As TBL_HY_LONGPD = Nothing
		typLongPd.initialize()
		typLongPd = CType(ksMemRead(TBL_HY_LONGPD_NAME, GetType(TBL_HY_LONGPD)), TBL_HY_LONGPD)

		If 1 = typLongPd.nGuidanceFlag Then
			m_typStatusDsp.lblGaidans.Text = m_strLongPdGuidanceMessage
			m_typStatusDsp.lblGaidans.BackColor = Color.FromArgb(m_nLongPdGuidanceBackColor)
			m_typStatusDsp.lblGaidans.ForeColor = Color.FromArgb(m_nLongPdGuidanceForeColor)
		Else
			m_typStatusDsp.lblGaidans.Text = ""
			m_typStatusDsp.lblGaidans.BackColor = g_ColorControl
			m_typStatusDsp.lblGaidans.ForeColor = Color.Black
		End If
	End Sub
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	''' <summary>
	''' �_�ŕ\��������
	''' </summary>
	''' <remarks></remarks>
	Private Sub InitBlink()
		lstBlk.Clear()

		lstBlk.Add(New BLINK(m_typStatusDsp.lblKiki, "�d�̏�", g_ColorControl))
		lstBlk.Add(New BLINK(m_typStatusDsp.lblKiki, "�y�̏�", g_ColorControl))

		lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "������", g_ColorControl))
		lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "���ۑ�", g_ColorControl))
		lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "��Ò�", g_ColorControl))
        lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "�k��", g_ColorControl))
		lstBlk.Add(New BLINK(m_typStatusDsp.lblKensa, "���ޕs��", g_ColorControl))

		m_tmBlk.Interval = 2000
		m_tmBlk.Start()

	End Sub


	''' <summary>
	''' �_�ŕ\��
	''' </summary>
	''' <remarks></remarks>
	Private Sub DispBlink()

		If m_blkStatus Then
			' ������
			m_blkStatus = False
			For Each blk As BLINK In lstBlk
				If blk.cnt.Text = blk.text Then
					blk.cnt.BackColor = g_ColorControl
					blk.cnt.ForeColor = Color.Black
				End If
			Next
		Else
			' �_����
			m_blkStatus = True
			For Each blk As BLINK In lstBlk
				If blk.cnt.Text = blk.text Then
					blk.cnt.BackColor = blk.colorOrg
					blk.cnt.ForeColor = Color.Yellow
				End If
			Next
		End If
	End Sub


	''' <summary>
	''' �\���̓_�ŗp�^�C�}�[
	''' </summary>
	''' <param name="sender"></param>
	''' <param name="e"></param>
	''' <remarks></remarks>
	Private Sub m_tmBlk_tick(ByVal sender As System.Object, ByVal e As EventArgs) Handles m_tmBlk.Tick
		' �_�ŏ�Ԃ𔽓]������
		m_blkStatus = Not m_blkStatus

		' �_�ŕ\�����s��
		DispBlink()
	End Sub

End Class
