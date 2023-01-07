using System;
using System.Collections.Generic;

namespace Copium.HeaderTool.Parser
{
    internal sealed class CppTypeScope
    {
        private readonly List<string> m_scopes = new List<string>();
        private string m_currentScope = null;


        public string GetCurrent()
        {
            if (m_currentScope == null)
            {
                m_currentScope = string.Join("::", m_scopes);
            }

            return m_currentScope;
        }


        public void Push(string typeScope)
        {
            m_scopes.Add(typeScope);
            m_currentScope = null;
        }

        public void Pop()
        {
            if (m_scopes.Count == 0)
            {
                throw new InvalidOperationException("Trying to Pop already empty CppTypeScope");
            }

            m_scopes.RemoveAt(m_scopes.Count - 1);
            m_currentScope = null;
        }
    }
}
