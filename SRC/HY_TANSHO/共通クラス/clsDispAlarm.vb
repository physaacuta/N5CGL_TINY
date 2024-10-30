'*******************************************************************************
'   �r�ڋߌx���\��
'
'   [Ver]
'       Ver.01  2013/12/20  ����
'
'	[����]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

''' <summary>
''' �r�ڋߌx���\��
''' </summary>
''' <remarks></remarks>
Public Class clsDispAlarm
    Implements IDisposable
    '///////////////////////////////////////////////////////////////////////////
    '�\����
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �A���[�����\���R���g���[��
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_ALARM_DSP
        '���x���͍������珇�Ɋi�[����
        Dim lblLamp As Label()                                                  '�r�ڋ߃����v
        '�C�j�V�����C�Y
        Public Sub initialize()
			ReDim lblLamp(ALARM_COL_NUM - 1)
        End Sub
    End Structure

    '///////////////////////////////////////////////////////////////////////////
    '�����o�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                                    '���\�[�X�J���t���O[True:�J����,False:���J��]

    Private mcls_Log As tClass_LogManager                                       '���O�Ǘ�

    Private m_typAlarmDsp As TYP_ALARM_DSP                                      '�A���[�����\���R���g���[��

    Private m_ColorArmKei As Color                                              '�y�̏჉���v�\���F
    Private m_ColorArmJyu As Color                                              '�d�̏჉���v�\���F

    '///////////////////////////////////////////////////////////////////////////
    '�v���p�e�B
    '///////////////////////////////////////////////////////////////////////////
#Region "�v���p�e�B"

    ''' <summary>
    ''' �y�̏჉���v�\���F
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property ColorArmKei() As Color
        Get
            Return m_ColorArmKei
        End Get
        Set(ByVal value As Color)
            m_ColorArmKei = value
        End Set
    End Property

    ''' <summary>
    ''' �d�̏჉���v�\���F
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Property ColorArmJyu() As Color
        Get
            Return m_ColorArmJyu
        End Get
        Set(ByVal value As Color)
            m_ColorArmJyu = value
        End Set
    End Property

#End Region

    '///////////////////////////////////////////////////////////////////////////
    '�����o�֐�
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="typAlarmDsp">�A���[�����\���R���g���[��</param>
    ''' <param name="clsLog">���O�Ǘ�</param>
    ''' <remarks></remarks>
    Public Sub New(ByVal typAlarmDsp As TYP_ALARM_DSP, ByRef clsLog As tClass_LogManager)
        Try
            m_typAlarmDsp = typAlarmDsp
            mcls_Log = clsLog
            m_ColorArmKei = Color.LimeGreen
            m_ColorArmJyu = Color.Red
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�ڋߌx���\�������ُ�[{0}]", ex.Message))
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
    ''' �r�ڋ߃����v�\��
    ''' </summary>
    ''' <param name="nDspPos">�o�͈ʒu</param>
    ''' <param name="nMen">��[0:�\,1:��]</param>
    ''' <param name="bDSPos">DS�ʒu[True:��,False:�E]</param>
    ''' <param name="lblLamp">�\�������v</param>
    ''' <remarks></remarks>
    Public Sub DspDefLamp(ByVal nDspPos As Integer, ByVal nMen As Integer, ByVal bDSPos As Boolean, Optional ByVal lblLamp() As Label = Nothing)
        Dim lstLamp As List(Of Label) = Nothing
        Dim typHyAlarm As TBL_HY_ALARM = Nothing
        Dim nCol As Integer

        Try
            '�x��o�͒����ǂݍ���[���L������]
            typHyAlarm = CType(ksMemRead(TBL_HY_ALARM_NAME, GetType(TBL_HY_ALARM)), TBL_HY_ALARM)

            '�\�������v�ݒ�
            lstLamp = New List(Of Label)
            If lblLamp Is Nothing Then
                For ii As Integer = 0 To m_typAlarmDsp.lblLamp.Length - 1
                    lstLamp.Add(m_typAlarmDsp.lblLamp(ii))
                Next
            Else
                For ii As Integer = 0 To lblLamp.Length - 1
                    lstLamp.Add(lblLamp(ii))
                Next
            End If

            '�r�ڋ߃����v�\��
			For ii As Integer = 0 To ALARM_COL_NUM - 1
				If bDSPos Then			'������
					nCol = ii
				Else					'�E����
					nCol = (ALARM_COL_NUM - ii) - 1
				End If
				lstLamp(ii).Text = ""
                lstLamp(ii).BackColor = Color.White
                If 0 < typHyAlarm.a_mode.kendai(nDspPos).torb(nMen).row(nCol) Then
                    If 2 = typHyAlarm.a_mode.kendai(nDspPos).torb(nMen).row(nCol) Then
                        '�d�r
                        lstLamp(ii).BackColor = m_ColorArmJyu
                    ElseIf 1 = typHyAlarm.a_mode.kendai(nDspPos).torb(nMen).row(nCol) Then
                        '�y�r
                        lstLamp(ii).BackColor = m_ColorArmKei
                    End If
                End If
            Next
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�r�ڋ߃����v�\���ُ�[{0}]", ex.Message))
        Finally
            If Not lstLamp Is Nothing Then
                lstLamp.Clear()
                lstLamp = Nothing
            End If
        End Try
    End Sub
End Class
