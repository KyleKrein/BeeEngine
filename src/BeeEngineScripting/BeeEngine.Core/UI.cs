using BeeEngine.Internal;

namespace BeeEngine.UI;
public delegate void UIEventCallback();
public class Document
{
		public Document(string name)
		{
				if (!name.Contains(".rml"))
				{
						name += ".rml";
				}
				m_Handle = InternalCalls.UI_CreateDocument(name);
				if (m_Handle == 0)
				{
						throw new FileNotFoundException(name);
				}
		}
		public void Show()
		{
				ThrowIfInvalid();
				InternalCalls.UI_ShowDocument(m_Handle);
		}
		public void Hide()
		{
				ThrowIfInvalid();
				InternalCalls.UI_HideDocument(m_Handle);
		}
		public void Close()
		{
				ThrowIfInvalid();
				InternalCalls.UI_CloseDocument(m_Handle);
				m_Handle = 0;
		}
		public void SetText(string elementId, string text)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetText(m_Handle, elementId, text);
		}
		public void SetVisibility(string elementId, bool visible)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetVisibility(m_Handle, elementId, visible);
		}
		public void SetClass(string elementId, string className, bool add)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetClass(m_Handle, elementId, className, add);
		}
		public void SetAttribute(string elementId, string attributeName, string value)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetAttribute(m_Handle, elementId, attributeName, value);
		}
		public void BindEvent(string elementId, string eventType, UIEventCallback callback)
		{
				ThrowIfInvalid();
				InternalCalls.UI_BindEvent(m_Handle, elementId, eventType, callback);
		}
		public string? CreateElement(string parentId, string tagName, string? elementId = null)
		{
				ThrowIfInvalid();
				if (elementId is null)
				{
						elementId = Guid.NewGuid().ToString();
				}
				return InternalCalls.UI_CreateElement(m_Handle, parentId, tagName, elementId);
		}
		public void DeleteElement(string elementId)
		{
				ThrowIfInvalid();
				InternalCalls.UI_DeleteElement(m_Handle, elementId);
		}
		public void SetStyle(string elementId, string property, string value)
		{
				ThrowIfInvalid();
				InternalCalls.UI_SetStyle(m_Handle, elementId, property, value);
		}
		public void ClearChildren(string elementId)
		{
				ThrowIfInvalid();
				InternalCalls.UI_ClearChildren(m_Handle, elementId);
		}
		void ThrowIfInvalid()
		{
				Log.AssertAndThrow(m_Handle != 0, "Document is invalid");
		}
		private ulong m_Handle;
}
