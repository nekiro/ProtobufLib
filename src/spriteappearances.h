/*
    Copyright (c) 2022 Marcin "Nekiro" Jałocha

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#ifndef SPRITEAPPEARANCES_H
#define SPRITEAPPEARANCES_H

#include "definitions.h"
#include "libbmp.h"

namespace nekiro_proto
{

enum class SpriteLayout
{
    ONE_BY_ONE = 0,
    ONE_BY_TWO = 1,
    TWO_BY_ONE = 2,
    TWO_BY_TWO = 3
};

struct SpriteSize {
    public:
        SpriteSize() {}
        SpriteSize(int height, int width) : height(height), width(width) {}

        void resize(int height, int width) {
            this->height = height;
            this->width = width;
        }
        void setHeight(int height) {
            this->height = height;
        }
        void setWidth(int width) {
            this->width = width;
        }
        int area() const {
            return width * height;
        }

        int height = 32;
        int width = 32;
};

struct EXPORT Sprite {
    public:
        Sprite() {
            pixels.resize(32 * 32 * 4, 0);
        }

        bool save(const std::string file, bool fixMagenta = false) {
            BmpImg image(size.width, size.height, pixels.data());
            return image.write(file, fixMagenta) == BMP_OK;
        }

        std::vector<uint8_t> pixels;
        SpriteSize size;
};

class EXPORT SpriteSheet
{
    public:
        SpriteSheet(int firstId, int lastId, SpriteLayout spriteLayout, const std::string& path) : firstId(firstId), lastId(lastId), spriteLayout(spriteLayout), path(path) {}

        SpriteSize getSpriteSize() {
            SpriteSize size(SPRITE_SIZE, SPRITE_SIZE);

            switch (spriteLayout) {
                case SpriteLayout::ONE_BY_ONE: break;
                case SpriteLayout::ONE_BY_TWO: size.setHeight(64); break;
                case SpriteLayout::TWO_BY_ONE: size.setWidth(64); break;
                case SpriteLayout::TWO_BY_TWO: size.resize(64, 64); break;
                default: break;
            }

            return size;
        }

        bool exportSheetImage(const std::string& file, bool fixMagenta = false) {
            BmpImg image(384, 384, data.get());
            return image.write(file, fixMagenta) == BMP_OK;
        };

        int firstId = 0;
        int lastId = 0;
        SpriteLayout spriteLayout = SpriteLayout::ONE_BY_ONE;
        std::unique_ptr<uint8_t[]> data;
        std::string path;
        bool loaded = false;
};

using SpriteSheetPtr = std::shared_ptr<SpriteSheet>;
using BmpImgPtr = std::shared_ptr<BmpImg>;
using SpritePtr = std::shared_ptr<Sprite>;

class EXPORT SpriteAppearances
{
    public:
        /**
         * @brief Loads all sprite sheets from the specified directory.
         * 
         * @param dir The directory containing the sprite sheets.
         * @param loadData If true, loads the data of the sprite sheets.
         */
        void loadSpriteSheets(const std::string& dir, bool loadData = true);

        /**
         * @brief Loads a single sprite sheet.
         * 
         * @param sheet The sprite sheet to load.
         */
        void loadSpriteSheet(const SpriteSheetPtr& sheet);

        /**
         * @brief Retrieves the sprite sheet containing the specified sprite ID.
         * 
         * @param id The ID of the sprite.
         * @param load If true, loads the sprite sheet if not already loaded.
         * @return SpriteSheetPtr The sprite sheet containing the sprite ID.
         */
        SpriteSheetPtr getSheetBySpriteId(int id, bool load = true);

        /**
         * @brief Exports the image of the specified sprite to a file.
         * 
         * @param id The ID of the sprite.
         * @param path The file path to save the sprite image.
         */
        void exportSpriteImage(int id, const std::string& path);

        /**
         * @brief Retrieves the sprite with the specified ID.
         * 
         * @param id The ID of the sprite.
         * @return SpritePtr The sprite with the specified ID.
         */
        SpritePtr getSprite(int id);

        /**
         * @brief Gets the total number of sprites.
         * 
         * @return int The total number of sprites.
         */
        int getSpritesCount() const {
            return spritesCount;
        }

    private:
        /**
         * @brief Retrieves the image of the specified sprite.
         * 
         * @param id The ID of the sprite.
         * @return BmpImgPtr The image of the specified sprite.
         */
        BmpImgPtr getSpriteImage(int id);

        int spritesCount = 0;
        std::vector<SpriteSheetPtr> sheets;
        std::map<int, SpritePtr> sprites;
};

}

#endif
