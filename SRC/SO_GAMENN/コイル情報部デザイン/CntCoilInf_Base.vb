Option Strict On

Imports tClassLibrary


#Const IsVirtual = False		' �g���₷���悤��MustInherit�w��ɂ�����A�p����Ńf�U�C�����g���Ȃ��Ȃ邽�߁A�؂�ւ��邱�Ƃ��ł���悤�ɂ����B�ǂ���ł���������


''' <summary>
''' ����
''' </summary>
''' <remarks></remarks>
#If IsVirtual Then
Public MustInherit Class CntCoilInf_Base
#Else
Public Class CntCoilInf_Base
#End If

	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ �p����Ŏ������˂�
	'/ /////////////////////////////////////////////////////////////////////////////////

#If IsVirtual Then
	''' <summary>
	''' �R���g���[���̏���������
	''' </summary>
	''' <remarks></remarks>
	Public MustOverride Sub Header_Init()

	''' <summary>
	''' �R�C�����\������
	''' </summary>
	''' <param name="sKizukenNo">�Ǘ�No</param>
	''' <param name="ou">�\��(0�I���W��)</param>
	''' <remarks></remarks>
	Public MustOverride Sub Header_Dsp(ByVal sKizukenNo as String )

#Else

	Public Overridable Sub Header_Init()

	End Sub
    Public Overridable Sub Header_Dsp(ByVal sKizukenNo As String, ByVal ou As Integer)

    End Sub
#End If



	'/ /////////////////////////////////////////////////////////////////////////////////
	'/ ���ʏ���
	'/ /////////////////////////////////////////////////////////////////////////////////

	''' <summary>
	''' �R�C�����擾
	''' </summary>
	''' <param name="strSql">SQL��(1���̂ݕԂ�SQL����)</param>
	''' <param name="tcls_DB">�ڑ�DB(New�ς�)</param>
	''' <param name="sqlRead">����(nothing)</param>
	''' <returns>���A���</returns>
	''' <remarks></remarks>
	Protected Function GetCoilInf(ByVal strSql As String, ByVal tcls_DB As tClass.tClass_SQLServer, ByRef sqlRead As Data.SqlClient.SqlDataReader) As Boolean
        '''' �f�[�^�擾
		' Select�����s
		sqlRead = tcls_DB.SelectExecute(strSql)
		If sqlRead Is Nothing Then
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "DB�ǂݍ��݃G���[ [" & DB_COIL_INF & "]")
			Return False
		End If

		' �f�[�^�擾
		Try
			' �ꌏ��������� false
			Return sqlRead.Read()

		Catch ex As Exception
			gcls_Log.Write(tClass.tClass_LogManager.LNO.ERR, "�R�C�����擾�G���[[" & ex.Message & "]")
			tcls_DB.Close()
			Return False
		End Try
	End Function
End Class


