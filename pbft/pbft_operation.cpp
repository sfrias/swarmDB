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

#include "pbft_operation.hpp"

using namespace bzn;

pbft_operation::pbft_operation(uint64_t view, uint64_t sequence, bzn::message request)
        : view(std::move(view)), sequence(std::move(sequence)), request(std::move(request)) {
}

void pbft_operation::record_preprepare() {
    this->preprepare_seen = true;
}

bool pbft_operation::has_preprepare() {
    return false;
}

operation_key_t pbft_operation::get_operation_key() {
    return std::tuple<uint64_t, uint64_t, bzn::message>(this->view, this->sequence, this->request);
}
