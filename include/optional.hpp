// Copyright (C) 2018 Bluzelle
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License, version 3,
// as published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#pragma once

#ifndef __APPLE__
#include <optional>
#else
#include <experimental/optional>
#endif

namespace bzn
{
    template<class T>
#ifndef __APPLE__
    using optional = std::optional<T>;
#else
    using optional = std::experimental::optional<T>;
#endif
}
