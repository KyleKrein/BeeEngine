#pragma once

#include "Locale/Locale.h"
#include "Path.h"
#include "String.h"
#include "UUID.h"
namespace BeeEngine
{
    /// @brief Configuration for a game
    /// This struct represents a configuration file Game.cfg that
    /// should be present in the root directory of the game.
    struct GameConfig
    {
        /// @brief Name of the game. Should be shown in the window title.
        String Name;
        Path StartingScene; // TODO: change to UUID
        /// @brief Default locale for the game. Will be used if current user locale is not supported.
        Locale::Localization DefaultLocale;
        /**
         * @brief Loads the game configuration from the file.
         *
         * @param path Path to the configuration file. Default is "Game.cfg"
         * @return GameConfig representation of the configuration file.
         */
        static GameConfig Deserialize(const Path& path = Path("Game.cfg"));
        /**
         * @brief Saves the game configuration to the file.
         * Make sure to call this function after changing the configuration.
         * Should not be used in runtime.
         * @param path Path to save the configuration file.
         * Should be in game root directory with name "Game.cfg"
         */
        void Serialize(const Path& path) const;
    };
} // namespace BeeEngine