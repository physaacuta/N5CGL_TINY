'*********************************************************************************
'入力項目の上下限値等々をチェックする基本クラス
'	[Ver]
'		Ver.01    2007/03/08  vs2005 対応
'
'	[メモ]
'		
'*********************************************************************************
Option Strict On								' tClassLibrary コーディング規約？？(厳密な遅延バインディング、型変換のチェック)

Namespace tClass
    Public Class tClass_ImputCheck

    '====================================================================
    '基本条件チェック
    '====================================================================
        ''' <summary>
        ''' 整数チェック
        ''' </summary>
        ''' <param name="strVal">チェック文字</param>
        ''' <param name="AnsVal">アンサー値</param>
        ''' <returns></returns>
        ''' <remarks></remarks>
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Integer) As Boolean
            If IsNumeric(strVal) Then		'入力項目が数値型に変換できるかチェックを行う
                AnsVal = CInt(strVal)
                Return True
            Else
                AnsVal = 0
                Return False
            End If
        End Function
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Short) As Boolean
            If IsNumeric(strVal) Then		'入力項目が数値型に変換できるかチェックを行う
                AnsVal = CShort(strVal)
                Return True
            Else
                AnsVal = 0
                Return False
            End If
        End Function
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Long) As Boolean
            If IsNumeric(strVal) Then		'入力項目が数値型に変換できるかチェックを行う
                AnsVal = CLng(strVal)
                Return True
            Else
                AnsVal = 0
                Return False
            End If
        End Function
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Single) As Boolean
            If IsNumeric(strVal) Then		'入力項目が数値型に変換できるかチェックを行う
                AnsVal = CSng(strVal)
                Return True
            Else
                AnsVal = 0.0
                Return False
            End If
        End Function
        Public Shared Function CheckData(ByVal strVal As String, ByRef AnsVal As Double) As Boolean
            If IsNumeric(strVal) Then		'入力項目が数値型に変換できるかチェックを行う
                AnsVal = CDbl(strVal)
                Return True
            Else
                AnsVal = 0.0
                Return False
            End If
        End Function

        ''' <summary>
        ''' 文字チェック
        ''' </summary>
        ''' <param name="strVal">チェック文字</param>
        ''' <param name="AnsVal">アンサー値</param>
        ''' <returns>空白ならFalse</returns>
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
    'ちょっと高度な条件チェック
    '====================================================================
        ''' <summary>
        ''' 文字列の入力値チェック
        ''' </summary>
        ''' <param name="str">チェック文字列 (Trimされた結果に書き換えられるから、注意)</param>
        ''' <param name="lMin">最小値 (Long.MinValue時チェックしない)</param>
        ''' <param name="lMax">最大値 (Long.MaxValue時チェックしない)</param>
        ''' <param name="bNullOk">NULL許可？ (True:OK)</param>
        ''' <returns>0:正常 1:空白 2:変換不可 3:下限異常 4:上限異常 5:小数点有り -1:かなり異常</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputData_Int(ByRef str As String, Optional ByVal lMin As Long = Long.MinValue, Optional ByVal lMax As Long = Long.MaxValue, Optional ByVal bNullOk As Boolean = False) As Integer
            Dim lVal As Long

            Try
                '' スペース削除
                str = Trim(str)

                '' NULLか判定
                If 0 = Len(str) Then
                    Return CInt(IIf(bNullOk, 0, 1))
                End If

                '' 数値か判定
                If Not IsNumeric(str) Then Return 2

                '' 小数点有無チェック
                If 0 < str.IndexOf(CChar(".")) Then Return 5

                lVal = CLng(str)

                '' 下限チェック
                If lMin <> Long.MinValue Then
                    If lVal < lMin Then Return 3
                End If

                '' 上限チェック
                If lMax <> Long.MaxValue Then
                    If lVal > lMax Then Return 4
                End If

                Return 0
            Catch ex As Exception
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' 文字列の入力値チェック
        ''' </summary>
        ''' <param name="str">チェック文字列 (Trimされた結果に書き換えられるから、注意)</param>
        ''' <param name="dMin">最小値 (Long.MinValue時チェックしない)</param>
        ''' <param name="dMax">最大値 (Long.MaxValue時チェックしない)</param>
        ''' <param name="bNullOk">NULL許可？ (True:OK)</param>
        ''' <param name="iKeta">有効少数位置 (1なら少数第一位までOK。 *.** の場合はNGとなる)</param>		
        ''' <returns>0:正常 1:空白 2:変換不可 3:下限異常 4:上限異常 5:小数点桁数異常 -1:かなり異常</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputData_Dbl(ByRef str As String, Optional ByVal dMin As Double = Double.MinValue, Optional ByVal dMax As Double = Double.MaxValue, Optional ByVal bNullOk As Boolean = False, Optional ByVal iKeta As Integer = 1) As Integer
            Dim dVal As Double

            Try
                '' スペース削除
                str = Trim(str)

                '' NULLか判定
                If 0 = Len(str) Then
                    Return CInt(IIf(bNullOk, 0, 1))
                End If

                '' 数値か判定
                If Not IsNumeric(str) Then Return 2

                '' 小数点位置チェック
                Dim iWk1 As Integer = str.IndexOf(CChar("."))
                If 0 < iWk1 Then					' とりあえず 少数点有り
                    Dim iWk2 As Integer = str.LastIndexOf(CChar("."))
                    If iWk1 <> iWk2 Then Return 5 ' 前から探したのと後ろから探したので位置が違うのは、点が2個以上あるから
                    If iWk1 = 0 Then Return 5 '先頭の点はNGとする

                    If (str.Length - iWk1 - 1) > iKeta Then Return 5 ' 点の後ろに指定桁異常の文字がある。
                End If

                dVal = CDbl(str)

                '' 下限チェック
                If dMin <> Double.MinValue Then
                    If dVal < dMin Then Return 3
                End If

                '' 上限チェック
                If dMax <> Double.MaxValue Then
                    If dVal > dMax Then Return 4
                End If

                Return 0
            Catch ex As Exception
                Return -1
            End Try
        End Function

        ''' <summary>
        ''' 文字列の入力値チェック
        ''' </summary>
        ''' <param name="str">チェック文字列 (Trimされた結果に書き換えられるから、注意)</param>
        ''' <param name="iMin">最小桁数 (マイナスはチェック無し。1以上の時は、空白NGとなるよ)</param>
        ''' <param name="iMax">最大桁数 (マイナスはチェック無し)</param>
        ''' <returns>0:正常 3:下限異常 4:上限異常 -1:かなり異常</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputData_Str(ByRef str As String, ByVal iMin As Integer, ByVal iMax As Integer) As Integer
            Dim iVal As Integer

            Try
                '' スペース削除
                str = Trim(str)

                '' 桁数取得
                iVal = tMod.LenB(str)

                '' 下限チェック
                If iMin > -1 Then
                    If iVal < iMin Then Return 3
                End If

                '' 上限チェック
                If iMax > -1 Then
                    If iVal > iMax Then Return 4
                End If

                Return 0
            Catch ex As Exception
                Return -1
            End Try
        End Function

    '====================================================================
    'ガイダンス機能拡張版条件チェック
    '====================================================================
        ''' <summary>
        ''' 文字列の入力値チェック
        ''' </summary>
        ''' <param name="str">チェック文字列 (Trimされた結果に書き換えられるから、注意)</param>
        ''' <param name="lMin">最小値 (Long.MinValue時チェックしない)</param>
        ''' <param name="lMax">最大値 (Long.MaxValue時チェックしない)</param>
        ''' <param name="bNullOk">NULL許可？ (True:OK)</param>
        ''' <param name="strMsg">ガイダンス文字(Nothing時無チェック)</param>
        ''' <param name="bDsp">異常時MsgBoxの表示有無</param>
        ''' <returns>0:OK 1:NG</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputMsg_Int(ByRef str As String, Optional ByVal lMin As Long = Long.MinValue, Optional ByVal lMax As Long = Long.MaxValue, Optional ByVal bNullOk As Boolean = False, Optional ByVal strMsg As String = "", Optional ByVal bDsp As Boolean = True) As Integer
            '' 出力用条件セット
            Dim wkMsg As String = ""
            If bDsp Then
                If "" <> strMsg Then wkMsg = "【 " & strMsg & " 】" & vbCrLf
            End If

            '' 入力値チェック
            '  0:正常 1:空白 2:変換不可 3:下限異常 4:上限異常 5:小数点有り -1:かなり異常
            Dim retc As Integer = CheckImputData_Int(str, lMin, lMax, bNullOk)
            Select Case retc
                Case 0		' 正常

                Case -1, 1, 2, 3, 4
                    If bDsp Then MsgBox(wkMsg & "適切な値ではありません。" & vbCrLf & "入力範囲：「" & lMin & " ～ " & lMax & "」", MsgBoxStyle.OkOnly, "入力チェック")

                Case 5
                    If bDsp Then MsgBox(wkMsg & "小数点入力は出来ません。" & vbCrLf & "入力範囲：「" & lMin & " ～ " & lMax & "」", MsgBoxStyle.OkOnly, "入力チェック")

            End Select

            Return CInt(IIf(retc = 0, 0, 1))
        End Function

        ''' <summary>
        ''' 文字列の入力値チェック
        ''' </summary>
        ''' <param name="str">チェック文字列 (Trimされた結果に書き換えられるから、注意)</param>
        ''' <param name="dMin">最小値 (Long.MinValue時チェックしない)</param>
        ''' <param name="dMax">最大値 (Long.MaxValue時チェックしない)</param>
        ''' <param name="bNullOk">NULL許可？ (True:OK)</param>
        ''' <param name="iKeta">有効少数位置 (1なら少数第一位までOK。 *.** の場合はNGとなる)</param>			
        ''' <param name="strMsg">ガイダンス文字(Nothing時無チェック)</param>
        ''' <param name="bDsp">異常時MsgBoxの表示有無</param>
        ''' <returns>0:OK 1:NG</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputMsg_Dbl(ByRef str As String, Optional ByVal dMin As Double = Double.MinValue, Optional ByVal dMax As Double = Double.MaxValue, Optional ByVal bNullOk As Boolean = False, Optional ByVal iKeta As Integer = 1, Optional ByVal strMsg As String = "", Optional ByVal bDsp As Boolean = True) As Integer
            '' 出力用条件セット
            Dim wkMsg As String = ""
            If bDsp Then
                If "" <> strMsg Then wkMsg = "【 " & strMsg & " 】" & vbCrLf
            End If

            '' 入力値チェック
            '  0:正常 1:空白 2:変換不可 3:下限異常 4:上限異常 5:小数点桁数異常 -1:かなり異常
            Dim retc As Integer = CheckImputData_Dbl(str, dMin, dMax, bNullOk, iKeta)
            Select Case retc
                Case 0      ' 正常

                Case -1, 1, 2, 3, 4
                    If bDsp Then MsgBox(wkMsg & "適切な値ではありません。" & vbCrLf & "入力範囲：「" & dMin & " ～ " & dMax & "」 有効桁数：" & iKeta, MsgBoxStyle.OkOnly, "入力チェック")

                Case 5
                    If bDsp Then MsgBox(wkMsg & "小数点桁数が異常です。" & vbCrLf & "入力範囲：「" & dMin & " ～ " & dMax & "」 有効桁数：" & iKeta, MsgBoxStyle.OkOnly, "入力チェック")

            End Select

            Return CInt(IIf(retc = 0, 0, 1))
        End Function


        ''' <summary>
        ''' 文字列の入力値チェック
        ''' </summary>
        ''' <param name="str">チェック文字列 (Trimされた結果に書き換えられるから、注意)</param>
        ''' <param name="iMin">最小桁数 (マイナスはチェック無し。1以上の時は、空白NGとなるよ)</param>
        ''' <param name="iMax">最大桁数 (マイナスはチェック無し)</param>
        ''' <param name="strMsg">ガイダンス文字(Nothing時無チェック)</param>
        ''' <param name="bDsp">異常時MsgBoxの表示有無</param>
        ''' <returns>0:OK 1:NG</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckImputMsg_Str(ByRef str As String, Optional ByVal iMin As Integer = 0, Optional ByVal iMax As Integer = Integer.MaxValue, Optional ByVal strMsg As String = "", Optional ByVal bDsp As Boolean = True) As Integer
            '' 出力用条件セット
            Dim wkMsg As String = ""
            If bDsp Then
                If "" <> strMsg Then wkMsg = "【 " & strMsg & " 】" & vbCrLf
            End If

            '' 入力値チェック
            '  0:正常 3:下限異常 4:上限異常 -1:かなり異常
            Dim retc As Integer = CheckImputData_Str(str, iMin, iMax)
            Select Case retc
                Case 0      ' 正常

                Case -1, 3, 4
                    If bDsp Then MsgBox(wkMsg & "適切な値ではありません。" & vbCrLf & "入力範囲：「" & iMin & " ～ " & iMax & "」", MsgBoxStyle.OkOnly, "入力チェック")
            End Select

            Return CInt(IIf(retc = 0, 0, 1))
        End Function


        ''' <summary>
        ''' ２項目の整合性チェック
        ''' </summary>
        ''' <param name="strMin">小さいと思われる項目</param>
        ''' <param name="strMax">大きいと思われる項目</param>
        ''' <param name="iNullOk">NULL許可？ (0:NULL許可しない 1:NULL 片方のみ許可 2:NULL 両方許可)</param>
        ''' <param name="bEq">同一値の場合は？ (False:同一値異常 True:同一値正常)</param>
        ''' <param name="strMsg">ガイダンス文字(Nothing時無チェック)</param>
        ''' <param name="bDsp">異常時MsgBoxの表示有無</param>
        ''' <returns>復帰情報</returns>
        ''' <remarks></remarks>
        Public Shared Function CheckMinMax(ByVal strMin As String, ByVal strMax As String, Optional ByVal iNullOk As Integer = 0, Optional ByVal bEq As Boolean = False, Optional ByVal strMsg As String = "", Optional ByVal bDsp As Boolean = True) As Boolean
            Dim retc As Boolean = True
            Dim wkMin As String = Trim(strMin)
            Dim wkMax As String = Trim(strMax)

            '' 出力用条件セット
            Dim wkMsg As String = ""
            If bDsp Then
                If "" <> strMsg Then wkMsg = "【 " & strMsg & " 】" & vbCrLf
            End If


            Try
                '' NULL チェック
                If iNullOk = 0 Then                                         ' NULL 許可しない
                    If "" = wkMin Or "" = wkMax Then
                        If bDsp Then MsgBox(wkMsg & "空白は、許可していません。値を入力してください。", MsgBoxStyle.OkOnly, "入力チェック")
                        Return False
                    End If

                ElseIf iNullOk = 1 Then                                     ' NULL 片方のみ許可
                    If "" = wkMin And "" = wkMax Then
                        If bDsp Then MsgBox(wkMsg & "空白は、許可していません。値を入力してください。", MsgBoxStyle.OkOnly, "入力チェック")
                        Return False
                    End If

                ElseIf iNullOk = 2 Then                                     ' NULL 両方許可
                    If "" = wkMin And "" = wkMax Then Return True
                End If


                '' 片方がNULLの時の整合性チェック
                If "" = wkMin Or "" = wkMax Then
                    If IsNumeric(wkMin) Or IsNumeric(wkMax) Then        'NULLはfalseとなる
                        Return True
                    Else
                        If bDsp Then MsgBox(wkMsg & "適切な値ではありません。値を変更してください。", MsgBoxStyle.OkOnly, "入力チェック")
                        Return False
                    End If
                End If


                '' ここまで来たら、必ず値が２ことも入っている
                If IsNumeric(wkMin) And IsNumeric(wkMax) Then
                    Dim dMin As Double = CDbl(wkMin)
                    Dim dMax As Double = CDbl(wkMax)
                    If dMin = dMax Then         '同値
                        If Not bEq Then
                            If bDsp Then MsgBox(wkMsg & "下限値 と 上限値 が同じ値です。", MsgBoxStyle.OkOnly, "入力チェック")
                        Else
                            Return True
                        End If

                    ElseIf dMin > dMax Then     '小さいと思われる方が大きかった
                        If bDsp Then MsgBox(wkMsg & "下限値 と 上限値 の整合性が異常です。", MsgBoxStyle.OkOnly, "入力チェック")
                    Else
                        Return True
                    End If

                Else
                    If bDsp Then MsgBox(wkMsg & "適切な値ではありません。値を変更してください。", MsgBoxStyle.OkOnly, "入力チェック")
                    Return False
                End If

            Catch ex As Exception
                If bDsp Then MsgBox(wkMsg & "適切な値ではありません。値を変更してください。", MsgBoxStyle.OkOnly, "入力チェック")
                Return False
            End Try
            Return False
        End Function



    End Class
End Namespace

