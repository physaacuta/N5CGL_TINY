'*********************************************************************************
'���͍��ڂ̏㉺���l���X���`�F�b�N�����{�N���X
'	[Ver]
'		Ver.01    2007/03/08  vs2005 �Ή�
'
'	[����]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary �R�[�f�B���O�K��H�H(�����Ȓx���o�C���f�B���O�A�^�ϊ��̃`�F�b�N)

Namespace tClass
    Public Class tClass_ImputCheck

    '====================================================================
    '��{�����`�F�b�N
    '====================================================================
        ''' <summary>
        ''' �����`�F�b�N
        ''' </summary>
        ''' <param name="strVal">�`�F�b�N����</param>
        ''' <param name="AnsVal">�A���T�[�l</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Integer) As Boolean
            If IsNumeric(strVal) Then		'���͍��ڂ����l�^�ɕϊ��ł��邩�`�F�b�N���s��
                AnsVal = CInt(strVal)
                Return True
            Else
                AnsVal = 0
                Return False
            End If
        End Function
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Short) As Boolean
            If IsNumeric(strVal) Then		'���͍��ڂ����l�^�ɕϊ��ł��邩�`�F�b�N���s��
                AnsVal = CShort(strVal)
                Return True
            Else
                AnsVal = 0
                Return False
            End If
        End Function
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Long) As Boolean
            If IsNumeric(strVal) Then		'���͍��ڂ����l�^�ɕϊ��ł��邩�`�F�b�N���s��
                AnsVal = CLng(strVal)
                Return True
            Else
                AnsVal = 0
                Return False
            End If
        End Function
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Single) As Boolean
            If IsNumeric(strVal) Then		'���͍��ڂ����l�^�ɕϊ��ł��邩�`�F�b�N���s��
                AnsVal = CSng(strVal)
                Return True
            Else
                AnsVal = 0.0
                Return False
            End If
        End Function
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Double) As Boolean
            If IsNumeric(strVal) Then		'���͍��ڂ����l�^�ɕϊ��ł��邩�`�F�b�N���s��
                AnsVal = CDbl(strVal)
                Return True
            Else
                AnsVal = 0.0
                Return False
            End If
        End Function

        ''' <summary>
        ''' �����`�F�b�N
        ''' </summary>
        ''' <param name="strVal">�`�F�b�N����</param>
        ''' <param name="AnsVal">�A���T�[�l</param>
        ''' <returns>�󔒂Ȃ�False</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As String) As Boolean
            AnsVal = Trim(strVal)
            If 0 < AnsVal.Length Then
                Return True
            Else
                AnsVal = ""
                Return False
            End If
        End Function


    '====================================================================
    '������ƍ��x�ȏ����`�F�b�N
    '====================================================================
        ''' <summary>
        ''' ������̓��͒l�`�F�b�N
        ''' </summary>
        ''' <param name="str">�`�F�b�N������ (Trim���ꂽ���ʂɏ����������邩��A����)</param>
        ''' <param name="lMin">�ŏ��l (Long.MinValue���`�F�b�N���Ȃ�)</param>
        ''' <param name="lMax">�ő�l (Long.MaxValue���`�F�b�N���Ȃ�)</param>
        ''' <param name="bNullOk">NULL���H (True:OK)</param>
        ''' <returns>0:���� 1:�� 2:�ϊ��s�� 3:�����ُ� 4:����ُ� 5:�����_�L�� -1:���Ȃ�ُ�</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputData_Int(ByRef str As String, Optional ByVal lMin As Long = Long.MinValue, Optional ByVal lMax As Long = Long.MaxValue, Optional ByVal bNullOk As Boolean = False) As Integer
            Dim lVal As Long

            Try
                '' �X�y�[�X�폜
                str = Trim(str)

                '' NULL������
                If 0 = Len(str) Then
                    Return CInt(IIf(bNullOk, 0, 1))
                End If

                '' ���l������
                If Not IsNumeric(str) Then Return 2

                '' �����_�L���`�F�b�N
                If 0 < str.IndexOf(CChar(".")) Then Return 5

                lVal = CLng(str)

                '' �����`�F�b�N
                If lMin <> Long.MinValue Then
                    If lVal < lMin Then Return 3
                End If

                '' ����`�F�b�N
                If lMax <> Long.MaxValue Then
                    If lVal > lMax Then Return 4
                End If

                Return 0
            Catch ex As Exception
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' ������̓��͒l�`�F�b�N
        ''' </summary>
        ''' <param name="str">�`�F�b�N������ (Trim���ꂽ���ʂɏ����������邩��A����)</param>
        ''' <param name="dMin">�ŏ��l (Long.MinValue���`�F�b�N���Ȃ�)</param>
        ''' <param name="dMax">�ő�l (Long.MaxValue���`�F�b�N���Ȃ�)</param>
        ''' <param name="bNullOk">NULL���H (True:OK)</param>
        ''' <param name="iKeta">�L�������ʒu (1�Ȃ班�����ʂ܂�OK�B *.** �̏ꍇ��NG�ƂȂ�)</param>		
        ''' <returns>0:���� 1:�� 2:�ϊ��s�� 3:�����ُ� 4:����ُ� 5:�����_�����ُ� -1:���Ȃ�ُ�</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputData_Dbl(ByRef str As String, Optional ByVal dMin As Double = Double.MinValue, Optional ByVal dMax As Double = Double.MaxValue, Optional ByVal bNullOk As Boolean = False, Optional ByVal iKeta As Integer = 1) As Integer
            Dim dVal As Double

            Try
                '' �X�y�[�X�폜
                str = Trim(str)

                '' NULL������
                If 0 = Len(str) Then
                    Return CInt(IIf(bNullOk, 0, 1))
                End If

                '' ���l������
                If Not IsNumeric(str) Then Return 2

                '' �����_�ʒu�`�F�b�N
                Dim iWk1 As Integer = str.IndexOf(CChar("."))
                If 0 < iWk1 Then					' �Ƃ肠���� �����_�L��
                    Dim iWk2 As Integer = str.LastIndexOf(CChar("."))
                    If iWk1 <> iWk2 Then Return 5 ' �O����T�����̂ƌ�납��T�����̂ňʒu���Ⴄ�̂́A�_��2�ȏ゠�邩��
                    If iWk1 = 0 Then Return 5 '�擪�̓_��NG�Ƃ���

                    If (str.Length - iWk1 - 1) > iKeta Then Return 5 ' �_�̌��Ɏw�茅�ُ�̕���������B
                End If

                dVal = CDbl(str)

                '' �����`�F�b�N
                If dMin <> Double.MinValue Then
                    If dVal < dMin Then Return 3
                End If

                '' ����`�F�b�N
                If dMax <> Double.MaxValue Then
                    If dVal > dMax Then Return 4
                End If

                Return 0
            Catch ex As Exception
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' ������̓��͒l�`�F�b�N
        ''' </summary>
        ''' <param name="str">�`�F�b�N������ (Trim���ꂽ���ʂɏ����������邩��A����)</param>
        ''' <param name="iMin">�ŏ����� (�}�C�i�X�̓`�F�b�N�����B1�ȏ�̎��́A��NG�ƂȂ��)</param>
        ''' <param name="iMax">�ő包�� (�}�C�i�X�̓`�F�b�N����)</param>
        ''' <returns>0:���� 3:�����ُ� 4:����ُ� -1:���Ȃ�ُ�</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputData_Str(ByRef str As String, ByVal iMin As Integer, ByVal iMax As Integer) As Integer
            Dim iVal As Integer

            Try
                '' �X�y�[�X�폜
                str = Trim(str)

                '' �����擾
                iVal = tMod.LenB(str)

                '' �����`�F�b�N
                If iMin > -1 Then
                    If iVal < iMin Then Return 3
                End If

                '' ����`�F�b�N
                If iMax > -1 Then
                    If iVal > iMax Then Return 4
                End If

                Return 0
            Catch ex As Exception
                Return -1
            End Try
        End Function

    '====================================================================
    '�K�C�_���X�@�\�g���ŏ����`�F�b�N
    '====================================================================
        ''' <summary>
        ''' ������̓��͒l�`�F�b�N
        ''' </summary>
        ''' <param name="str">�`�F�b�N������ (Trim���ꂽ���ʂɏ����������邩��A����)</param>
        ''' <param name="lMin">�ŏ��l (Long.MinValue���`�F�b�N���Ȃ�)</param>
        ''' <param name="lMax">�ő�l (Long.MaxValue���`�F�b�N���Ȃ�)</param>
        ''' <param name="bNullOk">NULL���H (True:OK)</param>
        ''' <param name="strMsg">�K�C�_���X����(Nothing�����`�F�b�N)</param>
        ''' <param name="bDsp">�ُ펞MsgBox�̕\���L��</param>
        ''' <returns>0:OK 1:NG</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputMsg_Int(ByRef str As String, Optional ByVal lMin As Long = Long.MinValue, Optional ByVal lMax As Long = Long.MaxValue, Optional ByVal bNullOk As Boolean = False, Optional ByVal strMsg As String = "", Optional ByVal bDsp As Boolean = True) As Integer
            '' �o�͗p�����Z�b�g
            Dim wkMsg As String = ""
            If bDsp Then
                If "" <> strMsg Then wkMsg = "�y " & strMsg & " �z" & vbCrLf
            End If

            '' ���͒l�`�F�b�N
            '  0:���� 1:�� 2:�ϊ��s�� 3:�����ُ� 4:����ُ� 5:�����_�L�� -1:���Ȃ�ُ�
            Dim retc As Integer = CheckImputData_Int(str, lMin, lMax, bNullOk)
            Select Case retc
                Case 0		' ����

                Case -1, 1, 2, 3, 4
                    If bDsp Then MsgBox(wkMsg & "�K�؂Ȓl�ł͂���܂���B" & vbCrLf & "���͔͈́F�u" & lMin & " �` " & lMax & "�v", MsgBoxStyle.OkOnly, "���̓`�F�b�N")

                Case 5
                    If bDsp Then MsgBox(wkMsg & "�����_���͂͏o���܂���B" & vbCrLf & "���͔͈́F�u" & lMin & " �` " & lMax & "�v", MsgBoxStyle.OkOnly, "���̓`�F�b�N")

            End Select

            Return CInt(IIf(retc = 0, 0, 1))
        End Function

        ''' <summary>
        ''' ������̓��͒l�`�F�b�N
        ''' </summary>
        ''' <param name="str">�`�F�b�N������ (Trim���ꂽ���ʂɏ����������邩��A����)</param>
        ''' <param name="dMin">�ŏ��l (Long.MinValue���`�F�b�N���Ȃ�)</param>
        ''' <param name="dMax">�ő�l (Long.MaxValue���`�F�b�N���Ȃ�)</param>
        ''' <param name="bNullOk">NULL���H (True:OK)</param>
        ''' <param name="iKeta">�L�������ʒu (1�Ȃ班�����ʂ܂�OK�B *.** �̏ꍇ��NG�ƂȂ�)</param>			
        ''' <param name="strMsg">�K�C�_���X����(Nothing�����`�F�b�N)</param>
        ''' <param name="bDsp">�ُ펞MsgBox�̕\���L��</param>
        ''' <returns>0:OK 1:NG</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputMsg_Dbl(ByRef str As String, Optional ByVal dMin As Double = Double.MinValue, Optional ByVal dMax As Double = Double.MaxValue, Optional ByVal bNullOk As Boolean = False, Optional ByVal iKeta As Integer = 1, Optional ByVal strMsg As String = "", Optional ByVal bDsp As Boolean = True) As Integer
            '' �o�͗p�����Z�b�g
            Dim wkMsg As String = ""
            If bDsp Then
                If "" <> strMsg Then wkMsg = "�y " & strMsg & " �z" & vbCrLf
            End If

            '' ���͒l�`�F�b�N
            '  0:���� 1:�� 2:�ϊ��s�� 3:�����ُ� 4:����ُ� 5:�����_�����ُ� -1:���Ȃ�ُ�
            Dim retc As Integer = CheckImputData_Dbl(str, dMin, dMax, bNullOk, iKeta)
            Select Case retc
                Case 0      ' ����

                Case -1, 1, 2, 3, 4
                    If bDsp Then MsgBox(wkMsg & "�K�؂Ȓl�ł͂���܂���B" & vbCrLf & "���͔͈́F�u" & dMin & " �` " & dMax & "�v �L�������F" & iKeta, MsgBoxStyle.OkOnly, "���̓`�F�b�N")

                Case 5
                    If bDsp Then MsgBox(wkMsg & "�����_�������ُ�ł��B" & vbCrLf & "���͔͈́F�u" & dMin & " �` " & dMax & "�v �L�������F" & iKeta, MsgBoxStyle.OkOnly, "���̓`�F�b�N")

            End Select

            Return CInt(IIf(retc = 0, 0, 1))
        End Function


        ''' <summary>
        ''' ������̓��͒l�`�F�b�N
        ''' </summary>
        ''' <param name="str">�`�F�b�N������ (Trim���ꂽ���ʂɏ����������邩��A����)</param>
        ''' <param name="iMin">�ŏ����� (�}�C�i�X�̓`�F�b�N�����B1�ȏ�̎��́A��NG�ƂȂ��)</param>
        ''' <param name="iMax">�ő包�� (�}�C�i�X�̓`�F�b�N����)</param>
        ''' <param name="strMsg">�K�C�_���X����(Nothing�����`�F�b�N)</param>
        ''' <param name="bDsp">�ُ펞MsgBox�̕\���L��</param>
        ''' <returns>0:OK 1:NG</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputMsg_Str(ByRef str As String, Optional ByVal iMin As Integer = 0, Optional ByVal iMax As Integer = Integer.MaxValue, Optional ByVal strMsg As String = "", Optional ByVal bDsp As Boolean = True) As Integer
            '' �o�͗p�����Z�b�g
            Dim wkMsg As String = ""
            If bDsp Then
                If "" <> strMsg Then wkMsg = "�y " & strMsg & " �z" & vbCrLf
            End If

            '' ���͒l�`�F�b�N
            '  0:���� 3:�����ُ� 4:����ُ� -1:���Ȃ�ُ�
            Dim retc As Integer = CheckImputData_Str(str, iMin, iMax)
            Select Case retc
                Case 0      ' ����

                Case -1, 3, 4
                    If bDsp Then MsgBox(wkMsg & "�K�؂Ȓl�ł͂���܂���B" & vbCrLf & "���͔͈́F�u" & iMin & " �` " & iMax & "�v", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
            End Select

            Return CInt(IIf(retc = 0, 0, 1))
        End Function


        ''' <summary>
        ''' �Q���ڂ̐������`�F�b�N
        ''' </summary>
        ''' <param name="strMin">�������Ǝv���鍀��</param>
        ''' <param name="strMax">�傫���Ǝv���鍀��</param>
        ''' <param name="iNullOk">NULL���H (0:NULL�����Ȃ� 1:NULL �Е��̂݋��� 2:NULL ��������)</param>
        ''' <param name="bEq">����l�̏ꍇ�́H (False:����l�ُ� True:����l����)</param>
        ''' <param name="strMsg">�K�C�_���X����(Nothing�����`�F�b�N)</param>
        ''' <param name="bDsp">�ُ펞MsgBox�̕\���L��</param>
        ''' <returns>���A���</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckMinMax(ByVal strMin As String, ByVal strMax As String, Optional ByVal iNullOk As Integer = 0, Optional ByVal bEq As Boolean = False, Optional ByVal strMsg As String = "", Optional ByVal bDsp As Boolean = True) As Boolean
            Dim retc As Boolean = True
            Dim wkMin As String = Trim(strMin)
            Dim wkMax As String = Trim(strMax)

            '' �o�͗p�����Z�b�g
            Dim wkMsg As String = ""
            If bDsp Then
                If "" <> strMsg Then wkMsg = "�y " & strMsg & " �z" & vbCrLf
            End If


            Try
                '' NULL �`�F�b�N
                If iNullOk = 0 Then                                         ' NULL �����Ȃ�
                    If "" = wkMin Or "" = wkMax Then
                        If bDsp Then MsgBox(wkMsg & "�󔒂́A�����Ă��܂���B�l����͂��Ă��������B", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                        Return False
                    End If

                ElseIf iNullOk = 1 Then                                     ' NULL �Е��̂݋���
                    If "" = wkMin And "" = wkMax Then
                        If bDsp Then MsgBox(wkMsg & "�󔒂́A�����Ă��܂���B�l����͂��Ă��������B", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                        Return False
                    End If

                ElseIf iNullOk = 2 Then                                     ' NULL ��������
                    If "" = wkMin And "" = wkMax Then Return True
                End If


                '' �Е���NULL�̎��̐������`�F�b�N
                If "" = wkMin Or "" = wkMax Then
                    If IsNumeric(wkMin) Or IsNumeric(wkMax) Then        'NULL��false�ƂȂ�
                        Return True
                    Else
                        If bDsp Then MsgBox(wkMsg & "�K�؂Ȓl�ł͂���܂���B�l��ύX���Ă��������B", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                        Return False
                    End If
                End If


                '' �����܂ŗ�����A�K���l���Q���Ƃ������Ă���
                If IsNumeric(wkMin) And IsNumeric(wkMax) Then
                    Dim dMin As Double = CDbl(wkMin)
                    Dim dMax As Double = CDbl(wkMax)
                    If dMin = dMax Then         '���l
                        If Not bEq Then
                            If bDsp Then MsgBox(wkMsg & "�����l �� ����l �������l�ł��B", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                        Else
                            Return True
                        End If

                    ElseIf dMin > dMax Then     '�������Ǝv��������傫������
                        If bDsp Then MsgBox(wkMsg & "�����l �� ����l �̐��������ُ�ł��B", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                    Else
                        Return True
                    End If

                Else
                    If bDsp Then MsgBox(wkMsg & "�K�؂Ȓl�ł͂���܂���B�l��ύX���Ă��������B", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                    Return False
                End If

            Catch ex As Exception
                If bDsp Then MsgBox(wkMsg & "�K�؂Ȓl�ł͂���܂���B�l��ύX���Ă��������B", MsgBoxStyle.OkOnly, "���̓`�F�b�N")
                Return False
            End Try
            Return False
        End Function



    End Class
End Namespace

