#include "Core/Application.h"
#include "Core/Events/EventImplementations.h"
#include "MacOSDragDrop.h"

#include <Cocoa/Cocoa.h>
#include <SDL3/SDL.h>
using namespace BeeEngine;
namespace BeeEngine
{
    void __internal_bee_add_file_to_drop_event(FileDropEvent *event, String &&path)
    {
        event->AddFile(std::move(path));
    }
}
static bool isDragging = false;
void HandleNewDragAndDrop()
{
    if(!isDragging)
    {
        isDragging = true;
        Application::GetInstance().AddEvent(CreateScope<FileDragStartEvent>());
    }
}
// Пример реализации NSDraggingDestination
@interface BeeDragDropHandler : NSView <NSDraggingDestination>
@end

@implementation BeeDragDropHandler
- (instancetype)initWithFrame:(NSRect)frame {
    self = [super initWithFrame:frame];
    if (self) {
        [self registerForDraggedTypes:@[NSFilenamesPboardType, NSPICTPboardType, NSTIFFPboardType, NSPDFPboardType]]; // Вы можете настроить типы
    }
    return self;
}
- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender {
    // Здесь можно вызвать C++ функцию
    HandleNewDragAndDrop();
    Application::GetInstance().AddEvent(CreateScope<FileDragEnterEvent>(sender.draggingLocation.x, WindowHandler::GetInstance()->GetHeight() - sender.draggingLocation.y));
    return NSDragOperationGeneric; // или другой NSDragOperation в зависимости от вашей логики
}

- (BOOL)wantsPeriodicDraggingUpdates {
    return YES;
}

- (NSDragOperation)draggingUpdated:(id<NSDraggingInfo>)sender {
    // Обновление операции drag
    HandleNewDragAndDrop();
    Application::GetInstance().AddEvent(CreateScope<FileDragEvent>(sender.draggingLocation.x, WindowHandler::GetInstance()->GetHeight() - sender.draggingLocation.y));
    return NSDragOperationGeneric;
}
- (void)draggingExited:(id<NSDraggingInfo>)sender {
    // Обработка выхода drag
    Application::GetInstance().AddEvent(CreateScope<FileDragLeaveEvent>(sender.draggingLocation.x, WindowHandler::GetInstance()->GetHeight() - sender.draggingLocation.y));
}

- (void)draggingEnded:(id<NSDraggingInfo>)sender {
    // Обработка окончания drag
    Application::GetInstance().AddEvent(CreateScope<FileDragEndEvent>());
}

- (BOOL)prepareForDragOperation:(id<NSDraggingInfo>)sender {
    // Подготовка к операции drag
    return YES;
}
- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender {
    // Выполнение операции drag. Получите данные из sender
    auto event = CreateScope<FileDropEvent>();
    NSPasteboard *pboard = [sender draggingPasteboard];
    if ([[pboard types] containsObject:NSFilenamesPboardType]) {
        NSArray *files = [pboard propertyListForType:NSFilenamesPboardType];
        for (NSString* file in files) {
            __internal_bee_add_file_to_drop_event(event.get(), file.UTF8String);
        }
    }
    Application::GetInstance().AddEvent(std::move(event));
    return YES;
}

- (void)concludeDragOperation:(id<NSDraggingInfo>)sender {
    // Завершение операции drag
    isDragging = false;
    //Application::GetInstance().AddEvent(CreateScope<FileDragEndEvent>());
}
- (void)updateDraggingItemsForDrag:(id<NSDraggingInfo>)sender {
}
@end
BeeDragDropHandler * dragDropHandler = nullptr;
void IntegrateDragAndDropSDL()
{
    auto nativeInfo = WindowHandler::GetInstance()->GetNativeInfo();
    auto sdlWindow = reinterpret_cast<SDL_Window*>(WindowHandler::GetInstance()->GetWindow());
    auto nsWindow = (NSWindow*)nativeInfo.window;
    auto sdlContentView = [nsWindow contentView];
    dragDropHandler = [[BeeDragDropHandler alloc] initWithFrame:sdlContentView.frame];
    //Auto resize
    dragDropHandler.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    //NSRect frame = NSMakeRect(0, 0, width, height);
    [nsWindow.contentView addSubview:dragDropHandler];
}
