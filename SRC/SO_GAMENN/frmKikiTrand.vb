'*******************************************************************************
'�@�@���ԃg�����h���
'	[Ver]
'		Ver.01    2014/02/27  ����
'
'	[����]
'		�@���ԃg�����h����ʂ̕\��
'*******************************************************************************
Option Strict On

Imports tClassLibrary
Imports tClassLibrary.tClass
Imports tClassLibrary.tCnt
Imports tClassLibrary.tMod

Public Class frmKikiTrand
    '/////////////////////////////////////////////////////////////////////////////////
    '�萔
    '/////////////////////////////////////////////////////////////////////////////////
    Private Const mc_MAX_GROUP As Integer = 5               '�O���t�O���[�v��
    Private Const mc_XScale As Integer = 10                 'X���X�P�[���Ԋu(����)
    Private Const mc_YScale As Integer = 10                 'Y���X�P�[���Ԋu(����)
    Private Const mc_ThinDspCnt As Integer = 5000           '�Ԉ����\����

    ' �����J���[�l
    Private ReadOnly sdcDefault() As System.Drawing.Color = {
        Color.Red, Color.Orange, Color.Yellow, Color.Green, Color.Lime,
        Color.GreenYellow, Color.Blue, Color.RoyalBlue, Color.Cyan, Color.DarkCyan,
        Color.Violet, Color.DeepPink, Color.DarkRed, Color.PaleVioletRed, Color.DarkGreen,
        Color.DarkSeaGreen, Color.PaleGreen, Color.DarkBlue, Color.Purple, Color.Magenta
    }

    '/////////////////////////////////////////////////////////////////////////////////
    '�񋓑�
    '/////////////////////////////////////////////////////////////////////////////////
    '�\�����ڗ�
    Private Enum em_DataColumnType
        iColor = 0                      ' Color
        iCheck                          ' Check
        iName                           ' Name
    End Enum

    '/////////////////////////////////////////////////////////////////////////////////
    '�\����
    '/////////////////////////////////////////////////////////////////////////////////
    ''' <summary>
    ''' �g�����h�O���t�\���e�[�u��
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_TREND_DSP
        Dim nNo As Integer                  'No
        Dim nKubun As Integer               '�@��敪
        Dim clsGraph As tClass_MapBase      '�O���t�G���A
        Dim dgvHanrei As DataGridView        '�}��z��
        Dim nudGraphMin As NumericUpDown    '�����l�z��
        Dim nudGraphMax As NumericUpDown    '����l�z��
        Dim txtGraphMin As TextBox          '�����l�z��
        Dim txtGraphMax As TextBox          '����l�z��
        Dim cmbCamLumi As ComboBox          '�@��
        Dim lblTaniMin As Label             '�����l�P��
        Dim lblTaniMax As Label             '����l�P��
        Dim grpGraph As GroupBox            '�O���[�v
        Dim picGraph As PictureBox          '�O���t�\���G���A
        Dim chkGraphProt As CheckBox        '�O���t�v���b�g�\���E��\��
        Dim cmbName As ComboBox             ' �g�����h��
        Dim nCnt As Integer                 '����
        Dim nCoilPos() As Integer           '�擾�ʒu
        Dim sTime() As String               '����
        Dim nData(,) As Single              '�f�[�^
        Dim nMin As Integer                 '�����l
        Dim nMax As Integer                 '����l
        Dim nThin As Integer                '�Ԉ�����
    End Structure

    ''' <summary>
    ''' �@��g�����h���
    ''' </summary>
    ''' <remarks></remarks>
    Private Structure TYP_TREND_INF
        Dim nKubun As Integer           '�@��敪
        Dim strDispGroup As String      '�\���O���[�v��
        Dim nMin As Integer             '�����l
        Dim nMax As Integer             '����l
        Dim nDecPlaces As Integer       '�����_�ʒu
        Dim strItemList As ArrayList    '���ږ����X�g
        '�C�j�V�����C�Y
        Public Sub initialize()
            nKubun = 0
            strDispGroup = String.Empty
            nMin = 0
            nMax = 0
            strItemList = New ArrayList
        End Sub
    End Structure

    '/////////////////////////////////////////////////////////////////////////////////
    '�����o �ϐ�
    '/////////////////////////////////////////////////////////////////////////////////
    Private m_bInitDate As Boolean = True                       '���������ݒ�[True:�ݒ蒆�AFalse:�ݒ芮]
    Private m_bInitKiki As Boolean = True                       '�@�평���ݒ�[True:�ݒ蒆�AFalse:�ݒ芮]
    Private m_bInitComb As Boolean = True                       '�@��R���{�����ݒ�[True:�ݒ蒆�AFalse:�ݒ芮]
    Private m_nKikiCount As Integer                             '�@�퐔
    Private m_typTrendDsp(mc_MAX_GROUP - 1) As TYP_TREND_DSP  '�g�����h�O���t�f�[�^
    Private m_strStartTime As String                            '�J�n����
    Private m_strEndTime As String                              '�I������
    Private m_nTimeCnt As Integer                               '�J�n-�I������
    Private m_bSetEnd As Boolean

    Private m_dctTrendInf As Dictionary(Of Integer, TYP_TREND_INF)
    Private m_nDefKiki(mc_MAX_GROUP - 1) As Integer             '�����\��No

    '/////////////////////////////////////////////////////////////////////////////////
    '�v���p�e�B
    '/////////////////////////////////////////////////////////////////////////////////

    '/////////////////////////////////////////////////////////////////////////////////
    '�����o �֐�
    '/////////////////////////////////////////////////////////////////////////////////
    Public Sub New()

        ' ���̌Ăяo���̓f�U�C�i�[�ŕK�v�ł��B
        InitializeComponent()

        ' InitializeComponent() �Ăяo���̌�ŏ�������ǉ����܂��B

        Dim s As Size
        s.Width = WIN_WIDTH_MAX
        s.Height = WIN_HEIGHT_MAX
        Me.MaximumSize = s
        s.Width = WIN_WIDTH_MIN
        s.Height = WIN_HEIGHT_MIN
        Me.MinimumSize = s

        m_dctTrendInf = New Dictionary(Of Integer, TYP_TREND_INF)

    End Sub

    ''' <summary>
    ''' �t�H�[���I������
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKikiTrand_Closing(ByVal sender As Object, ByVal e As System.ComponentModel.CancelEventArgs) Handles MyBase.Closing

        'ini�t�@�C����񏑍�
        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                PutTrendDsp(m_typTrendDsp(ii))
            Next ii

            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                '�O���t���񏑍�
                tMod.WriteIni(SO_GAMENN, "KIKITRANDNAME_" & CStr(ii + 1), LOCALINI_NAME, m_typTrendDsp(ii).cmbName.SelectedValue.ToString)
            Next ii
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�t�H�[���I������ ini�t�@�C����񏑍��ُ�[{0}],{1}", ex.Message, Me.Text))
        End Try

    End Sub

    ''' <summary>
    ''' �t�H�[�����[�h
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKikiTrand_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        Try
            Dim s As System.Windows.Forms.Screen = System.Windows.Forms.Screen.FromControl(Me.Owner)
            Me.Location = s.Bounds.Location
            Me.Height = Me.Owner.Height
            Me.Width = Me.Owner.Width

            '�^�C�g���ݒ�
            Call SetTitleName(Me)

            '���u��
            If NOA_ONLINE <> g_NOA Then
                '�t�H�[�����
                Me.tProp_FromFormatMode = gem_FromFormatMode.iOFFLINE
                Me.tProp_EndButtonEnable = False
            End If

            '�v�����^������΁A��ʈ��
            If tClass_HeadCopy.IsCheckPrinter Then
                btnF10.Enabled = True
            Else
                btnF10.Enabled = False
            End If

            '�@����擾
            GetKikiInf()

            '�@�헚���}�X�^�[�e�[�u���擾
            GetStHistoryMaster()

            '��ʏ����ݒ�
            SetInit()

            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                ' �R���{�{�b�N�X�I���C�x���g��ݒ�
                AddHandler m_typTrendDsp(ii).cmbName.SelectedIndexChanged, AddressOf cmbName_SelectedIndexChanged
            Next ii

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�t�H�[�����[�h�ُ�[{0}],{1}", ex.Message, Me.Text))
        End Try
    End Sub

    ''' <summary>
    ''' ���̉�ʂ̎�������C�x���g
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKikiTrand_Disposed(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Disposed
        Try
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�R�C���g�����h��� ��������ُ� [{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �@����擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetKikiInf()
        Try
            '������
            'm_typKikiInf.Clear()
            Dim typ As TYP_TREND_INF = Nothing
            m_dctTrendInf.Clear()

            '�@�퐔�擾
            m_nKikiCount = ReadIniInt(SO_GAMENN, "KIKI_COUNT", TASKINI_NAME)

            '�@����擾
            For ii As Integer = 0 To m_nKikiCount - 1
                '������
                Dim nStr As Integer = 1
                Dim nEnd As Integer = 0
                Dim nLen As Integer = 0
                Dim nItem As Integer = 0
                Dim strTmp As String
                Dim bEnd As Boolean = False
                Dim typWork As TYP_TREND_INF = Nothing
                typWork.initialize()
                '�@��ݒ�l�Ǎ�
                Dim strKikiInf As String = ReadIniStr(SO_GAMENN, "KIKI_INF_" & CStr(ii + 1), TASKINI_NAME)
                While True
                    nEnd = InStr(nStr, strKikiInf, ",")
                    If 0 = nEnd Then    '�ŏI����
                        bEnd = True
                        nLen = strKikiInf.Length - nStr + 1
                    Else                '�r������
                        nLen = nEnd - nStr
                    End If
                    strTmp = Mid(strKikiInf, nStr, nLen)
                    Select Case nItem
                        Case 0          '�敪
                            typWork.nKubun = CInt(strTmp)
                        Case 1          '�@��O���[�v��
                            typWork.strDispGroup = strTmp
                        Case 2          '�����l
                            typWork.nMin = CInt(strTmp)
                        Case 3          '����l
                            typWork.nMax = CInt(strTmp)
                        Case 4          '�����_�ʒu
                            typWork.nDecPlaces = CInt(strTmp)
                    End Select
                    nItem += 1
                    nStr = nEnd + 1
                    If bEnd Then
                        Exit While
                    End If
                End While
                m_dctTrendInf.Add(ii + 1, typWork)
            Next

            '�@�평���\���敪�擾
            For ii As Integer = 0 To m_nDefKiki.Count - 1
                m_nDefKiki(ii) = ReadIniInt(SO_GAMENN, "DEF_KIKI_" & CStr(ii + 1), TASKINI_NAME, ii + 1)
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�@����擾�G���[[{0}],{1}", ex.Message, Me.Text))
        End Try
    End Sub

    ''' <summary>
    ''' �@�헚���}�X�^�[�e�[�u���擾
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub GetStHistoryMaster()
        Dim strSQL As String
        Dim strItemName As ArrayList    '�@��}�X�^�[���ږ����X�g
        Dim nNameCount As Integer       '�@��}�X�^�[���̐�
        Dim clsDB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim clsRead As SqlClient.SqlDataReader = Nothing

        'DB�I�[�v��
        If Not clsDB.Open() Then
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ڑ��G���[")
            Exit Sub
        End If

        Try

            '�@��}�X�^�[���ږ����X�g
            strItemName = New ArrayList
            strItemName.Clear()

            '�@��}�X�^�[���ږ��擾
            strSQL = " SELECT"
            strSQL &= " NAME"
            strSQL &= " FROM  SYS.COLUMNS"
            strSQL &= " WHERE OBJECT_ID = OBJECT_ID('" & DB_ST_HISTORY_MASTER & "')"
            clsRead = clsDB.SelectExecute(strSQL)
            If clsRead Is Nothing Then
                Exit Try
            End If
            While clsRead.Read()
                strItemName.Add(CStr(clsRead.GetValue(0)))
            End While
            clsRead.Close()

            '�\�����ږ��̎擾
            nNameCount = 0
            For jj As Integer = 0 To strItemName.Count - 1
                If Mid(strItemName.Item(jj).ToString, 1, 2) = "����" Then
                    nNameCount += 1
                End If
            Next

            Dim keys As Integer() = New Integer(m_dctTrendInf.Keys.Count - 1) {}
            m_dctTrendInf.Keys.CopyTo(keys, 0)
            For ii As Integer = 0 To keys.Count - 1
                Dim typ As TYP_TREND_INF = m_dctTrendInf(keys(ii))

                strSQL = " SELECT"
                For jj As Integer = 0 To nNameCount - 1
                    If 0 = jj Then
                        strSQL &= " ����" & CStr(jj + 1)
                    Else
                        strSQL &= ",����" & CStr(jj + 1)
                    End If
                Next
                strSQL &= " FROM " & DB_ST_HISTORY_MASTER
                strSQL &= " WHERE �敪 = " & typ.nKubun.ToString
                clsRead = clsDB.SelectExecute(strSQL)
                If clsRead Is Nothing Then
                    Exit Try
                End If
                While clsRead.Read()
                    For jj As Integer = 0 To nNameCount - 1
                        If clsRead.IsDBNull(jj) Then
                            Continue For
                        End If
                        If String.Empty <> CStr(clsRead.GetValue(jj)).Trim() Then
                            typ.strItemList.Add(CStr(clsRead.GetValue(jj)))
                        End If
                    Next
                End While
                clsRead.Close()
                m_dctTrendInf.Remove(keys(ii))
                m_dctTrendInf.Add(keys(ii), typ)
            Next

        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�@�헚���}�X�^�[�e�[�u���擾�ُ�[{0}],{1}", ex.Message, Me.Text))
        Finally
            clsRead.Close()
            clsRead = Nothing
            clsDB.Close()
            clsDB.Dispose()
            clsDB = Nothing
        End Try
    End Sub

    ''' <summary>
    ''' ��ʏ����ݒ�
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetInit()

        Dim ii As Integer

        '�O���b�h�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).dgvHanrei = dgvHanrei1
        m_typTrendDsp(1).dgvHanrei = dgvHanrei2
        m_typTrendDsp(2).dgvHanrei = dgvHanrei3
        m_typTrendDsp(3).dgvHanrei = dgvHanrei4
        m_typTrendDsp(4).dgvHanrei = dgvHanrei5

        '�����l�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).nudGraphMin = nudGraphMin1
        m_typTrendDsp(1).nudGraphMin = nudGraphMin2
        m_typTrendDsp(2).nudGraphMin = nudGraphMin3
        m_typTrendDsp(3).nudGraphMin = nudGraphMin4
        m_typTrendDsp(4).nudGraphMin = nudGraphMin5

        '����l�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).nudGraphMax = nudGraphMax1
        m_typTrendDsp(1).nudGraphMax = nudGraphMax2
        m_typTrendDsp(2).nudGraphMax = nudGraphMax3
        m_typTrendDsp(3).nudGraphMax = nudGraphMax4
        m_typTrendDsp(4).nudGraphMax = nudGraphMax5

        '�s�N�`���[�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).picGraph = picGraph1
        m_typTrendDsp(1).picGraph = picGraph2
        m_typTrendDsp(2).picGraph = picGraph3
        m_typTrendDsp(3).picGraph = picGraph4
        m_typTrendDsp(4).picGraph = picGraph5

        '�O���t�v���b�g�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).chkGraphProt = chkGraphProt1
        m_typTrendDsp(1).chkGraphProt = chkGraphProt2
        m_typTrendDsp(2).chkGraphProt = chkGraphProt3
        m_typTrendDsp(3).chkGraphProt = chkGraphProt4
        m_typTrendDsp(4).chkGraphProt = chkGraphProt5

        '�O���t���R���{BOX�̃����o�[�ϐ��ݒ�
        m_typTrendDsp(0).cmbName = cmbName1
        m_typTrendDsp(1).cmbName = cmbName2
        m_typTrendDsp(2).cmbName = cmbName3
        m_typTrendDsp(3).cmbName = cmbName4
        m_typTrendDsp(4).cmbName = cmbName5


        '�C�x���g�n���h���֘A�t��
        For ii = 0 To m_typTrendDsp.Count - 1
            With m_typTrendDsp(ii)
                '�}�b�v������`
                .clsGraph = New tClass.tClass_MapBase
                MapInitialize(.clsGraph)

                '�R���{�{�b�N�X�ݒ�
                .nNo = tMod.ReadIniInt(SO_GAMENN, "KIKITRANDNAME_" & CStr(ii + 1), LOCALINI_NAME, m_nDefKiki(ii))
                cmbName_Init(.cmbName, .nNo)

                '�O���b�h�̏����ݒ�
                dgvInitialize(.dgvHanrei)

                '�C�x���g�n���h���֘A�t��
                '�}�b�v
                AddHandler .clsGraph.tEv_DrawPaint_End, AddressOf tEv_DrawPaint_End

                '�f�[�^�O���b�h�r���[
                AddHandler .dgvHanrei.CellContentClick, AddressOf dgvGraph_CellContentClick
                AddHandler .dgvHanrei.SelectionChanged, AddressOf dgvGraph_SelectionChanged

                '�s�N�`���[�{�b�N�X
                AddHandler .picGraph.Paint, AddressOf PictureBox_Paint

                '�`�F�b�N�{�b�N�X
                AddHandler .chkGraphProt.CheckedChanged, AddressOf chkGraphProt_CheckedChanged

                '�X�s�� �{�b�N�X (�A�b�v�_�E�� �R���g���[��)
                AddHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
                AddHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged

            End With

        Next ii

        '�J�n�I�����ԏ����\��
        dtpStartDay.Value = CDate(Format(Now, "yyyy/MM/dd") & " 00:00:00")
        dtpStartTime.Value = CDate(Format(Now, "yyyy/MM/dd") & " 00:00:00")
        dtpEndDay.Value = CDate(Format(Now, "yyyy/MM/dd") & " 23:59:59")
        dtpEndTime.Value = CDate(Format(Now, "yyyy/MM/dd") & " 23:59:59")

    End Sub

    ''' <summary>
    ''' �}�b�v��{����`
    ''' </summary>
    ''' <param name="MapBase">�O���t�}�b�v</param>
    ''' <remarks></remarks>
    Private Sub MapInitialize(ByVal MapBase As tClass_MapBase)
        '�O���t�`��ɕK�v�ȏ���ݒ�
        With MapBase
            '�\���̈���
            .tProp_AriaInf_BackGroundLineColor = Pens.Black
            .tProp_AriaInf_BackGroundColor = New SolidBrush(Color.White)
            .tProp_AriaInf_MapColor = New SolidBrush(Color.White)
            .tProp_AriaInf_AriaSize = picGraph1.Size
            .tProp_AriaInf_Margin = New Padding(40, 10, 35, 35)

            '�}�b�v�̈�
            .tProp_MapInf_IsCenterX = False
            .tProp_MapInf_IsCenterY = False
            .tProp_MapInf_IsBaseX_L = True
            .tProp_MapInf_IsBaseY_T = False
            .tProp_MapInf_IsStrechX = True
            .tProp_MapInf_IsStrechY = True
            .tProp_MapInf_LimitXmin = 1
            .tProp_MapInf_LimitXmax = 1000
            .tProp_MapInf_LimitYmin = 0
            .tProp_MapInf_LimitYmax = 100
            .tProp_MapInf_Offset = New tClass_MapBase.xyValue(0, 0)

            '�}�b�v�ΏۊO�̈�ݒ�
            .tProp_LimitInf_LimitDraw = False

            '�������ݒ�
            Dim p As Pen = New Pen(Color.LightBlue, 1)
            p.DashStyle = Drawing2D.DashStyle.DashDot
            .tProp_MemoriInf_LinePen = p
            .tProp_MemoriInf_StringFont = New Font("�l�r �S�V�b�N", 11, System.Drawing.FontStyle.Bold, GraphicsUnit.Pixel)
            .tProp_MemoriInf_StringBsh = New SolidBrush(Color.Black)
            .tProp_MemoriInf_PosStringX = 10
            .tProp_MemoriInf_PosStringY = 1
            .tProp_MemoriInf_ScaleInterval = New tClass_MapBase.xyValue(100, 10)
            .tProp_MemoriInf_ScaleStringFormatX = ""
            .tProp_MemoriInf_ScaleStringFormatY = "0"
            .tProp_MemoriInf_ScaleStringRange = New tClass_MapBase.RATE(1, 1)
            .tProp_MemoriInf_StringDisp_T = False
            .tProp_MemoriInf_StringDisp_B = False
            .tProp_MemoriInf_StringDisp_L = False
            .tProp_MemoriInf_StringDisp_R = False
            .tProp_MemoriInf_LineDsp_X = False
            .tProp_MemoriInf_LineDsp_Y = False
        End With
    End Sub

    ''' <summary>
    ''' ��ʃf�[�^�`�� (�ݒ�ύX���ɃC�x���g���s) (��ʕ\�����ɂ�)
    ''' </summary>
    ''' <remarks></remarks>
    Private Sub SetDsp(ByRef typDsp As TYP_TREND_DSP)

        Try

            ' ���̓f�[�^�`�F�b�N
            If False = InputDataCheck(typDsp) Then
                Exit Try
            End If

            ' �㉺���ێ�
            typDsp.nMax = CInt(typDsp.nudGraphMax.Value)
            typDsp.nMin = CInt(typDsp.nudGraphMin.Value)

            '''' �v���b�g�f�[�^���Z�b�g
            GetProtData(typDsp)

            '''' �}�b�v�̈�Ē�`
            MapReSetring(typDsp)

            '''' �}�b�v�Đ����˗�
            typDsp.clsGraph.DrawPaint()

            '''' �}�b�v�ĕ`��
            typDsp.picGraph.Refresh()

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("��ʃf�[�^�`�� ��O[{0}]", ex.Message))
        End Try

    End Sub

    ''' <summary>
    ''' ���̓f�[�^�`�F�b�N
    ''' </summary>
    ''' <param name="typDsp">�O���t�O���[�v�ԍ�(�O�I���W��)</param>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InputDataCheck(ByRef typDsp As TYP_TREND_DSP) As Boolean
        Dim strMin As String        ' dgv�̃f�[�^�i�[�p
        Dim strMax As String        ' dgv�̃f�[�^�i�[�p


        '�㉺�����͒l�`�F�b�N
        strMin = CStr(typDsp.nudGraphMin.Value)
        strMax = CStr(typDsp.nudGraphMax.Value)

        'NumericUpDown�ňُ펞�Ƀ��b�Z�[�W���o���Ɨ����邽�߁A�F�ւ������݂̂Ƃ���B
        If False = tClass.tClass_ImputCheck.CheckMinMax(strMin, strMax, 0, False, typDsp.cmbName.Text, False) Then
            typDsp.nudGraphMin.BackColor = Color.Red
            typDsp.nudGraphMax.BackColor = Color.Red
            Return False
        Else
            typDsp.nudGraphMin.BackColor = Color.White
            typDsp.nudGraphMax.BackColor = Color.White
        End If

        Return True
    End Function

    ''' <summary>
    ''' ���̓f�[�^�`�F�b�N
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Private Function InputDateCheck() As Boolean
        Dim strMsg As String

        If dtpStartDay.Value.ToString("yyyyMMdd") > dtpEndDay.Value.ToString("yyyyMMdd") Then
            strMsg = String.Format("�J�n���ƏI�������t�]���Ă��܂��B")
            MsgBox(strMsg, MsgBoxStyle.OkOnly, "���̓`�F�b�N")
            dtpStartDay.Select()
            dtpStartDay.Focus()
            Return False
        ElseIf dtpStartDay.Value.ToString("yyyyMMdd") = dtpEndDay.Value.ToString("yyyyMMdd") Then
            If dtpStartTime.Value.ToString("HHmmss") >= dtpEndTime.Value.ToString("HHmmss") Then
                strMsg = String.Format("�J�n���ԂƏI�����Ԃ��������t�]���Ă��܂��B")
                MsgBox(strMsg, MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                dtpStartTime.Select()
                dtpStartTime.Focus()
                Return False
            End If
        End If
        Return True

    End Function

    ''' <summary>
    ''' �}�b�v�Ē�`
    ''' </summary>
    ''' <param name="typGraph">�O���t�\���e�[�u��</param>
    ''' <remarks></remarks>
    Private Sub MapReSetring(ByVal typGraph As TYP_TREND_DSP)

        '�O���t�`��ɕK�v�ȏ���ݒ�
        With typGraph.clsGraph
            '�}�b�v�̈�
            .tProp_MapInf_LimitXmax = m_nTimeCnt
            .tProp_MapInf_LimitYmin = CInt(typGraph.nMin)
            .tProp_MapInf_LimitYmax = CInt(typGraph.nMax)

        End With
    End Sub

    ''' <summary>
    ''' dgv�̃Z���̃{�^���������ꂽ�Ƃ��̏���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGraph_CellContentClick(ByVal sender As Object, ByVal e As System.Windows.Forms.DataGridViewCellEventArgs)
        Dim dgv As DataGridView = CType(sender, DataGridView)

        Try
            '�s���N���b�N����Ă��Ȃ���ΏI��
            If 0 > e.RowIndex Then Exit Sub

            '��C���f�b�N�X�łǂ̗񂪃N���b�N���ꂽ������
            Select Case e.ColumnIndex
                Case em_DataColumnType.iColor  '�\���F
                    GetDlgColor(dgv, e.ColumnIndex, e.RowIndex)
                Case em_DataColumnType.iCheck
                    dgv(e.ColumnIndex, e.RowIndex).Value = dgv(e.ColumnIndex, e.RowIndex).EditedFormattedValue
                Case em_DataColumnType.iName
            End Select

            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                ' �O���t�O���[�v�̌���
                If sender Is m_typTrendDsp(ii).dgvHanrei Then
                    If 0 < m_typTrendDsp(ii).nCnt Then
                        ''''' �}�b�v�Đ����˗�
                        m_typTrendDsp(ii).clsGraph.DrawPaint()

                        ''''' �}�b�v�ĕ`��
                        m_typTrendDsp(ii).picGraph.Refresh()

                        Exit For
                    End If
                End If

            Next ii
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�O���b�h�N���b�N�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' dgv�̑I�����ύX���ꂽ��A�Z���̑I����Ԃ��L�����Z��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dgvGraph_SelectionChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Try
            Dim dgv As DataGridView = CType(sender, DataGridView)   '�Ώۂ�dgv
            dgv.CurrentCell.Selected = False
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�O���b�h�I��ύX�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �}�b�v�\���F�{�^���������ꂽ��A�F�I���_�C�A���O��\�����A�{�^���̐F��ύX
    ''' </summary>
    ''' <param name="dgv">�Ώۂ̃f�[�^�O���b�h�r���[</param>
    ''' <param name="nColIndex">�{�^���̗�C���f�b�N�X</param>
    ''' <param name="nRowIndex">�{�^���̍s�C���f�b�N�X</param>
    ''' <remarks></remarks>
    Private Sub GetDlgColor(ByVal dgv As DataGridView, ByVal nColIndex As Integer, ByVal nRowIndex As Integer)
        '�F�I���_�C�A���O�̐ݒ�
        '�F�̍쐬���g�p���Ȃ�
        dlgColor.AllowFullOpen = False

        '�F�I���_�C�A���O�̑I��F���A�{�^���̔w�i�F�ɂ���
        dlgColor.Color = dgv(nColIndex, nRowIndex).Style.BackColor

        '�F�I���_�C�A���O��ok�{�^���������ꂽ��A�{�^���̐F��ύX���� 
        If Windows.Forms.DialogResult.OK = dlgColor.ShowDialog() Then
            '�I��F���{�^���̔w�i�F��
            dgv(nColIndex, nRowIndex).Style.BackColor = dlgColor.Color
        End If

        dlgColor.Dispose()
    End Sub

    ''' <summary>
    ''' �`��
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' </summary>
    Private Sub PictureBox_Paint(ByVal sender As Object, ByVal e As System.Windows.Forms.PaintEventArgs)
        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                '�O���t�O���[�v�̌���
                If sender Is m_typTrendDsp(ii).picGraph Then
                    '�r�b�g�}�b�v�擾
                    Dim bmp As Bitmap = m_typTrendDsp(ii).clsGraph.tProp_Bmp
                    If bmp Is Nothing Then Return

                    '�w�i���Z�b�g
                    e.Graphics.DrawImage(bmp, 0, 0)
                End If
            Next ii
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�`��ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ��ʃf�[�^�`��
    ''' </summary>
    ''' <param name="cls"></param>
    ''' <param name="g"></param>
    ''' <remarks></remarks>
    Private Sub tEv_DrawPaint_End(ByVal cls As tClass_MapBase, ByVal g As System.Drawing.Graphics)
        Dim ii As Integer                   'INDEX
        Dim jj As Integer                   'INDEX
        Dim nPixel As Integer               '�}��X���ʒu
        Dim nGraphXInterval As Integer      'X���\������
        Dim nGraphXPoint As Integer         'X���\���|�C���^
        Dim nGraphYInterval As Integer      'Y���\������
        Dim nGraphYPoint As Integer         'Y���\���|�C���^
        Dim strWork As String               '���[�N�G���A
        Dim nTimeCnt As Integer             '���[�N�G���A ���Ԑ�


        ' �O���t�O���[�v�̌���
        For Each typ As TYP_TREND_DSP In m_typTrendDsp
            If cls IsNot typ.clsGraph Then
                Continue For
            End If

            'X���}��\��
            'X���X�P�[���Ԋu�Z�o
            nGraphXInterval = (m_nTimeCnt - 1) \ mc_XScale
            If 0 <> (m_nTimeCnt - 1) Mod mc_XScale Then
                nGraphXInterval += 1
            End If
            nGraphXPoint = 1
            For jj = 1 To m_nTimeCnt

                If jj = nGraphXPoint Or jj = m_nTimeCnt Then

                    'X�|�W�V�����擾
                    nPixel = cls.GetVtoP_X(jj)

                    '�ŏ��ƍŌ�͏c����\�����Ȃ�
                    If 1 <> jj AndAlso m_nTimeCnt <> jj Then
                        '��
                        g.DrawLine(cls.tProp_MemoriInf.LinePen, nPixel, cls.tProp_CalcAns.MapRect.Top + 1, nPixel, cls.tProp_CalcAns.MapRect.Bottom)

                    End If

                    '����
                    Dim cdtTime As Date = CDate(m_strStartTime).AddSeconds(jj - 1)
                    strWork = Format(cdtTime, "yyyy/MM/dd") & vbNewLine & Format(cdtTime, "HH:mm:ss")
                    cls.DrawString(g, nPixel, cls.tProp_CalcAns.MapRect.Bottom, 0, +cls.tProp_MemoriInf.nPosStringX, strWork, ContentAlignment.BottomCenter, cls.tProp_MemoriInf.StringFont, cls.tProp_MemoriInf.StringBsh)
                    nGraphXPoint += nGraphXInterval
                End If

            Next jj

            ''Y���}��\��
            'Y���X�P�[���Ԋu�Z�o
            nGraphYInterval = ((typ.nMax - typ.nMin) - 1) \ mc_YScale
            If 0 <> (typ.nMax - typ.nMin) - 1 Mod mc_YScale Then
                nGraphYInterval += 1
            End If
            nGraphYPoint = typ.nMin
            For jj = typ.nMin To typ.nMax
                If jj = nGraphYPoint Or jj = typ.nMax Then

                    'Y�|�W�V�����擾
                    nPixel = cls.GetVtoP_Y(jj)

                    '�ŏ��ƍŌ�͉�����\�����Ȃ�
                    If typ.nMin <> jj AndAlso typ.nMax <> jj Then
                        '��
                        g.DrawLine(cls.tProp_MemoriInf.LinePen, cls.tProp_CalcAns.MapRect.Left + 1, nPixel, cls.tProp_CalcAns.MapRect.Right, nPixel)

                    End If

                    '����
                    strWork = Format(jj, cls.tProp_MemoriInf.sScaleStringFormatY)
                    cls.DrawString(g, cls.tProp_CalcAns.MapRect.Left, nPixel, -cls.tProp_MemoriInf.nPosStringY, 0, strWork, ContentAlignment.MiddleLeft, cls.tProp_MemoriInf.StringFont, cls.tProp_MemoriInf.StringBsh)
                    nGraphYPoint += nGraphYInterval
                End If

            Next jj

            '�O���t�\���f�[�^=0�ŏI��
            If 0 = typ.nCnt Then Return

            '�`��
            Dim dVal As Single
            Dim x1, y1, x2, y2 As Integer
            Dim strDspTimeMax As String = ""            '�\���Z�o�p ���� �ő�
            Dim strDspTimeMin As String = ""            '�\���Z�o�p ���� �ŏ�
            Dim nDspDataMax As Single = 0              '�\���Z�o�p �f�[�^ �ő�
            Dim nDspDataMin As Single = 0              '�\���Z�o�p �f�[�^ �ŏ�
            Dim strDspTime(2 - 1) As String             '�\���p ����
            Dim nDspData(2 - 1) As Single              '�\���p �f�[�^
            Dim nThinCnt As Integer = 0                 '�Ԉ����p �J�E���^

            For ii = 0 To typ.dgvHanrei.RowCount - 1

                '�O���t�\���`�F�b�N ��\���Ŏ��̃O���t�i���j
                If False = CBool(typ.dgvHanrei(em_DataColumnType.iCheck, ii).Value) Then Continue For

                '�n�_�Z�o (�擪�̍��W�Z�o) X��
                strWork = CStr(CDate(typ.sTime(0)).ToBinary - CDate(m_strStartTime).ToBinary)
                If "0" = strWork Then
                    nTimeCnt = 0
                Else
                    nTimeCnt = CInt(strWork.Remove(strWork.Length - 7))
                End If
                x1 = cls.GetVtoP_X(nTimeCnt + 1)

                '�n�_�Z�o (�擪�̍��W�Z�o) Y��
                dVal = typ.nData(ii, 0)

                '�N�����v�ݒ�
                If dVal > typ.nMax Then dVal = typ.nMax
                If dVal < typ.nMin Then dVal = typ.nMin

                y1 = cls.GetVtoP_Y(dVal)

                Using p As New Pen(typ.dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor, 1)

                    '�v���b�g�̎w��
                    If typ.chkGraphProt.Checked Then
                        g.FillEllipse(New SolidBrush(p.Color), x1 - 3, y1 - 3, 6, 6)
                    End If

                    For jj = 0 To typ.nCnt - 1

                        If 2 >= typ.nThin Then
                            '�Ԉ����ݒ肪�Q�ȉ��̏ꍇ
                            '�z��Б��̂݃f�[�^��}��
                            strDspTime(0) = typ.sTime(jj)
                            nDspData(0) = typ.nData(ii, jj)
                            strDspTime(1) = ""
                            nDspData(1) = 0
                        Else
                            If 0 = nThinCnt Then
                                '�Ԉ����p�J�E���^���O(�Ԉ��������̍ŏ�)
                                '�ő�A�ŏ������Ƀf�[�^��}��
                                strDspTimeMax = typ.sTime(jj)
                                nDspDataMax = typ.nData(ii, jj)
                                strDspTimeMin = typ.sTime(jj)
                                nDspDataMin = typ.nData(ii, jj)
                                '�Ԉ����p�J�E���^ �C���N�������g
                                nThinCnt += 1
                                Continue For
                            ElseIf nThinCnt < typ.nThin Then
                                '�Ԉ����p�J�E���^���O(�Ԉ��������̍ŏ�)
                                If nDspDataMax <= typ.nData(ii, jj) Then
                                    '�ő�l�X�V
                                    strDspTimeMax = typ.sTime(jj)
                                    nDspDataMax = typ.nData(ii, jj)
                                ElseIf nDspDataMin >= typ.nData(ii, jj) Then
                                    '�ŏ��l�X�V
                                    strDspTimeMin = typ.sTime(jj)
                                    nDspDataMin = typ.nData(ii, jj)
                                End If
                                '�Ԉ����p�J�E���^ �C���N�������g
                                nThinCnt += 1
                                If nThinCnt <> typ.nThin And jj < typ.nCnt - 1 Then
                                    '�Ԉ����p�J�E���^���ݒ�l�ɓ��B���Ă��炸�A�f�[�^���l�`�w�ɂȂ��Ă��Ȃ�
                                    Continue For
                                Else
                                    '���Ԃɂ���ĕ\�����鏇�Ԃ����ւ���
                                    If CDate(strDspTimeMax).ToBinary > CDate(strDspTimeMin).ToBinary Then
                                        strDspTime(0) = strDspTimeMin
                                        nDspData(0) = nDspDataMin
                                        strDspTime(1) = strDspTimeMax
                                        nDspData(1) = nDspDataMax
                                    Else
                                        strDspTime(0) = strDspTimeMax
                                        nDspData(0) = nDspDataMax
                                        strDspTime(1) = strDspTimeMin
                                        nDspData(1) = nDspDataMin
                                    End If
                                    nThinCnt = 0
                                End If
                            End If
                        End If

                        For kk As Integer = 0 To 2 - 1
                            '�f�[�^�������Ă��Ȃ��ꍇ����
                            If strDspTime(kk) = "" Then Exit For
                            strWork = CStr(CDate(strDspTime(kk)).ToBinary - CDate(m_strStartTime).ToBinary)
                            If "0" = strWork Then
                                nTimeCnt = 0
                            Else
                                nTimeCnt = CInt(strWork.Remove(strWork.Length - 7))
                            End If
                            x2 = cls.GetVtoP_X(nTimeCnt + 1)

                            dVal = nDspData(kk)

                            If dVal > typ.nMax Then dVal = typ.nMax
                            If dVal < typ.nMin Then dVal = typ.nMin
                            y2 = cls.GetVtoP_Y(dVal)

                            '�`��
                            g.DrawLine(p, x1, y1, x2, y2)

                            '�v���b�g�̎w��
                            If typ.chkGraphProt.Checked Then
                                g.FillEllipse(New SolidBrush(p.Color), x2 - 3, y2 - 3, 6, 6)
                            End If

                            '���̏���
                            x1 = x2
                            y1 = y2
                        Next kk
                    Next jj
                End Using
            Next ii
            Exit For
        Next

    End Sub

    ''' <summary>
    ''' �v���b�g�\���`�F�b�N��ԕύX
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub chkGraphProt_CheckedChanged(ByVal sender As Object, ByVal e As System.EventArgs)
        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                '�O���t�O���[�v�̌���
                If sender Is m_typTrendDsp(ii).chkGraphProt Then
                    If 0 < m_typTrendDsp(ii).nCnt Then
                        '�}�b�v�Đ����˗�
                        m_typTrendDsp(ii).clsGraph.DrawPaint()

                        '�}�b�v�ĕ`��
                        m_typTrendDsp(ii).picGraph.Refresh()

                        Exit For
                    End If
                End If
            Next ii
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�v���b�g�\���ύX�ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �t�H�[�������\��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub frmKikiTrand_Shown(ByVal sender As Object, ByVal e As System.EventArgs) Handles Me.Shown
        Try
            '''' ��ʏ�����
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                DspInit(m_typTrendDsp(ii))
            Next ii

            m_bSetEnd = True

            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                SetDsp(m_typTrendDsp(ii))
            Next ii
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�t�H�[�������\���ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ��ʏ����\��
    ''' </summary>
    ''' <param name="typDsp">�O���t�\���e�[�u��</param>
    ''' <remarks></remarks>
    Private Sub DspInit(ByRef typDsp As TYP_TREND_DSP)

        With typDsp

            If Not m_dctTrendInf.ContainsKey(.nNo) Then
                '���݂��Ȃ��H
                Exit Sub
            End If

            Dim inf As TYP_TREND_INF = m_dctTrendInf(.nNo)

            ' �}����e�\��
            .dgvHanrei.RowCount = inf.strItemList.Count

            For ii As Integer = 0 To inf.strItemList.Count - 1

                ' �\���E��\��
                Dim nDisp As Integer = tMod.ReadIniInt(SO_GAMENN, "KIKITRANDDISP_" & CStr(.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME, 1)
                If 1 = nDisp Or 0 = nDisp Then
                    .dgvHanrei(em_DataColumnType.iCheck, ii).Value = nDisp
                Else
                    .dgvHanrei(em_DataColumnType.iCheck, ii).Value = 1
                End If

                ' �\���F
                Dim strColor As String = tMod.ReadIniStr(SO_GAMENN, "KIKITRANDCOLOR_" & CStr(.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME)
                If "" <> strColor Then
                    .dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32("FF" & strColor, 16))
                Else
                    .dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor = Color.FromArgb(Convert.ToInt32(Hex(sdcDefault(ii).ToArgb), 16))
                End If

                ' ����
                .dgvHanrei(em_DataColumnType.iName, ii).Value = inf.strItemList.Item(ii)
            Next ii

            '�n���h������U��������
            RemoveHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
            RemoveHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged

            .nudGraphMin.Maximum = inf.nMax
            .nudGraphMin.Minimum = inf.nMin
            .nudGraphMin.DecimalPlaces = inf.nDecPlaces
            If 0 < inf.nDecPlaces Then
                .nudGraphMin.Increment = CDec(1 / (10 ^ inf.nDecPlaces))
            Else
                .nudGraphMin.Increment = 1
            End If

            .nudGraphMax.Maximum = .nudGraphMin.Maximum
            .nudGraphMax.Minimum = .nudGraphMin.Minimum
            .nudGraphMax.DecimalPlaces = .nudGraphMin.DecimalPlaces
            .nudGraphMax.Increment = .nudGraphMin.Increment

            ' ����l
            .nudGraphMax.Text = tMod.ReadIniStr(SO_GAMENN, "KIKITRANDMAX_" & CStr(.nNo), LOCALINI_NAME)
            If False = IsNumeric(.nudGraphMax.Text) Then
                .nudGraphMax.Text = inf.nMax.ToString
            End If

            ' �����l
            .nudGraphMin.Text = tMod.ReadIniStr(SO_GAMENN, "KIKITRANDMIN_" & CStr(.nNo), LOCALINI_NAME)
            If False = IsNumeric(.nudGraphMin.Text) Then
                .nudGraphMin.Text = inf.nMin.ToString
            End If

            If False = tClass.tClass_ImputCheck.CheckMinMax(.nudGraphMin.Text, .nudGraphMax.Text, 0, False, .cmbName.Text, False) Then
                'NumericUpDown�ňُ�̏ꍇ�́A�f�t�H���g�ݒ�ɂ���
                .nudGraphMax.Text = inf.nMax.ToString
                .nudGraphMin.Text = inf.nMin.ToString
            End If

            '�n���h���Đݒ�
            AddHandler .nudGraphMin.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged
            AddHandler .nudGraphMax.ValueChanged, AddressOf nudGraphMaxMin_ValueChanged

        End With
    End Sub

    ''' <summary>
    ''' �g�����h�ݒ�ۑ�(Ini�t�@�C��������)
    ''' </summary>
    ''' <param name="typDsp"></param>
    Private Sub PutTrendDsp(ByVal typDsp As TYP_TREND_DSP)
        Dim strWk As String     ' �����񃏁[�N

        'ini�t�@�C����񏑍�
        For ii As Integer = 0 To typDsp.dgvHanrei.RowCount - 1
            '�\���E��\����񏑍�
            If True = CBool(typDsp.dgvHanrei(em_DataColumnType.iCheck, ii).Value) Then
                tMod.WriteIni(SO_GAMENN, "KIKITRANDDISP_" & CStr(typDsp.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME, "1")
            Else
                tMod.WriteIni(SO_GAMENN, "KIKITRANDDISP_" & CStr(typDsp.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME, "0")
            End If
        Next

        For ii As Integer = 0 To typDsp.dgvHanrei.RowCount - 1
            '�\���F��񏑍�
            strWk = Microsoft.VisualBasic.Right(Hex(typDsp.dgvHanrei(em_DataColumnType.iColor, ii).Style.BackColor.ToArgb), 6)
            tMod.WriteIni(SO_GAMENN, "KIKITRANDCOLOR_" & CStr(typDsp.nNo) & "_" & CStr(ii + 1), LOCALINI_NAME, strWk)
        Next

        '�㉺�����͒l�`�F�b�N
        Dim strMin As String = CStr(typDsp.nudGraphMin.Value)
        Dim strMax As String = CStr(typDsp.nudGraphMax.Value)

        If False = tClass.tClass_ImputCheck.CheckMinMax(strMin, strMax, 0, False, typDsp.cmbName.Text, False) Then
            'NumericUpDown�ňُ펞�̏ꍇ�́A�����݂��s��Ȃ�
        Else
            '�O���t����l����
            tMod.WriteIni(SO_GAMENN, "KIKITRANDMAX_" & CStr(typDsp.nNo), LOCALINI_NAME, typDsp.nudGraphMax.Text)

            '�O���t�����l����
            tMod.WriteIni(SO_GAMENN, "KIKITRANDMIN_" & CStr(typDsp.nNo), LOCALINI_NAME, typDsp.nudGraphMin.Text)
        End If
    End Sub

    '***************************************************************************
    ' �L�[�C�x���g
    '***************************************************************************
    ''' <summary>
    ''' �t�@���N�V�����L�[�C�x���g
    ''' </summary>
    ''' <param name="FncNo">�t�@���N�V����No 0�I���W��</param>
    ''' <param name="enable">�{�^���R���g���[���̗L������</param>
    ''' <param name="onKey"></param>
    ''' <remarks></remarks>
    Private Sub frmKikiTrand_tEv_FunctionButton(ByVal FncNo As Integer, ByVal enable As Boolean, ByVal onKey As System.Windows.Forms.Keys) Handles Me.tEv_FunctionButton
        '�t�@���N�V�����{�^���L�����ڂ̂�
        If Not enable Then
            Exit Sub
        End If

        Try
            Select Case FncNo
                Case 4                  'F05    ����
                    Search()
                Case 8                  'F09    CSV�o��
                    CsvOutTrand(CSV_PATH, "�@���ԃg�����h���.csv")
                Case 9                  'F10    ��ʈ��
                    PrintOut()
                Case 10                 'F11    ��ʕۑ�
                    HardCopy(IMG_PATH, "�@���ԃg�����h���_" & Format(Now, "yyyyMMdd_HHmmss") & ".jpg", Drawing.Imaging.ImageFormat.Jpeg)
                Case 11                 'F12    ���j���[
                    MyBase.ExecFromClose()
            End Select
        Catch ex As Exception
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�t�@���N�V���������ُ�[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' ���t�R���g���[���ɑ΂��� �z�C�[���ŉ��Z���Z���\�Ƃ���
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub dtp_MouseWheel(ByVal sender As Object, ByVal e As System.Windows.Forms.MouseEventArgs) Handles dtpStartDay.MouseWheel, dtpEndDay.MouseWheel, dtpStartTime.MouseWheel, dtpEndTime.MouseWheel
        '���݂�datetimepicker �Ɂ����L�[�������ꂽ���Ƃ���
        SendKeys.Send(CStr(IIf(e.Delta > 0, "{DOWN}", "{UP}")))
    End Sub

    '***************************************************************************
    '���W���[��
    '***************************************************************************
    ''' <summary>
    ''' �����`
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub DgvInit(ByVal dgv As DataGridView)

        With dgv
            .ColumnCount = 0
            .AutoGenerateColumns = False                          '�񎩓����� (false�ɂ����DataSource�������Ȃ��Ȃ�̂Œ���)
            .AllowUserToAddRows = False                           '���[�U�[ �s�ǉ�
            .AllowUserToDeleteRows = False                        '���[�U�[ �s�폜
            .AllowUserToOrderColumns = False                      '���[�U�[ ��ړ�
            .AllowUserToResizeColumns = False                     '���[�U�[ ��T�C�Y�ύX
            .AllowUserToResizeRows = False                        '���[�U�[ �s�T�C�Y�ύX
            .ShowCellErrors = False                               '�Z���G���[�̕\��
            .ShowCellToolTips = False                             '�c�[���q���g�̕\��
            .ShowEditingIcon = False                              '�ҏW���̃Z���̖���
            .ShowRowErrors = False                                '�G���[�s�̖���
            .ScrollBars = ScrollBars.Both                         '�X�N���[���o�[�̕\��
            .ColumnHeadersHeight = 42                             '�w�b�_�[�̍���
            .SelectionMode = DataGridViewSelectionMode.CellSelect '�Z���I��
            .MultiSelect = False                                  '�����s�I��
            .ReadOnly = False                                     '�ǂݎ���p�̓_��
            .EditMode = DataGridViewEditMode.EditOnEnter          '�Z����������
            .ColumnHeadersVisible = False                         '��w�b�_�\��
            .RowHeadersVisible = False                            '�s�w�b�_�\��
            .AutoSizeColumnsMode = DataGridViewAutoSizeColumnsMode.None '����T�C�Y��������
            .AutoSizeRowsMode = DataGridViewAutoSizeRowsMode.None '���s�T�C�Y��������
            .ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing  '
            .RowHeadersWidthSizeMode = DataGridViewRowHeadersWidthSizeMode.DisableResizing          '

            '��w�b�_�[ �����T�C�Y�ύX,�����F�ύX
            .ColumnHeadersDefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 10.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
            .ColumnHeadersDefaultCellStyle.ForeColor = Color.Black

            '�I���Z���F��`
            .DefaultCellStyle().SelectionBackColor = .DefaultCellStyle().BackColor

            .DefaultCellStyle().SelectionForeColor = Color.Black
            .AlternatingRowsDefaultCellStyle().BackColor = Color.AliceBlue  '1�s�����ɐF�ւ�
            .AlternatingRowsDefaultCellStyle().SelectionBackColor = Color.AliceBlue
            .DefaultCellStyle().Font = New System.Drawing.Font("�l�r ����", 12.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)

            .DefaultCellStyle().ForeColor = Color.Black

        End With

    End Sub

    ''' <summary>
    ''' ����dgv������
    ''' </summary>
    ''' <param name="dgv">�f�[�^�O���b�h�r���[</param>
    ''' <remarks></remarks>
    Private Sub dgvInitialize(ByVal dgv As DataGridView)

        '�����`
        DgvInit(dgv)

        With dgv
            '���`
            '1���
            Using clm As New DataGridViewButtonColumn
                clm.HeaderText = "�\���F"
                clm.Width = 45
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                clm.DefaultCellStyle.Font = New System.Drawing.Font("�l�r ����", 11.0, System.Drawing.FontStyle.Regular, GraphicsUnit.Point)
                .Columns.Add(clm)
            End Using

            '2���
            Using clm As New DataGridViewCheckBoxColumn
                clm.HeaderText = "�L������" & vbNewLine & "(�����F�L��)"

                clm.Width = 25
                clm.Frozen = False
                clm.ReadOnly = False
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter

                .Columns.Add(clm)
            End Using

            '3���
            Using clm As New DataGridViewTextBoxColumn
                clm.HeaderText = "����"
                clm.Width = 450
                clm.Frozen = False
                clm.ReadOnly = True
                clm.SortMode = DataGridViewColumnSortMode.NotSortable
                clm.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft
                clm.HeaderCell.Style.Alignment = DataGridViewContentAlignment.TopCenter
                .Columns.Add(clm)
            End Using

        End With

    End Sub

    ''' <summary>
    ''' ����
    ''' </summary>
    ''' <remarks></remarks>
    Public Sub Search()

        '���̓f�[�^�`�F�b�N
        If False = InputDateCheck() Then Exit Sub

        '���ԎZ�o
        '�J�n���Ԏ擾
        m_strStartTime = dtpStartDay.Value.ToString("yyyy/MM/dd ") & dtpStartTime.Value.ToString("HH:mm:ss")
        '�I�����Ԏ擾
        m_strEndTime = dtpEndDay.Value.ToString("yyyy/MM/dd ") & dtpEndTime.Value.ToString("HH:mm:ss")
        '�J�n-�I�����Ԏ擾
        Dim nFindTime As Long = CDate(m_strEndTime).ToBinary - CDate(m_strStartTime).ToBinary

        m_nTimeCnt = CInt(nFindTime \ 10000000) + 1

        For ii As Integer = 0 To m_typTrendDsp.Count - 1
            SetDsp(m_typTrendDsp(ii))
        Next ii

    End Sub

    ''' <summary>
    ''' �v���b�g�f�[�^�擾
    ''' </summary>
    ''' <param name="typGraph">�O���t�\���e�[�u��</param>
    ''' <remarks></remarks>
    Private Sub GetProtData(ByRef typGraph As TYP_TREND_DSP)

        Dim nGrpNo As Integer = typGraph.nNo    '�O���t�O���[�v�m��
        Dim nGrpNoCnt As Integer                'INDEX
        Dim strSQL As String                    'SQL��
        Dim sTime() As String                   'DB�擾�p �o�^���t
        Dim nData(,) As Single                 'DB�擾�p �@����
        Dim nCnt As Integer                     'DB�擾����


        typGraph.nCnt = nCnt

        If Not m_dctTrendInf.ContainsKey(typGraph.nNo) Then
            '���݂��Ȃ��H
            Exit Sub
        End If

        Dim inf As TYP_TREND_INF = m_dctTrendInf(typGraph.nNo)

        '�O���t�f�[�^������
        'SQL�ڑ�����
        Dim tcls_DB As New tClass_SQLServer(TASKINI_NAME, INI_DB)
        Dim sqlRead As Data.SqlClient.SqlDataReader = Nothing
        If Not tcls_DB.Open() Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "DB�ڑ��G���[")
            Exit Sub
        End If

        'SQL������
        Dim strKbn As String = String.Empty
        strKbn = inf.nKubun.ToString()
        strSQL = ""
        strSQL += " SELECT COUNT(*)"
        strSQL += " FROM " & DB_ST_HISTORY_INF
        strSQL += " WHERE �敪 = '" & strKbn & "'"
        strSQL += " AND �o�^���t >= '" & m_strStartTime & "'"
        strSQL += " AND �o�^���t <= '" & m_strEndTime & "'"

        '�f�[�^���擾
        tcls_DB.GetSelectKey(strSQL, nCnt)

        'SQL������
        strSQL = ""
        strSQL += " SELECT �o�^���t,"
        For nGrpNoCnt = 0 To typGraph.dgvHanrei.RowCount - 1
            strSQL += "        �@����_" & CStr(nGrpNoCnt + 1) & ","
        Next nGrpNoCnt
        strSQL = strSQL.Remove(strSQL.Length - 1, 1)
        strSQL += " FROM " & DB_ST_HISTORY_INF
        strSQL += " WHERE �敪 = '" & strKbn & "'"
        strSQL += " AND �o�^���t >= '" & m_strStartTime & "'"
        strSQL += " AND �o�^���t <= '" & m_strEndTime & "'"
        strSQL += " ORDER BY �o�^���t ASC"

        '�f�[�^�擾
        sqlRead = tcls_DB.SelectExecute(strSQL)
        If sqlRead Is Nothing Then
            gcls_Log.Write(tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_ST_HISTORY_INF & "]")
            Exit Sub
        End If

        ReDim Preserve sTime(nCnt - 1)
        ReDim Preserve nData(typGraph.dgvHanrei.RowCount - 1, nCnt - 1)

        nCnt = 0
        Try
            While sqlRead.Read()
                '�o�^���t�擾
                sTime(nCnt) = sqlRead.GetValue(0).ToString
                For nGrpNoCnt = 0 To typGraph.dgvHanrei.RowCount - 1
                    '�@���� �擾
                    If IsNumeric(sqlRead.GetValue(nGrpNoCnt + 1).ToString) Then
                        If inf.nDecPlaces > 0 Then
                            nData(nGrpNoCnt, nCnt) = CInt(sqlRead.GetValue(nGrpNoCnt + 1)) * CDec(1 / (10 ^ inf.nDecPlaces))
                        Else
                            nData(nGrpNoCnt, nCnt) = CInt(sqlRead.GetValue(nGrpNoCnt + 1).ToString)
                        End If
                    Else
                        nData(nGrpNoCnt, nCnt) = 0
                    End If
                Next nGrpNoCnt

                '�擾�����C���N�������g
                nCnt += 1
            End While
        Catch ex As Exception
            '�ُ펞�擾����=0
            nCnt = 0
            gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("�v���b�g�f�[�^�擾�G���[[{0}],{1}", ex.Message, Me.Text))
        Finally
            sqlRead.Close()
            sqlRead = Nothing
            tcls_DB.Close()
            tcls_DB.Dispose()
            tcls_DB = Nothing
        End Try

        typGraph.nCnt = nCnt
        typGraph.sTime = sTime
        typGraph.nData = nData
        typGraph.nThin = CInt(nCnt \ mc_ThinDspCnt)
        If 0 <> (nCnt Mod mc_ThinDspCnt) Then
            typGraph.nThin += 1
        End If

    End Sub

    ''' <summary>
    ''' �R���{�{�b�N�X
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub cmbName_SelectedIndexChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)

        Dim cmbData As ComboBox = CType(sender, ComboBox)

        If Not m_bSetEnd Then
            Exit Sub
        End If

        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                If cmbData IsNot m_typTrendDsp(ii).cmbName Then
                    Continue For
                End If

                PutTrendDsp(m_typTrendDsp(ii))

                m_typTrendDsp(ii).nNo = CType(cmbData.SelectedValue, Integer)

                DspInit(m_typTrendDsp(ii))

                SetDsp(m_typTrendDsp(ii))

            Next

        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�R���{�{�b�N�X ��O[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' CSV�o��
    ''' </summary>
    ''' <param name="strFolder">�ۑ��t�@�C���p�X</param>
    ''' <param name="strName">�ۑ��t�@�C����</param>
    ''' <remarks></remarks>
    Private Sub CsvOutTrand(ByVal strFolder As String, ByVal strName As String)

        Dim strFPath As String                          '�t���p�X
        Dim strMsg As String = ""                       '�ʒm���b�Z�[�W
        Dim strData As String = ""                      '1�s�̃f�[�^
        Dim nn As Integer                               'INDEX
        Dim nCntMax As Integer = 0                      '�f�[�^����MAX

        ''�t���p�X
        strFPath = IO.Path.Combine(strFolder, strName)

        ''�t�H���_�̗L���`�F�b�N(�����������낤)
        CheckFolder(strFolder)

        ''CSV�o��
        Using cls_Stram As New tClass_FileStream(strFPath, IO.FileMode.Create, IO.FileAccess.ReadWrite, IO.FileShare.ReadWrite)
            Try
                '�I�[�v��
                cls_Stram.Open()

                strData = ""
                '�w�b�_�[�o��(�g�����h���j
                For ii As Integer = 0 To mc_MAX_GROUP - 1
                    strData += m_typTrendDsp(ii).cmbName.Text & ","
                    For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                        strData += ","
                    Next jj
                    strData += ","
                Next ii

                '�Ō��","��r��
                strData = strData.Remove(strData.Length - 2, 2)

                '�b�r�u�֏�������
                cls_Stram.WriteLine(strData)

                strData = ""
                '�w�b�_�[�o��(�}�ᖼ�j
                For ii As Integer = 0 To mc_MAX_GROUP - 1
                    strData += "���t,"
                    For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                        strData += m_typTrendDsp(ii).dgvHanrei(em_DataColumnType.iName, jj).Value.ToString
                        strData += ","
                    Next jj
                    strData += ","
                Next ii

                '�Ō��","��r��
                strData = strData.Remove(strData.Length - 2, 2)

                '�b�r�u�֏�������
                cls_Stram.WriteLine(strData)

                '�f�[�^�����̍ő�l�擾
                For ii As Integer = 0 To mc_MAX_GROUP - 1
                    If nCntMax < m_typTrendDsp(ii).nCnt Then
                        nCntMax = m_typTrendDsp(ii).nCnt
                    End If
                Next ii

                '�f�[�^�o��
                nn = 0
                While nCntMax > nn
                    strData = ""
                    For ii As Integer = 0 To mc_MAX_GROUP - 1
                        '�f�[�^�o�͌������f�[�^�ő吔�ȉ��H
                        If m_typTrendDsp(ii).nCnt > nn Then
                            '�f�[�^�̐ݒ�
                            strData += m_typTrendDsp(ii).sTime(nn)
                            strData += ","
                            For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                                strData += CStr(m_typTrendDsp(ii).nData(jj, nn))
                                strData += ","
                            Next jj
                            strData += ","
                        Else
                            '�󔒂̐ݒ�
                            strData += ","
                            For jj As Integer = 0 To m_typTrendDsp(ii).dgvHanrei.RowCount - 1
                                strData += ","
                            Next jj
                            strData += ","
                        End If

                    Next ii
                    '�Ō��","��r��
                    strData = strData.Remove(strData.Length - 2, 2)

                    '�b�r�u�֏�������
                    cls_Stram.WriteLine(strData)

                    nn += 1
                    '
                End While

                '�N���[�Y
                cls_Stram.Close()
                strMsg = System.IO.Path.GetFullPath(strFPath) & "��CSV�t�@�C����ۑ����܂����B"

            Catch ex As Exception
                gcls_Log.Write(tClass_LogManager.LNO.ERR, String.Format("CSV�o�ُ͈�[{0}],{1}", ex.Message, Me.Text))
            End Try
        End Using

        '�K�C�_���X�\��
        Using frmMsg As New tClassForm.frmPop_MsgBox(strMsg, Me.Text)
            frmMsg.ShowDialog()
        End Using
    End Sub

    ''' <summary>
    ''' �A�b�v�_�E�� �R���g���[��
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="e"></param>
    ''' <remarks></remarks>
    Private Sub nudGraphMaxMin_ValueChanged(ByVal sender As System.Object, ByVal e As System.EventArgs)
        Dim nud As NumericUpDown = CType(sender, NumericUpDown)

        If Not m_bSetEnd Then
            Exit Sub
        End If

        Try
            For ii As Integer = 0 To m_typTrendDsp.Count - 1
                If nud IsNot m_typTrendDsp(ii).nudGraphMax AndAlso nud IsNot m_typTrendDsp(ii).nudGraphMin Then
                    Continue For
                End If

                ' ���̓f�[�^�`�F�b�N
                If False = InputDataCheck(m_typTrendDsp(ii)) Then
                    Exit Try
                End If

                SetDsp(m_typTrendDsp(ii))
            Next
        Catch ex As Exception
            gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, String.Format("�A�b�v�_�E�� �R���g���[�� ��O[{0}]", ex.Message))
        End Try
    End Sub

    ''' <summary>
    ''' �R���{�{�b�N�X�����ݒ�
    ''' </summary>
    ''' <param name="sender"></param>
    ''' <param name="nNo"></param>
    ''' <remarks></remarks>
    Private Sub cmbName_Init(ByVal sender As ComboBox, ByVal nNo As Integer)

        Dim ds As New DataTable()
        ds.Columns.Add("NAME", GetType(String))
        ds.Columns.Add("VALUE", GetType(Integer))

        '�g�����h���ɓo�^����Ă���O���[�v��
        Dim keys As Integer() = New Integer(m_dctTrendInf.Keys.Count - 1) {}
        m_dctTrendInf.Keys.CopyTo(keys, 0)
        For ii As Integer = 0 To keys.Count - 1
            Dim typ As TYP_TREND_INF = m_dctTrendInf(keys(ii))
            Dim row As DataRow = ds.NewRow()
            row("NAME") = typ.strDispGroup
            row("VALUE") = keys(ii)
            ds.Rows.Add(row)
        Next

        ds.AcceptChanges()
        sender.Items.Clear()
        sender.DataSource = ds
        sender.DisplayMember = "NAME"
        sender.ValueMember = "VALUE"
        sender.DropDownStyle = ComboBoxStyle.DropDownList
        sender.SelectedValue = nNo

    End Sub

End Class
