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

#ifndef APPEARANCES_H
#define APPEARANCES_H

#include "definitions.h"
#include "appearances.pb.h"

using TibiaAppearances = tibia::protobuf::appearances::Appearances;
using TibiaAppearance = tibia::protobuf::appearances::Appearance;

namespace nekiro_proto
{

enum ObjectType {
	OBJECT_TYPE_ITEM = 0,
	OBJECT_TYPE_LOOKTYPE = 1,
	OBJECT_TYPE_EFFECT = 2,
	OBJECT_TYPE_MISSILE = 3,
};

class EXPORT Appearances {
	public:
		Appearances() {};
		Appearances(const std::string& path) {
			parseAppearancesFromFile(path);
		}
		Appearances(std::stringstream& input) {
			parseAppearancesFromMemory(input);
		}

		void parseAppearancesFromFile(const std::string& path);
		void parseAppearancesFromMemory(std::stringstream& input);

		const std::vector<TibiaAppearance>& getAppearances(ObjectType type) {
			if (!isLoaded) {
				throw std::exception("Load appearances first");
			}

			return appearances[type];
		}

	private:
		std::array<std::vector<TibiaAppearance>, OBJECT_TYPE_MISSILE + 1> appearances;

		bool isLoaded = false;
};

}

#endif
