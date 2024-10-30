using System;
using System.Collections.Generic;
using System.Text;

namespace DBAccessN5CGL_TINY
{
    public class NumFeatSearchCond
    {
        public NumFeatSearchCond(int id, double l, double u)
        {
            EdasysID = id;
            lower = l;
            upper = u;
        }

        public int EdasysID;
        public double lower; // �����Ȃ���Double.NaN
        public double upper; // ����Ȃ���Double.NaN
    }

    /// <summary>
    /// ���j�o�[�T���ł�ncdetect���g�������É�5CGL�ɔ����������u��SQL�T�[�o�����r�f�[�^���擾����N���X
    /// </summary>
    public class DBAccess
    {
        public DBAccess()
        {
        }

        int m_cmd_timeout = 60;

        public int CommandTimeout
        {
            get { return m_cmd_timeout; }
            set { m_cmd_timeout = value; }
        }

        public void Init(System.Data.IDbConnection connection)
        {
            get_nangles(connection);                // �p�x���A�摜�����擾
            get_surface_cond(connection);           // �����\�ʏ�Ԃ��擾
            get_coil_attr_entries(connection);      // ��ʏ����e���擾
            get_attr_entries(connection);           // �����ʖ��̂��擾
        }

        public void Dispose()
        {
            m_surface_cond.Clear();
        }

        #region �p�x��

        /// <summary>
        /// �g�p���Ă���p�x���A�摜�����擾
        /// </summary>
        /// <param name="connection"></param>
        /// <returns></returns>
        private void get_nangles(System.Data.IDbConnection connection)
        {
            //System.Data.IDbCommand cmd = connection.CreateCommand();
            //cmd.CommandTimeout = m_cmd_timeout;
            //cmd.CommandText = @"select CamAngleNum from param_common";
            //try
            //{
            //    System.Data.IDataReader reader = cmd.ExecuteReader();
            //    try
            //    {
            //        if (!reader.Read()) return;
            //        m_nangles = reader.GetInt32(0);
            //        m_attr_nangles = m_nangles + 1;
            //    }
            //    finally
            //    {
            //        reader.Close();
            //    }
            //}
            //finally
            //{
            //    cmd.Dispose();
            //}

            // DB[PARAM_COMMON]�̍\���ύX�ɔ����A�p�x�����Œ�l�ƂȂ������߁A�����ύX
            m_nangles = 4;                      // �摜��
            m_attr_nangles = m_nangles + 1;     // �����ʐ�(����+�p�x��)
        }

        #endregion

        #region �\�ʏ�ԃR�[�h

        private System.Collections.Generic.Dictionary<int, string> m_surface_cond = new Dictionary<int, string>();

        /// <summary>
        /// �����\�ʏ�Ԃ��擾
        /// </summary>
        void get_surface_cond(System.Data.IDbConnection connection)
        {
            System.Data.IDbCommand cmd = connection.CreateCommand();
            cmd.CommandTimeout = m_cmd_timeout;
            cmd.CommandText = @"select * from M_SCODE_MASTER ";
            try
            {
                System.Data.IDataReader reader = cmd.ExecuteReader();
                try
                {
                    int index_id = reader.GetOrdinal("scode");
                    int index_name = reader.GetOrdinal("�����\�ʏ�Ԗ���");

                    while (reader.Read())
                    {
                        m_surface_cond.Add(reader.GetInt32(index_id), reader.IsDBNull(index_name) ? null : reader.GetString(index_name));
                    }
                }
                finally
                {
                    reader.Close();
                }
            }
            finally
            {
                cmd.Dispose();
            }
        }

        public string GetSurfaceCondName(int id)
        {
            return m_surface_cond.ContainsKey(id) ? m_surface_cond[id] : null;
        }

        #endregion

        #region �R�C����񂩂�̓�����

        private class CoilAttrEntry
        {
            private string m_name;
            private int m_edasys_id;
            private double m_coef;
            private bool m_is_num;
            private bool m_is_ascii;
            private string m_colname;

            public CoilAttrEntry(string name, int eda_id, double coef, bool is_ascii, string colname)
            {
                m_name = name;
                m_edasys_id = eda_id;
                m_coef = coef;
                m_is_num = true;
                m_is_ascii = is_ascii;
                m_colname = colname;
            }

            public CoilAttrEntry(string name, int eda_id, string colname)
            {
                m_name = name;
                m_edasys_id = eda_id;
                m_is_num = false;
                m_is_ascii = true;
                m_colname = colname;
            }

            public int EdasysID
            {
                get { return m_edasys_id; }
            }
            public string Name
            {
                get { return m_name; }
            }
            public double Coef
            {
                get
                {
                    return m_coef;
                }
            }
            public bool IsNum
            {
                get
                {
                    return m_is_num;
                }
            }
            public bool IsAscii
            {
                get
                {
                    return m_is_ascii;
                }
            }
            public string ColumnNameInColiInf
            {
                get
                {
                    return m_colname;
                }
            }
        };

        private CoilAttrEntry[] m_coil_attr_entries = null;

        /// <summary>
        /// ��ʏ����e���擾
        /// </summary>
        private void get_coil_attr_entries(System.Data.IDbConnection connection)
        {
            System.Collections.Generic.List<CoilAttrEntry> entries = new List<CoilAttrEntry>();

            System.Data.IDbCommand cmd = connection.CreateCommand();
            cmd.CommandText = "select * from PCINFO_DESC where EdasysID > 0 order by \"Order\"";
            try
            {
                System.Data.IDataReader reader = cmd.ExecuteReader();

                try
                {
                    int index_name = reader.GetOrdinal("Name");
                    int index_edaid = reader.GetOrdinal("EdasysID");
                    int index_ascii = reader.GetOrdinal("IsAscii");
                    int index_num = reader.GetOrdinal("IsNum");
                    int index_coef = reader.GetOrdinal("ConvCoef");
                    int index_colname = reader.GetOrdinal("CoilInfColName");

                    while (reader.Read())
                    {
                        int edaid = reader.GetInt32(index_edaid);
                        if (0 < edaid && !reader.IsDBNull(index_colname))
                        {
                            string name = reader.GetString(index_name);
                            bool is_ascii = reader.GetBoolean(index_ascii);
                            bool is_num = reader.GetBoolean(index_num);
                            double coef = reader.GetDouble(index_coef);
                            string colname = reader.GetString(index_colname);

                            CoilAttrEntry entry = is_num
                                ? new CoilAttrEntry(name, edaid, coef, is_ascii, colname)
                                : new CoilAttrEntry(name, edaid, colname);
                            entries.Add(entry);
                        }
                    }
                }
                finally
                {
                    reader.Close();
                }
            }
            finally
            {
                cmd.Dispose();
            }

            m_coil_attr_entries = entries.ToArray();
        }

        private class CoilInfFeatures
        {
            public string CoilNum = null;
            public SwdData.SparseArrayDouble NumFeatures = null;
            public SwdData.SparseArrayString StrFeatures = null;
            public CoilInfFeatures(string coil_num,
                SwdData.SparseArrayDouble num_features, SwdData.SparseArrayString str_features)
            {
                CoilNum = coil_num;
                NumFeatures = num_features;
                StrFeatures = str_features;
            }
        };

        /// <summary>
        /// key: filename, inf: �R�C�����֘A�̓�����
        /// </summary>
        private Dictionary<string, CoilInfFeatures> m_coilinf_features = new Dictionary<string, CoilInfFeatures>();

        private CoilInfFeatures get_coilinf_features(System.Data.IDbConnection connection, string filename)
        {
            if (m_coilinf_features.ContainsKey(filename)) return m_coilinf_features[filename];

            System.Data.IDbCommand cmd = connection.CreateCommand();
            try
            {
                cmd.CommandTimeout = m_cmd_timeout;
                cmd.CommandText = string.Format("select * from T_COIL_INF where �Ǘ�No = '{0}' ", filename);

                System.Data.IDataReader reader = cmd.ExecuteReader();
                try
                {
                    int index_coilnum = reader.GetOrdinal("���YNo");

                    if (reader.Read())
                    {
                        if (!reader.IsDBNull(index_coilnum))
                        {
                            SwdData.SparseArrayDouble num_features = new SwdData.SparseArrayDouble();
                            SwdData.SparseArrayString str_features = new SwdData.SparseArrayString();

                            foreach (CoilAttrEntry coilattr_entry in m_coil_attr_entries)
                            {
                                int index = reader.GetOrdinal(coilattr_entry.ColumnNameInColiInf); // CoilInf����l�������Ă���Ƃ��̃L�[

                                if (!coilattr_entry.IsAscii)
                                {
                                    int num_val = reader.IsDBNull(index)
                                        ? 0 : reader.GetInt32(index);
                                    num_features[coilattr_entry.EdasysID] = num_val * coilattr_entry.Coef;
                                }
                                else
                                {
                                    if (coilattr_entry.IsNum)
                                    {
                                        string str_val = reader.IsDBNull(index)
                                            ? "0.0" : reader.GetString(index);
                                        num_features[coilattr_entry.EdasysID] = float.Parse(str_val) * coilattr_entry.Coef;
                                    }
                                    else
                                    {
                                        string str_val = reader.IsDBNull(index)
                                            ? "" : reader.GetString(index);
                                        str_features[coilattr_entry.EdasysID] = str_val;
                                    }
                                }
                            }

                            CoilInfFeatures coilinf_features
                               = new CoilInfFeatures(reader.GetString(index_coilnum),
                                                       num_features, str_features);
                            m_coilinf_features.Add(filename, coilinf_features);

                            return coilinf_features;
                        }
                        else
                        {
                            return null;
                        }
                    }
                    else
                    {
                        return null;
                    }
                }
                finally
                {
                    reader.Close();
                }

            }
            finally
            {
                cmd.Dispose();
            }
        }

        #endregion

        #region �����ʃe�[�u��

        /// <summary>
        /// �摜��(=�p�x��)
        /// </summary>
        private int m_nangles = 0;

        /// <summary>
        /// �����ʃe�[�u����(���ʂ��܂ނ̂ŁA�p�x�� + 1)
        /// </summary>
        private int m_attr_nangles = 1;

        private readonly string[] m_attr_tables_up   = new string[] { "T_DEFECT_ATTR0_T", "T_DEFECT_ATTR1_T", "T_DEFECT_ATTR2_T", "T_DEFECT_ATTR3_T", "T_DEFECT_ATTR4_T" };
        private readonly string[] m_attr_tables_down = new string[] { "T_DEFECT_ATTR0_B", "T_DEFECT_ATTR1_B", "T_DEFECT_ATTR2_B", "T_DEFECT_ATTR3_B", "T_DEFECT_ATTR4_B" };

        private class AttrTableEntry
        {
            public AttrTableEntry(int index, int eda_id, string name)
            {
                this.Index    = index;
                this.EdasysID = eda_id;
                this.Name     = name;
            }
            public int Index; // �����ʃe�[�u���ɂ�����ԍ��B��"������99"�̂Ƃ���99�B
            public int EdasysID;
            public string Name;
        };

        /// <summary>
        /// �����ʖ��̂��擾 [�p�x(0:����, 1�`�p�x��)][]
        /// </summary>
        AttrTableEntry[][] m_attr_entries = null;

        void get_attr_entries(System.Data.IDbConnection connection)
        {
            System.Data.IDbCommand cmd = connection.CreateCommand();
            try
            {
                cmd.CommandTimeout = m_cmd_timeout;
                cmd.CommandText = @"select * from ATTR_NAME where EdasysID > 0 ";
                System.Collections.Generic.List<AttrTableEntry>[] entries = new List<AttrTableEntry>[m_attr_nangles];
                for (int i = 0; i < m_attr_nangles; i++) entries[i] = new List<AttrTableEntry>();
                System.Data.IDataReader reader = cmd.ExecuteReader();
                try
                {
                    int index_angle    = reader.GetOrdinal("Angle");
                    int index_index    = reader.GetOrdinal("Enum");
                    int index_edasysid = reader.GetOrdinal("EdasysID");
                    int index_name     = reader.GetOrdinal("Name");

                    while (reader.Read())
                    {
                        int angle = reader.GetInt32(index_angle);
                        if (angle < m_attr_nangles)
                        {
                            entries[angle].Add(new AttrTableEntry(
                                reader.GetInt32(index_index),
                                reader.GetInt32(index_edasysid),
                                reader.GetString(index_name)));
                        }
                    }

                    m_attr_entries = new AttrTableEntry[m_attr_nangles][];
                    for (int i = 0; i < m_attr_nangles; i++)
                    {
                        m_attr_entries[i] = entries[i].ToArray();
                    }
                }
                finally
                {
                    reader.Close();
                }
            }
            finally
            {
                cmd.Dispose();
            }
        }

        /// <summary>
        /// ������EdasysID��angle��index�ɕ�������
        /// </summary>
        /// <param name="edasys_id"></param>
        /// <param name="angle">0: ���ʁA1�ȏ�F�p�x��</param>
        /// <param name="index"></param>
        /// <returns></returns>
        bool get_attr_angle_index(int edasys_id, out int angle, out int index)
        {
            for (int i = 0; i < m_attr_nangles; i++)
            {
                AttrTableEntry[] tbls = m_attr_entries[i];
                for (int j = 0; j < tbls.Length; j++)
                {
                    if (edasys_id == tbls[j].EdasysID)
                    {
                        angle = i;
                        index = tbls[j].Index;
                        return true;
                    }
                }
            }

            angle = -1;
            index = -1;
            return false;
        }

        #endregion

        #region ���ڃ��X�g�̊l��

        /// <summary>
        /// �r��A�O���[�h�A�����ʖ��́A��ʏ����e���擾
        /// </summary>
        public SwdData.TermList GetTermlist(System.Data.IDbConnection connection)
        {
            SwdData.TermList termlist = new SwdData.TermList();
            System.Data.IDbCommand cmd = null;

            // �r�햼��
            cmd = connection.CreateCommand();
            cmd.CommandTimeout = m_cmd_timeout;
            cmd.CommandText = @"select * from TYPE_NAME ";
            try
            {
                System.Data.IDataReader reader = cmd.ExecuteReader();
                try
                {
                    int index_eda_id = reader.GetOrdinal("EdasysID");
                    int index_name   = reader.GetOrdinal("Name");
                    while (reader.Read())
                    {
                        int eda_id  = reader.GetInt32(index_eda_id);
                        string name = reader.GetString(index_name);
                        int r = 255;
                        int g = 0;
                        int b = 0;

                        SwdData.TermItem term_item = new SwdData.TermItem(
                            SwdData.Terms.DefectType,
                            eda_id,
                            name,
                            System.Drawing.Color.FromArgb(r, g, b));
                        termlist.AddTermItem(term_item);
                    }
                }
                finally
                {
                    reader.Close();
                }
            }
            finally
            {
                cmd.Dispose();
            }

            // �O���[�h����
            cmd = connection.CreateCommand();
            cmd.CommandTimeout = m_cmd_timeout;
            cmd.CommandText = @"select * from GRADE_NAME ";
            try
            {
                System.Data.IDataReader reader = cmd.ExecuteReader();
                try
                {
                    int index_eda_id = reader.GetOrdinal("EdasysID");
                    int index_name   = reader.GetOrdinal("Name");
                    int index_color  = reader.GetOrdinal("�}�b�v�\���F");
                    while (reader.Read())
                    {
                        int eda_id = reader.GetInt32(index_eda_id);
                        string name = reader.GetString(index_name);
                        int color = reader.GetInt32(index_color);

                        SwdData.TermItem term_item = new SwdData.TermItem(
                            SwdData.Terms.DefectSevirity,
                            eda_id,
                            name,
                            System.Drawing.Color.FromArgb(color));
                        termlist.AddTermItem(term_item);
                    }
                }
                finally
                {
                    reader.Close();
                }
            }
            finally
            {
                cmd.Dispose();
            }

            // �����ʖ���
            foreach (AttrTableEntry[] attr_entry_angle in m_attr_entries)
            {
                foreach (AttrTableEntry attr_entry in attr_entry_angle)
                {
                    SwdData.TermItem term_item = new SwdData.TermItem(
                        SwdData.Terms.NumericFeature,
                        attr_entry.EdasysID,
                        attr_entry.Name);
                    termlist.AddTermItem(term_item);
                }
            }

            // �R�C����񂩂�̓�����
            foreach (CoilAttrEntry coilattr_entry in m_coil_attr_entries)
            {
                if (coilattr_entry.IsNum)
                {
                    termlist.AddTermItem(new SwdData.TermItem(
                        SwdData.Terms.NumericFeature,
                        coilattr_entry.EdasysID,
                        coilattr_entry.Name));
                }
                else
                {
                    termlist.AddTermItem(new SwdData.TermItem(
                        SwdData.Terms.StringFeature,
                        coilattr_entry.EdasysID,
                        coilattr_entry.Name));
                }
            }
            return termlist;
        }

#endregion

#region �R�C�����̊l��

        /// <summary>
        /// �R�C���ԍ����w�肵�āA�o���P�ʂ̃R�C������Ԃ�
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="coil_num">���葤�R�C���̎��ʎq</param>
        /// <param name="count_on_type"></param>
        /// <param name="count_on_sevirity"></param>
        /// <returns></returns>
        public CoilListItem[] GetCoil(
            System.Data.IDbConnection connection,
            string coil_num,
            bool count_on_type,
            bool count_on_sevirity)
        {
            System.Data.IDbCommand cmd = connection.CreateCommand();

            try
            {
                cmd.CommandTimeout = m_cmd_timeout;
                StringBuilder sb = new StringBuilder();
                sb.Append("select ");
                sb.Append(" CI.�Ǘ�No, CI.�o�^���t, CI.���YNo, CI.�ޗ���, CI.������, CI.�����_�\, CI.�����_��");
                sb.Append(", CI.��ގ�, CI.�\�ʏ���1����, CI.�\�ʏ���2����, CI.�v�掟�H���R�[�h");
                sb.Append(", CR.���������� as ���������� ");
                sb.Append(", ISNULL( SUB.�����J�b�g��, 0 ) as �����J�b�g�� ");

                sb.Append(", CR.�����\�ʏ��No_�\ as �����\�ʏ��No_�\ ");
                sb.Append(", CR.�����\�ʏ��No_�� as �����\�ʏ��No_�� ");
                sb.Append(", CR.�\�ۑ��r�� as �\�ۑ��r�� ");
                sb.Append(", CR.���ۑ��r�� as ���ۑ��r�� ");
                sb.Append(", CCP.�擪�ʒu as �擪�ʒu ");
                sb.Append(", (CONVERT(bigint, CCP.�擪�ʒu) + CONVERT(bigint, CCP.�ۗL����)) as �����J�b�g�ʒu ");
                sb.Append(" from T_COIL_INF as CI ");
                sb.Append(" inner join T_COIL_RESULT as CR on CI.�Ǘ�No = CR.�Ǘ�No ");
                sb.Append(" left outer join T_COIL_CUT_POS as CCP on CR.�Ǘ�No = CCP.�Ǘ�No ");

                sb.Append("left outer join ");
                sb.Append("(");
                sb.Append("  select ccp.�Ǘ�No ");
                sb.Append(", count(*) as �����J�b�g�� ");
                sb.Append("	from T_COIL_CUT_POS as ccp ");
                sb.Append("	inner join T_COIL_INF as ci ");
                sb.Append("	on ccp.�Ǘ�No = ci.�Ǘ�No ");
                sb.Append("	group by ccp.�Ǘ�No ");
                sb.Append(") as SUB on SUB.�Ǘ�No = CI.�Ǘ�No ");


                sb.Append(" where CI.���YNo = '");
                sb.Append(coil_num);
                sb.Append("'");
                cmd.CommandText = sb.ToString();

                System.Collections.Generic.List<CoilListItem> coils = new List<CoilListItem>();
                collect_coils(cmd, coils);

                if (count_on_type || count_on_sevirity)
                {
                    get_ndefects_on_coil(connection, coils, count_on_type, count_on_sevirity, null);
                }

                return 0 == coils.Count ? null : coils.ToArray();
            }
            finally
            {
                cmd.Dispose();
            }
        }

        /// <summary>
        /// ���t�͈͂��w�肵�āA�o���P�ʂ̃R�C������Ԃ�
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="from"></param>
        /// <param name="to"></param>
        /// <param name="count_on_type"></param>
        /// <param name="count_on_sevirity"></param>
        /// <param name="progress"></param>
        /// <returns></returns>
        public CoilListItem[] GetCoils(
            System.Data.IDbConnection connection,
            System.DateTime from,
            System.DateTime to,
            bool count_on_type,
            bool count_on_sevirity,
            SwdData.IFormProgress progress)
        {
            string from_txt = "{" + string.Format("ts '{0:0000}-{1:00}-{2:00} {3:00}:{4:00}:{5:00}'", new object[] { from.Year, from.Month, from.Day, from.Hour, from.Minute, from.Second }) + "}";
            string to_txt = "{" + string.Format("ts '{0:0000}-{1:00}-{2:00} {3:00}:{4:00}:{5:00}'", new object[] { to.Year, to.Month, to.Day, to.Hour, to.Minute, to.Second }) + "}";

            System.Data.IDbCommand cmd = connection.CreateCommand();
            try
            {
                cmd.CommandTimeout = m_cmd_timeout;
                StringBuilder sb = new StringBuilder();
                sb.Append("select ");
                sb.Append(" CI.�Ǘ�No, CI.�o�^���t, CI.���YNo, CI.�ޗ���, CI.������, CI.�����_�\, CI.�����_��");
                sb.Append(", CI.��ގ�, CI.�\�ʏ���1����, CI.�\�ʏ���2����, CI.�v�掟�H���R�[�h");
                sb.Append(", CR.���������� as ���������� ");
                sb.Append(", ISNULL( SUB.�����J�b�g��, 0 ) as �����J�b�g�� ");

                sb.Append(", CR.�����\�ʏ��No_�\ as �����\�ʏ��No_�\ ");
                sb.Append(", CR.�����\�ʏ��No_�� as �����\�ʏ��No_�� ");
                sb.Append(", CR.�\�ۑ��r�� as �\�ۑ��r�� ");
                sb.Append(", CR.���ۑ��r�� as ���ۑ��r�� ");
                sb.Append(", CCP.�擪�ʒu as �擪�ʒu ");
                sb.Append(", (CONVERT(bigint, CCP.�擪�ʒu) + CONVERT(bigint, CCP.�ۗL����)) as �����J�b�g�ʒu ");
                sb.Append(" from T_COIL_INF as CI ");
                sb.Append(" inner join T_COIL_RESULT as CR on CI.�Ǘ�No = CR.�Ǘ�No ");
                sb.Append(" left outer join T_COIL_CUT_POS as CCP on CR.�Ǘ�No = CCP.�Ǘ�No ");

                sb.Append("left outer join ");
                sb.Append("(");
                sb.Append("  select ccp.�Ǘ�No ");
                sb.Append(", count(*) as �����J�b�g�� ");
                sb.Append("	from T_COIL_CUT_POS as ccp ");
                sb.Append("	inner join T_COIL_INF as ci ");
                sb.Append("	on ccp.�Ǘ�No = ci.�Ǘ�No ");
                sb.Append("	group by ccp.�Ǘ�No ");
                sb.Append(") as SUB on SUB.�Ǘ�No = CI.�Ǘ�No ");

                sb.Append(" where CI.�o�^���t between " + from_txt + " and " + to_txt);
                sb.Append(" order by CR.�Ǘ�No desc, CCP.�����q�� asc ");
                cmd.CommandText = sb.ToString();

                System.Collections.Generic.List<CoilListItem> coils = new List<CoilListItem>();
                collect_coils(cmd, coils);

                if (count_on_type || count_on_sevirity)
                {
                    if (null != progress)
                    {
                        progress.Val = 0;
                        progress.Max = coils.Count;
                    }
                    get_ndefects_on_coil(connection, coils, count_on_type, count_on_sevirity, progress);
                }

                return 0 == coils.Count ? null : coils.ToArray();
            }
            finally
            {
                cmd.Dispose();
            }
        }

        /// <summary>
        /// �R�C�������擾���ACoilListItem�ɓo�^
        /// </summary>
        /// <param name="cmd"></param>
        /// <param name="coils"></param>
        private void collect_coils(System.Data.IDbCommand cmd, System.Collections.Generic.List<CoilListItem> coils)
        {
            System.Data.IDataReader reader = cmd.ExecuteReader();

            const int pos_to_margin = 1000; // [mm]

            try
            {
                int index_filename = reader.GetOrdinal("�Ǘ�No");
                int index_datetime = reader.GetOrdinal("�o�^���t");
                int index_coilnum  = reader.GetOrdinal("���YNo");
                int index_length = reader.GetOrdinal("����������");
                int index_ncut = reader.GetOrdinal("�����J�b�g��");
                int index_cutposfrom = reader.GetOrdinal("�擪�ʒu");
                int index_cutposto = reader.GetOrdinal("�����J�b�g�ʒu");
                int index_width     = reader.GetOrdinal("�ޗ���");
                int index_thickness = reader.GetOrdinal("������");
                int index_surface_up = reader.GetOrdinal("�����\�ʏ��No_�\");
                int index_surface_dn = reader.GetOrdinal("�����\�ʏ��No_��");
                int index_insp_level_up = reader.GetOrdinal("�����_�\");
                int index_insp_level_dn = reader.GetOrdinal("�����_��");
                int index_primary_context = reader.GetOrdinal("��ގ�");
                int index_surf_proc_dig1 = reader.GetOrdinal("�\�ʏ���1����");
                int index_surf_proc_dig2 = reader.GetOrdinal("�\�ʏ���2����");
                int index_next_proc_sched = reader.GetOrdinal("�v�掟�H���R�[�h");
                int index_total_ndefects_up = reader.GetOrdinal("�\�ۑ��r��");
                int index_total_ndefects_dn = reader.GetOrdinal("���ۑ��r��");
                int i = 0;

                while (reader.Read())
                {
                    string filename = reader.GetString(index_filename);
                    string coilnum = reader.IsDBNull(index_coilnum) ? ("coil_" + filename) : reader.GetString(index_coilnum);

                    int surface_up = reader.GetInt32(index_surface_up);
                    int surface_dn = reader.GetInt32(index_surface_dn);
                    string surface_up_txt = m_surface_cond.ContainsKey(surface_up) ? m_surface_cond[surface_up] : null;
                    string surface_dn_txt = m_surface_cond.ContainsKey(surface_dn) ? m_surface_cond[surface_dn] : null;

                    int ncut = reader.GetInt32(index_ncut);
                    if (0 < ncut)
                    {
                        // �R�C����������
                        {
                            CoilListItem coil = new CoilListItem(
                                reader.GetDateTime(index_datetime),
                                filename,
                                coilnum,
                                i,
                                ncut - 1,
                                reader.GetInt32(index_cutposfrom),
                                i == ncut - 1 ? reader.GetInt32(index_length) : (int)reader.GetInt64(index_cutposto),
                                i == ncut - 1 ? reader.GetInt32(index_length) + pos_to_margin : (int)reader.GetInt64(index_cutposto),
                                reader.GetInt32(index_width),
                                reader.GetInt32(index_thickness) / 1000.0F,
                                reader.GetInt32(index_length),
                                surface_up_txt, surface_dn_txt,
                                reader.IsDBNull(index_insp_level_up) ? null : reader.GetString(index_insp_level_up),
                                reader.IsDBNull(index_insp_level_dn) ? null : reader.GetString(index_insp_level_dn),
                                reader.IsDBNull(index_primary_context) ? null : reader.GetString(index_primary_context),
                                reader.IsDBNull(index_surf_proc_dig1) ? null : reader.GetString(index_surf_proc_dig1),
                                reader.IsDBNull(index_surf_proc_dig2) ? null : reader.GetString(index_surf_proc_dig2),
                                reader.IsDBNull(index_next_proc_sched) ? null : reader.GetString(index_next_proc_sched),
                                reader.IsDBNull(index_total_ndefects_up) ? 0 : reader.GetInt32(index_total_ndefects_up),
                                reader.IsDBNull(index_total_ndefects_dn) ? 0 : reader.GetInt32(index_total_ndefects_dn)
                            );
                            coils.Add(coil);
                        }

                        // ���[�v�ϐ��C���N�������g
                        i++;
                        if (i == ncut)
                        {
                            //CoilListItem coil = new CoilListItem(
                            //    reader.GetDateTime(index_datetime),
                            //    filename,
                            //    coilnum,
                            //    i,
                            //    ncut,
                            //    reader.GetInt32(index_cutposfrom),
                            //    reader.GetInt32(index_length),
                            //    reader.GetInt32(index_length) + pos_to_margin,
                            //    reader.GetInt32(index_width),
                            //    reader.GetInt32(index_thickness) / 1000.0F,
                            //    reader.GetInt32(index_length),
                            //    surface_up_txt, surface_dn_txt,
                            //    reader.IsDBNull(index_insp_level_up) ? null : reader.GetString(index_insp_level_up),
                            //    reader.IsDBNull(index_insp_level_dn) ? null : reader.GetString(index_insp_level_dn),
                            //    reader.IsDBNull(index_primary_context) ? null : reader.GetString(index_primary_context),
                            //    reader.IsDBNull(index_surf_proc_dig1) ? null : reader.GetString(index_surf_proc_dig1),
                            //    reader.IsDBNull(index_surf_proc_dig2) ? null : reader.GetString(index_surf_proc_dig2),
                            //    reader.IsDBNull(index_next_proc_sched) ? null : reader.GetString(index_next_proc_sched),
                            //    reader.IsDBNull(index_total_ndefects_up) ? 0 : reader.GetInt32(index_total_ndefects_up),
                            //    reader.IsDBNull(index_total_ndefects_dn) ? 0 : reader.GetInt32(index_total_ndefects_dn)
                            //);
                            //coils.Add(coil);
                            i = 0;
                        }

                    }
                    else
                    {
                        // �R�C�������Ȃ�
                        CoilListItem coil = new CoilListItem(
                            reader.GetDateTime(index_datetime),
                            filename,
                            coilnum,
                            0,
                            0,
                            0,
                            reader.GetInt32(index_length),
                            reader.GetInt32(index_length) + pos_to_margin,
                            reader.GetInt32(index_width),
                            reader.GetInt32(index_thickness) / 1000.0F,
                            reader.GetInt32(index_length),
                            surface_up_txt, surface_dn_txt,
                            reader.IsDBNull(index_insp_level_up) ? null : reader.GetString(index_insp_level_up),
                            reader.IsDBNull(index_insp_level_dn) ? null : reader.GetString(index_insp_level_dn),
                            reader.IsDBNull(index_primary_context) ? null : reader.GetString(index_primary_context),
                            reader.IsDBNull(index_surf_proc_dig1) ? null : reader.GetString(index_surf_proc_dig1),
                            reader.IsDBNull(index_surf_proc_dig2) ? null : reader.GetString(index_surf_proc_dig2),
                            reader.IsDBNull(index_next_proc_sched) ? null : reader.GetString(index_next_proc_sched),
                            reader.IsDBNull(index_total_ndefects_up) ? 0 : reader.GetInt32(index_total_ndefects_up),
                            reader.IsDBNull(index_total_ndefects_dn) ? 0 : reader.GetInt32(index_total_ndefects_dn)
                        );
                        coils.Add(coil);
                    }
                }
            }
            finally
            {
                reader.Close();
            }
        }

        /// <summary>
        /// �r��E�O���[�h�����r��
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="coils"></param>
        /// <param name="on_type"></param>
        /// <param name="on_sevirity"></param>
        /// <param name="progress">���O��Max�͐ݒ肵�Ă�������</param>
        private void get_ndefects_on_coil(System.Data.IDbConnection connection,
            System.Collections.Generic.IEnumerable<CoilListItem> coils,
            bool on_type, bool on_sevirity,
            SwdData.IFormProgress progress)
        {
            System.Data.IDbCommand cmd = connection.CreateCommand();
            cmd.CommandTimeout = m_cmd_timeout;

            try
            {
                foreach (CoilListItem coil in coils)
                {
                    // �r�했
                    if (on_type)
                    {
                        // �\
                        cmd.CommandText = string.Format(
                            @"select d.�����r�� as �����r��, count(d.�����r��) as �r�� " +
                            @"from T_DEFECT_INF_T as d " +
                            @"inner join T_COIL_RESULT as c on d.�Ǘ�No = c.�Ǘ�No " +
                            @"where d.�Ǘ�No='{0}' and d.����ʒu between {1} and {2} " +
                            @"group by d.�����r�� ",
                            coil.Filename,
                            coil.PosFrom,
                            coil.PosToDefect);
                        System.Data.IDataReader reader = cmd.ExecuteReader();
                        try
                        {
                            int index_type = reader.GetOrdinal("�����r��");
                            int index_n    = reader.GetOrdinal("�r��");

                            while (reader.Read())
                            {
                                int type = reader.GetInt32(index_type);
                                int n    = reader.GetInt32(index_n);
                                coil.SetNDefectsOnType(SwdData.InspSurface.Up, type, n);
                            }
                        }
                        finally
                        {
                            reader.Close();
                        }

                        // ��
                        cmd.CommandText = string.Format(
                            @"select d.�����r�� as �����r��, count(d.�����r��) as �r�� " +
                            @"from T_DEFECT_INF_B as d " +
                            @"inner join T_COIL_RESULT as c on d.�Ǘ�No = c.�Ǘ�No " +
                            @"where d.�Ǘ�No='{0}' and d.����ʒu between {1} and {2} " +
                            @"group by d.�����r�� ",
                            coil.Filename,
                            coil.PosFrom,
                            coil.PosToDefect);
                        reader = cmd.ExecuteReader();
                        try
                        {
                            int index_type = reader.GetOrdinal("�����r��");
                            int index_n    = reader.GetOrdinal("�r��");

                            while (reader.Read())
                            {
                                int type = reader.GetInt32(index_type);
                                int n    = reader.GetInt32(index_n);
                                coil.SetNDefectsOnType(SwdData.InspSurface.Down, type, n);
                            }
                        }
                        finally
                        {
                            reader.Close();
                        }
                    }

                    // �O���[�h��
                    if (on_sevirity)
                    {
                        // �\
                        cmd.CommandText = string.Format(
                            @"select d.����O���[�h as ����O���[�h, count(d.����O���[�h) as �r�� " +
                            @"from T_DEFECT_INF_T as d " +
                            @"inner join T_COIL_RESULT as c on d.�Ǘ�No = c.�Ǘ�No " +
                            @"where d.�Ǘ�No='{0}' and d.����ʒu between {1} and {2} " +
                            @"group by d.����O���[�h ",
                            coil.Filename,
                            coil.PosFrom,
                            coil.PosToDefect);
                        System.Data.IDataReader reader = cmd.ExecuteReader();
                        try
                        {
                            int index_sevirity = reader.GetOrdinal("����O���[�h");
                            int index_n        = reader.GetOrdinal("�r��");

                            while (reader.Read())
                            {
                                int sevirity = reader.GetInt32(index_sevirity);
                                int n        = reader.GetInt32(index_n);
                                coil.SetNDefectsOnSevirity(SwdData.InspSurface.Up, sevirity, n);
                            }
                        }
                        finally
                        {
                            reader.Close();
                        }

                        // ��
                        cmd.CommandText = string.Format(
                            @"select d.����O���[�h as ����O���[�h, count(d.����O���[�h) as �r�� " +
                            @"from T_DEFECT_INF_B as d " +
                            @"inner join T_COIL_RESULT as c on d.�Ǘ�No = c.�Ǘ�No " +
                            @"where d.�Ǘ�No='{0}' and d.����ʒu between {1} and {2} " +
                            @"group by d.����O���[�h ",
                            coil.Filename,
                            coil.PosFrom,
                            coil.PosToDefect);
                        reader = cmd.ExecuteReader();
                        try
                        {
                            int index_sevirity = reader.GetOrdinal("����O���[�h");
                            int index_n        = reader.GetOrdinal("�r��");

                            while (reader.Read())
                            {
                                int sevirity = reader.GetInt32(index_sevirity);
                                int n        = reader.GetInt32(index_n);
                                coil.SetNDefectsOnSevirity(SwdData.InspSurface.Down, sevirity, n);
                            }
                        }
                        finally
                        {
                            reader.Close();
                        }
                    }

                    if (null != progress)
                    {
                        progress.Val++;
                        if (progress.IsCanceled) break;
                    }
                }
            }
            finally
            {
                cmd.Dispose();
            }
        }

#endregion

#region �r�n���h���̊l��

        /// <summary>
        /// 
        /// </summary>
        /// <param name="filename"></param>
        /// <param name="types"></param>
        /// <param name="sevirities"></param>
        /// <param name="postop_from">[m]</param>
        /// <param name="postop_to">[m]</param>
        /// <param name="surface"></param>
        /// <param name="tblname_inf"></param>
        /// <param name="connection"></param>
        /// <param name="handles"></param>
        private void get_defect_handle(CoilListItem coil, int[] types, int[] sevirities,
            float postop_from, float postop_to, SwdData.InspSurface surface,
            string tblname_inf, 
            System.Data.IDbConnection connection, System.Collections.Generic.List<DefectHandle> handles)
        {
            string type_list = null;
            if (null != types)
            {
                type_list = "";
                for(int i=0; i<types.Length; i++)
                {
                    type_list += types[i].ToString();
                    if (i != types.Length - 1) type_list += ",";
                }
            }
            string sevirity_list = null;
            if (null != sevirities)
            {
                sevirity_list = "";
                for (int i = 0; i < sevirities.Length; i++)
                {
                    sevirity_list += sevirities[i].ToString();
                    if (i != sevirities.Length - 1) sevirity_list += ",";
                }
            }

            System.Data.IDbCommand cmd = connection.CreateCommand();
            cmd.CommandTimeout = m_cmd_timeout;
            try
            {
                cmd.CommandText = string.Format(
                    @"select d.�rNo, d.�����r��, d.����O���[�h " +
                    @"from {0} as d " +
                    @"inner join T_COIL_RESULT as c on d.�Ǘ�No = c.�Ǘ�No " +
                    @"where d.�Ǘ�No='{1}' and d.����ʒu between {2} and {3} ",
                    tblname_inf,
                    coil.Filename,
                    coil.PosFrom,
                    coil.PosToDefect);
                if (null != type_list)
                {
                    cmd.CommandText += string.Format(" and �����r�� in ({0})", type_list);
                }
                if (null != sevirity_list)
                {
                    cmd.CommandText += string.Format(" and ����O���[�h in ({0})", sevirity_list);
                }
                if (!float.IsNaN(postop_from))
                {
                    cmd.CommandText += string.Format(" and ����ʒu >= {0}", (int)(postop_from * 1000) + coil.PosFrom);
                }
                if (!float.IsNaN(postop_to))
                {
                    cmd.CommandText += string.Format(" and ����ʒu <= {0}", (int)(postop_to * 1000) + coil.PosFrom);
                }

                System.Data.IDataReader reader = cmd.ExecuteReader();
                try
                {
                    int index = reader.GetOrdinal("�rNo");
                    while (reader.Read())
                    {
                        handles.Add(new DefectHandle(coil.DateTime, coil.Filename, surface, reader.GetInt32(index)));
                    }
                }
                finally
                {
                    reader.Close();
                }
            }
            finally
            {
                cmd.Dispose();
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="handles"></param>
        /// <param name="tblname_inf"></param>
        /// <param name="tblname_coilinf"></param>
        /// <param name="tblname_attr"></param>
        /// <param name="use_datetime"></param>
        /// <param name="datetime_from"></param>
        /// <param name="datetime_to"></param>
        /// <param name="coil_num"></param>
        /// <param name="surface">Up or Down�B���������ł͂Ȃ�Handle�𐶐�����Ƃ��Ɏg�p�����B</param>
        /// <param name="types"></param>
        /// <param name="sevirities"></param>
        /// <param name="use_width"></param>
        /// <param name="width_lower">[mm]</param>
        /// <param name="width_upper">[mm]</param>
        /// <param name="use_length"></param>
        /// <param name="length_lower">[mm]</param>
        /// <param name="length_upper">[mm]</param>
        /// <param name="use_posleft"></param>
        /// <param name="posleft_lower">[mm]</param>
        /// <param name="posleft_upper">[mm]</param>
        /// <param name="use_posright"></param>
        /// <param name="posright_lower">[mm]</param>
        /// <param name="posright_upper">[mm]</param>
        /// <param name="use_postop"></param>
        /// <param name="postop_lower">[m]</param>
        /// <param name="postop_upper">[m]</param>
        /// <param name="numfeat_cond"></param>
        /// <param name="insp_level_up"></param>
        /// <param name="insp_level_dn"></param>
        /// <param name="prim_cntxt"></param>
        /// <param name="surf_proc_dig1"></param>
        /// <param name="next_proc_sched"></param>
        /// <param name="progress"></param>
        private void get_defect_handle(
            System.Data.IDbConnection connection,
            List<DefectHandle> handles,
            string tblname_inf, string tblname_coilinf, string[] tblname_attr,
            bool use_datetime, DateTime datetime_from, DateTime datetime_to,
            string coil_num,
            SwdData.InspSurface surface,
            IEnumerable<int> types, IEnumerable<int> sevirities,
            bool use_width, float width_lower, float width_upper,
            bool use_length, float length_lower, float length_upper,
            bool use_posleft, float posleft_lower, float posleft_upper,
            bool use_posright, float posright_lower, float posright_upper,
            bool use_postop, float postop_lower, float postop_upper,
            NumFeatSearchCond[] numfeat_cond,
            string insp_level_up,
            string insp_level_dn,
            string prim_cntxt,
            string surf_proc_dig1,
            string next_proc_sched,
            SwdData.IFormProgress progress)
        {
            // ���l�����������̕���
            List<int>[] numfeatcond_index = new List<int>[m_attr_nangles];
            List<double>[] numfeatcond_upper = new List<double>[m_attr_nangles];
            List<double>[] numfeatcond_lower = new List<double>[m_attr_nangles];
            if (null != numfeat_cond)
            {
                foreach (NumFeatSearchCond cond in numfeat_cond)
                {
                    int angle;
                    int index;
                    if (get_attr_angle_index(cond.EdasysID, out angle, out index))
                    {
                        if (null == numfeatcond_index[angle]) numfeatcond_index[angle] = new List<int>();
                        if (null == numfeatcond_lower[angle]) numfeatcond_lower[angle] = new List<double>();
                        if (null == numfeatcond_upper[angle]) numfeatcond_upper[angle] = new List<double>();
                        numfeatcond_index[angle].Add(index);
                        numfeatcond_lower[angle].Add(cond.lower);
                        numfeatcond_upper[angle].Add(cond.upper);
                    }
                }
            }

            // �r���� ���������ɏ]���ASQL������
            System.Data.IDbCommand cmd = connection.CreateCommand();
            cmd.CommandTimeout = m_cmd_timeout;
            try
            {
                cmd.CommandText = string.Format(
                    @"select d.�Ǘ�No, d.�rNo, ci.�o�^���t " +
                    @"from {0} as d " +
                    @"inner join {1} as ci on d.�Ǘ�No = ci.�Ǘ�No " +
                    @"inner join T_COIL_RESULT as cr on d.�Ǘ�No = cr.�Ǘ�No ",
                    tblname_inf, tblname_coilinf);

                // �K�v�ȓ����ʃe�[�u����join
                for (int i = 0; i < m_attr_nangles; i++)
                {
                    if (null != numfeatcond_index[i])
                    {
                        cmd.CommandText += string.Format(
                            @"inner join {1} on d.�Ǘ�No = {1}.�Ǘ�No and d.�rNo = {1}.�rNo ",
                            tblname_inf, tblname_attr[i]);
                    }
                }

                cmd.CommandText += @"where 1 = 1 ";

                if (use_datetime)
                {
                    string from_txt = "{" + string.Format("ts '{0:0000}-{1:00}-{2:00} {3:00}:{4:00}:{5:00}'", new object[] { datetime_from.Year, datetime_from.Month, datetime_from.Day, datetime_from.Hour, datetime_from.Minute, datetime_from.Second }) + "}";
                    string to_txt = "{" + string.Format("ts '{0:0000}-{1:00}-{2:00} {3:00}:{4:00}:{5:00}'", new object[] { datetime_to.Year, datetime_to.Month, datetime_to.Day, datetime_to.Hour, datetime_to.Minute, datetime_to.Second }) + "}";

                    cmd.CommandText += string.Format(@"and ci.�o�^���t >= {1} and ci.�o�^���t <= {2} ",
                        tblname_coilinf, from_txt, to_txt);
                }
                if (null != coil_num && coil_num.Length > 0)
                {
                    cmd.CommandText += string.Format(@"and ci.���YNo = '{1}' ",
                        tblname_coilinf, coil_num);
                }
                if (null != types)
                {
                    string type_list = "";
                    bool first = true;
                    foreach (int type in types)
                    {
                        if (first) first = false;
                        else type_list += ",";
                        type_list += type.ToString();
                    }

                    cmd.CommandText += string.Format("and d.�����r�� in ({1}) ",
                        tblname_inf, type_list);
                }
                if (null != sevirities)
                {
                    string sevirity_list = "";
                    bool first = true;
                    foreach (int sevirity in sevirities)
                    {
                        if (first) first = false;
                        else sevirity_list += ",";
                        sevirity_list += sevirity.ToString();
                    }

                    cmd.CommandText += string.Format("and d.����O���[�h in ({1}) ",
                        tblname_inf, sevirity_list);
                }
                if (use_width)
                {
                    if (!float.IsNaN(width_lower))
                    {
                        if (!float.IsNaN(width_upper))
                        {
                            // lower and upper
                            cmd.CommandText += string.Format("and (d.�O��Xmax - d.�O��Xmin) * d.������\ between {1} and {2} ",
                                tblname_inf, width_lower, width_upper);
                        }
                        else
                        {
                            // lower
                            cmd.CommandText += string.Format("and (d.�O��Xmax - d.�O��Xmin) * d.������\ >= {1} ",
                                tblname_inf, width_lower);
                        }
                    }
                    else
                    {
                        if (!float.IsNaN(width_upper))
                        {
                            // upper
                            cmd.CommandText += string.Format("and (d.�O��Xmax - d.�O��Xmin) * d.������\ <= {1} ",
                                tblname_inf, width_upper);
                        }
                    }
                }
                if (use_length)
                {
                    if (!float.IsNaN(length_lower))
                    {
                        if (!float.IsNaN(length_upper))
                        {
                            // lower and upper
                            cmd.CommandText += string.Format("and (d.�O��Ymax - d.�O��Ymin) * d.�c����\ between {1} and {2} ",
                                tblname_inf, length_lower, length_upper);
                        }
                        else
                        {
                            // lower
                            cmd.CommandText += string.Format("and (d.�O��Ymax - d.�O��Ymin) * d.�c����\ >= {1} ",
                                tblname_inf, length_lower);
                        }
                    }
                    else
                    {
                        if (!float.IsNaN(length_upper))
                        {
                            // upper
                            cmd.CommandText += string.Format("and (d.�O��Ymax - d.�O��Ymin) * d.�c����\ <= {1} ",
                                tblname_inf, length_upper);
                        }
                    }
                }
                if (use_posleft)
                {
                    if (!float.IsNaN(posleft_lower))
                    {
                        if (!float.IsNaN(posleft_upper))
                        {
                            // lower and upper
                            cmd.CommandText += string.Format("and d.DS�ʒu between {1} and {2} ",
                                tblname_inf, posleft_lower, posleft_upper);
                        }
                        else
                        {
                            // lower
                            cmd.CommandText += string.Format("and d.DS�ʒu >= {1} ",
                                tblname_inf, posleft_lower);
                        }
                    }
                    else
                    {
                        if (!float.IsNaN(posleft_upper))
                        {
                            // upper
                            cmd.CommandText += string.Format("and d.DS�ʒu <= {1} ",
                                    tblname_inf, posleft_upper);
                        }
                    }
                }
                if (use_posright)
                {
                    if (!float.IsNaN(posright_lower))
                    {
                        if (!float.IsNaN(posright_upper))
                        {
                            // lower and upper
                            cmd.CommandText += string.Format("and d.WS�ʒu between {1} and {2} ",
                                tblname_inf, posright_lower, posright_upper);
                        }
                        else
                        {
                            // lower
                            cmd.CommandText += string.Format("and d.WS�ʒu >= {1} ",
                                tblname_inf, posright_lower);
                        }
                    }
                    else
                    {
                        if (!float.IsNaN(posright_upper))
                        {
                            // upper
                            cmd.CommandText += string.Format("and d.WS�ʒu <= {1} ",
                                    tblname_inf, posright_upper);
                        }
                    }
                }
                if (use_postop)
                {
                    if (!float.IsNaN(postop_lower))
                    {
                        if (!float.IsNaN(postop_upper))
                        {
                            // lower and upper
                            cmd.CommandText += string.Format("and d.����ʒu between {1} and {2} ",
                                tblname_inf, postop_lower * 1000, postop_upper * 1000);
                        }
                        else
                        {
                            // lower
                            cmd.CommandText += string.Format("and d.����ʒu >= {1} ",
                                tblname_inf, postop_lower * 1000);
                        }
                    }
                    else
                    {
                        if (!float.IsNaN(postop_upper))
                        {
                            // upper
                            cmd.CommandText += string.Format("and d.����ʒu <= {1} ",
                                    tblname_inf, postop_upper * 1000);
                        }
                    }
                }
                if (null != insp_level_up && insp_level_up.Length > 0)
                {
                    cmd.CommandText += string.Format("and ci.�����_�\ = '{1}' ",
                        tblname_coilinf, insp_level_up);
                }
                if (null != insp_level_dn && insp_level_dn.Length > 0)
                {
                    cmd.CommandText += string.Format("and ci.�����_�� = '{1}' ",
                        tblname_coilinf, insp_level_dn);
                }
                if (null != prim_cntxt && prim_cntxt.Length > 0)
                {
                    cmd.CommandText += string.Format("and ci.��ގ� = '{1}' ",
                        tblname_coilinf, prim_cntxt);
                }
                if (null != surf_proc_dig1 && surf_proc_dig1.Length > 0)
                {
                    cmd.CommandText += string.Format("and ci.�\�ʏ���1���� = '{1}' ",
                        tblname_coilinf, surf_proc_dig1);
                }
                if (null != next_proc_sched && next_proc_sched.Length > 0)
                {
                    cmd.CommandText += string.Format("and ci.�v�掟�H���R�[�h = '{1}' ",
                        tblname_coilinf, next_proc_sched);
                }
                for (int i = 0; i < m_attr_nangles; i++)
                {
                    string tblname = tblname_attr[i];
                    List<int> list_index = numfeatcond_index[i];
                    List<double> list_lower = numfeatcond_lower[i];
                    List<double> list_upper = numfeatcond_upper[i];
                    if (null != list_index)
                    {
                        for (int j = 0; j < list_index.Count; j++)
                        {
                            int index = list_index[j];
                            double lower = list_lower[j];
                            double upper = list_upper[j];
                            if (!double.IsNaN(lower))
                            {
                                if (!double.IsNaN(upper))
                                {
                                    cmd.CommandText += string.Format("and {0}.������{1} between {2} and {3} ",
                                        tblname, index, lower, upper);
                                }
                                else
                                {
                                    cmd.CommandText += string.Format("and {0}.������{1} >= {2} ",
                                        tblname, index, lower);
                                }
                            }
                            else
                            {
                                if (!double.IsNaN(upper))
                                {
                                    cmd.CommandText += string.Format("and {0}.������{1} <= {2} ",
                                        tblname, index, upper);
                                }
                            }
                        }
                    }
                }

                System.Data.IDataReader reader = cmd.ExecuteReader();
                try
                {
                    int index_datetime = reader.GetOrdinal("�o�^���t");
                    int index_filename = reader.GetOrdinal("�Ǘ�No");
                    int index_defectnum = reader.GetOrdinal("�rNo");
                    while (reader.Read())
                    {
                        if( !reader.IsDBNull(index_filename) && !reader.IsDBNull(index_defectnum) )
                        {
                            DateTime datetime = reader.GetDateTime(index_datetime);
                            string filename = reader.GetString(index_filename);
                            int defect_num = reader.GetInt32(index_defectnum);
                            DefectHandle handle = new DefectHandle(datetime, filename, surface, defect_num);
                            handles.Add(handle);
                        }
                    }
                }
                finally
                {
                    reader.Dispose();
                }
            }
            finally
            {
                cmd.Dispose();
            }
        }


        /// <summary>
        /// �r���擾
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="coil"></param>
        /// <param name="types"></param>
        /// <param name="sevirities"></param>
        /// <param name="postop_from">[m]</param>
        /// <param name="postop_to">[m]</param>
        /// <param name="surface"></param>
        /// <param name="handles"></param>
        public void GetDefectHandle(System.Data.IDbConnection connection,
            CoilListItem coil,
            int[] types, int[] sevirities,
            float postop_from, float postop_to,
            SwdData.InspSurface surface,
            System.Collections.Generic.List<DefectHandle> handles)
        {
            switch (surface)
            {
                case SwdData.InspSurface.Up:
                    get_defect_handle(coil, types, sevirities, postop_from, postop_to,
                        SwdData.InspSurface.Up, "T_DEFECT_INF_T", connection, handles);
                    break;

                case SwdData.InspSurface.Down:
                    get_defect_handle(coil, types, sevirities, postop_from, postop_to,
                        SwdData.InspSurface.Down, "T_DEFECT_INF_B", connection, handles);
                    break;

                case SwdData.InspSurface.Both:
                    get_defect_handle(coil, types, sevirities, postop_from, postop_to,
                        SwdData.InspSurface.Up, "T_DEFECT_INF_T", connection, handles);
                    get_defect_handle(coil, types, sevirities, postop_from, postop_to,
                        SwdData.InspSurface.Down, "T_DEFECT_INF_B", connection, handles);
                    break;
            }
        }
        
        /// <summary>
        /// �r����
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="use_datetime"></param>
        /// <param name="datetime_from"></param>
        /// <param name="datetime_to"></param>
        /// <param name="coil_num">�K�v�Ȃ����null</param>
        /// <param name="use_surface"></param>
        /// <param name="surface"></param>
        /// <param name="types">�K�v�Ȃ����null</param>
        /// <param name="sevirities">�K�v�Ȃ����null</param>
        /// <param name="use_width"></param>
        /// <param name="width_lower"></param>
        /// <param name="width_upper"></param>
        /// <param name="use_length"></param>
        /// <param name="length_lower"></param>
        /// <param name="length_upper"></param>
        /// <param name="use_posleft"></param>
        /// <param name="posleft_lower"></param>
        /// <param name="posleft_upper"></param>
        /// <param name="use_posright"></param>
        /// <param name="posright_lower"></param>
        /// <param name="posright_upper"></param>
        /// <param name="use_postop"></param>
        /// <param name="postop_lower"></param>
        /// <param name="postop_upper"></param>
        /// <param name="insp_level_up">�K�v�Ȃ����null</param>
        /// <param name="insp_level_dn">�K�v�Ȃ����null</param>
        /// <param name="prim_cntxt">�K�v�Ȃ����null</param>
        /// <param name="surf_proc_dig1">�K�v�Ȃ����null</param>
        /// <param name="next_proc_sched">�K�v�Ȃ����null</param>
        /// <param name="progress">�K�v�Ȃ����null</param>
        /// <returns></returns>
        public object[] SearchDefects(System.Data.IDbConnection connection,
            bool use_datetime, DateTime datetime_from, DateTime datetime_to,
            string coil_num,
            bool use_surface, SwdData.InspSurface surface,
            IEnumerable<int> types, IEnumerable<int> sevirities,
            bool use_width, float width_lower, float width_upper,
            bool use_length, float length_lower, float length_upper,
            bool use_posleft, float posleft_lower, float posleft_upper,
            bool use_posright, float posright_lower, float posright_upper,
            bool use_postop, float postop_lower, float postop_upper,
            NumFeatSearchCond[] numfeat_cond,
            string insp_level_up,
            string insp_level_dn,
            string prim_cntxt,
            string surf_proc_dig1,
            string next_proc_sched,
            SwdData.IFormProgress progress)
        {
            List<DefectHandle> handles = new List<DefectHandle>();

            if (!use_surface || surface == SwdData.InspSurface.Up)
            {
                get_defect_handle(connection, handles,
                    "T_DEFECT_INF_T", "T_COIL_INF", m_attr_tables_up,
                    use_datetime, datetime_from, datetime_to,
                    coil_num,
                    SwdData.InspSurface.Up,
                    types, sevirities,
                    use_width, width_lower, width_upper,
                    use_length, length_lower, length_upper,
                    use_posleft, posleft_lower, posleft_upper,
                    use_posright, posright_lower, posright_upper,
                    use_postop, postop_lower, postop_upper,
                    numfeat_cond,
                    insp_level_up,
                    insp_level_dn,
                    prim_cntxt,
                    surf_proc_dig1,
                    next_proc_sched,
                    progress);
            }

            if (!use_surface || surface == SwdData.InspSurface.Down)
            {
                get_defect_handle(connection, handles,
                    "T_DEFECT_INF_B", "T_COIL_INF", m_attr_tables_down,
                    use_datetime, datetime_from, datetime_to,
                    coil_num,
                    SwdData.InspSurface.Down,
                    types, sevirities,
                    use_width, width_lower, width_upper,
                    use_length, length_lower, length_upper,
                    use_posleft, posleft_lower, posleft_upper,
                    use_posright, posright_lower, posright_upper,
                    use_postop, postop_lower, postop_upper,
                    numfeat_cond,
                    insp_level_up,
                    insp_level_dn,
                    prim_cntxt,
                    surf_proc_dig1,
                    next_proc_sched,
                    progress);
            }

            return 0 == handles.Count ? null : handles.ToArray();
        }



#endregion

#region �r�n���h�������r���̊l��

        /// <summary>
        /// �r�n���h�������r���̊l��
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="handles"></param>
        /// <param name="result"></param>
        /// <param name="progress">���O��Max��ݒ肵�Ă�������</param>
        /// <returns></returns>
        public int GetDefectByHandle(System.Data.IDbConnection connection,
            System.Collections.Generic.IEnumerable<DefectHandle> handles,
            SwdData.IDefectItemCollection result,
            SwdData.IFormProgress progress)
        {
            int nerror = 0;
            foreach (DefectHandle handle in handles)
            {
                SwdData.IDefectItem defect_item = GetDefectByHandle(connection, handle);
                if (null == defect_item) nerror++;
                else                     result.Add(defect_item);

                if (null != progress)
                {
                    progress.Val++;
                    if (progress.IsCanceled) break;
                }
            }

            return nerror;
        }

        EnlargeJpeg m_enlarge_async = new EnlargeJpeg();
        class ArrayMemberAssignmentOperator
        {
            SwdData.DefectImage[] images;
            int index;

            public ArrayMemberAssignmentOperator(SwdData.DefectImage[] images, int index)
            {
                this.images = images;
                this.index = index;
            }
            public void Op(SwdData.DefectImage image)
            {
                images[index] = image;
            }
        }

        /// <summary>
        /// �r���A�r�摜�A�����ʁA��ʏ��̊l��
        /// </summary>
        /// <param name="connection"></param>
        /// <param name="handles"></param>
        public SwdData.IDefectItem GetDefectByHandle(System.Data.IDbConnection connection,
            DefectHandle handle)
        {
            string   tblname_inf  = null;
            string   tblname_img  = null;
            string[] tblname_attr = null;
            switch (handle.Surface)
            {
                case SwdData.InspSurface.Up:
                    tblname_inf = "T_DEFECT_INF_T";
                    tblname_img = "T_DEFECT_IMG_T";
                    tblname_attr = m_attr_tables_up;
                    break;

                case SwdData.InspSurface.Down:
                    tblname_inf = "T_DEFECT_INF_B";
                    tblname_img = "T_DEFECT_IMG_B";
                    tblname_attr = m_attr_tables_down;
                    break;

                default:
                    return null;
            }

            System.Data.IDbCommand cmd = connection.CreateCommand();
            cmd.CommandTimeout = m_cmd_timeout;
            try
            {
                System.Data.IDataReader reader = null;

                // ��{���
                cmd.CommandText = string.Format(
                    @"select * from {0} as d "+
                    @"inner join T_COIL_RESULT as c on d.�Ǘ�No = c.�Ǘ�No "+
                    @"where c.�Ǘ�No = '{1}' and �rNo = {2} ",
                    tblname_inf, handle.FileName, handle.DefectNum);
                reader = cmd.ExecuteReader();
                int xmin, xmax, ymin, ymax;
                float resox, resoy;
                int postop;
                float posleft, posright;
                int type, sevirity, group;
                int shrink;
                try
                {
                    if (!reader.Read()) return null;
                    xmin     = (int)reader["�O��Xmin"];
                    xmax     = (int)reader["�O��Xmax"];
                    ymin     = (int)reader["�O��Ymin"];
                    ymax     = (int)reader["�O��Ymax"];
                    resox    = (float)reader["������\"];
                    resoy    = (float)reader["�c����\"];
                    postop   = (int)reader["����ʒu"];
                    posleft  = (float)reader["DS�ʒu"];
                    posright = (float)reader["WS�ʒu"];
                    type     = (int)reader["�����r��"];
                    sevirity = (int)reader["����O���[�h"];
                    shrink   = (int)reader["�k����"];
                    group = 0;

                    if (shrink != 1) {
                        xmin = (int)reader["���摜�O��Xmin"];
                        xmax = (int)reader["���摜�O��Ymin"];
                        ymin = (int)reader["���摜�O��Xmax"];
                        ymax = (int)reader["���摜�O��Ymax"];
                        resox = (float)reader["��������\"];
                        resoy = (float)reader["���c����\"];
                    }
                }
                finally
                {
                    reader.Close();
                }

                // �摜
                cmd.CommandText = string.Format(
                    @"select * from {0} as d " +
                    @"inner join T_COIL_RESULT as c on d.�Ǘ�No = c.�Ǘ�No " +
                    @"where c.�Ǘ�No = '{1}' and �rNo = {2} ",
                    tblname_img, handle.FileName, handle.DefectNum);
                reader = cmd.ExecuteReader();
                SwdData.DefectImage[] image = new SwdData.DefectImage[m_nangles];
                try
                {

                    //if (!reader.Read()) return null;
                    //int index_image0 = reader.GetOrdinal("���摜");
                    //for (int i = 0; i < m_nangles; i++)
                    //{
                    //    image[i] = new SwdData.DefectImage((byte[])reader[index_image0 + i]);
                    //}

                    if (!reader.Read())
                    {
                        for (int i = 0; i < m_nangles; i++)
                        {
                            System.IO.MemoryStream ms = new System.IO.MemoryStream();
                            Properties.Resources.Default.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                            image[i] = new SwdData.DefectImage(ms.ToArray());

                            //image[i] = new SwdData.DefectImage(Properties.Resources.Default, System.Drawing.Imaging.ImageFormat.Bmp);
                        }
                    }
                    else
                    {
                        int index_image0 = reader.GetOrdinal("���摜");
                        for (int i = 0; i < m_nangles; i++)
                        {
                            if (reader.IsDBNull(index_image0 + i))
                            {
                                System.IO.MemoryStream ms = new System.IO.MemoryStream();
                                Properties.Resources.Default.Save(ms, System.Drawing.Imaging.ImageFormat.Jpeg);
                                image[i] = new SwdData.DefectImage(ms.ToArray());
                            }
                            else
                            {
                                byte[] jpeg = reader[index_image0 + i] as byte[];
                                if (shrink == 1)
                                {
                                    image[i] = new SwdData.DefectImage(jpeg);
                                }
                                else
                                {
                                    //image[i] = EnlargeJpeg.doit(jpeg, shrink);
                                    m_enlarge_async.Enque(jpeg, shrink, new ArrayMemberAssignmentOperator(image,i).Op);
                                }
                            }
                        }
                    }
                }
                finally
                {
                    reader.Close();
                }

                // ���l������
                SwdData.SparseArrayDouble num_features = new SwdData.SparseArrayDouble();
                for (int i = 0; i < m_attr_nangles; i++)
                {
                    cmd.CommandText = string.Format(
                        @"select * from {0} where �Ǘ�No = '{1}' and �rNo = {2} ",
                        tblname_attr[i], handle.FileName, handle.DefectNum);
                    reader = cmd.ExecuteReader();
                    try
                    {
                        if (!reader.Read()) return null;
                        int index_attr0 = reader.GetOrdinal("������0");
                        foreach (AttrTableEntry attr_entry in m_attr_entries[i])
                        {
                            num_features[attr_entry.EdasysID] = reader.GetDouble(index_attr0 + attr_entry.Index);
                        }
                    }
                    finally
                    {
                        reader.Close();
                    }
                }

                // �R�C����񂩂�̓�����
                CoilInfFeatures coilinf_features = get_coilinf_features(connection, handle.FileName);
                if (null != coilinf_features)
                {
                    foreach (SwdData.SparseArrayItemDouble saitem in coilinf_features.NumFeatures)
                    {
                        num_features[saitem.Index] = saitem.Val;
                    }
                }

                System.Drawing.Rectangle rect_bbox = new System.Drawing.Rectangle(
                    xmin, ymin,
                    xmax - xmin, ymax - ymin);
                float width = rect_bbox.Width * resox;
                float length = rect_bbox.Height * resoy;

                //�񓯊��̓ǂݍ��݂�����΂����ő҂Bimage
                m_enlarge_async.WaitCompleteAll();

                SwdData.IDefectItem defect_item = new SwdData.DefectItem(
                    handle.DefectNum,
                    null != coilinf_features ? coilinf_features.CoilNum : "Unknown",
                    handle.DateTime, handle.Surface, null,
                    width, length,
                    postop / 1000.0F, posleft, posright,
                    rect_bbox,
                    type, sevirity, group,
                    type, sevirity, group,
                    type, sevirity, group,
                    num_features,
                    null != coilinf_features ? new SwdData.SparseArrayString(coilinf_features.StrFeatures) : new SwdData.SparseArrayString(),
                    resox, resoy, image, false);
                return defect_item;
            }
            finally
            {
                cmd.Dispose();
            }
        }


#endregion
    }
}
