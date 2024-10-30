'*********************************************************************************
'	�e�L�X�g�t�@�C���Ƀ��O�o�͂��s�����N���X
'	[Ver]
'		Ver.01	2007/03/08	vs2005 �Ή� (�̂�tClass_Log��S�ʍ�蒼��)
'
'	[����]
'		Log�t�@�C�������݂ɓ��������N���X
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)
Imports System.IO                               ' IO����

'#Const tDEBUG = False
''#Const VBC_VER = 1

Namespace tClass
    ''' <summary>
    ''' �e�L�X�g�t�@�C���Ƀ��O�o�͂��s�����N���X
    ''' </summary>
    Public Class tClass_LogManager
        Implements IDisposable							' �f�X�g���N�^��`

        '********************************************************************
        '���J�ϐ���`
        '********************************************************************
        ''' <summary>
        ''' ���O����(ini�t�@�C����OutMode�ŏo�̗͂L���𐧌�\)
        ''' </summary>
        ''' <remarks></remarks>
        Public Enum LNO
            ''' <summary>����</summary>
            NON = 0
            ''' <summary>�� �K�C�_���X���Ɏw��</summary>
            MSG
            ''' <summary>�� �G���[���Ɏw��</summary>
            ERR
            ''' <summary>�� ��{�N���X���ł̃K�C�_���X���Ɏw��</summary>
            INF
            ''' <summary>�� ��{�N���X���ł̃G���[���Ɏw��</summary>
            WAR
        End Enum


        Private Const MAX_ROW_NUM As Integer = 20000		' 1�t�@�C��������̍s��
        Private Const MAX_LISTBOX As Integer = 200			' ���X�g�{�b�N�X�\���s��
        Private Const MAX_QUE As Integer = 512				' ���O�t�@�C���ő吔


        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private disposedValue As Boolean = False		' �d������Ăяo�������o����ɂ�
#If VBC_VER >= 8.0 Then							' Generic �ɑΉ����Ă���o�[�W������
        Private mque_Log As tClass_Queue(Of String)			' ���O�����Z�b�g���郊�X�g�v�f(�L���[�N���X)
#Else
        Private mque_Log As tClass_Queue					' ���O�����Z�b�g���郊�X�g�v�f(�L���[�N���X)
#End If
        Private m_wThread As tClass_WorkThread				' �����ݗp�X���b�h
        Private m_Encoding As System.Text.Encoding			' �G���R�[�f�B���O
        Private m_Stream As System.IO.StreamWriter			' �X�g���[��

        Private m_cntList As Windows.Forms.ListBox			' ���X�g�{�b�N�X�R���g���[���I�u�W�F�N�g
        Private m_bFileOut As Boolean						' �t�@�C���o�� (true:�t�@�C���o�͗L��)
        Private m_bListOut As Boolean						' ���X�g�{�b�N�X�R���g���[���o�� (true:���X�g�o�͗L��)


        '���O�t�@�C���֘A
        Private m_cBaseFolder As String						' �x�[�X�̃t�H���_
        Private m_cBaseFile As String						' �x�[�X�̃t�@�C����
        Private m_cNowFPath As String						' ���ݏo�͒��̃t�@�C���t���p�X

        Private m_nNowFileNo As Integer						' �t�@�C����
        Private m_nNowRow As Integer						' �s��
        Private m_nMaxCycleCnt As Integer					' �ő�T�C�N���b�N�t�@�C������
        Private m_nMaxBackCnt As Integer					' �ő�o�b�N�A�b�v����

        ' ListBox�ǉ��p
        Private Delegate Sub mDlgt_ListAdd(ByVal lst As Windows.Forms.ListBox, ByVal strLog() As String)	'���X�g�{�b�N�X�������ݗp�񓯊��f���Q�[�g


        '********************************************************************
        '�v���p�e�B��`
        '********************************************************************
        ''' <summary>
        ''' ���X�g�{�b�N�X�ւ̏o�͗L��
        ''' </summary>
        ''' <value>���X�g�\�������ɂ���ꍇ�́ANothing���w�肷��</value>
        ''' <remarks></remarks>
        Public WriteOnly Property tProp_SetListOut() As Windows.Forms.ListBox
            Set(ByVal value As Windows.Forms.ListBox)
                If value Is Nothing Then
                    m_bListOut = False
                Else
                    m_bListOut = True
                End If
                m_cntList = value
            End Set
        End Property
        ''' <summary>
        ''' �t�@�C���o�͗L��
        ''' </summary>
        ''' <value>���X�g�\�������ɂ���ꍇ�́ANothing���w�肷��</value>
        ''' <remarks></remarks>
        Public WriteOnly Property tProp_SetFileOut() As Boolean
            Set(ByVal value As Boolean)
                m_bFileOut = value
            End Set
        End Property
        ''' <summary>
        ''' ���݂̃t�@�C���t���p�X���o��
        ''' </summary>
        Public ReadOnly Property tProp_GetNowLogPath() As String
            Get
                Return m_cNowFPath
            End Get
        End Property

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="filename">�t�@�C���� (�ʏ�^�X�N��)  (Nothing�����O�o�͂��Ȃ�)</param>
        ''' <param name="folder">�t�H���_�p�X�w�� (�Ō��\�K�{) (�f�t�H���g�w��̎��́ANULL��OK)</param>	
        ''' <param name="list">ListBox�R���g���[�� (Nothing�����X�g�o�͂��Ȃ�)</param>
        ''' <param name="cycleCnt">�J��Ԃ��t�@�C����</param>
        ''' <param name="backCnt">�o�b�N�A�b�v��</param>				
        ''' <param name="encoding">�o�̓G���R�[�h</param>
        ''' <remarks>�o�C�i���o�͂̏ꍇ��,System.Text.Encoding.Unicode�ӂ���w��</remarks>
        Public Sub New(ByVal filename As String, Optional ByVal folder As String = "", Optional ByVal list As Windows.Forms.ListBox = Nothing, Optional ByVal cycleCnt As Integer = 3, Optional ByVal backCnt As Integer = 4, Optional ByVal encoding As System.Text.Encoding = Nothing)
            '' �����o�ϐ��Z�b�g
            m_nMaxCycleCnt = cycleCnt
            m_nMaxBackCnt = backCnt
            If encoding Is Nothing Then
                '�E�B���h�E�Y��VS�̕����`�����قȂ�ׁA�����I�ɃG���R�[�f�B���O���w��
                m_Encoding = System.Text.Encoding.GetEncoding("shift-jis")
            Else
                m_Encoding = encoding
            End If
            '' �o�͂��邵�Ȃ�����
            tProp_SetFileOut = CBool(IIf(filename = "", False, True))
            tProp_SetListOut = list

            '' �o�b�N�A�b�v (�t�@�C���o�͗L�莞�̂�)
            If m_bFileOut Then
                Dim ii As Integer
                Dim jj As Integer
                Dim cFileName As String
                Dim cFileNameNew As String

                ''�t�H���_�L��
                If folder = "" Then folder = ".\LOG\"
                If Not IO.Directory.Exists(folder) Then
                    IO.Directory.CreateDirectory(folder)
                End If
                m_cBaseFolder = folder
                m_cBaseFile = filename


                '' bk_xxx_@9_0.log �` bk_xxx_@9_9.log���폜  (�ŌÃt�@�C���폜)
                For ii = 0 To m_nMaxCycleCnt - 1
                    cFileName = m_cBaseFolder & "bk_" & m_cBaseFile & "_@" & m_nMaxBackCnt & "_" & ii & ".log"
                    IO.File.Delete(cFileName)
                Next ii

                '' bk_xxx_@0_0.log �` bk_xxx_@0_9.log �� bk_xxx_@1_0.log �` bk_xxx_@1_9.log �ɕύX (�o�b�N�A�b�v�t�@�C�����l�[��)
                For ii = 0 To m_nMaxCycleCnt - 1
                    For jj = m_nMaxBackCnt To 1 Step -1
                        cFileName = m_cBaseFolder & "bk_" & m_cBaseFile & "_@" & jj - 1 & "_" & ii & ".log"		' �ϊ���
                        cFileNameNew = m_cBaseFolder & "bk_" & m_cBaseFile & "_@" & jj & "_" & ii & ".log"	   ' �ϊ���

                        IO.File.Delete(cFileNameNew)			' �ړ���Ɋ��Ƀt�@�C��������ƃG���[�ɂȂ邩���x����
                        If IO.File.Exists(cFileName) Then IO.File.Move(cFileName, cFileNameNew) '�ړ����̃t�@�C��������ꍇ�̂�
                    Next jj
                Next ii

                '' xxx_0.log�`xxx_9.log �� bk_xxx_@0_0.log �` bk_xxx_@0_9.log �ɕύX (�ŐV�t�@�C�����o�b�N�A�b�v�t�@�C���Ƀ��l�[��)
                For ii = 0 To m_nMaxCycleCnt - 1
                    cFileName = m_cBaseFolder & m_cBaseFile & "_" & ii & ".log"		  ' �ϊ���
                    cFileNameNew = m_cBaseFolder & "bk_" & m_cBaseFile & "_@0_" & ii & ".log"	   ' �ϊ���

                    IO.File.Delete(cFileNameNew)			' �ړ���Ɋ��Ƀt�@�C��������ƃG���[�ɂȂ邩���x����
                    If IO.File.Exists(cFileName) Then IO.File.Move(cFileName, cFileNameNew) '�ړ����̃t�@�C��������ꍇ�̂�
                    'IO.File.Move(cFileName, cFileNameNew)
                Next ii


                '' ���O�t�@�C���I�[�v��
                cFileName = String.Format("{0}{1}_{2}.log", m_cBaseFolder, m_cBaseFile, m_nNowFileNo)
                m_Stream = New StreamWriter(cFileName, False, m_Encoding)
                m_Stream.AutoFlush = True
                m_cNowFPath = cFileName
            End If

            '''' �C���X�^���X����
#If VBC_VER >= 8.0 Then							' Generic �ɑΉ����Ă���o�[�W������
            mque_Log = New tClass_Queue(Of String)(MAX_QUE)
#Else
            mque_Log = New tClass_Queue(MAX_QUE)
#End If
            m_wThread = New tClass_WorkThread

            '''' ���C���X���b�h���s
            '������ƗD��x���߂ɐݒ� 
            '(���߂ɐݒ肷��� �L���[�ɂ��܂藭�܂炸 �������񐔂̃t�@�C���I�[�v���N���[�Y�����������)
            m_wThread.ThreadStart(AddressOf mainThread, Threading.ThreadPriority.Lowest, "tClass_LogManager", True)
        End Sub

        ' IDisposable
        Protected Overridable Sub Dispose(ByVal disposing As Boolean)
            If Not Me.disposedValue Then
                If disposing Then
                    ' TODO: �����I�ɌĂяo���ꂽ�Ƃ��ɃA���}�l�[�W ���\�[�X��������܂�
                End If

                ' TODO: ���L�̃A���}�l�[�W ���\�[�X��������܂�
                Try
                    '' �C���X�^���X�m�F
                    If Not m_wThread Is Nothing Then
                        '' ���C���X���b�h�I��
                        m_wThread.ThreadStop(500)
                        m_wThread.Dispose()
                        m_wThread = Nothing
                    End If
                    mque_Log.Dispose()
                    If Not m_Stream Is Nothing Then
                        m_Stream.Close()
                        m_Stream.Dispose()
                        m_Stream = Nothing
                    End If
                Catch ex As Exception
                Finally
                End Try
            End If
            Me.disposedValue = True
        End Sub

#Region " IDisposable Support "
        ''' <summary>
        ''' Dispose�f�X�g���N�^
        ''' </summary>
        ''' <remarks>�g�p��A�K���R�[�����鎖</remarks>
        Public Sub Dispose() Implements IDisposable.Dispose
            ' ���̃R�[�h��ύX���Ȃ��ł��������B�N���[���A�b�v �R�[�h����� Dispose(ByVal disposing As Boolean) �ɋL�q���܂��B
            Dispose(True)
            GC.SuppressFinalize(Me)
        End Sub
#End Region

        ''' <summary>
        ''' Finalize�f�X�g���N�^
        ''' </summary>
        ''' <remarks></remarks>
        Protected Overrides Sub Finalize()
            Dispose()
            MyBase.Finalize()
        End Sub

        ''' <summary>
        ''' �V�X���O��������
        ''' </summary>
        ''' <param name="no">�o��LNO</param>
        ''' <param name="strMsg">���b�Z�[�W</param>
        ''' <remarks>�񓯊���������</remarks>
        Public Overloads Sub Write(ByVal no As LNO, ByVal strMsg As String)
            Dim strLog As String
            Dim strTemp As String

            '' �V���{���t��
            Select Case no
                Case LNO.NON
                    strTemp = ""
                Case LNO.MSG
                    strTemp = "�� "
                Case LNO.ERR
                    strTemp = "�� "
                Case LNO.INF
                    strTemp = "�� "
                Case LNO.WAR
                    strTemp = "�� "
                Case Else
                    strTemp = ""
            End Select

            '' ���t�t��
            strLog = String.Format("{0:yyyy/MM/dd HH:mm:ss.fff} {1}{2}", Now(), strTemp, strMsg)
            '' �L���[�ɓo�^
            If 0 = mque_Log.GetFreeCount() Then		'�󂫖����̈� ���O�j��
                mque_Log.Free()

                strLog = String.Format("{0:yyyy/MM/dd HH:mm:ss.fff} {1}{2}", Now(), "�� ", "���O�L���[�t���I�I")
            End If

            '' �񓯊��L���[�ɒǉ�
            mque_Log.SetItem(strLog)
        End Sub


        ''' <summary>
        ''' ���C���X���b�h
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub mainThread()
            Dim EventAry(1) As Threading.WaitHandle		'�C�x���g
            Dim iRetc As Integer						'WaitAny ���A���

            '''' �V�O�i���Z�b�g
            EventAry(0) = m_wThread.g_evStop			' �X���b�h�I��
            EventAry(1) = mque_Log.g_evSem				' �L���[�L��

            Try
                While (True)
                    '' �V�O�i���҂�
                    iRetc = Threading.WaitHandle.WaitAny(EventAry)

                    '' �܂��I���m�F
                    Select Case iRetc
                        Case 0
                            tProp_SetListOut = Nothing		'�Ō�̈� ���X�g�\���͖���
                            '' �Ō�ɃL���[�ɗ��܂��Ă��郍�O�������I�ɓf���o��
                            LogSet()
                            Exit While

                        Case 1

                            ''Log�t�@�C���ɒǉ�
                            LogSet()
                        Case Else
                            '���肦�Ȃ�
                    End Select
                End While
            Catch ex As Threading.ThreadAbortException
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' �V�X���O�ɏ�������
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub LogSet()
            '' �L���[�ɗ��܂��Ă���S�Ẵf�[�^�擾
#If VBC_VER >= 8.0 Then							' Generic �ɑΉ����Ă���o�[�W������
            Dim objLog() As String		'���O�z��
            objLog = mque_Log.GetAll
            If objLog Is Nothing Then Exit Sub
#Else
            Dim o() As Object
            o = mque_Log.getAll()
            If o Is Nothing Then Exit Sub
            ''string�z�� �ɕϊ�
            Dim objLog(o.Length - 1) As String      '���O�z��
            For ii As Integer = 0 To o.Length - 1
                objLog(ii) = CStr(o(ii))
            Next ii
#End If


            '' ���X�g�{�b�N�X�ǉ�
            If m_bListOut Then
                Try
                    Dim args(1) As Object
                    args(0) = m_cntList
                    args(1) = objLog
                    m_cntList.BeginInvoke(New mDlgt_ListAdd(AddressOf lstWrite), args)
                Catch ex As Exception

                End Try
            End If

            '' �t�@�C���o��
            If m_bFileOut Then WriteFile(objLog)
        End Sub


        ''' <summary>
        ''' �t�@�C���o��
        ''' </summary>
        ''' <param name="strLog"></param>
        ''' <remarks></remarks>
        Private Sub WriteFile(ByVal strLog() As String)
            For ii As Integer = 0 To strLog.Length - 1
                '�t�@�C���o��
                m_Stream.WriteLine(strLog(ii))
                m_nNowRow += 1

                '�t�@�C���؂�ւ�
                If m_nNowRow > MAX_ROW_NUM Then
                    m_Stream.Close()
                    m_Stream.Dispose()
                    m_Stream = Nothing

                    m_nNowRow = 0
                    m_nNowFileNo += 1
                    If m_nNowFileNo > m_nMaxCycleCnt Then m_nNowFileNo = 0
                    Dim cFileName As String = String.Format("{0}{1}_{2}.log", m_cBaseFolder, m_cBaseFile, m_nNowFileNo)
                    m_Stream = New StreamWriter(cFileName, False, m_Encoding)
                    m_Stream.AutoFlush = True
                    m_cNowFPath = cFileName
                End If
            Next ii
        End Sub


        '********************************************************************
        ' ���X�g�{�b�N�X�ǉ��p
        '********************************************************************
            ''' <summary>
        ''' �񓯊���������
        ''' </summary>
        ''' <param name="lst">�������ݑΏۃR���g���[��</param>
        ''' <param name="strLog">�ǉ����O�z��</param>
        ''' <remarks></remarks>
        Private Sub lstWrite(ByVal lst As Windows.Forms.ListBox, ByVal strLog() As String)
            Dim ii As Integer
            Dim delCnt As Integer	'�폜�s��
            Dim addItem() As String
            Try
                '''' �p�t�H�[�}���X�΍� ��1�i
                If MAX_LISTBOX < strLog.Length Then		'�ő�\���s�����ǉ��s���̕��������ꍇ
                    ReDim addItem(MAX_LISTBOX - 1)
                    Array.Copy(strLog, strLog.Length - MAX_LISTBOX, addItem, 0, MAX_LISTBOX)
                    delCnt = MAX_LISTBOX
                Else
                    addItem = strLog
                    If lst.Items.Count >= MAX_LISTBOX Then		'�ő匏�����B�ς�
                        delCnt = strLog.Length
                    Else										'�ő匏�������B
                        delCnt = strLog.Length - (MAX_LISTBOX - lst.Items.Count)
                        If delCnt < 0 Then delCnt = 0
                    End If
                End If

                '''' �p�t�H�[�}���X�΍� ��2�i
                lst.BeginUpdate()
                '�܂��폜
                If delCnt = MAX_LISTBOX Then
                    lst.Items.Clear()
                Else
                    For ii = 1 To delCnt
                        lst.Items.RemoveAt(0)
                    Next
                End If
                '�ǉ�
                lst.Items.AddRange(addItem)
                '�ŐV�s��I����ԂɕύX
                lst.SelectedIndex = lst.Items.Count - 1
                lst.EndUpdate()
            Catch ex As Exception

            End Try
        End Sub

        ''' <summary>
        ''' �������N��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ShowEdit()
            Dim strCmd As String = "notepad.exe " & m_cNowFPath
            Shell(strCmd, AppWinStyle.NormalFocus, False)
        End Sub


    End Class
End Namespace