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

#include <gtest/gtest.h>
#include <include/bluzelle.hpp>
#include <pbft/pbft_operation.hpp>
#include <proto/bluzelle.pb.h>

using namespace ::testing;

namespace {

    const bzn::uuid_t TEST_NODE_UUID{"uuid4"};

    const bzn::peers_list_t TEST_PEER_LIST{{"127.0.0.1", 8081, "name1", "uuid1"},
                                           {"127.0.0.1", 8082, "name2", "uuid2"},
                                           {"127.0.0.1", 8083, "name3", "uuid3"},
                                           {"127.0.0.1", 8084, "name4", TEST_NODE_UUID}};

    class pbft_operation_test : public Test {
    public:
        pbft_request request;
        uint64_t view = 6;
        uint64_t sequence = 19;

        bzn::pbft_operation op;

        pbft_operation_test()
                : op(view, sequence, request, TEST_PEER_LIST)
        {
        }
    };

   TEST_F(pbft_operation_test, initially_unprepared){
       EXPECT_FALSE(this->op.is_prepared());

   }
}
