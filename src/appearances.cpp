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

#include "definitions.h"
#include "appearances.h"

namespace nekiro
{

void Appearances::parseAppearancesFromFile(const std::string& path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        throw std::exception("Unable to open given file.");
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    // close file
    file.close();

    parseAppearancesFromMemory(buffer);
}

void Appearances::parseAppearancesFromMemory(std::stringstream& input)
{
    TibiaAppearances tibiaAppearances;
    if (!tibiaAppearances.ParseFromIstream(&input)) {
        throw std::exception("Unable to parse appearances lib.");
    }

    // store them in vector
    const google::protobuf::RepeatedPtrField<TibiaAppearance>* repeatedAppearances = nullptr;
    for (int type = OBJECT_TYPE_ITEM; type <= OBJECT_TYPE_MISSILE; type++) {
        switch (type) {
            case OBJECT_TYPE_ITEM: repeatedAppearances = &tibiaAppearances.object(); break;
            case OBJECT_TYPE_LOOKTYPE: repeatedAppearances = &tibiaAppearances.outfit(); break;
            case OBJECT_TYPE_EFFECT: repeatedAppearances = &tibiaAppearances.effect(); break;
            case OBJECT_TYPE_MISSILE: repeatedAppearances = &tibiaAppearances.missile(); break;
            default:
                return;
        }

        appearances[type] = std::vector<TibiaAppearance>(repeatedAppearances->begin(), repeatedAppearances->end());
    }

    isLoaded = true;
}

}
