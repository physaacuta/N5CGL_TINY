'*********************************************************************************
'������\�p�t�H�[�}���X�J�E���^�ł̎�������
'	[Ver]
'		Ver.01    2005/12/23  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.Runtime.InteropServices			' DllImport

Namespace tClass
    ''' <summary>
    ''' ������\�p�t�H�[�}���X�J�E���^�ł̎�������
    ''' </summary>
    ''' <remarks>�n�[�h�̑Ή����K�v</remarks>
    Public Class tClass_PerformanceCounter
        '********************************************************************
        'API��`
        '********************************************************************
        ''' <summary>
        ''' ������\�p�t�H�[�}���X�J�E���^�l�擾
        ''' </summary>
        ''' <param name="lpPerformanceCount">�J�E���^�l</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        <DllImport("KERNEL32.DLL", EntryPoint:="QueryPerformanceCounter")> _
        Private Shared Function QueryPerformanceCounter( _
            ByRef lpPerformanceCount As Int64) _
            As Boolean
        End Function
        ''' <summary>
        ''' ���݂̎��g��
        ''' </summary>
        ''' <param name="lpPerformanceCount">���g��</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        <DllImport("KERNEL32.DLL", EntryPoint:="QueryPerformanceFrequency")> _
        Private Shared Function QueryPerformanceFrequency( _
            ByRef lpPerformanceCount As Int64) _
            As Boolean
        End Function

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private m_pf As Int64				' ���g��
        Private m_pc As Int64				' ���낢��g������_

        '' ����
        Private m_min As Double				' �ŏ��l	[ms]
        Private m_max As Double				' �ő�l	[ms]
        Private m_ave As Double				' ���ώ���	[ms]
        Private m_bFast As Boolean			' �����s�� True
        Private m_Cycle As Double			' �Ԋu [ms]

        '********************************************************************
        '�v���p�e�B
        '********************************************************************
        ''' <summary>
        ''' ���ώ��� [ms] 
        ''' </summary>
        ''' <returns>���ώ��� [ms] </returns>
        Public ReadOnly Property GetAveTime() As Double
            Get
                Return m_ave
            End Get
        End Property

        ''' <summary>
        ''' �Ԋu [ms]
        ''' </summary>
        ''' <returns>�Ԋu [ms]</returns>
        Public ReadOnly Property GetCycleTime() As Double
            Get
                Return m_Cycle
            End Get
        End Property

        ''' <summary>
        ''' �ŏ����� [ms]
        ''' </summary>
        ''' <returns>�Ԋu [ms]</returns>
        Public ReadOnly Property getMinTime() As Double
            Get
                Return m_min
            End Get
        End Property

        ''' <summary>
        ''' �ő厞�� [ms]
        ''' </summary>
        ''' <returns>�Ԋu [ms]</returns>
        Public ReadOnly Property getMaxTime() As Double
            Get
                Return m_max
            End Get
        End Property


        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �f�t�H���g �R���X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub New()
            '' ���g���擾
            QueryPerformanceFrequency(m_pf)
            m_pc = 0
            '' ���ʗp
            m_min = 9999.9			' �ŏ��l	[ms]
            m_max = 0				' �ő�l	[ms]
            m_ave = 0				' ���ώ���	[ms]
            m_bFast = False			' �����s�� True
            m_Cycle = 0
        End Sub

        ''' <summary>
        ''' 2�_�Ԏ��ԑ��菉����
        ''' </summary>
        ''' <param name="intervalCnt">���ώZ�o�Ԋu</param>
        ''' <remarks></remarks>
        Public Sub initAve(ByVal intervalCnt As Integer)
            m_pc = 0

            m_ave = 0.0								'// ���ώ���	[ms] 
            m_min = 9999.0							'// �ŏ��l
            m_max = 0.0								'// �ő�l
            m_bFast = False
            m_Cycle = 0
        End Sub

        ''' <summary>
        ''' 2�_�Ԏ��ԑ���J�n
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub CounterStart()
            QueryPerformanceCounter(m_pc)
        End Sub

        ''' <summary>
        ''' 2�_�Ԏ��ԑ��芮��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub CounterEnd()
            '' ���ԍ��Z�o
            Dim pc2 As Int64
            QueryPerformanceCounter(pc2)
            m_Cycle = (pc2 - m_pc) / m_pf * 1000.0		' [s] �� [ms] 

            ' �ő�ŏ����ԎZ�o
            If m_min > m_Cycle Then m_min = m_Cycle
            If m_max < m_Cycle Then m_max = m_Cycle

            ' ���ώ��ԎZ�o (�ȈՎZ�o)
            If m_bFast Then
                m_ave = m_ave * 0.8 + m_Cycle * 0.2
            Else
                m_ave = m_Cycle
                m_bFast = True
            End If
        End Sub

        ''' <summary>
        ''' �Ԋu���菉����
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub initCycle()
            m_pc = 0
            m_Cycle = 0.0								'// �Ԋu [ms]

            m_ave = 0.0								'// ���ώ���	[ms] 
            m_min = 9999.0								'// �ŏ��l
            m_max = 0.0								'// �ő�l
            m_bFast = False
        End Sub

        ''' <summary>
        ''' �Ԋu����
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub CounterCycle()
            '' ���ԍ��Z�o
            Dim pc2 As Int64
            QueryPerformanceCounter(pc2)
            m_Cycle = (pc2 - m_pc) / m_pf * 1000.0		' [s] �� [ms] 
            m_pc = pc2

            ' �ő�ŏ����ԎZ�o
            If m_min > m_Cycle Then m_min = m_Cycle
            If m_max < m_Cycle Then m_max = m_Cycle

            ' ���ώ��ԎZ�o (�ȈՎZ�o)
            If m_bFast Then
                m_ave = m_ave * 0.8 + m_Cycle * 0.2
            Else
                m_ave = m_Cycle
                m_bFast = True
            End If
        End Sub
    End Class
End Namespace