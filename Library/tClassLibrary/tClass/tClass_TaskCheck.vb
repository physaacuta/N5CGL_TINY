'*********************************************************************************
'�^�X�N�̋N���E��~�E�`�F�b�N���𐧌䂷��N���X
'	[Ver]
'		Ver.01    2005/12/30  vs2005 �Ή�
'
'	[����]
'
'*********************************************************************************
Option Strict On                                ' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Namespace tClass
    ''' <summary>
    ''' �^�X�N�̋N���E��~�E�`�F�b�N���𐧌䂷��N���X
    ''' </summary>
    ''' <remarks></remarks>
    Public Class tClass_TaskCheck
        Implements IDisposable                          ' �f�X�g���N�^��`

        '********************************************************************
        '�萔��`
        '********************************************************************
        ''' <summary>
        ''' �^�X�N�����s�����ꍇ�ɃR�[��(�񓯊��f���Q�[�g)
        ''' </summary>
        ''' <param name="sender">���C���X�^���X</param>
        ''' <param name="e">���s�����^�X�N��</param>
        ''' <remarks></remarks>
        Public Delegate Sub tDele_TaskCheck_Event(ByVal sender As Object, ByVal e As String)

        ''' <summary>
        ''' �`�F�b�NExe���\����
        ''' </summary>
        ''' <remarks></remarks>
        Private Structure typTaskinf
            ''' <summary>���sExe�t���p�X</summary> 
            Dim strFPath As String
            ''' <summary>�^�X�N����</summary> 
            Dim strName As String
            ''' <summary>���s�X�^�C��</summary> 
            Dim style As AppWinStyle
        End Structure

        '********************************************************************
        '�����o�[�ϐ���`
        '********************************************************************
        Private disposedValue As Boolean = False        ' �d������Ăяo�������o����ɂ�
        Private m_wThread As tClass_WorkThread			' ���C���X���b�h
        Private m_evGo As Threading.AutoResetEvent		' �������s�C�x���g
        Private m_deleEvent As tDele_TaskCheck_Event	' �^�X�N�����s�������ɃR�[��
        Private m_Interval As Integer                   ' �^�C�}���� [ms]
        Private m_pTaskInf As ArrayList					' �`�F�b�NExe���

        '********************************************************************
        '���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �R���X�g���N�^
        ''' </summary>
        ''' <param name="deleMethod">�R�[���o�b�N�֐��A�h���X(addressof �֐���)</param>
        ''' <param name="iniFPath">ini�t�@�C���t���p�X</param>
        ''' <param name="iniSession">�Z�b�V����</param>
        ''' <remarks></remarks>
        Public Sub New(ByVal deleMethod As tDele_TaskCheck_Event, ByVal iniFPath As String, ByVal iniSession As String)
            '''' �����o�ϐ��Z�b�g
            m_deleEvent = deleMethod
            m_pTaskInf = New ArrayList

            '''' ini�t�@�C����荞��
            m_Interval = tMod.tMod_IniFile.ReadIniInt(iniSession, "Interval", iniFPath, 0)		 ' �^�C�}���� [s]
            If m_Interval <= 0 Then Throw New ArgumentOutOfRangeException("�^�C�}����")
            m_Interval = m_Interval * 1000  '[s]��[ms]
            Dim cnt As Integer
            cnt = tMod.tMod_IniFile.ReadIniInt(iniSession, "CheckMaxCount", iniFPath, 0)
            If cnt < 0 Then Throw New ArgumentOutOfRangeException("�ő吔�ُ�")
            For ii As Integer = 1 To cnt         '���擾
                Dim inf As New typTaskinf
                inf.strFPath = tMod.tMod_IniFile.ReadIniStr(iniSession, String.Format("PATH_{0:000}", ii), iniFPath, "")
                If inf.strFPath.Length <= 0 Then Throw New ArgumentOutOfRangeException("�t�@�C���p�X�ُ�")
                inf.strName = tMod.tMod_IniFile.ReadIniStr(iniSession, String.Format("NAME_{0:000}", ii), iniFPath, "")
                If inf.strName.Length <= 0 Then Throw New ArgumentOutOfRangeException("�`�F�b�N���ُ̈�")
                Dim wk As Integer
                wk = tMod.tMod_IniFile.ReadIniInt(iniSession, String.Format("MODE_{0:000}", ii), iniFPath, -1)
                If wk <> 0 And wk <> 1 And wk <> 2 And wk <> 3 Then Throw New ArgumentOutOfRangeException("�\���X�^�C���ُ�")
                Select Case wk
                    Case 0  '�ʏ�
                        inf.style = AppWinStyle.NormalFocus
                    Case 1  '�ŏ� 
                        inf.style = AppWinStyle.MinimizedNoFocus
                    Case 2  '�ő�
                        inf.style = AppWinStyle.MaximizedFocus
                    Case 3  '��\��
                        inf.style = AppWinStyle.Hide
                End Select
                '���X�g�ɓo�^
                m_pTaskInf.Add(inf)
            Next ii

            '''' �C���X�^���X����
            m_evGo = New Threading.AutoResetEvent(False)
            m_wThread = New tClass_WorkThread
        End Sub
        ''' <param name="iniFPath">ini�t�@�C���t���p�X</param>
        ''' <param name="iniSession">�Z�b�V����</param>
        Public Sub New(ByVal iniFPath As String, ByVal iniSession As String)
            Me.New(Nothing, iniFPath, iniSession)
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
                    If m_wThread Is Nothing Then Exit Sub
                    '' ���C���X���b�h�I��
                    m_wThread.ThreadStop(5000)
                    m_wThread.Dispose()
                    m_wThread = Nothing
                    m_evGo.Close()
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
        ''' �X���b�h���s
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ThreadStart()
            m_wThread.ThreadStart(AddressOf ThreadMain, Threading.ThreadPriority.Normal, "tClass_TaskCheck", False)
        End Sub

        ''' <summary>
        ''' �X���b�h�I��
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub ThreadStop()
            m_wThread.ThreadStop(5000)
        End Sub

        ''' <summary>
        ''' �^�X�N�`�F�b�N�������s
        ''' </summary>
        ''' <remarks></remarks>
        Public Sub taskCheckGo()
            m_evGo.Set()
        End Sub

        ''' <summary>
        ''' ���C���X���b�h
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub ThreadMain()
            Dim EventAry(1) As Threading.WaitHandle     '�C�x���g
            Dim iRetc As Integer                        'WaitAny ���A���

            '''' �V�O�i���Z�b�g
            EventAry(0) = m_wThread.g_evStop
            EventAry(1) = m_evGo

            '''' �Ƃ肠�����������s
            m_evGo.Set()

            '''' ���C�����[�v
            Try
                While (True)
                    '''' �V�O�i���҂�
                    iRetc = Threading.WaitHandle.WaitAny(EventAry, m_Interval, False)

                    '''' �܂��I���m�F
                    Select Case iRetc
                        Case 0
                            Exit While
                        Case Else   '�������s,�^�C���A�E�g��
                            ' �^�X�N�`�F�b�N
                            Me.subTaskCheck()
                    End Select
                End While
            Catch ex As Threading.ThreadAbortException
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' �^�X�N�L���`�F�b�N
        ''' </summary>
        ''' <remarks></remarks>
        Private Sub subTaskCheck()
            Dim inf As typTaskinf

            '''' �o�^���ꂽ�f�[�^���`�F�b�N
            For Each inf In m_pTaskInf
                If Not TaskUmuCheck(inf.strName) Then
                    '''' �^�X�N�N��
                    StartExe(inf.strFPath, inf.style)
                    '''' �C�x���g�ʒm
                    If Not m_deleEvent Is Nothing Then
                        m_deleEvent.BeginInvoke(Me, inf.strName, Nothing, Nothing)
                    End If
                End If
            Next
        End Sub

 
        '********************************************************************
        '�ÓI���\�b�h��`
        '********************************************************************
        ''' <summary>
        ''' �v���Z�X���~���[�e�b�N�X�ɓo�^ (2�d�N���`�F�b�N)
        ''' </summary>
        ''' <param name="hMutex">Mutex�I�u�W�F�N�g</param>
        ''' <returns>���A��� (0:���N�������� 1:�N���ς� -1:�ُ�)</returns>
        ''' <remarks>�o�^���閼�̂� �A�v���P�[�V������</remarks>
        Public Shared Function ProcessStart(ByRef hMutex As System.Threading.Mutex) As Integer
            return ProcessStart(System.Windows.Forms.Application.ProductName, hMutex)
        End Function
        ''' <param name="strProductName">�~���[�e�b�N�X�ɓo�^����v���Z�X����</param>
        ''' <param name="hMutex">Mutex�I�u�W�F�N�g</param>
        ''' <returns>���A��� (0:���N�������� 1:�N���ς� -1:�ُ�)</returns>
        Public Shared Function ProcessStart(ByVal strProductName As String, ByRef hMutex As System.Threading.Mutex) As Integer
            hMutex = New System.Threading.Mutex(False, strProductName)    ' �~���[�e�b�N�X

            '''' 2�d�N���`�F�b�N
            Try
                'Mutex �̃V�O�i���҂�
                If hMutex.WaitOne(0, False) Then
                    GC.KeepAlive(hMutex)         'hMutex ���K�x�[�W �R���N�V�����Ώۂ��珜�O (����厖)
                    Return 0
                Else
                    Return 1
                End If
            Catch ex As Exception
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' �~���[�e�b�N�X���J��
        ''' </summary>
        ''' <param name="hMutex"></param>
        ''' <remarks></remarks>
        Public Shared Sub ProcessEnd(ByVal hMutex As System.Threading.Mutex)
            '''' �I�u�W�F�N�g�I������
            Try
                hMutex.Close()              'Mutex ���J������
            Catch ex As Exception
            End Try
        End Sub

        ''' <summary>
        ''' �^�X�N�����邩�`�F�b�N
        ''' </summary>
        ''' <param name="strTaskName">�^�X�N�� (�^�X�N�}�l�[�W���[�̃C���[�W��)</param>
        ''' <returns>Ture:���� False:���Ȃ�</returns>
        ''' <remarks></remarks>
        Public Shared Function TaskUmuCheck(ByVal strTaskName As String) As Boolean
            Dim pAry() As Process = Process.GetProcesses
            Dim p As Process
            Dim umu As Boolean = False

            '''' �v���Z�X�`�F�b�N
            For Each p In pAry
                Try
                    '' �t�@�C�����`�F�b�N
                    If strTaskName = p.MainModule.ModuleName Then
                        umu = True
                        p.Dispose()
                        Exit For
                    End If
                Catch ex As Exception
                    ' MainModule ���������̂������
                End Try
                p.Dispose()
            Next p
            Return umu
        End Function

        ''' <summary>
        ''' �^�X�N���̂���v���Z�X���̂ɕύX����
        ''' </summary>
        ''' <param name="strTaskName">�^�X�N�� (�^�X�N�}�l�[�W���[�̃C���[�W��)</param>
        ''' <returns>�擾�����v���Z�X����</returns>
        ''' <remarks></remarks>
        Public Shared Function getProcessName(ByVal strTaskName As String) As String
            Dim pAry() As Process = Process.GetProcesses
            Dim p As Process
            Dim strProcessName As String = ""

            '''' �v���Z�X�`�F�b�N
            For Each p In pAry
                '' �t�@�C�����`�F�b�N
                If strTaskName = p.MainModule.ModuleName Then
                    strProcessName = p.ProcessName
                    p.Dispose()
                    Exit For
                End If
                p.Dispose()
            Next p
            Return strProcessName
        End Function

        ''' <summary>
        ''' �v���Z�X�������I��
        ''' </summary>
        ''' <param name="strProcessName">�v���Z�X��</param>
        ''' <remarks></remarks>
        Public Shared Sub Kill(ByVal strProcessName As String)
            Dim pAry() As Process
            Dim p As Process

            '''' �Y���v���Z�X�������v���Z�X���擾 (���ꖼ�̂̃v���Z�X�����邩���m��Ȃ��הz�񂪋A��)
            pAry = Process.GetProcessesByName(strProcessName)
            For Each p In pAry
                p.Kill()        '�v���Z�X�������I��
                p.Dispose()
            Next
        End Sub

        ''' <summary>
        ''' �E�B���h�E�������Ă���v���Z�X���I��������
        ''' </summary>
        ''' <param name="strProcessName">�v���Z�X��</param>
        ''' <remarks></remarks>
        Public Shared Sub CloseWindow(ByVal strProcessName As String)
            Dim pAry() As Process
            Dim p As Process
            '''' �Y���v���Z�X�������v���Z�X���擾 (���ꖼ�̂̃v���Z�X�����邩���m��Ȃ��הz�񂪋A��)
            pAry = Process.GetProcessesByName(strProcessName)
            For Each p In pAry
                p.CloseMainWindow()     '�v���Z�X���I������
                p.Dispose()
            Next
        End Sub

        ''' <summary>
        ''' �w��^�X�N�̋N��
        ''' </summary>
        ''' <param name="strFPath">�N���^�X�N�̃t���p�X</param>
        ''' <returns>�N���v���Z�XID</returns>
        ''' <remarks></remarks>
        Public Overloads Shared Function StartExe(ByVal strFPath As String) As Integer
            Return StartExe(strFPath, AppWinStyle.NormalFocus)
        End Function
        ''' <param name="strPath">�N���^�X�N�̃p�X</param>
        ''' <param name="strExeName">�N���^�X�N����</param>
        ''' <returns>�N���v���Z�XID</returns>
        Public Overloads Shared Function StartExe(ByVal strPath As String, ByVal strExeName As String) As Integer
            Return StartExe(System.IO.Path.Combine(strPath, strExeName), AppWinStyle.NormalFocus)
        End Function
        ''' <param name="strPath">�N���^�X�N�̃p�X</param>
        ''' <param name="strExeName">�N���^�X�N����</param>
        ''' <param name="style">�N����� (Hide=0 NormalFocus=1 MinimizedFocus=2 MaximizedFocus=3 NormalNoFocus=4 MinimizedNoFocus=6)</param>
        ''' <returns>�N���v���Z�XID</returns>
        Public Overloads Shared Function StartExe(ByVal strPath As String, ByVal strExeName As String, ByVal style As AppWinStyle) As Integer
            Return StartExe(System.IO.Path.Combine(strPath, strExeName), style)
        End Function
        ''' <param name="strFPath">�N���^�X�N�̃t���p�X</param>
        ''' <param name="style">�N����� (Hide=0 NormalFocus=1 MinimizedFocus=2 MaximizedFocus=3 NormalNoFocus=4 MinimizedNoFocus=6)</param>
        ''' <returns>�N���v���Z�XID</returns>
        Public Overloads Shared Function StartExe(ByVal strFPath As String, ByVal style As AppWinStyle) As Integer
            Try
                '�ȒP�N����
                Return Microsoft.VisualBasic.Shell(strFPath, style)
            Catch ex As Exception
                Return 0
            End Try
        End Function
        ''' <param name="info">�N�����</param>
        ''' <returns>�N���v���Z�X</returns>
        Public Overloads Shared Function StartExe(ByVal info As ProcessStartInfo) As Process
            Try
                '''' ���낢��N���R�}���h���ݒ�\
                Return Process.Start(info)
            Catch ex As Exception
                Return Nothing
            End Try
        End Function
    End Class
End Namespace
