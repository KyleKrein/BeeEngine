//
// Created by alexl on 24.11.2023.
//

#include "WindowsDropSource.h"
#include "Windowing/WindowHandler/WindowHandler.h"
#include "Core/Events/EventImplementations.h"
#include "Core/Application.h"
#include "Core/Input.h"
#include "WindowsString.h"
#include "imgui.h"

namespace BeeEngine
{
    namespace Internal
    {

        IDropSource *WindowsDropSource::CreateInstance()
        {
            WindowsDropSource *pDropSource = new WindowsDropSource();
            pDropSource->AddRef();
            return pDropSource;
        }

        HRESULT STDMETHODCALLTYPE WindowsDropSource::QueryInterface(REFIID riid, void **ppvObject)
        {
            if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropSource))
            {
                *ppvObject = static_cast<IDropSource *>(this);
                AddRef();
                return S_OK;
            }
            else
            {
                *ppvObject = NULL;
                return E_NOINTERFACE;
            }
        }

        ULONG STDMETHODCALLTYPE WindowsDropSource::AddRef()
        {
            return ++m_nRefCount;
        }

        ULONG STDMETHODCALLTYPE WindowsDropSource::Release()
        {
            --m_nRefCount;
            if (m_nRefCount > 0)
                return m_nRefCount;
            delete this;
            return 0;
        }

        HRESULT STDMETHODCALLTYPE WindowsDropSource::QueryContinueDrag(BOOL fEscapePressed, DWORD grfKeyState)
        {
            if (fEscapePressed)
                return DRAGDROP_S_CANCEL;

            if (!(grfKeyState & (MK_LBUTTON | MK_RBUTTON)))
                return DRAGDROP_S_DROP;
            Application::GetInstance().AddEvent(CreateScope<FileDragEvent>(Input::GetMouseX(), Input::GetMouseY()));
            return S_OK;
        }

        HRESULT STDMETHODCALLTYPE WindowsDropSource::GiveFeedback(DWORD dwEffect)
        {
            return DRAGDROP_S_USEDEFAULTCURSORS;
        }

        WindowsDropSource::WindowsDropSource(): m_nRefCount(0)
        {

        }

        WindowsDropSource::~WindowsDropSource()
        {

        }
        IDataObject *GetFileDataObject(const TCHAR *pszFile, REFIID riid)
        {
            IDataObject *pDataObject = NULL;
            IShellFolder *pShellFolder;
            PIDLIST_RELATIVE pidlRelative;
            PIDLIST_ABSOLUTE pidl;
            HRESULT hr;

            pidl = ILCreateFromPath(pszFile);
            if (!pidl)
                return NULL;

            hr = SHBindToParent(pidl, IID_IShellFolder, (void**)&pShellFolder, (PCUITEMID_CHILD *)&pidlRelative);
            if (FAILED(hr))
            {
                ::CoTaskMemFree(pidl);
                return NULL;
            }

            const ITEMIDLIST *array[1] = { pidlRelative };
            hr = pShellFolder->GetUIObjectOf(NULL, ARRAYSIZE(array), array, riid, NULL, (void**)&pDataObject);

            pShellFolder->Release();
            ILFree(pidlRelative);
            ::CoTaskMemFree(pidl);

            if (FAILED(hr))
                pDataObject = NULL;

            return pDataObject;
        }

        HRESULT WindowsDropTarget::DragEnter(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
        {
            auto& io = ImGui::GetIO();
            io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
            Application::GetInstance().AddEvent(CreateScope<FileDragEnterEvent>(pt.x, pt.y));
            return S_OK;
        }

        HRESULT WindowsDropTarget::DragOver(DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
        {
            auto& io = ImGui::GetIO();
            io.AppAcceptingEvents = true;
            io.AppFocusLost = false;
            int32_t x = pt.x, y = pt.y;
            x-= WindowHandler::GetInstance()->GetXPosition();
            y-= WindowHandler::GetInstance()->GetYPosition();
            bool isFocused = Application::GetInstance().IsFocused();
            io.MousePos.x = isFocused ? pt.x : x;
            io.MousePos.y = isFocused ? pt.y : y;
            Application::GetInstance().AddEvent(CreateScope<FileDragEvent>(io.MousePos.x, io.MousePos.y));
            //Application::GetInstance().AddEvent(CreateScope<FileDragEvent>(x, y));
            return S_OK;
        }

        HRESULT WindowsDropTarget::DragLeave(void)
        {
            Application::GetInstance().AddEvent(CreateScope<FileDragLeaveEvent>(0, 0));//position is unknown
            return S_OK;
        }

        HRESULT WindowsDropTarget::Drop(IDataObject *pDataObj, DWORD grfKeyState, POINTL pt, DWORD *pdwEffect)
        {
            FORMATETC fmtetc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
            STGMEDIUM stgmed;

            if (FAILED(pDataObj->GetData(&fmtetc, &stgmed)))
                return E_INVALIDARG;

            HDROP hDrop = (HDROP)GlobalLock(stgmed.hGlobal);
            if (!hDrop)
                return E_INVALIDARG;

            UINT nFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, NULL, 0);
            auto event = CreateScope<FileDropEvent>();
            for (UINT i = 0; i < nFiles; i++)
            {
                WCHAR szFile[MAX_PATH];
                if (DragQueryFileW(hDrop, i, szFile, MAX_PATH))
                {
                    event->AddFile(Internal::WStringToUTF8(szFile));
                }
            }
            Application::GetInstance().AddEvent(std::move(event));
            GlobalUnlock(stgmed.hGlobal);
            ReleaseStgMedium(&stgmed);

            auto& io = ImGui::GetIO();
            io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
            return S_OK;
        }

        HRESULT WindowsDropTarget::QueryInterface(const IID &riid, void **ppvObject)
        {
            if (IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IDropTarget))
            {
                *ppvObject = static_cast<IDropTarget *>(this);
                AddRef();
                return S_OK;
            }
            else
            {
                *ppvObject = NULL;
                return E_NOINTERFACE;
            }
        }

        ULONG WindowsDropTarget::AddRef(void)
        {
            return ++m_nRefCount;
        }

        ULONG WindowsDropTarget::Release(void)
        {
            --m_nRefCount;
            if (m_nRefCount > 0)
                return m_nRefCount;
            delete this;
            return 0;
        }

        IDropTarget *WindowsDropTarget::CreateInstance()
        {
            WindowsDropTarget *pDropTarget = new WindowsDropTarget();
            pDropTarget->AddRef();
            return pDropTarget;
        }

        WindowsDropTarget::WindowsDropTarget()
        {

        }
    } // BeeEngine
} // Internal