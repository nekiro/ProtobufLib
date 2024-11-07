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

#include "spriteappearances.h"
#include "lzma.h"
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>

using json = nlohmann::json;
namespace fs = std::filesystem;

namespace nekiro_proto
{

void SpriteAppearances::loadSpriteSheets(const std::string& dir, bool loadData /* true*/)
{
    if (!fs::is_directory(dir)) {
        std::stringstream ss;
		ss << "Given directory isn't directory. (" << dir << ")";
        throw std::exception(ss.str().c_str());
    }

    fs::path catalogPath = fs::path(dir) / fs::path("catalog-content.json");
    if (!fs::exists(catalogPath)) {
        std::stringstream ss;
		ss << "catalog-content.json is not present in given directory. (" << catalogPath.string() << ")";
        throw std::exception(ss.str().c_str());
    }

    std::ifstream file(catalogPath, std::ios::in);
    if (!file.is_open()) {
        throw std::exception("Unable to open catalog-content.json.");
    }

    json document = json::parse(file, nullptr, false);

    file.close();

    for (const auto& obj : document) {
        const auto& type = obj["type"];
        if (type == "sprite") {
            int lastSpriteId = obj["lastspriteid"].get<int>();

            SpriteSheetPtr sheet = SpriteSheetPtr(new SpriteSheet(obj["firstspriteid"].get<int>(), lastSpriteId, static_cast<SpriteLayout>(obj["spritetype"].get<int>()), (fs::path(dir) / fs::path(obj["file"].get<std::string>())).string()));
            sheets.push_back(sheet);

            spritesCount = std::max<int>(spritesCount, lastSpriteId);

            if (loadData) {
                loadSpriteSheet(sheet);
            }
        }
    }
}

void SpriteAppearances::loadSpriteSheet(const SpriteSheetPtr& sheet)
{
    if (sheet->loaded) {
        return;
    }

    std::ifstream file(sheet->path, std::ios::binary | std::ios::in);
    if (!file.is_open()) {
        throw std::exception("Unable to open given file.");
    }

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));

    int pos = 0;

    file.close();

     /*
        CIP's header, always 32 (0x20) bytes.
        Header format:
        [0x00, X):          A variable number of NULL (0x00) bytes. The amount of pad-bytes can vary depending on how many
                            bytes the "7-bit integer encoded LZMA file size" take.
        [X, X + 0x05):      The constant byte sequence [0x70 0x0A 0xFA 0x80 0x24]
        [X + 0x05, 0x20]:   LZMA file size (Note: excluding the 32 bytes of this header) encoded as a 7-bit integer
    */

    while (buffer[pos++] == 0x00);
    pos += 4;
    while ((buffer[pos++] & 0x80) == 0x80);

    uint8_t lclppb = buffer[pos++];

    lzma_options_lzma options{};
    options.lc = lclppb % 9;

    int remainder = lclppb / 9;
    options.lp = remainder % 5;
    options.pb = remainder / 5;

    uint32_t dictionarySize = 0;
    for (uint8_t i = 0; i < 4; ++i) {
        dictionarySize += buffer[pos++] << (i * 8);
    }

    options.dict_size = dictionarySize;

    pos += 8; // cip compressed size

    lzma_stream stream = LZMA_STREAM_INIT;

    lzma_filter filters[2] = {
        lzma_filter{LZMA_FILTER_LZMA1, &options},
        lzma_filter{LZMA_VLI_UNKNOWN, NULL}
    };

    lzma_ret ret = lzma_raw_decoder(&stream, filters);
    if (ret != LZMA_OK) {
        std::stringstream ss;
		ss << "failed to initialize lzma raw decoder result: " << static_cast<int>(ret);
        throw std::exception(ss.str().c_str());
    }

    std::unique_ptr<uint8_t[]> decompressed = std::make_unique<uint8_t[]>(LZMA_UNCOMPRESSED_SIZE); // uncompressed size, bmp file + 122 bytes header

    stream.next_in = &buffer[pos];
    stream.next_out = decompressed.get();
    stream.avail_in = buffer.size();
    stream.avail_out = LZMA_UNCOMPRESSED_SIZE;

    ret = lzma_code(&stream, LZMA_RUN);
    if (ret != LZMA_STREAM_END) {
        std::stringstream ss;
		ss << "failed to decode lzma buffer result: " << static_cast<int>(ret);
        throw std::exception(ss.str().c_str());
    }

    lzma_end(&stream); // free memory

    // pixel data start (bmp header end offset)
    uint32_t data;
    std::memcpy(&data, decompressed.get() + 10, sizeof(uint32_t));

    sheet->data = std::make_unique<uint8_t[]>(LZMA_UNCOMPRESSED_SIZE);
    std::memcpy(sheet->data.get(), decompressed.get() + data, BYTES_IN_SPRITE_SHEET);
    sheet->loaded = true;
}

SpriteSheetPtr SpriteAppearances::getSheetBySpriteId(int id, bool load /* = true */)
{
    if (id == 0) {
        return nullptr;
    }

    // find sheet
    auto sheetIt = std::find_if(sheets.begin(), sheets.end(), [=](SpriteSheetPtr sheet) {
        return id >= sheet->firstId && id <= sheet->lastId;
    });

    if (sheetIt == sheets.end()) {
        return nullptr;
    }

    const SpriteSheetPtr& sheet = *sheetIt;

    if (load && !sheet->loaded) {
        loadSpriteSheet(sheet);
    }

    return sheet;
}

void SpriteAppearances::exportSpriteImage(int id, const std::string& path)
{
    BmpImgPtr image = getSpriteImage(id);
    if (image) {
        image->write(path);
    }
}

BmpImgPtr SpriteAppearances::getSpriteImage(int id)
{
    SpritePtr sprite = getSprite(id);
    if (!sprite) {
        throw std::exception("unknown sprite id");
    }

    BmpImgPtr image(new BmpImg(sprite->size.width, sprite->size.height, sprite->pixels.data()));
    return image;
}

SpritePtr SpriteAppearances::getSprite(int spriteId)
{
    // caching
    auto it = sprites.find(spriteId);
    if (it != sprites.end()) {
        return it->second;
    }

    SpriteSheetPtr sheet = getSheetBySpriteId(spriteId);
    if (!sheet || !sheet->loaded) {
        return nullptr;
    }

    const SpriteSize& size = sheet->getSpriteSize();

    SpritePtr sprite = SpritePtr(new Sprite());

    sprite->pixels.resize(size.area() * 4, 0);

    int spriteOffset = spriteId - sheet->firstId;
    int allColumns = size.width == 32 ? 12 : 6; // 64 pixel width == 6 columns each 64x or 32 pixels, 12 columns
    int spriteRow = static_cast<int>(std::floor(static_cast<float>(spriteOffset) / static_cast<float>(allColumns)));
    int spriteColumn = spriteOffset % allColumns;

    int spriteWidthBytes = size.width * 4;

    for (int height = size.height * spriteRow, offset = 0; height < size.height + (spriteRow * size.height); height++, offset++) {
        std::memcpy(&sprite->pixels[offset * spriteWidthBytes], &sheet->data[(height * SPRITE_SHEET_WIDTH_BYTES) + (spriteColumn * spriteWidthBytes)], spriteWidthBytes);
    }

    // cache it for faster later access
    sprites[spriteId] = sprite;

    return sprite;
}

}