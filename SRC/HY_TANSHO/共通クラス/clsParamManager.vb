'*******************************************************************************
'   �p�����[�^�Ǘ�
'
'   [Ver]
'       Ver.01  2017/08/31  ����
'
'   [����]
'
'*******************************************************************************
Option Strict On

Imports KizuLibCLI

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod
Imports System.Runtime.InteropServices

''' <summary>
''' �p�����[�^�Ǘ�
''' </summary>
''' <remarks></remarks>
Public Class clsParamManager
    Implements IDisposable
    '/////////////////////////////////////////////////////////////////////////////////
    '�\����
    '/////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �r���萔�}�X�^�[
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_COMMON_MASTER
        Dim nMapMin As Integer                              '�T����ʕ\����ԏ�
        Dim nMapMax As Integer                              '�T����ʕ\����ԑ�
        Dim nCntMapColorH As Integer                        '�d���r�}�b�v�w�i�F_�d
        Dim nCntMapColorM As Integer                        '�d���r�}�b�v�w�i�F_��
        Dim nCntMapColorL As Integer                        '�d���r�}�b�v�w�i�F_�y
        Dim nCntMapCountH As Integer                        '�d���r�}�b�v�r��臒l_�d
        Dim nCntMapCountM As Integer                        '�d���r�}�b�v�r��臒l_��
        Dim nCntMapCountL As Integer                        '�d���r�}�b�v�r��臒l_�y
        <VBFixedArray(NUM_MEN - 1), MarshalAs(UnmanagedType.ByValArray, SizeConst:=NUM_MEN)>
        Dim nLenDsp_Kensa() As Integer                      '�ݔ���_�\����__������(0�F�\ 1�F���j
        Dim nLenDsp_Cutter As Integer                       '�ݔ���_�\����__�V���[�J�b�g�ʒu

		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
		Dim nLongPd_GuidanceAvailable As Integer            ' ���������׃K�C�_���X�L��
		Dim nLongPd_NearFurnaceRangeFrom As Integer         ' ���������טF�`�J����͈�From
		Dim nLongPd_NearFurnaceRangeTo As Integer           ' ���������טF�`�J����͈�To
		Dim nLongPd_PeriodicPitchTolerance As Integer       ' ���������׎����s�b�`��ʕ\���p����
		Dim nLongPd_GuidanceForeColor As Integer            ' ���������׃K�C�_���X �����F
		Dim nLongPd_GuidanceBackColor As Integer            ' ���������׃K�C�_���X �w�i�F
		Dim strLongPd_GuidanceMessage As String             ' ���������׃K�C�_���X ���b�Z�[�W
		'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

		'�C�j�V�����C�Y
		Public Sub initialize()
            ReDim nLenDsp_Kensa(NUM_MEN - 1)
        End Sub
    End Structure

    ''' <summary>
    ''' �r��}�X�^�[
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_TYPE_NAME
        Dim nEdasysID As Integer                            'EdasysID
        Dim strName As String                               'Name
        Dim nPriority As Integer                            '�D��x
        Dim strMapName As String                            '�}�b�v�\������
        Dim nSymbolColor As Integer                         '�r�}�b�v�V���{���\���F
        Dim nMapSymbol As Integer                           '�r�}�b�v�V���{��
        Dim nImageType1 As Integer                          '�؏o�摜���1
        Dim nImageType2 As Integer                          '�؏o�摜���2
    End Structure

    ''' <summary>
    ''' �O���[�h�}�X�^�[
    ''' </summary>
    ''' <remarks></remarks>
    Public Structure TYP_GRADE_NAME
        Dim nEdasysID As Integer                            '�O���[�hEdasysID
        Dim strName As String                               'Name
        Dim nPriority As Integer                            '�D��x
        Dim nMapColor As Integer                            '�}�b�v�\���F
        Dim nDefGrade As Integer                            '���׃O���[�h
        Dim nSymbolColor As Integer                         '�r�}�b�v�V���{���\���F
    End Structure


    '///////////////////////////////////////////////////////////////////////////
    '�����o�ϐ�
    '///////////////////////////////////////////////////////////////////////////
    Private m_bIsDesposed As Boolean = False                                    '���\�[�X�J���t���O[True:�J����,False:���J��]

    Private mcls_Log As tClass_LogManager                                       '���O�Ǘ�

    Private m_dctType As Dictionary(Of Integer, TYP_TYPE_NAME)                  '�r��}�X�^�[(�L�[��EdasysID)
    Private m_dctGrade As Dictionary(Of Integer, TYP_GRADE_NAME)                '�O���[�h�}�X�^�[(�L�[��EdasysID)
    Private m_typCommonMaster As TYP_COMMON_MASTER                              '�萔�}�X�^�[

    '///////////////////////////////////////////////////////////////////////////
    '�v���p�e�B
    '///////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �T����ʕ\����ԑ�
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property MapStageBig() As Integer
        Get
            Return m_typCommonMaster.nMapMax
        End Get
    End Property

    ''' <summary>
    ''' �T����ʕ\����ԏ�
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property MapStageSmall() As Integer
        Get
            Return m_typCommonMaster.nMapMin
        End Get
    End Property

    ''' <summary>
    ''' �T����ʕ\����ԍŏ�
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property MapStageMostSmall() As Integer
        Get
            Return m_typCommonMaster.nMapMin
        End Get
    End Property


    ''' <summary>
    ''' �ݔ���_�\����__������ʒu[m]
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property PosKensa(ByVal nMen As Integer) As Integer
        Get
            Dim nMenWk As Integer = nMen
            If 0 > nMen OrElse NUM_MEN <= nMen Then        ' �ʂ��ُ�l�̏ꍇ
                nMenWk = 0
            End If

            Return CInt(Math.Ceiling(m_typCommonMaster.nLenDsp_Kensa(nMenWk) / 1000))
        End Get
    End Property

    ''' <summary>
    ''' �ݔ���_�\����__�V���[�J�b�g�ʒu[m]
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public ReadOnly Property PosCutter() As Integer
        Get
            Return CInt(Math.Ceiling(m_typCommonMaster.nLenDsp_Cutter / 1000))
        End Get
    End Property

	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
	''' <summary>
	''' ���������׃K�C�_���X�L��
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_GuidanceAvailable() As Integer
		Get
			Return m_typCommonMaster.nLongPd_GuidanceAvailable
		End Get
	End Property

	''' <summary>
	''' ���������טF�`�J����͈�From
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_NearFurnaceRangeFrom() As Integer
		Get
			Return m_typCommonMaster.nLongPd_NearFurnaceRangeFrom
		End Get
	End Property

	''' <summary>
	''' ���������טF�`�J����͈�To
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_NearFurnaceRangeTo() As Integer
		Get
			Return m_typCommonMaster.nLongPd_NearFurnaceRangeTo
		End Get
	End Property

	''' <summary>
	''' ���������׎����s�b�`��ʕ\���p����
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_PeriodicPitchTolerance() As Integer
		Get
			Return m_typCommonMaster.nLongPd_PeriodicPitchTolerance
		End Get
	End Property

	''' <summary>
	''' ���������׃K�C�_���X ���b�Z�[�W
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_GuidanceMessage() As String
		Get
			Return m_typCommonMaster.strLongPd_GuidanceMessage
		End Get
	End Property

	''' <summary>
	''' ���������׃K�C�_���X �����F
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_GuidanceForeColor() As Integer
		Get
			Return m_typCommonMaster.nLongPd_GuidanceForeColor
		End Get
	End Property

	''' <summary>
	''' ���������׃K�C�_���X �w�i�F
	''' </summary>
	''' <returns></returns>
	Public ReadOnly Property Pro_LongPd_GuidanceBackColor() As Integer
		Get
			Return m_typCommonMaster.nLongPd_GuidanceBackColor
		End Get
	End Property
	'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

	'///////////////////////////////////////////////////////////////////////////
	'�����o�֐�
	'///////////////////////////////////////////////////////////////////////////
	''' <summary>
	''' �R���X�g���N�^
	''' </summary>
	''' <param name="clsLog">���O�Ǘ�</param>
	''' <remarks></remarks>
	Public Sub New(ByRef clsLog As tClass_LogManager)
        Try
            mcls_Log = clsLog                                                       '���O�Ǘ�
            m_dctType = New Dictionary(Of Integer, TYP_TYPE_NAME)
            m_dctGrade = New Dictionary(Of Integer, TYP_GRADE_NAME)
            m_typCommonMaster.initialize()
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�p�����[�^�Ǘ������ُ�[{0}]", ex.Message))
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
                m_dctType.Clear()
                m_dctType = Nothing
                m_dctGrade.Clear()
                m_dctGrade = Nothing
            End If
            m_bIsDesposed = True                                                'Dispose�ς݂Ƃ���
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�I�u�W�F�N�g�j���ُ� [{0}]", ex.Message))
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
    ''' �S�p�����[�^�擾
    ''' </summary>
    ''' <returns>�擾����[True:����,False:���s]</returns>
    ''' <remarks></remarks>
    Public Function ReadAllParam() As Boolean
        '�T����ʕ\����Ԏ擾
        If Not ReadMapInterval() Then
            Return False
        End If

        '�r���萔�}�X�^�[�擾
        If Not ReadCommonMaster() Then
            Return False
        End If

        '�r��}�X�^�[�擾
        If Not ReadTypeName() Then
            Return False
        End If

        '�O���[�h�}�X�^�[�擾
        If Not ReadGradeName() Then
            Return False
        End If

        Return True
    End Function

    ''' <summary>
    ''' �T����ʕ\����Ԏ擾
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ReadMapInterval() As Boolean
        Dim bRet As Boolean = False
        Dim clsDB As tClass.tClass_SQLServer = Nothing
        Dim clsRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            'DB�ڑ�
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            If Not clsDB.Open() Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�ڑ����s"))
                Exit Try
            End If

            'SQL���s
            strSql = "SELECT"
            strSql &= " �T����ʕ\����ԏ�"
			strSql &= ",�T����ʕ\����ԑ�"
			strSql &= " FROM " & DB_COMMON_MASTER
            clsRead = clsDB.SelectExecute(strSql)
            If clsRead Is Nothing Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL���s���s[{0}]", strSql))
                Exit Try
            End If

            'DB�擾
            clsRead.Read()
			With m_typCommonMaster
				'�T����ʕ\����ԏ�
				.nMapMin = clsRead.GetInt32(clsRead.GetOrdinal("�T����ʕ\����ԏ�"))
				'�T����ʕ\����ԑ�
				.nMapMax = clsRead.GetInt32(clsRead.GetOrdinal("�T����ʕ\����ԑ�"))
			End With

			'�擾����
			bRet = True
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]�擾�ُ�[{1}]", DB_COMMON_MASTER, ex.Message))
        Finally
            If Not clsRead Is Nothing Then
                clsRead.Close()
                clsRead = Nothing
            End If
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �r���萔�}�X�^�[�擾
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ReadCommonMaster() As Boolean
        Dim bRet As Boolean = False
        Dim clsDB As tClass.tClass_SQLServer = Nothing
        Dim clsRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            'DB�ڑ�
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            If Not clsDB.Open() Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�ڑ����s"))
                Exit Try
            End If

            'SQL���s
            strSql = "SELECT"
            strSql &= " �d���r�}�b�v�w�i�F_�d"
            strSql &= ",�d���r�}�b�v�w�i�F_��"
            strSql &= ",�d���r�}�b�v�w�i�F_�y"
            strSql &= ",�d���r�}�b�v�r��臒l_�d"
            strSql &= ",�d���r�}�b�v�r��臒l_��"
            strSql &= ",�d���r�}�b�v�r��臒l_�y"
            strSql &= ",�ݔ���_�\����__��������"
            strSql &= ",�ݔ���_�\����__�\������"
			strSql &= ",�ݔ���_�\����__�V���["
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
			strSql &= ",LongPd_GuidanceAvailable"               ' ���������׃K�C�_���X�L��
			strSql &= ",LongPd_NearFurnaceRangeFrom"            ' ���������טF�`�J����͈�From
			strSql &= ",LongPd_NearFurnaceRangeTo"              ' ���������טF�`�J����͈�To
			strSql &= ",LongPd_PeriodicPitchTolerance"          ' ���������׎����s�b�`��ʕ\���p����
			strSql &= ",LongPd_GuidanceForeColor"               ' ���������׃K�C�_���X �����F
			strSql &= ",LongPd_GuidanceBackColor"               ' ���������׃K�C�_���X �w�i�F
			strSql &= ",LongPd_GuidanceMessage"                 ' ���������׃K�C�_���X ���b�Z�[�W
			'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------

			strSql &= " FROM " & DB_COMMON_MASTER
            clsRead = clsDB.SelectExecute(strSql)
            If clsRead Is Nothing Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL���s���s[{0}]", strSql))
                Exit Try
            End If

            'DB�擾
            clsRead.Read()
            With m_typCommonMaster
                '�d���r�}�b�v�w�i�F_�d
                .nCntMapColorH = clsRead.GetInt32(clsRead.GetOrdinal("�d���r�}�b�v�w�i�F_�d"))
                '�d���r�}�b�v�w�i�F_��
                .nCntMapColorM = clsRead.GetInt32(clsRead.GetOrdinal("�d���r�}�b�v�w�i�F_��"))
                '�d���r�}�b�v�w�i�F_�y
                .nCntMapColorL = clsRead.GetInt32(clsRead.GetOrdinal("�d���r�}�b�v�w�i�F_�y"))
                '�d���r�}�b�v�r��臒l_�d
                .nCntMapCountH = clsRead.GetInt32(clsRead.GetOrdinal("�d���r�}�b�v�r��臒l_�d"))
                '�d���r�}�b�v�r��臒l_��
                .nCntMapCountM = clsRead.GetInt32(clsRead.GetOrdinal("�d���r�}�b�v�r��臒l_��"))
                '�d���r�}�b�v�r��臒l_�y
                .nCntMapCountL = clsRead.GetInt32(clsRead.GetOrdinal("�d���r�}�b�v�r��臒l_�y"))
                '�ݔ���_�\����__�\������
                .nLenDsp_Kensa(0) = clsRead.GetInt32(clsRead.GetOrdinal("�ݔ���_�\����__�\������"))
                '�ݔ���_�\����__��������
                .nLenDsp_Kensa(1) = clsRead.GetInt32(clsRead.GetOrdinal("�ݔ���_�\����__��������"))
				'�ݔ���_�\����__�V���[�J�b�g�ʒu
				.nLenDsp_Cutter = clsRead.GetInt32(clsRead.GetOrdinal("�ݔ���_�\����__�V���["))
				'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) ---------------->>>>>
				'���������׃K�C�_���X�L��
				.nLongPd_GuidanceAvailable = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_GuidanceAvailable"))
				'���������טF�`�J����͈�From
				.nLongPd_NearFurnaceRangeFrom = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_NearFurnaceRangeFrom"))
				'���������טF�`�J����͈�To
				.nLongPd_NearFurnaceRangeTo = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_NearFurnaceRangeTo"))
				'���������׎����s�b�`��ʕ\���p����
				.nLongPd_PeriodicPitchTolerance = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_PeriodicPitchTolerance"))
				' ���������׃K�C�_���X �����F
				.nLongPd_GuidanceForeColor = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_GuidanceForeColor"))
				' ���������׃K�C�_���X �w�i�F
				.nLongPd_GuidanceBackColor = clsRead.GetInt32(clsRead.GetOrdinal("LongPd_GuidanceBackColor"))
				' ���������׃K�C�_���X ���b�Z�[�W
				.strLongPd_GuidanceMessage = clsRead.GetString(clsRead.GetOrdinal("LongPd_GuidanceMessage"))
				'// 2024.07.26 Kouno 2CGL�������u�Ӄ֐��x����@�\����(������) <<<<<----------------
			End With

            '�擾����
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]�擾�ُ�[{1}]", DB_COMMON_MASTER, ex.Message))
        Finally
            If Not clsRead Is Nothing Then
                clsRead.Close()
                clsRead = Nothing
            End If
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �r��}�X�^�[�擾
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ReadTypeName() As Boolean
        Dim bRet As Boolean = False
        Dim clsDB As tClass.tClass_SQLServer = Nothing
        Dim clsRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            'DB�ڑ�
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            If Not clsDB.Open() Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�ڑ����s"))
                Exit Try
            End If

            'SQL���s
            strSql = "SELECT"
            strSql &= " EdasysID"
            strSql &= ",Name"
            strSql &= ",�D��x"
            strSql &= ",�}�b�v�\������"
            strSql &= ",�r�}�b�v�V���{���\���F"
            strSql &= ",�r�}�b�v�V���{��"
            strSql &= ",ISNULL(�؏o�摜���1,1) as �؏o�摜���1"
            strSql &= ",ISNULL(�؏o�摜���2,2) as �؏o�摜���2"
            strSql &= " FROM " & DB_TYPE_NAME
            strSql &= " ORDER BY ID ASC"
            clsRead = clsDB.SelectExecute(strSql)
            If clsRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL���s���s[{0}]", strSql))
                Exit Try
            End If

            'DB�擾
            m_dctType.Clear()
            While clsRead.Read() = True
                Dim typTypeName As New TYP_TYPE_NAME
                With typTypeName
                    'EdasysID
                    .nEdasysID = clsRead.GetInt32(clsRead.GetOrdinal("EdasysID"))
                    'Name
                    .strName = clsRead.GetString(clsRead.GetOrdinal("Name"))
                    '�D��x
                    .nPriority = clsRead.GetInt32(clsRead.GetOrdinal("�D��x"))
                    '�}�b�v�\������
                    .strMapName = clsRead.GetString(clsRead.GetOrdinal("�}�b�v�\������"))
                    ' �r�}�b�v�V���{���\���F
                    .nSymbolColor = clsRead.GetInt32(clsRead.GetOrdinal("�r�}�b�v�V���{���\���F"))
                    ' �r�}�b�v�V���{��
                    .nMapSymbol = clsRead.GetInt32(clsRead.GetOrdinal("�r�}�b�v�V���{��"))
                    If "" <> clsRead.GetValue(5).ToString() Then
                        .nImageType1 = clsRead.GetInt32(clsRead.GetOrdinal("�؏o�摜���1"))
                    Else
                        .nImageType1 = 0
                    End If
                    If "" <> clsRead.GetValue(6).ToString() Then
                        .nImageType2 = clsRead.GetInt32(clsRead.GetOrdinal("�؏o�摜���2"))
                    Else
                        .nImageType2 = 0
                    End If

                End With
                '�f�B�N�V���i���o�^
                m_dctType.Add(typTypeName.nEdasysID, typTypeName)
            End While

            '�擾����
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]�擾�ُ�[{1}]", DB_TYPE_NAME, ex.Message))
        Finally
            If Not clsRead Is Nothing Then
                clsRead.Close()
                clsRead = Nothing
            End If
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �O���[�h�}�X�^�[�擾
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Function ReadGradeName() As Boolean
        Dim bRet As Boolean = False
        Dim clsDB As tClass.tClass_SQLServer = Nothing
        Dim clsRead As SqlClient.SqlDataReader = Nothing
        Dim strSql As String

        Try
            'DB�ڑ�
            clsDB = New tClass_SQLServer(TASKINI_NAME, INI_DB)
            Dim dbret As Boolean = clsDB.Open()
            If Not clsDB.Open() Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("DB�ڑ����s"))
                Exit Try
            End If

            'SQL���s
            strSql = "SELECT"
            strSql &= " EdasysID"
            strSql &= ",Name"
            strSql &= ",�D��x"
            strSql &= ",�}�b�v�\���F"
            strSql &= ",���׃O���[�h"
            strSql &= ",�r�}�b�v�V���{���\���F"
            strSql &= " FROM " & DB_GRADE_NAME
            strSql &= " ORDER BY ID ASC"
            clsRead = clsDB.SelectExecute(strSql)
            If clsRead Is Nothing = True Then
                mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("SQL���s���s[{0}]", strSql))
                Exit Try
            End If

            'DB�擾
            m_dctGrade.Clear()
            While clsRead.Read() = True
                Dim typGradeName As New TYP_GRADE_NAME
                With typGradeName
                    '�O���[�hEdasysID
                    .nEdasysID = clsRead.GetInt32(clsRead.GetOrdinal("EdasysID"))
                    'Name
                    .strName = clsRead.GetString(clsRead.GetOrdinal("Name"))
                    '�D��x
                    .nPriority = clsRead.GetInt32(clsRead.GetOrdinal("�D��x"))
                    '�}�b�v�\���F
                    .nMapColor = clsRead.GetInt32(clsRead.GetOrdinal("�}�b�v�\���F"))
                    '���׃O���[�h
                    .nDefGrade = clsRead.GetInt32(clsRead.GetOrdinal("���׃O���[�h"))
                    '�r�}�b�v�V���{���\���F 
                    .nSymbolColor = clsRead.GetInt32(clsRead.GetOrdinal("�r�}�b�v�V���{���\���F"))
                End With
                m_dctGrade.Add(typGradeName.nEdasysID, typGradeName)
            End While

            '�擾����
            bRet = True
        Catch ex As Exception
            mcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("[{0}]�擾�ُ�[{1}]", DB_GRADE_NAME, ex.Message))
        Finally
            If Not clsRead Is Nothing Then
                clsRead.Close()
                clsRead = Nothing
            End If
            If Not clsDB Is Nothing Then
                clsDB.Cancel()
                clsDB.Close()
                clsDB.Dispose()
                clsDB = Nothing
            End If
        End Try

        Return bRet
    End Function

    ''' <summary>
    ''' �r���̎擾
    ''' </summary>
    ''' <param name="nEdasysID">�r��EdasysID</param>
    ''' <returns>�r����</returns>
    ''' <remarks>�Ώۂ��Ȃ��ꍇ�͋󕶎�</remarks>
    Public Function GetTypeName(ByVal nEdasysID As Integer) As String
        Dim strName As String = ""                          '�Ώۖ���

        If m_dctType.ContainsKey(nEdasysID) Then            '�ΏۗL��
            strName = m_dctType(nEdasysID).strName
        End If

        Return strName
    End Function

    ''' <summary>
    ''' �r�}�b�v�\�����擾
    ''' </summary>
    ''' <param name="nEdasysID">�r��EdasysID</param>
    ''' <returns>�r�}�b�v�\����</returns>
    ''' <remarks>�Ώۂ��Ȃ��ꍇ�͋󕶎�</remarks>
    Public Function GetMapName(ByVal nEdasysID As Integer) As String
        Dim strMapName As String = ""                       '�Ώۖ���

        If m_dctType.ContainsKey(nEdasysID) Then            '�ΏۗL��
            strMapName = m_dctType(nEdasysID).strMapName
        End If

        Return strMapName
    End Function

    ''' <summary>
    ''' �r�D��x�擾
    ''' </summary>
    ''' <param name="nEdasysID">�r��EdasysID</param>
    ''' <returns>�r�D��x</returns>
    ''' <remarks>�Ώۂ��Ȃ��ꍇ��[999]</remarks>
    Public Function GetTypePriority(ByVal nEdasysID As Integer) As Integer
        Dim nPriority As Integer = 999                      '�Ώۖ���

        If m_dctType.ContainsKey(nEdasysID) Then            '�ΏۗL��
            nPriority = m_dctType(nEdasysID).nPriority
        End If

        Return nPriority
    End Function

    ''' <summary>
    ''' �}�b�v�\���F�擾
    ''' </summary>
    ''' <param name="nEdasysID">�O���[�hEdasysID</param>
    ''' <returns>�}�b�v�\���F</returns>
    ''' <remarks>�Ώۂ��Ȃ��ꍇ�͔��F</remarks>
    Public Function GetGradeColor(ByVal nEdasysID As Integer) As Color
        Dim typMapColor As Color = Color.White              '�Ώۖ���

        If m_dctGrade.ContainsKey(nEdasysID) Then           '�ΏۗL��
            typMapColor = tMod_Common.GetIntToColor(m_dctGrade(nEdasysID).nMapColor)
        End If

        Return typMapColor
    End Function

    ''' <summary>
    ''' �O���[�h���̎擾
    ''' </summary>
    ''' <param name="nEdasysID">�O���[�hEdasysID</param>
    ''' <returns>�O���[�h����</returns>
    ''' <remarks>�Ώۂ��Ȃ��ꍇ�͋󕶎�</remarks>
    Public Function GetGradeName(ByVal nEdasysID As Integer) As String
        Dim strName As String = ""                          '�Ώۖ���

        If m_dctGrade.ContainsKey(nEdasysID) Then           '�ΏۗL��
            strName = m_dctGrade(nEdasysID).strName
        End If

        Return strName
    End Function

    ''' <summary>
    ''' �O���[�h�D��x�擾
    ''' </summary>
    ''' <param name="nEdasysID">�O���[�hEdasysID</param>
    ''' <returns>�O���[�h�D��x</returns>
    ''' <remarks>�Ώۂ��Ȃ��ꍇ��[999]</remarks>
    Public Function GetGradePriority(ByVal nEdasysID As Integer) As Integer
        Dim nPriority As Integer = 999                      '�Ώۖ���

        If m_dctGrade.ContainsKey(nEdasysID) Then           '�ΏۗL��
            nPriority = m_dctGrade(nEdasysID).nPriority
        End If

        Return nPriority
    End Function


    ''' <summary>
    ''' ���׃O���[�h�擾
    ''' </summary>
    ''' <param name="nEdasysID">�O���[�hEdasysID</param>
    ''' <returns>���׃O���[�h</returns>
    ''' <remarks>�Ώۂ��Ȃ��ꍇ��[EM_DETECT_GR2.DETECT_GR_NON(���Q)]</remarks>
    Public Function GetDefectGrade(ByVal nEdasysID As Integer) As Integer
        Dim nDefGrade As Integer = EM_DETECT_GR2.DETECT_GR_NON      '���Q

        If m_dctGrade.ContainsKey(nEdasysID) Then                   '�ΏۗL��
            nDefGrade = m_dctGrade(nEdasysID).nDefGrade
        End If

        Return nDefGrade
    End Function

    ''' <summary>
    ''' �r�D��x��r
    ''' </summary>
    ''' <param name="nType1">�r1���r��EdasysID</param>
    ''' <param name="nType2">�r2���r��EdasysID</param>
    ''' <param name="nGrade1">�r1�̃O���[�hEdasysID</param>
    ''' <param name="nGrade2">�r2�̃O���[�hEdasysID</param>
    ''' <returns>��r����</returns>
    ''' <remarks>-1:�r1���D��x�� 0:�����D��x 1:�r2���D��x��</remarks>
    Public Function CompDefPriority(ByVal nType1 As Integer, ByVal nType2 As Integer, ByVal nGrade1 As Integer, ByVal nGrade2 As Integer) As Integer
        Dim nComp As Integer = 0                            '�����D��x
        Dim nPri1 As Integer = GetGradePriority(nGrade1)    '�O���[�h�D��x1
        Dim nPri2 As Integer = GetGradePriority(nGrade2)    '�O���[�h�D��x2

        '�D��x�͏������ق�������
        If nPri1 > nPri2 Then                               '�r2�̃O���[�h�D��x������
            nComp = 1                                       '�r2���D��
        ElseIf nPri1 < nPri2 Then                           '�r1�̃O���[�h�D��x������
            nComp = -1                                      '�r1���D��
        Else                                                '�O���[�h�D��x��������
            nPri1 = GetTypePriority(nType1)                 '�r�D��x1
            nPri2 = GetTypePriority(nType2)                 '�r�D��x2
            If nPri1 > nPri2 Then                           '�r2���r��D��x������
                nComp = 1                                   '�r2���D��
            ElseIf nPri1 < nPri2 Then                       '�r1���r��D��x������
                nComp = -1                                  '�r1���D��
            End If
        End If

        Return nComp
    End Function

    ''' <summary>
    ''' �d���r�}�b�v�w�i�F�擾
    ''' </summary>
    ''' <param name="nDefGrade">���׃O���[�h</param>
    ''' <returns>�d���r�}�b�v�w�i�F</returns>
    ''' <remarks>�Ώۂ��Ȃ��ꍇ�͔��F</remarks>
    Public Function GetCntMapColor(ByVal nDefGrade As Integer) As Color
        Dim typBackColor As Color = Color.White             '�Ώۖ���

        Select Case nDefGrade
            Case EM_DETECT_GR.DETECT_GR_KEI                 '�y����
                typBackColor = tMod_Common.GetIntToColor(m_typCommonMaster.nCntMapColorL)
            Case EM_DETECT_GR.DETECT_GR_TYU                 '������
                typBackColor = tMod_Common.GetIntToColor(m_typCommonMaster.nCntMapColorM)
            Case EM_DETECT_GR.DETECT_GR_JU                 '�d����
                typBackColor = tMod_Common.GetIntToColor(m_typCommonMaster.nCntMapColorH)
        End Select

        Return typBackColor
    End Function

    ''' <summary>
    ''' �d���r�}�b�v�r��臒l�擾
    ''' </summary>
    ''' <param name="nDefGrade">���׃O���[�h</param>
    ''' <returns>�d���r�}�b�v�r��臒l</returns>
    ''' <remarks>�Ώۂ��Ȃ��ꍇ�͌y����臒l</remarks>
    Public Function GetCntMapCount(ByVal nDefGrade As Integer) As Integer
        Dim nDefCount As Integer = m_typCommonMaster.nCntMapCountL  '�Ώۖ���

        Select Case nDefGrade
            Case EM_DETECT_GR.DETECT_GR_KEI                 '�y����
                nDefCount = m_typCommonMaster.nCntMapCountL
            Case EM_DETECT_GR.DETECT_GR_TYU                 '������
                nDefCount = m_typCommonMaster.nCntMapCountM
            Case EM_DETECT_GR.DETECT_GR_JU                 '�d����
                nDefCount = m_typCommonMaster.nCntMapCountH
        End Select

        Return nDefCount
    End Function


    ''' <summary>
    ''' �摜�\���ԍ���Ԃ��܂��B
    ''' </summary>
    ''' <param name="nEdasysID">�擾�������r��EdasysID</param>
    ''' <param name="emImagePos">�擾�������摜�ʒu</param>
    ''' <returns>�r�햼 �Ȃ��ꍇ�͋󕶎�</returns>
    ''' <remarks></remarks>
    Public Function GetTypeImage(ByVal nEdasysID As Integer, ByVal emImagePos As EM_IMAGE_POS) As Integer
        If m_dctType.ContainsKey(nEdasysID) = False Then                        ' EdasysID���Ȃ��ꍇ
            Return -1
        End If
        If emImagePos = EM_IMAGE_POS.IMG1 Then                                ' ��
            Return m_dctType(nEdasysID).nImageType1
        ElseIf emImagePos = EM_IMAGE_POS.IMG2 Then                            ' ��
            Return m_dctType(nEdasysID).nImageType2
        Else                                                                    ' ���̑�
            Return -1
        End If
    End Function


    ''' <summary>
    ''' �O���[�h�̃V���{���F���擾���܂�
    ''' </summary>
    ''' <param name="nEdasysID">�擾�������O���[�h��EdasysID</param>
    ''' <returns>�O���[�h�̃V���{���F</returns>
    ''' <remarks></remarks>
    Public Function GetGradeSymbolColor(ByVal nEdasysID As Integer) As Color
        If m_dctGrade.ContainsKey(nEdasysID) = False Then                       ' EdasysID���Ȃ��ꍇ
            Return Color.White
        End If
        Return tMod.GetIntToColor(m_dctGrade(nEdasysID).nSymbolColor)           ' �F��߂�
    End Function

    ''' <summary>
    ''' �r��̃V���{���F���擾���܂�
    ''' </summary>
    ''' <param name="nEdasysID">�擾�������O���[�h��EdasysID</param>
    ''' <returns>�r��̃V���{���F</returns>
    ''' <remarks></remarks>
    Public Function GetTypeSymbolColor(ByVal nEdasysID As Integer) As Color
        If m_dctType.ContainsKey(nEdasysID) = False Then                       ' EdasysID���Ȃ��ꍇ
            Return Color.White
        End If
        Return tMod.GetIntToColor(m_dctType(nEdasysID).nSymbolColor)           ' �F��߂�
    End Function


    ''' <summary>
    ''' �r�}�b�v�\����ʁi�U�z�}�j�p���r�V���{���擾
    ''' </summary>
    ''' <param name="nEdasysID">�擾�������r��EdasysID</param>
    ''' <returns>�r�}�b�v�V���{���ԍ�</returns>
    ''' <remarks></remarks>
    Public Function GetMapSymbolNo(ByVal nEdasysID As Integer) As Integer
        If m_dctType.ContainsKey(nEdasysID) = False Then                        ' EdasysID���Ȃ��ꍇ
            Return -1
        End If
        Return m_dctType(nEdasysID).nMapSymbol
    End Function

    ''' <summary>
    ''' �O���[�h�����擾���܂�
    ''' </summary>
    ''' <returns>�O���[�h�̗D��x</returns>
    ''' <remarks></remarks>
    Public Function GetParamGrade() As Dictionary(Of Integer, TYP_GRADE_NAME).ValueCollection
        Return m_dctGrade.Values
    End Function

    ''' <summary>
    ''' �ł��D��x���Ⴂ�O���[�h���擾
    ''' </summary>
    ''' <returns>�ł��D��x���Ⴂ�O���[�h��EdasysID</returns>
    ''' <remarks></remarks>
    Public Function GetPriorityMostLowGrade() As Integer

        Dim nWkEdasysID As Integer = 0
        Dim nWkPriority As Integer = 0

        '�ł��D��x���Ⴂ�̂�����
        For Each nEdasyID As Integer In m_dctGrade.Keys

            If nWkPriority < m_dctGrade(nEdasyID).nPriority Then
                nWkEdasysID = nEdasyID
                nWkPriority = m_dctGrade(nEdasyID).nPriority
            End If

        Next

        Return nWkEdasysID

    End Function

End Class
