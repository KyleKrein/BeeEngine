//
// Created by alexl on 04.06.2023.
//

#pragma once
#include "incbin.h"

#define PathToAsset(path) ASSETS_PATH "/" path
#define EmbedResource(name, path) extern "C" { INCBIN(name, PathToAsset(path)); }


EmbedResource(OpenSans, "OpenSans/OpenSans-VariableFont.ttf");