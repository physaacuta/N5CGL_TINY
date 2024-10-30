Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Imports System.Runtime.InteropServices

Public Class AllUserLogOff

	'********************************************************************
	' API�萔 ��`

	' C++�Ŏg�p���Ă����֐����g�p����ׁA��`����
	' �Y���Z�b�V���������O�I�t����֐�
	<DllImport("wtsapi32.dll", SetLastError:=True)> _
	Private Shared Function WTSLogoffSession(ByVal hServer As IntPtr, ByVal SessionId As Integer, ByVal bWait As Boolean) As Integer
	End Function

    ' ���[�U�[�؂�ւ�
    <DllImport("wtsapi32.dll", SetLastError:=True, ExactSpelling:=False, CharSet:=CharSet.Ansi)> _
    Private Shared Function WTSConnectSession(ByVal LogonId As Int32, ByVal TargetLogonID As Int32, <MarshalAs(UnmanagedType.LPStr)> ByVal pPassword As String, ByVal bWait As Boolean) As Integer
    End Function


	' �Z�b�V���������擾����֐�
	<DllImport("wtsapi32.dll", bestfitmapping:=True, CallingConvention:=CallingConvention.StdCall, CharSet:=CharSet.Auto, EntryPoint:="WTSEnumerateSessions", setlasterror:=True, ThrowOnUnmappableChar:=True)> _
	Private Shared Function WTSEnumerateSessions(ByVal hServer As IntPtr, <MarshalAs(UnmanagedType.U4)> ByVal Reserved As Int32, <MarshalAs(UnmanagedType.U4)> ByVal Version As Int32, ByRef ppSessionInfo As IntPtr, <MarshalAs(UnmanagedType.U4)> ByRef pCount As Int32) As Int32
	End Function


	' ���������J������֐�
	<DllImport("wtsapi32.dll")> _
	 Private Shared Sub WTSFreeMemory(ByVal pMemory As IntPtr)
	End Sub



	' �Z�b�V������Ԃ�Enum��`
    Private Enum WTS_CONNECTSTATE_CLASS
        WTSActive
        WTSConnected
        WTSConnectQuery
        WTSShadow
        WTSDisconnected
        WTSIdle
        WTSListen
        WTSReset
        WTSDown
        WTSInit
    End Enum

	' �Z�b�V���������i�[����\���̒�`
	<StructLayout(LayoutKind.Sequential, CharSet:=CharSet.Auto)> _
	Private Structure WTS_SESSION_INFO
		Dim nSessionID As Int32
		Dim pWinStationName As String
		Dim eState As WTS_CONNECTSTATE_CLASS
	End Structure


	'********************************************************************
	' ���C������

	''' <summary>
	''' �����ȊO�̃Z�b�V���������O�I�t����(Listen�Z�b�V�����ƃT�[�r�X�p�Z�b�V�����͏��O)
	''' </summary>
	''' <remarks></remarks>
	Public Shared Sub Exec()
		' �ϐ���`
		' �ϐ��̐錾_������
		Dim nMySessionID As Integer = Process.GetCurrentProcess.SessionId	' ���Z�b�V����ID�擾
		Dim pSessions As IntPtr = IntPtr.Zero	' �擾�Z�b�V�����|�C���^
		Dim nScount As Int32 = 0				' �Z�b�V������


		' �ڑ�����Ă���S�Z�b�V���������擾����
		Dim nRetVal As Int32 = WTSEnumerateSessions(IntPtr.Zero, 0, 1, pSessions, nScount)
		If 0 = nRetVal Then Return


		' �Z�b�V�����������𐶐�����
		Dim pSessionInfo() As WTS_SESSION_INFO = New WTS_SESSION_INFO(nScount) {}
		Dim nOffset As Int64 = pSessions.ToInt64

		' �Z�b�V���������������s��
		For ii As Integer = 0 To nScount - 1
			' �Z�b�V���������擾����
			pSessionInfo(ii) = CType(Marshal.PtrToStructure(New IntPtr(nOffset), GetType(WTS_SESSION_INFO)), WTS_SESSION_INFO)
			nOffset += Marshal.SizeOf(New WTS_SESSION_INFO)	 ' ����


			' �����̃Z�b�V����ID�ȊO�ƃT�[�r�X�p�Z�b�V����(ID:0)�ȊO�ƃ��b�X���Z�b�V����(65536)�̃��[�U�[�����O����
			If ((nMySessionID <> pSessionInfo(ii).nSessionID) And _
			   ((0 <> pSessionInfo(ii).nSessionID) And ("Services" <> pSessionInfo(ii).pWinStationName) And _
			   (WTS_CONNECTSTATE_CLASS.WTSListen <> pSessionInfo(ii).eState))) Then

				' �����̃��[�U���ȊO�ƃT�[�r�X�p�Z�b�V�����ȊO�ƃ��b�X���Z�b�V����(65536)�̃��[�U�[�����O�I�t����
				Dim nRet As Integer = WTSLogoffSession(IntPtr.Zero, pSessionInfo(ii).nSessionID, True)
				''gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, String.Format(" ���O�I�t�Z�b�V�������F{0} ���O�I�t�Z�b�V����ID: {1} ���O�I�t�֐��߂�l�F{2} ", pSessionInfo(ii).pWinStationName, pSessionInfo(ii).nSessionID, nRet))
				Continue For
			End If
			''gcls_Log.Write(tClassLibrary.tClass.tClass_LogManager.LNO.MSG, String.Format(" [���O�I���̂܂�] ���O�I�t�Z�b�V�������F{0} �Z�b�V����ID: {1} ", pSessionInfo(ii).pWinStationName, pSessionInfo(ii).nSessionID))
		Next

		' �J������
		WTSFreeMemory(pSessions)
		''gcls_Log.Dispose()
		''gcls_Log = Nothing
    End Sub

    ''' <summary>
    ''' �R���\�[������ �\�łȂ��ꍇ �R���\�[����\�ɕύX����
    ''' </summary>
    ''' <remarks></remarks>
    Public Shared Sub Change()
        Dim nMySessionID As Integer = Process.GetCurrentProcess.SessionId   ' ���Z�b�V����ID�擾
        Dim pSessions As IntPtr = IntPtr.Zero   ' �擾�Z�b�V�����|�C���^
        Dim nScount As Int32 = 0                ' �Z�b�V������

        Dim nRetVal As Int32 = WTSEnumerateSessions(IntPtr.Zero, 0, 1, pSessions, nScount)
        If 0 = nRetVal Then Return

        ' �R���\�[���Z�b�V����ID�����
        Dim nConsoleId As Int32 = 0    ' �R���\�[��ID
        Dim pSessionInfo() As WTS_SESSION_INFO = New WTS_SESSION_INFO(nScount) {}
        Dim nOffset As Int64 = pSessions.ToInt64
        For ii As Integer = 0 To nScount - 1
            pSessionInfo(ii) = CType(Marshal.PtrToStructure(New IntPtr(nOffset), GetType(WTS_SESSION_INFO)), WTS_SESSION_INFO)
            nOffset += Marshal.SizeOf(New WTS_SESSION_INFO)  ' ����


            If "Console" = pSessionInfo(ii).pWinStationName Then
                nConsoleId = pSessionInfo(ii).nSessionID
            End If
        Next

        ' �؂�ւ�
        If 0 <> nConsoleId AndAlso nMySessionID <> nConsoleId Then
            ' ��O�����́A�p�X���[�h�B����ł���΁A�󔒂ł��ǂ��Ǝv��
            WTSConnectSession(nMySessionID, nConsoleId, "", True)
        End If

        ' �J������
        WTSFreeMemory(pSessions)

    End Sub


    ''' <summary>
    ''' �A�N�e�B�u�ȃZ�b�V����ID��Ԃ�
    ''' </summary>
    ''' <returns></returns>
    ''' <remarks></remarks>
    Public Shared Function GetActiveSessionId() As Int32
        Dim nMySessionID As Integer = Process.GetCurrentProcess.SessionId   ' ���Z�b�V����ID�擾
        Dim pSessions As IntPtr = IntPtr.Zero   ' �擾�Z�b�V�����|�C���^
        Dim nScount As Int32 = 0                ' �Z�b�V������

        Dim nRetVal As Int32 = WTSEnumerateSessions(IntPtr.Zero, 0, 1, pSessions, nScount)
        If 0 = nRetVal Then Return -1

        ' �R���\�[���Z�b�V����ID�����
        Dim nActiveId As Int32 = -1    ' �A�N�e�B�uID
        Dim pSessionInfo() As WTS_SESSION_INFO = New WTS_SESSION_INFO(nScount) {}
        Dim nOffset As Int64 = pSessions.ToInt64
        For ii As Integer = 0 To nScount - 1
            pSessionInfo(ii) = CType(Marshal.PtrToStructure(New IntPtr(nOffset), GetType(WTS_SESSION_INFO)), WTS_SESSION_INFO)
            nOffset += Marshal.SizeOf(New WTS_SESSION_INFO)  ' ����


            If WTS_CONNECTSTATE_CLASS.WTSActive = pSessionInfo(ii).eState Then
                nActiveId = pSessionInfo(ii).nSessionID
            End If
        Next
        ' �J������
        WTSFreeMemory(pSessions)

        Return nActiveId
    End Function
End Class
