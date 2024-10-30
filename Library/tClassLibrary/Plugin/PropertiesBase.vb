Namespace RpiPlugin

    Public Interface IPropertiesProvider
        ReadOnly Property Document As PropertiesBase
    End Interface


    Public Class PropertiesBase
        Private m_dic As Dictionary(Of String, Reflection.MemberInfo)
        Private Sub MakeRefrectionDic()
            m_dic = New Dictionary(Of String, Reflection.MemberInfo)
            For Each f As Reflection.FieldInfo In Me.GetType.GetFields()
                If f.IsPublic Then
                    m_dic.Add(f.Name.ToLower, f)
                End If
            Next
            For Each f As Reflection.PropertyInfo In Me.GetType.GetProperties()
                m_dic.Add(f.Name.ToLower, f)
            Next

        End Sub

        Delegate Sub ValueChanged(ByVal name As String, doc As PropertiesBase)
        Public Event OnValueChanged As ValueChanged

        ''' <summary>
        ''' 変更を、購読者に通知してまわる
        ''' </summary>
        Public Overridable Sub Emit()
            Dim myname As String = Me.GetType.Name
            RaiseEvent OnValueChanged(myname, Me)
        End Sub

        ''' <summary>
        ''' 名前で値を取り出す。
        ''' </summary>
        ''' <param name="name">プロパティ・変数名</param>
        ''' <returns>その値</returns>
        Public Function GetValue(name As String) As Object
            If m_dic Is Nothing Then MakeRefrectionDic()

            Dim mi As Reflection.MemberInfo = Nothing
            If m_dic.TryGetValue(name.ToLower, mi) Then
                Dim fi As Reflection.FieldInfo = TryCast(mi, Reflection.FieldInfo)
                If fi IsNot Nothing Then
                    Return fi.GetValue(Me)
                End If

                Dim pi As Reflection.PropertyInfo = TryCast(mi, Reflection.PropertyInfo)
                If pi IsNot Nothing Then
                    Return pi.GetValue(Me, Nothing)
                End If
            End If
            Return Nothing
        End Function

        ''' <summary>
        ''' 名前で値をセットする
        ''' </summary>
        ''' <param name="name">プロパティ・変数名</param>
        ''' <param name="value">あたらしい値</param>
        ''' <returns>プロパティ変数があったかどうか</returns>
        Public Function SetValue(name As String, value As Object) As Boolean
            If m_dic Is Nothing Then MakeRefrectionDic()

            Dim mi As Reflection.MemberInfo = Nothing
            If m_dic.TryGetValue(name.ToLower, mi) Then
                Dim fi As Reflection.FieldInfo = TryCast(mi, Reflection.FieldInfo)
                If fi IsNot Nothing Then
                    fi.SetValue(Me, value)
                    Return True
                End If

                Dim pi As Reflection.PropertyInfo = TryCast(mi, Reflection.PropertyInfo)
                If pi IsNot Nothing Then
                    pi.SetValue(Me, value, Nothing)
                    Return True
                End If
            End If
            Return False
        End Function

        ''' <summary>
        ''' (obj as IPropertiesProvider).Document.name as T
        ''' </summary>
        ''' <typeparam name="T">子Propertyのクラス。PeropertiesBase継承型</typeparam>
        ''' <param name="obj">IPeopertyProviderを実装しているかもしれないobj</param>
        ''' <param name="name">子Propertyの名前</param>
        ''' <returns>取れなければNothing</returns>
        Public Shared Function GetPropertyMember(Of T As PropertiesBase)(obj As Object, Optional name As String = "") As T
            Dim idp As IPropertiesProvider = TryCast(obj, IPropertiesProvider)
            If idp Is Nothing Then Return Nothing

            Dim doc As PropertiesBase = idp.Document

            If String.IsNullOrEmpty(name) Then
                Dim prop As T = TryCast(doc.GetValue(GetType(T).Name), T)
                If prop IsNot Nothing Then Return prop

                Dim m_prop As T = TryCast(doc.GetValue("m_" & GetType(T).Name), T)
                If m_prop IsNot Nothing Then Return m_prop
            Else
                Dim prop As T = TryCast(doc.GetValue(name), T)
                If prop IsNot Nothing Then Return prop
            End If

            Return Nothing
        End Function
    End Class
End Namespace
