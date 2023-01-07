using System;
using System.Collections.Generic;

namespace Hesternal.HeaderTool.Parser
{
    internal sealed class ParsingContext
    {
        private enum ParsingContextType
        {
            None,
            Class,
            Namespace,
            Struct,

            ChtClass,
            ChtStruct,
        }

        private sealed class Context
        {
            public object CppDescObject;
            public ParsingContextType Type;

            public Context(ParsingContextType type)
            {
                CppDescObject = null;
                Type = type;
            }

            public Context()
                : this(ParsingContextType.None)
            {
            }

            public Context(CppStructDesc cppStructDesc)
            {
                CppDescObject = cppStructDesc;
                Type = ParsingContextType.ChtStruct;
            }

            public Context(CppClassDesc cppClassDesc)
            {
                CppDescObject = cppClassDesc;
                Type = ParsingContextType.ChtClass;
            }
        }


        private Context m_currentContext = new Context(ParsingContextType.None);
        private readonly Stack<Context> m_contextStack = new Stack<Context>();

        public bool IsNoneContext      => m_currentContext.Type == ParsingContextType.None;
        public bool IsChtStructContext => m_currentContext.Type == ParsingContextType.ChtStruct;
        public bool IsChtClassContext  => m_currentContext.Type == ParsingContextType.ChtClass;


        public bool TryGetCppStructDesc(out CppStructDesc cppStructDesc)
        {
            return _TryGetCppDescObject(IsChtStructContext, out cppStructDesc);
        }

        public bool TryGetCppClassDesc(out CppClassDesc cppClassDesc)
        {
            return _TryGetCppDescObject(IsChtClassContext, out cppClassDesc);
        }

        private bool _TryGetCppDescObject<TCppDesc>(bool canGet, out TCppDesc cppDescObject)
        {
            if (canGet)
            {
                cppDescObject = (TCppDesc)m_currentContext.CppDescObject;
                return true;
            }
            cppDescObject = default;
            return false;
        }


        public void PushContext_Class()     => _PushContext(ParsingContextType.Class);
        public void PushContext_Namespace() => _PushContext(ParsingContextType.Namespace);
        public void PushContext_Struct()    => _PushContext(ParsingContextType.Struct);

        public void PushContext(CppStructDesc cppStructDesc)
        {
            if (IsNoneContext == false)
            {
                m_contextStack.Push(m_currentContext);
            }

            m_currentContext = new Context(cppStructDesc);
        }

        public void PushContext(CppClassDesc cppClassDesc)
        {
            if (IsNoneContext == false)
            {
                m_contextStack.Push(m_currentContext);
            }

            m_currentContext = new Context(cppClassDesc);
        }

        private void _PushContext(ParsingContextType type)
        {
            if (IsNoneContext == false)
            {
                m_contextStack.Push(m_currentContext);
            }

            m_currentContext = new Context(type);
        }


        public void PopContext()
        {
            if (m_contextStack.Count != 0)
            {
                m_currentContext = m_contextStack.Pop();
            }
            else if (IsNoneContext == false)
            {
                m_currentContext = new Context();
            }
            else
            {
                throw new InvalidOperationException("Trying to Pop ParsingContext when it's already None");
            }
        }
    }
}
