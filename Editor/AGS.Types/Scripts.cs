using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using System.Xml;

namespace AGS.Types
{
    public class Scripts : IEnumerable
    {        
        private IList<ScriptAndHeader> _scripts;
        private Script headerWithoutScript;

        public Scripts()
        {
            _scripts = new ScriptFolders();
        }

        public Scripts(ScriptFolders scriptFolders)
        {
            _scripts = scriptFolders;
        }

        public void Clear()
        {
            _scripts.Clear();
        }

        public void Add(Script newScript)
        {
            if (headerWithoutScript != null || !newScript.IsHeader)
            {
                ScriptAndHeader scriptAndHeader = new ScriptAndHeader(headerWithoutScript, newScript);
                _scripts.Add(scriptAndHeader);
                headerWithoutScript = null;
            }
            else
            {
                headerWithoutScript = newScript;
            }            
        }

        public void AddAtTop(Script newScript)
        {
            if (headerWithoutScript != null || !newScript.IsHeader)
            {
                ScriptAndHeader scriptAndHeader = new ScriptAndHeader(headerWithoutScript, newScript);
                _scripts.Insert(0, scriptAndHeader);
                headerWithoutScript = null;
            }
            else
            {
                headerWithoutScript = newScript;
            }            
        }

        public void Remove(Script script)
        {
            for (int i = _scripts.Count; i >= 0; i--)
            {
                ScriptAndHeader scriptAndHeader = _scripts[i];
                if (script.Equals(scriptAndHeader.Header) ||
                    script.Equals(scriptAndHeader.Script))
                {
                    _scripts.RemoveAt(i);
                    break;
                }
            }            
        }

		public Script FindMatchingScriptOrHeader(Script scriptOrHeader)
		{
            foreach (ScriptAndHeader scriptAndHeader in _scripts)
            {
                if (scriptOrHeader.Equals(scriptAndHeader.Header)) return scriptAndHeader.Script;
                if (scriptOrHeader.Equals(scriptAndHeader.Script)) return scriptAndHeader.Header;
            }
            return null;
		}

        public void MoveScriptAndHeaderUp(Script script)
        {
            int index;
            ScriptAndHeader scriptAndHeader = FindScriptAndHeader(script, out index);
            if (scriptAndHeader == null) return;

            _scripts.RemoveAt(index);
            _scripts.Insert(index - 1, scriptAndHeader);                         
        }

        public void MoveScriptAndHeaderDown(Script script)
        {
            int index;
            ScriptAndHeader scriptAndHeader = FindScriptAndHeader(script, out index);
            if (scriptAndHeader == null) return;

            if (index < _scripts.Count - 1)
            {
                _scripts.RemoveAt(index);
                _scripts.Insert(index + 1, scriptAndHeader);                
            }
        }

        private ScriptAndHeader FindScriptAndHeader(Script script, out int index)
        {
            int currentIndex;
            ScriptAndHeader scriptAndHeader = null;
            for (currentIndex = 0; currentIndex < _scripts.Count; currentIndex++)
            {
                scriptAndHeader = _scripts[currentIndex];
                if (script.Equals(scriptAndHeader.Header) || script.Equals(scriptAndHeader.Script))
                {
                    index = currentIndex;
                    return scriptAndHeader;
                }
            }
            index = -1;
            return null;        
        }


        public int Count
        {
            get { return _scripts.Count; }
        }

        public Script this[int index]
        {
            get 
            {
                int currentIndex = -1;
                foreach (ScriptAndHeader scriptAndHeader in _scripts)
                {
                    if (scriptAndHeader.Header != null)
                    {
                        currentIndex++;
                        if (currentIndex == index) return scriptAndHeader.Header;
                    }
                    currentIndex++;
                    if (currentIndex == index) return scriptAndHeader.Script;
                }
                return null;
            }
        }

        public Script GetScriptByFilename(string filename)
        {
            foreach (ScriptAndHeader scriptAndHeader in _scripts)
            {
                if (scriptAndHeader.Header != null && scriptAndHeader.Header.FileName == filename)
                {
                    return scriptAndHeader.Header;
                }
                if (scriptAndHeader.Script.FileName == filename)
                {
                    return scriptAndHeader.Script;
                }
            }
            return null;
        }

        public IEnumerator GetEnumerator()
        {
            foreach (ScriptAndHeader scriptAndHeader in _scripts)
            {
                if (scriptAndHeader.Header != null) yield return scriptAndHeader.Header;
                yield return scriptAndHeader.Script;
            }
        }

        public void RefreshProjectTree(IAGSEditor editor, string selectedNodeID)
        {
            foreach (IEditorComponent component in editor.Components)
            {
                if ((component.ComponentID == "Scripts") && (component is IRePopulatableComponent))
                {
                    if (selectedNodeID == null)
                    {
                        (component as IRePopulatableComponent).RePopulateTreeView();
                    }
                    else
                    {
                        (component as IRePopulatableComponent).RePopulateTreeView(selectedNodeID);
                    }
                    return;
                }
            }
        }

        public void RefreshProjectTree(IAGSEditor editor)
        {
            RefreshProjectTree(editor, null);
        }

        public Scripts(XmlNode node)
        {
            _scripts = new ScriptFolders();
            foreach (XmlNode child in node.SelectSingleNode("Scripts").ChildNodes)
            {
                Add(new Script(child));
            }
        }

        public void ToXml(XmlTextWriter writer)
        {
            writer.WriteStartElement("Scripts");

            foreach (ScriptAndHeader script in _scripts)
            {
                if (script.Header != null) script.Header.ToXml(writer);
                script.Script.ToXml(writer);
            }

            writer.WriteEndElement();
        }
    }
}
