'*******************************************************************************
'  �|�C���^���N���X
'
' [Ver]
'      Ver.01  2013/12/20  ����
'
' [����]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI.KizuMod

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

''' <summary>
''' �|�C���^���N���X
''' </summary>
''' <remarks></remarks>
Public Class clsPointInf
    Implements IDisposable
    '///////////////////////////////////////////////////////////////////////////
    '�񋓑�
    '///////////////////////////////////////////////////////////////////////////
    '�Q�ƃ��R�[�h�ʒu
    Public Enum EM_DIV_RECORD
        DIV_RECORD_WRITE = 1                                '�����݃��R�[�h
        DIV_RECORD_SOUCHI                                   '�e�������u�P�ʂł̌��݃��R�[�h
        DIV_RECORD_BASE                                     '��_�ʒu�ł̌��ݕ\�����R�[�h
        DIV_RECORD_KENSA                                    '�e������ʒu�ł̌��ݕ\�����R�[�h
    End Enum

    '///////////////////////////////////////////////////////////////////////////
    '�����o�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                                    '���\�[�X�J���t���O[True:�J����,False:���J��]

    Private mcls_Log As tClass.tClass_LogManager                                '���O�Ǘ�

    Private m_typHyPoint As TBL_HY_POINT                                        '�|�C���^���
	Private mcls_DispCoil As clsDispCoil										'�R�C�����\��

    '///////////////////////////////////////////////////////////////////////////
    '�v���p�e�B
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �����݃��R�[�h
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecWriteBase() As Integer
        Get
            Return m_typHyPoint.nRecWriteBase
        End Get
    End Property

    ''' <summary>
    ''' �e�������u�P�ʂł̌��݃��R�[�h
    ''' </summary>
    ''' <param name="nMen">��[0:�\,1:��]</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecWrite(ByVal nMen As Integer) As Integer
        Get
            Dim nRecWrite As Integer = -1

            If nMen >= 0 And nMen < NUM_MEN Then
                nRecWrite = m_typHyPoint.nRecWrite(nMen)
            End If

            Return nRecWrite
        End Get
    End Property

    ''' <summary>
    ''' �\����_�ʒu�ł̌��ݕ\�����R�[�h
    ''' </summary>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecDispBase() As Integer
        Get
            Return m_typHyPoint.nRecDispBase
        End Get
    End Property

    ''' <summary>
    ''' �e������ʒu�ł̌��ݕ\�����R�[�h
    ''' </summary>
    ''' <param name="nKensa">������ʒu</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecDisp(ByVal nKensa As Integer) As Integer
        Get
            Dim nRecDisp As Integer = -1

            If nKensa >= 0 And nKensa < MAX_DISP_POS Then
                nRecDisp = m_typHyPoint.nRecDisp(nKensa)
            End If

            Return nRecDisp
        End Get
    End Property

    ''' <summary>
    ''' �e������ʒu�ł̌��ݕ\���R�C�����R�[�h
    ''' </summary>
    ''' <param name="nKensa">������ʒu</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RecCoilDisp(ByVal nKensa As Integer) As Integer
        Get
            Dim nRecDisp As Integer = -1

            'If nKensa >= 0 And nKensa < MAX_DISP_POS Then
            '    nRecDisp = m_typHyPoint.nRecCoilDisp(nKensa)
            'End If

            Return nRecDisp
        End Get
    End Property

    '''' <summary>
    '''' �n�ړ_�c��
    '''' </summary>
    '''' <param name="nKensa">������ʒu</param>
    '''' <value></value>
    '''' <returns></returns>
    '''' <remarks></remarks>
    'Public ReadOnly Property LenZan(ByVal nKensa As Integer) As Integer
    '    Get
    '        Dim nLenZan As Integer = -1

    '        If nKensa >= 0 And nKensa < MAX_DISP_POS Then
    '            nLenZan = m_typHyPoint.inf(nKensa).nLenZan
    '        End If

    '        Return nLenZan
    '    End Get
    'End Property

    ''' <summary>
    ''' �Ǘ�No
    ''' </summary>
    ''' <param name="nRec">���R�[�h</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property KizukenNo(ByVal nRec As Integer) As String
        Get
            Dim strKizukenNo As String = Nothing

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                strKizukenNo = tMod_Common.ByteToString(m_typHyPoint.pnt(nRec - 1).cKizukenNo)
            End If

            Return strKizukenNo
        End Get
    End Property

	''' <summary>
	''' �r���Ǘ��ԍ��Ŏw�肳�ꂽ���R�[�h�ԍ��擾
	''' </summary>
	''' <param name="sKizukenNo"></param>
	''' <value></value>
	''' <returns>���݂��Ȃ�:0 �Y�����R�[�h�ԍ�:1�`TNL_HY_REC_MAX </returns>
	''' <remarks></remarks>
	Public ReadOnly Property recNo(ByVal sKizukenNo As String) As Integer
		Get
			Dim nRec As Integer = 0
			For ii As Integer = 1 To TBL_HY_REC_MAX
				If sKizukenNo = tMod.ByteToString(m_typHyPoint.pnt(ii - 1).cKizukenNo) Then
					nRec = ii
				End If
			Next
			Return nRec
		End Get
	End Property

    ''' <summary>
    ''' ��ʕ\�����
    ''' </summary>
    ''' <param name="nRec">���R�[�h</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property DispStatus(ByVal nRec As Integer) As EM_DIV_DSP_STATUS
        Get
            Dim emDispStatus As EM_DIV_DSP_STATUS = Nothing

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                emDispStatus = m_typHyPoint.pnt(nRec - 1).emDispStatus
            End If

            Return emDispStatus
        End Get
    End Property


    '''' <summary>
    '''' �e�ʔ|�C���g�̒ʔ���
    '''' </summary>
    '''' <param name="nRec">���R�[�h</param>
    '''' <param name="nKensa">�ʔ|�C���g</param>
    '''' <value></value>
    '''' <returns></returns>
    '''' <remarks></remarks>
    'Public ReadOnly Property LenDisp(ByVal nRec As Integer, ByVal nKensa As Integer) As Integer
    '    Get
    '        Dim nLenDisp As Integer = -1

    '        If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
    '            If nKensa >= 0 And nKensa < MAX_DISP_POS Then
    '                nLenDisp = m_typHyPoint.pnt(nRec - 1).typ(0).nLenDisp(nKensa)
    '            End If
    '        End If

    '        Return nLenDisp
    '    End Get
    'End Property


    ''' <summary>
    ''' �e�ʔ|�C���g�ł̌��ݕ\���s
    ''' </summary>
    ''' <param name="nRec">���R�[�h</param>
    ''' <param name="nKensa">������ʒu</param>
    ''' <param name="nStage">�}�b�v�ؑ�[0:�ŏ�,1:�\����,2:�\����]</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RowDisp(ByVal nRec As Integer, ByVal nKensa As Integer, ByVal nStage As Integer) As Integer
        Get
            Dim nRowDisp As Integer = -1

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                If nKensa >= 0 And nKensa < MAX_DISP_POS Then
                    If nStage >= 0 And nStage < MAP_STAGE_NUM Then
                        nRowDisp = m_typHyPoint.pnt(nRec - 1).typ.nRowDisp(nKensa).nStage(nStage)
                    End If
                End If
            End If

            Return nRowDisp
        End Get
    End Property

    ''' <summary>
    ''' ���ݏ������ݍs
    ''' </summary>
    ''' <param name="nRec">���R�[�h</param>
    ''' <param name="nMen">��[0:�\,1:��]</param>
    ''' <param name="nStage">�}�b�v�ؑ�[0:�ŏ�,1:�\����,2:�\����]</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RowWrite(ByVal nRec As Integer, ByVal nMen As Integer, ByVal nStage As Integer) As Integer
        Get
            Dim nRowWrite As Integer = -1

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                If nMen >= 0 And nMen < NUM_MEN Then
                    If nStage >= 0 And nStage < MAP_STAGE_NUM Then
                        nRowWrite = m_typHyPoint.pnt(nRec - 1).typ.nRowWrite(nMen).nStage(nStage)
                    End If
                End If
            End If

            Return nRowWrite
        End Get
    End Property


    ''' <summary>
    ''' ������������R�C�����ɕς������߁A�R�C�����̂d�m�c�s��������ɕύX
    ''' </summary>
    ''' <param name="nRec">���R�[�h</param>
    ''' <param name="nStage">�}�b�v�ؑ�[0:�ŏ�,1:�\����,2:�\����]</param>
    ''' <value></value>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property RowLast(ByVal nRec As Integer, ByVal nStage As Integer) As Integer
        Get
            Dim nRowLast As Integer = -1

            If nRec > 0 And nRec <= TBL_HY_REC_MAX Then
                If nStage >= 0 And nStage < MAP_STAGE_NUM Then
                    nRowLast = m_typHyPoint.pnt(nRec - 1).typ.nRowLast(nStage)
                End If
            End If

            Return nRowLast
        End Get
    End Property


    ''' <summary>
    ''' �X�V���t���O
    ''' </summary>
    ''' <value></value>
    ''' <returns>TRUE:�Q�Ƌ֎~�AFALSE:�Q�Ɖ�</returns>
    ''' <remarks></remarks>
    Public ReadOnly Property Refresh() As Boolean
        Get
            Return CBool(m_typHyPoint.nRefresh)
        End Get
    End Property

    '///////////////////////////////////////////////////////////////////////////
    '�X�^�e�B�b�N�֐�
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �����R�[�h�擾
    ''' </summary>
    ''' <param name="nRecNo">���R�[�h</param>
    ''' <returns>�����R�[�h</returns>
    ''' <remarks></remarks>
    Public Shared Function GetNextRecNo(ByVal nRecNo As Integer) As Integer
        nRecNo += 1

        If nRecNo < 1 Then                                  '�s���l
            nRecNo = 1                                      '�擪
        End If

        If TBL_HY_REC_MAX < nRecNo Then                     '�ő�l�I�[�o�[
            nRecNo = 1                                      '�擪
        End If

        Return nRecNo
    End Function

    ''' <summary>
    ''' �O���R�[�h�擾
    ''' </summary>
    ''' <param name="nRecNo">���R�[�h</param>
    ''' <returns>�O���R�[�h</returns>
    ''' <remarks></remarks>
    Public Shared Function GetPrevRecNo(ByVal nRecNo As Integer) As Integer
        nRecNo -= 1

        If nRecNo < 1 Then                                  '�ŏ��l�A���_�[
            nRecNo = TBL_HY_REC_MAX                         '����
        End If

        If TBL_HY_REC_MAX < nRecNo Then                     '�s���l
            nRecNo = TBL_HY_REC_MAX                         '����
        End If

        Return nRecNo
    End Function

    '///////////////////////////////////////////////////////////////////////////
    '�����o�֐�
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    ''' <param name="clsLog"></param>
    ''' <remarks></remarks>
    Public Sub New(ByVal clsLog As tClass_LogManager)
        '������
        mcls_Log = clsLog                                                       '���O�Ǘ�

        m_typHyPoint = Nothing                                                  '�|�C���^���
        m_typHyPoint.initialize()
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
                m_typHyPoint = Nothing
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
    ''' �|�C���^���ǂݍ���
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub ReadHyPoint()
        Dim bRet As Boolean = False

        Try
            '�|�C���^���ǂݍ���
            m_typHyPoint = CType(ksMemRead(TBL_HY_POINT_NAME, GetType(TBL_HY_POINT)), TBL_HY_POINT)
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]�ǂݍ��ُ݈�[{1}]", TBL_HY_POINT_NAME, ex.Message))
        End Try
    End Sub
End Class
