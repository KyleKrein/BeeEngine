import Cocoa

@_cdecl("openFileDialogMacOS")
public func openFileDialogMacOS(filter: UnsafePointer<CChar>) -> UnsafePointer<CChar>
{
    let openPanel = NSOpenPanel()
    openPanel.canChooseFiles = true
    openPanel.canChooseDirectories = false
    openPanel.allowsMultipleSelection = false
    openPanel.allowedFileTypes = [String(cString: filter)]

    if openPanel.runModal() == .OK
    {
        return openPanel.url?.path.cString(using: .utf8) ?? nil
    }
    return nil
}

@_cdecl("saveFileDialogMacOS")
public func saveFileDialogMacOS(filter: UnsafePointer<CChar>) -> UnsafePointer<CChar>
{
    let savePanel = NSSavePanel()
    savePanel.allowedFileTypes = [String(cString: filter)]

    if savePanel.runModal() == .OK
    {
        return savePanel.url?.path.cString(using: .utf8) ?? nil
    }
    return nil
}

import Cocoa

@_cdecl("openFolderDialogMacOS")
public func openFolderDialogMacOS() -> UnsafePointer<CChar>
{
    let openPanel = NSOpenPanel()
    openPanel.canChooseFiles = false
    openPanel.canChooseDirectories = true
    openPanel.allowsMultipleSelection = false

    if openPanel.runModal() == .OK
    {
        return openPanel.url?.path.cString(using: .utf8) ?? nil
    }
    return nil
}
