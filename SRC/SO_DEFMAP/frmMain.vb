'*********************************************************************************
' �r�_�}�b�v���
'	[Ver]
'		Ver.01    2017/12/01  �V�K�쐬
'
'	[����]
'		
'*********************************************************************************
Option Strict On
Imports tClassLibrary
Imports System.Runtime.InteropServices
Imports System.Text

Public Class frmMain
    Implements IDisposable                          ' �f�X�g���N�^��`

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �O���C�x���g
    '/ /////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �t�@���N�V�����{�^�������C�x���g (�x�[�X�̒u������)
    ''' </summary>
    ''' <param name="FncNo">0�I���W��</param>
    ''' <remarks></remarks>
    Public Shadows Event tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �f���Q�[�g�֐�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private Delegate Sub m_deleDspNew(ByVal bReadCoil As Boolean)    '�ʃX���b�h����̃R���g���[���A�N�Z�X(�ŐV�Ǎ���)
    Private Delegate Sub m_deleChangeParam(ByVal nMode As Integer)   '�ʃX���b�h����̃R���g���[���A�N�Z�X(�p�����[�^�ēǂݍ���)

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�萔
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private DSP_DEFMAP_CNT As Integer = 3                               ' �\���r�}�b�v��
    Private DSP_DEFMAP_DATCNT As Integer = 10                           ' �f�[�^�擾�ΏۃR�C����

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�N���X
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private mcls_ParamManager As clsParamManager                        ' �p�����[�^�Ǘ��N���X
    Private mcls_Log As tClass.tClass_LogManager                        ' ���O�Ǘ��N���X
    Private mcls_DefMap(DSP_DEFMAP_CNT - 1) As clsDefMap                ' �r�}�b�v�\�����N���X

    '/ /////////////////////////////////////////////////////////////////////////////////
    '/ �����o�[�ϐ�
    '/ /////////////////////////////////////////////////////////////////////////////////
    Private m_bLoad As Boolean = False                                  ' ��ʃ��[�h���t���O
    Private m_bInit As Boolean = False                                  ' ��ʏ������t���O

    Private m_bUpTurn(NUM_MEN - 1) As Boolean                           ' �r�}�b�v �㉺���]�t���O(Ture:���],False:���]�Ȃ�)
    Private m_bSideTurn(NUM_MEN - 1) As Boolean                         ' �r�}�b�v ���E���]�t���O(Ture:���],False:���]�Ȃ�)

    Private m_nDspStartIdx As Integer = 0                               ' ���ݕ\�����̕\���J�nIndex
    Private m_nDspMaxIdx As Integer = 0                                 ' ���ݕ\�����̕\���\�ő�Index
    Private m_nDspMen As Integer = 0                                    ' ���ݕ\�����̖�(0:�\ 1:��)
    Private m_nRefreshMode As EM_REFRESH_MODE = EM_REFRESH_MODE.AUTO    ' �X�V���[�h

    Private m_ctlMap(DSP_DEFMAP_CNT - 1) As MAP_CTRL                    ' �}�b�v�p�R���g���[��

    '�\���p�f�[�^
    Private m_typDspData(DSP_DEFMAP_DATCNT - 1) As clsDefMap.DSP_DATA   ' ��ʕ\���f�[�^

    '�}�b�v�R���g���[��
    Private Structure MAP_CTRL
        Dim picMap As PictureBox
        Dim lblName As Label
        Dim lblCoilNo As Label
    End Structure

    '�t�@���N�V�����{�^����`
    Private Enum EM_BTN_MODE
        nPreCoil = 0        ' F1  �O�R�C��
        nNextCoil           ' F2  ���R�C��
        nChangeOU           ' F3  ���ʕ\���i�\�ʕ\���j
        nYobi1              ' F4  
        nStop               ' F5  �ꎞ��~
        nYobi2              ' F6
        nYobi3              ' F7
        nYobi4              ' F8
        nYobi5              ' F9
        nPrint              ' F10 ��ʈ��
        nSave               ' F11 ��ʕۑ�
        nClose              ' F12 ����
    End Enum

    ''' <summary>
    ''' �R���X�g���N�^
    ''' </summary>
    Public Sub New()
        ' ���̌Ăяo���́AWindows �t�H�[�� �f�U�C�i�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B
    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_Load(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Load
        '''' �^�C�g���ݒ�
        Call SetTitleName(Me)

        ' �t�H�[�����
        Me.tProp_EndButtonEnable = False

        m_bLoad = True
        For ii As Integer = 0 To DSP_DEFMAP_DATCNT - 1
            m_typDspData(ii).Initialize()
        Next

        '�����\���ݒ�
        Call DspInitProcess()

        ' label�����ݒ�
        m_ctlMap(0).lblCoilNo = lblCoilNo1
        m_ctlMap(1).lblCoilNo = lblCoilNo2
        m_ctlMap(2).lblCoilNo = lblCoilNo3
        m_ctlMap(0).lblName = lblDsp1
        m_ctlMap(1).lblName = lblDsp2
        m_ctlMap(2).lblName = lblDsp3

        ' PictureBox�����ݒ�
        m_ctlMap(0).picMap = picMap1
        m_ctlMap(1).picMap = picMap2
        m_ctlMap(2).picMap = picMap3
        For ii As Integer = 0 To DSP_DEFMAP_CNT - 1
            AddHandler m_ctlMap(ii).picMap.Paint, AddressOf picMap_Paint
        Next

        ' �{�^����`
        MyBase.AddFunctionButton(btnF01, EM_BTN_MODE.nPreCoil)      ' �O�R�C��
        MyBase.AddFunctionButton(btnF02, EM_BTN_MODE.nNextCoil)     ' ���R�C��
        MyBase.AddFunctionButton(btnF03, EM_BTN_MODE.nChangeOU)     ' ���ʕ\��
        MyBase.AddFunctionButton(btnF05, EM_BTN_MODE.nStop)         ' �ꎟ��~
        MyBase.AddFunctionButton(btnF10, EM_BTN_MODE.nPrint)        ' ��ʈ��
        MyBase.AddFunctionButton(btnF11, EM_BTN_MODE.nSave)         ' ��ʕۑ�
        MyBase.AddFunctionButton(btnF12, EM_BTN_MODE.nClose)        ' ����

        '�N���X�ݒ�
        mcls_ParamManager = New clsParamManager(gcls_Log)       '�p�����[�^�Ǘ�
        mcls_ParamManager.ReadAll()

        Call CreateDefMap()     '�r�}�b�v�\�����Ǘ�

        '��ʏ����\���i�󔒁j
        Call DspInit(True)

        Call ChangeRefreshMode(EM_REFRESH_MODE.AUTO)

        m_bLoad = False
        Application.DoEvents()

        '�r�}�b�v���щ�ʕ\���i�^�C�}�[�ŋN���j
        tmrDsp.Enabled = True

    End Sub

    ''' <summary>
    ''' �����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspInitProcess()

        Dim blnTurn As Boolean

        '�r�}�b�v�㉺���]�i�\�j
        blnTurn = CBool(IIf(0 = tMod.ReadIniInt(SO_DEFMAP, "DEFMAP_UP_TOP", TASKINI_NAME), False, True))
        m_bUpTurn(0) = blnTurn

        '�r�}�b�v�㉺���]�i���j
        blnTurn = CBool(IIf(0 = tMod.ReadIniInt(SO_DEFMAP, "DEFMAP_UP_BOT", TASKINI_NAME), False, True))
        m_bUpTurn(1) = blnTurn

        '�r�}�b�v���E���]�i�\�j
        blnTurn = CBool(IIf(0 = tMod.ReadIniInt(SO_DEFMAP, "DEFMAP_SIDE_TOP", TASKINI_NAME), False, True))
        m_bSideTurn(0) = blnTurn

        '�r�}�b�v���E���]�i���j
        blnTurn = CBool(IIf(0 = tMod.ReadIniInt(SO_DEFMAP, "DEFMAP_SIDE_BOT", TASKINI_NAME), False, True))
        m_bSideTurn(1) = blnTurn

    End Sub


    ''' <summary>
    ''' �r�}�b�v���Ǘ��N���X����
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub CreateDefMap()

        Dim typDefMap As clsDefMap.DSP_CONTROL_DATA = Nothing

        '�r�}�b�v�R���g���[�����Z�b�g
        For ii As Integer = 0 To DSP_DEFMAP_CNT - 1
            typDefMap.lblMsg = lblMsg         '�K�C�_���X���b�Z�[�W
            typDefMap.prbMsg = prbMsg         '�K�C�_���X�i��

            '�r�}�b�v���x��
            Select Case ii
                Case 0
                    typDefMap.lblWs = lblWs1
                    typDefMap.lblDs = lblDs1
                Case 1
                    typDefMap.lblWs = lblWs2
                    typDefMap.lblDs = lblDs2
                Case Else
                    typDefMap.lblWs = lblWs3
                    typDefMap.lblDs = lblDs3
            End Select

            typDefMap.lblNgMsg = lblNgMsg     '�ُ�K�C�_���X�\���p

            '�r�}�b�v�N���X
            mcls_DefMap(ii) = New clsDefMap(typDefMap, m_bUpTurn(m_nDspMen), m_bSideTurn(m_nDspMen), 0, mcls_Log, mcls_ParamManager)
        Next

    End Sub

    ''' <summary>
    ''' ����������
    ''' </summary>
    ''' <param name="blnExec">�{�^����������(True:�������AFalse:�{�^������������)</param>
    ''' <remarks></remarks>
    Private Sub DspInit(ByVal blnExec As Boolean)

        Dim ii As Integer

        '��ʊ�{��񏉊��\��
        Call DspBaseInit()

        '��ʏ������t���O�ݒ�
        m_bInit = True

        '�r�}�b�v�����\��
        For ii = 0 To DSP_DEFMAP_CNT - 1
            mcls_DefMap(ii).DspInit()
            mcls_DefMap(ii).tProp_nCoilLen = 0          '�����\���̃R�C������0�Ƃ���
            mcls_DefMap(ii).tProp_nCoilWid = 0          '�����\���̃R�C������0�Ƃ���B
        Next ii
        Call RefreshMap()                           ' �}�b�v�ĕ`��

        m_nDspStartIdx = 0

    End Sub

    ''' <summary>
    ''' ����������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspBaseInit()

        '�󔒕\��
        For ii As Integer = 0 To m_ctlMap.Length - 1
            m_ctlMap(ii).lblCoilNo.Text = ""
            m_ctlMap(ii).lblName.Text = ""
        Next

        lblFace.Text = "�\��"

        '���t���b�V���C�x���g�ŕ\��
        Call DspBase()

    End Sub

    ''' <summary>
    ''' �\������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspBase()
        For ii As Integer = 0 To m_ctlMap.Length - 1
            m_ctlMap(ii).lblCoilNo.Refresh()
            m_ctlMap(ii).lblName.Refresh()
        Next
    End Sub


    ''' <summary>
    ''' �r�}�b�v�ĕ`��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub RefreshMap()
        For ii As Integer = 0 To DSP_DEFMAP_CNT - 1
            m_ctlMap(ii).picMap.Refresh()
        Next
    End Sub


    ''' <summary>
    ''' �\���f�[�^�擾
    ''' </summary>
    ''' <param name="bGetCoilFlg">�R�C�����擾�t���O(True:�擾�i�K��l�j</param>
    ''' <remarks></remarks>
    Private Sub GetDspData(Optional ByVal bGetCoilFlg As Boolean = True)
        Dim typDspDataBuf(DSP_DEFMAP_DATCNT - 1) As clsDefMap.DSP_DATA
        Dim strSql As String
        Dim nCnt As Integer = 0
        Dim nWk As Integer
        Dim strWk As String

        Dim tcls_DB As New tClass.tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing


        Try
            Call DspGuidMsgLabel(True)

            'DB�I�[�v��
            If Not tcls_DB.Open() Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[ [" & DB_COIL_INF & "]")
                Exit Sub
            End If

            If True = bGetCoilFlg Then

                Call DspWaitInf(String.Format("�R�C�����擾��"), 1)

                ' ������
                For ii As Integer = 0 To typDspDataBuf.Length - 1
                    typDspDataBuf(ii).Initialize()
                Next

                m_nDspStartIdx = 0

                ' �\���Ώۂ̃R�C��10�����擾
                strSql = ""
                strSql &= "SELECT "
                strSql &= "    TOP " + DSP_DEFMAP_DATCNT.ToString + " "
                strSql &= "    * "
                strSql &= "FROM "
                strSql &= "    " + DB_COIL_INF + " I "
                strSql &= "    INNER JOIN " + DB_COIL_RESULT + " R "
                strSql &= "    ON I.�Ǘ�No = R.�Ǘ�No "
                strSql &= "ORDER BY "
                strSql &= "    I.�o�^���t DESC "

                sqlRead = tcls_DB.SelectExecute(strSql)
                While sqlRead.Read()
                    With typDspDataBuf(nCnt)
                        .strKizukenNo = sqlRead.GetSqlValue(sqlRead.GetOrdinal("�Ǘ�No")).ToString.Trim()

                        ' �R�C�����(��{��)
                        strWk = ""
                        If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("���YNo")) Then
                            strWk = sqlRead.GetString(sqlRead.GetOrdinal("���YNo"))
                        End If
                        .typBaseData.strSeisanNo = strWk

                        nWk = 0
                        If Not sqlRead.IsDBNull(sqlRead.GetOrdinal("�ޗ���")) Then
                            nWk = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�ޗ���")))
                        End If
                        .typBaseData.nWidZai = nWk

                        .typBaseData.nCoilLen = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("����������")))

                        .typBaseData.nPCode(0) = sqlRead.GetInt32(sqlRead.GetOrdinal("�����p�^�[���R�[�h_�\"))
                        .typBaseData.nPCode(1) = sqlRead.GetInt32(sqlRead.GetOrdinal("�����p�^�[���R�[�h_��"))

                    End With

                    nCnt += 1
                End While

                sqlRead.Close()

                For nCut As Integer = 0 To typDspDataBuf.Count - 1
                    '--------------
                    '�J�b�g�ʒu                    
                    '�J�b�g���擾
                    GetCutInf(typDspDataBuf(nCut), tcls_DB)

                    '--------------
                    '�̐��ʒu   
                    '�̐����擾
                    '�̐����̔��f�͎��{���Ȃ��̂ŕs�v
                    'GetSaisunInf(typDspDataBuf(nCnt), tcls_DB)
                Next

                m_nDspMaxIdx = nCnt - 1
                m_typDspData = typDspDataBuf
            End If

            ' �\���Ώۂ��r�����擾
            Dim nSetIdx As Integer = m_nDspStartIdx
            For ii As Integer = 0 To DSP_DEFMAP_CNT - 1
                If DSP_DEFMAP_DATCNT - 1 < nSetIdx Then
                    ' �擾����𒴉߂�����I��
                    Exit For
                End If

                If "" = m_typDspData(nSetIdx).strKizukenNo Then
                    ' �R�C���������Ȃ�I��
                    Exit For
                End If

                If True = m_typDspData(nSetIdx).bGetDefectFlg = True Then
                    ' �r���擾�ςȂ�X�L�b�v
                    nSetIdx += 1
                    Continue For
                End If

                m_typDspData(nSetIdx).bGetDefectFlg = True

                ' �r���擾
                GetDefectInf(nSetIdx + 1, tcls_DB, m_typDspData(nSetIdx).strKizukenNo, m_typDspData(nSetIdx))

                nSetIdx += 1
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�\���f�[�^�擾���s")

        Finally
            If sqlRead IsNot Nothing Then
                sqlRead.Close()
                sqlRead = Nothing
            End If

            If tcls_DB IsNot Nothing Then
                tcls_DB.Cancel()
                tcls_DB.Close()
                tcls_DB.Dispose()
                tcls_DB = Nothing
            End If
        End Try
    End Sub

    ''' <summary>
    ''' �J�b�g���擾
    ''' </summary>
    ''' <param name="typDspDataBuf">�\���p�f�[�^</param>
    ''' <param name="clsDB">SQL�T�[�o�[�ڑ��N���X</param>
    ''' <remarks></remarks>
    Private Function GetCutInf(ByRef typDspDataBuf As clsDefMap.DSP_DATA, ByRef clsDB As tClass.tClass_SQLServer) As Boolean
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""       'SQL��
        Dim bRet As Boolean = True      '�߂�l
        Dim nWk As Integer

        With typDspDataBuf
            strSql = ""
            strSql &= "SELECT"
            strSql &= " *"
            strSql &= " FROM"
            strSql &= " " & DB_COIL_CUT_POS
            strSql &= " WHERE"
            strSql &= " �Ǘ�No='" & .strKizukenNo & "'"
            strSql &= " AND �擪�ʒu > 0"
            strSql &= " ORDER BY �����q��"
            Try
                sqlRead = clsDB.SelectExecute(strSql)
                While sqlRead IsNot Nothing AndAlso sqlRead.Read()

                    Dim typCut As clsDefMap.CUT_DATA
                    typCut.initialize()

                    nWk = 0
                    If sqlRead.IsDBNull(sqlRead.GetOrdinal("�擪�ʒu")) Then
                        nWk = 0
                    Else
                        nWk = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�擪�ʒu")))
                    End If

                    '�J�b�g���L���𔻒�
                    If 0 >= nWk Then
                        typCut.cut_flg = 1      '�J�b�g������:�R�C����[(����ʒu0)
                    Else
                        typCut.cut_flg = 0      '�J�b�g������
                    End If

                    '�J�b�g�ʒu
                    typCut.Cut_Point = nWk

                    '�J�b�g�ʒu���ۑ�
                    .CutLineList.Add(typCut)
                End While

            Catch ex As Exception
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_COIL_CUT_POS & "]")
            Finally
                If sqlRead IsNot Nothing Then
                    sqlRead.Close()
                    sqlRead = Nothing
                End If

            End Try
        End With

    End Function

    ''' <summary>
    ''' �̐����擾
    ''' </summary>
    ''' <param name="typDspDataBuf">�\���p�f�[�^</param>
    ''' <param name="clsDB">SQL�T�[�o�[�ڑ��N���X</param>
    ''' <remarks></remarks>
    Private Function GetSaisunInf(ByRef typDspDataBuf As clsDefMap.DSP_DATA, ByRef clsDB As tClass.tClass_SQLServer) As Boolean
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""       'SQL��
        Dim bRet As Boolean = True      '�߂�l
        Dim nWk As Integer

        With typDspDataBuf
            strSql = ""
            strSql &= "SELECT "
            strSql &= "    TOP " + MAX_SHACUT_COUNT.ToString + " "
            strSql &= "    * "
            strSql &= "FROM "
            strSql &= "    " + DB_COIL_SAISUN_RESULT + " "
            strSql &= "WHERE "
            strSql &= "    �Ǘ�No = '" + .strKizukenNo + "' "
            strSql &= "ORDER BY "
            strSql &= "    ID DESC, �̐�No ASC "

            Try
                sqlRead = clsDB.SelectExecute(strSql)
                While sqlRead IsNot Nothing AndAlso sqlRead.Read()

                    Dim typSaisun As clsDefMap.SAISUN_DATA
                    typSaisun.initialize()

                    '�̐��J�n�ʒu
                    nWk = 0
                    If sqlRead.IsDBNull(sqlRead.GetOrdinal("�̐��J�n�ʒu")) Then
                        nWk = 0
                    Else
                        nWk = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�̐��J�n�ʒu")))
                    End If
                    typSaisun.Start_Point = nWk

                    '�̐��I���ʒu
                    nWk = 0
                    If sqlRead.IsDBNull(sqlRead.GetOrdinal("�̐��I���ʒu")) Then
                        nWk = 0
                    Else
                        nWk = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�̐��I���ʒu")))
                    End If
                    typSaisun.End_Point = nWk

                    '�̐����ۑ�
                    .SaisunList.Add(typSaisun)
                End While

            Catch ex As Exception
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_COIL_SAISUN_RESULT & "]")
            Finally
                If sqlRead IsNot Nothing Then
                    sqlRead.Close()
                    sqlRead = Nothing
                End If

            End Try
        End With

    End Function

    ''' <summary>
    ''' �r���擾
    ''' </summary>
    ''' <param name="strKizuKen"></param>
    ''' <remarks></remarks>
    Private Function GetDefectInf(ByVal nCoilIdx As Integer, ByRef clsDB As tClass.tClass_SQLServer, ByVal strKizuKen As String, ByRef dspData As clsDefMap.DSP_DATA) As Boolean
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        Dim strSql As String = ""       'SQL��
        Dim bRet As Boolean = True      '�߂�l
        Dim ii As Integer               '���[�v�J�E���^
        Dim nMaxCnt As Integer = 0      '���v�r��
        Dim nCnt As Integer             '�r��
        Dim strTableInf As String = ""  '�e�[�u��
        Dim strTableAttr As String = "" '�e�[�u��

        For nMen As Integer = 0 To NUM_MEN - 1
            dspData.DefectList(nMen).Clear()
        Next

        '�r���擾��
        Call DspWaitInf("�r���擾��" + vbCrLf + String.Format("(�R�C��{0}�{��)", nCoilIdx), 1)

        '�r���擾
        For ii = 0 To 1
            '�e�[�u������(0:�\,1:��)
            strTableInf = CStr(IIf(0 = ii, DB_DEFECT_INF_T, DB_DEFECT_INF_B))
            strTableAttr = CStr(IIf(0 = ii, DB_DEFECT_ATTR0_T, DB_DEFECT_ATTR0_B))

            'SQL���擾
            CreateDefectSql(strSql, strTableInf, strTableAttr, strKizuKen, CStr(dspData.typBaseData.nPCode(ii)), False, True)

            '���R�[�h�ǂݍ���
            nCnt = 0
            If Not clsDB.GetSelectKey(strSql, nCnt) Then
                gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r�����擾�G���[ [" & strTableInf & "]")
                Exit Function
            End If
            nMaxCnt += nCnt
        Next ii

        '�r���擾��
        Call DspWaitInf("�r���擾��" + vbCrLf + String.Format("(�R�C��{0}�{��)", nCoilIdx), nMaxCnt)

        '�r���擾

        Try
            nCnt = 1
            For ii = 0 To 1

                '�e�[�u������(0:�\,1:��)
                strTableInf = CStr(IIf(0 = ii, DB_DEFECT_INF_T, DB_DEFECT_INF_B))
                strTableAttr = CStr(IIf(0 = ii, DB_DEFECT_ATTR0_T, DB_DEFECT_ATTR0_B))

                'SQL���쐬
                CreateDefectSql(strSql, strTableInf, strTableAttr, strKizuKen, CStr(dspData.typBaseData.nPCode(ii)), True, True)

                ' Select�����s
                sqlRead = clsDB.SelectExecute(strSql)
                If sqlRead Is Nothing Then
                    gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & strTableInf & "]")
                    Return False
                End If

                While sqlRead.Read

                    '�r���Z�b�g
                    Call SetDefectData(dspData, sqlRead, ii, False)

                    '�󋵕\��
                    Call DspWaitProg(nCnt)
                    nCnt = nCnt + 1

                    Application.DoEvents()

                End While

                clsDB.Cancel()
                sqlRead.Close()
                sqlRead = Nothing
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r���擾�G���[ [" & ex.Message & "]")
            bRet = False

        Finally

            clsDB.Cancel()
            If Not sqlRead Is Nothing Then
                sqlRead.Close()
            End If
        End Try

        Return bRet

    End Function

    ''' <summary>
    ''' �r���擾SQL�쐬
    ''' </summary>
    ''' <param name="strSql">SQL��</param>
    ''' <param name="strTableInf">�e�[�u����(�r���)</param>
    ''' <param name="strTableAttr">�e�[�u����(������)</param>
    ''' <param name="strKizuKen">�Ǘ�No</param>
    ''' <param name="strPCode">PCode</param>
    ''' <param name="bMode">�擾���[�h(True:�ʏ�,False:��(COUNT(*)))</param>
    ''' <param name="bNagate">�������(True:F��T,False:T��F)</param>
    ''' <remarks></remarks>
    Private Sub CreateDefectSql(ByRef strSql As String, ByVal strTableInf As String, ByVal strTableAttr As String,
                                ByVal strKizuKen As String, ByVal strPCode As String, ByVal bMode As Boolean, ByVal bNagate As Boolean)

        Dim sbSql As New StringBuilder

        strSql = ""

        sbSql.AppendFormat("")

        If Not bMode Then
            sbSql.AppendFormat(" SELECT COUNT(*) ")
        Else
            sbSql.AppendFormat(" SELECT")
            sbSql.AppendFormat(" I.�rNo,")
            sbSql.AppendFormat(" I.�\���敪,")
            sbSql.AppendFormat(" I.�����r��,")
            sbSql.AppendFormat(" I.����O���[�h,")
            sbSql.AppendFormat(" I.����ʒu,")
            sbSql.AppendFormat(" ISNULL(I.��������Έʒu,0) As ��������Έʒu")
        End If

        sbSql.AppendFormat(" FROM ")
        sbSql.AppendFormat(" ( ")
        ' �r���(�w�肳�ꂽ�r��)
        sbSql.AppendFormat(" ( ")
        sbSql.AppendFormat(" SELECT ")
        sbSql.AppendFormat("     D.* ")
        sbSql.AppendFormat(" FROM ")
        sbSql.AppendFormat("     {0} D ", strTableInf)
        sbSql.AppendFormat("     INNER JOIN GRADE_NAME G ")
        sbSql.AppendFormat("     ON D.����O���[�h = G.EdasysId ")
        sbSql.AppendFormat("     INNER JOIN  ")
        sbSql.AppendFormat("     ( ")
        sbSql.AppendFormat("     SELECT ")
        sbSql.AppendFormat("         P.�r��EdasysId, ")
        sbSql.AppendFormat("         P.�����P���r�_�}�b�v��ʕ\��, ")
        sbSql.AppendFormat("         G.�D��x ")
        sbSql.AppendFormat("     FROM ")
        sbSql.AppendFormat("         M_PCODE_OUTPUT P ")
        sbSql.AppendFormat("         INNER JOIN GRADE_NAME G ")
        sbSql.AppendFormat("         ON P.�����P���r�_�}�b�v��ʕ\�� = G.EdasysId ")
        sbSql.AppendFormat("     WHERE ")
        sbSql.AppendFormat("         P.PCODE = '{0}' AND P.�r��EdasysId > -1 AND P.�����P���r�_�}�b�v��ʕ\�� > 0 ", strPCode)
        sbSql.AppendFormat("     ) F1 ")
        sbSql.AppendFormat("     ON F1.�r��EdasysId = D.�����r�� AND F1.�D��x >= G.�D��x ")
        sbSql.AppendFormat(" WHERE ")
        sbSql.AppendFormat("     D.�Ǘ�No = '{0}' ", strKizuKen)
        sbSql.AppendFormat(" )  ")


        sbSql.AppendFormat(" UNION ")

        ' �r���(���̑��r)
        sbSql.AppendFormat(" ( ")
        sbSql.AppendFormat(" SELECT ")
        sbSql.AppendFormat("     D.* ")
        sbSql.AppendFormat(" FROM ")
        sbSql.AppendFormat("     {0} D ", strTableInf)
        sbSql.AppendFormat("     INNER JOIN GRADE_NAME G ")
        sbSql.AppendFormat("     ON D.����O���[�h = G.EdasysId ")
        sbSql.AppendFormat("     INNER JOIN  ")
        sbSql.AppendFormat("     ( ")
        sbSql.AppendFormat("     SELECT ")
        sbSql.AppendFormat("         P.�r��EdasysId, ")
        sbSql.AppendFormat("         P.�����P���r�_�}�b�v��ʕ\��, ")
        sbSql.AppendFormat("         G.�D��x ")
        sbSql.AppendFormat("     FROM ")
        sbSql.AppendFormat("         M_PCODE_OUTPUT P ")
        sbSql.AppendFormat("         INNER JOIN GRADE_NAME G ")
        sbSql.AppendFormat("         ON P.�����P���r�_�}�b�v��ʕ\�� = G.EdasysId ")
        sbSql.AppendFormat("     WHERE ")
        sbSql.AppendFormat("         P.PCODE = '{0}' AND P.�r��EdasysId = -1 AND P.�����P���r�_�}�b�v��ʕ\�� > 0 ", strPCode)
        sbSql.AppendFormat("     ) F1 ")
        sbSql.AppendFormat("     ON F1.�D��x >= G.�D��x ")
        sbSql.AppendFormat("     INNER JOIN (   ")
        sbSql.AppendFormat("         SELECT ")
        sbSql.AppendFormat("             T.EdasysId ")
        sbSql.AppendFormat("         FROM ")
        sbSql.AppendFormat("             TYPE_NAME T ")
        sbSql.AppendFormat("         WHERE ")
        sbSql.AppendFormat("             EdasysId NOT IN ( ")
        sbSql.AppendFormat("                 SELECT ")
        sbSql.AppendFormat("                     �r��EdasysID ")
        sbSql.AppendFormat("                 FROM ")
        sbSql.AppendFormat("                     M_PCODE_OUTPUT ")
        sbSql.AppendFormat("                 WHERE ")
        sbSql.AppendFormat("                     PCODE = '{0}'  ", strPCode)
        sbSql.AppendFormat("                 AND �r��EdasysId > -1 ")
        sbSql.AppendFormat("             ) ")
        sbSql.AppendFormat("     ) F2 ")
        sbSql.AppendFormat("     ON D.�����r�� = F2.EdasysId  ")
        sbSql.AppendFormat(" WHERE ")
        sbSql.AppendFormat("     D.�Ǘ�No = '{0}' ", strKizuKen)
        sbSql.AppendFormat(" )  ")
        sbSql.AppendFormat(" )I ")

        ' WHERE
        sbSql.AppendFormat(" WHERE I.�d������ <> 1 ")

        If bMode Then
            If bNagate Then
                sbSql.AppendFormat(" order by I.����ʒu ASC")
            Else
                sbSql.AppendFormat(" order by I.����ʒu DESC")
            End If
        End If

        strSql = sbSql.ToString()

    End Sub

    ''' <summary>
    ''' �r���f�[�^�Z�b�g
    ''' </summary>
    ''' <param name="sqlRead">SQL�f�[�^</param>
    ''' <param name="nOuDiv">�\���敪</param>
    ''' <param name="bUpTrun">��������t���O(True:�t�],False:���])</param>
    ''' <remarks></remarks>
    Private Sub SetDefectData(ByRef dspData As clsDefMap.DSP_DATA, ByRef sqlRead As Data.SqlClient.SqlDataReader, ByVal nOuDiv As Integer, ByVal bUpTrun As Boolean)

        Dim typDefectInf As DETECT_BASE_DATA = Nothing  '�r���\����
        Dim nKizuNo As Integer = 0     '�rNo
        Dim typDefMap As clsDefMap.MAP_DATA = Nothing

        Try
            '�rNo 
            nKizuNo = CInt(sqlRead.GetValue(sqlRead.GetOrdinal("�rNo")))

            '�r���f�[�^�����Z�b�g
            With typDefectInf
                .TorB = CUShort(sqlRead.GetValue(sqlRead.GetOrdinal("�\���敪")))
                .nTid = CUShort(sqlRead.GetValue(sqlRead.GetOrdinal("�����r��")))
                .nGid = CUShort(sqlRead.GetValue(sqlRead.GetOrdinal("����O���[�h")))
                .y = CSng(sqlRead.GetValue(sqlRead.GetOrdinal("����ʒu")))
                .nKizuNo = nKizuNo

                '��������t�]�̏ꍇ�A�R�C�����������
                If bUpTrun Then .y = dspData.typBaseData.nCoilLen * 1000 - .y

            End With

            ''�r�}�b�v��ʕ\���p�r���擾

            '�\���̏�����
            typDefMap.initialize()

            With typDefMap
                .t_id = typDefectInf.nTid
                .g_id = typDefectInf.nGid
                '��������t�]���r�}�b�v��ʕ\�����ɍl������ׁA���]�O�����擾
                .y = CSng(sqlRead.GetValue(sqlRead.GetOrdinal("����ʒu")))
                .absX = CSng(sqlRead.GetValue(sqlRead.GetOrdinal("��������Έʒu")))
            End With

            '����ʒu�ő���r�ʒu��ۑ�
            If dspData.nMaxDefPos < CInt(typDefMap.y) Then dspData.nMaxDefPos = CInt(typDefMap.y)

            '�r�}�b�v�\�����ۑ�
            dspData.DefectList(nOuDiv).Add(typDefMap)

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�r���쐬�G���[ [" & ex.Message & "]")
            Exit Sub
        End Try
    End Sub


    ''' <summary>
    ''' �f�[�^�\��
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub DspData(Optional ByVal nDspStartIdx As Integer = 0, Optional ByVal nDspMen As Integer = 0)

        Dim strKensaO As String = ""        ' �����_�\
        Dim strKensaU As String = ""        ' �����_��
        Dim nCoilWid As Integer
        Dim nIndex As Integer = 0

        '�{�^��������
        Call EnableCoilChangeButton(False)

        '��ʏ������t���O����
        m_bInit = False

        '�ُ�K�C�_���X�\��������
        DspNgGuid(False)

        m_nDspStartIdx = nDspStartIdx
        m_nDspMen = nDspMen

        ' �\���ʕ\��
        If 0 = nDspMen Then
            lblFace.Text = "�\��"
            lblFace.ForeColor = Color.Yellow
            lblFace.BackColor = Color.Blue
        Else
            lblFace.Text = "����"
            lblFace.ForeColor = Color.Black
            lblFace.BackColor = Color.Orange
        End If

        '����{���\��
        nIndex = 0
        For ii As Integer = nDspStartIdx To m_typDspData.Length - 1
            With m_typDspData(ii)
                m_ctlMap(nIndex).lblCoilNo.Text = .typBaseData.strSeisanNo

                Dim dFontSize As Single = GetLabelFontScaleSize(m_ctlMap(nIndex).lblCoilNo, m_ctlMap(nIndex).lblCoilNo.Text, 16, m_ctlMap(nIndex).lblCoilNo.Width)
                m_ctlMap(nIndex).lblCoilNo.Font = New System.Drawing.Font("�l�r ����", dFontSize, System.Drawing.FontStyle.Bold, GraphicsUnit.Point)
            End With
            m_ctlMap(nIndex).lblName.Text = String.Format("{0}�{�O", ii + 1)

            nIndex += 1
            If nIndex >= DSP_DEFMAP_CNT Then
                Exit For
            End If
        Next

        '���t���b�V���C�x���g�ŕ\��
        Call DspBase()

        nIndex = 0
        For ii As Integer = nDspStartIdx To m_typDspData.Length - 1
            With m_typDspData(ii)
                nCoilWid = .typBaseData.nWidZai                          '�R�C�����擾

                If .nMaxDefPos < 0 Then
                    '�Œ��r�ʒu���}�C�i�X�l�ƂȂ�ꍇ��0���Z�b�g
                    .nMaxDefPos = 0
                End If

                '���r�}�b�v���\��
                '�r�}�b�v�ݒ�

                ' �R�C����
                If .nMaxDefPos <= .typBaseData.nCoilLen Then
                    mcls_DefMap(nIndex).tProp_nCoilLen = .typBaseData.nCoilLen       '�������������Z�b�g
                Else
                    mcls_DefMap(nIndex).tProp_nCoilLen = .nMaxDefPos                 '�Œ��r�ʒu���Z�b�g
                End If
                mcls_DefMap(nIndex).tProp_nCoilWid = nCoilWid                        '�R�C�����Z�b�g
                mcls_DefMap(nIndex).tProp_DefMapData = .DefectList(nDspMen)          '�r���Z�b�g
                mcls_DefMap(nIndex).tProp_nMode = nDspMen                            '�\���敪�Z�b�g
                mcls_DefMap(nIndex).tProp_bUpTurn = m_bUpTurn(nDspMen)               '�㉺���]�Z�b�g
                mcls_DefMap(nIndex).tProp_bSideTurn = m_bSideTurn(nDspMen)           '���E���]�Z�b�g
            End With

            m_ctlMap(nIndex).picMap.Tag = m_typDspData(ii)

            nIndex += 1
            If nIndex >= DSP_DEFMAP_CNT Then
                Exit For
            End If
        Next

        ' �c��̓N���A
        For ii As Integer = nIndex To DSP_DEFMAP_CNT - 1
            mcls_DefMap(ii).DspInit()
            m_ctlMap(ii).picMap.Tag = Nothing
            m_ctlMap(ii).lblName.Text = ""
            m_ctlMap(ii).lblCoilNo.Text = ""
        Next

        '�K�C�_���X�\��
        DspGuidMsgLabel(True)

        '�r�}�b�v�`��
        Call RefreshMap()

        '�K�C�_���X�\��
        DspGuidMsgLabel(False)

        ' �{�^���L����
        Call EnableCoilChangeButton(True)

    End Sub

    ''' <summary>
    ''' �t�H�[���N���[�Y
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmMain_FormClosing(ByVal sender As Object, ByVal e As System.Windows.Forms.FormClosingEventArgs) Handles Me.FormClosing
        ' ���C���X���b�h�I���ʒm
        modMain.TaskEnd()
    End Sub

    ''' <summary>
    ''' ����{�^���N���b�N
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub btnEnd_Click(ByVal sender As Object, ByVal e As System.EventArgs)

        Me.Close()

    End Sub

    ''' <summary>
    ''' �r�}�b�v �y�C���g�C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub picMap_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)

        '��ʃ��[�h���C�x���g�͖���
        If m_bLoad Then Exit Sub

        ' �`��ΏۃR���g���[���̎���
        Dim nIndex As Integer = CInt(CType(sender, PictureBox).Name.Replace("picMap", "")) - 1

        If CType(sender, PictureBox).Tag Is Nothing OrElse Not (TypeOf CType(sender, PictureBox).Tag Is clsDefMap.DSP_DATA) Then
            Exit Sub
        End If

        Dim dspData As clsDefMap.DSP_DATA = CType(CType(sender, PictureBox).Tag, clsDefMap.DSP_DATA)

        '�ΏۊO�h��ׂ�
        '�̐����̔��f�͎��{���Ȃ�
        'mcls_DefMap(nIndex).pinMapCutDrw(sender, e, dspData.SaisunList)

        '�r�}�b�v�x�[�X�\��
        mcls_DefMap(nIndex).picMapBase_Paint(sender, e)

        '�r�}�b�v�J�b�g�ʒu�\��
        If Not m_bInit Then
            mcls_DefMap(nIndex).picMapCut_Paint(sender, e, dspData.CutLineList)
        End If

        '�r�}�b�v�f�[�^�\���i�}�[�J�[�\���j
        mcls_DefMap(nIndex).picMapMarker_Paint(sender, e)
    End Sub

    ''' <summary>
    ''' �K�C�_���X���\��
    ''' </summary>
    ''' <param name="blnExec"></param>
    ''' <remarks></remarks>
    Private Sub DspGuidMsgLabel(ByVal blnExec As Boolean)

        '�f�[�^�擾���B�B�B
        pnlMsg.Visible = blnExec
        grpGuid.Visible = blnExec

        '�K�C�_���X�\��������
        If Not blnExec Then
            lblMsg.Text = ""
            prbMsg.Value = 0
        End If
        '���t���b�V���C�x���g�ŕ\��
        pnlMsg.Refresh()
        grpGuid.Refresh()
        lblMsg.Refresh()
        prbMsg.Refresh()

    End Sub

    ''' <summary>
    ''' �ُ팋�ʃK�C�_���X�\��
    ''' </summary>
    ''' <param name="blnExec"></param>
    ''' <remarks></remarks>
    Private Sub DspNgGuid(ByVal blnExec As Boolean)

        '�ُ팋�ʃK�C�_���X
        grpNgGuid.Visible = blnExec
        If Not blnExec Then lblNgMsg.Text = ""

        '���t���b�V���C�x���g�ŕ\��
        grpNgGuid.Refresh()
        lblNgMsg.Refresh()

    End Sub

    ''' <summary>
    ''' �f�[�^�擾�󋵕\����
    ''' </summary>
    ''' <param name="strVal"></param>
    ''' <param name="iMax"></param>
    ''' <remarks></remarks>
    Private Sub DspWaitInf(ByVal strVal As String, ByVal iMax As Long)
        lblMsg.Text = strVal
        prbMsg.Minimum = 0
        prbMsg.Maximum = CInt(IIf(iMax > prbMsg.Minimum, iMax, prbMsg.Minimum + 1))
        prbMsg.Value = 0

        '���t���b�V���C�x���g�ŕ\��
        pnlMsg.Refresh()
        grpGuid.Refresh()
        lblMsg.Refresh()
        prbMsg.Refresh()

    End Sub

    ''' <summary>
    ''' �f�[�^�擾���K�C�_���X �X�V
    ''' </summary>
    ''' <param name="nVal">�i���l</param>
    ''' <remarks></remarks>
    Private Sub DspWaitProg(ByVal nVal As Integer)

        '�i���l�Z�b�g
        prbMsg.Value = CInt(IIf(nVal < prbMsg.Maximum, nVal, prbMsg.Maximum))

    End Sub

    ''' <summary>
    ''' �r�}�b�v���щ�ʕ\���i��ʋN�����̃_�C�����O�\���Ή��j
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub tmrDsp_Tick(ByVal sender As Object, ByVal e As System.EventArgs) Handles tmrDsp.Tick
        '�����\���I���܂ŃC�x���g�𖳎�
        If Not m_bLoad = True Then
            '�^�C�}�[��~
            tmrDsp.Enabled = False
            Application.DoEvents()

            '�f�[�^�擾                
            Call GetDspData()

            '�r�}�b�v���щ�ʕ\��
            Call DspData()

            Me.BringToFront()
        End If
    End Sub

    ''' <summary>
    ''' �r�_�}�b�v�����ŐV�\��
    ''' </summary>
    ''' <param name="bReadCoil">�R�C�����ēǍ��t���O</param>
    ''' <remarks></remarks>
    Private Sub ForceDspNewData(ByVal bReadCoil As Boolean)
        '�������[�h�ɐؑ�
        Call ChangeRefreshMode(EM_REFRESH_MODE.AUTO)

        '�f�[�^�擾                
        Call GetDspData(bReadCoil)

        '�r�}�b�v���щ�ʕ\��
        Call DspData(0, m_nDspMen)
    End Sub


    ''' <summary>
    ''' �t�@���N�V�����{�^�������C�x���g
    ''' </summary>
    ''' <param name="FncNo">0�I���W��</param>
    ''' <param name="onKey">�{�^���N���b�N���������</param>
    ''' <remarks></remarks>
    Private Sub frmMain_tEv_FunctionButton(ByVal FncNo As Integer, ByVal onKey As System.Windows.Forms.Keys) Handles MyBase.tEv_FunctionButton

        '' �t�@���N�V�����ؑ�
        Select Case FncNo                       ' 0�I���W��
            Case EM_BTN_MODE.nPreCoil
                ' F1:�O�R�C��
                Call ChangeDspCoil(m_nDspStartIdx - 1)

            Case EM_BTN_MODE.nNextCoil
                ' F2:���R�C��
                Call ChangeDspCoil(m_nDspStartIdx + 1)

            Case EM_BTN_MODE.nChangeOU
                ' F3:���ʕ\���i�\�ʕ\���j
                Dim nDspMen As Integer = CInt(IIf(0 = m_nDspMen, 1, 0))
                Call ChangeDspMen(nDspMen)

            Case EM_BTN_MODE.nStop
                ' F5:�ꎞ��~
                Dim nRefreshMode As EM_REFRESH_MODE = CType(IIf(EM_REFRESH_MODE.AUTO = m_nRefreshMode, EM_REFRESH_MODE.PAUSE, EM_REFRESH_MODE.AUTO), EM_REFRESH_MODE)
                Call ChangeRefreshMode(nRefreshMode)

            Case EM_BTN_MODE.nPrint
                ' F10:��ʈ��
                MyBase.PrintOut()

            Case EM_BTN_MODE.nSave
                ' F11:��ʕۑ�
                MyBase.HardCopy(IMG_PATH, "�r�_�}�b�v���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)

            Case EM_BTN_MODE.nClose
                ' F12:����
                Me.Close()
        End Select

        '�Ăь��֒ʒm
        RaiseEvent tEv_FunctionButton(FncNo, onKey)

    End Sub

    ''' <summary>
    ''' �\���R�C���ؑ�
    ''' </summary>
    ''' <param name="nStartIdx">�\���J�nIndex</param>
    ''' <remarks></remarks>
    Private Sub ChangeDspCoil(ByVal nStartIdx As Integer)
        m_nDspStartIdx = nStartIdx

        ' �f�[�^�擾(�r���̂�)
        Call GetDspData(False)

        ' �\��
        Call DspData(nStartIdx, m_nDspMen)
    End Sub


    ''' <summary>
    ''' �\���ʐؑ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeDspMen(ByVal nDspMen As Integer)
        If 0 = nDspMen Then
            btnF03.Text = "���ʕ\��"
        Else
            btnF03.Text = "�\�ʕ\��"
        End If

        Call DspData(m_nDspStartIdx, nDspMen)
    End Sub

    ''' <summary>
    ''' �\�����[�h�ؑ�
    ''' </summary>
    ''' <param name="nRefreshMode"></param>
    ''' <remarks></remarks>
    Private Sub ChangeRefreshMode(ByVal nRefreshMode As EM_REFRESH_MODE)
        If EM_REFRESH_MODE.PAUSE = nRefreshMode Then
            ' �ꎞ��~
            m_nRefreshMode = EM_REFRESH_MODE.PAUSE
            Call EnableCoilChangeButton(True)
            lblMode.Text = "�ꎞ��~��"
            lblMode.BackColor = Color.Red
            lblMode.ForeColor = Color.White
            btnF05.Text = "��~����"
            btnF05.BackColor = Color.Red
            btnF05.ForeColor = Color.Yellow
        Else
            ' �����X�V
            m_nRefreshMode = EM_REFRESH_MODE.AUTO
            Call EnableCoilChangeButton(False)
            lblMode.Text = "�����X�V��"
            lblMode.BackColor = Color.Blue
            lblMode.ForeColor = Color.Yellow
            btnF05.Text = "�ꎞ��~"
            btnF05.BackColor = Color.FromKnownColor(KnownColor.Control)
            btnF05.ForeColor = Color.Black

            ' �ŐV�\��
            If m_nDspStartIdx > 0 Then
                Call DspData(0, m_nDspMen)
            End If
        End If
    End Sub

    ''' <summary>
    ''' �O�R�C���A���R�C���{�^������
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub EnableCoilChangeButton(ByVal bEnable As Boolean)
        If True = bEnable Then
            If EM_REFRESH_MODE.PAUSE = m_nRefreshMode Then
                If 0 >= m_nDspStartIdx Then
                    btnF01.Enabled = False
                Else
                    btnF01.Enabled = True
                End If

                If m_nDspMaxIdx - DSP_DEFMAP_CNT >= m_nDspStartIdx Then
                    btnF02.Enabled = True
                Else
                    btnF02.Enabled = False
                End If
            Else
                btnF01.Enabled = False
                btnF02.Enabled = False
            End If
        Else
            btnF01.Enabled = False
            btnF02.Enabled = False
        End If
    End Sub

    ''' <summary>
    ''' �p�����[�^�ύX����
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TranParam(ByVal nMode As Integer)

        Dim dele As New m_deleChangeParam(AddressOf ChangeParam)

        ' �ʃX���b�h����̃A�N�Z�X�\�Ƃ���
        If Me.InvokeRequired Then                   ' �ʃX���b�h����̃A�N�Z�X�v��
            Me.Invoke(dele, nMode)
        Else                                        ' �t�H�[�������X���b�h����̃A�N�Z�X�v��
            dele.DynamicInvoke()
        End If

    End Sub

    ''' <summary>
    ''' �p�����[�^�ύX
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub ChangeParam(ByVal nMode As Integer)

        ' �p�����[�^�ēǍ�
        mcls_ParamManager.ReadAll()

        ' ��ʍĕ`��
        Call ForceDspNewData(True)

    End Sub

    ''' <summary>
    ''' ��ʍX�V�ʒm
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub TranDspNew()

        Dim dele As New m_deleDspNew(AddressOf ForceDspNewData)

        ' �ʃX���b�h����̃A�N�Z�X�\�Ƃ���
        If Me.InvokeRequired Then                   ' �ʃX���b�h����̃A�N�Z�X�v��
            Me.Invoke(dele, True)
        Else                                        ' �t�H�[�������X���b�h����̃A�N�Z�X�v��
            dele.DynamicInvoke()
        End If

    End Sub

End Class
